/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/xtos/data.cc
 * DESCRIPTION:     XTOS module global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* XTOS Boot Protocol */
XTBL_BOOT_PROTOCOL Xtos::BootProtocol;

/* XTOS PE/COFF Image Protocol */
PXTBL_EXECUTABLE_IMAGE_PROTOCOL Xtos::PeCoffProtocol;

/* EFI XT Loader Protocol */
PXTBL_LOADER_PROTOCOL Xtos::XtLdrProtocol;
