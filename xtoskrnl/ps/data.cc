/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/po/data.cc
 * DESCRIPTION:     Process global and static data
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/* Pointer to the system process object */
PEPROCESS PS::Process::SystemProcess;

/* Global object type descriptor for Process objects */
POBJECT_TYPE PS::ProcessManager::ProcessType;

/* Global object type descriptor for Thread objects */
POBJECT_TYPE PS::ProcessManager::ThreadType;

/* Default system-wide quota block assigned to processes */
EPROCESS_QUOTA_BLOCK PS::Quota::DefaultQuotaBlock;

/* Lock used for quota management */
KSPIN_LOCK PS::Quota::QuotaLock;
