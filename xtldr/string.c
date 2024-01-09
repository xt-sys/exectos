/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/string.c
 * DESCRIPTION:     EFI string manipulation support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtldr.h>


/**
 * This routine formats the input string and prints it using specified routine.
 *
 * @param PrintCharRoutine
 *        Pointer to the routine that writes an input data to specific device.
 *
 * @param Format
 *        The formatted string that is to be written to the specified device.
 *
 * @param Arguments
 *        A value identifying a variable arguments list initialized with VA_START.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpStringPrint(IN IN BLPRINTCHAR PrintCharRoutine,
               IN PUINT16 Format,
               IN VA_LIST Arguments)
{
    PEFI_GUID Guid;
    PUCHAR String;
    PWCHAR WideString;
    ULONG PaddingCount;

    /* Read the variable arguments */
    for(; *Format; ++Format)
    {
        switch(*Format)
        {
            case L'%':
                switch(*++Format)
                {
                    case L'b':
                        /* Boolean */
                        BlpStringFormat(PrintCharRoutine, L"%s", VA_ARG(Arguments, INT32) ? "TRUE" : "FALSE");
                        break;
                    case L'c':
                        /* Character */
                        PrintCharRoutine(VA_ARG(Arguments, INT));
                        break;
                    case L'd':
                        /* Signed 32-bit integer */
                        BlpStringPrintSigned32(PrintCharRoutine, VA_ARG(Arguments, INT32), 10);
                        break;
                    case L'g':
                        /* EFI GUID */
                        Guid = VA_ARG(Arguments, PEFI_GUID);
                        BlpStringFormat(PrintCharRoutine, L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", Guid->Data1,
                                        Guid->Data2, Guid->Data3, Guid->Data4[0], Guid->Data4[1], Guid->Data4[2],
                                        Guid->Data4[3], Guid->Data4[4], Guid->Data4[5], Guid->Data4[6], Guid->Data4[7]);
                        break;
                    case L'l':
                        /* 64-bit numbers */
                        switch(*++Format)
                        {
                            case L'd':
                                /* Signed 64-bit integer */
                                BlpStringPrintSigned64(PrintCharRoutine, VA_ARG(Arguments, INT_PTR), 10);
                                break;
                            case L'u':
                                /* Unsigned 64-bit integer */
                                BlpStringPrintUnsigned64(PrintCharRoutine, VA_ARG(Arguments, UINT_PTR), 10, 0);
                                break;
                            case L'x':
                                /* Unsigned 64-bit hexadecimal integer */
                                BlpStringPrintUnsigned64(PrintCharRoutine, VA_ARG(Arguments, UINT_PTR), 16, 0);
                                break;
                            default:
                                /* Unknown by default */
                                PrintCharRoutine(L'?');
                                break;
                        }
                        break;
                    case L'p':
                        /* Pointer address */
                        BlpStringPrintUnsigned64(PrintCharRoutine, VA_ARG(Arguments, UINT_PTR), 16, 0);
                        break;
                    case L's':
                        /* String of characters */
                        String = VA_ARG(Arguments, PUCHAR);
                        while(*String)
                        {
                            PrintCharRoutine(*String++);
                        }
                        break;
                    case L'S':
                        WideString = VA_ARG(Arguments, PWCHAR);
                        while(*WideString)
                        {
                            PrintCharRoutine((UCHAR)*WideString++);
                        }
                        break;
                    case L'u':
                        /* Unsigned 32-bit integer */
                        BlpStringPrintUnsigned32(PrintCharRoutine, VA_ARG(Arguments, UINT32), 10, 0);
                        break;
                    case L'x':
                        /* Unsigned 32-bit hexadecimal integer */
                        BlpStringPrintUnsigned32(PrintCharRoutine, VA_ARG(Arguments, UINT32), 16, 0);
                        break;
                    case L'0':
                        /* Zero padded numbers */
                        ++Format;
                        PaddingCount = BlpStringReadPadding(&Format);
                        switch(*Format)
                        {
                            case L'd':
                                /* Zero-padded, signed 32-bit integer */
                                BlpStringPrintSigned32(PrintCharRoutine, VA_ARG(Arguments, INT32), 10);
                                break;
                            case L'l':
                                /* 64-bit numbers */
                                switch(*++Format)
                                {
                                    case L'd':
                                        /* Zero-padded, signed 64-bit integer */
                                        BlpStringPrintSigned64(PrintCharRoutine, VA_ARG(Arguments, INT_PTR), 10);
                                        break;
                                    case L'u':
                                        /* Zero-padded, unsigned 64-bit integer */
                                        BlpStringPrintUnsigned64(PrintCharRoutine, VA_ARG(Arguments, UINT_PTR), 10, PaddingCount);
                                        break;
                                    case L'x':
                                        /* Zero-padded, unsigned  64-bit hexadecimal integer */
                                        BlpStringPrintUnsigned64(PrintCharRoutine, VA_ARG(Arguments, UINT_PTR), 16, PaddingCount);
                                        break;
                                    default:
                                        /* Unknown by default */
                                        PrintCharRoutine(L'?');
                                        break;
                                }
                                break;
                            case L'u':
                                /* Zero-padded, unsigned 32-bit integer */
                                BlpStringPrintUnsigned32(PrintCharRoutine, VA_ARG(Arguments, UINT32), 10, PaddingCount);
                                break;
                            case L'x':
                                /* Zero-padded, unsigned 32-bit hexadecimal integer */
                                BlpStringPrintUnsigned32(PrintCharRoutine, VA_ARG(Arguments, UINT32), 16, PaddingCount);
                                break;
                            default:
                                /* Unknown by default */
                                PrintCharRoutine(L'?');
                                break;
                        }
                        break;
                    case L'%':
                        /* Percent character */
                        PrintCharRoutine(L'%');
                        break;
                    default:
                        /* Unknown by default */
                        PrintCharRoutine(L'?');
                        break;
                }
                break;
            case L'\r':
                /* Carriage return is ignored */
                break;
            case L'\n':
                /* New line together with carriage return */
                PrintCharRoutine(L'\r');
                PrintCharRoutine(L'\n');
                break;
            default:
                /* Put character by default */
                PrintCharRoutine(*Format);
                break;
        }
    }
}

/**
 * This routine formats the input string and prints it using specified routine.
 *
 * @param PrintCharRoutine
 *        Pointer to the routine that writes an input data to specific device.
 *
 * @param Format
 *        The formatted string that is to be written to the specified device.
 *
 * @param ...
 *        Depending on the format string, this routine might expect a sequence of additional arguments.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpStringFormat(IN BLPRINTCHAR PrintCharRoutine,
                IN PUINT16 Format,
                IN ...)
{
    VA_LIST Arguments;

    /* Initialise the va_list */
    VA_START(Arguments, Format);

    /* Format and print the string to the desired output */
    BlpStringPrint(PrintCharRoutine, Format, Arguments);

    /* Clean up the va_list */
    VA_END(Arguments);
}

/**
 * This routine converts 32-bit integer as string and prints it using specified routine.
 *
 * @param PrintCharRoutine
 *        Pointer to the routine that writes an input data to specific device.
 *
 * @param Number
 *        32-bit integer value.
 *
 * @param Base
 *        Specifies the number base system representation.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpStringPrintSigned32(IN BLPRINTCHAR PrintCharRoutine,
                       IN INT Number,
                       IN UINT Base)
{
    /* Print - (minus) if this is negative value */
    if(Number < 0)
    {
        PrintCharRoutine(L'-');
        Number *= -1;
    }

    /* Print the integer value */
    BlpStringPrintUnsigned32(PrintCharRoutine, Number, Base, 0);
}

/**
 * This routine converts 64-bit integer as string and prints it using specified routine.
 *
 * @param PrintCharRoutine
 *        Pointer to the routine that writes an input data to specific device.
 *
 * @param Number
 *        64-bit integer value.
 *
 * @param Base
 *        Specifies the number base system representation.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpStringPrintSigned64(IN BLPRINTCHAR PrintCharRoutine,
                       IN INT_PTR Number,
                       IN UINT_PTR Base)
{
    /* Print - (minus) if this is negative value */
    if(Number < 0)
    {
        PrintCharRoutine(L'-');
        Number *= -1;
    }

    /* Print the integer value */
    BlpStringPrintUnsigned64(PrintCharRoutine, Number, Base, 0);
}

/**
 * This routine converts 32-bit unsigned integer as string and prints it using specified routine.
 *
 * @param PrintCharRoutine
 *        Pointer to the routine that writes an input data to specific device.
 *
 * @param Number
 *        32-bit integer value.
 *
 * @param Base
 *        Specifies the number base system representation.
 *
 * @param Padding
 *        Specifies the number of leading zeros to complete the field width.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpStringPrintUnsigned32(IN BLPRINTCHAR PrintCharRoutine,
                         IN UINT Number,
                         IN UINT Base,
                         IN UINT Padding)
{
    UINT Buffer[20];
    UINT NumberLength;
    PUINT Pointer;

    /* Set pointer to the end of buffer */
    Pointer = Buffer + ARRAY_SIZE(Buffer);

    /* Convert value to specified base system */
    *--Pointer = 0;
    do
    {
        *--Pointer = BlpHexTable[Number % Base];
    } while(Pointer >= Buffer && (Number /= Base));

    /* Calculate number length */
    NumberLength = ARRAY_SIZE(Buffer) - (Pointer - Buffer) - 1;

    /* Check if leading zeros are needed */
    if(NumberLength < Padding)
    {
        Padding -= NumberLength;
        while(Padding--)
        {
            /* Write leading zeroes */
            PrintCharRoutine(L'0');
        }
    }

    /* Print value to the console */
    for(; *Pointer; ++Pointer)
    {
        PrintCharRoutine(*Pointer);
    }
}

/**
 * This routine converts 64-bit unsigned integer as string and prints it using specified routine.
 *
 * @param PrintCharRoutine
 *        Pointer to the routine that writes an input data to specific device.
 *
 * @param Number
 *        64-bit integer value.
 *
 * @param Base
 *        Specifies the number base system representation.
 *
 * @param Padding
 *        Specifies the number of leading zeros to complete the field width.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
BlpStringPrintUnsigned64(IN BLPRINTCHAR PrintCharRoutine,
                         IN UINT_PTR Number,
                         IN UINT_PTR Base,
                         IN UINT_PTR Padding)
{
    UINT16 Buffer[20];
    UINT_PTR NumberLength;
    PUINT16 Pointer;

    /* Set pointer to the end of buffer */
    Pointer = Buffer + ARRAY_SIZE(Buffer);

    /* Convert value to specified base system */
    *--Pointer = 0;
    do
    {
        *--Pointer = BlpHexTable[Number % Base];
    } while(Pointer >= Buffer && (Number /= Base));

    /* Calculate number length */
    NumberLength = ARRAY_SIZE(Buffer) - (Pointer - Buffer) - 1;

    /* Check if leading zeros are needed */
    if(NumberLength < Padding)
    {
        Padding -= NumberLength;
        while(Padding--)
        {
            /* Write leading zeroes */
            PrintCharRoutine(L'0');
        }
    }

    /* Print value to the console */
    for(; *Pointer; ++Pointer)
    {
        PrintCharRoutine(*Pointer);
    }
}

/**
 * Reads the number of padding characters from the format string.
 *
 * @param Format
 *        The format string.
 *
 * @return Number of padding characters.
 *
 * @since XT 1.0
 */
XTCDECL
UINT64
BlpStringReadPadding(IN PUINT16 *Format)
{
    ULONG Count = 0;
    PUINT16 Fmt = *Format;

    /* Read the padding */
    for(;; ++Fmt)
    {
        switch(*Fmt)
        {
            case L'0' ... L'9':
                /* Check the number of leading zeroes */
                Count = Count * 10 + *Fmt - L'0';
                break;
            default:
                /* No padding by default */
                *Format = Fmt;
                return Count;
        }
    }

    /* No padding by default */
    return 0;
}
