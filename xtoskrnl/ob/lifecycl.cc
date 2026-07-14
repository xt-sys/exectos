/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ob/lifecycl.cc
 * DESCRIPTION:     Object Manager Lifecycle Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Allocates and initializes the memory block for a new object and its headers.
 *
 * @param CreateInfo
 *        Supplies a pointer to the object creation information.
 *
 * @param OwnerProcessorMode
 *        Supplies the processor mode that will own the object.
 *
 * @param ObjectType
 *        Supplies an optional pointer to the object type definition.
 *
 * @param ObjectName
 *        Supplies an optional pointer to the unicode string representing the object's name.
 *
 * @param ObjectBodySize
 *        Supplies the size, in bytes, of the object body itself.
 *
 * @param ReturnedObjectHeader
 *        Receives a pointer to the newly allocated object header.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::LifeCycle::AllocateObject(IN POBJECT_CREATE_INFORMATION CreateInfo,
                              IN KPROCESSOR_MODE OwnerProcessorMode,
                              IN POBJECT_TYPE ObjectType,
                              IN PUNICODE_STRING ObjectName,
                              IN ULONG ObjectBodySize,
                              OUT POBJECT_HEADER *ReturnedObjectHeader)
{
    POBJECT_HEADER_CREATOR_INFO CreatorInfoHeader;
    POBJECT_HEADER_HANDLE_INFO HandleInfoHeader;
    POBJECT_HEADER_QUOTA_INFO QuotaInfoHeader;
    POBJECT_HEADER_NAME_INFO NameInfoHeader;
    OBJECT_OPTIONAL_HEADER_LAYOUT Layout;
    ULONG Tag, TotalAllocationSize;
    POBJECT_HEADER ObjectHeader;
    MMPOOL_TYPE PoolType;
    PVOID PoolBuffer;
    XTSTATUS Status;
    PUCHAR Offset;

    /* Calculate the sizes for all optional headers */
    CalculateOptionalHeaderSize(CreateInfo, ObjectType, ObjectName, &Layout);

    /* Determine the pool type */
    if(ObjectType == NULLPTR || ObjectType->TypeInfo.PoolType == NonPagedPool)
    {
        /* Use non-paged pool */
        PoolType = NonPagedPool;
    }
    else
    {
        /* Use paged pool */
        PoolType = PagedPool;
    }

    /* Compute the total required block size */
    TotalAllocationSize = Layout.TotalSize + sizeof(OBJECT_HEADER) + ObjectBodySize;
    Tag = (ObjectType == NULLPTR) ? TAG_OB_OBJECT_TYPE : ObjectType->Key;

    /* Allocate the memory block from the selected pool */
    Status = MM::Allocator::AllocatePool(PoolType, TotalAllocationSize, &PoolBuffer, Tag);
    if(Status != STATUS_SUCCESS || PoolBuffer == NULLPTR)
    {
        /* Allocation failed, return error code */
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Set the initial offset */
    Offset = (PUCHAR)PoolBuffer;

    /* Check if there is a Quota Information header */
    if(Layout.QuotaInfoSize != 0)
    {
        /* Initialize the Quota Information header */
        QuotaInfoHeader = (POBJECT_HEADER_QUOTA_INFO)Offset;
        QuotaInfoHeader->ExclusiveProcess = NULLPTR;
        QuotaInfoHeader->NonPagedPoolCharge = CreateInfo->NonPagedPoolCharge;
        QuotaInfoHeader->PagedPoolCharge = CreateInfo->PagedPoolCharge;
        QuotaInfoHeader->SecurityDescriptorCharge = CreateInfo->SecurityDescriptorCharge;

        /* Advance the pointer */
        Offset += Layout.QuotaInfoSize;
    }

    /* Check if there is a Handle Information header */
    if(Layout.HandleInfoSize != 0)
    {
        /* Initialize the Handle Information header */
        HandleInfoHeader = (POBJECT_HEADER_HANDLE_INFO)Offset;
        HandleInfoHeader->SingleEntry.HandleCount = 0;

        /* Advance the pointer */
        Offset += Layout.HandleInfoSize;
    }

    /* Check if there is a Name Information header */
    if(Layout.NameInfoSize != 0)
    {
        /* Initialize the Name Information header */
        NameInfoHeader = (POBJECT_HEADER_NAME_INFO)Offset;
        NameInfoHeader->Directory = NULLPTR;
        NameInfoHeader->Name = *ObjectName;
        NameInfoHeader->QueryReferences = 1;

        /* Advance the pointer */
        Offset += Layout.NameInfoSize;
    }

    /* Check if there is a Creator Information header */
    if(Layout.CreatorInfoSize != 0)
    {
        /* Initialize the Creator Information header */
        CreatorInfoHeader = (POBJECT_HEADER_CREATOR_INFO)Offset;
        CreatorInfoHeader->CreatorBackTraceIndex = 0;
        CreatorInfoHeader->CreatorUniqueProcess = PS::Process::GetCurrentProcess()->UniqueProcessId;
        RTL::LinkedList::InitializeListHead(&CreatorInfoHeader->TypeList);

        /* Advance the pointer */
        Offset += Layout.CreatorInfoSize;
    }

    /* Retrieve the object header */
    ObjectHeader = (POBJECT_HEADER)Offset;

    /* Initialize the object header */
    ObjectHeader->Flags = OBJECT_FLAG_NEW_OBJECT;
    ObjectHeader->HandleInfoOffset = Layout.HandleInfoSize ? (UCHAR)(Layout.TotalSize - Layout.QuotaInfoSize) : 0;
    ObjectHeader->NameInfoOffset = Layout.NameInfoSize ? (UCHAR)(Layout.CreatorInfoSize + Layout.NameInfoSize) : 0;
    ObjectHeader->QuotaInfoOffset = Layout.QuotaInfoSize ? (UCHAR)Layout.TotalSize : 0;

    /* Check if there is creator information */
    if(Layout.CreatorInfoSize != 0)
    {
        /* Flag the presence of creator information */
        ObjectHeader->Flags |= OBJECT_FLAG_CREATOR_INFO;
    }

    /* Check if there is handle information */
    if(Layout.HandleInfoSize != 0)
    {
        /* Flag the presence of handle information */
        ObjectHeader->Flags |= OBJECT_FLAG_SINGLE_PROCESS;
    }

    /* Check if the object is being created in kernel mode */
    if(OwnerProcessorMode == KernelMode)
    {
        /* Assign kernel mode ownership */
        ObjectHeader->Flags |= OBJECT_FLAG_KERNEL_MODE;
    }

    /* Check if creation information was provided */
    if(CreateInfo)
    {
        /* Check if the object is permanent */
        if(CreateInfo->Attributes & OBJECT_PERMANENT)
        {
            /* Flag the object to prevent deletion */
            ObjectHeader->Flags |= OBJECT_FLAG_PERMANENT;
        }

        /* Check if the object is exclusive */
        if(CreateInfo->Attributes & OBJECT_EXCLUSIVE)
        {
            /* Flag the object as exclusive to a single process */
            ObjectHeader->Flags |= OBJECT_FLAG_EXCLUSIVE;
        }
    }

    /* Initialize reference counters and data associations */
    ObjectHeader->HandleCount = 0;
    ObjectHeader->ObjectCreateInfo = CreateInfo;
    ObjectHeader->PointerCount = 1;
    ObjectHeader->SecurityDescriptor = NULLPTR;
    ObjectHeader->Type = ObjectType;

    /* Check if an object type was provided */
    if(ObjectType)
    {
        /* Increment the total active objects counter */
        RTL::Atomic::Increment32((VOLATILE PLONG)&ObjectType->TotalNumberOfObjects);

        /* Check if the current total exceeds the previous peak */
        if(ObjectType->TotalNumberOfObjects > ObjectType->HighWaterNumberOfObjects)
        {
            /* Update the high-water mark */
            ObjectType->HighWaterNumberOfObjects = ObjectType->TotalNumberOfObjects;
        }
    }

    /* Return the pointer to the initialized object */
    *ReturnedObjectHeader = ObjectHeader;

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Allocates a buffer for an object name string.
 *
 * @param Length
 *        Supplies the exact length of the object name in bytes, excluding the NULL terminator.
 *
 * @param UseLookaside
 *        Indicates whether the allocation should attempt to draw from the paged pool lookaside list.
 *
 * @param ObjectName
 *        Supplies a pointer to the Unicode string descriptor to be initialized with the new buffer.
 *
 * @return This routine returns a pointer to the allocated buffer, or NULLPTR if allocation fails.
 *
 * @since XT 1.0
 */
XTFASTCALL
PWCH
OB::LifeCycle::AllocateObjectName(IN ULONG Length,
                                  IN BOOLEAN UseLookaside,
                                  IN OUT PUNICODE_STRING ObjectName)
{
    ULONG MaximumLength;
    PVOID Buffer;

    /* Calculate the required maximum length and initialize the buffer pointer */
    MaximumLength = Length + sizeof(WCHAR);
    Buffer = NULLPTR;

    /* Check if the general pool must be used */
    if(!UseLookaside || MaximumLength > OBJECT_NAME_BUFFER_SIZE)
    {
        /* Allocate from the Memory Manager */
        MM::Allocator::AllocatePool(PagedPool, MaximumLength, (PVOID *)&Buffer, TAG_OB_NAME);
    }
    else
    {
        /* Set maximum length to the fixed lookaside entry size */
        MaximumLength = OBJECT_NAME_BUFFER_SIZE;

        /* Allocate from the lookaside list */
        Buffer = EX::LookasideList::AllocateFromPerProcessorList(LookasideNameBufferList);
    }

    /* Initialize the Unicode string descriptor */
    ObjectName->Buffer = (PWCH)Buffer;
    ObjectName->Length = (USHORT)Length;
    ObjectName->MaximumLength = (USHORT)MaximumLength;

    /* Return the buffer pointer */
    return (PWCH)Buffer;
}

/**
 * Calculates the memory layout and sizes for optional object header components.
 *
 * @param CreateInfo
 *        Supplies a pointer to the object creation information.
 *
 * @param ObjectType
 *        Supplies an optional pointer to the object type definition.
 *
 * @param ObjectName
 *        Supplies an optional pointer to the unicode string representing the object's name.
 *
 * @param Layout
 *        Supplies a pointer to the layout structure that will receive the calculated sizes.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::LifeCycle::CalculateOptionalHeaderSize(IN POBJECT_CREATE_INFORMATION CreateInfo,
                                           IN POBJECT_TYPE ObjectType,
                                           IN PUNICODE_STRING ObjectName,
                                           OUT POBJECT_OPTIONAL_HEADER_LAYOUT Layout)
{
    /* Zero the layout structure */
    RTL::Memory::ZeroMemory(Layout, sizeof(OBJECT_OPTIONAL_HEADER_LAYOUT));

    /* Check if the CreateInfo pointer is valid */
    if(CreateInfo == NULLPTR)
    {
        /* Assign default sizes */
        Layout->NameInfoSize = sizeof(OBJECT_HEADER_NAME_INFO);
        Layout->CreatorInfoSize = sizeof(OBJECT_HEADER_CREATOR_INFO);
    }
    else
    {
        /* Determine if quota tracking is required */
        if(ObjectType != NULLPTR &&
           (((CreateInfo->PagedPoolCharge != ObjectType->TypeInfo.DefaultPagedPoolCharge ||
              CreateInfo->NonPagedPoolCharge != ObjectType->TypeInfo.DefaultNonPagedPoolCharge ||
              CreateInfo->SecurityDescriptorCharge > SE_DEFAULT_SECURITY_QUOTA) &&
              PS::Process::GetCurrentProcess() != PS::Process::GetSystemProcess()) ||
             (CreateInfo->Attributes & OBJECT_EXCLUSIVE)))
        {
            /* Set the size for the Quota Information header */
            Layout->QuotaInfoSize = sizeof(OBJECT_HEADER_QUOTA_INFO);
        }

        /* Check if the object type requires maintaining handle counts */
        if(ObjectType && ObjectType->TypeInfo.MaintainHandleCount)
        {
            /* Set the size for the Handle Information header */
            Layout->HandleInfoSize = sizeof(OBJECT_HEADER_HANDLE_INFO);
        }

        /* Check if a valid object name was provided */
        if(ObjectName && ObjectName->Buffer)
        {
            /* Set the size for the Name Information header */
            Layout->NameInfoSize = sizeof(OBJECT_HEADER_NAME_INFO);
        }

        /* Check if the object type requires maintaining a creator type list */
        if(ObjectType && ObjectType->TypeInfo.MaintainTypeList)
        {
            /* Set the size for the Creator Information header */
            Layout->CreatorInfoSize = sizeof(OBJECT_HEADER_CREATOR_INFO);
        }
    }

    /* Calculate the total size required for all optional headers */
    Layout->TotalSize = Layout->QuotaInfoSize +
                        Layout->HandleInfoSize +
                        Layout->NameInfoSize +
                        Layout->CreatorInfoSize;
}

/**
 * Captures and validates object creation information and attributes passed.
 *
 * @param ObjectType
 *        Optionally supplies a pointer to the type of object being created.
 *
 * @param ProcessorMode
 *        Supplies the processor mode used to probe user-mode structures for accessibility.
 *
 * @param OwnerProcessorMode
 *        Supplies the processor mode that will own the object.
 *
 * @param ObjectAttributes
 *        Supplies a pointer to the object attributes structure.
 *
 * @param CapturedObjectName
 *        Supplies a pointer to a Unicode string descriptor that will receive the captured name.
 *
 * @param ObjectCreateInfo
 *        Supplies a pointer to the internal structure receiving the validated creation parameters.
 *
 * @param UseLookaside
 *        Indicates whether lookaside lists should be used for name buffer allocations.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::LifeCycle::CaptureObjectCreateInformation(IN POBJECT_TYPE ObjectType,
                                              IN KPROCESSOR_MODE ProcessorMode,
                                              IN KPROCESSOR_MODE OwnerProcessorMode,
                                              IN POBJECT_ATTRIBUTES ObjectAttributes,
                                              IN OUT PUNICODE_STRING CapturedObjectName,
                                              IN POBJECT_CREATE_INFORMATION ObjectCreateInfo,
                                              IN BOOLEAN UseLookaside)
{
    PSECURITY_QUALITY_OF_SERVICE SecurityQos;
    PSECURITY_DESCRIPTOR SecurityDescriptor;
    PUNICODE_STRING ObjectName;
    XTSTATUS Status;
    ULONG Size;

    /* Initialize local state variables */
    ObjectName = NULLPTR;
    Status = STATUS_SUCCESS;

    /* Zero out the output structure */
    RTL::Memory::ZeroMemory(ObjectCreateInfo, sizeof(OBJECT_CREATE_INFORMATION));

    /* Enter structured exception handler */
    __try
    {
        /* Check if the caller supplied object attributes */
        if(ObjectAttributes)
        {
            /* Check if the request originated from user mode */
            if(ProcessorMode != KernelMode)
            {
                /* Probe the object attributes structure */
                MM::Probe::ProbeForReadStructure(ObjectAttributes, sizeof(OBJECT_ATTRIBUTES), sizeof(ULONG_PTR));
            }

            /* Validate structure length and verify that no unknown attributes are specified */
            if((ObjectAttributes->Length != sizeof(OBJECT_ATTRIBUTES)) ||
               (ObjectAttributes->Attributes & ~OBJECT_VALID_ATTRIBUTES))
            {
                /* Return error code */
                Status = STATUS_INVALID_PARAMETER;
            }
            else
            {
                /* Capture the root directory and sanitize basic attribute flags */
                ObjectCreateInfo->RootDirectory = ObjectAttributes->RootDirectory;
                ObjectCreateInfo->Attributes = ObjectAttributes->Attributes & OBJECT_VALID_ATTRIBUTES;

                /* Check if the owner works in user mode */
                if(OwnerProcessorMode != KernelMode)
                {
                    /* Enforce kernel handle restrictions */
                    ObjectCreateInfo->Attributes &= ~OBJECT_KERNEL_HANDLE;
                }

                /* Cache pointers for subsequent capture and validation */
                ObjectName = ObjectAttributes->ObjectName;
                SecurityDescriptor = ObjectAttributes->SecurityDescriptor;
                SecurityQos = ObjectAttributes->SecurityQualityOfService;

                /* Check if security descriptor is present */
                if(SecurityDescriptor)
                {
                    /* Capture the security descriptor*/
                    Status = SE::Descriptor::CaptureSecurityDescriptor(SecurityDescriptor, ProcessorMode, PagedPool,
                                                                       TRUE, &ObjectCreateInfo->SecurityDescriptor);
                    if(Status != STATUS_SUCCESS)
                    {
                        /* Clear the descriptor pointer on failure */
                        ObjectCreateInfo->SecurityDescriptor = NULLPTR;
                    }
                    else
                    {
                        /* Compute and assign the security quota charge for the descriptor */
                        SE::Descriptor::ComputeQuotaInformationSize(ObjectCreateInfo->SecurityDescriptor, &Size);
                        ObjectCreateInfo->ProbeMode = ProcessorMode;
                        ObjectCreateInfo->SecurityDescriptorCharge = SE::Descriptor::ComputeSecurityQuota(Size);
                    }
                }

                /* Check if previous operations succeeded and QoS data is available */
                if(Status == STATUS_SUCCESS && SecurityQos)
                {
                    /* Check if the request originated from user mode */
                    if(ProcessorMode != KernelMode)
                    {
                        /* Probe the QoS structure */
                        MM::Probe::ProbeForReadStructure(SecurityQos, sizeof(SECURITY_QUALITY_OF_SERVICE), sizeof(ULONG));
                    }

                    /* Copy QoS data into the local structure and link the pointer */
                    ObjectCreateInfo->SecurityQualityOfService = *SecurityQos;
                    ObjectCreateInfo->SecurityQos = &ObjectCreateInfo->SecurityQualityOfService;
                }
            }
        }
    }
    __except (RTL::Exception::SystemFilter())
    {
        /* Catch memory access violations */
        Status = EXCEPTION_CODE;
    }

    /* Check If all structure and security captures succeeded */
    if(Status == STATUS_SUCCESS)
    {
        /* Validate if an object name is available */
        if(ObjectName)
        {
            /* Capture object name */
            Status = CaptureObjectName(ProcessorMode, ObjectName, CapturedObjectName, UseLookaside);
        }
        else
        {
            /* Initialize an empty name descriptor */
            CapturedObjectName->Buffer = NULLPTR;
            CapturedObjectName->Length = 0;
            CapturedObjectName->MaximumLength = 0;

            /* Check if this is a root directory */
            if(ObjectCreateInfo->RootDirectory)
            {
                /* Root directory cannot be used without an object name, set error code */
                Status = STATUS_OBJECT_NAME_INVALID;
            }
        }
    }

    /* Check if any steps failed */
    if(Status != STATUS_SUCCESS)
    {
        /* Release object creation resources */
        ReleaseObjectCreateInformation(ObjectCreateInfo);
    }

    /* Return status code */
    return Status;
}

/**
 * Captures an object name from user-mode or kernel-mode memory into a kernel buffer.
 *
 * @param ProcessorMode
 *        Supplies the processor mode used to probe user-mode structures for accessibility.
 *
 * @param ObjectName
 * Supplies a pointer to the original Unicode string descriptor to be captured.
 *
 * @param CapturedObjectName
 * Supplies a pointer to the Unicode string descriptor that will receive the captured buffer.
 *
 * @param UseLookaside
 * Indicates whether a lookaside list should be used for the buffer allocation if applicable.
 *
 * @return Returns STATUS_SUCCESS if the name was captured safely, or an appropriate error code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::LifeCycle::CaptureObjectName(IN KPROCESSOR_MODE ProcessorMode,
                                 IN PUNICODE_STRING ObjectName,
                                 IN OUT PUNICODE_STRING CapturedObjectName,
                                 IN BOOLEAN UseLookaside)
{
    UNICODE_STRING InputObjectName;
    XTSTATUS Status;
    ULONG Length;
    PWCH Buffer;

    /* Initialize local variables */
    Buffer = NULLPTR;
    Status = STATUS_SUCCESS;

    /* Initialize the captured name descriptor */
    CapturedObjectName->Buffer = NULLPTR;
    CapturedObjectName->Length = 0;
    CapturedObjectName->MaximumLength = 0;

    /* Enter structural exception handler */
    __try
    {
        /* Check if the string originates from user-mode */
        if(ProcessorMode != KernelMode)
        {
            /* Probe and copy the Unicode string descriptor */
            MM::Probe::ProbeForRead(ObjectName, sizeof(UNICODE_STRING), alignof(UNICODE_STRING));
            InputObjectName = *(CONST VOLATILE PUNICODE_STRING)ObjectName;

            /* Probe the string buffer for read access */
            MM::Probe::ProbeForRead(InputObjectName.Buffer, InputObjectName.Length, sizeof(WCHAR));
        }
        else
        {
            /* Copy the descriptor */
            InputObjectName = *ObjectName;
        }

        /* Check if the string is not empty */
        Length = InputObjectName.Length;
        if(Length > 0)
        {
            /* Ensure the length is properly aligned */
            if(((Length & (sizeof(WCHAR) - 1)) != 0) || (Length == (MAXUSHORT - sizeof(WCHAR) + 1)))
            {
                /* Set status code */
                Status = STATUS_OBJECT_NAME_INVALID;
            }
            else
            {
                /* Allocate the name buffer */
                Buffer = AllocateObjectName(Length, UseLookaside, CapturedObjectName);
                if(!Buffer)
                {
                    /* Buffer allocation failed, set status code */
                    Status = STATUS_INSUFFICIENT_RESOURCES;
                }
                else
                {
                    /* Copy the string data into the buffer */
                    RTL::Memory::CopyMemory(Buffer, InputObjectName.Buffer, Length);

                    /* NULL-terminate the string */
                    Buffer[Length / sizeof(WCHAR)] = (WCHAR)0;
                }
            }
        }
    }
    __except (RTL::Exception::SystemFilter())
    {
        /* Catch memory access violations */
        Status = EXCEPTION_CODE;

        /* Check if a buffer was allocated */
        if(Buffer)
        {
            /* Release the buffer */
            FreeObjectName(CapturedObjectName);
        }
    }

    /* Return status code */
    return Status;
}

/**
 * Creates a new object of the specified type and initializes it.
 *
 * @param ProcessorMode
 *        Supplies the processor mode used to probe parameters.
 *
 * @param ObjectType
 *        Supplies a pointer to the object type of the new object.
 *
 * @param ObjectAttributes
 *        Supplies an optional pointer to the object attributes structure.
 *
 * @param OwnerProcessorMode
 *        Supplies the processor mode that will own the object.
 *
 * @param ParseContext
 *        Supplies an optional pointer to a parse context structure.
 *
 * @param ObjectBodySize
 *        Supplies the size, in bytes, of the object body.
 *
 * @param PagedPoolCharge
 *        Supplies the paged pool charge for the object. If zero, default is used.
 *
 * @param NonPagedPoolCharge
 *        Supplies the non-paged pool charge for the object. If zero, default is used.
 *
 * @param Object
 *        Receives a pointer to the body of the newly created object.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::LifeCycle::CreateObject(IN KPROCESSOR_MODE ProcessorMode,
                            IN POBJECT_TYPE ObjectType,
                            IN POBJECT_ATTRIBUTES ObjectAttributes,
                            IN KPROCESSOR_MODE OwnerProcessorMode,
                            IN OUT PVOID ParseContext,
                            IN ULONG ObjectBodySize,
                            IN ULONG PagedPoolCharge,
                            IN ULONG NonPagedPoolCharge,
                            OUT PVOID *Object)
{
    POBJECT_CREATE_INFORMATION ObjectCreateInfo;
    UNICODE_STRING CapturedObjectName;
    POBJECT_HEADER ObjectHeader;
    XTSTATUS Status;

    /* Allocate the buffer for creation parameters */
    ObjectCreateInfo = (POBJECT_CREATE_INFORMATION)EX::LookasideList::AllocateFromPerProcessorList(LookasideCreateInfoList);
    if(!ObjectCreateInfo)
    {
        /* Allocation failed, return error code */
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Capture and probe the object attributes */
    Status = CaptureObjectCreateInformation(ObjectType, ProcessorMode, OwnerProcessorMode, ObjectAttributes,
                                            &CapturedObjectName, ObjectCreateInfo, FALSE);
    if(Status != STATUS_SUCCESS)
    {
        /* Capture failed, clean up and return error code */
        EX::LookasideList::FreeToPerProcessorList(LookasideCreateInfoList, ObjectCreateInfo);
        return Status;
    }

    /* Validate requested attributes */
    if(ObjectType && ObjectType->TypeInfo.InvalidAttributes & ObjectCreateInfo->Attributes)
    {
        /* Release the captured creation info structure */
        FreeObjectCreateInformation(ObjectCreateInfo);

        /* Check if object name was captured */
        if(CapturedObjectName.Buffer)
        {
            /* Release the object name buffer */
            FreeObjectName(&CapturedObjectName);
        }

        /* Return error code */
        return STATUS_INVALID_PARAMETER;
    }

    /* Check if paged pool charge was specified */
    if(PagedPoolCharge == 0)
    {
        /* Use default paged pool charge */
        PagedPoolCharge = ObjectType->TypeInfo.DefaultPagedPoolCharge;
    }

    /* Check if non-paged pool charge was specified */
    if(NonPagedPoolCharge == 0)
    {
        /* Use default non-paged pool charge */
        NonPagedPoolCharge = ObjectType->TypeInfo.DefaultNonPagedPoolCharge;
    }

    /* Set the paged and non-paged pool charges */
    ObjectCreateInfo->PagedPoolCharge = PagedPoolCharge;
    ObjectCreateInfo->NonPagedPoolCharge = NonPagedPoolCharge;

    /* Allocate the physical memory layout for the object */
    Status = AllocateObject(ObjectCreateInfo, OwnerProcessorMode, ObjectType,
                            &CapturedObjectName, ObjectBodySize, &ObjectHeader);
    if(Status != STATUS_SUCCESS)
    {
        /* Allocation failed, cleanup the captured data */
        FreeObjectCreateInformation(ObjectCreateInfo);

        /* Check if object name was captured */
        if(CapturedObjectName.Buffer)
        {
            /* Free the captured object name */
            FreeObjectName(&CapturedObjectName);
        }

        /* Return error code */
        return Status;
    }

    /* Point the output pointer to the object */
    *Object = &ObjectHeader->Body;

    /* Check if the object is permanent */
    if(ObjectHeader->Flags & OBJECT_FLAG_PERMANENT)
    {
        /* Apply security constraints */
        if(!SE::Privileges::CheckSinglePrivilege(SE_LUID_CREATE_PERMANENT_PRIVILEGE, ProcessorMode))
        {
            /* Privilege check failed, release the object and return error code */
            FreeObject(*Object);
            return STATUS_PRIVILEGE_NOT_HELD;
        }
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Enqueues an object header into a deferred deletion queue and queues a system worker thread if the list was empty.
 *
 * @param ObjectHeader
 *        Supplies a pointer to the object header to be queued for deletion.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::LifeCycle::DeferObjectDeletion(IN POBJECT_HEADER ObjectHeader)
{
    PVOID OldListHead, ActualListHead;

    /* Capture the snapshot of the removal list */
    OldListHead = RemoveObjectList;

    /* Enter the CAS loop */
    while(TRUE)
    {
        /* Link the current head to object header */
        ObjectHeader->NextToFree = OldListHead;

        /* Attempt to push the header to the list head */
        ActualListHead = RTL::Atomic::CompareExchangePointer(&RemoveObjectList, OldListHead, ObjectHeader);
        if(ActualListHead == OldListHead)
        {
            /* Successfully pushed to the stack, break the loop */
            break;
        }

        /* The list changed mid-flight, update the snapshot and retry */
        OldListHead = ActualListHead;
    }

    /* Check if the list was empty */
    if(!OldListHead)
    {
        /* Queue the deletion work item to the system worker queue */
        EX::WorkItem::QueueWorkItem(&RemoveObjectWorkItem, CriticalWorkQueue);
    }
}

/**
 * Destructs an object, including invoking security and type-specific deletion callbacks and freeing memory.
 *
 * @param Object
 *        Supplies a pointer to the object being destroyed.
 *
 * @param WorkerThread
 *        Indicates whether this routine is executed in the context of the system worker thread.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::LifeCycle::DeleteObject(IN PVOID Object,
                            IN BOOLEAN WorkerThread)
{
    POBJECT_HEADER_CREATOR_INFO CreatorInfo;
    POBJECT_HEADER_NAME_INFO NameInfo;
    POBJECT_HEADER ObjectHeader;
    POBJECT_TYPE ObjectType;

    /* Resolve object header */
    ObjectHeader = CONTAIN_RECORD(Object, OBJECT_HEADER, Body);

    /* Resolve  creator information, name information pointers and object type*/
    CreatorInfo = GetObjectCreatorInformation(ObjectHeader);
    NameInfo = GetObjectNameInformation(ObjectHeader);
    ObjectType = ObjectHeader->Type;

    /* Check if the object has creator info and is linked in a type tracking list */
    if(CreatorInfo && !RTL::LinkedList::ListEmpty(&CreatorInfo->TypeList))
    {
        /* Acquire exclusive access to the object type tracking list */
        KE::CriticalRegionGuard CriticalRegion;
        KE::PushLockExclusiveGuard PushLock(&ObjectType->TypeLock);

        /* Unlink the object from the creator type list */
        RTL::LinkedList::RemoveEntryList(&CreatorInfo->TypeList);
    }

    /* Check if the object has an allocated name buffer */
    if(NameInfo && NameInfo->Name.Buffer)
    {
        /* Free the name buffer pool allocation */
        FreeObjectName(&NameInfo->Name);

        /* Clear the name information fields */
        NameInfo->Name.Buffer = NULLPTR;
        NameInfo->Name.Length = 0;
        NameInfo->Name.MaximumLength = 0;
    }

    /* Check if a security descriptor is attached */
    if(ObjectHeader->SecurityDescriptor)
    {
        /* Invoke the security procedure to delete the descriptor */
        ObjectType->TypeInfo.SecurityProcedure(Object, DeleteSecurityDescriptor, NULLPTR, NULLPTR, NULLPTR,
                                               &ObjectHeader->SecurityDescriptor, NonPagedPool, NULLPTR);
    }

    /* Check if a custom deletion procedure is registered */
    if(ObjectType->TypeInfo.DeleteProcedure)
    {
        /* Check if execution is occurring outside of the system worker thread */
        if(!WorkerThread)
        {
            /* Mark the object for deferred deletion */
            ObjectHeader->Flags |= OBJECT_FLAG_DEFER_DELETE;
        }

        /* Invoke the type-specific delete procedure */
        ObjectType->TypeInfo.DeleteProcedure(Object);
    }

    /* Free the object memory */
    FreeObject(Object);
}

/**
 * Decrements the pointer reference count of an object.
 *
 * @param Object
 *        Supplies a pointer to the object body to dereference.
 *
 * @return This routine returns the updated reference count.
 */
XTFASTCALL
LONG_PTR
OB::LifeCycle::DereferenceObject(IN PVOID Object)
{
    POBJECT_HEADER ObjectHeader;
    LONG_PTR NewCount;

    /* Resolve the object header */
    ObjectHeader = CONTAIN_RECORD(Object, OBJECT_HEADER, Body);

    /* Decrement the reference count */
    NewCount = RTL::Atomic::Decrement64(&ObjectHeader->PointerCount);

    /* Check for object expiration */
    if(NewCount == 0)
    {
        /* Check if object can be deleted */
        if(KE::RunLevel::GetCurrentRunLevel() == PASSIVE_LEVEL)
        {
            /* Delete the object immediately */
            DeleteObject(Object, FALSE);
        }
        else
        {
            /* Defer deletion to worker thread */
            DeferObjectDeletion(ObjectHeader);
        }
    }

    /* Return the updated reference count */
    return NewCount;
}

/**
 * Decrements the pointer reference count of an object by a specified amount.
 *
 * @param Object
 *        Supplies a pointer to the object body to dereference.
 *
 * @param Count
 *        Specifies a number of references to release.
 *
 * @return This routine returns the updated reference count.
 */
XTFASTCALL
LONG_PTR
OB::LifeCycle::DereferenceObject(IN PVOID Object,
                                 IN ULONG Count)
{
    POBJECT_HEADER ObjectHeader;
    LONG_PTR NewCount;

    /* Resolve the object header */
    ObjectHeader = CONTAIN_RECORD(Object, OBJECT_HEADER, Body);

    /* Decrement the reference count */
    NewCount = RTL::Atomic::ExchangeAdd64(&ObjectHeader->PointerCount, -(LONG_PTR)(Count)) - Count;

    /* Check for object expiration */
    if(NewCount == 0)
    {
        /* Defer deletion to worker thread */
        DeferObjectDeletion(ObjectHeader);
    }

    /* Return the updated reference count */
    return NewCount;
}

/**
 * Decrements the reference count of an object and defers its deletion to a worker thread if the count reaches zero.
 *
 * @param Object
 *        Supplies a pointer to the object body.
 *
 * @return This routine returns the updated reference count.
 *
 * @since XT 1.0
 */
XTFASTCALL
LONG_PTR
OB::LifeCycle::DereferenceObjectDeferDelete(IN PVOID Object)
{
    POBJECT_HEADER ObjectHeader;
    LONG_PTR NewCount;

    /* Resolve the object header */
    ObjectHeader = CONTAIN_RECORD(Object, OBJECT_HEADER, Body);

    /* Decrement the reference count */
    NewCount = RTL::Atomic::Decrement64(&ObjectHeader->PointerCount);

    /* Check for object expiration */
    if(NewCount == 0)
    {
        /* Defer deletion to worker thread */
        DeferObjectDeletion(ObjectHeader);
    }

    /* Return the updated reference count */
    return NewCount;
}

/**
 * Decrements the query reference count of a NameInfo structure and frees its resources if the count reaches zero.
 *
 * @param NameInfo
 *        Supplies a pointer to the object header name information structure.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::LifeCycle::DereferenceObjectNameInformation(IN POBJECT_HEADER_NAME_INFO NameInfo)
{
    POBJECT_DIRECTORY Directory;

    /* Ensure the name info pointer is valid */
    if(!NameInfo)
    {
        /* Name info pointer is invalid, return */
        return;
    }

    /* Decrement the query reference count */
    if(RTL::Atomic::Decrement32((PLONG)&NameInfo->QueryReferences) != 0)
    {
        /* References are still active, return */
        return;
    }

    /* Check if the name buffer is allocated */
    if(NameInfo->Name.Buffer)
    {
        /* Free the name buffer */
        FreeObjectName(&NameInfo->Name);

        /* Reset the name string fields */
        NameInfo->Name.Buffer = NULLPTR;
        NameInfo->Name.Length = 0;
        NameInfo->Name.MaximumLength = 0;
    }

    /* Extract the directory pointer and check if it is valid */
    Directory = NameInfo->Directory;
    if(Directory != NULLPTR)
    {
        /* Clear the directory reference */
        NameInfo->Directory = NULLPTR;

        /* Defer the deletion of the directory object */
        DereferenceObjectDeferDelete(Directory);
    }
}

/**
 * Frees the memory allocated for an object and its associated optional headers.
 *
 * @param Object
 *        Supplies a pointer to the object body to be freed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::LifeCycle::FreeObject(IN PVOID Object)
{
    POBJECT_HEADER_HANDLE_INFO HandleInfo;
    POBJECT_HEADER_NAME_INFO NameInfo;
    POBJECT_HEADER ObjectHeader;
    POBJECT_TYPE ObjectType;
    PVOID AllocationBase;

    /* Resolve the object header and object type */
    ObjectHeader = CONTAIN_RECORD(Object, OBJECT_HEADER, Body);
    ObjectType = ObjectHeader->Type;

    /* Calculate the base of the pool allocation */
    AllocationBase = GetObjectAllocationBase(ObjectHeader);

    /* Check if the object type exists */
    if(ObjectType)
    {
        /* Decrement the count of active objects for this type */
        RTL::Atomic::Decrement32((VOLATILE PLONG)&ObjectType->TotalNumberOfObjects);
    }

    /* Check if the object is not fully initialized */
    if(ObjectHeader->Flags & OBJECT_FLAG_NEW_OBJECT)
    {
        /* Check if creation information exists */
        if(ObjectHeader->ObjectCreateInfo)
        {
            /* Free the captured creation parameters */
            FreeObjectCreateInformation((ObjectHeader->ObjectCreateInfo));

            /* Nullify the pointer */
            ObjectHeader->ObjectCreateInfo = NULLPTR;
        }
    }
    else
    {
        /* Fully initialized object, return quotas */
        ReturnObjectQuota(ObjectHeader, ObjectType);
    }

    /* Locate handle information */
    HandleInfo = (POBJECT_HEADER_HANDLE_INFO)(!ObjectHeader->HandleInfoOffset ? NULLPTR : (PCHAR)ObjectHeader -
                                                                                          ObjectHeader->HandleInfoOffset);
    if(HandleInfo && !(ObjectHeader->Flags & OBJECT_FLAG_SINGLE_PROCESS))
    {
        /* Free the handle */
        MM::Allocator::FreePool(HandleInfo->HandleCountDatabase);
    }

    /* Locate name information */
    NameInfo = (POBJECT_HEADER_NAME_INFO)(!ObjectHeader->NameInfoOffset ? NULLPTR : (PCHAR)ObjectHeader -
                                                                                    ObjectHeader->NameInfoOffset);
    if(NameInfo && NameInfo->Name.Buffer)
    {
        /* Free the object name buffer */
        FreeObjectName(&NameInfo->Name);
    }

    /* Mark object type and free memory */
    ObjectHeader->Type = (POBJECT_TYPE)(ULONG_PTR)0xBAD0B0B0;
    MM::Allocator::FreePool(AllocationBase, (ObjectType == NULLPTR ? TAG_OB_OBJECT_TYPE : ObjectType->Key));
}

/**
 * Releases creation information resources and returns them back to the lookaside list.
 *
 * @param ObjectCreateInfo
 *        Supplies a pointer to the object creation information structure to be freed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::LifeCycle::FreeObjectCreateInformation(IN POBJECT_CREATE_INFORMATION CreateInfo)
{
    /* Release the object creation information */
    ReleaseObjectCreateInformation(CreateInfo);

    /* Free the attributes from the lookaside list */
    EX::LookasideList::FreeToPerProcessorList(LookasideCreateInfoList, CreateInfo);
}


/**
 * Frees an object name buffer either to the general pool or a dedicated lookaside list.
 *
 * @param ObjectName
 *        Supplies a pointer to the Unicode string whose buffer is being released.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::LifeCycle::FreeObjectName(IN OUT PUNICODE_STRING ObjectName)
{
    /* Check if the buffer size differs from the lookaside list entry size */
    if(ObjectName->MaximumLength != OBJECT_NAME_BUFFER_SIZE)
    {
        /* Size mismatch, free the allocation to the system pool */
        MM::Allocator::FreePool(ObjectName->Buffer);
    }
    else
    {
        /* Size matches, return the buffer to the lookaside list */
        EX::LookasideList::FreeToPerProcessorList(LookasideNameBufferList, ObjectName->Buffer);
    }
}

/**
 * Resolves the base address of the pool allocation for a given object.
 *
 * @param ObjectHeader
 *        Supplies a pointer to the object header.
 *
 * @return This routine returns the pointer to the start of the memory block.
 *
 * @since XT 1.0
 */
XTAPI
PVOID
OB::LifeCycle::GetObjectAllocationBase(IN POBJECT_HEADER ObjectHeader)
{
    /* Check if the Quota Information header is present */
    if(ObjectHeader->QuotaInfoOffset)
    {
        /* Return the base address including the Quota Information header */
        return (PUCHAR)ObjectHeader - ObjectHeader->QuotaInfoOffset;
    }

    /* Check if the Handle Information header is present */
    if(ObjectHeader->HandleInfoOffset)
    {
        /* Return the base address including the Handle Information header */
        return (PUCHAR)ObjectHeader - ObjectHeader->HandleInfoOffset;
    }

    /* Check if the Name Information header is present */
    if(ObjectHeader->NameInfoOffset)
    {
        /* Return the base address including the Name Information header */
        return (PUCHAR)ObjectHeader - ObjectHeader->NameInfoOffset;
    }

    /* Check if the Creator Information header is present */
    if(ObjectHeader->Flags & OBJECT_FLAG_CREATOR_INFO)
    {
        /* Return the base address including the Creator Information header */
        return (PUCHAR)ObjectHeader - sizeof(OBJECT_HEADER_CREATOR_INFO);
    }

    /* No optional headers exist, return the header address */
    return ObjectHeader;
}

/**
 * Retrieves the optional creator information header for a given object.
 *
 * @param Header
 *        Supplies a pointer to the object's base header.
 *
 * @return This routine returns a pointer to the creator information, or NULLPTR if not present.
 *
 * @since XT 1.0
 */
XTFASTCALL
POBJECT_HEADER_CREATOR_INFO
OB::LifeCycle::GetObjectCreatorInformation(IN POBJECT_HEADER Header)
{
    /* Verify if the creator flag is set */
    if((Header->Flags & OBJECT_FLAG_CREATOR_INFO) == 0)
    {
        /* Creator information not present, return NULL pounter */
        return NULLPTR;
    }

    /* Return the creator information */
    return (POBJECT_HEADER_CREATOR_INFO)((PCHAR)Header - sizeof(OBJECT_HEADER_CREATOR_INFO));
}

/**
 * Retrieves the optional name information header for a given object.
 *
 * @param Header
 *        Supplies a pointer to the object's base header.
 *
 * @return This routine returns a pointer to the name information, or NULLPTR if not present.
 *
 * @since XT 1.0
 */
XTFASTCALL
POBJECT_HEADER_NAME_INFO
OB::LifeCycle::GetObjectNameInformation(IN POBJECT_HEADER Header)
{
    /* Check if the header is present */
    if(!Header->NameInfoOffset)
    {
        /* Name information not present, return NULL pointer */
        return NULLPTR;
    }

    /* Return the name information */
    return (POBJECT_HEADER_NAME_INFO)((PCHAR)Header - Header->NameInfoOffset);
}

/**
 * Initializes the Object lifecycle subsystem.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::LifeCycle::InitializeObjectLifeCycle(VOID)
{
    /* Clear tracking pointer */
    RemoveObjectList = NULLPTR;

    /* Initialize deferred removal work item */
    EX::WorkItem::InitializeWorkItem(&RemoveObjectWorkItem, ProcessDeferredDeletionQueue, NULLPTR);
}

/**
 * Processes the deferred object removal queue.
 *
 * @param Parameter
 *        Supplies an optional context parameter.
 *
 * @return This routine does not return a value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::LifeCycle::ProcessDeferredDeletionQueue(IN PVOID Parameter)
{
    POBJECT_HEADER ObjectHeader;
    POBJECT_HEADER NextObject;
    PVOID OldValue;

    /* Process the deferred removal queue */
    while(TRUE)
    {
        /* Exchange the queue pointer */
        ObjectHeader = (POBJECT_HEADER)RTL::Atomic::ExchangePointer(&RemoveObjectList, OBJECT_REMOVE_QUEUE_LOCKED);

        /* Traverse and process the detached list */
        while(ObjectHeader && ObjectHeader != (POBJECT_HEADER)OBJECT_REMOVE_QUEUE_LOCKED)
        {
            /* Capture the pointer to the next object */
            NextObject = (POBJECT_HEADER)ObjectHeader->NextToFree;

            /* Destroy object */
            DeleteObject(&ObjectHeader->Body, TRUE);

            /* Advance to the next object */
            ObjectHeader = NextObject;
        }

        /* Check if the queue is still locked */
        if(RemoveObjectList == OBJECT_REMOVE_QUEUE_LOCKED)
        {
            /* Attempt to clear the lock */
            OldValue = RTL::Atomic::CompareExchangePointer(&RemoveObjectList, OBJECT_REMOVE_QUEUE_LOCKED, NULLPTR);
            if(OldValue == OBJECT_REMOVE_QUEUE_LOCKED)
            {
                /* Empty list, exit the worker */
                break;
            }
        }
    }
}

/**
 * Increments the pointer reference count of an object.
 *
 * @param Object
 *        Supplies a pointer to the object body to reference.
 *
 * @return This routine returns the updated reference count.
 */
XTFASTCALL
LONG_PTR
OB::LifeCycle::ReferenceObject(IN PVOID Object)
{
    POBJECT_HEADER ObjectHeader;

    /* Resolve the object header */
    ObjectHeader = CONTAIN_RECORD(Object, OBJECT_HEADER, Body);

    /* Increment the reference count */
    return RTL::Atomic::Increment64(&ObjectHeader->PointerCount);
}

/**
 * Increments the pointer reference count of an object by a specified amount.
 *
 * @param Object
 *        Supplies a pointer to the object body to reference.
 *
 * @param Count
 *        Specifies a number of references to acquire.
 *
 * @return This routine returns the updated reference count.
 */
XTFASTCALL
LONG_PTR
OB::LifeCycle::ReferenceObject(IN PVOID Object,
                               IN ULONG Count)
{
    POBJECT_HEADER ObjectHeader;

    /* Resolve the object header */
    ObjectHeader = CONTAIN_RECORD(Object, OBJECT_HEADER, Body);

    /* Increment the reference count */
    return RTL::Atomic::ExchangeAdd64(&ObjectHeader->PointerCount, Count) + Count;
}

/**
 * Increments the pointer reference count on an object provided by a handle.
 *
 * @param Handle
 *        Supplies the handle to the object being referenced.
 *
 * @param AccessMask
 *        Supplies the access mask representing the requested access rights.
 *
 * @param ObjectType
 *        Optionally supplies a pointer to the expected object type to validate against.
 *
 * @param ProcessorMode
 *        Supplies the processor mode from which the handle is being accessed.
 *
 * @param Object
 *        Receives a pointer to the referenced object body upon success.
 *
 * @param HandleInformation
 *        Supplies an optional pointer that receives information regarding the handle's granted access and attributes.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::LifeCycle::ReferenceObject(IN HANDLE Handle,
                               IN ACCESS_MASK AccessMask,
                               IN POBJECT_TYPE ObjectType,
                               IN KPROCESSOR_MODE ProcessorMode,
                               OUT PVOID *Object,
                               OUT POBJECT_HANDLE_INFORMATION HandleInformation)
{
    PHANDLE_TABLE_ENTRY ObjectTableEntry;
    PHANDLE_TABLE_ENTRY_INFO ObjectInfo;
    POBJECT_HEADER ObjectHeader;
    PHANDLE_TABLE HandleTable;
    ACCESS_MASK GrantedAccess;
    POBJECT_TYPE PseudoType;
    PVOID PseudoObject;
    PEPROCESS Process;
    PETHREAD Thread;
    XTSTATUS Status;

    /* Initialize the output pointer */
    *Object = NULLPTR;

    /* Retrieve the current execution context */
    Thread = PS::Thread::GetCurrentThread();
    Process = PS::Process::GetCurrentProcess();

    /* Determine if the handle is a special kernel handle or a pseudohandle */
    if((LONG)(ULONG_PTR)Handle < 0)
    {
        /* Check if the handle points to the current process or thread */
        if(Handle == PS_CURRENT_PROCESS_HANDLE || Handle == PS_CURRENT_THREAD_HANDLE)
        {
            /* Check if the handle points to the current process */
            if(Handle == PS_CURRENT_PROCESS_HANDLE)
            {
                /* Bind the execution context to the current process pseudohandle */
                PseudoObject = (PVOID)Process;
                PseudoType = PS::ProcessManager::GetProcessType();
                GrantedAccess = Process->GrantedAccess;
            }
            else
            {
                /* Bind the execution context to the current thread pseudohandle */
                PseudoObject = (PVOID)Thread;
                PseudoType = PS::ProcessManager::GetThreadType();
                GrantedAccess = Thread->GrantedAccess;
            }

            /* Validate the object type */
            if(ObjectType && ObjectType != PseudoType)
            {
                /* Object type does not match the pseudohandle type, return error code */
                return STATUS_OBJECT_TYPE_MISMATCH;
            }

            /* Enforce security constraints for user-mode requests */
            if(ProcessorMode != KernelMode && SE::Access::ComputeDeniedAccesses(GrantedAccess, AccessMask))
            {
                /* The caller lacks the requested access rights, return error code */
                return STATUS_ACCESS_DENIED;
            }

            /* Resolve the object header */
            ObjectHeader = CONTAIN_RECORD(PseudoObject, OBJECT_HEADER, Body);

            /* Increment the reference count */
            RTL::Atomic::Increment64(&ObjectHeader->PointerCount);

            /* Check if handle information is requested */
            if(HandleInformation)
            {
                /* Populate the optional handle information descriptor */
                HandleInformation->GrantedAccess = GrantedAccess;
                HandleInformation->HandleAttributes = 0;
            }

            /* Assign the object pointer and return */
            *Object = PseudoObject;
            return STATUS_SUCCESS;
        }

        /* Check if request comes from a user-mode */
        if(ProcessorMode != KernelMode)
        {
            /* Reject user-mode attempt, return error code */
            return STATUS_INVALID_HANDLE;
        }

        /* Decode the kernel handle and select the system handle table */
        Handle = OB::Handle::DecodeKernelHandle(Handle);
        HandleTable = KernelHandleTable;
    }
    else
    {
        /* Use the process handle table */
        HandleTable = Process->ObjectTable;
    }

    /* Enter a critical region */
    KE::CriticalRegionGuard CriticalRegion(&Thread->ThreadControlBlock);

    /* Map the given handle to its underlying physical table entry */
    ObjectTableEntry = EX::Handle::MapHandleToPointer(HandleTable, Handle);
    if(!ObjectTableEntry)
    {
        /* The handle is invalid or closed, return error code */
        return STATUS_INVALID_HANDLE;
    }

    /* Resolve the object header */
    ObjectHeader = (POBJECT_HEADER)(((ULONG_PTR)ObjectTableEntry->Object) & ~OBJECT_HANDLE_ATTRIBUTES);

    /* Verify object type consistency */
    if(ObjectType && ObjectHeader->Type != ObjectType)
    {
        /* Set status code */
        Status = STATUS_OBJECT_TYPE_MISMATCH;
    }
    else
    {
        /* Decode the granted access mask */
        GrantedAccess = ((ObjectTableEntry->GrantedAccess) & ~OBJECT_ACCESS_PROTECT_CLOSE_BIT);

        /* Verify if the caller possesses the requested access rights */
        if(ProcessorMode != KernelMode && SE::Access::ComputeDeniedAccesses(GrantedAccess, AccessMask))
        {
            /* Set status code */
            Status = STATUS_ACCESS_DENIED;
        }
        else
        {
            /* Check if handle information is requested */
            if(HandleInformation)
            {
                /* Populate the optional handle information descriptor */
                HandleInformation->GrantedAccess = GrantedAccess;
                HandleInformation->HandleAttributes = OB::Handle::GetHandleAttributes(ObjectTableEntry);
            }

            /* Check if audit is required */
            if((ObjectTableEntry->ObAttributes & OBJECT_AUDIT_OBJECT_CLOSE) && AccessMask && ProcessorMode != KernelMode)
            {
                /* Fetch the handle information */
                ObjectInfo = EX::Handle::GetHandleInformation(HandleTable, Handle, TRUE);
                if(ObjectInfo && ObjectInfo->AuditMask)
                {
                    /* Generate a security audit event */
                    OB::Security::AuditObjectAccess(Handle, ObjectInfo, &ObjectHeader->Type->Name, AccessMask);
                }
            }

            /* Increment the reference count */
            RTL::Atomic::Increment64(&ObjectHeader->PointerCount);

            /* Assign the object pointer and mark the operation as successful */
            *Object = &ObjectHeader->Body;
            Status = STATUS_SUCCESS;
        }
    }

    /* Release the handle table */
    EX::Handle::UnlockHandleTableEntry(HandleTable, ObjectTableEntry);

    /* Return status code */
    return Status;
}

/**
 * Increments the query reference count of a NameInfo structure.
 *
 * @param ObjectHeader
 *        Supplies a pointer to the object header associated with the NameInfo.
 *
 * @return This routine returns a pointer to the referenced NameInfo, or NULLPTR if the reference could not be acquired.
 *
 * @since XT 1.0
 */
XTFASTCALL
POBJECT_HEADER_NAME_INFO
OB::LifeCycle::ReferenceObjectNameInformation(IN POBJECT_HEADER ObjectHeader)
{
    LONG OldReferences, NewReferences, ActualReferences;
    POBJECT_HEADER_NAME_INFO NameInfo;

    /* Resolve the name information structure from the object header */
    NameInfo = GetObjectNameInformation(ObjectHeader);

    /* Ensure the name info structure is present */
    if(!NameInfo)
    {
        /* Return NULL pointer */
        return NULLPTR;
    }

    /* Initialize the old reference count snapshot */
    OldReferences = *(volatile LONG*)&NameInfo->QueryReferences;

    /* Enter the CAS loop */
    while(TRUE)
    {
        /* Check if the reference count has dropped to zero */
        if((OldReferences & ~OBJECT_NAME_LOCKED) == 0)
        {
            /* Abort and return NULL pointer */
            return NULLPTR;
        }

        /* Calculate the incremented reference count */
        NewReferences = OldReferences + 1;

        /* Attempt to commit the incremented count */
        ActualReferences = RTL::Atomic::CompareExchange32((PLONG)&NameInfo->QueryReferences, OldReferences, NewReferences);
        if(ActualReferences == OldReferences)
        {
            /* Break the loop */
            break;
        }

        /* State changed, update the snapshot and retry */
        OldReferences = ActualReferences;
    }

    /* Check if the name is currently locked */
    if(OldReferences & OBJECT_NAME_LOCKED)
    {
        /* Synchronize with the directory lock */
        KE::PushLock::WaitOnPushLock(&NameInfo->Directory->Lock);
    }

    /* Return the referenced name info */
    return NameInfo;
}

/**
 * Releases security resources associated with an object creation information structure.
 *
 * @param CreateInfo
 *        Supplies a pointer to the object creation information structure to be released.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::LifeCycle::ReleaseObjectCreateInformation(IN POBJECT_CREATE_INFORMATION CreateInfo)
{
    /* Verify if a security descriptor was captured and is actively attached */
    if(CreateInfo->SecurityDescriptor)
    {
        /* Release the descriptor */
        SE::Descriptor::ReleaseSecurityDescriptor(CreateInfo->SecurityDescriptor, CreateInfo->ProbeMode, TRUE);

        /* Nullify the pointer */
        CreateInfo->SecurityDescriptor = NULLPTR;
    }
}

/**
 * Processes and returns pool quotas associated with an object's execution block.
 *
 * @param ObjectHeader
 *        Supplies a pointer to the object header.
 *
 * @param ObjectType
 *        Supplies a pointer to the object type definition.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::LifeCycle::ReturnObjectQuota(IN POBJECT_HEADER Header,
                                 IN POBJECT_TYPE ObjectType)
{
    POBJECT_HEADER_QUOTA_INFO Quota;
    ULONG NonPagedPoolCharge;
    ULONG PagedPoolCharge;

    /* Verify if a quota block is charged for the object */
    if(!Header->QuotaBlockCharged)
    {
        /* No quota charged, nothing to do */
        return;
    }

    /* Retrieve the quota information header */
    Quota = (POBJECT_HEADER_QUOTA_INFO)(Header->QuotaInfoOffset == 0 ? NULLPTR : ((PCHAR)Header - Header->QuotaInfoOffset));
    if(Quota)
    {
        /* Extract the pool charges */
        NonPagedPoolCharge = Quota->NonPagedPoolCharge;
        PagedPoolCharge = Quota->PagedPoolCharge + Quota->SecurityDescriptorCharge;
    }
    else
    {
        /* No quota header present, use default charges */
        NonPagedPoolCharge = ObjectType->TypeInfo.DefaultNonPagedPoolCharge;
        PagedPoolCharge = ObjectType->TypeInfo.DefaultPagedPoolCharge;

        /* Check if the object holds a default security descriptor quota */
        if(Header->Flags & OBJECT_FLAG_SECURITY_QUOTA)
        {
            /* Add the default security quota to the paged pool charge */
            PagedPoolCharge += SE_DEFAULT_SECURITY_QUOTA;
        }
    }

    /* Return the calculated pool charges */
    PS::Quota::ReturnSharedPoolQuota((PEPROCESS_QUOTA_BLOCK)Header->QuotaBlockCharged,
                                     PagedPoolCharge, NonPagedPoolCharge);

    /* Nullify the quota block pointer */
    Header->QuotaBlockCharged = NULLPTR;
}
