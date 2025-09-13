/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/kd/globals.cc
 * DESCRIPTION:     Kernel Debugger global and static data
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/* Pointer to DbgPrint() routine */
PKD_PRINT_ROUTINE KdPrint = nullptr;

/* Kernel Debugger mode */
KD_DEBUG_MODE KD::DebugIo::DebugMode;

/* Debugger I/O providers initialization routines */
PKD_INIT_ROUTINE KD::DebugIo::IoProvidersInitRoutines[KDBG_PROVIDERS_COUNT] = {
    InitializeFrameBufferProvider,
    InitializeSerialPortProvider
};

/* List of active I/O providers */
LIST_ENTRY KD::DebugIo::Providers;

/* Debugger's serial port handle */
CPPORT KD::DebugIo::SerialPort;

/* Pre-defined serial port addresses */
ULONG KD::DebugIo::SerialPortList[COMPORT_COUNT] = COMPORT_ADDRESS;
