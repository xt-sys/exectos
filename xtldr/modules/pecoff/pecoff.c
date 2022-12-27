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
PXT_BOOT_LOADER_PROTOCOL XtLdrProtocol;

/* XTOS PE/COFF Image Protocol */
XT_PECOFF_IMAGE_PROTOCOL XtPeCoffProtocol;

/**
 * Returns the address of the entry point.
 *
 * @param Image
 *        A pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param EntryPoint
 *        A pointer to the memory area where address of the image entry point will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeGetEntryPoint(IN PPECOFF_IMAGE_CONTEXT Image,
                OUT PVOID *EntryPoint)
{
    /* Validate input data */
    if(!Image || !Image->PeHeader)
    {
        /* Invalid parameter passed */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Get entry point and return success */
    *EntryPoint = (PUINT8)Image->VirtualAddress + Image->PeHeader->OptionalHeader.AddressOfEntryPoint;
    return STATUS_EFI_SUCCESS;
}

/**
 * Returns the machine type of the PE/COFF image.
 *
 * @param Image
 *        A pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param MachineType
 *        A pointer to the memory area where a value defined for the 'machine' field will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeGetMachineType(IN PPECOFF_IMAGE_CONTEXT Image,
                 OUT PUSHORT MachineType)
{
    /* Validate input data */
    if(!Image || !Image->PeHeader)
    {
        /* Invalid parameter passed */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Get image machine type and return success */
    *MachineType = Image->PeHeader->FileHeader.Machine;
    return STATUS_EFI_SUCCESS;
}

/**
 * Returns an information about subsystem that is required to run PE/COFF image.
 *
 * @param Image
 *        A pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param SubSystem
 *        A pointer to the memory area storing a value defined for the 'subsystem' field of the image.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeGetSubSystem(IN PPECOFF_IMAGE_CONTEXT Image,
               OUT PUSHORT SubSystem)
{
    /* Validate input data */
    if(!Image || !Image->PeHeader)
    {
        /* Invalid parameter passed */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Get image subsystem and return success */
    *SubSystem = Image->PeHeader->OptionalHeader.Subsystem;
    return STATUS_EFI_SUCCESS;
}

/**
 * Loads a PE/COFF image file.
 *
 * @param FileHandle
 *        The handle of the opened portable executable (PE) file.
 *
 * @param MemoryType
 *        Supplies the type of memory to be assigned to the memory descriptor.
 *
 * @param VirtualAddress
 *        Optional virtual address pointing to the memory area where PE/COFF file will be loaded.
 *
 * @param Image
 *        Supplies pointer to the memory area where loaded PE/COFF image context will be stored.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeLoadImage(IN PEFI_FILE_HANDLE FileHandle,
            IN LOADER_MEMORY_TYPE MemoryType,
            IN PVOID VirtualAddress,
            OUT PPECOFF_IMAGE_CONTEXT *Image)
{
    EFI_GUID FileInfoGuid = EFI_FILE_INFO_PROTOCOL_GUID;
    PPECOFF_IMAGE_SECTION_HEADER SectionHeader;
    PPECOFF_IMAGE_CONTEXT ImageData;
    EFI_PHYSICAL_ADDRESS Address;
    PEFI_FILE_INFO FileInfo;
    UINT_PTR ReadSize;
    EFI_STATUS Status;
    UINT SectionSize;
    SIZE_T Pages;
    PUCHAR Data;
    UINT Index;

    /* Set required size for getting file information */
    ReadSize = sizeof(EFI_FILE_INFO) + 32;

    /* Allocate necessary amount of memory */
    Status = XtLdrProtocol->AllocatePool(ReadSize, (PVOID *)&FileInfo);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        XtLdrProtocol->DbgPrint(L"ERROR: Memory pool allocation failure\n");
        return Status;
    }

    /* First attempt to get file information */
    Status = FileHandle->GetInfo(FileHandle, &FileInfoGuid, &ReadSize, FileInfo);
    if(Status == STATUS_EFI_BUFFER_TOO_SMALL)
    {
        /* Buffer it too small, but EFI tells the required size, let's reallocate */
        XtLdrProtocol->FreePool(&FileInfo);
        Status = XtLdrProtocol->AllocatePool(ReadSize, (PVOID *)&FileInfo);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure */
            XtLdrProtocol->DbgPrint(L"ERROR: Memory pool allocation failure\n");
            return Status;
        }

        /* Second attempt to get file information */
        Status = FileHandle->GetInfo(FileHandle, &FileInfoGuid, &ReadSize, FileInfo);
    }
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to get file information */
        XtLdrProtocol->DbgPrint(L"ERROR: Failed to get file information\n");
        return Status;
    }

    /* Allocate memory for storing image data */
    Status = XtLdrProtocol->AllocatePool(sizeof(PECOFF_IMAGE_CONTEXT), (PVOID *)&ImageData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        XtLdrProtocol->DbgPrint(L"ERROR: Memory pool allocation failure\n");
        return Status;
    }

    /* Store file size and memory type, nullify data and free up memory */
    ImageData->Data = NULL;
    ImageData->FileSize = FileInfo->FileSize;
    ImageData->MemoryType = MemoryType;
    XtLdrProtocol->FreePool(FileInfo);

    /* Calculate number of pages */
    Pages = EFI_SIZE_TO_PAGES(ImageData->FileSize);

    /* Allocate pages */
    Status = XtLdrProtocol->AllocatePages(Pages, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Pages allocation failure */
        XtLdrProtocol->DbgPrint(L"ERROR: Pages allocation failure\n");
        XtLdrProtocol->FreePool(ImageData);
        return Status;
    }

    /* Read PE/COFF image */
    ReadSize = Pages * EFI_PAGE_SIZE;
    Data = (PUCHAR)(UINT_PTR)Address;
    Status = FileHandle->Read(FileHandle, &ReadSize, Data);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to read data */
        XtLdrProtocol->DbgPrint(L"ERROR: Unable to read PE/COFF image file\n");
        XtLdrProtocol->FreePages(Pages, (EFI_PHYSICAL_ADDRESS)(UINT_PTR)Data);
        XtLdrProtocol->FreePool(ImageData);
        return Status;
    }

    /* Extract DOS and PE headers */
    ImageData->DosHeader = (PPECOFF_IMAGE_DOS_HEADER)Data;
    ImageData->PeHeader = (PPECOFF_IMAGE_PE_HEADER)((PUINT8)Data + ImageData->DosHeader->e_lfanew);

    /* Validate headers */
    Status = PepValidateImageHeaders(ImageData->DosHeader, ImageData->PeHeader, ImageData->FileSize);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Header validation failed, probably broken or invalid PE/COFF image */
        XtLdrProtocol->DbgPrint(L"ERROR: Invalid PE/COFF image headers\n");
        XtLdrProtocol->FreePages(Pages, (EFI_PHYSICAL_ADDRESS)(UINT_PTR)Data);
        XtLdrProtocol->FreePool(ImageData);
        return Status;
    }

    /* Make sure image is executable */
    if (!(ImageData->PeHeader->FileHeader.Characteristics & PECOFF_IMAGE_FILE_EXECUTABLE_IMAGE))
    {
        /* Loaded image is not executable */
        XtLdrProtocol->DbgPrint(L"ERROR: Non-executable PE/COFF image loaded\n");
        XtLdrProtocol->FreePages(Pages, (EFI_PHYSICAL_ADDRESS)(UINT_PTR)Data);
        XtLdrProtocol->FreePool(ImageData);
        return STATUS_EFI_LOAD_ERROR;
    }

    /* Store image size and calculate number of image pages */
    ImageData->ImageSize = ImageData->PeHeader->OptionalHeader.SizeOfImage;
    ImageData->ImagePages = EFI_SIZE_TO_PAGES(ImageData->ImageSize);

    /* Allocate image pages */
    Status = XtLdrProtocol->AllocatePages(ImageData->ImagePages, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Pages reallocation failure */
        XtLdrProtocol->DbgPrint(L"ERROR: Pages reallocation failure\n");
        XtLdrProtocol->FreePool(ImageData);
        return Status;
    }

    /* Store image data and virtual address */
    ImageData->Data = (PUINT8)(UINT_PTR)Address;
    ImageData->PhysicalAddress = (PVOID)(UINT_PTR)Address;
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
    RtlCopyMemory(ImageData->Data, Data, ImageData->PeHeader->OptionalHeader.SizeOfHeaders);

    /* Find section header */
    SectionHeader = (PPECOFF_IMAGE_SECTION_HEADER)((PUCHAR)&ImageData->PeHeader->OptionalHeader +
                                                   ImageData->PeHeader->FileHeader.SizeOfOptionalHeader);

    /* Load each section into memory */
    for(Index = 0; Index < ImageData->PeHeader->FileHeader.NumberOfSections; Index++)
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
    XtLdrProtocol->FreePages((EFI_PHYSICAL_ADDRESS)(UINT_PTR)Data, Pages);

    /* Perform relocation fixups */
    Status = PepRelocateLoadedImage(ImageData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to relocate image */
        XtLdrProtocol->DbgPrint(L"ERROR: PE/COFF image relocation failed\n");
        return Status;
    }

    /* Store image data */
    *Image = ImageData;

    /* Return SUCCESS */
    return STATUS_EFI_SUCCESS;
}

/**
 * Relocates PE/COFF image to the specified address.
 *
 * @param Image
 *        A pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param Address
 *        Destination address of memory region, where image should be relocated.
 *
 * @return This routine returns status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeRelocateImage(IN PPECOFF_IMAGE_CONTEXT Image,
                IN EFI_VIRTUAL_ADDRESS Address)
{
    UINT64 ImageBase, OldVirtualAddress;
    EFI_STATUS Status;

    /* Store original virtual address */
    OldVirtualAddress = (UINT_PTR)Image->VirtualAddress;

    /* Check PE/COFF image type */
    if(Image->PeHeader->OptionalHeader.Magic == PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* This is 64-bit PE32+, store its image base address */
        ImageBase = Image->PeHeader->OptionalHeader.ImageBase64;
    }
    else
    {
        /* This is 32-bit PE32, store its image base address */
        ImageBase = Image->PeHeader->OptionalHeader.ImageBase32;
    }

    /* Overwrite virtual address and relocate image once again */
    Image->VirtualAddress = (PVOID)(Address - OldVirtualAddress + ImageBase);
    Status = PepRelocateLoadedImage(Image);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Relocation failed */
        return Status;
    }

    /* Store new image virtual address */
    Image->VirtualAddress = (PVOID)Address;

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Relocates a loaded PE/COFF image.
 *
 * @param Image
 *        A pointer to the PE/COFF context structure representing the loaded image.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PepRelocateLoadedImage(IN PPECOFF_IMAGE_CONTEXT Image)
{
    PPECOFF_IMAGE_BASE_RELOCATION RelocationDir, RelocationEnd;
    PPECOFF_IMAGE_DATA_DIRECTORY DataDirectory;
    USHORT Offset, Type, Count;
    PUSHORT TypeOffset;
    UINT64 ImageBase;
    PUINT32 Address;
    PUINT64 LongPtr;
    PUINT ShortPtr;

    /* Make sure image is not stripped */
    if(Image->PeHeader->FileHeader.Characteristics & PECOFF_IMAGE_FILE_RELOCS_STRIPPED)
    {
        /* No relocation information found */
        return STATUS_EFI_UNSUPPORTED;
    }

    /* Set relocation data directory */
    DataDirectory = &Image->PeHeader->OptionalHeader.DataDirectory[PECOFF_IMAGE_DIRECTORY_ENTRY_BASERELOC];

    /* Check if loaded image should be relocated */
    if(Image->PeHeader->OptionalHeader.NumberOfRvaAndSizes <= PECOFF_IMAGE_DIRECTORY_ENTRY_BASERELOC ||
       DataDirectory->VirtualAddress == 0 || DataDirectory->Size < sizeof(PECOFF_IMAGE_BASE_RELOCATION))
    {
        /* No need to relocate the image */
        return STATUS_EFI_SUCCESS;
    }

    /* Check PE/COFF image type */
    if(Image->PeHeader->OptionalHeader.Magic == PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* This is 64-bit PE32+, store its image base address */
        ImageBase = Image->PeHeader->OptionalHeader.ImageBase64;
    }
    else
    {
        /* This is 32-bit PE32, store its image base address */
        ImageBase = Image->PeHeader->OptionalHeader.ImageBase32;
    }

    /* Set relocation pointers */
    RelocationDir = (PPECOFF_IMAGE_BASE_RELOCATION)((ULONG_PTR)Image->Data + DataDirectory->VirtualAddress);
    RelocationEnd = (PPECOFF_IMAGE_BASE_RELOCATION)((ULONG_PTR)RelocationDir + DataDirectory->Size);

    /* Do relocations */
    while(RelocationDir < RelocationEnd && RelocationDir->SizeOfBlock > 0)
    {
        /* Calculate number of relocations needed, address and type offset */
        Count = (RelocationDir->SizeOfBlock - sizeof(PECOFF_IMAGE_BASE_RELOCATION)) / sizeof(UINT16);
        Address = (UINT32*)((UINT8*)Image->Data + RelocationDir->VirtualAddress);
        TypeOffset = (UINT16*)((UINT8*)RelocationDir + sizeof(PECOFF_IMAGE_BASE_RELOCATION));

        /* Do relocations */
        while(Count--)
        {
            /* Calculate offset and relocation type */
            Offset = *TypeOffset & 0xFFF;
            Type = *TypeOffset >> 12;

            /* Check if end of the loaded address reached */
            if((PVOID)(PUSHORT)(Address + Offset) >= Image->Data + Image->ImageSize)
            {
                /* Do not relocate after the end of loaded image */
                break;
            }

            /* Make sure we are not going to relocate into .reloc section */
            if((ULONG_PTR)(Address + Offset) < (ULONG_PTR)RelocationDir ||
               (ULONG_PTR)(Address + Offset) >= (ULONG_PTR)RelocationEnd)
            {
                /* Apply relocation fixup */
                switch (Type)
                {
                    case PECOFF_IMAGE_REL_BASED_ABSOLUTE:
                        /* No relocation required */
                        break;
                    case PECOFF_IMAGE_REL_BASED_DIR64:
                        /* 64-bit relocation */
                        LongPtr = (UINT64*)((UINT8*)Address + Offset);
                        *LongPtr = *LongPtr - ImageBase + (UINT_PTR)Image->VirtualAddress;
                        break;
                    case PECOFF_IMAGE_REL_BASED_HIGHLOW:
                        /* 32-bit relocation of hight and low half of address */
                        ShortPtr = (UINT32*)((UINT8*)Address + Offset);
                        *ShortPtr = *ShortPtr - ImageBase + (UINT_PTR)Image->VirtualAddress;
                        break;
                    default:
                        /* Unknown or unsupported relocation type */
                        return STATUS_EFI_UNSUPPORTED;
                }
            }
            /* Increment the type offset */
            TypeOffset++;
        }
        /* Next relocation */
        RelocationDir += RelocationDir->SizeOfBlock;
    }

    /* Return SUCCESS */
    return STATUS_EFI_SUCCESS;
}

/**
 * Validates a PE/COFF image headers.
 *
 * @param DosHeader
 *        Pointer to the memory area with DOS header stored.
 *
 * @param PeHeader
 *        Pointer to the memory area with PE header stored.
 *
 * @param FileSize
 *        A PE/COFF image file size.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PepValidateImageHeaders(IN PPECOFF_IMAGE_DOS_HEADER DosHeader,
                        IN PPECOFF_IMAGE_PE_HEADER PeHeader,
                        IN SIZE_T FileSize)
{
    /* Validate file size */
    if(FileSize < sizeof(PECOFF_IMAGE_DOS_HEADER))
    {
        XtLdrProtocol->DbgPrint(L"WARNING: PE/COFF image shorter than DOS header\n");
        return STATUS_EFI_END_OF_FILE;
    }

    /* Validate DOS header */
    if(DosHeader->e_magic != PECOFF_IMAGE_DOS_SIGNATURE)
    {
        XtLdrProtocol->DbgPrint(L"WARNING: Invalid DOS signature found\n");
        return STATUS_EFI_INCOMPATIBLE_VERSION;
    }

    /* Validate PE header */
    if(PeHeader->Signature != PECOFF_IMAGE_NT_SIGNATURE && PeHeader->Signature != PECOFF_IMAGE_XT_SIGNATURE)
    {
        XtLdrProtocol->DbgPrint(L"WARNING: Invalid NT/XT signature found\n");
        return STATUS_EFI_INCOMPATIBLE_VERSION;
    }

    /* Validate optional header */
    if(PeHeader->OptionalHeader.Magic != PECOFF_IMAGE_PE_OPTIONAL_HDR32_MAGIC &&
       PeHeader->OptionalHeader.Magic != PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        XtLdrProtocol->DbgPrint(L"WARNING: Invalid optional header signature found\n");
        return STATUS_EFI_INCOMPATIBLE_VERSION;
    }

    /* Return success */
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
XTCDECL
EFI_STATUS
BlXtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                  IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_GUID Guid = XT_PECOFF_IMAGE_PROTOCOL_GUID;
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

    /* Set routines available via PE/COFF image protocol */
    XtPeCoffProtocol.GetEntryPoint = PeGetEntryPoint;
    XtPeCoffProtocol.GetMachineType = PeGetMachineType;
    XtPeCoffProtocol.GetSubSystem = PeGetSubSystem;
    XtPeCoffProtocol.Load = PeLoadImage;
    XtPeCoffProtocol.Relocate = PeRelocateImage;

    /* Register PE/COFF protocol */
    return EfiSystemTable->BootServices->InstallProtocolInterface(&Handle, &Guid, EFI_NATIVE_INTERFACE,
                                                                  &XtPeCoffProtocol);
}
