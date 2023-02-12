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


XTFASTCALL
CHAR
RtlAtomicCompareExchange8(IN VOLATILE PCHAR Address,
                          IN CHAR Comperand,
                          IN CHAR Exchange);

XTFASTCALL
SHORT
RtlAtomicCompareExchange16(IN VOLATILE PSHORT Address,
                           IN SHORT Comperand,
                           IN SHORT Exchange);

XTFASTCALL
LONG
RtlAtomicCompareExchange32(IN VOLATILE PLONG Address,
                           IN LONG Comperand,
                           IN LONG Exchange);

XTFASTCALL
LONGLONG
RtlAtomicCompareExchange64(IN VOLATILE PLONGLONG Address,
                           IN LONGLONG Comperand,
                           IN LONGLONG Exchange);

XTFASTCALL
PVOID
RtlAtomicCompareExchangePointer(IN VOLATILE PVOID *Address,
                                IN PVOID Comperand,
                                IN PVOID Exchange);

XTFASTCALL
CHAR
RtlAtomicDecrement8(IN VOLATILE PCHAR Address);

XTFASTCALL
SHORT
RtlAtomicDecrement16(IN VOLATILE PSHORT Address);

XTFASTCALL
LONG
RtlAtomicDecrement32(IN VOLATILE PLONG Address);

XTFASTCALL
LONGLONG
RtlAtomicDecrement64(IN VOLATILE PLONGLONG Address);

XTFASTCALL
CHAR
RtlAtomicExchange8(IN VOLATILE PCHAR Address,
                   IN CHAR Exchange);

XTFASTCALL
SHORT
RtlAtomicExchange16(IN VOLATILE PSHORT Address,
                    IN SHORT Exchange);

XTFASTCALL
LONG
RtlAtomicExchange32(IN VOLATILE PLONG Address,
                    IN LONG Exchange);

XTFASTCALL
LONGLONG
RtlAtomicExchange64(IN VOLATILE PLONGLONG Address,
                    IN LONGLONG Exchange);

XTFASTCALL
CHAR
RtlAtomicExchangeAdd8(IN VOLATILE PCHAR Address,
                      IN CHAR Value);

XTFASTCALL
SHORT
RtlAtomicExchangeAdd16(IN VOLATILE PSHORT Address,
                       IN SHORT Value);

XTFASTCALL
LONG
RtlAtomicExchangeAdd32(IN VOLATILE PLONG Address,
                       IN LONG Value);

XTFASTCALL
LONGLONG
RtlAtomicExchangeAdd64(IN VOLATILE PLONGLONG Address,
                       IN LONGLONG Value);

XTFASTCALL
PVOID
RtlAtomicExchangePointer(IN VOLATILE PVOID *Address,
                         IN PVOID Exchange);

XTFASTCALL
CHAR
RtlAtomicIncrement8(IN VOLATILE PCHAR Address);

XTFASTCALL
SHORT
RtlAtomicIncrement16(IN VOLATILE PSHORT Address);

XTFASTCALL
LONG
RtlAtomicIncrement32(IN VOLATILE PLONG Address);

XTFASTCALL
LONGLONG
RtlAtomicIncrement64(IN VOLATILE PLONGLONG Address);

XTAPI
SIZE_T
RtlCompareMemory(IN PCVOID LeftBuffer,
                 IN PCVOID RightBuffer,
                 IN SIZE_T Length);

XTAPI
VOID
RtlCopyMemory(OUT PVOID Destination,
              IN PCVOID Source,
              IN SIZE_T Length);

XTAPI
VOID
RtlFillMemory(OUT PVOID Destination,
              IN SIZE_T Length,
              IN UCHAR Value);

XTCDECL
VOID
RtlInitializeListHead(IN PLIST_ENTRY ListHead);

XTCDECL
VOID
RtlInitializeListHead32(IN PLIST_ENTRY32 ListHead);

XTCDECL
VOID
RtlInsertHeadList(IN OUT PLIST_ENTRY ListHead,
                  IN PLIST_ENTRY Entry);

XTCDECL
VOID
RtlInsertTailList(IN OUT PLIST_ENTRY ListHead,
                  IN PLIST_ENTRY Entry);

XTCDECL
BOOLEAN
RtlListEmpty(PLIST_ENTRY ListHead);

XTCDECL
BOOLEAN
RtlListLoop(IN PLIST_ENTRY ListHead);

XTAPI
VOID
RtlMoveMemory(OUT PVOID Destination,
              IN PCVOID Source,
              IN SIZE_T Length);

XTCDECL
VOID
RtlRemoveEntryList(IN PLIST_ENTRY Entry);

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
RtlStringLength(IN CONST PUCHAR String,
                IN SIZE_T MaxLength);

XTCDECL
INT
RtlStringToWideString(OUT PWCHAR Destination,
                      IN CONST PUCHAR *Source,
                      IN SIZE_T Length);

XTFASTCALL
ULONG
RtlUlongByteSwap(IN ULONG Source);

XTFASTCALL
ULONGLONG
RtlUlonglongByteSwap(IN ULONGLONG Source);

XTFASTCALL
USHORT
RtlUshortByteSwap(IN USHORT Source);

XTCDECL
INT
RtlWideStringCompare(IN CONST PWCHAR String1,
                     IN CONST PWCHAR String2,
                     IN CONST SIZE_T Length);

XTCDECL
PWCHAR
RtlWideStringConcatenate(OUT PWCHAR Destination,
                         IN PWCHAR Source,
                         IN SIZE_T Count);

XTCDECL
SIZE_T
RtlWideStringLength(IN CONST PWCHAR String,
                    IN SIZE_T MaxLength);

XTCDECL
PWCHAR
RtlWideStringTokenize(IN PWCHAR String,
                      IN CONST PWCHAR Delimiter,
                      IN OUT PWCHAR *SavePtr);

XTAPI
VOID
RtlZeroMemory(OUT PVOID Destination,
              IN SIZE_T Length);

#endif /* __XTDK_RTLFUNCS_H */
