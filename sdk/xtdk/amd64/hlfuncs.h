/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/hlfuncs.h
 * DESCRIPTION:     XT hardware abstraction layer routines specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_HLFUNCS_H
#define __XTDK_AMD64_HLFUNCS_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>
#include <amd64/xtstruct.h>


/* HAL library routines forward references */
XTCDECL
UCHAR
HlIoPortInByte(IN USHORT Port);

XTCDECL
ULONG
HlIoPortInLong(IN USHORT Port);

XTCDECL
USHORT
HlIoPortInShort(IN USHORT Port);

XTCDECL
VOID
HlIoPortOutByte(IN USHORT Port,
                IN UCHAR Data);

XTCDECL
VOID
HlIoPortOutLong(IN USHORT Port,
                IN ULONG Value);

XTCDECL
VOID
HlIoPortOutShort(IN USHORT Port,
                 IN USHORT Value);

#endif /* __XTDK_AMD64_HLFUNCS_H */
