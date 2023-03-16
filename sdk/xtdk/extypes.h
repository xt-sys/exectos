/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/extypes.h
 * DESCRIPTION:     Kernel executive structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_EXTYPES_H
#define __XTDK_EXTYPES_H

#include <xtbase.h>
#include <xttypes.h>


/* Executive rundown protection structure definition */
typedef struct _EX_RUNDOWN_REFERENCE
{
    union
    {
        ULONG_PTR Count;
        PVOID Ptr;
    };
} EX_RUNDOWN_REFERENCE, *PEX_RUNDOWN_REFERENCE;

#endif /* __XTDK_EXTYPES_H */
