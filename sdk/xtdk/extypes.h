/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/extypes.h
 * DESCRIPTION:     Kernel executive structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_EXTYPES_H
#define __XTDK_EXTYPES_H

#include <xtbase.h>
#include <xttypes.h>
#include <ketypes.h>


/* Rundown protection flags */
#define EX_RUNDOWN_ACTIVE                               0x1

/* Number of lookaside lists */
#define POOL_LOOKASIDE_LISTS                            32

/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* Kernel routine callbacks */
typedef XTSTATUS (XTAPI *PALLOCATE_FUNCTION)(IN MMPOOL_TYPE PoolType, IN SIZE_T Bytes, OUT PVOID *Memory, IN ULONG Tag);
typedef XTSTATUS (XTAPI *PALLOCATE_FUNCTION_EX)(IN MMPOOL_TYPE PoolType, IN SIZE_T Bytes, OUT PVOID *Memory, IN ULONG Tag, IN PLOOKASIDE_LIST_EX Lookaside);
typedef XTSTATUS (XTAPI *PFREE_FUNCTION)(IN PVOID Buffer);
typedef XTSTATUS (XTAPI *PFREE_FUNCTION_EX)(IN PVOID Buffer, IN OUT PLOOKASIDE_LIST_EX Lookaside);
typedef VOID (XTAPI *PWORKER_THREAD_ROUTINE)(IN PVOID Parameter);

/* Work queue types */
typedef enum _WORK_QUEUE_TYPE
{
    CriticalWorkQueue,
    DelayedWorkQueue,
    HyperCriticalWorkQueue,
    NormalWorkQueue,
    BackgroundWorkQueue,
    RealTimeWorkQueue,
    SuperCriticalWorkQueue,
    MaximumWorkQueue,
} WORK_QUEUE_TYPE, *PWORK_QUEUE_TYPE;

/* Owner entry structure definition */
typedef struct _OWNER_ENTRY
{
    ULONG_PTR OwnerThread;
    union
    {
        struct
        {
            ULONG IoPriorityBoosted:1;
            ULONG OwnerReferenced:1;
            ULONG OwnerCount:30;
        };
        ULONG TableSize;
    };
} OWNER_ENTRY, *POWNER_ENTRY;

/* Exclusive resource structure definition */
typedef struct _ERESOURCE
{
    LIST_ENTRY SystemResourcesList;
    POWNER_ENTRY OwnerTable;
    SHORT ActiveCount;
    USHORT Flag;
    VOLATILE PKSEMAPHORE SharedWaiters;
    VOLATILE PKEVENT ExclusiveWaiters;
    OWNER_ENTRY OwnerEntry;
    ULONG ActiveEntries;
    ULONG ContentionCount;
    ULONG NumberOfSharedWaiters;
    ULONG NumberOfExclusiveWaiters;
    union
    {
        PVOID Address;
        ULONG_PTR CreatorBackTraceIndex;
    };
    KSPIN_LOCK SpinLock;
} ERESOURCE, *PERESOURCE;

/* Executive rundown protection structure definition */
typedef union _EX_RUNDOWN_REFERENCE
{
    ULONG_PTR Count;
    PVOID Ptr;
} EX_RUNDOWN_REFERENCE, *PEX_RUNDOWN_REFERENCE;

/* Executive rundown wait block definition */
typedef struct _EX_RUNDOWN_WAIT_BLOCK
{
    ULONG_PTR Count;
    KEVENT WakeEvent;
} EX_RUNDOWN_WAIT_BLOCK, *PEX_RUNDOWN_WAIT_BLOCK;

/* Lookaside list structure definition */
typedef struct _GENERAL_LOOKASIDE
{
    union
    {
        SINGLE_LIST_HEADER ListHead;
        SINGLE_LIST_ENTRY SingleListHead;
    };
    USHORT Depth;
    USHORT MaximumDepth;
    ULONG TotalAllocates;
    union
    {
        ULONG AllocateMisses;
        ULONG AllocateHits;
    };
    ULONG TotalFrees;
    union {
        ULONG FreeMisses;
        ULONG FreeHits;
    };
    MMPOOL_TYPE Type;
    ULONG Tag;
    ULONG Size;
    union
    {
        PALLOCATE_FUNCTION_EX AllocateEx;
        PALLOCATE_FUNCTION Allocate;
    };
    union
    {
        PFREE_FUNCTION_EX FreeEx;
        PFREE_FUNCTION Free;
    };
    LIST_ENTRY ListEntry;
    ULONG LastTotalAllocates;
    union
    {
        ULONG LastAllocateMisses;
        ULONG LastAllocateHits;
    };
    ULONG Future[2];
} GENERAL_LOOKASIDE, *PGENERAL_LOOKASIDE;

/* Handle table entry structure definition */
typedef struct _HANDLE_TABLE_ENTRY
{
    union
    {
        PVOID Object;
        ULONG_PTR ObAttributes;
        PHANDLE_TABLE_ENTRY_INFO InfoTable;
        ULONG_PTR Value;
    };
    union
    {
        ULONG GrantedAccess;
        struct
        {
            USHORT GrantedAccessIndex;
            USHORT CreatorBackTraceIndex;
        };
        LONG NextFreeTableEntry;
    };
} HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;

/* Handle table entry info structure definition */
typedef struct _HANDLE_TABLE_ENTRY_INFO
{
    ULONG AuditMask;
} HANDLE_TABLE_ENTRY_INFO, *PHANDLE_TABLE_ENTRY_INFO;

/* Handle table structure definition */
typedef struct _HANDLE_TABLE
{
    ULONG_PTR TableCode;
    PEPROCESS QuotaProcess;
    PVOID UniqueProcessId;
    KPUSH_LOCK HandleTableLock[4];
    LIST_ENTRY HandleTableList;
    KPUSH_LOCK HandleContentionEvent;
    PVOID Reserved;
    LONG ExtraInfoPages;
    union
    {
        ULONG Flags;
        UCHAR StrictFIFO:1;
    };
    PHANDLE_TABLE_ENTRY FirstFreeHandle;
    PHANDLE_TABLE_ENTRY LastFreeHandle;
    LONG HandleCount;
    ULONG NextHandleNeedingPool;
} HANDLE_TABLE, *PHANDLE_TABLE;

/* Lookaside list pointers structure definition */
typedef struct _LOOKASIDE_LIST
{
    PGENERAL_LOOKASIDE Local;
    PGENERAL_LOOKASIDE Global;
} LOOKASIDE_LIST, *PLOOKASIDE_LIST;

/* Lookaside list extended structure definition */
typedef struct _LOOKASIDE_LIST_EX
{
  GENERAL_LOOKASIDE Global;
} LOOKASIDE_LIST_EX, *PLOOKASIDE_LIST_EX;

/* Non-paged lookaside list structure definition */
typedef struct _NONPAGED_LOOKASIDE_LIST
{
    GENERAL_LOOKASIDE Global;
} NONPAGED_LOOKASIDE_LIST, *PNONPAGED_LOOKASIDE_LIST;

/* Paged lookaside list structure definition */
typedef struct _PAGED_LOOKASIDE_LIST
{
    GENERAL_LOOKASIDE Global;
} PAGED_LOOKASIDE_LIST, *PPAGED_LOOKASIDE_LIST;

/* Work queue item structure definition */
typedef struct _WORK_QUEUE_ITEM
{
    LIST_ENTRY List;
    PWORKER_THREAD_ROUTINE WorkerRoutine;
    VOLATILE PVOID Parameter;
} WORK_QUEUE_ITEM, *PWORK_QUEUE_ITEM;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_EXTYPES_H */
