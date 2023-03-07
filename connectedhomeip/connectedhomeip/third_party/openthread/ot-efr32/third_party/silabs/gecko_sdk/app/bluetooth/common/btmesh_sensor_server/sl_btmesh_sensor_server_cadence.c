/***************************************************************************//**
 * @file
 * @brief BT Mesh Sensor Server cadence handler
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// C Standard Library headers
#include <stdio.h>
#include "sl_status.h"
// Bluetooth stack headers
#include "sl_btmesh_api.h"
#include "sl_btmesh_sensor.h"

#include "app_assert.h"
#include "em_common.h"

#include "sl_board_control_config.h"

#ifdef SL_CATALOG_SENSOR_RHT_PRESENT
#include "sl_sensor_rht_config.h"
#include "sl_sensor_rht.h"
#endif // SL_CATALOG_SENSOR_RHT_PRESENT

#ifdef SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT
#include "sl_btmesh_sensor_people_count_config.h"
#include "sl_btmesh_sensor_people_count.h"
#endif // SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT

#include "sl_btmesh_sensor_server.h"
#include "sl_btmesh_sensor_server_cadence.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/// Shift 7 positions
#define STATUS_TRIGGER_TYPE_SHIFT                         7
/// Shift 8 positions
#define FAST_CADENCE_DELTA_SHIFT                          8
/// Length of people count sensor get cadence status buffer
#define SENSOR_PEOPLE_COUNT_GET_CADENCE_BUF_LEN           10
/// Length of thermometer (discrete trigger type) get cadence status buffer
#define SENSOR_THERMOMETER_DISCRETE_GET_CADENCE_BUF_LEN   6
/// Length of thermometer (percentage trigger type) get cadence status buffer
#define SENSOR_THERMOMETER_PERCENTAGE_GET_CADENCE_BUF_LEN 8
/// Length of people count sensor set cadence parameter buffer
#define SENSOR_PEOPLE_COUNT_CADENCE_PARAM_LEN             9
/// Length of thermometer (discrete trigger type) set cadence status buffer
#define SENSOR_THERMOMETER_CADENCE_PERCENTAGE_PARAM_LEN   7
/// Length of thermometer (percentage trigger type) set cadence status buffer
#define SENSOR_THERMOMETER_CADENCE_DISCRETE_PARAM_LEN     5
/// Cadence period divisor parameter maximum value
#define MAX_PERIOD_DIVISOR                                0x0F
/// Cadence trigger type parameter maximum value
#define MAX_TRIGGER_TYPE                                  0x01
/// Cadence minimum interval parameter maximum value
#define MAX_PUBLISHING_MIN_INTERVAL                       0x1A
/// Percentage 100.00%
#define PERCENTAGE_FULL                                   10000

// Define the sensor index
// RHT sensor present and cadence enabled
#if SENSOR_THERMOMETER_CADENCE
#define SENSOR_RHT_INDEX                                  0

// RHT and people count sensor present
#if SENSOR_PEOPLE_COUNT_CADENCE
#define SENSOR_PEOPLE_COUNT_INDEX                         1
#endif // SENSOR_PEOPLE_COUNT_CADENCE

// RHT sensor not present, people count sensor present
#elif SENSOR_PEOPLE_COUNT_CADENCE
#define SENSOR_PEOPLE_COUNT_INDEX                         0
#endif // SENSOR_PEOPLE_COUNT_CADENCE

#if SENSOR_THERMOMETER_CADENCE || SENSOR_PEOPLE_COUNT_CADENCE
// Cadence parameters of supported sensors
static struct sensort_cadence_state cadences[SENSOR_THERMOMETER_CADENCE
                                             + SENSOR_PEOPLE_COUNT_CADENCE];

static int delta_abs (int val);
#endif

#if SENSOR_PEOPLE_COUNT_CADENCE
/// Previously measured people count value
static count16_t prev_people_count_data;

// People count sensor internals
static bool sensor_people_count_fast_cadence(count16_t people_count);
static bool sensor_people_count_delta_cadence(count16_t people_count);
#endif // SENSOR_PEOPLE_COUNT_CADENCE

#if SENSOR_THERMOMETER_CADENCE
/// Previously measured temperature value
static temperature_8_t prev_temp_data;

// Thermometer sensor internals
static bool sensor_thermometer_fast_cadence(temperature_8_t current_temperature);
static bool sensor_thermometer_delta_cadence(temperature_8_t current_temperature);
#endif // SENSOR_THERMOMETER_CADENCE

#if SENSOR_PEOPLE_COUNT_CADENCE

void sl_btmesh_sensor_people_count_cadence_init(count16_t people_count)
{
  static uint16_t delta_down =  SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_DELTA_DOWN_CFG_VAL;
  static uint16_t delta_up =  SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_DELTA_UP_CFG_VAL;

  static uint16_t cadence_low =  SL_BTMESH_SENSOR_PEOPLE_COUNT_FAST_CADENCE_LOW_CFG_VAL;
  static uint16_t cadence_high =  SL_BTMESH_SENSOR_PEOPLE_COUNT_FAST_CADENCE_HIGH_CFG_VAL;

  cadences[SENSOR_PEOPLE_COUNT_INDEX].property_id = PEOPLE_COUNT;
  cadences[SENSOR_PEOPLE_COUNT_INDEX].period_divisor =  SL_BTMESH_SENSOR_PEOPLE_COUNT_FAST_CADENCE_PERIOD_DIVISOR_CFG_VAL;
  cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_type =  SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_TYPE_CFG_VAL;
  cadences[SENSOR_PEOPLE_COUNT_INDEX].min_interval =  SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_MIN_INTERVAL_CFG_VAL;
  cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_low.size = sizeof(cadence_low);
  cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_low.value = (uint8_t*)(&cadence_low);
  cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_high.size = sizeof(cadence_high);
  cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_high.value = (uint8_t*)(&cadence_high);
  cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_down.size = sizeof(delta_down);
  cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_down.value = (uint8_t*)(&delta_down);
  cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_up.size = sizeof(delta_up);
  cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_up.value = (uint8_t*)(&delta_up);

  prev_people_count_data = people_count;
}

uint32_t sl_btmesh_sensor_people_count_handle_cadence(count16_t people_count, sl_btmesh_evt_sensor_server_publish_t publish_period)
{
  uint32_t ret_timer_value = publish_period.period_ms;

  // Check if the measured value requires publishing period modification
  if (sensor_people_count_delta_cadence(people_count)
      || sensor_people_count_fast_cadence(people_count)) {
    // Calculate new publishing timer value
    ret_timer_value = publish_period.period_ms / (1 << cadences[SENSOR_PEOPLE_COUNT_INDEX].period_divisor);
    if (ret_timer_value < (uint32_t)(1 << cadences[SENSOR_PEOPLE_COUNT_INDEX].min_interval)) {
      ret_timer_value = (uint32_t)(1 << cadences[SENSOR_PEOPLE_COUNT_INDEX].min_interval);
    }
  }
  prev_people_count_data = people_count;

  return ret_timer_value;
}

bool sl_btmesh_sensor_people_count_set_cadence(sl_btmesh_evt_sensor_setup_server_set_cadence_request_t* evt)
{
  bool ret_val = false;

  // Check if the received parameters are in valid range
  if ((SENSOR_PEOPLE_COUNT_CADENCE_PARAM_LEN == evt->params.len)
      && (MAX_PERIOD_DIVISOR >= evt->period_divisor)
      && (MAX_TRIGGER_TYPE >= evt->trigger_type)
      && (MAX_PUBLISHING_MIN_INTERVAL >= evt->params.data[4])) {
    ret_val = true;
  }

  // Store incoming people count sensor cadence parameters
  if (true == ret_val) {
    cadences[SENSOR_PEOPLE_COUNT_INDEX].period_divisor = evt->period_divisor;
    cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_type = evt->trigger_type;
    cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_down.value[0] = evt->params.data[0];
    cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_down.value[1] = evt->params.data[1];
    cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_up.value[0] = evt->params.data[2];
    cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_up.value[1] = evt->params.data[3];
    cadences[SENSOR_PEOPLE_COUNT_INDEX].min_interval = evt->params.data[4];
    cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_low.value[0] = evt->params.data[5];
    cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_low.value[1] = evt->params.data[6];
    cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_high.value[0] = evt->params.data[7];
    cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_high.value[1] = evt->params.data[8];
  }

  return ret_val;
}

uint16_t sl_btmesh_sensor_people_count_get_cadence(uint8_t length, uint8_t* get_cadence_buffer)
{
  uint16_t buffer_len = 0;

  if (SENSOR_PEOPLE_COUNT_GET_CADENCE_BUF_LEN <= length) {
    get_cadence_buffer[0] = cadences[SENSOR_PEOPLE_COUNT_INDEX].period_divisor
                            | (cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_type << STATUS_TRIGGER_TYPE_SHIFT);
    get_cadence_buffer[1] = cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_down.value[0];
    get_cadence_buffer[2] = cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_down.value[1];
    get_cadence_buffer[3] = cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_up.value[0];
    get_cadence_buffer[4] = cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_up.value[1];
    get_cadence_buffer[5] = cadences[SENSOR_PEOPLE_COUNT_INDEX].min_interval;
    get_cadence_buffer[6] = cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_low.value[0];
    get_cadence_buffer[7] = cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_low.value[1];
    get_cadence_buffer[8] = cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_high.value[0];
    get_cadence_buffer[9] = cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_high.value[1];
    buffer_len = SENSOR_PEOPLE_COUNT_GET_CADENCE_BUF_LEN;
  }

  return buffer_len;
}

/***************************************************************************//**
 * Check if the measured value requires publishing period modification.
 *
 * @param[in] people_count    People count sensor data value
 *
 * @return True if publishing period modification is required false otherwise
 ******************************************************************************/
static bool sensor_people_count_fast_cadence(count16_t people_count)
{
  bool ret_val = false;
  count16_t fast_cadence_high = (count16_t)(cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_high.value[0]
                                            | (cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_high.value[1] << FAST_CADENCE_DELTA_SHIFT));
  count16_t fast_cadence_low  = (count16_t)(cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_low.value[0]
                                            | (cadences[SENSOR_PEOPLE_COUNT_INDEX].fast_cadence_low.value[1] << FAST_CADENCE_DELTA_SHIFT));

  // Check if the Fast Cadence High value is equal or higher than the Fast Cadence Low value
  if (fast_cadence_high >= fast_cadence_low) {
    // If the measured value is within the closed interval of
    // [Fast Cadence Low, Fast Cadence High], Sensor Status publishing period
    // modification is required.
    if ((people_count >= fast_cadence_low)
        && (people_count <= fast_cadence_high)) {
      ret_val = true;
    }
    // Check if the Fast Cadence High value is lower than the Fast Cadence Low value
  } else {
    // If the measured value is lower than the Fast Cadence High value or is
    // higher than the Fast Cadence Low value, Sensor Status publishing period
    // modification is required.
    if ((people_count < fast_cadence_high)
        || (people_count > fast_cadence_low)) {
      ret_val = true;
    }
  }

  return ret_val;
}

/***************************************************************************//**
 * Check if the change of the measured value requires publishing period modification.
 *
 * @param[in] people_count    People count sensor data value
 *
 * @return True if publishing period modification is required false otherwise
 ******************************************************************************/
static bool sensor_people_count_delta_cadence(count16_t people_count)
{
  bool ret_val = false;
  uint16_t delta_percent;
  uint16_t delta_cadence_up = (uint16_t)(cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_up.value[0]
                                         | (cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_up.value[1] << FAST_CADENCE_DELTA_SHIFT));
  uint16_t delta_cadence_down = (uint16_t)(cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_down.value[0]
                                           | (cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_delta_down.value[1] << FAST_CADENCE_DELTA_SHIFT));

  // Check the unit and format of the Status Trigger Delta Down and
  // the Status Trigger Delta Up fields.
  if (SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_TYPE_DISCRETE_VALUE_CFG_VAL == cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_type) {
    // If the temperature change is rising and the measured quantity change
    // exceeds the configured Status Trigger Delta Up value, Sensor Status
    // publishing period modification is required.
    if ((people_count > prev_people_count_data)
        && ((people_count - prev_people_count_data) >= (count16_t)delta_cadence_up)) {
      ret_val = true;
      // If the temperature change is falling and the measured quantity change
      // exceeds the configured Status Trigger Delta Down value, Sensor Status
      // publishing period modification is required.
    } else if ((people_count < prev_people_count_data)
               && ((prev_people_count_data - people_count) >= (count16_t)delta_cadence_down)) {
      ret_val = true;
    } else {
    }
    // Same check with measured value represented unitless as percentage
  } else if ((SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_TYPE_PERCENTAGE_CFG_VAL == cadences[SENSOR_PEOPLE_COUNT_INDEX].status_trigger_type)
             && (prev_people_count_data != people_count)) {
    if (!prev_people_count_data) {
      delta_percent = PERCENTAGE_FULL;
    } else {
      delta_percent = delta_abs((delta_abs(prev_people_count_data - people_count) * PERCENTAGE_FULL) / prev_people_count_data);
    }

    if ((people_count > prev_people_count_data) && (delta_percent >= delta_cadence_up)) {
      ret_val = true;
    } else if ((people_count < prev_people_count_data) && (delta_percent >= delta_cadence_down)) {
      ret_val = true;
    } else {
    }
  } else {
  }

  return ret_val;
}

#endif // SENSOR_PEOPLE_COUNT_CADENCE

#if SENSOR_THERMOMETER_CADENCE

void sl_btmesh_sensor_thermometer_cadence_init(temperature_8_t temperature)
{
  static uint16_t delta_down =  SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_DOWN;
  static uint16_t delta_up =  SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_UP;

  static uint8_t cadence_low =  SENSOR_THERMOMETER_FAST_CADENCE_LOW;
  static uint8_t cadence_high =  SENSOR_THERMOMETER_FAST_CADENCE_HIGH;

  cadences[SENSOR_RHT_INDEX].property_id = PRESENT_AMBIENT_TEMPERATURE;
  cadences[SENSOR_RHT_INDEX].period_divisor =  SENSOR_THERMOMETER_FAST_CADENCE_PERIOD_DIVISOR;
  cadences[SENSOR_RHT_INDEX].status_trigger_type =  SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE;
  cadences[SENSOR_RHT_INDEX].min_interval =  SENSOR_THERMOMETER_STATUS_MIN_INTERVAL;
  cadences[SENSOR_RHT_INDEX].fast_cadence_low.size = sizeof(cadence_low);
  cadences[SENSOR_RHT_INDEX].fast_cadence_low.value = (uint8_t*)(&cadence_low);
  cadences[SENSOR_RHT_INDEX].fast_cadence_high.size = sizeof(cadence_high);
  cadences[SENSOR_RHT_INDEX].fast_cadence_high.value = (uint8_t*)(&cadence_high);
  cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.value = (uint8_t*)(&delta_down);
  cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.value = (uint8_t*)(&delta_up);

  if (SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_DISCRETE_VALUE == cadences[SENSOR_RHT_INDEX].status_trigger_type) {
    cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.size = sizeof(uint8_t);
    cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.size = sizeof(uint8_t);
  } else {
    cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.size = sizeof(delta_down);
    cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.size = sizeof(delta_up);
  }

  prev_temp_data = temperature;
}

uint32_t sl_btmesh_sensor_thermometer_handle_cadence(temperature_8_t temperature, sl_btmesh_evt_sensor_server_publish_t publish_period)
{
  uint32_t ret_timer_value = publish_period.period_ms;

  // Check if the measured value requires publishing period modification
  if (sensor_thermometer_delta_cadence(temperature)
      || sensor_thermometer_fast_cadence(temperature)) {
    // Calculate publishing timer value
    ret_timer_value = publish_period.period_ms / (1 << cadences[SENSOR_RHT_INDEX].period_divisor);
    if (ret_timer_value < (uint32_t)(1 << cadences[SENSOR_RHT_INDEX].min_interval)) {
      ret_timer_value = (uint32_t)(1 << cadences[SENSOR_RHT_INDEX].min_interval);
    }
  }
  prev_temp_data = temperature;

  return ret_timer_value;
}

bool sl_btmesh_sensor_thermometer_set_cadence(sl_btmesh_evt_sensor_setup_server_set_cadence_request_t* evt)
{
  bool ret_val = false;

  // Check if the received parameters are in valid range
  if ((MAX_PERIOD_DIVISOR >= evt->period_divisor)
      && (MAX_TRIGGER_TYPE >= evt->trigger_type)) {
    if ((SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_PERCENTAGE == evt->trigger_type)
        && (SENSOR_THERMOMETER_CADENCE_PERCENTAGE_PARAM_LEN == evt->params.len)
        && (MAX_PUBLISHING_MIN_INTERVAL >= evt->params.data[4])) {
      ret_val = true;
    } else if ((SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_DISCRETE_VALUE == evt->trigger_type)
               && (SENSOR_THERMOMETER_CADENCE_DISCRETE_PARAM_LEN == evt->params.len)
               && (MAX_PUBLISHING_MIN_INTERVAL >= evt->params.data[2])) {
      ret_val = true;
    } else {
    }
  }

  // store incoming thermometer sensor cadence parameters
  if (true == ret_val) {
    if ( SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_PERCENTAGE == evt->trigger_type) {
      cadences[SENSOR_RHT_INDEX].period_divisor = evt->period_divisor;
      cadences[SENSOR_RHT_INDEX].status_trigger_type = evt->trigger_type;
      cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.value[0] = evt->params.data[1];
      cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.value[1] = evt->params.data[0];
      cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.value[0] = evt->params.data[3];
      cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.value[1] = evt->params.data[2];
      cadences[SENSOR_RHT_INDEX].min_interval = evt->params.data[4];
      cadences[SENSOR_RHT_INDEX].fast_cadence_low.value[0] = evt->params.data[5];
      cadences[SENSOR_RHT_INDEX].fast_cadence_high.value[0] = evt->params.data[6];
      // In case of SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_DISCRETE_VALUE trigger type
    } else {
      cadences[SENSOR_RHT_INDEX].period_divisor = evt->period_divisor;
      cadences[SENSOR_RHT_INDEX].status_trigger_type = evt->trigger_type;
      cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.value[0] = evt->params.data[0];
      cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.value[0] = evt->params.data[1];
      cadences[SENSOR_RHT_INDEX].min_interval = evt->params.data[2];
      cadences[SENSOR_RHT_INDEX].fast_cadence_low.value[0] = evt->params.data[3];
      cadences[SENSOR_RHT_INDEX].fast_cadence_high.value[0] = evt->params.data[4];
    }
  }

  return ret_val;
}

uint16_t sl_btmesh_sensor_thermometer_get_cadence(uint8_t length, uint8_t* get_cadence_buffer)
{
  uint16_t buffer_len = 0;

  if ((SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_PERCENTAGE == cadences[SENSOR_RHT_INDEX].status_trigger_type)
      && (SENSOR_THERMOMETER_PERCENTAGE_GET_CADENCE_BUF_LEN <= length)) {
    get_cadence_buffer[0] = cadences[SENSOR_RHT_INDEX].period_divisor
                            | (cadences[SENSOR_RHT_INDEX].status_trigger_type << STATUS_TRIGGER_TYPE_SHIFT);
    get_cadence_buffer[1] = cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.value[1];
    get_cadence_buffer[2] = cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.value[0];
    get_cadence_buffer[3] = cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.value[1];
    get_cadence_buffer[4] = cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.value[0];
    get_cadence_buffer[5] = cadences[SENSOR_RHT_INDEX].min_interval;
    get_cadence_buffer[6] = cadences[SENSOR_RHT_INDEX].fast_cadence_low.value[0];
    get_cadence_buffer[7] = cadences[SENSOR_RHT_INDEX].fast_cadence_high.value[0];
    buffer_len = SENSOR_THERMOMETER_PERCENTAGE_GET_CADENCE_BUF_LEN;
  } else if ((SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_DISCRETE_VALUE == cadences[SENSOR_RHT_INDEX].status_trigger_type)
             && (SENSOR_THERMOMETER_DISCRETE_GET_CADENCE_BUF_LEN <= length)) {
    get_cadence_buffer[0] = cadences[SENSOR_RHT_INDEX].period_divisor
                            | (cadences[SENSOR_RHT_INDEX].status_trigger_type << STATUS_TRIGGER_TYPE_SHIFT);
    get_cadence_buffer[1] = cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.value[0];
    get_cadence_buffer[2] = cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.value[0];
    get_cadence_buffer[3] = cadences[SENSOR_RHT_INDEX].min_interval;
    get_cadence_buffer[4] = cadences[SENSOR_RHT_INDEX].fast_cadence_low.value[0];
    get_cadence_buffer[5] = cadences[SENSOR_RHT_INDEX].fast_cadence_high.value[0];
    buffer_len = SENSOR_THERMOMETER_DISCRETE_GET_CADENCE_BUF_LEN;
  } else {
  }

  return buffer_len;
}

/***************************************************************************//**
 * Check if the measured value requires publishing period modification.
 *
 * @param[in] temperature     Thermometer sensor data value.
 *
 * @return True if modification is required false otherwise
 ******************************************************************************/
static bool sensor_thermometer_fast_cadence(temperature_8_t temperature)
{
  bool ret_val = false;

  // Check if the Fast Cadence High value is equal or higher than the Fast Cadence Low value
  if ((temperature_8_t)(cadences[SENSOR_RHT_INDEX].fast_cadence_high.value[0])
      >= (temperature_8_t)(cadences[SENSOR_RHT_INDEX].fast_cadence_low.value[0])) {
    // If the measured value is within the closed interval of
    // [Fast Cadence Low, Fast Cadence High], Sensor Status publishing period
    // modification is required.
    if ((temperature >= (temperature_8_t)(cadences[SENSOR_RHT_INDEX].fast_cadence_low.value[0]))
        && (temperature <= (temperature_8_t)(cadences[SENSOR_RHT_INDEX].fast_cadence_high.value[0]))) {
      ret_val = true;
    }
    // If the Fast Cadence High value is lower than the Fast Cadence Low value
  } else {
    // If the measured value is lower than the Fast Cadence High value or is
    // higher than the Fast Cadence Low value, Sensor Status publishing period
    // modification is required.
    if ((temperature < (temperature_8_t)(cadences[SENSOR_RHT_INDEX].fast_cadence_high.value[0]))
        || (temperature > (temperature_8_t)(cadences[SENSOR_RHT_INDEX].fast_cadence_low.value[0]))) {
      ret_val = true;
    }
  }

  return ret_val;
}

/***************************************************************************//**
 * Check if the change of the measured value requires publishing period modification.
 *
 * @param[in] temperature     Thermometer sensor data value.
 *
 * @return True if modification is required false otherwise
 ******************************************************************************/
static bool sensor_thermometer_delta_cadence(temperature_8_t temperature)
{
  bool ret_val = false;
  uint16_t delta_percent;
  uint16_t percent_delta_up_value;
  uint16_t percent_delta_down_value;

  // Check the unit and format of the Status Trigger Delta Down and
  // the Status Trigger Delta Up fields.
  if (SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_DISCRETE_VALUE == cadences[SENSOR_RHT_INDEX].status_trigger_type) {
    // If the temperature change is rising edge and the measured quantity change
    // exceeds the configured Status Trigger Delta Up value, the Sensor Status
    // messages are published more frequently.
    if ((temperature > prev_temp_data)
        && ((temperature - prev_temp_data) >= (temperature_8_t)(cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.value[0]))) {
      ret_val = true;
      // If the temperature change is rising edge and the measured quantity change
      // exceeds the configured Status Trigger Delta Up value, the Sensor Status
      // messages are published more frequently.
    } else if ((temperature < prev_temp_data)
               && ((prev_temp_data - temperature) >= (temperature_8_t)(cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.value[0]))) {
      ret_val = true;
    } else {
    }
    // Same check with measured value represented unitless as percentage
  } else if ((SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_PERCENTAGE == cadences[SENSOR_RHT_INDEX].status_trigger_type)
             && (prev_temp_data != temperature)) {
    percent_delta_down_value = (uint16_t)(cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.value[0]
                                          | (cadences[SENSOR_RHT_INDEX].status_trigger_delta_down.value[1] << FAST_CADENCE_DELTA_SHIFT));
    percent_delta_up_value = (uint16_t)(cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.value[0]
                                        | (cadences[SENSOR_RHT_INDEX].status_trigger_delta_up.value[1] << FAST_CADENCE_DELTA_SHIFT));

    if (!prev_temp_data) {
      delta_percent = PERCENTAGE_FULL;
    } else {
      delta_percent = delta_abs((delta_abs(prev_temp_data - temperature) * PERCENTAGE_FULL) / prev_temp_data);
    }

    if ((temperature > prev_temp_data) && (delta_percent >= percent_delta_up_value)) {
      ret_val = true;
    } else if ((temperature < prev_temp_data) && (delta_percent >= percent_delta_down_value)) {
      ret_val = true;
    } else {
    }
  } else {
  }

  return ret_val;
}

#endif // SENSOR_THERMOMETER_CADENCE

#if SENSOR_THERMOMETER_CADENCE || SENSOR_PEOPLE_COUNT_CADENCE
// Return the absolute value of val.
static int delta_abs(int val)
{
  return val < 0 ? -val : val;
}
#endif
