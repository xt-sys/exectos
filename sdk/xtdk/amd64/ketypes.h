/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/ketypes.h
 * DESCRIPTION:     Kernel services related structures definitions specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_KETYPES_H
#define __XTDK_AMD64_KETYPES_H

#include <xttypes.h>


/* IOPM Definitions */
#define IO_ACCESS_MAP_NONE                0

/* Static Kernel-Mode address start */
#define KSEG0_BASE                        0xFFFFF80000000000

/* XTOS Kernel address base */
#define KSEG0_KERNEL_BASE                 0x0000000800000000

/* XTOS Kernel stack size */
#define KERNEL_STACK_SIZE                 0x8000

/* Global Descriptor Table (GDT) entry union definition */
typedef union _KGDTENTRY
{
    struct
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
            struct {
                ULONG BaseMiddle:8;
                ULONG Type:5;
                ULONG Dpl:2;
                ULONG Present:1;
                ULONG LimitHigh:4;
                ULONG System:1;
                ULONG LongMode:1;
                ULONG DefaultBig:1;
                ULONG Granularity:1;
                ULONG BaseHigh:8;
            } Bits;
        };
        ULONG BaseUpper;
        ULONG MustBeZero;
    };
    ULONG64 Alignment;
} KGDTENTRY, *PKGDTENTRY;

/* Interrupt Descriptor Table (IDT) entry union definition */
typedef union _KIDTENTRY
{
    struct
    {
        USHORT OffsetLow;
        USHORT Selector;
        USHORT IstIndex:3;
        USHORT Reserved0:5;
        USHORT Type:5;
        USHORT Dpl:2;
        USHORT Present:1;
        USHORT OffsetMiddle;
        ULONG OffsetHigh;
        ULONG Reserved1;
    };
    ULONG64 Alignment;
} KIDTENTRY, *PKIDTENTRY;

/* Task State Segment (TSS) structure definition */
typedef struct _KTSS
{
    ULONG Reserved0;
    ULONG64 Rsp0;
    ULONG64 Rsp1;
    ULONG64 Rsp2;
    ULONG64 Ist[8];
    ULONG64 Reserved1;
    USHORT Reserved2;
    USHORT IoMapBase;
} KTSS, *PKTSS;

#endif /* __XTDK_AMD64_KETYPES_H */
