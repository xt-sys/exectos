/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/globals.c
 * DESCRIPTION:     Memory Manager initialization routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/* Biggest free memory descriptor */
PLOADER_MEMORY_MAPPING MmFreeDescriptor;

/* Highest physical page number */
ULONG_PTR MmHighestPhysicalPage;

/* Lowest physical page number */
ULONG_PTR MmLowestPhysicalPage = -1;

/* Number of physical pages */
ULONG MmNumberOfPhysicalPages;

/* Old biggest free memory descriptor */
LOADER_MEMORY_MAPPING MmOldFreeDescriptor;
