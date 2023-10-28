/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/i686/mm.h
 * DESCRIPTION:     XT memory manager routines specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_I686_MM_H
#define __XTOSKRNL_I686_MM_H

#include <xtos.h>


XTFASTCALL
VOID
MmZeroPages(IN PVOID Address,
            IN ULONG Size);

#endif /* __XTOSKRNL_I686_MM_H */
