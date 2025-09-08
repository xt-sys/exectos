/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/data.cc
 * DESCRIPTION:     AMD64 architecture-specific global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Architecture-specific Library */
namespace AR
{

/* Initial kernel boot stack */
UCHAR ProcSup::BootStack[KERNEL_STACK_SIZE] = {};

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

} /* namespace */
