/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtadk/i686/ke.cc
 * DESCRIPTION:     ADK generator for i686 version of Kernel Library
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtkmapi.h>
#include <adkdefs.h>


/**
 * Generates a definitions file for the Kernel Library used by the XTOS kernel assembly code
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTCLINK
XTAPI
VOID
GenerateAssemblyDefinitions(VOID)
{
    /* Generate KTRAP_FRAME offsets */
    ADK_OFFSET(KTRAP_FRAME, PreviousMode);
    ADK_OFFSET(KTRAP_FRAME, Cr2);
    ADK_OFFSET(KTRAP_FRAME, Cr3);
    ADK_OFFSET(KTRAP_FRAME, Dr0);
    ADK_OFFSET(KTRAP_FRAME, Dr1);
    ADK_OFFSET(KTRAP_FRAME, Dr2);
    ADK_OFFSET(KTRAP_FRAME, Dr3);
    ADK_OFFSET(KTRAP_FRAME, Dr6);
    ADK_OFFSET(KTRAP_FRAME, Dr7);
    ADK_OFFSET(KTRAP_FRAME, SegDs);
    ADK_OFFSET(KTRAP_FRAME, SegEs);
    ADK_OFFSET(KTRAP_FRAME, SegFs);
    ADK_OFFSET(KTRAP_FRAME, SegGs);
    ADK_OFFSET(KTRAP_FRAME, Eax);
    ADK_OFFSET(KTRAP_FRAME, Ebx);
    ADK_OFFSET(KTRAP_FRAME, Ecx);
    ADK_OFFSET(KTRAP_FRAME, Edx);
    ADK_OFFSET(KTRAP_FRAME, Esi);
    ADK_OFFSET(KTRAP_FRAME, Edi);
    ADK_OFFSET(KTRAP_FRAME, Ebp);
    ADK_OFFSET(KTRAP_FRAME, Vector);
    ADK_OFFSET(KTRAP_FRAME, ErrorCode);
    ADK_OFFSET(KTRAP_FRAME, Eip);
    ADK_OFFSET(KTRAP_FRAME, SegCs);
    ADK_OFFSET(KTRAP_FRAME, Flags);
    ADK_OFFSET(KTRAP_FRAME, Esp);
    ADK_OFFSET(KTRAP_FRAME, SegSs);

    /* Generate KTRAP_FRAME size and REGISTERS_SIZE */
    ADK_SIZE(KTRAP_FRAME);
    ADK_SIZE_FROM(REGISTERS_SIZE, KTRAP_FRAME, Rax);
}
