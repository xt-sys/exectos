/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/xtos/includes/xtos.h
 * DESCRIPTION:     XTOS boot protocol support header
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_XTOS_H
#define __XTLDR_MODULES_XTOS_H

#include <blmod.h>


/* XTOS kernel entry point */
typedef VOID (XTAPI *PXT_ENTRY_POINT)(IN PKERNEL_INITIALIZATION_BLOCK BootParameters);

/* XTOS boot protocol related routines forward references */
EFI_STATUS
XtBootSystem(IN PXT_BOOT_PROTOCOL_PARAMETERS Parameters);

EFI_STATUS
XtpBootSequence(IN PEFI_FILE_HANDLE BootDir,
                IN PXT_BOOT_PROTOCOL_PARAMETERS Parameters);

EFI_STATUS
XtpInitializeLoaderBlock(IN PLIST_ENTRY MemoryMappings,
                         IN PVOID *VirtualAddress);

EFI_STATUS
XtpLoadModule(IN PEFI_FILE_HANDLE BootDir,
              IN PWCHAR FileName,
              IN PVOID VirtualAddress,
              IN LOADER_MEMORY_TYPE MemoryType,
              OUT PPECOFF_IMAGE_CONTEXT *ImageContext);

EFI_STATUS
BlXtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                  IN PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_MODULES_XTOS_H */
