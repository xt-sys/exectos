/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/cpu.hh
 * DESCRIPTION:     Serial (COM) port support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_CPORT_HH
#define __XTOSKRNL_HL_CPORT_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class ComPort
    {
        public:
            STATIC XTCDECL XTSTATUS ReadComPort(IN PCPPORT Port,
                                                OUT PUCHAR Byte,
                                                IN BOOLEAN Wait,
                                                IN BOOLEAN Poll);
            STATIC XTCDECL UCHAR ReadComPortLsr(IN PCPPORT Port,
                                                IN UCHAR Byte);
            STATIC XTCDECL XTSTATUS InitializeComPort(IN OUT PCPPORT Port,
                                                      IN PUCHAR PortAddress,
                                                      IN ULONG BaudRate);
            STATIC XTCDECL XTSTATUS WriteComPort(IN PCPPORT Port,
                                                 IN UCHAR Byte);
    };
}

#endif /* __XTOSKRNL_HL_CPORT_HH */
