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
UCHAR
HlIoPortInByte(IN USHORT Port);

XTAPI
VOID
HlIoPortOutByte(IN USHORT Port,
                IN UCHAR Data);

XTAPI
ULONG_PTR
HlReadCR0();

XTAPI
ULONG_PTR
HlReadCR2();

XTAPI
ULONG_PTR
HlReadCR3();

XTAPI
ULONG_PTR
HlReadCR4();

XTAPI
VOID
HlWriteCR0(IN UINT_PTR Data);

XTAPI
VOID
HlWriteCR2(IN UINT_PTR Data);

XTAPI
VOID
HlWriteCR3(IN UINT_PTR Data);

XTAPI
VOID
HlWriteCR4(IN UINT_PTR Data);

#endif /* __XTDK_I686_HLFUNCS_H */
