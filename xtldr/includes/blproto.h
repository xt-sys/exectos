/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/includes/blproto.h
 * DESCRIPTION:     XTLDR boot loader protocol support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_BLPROTO_H
#define __XTLDR_BLPROTO_H

#include <xtkmapi.h>
#include <bldefs.h>


/* Loader protocol routine pointers */
typedef EFI_STATUS (*PBL_ADD_VIRTUAL_MEMORY_MAPPING)(IN PLIST_ENTRY MemoryMappings, IN PVOID VirtualAddress, IN PVOID PhysicalAddress, IN UINT NumberOfPages, LOADER_MEMORY_TYPE MemoryType);
typedef EFI_STATUS (*PBL_ALLOCATE_PAGES)(IN UINT64 Size, OUT PEFI_PHYSICAL_ADDRESS Memory);
typedef EFI_STATUS (*PBL_ALLOCATE_POOL)(IN UINT_PTR Size, OUT PVOID *Memory);
typedef EFI_STATUS (*PBL_ENABLE_PAGING)(IN PLIST_ENTRY MemoryMappings, IN PVOID VirtualAddress, IN PEFI_LOADED_IMAGE_PROTOCOL ImageProtocol, IN PVOID *PtePointer);
typedef EFI_STATUS (*PBL_FREE_PAGES)(IN UINT64 Size, IN EFI_PHYSICAL_ADDRESS Memory);
typedef EFI_STATUS (*PBL_FREE_POOL)(IN PVOID Memory);
typedef EFI_STATUS (*PBL_GET_MEMORY_MAP)(OUT PEFI_MEMORY_MAP MemoryMap);
typedef EFI_STATUS (*PBL_GET_VIRTUAL_ADDRESS)(IN PLIST_ENTRY MemoryMappings, IN PVOID PhysicalAddress, OUT PVOID *VirtualAddress);
typedef EFI_STATUS (*PBL_INIT_VIRTUAL_MEMORY)(IN OUT PLIST_ENTRY MemoryMappings, IN OUT PVOID *MemoryMapAddress);
typedef EFI_STATUS (*PBL_MAP_VIRTUAL_MEMORY)(IN PLIST_ENTRY MemoryMappings, IN UINT_PTR VirtualAddress, IN UINT_PTR PhysicalAddress, IN UINT NumberOfPages, IN BOOLEAN PaeExtension, IN OUT PVOID *PtePointer);
typedef VOID (*PBL_GET_STACK)(OUT PVOID *Stack);
typedef VOID (*PBL_DBG_PRINT)(IN PUINT16 Format, IN ...);
typedef VOID (*PBL_EFI_PRINT)(IN PUINT16 Format, IN ...);
typedef EFI_STATUS (*PBL_CLOSE_VOLUME)(IN PEFI_HANDLE VolumeHandle);
typedef EFI_STATUS (*PBL_OPEN_VOLUME)(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath, OUT PEFI_HANDLE DiskHandle, OUT PEFI_FILE_HANDLE *FsHandle);

/* EFI XT Boot Loader Protocol */
typedef struct _XT_BOOT_LOADER_PROTOCOL
{
    PBL_ADD_VIRTUAL_MEMORY_MAPPING AddVirtualMemoryMapping;
    PBL_ALLOCATE_PAGES AllocatePages;
    PBL_ALLOCATE_POOL AllocatePool;
    PBL_FREE_PAGES FreePages;
    PBL_FREE_POOL FreePool;
    PBL_ENABLE_PAGING EnablePaging;
    PBL_GET_MEMORY_MAP GetMemoryMap;
    PBL_GET_VIRTUAL_ADDRESS GetVirtualAddress;
    PBL_INIT_VIRTUAL_MEMORY InitializeVirtualMemory;
    PBL_MAP_VIRTUAL_MEMORY MapVirtualMemory;
    PBL_GET_STACK GetStack;
    PBL_DBG_PRINT DbgPrint;
    PBL_EFI_PRINT EfiPrint;
    PBL_CLOSE_VOLUME CloseVolume;
    PBL_OPEN_VOLUME OpenVolume;
} XT_BOOT_LOADER_PROTOCOL, *PXT_BOOT_LOADER_PROTOCOL;

/* Loader protocol related routines forward references */
EFI_STATUS
BlGetXtLoaderProtocol(OUT PXT_BOOT_LOADER_PROTOCOL *LdrProtocol);

EFI_STATUS
BlLoadXtProtocol(OUT PVOID *ProtocolHandler,
                 IN PEFI_GUID ProtocolGuid);

#endif /* __XTLDR_BLPROTO_H */
