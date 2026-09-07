/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/nls.hh
 * DESCRIPTION:     National Language Support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_NLS_HH
#define __XTOSKRNL_RTL_NLS_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class Nls
    {
        public:
            STATIC XTAPI VOID GetDefaultCodePage(OUT PUSHORT AnsiCodePage,
                                                 OUT PUSHORT OemCodePage);
            STATIC XTAPI VOID InitializeCodePageTable(IN PUSHORT TableBase,
                                                      OUT PCPTABLE_INFO CodePageTable);
            STATIC XTAPI VOID InitializeNlsTables(IN PUSHORT AnsiTableBase,
                                                  IN PUSHORT OemTableBase,
                                                  IN PUSHORT CaseTableBase,
                                                  OUT PNLSTABLE_INFO NlsTable);
            STATIC XTAPI WCHAR ToLowerUnicodeCharacter(IN WCHAR WideCHaracter);
            STATIC XTAPI WCHAR ToUpperUnicodeCharacter(IN WCHAR WideCharacter);
    };
}

#endif /* __XTOSKRNL_RTL_NLS_HH */
