/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/timer.hh
 * DESCRIPTION:     ACPI Timer support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_TIMER_HH
#define __XTOSKRNL_HL_TIMER_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class Timer
    {
        private:
            STATIC ULONG AcpiPmPerformanceCounter;
            STATIC TIMER_TYPE ClockType;
            STATIC ULONG FractionalIncrement;
            STATIC PVOID HpetAddress;
            STATIC ULONGLONG HpetFrequency;
            STATIC KSPIN_LOCK PerformanceCounterLock;
            STATIC ULONGLONG PerformanceFrequency;
            STATIC ULONGLONG PitPerformanceCounter;
            STATIC ULONG PitRollover;
            STATIC BOOLEAN ProfilingEnabled;
            STATIC ULONG ProfilingTicks;
            STATIC ULONG RunningFraction;
            STATIC ULONGLONG SystemPerformanceCounter;
            STATIC ULONG TimeIncrement;
            STATIC TIMER_CAPABILITIES TimerCapabilities;
            STATIC ULONG TimerFrequency;
            STATIC TIMER_ROUTINES TimerRoutines;
            STATIC TIMER_TYPE TimerType;

        public:
            STATIC XTAPI VOID InitializeLocalClock(VOID);
            STATIC XTAPI VOID InitializeTimer(VOID);
            STATIC XTAPI LARGE_INTEGER QueryPerformanceCounter(OUT PLARGE_INTEGER PerformanceFrequency);
            STATIC XTAPI ULONG SetClockRate(IN ULONG Rate);
            STATIC XTAPI ULONG_PTR SetProfileInterval(IN ULONG_PTR Interval);
            STATIC XTAPI VOID StallExecution(IN ULONG MicroSeconds);
            STATIC XTAPI VOID StartProfileInterrupt(IN KPROFILE_SOURCE ProfileSource);
            STATIC XTAPI VOID StopProfileInterrupt(IN KPROFILE_SOURCE ProfileSource);

        private:
            STATIC XTAPI XTSTATUS CalibrateApicTimer();
            STATIC XTAPI ULONGLONG CalibrateTscCounter(VOID);
            STATIC XTAPI VOID ConfigureTimeIncrement(IN ULONGLONG BaseFrequency,
                                                     IN ULONGLONG HardwareDivider);
            STATIC XTAPI XTSTATUS DetectHpet(VOID);
            STATIC XTCDECL VOID HandleClockInterrupt(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleClockIpiInterrupt(IN PKTRAP_FRAME TrapFrame);
            STATIC XTAPI XTSTATUS InitializeApicTimer(VOID);
            STATIC XTAPI XTSTATUS InitializeHpetTimer(VOID);
            STATIC XTAPI XTSTATUS InitializePitTimer(VOID);
            STATIC XTAPI VOID ProbeTimerType(VOID);
            STATIC XTAPI ULONGLONG QueryPerformanceCounterAcpiPm(VOID);
            STATIC XTAPI ULONGLONG QueryPerformanceCounterHpet(VOID);
            STATIC XTAPI ULONGLONG QueryPerformanceCounterPit(VOID);
            STATIC XTAPI ULONGLONG QueryPerformanceCounterTsc(VOID);
            STATIC XTAPI VOID QueryTimerCapabilities(VOID);
            STATIC XTAPI ULONG SetClockRateApic(ULONG TargetIncrement);
            STATIC XTAPI VOID StallExecutionAcpiPm(IN ULONG MicroSeconds);
            STATIC XTAPI VOID StallExecutionHpet(IN ULONG MicroSeconds);
            STATIC XTAPI VOID StallExecutionPit(IN ULONG MicroSeconds);
            STATIC XTAPI VOID StallExecutionTsc(IN ULONG MicroSeconds);
            STATIC XTAPI BOOLEAN ValidateTimerSupport(IN TIMER_TYPE TimerType,
                                                      IN BOOLEAN IsClock);
    };
}

#endif /* __XTOSKRNL_HL_TIMER_HH */
