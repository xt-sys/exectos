/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/xtos/i686/cpu.c
 * DESCRIPTION:     XTOS i686 processor context
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

    /* Set descriptor limit */
    GdtEntry->LimitLow = Limit & 0xFFFF;
    GdtEntry->Bits.LimitHigh = (Limit >> 16) & 0xF;

    /* Initialize GDT entry */
    GdtEntry->Bits.DefaultBig = (SegmentMode & 2);
    GdtEntry->Bits.Dpl = (Dpl & 0x3);
    GdtEntry->Bits.Granularity = Granularity;
    GdtEntry->Bits.Reserved0 = 0;
    GdtEntry->Bits.Present = (Type != 0);
    GdtEntry->Bits.System = 0;
    GdtEntry->Bits.Type = (Type & 0x1F);
}

#define PCR_PAGES 7
/**
 * Initializes GDT, PCR and TSS descriptors.
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
 * @param Idt
 *        Supplies a pointer to memory area containing IDT to fill in.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtpInitializeDescriptors(IN PLIST_ENTRY MemoryMappings,
                         IN PVOID *VirtualAddress,
                         OUT PKGDTENTRY *Gdt,
                         OUT PKIDTENTRY *Idt)
{
    EFI_PHYSICAL_ADDRESS GdtAddress, IdtAddress, PcrAddress, TssAddress;
    KDESCRIPTOR OriginalIdt;
    PKTSS PhysicalTss, Tss;
    PVOID PhysicalPcr, Pcr;
    PKGDTENTRY GdtEntry;
    PKIDTENTRY IdtEntry;
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

    /* Map TSS and set its virtual address */
    XtLdrProtocol->AddVirtualMemoryMapping(MemoryMappings, *VirtualAddress, PhysicalTss,
                                           EFI_SIZE_TO_PAGES(sizeof(KTSS)), LoaderMemoryData);
    Tss = (PKTSS)*VirtualAddress;

    /* Set next valid virtual address */
    *VirtualAddress += EFI_SIZE_TO_PAGES(sizeof(KTSS)) * EFI_PAGE_SIZE;

    /* Print debug message */
    XtLdrProtocol->DbgPrint(L"Initializing Processor Control Region (PCR)\n");

    /* Allocate memory for PCR */
    Status = XtLdrProtocol->AllocatePages(PCR_PAGES, &PcrAddress);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        XtLdrProtocol->DbgPrint(L"Failed to allocate pages for PCR (Status Code: %lx)\n", Status);
        return Status;
    }

    /* Set PCR and fill it with zeroes */
    PhysicalPcr = (PVOID)(UINT_PTR)PcrAddress;
    RtlZeroMemory(PhysicalPcr, PCR_PAGES * EFI_PAGE_SIZE);

    /* Map TSS and set its virtual address */
    XtLdrProtocol->AddVirtualMemoryMapping(MemoryMappings, *VirtualAddress, PhysicalPcr,
                                           PCR_PAGES, LoaderStartupPcrPage);
    Pcr = *VirtualAddress;

    /* Set next valid virtual address */
    *VirtualAddress += PCR_PAGES * EFI_PAGE_SIZE;

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
    XtpInitializeGdtEntry(GdtEntry, KGDT_R0_CODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 2);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R0_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 2);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R3_CODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_USER, 2);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R3_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R3_TEB, 0x0, 0xFFF, KGDT_TYPE_DATA | KGDT_DESCRIPTOR_ACCESSED, KGDT_DPL_USER, 2);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R0_LDT, 0x0, 0x0, 0, KGDT_DPL_SYSTEM, 0);
    XtpInitializeGdtEntry(GdtEntry, KGDT_VDM_TILE, 0x0400, 0xFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 0);
    XtpInitializeGdtEntry(GdtEntry, KGDT_R0_PCR, (ULONG_PTR)Pcr, 0x1, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 2);
    XtpInitializeGdtEntry(GdtEntry, KGDT_SYS_TSS, (ULONG_PTR)Tss, sizeof(KTSS) - 1, I686_TSS, KGDT_DPL_SYSTEM, 0);
    XtpInitializeGdtEntry(GdtEntry, KGDT_DF_TSS, 0x20000, 0xFFFF, I686_TSS, KGDT_DPL_SYSTEM, 0);
    XtpInitializeGdtEntry(GdtEntry, KGDT_NMI_TSS, 0x20000, 0xFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 0);

    /* Map GDT and set its virtual address */
    Status = XtLdrProtocol->AddVirtualMemoryMapping(MemoryMappings, *VirtualAddress, GdtEntry,
                                                    EFI_SIZE_TO_PAGES(128 * sizeof(KGDTENTRY)), LoaderMemoryData);
    *Gdt = (PKGDTENTRY)*VirtualAddress;

    /* Set next valid virtual address */
    *VirtualAddress += (EFI_SIZE_TO_PAGES(128 * sizeof(KGDTENTRY)) * EFI_PAGE_SIZE);

    /* Print debug message */
    XtLdrProtocol->DbgPrint(L"Initializing Interrupt Descriptor Table (IDT)\n");

    /* Allocate memory for IDT */
    Status = XtLdrProtocol->AllocatePages(EFI_SIZE_TO_PAGES(256 * sizeof(KIDTENTRY)), &IdtAddress);
    if (Status != STATUS_EFI_SUCCESS) {
        /* Memory allocation failure */
        XtLdrProtocol->DbgPrint(L"Failed to allocate pages for IDT (Status Code: %lx)\n", Status);
        return Status;
    }

    /* Set IDT entry and fill it with zeroes */
    IdtEntry = (PKIDTENTRY)(UINT_PTR)IdtAddress;
    RtlZeroMemory(IdtEntry, EFI_SIZE_TO_PAGES(256 * sizeof(KIDTENTRY)) * EFI_PAGE_SIZE);

    /* Stores IDT register into new IDT entry */
    ArStoreInterruptDescriptorTable(&OriginalIdt.Limit);
    RtlCopyMemory(IdtEntry, OriginalIdt.Base, OriginalIdt.Limit + 1);

    /* Map IDT and set its virtual address */
    Status = XtLdrProtocol->AddVirtualMemoryMapping(MemoryMappings, *VirtualAddress, IdtEntry,
                                                    EFI_SIZE_TO_PAGES(256 * sizeof(KIDTENTRY)), LoaderMemoryData);
    *Idt = (PKIDTENTRY)*VirtualAddress;

    /* Set next valid virtual address */
    *VirtualAddress += (EFI_SIZE_TO_PAGES(256 * sizeof(KIDTENTRY)) * EFI_PAGE_SIZE);
    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Loads previously prepared CPU context and performs architecture-specific tasks needed to boot XTOS.
 *
 * @param Gdt
 *        Supplies a pointer to memory area containing GDT to load.
 *
 * @param Idt
 *        Supplies a pointer to memory area containing IDT to load.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
XtpLoadProcessorContext(IN PKGDTENTRY Gdt,
                        IN PKIDTENTRY Idt)
{
    KDESCRIPTOR GdtDescriptor, IdtDescriptor;

    GdtDescriptor.Base = (PVOID)(ULONG_PTR)Gdt;
    GdtDescriptor.Limit = 128 * sizeof(KGDTENTRY) - 1;

    IdtDescriptor.Base = Idt;
    IdtDescriptor.Limit = 256 * sizeof(KIDTENTRY) - 1;

    /* Load GDT and TSS */
    ArLoadGlobalDescriptorTable(&GdtDescriptor.Limit);
    ArLoadInterruptDescriptorTable(&IdtDescriptor.Limit);
    ArLoadTaskRegister((UINT32)KGDT_SYS_TSS);

    /* Load PCR in FS segment */
    ArLoadSegment(SEGMENT_FS, KGDT_R0_PCR);

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
 * @param Idt
 *        Supplies a pointer to memory area containing IDT to fill in.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtpSetProcessorContext(IN PLIST_ENTRY MemoryMappings,
                       IN PVOID *VirtualAddress,
                       OUT PKGDTENTRY *Gdt,
                       OUT PKIDTENTRY *Idt)
{
    EFI_STATUS Status;

    /* Print debug message */
    XtLdrProtocol->DbgPrint(L"Setting processor context\n");

    /* Disable interrupts */
    ArClearInterruptFlag();

    /* Initialize GDT */
    Status = XtpInitializeDescriptors(MemoryMappings, VirtualAddress, Gdt, Idt);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* GDT initialization failure */
        XtLdrProtocol->DbgPrint(L"Failed to initialize GDT (Status Code: %lx)\n", Status);
        return Status;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}
