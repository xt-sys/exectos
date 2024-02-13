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
BOOLEAN
RtlCompareGuids(IN PGUID Guid1,
                IN PGUID Guid2);

XTAPI
SIZE_T
RtlCompareMemory(IN PCVOID LeftBuffer,
                 IN PCVOID RightBuffer,
                 IN SIZE_T Length);

XTCDECL
SIZE_T
RtlCompareString(IN CONST PCHAR String1,
                 IN CONST PCHAR String2,
                 IN SIZE_T Length);

XTCDECL
SIZE_T
RtlCompareStringInsensitive(IN CONST PCHAR String1,
                            IN CONST PCHAR String2,
                            IN SIZE_T Length);

XTCDECL
SIZE_T
RtlCompareWideString(IN CONST PWCHAR String1,
                     IN CONST PWCHAR String2,
                     IN SIZE_T Length);

XTCDECL
SIZE_T
RtlCompareWideStringInsensitive(IN CONST PWCHAR String1,
                                IN CONST PWCHAR String2,
                                IN SIZE_T Length);

XTCDECL
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
LARGE_INTEGER
RtlDivideLargeInteger(IN LARGE_INTEGER Dividend,
                      IN ULONG Divisor,
                      OUT PULONG Remainder);


XTAPI
VOID
RtlFillMemory(OUT PVOID Destination,
              IN SIZE_T Length,
              IN UCHAR Value);

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
BOOLEAN
RtlSameMemory(IN PCVOID LeftBuffer,
              IN PCVOID RightBuffer,
              IN SIZE_T Length);

XTAPI
VOID
RtlSetMemory(OUT PVOID Destination,
             IN UCHAR Byte,
             IN SIZE_T Length);

XTCDECL
SIZE_T
RtlStringLength(IN CONST PCHAR String,
                IN SIZE_T MaxLength);

XTCDECL
SIZE_T
RtlStringToWideString(OUT PWCHAR Destination,
                      IN CONST PCHAR *Source,
                      IN SIZE_T Length);

XTCDECL
PWCHAR
RtlTokenizeWideString(IN PWCHAR String,
                      IN CONST PWCHAR Delimiter,
                      IN OUT PWCHAR *SavePtr);

XTCDECL
PCHAR
RtlTrimLeftString(IN CONST PCHAR String);

XTCDECL
PCHAR
RtlTrimRightString(IN CONST PCHAR String);

XTCDECL
PCHAR
RtlTrimString(IN CONST PCHAR String);

XTCDECL
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
