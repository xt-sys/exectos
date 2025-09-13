/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/includes/hl/acpi.hh
 * DESCRIPTION:     Advanced Configuration and Power Interface (ACPI) support
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTOSKRNL_HL_ACPI_HH
#define __XTOSKRNL_HL_ACPI_HH

#include <xtos.hh>


/* Hardware Layer */
namespace HL
{
    class Acpi
    {
        private:
            STATIC LIST_ENTRY CacheList;
            STATIC PACPI_RSDP RsdpStructure;
            STATIC ACPI_SYSTEM_INFO SystemInfo;
            STATIC ACPI_TIMER_INFO TimerInfo;

        public:
            STATIC XTAPI XTSTATUS GetAcpiSystemDescriptionPointer(OUT PACPI_RSDP *Rsdp);
            STATIC XTAPI XTSTATUS GetAcpiTable(IN ULONG Signature,
                                               OUT PACPI_DESCRIPTION_HEADER *AcpiTable);
            STATIC XTAPI XTSTATUS InitializeAcpi(VOID);
            STATIC XTAPI XTSTATUS InitializeAcpiSystemInformation(VOID);
        
        private:
            STATIC XTAPI VOID CacheAcpiTable(IN PACPI_DESCRIPTION_HEADER AcpiTable);
            STATIC XTAPI XTSTATUS InitializeAcpiCache(VOID);
            STATIC XTAPI XTSTATUS InitializeAcpiSystemDescriptionTable(OUT PACPI_DESCRIPTION_HEADER *AcpiTable);
            STATIC XTAPI XTSTATUS InitializeAcpiSystemStructure(VOID);
            STATIC XTAPI XTSTATUS InitializeAcpiTimer(VOID);
            STATIC XTAPI XTSTATUS QueryAcpiCache(IN ULONG Signature,
                                                 OUT PACPI_DESCRIPTION_HEADER *AcpiTable);
            STATIC XTAPI XTSTATUS QueryAcpiTables(IN ULONG Signature,
                                                  OUT PACPI_DESCRIPTION_HEADER *AcpiTable);
            STATIC XTAPI BOOLEAN ValidateAcpiTable(IN PVOID Buffer,
                                                   IN UINT_PTR Size);
    };
}

#endif /* __XTOSKRNL_HL_ACPI_HH */
