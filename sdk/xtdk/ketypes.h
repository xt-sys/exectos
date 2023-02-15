/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/ketypes.h
 * DESCRIPTION:     XT kernel core structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_KETYPES_H
#define __XTDK_KETYPES_H

#include <kertptr.h>
#include <xtstruct.h>
#include <xttarget.h>
#include <xttypes.h>


/* Maximum number of exception parameters */
#define EXCEPTION_MAXIMUM_PARAMETERS 15

/* APC Types */
typedef enum _MODE
{
    KernelMode,
    UserMode,
    MaximumMode
} MODE, *PMODE;

/* Exception disposition return values */
typedef enum _EXCEPTION_DISPOSITION
{
    ExceptionContinueExecution,
    ExceptionContinueSearch,
    ExceptionNestedException,
    ExceptionCollidedUnwind,
} EXCEPTION_DISPOSITION, *PEXCEPTION_DISPOSITION;

/* Exception record structure definition */
typedef struct _EXCEPTION_RECORD
{
    XTSTATUS ExceptionCode;
    ULONG ExceptionFlags;
    PEXCEPTION_RECORD ExceptionRecord;
    PVOID ExceptionAddress;
    ULONG NumberParameters;
    ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

/* Exception registration record  structure definition */
typedef struct _EXCEPTION_REGISTRATION_RECORD
{
    PEXCEPTION_REGISTRATION_RECORD Next;
    PEXCEPTION_ROUTINE Handler;
} EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD;

/* APC state structure definition */
typedef struct _KAPC_STATE
{
    LIST_ENTRY ApcListHead[MaximumMode];
    PKPROCESS Process;
    BOOLEAN KernelApcInProgress;
    BOOLEAN KernelApcPending;
    BOOLEAN UserApcPending;
} KAPC_STATE, *PKAPC_STATE;

/* Process control block structure definition */
typedef struct _KPROCESS
{
    INT PlaceHolder;
} KPROCESS, *PKPROCESS;

/* Thread control block structure definition */
typedef struct _KTHREAD
{
    PVOID InitialStack;
    PVOID KernelStack;
    PVOID StackBase;
    PVOID StackLimit;
    KAPC_STATE ApcState;
} KTHREAD, *PKTHREAD;

#endif /* __XTDK_KEFUNCS_H */
