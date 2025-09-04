/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/kdi.h
 * DESCRIPTION:     Kernel Debugger routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KDI_H
#define __XTOSKRNL_KDI_H

#include <xtos.h>


/* Kernel Debugger routines forward references */
XTAPI
XTSTATUS
KdInitializeDebugIoProviders(VOID);

XTAPI
VOID
KdSetPrintRoutine(PVOID DebugPrintRoutine);

XTCDECL
VOID
KdpDebugPrint(PCWSTR Format, ...);

XTAPI
XTSTATUS
KdpDetectDebugPorts(VOID);

XTAPI
XTSTATUS
KdpInitializeFrameBufferProvider(VOID);

XTAPI
XTSTATUS
KdpInitializeSerialPortProvider(VOID);

XTCDECL
XTSTATUS
KdpSerialWriteCharacter(WCHAR Character);

#endif /* __XTOSKRNL_KDI_H */
