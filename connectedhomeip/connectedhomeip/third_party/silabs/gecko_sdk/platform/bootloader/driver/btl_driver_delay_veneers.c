/***************************************************************************//**
 * @file
 * @brief Simple delay veneer functions.
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
#include "driver/btl_driver_delay.h"

// -----------------------------------------------------------------------------
// NSC functions

__attribute__((cmse_nonsecure_entry))
void delay_nsc_microseconds(uint32_t usecs)
{
  delay_microseconds(usecs);
}

__attribute__((cmse_nonsecure_entry))
void delay_nsc_init(void)
{
  delay_init();
}

__attribute__((cmse_nonsecure_entry))
void delay_nsc_milliseconds(uint32_t msecs, bool blocking)
{
  delay_milliseconds(msecs, blocking);
}

__attribute__((cmse_nonsecure_entry))
bool delay_nsc_expired(void)
{
  return delay_expired();
}