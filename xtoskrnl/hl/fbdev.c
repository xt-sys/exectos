/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/fbdev.c
 * DESCRIPTION:     FrameBuffer support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
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
    ULONG PositionX, PositionY;
    ULONG BackgroundColor;
    PCHAR CurrentLine;
    PULONG Pixel;

    /* Make sure frame buffer is already initialized */
    if(HlpFrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Convert background color and get pointer to frame buffer */
    BackgroundColor = HlpRGBColor(Color);
    CurrentLine = HlpFrameBufferData.Address;

    /* Fill the screen with the specified color */
    for(PositionY = 0; PositionY < HlpFrameBufferData.Height; PositionY++, CurrentLine += HlpFrameBufferData.Pitch)
    {
        /* Fill the current line with the specified color */
        Pixel = (PULONG)CurrentLine;
        for(PositionX = 0; PositionX < HlpFrameBufferData.Width; PositionX++)
        {
            /* Set the color of the pixel */
            Pixel[PositionX] = BackgroundColor;
        }
    }
}

/**
 * Displays a single character at the current cursor position inside the scroll region.
 *
 * @param Character
 *        Supplies the character to be displayed.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
XTSTATUS
HlDisplayCharacter(IN WCHAR Character)
{
    PSSFN_FONT_HEADER FbFont;

    /* Make sure frame buffer is already initialized */
    if(HlpFrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return STATUS_DEVICE_NOT_READY;
    }

    /* Get font information */
    FbFont = (PSSFN_FONT_HEADER)HlpFrameBufferData.Font;

    /* Handle special characters */
    switch(Character)
    {
        case L'\n':
            /* Move cursor to the beginning of the next line */
            HlpScrollRegionData.CursorX = HlpScrollRegionData.Left;
            HlpScrollRegionData.CursorY += FbFont->Height;
            break;
        case L'\t':
            /* Move cursor to the next tab stop */
            HlpScrollRegionData.CursorX += (8 - (HlpScrollRegionData.CursorX - HlpScrollRegionData.Left) / FbFont->Width % 8) * FbFont->Width;
            if (HlpScrollRegionData.CursorX >= HlpScrollRegionData.Right)
            {
                HlpScrollRegionData.CursorX = HlpScrollRegionData.Left;
                HlpScrollRegionData.CursorY += FbFont->Height;
            }
            break;
        default:
            /* Draw the character */
            HlpDrawCharacter(HlpScrollRegionData.CursorX, HlpScrollRegionData.CursorY, HlpScrollRegionData.TextColor, Character);

            /* Advance cursor */
            HlpScrollRegionData.CursorX += FbFont->Width;

            /* Check if cursor reached end of line */
            if(HlpScrollRegionData.CursorX >= HlpScrollRegionData.Right)
            {
                HlpScrollRegionData.CursorX = HlpScrollRegionData.Left;
                HlpScrollRegionData.CursorY += FbFont->Height;
            }
            break;
    }

    /* Check if cursor reached end of scroll region */
    if(HlpScrollRegionData.CursorY >= HlpScrollRegionData.Bottom)
    {
        /* Scroll one line up */
        HlpScrollRegion();
        HlpScrollRegionData.CursorY = HlpScrollRegionData.Bottom - FbFont->Height;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Returns the current resolution of the frame buffer display.
 *
 * @param Width
 *        A pointer to memory area where the screen width will be stored.
 *
 * @param Height
 *        A pointer to memory area where the screen height will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlGetFrameBufferResolution(OUT PULONG Width, OUT PULONG Height)
{
    *Width = HlpFrameBufferData.Width;
    *Height = HlpFrameBufferData.Height;
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
    PSYSTEM_RESOURCE_FRAMEBUFFER FrameBufferResource;
    PSYSTEM_RESOURCE_HEADER SystemResource;
    XTSTATUS Status;

    /* Check if display already initialized */
    if(HlpFrameBufferData.Initialized)
    {
        /* Nothing to do */
        return STATUS_SUCCESS;
    }

    /* Get FrameBuffer system resource */
    Status = KeGetSystemResource(SystemResourceFrameBuffer, &SystemResource);
    if(Status != STATUS_SUCCESS)
    {
        /* Resource not found */
        return STATUS_NOT_FOUND;
    }

    /* Cast system resource to FrameBuffer resource */
    FrameBufferResource = (PSYSTEM_RESOURCE_FRAMEBUFFER)SystemResource;

    /* Check if bootloader provided a framebuffer address */
    if(!FrameBufferResource->Header.VirtualAddress)
    {
        /* Display probably not initialized */
        return STATUS_DEVICE_NOT_READY;
    }

    /* Check if bootloader provided a custom font */
    if(FrameBufferResource->Font)
    {
        /* Use custom font */
        HlpFrameBufferData.Font = FrameBufferResource->Font;
    }
    else
    {
        /* Use default font */
        HlpFrameBufferData.Font = (PVOID)&XtFbDefaultFont;
    }

    /* Save framebuffer information and mark display as initialized */
    HlpFrameBufferData.Address = FrameBufferResource->Header.VirtualAddress;
    HlpFrameBufferData.Width = FrameBufferResource->Width;
    HlpFrameBufferData.Height = FrameBufferResource->Height;
    HlpFrameBufferData.BytesPerPixel = FrameBufferResource->BitsPerPixel / 8;
    HlpFrameBufferData.PixelsPerScanLine = FrameBufferResource->PixelsPerScanLine;
    HlpFrameBufferData.Pitch = FrameBufferResource->Pitch;
    HlpFrameBufferData.Pixels.BlueShift = FrameBufferResource->Pixels.BlueShift;
    HlpFrameBufferData.Pixels.BlueSize = FrameBufferResource->Pixels.BlueSize;
    HlpFrameBufferData.Pixels.GreenShift = FrameBufferResource->Pixels.GreenShift;
    HlpFrameBufferData.Pixels.GreenSize = FrameBufferResource->Pixels.GreenSize;
    HlpFrameBufferData.Pixels.RedShift = FrameBufferResource->Pixels.RedShift;
    HlpFrameBufferData.Pixels.RedSize = FrameBufferResource->Pixels.RedSize;
    HlpFrameBufferData.Pixels.ReservedShift = FrameBufferResource->Pixels.ReservedShift;
    HlpFrameBufferData.Pixels.ReservedSize = FrameBufferResource->Pixels.ReservedSize;
    HlpFrameBufferData.Initialized = TRUE;

    /* Clear screen */
    HlClearScreen(0x00000000);

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Sets the scrollable region of the screen and calculates character dimensions.
 *
 * @param Left
 *        Supplies the left pixel coordinate of the scroll region.
 *
 * @param Top
 *        Supplies the top pixel coordinate of the scroll region.
 *
 * @param Right
 *        Supplies the right pixel coordinate of the scroll region.
 *
 * @param Bottom
 *        Supplies the bottom pixel coordinate of the scroll region.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlInitializeScrollRegion(IN ULONG Left,
                         IN ULONG Top,
                         IN ULONG Right,
                         IN ULONG Bottom,
                         IN ULONG FontColor)
{
    PSSFN_FONT_HEADER FbFont;
    PCHAR PixelAddress;

    /* Make sure frame buffer is already initialized */
    if(HlpFrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Store pixel coordinates of the scroll region */
    HlpScrollRegionData.Left = Left;
    HlpScrollRegionData.Top = Top;
    HlpScrollRegionData.Right = Right;
    HlpScrollRegionData.Bottom = Bottom;

    /* Get font information */
    FbFont = (PSSFN_FONT_HEADER)HlpFrameBufferData.Font;

    /* Validate font information */
    if(FbFont && FbFont->Width > 0 && FbFont->Height > 0)
    {
        /* Calculate character dimensions */
        HlpScrollRegionData.WidthInChars = (Right - Left) / FbFont->Width;
        HlpScrollRegionData.HeightInChars = (Bottom - Top) / FbFont->Height;

        /* Ensure the bottom of the scroll region is an exact multiple of the font height */
        HlpScrollRegionData.Bottom = HlpScrollRegionData.Top + (HlpScrollRegionData.HeightInChars * FbFont->Height);
    }
    else
    {
        /* Fallback to 0 if font info is not available or invalid */
        HlpScrollRegionData.WidthInChars = 0;
        HlpScrollRegionData.HeightInChars = 0;
    }

    /* Initialize cursor position and font color */
    HlpScrollRegionData.CursorX = HlpScrollRegionData.Left;
    HlpScrollRegionData.CursorY = HlpScrollRegionData.Top;
    HlpScrollRegionData.TextColor = FontColor;

    /* Get the background color by reading the pixel at the top-left corner of the scroll region */
    PixelAddress = (PCHAR)HlpFrameBufferData.Address + (Top * HlpFrameBufferData.Pitch) +
                          (Left * HlpFrameBufferData.BytesPerPixel);
    HlpScrollRegionData.BackgroundColor = *((PULONG)PixelAddress);
}

/**
 * Draws a character on the framebuffer at the given position and color using the SSFN font.
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
HlpDrawCharacter(IN ULONG PositionX,
                 IN ULONG PositionY,
                 IN ULONG Color,
                 IN WCHAR WideCharacter)
{
    UINT CurrentFragment, Glyph, GlyphLimit, Index, Line, Mapping;
    PUCHAR Character, CharacterMapping, Fragment;
    UINT_PTR GlyphPixel, Pixel;
    PSSFN_FONT_HEADER FbFont;
    ULONG FontColor;

    /* Make sure frame buffer is already initialized */
    if(HlpFrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

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
    GlyphPixel = (UINT_PTR)HlpFrameBufferData.Address + PositionY * HlpFrameBufferData.Pitch +
                           PositionX * HlpFrameBufferData.BytesPerPixel;
    FontColor = HlpRGBColor(Color);

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
                Pixel += HlpFrameBufferData.BytesPerPixel;
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
HlpDrawPixel(IN ULONG PositionX,
             IN ULONG PositionY,
             IN ULONG Color)
{
    PCHAR PixelAddress;

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

    /* Calculate the address of the pixel in the frame buffer memory */
    PixelAddress = (PCHAR)HlpFrameBufferData.Address + (PositionY * HlpFrameBufferData.Pitch) +
                          (PositionX * HlpFrameBufferData.BytesPerPixel);

    /* Set the color of the pixel by writing to the corresponding memory location */
    *((PULONG)PixelAddress) = HlpRGBColor(Color);
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
HlpRGBColor(IN ULONG Color)
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

/**
 * Scrolls the content of the scroll region up by one line.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlpScrollRegion(VOID)
{
    PCHAR Destination, Source;
    PSSFN_FONT_HEADER FbFont;
    ULONG Line, PositionX;
    ULONG LineBytes;
    PULONG Pixel;

    /* Make sure frame buffer is already initialized */
    if(HlpFrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Get font information */
    FbFont = (PSSFN_FONT_HEADER)HlpFrameBufferData.Font;

    /* Calculate bytes per line in the scroll region */
    LineBytes = (HlpScrollRegionData.Right - HlpScrollRegionData.Left) * HlpFrameBufferData.BytesPerPixel;

    /* Scroll up each scan line in the scroll region */
    for(Line = HlpScrollRegionData.Top; Line < HlpScrollRegionData.Bottom - FbFont->Height; Line++)
    {
        Destination = (PCHAR)HlpFrameBufferData.Address + Line * HlpFrameBufferData.Pitch +
                      HlpScrollRegionData.Left * HlpFrameBufferData.BytesPerPixel;

        /* The source is one full text line (FbFont->Height) below the destination */
        Source = (PCHAR)HlpFrameBufferData.Address + (Line + FbFont->Height) * HlpFrameBufferData.Pitch +
                 HlpScrollRegionData.Left * HlpFrameBufferData.BytesPerPixel;

        /* Move each scan line in the scroll region up */
        RtlMoveMemory(Destination, Source, LineBytes);
    }

    /* Clear the last text line */
    for(Line = HlpScrollRegionData.Bottom - FbFont->Height; Line < HlpScrollRegionData.Bottom; Line++)
    {
        /* Get pointer to the start of the scan line to clear */
        Pixel = (PULONG)((PCHAR)HlpFrameBufferData.Address + Line * HlpFrameBufferData.Pitch +
                         HlpScrollRegionData.Left * HlpFrameBufferData.BytesPerPixel);

        /* Clear each pixel in the scan line with the background color */
        for(PositionX = 0; PositionX < (HlpScrollRegionData.Right - HlpScrollRegionData.Left); PositionX++)
        {
            Pixel[PositionX] = HlpScrollRegionData.BackgroundColor;
        }
    }
}
