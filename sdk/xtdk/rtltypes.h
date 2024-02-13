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

/* Floating point definitions */
#define DOUBLE_EXPONENT_MASK            0x7FF0000000000000ULL
#define DOUBLE_EXPONENT_SHIFT           0x34
#define DOUBLE_EXPONENT_BIAS            0x3FF

/* Runtime Library routine callbacks */
typedef VOID (*PWRITE_CHARACTER)(IN CHAR Character);
typedef VOID (*PWRITE_WIDE_CHARACTER)(IN WCHAR Character);

/* Runtime Library print context structure definition */
typedef struct _RTL_PRINT_CONTEXT
{
    PWRITE_CHARACTER WriteCharacter;
    PWRITE_WIDE_CHARACTER WriteWideCharacter;
    ULONG CharactersWritten;
    ULONG Limit;
} RTL_PRINT_CONTEXT, *PRTL_PRINT_CONTEXT;

#endif /* __XTDK_RTLTYPES_H */
