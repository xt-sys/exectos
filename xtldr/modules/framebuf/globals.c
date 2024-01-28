/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/framebuf/globals.c
 * DESCRIPTION:     EFI framebuffer module global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <framebuf.h>


/* Framebuffer display information */
XTBL_FRAMEBUFFER_INFORMATION FbpDisplayInfo;

/* Framebuffer protocol handler */
XTBL_FRAMEBUFFER_PROTOCOL FbpFrameBufferProtocol;

/* XTLDR protocol handler */
PXTBL_LOADER_PROTOCOL XtLdrProtocol;
