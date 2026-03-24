/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/mmtypes.h
 * DESCRIPTION:     Memory management data structures
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTDK_MMTYPES_H
#define __XTDK_MMTYPES_H

#include <xtbase.h>
#include ARCH_HEADER(xtstruct.h)


/* Number of hyper space pages */
#define MM_HYPERSPACE_PAGE_COUNT                   255

/* Number of free page list heads */
#define MM_MAX_FREE_PAGE_LIST_HEADS                4

/* Number of paging colors */
#define MM_PAGING_COLORS                           64

/* PTE frame mask definition */
#define MM_PFN_PTE_FRAME                           (((ULONG_PTR)1 << MM_PTE_FRAME_BITS) - 1)

/* Memory manager pool type mask definition */
#define MM_POOL_TYPE_MASK                          1

/* Bad pool caller reasons */
#define MM_POOL_INVALID_ALLOC_RUNLEVEL             8
#define MM_POOL_INVALID_FREE_RUNLEVEL              9

/* Pool flags */
#define MM_POOL_BIG_ALLOCATIONS_ENTRY_FREE         0x1
#define MM_POOL_PROTECTED                          0x80000000
#define MM_POOL_RAISE_EXCEPTION                    0x10

/* Number of reserved zeroed PTEs */
#define MM_RESERVED_ZERO_PTES                      32

/* Memory Manager Protection Bits */
#define MM_ZERO_ACCESS                             0
#define MM_READONLY                                1
#define MM_EXECUTE                                 2
#define MM_EXECUTE_READ                            3
#define MM_READWRITE                               4
#define MM_WRITECOPY                               5
#define MM_EXECUTE_READWRITE                       6
#define MM_EXECUTE_WRITECOPY                       7
#define MM_PROTECT_ACCESS                          7

/* Protection field shift */
#define MM_PROTECT_FIELD_SHIFT                     5

/* Memory manager page lists */
typedef enum _MMPAGELISTS
{
   ZeroedPageList = 0,
   FreePageList = 1,
   StandbyPageList = 2,
   ModifiedPageList = 3,
   ModifiedReadOnlyPageList = 4,
   BadPageList = 5,
   ActiveAndValid = 6,
   TransitionPage = 7
} MMPAGELISTS, *PMMPAGELISTS;

/* Page cache attributes */
typedef enum _MMPFN_CACHE_ATTRIBUTE
{
    PfnNonCached,
    PfnCached,
    PfnWriteCombined,
    PfnNotMapped
} MMPFN_CACHE_ATTRIBUTE, *PMMPFN_CACHE_ATTRIBUTE;

/* Memory Manager pool types */
typedef enum _MMPOOL_TYPE
{
    NonPagedPool = 0,
    PagedPool = 1,
    NonPagedPoolMustSucceed = 2,
    NonPagedPoolCacheAligned = 4,
    PagedPoolCacheAligned = 5,
    NonPagedPoolCacheAlignedMustSucceed = 6,
    MaxPoolType = 7,
    NonPagedPoolSession = 32,
    PagedPoolSession = 33,
    NonPagedPoolMustSucceedSession = 34,
    NonPagedPoolCacheAlignedSession = 36,
    PagedPoolCacheAlignedSession = 37,
    NonPagedPoolCacheAlignedMustSucceedSession = 38
} MMPOOL_TYPE, *PMMPOOL_TYPE;

/* Page table pool types */
typedef enum _MMSYSTEM_PTE_POOL_TYPE
{
    SystemPteSpace,
    NonPagedPoolExpansion,
    MaximumPtePoolTypes
} MMSYSTEM_PTE_POOL_TYPE, *PMMSYSTEM_PTE_POOL_TYPE;

/* Page map routines structure definition */
typedef CONST STRUCT _CMMPAGEMAP_ROUTINES
{
    VOID (XTAPI *ClearPte)(PHARDWARE_PTE PtePointer);
    BOOLEAN (XTAPI *PteValid)(PHARDWARE_PTE PtePointer);
    VOID (XTAPI *SetPteCaching)(PHARDWARE_PTE PtePointer, BOOLEAN CacheDisable, BOOLEAN WriteThrough);
    VOID (XTAPI *SetPte)(PHARDWARE_PTE PtePointer, PFN_NUMBER PageFrameNumber, BOOLEAN Writable);
} CMMPAGEMAP_ROUTINES, *PCMMPAGEMAP_ROUTINES;

/* Color tables structure definition */
typedef struct _MMCOLOR_TABLES
{
    ULONG_PTR Flink;
    PVOID Blink;
    ULONG_PTR Count;
} MMCOLOR_TABLES, *PMMCOLOR_TABLES;

/* Free pool entry structure definition */
typedef struct _MMFREE_POOL_ENTRY
{
    LIST_ENTRY List;
    PFN_COUNT Size;
    PMMFREE_POOL_ENTRY Owner;
} MMFREE_POOL_ENTRY, *PMMFREE_POOL_ENTRY;

/* Memory layout structure definition */
typedef struct _MMMEMORY_LAYOUT
{
    PMMPFN PfnDatabase;
    PFN_NUMBER PfnDatabaseSize;
    PVOID SelfMapAddress;
    PVOID HardwarePoolStart;
    PVOID HardwarePoolEnd;
    PVOID HyperSpaceStart;
    PVOID HyperSpaceEnd;
    PVOID LoaderMappingsStart;
    PVOID LoaderMappingsEnd;
    PFN_NUMBER LoaderMappingsSize;
    PVOID NonCanonicalStart;
    PVOID NonCanonicalEnd;
    PVOID NonPagedPoolStart;
    PVOID NonPagedPoolEnd;
    PFN_NUMBER NonPagedPoolSize;
    PVOID NonPagedExpansionPoolStart;
    PVOID NonPagedExpansionPoolEnd;
    PFN_NUMBER NonPagedExpansionPoolSize;
    PVOID NonPagedSystemPoolStart;
    PVOID NonPagedSystemPoolEnd;
    PFN_NUMBER NonPagedSystemPoolSize;
    PVOID PagedPoolStart;
    PVOID PagedPoolEnd;
    PFN_NUMBER PagedPoolSize;
    PVOID ReservedSystemPoolStart;
    PVOID ReservedSystemPoolEnd;
    PVOID SessionSpaceStart;
    PVOID SessionSpaceEnd;
    PFN_NUMBER SessionSpaceSize;
    PVOID SharedSystemPageStart;
    PVOID SharedSystemPageEnd;
    PVOID SystemCacheStart;
    PVOID SystemCacheEnd;
    PVOID SystemWorkingSetStart;
    PVOID SystemWorkingSetEnd;
    PVOID UserSpaceStart;
    PVOID UserSpaceEnd;
    PVOID PteSpaceStart;
    PVOID PteSpaceEnd;
} MMMEMORY_LAYOUT, *PMMMEMORY_LAYOUT;

/* Page Frame Entry structure definition */
typedef struct _MMPFNENTRY
{
    USHORT Modified:1;
    USHORT ReadInProgress:1;
    USHORT WriteInProgress:1;
    USHORT PrototypePte:1;
    USHORT PageColor:4;
    USHORT PageLocation:3;
    USHORT RemovalRequested:1;
    USHORT CacheAttribute:2;
    USHORT Rom:1;
    USHORT ParityError:1;
} MMPFNENTRY, *PMMPFNENTRY;

/* Page Frame List structure definition */
typedef struct _MMPFNLIST
{
    PFN_NUMBER Total;
    MMPAGELISTS ListName;
    PFN_NUMBER Flink;
    PFN_NUMBER Blink;
} MMPFNLIST, *PMMPFNLIST;

/* Physical memory run structure definition */
typedef struct _PHYSICAL_MEMORY_RUN
{
    PFN_NUMBER BasePage;
    PFN_NUMBER PageCount;
} PHYSICAL_MEMORY_RUN, *PPHYSICAL_MEMORY_RUN;

/* Physical memory descriptor structure definition */
typedef struct _PHYSICAL_MEMORY_DESCRIPTOR
{
    ULONG NumberOfRuns;
    PFN_NUMBER NumberOfPages;
    PHYSICAL_MEMORY_RUN Run[1];
} PHYSICAL_MEMORY_DESCRIPTOR, *PPHYSICAL_MEMORY_DESCRIPTOR;

/* Pool header structure definition */
typedef struct _POOL_HEADER
{
    union
    {
        struct
        {
            USHORT PreviousSize:9;
            USHORT PoolIndex:7;
            USHORT BlockSize:9;
            USHORT PoolType:7;
        };
        ULONG Long;
    };
    union
    {
        ULONG PoolTag;
        PEPROCESS ProcessBilled;
        struct
        {
            USHORT AllocatorBackTraceIndex;
            USHORT PoolTagHash;
        };
    };
} POOL_HEADER, *PPOOL_HEADER;

/* Pool descriptor structure definition */
typedef struct _POOL_TRACKING_BIG_ALLOCATIONS
{
    ULONG NumberOfPages;
    PVOID QuotaObject;
    ULONG Tag;
    PVOID VirtualAddress;
} POOL_TRACKING_BIG_ALLOCATIONS, *PPOOL_TRACKING_BIG_ALLOCATIONS;

/* Pool tracking table structure definition */
typedef struct _POOL_TRACKING_TABLE
{
    LONG NonPagedAllocations;
    SIZE_T NonPagedBytes;
    LONG NonPagedFrees;
    LONG PagedAllocations;
    SIZE_T PagedBytes;
    LONG PagedFrees;
    ULONG Tag;
} POOL_TRACKING_TABLE, *PPOOL_TRACKING_TABLE;

#endif /* __XTDK_MMTYPES_H */
