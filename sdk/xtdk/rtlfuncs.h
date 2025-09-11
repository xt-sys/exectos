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


/* Runtime Library routines forward references */
XTCLINK
XTAPI
VOID
RtlClearAllBits(IN PRTL_BITMAP BitMap);

XTCLINK
XTAPI
VOID
RtlClearBit(IN PRTL_BITMAP BitMap,
            IN ULONG_PTR Bit);

XTCLINK
XTAPI
VOID
RtlClearBits(IN PRTL_BITMAP BitMap,
             IN ULONG_PTR StartingIndex,
             IN ULONG_PTR Length);

XTCLINK
XTAPI
ULONG
RtlClearSetBits(IN PRTL_BITMAP BitMap,
                IN ULONG_PTR Length,
                IN ULONG_PTR Index);

XTCLINK
XTAPI
BOOLEAN
RtlCompareGuids(IN PGUID Guid1,
                IN PGUID Guid2);

XTCLINK
XTAPI
SIZE_T
RtlCompareMemory(IN PCVOID LeftBuffer,
                 IN PCVOID RightBuffer,
                 IN SIZE_T Length);

XTCLINK
XTAPI
SIZE_T
RtlCompareString(IN PCSTR String1,
                 IN PCSTR String2,
                 IN SIZE_T Length);

XTCLINK
XTAPI
SIZE_T
RtlCompareStringInsensitive(IN PCSTR String1,
                            IN PCSTR String2,
                            IN SIZE_T Length);

XTCLINK
XTAPI
SIZE_T
RtlCompareWideString(IN PCWSTR String1,
                     IN PCWSTR String2,
                     IN SIZE_T Length);

XTCLINK
XTAPI
SIZE_T
RtlCompareWideStringInsensitive(IN PCWSTR String1,
                                IN PCWSTR String2,
                                IN SIZE_T Length);

XTCLINK
XTAPI
PCHAR
RtlConcatenateString(OUT PCHAR Destination,
                     IN PCHAR Source,
                     IN SIZE_T Count);

XTCLINK
XTAPI
PWCHAR
RtlConcatenateWideString(OUT PWCHAR Destination,
                         IN PWCHAR Source,
                         IN SIZE_T Count);

XTCLINK
XTAPI
LARGE_INTEGER
RtlConvertToLargeInteger32(IN LONG Value);

XTCLINK
XTAPI
LARGE_INTEGER
RtlConvertToLargeIntegerUnsigned32(IN ULONG Value);

XTCLINK
XTAPI
VOID
RtlCopyMemory(OUT PVOID Destination,
              IN PCVOID Source,
              IN SIZE_T Length);

XTCLINK
XTAPI
VOID
RtlCopyString(IN PCHAR Destination,
              IN PCSTR Source,
              IN ULONG Length);

XTCLINK
XTAPI
VOID
RtlCopyWideString(IN PWCHAR Destination,
                  IN PCWSTR Source,
                  IN ULONG Length);

XTCLINK
XTAPI
LARGE_INTEGER
RtlDivideLargeInteger(IN LARGE_INTEGER Dividend,
                      IN ULONG Divisor,
                      OUT PULONG Remainder);

XTCLINK
XTAPI
ULONG_PTR
RtlFindClearBits(IN PRTL_BITMAP BitMap,
                 IN ULONG_PTR Length,
                 IN ULONG_PTR Index);

XTCLINK
XTAPI
ULONG_PTR
RtlFindSetBits(IN PRTL_BITMAP BitMap,
               IN ULONG_PTR Length,
               IN ULONG_PTR Index);

XTCLINK
XTAPI
PCSTR
RtlFindString(IN PCSTR Source,
              IN PCSTR Search);

XTCLINK
XTAPI
PCSTR
RtlFindStringInsensitive(IN PCSTR Source,
                         IN PCSTR Search);

XTCLINK
XTAPI
PCWSTR
RtlFindWideString(IN PCWSTR Source,
                  IN PCWSTR Search);

XTCLINK
XTAPI
PCWSTR
RtlFindWideStringInsensitive(IN PCWSTR Source,
                             IN PCWSTR Search);

XTCLINK
XTAPI
XTSTATUS
RtlFormatWideString(IN PRTL_PRINT_CONTEXT Context,
                    IN PCWSTR Format,
                    IN VA_LIST ArgumentList);

XTCLINK
XTAPI
VOID
RtlInitializeBitMap(IN PRTL_BITMAP BitMap,
                    IN PULONG_PTR Buffer,
                    IN ULONG Size);

XTCLINK
XTCDECL
VOID
RtlInitializeListHead(IN PLIST_ENTRY ListHead);

XTCLINK
XTCDECL
VOID
RtlInsertHeadList(IN OUT PLIST_ENTRY ListHead,
                  IN PLIST_ENTRY Entry);

XTCLINK
XTCDECL
VOID
RtlInsertTailList(IN OUT PLIST_ENTRY ListHead,
                  IN PLIST_ENTRY Entry);

XTCLINK
XTCDECL
BOOLEAN
RtlListEmpty(IN PLIST_ENTRY ListHead);

XTCLINK
XTCDECL
BOOLEAN
RtlListLoop(IN PLIST_ENTRY ListHead);

XTCLINK
XTAPI
VOID
RtlMoveMemory(OUT PVOID Destination,
              IN PCVOID Source,
              IN SIZE_T Length);

XTCLINK
XTAPI
LARGE_INTEGER
RtlMultiplyLargeInteger(IN LARGE_INTEGER Multiplicand,
                        IN LONG Multiplier);

XTCLINK
XTCDECL
VOID
RtlRemoveEntryList(IN PLIST_ENTRY Entry);

XTCLINK
XTAPI
VOID
RtlReverseString(IN OUT PCHAR String,
                 IN ULONG Length);

XTCLINK
XTAPI
VOID
RtlReverseWideString(IN OUT PWCHAR String,
                     IN ULONG Length);

XTCLINK
XTAPI
BOOLEAN
RtlSameMemory(IN PCVOID LeftBuffer,
              IN PCVOID RightBuffer,
              IN SIZE_T Length);

XTCLINK
XTAPI
VOID
RtlSetAllBits(IN PRTL_BITMAP BitMap);

XTCLINK
XTAPI
VOID
RtlSetBit(IN PRTL_BITMAP BitMap,
          IN ULONG_PTR Bit);

XTCLINK
XTAPI
VOID
RtlSetBits(IN PRTL_BITMAP BitMap,
           IN ULONG_PTR StartingIndex,
           IN ULONG_PTR Length);

XTCLINK
XTAPI
ULONG
RtlSetClearBits(IN PRTL_BITMAP BitMap,
                IN ULONG_PTR Length,
                IN ULONG_PTR Index);

XTCLINK
XTAPI
VOID
RtlSetMemory(OUT PVOID Destination,
             IN UCHAR Byte,
             IN SIZE_T Length);

XTCLINK
XTAPI
SIZE_T
RtlStringLength(IN PCSTR String,
                IN SIZE_T MaxLength);

XTCLINK
XTAPI
SIZE_T
RtlStringToWideString(OUT PWCHAR Destination,
                      IN PCSTR *Source,
                      IN SIZE_T Length);

XTCLINK
XTAPI
BOOLEAN
RtlTestBit(IN PRTL_BITMAP BitMap,
           IN ULONG_PTR Bit);

XTCLINK
XTAPI
PCHAR
RtlTokenizeString(IN PCHAR String,
                  IN PCSTR Delimiter,
                  IN OUT PCHAR *SavePtr);

XTCLINK
XTAPI
PWCHAR
RtlTokenizeWideString(IN PWCHAR String,
                      IN PCWSTR Delimiter,
                      IN OUT PWCHAR *SavePtr);

XTCLINK
XTAPI
CHAR
RtlToLowerCharacter(IN CHAR Character);

XTCLINK
XTAPI
WCHAR
RtlToLowerWideCharacter(IN WCHAR Character);

XTCLINK
XTAPI
CHAR
RtlToUpperCharacter(IN CHAR Character);

XTCLINK
XTAPI
WCHAR
RtlToUpperWideCharacter(IN WCHAR Character);

XTCLINK
XTAPI
PCHAR
RtlTrimLeftString(IN PCHAR String);

XTCLINK
XTAPI
PWCHAR
RtlTrimLeftWideString(IN PWCHAR String);

XTCLINK
XTAPI
PCHAR
RtlTrimRightString(IN PCHAR String);

XTCLINK
XTAPI
PWCHAR
RtlTrimRightWideString(IN PWCHAR String);

XTCLINK
XTAPI
PCHAR
RtlTrimString(IN PCHAR String);

XTCLINK
XTAPI
PWCHAR
RtlTrimWideString(IN PWCHAR String);

XTCLINK
XTAPI
SIZE_T
RtlWideStringLength(IN PCWSTR String,
                    IN SIZE_T MaxLength);

XTCLINK
XTAPI
VOID
RtlZeroMemory(OUT PVOID Destination,
              IN SIZE_T Length);

#endif /* __XTDK_RTLFUNCS_H */
