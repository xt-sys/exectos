/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/rtltypes.h
 * DESCRIPTION:     Runtime library structures definitions for AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_RTLTYPES_H
#define __XTDK_AMD64_RTLTYPES_H

#include <xtbase.h>
#include <rtltypes.h>


/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* Header for a sequenced single linked list structure definition */
typedef struct _SINGLE_LIST_HEADER
{
    ULONGLONG Alignment;
    ULONGLONG Region;
} SINGLE_LIST_HEADER, *PSINGLE_LIST_HEADER;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_AMD64_RTLTYPES_H */
