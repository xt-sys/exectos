/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtfw.h
 * DESCRIPTION:     EFI-dependent XT structures
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTFW_H
#define __XTDK_XTFW_H

#include <xtbase.h>
#include <xttypes.h>


/* Version number of the current kernel initialization block */
#define INITIALIZATION_BLOCK_VERSION                            1

/* Version number of the current XTOS loader protocol */
#define BOOT_PROTOCOL_VERSION                                   1

/* Memory allocation structures */
typedef enum _LOADER_MEMORY_TYPE
{
    LoaderExceptionBlock,
    LoaderSystemBlock,
    LoaderFree,
    LoaderBad,
    LoaderLoadedProgram,
    LoaderFirmwareTemporary,
    LoaderFirmwarePermanent,
    LoaderOsloaderHeap,
    LoaderOsloaderStack,
    LoaderSystemCode,
    LoaderHalCode,
    LoaderBootDriver,
    LoaderConsoleInDriver,
    LoaderConsoleOutDriver,
    LoaderStartupDpcStack,
    LoaderStartupKernelStack,
    LoaderStartupPanicStack,
    LoaderStartupPcrPage,
    LoaderStartupPdrPage,
    LoaderRegistryData,
    LoaderMemoryData,
    LoaderNlsData,
    LoaderSpecialMemory,
    LoaderBBTMemory,
    LoaderReserve,
    LoaderXIPRom,
    LoaderHALCachedMemory,
    LoaderMaximum
} LOADER_MEMORY_TYPE, *PLOADER_MEMORY_TYPE;

/* Firmware types enumeration list */
typedef enum _SYSTEM_FIRMWARE_TYPE
{
    SystemFirmwareInvalid,
    SystemFirmwareUnknown,
    SystemFirmwareEfi,
    SystemFirmwarePcat
} SYSTEM_FIRMWARE_TYPE, *PSYSTEM_FIRMWARE_TYPE;

/* Hardware information block */
typedef struct _HARDWARE_INFORMATION_BLOCK
{
} HARDWARE_INFORMATION_BLOCK, *PHARDWARE_INFORMATION_BLOCK;

/* PCAT Firmware information block */
typedef struct _PCAT_FIRMWARE_INFORMATION
{
    ULONG PlaceHolder;
} PCAT_FIRMWARE_INFORMATION, *PPCAT_FIRMWARE_INFORMATION;

/* UEFI Firmware information block */
typedef struct _UEFI_FIRMWARE_INFORMATION
{
    ULONG EfiVersion;
    PEFI_RUNTIME_SERVICES EfiRuntimeServices;
} UEFI_FIRMWARE_INFORMATION, *PUEFI_FIRMWARE_INFORMATION;

/* Firmware information block */
typedef struct _FIRMWARE_INFORMATION_BLOCK
{
    SYSTEM_FIRMWARE_TYPE FirmwareType;
    union
    {
        UEFI_FIRMWARE_INFORMATION EfiFirmware;
        PCAT_FIRMWARE_INFORMATION PcatFirmware;
    };
} FIRMWARE_INFORMATION_BLOCK, *PFIRMWARE_INFORMATION_BLOCK;

/* Boot Loader FrameBuffer information block */
typedef struct _LOADER_GRAPHICS_INFORMATION_BLOCK
{
    BOOLEAN Initialized;
    EFI_GRAPHICS_PROTOCOL Protocol;
    PVOID Address;
    ULONG_PTR BufferSize;
    UINT Width;
    UINT Height;
    UINT PixelsPerScanLine;
    UINT BitsPerPixel;
    UINT Pitch;
    PVOID Font;
} LOADER_GRAPHICS_INFORMATION_BLOCK, *PLOADER_GRAPHICS_INFORMATION_BLOCK;

/* Boot Loader information block */
typedef struct _LOADER_INFORMATION_BLOCK
{
    PVOID DbgPrint;
    LOADER_GRAPHICS_INFORMATION_BLOCK FrameBuffer;
} LOADER_INFORMATION_BLOCK, *PLOADER_INFORMATION_BLOCK;

/* Boot Loader memory mapping information */
typedef struct _LOADER_MEMORY_MAPPING
{
    LIST_ENTRY ListEntry;
    LOADER_MEMORY_TYPE MemoryType;
    ULONG BasePage;
    ULONG PageCount;
} LOADER_MEMORY_MAPPING, *PLOADER_MEMORY_MAPPING;

/* Loader provided information needed by the kernel to initialize */
typedef struct _KERNEL_INITIALIZATION_BLOCK
{
    ULONG BlockSize;
    ULONG BlockVersion;
    ULONG ProtocolVersion;
    PWCHAR KernelParameters;
    LIST_ENTRY LoadOrderListHead;
    LIST_ENTRY MemoryDescriptorListHead;
    LIST_ENTRY BootDriverListHead;
    HARDWARE_INFORMATION_BLOCK HardwareInformation;
    LOADER_INFORMATION_BLOCK LoaderInformation;
    FIRMWARE_INFORMATION_BLOCK FirmwareInformation;
} KERNEL_INITIALIZATION_BLOCK, *PKERNEL_INITIALIZATION_BLOCK;

#endif /* __XTDK_XTFW_H */
