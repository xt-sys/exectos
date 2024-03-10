/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/efifb.c
 * DESCRIPTION:     EFI framebuffer support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>
#include <xtfont.h>


/**
 * Clears the screen by drawing a filled black box.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlClearScreen(VOID)
{
    SIZE_T Line, PositionX, PositionY;
    PULONG FrameBuf;

    /* Get pointer to frame buffer */
    FrameBuf = HlpFrameBufferData.Address;

    /* Fill the screen with a black box */
    for(PositionY = 0; PositionY < HlpFrameBufferData.Height; PositionY++)
    {
        Line = PositionY * HlpFrameBufferData.PixelsPerScanLine;
        for(PositionX = 0; PositionX < HlpFrameBufferData.Width; PositionX++)
        {
            FrameBuf[Line + PositionX] = 0x00000000;
        }
    }
}

/**
 * Draw a pixel on the screen at the given position and color.
 *
 * @param PositionX
 *        Supplies the X coordinate of the pixel.
 *
 * @param PositionY
 *        Supplies the Y coordinate of the pixel.
 *
 * @param Color
 *        Specifies the color of the pixel.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlDrawPixel(IN ULONG PositionX,
            IN ULONG PositionY,
            IN ULONG Color)
{
    SIZE_T FrameBufferIndex;

    /* Make sure frame buffer is already initialized */
    if(HlpFrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Make sure point is not offscreen */
    if(PositionX >= HlpFrameBufferData.Width || PositionY >= HlpFrameBufferData.Height || Color > 0xFFFFFFFF)
    {
        /* Invalid pixel position or color given */
        return;
    }

    /* Calculate the index of the pixel in the frame buffer memory using the provided x and y coordinates */
    FrameBufferIndex = 4 * HlpFrameBufferData.PixelsPerScanLine * PositionY + 4 * PositionX;

    /* Set the color of the pixel by writing to the corresponding memory location */
    *((PULONG)(HlpFrameBufferData.Address + FrameBufferIndex)) = Color;
}

/**
 * Initializes frame buffer display.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HlInitializeFrameBuffer(VOID)
{
    /* Check if display already initialized */
    if(HlpFrameBufferData.Initialized)
    {
        /* Nothing to do */
        return STATUS_SUCCESS;
    }

    /* Check if framebuffer initialized by bootloader */
    if(!KeInitializationBlock->LoaderInformation.FrameBuffer.Initialized ||
       !KeInitializationBlock->LoaderInformation.FrameBuffer.Address)
    {
        /* Display not initialized */
        return STATUS_DEVICE_NOT_READY;
    }

    /* Check if custom font provided by bootloader */
    if(KeInitializationBlock->LoaderInformation.FrameBuffer.Font)
    {
        /* Use custom font */
        HlpFrameBufferData.Font = KeInitializationBlock->LoaderInformation.FrameBuffer.Font;
    }
    else
    {
        /* Use default font */
        HlpFrameBufferData.Font = (PVOID)&XtFbDefaultFont;
    }

    /* Save framebuffer information and mark display as initialized */
    HlpFrameBufferData.Address = KeInitializationBlock->LoaderInformation.FrameBuffer.Address;
    HlpFrameBufferData.Width = KeInitializationBlock->LoaderInformation.FrameBuffer.Width;
    HlpFrameBufferData.Height = KeInitializationBlock->LoaderInformation.FrameBuffer.Height;
    HlpFrameBufferData.BitsPerPixel = KeInitializationBlock->LoaderInformation.FrameBuffer.BitsPerPixel;
    HlpFrameBufferData.PixelsPerScanLine = KeInitializationBlock->LoaderInformation.FrameBuffer.PixelsPerScanLine;
    HlpFrameBufferData.Pitch = KeInitializationBlock->LoaderInformation.FrameBuffer.Pitch;
    HlpFrameBufferData.Initialized = TRUE;

    /* Clear screen */
    HlClearScreen();

    /* Return success */
    return STATUS_SUCCESS;
}
