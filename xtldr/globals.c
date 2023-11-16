/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/globals.c
 * DESCRIPTION:     XT Boot Loader global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtbl.h>


/* List of available block devices */
LIST_ENTRY EfiBlockDevices;

/* XT Boot Loader hex table */
STATIC PUINT16 EfiHexTable = L"0123456789ABCDEF";

/* EFI Image Handle */
EFI_HANDLE EfiImageHandle;

/* XT Boot Loader protocol */
XT_BOOT_LOADER_PROTOCOL EfiLdrProtocol;

/* EFI System Table */
PEFI_SYSTEM_TABLE EfiSystemTable;

/* EFI Secure Boot status */
INT_PTR EfiSecureBoot;

/* Serial port configuration */
CPPORT EfiSerialPort;
