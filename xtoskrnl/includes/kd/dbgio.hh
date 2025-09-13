/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/kd/dbgio.hh
 * DESCRIPTION:     Kernel Debugger I/O support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KD_DBGIO_HH
#define __XTOSKRNL_KD_DBGIO_HH

#include <xtos.hh>


/* Kernel Debugger */
namespace KD
{
    class DebugIo
    {
        public:
            STATIC PKD_PRINT_ROUTINE KdPrint;

        private:
            STATIC KD_DEBUG_MODE DebugMode;
            STATIC PKD_INIT_ROUTINE IoProvidersInitRoutines[KDBG_PROVIDERS_COUNT];
            STATIC LIST_ENTRY Providers;
            STATIC CPPORT SerialPort;
            STATIC ULONG SerialPortList[COMPORT_COUNT];

        public:
            STATIC XTCDECL VOID DbgPrint(PCWSTR Format,
                                         ...);
            STATIC XTCDECL VOID DbgPrintEx(PCWSTR Format,
                                           VA_LIST Arguments);
            STATIC XTAPI XTSTATUS InitializeDebugIoProviders(VOID);
            STATIC XTAPI VOID SetPrintRoutine(PKD_PRINT_ROUTINE DebugPrintRoutine);

        private:
            STATIC XTAPI XTSTATUS DetectDebugPorts(VOID);
            STATIC XTAPI XTSTATUS InitializeFrameBufferProvider(VOID);
            STATIC XTAPI XTSTATUS InitializeSerialPortProvider(VOID);
            STATIC XTCDECL XTSTATUS SerialWriteCharacter(WCHAR Character);
    };
}

#endif /* __XTOSKRNL_KD_DBGIO_HH */
