/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/pecoff/includes/pecoff.hh
 * DESCRIPTION:     Basic PE/COFF executable file format support header
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_PECOFF_HH
#define __XTLDR_PECOFF_HH

#include <xtblapi.h>


/* PE/COFF module for XTLDR */
class PeCoff
{
    private:
        STATIC XTBL_EXECUTABLE_IMAGE_PROTOCOL PeProtocol;
        STATIC PXTBL_LOADER_PROTOCOL XtLdrProtocol;

    public:
        STATIC XTCDECL EFI_STATUS GetEntryPoint(IN PVOID ImagePointer,
                                                OUT PVOID *EntryPoint);
        STATIC XTCDECL EFI_STATUS GetFileSize(IN PVOID ImagePointer,
                                              OUT PULONGLONG FileSize);
        STATIC XTCDECL EFI_STATUS GetImageSize(IN PVOID ImagePointer,
                                               OUT PUINT ImageSize);
        STATIC XTCDECL EFI_STATUS GetMachineType(IN PVOID ImagePointer,
                                                 OUT PUSHORT MachineType);
        STATIC XTCDECL EFI_STATUS GetSection(IN PVOID ImagePointer,
                                             IN PCHAR SectionName,
                                             OUT PULONG *RawData);
        STATIC XTCDECL EFI_STATUS GetSubSystem(IN PVOID ImagePointer,
                                               OUT PUSHORT SubSystem);
        STATIC XTCDECL EFI_STATUS GetVersion(IN PVOID ImagePointer,
                                             OUT PUSHORT Version);
        STATIC XTCDECL EFI_STATUS InitializeModule(IN EFI_HANDLE ImageHandle,
                                                   IN PEFI_SYSTEM_TABLE SystemTable);
        STATIC XTCDECL EFI_STATUS LoadImage(IN PEFI_FILE_HANDLE FileHandle,
                                            IN LOADER_MEMORY_TYPE MemoryType,
                                            IN PVOID VirtualAddress,
                                            OUT PVOID *ImagePointer);
        STATIC XTCDECL EFI_STATUS RelocateImage(IN PVOID ImagePointer,
                                                IN EFI_VIRTUAL_ADDRESS Address);
        STATIC XTCDECL EFI_STATUS UnloadImage(IN PVOID ImagePointer);
        STATIC XTCDECL EFI_STATUS VerifyImage(IN PVOID ImagePointer);

    private:
        STATIC XTCDECL EFI_STATUS RelocateLoadedImage(IN PPECOFF_IMAGE_CONTEXT Image);
};

#endif /* __XTLDR_PECOFF_HH */
