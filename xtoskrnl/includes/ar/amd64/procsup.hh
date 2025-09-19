/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/ar/amd64/procsup.hh
 * DESCRIPTION:     Architecture-specific routines for AMD64 processor initialization and system structure setup
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_AR_PROCSUP_HH
#define __XTOSKRNL_AR_PROCSUP_HH

#include <xtos.hh>


/* Architecture-specific Library */
namespace AR
{
    class ProcSup
    {
        private:
            STATIC UCHAR BootStack[KERNEL_STACK_SIZE];
            STATIC UCHAR FaultStack[KERNEL_STACK_SIZE];
            STATIC KGDTENTRY InitialGdt[GDT_ENTRIES];
            STATIC KIDTENTRY InitialIdt[IDT_ENTRIES];
            STATIC KPROCESSOR_BLOCK InitialProcessorBlock;
            STATIC KTSS InitialTss;

        public:
            STATIC XTAPI PVOID GetBootStack(VOID);
            STATIC XTAPI VOID GetTrampolineInformation(IN TRAMPOLINE_TYPE TrampolineType,
                                                       OUT PVOID *TrampolineCode,
                                                       OUT PULONG_PTR TrampolineSize);
            STATIC XTAPI VOID InitializeProcessor(IN PVOID ProcessorStructures);

        private:
            STATIC XTAPI VOID IdentifyProcessor(VOID);
            STATIC XTAPI VOID InitializeGdt(IN PKPROCESSOR_BLOCK ProcessorBlock);
            STATIC XTAPI VOID InitializeIdt(IN PKPROCESSOR_BLOCK ProcessorBlock);
            STATIC XTAPI VOID InitializeProcessorBlock(OUT PKPROCESSOR_BLOCK ProcessorBlock,
                                                       IN PKGDTENTRY Gdt,
                                                       IN PKIDTENTRY Idt,
                                                       IN PKTSS Tss,
                                                       IN PVOID DpcStack);
            STATIC XTAPI VOID InitializeProcessorRegisters(VOID);
            STATIC XTAPI VOID InitializeProcessorStructures(IN PVOID ProcessorStructures,
                                                            OUT PKGDTENTRY *Gdt,
                                                            OUT PKTSS *Tss,
                                                            OUT PKPROCESSOR_BLOCK *ProcessorBlock,
                                                            OUT PVOID *KernelBootStack,
                                                            OUT PVOID *KernelFaultStack);
            STATIC XTAPI VOID InitializeSegments(VOID);
            STATIC XTAPI VOID InitializeTss(IN PKPROCESSOR_BLOCK ProcessorBlock,
                                            IN PVOID KernelBootStack,
                                            IN PVOID KernelFaultStack);
            STATIC XTAPI VOID SetGdtEntry(IN PKGDTENTRY Gdt,
                                          IN USHORT Selector,
                                          IN ULONG_PTR Base,
                                          IN ULONG Limit,
                                          IN UCHAR Type,
                                          IN UCHAR Dpl,
                                          IN UCHAR SegmentMode);
            STATIC XTAPI VOID SetGdtEntryBase(IN PKGDTENTRY Gdt,
                                              IN USHORT Selector,
                                              IN ULONG_PTR Base);
            STATIC XTAPI VOID SetIdtGate(IN PKIDTENTRY Idt,
                                         IN USHORT Vector,
                                         IN PVOID Handler,
                                         IN USHORT Selector,
                                         IN USHORT Ist,
                                         IN USHORT Access);
    };
}

#endif /* __XTOSKRNL_AR_PROCSUP_HH */
