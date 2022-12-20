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

EFI_STATUS
BlAddVirtualMemoryMapping(IN PLIST_ENTRY MemoryMappings,
                          IN PVOID VirtualAddress,
                          IN PVOID PhysicalAddress,
                          IN UINT NumberOfPages,
                          LOADER_MEMORY_TYPE MemoryType);

EFI_STATUS
BlCloseVolume(IN PEFI_HANDLE VolumeHandle);

EFI_STATUS
BlComPortInitialize();

VOID
BlComPortPutChar(IN USHORT Character);

VOID
BlConsoleClearScreen();

VOID
BlConsoleInitialize();

VOID
BlConsolePutChar(IN USHORT Character);

LOADER_MEMORY_TYPE
BlConvertEfiMemoryType(IN EFI_MEMORY_TYPE EfiMemoryType);

VOID
BlCreateStack(IN PVOID *StackPtr,
              IN ULONG StackSize,
              IN PVOID Callback);

VOID
BlDbgPrint(IN PUINT16 Format,
           IN ...);

INT_PTR
BlEfiGetSecureBootStatus();

EFI_STATUS
BlEfiGetSystemConfigurationTable(IN PEFI_GUID TableGuid,
                                 OUT PVOID *Table);

EFI_STATUS
BlEfiMemoryAllocatePages(IN UINT64 Size,
                         OUT PEFI_PHYSICAL_ADDRESS Memory);

EFI_STATUS
BlEfiMemoryAllocatePool(IN UINT_PTR Size,
                        OUT PVOID *Memory);

EFI_STATUS
BlEfiMemoryFreePages(IN UINT64 Size,
                     IN EFI_PHYSICAL_ADDRESS Memory);

EFI_STATUS
BlEfiMemoryFreePool(IN PVOID Memory);

VOID
BlEfiPrint(IN PUINT16 Format,
           IN ...);

EFI_STATUS
BlEnablePaging(IN PLIST_ENTRY MemoryMappings,
               IN PVOID VirtualAddress,
               IN PEFI_LOADED_IMAGE_PROTOCOL ImageProtocol,
               IN PVOID *PtePointer);

EFI_STATUS
BlEnumerateEfiBlockDevices();

EFI_STATUS
BlFindVolumeDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL FsHandle,
                       IN CONST PWCHAR FileSystemPath,
                       OUT PEFI_DEVICE_PATH_PROTOCOL* DevicePath);

EFI_STATUS
BlGetMemoryMap(OUT PEFI_MEMORY_MAP MemoryMap);

VOID BlGetStackPointer(OUT PVOID *Stack);

EFI_STATUS
BlGetVolumeDevicePath(IN PUCHAR SystemPath,
                      OUT PEFI_DEVICE_PATH_PROTOCOL *DevicePath,
                      OUT PUCHAR *ArcName,
                      OUT PUCHAR *Path);

EFI_STATUS
BlInitializeVirtualMemory(IN OUT PLIST_ENTRY MemoryMappings,
                          IN OUT PVOID *MemoryMapAddress);

EFI_STATUS
BlLoadEfiModules();

EFI_STATUS
BlMapVirtualMemory(IN PLIST_ENTRY MemoryMappings,
                   IN UINT_PTR VirtualAddress,
                   IN UINT_PTR PhysicalAddress,
                   IN UINT NumberOfPages,
                   IN BOOLEAN PaeExtension,
                   IN OUT PVOID *PtePointer);

EFI_STATUS
BlOpenVolume(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
             OUT PEFI_HANDLE DiskHandle,
             OUT PEFI_FILE_HANDLE *FsHandle);

EFI_STATUS
BlRegisterXtLoaderProtocol();

EFI_STATUS
BlStartNewStack();

EFI_STATUS
BlStartXtLoader(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable);

INT
BlStringCompareInsensitive(IN PUCHAR String1,
                           IN PUCHAR String2);

VOID
BlStringPrint(IN VOID PutChar(IN USHORT Character),
              IN PUINT16 Format,
              IN VA_LIST Arguments);

EFI_STATUS
BlpDiscoverEfiBlockDevices(OUT PLIST_ENTRY BlockDevices);

EFI_STATUS
BlpDissectVolumeArcPath(IN PUCHAR SystemPath,
                        OUT PUCHAR *ArcName,
                        OUT PUCHAR *Path,
                        OUT PUSHORT DriveType,
                        OUT PULONG DriveNumber,
                        OUT PULONG PartNumber);

PEFI_DEVICE_PATH_PROTOCOL
BlpDuplicateDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath);

EFI_STATUS
BlpFindLastEfiBlockDeviceNode(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
                              OUT PEFI_DEVICE_PATH_PROTOCOL *LastNode);

BOOLEAN
BlpFindParentEfiBlockDevice(IN PLIST_ENTRY BlockDevices,
                            IN PEFI_BLOCK_DEVICE_DATA ChildNode,
                            OUT PEFI_BLOCK_DEVICE_DATA ParentNode);

VOID
BlpStringFormat(IN VOID PutChar(IN USHORT Character),
                IN PUINT16 Format,
                IN ...);

VOID
BlpStringPrintSigned32(IN VOID PutChar(IN USHORT Character),
                       IN INT32 Number,
                       IN UINT32 Base);

VOID
BlpStringPrintSigned64(IN VOID PutChar(IN USHORT Character),
                       IN INT_PTR Number,
                       IN UINT_PTR Base);

VOID
BlpStringPrintUnsigned32(IN VOID PutChar(IN USHORT Character),
                         IN UINT32 Number,
                         IN UINT32 Base,
                         IN UINT32 Padding);

VOID
BlpStringPrintUnsigned64(IN VOID PutChar(IN USHORT Character),
                         IN UINT_PTR Number,
                         IN UINT_PTR Base,
                         IN UINT_PTR Padding);

UINT64
BlpStringReadPadding(IN PUINT16 *Format);

#endif /* __XTLDR_XTBL_H */
