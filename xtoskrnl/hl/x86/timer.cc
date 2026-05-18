/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/x86/timer.cc
 * DESCRIPTION:     Timer support for x86 (i686/AMD64)
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

        /* Stall CPU execution for exactly 10 milliseconds */
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
 * Calibrates the Invariant TSC frequency.
 *
 * @return This routine returns the calculated TSC frequency in Hz.
 *
 * @since XT 1.0
 */
XTAPI
ULONGLONG
HL::Timer::CalibrateTscCounter(VOID)
{
    ULONGLONG InitialTickCount, FinalTickCount;
    ULONG TscAux;

    /* Get TSC frequency from the Core Crystal Clock */
    if(TimerCapabilities.Art && TimerCapabilities.TimerFrequency != 0 && TimerCapabilities.TscDenominator != 0)
    {
        /* CCC available, use it as the source of TSC frequency */
        return (TimerCapabilities.TimerFrequency * TimerCapabilities.TscNumerator) / TimerCapabilities.TscDenominator;
    }

    /* Latch the initial TSC value */
    InitialTickCount = AR::CpuFunc::ReadTimeStampCounterProcessor(&TscAux);

    /* Stall CPU execution for exactly 10 milliseconds */
    StallExecution(10000);

    /* Read current tick count from TSC */
    FinalTickCount = AR::CpuFunc::ReadTimeStampCounterProcessor(&TscAux);

    /* Calculate the elapsed ticks over the 10ms window */
    return (FinalTickCount - InitialTickCount) * 100;
}

/**
 * Calculates and configures the system time increments based on hardware timer properties.
 *
 * @param BaseFrequency
 *        Supplies the base running frequency of the hardware timer in Hz.
 *
 * @param HardwareDivider
 *        Supplies the programmed threshold/divider for the interrupt generation.
 */
XTAPI
VOID
HL::Timer::ConfigureTimeIncrement(IN ULONGLONG BaseFrequency, IN ULONGLONG HardwareDivider)
{
    /* Calculate the time increment */
    TimeIncrement = (ULONG)((10000000ULL * HardwareDivider) / BaseFrequency);

    /* Extract the fractional remainder */
    FractionalIncrement = (ULONG)((10000000000ULL * HardwareDivider) / BaseFrequency) % 1000;
    RunningFraction = 0;

    /* Synchronize the kernel's global timekeeping state with the new resolution settings */
    KE::SystemTime::SetTimeIncrement(TimeIncrement, TimeIncrement);
}

/**
 * Initializes the High Precision Event Timer (HPET) by discovering its ACPI configuration and mapping
 * its hardware registers into the kernel's virtual address space.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Timer::DetectHpet(VOID)
{
    ULONGLONG CounterPeriod;
    PACPI_HPET HpetTable;
    PHPET_REGISTERS Hpet;
    XTSTATUS Status;

    /* Reset global HPET state */
    HpetAddress = NULLPTR;
    HpetFrequency = 0;

    /* Retrieve the HPET table from the ACPI subsystem */
    Status = HL::Acpi::GetAcpiTable(ACPI_HPET_SIGNATURE, (PACPI_DESCRIPTION_HEADER*)&HpetTable);
    if(Status != STATUS_SUCCESS || !HpetTable)
    {
        /* HPET is not present on this system, return error code */
        return STATUS_NOT_FOUND;
    }

    /* Verify that the hardware registers are accessible via MMIO */
    if(HpetTable->BaseAddress.AddressSpaceID != ACPI_ADDRESS_SPACE_MEMORY)
    {
        /* HPET base address not found, return error */
        return STATUS_NOT_SUPPORTED;
    }

    /* Extract the physical base address from the Generic Address Structure (GAS) */
    if(HpetTable->BaseAddress.Address.QuadPart == 0)
    {
        /* Invalid address provided by firmware, return error code */
        return STATUS_UNSUCCESSFUL;
    }

    /* Map the physical hardware registers into the kernel's virtual memory space */
    Status = MM::HardwarePool::MapHardwareMemory(HpetTable->BaseAddress.Address, 1, FALSE, &HpetAddress);
    if(Status != STATUS_SUCCESS || !HpetAddress)
    {
        /* Memory mapping failed, return error code */
        return Status;
    }

    /* Configure the mapped memory region with Write-Through caching semantics */
    MM::HardwarePool::MarkHardwareMemoryWriteThrough(HpetAddress, 1);

    /* Extract the main counter period */
    Hpet = (PHPET_REGISTERS)HpetAddress;
    CounterPeriod = (Hpet->GeneralCapabilities >> 32) & 0xFFFFFFFF;

    /* Calculate the HPET operating frequency */
    if(CounterPeriod != 0)
    {
        /* Convert femtoseconds per tick to ticks per second */
        HpetFrequency = 1000000000000000ULL / CounterPeriod;
    }
    else
    {
        /* Assume the standard minimum HPET frequency (14.31818 MHz) */
        HpetFrequency = 14318180;
    }

    /* Enable the HPET main counter and disable legacy replacement */
    Hpet->GeneralConfiguration = (Hpet->GeneralConfiguration & ~HPET_CONFIG_LEGACY_REPLACEMENT) | HPET_CONFIG_ENABLE;

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Services the primary hardware clock interrupt, advancing the global system time,
 * maintaining local CPU thread quantums and performing profiling.
 *
 * @param TrapFrame
 *        Supplies a pointer to the hardware trap frame representing the interrupted execution context.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
HL::Timer::HandleClockInterrupt(IN PKTRAP_FRAME TrapFrame)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    KRUNLEVEL RunLevel;
    ULONG Increment;

    /* Start the interrupt */
    HL::Irq::BeginSystemInterrupt(CLOCK_LEVEL, &RunLevel);

    /* Check if PIT is the source of the interrupt */
    if(ClockType == TimerPit)
    {
        /* Check if system clock initialized the rollover */
        if(PitRollover != 0)
        {
            /* Update the PIT counter */
            PitPerformanceCounter += PitRollover;
        }
    }

    /* Check if profiling is currently active */
    if(ProfilingEnabled)
    {
        /* Retrieve the processor-specific control block */
        Prcb = KE::Processor::GetCurrentProcessorControlBlock();

        /* Trigger a profile sample if the countdown has expired */
        if(Prcb->ProfilingCountdown == 0)
        {
            /* Dispatch the profile interrupt */
            HL::Irq::HandleProfileInterrupt(TrapFrame);

            /* Reset the local countdown */
            Prcb->ProfilingCountdown = ProfilingTicks;
        }

        /* Decrement the profiling countdown for the current hardware tick */
        Prcb->ProfilingCountdown--;
    }

    /* Initialize the increment and update the fractional drift accumulator */
    Increment = TimeIncrement;
    RunningFraction += FractionalIncrement;

    /* Check for fractional overflow */
    if(RunningFraction >= 1000)
    {
        /* Apply the compensation tick and normalize the fractional remainder */
        Increment++;
        RunningFraction -= 1000;
    }

    /* Route the timekeeping logic based on the underlying timer topology */
    if(ClockType == TimerLapic)
    {
        /* Restrict global system time updates exclusively to the Bootstrap Processor */
        if(KE::Processor::GetCurrentProcessorNumber() == 0)
        {
            /* Advance the global system time */
            KE::SystemTime::UpdateSystemTime(TrapFrame, Increment, RunLevel);
        }
        else
        {
            /* Limit Application Processors (APs) to update runtimes */
            KE::Dispatcher::UpdateRunTime(TrapFrame, RunLevel);
        }
    }
    else
    {
        /* Advance the global system time */
        KE::SystemTime::UpdateSystemTime(TrapFrame, Increment, RunLevel);

        /* Broadcast an IPI to awaken all APs for local quantum updates */
        HL::Pic::SendBroadcastIpi(APIC_VECTOR_CLOCK_IPI, FALSE);
    }

    /* End the interrupt */
    HL::Irq::EndInterrupt(TrapFrame, RunLevel);
}

/**
 * Services the inter-processor clock interrupt, maintaining local CPU thread quantums and performing profiling.
 *
 * @param TrapFrame
 *        Supplies a pointer to the hardware trap frame representing the interrupted execution context.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
HL::Timer::HandleClockIpiInterrupt(IN PKTRAP_FRAME TrapFrame)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    KRUNLEVEL RunLevel;

    /* Start the interrupt */
    HL::Irq::BeginSystemInterrupt(CLOCK_LEVEL, &RunLevel);

    /* Check if profiling is currently active */
    if(ProfilingEnabled)
    {
        /* Retrieve the processor-specific control block */
        Prcb = KE::Processor::GetCurrentProcessorControlBlock();

        /* Trigger a profile sample if the countdown has expired */
        if(Prcb->ProfilingCountdown == 0)
        {
            /* Dispatch the profile interrupt */
            HL::Irq::HandleProfileInterrupt(TrapFrame);

            /* Reset the local countdown */
            Prcb->ProfilingCountdown = ProfilingTicks;
        }

        /* Decrement the profiling countdown for the current hardware tick */
        Prcb->ProfilingCountdown--;
    }

    /* Call the kernel to update runtimes */
    KE::Dispatcher::UpdateRunTime(TrapFrame, RunLevel);

    /* End the interrupt */
    HL::Irq::EndInterrupt(TrapFrame, RunLevel);
}

/**
 * Initializes and calibrates the Local APIC Timer.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Timer::InitializeApicTimer(VOID)
{
    APIC_LVT_REGISTER LvtRegister;
    XTSTATUS Status;
    ULONG Divider;

    /* Set APIC timer to divide by 1 */
    HL::Pic::WriteApicRegister(APIC_TDCR, TIMER_DivideBy1);

    /* Calibrate the APIC timer */
    Status = CalibrateApicTimer();
    if(Status != STATUS_SUCCESS)
    {
        /* APIC calibration failed, return error code */
        return Status;
    }

    /* Calculate the hardware threshold */
    Divider = TimerFrequency / 1000;

    /* Program the APIC timer for periodic mode */
    LvtRegister.Long = 0;
    LvtRegister.Mask = 0;
    LvtRegister.TimerMode = 1;
    LvtRegister.DeliveryMode = APIC_DM_FIXED;
    LvtRegister.TriggerMode = APIC_TGM_EDGE;
    LvtRegister.Vector = APIC_VECTOR_CLOCK;
    HL::Pic::WriteApicRegister(APIC_TMRLVTR, LvtRegister.Long);

    /* Jump-start the heartbeat */
    HL::Pic::WriteApicRegister(APIC_TICR, Divider);

    /* Configure the kernel timekeeping abstractions based on calibrated APIC metrics */
    HL::Timer::ConfigureTimeIncrement(TimerFrequency, Divider);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes the High Precision Event Timer (HPET) Timer.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Timer::InitializeHpetTimer(VOID)
{
    ULONGLONG ClockPeriodTicks, TicksPerMillisecond;
    PHPET_REGISTERS Hpet;

    /* Ensure the HPET hardware registers are successfully mapped */
    if(!HpetAddress)
    {
        /* The hardware mapping is not present, return error code */
        return STATUS_UNSUCCESSFUL;
    }

    /* Cast the mapped virtual address to the HPET hardware register */
    Hpet = (PHPET_REGISTERS)HpetAddress;

    /* Halt the main counter and disable legacy routing */
    Hpet->GeneralConfiguration &= ~(HPET_CONFIG_ENABLE | HPET_CONFIG_LEGACY_REPLACEMENT);

    /* Reset the main counter to a known baseline */
    Hpet->MainCounterValue = 0;

    /* Calculate the required hardware ticks for a standard 1-millisecond system clock interval */
    TicksPerMillisecond = HpetFrequency / 1000;
    ClockPeriodTicks = TicksPerMillisecond * 1;

    /* Configure Comparator 0 for periodic mode with interrupt enable */
    Hpet->Timers[0].Configuration |= (HPET_TIMER_CONFIG_ENABLED |
                                      HPET_TIMER_CONFIG_PERIODIC |
                                      HPET_TIMER_CONFIG_VALUE_ACCUMULATOR);

    /* Write the initial comparator value */
    Hpet->Timers[0].Comparator = ClockPeriodTicks;

    /* Write the periodic interval into the accumulator */
    Hpet->Timers[0].Comparator = ClockPeriodTicks;

    /* Enable the main counter and activate legacy replacement routing */
    Hpet->GeneralConfiguration |= (HPET_CONFIG_ENABLE | HPET_CONFIG_LEGACY_REPLACEMENT);

    /* Configure the kernel timekeeping abstractions based on HPET metrics */
    ConfigureTimeIncrement(HpetFrequency, ClockPeriodTicks);

    /* Route the IRQ 0 to the primary system clock vector */
    HL::Pic::AllocateSystemInterrupt(0, CLOCK_LEVEL, APIC_VECTOR_CLOCK);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes the legacy Programmable Interval Timer (PIT).
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::Timer::InitializePitTimer(VOID)
{
    UCHAR LowByte, HighByte;
    ULONG Divider;

    /* Set the target frequency (1000 Hz) and calculate the required hardware divider */
    Divider = PIT_BASE_FREQUENCY / 1000;

    /* Clamp the calculated divider to prevent 16-bit hardware overflow */
    if(Divider > 65535)
    {
        /* Cap the maximum possible interval */
        Divider = 65535;
    }
    else if(Divider < 1)
    {
        /* Enforce a minimum hardware divider */
        Divider = 1;
    }

    /* Persist the calculated divider into a global state */
    PitRollover = Divider;

    /* Split the 16-bit divider into an independent LSB and MSB */
    LowByte = (UCHAR)(Divider & 0xFF);
    HighByte = (UCHAR)((Divider >> 8) & 0xFF);

    /* Configure Channel 0 for LSB/MSB access, Mode 2 */
    HL::IoPort::WritePort8(PIT_COMMAND_PORT, PIT_CMD_CHANNEL0 |
                                             PIT_CMD_ACCESS_LOWBYTE_HIGHBYTE |
                                             PIT_MODE2_RATE_GENERATOR);

    /* Transmit the Least Significant Byte (LSB) */
    HL::IoPort::WritePort8(PIT_DATA_PORT0, LowByte);

    /* Transmit the Most Significant Byte (MSB) */
    HL::IoPort::WritePort8(PIT_DATA_PORT0, HighByte);

    /* Configure the kernel timekeeping abstractions based on legacy PIT metrics */
    ConfigureTimeIncrement(PIT_BASE_FREQUENCY, Divider);

    /* Route the standard PIT IRQ 0 to the primary system clock vector */
    HL::Pic::AllocateSystemInterrupt(0, CLOCK_LEVEL, APIC_VECTOR_CLOCK);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Performs the primary initialization of the system timer.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Timer::InitializeTimer(VOID)
{
    XTSTATUS Status;

    /* Probe hardware capabilities */
    ProbeTimerType();

    /* Check if a suitable hardware timer was selected */
    if(TimerRoutines.InitializeClock)
    {
        /* Proceed with system clock initialization */
        Status = TimerRoutines.InitializeClock();
        if(Status != STATUS_SUCCESS)
        {
            /* System cannot operate without a functional system clock interrupt */
            KE::Crash::Panic(0);
        }
    }

    /* Set the default system profile interval */
    HL::Timer::SetProfileInterval(1000);

    /* Ensure the profile interrupt generation is explicitly disabled */
    StopProfileInterrupt(ProfileXtKernel);

    /* Register the system clock interrupt handler */
    HL::Irq::RegisterSystemInterruptHandler(APIC_VECTOR_CLOCK, HandleClockInterrupt);
    HL::Irq::RegisterSystemInterruptHandler(APIC_VECTOR_CLOCK_IPI, HandleClockIpiInterrupt);
}

/**
 * Performs an initial Timer initialization, discovering hardware and routing the Timer Dispatch table.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::Timer::ProbeTimerType(VOID)
{
    PACPI_TIMER_INFO AcpiTimerInfo;
    WCHAR ParameterValue[16];
    XTSTATUS Status;

    /* Enumerate hardware timing capabilities */
    QueryTimerCapabilities();

    /* Discover and map the High Precision Event Timer */
    Status = DetectHpet();
    if(Status != STATUS_SUCCESS)
    {
        /* Diagnostic warning upon HPET subsystem initialization failure */
        DebugPrint(L"TIMER: HPET initialization failed or HPET not present\n");
    }

    /* Initialize the hardware selection states */
    ClockType = TimerNone;
    TimerType = TimerNone;

    /* Query the kernel boot environment for a user-specified system clock override */
    if(KE::BootInformation::GetKernelParameterValue(L"CLOCK", ParameterValue, 16) == STATUS_SUCCESS)
    {
        /* Evaluate the boot parameter string */
        if(RTL::WideString::FindWideStringInsensitive(ParameterValue, L"HPET"))
        {
            /* Designate the HPET */
            ClockType = TimerHpet;
        }
        else if(RTL::WideString::FindWideStringInsensitive(ParameterValue, L"LAPIC"))
        {
            /* Designate the Local APIC */
            ClockType = TimerLapic;
        }
        else if(RTL::WideString::FindWideStringInsensitive(ParameterValue, L"PIT"))
        {
            /* Designate the legacy PIT */
            ClockType = TimerPit;
        }
    }

    /* Query the kernel boot environment for a user-specified performance counter override */
    if(KE::BootInformation::GetKernelParameterValue(L"TIMER", ParameterValue, 16) == STATUS_SUCCESS)
    {
        /* Evaluate the boot parameter string */
        if(RTL::WideString::FindWideStringInsensitive(ParameterValue, L"ACPI") ||
           RTL::WideString::FindWideStringInsensitive(ParameterValue, L"PM"))
        {
            /* Designate the ACPI PM Timer */
            TimerType = TimerAcpiPm;
        }
        else if(RTL::WideString::FindWideStringInsensitive(ParameterValue, L"HPET"))
        {
            /* Designate the HPET */
            TimerType = TimerHpet;
        }
        else if(RTL::WideString::FindWideStringInsensitive(ParameterValue, L"PIT"))
        {
            /* Designate the legacy PIT */
            TimerType = TimerPit;
        }
        else if(RTL::WideString::FindWideStringInsensitive(ParameterValue, L"TSC"))
        {
            /* Designate the Invariant TSC */
            TimerType = TimerTsc;
        }
    }

    /* Verify the hardware viability of the requested system clock */
    if(ClockType != TimerNone && !ValidateTimerSupport(ClockType, TRUE))
    {
        /* Invalidate the clock selection upon hardware validation failure */
        DebugPrint(L"TIMER: Requested system clock [%d] unavailable\n", ClockType);
        ClockType = TimerNone;
    }

    /* Verify the hardware viability of the requested performance counter */
    if(TimerType != TimerNone && !ValidateTimerSupport(TimerType, FALSE))
    {
        /* Invalidate the counter selection upon hardware validation failure */
        DebugPrint(L"TIMER: Requested hardware counter [%d] unavailable\n", TimerType);
        TimerType = TimerNone;
    }

    /* Execute the autonomous hardware selection for the system clock */
    if(ClockType == TimerNone)
    {
        /* Probe system clock suitability */
        if(ValidateTimerSupport(TimerLapic, TRUE))
        {
            /* Select the Local APIC */
            ClockType = TimerLapic;
        }
        else if(ValidateTimerSupport(TimerHpet, TRUE))
        {
            /* Select the HPET */
            ClockType = TimerHpet;
        }
        else
        {
            /* Fallback to the legacy PIT */
            ClockType = TimerPit;
        }
    }

    /* Execute the autonomous hardware selection for the performance counter */
    if(TimerType == TimerNone)
    {
        /* Probe the performance counter suitability */
        if(ValidateTimerSupport(TimerTsc, FALSE))
        {
            /* Select the Invariant TSC */
            TimerType = TimerTsc;
        }
        else if(ValidateTimerSupport(TimerHpet, FALSE))
        {
            /* Select the HPET */
            TimerType = TimerHpet;
        }
        else if(ValidateTimerSupport(TimerAcpiPm, FALSE))
        {
            /* Select the ACPI PM Timer */
            TimerType = TimerAcpiPm;
        }
        else
        {
            /* Fallback to the legacy PIT */
            TimerType = TimerPit;
        }
    }

    /* Retrieve the ACPI PM Timer hardware configuration */
    HL::Acpi::GetAcpiTimerInfo(&AcpiTimerInfo);

    /* Determine if the ACPI PM Timer port is physically provisioned */
    if(AcpiTimerInfo->TimerPort != 0)
    {
        /* Temporarily route execution stalls through the active ACPI PM hardware */
        TimerRoutines.StallExecution = StallExecutionAcpiPm;
    }
    else
    {
        /* Temporarily route execution stalls through the legacy PIT hardware */
        TimerRoutines.StallExecution = StallExecutionPit;
    }

    /* Dispatch the initialization routines based on the resolved system clock */
    switch(ClockType)
    {
        case TimerLapic:
            /* Register the Local APIC initialization handler */
            TimerRoutines.InitializeClock = InitializeApicTimer;
            TimerRoutines.SetClockRate = SetClockRateApic;
            DebugPrint(L"System Clock: Local APIC Timer (ARAT), ");
            break;
        case TimerHpet:
            /* Register the HPET comparator initialization handler */
            TimerRoutines.InitializeClock = InitializeHpetTimer;
            TimerRoutines.SetClockRate = NULLPTR;
            DebugPrint(L"System Clock: HPET Comparator, ");
            break;
        default:
            /* Register the legacy PIT initialization handler */
            TimerRoutines.InitializeClock = InitializePitTimer;
            TimerRoutines.SetClockRate = NULLPTR;
            DebugPrint(L"System Clock: Legacy PIT, ");
            break;
    }

    /* Dispatch the routines and configure baseline frequencies for the performance counter */
    switch(TimerType)
    {
        case TimerTsc:
            /* Register the TSC */
            PerformanceFrequency = CalibrateTscCounter();
            TimerRoutines.QueryPerformanceCounter = QueryPerformanceCounterTsc;
            TimerRoutines.StallExecution = StallExecutionTsc;
            DebugPrint(L"Performance Counter: Invariant TSC @ %lluHz\n", PerformanceFrequency);
            break;
        case TimerHpet:
            /* Register the HPET */
            PerformanceFrequency = HpetFrequency;
            TimerRoutines.QueryPerformanceCounter = QueryPerformanceCounterHpet;
            TimerRoutines.StallExecution = StallExecutionHpet;
            DebugPrint(L"Performance Counter: HPET @ %lluHz\n", PerformanceFrequency);
            break;
        case TimerAcpiPm:
            /* Register the ACPI PM */
            PerformanceFrequency = 3579545;
            TimerRoutines.QueryPerformanceCounter = QueryPerformanceCounterAcpiPm;
            TimerRoutines.StallExecution = StallExecutionAcpiPm;
            KeInitializeSpinLock(&PerformanceCounterLock);
            DebugPrint(L"Performance Counter: ACPI PM Timer\n");
            break;
        default:
            /* Register the legacy PIT */
            PerformanceFrequency = 1193182;
            TimerRoutines.QueryPerformanceCounter = QueryPerformanceCounterPit;
            TimerRoutines.StallExecution = StallExecutionPit;
            KeInitializeSpinLock(&PerformanceCounterLock);
            DebugPrint(L"Performance Counter: Legacy PIT\n");
            break;
    }
}

/**
 * Retrieves the current value of the high-resolution performance counter.
 *
 * @param PerformanceFrequency
 *        Suplies an optional pointer to a variable that receives the performance counter frequency in Hz.
 *
 * @return This routine returns the current 64-bit monotonic tick count.
 *
 * @since XT 1.0
 */
XTAPI
LARGE_INTEGER
HL::Timer::QueryPerformanceCounter(OUT PLARGE_INTEGER Frequency)
{
    LARGE_INTEGER CurrentCounter;

    /* Check if the caller requested the frequency */
    if(Frequency)
    {
        /* Assign the cached hardware frequency */
        Frequency->QuadPart = PerformanceFrequency;
    }

    /* Dispatch to the specific hardware implementation */
    CurrentCounter.QuadPart = TimerRoutines.QueryPerformanceCounter();

    /* Return the retrieved timestamp */
    return CurrentCounter;
}

/**
 * Queries the current value of the ACPI Power Management Timer.
 *
 * @return This routine returns the current ACPI timer tick count.
 *
 * @since XT 1.0
 */
XTAPI
ULONGLONG
HL::Timer::QueryPerformanceCounterAcpiPm(VOID)
{
    PACPI_TIMER_INFO AcpiTimerInfo;
    ULONG CurrentValue;

    /* Retrieve the ACPI Timer configuration */
    HL::Acpi::GetAcpiTimerInfo(&AcpiTimerInfo);

    /* Raise RunLevel to DISPATCH_LEVEL and acquire spinlock */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
    KE::SpinLockGuard SpinLockGuard(&PerformanceCounterLock);

    /* Read the current hardware value and apply the bitmask */
    CurrentValue = HL::IoPort::ReadPort32(AcpiTimerInfo->TimerPort) & AcpiTimerInfo->MsbMask;

    /* Calculate delta, accumulate the results and update last value for the next call */
    SystemPerformanceCounter += (CurrentValue - AcpiPmPerformanceCounter) & AcpiTimerInfo->MsbMask;
    AcpiPmPerformanceCounter = CurrentValue;

    /* Return the accumulated value */
    return SystemPerformanceCounter;
}

/**
 * Queries the current value of the High Precision Event Timer (HPET).
 *
 * @return This routine returns the current HPET main counter value.
 *
 * @since XT 1.0
 */
XTAPI
ULONGLONG
HL::Timer::QueryPerformanceCounterHpet(VOID)
{
    /* Perform a direct MMIO read from the register address */
    return ((PHPET_REGISTERS)HpetAddress)->MainCounterValue;
}

/**
 * Queries the current value of the Legacy PIT.
 *
 * @return This routine returns the interpolated tick count.
 *
 * @since XT 1.0
 */
XTAPI
ULONGLONG
HL::Timer::QueryPerformanceCounterPit(VOID)
{
    ULONG ClockDelta, CounterValue;
    ULONGLONG PerformanceCounter;

    /* Check if system clock initialized the rollover */
    if(PitRollover == 0)
    {
        /* Return the baseline counter value */
        return SystemPerformanceCounter;
    }

    /* Raise RunLevel to DISPATCH_LEVEL and acquire spinlock */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
    KE::SpinLockGuard SpinLockGuard(&PerformanceCounterLock);

    /* Repeatedly sample the global tick count and the hardware counter */
    do
    {
        /* Get the current global performance tick updated by ISR */
        PerformanceCounter = PitPerformanceCounter;

        /* Send the LATCH command to freeze value in the PIT buffer */
        HL::IoPort::WritePort8(PIT_COMMAND_PORT, 0x00);

        /* Read the remaining count from the data port */
        CounterValue = HL::IoPort::ReadPort8(PIT_DATA_PORT0) | (HL::IoPort::ReadPort8(PIT_DATA_PORT0) << 8);
    }
    while(PerformanceCounter != PitPerformanceCounter);

    /* Clamp the value in case of a hardware glitch right at the rollover point */
    if(CounterValue > PitRollover)
    {
        /* Force the sampled counter value to the maximum programmed reload threshold */
        CounterValue = PitRollover;
    }

    /* Calculate how many ticks have passed since the last interrupt */
    ClockDelta = PitRollover - CounterValue;

    /* Synthesize the final high-precision timestamp */
    PerformanceCounter += ClockDelta;

    /* Guard against time drifting backward */
    if(PerformanceCounter < SystemPerformanceCounter)
    {
        /* Compensate missing interrupt */
        PerformanceCounter += PitRollover;
    }

    /* Update the last recorded counter */
    SystemPerformanceCounter = PerformanceCounter;

    /* Return the timestamp */
    return PerformanceCounter;
}

/**
 * Queries the current value of the Time Stamp Counter (TSC).
 *
 * @return This routine returns the current invariant TSC tick count.
 *
 * @since XT 1.0
 */
XTAPI
ULONGLONG
HL::Timer::QueryPerformanceCounterTsc(VOID)
{
    ULONG TscAux;

    /* Retrieve the timestamp */
    return AR::CpuFunc::ReadTimeStampCounterProcessor(&TscAux);
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
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    CPUID_REGISTERS CpuRegisters;
    ULONG MaxStandardLeaf;

    /* Get current processor control block */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Set timer capabilities based on supported CPU features */
    TimerCapabilities.Arat = (Prcb->CpuId.FeatureBits & KCF_ARAT) != 0;
    TimerCapabilities.InvariantTsc= (Prcb->CpuId.ExtendedFeatureBits & KCF_INVARIANT_TSC) != 0;
    TimerCapabilities.RDTSCP = (Prcb->CpuId.ExtendedFeatureBits & KCF_RDTSCP) != 0;
    TimerCapabilities.TscDeadline = (Prcb->CpuId.FeatureBits & KCF_TSC_DEADLINE) != 0;

    /* Query maximum standard CPUID leaf */
    RTL::Memory::ZeroMemory(&CpuRegisters, sizeof(CPUID_REGISTERS));
    CpuRegisters.Leaf = CPUID_GET_VENDOR_STRING;
    AR::CpuFunc::CpuId(&CpuRegisters);
    MaxStandardLeaf = CpuRegisters.Eax;

    /* Check Always Running Timer - ART if leaf supported */
    if(MaxStandardLeaf >= CPUID_GET_TSC_CRYSTAL_CLOCK)
    {
        /* Query the Time Stamp Counter and Core Crystal Clock information CPUID leaf */
        RTL::Memory::ZeroMemory(&CpuRegisters, sizeof(CPUID_REGISTERS));
        CpuRegisters.Leaf = CPUID_GET_TSC_CRYSTAL_CLOCK;
        AR::CpuFunc::CpuId(&CpuRegisters);

        /* Verify Always Running Timer support */
        if(CpuRegisters.Eax != 0 && CpuRegisters.Ebx != 0)
        {
            /* Mark the ART as supported */
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
}

/**
 * Requests a dynamic adjustment of the system clock resolution.
 *
 * @param Rate
 *        Supplies the requested clock rate change in 100-nanosecond units.
 *
 * @return This routine returns the actual clock rate granted by the hardware.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
HL::Timer::SetClockRate(IN ULONG Rate)
{
    ULONG HardwareRate;

    /* Validate and clamp the clock rate against architectural limits */
    if(Rate < HL_MINIMUM_CLOCK_RATE)
    {
        /* Enforce minimum resolution to prevent system-wide interrupt storms */
        Rate = HL_MINIMUM_CLOCK_RATE;
    }
    else if(Rate > HL_MAXIMUM_CLOCK_RATE)
    {
        /* Cap the clock rate to ensure responsiveness */
        Rate = HL_MAXIMUM_CLOCK_RATE;
    }

    /* Check if the active hardware backend supports dynamic rate scaling */
    if(TimerRoutines.SetClockRate)
    {
        /* Dispatch the reprogramming request */
        HardwareRate = TimerRoutines.SetClockRate(Rate);
    }
    else
    {
        /* Fallback to the current fixed rate as scaling is unsupported */
        HardwareRate = TimeIncrement;
    }

    /* Return the actual clock rate set */
    return HardwareRate;
}

/**
 * Adjusts the Local APIC Timer frequency to match the requested resolution.
 *
 * @param Rate
 *        Supplies the requested clock rate change in 100-nanosecond units.
 *
 * @return This routine returns the actual clock rate granted by the hardware.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
HL::Timer::SetClockRateApic(ULONG Rate)
{
    ULONG NewDivider;
    BOOLEAN Interrupts;

    /* TODO: Implement IPI broadcast to synchronize all cores in SMP mode */
    UNIMPLEMENTED;

    /* Calculate the hardware-specific tick count for the requested rate */
    NewDivider = (ULONG)(((ULONGLONG)TimerFrequency * Rate) / 10000000ULL);

    /* Prevent an invalid zero-count state */
    if(NewDivider == 0)
    {
        /* Enforce a single-tick */
        NewDivider = 1;
    }

    /* Check whether interrupts are enabled */
    Interrupts = AR::CpuFunc::InterruptsEnabled();

    /* Disable interrupts */
    AR::CpuFunc::ClearInterruptFlag();

    /* Commit the new divider to the TICR register */
    HL::Pic::WriteApicRegister(APIC_TICR, NewDivider);

    /* Synchronize the kernel's timekeeping math with the new hardware state */
    ConfigureTimeIncrement(TimerFrequency, NewDivider);

    /* Check whether interrupts need to be re-enabled */
    if(Interrupts)
    {
        /* Re-enable interrupts */
        AR::CpuFunc::SetInterruptFlag();
    }

    /* Return the actual clock rate set */
    return TimeIncrement;
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

    /* Calculate the required number of ticks for the requested interval */
    ProfilingTicks = (ULONG)(Interval / 10000);

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
    /* Dispatch the stall request */
    TimerRoutines.StallExecution(MicroSeconds);
}

/**
 * Stalls the CPU execution for a specified duration using the ACPI Power Management Timer.
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
HL::Timer::StallExecutionAcpiPm(IN ULONG MicroSeconds)
{
    PACPI_TIMER_INFO AcpiTimerInfo;
    ULONG StartTick, CurrentTick, Delta, TicksElapsed;
    ULONGLONG TargetTicks;

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

    /* Retrieve the ACPI PM Timer hardware configuration */
    HL::Acpi::GetAcpiTimerInfo(&AcpiTimerInfo);

    /* Calculate the target number of ticks based on the standard ACPI PM frequency */
    TargetTicks = ((ULONGLONG)MicroSeconds * 3579545ULL) / 1000000ULL;
    TicksElapsed = 0;

    /* Sample the initial hardware tick count and apply the hardware-specific bitmask */
    StartTick = HL::IoPort::ReadPort32(AcpiTimerInfo->TimerPort) & AcpiTimerInfo->MsbMask;

    /* Spin the processor until the accumulated hardware ticks reach the calculated target */
    while(TicksElapsed < TargetTicks)
    {
        /* Sample the current hardware tick count */
        CurrentTick = HL::IoPort::ReadPort32(AcpiTimerInfo->TimerPort) & AcpiTimerInfo->MsbMask;

        /* Calculate the tick delta */
        Delta = (CurrentTick - StartTick) & AcpiTimerInfo->MsbMask;

        /* Accumulate the elapsed ticks and advance the start marker */
        TicksElapsed += Delta;
        StartTick = CurrentTick;

        /* Issue a PAUSE instruction to relieve memory bus contention */
        AR::CpuFunc::YieldProcessor();
    }
}

/**
 * Stalls the CPU execution for a specified duration (maximum 3 seconds) using the High Precision Event Timer (HPET).
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
HL::Timer::StallExecutionHpet(IN ULONG MicroSeconds)
{
    PHPET_REGISTERS Hpet;
    ULONGLONG StartTick, TargetTicks;

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

    /* Cast the mapped virtual address to the HPET hardware register */
    Hpet = (PHPET_REGISTERS)HpetAddress;

    /* Calculate target ticks based on HPET frequency */
    TargetTicks = ((ULONGLONG)MicroSeconds * PerformanceFrequency) / 1000000ULL;
    StartTick = Hpet->MainCounterValue;

    /* Spin until the elapsed ticks reach the target */
    while((Hpet->MainCounterValue - StartTick) < TargetTicks)
    {
        /* Issue a PAUSE instruction to relieve memory bus contention */
        AR::CpuFunc::YieldProcessor();
    }
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
HL::Timer::StallExecutionPit(IN ULONG MicroSeconds)
{
    USHORT CurrentCount, PreviousCount;
    ULONG TargetTicks, TickCounter;
    UCHAR Port61State;

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

    /* Convert microseconds to PIT ticks using exact frequency arithmetic */
    TargetTicks = (ULONG)(((ULONGLONG)MicroSeconds * 1193182ULL) / 1000000ULL);
    TickCounter = 0;

    /* Save the current state of System Control Port B */
    Port61State = HL::IoPort::ReadPort8(0x61);

    /* Enable PIT Channel 2 and disable the PC Speaker */
    HL::IoPort::WritePort8(0x61, (Port61State & ~0x02) | 0x01);

    /* Configure PIT Channel 2 for Read/Write LSB then MSB, Mode 0 */
    HL::IoPort::WritePort8(PIT_COMMAND_PORT, 0xB0);

    /* Initialize the PIT counter with the maximum possible value */
    HL::IoPort::WritePort8(PIT_DATA_PORT2, 0xFF);
    HL::IoPort::WritePort8(PIT_DATA_PORT2, 0xFF);

    /* Latch and read the initial counter value */
    HL::IoPort::WritePort8(PIT_COMMAND_PORT, 0x80);
    PreviousCount = HL::IoPort::ReadPort8(PIT_DATA_PORT2);
    PreviousCount |= (HL::IoPort::ReadPort8(PIT_DATA_PORT2) << 8);

    /* Poll the PIT */
    while(TickCounter < TargetTicks)
    {
        /* Latch the current counter value without stopping the timer */
        HL::IoPort::WritePort8(PIT_COMMAND_PORT, 0x80);
        CurrentCount = HL::IoPort::ReadPort8(PIT_DATA_PORT2);
        CurrentCount |= (HL::IoPort::ReadPort8(PIT_DATA_PORT2) << 8);

        /* Calculate elapsed ticks since the last read */
        TickCounter += (PreviousCount - CurrentCount) & 0xFFFF;

        /* Update the tracking variable */
        PreviousCount = CurrentCount;
    }

    /* Restore the original state of PIT Port */
    HL::IoPort::WritePort8(0x61, Port61State);
}

/**
 * Stalls the CPU execution for a specified duration (maximum 3 seconds) using the Time Stamp Counter (TSC).
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
HL::Timer::StallExecutionTsc(IN ULONG MicroSeconds)
{
    ULONGLONG StartTick, TargetTicks;

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

    /* Calculate target ticks based on calibrated TSC frequency */
    TargetTicks = ((ULONGLONG)MicroSeconds * PerformanceFrequency) / 1000000ULL;
    StartTick = AR::CpuFunc::ReadTimeStampCounter();

    /* Spin until the elapsed ticks reach the target */
    while((AR::CpuFunc::ReadTimeStampCounter() - StartTick) < TargetTicks)
    {
        /* Issue a PAUSE instruction to relieve memory bus contention */
        AR::CpuFunc::YieldProcessor();
    }
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
    /* Handle only ProfileTime and ProfileXtKernel */
    if(ProfileSource != ProfileTime && ProfileSource != ProfileXtKernel)
    {
        /* Invalid profile source, do nothing */
        return;
    }

    /* Set the global software flag to enable profiling */
    ProfilingEnabled = TRUE;
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
    /* Handle only ProfileTime and ProfileXtKernel */
    if(ProfileSource != ProfileTime && ProfileSource != ProfileXtKernel)
    {
        /* Invalid profile source, do nothing */
        return;
    }

    /* Clear the global software flag to disable profiling */
    ProfilingEnabled = FALSE;
}

/**
 * Checks if the specified hardware timer is supported and available on the current system.
 *
 * @param TimerType
 *        Supplies the hardware timer type to query.
 *
 * @param IsClock
 *        Supplies TRUE if the timer is intended to be used as an interrupt generator (System Clock),
 *        or FALSE if it is intended to be used as a performance counter (Stoper).
 *
 * @return This routine returns TRUE if the timer is available and supported, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
HL::Timer::ValidateTimerSupport(IN TIMER_TYPE TimerType,
                                IN BOOLEAN IsClock)
{
    PACPI_TIMER_INFO AcpiTimerInfo;

    /* Query ACPI timer info */
    HL::Acpi::GetAcpiTimerInfo(&AcpiTimerInfo);

    /* Validate timer type */
    switch(TimerType)
    {
        case TimerTsc:
            /* Check if TSC and RDTSCP are supported */
            return (TimerCapabilities.InvariantTsc && TimerCapabilities.RDTSCP) ? TRUE : FALSE;
        case TimerHpet:
            /* Check if HPET is supported */
            return (HpetAddress != NULLPTR) ? TRUE : FALSE;
        case TimerAcpiPm:
            /* Check if ACPI PM Timer is supported */
            return (!IsClock && AcpiTimerInfo->TimerPort != 0) ? TRUE : FALSE;
        case TimerLapic:
            /* Check if LAPIC Timer (ARAT) is supported */
            return (IsClock && TimerCapabilities.Arat) ? TRUE : FALSE;
        case TimerPit:
            /* PIT Timer is always supported */
            return TRUE;
        default:
            /* Invalid timer type, return FALSE */
            return FALSE;
    }
}
