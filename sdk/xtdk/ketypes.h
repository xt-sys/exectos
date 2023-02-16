/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/ketypes.h
 * DESCRIPTION:     XT kernel core structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_KETYPES_H
#define __XTDK_KETYPES_H

#include <xtstruct.h>
#include <xttarget.h>
#include <xttypes.h>


/* Maximum number of exception parameters */
#define EXCEPTION_MAXIMUM_PARAMETERS 15

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

/* Kernel routine callbacks */
typedef EXCEPTION_DISPOSITION (*PEXCEPTION_ROUTINE)(IN PEXCEPTION_RECORD ExceptionRecord, IN PVOID EstablisherFrame, IN OUT PCONTEXT ContextRecord, IN OUT PVOID DispatcherContext);
typedef VOID (*PKNORMAL_ROUTINE)(IN PVOID NormalContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2);
typedef VOID (*PKKERNEL_ROUTINE)(IN PKAPC Apc, IN OUT PKNORMAL_ROUTINE *NormalRoutine, IN OUT PVOID *NormalContext, IN OUT PVOID *SystemArgument1, IN OUT PVOID *SystemArgument2);
typedef VOID (*PKRUNDOWN_ROUTINE)(IN PKAPC Apc);

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

/* Timer object structure definition */
typedef struct _KTIMER
{
    DISPATCHER_HEADER Header;
    ULARGE_INTEGER DueTime;
    LIST_ENTRY TimerListEntry;
    LONG Period;
} KTIMER, *PKTIMER;

/* Process control block structure definition */
typedef struct _KPROCESS
{
    INT PlaceHolder;
} KPROCESS, *PKPROCESS;

/* Thread control block structure definition */
typedef struct _KTHREAD
{
    DISPATCHER_HEADER Header;
    PVOID InitialStack;
    PVOID KernelStack;
    PVOID StackBase;
    PVOID StackLimit;
    KAPC_STATE ApcState;
} KTHREAD, *PKTHREAD;

#endif /* __XTDK_KEFUNCS_H */
