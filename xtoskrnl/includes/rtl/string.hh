/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/string.hh
 * DESCRIPTION:     String support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_STRING_HH
#define __XTOSKRNL_RTL_STRING_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class String
    {
        public:
            STATIC XTAPI SIZE_T CompareString(IN PCSTR String1,
                                              IN PCSTR String2,
                                              IN SIZE_T Length);
            STATIC XTAPI SIZE_T CompareStringInsensitive(IN PCSTR String1,
                                                         IN PCSTR String2,
                                                         IN SIZE_T Length);
            STATIC XTAPI PCHAR ConcatenateString(OUT PCHAR Destination,
                                                 IN PCHAR Source,
                                                 IN SIZE_T Count);
            STATIC XTAPI VOID CopyString(IN PCHAR Destination,
                                         IN PCSTR Source,
                                         IN ULONG Length);
            STATIC XTAPI PCSTR FindString(IN PCSTR Source,
                                          IN PCSTR Search);
            STATIC XTAPI PCSTR FindStringInsensitive(IN PCSTR Source,
                                                     IN PCSTR Search);
            STATIC XTAPI VOID ReverseString(IN OUT PCHAR String,
                                            IN ULONG Length);
            STATIC XTAPI SIZE_T StringLength(IN PCSTR String,
                                             IN SIZE_T MaxLength);
            STATIC XTAPI SIZE_T StringToWideString(OUT PWCHAR Destination,
                                                   IN PCSTR *Source,
                                                   IN SIZE_T Length);
            STATIC XTAPI PCHAR TokenizeString(IN PCHAR String,
                                              IN PCSTR Delimiter,
                                              IN OUT PCHAR *SavePtr);
            STATIC XTAPI CHAR ToLowerCharacter(IN CHAR Character);
            STATIC XTAPI CHAR ToUpperCharacter(IN CHAR Character);
            STATIC XTAPI PCHAR TrimLeftString(IN PCHAR String);
            STATIC XTAPI PCHAR TrimRightString(IN PCHAR String);
            STATIC XTAPI PCHAR TrimString(IN PCHAR String);
    };
}

#endif /* __XTOSKRNL_RTL_STRING_HH */
