/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtcommon.h
 * DESCRIPTION:     Basic and common XT structures for kernel and user modes
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTCOMMON_H
#define __XTDK_XTCOMMON_H

#include "xttypes.h"
#include "xtstruct.h"


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

#endif /* __XTDK_XTCOMMON_H */
