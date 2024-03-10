/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/hltypes.h
 * DESCRIPTION:     XT hardware abstraction layer structures definitions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_HLTYPES_H
#define __XTDK_HLTYPES_H

#include <xttypes.h>


/* Default serial port settings */
#define COMPORT_CLOCK_RATE          0x1C200
#define COMPORT_WAIT_TIMEOUT        204800

/* Serial port divisors */
#define COMPORT_DIV_DLL             0x00 /* Divisor Latch Least */
#define COMPORT_DIV_DLM             0x01 /* Divisor Latch Most */

/* Serial port control flags */
#define COMPORT_FLAG_INIT           0x01 /* Port Initialized */
#define COMPORT_FLAG_DBR            0x02 /* Default Baud Rate */
#define COMPORT_FLAG_MC             0x04 /* Modem Control */

/* Serial port Fifo Control Register (FCR) access masks */
#define COMPORT_FCR_DISABLE         0x00 /* Disable */
#define COMPORT_FCR_ENABLE          0x01 /* Enable */
#define COMPORT_FCR_RCVR_RESET      0x02 /* Receiver Reset */
#define COMPORT_FCR_TXMT_RESET      0x04 /* Transmitter Reset */

/* Serial port Line Control Register (LCR) access masks */
#define COMPORT_LCR_1STOP           0x00 /* 1 Stop Bit */
#define COMPORT_LCR_2STOP           0x04 /* 2 Stop Bits */
#define COMPORT_LCR_5DATA           0x00 /* 5 Data Bits */
#define COMPORT_LCR_6DATA           0x01 /* 6 Data Bits */
#define COMPORT_LCR_7DATA           0x02 /* 7 Data Bits */
#define COMPORT_LCR_8DATA           0x03 /* 8 Data Bits */
#define COMPORT_LCR_PARN            0x00 /* None Parity */
#define COMPORT_LCR_PARO            0x08 /* Odd Parity */
#define COMPORT_LCR_PARE            0x18 /* Even Parity */
#define COMPORT_LCR_PARM            0x28 /* Mark Parity */
#define COMPORT_LCR_PARS            0x38 /* Space Parity */
#define COMPORT_LCR_BREAK           0x40 /* Break */
#define COMPORT_LCR_DLAB            0x80 /* Divisor Latch Access Bit */

/* Serial port Line Status Register (LSR) access masks */
#define COMPORT_LSR_DIS             0x00 /* Disable */
#define COMPORT_LSR_DR              0x01 /* Data Ready */
#define COMPORT_LSR_OE              0x02 /* Overrun Error */
#define COMPORT_LSR_PE              0x04 /* Parity Error */
#define COMPORT_LSR_FE              0x08 /* Framing Error */
#define COMPORT_LSR_BI              0x10 /* Break Interrupt */
#define COMPORT_LSR_THRE            0x20 /* Transmit Holding Register Empty */
#define COMPORT_LSR_TEMPTY          0x40 /* Transmitter Empty */
#define COMPORT_LSR_FIFOE           0x80 /* FIFO Error */

/* Serial port Modem Control Register (MCR) access masks */
#define COMPORT_MCR_DTR             0x01 /* Data Terminal Ready */
#define COMPORT_MCR_RTS             0x02 /* Ready To Send */
#define COMPORT_MCR_OUT1            0x04 /* Generic Output 1 */
#define COMPORT_MCR_OUT2            0x08 /* Generic Output 2 */
#define COMPORT_MCR_NOM             0x0F /* Normal Operation Mode */
#define COMPORT_MCR_LOOP            0x10 /* Loopback Testing Mode */

/* Serial port Modem Status Register (MSR) access masks */
#define COMPORT_MSR_DCTS            0x01 /* Delta Clear To Send */
#define COMPORT_MSR_DDSR            0x02 /* Delta Data Set Ready */
#define COMPORT_MSR_DTRRTS          0x03 /* DTR and RTS */
#define COMPORT_MSR_TERI            0x04 /* Trailing Edge Ring Indicator */
#define COMPORT_MSR_DDCD            0x08 /* Delta Data Carrier Detect */
#define COMPORT_MSR_CTS             0x10 /* Clear To Send */
#define COMPORT_MSR_DSR             0x20 /* Data Set Ready */
#define COMPORT_MSR_RI              0x40 /* Ring Indicator */
#define COMPORT_MSR_DCD             0x80 /* Data Carrier Detect */
#define COMPORT_MSR_DSRCTSCD        0xB0 /* DSR, CTS and CD */
#define COMPORT_MSR_TST             0xAE /* Test Pattern */

/* Serial port offsets of the various registers */
#define COMPORT_REG_RBR             0x00 /* Receive Buffer Register */
#define COMPORT_REG_THR             0x00 /* Transmit Holding Register */
#define COMPORT_REG_IER             0x01 /* Interrupt Enable Register */
#define COMPORT_REG_IIR             0x02 /* Interrupt Identity Register */
#define COMPORT_REG_FCR             0x02 /* FIFO Control Register */
#define COMPORT_REG_LCR             0x03 /* Line Control Register */
#define COMPORT_REG_MCR             0x04 /* Modem Control Register */
#define COMPORT_REG_LSR             0x05 /* Line Status Register */
#define COMPORT_REG_MSR             0x06 /* Modem Status Register */
#define COMPORT_REG_SR              0x07 /* Scratch Register */

/* APIC Register Address Map */
typedef enum _APIC_REGISTER
{
    APIC_ID                       = 0x02, /* APIC ID Register */
    APIC_VER                      = 0x03, /* APIC Version Register */
    APIC_TPR                      = 0x08, /* Task Priority Register */
    APIC_APR                      = 0x09, /* Arbitration Priority Register */
    APIC_PPR                      = 0x0A, /* Processor Priority Register (R) */
    APIC_EOI                      = 0x0B, /* EOI Register */
    APIC_RRR                      = 0x0C, /* Remote Read Register */
    APIC_LDR                      = 0x0D, /* Logical Destination Register */
    APIC_DFR                      = 0x0E, /* Destination Format Register (not available in extended mode) */
    APIC_SIVR                     = 0x0F, /* Spurious Interrupt Vector Register */
    APIC_ISR                      = 0x10, /* Interrupt Service Register*/
    APIC_TMR                      = 0x18, /* Trigger Mode Register */
    APIC_IRR                      = 0x20, /* Interrupt Request Register */
    APIC_ESR                      = 0x28, /* Error Status Register */
    APIC_ICR0                     = 0x30, /* Interrupt Command Register */
    APIC_ICR1                     = 0x31, /* Interrupt Command Register (not available in extended mode) */
    APIC_TMRLVTR                  = 0x32, /* Timer Local Vector Table */
    APIC_THRMLVTR                 = 0x33, /* Thermal Local Vector Table */
    APIC_PCLVTR                   = 0x34, /* Performance Counter Local Vector Table */
    APIC_LINT0                    = 0x35, /* LINT0 Local Vector Table */
    APIC_LINT1                    = 0x36, /* LINT1 Local Vector Table */
    APIC_ERRLVTR                  = 0x37, /* Error Local Vector Table */
    APIC_TICR                     = 0x38, /* Initial Count Register for Timer */
    APIC_TCCR                     = 0x39, /* Current Count Register for Timer */
    APIC_TDCR                     = 0x3E, /* Timer Divide Configuration Register */
    APIC_EAFR                     = 0x40, /* extended APIC Feature register */
    APIC_EACR                     = 0x41, /* Extended APIC Control Register */
    APIC_SEOI                     = 0x42, /* Specific End Of Interrupt Register */
    APIC_EXT0LVTR                 = 0x50, /* Extended Interrupt 0 Local Vector Table */
    APIC_EXT1LVTR                 = 0x51, /* Extended Interrupt 1 Local Vector Table */
    APIC_EXT2LVTR                 = 0x52, /* Extended Interrupt 2 Local Vector Table */
    APIC_EXT3LVTR                 = 0x53  /* Extended Interrupt 3 Local Vector Table */
} APIC_REGISTER, *PAPIC_REGISTER;

/* APIC mode list */
typedef enum _HAL_APIC_MODE
{
    APIC_MODE_COMPAT,
    APIC_MODE_X2APIC
} HAL_APIC_MODE, *PHAL_APIC_MODE;

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
