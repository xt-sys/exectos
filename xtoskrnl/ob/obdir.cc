/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ob/obdir.cc
 * DESCRIPTION:     Object Manager Directory
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Acquires an exclusive pushlock for the specified object directory.
 *
 * @param Directory
 *        Supplies a pointer to the object directory being locked.
 *
 * @param LookupContext
 *        Supplies a pointer to the lookup context used to track the lock state.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::Directory::AcquireExclusiveDirectoryLock(IN OUT POBJECT_DIRECTORY Directory,
                                             IN OUT POBJECT_LOOKUP_CONTEXT LookupContext)
{
    /* Set the initial lock state */
    LookupContext->LockStateSignature = OBJECT_LOCK_STATE_WAIT_EXCLUSIVE;

    /* Disable kernel APCs */
    KE::KThread::EnterCriticalRegion();

    /* Acquire the directory lock for exclusive access */
    KE::PushLock::AcquireExclusivePushLock(&Directory->Lock);

    /* Update the lock state */
    LookupContext->LockStateSignature = OBJECT_LOCK_STATE_OWNED_EXCLUSIVE;
}

/**
 * Acquires a shared pushlock for the specified object directory.
 *
 * @param Directory
 *        Supplies a pointer to the object directory being locked.
 *
 * @param LookupContext
 *        Supplies a pointer to the lookup context used to track the lock state.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::Directory::AcquireSharedDirectoryLock(IN POBJECT_DIRECTORY Directory,
                                          IN POBJECT_LOOKUP_CONTEXT LookupContext)
{
    /* Set the initial lock state */
    LookupContext->LockStateSignature = OBJECT_LOCK_WAITSHARED_SIGNATURE;

    /* Disable kernel APCs */
    KE::KThread::EnterCriticalRegion();

    /* Acquire the directory lock for shared access */
    KE::PushLock::AcquireSharedPushLock(&Directory->Lock);

    /* Update the lock state */
    LookupContext->LockStateSignature = OBJECT_LOCK_OWNEDSHARED_SIGNATURE;
}

/**
 * Computes the case-insensitive hash index for an object name.
 *
 * @param Name
 *        Supplies a pointer to the unicode string containing the object name.
 *
 * @return This routine returns the computed hash index.
 *
 * @since XT 1.0
 */
XTFASTCALL
ULONG
OB::Directory::ComputeObjectNameHash(IN PCUNICODE_STRING Name)
{
    ULONG CharacterLength, HashIndex;
    WCHAR Character;
    PWCH Buffer;

    /* Initialize the buffer pointer, character count, and starting hash value */
    Buffer = Name->Buffer;
    CharacterLength = Name->Length / sizeof(WCHAR);
    HashIndex = 0;

    /* Iterate through each wide character */
    while(CharacterLength--)
    {
        /* Fetch the current character and advance the buffer pointer */
        Character = *Buffer++;

        /* Apply the NT string hashing polynomial */
        HashIndex += (HashIndex << 1) + (HashIndex >> 1);

        /* Check if the character is uppercase, numeric, or a symbol */
        if(Character < L'a')
        {
            /* Add the unmodified character */
            HashIndex += Character;
        }
        else if(Character > L'z')
        {
            /* Convert extended Unicode characters to uppercase */
            HashIndex += RTL::Nls::ToUpperUnicodeCharacter(Character);
        }
        else
        {
            /* Perform ASCII conversion from lowercase to uppercase */
            HashIndex += (Character - (L'a' - L'A'));
        }
    }

    /* Return the final hash value */
    return HashIndex % OBJECT_NUMBER_HASH_BUCKETS;
}

/**
 * Removes an object directory entry from its hash bucket and frees its memory.
 *
 * @param LookupContext
 *        Supplies a pointer to the lookup context containing the directory and hash index.
 *
 * @return This routine returns TRUE if the entry was successfully unlinked and freed, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
OB::Directory::DeleteDirectoryEntry(IN POBJECT_LOOKUP_CONTEXT LookupContext)
{
    POBJECT_DIRECTORY_ENTRY DirectoryEntry, *HashBucket;
    POBJECT_DIRECTORY Directory;

    /* Extract the directory from the lookup context */
    Directory = LookupContext->Directory;

    /* Verify that the directory pointer is valid */
    if(Directory)
    {
        /* Resolve the head of the collision chain */
        HashBucket = (POBJECT_DIRECTORY_ENTRY*)&Directory->HashBuckets[LookupContext->HashIndex];

        /* Retrieve the entry at the head of the chain */
        DirectoryEntry = *HashBucket;

        /* Ensure there isan entry to delete */
        if(DirectoryEntry)
        {
            /* Unlink the entry from the hash bucket chain */
            *HashBucket = DirectoryEntry->ChainLink;
            DirectoryEntry->ChainLink = NULLPTR;

            /* Free the memory */
            MM::Allocator::FreePool(DirectoryEntry);

            /* Return TRUE */
            return TRUE;
        }
    }

    /* No entry found, return FALSE */
    return FALSE;
}

/**
 * Initializes the Object Manager lookup context.
 *
 * @param LookupContext
 *        Supplies a pointer to the caller-allocated lookup context to initialize.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::Directory::InitializeLookupContext(OUT POBJECT_LOOKUP_CONTEXT LookupContext)
{
    /* Initialize the lookup context */
    LookupContext->Directory = NULLPTR;
    LookupContext->DirectoryLocked = FALSE;
    LookupContext->LockStateSignature = OBJECT_LOCK_STATE_INITIALIZED;
    LookupContext->Object = NULLPTR;
}

/**
 * Inserts a newly created object into an object directory.
 *
 * @param Directory
 *        Pointer to the object directory where the object will be inserted.
 *
 * @param LookupContext
 *        Pointer to the lookup context containing the precomputed hash index.
 *
 * @param ObjectHeader
 *        Pointer to the object header of the object being inserted.
 *
 * @return TRUE if the insertion was successful, FALSE if pool allocation failed.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
OB::Directory::InsertDirectoryEntry(IN OUT POBJECT_DIRECTORY Directory,
                                    IN POBJECT_LOOKUP_CONTEXT LookupContext,
                                    IN OUT POBJECT_HEADER ObjectHeader)
{
    POBJECT_DIRECTORY_ENTRY *HashBucket, NewEntry;
    POBJECT_HEADER_NAME_INFO NameInfo;
    XTSTATUS Status;

    /* Extract the name information header */
    NameInfo = OB::LifeCycle::GetObjectNameInformation(ObjectHeader);

    /* Allocate the directory entry */
    Status = MM::Allocator::AllocatePool(PagedPool, sizeof(OBJECT_DIRECTORY_ENTRY),
                                         (PVOID *)&NewEntry, TAG_OB_DIRECTORY);
    if(Status != STATUS_SUCCESS || !NewEntry)
    {
        /* Allocation failed, return FALSE */
        return FALSE;
    }

    /* Resolve the hash bucket head */
    HashBucket = (POBJECT_DIRECTORY_ENTRY*)&Directory->HashBuckets[LookupContext->HashIndex];

    /* Insert the entry at the head of the collision chain */
    NewEntry->ChainLink = *HashBucket;
    *HashBucket = NewEntry;

    /* Link the entry to the object body and establish the parent directory */
    NewEntry->Object = &ObjectHeader->Body;
    NameInfo->Directory = Directory;

    /* Return TRUE */
    return TRUE;
}

/**
 * Acquires an exclusive lock on the specified object directory.
 *
 * @param Directory
 *        Supplies a pointer to the object directory to lock.
 *
 * @param LookupContext
 *        Supplies a pointer to the active lookup context used to track the lock state.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::Directory::LockLookupContext(IN POBJECT_DIRECTORY Directory,
                                 IN POBJECT_LOOKUP_CONTEXT LookupContext)
{
    /* Acquire exclusive access to the directory */
    AcquireExclusiveDirectoryLock(Directory, LookupContext);

    /* Cache the directory pointer and lock ownership */
    LookupContext->Directory = Directory;
    LookupContext->DirectoryLocked = TRUE;
}

/**
 * Looks up an object directory entry by name within the namespace.
 *
 * @param Directory
 *        Supplies a pointer to the primary object directory to search.
 *
 * @param Name
 *        Supplies a pointer to the name of the object to locate.
 *
 * @param Attributes
 *        Supplies the object attributes bitmask.
 *
 * @param SearchGlobalDirectory
 *        Specifies whether to fallback into the device map's global shadow directory if the object is not found
 *        in the primary directory.
 *
 * @param LookupContext
 *        Supplies a pointer to the tracking context.
 *
 * @return This routine returns a pointer to the referenced object body, or NULLPTR if resolution fails.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
OB::Directory::LookupDirectoryEntry(IN POBJECT_DIRECTORY Directory,
                                    IN PUNICODE_STRING Name,
                                    IN ULONG Attributes,
                                    IN BOOLEAN SearchGlobalDirectory,
                                    OUT POBJECT_LOOKUP_CONTEXT LookupContext)
{
    POBJECT_HEADER_NAME_INFO PreviousNameInfo;
    POBJECT_DIRECTORY CurrentDirectory;
    BOOLEAN CaseInsensitive;
    PVOID FoundObject;
    ULONG HashIndex;

    /* Initialize the default return value */
    FoundObject = NULLPTR;

    /* Check if global device map traversal is available */
    if(!OB::DeviceMap::GetUniqueDeviceMaps())
    {
        /* Override caller's request to prevent shadow directory traversal */
        SearchGlobalDirectory = FALSE;
    }

    /* Guard against invalid parameter combination */
    if(Directory != NULLPTR && Name != NULLPTR && Name->Length != 0 && Name->Buffer != NULLPTR)
    {
        /* Determine if the string comparison should ignore case */
        CaseInsensitive = (Attributes & OBJECT_CASE_INSENSITIVE) ? TRUE : FALSE;

        /* Precompute the hash index */
        HashIndex = ComputeObjectNameHash(Name);

        /* Initialize the traversal pointer */
        CurrentDirectory = Directory;

        /* Cache the computed hash index in the lookup context */
        LookupContext->HashIndex = (USHORT)HashIndex;

        /* Iterate through the directory and linked shadow boundaries */
        while(CurrentDirectory)
        {
            /* Check if the directory is locked */
            if(!LookupContext->DirectoryLocked)
            {
                /* Acquire the directory lock */
                AcquireSharedDirectoryLock(CurrentDirectory, LookupContext);
            }

            /* Traverse the hash bucket chain to find a matching entry */
            FoundObject = SearchDirectory(CurrentDirectory, Name, CaseInsensitive, HashIndex, LookupContext);
            if(FoundObject)
            {
                /* Match found, secure the object and its name info */
                OB::LifeCycle::ReferenceObjectNameInformation(CONTAIN_RECORD(FoundObject, OBJECT_HEADER, Body));
                OB::LifeCycle::ReferenceObject(FoundObject);

                /* Check if the directory is locked */
                if(!LookupContext->DirectoryLocked)
                {
                    /* Drop the lock and restore standard kernel APC delivery */
                    OB::Directory::ReleaseDirectoryLock(CurrentDirectory, LookupContext);
                }

                /* Target resolved, break the loop */
                break;
            }

            /* Check if the directory is locked */
            if(!LookupContext->DirectoryLocked)
            {
                /* Drop the lock and restore standard kernel APC delivery */
                OB::Directory::ReleaseDirectoryLock(CurrentDirectory, LookupContext);
            }

            /* Check if fallback to the global shadow directory is possible */
            if(SearchGlobalDirectory && CurrentDirectory->DeviceMap != NULLPTR)
            {
                /* Resolve the shadow global devices directory in next iteration */
                CurrentDirectory = OB::DeviceMap::GetGlobalDevicesDirectory(CurrentDirectory);
            }
            else
            {
                /* Terminate the traversal loop */
                CurrentDirectory = NULLPTR;
            }
        }
    }

    /* Check if the lookup context contains a previously resolved object */
    if(LookupContext->Object != NULLPTR)
    {
        /* Resolve the old object's name information header */
        PreviousNameInfo = OB::LifeCycle::GetObjectNameInformation(CONTAIN_RECORD(LookupContext->Object,
                                                                                  OBJECT_HEADER,
                                                                                  Body));

        /* Release the references */
        OB::LifeCycle::DereferenceObjectNameInformation(PreviousNameInfo);
        OB::LifeCycle::DereferenceObject(LookupContext->Object);
    }

    /* Store the newly found object payload inside the caller's lookup context */
    LookupContext->Object = FoundObject;

    /* Return the object body */
    return FoundObject;
}

/**
 * Releases a previously acquired object directory lock.
 *
 * @param Directory
 *        Supplies a pointer to the object directory to unlock.
 *
 * @param LookupContext
 *        Supplies a pointer to the lookup context for state tracking.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::Directory::ReleaseDirectoryLock(IN OUT POBJECT_DIRECTORY Directory,
                                    IN OUT POBJECT_LOOKUP_CONTEXT LookupContext)
{
    /* Release the directory lock */
    KE::PushLock::ReleasePushLock(&Directory->Lock);

    /* Update the lock state */
    LookupContext->LockStateSignature = OBJECT_LOCK_STATE_RELEASED_SIGNATURE;

    /* Restore APC delivery */
    KE::KThread::LeaveCriticalRegion();
}

/**
 * Releases all resources held within an Object Manager lookup context.
 *
 * @param LookupContext
 *        Supplies a pointer to the active lookup context to be released.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::Directory::ReleaseLookupContext(IN OUT POBJECT_LOOKUP_CONTEXT LookupContext)
{
    /* Verify if a directory lock is held */
    if(LookupContext->DirectoryLocked)
    {
        /* Release the directory lock */
        ReleaseDirectoryLock(LookupContext->Directory, LookupContext);

        /* Update lookup context state */
        LookupContext->Directory = NULLPTR;
        LookupContext->DirectoryLocked = FALSE;
    }

    /* Release the object references held within a lookup context */
    ReleaseLookupContextObject(LookupContext);
}

/**
 * Releases the object reference held within a lookup context.
 *
 * @param LookupContext
 *        Supplies a pointer to the active lookup context containing the object to be released.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::Directory::ReleaseLookupContextObject(IN OUT POBJECT_LOOKUP_CONTEXT LookupContext)
{
    POBJECT_HEADER_NAME_INFO NameInfo;
    POBJECT_HEADER ObjectHeader;
    PVOID Object;

    /* Extract the cached object pointer from the lookup context */
    Object = LookupContext->Object;
    if(Object)
    {
        /* Resolve the object header */
        ObjectHeader = CONTAIN_RECORD(Object, OBJECT_HEADER, Body);

        /* Retrieve the optional name information header */
        NameInfo = OB::LifeCycle::GetObjectNameInformation(ObjectHeader);

        /* Verify if the object has a registered name */
        if(NameInfo)
        {
            /* Drop the name query reference */
            OB::LifeCycle::DereferenceObjectNameInformation(NameInfo);
        }

        /* Release the pointer reference */
        OB::LifeCycle::DereferenceObject(Object);

        /* Purge the pointer */
        LookupContext->Object = NULLPTR;
    }
}

/**
 * Traverses an object directory hash bucket to find a matching entry.
 *
 * @param Directory
 *        Supplies a pointer to the object directory being searched.
 *
 * @param Name
 *        Supplies ap ointer to the target object name.
 *
 * @param CaseInsensitive
 *        Specifies whether the string comparison should ignore case.
 *
 * @param HashIndex
 *        Supplies the pre-computed hash bucket index used to locate the collision chain.
 *
 * @param LookupContext
 *        Supplies a pointer to the caller's lookup context.
 *
 * @return This routine returns a pointer to the resolved object body if found, or NULLPTR otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
PVOID
OB::Directory::SearchDirectory(IN POBJECT_DIRECTORY Directory,
                               IN PCUNICODE_STRING Name,
                               IN BOOLEAN CaseInsensitive,
                               IN ULONG HashIndex,
                               IN POBJECT_LOOKUP_CONTEXT LookupContext)
{
    POBJECT_DIRECTORY_ENTRY DirectoryEntry, *HeadDirectoryEntry, *LookupBucket;
    POBJECT_HEADER_NAME_INFO NameInfo;
    POBJECT_HEADER ObjectHeader;

    /* Resolve the head pointer of the target hash bucket chain */
    LookupBucket = (POBJECT_DIRECTORY_ENTRY*)&Directory->HashBuckets[HashIndex];

    /* Iterate through the collision chain */
    HeadDirectoryEntry = LookupBucket;
    while((DirectoryEntry = *HeadDirectoryEntry) != NULLPTR)
    {
        /* Resolve the underlying object header */
        ObjectHeader = CONTAIN_RECORD(DirectoryEntry->Object, OBJECT_HEADER, Body);

        /* Retrieve the optional name information header */
        NameInfo = OB::LifeCycle::GetObjectNameInformation(ObjectHeader);

        /* Check if the names match */
        if((Name->Length == NameInfo->Name.Length) &&
           RTL::Unicode::CompareString(Name, &NameInfo->Name, CaseInsensitive) == 0)
        {
            /* Check if it is already at the head of the bucket */
            if(HeadDirectoryEntry != LookupBucket)
            {
                /* Check if the directory is locked */
                if(LookupContext->DirectoryLocked || KE::PushLock::ConvertSharedPushLockToExclusive(&Directory->Lock))
                {
                    /* Relocate the entry to the front */
                    *HeadDirectoryEntry = DirectoryEntry->ChainLink;
                    DirectoryEntry->ChainLink = *LookupBucket;
                    *LookupBucket = DirectoryEntry;
                }
            }

            /* Return the pointer to the object body */
            return DirectoryEntry->Object;
        }

        /* Advance to the next node entry */
        HeadDirectoryEntry = &DirectoryEntry->ChainLink;
    }

    /* Return NULL pointer*/
    return NULLPTR;
}
