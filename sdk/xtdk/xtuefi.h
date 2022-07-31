/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtuefi.h
 * DESCRIPTION:     XT UEFI support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTUEFI_H
#define __XTDK_XTUEFI_H

#include "xtdefs.h"
#include "xttypes.h"
#include "xtstruct.h"
#include "xtcommon.h"


/* EFI Pages related definitions */
#define EFI_PAGE_SIZE   4096
#define EFI_PAGE_MASK   0xFFF
#define EFI_PAGE_SHIFT  12

/* DeviceType definitions according to BBS specification */
#define EFI_BBS_TYPE_FLOPPY                 0x01
#define EFI_BBS_TYPE_HARDDRIVE              0x02
#define EFI_BBS_TYPE_CDROM                  0x03
#define EFI_BBS_TYPE_PCMCIA                 0x04
#define EFI_BBS_TYPE_USB                    0x05
#define EFI_BBS_TYPE_EMBEDDED_NETWORK       0x06
#define EFI_BBS_TYPE_DEV                    0x80
#define EFI_BBS_TYPE_UNKNOWN                0xFF

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
typedef PUINT16(*PEFI_DEVICE_PATH_TO_TEXT_NODE)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DeviceNode, IN BOOLEAN DisplayOnly, IN BOOLEAN AllowShortcuts);
typedef PUINT16(*PEFI_DEVICE_PATH_TO_TEXT_PATH)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath, IN BOOLEAN DisplayOnly, IN BOOLEAN AllowShortcuts);
typedef PEFI_DEVICE_PATH_PROTOCOL(*PEFI_DEVICE_PATH_FROM_TEXT_NODE)(IN CONST PUINT16 TextDeviceNode);
typedef PEFI_DEVICE_PATH_PROTOCOL(*PEFI_DEVICE_PATH_FROM_TEXT_PATH)(IN CONST PUINT16 TextDevicePath);
typedef UINT_PTR(*PEFI_DEVICE_PATH_UTILS_GET_DEVICE_PATH_SIZE)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath);
typedef PEFI_DEVICE_PATH_PROTOCOL(*PEFI_DEVICE_PATH_UTILS_DUP_DEVICE_PATH)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath);
typedef PEFI_DEVICE_PATH_PROTOCOL(*PEFI_DEVICE_PATH_UTILS_APPEND_PATH)(IN CONST PEFI_DEVICE_PATH_PROTOCOL Src1, IN CONST PEFI_DEVICE_PATH_PROTOCOL Src2);
typedef PEFI_DEVICE_PATH_PROTOCOL(*PEFI_DEVICE_PATH_UTILS_APPEND_NODE)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath, IN CONST PEFI_DEVICE_PATH_PROTOCOL DeviceNode);
typedef PEFI_DEVICE_PATH_PROTOCOL(*PEFI_DEVICE_PATH_UTILS_APPEND_INSTANCE)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath, IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePathInstance);
typedef PEFI_DEVICE_PATH_PROTOCOL(*PEFI_DEVICE_PATH_UTILS_GET_NEXT_INSTANCE)(IN OUT PEFI_DEVICE_PATH_PROTOCOL *DevicePathInstance, OUT PUINT_PTR DevicePathInstanceSize);
typedef PEFI_DEVICE_PATH_PROTOCOL(*PEFI_DEVICE_PATH_UTILS_CREATE_NODE)(IN UINT8 NodeType, IN UINT8 NodeSubType, IN UINT16 NodeLength);
typedef BOOLEAN (*PEFI_DEVICE_PATH_UTILS_IS_MULTI_INSTANCE)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath);

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

/* EFI IPv4 network protocol */
typedef struct _EFI_IPv4_ADDRESS
{
    UINT8 Addr[4];
} EFI_IPv4_ADDRESS, *PEFI_IPv4_ADDRESS;

/* EFI IPv6 network protocol */
typedef struct _EFI_IPv6_ADDRESS
{
    UINT8 Addr[16];
} EFI_IPv6_ADDRESS, *PEFI_IPv6_ADDRESS;

/* EFI MAC address definition */
typedef struct _EFI_MAC_ADDRESS
{
    UINT8 Addr[32];
} EFI_MAC_ADDRESS, *PEFI_MAC_ADDRESS;

/* EFI network configuration data structure */
typedef struct _EFI_MANAGED_NETWORK_CONFIG_DATA
{
    UINT32 ReceivedQueueTimeoutValue;
    UINT32 TransmitQueueTimeoutValue;
    UINT16 ProtocolTypeFilter;
    BOOLEAN EnableUnicastReceive;
    BOOLEAN EnableMulticastReceive;
    BOOLEAN EnableBroadcastReceive;
    BOOLEAN EnablePromiscuousReceive;
    BOOLEAN FlushQueuesOnReset;
    BOOLEAN EnableReceiveTimestamps;
    BOOLEAN DisableBackgroundPolling;
} EFI_MANAGED_NETWORK_CONFIG_DATA, *PEFI_MANAGED_NETWORK_CONFIG_DATA;

/* Describes the location of the device the handle is for */
typedef struct _EFI_DEVICE_PATH_PROTOCOL
{
    UINT8 Type;
    UINT8 SubType;
    UINT8 Length[2];
} EFI_DEVICE_PATH_PROTOCOL, *PEFI_DEVICE_PATH_PROTOCOL;

/* PCI device path node */
typedef struct _EFI_PCI_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT8 Function;
    UINT8 Device;
} EFI_PCI_DEVICE_PATH, *PEFI_PCI_DEVICE_PATH;

/* PCCARD device path node */
typedef struct _EFI_PCCARD_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT8 FunctionNumber;
} EFI_PCCARD_DEVICE_PATH, *PEFI_PCCARD_DEVICE_PATH;

/* MemMap device path node */
typedef struct _EFI_MEMMAP_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 MemoryType;
    EFI_PHYSICAL_ADDRESS StartingAddress;
    EFI_PHYSICAL_ADDRESS EndingAddress;
} EFI_MEMMAP_DEVICE_PATH, *PEFI_MEMMAP_DEVICE_PATH;

/* Vendor device path node */
typedef struct _EFI_VENDOR_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    EFI_GUID Guid;
} EFI_VENDOR_DEVICE_PATH, *PEFI_VENDOR_DEVICE_PATH;

/* Unknown Device Vendor device path node */
typedef struct _EFI_UKNOWN_DEVICE_VENDOR_DEVICE_PATH
{
    EFI_VENDOR_DEVICE_PATH DevicePath;
    UINT8 LegacyDriveLetter;
} EFI_UNKNOWN_DEVICE_VENDOR_DEVICE_PATH, *PEFI_UNKNOWN_DEVICE_VENDOR_DEVICE_PATH;

/* Controller device path node */
typedef struct _EFI_CONTROLLER_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 Controller;
} EFI_CONTROLLER_DEVICE_PATH, *PEFI_CONTROLLER_DEVICE_PATH;

/* ACPI device path node */
typedef struct _EFI_ACPI_HID_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 HID;
    UINT32 UID;
} EFI_ACPI_HID_DEVICE_PATH, *PEFI_ACPI_HID_DEVICE_PATH;

/* Expanded ACPI device path node */
typedef struct _EFI_EXPANDED_ACPI_HID_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 HID;
    UINT32 UID;
    UINT32 CID;
    UINT8 HidStr[1];
} EFI_EXPANDED_ACPI_HID_DEVICE_PATH, *PEFI_EXPANDED_ACPI_HID_DEVICE_PATH;

/* ACPI ADR device path node */
typedef struct _EFI_ACPI_ADR_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 ADR;
} EFI_ACPI_ADR_DEVICE_PATH, *PEFI_ACPI_ADR_DEVICE_PATH;

/* ATAPI device path node */
typedef struct _EFI_ATAPI_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT8 PrimarySecondary;
    UINT8 SlaveMaster;
    UINT16 Lun;
} EFI_ATAPI_DEVICE_PATH, *PEFI_ATAPI_DEVICE_PATH;

/* SCSI device path node */
typedef struct _EFI_SCSI_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT16 Pun;
    UINT16 Lun;
} EFI_SCSI_DEVICE_PATH, *PEFI_SCSI_DEVICE_PATH;

/* Fibre Channel device path node */
typedef struct _EFI_FIBRECHANNEL_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 Reserved;
    UINT64 WWN;
    UINT64 Lun;
} EFI_FIBRECHANNEL_DEVICE_PATH, *PEFI_FIBRECHANNEL_DEVICE_PATH;

/* Fibre Channerl EX subtype device path node */
typedef struct _EFI_FIBRECHANNELEX_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header ;
    UINT32 Reserved;
    UINT8 WWN[8];
    UINT8 Lun[8];
} EFI_FIBRECHANNELEX_DEVICE_PATH, *PEFI_FIBRECHANNELEX_DEVICE_PATH;

/* 1394 device path node */
typedef struct _EFI_1394_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 Reserved;
    UINT64 Guid;
} EFI_1394_DEVICE_PATH, *PEFI_1394_DEVICE_PATH;

/* USB device path node */
typedef struct _EFI_USB_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT8 Port;
    UINT8 Endpoint;
} EFI_USB_DEVICE_PATH, *PEFI_USB_DEVICE_PATH;

/* USB WWID device path node */
typedef struct _EFI_USB_WWID_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT16 InterfaceNumber;
    UINT16 VendorId;
    UINT16 ProductId;
    UINT16 SerialNumber[1];
} EFI_USB_WWID_DEVICE_PATH, *PEFI_USB_WWID_DEVICE_PATH;

/* USB Class device path node */
typedef struct _EFI_USB_CLASS_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT16 VendorId;
    UINT16 ProductId;
    UINT8 DeviceClass;
    UINT8 DeviceSubclass;
    UINT8 DeviceProtocol;
} EFI_USB_CLASS_DEVICE_PATH, *PEFI_USB_CLASS_DEVICE_PATH;

/* SATA device path node */
typedef struct _EFI_SATA_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT16 HBAPortNumber;
    UINT16 PortMultiplierPortNumber;
    UINT16 Lun;
} EFI_SATA_DEVICE_PATH, *PEFI_SATA_DEVICE_PATH;

/* Device Logical Unit device path node */
typedef struct _EFI_DEVICE_LOGICAL_UNIT_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT8 Lun;
} EFI_DEVICE_LOGICAL_UNIT_DEVICE_PATH, *PEFI_DEVICE_LOGICAL_UNIT_DEVICE_PATH;

/* I2O device path node */
typedef struct _EFI_I2O_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 Tid;
} EFI_I2O_DEVICE_PATH, *PEFI_I2O_DEVICE_PATH;

/* MAC Address device path node */
typedef struct _EFI_MAC_ADDR_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    EFI_MAC_ADDRESS MacAddress;
    UINT8 IfType;
} EFI_MAC_ADDR_DEVICE_PATH, *PEFI_MAC_ADDR_DEVICE_PATH;

/* IPv4 device path node */
typedef struct _EFI_IPv4_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    EFI_IPv4_ADDRESS LocalIpAddress;
    EFI_IPv4_ADDRESS RemoteIpAddress;
    UINT16 LocalPort;
    UINT16 RemotePort;
    UINT16 Protocol;
    BOOLEAN StaticIpAddress;
    EFI_IPv4_ADDRESS GatewayIpAddress ;
    EFI_IPv4_ADDRESS SubnetMask ;
} EFI_IPv4_DEVICE_PATH, *PEFI_IPv4_DEVICE_PATH;

/* IPv6 device path node */
typedef struct _EFI_IPv6_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    EFI_IPv6_ADDRESS LocalIpAddress;
    EFI_IPv6_ADDRESS RemoteIpAddress;
    UINT16 LocalPort;
    UINT16 RemotePort;
    UINT16 Protocol;
    BOOLEAN IPAddressOrigin ;
    UINT8 PrefixLength ;
    EFI_IPv6_ADDRESS GatewayIpAddress ;
} EFI_IPv6_DEVICE_PATH, *PEFI_IPv6_DEVICE_PATH;

/* Uniform Resource Identifiers SubType device path node */
typedef struct _EFI_URI_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT8 Uri[1];
} EFI_URI_DEVICE_PATH, *PEFI_URI_DEVICE_PATH;

/* VLAN device path node */
typedef struct _EFI_VLAN_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT16 VlanId;
} EFI_VLAN_DEVICE_PATH, *PEFI_VLAN_DEVICE_PATH;

/* InfiniBand device path node */
typedef struct _EFI_INFINIBAND_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 ResourceFlags;
    UINT8 PortGid[16];
    UINT64 ServiceId;
    UINT64 TargetPortId;
    UINT64 DeviceId;
} EFI_INFINIBAND_DEVICE_PATH, *PEFI_INFINIBAND_DEVICE_PATH;

/* UART device path node */
typedef struct _EFI_UART_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 Reserved;
    UINT64 BaudRate;
    UINT8 DataBits;
    UINT8 Parity;
    UINT8 StopBits;
} EFI_UART_DEVICE_PATH, *PEFI_UART_DEVICE_PATH;

/* Hard Drive device path node */
typedef struct _EFI_HARDDRIVE_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 PartitionNumber;
    UINT64 PartitionStart;
    UINT64 PartitionSize;
    UINT8 Signature[16];
    UINT8 MBRType;
    UINT8 SignatureType;
} EFI_HARDDRIVE_DEVICE_PATH, *PEFI_HARDDRIVE_DEVICE_PATH;

/* CDROM device path node */
typedef struct _EFI_CDROM_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 BootEntry;
    UINT64 PartitionStart;
    UINT64 PartitionSize;
} EFI_CDROM_DEVICE_PATH, *PEFI_CDROM_DEVICE_PATH;

/* File Path device path node */
typedef struct _EFI_FILEPATH_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT16 PathName[1];
} EFI_FILEPATH_DEVICE_PATH, *PEFI_FILEPATH_DEVICE_PATH;

/* Media Protocol device path node */
typedef struct _EFI_MEDIA_PROTOCOL_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    EFI_GUID Protocol;
} EFI_MEDIA_PROTOCOL_DEVICE_PATH, *PEFI_MEDIA_PROTOCOL_DEVICE_PATH;

/* Media Firmware File SubType device path node */
typedef struct _EFI_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    EFI_GUID FvFileName;
} EFI_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH, *PEFI_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH;

/* Media Firmware Volume SubType device path node */
typedef struct _EFI_MEDIA_FW_VOL_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    EFI_GUID FvName;
} EFI_MEDIA_FW_VOL_DEVICE_PATH, *PEFI_MEDIA_FW_VOL_DEVICE_PATH;

/* Media relative offset range device path node */
typedef struct _EFI_MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH {
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT32 Reserved;
    UINT64 StartingOffset;
    UINT64 EndingOffset;
} EFI_MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH, *PEFI_MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH;

/* BIOS Boot Specification (BBS) device path node */
typedef struct _EFI_BBS_BBS_DEVICE_PATH
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    UINT16 DeviceType;
    UINT16 StatusFlag;
    UINT8 String[1];
} EFI_BBS_BBS_DEVICE_PATH, *PEFI_BBS_BBS_DEVICE_PATH;

/* EFI device path nodes union */
typedef union _EFI_DEV_PATH
{
    EFI_DEVICE_PATH_PROTOCOL DevPath;
    EFI_PCI_DEVICE_PATH Pci;
    EFI_PCCARD_DEVICE_PATH PcCard;
    EFI_MEMMAP_DEVICE_PATH MemMap;
    EFI_VENDOR_DEVICE_PATH Vendor;
    EFI_UNKNOWN_DEVICE_VENDOR_DEVICE_PATH UnknownVendor;
    EFI_CONTROLLER_DEVICE_PATH Controller;
    EFI_ACPI_HID_DEVICE_PATH Acpi;
    EFI_ATAPI_DEVICE_PATH Atapi;
    EFI_SCSI_DEVICE_PATH Scsi;
    EFI_FIBRECHANNEL_DEVICE_PATH FibreChannel;
    EFI_1394_DEVICE_PATH F1394;
    EFI_USB_DEVICE_PATH Usb;
    EFI_USB_CLASS_DEVICE_PATH UsbClass;
    EFI_I2O_DEVICE_PATH I2O;
    EFI_MAC_ADDR_DEVICE_PATH MacAddr;
    EFI_IPv4_DEVICE_PATH Ipv4;
    EFI_IPv6_DEVICE_PATH Ipv6;
    EFI_URI_DEVICE_PATH Uri;
    EFI_INFINIBAND_DEVICE_PATH InfiniBand;
    EFI_UART_DEVICE_PATH Uart;
    EFI_HARDDRIVE_DEVICE_PATH HardDrive;
    EFI_CDROM_DEVICE_PATH CD;
    EFI_FILEPATH_DEVICE_PATH FilePath;
    EFI_MEDIA_PROTOCOL_DEVICE_PATH MediaProtocol;
    EFI_BBS_BBS_DEVICE_PATH Bbs;
} EFI_DEV_PATH, *PEFI_DEV_PATH;

/* EFI device path node pointers union */
typedef union _EFI_DEV_PATH_PTR
{
    PEFI_DEVICE_PATH_PROTOCOL DevPath;
    PEFI_PCI_DEVICE_PATH Pci;
    PEFI_PCCARD_DEVICE_PATH PcCard;
    PEFI_MEMMAP_DEVICE_PATH MemMap;
    PEFI_VENDOR_DEVICE_PATH Vendor;
    PEFI_UNKNOWN_DEVICE_VENDOR_DEVICE_PATH UnknownVendor;
    PEFI_CONTROLLER_DEVICE_PATH Controller;
    PEFI_ACPI_HID_DEVICE_PATH Acpi;
    PEFI_ATAPI_DEVICE_PATH Atapi;
    PEFI_SCSI_DEVICE_PATH Scsi;
    PEFI_FIBRECHANNEL_DEVICE_PATH FibreChannel;
    PEFI_1394_DEVICE_PATH F1394;
    PEFI_USB_DEVICE_PATH Usb;
    PEFI_USB_CLASS_DEVICE_PATH UsbClass;
    PEFI_I2O_DEVICE_PATH I2O;
    PEFI_MAC_ADDR_DEVICE_PATH MacAddr;
    PEFI_IPv4_DEVICE_PATH Ipv4;
    PEFI_IPv6_DEVICE_PATH Ipv6;
    PEFI_URI_DEVICE_PATH Uri;
    PEFI_INFINIBAND_DEVICE_PATH InfiniBand;
    PEFI_UART_DEVICE_PATH Uart;
    PEFI_HARDDRIVE_DEVICE_PATH HardDrive;
    PEFI_FILEPATH_DEVICE_PATH FilePath;
    PEFI_MEDIA_PROTOCOL_DEVICE_PATH MediaProtocol;
    PEFI_CDROM_DEVICE_PATH CD;
    PEFI_BBS_BBS_DEVICE_PATH Bbs;
} EFI_DEV_PATH_PTR, *PEFI_DEV_PATH_PTR;

/* EFI device path to text protocol */
typedef struct _EFI_DEVICE_PATH_TO_TEXT_PROTOCOL
{
    PEFI_DEVICE_PATH_TO_TEXT_NODE ConvertDeviceNodeToText;
    PEFI_DEVICE_PATH_TO_TEXT_PATH ConvertDevicePathToText;
} EFI_DEVICE_PATH_TO_TEXT_PROTOCOL, *PEFI_DEVICE_PATH_TO_TEXT_PROTOCOL;

/* EFI device path from text protocol */
typedef struct _EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL
{
    PEFI_DEVICE_PATH_FROM_TEXT_NODE ConvertTextToDeviceNode;
    PEFI_DEVICE_PATH_FROM_TEXT_PATH ConvertTextToDevicePath;
} EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL, *PEFI_DEVICE_PATH_FROM_TEXT_PROTOCOL;

/* EFI device path utility protocol */
typedef struct _EFI_DEVICE_PATH_UTILITIES_PROTOCOL
{
    PEFI_DEVICE_PATH_UTILS_GET_DEVICE_PATH_SIZE GetDevicePathSize;
    PEFI_DEVICE_PATH_UTILS_DUP_DEVICE_PATH DuplicateDevicePath;
    PEFI_DEVICE_PATH_UTILS_APPEND_PATH AppendDevicePath;
    PEFI_DEVICE_PATH_UTILS_APPEND_NODE AppendDeviceNode;
    PEFI_DEVICE_PATH_UTILS_APPEND_INSTANCE AppendDevicePathInstance;
    PEFI_DEVICE_PATH_UTILS_GET_NEXT_INSTANCE GetNextDevicePathInstance;
    PEFI_DEVICE_PATH_UTILS_IS_MULTI_INSTANCE IsDevicePathMultiInstance;
    PEFI_DEVICE_PATH_UTILS_CREATE_NODE CreateDeviceNode;
} EFI_DEVICE_PATH_UTILITIES_PROTOCOL, *PEFI_DEVICE_PATH_UTILITIES_PROTOCOL;

#endif /* __XTDK_XTUEFI_H */
