/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/hltypes.h
 * DESCRIPTION:     XT hardware abstraction layer structures definitions specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_HLTYPES_H
#define __XTDK_AMD64_HLTYPES_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>


/* Serial port I/O addresses */
#define COMPORT_ADDRESSES                               {0x000, 0x3F8, 0x2F8, 0x3E8, 0x2E8, 0x5F8, 0x4F8, 0x5E8, 0x4E8}

#endif /* __XTDK_AMD64_HLTYPES_H */
