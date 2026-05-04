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
    ULONG BackgroundColor, PositionY;
    PCHAR CurrentLine, TargetBuffer;
    UCHAR FillByte;

    /* Make sure frame buffer is already initialized */
    if(FrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Convert background color */
    BackgroundColor = GetRGBColor(Color);

    /* Extract the lower byte for SetMemory */
    FillByte = (UCHAR)(BackgroundColor & 0xFF);

    /* Determine target buffer */
    TargetBuffer = (ScreenShadowBuffer != NULLPTR) ? (PCHAR)ScreenShadowBuffer : (PCHAR)FrameBufferData.Address;
    CurrentLine = TargetBuffer;

    /* Fill the screen with the specified color */
    for(PositionY = 0; PositionY < FrameBufferData.Height; PositionY++, CurrentLine += FrameBufferData.Pitch)
    {
        /* Fill the current scanline with the background color byte */
        RTL::Memory::SetMemory(CurrentLine, FillByte, FrameBufferData.Width * FrameBufferData.BytesPerPixel);
    }

    /* Check if Shadow Buffer is active */
    if(ScreenShadowBuffer != NULLPTR)
    {
        /* Flush changes to VRAM */
        RTL::Memory::CopyMemory(FrameBufferData.Address, ScreenShadowBuffer,
                                FrameBufferData.Pitch * FrameBufferData.Height);
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
    ULONG CharacterX, CharacterY;
    PSSFN_FONT_HEADER FbFont;
    BOOLEAN VisibleCharacter;

    /* Make sure frame buffer is already initialized */
    if(FrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return STATUS_DEVICE_NOT_READY;
    }

    /* Get font information */
    FbFont = (PSSFN_FONT_HEADER)FrameBufferData.Font;

    /* Assume invisible character */
    VisibleCharacter = FALSE;

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
            if(ScrollRegionData.CursorX >= ScrollRegionData.Right)
            {
                ScrollRegionData.CursorX = ScrollRegionData.Left;
                ScrollRegionData.CursorY += FbFont->Height;
            }
            break;
        default:
            /* Save cursor position */
            CharacterX = ScrollRegionData.CursorX;
            CharacterY = ScrollRegionData.CursorY;

            /* Draw the character to RAM and mark it as visible */
            DrawCharacter(CharacterX, CharacterY, ScrollRegionData.TextColor, Character);
            VisibleCharacter = TRUE;

            /* Advance cursor */
            ScrollRegionData.CursorX += FbFont->Width;

            /* Check if cursor reached end of line */
            if(ScrollRegionData.CursorX >= ScrollRegionData.Right)
            {
                /* Reset cursor to the left margin and advance to the next line */
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
    else if(VisibleCharacter == TRUE)
    {
        /* Flush visible character to VRAM */
        UpdateScreenRegion(CharacterX, CharacterY,
                           CharacterX + FbFont->Width,
                           CharacterY + FbFont->Height);
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
    ULONG FontColor, GlyphOffset, PixelOffset;
    PSSFN_FONT_HEADER FbFont;

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
            /* There is a glyph for this character, check if it matches */
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
    GlyphOffset = (PositionY * FrameBufferData.Pitch) + (PositionX * FrameBufferData.BytesPerPixel);
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
        GlyphOffset += (CharacterMapping[1] - Mapping) * FrameBufferData.Pitch;
        Mapping = CharacterMapping[1];

        /* Extract glyph data from fragments table and advance */
        Glyph = ((Fragment[0] & 0x1F) + 1) << 3;
        GlyphLimit = Fragment[1] + 1;
        Fragment += 2;

        /* Look for kerning group for next code point */
        CurrentFragment = 1;
        while(GlyphLimit--)
        {
            /* Set the initial pixel offset for the current glyph fragment */
            PixelOffset = GlyphOffset;
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
                    /* Route the pixel draw operation to the active buffer */
                    if(ScreenShadowBuffer != NULLPTR)
                    {
                        /* Draw glyph pixel to Shadow Buffer */
                        *((PULONG)((PCHAR)ScreenShadowBuffer + PixelOffset)) = FontColor;
                    }
                    else
                    {
                        /* Draw glyph pixel directly to VRAM */
                        *((PULONG)((PCHAR)FrameBufferData.Address + PixelOffset)) = FontColor;
                    }
                }

                /* Advance pixel pointer */
                PixelOffset += FrameBufferData.BytesPerPixel;
                CurrentFragment <<= 1;
            }

            /* Advance to next line and increase mapping */
            GlyphOffset += FrameBufferData.Pitch;
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
    ULONG Offset;

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
    Offset = (PositionY * FrameBufferData.Pitch) + (PositionX * FrameBufferData.BytesPerPixel);

    /* Route the pixel draw operation to the active buffer */
    if(ScreenShadowBuffer != NULLPTR)
    {
        /* Set the color of the pixel by writing to the corresponding memory location (RAM) */
        *((PULONG)((PCHAR)ScreenShadowBuffer + Offset)) = GetRGBColor(Color);
    }
    else
    {
        /* Set the color of the pixel by writing to the corresponding memory location (VRAM) */
        *((PULONG)((PCHAR)FrameBufferData.Address + Offset)) = GetRGBColor(Color);
    }
}

/**
 * Enables the Shadow Buffer (Double Buffering) for high-performance rendering.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
HL::FrameBuffer::EnableShadowBuffer(VOID)
{
    ULONG FrameBufferSize;
    XTSTATUS Status;

    /* Check if the shadow buffer is already enabled */
    if(ScreenShadowBuffer != NULLPTR)
    {
        /* Nothing to do, return success */
        return STATUS_SUCCESS;
    }

    /* Make sure frame buffer is already initialized */
    if(FrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return STATUS_DEVICE_NOT_READY;
    }

    /* Calculate the total size of the framebuffer */
    FrameBufferSize = FrameBufferData.Pitch * FrameBufferData.Height;

    /* Allocate non-paged memory for the shadow buffer */
    Status = MM::Allocator::AllocatePool(NonPagedPool, FrameBufferSize,
                                         &ScreenShadowBuffer, SIGNATURE32('F', 'B', 'U', 'F'));
    if(Status != STATUS_SUCCESS)
    {
        /* Allocation failed, return status code */
        ScreenShadowBuffer = NULLPTR;
        return Status;
    }

    /* Synchronize the newly allocated shadow buffer with the current on-screen contents */
    RTL::Memory::CopyMemory(ScreenShadowBuffer, FrameBufferData.Address, FrameBufferSize);

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
    PCHAR TargetBuffer, Destination, Source;
    ULONG Line, PositionX, LineBytes;
    PSSFN_FONT_HEADER FbFont;
    PULONG Pixel;

    /* Make sure frame buffer is already initialized */
    if(FrameBufferData.Initialized == FALSE)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Retrieve font metrics and calculate line properties for the scroll operation */
    FbFont = (PSSFN_FONT_HEADER)FrameBufferData.Font;
    LineBytes = (ScrollRegionData.Right - ScrollRegionData.Left) * FrameBufferData.BytesPerPixel;
    TargetBuffer = (ScreenShadowBuffer != NULLPTR) ? (PCHAR)ScreenShadowBuffer : (PCHAR)FrameBufferData.Address;

    /* Process every line in the scroll region */
    for(Line = ScrollRegionData.Top; Line < ScrollRegionData.Bottom; Line++)
    {
        /* Calculate destination address for the current line */
        Destination = TargetBuffer + (Line * FrameBufferData.Pitch) +
                      (ScrollRegionData.Left * FrameBufferData.BytesPerPixel);

        /* Check if the current line needs to be copied from below or cleared */
        if(Line < ScrollRegionData.Bottom - FbFont->Height)
        {
            /* Copy the line from below */
            Source = Destination + (FbFont->Height * FrameBufferData.Pitch);
            RTL::Memory::CopyMemory(Destination, Source, LineBytes);
        }
        else
        {
            /* Fill the bottom line(s) with the background color */
            Pixel = (PULONG)Destination;
            for(PositionX = 0; PositionX < (ScrollRegionData.Right - ScrollRegionData.Left); PositionX++)
            {
                /* Overwrite the pixel with the background color */
                Pixel[PositionX] = ScrollRegionData.BackgroundColor;
            }
        }
    }

    /* Flush changes to VRAM if Shadow Buffer was used */
    if(ScreenShadowBuffer != NULLPTR)
    {
        /* Flush the updated scroll region to VRAM */
        UpdateScreenRegion(ScrollRegionData.Left,
                           ScrollRegionData.Top,
                           ScrollRegionData.Right,
                           ScrollRegionData.Bottom);
    }
}

/**
 * Flushes the current content of the Shadow Buffer to the visible FrameBuffer (VRAM).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::FrameBuffer::UpdateScreen(VOID)
{
    /* Make sure framebuffer is already initialized and shadow buffer is ready */
    if(FrameBufferData.Initialized == FALSE || ScreenShadowBuffer == NULLPTR)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Flush RAM to VRAM */
    RTL::Memory::CopyMemory(FrameBufferData.Address,
                            ScreenShadowBuffer,
                            FrameBufferData.Pitch * FrameBufferData.Height);
}

/**
 * Flushes a specific rectangular region from the Shadow Buffer to the visible FrameBuffer (VRAM).
 *
 * @param Left
 *        Supplies the left pixel coordinate of the region to update.
 *
 * @param Top
 *        Supplies the top pixel coordinate of the region to update.
 *
 * @param Right
 *        Supplies the right pixel coordinate of the region to update.
 *
 * @param Bottom
 *        Supplies the bottom pixel coordinate of the region to update.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HL::FrameBuffer::UpdateScreenRegion(IN ULONG Left,
                                    IN ULONG Top,
                                    IN ULONG Right,
                                    IN ULONG Bottom)
{
    ULONG Line, LineBytes;
    PCHAR Source, Destination;

    /* Make sure framebuffer is already initialized and shadow buffer is ready */
    if(FrameBufferData.Initialized == FALSE || ScreenShadowBuffer == NULLPTR)
    {
        /* Unable to operate on non-initialized frame buffer */
        return;
    }

    /* Make sure parameters are valid to prevent memory corruption */
    if(Left >= Right || Top >= Bottom || Right > FrameBufferData.Width || Bottom > FrameBufferData.Height)
    {
        /* Invalid region coordinates provided */
        return;
    }

    /* Calculate the width of the region */
    LineBytes = (Right - Left) * FrameBufferData.BytesPerPixel;

    /* Copy the specified region line by line */
    for(Line = Top; Line < Bottom; Line++)
    {
        /* Calculate the source address in the shadow buffer */
        Source = (PCHAR)ScreenShadowBuffer +
                 (Line * FrameBufferData.Pitch) +
                 (Left * FrameBufferData.BytesPerPixel);

        /* Calculate the destination address in the VRAM */
        Destination = (PCHAR)FrameBufferData.Address +
                      (Line * FrameBufferData.Pitch) +
                      (Left * FrameBufferData.BytesPerPixel);

        /* Flush RAM to VRAM */
        RTL::Memory::CopyMemory(Destination, Source, LineBytes);
    }
}
