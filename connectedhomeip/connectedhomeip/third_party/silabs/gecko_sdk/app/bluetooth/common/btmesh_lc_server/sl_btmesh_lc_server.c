/***************************************************************************//**
 * @file
 * @brief Bt Mesh LC Server module
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
#include <math.h>

#include <stdbool.h>
#include "em_common.h"
#include "sl_status.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_dcd.h"

#include "sl_btmesh_generic_model_capi_types.h"

// Mesh specific headers
#include "sl_btmesh_lib.h"
#include "sl_btmesh_device_properties.h"
#include "sl_btmesh_sensor.h"

#include "app_assert.h"
#include "sl_simple_timer.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

#include "sl_btmesh_lc_server_config.h"
#include "sl_btmesh_lc_server.h"
#include "sl_btmesh_lighting_server.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup LC_Server
 * @{
 ******************************************************************************/

#define FRACTION(num) (uint16_t)(((num) > 0                \
                                  ? (num) - (int32_t)(num) \
                                  : (int32_t)(num) - (num)) * 1000)

/// No flags used for message
#define NO_FLAGS                        0
/// Immediate transition time is 0 seconds
#define IMMEDIATE                       0
/// Callback has no parameters
#define NO_CALLBACK_DATA                (void *)NULL
/// High Priority
#define HIGH_PRIORITY                   0
/// Values greater than max 37200000 are treated as unknown remaining time
#define UNKNOWN_REMAINING_TIME          40000000
/**
 * @brief Binary state that determines the mode of operation of the controller
 *
 * - 0 The controller is turned off. The binding with the Light Lightness state
 * is disabled.
 * - 1 The controller is turned on. The binding with the Light Lightness state
 * is enabled.
 */
#define LC_MODE_DEFAULT                 0
/**
 * @brief Binary state that determines if a controller transitions from a
 * standby state when an occupancy sensor reports occupancy
 *
 * - 0 The controller does not transition from a standby state when occupancy
 * is reported.
 * - 1 The controller may transition from a standby state when occupancy
 * is reported.
 */
#define LC_OCCUPANCY_MODE_DEFAULT       1
/**
 * @brief Representing the integral coefficient that determines the integral
 * part of the equation defining the output of the regulator
 *
 * 0.0 - 1000.0 Integral coefficient when increasing output
 */
#define LC_REGULATOR_KIU_DEFAULT        250.0
/**
 * @brief Representing the integral coefficient that determines the integral
 * part of the equation defining the output of the regulator
 *
 * 0.0 - 1000.0 Integral coefficient when decreasing output
 */
#define LC_REGULATOR_KID_DEFAULT        25.0
/**
 * @brief Representing the proportional coefficient that determines the
 * proportional part of the equation defining the output of the regulator
 *
 * 0.0 - 1000.0 Proportional coefficient when increasing output
 */
#define LC_REGULATOR_KPU_DEFAULT        80.0
/**
 * @brief Representing the proportional coefficient that determines the
 * proportional part of the equation defining the output of the regulator
 *
 * 0.0 - 1000.0 Proportional coefficient when decreasing output
 */
#define LC_REGULATOR_KPD_DEFAULT        80.0
/**
 * @brief Representing the percentage accuracy of the regulator
 *
 * 0.0 - 100.0 Regulator accuracy (percentage)
 *
 * \note Representing half a percent, i.e. 4 represents 2.0 percent.
 */
#define LC_REGULATOR_ACCURACY_DEFAULT   4

/*******************************************************************************
 * Timer handle definitions.
 ******************************************************************************/
static sl_simple_timer_t lc_save_state_timer;
static sl_simple_timer_t lc_save_property_state_timer;
static sl_simple_timer_t lc_onoff_transition_timer;
static sl_simple_timer_t lc_delayed_onoff_timer;

// Timer callbacks
static void lc_save_state_timer_cb(sl_simple_timer_t *handle,
                                   void *data);
static void lc_save_property_state_timer_cb(sl_simple_timer_t *handle,
                                            void *data);
static void lc_onoff_transition_timer_cb(sl_simple_timer_t *handle,
                                         void *data);
static void lc_delayed_onoff_timer_cb(sl_simple_timer_t *handle,
                                      void *data);

/// LC state
static PACKSTRUCT(struct lc_state {
  uint8_t mode;                   /**< LC mode */
  uint8_t occupancy_mode;         /**< LC occupancy mode */
  uint8_t light_onoff;            /**< LC light onoff */
  uint8_t onoff_current;          /**< Current LC generic on/off value */
  uint8_t onoff_target;           /**< Target LC generic on/off value */
}) lc_state;

/// LC property state
static PACKSTRUCT(struct lc_property_state {
  /// Delay between receiving sensor occupancy message
  /// and changing the Light LC Occupancy state
  light_control_time_occupancy_delay time_occupancy_delay: 24;
  /// Transition time from a standby state to a run state
  light_control_time_fade_on time_fade_on: 24;
  /// Duration of the run state after last occupancy was detected
  light_control_time_run_on time_run_on: 24;
  /// Transition time from a run state to a prolong state
  light_control_time_fade time_fade: 24;
  /// Duration of the prolong state
  light_control_time_prolong time_prolong: 24;
  /// Transition time from a prolong state to a standby state
  /// when the transition is automatic
  light_control_time_standby_auto time_fade_standby_auto: 24;
  /// Transition time from a prolong state to a standby state
  /// when the transition is triggered by a manual operation
  light_control_time_standby_manual time_fade_standby_manual: 24;
  /// Lightness level in a run state
  uint16_t lightness_on;
  /// Lightness level in a prolong state
  uint16_t lightness_prolong;
  /// Lightness level in a standby state
  uint16_t lightness_standby;
  /// Required Ambient LuxLevel level in the Run state
  illuminance_t ambient_luxlevel_on: 24;
  /// Required Ambient LuxLevel level in the Prolong state
  illuminance_t ambient_luxlevel_prolong: 24;
  /// Required Ambient LuxLevel level in the Standby state
  illuminance_t ambient_luxlevel_standby: 24;
  /// Integral coefficient of PI light regulator when increasing output
  coefficient_t regulator_kiu;
  /// Integral coefficient of PI light regulator when decreasing output
  coefficient_t regulator_kid;
  /// Proportional coefficient of PI light regulator when increasing output
  coefficient_t regulator_kpu;
  /// Proportional coefficient of PI light regulator when decreasing output
  coefficient_t regulator_kpd;
  /// Accuracy of PI light regulator
  percentage_8_t regulator_accuracy;
}) lc_property_state;

/// copy of transition delay parameter, needed for delayed lc on/off request
static uint32_t delayed_lc_onoff_trans = 0;

static void lc_onoff_transition_complete(void);

static void delayed_lc_onoff_request(void);

/***************************************************************************//**
 * Initialization of the models supported by this node.
 * This function registers callbacks for each of the supported models.
 ******************************************************************************/
static void init_models(void);

/***************************************************************************//**
 * This function loads the saved light controller state from Persistent Storage
 * and copies the data in the global variable lc_state.
 * If PS key with ID 0x4005 does not exist or loading failed,
 * lc_state is set to zero and some default values are written to it.
 *
 * @return Returns SL_STATUS_OK (0) if succeeds, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lc_state_load(void)
{
  sl_status_t sc;
  size_t ps_len = 0;
  struct lc_state ps_data;

  sc = sl_bt_nvm_load(SL_BTMESH_LC_SERVER_PS_KEY_CFG_VAL,
                      sizeof(ps_data),
                      &ps_len,
                      (uint8_t *)&ps_data);

  // Set default values if ps_load fail or size of lc_state has changed
  if ((sc != SL_STATUS_OK) || (ps_len != sizeof(lc_state))) {
    memset(&lc_state, 0, sizeof(lc_state));
    lc_state.mode           = LC_MODE_DEFAULT;
    lc_state.occupancy_mode = LC_OCCUPANCY_MODE_DEFAULT;

    if (sc == SL_STATUS_OK) {
      // The sl_bt_nvm_load call was successful but the size of the loaded data
      // differs from the expected size therefore error code shall be set
      sc = SL_STATUS_INVALID_STATE;
      log("LC server lc_state loaded from PS with invalid size, "
          "use defaults. (expected=%zd,actual=%zd)\r\n",
          sizeof(lc_state),
          ps_len);
    } else {
      log_status_f(sc,
                   "LC server lc_state load from PS failed "
                   "or nvm is empty, use defaults.\r\n");
    }
  } else {
    memcpy(&lc_state, &ps_data, ps_len);
  }

  return sc;
}

/***************************************************************************//**
 * This function saves the current light controller state in Persistent Storage
 * so that the data is preserved over reboots and power cycles.
 * The light controller state is hold in a global variable lc_state.
 * A PS key with ID 0x4005 is used to store the whole structure.
 *
 * @return Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static int lc_state_store(void)
{
  sl_status_t sc = sl_bt_nvm_save(SL_BTMESH_LC_SERVER_PS_KEY_CFG_VAL,
                                  sizeof(struct lc_state),
                                  (const uint8_t *)&lc_state);

  log_status_level_f(APP_LOG_LEVEL_ERROR,
                     sc,
                     "LC server lc_state store in PS failed.\r\n");

  return sc;
}

/***************************************************************************//**
 * This function is called each time the light controller state in RAM
 * is changed. It sets up a soft timer that will save the state in flash after
 * small delay. The purpose is to reduce amount of unnecessary flash writes.
 ******************************************************************************/
static void lc_state_changed(void)
{
  sl_status_t sc = sl_simple_timer_start(&lc_save_state_timer,
                                         SL_BTMESH_LC_SERVER_NVM_SAVE_TIME_CFG_VAL,
                                         lc_save_state_timer_cb,
                                         NO_CALLBACK_DATA,
                                         false);
  app_assert_status_f(sc, "Failed to start LC State save timer\n");
}

/*******************************************************************************
 * This function is getter for current light controller mode.
 *
 * @return  current light controller mode
 ******************************************************************************/
uint8_t lc_get_mode(void)
{
  return lc_state.mode;
}

/*******************************************************************************
 * Light Controller state update on power up sequence.
 *
 * @param[in] element    Index of the element.
 * @param[in] onpowerup  Value of OnPowerUp state.
 ******************************************************************************/
void lc_onpowerup_update(uint16_t element, uint8_t onpowerup)
{
  sl_status_t sc_mode = SL_STATUS_OK;
  sl_status_t sc_om = SL_STATUS_OK;
  sl_status_t sc_onoff = SL_STATUS_OK;

  switch (onpowerup) {
    case MESH_GENERIC_ON_POWER_UP_STATE_OFF:
    case MESH_GENERIC_ON_POWER_UP_STATE_ON:
      lc_state.mode = 0;
      lc_state.light_onoff = 0;
      lc_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_OFF;
      lc_state.onoff_target = MESH_GENERIC_ON_OFF_STATE_OFF;
      sc_mode = sl_btmesh_lc_server_update_mode(element, lc_state.mode);
      sc_om = sl_btmesh_lc_server_update_om(element, lc_state.occupancy_mode);
      sc_onoff = sl_btmesh_lc_server_update_light_onoff(element,
                                                        lc_state.light_onoff,
                                                        IMMEDIATE);
      break;

    case MESH_GENERIC_ON_POWER_UP_STATE_RESTORE:
      if (lc_state.mode == 0) {
        sc_mode = sl_btmesh_lc_server_update_mode(element, lc_state.mode);
        sc_om = sl_btmesh_lc_server_update_om(element, lc_state.occupancy_mode);
      } else {
        sc_mode = sl_btmesh_lc_server_update_mode(element, lc_state.mode);
        sc_om = sl_btmesh_lc_server_update_om(element, lc_state.occupancy_mode);
        if (lc_state.light_onoff == 0) {
          sc_onoff = sl_btmesh_lc_server_update_light_onoff(element,
                                                            lc_state.light_onoff,
                                                            IMMEDIATE);
        } else {
          sc_onoff =
            sl_btmesh_lc_server_update_light_onoff(element,
                                                   lc_state.light_onoff,
                                                   lc_property_state.time_fade_on);
        }
      }
      break;

    default:
      break;
  }

  log_btmesh_status_f(sc_mode,
                      "lc_server_update_mode failed (elem=%d)\r\n",
                      element);

  log_btmesh_status_f(sc_om,
                      "lc_server_update_om failed (elem=%d)\r\n",
                      element);

  log_btmesh_status_f(sc_onoff,
                      "lc_server_update_light_onoff failed (elem=%d)\r\n",
                      element);

  lc_state_changed();
}

/***************************************************************************//**
 * This function loads the saved light controller property state from Persistent
 * Storage and copies the data in the global variable lc_property_state.
 * If PS key with ID 0x4006 does not exist or loading failed,
 * lc_property_state is set to zero and some default values are written to it.
 *
 * @return Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lc_property_state_load(void)
{
  sl_status_t sc;
  size_t ps_len = 0;
  struct lc_property_state ps_data;

  sc = sl_bt_nvm_load(SL_BTMESH_LC_SERVER_PROPERTY_PS_KEY_CFG_VAL,
                      sizeof(ps_data),
                      &ps_len,
                      (uint8_t *)&ps_data);

  // Set default values if ps_load fail or size of lc_property_state has changed
  if ((sc != SL_STATUS_OK) || (ps_len != sizeof(lc_property_state))) {
    memset(&lc_property_state, 0, sizeof(lc_property_state));
#if SL_BTMESH_LC_SERVER_PROPERTY_STATE_DEFAULT_ENABLE_CFG_VAL
    lc_property_state.time_occupancy_delay =
      SL_BTMESH_LC_SERVER_TIME_OCCUPANCY_DELAY_DEFAULT_CFG_VAL;
    lc_property_state.time_fade_on = SL_BTMESH_LC_SERVER_TIME_FADE_ON_DEFAULT_CFG_VAL;
    lc_property_state.time_run_on = SL_BTMESH_LC_SERVER_TIME_RUN_ON_DEFAULT_CFG_VAL;
    lc_property_state.time_fade = SL_BTMESH_LC_SERVER_TIME_FADE_DEFAULT_CFG_VAL;
    lc_property_state.time_prolong = SL_BTMESH_LC_SERVER_TIME_PROLONG_DEFAULT_CFG_VAL;
    lc_property_state.time_fade_standby_auto =
      SL_BTMESH_LC_SERVER_TIME_FADE_STANDBY_AUTO_DEFAULT_CFG_VAL;
    lc_property_state.time_fade_standby_manual =
      SL_BTMESH_LC_SERVER_TIME_FADE_STANDBY_MANUAL_DEFAULT_CFG_VAL;
    lc_property_state.lightness_on = SL_BTMESH_LC_SERVER_LIGHTNESS_ON_DEFAULT_CFG_VAL;
    lc_property_state.lightness_prolong = SL_BTMESH_LC_SERVER_LIGHTNESS_PROLONG_DEFAULT_CFG_VAL;
    lc_property_state.lightness_standby = SL_BTMESH_LC_SERVER_LIGHTNESS_STANDBY_DEFAULT_CFG_VAL;
    lc_property_state.ambient_luxlevel_on =
      SL_BTMESH_LC_SERVER_AMBIENT_LUX_LEVEL_ON_DEFAULT_CFG_VAL;
    lc_property_state.ambient_luxlevel_prolong =
      SL_BTMESH_LC_SERVER_AMBIENT_LUX_LEVEL_PROLONG_DEFAULT_CFG_VAL;
    lc_property_state.ambient_luxlevel_standby =
      SL_BTMESH_LC_SERVER_AMBIENT_LUX_LEVEL_STANDBY_DEFAULT_CFG_VAL;
#endif // SL_BTMESH_LC_SERVER_PROPERTY_STATE_DEFAULT_ENABLE_CFG_VAL
    lc_property_state.regulator_kiu       = LC_REGULATOR_KIU_DEFAULT;
    lc_property_state.regulator_kid       = LC_REGULATOR_KID_DEFAULT;
    lc_property_state.regulator_kpu       = LC_REGULATOR_KPU_DEFAULT;
    lc_property_state.regulator_kpd       = LC_REGULATOR_KPD_DEFAULT;
    lc_property_state.regulator_accuracy  = LC_REGULATOR_ACCURACY_DEFAULT;

    if (sc == SL_STATUS_OK) {
      // The sl_bt_nvm_load call was successful but the size of the loaded data
      // differs from the expected size therefore error code shall be set
      sc = SL_STATUS_INVALID_STATE;
      log("LC server lc_property_state loaded from PS with invalid size, "
          "use defaults. (expected=%zd,actual=%zd)\r\n",
          sizeof(lc_property_state),
          ps_len);
    } else {
      log_status_f(sc,
                   "LC server lc_property_state load from PS failed "
                   "or nvm is empty, use defaults.\r\n");
    }
  } else {
    memcpy(&lc_property_state, &ps_data, ps_len);
  }

  return sc;
}

/***************************************************************************//**
 * This function saves the current light controller property state in Persistent
 * Storage so that the data is preserved over reboots and power cycles.
 * The light controller property state is hold in a global variable
 * lc_property_state. A PS key with ID 0x4006 is used to store the
 * whole structure.
 *
 * @return Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static int lc_property_state_store(void)
{
  sl_status_t sc;

  sc = sl_bt_nvm_save(SL_BTMESH_LC_SERVER_PROPERTY_PS_KEY_CFG_VAL,
                      sizeof(struct lc_property_state),
                      (const uint8_t *)&lc_property_state);

  log_status_level_f(APP_LOG_LEVEL_ERROR,
                     sc,
                     "LC server lc_property_state store in PS failed.\r\n");

  return sc;
}

/***************************************************************************//**
 * This function is called each time the light controller property state in RAM
 * is changed. It sets up a soft timer that will save the state in flash after
 * small delay. The purpose is to reduce amount of unnecessary flash writes.
 ******************************************************************************/
static void lc_property_state_changed(void)
{
  sl_status_t sc = sl_simple_timer_start(&lc_save_property_state_timer,
                                         SL_BTMESH_LC_SERVER_NVM_SAVE_TIME_CFG_VAL,
                                         lc_save_property_state_timer_cb,
                                         NO_CALLBACK_DATA,
                                         false);
  app_assert_status_f(sc, "Failed to start LC Property Save timer\n");
}

/***************************************************************************//**
 * This function update property in stack based on property data.
 *
 * @param[in] element        Index of the element.
 * @param[in] property_data  Pointer to property data array that contains:
 *                             - property ID in first two bytes,
 *                             - length of data in third byte,
 *                             - property value in the next bytes.
 ******************************************************************************/
static void update_property(uint16_t element, const uint8_t *property_data)
{
  uint16_t property_id = (uint16_t)property_data[0]
                         | ((uint16_t)property_data[1] << 8);
  sl_status_t sc =
    sl_btmesh_lc_setup_server_update_property(element,
                                              property_id,
                                              property_data[2],
                                              &property_data[3]);

  log_btmesh_status_f(sc,
                      "lc_setup_server_update_property failed "
                      "(elem=%d,property=0x%04x)\r\n",
                      element,
                      property_id);
}

/***************************************************************************//**
 * This function update Light LC Time Occupancy Delay property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_time_occupancy_delay_update(uint16_t element)
{
  uint8_t property_data[6];
  light_control_time_occupancy_delay delay = lc_property_state.time_occupancy_delay;
  mesh_sensor_data_to_buf(LIGHT_CONTROL_TIME_OCCUPANCY_DELAY,
                          property_data,
                          (uint8_t *)&delay);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Time Fade On property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_time_fade_on_update(uint16_t element)
{
  uint8_t property_data[6];
  light_control_time_fade_on fade_on = lc_property_state.time_fade_on;
  mesh_sensor_data_to_buf(LIGHT_CONTROL_TIME_FADE_ON,
                          property_data,
                          (uint8_t *)&fade_on);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Time Run On property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_time_run_on_update(uint16_t element)
{
  uint8_t property_data[6];
  light_control_time_run_on run_on = lc_property_state.time_run_on;
  mesh_sensor_data_to_buf(LIGHT_CONTROL_TIME_RUN_ON,
                          property_data,
                          (uint8_t *)&run_on);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Time Fade property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_time_fade_update(uint16_t element)
{
  uint8_t property_data[6];
  light_control_time_fade fade = lc_property_state.time_fade;
  mesh_sensor_data_to_buf(LIGHT_CONTROL_TIME_FADE,
                          property_data,
                          (uint8_t *)&fade);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Time Prolong property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_time_prolong_update(uint16_t element)
{
  uint8_t property_data[6];
  light_control_time_prolong prolong = lc_property_state.time_prolong;
  mesh_sensor_data_to_buf(LIGHT_CONTROL_TIME_PROLONG,
                          property_data,
                          (uint8_t *)&prolong);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Time Fade Standby Auto property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_time_fade_standby_auto_update(uint16_t element)
{
  uint8_t property_data[6];
  light_control_time_standby_auto standby_auto = lc_property_state.time_fade_standby_auto;
  mesh_sensor_data_to_buf(LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO,
                          property_data,
                          (uint8_t *)&standby_auto);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Time Fade Standby Manual property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_time_fade_standby_manual_update(uint16_t element)
{
  uint8_t property_data[6];
  light_control_time_standby_manual standby_manual = lc_property_state.time_fade_standby_manual;
  mesh_sensor_data_to_buf(LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL,
                          property_data,
                          (uint8_t *)&standby_manual);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Lightness On property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_lightness_on_update(uint16_t element)
{
  uint8_t property_data[5];
  mesh_sensor_data_to_buf(LIGHT_CONTROL_LIGHTNESS_ON,
                          property_data,
                          (uint8_t *)&lc_property_state.lightness_on);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Lightness Prolong property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_lightness_prolong_update(uint16_t element)
{
  uint8_t property_data[5];
  mesh_sensor_data_to_buf(LIGHT_CONTROL_LIGHTNESS_PROLONG,
                          property_data,
                          (uint8_t *)&lc_property_state.lightness_prolong);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Lightness Standby property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_lightness_standby_update(uint16_t element)
{
  uint8_t property_data[5];
  mesh_sensor_data_to_buf(LIGHT_CONTROL_LIGHTNESS_STANDBY,
                          property_data,
                          (uint8_t *)&lc_property_state.lightness_standby);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Ambient LuxLevel On property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_ambient_luxlevel_on_update(uint16_t element)
{
  uint8_t property_data[6];
  illuminance_t ambient_luxlevel_on = lc_property_state.ambient_luxlevel_on;
  mesh_sensor_data_to_buf(LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON,
                          property_data,
                          (uint8_t *)&ambient_luxlevel_on);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Ambient LuxLevel Prolong property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_ambient_luxlevel_prolong_update(uint16_t element)
{
  uint8_t property_data[6];
  illuminance_t ambient_luxlevel_prolong = lc_property_state.ambient_luxlevel_prolong;
  mesh_sensor_data_to_buf(LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG,
                          property_data,
                          (uint8_t *)&ambient_luxlevel_prolong);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Ambient LuxLevel Standby property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_ambient_luxlevel_standby_update(uint16_t element)
{
  uint8_t property_data[6];
  illuminance_t ambient_luxlevel_standby = lc_property_state.ambient_luxlevel_standby;
  mesh_sensor_data_to_buf(LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY,
                          property_data,
                          (uint8_t *)&ambient_luxlevel_standby);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Regulator Kiu property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_regulator_kiu_update(uint16_t element)
{
  uint8_t property_data[7];
  mesh_sensor_data_to_buf(LIGHT_CONTROL_REGULATOR_KIU,
                          property_data,
                          (uint8_t *)&lc_property_state.regulator_kiu);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Regulator Kid property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_regulator_kid_update(uint16_t element)
{
  uint8_t property_data[7];
  mesh_sensor_data_to_buf(LIGHT_CONTROL_REGULATOR_KID,
                          property_data,
                          (uint8_t *)&lc_property_state.regulator_kid);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Regulator Kpu property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_regulator_kpu_update(uint16_t element)
{
  uint8_t property_data[7];
  mesh_sensor_data_to_buf(LIGHT_CONTROL_REGULATOR_KPU,
                          property_data,
                          (uint8_t *)&lc_property_state.regulator_kpu);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Regulator Kpd property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_regulator_kpd_update(uint16_t element)
{
  uint8_t property_data[7];
  mesh_sensor_data_to_buf(LIGHT_CONTROL_REGULATOR_KPD,
                          property_data,
                          (uint8_t *)&lc_property_state.regulator_kpd);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update Light LC Regulator Accuracy property in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_regulator_accuracy_update(uint16_t element)
{
  uint8_t property_data[4];
  mesh_sensor_data_to_buf(LIGHT_CONTROL_REGULATOR_ACCURACY,
                          property_data,
                          (uint8_t *)&lc_property_state.regulator_accuracy);
  update_property(element, property_data);
}

/***************************************************************************//**
 * This function update all light controller properties in stack.
 *
 * @param[in] element  Index of the element.
 ******************************************************************************/
static void lc_property_state_update(uint16_t element)
{
  lc_time_occupancy_delay_update(element);
  lc_time_fade_on_update(element);
  lc_time_run_on_update(element);
  lc_time_fade_update(element);
  lc_time_prolong_update(element);
  lc_time_fade_standby_auto_update(element);
  lc_time_fade_standby_manual_update(element);
  lc_lightness_on_update(element);
  lc_lightness_prolong_update(element);
  lc_lightness_standby_update(element);
  lc_ambient_luxlevel_on_update(element);
  lc_ambient_luxlevel_prolong_update(element);
  lc_ambient_luxlevel_standby_update(element);
  lc_regulator_kiu_update(element);
  lc_regulator_kid_update(element);
  lc_regulator_kpu_update(element);
  lc_regulator_kpd_update(element);
  lc_regulator_accuracy_update(element);
}

/*******************************************************************************
 * LC initialization.
 * This should be called at each boot if provisioning is already done.
 * Otherwise this function should be called after provisioning is completed.
 *
 * @param[in] element  Index of the element where LC model is initialized.
 *
 * @return Status of the initialization operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
sl_status_t sl_btmesh_lc_init(void)
{
  // Initialize lc server models
  const uint16_t element = BTMESH_LC_SERVER_LIGHT_LC;
  sl_status_t result;
  sl_status_t sc;

  result = sl_btmesh_lc_server_init(element);
  log_status_f(result,
               "sl_btmesh_lc_server_init failed (elem=%d)\r\n",
               element);

  memset(&lc_state, 0, sizeof(lc_state));
  lc_state_load();

  memset(&lc_property_state, 0, sizeof(lc_property_state));
  lc_property_state_load();

  // Set the regulator interval to 100 milliseconds. If you want to use shorter
  // intervals, you should disable some logs in order not to affect performance.
  sc = sl_btmesh_lc_server_set_regulator_interval(element, 100);
  log_status_f(sc, "sl_btmesh_lc_server_init failed (elem=%d)\r\n", element);

  lc_property_state_update(element);
  lc_property_state_changed();

  lc_onpowerup_update(element, sl_btmesh_get_lightness_onpowerup());

  init_models();

  // The status code of the sl_btmesh_lc_server_init is returned because the
  // successful initialization of the btmesh stack lc feature is essential
  // for the proper behavior of the module while the improper setup of some
  // properties and states are not that critical.
  return result;
}

/***************************************************************************//**
 * Handling of lc server mode updated event.
 *
 * @param[in] evt  Pointer to lc server mode updated event.
 ******************************************************************************/
static void handle_lc_server_mode_updated_event(
  sl_btmesh_evt_lc_server_mode_updated_t *evt)
{
  lc_state.mode = evt->mode_value;
  lc_state_changed();
}

/***************************************************************************//**
 * Handling of lc server occupancy mode updated event.
 *
 * @param[in] evt  Pointer to lc server occupancy mode updated event.
 ******************************************************************************/
static void handle_lc_server_om_updated_event(
  sl_btmesh_evt_lc_server_om_updated_t *evt)
{
  log("evt:sl_btmesh_evt_lc_server_om_updated_id, om=%u\r\n", evt->om_value);
  lc_state.occupancy_mode = evt->om_value;
  lc_state_changed();
}

/***************************************************************************//**
 * Handling of lc server light onoff updated event.
 *
 * @param[in] evt  Pointer to lc server light onoff updated event.
 ******************************************************************************/
static void handle_lc_server_light_onoff_updated_event(
  sl_btmesh_evt_lc_server_light_onoff_updated_t *evt)
{
  lc_state.light_onoff = evt->onoff_state;
  lc_state_changed();
}

/***************************************************************************//**
 * Handling of lc server linear output updated event.
 *
 * @param[in] evt  Pointer to lc server linear output updated event.
 ******************************************************************************/
static void handle_lc_server_linear_output_updated_event(
  sl_btmesh_evt_lc_server_linear_output_updated_t *evt)
{
  // Convert from linear to actual lightness value
  uint32_t lightness = (uint32_t)sqrt(65535
                                      * (uint32_t)(evt->linear_output_value));
  // Update LED
  sl_btmesh_lighting_set_level(lightness, IMMEDIATE);
}

/***************************************************************************//**
 * Printing the float number using integers.
 *
 * @param[in] number  Number to print.
 ******************************************************************************/
static void print_float(float number)
{
  if (number > INT32_MAX) {
    log("> %ld", INT32_MAX);
  } else if (number < INT32_MIN) {
    log("< %ld", INT32_MIN);
  } else {
    log("%ld.%03u", (int32_t)number, FRACTION(number));
  }
}

/***************************************************************************//**
 * Handling of lc setup server set property event.
 *
 * @param[in] evt  Pointer to lc setup server set property event.
 ******************************************************************************/
static void handle_lc_setup_server_set_property(
  sl_btmesh_evt_lc_setup_server_set_property_t *evt)
{
  for (int i = 0; i < evt->property_value.len; i++) {
    log("%2.2x", evt->property_value.data[i]);
  }
  log("\r\n");

  switch (evt->property_id) {
    case LIGHT_CONTROL_TIME_OCCUPANCY_DELAY:
      lc_property_state.time_occupancy_delay =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_TIME_OCCUPANCY_DELAY,
                                  evt->property_value.data)
        .time_millisecond_24;
      log("Light Control Time Occupancy Delay = %u.%03us\r\n",
          lc_property_state.time_occupancy_delay / 1000,
          lc_property_state.time_occupancy_delay % 1000);
      break;

    case LIGHT_CONTROL_TIME_FADE_ON:
      lc_property_state.time_fade_on =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_TIME_FADE_ON,
                                  evt->property_value.data)
        .time_millisecond_24;
      log("Light Control Time Fade On = %u.%03us\r\n",
          lc_property_state.time_fade_on / 1000,
          lc_property_state.time_fade_on % 1000);
      break;

    case LIGHT_CONTROL_TIME_RUN_ON:
      lc_property_state.time_run_on =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_TIME_RUN_ON,
                                  evt->property_value.data)
        .time_millisecond_24;
      log("Light Control Time Run On = %u.%03us\r\n",
          lc_property_state.time_run_on / 1000,
          lc_property_state.time_run_on % 1000);
      break;

    case LIGHT_CONTROL_TIME_FADE:
      lc_property_state.time_fade =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_TIME_FADE,
                                  evt->property_value.data)
        .time_millisecond_24;
      log("Light Control Time Fade = %u.%03us\r\n",
          lc_property_state.time_fade / 1000,
          lc_property_state.time_fade % 1000);
      break;

    case LIGHT_CONTROL_TIME_PROLONG:
      lc_property_state.time_prolong =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_TIME_PROLONG,
                                  evt->property_value.data)
        .time_millisecond_24;
      log("Light Control Time Prolong = %u.%03us\r\n",
          lc_property_state.time_prolong / 1000,
          lc_property_state.time_prolong % 1000);
      break;

    case LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO:
      lc_property_state.time_fade_standby_auto =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO,
                                  evt->property_value.data)
        .time_millisecond_24;
      log("Light Control Time Fade Standby Auto = %u.%03us\r\n",
          lc_property_state.time_fade_standby_auto / 1000,
          lc_property_state.time_fade_standby_auto % 1000);
      break;

    case LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL:
      lc_property_state.time_fade_standby_manual =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL,
                                  evt->property_value.data)
        .time_millisecond_24;
      log("Light Control Time Fade Standby Manual = %u.%03us\r\n",
          lc_property_state.time_fade_standby_manual / 1000,
          lc_property_state.time_fade_standby_manual % 1000);
      break;

    case LIGHT_CONTROL_LIGHTNESS_ON:
      lc_property_state.lightness_on =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_LIGHTNESS_ON,
                                  evt->property_value.data)
        .uint16;
      log("Light Control Lightness On = %u\r\n",
          lc_property_state.lightness_on);
      break;

    case LIGHT_CONTROL_LIGHTNESS_PROLONG:
      lc_property_state.lightness_prolong =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_LIGHTNESS_PROLONG,
                                  evt->property_value.data)
        .uint16;
      log("Light Control Lightness Prolong = %u\r\n",
          lc_property_state.lightness_prolong);
      break;

    case LIGHT_CONTROL_LIGHTNESS_STANDBY:
      lc_property_state.lightness_standby =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_LIGHTNESS_STANDBY,
                                  evt->property_value.data)
        .uint16;
      log("Light Control Lightness Standby = %u\r\n",
          lc_property_state.lightness_standby);
      break;

    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON:
      lc_property_state.ambient_luxlevel_on =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON,
                                  evt->property_value.data)
        .illuminance;
      log("Light Control Ambient LuxLevel On = %u.%02ulux\r\n",
          lc_property_state.ambient_luxlevel_on / 100,
          lc_property_state.ambient_luxlevel_on % 100);
      break;

    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG:
      lc_property_state.ambient_luxlevel_prolong =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG,
                                  evt->property_value.data)
        .illuminance;
      log("Light Control Ambient LuxLevel Prolong = %u.%02ulux\r\n",
          lc_property_state.ambient_luxlevel_prolong / 100,
          lc_property_state.ambient_luxlevel_prolong % 100);
      break;

    case LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY:
      lc_property_state.ambient_luxlevel_standby =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY,
                                  evt->property_value.data)
        .illuminance;
      log("Light Control Ambient LuxLevel Standby = %u.%02ulux\r\n",
          lc_property_state.ambient_luxlevel_standby / 100,
          lc_property_state.ambient_luxlevel_standby % 100);
      break;

    case LIGHT_CONTROL_REGULATOR_KIU:
      lc_property_state.regulator_kiu =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_REGULATOR_KIU,
                                  evt->property_value.data)
        .coefficient;
      log("Light Control Regulator Kiu = ");
      print_float(lc_property_state.regulator_kiu);
      log("\r\n");
      break;

    case LIGHT_CONTROL_REGULATOR_KID:
      lc_property_state.regulator_kid =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_REGULATOR_KID,
                                  evt->property_value.data)
        .coefficient;
      log("Light Control Regulator Kid = ");
      print_float(lc_property_state.regulator_kid);
      log("\r\n");
      break;

    case LIGHT_CONTROL_REGULATOR_KPU:
      lc_property_state.regulator_kpu =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_REGULATOR_KPU,
                                  evt->property_value.data)
        .coefficient;
      log("Light Control Regulator Kpu = ");
      print_float(lc_property_state.regulator_kpu);
      log("\r\n");
      break;

    case LIGHT_CONTROL_REGULATOR_KPD:
      lc_property_state.regulator_kpd =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_REGULATOR_KPD,
                                  evt->property_value.data)
        .coefficient;
      log("Light Control Regulator Kpd = ");
      print_float(lc_property_state.regulator_kpd);
      log("\r\n");
      break;

    case LIGHT_CONTROL_REGULATOR_ACCURACY:
      lc_property_state.regulator_accuracy =
        mesh_sensor_data_from_buf(LIGHT_CONTROL_REGULATOR_ACCURACY,
                                  evt->property_value.data)
        .percentage;
      if (lc_property_state.regulator_accuracy == 0xFF) {
        log("Light Control Regulator Accuracy = Value is not known\r\n");
      } else {
        log("Light Control Regulator Accuracy = %u.%u%%\r\n",
            lc_property_state.regulator_accuracy / 2,
            (lc_property_state.regulator_accuracy % 2) * 5);
      }
      break;

    default:
      break;
  }

  lc_property_state_changed();
}

/*******************************************************************************
 * Handle LC Server events.
 *
 * This function is called automatically by Universal Configurator after
 * enabling the component.
 *
 * @param[in] evt  Pointer to incoming event.
 ******************************************************************************/
void sl_btmesh_lc_server_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_lc_server_mode_updated_id:
      handle_lc_server_mode_updated_event(
        &(evt->data.evt_lc_server_mode_updated));
      break;

    case sl_btmesh_evt_lc_server_om_updated_id:
      handle_lc_server_om_updated_event(
        &(evt->data.evt_lc_server_om_updated));
      break;

    case sl_btmesh_evt_lc_server_light_onoff_updated_id:
      handle_lc_server_light_onoff_updated_event(
        &(evt->data.evt_lc_server_light_onoff_updated));
      break;

    case sl_btmesh_evt_lc_server_linear_output_updated_id:
      handle_lc_server_linear_output_updated_event(
        &(evt->data.evt_lc_server_linear_output_updated));
      break;

    case sl_btmesh_evt_lc_setup_server_set_property_id:
      handle_lc_setup_server_set_property(
        &(evt->data.evt_lc_setup_server_set_property));
      break;

    case sl_btmesh_evt_node_provisioned_id:
      sl_btmesh_lc_init();
      break;

    case sl_btmesh_evt_node_initialized_id:
      if (evt->data.evt_node_initialized.provisioned) {
        sl_btmesh_lc_init();
      }
      break;

    case sl_btmesh_evt_node_reset_id:
      sl_bt_nvm_erase(SL_BTMESH_LC_SERVER_PS_KEY_CFG_VAL);
      sl_bt_nvm_erase(SL_BTMESH_LC_SERVER_PROPERTY_PS_KEY_CFG_VAL);
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 * @addtogroup LC_GenericOnOff
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Response to LC generic on/off request.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] appkey_index   The application key index used in encrypting.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the response operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lc_onoff_response(uint16_t element_index,
                                     uint16_t client_addr,
                                     uint16_t appkey_index,
                                     uint32_t remaining_ms)
{
  sl_status_t sc;
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_on_off;
  current.on_off.on = lc_state.onoff_current;

  target.kind = mesh_generic_state_on_off;
  target.on_off.on = lc_state.onoff_target;

  sc = mesh_lib_generic_server_respond(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                       element_index,
                                       client_addr,
                                       appkey_index,
                                       &current,
                                       &target,
                                       remaining_ms,
                                       0x00);

  log_status_level_f(APP_LOG_LEVEL_ERROR,
                     sc,
                     "LC server respond failed "
                     "(claddr=0x%04x,mdl=0x%04x,elem=%d,state=0x%04x)\r\n",
                     client_addr,
                     MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                     element_index,
                     current.kind);
  return sc;
}

/***************************************************************************//**
 * Update LC generic on/off state.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lc_onoff_update(uint16_t element_index,
                                   uint32_t remaining_ms)
{
  sl_status_t sc;
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_on_off;
  current.on_off.on = lc_state.onoff_current;

  target.kind = mesh_generic_state_on_off;
  target.on_off.on = lc_state.onoff_target;

  sc = mesh_lib_generic_server_update(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                      element_index,
                                      &current,
                                      &target,
                                      remaining_ms);

  log_status_level_f(APP_LOG_LEVEL_ERROR,
                     sc,
                     "LC server state update failed "
                     "(mdl=0x%04x,elem=%d,state=0x%04x)\r\n",
                     MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                     element_index,
                     current.kind);
  return sc;
}

/***************************************************************************//**
 * Update LC generic on/off state and publish model state to the network.
 *
 * @param[in] element_index  Server model element index.
 * @param[in] remaining_ms   The remaining time in milliseconds.
 *
 * @return Status of the update and publish operation.
 *         Returns SL_STATUS_OK (0) if succeed, non-zero otherwise.
 ******************************************************************************/
static sl_status_t lc_onoff_update_and_publish(uint16_t element_index,
                                               uint32_t remaining_ms)
{
  sl_status_t sc;

  sc = lc_onoff_update(element_index, remaining_ms);
  if (sc == SL_STATUS_OK) {
    sc = mesh_lib_generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                         element_index,
                                         mesh_generic_state_on_off);
    log_btmesh_status_f(sc,
                        "LC server state publish failed "
                        "(mdl=0x%04x,elem=%d,state=0x%04x)\r\n",
                        MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                        element_index,
                        mesh_generic_state_on_off);
  }
  return sc;
}

/*******************************************************************************
 * This function process the requests for the LC generic on/off model.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] server_addr    Address the message was sent to.
 * @param[in] appkey_index   The application key index used in encrypting the request.
 * @param[in] request        Pointer to the request structure.
 * @param[in] transition_ms  Requested transition time (in milliseconds).
 * @param[in] delay_ms       Delay time (in milliseconds).
 * @param[in] request_flags  Message flags. Bitmask of the following:
 *                           - Bit 0: Nonrelayed. If nonzero indicates
 *                                    a response to a nonrelayed request.
 *                           - Bit 1: Response required. If nonzero client
 *                                    expects a response from the server.
 ******************************************************************************/
static void lc_onoff_request(uint16_t model_id,
                             uint16_t element_index,
                             uint16_t client_addr,
                             uint16_t server_addr,
                             uint16_t appkey_index,
                             const struct mesh_generic_request *request,
                             uint32_t transition_ms,
                             uint16_t delay_ms,
                             uint8_t request_flags)
{
  (void)model_id;
  (void)server_addr;
  sl_status_t sc;

  log("LC ON/OFF request: requested state=<%s>, transition=%lu, delay=%u\r\n",
      request->on_off ? "ON" : "OFF", transition_ms, delay_ms);

  if (lc_state.onoff_current == request->on_off) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Turning LC light <%s>\r\n", request->on_off ? "ON" : "OFF");
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lc_state.onoff_current = request->on_off;
      lc_state.onoff_target = request->on_off;
    } else if (delay_ms > 0) {
      // a delay has been specified for the lc light change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lc_state.onoff_target = request->on_off;
      sc = sl_simple_timer_start(&lc_delayed_onoff_timer,
                                 delay_ms,
                                 lc_delayed_onoff_timer_cb,
                                 NO_CALLBACK_DATA,
                                 false);
      app_assert_status_f(sc, "Failed to start LC Delayed Onoff Timer\n");
      // store transition parameter for later use
      delayed_lc_onoff_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lc_state.onoff_target = request->on_off;
      if (lc_state.onoff_target == MESH_GENERIC_ON_OFF_STATE_ON) {
        lc_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_ON;
      }
      lc_onoff_update(element_index, transition_ms);

      // lc current state will be updated when transition is complete
      sc = sl_simple_timer_start(&lc_onoff_transition_timer,
                                 transition_ms,
                                 lc_onoff_transition_timer_cb,
                                 NO_CALLBACK_DATA,
                                 false);
      app_assert_status_f(sc, "Failed to start LC Onoff Transition timer\n");
    }
    lc_state_changed();

#ifdef SL_CATALOG_BTMESH_SCENE_SERVER_PRESENT
    // State has changed, so the current scene number is reset
    sc = sl_btmesh_scene_server_reset_register(element_index);

    // The function can fail if there is no scene server model in the element or
    // the btmesh_feature_scene_server component is not present. Both of these
    // are configuration issues so assert can be used.
    app_assert_status_f(sc, "Failed to reset scene register.\n");
#endif
  }

  uint32_t remaining_ms = delay_ms + transition_ms;
  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    lc_onoff_response(element_index, client_addr, appkey_index, remaining_ms);
  }
  lc_onoff_update_and_publish(element_index, remaining_ms);
}

/*******************************************************************************
 * This function is a handler for LC generic on/off change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void lc_onoff_change(uint16_t model_id,
                            uint16_t element_index,
                            const struct mesh_generic_state *current,
                            const struct mesh_generic_state *target,
                            uint32_t remaining_ms)
{
  (void)model_id;
  (void)element_index;
  (void)target;
  (void)remaining_ms;

  if (current->on_off.on != lc_state.onoff_current) {
    log("LC ON/OFF state changed %u to %u\r\n",
        lc_state.onoff_current,
        current->on_off.on);

    lc_state.onoff_current = current->on_off.on;
    lc_state_changed();
  } else {
    log("Dummy LC ON/OFF change - same state as before\r\n");
  }
}

/*******************************************************************************
 * This function is a handler for LC generic on/off recall event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] transition_ms  Transition time (in milliseconds).
 ******************************************************************************/
static void lc_onoff_recall(uint16_t model_id,
                            uint16_t element_index,
                            const struct mesh_generic_state *current,
                            const struct mesh_generic_state *target,
                            uint32_t transition_ms)
{
  (void)model_id;

  log("LC Generic ON/OFF recall\r\n");
  if (transition_ms == IMMEDIATE) {
    lc_state.onoff_target = current->on_off.on;
  } else {
    lc_state.onoff_target = target->on_off.on;
  }

  if (lc_state.onoff_current == lc_state.onoff_target) {
    log("Request for current state received; no op\r\n");
  } else {
    log("Recall ON/OFF state <%s> with transition=%lu ms\r\n",
        lc_state.onoff_target ? "ON" : "OFF",
        transition_ms);

    if (transition_ms == IMMEDIATE) {
      lc_state.onoff_current = current->on_off.on;
    } else {
      if (lc_state.onoff_target == MESH_GENERIC_ON_OFF_STATE_ON) {
        lc_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_ON;
      }
      // lc current state will be updated when transition is complete
      sl_status_t sc = sl_simple_timer_start(&lc_onoff_transition_timer,
                                             transition_ms,
                                             lc_onoff_transition_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start LC Onoff Transition timer\n");
    }
    lc_state_changed();
  }

  lc_onoff_update_and_publish(element_index, transition_ms);
}

/***************************************************************************//**
 * This function is called when a LC on/off request
 * with non-zero transition time has completed.
 ******************************************************************************/
static void lc_onoff_transition_complete(void)
{
  // transition done -> set state, update and publish
  lc_state.onoff_current = lc_state.onoff_target;

  log("Transition complete. New state is %s\r\n",
      lc_state.onoff_current ? "ON" : "OFF");

  lc_state_changed();
  lc_onoff_update_and_publish(BTMESH_LC_SERVER_LIGHT_LC,
                              IMMEDIATE);
}

/***************************************************************************//**
 * This function is called when delay for LC on/off request has completed.
 ******************************************************************************/
static void delayed_lc_onoff_request(void)
{
  log("Starting delayed LC ON/OFF request: %u -> %u, %lu ms\r\n",
      lc_state.onoff_current,
      lc_state.onoff_target,
      delayed_lc_onoff_trans
      );

  if (delayed_lc_onoff_trans == 0) {
    // no transition delay, update state immediately

    lc_state.onoff_current = lc_state.onoff_target;
    lc_state_changed();
    lc_onoff_update_and_publish(BTMESH_LC_SERVER_LIGHT_LC,
                                delayed_lc_onoff_trans);
  } else {
    if (lc_state.onoff_target == MESH_GENERIC_ON_OFF_STATE_ON) {
      lc_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_ON;
      lc_onoff_update(BTMESH_LC_SERVER_LIGHT_LC, delayed_lc_onoff_trans);
    }

    // state is updated when transition is complete
    sl_status_t sc = sl_simple_timer_start(&lc_onoff_transition_timer,
                                           delayed_lc_onoff_trans,
                                           lc_onoff_transition_timer_cb,
                                           NO_CALLBACK_DATA,
                                           false);
    app_assert_status_f(sc, "Failed to start LC Onoff Transition timer\n");
  }
}

/***************************************************************************//**
 * Initialization of the models supported by this node.
 * This function registers callbacks for each of the supported models.
 ******************************************************************************/
static void init_models(void)
{
  sl_status_t sc;
  sc = mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                                BTMESH_LC_SERVER_LIGHT_LC,
                                                lc_onoff_request,
                                                lc_onoff_change,
                                                lc_onoff_recall);
  app_assert_status_f(sc,
                      "LC server failed to register handlers (mdl=0x%04x,elem=%d)\n",
                      MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                      BTMESH_LC_SERVER_LIGHT_LC);
}

/**************************************************************************//**
 * Timer Callbacks
 *****************************************************************************/
static void lc_save_state_timer_cb(sl_simple_timer_t *handle,
                                   void *data)
{
  (void)data;
  (void)handle;
  // save the light controller state
  lc_state_store();
}

static void lc_save_property_state_timer_cb(sl_simple_timer_t *handle,
                                            void *data)
{
  (void)data;
  (void)handle;
  // save the light controller property state
  lc_property_state_store();
}

static void lc_onoff_transition_timer_cb(sl_simple_timer_t *handle,
                                         void *data)
{
  (void)data;
  (void)handle;
  // transition for lc on/off request has completed, update the lc state
  lc_onoff_transition_complete();
}

static void lc_delayed_onoff_timer_cb(sl_simple_timer_t *handle,
                                      void *data)
{
  (void)data;
  (void)handle;
  // delay for lc on/off request has passed, now process the request
  delayed_lc_onoff_request();
}
/** @} (end addtogroup LC_Server) */
