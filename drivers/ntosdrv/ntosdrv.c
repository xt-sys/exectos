/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            drivers/ntosdrv/ntosdrv.c
 * DESCRIPTION:     NTOS compatibility driver
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtkmapi.h>

XTAPI
XTSTATUS
XtDriverEntry()
{
    /* Return success */
    return STATUS_SUCCESS;
}
