/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/mmfuncs.h
 * DESCRIPTION:     Memory manager routines specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_MMFUNCS_H
#define __XTDK_I686_MMFUNCS_H

#include <xtbase.h>


XTFASTCALL
VOID
MmZeroPages(IN PVOID Address,
            IN ULONG Size);

#endif /* __XTDK_I686_MMFUNCS_H */
