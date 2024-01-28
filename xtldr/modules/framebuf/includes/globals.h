/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/framebuf/includes/globals.h
 * DESCRIPTION:     EFI Framebuffer module global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_MODULES_GLOBALS_H
#define __XTLDR_MODULES_GLOBALS_H

#include <xtblapi.h>


/* Framebuffer display information */
EXTERN XTBL_FRAMEBUFFER_INFORMATION FbpDisplayInfo;

/* Framebuffer protocol handler */
EXTERN XTBL_FRAMEBUFFER_PROTOCOL FbpFrameBufferProtocol;

/* XTLDR protocol handler */
EXTERN PXTBL_LOADER_PROTOCOL XtLdrProtocol;

#endif /* __XTLDR_MODULES_GLOBALS_H */
