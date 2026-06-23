/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/xtbase.h
 * DESCRIPTION:     Basic and common native XT structures
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_XTBASE_H
#define __XTDK_XTBASE_H

#include <xtdefs.h>
#include <xtstruct.h>
#include <xttypes.h>


/* C/C++ specific code */
#ifndef __XTOS_ASSEMBLER__

/* Kernel affinity */
typedef ULONG_PTR KAFFINITY, *PKAFFINITY;

/* Kernel priority */
typedef LONG KPRIORITY, *PKPRIORITY;

/* Processor modes */
typedef CHAR KPROCESSOR_MODE, *PKPROCESSOR_MODE;

/* Interrupt Request Run Level (IRQL) */
typedef UCHAR KRUNLEVEL, *PKRUNLEVEL;

/* Spin locks synchronization mechanism */
typedef ULONG_PTR KSPIN_LOCK, *PKSPIN_LOCK;

/* Page Frame Number count */
typedef ULONG PFN_COUNT;

/* Page Frame Number */
typedef ULONG_PTR PFN_NUMBER, *PPFN_NUMBER;

/* Physical address */
typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

/* 128-bit 16-byte aligned XMM register */
typedef struct _M128
{
    ULONGLONG Low;
    LONGLONG High;
} ALIGN(16) M128, *PM128;

#endif /* __XTOS_ASSEMBLER_ */
#endif /* __XTDK_XTBASE_H */
