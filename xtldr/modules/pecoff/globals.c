/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/pecoff/globals.c
 * DESCRIPTION:     Basic PE/COFF executable file format support global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <pecoff.h>


/* XTOS PE/COFF Image Protocol */
XTBL_EXECUTABLE_IMAGE_PROTOCOL PeCoffProtocol;

/* EFI XT Loader Protocol */
PXTBL_LOADER_PROTOCOL XtLdrProtocol;
