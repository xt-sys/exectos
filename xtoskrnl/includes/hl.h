/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl.h
 * DESCRIPTION:     Private routine definitions for hardware abstraction layer
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_HL_H
#define __XTOSKRNL_HL_H

#include <xtos.h>


XTAPI
VOID
HlClearScreen(VOID);

XTCDECL
XTSTATUS
HlComPortGetByte(IN PCPPORT Port,
                 OUT PUCHAR Byte,
                 IN BOOLEAN Wait,
                 IN BOOLEAN Poll);

XTCDECL
UCHAR
HlComPortReadLsr(IN PCPPORT Port,
                 IN UCHAR Byte);

XTAPI
VOID
HlDrawPixel(IN ULONG PosX,
            IN ULONG PosY,
            IN ULONG Color);

XTAPI
XTSTATUS
HlInitializeDisplay(VOID);

#endif /* __XTOSKRNL_HL_H */
