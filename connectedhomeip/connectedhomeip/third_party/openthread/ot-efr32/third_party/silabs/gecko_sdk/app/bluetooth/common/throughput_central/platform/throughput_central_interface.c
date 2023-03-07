/***************************************************************************//**
 * @file
 * @brief Throughput test application - platform implementation
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
#include "sl_simple_timer.h"
#include "throughput_central_interface.h"
#include "throughput_types.h"
#include "app_assert.h"
#include "sl_sleeptimer.h"

/// Time storage variable
static throughput_count_t time_storage = 0;

/// RSSI refresh timer
static sl_simple_timer_t refresh_timer;

static void refresh_timer_callback(sl_simple_timer_t *timer,
                                   void *data)
{
  (void)timer;
  (void)data;
  timer_on_refresh_rssi();
}

/**************************************************************************//**
 * ASCII graphics for indicating wait status
 *****************************************************************************/
void waiting_indication(void)
{
  // Do nothing
}

/**************************************************************************//**
 * Start timer
 *****************************************************************************/
void timer_start()
{
  time_storage = sl_sleeptimer_get_tick_count64();
}

/**************************************************************************//**
 * Timer end. The return value of this function shall be the time passed
 * form the timer_start() call in seconds.
 *****************************************************************************/
float timer_end()
{
  return ((float)(sl_sleeptimer_get_tick_count64() - time_storage)
          / (float)sl_sleeptimer_get_timer_frequency());
}

/**************************************************************************//**
 * Start RSSI refresh timer
 *****************************************************************************/
void timer_refresh_rssi_start(void)
{
  // Start refresh timer
  sl_status_t sc;
  sc = sl_simple_timer_start(&refresh_timer,
                             THROUGHPUT_CENTRAL_REFRESH_TIMER_PERIOD,
                             refresh_timer_callback,
                             NULL,
                             true);
  app_assert_status(sc);
}

/**************************************************************************//**
 * Stop RSSI refresh timer
 *****************************************************************************/
void timer_refresh_rssi_stop(void)
{
  // Stop refresh timer
  sl_status_t sc;
  sc = sl_simple_timer_stop(&refresh_timer);
  app_assert_status(sc);
}
