/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/data.cc
 * DESCRIPTION:     Memory Manager global and static data
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/* Allocation descriptors dedicated for hardware layer */
LOADER_MEMORY_DESCRIPTOR MM::HardwarePool::HardwareAllocationDescriptors[MM_HARDWARE_ALLOCATION_DESCRIPTORS];

/* Live address of kernel's hardware heap */
PVOID MM::HardwarePool::HardwareHeapStart = MM_HARDWARE_HEAP_START_ADDRESS;

/* Number of used hardware allocation descriptors */
ULONG MM::HardwarePool::UsedHardwareAllocationDescriptors = 0;

/* Processor structures data (THIS IS A TEMPORARY HACK) */
UCHAR MM::KernelPool::ProcessorStructuresData[MAXIMUM_PROCESSORS][KPROCESSOR_STRUCTURES_SIZE] = {{0}};

/* Biggest free memory descriptor */
PLOADER_MEMORY_DESCRIPTOR MM::Pfn::FreeDescriptor;

/* Highest physical page number */
ULONG_PTR MM::Pfn::HighestPhysicalPage;

/* Lowest physical page number */
ULONG_PTR MM::Pfn::LowestPhysicalPage = -1;

/* Number of physical pages */
ULONG MM::Pfn::NumberOfPhysicalPages;

/* Old biggest free memory descriptor */
LOADER_MEMORY_DESCRIPTOR MM::Pfn::OriginalFreeDescriptor;

/* Instance of the page map routines for the current PML level */
MM::PPAGEMAP MM::Paging::PmlRoutines;
