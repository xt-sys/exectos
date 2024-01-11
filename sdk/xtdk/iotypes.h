/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/iotypes.h
 * DESCRIPTION:     I/O related type definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_IOTYPES_H
#define __XTDK_IOTYPES_H

#include <xttypes.h>


/* Number of PCI base address registers */
#define PCI_TYPE0_ADDRESSES                     6
#define PCI_TYPE1_ADDRESSES                     2
#define PCI_TYPE2_ADDRESSES                     5

/* PCI maximum number of devices */
#define PCI_MAX_BRIDGE_NUMBER                   255
#define PCI_MAX_DEVICES                         32
#define PCI_MAX_FUNCTION                        8

/* Invalid PCI vendor ID */
#define PCI_INVALID_VENDORID                    0xFFFF

/* PCI common config header types */
#define PCI_DEVICE_TYPE                         0x00
#define PCI_BRIDGE_TYPE                         0x01
#define PCI_CARDBUS_BRIDGE_TYPE                 0x02
#define PCI_MULTIFUNCTION                       0x80

/* PCI common config commands */
#define PCI_ENABLE_IO_SPACE                     0x0001
#define PCI_ENABLE_MEMORY_SPACE                 0x0002
#define PCI_ENABLE_BUS_MASTER                   0x0004
#define PCI_ENABLE_SPECIAL_CYCLES               0x0008
#define PCI_ENABLE_WRITE_AND_INVALIDATE         0x0010
#define PCI_ENABLE_VGA_COMPATIBLE_PALETTE       0x0020
#define PCI_ENABLE_PARITY                       0x0040
#define PCI_ENABLE_WAIT_CYCLE                   0x0080
#define PCI_ENABLE_SERR                         0x0100
#define PCI_ENABLE_FAST_BACK_TO_BACK            0x0200
#define PCI_DISABLE_LEVEL_INTERRUPT             0x0400

/* PCI common config statuses */
#define PCI_STATUS_INTERRUPT_PENDING            0x0008
#define PCI_STATUS_CAPABILITIES_LIST            0x0010
#define PCI_STATUS_66MHZ_CAPABLE                0x0020
#define PCI_STATUS_UDF_SUPPORTED                0x0040
#define PCI_STATUS_FAST_BACK_TO_BACK            0x0080
#define PCI_STATUS_DATA_PARITY_DETECTED         0x0100
#define PCI_STATUS_DEVSEL                       0x0600
#define PCI_STATUS_SIGNALED_TARGET_ABORT        0x0800
#define PCI_STATUS_RECEIVED_TARGET_ABORT        0x1000
#define PCI_STATUS_RECEIVED_MASTER_ABORT        0x2000
#define PCI_STATUS_SIGNALED_SYSTEM_ERROR        0x4000
#define PCI_STATUS_DETECTED_PARITY_ERROR        0x8000

/* PCI bridge control registers */
typedef struct _PCI_BRIDGE_CONTROL_REGISTER
{
    UINT Bar[2];
    UCHAR PrimaryBus;
    UCHAR SecondaryBus;
    UCHAR SubordinateBus;
    UCHAR SecondaryLatencyTimer;
    UCHAR IoBase;
    UCHAR IoLimit;
    USHORT SecondaryStatus;
    USHORT MemoryBase;
    USHORT MemoryLimit;
    USHORT PrefetchableMemoryBase;
    USHORT PrefetchableMemoryLimit;
    UINT PrefetchableBaseUpper32;
    UINT PrefetchableLimitUpper32;
    USHORT IoBaseUpper16;
    USHORT IoLimitUpper16;
    UINT Reserved;
    UINT ExpansionRomBAR;
    UCHAR InterruptLine;
    UCHAR InterruptPin;
    USHORT BridgeControl;
} PCI_BRIDGE_CONTROL_REGISTER, *PPCI_BRIDGE_CONTROL_REGISTER;

/* PCI and PCI-E common header structure */
typedef struct _PCI_COMMON_HEADER
{
    USHORT VendorId;
    USHORT DeviceId;
    USHORT Command;
    USHORT Status;
    UCHAR RevisionId;
    UCHAR ProgIf;
    UCHAR SubClass;
    UCHAR BaseClass;
    UCHAR CacheLineSize;
    UCHAR LatencyTimer;
    UCHAR HeaderType;
    UCHAR BIST;
    union
    {
        struct _PCI_TYPE0_HEADER
        {
            ULONG BaseAddresses[PCI_TYPE0_ADDRESSES];
            ULONG CIS;
            USHORT SubVendorId;
            USHORT SubSystemId;
            ULONG ROMBaseAddress;
            UCHAR CapabilitiesPtr;
            UCHAR Reserved1[3];
            ULONG Reserved2;
            UCHAR InterruptLine;
            UCHAR InterruptPin;
            UCHAR MinimumGrant;
            UCHAR MaximumLatency;
        } type0;
        struct _PCI_TYPE1_HEADER
        {
            ULONG BaseAddresses[PCI_TYPE1_ADDRESSES];
            UCHAR PrimaryBus;
            UCHAR SecondaryBus;
            UCHAR SubordinateBus;
            UCHAR SecondaryLatency;
            UCHAR IOBase;
            UCHAR IOLimit;
            USHORT SecondaryStatus;
            USHORT MemoryBase;
            USHORT MemoryLimit;
            USHORT PrefetchBase;
            USHORT PrefetchLimit;
            ULONG PrefetchBaseUpper32;
            ULONG PrefetchLimitUpper32;
            USHORT IOBaseUpper16;
            USHORT IOLimitUpper16;
            UCHAR CapabilitiesPtr;
            UCHAR Reserved1[3];
            ULONG ROMBaseAddress;
            UCHAR InterruptLine;
            UCHAR InterruptPin;
            USHORT BridgeControl;
        } type1;
        struct _PCI_TYPE2_HEADER
        {
            ULONG SocketRegistersBaseAddress;
            UCHAR CapabilitiesPtr;
            UCHAR Reserved;
            USHORT SecondaryStatus;
            UCHAR PrimaryBus;
            UCHAR SecondaryBus;
            UCHAR SubordinateBus;
            UCHAR SecondaryLatency;
            struct
            {
                ULONG Base;
                ULONG Limit;
            } Range[PCI_TYPE2_ADDRESSES - 1];
            UCHAR InterruptLine;
            UCHAR InterruptPin;
            USHORT BridgeControl;
        } type2;
    } u;
} PCI_COMMON_HEADER, *PPCI_COMMON_HEADER;

/* PCI and PCI-E common config structure */
typedef struct _PCI_COMMON_CONFIG
{
    PCI_COMMON_HEADER PciHeader;
    UCHAR DeviceSpecific[192];
} PCI_COMMON_CONFIG, *PPCI_COMMON_CONFIG;

/* PCI device independent region structure */
typedef struct _PCI_DEVICE_INDEPENDENT_REGION
{
    USHORT VendorId;
    USHORT DeviceId;
    USHORT Command;
    USHORT Status;
    UCHAR RevisionID;
    UCHAR ClassCode[3];
    UCHAR CacheLineSize;
    UCHAR LaytencyTimer;
    UCHAR HeaderType;
    UCHAR BIST;
} PCI_DEVICE_INDEPENDENT_REGION, *PPCI_DEVICE_INDEPENDENT_REGION;

/* PCI device header type region structure */
typedef struct _PCI_DEVICE_HEADER_TYPE_REGION
{
    UINT Bar[6];
    UINT CISPtr;
    USHORT SubsystemVendorID;
    USHORT SubsystemID;
    UINT ExpansionRomBar;
    UINT Reserved[2];
    UCHAR InterruptLine;
    UCHAR InterruptPin;
    UCHAR MinGnt;
    UCHAR MaxLat;
} PCI_DEVICE_HEADER_TYPE_REGION, *PPCI_DEVICE_HEADER_TYPE_REGION;

/* PCI device type 0 structure */
typedef struct _PCI_TYPE0_DEVICE
{
    PCI_DEVICE_INDEPENDENT_REGION Hdr;
    PCI_DEVICE_HEADER_TYPE_REGION Device;
} PCI_TYPE0_DEVICE, *PPCI_TYPE0_DEVICE;

/* PCI device type 1 structure */
typedef struct _PCI_TYPE1_DEVICE
{
    PCI_DEVICE_INDEPENDENT_REGION Hdr;
    PCI_BRIDGE_CONTROL_REGISTER Bridge;
} PCI_TYPE1_DEVICE, *PPCI_TYPE1_DEVICE;

#endif /* __XTDK_IOTYPES_H */
