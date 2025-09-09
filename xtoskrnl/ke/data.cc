/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/data.cc
 * DESCRIPTION:     
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/* Kernel Library */
namespace KE
{

/* Kernel initialization block passed by boot loader */
PKERNEL_INITIALIZATION_BLOCK BootInformation::InitializationBlock = {};

/* Kernel boot resources list */
LIST_ENTRY SystemResources::ResourcesListHead;

/* Kernel boot resources lock */
KSPIN_LOCK SystemResources::ResourcesLock;

/* Kernel initial process */
EPROCESS KProcess::InitialProcess;

/* Kernel initial thread */
ETHREAD KThread::InitialThread = {};

/* Kernel UBSAN active frame flag */
BOOLEAN KUbsan::ActiveFrame = FALSE;

} /* namespace */
