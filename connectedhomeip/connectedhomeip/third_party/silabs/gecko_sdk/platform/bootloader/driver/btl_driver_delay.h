/***************************************************************************//**
 * @file
 * @brief Hardware driver layer for simple delay.
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

#ifndef BTL_DRIVER_DELAY_H
#define BTL_DRIVER_DELAY_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup Driver
 * @brief Hardware drivers for bootloader
 * @{
 * @addtogroup Delay
 * @brief Basic delay functionality
 * @details Simple delay routines for use with components that require small
 *          delays.
 * @{
 */

/**
 * Delay for a number of microseconds.
 *
 * @note This function can be used without calling delay_init() first. This is not an accurate microsecond
 *       delay and can have a delay greater than expected, of about +50%. This error becomes significant
 *       for lower values delay.
 *
 * @param usecs Number of microseconds to delay
 */
void delay_microseconds(uint32_t usecs);

/**
 * Initialize the delay driver's millisecond counter.
 */
void delay_init(void);

/**
 * Delay for a number of milliseconds.
 *
 * @param msecs    Number of milliseconds to delay. The number should stay within
 *                 a single TIMER0 overflow (approx. 3300 ms).
 * @param blocking Whether to block until the delay has expired. If false, the
 *                 @ref delay_expired() function can be called to check whether
 *                 the delay has expired.
 */
void delay_milliseconds(uint32_t msecs, bool blocking);

/**
 * Check whether the current delay has expired.
 *
 * @return True if the delay has expired.
 */
bool delay_expired(void);

/**
 * @} (end addtogroup Delay)
 * @} (end addtogroup Driver)
 */

#endif // BTL_DRIVER_DELAY_H
