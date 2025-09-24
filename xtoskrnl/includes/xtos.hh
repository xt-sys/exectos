/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/xtos.hh
 * DESCRIPTION:     Top level header for the XT kernel
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */


/* Preprocessor macro for including arch-specific headers */
#define XTOS_ARCH_HEADER(subsystem, header) STRINGIFY(subsystem/_ARCH/header)

/* XT Kernel Mode Development Kit */
#include <xtkmapi.h>

/* XT OS version */
#include <xtver.h>

/* Kernel specific headers */
#include <ar.hh>
#include <ex.hh>
#include <hl.hh>
#include <kd.hh>
#include <ke.hh>
#include <mm.hh>
#include <po.hh>
#include <rtl.hh>
