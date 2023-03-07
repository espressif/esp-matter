/***************************************************************************//**
 * @file
 * @brief Non-secure simple delay functions.
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

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
// NSC functions

extern void delay_nsc_init(void);
extern void delay_nsc_microseconds(uint32_t usecs);
extern void delay_nsc_milliseconds(uint32_t msecs, bool blocking);
extern bool delay_nsc_expired(void);

// -----------------------------------------------------------------------------
// NS functions

void delay_init(void)
{
  delay_nsc_init();
}

void delay_microseconds(uint32_t usecs)
{
  delay_nsc_microseconds(usecs);
}

void delay_milliseconds(uint32_t msecs, bool blocking)
{
  delay_nsc_milliseconds(msecs, blocking);
}

bool delay_expired(void)
{
  return delay_nsc_expired();
}