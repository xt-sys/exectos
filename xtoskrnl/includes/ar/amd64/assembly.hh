/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ar/amd64/assembly.hh
 * DESCRIPTION:     Architecture-specific assembler routines forward references
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_AR_ASSEMBLY_HH
#define __XTOSKRNL_AR_ASSEMBLY_HH

#include <xtos.hh>


/* TrampolineEnableXpa end address to calculate trampoline size */
XTCLINK PVOID ArEnableExtendedPhysicalAddressingEnd[];

/* TrampolineApStartup end address to calculate trampoline size */
XTCLINK PVOID ArStartApplicationProcessorEnd[];


/* Forward reference for assembler code */
XTCLINK
XTCDECL
VOID
ArEnableExtendedPhysicalAddressing(IN ULONG_PTR PageMap);

XTCLINK
XTCDECL
VOID
ArStartApplicationProcessor(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x00(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x01(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x02(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x03(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x04(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x05(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x06(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x07(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x08(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x09(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x0A(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x0B(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x0C(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x0D(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x0E(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x10(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x11(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x12(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x13(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x1F(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x2C(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x2D(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0x2F(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0xE1(VOID);

XTCLINK
XTCDECL
VOID
ArTrap0xFF(VOID);

#endif /* __XTOSKRNL_AR_ASSEMBLY_HH */
