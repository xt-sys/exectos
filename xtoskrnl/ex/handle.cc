/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ex/handle.cc
 * DESCRIPTION:     Handle Table management interface
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


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
XTFASTCALL
PHANDLE_TABLE_ENTRY_INFO
EX::Handle::GetHandleInformation(IN PHANDLE_TABLE HandleTable,
                                 IN HANDLE Handle,
                                 IN BOOLEAN Locked)
{
    PHANDLE_TABLE_ENTRY InfoStructure, TableEntry;
    PHANDLE_TABLE_ENTRY_INFO ResultInfo;
    EXHANDLE BaseHandle, ExHandle;

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
        BaseHandle.Index &= ~(HANDLE_LOWLEVEL_COUNT - 1);

        /* Fetch the base information structure */
        InfoStructure = LookupHandleTableEntry(HandleTable, BaseHandle);

        /* Validate the retrieved structure */
        if(InfoStructure &&
           InfoStructure->NextFreeTableEntry == HANDLE_ADDITIONAL_INFO_SIGNATURE &&
           InfoStructure->InfoTable)
        {
            /* Decode the target index */
            ExHandle.GenericHandleOverlay = Handle;

            /* Resolve the information entry pointer */
            ResultInfo = &InfoStructure->InfoTable[ExHandle.Index % HANDLE_LOWLEVEL_COUNT];
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
EX::Handle::LockHandleTableEntry(IN PHANDLE_TABLE HandleTable,
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
        if(OldValue & EXHANDLE_TABLE_ENTRY_LOCK_BIT)
        {
            /* Clear the lock bit */
            NewValue = OldValue & ~EXHANDLE_TABLE_ENTRY_LOCK_BIT;

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
        if(!CurrentValue || (CurrentValue & EXHANDLE_TABLE_ENTRY_LOCK_BIT))
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
EX::Handle::LookupHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                   IN EXHANDLE Handle)
{
    PHANDLE_TABLE_ENTRY EntryTable;
    ULONG MaxHandle, TableLevel;
    ULONG_PTR Index, TableCode;
    PVOID *DirectoryTable;

    /* Strip the tag bits */
    Handle.Value &= ~(ULONG_PTR)(HANDLE_VALUE_INCREMENT - 1);

    /* Read the maximum handle value */
    MaxHandle = *(VOLATILE ULONG *)&HandleTable->NextHandleNeedingPool;
    if(Handle.Value >= MaxHandle)
    {
        /* Handle is out of bounds, return NULL pointer */
        return NULLPTR;
    }

    /* Convert the raw handle value to an array index */
    Index = Handle.Value / HANDLE_VALUE_INCREMENT;

    /* Read the table routing state */
    TableCode = *(VOLATILE ULONG_PTR *)&HandleTable->TableCode;

    /* Extract the table depth level */
    TableLevel = (ULONG)(TableCode & HANDLE_LEVEL_CODE_MASK);

    /* Validate the structural integrity of the table */
    if(TableLevel > 2)
    {
        /* Invalid table level, return NULL pointer */
        return NULLPTR;
    }

    /* Isolate the base address of the table */
    DirectoryTable = (PVOID *)(TableCode & ~(ULONG_PTR)HANDLE_LEVEL_CODE_MASK);

    /* Cascade through the directory levels */
    if(TableLevel == 2)
    {
        /* Resolve the highest level directory routing */
        DirectoryTable = (PVOID *)DirectoryTable[Index / (HANDLE_LOWLEVEL_COUNT * HANDLE_MIDLEVEL_COUNT)];
    }
    if(TableLevel >= 1)
    {
        /* Resolve the intermediate directory routing */
        DirectoryTable = (PVOID *)DirectoryTable[(Index / HANDLE_LOWLEVEL_COUNT) % HANDLE_MIDLEVEL_COUNT];
    }

    /* Resolve the final entry page base */
    EntryTable = (PHANDLE_TABLE_ENTRY)DirectoryTable;

    /* Return the memory location of the requested handle table entry */
    return &EntryTable[Index % HANDLE_LOWLEVEL_COUNT];
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
EX::Handle::MapHandleToPointer(IN PHANDLE_TABLE HandleTable,
                               IN HANDLE Handle)
{
    PHANDLE_TABLE_ENTRY TableEntry;
    EXHANDLE LocalHandle;

    /* Assume failure initially */
    TableEntry = NULLPTR;

    /* Overlay the generic handle */
    LocalHandle.GenericHandleOverlay = Handle;

    /* Validate the handle index */
    if(LocalHandle.Index & (HANDLE_LOWLEVEL_COUNT - 1))
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
EX::Handle::UnlockHandleTableEntry(IN PHANDLE_TABLE HandleTable,
                                   IN PHANDLE_TABLE_ENTRY HandleTableEntry)
{
    /* Unlock entry and signal the handle contention event */
    RTL::Atomic::Or64((VOLATILE PLONG_PTR)&HandleTableEntry->Value, (LONG_PTR)EXHANDLE_TABLE_ENTRY_LOCK_BIT);
    KE::PushLock::UnblockPushLock(&HandleTable->HandleContentionEvent, NULLPTR);
}
