/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/data.cc
 * DESCRIPTION:     I686 architecture-specific global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Architecture-specific Library */
namespace AR
{

/* Initial kernel boot stack */
UCHAR ProcSup::BootStack[KERNEL_STACK_SIZE] = {};

/* Double Fault gate */
UCHAR ProcSup::DoubleFaultTss[KTSS_IO_MAPS];

/* Initial kernel fault stack */
UCHAR ProcSup::FaultStack[KERNEL_STACK_SIZE] = {};

/* Initial GDT */
KGDTENTRY ProcSup::InitialGdt[GDT_ENTRIES] = {};

/* Initial IDT */
KIDTENTRY ProcSup::InitialIdt[IDT_ENTRIES] = {};

/* Initial Processor Block */
KPROCESSOR_BLOCK ProcSup::InitialProcessorBlock;

/* Initial TSS */
KTSS ProcSup::InitialTss;

/* NMI task gate */
UCHAR ProcSup::NonMaskableInterruptTss[KTSS_IO_MAPS];

} /* namespace */
