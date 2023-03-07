/***************************************************************************//**
 * @file
 * @brief
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
#ifndef SL_BTMESH_SENSOR_CLIENT_CONFIG_H
#define SL_BTMESH_SENSOR_CLIENT_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Sensor Client configuration

// <o SL_BTMESH_SENSOR_CLIENT_DISPLAYED_SENSORS_CFG_VAL> How many sensors can fit on screen
// <i> Default: 5
// <i> Defines the number of sensors which can fit on the LCD screen.
#define SL_BTMESH_SENSOR_CLIENT_DISPLAYED_SENSORS_CFG_VAL   (5)

// <e SL_BTMESH_SENSOR_CLIENT_LOGGING_CFG_VAL> Enable Logging
// <i> Default: 1
// <i> Enable / disable Logging for Sensor Client model specific messages for this component.
#define SL_BTMESH_SENSOR_CLIENT_LOGGING_CFG_VAL   (1)

// <s.128 SL_BTMESH_SENSOR_CLIENT_LOGGING_START_REGISTERING_DEVICES_CFG_VAL> Log text when registering devices starts
// <i> Set Log text in case the registration of devices with a specific sensor property ID is started
#define SL_BTMESH_SENSOR_CLIENT_LOGGING_START_REGISTERING_DEVICES_CFG_VAL "Registration of devices for property ID %4.4x started\r\n"

// <s.128 SL_BTMESH_SENSOR_CLIENT_LOGGING_REGISTERING_DEVICES_FAILED_CFG_VAL> Log text when registering devices fails
// <i> Set Log text in case the registration of devices with a specific sensor property ID is failed
#define SL_BTMESH_SENSOR_CLIENT_LOGGING_REGISTERING_DEVICES_FAILED_CFG_VAL "Registration of devices for property ID %4.4x failed\r\n"

// <s.128 SL_BTMESH_SENSOR_CLIENT_LOGGING_UNSUPPORTED_PROPERTY_CFG_VAL> Log text when unsupported sensor property ID
// <i> Set Log text in case the specific sensor property ID is not available on the remote device (e.g. no sensor)
#define SL_BTMESH_SENSOR_CLIENT_LOGGING_UNSUPPORTED_PROPERTY_CFG_VAL "Unsupported property id %4.4x\r\n"

// <s.128 SL_BTMESH_SENSOR_CLIENT_LOGGING_GET_DATA_FROM_PROPERTY_CFG_VAL> Log text when sensor data with property ID is requested
// <i> Set Log text in case property ID specific sensor data is requested from the remote device(s)
#define SL_BTMESH_SENSOR_CLIENT_LOGGING_GET_DATA_FROM_PROPERTY_CFG_VAL "Get Sensor Data from property ID %4.4x started\r\n"

// <s.128 SL_BTMESH_SENSOR_CLIENT_LOGGING_GET_DATA_FROM_PROPERTY_FAIL_CFG_VAL> Log text when sensor data request with property ID fails
// <i> Set Log text in case property ID specific sensor data request is failed
#define SL_BTMESH_SENSOR_CLIENT_LOGGING_GET_DATA_FROM_PROPERTY_FAIL_CFG_VAL "Get Sensor Data from property ID %4.4x failed\r\n"

// </e>

// </h>

// <<< end of configuration section >>>

#endif // SL_BTMESH_SENSOR_CLIENT_CONFIG_H
