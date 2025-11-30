/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm.hh
 * DESCRIPTION:     Memory Manager
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_HH
#define __XTOSKRNL_MM_HH

#include <xtos.hh>

#include XTOS_ARCH_HEADER(mm, pagemap.hh)
#include XTOS_ARCH_HEADER(mm, paging.hh)

#include <mm/hlpool.hh>
#include <mm/init.hh>
#include <mm/kpool.hh>

#endif /* __XTOSKRNL_MM_HH */
