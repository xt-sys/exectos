/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/bootutil.c
 * DESCRIPTION:     Helper functions used by the boot protocol during system startup
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtldr.h>


/**
 * Checks if a specific option exists in the list of provided boot parameters.
 *
 * @param Parameters
 *        A pointer to the wide-character string containing the boot parameters, separated by spaces.
 *
 * @param Needle
 *        A pointer to the wide-character string representing the kernel option to find.
 *
 * @return This routine returns TRUE if the option is found, otherwise FALSE.
 *
 * @since XT 1.0
 */
XTCDECL
BOOLEAN
BlGetBooleanParameter(IN CONST PWCHAR Parameters,
                      IN CONST PWCHAR Needle)
{
    PWCHAR CurrentPosition, TokenEnd, TokenStart;
    SIZE_T NeedleLength, TokenLength;

    /* Validate input data and ensure the option is not an empty string */
    if(Parameters == NULL || Needle == NULL || *Needle == L'\0')
    {
        /* One of the parameters was invalid */
        return FALSE;
    }

    CurrentPosition = Parameters;
    NeedleLength = RtlWideStringLength(Needle, 0);

    /* Iterate through the entire parameters string */
    while(*CurrentPosition != L'\0')
    {
        /* Skip any leading whitespace to find the start of the token */
        while(*CurrentPosition == L' ')
        {
            CurrentPosition++;
        }

        /* Check if end of the string has been reached */
        if(*CurrentPosition == L'\0')
        {
            /* End of string reached, no more tokens */
            break;
        }

        /* Identify the boundaries of the current token */
        TokenStart = CurrentPosition;
        TokenEnd = TokenStart;
        while(*TokenEnd != L'\0' && *TokenEnd != L' ')
        {
            TokenEnd++;
        }

        /* Calculate the length of the token found */
        TokenLength = TokenEnd - TokenStart;

         /* Compare the token length */
        if(TokenLength == NeedleLength)
        {
            /* Length matches, compare the strings */
            if(RtlCompareWideStringInsensitive(TokenStart, Needle, NeedleLength) == 0)
            {
                /* A match was found */
                return TRUE;
            }
        }

        /* Move the position past the current token to continue the search */
        CurrentPosition = TokenEnd;
    }

    /* No match was found */
    return FALSE;
}
