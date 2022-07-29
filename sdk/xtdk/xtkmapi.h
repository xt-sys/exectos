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
