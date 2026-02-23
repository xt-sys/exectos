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

/* Maximum double/integer value string length */
#define MAX_DOUBLE_STRING_SIZE          15
#define MAX_INTEGER_STRING_SIZE         25

/* Floating point definitions */
#define DOUBLE_EXPONENT_MASK            0x7FF0000000000000ULL
#define DOUBLE_EXPONENT_SHIFT           0x34
#define DOUBLE_EXPONENT_BIAS            0x3FF
#define DOUBLE_HIGH_VALUE_MASK          0x000FFFFF
#define DOUBLE_HIGH_VALUE_SHIFT         0x20
#define DOUBLE_PRECISION                6
#define DOUBLE_HEX_PRECISION            13
#define DOUBLE_SCIENTIFIC_PRECISION     -4
#define DOUBLE_SIGN_BIT                 0x8000000000000000ULL

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

/* Cryptographic related definitions */
#define SHA1_BLOCK_SIZE                 64
#define SHA1_DIGEST_SIZE                20

/* Runtime Library routine callbacks */
typedef XTSTATUS (*PWRITE_CHARACTER)(IN CHAR Character);
typedef XTSTATUS (*PWRITE_WIDE_CHARACTER)(IN WCHAR Character);

/* Variable types enumeration list */
typedef enum _RTL_VARIABLE_TYPE
{
    TypeUnknown,
    TypeAnsiString,
    TypeBoolean,
    TypeChar,
    TypeFloat,
    TypeGuid,
    TypeInteger,
    TypeString,
    TypeUnicodeString,
    TypeWideChar,
    TypeWideString
} RTL_VARIABLE_TYPE, *PRTL_VARIABLE_TYPE;

/* Bit Map structure definition */
typedef struct _RTL_BITMAP
{
    ULONG Size;
    PULONG_PTR Buffer;
} RTL_BITMAP, *PRTL_BITMAP;

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

/* Runtime Library SHA-1 context structure definition */
typedef struct _RTL_SHA1_CONTEXT
{
    ULONG   State[5];
    ULONG   Count[2];
    UCHAR   Buffer[SHA1_BLOCK_SIZE];
} RTL_SHA1_CONTEXT, *PRTL_SHA1_CONTEXT;

#endif /* __XTDK_RTLTYPES_H */
