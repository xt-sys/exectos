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
#include "xtstruct.h"
#include "xttypes.h"


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

XTAPI
UCHAR
HlIoPortInByte(IN USHORT Port);

XTAPI
VOID
HlIoPortOutByte(IN USHORT Port,
                IN UCHAR Data);

#endif /* __XTKLIB_LIBHL_H */
