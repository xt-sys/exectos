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
#define APIC_MSR_BASE                                   0x0000001B

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
#define PIC1_ELCR_PORT                                  0x04D0
#define PIC2_CONTROL_PORT                               0xA0
#define PIC2_DATA_PORT                                  0xA1
#define PIC2_ELCR_PORT                                  0x04D1

/* PIC vector definitions */
#define PIC1_VECTOR_SPURIOUS                            0x37

/* Serial port I/O addresses */
#define COMPORT_ADDRESSES                               {0x000, 0x3F8, 0x2F8, 0x3E8, 0x2E8, 0x5F8, 0x4F8, 0x5E8, 0x4E8}

#endif /* __XTDK_AMD64_HLTYPES_H */
