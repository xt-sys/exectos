/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/pic.c
 * DESCRIPTION:     Programmable Interrupt Controller (PIC) for x86 (i686/AMD64) support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Jozef Nagy <schkwve@gmail.com>
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
 * Disables the legacy 8259 Programmable Interrupt Controller (PIC).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlDisableLegacyPic(VOID)
{
    /* Mask all interrupts */
    HlIoPortOutByte(PIC1_DATA_PORT, 0xFF);
    HlIoPortOutByte(PIC2_DATA_PORT, 0xFF);
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
ULONG
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
                    IN ULONG Value)
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
    CpuRegisters.Leaf = CPUID_GET_CPU_FEATURES;
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
 * @param CpuNumber
 *        Supplies the number of the CPU, that is being initialized.
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

    /* Check if this is an x2APIC compatible machine */
    if(HlpCheckX2ApicSupport())
    {
        /* Enable x2APIC */
        HlpApicMode = APIC_MODE_X2APIC;
    }
    else
    {
        /* Use xAPIC compatibility mode */
        HlpApicMode = APIC_MODE_COMPAT;
    }

    /* Get processor number */
    CpuNumber = KeGetCurrentProcessorNumber();

    /* Enable the APIC */
    BaseRegister.LongLong = ArReadModelSpecificRegister(APIC_LAPIC_MSR_BASE);
    BaseRegister.Enable = 1;
    BaseRegister.ExtendedMode = (HlpApicMode == APIC_MODE_X2APIC);
    BaseRegister.BootStrapProcessor = (CpuNumber == 0) ? 1 : 0;
    ArWriteModelSpecificRegister(APIC_LAPIC_MSR_BASE, BaseRegister.LongLong);

    /* Raise APIC task priority (TPR) to mask off all interrupts */
    HlWriteApicRegister(APIC_TPR, 0xFF);

    /* xAPIC compatibility mode specific initialization */
    if(HlpApicMode == APIC_MODE_COMPAT)
    {
        /* Initialize Destination Format Register with flat model */
        HlWriteApicRegister(APIC_DFR, APIC_DF_FLAT);

        /* Set the logical APIC ID */
        HlWriteApicRegister(APIC_LDR, (1UL << CpuNumber) << 24);
    }

    /* Set the spurious interrupt vector */
    SpuriousRegister.Long = HlReadApicRegister(APIC_SIVR);
    SpuriousRegister.Vector = APIC_VECTOR_SPURIOUS;
    SpuriousRegister.SoftwareEnable = 1;
    SpuriousRegister.CoreChecking = 0;
    HlWriteApicRegister(APIC_SIVR, SpuriousRegister.Long);

    /* Initialize Logical Vector Table */
    LvtRegister.Long = 0;
    LvtRegister.MessageType = APIC_DM_FIXED;
    LvtRegister.DeliveryStatus = 0;
    LvtRegister.RemoteIRR = 0;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Mask = 0;
    LvtRegister.TimerMode = 0;

    /* Mask LVTR_ERROR first, to prevent local APIC error */
    LvtRegister.Vector = APIC_VECTOR_ERROR;
    HlWriteApicRegister(APIC_ERRLVTR, LvtRegister.Long);

    /* Mask LVT tables */
    LvtRegister.Vector = APIC_VECTOR_NMI;
    HlWriteApicRegister(APIC_TMRLVTR, LvtRegister.Long);
    HlWriteApicRegister(APIC_THRMLVTR, LvtRegister.Long);
    HlWriteApicRegister(APIC_PCLVTR, LvtRegister.Long);

    /* Mask LINT0 */
    LvtRegister.Vector = APIC_VECTOR_SPURIOUS;
    LvtRegister.MessageType = APIC_DM_EXTINT;
    HlWriteApicRegister(APIC_LINT0, LvtRegister.Long);

    /* Mask LINT1 */
    LvtRegister.Vector = APIC_VECTOR_NMI;
    LvtRegister.MessageType = APIC_DM_NMI;
    LvtRegister.TriggerMode = APIC_TGM_LEVEL;
    HlWriteApicRegister(APIC_LINT1, LvtRegister.Long);

    /* Mask ICR0 */
    CommandRegister.Long0 = 0;
    CommandRegister.Vector = APIC_VECTOR_ZERO;
    CommandRegister.MessageType = APIC_MT_INIT;
    CommandRegister.TriggerMode = APIC_TGM_LEVEL;
    CommandRegister.DestinationShortHand = APIC_DSH_AllIncludingSelf;
    HlWriteApicRegister(APIC_ICR0, CommandRegister.Long0);

    /* Clear errors after enabling vectors */
    HlWriteApicRegister(APIC_ESR, 0);

    /* Register interrupt handlers once the APIC initialization is done */
    KeSetInterruptHandler(APIC_VECTOR_SPURIOUS, HlpHandleApicSpuriousService);
    KeSetInterruptHandler(PIC1_VECTOR_SPURIOUS, HlpHandlePicSpuriousService);

    /* Lower APIC TPR to re-enable interrupts */
    HlWriteApicRegister(APIC_TPR, 0x00);
}

/**
 * Initializes the (A)PIC interrupt controller.
 *
 * @param CpuNumber
 *        Supplies the number of the CPU, that is being initialized.
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
    /* Disable legacy PIC and initialize APIC */
    HlDisableLegacyPic();
    HlpInitializeApic();
}
