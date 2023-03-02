/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/arfuncs.h
 * DESCRIPTION:     I686 architecture library routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_ARFUNCS_H
#define __XTDK_I686_ARFUNCS_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>
#include <i686/xtstruct.h>


/* Architecture library routines forward references */
XTCDECL
VOID
ArClearInterruptFlag();

XTCDECL
BOOLEAN
ArCpuId(IN OUT PCPUID_REGISTERS Registers);

XTCDECL
VOID
ArHalt();

XTAPI
VOID
ArInitializeProcessor(VOID);

XTCDECL
VOID
ArInvalidateTlbEntry(IN PVOID Address);

XTCDECL
VOID
ArLoadGlobalDescriptorTable(IN PVOID Source);

XTCDECL
VOID
ArLoadInterruptDescriptorTable(IN PVOID Source);

XTCDECL
VOID
ArLoadLocalDescriptorTable(IN USHORT Source);

XTCDECL
VOID
ArLoadSegment(IN USHORT Segment,
              IN ULONG Source);

XTCDECL
VOID
ArLoadTaskRegister(USHORT Source);

XTCDECL
ULONG_PTR
ArReadControlRegister(IN USHORT ControlRegister);

XTCDECL
ULONG
ArReadFSDualWord(ULONG Offset);

XTCDECL
ULONGLONG
ArReadModelSpecificRegister(IN ULONG Register);

XTCDECL
ULONGLONG
ArReadTimeStampCounter();

XTCDECL
VOID
ArSetInterruptFlag();

XTCDECL
VOID
ArStoreGlobalDescriptorTable(OUT PVOID Destination);

XTCDECL
VOID
ArStoreInterruptDescriptorTable(OUT PVOID Destination);

XTCDECL
VOID
ArStoreSegment(IN USHORT Segment,
               OUT PVOID Destination);

XTCDECL
VOID
ArStoreTaskRegister(OUT PVOID Destination);

XTCDECL
VOID
ArWriteControlRegister(IN USHORT ControlRegister,
                       IN UINT_PTR Value);

XTCDECL
VOID
ArWriteEflagsRegister(IN UINT_PTR Value);

XTCDECL
VOID
ArWriteModelSpecificRegister(IN ULONG Register,
                             IN ULONGLONG Value);

XTCDECL
VOID
ArYieldProcessor();

#endif /* __XTDK_I686_ARFUNCS_H */
