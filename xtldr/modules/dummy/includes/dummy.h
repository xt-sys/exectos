/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/dummy/includes/dummy.h
 * DESCRIPTION:     XTLDR Dummy Module header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_DUMMY_DUMMY_H
#define __XTLDR_DUMMY_DUMMY_H

#include <xtblapi.h>
#include <globals.h>


/* Dummy module routines forward references */
XTCDECL
EFI_STATUS
BlBootDummyOS(IN PXTBL_BOOT_PARAMETERS Parameters);

XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable);

#endif/* __XTLDR_DUMMY_DUMMY_H */
