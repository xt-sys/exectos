/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/ketypes.h
 * DESCRIPTION:     Kernel services related structures definitions specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_KETYPES_H
#define __XTDK_AMD64_KETYPES_H

#include <xtbase.h>
#include <xttypes.h>


/* Size of the exception area */
#define EXCEPTION_AREA_SIZE               64

/* IOPM Definitions */
#define IO_ACCESS_MAP_NONE                0

/* Size of the FNSAVE/FRSTOR save area */
#define FLOATING_SAVE_AREA_SIZE           ((sizeof(FLOATING_SAVE_AREA) + 15) & ~15)

/* Static Kernel-Mode address start */
#define KSEG0_BASE                        0xFFFFF80000000000

/* XTOS Kernel address base */
#define KSEG0_KERNEL_BASE                 0x0000000800000000

/* XTOS Kernel stack size */
#define KERNEL_STACK_SIZE                 0x8000

/* Kernel frames */
#define KEXCEPTION_FRAME_SIZE             sizeof(KEXCEPTION_FRAME)
#define KSWITCH_FRAME_SIZE                sizeof(KSWITCH_FRAME)
#define KTRAP_FRAME_ALIGN                 0x10
#define KTRAP_FRAME_SIZE                  sizeof(KTRAP_FRAME)

/* Size of legacy 387 registers */
#define SIZE_OF_80387_REGISTERS           80

/* Floating point state storing structure */
typedef struct _FLOATING_SAVE_AREA
{
    USHORT ControlWord;
    USHORT StatusWord;
    USHORT TagWord;
    ULONG ErrorOffset;
    USHORT ErrorSelector;
    USHORT ErrorOpcode;
    ULONG DataOffset;
    USHORT DataSelector;
    UCHAR RegisterArea[SIZE_OF_80387_REGISTERS];
} FLOATING_SAVE_AREA, *PFLOATING_SAVE_AREA;

/* Context frame structure definition */
typedef struct ALIGN(16) _CONTEXT
{
    ULONG64 P1Home;
    ULONG64 P2Home;
    ULONG64 P3Home;
    ULONG64 P4Home;
    ULONG64 P5Home;
    ULONG64 P6Home;
    ULONG ContextFlags;
    ULONG MxCsr;
    USHORT SegCs;
    USHORT SegDs;
    USHORT SegEs;
    USHORT SegFs;
    USHORT SegGs;
    USHORT SegSs;
    ULONG EFlags;
    ULONG64 Dr0;
    ULONG64 Dr1;
    ULONG64 Dr2;
    ULONG64 Dr3;
    ULONG64 Dr6;
    ULONG64 Dr7;
    ULONG64 Rax;
    ULONG64 Rcx;
    ULONG64 Rdx;
    ULONG64 Rbx;
    ULONG64 Rsp;
    ULONG64 Rbp;
    ULONG64 Rsi;
    ULONG64 Rdi;
    ULONG64 R8;
    ULONG64 R9;
    ULONG64 R10;
    ULONG64 R11;
    ULONG64 R12;
    ULONG64 R13;
    ULONG64 R14;
    ULONG64 R15;
    ULONG64 Rip;
    union
    {
        FLOATING_SAVE_AREA FloatSave;
        struct
        {
            M128 Header[2];
            M128 Legacy[8];
            M128 Xmm0;
            M128 Xmm1;
            M128 Xmm2;
            M128 Xmm3;
            M128 Xmm4;
            M128 Xmm5;
            M128 Xmm6;
            M128 Xmm7;
            M128 Xmm8;
            M128 Xmm9;
            M128 Xmm10;
            M128 Xmm11;
            M128 Xmm12;
            M128 Xmm13;
            M128 Xmm14;
            M128 Xmm15;
        };
    };
    M128 VectorRegister[26];
    ULONG64 VectorControl;
    ULONG64 DebugControl;
    ULONG64 LastBranchToRip;
    ULONG64 LastBranchFromRip;
    ULONG64 LastExceptionToRip;
    ULONG64 LastExceptionFromRip;
} CONTEXT, *PCONTEXT;

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

/* Exception frame definition */
typedef struct _KEXCEPTION_FRAME
{
    ULONG64 P1Home;
    ULONG64 P2Home;
    ULONG64 P3Home;
    ULONG64 P4Home;
    ULONG64 P5;
    ULONG64 InitialStack;
    M128 Xmm6;
    M128 Xmm7;
    M128 Xmm8;
    M128 Xmm9;
    M128 Xmm10;
    M128 Xmm11;
    M128 Xmm12;
    M128 Xmm13;
    M128 Xmm14;
    M128 Xmm15;
    ULONG64 TrapFrame;
    ULONG64 CallbackStack;
    ULONG64 OutputBuffer;
    ULONG64 OutputLength;
    UCHAR ExceptionRecord[EXCEPTION_AREA_SIZE];
    ULONG64 MxCsr;
    ULONG64 Rbp;
    ULONG64 Rbx;
    ULONG64 Rdi;
    ULONG64 Rsi;
    ULONG64 R12;
    ULONG64 R13;
    ULONG64 R14;
    ULONG64 R15;
    ULONG64 Return;
} KEXCEPTION_FRAME, *PKEXCEPTION_FRAME;

/* Switch frame definition */
typedef struct _KSWITCH_FRAME
{
    ULONG64 P1Home;
    ULONG64 P2Home;
    ULONG64 P3Home;
    ULONG64 P4Home;
    ULONG64 P5Home;
    ULONG MxCsr;
    UCHAR ApcBypass;
    UCHAR Fill1[3];
    ULONG64 Rbp;
    ULONG64 Return;
} KSWITCH_FRAME, *PKSWITCH_FRAME;

/* Trap frame definition */
typedef struct _KTRAP_FRAME
{
    ULONG64 P1Home;
    ULONG64 P2Home;
    ULONG64 P3Home;
    ULONG64 P4Home;
    ULONG64 P5;
    CHAR PreviousMode;
    UCHAR PreviousIrql;
    UCHAR FaultIndicator;
    UCHAR ExceptionActive;
    ULONG MxCsr;
    ULONG64 Rax;
    ULONG64 Rcx;
    ULONG64 Rdx;
    ULONG64 R8;
    ULONG64 R9;
    ULONG64 R10;
    ULONG64 R11;
    union
    {
        ULONG64 GsBase;
        ULONG64 GsSwap;
    };
    M128 Xmm0;
    M128 Xmm1;
    M128 Xmm2;
    M128 Xmm3;
    M128 Xmm4;
    M128 Xmm5;
    union
    {
        ULONG64 FaultAddress;
        ULONG64 ContextRecord;
        ULONG64 TimeStampCKCL;
    };
    ULONG64 Dr0;
    ULONG64 Dr1;
    ULONG64 Dr2;
    ULONG64 Dr3;
    ULONG64 Dr6;
    ULONG64 Dr7;
    union
    {
        struct
        {
            ULONG64 DebugControl;
            ULONG64 LastBranchToRip;
            ULONG64 LastBranchFromRip;
            ULONG64 LastExceptionToRip;
            ULONG64 LastExceptionFromRip;
        };
        struct
        {
            ULONG64 LastBranchControl;
            ULONG LastBranchMSR;
        };
    };
    USHORT SegDs;
    USHORT SegEs;
    USHORT SegFs;
    USHORT SegGs;
    ULONG64 TrapFrame;
    ULONG64 Rbx;
    ULONG64 Rdi;
    ULONG64 Rsi;
    ULONG64 Rbp;
    union
    {
        ULONG64 ErrorCode;
        ULONG64 ExceptionFrame;
        ULONG64 TimeStampKlog;
    };
    ULONG64 Rip;
    USHORT SegCs;
    UCHAR Fill0;
    UCHAR Logging;
    USHORT Fill1[2];
    ULONG EFlags;
    ULONG Fill2;
    ULONG64 Rsp;
    USHORT SegSs;
    USHORT Fill3;
    ULONG CodePatchCycle;
} KTRAP_FRAME, *PKTRAP_FRAME;

#endif /* __XTDK_AMD64_KETYPES_H */
