/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtuefi.h
 * DESCRIPTION:     XT UEFI support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTUEFI_H
#define __XTDK_XTUEFI_H

#include "xttypes.h"
#include "xtstruct.h"
#include "xtcommon.h"


/* Basic UEFI types */
typedef PVOID EFI_EVENT, *PEFI_EVENT;
typedef PVOID EFI_HANDLE, *PEFI_HANDLE;
typedef UINT64 EFI_LBA, *PEFI_LBA;
typedef UINT64 EFI_PHYSICAL_ADDRESS, *PEFI_PHYSICAL_ADDRESS;
typedef UINT_PTR EFI_STATUS, *PEFI_STATUS;
typedef UINT_PTR EFI_TPL, *PEFI_TPL;
typedef UINT64 EFI_VIRTUAL_ADDRESS, *PEFI_VIRTUAL_ADDRESS;

/* Enumeration list of EFI memory allocation types */
typedef enum _EFI_ALLOCATE_TYPE
{
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
} EFI_ALLOCATE_TYPE, *PEFI_ALLOCATE_TYPE;

/* Enumeration list of reset types */
typedef enum _EFI_RESET_TYPE
{
    EfiResetCold,
    EfiResetWarm,
    EfiResetShutdown
} EFI_RESET_TYPE, *PEFI_RESET_TYPE;

/* Enumeration list of timer delay types */
typedef enum _EFI_TIMER_DELAY
{
    TimerCancel,
    TimerPeriodic,
    TimerRelative,
    TimerTypeMax
} EFI_TIMER_DELAY, *PEFI_TIMER_DELAY;

/* Enumeration list of EFI Locate Search Types */
typedef enum _EFI_LOCATE_SEARCH_TYPE
{
    AllHandles,
    ByRegisterNotify,
    ByProtocol
} EFI_LOCATE_SEARCH_TYPE, *PEFI_LOCATE_SEARCH_TYPE;

/* Enumeration list of EFI Interface Types */
typedef enum _EFI_INTERFACE_TYPE
{
    EFI_NATIVE_INTERFACE,
    EFI_PCODE_INTERFACE
} EFI_INTERFACE_TYPE, *PEFI_INTERFACE_TYPE;

/* Enumeration of memory types introduced in UEFI */
typedef enum _EFI_MEMORY_TYPE
{
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE, *PEFI_MEMORY_TYPE;

/* EFI structures forward references */
typedef struct _EFI_CAPSULE_HEADER EFI_CAPSULE_HEADER, *PEFI_CAPSULE_HEADER;
typedef struct _EFI_GUID EFI_GUID, *PEFI_GUID;
typedef struct _EFI_MEMORY_DESCRIPTOR EFI_MEMORY_DESCRIPTOR, *PEFI_MEMORY_DESCRIPTOR;
typedef struct _EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES, *PEFI_RUNTIME_SERVICES;
typedef struct _EFI_TABLE_HEADER EFI_TABLE_HEADER, *PEFI_TABLE_HEADER;
typedef struct _EFI_TIME EFI_TIME, *PEFI_TIME;
typedef struct _EFI_TIME_CAPABILITIES EFI_TIME_CAPABILITIES, *PEFI_TIME_CAPABILITIES;
typedef struct _EFI_DEVICE_PATH_PROTOCOL EFI_DEVICE_PATH_PROTOCOL, *PEFI_DEVICE_PATH_PROTOCOL;
typedef struct _EFI_CONFIGURATION_TABLE EFI_CONFIGURATION_TABLE, *PEFI_CONFIGURATION_TABLE;
typedef struct _EFI_OPEN_PROTOCOL_INFORMATION_ENTRY EFI_OPEN_PROTOCOL_INFORMATION_ENTRY, *PEFI_OPEN_PROTOCOL_INFORMATION_ENTRY;
typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL, *PEFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL, *PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_MODE EFI_SIMPLE_TEXT_OUTPUT_MODE, *PEFI_SIMPLE_TEXT_OUTPUT_MODE;
typedef struct _EFI_INPUT_KEY EFI_INPUT_KEY, *PEFI_INPUT_KEY;

/* EFI routines forward references */
typedef EFI_STATUS (*PEFI_CONVERT_POINTER)(UINT64 DebugDisposition, PVOID *Address);
typedef EFI_STATUS (*PEFI_ALLOCATE_PAGES)(EFI_ALLOCATE_TYPE Type, UINT_PTR MemoryType, UINT_PTR Pages, EFI_PHYSICAL_ADDRESS *Memory);
typedef EFI_STATUS (*PEFI_ALLOCATE_POOL)(UINT_PTR PoolType, UINT_PTR Size, PVOID *Buffer);
typedef EFI_STATUS (*PEFI_FREE_PAGES)(EFI_PHYSICAL_ADDRESS Memory, UINT_PTR Pages);
typedef EFI_STATUS (*PEFI_FREE_POOL)(PVOID Buffer);
typedef VOID (*PEFI_EVENT_NOTIFY)(EFI_EVENT Event, PVOID Context);
typedef EFI_STATUS (*PEFI_CREATE_EVENT)(UINT32 Type, EFI_TPL NotifyTpl, PEFI_EVENT_NOTIFY NotifyFunction, PVOID NotifyContext, PEFI_GUID EventGroup, PEFI_EVENT Event);
typedef EFI_STATUS (*PEFI_GET_MEMORY_MAP)(PUINT_PTR MemoryMapSize, PEFI_MEMORY_DESCRIPTOR MemoryMap, PUINT_PTR MapKey, PUINT_PTR DescriptorSize, PUINT32 DescriptorVersion);
typedef EFI_STATUS (*PEFI_GET_VARIABLE)(PUINT16 VariableName, PEFI_GUID VendorGuid, PUINT32 Attributes, PUINT_PTR DataSize, PVOID Data);
typedef EFI_STATUS (*PEFI_GET_NEXT_HIGH_MONO_COUNT)(PUINT32 HighCount);
typedef EFI_STATUS (*PEFI_GET_NEXT_VARIABLE_NAME)(PUINT_PTR VariableNameSize, PUINT16 VariableName, PEFI_GUID VendorGuid);
typedef EFI_STATUS (*PEFI_GET_TIME)(PEFI_TIME Time, PEFI_TIME_CAPABILITIES Capabilities);
typedef EFI_STATUS (*PEFI_SET_TIME)(PEFI_TIME Time);
typedef EFI_STATUS (*PEFI_SET_TIMER)(EFI_EVENT Event, EFI_TIMER_DELAY Type, UINT64 TriggerTime);
typedef EFI_STATUS (*PEFI_SIGNAL_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS (*PEFI_CLOSE_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS (*PEFI_CHECK_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS (*PEFI_CREATE_EVENT_EX)(UINT32 Type, EFI_TPL NotifyTpl, PEFI_EVENT_NOTIFY NotifyFunction, PVOID NotifyContext, PEFI_GUID EventGroup, PEFI_EVENT Event);
typedef EFI_STATUS (*PEFI_WAIT_FOR_EVENT)(UINT_PTR NumberOfEvents, PEFI_EVENT Event, PUINT_PTR Index);
typedef EFI_STATUS (*PEFI_QUERY_CAPSULE_CAPABILITIES)(PEFI_CAPSULE_HEADER *CapsuleHeaderArray, UINT_PTR CapsuleCount, PUINT64 MaximumCapsuleSize, PEFI_RESET_TYPE ResetType);
typedef EFI_STATUS (*PEFI_QUERY_VARIABLE_INFO)(UINT32 Attributes, PUINT64 MaximumVariableStorageSize, PUINT64 RemainingVariableStorageSize, PUINT64 MaximumVariableSize);
typedef EFI_STATUS (*PEFI_RAISE_TPL)(EFI_TPL NewTpl);
typedef EFI_STATUS (*PEFI_RESET_SYSTEM)(EFI_RESET_TYPE ResetType, EFI_STATUS ResetStatus, UINT_PTR DataSize, PVOID ResetData);
typedef EFI_STATUS (*PEFI_RESTORE_TPL)(EFI_TPL OldTpl);
typedef EFI_STATUS (*PEFI_UPDATE_CAPSULE)(PEFI_CAPSULE_HEADER *CapsuleHeaderArray, UINT_PTR CapsuleCount, EFI_PHYSICAL_ADDRESS ScatterGatherList);
typedef EFI_STATUS (*PEFI_SET_VARIABLE)(PUINT16 VariableName, PEFI_GUID VendorGuid, UINT32 Attributes, UINT_PTR DataSize, PVOID Data);
typedef EFI_STATUS (*PEFI_SET_VIRTUAL_ADDRESS_MAP)(UINT_PTR MemoryMapSize, UINT_PTR DescriptorSize, UINT32 DescriptorVersion, PEFI_MEMORY_DESCRIPTOR VirtualMap);
typedef EFI_STATUS (*PEFI_GET_WAKEUP_TIME)(PUCHAR Enabled, PUCHAR Pending, PEFI_TIME Time);
typedef EFI_STATUS (*PEFI_SET_WAKEUP_TIME)(UCHAR Enable, PEFI_TIME Time);
typedef EFI_STATUS (*PEFI_INSTALL_PROTOCOL_INTERFACE)(PEFI_HANDLE Handle, PEFI_GUID Protocol, EFI_INTERFACE_TYPE InterfaceType, PVOID Interface);
typedef EFI_STATUS (*PEFI_REINSTALL_PROTOCOL_INTERFACE)(EFI_HANDLE Handle, PEFI_GUID Protocol, PVOID OldInterface, PVOID NewInterface);
typedef EFI_STATUS (*PEFI_UNINSTALL_PROTOCOL_INTERFACE)(EFI_HANDLE Handle, PEFI_GUID Protocol, PVOID Interface);
typedef EFI_STATUS (*PEFI_HANDLE_PROTOCOL)(EFI_HANDLE Handle, PEFI_GUID Protocol, PVOID *Interface);
typedef EFI_STATUS (*PEFI_REGISTER_PROTOCOL_NOTIFY)(PEFI_GUID Protocol, EFI_EVENT Event, PVOID *Registration);
typedef EFI_STATUS (*PEFI_LOCATE_HANDLE)(EFI_LOCATE_SEARCH_TYPE SearchType, PEFI_GUID Protocol, PVOID SearchKey, PUINT_PTR BufferSize, PEFI_HANDLE Buffer);
typedef EFI_STATUS (*PEFI_LOCATE_DEVICE_PATH)(PEFI_GUID Protocol, PEFI_DEVICE_PATH_PROTOCOL *DevicePath, PEFI_HANDLE Device);
typedef EFI_STATUS (*PEFI_LOCATE_HANDLE_BUFFER)(EFI_LOCATE_SEARCH_TYPE SearchType, PEFI_GUID Protocol, PVOID SearchKey, PUINT_PTR NoHandles, PEFI_HANDLE *Buffer);
typedef EFI_STATUS (*PEFI_LOCATE_PROTOCOL)(PEFI_GUID Protocol, PVOID Registration, PVOID *Interface);
typedef EFI_STATUS (*PEFI_INSTALL_CONFIGURATION_TABLE)(PEFI_GUID Guid, PVOID Table);
typedef EFI_STATUS (*PEFI_IMAGE_LOAD)(UCHAR BootPolicy, EFI_HANDLE ParentImageHandle, PEFI_DEVICE_PATH_PROTOCOL DevicePath, PVOID SourceBuffer, UINT_PTR SourceSize, PEFI_HANDLE ImageHandle);
typedef EFI_STATUS (*PEFI_IMAGE_UNLOAD)(EFI_HANDLE ImageHandle);
typedef EFI_STATUS (*PEFI_IMAGE_START)(EFI_HANDLE ImageHandle, PUINT_PTR ExitDataSize, PUINT16 *ExitData);
typedef EFI_STATUS (*PEFI_EXIT)(EFI_HANDLE ImageHandle, EFI_STATUS ExitStatus, UINT_PTR ExitDataSize, PUINT16 ExitData);
typedef EFI_STATUS (*PEFI_EXIT_BOOT_SERVICES)(EFI_HANDLE ImageHandle, UINT_PTR MapKey);
typedef EFI_STATUS (*PEFI_GET_NEXT_MONOTONIC_COUNT)(PUINT64 Count);
typedef EFI_STATUS (*PEFI_STALL)(UINT_PTR Microseconds);
typedef EFI_STATUS (*PEFI_SET_WATCHDOG_TIMER)(UINT_PTR Timeout, UINT64 WatchdogCode, UINT_PTR DataSize, PUINT16 WatchdogData);
typedef EFI_STATUS (*PEFI_CONNECT_CONTROLLER)(EFI_HANDLE ControllerHandle, PEFI_HANDLE DriverImageHandle, PEFI_DEVICE_PATH_PROTOCOL RemainingDevicePath, UCHAR Recursive);
typedef EFI_STATUS (*PEFI_DISCONNECT_CONTROLLER)(EFI_HANDLE ControllerHandle, EFI_HANDLE DriverImageHandle, EFI_HANDLE ChildHandle);
typedef EFI_STATUS (*PEFI_OPEN_PROTOCOL)(EFI_HANDLE Handle, PEFI_GUID Protocol, PVOID *Interface, EFI_HANDLE AgentHandle, EFI_HANDLE ControllerHandle, UINT32 Attributes);
typedef EFI_STATUS (*PEFI_CLOSE_PROTOCOL)(EFI_HANDLE Handle, PEFI_GUID Protocol, EFI_HANDLE AgentHandle, EFI_HANDLE ControllerHandle);
typedef EFI_STATUS (*PEFI_OPEN_PROTOCOL_INFORMATION)(EFI_HANDLE Handle, PEFI_GUID Protocol, PEFI_OPEN_PROTOCOL_INFORMATION_ENTRY *EntryBuffer, PUINT_PTR EntryCount);
typedef EFI_STATUS (*PEFI_PROTOCOLS_PER_HANDLE)(EFI_HANDLE Handle, PEFI_GUID **ProtocolBuffer, PUINT_PTR ProtocolBufferCount);
typedef EFI_STATUS (*PEFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES)(PEFI_HANDLE Handle, ...);
typedef EFI_STATUS (*PEFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES)(PEFI_HANDLE Handle, ...);
typedef EFI_STATUS (*PEFI_CALCULATE_CRC32)(PVOID Data, UINT_PTR DataSize, PUINT32 Crc32);
typedef EFI_STATUS (*PEFI_COPY_MEM)(PVOID Destination, PVOID Source, UINT_PTR Length);
typedef EFI_STATUS (*PEFI_SET_MEM)(PVOID Buffer, UINT_PTR Size, UINT8 Value);
typedef EFI_STATUS (*PEFI_INPUT_RESET)(PEFI_SIMPLE_TEXT_INPUT_PROTOCOL This, UCHAR ExtendedVerification);
typedef EFI_STATUS (*PEFI_INPUT_READ_KEY)(PEFI_SIMPLE_TEXT_INPUT_PROTOCOL This, PEFI_INPUT_KEY Key);
typedef EFI_STATUS (*PEFI_TEXT_RESET)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, UCHAR ExtendedVerification);
typedef EFI_STATUS (*PEFI_TEXT_STRING)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, PUINT16 String);
typedef EFI_STATUS (*PEFI_TEXT_TEST_STRING)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, PUINT16 String);
typedef EFI_STATUS (*PEFI_TEXT_QUERY_MODE)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, UINT_PTR ModeNumber, UINT_PTR *Columns, UINT_PTR *Rows);
typedef EFI_STATUS (*PEFI_TEXT_SET_MODE)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, UINT_PTR ModeNumber);
typedef EFI_STATUS (*PEFI_TEXT_SET_ATTRIBUTE)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, UINT_PTR Attribute);
typedef EFI_STATUS (*PEFI_TEXT_CLEAR_SCREEN)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This);
typedef EFI_STATUS (*PEFI_TEXT_SET_CURSOR_POSITION)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, UINT_PTR Column, UINT_PTR Row);
typedef EFI_STATUS (*PEFI_TEXT_ENABLE_CURSOR)(PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, UCHAR Visible);

/* 128-bit buffer containing a unique identifier value */
typedef struct _EFI_GUID
{
    UINT32 Data1;
    UINT16 Data2;
    UINT16 Data3;
    UINT8 Data4[8];
} EFI_GUID, *PEFI_GUID;

/* EFI Capsule Header */
typedef struct _EFI_CAPSULE_HEADER
{
    EFI_GUID CapsuleGuid;
    UINT32 HeaderSize;
    UINT32 Flags;
    UINT32 CapsuleImageSize;
} EFI_CAPSULE_HEADER, *PEFI_CAPSULE_HEADER;

/* Definition of an EFI memory descriptor */
typedef struct _EFI_MEMORY_DESCRIPTOR
{
    UINT32 Type;
    UINT32 Pad;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS VirtualStart;
    UINT64 NumberOfPages;
    UINT64 Attribute;
} EFI_MEMORY_DESCRIPTOR, *PEFI_MEMORY_DESCRIPTOR;

/* Data structure that precedes all of the standard EFI table types */
typedef struct _EFI_TABLE_HEADER
{
    UINT64 Signature;
    UINT32 Revision;
    UINT32 HeaderSize;
    UINT32 CRC32;
    UINT32 Reserved;
} EFI_TABLE_HEADER, *PEFI_TABLE_HEADER;

/* EFI Time Abstraction */
typedef struct _EFI_TIME
{
    UINT16 Year;
    UINT8 Month;
    UINT8 Day;
    UINT8 Hour;
    UINT8 Minute;
    UINT8 Second;
    UINT8 Pad1;
    UINT32 Nanosecond;
    INT16 TimeZone;
    UINT8 Daylight;
    UINT8 PAD2;
} EFI_TIME, *PEFI_TIME;

/* Provides the capabilities of the RTC device as exposed through the EFI interfaces */
typedef struct _EFI_TIME_CAPABILITIES
{
    UINT32 Resolution;
    UINT32 Accuracy;
    UCHAR SetsToZero;
} EFI_TIME_CAPABILITIES, *PEFI_TIME_CAPABILITIES;

/* Describes the location of the device the handle is for */
typedef struct _EFI_DEVICE_PATH_PROTOCOL
{
    UINT8 Type;
    UINT8 SubType;
    UINT8 Length[2];
} EFI_DEVICE_PATH_PROTOCOL, *PEFI_DEVICE_PATH_PROTOCOL;

/* EFI Open Protocol Information Entry */
typedef struct _EFI_OPEN_PROTOCOL_INFORMATION_ENTRY
{
    EFI_HANDLE AgentHandle;
    EFI_HANDLE ControllerHandle;
    UINT32 Attributes;
    UINT32 OpenCount;
} EFI_OPEN_PROTOCOL_INFORMATION_ENTRY, *PEFI_OPEN_PROTOCOL_INFORMATION_ENTRY;

/* EFI Boot Services Table */
typedef struct _EFI_BOOT_SERVICES
{
    EFI_TABLE_HEADER Hdr;
    PEFI_RAISE_TPL RaiseTPL;
    PEFI_RESTORE_TPL RestoreTPL;
    PEFI_ALLOCATE_PAGES AllocatePages;
    PEFI_FREE_PAGES FreePages;
    PEFI_GET_MEMORY_MAP GetMemoryMap;
    PEFI_ALLOCATE_POOL AllocatePool;
    PEFI_FREE_POOL FreePool;
    PEFI_CREATE_EVENT CreateEvent;
    PEFI_SET_TIMER SetTimer;
    PEFI_WAIT_FOR_EVENT WaitForEvent;
    PEFI_SIGNAL_EVENT SignalEvent;
    PEFI_CLOSE_EVENT CloseEvent;
    PEFI_CHECK_EVENT CheckEvent;
    PEFI_INSTALL_PROTOCOL_INTERFACE InstallProtocolInterface;
    PEFI_REINSTALL_PROTOCOL_INTERFACE ReinstallProtocolInterface;
    PEFI_UNINSTALL_PROTOCOL_INTERFACE UninstallProtocolInterface;
    PEFI_HANDLE_PROTOCOL HandleProtocol;
    PVOID Reserved;
    PEFI_REGISTER_PROTOCOL_NOTIFY RegisterProtocolNotify;
    PEFI_LOCATE_HANDLE LocateHandle;
    PEFI_LOCATE_DEVICE_PATH LocateDevicePath;
    PEFI_INSTALL_CONFIGURATION_TABLE InstallConfigurationTable;
    PEFI_IMAGE_LOAD LoadImage;
    PEFI_IMAGE_START StartImage;
    PEFI_EXIT Exit;
    PEFI_IMAGE_UNLOAD UnloadImage;
    PEFI_EXIT_BOOT_SERVICES ExitBootServices;
    PEFI_GET_NEXT_MONOTONIC_COUNT GetNextMonotonicCount;
    PEFI_STALL Stall;
    PEFI_SET_WATCHDOG_TIMER SetWatchdogTimer;
    PEFI_CONNECT_CONTROLLER ConnectController;
    PEFI_DISCONNECT_CONTROLLER DisconnectController;
    PEFI_OPEN_PROTOCOL OpenProtocol;
    PEFI_CLOSE_PROTOCOL CloseProtocol;
    PEFI_OPEN_PROTOCOL_INFORMATION OpenProtocolInformation;
    PEFI_PROTOCOLS_PER_HANDLE ProtocolsPerHandle;
    PEFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
    PEFI_LOCATE_PROTOCOL LocateProtocol;
    PEFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES InstallMultipleProtocolInterfaces;
    PEFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES UninstallMultipleProtocolInterfaces;
    PEFI_CALCULATE_CRC32 CalculateCrc32;
    PEFI_COPY_MEM CopyMem;
    PEFI_SET_MEM SetMem;
    PEFI_CREATE_EVENT_EX CreateEventEx;
} EFI_BOOT_SERVICES, *PEFI_BOOT_SERVICES;

/* EFI Runtime Services Table */
typedef struct _EFI_RUNTIME_SERVICES
{
    EFI_TABLE_HEADER Hdr;
    PEFI_GET_TIME GetTime;
    PEFI_SET_TIME SetTime;
    PEFI_GET_WAKEUP_TIME GetWakeupTime;
    PEFI_SET_WAKEUP_TIME SetWakeupTime;
    PEFI_SET_VIRTUAL_ADDRESS_MAP SetVirtualAddressMap;
    PEFI_CONVERT_POINTER ConvertPointer;
    PEFI_GET_VARIABLE GetVariable;
    PEFI_GET_NEXT_VARIABLE_NAME GetNextVariableName;
    PEFI_SET_VARIABLE SetVariable;
    PEFI_GET_NEXT_HIGH_MONO_COUNT GetNextHighMonotonicCount;
    PEFI_RESET_SYSTEM ResetSystem;
    PEFI_UPDATE_CAPSULE UpdateCapsule;
    PEFI_QUERY_CAPSULE_CAPABILITIES QueryCapsuleCapabilities;
    PEFI_QUERY_VARIABLE_INFO QueryVariableInfo;
} EFI_RUNTIME_SERVICES, *PEFI_RUNTIME_SERVICES;

/* Contains a set of GUID/pointer pairs comprised of the ConfigurationTable field in the EFI System Table */
typedef struct _EFI_CONFIGURATION_TABLE
{
    EFI_GUID VendorGuid;
    PVOID VendorTable;
} EFI_CONFIGURATION_TABLE, *PEFI_CONFIGURATION_TABLE;

/* Simple Text Input protocol from the UEFI 2.0 specification */
typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL
{
    PEFI_INPUT_RESET Reset;
    PEFI_INPUT_READ_KEY ReadKeyStroke;
    EFI_EVENT WaitForKey;
} EFI_SIMPLE_TEXT_INPUT_PROTOCOL, *PEFI_SIMPLE_TEXT_INPUT_PROTOCOL;

/* Simple Text Output protocol from the UEFI 2.0 specification */
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
{
    PEFI_TEXT_RESET Reset;
    PEFI_TEXT_STRING OutputString;
    PEFI_TEXT_TEST_STRING TestString;
    PEFI_TEXT_QUERY_MODE QueryMode;
    PEFI_TEXT_SET_MODE SetMode;
    PEFI_TEXT_SET_ATTRIBUTE SetAttribute;
    PEFI_TEXT_CLEAR_SCREEN ClearScreen;
    PEFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
    PEFI_TEXT_ENABLE_CURSOR EnableCursor;
    PEFI_SIMPLE_TEXT_OUTPUT_MODE Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL, *PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

/* Mode Structure pointed to by Simple Text Out protocol */
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_MODE
{
    INT32 MaxMode;
    INT32 Mode;
    INT32 Attribute;
    INT32 CursorColumn;
    INT32 CursorRow;
    UCHAR CursorVisible;
} EFI_SIMPLE_TEXT_OUTPUT_MODE, *PEFI_SIMPLE_TEXT_OUTPUT_MODE;

/* The keystroke information for the key that was pressed */
typedef struct _EFI_INPUT_KEY
{
    UINT16 ScanCode;
    UINT16 UnicodeChar;
} EFI_INPUT_KEY, *PEFI_INPUT_KEY;

/* EFI System Table */
typedef struct _EFI_SYSTEM_TABLE
{
    EFI_TABLE_HEADER Hdr;
    PUINT16 FirmwareVendor;
    UINT32 FirmwareRevision;
    EFI_HANDLE ConsoleInHandle;
    PEFI_SIMPLE_TEXT_INPUT_PROTOCOL ConIn;
    EFI_HANDLE ConsoleOutHandle;
    PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL ConOut;
    EFI_HANDLE StandardErrorHandle;
    PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL StdErr;
    PEFI_RUNTIME_SERVICES RuntimeServices;
    PEFI_BOOT_SERVICES BootServices;
    UINT_PTR NumberOfTableEntries;
    PEFI_CONFIGURATION_TABLE ConfigurationTable;
} EFI_SYSTEM_TABLE, *PEFI_SYSTEM_TABLE;

#endif /* __XTDK_XTUEFI_H */
