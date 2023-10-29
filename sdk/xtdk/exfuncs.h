/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/exfuncs.h
 * DESCRIPTION:     Kernel executive routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_EXFUNCS_H
#define __XTDK_EXFUNCS_H

#include <xtbase.h>
#include <xttypes.h>


/* Kernel Executive routines forward references */
XTFASTCALL
BOOLEAN
ExAcquireRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);

XTFASTCALL
VOID
ExInitializeRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);

XTFASTCALL
VOID
ExReleaseRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);

#endif /* __XTDK_EXFUNCS_H */
