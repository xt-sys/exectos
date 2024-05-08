/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/i686/ari.h
 * DESCRIPTION:     I686 architecture library routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_I686_ARI_H
#define __XTOSKRNL_I686_ARI_H

#include <xtos.h>


/* I686 architecture library routines forward references */
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
ArInitializeProcessor(IN PVOID ProcessorStructures);

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
ArLoadTaskRegister(IN USHORT Source);

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
ULONG
ArReadFSDualWord(IN ULONG Offset);

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
ArpDispatchTrap(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap00(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap01(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap02(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap03(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap04(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap05(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap06(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap07(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap08(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap09(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap0A(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap0B(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap0C(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap0D(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap0E(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap10(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap11(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap12(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap13(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap2A(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap2B(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap2C(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap2D(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrap2E(IN PKTRAP_FRAME TrapFrame);

XTCDECL
VOID
ArpHandleTrapFF(IN PKTRAP_FRAME TrapFrame);

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
ArpInitializeProcessorStructures(IN PVOID ProcessorStructures,
                                 OUT PKGDTENTRY *Gdt,
                                 OUT PKTSS *Tss,
                                 OUT PKPROCESSOR_BLOCK *ProcessorBlock,
                                 OUT PVOID *KernelBootStack,
                                 OUT PVOID *KernelFaultStack);

XTAPI
VOID
ArpInitializeSegments(VOID);

XTAPI
VOID
ArpInitializeTss(IN PKPROCESSOR_BLOCK ProcessorBlock,
                 IN PVOID KernelBootStack,
                 IN PVOID KernelFaultStack);

XTAPI
VOID
ArpSetDoubleFaultTssEntry(IN PKPROCESSOR_BLOCK ProcessorBlock,
                          IN PVOID KernelFaultStack);

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

XTAPI
VOID
ArpSetNonMaskableInterruptTssEntry(IN PKPROCESSOR_BLOCK ProcessorBlock,
                                   IN PVOID KernelFaultStack);

XTCDECL
VOID
ArpTrap0x00(VOID);

XTCDECL
VOID
ArpTrap0x01(VOID);

XTCDECL
VOID
ArpTrap0x02(VOID);

XTCDECL
VOID
ArpTrap0x03(VOID);

XTCDECL
VOID
ArpTrap0x04(VOID);

XTCDECL
VOID
ArpTrap0x05(VOID);

XTCDECL
VOID
ArpTrap0x06(VOID);

XTCDECL
VOID
ArpTrap0x07(VOID);

XTCDECL
VOID
ArpTrap0x08(VOID);

XTCDECL
VOID
ArpTrap0x09(VOID);

XTCDECL
VOID
ArpTrap0x0A(VOID);

XTCDECL
VOID
ArpTrap0x0B(VOID);

XTCDECL
VOID
ArpTrap0x0C(VOID);

XTCDECL
VOID
ArpTrap0x0D(VOID);

XTCDECL
VOID
ArpTrap0x0E(VOID);

XTCDECL
VOID
ArpTrap0x10(VOID);

XTCDECL
VOID
ArpTrap0x11(VOID);

XTCDECL
VOID
ArpTrap0x12(VOID);

XTCDECL
VOID
ArpTrap0x13(VOID);

XTCDECL
VOID
ArpTrap0x2A(VOID);

XTCDECL
VOID
ArpTrap0x2B(VOID);

XTCDECL
VOID
ArpTrap0x2C(VOID);

XTCDECL
VOID
ArpTrap0x2D(VOID);

XTCDECL
VOID
ArpTrap0x2E(VOID);

#endif /* __XTOSKRNL_I686_ARI_H */
