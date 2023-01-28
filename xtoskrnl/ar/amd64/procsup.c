/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/procsup.c
 * DESCRIPTION:     AMD64 processor functionality support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include "xtos.h"


/**
 * Initializes AMD64 processor specific structures.
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
    ArpInitializeGdtEntry(Gdt, KGDT_R0_CODE, 0x0, 0x0, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 0);
    ArpInitializeGdtEntry(Gdt, KGDT_R0_DATA, 0x0, 0x0, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 0);
    ArpInitializeGdtEntry(Gdt, KGDT_R3_CODE, 0x0, 0x0, KGDT_TYPE_CODE, KGDT_DPL_USER, 0);
    ArpInitializeGdtEntry(Gdt, KGDT_R3_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    ArpInitializeGdtEntry(Gdt, KGDT_R3_CMCODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_USER, 2);
    ArpInitializeGdtEntry(Gdt, KGDT_R3_CMTEB, 0x0, 0x0FFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    ArpInitializeGdtEntry(Gdt, KGDT_R0_LDT, 0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 0);
    ArpInitializeGdtEntry(Gdt, KGDT_SYS_TSS, 0, sizeof(KTSS), AMD64_TSS, KGDT_DPL_SYSTEM, 0);
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
    TssEntry->BaseLow = (ULONG64)Tss & 0xFFFF;
    TssEntry->BaseUpper = (ULONG64)Tss >> 32;
    TssEntry->LimitLow = (sizeof(KTSS) - 1) & 0xFFFF;
    TssEntry->Bits.BaseMiddle = ((ULONG64)Tss >> 16) & 0xFF;
    TssEntry->Bits.BaseHigh = ((ULONG64)Tss >> 24) & 0xFF;
    TssEntry->Bits.LimitHigh = (sizeof(KTSS) - 1) >> 16;
    TssEntry->Bits.DefaultBig = 0;
    TssEntry->Bits.Dpl = 0;
    TssEntry->Bits.Granularity = 0;
    TssEntry->Bits.LongMode = 0;
    TssEntry->Bits.Present = 1;
    TssEntry->Bits.System = 0;
    TssEntry->Bits.Type = AMD64_TSS;
    TssEntry->MustBeZero = 0;

    /* Fill TSS with zeroes */
    RtlZeroMemory(Tss, sizeof(KTSS));

    /* Setup I/O map and stacks for ring0 & traps */
    Tss->IoMapBase = sizeof(KTSS);
    Tss->Rsp0 = KeInitializationBlock->KernelBootStack;
    Tss->Ist[1] = KeInitializationBlock->KernelFaultStack;
    Tss->Ist[2] = KeInitializationBlock->KernelFaultStack;
    Tss->Ist[3] = KeInitializationBlock->KernelFaultStack;
}
