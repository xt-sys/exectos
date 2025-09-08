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
XTCLINK
XTCDECL
UCHAR
HlIoPortInByte(IN USHORT Port);

XTCLINK
XTCDECL
ULONG
HlIoPortInLong(IN USHORT Port);

XTCLINK
XTCDECL
USHORT
HlIoPortInShort(IN USHORT Port);

XTCLINK
XTCDECL
VOID
HlIoPortOutByte(IN USHORT Port,
                IN UCHAR Data);

XTCLINK
XTCDECL
VOID
HlIoPortOutLong(IN USHORT Port,
                IN ULONG Value);

XTCLINK
XTCDECL
VOID
HlIoPortOutShort(IN USHORT Port,
                 IN USHORT Value);

#endif /* __XTDK_AMD64_HLFUNCS_H */
