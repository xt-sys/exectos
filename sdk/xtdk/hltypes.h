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

/* Serial (COM) port initial state */
typedef struct _CPPORT
{
    PUCHAR Address;
    ULONG Baud;
    USHORT Flags;
} CPPORT, *PCPPORT;

#endif /* __XTDK_HLTYPES_H */
