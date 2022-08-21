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
#include <xtklib.h>


#define XT_BOOT_DEVICE_UNKNOWN               0x00
#define XT_BOOT_DEVICE_CDROM                 0x01
#define XT_BOOT_DEVICE_FLOPPY                0x02
#define XT_BOOT_DEVICE_HARDDISK              0x03
#define XT_BOOT_DEVICE_RAMDISK               0x04

/* EFI Image Handle */
EXTERN EFI_HANDLE EfiImageHandle;

/* EFI System Table */
EXTERN EFI_SYSTEM_TABLE *EfiSystemTable;

/* Serial port configuration */
EXTERN CPPORT EfiSerialPort;

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
BlStartXtLoader(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable);

VOID
BlStringPrint(IN VOID PutChar(IN USHORT Character),
              IN PUINT16 Format,
              IN VA_LIST Arguments);

EFI_STATUS
BlpDiscoverEfiBlockDevices(PLIST_ENTRY BlockDevices);

PEFI_DEVICE_PATH_PROTOCOL
BlpDuplicateDevicePath(PEFI_DEVICE_PATH_PROTOCOL DevicePath);

EFI_STATUS
BlpFindLastEfiBlockDeviceNode(PEFI_DEVICE_PATH_PROTOCOL DevicePath,
                              PEFI_DEVICE_PATH_PROTOCOL *LastNode);

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
