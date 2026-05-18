/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/data.cc
 * DESCRIPTION:     AMD64 architecture-specific global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Initial kernel boot stack */
UCHAR AR::ProcessorSupport::BootStack[KERNEL_STACK_SIZE] = {};

/* Initial kernel fault stack */
UCHAR AR::ProcessorSupport::FaultStack[KERNEL_STACK_SIZE] = {};

/* Initial GDT */
KGDTENTRY AR::ProcessorSupport::InitialGdt[GDT_ENTRIES] = {};

/* Initial IDT */
KIDTENTRY AR::ProcessorSupport::InitialIdt[IDT_ENTRIES] = {};

/* Initial Processor Block */
KPROCESSOR_BLOCK AR::ProcessorSupport::InitialProcessorBlock;

/* Initial TSS */
KTSS AR::ProcessorSupport::InitialTss;

/* Initial kernel NMI stack */
UCHAR AR::ProcessorSupport::NmiStack[KERNEL_STACK_SIZE] = {};

/* Unhandled interrupt routine */
PINTERRUPT_HANDLER AR::Traps::UnhandledInterruptRoutine = NULLPTR;
