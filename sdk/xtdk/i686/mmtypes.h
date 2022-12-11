/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/mmtypes.h
 * DESCRIPTION:     Memory management data structures for i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_MMTYPES_H
#define __XTDK_I686_MMTYPES_H

#include "xtcommon.h"


/* Page Table entry structure definition */
typedef struct _HARDWARE_PTE
{
    UINT32 Valid:1;
    UINT32 Write:1;
    UINT32 Owner:1;
    UINT32 WriteThrough:1;
    UINT32 CacheDisable:1;
    UINT32 Accessed:1;
    UINT32 Dirty:1;
    UINT32 LargePage:1;
    UINT32 Global:1;
    UINT32 CopyOnWrite:1;
    UINT32 Prototype:1;
    UINT32 Reserved:1;
    UINT32 PageFrameNumber:20;
} HARDWARE_PTE, *PHARDWARE_PTE;

/* Page Table entry with PAE support structure definition */
typedef struct _HARDWARE_PTE_PAE
{
    ULONGLONG Valid:1;
    ULONGLONG Write:1;
    ULONGLONG Owner:1;
    ULONGLONG WriteThrough:1;
    ULONGLONG CacheDisable:1;
    ULONGLONG Accessed:1;
    ULONGLONG Dirty:1;
    ULONGLONG LargePage:1;
    ULONGLONG Global:1;
    ULONGLONG CopyOnWrite:1;
    ULONGLONG Prototype:1;
    ULONGLONG Reserved0:1;
    ULONGLONG PageFrameNumber:28;
    ULONGLONG Reserved1:12;
    ULONGLONG SoftwareWsIndex:11;
    ULONGLONG NoExecute:1;
} HARDWARE_PTE_PAE, *PHARDWARE_PTE_PAE;

#endif /* __XTDK_I686_MMTYPES_H */
