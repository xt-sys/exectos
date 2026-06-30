/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/probe.cc
 * DESCRIPTION:     Memory Manager read/write probe support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
* Probes a generic memory buffer for read accessibility.
*
* @param Address
*        Supplies the base address of the buffer to probe.
*
* @param Size
*        Supplies the size of the buffer in bytes.
*
* @param Alignment
*        Supplies the required data alignment (must be 1, 2, 4, 8, or 16).
*
* @return This routine does not return any value.
*
* @since XT 1.0
*/
XTAPI
VOID
MM::Probe::ProbeForRead(IN CONST PVOID Address,
                        IN SIZE_T Size,
                        IN ULONG Alignment)
{
    /* Check if the buffer size is non-zero */
    if(Size)
    {
        /* Check if the base address violates the required alignment */
        if(((ULONG_PTR)Address & (Alignment - 1)) != 0)
        {
            /* Raise datatype misalignment exception */
            RTL::Exception::RaiseStatus(STATUS_DATATYPE_MISALIGNMENT);
        }

        /* Verify that the end address do not overflow and remains within user space */
        if((((ULONG_PTR)Address + Size) < (ULONG_PTR)Address) ||
           (((ULONG_PTR)Address + Size) > MM::Paging::GetUserProbeAddress()))
        {
            /* Raise access violation exception */
            RTL::Exception::RaiseStatus(STATUS_ACCESS_VIOLATION);
        }
    }
}

/**
 * Probes an array of fixed-size items for read accessibility.
 *
 * @param Array
 *        Supplies the base address of the array.
 *
 * @param ItemSize
 *        Supplies the size of a single array element in bytes.
 *
 * @param ItemCount
 *        Supplies the total number of elements in the array.
 *
 * @param Alignment
 *        Supplies the required array alignment (must be 1, 2, 4, 8, or 16).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Probe::ProbeForReadArray(IN CONST PVOID Array,
                             IN ULONG ItemSize,
                             IN ULONG ItemCount,
                             IN ULONG Alignment)
{
    ULONG ArraySize;

    /* Calculate total array size */
    ArraySize = ItemSize * ItemCount;

    /* Check for integer overflow */
    if((ItemSize != 0) && (ArraySize / ItemSize != ItemCount))
    {
        RTL::Exception::RaiseStatus(STATUS_INVALID_PARAMETER);
    }

    /* Perform boundary and alignment verification */
    ProbeForRead(Array, ArraySize, Alignment);
}

/**
 * Probes a structure for read accessibility.
 *
 * @param Address
 *        Supplies the base address of the structure to probe.
 *
 * @param Size
 *        Supplies the total size of the structure in bytes.
 *
 * @param Alignment
 *        Supplies the required structure alignment (must be 1, 2, 4, 8, or 16).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Probe::ProbeForReadStructure(IN CONST PVOID Address,
                                 IN SIZE_T Size,
                                 IN ULONG Alignment)
{
    /* Check if the buffer size is zero or greater than 64k */
    if((Size == 0) || (Size > 0x10000))
    {
        /* Perform boundary and alignment verification */
        ProbeForRead(Address, Size, Alignment);
    }
    else
    {
        /* Check if the base address violates the required alignment */
        if(((ULONG_PTR)Address & (Alignment - 1)) != 0)
        {
            /* Raise datatype misalignment exception */
            RTL::Exception::RaiseStatus(STATUS_DATATYPE_MISALIGNMENT);
        }

        /* Check address space boundary */
        if((ULONG_PTR)Address >= (ULONG_PTR)MM::Paging::GetUserProbeAddress())
        {
            /* Force access violation on the invalid probe boundary */
            *(VOLATILE PULONG CONST)MM::Paging::GetUserProbeAddress() = 0;
        }
    }
}

/**
 * Probes a generic memory buffer for write accessibility.
 *
 * @param Address
 *        Supplies the base address of the buffer to probe.
 *
 * @param Size
 *        Supplies the size of the buffer in bytes.
 *
 * @param Alignment
 *        Supplies the required data alignment (must be 1, 2, 4, 8, or 16).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Probe::ProbeForWrite(IN PVOID Address,
                         IN SIZE_T Size,
                         IN ULONG Alignment)
{
    ULONG_PTR CurrentAddress, LastAddress;

    /* Check if the buffer size is non-zero */
    if(Size)
    {
        /* Perform boundary and alignment verification */
        ProbeForRead(Address, Size, Alignment);

        /* Store the current address */
        CurrentAddress = (ULONG_PTR)Address;

        /* Calculate the end boundary of the requested buffer */
        LastAddress = CurrentAddress + Size - 1;
        LastAddress = (LastAddress & ~(MM_PAGE_SIZE - 1)) + MM_PAGE_SIZE;

        /* Loop through each page in the range */
        while(CurrentAddress < LastAddress)
        {
            /* Touch the page with a read-write cycle */
            *(VOLATILE PCHAR)CurrentAddress = *(volatile CHAR*)CurrentAddress;

            /* Advance to the next page */
            CurrentAddress = (CurrentAddress & ~(MM_PAGE_SIZE - 1)) + MM_PAGE_SIZE;
        }
    }
}

/**
 * Probes an array of fixed-size items for write accessibility.
 *
 * @param Array
 *        Supplies the base address of the array.
 *
 * @param ItemSize
 *        Supplies the size of a single array element in bytes.
 *
 * @param ItemCount
 *        Supplies the total number of elements in the array.
 *
 * @param Alignment
 *        Supplies the required data alignment (must be 1, 2, 4, 8, or 16).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Probe::ProbeForWriteArray(IN OUT PVOID Array,
                              IN ULONG ItemSize,
                              IN ULONG ItemCount,
                              IN ULONG Alignment)
{
    ULONG ArraySize;

    /* Calculate total array size */
    ArraySize = ItemSize * ItemCount;

    /* Check for integer overflow */
    if((ItemSize != 0) && (ArraySize / ItemSize != ItemCount))
    {
        RTL::Exception::RaiseStatus(STATUS_INVALID_PARAMETER);
    }

    /* Perform write accessibility verification */
    ProbeForWrite(Array, ArraySize, Alignment);
}




/**
 * Probes a structure for write accessibility.
 *
 * @param Address
 *        Supplies the base address of the structure to probe.
 *
 * @param Size
 *        Supplies the total size of the structure in bytes.
 *
 * @param Alignment
 *        Supplies the required structure alignment (must be 1, 2, 4, 8, or 16).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Probe::ProbeForWriteStructure(IN PVOID Address,
                                  IN SIZE_T Size,
                                  IN ULONG Alignment)
{
    /* Delegate fast-path boundary and alignment verification to the Read probe (DRY) */
    ProbeForReadStructure(Address, Size, Alignment);

    /* Check if the buffer size is zero or exceeds the page size */
    if((Size == 0) || (Size >= MM_PAGE_SIZE))
    {
        /* Perform write accessibility verification */
        ProbeForWrite(Address, Size, Alignment);
    }
    else
    {
        /* Touch the first byte to verify write access */
        *(VOLATILE PUCHAR)Address = *(VOLATILE PUCHAR)Address;

        /* Check if the buffer size exceeds the alignment */
        if(Size > Alignment)
        {
            /* Touch the last byte to ensure full structure spans valid memory */
            ((VOLATILE PUCHAR)Address)[(Size - 1) & ~(SIZE_T)(Alignment - 1)] =
            ((VOLATILE PUCHAR)Address)[(Size - 1) & ~(SIZE_T)(Alignment - 1)];
        }
    }
}
