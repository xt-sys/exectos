/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/hlfuncs.h
 * DESCRIPTION:     XT hardware abstraction layer routines specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_HLFUNCS_H
#define __XTDK_AMD64_HLFUNCS_H

#include "xtdefs.h"
#include "xtstruct.h"
#include "xttypes.h"


/* I/O port addresses for COM ports */
extern ULONG ComPortAddress[];

/* HAL library routines forward references */
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
ULONG_PTR
HlReadCR8();

XTAPI
VOID
HlWriteCR0(UINT_PTR Data);

XTAPI
VOID
HlWriteCR2(UINT_PTR Data);

XTAPI
VOID
HlWriteCR3(UINT_PTR Data);

XTAPI
VOID
HlWriteCR4(UINT_PTR Data);

XTAPI
VOID
HlWriteCR8(UINT_PTR Data);

#endif /* __XTDK_AMD64_HLFUNCS_H */
