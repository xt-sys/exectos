/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/dummy/data.cc
 * DESCRIPTION:     Dummy XTLDR module global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <dummy.hh>


/* Dummy Boot Protocol handler */
XTBL_BOOT_PROTOCOL Dummy::DummyProtocol;

/* XTLDR protocol handler */
PXTBL_LOADER_PROTOCOL Dummy::XtLdrProtocol;
