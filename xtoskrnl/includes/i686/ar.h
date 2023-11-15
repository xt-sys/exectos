/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/i686/ar.h
 * DESCRIPTION:     I686 architecture library routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_I686_AR_H
#define __XTOSKRNL_I686_AR_H

#include <xtos.h>


/* I686 architecture library routines forward references */
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
ULONG_PTR
ArReadDebugRegister(IN USHORT DebugRegister);

XTCDECL
ULONG
ArReadFSDualWord(ULONG Offset);

XTCDECL
ULONGLONG
ArReadModelSpecificRegister(IN ULONG Register);

XTCDECL
UINT
ArReadMxCsrRegister();

XTCDECL
ULONGLONG
ArReadTimeStampCounter();

XTAPI
VOID
ArSetGdtEntryBase(IN PKGDTENTRY Gdt,
                  IN USHORT Selector,
                  IN ULONG_PTR Base);

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
ArYieldProcessor();

XTCDECL
VOID
ArpHandleTrap00();

XTCDECL
VOID
ArpHandleTrap01();

XTCDECL
VOID
ArpHandleTrap02();

XTCDECL
VOID
ArpHandleTrap03();

XTCDECL
VOID
ArpHandleTrap04();

XTCDECL
VOID
ArpHandleTrap05();

XTCDECL
VOID
ArpHandleTrap06();

XTCDECL
VOID
ArpHandleTrap07();

XTCDECL
VOID
ArpHandleTrap08();

XTCDECL
VOID
ArpHandleTrap09();

XTCDECL
VOID
ArpHandleTrap0A();

XTCDECL
VOID
ArpHandleTrap0B();

XTCDECL
VOID
ArpHandleTrap0C();

XTCDECL
VOID
ArpHandleTrap0D();

XTCDECL
VOID
ArpHandleTrap0E();

XTCDECL
VOID
ArpHandleTrap10();

XTCDECL
VOID
ArpHandleTrap11();

XTCDECL
VOID
ArpHandleTrap12();

XTCDECL
VOID
ArpHandleTrap13();

XTCDECL
VOID
ArpHandleTrap2C();

XTCDECL
VOID
ArpHandleTrap2D();

XTCDECL
VOID
ArpHandleTrapFF();

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
ArpSetDoubleFaultTssEntry(IN PKPROCESSOR_BLOCK ProcessorBlock);

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
ArpSetNonMaskableInterruptTssEntry(IN PKPROCESSOR_BLOCK ProcessorBlock);

#endif /* __XTOSKRNL_I686_AR_H */
