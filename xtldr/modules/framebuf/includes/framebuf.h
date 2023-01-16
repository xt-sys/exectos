/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/framebuf/includes/framebuf.h
 * DESCRIPTION:     Framebuffer support module header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_FRAMEBUF_H
#define __XTLDR_MODULES_FRAMEBUF_H

#include <blmod.h>


/* XT FrameBuffer Information */
EXTERN XT_FRAMEBUFFER_INFORMATION FrameBufferInfo;

/* FrameBuffer support protocol related routines forward references */
XTCDECL
VOID
FbGetDisplayDriver(OUT PWCHAR *DriverName);

XTCDECL
VOID
FbGetDisplayInformation(OUT PLOADER_GRAPHICS_INFORMATION_BLOCK InformationBlock);

XTCDECL
EFI_STATUS
FbInitializeDisplay();

XTCDECL
VOID
FbPrintDisplayInformation();

XTCDECL
UINT32
GoppGetBitsPerPixel();

XTCDECL
EFI_STATUS
BlXtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                  IN PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_MODULES_FRAMEBUF_H */
