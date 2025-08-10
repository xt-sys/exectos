/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/amd64/artypes.h
 * DESCRIPTION:     AMD64 architecture library structure definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_AMD64_ARTYPES_H
#define __XTDK_AMD64_ARTYPES_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>


/* Control Register 0 constants */
#define CR0_PE                                          0x00000001
#define CR0_MP                                          0x00000002
#define CR0_EM                                          0x00000004
#define CR0_TS                                          0x00000008
#define CR0_ET                                          0x00000010
#define CR0_NE                                          0x00000020
#define CR0_WP                                          0x00010000
#define CR0_AM                                          0x00040000
#define CR0_NW                                          0x20000000
#define CR0_CD                                          0x40000000
#define CR0_PG                                          0x80000000

/* Control Register 4 constants */
#define CR4_VME                                         0x00000001
#define CR4_PVI                                         0x00000002
#define CR4_TSD                                         0x00000004
#define CR4_DE                                          0x00000008
#define CR4_PSE                                         0x00000010
#define CR4_PAE                                         0x00000020
#define CR4_MCE                                         0x00000040
#define CR4_PGE                                         0x00000080
#define CR4_PCE                                         0x00000100
#define CR4_FXSR                                        0x00000200
#define CR4_XMMEXCPT                                    0x00000400
#define CR4_LA57                                        0x00001000
#define CR4_RESERVED1                                   0x00001800
#define CR4_VMXE                                        0x00002000
#define CR4_SMXE                                        0x00004000
#define CR4_RESERVED2                                   0x00018000
#define CR4_XSAVE                                       0x00020000
#define CR4_RESERVED3                                   0xFFFC0000

/* Descriptors size */
#define GDT_ENTRIES                                     128
#define IDT_ENTRIES                                     256

/* Initial MXCSR control */
#define INITIAL_MXCSR                                   0x1F80

/* Page Attributes Table types */
#define PAT_TYPE_STRONG_UC                              0ULL
#define PAT_TYPE_USWC                                   1ULL
#define PAT_TYPE_WT                                     4ULL
#define PAT_TYPE_WP                                     5ULL
#define PAT_TYPE_WB                                     6ULL
#define PAT_TYPE_WEAK_UC                                7ULL

/* Segment defintions */
#define SEGMENT_CS                                      0x2E
#define SEGMENT_DS                                      0x3E
#define SEGMENT_ES                                      0x26
#define SEGMENT_SS                                      0x36
#define SEGMENT_FS                                      0x64
#define SEGMENT_GS                                      0x65

/* MSR values */
#define X86_MSR_SYSENTER_CS                             0x00000174
#define X86_MSR_SYSENTER_ESP                            0x00000175
#define X86_MSR_SYSENTER_EIP                            0x00000176
#define X86_MSR_POWER_CONTROL                           0x000001FC
#define X86_MSR_PAT                                     0x00000277
#define X86_MSR_EFER                                    0xC0000080
#define X86_MSR_STAR                                    0xC0000081
#define X86_MSR_LSTAR                                   0xC0000082
#define X86_MSR_CSTAR                                   0xC0000083
#define X86_MSR_FMASK                                   0xC0000084
#define X86_MSR_FSBASE                                  0xC0000100
#define X86_MSR_GSBASE                                  0xC0000101
#define X86_MSR_KERNEL_GSBASE                           0xC0000102

/* Processor features in the EFER MSR */
#define X86_MSR_EFER_SCE                                (1 <<  0)
#define X86_MSR_EFER_LME                                (1 <<  8)
#define X86_MSR_EFER_LMA                                (1 << 10)
#define X86_MSR_EFER_NXE                                (1 << 11)
#define X86_MSR_EFER_SVME                               (1 << 12)

/* X86 EFLAG bit masks definitions */
#define X86_EFLAGS_NF_MASK                              0x00000000 /* None */
#define X86_EFLAGS_CF_MASK                              0x00000001 /* Carry */
#define X86_EFLAGS_PF_MASK                              0x00000004 /* Parity */
#define X86_EFALGS_AF_MASK                              0x00000010 /* Aux Carry */
#define X86_EFLAGS_ZF_MASK                              0x00000040 /* Zero */
#define X86_EFLAGS_SF_MASK                              0x00000080 /* Sign */
#define X86_EFLAGS_TF_MASK                              0x00000100 /* Trap */
#define X86_EFLAGS_IF_MASK                              0x00000200 /* Interrupt */
#define X86_EFLAGS_DF_MASK                              0x00000400 /* Direction */
#define X86_EFLAGS_OF_MASK                              0x00000800 /* Overflow */
#define X86_EFLAGS_IOPL_MASK                            0x00003000 /* I/O Privilege */
#define X86_EFLAGS_NT_MASK                              0x00004000 /* Nested Task */
#define X86_EFLAGS_SIGN_MASK                            0x00008000 /* Sign */
#define X86_EFLAGS_RF_MASK                              0x00010000 /* Resume */
#define X86_EFLAGS_V86_MASK                             0x00020000 /* Virtual 8086 */
#define X86_EFLAGS_AC_MASK                              0x00040000 /* Alignment Check */
#define X86_EFLAGS_VIF_MASK                             0x00080000 /* Virtual Interrupt */
#define X86_EFLAGS_VIP_MASK                             0x00100000 /* Virtual Interrupt Pending */
#define X86_EFLAGS_ID_MASK                              0x00200000 /* Identification */

/* CPU vendor enumeration list */
typedef enum _CPU_VENDOR
{
    CPU_VENDOR_AMD = 0x68747541,
    CPU_VENDOR_INTEL = 0x756E6547,
    CPU_VENDOR_UNKNOWN = 0xFFFFFFFF
} CPU_VENDOR, *PCPU_VENDOR;

/* CPUID extended features (0x80000001) enumeration list */
typedef enum _CPUID_EXTENDED_FEATURES
{
    CPUID_FEATURES_ECX_LAHF_SAHF              = 1 << 0,
    CPUID_FEATURES_ECX_CMP_LEGACY             = 1 << 1,
    CPUID_FEATURES_ECX_SVM                    = 1 << 2,
    CPUID_FEATURES_ECX_EXT_APIC_SPACE         = 1 << 3,
    CPUID_FEATURES_ECX_ALT_MOV_CR8            = 1 << 4,
    CPUID_FEATURES_ECX_LZCNT                  = 1 << 5,
    CPUID_FEATURES_ECX_SSE4A                  = 1 << 6,
    CPUID_FEATURES_ECX_MISALIGNED_SSE         = 1 << 7,
    CPUID_FEATURES_ECX_PREFETCHW              = 1 << 8,
    CPUID_FEATURES_ECX_OSVW                   = 1 << 9,
    CPUID_FEATURES_ECX_IBS                    = 1 << 10,
    CPUID_FEATURES_ECX_XOP                    = 1 << 11,
    CPUID_FEATURES_ECX_SKINIT                 = 1 << 12,
    CPUID_FEATURES_ECX_WDT                    = 1 << 13,
    CPUID_FEATURES_ECX_LWP                    = 1 << 15,
    CPUID_FEATURES_ECX_FMA4                   = 1 << 16,
    CPUID_FEATURES_ECX_TCE                    = 1 << 17,
    CPUID_FEATURES_ECX_NODEID                 = 1 << 19,
    CPUID_FEATURES_ECX_TBM                    = 1 << 21,
    CPUID_FEATURES_ECX_TOPOLOGY_EXTENSIONS    = 1 << 22,
    CPUID_FEATURES_ECX_PERFCTR_EXT_CORE       = 1 << 23,
    CPUID_FEATURES_ECX_PERFCTR_EXT_NB         = 1 << 24,
    CPUID_FEATURES_ECX_DATA_BREAKPOINT_EXT    = 1 << 26,
    CPUID_FEATURES_ECX_PERF_TSC               = 1 << 27,
    CPUID_FEATURES_ECX_PERFCTR_EXT_L2I        = 1 << 28,
    CPUID_FEATURES_ECX_MONITORX_MWAITX        = 1 << 29,
    CPUID_FEATURES_ECX_CODEBP_ADDRMASK_EXT    = 1 << 30,
    CPUID_FEATURES_EDX_SYSCALL_SYSRET         = 1 << 11,
    CPUID_FEATURES_EDX_NX                     = 1 << 20,
    CPUID_FEATURES_EDX_AMD_MMX_EXT            = 1 << 22,
    CPUID_FEATURES_EDX_FFXSR                  = 1 << 25,
    CPUID_FEATURES_EDX_1G_PAGES               = 1 << 26,
    CPUID_FEATURES_EDX_RDTSCP                 = 1 << 27,
    CPUID_FEATURES_EDX_LONG_MODE              = 1 << 29,
    CPUID_FEATURES_EDX_3DNOW_EXT              = 1 << 30,
    CPUID_FEATURES_EDX_3DNOW                  = 1 << 31
} CPUID_EXTENDED_FEATURES, *PCPUID_EXTENDED_FEATURES;

/* CPUID STD1 features (0x00000001) enumeration list */
typedef enum _CPUID_STANDARD1_FEATURES
{
    CPUID_FEATURES_ECX_SSE3                   = 1 << 0,
    CPUID_FEATURES_ECX_PCLMUL                 = 1 << 1,
    CPUID_FEATURES_ECX_DTES64                 = 1 << 2,
    CPUID_FEATURES_ECX_MONITOR                = 1 << 3,
    CPUID_FEATURES_ECX_DS_CPL                 = 1 << 4,
    CPUID_FEATURES_ECX_VMX                    = 1 << 5,
    CPUID_FEATURES_ECX_SMX                    = 1 << 6,
    CPUID_FEATURES_ECX_EST                    = 1 << 7,
    CPUID_FEATURES_ECX_TM2                    = 1 << 8,
    CPUID_FEATURES_ECX_SSSE3                  = 1 << 9,
    CPUID_FEATURES_ECX_CID                    = 1 << 10,
    CPUID_FEATURES_ECX_SDBG                   = 1 << 11,
    CPUID_FEATURES_ECX_FMA                    = 1 << 12,
    CPUID_FEATURES_ECX_CX16                   = 1 << 13,
    CPUID_FEATURES_ECX_XTPR                   = 1 << 14,
    CPUID_FEATURES_ECX_PDCM                   = 1 << 15,
    CPUID_FEATURES_ECX_PCID                   = 1 << 17,
    CPUID_FEATURES_ECX_DCA                    = 1 << 18,
    CPUID_FEATURES_ECX_SSE4_1                 = 1 << 19,
    CPUID_FEATURES_ECX_SSE4_2                 = 1 << 20,
    CPUID_FEATURES_ECX_X2APIC                 = 1 << 21,
    CPUID_FEATURES_ECX_MOVBE                  = 1 << 22,
    CPUID_FEATURES_ECX_POPCNT                 = 1 << 23,
    CPUID_FEATURES_ECX_TSC                    = 1 << 24,
    CPUID_FEATURES_ECX_AES                    = 1 << 25,
    CPUID_FEATURES_ECX_XSAVE                  = 1 << 26,
    CPUID_FEATURES_ECX_OSXSAVE                = 1 << 27,
    CPUID_FEATURES_ECX_AVX                    = 1 << 28,
    CPUID_FEATURES_ECX_F16C                   = 1 << 29,
    CPUID_FEATURES_ECX_RDRAND                 = 1 << 30,
    CPUID_FEATURES_ECX_HYPERVISOR             = 1 << 31,
    CPUID_FEATURES_EDX_FPU                    = 1 << 0,
    CPUID_FEATURES_EDX_VME                    = 1 << 1,
    CPUID_FEATURES_EDX_DE                     = 1 << 2,
    CPUID_FEATURES_EDX_PSE                    = 1 << 3,
    CPUID_FEATURES_EDX_TSC                    = 1 << 4,
    CPUID_FEATURES_EDX_MSR                    = 1 << 5,
    CPUID_FEATURES_EDX_PAE                    = 1 << 6,
    CPUID_FEATURES_EDX_MCE                    = 1 << 7,
    CPUID_FEATURES_EDX_CX8                    = 1 << 8,
    CPUID_FEATURES_EDX_APIC                   = 1 << 9,
    CPUID_FEATURES_EDX_SEP                    = 1 << 11,
    CPUID_FEATURES_EDX_MTRR                   = 1 << 12,
    CPUID_FEATURES_EDX_PGE                    = 1 << 13,
    CPUID_FEATURES_EDX_MCA                    = 1 << 14,
    CPUID_FEATURES_EDX_CMOV                   = 1 << 15,
    CPUID_FEATURES_EDX_PAT                    = 1 << 16,
    CPUID_FEATURES_EDX_PSE36                  = 1 << 17,
    CPUID_FEATURES_EDX_PSN                    = 1 << 18,
    CPUID_FEATURES_EDX_CLFLUSH                = 1 << 19,
    CPUID_FEATURES_EDX_DS                     = 1 << 21,
    CPUID_FEATURES_EDX_ACPI                   = 1 << 22,
    CPUID_FEATURES_EDX_MMX                    = 1 << 23,
    CPUID_FEATURES_EDX_FXSR                   = 1 << 24,
    CPUID_FEATURES_EDX_SSE                    = 1 << 25,
    CPUID_FEATURES_EDX_SSE2                   = 1 << 26,
    CPUID_FEATURES_EDX_SS                     = 1 << 27,
    CPUID_FEATURES_EDX_HTT                    = 1 << 28,
    CPUID_FEATURES_EDX_TM                     = 1 << 29,
    CPUID_FEATURES_EDX_IA64                   = 1 << 30,
    CPUID_FEATURES_EDX_PBE                    = 1 << 31
} CPUID_STANDARD1_FEATURES, *PCPUID_STANDARD1_FEATURES;

/* CPUID STD7 features (0x00000007, subleaf 0) enumeration list */
typedef enum _CPUID_STANDARD7_LEAF0_FEATURES
{
    CPUID_FEATURES_EBX_FSGSBASE               = 1 << 0,
    CPUID_FEATURES_EBX_TSC_ADJUST             = 1 << 1,
    CPUID_FEATURES_EBX_SGX                    = 1 << 2,
    CPUID_FEATURES_EBX_BMI1                   = 1 << 3,
    CPUID_FEATURES_EBX_HLE                    = 1 << 4,
    CPUID_FEATURES_EBX_AVX2                   = 1 << 5,
    CPUID_FEATURES_EBX_FDP_DEPRECATION        = 1 << 6,
    CPUID_FEATURES_EBX_SMEP                   = 1 << 7,
    CPUID_FEATURES_EBX_BMI2                   = 1 << 8,
    CPUID_FEATURES_EBX_ERMS                   = 1 << 9,
    CPUID_FEATURES_EBX_INVPCID                = 1 << 10,
    CPUID_FEATURES_EBX_RTM                    = 1 << 11,
    CPUID_FEATURES_EBX_QOS_MONITORING         = 1 << 12,
    CPUID_FEATURES_EBX_DEPRECATE_FCS_FDS      = 1 << 13,
    CPUID_FEATURES_EBX_MPX                    = 1 << 14,
    CPUID_FEATURES_EBX_QOS_ENFORCEMENT        = 1 << 15,
    CPUID_FEATURES_EBX_AVX512F                = 1 << 16,
    CPUID_FEATURES_EBX_AVX512DQ               = 1 << 17,
    CPUID_FEATURES_EBX_RDSEED                 = 1 << 18,
    CPUID_FEATURES_EBX_ADX                    = 1 << 19,
    CPUID_FEATURES_EBX_SMAP                   = 1 << 20,
    CPUID_FEATURES_EBX_AVX512IFMA52           = 1 << 21,
    CPUID_FEATURES_EBX_CLFLUSHOPT             = 1 << 23,
    CPUID_FEATURES_EBX_CLWB                   = 1 << 24,
    CPUID_FEATURES_EBX_PROCESSOR_TRACE        = 1 << 25,
    CPUID_FEATURES_EBX_AVX512PF               = 1 << 26,
    CPUID_FEATURES_EBX_AVX512ER               = 1 << 27,
    CPUID_FEATURES_EBX_AVX512CD               = 1 << 28,
    CPUID_FEATURES_EBX_SHA                    = 1 << 29,
    CPUID_FEATURES_EBX_AVX512BW               = 1 << 30,
    CPUID_FEATURES_EBX_AVX512VL               = 1 << 31,
    CPUID_FEATURES_ECX_PREFETCHWT1            = 1 << 0,
    CPUID_FEATURES_ECX_AVX512_VBMI            = 1 << 1,
    CPUID_FEATURES_ECX_UMIP                   = 1 << 2,
    CPUID_FEATURES_ECX_PKU                    = 1 << 3,
    CPUID_FEATURES_ECX_OSPKE                  = 1 << 4,
    CPUID_FEATURES_ECX_WAITPKG                = 1 << 5,
    CPUID_FEATURES_ECX_AVX512_VBMI2           = 1 << 6,
    CPUID_FEATURES_ECX_CET_SS                 = 1 << 7,
    CPUID_FEATURES_ECX_GFNI                   = 1 << 8,
    CPUID_FEATURES_ECX_VAES                   = 1 << 9,
    CPUID_FEATURES_ECX_VPCLMULQDQ             = 1 << 10,
    CPUID_FEATURES_ECX_AVX512_VNNI            = 1 << 11,
    CPUID_FEATURES_ECX_AVX512_BITALG          = 1 << 12,
    CPUID_FEATURES_ECX_TME                    = 1 << 13,
    CPUID_FEATURES_ECX_AVX512_VPOPCNTDQ       = 1 << 14,
    CPUID_FEATURES_ECX_LA57                   = 1 << 16,
    CPUID_FEATURES_ECX_RDPID                  = 1 << 22,
    CPUID_FEATURES_ECX_KEYLOCKER              = 1 << 23,
    CPUID_FEATURES_ECX_BUS_LOCK_DETECT        = 1 << 24,
    CPUID_FEATURES_ECX_CLDEMOTE               = 1 << 25,
    CPUID_FEATURES_ECX_MOVDIRI                = 1 << 27,
    CPUID_FEATURES_ECX_MOVDIR64B              = 1 << 28,
    CPUID_FEATURES_ECX_ENQCMD                 = 1 << 29,
    CPUID_FEATURES_ECX_SGX_LAUNCH_CONFIG      = 1 << 30,
    CPUID_FEATURES_ECX_PKS                    = 1 << 31,
    CPUID_FEATURES_EDX_SGX_KEYS               = 1 << 1,
    CPUID_FEATURES_EDX_AVX512_4VNNIW          = 1 << 2,
    CPUID_FEATURES_EDX_AVX512_4FMAPS          = 1 << 3,
    CPUID_FEATURES_EDX_FAST_SHORT_REP_MOV     = 1 << 4,
    CPUID_FEATURES_EDX_UINTR                  = 1 << 5,
    CPUID_FEATURES_EDX_AVX512_VPINTERSECT     = 1 << 8,
    CPUID_FEATURES_EDX_SRBDS_CTRL             = 1 << 9,
    CPUID_FEATURES_EDX_MD_CLEAR               = 1 << 10,
    CPUID_FEATURES_EDX_RTM_ALWAYS_ABORT       = 1 << 11,
    CPUID_FEATURES_EDX_RTM_FORCE_ABORT        = 1 << 13,
    CPUID_FEATURES_EDX_SERIALIZE              = 1 << 14,
    CPUID_FEATURES_EDX_HYBRID                 = 1 << 15,
    CPUID_FEATURES_EDX_TSXLDTRK               = 1 << 16,
    CPUID_FEATURES_EDX_PCONFIG                = 1 << 18,
    CPUID_FEATURES_EDX_ARCH_LBR               = 1 << 19,
    CPUID_FEATURES_EDX_CET_IBT                = 1 << 20,
    CPUID_FEATURES_EDX_AMX_BF16               = 1 << 22,
    CPUID_FEATURES_EDX_AVX512_FP16            = 1 << 23,
    CPUID_FEATURES_EDX_AMX_TILE               = 1 << 24,
    CPUID_FEATURES_EDX_AMX_INT8               = 1 << 25,
    CPUID_FEATURES_EDX_SCA_IBRS_IBPB          = 1 << 26,
    CPUID_FEATURES_EDX_SCA_STIBP              = 1 << 27,
    CPUID_FEATURES_EDX_L1D_FLUSH              = 1 << 28,
    CPUID_FEATURES_EDX_ARCH_CAPABILITIES_MSR  = 1 << 29,
    CPUID_FEATURES_EDX_CORE_CAPABILITIES_MSR  = 1 << 30,
    CPUID_FEATURES_EDX_SCA_SSBD               = 1 << 31
} CPUID_STANDARD7_LEAF0_FEATURES, *PCPUID_STANDARD7_LEAF0_FEATURES;

/* CPUID STD7 features (0x00000007, subleaf 1) enumeration list */
typedef enum _CPUID_STANDARD7_LEAF1_FEATURES
{
    CPUID_FEATURES_EAX_SHA512                 = 1 << 0,
    CPUID_FEATURES_EAX_SM3                    = 1 << 1,
    CPUID_FEATURES_EAX_SM4                    = 1 << 2,
    CPUID_FEATURES_EAX_RAO_INT                = 1 << 3,
    CPUID_FEATURES_EAX_AVX_VNNI               = 1 << 4,
    CPUID_FEATURES_EAX_AVX512_BF16            = 1 << 5,
    CPUID_FEATURES_EAX_LASS                   = 1 << 6,
    CPUID_FEATURES_EAX_CMPCCXADD              = 1 << 7,
    CPUID_FEATURES_EAX_ARCH_PERFMON           = 1 << 8,
    CPUID_FEATURES_EAX_FAST_ZEROLEN_REP_MOVSB = 1 << 10,
    CPUID_FEATURES_EAX_FAST_ZEROLEN_REP_STOSB = 1 << 11,
    CPUID_FEATURES_EAX_FAST_ZEROLEN_REP_CMPSB = 1 << 12,
    CPUID_FEATURES_EAX_FRED                   = 1 << 17,
    CPUID_FEATURES_EAX_LKGS                   = 1 << 18,
    CPUID_FEATURES_EAX_WRMSRNS                = 1 << 19,
    CPUID_FEATURES_EAX_NMI_SOURCE_REPORTING   = 1 << 20,
    CPUID_FEATURES_EAX_AMX_FP16               = 1 << 21,
    CPUID_FEATURES_EAX_HRESET                 = 1 << 22,
    CPUID_FEATURES_EAX_AVX_IFMA               = 1 << 23,
    CPUID_FEATURES_EAX_LAM                    = 1 << 26,
    CPUID_FEATURES_EAX_MSRLIST                = 1 << 27,
    CPUID_FEATURES_EAX_INVD_DISABLE           = 1 << 30,
    CPUID_FEATURES_EAX_MOVRS                  = 1 << 31,
    CPUID_FEATURES_EBX_PPIN                   = 1 << 0,
    CPUID_FEATURES_EBX_TSE                    = 1 << 1,
    CPUID_FEATURES_EBX_CPUIDMAXVAL_LIM_RMV    = 1 << 3,
    CPUID_FEATURES_ECX_MSR_IMM                = 1 << 5,
    CPUID_FEATURES_EDX_AVX_VNNI_INT8          = 1 << 4,
    CPUID_FEATURES_EDX_AVX_NE_CONVERT         = 1 << 5,
    CPUID_FEATURES_EDX_AMX_COMPLEX            = 1 << 8,
    CPUID_FEATURES_EDX_AVX_VNNI_INT16         = 1 << 10,
    CPUID_FEATURES_EDX_USER_TIMER             = 1 << 13,
    CPUID_FEATURES_EDX_PREFETCHI              = 1 << 14,
    CPUID_FEATURES_EDX_USER_MSR               = 1 << 15,
    CPUID_FEATURES_EDX_UIRET_UIF              = 1 << 17,
    CPUID_FEATURES_EDX_CET_SSS                = 1 << 18,
    CPUID_FEATURES_EDX_AVX10                  = 1 << 19,
    CPUID_FEATURES_EDX_APX                    = 1 << 21,
    CPUID_FEATURES_EDX_MWAIT_AND_LEAF5        = 1 << 23
} CPUID_STANDARD7_LEAF1_FEATURES, *PCPUID_STANDARD7_LEAF1_FEATURES;

/* CPUID requests */
typedef enum _CPUID_REQUESTS
{
    CPUID_GET_VENDOR_STRING,
    CPUID_GET_CPU_FEATURES,
    CPUID_GET_TLB,
    CPUID_GET_SERIAL
} CPUID_REQUESTS, *PCPUID_REQUESTS;

/* Processor identification information */
typedef struct _CPU_IDENTIFICATION
{
    USHORT Family;
    USHORT Model;
    USHORT Stepping;
    CPU_VENDOR Vendor;
    UCHAR VendorName[13];
} CPU_IDENTIFICATION, *PCPU_IDENTIFICATION;

/* CPUID registers */
typedef struct _CPUID_REGISTERS
{
    UINT32 Leaf;
    UINT32 SubLeaf;
    UINT32 Eax;
    UINT32 Ebx;
    UINT32 Ecx;
    UINT32 Edx;
} CPUID_REGISTERS, *PCPUID_REGISTERS;

/* CPU signature read from CPUID structure definition */
typedef struct _CPUID_SIGNATURE
{
    ULONG Stepping:4;
    ULONG Model:4;
    ULONG Family:4;
    ULONG Unused1:4;
    ULONG ExtendedModel:4;
    ULONG ExtendedFamily:8;
    ULONG Unused2:4;
} CPU_SIGNATURE, *PCPU_SIGNATURE;

#endif /* __XTDK_AMD64_ARTYPES_H */
