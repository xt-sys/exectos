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
 *        Maximum number of characters to compare. If no limit set, it compares whole strings.
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
 * Appends a copy of the source wide string to the end of the destination wide string.
 *
 * @param Destination
 *        Supplies a pointer to the null-terminated wide string to append to.
 *
 * @param Source
 *        Supplies a pointer to the null-terminated wide string to copy from.
 *
 * @param Count
 *        Sets a maximum number of wide characters to copy. If no limit set, appends whole wide string.
 *
 * @return This routine returns a copy of a destination wide string.
 *
 * @since XT 1.0
 */
XTCDECL
PWCHAR
RtlWideStringConcatenate(PWCHAR Destination,
                         PWCHAR Source,
                         SIZE_T Count)
{
    PWCHAR DestString = Destination;

    /* Go to the end of destination wide string */
    while(*Destination)
    {
        Destination++;
    }

    /* Check if copy limit set */
    if(Count > 0)
    {
        /* Copy character-by-character */
        do
        {
            /* Check if NULL terminated character found */
            if((*Destination = *Source++) == '\0')
            {
                /* Break on '\0' character */
                break;
            }
            Destination++;
        }
        while(--Count != 0);

        /* Add NULL termination character to the end of destination wide string */
        *Destination = '\0';
    }
    else
    {
        /* No limit set, copy all wide characters */
        while((*Destination++ = *Source++) != 0);
    }

    /* Return copy of the destination wide string */
    return DestString;
}

/**
 * Calculates the length of a given wide string.
 *
 * @param String
 *        Pointer to the null-terminated wide string to be examined.
 *
 * @param MaxLength
 *        Maximum number of wide characters to examine. If no limit set, it examines whole string.
 *
 * @return The length of the null-terminated wide string.
 *
 * @since: XT 1.0
 */
XTCDECL
SIZE_T
RtlWideStringLength(IN CONST PWCHAR String,
                    IN SIZE_T MaxLength)
{
    SIZE_T Length;

    /* Check if NULL pointer passed */
    if(String == NULL)
    {
        return 0;
    }

    /* Iterate through the wide string */
    for(Length = 0; ; Length++)
    {

        /* Check if NULL found or max length limit reached */
        if((Length != 0 && Length == MaxLength) || !String[Length])
        {
            /* Finish examination */
            break;
        }
    }

    /* Return wide string length */
    return Length;
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
