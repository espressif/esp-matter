/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef __SL_WISUN_LED_DRIVER_H__
#define __SL_WISUN_LED_DRIVER_H__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "sl_status.h"
#include <inttypes.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// LED ID enum typedef
typedef enum sl_wisun_led_id {
  /// LED ID Unknown
  SL_WISUN_LED_UNKNOW_ID = 0,
  /// LED0 ID
  SL_WISUN_LED0_ID       = 1,
  /// LED1 ID
  SL_WISUN_LED1_ID       = 2,
} sl_wisun_led_id_t;

/// Continuous signal id for period setting
#define SL_WISUN_LED_PERIOD_CONTINOUS_SIGNAL      (-1)

/// LED signal typedef
typedef struct sl_wisun_led_signal {
  /// LED ID
  uint8_t id;
  /// High state time in millisec
  uint32_t high_ms;
  /// Low state time in millisec
  uint32_t low_ms;
  /// Period count, how many times will be repeated the H and L state.
  /// 'SL_WISUN_LED_PERIOD_CONTINOUS_SIGNAL' continuous repeat
  int32_t period;
} sl_wisun_led_signal_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Init LED driver component.
 * @details init thread and message queue
 *****************************************************************************/
void sl_wisun_led_driver_init(void);

/**************************************************************************//**
 * @brief Set LED signal.
 * @details The argument signal structure will be put the message queue
 * @param[in] led_signal LED signal structure
 *****************************************************************************/
void sl_wisun_led_set(const sl_wisun_led_signal_t *led_signal);

/**************************************************************************//**
 * @brief Clear LED by ID.
 * @details Send an empty signal which is filled with 0
 * @param led_id LED id
 *****************************************************************************/
void sl_wisun_led_clear(const sl_wisun_led_id_t led_id);

/**************************************************************************//**
 * @brief Get LED signal structure.
 * @details Create thread safe copy of the signal structure
 * @param[in] led_id LED ID
 * @param[out] dest Destination structure
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_wisun_led_get_signal(const sl_wisun_led_id_t led_id,
                                    sl_wisun_led_signal_t * const dest);

/**************************************************************************//**
 * @brief LED toggle.
 * @details Toggle the particular led with continuous signal
 * @param[in] led_id LED ID
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_wisun_led_toggle(const sl_wisun_led_id_t led_id);

/**************************************************************************//**
 * @brief Terminate the LED driver.
 * @details Terminate the thread with OS API
 * @return SL_STATUS_OK on success, SL_STATUS_FAIL on failure
 *****************************************************************************/
sl_status_t sl_wisun_led_terminate(void);

#endif
