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
            BOOLEAN Switched;

        public:
            LowerRunLevel(IN KRUNLEVEL RunLevel,
                          IN BOOLEAN ChangeLevel = TRUE)
            {
                PreviousRunLevel = KE::RunLevel::GetCurrentRunLevel();
                Switched = ChangeLevel;
                if(Switched)
                {
                    KE::RunLevel::LowerRunLevel(RunLevel);
                }
            }

            ~LowerRunLevel()
            {
                if(Switched)
                {
                    KE::RunLevel::RaiseRunLevel(PreviousRunLevel);
                }
            }

            LowerRunLevel(const LowerRunLevel&) = delete;
            LowerRunLevel& operator=(const LowerRunLevel&) = delete;
    };

    class RaiseRunLevel
    {
        private:
            KRUNLEVEL PreviousRunLevel;
            BOOLEAN Switched;

        public:
            RaiseRunLevel(IN KRUNLEVEL RunLevel,
                          IN BOOLEAN ChangeLevel = TRUE)
            {
                PreviousRunLevel = KE::RunLevel::GetCurrentRunLevel();
                Switched = ChangeLevel;
                if(Switched)
                {
                    KE::RunLevel::RaiseRunLevel(RunLevel);
                }
            }

            ~RaiseRunLevel()
            {
                if(Switched)
                {
                    KE::RunLevel::LowerRunLevel(PreviousRunLevel);
                }
            }

            RaiseRunLevel(const RaiseRunLevel&) = delete;
            RaiseRunLevel& operator=(const RaiseRunLevel&) = delete;
    };
}

#endif /* __XTOSKRNL_KE_RUNLEVEL_HH */
