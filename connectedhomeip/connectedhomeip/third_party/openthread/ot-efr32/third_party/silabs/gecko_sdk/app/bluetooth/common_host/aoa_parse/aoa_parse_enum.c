/***************************************************************************//**
 * @file
 * @brief AoA configuration parser for enumerations.
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

#include <strings.h>
#include <stddef.h>
#include "aoa_parse_enum.h"

// -----------------------------------------------------------------------------
// Locator reporting mode

static const char *report_mode_str[] = {
  "ANGLE",        // Default
  "ANGLE_REPORT", // Alias 1
  "ANGLEREPORT",  // Alias 2
  "IQREPORT",     // Default
  "IQ_REPORT",    // Alias 1
  "IQ"            // Alias 2
};

static const aoa_report_mode_t report_mode_enum[] = {
  ANGLE_REPORT,
  ANGLE_REPORT,
  ANGLE_REPORT,
  IQ_REPORT,
  IQ_REPORT,
  IQ_REPORT
};

#define REPORT_MODE_COUNT    (sizeof(report_mode_str) / sizeof(report_mode_str[0]))

sl_status_t aoa_parse_report_mode_from_string(char *str, aoa_report_mode_t *report_mode)
{
  if ((str == NULL) || (report_mode == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < REPORT_MODE_COUNT; i++) {
    if (strcasecmp(str, report_mode_str[i]) == 0) {
      *report_mode = report_mode_enum[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}

sl_status_t aoa_parse_report_mode_to_string(aoa_report_mode_t report_mode, char **str)
{
  if (str == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < REPORT_MODE_COUNT; i++) {
    if (report_mode == report_mode_enum[i]) {
      *str = (char *)report_mode_str[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}

// -----------------------------------------------------------------------------
// AoX mode

#define AOX_MODE_LIST                            \
  ITEM_DEF(ONE_SHOT_BASIC),                      \
  ITEM_DEF(ONE_SHOT_BASIC_LIGHTWEIGHT),          \
  ITEM_DEF(ONE_SHOT_FAST_RESPONSE),              \
  ITEM_DEF(ONE_SHOT_HIGH_ACCURACY),              \
  ITEM_DEF(ONE_SHOT_BASIC_AZIMUTH_ONLY),         \
  ITEM_DEF(ONE_SHOT_FAST_RESPONSE_AZIMUTH_ONLY), \
  ITEM_DEF(ONE_SHOT_HIGH_ACCURACY_AZIMUTH_ONLY), \
  ITEM_DEF(REAL_TIME_FAST_RESPONSE),             \
  ITEM_DEF(REAL_TIME_BASIC),                     \
  ITEM_DEF(REAL_TIME_HIGH_ACCURACY)

static const char *aox_mode_str[] = {
  #define ITEM_DEF(i) "SL_RTL_AOA_MODE_" #i
  AOX_MODE_LIST,
  #undef ITEM_DEF
  // AoD modes are not supported.
  #define ITEM_DEF(i) "SL_RTL_AOX_MODE_" #i
  AOX_MODE_LIST
  #undef ITEM_DEF
};

static const enum sl_rtl_aox_mode aox_mode_enum[] = {
  #define ITEM_DEF(i) SL_RTL_AOA_MODE_ ## i
  AOX_MODE_LIST,
  #undef ITEM_DEF
  // AoD modes are not supported.
  #define ITEM_DEF(i) SL_RTL_AOX_MODE_ ## i
  AOX_MODE_LIST
  #undef ITEM_DEF
};

#define AOX_MODE_COUNT    (sizeof(aox_mode_str) / sizeof(aox_mode_str[0]))

sl_status_t aoa_parse_aox_mode_from_string(char *str, enum sl_rtl_aox_mode *aox_mode)
{
  if ((str == NULL) || (aox_mode == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < AOX_MODE_COUNT; i++) {
    if (strcasecmp(str, aox_mode_str[i]) == 0) {
      *aox_mode = aox_mode_enum[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}

sl_status_t aoa_parse_aox_mode_to_string(enum sl_rtl_aox_mode aox_mode, char **str)
{
  if (str == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < AOX_MODE_COUNT; i++) {
    if (aox_mode == aox_mode_enum[i]) {
      *str = (char *)aox_mode_str[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}

// -----------------------------------------------------------------------------
// Antenna array type

#define ANTENNA_TYPE_LIST \
  ITEM_DEF(4x4_URA),      \
  ITEM_DEF(3x3_URA),      \
  ITEM_DEF(1x4_ULA),      \
  ITEM_DEF(4x4_DP_URA)

static const char *antenna_type_str[] = {
  #define ITEM_DEF(i) "SL_RTL_AOX_ARRAY_TYPE_" #i
  ANTENNA_TYPE_LIST
  #undef ITEM_DEF
};

static const enum sl_rtl_aox_array_type antenna_type_enum[] = {
  #define ITEM_DEF(i) SL_RTL_AOX_ARRAY_TYPE_ ## i
  ANTENNA_TYPE_LIST,
  #undef ITEM_DEF
};

#define ANTENNA_TYPE_COUNT    (sizeof(antenna_type_str) / sizeof(antenna_type_str[0]))

sl_status_t aoa_parse_antenna_type_from_string(char *str, enum sl_rtl_aox_array_type *antenna_type)
{
  if ((str == NULL) || (antenna_type == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < ANTENNA_TYPE_COUNT; i++) {
    if (strcasecmp(str, antenna_type_str[i]) == 0) {
      *antenna_type = antenna_type_enum[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}

sl_status_t aoa_parse_antenna_type_to_string(enum sl_rtl_aox_array_type antenna_type, char **str)
{
  if (str == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < ANTENNA_TYPE_COUNT; i++) {
    if (antenna_type == antenna_type_enum[i]) {
      *str = (char *)antenna_type_str[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}

// -----------------------------------------------------------------------------
// Location estimation mode

#define ESTIMATION_MODE_LIST         \
  ITEM_DEF(THREE_DIM_FAST_RESPONSE), \
  ITEM_DEF(THREE_DIM_HIGH_ACCURACY)

static const char *estimation_mode_str[] = {
  #define ITEM_DEF(i) "SL_RTL_LOC_ESTIMATION_MODE_" #i
  ESTIMATION_MODE_LIST
  #undef ITEM_DEF
};

static const enum sl_rtl_loc_estimation_mode estimation_mode_enum[] = {
  #define ITEM_DEF(i) SL_RTL_LOC_ESTIMATION_MODE_ ## i
  ESTIMATION_MODE_LIST,
  #undef ITEM_DEF
};

#define ESTIMATION_MODE_COUNT    (sizeof(estimation_mode_str) / sizeof(estimation_mode_str[0]))

sl_status_t aoa_parse_estimation_mode_from_string(char *str, enum sl_rtl_loc_estimation_mode *estimation_mode)
{
  if ((str == NULL) || (estimation_mode == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < ESTIMATION_MODE_COUNT; i++) {
    if (strcasecmp(str, estimation_mode_str[i]) == 0) {
      *estimation_mode = estimation_mode_enum[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}

sl_status_t aoa_parse_estimation_mode_to_string(enum sl_rtl_loc_estimation_mode estimation_mode, char **str)
{
  if (str == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < ESTIMATION_MODE_COUNT; i++) {
    if (estimation_mode == estimation_mode_enum[i]) {
      *str = (char *)estimation_mode_str[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}

// -----------------------------------------------------------------------------
// Measurement validation mode

#define VALIDATION_MODE_LIST \
  ITEM_DEF(MINIMUM),         \
  ITEM_DEF(MEDIUM),          \
  ITEM_DEF(FULL)

static const char *validation_mode_str[] = {
  #define ITEM_DEF(i) "SL_RTL_LOC_MEASUREMENT_VALIDATION_" #i
  VALIDATION_MODE_LIST
  #undef ITEM_DEF
};

static const enum sl_rtl_loc_measurement_validation_method validation_mode_enum[] = {
  #define ITEM_DEF(i) SL_RTL_LOC_MEASUREMENT_VALIDATION_ ## i
  VALIDATION_MODE_LIST,
  #undef ITEM_DEF
};

#define VALIDATION_MODE_COUNT    (sizeof(validation_mode_str) / sizeof(validation_mode_str[0]))

sl_status_t aoa_parse_validation_mode_from_string(char *str, enum sl_rtl_loc_measurement_validation_method *validation_mode)
{
  if ((str == NULL) || (validation_mode == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < VALIDATION_MODE_COUNT; i++) {
    if (strcasecmp(str, validation_mode_str[i]) == 0) {
      *validation_mode = validation_mode_enum[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}

sl_status_t aoa_parse_validation_mode_to_string(enum sl_rtl_loc_measurement_validation_method validation_mode, char **str)
{
  if (str == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  for (size_t i = 0; i < VALIDATION_MODE_COUNT; i++) {
    if (validation_mode == validation_mode_enum[i]) {
      *str = (char *)validation_mode_str[i];
      return SL_STATUS_OK;
    }
  }
  return SL_STATUS_INVALID_PARAMETER;
}
