/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/globals.c
 * DESCRIPTION:     Kernel runtime library global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/* This is required for floating numbers to keep LLVM happy */
int _fltused = 0xFEEDBULL;
