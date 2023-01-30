/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/globals.c
 * DESCRIPTION:     XT architecture library global variables
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include "xtos.h"


/* Initial GDT */
KGDTENTRY ArInitialGdt[GDT_ENTRIES] = {0};

/* Initial IDT */
KIDTENTRY ArInitialIdt[IDT_ENTRIES] = {0};

/* Initial Processor Block */
KPROCESSOR_BLOCK ArInitialProcessorBlock;

/* Initial TSS */
KTSS ArInitialTss;
