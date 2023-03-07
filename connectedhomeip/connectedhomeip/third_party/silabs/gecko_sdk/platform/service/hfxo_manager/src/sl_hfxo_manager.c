/***************************************************************************//**
 * @file
 * @brief HFXO Manager API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_device.h"
#include "sl_hfxo_manager.h"
#include "sli_hfxo_manager.h"
#include "sli_hfxo_manager_internal.h"
#include "sl_sleeptimer.h"
#include "sl_assert.h"
#include "sl_status.h"
#include <stdbool.h>

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/
// Table size of HFXO wake-up time measurement
#define HFXO_STARTUP_TIME_TABLE_SIZE  10

// Default time value in microseconds required to wake-up the hfxo oscillator.
#define HFXO_STARTUP_TIME_DEFAULT_VALUE_US  (600u)

/*******************************************************************************
 *****************************   DATA TYPES   **********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

// Time in ticks required for HFXO start-up after wake-up from sleep.
static uint32_t hfxo_startup_time_tick = 0;

static uint32_t hfxo_last_startup_time = 0;

static uint32_t hfxo_startup_time_table[HFXO_STARTUP_TIME_TABLE_SIZE];

static uint8_t hfxo_startup_time_table_index = 0;

static uint32_t hfxo_startup_time_sum_average = 0;

static uint32_t hfxo_startup_time_tc_inital = 0;

static bool hfxo_measurement_on = false;

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * HFXO Manager module hardware specific initialization.
 ******************************************************************************/
void sl_hfxo_manager_init_hardware(void)
{
  sli_hfxo_manager_init_hardware();
}

/***************************************************************************//**
 * Initialize HFXO Manager module.
 ******************************************************************************/
sl_status_t sl_hfxo_manager_init(void)
{
  sl_status_t status;

  // Initialize Sleeptimer module in case not already done.
  status = sl_sleeptimer_init();
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Set HFXO startup time to conservative default value
  hfxo_startup_time_tick = (((HFXO_STARTUP_TIME_DEFAULT_VALUE_US * sl_sleeptimer_get_timer_frequency()) + (1000000 - 1)) / 1000000);
  for (uint8_t i = 0; i < HFXO_STARTUP_TIME_TABLE_SIZE; i++) {
    hfxo_startup_time_table[i] = hfxo_startup_time_tick;
    hfxo_startup_time_sum_average += hfxo_startup_time_tick;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Updates Sleepy Crystal settings.
 *
 * @param  settings  Pointer to settings structure
 *
 * @return Status Code.
 *
 * @note Those settings are temporarily used to force oscillation on sleepy
 *       crystal.
 *       Default values should be enough to wake-up sleepy crystals. Otherwise,
 *       this function can be used.
 ******************************************************************************/
sl_status_t sl_hfxo_manager_update_sleepy_xtal_settings(sl_hfxo_manager_sleepy_xtal_settings_t *settings)
{
  return sli_hfxo_manager_update_sleepy_xtal_settings_hardware(settings);
}

/***************************************************************************//**
 * When this callback function is called, it means that HFXO failed twice in
 * a row to start with normal configurations. This may mean that there is a
 * bad crystal. When getting this callback, HFXO is running but its properties
 * (frequency, precision) are not guaranteed. This should be considered as an
 * error situation.
 ******************************************************************************/
__WEAK void sl_hfxo_manager_notify_consecutive_failed_startups(void)
{
  EFM_ASSERT(false);
}

/*******************************************************************************
 **********************   GLOBAL INTERNAL FUNCTIONS   **************************
 ******************************************************************************/

/***************************************************************************//**
 * Function to call just before starting HFXO, to save current tick count.
 ******************************************************************************/
void sli_hfxo_manager_begin_startup_measurement(void)
{
  hfxo_measurement_on = true;
  hfxo_startup_time_tc_inital = sl_sleeptimer_get_tick_count();
}

/***************************************************************************//**
 * Function to call just after HFXO becomes ready, to save current tick count
 * and calculate HFXO startup time.
 ******************************************************************************/
void sli_hfxo_manager_end_startup_measurement(void)
{
  if (hfxo_measurement_on == false) {
    return;
  }

  // Complete HFXO restore time measurement
  hfxo_last_startup_time = sl_sleeptimer_get_tick_count() - hfxo_startup_time_tc_inital;

  // With low precision clock, the HFXO startup time measure could be zero.
  // In that case, ensure it's a least 1 tick.
  hfxo_last_startup_time = (hfxo_last_startup_time == 0) ? 1 : hfxo_last_startup_time;

  // Calculate average for HFXO restore time
  hfxo_startup_time_sum_average -= (int32_t)hfxo_startup_time_table[hfxo_startup_time_table_index] - (int32_t)hfxo_last_startup_time;
  hfxo_startup_time_table[hfxo_startup_time_table_index] = hfxo_last_startup_time;
  hfxo_startup_time_tick = ((hfxo_startup_time_sum_average + (HFXO_STARTUP_TIME_TABLE_SIZE - 1) ) / HFXO_STARTUP_TIME_TABLE_SIZE);

  // Update index of wakeup time table
  hfxo_startup_time_table_index++;
  hfxo_startup_time_table_index %= HFXO_STARTUP_TIME_TABLE_SIZE;

  hfxo_measurement_on = false;
}

/***************************************************************************//**
 * Retrieves HFXO startup time average value.
 *
 * @return  HFXO startup time average value.
 ******************************************************************************/
uint32_t sli_hfxo_manager_get_startup_time(void)
{
  return hfxo_startup_time_tick;
}

/***************************************************************************//**
 * Retrieves HFXO startup time latest value.
 *
 * @return  HFXO startup time latest value.
 ******************************************************************************/
uint32_t sli_hfxo_manager_get_latest_startup_time(void)
{
  return hfxo_last_startup_time;
}
