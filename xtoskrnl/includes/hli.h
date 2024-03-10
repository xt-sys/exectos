/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hli.h
 * DESCRIPTION:     XT hardware abstraction layer routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_HLI_H
#define __XTOSKRNL_HLI_H

#include <xtos.h>

/* HAL library routines forward references */
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
XTSTATUS
HlComPortPutByte(IN PCPPORT Port,
                 IN UCHAR Byte);

XTCDECL
UCHAR
HlComPortReadLsr(IN PCPPORT Port,
                 IN UCHAR Byte);

XTAPI
VOID
HlDrawPixel(IN ULONG PosX,
            IN ULONG PosY,
            IN ULONG Color);

XTFASTCALL
KRUNLEVEL
HlGetRunLevel(VOID);


XTCDECL
XTSTATUS
HlInitializeComPort(IN OUT PCPPORT Port,
                    IN ULONG PortNumber,
                    IN PUCHAR PortAddress,
                    IN ULONG BaudRate);

XTAPI
XTSTATUS
HlInitializeFrameBuffer(VOID);

XTAPI
VOID
HlPutCharacter(IN ULONG PositionX,
               IN ULONG PositionY,
               IN ULONG Color,
               IN WCHAR WideCharacter);

XTFASTCALL
VOID
HlSetRunLevel(IN KRUNLEVEL RunLevel);

XTAPI
VOID
HlInitializeProcessor(VOID);

#endif /* __XTOSKRNL_HLI_H */
