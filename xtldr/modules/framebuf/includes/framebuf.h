/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/framebuf/includes/framebuf.h
 * DESCRIPTION:     EFI Framebuffer support module header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_FRAMEBUF_H
#define __XTLDR_MODULES_FRAMEBUF_H

#include <xtblapi.h>
#include <globals.h>


/* FrameBuffer support protocol related routines forward references */
XTCDECL
EFI_STATUS
FbInitializeDisplay();

XTCDECL
EFI_STATUS
FbpFindFramebufferAddress(OUT PEFI_PHYSICAL_ADDRESS Address);

XTCDECL
VOID
FbpGetColorMask(IN UINT EfiMask,
                OUT PUSHORT ColorMask,
                OUT PUSHORT ColorShift);

XTCDECL
VOID
FbpGetPixelInformation(IN OUT PXTBL_FRAMEBUFFER_INFORMATION FrameBufferInfo,
                       IN PEFI_PIXEL_BITMASK PixelsBitMask);

XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_MODULES_FRAMEBUF_H */
