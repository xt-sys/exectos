/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/ioreg.hh
 * DESCRIPTION:     Basic I/O registers access functionality
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_IOREG_HH
#define __XTOSKRNL_HL_IOREG_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class IoRegister
    {
        public:
            STATIC XTAPI UCHAR ReadRegister8(IN PVOID Register);
            STATIC XTAPI USHORT ReadRegister16(IN PVOID Register);
            STATIC XTAPI ULONG ReadRegister32(IN PVOID Register);
            STATIC XTAPI VOID WriteRegister8(IN PVOID Register,
                                             IN UCHAR Value);
            STATIC XTAPI VOID WriteRegister16(IN PVOID Register,
                                              IN USHORT Value);
            STATIC XTAPI VOID WriteRegister32(IN PVOID Register,
                                              IN ULONG Value);
    };
}

#endif /* __XTOSKRNL_HL_IOREG_HH */
