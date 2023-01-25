/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/xtos/amd64/cpu.c
 * DESCRIPTION:     XTOS AMD64 processor context
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Fills in an AMD64 GDT entry.
 *
 * @param Gdt
 *        Supplies a pointer to the GDT.
 *
 * @param Selector
 *        Specifies a segment selector of the GDT entry.
 *
 * @param Base
 *        Specifies a base address value of the descriptor.
 *
 * @param Limit
 *        Specifies a descriptor limit.
 *
 * @param Type
 *        Specifies a type of the descriptor.
 *
 * @param Dpl
 *        Specifies the descriptor privilege level.
 *
 * @param SegmentMode
 *        Specifies a segment mode of the descriptor.
 *
 * @return This routine does not return any value
 *
 * @since XT 1.0
 */
XTCDECL
VOID
XtpInitializeGdtEntry(IN PKGDTENTRY Gdt,
                      IN USHORT Selector,
                      IN ULONGLONG Base,
                      IN ULONG Limit,
                      IN UCHAR Type,
                      IN UCHAR Dpl,
                      IN UCHAR SegmentMode)
{
    PKGDTENTRY GdtEntry;
    UCHAR Granularity;

    /* Set the granularity flag depending on descriptor limit */
    if(Limit < 0x100000)
    {
        /* Limit is in 1B blocks */
        Granularity = 0;
    }
    else
    {
        /* Limit is in 4KB blocks */
        Granularity = 1;
        Limit >>= 12;
    }

    /* Get GDT entry */
    GdtEntry = (PKGDTENTRY)((ULONG64)Gdt + (Selector & ~RPL_MASK));

    /* Set GDT descriptor base */
    GdtEntry->BaseLow = Base & 0xFFFF;
    GdtEntry->Bits.BaseMiddle = (Base >> 16) & 0xFF;
    GdtEntry->Bits.BaseHigh = (Base >> 24) & 0xFF;
    GdtEntry->BaseUpper = Base >> 32;

    /* Set descriptor limit */
    GdtEntry->LimitLow = Limit & 0xFFFF;
    GdtEntry->Bits.LimitHigh = (Limit >> 16) & 0xF;

    /* Initialize GDT entry */
    GdtEntry->Bits.DefaultBig = (SegmentMode & 2);
    GdtEntry->Bits.Dpl = (Dpl & 0x3);
    GdtEntry->Bits.Granularity = Granularity;
    GdtEntry->Bits.LongMode = (SegmentMode & 1);
    GdtEntry->Bits.Present = (Type != 0);
    GdtEntry->Bits.System = 0;
    GdtEntry->Bits.Type = (Type & 0x1F);
    GdtEntry->MustBeZero = 0;
}

/**
 * Initializes GDT and TSS descriptors.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param VirtualAddress
 *        Supplies a pointer to the next valid, free and available virtual address.
 *
 * @param Gdt
 *        Supplies a pointer to memory area containing GDT to fill in.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtpInitializeDescriptors(IN PLIST_ENTRY MemoryMappings,
                         IN PVOID *VirtualAddress,
                         OUT PKGDTENTRY *Gdt)
{
    EFI_PHYSICAL_ADDRESS GdtAddress, TssAddress;
    PKTSS PhysicalTss, Tss;
    PKGDTENTRY GdtEntry;
    EFI_STATUS Status;

    /* Print debug message */
    XtLdrProtocol->DbgPrint(L"Initializing Task State Segment (TSS)\n");

    /* Allocate memory for TSS */
    Status = XtLdrProtocol->AllocatePages(EFI_SIZE_TO_PAGES(sizeof(KTSS)), &TssAddress);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        XtLdrProtocol->DbgPrint(L"Failed to allocate pages for TSS (Status Code: %lx)\n", Status);
        return Status;
    }

    /* Set TSS and fill it with zeroes */
    PhysicalTss = (PKTSS)(UINT_PTR)TssAddress;
    RtlZeroMemory(PhysicalTss, EFI_SIZE_TO_PAGES(sizeof(KTSS)) * EFI_PAGE_SIZE);

    /* Set RSP0 to point at the end of kernel startup stack */
    PhysicalTss->Rsp0 = (UINT_PTR)*VirtualAddress;

    /* Map TSS and set its virtual address */
    XtLdrProtocol->AddVirtualMemoryMapping(MemoryMappings, *VirtualAddress, PhysicalTss,
                                           EFI_SIZE_TO_PAGES(sizeof(KTSS)), LoaderMemoryData);
    Tss = (PKTSS)*VirtualAddress;

    /* Set next valid virtual address */
    *VirtualAddress += EFI_SIZE_TO_PAGES(sizeof(KTSS)) * EFI_PAGE_SIZE;

    /* Print debug message */
    XtLdrProtocol->DbgPrint(L"Initializing General Descriptor Table (GDT)\n");

    /* Allocate memory for GDT */
    Status = XtLdrProtocol->AllocatePages(EFI_SIZE_TO_PAGES(128 * sizeof(KGDTENTRY)), &GdtAddress);
    if (Status != STATUS_EFI_SUCCESS) {
        /* Memory allocation failure */
        XtLdrProtocol->DbgPrint(L"Failed to allocate pages for GDT (Status Code: %lx)\n", Status);
        return Status;
    }

    /* Set GDT entry and fill it with zeroes */
    GdtEntry = (PKGDTENTRY)(UINT_PTR)GdtAddress;
    RtlZeroMemory(GdtEntry, EFI_SIZE_TO_PAGES(128 * sizeof(KGDTENTRY)) * EFI_PAGE_SIZE);

    /* Initialize GDT entries */
    XtpInitializeGdtEntry(GdtEntry, KGDT_NULL, 0x0, 0x0, 0, KGDT_DPL_SYSTEM, 0);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R0_CODE, 0x0, 0x0, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 0);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R0_DATA, 0x0, 0x0, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 0);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R3_CODE, 0x0, 0x0, KGDT_TYPE_CODE, KGDT_DPL_USER, 0);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R3_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R3_CMCODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_USER, 2);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R3_CMTEB, 0x0, 0x0FFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R0_LDT, 0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 0);
    XtpInitializeGdtEntry(GdtEntry, KGDT_SYS_TSS, (ULONG_PTR)Tss, sizeof(KTSS), AMD64_TSS, KGDT_DPL_SYSTEM, 0);

    /* Map GDT and set its virtual address */
    Status = XtLdrProtocol->AddVirtualMemoryMapping(MemoryMappings, *VirtualAddress, GdtEntry,
                                                    EFI_SIZE_TO_PAGES(128 * sizeof(KGDTENTRY)), LoaderMemoryData);
    *Gdt = (PKGDTENTRY)*VirtualAddress;

    /* Set next valid virtual address */
    *VirtualAddress += (EFI_SIZE_TO_PAGES(128 * sizeof(KGDTENTRY)) * EFI_PAGE_SIZE);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Loads previously prepared CPU context and performs architecture-specific tasks needed to boot XTOS.
 *
 * @param Gdt
 *        Supplies a pointer to memory area containing GDT to load.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
XtpLoadProcessorContext(IN PKGDTENTRY Gdt)
{
    KDESCRIPTOR GdtDescriptor;

    GdtDescriptor.Base = Gdt;
    GdtDescriptor.Limit = 128 * sizeof(KGDTENTRY) - 1;

    /* Load GDT and TSS */
    HlLoadGlobalDescriptorTable(&GdtDescriptor.Limit);
    HlLoadTaskRegister((UINT32)KGDT_SYS_TSS);

    /* Re-enable IDE interrupts */
    HlIoPortOutByte(0x376, 0);
    HlIoPortOutByte(0x3F6, 0);
}

/**
 * Sets architecture specific processor context.
 *
 * @param MemoryMappings
 *        Supplies a pointer to linked list containing all memory mappings.
 *
 * @param VirtualAddress
 *        Supplies a pointer to the next valid, free and available virtual address.
 *
 * @param Gdt
 *        Supplies a pointer to memory area containing GDT to fill in.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtpSetProcessorContext(IN PLIST_ENTRY MemoryMappings,
                       IN PVOID *VirtualAddress,
                       OUT PKGDTENTRY *Gdt)
{
    EFI_STATUS Status;

    /* Print debug message */
    XtLdrProtocol->DbgPrint(L"Setting processor context\n");

    /* Disable interrupts */
    HlClearInterruptFlag();

    /* Initialize GDT */
    Status = XtpInitializeDescriptors(MemoryMappings, VirtualAddress, Gdt);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* GDT initialization failure */
        XtLdrProtocol->DbgPrint(L"Failed to initialize GDT (Status Code: %lx)\n", Status);
        return Status;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
