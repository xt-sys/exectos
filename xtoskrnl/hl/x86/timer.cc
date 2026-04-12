/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/timer.cc
 * DESCRIPTION:     APIC Timer support for x86 (i686/AMD64) support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Calibrates the Local APIC timer frequency.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Timer::CalibrateApicTimer()
{
    ULONG CurrentCount, Frequency, InitialCount;

    /* Get APIC timer frequency from the Core Crystal Clock */
    if(TimerCapabilities.Art && TimerCapabilities.TimerFrequency != 0)
    {
        /* CCC available, use it as the source of APIC timer frequency */
        Frequency = TimerCapabilities.TimerFrequency;
    }
    else
    {
        /* CCC unavailable, fallback to PIT calibration */
        InitialCount = 0xFFFFFFFF;

        /* Load the initial count into the APIC Timer and begin the countdown */
        HL::Pic::WriteApicRegister(APIC_TICR, InitialCount);

        /* Wait for 10ms */
        StallExecution(10000);

        /* Read current tick count from APIC timer and clear APIC timer */
        CurrentCount = HL::Pic::ReadApicRegister(APIC_TCCR);
        HL::Pic::WriteApicRegister(APIC_TICR, 0);

        /* Calculate APIC timer frequency based on ticks passed */
        Frequency = (InitialCount - CurrentCount) * 100;

        /* Verify APIC timer frequency */
        if(Frequency == 0)
        {
            /* Unable to calibrate APIC timer, return error */
            return STATUS_UNSUCCESSFUL;
        }
    }

    /* Save APIC timer frequency */
    TimerFrequency = Frequency;

    /* Print APIC timer frequency and return success */
    DebugPrint(L"APIC Timer calibrated: %u Ticks/s\n", TimerFrequency);
    return STATUS_SUCCESS;
}

/**
 * Initializes and calibrates the Local APIC Timer.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Timer::InitializeApicTimer(VOID)
{
    XTSTATUS Status;

    /* Set APIC timer to divide by 1 */
    HL::Pic::WriteApicRegister(APIC_TDCR, TIMER_DivideBy1);

    /* Calibrate the APIC timer */
    Status = CalibrateApicTimer();
    if(Status != STATUS_SUCCESS)
    {
        /* System cannot operate without a calibrated system timer, raise kernel panic */
        KE::Crash::Panic(0);
    }

    /* Set the default system profile interval */
    HL::Timer::SetProfileInterval(1000);

    /* Program the APIC timer for periodic mode */
    StopProfileInterrupt(ProfileXtKernel);
}

/**
 * Performs a basic Timer initialization.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Timer::InitializeTimer(VOID)
{
    /* Query timer capabilities */
    QueryTimerCapabilities();

    /* Initialize the APIC timer */
    InitializeApicTimer();
}

/**
 * Stalls the CPU execution for a specified duration (maximum 3 seconds) using the legacy PIT timer.
 *
 * @param MicroSeconds
 *        Specifies the number of microseconds to stall execution.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Timer::PitStallExecution(IN ULONG MicroSeconds)
{
    USHORT CurrentCount, PreviousCount;
    ULONG TargetTicks, TickCounter;

    /* Validate input parameter */
    if(MicroSeconds == 0)
    {
        /* Nothing to do */
        return;
    }
    else if(MicroSeconds > 3000000)
    {
        /* Cap execution stall to 3 seconds */
        MicroSeconds = 3000000;
    }

    /* Convert us to PIT ticks and initialize tick counter */
    TargetTicks = (MicroSeconds * 1193) / 1000;
    TickCounter = 0;

    /* Configure PIT Channel 0: Read/Write LSB then MSB, Mode 0 (Interrupt on Terminal Count) */
    HL::IoPort::WritePort8(PIT_COMMAND_PORT, 0x30);

    /* Initialize the PIT counter with the maximum possible value (0xFFFF) */
    HL::IoPort::WritePort8(PIT_DATA_PORT0, 0xFF);
    HL::IoPort::WritePort8(PIT_DATA_PORT0, 0xFF);

    /* Latch and read the initial counter value */
    HL::IoPort::WritePort8(PIT_COMMAND_PORT, 0x00);
    PreviousCount = HL::IoPort::ReadPort8(PIT_DATA_PORT0);
    PreviousCount |= (HL::IoPort::ReadPort8(PIT_DATA_PORT0) << 8);

    /* Poll the PIT */
    while(TickCounter < TargetTicks)
    {
        /* Latch the current counter value without stopping the timer */
        HL::IoPort::WritePort8(PIT_COMMAND_PORT, 0x00);
        CurrentCount = HL::IoPort::ReadPort8(PIT_DATA_PORT0);
        CurrentCount |= (HL::IoPort::ReadPort8(PIT_DATA_PORT0) << 8);

        /* Calculate elapsed ticks since the last read */
        TickCounter += (PreviousCount - CurrentCount) & 0xFFFF;

        /* Update the tracking variable */
        PreviousCount = CurrentCount;
    }
}

/**
 * Probes the processor via CPUID to detect available modern timing and clock generation features.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Timer::QueryTimerCapabilities(VOID)
{
    CPUID_REGISTERS CpuRegisters;
    ULONG MaxStandardLeaf;
    ULONG MaxExtendedLeaf;

    /* Query maximum standard CPUID leaf */
    CpuRegisters.Leaf = CPUID_GET_VENDOR_STRING;
    CpuRegisters.SubLeaf = 0;
    CpuRegisters.Eax = 0;
    CpuRegisters.Ebx = 0;
    CpuRegisters.Ecx = 0;
    CpuRegisters.Edx = 0;
    AR::CpuFunc::CpuId(&CpuRegisters);

    /* Save maximum supported standard CPUID leaf */
    MaxStandardLeaf = CpuRegisters.Eax;

    /* Query maximum extended CPUID leaf */
    CpuRegisters.Leaf = CPUID_GET_EXTENDED_MAX;
    CpuRegisters.SubLeaf = 0;
    CpuRegisters.Eax = 0;
    CpuRegisters.Ebx = 0;
    CpuRegisters.Ecx = 0;
    CpuRegisters.Edx = 0;
    AR::CpuFunc::CpuId(&CpuRegisters);

    /* Save maximum supported extended CPUID leaf */
    MaxExtendedLeaf = CpuRegisters.Eax;

    /* Check TSC-Deadline mode if leaf supported */
    if(MaxStandardLeaf >= CPUID_GET_STANDARD1_FEATURES)
    {
        CpuRegisters.Leaf = CPUID_GET_STANDARD1_FEATURES;
        CpuRegisters.SubLeaf = 0;
        CpuRegisters.Eax = 0;
        CpuRegisters.Ebx = 0;
        CpuRegisters.Ecx = 0;
        CpuRegisters.Edx = 0;
        AR::CpuFunc::CpuId(&CpuRegisters);

        /* Verify TSC-Deadline support */
        if(CpuRegisters.Ecx & CPUID_FEATURES_ECX_TSC_DEADLINE)
        {
            TimerCapabilities.TscDeadline = TRUE;
        }
    }

    /* Check Always Running APIC Timer - ARAT if leaf supported */
    if(MaxStandardLeaf >= CPUID_GET_POWER_MANAGEMENT)
    {
        CpuRegisters.Leaf = CPUID_GET_POWER_MANAGEMENT;
        CpuRegisters.SubLeaf = 0;
        CpuRegisters.Eax = 0;
        CpuRegisters.Ebx = 0;
        CpuRegisters.Ecx = 0;
        CpuRegisters.Edx = 0;
        AR::CpuFunc::CpuId(&CpuRegisters);

        /* Verify ARAT support */
        if(CpuRegisters.Eax & CPUID_FEATURES_EAX_ARAT)
        {
            TimerCapabilities.Arat = TRUE;
        }
    }

    /* Check Always Running Timer - ART if leaf supported */
    if(MaxStandardLeaf >= CPUID_GET_TSC_CRYSTAL_CLOCK)
    {
        CpuRegisters.Leaf = CPUID_GET_TSC_CRYSTAL_CLOCK;
        CpuRegisters.SubLeaf = 0;
        CpuRegisters.Eax = 0;
        CpuRegisters.Ebx = 0;
        CpuRegisters.Ecx = 0;
        CpuRegisters.Edx = 0;
        AR::CpuFunc::CpuId(&CpuRegisters);

        /* Verify ART support */
        if(CpuRegisters.Eax != 0 && CpuRegisters.Ebx != 0)
        {
            TimerCapabilities.Art = TRUE;

            /* Save the TSC scaling ratios */
            TimerCapabilities.TscDenominator = CpuRegisters.Eax;
            TimerCapabilities.TscNumerator = CpuRegisters.Ebx;

            /* Check if ECX contains the nominal frequency of the core crystal clock */
            if(CpuRegisters.Ecx != 0)
            {
                /* Save the base frequency for the APIC Timer */
                TimerCapabilities.TimerFrequency = CpuRegisters.Ecx;
            }
        }
    }

    /* Check RDTSCP instruction support if leaf supported */
    if(MaxExtendedLeaf >= CPUID_GET_EXTENDED_FEATURES)
    {
        CpuRegisters.Leaf = CPUID_GET_EXTENDED_FEATURES;
        CpuRegisters.SubLeaf = 0;
        CpuRegisters.Eax = 0;
        CpuRegisters.Ebx = 0;
        CpuRegisters.Ecx = 0;
        CpuRegisters.Edx = 0;
        AR::CpuFunc::CpuId(&CpuRegisters);

        /* Verify RDTSCP support */
        if(CpuRegisters.Edx & CPUID_FEATURES_EDX_RDTSCP)
        {
            TimerCapabilities.RDTSCP = TRUE;
        }
    }

    /* Check Invariant TSC if leaf supported */
    if(MaxExtendedLeaf >= CPUID_GET_ADVANCED_POWER_MANAGEMENT)
    {
        CpuRegisters.Leaf = CPUID_GET_ADVANCED_POWER_MANAGEMENT;
        CpuRegisters.SubLeaf = 0;
        CpuRegisters.Eax = 0;
        CpuRegisters.Ebx = 0;
        CpuRegisters.Ecx = 0;
        CpuRegisters.Edx = 0;
        AR::CpuFunc::CpuId(&CpuRegisters);

        /* Verify Invariant TSC support */
        if(CpuRegisters.Edx & CPUID_FEATURES_EDX_TSCI)
        {
            TimerCapabilities.InvariantTsc = TRUE;
        }
    }
}

/**
 * Sets the profile interrupt interval. The interval may be bounded by hardware capabilities.
 *
 * @param Interval
 *        Supplies the requested profile interval in 100-nanosecond units.
 *
 * @return This routine returns the actual profile interval that was set.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
HL::Timer::SetProfileInterval(IN ULONG_PTR Interval)
{
    /* Validate and bound the requested profile interval against hardware limits */
    if(Interval < MIN_PROFILE_INTERVAL)
    {
        /* Enforce the minimum profile interval limit */
        Interval = MIN_PROFILE_INTERVAL;
    }
    else if(Interval > MAX_PROFILE_INTERVAL)
    {
        /* Enforce the maximum profile interval limit */
        Interval = MAX_PROFILE_INTERVAL;
    }

    /* Calculate the number of APIC timer ticks required for the requested interval */
    ProfilingInterval = (TimerFrequency / 10000) * (Interval / 1000);

    /* Update the APIC Timer Initial Count Register (TICR) to apply the new interval immediately */
    HL::Pic::WriteApicRegister(APIC_TICR, ProfilingInterval);

    /* Return the actual interval */
    return Interval;
}

/**
 * Stalls the CPU execution for a specified duration.
 *
 * @param MicroSeconds
 *        Supplies the number of microseconds to stall execution.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Timer::StallExecution(IN ULONG MicroSeconds)
{
    UNIMPLEMENTED;

    /* ACPI PM Timer not supported, fall back to PIT */
    PitStallExecution(MicroSeconds);
}

/**
 * Enables the profile interrupt for the specified profile source.
 *
 * @param ProfileSource
 *        Supplies the source of the profile interrupt to start.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Timer::StartProfileInterrupt(IN KPROFILE_SOURCE ProfileSource)
{
    APIC_LVT_REGISTER LvtRegister;

    /* Handle only ProfileTime and ProfileXtKernel */
    if(ProfileSource != ProfileTime && ProfileSource != ProfileXtKernel)
    {
        /* Invalid profile source, do nothing */
        return;
    }

    /* Set the interval */
    HL::Pic::WriteApicRegister(APIC_TICR, ProfilingInterval);

    /* Unmask interrupt */
    LvtRegister.Long = 0;
    LvtRegister.Mask = 0;
    LvtRegister.DeliveryMode = APIC_DM_FIXED;
    LvtRegister.TimerMode = 1;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Vector = APIC_VECTOR_PROFILE;
    HL::Pic::WriteApicRegister(APIC_TMRLVTR, LvtRegister.Long);
}

/**
 * Disables the profile interrupt for the specified profile source.
 *
 * @param ProfileSource
 *        Supplies the source of the profile interrupt to stop.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Timer::StopProfileInterrupt(IN KPROFILE_SOURCE ProfileSource)
{
    APIC_LVT_REGISTER LvtRegister;

    /* Handle only ProfileTime and ProfileXtKernel */
    if(ProfileSource != ProfileTime && ProfileSource != ProfileXtKernel)
    {
        /* Invalid profile source, do nothing */
        return;
    }

    /* Mask interrupt */
    LvtRegister.Long = 0;
    LvtRegister.Mask = 1;
    LvtRegister.DeliveryMode = APIC_DM_FIXED;
    LvtRegister.TimerMode = 1;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Vector = APIC_VECTOR_PROFILE;
    HL::Pic::WriteApicRegister(APIC_TMRLVTR, LvtRegister.Long);
}
