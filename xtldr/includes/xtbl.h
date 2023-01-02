/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/includes/xtbl.h
 * DESCRIPTION:     Top level header for XTLDR
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_XTBL_H
#define __XTLDR_XTBL_H

#include <xtkmapi.h>
#include <blmod.h>
#include <xtver.h>


/* EFI Image Handle */
EXTERN EFI_HANDLE EfiImageHandle;

/* XT Boot Loader protocol */
EXTERN XT_BOOT_LOADER_PROTOCOL EfiLdrProtocol;

/* EFI System Table */
EXTERN PEFI_SYSTEM_TABLE EfiSystemTable;

/* EFI Secure Boot status */
EXTERN INT_PTR EfiSecureBoot;

/* New bootloader stack */
EXTERN PVOID EfiLoaderStack;

/* Serial port configuration */
EXTERN CPPORT EfiSerialPort;

XTCDECL
EFI_STATUS
BlActivateSerialControllerIO();

XTCDECL
EFI_STATUS
BlAddVirtualMemoryMapping(IN PLIST_ENTRY MemoryMappings,
                          IN PVOID VirtualAddress,
                          IN PVOID PhysicalAddress,
                          IN UINT NumberOfPages,
                          IN LOADER_MEMORY_TYPE MemoryType);

XTCDECL
EFI_STATUS
BlCloseVolume(IN PEFI_HANDLE VolumeHandle);

XTCDECL
EFI_STATUS
BlComPortInitialize();

XTCDECL
VOID
BlComPortPutChar(IN USHORT Character);

XTCDECL
VOID
BlConsoleClearScreen();

XTCDECL
VOID
BlConsoleInitialize();

XTCDECL
VOID
BlConsolePutChar(IN USHORT Character);

XTCDECL
LOADER_MEMORY_TYPE
BlConvertEfiMemoryType(IN EFI_MEMORY_TYPE EfiMemoryType);

XTCDECL
VOID
BlCreateStack(IN PVOID *StackPtr,
              IN ULONG StackSize,
              IN PVOID Callback);

XTCDECL
VOID
BlDbgPrint(IN PUINT16 Format,
           IN ...);

XTCDECL
INT_PTR
BlEfiGetSecureBootStatus();

XTCDECL
EFI_STATUS
BlEfiGetSystemConfigurationTable(IN PEFI_GUID TableGuid,
                                 OUT PVOID *Table);

XTCDECL
EFI_STATUS
BlEfiMemoryAllocatePages(IN UINT64 Size,
                         OUT PEFI_PHYSICAL_ADDRESS Memory);

XTCDECL
EFI_STATUS
BlEfiMemoryAllocatePool(IN UINT_PTR Size,
                        OUT PVOID *Memory);

XTCDECL
EFI_STATUS
BlEfiMemoryFreePages(IN UINT64 Size,
                     IN EFI_PHYSICAL_ADDRESS Memory);

XTCDECL
EFI_STATUS
BlEfiMemoryFreePool(IN PVOID Memory);

XTCDECL
VOID
BlEfiPrint(IN PUINT16 Format,
           IN ...);

XTCDECL
EFI_STATUS
BlEnablePaging(IN PLIST_ENTRY MemoryMappings,
               IN PVOID VirtualAddress,
               IN PEFI_LOADED_IMAGE_PROTOCOL ImageProtocol,
               IN PVOID *PtePointer);

XTCDECL
EFI_STATUS
BlEnumerateEfiBlockDevices();

XTCDECL
EFI_STATUS
BlFindVolumeDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL FsHandle,
                       IN CONST PWCHAR FileSystemPath,
                       OUT PEFI_DEVICE_PATH_PROTOCOL* DevicePath);

XTCDECL
EFI_STATUS
BlGetMemoryMap(OUT PEFI_MEMORY_MAP MemoryMap);

XTCDECL
VOID
BlGetStackPointer(OUT PVOID *Stack);

XTCDECL
EFI_STATUS
BlGetVirtualAddress(IN PLIST_ENTRY MemoryMappings,
                    IN PVOID PhysicalAddress,
                    OUT PVOID *VirtualAddress);

XTCDECL
EFI_STATUS
BlGetVolumeDevicePath(IN PUCHAR SystemPath,
                      OUT PEFI_DEVICE_PATH_PROTOCOL *DevicePath,
                      OUT PUCHAR *ArcName,
                      OUT PUCHAR *Path);

XTCDECL
EFI_STATUS
BlInitializeVirtualMemory(IN OUT PLIST_ENTRY MemoryMappings,
                          IN OUT PVOID *MemoryMapAddress);

XTCDECL
EFI_STATUS
BlLoadEfiModules();

XTCDECL
EFI_STATUS
BlLoadXtSystem();

XTCDECL
EFI_STATUS
BlMapVirtualMemory(IN PLIST_ENTRY MemoryMappings,
                   IN UINT_PTR VirtualAddress,
                   IN UINT_PTR PhysicalAddress,
                   IN UINT NumberOfPages,
                   IN OUT PVOID *PtePointer);

XTCDECL
EFI_STATUS
BlOpenVolume(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
             OUT PEFI_HANDLE DiskHandle,
             OUT PEFI_FILE_HANDLE *FsHandle);

XTCDECL
EFI_STATUS
BlRegisterXtLoaderProtocol();

XTCDECL
EFI_STATUS
BlStartNewStack();

XTCDECL
EFI_STATUS
BlStartXtLoader(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable);

XTCDECL
INT
BlStringCompareInsensitive(IN PUCHAR String1,
                           IN PUCHAR String2);

XTCDECL
VOID
BlStringPrint(IN VOID PutChar(IN USHORT Character),
              IN PUINT16 Format,
              IN VA_LIST Arguments);

XTCDECL
EFI_STATUS
BlpDiscoverEfiBlockDevices(OUT PLIST_ENTRY BlockDevices);

XTCDECL
EFI_STATUS
BlpDissectVolumeArcPath(IN PUCHAR SystemPath,
                        OUT PUCHAR *ArcName,
                        OUT PUCHAR *Path,
                        OUT PUSHORT DriveType,
                        OUT PULONG DriveNumber,
                        OUT PULONG PartNumber);

XTCDECL
PEFI_DEVICE_PATH_PROTOCOL
BlpDuplicateDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath);

XTCDECL
EFI_STATUS
BlpFindLastEfiBlockDeviceNode(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
                              OUT PEFI_DEVICE_PATH_PROTOCOL *LastNode);

XTCDECL
BOOLEAN
BlpFindParentEfiBlockDevice(IN PLIST_ENTRY BlockDevices,
                            IN PEFI_BLOCK_DEVICE_DATA ChildNode,
                            OUT PEFI_BLOCK_DEVICE_DATA ParentNode);

XTCDECL
VOID
BlpStringFormat(IN VOID PutChar(IN USHORT Character),
                IN PUINT16 Format,
                IN ...);

XTCDECL
VOID
BlpStringPrintSigned32(IN VOID PutChar(IN USHORT Character),
                       IN INT32 Number,
                       IN UINT32 Base);

XTCDECL
VOID
BlpStringPrintSigned64(IN VOID PutChar(IN USHORT Character),
                       IN INT_PTR Number,
                       IN UINT_PTR Base);

XTCDECL
VOID
BlpStringPrintUnsigned32(IN VOID PutChar(IN USHORT Character),
                         IN UINT32 Number,
                         IN UINT32 Base,
                         IN UINT32 Padding);

XTCDECL
VOID
BlpStringPrintUnsigned64(IN VOID PutChar(IN USHORT Character),
                         IN UINT_PTR Number,
                         IN UINT_PTR Base,
                         IN UINT_PTR Padding);

XTCDECL
UINT64
BlpStringReadPadding(IN PUINT16 *Format);

#endif /* __XTLDR_XTBL_H */
