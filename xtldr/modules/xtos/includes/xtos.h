/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/xtos/includes/xtos.h
 * DESCRIPTION:     XTOS boot protocol support header
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_XTOS_H
#define __XTLDR_MODULES_XTOS_H

#include <blmod.h>


EFI_STATUS
XtBootSystem(IN PXT_BOOT_PROTOCOL_PARAMETERS Parameters);

EFI_STATUS
XtpBootSequence(IN PEFI_FILE_HANDLE BootDir,
                IN PXT_BOOT_PROTOCOL_PARAMETERS Parameters);

EFI_STATUS
BlXtLdrModuleMain(EFI_HANDLE ImageHandle,
                  PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_MODULES_XTOS_H */
