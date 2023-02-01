/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/arpfuncs.h
 * DESCRIPTION:     Private routine definitions for architecture library
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_ARPFUNCS_H
#define __XTOSKRNL_ARPFUNCS_H

#include <xtos.h>


XTCDECL
VOID
ArpHandleTrap02();

XTCDECL
VOID
ArpHandleTrap08();

XTCDECL
VOID
ArpHandleTrap0D();

XTCDECL
VOID
ArpHandleTrap0E();

XTCDECL
VOID
ArpHandleTrapFF();

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

#endif /* __XTOSKRNL_ARPFUNCS_H */
