/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/procsup.cc
 * DESCRIPTION:     I686 processor functionality support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Gets the base address of the kernel boot stack.
 *
 * @return This routine returns a pointer to the kernel boot stack.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
AR::ProcSup::GetBootStack(VOID)
{
    return (PVOID)BootStack;
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
AR::ProcSup::IdentifyProcessor(VOID)
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
    Prcb->CpuId.Vendor = (CPU_VENDOR)CpuRegisters.Ebx;
    *(PULONG)&Prcb->CpuId.VendorName[0] = CpuRegisters.Ebx;
    *(PULONG)&Prcb->CpuId.VendorName[4] = CpuRegisters.Edx;
    *(PULONG)&Prcb->CpuId.VendorName[8] = CpuRegisters.Ecx;
    Prcb->CpuId.VendorName[12] = '\0';

    /* Get CPU standard features */
    RtlZeroMemory(&CpuRegisters, sizeof(CPUID_REGISTERS));
    CpuRegisters.Leaf = CPUID_GET_STANDARD1_FEATURES;
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
        if(Prcb->CpuId.Family >= 0xF)
        {
            Prcb->CpuId.Family = Prcb->CpuId.Family + CpuSignature.ExtendedFamily;
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

        if((Prcb->CpuId.Family == 0x6) || (Prcb->CpuId.Family == 0xF))
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
 * Initializes i686 processor specific structures.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
AR::ProcSup::InitializeProcessor(IN PVOID ProcessorStructures)
{
    KDESCRIPTOR GdtDescriptor, IdtDescriptor;
    PVOID KernelBootStack, KernelFaultStack;
    PKPROCESSOR_BLOCK ProcessorBlock;
    PKGDTENTRY Gdt;
    PKIDTENTRY Idt;
    PKTSS Tss;

    /* Check if processor structures buffer provided */
    if(ProcessorStructures)
    {
        /* Assign CPU structures from provided buffer */
        InitializeProcessorStructures(ProcessorStructures, &Gdt, &Tss, &ProcessorBlock,
                                      &KernelBootStack, &KernelFaultStack);

        /* Use global IDT */
        Idt = InitialIdt;
    }
    else
    {
        /* Use initial structures */
        Gdt = InitialGdt;
        Idt = InitialIdt;
        Tss = &InitialTss;
        KernelBootStack = &BootStack;
        KernelFaultStack = &FaultStack;
        ProcessorBlock = &InitialProcessorBlock;
    }

    /* Initialize processor block */
    InitializeProcessorBlock(ProcessorBlock, Gdt, Idt, Tss, KernelFaultStack);

    /* Initialize GDT, IDT and TSS */
    InitializeGdt(ProcessorBlock);
    InitializeIdt(ProcessorBlock);
    InitializeTss(ProcessorBlock, KernelBootStack, KernelFaultStack);

    /* Set GDT and IDT descriptors */
    GdtDescriptor.Base = Gdt;
    GdtDescriptor.Limit = (GDT_ENTRIES * sizeof(KGDTENTRY)) - 1;
    IdtDescriptor.Base = Idt;
    IdtDescriptor.Limit = (IDT_ENTRIES * sizeof(KIDTENTRY)) - 1;

    /* Load GDT, IDT and TSS */
    CpuFunc::LoadGlobalDescriptorTable(&GdtDescriptor.Limit);
    CpuFunc::LoadInterruptDescriptorTable(&IdtDescriptor.Limit);
    CpuFunc::LoadTaskRegister((UINT)KGDT_SYS_TSS);

    /* Enter passive IRQ level */
    HL::RunLevel::SetRunLevel(PASSIVE_LEVEL);

    /* Initialize segment registers */
    InitializeSegments();

    /* Initialize processor registers */
    InitializeProcessorRegisters();

    /* Identify processor */
    IdentifyProcessor();
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
AR::ProcSup::InitializeGdt(IN PKPROCESSOR_BLOCK ProcessorBlock)
{
    /* Initialize GDT entries */
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_NULL, 0x0, 0x0, KGDT_TYPE_NONE, KGDT_DPL_SYSTEM, 0);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_CODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 2);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 2);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_CODE, 0x0, 0xFFFFFFFF, KGDT_TYPE_CODE, KGDT_DPL_USER, 2);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_DATA, 0x0, 0xFFFFFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 2);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_SYS_TSS, (ULONG_PTR)ProcessorBlock->TssBase, sizeof(KTSS) - 1, I686_TSS, KGDT_DPL_SYSTEM, 0);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_PB, (ULONG_PTR)ProcessorBlock, sizeof(KPROCESSOR_BLOCK), KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 2);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_R3_TEB, 0x0, 0xFFF, KGDT_TYPE_DATA | KGDT_DESCRIPTOR_ACCESSED, KGDT_DPL_USER, 2);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_VDM_TILE, 0x0400, 0xFFFF, KGDT_TYPE_DATA, KGDT_DPL_USER, 0);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_R0_LDT, 0x0, 0x0, KGDT_TYPE_NONE, KGDT_DPL_SYSTEM, 0);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_DF_TSS, 0x20000, 0xFFFF, I686_TSS, KGDT_DPL_SYSTEM, 0);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_NMI_TSS, 0x20000, 0xFFFF, KGDT_TYPE_CODE, KGDT_DPL_SYSTEM, 0);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_VDBS, 0xB8000, 0x3FFF, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 0);
    SetGdtEntry(ProcessorBlock->GdtBase, KGDT_ALIAS, (ULONG_PTR)ProcessorBlock->GdtBase, (GDT_ENTRIES * sizeof(KGDTENTRY)) - 1, KGDT_TYPE_DATA, KGDT_DPL_SYSTEM, 0);
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
AR::ProcSup::InitializeIdt(IN PKPROCESSOR_BLOCK ProcessorBlock)
{
    UINT Vector;

    /* Fill in all vectors */
    for(Vector = 0; Vector < IDT_ENTRIES; Vector++)
    {
        /* Set the IDT to handle unexpected interrupts */
        SetIdtGate(ProcessorBlock->IdtBase, Vector, (PVOID)ArTrap0xFF, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    }

    /* Setup IDT handlers for known interrupts and traps */
    SetIdtGate(ProcessorBlock->IdtBase, 0x00, (PVOID)ArTrap0x00, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x01, (PVOID)ArTrap0x01, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x02, (PVOID)ArTrap0x02, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x03, (PVOID)ArTrap0x03, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING3);
    SetIdtGate(ProcessorBlock->IdtBase, 0x04, (PVOID)ArTrap0x04, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING3);
    SetIdtGate(ProcessorBlock->IdtBase, 0x05, (PVOID)ArTrap0x05, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x06, (PVOID)ArTrap0x06, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x07, (PVOID)ArTrap0x07, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x08, (PVOID)ArTrap0x08, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x09, (PVOID)ArTrap0x09, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x0A, (PVOID)ArTrap0x0A, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x0B, (PVOID)ArTrap0x0B, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x0C, (PVOID)ArTrap0x0C, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x0D, (PVOID)ArTrap0x0D, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x0E, (PVOID)ArTrap0x0E, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x10, (PVOID)ArTrap0x10, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x11, (PVOID)ArTrap0x11, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x12, (PVOID)ArTrap0x12, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x13, (PVOID)ArTrap0x13, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING0);
    SetIdtGate(ProcessorBlock->IdtBase, 0x2A, (PVOID)ArTrap0x2A, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING3);
    SetIdtGate(ProcessorBlock->IdtBase, 0x2B, (PVOID)ArTrap0x2B, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING3);
    SetIdtGate(ProcessorBlock->IdtBase, 0x2C, (PVOID)ArTrap0x2C, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING3);
    SetIdtGate(ProcessorBlock->IdtBase, 0x2D, (PVOID)ArTrap0x2D, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING3);
    SetIdtGate(ProcessorBlock->IdtBase, 0x2E, (PVOID)ArTrap0x2E, KGDT_R0_CODE, 0, KIDT_INTERRUPT | KIDT_ACCESS_RING3);
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
AR::ProcSup::InitializeProcessorBlock(OUT PKPROCESSOR_BLOCK ProcessorBlock,
                                      IN PKGDTENTRY Gdt,
                                      IN PKIDTENTRY Idt,
                                      IN PKTSS Tss,
                                      IN PVOID DpcStack)
{
    /* Set processor block and processor control block */
    ProcessorBlock->Self = ProcessorBlock;
    ProcessorBlock->CurrentPrcb = &ProcessorBlock->Prcb;

    /* Set GDT, IDT and TSS descriptors */
    ProcessorBlock->GdtBase = (PKGDTENTRY)(PVOID)Gdt;
    ProcessorBlock->IdtBase = Idt;
    ProcessorBlock->TssBase = Tss;

    /* Setup DPC stack */
    ProcessorBlock->Prcb.DpcStack = DpcStack;

    /* Setup processor control block */
    ProcessorBlock->Prcb.CpuNumber = ProcessorBlock->CpuNumber;
    ProcessorBlock->Prcb.SetMember = 1 << ProcessorBlock->CpuNumber;
    ProcessorBlock->Prcb.MultiThreadProcessorSet = 1 << ProcessorBlock->CpuNumber;

    /* Clear DR6 and DR7 registers */
    ProcessorBlock->Prcb.ProcessorState.SpecialRegisters.KernelDr6 = 0;
    ProcessorBlock->Prcb.ProcessorState.SpecialRegisters.KernelDr7 = 0;

    /* Set process and thread information */
    ProcessorBlock->Prcb.CurrentThread = &(KE::KThread::GetInitialThread())->ThreadControlBlock;
    ProcessorBlock->Prcb.CurrentThread->ApcState.Process = &(KE::KProcess::GetInitialProcess())->ProcessControlBlock;
    ProcessorBlock->Prcb.IdleThread = &(KE::KThread::GetInitialThread())->ThreadControlBlock;
    ProcessorBlock->Prcb.NextThread = nullptr;

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
AR::ProcSup::InitializeProcessorRegisters(VOID)
{
    /* Clear EFLAGS register */
    CpuFunc::WriteEflagsRegister(0);

    /* Enable write-protection */
    CpuFunc::WriteControlRegister(0, CpuFunc::ReadControlRegister(0) | CR0_WP);
}

/**
 * Initializes i686 processor specific structures with provided memory buffer.
 *
 * @param ProcessorStructures
 *        Supplies a pointer to the allocated buffer with processor structures.
 *
 * @param Gdt
 *        Supplies a pointer to the GDT.
 *
 * @param Tss
 *        Supplies a pointer to the TSS.
 *
 * @param ProcessorBlock
 *        Supplies a pointer to the processor block.
 *
 * @param KernelBootStack
 *        Supplies a pointer to the kernel boot stack.
 *
 * @param KernelFaultStack
 *        Supplies a pointer to the kernel fault stack.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
AR::ProcSup::InitializeProcessorStructures(IN PVOID ProcessorStructures,
                                           OUT PKGDTENTRY *Gdt,
                                           OUT PKTSS *Tss,
                                           OUT PKPROCESSOR_BLOCK *ProcessorBlock,
                                           OUT PVOID *KernelBootStack,
                                           OUT PVOID *KernelFaultStack)
{
    UINT_PTR Address;

    /* Align address to page size boundary and move to kernel boot stack */
    Address = ROUND_UP((UINT_PTR)ProcessorStructures, MM_PAGE_SIZE) + KERNEL_STACK_SIZE;

    /* Assign a space for kernel boot stack and advance */
    *KernelBootStack = (PVOID)Address;
    Address += KERNEL_STACK_SIZE;

    /* Assign a space for kernel fault stack, no advance needed as stack grows down */
    *KernelFaultStack = (PVOID)Address;

    /* Assign a space for GDT and advance */
    *Gdt = (PKGDTENTRY)(PVOID)Address;
    Address += sizeof(ArInitialGdt);

    /* Assign a space for Processor Block and advance */
    *ProcessorBlock = (PKPROCESSOR_BLOCK)(PVOID)Address;
    Address += sizeof(ArInitialProcessorBlock);

    /* Assign a space for TSS */
    *Tss = (PKTSS)(PVOID)Address;
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
AR::ProcSup::InitializeSegments(VOID)
{
    /* Initialize segments */
    CpuFunc::LoadSegment(SEGMENT_CS, KGDT_R0_CODE);
    CpuFunc::LoadSegment(SEGMENT_DS, KGDT_R3_DATA | RPL_MASK);
    CpuFunc::LoadSegment(SEGMENT_ES, KGDT_R3_DATA | RPL_MASK);
    CpuFunc::LoadSegment(SEGMENT_FS, KGDT_R0_PB);
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
AR::ProcSup::InitializeTss(IN PKPROCESSOR_BLOCK ProcessorBlock,
                           IN PVOID KernelBootStack,
                           IN PVOID KernelFaultStack)
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
    ProcessorBlock->TssBase->Esp0 = (ULONG_PTR)KernelBootStack;
    ProcessorBlock->TssBase->Flags = 0;

    /* Set LDT and SS */
    ProcessorBlock->TssBase->LDT = KGDT_R0_LDT;
    ProcessorBlock->TssBase->Ss0 = KGDT_R0_DATA;

    /* Initialize task gates for DoubleFault and NMI traps */
    SetDoubleFaultTssEntry(ProcessorBlock, KernelFaultStack);
    SetNonMaskableInterruptTssEntry(ProcessorBlock, KernelFaultStack);
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
AR::ProcSup::SetDoubleFaultTssEntry(IN PKPROCESSOR_BLOCK ProcessorBlock,
                                    IN PVOID KernelFaultStack)
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
    Tss = (PKTSS)DoubleFaultTss;
    Tss->IoMapBase = sizeof(KTSS);
    Tss->Flags = 0;
    Tss->LDT = KGDT_R0_LDT;
    Tss->CR3 = CpuFunc::ReadControlRegister(3);
    Tss->Esp = (ULONG_PTR)KernelFaultStack;
    Tss->Esp0 = (ULONG_PTR)KernelFaultStack;
    Tss->Eip = PtrToUlong(ArTrap0x08);
    Tss->Cs = KGDT_R0_CODE;
    Tss->Ds = KGDT_R3_DATA | RPL_MASK;
    Tss->Es = KGDT_R3_DATA | RPL_MASK;
    Tss->Fs = KGDT_R0_PB;
    Tss->Ss0 = KGDT_R0_DATA;
    CpuFunc::StoreSegment(SEGMENT_SS, (PVOID)&Tss->Ss);

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
AR::ProcSup::SetGdtEntry(IN PKGDTENTRY Gdt,
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
AR::ProcSup::SetGdtEntryBase(IN PKGDTENTRY Gdt,
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
AR::ProcSup::SetIdtGate(IN PKIDTENTRY Idt,
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
AR::ProcSup::SetNonMaskableInterruptTssEntry(IN PKPROCESSOR_BLOCK ProcessorBlock,
                                             IN PVOID KernelFaultStack)
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
    Tss = (PKTSS)NonMaskableInterruptTss;
    Tss->IoMapBase = sizeof(KTSS);
    Tss->Flags = 0;
    Tss->LDT = KGDT_R0_LDT;
    Tss->CR3 = ArReadControlRegister(3);
    Tss->Esp = (ULONG_PTR)KernelFaultStack;
    Tss->Esp0 = (ULONG_PTR)KernelFaultStack;
    Tss->Eip = PtrToUlong(ArTrap0x02);
    Tss->Cs = KGDT_R0_CODE;
    Tss->Ds = KGDT_R3_DATA | RPL_MASK;
    Tss->Es = KGDT_R3_DATA | RPL_MASK;
    Tss->Fs = KGDT_R0_PB;
    CpuFunc::StoreSegment(SEGMENT_SS, (PVOID)&Tss->Ss);

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
