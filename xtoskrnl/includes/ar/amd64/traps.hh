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

#endif /* __XTOSKRNL_AR_TRAPS_HH */
