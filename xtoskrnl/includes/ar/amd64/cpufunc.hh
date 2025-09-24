/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ar/amd64/cpufunc.hh
 * DESCRIPTION:     Architecture-specific CPU control and utility functions for low-level system operations
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_AR_CPUFUNC_HH
#define __XTOSKRNL_AR_CPUFUNC_HH

#include <xtos.hh>


/* Architecture-specific Library */
namespace AR
{
    class CpuFunc
    {
        public:
            STATIC XTCDECL VOID ClearInterruptFlag(VOID);
            STATIC XTCDECL BOOLEAN CpuId(IN OUT PCPUID_REGISTERS Registers);
            STATIC XTCDECL VOID FlushTlb(VOID);
            STATIC XTCDECL ULONG GetCpuFlags(VOID);
            STATIC XTASSEMBLY XTCDECL ULONG_PTR GetStackPointer(VOID);
            STATIC XTCDECL VOID Halt(VOID);
            STATIC XTCDECL BOOLEAN InterruptsEnabled(VOID);
            STATIC XTCDECL VOID InvalidateTlbEntry(IN PVOID Address);
            STATIC XTCDECL VOID LoadGlobalDescriptorTable(IN PVOID Source);
            STATIC XTCDECL VOID LoadInterruptDescriptorTable(IN PVOID Source);
            STATIC XTCDECL VOID LoadLocalDescriptorTable(IN USHORT Source);
            STATIC XTCDECL VOID LoadMxcsrRegister(IN ULONG Source);
            STATIC XTCDECL VOID LoadSegment(IN USHORT Segment,
                                            IN ULONG Source);
            STATIC XTCDECL VOID LoadTaskRegister(USHORT Source);
            STATIC XTCDECL VOID MemoryBarrier(VOID);
            STATIC XTCDECL ULONG_PTR ReadControlRegister(IN USHORT ControlRegister);
            STATIC XTCDECL ULONG_PTR ReadDebugRegister(IN USHORT DebugRegister);
            STATIC XTCDECL ULONGLONG ReadGSQuadWord(ULONG Offset);
            STATIC XTCDECL ULONGLONG ReadModelSpecificRegister(IN ULONG Register);
            STATIC XTCDECL UINT ReadMxCsrRegister(VOID);
            STATIC XTCDECL ULONGLONG ReadTimeStampCounter(VOID);
            STATIC XTCDECL VOID ReadWriteBarrier(VOID);
            STATIC XTCDECL VOID SetInterruptFlag(VOID);
            STATIC XTCDECL VOID StoreGlobalDescriptorTable(OUT PVOID Destination);
            STATIC XTCDECL VOID StoreInterruptDescriptorTable(OUT PVOID Destination);
            STATIC XTCDECL VOID StoreLocalDescriptorTable(OUT PVOID Destination);
            STATIC XTCDECL VOID StoreSegment(IN USHORT Segment,
                                             OUT PVOID Destination);
            STATIC XTCDECL VOID StoreTaskRegister(OUT PVOID Destination);
            STATIC XTCDECL VOID WriteControlRegister(IN USHORT ControlRegister,
                                                     IN UINT_PTR Value);
            STATIC XTCDECL VOID WriteDebugRegister(IN USHORT DebugRegister,
                                                   IN UINT_PTR Value);
            STATIC XTCDECL VOID WriteEflagsRegister(IN UINT_PTR Value);
            STATIC XTCDECL VOID WriteModelSpecificRegister(IN ULONG Register,
                                                           IN ULONGLONG Value);
            STATIC XTCDECL VOID YieldProcessor(VOID);
    };
}

#endif /* __XTOSKRNL_AR_CPUFUNC_HH */
