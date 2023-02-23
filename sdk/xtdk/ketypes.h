/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/ketypes.h
 * DESCRIPTION:     XT kernel core structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_KETYPES_H
#define __XTDK_KETYPES_H

#include <xtbase.h>
#include <xtstruct.h>
#include <xttarget.h>
#include <xttypes.h>
#include ARCH_HEADER(ketypes.h)


/* Exception types and handling mechanisms */
#define EXCEPTION_CONTINUE_SEARCH                   0x00
#define EXCEPTION_EXECUTE_HANDLER                   0x01
#define EXCEPTION_CONTINUE_EXECUTION                0xFF

/* Maximum number of exception parameters */
#define EXCEPTION_MAXIMUM_PARAMETERS                15

/* APC pending state length */
#define KAPC_STATE_LENGTH                           (FIELD_OFFSET(KAPC_STATE, UserApcPending) + sizeof(BOOLEAN))

/* Kernel service descriptor tables count */
#define KSERVICE_TABLES_COUNT                       4

/* Timer length */
#define KTIMER_LENGTH                               (FIELD_OFFSET(KTIMER, Period) + sizeof(LONG))

/* Kernel builtin wait blocks */
#define EVENT_WAIT_BLOCK                            2
#define KTHREAD_WAIT_BLOCK                          3
#define KTIMER_WAIT_BLOCK                           3
#define SEMAPHORE_WAIT_BLOCK                        2

/* Adjust reason */
typedef enum _ADJUST_REASON
{
    AdjustNone = 0,
    AdjustUnwait = 1,
    AdjustBoost = 2
} ADJUST_REASON, *PADJUST_REASON;

/* Exception disposition return values */
typedef enum _EXCEPTION_DISPOSITION
{
    ExceptionContinueExecution,
    ExceptionContinueSearch,
    ExceptionNestedException,
    ExceptionCollidedUnwind,
} EXCEPTION_DISPOSITION, *PEXCEPTION_DISPOSITION;

/* APC environment types */
typedef enum _KAPC_ENVIRONMENT
{
    OriginalApcEnvironment,
    AttachedApcEnvironment,
    CurrentApcEnvironment,
    InsertApcEnvironment
} KAPC_ENVIRONMENT, *PKAPC_ENVIRONMENT;

/* Kernel objects */
typedef enum _KOBJECTS
{
    EventNotificationObject = 0,
    EventSynchronizationObject = 1,
    MutantObject = 2,
    ProcessObject = 3,
    QueueObject = 4,
    SemaphoreObject = 5,
    ThreadObject = 6,
    GateObject = 7,
    TimerNotificationObject = 8,
    TimerSynchronizationObject = 9,
    Spare2Object = 10,
    Spare3Object = 11,
    Spare4Object = 12,
    Spare5Object = 13,
    Spare6Object = 14,
    Spare7Object = 15,
    Spare8Object = 16,
    Spare9Object = 17,
    ApcObject = 18,
    DpcObject = 19,
    DeviceQueueObject = 20,
    EventPairObject = 21,
    InterruptObject = 22,
    ProfileObject = 23,
    ThreadedDpcObject = 24,
    MaximumKernelObject = 25
} KOBJECTS, *PKOBJECTS;

/* Thread state */
typedef enum _KTHREAD_STATE
{
    Initialized,
    Ready,
    Running,
    Standby,
    Terminated,
    Waiting,
    Transition,
    DeferredReady
} KTHREAD_STATE, *PKTHREAD_STATE;

/* Timer type */
typedef enum _KTIMER_TYPE
{
    NotificationTimer,
    SynchronizationTimer
} KTIMER_TYPE, *PKTIMER_TYPE;

/* APC Types */
typedef enum _MODE
{
    KernelMode,
    UserMode,
    MaximumMode
} MODE, *PMODE;

/* Wait type */
typedef enum _WAIT_TYPE
{
    WaitAll,
    WaitAny
} WAIT_TYPE, *PWAIT_TYPE;

/* Kernel routine callbacks */
typedef EXCEPTION_DISPOSITION (*PEXCEPTION_ROUTINE)(IN PEXCEPTION_RECORD ExceptionRecord, IN PVOID EstablisherFrame, IN OUT PCONTEXT ContextRecord, IN OUT PVOID DispatcherContext);
typedef VOID (*PKNORMAL_ROUTINE)(IN PVOID NormalContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2);
typedef VOID (*PKKERNEL_ROUTINE)(IN PKAPC Apc, IN OUT PKNORMAL_ROUTINE *NormalRoutine, IN OUT PVOID *NormalContext, IN OUT PVOID *SystemArgument1, IN OUT PVOID *SystemArgument2);
typedef VOID (*PKRUNDOWN_ROUTINE)(IN PKAPC Apc);
typedef VOID (*PKSTART_ROUTINE)(IN PVOID StartContext);
typedef VOID (*PKSYSTEM_ROUTINE)(IN PKSTART_ROUTINE StartRoutine, IN PVOID StartContext);

/* Exception record structure definition */
typedef struct _EXCEPTION_RECORD
{
    XTSTATUS ExceptionCode;
    ULONG ExceptionFlags;
    PEXCEPTION_RECORD ExceptionRecord;
    PVOID ExceptionAddress;
    ULONG NumberParameters;
    ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

/* Asynchronous Procedure Call (APC) object structure definition */
typedef struct _KAPC
{
    UCHAR Type;
    UCHAR SpareByte0;
    UCHAR Size;
    UCHAR SpareByte1;
    ULONG SpareLong0;
    PKTHREAD Thread;
    LIST_ENTRY ApcListEntry;
    PKKERNEL_ROUTINE KernelRoutine;
    PKRUNDOWN_ROUTINE RundownRoutine;
    PKNORMAL_ROUTINE NormalRoutine;
    PVOID NormalContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
    CHAR ApcStateIndex;
    KPROCESSOR_MODE ApcMode;
    BOOLEAN Inserted;
} KAPC, *PKAPC;

/* Exception registration record  structure definition */
typedef struct _EXCEPTION_REGISTRATION_RECORD
{
    PEXCEPTION_REGISTRATION_RECORD Next;
    PEXCEPTION_ROUTINE Handler;
} EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD;

/* APC state structure definition */
typedef struct _KAPC_STATE
{
    LIST_ENTRY ApcListHead[MaximumMode];
    PKPROCESS Process;
    BOOLEAN KernelApcInProgress;
    BOOLEAN KernelApcPending;
    BOOLEAN UserApcPending;
} KAPC_STATE, *PKAPC_STATE;

/* Event gate structure definition */
typedef struct _KGATE
{
    DISPATCHER_HEADER Header;
} KGATE, *PKGATE;

/* Semaphore object structure definition */
typedef struct _KSEMAPHORE
{
    DISPATCHER_HEADER Header;
    LONG Limit;
} KSEMAPHORE, *PKSEMAPHORE;

/* Per processor lock queue structure definition */
typedef struct _KSPIN_LOCK_QUEUE
{
    PKSPIN_LOCK_QUEUE Next;
    PKSPIN_LOCK Lock;
} KSPIN_LOCK_QUEUE, *PKSPIN_LOCK_QUEUE;

/* Queue object structure definition */
typedef struct _KQUEUE
{
    DISPATCHER_HEADER Header;
    LIST_ENTRY EntryListHead;
    ULONG CurrentCount;
    ULONG MaximumCount;
    LIST_ENTRY ThreadListHead;
} KQUEUE, *PKQUEUE;

/* Kernel service table descriptor */
typedef struct _KSERVICE_DESCRIPTOR_TABLE
{
    PULONG_PTR Base;
    PULONG Count;
    ULONG Limit;
    PUCHAR Number;
} KSERVICE_DESCRIPTOR_TABLE, *PKSERVICE_DESCRIPTOR_TABLE;

/* Timer object structure definition */
typedef struct _KTIMER
{
    DISPATCHER_HEADER Header;
    ULARGE_INTEGER DueTime;
    LIST_ENTRY TimerListEntry;
    LONG Period;
} KTIMER, *PKTIMER;

/* Wait block structure definition */
typedef struct _KWAIT_BLOCK
{
    LIST_ENTRY WaitListEntry;
    PKTHREAD Thread;
    PVOID Object;
    PKWAIT_BLOCK *NextWaitBlock;
    USHORT WaitKey;
    UCHAR WaitType;
    UCHAR SpareByte;
    LONG SpareLong;
} KWAIT_BLOCK, *PKWAIT_BLOCK;

/* Process control block structure definition */
typedef struct _KPROCESS
{
    union
    {
        struct
        {
            LONG AutoAlignment:1;
            LONG DisableBoost:1;
            LONG DisableQuantum:1;
            LONG ReservedFlags:29;
        };
        LONG ProcessFlags;
    };
} KPROCESS, *PKPROCESS;

/* Thread control block structure definition */
typedef struct _KTHREAD
{
    DISPATCHER_HEADER Header;
    LIST_ENTRY MutantListHead;
    PVOID InitialStack;
    PVOID KernelStack;
    PVOID StackBase;
    PVOID StackLimit;
    KSPIN_LOCK ThreadLock;
    volatile UCHAR State;
    union
    {
        KAPC_STATE ApcState;
        struct
        {
            UCHAR ApcStateFill[KAPC_STATE_LENGTH];
            BOOLEAN ApcQueueable;
            VOLATILE UCHAR NextProcessor;
            VOLATILE UCHAR DeferredProcessor;
            UCHAR AdjustReason;
            SCHAR AdjustIncrement;
        };
    };
    KSPIN_LOCK ApcQueueLock;
    LONG_PTR WaitStatus;
    union
    {
        PKWAIT_BLOCK WaitBlockList;
        PKGATE GateObject;
    };
    BOOLEAN Alertable;
    BOOLEAN WaitNext;
    UCHAR WaitReason;
    SCHAR Priority;
    UCHAR StackSwap;
    VOLATILE UCHAR SwapBusy;
    BOOLEAN Alerted[MaximumMode];
    union
    {
        LIST_ENTRY WaitListEntry;
        SINGLE_LIST_ENTRY SwapListEntry;
    };
    PKQUEUE Queue;
    PVOID EnvironmentBlock;
    union
    {
        KTIMER Timer;
        struct
        {
            UCHAR TimerFill[KTIMER_LENGTH];
            union
            {
                struct
                {
                    LONG AutoAlignment:1;
                    LONG DisableBoost:1;
                    LONG ReservedFlags:30;
                };
                LONG ThreadFlags;
            };
        };
    };
    KWAIT_BLOCK WaitBlock[KTHREAD_WAIT_BLOCK + 1];
    LIST_ENTRY QueueListEntry;
    PKTRAP_FRAME TrapFrame;
    PVOID CallbackStack;
    PVOID ServiceTable;
    ULONG KernelLimit;
    UCHAR ApcStateIndex;
    BOOLEAN StackResident;
    PKPROCESS Process;
    PKAPC_STATE ApcStatePointer[2];
    KAPC_STATE SavedApcState;
    KAPC SuspendApc;
    KSEMAPHORE SuspendSemaphore;
} KTHREAD, *PKTHREAD;

#endif /* __XTDK_KEFUNCS_H */
