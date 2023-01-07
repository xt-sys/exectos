/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/i686/xtstruct.h
 * DESCRIPTION:     XT structures forward references specific to i686 architecture
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_I686_XTSTRUCT_H
#define __XTDK_I686_XTSTRUCT_H


/* Architecture-related structures forward references */
typedef struct _CPUID_REGISTERS CPUID_REGISTERS, *PCPUID_REGISTERS;
typedef struct _HARDWARE_PTE HARDWARE_PTE, *PHARDWARE_PTE;
typedef struct _HARDWARE_PTE_PAE HARDWARE_PTE_PAE, *PHARDWARE_PTE_PAE;
typedef struct _KGDTENTRY KGDTENTRY, *PKGDTENTRY;
typedef struct _KIDTENTRY KIDTENTRY, *PKIDTENTRY;
typedef struct _KIIO_ACCESS_MAP KIIO_ACCESS_MAP, *PKIIO_ACCESS_MAP;
typedef struct _KTSS KTSS, *PKTSS;

#endif /* __XTDK_I686_XTSTRUCT_H */
