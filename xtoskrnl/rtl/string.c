/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/string.c
 * DESCRIPTION:     String support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Compares at most specified number of characters of two C strings.
 *
 * @param String1
 *        String to be compared.
 *
 * @param String2
 *        String to be compared.
 *
 * @param Length
 *        Maximum number of characters to compare. If no limit set, it compares whole strings.
 *
 * @return Integral value indicating the relationship between the strings.
 *
 * @since XT 1.0
 */
XTCDECL
SIZE_T
RtlCompareString(IN CONST PCHAR String1,
                 IN CONST PCHAR String2,
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
 * Compares at most specified number of characters of two C strings, while ignoring differences in case.
 *
 * @param String1
 *        String to be compared.
 *
 * @param String2
 *        String to be compared.
 *
 * @param Length
 *        Maximum number of characters to compare. If no limit set, it compares whole strings.
 *
 * @return Integral value indicating the relationship between the strings.
 *
 * @since XT 1.0
 */
XTCDECL
SIZE_T
RtlCompareStringInsensitive(IN CONST PCHAR String1,
                            IN CONST PCHAR String2,
                            IN SIZE_T Length)
{
    CHAR Character1;
    CHAR Character2;
    ULONG Index = 0;

    /* Iterate through the strings */
    while(String1[Index] != '\0' && String2[Index] != '\0')
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

        /* Lowercase string1 character if needed */
        if(String1[Index] >= 'A' && String1[Index] <= 'Z')
        {
            Character1 = String1[Index] - 'A' + 'a';
        }

        /* Lowercase string2 character if needed */
        if(String2[Index] >= 'A' && String2[Index] <= 'Z')
        {
            Character2 = String2[Index] - 'A' + 'a';
        }

        /* Compare the characters */
        if(Character1 != Character2)
        {
            /* Strings are not equal */
            return Character1 > Character2 ? 1 : -1;
        }

        /* Get next character */
        Index++;
    }

    /* Strings are equal */
    return 0;
}

/**
 * Calculates the length of a given string.
 *
 * @param String
 *        Pointer to the null-terminated string to be examined.
 *
 * @param MaxLength
 *        Maximum number of characters to examine. If no limit set, it examines whole string.
 *
 * @return The length of the null-terminated string.
 *
 * @since: XT 1.0
 */
XTCDECL
SIZE_T
RtlStringLength(IN CONST PCHAR String,
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

    /* Return string length */
    return Length;
}

/**
 * Converts a multibyte character string to its wide character representation.
 *
 * @param Destination
 *        Pointer to wide character array where the wide string will be stored
 *
 * @param Source
 *        Pointer to the first element of a multibyte string to convert.
 *
 * @param Length
 *        Number of characters in the source string.
 *
 * @return Returns the number of wide characters written to the destination array on success, or -1 on error.
 *
 * @since XT 1.0
 */
XTCDECL
SIZE_T
RtlStringToWideString(OUT PWCHAR Destination,
                      IN CONST PCHAR *Source,
                      IN SIZE_T Length)
{
    PCHAR LocalSource = *Source;
    SIZE_T Count = Length;

    if(Destination == NULL)
    {
        return 0;
    }

    while(Count)
    {
        if((*Destination = *LocalSource) == 0)
        {
            LocalSource = NULL;
            break;
        }
        if(*Destination >= 0x80)
        {
            return -1;
        }
        LocalSource++;
        Destination++;
        Count--;
    }

    return Length - Count;
}
