/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ar/amd64/traps.hh
 * DESCRIPTION:     Trap handling routines and the dispatcher for processor exceptions
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_AR_TRAPS_HH
#define __XTOSKRNL_AR_TRAPS_HH

#include <xtos.hh>


/* Architecture-specific Library */
namespace AR
{
    class Traps
    {
        public:
            STATIC XTCDECL VOID DispatchTrap(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID InitializeSystemCallMsrs(VOID);

        private:
            STATIC XTCDECL VOID HandleSystemCall32(VOID);
            STATIC XTCDECL VOID HandleSystemCall64(VOID);
            STATIC XTCDECL VOID HandleTrap00(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap01(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap02(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap03(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap04(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap05(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap06(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap07(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap08(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap09(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap0A(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap0B(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap0C(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap0D(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap0E(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap10(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap11(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap12(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap13(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap1F(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap2C(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap2D(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrap2F(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrapE1(IN PKTRAP_FRAME TrapFrame);
            STATIC XTCDECL VOID HandleTrapFF(IN PKTRAP_FRAME TrapFrame);
    };
}

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

#endif /* __XTOSKRNL_AR_TRAPS_HH */
