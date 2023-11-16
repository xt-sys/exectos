/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtuefi.h
 * DESCRIPTION:     XT UEFI support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTUEFI_H
#define __XTDK_XTUEFI_H

#include <xtbase.h>
#include <xtdefs.h>
#include <xttypes.h>
#include <xtstruct.h>


/* EFI service signatures */
#define EFI_BOOT_SERVICES_SIGNATURE                        0x56524553544f4f42
#define EFI_RUNTIME_SERVICES_SIGNATURE                     0x56524553544e5552
#define EFI_SYSTEM_TABLE_SIGNATURE                         0x5453595320494249

/* Caching types for the memory range */
#define EFI_MEMORY_UC                                      0x0000000000000001
#define EFI_MEMORY_WC                                      0x0000000000000002
#define EFI_MEMORY_WT                                      0x0000000000000004
#define EFI_MEMORY_WB                                      0x0000000000000008
#define EFI_MEMORY_UCE                                     0x0000000000000010

/* Physical memory protection on range */
#define EFI_MEMORY_WP                                      0x0000000000001000
#define EFI_MEMORY_RP                                      0x0000000000002000
#define EFI_MEMORY_XP                                      0x0000000000004000
#define EFI_MEMORY_RO                                      0x0000000000020000

/* Physical memory persistence attribute */
#define EFI_MEMORY_NV                                      0x0000000000008000

/* Memory region provides higher reliability relative to other memory in the system */
#define EFI_MEMORY_MORE_RELIABLE                           0x0000000000010000

/* Specific-purpose memory (SPM) */
#define EFI_MEMORY_SP                                      0x0000000000040000

/* Memory region capable of being protected with the CPU's cryptography */
#define EFI_MEMORY_CPU_CRYPTO                              0x0000000000080000

/* EFI Runtime memory attribute */
#define EFI_MEMORY_RUNTIME                                 0x8000000000000000

/* EFI Pages related definitions */
#define EFI_PAGE_SIZE                                      4096
#define EFI_PAGE_MASK                                      0xFFF
#define EFI_PAGE_SHIFT                                     12

/* EFI open protocol */
#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL               0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL                     0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL                    0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER              0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER                        0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE                        0x00000020

/* EFI capsule flags */
#define EFI_CAPSULE_FLAGS_PERSIST_ACROSS_RESET             0x00010000
#define EFI_CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE            0x00020000
#define EFI_CAPSULE_FLAGS_INITIATE_RESET                   0x00040000

/* EFI variables related definitions */
#define EFI_VARIABLE_NON_VOLATILE                          0x00000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS                    0x00000002
#define EFI_VARIABLE_RUNTIME_ACCESS                        0x00000004
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD                 0x00000008
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS            0x00000010
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x00000020
#define EFI_VARIABLE_APPEND_WRITE                          0x00000040

/* Maximum EFI variable size */
#define EFI_MAXIMUM_VARIABLE_SIZE                          1024

/* EFI event types */
#define EFI_EVENT_TIMER                                    0x80000000
#define EFI_EVENT_RUNTIME                                  0x40000000
#define EFI_EVENT_RUNTIME_CONTEXT                          0x20000000
#define EFI_EVENT_NOTIFY_WAIT                              0x00000100
#define EFI_EVENT_NOTIFY_SIGNAL                            0x00000200
#define EFI_EVENT_SIGNAL_EXIT_BOOT_SERVICES                0x00000201
#define EFI_EVENT_SIGNAL_VIRTUAL_ADDRESS_CHANGE            0x60000202
#define EFI_EVENT_EFI_SIGNAL_MASK                          0x000000FF

/* EFI disk signature type */
#define EFI_DISK_SIGNATURE_TYPE_MBR                        0x01
#define EFI_DISK_SIGNATURE_TYPE_GPT                        0x02

/* EFI device path types */
#define EFI_HARDWARE_DEVICE_PATH                           0x01
#define EFI_ACPI_DEVICE_PATH                               0x02
#define EFI_MESSAGING_DEVICE_PATH                          0x03
#define EFI_MEDIA_DEVICE_PATH                              0x04
#define EFI_BBS_DEVICE_PATH                                0x05
#define EFI_END_DEVICE_PATH                                0x7F

/* EFI hardware device path subtypes */
#define EFI_HARDWARE_PCI_DP                                0x01
#define EFI_HARDWARE_PCCARD_DP                             0x02
#define EFI_HARDWARE_MEMMAP_DP                             0x03
#define EFI_HARDWARE_VENDOR_DP                             0x04
#define EFI_HARDWARE_CONTROLLER_DP                         0x05

/* EFI ACPI device path subtypes */
#define EFI_ACPI_DP                                        0x01
#define EFI_ACPI_EXPANDED_DP                               0x02

/* EFI messaging device path subtypes */
#define EFI_MESSAGING_ATAPI_DP                             0x01
#define EFI_MESSAGING_SCSI_DP                              0x02
#define EFI_MESSAGING_FIBRECHANNEL_DP                      0x03
#define EFI_MESSAGING_1394_DP                              0x04
#define EFI_MESSAGING_USB_DP                               0x05
#define EFI_MESSAGING_I2O_DP                               0x06
#define EFI_MESSAGING_INFINIBAND_DP                        0x09
#define EFI_MESSAGING_VENDOR_DP                            0x0A
#define EFI_MESSAGING_MAC_ADDR_DP                          0x0B
#define EFI_MESSAGING_IPv4_DP                              0x0C
#define EFI_MESSAGING_IPv6_DP                              0x0D
#define EFI_MESSAGING_UART_DP                              0x0E
#define EFI_MESSAGING_USB_CLASS_DP                         0x0F
#define EFI_MESSAGING_USB_WWID_DP                          0x10
#define EFI_MESSAGING_DEVICE_LOGICAL_UNIT_DP               0x11
#define EFI_MESSAGING_SATA_DP                              0x12
#define EFI_MESSAGING_VLAN_DP                              0x14
#define EFI_MESSAGING_FIBRECHANNELEX_DP                    0x15
#define EFI_MESSAGING_URI_DP                               0x18

/* EFI media device path subtypes */
#define EFI_MEDIA_HARDDRIVE_DP                             0x01
#define EFI_MEDIA_CDROM_DP                                 0x02
#define EFI_MEDIA_VENDOR_DP                                0x03
#define EFI_MEDIA_FILEPATH_DP                              0x04
#define EFI_MEDIA_PROTOCOL_DP                              0x05
#define EFI_MEDIA_PIWG_FW_FILE_DP                          0x06
#define EFI_MEDIA_PIWG_FW_VOL_DP                           0x07
#define EFI_MEDIA_RELATIVE_OFFSET_RANGE_DP                 0x08
#define EFI_MEDIA_RAMDISK_DP                               0x09

/* DeviceType definitions according to BBS specification */
#define EFI_BBS_TYPE_FLOPPY                                0x01
#define EFI_BBS_TYPE_HARDDRIVE                             0x02
#define EFI_BBS_TYPE_CDROM                                 0x03
#define EFI_BBS_TYPE_PCMCIA                                0x04
#define EFI_BBS_TYPE_USB                                   0x05
#define EFI_BBS_TYPE_EMBEDDED_NETWORK                      0x06
#define EFI_BBS_TYPE_DEV                                   0x80
#define EFI_BBS_TYPE_UNKNOWN                               0xFF

/* EFI end device path subtypes */
#define EFI_END_INSTANCE_DP                                0x01
#define EFI_END_UNPACKED_DP                                0x7F
#define EFI_END_ENTIRE_DP                                  0xFF

/* EFI file open modes */
#define EFI_FILE_MODE_READ                                 0x0000000000000001
#define EFI_FILE_MODE_WRITE                                0x0000000000000002
#define EFI_FILE_MODE_CREATE                               0x8000000000000000

/* EFI file attributes */
#define EFI_FILE_READ_ONLY                                 0x0000000000000001
#define EFI_FILE_HIDDEN                                    0x0000000000000002
#define EFI_FILE_SYSTEM                                    0x0000000000000004
#define EFI_FILE_RESERVIED                                 0x0000000000000008
#define EFI_FILE_DIRECTORY                                 0x0000000000000010
#define EFI_FILE_ARCHIVE                                   0x0000000000000020
#define EFI_FILE_VALID_ATTR                                0x0000000000000037

/* EFI PCI attributes */
#define EFI_PCI_ATTRIBUTE_ISA_MOTHERBOARD_IO               0x00001
#define EFI_PCI_ATTRIBUTE_ISA_IO                           0x00002
#define EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO                   0x00004
#define EFI_PCI_ATTRIBUTE_VGA_MEMORY                       0x00008
#define EFI_PCI_ATTRIBUTE_VGA_IO                           0x00010
#define EFI_PCI_ATTRIBUTE_IDE_PRIMARY_IO                   0x00020
#define EFI_PCI_ATTRIBUTE_IDE_SECONDARY_IO                 0x00040
#define EFI_PCI_ATTRIBUTE_MEMORY_WRITE_COMBINE             0x00080
#define EFI_PCI_ATTRIBUTE_IO                               0x00100
#define EFI_PCI_ATTRIBUTE_MEMORY                           0x00200
#define EFI_PCI_ATTRIBUTE_BUS_MASTER                       0x00400
#define EFI_PCI_ATTRIBUTE_MEMORY_CACHED                    0x00800
#define EFI_PCI_ATTRIBUTE_MEMORY_DISABLE                   0x01000
#define EFI_PCI_ATTRIBUTE_EMBEDDED_DEVICE                  0x02000
#define EFI_PCI_ATTRIBUTE_EMBEDDED_ROM                     0x04000
#define EFI_PCI_ATTRIBUTE_DUAL_ADDRESS_CYCLE               0x08000
#define EFI_PCI_ATTRIBUTE_ISA_IO_16                        0x10000
#define EFI_PCI_ATTRIBUTE_VGA_PALETTE_IO_16                0x20000
#define EFI_PCI_ATTRIBUTE_VGA_IO_16                        0x40000

/* EFI UART attributes */
#define EFI_UART_CLEAR_TO_SEND                             0x0010
#define EFI_UART_DATA_SET_READY                            0x0020
#define EFI_UART_RING_INDICATE                             0x0040
#define EFI_UART_CARRIER_DETECT                            0x0080
#define EFI_UART_REQUEST_TO_SEND                           0x0002
#define EFI_UART_DATA_TERMINAL_READY                       0x0001
#define EFI_UART_INPUT_BUFFER_EMPTY                        0x0100
#define EFI_UART_OUTPUT_BUFFER_EMPTY                       0x0200
#define EFI_UART_HARDWARE_LOOPBACK_ENABLE                  0x1000
#define EFI_UART_SOFTWARE_LOOPBACK_ENABLE                  0x2000
#define EFI_UART_HARDWARE_FLOW_CONTROL_ENABLE              0x4000

/* EFI Simple Network protocol */
#define EFI_SIMPLE_NETWORK_RECEIVE_UNICAST                 0x01
#define EFI_SIMPLE_NETWORK_RECEIVE_MULTICAST               0x02
#define EFI_SIMPLE_NETWORK_RECEIVE_BROADCAST               0x04
#define EFI_SIMPLE_NETWORK_RECEIVE_PROMISCUOUS             0x08
#define EFI_SIMPLE_NETWORK_RECEIVE_PROMISCUOUS_MULTICAST   0x10

/* EFI Processor StatusFlag bits */
#define EFI_PROCESSOR_AS_BSP_BIT                           0x00000001
#define EFI_PROCESSOR_ENABLED_BIT                          0x00000002
#define EFI_ROCESSOR_HEALTH_STATUS_BIT                     0x00000004

/* EFI list of failed CPUs termination */
#define EFI_PROCESSOR_END_OF_LIST                          0xFFFFFFFF

/* EFI protocols GUIDs */
#define EFI_BLOCK_IO_PROTOCOL_GUID                         {0x964E5B21, 0x6459, 0x11D2, {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#define EFI_BLOCK_IO2_PROTOCOL_GUID                        {0xA77B2472, 0xE282, 0x4E9F, {0xA2, 0x45, 0xC2, 0xC0, 0xE2, 0x7B, 0xBC, 0xC1}}
#define EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL_GUID     {0x3BC1B285, 0x8A15, 0x4A82, {0xAA, 0xBF, 0x4D, 0x7D, 0x13, 0xFB, 0x32, 0x65}}
#define EFI_COMPONENT_NAME_PROTOCOL_GUID                   {0x107A772C, 0xD5E1, 0x11D4, {0x9A, 0x46, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}}
#define EFI_COMPONENT_NAME2_PROTOCOL_GUID                  {0x6A7A5CFF, 0xE8D9, 0x4F70, {0xBA, 0xDA, 0x75, 0xAB, 0x30, 0x25, 0xCE, 0x14}}
#define EFI_DEBUG_IMAGE_INFO_TABLE_GUID                    {0x49152E77, 0x1ADA, 0x4764, {0xB7, 0xA2, 0x7A, 0xFE, 0xFE, 0xD9, 0x5E, 0x8B}}
#define EFI_DEBUG_SUPPORT_PROTOCOL_GUID                    {0x2755590C, 0x6F3C, 0x42FA, {0x9E, 0xA4, 0xA3, 0xBA, 0x54, 0x3C, 0xDA, 0x25}}
#define EFI_DEVICE_IO_PROTOCOL_GUID                        {0xAF6AC311, 0x84C3, 0x11D2, {0x8E, 0x3C, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#define EFI_DEVICE_PATH_PROTOCOL_GUID                      {0x09576E91, 0x6D3F, 0x11D2, {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#define EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL_GUID            {0x05C99A21, 0xC70F, 0x4AD2, {0x8A, 0x5F, 0x35, 0xDF, 0x33, 0x43, 0xF5, 0x1E}}
#define EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID              {0x8B843E20, 0x8132, 0x4852, {0x90, 0xCC, 0x55, 0x1A, 0x4E, 0x4A, 0x7F, 0x1C}}
#define EFI_DEVICE_PATH_UTILITIES_PROTOCOL_GUID            {0x0379BE4E, 0xD706, 0x437D, {0xB0, 0x37, 0xED, 0xB8, 0x2F, 0xB7, 0x72, 0xA4}}
#define EFI_DISK_IO_PROTOCOL_GUID                          {0xCE345171, 0xBA0B, 0x11D2, {0x8E, 0x4F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#define EFI_DISK_IO2_PROTOCOL_GUID                         {0x151C8EAE, 0x7F2C, 0x472C, {0x9E, 0x54, 0x98, 0x28, 0x19, 0x4F, 0x6A, 0x88}}
#define EFI_DRIVER_BINDING_PROTOCOL_GUID                   {0x18A031AB, 0xB443, 0x4D1A, {0xA5, 0xC0, 0x0C, 0x09, 0x26, 0x1E, 0x9F, 0x71}}
#define EFI_DRIVER_FAMILY_OVERRIDE_PROTOCOL_GUID           {0xB1EE129E, 0xDA36, 0x4181, {0x91, 0xF8, 0x04, 0xA4, 0x92, 0x37, 0x66, 0xA7}}
#define EFI_EBC_INTERPRETER_PROTOCOL_GUID                  {0x13AC6DD1, 0x73D0, 0x11D4, {0xB0, 0x6B, 0x00, 0xAA, 0x00, 0xBD, 0x6D, 0xE7}}
#define EFI_EDID_ACTIVE_PROTOCOL_GUID                      {0xBD8C1056, 0x9F36, 0x44EC, {0x92, 0xA8, 0xA6, 0x33, 0x7F, 0x81, 0x79, 0x86}}
#define EFI_EDID_DISCOVERED_PROTOCOL_GUID                  {0x1C0C34F6, 0xD380, 0x41FA, {0xA0, 0x49, 0x8a, 0xD0, 0x6C, 0x1A, 0x66, 0xAA}}
#define EFI_EDID_OVERRIDE_PROTOCOL_GUID                    {0x48ECB431, 0xFB72, 0x45C0, {0xA9, 0x22, 0xF4, 0x58, 0xFE, 0x04, 0x0B, 0xD5}}
#define EFI_FILE_INFO_PROTOCOL_GUID                        {0x09576E92, 0x6D3F, 0x11D2, {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#define EFI_FPSWA_PROTOCOL_GUID                            {0xC41B6531, 0x97B9, 0x11D3, {0x9A, 0x29, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}}
#define EFI_GLOBAL_VARIABLE_GUID                           {0x8BE4DF61, 0x93CA, 0x11D2, {0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C}}
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID                  {0x9042A9DE, 0x23DC, 0x4A38, {0x96, 0xFB, 0x7A, 0xDE, 0xD0, 0x80, 0x51, 0x6A}}
#define EFI_HASH_PROTOCOL_GUID                             {0xC5184932, 0xDBA5, 0x46DB, {0xA5, 0xBA, 0xCC, 0x0B, 0xDA, 0x9C, 0x14, 0x35}}
#define EFI_LOAD_FILE_PROTOCOL_GUID                        {0x56EC3091, 0x954C, 0x11d2, {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#define EFI_LOADED_IMAGE_PROTOCOL_GUID                     {0x5B1B31A1, 0x9562, 0x11d2, {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#define EFI_LOADED_IMAGE_DEVICE_PATH_PROTOCOL_GUID         {0xBC62157E, 0x3E33, 0x4FEC, {0x99, 0x20, 0x2D, 0x3B, 0x36, 0xD7, 0x50, 0xDF}}
#define EFI_MP_SERVICES_PROTOCOL_GUID                      {0x3FDDA605, 0xA76E, 0x4F46, {0xAD, 0x29, 0x12, 0xF4, 0x53, 0x1B, 0x3D, 0x08}}
#define EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL_GUID     {0xE18541CD, 0xF755, 0x4f73, {0x92, 0x8D, 0x64, 0x3C, 0x8A, 0x79, 0xB2, 0x29}}
#define EFI_PCI_IO_PROTOCOL_GUID                           {0x4CF5B200, 0x68B8, 0x4CA5, {0x9E, 0xEC, 0xB2, 0x3E, 0x3F, 0x50, 0x02, 0x9A}}
#define EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID               {0x2F707EBB, 0x4A1A, 0x11d4, {0x9A, 0x38, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}}
#define EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL_GUID         {0x6B30C738, 0xA391, 0x11D4, {0x9A, 0x3B, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}}
#define EFI_PXE_BASE_CODE_PROTOCOL_GUID                    {0x03C4E603, 0xAC28, 0x11D3, {0x9A, 0x2D, 0x00, 0x90, 0x27, 0x3D, 0xC1, 0x4D}}
#define EFI_PXE_BASE_CODE_CALLBACK_PROTOCOL_GUID           {0x245DCA21, 0xFB7F, 0x11D3, {0x8F, 0x01, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#define EFI_RNG_PROTOCOL_GUID                              {0x3152BCA5, 0xEADE, 0x433D, {0x86, 0x2E, 0xC0, 0x1C, 0xDC, 0x29, 0x1F, 0x44}}
#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID               {0x964E5B22, 0x6459, 0x11D2, {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#define EFI_SIMPLE_NETWORK_PROTOCOL_GUID                   {0xA19832B9, 0xAC25, 0x11D3, {0x9A, 0x2D, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}}
#define EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID                {0x387477C1, 0x69C7, 0x11D2, {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#define EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID             {0xDD9E7534, 0x7762, 0x4698, {0x8C, 0x14, 0xF5, 0x85, 0x17, 0xA6, 0x25, 0xAA}}
#define EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID               {0x387477C2, 0x69C7, 0x11D2, {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}
#define EFI_UART_IO_PROTOCOL_GUID                          {0xBB25CF6F, 0xF1D4, 0x11D2, {0x9A, 0x0C, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0xFD}}
#define EFI_UNICODE_COLLATION_PROTOCOL_GUID                {0x1D85CD7F, 0xF43D, 0x11D2, {0x9A, 0x0C, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}}
#define EFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL_GUID       {0x982C298B, 0xF4FA, 0x41CB, {0xB8, 0x38, 0x77, 0xAA, 0x68, 0x8F, 0xB8, 0x39}}

/* EFI Configuration Tables GUIDs */
#define EFI_CONFIG_TABLE_ACPI_TABLE_GUID                   {0xEB9D2D30, 0x2D88, 0x11D3, {0x9A, 0x16, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}}
#define EFI_CONFIG_TABLE_ACPI20_TABLE_GUID                 {0x8868E871, 0xE4F1, 0x11D3, {0xBC, 0x22, 0x00, 0x80, 0xC7, 0x3C, 0x88, 0x81}}
#define EFI_CONFIG_TABLE_DTB_TABLE_GUID                    {0xB1B621D5, 0xF19C, 0x41A5, {0x83, 0x0B, 0xD9, 0x15, 0x2C, 0x69, 0xAA, 0xE0}}
#define EFI_CONFIG_TABLE_MPS_TABLE_GUID                    {0xEB9D2D2F, 0x2D88, 0x11D3, {0x9A, 0x16, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}}
#define EFI_CONFIG_TABLE_SAL_SYSTEM_TABLE_GUID             {0xED9D2D32, 0x2D88, 0x11D3, {0x9A, 0x16, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}}
#define EFI_CONFIG_TABLE_SMBIOS_TABLE_GUID                 {0xEB9D2D31, 0x2D88, 0x11D3, {0x9A, 0x16, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}}
#define EFI_CONFIG_TABLE_SMBIOS3_TABLE_GUID                {0xF2FD1544, 0x9794, 0x4A2C, {0x99, 0x2E, 0xE5, 0xBB, 0xCf, 0x20, 0xE3, 0x94}}

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

/* Enumeration of PCI I/O protocol widths */
typedef enum _EFI_PCI_IO_PROTOCOL_WIDTH
{
    EfiPciIoWidthUint8,
    EfiPciIoWidthUint16,
    EfiPciIoWidthUint32,
    EfiPciIoWidthUint64,
    EfiPciIoWidthFifoUint8,
    EfiPciIoWidthFifoUint16,
    EfiPciIoWidthFifoUint32,
    EfiPciIoWidthFifoUint64,
    EfiPciIoWidthFillUint8,
    EfiPciIoWidthFillUint16,
    EfiPciIoWidthFillUint32,
    EfiPciIoWidthFillUint64,
    EfiPciIoWidthMaximum
} EFI_PCI_IO_PROTOCOL_WIDTH, *PEFI_PCI_IO_PROTOCOL_WIDTH;

/* Enumeration of EFI PCI I/O protocol operations */
typedef enum _EFI_PCI_IO_PROTOCOL_OPERATION
{
    EfiPciIoOperationBusMasterRead,
    EfiPciIoOperationBusMasterWrite,
    EfiPciIoOperationBusMasterCommonBuffer,
    EfiPciIoOperationMaximum
} EFI_PCI_IO_PROTOCOL_OPERATION, *PEFI_PCI_IO_PROTOCOL_OPERATION;

/* Enumeration of EFI PCI Root Bridge I/O protocol operations */
typedef enum _EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION
{
    EfiPciOperationBusMasterRead,
    EfiPciOperationBusMasterWrite,
    EfiPciOperationBusMasterCommonBuffer,
    EfiPciOperationBusMasterRead64,
    EfiPciOperationBusMasterWrite64,
    EfiPciOperationBusMasterCommonBuffer64,
    EfiPciOperationMaximum
} EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION, *PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION;

/* Enumeration of EFI PCI I/O protocol attribute operations */
typedef enum _EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION
{
    EfiPciIoAttributeOperationGet,
    EfiPciIoAttributeOperationSet,
    EfiPciIoAttributeOperationEnable,
    EfiPciIoAttributeOperationDisable,
    EfiPciIoAttributeOperationSupported,
    EfiPciIoAttributeOperationMaximum
} EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION, *PEFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION;

/* Enumeration of EFI I/O widths */
typedef enum _EFI_IO_WIDTH
{
    IO_UINT8,
    IO_UINT16,
    IO_UINT32,
    IO_UINT64,
    MMIO_COPY_UINT8,
    MMIO_COPY_UINT16,
    MMIO_COPY_UINT32,
    MMIO_COPY_UINT64
} EFI_IO_WIDTH, *PEFI_IO_WIDTH;

/* Enumeration of EFI I/O operation types */
typedef enum _EFI_IO_OPERATION_TYPE
{
    EfiBusMasterRead,
    EfiBusMasterWrite,
    EfiBusMasterCommonBuffer
} EFI_IO_OPERATION_TYPE, *PEFI_IO_OPERATION_TYPE;

/* Enumeration of EFI GOP output BLT operations */
typedef enum _EFI_GRAPHICS_OUTPUT_BLT_OPERATION
{
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION, *PEFI_GRAPHICS_OUTPUT_BLT_OPERATION;

/* Enumeration of EFI GOP pixel formats */
typedef enum _EFI_GRAPHICS_PIXEL_FORMAT
{
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT, *PEFI_GRAPHICS_PIXEL_FORMAT;

/* Enumeration of EGI graphics protocols */
typedef enum _EFI_GRAPHICS_PROTOCOL
{
    NONE,
    GOP,
    UGA
} EFI_GRAPHICS_PROTOCOL, *PEFI_GRAPHICS_PROTOCOL;

/* Enumeration of EFI UGA output BLT operations */
typedef enum _EFI_UNIVERSA_GRAPHICS_BLT_OPERATION
{
    EfiUgaVideoFill,
    EfiUgaVideoToBltBuffer,
    EfiUgaBltBufferToVideo,
    EfiUgaVideoToVideo,
    EfiUgaBltMax
} EFI_UNIVERSA_GRAPHICS_BLT_OPERATION, *PEFI_UNIVERSA_GRAPHICS_BLT_OPERATION;

/* Enumeration of UART parity types */
typedef enum _EFI_UART_PARITY_TYPE
{
    DefaultParity,
    NoParity,
    EvenParity,
    OddParity,
    MarkParity,
    SpaceParity
} EFI_UART_PARITY_TYPE, *PEFI_UART_PARITY_TYPE;

/* Enumeration of UART stop bit types */
typedef enum _EFI_UART_STOP_BITS_TYPE
{
    DefaultStopBits,
    OneStopBit,
    OneFiveStopBits,
    TwoStopBits
} EFI_UART_STOP_BITS_TYPE, *PEFI_UART_STOP_BITS_TYPE;

/* Enumeration of network state */
typedef enum _EFI_SIMPLE_NETWORK_STATE
{
    EfiSimpleNetworkStopped,
    EfiSimpleNetworkStarted,
    EfiSimpleNetworkInitialized,
    EfiSimpleNetworkMaxState
} EFI_SIMPLE_NETWORK_STATE, *PEFI_SIMPLE_NETWORK_STATE;

/* Enumeration of PXE TFTP opcodes */
typedef enum _EFI_PXE_BASE_CODE_TFTP_OPCODE
{
    EFI_PXE_BASE_CODE_TFTP_FIRST,
    EFI_PXE_BASE_CODE_TFTP_GET_FILE_SIZE,
    EFI_PXE_BASE_CODE_TFTP_READ_FILE,
    EFI_PXE_BASE_CODE_TFTP_WRITE_FILE,
    EFI_PXE_BASE_CODE_TFTP_READ_DIRECTORY,
    EFI_PXE_BASE_CODE_MTFTP_GET_FILE_SIZE,
    EFI_PXE_BASE_CODE_MTFTP_READ_FILE,
    EFI_PXE_BASE_CODE_MTFTP_READ_DIRECTORY,
    EFI_PXE_BASE_CODE_MTFTP_LAST
} EFI_PXE_BASE_CODE_TFTP_OPCODE, *PEFI_PXE_BASE_CODE_TFTP_OPCODE;

/* Enumeration of PXE functions */
typedef enum _EFI_PXE_BASE_CODE_FUNCTION
{
    EFI_PXE_BASE_CODE_FUNCTION_FIRST,
    EFI_PXE_BASE_CODE_FUNCTION_DHCP,
    EFI_PXE_BASE_CODE_FUNCTION_DISCOVER,
    EFI_PXE_BASE_CODE_FUNCTION_MTFTP,
    EFI_PXE_BASE_CODE_FUNCTION_UDP_WRITE,
    EFI_PXE_BASE_CODE_FUNCTION_UDP_READ,
    EFI_PXE_BASE_CODE_FUNCTION_ARP,
    EFI_PXE_BASE_CODE_FUNCTION_IGMP,
    EFI_PXE_BASE_CODE_PXE_FUNCTION_LAST
} EFI_PXE_BASE_CODE_FUNCTION, *PEFI_PXE_BASE_CODE_FUNCTION;

/* Enumeration of PXE callback statuses */
typedef enum _EFI_PXE_BASE_CODE_CALLBACK_STATUS
{
    EFI_PXE_BASE_CODE_CALLBACK_STATUS_FIRST,
    EFI_PXE_BASE_CODE_CALLBACK_STATUS_CONTINUE,
    EFI_PXE_BASE_CODE_CALLBACK_STATUS_ABORT,
    EFI_PXE_BASE_CODE_CALLBACK_STATUS_LAST
} EFI_PXE_BASE_CODE_CALLBACK_STATUS, *PEFI_PXE_BASE_CODE_CALLBACK_STATUS;

/* EFI routine callbacks */
typedef EFI_STATUS (*PEFI_CONVERT_POINTER)(IN UINT_PTR DebugDisposition, IN OUT PVOID *Address);
typedef EFI_STATUS (*PEFI_ALLOCATE_PAGES)(IN EFI_ALLOCATE_TYPE Type, IN EFI_MEMORY_TYPE MemoryType, IN UINT_PTR NoPages, OUT PEFI_PHYSICAL_ADDRESS Memory);
typedef EFI_STATUS (*PEFI_ALLOCATE_POOL)(IN EFI_MEMORY_TYPE PoolType, IN UINT_PTR Size, OUT PVOID *Buffer);
typedef EFI_STATUS (*PEFI_FREE_PAGES)(IN EFI_PHYSICAL_ADDRESS Memory, IN UINT_PTR NoPages);
typedef EFI_STATUS (*PEFI_FREE_POOL)(IN PVOID Buffer);
typedef VOID (*PEFI_EVENT_NOTIFY)(IN EFI_EVENT Event, IN PVOID Context);
typedef EFI_STATUS (*PEFI_CREATE_EVENT)(IN UINT32 Type, IN EFI_TPL NotifyTpl, IN PEFI_EVENT_NOTIFY NotifyFunction, IN PVOID NotifyContext, OUT PEFI_EVENT Event);
typedef EFI_STATUS (*PEFI_GET_MEMORY_MAP)(IN OUT PUINT_PTR MemoryMapSize, IN OUT PEFI_MEMORY_DESCRIPTOR MemoryMap, OUT PUINT_PTR MapKey, OUT PUINT_PTR DescriptorSize, OUT PUINT32 DescriptorVersion);
typedef EFI_STATUS (*PEFI_GET_VARIABLE)(IN PUINT16 VariableName, IN PEFI_GUID VendorGuid, OUT PUINT32 Attributes, IN OUT PUINT_PTR DataSize, OUT PVOID Data);
typedef EFI_STATUS (*PEFI_GET_NEXT_HIGH_MONO_COUNT)(OUT PUINT32 HighCount);
typedef EFI_STATUS (*PEFI_GET_NEXT_VARIABLE_NAME)(IN OUT PUINT_PTR VariableNameSize, IN OUT PUINT16 VariableName, IN OUT PEFI_GUID VendorGuid);
typedef EFI_STATUS (*PEFI_GET_TIME)(OUT PEFI_TIME Time, OUT PEFI_TIME_CAPABILITIES Capabilities);
typedef EFI_STATUS (*PEFI_SET_TIME)(IN PEFI_TIME Time);
typedef EFI_STATUS (*PEFI_SET_TIMER)(IN EFI_EVENT Event, IN EFI_TIMER_DELAY Type, IN UINT64 TriggerTime);
typedef EFI_STATUS (*PEFI_SIGNAL_EVENT)(IN EFI_EVENT Event);
typedef EFI_STATUS (*PEFI_CLOSE_EVENT)(IN EFI_EVENT Event);
typedef EFI_STATUS (*PEFI_CHECK_EVENT)(IN EFI_EVENT Event);
typedef EFI_STATUS (*PEFI_CREATE_EVENT_EX)(IN UINT32 Type, IN EFI_TPL NotifyTpl, IN PEFI_EVENT_NOTIFY NotifyFunction, IN CONST PVOID NotifyContext, IN CONST PEFI_GUID EventGroup, OUT PEFI_EVENT Event);
typedef EFI_STATUS (*PEFI_WAIT_FOR_EVENT)(IN UINT_PTR NumberOfEvents, IN PEFI_EVENT Event, OUT PUINT_PTR Index);
typedef EFI_STATUS (*PEFI_QUERY_CAPSULE_CAPABILITIES)(IN PEFI_CAPSULE_HEADER *CapsuleHeaderArray, IN UINT_PTR CapsuleCount, OUT PUINT64 MaximumCapsuleSize, OUT PEFI_RESET_TYPE ResetType);
typedef EFI_STATUS (*PEFI_QUERY_VARIABLE_INFO)(IN UINT32 Attributes, OUT PUINT64 MaximumVariableStorageSize, OUT PUINT64 RemainingVariableStorageSize, OUT PUINT64 MaximumVariableSize);
typedef EFI_STATUS (*PEFI_RAISE_TPL)(IN EFI_TPL NewTpl);
typedef EFI_STATUS (*PEFI_RESET_SYSTEM)(IN EFI_RESET_TYPE ResetType, IN EFI_STATUS ResetStatus, IN UINT_PTR DataSize, IN PUINT16 ResetData);
typedef EFI_STATUS (*PEFI_RESTORE_TPL)(IN EFI_TPL OldTpl);
typedef EFI_STATUS (*PEFI_UPDATE_CAPSULE)(IN PEFI_CAPSULE_HEADER *CapsuleHeaderArray, IN UINT_PTR CapsuleCount, IN EFI_PHYSICAL_ADDRESS ScatterGatherList);
typedef EFI_STATUS (*PEFI_SET_VARIABLE)(IN PUINT16 VariableName, IN PEFI_GUID VendorGuid, IN UINT32 Attributes, IN UINT_PTR DataSize, IN PVOID Data);
typedef EFI_STATUS (*PEFI_SET_VIRTUAL_ADDRESS_MAP)(IN UINT_PTR MemoryMapSize, IN UINT_PTR DescriptorSize, IN UINT32 DescriptorVersion, IN PEFI_MEMORY_DESCRIPTOR VirtualMap);
typedef EFI_STATUS (*PEFI_GET_WAKEUP_TIME)(OUT UCHAR Enabled, OUT UCHAR Pending, OUT PEFI_TIME Time);
typedef EFI_STATUS (*PEFI_SET_WAKEUP_TIME)(IN UCHAR Enable, IN PEFI_TIME Time);
typedef EFI_STATUS (*PEFI_INSTALL_PROTOCOL_INTERFACE)(IN OUT PEFI_HANDLE Handle, IN PEFI_GUID Protocol, IN EFI_INTERFACE_TYPE InterfaceType, IN PVOID Interface);
typedef EFI_STATUS (*PEFI_REINSTALL_PROTOCOL_INTERFACE)(IN EFI_HANDLE Handle, IN PEFI_GUID Protocol, IN PVOID OldInterface, IN PVOID NewInterface);
typedef EFI_STATUS (*PEFI_UNINSTALL_PROTOCOL_INTERFACE)(IN EFI_HANDLE Handle, IN PEFI_GUID Protocol, IN PVOID Interface);
typedef EFI_STATUS (*PEFI_HANDLE_PROTOCOL)(IN EFI_HANDLE Handle, IN PEFI_GUID Protocol, OUT PVOID *Interface);
typedef EFI_STATUS (*PEFI_REGISTER_PROTOCOL_NOTIFY)(IN PEFI_GUID Protocol, IN EFI_EVENT Event, OUT PVOID *Registration);
typedef EFI_STATUS (*PEFI_LOCATE_HANDLE)(IN EFI_LOCATE_SEARCH_TYPE SearchType, IN PEFI_GUID Protocol, IN PVOID SearchKey, IN OUT PUINT_PTR BufferSize, OUT PEFI_HANDLE Buffer);
typedef EFI_STATUS (*PEFI_LOCATE_DEVICE_PATH)(IN PEFI_GUID Protocol, IN OUT PEFI_DEVICE_PATH_PROTOCOL *DevicePath, OUT PEFI_HANDLE Device);
typedef EFI_STATUS (*PEFI_LOCATE_HANDLE_BUFFER)(IN EFI_LOCATE_SEARCH_TYPE SearchType, IN PEFI_GUID Protocol, IN PVOID SearchKey, IN OUT PUINT_PTR NoHandles, OUT PEFI_HANDLE *Buffer);
typedef EFI_STATUS (*PEFI_LOCATE_PROTOCOL)(IN PEFI_GUID Protocol, IN PVOID Registration, OUT PVOID *Interface);
typedef EFI_STATUS (*PEFI_INSTALL_CONFIGURATION_TABLE)(IN PEFI_GUID Guid, IN PVOID Table);
typedef EFI_STATUS (*PEFI_IMAGE_LOAD)(IN UCHAR BootPolicy, IN EFI_HANDLE ParentImageHandle, IN PEFI_DEVICE_PATH_PROTOCOL DevicePath, IN PVOID SourceBuffer, IN UINT_PTR SourceSize, OUT PEFI_HANDLE ImageHandle);
typedef EFI_STATUS (*PEFI_IMAGE_UNLOAD)(IN EFI_HANDLE ImageHandle);
typedef EFI_STATUS (*PEFI_IMAGE_START)(IN EFI_HANDLE ImageHandle, OUT PUINT_PTR ExitDataSize, OUT PUINT16 *ExitData);
typedef EFI_STATUS (*PEFI_EXIT)(IN EFI_HANDLE ImageHandle, IN EFI_STATUS ExitStatus, IN UINT_PTR ExitDataSize, IN PUINT16 ExitData);
typedef EFI_STATUS (*PEFI_EXIT_BOOT_SERVICES)(IN EFI_HANDLE ImageHandle, IN UINT_PTR MapKey);
typedef EFI_STATUS (*PEFI_GET_NEXT_MONOTONIC_COUNT)(OUT PUINT64 Count);
typedef EFI_STATUS (*PEFI_STALL)(IN UINT_PTR Microseconds);
typedef EFI_STATUS (*PEFI_SET_WATCHDOG_TIMER)(IN UINT_PTR Timeout, IN UINT64 WatchdogCode, IN UINT_PTR DataSize, IN PUINT16 WatchdogData);
typedef EFI_STATUS (*PEFI_CONNECT_CONTROLLER)(IN EFI_HANDLE ControllerHandle, IN PEFI_HANDLE DriverImageHandle, IN PEFI_DEVICE_PATH_PROTOCOL RemainingDevicePath, IN UCHAR Recursive);
typedef EFI_STATUS (*PEFI_DISCONNECT_CONTROLLER)(IN EFI_HANDLE ControllerHandle, IN EFI_HANDLE DriverImageHandle, IN EFI_HANDLE ChildHandle);
typedef EFI_STATUS (*PEFI_OPEN_PROTOCOL)(IN EFI_HANDLE Handle, IN PEFI_GUID Protocol, OUT PVOID *Interface, IN EFI_HANDLE AgentHandle, IN EFI_HANDLE ControllerHandle, IN UINT32 Attributes);
typedef EFI_STATUS (*PEFI_CLOSE_PROTOCOL)(IN EFI_HANDLE Handle, IN PEFI_GUID Protocol, IN EFI_HANDLE AgentHandle, IN EFI_HANDLE ControllerHandle);
typedef EFI_STATUS (*PEFI_OPEN_PROTOCOL_INFORMATION)(IN EFI_HANDLE Handle, IN PEFI_GUID Protocol, OUT PEFI_OPEN_PROTOCOL_INFORMATION_ENTRY *EntryBuffer, OUT PUINT_PTR EntryCount);
typedef EFI_STATUS (*PEFI_PROTOCOLS_PER_HANDLE)(IN EFI_HANDLE Handle, OUT PEFI_GUID **ProtocolBuffer, OUT PUINT_PTR ProtocolBufferCount);
typedef EFI_STATUS (*PEFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES)(IN OUT PEFI_HANDLE Handle, ...);
typedef EFI_STATUS (*PEFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES)(IN OUT PEFI_HANDLE Handle, ...);
typedef EFI_STATUS (*PEFI_CALCULATE_CRC32)(IN PVOID Data, IN UINT_PTR DataSize, OUT PUINT32 Crc32);
typedef EFI_STATUS (*PEFI_COPY_MEM)(IN OUT PVOID Destination, IN PVOID Source, IN UINT_PTR Length);
typedef EFI_STATUS (*PEFI_SET_MEM)(IN OUT PVOID Buffer, IN UINT_PTR Size, IN UINT8 Value);
typedef EFI_STATUS (*PEFI_INPUT_RESET)(IN PEFI_SIMPLE_TEXT_INPUT_PROTOCOL This, IN UCHAR ExtendedVerification);
typedef EFI_STATUS (*PEFI_INPUT_READ_KEY)(IN PEFI_SIMPLE_TEXT_INPUT_PROTOCOL This, OUT PEFI_INPUT_KEY Key);
typedef EFI_STATUS (*PEFI_TEXT_RESET)(IN PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, IN UCHAR ExtendedVerification);
typedef EFI_STATUS (*PEFI_TEXT_OUTPUT_STRING)(IN PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, IN PUINT16 String);
typedef EFI_STATUS (*PEFI_TEXT_TEST_STRING)(IN PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, IN PUINT16 String);
typedef EFI_STATUS (*PEFI_TEXT_QUERY_MODE)(IN PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, IN UINT_PTR ModeNumber, OUT PUINT_PTR Columns, OUT PUINT_PTR Rows);
typedef EFI_STATUS (*PEFI_TEXT_SET_MODE)(IN PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, IN UINT_PTR ModeNumber);
typedef EFI_STATUS (*PEFI_TEXT_SET_ATTRIBUTE)(IN PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, IN UINT_PTR Attribute);
typedef EFI_STATUS (*PEFI_TEXT_CLEAR_SCREEN)(IN PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This);
typedef EFI_STATUS (*PEFI_TEXT_SET_CURSOR_POSITION)(IN PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, IN UINT_PTR Column, IN UINT_PTR Row);
typedef EFI_STATUS (*PEFI_TEXT_ENABLE_CURSOR)(IN PEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL This, IN UCHAR Enable);
typedef EFI_STATUS (*PEFI_INPUT_RESET_EX)(IN PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL This, IN UCHAR ExtendedVerification);
typedef EFI_STATUS (*PEFI_INPUT_READ_KEY_EX)(IN PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL This, OUT PEFI_KEY_DATA KeyData);
typedef EFI_STATUS (*PEFI_SET_STATE)(IN PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL This, IN PUINT8 KeyToggleState);
typedef EFI_STATUS (*PEFI_KEY_NOTIFY_FUNCTION)(IN PEFI_KEY_DATA KeyData);
typedef EFI_STATUS (*PEFI_REGISTER_KEYSTROKE_NOTIFY)(IN PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL This, IN PEFI_KEY_DATA KeyData, IN PEFI_KEY_NOTIFY_FUNCTION KeyNotificationFunction, OUT PVOID *NotifyHandle);
typedef EFI_STATUS (*PEFI_UNREGISTER_KEYSTROKE_NOTIFY)(IN PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL This, IN PVOID NotificationHandle);
typedef PUINT16 (*PEFI_DEVICE_PATH_TO_TEXT_NODE)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DeviceNode, IN UCHAR DisplayOnly, IN UCHAR AllowShortcuts);
typedef PUINT16 (*PEFI_DEVICE_PATH_TO_TEXT_PATH)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath, IN UCHAR DisplayOnly, IN UCHAR AllowShortcuts);
typedef PEFI_DEVICE_PATH_PROTOCOL (*PEFI_DEVICE_PATH_FROM_TEXT_NODE)(IN CONST PUINT16 TextDeviceNode);
typedef PEFI_DEVICE_PATH_PROTOCOL (*PEFI_DEVICE_PATH_FROM_TEXT_PATH)(IN CONST PUINT16 TextDevicePath);
typedef UINT_PTR (*PEFI_DEVICE_PATH_UTILS_GET_DEVICE_PATH_SIZE)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath);
typedef PEFI_DEVICE_PATH_PROTOCOL (*PEFI_DEVICE_PATH_UTILS_DUP_DEVICE_PATH)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath);
typedef PEFI_DEVICE_PATH_PROTOCOL (*PEFI_DEVICE_PATH_UTILS_APPEND_PATH)(IN CONST PEFI_DEVICE_PATH_PROTOCOL Src1, IN CONST PEFI_DEVICE_PATH_PROTOCOL Src2);
typedef PEFI_DEVICE_PATH_PROTOCOL (*PEFI_DEVICE_PATH_UTILS_APPEND_NODE)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath, IN CONST PEFI_DEVICE_PATH_PROTOCOL DeviceNode);
typedef PEFI_DEVICE_PATH_PROTOCOL (*PEFI_DEVICE_PATH_UTILS_APPEND_INSTANCE)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath, IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePathInstance);
typedef PEFI_DEVICE_PATH_PROTOCOL (*PEFI_DEVICE_PATH_UTILS_GET_NEXT_INSTANCE)(IN OUT PEFI_DEVICE_PATH_PROTOCOL *DevicePathInstance, OUT PUINT_PTR DevicePathInstanceSize);
typedef PEFI_DEVICE_PATH_PROTOCOL (*PEFI_DEVICE_PATH_UTILS_CREATE_NODE)(IN UINT8 NodeType, IN UINT8 NodeSubType, IN UINT16 NodeLength);
typedef UCHAR (*PEFI_DEVICE_PATH_UTILS_IS_MULTI_INSTANCE)(IN CONST PEFI_DEVICE_PATH_PROTOCOL DevicePath);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_POLL_IO_MEM)(IN PEFI_PCI_IO_PROTOCOL This, IN EFI_PCI_IO_PROTOCOL_WIDTH Width, IN UINT8 BarIndex, IN UINT64 Offset, IN UINT64 Mask, IN UINT64 Value, IN UINT64 Delay, OUT PUINT64 Result);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_POLL_IO_MEM)(IN PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL This, IN EFI_PCI_IO_PROTOCOL_WIDTH Width, IN UINT64 Address, IN UINT64 Mask, IN UINT64 Value, IN UINT64 Delay, OUT PUINT64 Result);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_IO_MEM)(IN PEFI_PCI_IO_PROTOCOL This, IN EFI_PCI_IO_PROTOCOL_WIDTH Width, IN UINT8 BarIndex, IN UINT64 Offset, IN UINT_PTR Count, IN OUT PVOID Buffer);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_IO_MEM)(IN PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL This, IN EFI_PCI_IO_PROTOCOL_WIDTH Width, IN UINT64 Address, IN UINT_PTR Count, IN OUT PVOID Buffer);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_CONFIG)(IN PEFI_PCI_IO_PROTOCOL This, IN EFI_PCI_IO_PROTOCOL_WIDTH Width, IN UINT32 Offset, IN UINT_PTR Count, IN OUT PVOID Buffer);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_CONFIGURATION)(IN PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL This, OUT PVOID *Resources);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_COPY_MEM)(IN PEFI_PCI_IO_PROTOCOL This, IN EFI_PCI_IO_PROTOCOL_WIDTH Width, IN UINT8 DestBarIndex, IN UINT64 DestOffset, IN UINT8 SrcBarIndex, IN UINT64 SrcOffset, IN UINT_PTR Count);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_COPY_MEM)(IN PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL This, IN EFI_PCI_IO_PROTOCOL_WIDTH Width, IN UINT64 DestAddress, IN UINT64 SrcAddress, IN UINT_PTR Count);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_MAP)(IN PEFI_PCI_IO_PROTOCOL This, IN EFI_PCI_IO_PROTOCOL_OPERATION Operation, IN PVOID HostAddress, IN OUT PUINT_PTR NumberOfBytes, OUT PEFI_PHYSICAL_ADDRESS DeviceAddress, OUT PVOID *Mapping);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_MAP)(IN PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL This, IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_OPERATION Operation, IN PVOID HostAddress, IN OUT PUINT_PTR NumberOfBytes, OUT PEFI_PHYSICAL_ADDRESS DeviceAddress, OUT PVOID *Mapping);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_UNMAP)(IN PEFI_PCI_IO_PROTOCOL This, IN PVOID Mapping);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_UNMAP)(IN PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL This, IN PVOID Mapping);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_ALLOCATE_BUFFER)(IN PEFI_PCI_IO_PROTOCOL This, IN EFI_ALLOCATE_TYPE Type, IN EFI_MEMORY_TYPE MemoryType, IN UINT_PTR Pages, OUT PVOID *HostAddress, IN UINT64 Attributes);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ALLOCATE_BUFFER)(IN PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL This, IN EFI_ALLOCATE_TYPE Type, IN EFI_MEMORY_TYPE MemoryType, IN UINT_PTR Pages, IN OUT PVOID *HostAddress, IN UINT64 Attributes);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_FREE_BUFFER)(IN PEFI_PCI_IO_PROTOCOL This, IN UINT_PTR Pages, IN PVOID HostAddress);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_FREE_BUFFER)(IN PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL This, IN UINT_PTR Pages, IN PVOID HostAddress);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_FLUSH)(IN PEFI_PCI_IO_PROTOCOL This);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_FLUSH)(IN PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL This);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_GET_LOCATION)(IN PEFI_PCI_IO_PROTOCOL This, OUT PUINT_PTR SegmentNumber, OUT PUINT_PTR BusNumber, OUT PUINT_PTR DeviceNumber, OUT PUINT_PTR FunctionNumber);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_ATTRIBUTES)(IN PEFI_PCI_IO_PROTOCOL This, IN EFI_PCI_IO_PROTOCOL_ATTRIBUTE_OPERATION Operation, IN UINT64 Attributes, OUT PUINT64 Result);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_GET_BAR_ATTRIBUTES)(IN PEFI_PCI_IO_PROTOCOL This, IN UINT8 BarIndex, OUT PUINT64 Supports, OUT PVOID *Resources);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GET_ATTRIBUTES)(IN PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL This, OUT PUINT64 Supports, OUT PUINT64 Attributes);
typedef EFI_STATUS (*EFI_PCI_IO_PROTOCOL_SET_BAR_ATTRIBUTES)(IN PEFI_PCI_IO_PROTOCOL This, IN UINT64 Attributes, IN UINT8 BarIndex, IN OUT PUINT64 Offset, IN OUT PUINT64 Length);
typedef EFI_STATUS (*EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_SET_ATTRIBUTES)(IN PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL This, IN UINT64 Attributes, IN OUT PUINT64 ResourceBase, IN OUT PUINT64 ResourceLength);
typedef EFI_STATUS (*PEFI_BLOCK_RESET)(IN PEFI_BLOCK_IO_PROTOCOL This, IN UCHAR ExtendedVerification);
typedef EFI_STATUS (*PEFI_BLOCK_READ)(IN PEFI_BLOCK_IO_PROTOCOL This, IN UINT32 MediaId, IN EFI_LBA LBA, IN UINT_PTR BufferSize, OUT PVOID Buffer);
typedef EFI_STATUS (*PEFI_BLOCK_WRITE)(IN PEFI_BLOCK_IO_PROTOCOL This, IN UINT32 MediaId, IN EFI_LBA LBA, IN UINT_PTR BufferSize, IN PVOID Buffer);
typedef EFI_STATUS (*PEFI_BLOCK_FLUSH)(IN PEFI_BLOCK_IO_PROTOCOL This);
typedef EFI_STATUS (*PEFI_BLOCK_RESET_EX)(IN PEFI_BLOCK_IO2_PROTOCOL This, IN UCHAR ExtendedVerification);
typedef EFI_STATUS (*PEFI_BLOCK_READ_EX)(IN PEFI_BLOCK_IO2_PROTOCOL This, IN UINT32 MediaId, IN EFI_LBA LBA, IN OUT PEFI_BLOCK_IO2_TOKEN Token, IN UINT_PTR BufferSize, OUT PVOID Buffer);
typedef EFI_STATUS (*PEFI_BLOCK_WRITE_EX)(IN PEFI_BLOCK_IO2_PROTOCOL This, IN UINT32 MediaId, IN EFI_LBA LBA, IN OUT PEFI_BLOCK_IO2_TOKEN Token, IN UINT_PTR BufferSize, IN PVOID Buffer);
typedef EFI_STATUS (*PEFI_BLOCK_FLUSH_EX)(IN PEFI_BLOCK_IO2_PROTOCOL This, IN OUT PEFI_BLOCK_IO2_TOKEN Token);
typedef EFI_STATUS (*PEFI_DISK_READ)(IN PEFI_DISK_IO_PROTOCOL This, IN UINT32 MediaId, IN UINT64 Offset, IN UINT_PTR BufferSize, OUT PVOID Buffer);
typedef EFI_STATUS (*PEFI_DISK_WRITE)(IN PEFI_DISK_IO_PROTOCOL This, IN UINT32 MediaId, IN UINT64 Offset, IN UINT_PTR BufferSize, IN PVOID Buffer);
typedef EFI_STATUS (*PEFI_DISK_CANCEL_EX)(IN PEFI_DISK_IO2_PROTOCOL This);
typedef EFI_STATUS (*PEFI_DISK_READ_EX)(IN PEFI_DISK_IO2_PROTOCOL This, IN UINT32 MediaId, IN UINT64 Offset, IN OUT PEFI_DISK_IO2_TOKEN Token, IN UINT_PTR BufferSize, OUT PVOID Buffer);
typedef EFI_STATUS (*PEFI_DISK_WRITE_EX)(IN PEFI_DISK_IO2_PROTOCOL This, IN UINT32 MediaId, IN UINT64 Offset, IN OUT PEFI_DISK_IO2_TOKEN Token, IN UINT_PTR BufferSize, IN PVOID Buffer);
typedef EFI_STATUS (*PEFI_DISK_FLUSH_EX)(IN PEFI_DISK_IO2_PROTOCOL This, IN OUT PEFI_DISK_IO2_TOKEN Token);
typedef EFI_STATUS (*PEFI_VOLUME_OPEN)(IN PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL This, OUT PEFI_FILE_HANDLE *Root);
typedef EFI_STATUS (*PEFI_FILE_OPEN)(IN PEFI_FILE_HANDLE File, OUT PEFI_FILE_HANDLE *NewHandle, IN PUINT16 FileName, IN UINT64 OpenMode, IN UINT64 Attributes);
typedef EFI_STATUS (*PEFI_FILE_CLOSE)(IN PEFI_FILE_HANDLE File);
typedef EFI_STATUS (*PEFI_FILE_DELETE)(IN PEFI_FILE_HANDLE File);
typedef EFI_STATUS (*PEFI_FILE_READ)(IN PEFI_FILE_HANDLE File, IN OUT PUINT_PTR BufferSize, OUT PVOID Buffer);
typedef EFI_STATUS (*PEFI_FILE_WRITE)(IN PEFI_FILE_HANDLE File, IN OUT PUINT_PTR BufferSize, IN PVOID Buffer);
typedef EFI_STATUS (*PEFI_FILE_SET_POSITION)(IN PEFI_FILE_HANDLE File, IN UINT64 Position);
typedef EFI_STATUS (*PEFI_FILE_GET_POSITION)(IN PEFI_FILE_HANDLE File, OUT PUINT64 Position);
typedef EFI_STATUS (*PEFI_FILE_GET_INFO)(IN PEFI_FILE_HANDLE File, IN PEFI_GUID InformationType, IN OUT PUINT_PTR BufferSize, OUT PVOID Buffer);
typedef EFI_STATUS (*PEFI_FILE_SET_INFO)(IN PEFI_FILE_HANDLE File, IN PEFI_GUID InformationType, IN UINT_PTR BufferSize, IN PVOID Buffer);
typedef EFI_STATUS (*PEFI_FILE_FLUSH)(IN PEFI_FILE_HANDLE File);
typedef EFI_STATUS (*PEFI_FILE_OPEN_EX)(IN PEFI_FILE_HANDLE File, OUT PEFI_FILE_HANDLE *NewHandle, IN PUINT16 FileName, IN UINT64 OpenMode, IN UINT64 Attributes, IN OUT PEFI_FILE_IO_TOKEN Token);
typedef EFI_STATUS (*PEFI_FILE_READ_EX)(IN PEFI_FILE_HANDLE File, IN OUT PEFI_FILE_IO_TOKEN Token);
typedef EFI_STATUS (*PEFI_FILE_WRITE_EX)(IN PEFI_FILE_HANDLE File, IN OUT PEFI_FILE_IO_TOKEN Token);
typedef EFI_STATUS (*PEFI_FILE_FLUSH_EX)(IN PEFI_FILE_HANDLE File, IN OUT PEFI_FILE_IO_TOKEN Token);
typedef EFI_STATUS (*PEFI_LOAD_FILE)(IN PEFI_LOAD_FILE_PROTOCOL This, IN PEFI_DEVICE_PATH_PROTOCOL FilePath, IN UCHAR BootPolicy, IN OUT PUINT_PTR BufferSize, IN PVOID Buffer);
typedef EFI_STATUS (*PEFI_DEVICE_IO)(IN PEFI_DEVICE_IO_PROTOCOL This, IN EFI_IO_WIDTH Width, IN UINT64 Address, IN UINT_PTR Count, IN OUT PVOID Buffer);
typedef EFI_STATUS (*PEFI_PCIDEV_DEVICE_PATH)(IN PEFI_DEVICE_IO_PROTOCOL This, IN UINT64 Address, IN OUT PEFI_DEVICE_PATH_PROTOCOL *PciDevicePath);
typedef EFI_STATUS (*PEFI_IO_MAP)(IN PEFI_DEVICE_IO_PROTOCOL This, IN EFI_IO_OPERATION_TYPE Operation, IN PEFI_PHYSICAL_ADDRESS HostAddress, IN OUT PUINT_PTR NumberOfBytes, OUT PEFI_PHYSICAL_ADDRESS DeviceAddress, OUT PVOID *Mapping);
typedef EFI_STATUS (*PEFI_IO_UNMAP)(IN PEFI_DEVICE_IO_PROTOCOL This, IN PVOID Mapping);
typedef EFI_STATUS (*PEFI_IO_ALLOCATE_BUFFER)(IN PEFI_DEVICE_IO_PROTOCOL This, IN EFI_ALLOCATE_TYPE Type, IN EFI_MEMORY_TYPE MemoryType, IN UINT_PTR Pages, IN OUT PEFI_PHYSICAL_ADDRESS HostAddress);
typedef EFI_STATUS (*PEFI_IO_FLUSH)(IN PEFI_DEVICE_IO_PROTOCOL This);
typedef EFI_STATUS (*PEFI_IO_FREE_BUFFER)(IN PEFI_DEVICE_IO_PROTOCOL This, IN UINT_PTR Pages, IN EFI_PHYSICAL_ADDRESS HostAddress);
typedef INT_PTR (*PEFI_UNICODE_STRICOLL)(IN PEFI_UNICODE_COLLATION_PROTOCOL This, IN PUINT16 s1, IN PUINT16 s2);
typedef UCHAR (*PEFI_UNICODE_METAIMATCH)(IN PEFI_UNICODE_COLLATION_PROTOCOL This, IN PUINT16 String, IN PUINT16 Pattern);
typedef VOID (*PEFI_UNICODE_STRLWR)(IN PEFI_UNICODE_COLLATION_PROTOCOL This, IN OUT PUINT16 Str);
typedef VOID (*PEFI_UNICODE_STRUPR)(IN PEFI_UNICODE_COLLATION_PROTOCOL This, IN OUT PUINT16 Str);
typedef VOID (*PEFI_UNICODE_FATTOSTR)(IN PEFI_UNICODE_COLLATION_PROTOCOL This, IN UINT_PTR FatSize, IN PUINT8 Fat, OUT PUINT16 String);
typedef UCHAR (*PEFI_UNICODE_STRTOFAT)(IN PEFI_UNICODE_COLLATION_PROTOCOL This, IN PUINT16 String, IN UINT_PTR FatSize, OUT PUINT8 Fat);
typedef EFI_STATUS (*PEFI_HASH_GET_HASH_SIZE)(IN CONST PEFI_HASH_PROTOCOL This, IN CONST PEFI_GUID HashAlgorithm, OUT PUINT_PTR HashSize);
typedef EFI_STATUS (*PEFI_HASH_HASH)(IN CONST PEFI_HASH_PROTOCOL This, IN CONST PEFI_GUID HashAlgorithm, IN UCHAR Extend, IN CONST PUINT8 Message, IN UINT64 MessageSize, IN OUT PEFI_HASH_OUTPUT Hash);
typedef EFI_STATUS (*PEFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE)(IN PEFI_GRAPHICS_OUTPUT_PROTOCOL This, IN UINT32 ModeNumber, OUT PUINT_PTR SizeOfInfo, OUT PEFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info);
typedef EFI_STATUS (*PEFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE)(IN PEFI_GRAPHICS_OUTPUT_PROTOCOL This, IN UINT32 ModeNumber);
typedef EFI_STATUS (*PEFI_GRAPHICS_OUTPUT_PROTOCOL_BLT)(IN PEFI_GRAPHICS_OUTPUT_PROTOCOL This, IN PEFI_GRAPHICS_OUTPUT_BLT_PIXEL BltBuffer, IN EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation, IN UINT_PTR SourceX, IN UINT_PTR SourceY, IN UINT_PTR DestinationX, IN UINT_PTR DestinationY, IN UINT_PTR Width, IN UINT_PTR Height, IN UINT_PTR Delta);
typedef EFI_STATUS (*PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL_GET_MODE)(IN PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL This, OUT PUINT32 HorizontalResolution, OUT PUINT32 VerticalResolution, OUT PUINT32 ColorDepth, OUT PUINT32 RefreshRate);
typedef EFI_STATUS (*PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL_SET_MODE)(IN PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL This, IN UINT32 HorizontalResolution, IN UINT32 VerticalResolution, IN UINT32 ColorDepth, IN UINT32 RefreshRate);
typedef EFI_STATUS (*PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL_BLT)(IN PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL This, IN PEFI_UNIVERSAL_GRAPHICS_BLT_PIXEL BltBuffer, IN EFI_UNIVERSA_GRAPHICS_BLT_OPERATION BltOperation, IN UINT_PTR SourceX, IN UINT_PTR SourceY, IN UINT_PTR DestinationX, IN UINT_PTR DestinationY, IN UINT_PTR Width, IN UINT_PTR Height, IN UINT_PTR Delta);
typedef EFI_STATUS (*PEFI_EDID_OVERRIDE_PROTOCOL_GET_EDID)(IN PEFI_EDID_OVERRIDE_PROTOCOL This, IN PEFI_HANDLE ChildHandle, OUT PUINT32 Attributes, IN OUT PUINT_PTR EdidSize, IN OUT PUINT8 *Edid);
typedef EFI_STATUS (*PEFI_SERVICE_BINDING_CREATE_CHILD)(IN PEFI_SERVICE_BINDING This, IN PEFI_HANDLE ChildHandle);
typedef EFI_STATUS (*PEFI_SERVICE_BINDING_DESTROY_CHILD)(IN PEFI_SERVICE_BINDING This, IN EFI_HANDLE ChildHandle);
typedef EFI_STATUS (*PEFI_DRIVER_BINDING_PROTOCOL_SUPPORTED)(IN PEFI_DRIVER_BINDING_PROTOCOL This, IN EFI_HANDLE ControllerHandle, IN PEFI_DEVICE_PATH_PROTOCOL RemainingDevicePath);
typedef EFI_STATUS (*PEFI_DRIVER_BINDING_PROTOCOL_START)(IN PEFI_DRIVER_BINDING_PROTOCOL This, IN EFI_HANDLE ControllerHandle, IN PEFI_DEVICE_PATH_PROTOCOL RemainingDevicePath);
typedef EFI_STATUS (*PEFI_DRIVER_BINDING_PROTOCOL_STOP)(IN PEFI_DRIVER_BINDING_PROTOCOL This, IN EFI_HANDLE ControllerHandle, IN UINT_PTR NumberOfChildren, IN PEFI_HANDLE ChildHandleBuffer);
typedef EFI_STATUS (*PEFI_COMPONENT_NAME_GET_DRIVER_NAME)(IN PEFI_COMPONENT_NAME_PROTOCOL This, IN PUINT8 Language, OUT PUINT16 *DriverName);
typedef EFI_STATUS (*PEFI_COMPONENT_NAME_GET_CONTROLLER_NAME)(IN PEFI_COMPONENT_NAME_PROTOCOL This, IN EFI_HANDLE ControllerHandle, IN EFI_HANDLE ChildHandle, IN PUINT8 Language, OUT PUINT16 *ControllerName);
typedef EFI_STATUS (*PEFI_COMPONENT_NAME2_GET_DRIVER_NAME)(IN PEFI_COMPONENT_NAME2_PROTOCOL This, IN PUINT8 Language, OUT PUINT16 *DriverName);
typedef EFI_STATUS (*PEFI_COMPONENT_NAME2_GET_CONTROLLER_NAME)(IN PEFI_COMPONENT_NAME2_PROTOCOL This, IN EFI_HANDLE ControllerHandle, IN EFI_HANDLE ChildHandle, IN PUINT8 Language, OUT PUINT16 *ControllerName);
typedef EFI_STATUS (*PEFI_RNG_GET_INFO)(IN PEFI_RNG_PROTOCOL This, IN OUT PUINT_PTR RNGAlgorithmListSize, OUT PEFI_GUID RNGAlgorithmList);
typedef EFI_STATUS (*PEFI_RNG_GET_RNG)(IN PEFI_RNG_PROTOCOL This, IN PEFI_GUID RNGAlgorithm, IN UINT_PTR RNGValueLength, OUT PUINT8 RNGValue);
typedef EFI_STATUS (*PEFI_PLATFORM_DRIVER_OVERRIDE_GET_DRIVER)(IN PEFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL This, IN EFI_HANDLE ControllerHandle, IN OUT PEFI_HANDLE DriverImageHandle);
typedef EFI_STATUS (*PEFI_PLATFORM_DRIVER_OVERRIDE_GET_DRIVER_PATH)(IN PEFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL This, IN EFI_HANDLE ControllerHandle, IN OUT PEFI_DEVICE_PATH_PROTOCOL *DriverImagePath);
typedef EFI_STATUS (*PEFI_PLATFORM_DRIVER_OVERRIDE_DRIVER_LOADED)(IN PEFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL This, IN EFI_HANDLE ControllerHandle, IN PEFI_DEVICE_PATH_PROTOCOL DriverImagePath, IN EFI_HANDLE DriverImageHandle);
typedef EFI_STATUS (*PEFI_BUS_SPECIFIC_DRIVER_OVERRIDE_GET_DRIVER)(IN PEFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL This, IN OUT PEFI_HANDLE DriverImageHandle);
typedef UINT32 (*PEFI_DRIVER_FAMILY_OVERRIDE_GET_VERSION)(IN PEFI_DRIVER_FAMILY_OVERRIDE_PROTOCOL This);
typedef EFI_STATUS (*PEFI_EBC_CREATE_THUNK)(IN PEFI_EBC_PROTOCOL This, IN EFI_HANDLE ImageHandle, IN PVOID EbcEntryPoint, OUT PVOID *Thunk);
typedef EFI_STATUS (*PEFI_EBC_UNLOAD_IMAGE)(IN PEFI_EBC_PROTOCOL This, IN EFI_HANDLE ImageHandle);
typedef EFI_STATUS (*PEFI_EBC_ICACHE_FLUSH)(IN EFI_PHYSICAL_ADDRESS Start, IN UINT64 Length);
typedef EFI_STATUS (*PEFI_EBC_REGISTER_ICACHE_FLUSH)(IN PEFI_EBC_PROTOCOL This, IN PEFI_EBC_ICACHE_FLUSH Flush);
typedef EFI_STATUS (*PEFI_EBC_GET_VERSION)(IN PEFI_EBC_PROTOCOL This, IN OUT PUINT64 Version);
typedef EFI_STATUS (*PEFI_UART_RESET)(IN PEFI_UART_IO_PROTOCOL This);
typedef EFI_STATUS (*PEFI_UART_SET_ATTRIBUTES)(IN PEFI_UART_IO_PROTOCOL This, IN UINT64 BaudRate, IN UINT32 ReceiveFifoDepth, IN UINT32 Timeout, IN EFI_UART_PARITY_TYPE Parity, IN UINT8 DataBits, IN EFI_UART_STOP_BITS_TYPE StopBits);
typedef EFI_STATUS (*PEFI_UART_SET_CONTROL_BITS)(IN PEFI_UART_IO_PROTOCOL This, IN UINT32 Control);
typedef EFI_STATUS (*PEFI_UART_GET_CONTROL_BITS)(IN PEFI_UART_IO_PROTOCOL This, OUT PUINT32 Control);
typedef EFI_STATUS (*PEFI_UART_WRITE)(IN PEFI_UART_IO_PROTOCOL This, IN OUT PUINT_PTR BufferSize, IN PVOID Buffer);
typedef EFI_STATUS (*PEFI_UART_READ)(IN PEFI_UART_IO_PROTOCOL This, IN OUT PUINT_PTR BufferSize, OUT PVOID Buffer);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_START)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_STOP)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_INITIALIZE)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This, IN UINT_PTR ExtraRxBufferSize, IN UINT_PTR ExtraTxBufferSize);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_RESET)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This, IN UCHAR ExtendedVerification);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_SHUTDOWN)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_RECEIVE_FILTERS)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This, IN UINT32 Enable, IN UINT32 Disable, IN UCHAR ResetMCastFilter, IN UINT_PTR MCastFilterCnt, IN PEFI_MAC_ADDRESS MCastFilter);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_STATION_ADDRESS)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This, IN UCHAR Reset, IN PEFI_MAC_ADDRESS New);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_STATISTICS)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This, IN UCHAR Reset, IN OUT PUINT_PTR StatisticsSize, OUT PEFI_NETWORK_STATISTICS StatisticsTable);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_MCAST_IP_TO_MAC)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This, IN UCHAR IPv6, IN PEFI_IP_ADDRESS IP, OUT PEFI_MAC_ADDRESS MAC);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_NVDATA)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This, IN UCHAR ReadWrite, IN UINT_PTR Offset, IN UINT_PTR BufferSize, IN OUT PVOID Buffer);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_GET_STATUS)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This, OUT PUINT32 InterruptStatus, OUT PVOID *TxBuf);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_TRANSMIT)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This, IN UINT_PTR HeaderSize, IN UINT_PTR BufferSize, IN PVOID Buffer, IN PEFI_MAC_ADDRESS SrcAddr, IN PEFI_MAC_ADDRESS DestAddr, IN PUINT16 Protocol);
typedef EFI_STATUS (*PEFI_SIMPLE_NETWORK_RECEIVE)(IN PEFI_SIMPLE_NETWORK_PROTOCOL This, OUT PUINT_PTR HeaderSize, IN OUT PUINT_PTR BufferSize, OUT PVOID Buffer, OUT PEFI_MAC_ADDRESS SrcAddr, OUT PEFI_MAC_ADDRESS DestAddr, OUT PUINT16 Protocol);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_START)(IN PEFI_PXE_BASE_CODE_PROTOCOL This, IN UCHAR UseIpv6);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_STOP)(IN PEFI_PXE_BASE_CODE_PROTOCOL This);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_DHCP)(IN PEFI_PXE_BASE_CODE_PROTOCOL This, IN UCHAR SortOffers);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_DISCOVER)(IN PEFI_PXE_BASE_CODE_PROTOCOL This, IN UINT16 Type, IN PUINT16 Layer, IN UCHAR UseBis, IN OUT PEFI_PXE_BASE_CODE_DISCOVER_INFO Info);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_MTFTP)(IN PEFI_PXE_BASE_CODE_PROTOCOL This, IN EFI_PXE_BASE_CODE_TFTP_OPCODE Operation, IN OUT PVOID *BufferPtr, IN UCHAR Overwrite, IN OUT PUINT64 BufferSize, IN PUINT_PTR BlockSize, IN PEFI_IP_ADDRESS ServerIp, IN PUINT8 Filename, IN PEFI_PXE_BASE_CODE_MTFTP_INFO Info, IN UCHAR DontUseBuffer);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_UDP_WRITE)(IN PEFI_PXE_BASE_CODE_PROTOCOL This, IN UINT16 OpFlags, IN PEFI_IP_ADDRESS DestIp, IN PUINT16 DestPort, IN PEFI_IP_ADDRESS GatewayIp, IN PEFI_IP_ADDRESS SrcIp, IN OUT PUINT16 SrcPort, IN PUINT_PTR HeaderSize, IN PVOID HeaderPtr, IN PUINT_PTR BufferSize, IN PVOID BufferPtr);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_UDP_READ)(IN PEFI_PXE_BASE_CODE_PROTOCOL This, IN UINT16 OpFlags, IN OUT PEFI_IP_ADDRESS DestIp, IN OUT PUINT16 DestPort, IN OUT PEFI_IP_ADDRESS SrcIp, IN OUT PUINT16 SrcPort, IN PUINT_PTR HeaderSize, IN PVOID HeaderPtr, IN OUT PUINT_PTR BufferSize, IN PVOID BufferPtr);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_SET_IP_FILTER)(IN PEFI_PXE_BASE_CODE_PROTOCOL This, IN PEFI_PXE_BASE_CODE_IP_FILTER NewFilter);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_ARP)(IN PEFI_PXE_BASE_CODE_PROTOCOL This, IN PEFI_IP_ADDRESS IpAddr, IN PEFI_MAC_ADDRESS MacAddr);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_SET_PARAMETERS)(IN PEFI_PXE_BASE_CODE_PROTOCOL This, IN PUCHAR NewAutoArp, IN PUCHAR NewSendGUID, IN PUINT8 NewTTL, IN PUINT8 NewToS, IN PUCHAR NewMakeCallback);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_SET_STATION_IP)(IN PEFI_PXE_BASE_CODE_PROTOCOL This, IN PEFI_IP_ADDRESS NewStationIp, IN PEFI_IP_ADDRESS NewSubnetMask);
typedef EFI_STATUS (*PEFI_PXE_BASE_CODE_SET_PACKETS)(IN PEFI_PXE_BASE_CODE_PROTOCOL This, PUCHAR NewDhcpDiscoverValid, PUCHAR NewDhcpAckReceived, PUCHAR NewProxyOfferReceived, PUCHAR NewPxeDiscoverValid, PUCHAR NewPxeReplyReceived, PUCHAR NewPxeBisReplyReceived, IN PEFI_PXE_BASE_CODE_PACKET NewDhcpDiscover, IN PEFI_PXE_BASE_CODE_PACKET NewDhcpAck, IN PEFI_PXE_BASE_CODE_PACKET NewProxyOffer, IN PEFI_PXE_BASE_CODE_PACKET NewPxeDiscover, IN PEFI_PXE_BASE_CODE_PACKET NewPxeReply, IN PEFI_PXE_BASE_CODE_PACKET NewPxeBisReply);
typedef EFI_PXE_BASE_CODE_CALLBACK_STATUS (*PEFI_PXE_CALLBACK)(IN PEFI_PXE_BASE_CODE_CALLBACK_PROTOCOL This, IN EFI_PXE_BASE_CODE_FUNCTION Function, IN UCHAR Received, IN UINT32 PacketLen, IN PEFI_PXE_BASE_CODE_PACKET Packet);
typedef VOID (*PEFI_AP_PROCEDURE)(IN OUT PVOID Buffer);
typedef EFI_STATUS (*PEFI_MP_SERVICES_ENABLEDISABLEAP)(IN PEFI_MP_SERVICES_PROTOCOL This, IN UINT_PTR ProcessorNumber, IN BOOLEAN EnableAP, IN PUINT32 HealthFlag);
typedef EFI_STATUS (*PEFI_MP_SERVICES_GET_NUMBER_OF_PROCESSORS)(IN PEFI_MP_SERVICES_PROTOCOL This, OUT PUINT_PTR NumberOfProcessors, OUT PUINT_PTR NumberOfEnabledProcessors);
typedef EFI_STATUS (*PEFI_MP_SERVICES_GET_PROCESSOR_INFO)(IN PEFI_MP_SERVICES_PROTOCOL This, IN UINT_PTR ProcessorNumber, OUT PEFI_PROCESSOR_INFORMATION ProcessorInfoBuffer);
typedef EFI_STATUS (*PEFI_MP_SERVICES_STARTUP_ALL_APS)(IN PEFI_MP_SERVICES_PROTOCOL This, IN PEFI_AP_PROCEDURE Procedure, IN BOOLEAN SingleThread, IN EFI_EVENT WaitEvent, IN UINT_PTR TimeoutInMicroSeconds, IN PVOID ProcedureArgument, OUT PUINT_PTR *FailedCpuList);
typedef EFI_STATUS (*PEFI_MP_SERVICES_STARTUP_THIS_AP)(IN PEFI_MP_SERVICES_PROTOCOL This, IN PEFI_AP_PROCEDURE Procedure, IN UINT_PTR ProcessorNumber, IN EFI_EVENT WaitEvent, IN UINT_PTR TimeoutInMicroseconds, IN PVOID ProcedureArgument, OUT PBOOLEAN Finished);
typedef EFI_STATUS (*PEFI_MP_SERVICES_SWITCH_BSP)(IN PEFI_MP_SERVICES_PROTOCOL This, IN UINT_PTR ProcessorNumber, IN BOOLEAN EnableOldBSP);
typedef EFI_STATUS (*PEFI_MP_SERVICES_WHOAMI)(IN PEFI_MP_SERVICES_PROTOCOL This, OUT PUINT_PTR ProcessorNumber);

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

/* EFI Capsule Block Descriptor */
typedef struct _EFI_CAPSULE_BLOCK_DESCRIPTOR
{
    UINT64 Length;
    union
    {
        EFI_PHYSICAL_ADDRESS DataBlock;
        EFI_PHYSICAL_ADDRESS ContinuationPointer;
    } Union;
} EFI_CAPSULE_BLOCK_DESCRIPTOR, *PEFI_CAPSULE_BLOCK_DESCRIPTOR;

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

/* Definition of an EFI memory map */
typedef struct _EFI_MEMORY_MAP
{
  PEFI_MEMORY_DESCRIPTOR Map;
  UINT_PTR MapSize;
  UINT_PTR MapKey;
  UINT_PTR DescriptorSize;
  UINT32 DescriptorVersion;
} EFI_MEMORY_MAP, *PEFI_MEMORY_MAP;

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
    PEFI_TEXT_OUTPUT_STRING OutputString;
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

/* EFI Key State information */
typedef struct _EFI_KEY_STATE
{
    UINT32 KeyShiftState;
    UINT8 KeyToggleState;
} EFI_KEY_STATE, *PEFI_KEY_STATE;

/* EFI Key Data information */
typedef struct _EFI_KEY_DATA
{
    EFI_INPUT_KEY Key;
    EFI_KEY_STATE KeyState;
} EFI_KEY_DATA, *PEFI_KEY_DATA;

/* EFI Simple Text Input Ex protocol */
typedef struct _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL
{
    PEFI_INPUT_RESET_EX Reset;
    PEFI_INPUT_READ_KEY_EX ReadKeyStrokeEx;
    EFI_EVENT WaitForKeyEx;
    PEFI_SET_STATE SetState;
    PEFI_REGISTER_KEYSTROKE_NOTIFY RegisterKeyNotify;
    PEFI_UNREGISTER_KEYSTROKE_NOTIFY UnregisterKeyNotify;
} EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL, *PEFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL;

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

/* EFI network interface identifier protocol */
typedef struct _EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL
{
    UINT64 Revision;
    UINT64 ID;
    UINT64 ImageAddr;
    UINT32 ImageSize;
    UINT8 StringId[4];
    UINT8 Type;
    UINT8 MajorVer;
    UINT8 MinorVer;
    UCHAR Ipv6Supported;
    UINT8 IfNum;
} EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL, *PEFI_NETWORK_INTERFACE_IDENTIFIER_INTERFACE;

/* EFI network configuration data structure */
typedef struct _EFI_MANAGED_NETWORK_CONFIG_DATA
{
    UINT32 ReceivedQueueTimeoutValue;
    UINT32 TransmitQueueTimeoutValue;
    UINT16 ProtocolTypeFilter;
    UCHAR EnableUnicastReceive;
    UCHAR EnableMulticastReceive;
    UCHAR EnableBroadcastReceive;
    UCHAR EnablePromiscuousReceive;
    UCHAR FlushQueuesOnReset;
    UCHAR EnableReceiveTimestamps;
    UCHAR DisableBackgroundPolling;
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
    UCHAR StaticIpAddress;
    EFI_IPv4_ADDRESS GatewayIpAddress;
    EFI_IPv4_ADDRESS SubnetMask;
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
    UCHAR IPAddressOrigin;
    UINT8 PrefixLength;
    EFI_IPv6_ADDRESS GatewayIpAddress;
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
typedef struct _EFI_MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH
{
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

/* PCI I/O protocol access registers */
typedef struct _EFI_PCI_IO_PROTOCOL_ACCESS
{
    EFI_PCI_IO_PROTOCOL_IO_MEM Read;
    EFI_PCI_IO_PROTOCOL_IO_MEM Write;
} EFI_PCI_IO_PROTOCOL_ACCESS, *PEFI_PCI_IO_PROTOCOL_ACCESS;

/* PCI Root Bridge I/O protocol access registers */
typedef struct _EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS
{
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_IO_MEM Read;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_IO_MEM Write;
} EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS, *PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS;

/* PCI I/O protocol configuration access registers */
typedef struct _EFI_PCI_IO_PROTOCOL_CONFIG_ACCESS
{
    EFI_PCI_IO_PROTOCOL_CONFIG Read;
    EFI_PCI_IO_PROTOCOL_CONFIG Write;
} EFI_PCI_IO_PROTOCOL_CONFIG_ACCESS, *PEFI_PCI_IO_PROTOCOL_CONFIG_ACCESS;

/* PCI Root Bridge I/O protocol address */
typedef struct _EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS
{
    UINT8 Register;
    UINT8 Function;
    UINT8 Device;
    UINT8 Bus;
    UINT32 ExtendedRegister;
} EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS, *PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS;

/* EFI PCI I/O protocol */
typedef struct _EFI_PCI_IO_PROTOCOL
{
    EFI_PCI_IO_PROTOCOL_POLL_IO_MEM PollMem;
    EFI_PCI_IO_PROTOCOL_POLL_IO_MEM PollIo;
    EFI_PCI_IO_PROTOCOL_ACCESS Mem;
    EFI_PCI_IO_PROTOCOL_ACCESS Io;
    EFI_PCI_IO_PROTOCOL_CONFIG_ACCESS Pci;
    EFI_PCI_IO_PROTOCOL_COPY_MEM CopyMem;
    EFI_PCI_IO_PROTOCOL_MAP Map;
    EFI_PCI_IO_PROTOCOL_UNMAP Unmap;
    EFI_PCI_IO_PROTOCOL_ALLOCATE_BUFFER AllocateBuffer;
    EFI_PCI_IO_PROTOCOL_FREE_BUFFER FreeBuffer;
    EFI_PCI_IO_PROTOCOL_FLUSH Flush;
    EFI_PCI_IO_PROTOCOL_GET_LOCATION GetLocation;
    EFI_PCI_IO_PROTOCOL_ATTRIBUTES Attributes;
    EFI_PCI_IO_PROTOCOL_GET_BAR_ATTRIBUTES GetBarAttributes;
    EFI_PCI_IO_PROTOCOL_SET_BAR_ATTRIBUTES SetBarAttributes;
    UINT64 RomSize;
    PVOID RomImage;
} EFI_PCI_IO_PROTOCOL, *PEFI_PCI_IO_PROTOCOL;

/* EFI PCI Root Bridge I/O protocol */
typedef struct _EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL
{
    EFI_HANDLE ParentHandle;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_POLL_IO_MEM PollMem;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_POLL_IO_MEM PollIo;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS Mem;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS Io;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ACCESS Pci;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_COPY_MEM CopyMem;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_MAP Map;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_UNMAP Unmap;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_ALLOCATE_BUFFER AllocateBuffer;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_FREE_BUFFER FreeBuffer;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_FLUSH Flush;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GET_ATTRIBUTES GetAttributes;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_SET_ATTRIBUTES SetAttributes;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_CONFIGURATION Configuration;
    UINT32 SegmentNumber;
} EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL, *PEFI_PCI_ROOT_BRIDGE_IO_PROTOCOL;

/* Describes block device */
typedef struct _EFI_BLOCK_DEVICE
{
    LIST_ENTRY ListEntry;
    EFI_GUID Guid;
    USHORT DriveType;
    ULONG DriveNumber;
    ULONG PartitionNumber;
    PEFI_GUID PartitionGuid;
    PEFI_DEVICE_PATH_PROTOCOL DevicePath;
} EFI_BLOCK_DEVICE, *PEFI_BLOCK_DEVICE;

/* Describes block device I/O and DP protocols */
typedef struct _EFI_BLOCK_DEVICE_DATA
{
    LIST_ENTRY ListEntry;
    PEFI_DEVICE_PATH_PROTOCOL DevicePath;
    PEFI_BLOCK_IO_PROTOCOL BlockIo;
} EFI_BLOCK_DEVICE_DATA, *PEFI_BLOCK_DEVICE_DATA;

/* Block I/O media structure */
typedef struct _EFI_BLOCK_IO_MEDIA
{
    UINT32 MediaId;
    UCHAR RemovableMedia;
    UCHAR MediaPresent;
    UCHAR LogicalPartition;
    UCHAR ReadOnly;
    UCHAR WriteCaching;
    UINT32 BlockSize;
    UINT32 IoAlign;
    EFI_LBA LastBlock;
    EFI_LBA LowestAlignedLba;
    UINT32 LogicalBlocksPerPhysicalBlock;
    UINT32 OptimalTransferLengthGranularity;
} EFI_BLOCK_IO_MEDIA, *PEFI_BLOCK_IO_MEDIA;

/* EFI Block I/O protocol */
typedef struct _EFI_BLOCK_IO_PROTOCOL
{
    UINT64 Revision;
    PEFI_BLOCK_IO_MEDIA Media;
    PEFI_BLOCK_RESET Reset;
    PEFI_BLOCK_READ ReadBlocks;
    PEFI_BLOCK_WRITE WriteBlocks;
    PEFI_BLOCK_FLUSH FlushBlocks;
} EFI_BLOCK_IO_PROTOCOL, *PEFI_BLOCK_IO_PROTOCOL;

/* EFI Block IO2 protocol */
typedef struct _EFI_BLOCK_IO2_PROTOCOL
{
    PEFI_BLOCK_IO_MEDIA Media;
    PEFI_BLOCK_RESET_EX Reset;
    PEFI_BLOCK_READ_EX ReadBlocksEx;
    PEFI_BLOCK_WRITE_EX WriteBlocksEx;
    PEFI_BLOCK_FLUSH_EX FlushBlocksEx;
} EFI_BLOCK_IO2_PROTOCOL, *PEFI_BLOCK_IO2_PROTOCOL;

/* EFI Block IO2 token */
typedef struct _EFI_BLOCK_IO2_TOKEN
{
    EFI_EVENT Event;
    EFI_STATUS TransactionStatus;
} EFI_BLOCK_IO2_TOKEN, *PEFI_BLOCK_IO2_TOKEN;

/* EFI Disk IO protocol */
typedef struct _EFI_DISK_IO_PROTOCOL
{
    UINT64 Revision;
    PEFI_DISK_READ ReadDisk;
    PEFI_DISK_WRITE WriteDisk;
} EFI_DISK_IO_PROTOCOL, *PEFI_DISK_IO_PROTOCOL;

/* EFI Disk IO2 token */
typedef struct _EFI_DISK_IO2_TOKEN
{
    EFI_EVENT Event;
    EFI_STATUS TransactionStatus;
} EFI_DISK_IO2_TOKEN, *PEFI_DISK_IO2_TOKEN;

/* EFI Disk IO2 protocol */
typedef struct _EFI_DISK_IO2_PROTOCOL
{
    UINT64 Revision;
    PEFI_DISK_CANCEL_EX Cancel;
    PEFI_DISK_READ_EX ReadDiskEx;
    PEFI_DISK_WRITE_EX WriteDiskEx;
    PEFI_DISK_FLUSH_EX FlushDiskEx;
} EFI_DISK_IO2_PROTOCOL, *PEFI_DISK_IO2_PROTOCOL;

/* EFI Simple File System (SFS) protocol */
typedef struct _EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
{
    UINT64 Revision;
    PEFI_VOLUME_OPEN OpenVolume;
} EFI_SIMPLE_FILE_SYSTEM_PROTOCOL, *PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

/* EFI File I/O token */
typedef struct _EFI_FILE_IO_TOKEN
{
    EFI_EVENT Event;
    EFI_STATUS Status;
    UINT_PTR BufferSize;
    PVOID Buffer;
} EFI_FILE_IO_TOKEN, *PEFI_FILE_IO_TOKEN;

/* EFI File Handle */
typedef struct _EFI_FILE_HANDLE
{
    UINT64 Revision;
    PEFI_FILE_OPEN Open;
    PEFI_FILE_CLOSE Close;
    PEFI_FILE_DELETE Delete;
    PEFI_FILE_READ Read;
    PEFI_FILE_WRITE Write;
    PEFI_FILE_GET_POSITION GetPosition;
    PEFI_FILE_SET_POSITION SetPosition;
    PEFI_FILE_GET_INFO GetInfo;
    PEFI_FILE_SET_INFO SetInfo;
    PEFI_FILE_FLUSH Flush;
    PEFI_FILE_OPEN_EX OpenEx;
    PEFI_FILE_READ_EX ReadEx;
    PEFI_FILE_WRITE_EX WriteEx;
    PEFI_FILE_FLUSH_EX FlushEx;
} EFI_FILE_HANDLE, *PEFI_FILE_HANDLE;

/* EFI File Info structure */
typedef struct _EFI_FILE_INFO
{
    UINT64 Size;
    UINT64 FileSize;
    UINT64 PhysicalSize;
    EFI_TIME CreateTime;
    EFI_TIME LastAccessTime;
    EFI_TIME ModificationTime;
    UINT64 Attribute;
    UINT16 FileName[1];
} EFI_FILE_INFO, *PEFI_FILE_INFO;

/* EFI File System Info structure */
typedef struct _EFI_FILE_SYSTEM_INFO
{
    UINT64 Size;
    UCHAR ReadOnly;
    UINT64 VolumeSize;
    UINT64 FreeSpace;
    UINT32 BlockSize;
    UINT16 VolumeLabel[1];
} EFI_FILE_SYSTEM_INFO, *PEFI_FILE_SYSTEM_INFO;

/* EFI File System Volume Label */
typedef struct _EFI_FILE_SYSTEM_VOLUME_LABEL
{
    UINT16 VolumeLabel[1];
} EFI_FILE_SYSTEM_VOLUME_LABEL, *PEFI_FILE_SYSTEM_VOLUME_LABEL;

/* Load file protocol */
typedef struct _EFI_LOAD_FILE_PROTOCOL
{
    PEFI_LOAD_FILE LoadFile;
} EFI_LOAD_FILE_PROTOCOL, *PEFI_LOAD_FILE_PROTOCOL;

/* EFI I/O access structure */
typedef struct _EFI_IO_ACCESS
{
    PEFI_DEVICE_IO Read;
    PEFI_DEVICE_IO Write;
} EFI_IO_ACCESS, *PEFI_IO_ACCESS;

/* EFI Device I/O protocol */
typedef struct _EFI_DEVICE_IO_PROTOCOL
{
    EFI_IO_ACCESS Mem;
    EFI_IO_ACCESS Io;
    EFI_IO_ACCESS Pci;
    PEFI_IO_MAP Map;
    PEFI_PCIDEV_DEVICE_PATH PciDevicePath;
    PEFI_IO_UNMAP Unmap;
    PEFI_IO_ALLOCATE_BUFFER AllocateBuffer;
    PEFI_IO_FLUSH Flush;
    PEFI_IO_FREE_BUFFER FreeBuffer;
} EFI_DEVICE_IO_PROTOCOL, *PEFI_DEVICE_IO_PROTOCOL;

/* EFI Hash Output union */
typedef union _EFI_HASH_OUTPUT
{
    PUINT8 Md5Hash;
    PUINT8 Sha1Hash;
    PUINT8 Sha224Hash;
    PUINT8 Sha256Hash;
    PUINT8 Sha384Hash;
    PUINT8 Sha512Hash;
} EFI_HASH_OUTPUT, *PEFI_HASH_OUTPUT;

/* EFI Hash protocol */
typedef struct _EFI_HASH_PROTOCOL
{
    PEFI_HASH_GET_HASH_SIZE GetHashSize;
    PEFI_HASH_HASH Hash;
} EFI_HASH_PROTOCOL, *PEFI_HASH_PROTOCOL;

/* EFI Unicode Collation protocol */
typedef struct _EFI_UNICODE_COLLATION_PROTOCOL
{
    PEFI_UNICODE_STRICOLL StriColl;
    PEFI_UNICODE_METAIMATCH MetaiMatch;
    PEFI_UNICODE_STRLWR StrLwr;
    PEFI_UNICODE_STRUPR StrUpr;
    PEFI_UNICODE_FATTOSTR FatToStr;
    PEFI_UNICODE_STRTOFAT StrToFat;
    PUINT8 SupportedLanguages;
} EFI_UNICODE_COLLATION_PROTOCOL, *PEFI_UNICODE_COLLATION_PROTOCOL;

/* EFI Pixel bitmask */
typedef struct _EFI_PIXEL_BITMASK
{
    UINT32 RedMask;
    UINT32 GreenMask;
    UINT32 BlueMask;
    UINT32 ReservedMask;
} EFI_PIXEL_BITMASK, *PEFI_PIXEL_BITMASK;

/* EFI GOP output mode information */
typedef struct _EFI_GRAPHICS_OUTPUT_MODE_INFORMATION
{
    UINT32 Version;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
    EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    EFI_PIXEL_BITMASK PixelInformation;
    UINT32 PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION, *PEFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

/* EFI GOP output blt pixel */
typedef struct _EFI_GRAPHICS_OUTPUT_BLT_PIXEL
{
    UINT8 Blue;
    UINT8 Green;
    UINT8 Red;
    UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL, *PEFI_GRAPHICS_OUTPUT_BLT_PIXEL;

/* EFI GOP output blt pixel */
typedef union _EFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Pixel;
    UINT32 Raw;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION, *PEFI_GRAPHICS_OUTPUT_BLT_PIXEL_UNION;

/* EFI GOP output protocol mode */
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE
{
    UINT32 MaxMode;
    UINT32 Mode;
    PEFI_GRAPHICS_OUTPUT_MODE_INFORMATION Info;
    UINT_PTR SizeOfInfo;
    EFI_PHYSICAL_ADDRESS FrameBufferBase;
    UINT_PTR FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE, *PEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

/* EFI GOP output protocol */
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL
{
    PEFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE QueryMode;
    PEFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE SetMode;
    PEFI_GRAPHICS_OUTPUT_PROTOCOL_BLT Blt;
    PEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE Mode;
} EFI_GRAPHICS_OUTPUT_PROTOCOL, *PEFI_GRAPHICS_OUTPUT_PROTOCOL;

/* EFI UGA adapter protocol */
typedef struct _EFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL
{
    PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL_GET_MODE GetMode;
    PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL_SET_MODE SetMode;
    PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL_BLT Blt;
} EFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL, *PEFI_UNIVERSAL_GRAPHICS_ADAPTER_PROTOCOL;

/* EFI UGA PIXEL */
typedef struct _EFI_UNIVERSAL_GRAPHICS_BLT_PIXEL
{
    UINT8 Blue;
    UINT8 Green;
    UINT8 Red;
    UINT8 Reserved;
} EFI_UNIVERSAL_GRAPHICS_BLT_PIXEL, *PEFI_UNIVERSAL_GRAPHICS_BLT_PIXEL;

/* EFI EDID discover protocol */
typedef struct _EFI_EDID_DISCOVERED_PROTOCOL
{
    UINT32 SizeOfEdid;
    PUINT8 Edid;
} EFI_EDID_DISCOVERED_PROTOCOL, *PEFI_EDID_DISCOVERED_PROTOCOL;

/* EFI EDID active protocol */
typedef struct _EFI_EDID_ACTIVE_PROTOCOL
{
    UINT32 SizeOfEdid;
    PUINT8 Edid;
} EFI_EDID_ACTIVE_PROTOCOL, *PEFI_EDID_ACTIVE_PROTOCOL;

/* EFI EDID override protocol */
typedef struct _EFI_EDID_OVERRIDE_PROTOCOL
{
    PEFI_EDID_OVERRIDE_PROTOCOL_GET_EDID GetEdid;
} EFI_EDID_OVERRIDE_PROTOCOL, *PEFI_EDID_OVERRIDE_PROTOCOL;

/* EFI Service binding */
typedef struct _EFI_SERVICE_BINDING
{
    PEFI_SERVICE_BINDING_CREATE_CHILD CreateChild;
    PEFI_SERVICE_BINDING_DESTROY_CHILD DestroyChild;
} EFI_SERVICE_BINDING, *PEFI_SERVICE_BINDING;

/* EFI driver binding protocol */
typedef struct _EFI_DRIVER_BINDING_PROTOCOL
{
    PEFI_DRIVER_BINDING_PROTOCOL_SUPPORTED Supported;
    PEFI_DRIVER_BINDING_PROTOCOL_START Start;
    PEFI_DRIVER_BINDING_PROTOCOL_STOP Stop;
    UINT32 Version;
    EFI_HANDLE ImageHandle;
    EFI_HANDLE DriverBindingHandle;
} EFI_DRIVER_BINDING_PROTOCOL, *PEFI_DRIVER_BINDING_PROTOCOL;

/* EFI component name protocol */
typedef struct _EFI_COMPONENT_NAME_PROTOCOL
{
    PEFI_COMPONENT_NAME_GET_DRIVER_NAME GetDriverName;
    PEFI_COMPONENT_NAME_GET_CONTROLLER_NAME GetControllerName;
    PUINT8 SupportedLanguages;
} EFI_COMPONENT_NAME_PROTOCOL, *PEFI_COMPONENT_NAME_PROTOCOL;

/* EFI component name2 protocol */
typedef struct _EFI_COMPONENT_NAME2_PROTOCOL
{
    PEFI_COMPONENT_NAME2_GET_DRIVER_NAME GetDriverName;
    PEFI_COMPONENT_NAME2_GET_CONTROLLER_NAME GetControllerName;
    PUINT8 SupportedLanguages;
} EFI_COMPONENT_NAME2_PROTOCOL, *PEFI_COMPONENT_NAME2_PROTOCOL;

/* EFI loaded image protocol */
typedef struct _EFI_LOADED_IMAGE_PROTOCOL
{
    UINT32 Revision;
    EFI_HANDLE ParentHandle;
    PEFI_SYSTEM_TABLE SystemTable;
    EFI_HANDLE DeviceHandle;
    PEFI_DEVICE_PATH_PROTOCOL FilePath;
    PVOID Reserved;
    UINT32 LoadOptionsSize;
    PVOID LoadOptions;
    PVOID ImageBase;
    UINT64 ImageSize;
    EFI_MEMORY_TYPE ImageCodeType;
    EFI_MEMORY_TYPE ImageDataType;
    PEFI_IMAGE_UNLOAD Unload;
} EFI_LOADED_IMAGE_PROTOCOL, *PEFI_LOADED_IMAGE_PROTOCOL;

/* EFI RNG protocol */
typedef struct _EFI_RNG_PROTOCOL
{
    PEFI_RNG_GET_INFO GetInfo;
    PEFI_RNG_GET_RNG GetRNG;
} EFI_RNG_PROTOCOL, *PEFI_RNG_PROTOCOL;

/* EFI platform driver override protocol */
typedef struct _EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL
{
    PEFI_PLATFORM_DRIVER_OVERRIDE_GET_DRIVER GetDriver;
    PEFI_PLATFORM_DRIVER_OVERRIDE_GET_DRIVER_PATH GetDriverPath;
    PEFI_PLATFORM_DRIVER_OVERRIDE_DRIVER_LOADED DriverLoaded;
} EFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL, *PEFI_PLATFORM_DRIVER_OVERRIDE_PROTOCOL;

/* EFI bus specific driver override protocol */
typedef struct _EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL
{
    PEFI_BUS_SPECIFIC_DRIVER_OVERRIDE_GET_DRIVER GetDriver;
} EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL, *PEFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL;

/* EFI driver family override protocol */
typedef struct _EFI_DRIVER_FAMILY_OVERRIDE_PROTOCOL
{
    PEFI_DRIVER_FAMILY_OVERRIDE_GET_VERSION GetVersion;
} EFI_DRIVER_FAMILY_OVERRIDE_PROTOCOL, *PEFI_DRIVER_FAMILY_OVERRIDE_PROTOCOL;

/* EFI EBC protocol */
typedef struct _EFI_EBC_PROTOCOL
{
    PEFI_EBC_CREATE_THUNK CreateThunk;
    PEFI_EBC_UNLOAD_IMAGE UnloadImage;
    PEFI_EBC_REGISTER_ICACHE_FLUSH RegisterICacheFlush;
    PEFI_EBC_GET_VERSION GetVersion;
} EFI_EBC_PROTOCOL, *PEFI_EBC_PROTOCOL;

/* EFI partition header */
typedef struct _EFI_PARTITION_HEADER
{
    EFI_TABLE_HEADER Hdr;
    UINT32 DirectoryAllocationNumber;
    UINT32 BlockSize;
    EFI_LBA FirstUsableLba;
    EFI_LBA LastUsableLba;
    EFI_LBA UnusableSpace;
    EFI_LBA FreeSpace;
    EFI_LBA RootFile;
    EFI_LBA SecutiryFile;
} EFI_PARTITION_HEADER, *PEFI_PARTITION_HEADER;

/* MBR partition information */
typedef struct _EFI_MBR_PARTITION_RECORD
{
    UINT8 BootIndicator;
    UINT8 StartHead;
    UINT8 StartSector;
    UINT8 StartTrack;
    UINT8 OSIndicator;
    UINT8 EndHead;
    UINT8 EndSector;
    UINT8 EndTrack;
    UINT8 StartingLBA[4];
    UINT8 SizeInLBA[4];
} EFI_MBR_PARTITION_RECORD, *PEFI_MBR_PARTITION_RECORD;

/* Master Boot Record (MBR) information */
typedef struct _EFI_MASTER_BOOT_RECORD
{
    UINT8 BootStrapCode[440];
    UINT8 UniqueMbrSignature[4];
    UINT8 Unknown[2];
    EFI_MBR_PARTITION_RECORD Partition[4];
    UINT16 Signature;
} EFI_MASTER_BOOT_RECORD, *PEFI_MASTER_BOOT_RECORD;

/* GUID Partition Table (GPT) header */
typedef struct _EFI_GPT_PARTITION_TABLE_HEADER
{
    EFI_TABLE_HEADER Header;
    EFI_LBA MyLBA;
    EFI_LBA AlternateLBA;
    EFI_LBA FirstUsableLBA;
    EFI_LBA LastUsableLBA;
    EFI_GUID DiskGUID;
    EFI_LBA PartitionEntryLBA;
    UINT32 NumberOfPartitionEntries;
    UINT32 SizeOfPartitionEntry;
    UINT32 PartitionEntryArrayCRC32;
} EFI_GPT_PARTITION_TABLE_HEADER, *PEFI_GPT_PARTITION_TABLE_HEADER;

/* GUID Partition Table (GPT) partition entry */
typedef struct _EFI_GPT_PARTITION_ENTRY
{
    EFI_GUID PartitionTypeGUID;
    EFI_GUID UniquePartitionGUID;
    EFI_LBA StartingLBA;
    EFI_LBA EndingLBA;
    UINT64 Attributes;
    UINT16 PartitionName[36];
} EFI_GPT_PARTITION_ENTRY, *PEFI_GPT_PARTITION_ENTRY;

/* EFI file header */
typedef struct _EFI_FILE_HEADER
{
    EFI_TABLE_HEADER Hdr;
    UINT32 Class;
    UINT32 LBALOffset;
    EFI_LBA Parent;
    UINT64 FileSize;
    UINT64 FileAttributes;
    EFI_TIME FileCreateTime;
    EFI_TIME FileModificationTime;
    EFI_GUID VendorGuid;
    UINT16 FileString[260];
} EFI_FILE_HEADER, *PEFI_FILE_HEADER;

/* Logical Block Address List (LBA List) */
typedef struct _EFI_LBAL
{
    EFI_TABLE_HEADER Hdr;
    UINT32 Class;
    EFI_LBA Parent;
    EFI_LBA Next;
    UINT32 ArraySize;
    UINT32 ArrayCount;
} EFI_LBAL, *PEFI_LBAL;

/* Logical Block run-length */
typedef struct _EFI_RL
{
    EFI_LBA     Start;
    UINT64      Length;
} EFI_RL, *PEFI_RL;

/* EFI UART I/O mode */
typedef struct _EFI_UART_IO_MODE
{
    UINT32 ControlMask;
    UINT32 Timeout;
    UINT64 BaudRate;
    UINT32 ReceiveFifoDepth;
    UINT32 DataBits;
    UINT32 Parity;
    UINT32 StopBits;
} EFI_UART_IO_MODE, *PEFI_UART_IO_MODE;

/* EFI UART I/O protocol */
typedef struct _EFI_UART_IO_PROTOCOL {
    UINT32 Revision;
    PEFI_UART_RESET Reset;
    PEFI_UART_SET_ATTRIBUTES SetAttributes;
    PEFI_UART_SET_CONTROL_BITS SetControl;
    PEFI_UART_GET_CONTROL_BITS GetControl;
    PEFI_UART_WRITE Write;
    PEFI_UART_READ Read;
    PEFI_UART_IO_MODE Mode;
} EFI_UART_IO_PROTOCOL, *PEFI_UART_IO_PROTOCOL;

/* EFI IP address definition */
typedef union _EFI_IP_ADDRESS
{
    UINT32 Addr[4];
    EFI_IPv4_ADDRESS v4;
    EFI_IPv6_ADDRESS v6;
} EFI_IP_ADDRESS, *PEFI_IP_ADDRESS;

/* EFI DHCPv4 packet definition */
typedef struct _EFI_PXE_BASE_CODE_DHCPV4_PACKET
{
    UINT8 BootpOpcode;
    UINT8 BootpHwType;
    UINT8 BootpHwAddrLen;
    UINT8 BootpGateHops;
    UINT32 BootpIdent;
    UINT16 BootpSeconds;
    UINT16 BootpFlags;
    UINT8 BootpCiAddr[4];
    UINT8 BootpYiAddr[4];
    UINT8 BootpSiAddr[4];
    UINT8 BootpGiAddr[4];
    UINT8 BootpHwAddr[16];
    UINT8 BootpSrvName[64];
    UINT8 BootpBootFile[128];
    UINT32 DhcpMagik;
    UINT8 DhcpOptions[56];
} EFI_PXE_BASE_CODE_DHCPV4_PACKET, *PEFI_PXE_BASE_CODE_DHCPV4_PACKET;

/* EFI DHCPv6 packet definition */
typedef struct _EFI_PXE_BASE_CODE_DHCPV6_PACKET
{
    UINT32 MessageType:8;
    UINT32 TransactionId:24;
    UINT8 DhcpOptions[1024];
} EFI_PXE_BASE_CODE_DHCPV6_PACKET, *PEFI_PXE_BASE_CODE_DHCPV6_PACKET;

/* EFI packet definition */
typedef union _EFI_PXE_BASE_CODE_PACKET
{
    UINT8 Raw[1472];
    EFI_PXE_BASE_CODE_DHCPV4_PACKET Dhcpv4;
    EFI_PXE_BASE_CODE_DHCPV6_PACKET Dhcpv6;
} EFI_PXE_BASE_CODE_PACKET, *PEFI_PXE_BASE_CODE_PACKET;

/* EFI ICMP error definition */
typedef struct _EFI_PXE_BASE_CODE_ICMP_ERROR
{
    UINT8 Type;
    UINT8 Code;
    UINT16 Checksum;
    union
    {
        UINT32 reserved;
        UINT32 Mtu;
        UINT32 Pointer;
        struct
        {
            UINT16 Identifier;
            UINT16 Sequence;
        } Echo;
    } u;
    UINT8 Data[494];
} EFI_PXE_BASE_CODE_ICMP_ERROR, *PEFI_PXE_BASE_CODE_ICMP_ERROR;

/* EFI TFTP error definition */
typedef struct _EFI_PXE_BASE_CODE_TFTP_ERROR
{
    UINT8 ErrorCode;
    UINT8 ErrorString[127];
} EFI_PXE_BASE_CODE_TFTP_ERROR, *PEFI_PXE_BASE_CODE_TFTP_ERROR;

/* EFI IP filter */
typedef struct _EFI_PXE_BASE_CODE_IP_FILTER
{
    UINT8 Filters;
    UINT8 IpCnt;
    UINT16 reserved;
    EFI_IP_ADDRESS IpList[8];
} EFI_PXE_BASE_CODE_IP_FILTER, *PEFI_PXE_BASE_CODE_IP_FILTER;

/* EFI ARP cache entry */
typedef struct _EFI_PXE_BASE_CODE_ARP_ENTRY
{
    EFI_IP_ADDRESS IpAddr;
    EFI_MAC_ADDRESS MacAddr;
} EFI_PXE_BASE_CODE_ARP_ENTRY, *PEFI_PXE_BASE_CODE_ARP_ENTRY;

/* EFI route cache entry */
typedef struct _EFI_PXE_BASE_CODE_ROUTE_ENTRY
{
    EFI_IP_ADDRESS IpAddr;
    EFI_IP_ADDRESS SubnetMask;
    EFI_IP_ADDRESS GwAddr;
} EFI_PXE_BASE_CODE_ROUTE_ENTRY, *PEFI_PXE_BASE_CODE_ROUTE_ENTRY;

/* EFI service list */
typedef struct _EFI_PXE_BASE_CODE_SRVLIST
{
    UINT16 Type;
    UCHAR AcceptAnyResponse;
    UINT8 Reserved;
    EFI_IP_ADDRESS IpAddr;
} EFI_PXE_BASE_CODE_SRVLIST, *PEFI_PXE_BASE_CODE_SRVLIST;

/* EFI network discovery info */
typedef struct _EFI_PXE_BASE_CODE_DISCOVER_INFO
{
    UCHAR UseMCast;
    UCHAR UseBCast;
    UCHAR UseUCast;
    UCHAR MustUseList;
    EFI_IP_ADDRESS ServerMCastIp;
    UINT16 IpCnt;
    EFI_PXE_BASE_CODE_SRVLIST SrvList[1];
} EFI_PXE_BASE_CODE_DISCOVER_INFO, *PEFI_PXE_BASE_CODE_DISCOVER_INFO;

/* EFI TFTP info */
typedef struct _EFI_PXE_BASE_CODE_MTFTP_INFO
{
    EFI_IP_ADDRESS MCastIp;
    UINT16 CPort;
    UINT16 SPort;
    UINT16 ListenTimeout;
    UINT16 TransmitTimeout;
} EFI_PXE_BASE_CODE_MTFTP_INFO, *PEFI_PXE_BASE_CODE_MTFTP_INFO;

/* EFI PXE base code mode structure */
typedef struct _EFI_PXE_BASE_CODE_MODE
{
    UCHAR Started;
    UCHAR Ipv6Available;
    UCHAR Ipv6Supported;
    UCHAR UsingIpv6;
    UCHAR BisSupported;
    UCHAR BisDetected;
    UCHAR AutoArp;
    UCHAR SendGUID;
    UCHAR DhcpDiscoverValid;
    UCHAR DhcpAckReceived;
    UCHAR ProxyOfferReceived;
    UCHAR PxeDiscoverValid;
    UCHAR PxeReplyReceived;
    UCHAR PxeBisReplyReceived;
    UCHAR IcmpErrorReceived;
    UCHAR TftpErrorReceived;
    UCHAR MakeCallbacks;
    UINT8 TTL;
    UINT8 ToS;
    EFI_IP_ADDRESS StationIp;
    EFI_IP_ADDRESS SubnetMask;
    EFI_PXE_BASE_CODE_PACKET DhcpDiscover;
    EFI_PXE_BASE_CODE_PACKET DhcpAck;
    EFI_PXE_BASE_CODE_PACKET ProxyOffer;
    EFI_PXE_BASE_CODE_PACKET PxeDiscover;
    EFI_PXE_BASE_CODE_PACKET PxeReply;
    EFI_PXE_BASE_CODE_PACKET PxeBisReply;
    EFI_PXE_BASE_CODE_IP_FILTER IpFilter;
    UINT32 ArpCacheEntries;
    EFI_PXE_BASE_CODE_ARP_ENTRY ArpCache[8];
    UINT32 RouteTableEntries;
    EFI_PXE_BASE_CODE_ROUTE_ENTRY RouteTable[8];
    EFI_PXE_BASE_CODE_ICMP_ERROR IcmpError;
    EFI_PXE_BASE_CODE_TFTP_ERROR TftpError;
} EFI_PXE_BASE_CODE_MODE, *PEFI_PXE_BASE_CODE_MODE;

/* EFI PXE base code protocol */
typedef struct _EFI_PXE_BASE_CODE_PROTOCOL {
    UINT64 Revision;
    PEFI_PXE_BASE_CODE_START Start;
    PEFI_PXE_BASE_CODE_STOP Stop;
    PEFI_PXE_BASE_CODE_DHCP Dhcp;
    PEFI_PXE_BASE_CODE_DISCOVER Discover;
    PEFI_PXE_BASE_CODE_MTFTP Mtftp;
    PEFI_PXE_BASE_CODE_UDP_WRITE UdpWrite;
    PEFI_PXE_BASE_CODE_UDP_READ UdpRead;
    PEFI_PXE_BASE_CODE_SET_IP_FILTER SetIpFilter;
    PEFI_PXE_BASE_CODE_ARP Arp;
    PEFI_PXE_BASE_CODE_SET_PARAMETERS SetParameters;
    PEFI_PXE_BASE_CODE_SET_STATION_IP SetStationIp;
    PEFI_PXE_BASE_CODE_SET_PACKETS SetPackets;
    PEFI_PXE_BASE_CODE_MODE Mode;
} EFI_PXE_BASE_CODE_PROTOCOL, *PEFI_PXE_BASE_CODE_PROTOCOL;

/* EFI PXE base code callback protocol */
typedef struct _EFI_PXE_BASE_CODE_CALLBACK_PROTOCOL {
    UINT64 Revision;
    PEFI_PXE_CALLBACK Callback;
} EFI_PXE_BASE_CODE_CALLBACK_PROTOCOL, *PEFI_PXE_BASE_CODE_CALLBACK_PROTOCOL;

/* EFI network statistics structure */
typedef struct _EFI_NETWORK_STATISTICS
{
    UINT64 RxTotalFrames;
    UINT64 RxGoodFrames;
    UINT64 RxUndersizeFrames;
    UINT64 RxOversizeFrames;
    UINT64 RxDroppedFrames;
    UINT64 RxUnicastFrames;
    UINT64 RxBroadcastFrames;
    UINT64 RxMulticastFrames;
    UINT64 RxCrcErrorFrames;
    UINT64 RxTotalBytes;
    UINT64 TxTotalFrames;
    UINT64 TxGoodFrames;
    UINT64 TxUndersizeFrames;
    UINT64 TxOversizeFrames;
    UINT64 TxDroppedFrames;
    UINT64 TxUnicastFrames;
    UINT64 TxBroadcastFrames;
    UINT64 TxMulticastFrames;
    UINT64 TxCrcErrorFrames;
    UINT64 TxTotalBytes;
    UINT64 Collisions;
    UINT64 UnsupportedProtocol;
} EFI_NETWORK_STATISTICS, *PEFI_NETWORK_STATISTICS;

/* EFI network mode structure */
typedef struct _EFI_SIMPLE_NETWORK_MODE
{
    UINT32 State;
    UINT32 HwAddressSize;
    UINT32 MediaHeaderSize;
    UINT32 MaxPacketSize;
    UINT32 NvRamSize;
    UINT32 NvRamAccessSize;
    UINT32 ReceiveFilterMask;
    UINT32 ReceiveFilterSetting;
    UINT32 MaxMCastFilterCount;
    UINT32 MCastFilterCount;
    EFI_MAC_ADDRESS MCastFilter[16];
    EFI_MAC_ADDRESS CurrentAddress;
    EFI_MAC_ADDRESS BroadcastAddress;
    EFI_MAC_ADDRESS PermanentAddress;
    UINT8 IfType;
    UCHAR MacAddressChangeable;
    UCHAR MultipleTxSupported;
    UCHAR MediaPresentSupported;
    UCHAR MediaPresent;
} EFI_SIMPLE_NETWORK_MODE, *PEFI_SIMPLE_NETWORK_MODE;

/* EFI network protocol structure */
typedef struct _EFI_SIMPLE_NETWORK_PROTOCOL
{
    UINT64 Revision;
    PEFI_SIMPLE_NETWORK_START Start;
    PEFI_SIMPLE_NETWORK_STOP Stop;
    PEFI_SIMPLE_NETWORK_INITIALIZE Initialize;
    PEFI_SIMPLE_NETWORK_RESET Reset;
    PEFI_SIMPLE_NETWORK_SHUTDOWN Shutdown;
    PEFI_SIMPLE_NETWORK_RECEIVE_FILTERS ReceiveFilters;
    PEFI_SIMPLE_NETWORK_STATION_ADDRESS StationAddress;
    PEFI_SIMPLE_NETWORK_STATISTICS Statistics;
    PEFI_SIMPLE_NETWORK_MCAST_IP_TO_MAC MCastIpToMac;
    PEFI_SIMPLE_NETWORK_NVDATA NvData;
    PEFI_SIMPLE_NETWORK_GET_STATUS GetStatus;
    PEFI_SIMPLE_NETWORK_TRANSMIT Transmit;
    PEFI_SIMPLE_NETWORK_RECEIVE Receive;
    EFI_EVENT WaitForPacket;
    PEFI_SIMPLE_NETWORK_MODE Mode;
} EFI_SIMPLE_NETWORK_PROTOCOL, *PEFI_SIMPLE_NETWORK_PROTOCOL;

/* EFI MP (Multiprocessor) services protocol */
typedef struct _EFI_MP_SERVICES_PROTOCOL
{
    PEFI_MP_SERVICES_GET_NUMBER_OF_PROCESSORS GetNumberOfProcessors;
    PEFI_MP_SERVICES_GET_PROCESSOR_INFO GetProcessorInfo;
    PEFI_MP_SERVICES_STARTUP_ALL_APS StartupAllAPs;
    PEFI_MP_SERVICES_STARTUP_THIS_AP StartupThisAP;
    PEFI_MP_SERVICES_SWITCH_BSP SwitchBSP;
    PEFI_MP_SERVICES_ENABLEDISABLEAP EnableDisableAP;
    PEFI_MP_SERVICES_WHOAMI WhoAmI;
} EFI_MP_SERVICES_PROTOCOL, *PEFI_MP_SERVICES_PROTOCOL;

/* EFI processor physical location structure */
typedef struct _EFI_PROCESSOR_PHYSICAL_LOCATION
{
    UINT32 Package;
    UINT32 Core;
    UINT32 Thread;
} EFI_PROCESSOR_PHYSICAL_LOCATION, *PEFI_PROCESSOR_PHYSICAL_LOCATION;

/* EFI processor information structure */
typedef struct _EFI_PROCESSOR_INFORMATION
{
    UINT64 ProcessorId;
    UINT32 StatusFlag;
    EFI_PROCESSOR_PHYSICAL_LOCATION Location;
} EFI_PROCESSOR_INFORMATION, *PEFI_PROCESSOR_INFORMATION;

#endif /* __XTDK_XTUEFI_H */
