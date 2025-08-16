/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/i686/globals.c
 * DESCRIPTION:     i686-specific global variables for the Memory Manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.h>


/* Page mapping routines for systems using 2-level paging (PML2) */
CMMPAGEMAP_ROUTINES MmpPml2Routines = {
    .ClearPte = MmpClearPte,
    .PteValid = MmpPml2PteValid,
    .SetPteCaching = MmpSetPml2PteCaching,
    .SetPte = MmpSetPml2Pte,
};

/* Page mapping routines for systems using 3-level paging (PML3) */
CMMPAGEMAP_ROUTINES MmpPml3Routines = {
    .ClearPte = MmpClearPte,
    .PteValid = MmpPml3PteValid,
    .SetPteCaching = MmpSetPml3PteCaching,
    .SetPte = MmpSetPml3Pte,
};
