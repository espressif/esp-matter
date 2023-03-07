/***************************************************************************//**
 * @file
 * @brief The TZ utilities for Gecko Bootloader
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <arm_cmse.h>
#include "btl_tz_utils.h"
#include "core/btl_reset.h"
#include "api/btl_reset_info.h"

// -----------------------------------------------------------------------------
// Static functions

void bl_fatal_assert_action(void)
{
  reset_resetWithReason(BOOTLOADER_RESET_REASON_TZ_FAULT);
  while (true) ;
}

// Check if non-secure memory is actually non-secure as expected
bool bl_verify_ns_memory_access(const void *p, size_t s)
{
  int flags = CMSE_MPU_READWRITE | CMSE_NONSECURE;
  if (s == 0u) {
    return false;
  }

  if (cmse_check_address_range((void *)p, s, flags) != NULL) {
    return true;
  }

  // The secure memory is accessed
  return false;
}
