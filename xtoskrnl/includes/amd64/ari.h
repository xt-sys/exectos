/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/amd64/ari.h
 * DESCRIPTION:     AMD64 architecture library routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_AMD64_ARI_H
#define __XTOSKRNL_AMD64_ARI_H

#include <xtos.h>


/* AMD64 architecture library routines forward references */
XTCDECL
VOID
ArClearInterruptFlag(VOID);

XTCDECL
BOOLEAN
ArCpuId(IN OUT PCPUID_REGISTERS Registers);

XTCDECL
VOID
ArFlushTlb(VOID);

XTASSEMBLY
XTCDECL
ULONG_PTR
ArGetStackPointer(VOID);

XTCDECL
VOID
ArHalt(VOID);

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
ArLoadMxcsrRegister(IN ULONG Source);

XTCDECL
VOID
ArLoadSegment(IN USHORT Segment,
              IN ULONG Source);

XTCDECL
VOID
ArLoadTaskRegister(USHORT Source);

XTCDECL
VOID
ArMemoryBarrier(VOID);

XTCDECL
ULONG_PTR
ArReadControlRegister(IN USHORT ControlRegister);

XTCDECL
ULONG_PTR
ArReadDebugRegister(IN USHORT DebugRegister);

XTCDECL
ULONGLONG
ArReadGSQuadWord(ULONG Offset);

XTCDECL
ULONGLONG
ArReadModelSpecificRegister(IN ULONG Register);

XTCDECL
UINT
ArReadMxCsrRegister(VOID);

XTCDECL
ULONGLONG
ArReadTimeStampCounter(VOID);

XTCDECL
VOID
ArReadWriteBarrier(VOID);

XTAPI
VOID
ArSetGdtEntryBase(IN PKGDTENTRY Gdt,
                  IN USHORT Selector,
                  IN ULONG_PTR Base);

XTCDECL
VOID
ArSetInterruptFlag(VOID);

XTCDECL
VOID
ArStoreGlobalDescriptorTable(OUT PVOID Destination);

XTCDECL
VOID
ArStoreInterruptDescriptorTable(OUT PVOID Destination);

XTCDECL
VOID
ArStoreLocalDescriptorTable(OUT PVOID Destination);

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
ArWriteDebugRegister(IN USHORT DebugRegister,
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
ArYieldProcessor(VOID);

XTCDECL
VOID
ArpHandleSystemCall32(VOID);

XTCDECL
VOID
ArpHandleSystemCall64(VOID);

XTCDECL
VOID
ArpHandleTrap00(VOID);

XTCDECL
VOID
ArpHandleTrap01(VOID);

XTCDECL
VOID
ArpHandleTrap02(VOID);

XTCDECL
VOID
ArpHandleTrap03(VOID);

XTCDECL
VOID
ArpHandleTrap04(VOID);

XTCDECL
VOID
ArpHandleTrap05(VOID);

XTCDECL
VOID
ArpHandleTrap06(VOID);

XTCDECL
VOID
ArpHandleTrap07(VOID);

XTCDECL
VOID
ArpHandleTrap08(VOID);

XTCDECL
VOID
ArpHandleTrap09(VOID);

XTCDECL
VOID
ArpHandleTrap0A(VOID);

XTCDECL
VOID
ArpHandleTrap0B(VOID);

XTCDECL
VOID
ArpHandleTrap0C(VOID);

XTCDECL
VOID
ArpHandleTrap0D(VOID);

XTCDECL
VOID
ArpHandleTrap0E(VOID);

XTCDECL
VOID
ArpHandleTrap10(VOID);

XTCDECL
VOID
ArpHandleTrap11(VOID);

XTCDECL
VOID
ArpHandleTrap12(VOID);

XTCDECL
VOID
ArpHandleTrap13(VOID);

XTCDECL
VOID
ArpHandleTrap2C(VOID);

XTCDECL
VOID
ArpHandleTrap2D(VOID);

XTCDECL
VOID
ArpHandleTrapFF(VOID);

XTAPI
VOID
ArpIdentifyProcessor(VOID);

XTAPI
VOID
ArpInitializeGdt(IN PKPROCESSOR_BLOCK ProcessorBlock);

XTAPI
VOID
ArpInitializeIdt(IN PKPROCESSOR_BLOCK ProcessorBlock);

XTAPI
VOID
ArpInitializeProcessorBlock(OUT PKPROCESSOR_BLOCK ProcessorBlock,
                            IN PKGDTENTRY Gdt,
                            IN PKIDTENTRY Idt,
                            IN PKTSS Tss,
                            IN PVOID DpcStack);

XTAPI
VOID
ArpInitializeProcessorRegisters(VOID);

XTAPI
VOID
ArpInitializeSegments(VOID);

XTAPI
VOID
ArpInitializeTss(IN PKPROCESSOR_BLOCK ProcessorBlock);

XTAPI
VOID
ArpSetGdtEntry(IN PKGDTENTRY Gdt,
               IN USHORT Selector,
               IN ULONG_PTR Base,
               IN ULONG Limit,
               IN UCHAR Type,
               IN UCHAR Dpl,
               IN UCHAR SegmentMode);

XTAPI
VOID
ArpSetIdtGate(IN PKIDTENTRY Idt,
              IN USHORT Vector,
              IN PVOID Handler,
              IN USHORT Selector,
              IN USHORT Ist,
              IN USHORT Access);

#endif /* __XTOSKRNL_AMD64_ARI_H */
