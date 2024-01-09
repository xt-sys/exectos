/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/pecoff_o/includes/pecoff.h
 * DESCRIPTION:     PE/COFF executable file format support header
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_PECOFF_O_H
#define __XTLDR_MODULES_PECOFF_O_H

#include <xtblapi.h>


/* PE/COFF image protocol related routines forward references */
XTCDECL
EFI_STATUS
PeGetEntryPoint(IN PVOID ImagePointer,
                OUT PVOID *EntryPoint);

XTCDECL
EFI_STATUS
PeGetMachineType(IN PVOID ImagePointer,
                 OUT PUSHORT MachineType);

XTCDECL
EFI_STATUS
PeGetSubSystem(IN PVOID ImagePointer,
               OUT PUSHORT SubSystem);

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
PepRelocateLoadedImage(IN PPECOFF_IMAGE_CONTEXT Image);

XTCDECL
EFI_STATUS
PepValidateImageHeaders(IN PPECOFF_IMAGE_DOS_HEADER DosHeader,
                        IN PPECOFF_IMAGE_PE_HEADER PeHeader,
                        IN SIZE_T FileSize);

XTCDECL
EFI_STATUS
BlXtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                  IN PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_MODULES_PECOFF_O_H */
