/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/dbg.hh
 * DESCRIPTION:     Kernel debugging support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_DBG_HH
#define __XTOSKRNL_DBG_HH

#include <xtos.hh>


/* Redefine DebugPrint macro for the kernel to enable early debugging */
#undef DebugPrint
#ifdef DBG
    #define DebugPrint(Format, ...)     if(KD::DebugIo::KdPrint) KD::DebugIo::KdPrint(Format, __VA_ARGS__);
#else
    #define DebugPrint(Format, ...)     ((VOID)NULL)
#endif

#endif /* __XTOSKRNL_DBG_HH */
