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

/* Lookup table for days preceding a month, Index 0 is normal year, Index 1 is leap year */
CUSHORT RTL::Time::DaysPrecedingMonth[2][12] = {
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}
};
