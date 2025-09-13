/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtkmapi.h
 * DESCRIPTION:     Top level header for the kernel-mode XT API
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

/* Base XT headers */
#include <xtcompat.h>
#include <xtdefs.h>
#include <xtstatus.h>
#include <xttarget.h>
#include <xttypes.h>

/* XT forward references */
#include <xtstruct.h>

/* Architecture-specific XT forward references */
#include ARCH_HEADER(xtstruct.h)

/* Architecture-independent XT API */
#include <xtbase.h>
#include <xtdebug.h>
#include <xtguid.h>
#include <xtfw.h>
#include <xtimage.h>
#include <xtuefi.h>

/* Low level data types headers */
#include <extypes.h>
#include <hltypes.h>
#include <iotypes.h>
#include <kdtypes.h>
#include <ketypes.h>
#include <ldrtypes.h>
#include <mmtypes.h>
#include <potypes.h>
#include <pstypes.h>
#include <rtltypes.h>

/* Architecture-specific low level data types headers */
#include ARCH_HEADER(artypes.h)
#include ARCH_HEADER(hltypes.h)
#include ARCH_HEADER(ketypes.h)
#include ARCH_HEADER(mmtypes.h)

/* XT routines */
#include <exfuncs.h>
#include <hlfuncs.h>
#include <kdfuncs.h>
#include <kefuncs.h>
#include <rtlfuncs.h>

/* Architecture specific XT routines */
#include ARCH_HEADER(arfuncs.h)
#include ARCH_HEADER(hlfuncs.h)
