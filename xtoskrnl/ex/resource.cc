/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ex/resource.cc
 * DESCRIPTION:     Exclusive resource synchronization support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Deletes a system resource and frees its associated memory allocations.
 *
 * @param Resource
 *        Supplies a pointer to the resource to be deleted.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
EX::Resources::DeleteResource(IN PERESOURCE Resource)
{
    /* Start a guarded code block */
    {
        /* Raise runlevel to DISPATCH level and acquire system resources lock */
        KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
        KE::QueuedSpinLockGuard Guard(&SystemResourcesLock);

        /* Remove the resource from the system resource list */
        RTL::LinkedList::RemoveEntryList(&Resource->SystemResourcesList);
    }

    /* Check if exclusive waiters event exists */
    if(Resource->ExclusiveWaiters)
    {
        /* Free the exclusive waiters event */
        MM::Allocator::FreePool(Resource->ExclusiveWaiters);
    }

    /* Check if owner table exists */
    if(Resource->OwnerTable)
    {
        /* Free the owner table */
        MM::Allocator::FreePool(Resource->OwnerTable);
    }

    /* Check if shared waiters semaphore exists */
    if(Resource->SharedWaiters)
    {
        /* Free the shared waiters semaphore */
        MM::Allocator::FreePool(Resource->SharedWaiters);
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes a system resource object.
 *
 * @param Resource
 *        Supplies a pointer to the resource structure to be initialized.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
EX::Resources::InitializeResource(IN PERESOURCE Resource)
{
    /* Clear the entire resource structure */
    RTL::Memory::ZeroMemory(Resource, sizeof(ERESOURCE));

    /* Initialize the spinlock used for managing this resource */
    KE::SpinLock::InitializeSpinLock(&Resource->SpinLock);

    /* Raise runlevel to DISPATCH level and acquire the global system resources lock */
    KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);
    KE::QueuedSpinLockGuard Guard(&SystemResourcesLock);

    /* Insert the resource into the global tracking list */
    RTL::LinkedList::InsertTailList(&SystemResourcesList, &Resource->SystemResourcesList);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes the global system resource structures and synchronization primitives.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
EX::Resources::InitializeSystemResources(VOID)
{
    /* Set the default timeout for resource operations to 4 seconds relative to now */
    ResourcesTimeOut.QuadPart = -40000000LL;

    /* Initialize the global system resources list and its protecting spinlock */
    RTL::LinkedList::InitializeListHead(&SystemResourcesList);
    KE::SpinLock::InitializeSpinLock(&SystemResourcesLock);
}

/**
 * Reinitializes an existing resource object, resetting its state and synchronization primitives.
 *
 * @param Resource
 *        Supplies a pointer to the resource structure to be reinitialized.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
EX::Resources::ReinitializeResource(IN PERESOURCE Resource)
{
    ULONG Index;

    /* Zero the active count and flags */
    Resource->ActiveCount = 0;
    Resource->ActiveEntries = 0;
    Resource->Flag = 0;

    /* Zero the contention count and counters */
    Resource->ContentionCount = 0;
    Resource->NumberOfSharedWaiters = 0;
    Resource->NumberOfExclusiveWaiters = 0;

    /* Zero the owner entry */
    Resource->OwnerEntry.OwnerThread = 0;
    Resource->OwnerEntry.TableSize = 0;

    /* Check if exclusive waiters event exists */
    if(Resource->ExclusiveWaiters)
    {
        /* Reinitialize the event */
        KE::Event::InitializeEvent(Resource->ExclusiveWaiters, SynchronizationEvent, FALSE);
    }

    /* Check if owner table exists */
    if(Resource->OwnerTable)
    {
        /* Iterate over the owner table */
        for(Index = 1; Index < Resource->OwnerTable->TableSize; Index++)
        {
            /* Zero the owner table */
            Resource->OwnerTable[Index].OwnerCount = 0;
            Resource->OwnerTable[Index].OwnerThread = 0;
        }
    }

    /* Check if shared waiters semaphore exists */
    if(Resource->SharedWaiters)
    {
        /* Reinitialize the semaphore */
        KE::Semaphore::InitializeSemaphore(Resource->SharedWaiters, 0, MAXLONG);
    }

    /* Return success */
    return STATUS_SUCCESS;
}
