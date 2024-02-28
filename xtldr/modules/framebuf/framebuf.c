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
MODULE_VERSION(L"0.1");

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
FbGetDisplayInformation(OUT PXTBL_FRAMEBUFFER_INFORMATION FbInfo)
{
    /* Check if framebuffer is initialized */
    if(!FbpDisplayInfo.Initialized)
    {
        /* Return error if framebuffer is not initialized */
        return STATUS_EFI_NOT_READY;
    }

    /* Copy framebuffer information */
    FbInfo->BitsPerPixel = FbpDisplayInfo.BitsPerPixel;
    FbInfo->BytesPerPixel = FbpDisplayInfo.BytesPerPixel;
    FbInfo->PixelFormat = FbpDisplayInfo.PixelFormat;
    FbInfo->PixelInformation = FbpDisplayInfo.PixelInformation;
    FbInfo->PixelsPerScanLine = FbpDisplayInfo.PixelsPerScanLine;
    FbInfo->Width = FbpDisplayInfo.Width;
    FbInfo->Height = FbpDisplayInfo.Height;
    FbInfo->Pitch = FbpDisplayInfo.Pitch;
    FbInfo->FrameBufferBase = FbpDisplayInfo.FrameBufferBase;
    FbInfo->FrameBufferSize = FbpDisplayInfo.FrameBufferSize;
    FbInfo->Protocol = FbpDisplayInfo.Protocol;
    FbInfo->Initialized = FbpDisplayInfo.Initialized;

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
    PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL Uga;
    PEFI_GRAPHICS_OUTPUT_PROTOCOL Gop;
    EFI_PIXEL_BITMASK PixelBitMask;
    UINT32 Depth, Refresh;
    EFI_HANDLE Handle;
    EFI_STATUS Status;

    /* Check if framebuffer already initialized */
    if(!FbpDisplayInfo.Initialized)
    {
        /* Set initial framebuffer state */
        XtLdrProtocol->Debug.Print(L"Initializing framebuffer device\n");
        FbpDisplayInfo.Protocol = NONE;
        FbpDisplayInfo.Initialized = FALSE;

        /* Attempt to open EFI GOP protocol */
        Status = XtLdrProtocol->Protocol.Open(&Handle, (PVOID*)&Gop, &GopGuid);

        /* Check if Graphics Output Protocol (GOP) is available */
        if(Status == STATUS_EFI_SUCCESS)
        {
            /* Found GOP */
            XtLdrProtocol->Debug.Print(L"Found EFI-GOP compatible display adapter\n");

            /* Set framebuffer information */
            FbpDisplayInfo.Protocol = GOP;
            FbpDisplayInfo.Width = Gop->Mode->Info->HorizontalResolution;
            FbpDisplayInfo.Height = Gop->Mode->Info->VerticalResolution;
            FbpDisplayInfo.PixelsPerScanLine = Gop->Mode->Info->PixelsPerScanLine;
            FbpDisplayInfo.PixelFormat = Gop->Mode->Info->PixelFormat;

            /* Get pixel bit mask information */
            PixelBitMask = Gop->Mode->Info->PixelInformation;

            /* Set framebuffer address and size */
            FbpDisplayInfo.FrameBufferBase = Gop->Mode->FrameBufferBase;
            FbpDisplayInfo.FrameBufferSize = Gop->Mode->FrameBufferSize;

            /* Close GOP protocol */
            Status = XtLdrProtocol->Protocol.Close(Handle, &GopGuid);
        }
        else
        {
            /* GOP is unavailable, attempt to open UGA protocol */
            Status = XtLdrProtocol->Protocol.Open(&Handle, (PVOID*)&Uga, &UgaGuid);

            /* Check if Universal Graphics Adapter (UGA) is available */
            if(Status == STATUS_EFI_SUCCESS)
            {
                /* Get current video mode */
                Status = Uga->GetMode(Uga, &FbpDisplayInfo.Width, &FbpDisplayInfo.Height, &Depth, &Refresh);
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Unable to get current UGA mode */
                    XtLdrProtocol->Debug.Print(L"ERROR: Failed to get current UGA mode (Status Code: 0x%zX)\n", Status);

                    /* Close UGA protocol and return error */
                    XtLdrProtocol->Protocol.Close(Handle, &UgaGuid);
                    return STATUS_EFI_DEVICE_ERROR;
                }

                /* Set framebuffer information */
                FbpDisplayInfo.Protocol = UGA;
                FbpDisplayInfo.PixelsPerScanLine = FbpDisplayInfo.Width;
                FbpDisplayInfo.PixelFormat = PixelBlueGreenRedReserved8BitPerColor;

                /* Get pixel bit mask information */
                PixelBitMask = (EFI_PIXEL_BITMASK){0, 0, 0, 0};

                /* Set framebuffer address */
                Status = FbpFindFramebufferAddress(&FbpDisplayInfo.FrameBufferBase);
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Unable to find framebuffer address */
                    XtLdrProtocol->Debug.Print(L"ERROR: Failed to get EFI FB address (Status Code: 0x%zX)\n", Status);

                    /* Close UGA protocol and return error */
                    XtLdrProtocol->Protocol.Close(Handle, &UgaGuid);
                    return STATUS_EFI_DEVICE_ERROR;
                }

                /* Set framebuffer size */
                FbpDisplayInfo.FrameBufferSize = FbpDisplayInfo.Width * 
                                                  FbpDisplayInfo.Height *
                                                  FbpDisplayInfo.BytesPerPixel + 1024;

                /* Close UGA protocol */
                XtLdrProtocol->Protocol.Close(Handle, &UgaGuid);
            }
        }

        /* Make sure framebuffer initialized properly */
        if(FbpDisplayInfo.Protocol == NONE)
        {
            /* GOP and UGA unavailable */
            XtLdrProtocol->Debug.Print(L"WARNING: No display adapter found!\n");
            return STATUS_EFI_NOT_FOUND;
        }

        /* Get pixel information */
        FbpGetPixelInformation(&FbpDisplayInfo, &PixelBitMask);

        /* Set additional framebuffer information */
        FbpDisplayInfo.BytesPerPixel = FbpDisplayInfo.BitsPerPixel >> 3;
        FbpDisplayInfo.Pitch = FbpDisplayInfo.PixelsPerScanLine * (FbpDisplayInfo.BitsPerPixel / 8);

        /* Set framebuffer initialization flag */
        FbpDisplayInfo.Initialized = TRUE;
    }

    /* Return success */
    return STATUS_SUCCESS;
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
FbpGetPixelInformation(IN OUT PXTBL_FRAMEBUFFER_INFORMATION FrameBufferInfo,
                       IN PEFI_PIXEL_BITMASK PixelsBitMask)
{
    UINT CompoundMask;

    /* Check reported pixel format */
    switch(FrameBufferInfo->PixelFormat)
    {
        case PixelBlueGreenRedReserved8BitPerColor:
            /* BGRR, 32 bits per pixel */
            FrameBufferInfo->BitsPerPixel = 32;
            FrameBufferInfo->PixelInformation.BlueMask = 0xFF;
            FrameBufferInfo->PixelInformation.BlueShift = 0;
            FrameBufferInfo->PixelInformation.GreenMask = 0xFF;
            FrameBufferInfo->PixelInformation.GreenShift = 8;
            FrameBufferInfo->PixelInformation.RedMask = 0xFF;
            FrameBufferInfo->PixelInformation.RedShift = 16;
            FrameBufferInfo->PixelInformation.ReservedMask = 0xFF;
            FrameBufferInfo->PixelInformation.ReservedShift = 24;
            break;
        case PixelRedGreenBlueReserved8BitPerColor:
            /* RGBR, 32 bits per pixel */
            FrameBufferInfo->BitsPerPixel = 32;
            FrameBufferInfo->PixelInformation.BlueMask = 0xFF;
            FrameBufferInfo->PixelInformation.BlueShift = 16;
            FrameBufferInfo->PixelInformation.GreenMask = 0xFF;
            FrameBufferInfo->PixelInformation.GreenShift = 8;
            FrameBufferInfo->PixelInformation.RedMask = 0xFF;
            FrameBufferInfo->PixelInformation.RedShift = 0;
            FrameBufferInfo->PixelInformation.ReservedMask = 0xFF;
            FrameBufferInfo->PixelInformation.ReservedShift = 24;
            break;
        case PixelBitMask:
            /* Assume 32 bits per pixel */
            FrameBufferInfo->BitsPerPixel = 32;

            /* Calculate compound mask */
            CompoundMask = PixelsBitMask->RedMask |
                           PixelsBitMask->GreenMask |
                           PixelsBitMask->BlueMask |
                           PixelsBitMask->ReservedMask;

            /* Recalculate bits per pixel */
            while((CompoundMask & (1 << 31)) == 0)
            {
                FrameBufferInfo->BitsPerPixel--;
                CompoundMask <<= 1;
            }

            /* Set pixel information */
            FbpGetColorMask(PixelsBitMask->RedMask, &FrameBufferInfo->PixelInformation.RedMask,
                            &FrameBufferInfo->PixelInformation.RedShift);
            FbpGetColorMask(PixelsBitMask->GreenMask, &FrameBufferInfo->PixelInformation.GreenMask,
                            &FrameBufferInfo->PixelInformation.GreenShift);
            FbpGetColorMask(PixelsBitMask->BlueMask, &FrameBufferInfo->PixelInformation.BlueMask,
                            &FrameBufferInfo->PixelInformation.BlueShift);
            FbpGetColorMask(PixelsBitMask->ReservedMask, &FrameBufferInfo->PixelInformation.ReservedMask,
                            &FrameBufferInfo->PixelInformation.ReservedShift);
            break;
        default:
            /* Unknown pixel format */
            FrameBufferInfo->BitsPerPixel = 0;
            FrameBufferInfo->PixelInformation.BlueMask = 0x0;
            FrameBufferInfo->PixelInformation.BlueShift = 0;
            FrameBufferInfo->PixelInformation.GreenMask = 0x0;
            FrameBufferInfo->PixelInformation.GreenShift = 0;
            FrameBufferInfo->PixelInformation.RedMask = 0x0;
            FrameBufferInfo->PixelInformation.RedShift = 0;
            FrameBufferInfo->PixelInformation.ReservedMask = 0x0;
            FrameBufferInfo->PixelInformation.ReservedShift = 0;
            break;
    }
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

    /* Set routines available via XTLDR framebuffer protocol */
    FbpFrameBufferProtocol.GetDisplayDriver = FbGetDisplayDriver;
    FbpFrameBufferProtocol.GetDisplayInformation = FbGetDisplayInformation;
    FbpFrameBufferProtocol.Initialize = FbInitializeDisplay;

    /* Register XTOS boot protocol */
    return XtLdrProtocol->Protocol.Install(&FbpFrameBufferProtocol, &Guid);
}
