/***************************************************************************//**
 * @file
 * @brief BT Mesh Sensor Server Instances
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

// C Standard Library headers
#include <stdio.h>
#include <math.h>
#include "sl_status.h"
// Bluetooth stack headers
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_sensor.h"
#include "sl_btmesh_dcd.h"
#include "sl_simple_timer.h"

#include "app_assert.h"
#include "em_common.h"

#include "sl_board_control_config.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

#ifdef SL_CATALOG_SENSOR_RHT_PRESENT
#include "sl_sensor_rht_config.h"
#include "sl_sensor_rht.h"
#endif // SL_CATALOG_SENSOR_RHT_PRESENT

#ifdef SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT
#include "sl_btmesh_sensor_people_count_config.h"
#include "sl_btmesh_sensor_people_count.h"
#endif // SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT

#ifdef SL_CATALOG_SENSOR_LIGHT_PRESENT
#include "sl_sensor_light_config.h"
#include "sl_sensor_light.h"
#elif defined(SL_CATALOG_SENSOR_LUX_PRESENT)
#include "sl_sensor_lux_config.h"
#include "sl_sensor_lux.h"
#endif

#include "sl_btmesh_sensor_server.h"
#include "sl_btmesh_sensor_server_config.h"
#include "sl_btmesh_sensor_server_cadence.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup Sensor
 * @{
 ******************************************************************************/

#define SENSOR_SERVER_SEND_FAILED_TEXT "Sensor server send %s failed\r\n"
#define SENSOR_SETUP_SERVER_SEND_FAILED_TEXT "Sensor setup server send %s failed\r\n"

/// The unused 0 address is used for publishing
#define PUBLISH_TO_ALL_NODES    0
/// Parameter ignored for publishing
#define IGNORED                 0
/// No flags used for message
#define NO_FLAGS                0
/// Pre-scale value for temperature sensor raw data
#define TEMPERATURE_PRE_SCALE   2
/// Offset value for temperature sensor pre-scaled value
#define TEMPERATURE_OFFSET      499
/// Scale value for temperature sensor final value
#define TEMPERATURE_SCALE_VAL   1000
/// Length of sensor data buffer
#define SENSOR_DATA_BUF_LEN     15
/// Property ID indicating reading every sensor
#define PROPERTY_ID_ALL         0
/// Buffer length for get cadence parameters
#define SENSOR_CADENCE_BUF_LEN  10
/// Acknowledgement request mask
#define SET_CADENCE_ACK_FLAG    2
/// Callback has no parameters
#define NO_CALLBACK_DATA        (void *)NULL
/// Multiplier for seconds conversion to millisenconds
#define SEC_TO_MS               1000
/// Sensor Update Interval formula power extractor constant
#define EXTRACTOR_CONSTANT      64

#define MIN(a, b)               (((a) < (b)) ? (a) : (b))

#ifdef SL_CATALOG_SENSOR_RHT_PRESENT
temperature_8_t get_temperature(void);
bool            rht_initialized;
#endif // SL_CATALOG_SENSOR_RHT_PRESENT

#if defined(SL_CATALOG_SENSOR_LIGHT_PRESENT) \
  || defined(SL_CATALOG_SENSOR_LUX_PRESENT)
illuminance_t get_light(void);
#endif //SL_CATALOG_SENSOR_LIGHT_PRESENT || SL_CATALOG_SENSOR_LUX_PRESENT

#if SENSOR_THERMOMETER_CADENCE || SENSOR_PEOPLE_COUNT_CADENCE
static uint32_t prev_publish_timeout;
static sl_btmesh_evt_sensor_server_publish_t publish_period;

// -------------------------------
// Periodic timer handles
static sl_simple_timer_t sensor_server_data_timer;
static sl_simple_timer_t sensor_server_publish_timer;

// -------------------------------
// Periodic timer callbacks
static void sensor_server_data_timer_cb(sl_simple_timer_t *handle, void *data);
static void sensor_server_publish_timer_cb(sl_simple_timer_t *handle, void *data);
#endif

void sl_btmesh_sensor_server_node_init(void)
{
  /// Descriptors of supported sensors
  /* The following properties are defined
   * 1. People count property (property ID: 0x004C)
   * 2. Present ambient light property (property ID: 0x004E)
   * 3. Present ambient temperature property (property ID: 0x004F)
   * NOTE: the properties must be ordered in ascending order by property ID
   */
  static const sensor_descriptor_t descriptors[] = {
#ifdef SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT
    {
      .property_id = PEOPLE_COUNT,
      .positive_tolerance = SL_BTMESH_SENSOR_PEOPLE_COUNT_POSITIVE_TOLERANCE_CFG_VAL,
      .negative_tolerance = SL_BTMESH_SENSOR_PEOPLE_COUNT_NEGATIVE_TOLERANCE_CFG_VAL,
      .sampling_function = SL_BTMESH_SENSOR_PEOPLE_COUNT_SAMPLING_FUNCTION_CFG_VAL,
      .measurement_period = SL_BTMESH_SENSOR_PEOPLE_COUNT_MEASUREMENT_PERIOD_CFG_VAL,
      .update_interval = SL_BTMESH_SENSOR_PEOPLE_COUNT_UPDATE_INTERVAL_CFG_VAL
    },
#endif // SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT
#if defined(SL_BOARD_ENABLE_SENSOR_LIGHT) \
    && SL_BOARD_ENABLE_SENSOR_LIGHT
#ifdef SL_CATALOG_SENSOR_LIGHT_PRESENT
    {
      .property_id = PRESENT_AMBIENT_LIGHT_LEVEL,
      .positive_tolerance = SENSOR_LIGHT_POSITIVE_TOLERANCE,
      .negative_tolerance = SENSOR_LIGHT_NEGATIVE_TOLERANCE,
      .sampling_function = SENSOR_LIGHT_SAMPLING_FUNCTION,
      .measurement_period = SENSOR_LIGHT_MEASUREMENT_PERIOD,
      .update_interval = SENSOR_LIGHT_UPDATE_INTERVAL
    },
#elif defined(SL_CATALOG_SENSOR_LUX_PRESENT)
    {
      .property_id = PRESENT_AMBIENT_LIGHT_LEVEL,
      .positive_tolerance = SENSOR_LUX_POSITIVE_TOLERANCE,
      .negative_tolerance = SENSOR_LUX_NEGATIVE_TOLERANCE,
      .sampling_function = SENSOR_LUX_SAMPLING_FUNCTION,
      .measurement_period = SENSOR_LUX_MEASUREMENT_PERIOD,
      .update_interval = SENSOR_LUX_UPDATE_INTERVAL
    },
#endif // SL_CATALOG_SENSOR_LIGHT_PRESENT, SL_CATALOG_SENSOR_LUX_PRESENT
#endif // SL_BOARD_ENABLE_SENSOR_LIGHT
#if defined(SL_CATALOG_SENSOR_RHT_PRESENT) \
    && defined(SL_BOARD_ENABLE_SENSOR_RHT) \
    && SL_BOARD_ENABLE_SENSOR_RHT
    {
      .property_id = PRESENT_AMBIENT_TEMPERATURE,
      .positive_tolerance = SENSOR_THERMOMETER_POSITIVE_TOLERANCE,
      .negative_tolerance = SENSOR_THERMOMETER_NEGATIVE_TOLERANCE,
      .sampling_function = SENSOR_THERMOMETER_SAMPLING_FUNCTION,
      .measurement_period = SENSOR_THERMOMETER_MEASUREMENT_PERIOD,
      .update_interval = SENSOR_THERMOMETER_UPDATE_INTERVAL
    },
#endif // SL_CATALOG_SENSOR_RHT_PRESENT
  };

  uint16_t status = mesh_lib_sensor_server_init(BTMESH_SENSOR_SERVER_MAIN,
                                                sizeof(descriptors)
                                                / sizeof(sensor_descriptor_t),
                                                descriptors);
  app_assert_status_f(status, "Sensor Init Error\n");

#ifdef SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT
  sl_btmesh_set_people_count(0);
#endif // SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT
#if defined(SL_BOARD_ENABLE_SENSOR_LIGHT) && SL_BOARD_ENABLE_SENSOR_LIGHT
#ifdef SL_CATALOG_SENSOR_LIGHT_PRESENT
  {
    sl_status_t sc;
    sc = sl_sensor_light_init();
    if (sc != SL_STATUS_OK) {
      app_log_warning("Ambient light and UV index sensor initialization failed.");
      app_log_nl();
    }
  }
#endif // SL_CATALOG_SENSOR_LIGHT_PRESENT
#ifdef SL_CATALOG_SENSOR_LUX_PRESENT
  {
    sl_status_t sc;
    sc = sl_sensor_lux_init();
    if (sc != SL_STATUS_OK) {
      app_log_warning("Ambient light sensor initialization failed.");
      app_log_nl();
    }
  }
#endif // SL_CATALOG_SENSOR_LUX_PRESENT
#endif // SL_BOARD_ENABLE_SENSOR_LIGHT
#if defined(SL_CATALOG_SENSOR_RHT_PRESENT) \
  && defined(SL_BOARD_ENABLE_SENSOR_RHT)   \
  && SL_BOARD_ENABLE_SENSOR_RHT
  {
    sl_status_t sc;
    sc = sl_sensor_rht_init();
    if (sc != SL_STATUS_OK) {
      app_log_warning("Relative Humidity and Temperature sensor initialization failed.");
      app_log_nl();
#if SENSOR_THERMOMETER_CADENCE
      rht_initialized = false;
    } else {
      rht_initialized = true;
    }
#else
    }
#endif // SENSOR_THERMOMETER_CADENCE
  }
#endif // SL_CATALOG_SENSOR_RHT_PRESENT

#if SENSOR_PEOPLE_COUNT_CADENCE && SENSOR_THERMOMETER_CADENCE
  uint32_t update_interval;
  sl_btmesh_sensor_people_count_cadence_init(0);
  if (rht_initialized == true) {
    sl_btmesh_sensor_thermometer_cadence_init(get_temperature());
    update_interval = MIN(SENSOR_THERMOMETER_UPDATE_INTERVAL, SL_BTMESH_SENSOR_PEOPLE_COUNT_UPDATE_INTERVAL_CFG_VAL);
  } else {
    update_interval = SL_BTMESH_SENSOR_PEOPLE_COUNT_UPDATE_INTERVAL_CFG_VAL;
  }
#elif SENSOR_PEOPLE_COUNT_CADENCE
  uint32_t update_interval;
  sl_btmesh_sensor_people_count_cadence_init(0);
  update_interval = SL_BTMESH_SENSOR_PEOPLE_COUNT_UPDATE_INTERVAL_CFG_VAL;
#elif SENSOR_THERMOMETER_CADENCE
  uint32_t update_interval;
  if (rht_initialized == true) {
    sl_btmesh_sensor_thermometer_cadence_init(get_temperature());
    update_interval = SENSOR_THERMOMETER_UPDATE_INTERVAL;
  } else {
    update_interval = 0;
  }
#endif

#if SENSOR_PEOPLE_COUNT_CADENCE || SENSOR_THERMOMETER_CADENCE
  if (update_interval != 0) {
    sl_status_t sc = sl_simple_timer_start(&sensor_server_data_timer,
                                           ((uint32_t)(pow((double)1.1, ((double)update_interval - 64)) * 1000)),
                                           sensor_server_data_timer_cb,
                                           NO_CALLBACK_DATA,
                                           true);
    app_assert_status_f(sc, "Failed to start periodic sensor_server_data_timer\r\n");
  }
#endif
}

/***************************************************************************//**
 * Handling of sensor server get request event.
 * It sending sensor status message with data for all of supported Properties ID,
 * if there is no Property ID field in request. If request contains Property ID
 * that is supported, functions reply with the sensor status message with data
 * for this Property ID, in other case the message contains no data.
 *
 * @param[in] evt  Pointer to sensor server get request event.
 ******************************************************************************/
static void handle_sensor_server_get_request(
  sl_btmesh_evt_sensor_server_get_request_t *evt)
{
  // A slot for all sensor data
  uint8_t sensor_data[SENSOR_DATA_BUF_LEN];
  uint8_t len = 0;
  sl_status_t sc;

  (void)evt;
#ifdef SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT
  if ((evt->property_id == PEOPLE_COUNT)
      || (evt->property_id == PROPERTY_ID_ALL)) {
    count16_t people_count = sl_btmesh_get_people_count();
    sl_btmesh_sensor_server_on_people_count_measurement(people_count);
    len += mesh_sensor_data_to_buf(PEOPLE_COUNT,
                                   &sensor_data[len],
                                   (uint8_t*)&people_count);
  }
#endif // SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT
#if defined(SL_BOARD_ENABLE_SENSOR_LIGHT) \
  && SL_BOARD_ENABLE_SENSOR_LIGHT
#if defined(SL_CATALOG_SENSOR_LIGHT_PRESENT) \
  || defined(SL_CATALOG_SENSOR_LUX_PRESENT)
  if ((evt->property_id == PRESENT_AMBIENT_LIGHT_LEVEL)
      || (evt->property_id == PROPERTY_ID_ALL)) {
    illuminance_t light = get_light();
    len += mesh_sensor_data_to_buf(PRESENT_AMBIENT_LIGHT_LEVEL,
                                   &sensor_data[len],
                                   (uint8_t*)&light);
  }
#endif // SL_CATALOG_SENSOR_LIGHT_PRESENT || SL_CATALOG_SENSOR_LUX_PRESENT
#endif // SL_BOARD_ENABLE_SENSOR_LIGHT
#if defined(SL_CATALOG_SENSOR_RHT_PRESENT) \
  && defined(SL_BOARD_ENABLE_SENSOR_RHT)   \
  && SL_BOARD_ENABLE_SENSOR_RHT
  if ((evt->property_id == PRESENT_AMBIENT_TEMPERATURE)
      || (evt->property_id == PROPERTY_ID_ALL)) {
    temperature_8_t temperature = get_temperature();
    len += mesh_sensor_data_to_buf(PRESENT_AMBIENT_TEMPERATURE,
                                   &sensor_data[len],
                                   (uint8_t*)&temperature);
  }
#endif // SL_CATALOG_SENSOR_RHT_PRESENT
  if (len > 0) {
    sc = sl_btmesh_sensor_server_send_status(evt->client_address,
                                             BTMESH_SENSOR_SERVER_MAIN,
                                             evt->appkey_index,
                                             NO_FLAGS,
                                             len,
                                             sensor_data);
  } else {
    sensor_data[0] = evt->property_id & 0xFF;
    sensor_data[1] = ((evt->property_id) >> 8) & 0xFF;
    sensor_data[2] = 0; // Length is 0 for unsupported property_id
    sc = sl_btmesh_sensor_server_send_status(evt->client_address,
                                             BTMESH_SENSOR_SERVER_MAIN,
                                             evt->appkey_index,
                                             NO_FLAGS,
                                             3,
                                             sensor_data);
  }
  log_status_error_f(sc,
                     SENSOR_SERVER_SEND_FAILED_TEXT,
                     "status");
}

/***************************************************************************//**
 * Handling of sensor server get column request event.
 * Used Property IDs does not have sensor series column state,
 * so reply has the same data as request according to specification.
 *
 * @param[in] evt  Pointer to sensor server get column request event.
 ******************************************************************************/
static void handle_sensor_server_get_column_request(
  sl_btmesh_evt_sensor_server_get_column_request_t *evt)
{
  sl_status_t sc;
  sc = sl_btmesh_sensor_server_send_column_status(evt->client_address,
                                                  BTMESH_SENSOR_SERVER_MAIN,
                                                  evt->appkey_index,
                                                  NO_FLAGS,
                                                  evt->property_id,
                                                  evt->column_ids.len,
                                                  evt->column_ids.data);

  log_status_error_f(sc,
                     SENSOR_SERVER_SEND_FAILED_TEXT,
                     "column status");
}

/***************************************************************************//**
 * Handling of sensor server get series request event.
 * Used Property IDs does not have sensor series column state,
 * so reply has only Property ID according to specification.
 *
 * @param[in] evt  Pointer to sensor server get series request event.
 ******************************************************************************/
static void handle_sensor_server_get_series_request(
  sl_btmesh_evt_sensor_server_get_series_request_t *evt)
{
  sl_status_t sc;
  sc = sl_btmesh_sensor_server_send_series_status(evt->client_address,
                                                  BTMESH_SENSOR_SERVER_MAIN,
                                                  evt->appkey_index,
                                                  NO_FLAGS,
                                                  evt->property_id,
                                                  0,
                                                  NULL);

  log_status_error_f(sc,
                     SENSOR_SERVER_SEND_FAILED_TEXT,
                     "series status");
}

/***************************************************************************//**
 * It is used for sensor states publishing
 *
 * @return  none
 ******************************************************************************/
static void sensor_server_publish(void)
{
  uint8_t sensor_data[SENSOR_DATA_BUF_LEN];
  uint8_t len = 0;

#ifdef SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT
  count16_t people_count = sl_btmesh_get_people_count();
  sl_btmesh_sensor_server_on_people_count_measurement(people_count);
  len += mesh_sensor_data_to_buf(PEOPLE_COUNT,
                                 &sensor_data[len],
                                 (uint8_t*)&people_count);
#endif // SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT

#if defined(SL_BOARD_ENABLE_SENSOR_LIGHT) && SL_BOARD_ENABLE_SENSOR_LIGHT
#if defined(SL_CATALOG_SENSOR_LIGHT_PRESENT) \
  || defined(SL_CATALOG_SENSOR_LUX_PRESENT)
  illuminance_t light = get_light();
  len += mesh_sensor_data_to_buf(PRESENT_AMBIENT_LIGHT_LEVEL,
                                 &sensor_data[len],
                                 (uint8_t*)&light);
#endif // SL_CATALOG_SENSOR_LIGHT_PRESENT || SL_CATALOG_SENSOR_LUX_PRESENT
#endif // SL_BOARD_ENABLE_SENSOR_LIGHT

#if defined(SL_BOARD_ENABLE_SENSOR_RHT) && SL_BOARD_ENABLE_SENSOR_RHT
#ifdef SL_CATALOG_SENSOR_RHT_PRESENT
  temperature_8_t temperature = get_temperature();
  len += mesh_sensor_data_to_buf(PRESENT_AMBIENT_TEMPERATURE,
                                 &sensor_data[len],
                                 (uint8_t*) &temperature);
#endif // SL_CATALOG_SENSOR_RHT_PRESENT
#endif // SL_BOARD_ENABLE_SENSOR_RHT

  if (len > 0) {
    sl_status_t sc = sl_btmesh_sensor_server_send_status(PUBLISH_TO_ALL_NODES,
                                                         BTMESH_SENSOR_SERVER_MAIN,
                                                         IGNORED,
                                                         NO_FLAGS,
                                                         len,
                                                         sensor_data);
    log_btmesh_status_f(sc, SENSOR_SERVER_SEND_FAILED_TEXT, "status");
  }
}

/***************************************************************************//**
 * Handling of sensor server publish event.
 * Indicates that the publishing period timer elapsed and updates the current
 * publishing period that can be used to estimate the next tick, e.g., when
 * the state should be reported at higher frequency.
 *
 * @param[in] evt  Pointer to sensor server publish request event structure
 ******************************************************************************/
static void handle_sensor_server_publish_event(
  sl_btmesh_evt_sensor_server_publish_t *evt)
{
#if SENSOR_THERMOMETER_CADENCE || SENSOR_PEOPLE_COUNT_CADENCE
  publish_period = *evt;

#else
  (void)evt;
  sensor_server_publish();
#endif
}

/***************************************************************************//**
 * Handling of sensor setup server get cadence request event.
 *
 * @param[in] evt  Pointer to sensor server get cadence request event.
 ******************************************************************************/
static void handle_sensor_setup_server_get_cadence_request(
  sl_btmesh_evt_sensor_setup_server_get_cadence_request_t *evt)
{
  sl_status_t sc;
  uint16_t buff_len = 0;
  uint8_t* buff_addr = NULL;

#if SENSOR_THERMOMETER_CADENCE || SENSOR_PEOPLE_COUNT_CADENCE
  uint8_t cadence_status_buf[SENSOR_CADENCE_BUF_LEN];
#endif

#if SENSOR_THERMOMETER_CADENCE
  if (evt->property_id == PRESENT_AMBIENT_TEMPERATURE) {
    buff_len = sl_btmesh_sensor_thermometer_get_cadence(SENSOR_CADENCE_BUF_LEN, cadence_status_buf);
    buff_addr = cadence_status_buf;
  }
#endif // SENSOR_THERMOMETER_CADENCE

#if SENSOR_PEOPLE_COUNT_CADENCE
  if (evt->property_id == PEOPLE_COUNT) {
    buff_len = sl_btmesh_sensor_people_count_get_cadence(SENSOR_CADENCE_BUF_LEN, cadence_status_buf);
    buff_addr = cadence_status_buf;
  }
#endif // SENSOR_THERMOMETER_CADENCE

  sc = sl_btmesh_sensor_setup_server_send_cadence_status(evt->client_address,
                                                         BTMESH_SENSOR_SERVER_MAIN,
                                                         evt->appkey_index,
                                                         NO_FLAGS,
                                                         evt->property_id,
                                                         buff_len,
                                                         buff_addr);
  log_status_error_f(sc,
                     SENSOR_SETUP_SERVER_SEND_FAILED_TEXT,
                     "cadence status");
}

/***************************************************************************//**
 * Handling of sensor setup server set cadence request event.
 *
 * @param[in] evt  Pointer to sensor server set cadence request event.
 ******************************************************************************/
static void handle_sensor_setup_server_set_cadence_request(
  sl_btmesh_evt_sensor_setup_server_set_cadence_request_t *evt)
{
  bool param_validity = true;
  uint16_t buff_len = 0;
  uint8_t* buff_addr = NULL;

#if SENSOR_THERMOMETER_CADENCE || SENSOR_PEOPLE_COUNT_CADENCE
  uint8_t cadence_status_buf[SENSOR_CADENCE_BUF_LEN];
#endif

#if SENSOR_THERMOMETER_CADENCE
  if (evt->property_id == PRESENT_AMBIENT_TEMPERATURE) {
    // store incoming cadence parameters
    param_validity = sl_btmesh_sensor_thermometer_set_cadence(evt);
    if (((evt->flags & SET_CADENCE_ACK_FLAG) == SET_CADENCE_ACK_FLAG)
        && (param_validity == true)) {
      // prepare buffer for cadence status response
      buff_len = sl_btmesh_sensor_thermometer_get_cadence(SENSOR_CADENCE_BUF_LEN, cadence_status_buf);
      buff_addr = cadence_status_buf;
    }
  }
#endif // SENSOR_THERMOMETER_CADENCE

#if SENSOR_PEOPLE_COUNT_CADENCE
  if (evt->property_id == PEOPLE_COUNT) {
    // store incoming cadence parameters
    param_validity = sl_btmesh_sensor_people_count_set_cadence(evt);
    if (((evt->flags & SET_CADENCE_ACK_FLAG) == SET_CADENCE_ACK_FLAG)
        && (param_validity == true)) {
      // prepare buffer for cadence status response
      buff_len = sl_btmesh_sensor_people_count_get_cadence(SENSOR_CADENCE_BUF_LEN, cadence_status_buf);
      buff_addr = cadence_status_buf;
    }
  }
#endif // SENSOR_PEOPLE_COUNT_CADENCE
  if (((evt->flags & SET_CADENCE_ACK_FLAG) == SET_CADENCE_ACK_FLAG)
      && (param_validity == true)) {
    sl_status_t sc = sl_btmesh_sensor_setup_server_send_cadence_status(evt->client_address,
                                                                       BTMESH_SENSOR_SERVER_MAIN,
                                                                       evt->appkey_index,
                                                                       NO_FLAGS,
                                                                       evt->property_id,
                                                                       buff_len,
                                                                       buff_addr);
    log_status_error_f(sc,
                       SENSOR_SETUP_SERVER_SEND_FAILED_TEXT,
                       "cadence status");
  }
}

/***************************************************************************//**
 * Handling of sensor setup server get settings request event.
 * Settings are not supported now, so reply has only Property ID
 * according to specification.
 *
 * @param[in] evt  Pointer to sensor server get settings request event.
 ******************************************************************************/
static void handle_sensor_setup_server_get_settings_request(
  sl_btmesh_evt_sensor_setup_server_get_settings_request_t *evt)
{
  sl_status_t sc;
  sc = sl_btmesh_sensor_setup_server_send_settings_status(evt->client_address,
                                                          BTMESH_SENSOR_SERVER_MAIN,
                                                          evt->appkey_index,
                                                          NO_FLAGS,
                                                          evt->property_id,
                                                          0,
                                                          NULL);
  log_status_error_f(sc,
                     SENSOR_SETUP_SERVER_SEND_FAILED_TEXT,
                     "settings status");
}

/***************************************************************************//**
 * Handling of sensor setup server get setting request event.
 * Settings are not supported now, so reply has only Property ID
 * and Sensor Property ID according to specification.
 *
 * @param[in] evt  Pointer to sensor server get setting request event.
 ******************************************************************************/
static void handle_sensor_setup_server_get_setting_request(
  sl_btmesh_evt_sensor_setup_server_get_setting_request_t *evt)
{
  sl_status_t sc;
  sc = sl_btmesh_sensor_setup_server_send_setting_status(evt->client_address,
                                                         BTMESH_SENSOR_SERVER_MAIN,
                                                         evt->appkey_index,
                                                         NO_FLAGS,
                                                         evt->property_id,
                                                         evt->setting_id,
                                                         0,
                                                         NULL);
  log_status_error_f(sc,
                     SENSOR_SETUP_SERVER_SEND_FAILED_TEXT,
                     "setting status");
}

/***************************************************************************//**
 * Handling of sensor setup server set setting request event.
 * Settings are not supported now, so reply has only Property ID
 * and Sensor Property ID according to specification.
 *
 * @param[in] evt  Pointer to sensor server set setting request event.
 ******************************************************************************/
static void handle_sensor_setup_server_set_setting_request(
  sl_btmesh_evt_sensor_setup_server_set_setting_request_t *evt)
{
  sl_status_t sc;
  sc = sl_btmesh_sensor_setup_server_send_setting_status(evt->client_address,
                                                         BTMESH_SENSOR_SERVER_MAIN,
                                                         evt->appkey_index,
                                                         NO_FLAGS,
                                                         evt->property_id,
                                                         evt->setting_id,
                                                         0,
                                                         NULL);
  log_status_error_f(sc,
                     SENSOR_SETUP_SERVER_SEND_FAILED_TEXT,
                     "setting status");
}

/***************************************************************************//**
 *  Handling of mesh events by sensor server component.
 *  It handles:
 *   - node_initialized
 *   - node_provisioned
 *   - sensor_server_get_request
 *   - sensor_server_get_column_request
 *   - sensor_server_get_series_request
 *   - sensor_setup_server_get_cadence_request
 *   - sensor_setup_server_set_cadence_request
 *   - sensor_setup_server_get_settings_request
 *   - sensor_setup_server_get_setting_request
 *   - sensor_setup_server_set_setting_request
 *
 *  @param[in] evt  Pointer to incoming sensor server event.
 ******************************************************************************/
void sl_btmesh_handle_sensor_server_events(sl_btmesh_msg_t* evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      if (evt->data.evt_node_initialized.provisioned) {
        sl_btmesh_sensor_server_node_init();
      }
      break;

    case sl_btmesh_evt_node_provisioned_id:
      sl_btmesh_sensor_server_node_init();
      break;

    case sl_btmesh_evt_sensor_server_get_request_id:
      handle_sensor_server_get_request(
        &(evt->data.evt_sensor_server_get_request));
      break;

    case sl_btmesh_evt_sensor_server_get_column_request_id:
      handle_sensor_server_get_column_request(
        &(evt->data.evt_sensor_server_get_column_request));
      break;

    case sl_btmesh_evt_sensor_server_get_series_request_id:
      handle_sensor_server_get_series_request(
        &(evt->data.evt_sensor_server_get_series_request));
      break;

    case sl_btmesh_evt_sensor_server_publish_id:
      handle_sensor_server_publish_event(
        &(evt->data.evt_sensor_server_publish));
      break;

    case sl_btmesh_evt_sensor_setup_server_get_cadence_request_id:
      handle_sensor_setup_server_get_cadence_request(
        &(evt->data.evt_sensor_setup_server_get_cadence_request));
      break;

    case sl_btmesh_evt_sensor_setup_server_set_cadence_request_id:
      handle_sensor_setup_server_set_cadence_request(
        &(evt->data.evt_sensor_setup_server_set_cadence_request));
      break;

    case sl_btmesh_evt_sensor_setup_server_get_settings_request_id:
      handle_sensor_setup_server_get_settings_request(
        &(evt->data.evt_sensor_setup_server_get_settings_request));
      break;

    case sl_btmesh_evt_sensor_setup_server_get_setting_request_id:
      handle_sensor_setup_server_get_setting_request(
        &(evt->data.evt_sensor_setup_server_get_setting_request));
      break;

    case sl_btmesh_evt_sensor_setup_server_set_setting_request_id:
      handle_sensor_setup_server_set_setting_request(
        &(evt->data.evt_sensor_setup_server_set_setting_request));
      break;

    default:
      break;
  }
}

#ifdef SL_CATALOG_SENSOR_RHT_PRESENT
/***************************************************************************//**
 * Get the current temperature value measured by sensor.
 *
 * @return Current value of temperature.
 ******************************************************************************/
temperature_8_t get_temperature(void)
{
  int32_t temp_data = 0;
  uint32_t temp_rh = 0;
  temperature_8_t temperature = SL_BTMESH_SENSOR_TEMPERATURE_VALUE_UNKNOWN;
  sl_status_t sc = sl_sensor_rht_get(&temp_rh, &temp_data);
  if (sc == SL_STATUS_OK) {
    temp_data = (((temp_data
                   * TEMPERATURE_PRE_SCALE)
                  + TEMPERATURE_OFFSET)
                 / TEMPERATURE_SCALE_VAL);
    temperature = (temperature_8_t)temp_data;
  } else if (sc != SL_STATUS_NOT_INITIALIZED) {
    log("Warning! Invalid temperature reading: %u %d\n",
        temp_rh,
        temp_data);
  }
  sl_btmesh_sensor_server_on_temperature_measurement(temperature);
  return temperature;
}
#endif // SL_CATALOG_SENSOR_RHT_PRESENT

#if defined(SL_CATALOG_SENSOR_LIGHT_PRESENT) \
  || defined(SL_CATALOG_SENSOR_LUX_PRESENT)
/***************************************************************************//**
 * Get the current light value measured by sensor.
 *
 * @return Current value of light reading.
 ******************************************************************************/
illuminance_t get_light(void)
{
  float lux;
  illuminance_t light = SL_BTMESH_SENSOR_LIGHT_VALUE_UNKNOWN;
  sl_status_t sc;
#ifdef SL_CATALOG_SENSOR_LIGHT_PRESENT
  float uvi;

  sc = sl_sensor_light_get(&lux, &uvi);
#elif defined(SL_CATALOG_SENSOR_LUX_PRESENT)
  sc = sl_sensor_lux_get(&lux);
#endif // SL_CATALOG_SENSOR_LIGHT_PRESENT
  if (sc == SL_STATUS_OK) {
    light = (illuminance_t)lux;
  } else if (sc != SL_STATUS_NOT_INITIALIZED) {
    log("Warning! Invalid light reading: %6ulx\n", (illuminance_t)lux);
  }
  sl_btmesh_sensor_server_on_light_measurement(light);
  return light;
}
#endif // SL_CATALOG_SENSOR_LIGHT_PRESENT || SL_CATALOG_SENSOR_LUX_PRESENT

/***************************************************************************//**
 * Timer Callbacks
 ******************************************************************************/
#if SENSOR_THERMOMETER_CADENCE || SENSOR_PEOPLE_COUNT_CADENCE
/***************************************************************************//**
 * Get measured value from sensors and analyze cadence conditions timer callback
 *
 * @param[in] handle  Pointer to the timer handle
 * @param[in] data    Pointer to callback data
 *
 * @return none
 ******************************************************************************/
static void sensor_server_data_timer_cb(sl_simple_timer_t *handle, void *data)
{
  (void)data;
  (void)handle;
  uint32_t publ_timer_thermometer = publish_period.period_ms;
  uint32_t publ_timer_people_count = publish_period.period_ms;
  uint32_t publ_timeout;
  sl_status_t sc;

#if SENSOR_THERMOMETER_CADENCE
  if (rht_initialized == true) {
    publ_timer_thermometer = sl_btmesh_sensor_thermometer_handle_cadence(get_temperature(), publish_period);
  }
#endif // SENSOR_THERMOMETER_CADENCE

#if SENSOR_PEOPLE_COUNT_CADENCE
  publ_timer_people_count = sl_btmesh_sensor_people_count_handle_cadence(sl_btmesh_get_people_count(), publish_period);
#endif // SENSOR_PEOPLE_COUNT_CADENCE

  if (publ_timer_thermometer > publ_timer_people_count) {
    publ_timeout = publ_timer_people_count;
  } else {
    publ_timeout = publ_timer_thermometer;
  }

  if (prev_publish_timeout != publ_timeout) {
    log_info("Publishing period: %d ms\r\n", publ_timeout);
    //Stop publish timer
    sc = sl_simple_timer_stop(&sensor_server_publish_timer);

    app_assert_status_f(sc, "Failed to stop periodic sensor_server_publish_timer\r\n");

    //Restart publishing timer with the new timer value
    sc = sl_simple_timer_start(&sensor_server_publish_timer,
                               publ_timeout,
                               sensor_server_publish_timer_cb,
                               NO_CALLBACK_DATA,
                               true);
    app_assert_status_f(sc, "Failed to start periodic sensor_server_publish_timer\r\n");
  }
  prev_publish_timeout = publ_timeout;
}

/***************************************************************************//**
 * Publish sensor status timer callback
 *
 * @param[in] handle  Pointer to the timer handle
 * @param[in] data    Pointer to callback data
 *
 * @return none
 ******************************************************************************/
static void sensor_server_publish_timer_cb(sl_simple_timer_t *handle, void *data)
{
  (void)data;
  (void)handle;

  sensor_server_publish();
}
#endif
/**************************************************************************//**
 * @addtogroup btmesh_sens_srv_cb_weak Weak implementation of callbacks
 * @{
 *****************************************************************************/
SL_WEAK void sl_btmesh_sensor_server_on_temperature_measurement(temperature_8_t temperature)
{
  (void) temperature;
}

SL_WEAK void sl_btmesh_sensor_server_on_light_measurement(illuminance_t light)
{
  (void) light;
}

SL_WEAK void sl_btmesh_sensor_server_on_people_count_measurement(count16_t people)
{
  (void) people;
}
/** @} (end addtogroup btmesh_sens_srv_cb_weak) */

/** @} (end addtogroup Sensor) */
