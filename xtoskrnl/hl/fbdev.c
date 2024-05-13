/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/fbdev.c
 * DESCRIPTION:     FrameBuffer support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>
#include <xtfont.h>


/**
 * Clears the screen by drawing a box filled with specified color.
 *
 * @param Color
 *        Specifies the color of the box used to fill the screen in (A)RGB format.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlClearScreen(IN ULONG Color)
{
    SIZE_T Line, PositionX, PositionY;
    ULONG BackgroundColor;
    PULONG FrameBuf;

    /* Get pointer to frame buffer */
    FrameBuf = HlpFrameBufferData.Address;

    /* Convert background color */
    BackgroundColor = HlRGBColor(Color);

    /* Fill the screen with a black box */
    for(PositionY = 0; PositionY < HlpFrameBufferData.Height; PositionY++)
    {
        Line = PositionY * HlpFrameBufferData.PixelsPerScanLine;
        for(PositionX = 0; PositionX < HlpFrameBufferData.Width; PositionX++)
        {
            FrameBuf[Line + PositionX] = BackgroundColor;
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
 *        Specifies the color of the pixel in (A)RGB format.
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
    *((PULONG)(HlpFrameBufferData.Address + FrameBufferIndex)) = HlRGBColor(Color);
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
    HlpFrameBufferData.Pixels.BlueShift = KeInitializationBlock->LoaderInformation.FrameBuffer.Pixels.BlueShift;
    HlpFrameBufferData.Pixels.BlueSize = KeInitializationBlock->LoaderInformation.FrameBuffer.Pixels.BlueSize;
    HlpFrameBufferData.Pixels.GreenShift = KeInitializationBlock->LoaderInformation.FrameBuffer.Pixels.GreenShift;
    HlpFrameBufferData.Pixels.GreenSize = KeInitializationBlock->LoaderInformation.FrameBuffer.Pixels.GreenSize;
    HlpFrameBufferData.Pixels.RedShift = KeInitializationBlock->LoaderInformation.FrameBuffer.Pixels.RedShift;
    HlpFrameBufferData.Pixels.RedSize = KeInitializationBlock->LoaderInformation.FrameBuffer.Pixels.RedSize;
    HlpFrameBufferData.Pixels.ReservedShift = KeInitializationBlock->LoaderInformation.FrameBuffer.Pixels.ReservedShift;
    HlpFrameBufferData.Pixels.ReservedSize = KeInitializationBlock->LoaderInformation.FrameBuffer.Pixels.ReservedSize;
    HlpFrameBufferData.Initialized = TRUE;

    /* Clear screen */
    HlClearScreen(0x00000000);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Puts a wide character on the framebuffer at the given position and color using the SSFN font.
 *
 * @param PositionX
 *        Supplies the X coordinate of the character.
 *
 * @param PositionY
 *        Supplies the Y coordinate of the character.
 *
 * @param Color
 *        Supplies the font color in (A)RGB format.
 *
 * @param WideCharacter
 *        Supplies the wide character to be drawn on the framebuffer.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlPutCharacter(IN ULONG PositionX,
               IN ULONG PositionY,
               IN ULONG Color,
               IN WCHAR WideCharacter)
{
    UINT CurrentFragment, Glyph, GlyphLimit, Index, Line, Mapping;
    PUCHAR Character, CharacterMapping, Fragment;
    UINT_PTR GlyphPixel, Pixel;
    PSSFN_FONT_HEADER FbFont;
    ULONG FontColor;

    /* Get pointers to font data */
    FbFont = (PSSFN_FONT_HEADER)HlpFrameBufferData.Font;
    CharacterMapping = (PUCHAR)FbFont + FbFont->CharactersOffset;

    /* Find the character in the font's character table */
    Character = 0;
    for(Index = 0; Index < 0x110000; Index++)
    {
        if(CharacterMapping[0] == 0xFF)
        {
            /* Skip 65535 code points */
            Index += 65535;
            CharacterMapping++;
        }
        else if((CharacterMapping[0] & 0xC0) == 0xC0)
        {
            /* Skip (N << 8 + additional byte) + 1 code points (up to 16128) */
            Index += (((CharacterMapping[0] & 0x3F) << 8) | CharacterMapping[1]);
            CharacterMapping += 2;
        }
        else if((CharacterMapping[0] & 0xC0) == 0x80)
        {
            /* Skip N + 1 code points (up to 64) */
            Index += (CharacterMapping[0] & 0x3F);
            CharacterMapping++;
        }
        else
        {
            /* There's a glyph for this character, check if it matches */
            if(Index == WideCharacter)
            {
                /* Found the character, break loop */
                Character = CharacterMapping;
                break;
            }

            /* Move to next character table entry */
            CharacterMapping += (6 + CharacterMapping[1] * (CharacterMapping[0] & 0x40 ? 6 : 5));
        }
    }

    /* Make sure the character has been found in the font */
    if(!Character)
    {
        /* Character not found, don't draw anything */
        return;
    }

    /* Find the glyph position on the frame buffer and set font color */
    GlyphPixel = (UINT_PTR)HlpFrameBufferData.Address + PositionY * HlpFrameBufferData.Pitch + PositionX * 4;
    FontColor = HlRGBColor(Color);

    /* Check all kerning fragments */
    Mapping = 0;
    CharacterMapping = Character + 6;
    for(Index = 0; Index < Character[1]; Index++)
    {
        /* Check if number of fragments is not exceeded */
        if(CharacterMapping[0] == 255 && CharacterMapping[1] == 255)
        {
            /* Get next mapping */
            continue;
        }

        /* Get pointer to fragment */
        Fragment = (PUCHAR)FbFont + (CharacterMapping[2] |
                                     (CharacterMapping[3] << 8) |
                                     (CharacterMapping[4] << 16) |
                                     ((Character[0] & 0x40) ? (CharacterMapping[5] << 24) : 0));

        /* Check if fragment is printable */
        if((Fragment[0] & 0xE0) != 0x80)
        {
            /* Skip fragment */
            continue;
        }

        /* Get initial glyph line */
        GlyphPixel += (CharacterMapping[1] - Mapping) * HlpFrameBufferData.Pitch;
        Mapping = CharacterMapping[1];

        /* Extract glyph data from fragments table and advance */
        Glyph = ((Fragment[0] & 0x1F) + 1) << 3;
        GlyphLimit = Fragment[1] + 1;
        Fragment += 2;

        /* Look for kerning group for next code point */
        CurrentFragment = 1;
        while(GlyphLimit--)
        {
            Pixel = GlyphPixel;
            for(Line = 0; Line < Glyph; Line++)
            {
                /* Decode compressed offsets */
                if(CurrentFragment > 0x80)
                {
                    /* Advance to next fragment */
                    Fragment++;
                    CurrentFragment = 1;
                }

                /* Check if pixel should be drawn */
                if(*Fragment & CurrentFragment)
                {
                    /* Draw glyph pixel */
                    *((PULONG)Pixel) = FontColor;
                }

                /* Advance pixel pointer */
                Pixel += 4;
                CurrentFragment <<= 1;
            }

            /* Advance to next line and increase mapping */
            GlyphPixel += HlpFrameBufferData.Pitch;
            Mapping++;
        }

        /* Get next mapping */
        CharacterMapping += Character[0] & 0x40 ? 6 : 5;
    }
}

/**
 * Converts color format from (A)RGB one expected by current FrameBuffer.
 *
 * @param Color
 *        Specifies the color in (A)RGB format.
 *
 * @return Returns the color in FrameBuffer format.
 *
 * @since XT 1.0
 */
XTAPI
ULONG
HlRGBColor(IN ULONG Color)
{
    USHORT Blue, Green, Red, Reserved;

    /* Extract color components from (A)RGB value */
    Blue = (USHORT)(Color & 0xFF);
    Green = (USHORT)((Color >> 8) & 0xFF);
    Red = (USHORT)((Color >> 16) & 0xFF);
    Reserved = (USHORT)((Color >> 24) & 0xFF);

    /* Return color in FrameBuffer pixel format */
    return (ULONG)((Blue << HlpFrameBufferData.Pixels.BlueShift) | (Green << HlpFrameBufferData.Pixels.GreenShift) |
                   (Red << HlpFrameBufferData.Pixels.RedShift) | (Reserved << HlpFrameBufferData.Pixels.ReservedShift));
}
