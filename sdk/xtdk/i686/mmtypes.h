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

/* Page directory and page base legacy address */
#define MM_PDE_LEGACY_BASE                         0xC0300000

/* PTE legacy shift values */
#define MM_PTE_LEGACY_SHIFT                        2
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

/* Trampoline code address */
#define MM_TRAMPOLINE_ADDRESS                      0x80000

/* Page size enumeration list */
typedef enum _PAGE_SIZE
{
    Size4K,
    Size2M,
    Size4M
} PAGE_SIZE, *PPAGE_SIZE;

/* Legacy Page Table entry structure definition (PML2) */
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

/* Page Table entry structure definition (PML3) */
typedef struct _HARDWARE_MODERN_PTE
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
} HARDWARE_MODERN_PTE, *PHARDWARE_MODERN_PTE;

/* Generic Page Table entry union to abstract PML2 and PML3 formats */
typedef union _HARDWARE_PTE
{
    ULONGLONG Long;
    HARDWARE_LEGACY_PTE Pml2;
    HARDWARE_MODERN_PTE Pml3;
} HARDWARE_PTE, *PHARDWARE_PTE;

/* Page map information structure definition */
typedef struct _MMPAGEMAP_INFO
{
    BOOLEAN Xpa;
    ULONG EmptyPteList;
    ULONG PteBase;
    ULONG PdeBase;
    ULONG PdiShift;
    ULONG PteShift;
} MMPAGEMAP_INFO, *PMMPAGEMAP_INFO;

/* Legacy Page Table Entry hardware structure definition (PML2) */
typedef struct _MMPML2_PTE_HARDWARE
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
    ULONG Write:1;
    ULONG PageFrameNumber:20;
} MMPML2_PTE_HARDWARE, *PMMPML2_PTE_HARDWARE;

/* Legacy Page Table Entry list structure definition (PML2) */
typedef struct _MMPML2_PTE_LIST
{
    ULONG Valid:1;
    ULONG OneEntry:1;
    ULONG Reserved0:8;
    ULONG Prototype:1;
    ULONG Reserved1:1;
    ULONG NextEntry:20;
} MMPML2_PTE_LIST, *PMMPML2_PTE_LIST;

/* Legacy Page Table Entry subsection structure definition (PML2) */
typedef struct _MMPML2_PTE_PROTOTYPE
{
    ULONG Valid:1;
    ULONG ProtoAddressLow:7;
    ULONG ReadOnly:1;
    ULONG WhichPool:1;
    ULONG Prototype:1;
    ULONG ProtoAddressHigh:21;
} MMPML2_PTE_PROTOTYPE, *PMMPML2_PTE_PROTOTYPE;

/* Legacy Page Table Entry software structure definition (PML2) */
typedef struct _MMPML2_PTE_SOFTWARE
{
    ULONG Valid:1;
    ULONG PageFileLow:4;
    ULONG Protection:5;
    ULONG Prototype:1;
    ULONG Transition:1;
    ULONG PageFileHigh:20;
} MMPML2_PTE_SOFTWARE, *PMMPML2_PTE_SOFTWARE;

/* Legacy Page Table Entry subsection structure definition (PML2) */
typedef struct _MMPML2_PTE_SUBSECTION
{
    ULONG Valid:1;
    ULONG SubsectionAddressLow:4;
    ULONG Protection:5;
    ULONG Prototype:1;
    ULONG SubsectionAddressHigh:20;
    ULONG WhichPool:1;
} MMPML2_PTE_SUBSECTION, *PMMPML2_PTE_SUBSECTION;

/* Legacy Page Table Entry transition structure definition (PML2) */
typedef struct _MMPML2_PTE_TRANSITION
{
    ULONG Valid:1;
    ULONG Write:1;
    ULONG Owner:1;
    ULONG WriteThrough:1;
    ULONG CacheDisable:1;
    ULONG Protection:5;
    ULONG Prototype:1;
    ULONG Transition:1;
    ULONG PageFrameNumber:20;
} MMPML2_PTE_TRANSITION, *PMMPML2_PTE_TRANSITION;

/* Legacy Page Table Entry union definition (PML2) */
typedef union _MMPML2_PTE
{
    ULONG Long;
    HARDWARE_PTE Flush;
    MMPML2_PTE_HARDWARE Hard;
    MMPML2_PTE_PROTOTYPE Proto;
    MMPML2_PTE_SOFTWARE Soft;
    MMPML2_PTE_TRANSITION Trans;
    MMPML2_PTE_SUBSECTION Subsect;
    MMPML2_PTE_LIST List;
} MMPML2_PTE, *PMMPML2_PTE;

/* Page Table Entry hardware structure definition (PML3) */
typedef struct _MMPML3_PTE_HARDWARE
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
    ULONGLONG Reserved0:25;
    ULONGLONG NoExecute:1;
} MMPML3_PTE_HARDWARE, *PMMPML3_PTE_HARDWARE;

/* Page Table Entry list structure definition (PML3) */
typedef struct _MMPML3_PTE_LIST
{
    ULONGLONG Valid:1;
    ULONGLONG OneEntry:1;
    ULONGLONG Reserved0:8;
    ULONGLONG Prototype:1;
    ULONGLONG Reserved1:21;
    ULONGLONG NextEntry:32;
} MMPML3_PTE_LIST, *PMMPML3_PTE_LIST;

/* Page Table Entry subsection structure definition (PML3) */
typedef struct _MMPML3_PTE_PROTOTYPE
{
    ULONGLONG Valid:1;
    ULONGLONG Reserved0:7;
    ULONGLONG ReadOnly:1;
    ULONGLONG Reserved1:1;
    ULONGLONG Prototype:1;
    ULONGLONG Protection:5;
    ULONGLONG Reserved2:16;
    ULONGLONG ProtoAddress:32;
} MMPML3_PTE_PROTOTYPE, *PMMPML3_PTE_PROTOTYPE;

/* Page Table Entry software structure definition (PML3) */
typedef struct _MMPML3_PTE_SOFTWARE
{
    ULONGLONG Valid:1;
    ULONGLONG PageFileLow:4;
    ULONGLONG Protection:5;
    ULONGLONG Prototype:1;
    ULONGLONG Transition:1;
    ULONGLONG Reserved0:20;
    ULONGLONG PageFileHigh:32;
} MMPML3_PTE_SOFTWARE, *PMMPML3_PTE_SOFTWARE;

/* Page Table Entry subsection structure definition (PML3) */
typedef struct _MMPML3_PTE_SUBSECTION
{
    ULONGLONG Valid:1;
    ULONGLONG Reserved0:4;
    ULONGLONG Protection:5;
    ULONGLONG Prototype:1;
    ULONGLONG Reserved1:21;
    ULONGLONG SubsectionAddress:32;
} MMPML3_PTE_SUBSECTION, *PMMPML3_PTE_SUBSECTION;

/* Page Table Entry transition structure definition (PML3) */
typedef struct _MMPML3_PTE_TRANSITION
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
} MMPML3_PTE_TRANSITION, *PMMPML3_PTE_TRANSITION;

/* Page Table Entry union definition (PML3) */
typedef union _MMPML3_PTE
{
    ULONGLONG Long;
    HARDWARE_PTE Flush;
    MMPML3_PTE_HARDWARE Hardware;
    MMPML3_PTE_PROTOTYPE Prototype;
    MMPML3_PTE_SOFTWARE Software;
    MMPML3_PTE_TRANSITION Transition;
    MMPML3_PTE_SUBSECTION Subsection;
    MMPML3_PTE_LIST List;
} MMPML3_PTE, *PMMPML3_PTE;

/* Generic Page Table Entry union to abstract PML2 and PML3 formats */
typedef union _MMPTE
{
    ULONGLONG Long;
    MMPML2_PTE Pml2;
    MMPML3_PTE Pml3;
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
