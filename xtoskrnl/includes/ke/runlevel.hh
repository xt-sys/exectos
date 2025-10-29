/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/runlevel.hh
 * DESCRIPTION:     Running Level management support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_RUNLEVEL_HH
#define __XTOSKRNL_KE_RUNLEVEL_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class RunLevel
    {
        public:
            STATIC XTFASTCALL KRUNLEVEL GetCurrentRunLevel(VOID);
            STATIC XTFASTCALL VOID LowerRunLevel(IN KRUNLEVEL RunLevel);
            STATIC XTFASTCALL KRUNLEVEL RaiseRunLevel(IN KRUNLEVEL RunLevel);
    };

    class LowerRunLevel
    {
        private:
            KRUNLEVEL PreviousRunLevel;

        public:
            LowerRunLevel(KRUNLEVEL RunLevel)
            {
                PreviousRunLevel = KE::RunLevel::GetCurrentRunLevel();
                KE::RunLevel::LowerRunLevel(RunLevel);
            }

            ~LowerRunLevel()
            {
                KE::RunLevel::RaiseRunLevel(PreviousRunLevel);
            }

            LowerRunLevel(const LowerRunLevel&) = delete;
            LowerRunLevel& operator=(const LowerRunLevel&) = delete;
    };

    class RaiseRunLevel
    {
        private:
            KRUNLEVEL PreviousRunLevel;

        public:
            RaiseRunLevel(KRUNLEVEL RunLevel)
            {
                PreviousRunLevel = KE::RunLevel::GetCurrentRunLevel();
                KE::RunLevel::RaiseRunLevel(RunLevel);
            }

            ~RaiseRunLevel()
            {
                KE::RunLevel::LowerRunLevel(PreviousRunLevel);
            }

            RaiseRunLevel(const RaiseRunLevel&) = delete;
            RaiseRunLevel& operator=(const RaiseRunLevel&) = delete;
    };
}

#endif /* __XTOSKRNL_KE_RUNLEVEL_HH */
