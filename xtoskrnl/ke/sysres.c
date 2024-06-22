/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/sysres.c
 * DESCRIPTION:     System resources management; This code is based on the MinocaOS implementation
 *                  Copyright(C) 2012 Minoca Corp. (https://github.com/minoca/os/blob/master/kernel/ke/sysres.c)
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
KeAcquireSystemResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                        OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader)
{
    /* Get system resource and acquire an ownership */
    return KepGetSystemResource(ResourceType, TRUE, ResourceHeader);
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
KeGetSystemResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
                    OUT PSYSTEM_RESOURCE_HEADER *ResourceHeader)
{
    /* Get system resource without acquiring an ownership */
    return KepGetSystemResource(ResourceType, FALSE, ResourceHeader);
}

/**
 * Releases system resource.
 *
 * @param ResourceHeader
 *        Specifies a pointer to the system resource header.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KeReleaseSystemResource(IN PSYSTEM_RESOURCE_HEADER ResourceHeader)
{
    /* Disable interrupts and acquire a spinlock */
    ArClearInterruptFlag();
    KeAcquireSpinLock(&KepSystemResourcesLock);

    /* Release resource lock */
    ResourceHeader->ResourceLocked = FALSE;

    /* Release spinlock and enable interrupts */
    KeReleaseSpinLock(&KepSystemResourcesLock);
    ArSetInterruptFlag();
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
KepGetSystemResource(IN SYSTEM_RESOURCE_TYPE ResourceType,
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
    Interrupts = ArInterruptsEnabled();

    /* Disable interrupts and acquire a spinlock */
    ArClearInterruptFlag();
    KeAcquireSpinLock(&KepSystemResourcesLock);

    /* Iterate through system resources list */
    ListEntry = KepSystemResourcesListHead.Flink;
    while(ListEntry != &KepSystemResourcesListHead)
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
    if(ListEntry == &KepSystemResourcesListHead)
    {
        /* Resource not found, return NULL */
        Resource = NULL;
        Status = STATUS_NOT_FOUND;
    }

    /* Release spinlock and re-enable interrupts if necessary */
    KeReleaseSpinLock(&KepSystemResourcesLock);
    if(Interrupts)
    {
        /* Re-enable interrupts */
        ArSetInterruptFlag();
    }

    /* Return resource header and status code */
    *ResourceHeader = Resource;
    return Status;
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
KepInitializeSystemResources(VOID)
{
    PSYSTEM_RESOURCE_HEADER ResourceHeader;
    PLIST_ENTRY ListEntry, NextListEntry;
    ULONG ResourceSize;

    /* Initialize system resources spin lock and resource list */
    KeInitializeSpinLock(&KepSystemResourcesLock);
    RtlInitializeListHead(&KepSystemResourcesListHead);

    /* Make sure there are some system resources available */
    if(!RtlListEmpty(&KeInitializationBlock->SystemResourcesListHead))
    {
        /* Iterate through system resources list */
        ListEntry = KeInitializationBlock->SystemResourcesListHead.Flink;
        while(ListEntry != &KeInitializationBlock->SystemResourcesListHead)
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
                RtlInsertTailList(&KepSystemResourcesListHead, &ResourceHeader->ListEntry);
            }

            /* Go to the next list entry */
            ListEntry = NextListEntry;
        }
    }
}
