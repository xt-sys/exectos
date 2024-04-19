/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/procsup.c
 * DESCRIPTION:     AMD64 processor functionality support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
    HlSetRunLevel(PASSIVE_LEVEL);

    /* Initialize segment registers */
    ArpInitializeSegments();

    /* Set GS base */
    ArWriteModelSpecificRegister(X86_MSR_GSBASE, (ULONGLONG)ProcessorBlock);
    ArWriteModelSpecificRegister(X86_MSR_KERNEL_GSBASE, (ULONGLONG)ProcessorBlock);

    /* Initialize processor registers */
    ArpInitializeProcessorRegisters();

    /* Identify processor */
    ArpIdentifyProcessor();
}

/**
 * Updates an existing AMD64 GDT entry with new base address.
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
    GdtEntry->BaseUpper = (Base >> 32);
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
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_CMCODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_USER, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_CODE, 0x0, 0x0, KGDT_TYPE_CODE, KGDT_DPL_USER, 1);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_SYS_TSS, (ULONG_PTR)ProcessorBlock->TssBase, sizeof(KTSS), AMD64_TSS, KGDT_DPL_SYSTEM, 0);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_CMTEB, 0x0, 0x0FFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    ArpSetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_LDT, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 0);
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
        ArpSetIdtGate(ProcessorBlock->IdtBase, Vector, ArpHandleTrapFF, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    }

    /* Setup IDT handlers for known interrupts and traps */
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x00, ArpTrap0x00, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x01, ArpTrap0x01, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x02, ArpTrap0x02, KGDT_R0_CODE, KIDT_IST_PANIC, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x03, ArpTrap0x03, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING3);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x04, ArpTrap0x04, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING3);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x05, ArpTrap0x05, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x06, ArpTrap0x06, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x07, ArpTrap0x07, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x08, ArpTrap0x08, KGDT_R0_CODE, KIDT_IST_PANIC, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x09, ArpTrap0x09, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0A, ArpTrap0x0A, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0B, ArpTrap0x0B, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0C, ArpTrap0x0C, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0D, ArpTrap0x0D, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x0E, ArpTrap0x0E, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x10, ArpTrap0x10, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x11, ArpTrap0x11, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x12, ArpTrap0x12, KGDT_R0_CODE, KIDT_IST_MCA, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x13, ArpTrap0x13, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x1F, ArpTrap0x1F, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x2C, ArpTrap0x2C, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING3);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x2D, ArpTrap0x2D, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING3);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0x2F, ArpTrap0x2F, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
    ArpSetIdtGate(ProcessorBlock->IdtBase, 0xE1, ArpTrap0xE1, KGDT_R0_CODE, KIDT_IST_RESERVED, KIDT_ACCESS_RING0);
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

    /* Set process and thread information */
    ProcessorBlock->Prcb.CurrentThread = &KeInitialThread.ThreadControlBlock;
    ProcessorBlock->Prcb.CurrentThread->ApcState.Process = &KeInitialProcess.ProcessControlBlock;
    ProcessorBlock->Prcb.IdleThread = &KeInitialThread.ThreadControlBlock;
    ProcessorBlock->Prcb.NextThread = NULL;

    /* Set initial MXCSR register value */
    ProcessorBlock->Prcb.MxCsr = INITIAL_MXCSR;

    /* Set initial runlevel */
    ProcessorBlock->RunLevel = PASSIVE_LEVEL;
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
    ULONGLONG PatAttributes;

    /* Enable FXSAVE restore */
    ArWriteControlRegister(4, ArReadControlRegister(4) | CR4_FXSR);

    /* Enable XMMI exceptions */
    ArWriteControlRegister(4, ArReadControlRegister(4) | CR4_XMMEXCPT);

    /* Set debugger extension */
    ArWriteControlRegister(4, ArReadControlRegister(4) | CR4_DE);

    /* Enable large pages */
    ArWriteControlRegister(4, ArReadControlRegister(4) | CR4_PSE);

    /* Enable write-protection */
    ArWriteControlRegister(0, ArReadControlRegister(0) | CR0_WP);

    /* Set alignment mask */
    ArWriteControlRegister(0, ArReadControlRegister(0) | CR0_AM);

    /* Disable FPU monitoring */
    ArWriteControlRegister(0, ArReadControlRegister(0) & ~CR0_MP);

    /* Disable x87 FPU exceptions */
    ArWriteControlRegister(0, ArReadControlRegister(0) & ~CR0_NE);

    /* Flush the TLB */
    ArFlushTlb();

    /* Initialize system calls MSR */
    ArWriteModelSpecificRegister(X86_MSR_STAR, (((ULONG64)KGDT_R3_CMCODE | RPL_MASK) << 48) | ((ULONG64)KGDT_R0_CODE << 32));
    ArWriteModelSpecificRegister(X86_MSR_CSTAR, (ULONG64)&ArpHandleSystemCall32);
    ArWriteModelSpecificRegister(X86_MSR_LSTAR, (ULONG64)&ArpHandleSystemCall64);
    ArWriteModelSpecificRegister(X86_MSR_FMASK, X86_EFLAGS_IF_MASK | X86_EFLAGS_TF_MASK);

    /* Enable system call extensions (SCE) in EFER MSR */
    ArWriteModelSpecificRegister(X86_MSR_EFER, ArReadModelSpecificRegister(X86_MSR_EFER) | X86_MSR_EFER_SCE);

    /* Enable No-Execute (NXE) in EFER MSR */
    ArWriteModelSpecificRegister(X86_MSR_EFER, ArReadModelSpecificRegister(X86_MSR_EFER) | X86_MSR_EFER_NXE);

    /* Initialize Page Attribute Table */
    PatAttributes = (PAT_TYPE_WB << 0) | (PAT_TYPE_USWC << 8) | (PAT_TYPE_WEAK_UC << 16) | (PAT_TYPE_STRONG_UC << 24) |
                    (PAT_TYPE_WB << 32) | (PAT_TYPE_USWC << 40) | (PAT_TYPE_WEAK_UC << 48) | (PAT_TYPE_STRONG_UC << 56);
    ArWriteModelSpecificRegister(X86_MSR_PAT, PatAttributes);

    /* Initialize MXCSR register */
    ArLoadMxcsrRegister(INITIAL_MXCSR);
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
    /* Initialize segments */
    ArLoadSegment(SEGMENT_CS, KGDT_R0_CODE);
    ArLoadSegment(SEGMENT_DS, KGDT_R3_DATA | RPL_MASK);
    ArLoadSegment(SEGMENT_ES, KGDT_R3_DATA | RPL_MASK);
    ArLoadSegment(SEGMENT_FS, KGDT_R3_CMTEB | RPL_MASK);
    ArLoadSegment(SEGMENT_GS, KGDT_R3_DATA | RPL_MASK);
    ArLoadSegment(SEGMENT_SS, KGDT_R0_DATA);
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
    ProcessorBlock->TssBase->Rsp0 = (ULONG_PTR)&ArKernelBootStack;
    ProcessorBlock->TssBase->Ist[KIDT_IST_PANIC] = (ULONG_PTR)&ArKernelFaultStack;
    ProcessorBlock->TssBase->Ist[KIDT_IST_MCA] = (ULONG_PTR)&ArKernelFaultStack;
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
    GdtEntry->BaseUpper = (Base >> 32);

    /* Set descriptor limit */
    GdtEntry->LimitLow = (Limit & 0xFFFF);
    GdtEntry->Bits.LimitHigh = ((Limit >> 16) & 0xF);

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
    Idt[Vector].OffsetLow = ((ULONG_PTR)Handler & 0xFFFF);
    Idt[Vector].OffsetMiddle = (((ULONG_PTR)Handler >> 16) & 0xFFFF);
    Idt[Vector].OffsetHigh = (ULONG_PTR)Handler >> 32;
    Idt[Vector].Dpl = Access;
    Idt[Vector].IstIndex = Ist;
    Idt[Vector].Present = 1;
    Idt[Vector].Selector = Selector;
    Idt[Vector].Type = 0xE;
}
