/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtstruct.h
 * DESCRIPTION:     XT structures forward references
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTSTRUCT_H
#define __XTDK_XTSTRUCT_H

#include <xtdefs.h>


/* Enumeration lists forward references */
typedef enum _ADJUST_REASON ADJUST_REASON, *PADJUST_REASON;
typedef enum _BOOLEAN BOOLEAN, *PBOOLEAN;
typedef enum _EXCEPTION_DISPOSITION EXCEPTION_DISPOSITION, *PEXCEPTION_DISPOSITION;
typedef enum _EFI_ALLOCATE_TYPE EFI_ALLOCATE_TYPE, *PEFI_ALLOCATE_TYPE;
typedef enum _EFI_FRAMEWORK_CPU_DESIGNATION EFI_FRAMEWORK_CPU_DESIGNATION, *PEFI_FRAMEWORK_CPU_DESIGNATION;
typedef enum _EFI_GRAPHICS_OUTPUT_BLT_OPERATION EFI_GRAPHICS_OUTPUT_BLT_OPERATION, *PEFI_GRAPHICS_OUTPUT_BLT_OPERATION;
typedef enum _EFI_GRAPHICS_PIXEL_FORMAT EFI_GRAPHICS_PIXEL_FORMAT, *PEFI_GRAPHICS_PIXEL_FORMAT;
typedef enum _EFI_GRAPHICS_PROTOCOL EFI_GRAPHICS_PROTOCOL, *PEFI_GRAPHICS_PROTOCOL;
typedef enum _EFI_INTERFACE_TYPE EFI_INTERFACE_TYPE, *PEFI_INTERFACE_TYPE;
typedef enum _EFI_IO_OPERATION_TYPE EFI_IO_OPERATION_TYPE, *PEFI_IO_OPERATION_TYPE;
typedef enum _EFI_IO_WIDTH EFI_IO_WIDTH, *PEFI_IO_WIDTH;
typedef enum _EFI_LOCATE_SEARCH_TYPE EFI_LOCATE_SEARCH_TYPE, *PEFI_LOCATE_SEARCH_TYPE;
typedef enum _EFI_MEMORY_TYPE EFI_MEMORY_TYPE, *PEFI_MEMORY_TYPE;
typedef enum _EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION, *PEFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION;
typedef enum _EFI_PCI_IO_PROTOCOL_OPERATION EFI_PCI_IO_PROTOCOL_OPERATION, *PEFI_PCI_IO_PROTOCOL_OPERATION;
typedef enum _EFI_PCI_IO_PROTOCOL_WIDTH EFI_PCI_IO_PROTOCOL_WIDTH, *PEFI_PCI_IO_PROTOCOL_WIDTH;
typedef enum _EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION, *PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION;
typedef enum _EFI_PXE_BASE_CODE_CALLBACK_STATUS EFI_PXE_BASE_CODE_CALLBACK_STATUS, *PEFI_PXE_BASE_CODE_CALLBACK_STATUS;
typedef enum _EFI_PXE_BASE_CODE_FUNCTION EFI_PXE_BASE_CODE_FUNCTION, *PEFI_PXE_BASE_CODE_FUNCTION;
typedef enum _EFI_PXE_BASE_CODE_TFTP_OPCODE EFI_PXE_BASE_CODE_TFTP_OPCODE, *PEFI_PXE_BASE_CODE_TFTP_OPCODE;
typedef enum _EFI_RESET_TYPE EFI_RESET_TYPE, *PEFI_RESET_TYPE;
typedef enum _EFI_SIMPLE_NETWORK_STATE EFI_SIMPLE_NETWORK_STATE, *PEFI_SIMPLE_NETWORK_STATE;
typedef enum _EFI_TIMER_DELAY EFI_TIMER_DELAY, *PEFI_TIMER_DELAY;
typedef enum _EFI_UART_PARITY_TYPE EFI_UART_PARITY_TYPE, *PEFI_UART_PARITY_TYPE;
typedef enum _EFI_UART_STOP_BITS_TYPE EFI_UART_STOP_BITS_TYPE, *PEFI_UART_STOP_BITS_TYPE;
typedef enum _EFI_UNIVERSA_GRAPHICS_BLT_OPERATION EFI_UNIVERSA_GRAPHICS_BLT_OPERATION, *PEFI_UNIVERSA_GRAPHICS_BLT_OPERATION;
typedef enum _KAPC_ENVIRONMENT KAPC_ENVIRONMENT, *PKAPC_ENVIRONMENT;
typedef enum _KDPC_IMPORTANCE KDPC_IMPORTANCE, *PKDPC_IMPORTANCE;
typedef enum _KEVENT_TYPE KEVENT_TYPE, *PKEVENT_TYPE;
typedef enum _KOBJECTS KOBJECTS, *PKOBJECTS;
typedef enum _KPROCESS_STATE KPROCESS_STATE, *PKPROCESS_STATE;
typedef enum _KTHREAD_STATE KTHREAD_STATE, *PKTHREAD_STATE;
typedef enum _KTIMER_TYPE KTIMER_TYPE, *PKTIMER_TYPE;
typedef enum _LOADER_MEMORY_TYPE LOADER_MEMORY_TYPE, *PLOADER_MEMORY_TYPE;
typedef enum _MODE MODE, *PMODE;
typedef enum _SYSTEM_FIRMWARE_TYPE SYSTEM_FIRMWARE_TYPE, *PSYSTEM_FIRMWARE_TYPE;
typedef enum _WAIT_TYPE WAIT_TYPE, *PWAIT_TYPE;

/* Structures forward references */
typedef struct _ANSI_STRING ANSI_STRING, *PANSI_STRING;
typedef struct _ANSI_STRING32 ANSI_STRING32, *PANSI_STRING32;
typedef struct _ANSI_STRING64 ANSI_STRING64, *PANSI_STRING64;
typedef struct _CPPORT CPPORT, *PCPPORT;
typedef struct _CSTRING CSTRING, *PCSTRING;
typedef struct _EFI_1394_DEVICE_PATH EFI_1394_DEVICE_PATH, *PEFI_1394_DEVICE_PATH;
typedef struct _EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR, *PEFI_ACPI_ADDRESS_SPACE_DESCRIPTOR;
typedef struct _EFI_ACPI_ADR_DEVICE_PATH EFI_ACPI_ADR_DEVICE_PATH, *PEFI_ACPI_ADR_DEVICE_PATH;
typedef struct _EFI_ACPI_HID_DEVICE_PATH EFI_ACPI_HID_DEVICE_PATH, *PEFI_ACPI_HID_DEVICE_PATH;
typedef struct _EFI_ATAPI_DEVICE_PATH EFI_ATAPI_DEVICE_PATH, *PEFI_ATAPI_DEVICE_PATH;
typedef struct _EFI_ATAPI_IDENTIFY EFI_ATAPI_IDENTIFY, *PEFI_ATAPI_IDENTIFY;
typedef struct _EFI_BBS_BBS_DEVICE_PATH EFI_BBS_BBS_DEVICE_PATH, *PEFI_BBS_BBS_DEVICE_PATH;
typedef struct _EFI_BBS_STATUS_FLAGS EFI_BBS_STATUS_FLAGS, *PEFI_BBS_STATUS_FLAGS;
typedef struct _EFI_BBS_TABLE EFI_BBS_TABLE, *PEFI_BBS_TABLE;
typedef struct _EFI_BLOCK_DEVICE EFI_BLOCK_DEVICE, *PEFI_BLOCK_DEVICE;
typedef struct _EFI_BLOCK_DEVICE_DATA EFI_BLOCK_DEVICE_DATA, *PEFI_BLOCK_DEVICE_DATA;
typedef struct _EFI_BLOCK_IO_MEDIA EFI_BLOCK_IO_MEDIA, *PEFI_BLOCK_IO_MEDIA;
typedef struct _EFI_BLOCK_IO_PROTOCOL EFI_BLOCK_IO_PROTOCOL, *PEFI_BLOCK_IO_PROTOCOL;
typedef struct _EFI_BLOCK_IO2_PROTOCOL EFI_BLOCK_IO2_PROTOCOL, *PEFI_BLOCK_IO2_PROTOCOL;
typedef struct _EFI_BLOCK_IO2_TOKEN EFI_BLOCK_IO2_TOKEN, *PEFI_BLOCK_IO2_TOKEN;
typedef struct _EFI_BOOT_SERVICES EFI_BOOT_SERVICES, *PEFI_BOOT_SERVICES;
typedef struct _EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL, *PEFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL;
typedef struct _EFI_BYTE_REGS EFI_BYTE_REGS, *PEFI_BYTE_REGS;
typedef struct _EFI_CAPSULE_BLOCK_DESCRIPTOR EFI_CAPSULE_BLOCK_DESCRIPTOR, *PEFI_CAPSULE_BLOCK_DESCRIPTOR;
typedef struct _EFI_CAPSULE_HEADER EFI_CAPSULE_HEADER, *PEFI_CAPSULE_HEADER;
typedef struct _EFI_CDROM_DEVICE_PATH EFI_CDROM_DEVICE_PATH, *PEFI_CDROM_DEVICE_PATH;
typedef struct _EFI_COMPONENT_NAME_PROTOCOL EFI_COMPONENT_NAME_PROTOCOL, *PEFI_COMPONENT_NAME_PROTOCOL;
typedef struct _EFI_COMPONENT_NAME2_PROTOCOL EFI_COMPONENT_NAME2_PROTOCOL, *PEFI_COMPONENT_NAME2_PROTOCOL;
typedef struct _EFI_CONFIGURATION_TABLE EFI_CONFIGURATION_TABLE, *PEFI_CONFIGURATION_TABLE;
typedef struct _EFI_CONTROLLER_DEVICE_PATH EFI_CONTROLLER_DEVICE_PATH, *PEFI_CONTROLLER_DEVICE_PATH;
typedef struct _EFI_DEVICE_IO_PROTOCOL EFI_DEVICE_IO_PROTOCOL, *PEFI_DEVICE_IO_PROTOCOL;
typedef struct _EFI_DEVICE_LOGICAL_UNIT_DEVICE_PATH EFI_DEVICE_LOGICAL_UNIT_DEVICE_PATH, *PEFI_DEVICE_LOGICAL_UNIT_DEVICE_PATH;
typedef struct _EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL, *PEFI_DEVICE_PATH_FROM_TEXT_PROTOCOL;
typedef struct _EFI_DEVICE_PATH_PROTOCOL EFI_DEVICE_PATH_PROTOCOL, *PEFI_DEVICE_PATH_PROTOCOL;
typedef struct _EFI_DEVICE_PATH_TO_TEXT_PROTOCOL EFI_DEVICE_PATH_TO_TEXT_PROTOCOL, *PEFI_DEVICE_PATH_TO_TEXT_PROTOCOL;
typedef struct _EFI_DEVICE_PATH_UTILITIES_PROTOCOL EFI_DEVICE_PATH_UTILITIES_PROTOCOL, *PEFI_DEVICE_PATH_UTILITIES_PROTOCOL;
typedef struct _EFI_DISK_IO_PROTOCOL EFI_DISK_IO_PROTOCOL, *PEFI_DISK_IO_PROTOCOL;
typedef struct _EFI_DISK_IO2_PROTOCOL EFI_DISK_IO2_PROTOCOL, *PEFI_DISK_IO2_PROTOCOL;
typedef struct _EFI_DISK_IO2_TOKEN EFI_DISK_IO2_TOKEN, *PEFI_DISK_IO2_TOKEN;
typedef struct _EFI_DRIVER_BINDING_PROTOCOL EFI_DRIVER_BINDING_PROTOCOL, *PEFI_DRIVER_BINDING_PROTOCOL;
typedef struct _EFI_DRIVER_FAMILY_OVERRIDE_PROTOCOL EFI_DRIVER_FAMILY_OVERRIDE_PROTOCOL, *PEFI_DRIVER_FAMILY_OVERRIDE_PROTOCOL;
typedef struct _EFI_DWORD_REGS EFI_DWORD_REGS, *PEFI_DWORD_REGS;
typedef struct _EFI_EBC_PROTOCOL EFI_EBC_PROTOCOL, *PEFI_EBC_PROTOCOL;
typedef struct _EFI_EDID_ACTIVE_PROTOCOL EFI_EDID_ACTIVE_PROTOCOL, *PEFI_EDID_ACTIVE_PROTOCOL;
typedef struct _EFI_EDID_DISCOVERED_PROTOCOL EFI_EDID_DISCOVERED_PROTOCOL, *PEFI_EDID_DISCOVERED_PROTOCOL;
typedef struct _EFI_EDID_OVERRIDE_PROTOCOL EFI_EDID_OVERRIDE_PROTOCOL, *PEFI_EDID_OVERRIDE_PROTOCOL;
typedef struct _EFI_EFLAGS_REG EFLAGS_REG, *PEFI_EFLAGS_REG;
typedef struct _EFI_EXPANDED_ACPI_HID_DEVICE_PATH EFI_EXPANDED_ACPI_HID_DEVICE_PATH, *PEFI_EXPANDED_ACPI_HID_DEVICE_PATH;
typedef struct _EFI_FIBRECHANNEL_DEVICE_PATH EFI_FIBRECHANNEL_DEVICE_PATH, *PEFI_FIBRECHANNEL_DEVICE_PATH;
typedef struct _EFI_FIBRECHANNELEX_DEVICE_PATH EFI_FIBRECHANNELEX_DEVICE_PATH, *PEFI_FIBRECHANNELEX_DEVICE_PATH;
typedef struct _EFI_FILE_HANDLE EFI_FILE_HANDLE, *PEFI_FILE_HANDLE;
typedef struct _EFI_FILE_HEADER EFI_FILE_HEADER, *PEFI_FILE_HEADER;
typedef struct _EFI_FILE_INFO EFI_FILE_INFO, *PEFI_FILE_INFO;
typedef struct _EFI_FILE_IO_TOKEN EFI_FILE_IO_TOKEN, *PEFI_FILE_IO_TOKEN;
typedef struct _EFI_FILE_SYSTEM_INFO EFI_FILE_SYSTEM_INFO, *PEFI_FILE_SYSTEM_INFO;
typedef struct _EFI_FILE_SYSTEM_VOLUME_LABEL EFI_FILE_SYSTEM_VOLUME_LABEL, *PEFI_FILE_SYSTEM_VOLUME_LABEL;
typedef struct _EFI_FILEPATH_DEVICE_PATH EFI_FILEPATH_DEVICE_PATH, *PEFI_FILEPATH_DEVICE_PATH;
typedef struct _EFI_FLAGS_REG EFI_FLAGS_REG, *PEFI_FLAGS_REG;
typedef struct _EFI_FRAMEWORK_MP_HEALTH EFI_FRAMEWORK_MP_HEALTH, *PEFI_FRAMEWORK_MP_HEALTH;
typedef struct _EFI_FRAMEWORK_MP_PROCESSOR_CONTEXT EFI_FRAMEWORK_MP_PROCESSOR_CONTEXT, *PEFI_FRAMEWORK_MP_PROCESSOR_CONTEXT;
typedef struct _EFI_FRAMEWORK_MP_SERVICES_PROTOCOL EFI_FRAMEWORK_MP_SERVICES_PROTOCOL, *PEFI_FRAMEWORK_MP_SERVICES_PROTOCOL;
typedef struct _EFI_GPT_PARTITION_TABLE_HEADER EFI_GPT_PARTITION_TABLE_HEADER, *PEFI_GPT_PARTITION_TABLE_HEADER;
typedef struct _EFI_GPT_PARTITION_ENTRY EFI_GPT_PARTITION_ENTRY, *PEFI_GPT_PARTITION_ENTRY;
typedef struct _EFI_GRAPHICS_OUTPUT_BLT_PIXEL EFI_GRAPHICS_OUTPUT_BLT_PIXEL, *PEFI_GRAPHICS_OUTPUT_BLT_PIXEL;
typedef struct _EFI_GRAPHICS_OUTPUT_MODE_INFORMATION EFI_GRAPHICS_OUTPUT_MODE_INFORMATION, *PEFI_GRAPHICS_OUTPUT_MODE_INFORMATION;
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL, *PEFI_GRAPHICS_OUTPUT_PROTOCOL;
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE, *PEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;
typedef struct _EFI_GUID EFI_GUID, *PEFI_GUID;
typedef struct _EFI_HARDDRIVE_DEVICE_PATH EFI_HARDDRIVE_DEVICE_PATH, *PEFI_HARDDRIVE_DEVICE_PATH;
typedef struct _EFI_HASH_PROTOCOL EFI_HASH_PROTOCOL, *PEFI_HASH_PROTOCOL;
typedef struct _EFI_HDD_INFO EFI_HDD_INFO, *PEFI_HDD_INFO;
typedef struct _EFI_I2O_DEVICE_PATH EFI_I2O_DEVICE_PATH, *PEFI_I2O_DEVICE_PATH;
typedef struct _EFI_INFINIBAND_DEVICE_PATH EFI_INFINIBAND_DEVICE_PATH, *PEFI_INFINIBAND_DEVICE_PATH;
typedef struct _EFI_INPUT_KEY EFI_INPUT_KEY, *PEFI_INPUT_KEY;
typedef struct _EFI_IO_ACCESS EFI_IO_ACCESS, *PEFI_IO_ACCESS;
typedef struct _EFI_IPv4_ADDRESS EFI_IPv4_ADDRESS, *PEFI_IPv4_ADDRESS;
typedef struct _EFI_IPv4_DEVICE_PATH EFI_IPv4_DEVICE_PATH, *PEFI_IPv4_DEVICE_PATH;
typedef struct _EFI_IPv6_ADDRESS EFI_IPv6_ADDRESS, *PEFI_IPv6_ADDRESS;
typedef struct _EFI_IPv6_DEVICE_PATH EFI_IPv6_DEVICE_PATH, *PEFI_IPv6_DEVICE_PATH;
typedef struct _EFI_KEY_DATA EFI_KEY_DATA, *PEFI_KEY_DATA;
typedef struct _EFI_KEY_STATE EFI_KEY_STATE, *PEFI_KEY_STATE;
typedef struct _EFI_LBAL EFI_LBAL, *PEFI_LBAL;
typedef struct _EFI_LEGACY_BIOS_PROTOCOL EFI_LEGACY_BIOS_PROTOCOL, *PEFI_LEGACY_BIOS_PROTOCOL;
typedef struct _EFI_LOAD_FILE_PROTOCOL EFI_LOAD_FILE_PROTOCOL, *PEFI_LOAD_FILE_PROTOCOL;
typedef struct _EFI_LOADED_IMAGE_PROTOCOL EFI_LOADED_IMAGE_PROTOCOL, *PEFI_LOADED_IMAGE_PROTOCOL;
typedef struct _EFI_MAC_ADDR_DEVICE_PATH EFI_MAC_ADDR_DEVICE_PATH, *PEFI_MAC_ADDR_DEVICE_PATH;
typedef struct _EFI_MAC_ADDRESS EFI_MAC_ADDRESS, *PEFI_MAC_ADDRESS;
typedef struct _EFI_MANAGED_NETWORK_CONFIG_DATA EFI_MANAGED_NETWORK_CONFIG_DATA, *PEFI_MANAGED_NETWORK_CONFIG_DATA;
typedef struct _EFI_MASTER_BOOT_RECORD EFI_MASTER_BOOT_RECORD, *PEFI_MASTER_BOOT_RECORD;
typedef struct _EFI_MBR_PARTITION_RECORD EFI_MBR_PARTITION_RECORD, *PEFI_MBR_PARTITION_RECORD;
typedef struct _EFI_MEDIA_FW_VOL_DEVICE_PATH EFI_MEDIA_FW_VOL_DEVICE_PATH, *PEFI_MEDIA_FW_VOL_DEVICE_PATH;
typedef struct _EFI_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH EFI_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH, *PEFI_MEDIA_FW_VOL_FILEPATH_DEVICE_PATH;
typedef struct _EFI_MEDIA_PROTOCOL_DEVICE_PATH EFI_MEDIA_PROTOCOL_DEVICE_PATH, *PEFI_MEDIA_PROTOCOL_DEVICE_PATH;
typedef struct _EFI_MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH EFI_MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH, *PEFI_MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH;
typedef struct _EFI_MEMMAP_DEVICE_PATH EFI_MEMMAP_DEVICE_PATH, *PEFI_MEMMAP_DEVICE_PATH;
typedef struct _EFI_MEMORY_DESCRIPTOR EFI_MEMORY_DESCRIPTOR, *PEFI_MEMORY_DESCRIPTOR;
typedef struct _EFI_MEMORY_MAP EFI_MEMORY_MAP, *PEFI_MEMORY_MAP;
typedef struct _EFI_MP_SERVICES_PROTOCOL EFI_MP_SERVICES_PROTOCOL, *PEFI_MP_SERVICES_PROTOCOL;
typedef struct _EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL, *PEFI_NETWORK_INTERFACE_IDENTIFIER_INTERFACE;
typedef struct _EFI_NETWORK_STATISTICS EFI_NETWORK_STATISTICS, *PEFI_NETWORK_STATISTICS;
typedef struct _EFI_OPEN_PROTOCOL_INFORMATION_ENTRY EFI_OPEN_PROTOCOL_INFORMATION_ENTRY, *PEFI_OPEN_PROTOCOL_INFORMATION_ENTRY;
typedef struct _EFI_PARTITION_HEADER EFI_PARTITION_HEADER, *PEFI_PARTITION_HEADER;
typedef struct _EFI_PCCARD_DEVICE_PATH EFI_PCCARD_DEVICE_PATH, *PEFI_PCCARD_DEVICE_PATH;
typedef struct _EFI_PCI_DEVICE_PATH EFI_PCI_DEVICE_PATH, *PEFI_PCI_DEVICE_PATH;
typedef struct _EFI_PCI_IO_PROTOCOL EFI_PCI_IO_PROTOCOL, *PEFI_PCI_IO_PROTOCOL;
typedef struct _EFI_PCI_IO_PROTOCOL_ACCESS EFI_PCI_IO_PROTOCOL_ACCESS, *PEFI_PCI_IO_PROTOCOL_ACCESS;
typedef struct _EFI_PCI_IO_PROTOCOL_CONFIG_ACCESS EFI_PCI_IO_PROTOCOL_CONFIG_ACCESS, *PEFI_PCI_IO_PROTOCOL_CONFIG_ACCESS;
typedef struct _EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL, *PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL;
typedef struct _EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS, *PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS;
typedef struct _EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS, *PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS;
typedef struct _EFI_PIXEL_BITMASK EFI_PIXEL_BITMASK, *PEFI_PIXEL_BITMASK;
typedef struct _EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL, *PEFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL;
typedef struct _EFI_PROCESSOR_INFORMATION EFI_PROCESSOR_INFORMATION, *PEFI_PROCESSOR_INFORMATION;
typedef struct _EFI_PROCESSOR_PHYSICAL_LOCATION EFI_PROCESSOR_PHYSICAL_LOCATION, *PEFI_PROCESSOR_PHYSICAL_LOCATION;
typedef struct _EFI_PXE_BASE_CODE_ARP_ENTRY EFI_PXE_BASE_CODE_ARP_ENTRY, *PEFI_PXE_BASE_CODE_ARP_ENTRY;
typedef struct _EFI_PXE_BASE_CODE_CALLBACK_PROTOCOL EFI_PXE_BASE_CODE_CALLBACK_PROTOCOL, *PEFI_PXE_BASE_CODE_CALLBACK_PROTOCOL;
typedef struct _EFI_PXE_BASE_CODE_DHCPV4_PACKET EFI_PXE_BASE_CODE_DHCPV4_PACKET, *PEFI_PXE_BASE_CODE_DHCPV4_PACKET;
typedef struct _EFI_PXE_BASE_CODE_DHCPV6_PACKET EFI_PXE_BASE_CODE_DHCPV6_PACKET, *PEFI_PXE_BASE_CODE_DHCPV6_PACKET;
typedef struct _EFI_PXE_BASE_CODE_DISCOVER_INFO EFI_PXE_BASE_CODE_DISCOVER_INFO, *PEFI_PXE_BASE_CODE_DISCOVER_INFO;
typedef struct _EFI_PXE_BASE_CODE_ICMP_ERROR EFI_PXE_BASE_CODE_ICMP_ERROR, *PEFI_PXE_BASE_CODE_ICMP_ERROR;
typedef struct _EFI_PXE_BASE_CODE_IP_FILTER EFI_PXE_BASE_CODE_IP_FILTER, *PEFI_PXE_BASE_CODE_IP_FILTER;
typedef struct _EFI_PXE_BASE_CODE_MODE EFI_PXE_BASE_CODE_MODE, *PEFI_PXE_BASE_CODE_MODE;
typedef struct _EFI_PXE_BASE_CODE_MTFTP_INFO EFI_PXE_BASE_CODE_MTFTP_INFO, *PEFI_PXE_BASE_CODE_MTFTP_INFO;
typedef struct _EFI_PXE_BASE_CODE_PROTOCOL EFI_PXE_BASE_CODE_PROTOCOL, *PEFI_PXE_BASE_CODE_PROTOCOL;
typedef struct _EFI_PXE_BASE_CODE_ROUTE_ENTRY EFI_PXE_BASE_CODE_ROUTE_ENTRY, *PEFI_PXE_BASE_CODE_ROUTE_ENTRY;
typedef struct _EFI_PXE_BASE_CODE_SRVLIST EFI_PXE_BASE_CODE_SRVLIST, *PEFI_PXE_BASE_CODE_SRVLIST;
typedef struct _EFI_PXE_BASE_CODE_TFTP_ERROR EFI_PXE_BASE_CODE_TFTP_ERROR, *PEFI_PXE_BASE_CODE_TFTP_ERROR;
typedef struct _EFI_RL EFI_RL, *PEFI_RL;
typedef struct _EFI_RNG_PROTOCOL EFI_RNG_PROTOCOL, *PEFI_RNG_PROTOCOL;
typedef struct _EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES, *PEFI_RUNTIME_SERVICES;
typedef struct _EFI_SATA_DEVICE_PATH EFI_SATA_DEVICE_PATH, *PEFI_SATA_DEVICE_PATH;
typedef struct _EFI_SCSI_DEVICE_PATH EFI_SCSI_DEVICE_PATH, *PEFI_SCSI_DEVICE_PATH;
typedef struct _EFI_SERVICE_BINDING EFI_SERVICE_BINDING, *PEFI_SERVICE_BINDING;
typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL EFI_SIMPLE_FILE_SYSTEM_PROTOCOL, *PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL;
typedef struct _EFI_SIMPLE_NETWORK_MODE EFI_SIMPLE_NETWORK_MODE, *PEFI_SIMPLE_NETWORK_MODE;
typedef struct _EFI_SIMPLE_NETWORK_PROTOCOL EFI_SIMPLE_NETWORK_PROTOCOL, *PEFI_SIMPLE_NETWORK_PROTOCOL;
typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL, *PEFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL, *PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_MODE EFI_SIMPLE_TEXT_OUTPUT_MODE, *PEFI_SIMPLE_TEXT_OUTPUT_MODE;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL, *PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef struct _EFI_SYSTEM_TABLE EFI_SYSTEM_TABLE, *PEFI_SYSTEM_TABLE;
typedef struct _EFI_TABLE_HEADER EFI_TABLE_HEADER, *PEFI_TABLE_HEADER;
typedef struct _EFI_TIME EFI_TIME, *PEFI_TIME;
typedef struct _EFI_TIME_CAPABILITIES EFI_TIME_CAPABILITIES, *PEFI_TIME_CAPABILITIES;
typedef struct _EFI_UART_DEVICE_PATH EFI_UART_DEVICE_PATH, *PEFI_UART_DEVICE_PATH;
typedef struct _EFI_UART_IO_MODE EFI_UART_IO_MODE, *PEFI_UART_IO_MODE;
typedef struct _EFI_UART_IO_PROTOCOL EFI_UART_IO_PROTOCOL, *PEFI_UART_IO_PROTOCOL;
typedef struct _EFI_UDC_ATTRIBUTES EFI_UDC_ATTRIBUTES, *PEFI_UDC_ATTRIBUTES;
typedef struct _EFI_UKNOWN_DEVICE_VENDOR_DEVICE_PATH EFI_UNKNOWN_DEVICE_VENDOR_DEVICE_PATH, *PEFI_UNKNOWN_DEVICE_VENDOR_DEVICE_PATH;
typedef struct _EFI_UNICODE_COLLATION_PROTOCOL EFI_UNICODE_COLLATION_PROTOCOL, *PEFI_UNICODE_COLLATION_PROTOCOL;
typedef struct _EFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL EFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL, *PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL;
typedef struct _EFI_UNIVERSAL_GRAPHICS_BLT_PIXEL EFI_UNIVERSAL_GRAPHICS_BLT_PIXEL, *PEFI_UNIVERSAL_GRAPHICS_BLT_PIXEL;
typedef struct _EFI_URI_DEVICE_PATH EFI_URI_DEVICE_PATH, *PEFI_URI_DEVICE_PATH;
typedef struct _EFI_USB_CLASS_DEVICE_PATH EFI_USB_CLASS_DEVICE_PATH, *PEFI_USB_CLASS_DEVICE_PATH;
typedef struct _EFI_USB_DEVICE_PATH EFI_USB_DEVICE_PATH, *PEFI_USB_DEVICE_PATH;
typedef struct _EFI_USB_WWID_DEVICE_PATH EFI_USB_WWID_DEVICE_PATH, *PEFI_USB_WWID_DEVICE_PATH;
typedef struct _EFI_VENDOR_DEVICE_PATH EFI_VENDOR_DEVICE_PATH, *PEFI_VENDOR_DEVICE_PATH;
typedef struct _EFI_VLAN_DEVICE_PATH EFI_VLAN_DEVICE_PATH, *PEFI_VLAN_DEVICE_PATH;
typedef struct _EFI_WORD_REGS EFI_WORD_REGS, *PEFI_WORD_REGS;
typedef struct _EPROCESS EPROCESS, *PEPROCESS;
typedef struct _ETHREAD ETHREAD, *PETHREAD;
typedef struct _EX_RUNDOWN_REFERENCE EX_RUNDOWN_REFERENCE, *PEX_RUNDOWN_REFERENCE;
typedef struct _EXCEPTION_RECORD EXCEPTION_RECORD, *PEXCEPTION_RECORD;
typedef struct _EXCEPTION_REGISTRATION_RECORD EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD;
typedef struct _FIRMWARE_INFORMATION_BLOCK FIRMWARE_INFORMATION_BLOCK, *PFIRMWARE_INFORMATION_BLOCK;
typedef struct _FLOAT128 FLOAT128, *PFLOAT128;
typedef struct _GUID GUID, *PGUID;
typedef struct _KAPC KAPC, *PKAPC;
typedef struct _KAPC_STATE KAPC_STATE, *PKAPC_STATE;
typedef struct _KDPC KDPC, *PKDPC;
typedef struct _KDPC_DATA KDPC_DATA, *PKDPC_DATA;
typedef struct _KERNEL_INITIALIZATION_BLOCK KERNEL_INITIALIZATION_BLOCK, *PKERNEL_INITIALIZATION_BLOCK;
typedef struct _KEVENT KEVENT, *PKEVENT;
typedef struct _KGATE KGATE, *PKGATE;
typedef struct _KLOCK_QUEUE_HANDLE KLOCK_QUEUE_HANDLE, *PKLOCK_QUEUE_HANDLE;
typedef struct _KPROCESS KPROCESS, *PKPROCESS;
typedef struct _KQUEUE KQUEUE, *PKQUEUE;
typedef struct _KSEMAPHORE KSEMAPHORE, *PKSEMAPHORE;
typedef struct _KSERVICE_DESCRIPTOR_TABLE KSERVICE_DESCRIPTOR_TABLE, *PKSERVICE_DESCRIPTOR_TABLE;
typedef struct _KSPIN_LOCK_QUEUE KSPIN_LOCK_QUEUE, *PKSPIN_LOCK_QUEUE;
typedef struct _KTHREAD KTHREAD, *PKTHREAD;
typedef struct _KTIMER KTIMER, *PKTIMER;
typedef struct _KWAIT_BLOCK KWAIT_BLOCK, *PKWAIT_BLOCK;
typedef struct _LDR_DATA_TABLE_ENTRY LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;
typedef struct _LIST_ENTRY LIST_ENTRY, *PLIST_ENTRY;
typedef struct _LIST_ENTRY32 LIST_ENTRY32, *PLIST_ENTRY32;
typedef struct _LIST_ENTRY64 LIST_ENTRY64, *PLIST_ENTRY64;
typedef struct _LOADER_GRAPHICS_INFORMATION_BLOCK LOADER_GRAPHICS_INFORMATION_BLOCK, *PLOADER_GRAPHICS_INFORMATION_BLOCK;
typedef struct _LOADER_INFORMATION_BLOCK LOADER_INFORMATION_BLOCK, *PLOADER_INFORMATION_BLOCK;
typedef struct _LOADER_MEMORY_MAPPING LOADER_MEMORY_MAPPING, *PLOADER_MEMORY_MAPPING;
typedef struct _M128 M128, *PM128;
typedef struct _PCAT_FIRMWARE_INFORMATION PCAT_FIRMWARE_INFORMATION, *PPCAT_FIRMWARE_INFORMATION;
typedef struct _PCI_BRIDGE_CONTROL_REGISTER PCI_BRIDGE_CONTROL_REGISTER, *PPCI_BRIDGE_CONTROL_REGISTER;
typedef struct _PCI_COMMON_CONFIG PCI_COMMON_CONFIG, *PPCI_COMMON_CONFIG;
typedef struct _PCI_COMMON_HEADER PCI_COMMON_HEADER, *PPCI_COMMON_HEADER;
typedef struct _PCI_DEVICE_HEADER_TYPE_REGION PCI_DEVICE_HEADER_TYPE_REGION, *PPCI_DEVICE_HEADER_TYPE_REGION;
typedef struct _PCI_DEVICE_INDEPENDENT_REGION PCI_DEVICE_INDEPENDENT_REGION, *PPCI_DEVICE_INDEPENDENT_REGION;
typedef struct _PCI_TYPE0_DEVICE PCI_TYPE0_DEVICE, *PPCI_TYPE0_DEVICE;
typedef struct _PCI_TYPE1_DEVICE PCI_TYPE1_DEVICE, *PPCI_TYPE1_DEVICE;
typedef struct _PECOFF_IMAGE_BASE_RELOCATION PECOFF_IMAGE_BASE_RELOCATION, *PPECOFF_IMAGE_BASE_RELOCATION;
typedef struct _PECOFF_IMAGE_DATA PECOFF_IMAGE_DATA, *PPECOFF_IMAGE_DATA;
typedef struct _PECOFF_IMAGE_DATA_DIRECTORY PECOFF_IMAGE_DATA_DIRECTORY, *PPECOFF_IMAGE_DATA_DIRECTORY;
typedef struct _PECOFF_IMAGE_DOS_HEADER PECOFF_IMAGE_DOS_HEADER, *PPECOFF_IMAGE_DOS_HEADER;
typedef struct _PECOFF_IMAGE_EXPORT_DIRECTORY PECOFF_IMAGE_EXPORT_DIRECTORY, *PPECOFF_IMAGE_EXPORT_DIRECTORY;
typedef struct _PECOFF_IMAGE_FILE_HEADER PECOFF_IMAGE_FILE_HEADER, *PPECOFF_IMAGE_FILE_HEADER;
typedef struct _PECOFF_IMAGE_IMPORT_DESCRIPTOR PECOFF_IMAGE_IMPORT_DESCRIPTOR, *PPECOFF_IMAGE_IMPORT_DESCRIPTOR;
typedef struct _PECOFF_IMAGE_LOAD_CONFIG_CODE_INTEGRITY PECOFF_IMAGE_LOAD_CONFIG_CODE_INTEGRITY, *PPECOFF_IMAGE_LOAD_CONFIG_CODE_INTEGRITY;
typedef struct _PECOFF_IMAGE_LOAD_CONFIG_DIRECTORY32 PECOFF_IMAGE_LOAD_CONFIG_DIRECTORY32, *PPECOFF_IMAGE_LOAD_CONFIG_DIRECTORY32;
typedef struct _PECOFF_IMAGE_LOAD_CONFIG_DIRECTORY64 PECOFF_IMAGE_LOAD_CONFIG_DIRECTORY64, *PPECOFF_IMAGE_LOAD_CONFIG_DIRECTORY64;
typedef struct _PECOFF_IMAGE_OPTIONAL_HEADER PECOFF_IMAGE_OPTIONAL_HEADER, *PPECOFF_IMAGE_OPTIONAL_HEADER;
typedef struct _PECOFF_IMAGE_OS2_HEADER PECOFF_IMAGE_OS2_HEADER, *PPECOFF_IMAGE_OS2_HEADER;
typedef struct _PECOFF_IMAGE_PE_HEADER PECOFF_IMAGE_PE_HEADER, *PPECOFF_IMAGE_PE_HEADER;
typedef struct _PECOFF_IMAGE_RESOURCE_DATA_ENTRY PECOFF_IMAGE_RESOURCE_DATA_ENTRY, *PPECOFF_IMAGE_RESOURCE_DATA_ENTRY;
typedef struct _PECOFF_IMAGE_RESOURCE_DIRECTORY PECOFF_IMAGE_RESOURCE_DIRECTORY, *PPECOFF_IMAGE_RESOURCE_DIRECTORY;
typedef struct _PECOFF_IMAGE_RESOURCE_DIRECTORY_ENTRY PECOFF_IMAGE_RESOURCE_DIRECTORY_ENTRY, *PPECOFF_IMAGE_RESOURCE_DIRECTORY_ENTRY;
typedef struct _PECOFF_IMAGE_ROM_HEADER PECOFF_IMAGE_ROM_HEADER, *PPECOFF_IMAGE_ROM_HEADER;
typedef struct _PECOFF_IMAGE_ROM_OPTIONAL_HEADER PECOFF_IMAGE_ROM_OPTIONAL_HEADER, *PPECOFF_IMAGE_ROM_OPTIONAL_HEADER;
typedef struct _PECOFF_IMAGE_SECTION_HEADER PECOFF_IMAGE_SECTION_HEADER, *PPECOFF_IMAGE_SECTION_HEADER;
typedef struct _PECOFF_IMAGE_VXD_HEADER PECOFF_IMAGE_VXD_HEADER, *PPECOFF_IMAGE_VXD_HEADER;
typedef struct _PROCESSOR_POWER_STATE PROCESSOR_POWER_STATE, *PPROCESSOR_POWER_STATE;
typedef struct _SINGLE_LIST_ENTRY SINGLE_LIST_ENTRY, *PSINGLE_LIST_ENTRY;
typedef struct _STRING STRING, *PSTRING;
typedef struct _STRING32 STRING32, *PSTRING32;
typedef struct _STRING64 STRING64, *PSTRING64;
typedef struct _THREAD_INFORMATION_BLOCK THREAD_INFORMATION_BLOCK, *PTHREAD_INFORMATION_BLOCK;
typedef struct _UEFI_FIRMWARE_INFORMATION UEFI_FIRMWARE_INFORMATION, *PUEFI_FIRMWARE_INFORMATION;
typedef struct _UNICODE_STRING UNICODE_STRING, *PUNICODE_STRING;
typedef struct _UNICODE_STRING32 UNICODE_STRING32, *PUNICODE_STRING32;
typedef struct _UNICODE_STRING64 UNICODE_STRING64, *PUNICODE_STRING64;
typedef struct _XTBL_BOOT_PARAMETERS XTBL_BOOT_PARAMETERS, *PXTBL_BOOT_PARAMETERS;
typedef struct _XTBL_BOOT_PROTOCOL XTBL_BOOT_PROTOCOL, *PXTBL_BOOT_PROTOCOL;
typedef struct _XTBL_BOOTMENU_ITEM XTBL_BOOTMENU_ITEM, *PXTBL_BOOTMENU_ITEM;
typedef struct _XTBL_CONFIG_ENTRY XTBL_CONFIG_ENTRY, *PXTBL_CONFIG_ENTRY;
typedef struct _XTBL_CONFIG_SECTION XTBL_CONFIG_SECTION, *PXTBL_CONFIG_SECTION;
typedef struct _XTBL_DIALOG_HANDLE XTBL_DIALOG_HANDLE, *PXTBL_DIALOG_HANDLE;
typedef struct _XTBL_EXECUTABLE_IMAGE_PROTOCOL XTBL_EXECUTABLE_IMAGE_PROTOCOL, *PXTBL_EXECUTABLE_IMAGE_PROTOCOL;
typedef struct _XTBL_FRAMEBUFFER_INFORMATION XTBL_FRAMEBUFFER_INFORMATION, *PXTBL_FRAMEBUFFER_INFORMATION;
typedef struct _XTBL_FRAMEBUFFER_PROTOCOL XTBL_FRAMEBUFFER_PROTOCOL, *PXTBL_FRAMEBUFFER_PROTOCOL;
typedef struct _XTBL_KNOWN_BOOT_PROTOCOL XTBL_KNOWN_BOOT_PROTOCOL, *PXTBL_KNOWN_BOOT_PROTOCOL;
typedef struct _XTBL_LOADER_PROTOCOL XTBL_LOADER_PROTOCOL, *PXTBL_LOADER_PROTOCOL;
typedef struct _XTBL_MEMORY_MAPPING XTBL_MEMORY_MAPPING, *PXTBL_MEMORY_MAPPING;
typedef struct _XTBL_MODULE_AUTHORS XTBL_MODULE_AUTHORS, *PXTBL_MODULE_AUTHORS;
typedef struct _XTBL_MODULE_DEPS XTBL_MODULE_DEPS, *PXTBL_MODULE_DEPS;
typedef struct _XTBL_MODULE_INFO XTBL_MODULE_INFO, *PXTBL_MODULE_INFO;
typedef struct _XTBL_PAGE_MAPPING XTBL_PAGE_MAPPING, *PXTBL_PAGE_MAPPING;
typedef struct _XTBL_STATUS XTBL_STATUS, *PXTBL_STATUS;

/* Unions forward references */
typedef union _EFI_DEV_PATH EFI_DEV_PATH, *PEFI_DEV_PATH;
typedef union _EFI_DEV_PATH_PTR EFI_DEV_PATH_PTR, *PEFI_DEV_PATH_PTR;
typedef union _EFI_FRAMEWORK_MP_HEALTH_FLAGS EFI_FRAMEWORK_MP_HEALTH_FLAGS, *PEFI_FRAMEWORK_MP_HEALTH_FLAGS;
typedef union _EFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION EFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION, *PEFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION;
typedef union _EFI_HASH_OUTPUT EFI_HASH_OUTPUT, *PEFI_HASH_OUTPUT;
typedef union _EFI_IA32_REGISTER_SET EFI_IA32_REGISTER_SET, *PEFI_IA32_REGISTER_SET;
typedef union _EFI_IP_ADDRESS EFI_IP_ADDRESS, *PEFI_IP_ADDRESS;
typedef union _EFI_PXE_BASE_CODE_PACKET EFI_PXE_BASE_CODE_PACKET, *PEFI_PXE_BASE_CODE_PACKET;
typedef union _LARGE_INTEGER LARGE_INTEGER, *PLARGE_INTEGER;
typedef union _SINGLE_LIST_HEADER SINGLE_LIST_HEADER, *PSINGLE_LIST_HEADER;
typedef union _ULARGE_INTEGER ULARGE_INTEGER, *PULARGE_INTEGER;

#endif /* __XTDK_XTSTRUCT_H */
