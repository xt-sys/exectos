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


/* EFI Image Handle */
EXTERN EFI_HANDLE EfiImageHandle;

/* EFI System Table */
EXTERN EFI_SYSTEM_TABLE *EfiSystemTable;

EFI_STATUS
XtLoaderStartup(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_XTBL_H */
