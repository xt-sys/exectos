/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/po/psmgr.cc
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
