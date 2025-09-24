/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/data.cc
 * DESCRIPTION:     AMD64 architecture-specific global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Initial kernel boot stack */
UCHAR AR::ProcSup::BootStack[KERNEL_STACK_SIZE] = {};

/* Initial kernel fault stack */
UCHAR AR::ProcSup::FaultStack[KERNEL_STACK_SIZE] = {};

/* Initial GDT */
KGDTENTRY AR::ProcSup::InitialGdt[GDT_ENTRIES] = {};

/* Initial IDT */
KIDTENTRY AR::ProcSup::InitialIdt[IDT_ENTRIES] = {};

/* Initial Processor Block */
KPROCESSOR_BLOCK AR::ProcSup::InitialProcessorBlock;

/* Initial TSS */
KTSS AR::ProcSup::InitialTss;
