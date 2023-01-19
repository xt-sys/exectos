/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/efifb.c
 * DESCRIPTION:     EFI framebuffer support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
    SIZE_T PositionX, PositionY;

    /* Fill the screen with a black box */
    for(PositionX = 0; PositionX < HlpFrameBufferData.Width; PositionX++)
    {
        for(PositionY = 0; PositionY < HlpFrameBufferData.Height; PositionY++)
        {
            HlDrawPixel(PositionX, PositionY, 0x00000000);
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
    SIZE_T FrameBufferIndex = PositionY * HlpFrameBufferData.PixelsPerScanLine + PositionX;

    /* Set the color of the pixel by writing to the corresponding memory location */
    HlpFrameBufferData.Address[FrameBufferIndex] = Color;
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
HlInitializeDisplay(VOID)
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

    /* Save framebuffer information and mark display as initialized */
    HlpFrameBufferData.Address = (PULONG)KeInitializationBlock->LoaderInformation.FrameBuffer.Address;
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
