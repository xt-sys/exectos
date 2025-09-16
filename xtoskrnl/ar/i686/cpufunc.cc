/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/cpufunc.cc
 * DESCRIPTION:     Routines to provide access to special i686 CPU instructions
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Instructs the processor to clear the interrupt flag.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::ClearInterruptFlag(VOID)
{
    __asm__ volatile("cli");
}

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
XTCDECL
BOOLEAN
AR::CpuFunc::CpuId(IN OUT PCPUID_REGISTERS Registers)
{
    UINT32 MaxLeaf;

    /* Get highest function ID available */
    __asm__ volatile("cpuid"
                     : "=a" (MaxLeaf)
                     : "a" (Registers->Leaf & 0x80000000)
                     : "rbx",
                       "rcx",
                       "rdx");

    /* Check if CPU supports this command */
    if(Registers->Leaf > MaxLeaf)
    {
        /* Cannot call it, return FALSE */
        return FALSE;
    }

    /* Execute CPUID function */
    __asm__ volatile("cpuid"
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
 * Partially flushes the Translation Lookaside Buffer (TLB)
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::FlushTlb(VOID)
{
    /* Flush the TLB by resetting the CR3 */
    ArWriteControlRegister(3, ArReadControlRegister(3));
}

/**
 * Gets the EFLAGS register.
 *
 * @return This routine returns the EFLAGS register.
 *
 * @since XT 1.0
 */
XTCDECL
ULONG
AR::CpuFunc::GetCpuFlags(VOID)
{
    ULONG_PTR Flags;

    /* Get EFLAGS register */
    __asm__ volatile("pushf\n"
                     "pop %0\n"
                     : "=rm" (Flags)
                     :
                     : "memory");

    /* Return flags */
    return Flags;
}

/**
 * Gets the address of the current stack register.
 *
 * @return This routine returns the current stack pointer.
 *
 * @since XT 1.0
 */
XTASSEMBLY
XTCDECL
ULONG_PTR
AR::CpuFunc::GetStackPointer(VOID)
{
    /* Get current stack pointer */
    __asm__ volatile("mov %%esp, %%eax\n"
                     "ret\n"
                      :
                      :
                      :);
}

/**
 * Halts the central processing unit (CPU).
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::Halt(VOID)
{
    __asm__ volatile("hlt");
}

/**
 * Checks whether interrupts are enabled or not.
 *
 * @return This routine returns TRUE if interrupts are enabled, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTCDECL
BOOLEAN
AR::CpuFunc::InterruptsEnabled(VOID)
{
    ULONG_PTR Flags;

    /* Get RFLAGS register */
    Flags = GetCpuFlags();

    /* Check if interrupts are enabled and return result */
    return (Flags & X86_EFLAGS_IF_MASK) ? TRUE : FALSE;
}

/**
 * Invalidates the TLB (Translation Lookaside Buffer) for specified virtual address.
 *
 * @param Address
 *        Suuplies a virtual address whose associated TLB entry will be invalidated.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::InvalidateTlbEntry(PVOID Address)
{
    __asm__ volatile("invlpg (%0)"
                     :
                     : "b" (Address)
                     : "memory");
}

/**
 * Loads the value in the source operand into the global descriptor table register (GDTR).
 *
 * @param Source
 *        Specifies a memory location that contains the base address of GDT.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::LoadGlobalDescriptorTable(IN PVOID Source)
{
    __asm__ volatile("lgdt %0"
                     :
                     : "m" (*(PSHORT)Source)
                     : "memory");
}

/**
 * Loads the value in the source operand into the interrupt descriptor table register (IDTR).
 *
 * @param Source
 *        Specifies a memory location that contains the base address of IDT.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::LoadInterruptDescriptorTable(IN PVOID Source)
{
    __asm__ volatile("lidt %0"
                     :
                     : "m" (*(PSHORT)Source)
                     : "memory");
}

/**
 * Loads the value in the source operand into the local descriptor table register (LDTR).
 *
 * @param Source
 *        Specifies a selector value.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::LoadLocalDescriptorTable(IN USHORT Source)
{
    __asm__ volatile("lldtw %0"
                     :
                     : "g" (Source));
}

/**
 * Loads source data into specified segment.
 *
 * @param Segment
 *        Supplies a segment identification.
 *
 * @param Source
 *        Supplies a pointer to the memory area containing data that will be loaded into specified segment.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::LoadSegment(IN USHORT Segment,
                     IN ULONG Source)
{
    switch(Segment)
    {
        case SEGMENT_CS:
            /* Load CS Segment */
            __asm__ volatile("mov %0, %%eax\n"
                             "push %%eax\n"
                             "lea label, %%eax\n"
                             "push %%eax\n"
                             "lret\n"
                             "label:"
                             :
                             : "ri" (Source)
                             : "eax");
            break;
        case SEGMENT_DS:
            /* Load DS Segment */
            __asm__ volatile("movl %0, %%ds"
                             :
                             : "r" (Source));
            break;
        case SEGMENT_ES:
            /* Load ES Segment */
            __asm__ volatile("movl %0, %%es"
                             :
                             : "r" (Source));
            break;
        case SEGMENT_FS:
            /* Load FS Segment */
            __asm__ volatile("movl %0, %%fs"
                             :
                             : "r" (Source));
            break;
        case SEGMENT_GS:
            /* Load GS Segment */
            __asm__ volatile("movl %0, %%gs"
                             :
                             : "r" (Source));
            break;
        case SEGMENT_SS:
            /* Load SS Segment */
            __asm__ volatile("movl %0, %%ss"
                             :
                             : "r" (Source));
            break;
    }
}

/**
 * Loads Task Register (TR) with a segment selector that points to TSS.
 *
 * @param Source
 *        Supplies the segment selector in the GDT describing the TSS.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::LoadTaskRegister(USHORT Source)
{
    __asm__ volatile("ltr %0"
                     :
                     : "rm" (Source));
}

/**
 * Orders memory accesses as seen by other processors.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::MemoryBarrier(VOID)
{
   LONG Barrier;
   __asm__ volatile("xchg %%eax, %0"
                    :
                    : "m" (Barrier)
                    : "%eax");
}

/**
 * Reads the specified CPU control register and returns its value.
 *
 * @param ControlRegister
 *        Supplies a number of a control register which controls the general behavior of a CPU.
 *
 * @return The value stored in the control register.
 *
 * @since XT 1.0
 */
XTCDECL
ULONG_PTR
AR::CpuFunc::ReadControlRegister(IN USHORT ControlRegister)
{
    ULONG_PTR Value;

    /* Read a value from specified CR register */
    switch(ControlRegister)
    {
        case 0:
            /* Read value from CR0 */
            __asm__ volatile("mov %%cr0, %0"
                             : "=r" (Value)
                             :
                             : "memory");
            break;
        case 2:
            /* Read value from CR2 */
            __asm__ volatile("mov %%cr2, %0"
                             : "=r" (Value)
                             :
                             : "memory");
            break;
        case 3:
            /* Read value from CR3 */
            __asm__ volatile("mov %%cr3, %0"
                             : "=r" (Value)
                             :
                             : "memory");
            break;
        case 4:
            /* Read value from CR4 */
            __asm__ volatile("mov %%cr4, %0"
                             : "=r" (Value)
                             :
                             : "memory");
            break;
        default:
            /* Invalid control register set */
            Value = 0;
            break;
    }

    /* Return value read from given CR register */
    return Value;
}

/**
 * Reads the specified CPU debug register and returns its value.
 *
 * @param DebugRegister
 *        Supplies a number of a debug register to read from.
 *
 * @return The value stored in the specified debug register.
 *
 * @since XT 1.0
 */
XTCDECL
ULONG_PTR
AR::CpuFunc::ReadDebugRegister(IN USHORT DebugRegister)
{
    ULONG_PTR Value;

    /* Read a value from specified DR register */
    switch(DebugRegister)
    {
        case 0:
            /* Read value from DR0 */
            __asm__ volatile("mov %%dr0, %0"
                             : "=r" (Value));
            break;
        case 1:
            /* Read value from DR1 */
            __asm__ volatile("mov %%dr1, %0"
                             : "=r" (Value));
            break;
        case 2:
            /* Read value from DR2 */
            __asm__ volatile("mov %%dr2, %0"
                             : "=r" (Value));
            break;
        case 3:
            /* Read value from DR3 */
            __asm__ volatile("mov %%dr3, %0"
                             : "=r" (Value));
            break;
        case 4:
            /* Read value from DR4 */
            __asm__ volatile("mov %%dr4, %0"
                             : "=r" (Value));
            break;
        case 5:
            /* Read value from DR5 */
            __asm__ volatile("mov %%dr5, %0"
                             : "=r" (Value));
            break;
        case 6:
            /* Read value from DR6 */
            __asm__ volatile("mov %%dr6, %0"
                             : "=r" (Value));
            break;
        case 7:
            /* Read value from DR7 */
            __asm__ volatile("mov %%dr7, %0"
                             : "=r" (Value));
            break;
        default:
            /* Invalid debug register set */
            Value = 0;
            break;
    }

    /* Return value read from given DR register */
    return Value;
}

/**
 * Reads dualword from a memory location specified by an offset relative to the beginning of the FS segment.
 *
 * @param Offset
 *        Specifies the offset from the beginning of FS segment.
 *
 * @return Returns the value read from the specified memory location relative to FS segment.
 *
 * @since XT 1.0
 */
XTCDECL
ULONG
AR::CpuFunc::ReadFSDualWord(ULONG Offset)
{
    ULONG Value;
    __asm__ volatile("movl %%fs:%a[Offset], %k[Value]"
                     : [Value] "=r" (Value)
                     : [Offset] "ir" (Offset));
    return Value;
}

/**
 * Reads a 64-bit value from the requested Model Specific Register (MSR).
 *
 * @param Register
 *        Supplies the MSR to read.
 *
 * @return This routine returns the 64-bit MSR value.
 *
 * @since XT 1.0
 */
XTCDECL
ULONGLONG
AR::CpuFunc::ReadModelSpecificRegister(IN ULONG Register)
{
    ULONGLONG Value;

    __asm__ volatile("rdmsr"
                     : "=A" (Value)
                     : "c" (Register));
    return Value;
}

/**
 * Reads the contents of the MXCSR control/status register.
 *
 * @return This routine returns the contents of the MXCSR register as a 32-bit unsigned integer value.
 *
 * @since XT 1.0
 */
XTCDECL
UINT
AR::CpuFunc::ReadMxCsrRegister(VOID)
{
    return __builtin_ia32_stmxcsr();
}

/**
 * Reads the current value of the CPU's time-stamp counter.
 *
 * @return This routine returns the current instruction cycle count since the processor was started.
 *
 * @since XT 1.0
 */
XTCDECL
ULONGLONG
AR::CpuFunc::ReadTimeStampCounter(VOID)
{
    ULONGLONG Value;

    __asm__ volatile("rdtsc"
                     : "=A" (Value));

    return Value;
}

/**
 * Orders memory accesses as seen by other processors, without fence.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::ReadWriteBarrier(VOID)
{
    __asm__ volatile(""
                     :
                     :
                     : "memory");
}

/**
 * Instructs the processor to set the interrupt flag.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::SetInterruptFlag(VOID)
{
    __asm__ volatile("sti");
}

/**
 * Stores GDT register into the given memory area.
 *
 * @param Destination
 *        Supplies a pointer to the memory area where GDT will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::StoreGlobalDescriptorTable(OUT PVOID Destination)
{
    __asm__ volatile("sgdt %0"
                     : "=m" (*(PSHORT)Destination)
                     :
                     : "memory");
}

/**
 * Stores IDT register into the given memory area.
 *
 * @param Destination
 *        Supplies a pointer to the memory area where IDT will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::StoreInterruptDescriptorTable(OUT PVOID Destination)
{
    __asm__ volatile("sidt %0"
                     : "=m" (*(PSHORT)Destination)
                     :
                     : "memory");
}

/**
 * Stores LDT register into the given memory area.
 *
 * @param Destination
 *        Supplies a pointer to the memory area where LDT will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::StoreLocalDescriptorTable(OUT PVOID Destination)
{
    __asm__ volatile("sldt %0"
                     : "=m" (*(PSHORT)Destination)
                     :
                     : "memory");
}

/**
 * Stores specified segment into the given memory area.
 *
 * @param Segment
 *        Supplies a segment identification.
 *
 * @param Destination
 *        Supplies a pointer to the memory area where segment data will be stored.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::StoreSegment(IN USHORT Segment,
                      OUT PVOID Destination)
{
    switch(Segment)
    {
        case SEGMENT_CS:
            __asm__ volatile("movl %%cs, %0"
                             : "=r" (*(PUINT)Destination));
            break;
        case SEGMENT_DS:
            __asm__ volatile("movl %%ds, %0"
                             : "=r" (*(PUINT)Destination));
            break;
        case SEGMENT_ES:
            __asm__ volatile("movl %%es, %0"
                             : "=r" (*(PUINT)Destination));
            break;
        case SEGMENT_FS:
            __asm__ volatile("movl %%fs, %0"
                             : "=r" (*(PUINT)Destination));
            break;
        case SEGMENT_GS:
            __asm__ volatile("movl %%gs, %0"
                             : "=r" (*(PUINT)Destination));
            break;
        case SEGMENT_SS:
            __asm__ volatile("movl %%ss, %0"
                             : "=r" (*(PUINT)Destination));
            break;
        default:
            Destination = NULL;
            break;
    }
}

/**
 * Stores TR into the given memory area.
 *
 * @param Destination
 *        Supplies a pointer to the memory area where TR will be stores.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::StoreTaskRegister(OUT PVOID Destination)
{
    __asm__ volatile("str %0"
                     : "=m" (*(PULONG)Destination)
                     :
                     : "memory");
}

/**
 * Writes a value to the specified CPU control register.
 *
 * @param ControlRegister
 *        Supplies a number of a control register which controls the general behavior of a CPU.
 *
 * @param Value
 *        Suplies a value to write to the CR register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::WriteControlRegister(IN USHORT ControlRegister,
                              IN UINT_PTR Value)
{
    /* Write a value into specified control register */
    switch(ControlRegister)
    {
        case 0:
            /* Write value to CR0 */
            __asm__ volatile("mov %0, %%cr0"
                             :
                             : "r" (Value)
                             : "memory");
            break;
        case 2:
            /* Write value to CR2 */
            __asm__ volatile("mov %0, %%cr2"
                             :
                             : "r" (Value)
                             : "memory");
            break;
        case 3:
            /* Write value to CR3 */
            __asm__ volatile("mov %0, %%cr3"
                             :
                             : "r" (Value)
                             : "memory");
            break;
        case 4:
            /* Write value to CR4 */
            __asm__ volatile("mov %0, %%cr4"
                             :
                             : "r" (Value)
                             : "memory");
            break;
    }
}

/**
 * Writes a value to the specified CPU debug register.
 *
 * @param DebugRegister
 *        Supplies a number of a debug register for write operation.
 *
 * @param Value
 *        Suplies a value to write to the specified DR register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::WriteDebugRegister(IN USHORT DebugRegister,
                            IN UINT_PTR Value)
{
    /* Write a value into specified debug register */
    switch(DebugRegister)
    {
        case 0:
            /* Write value to DR0 */
            __asm__ volatile("mov %0, %%dr0"
                             :
                             : "r" (Value)
                             : "memory");
        case 1:
            /* Write value to DR1 */
            __asm__ volatile("mov %0, %%dr1"
                             :
                             : "r" (Value)
                             : "memory");
        case 2:
            /* Write value to DR2 */
            __asm__ volatile("mov %0, %%dr2"
                             :
                             : "r" (Value)
                             : "memory");
        case 3:
            /* Write value to DR3 */
            __asm__ volatile("mov %0, %%dr3"
                             :
                             : "r" (Value)
                             : "memory");
        case 4:
            /* Write value to DR4 */
            __asm__ volatile("mov %0, %%dr4"
                             :
                             : "r" (Value)
                             : "memory");
        case 5:
            /* Write value to DR5 */
            __asm__ volatile("mov %0, %%dr5"
                             :
                             : "r" (Value)
                             : "memory");
        case 6:
            /* Write value to DR6 */
            __asm__ volatile("mov %0, %%dr6"
                             :
                             : "r" (Value)
                             : "memory");
        case 7:
            /* Write value to DR7 */
            __asm__ volatile("mov %0, %%dr7"
                             :
                             : "r" (Value)
                             : "memory");
    }
}

/**
 * Writes the specified value to the program status and control (EFLAGS) register.
 *
 * @param Value
 *        The value to write to the EFLAGS register.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::WriteEflagsRegister(IN UINT_PTR Value)
{
    __asm__ volatile("push %0\n"
                     "popf"
                     :
                     : "rim" (Value));
}

/**
 * Writes a 64-bit value to the requested Model Specific Register (MSR).
 *
 * @param Register
 *        Supplies the MSR register to write.
 *
 * @param Value
 *        Supplies the 64-bit value to write.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::WriteModelSpecificRegister(IN ULONG Register,
                                    IN ULONGLONG Value)
{
    __asm__ volatile("wrmsr"
                     :
                     : "c" (Register),
                       "A" (Value));
}

/**
 * Yields a current thread running on the processor.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCDECL
VOID
AR::CpuFunc::YieldProcessor(VOID)
{
    __asm__ volatile("pause"
                     :
                     :
                     : "memory");
}



/* NEEDED BY XTLDR */
XTCLINK
XTCDECL
VOID
ArClearInterruptFlag(VOID)
{
    AR::CpuFunc::ClearInterruptFlag();
}

/* NEEDED BY XTLDR */
XTCLINK
XTCDECL
BOOLEAN
ArCpuId(IN OUT PCPUID_REGISTERS Registers)
{
    return AR::CpuFunc::CpuId(Registers);
}

/* NEEDED BY XTLDR */
XTCLINK
XTCDECL
VOID
ArHalt(VOID)
{
    AR::CpuFunc::Halt();
}

/* NEEDED BY XTLDR */
XTCLINK
XTCDECL
ULONG_PTR
ArReadControlRegister(IN USHORT ControlRegister)
{
    return AR::CpuFunc::ReadControlRegister(ControlRegister);
}

/* NEEDED BY XTLDR */
XTCLINK
XTCDECL
ULONGLONG
ArReadModelSpecificRegister(IN ULONG Register)
{
    return AR::CpuFunc::ReadModelSpecificRegister(Register);
}

/* NEEDED BY XTLDR */
XTCLINK
XTCDECL
VOID
ArWriteControlRegister(IN USHORT ControlRegister,
                       IN UINT_PTR Value)
{
    AR::CpuFunc::WriteControlRegister(ControlRegister, Value);
}
