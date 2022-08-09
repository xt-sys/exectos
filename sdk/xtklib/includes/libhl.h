/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtklib/includes/libhl.h
 * DESCRIPTION:     Hardware Abstraction Layer library
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTKLIB_LIBHL_H
#define __XTKLIB_LIBHL_H

#include "xtdefs.h"
#include "xttypes.h"
#include "xtstruct.h"


/* I/O port addresses for COM ports */
extern ULONG ComPortAddress[];

/* HAL library routines forward references */
XTSTATUS
HlComPortGetByte(IN PCPPORT Port,
                 OUT PUCHAR Byte,
                 IN BOOLEAN Wait,
                 IN BOOLEAN Poll);

UCHAR
HlComPortReadLsr(IN PCPPORT Port,
                 IN UCHAR Byte);

XTSTATUS 
HlComPortPutByte(IN PCPPORT Port,
                 IN UCHAR Byte);

XTSTATUS
HlInitializeComPort(IN OUT PCPPORT Port,
                    IN ULONG PortNumber,
                    IN ULONG BaudRate);

UCHAR
XTAPI
HlIoPortInByte(IN USHORT Port);

VOID
XTAPI
HlIoPortOutByte(IN USHORT Port,
                IN UCHAR Data);

#endif /* __XTKLIB_LIBHL_H */
