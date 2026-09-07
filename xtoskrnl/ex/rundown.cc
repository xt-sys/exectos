/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ex/rundown.cc
 * DESCRIPTION:     Rundown protection mechanism
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 *                  Aiken Harris <harraiken91@gmail.com>
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
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
EX::Rundown::AcquireProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    /* Acquire protection */
    return AcquireProtection(Descriptor, 1);
}

/**
 * Acquires the rundown protection for given descriptor.
 *
 * @param Descriptor
 *        Supplies a pointer to the rundown block descriptor.
 *
 * @param Count
 *        Supplies the number of times the protection should be acquired.
 *
 * @return This routine returns TRUE if protection acquired successfully, or FALSE otherwise.
 *
 * @since XT 1.0
 */
XTFASTCALL
BOOLEAN
EX::Rundown::AcquireProtection(IN PEX_RUNDOWN_REFERENCE Descriptor,
                               IN ULONG Count)
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
        NewValue = CurrentValue + 2 * Count;

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
 * @since XT 1.0
 */
XTFASTCALL
VOID
EX::Rundown::CompleteProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    /* Mark the descriptor as completely run down */
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
 * @since XT 1.0
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
 * @since XT 1.0
 */
XTFASTCALL
VOID
EX::Rundown::ReInitializeProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    /* Reset the descriptor to its initial state */
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
 * @since XT 1.0
 */
XTFASTCALL
VOID
EX::Rundown::ReleaseProtection(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    /* Release protection */
    ReleaseProtection(Descriptor, 1);
}

/**
 * Releases the rundown protection for given descriptor.
 *
 * @param Descriptor
 *        Supplies a pointer to the descriptor to be initialized.
 *
 * @param Count
 *        Supplies the number of references to release.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
EX::Rundown::ReleaseProtection(IN PEX_RUNDOWN_REFERENCE Descriptor,
                               IN ULONG Count)
{
    ULONG_PTR CurrentValue, NewValue;
    PEX_RUNDOWN_WAIT_BLOCK WaitBlock;

    /* Read the current state of the rundown descriptor */
    CurrentValue = Descriptor->Count;

    /* Enter a CAS loop */
    while(TRUE)
    {
        /* Check if the rundown is currently active */
        if(CurrentValue & 0x1)
        {
            /* Extract the pointer to the wait block */
            WaitBlock = (PEX_RUNDOWN_WAIT_BLOCK)(CurrentValue & ~0x1);

            /* Decrement the pending reference count */
            if(!RTL::Atomic::Decrement64((PLONG_PTR)&WaitBlock->Count))
            {
                /* Signal the event to wake up the teardown thread */
                KE::Event::SetEvent(&WaitBlock->WakeEvent, 0, FALSE);
            }

            /* Break the loop */
            break;
        }
        else
        {
            /* Attempt to decrement the usage count */
            NewValue = CurrentValue - 2 * Count;

            /* Exchange the value */
            NewValue = (ULONG_PTR)RTL::Atomic::CompareExchangePointer(&Descriptor->Ptr, (PVOID)NewValue,
                                                                      (PVOID)CurrentValue);

            /* Check if the atomic swap succeeded without interference from other processors */
            if(NewValue == CurrentValue)
            {
                /* Reference count is decremented, break the loop */
                break;
            }

            /* Collision detected, update current state and retry */
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
 * @since XT 1.0
 */
XTFASTCALL
VOID
EX::Rundown::WaitForProtectionRelease(IN PEX_RUNDOWN_REFERENCE Descriptor)
{
    UNIMPLEMENTED;
}
