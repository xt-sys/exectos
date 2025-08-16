/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/amd64/globals.c
 * DESCRIPTION:     AMD64-specific global variables for the Memory Manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


/* Page mapping routines for systems using 4-level paging (PML4) */
CMMPAGEMAP_ROUTINES MmpPml4Routines = {
    // .ClearPte = MmpClearPte,
    // .PteValid = MmpPml2PteValid,
    // .SetPteCaching = MmpSetPml2PteCaching,
    // .SetPte = MmpSetPml2Pte,
};

/* Page mapping routines for systems using 5-level paging (PML5) */
CMMPAGEMAP_ROUTINES MmpPml5Routines = {
    // .ClearPte = MmpClearPte,
    // .PteValid = MmpPml3PteValid,
    // .SetPteCaching = MmpSetPml3PteCaching,
    // .SetPte = MmpSetPml3Pte,
};
