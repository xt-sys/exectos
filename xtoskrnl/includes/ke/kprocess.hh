/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ke/kprocess.hh
 * DESCRIPTION:     XT kernel process manipulation support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_KE_KPROCESS_HH
#define __XTOSKRNL_KE_KPROCESS_HH

#include <xtos.hh>


/* Kernel Library */
namespace KE
{
    class KProcess
    {
        private:
            STATIC EPROCESS InitialProcess;

        public:
            STATIC XTAPI PKPROCESS GetIdleProcess(VOID);
            STATIC XTAPI PEPROCESS GetInitialProcess(VOID);
            STATIC XTAPI VOID InitializeIdleProcess(IN OUT PKPROCESS Process,
                                                    IN PULONG_PTR DirectoryTable);
            STATIC XTAPI VOID InitializeProcess(IN OUT PKPROCESS Process,
                                                IN KPRIORITY Priority,
                                                IN PULONG_PTR DirectoryTable,
                                                IN BOOLEAN Alignment);
    };
}

#endif /* __XTOSKRNL_KE_KPROCESS_HH */
