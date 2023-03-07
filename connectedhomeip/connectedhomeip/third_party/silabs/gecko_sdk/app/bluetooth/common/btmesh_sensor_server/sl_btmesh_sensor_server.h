/***************************************************************************//**
 * @file
 * @brief sl_btmesh_sensor_server.h
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

#ifndef SL_BTMESH_SENSOR_SERVER_H
#define SL_BTMESH_SENSOR_SERVER_H

#include "sl_btmesh_device_properties.h"

#define SL_BTMESH_SENSOR_LIGHT_VALUE_UNKNOWN           (0xFFFFFFFF)
#define SL_BTMESH_SENSOR_TEMPERATURE_VALUE_UNKNOWN     (0x7F)

/**************************************************************************//**
 * Initialize Sensor Server.
 *
 * This function is called automatically by Universal Configurator after
 * enabling the component.
 *
 *****************************************************************************/
void sl_btmesh_sensor_server_node_init(void);

/**************************************************************************//**
 * Handle Sensor Server events.
 *
 * This function is called automatically by Universal Configurator after
 * enabling the component.
 *
 *****************************************************************************/
void sl_btmesh_handle_sensor_server_events(sl_btmesh_msg_t* pEvt);

/***************************************************************************//**
 * Called when a temperature measurement is done
 * @note If no implementation is provided in the application then a default weak
 *       implementation if provided which is a no-operation. (empty function)
 *
 * @param[in] temperature Temperature value in 0.5 degree Celsius steps
 ******************************************************************************/
void sl_btmesh_sensor_server_on_temperature_measurement(temperature_8_t temperature);

/***************************************************************************//**
 * Called when a light measurement is done
 * @note If no implementation is provided in the application then a default weak
 *       implementation if provided which is a no-operation. (empty function)
 *
 * @param[in] temperature Temperature value in lux
 ******************************************************************************/
void sl_btmesh_sensor_server_on_light_measurement(illuminance_t light);

/***************************************************************************//**
 * Called when a people count sensor measurement is done
 * @note If no implementation is provided in the application then a default weak
 *       implementation if provided which is a no-operation. (empty function)
 *
 * @param[in] temperature People count sensor value
 ******************************************************************************/
void sl_btmesh_sensor_server_on_people_count_measurement(count16_t people);

#endif // SL_BTMESH_SENSOR_SERVER_H
