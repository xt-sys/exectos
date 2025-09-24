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


/* Hardware layer routines forward references */
XTCLINK
XTCDECL
UCHAR
HlReadPort8(IN USHORT Port);

XTCLINK
XTCDECL
USHORT
HlReadPort16(IN USHORT Port);

XTCLINK
XTCDECL
ULONG
HlReadPort32(IN USHORT Port);

XTCLINK
XTCDECL
VOID
HlWritePort8(IN USHORT Port,
             IN UCHAR Data);

XTCLINK
XTCDECL
VOID
HlWritePort16(IN USHORT Port,
              IN USHORT Value);

XTCLINK
XTCDECL
VOID
HlWritePort32(IN USHORT Port,
              IN ULONG Value);

#endif /* __XTDK_I686_HLFUNCS_H */
