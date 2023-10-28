/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/arfuncs.h
 * DESCRIPTION:     I686 architecture library routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_ARFUNCS_H
#define __XTDK_I686_ARFUNCS_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>
#include <i686/xtstruct.h>


/* Routines used by XTLDR */
XTCDECL
VOID
ArClearInterruptFlag();

XTCDECL
VOID
ArHalt();

XTCDECL
VOID
ArWriteControlRegister(IN USHORT ControlRegister,
                       IN UINT_PTR Value);

#endif /* __XTDK_I686_ARFUNCS_H */
