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
XTAPI
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
XTAPI
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
XTAPI
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
 * Formats a wide string according to the given printf-alike format string.
 *
 * @param Context
 *        Supplies a pointer to the print context structure.
 *
 * @param Format
 *        Supplies a pointer to the printf-alike format string.
 *
 * @param ArgumentList
 *        Supplies a list of arguments to the format string.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RtlFormatWideString(IN PRTL_PRINT_CONTEXT Context,
                    IN PCWSTR Format,
                    IN VA_LIST ArgumentList)
{
    VA_LIST LocalArgumentList;
    XTSTATUS Status;
    ULONG Index;

    /* Make sure, that we have valid context and write routine */
    if(Context == NULL || Context->WriteWideCharacter == NULL)
    {
        /* Invalid context or write routine not set */
        return FALSE;
    }

    /* Check format string pointer */
    if(Format == NULL)
    {
        /* Write null string */
        Format = L"(null)";
    }

    /* Make a copy of the argument list */
    VA_COPY(LocalArgumentList, ArgumentList);

    /* Iterate through format string */
    Index = 0;
    while(Format[Index] != L'\0')
    {
        /* Look for format specifier */
        if(Format[Index] == L'%')
        {
            /* Handle format along with arguments */
            Status = RtlpFormatWideStringArgumentSpecifier(Context, Format, &LocalArgumentList, &Index);
        }
        else
        {
            /* Write wide character and increase string index */
            Status = RtlpWriteWideCharacter(Context, Format[Index]);
            Index++;
        }

        /* Make sure character written successfully */
        if(Status != STATUS_SUCCESS)
        {
            /* Return status code */
            return Status;
        }
    }

    /* Clean up the argument list */
    VA_END(LocalArgumentList);

    /* Return success */
    return STATUS_SUCCESS;
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
XTAPI
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
XTAPI
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
 * Formats a wide string according to the given printf-alike format string and a list of arguments.
 *
 * @param Context
 *        Supplies a pointer to the print context structure.
 *
 * @param Format
 *        Supplies a pointer to the printf-alike format string.
 *
 * @param ArgumentList
 *        Supplies a list of arguments to the format string.
 *
 * @param Index
 *        Supplies a pointer to the position of the current format specifier, that will be advanced beyond the specifier.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RtlpFormatWideStringArgumentSpecifier(IN PRTL_PRINT_CONTEXT Context,
                                     IN PCWSTR Format,
                                     IN PVA_LIST ArgumentList,
                                     IN OUT PULONG Index)
{
    RTL_PRINT_FORMAT_PROPERTIES FormatProperties;
    PUNICODE_STRING UnicodeStrArg;
    WCHAR Specifier, WideCharArg;
    PANSI_STRING AnsiStrArg;
    LONGLONG SpecifierValue;
    VA_LIST ArgumentsCopy;
    LARGE_DOUBLE FloatArg;
    PCWSTR FormatIndex;
    ULONG ArgPosition;
    PWCHAR WideStrArg;
    ULONGLONG IntArg;
    XTSTATUS Status;
    PGUID GuidArg;
    PCHAR StrArg;
    CHAR CharArg;

    /* Set current format specifier index and custom argument position */
    FormatIndex = Format + *Index + 1;
    ArgPosition = 0;

    /* Initialize format properties */
    RtlZeroMemory(&FormatProperties, sizeof(RTL_PRINT_FORMAT_PROPERTIES));
    FormatProperties.IntegerSize = sizeof(INT);
    FormatProperties.Precision = -1;


    /* Make a copy of the argument list */
    VA_COPY(ArgumentsCopy, *ArgumentList);

    /* Lookup parameter field */
    if((*FormatIndex >= L'1') && (*FormatIndex <= L'9'))
    {
        /* POSIX extension found, read its value */
        SpecifierValue = RtlpGetWideStringSpecifierValue((PWSTR*)&FormatIndex);

        /* Make sure parameter field ends with '$' character */
        if(*FormatIndex == L'$')
        {
            /* Store number of parameter and increment index */
            ArgPosition = (ULONG)SpecifierValue;
            FormatIndex++;
        }
        else
        {
            /* Field not ended with '$' character, treat it as width field */
            FormatProperties.FieldWidth = (ULONG)SpecifierValue;
        }
    }

    /* Lookup flags field */
    while(TRUE)
    {
        /* Look for a valid flag and set properties */
        if(*FormatIndex == L'\'')
        {
            /* Thousands grouping separator applied */
            FormatProperties.ThousandsGrouping = TRUE;
        }
        else if(*FormatIndex == L'-')
        {
            /* Left-align the output */
            FormatProperties.LeftJustified = TRUE;
        }
        else if(*FormatIndex == L' ')
        {
            /* Prepend a space for positive signed-numeric types */
            FormatProperties.SpaceForPlus = TRUE;
        }
        else if(*FormatIndex == L'+')
        {
            /* Prepend a plus for positive signed-numeric types */
            FormatProperties.AlwaysPrintSign = TRUE;
        }
        else if(*FormatIndex == L'#')
        {
            /* Convert to an alternate form */
            FormatProperties.PrintRadix = TRUE;
        }
        else if(*FormatIndex == L'0')
        {
            /* Prepend zeros for numeric types */
            FormatProperties.PrintLeadingZeroes = TRUE;
        }
        else
        {
            /* No more flags to read */
            break;
        }

        /* Move to the next character */
        FormatIndex++;
    }

    /* Check if output is left-justified */
    if(FormatProperties.LeftJustified)
    {
        /* Left justified output can't have leading zeros */
        FormatProperties.PrintLeadingZeroes = FALSE;
    }

    /* Check if plus for positive signed-numeric types is enabled */
    if(FormatProperties.AlwaysPrintSign)
    {
        /* Do not append a space when plus character is enabled */
        FormatProperties.SpaceForPlus = FALSE;
    }

    /* Lookup width field */
    if(*FormatIndex == L'*')
    {
        /* Skip dynamic width field indicator */
        FormatIndex++;

        /* Read dynamic width value from the argument list */
        FormatProperties.FieldWidth = VA_ARG(*ArgumentList, INT);
    }
    else if((*FormatIndex >= L'1') && (*FormatIndex <= L'9'))
    {
        /* Read a numeric width value */
        FormatProperties.FieldWidth = RtlpGetWideStringSpecifierValue((PWSTR*)&FormatIndex);
    }

    /* Check if field width is set to negative value */
    if(FormatProperties.FieldWidth < 0)
    {
        /* Force left-aligned output and turn field width into positive value */
        FormatProperties.LeftJustified = TRUE;
        FormatProperties.FieldWidth *= -1;
    }

    /* Lookup precision field */
    if(*FormatIndex == L'.')
    {
        /* Skip precision field indicator */
        FormatIndex++;

        /* Look for a dynamic precision value indicator */
        if(*FormatIndex == L'*')
        {
            /* Read dynamic precision value from the argument list */
            FormatIndex++;
            FormatProperties.Precision = VA_ARG(*ArgumentList, INT);
        }
        else if((*FormatIndex >= L'0') && (*FormatIndex <= L'9'))
        {
            /* Read a numeric precision value */
            FormatProperties.Precision = RtlpGetWideStringSpecifierValue((PWSTR*)&FormatIndex);
        }
        else
        {
            /* Set default precision */
            FormatProperties.Precision = 0;
        }
    }

    /* Check if precision is set to negative value */
    if(FormatProperties.Precision < 0)
    {
        /* Disable precision */
        FormatProperties.Precision = -1;
    }

    /* Lookup argument length modifier */
    Specifier = *FormatIndex;
    switch(Specifier)
    {
        case L'h':
            /* SHORT-sized integer argument */
            FormatIndex++;
            FormatProperties.IntegerSize = sizeof(SHORT);
            if(*FormatIndex == L'h')
            {
                /* CHAR-sized integer argument */
                FormatIndex++;
                FormatProperties.IntegerSize = sizeof(CHAR);
            }
            break;
        case L'j':
            /* INTMAX-sized integer argument */
            FormatIndex++;
            FormatProperties.IntegerSize = sizeof(LONG_PTR);
            break;
        case L'l':
            /* LONG-sized double/integer argument */
            FormatIndex++;
            FormatProperties.LongDouble = TRUE;
            FormatProperties.LongInteger = TRUE;
            FormatProperties.IntegerSize = sizeof(LONG);
            if(*FormatIndex == L'l')
            {
                /* LONGLONG-sized integer argument */
                FormatIndex++;
                FormatProperties.LongDouble = FALSE;
                FormatProperties.LongInteger = FALSE;
                FormatProperties.IntegerSize = sizeof(LONGLONG);
            }
            break;
        case L'q':
            /* BSD extension: 64-bit (quad word) integer argument */
            FormatIndex++;
            FormatProperties.IntegerSize = sizeof(LONGLONG);
            break;
        case L't':
            /* PTRDIFF-sized integer argument */
            FormatIndex++;
            FormatProperties.IntegerSize = sizeof(PVOID);
            break;
        case L'w':
            /* MSVC extension: wide character or wide string argument */
            FormatIndex++;
            FormatProperties.LongInteger = TRUE;
            break;
        case L'z':
            /* SIZE_T-sized integer argument */
            FormatIndex++;
            FormatProperties.IntegerSize = sizeof(SIZE_T);
            break;
        case L'I':
            /* MSVC extension: SIZE_T-sized integer argument */
            FormatIndex++;
            FormatProperties.IntegerSize = sizeof(SIZE_T);
            if((*FormatIndex == L'3') && (*(FormatIndex + 1) == L'2'))
            {
                /* MSVC extension: 32-bit (double word) integer argument */
                FormatIndex += 2;
                FormatProperties.LongInteger = TRUE;
                FormatProperties.IntegerSize = sizeof(LONG);
            }
            else if((*FormatIndex == L'6') && (*(FormatIndex + 1) == L'4'))
            {
                /* MSVC extension: 64-bit (quad word) integer argument */
                FormatIndex += 2;
                FormatProperties.IntegerSize = sizeof(LONGLONG);
            }
            break;
        case L'L':
            /* LONG-sized double argument */
            FormatIndex++;
            FormatProperties.LongDouble = TRUE;
            FormatProperties.IntegerSize = sizeof(LDOUBLE);
            break;
    }

    /* Lookup format specifier */
    Specifier = *FormatIndex++;

    /* Handle char and string modifiers */
    if(FormatProperties.LongInteger)
    {
        if(Specifier == L'c')
        {
            /* Wide character argument */
            Specifier = L'C';
        }
        else if(Specifier == L's')
        {
            /* Wide string argument */
            Specifier = L'S';
        }
    }

    /* Lookup format specifier */
    FormatProperties.UnsignedValue = TRUE;
    switch(Specifier)
    {
        case L'a':
            /* Double argument as hexadecimal number (lowercase) */
            FormatProperties.VariableType = Float;
            FormatProperties.ScientificFormat = TRUE;
            FormatProperties.Radix = 16;
            break;
        case L'A':
            /* Double argument as hexadecimal number (uppercase) */
            FormatProperties.VariableType = Float;
            FormatProperties.ScientificFormat = TRUE;
            FormatProperties.PrintUpperCase = TRUE;
            FormatProperties.Radix = 16;
            break;
        case L'b':
            /* XTOS extension: Boolean argument (lowercase) */
            FormatProperties.VariableType = Boolean;
            break;
        case L'B':
            /* XTOS extension: Boolean argument (uppercase) */
            FormatProperties.VariableType = Boolean;
            FormatProperties.PrintUpperCase = TRUE;
            break;
        case L'c':
            /* Character argument */
            FormatProperties.VariableType = Char;
            break;
        case L'C':
            /* Wide character argument */
            FormatProperties.VariableType = WideChar;
            break;
        case L'd':
        case L'i':
            /* Signed integer argument as decimal number */
            FormatProperties.VariableType = Integer;
            FormatProperties.Radix = 10;
            FormatProperties.UnsignedValue = FALSE;
            break;
        case L'e':
            /* Double argument in scientific notation (lowercase) */
            FormatProperties.VariableType = Float;
            FormatProperties.ScientificFormat = TRUE;
            break;
        case L'E':
            /* Double argument in scientific notation (uppercase) */
            FormatProperties.VariableType = Float;
            FormatProperties.ScientificFormat = TRUE;
            FormatProperties.PrintUpperCase = TRUE;
            break;
        case L'f':
            /* Double argument as floating point number (lowercase) */
            FormatProperties.VariableType = Float;
            FormatProperties.FloatFormat = TRUE;
            break;
        case L'F':
            /* Double argument as floating point number (uppercase) */
            FormatProperties.VariableType = Float;
            FormatProperties.FloatFormat = TRUE;
            FormatProperties.PrintUpperCase = TRUE;
            break;
        case L'g':
            /* Double argument as either floating point number or in scientific notation (lowercase) */
            FormatProperties.VariableType = Float;
            FormatProperties.SignificantDigitPrecision = TRUE;
            break;
        case L'G':
            /* Double argument as either floating point number or in scientific notation (uppercase) */
            FormatProperties.VariableType = Float;
            FormatProperties.PrintUpperCase = TRUE;
            FormatProperties.SignificantDigitPrecision = TRUE;
            break;
        case L'n':
            /* Write number of characters written so far into an integer pointer parameter */
            FormatProperties.VariableType = Integer;
            FormatProperties.IntegerSize = sizeof(PVOID);
            break;
        case L'o':
            /* Unsigned integer argument as octal number */
            FormatProperties.VariableType = Integer;
            FormatProperties.Radix = 8;
            break;
        case L'p':
            /* Pointer argument as hexadecimal number (lowercase) */
            FormatProperties.VariableType = Integer;
            FormatProperties.IntegerSize = sizeof(UINT_PTR);
            FormatProperties.Radix = 16;
            FormatProperties.PrintRadix = TRUE;
            break;
        case L'P':
            /* XTOS extension: Pointer argument as hexadecimal number (uppercase) */
            FormatProperties.VariableType = Integer;
            FormatProperties.IntegerSize = sizeof(UINT_PTR);
            FormatProperties.Radix = 16;
            FormatProperties.PrintUpperCase = TRUE;
            FormatProperties.PrintRadix = TRUE;
            break;
        case L's':
            /* String argument */
            FormatProperties.VariableType = String;
            break;
        case L'S':
            /* Wide string argument */
            FormatProperties.VariableType = WideString;
            break;
        case L'u':
            /* Unsigned integer argument as decimal number */
            FormatProperties.VariableType = Integer;
            FormatProperties.Radix = 10;
            break;
        case L'v':
            /* XTOS extension: UUID/GUID argument (lowercase) */
            FormatProperties.VariableType = Guid;
            break;
        case L'V':
            /* XTOS extension: UUID/GUID argument (uppercase) */
            FormatProperties.VariableType = Guid;
            FormatProperties.PrintUpperCase = TRUE;
            break;
        case L'x':
            /* Unsigned integer argument as hexadecimal number (lowercase) */
            FormatProperties.VariableType = Integer;
            FormatProperties.Radix = 16;
            break;
        case L'X':
            /* Unsigned integer argument as hexadecimal number (uppercase) */
            FormatProperties.VariableType = Integer;
            FormatProperties.Radix = 16;
            FormatProperties.PrintUpperCase = TRUE;
            break;
        case L'Z':
            /* MSVC extension: ANSI/Unicode string argument */
            FormatProperties.VariableType = FormatProperties.LongInteger ? UnicodeString : AnsiString;
            break;
        case L'%':
            /* Print '%' character */
            FormatProperties.VariableType = Unknown;
            WideCharArg = L'%';
            break;
        default:
            /* Unknown format specifier, print '?' character */
            FormatProperties.VariableType = Unknown;
            WideCharArg = L'?';
            break;
    }

    /* Finally, write the formatted argument */
    if(FormatProperties.VariableType == Unknown)
    {
        /* Write defined wide character */
        Status = RtlpWriteWideStringValue(Context, &FormatProperties, &WideCharArg, 1);
    }
    if(FormatProperties.VariableType == Boolean)
    {
        /* Boolean type */
        if(ArgPosition != 0)
        {
            /* Get argument value from specified argument position */
            IntArg = RtlpGetWideStringArgument(&ArgumentsCopy, ArgPosition, FormatProperties.IntegerSize);
        }
        else
        {
            /* Get argument value from the next argument */
            IntArg = VA_ARG(*ArgumentList, ULONGLONG);
        }

        /* Check if using uppercase format */
        if(FormatProperties.PrintUpperCase)
        {
            /* Set uppercase boolean string depending on argument value */
            WideStrArg = IntArg ? L"TRUE" : L"FALSE";
        }
        else
        {
            /* Set lowercase boolean string depending on argument value */
            WideStrArg = IntArg ? L"true" : L"false";
        }

        /* Write formatted boolean string */
        Status = RtlpWriteWideStringValue(Context, &FormatProperties, WideStrArg, RtlWideStringLength(WideStrArg, 0));
    }
    else if(FormatProperties.VariableType == Guid)
    {
        /* GUID type */
        if(ArgPosition != 0)
        {
            /* Get argument value from specified argument position */
            IntArg = RtlpGetWideStringArgument(&ArgumentsCopy, ArgPosition, sizeof(PGUID));
            GuidArg = (PGUID)(UINT_PTR)IntArg;
        }
        else
        {
            /* Get argument value from the next argument */
            GuidArg = VA_ARG(*ArgumentList, PGUID);
        }

        /* Make sure a pointer to GUID is not NULL */
        if(GuidArg != NULL)
        {
            /* Check if using uppercase format */
            if(FormatProperties.PrintUpperCase)
            {
                /* Use uppercase GUID format string */
                WideStrArg = L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X";
            }
            else
            {
                /* Use lowercase GUID format string */
                WideStrArg = L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x";
            }
            /* Write formatted GUID string */
            Status = RtlpWriteWideStringCustomValue(Context, WideStrArg, GuidArg->Data1, GuidArg->Data2, GuidArg->Data3,
                                                    GuidArg->Data4[0], GuidArg->Data4[1], GuidArg->Data4[2],
                                                    GuidArg->Data4[3], GuidArg->Data4[4], GuidArg->Data4[5],
                                                    GuidArg->Data4[6], GuidArg->Data4[7]);
        }
    }
    else if(FormatProperties.VariableType == Char)
    {
        /* Character type */
        if(ArgPosition != 0)
        {
            /* Get argument value from specified argument position */
            CharArg = (UCHAR)RtlpGetWideStringArgument(&ArgumentsCopy, ArgPosition, sizeof(UCHAR));
        }
        else
        {
            /* Get argument value from the next argument */
            CharArg = VA_ARG(*ArgumentList, INT);
        }

        /* Write formatted character */
        Status = RtlpWriteWideStringStringValue(Context, &FormatProperties, &CharArg, 1);
    }
    else if(FormatProperties.VariableType == WideChar)
    {
        /* Wide character type */
        if(ArgPosition != 0)
        {
            /* Get argument value from specified argument position */
            WideCharArg = (WCHAR)RtlpGetWideStringArgument(&ArgumentsCopy, ArgPosition, sizeof(WCHAR));
        }
        else
        {
            /* Get argument value from the next argument */
            WideCharArg = VA_ARG(*ArgumentList, INT);
        }

        /* Write formatted wide character */
        Status = RtlpWriteWideStringValue(Context, &FormatProperties, &WideCharArg, 1);
    }
    else if(FormatProperties.VariableType == Float)
    {
        /* Float/Double type */
        if(ArgPosition != 0)
        {
            /* Get argument value from specified argument position */
            FloatArg.QuadPart = RtlpGetWideStringArgument(&ArgumentsCopy, ArgPosition, sizeof(ULONGLONG));
        }
        else
        {
            /* Get argument value from the next argument, depending on its size */
            if(FormatProperties.LongDouble)
            {
                FloatArg.DoublePart = VA_ARG(*ArgumentList, LDOUBLE);
            }
            else
            {
                FloatArg.DoublePart = VA_ARG(*ArgumentList, DOUBLE);
            }
        }

        /* Write formatted double value */
        // Status = RtlpWriteWideStringDoubleValue(Context, &FormatProperties, FloatArg.DoublePart);
    }
    else if(FormatProperties.VariableType == Integer)
    {
        /* Integer type */
        if(ArgPosition != 0)
        {
            /* Get argument value from specified argument position */
            IntArg = RtlpGetWideStringArgument(&ArgumentsCopy, ArgPosition, FormatProperties.IntegerSize);

            /* Convert to required integer size */
            switch(FormatProperties.IntegerSize)
            {
                case sizeof(CHAR):
                    IntArg = (UCHAR)IntArg;
                    break;
                case sizeof(SHORT):
                    IntArg = (USHORT)IntArg;
                    break;
                case sizeof(LONG):
                    IntArg = (ULONG)IntArg;
                    break;
                case sizeof(LONGLONG):
                    IntArg = (ULONGLONG)IntArg;
                    break;
                default:
                    return STATUS_INVALID_PARAMETER;
            }
        }
        else
        {
            /* Get argument value from the next argument, depending on its size */
            switch(FormatProperties.IntegerSize)
            {
                case sizeof(CHAR):
                    IntArg = (CHAR)VA_ARG(*ArgumentList, UINT);
                    break;
                case sizeof(SHORT):
                    IntArg = (SHORT)VA_ARG(*ArgumentList, UINT);
                    break;
                case sizeof(LONG):
                    IntArg = VA_ARG(*ArgumentList, ULONG);
                    break;
                case sizeof(LONGLONG):
                    IntArg = VA_ARG(*ArgumentList, ULONGLONG);
                    break;
                default:
                    return STATUS_INVALID_PARAMETER;
            }
        }

        /* Check if using 'n' specifier */
        if(Specifier == L'n')
        {
            /* Make sure, that integer pointer parameter is not NULL */
            if(IntArg != (UINT_PTR)NULL)
            {
                /* Store number of characters written in integer pointer parameter */
                *((PINT)(UINT_PTR)IntArg) = Context->CharactersWritten;
            }
        }
        else
        {
            /* Write formatted integer value */
            Status = RtlpWriteWideStringIntegerValue(Context, &FormatProperties, IntArg);
        }
    }
    else if(FormatProperties.VariableType == String)
    {
        /* String type */
        if(ArgPosition != 0)
        {
            /* Get argument value from specified argument position */
            IntArg = RtlpGetWideStringArgument(&ArgumentsCopy, ArgPosition, sizeof(PCHAR));
            StrArg = (PCHAR)(UINT_PTR)IntArg;
        }
        else
        {
            /* Get argument value from the next argument */
            StrArg = VA_ARG(*ArgumentList, PCHAR);
        }

        /* Write formatted string value */
        Status = RtlpWriteWideStringStringValue(Context, &FormatProperties, StrArg, RtlStringLength(StrArg, 0));
    }
    else if(FormatProperties.VariableType == WideString)
    {
        /* Wide string type */
        if(ArgPosition != 0)
        {
            /* Get argument value from specified argument position */
            IntArg = RtlpGetWideStringArgument(&ArgumentsCopy, ArgPosition, sizeof(PWCHAR));
            WideStrArg = (PWCHAR)(UINT_PTR)IntArg;
        }
        else
        {
            /* Get argument value from the next argument */
            WideStrArg = VA_ARG(*ArgumentList, PWCHAR);
        }

        /* Write formatted wide string value */
        Status = RtlpWriteWideStringValue(Context, &FormatProperties, WideStrArg, RtlWideStringLength(WideStrArg, 0));
    }
    else if(FormatProperties.VariableType == AnsiString )
    {
        /* ANSI string type */
        if(ArgPosition != 0)
        {
            /* Get argument value from specified argument position */
            IntArg = RtlpGetWideStringArgument(&ArgumentsCopy, ArgPosition, sizeof(PANSI_STRING));
            AnsiStrArg = (PANSI_STRING)(UINT_PTR)IntArg;
        }
        else
        {
            /* Get argument value from the next argument */
            AnsiStrArg = VA_ARG(*ArgumentList, PANSI_STRING);
        }

        /* Make sure a pointer to ANSI_STRING is not NULL */
        if(AnsiStrArg != NULL)
        {
            /* Write formatted ANSI string value */
            Status = RtlpWriteWideStringStringValue(Context, &FormatProperties, AnsiStrArg->Buffer, AnsiStrArg->Length);
        }
    }
    else if(FormatProperties.VariableType == UnicodeString)
    {
        /* Unicode string type */
        if(ArgPosition != 0)
        {
            /* Get argument value from specified argument position */
            IntArg = RtlpGetWideStringArgument(&ArgumentsCopy, ArgPosition, sizeof(PUNICODE_STRING));
            UnicodeStrArg = (PUNICODE_STRING)(UINT_PTR)IntArg;
        }
        else
        {
            /* Get argument value from the next argument */
            UnicodeStrArg = VA_ARG(*ArgumentList, PUNICODE_STRING);
        }

        /* Make sure a pointer to UNICODE_STRING is not NULL */
        if(UnicodeStrArg != NULL)
        {
            /* Write formatted UNICODE string value */
            Status = RtlpWriteWideStringValue(Context, &FormatProperties, UnicodeStrArg->Buffer, UnicodeStrArg->Length);
        }
    }

    /* Cleanup ArgumentsCopy object */
    VA_END(ArgumentsCopy);

    /* Increase index position according to number of characters consumed */
    *Index += ((UINT_PTR)FormatIndex - (UINT_PTR)(Format + *Index)) / sizeof(WCHAR);

    /* Return status code */
    return Status;
}

/**
 * Gets the positional argument by scanning the argument list.
 *
 * @param ArgumentList
 *        Supplies a pointer to the argument list.
 *
 * @param ArgumentNumber
 *        Supplies the argument number.
 *
 * @param ArgumentSize
 *        Supplies the expected size of the argument.
 */
XTAPI
ULONGLONG
RtlpGetWideStringArgument(IN PVA_LIST ArgumentList,
                          IN ULONG ArgumentNumber,
                          IN LONG ArgumentSize)
{
    VA_LIST ArgumentsCopy;
    ULONGLONG Value;
    ULONG Index;

    /* Make a copy of the argument list */
    VA_COPY(ArgumentsCopy, *ArgumentList);

    /* Skip all arguments before the specified one */
    for(Index = 1; Index < ArgumentNumber; Index += 1)
    {
        /* Skip the argument */
        Value = VA_ARG(ArgumentsCopy, LONGLONG);
    }

    /* Set default value */
    Value = 0;

    /* Get the argument value depending on its declared size */
    switch(ArgumentSize)
    {
        case sizeof(CHAR):
            Value = (UCHAR)VA_ARG(ArgumentsCopy, INT);
            break;
        case sizeof(SHORT):
            Value = (USHORT)VA_ARG(ArgumentsCopy, INT);
            break;
        case sizeof(LONG):
            Value = VA_ARG(ArgumentsCopy, LONG);
            break;
        case sizeof(LONGLONG):
            Value = VA_ARG(ArgumentsCopy, LONGLONG);
            break;
        default:
            Value = 0;
            break;
    }

    /* Cleanup ArgumentsCopy object and return the argument value */
    VA_END(ArgumentsCopy);
    return Value;
}

/**
 * Gets the specifier integer value from the wide string advancing the pointer.
 *
 * @param Format
 *        Supplies a pointer to the wide string format, at integer value position.
 *
 * @return This routine returns a specifier integer value read from wide string format, or zero if no valid value found.
 *
 * @since XT 1.0
 */
XTAPI
ULONGLONG
RtlpGetWideStringSpecifierValue(IN PWCHAR *Format)
{
    ULONG Count;
    PWCHAR Fmt;

    /* Initialize variables */
    Fmt = *Format;
    Count = 0;

    /* Read the specifier value */
    for(;; Fmt++)
    {
        switch(*Fmt)
        {
            case L'0' ... L'9':
                /* Read the number from wide string */
                Count = Count * 10 + *Fmt - L'0';
                break;
            default:
                /* No more numbers, return count */
                *Format = Fmt;
                return Count;
        }
    }

    /* Return zero if no value specified */
    return 0;
}

/**
 * Writes a wide character to the destination provided by the print context.
 *
 * @param Context
 *        Supplies a pointer to the print context structure.
 *
 * @param Character
 *        Supplies the wide character to write.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RtlpWriteWideCharacter(IN PRTL_PRINT_CONTEXT Context,
                       IN WCHAR Character)
{
    XTSTATUS Status;

    /* Write wide character and increment number of characters written so far */
    Status = Context->WriteWideCharacter(Character);
    Context->CharactersWritten++;

    /* Return status code */
    return Status;
}

/**
 * Writes a wide string custom-formatted value to the destination provided by the print context.
 *
 * @param Context
 *        Supplies a pointer to the print context structure.
 *
 * @param Format
 *        Supplies a pointer to the printf-alike format string.
 *
 * @param ...
 *        Depending on the format string, this routine might expect a sequence of additional arguments.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RtlpWriteWideStringCustomValue(IN PRTL_PRINT_CONTEXT Context,
                               IN PCWSTR Format,
                               IN ...)
{
    VA_LIST Arguments;
    XTSTATUS Status;

    /* Initialise the va_list */
    VA_START(Arguments, Format);

    /* Format and print the string to the desired output */
    Status = RtlFormatWideString(Context, Format, Arguments);

    /* Clean up the va_list */
    VA_END(Arguments);

    /* Return status code */
    return Status;
}

/**
 * Writes a wide string integer value to the destination provided by the print context.
 *
 * @param Context
 *        Supplies a pointer to the print context structure.
 *
 * @param FormatProperties
 *        Supplies a pointer to the print format properties structure, describing the style characteristics.
 *
 * @param Integer
 *        Supplies the integer value to write as a wide string.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RtlpWriteWideStringIntegerValue(IN PRTL_PRINT_CONTEXT Context,
                                IN PRTL_PRINT_FORMAT_PROPERTIES FormatProperties,
                                IN ULONGLONG Integer)
{
    LONG BufferIndex, FieldLength, IntegerLength, PrecisionLength, PrefixIndex, PrefixLength;
    WCHAR Buffer[MAX_INTEGER_STRING_SIZE];
    ULONGLONG NextInteger, Remainder;
    BOOLEAN Negative;
    WCHAR Prefix[4];
    WCHAR Character;
    XTSTATUS Status;

    /* Set default values */
    IntegerLength = 0;
    Negative = FALSE;

    /* Check if this is signed integer */
    if(!FormatProperties->UnsignedValue)
    {
        /* Check integer size and extend to signed value */
        switch(FormatProperties->IntegerSize)
        {
            case sizeof(CHAR):
                Integer = (CHAR)Integer;
                break;
            case sizeof(SHORT):
                Integer = (SHORT)Integer;
                break;
            case sizeof(LONG):
                Integer = (LONG)Integer;
                break;
        }
    }

    /* Check if the integer value is zero */
    if(Integer == 0)
    {
        /* Cannot print radix if integer is zero */
        FormatProperties->PrintRadix = FALSE;
    }

    /* Check if any of the integer value or precision is non-zero */
    if(Integer != 0 || FormatProperties->Precision != 0)
    {
        /* Check if the integer is negative */
        if(!FormatProperties->UnsignedValue && (LONGLONG)Integer < 0)
        {
            /* Mark the integer as negative and turn it positive */
            Negative = TRUE;
            Integer *= -1;
        }

        /* Initialize the buffer */
        RtlZeroMemory(Buffer, sizeof(Buffer));

        /* Convert the integer into a reversed wide string */
        do
        {
            /* Get the next digit */
            NextInteger = RtlDivideUnsigned64(Integer, FormatProperties->Radix, &Remainder);

            /* Convert the digit into a character */
            Character = (WCHAR)Remainder;
            if(Character > 9)
            {
                /* Check if the character should be upper case */
                if(FormatProperties->PrintUpperCase)
                {
                    /* Get the uppercase character */
                    Character = Character - 10 + L'A';
                }
                else
                {
                    /* Get the lowercase character */
                    Character = Character - 10 + L'a';
                }
            }
            else
            {
                /* Get the numeric character */
                Character += L'0';
            }

            /* Store the character in the buffer */
            Buffer[IntegerLength] = Character;
            IntegerLength += 1;

            /* Get next signed digit */
            Integer = NextInteger;
        }
        while(Integer > 0);

        /* Reverse the string representation of the integer */
        RtlReverseWideString(Buffer, IntegerLength);
    }

    /* Handle the sign decoration */
    PrefixLength = 0;
    if(!FormatProperties->UnsignedValue && Negative)
    {
        /* Signed negative value, write '-' character */
        Prefix[PrefixLength] = L'-';
        PrefixLength += 1;
    }
    else if(FormatProperties->AlwaysPrintSign)
    {
        /* Write '+' character for positive value */
        Prefix[PrefixLength] = L'+';
        PrefixLength += 1;
    }
    else if(FormatProperties->SpaceForPlus)
    {
        /* Write ' ' character for positive value */
        Prefix[PrefixLength] = L' ';
        PrefixLength += 1;
    }

    /* Handle the radix decoration */
    if(FormatProperties->PrintRadix)
    {
        if(FormatProperties->Radix == 8)
        {
            /* Check if leading zero is required */
            if(Buffer[0] != L'0')
            {
                /* Write '0' character */
                Prefix[PrefixLength] = L'0';
                PrefixLength += 1;
            }
        }
        else if(FormatProperties->Radix == 16)
        {
            /* Write '0x' characters */
            Prefix[PrefixLength] = L'0';
            PrefixLength += 1;

            /* Write lowercase 'x' character */
            Prefix[PrefixLength] = L'x';

            PrefixLength += 1;
        }
    }

    /* Calculate the precision */
    PrecisionLength = 0;
    if(IntegerLength < FormatProperties->Precision)
    {
        PrecisionLength = FormatProperties->Precision - IntegerLength;
    }

    /* Calculate the field length */
    FieldLength = 0;
    if(IntegerLength + PrefixLength + PrecisionLength < FormatProperties->FieldWidth)
    {
        FieldLength = FormatProperties->FieldWidth - (IntegerLength + PrefixLength + PrecisionLength);
    }

    /* Check if leading zero padding is required and if field is left aligned */
    if(!FormatProperties->LeftJustified || FormatProperties->PrintLeadingZeroes)
    {
        Character = L' ';
        if(FormatProperties->PrintLeadingZeroes)
        {
            /* Write leading zero padding characters */
            Character = L'0';
            for(PrefixIndex = 0; PrefixIndex < PrefixLength; PrefixIndex++)
            {
                Status = RtlpWriteWideCharacter(Context, Prefix[PrefixIndex]);
                if(Status != STATUS_SUCCESS)
                {
                    /* Failed to write character, return status code */
                    return Status;
                }
            }

            /* Clear prefix */
            PrefixLength = 0;
        }

        /* Write additional field width characters */
        while(FieldLength > 0)
        {
            Status = RtlpWriteWideCharacter(Context, Character);
            if(Status != STATUS_SUCCESS)
            {
                /* Failed to write character, return status code */
                return Status;
            }

            /* Decrement field length */
            FieldLength--;
        }
    }

    /* Write the prefix characters */
    for(PrefixIndex = 0; PrefixIndex < PrefixLength; PrefixIndex++)
    {
        Status = RtlpWriteWideCharacter(Context, Prefix[PrefixIndex]);
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to write character, return status code */
            return Status;
        }
    }

    /* Fill the precision characters with '0' */
    while(PrecisionLength > 0)
    {
        Status = RtlpWriteWideCharacter(Context, L'0');
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to write character, return status code */
            return Status;
        }

        /* Decrement precision length */
        PrecisionLength--;
    }

    /* Write the actual integer value */
    for(BufferIndex = 0; BufferIndex < IntegerLength; BufferIndex++)
    {
        Status = RtlpWriteWideCharacter(Context, Buffer[BufferIndex]);
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to write character, return status code */
            return Status;
        }
    }

    /* Write additional field width with ' ' characters */
    while(FieldLength > 0)
    {
        Status = RtlpWriteWideCharacter(Context, L' ');
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to write character, return status code */
            return Status;
        }

        /* Decrement field length */
        FieldLength--;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Writes a string value to the destination provided by the print context.
 *
 * @param Context
 *        Supplies a pointer to the print context structure.
 *
 * @param FormatProperties
 *        Supplies a pointer to the print format properties structure, describing the style characteristics.
 *
 * @param String
 *        Supplies the string value to write as a wide string.
 *
 * @param Character
 *        Specifies whether the string value is expected to be a single character or not.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RtlpWriteWideStringStringValue(PRTL_PRINT_CONTEXT Context,
                               PRTL_PRINT_FORMAT_PROPERTIES FormatProperties,
                               PCHAR String,
                               SIZE_T StringLength)
{
    WCHAR WideCharacter[2];
    ULONG PaddingLength;
    XTSTATUS Status;

    /* Check for NULL string */
    if(String == NULL)
    {
        /* Print '(null)' instead */
        String = "(null)";
        StringLength = 6;
    }

    /* Check if string length exceeds precision limit */
    if((FormatProperties->Precision >= 0) && (StringLength > FormatProperties->Precision))
    {
        /* Limit string length to precision number */
        StringLength = FormatProperties->Precision;
    }

    /* Calculate padding */
    PaddingLength = 0;
    if(FormatProperties->FieldWidth > StringLength)
    {
        PaddingLength = FormatProperties->FieldWidth - StringLength;
    }

    /* Check is left side padding is required */
    if(!FormatProperties->LeftJustified)
    {
        /* Pad left */
        while(PaddingLength > 0)
        {
            /* Write space */
            Status = RtlpWriteWideCharacter(Context, L' ');
            if(Status != STATUS_SUCCESS)
            {
                /* Failed to write character, return status code */
                return Status;
            }

            /* Decrement padding length */
            PaddingLength--;
        }
    }

    /* Write string character by character */
    while(StringLength)
    {
        /* Prepare wide character to write */
        WideCharacter[0] = *String;
        WideCharacter[1] = 0;

        /* Write wide character */
        Status = RtlpWriteWideCharacter(Context, *WideCharacter);
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to write character, return status code */
            return Status;
        }

        /* Get next character */
        StringLength--;
        String++;
    }

    /* Pad right, if required */
    while(PaddingLength > 0)
    {
        /* Write space */
        Status = RtlpWriteWideCharacter(Context, L' ');
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to write character, return status code */
            return Status;
        }

        /* Decrement padding length */
        PaddingLength--;
    }

    /* Return success */
    return STATUS_SUCCESS;
}

/**
 * Writes a wide string value to the destination provided by the print context.
 *
 * @param Context
 *        Supplies a pointer to the print context structure.
 *
 * @param FormatProperties
 *        Supplies a pointer to the print format properties structure, describing the style characteristics.
 *
 * @param String
 *        Supplies the wide string value to write.
 *
 * @param Character
 *        Specifies whether the string value is expected to be a single character or not.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
RtlpWriteWideStringValue(PRTL_PRINT_CONTEXT Context,
                         PRTL_PRINT_FORMAT_PROPERTIES FormatProperties,
                         PWCHAR String,
                         SIZE_T StringLength)
{
    ULONG PaddingLength;
    XTSTATUS Status;

    /* Check for NULL string */
    if(String == NULL)
    {
        /* Print '(null)' instead */
        String = L"(null)";
        StringLength = 6;
    }

    /* Check if string length exceeds precision limit */
    if((FormatProperties->Precision >= 0) && (StringLength > FormatProperties->Precision))
    {
        /* Limit string length to precision number */
        StringLength = FormatProperties->Precision;
    }

    /* Calculate padding */
    PaddingLength = 0;
    if(FormatProperties->FieldWidth > StringLength)
    {
        PaddingLength = FormatProperties->FieldWidth - StringLength;
    }


    /* Check is left side padding is required */
    if(!FormatProperties->LeftJustified)
    {
        /* Pad left */
        while(PaddingLength > 0)
        {
            /* Write space */
            Status = RtlpWriteWideCharacter(Context, L' ');
            if(Status != STATUS_SUCCESS)
            {
                /* Failed to write character, return status code */
                return Status;
            }

            /* Decrement padding length */
            PaddingLength--;
        }
    }

    /* Write string character by character */
    while(StringLength != 0)
    {
        /* Write wide character */
        Status = RtlpWriteWideCharacter(Context, *String);
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to write character, return status code */
            return Status;
        }

        /* Get next character */
        StringLength--;
        String++;
    }

    /* Pad right, if required */
    while(PaddingLength > 0)
    {
        /* Write space */
        Status = RtlpWriteWideCharacter(Context, L' ');
        if(Status != STATUS_SUCCESS)
        {
            /* Failed to write character, return status code */
            return Status;
        }

        /* Decrement padding length */
        PaddingLength--;
    }

    /* Return success */
    return STATUS_SUCCESS;
}
