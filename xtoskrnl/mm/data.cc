/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/data.cc
 * DESCRIPTION:     Memory Manager global and static data
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/* Array of free page lists segregated by cache color */
PMMCOLOR_TABLES MM::Colors::FreePages[FreePageList + 1];

/* Array of modified pages segregated by cache color */
MMPFNLIST MM::Colors::ModifiedPages[MM_PAGING_COLORS] = {{0, ModifiedPageList, MAXULONG_PTR, MAXULONG_PTR}};

/* Number of supported page colors */
ULONG MM::Colors::PagingColors;

/* Bitmask used to calculate the cache color index */
ULONG MM::Colors::PagingColorsMask;

/* Allocation descriptors dedicated for hardware layer */
LOADER_MEMORY_DESCRIPTOR MM::HardwarePool::HardwareAllocationDescriptors[MM_HARDWARE_ALLOCATION_DESCRIPTORS];

/* Live address of kernel's hardware heap */
PVOID MM::HardwarePool::HardwareHeapStart = MM_HARDWARE_HEAP_START_ADDRESS;

/* Number of used hardware allocation descriptors */
ULONG MM::HardwarePool::UsedHardwareAllocationDescriptors = 0;

/* Processor structures data (THIS IS A TEMPORARY HACK) */
UCHAR MM::KernelPool::ProcessorStructuresData[MAXIMUM_PROCESSORS][KPROCESSOR_STRUCTURES_SIZE] = {{0}};

/* Global structure describing the virtual memory layout of the system */
MMMEMORY_LAYOUT MM::Manager::MemoryLayout;

/* Total number of PTEs reserved for system space mapping */
PFN_NUMBER MM::Manager::NumberOfSystemPtes;

/* Instance of the page map routines for the current PML level */
MM::PPAGEMAP MM::Paging::PmlRoutines;

/* Total number of physical pages available for allocation */
PFN_NUMBER MM::Pfn::AvailablePages;

/* Head of the list containing physical pages marked as defective */
MMPFNLIST MM::Pfn::BadPagesList = {0, BadPageList, MAXULONG_PTR, MAXULONG_PTR};

/* Biggest free memory descriptor */
PLOADER_MEMORY_DESCRIPTOR MM::Pfn::FreeDescriptor;

/* List containing free physical pages */
MMPFNLIST MM::Pfn::FreePagesList = {0, FreePageList, MAXULONG_PTR, MAXULONG_PTR};

/* Highest physical page number */
ULONG_PTR MM::Pfn::HighestPhysicalPage;

/* Lowest physical page number */
ULONG_PTR MM::Pfn::LowestPhysicalPage = -1;

/* List containing modified pages */
MMPFNLIST MM::Pfn::ModifiedPagesList = {0, ModifiedPageList, MAXULONG_PTR, MAXULONG_PTR};

/* List containing modified pages mapped as read-only */
MMPFNLIST MM::Pfn::ModifiedReadOnlyPagesList = {0, ModifiedReadOnlyPageList, MAXULONG_PTR, MAXULONG_PTR};

/* Number of physical pages */
ULONGLONG MM::Pfn::NumberOfPhysicalPages;

/* Old biggest free memory descriptor */
LOADER_MEMORY_DESCRIPTOR MM::Pfn::OriginalFreeDescriptor;

/* Array of pointers to PFN lists */
PMMPFNLIST MM::Pfn::PageLocationList[] = {&ZeroedPagesList,
                                          &FreePagesList,
                                          &StandbyPagesList,
                                          &ModifiedPagesList,
                                          &ModifiedReadOnlyPagesList,
                                          &BadPagesList,
                                          NULLPTR,
                                          NULLPTR};

/* Size of the PFN database in pages */
PFN_NUMBER MM::Pfn::PfnDatabaseSize;

/* List containing pages mapped as Read-Only (ROM) */
MMPFNLIST MM::Pfn::RomPagesList = {0, StandbyPageList, MAXULONG_PTR, MAXULONG_PTR};

/* List containing standby pages (clean, can be reclaimed or repurposed) */
MMPFNLIST MM::Pfn::StandbyPagesList = {0, StandbyPageList, MAXULONG_PTR, MAXULONG_PTR};

/* List containing free physical pages that have been zeroed out */
MMPFNLIST MM::Pfn::ZeroedPagesList = {0, ZeroedPageList, MAXULONG_PTR, MAXULONG_PTR};

/* Array of lists for available System PTEs, separated by pool type */
MMPTE MM::Pte::FirstSystemFreePte[MaximumPtePoolTypes];

/* PTE list terminator */
ULONG_PTR MM::Pte::ListTerminator;

/* Virtual base address of the System PTE space */
PMMPTE MM::Pte::SystemPteBase;

/* End addresses for the System PTE ranges */
PMMPTE MM::Pte::SystemPtesEnd[MaximumPtePoolTypes];

/* Start addresses for the System PTE ranges */
PMMPTE MM::Pte::SystemPtesStart[MaximumPtePoolTypes];

/* Total count of available System PTEs */
ULONG MM::Pte::TotalSystemFreePtes[MaximumPtePoolTypes];

/* Template PTE entry containing standard flags for a valid, present kernel page */
MMPTE MM::Pte::ValidPte;
