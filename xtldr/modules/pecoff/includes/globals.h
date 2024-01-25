/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/pecoff/includes/globals.h
 * DESCRIPTION:     Basic PE/COFF executable file format support global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_PECOFF_GLOBALS_H
#define __XTLDR_PECOFF_GLOBALS_H

#include <xtblapi.h>


/* XTOS PE/COFF Image Protocol */
EXTERN XTBL_EXECUTABLE_IMAGE_PROTOCOL PeCoffProtocol;

/* EFI XT Loader Protocol */
EXTERN PXTBL_LOADER_PROTOCOL XtLdrProtocol;

#endif /* __XTLDR_PECOFF_GLOBALS_H */
