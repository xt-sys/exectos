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


/* EFI Image Handle */
EXTERN EFI_HANDLE EfiImageHandle;

/* EFI System Table */
EXTERN EFI_SYSTEM_TABLE *EfiSystemTable;

/* Serial port configuration */
EXTERN CPPORT EfiSerialPort;

VOID
BlComPortPutChar(IN USHORT Character);

VOID
BlConsoleClearScreen();

EFI_STATUS
BlConsoleInitialize();

VOID
BlConsolePutChar(IN USHORT Character);

VOID
BlDbgPrint(IN PUINT16 Format,
           IN ...);

VOID
BlEfiPrint(IN PUINT16 Format,
           IN ...);

EFI_STATUS
BlStartXtLoader(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable);

VOID
BlStringPrint(IN VOID PutChar(IN USHORT Character),
              IN PUINT16 Format,
              IN VA_LIST Arguments);

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
