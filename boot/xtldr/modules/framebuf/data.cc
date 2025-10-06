/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/framebuf/data.cc
 * DESCRIPTION:     EFI framebuffer module global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <framebuf.hh>


/* Framebuffer display information */
XTBL_FRAMEBUFFER_INFORMATION FrameBuffer::DisplayInfo;

/* Framebuffer protocol handler */
XTBL_FRAMEBUFFER_PROTOCOL FrameBuffer::FbProtocol;

/* XTLDR protocol handler */
PXTBL_LOADER_PROTOCOL FrameBuffer::XtLdrProtocol;
