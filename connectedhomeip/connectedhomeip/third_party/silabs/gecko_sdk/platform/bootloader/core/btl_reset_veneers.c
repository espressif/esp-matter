/***************************************************************************//**
 * @file
 * @brief Reset cause signaling veneer functions for Gecko bootloader
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

// -----------------------------------------------------------------------------
// Includes

#include "core/btl_tz_utils.h"
#include "core/btl_reset.h"

// -----------------------------------------------------------------------------
// NSC functions

__attribute__((cmse_nonsecure_entry))
void reset_nsc_resetWithReason(uint16_t resetReason)
{
  reset_resetWithReason(resetReason);
}

__attribute__((cmse_nonsecure_entry))
void reset_nsc_setResetReason(uint16_t resetReason)
{
  reset_setResetReason(resetReason);
}

__attribute__((cmse_nonsecure_entry))
uint16_t reset_nsc_getResetReason(void)
{
  return reset_getResetReason();
}
