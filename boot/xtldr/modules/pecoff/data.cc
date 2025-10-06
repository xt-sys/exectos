/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/pecoff/globals.cc
 * DESCRIPTION:     Basic PE/COFF executable file format global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <pecoff.hh>


/* XTOS PE/COFF Image Protocol */
XTBL_EXECUTABLE_IMAGE_PROTOCOL PeCoff::PeProtocol;

/* EFI XT Loader Protocol */
PXTBL_LOADER_PROTOCOL PeCoff::XtLdrProtocol;
