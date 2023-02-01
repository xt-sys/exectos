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

    /* Initialize GDT, IDT and TSS */
    ArpInitializeGdt(ProcessorBlock);
    ArpInitializeTss(ProcessorBlock);
    ArpInitializeIdt(ProcessorBlock);

    /* Set GDT and IDT descriptors */
    GdtDescriptor.Base = Gdt;
    GdtDescriptor.Limit = (GDT_ENTRIES * sizeof(KGDTENTRY)) - 1;
    IdtDescriptor.Base = Idt;
    IdtDescriptor.Limit = (IDT_ENTRIES * sizeof(KIDTENTRY)) - 1;

    /* Load GDT, IDT and TSS */
    ArLoadGlobalDescriptorTable(&GdtDescriptor.Limit);
    ArLoadInterruptDescriptorTable(&IdtDescriptor.Limit);
    ArLoadTaskRegister((UINT)KGDT_SYS_TSS);

    /* Set GS base */
    ArWriteModelSpecificRegister(X86_MSR_GSBASE, (ULONGLONG)ProcessorBlock);
    ArWriteModelSpecificRegister(X86_MSR_KERNEL_GSBASE, (ULONGLONG)ProcessorBlock);

    /* Enter passive IRQ level */
    ProcessorBlock->Irql = PASSIVE_LEVEL;
    ArWriteControlRegister(8, PASSIVE_LEVEL);
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
ArpInitializeGdt(IN PKPROCESSOR_BLOCK ProcessorBlock)
{
    /* Initialize GDT entries */
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_NULL, 0x0, 0x0, KGDT_TYPE_NONE, KGDT_DPL_SYSTEM, 1);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_CODE, 0x0, 0x0, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 1);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_DATA, 0x0, 0x0, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 1);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_CODE, 0x0, 0x0, KGDT_TYPE_CODE, KGDT_DPL_USER, 1);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_CMCODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_USER, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_CMTEB, 0x0, 0x0FFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_LDT, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_SYS_TSS, (ULONGLONG)ProcessorBlock->TssBase, sizeof(KTSS), AMD64_TSS, KGDT_DPL_SYSTEM, 0);
}

/**
 * Initializes the kernel's Interrupt Descriptor Table (GDT).
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
ArpInitializeIdt(IN PKPROCESSOR_BLOCK ProcessorBlock)
{
    UINT Vector;

    /* Fill in all vectors */
    for(Vector = 0; Vector < IDT_ENTRIES; Vector++)
    {
        /* Set the IDT to handle unexpected interrupts */
        ArpSetIdtGate(ProcessorBlock->IdtBase, Vector, ArpHandleTrapFF, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    }

    /* Setup IDT handlers for known interrupts and traps */
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x02, ArpHandleTrap02, KGDT_R0_CODE, KIDT_IST_PANIC, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x08, ArpHandleTrap08, KGDT_R0_CODE, KIDT_IST_PANIC, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0D, ArpHandleTrap0D, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0E, ArpHandleTrap0E, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
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
    /* Fill processor block with zeroes */
    RtlZeroMemory(ProcessorBlock, sizeof(KPROCESSOR_BLOCK));

    /* Set processor block and processor control block */
    ProcessorBlock->Self = ProcessorBlock;
    ProcessorBlock->CurrentPrcb = &ProcessorBlock->Prcb;

    /* Set GDT, IDT and TSS descriptors */
    ProcessorBlock->GdtBase = (PVOID)Gdt;
    ProcessorBlock->IdtBase = Idt;
    ProcessorBlock->TssBase = Tss;
    ProcessorBlock->Prcb.RspBase = Tss->Rsp0;

    /* Setup DPC stack */
    ProcessorBlock->Prcb.DpcStack = DpcStack;

    /* Setup processor control block */
    ProcessorBlock->Prcb.Number = 0;
    ProcessorBlock->Prcb.SetMember = 1ULL;
    ProcessorBlock->Prcb.MultiThreadProcessorSet = 1ULL;

    /* Clear DR6 and DR7 registers */
    ProcessorBlock->Prcb.ProcessorState.SpecialRegisters.KernelDr6 = 0;
    ProcessorBlock->Prcb.ProcessorState.SpecialRegisters.KernelDr7 = 0;

    /* Set initial MXCSR register value */
    ProcessorBlock->Prcb.MxCsr = INITIAL_MXCSR;
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
ArpInitializeTss(IN PKPROCESSOR_BLOCK ProcessorBlock)
{
    /* Fill TSS with zeroes */
    RtlZeroMemory(ProcessorBlock->TssBase, sizeof(KTSS));

    /* Setup I/O map and stacks for ring0 & traps */
    ProcessorBlock->TssBase->IoMapBase = sizeof(KTSS);
    ProcessorBlock->TssBase->Rsp0 = KeInitializationBlock->KernelBootStack;
    ProcessorBlock->TssBase->Ist[1] = KeInitializationBlock->KernelFaultStack;
    ProcessorBlock->TssBase->Ist[2] = KeInitializationBlock->KernelFaultStack;
    ProcessorBlock->TssBase->Ist[3] = KeInitializationBlock->KernelFaultStack;
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
    GdtEntry->BaseLow = Base & 0xFFFF;
    GdtEntry->Bytes.BaseMiddle = (Base >> 16) & 0xFF;
    GdtEntry->Bytes.BaseHigh = (Base >> 24) & 0xFF;
    GdtEntry->BaseUpper = Base >> 32;

    /* Set descriptor limit */
    GdtEntry->LimitLow = Limit & 0xFFFF;
    GdtEntry->Bits.LimitHigh = (Limit >> 16) & 0xF;

    /* Initialize GDT entry */
    GdtEntry->Bits.DefaultBig = !!(SegmentMode & 2);
    GdtEntry->Bits.Dpl = (Dpl & 0x3);
    GdtEntry->Bits.Granularity = Granularity;
    GdtEntry->Bits.LongMode = !!(SegmentMode & 1);
    GdtEntry->Bits.Present = (Type != 0);
    GdtEntry->Bits.System = 0;
    GdtEntry->Bits.Type = (Type & 0x1F);
    GdtEntry->MustBeZero = 0;
}

/**
 * Fills in a call, interrupt, task or trap gate entry.
 *
 * @param Idt
 *        Supplies a pointer to IDT structure, where gate is located.
 *
 * @param Vector
 *        Supplies a gate vector pointing to the interrupt gate in the IDT
 *
 * @param Handler
 *        Supplies a pointer to the interrupt handler of the specified gate.
 *
 * @param Selector
 *        Supplies the code selector the gate should run in.
 *
 * @param Ist
 *        Supplies the interrupt stack table entry the gate should run in.
 *
 * @param Access
 *        Supplies the gate access rights.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
ArpSetIdtGate(IN PKIDTENTRY Idt,
              IN USHORT Vector,
              IN PVOID Handler,
              IN USHORT Selector,
              IN USHORT Ist,
              IN USHORT Access)
{
    /* Setup the gate */
    Idt[Vector].OffsetLow = (ULONG_PTR)Handler;
    Idt[Vector].OffsetMiddle = ((ULONG_PTR)Handler >> 16);
    Idt[Vector].OffsetHigh = (ULONG_PTR)Handler >> 32;
    Idt[Vector].Dpl = Access;
    Idt[Vector].IstIndex = Ist;
    Idt[Vector].Present = 1;
    Idt[Vector].Selector = Selector;
    Idt[Vector].Type = 0xE;
}
