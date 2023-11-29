/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/amd64/hl.h
 * DESCRIPTION:     XT hardware abstraction layer routines specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_AMD64_HL_H
#define __XTOSKRNL_AMD64_HL_H

#include <xtos.h>


/* HAL library routines forward references */
XTCDECL
VOID
HlDisablePic(VOID);

XTFASTCALL
ULONG
HlReadApicRegister(IN APIC_REGISTER Register);

XTFASTCALL
VOID
HlWriteApicRegister(IN APIC_REGISTER Register,
                    IN ULONG Value);

XTAPI
BOOLEAN
HlpCheckX2ApicSupport(VOID);

XTCDECL
VOID
HlpHandleApicSpuriousService();

XTCDECL
VOID
HlpHandlePicSpuriousService();

XTAPI
VOID
HlpInitializeApic(VOID);

XTFASTCALL
KRUNLEVEL
HlpTransformApicTprToRunLevel(IN UCHAR Tpr);

XTFASTCALL
UCHAR
HlpTransformRunLevelToApicTpr(IN KRUNLEVEL RunLevel);

#endif /* __XTOSKRNL_AMD64_HL_H */
