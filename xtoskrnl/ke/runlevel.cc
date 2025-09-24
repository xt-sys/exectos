/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/runlevel.cc
 * DESCRIPTION:     Running Level management support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/**
 * Gets the current running level of the current processor.
 *
 * @return This routine returns the current running level.
 *
 * @since XT 1.0
 */
XTFASTCALL
KRUNLEVEL
KE::RunLevel::GetCurrentRunLevel(VOID)
{
    return HL::RunLevel::GetRunLevel();
}

/**
 * Lowers the running level of the current processor.
 *
 * @param RunLevel
 *        Supplies the new running level to lower to.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
XTFASTCALL
VOID
KE::RunLevel::LowerRunLevel(IN KRUNLEVEL RunLevel)
{
    KRUNLEVEL OldRunLevel;

    /* Read current run level */
    OldRunLevel = HL::RunLevel::GetRunLevel();

    /* Validate run level lowerage */
    if(OldRunLevel > RunLevel)
    {
        /* Set new, lower run level */
        HL::RunLevel::SetRunLevel(RunLevel);
    }
}

/**
 * Raises the running level of the current processor.
 *
 * @param RunLevel
 *        Supplies the new running level to raise to.
 *
 * @return This routine returns the old running level.
 *
 * @since XT 1.0
 */
XTFASTCALL
KRUNLEVEL
KE::RunLevel::RaiseRunLevel(IN KRUNLEVEL RunLevel)
{
    KRUNLEVEL OldRunLevel;

    /* Read current run level */
    OldRunLevel = HL::RunLevel::GetRunLevel();

    /* Validate run level raise */
    if(OldRunLevel < RunLevel)
    {
        /* Set new, higher run level */
        HL::RunLevel::SetRunLevel(RunLevel);
    }

    /* Return old run level */
    return OldRunLevel;
}
