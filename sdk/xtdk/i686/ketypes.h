/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/ketypes.h
 * DESCRIPTION:     Kernel services related structures definitions specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_KETYPES_H
#define __XTDK_I686_KETYPES_H

#include <xttypes.h>


/* IOPM Definitions */
#define IOPM_COUNT                        1
#define IOPM_SIZE                         8192
#define IOPM_FULL_SIZE                    8196
#define IOPM_ACCESS_MAP_NONE              0
#define IOPM_DIRECTION_MAP_SIZE           32

/* Static Kernel-Mode address start */
#define KSEG0_BASE                        0x80000000

/* XTOS Kernel address base */
#define KSEG0_KERNEL_BASE                 0x01800000

/* XTOS Kernel stack size */
#define KERNEL_STACK_SIZE                 0x4000

/* Global Descriptor Table (GDT) entry structure definition */
typedef struct _KGDTENTRY
{
    USHORT LimitLow;
    USHORT BaseLow;
    union
    {
        struct
        {
            UCHAR BaseMiddle;
            UCHAR Flags1;
            UCHAR Flags2;
            UCHAR BaseHigh;
        } Bytes;
        struct
        {
            ULONG BaseMid:8;
            ULONG Type:5;
            ULONG Dpl:2;
            ULONG Present:1;
            ULONG LimitHigh:4;
            ULONG System:1;
            ULONG Reserved0:1;
            ULONG DefaultBig:1;
            ULONG Granularity:1;
            ULONG BaseHigh:8;
        } Bits;
    } HighWord;
} KGDTENTRY, *PKGDTENTRY;

/* Interrupt Descriptor Table (IDT) entry structure definition */
typedef struct _KIDTENTRY {
    USHORT Offset;
    USHORT Selector;
    USHORT Access;
    USHORT ExtendedOffset;
} KIDTENTRY, *PKIDTENTRY;

/* Interrupt direction access map structure definition */
typedef struct _KIIO_ACCESS_MAP {
    UCHAR DirectionMap[IOPM_DIRECTION_MAP_SIZE];
    UCHAR IoMap[IOPM_FULL_SIZE];
} KIIO_ACCESS_MAP, *PKIIO_ACCESS_MAP;

/* Task State Segment (TSS) structure definition */
typedef struct _KTSS {
    USHORT Backlink;
    USHORT Reserved0;
    ULONG Esp0;
    USHORT Ss0;
    USHORT Reserved1;
    ULONG NotUsed1[4];
    ULONG CR3;
    ULONG Eip;
    ULONG EFlags;
    ULONG Eax;
    ULONG Ecx;
    ULONG Edx;
    ULONG Ebx;
    ULONG Esp;
    ULONG Ebp;
    ULONG Esi;
    ULONG Edi;
    USHORT Es;
    USHORT Reserved2;
    USHORT Cs;
    USHORT Reserved3;
    USHORT Ss;
    USHORT Reserved4;
    USHORT Ds;
    USHORT Reserved5;
    USHORT Fs;
    USHORT Reserved6;
    USHORT Gs;
    USHORT Reserved7;
    USHORT LDT;
    USHORT Reserved8;
    USHORT Flags;
    USHORT IoMapBase;
    KIIO_ACCESS_MAP IoMaps[IOPM_COUNT];
    UCHAR IntDirectionMap[IOPM_DIRECTION_MAP_SIZE];
} KTSS, *PKTSS;

#endif /* __XTDK_I686_KETYPES_H */
