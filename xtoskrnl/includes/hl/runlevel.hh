/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/runlevel.hh
 * DESCRIPTION:     Run Level management support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_RUNLEVEL_HH
#define __XTOSKRNL_HL_RUNLEVEL_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class RunLevel
    {
        public:
            STATIC XTFASTCALL KRUNLEVEL GetRunLevel(VOID);
            STATIC XTFASTCALL VOID SetRunLevel(IN KRUNLEVEL RunLevel);

        private:
            STATIC XTFASTCALL KRUNLEVEL TransformApicTprToRunLevel(IN UCHAR Tpr);
            STATIC XTFASTCALL UCHAR TransformRunLevelToApicTpr(IN KRUNLEVEL RunLevel);
    };
}

#endif /* __XTOSKRNL_HL_RUNLEVEL_HH */
