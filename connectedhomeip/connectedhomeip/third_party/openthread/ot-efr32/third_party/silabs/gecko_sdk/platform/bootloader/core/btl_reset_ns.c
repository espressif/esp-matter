/***************************************************************************//**
 * @file
 * @brief Non-secure reset cause signaling functions.
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
#include "core/btl_reset_ns.h"

// -----------------------------------------------------------------------------
// NSC functions

extern void reset_nsc_resetWithReason(uint16_t resetReason);
extern void reset_nsc_setResetReason(uint16_t resetReason);
extern uint16_t reset_nsc_getResetReason(void);

// -----------------------------------------------------------------------------
// NS functions

void reset_resetWithReason(uint16_t resetReason)
{
  reset_nsc_resetWithReason(resetReason);
}

void reset_setResetReason(uint16_t resetReason)
{
  reset_setResetReason(resetReason);
}

uint16_t reset_getResetReason(void)
{
  return reset_nsc_getResetReason();
}
