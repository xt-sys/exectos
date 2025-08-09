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
#include <mmtypes.h>


/* Pages related definitions */
#define MM_PAGE_MASK                               (MM_PAGE_SIZE - 1)
#define MM_PAGE_SHIFT                              12
#define MM_PAGE_SIZE                               4096

/* Page directory and page base addresses */
#define MM_PTE_BASE                                0xC0000000
#define MM_PDE_BASE                                0xC0600000

/* PTE shift values */
#define MM_PTE_SHIFT                               3
#define MM_PTI_SHIFT                               12
#define MM_PDI_SHIFT                               21
#define MM_PPI_SHIFT                               30

/* PTE legacy shift values */
#define MM_PDI_LEGACY_SHIFT                        22

/* Minimum number of physical pages needed by the system */
#define MM_MINIMUM_PHYSICAL_PAGES                  1100

/* Default number of secondary colors */
#define MM_DEFAULT_SECONDARY_COLORS                64

/* Number of HAL allocation descriptors */
#define MM_HARDWARE_ALLOCATION_DESCRIPTORS         64

/* Kernel HAL heap initial start address */
#define MM_HARDWARE_HEAP_START_ADDRESS             ((PVOID)(((ULONG_PTR)MM_HARDWARE_VA_START) + 1024 * 1024))

/* HAL memory pool virtual address start */
#define MM_HARDWARE_VA_START                       0xFFC00000

/* Maximum physical address used by HAL allocations */
#define MM_MAXIMUM_PHYSICAL_ADDRESS                0xFFFFFFFF


/* Page size enumeration list */
typedef enum _PAGE_SIZE
{
    Size4K,
    Size2M,
    Size4M
} PAGE_SIZE, *PPAGE_SIZE;

/* Page Table entry structure definition (with PAE support) */
typedef struct _HARDWARE_PTE
{
    ULONGLONG Valid:1;
    ULONGLONG Writable:1;
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
    ULONGLONG PageFrameNumber:26;
    ULONGLONG Reserved1:14;
    ULONGLONG SoftwareWsIndex:11;
    ULONGLONG NoExecute:1;
} HARDWARE_PTE, *PHARDWARE_PTE;

/* Page Table Entry on PAE enabled system */
typedef struct _MMPTE_HARDWARE
{
    ULONGLONG Valid:1;
    ULONGLONG Writable:1;
    ULONGLONG Owner:1;
    ULONGLONG WriteThrough:1;
    ULONGLONG CacheDisable:1;
    ULONGLONG Accessed:1;
    ULONGLONG Dirty:1;
    ULONGLONG LargePage:1;
    ULONGLONG Global:1;
    ULONGLONG CopyOnWrite:1;
    ULONGLONG Prototype:1;
    ULONGLONG Write:1;
    ULONGLONG PageFrameNumber:26;
    ULONGLONG Reserved1:25;
    ULONGLONG NoExecute:1;
} MMPTE_HARDWARE, *PMMPTE_HARDWARE;

/* Page Table Entry list structure definition (with PAE support) */
typedef struct _MMPTE_LIST
{
    ULONGLONG Valid:1;
    ULONGLONG OneEntry:1;
    ULONGLONG Reserved1:8;
    ULONGLONG Prototype:1;
    ULONGLONG Reserved2:21;
    ULONGLONG NextEntry:32;
} MMPTE_LIST, *PMMPTE_LIST;

/* Page Table Entry subsection structure definition (with PAE support) */
typedef struct _MMPTE_PROTOTYPE
{
    ULONGLONG Valid:1;
    ULONGLONG Reserved1:7;
    ULONGLONG ReadOnly:1;
    ULONGLONG Reserved2:1;
    ULONGLONG Prototype:1;
    ULONGLONG Protection:5;
    ULONGLONG Reserved3:16;
    ULONGLONG ProtoAddress:32;
} MMPTE_PROTOTYPE, *PMMPTE_PROTOTYPE;

/* Page Table Entry software structure definition (with PAE support) */
typedef struct _MMPTE_SOFTWARE
{
    ULONGLONG Valid:1;
    ULONGLONG PageFileLow:4;
    ULONGLONG Protection:5;
    ULONGLONG Prototype:1;
    ULONGLONG Transition:1;
    ULONGLONG Reserved1:20;
    ULONGLONG PageFileHigh:32;
} MMPTE_SOFTWARE, *PMMPTE_SOFTWARE;

/* Page Table Entry subsection structure definition (with PAE support) */
typedef struct _MMPTE_SUBSECTION
{
    ULONGLONG Valid:1;
    ULONGLONG Reserved1:4;
    ULONGLONG Protection:5;
    ULONGLONG Prototype:1;
    ULONGLONG Reserved2:21;
    ULONGLONG SubsectionAddress:32;
} MMPTE_SUBSECTION, *PMMPTE_SUBSECTION;

/* Page Table Entry transition structure definition (with PAE support) */
typedef struct _MMPTE_TRANSITION
{
    ULONGLONG Valid:1;
    ULONGLONG Write:1;
    ULONGLONG Owner:1;
    ULONGLONG WriteThrough:1;
    ULONGLONG CacheDisable:1;
    ULONGLONG Protection:5;
    ULONGLONG Prototype:1;
    ULONGLONG Transition:1;
    ULONGLONG PageFrameNumber:26;
    ULONGLONG Unused:26;
} MMPTE_TRANSITION, *PMMPTE_TRANSITION;

/* Page Table Entry structure definition (with PAE support) */
typedef union _MMPTE
{
    ULONGLONG Long;
    HARDWARE_PTE Flush;
    MMPTE_HARDWARE Hardware;
    MMPTE_PROTOTYPE Prototype;
    MMPTE_SOFTWARE Software;
    MMPTE_TRANSITION Transition;
    MMPTE_SUBSECTION Subsection;
    MMPTE_LIST List;
} MMPTE, *PMMPTE;

/* Legacy Page Table entry structure definition (without PAE support) */
typedef struct _HARDWARE_LEGACY_PTE
{
    ULONG Valid:1;
    ULONG Writable:1;
    ULONG Owner:1;
    ULONG WriteThrough:1;
    ULONG CacheDisable:1;
    ULONG Accessed:1;
    ULONG Dirty:1;
    ULONG LargePage:1;
    ULONG Global:1;
    ULONG CopyOnWrite:1;
    ULONG Prototype:1;
    ULONG Reserved0:1;
    ULONG PageFrameNumber:20;
} HARDWARE_LEGACY_PTE, *PHARDWARE_LEGACY_PTE;

/* Page Frame Number structure definition */
typedef struct _MMPFN
{
    union
    {
        PFN_NUMBER Flink;
        ULONG WsIndex;
        PKEVENT Event;
        XTSTATUS ReadStatus;
        SINGLE_LIST_ENTRY NextStackPfn;
    } u1;
    PMMPTE PteAddress;
    union
    {
        PFN_NUMBER Blink;
        ULONG_PTR ShareCount;
    } u2;
    union
    {
        MMPFNENTRY e1;
        struct
        {
            USHORT ShortFlags;
            USHORT ReferenceCount;
        } e2;
    } u3;
    union
    {
        MMPTE OriginalPte;
        LONG AweReferenceCount;
    };
    union
    {
        ULONG_PTR EntireFrame;
        struct
        {
            ULONG_PTR PteFrame:26;
            ULONG_PTR InPageError:1;
            ULONG_PTR VerifierAllocation:1;
            ULONG_PTR AweAllocation:1;
            ULONG_PTR LockCharged:1;
            ULONG_PTR KernelStack:1;
            ULONG_PTR MustBeCached:1;
        };
    } u4;
} MMPFN, *PMMPFN;

#endif /* __XTDK_I686_MMTYPES_H */
