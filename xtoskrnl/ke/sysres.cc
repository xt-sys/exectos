/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/sysres.cc
 * DESCRIPTION:     System resources management; This code is based on the MinocaOS implementation
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Kernel Library */
namespace KE
{

/**
 * Looks for an unacquired system resource of the specified type and acquires it.
 *
 * @param ResourceType
 *        Supplies system resource type.
 *
 * @param ResourceHeader
 *        Specifies a memory area where a pointer to the system resource header will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
SystemResources::AcquireResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                                 OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader)
{
    /* Get system resource and acquire an ownership */
    return GetSystemResource(ResourceType, TRUE, ResourceHeader);
}

/**
 * Looks for an unacquired system resource of the specified type.
 *
 * @param ResourceType
 *        Supplies system resource type.
 *
 * @param Acquire
 *        Specifies whether system resource should be acquired or not.
 *
 * @param ResourceHeader
 *        Specifies a memory area where a pointer to the system resource header will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
SystemResources::GetSystemResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                                   IN BOOLEAN ResourceLock,
                                   OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader)
{
    PSYSTEM_RESOURCE_HEADER Resource;
    PLIST_ENTRY ListEntry;
    BOOLEAN Interrupts;
    XTSTATUS Status;

    /* Assume resource found successfully */
    Status = STATUS_SUCCESS;

    /* Check if interrupts are enabled */
    Interrupts = AR::CpuFunc::InterruptsEnabled();

    /* Disable interrupts and acquire a spinlock */
    AR::CpuFunc::ClearInterruptFlag();
    SpinLock::AcquireSpinLock(&ResourcesLock);

    /* Iterate through system resources list */
    ListEntry = ResourcesListHead.Flink;
    while(ListEntry != &ResourcesListHead)
    {
        /* Get resource header */
        Resource = CONTAIN_RECORD(ListEntry, SYSTEM_RESOURCE_HEADER, ListEntry);

        /* Check if resource type matches */
        if(Resource->ResourceType == ResourceType)
        {
            /* Check if resource is locked */
            if(Resource->ResourceLocked)
            {
                /* Resource locked, set status code and stop browsing a list */
                Status = STATUS_RESOURCE_LOCKED;
                break;
            }

            /* Check if resource lock should be acquired */
            if(ResourceLock)
            {
                /* Acquire resource lock */
                Resource->ResourceLocked = TRUE;
            }

            /* Stop browsing a list */
            break;
        }

        /* Go to the next list entry */
        ListEntry = ListEntry->Flink;
    }

    /* Check if resource was found */
    if(ListEntry == &ResourcesListHead)
    {
        /* Resource not found, return NULL */
        Resource = nullptr;
        Status = STATUS_NOT_FOUND;
    }

    /* Release spinlock and re-enable interrupts if necessary */
    SpinLock::ReleaseSpinLock(&ResourcesLock);
    if(Interrupts)
    {
        /* Re-enable interrupts */
        AR::CpuFunc::SetInterruptFlag();
    }

    /* Return resource header and status code */
    *ResourceHeader = Resource;
    return Status;
}

/**
 * Looks for an unacquired system resource of the specified type and returns it without acquiring an ownership.
 *
 * @param ResourceType
 *        Supplies system resource type.
 *
 * @param ResourceHeader
 *        Specifies a memory area where a pointer to the system resource header will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
SystemResources::GetResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                             OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader)
{
    /* Get system resource without acquiring an ownership */
    return GetSystemResource(ResourceType, FALSE, ResourceHeader);
}

/**
 * Initializes system resource management.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
VOID
SystemResources::InitializeResources(VOID)
{
    PSYSTEM_RESOURCE_HEADER ResourceHeader;
    PLIST_ENTRY ListEntry, NextListEntry;
    ULONG ResourceSize;

    /* Initialize system resources spin lock and resource list */
    SpinLock::InitializeSpinLock(&ResourcesLock);
    RtlInitializeListHead(&ResourcesListHead);

    /* Make sure there are some system resources available */
    if(!RtlListEmpty(BootInformation::GetSystemResources()))
    {
        /* Iterate through system resources list */
        ListEntry = BootInformation::GetSystemResources()->Flink;
        while(ListEntry != BootInformation::GetSystemResources())
        {
            /* Get resource header and next list entry */
            ResourceHeader = CONTAIN_RECORD(ListEntry, SYSTEM_RESOURCE_HEADER, ListEntry);
            NextListEntry = ListEntry->Flink;

            /* Basic resource type validation */
            switch(ResourceHeader->ResourceType)
            {
                case SystemResourceAcpi:
                    /* ACPI system resource */
                    ResourceSize = sizeof(SYSTEM_RESOURCE_ACPI);
                    break;
                case SystemResourceFrameBuffer:
                    /* FrameBuffer system resource */
                    ResourceSize = sizeof(SYSTEM_RESOURCE_FRAMEBUFFER);
                    break;
                default:
                    /* Unknown system resource type, skip it */
                    ResourceSize = 0;
                    break;
            }

            /* Validate resource size */
            if(ResourceSize != 0 && ResourceSize == ResourceHeader->ResourceSize)
            {
                /* Move valid resource to the internal kernel list of system resources */
                RtlRemoveEntryList(&ResourceHeader->ListEntry);
                RtlInsertTailList(&ResourcesListHead, &ResourceHeader->ListEntry);
            }

            /* Go to the next list entry */
            ListEntry = NextListEntry;
        }
    }
}

/**
 * Releases boot system resource.
 *
 * @param ResourceHeader
 *        Specifies a pointer to the boot system resource header.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
SystemResources::ReleaseResource(IN PSYSTEM_RESOURCE_HEADER ResourceHeader)
{
    /* Disable interrupts and acquire a spinlock */
    AR::CpuFunc::ClearInterruptFlag();
    SpinLock::AcquireSpinLock(&ResourcesLock);

    /* Release resource lock */
    ResourceHeader->ResourceLocked = FALSE;

    /* Release spinlock and enable interrupts */
    SpinLock::ReleaseSpinLock(&ResourcesLock);
    AR::CpuFunc::SetInterruptFlag();
}

} /* namespace */
