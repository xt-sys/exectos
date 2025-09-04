/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/xtos.h
 * DESCRIPTION:     Top level header for the XT kernel
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

/* XT Development Kit */
#include <xtkmapi.h>

/* XT OS version */
#include <xtver.h>

/* Kernel specific headers */
#include "globals.h"
#include "hli.h"
#include "kdi.h"
#include "kei.h"
#include "mmi.h"
#include "poi.h"
#include "rtli.h"

#include ARCH_HEADER(globals.h)
#include ARCH_HEADER(ari.h)
#include ARCH_HEADER(hli.h)
#include ARCH_HEADER(kei.h)
#include ARCH_HEADER(mmi.h)
#include ARCH_HEADER(rtli.h)
