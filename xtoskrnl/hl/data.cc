/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/data.cc
 * DESCRIPTION:     Hardware Layer global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


/* Tracks the total number of currently cached ACPI tables */
ULONG HL::Acpi::CacheCount = 0;

/* Array holding the cached ACPI tables */
ACPI_CACHE_LIST HL::Acpi::CacheEntries[ACPI_MAX_CACHED_TABLES];

/* Head of the linked list tracking dynamically mapped ACPI tables */
LIST_ENTRY HL::Acpi::CacheList;

/* Pointer to the ACPI Root System Description Pointer (RSDP) */
PACPI_RSDP HL::Acpi::RsdpStructure;

/* Global architectural system states and hardware feature flags parsed from the ACPI tables */
ACPI_SYSTEM_INFO HL::Acpi::SystemInfo;

/* Hardware configuration details and port addresses for the ACPI Power Management Timer */
ACPI_TIMER_INFO HL::Acpi::TimerInfo;

/* Represents the number of active processors */
KAFFINITY HL::Cpu::ActiveProcessors;

/* Metadata detailing the linear frame buffer geometry */
HL_FRAMEBUFFER_DATA HL::FrameBuffer::FrameBufferData;

/* Pointer to the RAM shadow buffer used for double-buffered rendering */
PVOID HL::FrameBuffer::ScreenShadowBuffer;

/* Tracks the bounding box, dimensions, and cursor position for the kernel video console */
HL_SCROLL_REGION_DATA HL::FrameBuffer::ScrollRegionData;

/* Indicates the active hardware interrupt controller mode */
APIC_MODE HL::Pic::ApicMode;

/* Total number of I/O APIC chips discovered and initialized */
ULONG HL::Pic::ControllerCount;

/* Array containing MMIO bases, IDs, and line counts for all I/O APICs */
IOAPIC_DATA HL::Pic::Controllers[IOAPIC_MAX_CONTROLLERS];

/* Total number of legacy ISA interrupt routing overrides */
ULONG HL::Pic::IrqOverrideCount;

/* Hardware routing definitions mapping legacy ISA interrupts to Global System Interrupts (GSI) */
ACPI_MADT_INTERRUPT_OVERRIDE HL::Pic::IrqOverrides[IOAPIC_MAX_OVERRIDES];

/* Lookup table mapping allocated hardware APIC vectors to their assigned Run Levels */
UCHAR HL::Pic::MappedVectors[256];

/* Accumulated tick value of the ACPI Power Management Timer */
ULONG HL::Timer::AcpiPmPerformanceCounter = 0;

/* Primary hardware timer driving the periodic system clock interrupt */
TIMER_TYPE HL::Timer::ClockType;

/* Fractional remainder used to maintain long-term system clock accuracy */
ULONG HL::Timer::FractionalIncrement = 0;

/* Virtual address mapped to the HPET hardware MMIO registers */
PVOID HL::Timer::HpetAddress = NULLPTR;

/* Operating frequency of the High Precision Event Timer in ticks per second */
ULONGLONG HL::Timer::HpetFrequency = 0;

/* Spinlock protecting concurrent multiprocessor access to the global performance counters */
KSPIN_LOCK HL::Timer::PerformanceCounterLock;

/* The performance counter frequency in ticks per second */
ULONGLONG HL::Timer::PerformanceFrequency = 0;

/* Absolute monotonic tick count driven by the legacy Programmable Interval Timer */
ULONGLONG HL::Timer::PitPerformanceCounter;

/* Programmed hardware divider interval used to increment the PIT performance counter */
ULONG HL::Timer::PitRollover;

/* Flag indicating whether statistical system execution profiling is currently active */
BOOLEAN HL::Timer::ProfilingEnabled = FALSE;

/* Tick interval required to trigger a profile interrupt */
ULONG HL::Timer::ProfilingTicks;

/* Global accumulator for fractional time drift and precision compensation */
ULONG HL::Timer::RunningFraction = 0;

/* System counter driven by the highest precision available hardware */
ULONGLONG HL::Timer::SystemPerformanceCounter;

/* Current base clock increment in standard 100-nanosecond intervals */
ULONG HL::Timer::TimeIncrement = 0;

/* Timer capabilities */
TIMER_CAPABILITIES HL::Timer::TimerCapabilities = {0};

/* APIC timer frequency */
ULONG HL::Timer::TimerFrequency;

/* Function dispatch table for the active hardware timer backend */
TIMER_ROUTINES HL::Timer::TimerRoutines = {0};

/* Identifies the hardware timer backing */
TIMER_TYPE HL::Timer::TimerType;
