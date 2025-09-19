/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/ar/i686/boot.S
 * DESCRIPTION:     i686-specific boot code for setting up the low-level CPU environment
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <ar/amd64/asmsup.h>

.altmacro
.text


/**
 * Starts an application processor (AP). This is just a stub.
 *
 * @return This routine does not return any value.
 *
 * @since XT 1.0
 */
.global _ArStartApplicationProcessor
_ArStartApplicationProcessor:

.global _ArStartApplicationProcessorEnd
_ArStartApplicationProcessorEnd:
