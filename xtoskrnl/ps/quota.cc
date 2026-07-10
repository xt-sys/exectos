/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/po/quota.cc
 * DESCRIPTION:     Process Quota Management
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/**
 * Decrements the reference count of a quota block and destroys it if it reaches zero.
 *
 * @param QuotaBlock
 *        Supplies a pointer to the process quota block to dereference.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
PS::Quota::DereferenceQuotaBlock(IN PEPROCESS_QUOTA_BLOCK QuotaBlock)
{
    SIZE_T ReturnAmount;
    ULONG Index;

    /* Guard against null pointer and the bypass marker */
    if(QuotaBlock == NULLPTR || QuotaBlock == PS_QUOTA_BYPASS_MARKER)
    {
        /* No cleanup is required, return */
        return;
    }

    /* Decrement the reference count */
    if(RTL::Atomic::Decrement32((VOLATILE PLONG)&QuotaBlock->ReferenceCount) == 0)
    {
        /* Start a guarded code block */
        {
            /* Raise runlevel to DISPATCH level */
            KE::RaiseRunLevel RunLevel(DISPATCH_LEVEL);

            /* Iterate through all available pool quota types */
            for(Index = PsNonPagedPool; Index <= PsPagedPool; Index++)
            {
                /* Calculate the total residual quota */
                ReturnAmount = QuotaBlock->QuotaEntry[Index].Return + QuotaBlock->QuotaEntry[Index].Limit;

                /* Check if there is any residual quota left */
                if(ReturnAmount > 0)
                {
                    /* Return the residual quota */
                    MM::Quota::ReturnPoolQuota((MMPOOL_TYPE)Index, ReturnAmount);
                }
            }

            /* Acquire a quota lock */
            KE::SpinLockGuard SpinLock(&QuotaLock);

            /* Unlink the quota block from the global tracking list */
            RTL::LinkedList::RemoveEntryList(&QuotaBlock->QuotaList);
        }

        /* Free the physical memory */
        MM::Allocator::FreePool(QuotaBlock, TAG_PS_QUOTA_BLOCK);
    }
}

/**
 * Returns a specified amount of quota to a quota block and updates process usage.
 *
 * @param QuotaBlock
 *        Supplies the quota block receiving the returned quota.
 *
 * @param Process
 *        Supplies an optional pointer to the process whose usage is being reduced.
 *
 * @param QuotaType
 *        Supplies the type of quota being returned.
 *
 * @param Amount
 *        Supplies the amount of quota to return.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
PS::Quota::ReturnProcessQuota(IN PEPROCESS_QUOTA_BLOCK QuotaBlock,
                              IN PEPROCESS Process,
                              IN PS_QUOTA_TYPE QuotaType,
                              IN SIZE_T Amount)
{
    SIZE_T AccumulatedGiveBack, ExtractedReturnAmount, GiveBackAmount, GiveBackLimit, MinGiveBackThreshold;
    SIZE_T CurrentLimit, CurrentUsage, ExpectedLimit, NewLimit;
    PEPROCESS_QUOTA_ENTRY QuotaEntry;

    /* Initialize local pointers and fetch the current usage for the specified quota type */
    QuotaEntry = &QuotaBlock->QuotaEntry[QuotaType];
    CurrentUsage = QuotaEntry->Usage;

    /* Check if process provided */
    if(Process != NULLPTR)
    {
        /* Check if this is the system process */
        if(Process == PS::Process::GetSystemProcess())
        {
            /* Do not do anything for system processes, return */
            return;
        }
        else if(Process->QuotaUsage[QuotaType] < Amount || CurrentUsage < Amount)
        {
            /* Quota underflow detected, raise kernel panic */
            KE::Crash::Panic(0x21,
                             (ULONG_PTR)Process,
                             (ULONG_PTR)QuotaType,
                             Process ? (ULONG_PTR)Process->QuotaUsage[QuotaType] : (ULONG_PTR)CurrentUsage,
                             (ULONG_PTR)Amount);
        }
    }

    /* Determine the thresholds for returning excess quota limit */
    CurrentLimit = QuotaEntry->Limit;
    MinGiveBackThreshold = (MMPAGED_QUOTA_INCREASE > MMNONPAGED_QUOTA_INCREASE) ? MMNONPAGED_QUOTA_INCREASE
                                                                                : MMPAGED_QUOTA_INCREASE;

    /* Check if the current limit exceeds usage by at least the minimum giveback threshold */
    if((CurrentLimit > CurrentUsage) && ((CurrentLimit - CurrentUsage) > MinGiveBackThreshold))
    {
        /* Only proceed if this is a pool quota */
        if(QuotaType != PsPageFile && QuotaBlock != &DefaultQuotaBlock)
        {
            /* Calculate the exact amount of limit to trim based on pool type block size */
            GiveBackLimit = (QuotaType == PsPagedPool) ? MMPAGED_QUOTA_INCREASE : MMNONPAGED_QUOTA_INCREASE;
            GiveBackAmount = (GiveBackLimit > Amount) ? Amount : GiveBackLimit;

            /* Attempt a lock-free update to shrink the quota limit */
            NewLimit = CurrentLimit - GiveBackAmount;
            ExpectedLimit = (SIZE_T)RTL::Atomic::CompareExchange64((PLONG_PTR)&QuotaEntry->Limit, (LONG_PTR)NewLimit,
                                                                   (LONG_PTR)CurrentLimit);

            /* Check if the atomic limit update succeeded */
            if(ExpectedLimit == CurrentLimit)
            {
                /* Add to the return counter */
                AccumulatedGiveBack = RTL::Atomic::Add64((PLONG_PTR)&QuotaEntry->Return, GiveBackAmount);

                /* Check if the accumulated returns exceed the threshold */
                if(AccumulatedGiveBack > GiveBackLimit)
                {
                    /* Extract the accumulated return amount and reset the counter to zero */
                    ExtractedReturnAmount = (SIZE_T)RTL::Atomic::Exchange64((PLONG_PTR)&QuotaEntry->Return, (LONG_PTR)0);

                    /* Check if there is anything to return */
                    if(ExtractedReturnAmount > 0)
                    {
                        /* Return the extracted quota back to the system pool */
                        MM::Quota::ReturnPoolQuota((MMPOOL_TYPE)QuotaType, ExtractedReturnAmount);
                    }
                }
            }
        }
    }

    /* Decrease the block's overall usage count */
    RTL::Atomic::Add64((PLONG_PTR)&QuotaEntry->Usage, (SIZE_T)(-(SSIZE_T)Amount));

    /* Check if a specific process was provided */
    if(Process)
    {
        /* Decrease the process's specific usage count */
        RTL::Atomic::Add64((PLONG_PTR)&Process->QuotaUsage[QuotaType], (SIZE_T)(-(SSIZE_T)Amount));
    }
}

/**
 * Returns previously charged pool amounts back to a shared quota block.
 *
 * @param QuotaBlock
 *        Supplies a pointer to the process quota block.
 *
 * @param PagedAmount
 *        Supplies the amount of paged pool quota to return, in bytes.
 *
 * @param NonPagedAmount
 *        Supplies the amount of non-paged pool quota to return, in bytes.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTAPI
VOID
PS::Quota::ReturnSharedPoolQuota(IN PEPROCESS_QUOTA_BLOCK QuotaBlock,
                                 IN SIZE_T PagedAmount,
                                 IN SIZE_T NonPagedAmount)
{
    /* Check for NULL pointers and bypass markers */
    if(!QuotaBlock || QuotaBlock == PS_QUOTA_BYPASS_MARKER)
    {
        /* No quota tracking is required for the system process or empty blocks, return */
        return;
    }

    /* Check for paged pool quota */
    if(PagedAmount > 0)
    {
        /* Process the return of paged pool memory */
        ReturnProcessQuota(QuotaBlock, NULLPTR, PsPagedPool, PagedAmount);
    }

    /* Check for non-paged pool quota */
    if(NonPagedAmount > 0)
    {
        /* Process the return of non-paged pool memory */
        ReturnProcessQuota(QuotaBlock, NULLPTR, PsNonPagedPool, NonPagedAmount);
    }

    /* Drop the reference count on the quota block */
    DereferenceQuotaBlock(QuotaBlock);
}
