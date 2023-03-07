/***************************************************************************//**
 * @file
 * @brief Automation IO GATT service Digital-In strong implementations
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
#include "sl_simple_button_instances.h"
#include "sli_gatt_service_aio.h"

#if (SL_SIMPLE_BUTTON_COUNT > AIO_DIGITAL_COUNT_MAX)
#error Maximal input count exceeded.
#endif

uint8_t aio_digital_in_get_num(void)
{
  return SL_SIMPLE_BUTTON_COUNT;
}

uint8_t aio_digital_in_get_state(void)
{
  // Read button states
  uint8_t aio_state = 0;
  sl_button_state_t btn_state;

  for (uint8_t i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++) {
    btn_state = sl_button_get_state(SL_SIMPLE_BUTTON_INSTANCE(i));
    if (btn_state == SL_SIMPLE_BUTTON_PRESSED) {
      aio_state |= AIO_DIGITAL_STATE_ACTIVE << (i * AIO_DIGITAL_STATE_SIZE);
    }
    app_log_info("AIO in: %d=%d" APP_LOG_NL, i, btn_state);
  }
  return aio_state;
}
