/***************************************************************************//**
 * @file
 * @brief AoA configuration parser.
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

#ifndef AOA_PARSE_H
#define AOA_PARSE_H

#include "sl_status.h"
#include "aoa_types.h"
#include "aoa_util.h"
#ifdef RTL_LIB
#include "sl_rtl_clib_api.h"
#endif // RTL_LIB

/**************************************************************************//**
 * Load file into memory.
 *
 * @param[in] filename File to be loaded.
 * @return Pointer where the file has been loaded into.
 * @retval NULL Failed to load the file.
 *
 * @note Allocated memory has to bee freed after use.
 *****************************************************************************/
char* aoa_parse_load_file(const char *filename);

/**************************************************************************//**
 * Initialise parser module.
 *
 * @param[in] config Buffer that holds the configuration.
 *
 * @retval SL_STATUS_ALREADY_INITIALIZED - Already initialized.
 * @retval SL_STATUS_NULL_POINTER - Input is a null pointer.
 * @retval SL_STATUS_INITIALIZATION - Json module initialization failed.
 * @retval SL_STATUS_OK - Initialization completed.
 *****************************************************************************/
sl_status_t aoa_parse_init(const char *config);

/**************************************************************************//**
 * Check if config present in the config JSON.
 *
 * @param[in] config_name Configuration name.
 * @param[in] locator_id Locator id.
 *
 * @retval SL_STATUS_NOT_FOUND - Configuration not found in the JSON.
 * @retval SL_STATUS_OK - Configuration found.
 *****************************************************************************/
sl_status_t aoa_parse_check_config_exist(char *config_name,
                                         aoa_id_t locator_id);

/**************************************************************************//**
 * Parse float configuration.
 *
 * @param[out] config_value Config value.
 * @param[in] config_name Config name.
 * @param[in] locator_id Locator id. Pass NULL to omit.
 *
 * @retval SL_STATUS_NOT_INITIALIZED - Module not initialized.
 * @retval SL_STATUS_NULL_POINTER - Input is a null pointer.
 * @retval SL_STATUS_NOT_FOUND - Configuration not found.
 * @retval SL_STATUS_OK - Parsing was successful.
 *****************************************************************************/
sl_status_t aoa_parse_float_config(float *config_value,
                                   char *config_name,
                                   aoa_id_t locator_id);

/**************************************************************************//**
 * Parse bool configuration.
 *
 * @param[out] config_value Config value.
 * @param[in] config_name Config name.
 * @param[in] locator_id Locator id. Pass NULL to omit.
 *
 * @retval SL_STATUS_NOT_INITIALIZED - Module not initialized.
 * @retval SL_STATUS_NULL_POINTER - Input is a null pointer.
 * @retval SL_STATUS_NOT_FOUND - Configuration not found.
 * @retval SL_STATUS_INVALID_CONFIGURATION - Invalid config value.
 * @retval SL_STATUS_OK - Parsing was successful.
 *****************************************************************************/
sl_status_t aoa_parse_bool_config(bool *config_value,
                                  char *config_name,
                                  aoa_id_t locator_id);

/**************************************************************************//**
 * Parse string configuration.
 *
 * @param[out] config_value Config value.
 * @param[in] config_name Config name.
 * @param[in] locator_id Locator id.
 *
 * @retval SL_STATUS_NOT_INITIALIZED - Module not initialized.
 * @retval SL_STATUS_NULL_POINTER - Input is a null pointer.
 * @retval SL_STATUS_NOT_FOUND - Configuration not found.
 * @retval SL_STATUS_INVALID_CONFIGURATION - Invalid config value.
 * @retval SL_STATUS_OK - Parsing was successful.
 *****************************************************************************/
sl_status_t aoa_parse_string_config(char **config_value,
                                    char *config_name,
                                    aoa_id_t locator_id);

/**************************************************************************//**
 * Parse uint16 configuration.
 *
 * @param[out] config_value Config value.
 * @param[in] config_name Config name.
 * @param[in] locator_id Locator id. Pass NULL to omit.
 *
 * @retval SL_STATUS_NOT_INITIALIZED - Module not initialized.
 * @retval SL_STATUS_NULL_POINTER - Input is a null pointer.
 * @retval SL_STATUS_NOT_FOUND - Configuration not found.
 * @retval SL_STATUS_INVALID_CONFIGURATION - Invalid config value.
 * @retval SL_STATUS_INVALID_RANGE - Config value is out of range.
 * @retval SL_STATUS_OK - Parsing was successful.
 *****************************************************************************/
sl_status_t aoa_parse_uint16_config(uint16_t *config_value,
                                    char *config_name,
                                    aoa_id_t locator_id);

/**************************************************************************//**
 * Parse uint32 configuration.
 *
 * @param[out] config_value Config value.
 * @param[in] config_name Config name.
 *
 * @retval SL_STATUS_NOT_INITIALIZED - Module not initialized.
 * @retval SL_STATUS_NULL_POINTER - Input is a null pointer.
 * @retval SL_STATUS_NOT_FOUND - Configuration not found.
 * @retval SL_STATUS_INVALID_CONFIGURATION - Invalid config value.
 * @retval SL_STATUS_INVALID_RANGE - Config value is out of range.
 * @retval SL_STATUS_OK - Parsing was successful.
 *****************************************************************************/
sl_status_t aoa_parse_uint32_config(uint32_t *config_value,
                                    char *config_name,
                                    aoa_id_t locator_id);

#ifdef RTL_LIB
/**************************************************************************//**
 * Parse next item from the locator config list.
 *
 * @param[out] id ID of the locator.
 * @param[out] loc Locator item which contains the position and orientation.
 *
 * @retval SL_STATUS_NOT_INITIALIZED - Module not initialized.
 * @retval SL_STATUS_NULL_POINTER - Input is a null pointer.
 * @retval SL_STATUS_NOT_FOUND No more item found, use it for iteration.
 * @retval SL_STATUS_OK - Parsing was successful.
 *****************************************************************************/
sl_status_t aoa_parse_locator(aoa_id_t id,
                              struct sl_rtl_loc_locator_item *loc);
#endif

/**************************************************************************//**
 * Parse azimuth angle mask configuration.
 *
 * @param[out] min Azimuth mask minimum value in degrees.
 * @param[out] max Azimuth mask maximum value in degrees.
 * @param[in] locator_id Locator id.
 *
 * @retval SL_STATUS_NOT_INITIALIZED - Module not initialized.
 * @retval SL_STATUS_NULL_POINTER - Input is a null pointer.
 * @retval SL_STATUS_NOT_FOUND - No azimuth configuration found.
 * @retval SL_STATUS_OK -  Parsing was successful.
 *****************************************************************************/
sl_status_t aoa_parse_azimuth(float *min, float *max, aoa_id_t locator_id);

/**************************************************************************//**
 * Parse elevation angle mask configuration.
 *
 * @param[out] min Elevation mask minimum value in degrees.
 * @param[out] max Elevation mask maximum value in degrees.
 * @param[in] locator_id Locator id.
 *
 * @retval SL_STATUS_NOT_INITIALIZED - Module not initialized.
 * @retval SL_STATUS_NULL_POINTER - Input is a null pointer.
 * @retval SL_STATUS_NOT_FOUND - No azimuth configuration found.
 * @retval SL_STATUS_OK -  Parsing was successful.
 *****************************************************************************/
sl_status_t aoa_parse_elevation(float *min, float *max, aoa_id_t locator_id);

/**************************************************************************//**
 * Parse next item from the allowlist.
 *
 * @param[out] address address of the item.
 * @param[out] address_type address type of the item.
 * @param[in] locator_id Locator id.
 *
 * @retval SL_STATUS_NOT_INITIALIZED - Module not initialized.
 * @retval SL_STATUS_NULL_POINTER - Input is a null pointer.
 * @retval SL_STATUS_NOT_FOUND - No allowlist configuration found.
 * @retval SL_STATUS_OK -  Parsing was successful.
 *****************************************************************************/
sl_status_t aoa_parse_allowlist(uint8_t address[ADR_LEN],
                                uint8_t *address_type,
                                aoa_id_t locator_id);

/**************************************************************************//**
 * Parse antenna array configuration.
 *
 * @param[out] antenna_array Antenna array. Must be freed after use.
 * @param[out] array_length Antenna array length.
 * @param[in] locator_id Locator id.
 *
 * @retval SL_STATUS_NOT_INITIALIZED - Module not initialized.
 * @retval SL_STATUS_NULL_POINTER - Input is a null pointer.
 * @retval SL_STATUS_NOT_FOUND - Antenna mode configuration not found.
 * @retval SL_STATUS_OK - Parsing was successful.
 *****************************************************************************/
sl_status_t aoa_parse_antenna_array(uint8_t **antenna_array,
                                    uint8_t *array_length,
                                    aoa_id_t locator_id);

/**************************************************************************//**
 * Deinitialise parser module.
 *****************************************************************************/
sl_status_t aoa_parse_deinit(void);

#endif // AOA_PARSE_H
