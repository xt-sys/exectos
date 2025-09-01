/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/pic.c
 * DESCRIPTION:     Programmable Interrupt Controller (PIC) for x86 (i686/AMD64) support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Clears all errors on the APIC.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlClearApicErrors(VOID)
{
    /* Clear APIC errors */
    HlWriteApicRegister(APIC_ESR, 0);
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
HlReadApicRegister(IN APIC_REGISTER Register)
{
    if(HlpApicMode == APIC_MODE_X2APIC)
    {
        /* Read from x2APIC MSR */
        return ArReadModelSpecificRegister((ULONG)(APIC_X2APIC_MSR_BASE + Register));
    }
    else
    {
        /* Read from xAPIC */
        return RtlReadRegisterLong((PULONG)(APIC_BASE + (Register << 4)));
    }
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
HlSendEoi(VOID)
{
    /* Send APIC EOI */
    HlWriteApicRegister(APIC_EOI, 0);
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
HlWriteApicRegister(IN APIC_REGISTER Register,
                    IN ULONGLONG Value)
{
    if(HlpApicMode == APIC_MODE_X2APIC)
    {
        /* Write to x2APIC MSR */
        ArWriteModelSpecificRegister((ULONG)(APIC_X2APIC_MSR_BASE + Register), Value);
    }
    else
    {
        /* Write to xAPIC */
        RtlWriteRegisterLong((PULONG)(APIC_BASE + (Register << 4)), Value);
    }
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
HlpCheckX2ApicSupport(VOID)
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
    ArCpuId(&CpuRegisters);

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
 * Gets the local APIC ID of the current processor.
 *
 * @return This routine returns the current processor's local APIC ID.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
HlpGetCpuApicId(VOID)
{
    ULONG ApicId;

    /* Read APIC ID register */
    ApicId = HlReadApicRegister(APIC_ID);

    /* Return logical CPU ID depending on current APIC mode */
    return (HlpApicMode == APIC_MODE_COMPAT) ? ((ApicId & 0xFFFFFFFF) >> APIC_XAPIC_LDR_SHIFT) : ApicId;
}

/**
 * Allows an APIC spurious interrupts to end up.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
HlpHandleApicSpuriousService(VOID)
{
}

/**
 * Allows a PIC spurious interrupts to end up.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
HlpHandlePicSpuriousService(VOID)
{
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
HlpInitializeApic(VOID)
{
    APIC_SPURIOUS_REGISTER SpuriousRegister;
    APIC_COMMAND_REGISTER CommandRegister;
    APIC_BASE_REGISTER BaseRegister;
    APIC_LVT_REGISTER LvtRegister;
    ULONG CpuNumber;

    /* Determine APIC mode (xAPIC compatibility or x2APIC) */
    if(HlpCheckX2ApicSupport())
    {
        /* Enable x2APIC mode */
        HlpApicMode = APIC_MODE_X2APIC;
    }
    else
    {
        /* Fall back to xAPIC compatibility mode */
        HlpApicMode = APIC_MODE_COMPAT;
    }

    /* Get current processor number */
    CpuNumber = KeGetCurrentProcessorNumber();

    /* Enable the APIC */
    BaseRegister.LongLong = ArReadModelSpecificRegister(APIC_LAPIC_MSR_BASE);
    BaseRegister.Enable = 1;
    BaseRegister.ExtendedMode = (HlpApicMode == APIC_MODE_X2APIC);
    BaseRegister.BootStrapProcessor = (CpuNumber == 0) ? 1 : 0;
    ArWriteModelSpecificRegister(APIC_LAPIC_MSR_BASE, BaseRegister.LongLong);

    /* Mask all interrupts by raising Task Priority Register (TPR)  */
    HlWriteApicRegister(APIC_TPR, 0xFF);

    /* Perform initialization specific to xAPIC compatibility mode */
    if(HlpApicMode == APIC_MODE_COMPAT)
    {
        /* Use Flat Model for destination format (not supported in x2APIC) */
        HlWriteApicRegister(APIC_DFR, APIC_DF_FLAT);

        /* Set the logical APIC ID for this processor (read-only in x2APIC) */
        HlWriteApicRegister(APIC_LDR, (1UL << CpuNumber) << 24);
    }

    /* Configure the spurious interrupt vector */
    SpuriousRegister.Long = HlReadApicRegister(APIC_SIVR);
    SpuriousRegister.Vector = APIC_VECTOR_SPURIOUS;
    SpuriousRegister.SoftwareEnable = 1;
    SpuriousRegister.CoreChecking = 0;
    HlWriteApicRegister(APIC_SIVR, SpuriousRegister.Long);

    /* Setup the LVT Error entry to deliver APIC errors on a dedicated vector */
    HlWriteApicRegister(APIC_ERRLVTR, APIC_VECTOR_ERROR);

    /* Program the APIC timer for periodic mode */
    LvtRegister.Long = 0;
    LvtRegister.Mask = 1;
    LvtRegister.MessageType = APIC_DM_FIXED;
    LvtRegister.TimerMode = 1;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Vector = APIC_VECTOR_PROFILE;
    HlWriteApicRegister(APIC_TMRLVTR, LvtRegister.Long);

    /* Configure the performance counter overflow */
    LvtRegister.Long = 0;
    LvtRegister.Mask = 0;
    LvtRegister.MessageType = APIC_DM_FIXED;
    LvtRegister.TimerMode = 0;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Vector = APIC_VECTOR_PERF;
    HlWriteApicRegister(APIC_PCLVTR, LvtRegister.Long);

    /* Configure the LINT0 pin */
    LvtRegister.Long = 0;
    LvtRegister.Mask = 1;
    LvtRegister.MessageType = APIC_DM_FIXED;
    LvtRegister.TimerMode = 0;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Vector = APIC_VECTOR_SPURIOUS;
    HlWriteApicRegister(APIC_LINT0, LvtRegister.Long);

    /* Configure the LINT1 pin */
    LvtRegister.Long = 0;
    LvtRegister.Mask = 0;
    LvtRegister.MessageType = APIC_DM_NMI;
    LvtRegister.TimerMode = 0;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Vector = APIC_VECTOR_NMI;
    HlWriteApicRegister(APIC_LINT1, LvtRegister.Long);

    /* Register interrupt handlers */
    KeSetInterruptHandler(APIC_VECTOR_SPURIOUS, HlpHandleApicSpuriousService);
    KeSetInterruptHandler(PIC1_VECTOR_SPURIOUS, HlpHandlePicSpuriousService);

    /* Clear any pre-existing errors */
    HlWriteApicRegister(APIC_ESR, 0);

    /* Re-enable all interrupts by lowering the Task Priority Register */
    HlWriteApicRegister(APIC_TPR, 0x00);
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
HlpInitializeLegacyPic(VOID)
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
    HlIoPortOutByte(PIC1_CONTROL_PORT, Icw1.Bits);

    /* Initialize ICW2 for PIC1 port */
    Icw2.Bits = 0x00;
    HlIoPortOutByte(PIC1_DATA_PORT, Icw2.Bits);

    /* Initialize ICW3 for PIC1 port */
    Icw3.Bits = 0;
    Icw3.SlaveIrq2 = TRUE;
    HlIoPortOutByte(PIC1_DATA_PORT, Icw3.Bits);

    /* Initialize ICW4 for PIC1 port */
    Icw4.BufferedMode = NonBuffered;
    Icw4.EoiMode = NormalEoi;
    Icw4.Reserved = 0;
    Icw4.SpecialFullyNestedMode = FALSE;
    Icw4.SystemMode = New8086Mode;
    HlIoPortOutByte(PIC1_DATA_PORT, Icw4.Bits);

    /* Mask all interrupts on PIC1 port */
    HlIoPortOutByte(PIC1_DATA_PORT, 0xFF);

    /* Initialize ICW1 for PIC2 port */
    Icw1.Init = TRUE;
    Icw1.InterruptMode = EdgeTriggered;
    Icw1.InterruptVectorAddress = 0;
    Icw1.Interval = Interval8;
    Icw1.NeedIcw4 = TRUE;
    Icw1.OperatingMode = Cascade;
    HlIoPortOutByte(PIC2_CONTROL_PORT, Icw1.Bits);

    /* Initialize ICW2 for PIC2 port */
    Icw2.Bits = 0x08;
    HlIoPortOutByte(PIC2_DATA_PORT, Icw2.Bits);

    /* Initialize ICW3 for PIC2 port */
    Icw3.Bits = 0;
    Icw3.SlaveId = 2;
    HlIoPortOutByte(PIC2_DATA_PORT, Icw3.Bits);

    /* Initialize ICW4 for PIC2 port */
    Icw4.BufferedMode = NonBuffered;
    Icw4.EoiMode = NormalEoi;
    Icw4.Reserved = 0;
    Icw4.SpecialFullyNestedMode = FALSE;
    Icw4.SystemMode = New8086Mode;
    HlIoPortOutByte(PIC2_DATA_PORT, Icw4.Bits);

    /* Mask all interrupts on PIC2 port */
    HlIoPortOutByte(PIC2_DATA_PORT, 0xFF);
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
HlpInitializePic(VOID)
{
    /* Initialize APIC */
    HlpInitializeApic();

    /* Initialize legacy PIC */
    HlpInitializeLegacyPic();
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
HlpSendIpi(ULONG ApicId,
           ULONG Vector)
{
    /* Check current APIC mode */
    if(HlpApicMode == APIC_MODE_X2APIC)
    {
        /* Send IPI using x2APIC mode */
        HlWriteApicRegister(APIC_ICR0, ((ULONGLONG)ApicId << 32) | Vector);
    }
    else
    {
        /* Send IPI using xAPIC compatibility mode */
        HlWriteApicRegister(APIC_ICR1, ApicId << 24);
        HlWriteApicRegister(APIC_ICR0, Vector);
    }
}
