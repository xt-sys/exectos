/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/exports.cc
 * DESCRIPTION:     C-compatible API wrappers for exported kernel functions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Retrieves the current value of the high-resolution performance counter.
 *
 * @param PerformanceFrequency
 *        Suplies an optional pointer to a variable that receives the performance counter frequency in Hz.
 *
 * @return This routine returns the current 64-bit monotonic tick count.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
LARGE_INTEGER
HlQueryPerformanceCounter(OUT PLARGE_INTEGER PerformanceFrequency)
{
    return HL::Timer::QueryPerformanceCounter(PerformanceFrequency);
}

/**
 * Reads the 8-bit data from the specified I/O port.
 *
 * @param Port
 *        Specifies the address to read from, in the range of 0-0xFFFF.
 *
 * @return The value read from the port.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
UCHAR
HlReadPort8(IN USHORT Port)
{
    return HL::IoPort::ReadPort8(Port);
}

/**
 * Reads the 16-bit data from the specified I/O port.
 *
 * @param Port
 *        Specifies the address to read from, in the range of 0-0xFFFF.
 *
 * @return The value read from the port.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
USHORT
HlReadPort16(IN USHORT Port)
{
    return HL::IoPort::ReadPort16(Port);
}

/**
 * Reads the 32-bit data from the specified I/O port.
 *
 * @param Port
 *        Specifies the address to read from, in the range of 0-0xFFFF.
 *
 * @return The value read from the port.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
ULONG
HlReadPort32(IN USHORT Port)
{
    return HL::IoPort::ReadPort32(Port);
}

/**
 * Reads an 8-bit data from a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address holding data to read.
 *
 * @return This routine returns a value at the specified register.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
UCHAR
HlReadRegister8(IN PVOID Register)
{
    return HL::IoRegister::ReadRegister8(Register);
}

/**
 * Reads a 16-bit data from a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address holding data to read.
 *
 * @return This routine returns a value at the specified register.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
USHORT
HlReadRegister16(IN PVOID Register)
{
    return HL::IoRegister::ReadRegister16(Register);
}

/**
 * Reads a 32-bit data from a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address holding data to read.
 *
 * @return This routine returns a value at the specified register.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
ULONG
HlReadRegister32(IN PVOID Register)
{
    return HL::IoRegister::ReadRegister32(Register);
}

/**
 * Requests a dynamic adjustment of the system clock resolution.
 *
 * @param Rate
 *        The requested clock rate change in 100-nanosecond units.
 *
 * @return This routine returns the actual clock rate granted by the hardware.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
ULONG
HlSetClockRate(IN ULONG Rate)
{
    return HL::Timer::SetClockRate(Rate);
}

/**
 * Writes the 8-bit data to the specified I/O port.
 *
 * @param Port
 *        Specifies the address to write to, in the range of 0-0xFFFF.
 *
 * @param Value
 *        Supplies the value to write.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
VOID
HlWritePort8(IN USHORT Port,
             IN UCHAR Value)
{
    HL::IoPort::WritePort8(Port, Value);
}

/**
 * Writes the 16-bit data to the specified I/O port.
 *
 * @param Port
 *        Specifies the address to write to, in the range of 0-0xFFFF.
 *
 * @param Value
 *        Supplies the value to write.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
VOID
HlWritePort16(IN USHORT Port,
              IN USHORT Value)
{
    HL::IoPort::WritePort16(Port, Value);
}

/**
 * Writes the 32-bit data to the specified I/O port.
 *
 * @param Port
 *        Specifies the address to write to, in the range of 0-0xFFFF.
 *
 * @param Value
 *        Supplies the value to write.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTCDECL
VOID
HlWritePort32(IN USHORT Port,
              IN ULONG Value)
{
    HL::IoPort::WritePort32(Port, Value);
}

/**
 * Writes an 8-bit value into a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address where data will be written.
 *
 * @param Value
 *        Supplies a new value that will be stored into a register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
HlWriteRegister8(IN PVOID Register,
                 IN UCHAR Value)
{
    HL::IoRegister::WriteRegister8(Register, Value);
}

/**
 * Writes a 16-bit value into a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address where data will be written.
 *
 * @param Value
 *        Supplies a new value that will be stored into a register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
HlWriteRegister16(IN PVOID Register,
                  IN USHORT Value)
{
    HL::IoRegister::WriteRegister16(Register, Value);
}

/**
 * Writes a 32-bit value into a specified register address.
 *
 * @param Register
 *        Supplies a pointer to register address where data will be written.
 *
 * @param Value
 *        Supplies a new value that will be stored into a register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
HlWriteRegister32(IN PVOID Register,
                  IN ULONG Value)
{
    HL::IoRegister::WriteRegister32(Register, Value);
}
