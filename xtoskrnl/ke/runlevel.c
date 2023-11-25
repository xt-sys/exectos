/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/runlevel.c
 * DESCRIPTION:     Running Level management support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/**
 * Gets the current running level of the current processor.
 *
 * @return This routine returns the current running level.
 *
 * @since XT 1.0
 */
XTFASTCALL
KRUNLEVEL
KeGetCurrentRunLevel(VOID)
{
    return HlGetRunLevel();
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
KeLowerRunLevel(IN KRUNLEVEL RunLevel)
{
    KRUNLEVEL OldRunLevel;

    /* Read current run level */
    OldRunLevel = HlGetRunLevel();

    /* Validate run level lowerage */
    if(OldRunLevel > RunLevel)
    {
        /* Set new, lower run level */
        HlSetRunLevel(RunLevel);
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
KeRaiseRunLevel(IN KRUNLEVEL RunLevel)
{
    KRUNLEVEL OldRunLevel;

    /* Read current run level */
    OldRunLevel = HlGetRunLevel();

    /* Validate run level raise */
    if(OldRunLevel < RunLevel)
    {
        /* Set new, higher run level */
        HlSetRunLevel(RunLevel);
    }

    /* Return old run level */
    return OldRunLevel;
}
