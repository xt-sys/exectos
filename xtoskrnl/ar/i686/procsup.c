/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/procsup.c
 * DESCRIPTION:     AMD64 processor functionality support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include "xtos.h"


/**
 * Initializes i686 processor specific structures.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
ArInitializeProcessor(VOID)
{
    PKGDTENTRY Gdt;
    PKTSS Tss;
    KDESCRIPTOR GdtDescriptor;

    /* Use initial structures */
    Gdt = ArInitialGdt;
    Tss = &ArInitialTss;

    /* Initialize GDT and TSS */
    ArpInitializeGdt(Gdt);
    ArpInitializeTss(Tss, Gdt);

    /* Set GDT descriptor */
    GdtDescriptor.Base = Gdt;
    GdtDescriptor.Limit = (GDT_ENTRIES * sizeof(PKGDTENTRY)) - 1;

    /* Load GDT and TSS */
    ArLoadGlobalDescriptorTable(&GdtDescriptor.Limit);
    ArLoadTaskRegister((UINT32)KGDT_SYS_TSS);
}

/**
 * Initializes the kernel's Global Descriptor Table (GDT).
 *
 * @param Gdt
 *        Supplies a pointer to the GDT to use.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
ArpInitializeGdt(IN PKGDTENTRY Gdt)
{
    /* Initialize GDT entries */
    ArpInitializeGdtEntry(Gdt, KGDT_NULL, 0x0, 0x0, 0, KGDT_DPL_SYSTEM, 0);
    ArpInitializeGdtEntry(Gdt, KGDT_R0_CODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 2);
    ArpInitializeGdtEntry(Gdt, KGDT_R0_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 2);
    ArpInitializeGdtEntry(Gdt, KGDT_R3_CODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_USER, 2);
    ArpInitializeGdtEntry(Gdt, KGDT_R3_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    ArpInitializeGdtEntry(Gdt, KGDT_R3_TEB, 0x0, 0xFFF, KGDT_TYPE_DATA | KGDT_DESCRIPTOR_ACCESSED, KGDT_DPL_USER, 2);
    ArpInitializeGdtEntry(Gdt, KGDT_R0_LDT, 0x0, 0x0, 0, KGDT_DPL_SYSTEM, 0);
    ArpInitializeGdtEntry(Gdt, KGDT_VDM_TILE, 0x0400, 0xFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 0);
    ArpInitializeGdtEntry(Gdt, KGDT_R0_PCR, 0x0, 0x1, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 2);
    ArpInitializeGdtEntry(Gdt, KGDT_SYS_TSS, 0x0, sizeof(KTSS) - 1, I686_TSS, KGDT_DPL_SYSTEM, 0);
    ArpInitializeGdtEntry(Gdt, KGDT_DF_TSS, 0x20000, 0xFFFF, I686_TSS, KGDT_DPL_SYSTEM, 0);
    ArpInitializeGdtEntry(Gdt, KGDT_NMI_TSS, 0x20000, 0xFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 0);
}

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
XTAPI
VOID
ArpInitializeGdtEntry(IN PKGDTENTRY Gdt,
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

/**
 * Initializes the kernel's Task State Segment (TSS).
 *
 * @param Tss
 *        Supplies a pointer to the TSS to use.
 *
 * @param Gdt
 *        Supplies a pointer to the GDT to use.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
ArpInitializeTss(IN PKTSS Tss,
                 IN PKGDTENTRY Gdt)
{
    PKGDTENTRY TssEntry;

    /* Get TSS entry from GDT */
    TssEntry = (PKGDTENTRY)(&(Gdt[KGDT_SYS_TSS / sizeof(KGDTENTRY)]));

    /* Initialize TSS entry */
    TssEntry->Bits.Dpl = 0;
    TssEntry->Bits.Present = 1;
    TssEntry->Bits.Type = I686_TSS;
    TssEntry->LimitLow = sizeof(KTSS) - 1;
    TssEntry->Bits.LimitHigh = 0;

    /* Clear I/O map */
    RtlFillMemory(Tss->IoMaps[0].IoMap, IOPM_FULL_SIZE, 0xFF);

    /* Fill Interrupt Direction Maps with zeroes */
    RtlZeroMemory(Tss->IoMaps[0].DirectionMap, IOPM_DIRECTION_MAP_SIZE);

    /* Enable DPMI support */
    Tss->IoMaps[0].DirectionMap[0] = 4;
    Tss->IoMaps[0].DirectionMap[3] = 0x18;
    Tss->IoMaps[0].DirectionMap[4] = 0x18;

    /* Fill default Interrupt Direction Map with zeroes */
    RtlZeroMemory(Tss->IntDirectionMap, IOPM_DIRECTION_MAP_SIZE);

    /* Enable DPMI support */
    Tss->IntDirectionMap[0] = 4;
    Tss->IntDirectionMap[3] = 0x18;
    Tss->IntDirectionMap[4] = 0x18;

    /* Set I/O map base and disable traps */
    Tss->IoMapBase = 0x68;
    Tss->Flags = 0;

    /* Set LDT and SS */
    Tss->LDT = KGDT_R0_LDT;
    Tss->Ss0 = KGDT_R0_DATA;
}
