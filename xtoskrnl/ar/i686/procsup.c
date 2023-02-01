/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/procsup.c
 * DESCRIPTION:     I686 processor functionality support
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
    KDESCRIPTOR GdtDescriptor, IdtDescriptor;
    PKPROCESSOR_BLOCK ProcessorBlock;
    PKGDTENTRY Gdt;
    PKIDTENTRY Idt;
    PKTSS Tss;

    /* Use initial structures */
    Gdt = ArInitialGdt;
    Idt = ArInitialIdt;
    Tss = &ArInitialTss;

    /* Load processor block */
    ProcessorBlock = CONTAIN_RECORD(&ArInitialProcessorBlock.Prcb, KPROCESSOR_BLOCK, Prcb);

    /* Initialize processor block */
    ArpInitializeProcessorBlock(ProcessorBlock, Gdt, Idt, Tss, (PVOID)KeInitializationBlock->KernelFaultStack);

    /* Initialize GDT and TSS */
    ArpInitializeGdt(ProcessorBlock);
    ArpInitializeTss(ProcessorBlock);

    /* Set GDT and IDT descriptors */
    GdtDescriptor.Base = Gdt;
    GdtDescriptor.Limit = (GDT_ENTRIES * sizeof(PKGDTENTRY)) - 1;
    IdtDescriptor.Base = Idt;
    IdtDescriptor.Limit = (IDT_ENTRIES * sizeof(PKIDTENTRY)) - 1;

    /* Load GDT, IDT and TSS */
    ArLoadGlobalDescriptorTable(&GdtDescriptor.Limit);
    ArLoadInterruptDescriptorTable(&IdtDescriptor.Limit);
    ArLoadTaskRegister((UINT)KGDT_SYS_TSS);

    /* Load FS segment */
    ArLoadSegment(SEGMENT_FS, KGDT_R0_PCR);

    /* Enter passive IRQ level */
    ProcessorBlock->Irql = PASSIVE_LEVEL;
}

/**
 * Initializes the kernel's Global Descriptor Table (GDT).
 *
 * @param ProcessorBlock
 *        Supplies a pointer to the processor block to use.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
ArpInitializeGdt(IN PKPROCESSOR_BLOCK ProcessorBlock)
{
    /* Initialize GDT entries */
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_NULL, 0x0, 0x0, KGDT_TYPE_NONE, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_CODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_CODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_USER, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_TEB, 0x0, 0xFFF, KGDT_TYPE_DATA | KGDT_DESCRIPTOR_ACCESSED, KGDT_DPL_USER, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_LDT, 0x0, 0x0, KGDT_TYPE_NONE, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_VDM_TILE, 0x0400, 0xFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_PCR, (ULONG_PTR)ProcessorBlock, sizeof(KPROCESSOR_BLOCK), KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_SYS_TSS, (ULONG_PTR)ProcessorBlock->TssBase, sizeof(KTSS) - 1, I686_TSS, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_DF_TSS, 0x20000, 0xFFFF, I686_TSS, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_NMI_TSS, 0x20000, 0xFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_VDBS, 0xB8000, 0x3FFF, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 0);
}

/**
 * Initializes processor block.
 *
 * @param ProcessorBlock
 *        Supplies a pointer to the processor block to initialize.
 *
 * @param Gdt
 *        Supplies a pointer to the GDT for this processor block.
 *
 * @param Idt
 *        Supplies a pointer to the IDT for this processor block.
 *
 * @param Tss
 *        Supplies a pointer to the TSS for this processor block.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
ArpInitializeProcessorBlock(OUT PKPROCESSOR_BLOCK ProcessorBlock,
                            IN PKGDTENTRY Gdt,
                            IN PKIDTENTRY Idt,
                            IN PKTSS Tss,
                            IN PVOID DpcStack)
{
    /* Fill PCR with zeroes */
    RtlZeroMemory(ProcessorBlock, sizeof(KPROCESSOR_BLOCK));

    /* Set processor block and processor control block */
    ProcessorBlock->Self = ProcessorBlock;
    ProcessorBlock->CurrentPrcb = &ProcessorBlock->Prcb;

    /* Set GDT, IDT and TSS descriptors */
    ProcessorBlock->GdtBase = Gdt;
    ProcessorBlock->IdtBase = Idt;
    ProcessorBlock->TssBase = Tss;

    /* Setup DPC stack */
    ProcessorBlock->Prcb.DpcStack = DpcStack;

    /* Setup processor control block */
    ProcessorBlock->Prcb.Number = 0;
    ProcessorBlock->Prcb.SetMember = 1;
    ProcessorBlock->Prcb.MultiThreadProcessorSet = 1;

    /* Clear DR6 and DR7 registers */
    ProcessorBlock->Prcb.ProcessorState.SpecialRegisters.KernelDr6 = 0;
    ProcessorBlock->Prcb.ProcessorState.SpecialRegisters.KernelDr7 = 0;
}

/**
 * Initializes the kernel's Task State Segment (TSS).
 *
 * @param ProcessorBlock
 *        Supplies a pointer to the processor block to use.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
ArpInitializeTss(IN PKPROCESSOR_BLOCK ProcessorBlock)
{
    PKGDTENTRY TssEntry;

    /* Get TSS entry from GDT */
    TssEntry = (PKGDTENTRY)(&(ProcessorBlock->GdtBase[KGDT_SYS_TSS / sizeof(KGDTENTRY)]));

    /* Initialize TSS entry */
    TssEntry->Bits.Dpl = 0;
    TssEntry->Bits.Present = 1;
    TssEntry->Bits.Type = I686_TSS;
    TssEntry->LimitLow = sizeof(KTSS) - 1;
    TssEntry->Bits.LimitHigh = 0;

    /* Clear I/O map */
    RtlFillMemory(ProcessorBlock->TssBase->IoMaps[0].IoMap, IOPM_FULL_SIZE, 0xFF);

    /* Fill Interrupt Direction Maps with zeroes */
    RtlZeroMemory(ProcessorBlock->TssBase->IoMaps[0].DirectionMap, IOPM_DIRECTION_MAP_SIZE);

    /* Enable DPMI support */
    ProcessorBlock->TssBase->IoMaps[0].DirectionMap[0] = 4;
    ProcessorBlock->TssBase->IoMaps[0].DirectionMap[3] = 0x18;
    ProcessorBlock->TssBase->IoMaps[0].DirectionMap[4] = 0x18;

    /* Fill default Interrupt Direction Map with zeroes */
    RtlZeroMemory(ProcessorBlock->TssBase->IntDirectionMap, IOPM_DIRECTION_MAP_SIZE);

    /* Enable DPMI support */
    ProcessorBlock->TssBase->IntDirectionMap[0] = 4;
    ProcessorBlock->TssBase->IntDirectionMap[3] = 0x18;
    ProcessorBlock->TssBase->IntDirectionMap[4] = 0x18;

    /* Set I/O map base and disable traps */
    ProcessorBlock->TssBase->IoMapBase = sizeof(KTSS);
    ProcessorBlock->TssBase->Flags = 0;

    /* Set LDT and SS */
    ProcessorBlock->TssBase->LDT = KGDT_R0_LDT;
    ProcessorBlock->TssBase->Ss0 = KGDT_R0_DATA;
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
ArpSetGdtEntry(IN PKGDTENTRY Gdt,
               IN USHORT Selector,
               IN ULONG_PTR Base,
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
    GdtEntry = (PKGDTENTRY)((ULONG_PTR)Gdt + (Selector & ~RPL_MASK));

    /* Set GDT descriptor base */
    GdtEntry->BaseLow = (Base & 0xFFFF);
    GdtEntry->Bytes.BaseMiddle = ((Base >> 16) & 0xFF);
    GdtEntry->Bytes.BaseHigh = ((Base >> 24) & 0xFF);

    /* Set descriptor limit */
    GdtEntry->LimitLow = (Limit & 0xFFFF);
    GdtEntry->Bits.LimitHigh = ((Limit >> 16) & 0xF);

    /* Initialize GDT entry */
    GdtEntry->Bits.DefaultBig = !!(SegmentMode & 2);
    GdtEntry->Bits.Dpl = (Dpl & 0x3);
    GdtEntry->Bits.Granularity = Granularity;
    GdtEntry->Bits.Reserved0 = 0;
    GdtEntry->Bits.Present = (Type != 0);
    GdtEntry->Bits.System = 0;
    GdtEntry->Bits.Type = (Type & 0x1F);
}
