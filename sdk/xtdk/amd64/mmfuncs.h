/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/mmfuncs.h
 * DESCRIPTION:     Memory manager routines specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_MMFUNCS_H
#define __XTDK_AMD64_MMFUNCS_H

#include <xtbase.h>


XTFASTCALL
VOID
MmZeroPages(IN PVOID Address,
            IN ULONG Size);

#endif /* __XTDK_AMD64_MMFUNCS_H */
