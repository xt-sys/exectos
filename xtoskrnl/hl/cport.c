/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/cport.c
 * DESCRIPTION:     Serial (COM) port support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * This routine gets a byte from serial port.
 *
 * @param Port
 *        Address of port object describing a port settings.
 *
 * @param Byte
 *        Address of variable where to store the result.
 *
 * @param Wait
 *        Specifies whether to wait for a data or not.
 *
 * @param Poll
 *        Indicates whether to only poll, not reading the data.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
XTSTATUS
HlComPortGetByte(IN PCPPORT Port,
                 OUT PUCHAR Byte,
                 IN BOOLEAN Wait,
                 IN BOOLEAN Poll)
{
    UCHAR Lsr;
    ULONG Retry;

    /* Make sure the port has been initialized */
    if(Port->Address == 0)
    {
        return STATUS_DEVICE_NOT_READY;
    }

    /* Retry getting data if allowed to wait */
    Retry = Wait ? COMPORT_WAIT_TIMEOUT : 1;
    while(Retry--)
    {
        /* Get LSR for data ready */
        Lsr = HlComPortReadLsr(Port, COMPORT_LSR_DR);
        if((Lsr & COMPORT_LSR_DR) == COMPORT_LSR_DR)
        {
            /* Check for errors */
            if(Lsr & (COMPORT_LSR_FE | COMPORT_LSR_OE | COMPORT_LSR_PE))
            {
                /* Framing, parity or overrun error occurred */
                *Byte = 0;
                return STATUS_IO_DEVICE_ERROR;
            }

            /* Check if only polling */
            if(Poll)
            {
                /* Only polling, return success */
                return STATUS_SUCCESS;
            }

            /* Read the byte from serial port */
            *Byte = HlIoPortInByte(PtrToUshort(Port->Address + (ULONG)COMPORT_REG_RBR));

            /* Check if in modem control mode */
            if(Port->Flags & COMPORT_FLAG_MC)
            {
                /* Handle Carrier Detected (CD) */
                if((HlIoPortInByte(PtrToShort(Port->Address + (ULONG)COMPORT_REG_MSR)) & COMPORT_MSR_DCD) == 0)
                {
                    /* Skip byte if no CD present */
                    continue;
                }
            }
            return STATUS_SUCCESS;
        }
    }

    /* Reset LSR and return that no data found */
    HlComPortReadLsr(Port, 0);
    return STATUS_NOT_FOUND;
}

/**
 * This routine writes a byte to the serial port.
 *
 * @param Port
 *        Address of port object describing a port settings.
 *
 * @param Byte
 *        Data to be written.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
XTSTATUS 
HlComPortPutByte(IN PCPPORT Port,
                 IN UCHAR Byte)
{
    UCHAR Lsr, Msr;

    /* Make sure the port has been initialized */
    if(Port->Address == 0)
    {
        return STATUS_DEVICE_NOT_READY;
    }

    /* Check if port is in modem control */
    while(Port->Flags & COMPORT_FLAG_MC)
    {
        /* Get the Modem Status Register (MSR) */
        Msr = HlIoPortInByte(PtrToUshort(Port->Address + (ULONG)COMPORT_REG_MSR)) & COMPORT_MSR_DSRCTSCD;
        if(Msr != COMPORT_MSR_DSRCTSCD)
        {
            /* Take character, if CD is not set */
            Lsr = HlComPortReadLsr(Port, 0);
            if((Msr & COMPORT_MSR_DCD) == 0 && (Lsr & COMPORT_LSR_DR) == COMPORT_LSR_DR)
            {
                /* Eat the character */
                HlIoPortInByte(PtrToUshort(Port->Address + (ULONG)COMPORT_REG_RBR));
            }
        }
        else
        {
            /* CD, CTS and DSR are set, we can continue */
            break;
        }
    }

    /* Wait for busy port */
    while((HlComPortReadLsr(Port, COMPORT_LSR_THRE) & COMPORT_LSR_THRE) == 0);

    /* Send byte to the port */
    HlIoPortOutByte(PtrToUshort(Port->Address + (ULONG)COMPORT_REG_THR), Byte);

    return STATUS_SUCCESS;
}

/**
 * Reads LSR from specified serial port.
 *
 * @param Port
 *        Address of COM port.
 *
 * @param Byte
 *        Value expected from the port.
 *
 * @return Byte read from COM port.
 *
 * @since XT 1.0
 */
XTCDECL
UCHAR
HlComPortReadLsr(IN PCPPORT Port,
                 IN UCHAR Byte)
{
    UCHAR Lsr, Msr;
    STATIC UCHAR RingFlag;

    /* Read the Line Status Register (LSR) */
    Lsr = HlIoPortInByte(PtrToUshort(Port->Address + (ULONG)COMPORT_REG_LSR));

    /* Check if expected byte is present */
    if((Lsr & Byte) == 0)
    {
        /* Check Modem Status Register (MSR) for ring indicator */
        Msr = HlIoPortInByte(PtrToUshort(Port->Address + (ULONG)COMPORT_REG_MSR));
        RingFlag |= (Msr & COMPORT_MSR_RI) ? 1 : 2;
        if(RingFlag == 3)
        {
            /* Ring indicator toggled, use modem control */
            Port->Flags |= COMPORT_FLAG_MC;
        }
    }

    /* Return byte read */
    return Lsr;
}

/**
 * This routine initializes the COM port.
 *
 * @param Port
 *        Address of port object describing a port settings.
 *
 * @param PortNumber
 *        Supplies a port number.
 *
 * @param PortAddress
 *        Supplies an address of the COM port.
 *
 * @param BaudRate
 *        Supplies an optional port baud rate.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
XTSTATUS
HlInitializeComPort(IN OUT PCPPORT Port,
                    IN PUCHAR PortAddress,
                    IN ULONG BaudRate)
{
    USHORT Flags;
    UCHAR Byte;
    ULONG Mode;

    /* Initialize variables */
    Byte = 0;
    Flags = 0;

    /* Check if baud rate is set */
    if(BaudRate == 0)
    {
        /* Use default baud (clock) rate if not set */
        BaudRate = COMPORT_CLOCK_RATE;
        Flags |= COMPORT_FLAG_DBR;
    }

    /* Check whether this port is not already initialized */
    if((Port->Address == PortAddress) && (Port->Baud == BaudRate))
    {
        return STATUS_SUCCESS;
    }

    /* Test if chosen COM port exists */
    do
    {
        /* Check whether the 16450/16550 scratch register exists */
        HlIoPortOutByte(PtrToUshort(PortAddress + (ULONG)COMPORT_REG_SR), Byte);
        if(HlIoPortInByte(PtrToUshort(PortAddress + (ULONG)COMPORT_REG_SR)) != Byte)
        {
            return STATUS_NOT_FOUND;
        }
    }
    while(++Byte != 0);

    /* Disable interrupts */
    HlIoPortOutByte(PtrToUshort(PortAddress + (ULONG)COMPORT_REG_LCR), COMPORT_LSR_DIS);
    HlIoPortOutByte(PtrToUshort(PortAddress + (ULONG)COMPORT_REG_IER), COMPORT_LSR_DIS);

    /* Enable Divisor Latch Access Bit (DLAB) */
    HlIoPortOutByte(PtrToUshort(PortAddress + (ULONG)COMPORT_REG_LCR), COMPORT_LCR_DLAB);

    /* Set baud rate */
    Mode = COMPORT_CLOCK_RATE / BaudRate;
    HlIoPortOutByte(PtrToUshort(PortAddress + (ULONG)COMPORT_DIV_DLL), (UCHAR)(Mode & 0xFF));
    HlIoPortOutByte(PtrToUshort(PortAddress + (ULONG)COMPORT_DIV_DLM), (UCHAR)((Mode >> 8) & 0xFF));

    /* Set 8 data bits, 1 stop bits, no parity (8n1) */
    HlIoPortOutByte(PtrToUshort(PortAddress + (ULONG)COMPORT_REG_LCR),
                       COMPORT_LCR_8DATA | COMPORT_LCR_1STOP | COMPORT_LCR_PARN);

    /* Enable DTR, RTS and OUT2 */
    HlIoPortOutByte(PtrToUshort(PortAddress + (ULONG)COMPORT_REG_MCR),
                       COMPORT_MCR_DTR | COMPORT_MCR_RTS | COMPORT_MCR_OUT2);

    /* Enable FIFO */
    HlIoPortOutByte(PtrToUshort(PortAddress + (ULONG)COMPORT_REG_FCR),
                       COMPORT_FCR_ENABLE | COMPORT_FCR_RCVR_RESET | COMPORT_FCR_TXMT_RESET);

    /* Mark port as fully initialized */
    Flags |= COMPORT_FLAG_INIT;

    /* Make sure port works in Normal Operation Mode (NOM) */
    HlIoPortOutByte(PtrToUshort(PortAddress + (ULONG)COMPORT_REG_MCR), COMPORT_MCR_NOM);

    /* Read junk data out of the Receive Buffer Register (RBR) */
    HlIoPortInByte(PtrToUshort(Port->Address + (ULONG)COMPORT_REG_RBR));

    /* Store port details */
    Port->Address = PortAddress;
    Port->Baud = BaudRate;
    Port->Flags = Flags;

    /* Return success */
    return STATUS_SUCCESS;
}
