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
#include <ketypes.h>


/* Kernel's representation of a process object */
typedef struct _EPROCESS
{
    KPROCESS ProcessControlBlock;
    UINT Reserved0;
} EPROCESS, *PEPROCESS;

/* Kernel's representation of a thread object */
typedef struct _ETHREAD
{
    KTHREAD ThreadControlBlock;
    UINT Reserved0;
} ETHREAD, *PETHREAD;

#endif /* __XTDK_PSTYPES_H */
