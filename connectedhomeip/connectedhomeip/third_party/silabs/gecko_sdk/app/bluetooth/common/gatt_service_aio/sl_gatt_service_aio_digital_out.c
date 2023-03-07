/***************************************************************************//**
 * @file
 * @brief Automation IO GATT service Digital-Out strong implementations
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app_log.h"
#include "sl_simple_led_instances.h"
#include "sli_gatt_service_aio.h"

#if (SL_SIMPLE_LED_COUNT > AIO_DIGITAL_COUNT_MAX)
#error Maximal output count exceeded.
#endif

uint8_t aio_digital_out_get_num(void)
{
  return SL_SIMPLE_LED_COUNT;
}

uint8_t aio_digital_out_get_state(void)
{
  // Read LED states
  uint8_t aio_state = 0;
  sl_led_state_t led_state;

  for (uint8_t i = 0; i < SL_SIMPLE_LED_COUNT; i++) {
    led_state = sl_led_get_state(SL_SIMPLE_LED_INSTANCE(i));
    if (led_state == SL_LED_CURRENT_STATE_ON) {
      aio_state |= AIO_DIGITAL_STATE_ACTIVE << (i * AIO_DIGITAL_STATE_SIZE);
    }
  }
  return aio_state;
}

void aio_digital_out_set_state(uint8_t state)
{
  uint8_t led_state;
  for (uint8_t i = 0; i < SL_SIMPLE_LED_COUNT; i++) {
    led_state = (state >> (i * AIO_DIGITAL_STATE_SIZE)) & AIO_DIGITAL_STATE_MASK;
    if (led_state == AIO_DIGITAL_STATE_ACTIVE) {
      sl_led_turn_on(SL_SIMPLE_LED_INSTANCE(i));
    } else {
      sl_led_turn_off(SL_SIMPLE_LED_INSTANCE(i));
    }
    app_log_info("AIO out: %d=%d" APP_LOG_NL, i, led_state);
  }
}
