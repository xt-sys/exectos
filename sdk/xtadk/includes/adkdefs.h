/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtadk/adkdefs.h
 * DESCRIPTION:     Definitions for XTADK
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#ifndef __XTADK_ADKDEFS_H
#define __XTADK_ADKDEFS_H


/* Macros for calculating structure size and offsets for assembler code */
#define ADK_DEFINE(Symbol, Value)              __asm__ volatile("\n\t# ==> " #Symbol " %c0" : : "i" ((SIZE_T)(Value)))
#define ADK_OFFSET(Structure, Member)          ADK_DEFINE(Structure ## _ ## Member, FIELD_OFFSET(Structure, Member))
#define ADK_SIZE(Structure)                    ADK_DEFINE(Structure ## _SIZE, sizeof(Structure))
#define ADK_SIZE_FROM(Name, Structure, Member) ADK_DEFINE(Structure ## _ ## Name, sizeof(Structure) - FIELD_OFFSET(Structure, Member))

#endif /* __XTADK_ADKDEFS_H */
