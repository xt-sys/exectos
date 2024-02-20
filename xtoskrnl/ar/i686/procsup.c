/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/procsup.c
 * DESCRIPTION:     I686 processor functionality support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
    PVOID KernelFaultStack;
    PKGDTENTRY Gdt;
    PKIDTENTRY Idt;
    PKTSS Tss;

    /* Use initial structures */
    Gdt = ArInitialGdt;
    Idt = ArInitialIdt;
    Tss = &ArInitialTss;
    KernelFaultStack = &ArKernelFaultStack;

    /* Load processor block */
    ProcessorBlock = CONTAIN_RECORD(&ArInitialProcessorBlock.Prcb, KPROCESSOR_BLOCK, Prcb);

    /* Initialize processor block */
    ArpInitializeProcessorBlock(ProcessorBlock, Gdt, Idt, Tss, KernelFaultStack);

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

    /* Enter passive IRQ level */
    ProcessorBlock->RunLevel = PASSIVE_LEVEL;

    /* Initialize segment registers */
    ArpInitializeSegments();

    /* Load FS segment */
    ArLoadSegment(SEGMENT_FS, KGDT_R0_PB);

    /* Initialize processor registers */
    ArpInitializeProcessorRegisters();

    /* Identify processor */
    ArpIdentifyProcessor();
}

/**
 * Updates an existing i686 GDT entry with new base address.
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
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
ArSetGdtEntryBase(IN PKGDTENTRY Gdt,
                  IN USHORT Selector,
                  IN ULONG_PTR Base)
{
    PKGDTENTRY GdtEntry;

    /* Get GDT entry */
    GdtEntry = (PKGDTENTRY)((ULONG_PTR)Gdt + (Selector & ~RPL_MASK));

    /* Set new GDT descriptor base */
    GdtEntry->BaseLow = (Base & 0xFFFF);
    GdtEntry->Bytes.BaseMiddle = ((Base >> 16) & 0xFF);
    GdtEntry->Bytes.BaseHigh = ((Base >> 24) & 0xFF);
}

/**
 * Identifies processor type (vendor, model, stepping) as well as looks for available CPU features and stores them
 * in Processor Control Block (PRCB).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
ArpIdentifyProcessor(VOID)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    CPUID_REGISTERS CpuRegisters;
    CPUID_SIGNATURE CpuSignature;

    /* Not fully implemented yet */
    UNIMPLEMENTED;

    /* Get current processor control block */
    Prcb = KeGetCurrentProcessorControlBlock();

    /* Get CPU vendor by issueing CPUID instruction */
    RtlZeroMemory(&CpuRegisters, sizeof(CPUID_REGISTERS));
    CpuRegisters.Leaf = CPUID_GET_VENDOR_STRING;
    ArCpuId(&CpuRegisters);

    /* Store CPU vendor in processor control block */
    Prcb->CpuId.Vendor = CpuRegisters.Ebx;
    Prcb->CpuId.VendorName[0] = CpuRegisters.Ebx;
    Prcb->CpuId.VendorName[4] = CpuRegisters.Edx;
    Prcb->CpuId.VendorName[8] = CpuRegisters.Ecx;
    Prcb->CpuId.VendorName[12] = '\0';

    /* Get CPU features */
    RtlZeroMemory(&CpuRegisters, sizeof(CPUID_REGISTERS));
    CpuRegisters.Leaf = CPUID_GET_CPU_FEATURES;
    ArCpuId(&CpuRegisters);

    /* Store CPU signature in processor control block */
    CpuSignature = *(PCPUID_SIGNATURE)&CpuRegisters.Eax;
    Prcb->CpuId.Family = CpuSignature.Family;
    Prcb->CpuId.Model = CpuSignature.Model;
    Prcb->CpuId.Stepping = CpuSignature.Stepping;

    /* CPU vendor specific quirks */
    if(Prcb->CpuId.Vendor == CPU_VENDOR_AMD)
    {
        /* AMD CPU */
        Prcb->CpuId.Family = Prcb->CpuId.Family + CpuSignature.ExtendedFamily;
        if(Prcb->CpuId.Model == 0xF)
        {
            Prcb->CpuId.Model = Prcb->CpuId.Model + (CpuSignature.ExtendedModel << 4);
        }
    }
    else if(Prcb->CpuId.Vendor == CPU_VENDOR_INTEL)
    {
        /* Intel CPU */
        if(Prcb->CpuId.Family == 0xF)
        {
            Prcb->CpuId.Family = Prcb->CpuId.Family + CpuSignature.ExtendedFamily;
        }

        if((Prcb->CpuId.Family == 0xF) || (Prcb->CpuId.Family == 0x6))
        {
            Prcb->CpuId.Model = Prcb->CpuId.Model + (CpuSignature.ExtendedModel << 4);
        }
    }
    else
    {
        /* Unknown CPU vendor */
        Prcb->CpuId.Vendor = CPU_VENDOR_UNKNOWN;
    }

    /* TODO: Store a list of CPU features in processor control block */
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
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_SYS_TSS, (ULONG_PTR)ProcessorBlock->TssBase, sizeof(KTSS) - 1, I686_TSS, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_PB, (ULONG_PTR)ProcessorBlock, sizeof(KPROCESSOR_BLOCK), KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_TEB, 0x0, 0xFFF, KGDT_TYPE_DATA | KGDT_DESCRIPTOR_ACCESSED, KGDT_DPL_USER, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_VDM_TILE, 0x0400, 0xFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_LDT, 0x0, 0x0, KGDT_TYPE_NONE, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_DF_TSS, 0x20000, 0xFFFF, I686_TSS, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_NMI_TSS, 0x20000, 0xFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_VDBS, 0xB8000, 0x3FFF, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_ALIAS, (ULONG_PTR)ProcessorBlock->GdtBase, (GDT_ENTRIES * sizeof(KGDTENTRY)) - 1, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 0);
}

/**
 * Initializes the kernel's Interrupt Descriptor Table (IDT).
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
        ArpSetIdtGate(ProcessorBlock->IdtBase, Vector, ArpHandleTrapFF, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    }

    /* Setup IDT handlers for known interrupts and traps */
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x00, ArpHandleTrap00, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x01, ArpHandleTrap01, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x02, ArpHandleTrap02, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x03, ArpHandleTrap03, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING3);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x04, ArpHandleTrap04, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING3);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x05, ArpHandleTrap05, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x06, ArpHandleTrap06, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x07, ArpHandleTrap07, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x08, ArpHandleTrap08, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x09, ArpHandleTrap09, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0A, ArpHandleTrap0A, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0B, ArpHandleTrap0B, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0C, ArpHandleTrap0C, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0D, ArpHandleTrap0D, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0E, ArpHandleTrap0E, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x10, ArpHandleTrap10, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x11, ArpHandleTrap11, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x12, ArpHandleTrap12, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x13, ArpHandleTrap13, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x2C, ArpHandleTrap2C, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING3);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x2D, ArpHandleTrap2D, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING3);
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

    /* Set process and thread information */
    ProcessorBlock->Prcb.CurrentThread = &KeInitialThread.ThreadControlBlock;
    ProcessorBlock->Prcb.CurrentThread->ApcState.Process = &KeInitialProcess.ProcessControlBlock;
    ProcessorBlock->Prcb.IdleThread = &KeInitialThread.ThreadControlBlock;
    ProcessorBlock->Prcb.NextThread = NULL;
}

/**
 * Initializes processor registers and other boot structures.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
ArpInitializeProcessorRegisters(VOID)
{
    /* Clear EFLAGS register */
    ArWriteEflagsRegister(0);

    /* Enable write-protection */
    ArWriteControlRegister(0, ArReadControlRegister(0) | CR0_WP);
}

/**
 * Initializes segment registers.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
ArpInitializeSegments(VOID)
{
    ArLoadSegment(SEGMENT_DS, KGDT_R3_DATA | RPL_MASK);
    ArLoadSegment(SEGMENT_ES, KGDT_R3_DATA | RPL_MASK);
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
    /* Clear I/O map */
    RtlSetMemory(ProcessorBlock->TssBase->IoMaps[0].IoMap, 0xFF, IOPM_FULL_SIZE);

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

    /* Initialize task gates for DoubleFault and NMI traps */
    ArpSetDoubleFaultTssEntry(ProcessorBlock);
    ArpSetNonMaskableInterruptTssEntry(ProcessorBlock);
}

/**
 * Initializes the DoubleFault TSS entry in the Global Descriptor Table.
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
ArpSetDoubleFaultTssEntry(IN PKPROCESSOR_BLOCK ProcessorBlock)
{
    PKGDTENTRY TaskGateEntry, TssEntry;
    PKTSS Tss;

    /* Setup task gate for DoubleFault trap */
    TaskGateEntry = (PKGDTENTRY)&ProcessorBlock->IdtBase[8];
    TaskGateEntry->Bits.Dpl = 0;
    TaskGateEntry->Bits.Present = 1;
    TaskGateEntry->Bits.Type = I686_TASK_GATE;
    ((PKIDTENTRY)TaskGateEntry)->Selector = KGDT_DF_TSS;

    /* Initialize DoubleFault TSS and set initial state */
    Tss = (PKTSS)ArpDoubleFaultTss;
    Tss->IoMapBase = sizeof(KTSS);
    Tss->Flags = 0;
    Tss->LDT = KGDT_R0_LDT;
    Tss->CR3 = ArReadControlRegister(3);
    Tss->Esp = (ULONG_PTR)&ArKernelFaultStack;
    Tss->Esp0 = (ULONG_PTR)&ArKernelFaultStack;
    Tss->Eip = PtrToUlong(ArpHandleTrap08);
    Tss->Cs = KGDT_R0_CODE;
    Tss->Ds = KGDT_R3_DATA | RPL_MASK;
    Tss->Es = KGDT_R3_DATA | RPL_MASK;
    Tss->Fs = KGDT_R0_PB;
    Tss->Ss0 = KGDT_R0_DATA;
    ArStoreSegment(SEGMENT_SS, (PVOID)&Tss->Ss);

    /* Setup DoubleFault TSS entry in Global Descriptor Table */
    TssEntry = (PKGDTENTRY)(&(ProcessorBlock->GdtBase[KGDT_DF_TSS / sizeof(KGDTENTRY)]));
    TssEntry->BaseLow = ((ULONG_PTR)Tss & 0xFFFF);
    TssEntry->Bytes.BaseMiddle = ((ULONG_PTR)Tss >> 16);
    TssEntry->Bytes.BaseHigh = ((ULONG_PTR)Tss >> 24);
    TssEntry->LimitLow = sizeof(KTSS) - 1;
    TssEntry->Bits.LimitHigh = 0;
    TssEntry->Bits.Dpl = 0;
    TssEntry->Bits.Present = 1;
    TssEntry->Bits.Type = I686_TSS;
}

/**
 * Fills in an i686 GDT entry.
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
    Idt[Vector].Offset = (USHORT)((ULONG)Handler & 0xFFFF);
    Idt[Vector].ExtendedOffset = (USHORT)((ULONG)Handler >> 16);
    Idt[Vector].Access = 0x8000 | (Access << 8);
    Idt[Vector].Selector = Selector;
}

/**
 * Initializes the Non-Maskable Interrupt TSS entry in the Global Descriptor Table.
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
ArpSetNonMaskableInterruptTssEntry(IN PKPROCESSOR_BLOCK ProcessorBlock)
{
    PKGDTENTRY TaskGateEntry, TssEntry;
    PKTSS Tss;

    /* Setup task gate for NMI */
    TaskGateEntry = (PKGDTENTRY)&ProcessorBlock->IdtBase[2];
    TaskGateEntry->Bits.Dpl = 0;
    TaskGateEntry->Bits.Present = 1;
    TaskGateEntry->Bits.Type = I686_TASK_GATE;
    ((PKIDTENTRY)TaskGateEntry)->Selector = KGDT_NMI_TSS;

    /* Initialize NMI TSS and set initial state */
    Tss = (PKTSS)ArpNonMaskableInterruptTss;
    Tss->IoMapBase = sizeof(KTSS);
    Tss->Flags = 0;
    Tss->LDT = KGDT_R0_LDT;
    Tss->CR3 = ArReadControlRegister(3);
    Tss->Esp = (ULONG_PTR)&ArKernelFaultStack;
    Tss->Esp0 = (ULONG_PTR)&ArKernelFaultStack;
    Tss->Eip = PtrToUlong(ArpHandleTrap02);
    Tss->Cs = KGDT_R0_CODE;
    Tss->Ds = KGDT_R3_DATA | RPL_MASK;
    Tss->Es = KGDT_R3_DATA | RPL_MASK;
    Tss->Fs = KGDT_R0_PB;
    ArStoreSegment(SEGMENT_SS, (PVOID)&Tss->Ss);

    /* Setup NMI TSS entry in Global Descriptor Table */
    TssEntry = (PKGDTENTRY)(&(ProcessorBlock->GdtBase[KGDT_NMI_TSS / sizeof(KGDTENTRY)]));
    TssEntry->BaseLow = ((ULONG_PTR)Tss & 0xFFFF);
    TssEntry->Bytes.BaseMiddle = ((ULONG_PTR)Tss >> 16);
    TssEntry->Bytes.BaseHigh = ((ULONG_PTR)Tss >> 24);
    TssEntry->LimitLow = sizeof(KTSS) - 1;
    TssEntry->Bits.LimitHigh = 0;
    TssEntry->Bits.Dpl = 0;
    TssEntry->Bits.Present = 1;
    TssEntry->Bits.Type = I686_TSS;
}
