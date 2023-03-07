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

#ifndef AOA_PARSE_ENUM_H
#define AOA_PARSE_ENUM_H

#include "sl_status.h"
#include "sl_rtl_clib_api.h"

/// Enum for locator report mode
typedef enum {
  ANGLE_REPORT = 0,
  IQ_REPORT,
} aoa_report_mode_t;

/**
 * Convert string to locator reporting mode.
 *
 * @param[in] str Input string.
 * @param[out] report_mode Locator reporting mode.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_report_mode_from_string(char *str, aoa_report_mode_t *report_mode);

/**
 * Convert locator reporting mode to string.
 *
 * @param[in] report_mode Locator reporting mode.
 * @param[out] str Output string.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_report_mode_to_string(aoa_report_mode_t report_mode, char **str);

/**
 * Convert string to AoX estimator mode.
 *
 * @param[in] str Input string.
 * @param[out] aox_mode AoA/AoD estimator mode.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_aox_mode_from_string(char *str, enum sl_rtl_aox_mode *aox_mode);

/**
 * Convert AoX estimator mode to string.
 *
 * @param[in] aox_mode AoA/AoD estimator mode.
 * @param[out] str Output string.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_aox_mode_to_string(enum sl_rtl_aox_mode aox_mode, char **str);

/**
 * Convert string to antenna array type.
 *
 * @param[in] str Input string.
 * @param[out] antenna_type Antenna array type.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_antenna_type_from_string(char *str, enum sl_rtl_aox_array_type *antenna_type);

/**
 * Convert antenna array type to string.
 *
 * @param[in] antenna_type Antenna array type.
 * @param[out] str Output string.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_antenna_type_to_string(enum sl_rtl_aox_array_type antenna_type, char **str);

/**
 * Convert string to location estimation mode.
 *
 * @param[in] str Input string.
 * @param[out] estimation_mode Location estimation mode.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_estimation_mode_from_string(char *str, enum sl_rtl_loc_estimation_mode *estimation_mode);

/**
 * Convert location estimation mode to string.
 *
 * @param[in] estimation_mode Location estimation mode.
 * @param[out] str Output string.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_estimation_mode_to_string(enum sl_rtl_loc_estimation_mode estimation_mode, char **str);

/**
 * Convert string to measurement validation mode.
 *
 * @param[in] str Input string.
 * @param[out] validation_mode Measurement validation mode.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_validation_mode_from_string(char *str, enum sl_rtl_loc_measurement_validation_method *validation_mode);

/**
 * Convert measurement validation mode to string.
 *
 * @param[in] validation_mode Measurement validation mode.
 * @param[out] str Output string.
 * @return Status ot the conversion.
 * @retval SL_STATUS_OK The conversion was successful.
 * @retval SL_STATUS_NULL_POINTER Invalid null pointer received as argument.
 * @retval SL_STATUS_INVALID_PARAMETER The input was invalid.
 */
sl_status_t aoa_parse_validation_mode_to_string(enum sl_rtl_loc_measurement_validation_method validation_mode, char **str);

#endif // AOA_PARSE_ENUM_H
