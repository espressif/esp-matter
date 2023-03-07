/***************************************************************************//**
 * @file
 * @brief sl_sensor_sink_config.h
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef __SL_SENSOR_SINK_CONFIG__
#define __SL_SENSOR_SINK_CONFIG__

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
// <<< Use Configuration Wizard in Context Menu >>>

// <h>Connect Debug Print configuration

// <q SL_SENSOR_SINK_ENDPOINT> Default endpoint for Sensor and Sink apps
// <i> Default: 1
// <i> Default endpoint for Sensor and Sink apps
#define SL_SENSOR_SINK_ENDPOINT                             (1)

// <o SL_SENSOR_SINK_TX_POWER> Default TX power level and IDs used for the network.
// <i> Default TX power level and IDs used for the network.
// <i> Default: 0
#define SL_SENSOR_SINK_TX_POWER                     (0)

// <o SL_SENSOR_SINK_PAN_ID> Sensor Sink PAN ID
// <i> Sensor Sink PAN ID
// <i> Default: 0x01FF
#define SL_SENSOR_SINK_PAN_ID                    (0x01FF)

// <o SL_SENSOR_SINK_DATA_OFFSET> Sensor Sink data offset
// <i> Sensor Sink data offset
// <i> Default: 0
#define SL_SENSOR_SINK_DATA_OFFSET                    (0)

// <o SL_SENSOR_SINK_DATA_LENGTH> Sensor Sink data length
// <i> Sensor Sink data length
// <i> Default: 8
#define SL_SENSOR_SINK_DATA_LENGTH                   (8)

// <o SL_SENSOR_SINK_SECURITY_KEY> The default security key to use for communications
// <i> Sensor The default security key to use for communications
// <i> Default: {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}
#define SL_SENSOR_SINK_SECURITY_KEY { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, \
                                      0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, \
                                      0xAA, 0xAA, 0xAA, 0xAA }

// </h>

// <<< end of configuration section >>>
#endif // __SL_SENSOR_SINK_CONFIG__
