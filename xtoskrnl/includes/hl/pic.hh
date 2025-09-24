/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/pic.hh
 * DESCRIPTION:     HAL processor support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_PIC_HH
#define __XTOSKRNL_HL_PIC_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class Pic
    {
        private:
            STATIC APIC_MODE ApicMode;

        public:
            STATIC XTAPI VOID ClearApicErrors(VOID);
            STATIC XTAPI ULONG GetCpuApicId(VOID);
            STATIC XTAPI VOID InitializePic(VOID);
            STATIC XTFASTCALL ULONGLONG ReadApicRegister(IN APIC_REGISTER Register);
            STATIC XTAPI VOID SendEoi(VOID);
            STATIC XTAPI VOID SendIpi(ULONG ApicId,
                                      ULONG Vector);
            STATIC XTFASTCALL VOID WriteApicRegister(IN APIC_REGISTER Register,
                                                     IN ULONGLONG Value);

        private:
            STATIC XTAPI BOOLEAN CheckX2ApicSupport(VOID);
            STATIC XTCDECL VOID HandleApicSpuriousService(VOID);
            STATIC XTCDECL VOID HandlePicSpuriousService(VOID);
            STATIC XTAPI VOID InitializeApic(VOID);
            STATIC XTAPI VOID InitializeLegacyPic(VOID);
    };
}

#endif /* __XTOSKRNL_HL_PIC_HH */
