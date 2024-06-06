/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/hltypes.h
 * DESCRIPTION:     XT hardware abstraction layer structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_HLTYPES_H
#define __XTDK_HLTYPES_H

#include <xtbase.h>
#include <xtdefs.h>
#include <xttypes.h>
#include ARCH_HEADER(hltypes.h)


/* ACPI Root System Description Pointer (RSDP) signature */
#define ACPI_RSDP_SIGNATURE                         0x2052545020445352

/* ACPI table signatures */
#define ACPI_BERT_SIGNATURE                         0x54524542 /* Boot Error Record Table */
#define ACPI_BGRT_SIGNATURE                         0x54524742 /* Boot Graphics Record Table */
#define ACPI_BOOT_SIGNATURE                         0x544F4F42 /* ACPI BOOT Table */
#define ACPI_CPEP_SIGNATURE                         0x50455043 /* Corrected Platform Error Polling Table */
#define ACPI_DBG2_SIGNATURE                         0x32474244 /* Debug Port Table v2 */
#define ACPI_DBGP_SIGNATURE                         0x50474244 /* Debug Port Table */
#define ACPI_DMAR_SIGNATURE                         0x52414D44 /* DMA Remapping Table */
#define ACPI_DSDT_SIGNATURE                         0x54445344 /* Differentiated System Description Table */
#define ACPI_ECDT_SIGNATURE                         0x54444345 /* Embedded Controller Description Table */
#define ACPI_ERST_SIGNATURE                         0x54535245 /* Error Record Serialization Table */
#define ACPI_FACS_SIGNATURE                         0x53434146 /* Firmware ACPI Control Structure */
#define ACPI_FADT_SIGNATURE                         0x50434146 /* Fixed ACPI Description Table */
#define ACPI_FBPT_SIGNATURE                         0x54504246 /* Firmware Boot Performance Table */
#define ACPI_FPDT_SIGNATURE                         0x54445046 /* Firmware Performance Data Table */
#define ACPI_GTDT_SIGNATURE                         0x54445447 /* Generic Timer Description Table */
#define ACPI_HPET_SIGNATURE                         0x54455048 /* High Precision Event Timer */
#define ACPI_IVRS_SIGNATURE                         0x53525649 /* AMD IOMMU Resource Table */
#define ACPI_MADT_SIGNATURE                         0x43495041 /* MADT/APIC Description Table */
#define ACPI_MCFG_SIGNATURE                         0x4746434D /* Memory Mapped Configuration Space Access Table */
#define ACPI_MPST_SIGNATURE                         0x5453504D /* Memory Power State Table*/
#define ACPI_MSCT_SIGNATURE                         0x5443534D /* Maximum System Characteristics Table */
#define ACPI_NFIT_SIGNATURE                         0x5449464E /* NVDIMM Firmware Interface Table */
#define ACPI_PMMT_SIGNATURE                         0x544D4D50 /* Platform Memory Topology Table */
#define ACPI_PSDT_SIGNATURE                         0x54445350 /* Persistent System Description Table */
#define ACPI_RAS2_SIGNATURE                         0x32534152 /* ACPI RAS2 Feature Table */
#define ACPI_RASF_SIGNATURE                         0x46534152 /* ACPI RAS Feature Table */
#define ACPI_RSDT_SIGNATURE                         0x54445352 /* Root System Description Table */
#define ACPI_SBST_SIGNATURE                         0x54534253 /* Smart Battery Subsystem Table */
#define ACPI_SDEV_SIGNATURE                         0x56454453 /* Secure Device Table */
#define ACPI_SLIT_SIGNATURE                         0x54494C53 /* System Locality Distance Information Table */
#define ACPI_SPCR_SIGNATURE                         0x52435053 /* Serial Port Console Redirection Table */
#define ACPI_SRAT_SIGNATURE                         0x54415253 /* Static Resource Affinity Table */
#define ACPI_SSDT_SIGNATURE                         0x54445353 /* Secondary System Descriptor Table */
#define ACPI_TPM2_SIGNATURE                         0x324D5054 /* ACPI TPM 2.0 Table */
#define ACPI_WAET_SIGNATURE                         0x54454157 /* Windows ACPI Enlightenment Table */
#define ACPI_WDAT_SIGNATURE                         0x54414457 /* Watch Dog Action Table */
#define ACPI_WDTT_SIGNATURE                         0x54524457 /* Watchdog Timer Resource Table */
#define ACPI_WPBT_SIGNATURE                         0x54425057 /* Windows Platform Binary Table */
#define ACPI_WSMT_SIGNATURE                         0x544D5357 /* Windows SMM Security Mitigation Table */
#define ACPI_XSDT_SIGNATURE                         0x54445358 /* eXtended System Descriptor Table */

/* ACPI FADT flags masks */
#define ACPI_FADT_32BIT_TIMER                       (1<<8)

/* ACPI Timer bit masks */
#define ACPI_FADT_TIMER_32BIT                       0x80000000
#define ACPI_FADT_TIMER_24BIT                       0x00800000

/* ACPI MADT subtable type definitions */
#define ACPI_MADT_LOCAL_APIC                        0
#define ACPI_MADT_IOAPIC                            1
#define ACPI_MADT_INT_OVERRIDE                      2
#define ACPI_MADT_NMI_SOURCE                        3
#define ACPI_MADT_LOCAL_NMI_SOURCE                  4
#define ACPI_MADT_ADDRESS_EXTENSION                 5
#define ACPI_MADT_IO_SAPIC                          6
#define ACPI_MADT_LOCAL_SAPIC                       7
#define ACPI_MADT_PLATFORM_INTERRUPT_SOURCE         8

/* ACPI MADT Processor Local APIC Flags */
#define ACPI_MADT_PLACE_ENABLED                     0 /* Processor Local APIC CPU Enabled */
#define ACPI_MADT_PLAOC_ENABLED                     1 /* Processor Local APIC Online Capable */

/* Default serial port settings */
#define COMPORT_CLOCK_RATE                          0x1C200
#define COMPORT_WAIT_TIMEOUT                        204800

/* Serial port divisors */
#define COMPORT_DIV_DLL                             0x00 /* Divisor Latch Least */
#define COMPORT_DIV_DLM                             0x01 /* Divisor Latch Most */

/* Serial port control flags */
#define COMPORT_FLAG_INIT                           0x01 /* Port Initialized */
#define COMPORT_FLAG_DBR                            0x02 /* Default Baud Rate */
#define COMPORT_FLAG_MC                             0x04 /* Modem Control */

/* Serial port Fifo Control Register (FCR) access masks */
#define COMPORT_FCR_DISABLE                         0x00 /* Disable */
#define COMPORT_FCR_ENABLE                          0x01 /* Enable */
#define COMPORT_FCR_RCVR_RESET                      0x02 /* Receiver Reset */
#define COMPORT_FCR_TXMT_RESET                      0x04 /* Transmitter Reset */

/* Serial port Line Control Register (LCR) access masks */
#define COMPORT_LCR_1STOP                           0x00 /* 1 Stop Bit */
#define COMPORT_LCR_2STOP                           0x04 /* 2 Stop Bits */
#define COMPORT_LCR_5DATA                           0x00 /* 5 Data Bits */
#define COMPORT_LCR_6DATA                           0x01 /* 6 Data Bits */
#define COMPORT_LCR_7DATA                           0x02 /* 7 Data Bits */
#define COMPORT_LCR_8DATA                           0x03 /* 8 Data Bits */
#define COMPORT_LCR_PARN                            0x00 /* None Parity */
#define COMPORT_LCR_PARO                            0x08 /* Odd Parity */
#define COMPORT_LCR_PARE                            0x18 /* Even Parity */
#define COMPORT_LCR_PARM                            0x28 /* Mark Parity */
#define COMPORT_LCR_PARS                            0x38 /* Space Parity */
#define COMPORT_LCR_BREAK                           0x40 /* Break */
#define COMPORT_LCR_DLAB                            0x80 /* Divisor Latch Access Bit */

/* Serial port Line Status Register (LSR) access masks */
#define COMPORT_LSR_DIS                             0x00 /* Disable */
#define COMPORT_LSR_DR                              0x01 /* Data Ready */
#define COMPORT_LSR_OE                              0x02 /* Overrun Error */
#define COMPORT_LSR_PE                              0x04 /* Parity Error */
#define COMPORT_LSR_FE                              0x08 /* Framing Error */
#define COMPORT_LSR_BI                              0x10 /* Break Interrupt */
#define COMPORT_LSR_THRE                            0x20 /* Transmit Holding Register Empty */
#define COMPORT_LSR_TEMPTY                          0x40 /* Transmitter Empty */
#define COMPORT_LSR_FIFOE                           0x80 /* FIFO Error */

/* Serial port Modem Control Register (MCR) access masks */
#define COMPORT_MCR_DTR                             0x01 /* Data Terminal Ready */
#define COMPORT_MCR_RTS                             0x02 /* Ready To Send */
#define COMPORT_MCR_OUT1                            0x04 /* Generic Output 1 */
#define COMPORT_MCR_OUT2                            0x08 /* Generic Output 2 */
#define COMPORT_MCR_NOM                             0x0F /* Normal Operation Mode */
#define COMPORT_MCR_LOOP                            0x10 /* Loopback Testing Mode */

/* Serial port Modem Status Register (MSR) access masks */
#define COMPORT_MSR_DCTS                            0x01 /* Delta Clear To Send */
#define COMPORT_MSR_DDSR                            0x02 /* Delta Data Set Ready */
#define COMPORT_MSR_DTRRTS                          0x03 /* DTR and RTS */
#define COMPORT_MSR_TERI                            0x04 /* Trailing Edge Ring Indicator */
#define COMPORT_MSR_DDCD                            0x08 /* Delta Data Carrier Detect */
#define COMPORT_MSR_CTS                             0x10 /* Clear To Send */
#define COMPORT_MSR_DSR                             0x20 /* Data Set Ready */
#define COMPORT_MSR_RI                              0x40 /* Ring Indicator */
#define COMPORT_MSR_DCD                             0x80 /* Data Carrier Detect */
#define COMPORT_MSR_DSRCTSCD                        0xB0 /* DSR, CTS and CD */
#define COMPORT_MSR_TST                             0xAE /* Test Pattern */

/* Serial port offsets of the various registers */
#define COMPORT_REG_RBR                             0x00 /* Receive Buffer Register */
#define COMPORT_REG_THR                             0x00 /* Transmit Holding Register */
#define COMPORT_REG_IER                             0x01 /* Interrupt Enable Register */
#define COMPORT_REG_IIR                             0x02 /* Interrupt Identity Register */
#define COMPORT_REG_FCR                             0x02 /* FIFO Control Register */
#define COMPORT_REG_LCR                             0x03 /* Line Control Register */
#define COMPORT_REG_MCR                             0x04 /* Modem Control Register */
#define COMPORT_REG_LSR                             0x05 /* Line Status Register */
#define COMPORT_REG_MSR                             0x06 /* Modem Status Register */
#define COMPORT_REG_SR                              0x07 /* Scratch Register */

/* Generic Address structure */
typedef struct _GENERIC_ADDRESS
{
    UCHAR AddressSpaceID;
    UCHAR BitWidth;
    UCHAR BitOffset;
    UCHAR Reserved;
    PHYSICAL_ADDRESS Address;
} GENERIC_ADDRESS, *PGENERIC_ADDRESS;

/* Each ACPI table description header structure */
typedef struct _ACPI_DESCRIPTION_HEADER
{
    ULONG Signature;
    ULONG Length;
    UCHAR Revision;
    UCHAR Checksum;
    UCHAR OemId[6];
    UCHAR OemTableID[8];
    ULONG OemRevision;
    UCHAR CreatorID[4];
    ULONG CreatorRev;
} ACPI_DESCRIPTION_HEADER, *PACPI_DESCRIPTION_HEADER;

/* Each ACPI subtable description header structure */
typedef struct _ACPI_SUBTABLE_HEADER
{
    UCHAR Type;
    UCHAR Length;
} ACPI_SUBTABLE_HEADER, *PACPI_SUBTABLE_HEADER;

/* ACPI cache list structure */
typedef struct _ACPI_CACHE_LIST
{
    LIST_ENTRY ListEntry;
    ACPI_DESCRIPTION_HEADER Header;
} ACPI_CACHE_LIST, *PACPI_CACHE_LIST;

/* ACPI Root System Description Table Pointer (RSDP) structure */
typedef struct _ACPI_RSDP
{
    ULONGLONG Signature;
    UCHAR Checksum;
    UCHAR OemId[6];
    UCHAR Revision;
    ULONG RsdtAddress;
    ULONG Length;
    ULONGLONG XsdtAddress;
    UCHAR XChecksum;
    UCHAR Reserved[3];
} ACPI_RSDP, *PACPI_RSDP;

/* ACPI Root System Description Table (RSDT) structure */
typedef struct _ACPI_RSDT
{
    ACPI_DESCRIPTION_HEADER Header;
    ULONG Tables[];
} ACPI_RSDT, *PACPI_RSDT;

/* ACPI eXtended Root System Description Table (XSDT) structure */
typedef struct _ACPI_XSDT
{
    ACPI_DESCRIPTION_HEADER Header;
    ULONGLONG Tables[];
} PACKED ACPI_XSDT, *PACPI_XSDT;

/* Fixed ACPI Description Table (FADT) structure */
typedef struct _ACPI_FADT
{
    ACPI_DESCRIPTION_HEADER Header;
    ULONG FirmwareCtrl;
    ULONG Dsdt;
    UCHAR IntModel;
    UCHAR PmProfile;
    USHORT SciIntVector;
    ULONG SmiCmdIoPort;
    UCHAR AcpiOnValue;
    UCHAR AcpiOffValue;
    UCHAR S4BiosReq;
    UCHAR PStateControl;
    ULONG Pm1aEvtBlkIoPort;
    ULONG Pm1bEvtBlkIoPort;
    ULONG Pm1aCtrlBlkIoPort;
    ULONG Pm1bCtrlBlkIoPort;
    ULONG Pm2CtrlBlkIoPort;
    ULONG PmTmrBlkIoPort;
    ULONG Gp0BlkIoPort;
    ULONG Gp1BlkIoPort;
    UCHAR Pm1EvtLen;
    UCHAR Pm1CtrlLen;
    UCHAR Pm2CtrlLen;
    UCHAR PmTmrLen;
    UCHAR Gp0BlkLen;
    UCHAR Gp1BlkLen;
    UCHAR Gp1Base;
    UCHAR CStateControl;
    USHORT Lvl2Latency;
    USHORT Lvl3Latency;
    USHORT FlushSize;
    USHORT FlushStride;
    UCHAR DutyOffset;
    UCHAR DutyWidth;
    UCHAR DayAlarmIndex;
    UCHAR MonthAlarmIndex;
    UCHAR CenturyAlarmIndex;
    USHORT BootArch;
    UCHAR Reserved0;
    ULONG Flags;
    GENERIC_ADDRESS ResetReg;
    UCHAR ResetVal;
    USHORT ArmBootArch;
    UCHAR Reserved1;
    PHYSICAL_ADDRESS XFirmwareCtrl;
    PHYSICAL_ADDRESS XDsdt;
    GENERIC_ADDRESS XPm1aEvtBlk;
    GENERIC_ADDRESS XPm1bEvtBlk;
    GENERIC_ADDRESS XPm1aCtrlBlk;
    GENERIC_ADDRESS XPm1bCtrlBlk;
    GENERIC_ADDRESS XPm2CtrlBlk;
    GENERIC_ADDRESS XPmTmrBlk;
    GENERIC_ADDRESS XGp0Blk;
    GENERIC_ADDRESS XGp1Blk;
    GENERIC_ADDRESS SleepControlReg;
    GENERIC_ADDRESS SleepStatusReg;
} ACPI_FADT, *PACPI_FADT;

/* ACPI Multiple APIC Description Table (MADT) structure */
typedef struct _ACPI_MADT
{
    ACPI_DESCRIPTION_HEADER Header;
    ULONG LocalApicAddress;
    ULONG Flags;
    ULONG ApicTables[];
} ACPI_MADT, *PACPI_MADT;

typedef struct _ACPI_MADT_TABLE_LOCAL_APIC
{
    ACPI_SUBTABLE_HEADER Header;
    UCHAR ProcessorId;
    UCHAR Id;
    ULONG LapicFlags;
} ACPI_MADT_TABLE_LOCAL_APIC, *PACPI_MADT_TABLE_LOCAL_APIC;

/* ACPI System Information */
typedef struct _ACPI_SYSTEM_INFO
{
    ULONG CpuCount;
    ULONG RunningCpus;
    ULONG BusCount;
    ULONG IoApicCount;
    ULONG IntiCount;
    ULONG LintiCount;
    ULONG ImcrPresent;
    ULONG ApicBase;
    ULONG IoApicPhysicalBase[APIC_MAX_IOAPICS];
    PULONG IoApicVirtualBase[APIC_MAX_IOAPICS];
    ULONG IoApicIntiBase[APIC_MAX_IOAPICS];
} ACPI_SYSTEM_INFO, *PACPI_SYSTEM_INFO;

/* ACPI Timer information structure */
typedef struct _ACPI_TIMER_INFO
{
    ULONG TimerPort;
    ULONG MsbMask;
} ACPI_TIMER_INFO, *PACPI_TIMER_INFO;

/* Serial (COM) port initial state */
typedef struct _CPPORT
{
    PUCHAR Address;
    ULONG Baud;
    USHORT Flags;
} CPPORT, *PCPPORT;

/* HAL framebuffer data structure */
typedef struct _HAL_FRAMEBUFFER_DATA
{
    BOOLEAN Initialized;
    PVOID Address;
    ULONG_PTR BufferSize;
    UINT Width;
    UINT Height;
    UINT PixelsPerScanLine;
    UINT BitsPerPixel;
    UINT Pitch;
    PVOID Font;
    struct
    {
        USHORT BlueShift;
        USHORT BlueSize;
        USHORT GreenShift;
        USHORT GreenSize;
        USHORT RedShift;
        USHORT RedSize;
        USHORT ReservedShift;
        USHORT ReservedSize;
    } Pixels;
} HAL_FRAMEBUFFER_DATA, *PHAL_FRAMEBUFFER_DATA;

/* SMBIOS table header structure */
typedef struct _SMBIOS_TABLE_HEADER
{
    UCHAR Signature[4];
    UCHAR Checksum;
    UCHAR Length;
    UCHAR MajorVersion;
    UCHAR MinorVersion;
    USHORT MaxStructureSize;
    UCHAR EntryPointRevision;
    UCHAR Reserved[5];
    UCHAR Signature2[5];
    UCHAR IntermediateChecksum;
    USHORT TableLength;
    ULONG TableAddress;
    USHORT NumberOfStructures;
    UCHAR BcdRevision;
} SMBIOS_TABLE_HEADER, *PSMBIOS_TABLE_HEADER;

/* SMBIOS3 table header structure */
typedef struct _SMBIOS3_TABLE_HEADER
{
    UCHAR Signature[5];
    UCHAR Checksum;
    UCHAR Length;
    UCHAR MajorVersion;
    UCHAR MinorVersion;
    UCHAR DocRevision;
    UCHAR EntryPointRevision;
    UCHAR Reserved;
    ULONG MaxStructureSize;
    ULONGLONG TableAddress;
} SMBIOS3_TABLE_HEADER, *PSMBIOS3_TABLE_HEADER;

#endif /* __XTDK_HLTYPES_H */
