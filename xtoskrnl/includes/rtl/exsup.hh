/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/exsup.hh
 * DESCRIPTION:     Exception handling
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_EXSUP_HH
#define __XTOSKRNL_RTL_EXSUP_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class Exception
    {
        public:
            STATIC XTAPI VOID RaiseException(IN PEXCEPTION_RECORD ExceptionRecord);
            STATIC XTAPI VOID RaiseStatus(IN XTSTATUS Status);
            STATIC XTAPI LONG SystemFilter(VOID);
    };
}

#endif /* __XTOSKRNL_RTL_EXSUP_HH */
