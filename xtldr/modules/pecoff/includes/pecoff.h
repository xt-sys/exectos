/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/pecoff/includes/pecoff.h
 * DESCRIPTION:     PE/COFF executable file format support header
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_PECOFF_H
#define __XTLDR_MODULES_PECOFF_H

#include <blmod.h>


/* PE/COFF image protocol related routines forward references */
EFI_STATUS
PeGetEntryPoint(IN PPECOFF_IMAGE_CONTEXT Image,
                OUT PVOID *EntryPoint);

EFI_STATUS
PeGetMachineType(IN PPECOFF_IMAGE_CONTEXT Image,
                 OUT PUSHORT MachineType);

EFI_STATUS
PeGetSubSystem(IN PPECOFF_IMAGE_CONTEXT Image,
               OUT PUSHORT SubSystem);

EFI_STATUS
PeLoadImage(IN PEFI_FILE_HANDLE FileHandle,
            IN LOADER_MEMORY_TYPE MemoryType,
            IN PVOID VirtualAddress,
            OUT PPECOFF_IMAGE_CONTEXT *Image);

EFI_STATUS
PeRelocateImage(IN PPECOFF_IMAGE_CONTEXT Image,
                IN EFI_VIRTUAL_ADDRESS Address);

EFI_STATUS
PepRelocateLoadedImage(IN PPECOFF_IMAGE_CONTEXT Image);

EFI_STATUS
PepValidateImageHeaders(IN PPECOFF_IMAGE_DOS_HEADER DosHeader,
                        IN PPECOFF_IMAGE_PE_HEADER PeHeader,
                        IN SIZE_T FileSize);

EFI_STATUS
BlXtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                  IN PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_MODULES_PECOFF_H */
