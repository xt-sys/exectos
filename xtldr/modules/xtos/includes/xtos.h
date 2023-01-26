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


/* EFI XT Loader Protocol */
EXTERN PXT_BOOT_LOADER_PROTOCOL XtLdrProtocol;

/* XTOS kernel entry point */
typedef VOID (XTAPI *PXT_ENTRY_POINT)(IN PKERNEL_INITIALIZATION_BLOCK BootParameters);

/* XTOS boot protocol related routines forward references */
XTCDECL
EFI_STATUS
XtBootSystem(IN PXT_BOOT_PROTOCOL_PARAMETERS Parameters);

XTCDECL
EFI_STATUS
XtpBootSequence(IN PEFI_FILE_HANDLE BootDir,
                IN PXT_BOOT_PROTOCOL_PARAMETERS Parameters);

XTCDECL
VOID
XtpInitializeGdtEntry(IN PKGDTENTRY Gdt,
                      IN USHORT Selector,
                      IN ULONGLONG Base,
                      IN ULONG Limit,
                      IN UCHAR Type,
                      IN UCHAR Dpl,
                      IN UCHAR SegmentMode);

XTCDECL
EFI_STATUS
XtpInitializeDescriptors(IN PLIST_ENTRY MemoryMappings,
                         IN PVOID *VirtualAddress,
                         OUT PKGDTENTRY *Gdt,
                         OUT PKIDTENTRY *Idt);

XTCDECL
EFI_STATUS
XtpInitializeLoaderBlock(IN PLIST_ENTRY MemoryMappings,
                         IN PVOID *VirtualAddress);

XTCDECL
EFI_STATUS
XtpLoadModule(IN PEFI_FILE_HANDLE BootDir,
              IN PWCHAR FileName,
              IN PVOID VirtualAddress,
              IN LOADER_MEMORY_TYPE MemoryType,
              OUT PPECOFF_IMAGE_CONTEXT *ImageContext);

XTCDECL
VOID
XtpLoadProcessorContext(IN PKGDTENTRY Gdt,
                        IN PKIDTENTRY Idt);

XTCDECL
EFI_STATUS
XtpSetProcessorContext(IN PLIST_ENTRY MemoryMappings,
                       IN PVOID *VirtualAddress,
                       OUT PKGDTENTRY *Gdt,
                       OUT PKIDTENTRY *Idt);

XTCDECL
EFI_STATUS
BlXtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                  IN PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_MODULES_XTOS_H */
