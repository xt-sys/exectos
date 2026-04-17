/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/pic.cc
 * DESCRIPTION:     Programmable Interrupt Controller (PIC) for x86 (i686/AMD64) support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Allocates, maps and commits a requested system interrupt level internally.
 *
 * @param RunLevel
 *        Supplies the actual system run level to allocate.
 *
 * @param Vector
 *        Supplies the interrupt handler vector assigned to process requests originating on the line.
 *
 * @return This routine returns the configured target vector.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Pic::AllocateSystemInterrupt(IN UCHAR Irq,
                                 IN UCHAR RunLevel,
                                 IN UCHAR Vector)
{
    IOAPIC_REDIRECTION_REGISTER Register;
    PIOAPIC_DATA Controller;
    ULONG EntryNumber, Gsi;
    XTSTATUS Status;
    USHORT Flags;

    /* Determine the GSI and flags for the requested IRQ */
    ResolveInterruptOverride(Irq, &Gsi, &Flags);

    /* Find the APIC controller for the GSI */
    Status = GetIoApicController(Gsi, &Controller, &EntryNumber);
    if(Status != STATUS_SUCCESS)
    {
        /* GSI maps to an invalid controller, return */
        DebugPrint(L"ERROR: Hardware IRQ / GSI maps to an invalid controller!\n");
        return;
    }

    /* Model a logical connection */
    Register.DeliveryMode = APIC_DM_LOWPRIO;
    Register.DeliveryStatus = 0;
    Register.Destination = HL::Pic::ReadApicRegister(APIC_ID) >> 24;
    Register.DestinationMode = APIC_DM_Physical;
    Register.Mask = 1;
    Register.PinPolarity = ((Flags & 0x03) == 0x03) ? 1 : 0;
    Register.RemoteIRR = 0;
    Register.Reserved = 0;
    Register.TriggerMode = (((Flags >> 2) & 0x03) == 0x03) ? APIC_TGM_LEVEL : APIC_TGM_EDGE;
    Register.Vector = Vector;

    /* Flash logical rules back into the hardware configuration index */
    WriteRedirectionEntry(Controller, EntryNumber, Register);

    /* Persist the allocated slot so standard routing algorithms don't overlap it */
    MappedVectors[Vector] = RunLevel;
}

/**
 * Checks whether the APIC is supported by the processor.
 *
 * @return This routine returns TRUE if APIC is supported, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
HL::Pic::CheckApicSupport(VOID)
{
    CPUID_REGISTERS CpuRegisters;

    /* Prepare CPUID registers */
    CpuRegisters.Leaf = CPUID_GET_STANDARD1_FEATURES;
    CpuRegisters.SubLeaf = 0;
    CpuRegisters.Eax = 0;
    CpuRegisters.Ebx = 0;
    CpuRegisters.Ecx = 0;
    CpuRegisters.Edx = 0;

    /* Get CPUID */
    AR::CpuFunc::CpuId(&CpuRegisters);

    /* Check APIC status from the CPUID results */
    if(!(CpuRegisters.Edx & CPUID_FEATURES_EDX_APIC))
    {
        /* APIC is not supported */
        return FALSE;
    }

    /* APIC is supported */
    return TRUE;
}

/**
 * Checks whether the x2APIC extension is supported by the processor.
 *
 * @return This routine returns TRUE if x2APIC is supported, or FALSE otherwise.
 *
 * @since XT 1.0
 *
 * @todo Check if bits 0 and 1 of DMAR ACPI table flags are set after implementing ACPI support.
 *       Intel VT-d spec says x2apic should not be enabled if they are.
 */
XTAPI
BOOLEAN
HL::Pic::CheckX2ApicSupport(VOID)
{
    CPUID_REGISTERS CpuRegisters;

    /* Prepare CPUID registers */
    CpuRegisters.Leaf = CPUID_GET_STANDARD1_FEATURES;
    CpuRegisters.SubLeaf = 0;
    CpuRegisters.Eax = 0;
    CpuRegisters.Ebx = 0;
    CpuRegisters.Ecx = 0;
    CpuRegisters.Edx = 0;

    /* Get CPUID */
    AR::CpuFunc::CpuId(&CpuRegisters);

    /* Check x2APIC status from the CPUID results */
    if(!(CpuRegisters.Ecx & CPUID_FEATURES_ECX_X2APIC))
    {
        /* x2APIC is not supported */
        return FALSE;
    }

    /* x2APIC is supported */
    return TRUE;
}

/**
 * Clears all errors on the APIC.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Pic::ClearApicErrors(VOID)
{
    /* Clear APIC errors */
    WriteApicRegister(APIC_ESR, 0);
}

/**
 * Searches the ACPI MADT tables for the I/O APIC controllers.
 *
 * @return This routine returns the status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Pic::DetectIoApicControllers(VOID)
{
    PACPI_MADT_INTERRUPT_OVERRIDE OverrideDescriptor;
    PACPI_MADT_IOAPIC IoApicDescriptor;
    PACPI_SUBTABLE_HEADER SubTable;
    ULONG_PTR MadtTable;
    PACPI_MADT Madt;
    XTSTATUS Status;

    /* Initialize number of I/O APIC Controllers */
    ControllerCount = 0;

    /* Get Multiple APIC Description Table (MADT) */
    Status = HL::Acpi::GetAcpiTable(ACPI_MADT_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&Madt);
    if(Status == STATUS_SUCCESS && Madt != NULLPTR)
    {
        /* Set APIC table traverse pointer */
        MadtTable = (ULONG_PTR)Madt->ApicTables;

        /* Traverse all MADT tables to discover IOAPIC configurations */
        while(MadtTable < ((ULONG_PTR)Madt + Madt->Header.Length))
        {
            /* Extract active header element */
            SubTable = (PACPI_SUBTABLE_HEADER)MadtTable;

            /* Prevent infinite traversal loops on corrupted firmware definitions */
            if(SubTable->Length == 0)
            {
                /* Invalid MADT table, break loop */
                break;
            }

            /* Test specifically for I/O APIC component identity */
            if(SubTable->Type == ACPI_MADT_TYPE_IOAPIC &&
               SubTable->Length >= sizeof(ACPI_MADT_IOAPIC))
            {
                /* Extract I/O APIC descriptor */
                IoApicDescriptor = (PACPI_MADT_IOAPIC)MadtTable;

                /* Set information about this I/O APIC Controller */
                Controllers[ControllerCount].GsiBase = IoApicDescriptor->GlobalIrqBase;
                Controllers[ControllerCount].Identifier = IoApicDescriptor->IoApicId;
                Controllers[ControllerCount].PhysicalAddress.QuadPart = IoApicDescriptor->IoApicAddress;

                /* Increment I/O APIC controller index */
                ControllerCount++;
            }
            else if(SubTable->Type == ACPI_MADT_TYPE_INT_OVERRIDE &&
                    SubTable->Length >= sizeof(ACPI_MADT_INTERRUPT_OVERRIDE))
            {
                /* Check if maximum number of interrupt overrides has not been reached */
                if(IrqOverrideCount < IOAPIC_MAX_OVERRIDES)
                {
                    /* Extract interrupt override descriptor */
                    OverrideDescriptor = (PACPI_MADT_INTERRUPT_OVERRIDE)MadtTable;

                    /* Save information about this interrupt override */
                    IrqOverrides[IrqOverrideCount].Bus = OverrideDescriptor->Bus;
                    IrqOverrides[IrqOverrideCount].Flags = OverrideDescriptor->Flags;
                    IrqOverrides[IrqOverrideCount].GlobalSystemInterrupt = OverrideDescriptor->GlobalSystemInterrupt;
                    IrqOverrides[IrqOverrideCount].SourceIrq = OverrideDescriptor->SourceIrq;

                    /* Increment interrupt override index */
                    IrqOverrideCount++;
                }
            }

            /* Ensure, maximum number of I/O APIC controllers has not been reached */
            if(ControllerCount >= IOAPIC_MAX_CONTROLLERS)
            {
                /* No more I/O APIC controllers supported, break loop */
                break;
            }

            /* Go to the next subtable */
            MadtTable += SubTable->Length;
        }
    }

    /* Check if any I/O APIC controllers were found */
    if(ControllerCount == 0)
    {
        /* No I/O APIC controllers found, return failure */
        return STATUS_NOT_FOUND;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Gets the local APIC ID of the current processor.
 *
 * @return This routine returns the current processor's local APIC ID.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
HL::Pic::GetCpuApicId(VOID)
{
    ULONG ApicId;

    /* Read APIC ID register */
    ApicId = ReadApicRegister(APIC_ID);

    /* Return logical CPU ID depending on current APIC mode */
    return (ApicMode == APIC_MODE_COMPAT) ? ((ApicId & 0xFFFFFFFF) >> APIC_XAPIC_LDR_SHIFT) : ApicId;
}

/**
 * Gets the I/O APIC controller information for the specified GSI.
 *
 * @param Gsi
 *        Supplies the GSI to get the I/O APIC controller information for.
 *
 * @param Controller
 *        Supplies a pointer to the memory area where the I/O APIC controller information will be stored.
 *
 * @param EntryNumber
 *        Supplies a pointer to the memory area where the entry number will be stored.
 *
 * @return This routine returns the status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Pic::GetIoApicController(IN ULONG Gsi,
                             OUT PIOAPIC_DATA *Controller,
                             OUT PULONG EntryNumber)
{
    ULONG ControllerIndex;

    /* Iterate over all available I/O APIC controllers */
    for(ControllerIndex = 0; ControllerIndex < ControllerCount; ControllerIndex++)
    {
        /* Check if the GSI belongs to this I/O APIC controller */
        if(Gsi >= Controllers[ControllerIndex].GsiBase &&
           Gsi < (Controllers[ControllerIndex].GsiBase + Controllers[ControllerIndex].LineCount))
        {
            /* Return I/O APIC controller information */
            *Controller = &Controllers[ControllerIndex];
            *EntryNumber = (ULONG)(Gsi - Controllers[ControllerIndex].GsiBase);
            return STATUS_SUCCESS;
        }
    }

    /* GSI does not belong to any I/O APIC controller, return failure */
    return STATUS_NOT_FOUND;
}

/**
 * Initializes the APIC interrupt controller.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Pic::InitializeApic(VOID)
{
    APIC_SPURIOUS_REGISTER SpuriousRegister;
    APIC_BASE_REGISTER BaseRegister;
    APIC_LVT_REGISTER LvtRegister;
    ULONG CpuNumber;

    /* Check APIC support */
    if(!CheckApicSupport())
    {
        /* APIC is not supported, raise kernel panic */
        DebugPrint(L"ERROR: Local APIC not present.\n");
        KE::Crash::Panic(0x5D, CPUID_GET_STANDARD1_FEATURES, 0x0, 0x0, CPUID_FEATURES_EDX_APIC);
    }

    /* Determine APIC mode (xAPIC compatibility or x2APIC) */
    if(CheckX2ApicSupport())
    {
        /* Enable x2APIC mode */
        ApicMode = APIC_MODE_X2APIC;
    }
    else
    {
        /* Fall back to xAPIC compatibility mode */
        ApicMode = APIC_MODE_COMPAT;
    }

    /* Get current processor number */
    CpuNumber = KE::Processor::GetCurrentProcessorNumber();

    /* Enable the APIC */
    BaseRegister.LongLong = AR::CpuFunc::ReadModelSpecificRegister(APIC_LAPIC_MSR_BASE);
    BaseRegister.Enable = 1;
    BaseRegister.ExtendedMode = (ApicMode == APIC_MODE_X2APIC);
    BaseRegister.BootStrapProcessor = (CpuNumber == 0) ? 1 : 0;
    AR::CpuFunc::WriteModelSpecificRegister(APIC_LAPIC_MSR_BASE, BaseRegister.LongLong);

    /* Mask all interrupts by raising Task Priority Register (TPR)  */
    WriteApicRegister(APIC_TPR, 0xFF);

    /* Perform initialization specific to xAPIC compatibility mode */
    if(ApicMode == APIC_MODE_COMPAT)
    {
        /* Use Flat Model for destination format (not supported in x2APIC) */
        WriteApicRegister(APIC_DFR, APIC_DF_FLAT);

        /* Set the logical APIC ID for this processor (read-only in x2APIC) */
        WriteApicRegister(APIC_LDR, (1UL << CpuNumber) << 24);
    }

    /* Configure the spurious interrupt vector */
    SpuriousRegister.Long = ReadApicRegister(APIC_SIVR);
    SpuriousRegister.Vector = APIC_VECTOR_SPURIOUS;
    SpuriousRegister.SoftwareEnable = 1;
    SpuriousRegister.CoreChecking = 0;
    WriteApicRegister(APIC_SIVR, SpuriousRegister.Long);

    /* Setup the LVT Error entry to deliver APIC errors on a dedicated vector */
    WriteApicRegister(APIC_ERRLVTR, APIC_VECTOR_ERROR);

    /* Program the APIC timer for periodic mode */
    LvtRegister.Long = 0;
    LvtRegister.Mask = 1;
    LvtRegister.DeliveryMode = APIC_DM_FIXED;
    LvtRegister.TimerMode = 1;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Vector = APIC_VECTOR_PROFILE;
    WriteApicRegister(APIC_TMRLVTR, LvtRegister.Long);

    /* Configure the performance counter overflow */
    LvtRegister.Long = 0;
    LvtRegister.Mask = 0;
    LvtRegister.DeliveryMode = APIC_DM_FIXED;
    LvtRegister.TimerMode = 0;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Vector = APIC_VECTOR_PERF;
    WriteApicRegister(APIC_PCLVTR, LvtRegister.Long);

    /* Configure the LINT0 pin */
    LvtRegister.Long = 0;
    LvtRegister.Mask = 1;
    LvtRegister.DeliveryMode = APIC_DM_FIXED;
    LvtRegister.TimerMode = 0;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Vector = APIC_VECTOR_SPURIOUS;
    WriteApicRegister(APIC_LINT0, LvtRegister.Long);

    /* Configure the LINT1 pin */
    LvtRegister.Long = 0;
    LvtRegister.Mask = 0;
    LvtRegister.DeliveryMode = APIC_DM_NMI;
    LvtRegister.TimerMode = 0;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Vector = APIC_VECTOR_NMI;
    WriteApicRegister(APIC_LINT1, LvtRegister.Long);

    /* Register interrupt handlers */
    HL::Irq::RegisterInterruptHandler(APIC_VECTOR_SPURIOUS, (PVOID)ArHandleSpuriousInterrupt);
    HL::Irq::RegisterSystemInterruptHandler(APIC_VECTOR_PROFILE, HL::Irq::HandleProfileInterrupt);

    /* Clear any pre-existing errors */
    WriteApicRegister(APIC_ESR, 0);

    /* Re-enable all interrupts by lowering the Task Priority Register */
    WriteApicRegister(APIC_TPR, 0x00);
}

/**
 * Initializes the global I/O APIC controller setup over the entire redirection span.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Pic::InitializeIOApic(VOID)
{
    ULONG ControllerIndex, LineIndex, Vector, VersionRegister;
    IOAPIC_REDIRECTION_REGISTER Register;
    XTSTATUS Status;

    /* Detect I/O APIC controllers */
    Status = DetectIoApicControllers();
    if(Status != STATUS_SUCCESS)
    {
        DebugPrint(L"ERROR: I/O APIC Controller not present.\n");
        KE::Crash::Panic(0x5D, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
    }

    /* Iterate over all I/O APIC controllers */
    ControllerIndex = 0;
    while(ControllerIndex < ControllerCount)
    {
        /* Map the I/O APIC controller memory into hardware space */
        MM::HardwarePool::MapHardwareMemory(Controllers[ControllerIndex].PhysicalAddress,
                                            1,
                                            FALSE,
                                            (PVOID*)&Controllers[ControllerIndex].VirtualAddress);

        /* Perform a memory barrier */
        AR::CpuFunc::MemoryBarrier();
        AR::CpuFunc::ReadWriteBarrier();

        /* Read the version register and calculate the maximum number of redirection entries */
        VersionRegister = ReadIOApicRegister(&Controllers[ControllerIndex], IOAPIC_VER);
        Controllers[ControllerIndex].LineCount = ((VersionRegister >> 16) & 0xFF) + 1;

        /* Set up the default redirection entry for this controller */
        Register.DeliveryMode = APIC_DM_FIXED;
        Register.DeliveryStatus = 0;
        Register.Destination = ReadIOApicRegister(&Controllers[ControllerIndex], IOAPIC_ID) >> 24;
        Register.DestinationMode = 0;
        Register.Mask = 1;
        Register.PinPolarity = 0;
        Register.RemoteIRR = 0;
        Register.Reserved = 0;
        Register.TriggerMode = APIC_TGM_EDGE;
        Register.Vector = IOAPIC_VECTOR_FREE;

        /* Propagate defaults across the array of potential handlers */
        for(LineIndex = 0; LineIndex < Controllers[ControllerIndex].LineCount; LineIndex++)
        {
            /* Write default values into the redirection table */
            WriteRedirectionEntry(&Controllers[ControllerIndex], LineIndex, Register);
        }

        /* Print information about the I/O APIC controller */
        DebugPrint(L"Initialized I/O APIC Controller #%lu at 0x%llX (ID: %lu, GSI Base: %lu, Line Count: %lu)\n",
                   ControllerIndex, Controllers[ControllerIndex].VirtualAddress,
                   Controllers[ControllerIndex].Identifier, Controllers[ControllerIndex].GsiBase,
                   Controllers[ControllerIndex].LineCount);

        /* Go to the next I/O APIC controller */
        ControllerIndex++;
    }

    /* Assign initial clean state for mapping translations */
    for(Vector = 0; Vector <= 255; Vector++)
    {
        /* Set vector to free */
        MappedVectors[Vector] = IOAPIC_VECTOR_FREE;
    }
}

/**
 * Initializes the legacy PIC interrupt controller.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Pic::InitializeLegacyPic(VOID)
{
    PIC_I8259_ICW1 Icw1;
    PIC_I8259_ICW2 Icw2;
    PIC_I8259_ICW3 Icw3;
    PIC_I8259_ICW4 Icw4;

    /* Initialize ICW1 for PIC1 port */
    Icw1.Init = TRUE;
    Icw1.InterruptMode = EdgeTriggered;
    Icw1.InterruptVectorAddress = 0;
    Icw1.Interval = Interval8;
    Icw1.NeedIcw4 = TRUE;
    Icw1.OperatingMode = Cascade;
    HL::IoPort::WritePort8(PIC1_CONTROL_PORT, Icw1.Bits);

    /* Initialize ICW2 for PIC1 port */
    Icw2.Bits = 0x00;
    HL::IoPort::WritePort8(PIC1_DATA_PORT, Icw2.Bits);

    /* Initialize ICW3 for PIC1 port */
    Icw3.Bits = 0;
    Icw3.SlaveIrq2 = TRUE;
    HL::IoPort::WritePort8(PIC1_DATA_PORT, Icw3.Bits);

    /* Initialize ICW4 for PIC1 port */
    Icw4.BufferedMode = NonBuffered;
    Icw4.EoiMode = NormalEoi;
    Icw4.Reserved = 0;
    Icw4.SpecialFullyNestedMode = FALSE;
    Icw4.SystemMode = New8086Mode;
    HL::IoPort::WritePort8(PIC1_DATA_PORT, Icw4.Bits);

    /* Mask all interrupts on PIC1 port */
    HL::IoPort::WritePort8(PIC1_DATA_PORT, 0xFF);

    /* Initialize ICW1 for PIC2 port */
    Icw1.Init = TRUE;
    Icw1.InterruptMode = EdgeTriggered;
    Icw1.InterruptVectorAddress = 0;
    Icw1.Interval = Interval8;
    Icw1.NeedIcw4 = TRUE;
    Icw1.OperatingMode = Cascade;
    HL::IoPort::WritePort8(PIC2_CONTROL_PORT, Icw1.Bits);

    /* Initialize ICW2 for PIC2 port */
    Icw2.Bits = 0x08;
    HL::IoPort::WritePort8(PIC2_DATA_PORT, Icw2.Bits);

    /* Initialize ICW3 for PIC2 port */
    Icw3.Bits = 0;
    Icw3.SlaveId = 2;
    HL::IoPort::WritePort8(PIC2_DATA_PORT, Icw3.Bits);

    /* Initialize ICW4 for PIC2 port */
    Icw4.BufferedMode = NonBuffered;
    Icw4.EoiMode = NormalEoi;
    Icw4.Reserved = 0;
    Icw4.SpecialFullyNestedMode = FALSE;
    Icw4.SystemMode = New8086Mode;
    HL::IoPort::WritePort8(PIC2_DATA_PORT, Icw4.Bits);

    /* Mask all interrupts on PIC2 port */
    HL::IoPort::WritePort8(PIC2_DATA_PORT, 0xFF);

    /* Register interrupt handler */
    HL::Irq::RegisterInterruptHandler(PIC1_VECTOR_SPURIOUS, (PVOID)ArHandleSpuriousInterrupt);
}

/**
 * Initializes the (A)PIC interrupt controller.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @todo Initialize APIC only when supported, otherwise fall back to legacy PIC.
 */
XTAPI
VOID
HL::Pic::InitializePic(VOID)
{
    /* Initialize APIC */
    InitializeApic();

    /* Initialize legacy PIC */
    InitializeLegacyPic();
}

/**
 * Reads from the APIC register.
 *
 * @param Register
 *        Supplies the APIC register to read from.
 *
 * @return This routine returns the value read from the APIC register.
 *
 * @since XT 1.0
 */
XTFASTCALL
ULONGLONG
HL::Pic::ReadApicRegister(IN APIC_REGISTER Register)
{
    if(ApicMode == APIC_MODE_X2APIC)
    {
        /* Read from x2APIC MSR */
        return AR::CpuFunc::ReadModelSpecificRegister((ULONG)(APIC_X2APIC_MSR_BASE + Register));
    }
    else
    {
        /* Read from xAPIC */
        return HL::IoRegister::ReadRegister32((PULONG)(APIC_BASE + (Register << 4)));
    }
}

/**
 * Reads from the I/O APIC register.
 *
 * @param Controller
 *        Supplies the I/O APIC controller to read from.
 *
 * @param Register
 *        Supplies the I/O APIC register to read from.
 *
 * @return This routine returns the value read from the given IO APIC register.
 *
 * @since XT 1.0
 */
XTFASTCALL
ULONG
HL::Pic::ReadIOApicRegister(IN PIOAPIC_DATA Controller,
                            IN UCHAR Register)
{
    /* Write the target address into the index register */
    HL::IoRegister::WriteRegister32((PULONG)(Controller->VirtualAddress + IOAPIC_IOREGSEL), Register);

    /* Fetch the resultant value from the data window */
    return HL::IoRegister::ReadRegister32((PULONG)(Controller->VirtualAddress + IOAPIC_IOWIN));
}

/**
 * Reads a configuration entry from the I/O APIC redirection table.
 *
 * @param Controller
 *        Supplies the I/O APIC controller to read from.
 *
 * @param EntryNumber
 *        Supplies the redirection table entry number to read.
 *
 * @return This routine returns the populated redirection table entry.
 *
 * @since XT 1.0
 */
XTFASTCALL
IOAPIC_REDIRECTION_REGISTER
HL::Pic::ReadRedirectionEntry(IN PIOAPIC_DATA Controller,
                              IN ULONG EntryNumber)
{
    IOAPIC_REDIRECTION_REGISTER Register;
    ULONG Offset;

    /* Derive the offset corresponding to the index */
    Offset = IOAPIC_REDTBL + (EntryNumber * IOAPIC_RTE_SIZE);

    /* Read the low and high portions mapping to the 64-bit construct */
    Register.Base = ReadIOApicRegister(Controller, Offset);
    Register.Extended = ReadIOApicRegister(Controller, Offset + 1);

    /* Return the redirection table entry */
    return Register;
}

/**
 * Resolves the GSI and flags for the specified IRQ.
 *
 * @param Irq
 *        Supplies the IRQ number to get the GSI and flags for.
 *
 * @param Gsi
 *        Supplies a pointer to the memory area where the GSI will be stored.
 *
 * @param Flags
 *        Supplies a pointer to the memory area where the flags will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Pic::ResolveInterruptOverride(IN UCHAR Irq,
                                  OUT PULONG Gsi,
                                  OUT PUSHORT Flags)
{
    ULONG Index;

    /* Iterate over all I/O APIC overrides */
    for(Index = 0; Index < IrqOverrideCount; Index++)
    {
        /* Check if this IRQ has been overridden */
        if(IrqOverrides[Index].SourceIrq == Irq)
        {
            /* Return overridden GSI and flags */
            *Flags = IrqOverrides[Index].Flags;
            *Gsi = IrqOverrides[Index].GlobalSystemInterrupt;
        }
    }

    /* Return original IRQ number as GSI and no flags */
    *Flags = 0;
    *Gsi = (ULONG)Irq;
}

/**
 * Signals to the APIC that handling an interrupt is complete.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Pic::SendEoi(VOID)
{
    /* Send APIC EOI */
    WriteApicRegister(APIC_EOI, 0);
}

/**
 * Sends an IPI (Inter-Processor Interrupt) to the specified CPU.
 *
 * @param ApicId
 *        Supplies a CPU APIC ID to send an IPI to.
 *
 * @param Vector
 *        Supplies the IPI vector to send.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Pic::SendIpi(IN ULONG ApicId,
                 IN ULONG Vector)
{
    /* Check current APIC mode */
    if(ApicMode == APIC_MODE_X2APIC)
    {
        /* Send IPI using x2APIC mode */
        WriteApicRegister(APIC_ICR0, ((ULONGLONG)ApicId << 32) | Vector);
    }
    else
    {
        /* Send IPI using xAPIC compatibility mode */
        WriteApicRegister(APIC_ICR1, ApicId << 24);
        WriteApicRegister(APIC_ICR0, Vector);
    }
}

/**
 * Sends a Self-IPI (Inter-Processor Interrupt) to the current CPU.
 *
 * @param Vector
 *        Supplies the IPI vector to send.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Pic::SendSelfIpi(IN ULONG Vector)
{
    BOOLEAN Interrupts;

    /* Check whether interrupts are enabled */
    Interrupts = AR::CpuFunc::InterruptsEnabled();

    /* Disable interrupts */
    AR::CpuFunc::ClearInterruptFlag();

    /* Check current APIC mode */
    if(ApicMode == APIC_MODE_X2APIC)
    {
        /* In x2APIC mode, a dedicated Self-IPI register is used */
        WriteApicRegister(APIC_SIPI, Vector);
    }
    else
    {
        /* Wait for the APIC to clear the delivery status */
        while((ReadApicRegister(APIC_ICR0) & 0x1000) != 0)
        {
            /* Yield the processor */
            AR::CpuFunc::YieldProcessor();
        }

        /* In xAPIC compatibility mode, ICR0 is used */
        WriteApicRegister(APIC_ICR0, Vector | (1 << 18));
    }

    /* Wait for the APIC to complete delivery of the IPI */
    while((ReadApicRegister(APIC_ICR0) & 0x1000) != 0)
    {
        /* Yield the processor */
        AR::CpuFunc::YieldProcessor();
    }

    /* Check whether interrupts need to be re-enabled */
    if(Interrupts)
    {
        /* Re-enable interrupts */
        AR::CpuFunc::SetInterruptFlag();
    }
}

/**
 * Translates a given Global System Interrupt (GSI) into an active system interrupt vector.
 *
 * @param Gsi
 *        Supplies the GSI to translate.
 *
 * @return This routine returns the underlying associated system vector mapping.
 *
 * @since XT 1.0
 */
XTFASTCALL
UCHAR
HL::Pic::TranslateGsiToVector(IN ULONG Gsi)
{
    IOAPIC_REDIRECTION_REGISTER Register;
    PIOAPIC_DATA Controller;
    ULONG EntryNumber;
    XTSTATUS Status;

    /* Find the APIC controller for the GSI */
    Status = GetIoApicController(Gsi, &Controller, &EntryNumber);
    if(Status != STATUS_SUCCESS)
    {
        /* GSI maps to an invalid controller, return free */
        return IOAPIC_VECTOR_FREE;
    }

    /* Read the redirection table entry */
    Register.Base = ReadIOApicRegister(Controller, IOAPIC_REDTBL + (EntryNumber * IOAPIC_RTE_SIZE));

    /* Return the vector */
    return (UCHAR)Register.Vector;
}

/**
 * Writes to the APIC register.
 *
 * @param Register
 *        Supplies the APIC register to write to.
 *
 * @param Value
 *        Supplies the value to write to the APIC register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
HL::Pic::WriteApicRegister(IN APIC_REGISTER Register,
                           IN ULONGLONG Value)
{
    if(ApicMode == APIC_MODE_X2APIC)
    {
        /* Write to x2APIC MSR */
        AR::CpuFunc::WriteModelSpecificRegister((ULONG)(APIC_X2APIC_MSR_BASE + Register), Value);
    }
    else
    {
        /* Write to xAPIC */
        HL::IoRegister::WriteRegister32((PULONG)(APIC_BASE + (Register << 4)), Value);
    }
}

/**
 * Writes a value to the I/O APIC register.
 *
 * @param Controller
 *        Supplies the I/O APIC controller to write to.
 *
 * @param Register
 *        Supplies the I/O APIC register to write to.
 *
 * @param DataValue
 *        Supplies the value to write to the designated I/O APIC register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
HL::Pic::WriteIOApicRegister(IN PIOAPIC_DATA Controller,
                             IN UCHAR Register,
                             IN ULONG DataValue)
{
    /* Provide the index to the control port */
    HL::IoRegister::WriteRegister32((PULONG)(Controller->VirtualAddress + IOAPIC_IOREGSEL), Register);

    /* Commit the value via the data port */
    HL::IoRegister::WriteRegister32((PULONG)(Controller->VirtualAddress + IOAPIC_IOWIN), DataValue);
}

/**
 * Writes a configuration entry into the I/O APIC redirection table.
 *
 * @param Controller
 *        Supplies the I/O APIC controller to write to.
 *
 * @param EntryNumber
 *        Supplies the redirection table entry number to write.
 *
 * @param EntryData
 *        Supplies the redirection table entry data to write.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
HL::Pic::WriteRedirectionEntry(IN PIOAPIC_DATA Controller,
                               IN ULONG EntryNumber,
                               IN IOAPIC_REDIRECTION_REGISTER EntryData)
{
    ULONG Offset;

    /* Calculate the offset of the redirection entry */
    Offset = IOAPIC_REDTBL + (EntryNumber * IOAPIC_RTE_SIZE);

    /* Mask the entry to prevent spurious interrupts */
    WriteIOApicRegister(Controller, Offset, IOAPIC_RTE_MASKED);

    /* Write the lower and upper chunks of the entry */
    WriteIOApicRegister(Controller, Offset + 1, EntryData.Extended);
    WriteIOApicRegister(Controller, Offset, EntryData.Base);
}
