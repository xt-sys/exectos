/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/data.cc
 * DESCRIPTION:     I686 architecture-specific global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Initial kernel boot stack */
UCHAR AR::ProcessorSupport::BootStack[KERNEL_STACK_SIZE] = {};

/* Double Fault gate */
UCHAR AR::ProcessorSupport::DoubleFaultTss[KTSS_IO_MAPS];

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

/* NMI task gate */
UCHAR AR::ProcessorSupport::NonMaskableInterruptTss[KTSS_IO_MAPS];

/* Unhandled interrupt routine */
PINTERRUPT_HANDLER AR::Traps::UnhandledInterruptRoutine = NULLPTR;
