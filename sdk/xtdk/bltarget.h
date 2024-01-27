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


/* Boot Loader module segment macros */
#define XTBL_MODDEPS                                                SEGMENT(".moddeps") CONST WCHAR XtBlpDeps[][9]
#define XTBL_MODINFO                                                SEGMENT(".modinfo") CONST WCHAR XtBlpInfo[]

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
