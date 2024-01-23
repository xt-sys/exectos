/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/chainldr/includes/chainldr.h
 * DESCRIPTION:     XTLDR Chain Loader header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_CHAINLDR_CHAINLDR_H
#define __XTLDR_CHAINLDR_CHAINLDR_H

#include <xtblapi.h>
#include <globals.h>


/* ChainLoader module routines forward references */
XTCDECL
EFI_STATUS
ChBootSystem(IN PXTBL_BOOT_PARAMETERS Parameters);

XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable);

#endif/* __XTLDR_CHAINLDR_CHAINLDR_H */
