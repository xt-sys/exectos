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

/* EFI XT protocols GUIDs */
#define XT_BOOT_LOADER_PROTOCOL_GUID         {0x21FF49E8, 0x5854, 0x424C, {0xE9, 0x11, 0x00, 0x86, 0xE1, 0x2C, 0xD9, 0xC1}}
#define XT_ELF_IMAGE_PROTOCOL_GUID           {0x9B539887, 0x5854, 0x424C, {0x42, 0xFA, 0x00, 0x90, 0xC9, 0x04, 0x66, 0xA4}}
#define XT_HIVE_IMAGE_PROTOCOL_GUID          {0x5A3688A2, 0x5854, 0x424C, {0xBC, 0xDF, 0x00, 0x8F, 0xAA, 0x52, 0x63, 0xD4}}
#define XT_PECOFF_IMAGE_PROTOCOL_GUID        {0x85DAFF92, 0x5854, 0x424C, {0xB1, 0xBF, 0x00, 0x79, 0x39, 0xEA, 0x6E, 0xFD}}

/* EFI XT routines forward references */
typedef VOID (*PBL_DBG_PRINT)(IN PUINT16 Format, IN ...);
typedef VOID (*PBL_EFI_PRINT)(IN PUINT16 Format, IN ...);

/* EFI XT Boot Loader Protocol */
typedef struct _XT_BOOT_LOADER_PROTOCOL
{
    PBL_DBG_PRINT DbgPrint;
    PBL_EFI_PRINT EfiPrint;
} XT_BOOT_LOADER_PROTOCOL, *PXT_BOOT_LOADER_PROTOCOL;

#endif /* __XTLDR_BLDEFS_H */
