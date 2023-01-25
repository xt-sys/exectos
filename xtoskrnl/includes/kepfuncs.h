/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/kefuncs.h
 * DESCRIPTION:     Private routine definitions for kernel services
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_KEFUNCS_H
#define __XTOSKRNL_KEFUNCS_H

#include <xtos.h>


XTAPI
VOID
KepStartKernel(VOID);

XTAPI
VOID
KepInitializeStack(IN PKERNEL_INITIALIZATION_BLOCK Parameters);

XTAPI
VOID
KepSwitchBootStack(IN ULONG_PTR Stack);

#endif /* __XTOSKRNL_KEFUNCS_H */
