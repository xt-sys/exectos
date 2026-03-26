/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/exports.cc
 * DESCRIPTION:     C-compatible API wrappers for exported kernel functions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Allocates a block of memory from the specified pool type.
 *
 * @param PoolType
 *        Specifies the type of pool to allocate from.
 *
 * @param Bytes
 *        Specifies the number of bytes to allocate.
 *
 * @param Memory
 *        Supplies a pointer to the allocated memory.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MmAllocatePool(IN MMPOOL_TYPE PoolType,
               IN SIZE_T Bytes,
               OUT PVOID *Memory)
{
    return MM::Allocator::AllocatePool(PoolType, Bytes, Memory);
}

/**
 * Allocates a block of memory from the specified pool type.
 *
 * @param PoolType
 *        Specifies the type of pool to allocate from.
 *
 * @param Bytes
 *        Specifies the number of bytes to allocate.
 *
 * @param Memory
 *        Supplies a pointer to the allocated memory.
 *
 * @param Tag
 *        Specifies the allocation identifying tag.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MmAllocatePoolWithTag(IN MMPOOL_TYPE PoolType,
                      IN SIZE_T Bytes,
                      OUT PVOID *Memory,
                      IN ULONG Tag)
{
    return MM::Allocator::AllocatePool(PoolType, Bytes, Memory, Tag);
}

/**
 * Frees a previously allocated memory pool.
 *
 * @param VirtualAddress
 *        Supplies the base virtual address of the pool allocation to free.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MmFreePool(IN PVOID VirtualAddress)
{
    return MM::Allocator::FreePool(VirtualAddress);
}

/**
 * Frees a previously allocated memory pool.
 *
 * @param VirtualAddress
 *        Supplies the base virtual address of the pool allocation to free.
 *
 * @param Tag
 *        Specifies the allocation identifying tag.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MmFreePoolWithTag(IN PVOID VirtualAddress,
                  IN ULONG Tag)
{
    return MM::Allocator::FreePool(VirtualAddress, Tag);
}
