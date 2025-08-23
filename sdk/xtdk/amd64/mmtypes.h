/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/mmtypes.h
 * DESCRIPTION:     Memory management data structures for AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_MMTYPES_H
#define __XTDK_AMD64_MMTYPES_H

#include <xtbase.h>
#include <mmtypes.h>


/* Pages related definitions */
#define MM_PAGE_MASK                               (MM_PAGE_SIZE - 1)
#define MM_PAGE_SHIFT                              12L
#define MM_PAGE_SIZE                               4096

/* Page directory and page base addresses for 4-level paging */
#define MM_PTE_BASE                                0xFFFFF68000000000ULL
#define MM_PDE_BASE                                0xFFFFF6FB40000000ULL
#define MM_PPE_BASE                                0xFFFFF6FB7DA00000ULL
#define MM_PXE_BASE                                0xFFFFF6FB7DBED000ULL

/* Page directory and page base addresses for 5-level paging */
#define MM_PTE_LA57_BASE                           0xFFFF000000000000ULL
#define MM_PDE_LA57_BASE                           0xFFFF010000000000ULL
#define MM_PPE_LA57_BASE                           0xFFFF010800000000ULL
#define MM_PXE_LA57_BASE                           0xFFFF010840000000ULL
#define MM_P5E_LA57_BASE                           0xFFFF010840200000ULL

/* PTE shift values */
#define MM_PTE_SHIFT                               3
#define MM_PTI_SHIFT                               12
#define MM_PDI_SHIFT                               21
#define MM_PPI_SHIFT                               30
#define MM_PXI_SHIFT                               39
#define MM_P5I_SHIFT                               48

/* Number of PTEs per page */
#define MM_PTE_PER_PAGE                            512
#define MM_PDE_PER_PAGE                            512
#define MM_PPE_PER_PAGE                            512
#define MM_PXE_PER_PAGE                            512

/* Minimum number of physical pages needed by the system */
#define MM_MINIMUM_PHYSICAL_PAGES                  2048

/* Default number of secondary colors */
#define MM_DEFAULT_SECONDARY_COLORS                64

/* Number of HAL allocation descriptors */
#define MM_HARDWARE_ALLOCATION_DESCRIPTORS         64

/* Kernel HAL heap initial start address */
#define MM_HARDWARE_HEAP_START_ADDRESS             ((PVOID)(((ULONG_PTR)MM_HARDWARE_VA_START) + 1024 * 1024))

/* HAL memory pool virtual address start */
#define MM_HARDWARE_VA_START                       0xFFFFFFFFFFC00000ULL

/* Maximum physical address used by HAL allocations */
#define MM_MAXIMUM_PHYSICAL_ADDRESS                0x00000000FFFFFFFFULL

/* Trampoline code address */
#define MM_TRAMPOLINE_ADDRESS                      0x80000

/* Page size enumeration list */
typedef enum _PAGE_SIZE
{
    Size4K,
    Size2M,
    Size1G
} PAGE_SIZE, *PPAGE_SIZE;

/* Page Table Entry structure definition */
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
    ULONGLONG Reserved1:1;
    ULONGLONG PageFrameNumber:36;
    ULONGLONG Reserved2:4;
    ULONGLONG SoftwareWsIndex:11;
    ULONGLONG NoExecute:1;
} HARDWARE_PTE, *PHARDWARE_PTE;

/* Page map information structure definition */
typedef struct _MMPAGEMAP_INFO
{
    BOOLEAN Xpa;
    ULONGLONG PteBase;
    ULONGLONG PdeBase;
    ULONGLONG PpeBase;
    ULONGLONG PxeBase;
    ULONGLONG P5eBase;
    ULONG VaBits;
} MMPAGEMAP_INFO, *PMMPAGEMAP_INFO;

/* A Page Table Entry on AMD64 system */
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
    ULONGLONG PageFrameNumber:36;
    ULONGLONG Reserved1:2;
    ULONGLONG SoftwareWsIndex:11;
    ULONGLONG NoExecute:1;
} MMPTE_HARDWARE, *PMMPTE_HARDWARE;

/* A Page Table Entry on AMD64 system with large pages */
typedef struct _MMPTE_HARDWARE_LARGEPAGE
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
    ULONGLONG Reserved1:1;
    ULONGLONG PAT:1;
    ULONGLONG Reserved2:8;
    ULONGLONG PageFrameNumber:19;
    ULONGLONG Reserved3:24;
} MMPTE_HARDWARE_LARGEPAGE, *PMMPTE_HARDWARE_LARGEPAGE;

/* Page Table Entry list structure definition */
typedef struct _MMPTE_LIST
{
    ULONGLONG Valid:1;
    ULONGLONG OneEntry:1;
    ULONGLONG Reserved1:3;
    ULONGLONG Protection:5;
    ULONGLONG Prototype:1;
    ULONGLONG Transition:1;
    ULONGLONG Reserved2:20;
    ULONGLONG NextEntry:32;
} MMPTE_LIST, *PMMPTE_LIST;

/* Page Table Entry prototype structure definition */
typedef struct _MMPTE_PROTOTYPE
{
    ULONGLONG Valid:1;
    ULONGLONG Reserved1:7;
    ULONGLONG ReadOnly:1;
    ULONGLONG Reserved2:1;
    ULONGLONG Prototype:1;
    ULONGLONG Protection:5;
    LONGLONG ProtoAddress:48;
} MMPTE_PROTOTYPE, *PMMPTE_PROTOTYPE;

/* Software Page Table Entry structure definition */
typedef struct _MMPTE_SOFTWARE
{
    ULONGLONG Valid:1;
    ULONGLONG PageFileLow:4;
    ULONGLONG Protection:5;
    ULONGLONG Prototype:1;
    ULONGLONG Transition:1;
    ULONGLONG UsedPageTableEntries:10;
    ULONGLONG Reserved:10;
    ULONGLONG PageFileHigh:32;
} MMPTE_SOFTWARE, *PMMPTE_SOFTWARE;

/* Page Table Entry subsection structure definition */
typedef struct _MMPTE_SUBSECTION
{
    ULONGLONG Valid:1;
    ULONGLONG Reserved1:4;
    ULONGLONG Protection:5;
    ULONGLONG Prototype:1;
    ULONGLONG Reserved2:5;
    LONGLONG SubsectionAddress:48;
} MMPTE_SUBSECTION, *PMMPTE_SUBSECTION;

/* Page Table Entry transition structure definition */
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
    ULONGLONG PageFrameNumber:36;
    ULONGLONG Unused:16;
} MMPTE_TRANSITION, *PMMPTE_TRANSITION;

/* Page Table Entry structure definition */
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
            ULONG_PTR PteFrame:58;
            ULONG_PTR InPageError:1;
            ULONG_PTR VerifierAllocation:1;
            ULONG_PTR AweAllocation:1;
            ULONG_PTR LockCharged:1;
            ULONG_PTR KernelStack:1;
            ULONG_PTR MustBeCached:1;
        };
    } u4;
} MMPFN, *PMMPFN;

#endif /* __XTDK_AMD64_MMTYPES_H */
