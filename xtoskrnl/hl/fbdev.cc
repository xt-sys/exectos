/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/fbdev.cc
 * DESCRIPTION:     FrameBuffer support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>
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
HL::FrameBuffer::ClearScreen(IN ULONG Color)
{
    ULONG PositionX, PositionY;
    ULONG BackgroundColor;
    PCHAR CurrentLine;
    PULONG Pixel;

    /* Make sure frame buffer is already initialized */
    if(FrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Convert background color and get pointer to frame buffer */
    BackgroundColor = GetRGBColor(Color);
    CurrentLine = (PCHAR)FrameBufferData.Address;

    /* Fill the screen with the specified color */
    for(PositionY = 0; PositionY < FrameBufferData.Height; PositionY++, CurrentLine += FrameBufferData.Pitch)
    {
        /* Fill the current line with the specified color */
        Pixel = (PULONG)CurrentLine;
        for(PositionX = 0; PositionX < FrameBufferData.Width; PositionX++)
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
HL::FrameBuffer::DisplayCharacter(IN WCHAR Character)
{
    PSSFN_FONT_HEADER FbFont;

    /* Make sure frame buffer is already initialized */
    if(FrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return STATUS_DEVICE_NOT_READY;
    }

    /* Get font information */
    FbFont = (PSSFN_FONT_HEADER)FrameBufferData.Font;

    /* Handle special characters */
    switch(Character)
    {
        case L'\n':
            /* Move cursor to the beginning of the next line */
            ScrollRegionData.CursorX = ScrollRegionData.Left;
            ScrollRegionData.CursorY += FbFont->Height;
            break;
        case L'\t':
            /* Move cursor to the next tab stop */
            ScrollRegionData.CursorX += (8 - (ScrollRegionData.CursorX - ScrollRegionData.Left) / FbFont->Width % 8) * FbFont->Width;
            if (ScrollRegionData.CursorX >= ScrollRegionData.Right)
            {
                ScrollRegionData.CursorX = ScrollRegionData.Left;
                ScrollRegionData.CursorY += FbFont->Height;
            }
            break;
        default:
            /* Draw the character */
            DrawCharacter(ScrollRegionData.CursorX, ScrollRegionData.CursorY, ScrollRegionData.TextColor, Character);

            /* Advance cursor */
            ScrollRegionData.CursorX += FbFont->Width;

            /* Check if cursor reached end of line */
            if(ScrollRegionData.CursorX >= ScrollRegionData.Right)
            {
                ScrollRegionData.CursorX = ScrollRegionData.Left;
                ScrollRegionData.CursorY += FbFont->Height;
            }
            break;
    }

    /* Check if cursor reached end of scroll region */
    if(ScrollRegionData.CursorY >= ScrollRegionData.Bottom)
    {
        /* Scroll one line up */
        ScrollRegion();
        ScrollRegionData.CursorY = ScrollRegionData.Bottom - FbFont->Height;
    }

    /* Return success */
    return STATUS_SUCCESS;
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
HL::FrameBuffer::DrawCharacter(IN ULONG PositionX,
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
    if(FrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Get pointers to font data */
    FbFont = (PSSFN_FONT_HEADER)FrameBufferData.Font;
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
    GlyphPixel = (UINT_PTR)FrameBufferData.Address + PositionY * FrameBufferData.Pitch +
                           PositionX * FrameBufferData.BytesPerPixel;
    FontColor = GetRGBColor(Color);

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
        GlyphPixel += (CharacterMapping[1] - Mapping) * FrameBufferData.Pitch;
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
                Pixel += FrameBufferData.BytesPerPixel;
                CurrentFragment <<= 1;
            }

            /* Advance to next line and increase mapping */
            GlyphPixel += FrameBufferData.Pitch;
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
HL::FrameBuffer::DrawPixel(IN ULONG PositionX,
                           IN ULONG PositionY,
                           IN ULONG Color)
{
    PCHAR PixelAddress;

    /* Make sure frame buffer is already initialized */
    if(FrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Make sure point is not offscreen */
    if(PositionX >= FrameBufferData.Width || PositionY >= FrameBufferData.Height || Color > 0xFFFFFFFF)
    {
        /* Invalid pixel position or color given */
        return;
    }

    /* Calculate the address of the pixel in the frame buffer memory */
    PixelAddress = (PCHAR)FrameBufferData.Address + (PositionY * FrameBufferData.Pitch) +
                          (PositionX * FrameBufferData.BytesPerPixel);

    /* Set the color of the pixel by writing to the corresponding memory location */
    *((PULONG)PixelAddress) = GetRGBColor(Color);
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
HL::FrameBuffer::GetFrameBufferResolution(OUT PULONG Width,
                                          OUT PULONG Height)
{
    *Width = FrameBufferData.Width;
    *Height = FrameBufferData.Height;
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
HL::FrameBuffer::GetRGBColor(IN ULONG Color)
{
    USHORT Blue, Green, Red, Reserved;

    /* Extract color components from (A)RGB value */
    Blue = (USHORT)(Color & 0xFF);
    Green = (USHORT)((Color >> 8) & 0xFF);
    Red = (USHORT)((Color >> 16) & 0xFF);
    Reserved = (USHORT)((Color >> 24) & 0xFF);

    /* Return color in FrameBuffer pixel format */
    return (ULONG)((Blue << FrameBufferData.Pixels.BlueShift) | (Green << FrameBufferData.Pixels.GreenShift) |
                   (Red << FrameBufferData.Pixels.RedShift) | (Reserved << FrameBufferData.Pixels.ReservedShift));
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
HL::FrameBuffer::InitializeFrameBuffer(VOID)
{
    PSYSTEM_RESOURCE_FRAMEBUFFER FrameBufferResource;
    PSYSTEM_RESOURCE_HEADER SystemResource;
    XTSTATUS Status;

    /* Check if display already initialized */
    if(FrameBufferData.Initialized)
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
        FrameBufferData.Font = FrameBufferResource->Font;
    }
    else
    {
        /* Use default font */
        FrameBufferData.Font = (PVOID)&XtFbDefaultFont;
    }

    /* Save framebuffer information and mark display as initialized */
    FrameBufferData.Address = FrameBufferResource->Header.VirtualAddress;
    FrameBufferData.Width = FrameBufferResource->Width;
    FrameBufferData.Height = FrameBufferResource->Height;
    FrameBufferData.BytesPerPixel = FrameBufferResource->BitsPerPixel / 8;
    FrameBufferData.PixelsPerScanLine = FrameBufferResource->PixelsPerScanLine;
    FrameBufferData.Pitch = FrameBufferResource->Pitch;
    FrameBufferData.Pixels.BlueShift = FrameBufferResource->Pixels.BlueShift;
    FrameBufferData.Pixels.BlueSize = FrameBufferResource->Pixels.BlueSize;
    FrameBufferData.Pixels.GreenShift = FrameBufferResource->Pixels.GreenShift;
    FrameBufferData.Pixels.GreenSize = FrameBufferResource->Pixels.GreenSize;
    FrameBufferData.Pixels.RedShift = FrameBufferResource->Pixels.RedShift;
    FrameBufferData.Pixels.RedSize = FrameBufferResource->Pixels.RedSize;
    FrameBufferData.Pixels.ReservedShift = FrameBufferResource->Pixels.ReservedShift;
    FrameBufferData.Pixels.ReservedSize = FrameBufferResource->Pixels.ReservedSize;
    FrameBufferData.Initialized = TRUE;

    /* Clear screen */
    ClearScreen(0x00000000);

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
HL::FrameBuffer::InitializeScrollRegion(IN ULONG Left,
                                        IN ULONG Top,
                                        IN ULONG Right,
                                        IN ULONG Bottom,
                                        IN ULONG FontColor)
{
    PSSFN_FONT_HEADER FbFont;
    PCHAR PixelAddress;

    /* Make sure frame buffer is already initialized */
    if(FrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Store pixel coordinates of the scroll region */
    ScrollRegionData.Left = Left;
    ScrollRegionData.Top = Top;
    ScrollRegionData.Right = Right;
    ScrollRegionData.Bottom = Bottom;

    /* Get font information */
    FbFont = (PSSFN_FONT_HEADER)FrameBufferData.Font;

    /* Validate font information */
    if(FbFont && FbFont->Width > 0 && FbFont->Height > 0)
    {
        /* Calculate character dimensions */
        ScrollRegionData.WidthInChars = (Right - Left) / FbFont->Width;
        ScrollRegionData.HeightInChars = (Bottom - Top) / FbFont->Height;

        /* Ensure the bottom of the scroll region is an exact multiple of the font height */
        ScrollRegionData.Bottom = ScrollRegionData.Top + (ScrollRegionData.HeightInChars * FbFont->Height);
    }
    else
    {
        /* Fallback to 0 if font info is not available or invalid */
        ScrollRegionData.WidthInChars = 0;
        ScrollRegionData.HeightInChars = 0;
    }

    /* Initialize cursor position and font color */
    ScrollRegionData.CursorX = ScrollRegionData.Left;
    ScrollRegionData.CursorY = ScrollRegionData.Top;
    ScrollRegionData.TextColor = FontColor;

    /* Get the background color by reading the pixel at the top-left corner of the scroll region */
    PixelAddress = (PCHAR)FrameBufferData.Address + (Top * FrameBufferData.Pitch) +
                          (Left * FrameBufferData.BytesPerPixel);
    ScrollRegionData.BackgroundColor = *((PULONG)PixelAddress);
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
HL::FrameBuffer::ScrollRegion(VOID)
{
    PCHAR Destination, Source;
    PSSFN_FONT_HEADER FbFont;
    ULONG Line, PositionX;
    ULONG LineBytes;
    PULONG Pixel;

    /* Make sure frame buffer is already initialized */
    if(FrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Get font information */
    FbFont = (PSSFN_FONT_HEADER)FrameBufferData.Font;

    /* Calculate bytes per line in the scroll region */
    LineBytes = (ScrollRegionData.Right - ScrollRegionData.Left) * FrameBufferData.BytesPerPixel;

    /* Scroll up each scan line in the scroll region */
    for(Line = ScrollRegionData.Top; Line < ScrollRegionData.Bottom - FbFont->Height; Line++)
    {
        Destination = (PCHAR)FrameBufferData.Address + Line * FrameBufferData.Pitch +
                      ScrollRegionData.Left * FrameBufferData.BytesPerPixel;

        /* The source is one full text line (FbFont->Height) below the destination */
        Source = (PCHAR)FrameBufferData.Address + (Line + FbFont->Height) * FrameBufferData.Pitch +
                 ScrollRegionData.Left * FrameBufferData.BytesPerPixel;

        /* Move each scan line in the scroll region up */
        RTL::Memory::MoveMemory(Destination, Source, LineBytes);
    }

    /* Clear the last text line */
    for(Line = ScrollRegionData.Bottom - FbFont->Height; Line < ScrollRegionData.Bottom; Line++)
    {
        /* Get pointer to the start of the scan line to clear */
        Pixel = (PULONG)((PCHAR)FrameBufferData.Address + Line * FrameBufferData.Pitch +
                         ScrollRegionData.Left * FrameBufferData.BytesPerPixel);

        /* Clear each pixel in the scan line with the background color */
        for(PositionX = 0; PositionX < (ScrollRegionData.Right - ScrollRegionData.Left); PositionX++)
        {
            Pixel[PositionX] = ScrollRegionData.BackgroundColor;
        }
    }
}


/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTCLINK
XTCDECL
XTSTATUS
HlDisplayCharacter(IN WCHAR Character)
{
    return HL::FrameBuffer::DisplayCharacter(Character);
}

/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTCLINK
XTAPI
VOID
HlGetFrameBufferResolution(OUT PULONG Width,
                           OUT PULONG Height)
{
    HL::FrameBuffer::GetFrameBufferResolution(Width, Height);
}

/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTCLINK
XTAPI
XTSTATUS
HlInitializeFrameBuffer(VOID)
{
    return HL::FrameBuffer::InitializeFrameBuffer();
}

/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTCLINK
XTAPI
VOID
HlInitializeScrollRegion(IN ULONG Left,
                         IN ULONG Top,
                         IN ULONG Right,
                         IN ULONG Bottom,
                         IN ULONG FontColor)
{
    HL::FrameBuffer::InitializeScrollRegion(Left, Top, Right, Bottom, FontColor);
}
