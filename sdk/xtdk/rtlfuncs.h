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

XTCDECL
VOID
RtlRemoveEntryList(IN PLIST_ENTRY Entry);


/* Runtime Library routines forward references */
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

XTAPI
VOID
RtlMoveMemory(OUT PVOID Destination,
              IN PCVOID Source,
              IN SIZE_T Length);

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
