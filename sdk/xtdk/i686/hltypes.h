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


/* APIC base address */
#define APIC_BASE                                       0xFFFE0000

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

/* Serial port I/O addresses */
#define COMPORT_ADDRESSES                               {0x000, 0x3F8, 0x2F8, 0x3E8, 0x2E8, 0x5F8, 0x4F8, 0x5E8, 0x4E8}

#endif /* __XTDK_I686_HLTYPES_H */
