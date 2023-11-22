/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/panic.c
 * DESCRIPTION:     System shutdown and kernel panic mechanism
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Halts the system.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KeHaltSystem()
{
    /* Enter infinite loop */
    for(;;)
    {
        /* Halt system */
        ArClearInterruptFlag();
        ArHalt();
    }

}

/**
 * Crashes the system upon detecting a fatal error in which either it is unable to recover or continue to run system.
 *
 * @param Code
 *        Specifies the reason for the kernel panic.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KePanic(IN ULONG Code)
{
    KePanicEx(Code, 0, 0, 0, 0);
}

/**
 * Crashes the system upon detecting a fatal error in which either it is unable to recover or continue to run system.
 *
 * @param Code
 *        Specifies the reason for the kernel panic.
 *
 * @param Parameter1
 *        Supplies additional information about the kernel panic.
 *
 * @param Parameter2
 *        Supplies additional information about the kernel panic.
 *
 * @param Parameter3
 *        Supplies additional information about the kernel panic.
 *
 * @param Parameter4
 *        Supplies additional information about the kernel panic.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
KePanicEx(IN ULONG Code,
          IN ULONG_PTR Parameter1,
          IN ULONG_PTR Parameter2,
          IN ULONG_PTR Parameter3,
          IN ULONG_PTR Parameter4)
{
    KeDbgPrint(L"Fatal System Error: 0x%08lx\nKernel Panic!\n\n", Code);
    for(;;)
    {
        ArClearInterruptFlag();
        ArHalt();
    }
}
