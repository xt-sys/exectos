/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/amd64/procsup.c
 * DESCRIPTION:     AMD64 processor functionality support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include "xtos.h"


/* Initial GDT */
KGDTENTRY ArInitialGdt[GDT_ENTRIES] = {0};

/* Initial IDT */
KIDTENTRY ArInitialIdt[IDT_ENTRIES] = {0};

/* Initial TSS */
KTSS ArInitialTss;
