/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/nls.cc
 * DESCRIPTION:     National Language Support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Retrieves the default ANSI and OEM code page identifiers for the system.
 *
 * @param AnsiCodePage
 *        Receives the default ANSI code page identifier.
 *
 * @param OemCodePage
 *        Receives the default OEM code page identifier.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::Nls::GetDefaultCodePage(OUT PUSHORT AnsiCodePage,
                             OUT PUSHORT OemCodePage)
{
    /* No NLS support */
    UNIMPLEMENTED;

    /* Set fallback values to zero */
    *AnsiCodePage = 0;
    *OemCodePage = 0;
}

/**
 * Initializes a code page table information structure from a raw table base pointer.
 *
 * @param TableBase
 *        Supplies a pointer to the raw code page data.
 *
 * @param CodePageTable
 *        Receives the initialized code page table information structure.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::Nls::InitializeCodePageTable(IN PUSHORT TableBase,
                                  OUT PCPTABLE_INFO CodePageTable)
{
    /* No NLS support */
    UNIMPLEMENTED;
}

/**
 * Initializes the global NLS table information structure using raw table bases.
 *
 * @param AnsiTableBase
 *        Supplies a pointer to the raw ANSI translation table data.
 *
 * @param OemTableBase
 *        Supplies a pointer to the raw OEM translation table data.
 *
 * @param CaseTableBase
 *        Supplies a pointer to the raw Unicode uppercase/lowercase mapping table data.
 *
 * @param NlsTable
 *        Receives the fully initialized NLS table structure.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::Nls::InitializeNlsTables(IN PUSHORT AnsiTableBase,
                              IN PUSHORT OemTableBase,
                              IN PUSHORT CaseTableBase,
                              OUT PNLSTABLE_INFO NlsTable)
{
    /* No NLS support */
    UNIMPLEMENTED;
}

/**
 * Converts a wide character to its lowercase equivalent using Unicode rules.
 *
 * @param WideCharacter
 *        Supplies the Unicode wide character to convert.
 *
 * @return This routine returns the lowercase equivalent of the provided wide character.
 *
 * @since XT 1.0
 */
XTAPI
WCHAR
RTL::Nls::ToLowerUnicodeCharacter(IN WCHAR WideCharacter)
{
    /* This is temporary wrapper with no NLS support */
    UNIMPLEMENTED;

    /* Fall back to the internal wide string character transformation engine */
    return RTL::WideString::ToLowerWideCharacter(WideCharacter);
}

/**
 * Converts a wide character to its uppercase equivalent using Unicode rules.
 *
 * @param WideCharacter
 *        Supplies the Unicode wide character to convert.
 *
 * @return This routine returns the uppercase equivalent of the provided wide character.
 *
 * @since XT 1.0
 */
XTAPI
WCHAR
RTL::Nls::ToUpperUnicodeCharacter(IN WCHAR WideCharacter)
{
    /* This is temporary wrapper with no NLS support */
    UNIMPLEMENTED;

    /* Fallback to the internal wide string character transformation engine */
    return RTL::WideString::ToUpperWideCharacter(WideCharacter);
}
