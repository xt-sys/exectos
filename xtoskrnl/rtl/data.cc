/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/data.cc
 * DESCRIPTION:     Runtime Library global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* This is required for floating numbers to keep LLVM happy */
XTCLINK INT _fltused = 0xFEEDBULL;
