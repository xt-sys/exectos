/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/data.cc
 * DESCRIPTION:     Hardware Layer global and static data
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.hh>


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
