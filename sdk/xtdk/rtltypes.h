/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/rtltypes.h
 * DESCRIPTION:     Runtime library structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_RTLTYPES_H
#define __XTDK_RTLTYPES_H

#include <xtbase.h>
#include <xttypes.h>


/* UUID string lengths */
#define GUID_STRING_LENGTH              38
#define PARTUUID_STRING_LENGTH          13

/* Maximum integer value string length */
#define MAX_INTEGER_STRING_SIZE         25

/* Floating point definitions */
#define DOUBLE_EXPONENT_MASK            0x7FF0000000000000ULL
#define DOUBLE_EXPONENT_SHIFT           0x34
#define DOUBLE_EXPONENT_BIAS            0x3FF

/* Print flag definitions */
#define PFL_ALWAYS_PRINT_SIGN           0x00000001
#define PFL_SPACE_FOR_PLUS              0x00000002
#define PFL_LEFT_JUSTIFIED              0x00000004
#define PFL_LEADING_ZEROES              0x00000008
#define PFL_LONG_INTEGER                0x00000010
#define PFL_LONG_DOUBLE                 0x00000020
#define PFL_WIDE_CHARACTER              0x00000040
#define PFL_SHORT_VALUE                 0x00000080
#define PFL_UNSIGNED                    0x00000100
#define PFL_UPPERCASE                   0x00000200
#define PFL_PRINT_RADIX                 0x00000400
#define PFL_FLOAT_FORMAT                0x00000800
#define PFL_SCI_FORMAT                  0x00001000
#define PFL_DIGIT_PRECISION             0x00002000
#define PFL_THOUSANDS_GROUPING          0x00004000

/* Runtime Library routine callbacks */
typedef XTSTATUS (*PWRITE_CHARACTER)(IN CHAR Character);
typedef XTSTATUS (*PWRITE_WIDE_CHARACTER)(IN WCHAR Character);

/* Variable types enumeration list */
typedef enum _RTL_VARIABLE_TYPE
{
    Unknown,
    AnsiString,
    Boolean,
    Char,
    Float,
    Guid,
    Integer,
    String,
    UnicodeString,
    WideChar,
    WideString
} RTL_VARIABLE_TYPE, *PRTL_VARIABLE_TYPE;

/* Runtime Library print context structure definition */
typedef struct _RTL_PRINT_CONTEXT
{
    PWRITE_CHARACTER WriteCharacter;
    PWRITE_WIDE_CHARACTER WriteWideCharacter;
    ULONG CharactersWritten;
} RTL_PRINT_CONTEXT, *PRTL_PRINT_CONTEXT;

/* Runtime Library print format properties structure definition */
typedef struct _RTL_PRINT_FORMAT_PROPERTIES
{
    RTL_VARIABLE_TYPE VariableType;
    ULONG Radix;
    LONG FieldWidth;
    LONG IntegerSize;
    LONG Precision;
    LONG Flags;
} RTL_PRINT_FORMAT_PROPERTIES, *PRTL_PRINT_FORMAT_PROPERTIES;

#endif /* __XTDK_RTLTYPES_H */
