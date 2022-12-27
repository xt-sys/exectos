/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/dummy/includes/dummy.h
 * DESCRIPTION:     Dummy module header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_DUMMY_H
#define __XTLDR_MODULES_DUMMY_H

#include <blmod.h>


/* Dummy module related routines forward references */
XTCDECL
EFI_STATUS
BlXtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                  IN PEFI_SYSTEM_TABLE SystemTable);

#endif /* __XTLDR_MODULES_DUMMY_H */
