/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/pecoff/includes/pecoff.h
 * DESCRIPTION:     Basic PE/COFF executable file format support header
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_PECOFF_H
#define __XTLDR_PECOFF_H

#include <xtblapi.h>
#include <globals.h>


/* PE/COFF image protocol related routines forward references */
XTCDECL
EFI_STATUS
PeGetEntryPoint(IN PVOID ImagePointer,
                OUT PVOID *EntryPoint);

XTCDECL
EFI_STATUS
PeGetFileSize(IN PVOID ImagePointer,
              OUT PULONGLONG FileSize);

XTCDECL
EFI_STATUS
PeGetImageSize(IN PVOID ImagePointer,
               OUT PUINT ImageSize);

XTCDECL
EFI_STATUS
PeGetMachineType(IN PVOID ImagePointer,
                 OUT PUSHORT MachineType);

XTCDECL
EFI_STATUS
PeGetSection(IN PVOID ImagePointer,
             IN PCHAR SectionName,
             OUT PULONG *RawData);

XTCDECL
EFI_STATUS
PeGetSubSystem(IN PVOID ImagePointer,
               OUT PUSHORT SubSystem);

XTCDECL
EFI_STATUS
PeGetVersion(IN PVOID ImagePointer,
             OUT PUSHORT Version);

XTCDECL
EFI_STATUS
PeLoadImage(IN PEFI_FILE_HANDLE FileHandle,
            IN LOADER_MEMORY_TYPE MemoryType,
            IN PVOID VirtualAddress,
            OUT PVOID *ImagePointer);

XTCDECL
EFI_STATUS
PeRelocateImage(IN PVOID ImagePointer,
                IN EFI_VIRTUAL_ADDRESS Address);

XTCDECL
EFI_STATUS
PeUnloadImage(IN PVOID ImagePointer);

XTCDECL
EFI_STATUS
PeVerifyImage(IN PVOID ImagePointer);

XTCDECL
EFI_STATUS
PepRelocateLoadedImage(IN PPECOFF_IMAGE_CONTEXT Image);

XTCDECL
EFI_STATUS
BlXtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                  IN PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_PECOFF_H */
