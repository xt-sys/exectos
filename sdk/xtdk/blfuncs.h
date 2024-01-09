/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/blfuncs.h
 * DESCRIPTION:     XT Boot Manager routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_BLFUNCS_H
#define __XTDK_BLFUNCS_H

#include <xttypes.h>
#include <xtuefi.h>


/* XT BootLoader routines forward references */
XTCDECL
EFI_STATUS
BlGetXtLdrProtocol(IN PEFI_SYSTEM_TABLE SystemTable,
                   IN EFI_HANDLE ImageHandle,
                   OUT PXTBL_LOADER_PROTOCOL *ProtocolHandler);

#endif /* __XTDK_BLFUNCS_H */
