/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/includes/blmod.h
 * DESCRIPTION:     XTLDR modules support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_BLMOD_H
#define __XTLDR_BLMOD_H

#include <xtkmapi.h>
#include <bldefs.h>


EFI_STATUS
BlGetXtLoaderProtocol(PXT_BOOT_LOADER_PROTOCOL *LdrProtocol);

#endif /* __XTLDR_BLMOD_H */
