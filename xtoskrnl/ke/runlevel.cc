/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ke/runlevel.cc
 * DESCRIPTION:     Running Level management support
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Kernel Library */
namespace KE
{

/**
 * Gets the current running level of the current processor.
 *
 * @return This routine returns the current running level.
 *
 * @since XT 1.0
 */
XTFASTCALL
KRUNLEVEL
RunLevel::GetCurrentRunLevel(VOID)
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
RunLevel::LowerRunLevel(IN KRUNLEVEL RunLevel)
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
RunLevel::RaiseRunLevel(IN KRUNLEVEL RunLevel)
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

} /* namespace */


/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTCLINK
XTFASTCALL
KRUNLEVEL
KeGetCurrentRunLevel(VOID)
{
    return KE::RunLevel::GetCurrentRunLevel();
}

/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTCLINK
XTFASTCALL
VOID
KeLowerRunLevel(KRUNLEVEL RunLevel)
{
    KE::RunLevel::LowerRunLevel(RunLevel);
}

/* TEMPORARY FOR COMPATIBILITY WITH C CODE */
XTCLINK
XTFASTCALL
KRUNLEVEL
KeRaiseRunLevel(KRUNLEVEL RunLevel)
{
    return KE::RunLevel::RaiseRunLevel(RunLevel);
}
