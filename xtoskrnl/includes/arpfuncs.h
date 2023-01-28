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


XTAPI
VOID
ArpInitializeGdt(IN PKGDTENTRY Gdt);

XTAPI
VOID
ArpInitializeGdtEntry(IN PKGDTENTRY Gdt,
                      IN USHORT Selector,
                      IN ULONGLONG Base,
                      IN ULONG Limit,
                      IN UCHAR Type,
                      IN UCHAR Dpl,
                      IN UCHAR SegmentMode);

XTAPI
VOID
ArpInitializeTss(IN PKTSS Tss,
                 IN PKGDTENTRY Gdt);

#endif /* __XTOSKRNL_ARPFUNCS_H */
