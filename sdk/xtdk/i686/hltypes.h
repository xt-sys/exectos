/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/hltypes.h
 * DESCRIPTION:     XT hardware abstraction layer structures definitions specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_HLTYPES_H
#define __XTDK_I686_HLTYPES_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>
#include ARCH_HEADER(xtstruct.h)


/* APIC base addresses */
#define APIC_BASE                                       0xFFFE0000
#define APIC_LAPIC_MSR_BASE                             0x0000001B
#define APIC_X2APIC_MSR_BASE                            0x00000800

/* APIC vector definitions */
#define APIC_VECTOR_ZERO                                0x00
#define APIC_VECTOR_SPURIOUS                            0x1F
#define APIC_VECTOR_APC                                 0x3D
#define APIC_VECTOR_DPC                                 0x41
#define APIC_VECTOR_REBOOT                              0x50
#define APIC_VECTOR_DEVICE1                             0x51
#define APIC_VECTOR_DEVICE2                             0x61
#define APIC_VECTOR_DEVICE3                             0x71
#define APIC_VECTOR_DEVICE4                             0x81
#define APIC_VECTOR_DEVICE5                             0x91
#define APIC_VECTOR_DEVICE6                             0xA1
#define APIC_VECTOR_DEVICE7                             0xB1
#define APIC_VECTOR_GENERIC                             0xC1
#define APIC_VECTOR_SYNC                                0xC1
#define APIC_VECTOR_CLOCK                               0xD1
#define APIC_VECTOR_IPI                                 0xE1
#define APIC_VECTOR_ERROR                               0xE3
#define APIC_VECTOR_POWERFAIL                           0xEF
#define APIC_VECTOR_PROFILE                             0xFD
#define APIC_VECTOR_PERF                                0xFE
#define APIC_VECTOR_NMI                                 0xFF

/* APIC destination formats */
#define APIC_DF_FLAT                                    0xFFFFFFFF
#define APIC_DF_CLUSTER                                 0x0FFFFFFF

/* APIC delivery modes */
#define APIC_DM_FIXED                                   0x0
#define APIC_DM_LOWPRIO                                 0x1
#define APIC_DM_SMI                                     0x2
#define APIC_DM_REMOTE                                  0x3
#define APIC_DM_NMI                                     0x4
#define APIC_DM_INIT                                    0x5
#define APIC_DM_STARTUP                                 0x6
#define APIC_DM_EXTINT                                  0x7

/* APIC trigger modes */
#define APIC_TGM_EDGE                                   0
#define APIC_TGM_LEVEL                                  1

/* APIC LDR (Logical Destination Register) shifts */
#define APIC_X2APIC_LDR_SHIFT                           16
#define APIC_XAPIC_LDR_SHIFT                            24

/* Maximum number of I/O APICs */
#define APIC_MAX_IOAPICS                                64

/* 8259/ISP PIC ports definitions */
#define PIC1_CONTROL_PORT                               0x20
#define PIC1_DATA_PORT                                  0x21
#define PIC1_ELCR_PORT                                  0x04D0
#define PIC2_CONTROL_PORT                               0xA0
#define PIC2_DATA_PORT                                  0xA1
#define PIC2_ELCR_PORT                                  0x04D1

/* PIC vector definitions */
#define PIC1_VECTOR_SPURIOUS                            0x37

/* Serial ports information */
#define COMPORT_ADDRESS                                 {0x3F8, 0x2F8, 0x3E8, 0x2E8, 0x5F8, 0x4F8, 0x5E8, 0x4E8}
#define COMPORT_COUNT                                   8

/* Initial stall factor */
#define INITIAL_STALL_FACTOR                            100

/* APIC Register Address Map */
typedef enum _APIC_REGISTER
{
    APIC_ID       = 0x02, /* APIC ID Register */
    APIC_VER      = 0x03, /* APIC Version Register */
    APIC_TPR      = 0x08, /* Task Priority Register */
    APIC_APR      = 0x09, /* Arbitration Priority Register */
    APIC_PPR      = 0x0A, /* Processor Priority Register (R) */
    APIC_EOI      = 0x0B, /* EOI Register */
    APIC_RRR      = 0x0C, /* Remote Read Register */
    APIC_LDR      = 0x0D, /* Logical Destination Register */
    APIC_DFR      = 0x0E, /* Destination Format Register (not available in extended mode) */
    APIC_SIVR     = 0x0F, /* Spurious Interrupt Vector Register */
    APIC_ISR      = 0x10, /* Interrupt Service Register*/
    APIC_TMR      = 0x18, /* Trigger Mode Register */
    APIC_IRR      = 0x20, /* Interrupt Request Register */
    APIC_ESR      = 0x28, /* Error Status Register */
    APIC_ICR0     = 0x30, /* Interrupt Command Register */
    APIC_ICR1     = 0x31, /* Interrupt Command Register (not available in extended mode) */
    APIC_TMRLVTR  = 0x32, /* Timer Local Vector Table */
    APIC_THRMLVTR = 0x33, /* Thermal Local Vector Table */
    APIC_PCLVTR   = 0x34, /* Performance Counter Local Vector Table */
    APIC_LINT0    = 0x35, /* LINT0 Local Vector Table */
    APIC_LINT1    = 0x36, /* LINT1 Local Vector Table */
    APIC_ERRLVTR  = 0x37, /* Error Local Vector Table */
    APIC_TICR     = 0x38, /* Initial Count Register for Timer */
    APIC_TCCR     = 0x39, /* Current Count Register for Timer */
    APIC_TDCR     = 0x3E, /* Timer Divide Configuration Register */
    APIC_EAFR     = 0x40, /* extended APIC Feature register */
    APIC_EACR     = 0x41, /* Extended APIC Control Register */
    APIC_SEOI     = 0x42, /* Specific End Of Interrupt Register */
    APIC_EXT0LVTR = 0x50, /* Extended Interrupt 0 Local Vector Table */
    APIC_EXT1LVTR = 0x51, /* Extended Interrupt 1 Local Vector Table */
    APIC_EXT2LVTR = 0x52, /* Extended Interrupt 2 Local Vector Table */
    APIC_EXT3LVTR = 0x53  /* Extended Interrupt 3 Local Vector Table */
} APIC_REGISTER, *PAPIC_REGISTER;

/* APIC mode list */
typedef enum _APIC_MODE
{
    APIC_MODE_COMPAT,
    APIC_MODE_X2APIC
} APIC_MODE, *PAPIC_MODE;

/* APIC destination short-hand enumeration list */
typedef enum _APIC_DSH
{
    APIC_DSH_Destination,
    APIC_DSH_Self,
    APIC_DSH_AllIncludingSelf,
    APIC_DSH_AllExclusingSelf
} APIC_DSH, *PAPIC_DSH;

/* APIC message type enumeration list */
typedef enum _APIC_MT
{
    APIC_MT_Fixed,
    APIC_MT_LowestPriority,
    APIC_MT_SMI,
    APIC_MT_RemoteRead,
    APIC_MT_NMI,
    APIC_MT_INIT,
    APIC_MT_Startup,
    APIC_MT_ExtInt,
} APIC_MT, *PAPIC_MT;

/* I8259 PIC interrupt mode enumeration list */
typedef enum _PIC_I8259_ICW1_INTERRUPT_MODE
{
    EdgeTriggered,
    LevelTriggered
} PIC_I8259_ICW1_INTERRUPT_MODE, *PPIC_I8259_ICW1_INTERRUPT_MODE;

/* I8259 PIC interval enumeration list */
typedef enum _PIC_I8259_ICW1_INTERVAL
{
    Interval8,
    Interval4
} PIC_I8259_ICW1_INTERVAL, *PPIC_I8259_ICW1_INTERVAL;

/* I8259 PIC operating mode enumeration list */
typedef enum _PIC_I8259_ICW1_OPERATING_MODE
{
    Cascade,
    Single
} PIC_I8259_ICW1_OPERATING_MODE, *PPIC_I8259_ICW1_OPERATING_MODE;

/* I8259 PIC buffered mode enumeration list */
typedef enum _PIC_I8259_ICW4_BUFFERED_MODE
{
    NonBuffered,
    NonBuffered2,
    BufferedSlave,
    BufferedMaster
} PIC_I8259_ICW4_BUFFERED_MODE, *PPIC_I8259_ICW4_BUFFERED_MODE;

/* I8259 PIC End Of Interrupt (EOI) mode enumeration list */
typedef enum _PIC_I8259_ICW4_EOI_MODE
{
    NormalEoi,
    AutomaticEoi
} PIC_I8259_ICW4_EOI_MODE, *PPIC_I8259_ICW4_EOI_MODE;

/* I8259 PIC system mode enumeration list */
typedef enum _PIC_I8259_ICW4_SYSTEM_MODE
{
    Mcs8085Mode,
    New8086Mode
} PIC_I8259_ICW4_SYSTEM_MODE, *PPIC_I8259_ICW4_SYSTEM_MODE;

/* APIC Base Register */
typedef union _APIC_BASE_REGISTER
{
    ULONGLONG LongLong;
    struct
    {
        ULONGLONG Reserved1:8;
        ULONGLONG BootStrapProcessor:1;
        ULONGLONG Reserved2:1;
        ULONGLONG ExtendedMode:1;
        ULONGLONG Enable:1;
        ULONGLONG BaseAddress:40;
        ULONGLONG Reserved3:12;
    };
} APIC_BASE_REGISTER, *PAPIC_BASE_REGISTER;

/* APIC Command Register */
typedef union _APIC_COMMAND_REGISTER
{
    ULONGLONG LongLong;
    struct
    {
        ULONG Long0;
        ULONG Long1;
    };
    struct
    {
        ULONGLONG Vector:8;
        ULONGLONG MessageType:3;
        ULONGLONG DestinationMode:1;
        ULONGLONG DeliveryStatus:1;
        ULONGLONG ReservedMBZ:1;
        ULONGLONG Level:1;
        ULONGLONG TriggerMode:1;
        ULONGLONG RemoteReadStatus:2;
        ULONGLONG DestinationShortHand:2;
        ULONGLONG Reserved2MBZ:36;
        ULONGLONG Destination:8;
    };
} APIC_COMMAND_REGISTER, *PAPIC_COMMAND_REGISTER;

/* APIC Local Vector Table (LVT) Register */
typedef union _APIC_LVT_REGISTER
{
    ULONG Long;
    struct
    {
        ULONG Vector:8;
        ULONG MessageType:3;
        ULONG Reserved1:1;
        ULONG DeliveryStatus:1;
        ULONG Reserved2:1;
        ULONG RemoteIRR:1;
        ULONG TriggerMode:1;
        ULONG Mask:1;
        ULONG TimerMode:1;
        ULONG Reserved3:13;
    };
} APIC_LVT_REGISTER, *PAPIC_LVT_REGISTER;

/* APIC Spurious Register */
typedef union _APIC_SPURIOUS_REGISTER
{
    ULONG Long;
    struct
    {
        ULONG Vector:8;
        ULONG SoftwareEnable:1;
        ULONG CoreChecking:1;
        ULONG Reserved:22;
    };
} APIC_SPURIOUS_REGISTER, *PAPIC_SPURIOUS_REGISTER;

/* I8259 PIC register structure */
typedef union _PIC_I8259_ICW1
{
    struct
    {
        UCHAR NeedIcw4:1;
        UCHAR OperatingMode:1;
        UCHAR Interval:1;
        UCHAR InterruptMode:1;
        UCHAR Init:1;
        UCHAR InterruptVectorAddress:3;
    };
    UCHAR Bits;
} PIC_I8259_ICW1, *PPIC_I8259_ICW1;

/* I8259 PIC register structure */
typedef union _PIC_I8259_ICW2
{
    struct
    {
        UCHAR Sbz:3;
        UCHAR InterruptVector:5;
    };
    UCHAR Bits;
} PIC_I8259_ICW2, *PPIC_I8259_ICW2;

/* I8259 PIC register structure */
typedef union _PIC_I8259_ICW3
{
    union
    {
        struct
        {
            UCHAR SlaveIrq0:1;
            UCHAR SlaveIrq1:1;
            UCHAR SlaveIrq2:1;
            UCHAR SlaveIrq3:1;
            UCHAR SlaveIrq4:1;
            UCHAR SlaveIrq5:1;
            UCHAR SlaveIrq6:1;
            UCHAR SlaveIrq7:1;
        };
        struct
        {
            UCHAR SlaveId:3;
            UCHAR Reserved:5;
        };
    };
    UCHAR Bits;
} PIC_I8259_ICW3, *PPIC_I8259_ICW3;

/* I8259 PIC register structure */
typedef union _PIC_I8259_ICW4
{
    struct
    {
        UCHAR SystemMode:1;
        UCHAR EoiMode:1;
        UCHAR BufferedMode:2;
        UCHAR SpecialFullyNestedMode:1;
        UCHAR Reserved:3;
    };
    UCHAR Bits;
} PIC_I8259_ICW4, *PPIC_I8259_ICW4;

#endif /* __XTDK_I686_HLTYPES_H */
