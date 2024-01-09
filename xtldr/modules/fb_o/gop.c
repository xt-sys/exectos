/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/fb_o/gop.c
 * DESCRIPTION:     Graphical Output Protocol (GOP) support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <framebuf.h>


/**
 * Returns a number of bits per pixel (BPP) in the current video mode.
 *
 * @return A number of bits per pixel.
 *
 * @since XT 1.0
 */
XTCDECL
UINT32
GoppGetBitsPerPixel()
{
    UINT32 BitsPerPixel, CompoundMask;

    switch(FrameBufferInfo.Adapter.GOP->Mode->Info->PixelFormat)
    {
        case PixelBlueGreenRedReserved8BitPerColor:
        case PixelRedGreenBlueReserved8BitPerColor:
        case PixelBltOnly:
            BitsPerPixel = 32;
            break;
        case PixelBitMask:
            BitsPerPixel = 32;
            CompoundMask = FrameBufferInfo.Adapter.GOP->Mode->Info->PixelInformation.RedMask |
                           FrameBufferInfo.Adapter.GOP->Mode->Info->PixelInformation.GreenMask |
                           FrameBufferInfo.Adapter.GOP->Mode->Info->PixelInformation.BlueMask |
                           FrameBufferInfo.Adapter.GOP->Mode->Info->PixelInformation.ReservedMask;
            while((CompoundMask & (1 << 31)) == 0)
            {
                BitsPerPixel--;
                CompoundMask <<= 1;
            }
            break;
        default:
            BitsPerPixel = 0;
            break;
    }

    /* Return bpp */
    return BitsPerPixel;
}
