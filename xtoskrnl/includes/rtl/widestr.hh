/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/widestr.hh
 * DESCRIPTION:     Wide string support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_WIDESTR_HH
#define __XTOSKRNL_RTL_WIDESTR_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class WideString
    {
        public:
            STATIC XTAPI SIZE_T CompareWideString(IN PCWSTR String1,
                                                  IN PCWSTR String2,
                                                  IN SIZE_T Length);
            STATIC XTAPI SIZE_T CompareWideStringInsensitive(IN PCWSTR String1,
                                                             IN PCWSTR String2,
                                                             IN SIZE_T Length);
            STATIC XTAPI PWCHAR ConcatenateWideString(OUT PWCHAR Destination,
                                                      IN PWCHAR Source,
                                                      IN SIZE_T Count);
            STATIC XTAPI VOID CopyWideString(IN PWCHAR Destination,
                                             IN PCWSTR Source,
                                             IN ULONG Length);
            STATIC XTAPI PCWSTR FindWideString(IN PCWSTR Source,
                                               IN PCWSTR Search);
            STATIC XTAPI PCWSTR FindWideStringInsensitive(IN PCWSTR Source,
                                                          IN PCWSTR Search);
            STATIC XTAPI XTSTATUS FormatWideString(IN PRTL_PRINT_CONTEXT Context,
                                                   IN PCWSTR Format,
                                                   IN VA_LIST ArgumentList);
            STATIC XTAPI VOID ReverseWideString(IN OUT PWCHAR String,
                                                IN ULONG Length);
            STATIC XTAPI PWCHAR TokenizeWideString(IN PWCHAR String,
                                                   IN PCWSTR Delimiter,
                                                   IN OUT PWCHAR *SavePtr);
            STATIC XTAPI WCHAR ToLowerWideCharacter(IN WCHAR Character);
            STATIC XTAPI WCHAR ToUpperWideCharacter(IN WCHAR Character);
            STATIC XTAPI PWCHAR TrimLeftWideString(IN PWCHAR String);
            STATIC XTAPI PWCHAR TrimRightWideString(IN PWCHAR String);
            STATIC XTAPI PWCHAR TrimWideString(IN PWCHAR String);
            STATIC XTAPI SIZE_T WideStringLength(IN PCWSTR String,
                                                 IN SIZE_T MaxLength);

        private:
            STATIC XTAPI XTSTATUS FormatArgumentSpecifier(IN PRTL_PRINT_CONTEXT Context,
                                                          IN PCWSTR Format,
                                                          IN PVA_LIST ArgumentList,
                                                          IN OUT PULONG Index);
            STATIC XTAPI ULONGLONG GetArgument(IN PVA_LIST ArgumentList,
                                               IN ULONG ArgumentNumber,
                                               IN LONG ArgumentSize);
            STATIC XTAPI ULONGLONG GetSpecifierValue(IN PWCHAR *Format);
            STATIC XTAPI XTSTATUS WriteWideCharacter(IN PRTL_PRINT_CONTEXT Context,
                                                     IN WCHAR Character);
            STATIC XTCDECL XTSTATUS WriteCustomValue(IN PRTL_PRINT_CONTEXT Context,
                                                     IN PCWSTR Format,
                                                     IN ...);
            STATIC XTAPI XTSTATUS WriteDoubleValue(IN PRTL_PRINT_CONTEXT Context,
                                                   IN PRTL_PRINT_FORMAT_PROPERTIES FormatProperties,
                                                   IN DOUBLE Value);
            STATIC XTAPI XTSTATUS WriteHexDoubleValue(IN PRTL_PRINT_CONTEXT Context,
                                                      IN PRTL_PRINT_FORMAT_PROPERTIES FormatProperties,
                                                      IN DOUBLE Double);
            STATIC XTAPI XTSTATUS WriteIntegerValue(IN PRTL_PRINT_CONTEXT Context,
                                                    IN PRTL_PRINT_FORMAT_PROPERTIES FormatProperties,
                                                    IN ULONGLONG Integer);
            STATIC XTAPI XTSTATUS WriteStringValue(PRTL_PRINT_CONTEXT Context,
                                                   PRTL_PRINT_FORMAT_PROPERTIES FormatProperties,
                                                   PCSTR String,
                                                   SIZE_T StringLength);
            STATIC XTAPI XTSTATUS WriteValue(PRTL_PRINT_CONTEXT Context,
                                             PRTL_PRINT_FORMAT_PROPERTIES FormatProperties,
                                             PCWSTR String,
                                             SIZE_T StringLength);
    };
}

#endif /* __XTOSKRNL_RTL_WIDESTR_HH */
