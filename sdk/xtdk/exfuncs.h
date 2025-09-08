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
XTCLINK
XTFASTCALL
BOOLEAN
ExAcquireRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);

XTCLINK
XTFASTCALL
VOID
ExCompleteRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);

XTCLINK
XTFASTCALL
VOID
ExInitializeRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);

XTCLINK
XTFASTCALL
VOID
ExReInitializeRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);

XTCLINK
XTFASTCALL
VOID
ExReleaseRundownProtection(IN PEX_RUNDOWN_REFERENCE Descriptor);

XTCLINK
XTFASTCALL
VOID
ExWaitForRundownProtectionRelease(IN PEX_RUNDOWN_REFERENCE Descriptor);

#endif /* __XTDK_EXFUNCS_H */
