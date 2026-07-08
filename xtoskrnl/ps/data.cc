/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtoskrnl/po/data.cc
 * DESCRIPTION:     Process global and static data
 * DEVELOPERS:      Aiken Harris <harraiken91@gmail.com>
 */

#include <xtos.hh>


/* Default system-wide quota block assigned to processes */
EPROCESS_QUOTA_BLOCK PS::Quota::DefaultQuotaBlock;
