/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/potypes.h
 * DESCRIPTION:     Power manager subsystem structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_POTYPES_H
#define __XTDK_POTYPES_H

#include <xttypes.h>
#include <xtstruct.h>
#include <ketypes.h>


/* Power Manager routine callbacks */
typedef VOID (XTFASTCALL *PPROCESSOR_IDLE_FUNCTION)(IN PPROCESSOR_POWER_STATE PowerState);
typedef XTSTATUS (XTFASTCALL *PSET_PROCESSOR_THROTTLE)(IN UCHAR Throttle);

/* Processor IDLE times structure definition */
typedef struct _PROCESSOR_IDLE_TIMES
{
    ULONGLONG StartTime;
    ULONGLONG EndTime;
    ULONG IdleHandlerReserved[4];
} PROCESSOR_IDLE_TIMES, *PPROCESSOR_IDLE_TIMES;

/* Processor performance state structure definition */
typedef struct _PROCESSOR_PERF_STATE
{
    UCHAR PercentFrequency;
    UCHAR MinCapacity;
    USHORT Power;
    UCHAR IncreaseLevel;
    UCHAR DecreaseLevel;
    USHORT Flags;
    ULONG IncreaseTime;
    ULONG DecreaseTime;
    ULONG IncreaseCount;
    ULONG DecreaseCount;
    ULONGLONG PerformanceTime;
} PROCESSOR_PERF_STATE, *PPROCESSOR_PERF_STATE;

/* Processor power state structure definition */
typedef struct _PROCESSOR_POWER_STATE
{
    PPROCESSOR_IDLE_FUNCTION IdleFunction;
    ULONG Idle0TimeLimit;
    ULONG Idle0LastTime;
    PVOID IdleHandlers;
    PVOID IdleState;
    ULONG IdleHandlersCount;
    ULONGLONG LastCheck;
    PROCESSOR_IDLE_TIMES IdleTimes;
    ULONG IdleTime1;
    ULONG PromotionCheck;
    ULONG IdleTime2;
    UCHAR CurrentThrottle;
    UCHAR ThermalThrottleLimit;
    UCHAR CurrentThrottleIndex;
    UCHAR ThermalThrottleIndex;
    ULONG PerfSystemTime;
    ULONG PerfIdleTime;
    ULONG LastSysTime;
    ULONGLONG TotalIdleStateTime[3];
    ULONG TotalIdleTransitions[3];
    ULONGLONG PreviousC3StateTime;
    UCHAR KneeThrottleIndex;
    UCHAR ThrottleLimitIndex;
    UCHAR PerfStatesCount;
    UCHAR ProcessorMinThrottle;
    UCHAR ProcessorMaxThrottle;
    UCHAR LastBusyPercentage;
    UCHAR LastC3Percentage;
    UCHAR LastAdjustedBusyPercentage;
    ULONG PromotionCount;
    ULONG DemotionCount;
    ULONG ErrorCount;
    ULONG RetryCount;
    ULONG Flags;
    LARGE_INTEGER PerfCounterFrequency;
    ULONG PerfTickCount;
    KTIMER PerfTimer;
    KDPC PerfDpc;
    PPROCESSOR_PERF_STATE PerfStates;
    PSET_PROCESSOR_THROTTLE PerfSetThrottle;
    ULONG LastC3UserTime;
} PROCESSOR_POWER_STATE, *PPROCESSOR_POWER_STATE;

#endif /* __XTDK_POTYPES_H */
