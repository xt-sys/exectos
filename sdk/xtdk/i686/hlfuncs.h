/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/hlfuncs.h
 * DESCRIPTION:     XT hardware abstraction layer routines specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_HLFUNCS_H
#define __XTDK_I686_HLFUNCS_H

#include "xtdefs.h"
#include "xtstruct.h"
#include "xttypes.h"


/* I/O port addresses for COM ports */
extern ULONG ComPortAddress[];

/* HAL library routines forward references */
XTAPI
BOOLEAN
HlCpuId(IN OUT PCPUID_REGISTERS Registers);

XTAPI
VOID
HlHalt();

XTAPI
UCHAR
HlIoPortInByte(IN USHORT Port);

XTAPI
VOID
HlIoPortOutByte(IN USHORT Port,
                IN UCHAR Data);

XTAPI
ULONG_PTR
HlReadControlRegister(USHORT ControlRegister);

XTAPI
VOID
HlWriteControlRegister(USHORT ControlRegister,
                       UINT_PTR Value);

#endif /* __XTDK_I686_HLFUNCS_H */
