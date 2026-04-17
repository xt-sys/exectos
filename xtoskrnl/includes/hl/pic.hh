/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/pic.hh
 * DESCRIPTION:     HAL processor support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_PIC_HH
#define __XTOSKRNL_HL_PIC_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class Pic
    {
        private:
            STATIC APIC_MODE ApicMode;
            STATIC ULONG ControllerCount;
            STATIC IOAPIC_DATA Controllers[IOAPIC_MAX_CONTROLLERS];
            STATIC ULONG IrqOverrideCount;
            STATIC ACPI_MADT_INTERRUPT_OVERRIDE IrqOverrides[IOAPIC_MAX_OVERRIDES];
            STATIC UCHAR MappedVectors[256];

        public:
            STATIC XTAPI VOID ClearApicErrors(VOID);
            STATIC XTAPI ULONG GetCpuApicId(VOID);
            STATIC XTAPI VOID InitializeIOApic(VOID);
            STATIC XTAPI VOID InitializePic(VOID);
            STATIC XTFASTCALL ULONGLONG ReadApicRegister(IN APIC_REGISTER Register);
            STATIC XTAPI VOID SendEoi(VOID);
            STATIC XTAPI VOID SendIpi(IN ULONG ApicId,
                                      IN ULONG Vector);
            STATIC XTAPI VOID SendSelfIpi(IN ULONG Vector);
            STATIC XTFASTCALL VOID WriteApicRegister(IN APIC_REGISTER Register,
                                                     IN ULONGLONG Value);

        private:
            STATIC XTAPI VOID AllocateSystemInterrupt(IN UCHAR Irq,
                                                      IN UCHAR RunLevel,
                                                      IN UCHAR Vector);
            STATIC XTAPI BOOLEAN CheckApicSupport(VOID);
            STATIC XTAPI BOOLEAN CheckX2ApicSupport(VOID);
            STATIC XTAPI XTSTATUS DetectIoApicControllers(VOID);
            STATIC XTAPI VOID ResolveInterruptOverride(IN UCHAR Irq,
                                                       OUT PULONG Gsi,
                                                       OUT PUSHORT Flags);
            STATIC XTAPI XTSTATUS GetIoApicController(IN ULONG Gsi,
                                                      OUT PIOAPIC_DATA *Controller,
                                                      OUT PULONG EntryNumber);
            STATIC XTAPI VOID InitializeApic(VOID);
            STATIC XTAPI VOID InitializeLegacyPic(VOID);
            STATIC XTFASTCALL ULONG ReadIOApicRegister(IN PIOAPIC_DATA Controller,
                                                       IN UCHAR Register);
            STATIC XTFASTCALL IOAPIC_REDIRECTION_REGISTER ReadRedirectionEntry(IN PIOAPIC_DATA Controller,
                                                                               IN ULONG EntryNumber);
            STATIC XTFASTCALL UCHAR TranslateGsiToVector(IN ULONG Gsi);
            STATIC XTFASTCALL VOID WriteIOApicRegister(IN PIOAPIC_DATA Controller,
                                                       IN UCHAR Register,
                                                       IN ULONG DataValue);
            STATIC XTFASTCALL VOID WriteRedirectionEntry(IN PIOAPIC_DATA Controller,
                                                         IN ULONG EntryNumber,
                                                         IN IOAPIC_REDIRECTION_REGISTER EntryData);
    };
}

#endif /* __XTOSKRNL_HL_PIC_HH */
