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
#define MM_PTE_LA57_BASE                           0xFFED000000000000ULL
#define MM_PDE_LA57_BASE                           0xFFEDF68000000000ULL
#define MM_PPE_LA57_BASE                           0xFFEDF6FB40000000ULL
#define MM_PXE_LA57_BASE                           0xFFEDF6FB7DA00000ULL
#define MM_P5E_LA57_BASE                           0xFFEDF6FB7DBED000ULL

/* Self map address */
#define MM_PML4_SELF_MAP_ADDRESS                   0xFFFFF6FB7DBEDF68ULL
#define MM_PML5_SELF_MAP_ADDRESS                   0xFFEDF6FB7DBEDF68ULL

/* PTE shift values */
#define MM_PTE_SHIFT                               3
#define MM_PTI_SHIFT                               12
#define MM_PDI_SHIFT                               21
#define MM_PPI_SHIFT                               30
#define MM_PXI_SHIFT                               39
#define MM_P5I_SHIFT                               48

/* PTE state flags */
#define MM_PTE_VALID                               0x0000000000000001ULL
#define MM_PTE_ACCESSED                            0x0000000000000020ULL
#define MM_PTE_DIRTY                               0x0000000000000040ULL

/* PTE scope flags */
#define MM_PTE_LARGE_PAGE                          0x0000000000000080ULL
#define MM_PTE_GLOBAL                              0x0000000000000100ULL

/* PTE access flags */
#define MM_PTE_NOACCESS                            0x0000000000000000ULL
#define MM_PTE_READONLY                            0x0000000000000000ULL
#define MM_PTE_EXECUTE                             0x0000000000000000ULL
#define MM_PTE_EXECUTE_READ                        0x0000000000000000ULL
#define MM_PTE_READWRITE                           0x8000000000000002ULL
#define MM_PTE_WRITECOPY                           0x8000000000000200ULL
#define MM_PTE_EXECUTE_READWRITE                   0x0000000000000002ULL
#define MM_PTE_EXECUTE_WRITECOPY                   0x0000000000000200ULL

/* PTE protection flags */
#define MM_PTE_NOEXECUTE                           0x8000000000000000ULL
#define MM_PTE_PROTECT                             0x8000000000000612ULL

/* PTE cache flags */
#define MM_PTE_CACHE_ENABLE                        0x0000000000000000ULL
#define MM_PTE_CACHE_DISABLE                       0x0000000000000010ULL
#define MM_PTE_CACHE_WRITECOMBINED                 0x0000000000000010ULL
#define MM_PTE_CACHE_WRITETHROUGH                  0x0000000000000008ULL

/* PTE software flags */
#define MM_PTE_COPY_ON_WRITE                       0x0000000000000200ULL
#define MM_PTE_PROTOTYPE                           0x0000000000000400ULL
#define MM_PTE_TRANSITION                          0x0000000000000800ULL

/* PTE protection bits */
#define MM_PTE_PROTECTION_BITS                     5

/* Base address of the system page table */
#define MM_SYSTEM_PTE_BASE                         KSEG0_BASE

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
    ULONGLONG EmptyPteList;
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
    ULONG UsedPageTableEntries;
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
            ULONG_PTR PteFrame:57;
            ULONG_PTR InPageError:1;
            ULONG_PTR VerifierAllocation:1;
            ULONG_PTR AweAllocation:1;
            ULONG_PTR Priority:3;
            ULONG_PTR MustBeCached:1;
        };
    } u4;
} MMPFN, *PMMPFN;

#endif /* __XTDK_AMD64_MMTYPES_H */
