/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/hlfuncs.h
 * DESCRIPTION:     XT hardware abstraction layer routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_HLFUNCS_H
#define __XTDK_HLFUNCS_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>


/* Routines used by XTLDR */
XTCLINK
XTCDECL
XTSTATUS
HlWriteComPort(IN PCPPORT Port,
               IN UCHAR Byte);

XTCLINK
XTCDECL
XTSTATUS
HlInitializeComPort(IN OUT PCPPORT Port,
                    IN PUCHAR PortAddress,
                    IN ULONG BaudRate);

/* Hardware layer routines forward references */
XTCLINK
XTAPI
UCHAR
HlReadRegister8(IN PVOID Register);

XTCLINK
XTAPI
USHORT
HlReadRegister16(IN PVOID Register);

XTCLINK
XTAPI
ULONG
HlReadRegister32(IN PVOID Register);

XTCLINK
XTAPI
VOID
HlWriteRegister8(IN PVOID Register,
                 IN UCHAR Value);

XTCLINK
XTAPI
VOID
HlWriteRegister16(IN PVOID Register,
                  IN USHORT Value);

XTCLINK
XTAPI
VOID
HlWriteRegister32(IN PVOID Register,
                  IN ULONG Value);

#endif /* __XTDK_HLFUNCS_H */
