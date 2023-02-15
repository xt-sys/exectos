/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/rtlfuncs.h
 * DESCRIPTION:     XT runtime library routines specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_RTLFUNCS_H
#define __XTDK_I686_RTLFUNCS_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>


XTAPI
VOID
RtlGetStackLimits(OUT PULONG_PTR StackBase,
                  OUT PULONG_PTR StackLimit);

#endif /* __XTDK_I686_RTLFUNCS_H */
