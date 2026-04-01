/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/mmfuncs.h
 * DESCRIPTION:     XTOS memory manager routine definitions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTDK_MMFUNCS_H
#define __XTDK_MMFUNCS_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>


/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* Memory manager routines forward references */
XTAPI
XTSTATUS
MmAllocatePool(IN MMPOOL_TYPE PoolType,
               IN SIZE_T Bytes,
               OUT PVOID *Memory);

XTAPI
XTSTATUS
MmAllocatePoolWithTag(IN MMPOOL_TYPE PoolType,
                      IN SIZE_T Bytes,
                      OUT PVOID *Memory,
                      IN ULONG Tag);

XTAPI
XTSTATUS
MmFreePool(IN PVOID VirtualAddress);

XTAPI
XTSTATUS
MmFreePoolWithTag(IN PVOID VirtualAddress,
                  IN ULONG Tag);

#endif /* __XTOS_ASSEMBLER__ */
#endif /* __XTDK_MMFUNCS_H */
