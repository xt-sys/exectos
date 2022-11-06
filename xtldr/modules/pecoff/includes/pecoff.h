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


EFI_STATUS PeLoadImage(IN PEFI_FILE_HANDLE FileHandle,
                       IN PVOID VirtualAddress,
                       OUT PPECOFF_IMAGE_CONTEXT *Image);

EFI_STATUS
PepReadImageHeader(IN PUCHAR ImageData,
                   IN SIZE_T FileSize,
                   OUT PPECOFF_IMAGE_PE_HEADER *PeHeader);

EFI_STATUS
BlXtLdrModuleMain(EFI_HANDLE ImageHandle,
                  PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_MODULES_PECOFF_H */
