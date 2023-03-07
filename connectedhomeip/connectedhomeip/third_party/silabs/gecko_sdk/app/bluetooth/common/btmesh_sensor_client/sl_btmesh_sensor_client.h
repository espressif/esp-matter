/***************************************************************************//**
 * @file
 * @brief sl_btmesh_sensor_client.h
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

#ifndef SL_BTMESH_SENSOR_CLIENT_H
#define SL_BTMESH_SENSOR_CLIENT_H

#include "sl_btmesh_sensor.h"
#include "sl_status.h"
#include "sl_enum.h"

// -----------------------------------------------------------------------------
// Sensor Data Constants which represents the unknown value of each sensors

#define SL_BTMESH_SENSOR_CLIENT_ILLUMINANCE_UNKNOWN  ((illuminance_t)   0xFFFFFF)
#define SL_BTMESH_SENSOR_CLIENT_PEOPLE_COUNT_UNKNOWN ((count16_t)       0xFFFF)
#define SL_BTMESH_SENSOR_CLIENT_TEMPERATURE_UNKNOWN  ((temperature_8_t) 0x7F)

/***************************************************************************//**
 * Enumeration representing the status of the sensor data received from the
 * sensor server.
 ******************************************************************************/
SL_ENUM(sl_btmesh_sensor_client_data_status_t){
  /// Valid sensor data
  SL_BTMESH_SENSOR_CLIENT_DATA_VALID = 0,

  /// No valid measured data is available
  SL_BTMESH_SENSOR_CLIENT_DATA_UNKNOWN = 1,

  /// No sensor is available on the sensor server
  SL_BTMESH_SENSOR_CLIENT_DATA_NOT_AVAILABLE = 2
};

// -----------------------------------------------------------------------------
// Callbacks

/***************************************************************************//**
 * Called when Sensor Server discovery is started
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 ******************************************************************************/
void sl_btmesh_sensor_client_on_discovery_started(uint16_t property_id);

/***************************************************************************//**
 * Called when a Device with the current Device Property ID was found
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 *
 * @param[in] property_id  New property ID for which the new device
 *                         was registered.
 * @param[in] address      Address of the new device.
 ******************************************************************************/
void sl_btmesh_sensor_client_on_new_device_found(uint16_t property_id,
                                                 uint16_t address);

/***************************************************************************//**
 * Called when temperature sensor data is received from one of the
 * registered devices
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 *
 * @param[in] sensor_idx  The sensor index represents the order the sensor
 *                        servers were registered.
 * @param[in] address     Address of the sensor server.
 * @param[in] status      Determines if the data is valid, available or unknown
 * @param[in] temperature Measured temperature on the sensor server
 *                        in 0.5 degree Celsius steps.
 ******************************************************************************/
void sl_btmesh_sensor_client_on_new_temperature_data(uint8_t sensor_idx,
                                                     uint16_t address,
                                                     sl_btmesh_sensor_client_data_status_t status,
                                                     temperature_8_t temperature);

/***************************************************************************//**
 * Called when people count sensor data is received from one of the
 * registered devices.
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 *
 * @param[in] sensor_idx  The sensor index represents the order the sensor
 *                        servers were registered.
 * @param[in] address     Address of the sensor server.
 * @param[in] status      Determines if the data is valid, available or unknown
 * @param[in] temperature Measured people count on the sensor server.
 ******************************************************************************/
void sl_btmesh_sensor_client_on_new_people_count_data(uint8_t sensor_idx,
                                                      uint16_t address,
                                                      sl_btmesh_sensor_client_data_status_t status,
                                                      count16_t people_count);

/***************************************************************************//**
 * Called when illuminance sensor data is received from one of the
 * registered devices.
 *
 * This is a callback which can be implemented in the application.
 * @note If no implementation is provided in the application then a default weak
 *       implementation is provided which is a no-operation. (empty function)
 *
 * @param[in] sensor_idx  The sensor index represents the order the sensor
 *                        servers were registered.
 * @param[in] address     Address of the sensor server.
 * @param[in] status      Determines if the data is valid, available or unknown
 * @param[in] illuminance Measured illuminance on the sensor server in lux with
 *                        a resolution of 0.01.
 *                        (0xFFFFFF represents unknown data)
 ******************************************************************************/
void sl_btmesh_sensor_client_on_new_illuminance_data(uint8_t sensor_idx,
                                                     uint16_t address,
                                                     sl_btmesh_sensor_client_data_status_t status,
                                                     illuminance_t illuminance);

// -----------------------------------------------------------------------------
// Functions

/***************************************************************************//**
 * Updating the list of registered devices.
 * All previously registered device will be lost.
 *
 * Generated callback:
 * SL_WEAK void sl_btmesh_sensor_client_on_discovery_started(uint16_t property_id);
 * SL_WEAK void sl_btmesh_sensor_client_on_new_device_found(uint16_t property_id,
 *                                                          uint16_t address);
 *
 * @param[in] property  New property ID for which devices need to be
 *                      registered.
 *
 * @return Status of the operation.
 *         Returns SL_STATUS_OK(0) if succeeded, non-zero otherwise.
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_update_registered_devices(mesh_device_properties_t property);

/***************************************************************************//**
 * Requesting new sensor data for the currently selected sensor property ID.
 *
 * Generated callback:
 *
 * Called callback signature depends on the received sensor data type.
 * The <> symbols marks the part which is different due to the different sensor
 * types in these callbacks.
 *
 * SL_WEAK void sl_btmesh_sensor_client_on_new_<sensor>_data(
 *                          uint8_t sensor_idx,
 *                          uint16_t address,
 *                          sl_btmesh_sensor_client_data_status_t status,
 *                          <sensor_data_type_t> <sensor_data>)
 *
 * @param[in] property  New property ID for which devices need to be registered.
 *
 * @return Status of the operation.
 *         Returns SL_STATUS_OK(0) if succeeded, non-zero otherwise.
 ******************************************************************************/
sl_status_t sl_btmesh_sensor_client_get_sensor_data(mesh_device_properties_t property);

// -----------------------------------------------------------------------------
// Functions which are automatically called when the component is selected

/***************************************************************************//**
 * Handle Sensor Client events.
 *
 * This function is called automatically by Universal Configurator after
 * enabling the component.
 *
 * @param[in] evt Pointer to incoming event
 ******************************************************************************/
void sl_btmesh_handle_sensor_client_on_event(sl_btmesh_msg_t *evt);

#endif // SL_BTMESH_SENSOR_CLIENT_H
