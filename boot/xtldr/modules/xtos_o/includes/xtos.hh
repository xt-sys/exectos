/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/xtos/includes/xtos.hh
 * DESCRIPTION:     XTOS boot protocol support header
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_XTOS_HH
#define __XTLDR_MODULES_XTOS_HH

#include <xtblapi.h>


/* XTOS kernel entry point */
typedef VOID (XTAPI *PXT_ENTRY_POINT)(IN PKERNEL_INITIALIZATION_BLOCK BootParameters);

/* XTOS trampoline entry point */
typedef VOID (*PXT_TRAMPOLINE_ENTRY)(UINT64 PageMap);


/* XTOS module for XTLDR */
class Xtos
{
    private:
        STATIC XTBL_BOOT_PROTOCOL BootProtocol;
        STATIC PXTBL_EXECUTABLE_IMAGE_PROTOCOL PeCoffProtocol;
        STATIC PXTBL_LOADER_PROTOCOL XtLdrProtocol;

    public:
        STATIC XTCDECL EFI_STATUS BootSystem(IN PXTBL_BOOT_PARAMETERS Parameters);
        STATIC XTCDECL EFI_STATUS InitializeModule(IN EFI_HANDLE ImageHandle,
                                                   IN PEFI_SYSTEM_TABLE SystemTable);

    private:
        STATIC XTCDECL ULONG DeterminePagingLevel(IN CONST PWCHAR Parameters);
        STATIC XTCDECL EFI_STATUS EnablePaging(IN PXTBL_PAGE_MAPPING PageMap);
        STATIC XTCDECL VOID GetDisplayInformation(OUT PSYSTEM_RESOURCE_FRAMEBUFFER FrameBufferResource,
                                                  IN PEFI_PHYSICAL_ADDRESS FrameBufferBase,
                                                  IN PULONG_PTR FrameBufferSize,
                                                  IN PXTBL_FRAMEBUFFER_MODE_INFORMATION FrameBufferModeInfo);
        STATIC XTCDECL EFI_STATUS GetMemoryDescriptorList(IN PXTBL_PAGE_MAPPING PageMap,
                                                          OUT PLIST_ENTRY MemoryDescriptorList);
        STATIC XTCDECL EFI_STATUS GetSystemResourcesList(IN PXTBL_PAGE_MAPPING PageMap,
                                                         OUT PLIST_ENTRY SystemResourcesList);
        STATIC XTCDECL EFI_STATUS InitializeApicBase(IN PXTBL_PAGE_MAPPING PageMap);
        STATIC XTCDECL EFI_STATUS InitializeLoaderBlock(IN PXTBL_PAGE_MAPPING PageMap,
                                                        OUT PKERNEL_INITIALIZATION_BLOCK *KernelParameters,
                                                        IN PXTBL_BOOT_PARAMETERS Parameters);
        STATIC XTCDECL EFI_STATUS LoadModule(IN PEFI_FILE_HANDLE BootDir,
                                             IN PWCHAR FileName,
                                             IN LOADER_MEMORY_TYPE MemoryType,
                                             IN OUT PXTBL_PAGE_MAPPING PageMap,
                                             OUT PPECOFF_IMAGE_CONTEXT *ImageContext);
        STATIC XTCDECL EFI_STATUS MapHardwareMemoryPool(IN PXTBL_PAGE_MAPPING PageMap);
        STATIC XTCDECL EFI_STATUS MapMemory(IN OUT PXTBL_PAGE_MAPPING PageMap,
                                            IN ULONGLONG Address,
                                            IN ULONG NumberOfPages,
                                            IN LOADER_MEMORY_TYPE MemoryType,
                                            IN BOOLEAN KernelMapping);
        STATIC XTCDECL PVOID PhysicalAddressToVirtual(PVOID PhysicalAddress);
        STATIC XTCDECL EFI_STATUS PhysicalListToVirtual(IN OUT PLIST_ENTRY ListHead);
        STATIC XTCDECL EFI_STATUS RunBootSequence(IN PEFI_FILE_HANDLE BootDir,
                                                  IN PXTBL_BOOT_PARAMETERS Parameters);
};

#endif /* __XTLDR_MODULES_XTOS_HH */
