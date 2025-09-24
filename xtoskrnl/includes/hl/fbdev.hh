/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/fbdev.hh
 * DESCRIPTION:     FrameBuffer support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_FBDEV_HH
#define __XTOSKRNL_HL_FBDEV_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class FrameBuffer
    {
        private:
            STATIC HL_FRAMEBUFFER_DATA FrameBufferData;
            STATIC HL_SCROLL_REGION_DATA ScrollRegionData;

        public:
            STATIC XTAPI VOID ClearScreen(IN ULONG Color);
            STATIC XTCDECL XTSTATUS DisplayCharacter(IN WCHAR Character);
            STATIC XTAPI VOID GetFrameBufferResolution(OUT PULONG Width,
                                                       OUT PULONG Height);
            STATIC XTAPI XTSTATUS InitializeFrameBuffer(VOID);
            STATIC XTAPI VOID InitializeScrollRegion(IN ULONG Left,
                                                     IN ULONG Top,
                                                     IN ULONG Right,
                                                     IN ULONG Bottom,
                                                     IN ULONG FontColor);


        private:
            STATIC XTAPI VOID DrawCharacter(IN ULONG PositionX,
                                            IN ULONG PositionY,
                                            IN ULONG Color,
                                            IN WCHAR WideCharacter);
            STATIC XTAPI VOID DrawPixel(IN ULONG PositionX,
                                        IN ULONG PositionY,
                                        IN ULONG Color);
            STATIC XTAPI ULONG GetRGBColor(IN ULONG Color);
            STATIC XTAPI VOID ScrollRegion(VOID);
    };
}

#endif /* __XTOSKRNL_HL_FBDEV_HH */
