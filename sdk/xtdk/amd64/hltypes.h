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


/* APIC base address */
#define APIC_BASE                                       0xFFFFFFFFFFFE0000

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

/* Serial port I/O addresses */
#define COMPORT_ADDRESSES                               {0x000, 0x3F8, 0x2F8, 0x3E8, 0x2E8, 0x5F8, 0x4F8, 0x5E8, 0x4E8}

#endif /* __XTDK_AMD64_HLTYPES_H */
