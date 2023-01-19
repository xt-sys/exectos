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


/* HAL library routines forward references */
XTAPI
VOID
HlClearScreen(VOID);

XTCDECL
XTSTATUS
HlComPortGetByte(IN PCPPORT Port,
                 OUT PUCHAR Byte,
                 IN BOOLEAN Wait,
                 IN BOOLEAN Poll);

XTCDECL
XTSTATUS
HlComPortPutByte(IN PCPPORT Port,
                 IN UCHAR Byte);

XTCDECL
UCHAR
HlComPortReadLsr(IN PCPPORT Port,
                 IN UCHAR Byte);

XTAPI
VOID
HlDrawPixel(IN ULONG PosX,
            IN ULONG PosY,
            IN ULONG Color);

XTCDECL
XTSTATUS
HlInitializeComPort(IN OUT PCPPORT Port,
                    IN ULONG PortNumber,
                    IN PUCHAR PortAddress,
                    IN ULONG BaudRate);

XTAPI
XTSTATUS
HlInitializeDisplay(VOID);

#endif /* __XTDK_HLFUNCS_H */
