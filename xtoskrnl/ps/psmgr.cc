/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ps/psmgr.cc
 * DESCRIPTION:     Process Manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Retrieves the global object type descriptor for Process objects.
 *
 * @return This routine returns a pointer to the process object type structure.
 *
 * @since XT 1.0
 */
XTFASTCALL
POBJECT_TYPE
PS::ProcessManager::GetProcessType(VOID)
{
    /* Return the process object type descriptor */
    return ProcessType;
}

/**
 * Retrieves the global object type descriptor for Thread objects.
 *
 * @return This routine returns a pointer to the thread object type structure.
 *
 * @since XT 1.0
 */
XTFASTCALL
POBJECT_TYPE
PS::ProcessManager::GetThreadType(VOID)
{
    /* Return the thread object type descriptor */
    return ThreadType;
}

/**
 * Initializes the Process Manager subsystem.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
PS::ProcessManager::InitializeProcessManager(VOID)
{
    OBJECT_TYPE_INITIALIZER ObjectTypeInitializer;
    UNICODE_STRING ObjectTypeName;

    /* Clear the object type initializer */
    RTL::Memory::ZeroMemory(&ObjectTypeInitializer, sizeof(ObjectTypeInitializer));

    /* Initialize common attributes */
    ObjectTypeInitializer.InvalidAttributes = OBJECT_EXCLUSIVE | OBJECT_OPENIF | OBJECT_PERMANENT;
    ObjectTypeInitializer.Length = sizeof(ObjectTypeInitializer);
    ObjectTypeInitializer.PoolType = NonPagedPool;
    ObjectTypeInitializer.SecurityRequired = TRUE;

    /* Initialize the Process object type */
    RTL::Unicode::InitializeString(&ObjectTypeName, L"Process");
    ObjectTypeInitializer.DefaultNonPagedPoolCharge = sizeof(EPROCESS);
    ObjectTypeInitializer.DeleteProcedure = PS::Process::DeleteProcess;
    ObjectTypeInitializer.GenericMapping = PS_PROCESS_GENERIC_MAPPING;
    ObjectTypeInitializer.ValidAccessMask = SE_PROCESS_ALL_ACCESS;
    OB::TypeRegistry::CreateObjectType(&ObjectTypeName, &ObjectTypeInitializer, NULLPTR, &ProcessType);

    /*  Initialize the Thread object type */
    RTL::Unicode::InitializeString(&ObjectTypeName, L"Thread");
    ObjectTypeInitializer.DefaultNonPagedPoolCharge = sizeof(ETHREAD);
    ObjectTypeInitializer.DeleteProcedure = PS::Thread::DeleteThread;
    ObjectTypeInitializer.GenericMapping = PS_THREAD_GENERIC_MAPPING;
    ObjectTypeInitializer.ValidAccessMask = SE_THREAD_ALL_ACCESS;
    OB::TypeRegistry::CreateObjectType(&ObjectTypeName, &ObjectTypeInitializer, NULLPTR, &ThreadType);

    /* Initialize system quota */
    PS::Quota::InitializeQuota();

    /* Create system process */
    PS::Process::CreateInitialSystemProcess();

    /* Initialize IDLE process */
    PS::Process::InitializeIdleProcess();
}
