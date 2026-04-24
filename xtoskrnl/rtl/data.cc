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

/* Lookup table for days in a month, Index 0 is normal year, Index 1 is leap year */
CUSHORT RTL::Time::DaysInMonth[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};
