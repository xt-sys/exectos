/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/hlfuncs.h
 * DESCRIPTION:     XT hardware abstraction layer routines specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_HLFUNCS_H
#define __XTDK_I686_HLFUNCS_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>
#include <i686/xtstruct.h>


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

#endif /* __XTDK_I686_HLFUNCS_H */
