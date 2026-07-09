/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/unicode.cc
 * DESCRIPTION:     Unicode support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Compares two Unicode strings.
 *
 * @param String1
 *        Supplies a pointer to the first UNICODE_STRING.
 *
 * @param String2
 *        Supplies a pointer to the second UNICODE_STRING.
 *
 * @param CaseInsensitive
 *        Specifies whether the comparison should be case-insensitive (TRUE) or case-sensitive (FALSE).
 *
 * @return This routine returns an integral value indicating the relationship between the wide strings.
 */
XTAPI
LONG
RTL::Unicode::CompareString(IN PCUNICODE_STRING String1,
                            IN PCUNICODE_STRING String2,
                            IN BOOLEAN CaseInsensitive)
{
    SIZE_T WideCharsToCompare;
    PWCHAR Buffer1, Buffer2;
    LONG Difference;

    /* Isolate the bounded safe region determined by the shortest string */
    WideCharsToCompare = (String1->Length < String2->Length ? String1->Length : String2->Length) >> 1;

    /* Cache the string buffer pointers */
    Buffer1 = String1->Buffer;
    Buffer2 = String2->Buffer;

    /* Determine the comparison mode */
    if(CaseInsensitive)
    {
        /* Perform a case-insensitive comparison */
        while(WideCharsToCompare--)
        {
            /* Convert both wide characters to uppercase and compute their lexicographical difference */
            Difference = (LONG)RTL::Nls::ToUpperUnicodeCharacter(*Buffer1++) -
                         (LONG)RTL::Nls::ToUpperUnicodeCharacter(*Buffer2++);
            if(Difference != 0)
            {
                /* A character mismatch found, return the computed difference */
                return Difference;
            }
        }
    }
    else
    {
        /* Perform a case-sensitive comparison */
        while(WideCharsToCompare--)
        {
            /* Compute the lexicographical difference between the wide characters */
            Difference = (LONG)(*Buffer1++) - (LONG)(*Buffer2++);
            if(Difference != 0)
            {
                /* A character mismatch found, return the computed difference */
                return Difference;
            }
        }
    }

    /* All characters match, return the relationship determined by the difference in length */
    return (LONG)String1->Length - (LONG)String2->Length;
}

/**
 * Copies a source Unicode string to a destination Unicode string.
 *
 * @param Destination
 *        Supplies a pointer to the destination string.
 *
 * @param Source
 *        Supplies an optional pointer to the source string. If this parameter is not provided,
 *        the destination string is effectively emptied.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
RTL::Unicode::CopyString(IN OUT PUNICODE_STRING Destination,
                         IN PCUNICODE_STRING Source)
{
    ULONG Length;

    /* Check if the source string pointer is valid */
    if(!Source)
    {
        /* Source is missing, clear the destination string and return */
        Destination->Length = 0;
        return;
    }

    /* Calculate the length to copy */
    Length = MIN(Source->Length, Destination->MaximumLength);
    Destination->Length = Length;

    /* Copy the source string data into the destination buffer */
    RTL::Memory::CopyMemory(Destination->Buffer, Source->Buffer, Length);

    /* Check if there is enough space left in the buffer to NULL-terminate */
    if(Destination->Length < Destination->MaximumLength)
    {
        /* Append a NULL terminator */
        Destination->Buffer[Length / sizeof(WCHAR)] = (WCHAR)0;
    }
}

/**
 * Initializes a Unicode string.
 *
 * @param Destination
 *        Supplies a pointer to the UNICODE_STRING structure to be initialized.
 *
 * @param Source
 *        Supplies an optional pointer to a NULL-terminated wide character string.
 *
 * @param Truncate
 *        Specifies whether to truncate the source string to the maximum allowed length.
 *
 * @return This routine returns a status code indicating the success or failure of the operation.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RTL::Unicode::InitializeString(OUT PUNICODE_STRING Destination,
                               IN PCWSTR Source,
                               IN BOOLEAN Truncate)
{
    ULONG Size;

    /* Check if the source string pointer is valid */
    if(!Source)
    {
        /* Source is missing, initialize the destination as empty string */
        Destination->Length = 0;
        Destination->MaximumLength = 0;

        /* Return success */
        return STATUS_SUCCESS;
    }

    /* Calculate the size of the string in bytes */
    Size = RTL::WideString::WideStringLength(Source, 0) * sizeof(WCHAR);

    /* Check if the size exceeds the maximum allowed */
    if(Size > ((MAXUSHORT & ~1) - sizeof(WCHAR)))
    {
        /* Check if truncation is not explicitly forced */
        if(!Truncate)
        {
            /* Return error code */
            return STATUS_NAME_TOO_LONG;
        }

        /* Force truncation to the maximum safe length */
        Size = (MAXUSHORT & ~1) - sizeof(WCHAR);
    }

    /* Initialize the destination string fields */
    Destination->Buffer = (PWCHAR)Source;
    Destination->Length = (USHORT)Size;
    Destination->MaximumLength = (USHORT)Size + sizeof(WCHAR);

    /* Return success */
    return STATUS_SUCCESS;
}
