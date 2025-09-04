/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/kd/globals.c
 * DESCRIPTION:     Architecture independent global variables related to KD subsystem
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


/* Pointer to DbgPrint() routine */
PKD_PRINT_ROUTINE KdPrint = NULL;

/* Kernel Debugger mode */
KD_DEBUG_MODE KdpDebugMode;

/* Debugger I/O providers initialization routines */
PKD_INIT_ROUTINE KdpIoProvidersInitRoutines[KDBG_PROVIDERS_COUNT] = {
    KdpInitializeFrameBufferProvider,
    KdpInitializeSerialPortProvider
};

/* List of active I/O providers */
LIST_ENTRY KdpProviders;

/* Debugger's serial port handle */
CPPORT KdpSerialPort;

/* Pre-defined serial port addresses */
ULONG KdpSerialPortList[COMPORT_COUNT] = COMPORT_ADDRESS;
