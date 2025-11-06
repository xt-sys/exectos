/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/mmtypes.h
 * DESCRIPTION:     Memory management data structures
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_MMTYPES_H
#define __XTDK_MMTYPES_H

#include <xtbase.h>
#include ARCH_HEADER(xtstruct.h)


/* Memory manager page lists */
typedef enum _MMPAGELISTS
{
   ZeroedPageList = 0,
   FreePageList = 1,
   StandbyPageList = 2,
   ModifiedPageList = 3,
   ModifiedNoWritePageList = 4,
   BadPageList = 5,
   ActiveAndValid = 6,
   TransitionPage = 7
} MMPAGELISTS, *PMMPAGELISTS;

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

#endif /* __XTDK_MMTYPES_H */
