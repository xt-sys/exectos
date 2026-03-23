/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/data.cc
 * DESCRIPTION:     Memory Manager global and static data
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/* Global table used to track pool memory allocations */
PPOOL_TRACKING_TABLE MM::Allocator::AllocationsTrackingTable;

/* Spinlock protecting the allocations table */
KSPIN_LOCK MM::Allocator::AllocationsTrackingTableLock;

/* Bitmask used during the hashing process */
SIZE_T MM::Allocator::AllocationsTrackingTableMask;

/* Total number of entries in the global allocations tracking table */
SIZE_T MM::Allocator::AllocationsTrackingTableSize;

/* Active number of big allocations to trigger table expansion */
ULONG MM::Allocator::BigAllocationsInUse;

/* Pointer to the hash table for tracking page-aligned memory */
PPOOL_TRACKING_BIG_ALLOCATIONS MM::Allocator::BigAllocationsTrackingTable;

/* Bitmask used for fast modulo arithmetic during hash bucket lookups */
SIZE_T MM::Allocator::BigAllocationsTrackingTableHash;

/* Spinlock protecting the big allocations table */
KSPIN_LOCK MM::Allocator::BigAllocationsTrackingTableLock;

/* Maximum capacity of the tracking hash table */
SIZE_T MM::Allocator::BigAllocationsTrackingTableSize;

/* Array of CPU-local tracking tables */
PPOOL_TRACKING_TABLE MM::Allocator::TagTables[MM_POOL_TRACKING_TABLES];

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

/* Global structure describing the virtual memory layout of the system */
MMMEMORY_LAYOUT MM::Manager::MemoryLayout;

/* Total number of PTEs reserved for system space mapping */
PFN_NUMBER MM::Manager::NumberOfSystemPtes;

/* Physical memory block descriptor */
PPHYSICAL_MEMORY_DESCRIPTOR MM::Manager::PhysicalMemoryBlock;

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
ULONG_PTR MM::Pfn::LowestPhysicalPage;

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

/* Bitmap used to track physical pages */
RTL_BITMAP MM::Pfn::PfnBitMap;

/* List containing pages mapped as Read-Only (ROM) */
MMPFNLIST MM::Pfn::RomPagesList = {0, StandbyPageList, MAXULONG_PTR, MAXULONG_PTR};

/* List containing standby pages (clean, can be reclaimed or repurposed) */
MMPFNLIST MM::Pfn::StandbyPagesList = {0, StandbyPageList, MAXULONG_PTR, MAXULONG_PTR};

/* List containing free physical pages that have been zeroed out */
MMPFNLIST MM::Pfn::ZeroedPagesList = {0, ZeroedPageList, MAXULONG_PTR, MAXULONG_PTR};

/* Non-paged pool descriptor */
POOL_DESCRIPTOR MM::Pool::NonPagedPoolDescriptor;

/* PFN marking the initial non-paged pool end boundary */
PFN_NUMBER MM::Pool::NonPagedPoolFrameEnd;

/* PFN marking the initial non-paged pool start boundary */
PFN_NUMBER MM::Pool::NonPagedPoolFrameStart;

/* Array of non-paged pool free list heads */
LIST_ENTRY MM::Pool::NonPagedPoolFreeList[MM_MAX_FREE_PAGE_LIST_HEADS];

/* Random cookie used to obfuscate pool links */
ULONG MM::Pool::PoolSecureCookie;

/* Array of pool descriptors */
PPOOL_DESCRIPTOR MM::Pool::PoolVector[2];

/* Array of lists for available System PTEs, separated by pool type */
MMPTE MM::Pte::FirstSystemFreePte[MaximumPtePoolTypes];

/* Virtual base address of the System PTE space */
PMMPTE MM::Pte::SystemPteBase;

/* End addresses for the System PTE ranges */
PMMPTE MM::Pte::SystemPtesEnd[MaximumPtePoolTypes];

/* Start addresses for the System PTE ranges */
PMMPTE MM::Pte::SystemPtesStart[MaximumPtePoolTypes];

/* Total count of available System PTEs */
PFN_COUNT MM::Pte::TotalSystemFreePtes[MaximumPtePoolTypes];

/* Template PTE entry containing standard flags for a valid, present kernel page */
MMPTE MM::Pte::ValidPte;
