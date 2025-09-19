/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/pecoff/pecoff.cc
 * DESCRIPTION:     Basic PE/COFF executable file format support module
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <pecoff.hh>


/* PE/COFF_O module information */
MODULE_AUTHOR(L"Rafal Kupiec <belliash@codingworkshop.eu.org>");
MODULE_DESCRIPTION(L"Basic PE/COFF executable file format support");
MODULE_LICENSE(L"GPLv3");
MODULE_VERSION(L"0.1");


/**
 * Returns the address of the entry point.
 *
 * @param ImagePointer
 *        Supplies a pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param EntryPoint
 *        Supplies a pointer to the memory area where address of the image entry point will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::GetEntryPoint(IN PVOID ImagePointer,
                      OUT PVOID *EntryPoint)
{
    PPECOFF_IMAGE_CONTEXT Image;

    /* Get PE/COFF image pointer*/
    Image = (PPECOFF_IMAGE_CONTEXT)ImagePointer;

    /* Validate input data */
    if(!Image || !Image->PeHeader)
    {
        /* Invalid parameter passed */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Check PE/COFF image type */
    if(Image->PeHeader->OptionalHeader32.Magic == PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* Get entry point from 64-bit optional header */
        *EntryPoint = (PUCHAR)Image->VirtualAddress + Image->PeHeader->OptionalHeader64.AddressOfEntryPoint;
    }
    else
    {
        /* Get entry point from 32-bit optional header */
        *EntryPoint = (PUCHAR)Image->VirtualAddress + Image->PeHeader->OptionalHeader32.AddressOfEntryPoint;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Returns the size of the loaded PE/COFF file.
 *
 * @param ImagePointer
 *        Supplies a pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param ImageSize
 *        Supplies a pointer to the memory area where file size will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::GetFileSize(IN PVOID ImagePointer,
                    OUT PULONGLONG FileSize)
{
    PPECOFF_IMAGE_CONTEXT Image;

    /* Get PE/COFF image pointer*/
    Image = (PPECOFF_IMAGE_CONTEXT)ImagePointer;

    /* Validate input data */
    if(!Image || !Image->ImageSize)
    {
        /* Invalid parameter passed */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Get image size and return success */
    *FileSize = Image->FileSize;
    return STATUS_EFI_NOT_FOUND;
}

/**
 * Returns the size of the loaded PE/COFF image.
 *
 * @param ImagePointer
 *        Supplies a pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param ImageSize
 *        Supplies a pointer to the memory area where image size will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::GetImageSize(IN PVOID ImagePointer,
                     OUT PUINT ImageSize)
{
    PPECOFF_IMAGE_CONTEXT Image;

    /* Get PE/COFF image pointer*/
    Image = (PPECOFF_IMAGE_CONTEXT)ImagePointer;

    /* Validate input data */
    if(!Image || !Image->ImageSize)
    {
        /* Invalid parameter passed */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Get image size and return success */
    *ImageSize = Image->ImageSize;
    return STATUS_EFI_NOT_FOUND;
}

/**
 * Returns the machine type of the PE/COFF image.
 *
 * @param ImagePointer
 *        Supplies a pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param MachineType
 *        Supplies a pointer to the memory area where a value defined for the 'machine' field will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::GetMachineType(IN PVOID ImagePointer,
                       OUT PUSHORT MachineType)
{
    PPECOFF_IMAGE_CONTEXT Image;

    /* Get PE/COFF image pointer*/
    Image = (PPECOFF_IMAGE_CONTEXT)ImagePointer;

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
 * Returns an address to the specified section in the PE/COFF image.
 *
 * @param ImagePointer
 *        Supplies a pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param SectionName
 *        Supplies a name of the requested section.
 *
 * @param RawData
 *        Supplies a pointer to the memory area where the address of the requested section will be stored.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::GetSection(IN PVOID ImagePointer,
                   IN PCHAR SectionName,
                   OUT PULONG *RawData)
{
    PPECOFF_IMAGE_SECTION_HEADER SectionHeader;
    PPECOFF_IMAGE_CONTEXT Image;
    SIZE_T SectionNameLength;
    USHORT SectionIndex;

    /* Get PE/COFF image pointer*/
    Image = (PPECOFF_IMAGE_CONTEXT)ImagePointer;

    /* Validate input data */
    if(!Image || !Image->PeHeader)
    {
        /* Invalid parameter passed */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Check PE/COFF image type */
    if(Image->PeHeader->OptionalHeader32.Magic == PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* Find section header in 64-bit optional header */
        SectionHeader = (PPECOFF_IMAGE_SECTION_HEADER)((PUCHAR)&Image->PeHeader->OptionalHeader64 +
                                                       Image->PeHeader->FileHeader.SizeOfOptionalHeader);
    }
    else
    {
        /* Find section header in 32-bit optional header */
        SectionHeader = (PPECOFF_IMAGE_SECTION_HEADER)((PUCHAR)&Image->PeHeader->OptionalHeader32 +
                                                       Image->PeHeader->FileHeader.SizeOfOptionalHeader);
    }

    /* Get section name length */
    SectionNameLength = XtLdrProtocol->String.Length(SectionName, 0);

    /* Iterate through all image sections */
    for(SectionIndex = 0; SectionIndex < Image->PeHeader->FileHeader.NumberOfSections; SectionIndex++)
    {
        /* Check section name */
        if(XtLdrProtocol->String.Compare((PCHAR)SectionHeader[SectionIndex].Name, SectionName, SectionNameLength) == 0)
        {
            /* Store section address and return */
            *RawData = (PULONG)((PUCHAR)Image->Data + SectionHeader[SectionIndex].PointerToRawData);
            return STATUS_EFI_SUCCESS;
        }
    }

    /* Section not found if reached here */
    return STATUS_EFI_NOT_FOUND;
}

/**
 * Returns an information about subsystem that is required to run PE/COFF image.
 *
 * @param ImagePointer
 *        Supplies a pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param SubSystem
 *        Supplies a pointer to the memory area storing a value defined for the 'subsystem' field of the image.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::GetSubSystem(IN PVOID ImagePointer,
                     OUT PUSHORT SubSystem)
{
    PPECOFF_IMAGE_CONTEXT Image;

    /* Get PE/COFF image pointer*/
    Image = (PPECOFF_IMAGE_CONTEXT)ImagePointer;

    /* Validate input data */
    if(!Image || !Image->PeHeader)
    {
        /* Invalid parameter passed */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Check PE/COFF image type */
    if(Image->PeHeader->OptionalHeader32.Magic == PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* Get image subsystem from 64-bit optional header */
        *SubSystem = Image->PeHeader->OptionalHeader64.Subsystem;
    }
    else
    {
        /* Get image subsystem from 32-bit optional header */
        *SubSystem = Image->PeHeader->OptionalHeader32.Subsystem;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Returns an information about major image version.
 *
 * @param ImagePointer
 *        Supplies a pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param Version
 *        Supplies a pointer to the memory area storing a major image version.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::GetVersion(IN PVOID ImagePointer,
                   OUT PUSHORT Version)
{
    PPECOFF_IMAGE_CONTEXT Image;

    /* Get PE/COFF image pointer*/
    Image = (PPECOFF_IMAGE_CONTEXT)ImagePointer;

    /* Validate input data */
    if(!Image || !Image->PeHeader)
    {
        /* Invalid parameter passed */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Check PE/COFF image type */
    if(Image->PeHeader->OptionalHeader32.Magic == PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* Get image major version from 64-bit optional header */
        *Version = Image->PeHeader->OptionalHeader64.MajorImageVersion;
    }
    else
    {
        /* Get image major version from 32-bit optional header */
        *Version = Image->PeHeader->OptionalHeader32.MajorImageVersion;
    }

    /* Return success */
    return STATUS_EFI_SUCCESS;
}

/**
 * Initializes PECOFF module by opening XTLDR protocol and installing PECOFF protocol.
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
PeCoff::InitializeModule(IN EFI_HANDLE ImageHandle,
                         IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_GUID Guid = XT_PECOFF_IMAGE_PROTOCOL_GUID;
    EFI_STATUS Status;

    /* Open the XTLDR protocol */
    Status = BlGetXtLdrProtocol(SystemTable, ImageHandle, &XtLdrProtocol);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open loader protocol */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Set routines available via PE/COFF image protocol */
    PeProtocol.GetEntryPoint = GetEntryPoint;
    PeProtocol.GetFileSize = GetFileSize;
    PeProtocol.GetImageSize = GetImageSize;
    PeProtocol.GetMachineType = GetMachineType;
    PeProtocol.GetSection = GetSection;
    PeProtocol.GetSubSystem = GetSubSystem;
    PeProtocol.GetVersion = GetVersion;
    PeProtocol.LoadImage = LoadImage;
    PeProtocol.RelocateImage = RelocateImage;
    PeProtocol.UnloadImage = UnloadImage;
    PeProtocol.VerifyImage = VerifyImage;

    /* Register PE/COFF protocol */
    return XtLdrProtocol->Protocol.Install(&PeProtocol, &Guid);
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
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::LoadImage(IN PEFI_FILE_HANDLE FileHandle,
                  IN LOADER_MEMORY_TYPE MemoryType,
                  IN PVOID VirtualAddress,
                  OUT PVOID *ImagePointer)
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
    Status = XtLdrProtocol->Memory.AllocatePool(ReadSize, (PVOID *)&FileInfo);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        XtLdrProtocol->Debug.Print(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* First attempt to get file information */
    Status = FileHandle->GetInfo(FileHandle, &FileInfoGuid, &ReadSize, FileInfo);
    if(Status == STATUS_EFI_BUFFER_TOO_SMALL)
    {
        /* Buffer it too small, but EFI tells the required size, let's reallocate */
        XtLdrProtocol->Memory.FreePool(&FileInfo);
        Status = XtLdrProtocol->Memory.AllocatePool(ReadSize, (PVOID *)&FileInfo);
        if(Status != STATUS_EFI_SUCCESS)
        {
            /* Memory allocation failure */
            XtLdrProtocol->Debug.Print(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
            return Status;
        }

        /* Second attempt to get file information */
        Status = FileHandle->GetInfo(FileHandle, &FileInfoGuid, &ReadSize, FileInfo);
    }
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Unable to get file information */
        XtLdrProtocol->Debug.Print(L"ERROR: Failed to get PE/COFF file information (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Allocate memory for storing image data */
    Status = XtLdrProtocol->Memory.AllocatePool(sizeof(PECOFF_IMAGE_CONTEXT), (PVOID *)&ImageData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Memory allocation failure */
        XtLdrProtocol->Debug.Print(L"ERROR: Memory allocation failure (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Store file size and memory type, nullify data and free up memory */
    ImageData->Data = NULLPTR;
    ImageData->FileSize = FileInfo->FileSize;
    ImageData->MemoryType = MemoryType;
    XtLdrProtocol->Memory.FreePool(FileInfo);

    /* Calculate number of pages */
    Pages = EFI_SIZE_TO_PAGES(ImageData->FileSize);

    /* Allocate pages */
    Status = XtLdrProtocol->Memory.AllocatePages(AllocateAnyPages, Pages, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Pages allocation failure */
        XtLdrProtocol->Debug.Print(L"ERROR: Pages allocation failure (Status Code: 0x%zX)\n", Status);
        XtLdrProtocol->Memory.FreePool(ImageData);
        return Status;
    }

    /* Read PE/COFF image */
    ReadSize = Pages * EFI_PAGE_SIZE;
    Data = (PUCHAR)(UINT_PTR)Address;
    Status = FileHandle->Read(FileHandle, &ReadSize, Data);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to read data */
        XtLdrProtocol->Debug.Print(L"ERROR: Failed to read PE/COFF image file (Status Code: 0x%zX)\n", Status);
        XtLdrProtocol->Memory.FreePages(Pages, (EFI_PHYSICAL_ADDRESS)(UINT_PTR)Data);
        XtLdrProtocol->Memory.FreePool(ImageData);
        return Status;
    }

    /* Extract DOS and PE headers */
    ImageData->DosHeader = (PPECOFF_IMAGE_DOS_HEADER)Data;
    ImageData->PeHeader = (PPECOFF_IMAGE_PE_HEADER)((PUCHAR)Data + ImageData->DosHeader->PeHeaderOffset);

    /* Validate headers */
    Status = PeCoff::VerifyImage(ImageData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Header validation failed, probably broken or invalid PE/COFF image */
        XtLdrProtocol->Debug.Print(L"ERROR: Invalid PE/COFF image headers (Status Code: 0x%zX)\n", Status);
        XtLdrProtocol->Memory.FreePages(Pages, (EFI_PHYSICAL_ADDRESS)(UINT_PTR)Data);
        XtLdrProtocol->Memory.FreePool(ImageData);
        return Status;
    }

    /* Make sure image is executable */
    if (!(ImageData->PeHeader->FileHeader.Characteristics & PECOFF_IMAGE_FILE_EXECUTABLE_IMAGE))
    {
        /* Loaded image is not executable */
        XtLdrProtocol->Debug.Print(L"ERROR: Non-executable PE/COFF image loaded\n");
        XtLdrProtocol->Memory.FreePages(Pages, (EFI_PHYSICAL_ADDRESS)(UINT_PTR)Data);
        XtLdrProtocol->Memory.FreePool(ImageData);
        return STATUS_EFI_LOAD_ERROR;
    }

    /* Store image size depending on the PE/COFF image type */
    if(ImageData->PeHeader->OptionalHeader32.Magic == PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* Store 64-bit image size */
        ImageData->ImageSize = ImageData->PeHeader->OptionalHeader64.SizeOfImage;
    }
    else
    {
        /* Store 32-bit image size */
        ImageData->ImageSize = ImageData->PeHeader->OptionalHeader32.SizeOfImage;
    }

    /* Calculate number of image pages */
    ImageData->ImagePages = EFI_SIZE_TO_PAGES(ImageData->ImageSize);

    /* Allocate image pages */
    Status = XtLdrProtocol->Memory.AllocatePages(AllocateAnyPages, ImageData->ImagePages, &Address);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Pages reallocation failure */
        XtLdrProtocol->Debug.Print(L"ERROR: Pages reallocation failure (Status Code: 0x%zX)\n", Status);
        XtLdrProtocol->Memory.FreePool(ImageData);
        return Status;
    }

    /* Store image data and virtual address */
    ImageData->Data = (PUCHAR)(UINT_PTR)Address;
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

    /* Check the PE/COFF image type */
    if(ImageData->PeHeader->OptionalHeader32.Magic == PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* Copy all PE32+ sections */
        XtLdrProtocol->Memory.CopyMemory(ImageData->Data, Data, ImageData->PeHeader->OptionalHeader64.SizeOfHeaders);

        /* Find PE32+ section header */
        SectionHeader = (PPECOFF_IMAGE_SECTION_HEADER)((PUCHAR)&ImageData->PeHeader->OptionalHeader64 +
                                                       ImageData->PeHeader->FileHeader.SizeOfOptionalHeader);
    }
    else
    {
        /* Copy all PE32 sections */
        XtLdrProtocol->Memory.CopyMemory(ImageData->Data, Data, ImageData->PeHeader->OptionalHeader64.SizeOfHeaders);

        /* Find PE32 section header */
        SectionHeader = (PPECOFF_IMAGE_SECTION_HEADER)((PUCHAR)&ImageData->PeHeader->OptionalHeader64 +
                                                       ImageData->PeHeader->FileHeader.SizeOfOptionalHeader);
    }

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
            XtLdrProtocol->Memory.CopyMemory((PUCHAR)ImageData->Data + SectionHeader[Index].VirtualAddress,
                                             Data + SectionHeader[Index].PointerToRawData, SectionSize);
        }

        /* Check if raw size is shorter than virtual size */
        if(SectionSize < SectionHeader[Index].Misc.VirtualSize)
        {
            /* Fill remaining space with zeroes */
            XtLdrProtocol->Memory.ZeroMemory((PUCHAR)ImageData->Data + SectionHeader[Index].VirtualAddress + SectionSize,
                                             SectionHeader[Index].Misc.VirtualSize - SectionSize);
        }
    }

    /* Free pages */
    XtLdrProtocol->Memory.FreePages((EFI_PHYSICAL_ADDRESS)(UINT_PTR)Data, Pages);

    /* Perform relocation fixups */
    Status = PeCoff::RelocateLoadedImage(ImageData);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to relocate image */
        XtLdrProtocol->Debug.Print(L"ERROR: PE/COFF image relocation failed (Status Code: 0x%zX)\n", Status);
        return Status;
    }

    /* Store image data */
    *ImagePointer = ImageData;

    /* Return SUCCESS */
    return STATUS_EFI_SUCCESS;
}

/**
 * Relocates PE/COFF image to the specified address.
 *
 * @param ImagePointer
 *        Supplies a pointer to the PE/COFF context structure representing the loaded image.
 *
 * @param Address
 *        Specifies destination address of memory region, where image should be relocated.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::RelocateImage(IN PVOID ImagePointer,
                      IN EFI_VIRTUAL_ADDRESS Address)
{
    PPECOFF_IMAGE_CONTEXT Image;
    ULONGLONG ImageBase, OldVirtualAddress;
    EFI_STATUS Status;

    /* Get PE/COFF image pointer*/
    Image = (PPECOFF_IMAGE_CONTEXT)ImagePointer;

    /* Store original virtual address */
    OldVirtualAddress = (UINT_PTR)Image->VirtualAddress;

    /* Check PE/COFF image type */
    if(Image->PeHeader->OptionalHeader32.Magic == PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* This is 64-bit PE32+, store its image base address */
        ImageBase = Image->PeHeader->OptionalHeader64.ImageBase;
    }
    else
    {
        /* This is 32-bit PE32, store its image base address */
        ImageBase = Image->PeHeader->OptionalHeader32.ImageBase;
    }

    /* Overwrite virtual address and relocate image once again */
    Image->VirtualAddress = (PVOID)(Address - OldVirtualAddress + ImageBase);
    Status = PeCoff::RelocateLoadedImage(Image);
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
 *        Supplies a pointer to the PE/COFF context structure representing the loaded image.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::RelocateLoadedImage(IN PPECOFF_IMAGE_CONTEXT Image)
{
    PPECOFF_IMAGE_BASE_RELOCATION RelocationDir, RelocationEnd;
    PPECOFF_IMAGE_DATA_DIRECTORY DataDirectory;
    USHORT Offset, Type, Count;
    PUSHORT TypeOffset;
    ULONGLONG ImageBase;
    PUINT Address;
    PULONGLONG LongPtr;
    PUINT ShortPtr;

    /* Make sure image is not stripped */
    if(Image->PeHeader->FileHeader.Characteristics & PECOFF_IMAGE_FILE_RELOCS_STRIPPED)
    {
        /* No relocation information found */
        XtLdrProtocol->Debug.Print(L"WARNING: PE/COFF image is stripped and contains no information about relocations\n");
        return STATUS_EFI_SUCCESS;
    }

    /* Check PE/COFF image type */
    if(Image->PeHeader->OptionalHeader32.Magic == PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* Set relocation data directory and image base address */
        DataDirectory = &Image->PeHeader->OptionalHeader64.DataDirectory[PECOFF_IMAGE_DIRECTORY_ENTRY_BASERELOC];
        ImageBase = Image->PeHeader->OptionalHeader64.ImageBase;

        /* Check if loaded 64-bit PE32+ image should be relocated */
        if(Image->PeHeader->OptionalHeader64.NumberOfRvaAndSizes <= PECOFF_IMAGE_DIRECTORY_ENTRY_BASERELOC ||
        DataDirectory->VirtualAddress == 0 || DataDirectory->Size < sizeof(PECOFF_IMAGE_BASE_RELOCATION))
        {
            /* No need to relocate the image */
            return STATUS_EFI_SUCCESS;
        }
    }
    else
    {
        /* Check if loaded 32-bit PE32 image should be relocated */
        /* Set relocation data directory and image base address */
        DataDirectory = &Image->PeHeader->OptionalHeader32.DataDirectory[PECOFF_IMAGE_DIRECTORY_ENTRY_BASERELOC];
        ImageBase = Image->PeHeader->OptionalHeader32.ImageBase;

        if(Image->PeHeader->OptionalHeader32.NumberOfRvaAndSizes <= PECOFF_IMAGE_DIRECTORY_ENTRY_BASERELOC ||
        DataDirectory->VirtualAddress == 0 || DataDirectory->Size < sizeof(PECOFF_IMAGE_BASE_RELOCATION))
        {
            /* No need to relocate the image */
            return STATUS_EFI_SUCCESS;
        }
    }

    /* Set relocation pointers */
    RelocationDir = (PPECOFF_IMAGE_BASE_RELOCATION)((ULONG_PTR)Image->Data + DataDirectory->VirtualAddress);
    RelocationEnd = (PPECOFF_IMAGE_BASE_RELOCATION)((ULONG_PTR)RelocationDir + DataDirectory->Size);

    /* Do relocations */
    while(RelocationDir < RelocationEnd && RelocationDir->SizeOfBlock > 0)
    {
        /* Calculate number of relocations needed, address and type offset */
        Count = (RelocationDir->SizeOfBlock - sizeof(PECOFF_IMAGE_BASE_RELOCATION)) / sizeof(USHORT);
        Address = (PUINT)((PUCHAR)Image->Data + RelocationDir->VirtualAddress);
        TypeOffset = (PUSHORT)((PUCHAR)RelocationDir + sizeof(PECOFF_IMAGE_BASE_RELOCATION));

        /* Do relocations */
        while(Count--)
        {
            /* Calculate offset and relocation type */
            Offset = *TypeOffset & 0xFFF;
            Type = *TypeOffset >> 12;

            /* Check if end of the loaded address reached */
            if((PVOID)(PUSHORT)(Address + Offset) >= (PUCHAR)Image->Data + Image->ImageSize)
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
                        LongPtr = (PULONGLONG)((PUCHAR)Address + Offset);
                        *LongPtr = *LongPtr - ImageBase + (UINT_PTR)Image->VirtualAddress;
                        break;
                    case PECOFF_IMAGE_REL_BASED_HIGHLOW:
                        /* 32-bit relocation of hight and low half of address */
                        ShortPtr = (PUINT)((PUCHAR)Address + Offset);
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
        RelocationDir = (PPECOFF_IMAGE_BASE_RELOCATION)((PUCHAR)RelocationDir + RelocationDir->SizeOfBlock);
    }

    /* Return SUCCESS */
    return STATUS_EFI_SUCCESS;
}

/**
 * Unloads a PE/COFF image file and frees allocated memory.
 *
 * @param ImagePointer
 *        Supplies a pointer to the PE/COFF context structure representing the loaded image.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::UnloadImage(IN PVOID ImagePointer)
{
    PPECOFF_IMAGE_CONTEXT Image;
    EFI_STATUS Status;

    /* Get PE/COFF image pointer*/
    Image = (PPECOFF_IMAGE_CONTEXT)ImagePointer;

    /* Validate input data */
    if(!Image || !Image->Data)
    {
        /* Invalid parameter passed */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Free memory allocated for the image */
    Status = XtLdrProtocol->Memory.FreePages(Image->ImagePages, (EFI_PHYSICAL_ADDRESS)(UINT_PTR)Image->Data);
    Status |= XtLdrProtocol->Memory.FreePool(Image);

    /* Return status */
    return Status;
}

/**
 * Validates a PE/COFF image headers.
 *
 * @param ImagePointer
 *        Supplies a pointer to the PE/COFF context structure representing the loaded image.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
PeCoff::VerifyImage(IN PVOID ImagePointer)
{
    PPECOFF_IMAGE_CONTEXT Image;

    /* Get PE/COFF image pointer*/
    Image = (PPECOFF_IMAGE_CONTEXT)ImagePointer;

    /* Validate input data */
    if(!Image || !Image->PeHeader)
    {
        /* Invalid parameter passed */
        return STATUS_EFI_INVALID_PARAMETER;
    }

    /* Validate file size */
    if(Image->FileSize < sizeof(PECOFF_IMAGE_DOS_HEADER))
    {
        /* PE/COFF image shorter than DOS header, return error*/
        return STATUS_EFI_END_OF_FILE;
    }

    /* Validate DOS header */
    if(Image->DosHeader->Magic != PECOFF_IMAGE_DOS_SIGNATURE)
    {
        /* Invalid DOS signature, return error */
        return STATUS_EFI_INCOMPATIBLE_VERSION;
    }

    /* Validate PE header */
    if(Image->PeHeader->Signature != PECOFF_IMAGE_NT_SIGNATURE &&
       Image->PeHeader->Signature != PECOFF_IMAGE_XT_SIGNATURE)
    {
        /* Invalid PE signature, return error */
        return STATUS_EFI_INCOMPATIBLE_VERSION;
    }

    /* Validate optional header */
    if(Image->PeHeader->OptionalHeader32.Magic != PECOFF_IMAGE_PE_OPTIONAL_HDR32_MAGIC &&
       Image->PeHeader->OptionalHeader64.Magic != PECOFF_IMAGE_PE_OPTIONAL_HDR64_MAGIC)
    {
        /* Invalid optional header signature, return error */
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
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable)
{
    /* Initialize PECOFF module */
    return PeCoff::InitializeModule(ImageHandle, SystemTable);
}
