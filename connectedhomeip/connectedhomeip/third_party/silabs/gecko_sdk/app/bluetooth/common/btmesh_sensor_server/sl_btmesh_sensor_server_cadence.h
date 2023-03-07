/***************************************************************************//**
 * @file
 * @brief sl_btmesh_sensor_server_cadence.h
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

#ifndef SL_BTMESH_SENSOR_SERVER_CADENCE_H
#define SL_BTMESH_SENSOR_SERVER_CADENCE_H

#include "sl_btmesh_device_properties.h"
#include "sl_btmesh_api.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_SENSOR_RHT_PRESENT
#include "sl_sensor_rht_config.h"
#endif // SL_CATALOG_SENSOR_RHT_PRESENT

#ifdef SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT
#include "sl_btmesh_sensor_people_count_config.h"
#endif // SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT

#if defined(SL_CATALOG_SENSOR_RHT_PRESENT) \
  && defined(SL_BOARD_ENABLE_SENSOR_RHT)   \
  && SL_BOARD_ENABLE_SENSOR_RHT            \
  && SENSOR_THERMOMETER_CADENCE_ENABLE

#define SENSOR_THERMOMETER_CADENCE        1
#else
#define SENSOR_THERMOMETER_CADENCE        0
#endif

#if defined(SL_CATALOG_BTMESH_SENSOR_PEOPLE_COUNT_PRESENT) \
  && SL_BTMESH_SENSOR_PEOPLE_COUNT_CADENCE_ENABLE_CFG_VAL

#define SENSOR_PEOPLE_COUNT_CADENCE       1
#else
#define SENSOR_PEOPLE_COUNT_CADENCE       0
#endif

#if SENSOR_PEOPLE_COUNT_CADENCE
/***************************************************************************//**
 * Initialize people count sensor cadence internals.
 *
 * @param[in] people_count  People count sensor data value.
 *
 * @return none
 ******************************************************************************/
void sl_btmesh_sensor_people_count_cadence_init(count16_t people_count);

/***************************************************************************//**
 * Handle thermometer measured value cadence condition
 *
 * @param[in] people_count    People count sensor data value.
 * @param[in] publish_period  Publish event data structure
 *
 * @return publishing timer value based on cadence parameters and measured value in ms
 ******************************************************************************/
uint32_t sl_btmesh_sensor_people_count_handle_cadence(count16_t people_count, sl_btmesh_evt_sensor_server_publish_t publish_period);

/***************************************************************************//**
 * Update thermometer sensor cadence internal parameters with input cadence data.
 *
 * @param[in] evt  Pointer to sensor server set cadence request event.
 *
 * @return True if received parameters are in valid range false otherwise
 ******************************************************************************/
bool sl_btmesh_sensor_people_count_set_cadence(sl_btmesh_evt_sensor_setup_server_set_cadence_request_t* evt);

/***************************************************************************//**
 * Fill the input buffer with people count sensor cadence parameters.
 *
 * @param[out] get_cadence_buffer Parameters data buffer
 *
 * @return Parameters data buffer length
 ******************************************************************************/
uint16_t sl_btmesh_sensor_people_count_get_cadence(uint8_t length, uint8_t* get_cadence_buffer);

#endif // SENSOR_PEOPLE_COUNT_CADENCE

#if SENSOR_THERMOMETER_CADENCE
/***************************************************************************//**
 * Initialize temperature sensor cadence internals.
 *
 * @param[in] temperature  Thermometer sensor data value.
 *
 * @return none
 ******************************************************************************/
void sl_btmesh_sensor_thermometer_cadence_init(temperature_8_t temperature);

/***************************************************************************//**
 * Handle thermometer measured value cadence condition
 *
 * @param[in] temperature     Thermometer sensor data value.
 * @param[in] publish_period  Publish event data structure
 *
 * @return publishing timer value based on cadence parameters and measured value in ms
 ******************************************************************************/
uint32_t sl_btmesh_sensor_thermometer_handle_cadence(temperature_8_t temperature, sl_btmesh_evt_sensor_server_publish_t publish_period);

/***************************************************************************//**
 * Update thermometer sensor cadence internal parameters with input cadence data.
 *
 * @param[in] evt  Pointer to sensor server set cadence request event.
 *
 * @return True if received parameters are in valid range false otherwise
 ******************************************************************************/
bool sl_btmesh_sensor_thermometer_set_cadence(sl_btmesh_evt_sensor_setup_server_set_cadence_request_t* evt);

/***************************************************************************//**
 * Fill the input buffer with thermometer sensor cadence parameters.
 *
 * @param[out] get_cadence_buffer Parameters data buffer
 *
 * @return Parameters data buffer length
 ******************************************************************************/
uint16_t sl_btmesh_sensor_thermometer_get_cadence(uint8_t length, uint8_t* get_cadence_buffer);
#endif // SENSOR_THERMOMETER_CADENCE

#endif // SL_BTMESH_SENSOR_SERVER_CADENCE_H
