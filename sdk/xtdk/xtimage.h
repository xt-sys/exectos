/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtimage.h
 * DESCRIPTION:     Executable image structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTIMAGE_H
#define __XTDK_XTIMAGE_H

#include "xtdefs.h"
#include "xtstruct.h"
#include "xttypes.h"


/* PE/COFF file image signatures */
#define PECOFF_IMAGE_DOS_SIGNATURE                         0x5A4D /* MZ */
#define PECOFF_IMAGE_OS2_SIGNATURE                         0x454E /* NE */
#define PECOFF_IMAGE_OS2LE_SIGNATURE                       0x454C /* LE */
#define PECOFF_IMAGE_VXD_SIGNATURE                         0x454C /* LE */
#define PECOFF_IMAGE_NT_SIGNATURE                          0x00004550 /* PE00 */
#define PECOFF_IMAGE_XT_SIGNATURE                          0x54584550 /* PEXT */

/* PE/COFF image subsystems */
#define PECOFF_IMAGE_SUBSYSTEM_UNKNOWN                     0
#define PECOFF_IMAGE_SUBSYSTEM_NATIVE                      1
#define PECOFF_IMAGE_SUBSYSTEM_WINDOWS_GUI                 2
#define PECOFF_IMAGE_SUBSYSTEM_WINDOWS_CUI                 3
#define PECOFF_IMAGE_SUBSYSTEM_WINDOWS_CE_OLD              4
#define PECOFF_IMAGE_SUBSYSTEM_OS2_CUI                     5
#define PECOFF_IMAGE_SUBSYSTEM_POSIX_CUI                   7
#define PECOFF_IMAGE_SUBSYSTEM_NATIVE_WINDOWS              8
#define PECOFF_IMAGE_SUBSYSTEM_WINDOWS_CE_GUI              9
#define PECOFF_IMAGE_SUBSYSTEM_EFI_APPLICATION             10
#define PECOFF_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER     11
#define PECOFF_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER          12
#define PECOFF_IMAGE_SUBSYSTEM_EFI_ROM                     13
#define PECOFF_IMAGE_SUBSYSTEM_XBOX                        14
#define PECOFF_IMAGE_SUBSYSTEM_XT_NATIVE_APPLICATION       21
#define PECOFF_IMAGE_SUBSYSTEM_XT_NATIVE_DRIVER            22
#define PECOFF_IMAGE_SUBSYSTEM_XT_DYNAMIC_LIBRARY          23
#define PECOFF_IMAGE_SUBSYSTEM_XT_APPLICATION_CLI          24
#define PECOFF_IMAGE_SUBSYSTEM_XT_APPLICATION_GDI          25

/* PE/COFF file image architecture */
#define PECOFF_IMAGE_FILE_MACHINE_UNKNOWN                  0x0000
#define PECOFF_IMAGE_FILE_MACHINE_I386                     0x014C
#define PECOFF_IMAGE_FILE_MACHINE_R4000                    0x0166
#define PECOFF_IMAGE_FILE_MACHINE_R3000                    0x0162
#define PECOFF_IMAGE_FILE_MACHINE_R10000                   0x0168
#define PECOFF_IMAGE_FILE_MACHINE_WCEMIPSV2                0x0169
#define PECOFF_IMAGE_FILE_MACHINE_ALPHA                    0x0184
#define PECOFF_IMAGE_FILE_MACHINE_SH3                      0x01A2
#define PECOFF_IMAGE_FILE_MACHINE_SH3DSP                   0x01A3
#define PECOFF_IMAGE_FILE_MACHINE_SH3E                     0x01A4
#define PECOFF_IMAGE_FILE_MACHINE_SH4                      0x01A6
#define PECOFF_IMAGE_FILE_MACHINE_SH5                      0x01A8
#define PECOFF_IMAGE_FILE_MACHINE_ARM                      0x01C0
#define PECOFF_IMAGE_FILE_MACHINE_THUMB                    0x01C2
#define PECOFF_IMAGE_FILE_MACHINE_AM33                     0x01D3
#define PECOFF_IMAGE_FILE_MACHINE_POWERPC                  0x01F0
#define PECOFF_IMAGE_FILE_MACHINE_POWERPCFP                0x01F1
#define PECOFF_IMAGE_FILE_MACHINE_IA64                     0x0200
#define PECOFF_IMAGE_FILE_MACHINE_MIPS16                   0x0266
#define PECOFF_IMAGE_FILE_MACHINE_ALPHA64                  0x0284
#define PECOFF_IMAGE_FILE_MACHINE_MIPSFPU                  0x0366
#define PECOFF_IMAGE_FILE_MACHINE_MIPSFPU16                0x0466
#define PECOFF_IMAGE_FILE_MACHINE_AXP64                    0x0284
#define PECOFF_IMAGE_FILE_MACHINE_TRICORE                  0x0520
#define PECOFF_IMAGE_FILE_MACHINE_CEF                      0x0CEF
#define PECOFF_IMAGE_FILE_MACHINE_EBC                      0x0EBC
#define PECOFF_IMAGE_FILE_MACHINE_AMD64                    0x8664
#define PECOFF_IMAGE_FILE_MACHINE_M32R                     0x9041
#define PECOFF_IMAGE_FILE_MACHINE_CEE                      0xC0EE

/* PE/COFF image characteristic attributes */
#define PECOFF_IMAGE_FILE_RELOCS_STRIPPED                  0x0001
#define PECOFF_IMAGE_FILE_EXECUTABLE_IMAGE                 0x0002
#define PECOFF_IMAGE_FILE_LINE_NUMS_STRIPPED               0x0004
#define PECOFF_IMAGE_FILE_LOCAL_SYMS_STRIPPED              0x0008
#define PECOFF_IMAGE_FILE_AGGRESIVE_WS_TRIM                0x0010
#define PECOFF_IMAGE_FILE_LARGE_ADDRESS_AWARE              0x0020
#define PECOFF_IMAGE_FILE_BYTES_REVERSED_LO                0x0080
#define PECOFF_IMAGE_FILE_32BIT_MACHINE                    0x0100
#define PECOFF_IMAGE_FILE_DEBUG_STRIPPED                   0x0200
#define PECOFF_IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP          0x0400
#define PECOFF_IMAGE_FILE_NET_RUN_FROM_SWAP                0x0800
#define PECOFF_IMAGE_FILE_SYSTEM                           0x1000
#define PECOFF_IMAGE_FILE_DLL                              0x2000
#define PECOFF_IMAGE_FILE_UP_SYSTEM_ONLY                   0x4000
#define PECOFF_IMAGE_FILE_BYTES_REVERSED_HI                0x8000

/* PE/COFF directory entries count */
#define PECOFF_IMAGE_NUMBEROF_DIRECTORY_ENTRIES            16

/* PE/COFF image HDR magic */
#define PECOFF_IMAGE_NT_OPTIONAL_HDR32_MAGIC               0x10b
#define PECOFF_IMAGE_NT_OPTIONAL_HDR64_MAGIC               0x20b
#define PECOFF_IMAGE_ROM_OPTIONAL_HDR_MAGIC                0x107

/* PE/COFF directory entries */
#define PECOFF_IMAGE_DIRECTORY_ENTRY_EXPORT                0x0
#define PECOFF_IMAGE_DIRECTORY_ENTRY_IMPORT                0x1
#define PECOFF_IMAGE_DIRECTORY_ENTRY_RESOURCE              0x2
#define PECOFF_IMAGE_DIRECTORY_ENTRY_EXCEPTION             0x3
#define PECOFF_IMAGE_DIRECTORY_ENTRY_SECURITY              0x4
#define PECOFF_IMAGE_DIRECTORY_ENTRY_BASERELOC             0x5
#define PECOFF_IMAGE_DIRECTORY_ENTRY_DEBUG                 0x6
#define PECOFF_IMAGE_DIRECTORY_ENTRY_ARCHITECTURE          0x7
#define PECOFF_IMAGE_DIRECTORY_ENTRY_GLOBALPTR             0x8
#define PECOFF_IMAGE_DIRECTORY_ENTRY_TLS                   0x9
#define PECOFF_IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG           0xA
#define PECOFF_IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT          0xB
#define PECOFF_IMAGE_DIRECTORY_ENTRY_IAT                   0xC
#define PECOFF_IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT          0xD
#define PECOFF_IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR        0xE

/* PE/COFF image relocation types */
#define PECOFF_IMAGE_REL_BASED_ABSOLUTE                    0x0
#define PECOFF_IMAGE_REL_BASED_HIGH                        0x1
#define PECOFF_IMAGE_REL_BASED_LOW                         0x2
#define PECOFF_IMAGE_REL_BASED_HIGHLOW                     0x3
#define PECOFF_IMAGE_REL_BASED_HIGHADJ                     0x4
#define PECOFF_IMAGE_REL_BASED_MIPS_JMPADDR                0x5
#define PECOFF_IMAGE_REL_BASED_SECTION                     0x6
#define PECOFF_IMAGE_REL_BASED_REL32                       0x7
#define PECOFF_IMAGE_REL_BASED_VXD_RELATIVE                0x8
#define PECOFF_IMAGE_REL_BASED_MIPS_JMPADDR16              0x9
#define PECOFF_IMAGE_REL_BASED_IA64_IMM64                  0x9
#define PECOFF_IMAGE_REL_BASED_DIR64                       0xA

/* PE/COFF related sizes */
#define PECOFF_IMAGE_SIZEOF_SHORT_NAME                     8
#define PECOFF_IMAGE_SIZEOF_BASE_RELOCATION                8
#define PECOFF_IMAGE_SIZEOF_SECTION_HEADER                 40
#define PECOFF_IMAGE_SIZEOF_ROM_OPTIONAL_HEADER            56
#define PECOFF_IMAGE_SIZEOF_STD_OPTIONAL_HEADER            28
#define PECOFF_IMAGE_SIZEOF_NT_OPTIONAL32_HEADER           224
#define PECOFF_IMAGE_SIZEOF_NT_OPTIONAL64_HEADER           240

/* PE/COFF image section characteristics */
#define PECOFF_IMAGE_SCN_TYPE_REG                          0x00000000
#define PECOFF_IMAGE_SCN_TYPE_DSECT                        0x00000001
#define PECOFF_IMAGE_SCN_TYPE_NOLOAD                       0x00000002
#define PECOFF_IMAGE_SCN_TYPE_GROUP                        0x00000004
#define PECOFF_IMAGE_SCN_TYPE_NO_PAD                       0x00000008
#define PECOFF_IMAGE_SCN_TYPE_COPY                         0x00000010
#define PECOFF_IMAGE_SCN_CNT_CODE                          0x00000020
#define PECOFF_IMAGE_SCN_CNT_INITIALIZED_DATA              0x00000040
#define PECOFF_IMAGE_SCN_CNT_UNINITIALIZED_DATA            0x00000080
#define PECOFF_IMAGE_SCN_LNK_OTHER                         0x00000100
#define PECOFF_IMAGE_SCN_LNK_INFO                          0x00000200
#define PECOFF_IMAGE_SCN_TYPE_OVER                         0x00000400
#define PECOFF_IMAGE_SCN_LNK_REMOVE                        0x00000800
#define PECOFF_IMAGE_SCN_LNK_COMDAT                        0x00001000
#define PECOFF_IMAGE_SCN_NO_DEFER_SPEC_EXC                 0x00004000
#define PECOFF_IMAGE_SCN_GPREL                             0x00008000
#define PECOFF_IMAGE_SCN_MEM_FARDATA                       0x00008000
#define PECOFF_IMAGE_SCN_MEM_PURGEABLE                     0x00020000
#define PECOFF_IMAGE_SCN_MEM_16BIT                         0x00020000
#define PECOFF_IMAGE_SCN_MEM_LOCKED                        0x00040000
#define PECOFF_IMAGE_SCN_MEM_PRELOAD                       0x00080000
#define PECOFF_IMAGE_SCN_ALIGN_1BYTES                      0x00100000
#define PECOFF_IMAGE_SCN_ALIGN_2BYTES                      0x00200000
#define PECOFF_IMAGE_SCN_ALIGN_4BYTES                      0x00300000
#define PECOFF_IMAGE_SCN_ALIGN_8BYTES                      0x00400000
#define PECOFF_IMAGE_SCN_ALIGN_16BYTES                     0x00500000
#define PECOFF_IMAGE_SCN_ALIGN_32BYTES                     0x00600000
#define PECOFF_IMAGE_SCN_ALIGN_64BYTES                     0x00700000
#define PECOFF_IMAGE_SCN_ALIGN_128BYTES                    0x00800000
#define PECOFF_IMAGE_SCN_ALIGN_256BYTES                    0x00900000
#define PECOFF_IMAGE_SCN_ALIGN_512BYTES                    0x00A00000
#define PECOFF_IMAGE_SCN_ALIGN_1024BYTES                   0x00B00000
#define PECOFF_IMAGE_SCN_ALIGN_2048BYTES                   0x00C00000
#define PECOFF_IMAGE_SCN_ALIGN_4096BYTES                   0x00D00000
#define PECOFF_IMAGE_SCN_ALIGN_8192BYTES                   0x00E00000
#define PECOFF_IMAGE_SCN_ALIGN_MASK                        0x00F00000
#define PECOFF_IMAGE_SCN_LNK_NRELOC_OVFL                   0x01000000
#define PECOFF_IMAGE_SCN_MEM_DISCARDABLE                   0x02000000
#define PECOFF_IMAGE_SCN_MEM_NOT_CACHED                    0x04000000
#define PECOFF_IMAGE_SCN_MEM_NOT_PAGED                     0x08000000
#define PECOFF_IMAGE_SCN_MEM_SHARED                        0x10000000
#define PECOFF_IMAGE_SCN_MEM_EXECUTE                       0x20000000
#define PECOFF_IMAGE_SCN_MEM_READ                          0x40000000
#define PECOFF_IMAGE_SCN_MEM_WRITE                         0x80000000

/* PE/COFF directory format */
typedef struct _PECOFF_IMAGE_DATA_DIRECTORY
{
    ULONG VirtualAddress;
    ULONG Size;
} PECOFF_IMAGE_DATA_DIRECTORY, *PPECOFF_IMAGE_DATA_DIRECTORY;

/* PE file image header */
typedef struct _PECOFF_IMAGE_FILE_HEADER
{
    USHORT Machine;
    USHORT NumberOfSections;
    ULONG TimeDateStamp;
    ULONG PointerToSymbolTable;
    ULONG NumberOfSymbols;
    USHORT SizeOfOptionalHeader;
    USHORT Characteristics;
} PECOFF_IMAGE_FILE_HEADER, *PPECOFF_IMAGE_FILE_HEADER;

/* DOS PE image header */
typedef struct _PECOFF_IMAGE_DOS_HEADER
{
    USHORT e_magic;
    USHORT e_cblp;
    USHORT e_cp;
    USHORT e_crlc;
    USHORT e_cparhdr;
    USHORT e_minalloc;
    USHORT e_maxalloc;
    USHORT e_ss;
    USHORT e_sp;
    USHORT e_csum;
    USHORT e_ip;
    USHORT e_cs;
    USHORT e_lfarlc;
    USHORT e_ovno;
    USHORT e_res[4];
    USHORT e_oemid;
    USHORT e_oeminfo;
    USHORT e_res2[10];
    LONG e_lfanew;
} PECOFF_IMAGE_DOS_HEADER, *PPECOFF_IMAGE_DOS_HEADER;

/* OS/2 PE image header */
typedef struct _PECOFF_IMAGE_OS2_HEADER
{
    USHORT ne_magic;
    CHAR ne_ver;
    CHAR ne_rev;
    USHORT ne_enttab;
    USHORT ne_cbenttab;
    LONG ne_crc;
    USHORT ne_flags;
    USHORT ne_autodata;
    USHORT ne_heap;
    USHORT ne_stack;
    LONG ne_csip;
    LONG ne_sssp;
    USHORT ne_cseg;
    USHORT ne_cmod;
    USHORT ne_cbnrestab;
    USHORT ne_segtab;
    USHORT ne_rsrctab;
    USHORT ne_restab;
    USHORT ne_modtab;
    USHORT ne_imptab;
    LONG ne_nrestab;
    USHORT ne_cmovent;
    USHORT ne_align;
    USHORT ne_cres;
    UCHAR ne_exetyp;
    UCHAR ne_flagsothers;
    USHORT ne_pretthunks;
    USHORT ne_psegrefbytes;
    USHORT ne_swaparea;
    USHORT ne_expver;
} PECOFF_IMAGE_OS2_HEADER, *PPECOFF_IMAGE_OS2_HEADER;

/* Windows VXD PE image header */
typedef struct _PECOFF_IMAGE_VXD_HEADER
{
    USHORT e32_magic;
    UCHAR e32_border;
    UCHAR e32_worder;
    ULONG e32_level;
    USHORT e32_cpu;
    USHORT e32_os;
    ULONG e32_ver;
    ULONG e32_mflags;
    ULONG e32_mpages;
    ULONG e32_startobj;
    ULONG e32_eip;
    ULONG e32_stackobj;
    ULONG e32_esp;
    ULONG e32_pagesize;
    ULONG e32_lastpagesize;
    ULONG e32_fixupsize;
    ULONG e32_fixupsum;
    ULONG e32_ldrsize;
    ULONG e32_ldrsum;
    ULONG e32_objtab;
    ULONG e32_objcnt;
    ULONG e32_objmap;
    ULONG e32_itermap;
    ULONG e32_rsrctab;
    ULONG e32_rsrccnt;
    ULONG e32_restab;
    ULONG e32_enttab;
    ULONG e32_dirtab;
    ULONG e32_dircnt;
    ULONG e32_fpagetab;
    ULONG e32_frectab;
    ULONG e32_impmod;
    ULONG e32_impmodcnt;
    ULONG e32_impproc;
    ULONG e32_pagesum;
    ULONG e32_datapage;
    ULONG e32_preload;
    ULONG e32_nrestab;
    ULONG e32_cbnrestab;
    ULONG e32_nressum;
    ULONG e32_autodata;
    ULONG e32_debuginfo;
    ULONG e32_debuglen;
    ULONG e32_instpreload;
    ULONG e32_instdemand;
    ULONG e32_heapsize;
    UCHAR e32_res3[12];
    ULONG e32_winresoff;
    ULONG e32_winreslen;
    USHORT e32_devid;
    USHORT e32_ddkver;
} PECOFF_IMAGE_VXD_HEADER, *PPECOFF_IMAGE_VXD_HEADER;

/* PE/COFF section header */
typedef struct _PECOFF_IMAGE_SECTION_HEADER
{
    UCHAR Name[PECOFF_IMAGE_SIZEOF_SHORT_NAME];
    union
    {
            ULONG PhysicalAddress;
            ULONG VirtualSize;
    } Misc;
    ULONG VirtualAddress;
    ULONG SizeOfRawData;
    ULONG PointerToRawData;
    ULONG PointerToRelocations;
    ULONG PointerToLinenumbers;
    USHORT NumberOfRelocations;
    USHORT NumberOfLinenumbers;
    ULONG Characteristics;
} PECOFF_IMAGE_SECTION_HEADER, *PPECOFF_IMAGE_SECTION_HEADER;

/* 32-bit PE/COFF image optional header */
typedef struct _PECOFF_IMAGE_OPTIONAL_HEADER32
{
    USHORT Magic;
    UCHAR MajorLinkerVersion;
    UCHAR MinorLinkerVersion;
    ULONG SizeOfCode;
    ULONG SizeOfInitializedData;
    ULONG SizeOfUninitializedData;
    ULONG AddressOfEntryPoint;
    ULONG BaseOfCode;
    ULONG BaseOfData;
    ULONG ImageBase;
    ULONG SectionAlignment;
    ULONG FileAlignment;
    USHORT MajorOperatingSystemVersion;
    USHORT MinorOperatingSystemVersion;
    USHORT MajorImageVersion;
    USHORT MinorImageVersion;
    USHORT MajorSubsystemVersion;
    USHORT MinorSubsystemVersion;
    ULONG Win32VersionValue;
    ULONG SizeOfImage;
    ULONG SizeOfHeaders;
    ULONG CheckSum;
    USHORT Subsystem;
    USHORT DllCharacteristics;
    ULONG SizeOfStackReserve;
    ULONG SizeOfStackCommit;
    ULONG SizeOfHeapReserve;
    ULONG SizeOfHeapCommit;
    ULONG LoaderFlags;
    ULONG NumberOfRvaAndSizes;
    PECOFF_IMAGE_DATA_DIRECTORY DataDirectory[PECOFF_IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} PECOFF_IMAGE_OPTIONAL_HEADER32, *PPECOFF_IMAGE_OPTIONAL_HEADER32;

/* 64-bit PE/COFF image optional header */
typedef struct _PECOFF_IMAGE_OPTIONAL_HEADER64
{
    USHORT Magic;
    UCHAR MajorLinkerVersion;
    UCHAR MinorLinkerVersion;
    ULONG SizeOfCode;
    ULONG SizeOfInitializedData;
    ULONG SizeOfUninitializedData;
    ULONG AddressOfEntryPoint;
    ULONG BaseOfCode;
    ULONGLONG ImageBase;
    ULONG SectionAlignment;
    ULONG FileAlignment;
    USHORT MajorOperatingSystemVersion;
    USHORT MinorOperatingSystemVersion;
    USHORT MajorImageVersion;
    USHORT MinorImageVersion;
    USHORT MajorSubsystemVersion;
    USHORT MinorSubsystemVersion;
    ULONG Win32VersionValue;
    ULONG SizeOfImage;
    ULONG SizeOfHeaders;
    ULONG CheckSum;
    USHORT Subsystem;
    USHORT DllCharacteristics;
    ULONGLONG SizeOfStackReserve;
    ULONGLONG SizeOfStackCommit;
    ULONGLONG SizeOfHeapReserve;
    ULONGLONG SizeOfHeapCommit;
    ULONG LoaderFlags;
    ULONG NumberOfRvaAndSizes;
    PECOFF_IMAGE_DATA_DIRECTORY DataDirectory[PECOFF_IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} PECOFF_IMAGE_OPTIONAL_HEADER64, *PPECOFF_IMAGE_OPTIONAL_HEADER64;

/* PE/COFF ROM optional header */
typedef struct _PECOFF_IMAGE_ROM_OPTIONAL_HEADER
{
    USHORT Magic;
    UCHAR MajorLinkerVersion;
    UCHAR MinorLinkerVersion;
    ULONG SizeOfCode;
    ULONG SizeOfInitializedData;
    ULONG SizeOfUninitializedData;
    ULONG AddressOfEntryPoint;
    ULONG BaseOfCode;
    ULONG BaseOfData;
    ULONG BaseOfBss;
    ULONG GprMask;
    ULONG CprMask[4];
    ULONG GpValue;
} PECOFF_IMAGE_ROM_OPTIONAL_HEADER, *PPECOFF_IMAGE_ROM_OPTIONAL_HEADER;

/* 32-bit PE/COFF NT image headers */
typedef struct _PECOFF_IMAGE_NT_HEADERS32
{
    ULONG Signature;
    PECOFF_IMAGE_FILE_HEADER FileHeader;
    PECOFF_IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} PECOFF_IMAGE_NT_HEADERS32, *PPECOFF_IMAGE_NT_HEADERS32;

/* 64-bit PE/COFF NT image headers */
typedef struct _PECOFF_IMAGE_NT_HEADERS64 {
    ULONG Signature;
    PECOFF_IMAGE_FILE_HEADER FileHeader;
    PECOFF_IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} PECOFF_IMAGE_NT_HEADERS64, *PPECOFF_IMAGE_NT_HEADERS64;

/* PE/COFF ROM image headers */
typedef struct _PECOFF_IMAGE_ROM_HEADERS {
    PECOFF_IMAGE_FILE_HEADER FileHeader;
    PECOFF_IMAGE_ROM_OPTIONAL_HEADER OptionalHeader;
} PECOFF_IMAGE_ROM_HEADERS, *PPECOFF_IMAGE_ROM_HEADERS;

/* PE/COFF based relocation format */
typedef struct _PECOFF_IMAGE_BASE_RELOCATION
{
    ULONG VirtualAddress;
    ULONG SizeOfBlock;
} PECOFF_IMAGE_BASE_RELOCATION, *PPECOFF_IMAGE_BASE_RELOCATION;

/* PE/COFF image load config code integrity */
typedef struct _PECOFF_IMAGE_LOAD_CONFIG_CODE_INTEGRITY
{
    USHORT Flags;
    USHORT Catalog;
    ULONG CatalogOffset;
    ULONG Reserved;
} PECOFF_IMAGE_LOAD_CONFIG_CODE_INTEGRITY, *PPECOFF_IMAGE_LOAD_CONFIG_CODE_INTEGRITY;

/* 32-bit load configuration directory */
typedef struct _PECOFF_IMAGE_LOAD_CONFIG_DIRECTORY32
{
    ULONG Size;
    ULONG TimeDateStamp;
    USHORT MajorVersion;
    USHORT MinorVersion;
    ULONG GlobalFlagsClear;
    ULONG GlobalFlagsSet;
    ULONG CriticalSectionDefaultTimeout;
    ULONG DeCommitFreeBlockThreshold;
    ULONG DeCommitTotalFreeThreshold;
    ULONG LockPrefixTable;
    ULONG MaximumAllocationSize;
    ULONG VirtualMemoryThreshold;
    ULONG ProcessHeapFlags;
    ULONG ProcessAffinityMask;
    USHORT CSDVersion;
    USHORT Reserved1;
    ULONG EditList;
    ULONG SecurityCookie;
    ULONG SEHandlerTable;
    ULONG SEHandlerCount;
} PECOFF_IMAGE_LOAD_CONFIG_DIRECTORY32, *PPECOFF_IMAGE_LOAD_CONFIG_DIRECTORY32;

/* 64-bit load configuration directory */
typedef struct _PECOFF_IMAGE_LOAD_CONFIG_DIRECTORY64
{
    ULONG Size;
    ULONG TimeDateStamp;
    USHORT MajorVersion;
    USHORT MinorVersion;
    ULONG GlobalFlagsClear;
    ULONG GlobalFlagsSet;
    ULONG CriticalSectionDefaultTimeout;
    ULONGLONG DeCommitFreeBlockThreshold;
    ULONGLONG DeCommitTotalFreeThreshold;
    ULONGLONG LockPrefixTable;
    ULONGLONG MaximumAllocationSize;
    ULONGLONG VirtualMemoryThreshold;
    ULONGLONG ProcessAffinityMask;
    ULONG ProcessHeapFlags;
    USHORT CSDVersion;
    USHORT DependentLoadFlags;
    ULONGLONG EditList;
    ULONGLONG SecurityCookie;
    ULONGLONG SEHandlerTable;
    ULONGLONG SEHandlerCount;
    ULONGLONG GuardCFCheckFunctionPointer;
    ULONGLONG GuardCFDispatchFunctionPointer;
    ULONGLONG GuardCFFunctionTable;
    ULONGLONG GuardCFFunctionCount;
    ULONG GuardFlags;
    PECOFF_IMAGE_LOAD_CONFIG_CODE_INTEGRITY CodeIntegrity;
    ULONGLONG GuardAddressTakenIatEntryTable;
    ULONGLONG GuardAddressTakenIatEntryCount;
    ULONGLONG GuardLongJumpTargetTable;
    ULONGLONG GuardLongJumpTargetCount;
    ULONGLONG DynamicValueRelocTable;
    ULONGLONG CHPEMetadataPointer;
    ULONGLONG GuardRFFailureRoutine;
    ULONGLONG GuardRFFailureRoutineFunctionPointer;
    ULONG DynamicValueRelocTableOffset;
    USHORT DynamicValueRelocTableSection;
    USHORT Reserved2;
    ULONGLONG GuardRFVerifyStackPointerFunctionPointer;
    ULONG HotPatchTableOffset;
    ULONG Reserved3;
    ULONGLONG EnclaveConfigurationPointer;
    ULONGLONG VolatileMetadataPointer;
} PECOFF_IMAGE_LOAD_CONFIG_DIRECTORY64, *PPECOFF_IMAGE_LOAD_CONFIG_DIRECTORY64;

/* PE/COFF image import descriptor */
typedef struct _PECOFF_IMAGE_IMPORT_DESCRIPTOR
{
    union
    {
        ULONG Characteristics;
        ULONG OriginalFirstThunk;
    };
    ULONG TimeDateStamp;
    ULONG ForwarderChain;
    ULONG Name;
    ULONG FirstThunk;
} PECOFF_IMAGE_IMPORT_DESCRIPTOR, *PPECOFF_IMAGE_IMPORT_DESCRIPTOR;

/* PE/COFF image export directory */
typedef struct _PECOFF_IMAGE_EXPORT_DIRECTORY
{
    ULONG Characteristics;
    ULONG TimeDateStamp;
    USHORT MajorVersion;
    USHORT MinorVersion;
    ULONG Name;
    ULONG Base;
    ULONG NumberOfFunctions;
    ULONG NumberOfNames;
    ULONG AddressOfFunctions;
    ULONG AddressOfNames;
    ULONG AddressOfNameOrdinals;
} PECOFF_IMAGE_EXPORT_DIRECTORY, *PPECOFF_IMAGE_EXPORT_DIRECTORY;

/* PE/COFF image resource directory */
typedef struct _PECOFF_IMAGE_RESOURCE_DIRECTORY
{
    ULONG Characteristics;
    ULONG TimeDateStamp;
    USHORT MajorVersion;
    USHORT MinorVersion;
    USHORT NumberOfNamedEntries;
    USHORT NumberOfIdEntries;
} PECOFF_IMAGE_RESOURCE_DIRECTORY, *PPECOFF_IMAGE_RESOURCE_DIRECTORY;

/* PE/COFF image resource directory entry */
typedef struct _PECOFF_IMAGE_RESOURCE_DIRECTORY_ENTRY
{
    union
    {
        struct
        {
            ULONG NameOffset:31;
            ULONG NameIsString:1;
        };
        ULONG Name;
        USHORT Id;
    };
    union
    {
        ULONG OffsetToData;
        struct {
            ULONG OffsetToDirectory:31;
            ULONG DataIsDirectory:1;
        };
    };
} PECOFF_IMAGE_RESOURCE_DIRECTORY_ENTRY, *PPECOFF_IMAGE_RESOURCE_DIRECTORY_ENTRY;

/* PE/COFF image resource data entry */
typedef struct _PECOFF_IMAGE_RESOURCE_DATA_ENTRY
{
    ULONG OffsetToData;
    ULONG Size;
    ULONG CodePage;
    ULONG Reserved;
} PECOFF_IMAGE_RESOURCE_DATA_ENTRY, *PPECOFF_IMAGE_RESOURCE_DATA_ENTRY;

#endif /* __XTDK_XTIMAGE_H */
