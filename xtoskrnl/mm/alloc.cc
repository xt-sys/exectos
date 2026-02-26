/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/mm/alloc.cc
 * DESCRIPTION:     Memory manager pool allocation
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Allocates pages from the non-paged pool.
 *
 * @param Pages
 *        Specifies the number of pages to allocate.
 *
 * @param Memory
 *        Supplies a pointer to the allocated pool.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::AllocateNonPagedPoolPages(IN PFN_COUNT Pages,
                                         OUT PVOID *Memory)
{
    UNIMPLEMENTED;

    /* Return not implemented status code */
    return STATUS_NOT_IMPLEMENTED;
}

/**
 * Allocates pages from the paged pool.
 *
 * @param Pages
 *        Specifies the number of pages to allocate.
 *
 * @param Memory
 *        Supplies a pointer to the allocated pool.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::AllocatePagedPoolPages(IN PFN_COUNT Pages,
                                      OUT PVOID *Memory)
{
    UNIMPLEMENTED;

    /* Return not implemented status code */
    return STATUS_NOT_IMPLEMENTED;
}

/**
 * Allocates pages from the specified pool type.
 *
 * @param PoolType
 *        Specifies the type of pool to allocate pages from.
 *
 * @param Bytes
 *        Specifies the number of bytes to allocate.
 *
 * @param Memory
 *        Supplies a pointer to the allocated pool.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
MM::Allocator::AllocatePages(IN MMPOOL_TYPE PoolType,
                             IN SIZE_T Bytes,
                             OUT PVOID *Memory)
{
    PFN_COUNT Pages;

    /* Initialize the output parameter */
    *Memory = NULLPTR;

    /* Convert bytes to pages */
    Pages = SIZE_TO_PAGES(Bytes);

    /* Check if there are any pages to allocate */
    if(!Pages)
    {
        /* Nothing to allocate, return NULLPTR */
        return STATUS_INVALID_PARAMETER;
    }

    /* Switch on pool type */
    switch(PoolType & MM_POOL_TYPE_MASK)
    {
        case NonPagedPool:
            /* Allocate non-paged pool */
            return AllocateNonPagedPoolPages(Pages, Memory);
        case PagedPool:
            /* Allocate paged pool */
            return AllocatePagedPoolPages(Pages, Memory);
    }

    /* Invalid pool type specified, return error */
    return STATUS_INVALID_PARAMETER;
}

/**
 * Initializes the non-paged pool for memory allocator.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Allocator::InitializeNonPagedPool(VOID)
{
	UNIMPLEMENTED;

    /* Map PTEs for the non-paged pool */
    MapNonPagedPool();
}

/**
 * Initializes the non-paged pool for memory allocator.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
MM::Allocator::InitializePagedPool(VOID)
{
    UNIMPLEMENTED;
}
