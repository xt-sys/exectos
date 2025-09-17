/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/chainldr/data.cc
 * DESCRIPTION:     CHAINLDR module global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <chainldr.hh>


/* ChainLoader Boot Protocol */
XTBL_BOOT_PROTOCOL ChainLoader::BootProtocol;

/* XTLDR protocol handler */
PXTBL_LOADER_PROTOCOL ChainLoader::XtLdrProtocol;
