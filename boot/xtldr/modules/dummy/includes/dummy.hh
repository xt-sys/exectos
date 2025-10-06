/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/dummy/includes/dummy.hh
 * DESCRIPTION:     XTLDR Dummy Module header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_DUMMY_DUMMY_HH
#define __XTLDR_DUMMY_DUMMY_HH

#include <xtblapi.h>


/* DUMMY module for XTLDR */
class Dummy
{
    private:
        STATIC XTBL_BOOT_PROTOCOL DummyProtocol;
        STATIC PXTBL_LOADER_PROTOCOL XtLdrProtocol;

    public:
        STATIC EFI_STATUS BootSystem(IN PXTBL_BOOT_PARAMETERS Parameters);
        STATIC EFI_STATUS InitializeModule(IN EFI_HANDLE ImageHandle,
                                           IN PEFI_SYSTEM_TABLE SystemTable);
};

#endif/* __XTLDR_DUMMY_DUMMY_HH */
