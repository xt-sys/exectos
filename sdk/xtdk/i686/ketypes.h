/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/ketypes.h
 * DESCRIPTION:     Kernel services related structures definitions specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_KETYPES_H
#define __XTDK_I686_KETYPES_H

#include <xtstruct.h>
#include <xttypes.h>


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
#define KGDT_TYPE_CODE                    (0x10 | KGDT_DESCRIPTOR_CODE | KGDT_DESCRIPTOR_EXECUTE_READ)
#define KGDT_TYPE_DATA                    (0x10 | KGDT_DESCRIPTOR_READ_WRITE)

/* I686 Segment Types */
#define I686_TASK_GATE                    0x5
#define I686_TSS                          0x9
#define I686_ACTIVE_TSS                   0xB
#define I686_CALL_GATE                    0xC
#define I686_INTERRUPT_GATE               0xE
#define I686_TRAP_GATE                    0xF

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

#endif /* __XTDK_I686_KETYPES_H */
