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
#include "hl.h"
#include "ke.h"
#include "mm.h"
#include "po.h"
#include "rtl.h"

#include ARCH_HEADER(globals.h)
#include ARCH_HEADER(ar.h)
#include ARCH_HEADER(ke.h)
#include ARCH_HEADER(mm.h)
#include ARCH_HEADER(rtl.h)
