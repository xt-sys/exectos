/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/init.c
 * DESCRIPTION:     Architecture specific Memory Manager initialization routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Gets the address of the PDE (Page Directory Entry), that maps given address.
 *
 * @param Address
 *        Specifies the address to find the PDE for.
 *
 * @return This routine returns the address of the PDE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MmpGetPdeAddress(PVOID Address)
{
    ULONGLONG Offset;

    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << 48) - 1)) >> MM_PDI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPTE)(MM_PDE_BASE + Offset);
}

/**
 * Gets the address of the PPE (Page Directory Pointer Table Entry), that maps given address.
 *
 * @param Address
 *        Specifies the address to find the PPE for.
 *
 * @return This routine returns the address of the PPE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MmpGetPpeAddress(PVOID Address)
{
    ULONGLONG Offset;

    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << 48) - 1)) >> MM_PPI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPTE)(MM_PPE_BASE + Offset);
}

/**
 * Gets the address of the PTE (Page Table Entry), that maps given address.
 *
 * @param Address
 *        Specifies the address to find the PTE for.
 *
 * @return This routine returns the address of the PTE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MmpGetPteAddress(PVOID Address)
{
    ULONGLONG Offset;

    Offset = ((((ULONGLONG)Address & (((ULONGLONG)1 << 48) - 1)) >> MM_PTI_SHIFT) << MM_PTE_SHIFT);
    return (PMMPTE)(MM_PTE_BASE + Offset);
}

/**
 * Gets the address of the PXE (Extended Page Entry), that maps given address.
 *
 * @param Address
 *        Specifies the address to find the PXE for.
 *
 * @return This routine returns the address of the PXE.
 *
 * @since XT 1.0
 */
XTAPI
PMMPTE
MmpGetPxeAddress(PVOID Address)
{
    ULONGLONG Offset;

    Offset = (((ULONGLONG)Address >> MM_PXI_SHIFT) & (MM_PXE_PER_PAGE - 1));
    return (PMMPTE)(MM_PXE_BASE + Offset);
}

/**
 * Performs architecture specific initialization of the XTOS Memory Manager.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MmpInitializeArchitecture()
{
    UNIMPLEMENTED;
}
