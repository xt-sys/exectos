/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/ioport.hh
 * DESCRIPTION:     I/O port access routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_IOPORT_HH
#define __XTOSKRNL_HL_IOPORT_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class IoPort
    {
        public:
            STATIC XTCDECL UCHAR ReadPort8(IN USHORT Port);
            STATIC XTCDECL USHORT ReadPort16(IN USHORT Port);
            STATIC XTCDECL ULONG ReadPort32(IN USHORT Port);
            STATIC XTCDECL VOID WritePort8(IN USHORT Port,
                                           IN UCHAR Value);
            STATIC XTCDECL VOID WritePort16(IN USHORT Port,
                                            IN USHORT Value);
            STATIC XTCDECL VOID WritePort32(IN USHORT Port,
                                            IN ULONG Value);
    };
}

#endif /* __XTOSKRNL_HL_IOPORT_HH */
