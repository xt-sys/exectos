/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ex/rundown.cc
 * DESCRIPTION:     Rundown protection mechanism
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Acquires the rundown protection for given descriptor.
 *
 * @param Descriptor
 *        Supplies a pointer to the rundown block descriptor.
 *
 * @return This routine returns TRUE if protection acquired successfully, or FALSE otherwise.
 *
 * @since NT 5.1
 */
XTFASTCALL
BOOLEAN
EX::Rundown::AcquireProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    ULONG_PTR CurrentValue, NewValue;

    /* Get current value */
    CurrentValue = Descriptor->Count;

    /* Main loop execution */
    while(TRUE)
    {
        /* Make sure protection is not active yet */
        if(CurrentValue & 0x1)
        {
            /* Already active, nothing to do */
            return FALSE;
        }

        /* Attempt to increment the usage count */
        NewValue = CurrentValue + 2;

        /* Exchange the value */
        NewValue = (ULONG_PTR)RTL::Atomic::CompareExchangePointer(&Descriptor->Ptr, (PVOID)NewValue,
                                                                  (PVOID)CurrentValue);

        /* Make sure protection acquired */
        if(NewValue == CurrentValue)
        {
            /* Successfully acquired protection */
            return TRUE;
        }

        /* Update value and try once again */
        CurrentValue = NewValue;
    }
}

/**
 * Marks the rundown descriptor as completed.
 *
 * @param Descriptor
 *        Supplies a pointer to the descriptor to be completed.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTFASTCALL
VOID
EX::Rundown::CompleteProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    RTL::Atomic::ExchangePointer(&Descriptor->Ptr, (PVOID)EX_RUNDOWN_ACTIVE);
}

/**
 * Initializes the rundown protection descriptor.
 *
 * @param Descriptor
 *        Supplies a pointer to the descriptor to be initialized.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTFASTCALL
VOID
EX::Rundown::InitializeProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    /* Reset descriptor counter */
    Descriptor->Count = 0;
}

/**
 * Reinitializes the rundown protection structure after it has been completed.
 *
 * @param Descriptor
 *        Supplies a pointer to the descriptor to be reinitialized.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTFASTCALL
VOID
EX::Rundown::ReInitializeProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    RTL::Atomic::ExchangePointer(&Descriptor->Ptr, NULLPTR);
}

/**
 * Releases the rundown protection for given descriptor.
 *
 * @param Descriptor
 *        Supplies a pointer to the descriptor to be initialized.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTFASTCALL
VOID
EX::Rundown::ReleaseProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    ULONG_PTR CurrentValue, NewValue;
    PEX_RUNDOWN_WAIT_BLOCK WaitBlock;

    CurrentValue = Descriptor->Count;

    while(TRUE)
    {
        if(CurrentValue & 0x1)
        {
            WaitBlock = (PEX_RUNDOWN_WAIT_BLOCK)(CurrentValue & ~0x1);

            if(!RTL::Atomic::Decrement64((PLONG_PTR)&WaitBlock->Count))
            {
                KE::Event::SetEvent(&WaitBlock->WakeEvent, 0, FALSE);
            }

            break;
        }
        else
        {
            /* Attempt to decrement the usage count */
            NewValue = CurrentValue - 2;

            /* Exchange the value */
            NewValue = (ULONG_PTR)RTL::Atomic::CompareExchangePointer(&Descriptor->Ptr, (PVOID)NewValue,
                                                                      (PVOID)CurrentValue);

            if(NewValue == CurrentValue)
            {
                break;
            }

            CurrentValue = NewValue;
        }
    }
}

/**
 * Waits until rundown protection calls are completed.
 *
 * @param Descriptor
 *        Supplies a pointer to the rundown block descriptor.
 *
 * @return This routine does not return any value.
 *
 * @since NT 5.1
 */
XTFASTCALL
VOID
EX::Rundown::WaitForProtectionRelease(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    UNIMPLEMENTED;
}
