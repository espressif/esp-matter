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
#ifndef SL_SENSOR_RHT_CONFIG_H
#define SL_SENSOR_RHT_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#define SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_DISCRETE_VALUE     0
#define SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_PERCENTAGE         1
// Minimum delta cadence value in case of percentage trigger type
#define SENSOR_THERMOMETER_MIN_PERCENTAGE_DELTA_VALUE             0
// Maximum delta cadence value in case of percentage trigger type
#define SENSOR_THERMOMETER_MAX_PERCENTAGE_DELTA_VALUE             65535
// Minimum delta cadence value in case of discrete trigger type
#define SENSOR_THERMOMETER_MIN_DISCRETE_DELTA_VALUE               -128
// Maximum delta cadence value in case of discrete trigger type
#define SENSOR_THERMOMETER_MAX_DISCRETE_DELTA_VALUE               127

// <h> Bluetooth Mesh - Temperature Sensor

// <h> Sensor attributes

// <o SENSOR_THERMOMETER_POSITIVE_TOLERANCE> Positive tolerance of sensor.
// <0-4095:1>
// <i> Default: 0 (Unspecified)
// <i> 12-bit Positive Tolerance value (1 - 4095) or Unspecified (0). The value is derived as ERR_P [%] = 100 [%] * x / 4095
#define SENSOR_THERMOMETER_POSITIVE_TOLERANCE 0

// <o SENSOR_THERMOMETER_NEGATIVE_TOLERANCE> Negative tolerance of sensor.
// <0-4095:1>
// <i> Default: 0 (Unspecified)
// <i> 12-bit Negative Tolerance value (1 - 4095) or Unspecified (0). The value is derived as ERR_N [%] = 100 [%] * x / 4095
#define SENSOR_THERMOMETER_NEGATIVE_TOLERANCE 0

// <o SENSOR_THERMOMETER_SAMPLING_FUNCTION> Sampling function
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
#define SENSOR_THERMOMETER_SAMPLING_FUNCTION       SAMPLING_UNSPECIFIED

// <o SENSOR_THERMOMETER_MEASUREMENT_PERIOD> Measurement Period of sensor.
// <0-255:1>
// <i> Default: 0 (Not Applicable)
// <i> 8 bit value (1 - 255) or Not Applicable (0). Time period in seconds is derived as T [s] = 1.1 ^ (x - 64)
#define SENSOR_THERMOMETER_MEASUREMENT_PERIOD 0

// <o SENSOR_THERMOMETER_UPDATE_INTERVAL> Update Interval of sensor.
// <0-255:1>
// <i> Default: 0 (Not Applicable)
// <i> 8 bit value (1-255) or Not Applicable (0). Update interval in seconds is derived as I [s] = 1.1 ^ (x - 64)
#define SENSOR_THERMOMETER_UPDATE_INTERVAL 0

// </h>

// <e SENSOR_THERMOMETER_CADENCE_ENABLE> Sensor cadence
// <i> Enables Cadence.
// <i> Default: 0
#define SENSOR_THERMOMETER_CADENCE_ENABLE             0

// <o SENSOR_THERMOMETER_FAST_CADENCE_PERIOD_DIVISOR> Fast Cadence Period Divisor
// <0-15:1>
// <i> Default: 0 (Divisor of 1)
// <i> 7 bit value (0-15), other values are Prohibited. The value is represented as a 2 ^ n divisor of the Publish Period.
// <i> For example value 0x00 would have a divisor of 1, the Publish Period would not change.
#define SENSOR_THERMOMETER_FAST_CADENCE_PERIOD_DIVISOR 0

// <o SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE> Status Trigger Type
// <SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_DISCRETE_VALUE=> Discrete Value
// <SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_PERCENTAGE=> Percentage
// <i> Default: SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_DISCRETE_VALUE
// <i> Defines the unit and format of the Status Trigger Delta fields
#define SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE          SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_DISCRETE_VALUE

// <o SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_DOWN> Status Trigger Delta Down
// <i> Default: 0
// <i> The Status Trigger Delta Down field shall control the negative change of a measured quantity that
// <i> triggers publication of a Sensor Status message. The format is defined by the Status Trigger Type field.
// <i> In case of percentage Status Trigger Type the value is represented unitless with a resolution of 0.01 percent,
// <i> e.g. value 1534 represents 15.34%. In case of discrete Status Trigger Type the format is degree Celsius
// <i> with a resolution of 0.5, e.g. value 8 represents 4°C.
#define SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_DOWN    0

// <o SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_UP> Status Trigger Delta Up
// <i> Default: 0
// <i> The Status Trigger Delta Up field shall control the positive change of a measured quantity that
// <i> triggers publication of a Sensor Status message. The format is defined by the Status Trigger Type field.
// <i> In case of percentage Status Trigger Type the value is represented unitless with a resolution of 0.01 percent,
// <i> e.g. value 1534 represents 15.34%. In case of discrete Status Trigger Type the format is degree Celsius
// <i> with a resolution of 0.5, e.g. value 8 represents 4°C.
#define SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_UP      0

// <o SENSOR_THERMOMETER_STATUS_MIN_INTERVAL> Status Min Interval
// <0-26:1>
// <i> Default: 0
// <i> 8 bit value (0-26), other values are Prohibited. The value is represented as a 2 ^ n milliseconds.
// <i> For example, the value 10 would represent an interval of 1024ms.
// <i> The Status Min Interval field shall control the minimum interval between publishing two consecutive Sensor Status messages.
#define SENSOR_THERMOMETER_STATUS_MIN_INTERVAL          0

// <o SENSOR_THERMOMETER_FAST_CADENCE_LOW> Fast Cadence Low
// <-128..127:1>
// <i> Default: 0
// <i> The Fast Cadence Low field shall define the lower boundary of a range of measured quantities when
// <i> the publishing cadence is increased as defined by the Fast Cadence Period Divisor field.
// <i> The format is degree Celsius with a resolution of 0.5, e.g. value 8 represents 4°C.
#define SENSOR_THERMOMETER_FAST_CADENCE_LOW             0

// <o SENSOR_THERMOMETER_FAST_CADENCE_HIGH> Fast Cadence High
// <-128..127:1>
// <i> Default: 0
// <i> The Fast Cadence High field shall define the upper boundary of a range of measured quantities when
// <i> the publishing cadence is increased as defined by the Fast Cadence Period Divisor field.
// <i> The format is degree Celsius with a resolution of 0.5, e.g. value 8 represents 4°C.
#define SENSOR_THERMOMETER_FAST_CADENCE_HIGH            0

// </e>

// </h>

// <<< end of configuration section >>>

// The delta down and delta up values shall be in valid range based on trigger type value
#if ((SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE == SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_DISCRETE_VALUE) \
  && ((SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_DOWN > SENSOR_THERMOMETER_MAX_DISCRETE_DELTA_VALUE)     \
  || (SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_DOWN < SENSOR_THERMOMETER_MIN_DISCRETE_DELTA_VALUE)      \
  || (SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_UP > SENSOR_THERMOMETER_MAX_DISCRETE_DELTA_VALUE)        \
  || (SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_UP < SENSOR_THERMOMETER_MIN_DISCRETE_DELTA_VALUE)))
#error "If in the 'Relative Humidity and Temperature sensor' component configuration               \
  the 'Status Trigger Type' is configured as 'Discrete Value' then the 'Status Trigger Delta Down' \
  and 'Status Trigger Delta Up' values valid range should be between <-128..127>"
#elif ((SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE == SENSOR_THERMOMETER_STATUS_TRIGGER_TYPE_PERCENTAGE) \
  && ((SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_DOWN > SENSOR_THERMOMETER_MAX_PERCENTAGE_DELTA_VALUE) \
  || (SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_DOWN < SENSOR_THERMOMETER_MIN_PERCENTAGE_DELTA_VALUE)  \
  || (SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_UP > SENSOR_THERMOMETER_MAX_PERCENTAGE_DELTA_VALUE)    \
  || (SENSOR_THERMOMETER_STATUS_TRIGGER_DELTA_UP < SENSOR_THERMOMETER_MIN_PERCENTAGE_DELTA_VALUE)))

#error "If in the 'Relative Humidity and Temperature sensor' component configuration           \
  the 'Status Trigger Type' is configured as 'Percentage' then the 'Status Trigger Delta Down' \
  and 'Status Trigger Delta Up' values valid range should be between <0..65535>"
#endif

#endif // SL_SENSOR_RHT_CONFIG_H
