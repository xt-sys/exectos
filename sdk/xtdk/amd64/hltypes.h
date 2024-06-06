/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/hltypes.h
 * DESCRIPTION:     XT hardware abstraction layer structures definitions specific to AMD64 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_HLTYPES_H
#define __XTDK_AMD64_HLTYPES_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>
#include ARCH_HEADER(xtstruct.h)


/* APIC base addresses */
#define APIC_BASE                                       0xFFFFFFFFFFFE0000
#define APIC_LAPIC_MSR_BASE                             0x0000001B
#define APIC_X2APIC_MSR_BASE                            0x00000800

/* APIC vector definitions */
#define APIC_VECTOR_ZERO                                0x00
#define APIC_VECTOR_APC                                 0x1F
#define APIC_VECTOR_DPC                                 0x2F
#define APIC_VECTOR_CMCI                                0x35
#define APIC_VECTOR_SPURIOUS                            0x3F
#define APIC_VECTOR_REBOOT                              0x50
#define APIC_VECTOR_GENERIC                             0xC1
#define APIC_VECTOR_SYNC                                0xD1
#define APIC_VECTOR_CLOCK                               0xD1
#define APIC_VECTOR_CLOCK_IPI                           0xD2
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
#define APIC_DM_FIXED                                   0
#define APIC_DM_LOWPRIO                                 1
#define APIC_DM_SMI                                     2
#define APIC_DM_REMOTE                                  3
#define APIC_DM_NMI                                     4
#define APIC_DM_INIT                                    5
#define APIC_DM_STARTUP                                 6
#define APIC_DM_EXTINT                                  7

/* APIC trigger modes */
#define APIC_TGM_EDGE                                   0
#define APIC_TGM_LEVEL                                  1

/* 8259/ISP PIC ports definitions */
#define PIC1_CONTROL_PORT                               0x20
#define PIC1_DATA_PORT                                  0x21
#define PIC2_CONTROL_PORT                               0xA0
#define PIC2_DATA_PORT                                  0xA1

/* PIC vector definitions */
#define PIC1_VECTOR_SPURIOUS                            0x37

/* Serial port I/O addresses */
#define COMPORT_ADDRESSES                               {0x000, 0x3F8, 0x2F8, 0x3E8, 0x2E8, 0x5F8, 0x4F8, 0x5E8, 0x4E8}

/* Initial stall factor */
#define INITIAL_STALL_FACTOR                            100

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

/* Processor identity structure */
typedef struct _HAL_PROCESSOR_IDENTITY
{
    UCHAR ProcessorId;
    UCHAR LApicId;
    BOOLEAN Bsp;
    BOOLEAN Started;
    PKPROCESSOR_BLOCK ProcessorBlock;
} HAL_PROCESSOR_IDENTITY, *PHAL_PROCESSOR_IDENTITY;

#endif /* __XTDK_AMD64_HLTYPES_H */
