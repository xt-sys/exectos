/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/pic.c
 * DESCRIPTION:     Programmable Interrupt Controller (PIC) support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
HlpHandleApicSpuriousService()
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
HlpHandlePicSpuriousService()
{
}

/**
 * Initializes the APIC interrupt controller.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 *
 * @todo Register interrupt handlers for spurious vectors.
 */
XTAPI
VOID
HlpInitializeApic()
{
    APIC_BASE_REGISTER BaseRegister;
    APIC_LVT_REGISTER LvtRegister;
    APIC_SPURIOUS_REGISTER SpuriousRegister;
    ULONG CpuNumber = 0;

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

    /* Enable the APIC */
    BaseRegister.LongLong = ArReadModelSpecificRegister(APIC_LAPIC_MSR_BASE);
    BaseRegister.Enable = 1;
    BaseRegister.ExtendedMode = (HlpApicMode == APIC_MODE_X2APIC);
    BaseRegister.BootStrapProcessor = 1;
    ArWriteModelSpecificRegister(APIC_LAPIC_MSR_BASE, BaseRegister.LongLong);

    /* xAPIC compatibility mode specific initialization */
    if(HlpApicMode == APIC_MODE_COMPAT)
    {
        /* Initialize Destination Format Register with flat model */
        HlWriteApicRegister(APIC_DFR, APIC_DF_FLAT);

        /* Set the logical APIC ID */
        HlWriteApicRegister(APIC_LDR, (1UL << CpuNumber) << 24);
    }

    /* Set the spurious interrupt vector and register interrupt handlers */
    SpuriousRegister.Long = HlReadApicRegister(APIC_SIVR);
    SpuriousRegister.Vector = APIC_VECTOR_SPURIOUS;
    SpuriousRegister.SoftwareEnable = 1;
    SpuriousRegister.CoreChecking = 0;
    HlWriteApicRegister(APIC_SIVR, SpuriousRegister.Long);

    /* Initialize Logical Vector Table */
    LvtRegister.Long = 0;
    LvtRegister.Vector = APIC_VECTOR_NMI;
    LvtRegister.MessageType = APIC_DM_FIXED;
    LvtRegister.DeliveryStatus = 0;
    LvtRegister.RemoteIRR = 0;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Mask = 0;
    LvtRegister.TimerMode = 0;

    /* Mask LVT tables */
    HlWriteApicRegister(APIC_TMRLVTR, LvtRegister.Long);
    HlWriteApicRegister(APIC_THRMLVTR, LvtRegister.Long);
    HlWriteApicRegister(APIC_PCLVTR, LvtRegister.Long);

    /* Mask LINT0 */
    LvtRegister.Vector = APIC_VECTOR_SPURIOUS;
    LvtRegister.MessageType = APIC_DM_EXTINT;
    HlWriteApicRegister(APIC_LINT0, LvtRegister.Long);

    /* Mask LINT1 */
    LvtRegister.Mask = 0;
    LvtRegister.Vector = APIC_VECTOR_NMI;
    LvtRegister.MessageType = APIC_DM_NMI;
    LvtRegister.TriggerMode = APIC_TGM_LEVEL;
    HlWriteApicRegister(APIC_LINT1, LvtRegister.Long);

    /* Mask LVTR_ERROR */
    LvtRegister.Vector = APIC_VECTOR_ERROR;
    LvtRegister.MessageType = APIC_DM_FIXED;
    HlWriteApicRegister(APIC_ERRLVTR, LvtRegister.Long);

    /* Clear errors after enabling vectors */
    HlWriteApicRegister(APIC_ESR, 0);
}
