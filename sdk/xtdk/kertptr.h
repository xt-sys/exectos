/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            sdk/xtdk/kertptr.h
 * DESCRIPTION:     XT kernel core handlers and routine pointers
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#ifndef __XTDK_KERTPTR_H
#define __XTDK_KERTPTR_H

#include <xtstruct.h>
#include <xttarget.h>
#include <xttypes.h>
#include ARCH_HEADER(ketypes.h)


/* Kernel routine callbacks */
typedef EXCEPTION_DISPOSITION (*PEXCEPTION_ROUTINE)(IN PEXCEPTION_RECORD ExceptionRecord, IN PVOID EstablisherFrame, IN OUT PCONTEXT ContextRecord, IN OUT PVOID DispatcherContext);

#endif /* __XTDK_KERTPTR_H */
