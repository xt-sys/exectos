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
XTCDECL
BOOLEAN
HlCpuId(IN OUT PCPUID_REGISTERS Registers);

XTCDECL
VOID
HlHalt();

XTCDECL
UCHAR
HlIoPortInByte(IN USHORT Port);

XTCDECL
VOID
HlIoPortOutByte(IN USHORT Port,
                IN UCHAR Data);

XTCDECL
ULONG_PTR
HlReadControlRegister(IN USHORT ControlRegister);

XTCDECL
VOID
HlWriteControlRegister(IN USHORT ControlRegister,
                       IN UINT_PTR Value);

#endif /* __XTDK_I686_HLFUNCS_H */
