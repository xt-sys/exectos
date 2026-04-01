/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/kdfuncs.h
 * DESCRIPTION:     XTOS kernel debugger routine definitions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTDK_KDFUNCS_H
#define __XTDK_KDFUNCS_H

#include <xtdefs.h>
#include <xttypes.h>


/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* Kernel debugger routines forward references */
XTCLINK
XTCDECL
VOID
DbgPrint(PCWSTR Format,
         ...);

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_KDFUNCS_H */
