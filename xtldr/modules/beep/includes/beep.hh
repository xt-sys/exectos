/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/beep/includes/beep.hh
 * DESCRIPTION:     XTLDR Beep Module header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_BEEP_BEEP_HH
#define __XTLDR_BEEP_BEEP_HH

#include <xtblapi.h>


/* BEEP module for XTLDR */
class Beep
{
    private:
        STATIC PXTBL_LOADER_PROTOCOL XtLdrProtocol;

    public:
        STATIC XTCDECL EFI_STATUS InitializeModule(IN EFI_HANDLE ImageHandle,
                                                   IN PEFI_SYSTEM_TABLE SystemTable);
        STATIC XTCDECL VOID PlayTune(IN PWCHAR Arguments);

    private:
        STATIC XTCDECL VOID DisableToneBeep();
        STATIC XTCDECL VOID EnableToneBeep(IN UINT Pitch);
        STATIC XTCDECL UINT WideStringToNumber(IN PWCHAR String);
};

#endif /* __XTLDR_BEEP_BEEP_HH */
