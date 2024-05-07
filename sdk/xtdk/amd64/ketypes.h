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
#include <xtstruct.h>
#include <xttypes.h>
#include <potypes.h>
#include ARCH_HEADER(xtstruct.h)
#include ARCH_HEADER(artypes.h)


/* Selector masks */
#define MODE_MASK                                 0x0001
#define RPL_MASK                                  0x0003

/* GDT selector names */
#define KGDT_NULL                                 0x0000
#define KGDT_R0_CODE                              0x0010
#define KGDT_R0_DATA                              0x0018
#define KGDT_R3_CMCODE                            0x0020
#define KGDT_R3_DATA                              0x0028
#define KGDT_R3_CODE                              0x0030
#define KGDT_SYS_TSS                              0x0040
#define KGDT_R3_CMTEB                             0x0050
#define KGDT_R0_LDT                               0x0060
#define KGDT_ALIAS                                0x0070

/* GDT descriptor privilege levels */
#define KGDT_DPL_SYSTEM                           0
#define KGDT_DPL_USER                             3

/* GDT descriptor properties */
#define KGDT_DESCRIPTOR_ACCESSED                  0x01
#define KGDT_DESCRIPTOR_READ_WRITE                0x02
#define KGDT_DESCRIPTOR_EXECUTE_READ              0x02
#define KGDT_DESCRIPTOR_EXPAND_DOWN               0x04
#define KGDT_DESCRIPTOR_CONFORMING                0x04
#define KGDT_DESCRIPTOR_CODE                      0x08

/* GDT descriptor type codes */
#define KGDT_TYPE_NONE                            0x0
#define KGDT_TYPE_CODE                            (0x10 | KGDT_DESCRIPTOR_CODE | KGDT_DESCRIPTOR_EXECUTE_READ)
#define KGDT_TYPE_DATA                            (0x10 | KGDT_DESCRIPTOR_READ_WRITE)

/* IDT access levels */
#define KIDT_ACCESS_RING0                         0x0
#define KIDT_ACCESS_RING3                         0x3

/* IDT Interrupt Stack Table entries */
#define KIDT_IST_RESERVED                         0
#define KIDT_IST_PANIC                            1
#define KIDT_IST_MCA                              2

/* AMD64 Segment Types */
#define AMD64_TASK_GATE                           0x5
#define AMD64_TSS                                 0x9
#define AMD64_ACTIVE_TSS                          0xB
#define AMD64_CALL_GATE                           0xC
#define AMD64_INTERRUPT_GATE                      0xE
#define AMD64_TRAP_GATE                           0xF

/* EFLAGS bits definitions */
#define EFLAGS_NF_MASK                            0x00000000L /* None */
#define EFLAGS_CF_MASK                            0x00000001L /* Carry */
#define EFLAGS_PF_MASK                            0x00000004L /* Parity */
#define EFLAGS_AF_MASK                            0x00000010L /* Aux Carry */
#define EFLAGS_ZF_MASK                            0x00000040L /* Zero */
#define EFLAGS_SF_MASK                            0x00000080L /* Sign */
#define EFLAGS_TF                                 0x00000100L /* Trap */
#define EFLAGS_INTERRUPT_MASK                     0x00000200L /* Interrupt */
#define EFLAGS_DF_MASK                            0x00000400L /* Direction */
#define EFLAGS_OF_MASK                            0x00000800L /* Overflow */
#define EFLAGS_IOPL_MASK                          0x00003000L /* I/O Privilege */
#define EFLAGS_NT                                 0x00004000L /* Nested Task */
#define EFLAGS_SIGN_MASK                          0x00008000L /* Sign */
#define EFLAGS_RF                                 0x00010000L /* Resume */
#define EFLAGS_V86_MASK                           0x00020000L /* Virtual 8086 */
#define EFLAGS_ALIGN_CHECK                        0x00040000L /* Alignment Check */
#define EFLAGS_VIF                                0x00080000L /* Virtual Interrupt */
#define EFLAGS_VIP                                0x00100000L /* Virtual Interrupt Pending */
#define EFLAGS_ID_MASK                            0x00200000L /* Identification */

/* EFLAGS sanitize masks */
#define EFLAGS_KERNELMODE                         0x00210FD5L
#define EFLAGS_USERMODE                           0x00010DD5L

/* Context control flags */
#define CONTEXT_ARCHITECTURE                      0x00100000
#define CONTEXT_CONTROL                           (CONTEXT_ARCHITECTURE | 0x01)
#define CONTEXT_INTEGER                           (CONTEXT_ARCHITECTURE | 0x02)
#define CONTEXT_SEGMENTS                          (CONTEXT_ARCHITECTURE | 0x04)
#define CONTEXT_FLOATING_POINT                    (CONTEXT_ARCHITECTURE | 0x08)
#define CONTEXT_DEBUG_REGISTERS                   (CONTEXT_ARCHITECTURE | 0x10)

/* Interrupt request levels definitions */
#define PASSIVE_LEVEL                             0
#define LOW_LEVEL                                 0
#define APC_LEVEL                                 1
#define DISPATCH_LEVEL                            2
#define CMC_LEVEL                                 5
#define DEVICE1_LEVEL                             6
#define DEVICE2_LEVEL                             7
#define DEVICE3_LEVEL                             8
#define DEVICE4_LEVEL                             9
#define DEVICE5_LEVEL                             10
#define DEVICE6_LEVEL                             11
#define DEVICE7_LEVEL                             12
#define SYNC_LEVEL                                12
#define CLOCK_LEVEL                               13
#define IPI_LEVEL                                 14
#define DRS_LEVEL                                 14
#define POWER_LEVEL                               14
#define PROFILE_LEVEL                             15
#define HIGH_LEVEL                                15

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

/* XTOS Kernel stack guard pages */
#define KERNEL_STACK_GUARD_PAGES          1

/* Kernel frames */
#define KEXCEPTION_FRAME_SIZE             sizeof(KEXCEPTION_FRAME)
#define KSWITCH_FRAME_SIZE                sizeof(KSWITCH_FRAME)
#define KTRAP_FRAME_ALIGN                 0x10
#define KTRAP_FRAME_SIZE                  sizeof(KTRAP_FRAME)

/* Return address size pushed by 'call' instruction */
#define KRETURN_ADDRESS_SIZE              0x8

/* Size of legacy 387 registers */
#define SIZE_OF_80387_REGISTERS           80

/* NPX state definitions */
#define NPX_STATE_UNUSED                  0x0
#define NPX_STATE_SCRUB                   0x1
#define NPX_STATE_SWITCH                  0x2

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
typedef struct _CONTEXT
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
} ALIGN(16) CONTEXT, *PCONTEXT;

/* Pseudo 64-bit descriptor structure definition */
typedef struct _KDESCRIPTOR
{
    USHORT Pad[3];
    USHORT Limit;
    PVOID Base;
} KDESCRIPTOR, *PKDESCRIPTOR;

/* Global Descriptor Table (GDT) entry union definition */
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
} KGDTENTRY, *PKGDTENTRY;

/* Interrupt Descriptor Table (IDT) entry union definition */
typedef struct _KIDTENTRY
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
} PACK KTSS, *PKTSS;

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

/* Thread start frame definition */
typedef struct _KSTART_FRAME
{
    ULONG64 P1Home;
    ULONG64 P2Home;
    ULONG64 P3Home;
    ULONG64 P4Home;
    ULONG64 Reserved;
    ULONG64 Return;
} KSTART_FRAME, *PKSTART_FRAME;

/* Switch frame definition */
typedef struct _KSWITCH_FRAME
{
    ULONG64 P1Home;
    ULONG64 P2Home;
    ULONG64 P3Home;
    ULONG64 P4Home;
    ULONG64 P5Home;
    ULONG MxCsr;
    KRUNLEVEL ApcBypass;
    UCHAR Reserved[3];
    ULONG64 Rbp;
    ULONG64 Return;
} KSWITCH_FRAME, *PKSWITCH_FRAME;

/* Trap frame definition */
typedef struct _KTRAP_FRAME
{
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
    ULONG MxCsr;
    ULONG PreviousMode;
    ULONGLONG Cr2;
    ULONGLONG Cr3;
    ULONGLONG Dr0;
    ULONGLONG Dr1;
    ULONGLONG Dr2;
    ULONGLONG Dr3;
    ULONGLONG Dr6;
    ULONGLONG Dr7;
    USHORT SegDs;
    USHORT SegEs;
    USHORT SegFs;
    USHORT SegGs;
    ULONGLONG Rax;
    ULONGLONG Rbx;
    ULONGLONG Rcx;
    ULONGLONG Rdx;
    ULONGLONG R8;
    ULONGLONG R9;
    ULONGLONG R10;
    ULONGLONG R11;
    ULONGLONG R12;
    ULONGLONG R13;
    ULONGLONG R14;
    ULONGLONG R15;
    ULONGLONG Rsi;
    ULONGLONG Rdi;
    ULONGLONG Rbp;
    ULONGLONG Vector;
    union {
        ULONGLONG ErrorCode;
        ULONGLONG ExceptionFrame;
    };
    ULONGLONG Rip;
    ULONGLONG SegCs;
    ULONGLONG Flags;
    ULONGLONG Rsp;
    ULONGLONG SegSs;
} KTRAP_FRAME, *PKTRAP_FRAME;

/* Thread initialization frame definition */
typedef struct _KTHREAD_INIT_FRAME
{
    KSWITCH_FRAME SwitchFrame;
    KSTART_FRAME StartFrame;
    KEXCEPTION_FRAME ExceptionFrame;
    KTRAP_FRAME TrapFrame;
    FLOATING_SAVE_AREA NpxFrame;
} KTHREAD_INIT_FRAME, *PKTHREAD_INIT_FRAME;

/* Special kernel registers structure definition */
typedef struct _KSPECIAL_REGISTERS
{
    ULONG64 Cr0;
    ULONG64 Cr2;
    ULONG64 Cr3;
    ULONG64 Cr4;
    ULONG64 KernelDr0;
    ULONG64 KernelDr1;
    ULONG64 KernelDr2;
    ULONG64 KernelDr3;
    ULONG64 KernelDr6;
    ULONG64 KernelDr7;
    KDESCRIPTOR Gdtr;
    KDESCRIPTOR Idtr;
    USHORT Tr;
    USHORT Ldtr;
    ULONG MxCsr;
    ULONG64 DebugControl;
    ULONG64 LastBranchToRip;
    ULONG64 LastBranchFromRip;
    ULONG64 LastExceptionToRip;
    ULONG64 LastExceptionFromRip;
    ULONG64 Cr8;
    ULONG64 MsrGsBase;
    ULONG64 MsrGsSwap;
    ULONG64 MsrStar;
    ULONG64 MsrLStar;
    ULONG64 MsrCStar;
    ULONG64 MsrSyscallMask;
} KSPECIAL_REGISTERS, *PKSPECIAL_REGISTERS;

/* Processor state frame structure definition */
typedef struct _KPROCESSOR_STATE
{
    KSPECIAL_REGISTERS SpecialRegisters;
    CONTEXT ContextFrame;
} KPROCESSOR_STATE, *PKPROCESSOR_STATE;

/* Processor Control Block (PRCB) structure definition */
typedef struct _KPROCESSOR_CONTROL_BLOCK
{
    ULONG MxCsr;
    UCHAR Number;
    PKTHREAD CurrentThread;
    PKTHREAD IdleThread;
    PKTHREAD NextThread;
    ULONG64 RspBase;
    ULONG_PTR SetMember;
    CPU_IDENTIFICATION CpuId;
    KPROCESSOR_STATE ProcessorState;
    KSPIN_LOCK_QUEUE LockQueue[MaximumLock];
    KDPC_DATA DpcData[2];
    PVOID DpcStack;
    VOLATILE BOOLEAN DpcRoutineActive;
    VOLATILE ULONG_PTR TimerRequest;
    ULONG_PTR MultiThreadProcessorSet;
    SINGLE_LIST_ENTRY DeferredReadyListHead;
    PROCESSOR_POWER_STATE PowerState;
} KPROCESSOR_CONTROL_BLOCK, *PKPROCESSOR_CONTROL_BLOCK;

/* Processor Block structure definition */
typedef struct _KPROCESSOR_BLOCK
{
    union
    {
        THREAD_INFORMATION_BLOCK ThreadInformationBlock;
        struct
        {
            PKGDTENTRY GdtBase;
            PKTSS TssBase;
            PKPROCESSOR_BLOCK Self;
            PKPROCESSOR_CONTROL_BLOCK CurrentPrcb;
        };
    };
    PKIDTENTRY IdtBase;
    KRUNLEVEL RunLevel;
    KPROCESSOR_CONTROL_BLOCK Prcb;
    ULONG Irr;
    ULONG IrrActive;
    ULONG Idr;
    ULONG ContextSwitches;
    KAFFINITY SetMember;
    ULONG StallScaleFactor;
    UCHAR CpuNumber;
} KPROCESSOR_BLOCK, *PKPROCESSOR_BLOCK;

/* Thread Environment Block (TEB) structure definition */
typedef struct _THREAD_ENVIRONMENT_BLOCK
{
    THREAD_INFORMATION_BLOCK InformationBlock;
} THREAD_ENVIRONMENT_BLOCK, *PTHREAD_ENVIRONMENT_BLOCK;

#endif /* __XTDK_AMD64_KETYPES_H */
