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
XTCDECL
XTSTATUS
HlComPortPutByte(IN PCPPORT Port,
                 IN UCHAR Byte);

XTCDECL
XTSTATUS
HlInitializeComPort(IN OUT PCPPORT Port,
                    IN ULONG PortNumber,
                    IN PUCHAR PortAddress,
                    IN ULONG BaudRate);

#endif /* __XTDK_HLFUNCS_H */
