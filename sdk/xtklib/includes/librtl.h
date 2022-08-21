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
#include "xtstruct.h"
#include "xttypes.h"


XTAPI
PVOID
RtlCopyMemory(IN PVOID Destination,
              IN PCVOID Source,
              IN SIZE_T Length);

VOID
RtlInitializeListHead(IN PLIST_ENTRY ListHead);

VOID
RtlInitializeListHead32(IN PLIST_ENTRY32 ListHead);

VOID
RtlInsertHeadList(IN OUT PLIST_ENTRY ListHead,
                  IN OUT PLIST_ENTRY Entry);

VOID
RtlInsertTailList(IN OUT PLIST_ENTRY ListHead,
                  IN OUT PLIST_ENTRY Entry);

BOOLEAN
RtlListEmpty(PLIST_ENTRY ListHead);

XTAPI
SIZE_T
RtlSameMemory(IN PCVOID LeftBuffer,
              IN PCVOID RightBuffer,
              IN SIZE_T Length);

INT
RtlWideStringCompare(IN CONST PWCHAR String1,
                     IN CONST PWCHAR String2,
                     IN CONST ULONG Length);

PWCHAR
RtlWideStringTokenize(IN PWCHAR String,
                      IN CONST PWCHAR Delimiter,
                      IN OUT PWCHAR *SavePtr);

#endif /* __XTKLIB_LIBRTL_H */
