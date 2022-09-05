/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/rtlfuncs.h
 * DESCRIPTION:     XT runtime library routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_RTLFUNCS_H
#define __XTDK_RTLFUNCS_H

#include "xtdefs.h"
#include "xtstruct.h"
#include "xttypes.h"


XTAPI
VOID
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
BOOLEAN
RtlSameMemory(IN PCVOID LeftBuffer,
              IN PCVOID RightBuffer,
              IN SIZE_T Length);

XTCDECL
INT
RtlWideStringCompare(IN CONST PWCHAR String1,
                     IN CONST PWCHAR String2,
                     IN CONST ULONG Length);

XTCDECL
PWCHAR
RtlWideStringTokenize(IN PWCHAR String,
                      IN CONST PWCHAR Delimiter,
                      IN OUT PWCHAR *SavePtr);

#endif /* __XTDK_RTLFUNCS_H */
