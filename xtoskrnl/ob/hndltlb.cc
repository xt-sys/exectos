/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ob/hndltbl.cc
 * DESCRIPTION:     Object Manager Handle Table Management Interface
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Allocates and initializes a new handle table structure.
 *
 * @param Process
 *        Supplies the optional process to charge quota against.
 *
 * @param Initialize
 *        Indicates whether to build the initial free list.
 *
 * @return This routine returns a pointer to the handle table, or NULL pointer on failure.
 *
 * @since XT 1.0
 */
PHANDLE_TABLE
OB::HandleTable::AllocateHandleTable(IN PEPROCESS Process,
                                     IN BOOLEAN Initialize)
{
    PHANDLE_TABLE_ENTRY HandleEntry, LowLevelTable;
    PHANDLE_TABLE HandleTable;
    ULONG Index;

    /* Allocate a memory block for the handle table structure */
    HandleTable = (PHANDLE_TABLE)AllocateTableMemory(Process, sizeof(HANDLE_TABLE), TRUE);
    if(!HandleTable)
    {
        /* Memory allocation failed, return a NULL pointer */
        return NULLPTR;
    }

    /* Allocate a memory block for the initial low-level handle array */
    LowLevelTable = (PHANDLE_TABLE_ENTRY)AllocateTableMemory(Process, MM_PAGE_SIZE, FALSE);
    if(!LowLevelTable)
    {
        /* Allocation failed, free the handle table structure and return a NULL pointer */
        FreeTableMemory(Process, HandleTable, sizeof(HANDLE_TABLE));
        return NULLPTR;
    }

    /* Publish the low-level table into the handle table structure */
    HandleTable->TableCode = (ULONG_PTR)LowLevelTable;

    /* Initialize the first entry of the root table */
    HandleEntry = &LowLevelTable[0];
    HandleEntry->NextFreeTableEntry = OBJECT_HANDLE_ADDITIONAL_INFO_SIGNATURE;
    HandleEntry->Value = 0;

    /* Check if the initialization is required */
    if(Initialize)
    {
        /* Advance to the first handle entry */
        HandleEntry++;

        /* Iterate through the low-level table block */
        for(Index = 1; Index < OBJECT_HANDLE_LOWLEVEL_COUNT - 1; Index++)
        {
            /* Link the current entry to the next available handle index */
            HandleEntry->Value = 0;
            HandleEntry->NextFreeTableEntry = (Index + 1) * OBJECT_HANDLE_VALUE_INCREMENT;

            /* Advance the pointer */
            HandleEntry++;
        }

        /* Terminate the free list chain */
        HandleEntry->Value = 0;
        HandleEntry->NextFreeTableEntry = 0;

        /* Initialize the free list head */
        HandleTable->FirstFreeHandle = (PHANDLE_TABLE_ENTRY)(ULONG_PTR)OBJECT_HANDLE_VALUE_INCREMENT;
    }

    /* Set the next handle needing pool */
    HandleTable->NextHandleNeedingPool = OBJECT_HANDLE_LOWLEVEL_COUNT * OBJECT_HANDLE_VALUE_INCREMENT;

    /* Associate the handle table with the target process */
    HandleTable->QuotaProcess = Process;

    /* Check if a target quota process was provided */
    if(Process)
    {
        /* Inherit the process identifier */
        HandleTable->UniqueProcessId = Process->UniqueProcessId;
    }
    else
    {
        /* Inherit the process identifier from the current process */
        HandleTable->UniqueProcessId = PS::Process::GetCurrentProcess()->UniqueProcessId;
    }

    /* Clear all flags */
    HandleTable->Flags = 0;

    /* Iterate through the handle table pushlocks */
    for(Index = 0; Index < OBJECT_HANDLE_VALUE_INCREMENT; Index++)
    {
        /* Initialize the pushlock */
        KE::PushLock::InitializePushLock(&HandleTable->HandleTableLock[Index]);
    }

    /* Initialize the pushlock used for handle contention synchronization */
    KE::PushLock::InitializePushLock(&HandleTable->HandleContentionEvent);

    /* Return the pointer to the handle table */
    return HandleTable;
}

/**
 * Allocates a new handle table entry.
 *
 * @param HandleTable
 *        Supplies the handle table to allocate from.
 *
 * @param Initialize
 *        Indicates whether to initialize the free list for new entries.
 *
 * @return This routine returns TRUE on success, or FALSE otherwise.
 *
 * @since XT 1.0
 */
BOOLEAN
OB::HandleTable::AllocateHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                          IN BOOLEAN Initialize)
{
    PHANDLE_TABLE_ENTRY **HighLevelTable, **HighLevelTableEntry, LowLevelTable, *MediumLevelTable, *MediumLevelTableEntry;
    ULONG FirstFree, FirstFreeIndex, HighLevelIndex, MediumLevelIndex, OldIndex, RemainingIndex, TableLevel;
    ULONG_PTR CapturedTable;

    /* Capture the current table structure */
    CapturedTable = HandleTable->TableCode;

    /* Extract the depth level of the handle table */
    TableLevel = (ULONG)(CapturedTable & OBJECT_HANDLE_LEVEL_CODE_MASK);

    /* Mask out the level bits */
    CapturedTable = CapturedTable & ~OBJECT_HANDLE_LEVEL_CODE_MASK;

    /* Initialize the low-level table pointer */
    LowLevelTable = NULLPTR;

    /* Check if the handle table is currently operating as a single-level structure */
    if(!TableLevel)
    {
        /* Allocate a medium-level table directory */
        MediumLevelTable = AllocateMediumLevelTable(HandleTable, Initialize, &LowLevelTable);
        if(!MediumLevelTable)
        {
            /* Table directory allocation failed, return FALSE */
            return FALSE;
        }

        /* Link the low-level table into the medium-level directory */
        MediumLevelTable[1] = MediumLevelTable[0];

        /* Set the original flat table as the first entry in the medium-level directory */
        MediumLevelTable[0] = (PHANDLE_TABLE_ENTRY)CapturedTable;

        /* Encode the table pointer */
        CapturedTable = ((ULONG_PTR)MediumLevelTable) | 1;

        /* Publish the multi-level table structure */
        RTL::Atomic::ExchangePointer((PVOID *)&HandleTable->TableCode, (PVOID)CapturedTable);
    }
    else if(TableLevel == 1)
    {
        /* Cast the captured table pointer to a medium-level directory array */
        MediumLevelTableEntry = (PHANDLE_TABLE_ENTRY *)CapturedTable;

        /* Calculate the directory index */
        MediumLevelIndex = HandleTable->NextHandleNeedingPool /
                           (OBJECT_HANDLE_LOWLEVEL_COUNT * OBJECT_HANDLE_VALUE_INCREMENT);

        /* Check if the current medium-level directory has remaining capacity */
        if(MediumLevelIndex < OBJECT_HANDLE_MEDIUMLEVEL_COUNT)
        {
            /* Allocate a low-level table leaf */
            LowLevelTable = AllocateLowLevelTable(HandleTable, Initialize);
            if(!LowLevelTable)
            {
                /* Table allocation failed, return FALSE */
                return FALSE;
            }

            /* Publish the low-level table */
            RTL::Atomic::ExchangePointer((PVOID *)(&MediumLevelTableEntry[MediumLevelIndex]), LowLevelTable);
        }
        else
        {
            /* The medium-level directory is exhausted, allocate a high-level table directory */
            HighLevelTable = (PHANDLE_TABLE_ENTRY**)AllocateTableMemory(HandleTable->QuotaProcess,
                                                                        OBJECT_HANDLE_HIGHLEVEL_SIZE, TRUE);
            if(!HighLevelTable)
            {
                /* Table allocation failed, return FALSE */
                return FALSE;
            }

            /* Allocate a medium-level directory */
            MediumLevelTable = AllocateMediumLevelTable(HandleTable, Initialize, &LowLevelTable);
            if(!MediumLevelTable)
            {
                /* Free the high-level table and return FALSE */
                FreeTableMemory(HandleTable->QuotaProcess, HighLevelTable, OBJECT_HANDLE_HIGHLEVEL_SIZE);
                return FALSE;
            }

            /* Link the medium-level structure as the first branch */
            HighLevelTable[0] = (PHANDLE_TABLE_ENTRY *)CapturedTable;

            /* Link the medium-level structure as the second branch */
            HighLevelTable[1] = MediumLevelTable;

            /* Encode the table pointer */
            CapturedTable = ((ULONG_PTR)HighLevelTable) | 2;

            /* Publish the high-level table structure */
            RTL::Atomic::ExchangePointer((PVOID *)&HandleTable->TableCode, (PVOID)CapturedTable);
        }
    }
    else if(TableLevel == 2)
    {
        /* Cast the captured table pointer to a high-level directory array */
        HighLevelTableEntry = (PHANDLE_TABLE_ENTRY **)CapturedTable;

        /* Calculate the root index into the high-level table */
        HighLevelIndex = HandleTable->NextHandleNeedingPool /
                         (OBJECT_HANDLE_MEDIUMLEVEL_THRESHOLD * OBJECT_HANDLE_VALUE_INCREMENT);

        /* Ensure there is room in the high-level table for the entry */
        if(HighLevelIndex >= OBJECT_HANDLE_HIGHLEVEL_COUNT)
        {
            /* Table is full, return FALSE */
            return FALSE;
        }

        /* Check if the required medium-level directory branch needs to be created */
        if(!HighLevelTableEntry[HighLevelIndex])
        {
            /* Allocate a medium-level table directory */
            MediumLevelTable = AllocateMediumLevelTable(HandleTable, Initialize, &LowLevelTable);
            if(!MediumLevelTable)
            {
                /* Table allocation failed, return FALSE */
                return FALSE;
            }

            /* Publish the medium-level directory */
            RTL::Atomic::ExchangePointer((PVOID *)&HighLevelTableEntry[HighLevelIndex], MediumLevelTable);
        }
        else
        {
            /* Calculate the localized index within the medium-level branch */
            RemainingIndex = (HandleTable->NextHandleNeedingPool / OBJECT_HANDLE_VALUE_INCREMENT) -
                             HighLevelIndex * OBJECT_HANDLE_MEDIUMLEVEL_THRESHOLD;
            MediumLevelIndex = RemainingIndex / OBJECT_HANDLE_LOWLEVEL_COUNT;

            /* Allocate a low-level table leaf */
            LowLevelTable = AllocateLowLevelTable(HandleTable, Initialize);
            if(!LowLevelTable)
            {
                /* Table allocation failed, return FALSE */
                return FALSE;
            }

            /* Publish the low-level table */
            RTL::Atomic::ExchangePointer((PVOID *)(&HighLevelTableEntry[HighLevelIndex][MediumLevelIndex]),
                                         LowLevelTable);
        }
    }

    /* Reserve the allocated index block and capture the base handle value */
    OldIndex = RTL::Atomic::ExchangeAdd32((PLONG)&HandleTable->NextHandleNeedingPool,
                                          OBJECT_HANDLE_LOWLEVEL_COUNT * OBJECT_HANDLE_VALUE_INCREMENT);

    /* Check if the initialization flag is set */
    if(Initialize)
    {
        /* Calculate the starting index */
        FirstFreeIndex = OldIndex + OBJECT_HANDLE_VALUE_INCREMENT;

        /* Enter exchange loop */
        while(TRUE)
        {
            /* Capture the state of the primary free list */
            FirstFree = *(VOLATILE ULONG *)&HandleTable->FirstFreeHandle;

            /* Chain the tail of allocated page to the current list head */
            LowLevelTable[OBJECT_HANDLE_LOWLEVEL_COUNT - 1].NextFreeTableEntry = FirstFree;

            /* Swap the new chain head */
            if (FirstFree == (ULONG)RTL::Atomic::CompareExchange32((PLONG)&HandleTable->FirstFreeHandle,
                                                                          FirstFreeIndex,
                                                                          FirstFree))
            {
                /* Exchange succeeded, break the loop */
                break;
            }
        }
    }

    /* Handle allocation succeeded, return TRUE */
    return TRUE;
}

/**
 * Allocates a new handle table entry.
 *
 * @param HandleTable
 *        Supplies the handle table to allocate from.
 *
 * @param Handle
 *        Receives the allocated handle value.
 *
 * @return This routine returns a pointer to the allocated entry, or NULL pointer on failure.
 *
 * @since XT 1.0
 */
XTAPI
PHANDLE_TABLE_ENTRY
OB::HandleTable::AllocateHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                          OUT POBJECT_HANDLE Handle)
{
    ULONG FreeHandle, LockIndex, NextFreeHandle, OldFreeHandle;
    PHANDLE_TABLE_ENTRY TableEntry;
    BOOLEAN ExpansionSucceeded;
    OBJECT_HANDLE LocalHandle;
    PKTHREAD CurrentThread;

    /* Retrieve the currently executing thread */
    CurrentThread = KE::Processor::GetCurrentThread();

    /* Enter the retry loop */
    while(TRUE)
    {
        /* Get the current head of the primary free list */
        OldFreeHandle = (ULONG)(ULONG_PTR)HandleTable->FirstFreeHandle;

        /* Enter the replenishment loop */
        while(!OldFreeHandle)
        {
            /* Start a guarded code block */
            {
                /* Acquire a critical region guard */
                KE::CriticalRegionGuard CriticalRegion(CurrentThread);

                /* Acquire the handle table lock */
                KE::PushLockExclusiveGuard PushLock(&HandleTable->HandleTableLock[0]);

                /* Check if another thread replenished the free list */
                OldFreeHandle = (ULONG)(ULONG_PTR)HandleTable->FirstFreeHandle;
                if(OldFreeHandle)
                {
                    /* The free list was replenished, break the loop execution */
                    break;
                }

                /* Pull available handles */
                OldFreeHandle = MoveFreeHandles(HandleTable);
                if(OldFreeHandle)
                {
                    /* Break the loop execution */
                    break;
                }

                /* Allocate a handle table entry */
                ExpansionSucceeded = AllocateHandleTableEntry(HandleTable, TRUE);
            }

            /* Get the free list head */
            OldFreeHandle = (ULONG)(ULONG_PTR)HandleTable->FirstFreeHandle;

            /* Check if the block allocation failed */
            if(!ExpansionSucceeded)
            {
                /* Verify if the free list remains depleted */
                if(!OldFreeHandle)
                {
                    /* Fail the allocation request and return NULL pointer */
                    Handle->GenericHandleOverlay = NULLPTR;
                    return NULLPTR;
                }
            }
        }

        /* Extract the handle value and retrieve its underlying table entry */
        LocalHandle.Value = (OldFreeHandle & OBJECT_FREE_HANDLE_MASK);
        TableEntry = LookupHandleTableEntry(HandleTable, LocalHandle);

        /* Calculate the specific lock index */
        LockIndex = ((OldFreeHandle & OBJECT_FREE_HANDLE_MASK) >> 2) % OBJECT_HANDLE_VALUE_INCREMENT;

        /* Start a guarded code block */
        {
            /* Acquire a critical region guard */
            KE::CriticalRegionGuard CriticalRegion(CurrentThread);

            /* Acquire the handle table lock */
            KE::PushLockSharedGuard PushLock(&HandleTable->HandleTableLock[LockIndex]);

            /* Verify that the free list head has not been modified */
            if(OldFreeHandle != *(volatile ULONG *)&HandleTable->FirstFreeHandle)
            {
                /* The list changed, restart the allocation */
                continue;
            }

            /* Memory barrier */
            AR::CpuFunctions::MemoryBarrier();

            /* Retrieve the index of the next free entry in the chain */
            NextFreeHandle = *(volatile ULONG *)&TableEntry->NextFreeTableEntry;

            /* Replace the free list head with the next index */
            FreeHandle = RTL::Atomic::CompareExchange32((PLONG)&HandleTable->FirstFreeHandle,
                                                             NextFreeHandle,
                                                             OldFreeHandle);
        }

        /* Check if the replace succeeded */
        if(FreeHandle == OldFreeHandle)
        {
            /* Exit the retry loop */
            break;
        }
    }

    /* Increment the handle allocation count */
    RTL::Atomic::Increment32(&HandleTable->HandleCount);

    /* Store the handle and return a pointer to the allocated entry */
    *Handle = LocalHandle;
    return TableEntry;
}

/**
 * Allocates a low-level handle table page.
 *
 * @param HandleTable
 *        Supplies the parent handle table.
 *
 * @param Initialize
 *        Indicates whether to initialize the free list within the page.
 *
 * @return This routine returns a pointer to the allocated table, or NULL pointer on failure.
 *
 * @since XT 1.0
 */
XTAPI
PHANDLE_TABLE_ENTRY
OB::HandleTable::AllocateLowLevelTable(IN PHANDLE_TABLE HandleTable,
                                       IN BOOLEAN Initialize)
{
    PHANDLE_TABLE_ENTRY HandleEntry, LowLevelEntry;
    ULONG BaseHandle, Index;

    /* Allocate a memory block for the handle table */
    LowLevelEntry = (PHANDLE_TABLE_ENTRY)AllocateTableMemory(HandleTable->QuotaProcess, MM_PAGE_SIZE, FALSE);
    if(!LowLevelEntry)
    {
        /* Memory allocation failed, return a NULL pointer */
        return NULLPTR;
    }

    /* Initialize the first entry of the page as the extended information header */
    HandleEntry = &LowLevelEntry[0];
    HandleEntry->NextFreeTableEntry = OBJECT_HANDLE_ADDITIONAL_INFO_SIGNATURE;
    HandleEntry->Value = 0;

    /* Check if initialization requested */
    if(Initialize)
    {
        /* Advance to the first usable handle entry */
        HandleEntry++;

        /* Calculate the base handle index */
        BaseHandle = HandleTable->NextHandleNeedingPool + 2 * OBJECT_HANDLE_VALUE_INCREMENT;

        /* Iterate through the table block */
        for(Index = BaseHandle;
            Index < BaseHandle + (OBJECT_HANDLE_LOWLEVEL_COUNT - 2) * OBJECT_HANDLE_VALUE_INCREMENT;
            Index += OBJECT_HANDLE_VALUE_INCREMENT)
        {
            /* Link the current entry to the next available handle index */
            HandleEntry->NextFreeTableEntry = Index;
            HandleEntry->Value = 0;

            /* Advance to the next handle table entry */
            HandleEntry++;
        }

        /* Terminate the free list chain */
        HandleEntry->NextFreeTableEntry = 0;
        HandleEntry->Value = 0;
    }

    /* Return the pointer to the handle table */
    return LowLevelEntry;
}

/**
 * Allocates a mid-level table with an initial low-level table.
 *
 * @param HandleTable
 *        Supplies the parent handle table.
 *
 * @param Initialize
 *        Indicates whether to initialize the free list.
 *
 * @param LowLevelEntry
 *        Receives the pointer to the allocated low-level table.
 *
 * @return This routine returns a pointer to the mid-level table, or NULL on failure.
 *
 * @since XT 1.0
 */
PHANDLE_TABLE_ENTRY*
OB::HandleTable::AllocateMediumLevelTable(IN PHANDLE_TABLE HandleTable,
                                          IN BOOLEAN Initialize,
                                          OUT PHANDLE_TABLE_ENTRY *LowLevelEntry)
{
    PHANDLE_TABLE_ENTRY *MediumLevelEntry;
    PHANDLE_TABLE_ENTRY LowLevel;

    /* Allocate a memory block for the medium-level handle table */
    MediumLevelEntry = (PHANDLE_TABLE_ENTRY*)AllocateTableMemory(HandleTable->QuotaProcess, MM_PAGE_SIZE, TRUE);
    if(!MediumLevelEntry)
    {
        /* Memory allocation failed, return a NULL pointer */
        return NULLPTR;
    }

    /* Allocate the initial low-level handle table page */
    LowLevel = AllocateLowLevelTable(HandleTable, Initialize);
    if(!LowLevel)
    {
        /* Allocation failed, free the medium-level table and return a NULL pointer */
        FreeTableMemory(HandleTable->QuotaProcess, MediumLevelEntry, MM_PAGE_SIZE);
        return NULLPTR;
    }

    /* Link the initial table into the first slot */
    MediumLevelEntry[0] = LowLevel;
    *LowLevelEntry = LowLevel;

    /* Return the medium-level handle table */
    return MediumLevelEntry;
}

/**
 * Allocates a memory block for handle table structures.
 *
 * @param QuotaProcess
 *        Optionally supplies a pointer to the process object that will be charged for the memory quota.
 *
 * @param Size
 *        Supplies the size, in bytes, of the memory block to allocate.
 *
 * @param ZeroMemory
 *        Supplies a boolean value indicating whether the allocated memory block should be zero-initialized.
 *
 * @return This routine returns a pointer to the newly allocated memory block, or NULLPTR if the allocation fails.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
OB::HandleTable::AllocateTableMemory(IN PEPROCESS QuotaProcess,
                                     IN SIZE_T Size,
                                     IN BOOLEAN ZeroMemory)
{
    PVOID MemoryBuffer;
    XTSTATUS Status;

    /* Allocate a paged pool memory block with the object table tag */
    MM::Allocator::AllocatePool(PagedPool, Size, &MemoryBuffer, TAG_OB_OBJECT_TABLE);
    if(!MemoryBuffer)
    {
        /* Memory allocation failed, return a NULL pointer */
        return NULLPTR;
    }

    /* Check if the caller requested zero-initialized memory */
    if(ZeroMemory)
    {
        /* Clear the contents of the allocated memory block */
        RTL::Memory::ZeroMemory(MemoryBuffer, Size);
    }

    /* Check if a quota process was specified */
    if(QuotaProcess)
    {
        /* Charge the paged pool memory allocation against the target process quota */
        Status = PS::Quota::ChargeProcessQuota(QuotaProcess->QuotaBlock, QuotaProcess, PsPagedPool, Size);
        if(Status != STATUS_SUCCESS)
        {
            /* Quota charge failed, free the allocated memory and return a NULL pointer */
            MM::Allocator::FreePool(MemoryBuffer, TAG_OB_OBJECT_TABLE);
            return NULLPTR;
        }
    }

    /* Return a pointer to the allocated memory block */
    return MemoryBuffer;
}

/**
 * Changes the contents of a handle entry via a callback.
 *
 * @param HandleTable
 *        Supplies the handle table containing the entry.
 *
 * @param Handle
 *        Supplies the handle value to modify.
 *
 * @param Callback
 *        Supplies the callback that performs the modification.
 *
 * @param Parameter
 *        Supplies an opaque parameter passed to the callback.
 *
 * @return This routine returns TRUE if the change succeeded, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
OB::HandleTable::ChangeHandle(IN PHANDLE_TABLE HandleTable,
                              IN HANDLE Handle,
                              IN POB_CHANGE_HANDLE_ROUTINE Callback,
                              IN ULONG_PTR Parameter
    )
{
    PHANDLE_TABLE_ENTRY HandleTableEntry;
    OBJECT_HANDLE LocalHandle;
    PKTHREAD CurrentThread;
    BOOLEAN ReturnValue;

    /* Initialize the default return value */
    ReturnValue = FALSE;

    /* Overlay the generic handle value */
    LocalHandle.GenericHandleOverlay = Handle;

    /* Retrieve the currently executing thread */
    CurrentThread = KE::Processor::GetCurrentThread();

    /* Retrieve the corresponding handle table entry */
    HandleTableEntry = LookupHandleTableEntry(HandleTable, LocalHandle);

    /* Ensure the entry exists and points to an active object */
    if(HandleTableEntry && ValidateObjectEntry(HandleTableEntry))
    {
        /* Acquire a critical region guard */
        KE::CriticalRegionGuard CriticalRegion(CurrentThread);

        /* Acquire the exclusive lock for this entry */
        if(LockHandleTableEntry(HandleTable, HandleTableEntry))
        {
            /* Execute the provided callback */
            ReturnValue = (*Callback)(HandleTableEntry, Parameter);

            /* Release the lock */
            UnlockHandleTableEntry(HandleTable, HandleTableEntry);
        }
    }

    /* Return the callback result */
    return ReturnValue;
}

/**
 * Creates a new handle entry in a handle table.
 *
 * @param HandleTable
 *        Supplies the handle table to create the entry in.
 *
 * @param HandleTableEntry
 *        Supplies the template entry to copy.
 *
 * @return This routine returns the created handle value, or NULL on failure.
 *
 * @since XT 1.0
 */
XTAPI
HANDLE
OB::HandleTable::CreateHandle(IN PHANDLE_TABLE HandleTable,
                              IN PHANDLE_TABLE_ENTRY HandleTableEntry)
{
    PHANDLE_TABLE_ENTRY NewHandleTableEntry;
    PETHREAD CurrentThread;
    OBJECT_HANDLE Handle;

    /* Initialize the generic handle overlay */
    Handle.GenericHandleOverlay = NULLPTR;

    /* Allocate a new handle table entry */
    NewHandleTableEntry = AllocateHandleTableEntry(HandleTable, &Handle);
    if(NewHandleTableEntry)
    {
        /* Retrieve the current thread object */
        CurrentThread = PS::Thread::GetCurrentThread();

        /* Acquire a critical region guard */
        KE::CriticalRegionGuard CriticalRegion(&CurrentThread->ThreadControlBlock);

        /* Copy the contents of the template entry */
        *NewHandleTableEntry = *HandleTableEntry;

        /* Unlock the handle table entry */
        UnlockHandleTableEntry(HandleTable, NewHandleTableEntry);
    }

    /* Return the resulting handle value */
    return Handle.GenericHandleOverlay;
}

/**
 * Creates and initializes a new handle table.
 *
 * @param Process
 *        Supplies the optional process to charge quota against.
 *
 * @return This routine returns a pointer to the handle table, or NULL pointer on failure.
 *
 * @since XT 1.0
 */
XTAPI
PHANDLE_TABLE
OB::HandleTable::CreateHandleTable(IN PEPROCESS Process)
{
    PKTHREAD CurrentThread;
    PHANDLE_TABLE HandleTable;

    /* Get the current thread */
    CurrentThread = KE::Processor::GetCurrentThread();

    /* Allocate a new handle table */
    HandleTable = AllocateHandleTable(Process, TRUE);
    if(!HandleTable)
    {
        /* Handle table allocation failure, return NULL pointer */
        return NULLPTR;
    }

    /* Enter a critical region and acquire the handle table lock */
    KE::CriticalRegionGuard CriticalRegion(CurrentThread);
    KE::PushLockExclusiveGuard PushLock(&HandleTableListLock);

    /* Insert the new handle table into the global list */
    RTL::LinkedList::InsertTailList(&HandleTableListHead, &HandleTable->HandleTableList);

    /* Return the new handle table */
    return HandleTable;
}

/**
 * Removes a handle from a handle table.
 *
 * @param HandleTable
 *        Supplies the handle table containing the handle.
 *
 * @param Handle
 *        Supplies the handle value to remove.
 *
 * @param HandleTableEntry
 *        Optionally supplies a pre-locked entry pointer.
 *
 * @return This routine returns TRUE if the handle was removed, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
OB::HandleTable::DestroyHandle(IN PHANDLE_TABLE HandleTable,
                               IN HANDLE Handle,
                               IN PHANDLE_TABLE_ENTRY HandleTableEntry)
{
    OBJECT_HANDLE LocalHandle;
    PETHREAD CurrentThread;

    /* Capture the provided handle value */
    LocalHandle.GenericHandleOverlay = Handle;

    /* Retrieve the current thread object */
    CurrentThread = PS::Thread::GetCurrentThread();

    /* Acquire a critical region guard */
    KE::CriticalRegionGuard CriticalRegion(&CurrentThread->ThreadControlBlock);

    /* Check if a handle table entry was provided by the caller */
    if(!HandleTableEntry)
    {
        /* Resolve the local handle to its corresponding handle table entry */
        HandleTableEntry = LookupHandleTableEntry(HandleTable, LocalHandle);

        /* Verify that the resolved entry points to a valid object */
        if(!ValidateObjectEntry(HandleTableEntry))
        {
            /* Invalid object entry, return FALSE */
            return FALSE;
        }

        /* Acquire the lock */
        if(!LockHandleTableEntry(HandleTable, HandleTableEntry))
        {
            /* Entry locking failed, return FALSE */
            return FALSE;
        }
    }

    /* Clear the object pointer  */
    RTL::Atomic::ExchangePointer(&HandleTableEntry->Object, NULLPTR);

    /* Unblock any threads waiting on the handle table */
    KE::PushLock::UnblockPushLock(&HandleTable->HandleContentionEvent, NULLPTR);

    /* Release the handle table entry */
    FreeHandleTableEntry(HandleTable, LocalHandle, HandleTableEntry);

    /* Return TRUE */
    return TRUE;
}

/**
 * Destroys a handle table and all its entries.
 *
 * @param HandleTable
 *        Supplies the handle table to destroy.
 *
 * @param Callback
 *        Supplies an optional callback invoked for each valid handle.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
VOID
OB::HandleTable::DestroyHandleTable(IN PHANDLE_TABLE HandleTable,
                                    IN POB_DESTROY_HANDLE_ROUTINE Callback)
{
    PHANDLE_TABLE_ENTRY HandleTableEntry;
    OBJECT_HANDLE Handle;

    /* Remove the handle table from the tracking list */
    RemoveHandleTable(HandleTable);

    /* Check if a handle destruction callback was provided */
    if(Callback)
    {
        /* Initialize the handle */
        Handle.Value = 0;

        /* Iterate through all handles within the table */
        while(TRUE)
        {
            /* Retrieve the underlying handle table entry */
            HandleTableEntry = LookupHandleTableEntry(HandleTable, Handle);
            if(!HandleTableEntry)
            {
                /* Handle is out of bounds, abort the enumeration */
                break;
            }

            /* Validate that the entry points to a registered object */
            if(ValidateObjectEntry(HandleTableEntry))
            {
                /* Invoke the destruction callback */
                (*Callback)(Handle.GenericHandleOverlay);
            }

            /* Advance to the next handle value */
            Handle.Value += OBJECT_HANDLE_VALUE_INCREMENT;
        }
    }

    /* Release all memory */
    FreeHandleTable(HandleTable);
}

/**
 * Duplicates copy of a handle table.
 *
 * @param Process
 *        Supplies the optional process to charge quota against.
 *
 * @param OldHandleTable
 *        Supplies the source handle table to duplicate.
 *
 * @param Callback
 *        Supplies the callback invoked for each duplicated handle.
 *
 * @param Mask
 *        Supplies a mask applied to object pointers during duplication.
 *
 * @return This routine returns a pointer to the new handle table, or NULL pointer on failure.
 *
 * @since XT 1.0
 */
XTAPI
PHANDLE_TABLE
OB::HandleTable::DuplicateHandleTable(IN PEPROCESS Process,
                                      IN PHANDLE_TABLE OldHandleTable,
                                      IN POB_DUPLICATE_HANDLE_ROUTINE Callback,
                                      IN ULONG_PTR Mask)
{
    PHANDLE_TABLE_ENTRY NewHandleTableEntry, OldHandleTableEntry;
    PHANDLE_TABLE_ENTRY_INFO EntryInfo;
    PHANDLE_TABLE DuplicateHandleTable;
    ULONG Index, RemainingEntries;
    BOOLEAN FreeEntry, Ready;
    PKTHREAD CurrentThread;
    OBJECT_HANDLE Handle;

    /* Retrieve the currently executing thread */
    CurrentThread = KE::Processor::GetCurrentThread();

    /* Allocate a handle table */
    DuplicateHandleTable = AllocateHandleTable(Process, FALSE);
    if(!DuplicateHandleTable)
    {
        /* Allocation failed, return NULL pointer */
        return NULLPTR;
    }

    /* Expand the handle table to match the capacity */
    while(DuplicateHandleTable->NextHandleNeedingPool < OldHandleTable->NextHandleNeedingPool)
    {
        /* Allocate the handle table entries */
        if(!AllocateHandleTableEntry(DuplicateHandleTable, FALSE))
        {
            /* Allocation failed, free handle table and return NULL pointer */
            FreeHandleTable(DuplicateHandleTable);
            return NULLPTR;
        }
    }

    /* Memory barrier */
    AR::CpuFunctions::MemoryBarrier();

    /* Initialize the base state of the cloned table */
    DuplicateHandleTable->ExtraInfoPages = 0;
    DuplicateHandleTable->FirstFreeHandle = 0;
    DuplicateHandleTable->HandleCount = 0;

    /* Initialize the handle value */
    Handle.Value = OBJECT_HANDLE_VALUE_INCREMENT;

    /* Acquire a critical region guard */
    KE::CriticalRegionGuard CriticalRegion(CurrentThread);

    /* Enumerate all allocated handle table entry layers */
    while(TRUE)
    {
        /* Retrieve the physical pointer to the handle table entry */
        NewHandleTableEntry = LookupHandleTableEntry(DuplicateHandleTable, Handle);
        if(!NewHandleTableEntry)
        {
            /* Allocated table layers exhausted, break the enumeration loop */
            break;
        }

        /* Retrieve the  pointer from the source table */
        OldHandleTableEntry = LookupHandleTableEntry(OldHandleTable, Handle);

        /* Calculate how many entries remain to be processed */
        RemainingEntries = OBJECT_HANDLE_LOWLEVEL_COUNT -
                           ((Handle.Value / OBJECT_HANDLE_VALUE_INCREMENT) % OBJECT_HANDLE_LOWLEVEL_COUNT);

        /* Process all entries */
        for(Index = 0; Index < RemainingEntries; Index++)
        {
            /* Check if the entry matches the required mask and lock it */
            if(!(OldHandleTableEntry->Value & Mask) || !LockHandleTableEntry(OldHandleTable, OldHandleTableEntry))
            {
                /* Invalid or locked entry, push to free list */
                FreeEntry = TRUE;
            }
            else
            {
                /* Perform a raw memory copy */
                *NewHandleTableEntry = *OldHandleTableEntry;

                /* Retrieve extended information attached to the handle */
                EntryInfo = GetHandleInformation(OldHandleTable, Handle.GenericHandleOverlay, TRUE);
                if(EntryInfo)
                {
                    /* Check the readiness of the entry */
                    Ready = (SetHandleInformation(DuplicateHandleTable, Handle.GenericHandleOverlay,
                                                  EntryInfo, TRUE) == STATUS_SUCCESS);
                }
                else
                {
                    /* No extended info exists, ready to proceed */
                    Ready = TRUE;
                }

                /* Verify the readiness */
                if(Ready)
                {
                    /* Invoke the duplication callback */
                    if((*Callback)(Process, OldHandleTable, OldHandleTableEntry, NewHandleTableEntry))
                    {
                        /* The callback accepted the duplication */
                        FreeEntry = FALSE;

                        /* Increment the handle count and ensure the new entry remains locked */
                        DuplicateHandleTable->HandleCount += 1;
                        NewHandleTableEntry->Value |= OBJECT_HANDLE_TABLE_ENTRY_LOCK_BIT;
                    }
                    else
                    {
                        /* Check if extended information was attached */
                        if(EntryInfo)
                        {
                            /* Clear the audit mask */
                            EntryInfo->AuditMask = 0;
                        }

                        /* Discarded entry, push to free list */
                        FreeEntry = TRUE;
                    }
                }
                else
                {
                    /* Setup failed, release the source entry lock */
                    UnlockHandleTableEntry(OldHandleTable, OldHandleTableEntry);

                    /* Unusable entry, push to free list */
                    FreeEntry = TRUE;
                }
            }

            /* Process entries designated for the free list */
            if(FreeEntry)
            {
                /* Link the unused entry into the free list chain */
                NewHandleTableEntry->NextFreeTableEntry = (ULONG)(ULONG_PTR)DuplicateHandleTable->FirstFreeHandle;
                NewHandleTableEntry->Object = NULLPTR;

                /* Update the head of the free list */
                DuplicateHandleTable->FirstFreeHandle = (PHANDLE_TABLE_ENTRY)(ULONG_PTR)Handle.Value;
            }

            /* Advance the logical handle value */
            Handle.Value += OBJECT_HANDLE_VALUE_INCREMENT;

            /* Advance the table entry pointers */
            NewHandleTableEntry++;
            OldHandleTableEntry++;
        }
    }

    /* Acquire the handle table lock */
    KE::PushLockExclusiveGuard PushLock(&HandleTableListLock);

    /* Insert the handle table into the global tracking list */
    RTL::LinkedList::InsertTailList(&HandleTableListHead, &DuplicateHandleTable->HandleTableList);

    /* Return the duplicated handle table */
    return DuplicateHandleTable;
}

/**
 * Enumerates all valid handles in a handle table.
 *
 * @param HandleTable
 *        Supplies the handle table to enumerate.
 *
 * @param Callback
 *        Supplies the callback invoked for each valid handle.
 *
 * @param Parameter
 *        Supplies an opaque parameter passed to the callback.
 *
 * @param Handle
 *        Optionally receives the handle where enumeration stopped.
 *
 * @return This routine returns TRUE if enumeration stopped early, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
OB::HandleTable::EnumerateHandleTable(IN PHANDLE_TABLE HandleTable,
                                      IN POB_ENUMERATE_HANDLE_ROUTINE Callback,
                                      IN PVOID Parameter,
                                      OUT PHANDLE Handle)
{
    PHANDLE_TABLE_ENTRY HandleTableEntry;
    OBJECT_HANDLE LocalHandle;
    PKTHREAD CurrentThread;
    BOOLEAN ResultValue;

    /* Initialize the default return value */
    ResultValue = FALSE;

    /* Retrieve the currently executing thread */
    CurrentThread = KE::Processor::GetCurrentThread();

    /* Acquire a critical region guard */
    KE::CriticalRegionGuard CriticalRegion(CurrentThread);

    /* Initialize the handle value */
    LocalHandle.Value = 0;

    /* Iterate through all handles within the table */
    while(TRUE)
    {
        /* Retrieve the handle table entry for the current value */
        HandleTableEntry = LookupHandleTableEntry(HandleTable, LocalHandle);
        if(!HandleTableEntry)
        {
            /* Handle is out of bounds, abort enumeration */
            break;
        }

        /* Validate that the entry points to a registered object */
        if(ValidateObjectEntry(HandleTableEntry))
        {
            /* Acquire the  lock */
            if(LockHandleTableEntry(HandleTable, HandleTableEntry))
            {
                /* Invoke the enumeration callback */
                ResultValue = (*Callback)(HandleTableEntry, LocalHandle.GenericHandleOverlay, Parameter);

                /* Release the lock */
                UnlockHandleTableEntry(HandleTable, HandleTableEntry);

                /* Check if the callback requested termination */
                if(ResultValue)
                {
                    /* Check if the caller provided an output parameter */
                    if(Handle)
                    {
                        /* Store the handle */
                        *Handle = LocalHandle.GenericHandleOverlay;
                    }

                    /* Exit the enumeration loop */
                    break;
                }
            }
        }

        /* Advance to the next handle value */
        LocalHandle.Value += OBJECT_HANDLE_VALUE_INCREMENT;
    }

    /* Return enumeration result */
    return ResultValue;
}

/**
 * Frees and tears down a handle table structure.
 *
 * @param HandleTable
 *        Supplies the handle table to free.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
VOID
OB::HandleTable::FreeHandleTable(IN PHANDLE_TABLE HandleTable)
{
    PHANDLE_TABLE_ENTRY **HighLevelTable, LowLevelTable, *MediumLevelTable;
    ULONG HighLevelIndex, MediumLevelIndex, TableLevel;
    ULONG_PTR CapturedTable;
    PEPROCESS Process;


    /* Capture the current table */
    CapturedTable = HandleTable->TableCode;

    /* Extract the handle table depth level */
    TableLevel = (ULONG)(CapturedTable & OBJECT_HANDLE_LEVEL_CODE_MASK);

    /* Mask out the level bits */
    CapturedTable = CapturedTable & ~OBJECT_HANDLE_LEVEL_CODE_MASK;

    /* Retrieve the quota process associated with handle table */
    Process = HandleTable->QuotaProcess;

    /* Check if the handle table is currently operating as a single-level structure */
    if(!TableLevel)
    {
        /* Extract the low-level table */
        LowLevelTable = (PHANDLE_TABLE_ENTRY)CapturedTable;

        /* Free the low-level directly */
        FreeLowLevelTable(Process, LowLevelTable);
    }
    else if(TableLevel == 1)
    {
        /* Extract the medium-level directory */
        MediumLevelTable = (PHANDLE_TABLE_ENTRY *)CapturedTable;

        /* Initialize the medium-level directory index */
        MediumLevelIndex = 0;

        /* Enumerate and free all allocated low-level leaves */
        while(TRUE)
        {
            /* Check if the maximum count reached */
            if(MediumLevelIndex >= OBJECT_HANDLE_MEDIUMLEVEL_COUNT || !MediumLevelTable[MediumLevelIndex])
            {
                /* Break the enumeration loop */
                break;
            }

            /* Free the low-level table leaf */
            FreeLowLevelTable(Process, MediumLevelTable[MediumLevelIndex]);

            /* Advance to the next medium-level directory slot */
            MediumLevelIndex++;
        }

        /* Free the medium-level directory itself */
        FreeTableMemory(Process, MediumLevelTable, MM_PAGE_SIZE);
    }
    else
    {
        /* Extract the high-level directory */
        HighLevelTable = (PHANDLE_TABLE_ENTRY **)CapturedTable;

        /* Initialize the high-level root index */
        HighLevelIndex = 0;

        /* Enumerate and free all allocated medium-level directories */
        while(TRUE)
        {
            /* Check if the maximum count reached  */
            if(HighLevelIndex >= OBJECT_HANDLE_HIGHLEVEL_COUNT || !HighLevelTable[HighLevelIndex])
            {
                /* Break the enumeration loop */
                break;
            }

            /* Initialize the medium-level directory index */
            MediumLevelIndex = 0;

            /* Enumerate and free all allocated low-level leaves */
            while(TRUE)
            {
                /* Check if the maximum count reached */
                if(MediumLevelIndex >= OBJECT_HANDLE_MEDIUMLEVEL_COUNT ||
                   !HighLevelTable[HighLevelIndex][MediumLevelIndex])
                {
                    /* Break the inner enumeration loop */
                    break;
                }

                /* Free the low-level table leaf */
                FreeLowLevelTable(Process, HighLevelTable[HighLevelIndex][MediumLevelIndex]);

                /* Advance to the next mediun-level directory slot */
                MediumLevelIndex++;
            }

            /* Free the current medium-level directory page */
            FreeTableMemory(Process, HighLevelTable[HighLevelIndex], MM_PAGE_SIZE);

            /* Advance to the next high-level directory slot */
            HighLevelIndex++;
        }

        /* Free the high-level root directory itself */
        FreeTableMemory(Process, HighLevelTable, OBJECT_HANDLE_HIGHLEVEL_SIZE);
    }

    /* Free the base handle table */
    FreeTableMemory(Process, HandleTable, sizeof(HANDLE_TABLE));
}

/**
 * Frees a handle table entry back to the free list.
 *
 * @param HandleTable
 *        Supplies the parent handle table.
 *
 * @param Handle
 *        Supplies the handle being freed.
 *
 * @param HandleTableEntry
 *        Supplies the entry being freed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
VOID
OB::HandleTable::FreeHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                      IN OBJECT_HANDLE Handle,
                                      IN PHANDLE_TABLE_ENTRY HandleTableEntry)
{
    ULONG HandleValue, ListHead, LockIndex, SequenceTag;
    PHANDLE_TABLE_ENTRY_INFO EntryInfo;
    PULONG TargetListPointer;

    /* Retrieve the extended information attached to the handle */
    EntryInfo = GetHandleInformation(HandleTable, Handle.GenericHandleOverlay, TRUE);
    if(EntryInfo)
    {
        /* Clear the audit mask */
        EntryInfo->AuditMask = 0;
    }

    /* Decrement the handle allocation count */
    RTL::Atomic::Decrement32(&HandleTable->HandleCount);

    /* Mask out the lower control bits */
    HandleValue = (ULONG)Handle.Value & ~(OBJECT_HANDLE_VALUE_INCREMENT - 1);

    /* Check if strict FIFO mode is enabled */
    if(!HandleTable->StrictFIFO)
    {
        /* Calculate the lock index */
        LockIndex = (HandleValue >> 2) % OBJECT_HANDLE_VALUE_INCREMENT;

        /* Acquire the segment lock */
        if(KE::PushLock::TryAcquireExclusivePushLock(&HandleTable->HandleTableLock[LockIndex]))
        {
            /* Lock acquired, use the free list */
            SequenceTag = 0;
            TargetListPointer = (PULONG)&HandleTable->FirstFreeHandle;
        }
        else
        {
            /* Lock contended, use the delayed list */
            SequenceTag = 0;
            TargetListPointer = (PULONG)&HandleTable->LastFreeHandle;
        }
    }
    else
    {
        /* Send freed handle to the delayed list */
        SequenceTag = 0;
        TargetListPointer = (PULONG)&HandleTable->LastFreeHandle;
    }

    /* Enter a loop to publish the freed handle */
    while(TRUE)
    {
        /* Capture the state of the targeted list head */
        ListHead = *(VOLATILE ULONG *)TargetListPointer;

        /* Chain the freed table entry to the list head */
        HandleTableEntry->NextFreeTableEntry = ListHead;

        /* Publish the freed handle */
        if((ULONG)RTL::Atomic::CompareExchange32((PLONG)TargetListPointer,
                                                 HandleValue + SequenceTag,
                                                 ListHead) == ListHead)
        {
            /* Publish succeeded, break the loop */
            break;
        }
    }
}

/**
 * Frees a low-level handle table page.
 *
 * @param QuotaProcess
 *        Supplies the process to return quota to.
 *
 * @param TableEntry
 *        Supplies the low-level table to free.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
VOID
OB::HandleTable::FreeLowLevelTable(IN PEPROCESS QuotaProcess,
                                   IN PHANDLE_TABLE_ENTRY TableEntry)
{
    /* Check if an extended information table is attached */
    if(TableEntry[0].Object)
    {
        /* Free the handle information table */
        FreeTableMemory(QuotaProcess, TableEntry[0].Object,
                        OBJECT_HANDLE_LOWLEVEL_COUNT * sizeof(HANDLE_TABLE_ENTRY_INFO));
    }

    /* Release the handle table */
    FreeTableMemory(QuotaProcess, TableEntry, MM_PAGE_SIZE);
}

/**
 * Frees a handle table memory block and returns the associated quota.
 *
 * @param QuotaProcess
 *        Optionally supplies a pointer to the process object that will receive the memory quota reimbursement.
 *
 * @param MemoryBuffer
 *        Supplies a pointer to the allocated pool memory block to be freed.
 *
 * @param Size
 *        Supplies the size, in bytes, of the memory block being freed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::HandleTable::FreeTableMemory(IN PEPROCESS QuotaProcess,
                                 IN PVOID MemoryBuffer,
                                 IN SIZE_T Size)
{
    /* Release the allocated pool memory block associated with the object table tag */
    MM::Allocator::FreePool(MemoryBuffer, TAG_OB_OBJECT_TABLE);

    /* Check if a valid quota process object was specified */
    if(QuotaProcess)
    {
        /* Reimburse the paged pool memory quota to the specified process */
        PS::Quota::ReturnProcessQuota(QuotaProcess->QuotaBlock, QuotaProcess, PsPagedPool, Size);
    }
}

/**
 * Retrieves the extended information for a specific handle table entry.
 *
 * @param HandleTable
 *        Supplies a pointer to the handle table containing the handle.
 *
 * @param Handle
 *        Supplies the handle for which the extended information is requested.
 *
 * @param Locked
 *        Specifies whether the handle table entry is currently locked by the caller.
 *
 * @return This routine returns a pointer to the handle table entry information, or NULL pointer if the table
 *         does not support extra info pages or lookup fails.
 *
 * @since XT 1.0
 */
XTAPI
PHANDLE_TABLE_ENTRY_INFO
OB::HandleTable::GetHandleInformation(IN PHANDLE_TABLE HandleTable,
                                      IN HANDLE Handle,
                                      IN BOOLEAN Locked)
{
    PHANDLE_TABLE_ENTRY InfoStructure;
    PHANDLE_TABLE_ENTRY TableEntry;
    PHANDLE_TABLE_ENTRY_INFO ResultInfo;
    OBJECT_HANDLE BaseHandle, ObHandle;

    /* Initialize local state */
    ResultInfo = NULLPTR;
    TableEntry = NULLPTR;

    /* Check if the handle table maintains extra information pages */
    if(HandleTable->ExtraInfoPages)
    {
        /* Check if the caller has not locked the entry */
        if(!Locked)
        {
            /* Acquire the lock */
            TableEntry = MapHandleToPointer(HandleTable, Handle);
            if(!TableEntry)
            {
                /* The handle is invalid or mapping failed, return NULL pointer */
                return NULLPTR;
            }
        }

        /* Isolate the base index of the handle table page */
        BaseHandle.GenericHandleOverlay = Handle;
        BaseHandle.Index &= ~(OBJECT_HANDLE_LOWLEVEL_COUNT - 1);

        /* Fetch the base information structure */
        InfoStructure = LookupHandleTableEntry(HandleTable, BaseHandle);

        /* Validate the retrieved structure */
        if(InfoStructure &&
           InfoStructure->NextFreeTableEntry == OBJECT_HANDLE_ADDITIONAL_INFO_SIGNATURE &&
           InfoStructure->InfoTable)
        {
            /* Decode the target index */
            ObHandle.GenericHandleOverlay = Handle;

            /* Resolve the information entry pointer */
            ResultInfo = &InfoStructure->InfoTable[ObHandle.Index % OBJECT_HANDLE_LOWLEVEL_COUNT];
        }
        else
        {
            /* Check if locked */
            if(TableEntry)
            {
                /* Release the lock */
                UnlockHandleTableEntry(HandleTable, TableEntry);
            }
        }
    }

    /* Return the information pointer */
    return ResultInfo;
}

/**
 * Initializes the handle table.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::HandleTable::InitializeHandleTable(VOID)
{
    /* Initialize the handle table list and its synchronization pushlock */
    RTL::LinkedList::InitializeListHead(&HandleTableListHead);
    KE::PushLock::InitializePushLock(&HandleTableListLock);
}

/**
 * Acquires the lock on a handle table entry, blocking the current thread if contention occurs.
 *
 * @param HandleTable
 *        Supplies a pointer to the handle table containing the entry.
 *
 * @param HandleTableEntry
 *        Supplies a pointer to the handle table entry to lock.
 *
 * @return This routine returns TRUE if the lock was successfully acquired, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
OB::HandleTable::LockHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                      IN PHANDLE_TABLE_ENTRY HandleTableEntry)
{
    LONG_PTR CurrentValue, NewValue, OldValue;
    KPUSH_LOCK_WAIT_BLOCK WaitBlock;

    /* Enter a retry loop */
    while(TRUE)
    {
        /* Get the state of the entry */
        OldValue = *(VOLATILE LONG_PTR *)&HandleTableEntry->Value;

        /* Check if the entry is currently unlocked */
        if(OldValue & OBJECT_HANDLE_TABLE_ENTRY_LOCK_BIT)
        {
            /* Clear the lock bit */
            NewValue = OldValue & ~OBJECT_HANDLE_TABLE_ENTRY_LOCK_BIT;

            /* Attempt to apply the lock */
            if(RTL::Atomic::CompareExchangePointer((PVOID *)&HandleTableEntry->Value,
                                                   (PVOID)NewValue, (PVOID)OldValue) == (PVOID)OldValue)
            {
                /* Lock acquired, return TRUE */
                return TRUE;
            }
        }
        else
        {
            /* Check if the entry is completely empty */
            if(!OldValue)
            {
                /* Cannot proceed, return FALSE */
                return FALSE;
            }
        }

        /* The entry is locked by another thread, block thread */
        KE::PushLock::BlockPushLock(&HandleTable->HandleContentionEvent, &WaitBlock);

        /* Check if the entry was freed or unlocked */
        CurrentValue = *(VOLATILE LONG_PTR *)&HandleTableEntry->Value;
        if(!CurrentValue || (CurrentValue & OBJECT_HANDLE_TABLE_ENTRY_LOCK_BIT))
        {
            /* The state changed, abort the wait */
            KE::PushLock::UnblockPushLock(&HandleTable->HandleContentionEvent, &WaitBlock);
        }
        else
        {
            /* The entry is locked, commit to the wait state */
            KE::Dispatcher::WaitForSingleObject(&WaitBlock.WakeEvent, Executive, KernelMode, FALSE, NULLPTR);
        }
    }
}

/**
 * Translates a generic handle into a pointer to its underlying handle table entry.
 *
 * @param HandleTable
 *        Supplies a pointer to the handle table performing the lookup.
 *
 * @param Handle
 *        Supplies the structured handle to be translated.
 *
 * @return This routine returns a pointer to the corresponding handle table entry, or NULL pointer if the handle
 *         is out of bounds or invalid.
 *
 * @since XT 1.0
 */
XTAPI
PHANDLE_TABLE_ENTRY
OB::HandleTable::LookupHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                        IN OBJECT_HANDLE Handle)
{
    PHANDLE_TABLE_ENTRY EntryTable;
    ULONG MaxHandle, TableLevel;
    ULONG_PTR Index, TableCode;
    PVOID *DirectoryTable;

    /* Strip the tag bits */
    Handle.Value &= ~(ULONG_PTR)(OBJECT_HANDLE_VALUE_INCREMENT - 1);

    /* Read the maximum handle value */
    MaxHandle = *(VOLATILE ULONG *)&HandleTable->NextHandleNeedingPool;
    if(Handle.Value >= MaxHandle)
    {
        /* Handle is out of bounds, return NULL pointer */
        return NULLPTR;
    }

    /* Convert the raw handle value to an array index */
    Index = Handle.Value / OBJECT_HANDLE_VALUE_INCREMENT;

    /* Read the table routing state */
    TableCode = *(VOLATILE ULONG_PTR *)&HandleTable->TableCode;

    /* Extract the table depth level */
    TableLevel = (ULONG)(TableCode & OBJECT_HANDLE_LEVEL_CODE_MASK);

    /* Validate the structural integrity of the table */
    if(TableLevel > 2)
    {
        /* Invalid table level, return NULL pointer */
        return NULLPTR;
    }

    /* Isolate the base address of the table */
    DirectoryTable = (PVOID *)(TableCode & ~(ULONG_PTR)OBJECT_HANDLE_LEVEL_CODE_MASK);

    /* Cascade through the directory levels */
    if(TableLevel == 2)
    {
        /* Resolve the highest level directory routing */
        DirectoryTable = (PVOID *)DirectoryTable[Index / (OBJECT_HANDLE_LOWLEVEL_COUNT * OBJECT_HANDLE_MEDIUMLEVEL_COUNT)];
    }
    if(TableLevel >= 1)
    {
        /* Resolve the intermediate directory routing */
        DirectoryTable = (PVOID *)DirectoryTable[(Index / OBJECT_HANDLE_LOWLEVEL_COUNT) % OBJECT_HANDLE_MEDIUMLEVEL_COUNT];
    }

    /* Resolve the final entry page base */
    EntryTable = (PHANDLE_TABLE_ENTRY)DirectoryTable;

    /* Return the memory location of the requested handle table entry */
    return &EntryTable[Index % OBJECT_HANDLE_LOWLEVEL_COUNT];
}

/**
 * Resolves a handle to its corresponding handle table entry and locks it.
 *
 * @param HandleTable
 *        Supplies a pointer to the handle table where the lookup will be performed.
 *
 * @param Handle
 *        Supplies the generic handle to map.
 *
 * @return This routine returns a pointer to the locked handle table entry on success, or NULL pointer on failure.
 *
 * @since XT 1.0
 */
XTAPI
PHANDLE_TABLE_ENTRY
OB::HandleTable::MapHandleToPointer(IN PHANDLE_TABLE HandleTable,
                                    IN HANDLE Handle)
{
    PHANDLE_TABLE_ENTRY TableEntry;
    OBJECT_HANDLE LocalHandle;

    /* Assume failure initially */
    TableEntry = NULLPTR;

    /* Overlay the generic handle */
    LocalHandle.GenericHandleOverlay = Handle;

    /* Validate the handle index */
    if(LocalHandle.Index & (OBJECT_HANDLE_LOWLEVEL_COUNT - 1))
    {
        /* Lookup the handle table entry */
        TableEntry = LookupHandleTableEntry(HandleTable, LocalHandle);
        if(TableEntry)
        {
            /* Acquire the entry lock */
            if(!LockHandleTableEntry(HandleTable, TableEntry))
            {
                /* Lock acquisition failed, reset the pointer */
                TableEntry = NULLPTR;
            }
        }
    }

    /* Return the table entry pointer */
    return TableEntry;
}

/**
 * Maps a handle to a locked pointer with mode-specific error handling.
 *
 * @param HandleTable
 *        Supplies the handle table to query.
 *
 * @param Handle
 *        Supplies the handle value to map.
 *
 * @param ProcessorMode
 *        Supplies the previous processor mode of the caller.
 *
 * @return This routine returns a pointer to the locked entry, or NULL pointer on failure.
 *
 * @since XT 1.0
 */
XTAPI
PHANDLE_TABLE_ENTRY
OB::HandleTable::MapHandleToPointer(IN PHANDLE_TABLE HandleTable,
                                    IN HANDLE Handle,
                                    IN KPROCESSOR_MODE ProcessorMode)
{
    PHANDLE_TABLE_ENTRY TableEntry;
    OBJECT_HANDLE LocalHandle;

    /* Assume failure initially */
    TableEntry = NULLPTR;

    /* Overlay the generic handle */
    LocalHandle.GenericHandleOverlay = Handle;

    /* Validate the handle index */
    if(!(LocalHandle.Index & (OBJECT_HANDLE_LOWLEVEL_COUNT - 1)))
    {
        /* Invalid handle index alignment, return a NULL pointer */
        return NULLPTR;
    }

    /* Retrieve the handle table entry corresponding to the specified handle value */
    TableEntry = LookupHandleTableEntry(HandleTable, LocalHandle);
    if(!TableEntry)
    {
        /* Entry lookup failed, return a NULL pointer */
        return NULLPTR;
    }

    /* Acquire the entry lock */
    if(!LockHandleTableEntry(HandleTable, TableEntry))
    {
        /* Lock acquisition failed, reset the pointer */
        TableEntry = NULLPTR;
    }

    /* Return a pointer to the handle table entry */
    return TableEntry;
}

/**
 * Moves free handles from the alternate free list to the primary free list.
 *
 * @param HandleTable
 *        Supplies the handle table to process.
 *
 * @return This routine returns the old value of the last free handle.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
OB::HandleTable::MoveFreeHandles(IN PHANDLE_TABLE HandleTable)
{
    ULONG CurrentFirstFree, FreeSize, Index, LockId, NewFirstFreeIndex, NewIndex, OldIndex, OldValue;
    PHANDLE_TABLE_ENTRY Entry, FirstEntry;
    BOOLEAN StrictFIFO;
    OBJECT_HANDLE Handle;

    /* Extract the current free list and zero it out */
    OldValue = RTL::Atomic::Exchange32((PLONG)&HandleTable->LastFreeHandle, 0);

    /* Assign the extracted old value and check if there are any free handles */
    Index = OldValue;
    if(!Index)
    {
        /* No free handles available, return old value */
        return OldValue;
    }

    /* Acquire all secondary handle table locks for structural integrity */
    for(LockId = 1; LockId < OBJECT_HANDLE_VALUE_INCREMENT; LockId++)
    {
        /* Block on the designated segment lock */
        KE::PushLock::AcquireExclusivePushLock(&HandleTable->HandleTableLock[LockId]);
    }

    /* Capture the FIFO requirement flag */
    StrictFIFO = HandleTable->StrictFIFO;

    /* Check if the handle table operates without strict FIFO ordering */
    if(!StrictFIFO)
    {
        /* Append the sequence */
        if(!RTL::Atomic::CompareExchange32((PLONG)&HandleTable->FirstFreeHandle, OldValue, 0))
        {
            /* Release all secondary segment locks */
            for(LockId = 1; LockId < OBJECT_HANDLE_VALUE_INCREMENT; LockId++)
            {
                /* Unblock the designated segment lock */
                KE::PushLock::ReleaseExclusivePushLock(&HandleTable->HandleTableLock[LockId]);
            }

            /* Return the old value */
            return OldValue;
        }
    }

    /* Initialize tracking variables */
    FirstEntry = NULLPTR;
    FreeSize = 0;
    OldIndex = 0;

    /* Traverse the alternate free list chain */
    while(Index)
    {
        /* Increment the free handle counter */
        FreeSize++;

        /* Map the raw index to its handle table entry */
        Handle.Value = Index;
        Entry = LookupHandleTableEntry(HandleTable, Handle);

        /* Link the current entry to point backwards */
        NewIndex = Entry->NextFreeTableEntry;
        Entry->NextFreeTableEntry = OldIndex;

        /* Check if the old index is zero */
        if(!OldIndex)
        {
            /* Store the pointer to the newly formed chain tail */
            FirstEntry = Entry;
        }

        /* Advance the traversal pointers */
        OldIndex = Index;
        Index = NewIndex;
    }

    /* Calculate the target index */
    NewFirstFreeIndex = OldIndex;

    /* Loop until the exchange succeeds */
    while(TRUE)
    {
        /* Capture the state of the primary free list */
        CurrentFirstFree = *(VOLATILE ULONG *)&HandleTable->FirstFreeHandle;

        /* Chain tail to the current head of the primary list */
        FirstEntry->NextFreeTableEntry = CurrentFirstFree;

        /* Attempt to swap the new chain head */
        if(CurrentFirstFree == (ULONG)RTL::Atomic::CompareExchange32((PLONG)&HandleTable->FirstFreeHandle,
                                                                     NewFirstFreeIndex,
                                                                     CurrentFirstFree))
        {
            /* The exchange succeeded, exit the loop */
            break;
        }
    }

    /* Release all handle table locks */
    for(LockId = 1; LockId < OBJECT_HANDLE_VALUE_INCREMENT; LockId++)
    {
        /* Unblock the designated segment lock */
        KE::PushLock::ReleaseExclusivePushLock(&HandleTable->HandleTableLock[LockId]);
    }

    /* Check if the batch size is below the threshold */
    if(FreeSize < 100 && StrictFIFO)
    {
        /* Reset the old value to zero */
        OldValue = 0;
    }

    /* Return the original first handle value */
    return OldValue;
}

/**
 * Removes a handle table from the global list.
 *
 * @param HandleTable
 *        Supplies the handle table to remove.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::HandleTable::RemoveHandleTable(IN PHANDLE_TABLE HandleTable)
{
    PKTHREAD CurrentThread;

    /* Get the current thread */
    CurrentThread = KE::Processor::GetCurrentThread();

    /* Enter a critical region and acquire the handle table lock */
    KE::CriticalRegionGuard CriticalRegion(CurrentThread);
    KE::PushLockExclusiveGuard PushLock(&HandleTableListLock);

    /* Remove the handle table from the global list and initialize the list head */
    RTL::LinkedList::RemoveEntryList(&HandleTable->HandleTableList);
    RTL::LinkedList::InitializeListHead(&HandleTable->HandleTableList);
}

/**
 * Sets extended information for a handle table entry.
 *
 * @param HandleTable
 *        Supplies a pointer to the handle table containing the entry.
 *
 * @param Handle
 *        Supplies the handle for which the information is being updated.
 *
 * @param EntryInfo
 *        Supplies a pointer to the entry information structure to be copied.
 *
 * @param Locked
 *        Supplies a boolean value indicating whether the handle table is already locked.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::HandleTable::SetHandleInformation(IN PHANDLE_TABLE HandleTable,
                                      IN HANDLE Handle,
                                      IN PHANDLE_TABLE_ENTRY_INFO EntryInfo,
                                      IN BOOLEAN Locked)
{
    PHANDLE_TABLE_ENTRY InfoStructure, TableEntry;
    PHANDLE_TABLE_ENTRY_INFO InfoTable;
    OBJECT_HANDLE ExHandle;

    /* Initialize local state tracking */
    ExHandle.GenericHandleOverlay = Handle;
    ExHandle.Index &= ~(OBJECT_HANDLE_LOWLEVEL_COUNT - 1);
    TableEntry = NULLPTR;

    /* Acquire a critical region guard */
    KE::CriticalRegionGuard CriticalRegion(NULLPTR, !Locked);

    /* Check if the caller specified that the handle table is unlocked */
    if(!Locked)
    {
        /* Map the supplied handle to its corresponding handle table entry */
        TableEntry = MapHandleToPointer(HandleTable, Handle);
        if(!TableEntry)
        {
            /* Handle mapping failed, return error code */
            return STATUS_UNSUCCESSFUL;
        }
    }

    /* Retrieve the base info structure */
    InfoStructure = LookupHandleTableEntry(HandleTable, ExHandle);
    if(!InfoStructure || InfoStructure->NextFreeTableEntry != OBJECT_HANDLE_ADDITIONAL_INFO_SIGNATURE)
    {
        /* Check if the handle table entry was mapped */
        if(TableEntry)
        {
            /* Unlock the mapped handle table entry */
            UnlockHandleTableEntry(HandleTable, TableEntry);
        }

        /* Invalid info structure signature, return error code */
        return STATUS_UNSUCCESSFUL;
    }

    /* Retrieve the pointer to the handle table entry info table */
    InfoTable = InfoStructure->InfoTable;
    if(!InfoTable)
    {
        /* Allocate memory for the table */
        InfoTable = (PHANDLE_TABLE_ENTRY_INFO)AllocateTableMemory(HandleTable->QuotaProcess,
                                                                  OBJECT_HANDLE_LOWLEVEL_COUNT * sizeof(HANDLE_TABLE_ENTRY_INFO),
                                                                  FALSE);
        if(InfoTable)
        {
            /* Attempt to publish the info table pointer */
            if(!RTL::Atomic::CompareExchangePointer((PVOID *)&InfoStructure->InfoTable, InfoTable, NULLPTR))
            {
                /* Increment the extra info pages counter */
                RTL::Atomic::Increment32(&HandleTable->ExtraInfoPages);
            }
            else
            {
                /* Free the redundant info table */
                FreeTableMemory(HandleTable->QuotaProcess, InfoTable,
                                OBJECT_HANDLE_LOWLEVEL_COUNT * sizeof(HANDLE_TABLE_ENTRY_INFO));

                /* Retrieve the info table pointer */
                InfoTable = InfoStructure->InfoTable;
            }
        }
    }

    /* Verify that a valid info table pointer is available */
    if(!InfoTable)
    {
        /* Check if the handle table entry was mapped */
        if(TableEntry)
        {
            /* Unlock the mapped handle table entry */
            UnlockHandleTableEntry(HandleTable, TableEntry);
        }

        /* Info table allocation failed, return error code */
        return STATUS_UNSUCCESSFUL;
    }

    /* Restore the original handle */
    ExHandle.GenericHandleOverlay = Handle;
    InfoTable[ExHandle.Index % OBJECT_HANDLE_LOWLEVEL_COUNT] = *EntryInfo;

    /* Check if the handle table entry is still locked */
    if(TableEntry)
    {
        /* Unlock the handle table entry */
        UnlockHandleTableEntry(HandleTable, TableEntry);
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Captures a snapshot of all handle tables in the system.
 *
 * @param SnapshotHandleEntry
 *        Supplies the callback invoked for each handle entry.
 *
 * @param HandleInformation
 *        Supplies the buffer to fill with handle information.
 *
 * @param Length
 *        Supplies the size of the buffer in bytes.
 *
 * @param RequiredLength
 *        Receives the required buffer size.
 *
 * @return This routine returns the status of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::HandleTable::SnapshotHandleTables(IN POB_SNAPSHOT_HANDLE_ENTRY SnapshotHandleEntry,
                                      IN OUT PSYSTEM_HANDLE_INFORMATION HandleInformation,
                                      IN ULONG Length,
                                      IN OUT PULONG RequiredLength)
{
    PSYSTEM_HANDLE_TABLE_ENTRY_INFO HandleEntryInfo;
    PHANDLE_TABLE_ENTRY HandleTableEntry;
    PHANDLE_TABLE HandleTable;
    PKTHREAD CurrentThread;
    PLIST_ENTRY NextEntry;
    OBJECT_HANDLE Handle;
    XTSTATUS Status;

    /* Retrieve the currently executing thread */
    CurrentThread = KE::Processor::GetCurrentThread();

    /* Initialize the handle entry information */
    HandleEntryInfo = &HandleInformation->Handles[0];
    HandleInformation->NumberOfHandles = 0;

    /* Acquire a critical region guard */
    KE::CriticalRegionGuard CriticalRegion(CurrentThread);

    /* Acquire the global handle table list lock */
    KE::PushLockSharedGuard PushLock(&HandleTableListLock);

    /* Traverse the global list of handle tables */
    for(NextEntry = HandleTableListHead.Flink; NextEntry != &HandleTableListHead; NextEntry = NextEntry->Flink)
    {
        /* Resolve the handle table record */
        HandleTable = CONTAIN_RECORD(NextEntry, HANDLE_TABLE, HandleTableList);

        /* Initialize the handle value */
        Handle.Value = 0;

        /* Iterate through all handles within the current table */
        while (TRUE)
        {
            /* Retrieve the underlying handle table entry */
            HandleTableEntry = LookupHandleTableEntry(HandleTable, Handle);
            if(!HandleTableEntry)
            {
                /* Handle is out of bounds, break the loop */
                break;
            }

            /* Validate that the entry actually points to a registered object */
            if(ValidateObjectEntry(HandleTableEntry))
            {
                /* Increment the global handle count */
                HandleInformation->NumberOfHandles += 1;

                /* Acquire the lock */
                if(LockHandleTableEntry(HandleTable, HandleTableEntry))
                {
                    /* Invoke the snapshot callback */
                    Status = (*SnapshotHandleEntry)(&HandleEntryInfo, HandleTable->UniqueProcessId, HandleTableEntry,
                                                    Handle.GenericHandleOverlay, Length, RequiredLength);

                    /* Release the lock */
                    UnlockHandleTableEntry(HandleTable, HandleTableEntry);

                    /* Check the callback status code */
                    if(Status != STATUS_SUCCESS)
                    {
                        /* Callback routine failed, return status code */
                        return Status;
                    }
                }
            }

            /* Advance to the next handle value */
            Handle.Value += OBJECT_HANDLE_VALUE_INCREMENT;
        }
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Captures an extended snapshot of all handle tables in the system.
 *
 * @param SnapshotHandleEntry
 *        Supplies the callback invoked for each handle entry.
 *
 * @param HandleInformation
 *        Supplies the buffer to fill with extended handle information.
 *
 * @param Length
 *        Supplies the size of the buffer in bytes.
 *
 * @param RequiredLength
 *        Receives the required buffer size.
 *
 * @return This routine returns the status of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::HandleTable::SnapshotHandleTables(IN POB_SNAPSHOT_HANDLE_ENTRY_EX SnapshotHandleEntry,
                                      IN OUT PSYSTEM_HANDLE_INFORMATION_EX HandleInformation,
                                      IN ULONG Length,
                                      IN OUT PULONG RequiredLength)
{
    PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX HandleEntryInfo;
    PHANDLE_TABLE_ENTRY HandleTableEntry;
    PHANDLE_TABLE HandleTable;
    PKTHREAD CurrentThread;
    PLIST_ENTRY NextEntry;
    OBJECT_HANDLE Handle;
    XTSTATUS Status;

    /* Retrieve the currently executing thread */
    CurrentThread = KE::Processor::GetCurrentThread();

    /* Initialize the extended handle entry information */
    HandleEntryInfo = &HandleInformation->Handles[0];
    HandleInformation->NumberOfHandles = 0;

    /* Acquire a critical region guard */
    KE::CriticalRegionGuard CriticalRegion(CurrentThread);

    /* Acquire the global handle table list lock */
    KE::PushLockSharedGuard PushLock(&HandleTableListLock);

    /* Traverse the global list of handle tables */
    for(NextEntry = HandleTableListHead.Flink; NextEntry != &HandleTableListHead; NextEntry = NextEntry->Flink)
    {
        /* Resolve the handle table record */
        HandleTable = CONTAIN_RECORD(NextEntry, HANDLE_TABLE, HandleTableList);

        /* Initialize the handle value */
        Handle.Value = 0;

        /* Iterate through all handles within the current table */
        while (TRUE)
        {
            /* Retrieve the underlying handle table entry */
            HandleTableEntry = LookupHandleTableEntry(HandleTable, Handle);
            if(!HandleTableEntry)
            {
                /* Handle is out of bounds, break the loop */
                break;
            }

            /* Validate that the entry actually points to a registered object */
            if(ValidateObjectEntry(HandleTableEntry))
            {
                /* Increment the global extended handle count */
                HandleInformation->NumberOfHandles += 1;

                /* Acquire the lock */
                if(LockHandleTableEntry(HandleTable, HandleTableEntry))
                {
                    /* Invoke the extended snapshot callback */
                    Status = (*SnapshotHandleEntry)(&HandleEntryInfo, HandleTable->UniqueProcessId, HandleTableEntry,
                                                    Handle.GenericHandleOverlay, Length, RequiredLength);

                    /* Release the lock */
                    UnlockHandleTableEntry(HandleTable, HandleTableEntry);

                    /* Check the callback status code */
                    if(Status != STATUS_SUCCESS)
                    {
                        /* Callback routine failed, return status code */
                        return Status;
                    }
                }
            }

            /* Advance to the next handle value */
            Handle.Value += OBJECT_HANDLE_VALUE_INCREMENT;
        }
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Sweeps a handle table without synchronization.
 *
 * @param HandleTable
 *        Supplies the handle table to sweep.
 *
 * @param Callback
 *        Supplies the callback invoked for each valid handle.
 *
 * @param Parameter
 *        Supplies an opaque parameter passed to the callback.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::HandleTable::SweepHandleTable(IN PHANDLE_TABLE HandleTable,
                                  IN POB_ENUMERATE_HANDLE_ROUTINE Callback,
                                  IN PVOID Parameter)
{
    PHANDLE_TABLE_ENTRY HandleTableEntry;
    ULONG Index, RemainingEntries;
    OBJECT_HANDLE Handle;

    /* Initialize the handle value */
    Handle.Value = OBJECT_HANDLE_VALUE_INCREMENT;

    /* Traverse the handle table */
    while(TRUE)
    {
        /* Resolve the current handle value */
        HandleTableEntry = LookupHandleTableEntry(HandleTable, Handle);
        if(!HandleTableEntry)
        {
            /* Handle entry lookup fails, break loop */
            break;
        }

        /* Calculate the number of remaining entries */
        RemainingEntries = OBJECT_HANDLE_LOWLEVEL_COUNT -
                           ((Handle.Value / OBJECT_HANDLE_VALUE_INCREMENT) % OBJECT_HANDLE_LOWLEVEL_COUNT);

        /* Iterate through the remaining handle entries */
        for(Index = 0; Index < RemainingEntries; Index++)
        {
            /* Lock the current handle table entry */
            if(LockHandleTableEntry(HandleTable, HandleTableEntry))
            {
                /* Execute the specified callback routine */
                (*Callback)(HandleTableEntry, Handle.GenericHandleOverlay, Parameter);
            }

            /* Increment the handle value */
            Handle.Value += OBJECT_HANDLE_VALUE_INCREMENT;

            /* Advance to the next handle slot */
            HandleTableEntry++;
        }
    }
}

/**
 * Unlocks a previously locked handle table entry and wakes up any threads waiting on the table's contention event.
 *
 * @param HandleTable
 *        Supplies a pointer to the handle table containing the entry.
 *
 * @param HandleTableEntry
 *        Supplies a pointer to the handle table entry to be unlocked.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::HandleTable::UnlockHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                        IN PHANDLE_TABLE_ENTRY HandleTableEntry)
{
    /* Unlock entry and signal the handle contention event */
    RTL::Atomic::Or64((VOLATILE PLONG_PTR)&HandleTableEntry->Value, (LONG_PTR)OBJECT_HANDLE_TABLE_ENTRY_LOCK_BIT);
    KE::PushLock::UnblockPushLock(&HandleTable->HandleContentionEvent, NULLPTR);
}

/**
 * Checks if a handle table entry is valid and points to an actual object.
 *
 * @param Entry
 *        Supplies a pointer to the handle table entry to validate.
 *
 * @return This routine returns TRUE if the entry is valid, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTINLINE
BOOLEAN
OB::HandleTable::ValidateObjectEntry(IN PHANDLE_TABLE_ENTRY Entry)
{
    /* Return TRUE if the entry and its underlying object pointer exist */
    return (Entry && Entry->Object && Entry->NextFreeTableEntry != OBJECT_HANDLE_ADDITIONAL_INFO_SIGNATURE);
}
