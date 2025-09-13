/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/data.cc
 * DESCRIPTION:     Kernel Library global and static data
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/* Kernel initialization block passed by boot loader */
PKERNEL_INITIALIZATION_BLOCK KE::BootInformation::InitializationBlock = {};

/* Kernel boot resources list */
LIST_ENTRY KE::SystemResources::ResourcesListHead;

/* Kernel boot resources lock */
KSPIN_LOCK KE::SystemResources::ResourcesLock;

/* Kernel initial process */
EPROCESS KE::KProcess::InitialProcess;

/* Kernel initial thread */
ETHREAD KE::KThread::InitialThread = {};

/* Kernel UBSAN active frame flag */
BOOLEAN KE::KUbsan::ActiveFrame = FALSE;
