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

/* Kernel priority */
typedef LONG KPRIORITY, *PKPRIORITY;

/* Processor modes */
typedef CHAR KPROCESSOR_MODE, *PKPROCESSOR_MODE;

/* Spin locks synchronization mechanism */
typedef ULONG_PTR KSPIN_LOCK, *PKSPIN_LOCK;

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

/* Single linked list structure definition */
typedef struct _SINGLE_LIST_ENTRY
{
    PSINGLE_LIST_ENTRY Next;
} SINGLE_LIST_ENTRY, *PSINGLE_LIST_ENTRY;

/* Header for a sequenced single linked list union definition */
typedef union _SINGLE_LIST_HEADER
{
    ULONGLONG Alignment;
    struct
    {
        SINGLE_LIST_ENTRY Next;
        USHORT Depth;
        USHORT Sequence;
    };
} SINGLE_LIST_HEADER, *PSINGLE_LIST_HEADER;

/* 128-bit 16-byte aligned XMM register */
typedef struct _M128
{
    ULONGLONG Low;
    LONGLONG High;
} ALIGN(16) M128, *PM128;

/* Dispatcher object header structure definition */
typedef struct _DISPATCHER_HEADER
{
    UCHAR Type;
    UCHAR Absolute;
    UCHAR Inserted;
    BOOLEAN DebugActive;
    LONG SignalState;
    LIST_ENTRY WaitListHead;
} DISPATCHER_HEADER, *PDISPATCHER_HEADER;

#endif /* __XTDK_XTBASE_H */
