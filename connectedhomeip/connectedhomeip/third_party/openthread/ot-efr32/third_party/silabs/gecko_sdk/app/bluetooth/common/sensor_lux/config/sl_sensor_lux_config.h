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
#ifndef SL_SENSOR_LUX_CONFIG_H
#define SL_SENSOR_LUX_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Bluetooth Mesh - Lux Sensor

// <o SENSOR_LUX_POSITIVE_TOLERANCE> Positive tolerance of sensor.
// <0-4095:1>
// <i> Default: 0 (Unspecified)
// <i> 12-bit Positive Tolerance value (1 - 4095) or Unspecified (0). The value is derived as ERR_P [%] = 100 [%] * x / 4095
#define SENSOR_LUX_POSITIVE_TOLERANCE 0

// <o SENSOR_LUX_NEGATIVE_TOLERANCE> Negative tolerance of sensor.
// <0-4095:1>
// <i> Default: 0 (Unspecified)
// <i> 12-bit Negative Tolerance value (1 - 4095) or Unspecified (0). The value is derived as ERR_N [%] = 100 [%] * x / 4095
#define SENSOR_LUX_NEGATIVE_TOLERANCE 0

// <o SENSOR_LUX_SAMPLING_FUNCTION> Sampling function
// <SAMPLING_UNSPECIFIED=> Unspecified
// <SAMPLING_INSTANTANEOUS=> Instantaneous sampling
// <SAMPLING_ARITHMETIC_MEAN=> Arithmetic mean
// <SAMPLING_RMS=> Root mean square
// <SAMPLING_ARITHMETIC_MEAN=> Poll
// <SAMPLING_MAXIMUM=> Maximum value
// <SAMPLING_MINIMUM=> Minimum value
// <SAMPLING_ACCUMULATED=> Cumulative moving average updated with the frequency given by Sensor Update Interval
// <SAMPLING_COUNT=> Number of "events" over the period of time defined by the Measurement Period
// <SAMPLING_RFU=> Reserved for Future Use
// <i> Default: Unspecified
#define SENSOR_LUX_SAMPLING_FUNCTION       SAMPLING_UNSPECIFIED

// <o SENSOR_LUX_MEASUREMENT_PERIOD> Measurement Period of sensor.
// <0-255:1>
// <i> Default: 0 (Not Applicable)
// <i> 8 bit value (1 - 255) or Not Applicable (0). Time period in seconds is derived as T [s] = 1.1 ^ (x - 64)
#define SENSOR_LUX_MEASUREMENT_PERIOD 0

// <o SENSOR_LUX_UPDATE_INTERVAL> Update Interval of sensor.
// <0-255:1>
// <i> Default: 0 (Not Applicable)
// <i> 8 bit value (1 - 255) or Not Applicable (0). Update interval in seconds is derived as I [s] = 1.1 ^ (x - 64)
#define SENSOR_LUX_UPDATE_INTERVAL 0

// </h>

// <<< end of configuration section >>>

#endif // SL_SENSOR_LUX_CONFIG_H
