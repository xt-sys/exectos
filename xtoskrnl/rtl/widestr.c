/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/widestr.c
 * DESCRIPTION:     Wide string support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include "xtkmapi.h"


/**
 * Compares at most specified number of characters of two C wide strings.
 *
 * @param String1
 *        Wide string to be compared.
 *
 * @param String2
 *        Wide string to be compared.
 *
 * @param Length
 *        Maximum number of characters to compare.
 *
 * @return Integral value indicating the relationship between the wide strings.
 *
 * @since XT 1.0
 */
XTCDECL
INT
RtlWideStringCompare(IN CONST PWCHAR String1,
                     IN CONST PWCHAR String2,
                     IN CONST ULONG Length)
{
    ULONG Index;

    /* Iterate through the strings */
    for(Index = 0; ; Index++) {
        /* Check if length limit reached */
        if(Index != 0 && Index == Length)
        {
            /* Skip checking next characters */
            break;
        }

        /* Check if string characters are equal */
        if(String1[Index] != String2[Index])
        {
            /* Different characters found */
            return String1[Index] < String2[Index] ? -1 : 1;
        }

        /* Check if end of string reached */
        if(!String1[Index] || !String2[Index])
        {
            /* Equal strings until the end of one of them */
            return 0;
        }
    }

    /* Strings are equal */
    return 0;
}

/**
 * Finds the next token in a null-terminated wide string.
 *
 * @param String
 *        Pointer to the null-terminated wide string to tokenize.
 *
 * @param Delimiter
 *        Pointer to the null-terminated wide string identifying delimiters.
 *
 * @param SavePtr
 *        Pointer to an object used to store routine internal state.
 *
 * @return Pointer to the beginning of the next token or NULL if there are no more tokens.
 *
 * @since: XT 1.0
 */
XTCDECL
PWCHAR
RtlWideStringTokenize(IN PWCHAR String,
                      IN CONST PWCHAR Delimiter,
                      IN OUT PWCHAR *SavePtr)
{
    PWCHAR Span, Token;
    WCHAR Char, SpanChar;

    /* Check if there is anything to tokenize */
    if(String == NULL && (String = *SavePtr) == NULL)
    {
        /* Empty string given */
        return NULL;
    }

    /* Check non-delimiter characters */
    Char = *String++;
    if(Char == L'\0')
    {
        *SavePtr = NULL;
        return NULL;
    }
    Token = String - 1;

    /* Scan token for delimiters */
    for(;;)
    {
        Char = *String++;
        Span = (WCHAR *)Delimiter;
        do
        {
            if((SpanChar = *Span++) == Char)
            {
                if(Char == L'\0')
                {
                    String = NULL;
                }
                else
                {
                    String[-1] = L'\0';
                }

                /* Store pointer to the next token */
                *SavePtr = String;

                /* Return token */
                return Token;
            }
        }
        while(SpanChar != L'\0');
    }
}
