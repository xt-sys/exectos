/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ob/typereg.cc
 * DESCRIPTION:     Object Manager Type Registry
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Creates and initializes a new object type within the Object Manager namespace.
 *
 * @param TypeName
 *        Supplies the requested name of the new object type.
 *
 * @param Initializer
 *        Supplies the initialization parameters for the new object type.
 *
 * @param SecurityDescriptor
 *        Supplies an optional security descriptor. (Currently ignored).
 *
 * @param ObjectType
 *        Receives a pointer to the newly created object type.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::TypeRegistry::CreateObjectType(IN PUNICODE_STRING TypeName,
                                   IN POBJECT_TYPE_INITIALIZER Initializer,
                                   IN PSECURITY_DESCRIPTOR SecurityDescriptor,
                                   OUT POBJECT_TYPE *ObjectType)
{
    POBJECT_HEADER_CREATOR_INFO CreatorInfo;
    OBJECT_LOOKUP_CONTEXT LookupContext;
    POBJECT_HEADER NewObjectTypeHeader;
    POBJECT_TYPE NewObjectType;
    UNICODE_STRING ObjectName;
    XTSTATUS Status;

    /* Validate incoming parameters */
    Status = ValidateObjectTypeParameters(TypeName, Initializer);
    if(Status != STATUS_SUCCESS)
    {
        /* Parameter validation failed, return error code */
        return Status;
    }

    /* Initialize the lookup context */
    OB::Directory::InitializeLookupContext(&LookupContext);

    /* Check if the global type directory exists */
    if(ObjectTypeDirectory)
    {
        /* Acquire the directory lock and prepare for namespace lookup */
        OB::Directory::LockLookupContext(ObjectTypeDirectory, &LookupContext);

        /* Search the directory to check if an object type with this name already exists */
        if(OB::Directory::LookupDirectoryEntry(ObjectTypeDirectory, TypeName, OBJECT_CASE_INSENSITIVE,
                                               FALSE, &LookupContext))
        {
            /* A collision detected, release the directory lock and lookup context */
            OB::Directory::ReleaseLookupContext(&LookupContext);

            /* Reject the creation due to a naming conflict */
            return STATUS_OBJECT_NAME_COLLISION;
        }
    }

    /* Allocate paged memory for the persistent object type name */
    Status = MM::Allocator::AllocatePool(PagedPool, (ULONG)TypeName->MaximumLength,
                                         (PVOID*)&ObjectName.Buffer, TAG_OB_NAME);
    if(Status != STATUS_SUCCESS || ObjectName.Buffer == NULLPTR)
    {
        /* Pool allocation failed; release the lookup context and return error code */
        OB::Directory::ReleaseLookupContext(&LookupContext);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Copy the requested type name */
    ObjectName.MaximumLength = TypeName->MaximumLength;
    RTL::Unicode::CopyString(&ObjectName, TypeName);

    /* Allocate the physical memory block for the Object Type */
    Status = OB::LifeCycle::AllocateObject(NULLPTR, KernelMode, MasterObjectType, &ObjectName,
                                           sizeof(OBJECT_TYPE), &NewObjectTypeHeader);
    if(Status != STATUS_SUCCESS)
    {
        /* Object allocation failed; release the directory lock and free allocated name buffer */
        OB::Directory::ReleaseLookupContext(&LookupContext);
        MM::Allocator::FreePool(ObjectName.Buffer);

        /* Return error code */
        return Status;
    }

    /* Resolve the object body pointer and assign the captured name */
    NewObjectTypeHeader->Flags |= OBJECT_FLAG_KERNEL_MODE | OBJECT_FLAG_PERMANENT;
    NewObjectType = (POBJECT_TYPE)&NewObjectTypeHeader->Body;
    NewObjectType->Name = ObjectName;

    /* Zero out the statistics block */
    RTL::Memory::ZeroMemory(&NewObjectType->TotalNumberOfObjects,
                            FIELD_OFFSET(OBJECT_TYPE, TypeInfo) - FIELD_OFFSET(OBJECT_TYPE, TotalNumberOfObjects));

    /* Check if master object type is already initialized */
    if(MasterObjectType == NULLPTR)
    {
        /* Set the global master type, link its header to itself, and set the default tag */
        MasterObjectType = NewObjectType;
        NewObjectTypeHeader->Type = MasterObjectType;
        NewObjectType->Key = TAG_OB_OBJECT_TYPE;
        NewObjectType->TotalNumberOfObjects = 1;
    }
    else
    {
        /* Generate a clean tag from object name */
        NewObjectType->Key = GenerateObjectPoolTag(TypeName);
    }

    /* Initialize default objects and pool charges */
    InitializeObjectType(NewObjectType, Initializer, &ObjectName);

    /* Suspend APCs and acquire exclusive access to the master type tracking list */
    KE::KThread::EnterCriticalRegion();
    KE::PushLock::AcquireExclusivePushLock(&MasterObjectType->TypeLock);

    /* Retrieve the creator information header */
    CreatorInfo = OB::LifeCycle::GetObjectCreatorInformation(NewObjectTypeHeader);
    if(CreatorInfo != NULLPTR)
    {
        /* Link the new object type */
        RTL::LinkedList::InsertTailList(&MasterObjectType->TypeList, &CreatorInfo->TypeList);

        /* Prevent double-insertion during handle creation */
        NewObjectTypeHeader->Flags &= ~OBJECT_FLAG_CREATOR_INFO;
    }

    /* Assign a type index */
    NewObjectType->Index = MasterObjectType->TotalNumberOfObjects;
    if(NewObjectType->Index < OBJECT_MAX_DEFINED_OBJECT_TYPES)
    {
        /* Register the new type in the global table */
        ObjectTypesTable[NewObjectType->Index - 1] = NewObjectType;
    }

    /* Release the master type lock and restore APC delivery */
    KE::PushLock::ReleaseExclusivePushLock(&MasterObjectType->TypeLock);
    KE::KThread::LeaveCriticalRegion();

    /* Publish into the namespace directory */
    if(ObjectTypeDirectory == NULLPTR ||
       OB::Directory::InsertDirectoryEntry(ObjectTypeDirectory, &LookupContext, NewObjectTypeHeader))
    {
        /* Verify if the directory is present */
        if(ObjectTypeDirectory)
        {
            /* Increment the reference count */
            OB::LifeCycle::ReferenceObject(ObjectTypeDirectory);
        }

        /* Release the directory lock and context */
        OB::Directory::ReleaseLookupContext(&LookupContext);

        /* Return the pointer to the object type */
        *ObjectType = NewObjectType;
        return STATUS_SUCCESS;
    }
    else
    {
        /* Directory insertion failed, release lock and return error code */
        OB::Directory::ReleaseLookupContext(&LookupContext);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
}

/**
 * Cleans up internal resources associated with an object type before its deletion.
 *
 * @param Object
 *        Supplies a pointer to the Object Type body being deleted.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::TypeRegistry::DeleteObjectType(IN PVOID Object)
{
    /* Nothing to do */
}

/**
 * Generates a 4-byte pool tag based on the object type name.
 *
 * @param TypeName
 *        Supplies the name of the object type.
 *
 * @return This routine returns a 32-bit pool tag.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
OB::TypeRegistry::GenerateObjectPoolTag(IN PUNICODE_STRING TypeName)
{
    ULONG Index, Tag;
    PUCHAR TagBytes;

    /* Default tag to four spaces */
    Tag = SIGNATURE32(' ', ' ', ' ', ' ');
    TagBytes = (PUCHAR)&Tag;

    /* Extract up to the first 4 characters */
    for(Index = 0; Index < 4; Index++)
    {
        /* Verify if the current index falls within the valid bounds */
        if(Index < (TypeName->Length / sizeof(WCHAR)))
        {
            /* Strip the high unicode byte and store the resulting ASCII character  */
            TagBytes[Index] = (UCHAR)(TypeName->Buffer[Index] & 0xFF);
        }
    }

    /* Return the tag */
    return Tag;
}

/**
 * Initializes the internal body and configuration of a newly allocated object type.
 *
 * @param ObjectType
 *        Supplies a pointer to the object type to initialize.
 *
 * @param Initializer
 *        Supplies the initialization parameters.
 *
 * @param TypeName
 *        Supplies the name of the object type.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::TypeRegistry::InitializeObjectType(IN OUT POBJECT_TYPE ObjectType,
                                       IN POBJECT_TYPE_INITIALIZER Initializer,
                                       IN PUNICODE_STRING TypeName)
{
    ULONG HeaderCharge;

    /* Calculate standard header charges */
    HeaderCharge = sizeof(OBJECT_HEADER) + sizeof(OBJECT_HEADER_NAME_INFO);

    /* Verify if the object type requires handle count tracking */
    if(Initializer->MaintainHandleCount)
    {
        /* Include the size of the handle information header in the total charge */
        HeaderCharge += sizeof(OBJECT_HEADER_HANDLE_INFO);
    }

    /* Copy initialization data */
    RTL::Memory::CopyMemory(&ObjectType->TypeInfo, Initializer, sizeof(OBJECT_TYPE_INITIALIZER));

    /* Enforce global type list maintenance and configure the target pool type */
    ObjectType->TypeInfo.MaintainTypeList = TRUE;
    ObjectType->TypeInfo.PoolType = Initializer->PoolType;

    /* Check pool type */
    if(Initializer->PoolType == NonPagedPool)
    {
        /* Apply the charge to the non-paged pool requirement */
        ObjectType->TypeInfo.DefaultNonPagedPoolCharge += HeaderCharge;
    }
    else
    {
        /* Apply the charge to the paged pool requirement */
        ObjectType->TypeInfo.DefaultPagedPoolCharge += HeaderCharge;
    }

    /* Check if the caller supplied a custom procedure for security descriptor operations */
    if(Initializer->SecurityProcedure == NULLPTR)
    {
        /* No custom procedure provided, attach the generic system handler */
        ObjectType->TypeInfo.SecurityProcedure = OB::Security::ProcessObjectSecurityDescriptor;
    }

    /* Initialize synchronization mechanisms */
    KE::PushLock::InitializePushLock(&ObjectType->TypeLock);

    /* Initialize the queue for registered callbacks and the tracking list */
    RTL::LinkedList::InitializeListHead(&ObjectType->CallbackList);
    RTL::LinkedList::InitializeListHead(&ObjectType->TypeList);

    /* Check if the object type has a default object */
    if(ObjectType->TypeInfo.UseDefaultObject)
    {
        /* Map to the global default event and append the SYNCHRONIZE access right */
        ObjectType->DefaultObject = &DefaultEvent;
        ObjectType->TypeInfo.ValidAccessMask |= SE_SYNCHRONIZE;
    }
    else if(TypeName->Length == 8 && !RTL::WideString::CompareWideString(TypeName->Buffer, L"File", 0))
    {
        /* Point the synchronization offset directly to the internal Event field */
        ObjectType->DefaultObject = (PVOID)(ULONG_PTR)FIELD_OFFSET(FILE_OBJECT, Event);
    }
    else if(TypeName->Length == 24 && !RTL::WideString::CompareWideString(TypeName->Buffer, L"WaitablePort", 0))
    {
        /* Point the synchronization offset directly to the WaitEvent field */
        ObjectType->DefaultObject = (PVOID)(ULONG_PTR)FIELD_OFFSET(LPCP_PORT_OBJECT, WaitEvent);
    }
    else
    {
        /* Disable synchronization for this object type */
        ObjectType->DefaultObject = NULLPTR;
    }
}

/**
 * Initializes the global Object Type Registry and bootstraps the master type object.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::TypeRegistry::InitializeObjectTypeRegistry(VOID)
{
    OBJECT_TYPE_INITIALIZER ObjectTypeInitializer;
    UNICODE_STRING TypeName;
    XTSTATUS Status;

    /* Initialize the global directory pointer */
    ObjectTypeDirectory = NULLPTR;

    /* Clear the global object type lookup table and the default synchronization event block */
    RTL::Memory::ZeroMemory(ObjectTypesTable, sizeof(ObjectTypesTable));
    RTL::Memory::ZeroMemory(&DefaultEvent, sizeof(KEVENT));

    /* Clear the initialization structure */
    RTL::Memory::ZeroMemory(&ObjectTypeInitializer, sizeof(OBJECT_TYPE_INITIALIZER));

    /* Configure the attributes and pool charges for the master "Type" object */
    ObjectTypeInitializer.DefaultNonPagedPoolCharge = sizeof(OBJECT_TYPE);
    ObjectTypeInitializer.InvalidAttributes = OBJECT_OPENLINK;
    ObjectTypeInitializer.Length = sizeof(OBJECT_TYPE_INITIALIZER);
    ObjectTypeInitializer.MaintainTypeList = TRUE;
    ObjectTypeInitializer.PoolType = NonPagedPool;
    ObjectTypeInitializer.UseDefaultObject = TRUE;
    ObjectTypeInitializer.ValidAccessMask = SE_OBJECT_TYPE_ALL_ACCESS;

    /* Register default destructor */
    ObjectTypeInitializer.DeleteProcedure = &DeleteObjectType;

    /* Initialize the descriptor for the "Type" name */
    RTL::Unicode::InitializeString(&TypeName, L"Type");

    /* Create the master object type */
    Status = CreateObjectType(&TypeName, &ObjectTypeInitializer, NULLPTR, &MasterObjectType);
    if(Status != STATUS_SUCCESS)
    {
        /* Object type creation failed, return error code */
        return STATUS_UNSUCCESSFUL;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Validates the parameters supplied for creating a new object type.
 *
 * @param TypeName
 *        Supplies the requested name of the new object type.
 *
 * @param Initializer
 *        Supplies the initialization parameters for the new object type.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::TypeRegistry::ValidateObjectTypeParameters(IN PUNICODE_STRING TypeName,
                                               IN POBJECT_TYPE_INITIALIZER Initializer)
{
    ULONG Index;
    PWCH Buffer;

    /* Check the basic string validity and alignment */
    if(TypeName == NULLPTR || TypeName->Length == 0 || (TypeName->Length % sizeof(WCHAR)) != 0)
    {
        /* Invalid type name, return error code */
        return STATUS_INVALID_PARAMETER;
    }

    /* Check initializer block integrity and allowed flags */
    if(Initializer == NULLPTR ||
       (Initializer->InvalidAttributes & ~OBJECT_VALID_ATTRIBUTES) != 0 ||
       Initializer->Length != sizeof(OBJECT_TYPE_INITIALIZER))
    {
        /* Invalid initializer block, return error code */
        return STATUS_INVALID_PARAMETER;
    }

    /* Check the maintaining handle count and procedure presence */
    if(Initializer->MaintainHandleCount &&
       Initializer->OpenProcedure == NULLPTR &&
       Initializer->CloseProcedure == NULLPTR)
    {
        /* Invalid initializer block, return error code */
        return STATUS_INVALID_PARAMETER;
    }

    /* Validate memory pool type */
    if(!Initializer->UseDefaultObject && Initializer->PoolType != NonPagedPool)
    {
        /* Invalid pool type, return error code */
        return STATUS_INVALID_PARAMETER;
    }

    /* Extract the buffer pointer */
    Buffer = TypeName->Buffer;
    Index = TypeName->Length / sizeof(WCHAR);

    /* Scan for illegal path separators in the type name */
    while(Index--)
    {
        /* Check for path separator */
        if(*Buffer++ == OBJECT_NAME_PATH_SEPARATOR)
        {
            /* Invalid type name, return error code */
            return STATUS_OBJECT_NAME_INVALID;
        }
    }

    /* Return success */
    return STATUS_SUCCESS;
}
