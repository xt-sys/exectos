/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/ketypes.h
 * DESCRIPTION:     Kernel services related structures definitions specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_KETYPES_H
#define __XTDK_I686_KETYPES_H

#include <xtbase.h>
#include <xtstruct.h>
#include <xttypes.h>
#include ARCH_HEADER(xtstruct.h)


/* Selector masks */
#define MODE_MASK                         0x0001
#define RPL_MASK                          0x0003

/* GDT selector names */
#define KGDT_NULL                         0x0000
#define KGDT_R0_CODE                      0x0008
#define KGDT_R0_DATA                      0x0010
#define KGDT_R3_CODE                      0x0018
#define KGDT_R3_DATA                      0x0020
#define KGDT_SYS_TSS                      0x0028
#define KGDT_R0_PCR                       0x0030
#define KGDT_R3_TEB                       0x0038
#define KGDT_VDM_TILE                     0x0040
#define KGDT_R0_LDT                       0x0048
#define KGDT_DF_TSS                       0x0050
#define KGDT_NMI_TSS                      0x0058
#define KGDT_VDBS                         0x0068

/* GDT descriptor privilege levels */
#define KGDT_DPL_SYSTEM                   0
#define KGDT_DPL_USER                     3

/* GDT descriptor properties */
#define KGDT_DESCRIPTOR_ACCESSED          0x01
#define KGDT_DESCRIPTOR_READ_WRITE        0x02
#define KGDT_DESCRIPTOR_EXECUTE_READ      0x02
#define KGDT_DESCRIPTOR_EXPAND_DOWN       0x04
#define KGDT_DESCRIPTOR_CONFORMING        0x04
#define KGDT_DESCRIPTOR_CODE              0x08

/* GDT descriptor type codes */
#define KGDT_TYPE_NONE                    0x0
#define KGDT_TYPE_CODE                    (0x10 | KGDT_DESCRIPTOR_CODE | KGDT_DESCRIPTOR_EXECUTE_READ)
#define KGDT_TYPE_DATA                    (0x10 | KGDT_DESCRIPTOR_READ_WRITE)

/* IDT access levels */
#define KIDT_ACCESS_RING0                 0x00
#define KIDT_ACCESS_RING3                 0x60

/* IDT gate types */
#define KIDT_TYPE_TASK                    0x05
#define KIDT_TYPE_CALL                    0x0C
#define KIDT_TYPE_INTERRUPT               0x0E
#define KIDT_TYPE_TRAP                    0x0F

/* TSS Offsets */
#define KTSS_ESP0                         0x04
#define KTSS_CR3                          0x1C
#define KTSS_EIP                          0x20
#define KTSS_EFLAGS                       0x24
#define KTSS_EAX                          0x28
#define KTSS_ECX                          0x2C
#define KTSS_EDX                          0x30
#define KTSS_EBX                          0x34
#define KTSS_ESP                          0x38
#define KTSS_EBP                          0x3C
#define KTSS_ESI                          0x40
#define KTSS_EDI                          0x44
#define KTSS_ES                           0x48
#define KTSS_CS                           0x4C
#define KTSS_SS                           0x50
#define KTSS_DS                           0x54
#define KTSS_FS                           0x58
#define KTSS_GS                           0x5C
#define KTSS_LDT                          0x60
#define KTSS_IOMAPBASE                    0x66
#define KTSS_IO_MAPS                      0x68

/* I686 Segment Types */
#define I686_TASK_GATE                    0x5
#define I686_TSS                          0x9
#define I686_ACTIVE_TSS                   0xB
#define I686_CALL_GATE                    0xC
#define I686_INTERRUPT_GATE               0xE
#define I686_TRAP_GATE                    0xF

/* Interrupt request levels definitions */
#define PASSIVE_LEVEL                     0
#define LOW_LEVEL                         0
#define APC_LEVEL                         1
#define DISPATCH_LEVEL                    2
#define CMCI_LEVEL                        5
#define PROFILE_LEVEL                     27
#define SYNC_LEVEL                        27
#define CLOCK_LEVEL                       28
#define IPI_LEVEL                         29
#define POWER_LEVEL                       30
#define HIGH_LEVEL                        31

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

/* Kernel frames */
#define KTRAP_FRAME_ALIGN                 0x08
#define KTRAP_FRAME_SIZE                  sizeof(KTRAP_FRAME)
#define NPX_FRAME_SIZE                    0x210

/* Number of supported extensions */
#define MAXIMUM_SUPPORTED_EXTENSION       512

/* Size of 387 registers */
#define SIZE_OF_80387_REGISTERS           80

/* Floating point state storing structure */
typedef struct _FLOATING_SAVE_AREA
{
    ULONG ControlWord;
    ULONG StatusWord;
    ULONG TagWord;
    ULONG ErrorOffset;
    ULONG ErrorSelector;
    ULONG DataOffset;
    ULONG DataSelector;
    UCHAR RegisterArea[SIZE_OF_80387_REGISTERS];
    ULONG Cr0NpxState;
} FLOATING_SAVE_AREA, *PFLOATING_SAVE_AREA;

/* Context frame structure definition */
typedef struct _CONTEXT
{
    ULONG ContextFlags;
    ULONG Dr0;
    ULONG Dr1;
    ULONG Dr2;
    ULONG Dr3;
    ULONG Dr6;
    ULONG Dr7;
    FLOATING_SAVE_AREA FloatSave;
    ULONG SegGs;
    ULONG SegFs;
    ULONG SegEs;
    ULONG SegDs;
    ULONG Edi;
    ULONG Esi;
    ULONG Ebx;
    ULONG Edx;
    ULONG Ecx;
    ULONG Eax;
    ULONG Ebp;
    ULONG Eip;
    ULONG SegCs;
    ULONG EFlags;
    ULONG Esp;
    ULONG SegSs;
    UCHAR ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];
} CONTEXT, *PCONTEXT;

/* Pseudo descriptor structure definition */
typedef struct _KDESCRIPTOR
{
    USHORT Pad;
    USHORT Limit;
    PVOID Base;
} KDESCRIPTOR, *PKDESCRIPTOR;

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
            ULONG BaseMiddle:8;
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
    };
} KGDTENTRY, *PKGDTENTRY;

/* Interrupt Descriptor Table (IDT) entry structure definition */
typedef struct _KIDTENTRY
{
    USHORT Offset;
    USHORT Selector;
    USHORT Access;
    USHORT ExtendedOffset;
} KIDTENTRY, *PKIDTENTRY;

/* Interrupt direction access map structure definition */
typedef struct _KIIO_ACCESS_MAP
{
    UCHAR DirectionMap[IOPM_DIRECTION_MAP_SIZE];
    UCHAR IoMap[IOPM_FULL_SIZE];
} KIIO_ACCESS_MAP, *PKIIO_ACCESS_MAP;

/* Task State Segment (TSS) structure definition */
typedef struct _KTSS
{
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

/* Exception frame definition (not available on ia32) */
typedef struct _KEXCEPTION_FRAME
{
    ULONG PlaceHolder;
} KEXCEPTION_FRAME, *PKEXCEPTION_FRAME;

/* Trap frame definition */
typedef struct _KTRAP_FRAME
{
    ULONG DbgEbp;
    ULONG DbgEip;
    ULONG DbgArgMark;
    ULONG DbgArgPointer;
    ULONG TempSegCs;
    ULONG TempEsp;
    ULONG Dr0;
    ULONG Dr1;
    ULONG Dr2;
    ULONG Dr3;
    ULONG Dr6;
    ULONG Dr7;
    ULONG SegGs;
    ULONG SegEs;
    ULONG SegDs;
    ULONG Edx;
    ULONG Ecx;
    ULONG Eax;
    ULONG PreviousPreviousMode;
    PEXCEPTION_REGISTRATION_RECORD ExceptionList;
    ULONG SegFs;
    ULONG Edi;
    ULONG Esi;
    ULONG Ebx;
    ULONG Ebp;
    ULONG ErrCode;
    ULONG Eip;
    ULONG SegCs;
    ULONG EFlags;
    ULONG HardwareEsp;
    ULONG HardwareSegSs;
    ULONG V86Es;
    ULONG V86Ds;
    ULONG V86Fs;
    ULONG V86Gs;
} KTRAP_FRAME, *PKTRAP_FRAME;

/* Special kernel registers structure definition */
typedef struct _KSPECIAL_REGISTERS
{
    ULONG Cr0;
    ULONG Cr2;
    ULONG Cr3;
    ULONG Cr4;
    ULONG KernelDr0;
    ULONG KernelDr1;
    ULONG KernelDr2;
    ULONG KernelDr3;
    ULONG KernelDr6;
    ULONG KernelDr7;
    KDESCRIPTOR Gdtr;
    KDESCRIPTOR Idtr;
    USHORT Tr;
    USHORT Ldtr;
    ULONG Reserved[6];
} KSPECIAL_REGISTERS, *PKSPECIAL_REGISTERS;

/* Processor state frame structure definition */
typedef struct _KPROCESSOR_STATE
{
    CONTEXT ContextFrame;
    KSPECIAL_REGISTERS SpecialRegisters;
} KPROCESSOR_STATE, *PKPROCESSOR_STATE;

/* Processor Control Block (PRCB) structure definition */
typedef struct _KPROCESSOR_CONTROL_BLOCK
{
    UCHAR Number;
    ULONG_PTR SetMember;
    KPROCESSOR_STATE ProcessorState;
    ULONG_PTR MultiThreadProcessorSet;
    PVOID DpcStack;
} KPROCESSOR_CONTROL_BLOCK, *PKPROCESSOR_CONTROL_BLOCK;

/* Processor Block structure definition */
typedef struct _KPROCESSOR_BLOCK
{
    PKPROCESSOR_BLOCK Self;
    PKPROCESSOR_CONTROL_BLOCK CurrentPrcb;
    KIRQL Irql;
    PKIDTENTRY IdtBase;
    PKGDTENTRY GdtBase;
    PKTSS TssBase;
    KPROCESSOR_CONTROL_BLOCK Prcb;
} KPROCESSOR_BLOCK, *PKPROCESSOR_BLOCK;

#endif /* __XTDK_I686_KETYPES_H */
