/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/includes/globals.h
 * DESCRIPTION:     XTLDR global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_GLOBALS_H
#define __XTLDR_GLOBALS_H

#include <xtblapi.h>


/* XT Boot Loader registered boot protocol list */
EXTERN LIST_ENTRY BlpBootProtocols;

/* XT Boot Loader serial ports list */
EXTERN ULONG BlComPortList[COMPORT_COUNT];

/* XT Boot Loader configuration list */
EXTERN LIST_ENTRY BlpConfig;

/* XT Boot Loader loaded configuration */
EXTERN LIST_ENTRY BlpConfigSections;

/* List of user-editable boot options */
EXTERN PCWSTR BlpEditableConfigOptions[];

/* XT Boot Loader protocol */
EXTERN XTBL_LOADER_PROTOCOL BlpLdrProtocol;

/* XT Boot Loader loaded modules list */
EXTERN LIST_ENTRY BlpLoadedModules;

/* XT Boot Loader menu list */
EXTERN PLIST_ENTRY BlpMenuList;

/* XT Boot Loader status data */
EXTERN XTBL_STATUS BlpStatus;

/* List of available block devices */
EXTERN LIST_ENTRY EfiBlockDevices;

/* EFI Image Handle */
EXTERN EFI_HANDLE EfiImageHandle;

/* EFI System Table */
EXTERN PEFI_SYSTEM_TABLE EfiSystemTable;

#endif /* __XTLDR_GLOBALS_H */
