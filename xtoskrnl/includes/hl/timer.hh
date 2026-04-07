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
            STATIC ULONG ProfilingInterval;
            STATIC ULONG TimerFrequency;

        public:
            STATIC XTAPI VOID InitializeApicTimer(VOID);
            STATIC XTAPI ULONG_PTR SetProfileInterval(IN ULONG_PTR Interval);
            STATIC XTAPI VOID StartProfileInterrupt(IN KPROFILE_SOURCE ProfileSource);
            STATIC XTAPI VOID StopProfileInterrupt(IN KPROFILE_SOURCE ProfileSource);

        private:
            STATIC XTAPI XTSTATUS CalibrateApicTimer();
            STATIC XTAPI XTSTATUS GetApicTimerFrequency(OUT PULONG Frequency);
            STATIC XTAPI VOID PitStallExecution(IN ULONG Us);
            STATIC XTAPI VOID StallExecution(IN ULONG MicroSeconds);
    };
}

#endif /* __XTOSKRNL_HL_TIMER_HH */
