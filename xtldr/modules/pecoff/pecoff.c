/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/pecoff/pecoff.c
 * DESCRIPTION:     PE/COFF executable file format support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <pecoff.h>


/* EFI Image Handle */
EFI_HANDLE EfiImageHandle;

/* EFI System Table */
PEFI_SYSTEM_TABLE EfiSystemTable;

/* EFI XT Loader Protocol */
PXT_BOOT_LOADER_PROTOCOL EfiXtLdrProtocol;

/* XTOS PE/COFF Image Protocol */
XT_PECOFF_IMAGE_PROTOCOL XtPeCoffProtocol;

/**
 * Loads a PE/COFF image file.
 *
 * @param FileHandle
 *        The handle of the opened portable executable (PE) file.
 *
 * @param VirtualAddress
 *        Optional virtual address pointing to the memory area where PE/COFF file will be loaded.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
EFI_STATUS PeLoadImage(IN PEFI_FILE_HANDLE FileHandle,
                       IN PVOID VirtualAddress,
                       OUT PPECOFF_IMAGE_CONTEXT *Image)
{
    EFI_GUID FileInfoGuid = EFI_FILE_INFO_PROTOCOL_GUID;
    PPECOFF_IMAGE_SECTION_HEADER SectionHeader;
    PPECOFF_IMAGE_PE_HEADER PeHeader;
    PPECOFF_IMAGE_CONTEXT ImageData;
    EFI_PHYSICAL_ADDRESS Address;
    PEFI_FILE_INFO FileInfo;
    SIZE_T Size, Pages;
    EFI_STATUS Status;
    UINT_PTR ReadSize;
    UINT SectionSize;
    PUCHAR Data;
    UINT Index;

    /* Set required size for getting file information */
    Size = sizeof(EFI_FILE_INFO) + 32;

    /* Allocate necessary amount of memory */
    Status = EfiXtLdrProtocol->AllocatePool(Size, (PVOID *)&FileInfo);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        EfiXtLdrProtocol->DbgPrint(L"ERROR: Memory pool allocation failure\n");
        return Status;
    }

    /* First attempt to get file information */
    Status = FileHandle->GetInfo(FileHandle, &FileInfoGuid, &Size, FileInfo);
    if(Status == STATUS_EFI_BUFFER_TOO_SMALL)
    {
        /* Buffer it too small, but EFI tells the required size, let's reallocate */
        EfiXtLdrProtocol->FreePool(&FileInfo);
        Status = EfiXtLdrProtocol->AllocatePool(Size, (PVOID *)&FileInfo);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure */
            EfiXtLdrProtocol->DbgPrint(L"ERROR: Memory pool allocation failure\n");
            return Status;
        }
        /* Second attempt to get file information */
        Status = FileHandle->GetInfo(FileHandle, &FileInfoGuid, &Size, FileInfo);
    }
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to get file information */
        EfiXtLdrProtocol->DbgPrint(L"ERROR: Failed to get file information\n");
        return Status;
    }

    /* Allocate memory for storing image data */
    Status = EfiXtLdrProtocol->AllocatePool(sizeof(PECOFF_IMAGE_CONTEXT), (PVOID *)&ImageData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        EfiXtLdrProtocol->DbgPrint(L"ERROR: Memory pool allocation failure\n");
        return Status;
    }

    /* Store file size and free memory */
    ImageData->FileSize = FileInfo->FileSize;
    EfiXtLdrProtocol->FreePool(FileInfo);

    /* Calculate number of pages */
    Pages = EFI_SIZE_TO_PAGES(ImageData->FileSize);

    /* Allocate pages */
    Status = EfiXtLdrProtocol->AllocatePages(Pages, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Pages allocation failure */
        EfiXtLdrProtocol->DbgPrint(L"ERROR: Pages allocation failure\n");
        EfiXtLdrProtocol->FreePool(ImageData);
        return Status;
    }

    /* Read PE/COFF image */
    ReadSize = Pages * EFI_PAGE_SIZE;
    Data = (PUCHAR)(UINT_PTR)Address;
    Status = FileHandle->Read(FileHandle, &ReadSize, Data);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to read data */
        EfiXtLdrProtocol->DbgPrint(L"ERROR: Unable to read PE/COFF image file\n");
        EfiXtLdrProtocol->FreePages(Pages, (EFI_PHYSICAL_ADDRESS)(UINT_PTR)Data);
        EfiXtLdrProtocol->FreePool(ImageData);
        return Status;
    }

    /* Read and validate headers */
    Status = PepReadImageHeader(Data, ImageData->FileSize, &PeHeader);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Header validation failed, probably broken or invalid PE/COFF image */
        EfiXtLdrProtocol->DbgPrint(L"ERROR: Invalid PE/COFF image header\n");
        EfiXtLdrProtocol->FreePages(Pages, (EFI_PHYSICAL_ADDRESS)(UINT_PTR)Data);
        EfiXtLdrProtocol->FreePool(ImageData);
        return Status;
    }

    /* Store image size and calculate number of image pages */
    ImageData->ImageSize = PeHeader->OptionalHeader.SizeOfImage;
    ImageData->ImagePages = EFI_SIZE_TO_PAGES(ImageData->ImageSize);

    /* Allocate image pages */
    Status = EfiXtLdrProtocol->AllocatePages(ImageData->ImagePages, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Pages reallocation failure */
        EfiXtLdrProtocol->DbgPrint(L"ERROR: Pages reallocation failure\n");
        EfiXtLdrProtocol->FreePool(ImageData);
        return Status;
    }

    /* Store image data and base address */
    ImageData->Data = (PUINT8)(UINT_PTR)Address;
    if(VirtualAddress)
    {
        /* Virtual address passed to this routine */
        ImageData->VirtualAddress = VirtualAddress;
    }
    else
    {
        /* Virtual address not specified, use physical address */
        ImageData->VirtualAddress = (PVOID)(UINT_PTR)Address;
    }

    /* Copy all sections */
    RtlCopyMemory(ImageData->Data, Data, PeHeader->OptionalHeader.SizeOfHeaders);

    /* Find section header */
    SectionHeader = (PPECOFF_IMAGE_SECTION_HEADER)((PUCHAR)&PeHeader->OptionalHeader +
                                                   PeHeader->FileHeader.SizeOfOptionalHeader);

    /* Load each section into memory */
    for(Index = 0; Index < PeHeader->FileHeader.NumberOfSections; Index++)
    {
        /* Check section raw data size and section virtual size */
        if(SectionHeader[Index].SizeOfRawData < SectionHeader[Index].Misc.VirtualSize)
        {
            /* Use raw data size if it is smaller than virtual size */
            SectionSize = SectionHeader[Index].SizeOfRawData;
        }
        else
        {
            /* User virtual size otherwise */
            SectionSize = SectionHeader[Index].Misc.VirtualSize;
        }

        /* Make sure section is available */
        if(SectionSize > 0 && SectionHeader[Index].PointerToRawData != 0)
        {
            /* Copy section */
            RtlCopyMemory((PUINT8)ImageData->Data + SectionHeader[Index].VirtualAddress,
                          Data + SectionHeader[Index].PointerToRawData, SectionSize);
        }

        /* Check if raw size is shorter than virtual size */
        if(SectionSize < SectionHeader[Index].Misc.VirtualSize)
        {
            /* Fill remaining space with zeroes */
            RtlZeroMemory((PUINT8)ImageData->Data + SectionHeader[Index].VirtualAddress + SectionSize,
                         SectionHeader[Index].Misc.VirtualSize - SectionSize);
        }
    }

    /* Free pages */
    EfiXtLdrProtocol->FreePages((EFI_PHYSICAL_ADDRESS)(UINT_PTR)Data, Pages);

    /* Store image data */
    *Image = ImageData;

    /* Return SUCCESS */
    return STATUS_EFI_SUCCESS;
}

/**
 * Reads and validate a PE/COFF image headers
 *
 * @param ImageData
 *        A pointer to the buffer containing PE/COFF image contents.
 *
 * @param FileSize
 *        An input PE/COFF image file size.
 *
 * @param PeHeader
 *        Pointer to the memory area where PE header will be saved.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
EFI_STATUS
PepReadImageHeader(IN PUCHAR ImageData,
                   IN SIZE_T FileSize,
                   OUT PPECOFF_IMAGE_PE_HEADER *PeHeader)
{
    PPECOFF_IMAGE_DOS_HEADER DosHeader;

    /* Validate file size */
    if(FileSize < sizeof(PECOFF_IMAGE_DOS_HEADER))
    {
        EfiXtLdrProtocol->DbgPrint(L"WARNING: PE/COFF image shorter than DOS header\n");
        return STATUS_EFI_END_OF_FILE;
    }

    /* Validate DOS header */
    DosHeader = (PPECOFF_IMAGE_DOS_HEADER)ImageData;
    if(DosHeader->e_magic != PECOFF_IMAGE_DOS_SIGNATURE)
    {
        EfiXtLdrProtocol->DbgPrint(L"WARNING: Invalid DOS signature found\n");
        return STATUS_EFI_INCOMPATIBLE_VERSION;
    }

    /* Validate NT/XT header */
    *PeHeader = (PPECOFF_IMAGE_PE_HEADER)(ImageData + DosHeader->e_lfanew);
    if((*PeHeader)->Signature != PECOFF_IMAGE_NT_SIGNATURE && (*PeHeader)->Signature != PECOFF_IMAGE_XT_SIGNATURE)
    {
        EfiXtLdrProtocol->DbgPrint(L"WARNING: Invalid NT/XT signature found\n");
        return STATUS_EFI_INCOMPATIBLE_VERSION;
    }

    /* Validate optional header */
    if((*PeHeader)->OptionalHeader.Magic != PECOFF_IMAGE_PE_OPTIONAL_HDR32_MAGIC &&
       (*PeHeader)->OptionalHeader.Magic != PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        EfiXtLdrProtocol->DbgPrint(L"WARNING: Invalid optional header signature found\n");
        return STATUS_EFI_INCOMPATIBLE_VERSION;
    }

    /* Return SUCCESS */
    return STATUS_EFI_SUCCESS;
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
EFI_STATUS
BlXtLdrModuleMain(EFI_HANDLE ImageHandle,
                  PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_GUID Guid = XT_PECOFF_IMAGE_PROTOCOL_GUID;
    EFI_HANDLE Handle = NULL;
    EFI_STATUS Status;

    /* Set the system table and image handle */
    EfiImageHandle = ImageHandle;
    EfiSystemTable = SystemTable;

    /* Open the XTLDR protocol */
    Status = BlGetXtLoaderProtocol(&EfiXtLdrProtocol);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open loader protocol */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Set routines available via PE/COFF image protocol */
    XtPeCoffProtocol.Load = PeLoadImage;

    /* Register PE/COFF protocol */
    return EfiSystemTable->BootServices->InstallProtocolInterface(&Handle, &Guid, EFI_NATIVE_INTERFACE, &XtPeCoffProtocol);
}
