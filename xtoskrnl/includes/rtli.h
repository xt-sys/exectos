/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtli.h
 * DESCRIPTION:     XT runtime library routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_RTLI_H
#define __XTOSKRNL_RTLI_H

#include <xtos.h>


/* Runtime Library routines forward references */
XTFASTCALL
CHAR
RtlAtomicAnd8(IN VOLATILE PCHAR Address,
              IN CHAR Mask);

XTFASTCALL
SHORT
RtlAtomicAnd16(IN VOLATILE PSHORT Address,
               IN SHORT Mask);

XTFASTCALL
LONG
RtlAtomicAnd32(IN VOLATILE PLONG Address,
               IN LONG Mask);

XTFASTCALL
LONG_PTR
RtlAtomicAnd64(IN VOLATILE PLONG_PTR Address,
               IN LONG_PTR Mask);

XTFASTCALL
UCHAR
RtlAtomicBitTestAndSet(IN VOLATILE PLONG Base,
                       IN LONG Offset);

XTFASTCALL
UCHAR
RtlAtomicBitTestAndSet64(IN VOLATILE PLONGLONG Base,
                         IN LONGLONG Offset);

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
LONG_PTR
RtlAtomicCompareExchange64(IN VOLATILE PLONG_PTR Address,
                           IN LONG_PTR Comperand,
                           IN LONG_PTR Exchange);

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
LONG_PTR
RtlAtomicDecrement64(IN VOLATILE PLONG_PTR Address);

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
LONG_PTR
RtlAtomicExchange64(IN VOLATILE PLONG_PTR Address,
                    IN LONG_PTR Exchange);

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
LONG_PTR
RtlAtomicExchangeAdd64(IN VOLATILE PLONG_PTR Address,
                       IN LONG_PTR Value);

XTFASTCALL
PVOID
RtlAtomicExchangePointer(IN VOLATILE PVOID *Address,
                         IN PVOID Exchange);

XTFASTCALL
PSINGLE_LIST_ENTRY
RtlAtomicFlushSingleList(IN PSINGLE_LIST_HEADER Header);

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
LONG_PTR
RtlAtomicIncrement64(IN VOLATILE PLONG_PTR Address);

XTFASTCALL
CHAR
RtlAtomicOr8(IN VOLATILE PCHAR Address,
             IN CHAR Mask);

XTFASTCALL
SHORT
RtlAtomicOr16(IN VOLATILE PSHORT Address,
              IN SHORT Mask);

XTFASTCALL
LONG
RtlAtomicOr32(IN VOLATILE PLONG Address,
              IN LONG Mask);

XTFASTCALL
LONG_PTR
RtlAtomicOr64(IN VOLATILE PLONG_PTR Address,
              IN LONG_PTR Mask);

XTFASTCALL
PSINGLE_LIST_ENTRY
RtlAtomicPopEntrySingleList(IN PSINGLE_LIST_HEADER Header);

XTFASTCALL
PSINGLE_LIST_ENTRY
RtlAtomicPushEntrySingleList(IN PSINGLE_LIST_HEADER Header,
                             IN PSINGLE_LIST_ENTRY Entry);

XTFASTCALL
CHAR
RtlAtomicXor8(IN VOLATILE PCHAR Address,
              IN CHAR Mask);

XTFASTCALL
SHORT
RtlAtomicXor16(IN VOLATILE PSHORT Address,
               IN SHORT Mask);

XTFASTCALL
LONG
RtlAtomicXor32(IN VOLATILE PLONG Address,
               IN LONG Mask);

XTFASTCALL
LONG_PTR
RtlAtomicXor64(IN VOLATILE PLONG_PTR Address,
               IN LONG_PTR Mask);

XTFASTCALL
USHORT
RtlByteSwap16(IN USHORT Source);

XTFASTCALL
ULONG
RtlByteSwap32(IN ULONG Source);

XTFASTCALL
ULONGLONG
RtlByteSwap64(IN ULONGLONG Source);

XTAPI
INT
RtlCountLeadingZeroes32(IN ULONG Value);

XTAPI
INT
RtlCountLeadingZeroes64(IN ULONG Value);

XTAPI
INT
RtlCountTrailingZeroes32(IN ULONG Value);

XTAPI
INT
RtlCountTrailingZeroes64(IN ULONGLONG Value);

XTAPI
LONGLONG
RtlDivide32(IN LONG Dividend,
            IN LONG Divisor,
            OUT PLONG Remainder);

XTAPI
LONGLONG
RtlDivide64(IN LONGLONG Dividend,
            IN LONGLONG Divisor,
            OUT PLONGLONG Remainder);

XTAPI
ULONGLONG
RtlDivideUnsigned32(IN ULONG Dividend,
                    IN ULONG Divisor,
                    OUT PULONG Remainder);

XTAPI
ULONGLONG
RtlDivideUnsigned64(IN ULONGLONG Dividend,
                    IN ULONGLONG Divisor,
                    OUT PULONGLONG Remainder);

XTAPI
LONG
RtlGetBaseExponent(IN DOUBLE Value,
                   OUT PDOUBLE PowerOfTen);

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

XTCDECL
VOID
RtlRemoveEntryList(IN PLIST_ENTRY Entry);

XTAPI
XTSTATUS
RtlpFormatWideStringArgumentSpecifier(IN PRTL_PRINT_CONTEXT Context,
                                     IN PCWSTR Format,
                                     IN PVA_LIST ArgumentList,
                                     IN OUT PULONG Index);

XTAPI
ULONGLONG
RtlpGetWideStringArgument(IN PVA_LIST ArgumentList,
                          IN ULONG ArgumentNumber,
                          IN LONG ArgumentSize);

XTAPI
ULONGLONG
RtlpGetWideStringSpecifierValue(IN PWCHAR *Format);

XTAPI
XTSTATUS
RtlpWriteWideCharacter(IN PRTL_PRINT_CONTEXT Context,
                       IN WCHAR Character);

XTAPI
XTSTATUS
RtlpWriteWideStringCustomValue(IN PRTL_PRINT_CONTEXT Context,
                               IN PCWSTR Format,
                               IN ...);

XTAPI
XTSTATUS
RtlpWriteWideStringIntegerValue(IN PRTL_PRINT_CONTEXT Context,
                                IN PRTL_PRINT_FORMAT_PROPERTIES FormatProperties,
                                IN ULONGLONG Integer);

XTAPI
XTSTATUS
RtlpWriteWideStringStringValue(PRTL_PRINT_CONTEXT Context,
                               PRTL_PRINT_FORMAT_PROPERTIES FormatProperties,
                               PCHAR String,
                               SIZE_T StringLength);

XTAPI
XTSTATUS
RtlpWriteWideStringValue(PRTL_PRINT_CONTEXT Context,
                         PRTL_PRINT_FORMAT_PROPERTIES FormatProperties,
                         PWCHAR String,
                         SIZE_T StringLength);

#endif /* __XTOSKRNL_RTLI_H */
