/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/bltarget.h
 * DESCRIPTION:     XT Boot Loader target architecture specific definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_BLTARGET_H
#define __XTDK_BLTARGET_H

#include <xttypes.h>


/* Boot Loader module information macros */
#define MODULE_AUTHOR(_author)                                      XTBL_MODULE_INFO(author, author, _author)
#define MODULE_DEPENDENCY(_softdeps)                                XTBL_MODULE_INFO(softdeps, softdeps, _softdeps)
#define MODULE_DESCRIPTION(_description)                            XTBL_MODULE_INFO(description, description, _description)
#define MODULE_LICENSE(_license)                                    XTBL_MODULE_INFO(license, license, _license)
#define MODULE_VERSION(_version)                                    XTBL_MODULE_INFO(version, version, _version)

/* Boot Loader module information segment macro */
#define XTBL_MODULE_INFO(Tag, Name, Data)                           STATIC CONST WCHAR UNIQUE(Name)[] \
                                                                    USED SEGMENT(".modinfo") ALIGN(1) \
                                                                    = STRINGIFY(Tag) "=" Data

/* XTLDR directories */
#define XTBL_LOADER_DIRECTORY_PATH                                  L"\\EFI\\BOOT\\XTLDR\\"
#define XTBL_MODULES_DIRECTORY_PATH                                 L"\\EFI\\BOOT\\XTLDR\\MODULES\\"
#define XTBL_THEMES_DIRECTORY_PATH                                  L"\\EFI\\BOOT\\XTLDR\\THEMES\\"

/* Architecture specific definitions */
#if defined(__i386__) || defined(__i686__)
    #define XTBL_ARCH_LOADER_DIRECTORY_PATH                         L"\\EFI\\BOOT\\XTLDR32\\"
    #define XTBL_ARCH_MODULES_DIRECTORY_PATH                        L"\\EFI\\BOOT\\XTLDR32\\MODULES\\"
    #define XTBL_ARCH_THEMES_DIRECTORY_PATH                         L"\\EFI\\BOOT\\XTLDR32\\THEMES\\"
#elif defined(__amd64__) || defined(__x86_64__)
    #define XTBL_ARCH_LOADER_DIRECTORY_PATH                         L"\\EFI\\BOOT\\XTLDR64\\"
    #define XTBL_ARCH_MODULES_DIRECTORY_PATH                        L"\\EFI\\BOOT\\XTLDR64\\MODULES\\"
    #define XTBL_ARCH_THEMES_DIRECTORY_PATH                         L"\\EFI\\BOOT\\XTLDR64\\THEMES\\"
#else
    #error Unknown architecture
#endif

#endif /* __XTDK_BLTARGET_H */
