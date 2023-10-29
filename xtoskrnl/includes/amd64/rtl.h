/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/amd64/rtl.h
 * DESCRIPTION:     XT runtime library routines specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_AMD64_RTL_H
#define __XTOSKRNL_AMD64_RTL_H

#include <xtos.h>


/* Runtime Library routines specific to AMD64 forward references */
XTAPI
VOID
RtlGetStackLimits(OUT PULONG_PTR StackBase,
                  OUT PULONG_PTR StackLimit);

#endif /* __XTOSKRNL_AMD64_RTL_H */
