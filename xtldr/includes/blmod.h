/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/includes/blmod.h
 * DESCRIPTION:     Top level header for XTLDR modules support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTLDR_BLMOD_H
#define __XTLDR_BLMOD_H

#include <xtkmapi.h>
#include <bldefs.h>
#include <blproto.h>


/* Structures forward declarations */
typedef struct _XT_BOOT_PROTOCOL XT_BOOT_PROTOCOL, *PXT_BOOT_PROTOCOL;
typedef struct _XT_BOOT_PROTOCOL_PARAMETERS XT_BOOT_PROTOCOL_PARAMETERS, *PXT_BOOT_PROTOCOL_PARAMETERS;
typedef struct _XT_PECOFFF_IMAGE_PROTOCOL XT_PECOFF_IMAGE_PROTOCOL, *PXT_PECOFF_IMAGE_PROTOCOL;

/* Pointers to the routines provided by the modules */
typedef EFI_STATUS (*PXT_BOOTPROTO_BOOT_SYSTEM)(IN PXT_BOOT_PROTOCOL_PARAMETERS Parameters);
typedef EFI_STATUS (*PXT_PECOFF_GET_ENTRY_POINT)(IN PPECOFF_IMAGE_CONTEXT Image, OUT PVOID *EntryPoint);
typedef EFI_STATUS (*PXT_PECOFF_PROTOCOL_LOAD)(IN PEFI_FILE_HANDLE FileHandle, IN LOADER_MEMORY_TYPE MemoryType,
                                               IN PVOID VirtualAddress, OUT PPECOFF_IMAGE_CONTEXT *Image);

/* XT common boot protocols */
typedef struct _XT_BOOT_PROTOCOL
{
    PXT_BOOTPROTO_BOOT_SYSTEM BootSystem;
} XT_BOOT_PROTOCOL, *PXT_BOOT_PROTOCOL;

/* XT common boot protocol parameters */
typedef struct _XT_BOOT_PROTOCOL_PARAMETERS
{
    PEFI_DEVICE_PATH_PROTOCOL DevicePath;
    PWCHAR ArcName;
    PWCHAR SystemPath;
    PWCHAR KernelFile;
    PWCHAR InitrdFile;
    PWCHAR HalFile;
    PWCHAR Arguments;
} XT_BOOT_PROTOCOL_PARAMETERS, *PXT_BOOT_PROTOCOL_PARAMETERS;

/* EFI XT PE/COFF Image Protocol */
typedef struct _XT_PECOFFF_IMAGE_PROTOCOL
{
    PXT_PECOFF_GET_ENTRY_POINT GetEntryPoint;
    PXT_PECOFF_PROTOCOL_LOAD Load;
} XT_PECOFF_IMAGE_PROTOCOL, *PXT_PECOFF_IMAGE_PROTOCOL;

#endif /* __XTLDR_BLMOD_H */
