/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/widestr.c
 * DESCRIPTION:     Wide string support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


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
 *        Maximum number of characters to compare. If no limit set, it compares whole wide strings.
 *
 * @return Integral value indicating the relationship between the wide strings.
 *
 * @since XT 1.0
 */
XTCDECL
SIZE_T
RtlCompareWideString(IN CONST PWCHAR String1,
                     IN CONST PWCHAR String2,
                     IN SIZE_T Length)
{
    SIZE_T Index;

    /* Iterate through the strings */
    for(Index = 0; ; Index++) {
        /* Check if length limit reached */
        if(Index != 0 && Index == Length)
        {
            /* Skip checking next characters */
            break;
        }

        /* Check if wide string characters are equal */
        if(String1[Index] != String2[Index])
        {
            /* Different characters found */
            return String1[Index] < String2[Index] ? -1 : 1;
        }

        /* Check if end of wide string reached */
        if(!String1[Index] || !String2[Index])
        {
            /* Equal wide strings until the end of one of them */
            return 0;
        }
    }

    /* Wide strings are equal */
    return 0;
}

/**
 * Compares at most specified number of characters of two C wide strings, while ignoring differences in case.
 *
 * @param String1
 *        Wide string to be compared.
 *
 * @param String2
 *        Wide string to be compared.
 *
 * @param Length
 *        Maximum number of characters to compare. If no limit set, it compares whole wide strings.
 *
 * @return Integral value indicating the relationship between the wide strings.
 *
 * @since XT 1.0
 */
XTCDECL
SIZE_T
RtlCompareWideStringInsensitive(IN CONST PWCHAR String1,
                                IN CONST PWCHAR String2,
                                IN SIZE_T Length)
{
    WCHAR Character1;
    WCHAR Character2;
    ULONG Index = 0;

    /* Iterate through the wide strings */
    while(String1[Index] != L'\0' && String2[Index] != L'\0')
    {
        /* Check if length limit reached */
        if(Index != 0 && Index == Length)
        {
            /* Skip checking next characters */
            break;
        }

        /* Get the characters */
        Character1 = String1[Index];
        Character2 = String2[Index];

        /* Lowercase wide string1 character if needed */
        if(String1[Index] >= L'A' && String1[Index] <= L'Z')
        {
            Character1 = String1[Index] - L'A' + L'a';
        }

        /* Lowercase wide string2 character if needed */
        if(String2[Index] >= L'A' && String2[Index] <= L'Z')
        {
            Character2 = String2[Index] - L'A' + L'a';
        }

        /* Compare the characters */
        if(Character1 != Character2)
        {
            /* Wide strings are not equal */
            return Character1 > Character2 ? 1 : -1;
        }

        /* Get next character */
        Index++;
    }

    /* Wide strings are equal */
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
RtlConcatenateWideString(OUT PWCHAR Destination,
                         IN PWCHAR Source,
                         IN SIZE_T Count)
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
            if((*Destination = *Source++) == L'\0')
            {
                /* Break on '\0' character */
                break;
            }
            Destination++;
        }
        while(--Count != 0);

        /* Add NULL termination character to the end of destination wide string */
        *Destination = L'\0';
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
 * Reverses a characters order in a wide string. It modifies the original, input variable.
 *
 * @param String
 *        Supplies a pointer to the wide string to reverse.
 *
 * @param Length
 *        Supplies the length of the wide string to reverse.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RtlReverseWideString(IN OUT PWCHAR String,
                     IN ULONG Length)
{
    WCHAR TempChar;
    ULONG Index;

    /* Iterate through the string */
    for(Index = 0; Index < (Length / 2); Index++)
    {
        /* Swap characters */
        TempChar = String[Index];
        String[Index] = String[Length - Index - 1];
        String[Length - Index - 1] = TempChar;
    }
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
RtlTokenizeWideString(IN PWCHAR String,
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
        Span = (PWCHAR)Delimiter;
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

/**
 * Removes certain characters from a beginning of the wide string.
 *
 * @param String
 *        Pointer to the null-terminated wide string to be trimmed.
 *
 * @return This routine returns a pointer to the left-trimmed wide string.
 *
 * @since XT 1.0
 */
XTAPI
PWCHAR
RtlTrimLeftWideString(IN CONST PWCHAR String)
{
    PWCHAR Start;

    /* Initialize pointer */
    Start = String;

    /* Skip all leading whitespaces */
    while(*Start == L' ' || *Start == L'\n' || *Start == L'\t' || *Start == L'\r' || *Start == L'\v' || *Start == L'\f')
    {
        /* Advance to the next character */
        Start++;
    }

    /* Return left-trimmed string */
    return Start;
}

/**
 * Removes certain characters from the end of the wide string.
 *
 * @param String
 *        Pointer to the null-terminated wide string to be trimmed.
 *
 * @return This routine returns a pointer to the right-trimmed wide string.
 *
 * @since XT 1.0
 */
XTAPI
PWCHAR
RtlTrimRightWideString(IN CONST PWCHAR String)
{
    PWCHAR End;

    /* Find end of the string */
    End = String + RtlWideStringLength(String, 0);

    /* Skip all trailing whitespaces */
    while((End != String) && (*End == L' ' || *End == L'\n' || *End == L'\t' || *End == L'\r' || *End == L'\v' || *End == L'\f'))
    {
        /* Move to the previous character */
        End--;
    }

    /* Terminate the string */
    *End = 0;

    /* Return right-trimmed string */
    return String;
}

/**
 * Removes certain characters from the beginning and the end of the wide string.
 *
 * @param String
 *        Pointer to the null-terminated wide string to be trimmed.
 *
 * @return This routine returns a pointer to the trimmed wide string.
 *
 * @since XT 1.0
 */
XTAPI
PWCHAR
RtlTrimWideString(IN CONST PWCHAR String)
{
    return RtlTrimLeftWideString(RtlTrimRightWideString(String));
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
