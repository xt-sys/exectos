/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/pstypes.h
 * DESCRIPTION:     Process-related data structure definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_PSTYPES_H
#define __XTDK_PSTYPES_H

#include <xttypes.h>
#include <xtstruct.h>
#include <extypes.h>
#include <iotypes.h>
#include <mmtypes.h>
#include <setypes.h>


/* Quota bypass marker */
#define PS_QUOTA_BYPASS_MARKER                              ((PEPROCESS_QUOTA_BLOCK)1)

/* Current process & thread handles */
#define PS_CURRENT_PROCESS_HANDLE                           ((HANDLE)(LONG_PTR)-1)
#define PS_CURRENT_THREAD_HANDLE                            ((HANDLE)(LONG_PTR)-2)

/* Generic access rights mapping for process */
#define PS_PROCESS_GENERIC_MAPPING                          {SE_STANDARD_RIGHTS_READ | \
                                                             SE_PROCESS_QUERY_INFORMATION | \
                                                             SE_PROCESS_VM_READ, \
                                                             SE_STANDARD_RIGHTS_WRITE | \
                                                             SE_PROCESS_CREATE_PROCESS | \
                                                             SE_PROCESS_CREATE_THREAD | \
                                                             SE_PROCESS_DUP_HANDLE | \
                                                             SE_PROCESS_SET_INFORMATION | \
                                                             SE_PROCESS_SET_QUOTA | \
                                                             SE_PROCESS_SUSPEND_RESUME | \
                                                             SE_PROCESS_TERMINATE | \
                                                             SE_PROCESS_VM_OPERATION | \
                                                             SE_PROCESS_VM_WRITE, \
                                                             SE_STANDARD_RIGHTS_EXECUTE | \
                                                             SE_PROCESS_ALL_ACCESS | \
                                                             SE_SYNCHRONIZE} \

/* Generic access rights mapping for thread */
#define PS_THREAD_GENERIC_MAPPING                           {SE_STANDARD_RIGHTS_READ | \
                                                             SE_THREAD_GET_CONTEXT | \
                                                             SE_THREAD_QUERY_INFORMATION, \
                                                             SE_STANDARD_RIGHTS_WRITE | \
                                                             SE_THREAD_ALERT | \
                                                             SE_THREAD_SET_CONTEXT | \
                                                             SE_THREAD_SET_INFORMATION | \
                                                             SE_THREAD_SUSPEND_RESUME | \
                                                             SE_THREAD_TERMINATE, \
                                                             SE_STANDARD_RIGHTS_EXECUTE | \
                                                             SE_THREAD_ALL_ACCESS | \
                                                             SE_SYNCHRONIZE}

/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* Process quota types */
typedef enum _PS_QUOTA_TYPE
{
    PsNonPagedPool,
    PsPagedPool,
    PsPageFile,
    PsQuotaTypes
} PS_QUOTA_TYPE, *PPS_QUOTA_TYPE;

/* Kernel's representation of a process object */
typedef struct _EPROCESS
{
    KPROCESS ProcessControlBlock;
    KPUSH_LOCK ProcessLock;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER ExitTime;
    EX_RUNDOWN_REFERENCE RundownProtect;
    HANDLE UniqueProcessId;
    LIST_ENTRY ActiveProcessLinks;
    SIZE_T QuotaUsage[PsQuotaTypes];
    SIZE_T QuotaPeak[PsQuotaTypes];
    PVOID DebugPort;
    PVOID ExceptionPort;
    PHANDLE_TABLE ObjectTable;
    PVOID Job;
    PVOID SectionObject;
    PVOID SectionBaseAddress;
    PEPROCESS_QUOTA_BLOCK QuotaBlock;
    HANDLE InheritedFromUniqueProcessId;
    PVOID DeviceMap;
    CHAR ImageFileName[16];
    LIST_ENTRY ThreadListHead;
    ACCESS_MASK GrantedAccess;
    ULONG DefaultHardErrorProcessing;
    PPEB ProcessEnvironmentBlock;
    SECURITY_AUDIT_PROCESS_CREATION_INFO AuditProcessCreationInfo;
    MMSUPPORT Vm;
    union
    {
        struct
        {
            ULONG CreateReported:1;
            ULONG NoDebugInherit:1;
            ULONG ProcessExiting:1;
            ULONG ProcessDelete:1;
            ULONG Reserved1:1;
            ULONG VmDeleted:1;
            ULONG OutswapEnabled:1;
            ULONG Outswapped:1;
            ULONG ForkFailed:1;
            ULONG Reserved2:1;
            ULONG AddressSpaceInitialized:2;
            ULONG SetTimerResolution:1;
            ULONG BreakOnTermination:1;
            ULONG DeprioritizeViews:1;
            ULONG WriteWatch:1;
            ULONG ProcessInSession:1;
            ULONG OverrideAddressSpace:1;
            ULONG HasAddressSpace:1;
            ULONG LaunchPrefetched:1;
            ULONG InjectInpageErrors:1;
            ULONG VmTopDown:1;
            ULONG ImageNotifyDone:1;
            ULONG PdeUpdateNeeded:1;
            ULONG Reserved3:1;
            ULONG SmapAllowed:1;
            ULONG ProcessInserted:1;
            ULONG DefaultIoPriority:3;
            ULONG Reserved4:1;
            ULONG Reserved5:1;
        };
        ULONG Flags;
    };
    XTSTATUS ExitStatus;
    UCHAR PriorityClass;
} EPROCESS, *PEPROCESS;

/* Kernel's representation of a process quota entry*/
typedef struct _EPROCESS_QUOTA_ENTRY
{
    SIZE_T Usage;
    SIZE_T Limit;
    SIZE_T Peak;
    SIZE_T Return;
} EPROCESS_QUOTA_ENTRY, *PEPROCESS_QUOTA_ENTRY;

/* Kernel's representation of a process quota block */
typedef struct _EPROCESS_QUOTA_BLOCK
{
    EPROCESS_QUOTA_ENTRY QuotaEntry[PsQuotaTypes];
    LIST_ENTRY QuotaList;
    ULONG ReferenceCount;
    ULONG ProcessCount;
} EPROCESS_QUOTA_BLOCK, *PEPROCESS_QUOTA_BLOCK;

/* Kernel's representation of a thread object */
typedef struct _ETHREAD
{
    KTHREAD ThreadControlBlock;
    ACCESS_MASK GrantedAccess;
} ETHREAD, *PETHREAD;

/* Kernel's representation of a process environment block */
typedef struct _PEB
{
    BOOLEAN InheritedAddressSpace;
    BOOLEAN ReadImageFileExecOptions;
    BOOLEAN BeingDebugged;
    union
    {
        BOOLEAN BitField;
        struct
        {
            BOOLEAN ImageUsesLargePages:1;
            BOOLEAN IsProtectedProcess:1;
            BOOLEAN IsLegacyProcess:1;
            BOOLEAN IsImageDynamicallyRelocated:1;
            BOOLEAN SkipPatchingUser32Forwarders:1;
            BOOLEAN SpareBits:3;
        };
    };
    HANDLE Mutant;
    PVOID ImageBaseAddress;
    PLDR_PEB_DATA Ldr;
    PVOID SubSystemData;
    PVOID ProcessHeap;
    PVOID AltThunkSListPtr;
    PVOID SparePtr2;
    ULONG EnvironmentUpdateCount;
    PVOID KernelCallbackTable;
    ULONG SystemReserved[1];
    ULONG SpareUlong;
    PPEB_FREE_BLOCK FreeList;
    ULONG TlsExpansionCounter;
    PVOID TlsBitmap;
    ULONG TlsBitmapBits[2];
    PVOID ReadOnlySharedMemoryBase;
    PVOID HotpatchInformation;
    PVOID *ReadOnlyStaticServerData;
    PVOID AnsiCodePageData;
    PVOID OemCodePageData;
    PVOID UnicodeCaseTableData;
    ULONG NumberOfProcessors;
    ULONG GlobalFlag;
    LARGE_INTEGER CriticalSectionTimeout;
    ULONG_PTR HeapSegmentReserve;
    ULONG_PTR HeapSegmentCommit;
    ULONG_PTR HeapDeCommitTotalFreeThreshold;
    ULONG_PTR HeapDeCommitFreeBlockThreshold;
    ULONG NumberOfHeaps;
    ULONG MaximumNumberOfHeaps;
    PVOID *ProcessHeaps;
    PVOID ProcessStarterHelper;
    ULONG ImageSubsystem;
    ULONG ImageSubsystemMajorVersion;
    ULONG ImageSubsystemMinorVersion;
    ULONG_PTR ImageProcessAffinityMask;
    PVOID TlsExpansionBitmap;
    ULONG TlsExpansionBitmapBits[32];
    ULONG SessionId;
    ULARGE_INTEGER AppCompatFlags;
    ULARGE_INTEGER AppCompatFlagsUser;
    PVOID pShimData;
    PVOID AppCompatInfo;
    UNICODE_STRING CSDVersion;
    ULONG_PTR MinimumStackCommit;
    PVOID *FlsCallback;
    LIST_ENTRY FlsListHead;
    PVOID FlsBitmap;
    ULONG FlsBitmapBits[4];
    ULONG FlsHighIndex;
} PEB, *PPEB;

/* Process Environment Block Free Block structure definition */
typedef struct _PEB_FREE_BLOCK
{
    PPEB_FREE_BLOCK Next;
    ULONG Size;
} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;

/* Kernel's representation of a thread environment block */
typedef struct _TEB
{
    THREAD_INFORMATION_BLOCK ThreadInformationBlock;
    PVOID EnvironmentPointer;
    CLIENT_ID ClientId;
    PVOID ActiveRpcHandle;
    PVOID ThreadLocalStoragePointer;
    PPEB ProcessEnvironmentBlock;
    ULONG LastErrorValue;
    ULONG CountOfOwnedCriticalSections;
    ULONG UserReserved[5];
    LCID CurrentLocale;
    ULONG FpSoftwareStatusRegister;
    PVOID SystemReserved1[54];
    LONG ExceptionCode;
    PACTIVATION_CONTEXT_STACK ActivationContextStackPointer;
    CLIENT_ID RealClientId;
    ULONG LastStatusValue;
    UNICODE_STRING StaticUnicodeString;
    WCHAR StaticUnicodeBuffer[261];
    PVOID DeallocationStack;
    PVOID TlsSlots[64];
    LIST_ENTRY TlsLinks;
    PVOID ReservedForNtRpc;
    PVOID DbgSsReserved[2];
    ULONG HardErrorMode;
    PVOID Instrumentation[14];
    PVOID SubProcessTag;
    PVOID EtwTraceData;
    PVOID WinSockData;
    BOOLEAN InDbgPrint;
    BOOLEAN FreeStackOnTermination;
    BOOLEAN HasFiberData;
    UCHAR IdealProcessor;
    ULONG GuaranteedStackBytes;
    PVOID ReservedForPerf;
    PVOID ReservedForOle;
    ULONG WaitingOnLoaderLock;
    ULONG_PTR SparePointer1;
    ULONG_PTR SoftPatchPtr1;
    ULONG_PTR SoftPatchPtr2;
    PVOID *TlsExpansionSlots;
    ULONG ImpersonationLocale;
    ULONG IsImpersonating;
    PVOID NlsCache;
    PVOID ShimData;
    ULONG HeapVirtualAffinity;
    HANDLE CurrentTransactionHandle;
    PTEB_ACTIVE_FRAME ActiveFrame;
    PVOID FlsData;
    BOOLEAN SafeThunkCall;
    BOOLEAN BooleanSpare[3];
} TEB, *PTEB;

/* Thread Environment Block Active Frame structure definition */
typedef struct _TEB_ACTIVE_FRAME
{
    ULONG Flags;
    PTEB_ACTIVE_FRAME Previous;
    PTEB_ACTIVE_FRAME_CONTEXT Context;
} TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME;

/* Thread Environment Block Active Frame Context structure definition */
typedef struct _TEB_ACTIVE_FRAME_CONTEXT
{
    ULONG Flags;
    LPSTR FrameName;
} TEB_ACTIVE_FRAME_CONTEXT, *PTEB_ACTIVE_FRAME_CONTEXT;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_PSTYPES_H */
