/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtkmapi.h
 * DESCRIPTION:     Top level header for the kernel-mode XT API
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

/* Base XT headers */
#include "xttarget.h"
#include "xttypes.h"
#include "xtdefs.h"
#include "xtstatus.h"

/* XT forward references */
#include "xtstruct.h"

/* Architecture-specific XT forward references */
#include ARCH_HEADER(xtstruct.h)

/* Architecture-independent XT API */
#include "xtcommon.h"
#include "xtimage.h"
#include "xtuefi.h"
#include "xtfw.h"

/* Low level data types headers */
#include "rtltypes.h"
#include "ldrtypes.h"
#include "hltypes.h"

/* Architecture-specific low level data types headers */
#include ARCH_HEADER(hltypes.h)
#include ARCH_HEADER(ketypes.h)
#include ARCH_HEADER(mmtypes.h)

/* XT routines */
#include "hlfuncs.h"
#include "kefuncs.h"
#include "rtlfuncs.h"

/* Architecture specific XT routines*/
#include ARCH_HEADER(hlfuncs.h)
