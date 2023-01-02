/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/iotypes.h
 * DESCRIPTION:     I/O related type definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_IOTYPES_H
#define __XTDK_IOTYPES_H

#include "xttypes.h"


/* Number of PCI base address registers */
#define PCI_TYPE0_ADDRESSES             6
#define PCI_TYPE1_ADDRESSES             2
#define PCI_TYPE2_ADDRESSES             5

/* PCI and PCI-E common header structure */
typedef struct _PCI_COMMON_HEADER {
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

#endif /* __XTDK_IOTYPES_H */
