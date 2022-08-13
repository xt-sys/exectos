/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtklib/includes/librtl.h
 * DESCRIPTION:     Kernel mode runtime library
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTKLIB_LIBRTL_H
#define __XTKLIB_LIBRTL_H

#include "xtdefs.h"
#include "xttypes.h"


UINT64
RtlWideStringCompare(IN CONST PWCHAR String1,
                     IN CONST PWCHAR String2,
                     IN CONST ULONG Length);

PWCHAR
RtlWideStringTokenize(IN PWCHAR String,
                      IN CONST PWCHAR Delimiter,
                      IN OUT PWCHAR *SavePtr);

#endif /* __XTKLIB_LIBRTL_H */
