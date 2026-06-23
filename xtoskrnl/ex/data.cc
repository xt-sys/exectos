/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ex/data.cc
 * DESCRIPTION:     Kernel Executive global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Tracks all active non-paged lookaside lists in the system */
LIST_ENTRY EX::LookasideList::NonPagedLookasideListHead;

/* Spinlock protecting the integrity of the NonPagedLookasideListHead */
KSPIN_LOCK EX::LookasideList::NonPagedLookasideListLock;

/* Array of standard system lookaside lists for non-paged pool allocations */
GENERAL_LOOKASIDE EX::LookasideList::NonPagedPoolLookasideLists[POOL_LOOKASIDE_LISTS];

/* Tracks all active paged lookaside lists in the system */
LIST_ENTRY EX::LookasideList::PagedLookasideListHead;

/* Spinlock protecting the integrity of the PagedLookasideListHead */
KSPIN_LOCK EX::LookasideList::PagedLookasideListLock;

/* Array of standard system lookaside lists for paged pool allocations */
GENERAL_LOOKASIDE EX::LookasideList::PagedPoolLookasideLists[POOL_LOOKASIDE_LISTS];

/* Tracks all standard pool lookaside lists */
LIST_ENTRY EX::LookasideList::PoolLookasideListHead;

/* Tracks dynamic system lookaside lists */
LIST_ENTRY EX::LookasideList::SystemLookasideListHead;
