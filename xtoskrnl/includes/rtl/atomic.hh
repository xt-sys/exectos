/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/atomic.hh
 * DESCRIPTION:     Atomic operations support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_ATOMIC_HH
#define __XTOSKRNL_RTL_ATOMIC_HH

#include <xtos.hh>


/* Runtime Library */
namespace RTL
{
    class Atomic
    {
        public:
            STATIC XTFASTCALL CHAR And8(IN PCHAR Address,
                                        IN CHAR Mask);
            STATIC XTFASTCALL SHORT And16(IN PSHORT Address,
                                          IN SHORT Mask);
            STATIC XTFASTCALL LONG And32(IN PLONG Address,
                                         IN LONG Mask);
            STATIC XTFASTCALL LONG_PTR And64(IN PLONG_PTR Address,
                                             IN LONG_PTR Mask);
            STATIC XTFASTCALL UCHAR BitTestAndSet(IN PLONG Base,
                                                  IN LONG Offset);
            STATIC XTFASTCALL UCHAR BitTestAndSet64(IN PLONGLONG Base,
                                                    IN LONGLONG Offset);
            STATIC XTFASTCALL CHAR CompareExchange8(IN PCHAR Address,
                                                 IN CHAR Comperand,
                                                 IN CHAR Exchange);
            STATIC XTFASTCALL SHORT CompareExchange16(IN PSHORT Address,
                                                   IN SHORT Comperand,
                                                   IN SHORT Exchange);
            STATIC XTFASTCALL LONG CompareExchange32(IN PLONG Address,
                                                  IN LONG Comperand,
                                                  IN LONG Exchange);
            STATIC XTFASTCALL LONG_PTR CompareExchange64(IN PLONG_PTR Address,
                                                      IN LONG_PTR Comperand,
                                                      IN LONG_PTR Exchange);
            STATIC XTFASTCALL PVOID CompareExchangePointer(IN PVOID *Address,
                                                        IN PVOID Comperand,
                                                        IN PVOID Exchange);
            STATIC XTFASTCALL CHAR Decrement8(IN PCHAR Address);
            STATIC XTFASTCALL SHORT Decrement16(IN PSHORT Address);
            STATIC XTFASTCALL LONG Decrement32(IN PLONG Address);
            STATIC XTFASTCALL LONG_PTR Decrement64(IN PLONG_PTR Address);
            STATIC XTFASTCALL CHAR Exchange8(IN PCHAR Address,
                                             IN CHAR Exchange);
            STATIC XTFASTCALL SHORT Exchange16(IN PSHORT Address,
                                               IN SHORT Exchange);
            STATIC XTFASTCALL LONG Exchange32(IN PLONG Address,
                                              IN LONG Exchange);
            STATIC XTFASTCALL LONG_PTR Exchange64(IN PLONG_PTR Address,
                                                  IN LONG_PTR Exchange);
            STATIC XTFASTCALL CHAR ExchangeAdd8(IN PCHAR Address,
                                                IN CHAR Value);
            STATIC XTFASTCALL SHORT ExchangeAdd16(IN PSHORT Address,
                                                  IN SHORT Value);
            STATIC XTFASTCALL LONG ExchangeAdd32(IN PLONG Address,
                                                 IN LONG Value);
            STATIC XTFASTCALL LONG_PTR ExchangeAdd64(IN PLONG_PTR Address,
                                                     IN LONG_PTR Value);
            STATIC XTFASTCALL PVOID ExchangePointer(IN PVOID *Address,
                                                    IN PVOID Exchange);
            STATIC XTFASTCALL PSINGLE_LIST_ENTRY FlushSingleList(IN PSINGLE_LIST_HEADER Header);
            STATIC XTFASTCALL CHAR Increment8(IN PCHAR Address);
            STATIC XTFASTCALL SHORT Increment16(IN PSHORT Address);
            STATIC XTFASTCALL LONG Increment32(IN PLONG Address);
            STATIC XTFASTCALL LONG_PTR Increment64(IN PLONG_PTR Address);
            STATIC XTFASTCALL CHAR Or8(IN PCHAR Address,
                                       IN CHAR Mask);
            STATIC XTFASTCALL SHORT Or16(IN PSHORT Address,
                                         IN SHORT Mask);
            STATIC XTFASTCALL LONG Or32(IN PLONG Address,
                                        IN LONG Mask);
            STATIC XTFASTCALL LONG_PTR Or64(IN PLONG_PTR Address,
                                            IN LONG_PTR Mask);
            STATIC XTFASTCALL XTFASTCALL PSINGLE_LIST_ENTRY PopEntrySingleList(IN PSINGLE_LIST_HEADER Header);
            STATIC XTFASTCALL PSINGLE_LIST_ENTRY PushEntrySingleList(IN PSINGLE_LIST_HEADER Header,
                                                                     IN PSINGLE_LIST_ENTRY Entry);
            STATIC XTFASTCALL CHAR Xor8(IN PCHAR Address,
                                        IN CHAR Mask);
            STATIC XTFASTCALL SHORT Xor16(IN PSHORT Address,
                                          IN SHORT Mask);
            STATIC XTFASTCALL LONG Xor32(IN PLONG Address,
                                         IN LONG Mask);
            STATIC XTFASTCALL LONG_PTR Xor64(IN PLONG_PTR Address,
                                             IN LONG_PTR Mask);
    };
}

#endif /* __XTOSKRNL_RTL_ATOMIC_HH */
