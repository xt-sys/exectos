/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/firmware.hh
 * DESCRIPTION:     UEFI/BIOS Firmware support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_FIRMWARE_HH
#define __XTOSKRNL_HL_FIRMWARE_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class Firmware
    {
        public:
            STATIC XTFASTCALL UCHAR ReadCmosRegister(IN UCHAR Register);
            STATIC XTFASTCALL VOID WriteCmosRegister(IN UCHAR Register,
                                                     IN UCHAR Value);
    };
}

#endif /* __XTOSKRNL_HL_FIRMWARE_HH */
