/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/framebuf/framebuf.c
 * DESCRIPTION:     EFI framebuffer support module for XTLDR
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <framebuf.h>


/* PE/COFF_O module information */
MODULE_AUTHOR(L"Rafal Kupiec <belliash@codingworkshop.eu.org>");
MODULE_DESCRIPTION(L"EFI FB (FrameBuffer) support");
MODULE_LICENSE(L"GPLv3");
MODULE_VERSION(L"0.2");

/**
 * Provides an EFI Frame Buffer protocol driver name used for initialization.
 *
 * @param Protocol
 *        Supplies a pointer to the memory area where framebuffer driver information will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
FbGetDisplayDriver(OUT PEFI_GRAPHICS_PROTOCOL Protocol)
{
        /* Check if framebuffer is initialized */
    if(!FbpDisplayInfo.Initialized)
    {
        /* Return error if framebuffer is not initialized */
        return STATUS_EFI_NOT_READY;
    }

    /* Copy framebuffer driver information */
    *Protocol = FbpDisplayInfo.Protocol;

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Returns information about EFI Frame Buffer.
 *
 * @param FbInfo
 *        Supplies a pointer to the memory area where framebuffer information will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
FbGetDisplayInformation(OUT PEFI_PHYSICAL_ADDRESS FrameBufferBase,
                        OUT PULONG_PTR FrameBufferSize,
                        OUT PXTBL_FRAMEBUFFER_MODE_INFORMATION ModeInfo)
{
    /* Check if framebuffer is initialized */
    if(!FbpDisplayInfo.Initialized)
    {
        /* Return error if framebuffer is not initialized */
        return STATUS_EFI_NOT_READY;
    }

    /* Set basic framebuffer information */
    *FrameBufferBase = FbpDisplayInfo.FrameBufferBase;
    *FrameBufferSize = FbpDisplayInfo.FrameBufferSize;

    /* Set framebuffer mode information */
    ModeInfo->Width = FbpDisplayInfo.ModeInfo.Width;
    ModeInfo->Height = FbpDisplayInfo.ModeInfo.Height;
    ModeInfo->Depth = FbpDisplayInfo.ModeInfo.Depth;
    ModeInfo->RefreshRate = FbpDisplayInfo.ModeInfo.RefreshRate;
    ModeInfo->BitsPerPixel = FbpDisplayInfo.ModeInfo.BitsPerPixel;
    ModeInfo->BytesPerPixel = FbpDisplayInfo.ModeInfo.BytesPerPixel;
    ModeInfo->PixelsPerScanLine = FbpDisplayInfo.ModeInfo.PixelsPerScanLine;
    ModeInfo->Pitch = FbpDisplayInfo.ModeInfo.Pitch;
    ModeInfo->PixelFormat = FbpDisplayInfo.ModeInfo.PixelFormat;
    ModeInfo->PixelInformation = FbpDisplayInfo.ModeInfo.PixelInformation;

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Determines the preferred (native) screen resolution from EDID. This works only with GOP.
 *
 * @param PreferredWidth
 *        Supplies a pointer to the memory area where preferred screen width will be stored.
 *
 * @param PreferredHeight
 *        Supplies a pointer to the memory area where preferred screen height will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
FbGetPreferredScreenResolution(OUT PUINT PreferredWidth,
                               OUT PUINT PreferredHeight)
{
    EFI_GUID GopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GUID EdidGuid = EFI_EDID_ACTIVE_PROTOCOL_GUID;
    PEFI_EDID_ACTIVE_PROTOCOL ActiveEdid;
    EFI_STATUS Status;

    /* Check if framebuffer is initialized */
    if(!FbpDisplayInfo.Initialized)
    {
        /* Framebuffer not ready to use EDID protocol */
        return STATUS_EFI_NOT_READY;
    }

    /* Check if GOP device driver is used */
    if(FbpDisplayInfo.Protocol != GOP)
    {
        /* Unsupported device driver */
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Open EDID protocol */
    Status = XtLdrProtocol->Protocol.OpenHandle(FbpDisplayInfo.Handle, (PVOID *)&ActiveEdid, &EdidGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open EDID protocol, close GOP protocol and return */
        XtLdrProtocol->Protocol.Close(FbpDisplayInfo.Handle, &GopGuid);
        return Status;
    }

    /* Return preferred screen resolution */
    *PreferredWidth = ActiveEdid->Edid[0x38] | ((ActiveEdid->Edid[0x3A] & 0xF0) << 4);
    *PreferredHeight = ActiveEdid->Edid[0x3B] | ((ActiveEdid->Edid[0x3D] & 0xF0) << 4);

    /* Close EDID & GOP protocols */
    XtLdrProtocol->Protocol.Close(FbpDisplayInfo.Handle, &EdidGuid);

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Initializes FrameBuffer device on GOP and UGA compatible adapters.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
FbInitializeDisplay()
{
    EFI_GUID GopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GUID UgaGuid = EFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL_GUID;
    PEFI_GRAPHICS_OUTPUT_MODE_INFORMATION GopModeInfo;
    UINT Depth, QueryMode, Refresh;
    UINT_PTR InfoSize;
    EFI_STATUS Status;

    /* Check if framebuffer already initialized */
    if(!FbpDisplayInfo.Initialized)
    {
        /* Print debug message */
        XtLdrProtocol->Debug.Print(L"Initializing framebuffer device\n");

        /* Attempt to open EFI GOP protocol */
        Status = XtLdrProtocol->Protocol.Open(&FbpDisplayInfo.Handle, (PVOID*)&FbpDisplayInfo.Driver.Gop, &GopGuid);

        /* Check if Graphics Output Protocol (GOP) is available */
        if(Status == STATUS_EFI_SUCCESS)
        {
            /* Check if there are any video modes available */
            if(FbpDisplayInfo.Driver.Gop->Mode->MaxMode == 0)
            {
                /* No video modes available */
                XtLdrProtocol->Debug.Print(L"ERROR: No GOP video mode available\n");

                /* Close GOP protocol and return error */
                XtLdrProtocol->Protocol.Close(FbpDisplayInfo.Handle, &GopGuid);
                return STATUS_EFI_UNSUPPORTED;
            }

            /* Query current graphics mode */
            QueryMode = FbpDisplayInfo.Driver.Gop->Mode == NULL ? 0 : FbpDisplayInfo.Driver.Gop->Mode->Mode;
            Status = FbpDisplayInfo.Driver.Gop->QueryMode(FbpDisplayInfo.Driver.Gop, QueryMode, &InfoSize, &GopModeInfo);
            if(Status == STATUS_EFI_NOT_STARTED)
            {
                /* Set the mode to circumvent buggy UEFI firmware */
                Status = FbpDisplayInfo.Driver.Gop->SetMode(FbpDisplayInfo.Driver.Gop, 0);
            }
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Unable to query GOP modes */
                XtLdrProtocol->Debug.Print(L"ERROR: Failed to get GOP native mode (Status Code: 0x%zX)\n");

                /* Close GOP protocol and return error */
                XtLdrProtocol->Protocol.Close(FbpDisplayInfo.Handle, &GopGuid);
                return STATUS_EFI_UNSUPPORTED;
            }

            /* Store frame buffer base address and protocol used */
            FbpDisplayInfo.FrameBufferBase = FbpDisplayInfo.Driver.Gop->Mode->FrameBufferBase;
            FbpDisplayInfo.DefaultMode = FbpDisplayInfo.Driver.Gop->Mode->Mode;
            FbpDisplayInfo.Protocol = GOP;

            /* Get current mode information */
            Status = FbpGetModeInfo();
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Unable to get mode information */
                XtLdrProtocol->Debug.Print(L"ERROR: Failed to get GOP mode information (Status Code: 0x%zX)\n");

                /* Close GOP protocol and return error */
                XtLdrProtocol->Protocol.Close(FbpDisplayInfo.Handle, &GopGuid);
                return STATUS_EFI_UNSUPPORTED;
            }

            /* Found GOP */
            XtLdrProtocol->Debug.Print(L"Found EFI-GOP compatible display adapter @ %P (%zu bytes)\n",
                                       FbpDisplayInfo.FrameBufferBase, FbpDisplayInfo.FrameBufferSize);

            /* Close GOP protocol */
            Status = XtLdrProtocol->Protocol.Close(FbpDisplayInfo.Handle, &GopGuid);
        }
        else
        {
            /* GOP is unavailable, attempt to open UGA protocol */
            Status = XtLdrProtocol->Protocol.Open(&FbpDisplayInfo.Handle, (PVOID*)&FbpDisplayInfo.Driver.Uga, &UgaGuid);

            /* Check if Universal Graphics Adapter (UGA) is available */
            if(Status == STATUS_EFI_SUCCESS)
            {
                /* Get current video mode */
                Status = FbpDisplayInfo.Driver.Uga->GetMode(FbpDisplayInfo.Driver.Uga, &FbpDisplayInfo.ModeInfo.Width,
                                                            &FbpDisplayInfo.ModeInfo.Height, &Depth, &Refresh);
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Unable to get current UGA mode */
                    XtLdrProtocol->Debug.Print(L"ERROR: Failed to get current UGA mode (Status Code: 0x%zX)\n", Status);

                    /* Close UGA protocol and return error */
                    XtLdrProtocol->Protocol.Close(FbpDisplayInfo.Handle, &UgaGuid);
                    return STATUS_EFI_DEVICE_ERROR;
                }

                /* Find framebuffer address */
                Status = FbpFindFramebufferAddress(&FbpDisplayInfo.FrameBufferBase);
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Unable to find framebuffer address */
                    XtLdrProtocol->Debug.Print(L"ERROR: Failed to get EFI FB address (Status Code: 0x%zX)\n", Status);

                    /* Close UGA protocol and return error */
                    XtLdrProtocol->Protocol.Close(FbpDisplayInfo.Handle, &UgaGuid);
                    return STATUS_EFI_DEVICE_ERROR;
                }

                /* Store framebuffer protocol information */
                FbpDisplayInfo.DefaultMode = 0;
                FbpDisplayInfo.Protocol = UGA;

                /* Get mode information */
                Status = FbpGetModeInfo();
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Unable to get mode information */
                    XtLdrProtocol->Debug.Print(L"ERROR: Failed to get UGA mode information (Status Code: 0x%zX)\n");
                    return STATUS_EFI_UNSUPPORTED;
                }

                /* Found UGA */
                XtLdrProtocol->Debug.Print(L"Found EFI-UGA compatible display adapter @ %P (%zu bytes)\n",
                                           FbpDisplayInfo.FrameBufferBase, FbpDisplayInfo.FrameBufferSize);

                /* Close UGA protocol */
                XtLdrProtocol->Protocol.Close(FbpDisplayInfo.Handle, &UgaGuid);
            }
        }

        /* Make sure framebuffer initialized properly */
        if(FbpDisplayInfo.Protocol == NONE)
        {
            /* GOP and UGA unavailable */
            XtLdrProtocol->Debug.Print(L"WARNING: No display adapter found!\n");
            return STATUS_EFI_NOT_FOUND;
        }

        XtLdrProtocol->Debug.Print(L"Current screen resolution is %ux%ux%u\n", FbpDisplayInfo.ModeInfo.Width,
                                   FbpDisplayInfo.ModeInfo.Height, FbpDisplayInfo.ModeInfo.BitsPerPixel);

        /* Set framebuffer initialization flag */
        FbpDisplayInfo.Initialized = TRUE;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Sets custom screen resolution, based on the provided width and height.
 *
 * @param Width
 *        Supplies the width of the screen.
 *
 * @param Height
 *        Supplies the height of the screen.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
FbSetScreenResolution(IN UINT Width,
                      IN UINT Height)
{
    PEFI_GRAPHICS_OUTPUT_MODE_INFORMATION ModeInfo;
    BOOLEAN ModeChanged;
    EFI_STATUS Status;
    UINT_PTR Size;
    UINT Mode;

    /* Check if framebuffer is initialized */
    if(!FbpDisplayInfo.Initialized)
    {
        /* Framebuffer not ready to change screen mode */
        return STATUS_EFI_NOT_READY;
    }

    ModeChanged = FALSE;

    /* Change screen mode depending on display adapter protocol */
    switch(FbpDisplayInfo.Protocol)
    {
        case GOP:
            /* GOP available, check if user specified screen resolution */
            if(Width == 0 || Height == 0)
            {
                /* No resolution specified, temporarily set lowest supported screen resolution */
                Status = FbpDisplayInfo.Driver.Gop->SetMode(FbpDisplayInfo.Driver.Gop, 1);
                if(Status == STATUS_EFI_SUCCESS)
                {
                    /* Restore default graphics mode */
                    Status = FbpDisplayInfo.Driver.Gop->SetMode(FbpDisplayInfo.Driver.Gop, FbpDisplayInfo.DefaultMode);
                    ModeChanged = (Status == STATUS_EFI_SUCCESS);
                }
            }
            else
            {
                /* User specified screen resolution, find a corresponding mode */
                Mode = 1;
                while(Mode <= FbpDisplayInfo.Driver.Gop->Mode->MaxMode)
                {
                    /* Get mode information */
                    Status = FbpDisplayInfo.Driver.Gop->QueryMode(FbpDisplayInfo.Driver.Gop, Mode, &Size, &ModeInfo);
                    if(Status == STATUS_EFI_SUCCESS && Size >= sizeof(*ModeInfo) && ModeInfo != NULL)
                    {
                        /* Check if match found */
                        if(ModeInfo->HorizontalResolution == Width && ModeInfo->VerticalResolution == Height)
                        {
                            /* Found corresponding mode, attempt to set it */
                            Status = FbpDisplayInfo.Driver.Gop->SetMode(FbpDisplayInfo.Driver.Gop, Mode);
                            if(Status == STATUS_EFI_SUCCESS)
                            {
                                /* New mode set correctly, use it */
                                ModeChanged = TRUE;
                                break;
                            }
                        }
                    }

                    /* Try with next mode */
                    Mode++;
                }
            }
            break;
        case UGA:
            /* Set UGA screen mode, trying to keep current color depth and refresh rate */
            Status = FbpDisplayInfo.Driver.Uga->SetMode(FbpDisplayInfo.Driver.Uga, Width, Height,
                                                        FbpDisplayInfo.ModeInfo.Depth,
                                                        FbpDisplayInfo.ModeInfo.RefreshRate);
            if(Status == STATUS_EFI_SUCCESS)
            {
                /* New mode set correctly, use it */
                ModeChanged = TRUE;
            }
            break;
        default:
            /* This should never be reached */
            break;
    }

    if(!ModeChanged)
    {
        /* Failed to change screen mode */
        XtLdrProtocol->Debug.Print(L"ERROR: Failed to change screen mode to %ux%u (Status Code: 0x%zX)\n",
                                   Width, Height, Status);
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Get new screen mode information */
    Status = FbpGetModeInfo();
    if(Status == STATUS_EFI_SUCCESS)
    {
        XtLdrProtocol->Debug.Print(L"Changed screen resolution to %ux%ux%u\n", FbpDisplayInfo.ModeInfo.Width,
                                   FbpDisplayInfo.ModeInfo.Height, FbpDisplayInfo.ModeInfo.BitsPerPixel);
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Finds a PCI Display Adapter and returns its framebuffer address.
 *
 * @param Address
 *        Supplies a pointer to the memory area where framebuffer address will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
FbpFindFramebufferAddress(OUT PEFI_PHYSICAL_ADDRESS Address)
{
    EFI_GUID PciIoGuid = EFI_PCI_IO_PROTOCOL_GUID;
    PEFI_ACPI_ADDRESS_SPACE_DESCRIPTOR BarInfo;
    PEFI_PCI_IO_PROTOCOL IoProtocol;
    ULONGLONG FramebufAddressLength;
    PCI_TYPE0_DEVICE PciDevice;
    PVOID FramebufAddress;
    UINT_PTR HandlesCount;
    EFI_HANDLE *Handles;
    EFI_STATUS Status;
    UINT Index;

    /* Initialize variables */
    FramebufAddressLength = 0;
    Handles = NULL;

    /* Locate EFI_PCI_IO_PROTOCOL handles */
    Status = XtLdrProtocol->Protocol.LocateHandles(&Handles, &HandlesCount, &PciIoGuid);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to get handles, return error code */
        XtLdrProtocol->Debug.Print(L"ERROR: Failed to get handles (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Iterate through handles */
    for(Index = 0; Index < HandlesCount; Index++)
    {
        /* Open EFI_PCI_IO_PROTOCOL handle */
        Status = XtLdrProtocol->Protocol.OpenHandle(Handles[Index], (PVOID *)&IoProtocol, &PciIoGuid);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to open protocol, continue with next handle */
            XtLdrProtocol->Debug.Print(L"ERROR: Failed to open protocol (Status Code: 0x%zX)\n", Status);
            continue;
        }

        /* Read PCI controller registers from PCI configuration space */
        Status = IoProtocol->Pci.Read(IoProtocol, EfiPciIoWidthUint32, 0, sizeof(PciDevice) / sizeof(UINT), &PciDevice);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Failed to read PCI device class */
            XtLdrProtocol->Debug.Print(L"ERROR: Failed to read class (Status Code: 0x%zX)\n", Status);

            /* Close protocol and continue with next handle */
            XtLdrProtocol->Protocol.Close(Handles[Index], &PciIoGuid);
            continue;
        }

        /* Check if device is a graphics adapter */
        if(PciDevice.Hdr.ClassCode[2] != 0x03)
        {
            /* Not a graphics adapter, close protocol and continue with next handle */
            XtLdrProtocol->Protocol.Close(Handles[Index], &PciIoGuid);
            continue;
        }

        /* Iterate through all PCI device's Base Address Registers (BARs) */
        for(UINT Bars = 0; Bars < 6; Bars++)
        {
            /* Get BAR attributes */
            Status = IoProtocol->GetBarAttributes(IoProtocol, Bars, NULL, (VOID **)&BarInfo);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to get BAR attributes, continue with next BAR */
                continue;
            }

            /* Check if this BAR is 'Memory Range' of 'ACPI QWORD Address Space' */
            if(BarInfo->SpaceDescriptor == EFI_ACPI_ADDRESS64_SPACE_DESCRIPTOR &&
               BarInfo->ResourceType == EFI_ACPI_ADDRESS_SPACE_TYPE_MEMORY)
            {
                /* Check if this BAR is the biggest we've seen so far */
                if(BarInfo->AddressLength > FramebufAddressLength)
                {
                    /* The biggest BAR should be the framebuffer; save its address and length */
                    FramebufAddress = (PVOID)(ULONG_PTR)(BarInfo->AddressRangeMin << 16);
                    FramebufAddressLength = BarInfo->AddressLength;
                }
            }
        }

        /* Close handle and continue with next one */
        XtLdrProtocol->Protocol.Close(Handles[Index], &PciIoGuid);
    }

    /* Set framebuffer address and return success */
    *Address = (EFI_PHYSICAL_ADDRESS)FramebufAddress;
    return STATUS_EFI_SUCCESS;
}

/**
 * Calculates color mask and shift based upon pixel bit mask.
 *
 * @param PixelBitMask
 *        Provides a pixel bit mask.
 *
 * @param ColorMask
 *        Supplies a pointer to the memory area where the color mask will be stored.
 *
 * @param ColorShift
 *        Supplies a pointer to the memory area where the color shift (position) will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
FbpGetColorMask(IN UINT PixelBitMask,
                OUT PUSHORT ColorMask,
                OUT PUSHORT ColorShift)
{
    UINT Index, Mask;

    /* Initialize variables */
    Index = 0;
    Mask = 1;

    /* Make sure EfiMask is not zero */
    if(PixelBitMask)
    {
        while((Index < 32) && ((PixelBitMask & Mask) == 0))
        {
            Index++;
            Mask <<= 1;
        }

        /* Set color mask and shift */
        *ColorShift = Index;
        *ColorMask = (Mask >> Index);
    }
    else
    {
        /* Set default color mask and shift */
        *ColorMask = 0;
        *ColorShift = 0;
    }
}

/**
 * Gets information about the current display mode and stores it in internal structure.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
FbpGetModeInfo()
{
    PEFI_GRAPHICS_OUTPUT_MODE_INFORMATION ModeInfo;
    EFI_PIXEL_BITMASK PixelBitMask;
    XTSTATUS Status;
    UINT_PTR Size;

    switch(FbpDisplayInfo.Protocol)
    {
        case GOP:
            /* Query GOP mode information */
            Status = FbpDisplayInfo.Driver.Gop->QueryMode(FbpDisplayInfo.Driver.Gop,
                                                          FbpDisplayInfo.Driver.Gop->Mode->Mode,
                                                          &Size, &ModeInfo);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to get GOP mode information, return error */
                return Status;
            }

            /* Get pixel bit mask information */
            FbpGetPixelInformation(&FbpDisplayInfo.Driver.Gop->Mode->Info->PixelInformation);

            /* Store GOP framebuffer information */
            FbpDisplayInfo.ModeInfo.Width = FbpDisplayInfo.Driver.Gop->Mode->Info->HorizontalResolution;
            FbpDisplayInfo.ModeInfo.Height = FbpDisplayInfo.Driver.Gop->Mode->Info->VerticalResolution;
            FbpDisplayInfo.ModeInfo.Depth = FbpDisplayInfo.ModeInfo.BitsPerPixel;
            FbpDisplayInfo.ModeInfo.PixelsPerScanLine = FbpDisplayInfo.Driver.Gop->Mode->Info->PixelsPerScanLine;
            FbpDisplayInfo.ModeInfo.Pitch = FbpDisplayInfo.ModeInfo.PixelsPerScanLine *
                                            (FbpDisplayInfo.ModeInfo.BitsPerPixel / 8);
            FbpDisplayInfo.ModeInfo.RefreshRate = 0;

            /* Store pixel format information and frame buffer size */
            FbpDisplayInfo.ModeInfo.PixelFormat = FbpDisplayInfo.Driver.Gop->Mode->Info->PixelFormat;
            FbpDisplayInfo.FrameBufferSize = FbpDisplayInfo.Driver.Gop->Mode->FrameBufferSize;
            break;
        case UGA:
            /* Query UGA mode information */
            Status = FbpDisplayInfo.Driver.Uga->GetMode(FbpDisplayInfo.Driver.Uga, &FbpDisplayInfo.ModeInfo.Width,
                                                        &FbpDisplayInfo.ModeInfo.Height, &FbpDisplayInfo.ModeInfo.Depth,
                                                        &FbpDisplayInfo.ModeInfo.RefreshRate);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Failed to get UGA mode information, return error */
                return Status;
            }

            /* Get pixel bit mask information */
            PixelBitMask = (EFI_PIXEL_BITMASK){0, 0, 0, 0};
            FbpGetPixelInformation(&PixelBitMask);

            /* Store UGA framebuffer information */
            FbpDisplayInfo.ModeInfo.PixelsPerScanLine = FbpDisplayInfo.ModeInfo.Width;
            FbpDisplayInfo.ModeInfo.Pitch = FbpDisplayInfo.ModeInfo.PixelsPerScanLine *
                                            (FbpDisplayInfo.ModeInfo.BitsPerPixel / 8);

            /* Store pixel format information and recalculate frame buffer size */
            FbpDisplayInfo.ModeInfo.PixelFormat = PixelBlueGreenRedReserved8BitPerColor;
            FbpDisplayInfo.FrameBufferSize = FbpDisplayInfo.ModeInfo.Width *
                                             FbpDisplayInfo.ModeInfo.Height *
                                             FbpDisplayInfo.ModeInfo.BytesPerPixel + 1024;
            break;
        default:
            /* This should never be reached as no other display driver is supported */
            break;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/** 
 * Gets pixel information based on the reported pixel format.
 *
 * @param FrameBufferInfo
 *        Supplies a pointer to the framebuffer information structure.
 *
 * @param PixelsBitMask
 *        Supplies a pointer to the pixel bit mask information provided by EFI graphics protocol.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
FbpGetPixelInformation(IN PEFI_PIXEL_BITMASK PixelsBitMask)
{
    UINT CompoundMask;

    /* Check reported pixel format */
    switch(FbpDisplayInfo.ModeInfo.PixelFormat)
    {
        case PixelBlueGreenRedReserved8BitPerColor:
            /* BGRR, 32 bits per pixel */
            FbpDisplayInfo.ModeInfo.BitsPerPixel = 32;
            FbpDisplayInfo.ModeInfo.PixelInformation.BlueMask = 0xFF;
            FbpDisplayInfo.ModeInfo.PixelInformation.BlueShift = 0;
            FbpDisplayInfo.ModeInfo.PixelInformation.GreenMask = 0xFF;
            FbpDisplayInfo.ModeInfo.PixelInformation.GreenShift = 8;
            FbpDisplayInfo.ModeInfo.PixelInformation.RedMask = 0xFF;
            FbpDisplayInfo.ModeInfo.PixelInformation.RedShift = 16;
            FbpDisplayInfo.ModeInfo.PixelInformation.ReservedMask = 0xFF;
            FbpDisplayInfo.ModeInfo.PixelInformation.ReservedShift = 24;
            break;
        case PixelRedGreenBlueReserved8BitPerColor:
            /* RGBR, 32 bits per pixel */
            FbpDisplayInfo.ModeInfo.BitsPerPixel = 32;
            FbpDisplayInfo.ModeInfo.PixelInformation.BlueMask = 0xFF;
            FbpDisplayInfo.ModeInfo.PixelInformation.BlueShift = 16;
            FbpDisplayInfo.ModeInfo.PixelInformation.GreenMask = 0xFF;
            FbpDisplayInfo.ModeInfo.PixelInformation.GreenShift = 8;
            FbpDisplayInfo.ModeInfo.PixelInformation.RedMask = 0xFF;
            FbpDisplayInfo.ModeInfo.PixelInformation.RedShift = 0;
            FbpDisplayInfo.ModeInfo.PixelInformation.ReservedMask = 0xFF;
            FbpDisplayInfo.ModeInfo.PixelInformation.ReservedShift = 24;
            break;
        case PixelBitMask:
            /* Assume 32 bits per pixel */
            FbpDisplayInfo.ModeInfo.BitsPerPixel = 32;

            /* Calculate compound mask */
            CompoundMask = PixelsBitMask->RedMask |
                           PixelsBitMask->GreenMask |
                           PixelsBitMask->BlueMask |
                           PixelsBitMask->ReservedMask;

            /* Recalculate bits per pixel */
            while((CompoundMask & (1 << 31)) == 0)
            {
                FbpDisplayInfo.ModeInfo.BitsPerPixel--;
                CompoundMask <<= 1;
            }

            /* Set pixel information */
            FbpGetColorMask(PixelsBitMask->RedMask, &FbpDisplayInfo.ModeInfo.PixelInformation.RedMask,
                            &FbpDisplayInfo.ModeInfo.PixelInformation.RedShift);
            FbpGetColorMask(PixelsBitMask->GreenMask, &FbpDisplayInfo.ModeInfo.PixelInformation.GreenMask,
                            &FbpDisplayInfo.ModeInfo.PixelInformation.GreenShift);
            FbpGetColorMask(PixelsBitMask->BlueMask, &FbpDisplayInfo.ModeInfo.PixelInformation.BlueMask,
                            &FbpDisplayInfo.ModeInfo.PixelInformation.BlueShift);
            FbpGetColorMask(PixelsBitMask->ReservedMask, &FbpDisplayInfo.ModeInfo.PixelInformation.ReservedMask,
                            &FbpDisplayInfo.ModeInfo.PixelInformation.ReservedShift);
            break;
        default:
            /* Unknown pixel format */
            FbpDisplayInfo.ModeInfo.BitsPerPixel = 0;
            FbpDisplayInfo.ModeInfo.PixelInformation.BlueMask = 0x0;
            FbpDisplayInfo.ModeInfo.PixelInformation.BlueShift = 0;
            FbpDisplayInfo.ModeInfo.PixelInformation.GreenMask = 0x0;
            FbpDisplayInfo.ModeInfo.PixelInformation.GreenShift = 0;
            FbpDisplayInfo.ModeInfo.PixelInformation.RedMask = 0x0;
            FbpDisplayInfo.ModeInfo.PixelInformation.RedShift = 0;
            FbpDisplayInfo.ModeInfo.PixelInformation.ReservedMask = 0x0;
            FbpDisplayInfo.ModeInfo.PixelInformation.ReservedShift = 0;
            break;
    }

    /* Calculate bytes per pixel based on bits per pixel */
    FbpDisplayInfo.ModeInfo.BytesPerPixel = FbpDisplayInfo.ModeInfo.BitsPerPixel >> 3;
}

/**
 * This routine is the entry point of the XT EFI boot loader module.
 *
 * @param ImageHandle
 *        Firmware-allocated handle that identifies the image.
 *
 * @param SystemTable
 *        Provides the EFI system table.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_GUID Guid = XT_FRAMEBUFFER_PROTOCOL_GUID;
    EFI_STATUS Status;

    /* Open the XTLDR protocol */
    Status = BlGetXtLdrProtocol(SystemTable, ImageHandle, &XtLdrProtocol);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open loader protocol */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Set initial framebuffer state */
    FbpDisplayInfo.Protocol = NONE;
    FbpDisplayInfo.Initialized = FALSE;

    /* Set routines available via XTLDR framebuffer protocol */
    FbpFrameBufferProtocol.GetDisplayDriver = FbGetDisplayDriver;
    FbpFrameBufferProtocol.GetDisplayInformation = FbGetDisplayInformation;
    FbpFrameBufferProtocol.GetPreferredScreenResolution = FbGetPreferredScreenResolution;
    FbpFrameBufferProtocol.Initialize = FbInitializeDisplay;
    FbpFrameBufferProtocol.SetScreenResolution = FbSetScreenResolution;

    /* Register XTOS boot protocol */
    return XtLdrProtocol->Protocol.Install(&FbpFrameBufferProtocol, &Guid);
}
