/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/unicode.hh
 * DESCRIPTION:     Unicode support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_UNICODE_HH
#define __XTOSKRNL_RTL_UNICODE_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class Unicode
    {
        public:
            STATIC XTAPI LONG CompareString(IN PCUNICODE_STRING String1,
                                            IN PCUNICODE_STRING String2,
                                            IN BOOLEAN CaseInsensitive);
            STATIC XTAPI VOID CopyString(IN OUT PUNICODE_STRING Destination,
                                         IN PCUNICODE_STRING Source);
            STATIC XTAPI XTSTATUS InitializeString(OUT PUNICODE_STRING Destination,
                                                   IN PCWSTR Source,
                                                   IN BOOLEAN Truncate = FALSE);
    };
}

#endif /* __XTOSKRNL_RTL_UNICODE_HH */
