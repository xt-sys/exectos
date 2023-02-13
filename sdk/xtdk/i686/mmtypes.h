/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/mmtypes.h
 * DESCRIPTION:     Memory management data structures for i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_MMTYPES_H
#define __XTDK_I686_MMTYPES_H

#include <xtbase.h>


/* Pages related definitions */
#define MM_PAGE_MASK                               0xFFF
#define MM_PAGE_SHIFT                              12
#define MM_PAGE_SIZE                               4096

/* Page Table entry structure definition (with PAE support) */
typedef struct _HARDWARE_PTE
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
} HARDWARE_PTE, *PHARDWARE_PTE;

#endif /* __XTDK_I686_MMTYPES_H */
