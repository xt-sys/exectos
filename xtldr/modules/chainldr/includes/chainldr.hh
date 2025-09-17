/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/chainldr/includes/chainldr.hh
 * DESCRIPTION:     XTLDR Chain Loader header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_CHAINLDR_CHAINLDR_HH
#define __XTLDR_CHAINLDR_CHAINLDR_HH

#include <xtblapi.h>


/* CHAINLDR module for XTLDR */
class ChainLoader
{
    private:
        STATIC XTBL_BOOT_PROTOCOL BootProtocol;
        STATIC PXTBL_LOADER_PROTOCOL XtLdrProtocol;

    public:
        STATIC XTCDECL EFI_STATUS BootSystem(IN PXTBL_BOOT_PARAMETERS Parameters);
        STATIC XTCDECL EFI_STATUS InitializeModule(IN EFI_HANDLE ImageHandle,
                                                   IN PEFI_SYSTEM_TABLE SystemTable);
};

#endif /* __XTLDR_CHAINLDR_CHAINLDR_HH */
