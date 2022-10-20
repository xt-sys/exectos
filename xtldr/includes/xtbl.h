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
#include <bldefs.h>
#include <blproto.h>


/* EFI Image Handle */
EXTERN EFI_HANDLE EfiImageHandle;

/* XT Boot Loader protocol */
EXTERN XT_BOOT_LOADER_PROTOCOL EfiLdrProtocol;

/* EFI System Table */
EXTERN PEFI_SYSTEM_TABLE EfiSystemTable;

/* EFI Secure Boot status */
EXTERN INT_PTR EfiSecureBoot;

/* Serial port configuration */
EXTERN CPPORT EfiSerialPort;

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

VOID
BlDbgPrint(IN PUINT16 Format,
           IN ...);

INT_PTR
BlEfiGetSecureBootStatus();

EFI_STATUS
BlEfiGetSystemConfigurationTable(IN PEFI_GUID TableGuid,
                                 OUT PVOID *Table);

EFI_STATUS
BlEfiMemoryAllocatePool(IN UINT_PTR Size,
                        OUT PVOID *Memory);

EFI_STATUS
BlEfiMemoryFreePool(IN PVOID Memory);

VOID
BlEfiPrint(IN PUINT16 Format,
           IN ...);

EFI_STATUS
BlEnumerateEfiBlockDevices();

EFI_STATUS
BlFindVolumeDevicePath(IN PEFI_DEVICE_PATH_PROTOCOL FsHandle,
                       IN CONST PWCHAR FileSystemPath,
                       OUT PEFI_DEVICE_PATH_PROTOCOL* DevicePath);

EFI_STATUS
BlGetVolumeDevicePath(IN PUCHAR SystemPath,
                      OUT PEFI_DEVICE_PATH_PROTOCOL *DevicePath,
                      OUT PUCHAR *Path);

EFI_STATUS
BlLoadEfiModules();

EFI_STATUS
BlOpenVolume(IN PEFI_DEVICE_PATH_PROTOCOL DevicePath,
             OUT PEFI_HANDLE DiskHandle,
             OUT PEFI_FILE_HANDLE *FsHandle);

EFI_STATUS
BlRegisterXtLoaderProtocol();

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
