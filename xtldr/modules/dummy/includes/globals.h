/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/dummy/includes/globals.h
 * DESCRIPTION:     XTLDR Dummy Module global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_DUMMY_GLOBALS_H
#define __XTLDR_DUMMY_GLOBALS_H

#include <dummy.h>


/* XTLDR protocol handler */
EXTERN PXTBL_LOADER_PROTOCOL XtLdrProtocol;

/* Dummy Boot Protocol handler */
EXTERN XTBL_BOOT_PROTOCOL BlpDummyProtocol;

#endif/* __XTLDR_DUMMY_GLOBALS_H */
