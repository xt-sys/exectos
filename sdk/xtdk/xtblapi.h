/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtblapi.h
 * DESCRIPTION:     Top level header for the XT Boot Loader API
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

/* Base XT headers */
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
#include <xtfw.h>
#include <xtimage.h>
#include <xtuefi.h>
#include <xtdebug.h>
#include <xtguid.h>

/* Architecture independent XT kernel data types */
#include <hltypes.h>
#include <iotypes.h>
#include <rtltypes.h>

/* Architecture dependent XT kernel data types */
#include ARCH_HEADER(artypes.h)
#include ARCH_HEADER(hltypes.h)
#include ARCH_HEADER(ketypes.h)
#include ARCH_HEADER(mmtypes.h)

/* XT Kernel runtime routines */
#include <hlfuncs.h>
#include <rtlfuncs.h>

/* Architecture specific XT kernel routines */
#include ARCH_HEADER(arfuncs.h)
#include ARCH_HEADER(hlfuncs.h)

/* Boot Manager specific structures */
#include <bltarget.h>
#include <bltypes.h>
#include <blfuncs.h>
