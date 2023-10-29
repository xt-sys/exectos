/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/i686/rtl.h
 * DESCRIPTION:     XT runtime library routines specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_I686_RTL_H
#define __XTOSKRNL_I686_RTL_H

#include <xtos.h>


/* Runtime Library routines specific to I686 forward references */
XTAPI
VOID
RtlGetStackLimits(OUT PULONG_PTR StackBase,
                  OUT PULONG_PTR StackLimit);

#endif /* __XTOSKRNL_I686_RTL_H */
