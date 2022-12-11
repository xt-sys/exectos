/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/hltypes.h
 * DESCRIPTION:     XT hardware abstraction layer structures definitions specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_HLTYPES_H
#define __XTDK_I686_HLTYPES_H

#include "xtdefs.h"
#include "xtstruct.h"
#include "xttypes.h"


typedef enum _CPUID_FEATURES
{
    CPUID_FEATURES_ECX_SSE3         = 1 << 0,
    CPUID_FEATURES_ECX_PCLMUL       = 1 << 1,
    CPUID_FEATURES_ECX_DTES64       = 1 << 2,
    CPUID_FEATURES_ECX_MONITOR      = 1 << 3,
    CPUID_FEATURES_ECX_DS_CPL       = 1 << 4,
    CPUID_FEATURES_ECX_VMX          = 1 << 5,
    CPUID_FEATURES_ECX_SMX          = 1 << 6,
    CPUID_FEATURES_ECX_EST          = 1 << 7,
    CPUID_FEATURES_ECX_TM2          = 1 << 8,
    CPUID_FEATURES_ECX_SSSE3        = 1 << 9,
    CPUID_FEATURES_ECX_CID          = 1 << 10,
    CPUID_FEATURES_ECX_SDBG         = 1 << 11,
    CPUID_FEATURES_ECX_FMA          = 1 << 12,
    CPUID_FEATURES_ECX_CX16         = 1 << 13,
    CPUID_FEATURES_ECX_XTPR         = 1 << 14,
    CPUID_FEATURES_ECX_PDCM         = 1 << 15,
    CPUID_FEATURES_ECX_PCID         = 1 << 17,
    CPUID_FEATURES_ECX_DCA          = 1 << 18,
    CPUID_FEATURES_ECX_SSE4_1       = 1 << 19,
    CPUID_FEATURES_ECX_SSE4_2       = 1 << 20,
    CPUID_FEATURES_ECX_X2APIC       = 1 << 21,
    CPUID_FEATURES_ECX_MOVBE        = 1 << 22,
    CPUID_FEATURES_ECX_POPCNT       = 1 << 23,
    CPUID_FEATURES_ECX_TSC          = 1 << 24,
    CPUID_FEATURES_ECX_AES          = 1 << 25,
    CPUID_FEATURES_ECX_XSAVE        = 1 << 26,
    CPUID_FEATURES_ECX_OSXSAVE      = 1 << 27,
    CPUID_FEATURES_ECX_AVX          = 1 << 28,
    CPUID_FEATURES_ECX_F16C         = 1 << 29,
    CPUID_FEATURES_ECX_RDRAND       = 1 << 30,
    CPUID_FEATURES_ECX_HYPERVISOR   = 1 << 31,
    CPUID_FEATURES_EDX_FPU          = 1 << 0,
    CPUID_FEATURES_EDX_VME          = 1 << 1,
    CPUID_FEATURES_EDX_DE           = 1 << 2,
    CPUID_FEATURES_EDX_PSE          = 1 << 3,
    CPUID_FEATURES_EDX_TSC          = 1 << 4,
    CPUID_FEATURES_EDX_MSR          = 1 << 5,
    CPUID_FEATURES_EDX_PAE          = 1 << 6,
    CPUID_FEATURES_EDX_MCE          = 1 << 7,
    CPUID_FEATURES_EDX_CX8          = 1 << 8,
    CPUID_FEATURES_EDX_APIC         = 1 << 9,
    CPUID_FEATURES_EDX_SEP          = 1 << 11,
    CPUID_FEATURES_EDX_MTRR         = 1 << 12,
    CPUID_FEATURES_EDX_PGE          = 1 << 13,
    CPUID_FEATURES_EDX_MCA          = 1 << 14,
    CPUID_FEATURES_EDX_CMOV         = 1 << 15,
    CPUID_FEATURES_EDX_PAT          = 1 << 16,
    CPUID_FEATURES_EDX_PSE36        = 1 << 17,
    CPUID_FEATURES_EDX_PSN          = 1 << 18,
    CPUID_FEATURES_EDX_CLFLUSH      = 1 << 19,
    CPUID_FEATURES_EDX_DS           = 1 << 21,
    CPUID_FEATURES_EDX_ACPI         = 1 << 22,
    CPUID_FEATURES_EDX_MMX          = 1 << 23,
    CPUID_FEATURES_EDX_FXSR         = 1 << 24,
    CPUID_FEATURES_EDX_SSE          = 1 << 25,
    CPUID_FEATURES_EDX_SSE2         = 1 << 26,
    CPUID_FEATURES_EDX_SS           = 1 << 27,
    CPUID_FEATURES_EDX_HTT          = 1 << 28,
    CPUID_FEATURES_EDX_TM           = 1 << 29,
    CPUID_FEATURES_EDX_IA64         = 1 << 30,
    CPUID_FEATURES_EDX_PBE          = 1 << 31
} CPUID_FEATURES, *PCPUID_FEATURES;

typedef enum _CPUID_REQUESTS
{
    CPUID_GET_VENDOR_STRING,
    CPUID_GET_CPU_FEATURES,
    CPUID_GET_TLB,
    CPUID_GET_SERIAL
} CPUID_REQUESTS, *PCPUID_REQUESTS;

typedef struct _CPUID_REGISTERS
{
    UINT32 Leaf;
    UINT32 SubLeaf;
    UINT32 Eax;
    UINT32 Ebx;
    UINT32 Ecx;
    UINT32 Edx;
} CPUID_REGISTERS, *PCPUID_REGISTERS;

#endif /* __XTDK_I686_HLTYPES_H */
