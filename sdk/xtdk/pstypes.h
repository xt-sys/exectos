/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/pstypes.h
 * DESCRIPTION:     Process-related data structure definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_PSTYPES_H
#define __XTDK_PSTYPES_H

#include <xttypes.h>
#include <xtstruct.h>
#include <extypes.h>


/* Quota bypass marker */
#define PS_QUOTA_BYPASS_MARKER                              ((PEPROCESS_QUOTA_BLOCK)1)

/* Current process & thread handles */
#define PS_CURRENT_PROCESS_HANDLE                           ((HANDLE)(LONG_PTR)-1)
#define PS_CURRENT_THREAD_HANDLE                            ((HANDLE)(LONG_PTR)-2)

/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* Process quota types */
typedef enum _PS_QUOTA_TYPE
{
    PsNonPagedPool,
    PsPagedPool,
    PsPageFile,
    PsQuotaTypes
} PS_QUOTA_TYPE, *PPS_QUOTA_TYPE;

/* Kernel's representation of a process object */
typedef struct _EPROCESS
{
    KPROCESS ProcessControlBlock;
    KPUSH_LOCK ProcessLock;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER ExitTime;
    EX_RUNDOWN_REFERENCE RundownProtect;
    HANDLE UniqueProcessId;
    SIZE_T QuotaUsage[PsQuotaTypes];
    PHANDLE_TABLE ObjectTable;
    ACCESS_MASK GrantedAccess;
} EPROCESS, *PEPROCESS;

/* Kernel's representation of a process quota entry*/
typedef struct _EPROCESS_QUOTA_ENTRY
{
    SIZE_T Usage;
    SIZE_T Limit;
    SIZE_T Peak;
    SIZE_T Return;
} EPROCESS_QUOTA_ENTRY, *PEPROCESS_QUOTA_ENTRY;

/* Kernel's representation of a process quota block */
typedef struct _EPROCESS_QUOTA_BLOCK
{
    EPROCESS_QUOTA_ENTRY QuotaEntry[PsQuotaTypes];
    LIST_ENTRY QuotaList;
    ULONG ReferenceCount;
    ULONG ProcessCount;
} EPROCESS_QUOTA_BLOCK, *PEPROCESS_QUOTA_BLOCK;

/* Kernel's representation of a thread object */
typedef struct _ETHREAD
{
    KTHREAD ThreadControlBlock;
    ACCESS_MASK GrantedAccess;
} ETHREAD, *PETHREAD;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_PSTYPES_H */
