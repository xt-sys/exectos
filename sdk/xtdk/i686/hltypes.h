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
#define PIC1_ELCR_PORT                                  0x04D0
#define PIC2_CONTROL_PORT                               0xA0
#define PIC2_DATA_PORT                                  0xA1
#define PIC2_ELCR_PORT                                  0x04D1

/* PIC vector definitions */
#define PIC1_VECTOR_SPURIOUS                            0x37

/* Serial port I/O addresses */
#define COMPORT_ADDRESSES                               {0x000, 0x3F8, 0x2F8, 0x3E8, 0x2E8, 0x5F8, 0x4F8, 0x5E8, 0x4E8}

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

#endif /* __XTDK_I686_HLTYPES_H */
