/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/amd64/cpufunc.c
 * DESCRIPTION:     Routines to provide access to special AMD64 CPU instructions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include "xtkmapi.h"


/**
 * Retrieves a various amount of information about the CPU.
 *
 * @param Registers
 *        Supplies a pointer to the structure containing all the necessary registers and leafs for CPUID.
 *
 * @return TRUE if CPUID function could be executed, FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
HlCpuId(IN OUT PCPUID_REGISTERS Registers)
{
    UINT32 MaxLeaf;

    /* Get highest function ID available */
    asm volatile("cpuid"
                 : "=a" (MaxLeaf)
                 : "a" (Registers->Leaf & 0x80000000)
                 : "rbx", "rcx", "rdx");

    /* Check if CPU supports this command */
    if(Registers->Leaf > MaxLeaf)
    {
        /* Cannot call it, return FALSE */
        return FALSE;
    }

    /* Execute CPUID function */
    asm volatile("cpuid"
                 : "=a" (Registers->Eax),
                 "=b" (Registers->Ebx),
                 "=c" (Registers->Ecx),
                 "=d" (Registers->Edx)
                 : "a" (Registers->Leaf),
                 "c" (Registers->SubLeaf));

    /* Return TRUE */
    return TRUE;
}

/**
 * Halts the central processing unit (CPU).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlHalt()
{
    while(TRUE)
    {
        asm volatile("hlt");
    }
}

/**
 * Reads the data from the specified I/O port.
 *
 * @param Port
 *        Specifies the port number in the range of 0-0xFFFF.
 *
 * @return The value read from the port.
 *
 * @since XT 1.0
 */
XTAPI
UCHAR
HlIoPortInByte(IN USHORT Port)
{
    UCHAR Value;
    asm volatile("inb %1, %0"
                 : "=a"(Value)
                 : "Nd"(Port));
    return Value;
}

/**
 * Writes the data to the specified I/O port.
 *
 * @param Port
 *        Specifies the port number in the range of 0-0xFFFF.
 *
 * @param Value
 *        Supplies the value to write.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlIoPortOutByte(IN USHORT Port,
                IN UCHAR Value)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(Value),
                 "Nd"(Port));
}

/**
 * Reads the CR0 register and returns its value.
 *
 * @return The value stored in the CR0 register.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
HlReadCR0()
{
    ULONG_PTR Value;
    asm volatile("mov %%cr0, %0"
                 : "=r" (Value)
                 :
                 : "memory");
    return Value;
}

/**
 * Reads the CR2 register and returns its value.
 *
 * @return The value stored in the CR2 register.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
HlReadCR2()
{
    ULONG_PTR Value;
    asm volatile("mov %%cr2, %0"
                 : "=r" (Value)
                 :
                 : "memory");
    return Value;
}

/**
 * Reads the CR3 register and returns its value.
 *
 * @return The value stored in the CR3 register.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
HlReadCR3()
{
    ULONG_PTR Value;
    asm volatile("mov %%cr3, %0"
                 : "=r" (Value)
                 :
                 : "memory");
    return Value;
}

/**
 * Reads the CR4 register and returns its value.
 *
 * @return The value stored in the CR4 register.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
HlReadCR4()
{
    ULONG_PTR Value;
    asm volatile("mov %%cr4, %0"
                 : "=r" (Value)
                 :
                 : "memory");
    return Value;
}

/**
 * Reads the CR8 register and returns its value.
 *
 * @return The value stored in the CR8 register.
 *
 * @since XT 1.0
 */
XTAPI
ULONG_PTR
HlReadCR8()
{
    ULONG_PTR Value;
    asm volatile("mov %%cr8, %0"
                 : "=r" (Value)
                 :
                 : "memory");
    return Value;
}

/**
 * Writes the value to the CR0 register.
 *
 * @param Data
 *        The value to write to the CR0 register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlWriteCR0(UINT_PTR Data)
{
    asm volatile("mov %0, %%cr0"
                 :
                 : "r"(Data)
                 : "memory");
}

/**
 * Writes the value to the CR2 register.
 *
 * @param Data
 *        The value to write to the CR2 register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlWriteCR2(UINT_PTR Data)
{
    asm volatile("mov %0, %%cr2"
                 :
                 : "r"(Data)
                 : "memory");
}

/**
 * Writes the value to the CR3 register.
 *
 * @param Data
 *        The value to write to the CR3 register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlWriteCR3(UINT_PTR Data)
{
    asm volatile("mov %0, %%cr3"
                 :
                 : "r"(Data)
                 : "memory");
}

/**
 * Writes the value to the CR4 register.
 *
 * @param Data
 *        The value to write to the CR4 register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlWriteCR4(UINT_PTR Data)
{
    asm volatile("mov %0, %%cr4"
                 :
                 : "r"(Data)
                 : "memory");
}

/**
 * Writes the value to the CR8 register.
 *
 * @param Data
 *        The value to write to the CR8 register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
HlWriteCR8(UINT_PTR Data)
{
    asm volatile("mov %0, %%cr8"
                 :
                 : "r"(Data)
                 : "memory");
}
