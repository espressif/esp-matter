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

#ifndef BTL_RESET_NS_H
#define BTL_RESET_NS_H

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

// -----------------------------------------------------------------------------
// Includes

#include <inttypes.h>

// -----------------------------------------------------------------------------
// Function declarations

// Reset from the bootloader with a reset cause.
void reset_resetWithReason(uint16_t resetReason);

// Set a reset reason.
void reset_setResetReason(uint16_t resetReason);

// Get the reset reason without verifying it.
uint16_t reset_getResetReason(void);

/** @endcond */

#endif // BTL_RESET_NS_H