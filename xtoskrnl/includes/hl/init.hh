/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/init.hh
 * DESCRIPTION:     Hardware layer initialization
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_INIT_HH
#define __XTOSKRNL_HL_INIT_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class Init
    {
        public:
            STATIC XTAPI XTSTATUS InitializeSystem(VOID);
    };
}

#endif /* __XTOSKRNL_HL_INIT_HH */
