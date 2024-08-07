/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/amd64/hli.h
 * DESCRIPTION:     XT hardware abstraction layer routines specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_AMD64_HLI_H
#define __XTOSKRNL_AMD64_HLI_H

#include <xtos.h>


/* HAL library routines forward references */
XTAPI
VOID
HlClearApicErrors(VOID);

XTFASTCALL
ULONGLONG
HlReadApicRegister(IN APIC_REGISTER Register);

XTAPI
VOID
HlSendEoi(VOID);

XTFASTCALL
VOID
HlWriteApicRegister(IN APIC_REGISTER Register,
                    IN ULONGLONG Value);

XTAPI
BOOLEAN
HlpCheckX2ApicSupport(VOID);

XTAPI
ULONG
HlpGetCpuApicId(VOID);

XTCDECL
VOID
HlpHandleApicSpuriousService(VOID);

XTCDECL
VOID
HlpHandlePicSpuriousService(VOID);

XTAPI
VOID
HlpInitializeApic();

XTAPI
VOID
HlpInitializeLegacyPic(VOID);

XTAPI
VOID
HlpInitializePic();

XTAPI
VOID
HlpSendIpi(ULONG ApicId,
           ULONG Vector);

XTFASTCALL
KRUNLEVEL
HlpTransformApicTprToRunLevel(IN UCHAR Tpr);

XTFASTCALL
UCHAR
HlpTransformRunLevelToApicTpr(IN KRUNLEVEL RunLevel);

#endif /* __XTOSKRNL_AMD64_HLI_H */
