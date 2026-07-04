/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ex/laslist.cc
 * DESCRIPTION:     Lookaside Lists support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


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

/**
 * Allocates a memory block from the specified per-processor lookaside list.
 *
 * @param Number
 *        Supplies the index of the lookaside list to allocate from.
 *
 * @return This routine returns a pointer to the allocated memory block, or NULLPTR if allocation fails.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
EX::LookasideList::AllocateFromPerProcessorList(IN NONPAGED_LOOKASIDE_NUMBER Number)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    PGENERAL_LOOKASIDE Lookaside;
    PVOID Entry;

    /* Retrieve the current Processor Control Block */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Target the processor's local lookaside list and increment the tracking metric */
    Lookaside = Prcb->LookasideList[Number].Local;
    Lookaside->TotalAllocates++;

    /* Attempt a pop from the local list */
    Entry = RTL::Atomic::PopEntrySingleList(&Lookaside->ListHead);
    if(Entry != NULLPTR)
    {
        /* Return the retrieved a pre-allocated block */
        return Entry;
    }

    /* Record a local cache miss */
    Lookaside->AllocateMisses++;
    Lookaside->TotalAllocates++;

    /* Switch the target to the shared global lookaside list */
    Lookaside = Prcb->LookasideList[Number].Global;

    /* Attempt a pop from the global list */
    Entry = RTL::Atomic::PopEntrySingleList(&Lookaside->ListHead);
    if(Entry != NULLPTR)
    {
        /* Return the retrieved a pre-allocated block */
        return Entry;
    }

    /* Both caches are saturated, record a miss */
    Lookaside->AllocateMisses++;

    /* Invoke the pool allocator */
    (Lookaside->Allocate)(Lookaside->Type, Lookaside->Size, &Entry, Lookaside->Tag);

    /* Return the newly allocated block */
    return Entry;
}

/**
 * Returns a previously allocated memory block to the specified non-paged lookaside list.
 *
 * @param LookasideList
 *        Supplies a pointer to the non-paged lookaside list.
 *
 * @param Entry
 *        Supplies a pointer to the memory block being freed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
EX::LookasideList::FreeToLookasideList(IN PNONPAGED_LOOKASIDE_LIST LookasideList,
                                       IN PVOID Entry)
{
    /* Increment the tracking metric */
    LookasideList->Global.TotalFrees++;

    /* Verify if the lookaside list has reached its maximum capacity threshold */
    if(RTL::SinglyList::QueryListDepth(&LookasideList->Global.ListHead) >= LookasideList->Global.Depth)
    {
        /* The list is full, record a capacity miss */
        LookasideList->Global.FreeMisses++;
        (LookasideList->Global.Free)(Entry);
    }
    else
    {
        /* Push the block onto the lookaside list */
        RTL::Atomic::PushEntrySingleList(&LookasideList->Global.ListHead, (PSINGLE_LIST_ENTRY)Entry);
    }
}

/**
 * Returns a previously allocated memory block to the specified paged lookaside list.
 *
 * @param LookasideList
 *        Supplies a pointer to the paged lookaside list.
 *
 * @param Entry
 *        Supplies a pointer to the memory block being freed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
EX::LookasideList::FreeToLookasideList(IN PPAGED_LOOKASIDE_LIST LookasideList,
                                       IN PVOID Entry)
{
    /* Increment the tracking metric */
    LookasideList->Global.TotalFrees++;

    /* Verify if the lookaside list has reached its maximum capacity threshold */
    if(RTL::SinglyList::QueryListDepth(&LookasideList->Global.ListHead) >= LookasideList->Global.Depth)
    {
        /* The list is full, record a capacity miss */
        LookasideList->Global.FreeMisses++;
        (LookasideList->Global.Free)(Entry);
    }
    else
    {
        /* Push the block onto the lookaside list */
        RTL::Atomic::PushEntrySingleList(&LookasideList->Global.ListHead, (PSINGLE_LIST_ENTRY)Entry);
    }
}

/**
 * Frees a memory block back to the specified per-processor lookaside list.
 *
 * @param Number
 *        Supplies the index of the lookaside list receiving the freed block.
 *
 * @param Entry
 *        Supplies a pointer to the memory block being freed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
EX::LookasideList::FreeToPerProcessorList(IN NONPAGED_LOOKASIDE_NUMBER Number,
                                          IN PVOID Entry)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    PGENERAL_LOOKASIDE Lookaside;

    /* Retrieve the current Processor Control Block */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Target the processor's local lookaside list and increment the tracking metric */
    Lookaside = Prcb->LookasideList[Number].Local;
    Lookaside->TotalFrees += 1;

    /* Check if the lookaside list has reached its maximum capacity threshold */
    if(RTL::SinglyList::QueryListDepth(&Lookaside->ListHead) >= Lookaside->Depth)
    {
        /* The local list is full, record a capacity miss */
        Lookaside->FreeMisses++;
        Lookaside->TotalFrees++;

        /* Switch the target to the shared global lookaside list */
        Lookaside = Prcb->LookasideList[Number].Global;

        /* Check if the lookaside list has reached its maximum capacity threshold */
        if(RTL::SinglyList::QueryListDepth(&Lookaside->ListHead) >= Lookaside->Depth)
        {
            /* Both caches are saturated, record a miss */
            Lookaside->FreeMisses++;
            (Lookaside->Free)(Entry);

            /* Exit the routine */
            return;
        }
    }

    /* Push the block onto the selected lookaside list */
    RTL::Atomic::PushEntrySingleList(&Lookaside->ListHead, (PSINGLE_LIST_ENTRY)Entry);
}

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
