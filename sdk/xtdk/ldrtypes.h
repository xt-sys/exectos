/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/ldrtypes.h
 * DESCRIPTION:     Loader structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_LDRTYPES_H
#define __XTDK_LDRTYPES_H

#include "xtbase.h"


/* Loader Data Table Entry Flags */
#define LDR_DTE_STATIC_LINK                        0x00000002
#define LDR_DTE_IMAGE_DLL                          0x00000004
#define LDR_DTE_SHIMENG_SUPPRESSED_ENTRY           0x00000008
#define LDR_DTE_IMAGE_INTEGRITY_FORCED             0x00000020
#define LDR_DTE_LOAD_IN_PROGRESS                   0x00001000
#define LDR_DTE_UNLOAD_IN_PROGRESS                 0x00002000
#define LDR_DTE_ENTRY_PROCESSED                    0x00004000
#define LDR_DTE_ENTRY_INSERTED                     0x00008000
#define LDR_DTE_CURRENT_LOAD                       0x00010000
#define LDR_DTE_FAILED_BUILTIN_LOAD                0x00020000
#define LDR_DTE_DONT_CALL_FOR_THREADS              0x00040000
#define LDR_DTE_PROCESS_ATTACH_CALLED              0x00080000
#define LDR_DTE_DEBUG_SYMBOLS_LOADED               0x00100000
#define LDR_DTE_IMAGE_NOT_AT_BASE                  0x00200000
#define LDR_DTE_COR_IMAGE                          0x00400000
#define LDR_DTE_COR_OWNS_UNMAP                     0x00800000
#define LDR_DTE_SYSTEM_MAPPED                      0x01000000
#define LDR_DTE_IMAGE_VERIFYING                    0x02000000
#define LDR_DTE_DRIVER_DEPENDENT_DLL               0x04000000
#define LDR_DTE_ENTRY_NATIVE                       0x08000000
#define LDR_DTE_REDIRECTED                         0x10000000
#define LDR_DTE_NON_PAGED_DEBUG_INFO               0x20000000
#define LDR_DTE_MM_LOADED                          0x40000000
#define LDR_DTE_COMPAT_DATABASE_PROCESSED          0x80000000

/* Loader data table entry */
typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union
    {
        LIST_ENTRY HashLinks;
        struct
        {
            PVOID SectionPointer;
            ULONG CheckSum;
        };
    };
    union
    {
        ULONG TimeDateStamp;
        PVOID LoadedImports;
    };
    PVOID EntryPointActivationContext;
    PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

#endif /* __XTDK_LDRTYPES_H */
