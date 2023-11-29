/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtguid.h
 * DESCRIPTION:     XTOS Globally Unique Identifiers
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTGUID_H
#define __XTDK_XTGUID_H


/* EFI XT protocols GUIDs */
#define XT_BOOT_LOADER_PROTOCOL_GUID         {0x58544F53, 0x5854, 0x4357, {0x00, 0x42, 0x4F, 0x4F, 0x54, 0x4C, 0x50, 0x54}}
#define XT_ELF_IMAGE_PROTOCOL_GUID           {0x58544F53, 0x5854, 0x4357, {0x45, 0x4C, 0x46, 0x49, 0x4D, 0x47, 0x50, 0x54}}
#define XT_FRAMEBUFFER_PROTOCOL_GUID         {0x58544F53, 0x5854, 0x4357, {0x00, 0x00, 0x46, 0x42, 0x55, 0x46, 0x50, 0x54}}
#define XT_HIVE_IMAGE_PROTOCOL_GUID          {0x58544F53, 0x5854, 0x4357, {0x48, 0x49, 0x56, 0x49, 0x4D, 0x47, 0x50, 0x54}}
#define XT_PECOFF_IMAGE_PROTOCOL_GUID        {0x58544F53, 0x5854, 0x4357, {0x00, 0x50, 0x45, 0x49, 0x4D, 0x47, 0x50, 0x54}}

/* EFI XT boot protocols GUIDs */
#define XT_CHAIN_BOOT_PROTOCOL_GUID          {0x58544F53, 0x5854, 0x4357, {0x00, 0x43, 0x48, 0x41, 0x49, 0x4E, 0x50, 0x54}}
#define XT_LINUX_BOOT_PROTOCOL_GUID          {0x58544F53, 0x5854, 0x4357, {0x00, 0x4C, 0x49, 0x4E, 0x55, 0x58, 0x50, 0x54}}
#define XT_WINNT_BOOT_PROTOCOL_GUID          {0x58544F53, 0x5854, 0x4357, {0x00, 0x57, 0x49, 0x4E, 0x4E, 0x54, 0x50, 0x54}}
#define XT_XTOS_BOOT_PROTOCOL_GUID           {0x58544F53, 0x5854, 0x4357, {0x00, 0x00, 0x58, 0x54, 0x4F, 0x53, 0x50, 0x54}}

/* XTOS GUIDs */
#define XTOS_EXECTOS_GUID                    {0x58544F53, 0x5854, 0x4357, {0x00, 0x45, 0x58, 0x45, 0x43, 0x54, 0x4F, 0x53}}
#define XTOS_XTLDR_LOADER_GUID               {0x58544F53, 0x5854, 0x4357, {0x00, 0x00, 0x00, 0x58, 0x54, 0x4C, 0x44, 0x52}}

#endif /* __XTDK_XTGUID_H */