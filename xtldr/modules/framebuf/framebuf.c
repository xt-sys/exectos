/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/framebuf/framebuf.c
 * DESCRIPTION:     Boot loader framebuffer support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <framebuf.h>


/* EFI Image Handle */
EFI_HANDLE EfiImageHandle;

/* EFI System Table */
PEFI_SYSTEM_TABLE EfiSystemTable;

/* EFI XT Loader Protocol */
PXT_BOOT_LOADER_PROTOCOL XtLdrProtocol;

/* XT FrameBuffer Information */
XT_FRAMEBUFFER_INFORMATION FrameBufferInfo;

/* XT FrameBuffer Protocol */
XT_FRAMEBUFFER_PROTOCOL XtFramebufferProtocol;

/**
 * Provides a current FrameBuffer driver name.
 *
 * @param DriverName
 *        Supplies a pointer to the memory area where FB driver name will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
FbGetDisplayDriver(OUT PWCHAR *DriverName)
{
    switch(FrameBufferInfo.Protocol)
    {
        case GOP:
            *DriverName = L"GOP";
            break;
        case UGA:
            *DriverName = L"UGA";
            break;
        default:
            *DriverName = L"NONE";
            break;
    }
}

/**
 * Returns information about frame buffer in XTOS compatible format.
 *
 * @param InformationBlock
 *        A pointer to memory area containing XT structure where all the information will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
FbGetDisplayInformation(OUT PLOADER_GRAPHICS_INFORMATION_BLOCK InformationBlock)
{
    InformationBlock->Initialized = FrameBufferInfo.Initialized;
    InformationBlock->Protocol = FrameBufferInfo.Protocol;
    InformationBlock->Address = (PVOID)(ULONG_PTR)FrameBufferInfo.FrameBufferBase;
    InformationBlock->BufferSize = FrameBufferInfo.FrameBufferSize;
    InformationBlock->Width = FrameBufferInfo.HorizontalResolution;
    InformationBlock->Height = FrameBufferInfo.VerticalResolution;
    InformationBlock->BitsPerPixel = FrameBufferInfo.BitsPerPixel;
    InformationBlock->PixelsPerScanLine = FrameBufferInfo.PixelsPerScanLine;
    InformationBlock->Pitch = FrameBufferInfo.Pitch;
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
    UINT32 Parameter1, Parameter2;
    EFI_STATUS Status;

    /* Check if framebuffer already initialized */
    if(!FrameBufferInfo.Initialized)
    {
        /* Initialize framebuffer */
        XtLdrProtocol->DbgPrint(L"Initializing framebuffer device\n");
        FrameBufferInfo.Protocol = NONE;
        FrameBufferInfo.Initialized = FALSE;

        /* Check if GOP already in use */
        Status = EfiSystemTable->BootServices->HandleProtocol(EfiSystemTable->ConsoleOutHandle, &GopGuid,
                                                              (PVOID*)&FrameBufferInfo.Adapter.GOP);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Locate GOP protocol */
            Status = EfiSystemTable->BootServices->LocateProtocol(&GopGuid, NULL, (PVOID *)&FrameBufferInfo.Adapter.GOP);
        }

        /* Check if Graphics Output Protocol is available */
        if(Status == STATUS_EFI_SUCCESS)
        {
            /* Found GOP */
            XtLdrProtocol->DbgPrint(L"Found GOP compatible display adapter\n");

            /* Set framebuffer parameters */
            FrameBufferInfo.HorizontalResolution = FrameBufferInfo.Adapter.GOP->Mode->Info->HorizontalResolution;
            FrameBufferInfo.VerticalResolution = FrameBufferInfo.Adapter.GOP->Mode->Info->VerticalResolution;
            FrameBufferInfo.BitsPerPixel = GoppGetBitsPerPixel();
            FrameBufferInfo.BytesPerPixel = FrameBufferInfo.BitsPerPixel >> 3;
            FrameBufferInfo.PixelsPerScanLine = FrameBufferInfo.Adapter.GOP->Mode->Info->PixelsPerScanLine;
            FrameBufferInfo.PixelFormat = FrameBufferInfo.Adapter.GOP->Mode->Info->PixelFormat;
            FrameBufferInfo.Pitch = FrameBufferInfo.PixelsPerScanLine * (FrameBufferInfo.BitsPerPixel / 8);
            FrameBufferInfo.FrameBufferBase = FrameBufferInfo.Adapter.GOP->Mode->FrameBufferBase;
            FrameBufferInfo.FrameBufferSize = FrameBufferInfo.Adapter.GOP->Mode->FrameBufferSize;
            FrameBufferInfo.Protocol = GOP;
            FrameBufferInfo.Initialized = TRUE;
        }
        else
        {
            /* GOP is unavailable */
            FrameBufferInfo.Adapter.GOP = NULL;

            /* Check if UGA already in use */
            Status = EfiSystemTable->BootServices->HandleProtocol(EfiSystemTable->ConsoleOutHandle, &UgaGuid,
                                                                  (PVOID*)&FrameBufferInfo.Adapter.UGA);
            if(Status != STATUS_EFI_SUCCESS)
            {
                /* Locate UGA protocol */
                Status = EfiSystemTable->BootServices->LocateProtocol(&UgaGuid, NULL,
                                                                      (PVOID*)&FrameBufferInfo.Adapter.UGA);
            }

            /* Check if Universal Graphics Adapter is available */
            if(Status == STATUS_EFI_SUCCESS)
            {
                /* Found UGA */
                XtLdrProtocol->DbgPrint(L"Found UGA compatible display adapter\n");

                /* Get current mode */
                Status = FrameBufferInfo.Adapter.UGA->GetMode(FrameBufferInfo.Adapter.UGA,
                                                              &FrameBufferInfo.HorizontalResolution,
                                                              &FrameBufferInfo.VerticalResolution,
                                                              &Parameter1, &Parameter2);
                if(Status != STATUS_EFI_SUCCESS)
                {
                    /* Unable to get current UGA mode */
                    XtLdrProtocol->DbgPrint(L"Failed to get current UGA mode (Status code: %lx)\n", Status);
                    FrameBufferInfo.Adapter.UGA = NULL;
                }
                else
                {
                    /* Set framebuffer parameters */
                    FrameBufferInfo.BitsPerPixel = 32;
                    FrameBufferInfo.BytesPerPixel = 4;
                    FrameBufferInfo.PixelsPerScanLine = FrameBufferInfo.HorizontalResolution;
                    FrameBufferInfo.PixelFormat = PixelBlueGreenRedReserved8BitPerColor;
                    FrameBufferInfo.Pitch = FrameBufferInfo.PixelsPerScanLine * (FrameBufferInfo.BitsPerPixel / 8);
                    FrameBufferInfo.FrameBufferBase = 0;
                    FrameBufferInfo.FrameBufferSize = FrameBufferInfo.HorizontalResolution * 
                                                      FrameBufferInfo.VerticalResolution *
                                                      FrameBufferInfo.BytesPerPixel + 1024;
                    FrameBufferInfo.Protocol = UGA;

                    /* Temporarily set this to FALSE, as we don't set FB base and we cannot use it anyway */
                    FrameBufferInfo.Initialized = FALSE;
                }
            }
        }

        /* Make sure framebuffer initialized properly */
        if(!FrameBufferInfo.Initialized)
        {
            /* GOP and UGA unavailable */
            XtLdrProtocol->DbgPrint(L"No display adapter found\n");
            return STATUS_EFI_NOT_FOUND;
        }
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Prints important information about framebuffer.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
FbPrintDisplayInformation()
{
    PWCHAR DriverName;

    /* Make sure frame buffer is initialized */
    if(!FrameBufferInfo.Initialized)
    {
        /* No FrameBuffer */
        XtLdrProtocol->DbgPrint(L"No display adapters initialized, unable to print video information\n");
        return;
    }

    /* Get display driver name */
    FbGetDisplayDriver(&DriverName);

    /* Print video information */
    XtLdrProtocol->DbgPrint(L"XTLDR Framebuffer information:\n"
                            L"   FrameBuffer Address: 0x%lx\n"
                            L"   FrameBuffer Size: %lu\n"
                            L"   FrameBuffer Driver: %S\n"
                            L"   Current Resolution: %ux%ux%u\n"
                            L"   Pixel Format: %u\n"
                            L"   Pixels Per ScanLine: %u\n",
                            FrameBufferInfo.FrameBufferBase, FrameBufferInfo.FrameBufferSize, DriverName,
                            FrameBufferInfo.HorizontalResolution, FrameBufferInfo.VerticalResolution,
                            FrameBufferInfo.BitsPerPixel, FrameBufferInfo.PixelFormat,
                            FrameBufferInfo.PixelsPerScanLine);
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
BlXtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                  IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_GUID Guid = XT_FRAMEBUFFER_PROTOCOL_GUID;
    EFI_HANDLE Handle = NULL;
    EFI_STATUS Status;

    /* Set the system table and image handle */
    EfiImageHandle = ImageHandle;
    EfiSystemTable = SystemTable;

    /* Open the XTLDR protocol */
    Status = BlGetXtLoaderProtocol(&XtLdrProtocol);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open loader protocol */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    XtFramebufferProtocol.GetDisplayDriver = FbGetDisplayDriver;
    XtFramebufferProtocol.GetDisplayInformation = FbGetDisplayInformation;
    XtFramebufferProtocol.Initialize = FbInitializeDisplay;
    XtFramebufferProtocol.PrintDisplayInformation = FbPrintDisplayInformation;

    /* Register XTOS boot protocol */
    return EfiSystemTable->BootServices->InstallProtocolInterface(&Handle, &Guid, EFI_NATIVE_INTERFACE,
                                                                  &XtFramebufferProtocol);
}
