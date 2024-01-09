/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/xtos/includes/xtos.h
 * DESCRIPTION:     XTOS boot protocol support header
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_XTOS_H
#define __XTLDR_MODULES_XTOS_H

#include <xtblapi.h>


typedef VOID (*PXT_FRAMEBUFFER_GET_DISPLAY_DRIVER)(OUT PWCHAR DriverName);
typedef VOID (*PXT_FRAMEBUFFER_GET_DISPLAY_INFORMATION)(OUT PLOADER_GRAPHICS_INFORMATION_BLOCK InformationBlock);
typedef EFI_STATUS (*PXT_FRAMEBUFFER_INITIALIZE)();
typedef VOID (*PXT_FRAMEBUFFER_PRINT_DISPLAY_INFORMATION)();

/* XT framebuffer support protocol */
typedef struct _XT_FRAMEBUFFER_PROTOCOL
{
    PXT_FRAMEBUFFER_GET_DISPLAY_DRIVER GetDisplayDriver;
    PXT_FRAMEBUFFER_GET_DISPLAY_INFORMATION GetDisplayInformation;
    PXT_FRAMEBUFFER_INITIALIZE Initialize;
    PXT_FRAMEBUFFER_PRINT_DISPLAY_INFORMATION PrintDisplayInformation;
} XT_FRAMEBUFFER_PROTOCOL, *PXT_FRAMEBUFFER_PROTOCOL;

/* EFI XT Loader Protocol */
EXTERN PXTBL_LOADER_PROTOCOL XtLdrProtocol;

/* XTOS kernel entry point */
typedef VOID (XTAPI *PXT_ENTRY_POINT)(IN PKERNEL_INITIALIZATION_BLOCK BootParameters);

/* XTOS boot protocol related routines forward references */
XTCDECL
EFI_STATUS
XtAddVirtualMemoryMapping(IN PLIST_ENTRY MemoryMappings,
                          IN PVOID VirtualAddress,
                          IN PVOID PhysicalAddress,
                          IN UINT NumberOfPages,
                          IN LOADER_MEMORY_TYPE MemoryType);

XTCDECL
EFI_STATUS
XtBootSystem(IN PXTBL_BOOT_PARAMETERS Parameters);

XTCDECL
LOADER_MEMORY_TYPE
XtConvertEfiMemoryType(IN EFI_MEMORY_TYPE EfiMemoryType);

XTCDECL
EFI_STATUS
XtEnablePaging(IN PLIST_ENTRY MemoryMappings,
               IN PVOID VirtualAddress,
               IN PEFI_LOADED_IMAGE_PROTOCOL ImageProtocol,
               IN PVOID *PtePointer);

XTCDECL
EFI_STATUS
XtGetVirtualAddress(IN PLIST_ENTRY MemoryMappings,
                    IN PVOID PhysicalAddress,
                    OUT PVOID *VirtualAddress);

XTCDECL
EFI_STATUS
XtInitializeVirtualMemory(IN OUT PLIST_ENTRY MemoryMappings,
                          IN OUT PVOID *MemoryMapAddress);

XTCDECL
EFI_STATUS
XtMapVirtualMemory(IN PLIST_ENTRY MemoryMappings,
                   IN UINT_PTR VirtualAddress,
                   IN UINT_PTR PhysicalAddress,
                   IN UINT NumberOfPages,
                   IN OUT PVOID *PtePointer);

XTCDECL
EFI_STATUS
XtpBootSequence(IN PEFI_FILE_HANDLE BootDir,
                IN PXTBL_BOOT_PARAMETERS Parameters);


XTCDECL
EFI_STATUS
XtpInitializeApicBase(IN PLIST_ENTRY MemoryMappings);

XTCDECL
EFI_STATUS
XtpInitializeLoaderBlock(IN PLIST_ENTRY MemoryMappings,
                         IN PVOID *VirtualAddress,
                         IN PXTBL_BOOT_PARAMETERS Parameters);

XTCDECL
EFI_STATUS
XtpLoadModule(IN PEFI_FILE_HANDLE BootDir,
              IN PWCHAR FileName,
              IN PVOID VirtualAddress,
              IN LOADER_MEMORY_TYPE MemoryType,
              OUT PPECOFF_IMAGE_CONTEXT *ImageContext);

XTCDECL
EFI_STATUS
BlXtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                  IN PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_MODULES_XTOS_H */
