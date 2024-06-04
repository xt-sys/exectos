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
HlClearScreen(IN ULONG Color);

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

XTAPI
XTSTATUS
HlGetAcpiSystemDescriptionPointer(OUT PACPI_RSDP *Rsdp);

XTAPI
XTSTATUS
HlGetAcpiTable(IN ULONG Signature,
               OUT PACPI_DESCRIPTION_HEADER *AcpiTable);

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
HlInitializeProcessor();

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
HlpCacheAcpiTable(IN PACPI_DESCRIPTION_HEADER AcpiTable);

XTAPI
XTSTATUS
HlpInitializeAcpi(VOID);

XTAPI
XTSTATUS
HlpInitializeAcpiCache(VOID);

XTAPI
XTSTATUS
HlpInitializeAcpiSystemDescriptionTable(OUT PACPI_DESCRIPTION_HEADER *AcpiTable);

XTAPI
XTSTATUS
HlpQueryAcpiCache(IN ULONG Signature,
                  OUT PACPI_DESCRIPTION_HEADER *AcpiTable);

XTAPI
XTSTATUS
HlpQueryAcpiTables(IN ULONG Signature,
                   OUT PACPI_DESCRIPTION_HEADER *AcpiTable);

XTAPI
ULONG
HlpRGBColor(IN ULONG Color);

XTAPI
BOOLEAN
HlpValidateAcpiTable(IN PVOID Buffer,
                     IN UINT_PTR Size);

#endif /* __XTOSKRNL_HLI_H */
