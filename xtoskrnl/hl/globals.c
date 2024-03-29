/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/hl/globals.c
 * DESCRIPTION:     Architecture independent global variables related to HL subsystem
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <xtos.h>


/* Active processors count */
KAFFINITY HlpActiveProcessors;

/* APIC mode */
HAL_APIC_MODE HlpApicMode;

/* FrameBuffer information */
HAL_FRAMEBUFFER_DATA HlpFrameBufferData;

/* Processors identity table */
HAL_PROCESSOR_IDENTITY HlpProcessorsIdentity[MAXIMUM_PROCESSORS] = {{0}};
