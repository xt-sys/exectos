/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ex/laslist.cc
 * DESCRIPTION:     Lookaside Lists support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Initializes a generic lookaside list and inserts it into the tracking list.
 *
 * @param LookasideList
 *        Supplies a pointer to the general lookaside list structure to initialize.
 *
 * @param PoolType
 *        Supplies the type of memory pool to be allocated.
 *
 * @param Size
 *        Supplies the size, in bytes, of the entries to be allocated from the lookaside list.
 *
 * @param Tag
 *        Supplies the pool tag to be used for allocations.
 *
 * @param MaxDepth
 *        Supplies the maximum number of entries the lookaside list should hold.
 *
 * @param ListHead
 *        Supplies a pointer to the linked list header used to track this lookaside list.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
EX::LookasideList::InitializeLookasideList(IN OUT PGENERAL_LOOKASIDE LookasideList,
                                           IN MMPOOL_TYPE PoolType,
                                           IN ULONG Size,
                                           IN ULONG Tag,
                                           IN USHORT MaxDepth,
                                           IN PLIST_ENTRY ListHead)
{
    /* Initialize the singly linked list header and insert it into the tracking list */
    RTL::SinglyList::InitializeListHead(&LookasideList->ListHead);
    RTL::LinkedList::InsertTailList(ListHead, &LookasideList->ListEntry);

    /* Initialize lookaside list properties */
    LookasideList->Allocate = &MM::Allocator::AllocatePool;
    LookasideList->AllocateHits = 0;
    LookasideList->Depth = 2;
    LookasideList->Free = &MM::Allocator::FreePool;
    LookasideList->FreeHits = 0;
    LookasideList->LastAllocateHits = 0;
    LookasideList->LastTotalAllocates = 0;
    LookasideList->MaximumDepth = MaxDepth;
    LookasideList->Size = Size;
    LookasideList->Tag = Tag;
    LookasideList->TotalAllocates = 0;
    LookasideList->TotalFrees = 0;
    LookasideList->Type = PoolType;
}

/**
 * Initializes the per-processor lookaside list pointers.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
EX::LookasideList::InitializePointers(VOID)
{
    PGENERAL_LOOKASIDE LookasideEntry;
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    ULONG Index;

    /* Retrieve the processor control block */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Initialize both non-paged and paged lookaside lists for all pool indexes */
    for(Index = 0; Index < POOL_LOOKASIDE_LISTS; Index++)
    {
        /* Get the non-paged lookaside list entry */
        LookasideEntry = &NonPagedPoolLookasideLists[Index];

        /* Initialize the singly linked list header for the non-paged entry */
        RTL::SinglyList::InitializeListHead(&LookasideEntry->ListHead);

        /* Assign the initialized non-paged entry to the PRCB */
        Prcb->NonPagedLookasideList[Index].Global = LookasideEntry;
        Prcb->NonPagedLookasideList[Index].Local = LookasideEntry;

        /* Get the paged lookaside list entry */
        LookasideEntry = &PagedPoolLookasideLists[Index];

        /* Initialize the singly linked list header for the paged entry */
        RTL::SinglyList::InitializeListHead(&LookasideEntry->ListHead);

        /* Assign the initialized paged entry to the PRCB */
        Prcb->PagedLookasideList[Index].Global = LookasideEntry;
        Prcb->PagedLookasideList[Index].Local = LookasideEntry;
    }
}

/**
 * Initializes global system lookaside lists and their synchronization primitives.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
EX::LookasideList::InitializeSystemLookasideLists(VOID)
{
    ULONG Index;

    /* Initialize spinlocks for protecting lookaside list operations */
    KE::SpinLock::InitializeSpinLock(&NonPagedLookasideListLock);
    KE::SpinLock::InitializeSpinLock(&PagedLookasideListLock);

    /* Initialize global list heads for tracking various types of lookaside lists */
    RTL::LinkedList::InitializeListHead(&NonPagedLookasideListHead);
    RTL::LinkedList::InitializeListHead(&PagedLookasideListHead);
    RTL::LinkedList::InitializeListHead(&PoolLookasideListHead);
    RTL::LinkedList::InitializeListHead(&SystemLookasideListHead);

    /* Initialize standard pool lookaside lists */
    for(Index = 0; Index < POOL_LOOKASIDE_LISTS; Index++)
    {
        /* Initialize non-paged pool lookaside list */
        InitializeLookasideList(&NonPagedPoolLookasideLists[Index], NonPagedPool, (Index + 1) * 8,
                                TAG_MM_MEMORY_POOL, 256, &PoolLookasideListHead);

        /* Initialize paged pool lookaside list */
        InitializeLookasideList(&PagedPoolLookasideLists[Index], PagedPool, (Index + 1) * 8,
                                TAG_MM_MEMORY_POOL, 256, &PoolLookasideListHead);
    }
}

/**
 * Attempts to allocate a fixed-size buffer from the specified non-paged lookaside list.
 *
 * @param LookasideList
 *        Supplies a pointer to the non-paged lookaside list.
 *
 * @return Returns a pointer to the allocated buffer, or NULLPTR if the allocation fails.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
EX::LookasideList::AllocateFromLookasideList(IN PNONPAGED_LOOKASIDE_LIST LookasideList)
{
    PVOID Buffer;

    /* Increment the total number of allocation requests */
    LookasideList->Global.TotalAllocates++;

    /* Attempt to pop an entry from the lookaside list */
    Buffer = RTL::Atomic::PopEntrySingleList(&LookasideList->Global.ListHead);
    if(!Buffer)
    {
        /* Increment the miss counter */
        LookasideList->Global.AllocateMisses++;

        /* Fallback to standard pool allocator */
        (LookasideList->Global.Allocate)(LookasideList->Global.Type, LookasideList->Global.Size,
                                         &Buffer, LookasideList->Global.Tag);
    }

    /* Return the allocated buffer */
    return Buffer;
}

/**
 * Attempts to allocate a fixed-size buffer from the specified paged lookaside list.
 *
 * @param LookasideList
 *        Supplies a pointer to the paged lookaside list.
 *
 * @return Returns a pointer to the allocated buffer, or NULLPTR if the allocation fails.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
EX::LookasideList::AllocateFromLookasideList(IN PPAGED_LOOKASIDE_LIST LookasideList)
{
    PVOID Buffer;

    /* Increment the total number of allocation requests */
    LookasideList->Global.TotalAllocates++;

    /* Attempt to pop an entry from the lookaside list */
    Buffer = RTL::Atomic::PopEntrySingleList(&LookasideList->Global.ListHead);
    if(!Buffer)
    {
        /* Increment the miss counter */
        LookasideList->Global.AllocateMisses++;

        /* Fallback to standard pool allocator */
        (LookasideList->Global.Allocate)(LookasideList->Global.Type, LookasideList->Global.Size,
                                         &Buffer, LookasideList->Global.Tag);
    }

    /* Return the allocated buffer */
    return Buffer;
}
