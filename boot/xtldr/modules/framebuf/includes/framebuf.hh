/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/framebuf/includes/framebuf.hh
 * DESCRIPTION:     EFI Framebuffer support module header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_FRAMEBUF_HH
#define __XTLDR_MODULES_FRAMEBUF_HH

#include <xtblapi.h>


class FrameBuffer
{
    private:
        STATIC XTBL_FRAMEBUFFER_INFORMATION DisplayInfo;
        STATIC XTBL_FRAMEBUFFER_PROTOCOL FbProtocol;
        STATIC PXTBL_LOADER_PROTOCOL XtLdrProtocol;

    public:
        STATIC XTCDECL EFI_STATUS GetDisplayDriver(OUT PEFI_GRAPHICS_PROTOCOL Protocol);
        STATIC XTCDECL EFI_STATUS GetDisplayInformation(OUT PEFI_PHYSICAL_ADDRESS FrameBufferBase,
                                                        OUT PULONG_PTR FrameBufferSize,
                                                        OUT PXTBL_FRAMEBUFFER_MODE_INFORMATION ModeInfo);
        STATIC XTCDECL EFI_STATUS GetPreferredScreenResolution(OUT PUINT PreferredWidth,
                                                               OUT PUINT PreferredHeight);
        STATIC XTCDECL EFI_STATUS InitializeDisplay();
        STATIC XTCDECL EFI_STATUS InitializeModule(IN EFI_HANDLE ImageHandle,
                                                   IN PEFI_SYSTEM_TABLE SystemTable);
        STATIC XTCDECL EFI_STATUS SetScreenResolution(IN UINT Width,
                                                      IN UINT Height);

    private:
        STATIC EFI_STATUS FindFramebufferAddress(OUT PEFI_PHYSICAL_ADDRESS Address);
        STATIC XTCDECL VOID GetColorMask(IN UINT EfiMask,
                                         OUT PUSHORT ColorSize,
                                         OUT PUSHORT ColorShift);
        STATIC XTCDECL EFI_STATUS GetModeInformation();
        STATIC XTCDECL VOID GetPixelInformation(IN PEFI_PIXEL_BITMASK PixelsBitMask);
};

#endif /* __XTLDR_MODULES_FRAMEBUF_HH */
