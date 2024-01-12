/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/fb_o/includes/framebuf.h
 * DESCRIPTION:     Framebuffer support module header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_FRAMEBUF_H
#define __XTLDR_MODULES_FRAMEBUF_H

#include <xtblapi.h>


typedef VOID (*PXT_FRAMEBUFFER_GET_DISPLAY_DRIVER)(OUT PWCHAR DriverName);
typedef VOID (*PXT_FRAMEBUFFER_GET_DISPLAY_INFORMATION)(OUT PLOADER_GRAPHICS_INFORMATION_BLOCK InformationBlock);
typedef EFI_STATUS (*PXT_FRAMEBUFFER_INITIALIZE)();
typedef VOID (*PXT_FRAMEBUFFER_PRINT_DISPLAY_INFORMATION)();

/* XT framebuffer support protocol */
typedef struct _XT_FRAMEBUFFER_PROTOCOL
{
    PXT_FRAMEBUFFER_GET_DISPLAY_DRIVER GetDisplayDriver;
    PXT_FRAMEBUFFER_GET_DISPLAY_INFORMATION GetDisplayInformation;
    PXT_FRAMEBUFFER_INITIALIZE Initialize;
    PXT_FRAMEBUFFER_PRINT_DISPLAY_INFORMATION PrintDisplayInformation;
} XT_FRAMEBUFFER_PROTOCOL, *PXT_FRAMEBUFFER_PROTOCOL;

/* XT framebuffer information structure definition */
typedef struct _XT_FRAMEBUFFER_INFORMATION
{
    BOOLEAN Initialized;
    EFI_GRAPHICS_PROTOCOL Protocol;
    union
    {
        PEFI_GRAPHICS_OUTPUT_PROTOCOL GOP;
        PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL UGA;
    } Adapter;
    UINT HorizontalResolution;
    UINT VerticalResolution;
    UINT BitsPerPixel;
    UINT BytesPerPixel;
    UINT PixelsPerScanLine;
    UINT Pitch;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    EFI_PHYSICAL_ADDRESS FrameBufferBase;
    ULONG_PTR FrameBufferSize;
} XT_FRAMEBUFFER_INFORMATION, *PXT_FRAMEBUFFER_INFORMATION;

/* XT FrameBuffer Information */
EXTERN XT_FRAMEBUFFER_INFORMATION FrameBufferInfo;

/* FrameBuffer support protocol related routines forward references */
XTCDECL
VOID
FbGetDisplayDriver(OUT PWCHAR DriverName);

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

XTCDECL
EFI_STATUS
XtLdrTestGetFramebufferAddress(OUT PULONG *Address);

#endif /* __XTLDR_MODULES_FRAMEBUF_H */
