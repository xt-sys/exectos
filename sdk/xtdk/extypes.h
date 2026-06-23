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

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_EXTYPES_H */
