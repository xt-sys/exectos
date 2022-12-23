/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/string.c
 * DESCRIPTION:     String support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include "xtkmapi.h"


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
RtlStringLength(IN CONST PUCHAR String,
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
INT
RtlStringToWideString(OUT PWCHAR Destination,
                      IN CONST PUCHAR *Source,
                      IN SIZE_T Length)
{
    PUCHAR LocalSource = *Source;
    SIZE_T Count = Length;

    if(Destination == NULL)
    {
        return 0;
    }

    while(Count)
    {
        if((*Destination = (UCHAR)*LocalSource) == 0)
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
