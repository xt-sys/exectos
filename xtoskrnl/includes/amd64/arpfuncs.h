/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/amd64/arpfuncs.h
 * DESCRIPTION:     Private AMD64 routine definitions for architecture library
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_AMD64_ARPFUNCS_H
#define __XTOSKRNL_AMD64_ARPFUNCS_H

#include <xtos.h>


XTAPI
VOID
ArSetGdtEntryBase(IN PKGDTENTRY Gdt,
                  IN USHORT Selector,
                  IN ULONG_PTR Base);

XTCDECL
VOID
ArpHandleSystemCall32();

XTCDECL
VOID
ArpHandleSystemCall64();

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

#endif /* __XTOSKRNL_AMD64_ARPFUNCS_H */
