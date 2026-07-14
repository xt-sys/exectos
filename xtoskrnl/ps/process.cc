/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/po/process.cc
 * DESCRIPTION:     Process Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Creates the global IDLE process.
 *
 * @param Prcb
 *        Supplies a pointer to the Processor Control Block of the Bootstrap Processor.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
PS::Process::CreateIdleProcess(IN PKPROCESSOR_CONTROL_BLOCK Prcb)
{
    ULONG_PTR PageDirectory[2];
    PKPROCESS IdleProcess;
    PKTHREAD IdleThread;
    XTSTATUS Status;

    /* Get initial IDLE thread */
    IdleThread = Prcb->CurrentThread;

    /* Get initial IDLE process */
    IdleProcess = IdleThread->ApcState.Process;

    /* Setup placeholder for page directory entries */
    PageDirectory[0] = 0;
    PageDirectory[1] = 0;

    /* Initialize IDLE process */
    Status = KE::KProcess::InitializeIdleProcess(IdleProcess, PageDirectory);
    if(Status != STATUS_SUCCESS)
    {
        /* Failed to initialize IDLE process, return status code */
        return Status;
    }

    /* Initialize IDLE thread */
    return KE::KThread::InitializeIdleThread(IdleProcess, IdleThread, Prcb, AR::ProcessorSupport::GetBootStack());
}

/**
 * Creates and initializes the primary system process.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
PS::Process::CreateInitialSystemProcess(VOID)
{
    OBJECT_ATTRIBUTES Attributes;
    XTSTATUS Status;

    /* Initialize process object attributes */
    OB::Manager::InitializeObjectAttributes(&Attributes, NULLPTR, 0, NULLPTR, NULLPTR);

    /* Spawn the root system process */
    Status = CreateKernelProcess(&SystemProcessHandle, SE_PROCESS_ALL_ACCESS, &Attributes,
                                 NULLPTR, 0, NULLPTR, NULLPTR, NULLPTR, FALSE);
    if(Status != STATUS_SUCCESS)
    {
        /* Process creation failed, return error code */
        return Status;
    }

    /* Reference the process handle for a persistent kernel object pointer */
    Status = OB::LifeCycle::ReferenceObject(SystemProcessHandle, 0L, PS::ProcessManager::GetProcessType(),
                                            KernelMode, (PVOID *)&SystemProcess, NULLPTR);
    if(Status != STATUS_SUCCESS)
    {
        /* Handle reference failed, return error code */
        return Status;
    }

    /* Set the diagnostic image file name */
    RTL::String::CopyString(&SystemProcess->ImageFileName[0], "System", 6);

    /* Allocate memory for the process creation audit information */
    Status = MM::Allocator::AllocatePool(PagedPool, sizeof(OBJECT_NAME_INFORMATION),
                                         (PVOID *)&SystemProcess->AuditProcessCreationInfo.ImageFileName,
                                         TAG_SE_PRIVILEGE_ARRAY);
    if(Status != STATUS_SUCCESS || !SystemProcess->AuditProcessCreationInfo.ImageFileName)
    {
        /* Memory allocation failed, return error code */
        return Status;
    }

    /* Zero the allocated audit information */
    RtlZeroMemory(SystemProcess->AuditProcessCreationInfo.ImageFileName, sizeof(OBJECT_NAME_INFORMATION));

    /* Return success */
    return STATUS_SUCCESS;
}


/**
 * Creates, configures, and publishes a new kernel process object.
 *
 * @param ProcessHandle
 *        Receives the handle to the newly created process.
 *
 * @param AccessMask
 *        Supplies the desired access rights to be granted.
 *
 * @param Attributes
 *        Supplies an optional pointer to the object attributes for process namespace insertion.
 *
 * @param ParentProcess
 *        Supplies an optional handle to the parent process.
 *
 * @param Flags
 *        Supplies process creation behavioral flags.
 *
 * @param SectionHandle
 *        Supplies an optional handle to the backed file mapping section.
 *
 * @param DebugPort
 *        Supplies an optional handle to the debugging registration port.
 *
 * @param ExceptionPort
 *        Supplies an optional handle to the IPC exception port.
 *
 * @param JobMember
 *        Specifies if the process is part of a job hierarchy.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
PS::Process::CreateKernelProcess(OUT PHANDLE ProcessHandle,
                                 IN ACCESS_MASK AccessMask,
                                 IN POBJECT_ATTRIBUTES Attributes,
                                 IN HANDLE ParentProcess,
                                 IN ULONG Flags,
                                 IN HANDLE SectionHandle,
                                 IN HANDLE DebugPort,
                                 IN HANDLE ExceptionPort,
                                 IN BOOLEAN JobMember)
{
    UNIMPLEMENTED;

    /* Not implemented */
    return STATUS_NOT_IMPLEMENTED;
}

/**
 * Creates a new process as a child of the main system process.
 *
 * @param ProcessHandle
 *        Receives the handle to the newly created process.
 *
 * @param AccessMask
 *        Supplies the desired access rights to be granted.
 *
 * @param Attributes
 *        Supplies an optional pointer to the object attributes for process namespace insertion.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
PS::Process::CreateSystemProcess(OUT PHANDLE ProcessHandle,
                                 IN ACCESS_MASK AccessMask,
                                 IN POBJECT_ATTRIBUTES Attributes)
{
    /* Create new process */
    return CreateKernelProcess(ProcessHandle, AccessMask, Attributes, SystemProcessHandle,
                               0, NULLPTR, NULLPTR, NULLPTR, FALSE);
}

/**
 * Deletes a Process object when the final reference is released.
 *
 * @param ProcessObject
 *        Supplies a pointer to the terminating process object.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
PS::Process::DeleteProcess(IN PVOID ProcessObject)
{
    UNIMPLEMENTED;
}

/**
 * Returns a pointer to the process object associated with the currently executing thread.
 *
 * @return This routine returns a pointer to the current process object.
 *
 * @since XT 1.0
 */
XTFASTCALL
PEPROCESS
PS::Process::GetCurrentProcess(VOID)
{
    /* Return the current process */
    return (PEPROCESS)KE::Processor::GetCurrentThread()->ApcState.Process;
}

/**
 * Returns a pointer to the primary system process object.
 *
 * @return This routine returns a pointer to the system process object.
 *
 * @since XT 1.0
 */
XTFASTCALL
PEPROCESS
PS::Process::GetSystemProcess(VOID)
{
    /* Return the system process */
    return SystemProcess;
}

/**
 * Initializes the system idle process.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
PS::Process::InitializeIdleProcess(VOID)
{
    /* Bind the currently executing bootstrap process as the system idle process */
    IdleProcess = GetCurrentProcess();

    /* Initialize the process lock and rundown protection */
    KE::PushLock::InitializePushLock(&IdleProcess->ProcessLock);
    EX::Rundown::InitializeProtection(&IdleProcess->RundownProtect);

    /* Prepare the process thread tracking list */
    RTL::LinkedList::InitializeListHead(&IdleProcess->ThreadListHead);

    /* Set the diagnostic image file name */
    RTL::String::CopyString(&IdleProcess->ImageFileName[0], "Idle", 4);

    /* Reset the accounting metric */
    IdleProcess->ProcessControlBlock.KernelTime = 0;
}
