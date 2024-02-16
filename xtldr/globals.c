/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/globals.c
 * DESCRIPTION:     XT Boot Loader global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/* XT Boot Loader registered boot protocol list */
LIST_ENTRY BlpBootProtocols;

/* XT Boot Loader configuration list */
LIST_ENTRY BlpConfig;

/* XT Boot Loader loaded configuration */
LIST_ENTRY BlpConfigSections;

/* XT Boot Loader protocol */
XTBL_LOADER_PROTOCOL BlpLdrProtocol;

/* XT Boot Loader loaded modules list */
LIST_ENTRY BlpLoadedModules;

/* XT Boot Loader menu list */
PLIST_ENTRY BlpMenuList = NULL;

/* XT Boot Loader status data */
XTBL_STATUS BlpStatus = {0};

/* List of available block devices */
LIST_ENTRY EfiBlockDevices;

/* EFI Image Handle */
EFI_HANDLE EfiImageHandle;

/* EFI System Table */
PEFI_SYSTEM_TABLE EfiSystemTable;
