/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/rtltypes.h
 * DESCRIPTION:     Runtime library structures definitions for AMD64 architecture
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTDK_AMD64_RTLTYPES_H
#define __XTDK_AMD64_RTLTYPES_H

#include <xtbase.h>
#include <rtltypes.h>


/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* Header for a sequenced single linked list union definition */
typedef union _SINGLE_LIST_HEADER
{
    struct
    {
        ULONGLONG Alignment;
        ULONGLONG Region;
    };
    struct
    {
        ULONGLONG Depth:16;
        ULONGLONG Sequence:9;
        ULONGLONG NextEntry:39;
        ULONGLONG HeaderType:1;
        ULONGLONG Init:1;
        ULONGLONG Reserved:59;
        ULONGLONG Region:3;
    } Header8;
    struct
    {
        ULONGLONG Depth:16;
        ULONGLONG Sequence:48;
        ULONGLONG HeaderType:1;
        ULONGLONG Init:1;
        ULONGLONG Reserved:2;
        ULONGLONG NextEntry:60;
    } Header16;
    struct
    {
        ULONGLONG Depth:16;
        ULONGLONG Sequence:48;
        ULONGLONG HeaderType:1;
        ULONGLONG Reserved:3;
        ULONGLONG NextEntry:60;
    } Header64;
} SINGLE_LIST_HEADER, *PSINGLE_LIST_HEADER;

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_AMD64_RTLTYPES_H */
