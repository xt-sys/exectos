/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/potypes.h
 * DESCRIPTION:     Power manager subsystem structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_POTYPES_H
#define __XTDK_POTYPES_H

#include <xttypes.h>
#include <xtstruct.h>
#include <ketypes.h>


/* Power Manager routine callbacks */
typedef VOID (XTFASTCALL *PPROCESSOR_IDLE_FUNCTION)(PPROCESSOR_POWER_STATE PowerState);

/* Processor power state structure definition */
typedef struct _PROCESSOR_POWER_STATE
{
    PPROCESSOR_IDLE_FUNCTION IdleFunction;
    ULONG Idle0TimeLimit;
    UCHAR CurrentThrottle;
    KTIMER PerfTimer;
    KDPC PerfDpc;
} PROCESSOR_POWER_STATE, *PPROCESSOR_POWER_STATE;

#endif /* __XTDK_POTYPES_H */
