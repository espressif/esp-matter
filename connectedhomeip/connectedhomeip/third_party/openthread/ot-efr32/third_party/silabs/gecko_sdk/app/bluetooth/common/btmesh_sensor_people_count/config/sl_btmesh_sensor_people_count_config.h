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
#ifndef SL_BTMESH_SENSOR_PEOPLE_COUNT_CONFIG_H
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#define SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_TYPE_DISCRETE_VALUE_CFG_VAL   0
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_TYPE_PERCENTAGE_CFG_VAL       1

// <h> Bluetooth Mesh - People Count

// <h> Sensor attributes

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_POSITIVE_TOLERANCE_CFG_VAL> Positive tolerance of sensor.
// <0-4095:1>
// <i> Default: 0 (Unspecified)
// <i> 12-bit Positive Tolerance value (1 - 4095) or Unspecified (0). The value is derived as ERR_P [%] = 100 [%] * x / 4095
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_POSITIVE_TOLERANCE_CFG_VAL 0

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_NEGATIVE_TOLERANCE_CFG_VAL> Negative tolerance of sensor.
// <0-4095:1>
// <i> Default: 0 (Unspecified)
// <i> 12-bit Negative Tolerance value (1 - 4095) or Unspecified (0). The value is derived as ERR_N [%] = 100 [%] * x / 4095
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_NEGATIVE_TOLERANCE_CFG_VAL 0

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_SAMPLING_FUNCTION_CFG_VAL> Sampling function
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
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_SAMPLING_FUNCTION_CFG_VAL       SAMPLING_UNSPECIFIED

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_MEASUREMENT_PERIOD_CFG_VAL> Measurement Period of sensor.
// <0-255:1>
// <i> Default: 0 (Not Applicable)
// <i> 8 bit value (1 - 255) or Not Applicable (0). Time period in seconds is derived as T [s] = 1.1 ^ (x - 64)
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_MEASUREMENT_PERIOD_CFG_VAL 0

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_UPDATE_INTERVAL_CFG_VAL> Update Interval of sensor.
// <0-255:1>
// <i> Default: 0 (Not Applicable)
// <i> 8 bit value (1 - 255) or Not Applicable (0). Time period in seconds is derived as T [s] = 1.1 ^ (x - 64)
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_UPDATE_INTERVAL_CFG_VAL 0

// </h>

// <e SL_BTMESH_SENSOR_PEOPLE_COUNT_CADENCE_ENABLE_CFG_VAL> Sensor cadence
// <i> Enables Cadence.
// <i> Default: 0
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_CADENCE_ENABLE_CFG_VAL              0

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_FAST_CADENCE_PERIOD_DIVISOR_CFG_VAL> Fast Cadence Period Divisor
// <0-15:1>
// <i> Default: 0 (Divisor of 1)
// <i> 7 bit value (0-15), other values are Prohibited. The value is represented as a 2 ^ n divisor of the Publish Period.
// <i> For example value 0x00 would have a divisor of 1, the Publish Period would not change.
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_FAST_CADENCE_PERIOD_DIVISOR_CFG_VAL 0

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_TYPE_CFG_VAL> Status Trigger Type
// <SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_TYPE_DISCRETE_VALUE_CFG_VAL=> Discrete Value
// <SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_TYPE_PERCENTAGE_CFG_VAL=> Percentage
// <i> Default: SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_TYPE_DISCRETE_VALUE_CFG_VAL
// <i> Defines the unit and format of the Status Trigger Delta fields
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_TYPE_CFG_VAL         SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_TYPE_DISCRETE_VALUE_CFG_VAL

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_DELTA_DOWN_CFG_VAL> Status Trigger Delta Down
// <0-65535:1>
// <i> Default: 0
// <i> The Status Trigger Delta Down field shall control the negative change of a measured quantity that
// <i> triggers publication of a Sensor Status message. The format is defined by the Status Trigger Type field.
// <i> In case of percentage Status Trigger Type the value is represented unitless with a resolution of 0.01 percent,
// <i> e.g. value 1534 represents 15.34%. In case of discrete Status Trigger Type the format represents
// <i> the people count value.
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_DELTA_DOWN_CFG_VAL   0

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_DELTA_UP_CFG_VAL> Status Trigger Delta Up
// <0-65535:1>
// <i> Default: 0
// <i> The Status Trigger Delta Up field shall control the positive change of a measured quantity that
// <i> triggers publication of a Sensor Status message. The format is defined by the Status Trigger Type field.
// <i> In case of percentage Status Trigger Type the value is represented unitless with a resolution of 0.01 percent,
// <i> e.g. value 1534 represents 15.34%. In case of discrete Status Trigger Type the format represents
// <i> the people count value.
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_TRIGGER_DELTA_UP_CFG_VAL     0

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_MIN_INTERVAL_CFG_VAL> Status Min Interval
// <0-26:1>
// <i> Default: 0
// <i> 8 bit value (0-26), other values are Prohibited. The value is represented as a 2 ^ n milliseconds.
// <i> For example, the value 10 would represent an interval of 1024ms.
// <i> The Status Min Interval field shall control the minimum interval between publishing two consecutive Sensor Status messages.
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_STATUS_MIN_INTERVAL_CFG_VAL         0

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_FAST_CADENCE_LOW_CFG_VAL> Fast Cadence Low
// <0-65535:1>
// <i> Default: 0
// <i> The Fast Cadence Low field shall define the lower boundary of a range of measured quantities when
// <i> the publishing cadence is increased as defined by the Fast Cadence Period Divisor field.
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_FAST_CADENCE_LOW_CFG_VAL            0

// <o SL_BTMESH_SENSOR_PEOPLE_COUNT_FAST_CADENCE_HIGH_CFG_VAL> Fast Cadence High
// <0-65535:1>
// <i> Default: 0
// <i> The Fast Cadence High field shall define the upper boundary of a range of measured quantities when
// <i> the publishing cadence is increased as defined by the Fast Cadence Period Divisor field.
#define SL_BTMESH_SENSOR_PEOPLE_COUNT_FAST_CADENCE_HIGH_CFG_VAL           0

// </e>

// </h>

// <<< end of configuration section >>>

#endif // SL_BTMESH_SENSOR_PEOPLE_COUNT_CONFIG_H
