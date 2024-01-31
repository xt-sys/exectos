/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/rtl/guid.c
 * DESCRIPTION:     GUID manipulation routines
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


XTAPI
BOOLEAN
RtlCompareGuids(IN PGUID Guid1,
                IN PGUID Guid2)
{
    PUINT Guid1Ptr, Guid2Ptr;

    /* Cast GUIDs to UINT to compare 32-bits at a time */
    Guid1Ptr = (PUINT)Guid1;
    Guid2Ptr = (PUINT)Guid2;

    /* Compare GUIDs */
    return(Guid1Ptr[0] == Guid2Ptr[0] && Guid1Ptr[1] == Guid2Ptr[1] &&
           Guid1Ptr[2] == Guid2Ptr[2] && Guid1Ptr[3] == Guid2Ptr[3]);
}
