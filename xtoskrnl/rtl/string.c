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
XTAPI
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
XTAPI
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
 * Appends a copy of the source string to the end of the destination string.
 *
 * @param Destination
 *        Supplies a pointer to the null-terminated string to append to.
 *
 * @param Source
 *        Supplies a pointer to the null-terminated string to copy from.
 *
 * @param Count
 *        Sets a maximum number of characters to copy. If no limit set, appends whole string.
 *
 * @return This routine returns a copy of a destination string.
 *
 * @since XT 1.0
 */
XTAPI
PCHAR
RtlConcatenateString(OUT PCHAR Destination,
                     IN PCHAR Source,
                     IN SIZE_T Count)
{
    PCHAR DestString = Destination;

    /* Go to the end of destination string */
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

        /* Add NULL termination character to the end of destination string */
        *Destination = '\0';
    }
    else
    {
        /* No limit set, copy all characters */
        while((*Destination++ = *Source++) != 0);
    }

    /* Return copy of the destination string */
    return DestString;
}

/**
 * Copies a string from a buffer into another buffer, ensuring that the destination string is NULL-terminated.
 *
 * @param Destination
 *        Supplies a pointer to the destination buffer.
 *
 * @param Source
 *        Supplies a pointer to the source buffer.
 *
 * @param Length
 *        Supplies the length of the string to copy.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RtlCopyString(IN PCHAR Destination,
              IN PCCHAR Source,
              IN ULONG Length)
{
    ULONG Index;

    /* Copy characters */
    for(Index = 0; Index < Length; Index++)
    {
        /* Copy source character */
        Destination[Index] = Source[Index];

        /* Check if NULL terminated character found */
        if(Source[Index] == '\0')
        {
            /* End of source string reached */
            break;
        }
    }

    /* Make sure the destination string is terminated properly */
    Destination[Index] = '\0';
}

/**
 * Reverses a characters order in a string. It modifies the original, input variable.
 *
 * @param String
 *        Supplies a pointer to the string to reverse.
 *
 * @param Length
 *        Supplies the length of the string to reverse.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RtlReverseString(IN OUT PCHAR String,
                 IN ULONG Length)
{
    UCHAR TempChar;
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
XTAPI
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
XTAPI
SIZE_T
RtlStringToWideString(OUT PWCHAR Destination,
                      IN CONST PCHAR *Source,
                      IN SIZE_T Length)
{
    PCHAR LocalSource = *Source;
    SIZE_T Count = Length;

    /* Check if NULL pointer passed */
    if(Destination == NULL)
    {
        /* No wide characters written */
        return 0;
    }

    /* Iterate through the string */
    while(Count)
    {
        /* Copy character */
        if((*Destination = *LocalSource) == 0)
        {
            /* End of string reached */
            LocalSource = NULL;
            break;
        }

        /* Check if character is valid */
        if(*Destination >= 0x80)
        {
            /* Invalid character, return error */
            return -1;
        }

        /* Advance pointers */
        LocalSource++;
        Destination++;
        Count--;
    }

    /* Return number of wide characters written */
    return Length - Count;
}

/**
 * Finds the next token in a null-terminated string.
 *
 * @param String
 *        Pointer to the null-terminated string to tokenize.
 *
 * @param Delimiter
 *        Pointer to the null-terminated string identifying delimiters.
 *
 * @param SavePtr
 *        Pointer to an object used to store routine internal state.
 *
 * @return Pointer to the beginning of the next token or NULL if there are no more tokens.
 *
 * @since: XT 1.0
 */
XTAPI
PCHAR
RtlTokenizeString(IN PCHAR String,
                  IN CONST PCHAR Delimiter,
                  IN OUT PCHAR *SavePtr)
{
    PCHAR Span, Token;
    CHAR Char, SpanChar;

    /* Check if there is anything to tokenize */
    if(String == NULL && (String = *SavePtr) == NULL)
    {
        /* Empty string given */
        return NULL;
    }

    /* Check non-delimiter characters */
    Char = *String++;
    if(Char == '\0')
    {
        *SavePtr = NULL;
        return NULL;
    }
    Token = String - 1;

    /* Scan token for delimiters */
    for(;;)
    {
        Char = *String++;
        Span = (PCHAR)Delimiter;
        do
        {
            /* Check if delimiter found */
            if((SpanChar = *Span++) == Char)
            {
                /* Check if end of string reached */
                if(Char == '\0')
                {
                    /* End of string reached, no more tokens */
                    String = NULL;
                }
                else
                {
                    /* Terminate token */
                    String[-1] = '\0';
                }

                /* Store pointer to the next token */
                *SavePtr = String;

                /* Return token */
                return Token;
            }
        }
        while(SpanChar != '\0');
    }
}

/**
 * Removes certain characters from a beginning of the string.
 *
 * @param String
 *        Pointer to the null-terminated string to be trimmed.
 *
 * @return This routine returns a pointer to the left-trimmed string.
 *
 * @since XT 1.0
 */
XTAPI
PCHAR
RtlTrimLeftString(IN CONST PCHAR String)
{
    PCHAR Start;

    /* Initialize pointer */
    Start = String;

    /* Skip all leading whitespaces */
    while(*Start == ' ' || *Start == '\n' || *Start == '\t' || *Start == '\r' || *Start == '\v' || *Start == '\f')
    {
        /* Advance to the next character */
        Start++;
    }

    /* Return left-trimmed string */
    return Start;
}

/**
 * Removes certain characters from the end of the string.
 *
 * @param String
 *        Pointer to the null-terminated string to be trimmed.
 *
 * @return This routine returns a pointer to the right-trimmed string.
 *
 * @since XT 1.0
 */
XTAPI
PCHAR
RtlTrimRightString(IN CONST PCHAR String)
{
    PCHAR End;

    /* Find end of the string */
    End = String + RtlStringLength(String, 0);

    /* Skip all trailing whitespaces */
    while((End != String) && (*End == ' ' || *End == '\n' || *End == '\t' || *End == '\r' || *End == '\v' || *End == '\f'))
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
 * Removes certain characters from the beginning and the end of the string.
 *
 * @param String
 *        Pointer to the null-terminated string to be trimmed.
 *
 * @return This routine returns a pointer to the trimmed string.
 *
 * @since XT 1.0
 */
XTAPI
PCHAR
RtlTrimString(IN CONST PCHAR String)
{
    return RtlTrimLeftString(RtlTrimRightString(String));
}
