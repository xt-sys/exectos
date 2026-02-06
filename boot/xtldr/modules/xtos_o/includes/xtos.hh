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
        STATIC XTCDECL EFI_STATUS AddVirtualMemoryMapping(IN PLIST_ENTRY MemoryMappings,
                                                          IN PVOID VirtualAddress,
                                                          IN PVOID PhysicalAddress,
                                                          IN UINT NumberOfPages,
                                                          IN LOADER_MEMORY_TYPE MemoryType);
        STATIC XTCDECL LOADER_MEMORY_TYPE ConvertEfiMemoryType(IN EFI_MEMORY_TYPE EfiMemoryType);
        STATIC XTCDECL BOOLEAN DetermineMappingStrategy();
        STATIC XTCDECL ULONG DeterminePagingLevel(IN CONST PWCHAR Parameters);
        STATIC XTCDECL EFI_STATUS EnablePaging(IN PXTBL_PAGE_MAPPING PageMap);
        STATIC XTCDECL VOID GetDisplayInformation(OUT PSYSTEM_RESOURCE_FRAMEBUFFER FrameBufferResource,
                                                  IN PEFI_PHYSICAL_ADDRESS FrameBufferBase,
                                                  IN PULONG_PTR FrameBufferSize,
                                                  IN PXTBL_FRAMEBUFFER_MODE_INFORMATION FrameBufferModeInfo);
        STATIC XTCDECL EFI_STATUS GetMemoryDescriptorList(IN PXTBL_PAGE_MAPPING PageMap,
                                                          IN PVOID *VirtualAddress,
                                                          OUT PLIST_ENTRY MemoryDescriptorList);
        STATIC XTCDECL EFI_STATUS GetSystemResourcesList(IN PXTBL_PAGE_MAPPING PageMap,
                                                         IN PVOID *VirtualAddress,
                                                         OUT PLIST_ENTRY SystemResourcesList);
        STATIC XTCDECL EFI_STATUS GetVirtualAddress(IN PLIST_ENTRY MemoryMappings,
                                                    IN PVOID PhysicalAddress,
                                                    OUT PVOID *VirtualAddress);
        STATIC XTCDECL EFI_STATUS InitializeApicBase(IN PXTBL_PAGE_MAPPING PageMap);
        STATIC XTCDECL EFI_STATUS InitializeLoaderBlock(IN PXTBL_PAGE_MAPPING PageMap,
                                                        IN PVOID *VirtualAddress,
                                                        IN PXTBL_BOOT_PARAMETERS Parameters);
        STATIC XTCDECL EFI_STATUS InitializeVirtualMemory(IN OUT PLIST_ENTRY MemoryMappings,
                                                          IN OUT PVOID *MemoryMapAddress);
        STATIC XTCDECL EFI_STATUS LoadModule(IN PEFI_FILE_HANDLE BootDir,
                                             IN PWCHAR FileName,
                                             IN PVOID VirtualAddress,
                                             IN LOADER_MEMORY_TYPE MemoryType,
                                             OUT PPECOFF_IMAGE_CONTEXT *ImageContext);
        STATIC XTCDECL EFI_STATUS MapHardwareMemoryPool(IN PXTBL_PAGE_MAPPING PageMap);
        STATIC XTCDECL EFI_STATUS MapVirtualMemory(IN PLIST_ENTRY MemoryMappings,
                                                   IN UINT_PTR VirtualAddress,
                                                   IN UINT_PTR PhysicalAddress,
                                                   IN UINT NumberOfPages,
                                                   IN OUT PVOID *PtePointer);
        STATIC XTCDECL EFI_STATUS RunBootSequence(IN PEFI_FILE_HANDLE BootDir,
                                                  IN PXTBL_BOOT_PARAMETERS Parameters);
};

#endif /* __XTLDR_MODULES_XTOS_HH */
