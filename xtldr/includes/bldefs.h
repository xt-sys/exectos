/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/includes/bldefs.h
 * DESCRIPTION:     Definitions for the XT boot loader
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_BLDEFS_H
#define __XTLDR_BLDEFS_H

#include <xtkmapi.h>


/* EFI XT boot devices */
#define XT_BOOT_DEVICE_UNKNOWN               0x00
#define XT_BOOT_DEVICE_CDROM                 0x01
#define XT_BOOT_DEVICE_FLOPPY                0x02
#define XT_BOOT_DEVICE_HARDDISK              0x03
#define XT_BOOT_DEVICE_RAMDISK               0x04

#endif /* __XTLDR_BLDEFS_H */
