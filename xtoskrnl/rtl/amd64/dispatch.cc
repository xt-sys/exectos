/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/amd64/dispatch.cc
 * DESCRIPTION:     Dispatching support for AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Returns the stack limits for the current thread.
 *
 * @param StackBase
 *        Supplies a pointer to memory area, where the stack base will be stored.
 *
 * @param StackLimit
 *        Suppliws a pointer to memory area, where the stack limit will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::Dispatcher::GetStackLimits(OUT PULONG_PTR StackBase,
                                OUT PULONG_PTR StackLimit)
{
    PKTHREAD Thread = KeGetCurrentThread();
    *StackBase = (ULONG_PTR)Thread->StackBase;
    *StackLimit = (ULONG_PTR)Thread->StackLimit;
}
