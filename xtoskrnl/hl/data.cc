/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/data.cc
 * DESCRIPTION:     Hardware Layer global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* ACPI tables cache count */
ULONG HL::Acpi::CacheCount = 0;

/* ACPI tables cache entries */
ACPI_CACHE_LIST HL::Acpi::CacheEntries[ACPI_MAX_CACHED_TABLES];

/* ACPI tables cache list */
LIST_ENTRY HL::Acpi::CacheList;

/* ACPI Root System Description Pointer (RSDP) */
PACPI_RSDP HL::Acpi::RsdpStructure;

/* System information */
ACPI_SYSTEM_INFO HL::Acpi::SystemInfo;

/* ACPI timer information */
ACPI_TIMER_INFO HL::Acpi::TimerInfo;

/* Active processors count */
KAFFINITY HL::Cpu::ActiveProcessors;

/* FrameBuffer information */
HL_FRAMEBUFFER_DATA HL::FrameBuffer::FrameBufferData;

/* Scroll region information */
HL_SCROLL_REGION_DATA HL::FrameBuffer::ScrollRegionData;

/* APIC mode */
APIC_MODE HL::Pic::ApicMode;

/* Number of I/O APIC controllers */
ULONG HL::Pic::ControllerCount;

/* I/O APIC controllers information */
IOAPIC_DATA HL::Pic::Controllers[IOAPIC_MAX_CONTROLLERS];

/* Number of I/O APIC overrides */
ULONG HL::Pic::IrqOverrideCount;

/* I/O APIC overrides information */
ACPI_MADT_INTERRUPT_OVERRIDE HL::Pic::IrqOverrides[IOAPIC_MAX_OVERRIDES];

/* Mapped interrupt vectors */
UCHAR HL::Pic::MappedVectors[256];

/* Kernel profiling interval */
ULONG HL::Timer::ProfilingInterval;

/* Timer capabilities */
TIMER_CAPABILITIES HL::Timer::TimerCapabilities = {0};

/* APIC timer frequency */
ULONG HL::Timer::TimerFrequency;
