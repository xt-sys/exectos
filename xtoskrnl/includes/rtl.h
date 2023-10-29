/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl.h
 * DESCRIPTION:     XT runtime library routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTOSKRNL_RTL_H
#define __XTOSKRNL_RTL_H

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

#endif /* __XTOSKRNL_RTL_H */
