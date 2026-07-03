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


/* Processor performance scale factor */
#define POWER_PERFORMANCE_SCALE                     100


/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* Power Manager routine callbacks */
typedef VOID (XTFASTCALL *PPROCESSOR_IDLE_FUNCTION)(IN PPROCESSOR_POWER_STATE PowerState);
typedef XTSTATUS (XTFASTCALL *PSET_PROCESSOR_THROTTLE)(IN UCHAR Throttle);

/* Device power states */
typedef enum _DEVICE_POWER_STATE
{
    PowerDeviceUnspecified = 0,
    PowerDeviceD0,
    PowerDeviceD1,
    PowerDeviceD2,
    PowerDeviceD3,
    PowerDeviceMaximum
} DEVICE_POWER_STATE, *PDEVICE_POWER_STATE;

/* Power actions list */
typedef enum _POWER_ACTION
{
    PowerActionNone = 0,
    PowerActionReserved,
    PowerActionSleep,
    PowerActionHibernate,
    PowerActionShutdown,
    PowerActionShutdownReset,
    PowerActionShutdownOff,
    PowerActionWarmEject
} POWER_ACTION, *PPOWER_ACTION;

typedef enum _POWER_STATE_TYPE
{
    SystemPowerState = 0,
    DevicePowerState
} POWER_STATE_TYPE, *PPOWER_STATE_TYPE;

/* System power states */
typedef enum _SYSTEM_POWER_STATE
{
    PowerSystemUnspecified = 0,
    PowerSystemWorking,
    PowerSystemSleeping1,
    PowerSystemSleeping2,
    PowerSystemSleeping3,
    PowerSystemHibernate,
    PowerSystemShutdown,
    PowerSystemMaximum
} SYSTEM_POWER_STATE, *PSYSTEM_POWER_STATE;

/* Power state union definition */
typedef union _POWER_STATE
{
    SYSTEM_POWER_STATE SystemState;
    DEVICE_POWER_STATE DeviceState;
} POWER_STATE, *PPOWER_STATE;

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

/* System power state context structure definition */
typedef struct _SYSTEM_POWER_STATE_CONTEXT
{
    union
    {
        struct
        {
            ULONG Reserved1:8;
            ULONG TargetSystemState:4;
            ULONG EffectiveSystemState:4;
            ULONG CurrentSystemState:4;
            ULONG IgnoreHibernationPath:1;
            ULONG PseudoTransition:1;
            ULONG Reserved2:10;
        };
        ULONG ContextAsUlong;
    };
} SYSTEM_POWER_STATE_CONTEXT, *PSYSTEM_POWER_STATE_CONTEXT;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_POTYPES_H */
