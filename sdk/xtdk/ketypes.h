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
#include ARCH_HEADER(xtstruct.h)


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

/* Quantum values */
#define READY_SKIP_QUANTUM                          2
#define THREAD_QUANTUM                              6

/* Thread priority levels */
#define THREAD_LOW_PRIORITY                         0
#define THREAD_LOW_REALTIME_PRIORITY                16
#define THREAD_HIGH_PRIORITY                        31
#define THREAD_MAXIMUM_PRIORITY                     32

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

/* DPC importance enumeration list */
typedef enum _KDPC_IMPORTANCE
{
    LowImportance,
    MediumImportance,
    HighImportance,
    MediumHighImportance
} KDPC_IMPORTANCE, *PKDPC_IMPORTANCE;

/* Event types list */
typedef enum _KEVENT_TYPE
{
    NotificationEvent,
    SynchronizationEvent
} KEVENT_TYPE, *PKEVENT_TYPE;

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

/* Process states */
typedef enum _KPROCESS_STATE
{
    ProcessInMemory,
    ProcessOutOfMemory,
    ProcessInTransition,
    ProcessOutTransition,
    ProcessInSwap,
    ProcessOutSwap
} KPROCESS_STATE, *PKPROCESS_STATE;

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

/* Spin lock queue levels */
typedef enum _KSPIN_LOCK_QUEUE_LEVEL
{
    DispatcherLock,
    ExpansionLock,
    PfnLock,
    SystemSpaceLock,
    VacbLock,
    MasterLock,
    NonPagedAllocPoolLock,
    IoCancelLock,
    WorkQueueLock,
    IoVpbLock,
    IoDatabaseLock,
    IoCompletionLock,
    FileSystemLock,
    AfdWorkQueueLock,
    BcbLock,
    NonPagedPoolLock,
    ReservedSystemLock,
    TimerTableLock,
    MaximumLock
} KSPIN_LOCK_QUEUE_LEVEL, *PKSPIN_LOCK_QUEUE_LEVEL;

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

/* System resource types */
typedef enum _SYSTEM_RESOURCE_TYPE
{
    SystemResourceInvalid,
    SystemResourceAcpi,
    SystemResourceFrameBuffer
} SYSTEM_RESOURCE_TYPE, *PSYSTEM_RESOURCE_TYPE;

/* Wait type */
typedef enum _WAIT_TYPE
{
    WaitAll,
    WaitAny
} WAIT_TYPE, *PWAIT_TYPE;

/* Kernel UBSAN data types enumeration list */
typedef enum _KUBSAN_DATA_TYPE
{
    DataTypeInt,
    DataTypeFloat,
    DataTypeUnknown = 0xFFFF
} KUBSAN_DATA_TYPE, *PKUBSAN_DATA_TYPE;

/* Kernel routine callbacks */
typedef EXCEPTION_DISPOSITION (XTCDECL *PEXCEPTION_ROUTINE)(IN PEXCEPTION_RECORD ExceptionRecord, IN PVOID EstablisherFrame, IN OUT PCONTEXT ContextRecord, IN OUT PVOID DispatcherContext);
typedef VOID (XTAPI *PKDEFERRED_ROUTINE)(IN PKDPC Dpc, IN PVOID DeferredContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2);
typedef VOID (XTAPI *PKNORMAL_ROUTINE)(IN PVOID NormalContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2);
typedef VOID (XTAPI *PKKERNEL_ROUTINE)(IN PKAPC Apc, IN OUT PKNORMAL_ROUTINE *NormalRoutine, IN OUT PVOID *NormalContext, IN OUT PVOID *SystemArgument1, IN OUT PVOID *SystemArgument2);
typedef VOID (XTAPI *PKRUNDOWN_ROUTINE)(IN PKAPC Apc);
typedef VOID (XTCDECL *PKSTART_ROUTINE)(IN PVOID StartContext);
typedef VOID (XTCDECL *PKSYSTEM_ROUTINE)(IN PKSTART_ROUTINE StartRoutine, IN PVOID StartContext);

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

/* Deferred Procedure Call (DPC) object structure definition */
typedef struct _KDPC
{
    UCHAR Type;
    UCHAR Importance;
    UCHAR Number;
    UCHAR Expedite;
    LIST_ENTRY DpcListEntry;
    PKDEFERRED_ROUTINE DeferredRoutine;
    PVOID DeferredContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
    PVOID DpcData;
} KDPC, *PKDPC;

/* DPC data structure definition */
typedef struct _KDPC_DATA
{
    LIST_ENTRY DpcListHead;
    KSPIN_LOCK DpcLock;
    VOLATILE ULONG DpcQueueDepth;
    ULONG DpcCount;
} KDPC_DATA, *PKDPC_DATA;

/* Event object structure definition */
typedef struct _KEVENT
{
    DISPATCHER_HEADER Header;
} KEVENT, *PKEVENT;

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

/* Per processor lock queue handle structure definition */
typedef struct _KLOCK_QUEUE_HANDLE
{
    KSPIN_LOCK_QUEUE LockQueue;
    KRUNLEVEL OldRunLevel;
} KLOCK_QUEUE_HANDLE, *PKLOCK_QUEUE_HANDLE;

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
    PKDPC Dpc;
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

/* XT Thread Information Block (TIB) definition */
typedef struct _THREAD_INFORMATION_BLOCK
{
    PEXCEPTION_REGISTRATION_RECORD ExceptionList;
    PVOID StackBase;
    PVOID StackLimit;
    PVOID SubSystemTib;
    PVOID ArbitraryUserPointer;
    PTHREAD_INFORMATION_BLOCK Self;
} THREAD_INFORMATION_BLOCK, *PTHREAD_INFORMATION_BLOCK;

/* Process control block structure definition */
typedef struct _KPROCESS
{
    DISPATCHER_HEADER Header;
    LIST_ENTRY ProfileListHead;
    ULONG_PTR DirectoryTable[2];
    USHORT IopmOffset;
    UCHAR Iopl;
    VOLATILE KAFFINITY ActiveProcessors;
    ULONG KernelTime;
    ULONG UserTime;
    LIST_ENTRY ReadyListHead;
    SINGLE_LIST_ENTRY SwapListEntry;
    PVOID VdmTrapHandler;
    LIST_ENTRY ThreadListHead;
    KSPIN_LOCK ProcessLock;
    KAFFINITY Affinity;
    union
    {
        struct
        {
            BOOLEAN AutoAlignment;
            BOOLEAN DisableBoost;
            BOOLEAN DisableQuantum;
            LONG ReservedFlags:29;
        };
        LONG ProcessFlags;
    };
    ULONG_PTR StackCount;
    SCHAR BasePriority;
    SCHAR Quantum;
    UCHAR State;
    UCHAR ThreadSeed;
    UCHAR PowerState;
    UCHAR IdealNode;
    UCHAR Spare;
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

    ULONG ContextSwitches;
    VOLATILE UCHAR State;
    UCHAR NpxState;
    KRUNLEVEL WaitRunLevel;
    KPROCESSOR_MODE WaitMode;
    PTHREAD_ENVIRONMENT_BLOCK EnvironmentBlock;
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
    PKWAIT_BLOCK WaitBlockList;
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
    ULONG WaitTime;
    union
    {
        struct
        {
            SHORT KernelApcDisable;
            SHORT SpecialApcDisable;
        };
        ULONG CombinedApcDisable;
    };
    KTIMER Timer;
    KWAIT_BLOCK WaitBlock[KTHREAD_WAIT_BLOCK + 1];
    LIST_ENTRY QueueListEntry;
    UCHAR ApcStateIndex;
    BOOLEAN Preempted;
    BOOLEAN ProcessReadyQueue;
    BOOLEAN KernelStackResident;
    CHAR Saturation;
    UCHAR IdealProcessor;
    SCHAR BasePriority;
    UCHAR Spare4;
    SCHAR PriorityDecrement;
    SCHAR Quantum;
    BOOLEAN SystemAffinityActive;
    CHAR PreviousMode;
    UCHAR ResourceIndex;
    UCHAR DisableBoost;
    KAFFINITY UserAffinity;
    PKPROCESS Process;
    KAFFINITY Affinity;
    PVOID ServiceTable;
    PKAPC_STATE ApcStatePointer[2];
    KAPC_STATE SavedApcState;
    PVOID CallbackStack;
    PVOID SubSystemThread;
    PKTRAP_FRAME TrapFrame;
    ULONG KernelTime;
    ULONG UserTime;
    KAPC SuspendApc;
    KSEMAPHORE SuspendSemaphore;
    PVOID TlsArray;
    PVOID LegoData;
    LIST_ENTRY ThreadListEntry;
    UCHAR LargeStack;
    UCHAR PowerState;
    UCHAR NpxIrql;
    UCHAR Spare5;
    BOOLEAN AutoAlignment;
    UCHAR Iopl;
    CCHAR FreezeCount;
    CCHAR SuspendCount;
    UCHAR Spare0[1];
    UCHAR UserIdealProcessor;
    UCHAR Spare2[3];
    ULONG KernelLimit;
    BOOLEAN StackResident;
} KTHREAD, *PKTHREAD;

/* System resource common header structure definition */
typedef struct _SYSTEM_RESOURCE_HEADER
{
    LIST_ENTRY ListEntry;
    SYSTEM_RESOURCE_TYPE ResourceType;
    BOOLEAN ResourceLocked;
    ULONG ResourceSize;
    PVOID PhysicalAddress;
    PVOID VirtualAddress;
} SYSTEM_RESOURCE_HEADER, *PSYSTEM_RESOURCE_HEADER;

/* ACPI system resource structure definition */
typedef struct _SYSTEM_RESOURCE_ACPI
{
    SYSTEM_RESOURCE_HEADER Header;
    PVOID ApicBase;
} SYSTEM_RESOURCE_ACPI, *PSYSTEM_RESOURCE_ACPI;

/* FrameBuffer system resource structure definition */
typedef struct _SYSTEM_RESOURCE_FRAMEBUFFER
{
    SYSTEM_RESOURCE_HEADER Header;
    ULONG_PTR BufferSize;
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT PixelsPerScanLine;
    UINT BitsPerPixel;
    UINT Pitch;
    PVOID Font;
    struct
    {
        USHORT BlueShift;
        USHORT BlueSize;
        USHORT GreenShift;
        USHORT GreenSize;
        USHORT RedShift;
        USHORT RedSize;
        USHORT ReservedShift;
        USHORT ReservedSize;
    } Pixels;
} SYSTEM_RESOURCE_FRAMEBUFFER, *PSYSTEM_RESOURCE_FRAMEBUFFER;

/* Kernel UBSAN source location structure definition */
typedef struct _KUBSAN_SOURCE_LOCATION
{
    PCCHAR FileName;
    union
    {
        ULONG Reported;
        struct
        {
            UINT Line;
            UINT Column;
        };
    };
} KUBSAN_SOURCE_LOCATION, *PKUBSAN_SOURCE_LOCATION;

/* Kernel UBSAN type descriptor structure definition */
typedef struct _KUBSAN_TYPE_DESCRIPTOR
{
    USHORT DataType;
    USHORT TypeInfo;
    CHAR TypeName[1];
} KUBSAN_TYPE_DESCRIPTOR, *PKUBSAN_TYPE_DESCRIPTOR;

/* Kernel UBSAN float cast overflow data structure definition */
typedef struct _KUBSAN_FLOAT_CAST_OVERFLOW_DATA
{
    KUBSAN_SOURCE_LOCATION Location;
    PKUBSAN_TYPE_DESCRIPTOR LhsType;
    PKUBSAN_TYPE_DESCRIPTOR RhsType;
} KUBSAN_FLOAT_CAST_OVERFLOW_DATA, *PKUBSAN_FLOAT_CAST_OVERFLOW_DATA;

/* Kernel UBSAN function type mismatch data structure definition */
typedef struct _KUBSAN_FUNCTION_TYPE_MISMATCH_DATA
{
    KUBSAN_SOURCE_LOCATION Location;
    PKUBSAN_TYPE_DESCRIPTOR Type;
} KUBSAN_FUNCTION_TYPE_MISMATCH_DATA, *PKUBSAN_FUNCTION_TYPE_MISMATCH_DATA;

/* Kernel UBSAN invalid builtin data structure definition */
typedef struct _KUBSAN_INVALID_BUILTIN_DATA
{
    KUBSAN_SOURCE_LOCATION Location;
    UCHAR Kind;
} KUBSAN_INVALID_BUILTIN_DATA, *PKUBSAN_INVALID_BUILTIN_DATA;

/* Kernel UBSAN shift out of bounds data structure definition */
typedef struct _KUBSAN_SHIFT_OUT_OF_BOUNDS_DATA
{
    KUBSAN_SOURCE_LOCATION Location;
    PKUBSAN_TYPE_DESCRIPTOR LhsType;
    PKUBSAN_TYPE_DESCRIPTOR RhsType;
} KUBSAN_SHIFT_OUT_OF_BOUNDS_DATA, *PKUBSAN_SHIFT_OUT_OF_BOUNDS_DATA;

/* Kernel UBSAN out of bounds data structure definition */
typedef struct _KUBSAN_OUT_OF_BOUNDS_DATA
{
    KUBSAN_SOURCE_LOCATION Location;
    PKUBSAN_TYPE_DESCRIPTOR ArrayType;
    PKUBSAN_TYPE_DESCRIPTOR IndexType;
} KUBSAN_OUT_OF_BOUNDS_DATA, *PKUBSAN_OUT_OF_BOUNDS_DATA;

/* Kernel UBSAN overflow data structure definition */
typedef struct _KUBSAN_OVERFLOW_DATA
{
    KUBSAN_SOURCE_LOCATION Location;
    PKUBSAN_TYPE_DESCRIPTOR Type;
} KUBSAN_OVERFLOW_DATA, *PKUBSAN_OVERFLOW_DATA;

/* Kernel UBSAN type mismatch data structure definition */
typedef struct _KUBSAN_TYPE_MISMATCH_DATA
{
    KUBSAN_SOURCE_LOCATION Location;
    PKUBSAN_TYPE_DESCRIPTOR Type;
    ULONG Alignment;
    UCHAR TypeCheckKind;
} KUBSAN_TYPE_MISMATCH_DATA, *PKUBSAN_TYPE_MISMATCH_DATA;

/* Kernel UBSAN type mismatch data structure definition */
typedef struct _KUBSAN_TYPE_MISMATCH_DATA_V1
{
    KUBSAN_SOURCE_LOCATION Location;
    PKUBSAN_TYPE_DESCRIPTOR Type;
    UCHAR LogAlignment;
    UCHAR TypeCheckKind;
} KUBSAN_TYPE_MISMATCH_DATA_V1, *PKUBSAN_TYPE_MISMATCH_DATA_V1;

#endif /* __XTDK_KEFUNCS_H */
