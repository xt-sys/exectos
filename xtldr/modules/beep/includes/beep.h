/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/beep/includes/beep.h
 * DESCRIPTION:     XTLDR Beep Module header file
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_BEEP_BEEP_H
#define __XTLDR_BEEP_BEEP_H

#include <xtblapi.h>
#include <globals.h>


/* Beep module routines forward references */
XTCDECL
VOID
BpDisableToneBeep();

XTCDECL
VOID
BpEnableToneBeep(IN UINT Pitch);

XTCDECL
VOID
BpPlayTune(IN PWCHAR Arguments);

XTCDECL
UINT
BpWideStringToNumber(IN PWCHAR String);

XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable);

#endif/* __XTLDR_BEEP_BEEP_H */
