/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/arfuncs.h
 * DESCRIPTION:     AMD64 architecture library routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_ARFUNCS_H
#define __XTDK_AMD64_ARFUNCS_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>
#include <amd64/xtstruct.h>


/* Routines used by XTLDR */
XTCLINK
XTCDECL
VOID
ArClearInterruptFlag(VOID);

XTCLINK
XTCDECL
BOOLEAN
ArCpuId(IN OUT PCPUID_REGISTERS Registers);

XTCLINK
XTCDECL
VOID
ArEnableExtendedPhysicalAddressing(IN ULONG_PTR PageMap);

XTCLINK
XTCDECL
VOID
ArHalt(VOID);

XTCLINK
XTCDECL
ULONG_PTR
ArReadControlRegister(IN USHORT ControlRegister);

XTCLINK
XTCDECL
ULONGLONG
ArReadModelSpecificRegister(IN ULONG Register);

XTCLINK
XTCDECL
VOID
ArWriteControlRegister(IN USHORT ControlRegister,
                       IN UINT_PTR Value);

#endif /* __XTDK_AMD64_ARFUNCS_H */
