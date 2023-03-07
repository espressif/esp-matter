/***************************************************************************//**
 * @file
 * @brief Time-related APIs and defines for the Price Common plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SILABS_PRICE_COMMON_TIME_H
#define SILABS_PRICE_COMMON_TIME_H

enum {
  START_OF_TIMEBASE = 0x00,
  END_OF_TIMEBASE = 0x01
};

enum {
  DURATION_TYPE_MINS = 0x00,
  DURATION_TYPE_DAYS_START   = 0x01,
  DURATION_TYPE_DAYS_END     = 0x11,
  DURATION_TYPE_WEEKS_START  = 0x02,
  DURATION_TYPE_WEEKS_END    = 0x12,
  DURATION_TYPE_MONTHS_START = 0x03,
  DURATION_TYPE_MONTHS_END   = 0x13,
};

/**
 * @brief Converts the duration to a number of seconds based on the duration type parameter.
 *
 */
uint32_t emberAfPluginPriceCommonClusterConvertDurationToSeconds(uint32_t startTimeUtc, uint32_t duration, uint8_t durationType);

/**
 * @brief Calculates a new UTC start time value based on the duration type parameter.
 *
 */
uint32_t emberAfPluginPriceCommonClusterGetAdjustedStartTime(uint32_t startTimeUtc, uint8_t durationType);

#endif // #ifndef _PRICE_COMMON_TIME_H_
