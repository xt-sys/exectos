/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm.h
 * DESCRIPTION:     Memory manager routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_MM_H
#define __XTOSKRNL_MM_H

#include <xtos.h>


/* Memory Manager routines forward references */
XTAPI
XTSTATUS
MmAllocateKernelStack(IN PVOID *Stack,
                      IN BOOLEAN LargeStack,
                      IN UCHAR SystemNode);

XTAPI
VOID
MmFreeKernelStack(IN PVOID Stack,
                  IN BOOLEAN LargeStack);

#endif /* __XTOSKRNL_MM_H */
