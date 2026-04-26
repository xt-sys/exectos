/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/rtl/i686/intrin.hh
 * DESCRIPTION:     Compiler intrinsic support routines
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_RTL_I686_INTRIN_HH
#define __XTOSKRNL_RTL_I686_INTRIN_HH

#include <xtos.hh>


/* Forward declarations enforcing the XTAPI calling convention and preserving the raw unmangled symbol names */
XTCLINK XTAPI LONGLONG _alldiv(IN LONGLONG Dividend, IN LONGLONG Divisor) XTSYMBOL("__alldiv");
XTCLINK XTAPI LONGLONG _alldvrm(IN LONGLONG Dividend, IN LONGLONG Divisor, OUT PLONGLONG Remainder) XTSYMBOL("__alldvrm");
XTCLINK XTAPI LONGLONG _allrem(IN LONGLONG Dividend, IN LONGLONG Divisor) XTSYMBOL("__allrem");
XTCLINK XTAPI ULONGLONG _aulldiv(IN ULONGLONG Dividend, IN ULONGLONG Divisor) XTSYMBOL("__aulldiv");
XTCLINK XTAPI ULONGLONG _aulldvrm(IN ULONGLONG Dividend, IN ULONGLONG Divisor, OUT PULONGLONG Remainder) XTSYMBOL("__aulldvrm");
XTCLINK XTAPI ULONGLONG _aullrem(IN ULONGLONG Dividend, IN ULONGLONG Divisor) XTSYMBOL("__aullrem");

#endif /* __XTOSKRNL_RTL_I686_INTRIN_HH */
