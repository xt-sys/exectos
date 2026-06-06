/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/kd/debug.cc
 * DESCRIPTION:     Kernel Debugger
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Determines whether the interactive kernel debugger is currently active and controlling the execution flow.
 *
 * @return This routine returns TRUE if the kernel debugger is active, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTAPI
BOOLEAN
KD::Debugger::DebuggerActive(VOID)
{
    return Active;
}
