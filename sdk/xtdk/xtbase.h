/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtbase.h
 * DESCRIPTION:     Basic and common native XT structures
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTBASE_H
#define __XTDK_XTBASE_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>


/* Interrupt Request Level (IRQL) */
typedef UCHAR KIRQL, *PKIRQL;

/* Processor modes */
typedef CHAR KPROCESSOR_MODE, *PKPROCESSOR_MODE;

/* 128-bit buffer containing a unique identifier value */
typedef struct _GUID
{
    UINT Data1;
    USHORT Data2;
    USHORT Data3;
    UCHAR Data4[8];
} GUID, *PGUID;

/* Double linked list structure definition */
typedef struct _LIST_ENTRY
{
    PLIST_ENTRY Flink;
    PLIST_ENTRY Blink;
} LIST_ENTRY, *PLIST_ENTRY;

/* 32-bit double linked list structure definition */
typedef struct _LIST_ENTRY32
{
    ULONG Flink;
    ULONG Blink;
} LIST_ENTRY32, *PLIST_ENTRY32;

/* 64-bit double linked list structure definition */
typedef struct _LIST_ENTRY64
{
    ULONGLONG Flink;
    ULONGLONG Blink;
} LIST_ENTRY64, *PLIST_ENTRY64;

/* 128-bit 16-byte aligned XMM register */
typedef struct _M128
{
    ULONGLONG Low;
    LONGLONG High;
} ALIGN(16) M128, *PM128;

#endif /* __XTDK_XTBASE_H */
