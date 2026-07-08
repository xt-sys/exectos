/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/mm/tags.hh
 * DESCRIPTION:     Memory Manager allocation tags
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_MM_TAGS_HH
#define __XTOSKRNL_MM_TAGS_HH

#include <xtos.hh>


/* Allocation tags */
#define TAG_HL_FRAMEBUFFER                                      SIGNATURE32('F', 'B', 'U', 'F')
#define TAG_MM_BIG_ALLOC                                        SIGNATURE32('B', 'I', 'G', 'A')
#define TAG_MM_MEMORY_MGR                                       SIGNATURE32('M', 'M', 'G', 'R')
#define TAG_MM_OVERFLOW                                         SIGNATURE32('O', 'V', 'F', 'L')
#define TAG_MM_MEMORY_POOL                                      SIGNATURE32('P', 'O', 'O', 'L')
#define TAG_MM_NONE                                             SIGNATURE32('N', 'O', 'N', 'E')
#define TAG_PS_QUOTA_BLOCK                                      SIGNATURE32('P', 'S', 'Q', 'B')
#define TAG_SE_DESCRIPTOR                                       SIGNATURE32('S', 'E', 'S', 'D')

#endif /* __XTOSKRNL_MM_TAGS_HH */
