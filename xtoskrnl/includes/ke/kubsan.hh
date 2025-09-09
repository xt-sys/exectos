/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/kubsan.hh
 * DESCRIPTION:     Kernel Undefined Behaviour Sanitizer (UBSAN) error reporting handler
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_KUBSAN_HH
#define __XTOSKRNL_KE_KUBSAN_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class KUbsan
    {
        private:
            STATIC BOOLEAN ActiveFrame;

        public:
            STATIC XTCDECL VOID HandleDivisionOverflow(PKUBSAN_OVERFLOW_DATA Data,
                                                       PVOID Lhs,
                                                       PVOID Rhs);
            STATIC XTCDECL VOID HandleFloatCastOverflow(PKUBSAN_FLOAT_CAST_OVERFLOW_DATA Data,
                                                        ULONG_PTR Lhs,
                                                        ULONG_PTR Rhs);
            STATIC XTCDECL VOID HandleFunctionTypeMismatch(PKUBSAN_FUNCTION_TYPE_MISMATCH_DATA Data,
                                                           ULONG_PTR Pointer);
            STATIC XTCDECL VOID HandleIntegerOverflow(PKUBSAN_OVERFLOW_DATA Data,
                                                      ULONG_PTR Lhs,
                                                      ULONG_PTR Rhs);
            STATIC XTCDECL VOID HandleInvalidBuiltin(PKUBSAN_INVALID_BUILTIN_DATA Data);
            STATIC XTCDECL VOID HandleMisalignedAccess(PKUBSAN_TYPE_MISMATCH_DATA Data,
                                                       ULONG_PTR Pointer);
            STATIC XTCDECL VOID HandleNegateOverflow(PKUBSAN_OVERFLOW_DATA Data,
                                                     ULONG_PTR OldValue);
            STATIC XTCDECL VOID HandleNullPointerDereference(PKUBSAN_TYPE_MISMATCH_DATA Data);
            STATIC XTCDECL VOID HandleObjectSizeMismatch(PKUBSAN_TYPE_MISMATCH_DATA Data,
                                                         ULONG_PTR Pointer);
            STATIC XTCDECL VOID HandleOutOfBounds(PKUBSAN_OUT_OF_BOUNDS_DATA Data,
                                                  ULONG_PTR Index);
            STATIC XTCDECL VOID HandlePointerOverflow(PKUBSAN_OVERFLOW_DATA Data,
                                                      ULONG_PTR Lhs,
                                                      ULONG_PTR Rhs);
            STATIC XTCDECL VOID HandleShiftOutOfBounds(PKUBSAN_SHIFT_OUT_OF_BOUNDS_DATA Data,
                                                       ULONG_PTR Lhs,
                                                       ULONG_PTR Rhs);
            STATIC XTCDECL VOID HandleTypeMismatch(PKUBSAN_TYPE_MISMATCH_DATA Data,
                                                   ULONG_PTR Pointer);

        private:
            STATIC XTCDECL BOOLEAN CheckReport(PKUBSAN_SOURCE_LOCATION Location);
            STATIC XTCDECL VOID EnterFrame(PKUBSAN_SOURCE_LOCATION Location,
                                           PCCHAR Reason);
            STATIC XTCDECL LONGLONG GetSignedValue(PKUBSAN_TYPE_DESCRIPTOR Type,
                                                   PVOID Value);
            STATIC XTCDECL PCCHAR GetTypeKind(UCHAR TypeCheckKind);
            STATIC XTCDECL ULONGLONG GetUnsignedValue(PKUBSAN_TYPE_DESCRIPTOR Type,
                                                      PVOID Value);
            STATIC XTCDECL VOID LeaveFrame();
    };
}

#endif /* __XTOSKRNL_KE_KUBSAN_HH */
