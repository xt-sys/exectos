/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            drivers/ntosdrv/ntosdrv.c
 * DESCRIPTION:     NTOS compatibility driver
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtkmapi.h>


/**
 * This routine is the entry point of the driver.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTAPI
XTSTATUS
XtDriverEntry()
{
    /* Return success */
    return STATUS_SUCCESS;
}
