/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/rtlfuncs.h
 * DESCRIPTION:     XT runtime library routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_RTLFUNCS_H
#define __XTDK_RTLFUNCS_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>
#include <rtltypes.h>


/* Routines used by XTLDR */
XTCDECL
VOID
RtlInitializeListHead(IN PLIST_ENTRY ListHead);

XTCDECL
VOID
RtlInsertHeadList(IN OUT PLIST_ENTRY ListHead,
                  IN PLIST_ENTRY Entry);

XTCDECL
VOID
RtlInsertTailList(IN OUT PLIST_ENTRY ListHead,
                  IN PLIST_ENTRY Entry);

XTAPI
UCHAR
RtlReadRegisterByte(IN VOLATILE PVOID Register);

XTAPI
ULONG
RtlReadRegisterLong(IN VOLATILE PVOID Register);

XTAPI
USHORT
RtlReadRegisterShort(IN VOLATILE PVOID Register);


XTCDECL
VOID
RtlRemoveEntryList(IN PLIST_ENTRY Entry);


/* Runtime Library routines forward references */
XTAPI
VOID
RtlClearAllBits(IN PRTL_BITMAP BitMap);

XTAPI
VOID
RtlClearBit(IN PRTL_BITMAP BitMap,
            IN ULONG_PTR Bit);

XTAPI
VOID
RtlClearBits(IN PRTL_BITMAP BitMap,
             IN ULONG_PTR StartingIndex,
             IN ULONG_PTR Length);

XTAPI
ULONG
RtlClearSetBits(IN PRTL_BITMAP BitMap,
                IN ULONG_PTR Length,
                IN ULONG_PTR Index);

XTAPI
BOOLEAN
RtlCompareGuids(IN PGUID Guid1,
                IN PGUID Guid2);

XTAPI
SIZE_T
RtlCompareMemory(IN PCVOID LeftBuffer,
                 IN PCVOID RightBuffer,
                 IN SIZE_T Length);

XTAPI
SIZE_T
RtlCompareString(IN CONST PCHAR String1,
                 IN CONST PCHAR String2,
                 IN SIZE_T Length);

XTAPI
SIZE_T
RtlCompareStringInsensitive(IN CONST PCHAR String1,
                            IN CONST PCHAR String2,
                            IN SIZE_T Length);

XTAPI
SIZE_T
RtlCompareWideString(IN CONST PWCHAR String1,
                     IN CONST PWCHAR String2,
                     IN SIZE_T Length);

XTAPI
SIZE_T
RtlCompareWideStringInsensitive(IN CONST PWCHAR String1,
                                IN CONST PWCHAR String2,
                                IN SIZE_T Length);

XTAPI
PCHAR
RtlConcatenateString(OUT PCHAR Destination,
                     IN PCHAR Source,
                     IN SIZE_T Count);

XTAPI
PWCHAR
RtlConcatenateWideString(OUT PWCHAR Destination,
                         IN PWCHAR Source,
                         IN SIZE_T Count);

XTAPI
LARGE_INTEGER
RtlConvertToLargeInteger32(IN LONG Value);

XTAPI
LARGE_INTEGER
RtlConvertToLargeIntegerUnsigned32(IN ULONG Value);

XTAPI
VOID
RtlCopyMemory(OUT PVOID Destination,
              IN PCVOID Source,
              IN SIZE_T Length);

XTAPI
VOID
RtlCopyString(IN PCHAR Destination,
              IN PCCHAR Source,
              IN ULONG Length);

XTAPI
VOID
RtlCopyWideString(IN PWCHAR Destination,
                  IN CONST PWCHAR Source,
                  IN ULONG Length);

XTAPI
LARGE_INTEGER
RtlDivideLargeInteger(IN LARGE_INTEGER Dividend,
                      IN ULONG Divisor,
                      OUT PULONG Remainder);

XTAPI
ULONG_PTR
RtlFindClearBits(IN PRTL_BITMAP BitMap,
                 IN ULONG_PTR Length,
                 IN ULONG_PTR Index);

XTAPI
ULONG_PTR
RtlFindSetBits(IN PRTL_BITMAP BitMap,
               IN ULONG_PTR Length,
               IN ULONG_PTR Index);

XTAPI
PCHAR
RtlFindString(IN PCHAR Source,
              IN PCHAR Search);

XTAPI
PCHAR
RtlFindStringInsensitive(IN PCHAR Source,
                         IN PCHAR Search);

XTAPI
PWCHAR
RtlFindWideString(IN PWCHAR Source,
                  IN PWCHAR Search);

XTAPI
PWCHAR
RtlFindWideStringInsensitive(IN PWCHAR Source,
                             IN PWCHAR Search);

XTAPI
XTSTATUS
RtlFormatWideString(IN PRTL_PRINT_CONTEXT Context,
                    IN PCWSTR Format,
                    IN VA_LIST ArgumentList);

XTAPI
VOID
RtlInitializeBitMap(IN PRTL_BITMAP BitMap,
                    IN PULONG_PTR Buffer,
                    IN ULONG Size);

XTAPI
VOID
RtlMoveMemory(OUT PVOID Destination,
              IN PCVOID Source,
              IN SIZE_T Length);

XTAPI
LARGE_INTEGER
RtlMultiplyLargeInteger(IN LARGE_INTEGER Multiplicand,
                        IN LONG Multiplier);

XTAPI
VOID
RtlReverseString(IN OUT PCHAR String,
                 IN ULONG Length);

XTAPI
VOID
RtlReverseWideString(IN OUT PWCHAR String,
                     IN ULONG Length);

XTAPI
BOOLEAN
RtlSameMemory(IN PCVOID LeftBuffer,
              IN PCVOID RightBuffer,
              IN SIZE_T Length);

XTAPI
VOID
RtlSetAllBits(IN PRTL_BITMAP BitMap);

XTAPI
VOID
RtlSetBit(IN PRTL_BITMAP BitMap,
          IN ULONG_PTR Bit);

XTAPI
VOID
RtlSetBits(IN PRTL_BITMAP BitMap,
           IN ULONG_PTR StartingIndex,
           IN ULONG_PTR Length);

XTAPI
ULONG
RtlSetClearBits(IN PRTL_BITMAP BitMap,
                IN ULONG_PTR Length,
                IN ULONG_PTR Index);

XTAPI
VOID
RtlSetMemory(OUT PVOID Destination,
             IN UCHAR Byte,
             IN SIZE_T Length);

XTAPI
SIZE_T
RtlStringLength(IN CONST PCHAR String,
                IN SIZE_T MaxLength);

XTAPI
SIZE_T
RtlStringToWideString(OUT PWCHAR Destination,
                      IN CONST PCHAR *Source,
                      IN SIZE_T Length);

XTAPI
BOOLEAN
RtlTestBit(IN PRTL_BITMAP BitMap,
           IN ULONG_PTR Bit);

XTAPI
PCHAR
RtlTokenizeString(IN PCHAR String,
                  IN CONST PCHAR Delimiter,
                  IN OUT PCHAR *SavePtr);

XTAPI
PWCHAR
RtlTokenizeWideString(IN PWCHAR String,
                      IN CONST PWCHAR Delimiter,
                      IN OUT PWCHAR *SavePtr);

XTAPI
CHAR
RtlToLowerCharacter(IN CHAR Character);

XTAPI
WCHAR
RtlToLowerWideCharacter(IN WCHAR Character);

XTAPI
CHAR
RtlToUpperCharacter(IN CHAR Character);

XTAPI
WCHAR
RtlToUpperWideCharacter(IN WCHAR Character);

XTAPI
PCHAR
RtlTrimLeftString(IN CONST PCHAR String);

XTAPI
PWCHAR
RtlTrimLeftWideString(IN CONST PWCHAR String);

XTAPI
PCHAR
RtlTrimRightString(IN CONST PCHAR String);

XTAPI
PWCHAR
RtlTrimRightWideString(IN CONST PWCHAR String);

XTAPI
PCHAR
RtlTrimString(IN CONST PCHAR String);

XTAPI
PWCHAR
RtlTrimWideString(IN CONST PWCHAR String);

XTAPI
SIZE_T
RtlWideStringLength(IN CONST PWCHAR String,
                    IN SIZE_T MaxLength);

XTAPI
VOID
RtlWriteRegisterByte(IN VOLATILE PVOID Register,
                     IN UCHAR Value);

XTAPI
VOID
RtlWriteRegisterLong(IN VOLATILE PVOID Register,
                     IN ULONG Value);

XTAPI
VOID
RtlWriteRegisterShort(IN VOLATILE PVOID Register,
                      IN USHORT Value);

XTAPI
VOID
RtlZeroMemory(OUT PVOID Destination,
              IN SIZE_T Length);

#endif /* __XTDK_RTLFUNCS_H */
