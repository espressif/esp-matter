/***************************************************************************//**
 * @file
 * @brief Reset cause signalling for the Silicon Labs bootloader
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef BTL_RESET_H
#define BTL_RESET_H

#include "core/btl_util.h"

MISRAC_DISABLE
#include "em_common.h"
MISRAC_ENABLE

/***************************************************************************//**
 * @addtogroup Core Bootloader Core
 * @{
 * @addtogroup Reset
 * @brief Methods to reset from the bootloader to the app
 * @details
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Reset from the bootloader with a reset cause.
 *
 * @note This function does not return.
 *
 * @param resetReason A reset reason as defined in
 *                    [the bootloader interface](@ref ResetInterface)
 ******************************************************************************/
void reset_resetWithReason(uint16_t resetReason);

/***************************************************************************//**
 * Set a reset reason.
 *
 * @param resetReason A reset reason as defined in
 *                    [the bootloader interface](@ref ResetInterface)
 ******************************************************************************/
void reset_setResetReason(uint16_t resetReason);

/***************************************************************************//**
 * Use the lower 4 bits of the reset cause signature to store a reset counter
 * able to count up to 15 before wrapping around. The value of the counter
 * will be preserved across resets.
 ******************************************************************************/
void reset_enableResetCounter(void);

/***************************************************************************//**
 * Clear and disable the reset counter.
 ******************************************************************************/
void reset_disableResetCounter(void);

/***************************************************************************//**
 * Check whether the reset counter is enabled.
 *
 * @return True if reset counter is enabled, else false.
 ******************************************************************************/
bool reset_resetCounterEnabled(void);

/***************************************************************************//**
 * Increment the reset counter by one.
 *
 * Wraps around to 0 if the current counter value is equal to 15.
 ******************************************************************************/
void reset_incrementResetCounter(void);

/***************************************************************************//**
 * Get the reset counter value.
 *
 * @note The reset counter has to be enabled for this value to be valid.
 *
 * @return The reset counter value
 ******************************************************************************/
uint8_t reset_getResetCounter(void);

/***************************************************************************//**
 * Get the reset reason without verifying it.
 *
 * @return The reset reason
 ******************************************************************************/
uint16_t reset_getResetReason(void);

/***************************************************************************//**
 * Invalidate the reset reason.
 *
 * @note This will also disable the reset counter if it is in use.
 ******************************************************************************/
void reset_invalidateResetReason(void);

/***************************************************************************//**
 * Classify reset and get the reset reason.
 *
 * @return Reset cause or @ref BOOTLOADER_RESET_REASON_UNKNOWN
 ******************************************************************************/
uint16_t reset_classifyReset(void);

/** @} addtogroup reset */
/** @} addtogroup core */

#endif // BTL_RESET_H
