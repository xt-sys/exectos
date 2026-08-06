/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ob/obmgr.cc
 * DESCRIPTION:     Object Manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Initializes an object attributes structure.
 *
 * @param ObjectAttributes
 *        Supplies a pointer to the object attributes structure to be initialized.
 *
 * @param ObjectName
 *        Supplies an optional pointer to a Unicode string representing the object name.
 *
 * @param Attributes
 *        Supplies a bitmask of flags controlling object behavior.
 *
 * @param Directory
 *        Supplies an optional handle to the root directory for relative path parsing.
 *
 * @param SecurityDescriptor
 *        Supplies an optional pointer to a security descriptor for the object.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
OB::Manager::InitializeObjectAttributes(OUT POBJECT_ATTRIBUTES ObjectAttributes,
                                        IN PUNICODE_STRING ObjectName,
                                        IN ULONG Attributes,
                                        IN HANDLE Directory,
                                        IN PVOID SecurityDescriptor)
{
    /* Initialize the object attributes structure */
    ObjectAttributes->Attributes = Attributes;
    ObjectAttributes->Length = sizeof(OBJECT_ATTRIBUTES);
    ObjectAttributes->ObjectName = ObjectName;
    ObjectAttributes->RootDirectory = Directory;
    ObjectAttributes->SecurityDescriptor = SecurityDescriptor;
    ObjectAttributes->SecurityQualityOfService = NULLPTR;
}

/**
 * Initializes the Object Manager subsystem.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
OB::Manager::InitializeObjectManager(VOID)
{
    XTSTATUS Status;

    /* Zero out the static global lookaside structures */
    RTL::Memory::ZeroMemory(&CreateInfoList, sizeof(GENERAL_LOOKASIDE));
    RTL::Memory::ZeroMemory(&NameBufferList, sizeof(GENERAL_LOOKASIDE));

    /* Initialize the system-wide global lookaside list for object create information structures */
    EX::LookasideList::InitializeLookasideList(&CreateInfoList, NonPagedPool,
                                               sizeof(OBJECT_CREATE_INFORMATION), TAG_OB_CREATE_INFO,
                                               64, EX::LookasideList::GetSystemLookasideListHead());

    /* Initialize the system-wide global lookaside list for object name buffers */
    EX::LookasideList::InitializeLookasideList(&NameBufferList, PagedPool,
                                               OBJECT_NAME_BUFFER_SIZE, TAG_OB_NAME,
                                               32, EX::LookasideList::GetSystemLookasideListHead());

    /* Initialize system lookaside list for BSP */
    InitializeSystemLookasideList();

    /* Initialize handle table support */
    OB::HandleTable::InitializeHandleTable();

    /* Initialize device map support */
    OB::DeviceMap::InitializeDeviceMap();

    /* Initialize object life cycle support */
    OB::LifeCycle::InitializeObjectLifeCycle();

    /* Initialize object type registry */
    Status = OB::TypeRegistry::InitializeObjectTypeRegistry();
    if(Status != STATUS_SUCCESS)
    {
        /* Initialization failed, return error code */
        return Status;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Initializes the per-processor lookaside lists for the Object Manager.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
OB::Manager::InitializeSystemLookasideList(VOID)
{
    PKPROCESSOR_CONTROL_BLOCK Prcb;
    PGENERAL_LOOKASIDE LookasideList;
    XTSTATUS Status;

    /* Retrieve the Processor Control Block */
    Prcb = KE::Processor::GetCurrentProcessorControlBlock();

    /* Allocate a contiguous memory block for lookaside lists */
    Status = MM::Allocator::AllocatePool(NonPagedPool, sizeof(GENERAL_LOOKASIDE) * 2,
                                         (PVOID*)&LookasideList, TAG_OB_CREATE_INFO);

    /* Wire the global pointer for create information structures */
    Prcb->LookasideList[LookasideCreateInfoList].Global = &CreateInfoList;

    /* Check if the contiguous allocation was successful */
    if(Status == STATUS_SUCCESS && LookasideList)
    {
        /* Map the pointer to the first half of the contiguous block */
        Prcb->LookasideList[LookasideCreateInfoList].Local = &LookasideList[0];

        /* Initialize the local list */
        EX::LookasideList::InitializeLookasideList(&LookasideList[0], NonPagedPool,
                                                   sizeof(OBJECT_CREATE_INFORMATION), TAG_OB_CREATE_INFO,
                                                   64, EX::LookasideList::GetSystemLookasideListHead());
    }
    else
    {
        /* Allocation failed, use shared global list */
        Prcb->LookasideList[LookasideCreateInfoList].Local = &CreateInfoList;
    }

    /* Wire the global pointer for object name buffers */
    Prcb->LookasideList[LookasideNameBufferList].Global = &NameBufferList;

    /* Check if the contiguous allocation was successful */
    if(Status == STATUS_SUCCESS && LookasideList)
    {
        /* Map the pointer to the second half of the contiguous block */
        Prcb->LookasideList[LookasideNameBufferList].Local = &LookasideList[1];

        /* Initialize the local list */
        EX::LookasideList::InitializeLookasideList(&LookasideList[1], PagedPool,
                                                   OBJECT_NAME_BUFFER_SIZE, TAG_OB_NAME,
                                                   32, EX::LookasideList::GetSystemLookasideListHead());
    }
    else
    {
        /* Allocation failed, use shared global list */
        Prcb->LookasideList[LookasideNameBufferList].Local = &NameBufferList;
    }
}
