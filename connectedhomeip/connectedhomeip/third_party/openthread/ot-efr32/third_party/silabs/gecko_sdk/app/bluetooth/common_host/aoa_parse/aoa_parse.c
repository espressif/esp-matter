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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "aoa_util.h"
#include "aoa_parse.h"

// -----------------------------------------------------------------------------
// Defines

#define CHECK_TYPE(x, t)  if (((x) == NULL) || ((x)->type != (t))) return SL_STATUS_FAIL

// -----------------------------------------------------------------------------
// Private variables

static cJSON *root = NULL;
static int locator_index;

static int allowlist_index;
static int azimuth_mask_index;
static int elevation_mask_index;

// -----------------------------------------------------------------------------
// Private function declarations

static sl_status_t aoa_parse_find_locator_config(cJSON **locator,
                                                 aoa_id_t locator_id);

static sl_status_t aoa_parse_generic_config(cJSON **param,
                                            char *config_name,
                                            aoa_id_t locator_id);

// -----------------------------------------------------------------------------
// Public function definitions

/**************************************************************************//**
 * Load file into memory.
 *****************************************************************************/
char* aoa_parse_load_file(const char *filename)
{
  FILE *f = NULL;
  long fsize = 0;
  char *buffer = NULL;

  f = fopen(filename, "rb");
  if (f != NULL) {
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = malloc(fsize + 1);
    if (buffer != NULL) {
      fread(buffer, 1, fsize, f);
      buffer[fsize] = 0;
    }
    fclose(f);
  }
  return buffer;
}

/**************************************************************************//**
 * Initialise parser module.
 *****************************************************************************/
sl_status_t aoa_parse_init(const char *config)
{
  // Check preconditions.
  if (NULL != root) {
    return SL_STATUS_ALREADY_INITIALIZED;
  }
  if (NULL == config) {
    return SL_STATUS_NULL_POINTER;
  }

  root = cJSON_Parse(config);
  if (NULL == root) {
    return SL_STATUS_INITIALIZATION;
  }

  locator_index = 0;
  allowlist_index = 0;
  azimuth_mask_index = 0;
  elevation_mask_index = 0;

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Check if a config exists in the JSON.
 *****************************************************************************/
sl_status_t aoa_parse_check_config_exist(char *config_name,
                                         aoa_id_t locator_id)
{
  cJSON *item;
  cJSON *locator;
  sl_status_t sc;

  // Check for locator in positioning config
  sc = aoa_parse_find_locator_config(&locator, locator_id);
  if (sc != SL_STATUS_OK) {
    // Try to parse as single locator config
    locator = root;
  }

  // Try parse locator specific config
  item = cJSON_GetObjectItem(locator, config_name);
  if (item == NULL) {
    return SL_STATUS_NOT_FOUND;
  }

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Parse float configuration.
 *****************************************************************************/
sl_status_t aoa_parse_float_config(float *config_value,
                                   char *config_name,
                                   aoa_id_t locator_id)
{
  sl_status_t sc;
  cJSON *param;

  // Check preconditions.
  if (config_value == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sc = aoa_parse_generic_config(&param, config_name, locator_id);
  if (sc == SL_STATUS_OK) {
    if (param->type == cJSON_Number) {
      *config_value = param->valuedouble;
    } else {
      sc = SL_STATUS_INVALID_CONFIGURATION;
    }
  }
  return sc;
}

/**************************************************************************//**
 * Parse bool configuration.
 *****************************************************************************/
sl_status_t aoa_parse_bool_config(bool *config_value,
                                  char *config_name,
                                  aoa_id_t locator_id)
{
  sl_status_t sc;
  cJSON *param;

  // Check preconditions.
  if (config_value == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sc = aoa_parse_generic_config(&param, config_name, locator_id);
  if (sc == SL_STATUS_OK) {
    if (param->type == cJSON_False) {
      *config_value = false;
    } else if (param->type == cJSON_True) {
      *config_value = true;
    } else {
      sc = SL_STATUS_INVALID_CONFIGURATION;
    }
  }
  return sc;
}

/**************************************************************************//**
 * Parse string configuration.
 *****************************************************************************/
sl_status_t aoa_parse_string_config(char **config_value,
                                    char *config_name,
                                    aoa_id_t locator_id)
{
  sl_status_t sc;
  cJSON *param;

  // Check preconditions.
  if (config_value == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sc = aoa_parse_generic_config(&param, config_name, locator_id);
  if (sc == SL_STATUS_OK) {
    if (param->type == cJSON_String) {
      *config_value = param->valuestring;
    } else {
      sc = SL_STATUS_INVALID_CONFIGURATION;
    }
  }
  return sc;
}

/**************************************************************************//**
 * Parse uint16 configuration.
 *****************************************************************************/
sl_status_t aoa_parse_uint16_config(uint16_t *config_value,
                                    char *config_name,
                                    aoa_id_t locator_id)
{
  sl_status_t sc;
  cJSON *param;

  // Check preconditions.
  if (config_value == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sc = aoa_parse_generic_config(&param, config_name, locator_id);

  if (sc == SL_STATUS_OK) {
    if (param->type == cJSON_Number) {
      if ((param->valueint < 0) || (param->valueint > UINT16_MAX)) {
        sc = SL_STATUS_INVALID_RANGE;
      } else {
        *config_value = param->valueint;
      }
    } else {
      sc = SL_STATUS_INVALID_CONFIGURATION;
    }
  }

  return sc;
}

/**************************************************************************//**
 * Parse uint32 config.
 *****************************************************************************/
sl_status_t aoa_parse_uint32_config(uint32_t *config_value,
                                    char *config_name,
                                    aoa_id_t locator_id)
{
  sl_status_t sc;
  cJSON *param;

  // Check preconditions.
  if (config_value == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  sc = aoa_parse_generic_config(&param, config_name, locator_id);

  if (sc == SL_STATUS_OK) {
    if (param->type == cJSON_Number) {
      if ((param->valueint < 0) || (param->valueint > UINT32_MAX)) {
        sc = SL_STATUS_INVALID_RANGE;
      } else {
        *config_value = param->valueint;
      }
    } else {
      sc = SL_STATUS_INVALID_CONFIGURATION;
    }
  }

  return sc;
}

#ifdef RTL_LIB
/**************************************************************************//**
 * Parse next item from the locator config list.
 *****************************************************************************/
sl_status_t aoa_parse_locator(aoa_id_t id,
                              struct sl_rtl_loc_locator_item *loc)
{
  cJSON *array;
  cJSON *item;
  cJSON *param;
  cJSON *subparam;
  uint8_t address[ADR_LEN];
  uint8_t address_type;

  // Check preconditions.
  if (NULL == root) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if ((NULL == id) || (NULL == loc)) {
    return SL_STATUS_NULL_POINTER;
  }

  array = cJSON_GetObjectItem(root, "locators");
  CHECK_TYPE(array, cJSON_Array);
  // Check if locator index is valid.
  if (locator_index >= cJSON_GetArraySize(array)) {
    return SL_STATUS_NOT_FOUND;
  }
  // Get next locator element from the array.
  item = cJSON_GetArrayItem(array, locator_index);
  CHECK_TYPE(item, cJSON_Object);

  // Parse locator ID.
  param = cJSON_GetObjectItem(item, "id");
  CHECK_TYPE(param, cJSON_String);
  aoa_id_copy(id, param->valuestring);

  // Convert the id to address and back. This will take care about the case.
  aoa_id_to_address(id, address, &address_type);
  aoa_address_to_id(address, address_type, id);

  // Parse position.
  param = cJSON_GetObjectItem(item, "coordinate");
  CHECK_TYPE(param, cJSON_Object);
  subparam = cJSON_GetObjectItem(param, "x");
  CHECK_TYPE(subparam, cJSON_Number);
  loc->coordinate_x = (float)subparam->valuedouble;
  subparam = cJSON_GetObjectItem(param, "y");
  CHECK_TYPE(subparam, cJSON_Number);
  loc->coordinate_y = (float)subparam->valuedouble;
  subparam = cJSON_GetObjectItem(param, "z");
  CHECK_TYPE(subparam, cJSON_Number);
  loc->coordinate_z = (float)subparam->valuedouble;

  // Parse orientation.
  param = cJSON_GetObjectItem(item, "orientation");
  CHECK_TYPE(param, cJSON_Object);
  subparam = cJSON_GetObjectItem(param, "x");
  CHECK_TYPE(subparam, cJSON_Number);
  loc->orientation_x_axis_degrees = (float)subparam->valuedouble;
  subparam = cJSON_GetObjectItem(param, "y");
  CHECK_TYPE(subparam, cJSON_Number);
  loc->orientation_y_axis_degrees = (float)subparam->valuedouble;
  subparam = cJSON_GetObjectItem(param, "z");
  CHECK_TYPE(subparam, cJSON_Number);
  loc->orientation_z_axis_degrees = (float)subparam->valuedouble;

  // Increment locator index.
  ++locator_index;
  // Reset array indices.
  allowlist_index = 0;
  azimuth_mask_index = 0;
  elevation_mask_index = 0;

  return SL_STATUS_OK;
}
#endif //RTL_LIB

/**************************************************************************//**
 * Parse azimuth angle mask configuration.
 *****************************************************************************/
sl_status_t aoa_parse_azimuth(float *min, float *max, aoa_id_t locator_id)
{
  cJSON *array;
  cJSON *param;
  cJSON *subparam;
  cJSON *locator;
  sl_status_t sc;

  // Check preconditions.
  if (NULL == root) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if ((NULL == min) || (NULL == max)) {
    return SL_STATUS_NULL_POINTER;
  }

  // Check for locator in positioning config
  sc = aoa_parse_find_locator_config(&locator, locator_id);
  if (sc != SL_STATUS_OK) {
    // Try to parse as single locator config
    locator = root;
  }

  // Try parse locator specific config
  array = cJSON_GetObjectItem(locator, "azimuthMask");
  if (NULL == array) {
    return SL_STATUS_NOT_FOUND;
  }

  // Check if azimuth mask index is valid.
  if (azimuth_mask_index >= cJSON_GetArraySize(array)) {
    return SL_STATUS_NOT_FOUND;
  }

  // Get next azimuth mask from the array.
  param = cJSON_GetArrayItem(array, azimuth_mask_index);

  subparam = cJSON_GetObjectItem(param, "min");
  CHECK_TYPE(subparam, cJSON_Number);
  *min = (float)subparam->valuedouble;
  subparam = cJSON_GetObjectItem(param, "max");
  CHECK_TYPE(subparam, cJSON_Number);
  *max = (float)subparam->valuedouble;

  // Increment index.
  ++azimuth_mask_index;

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Parse elevation angle mask configuration.
 *****************************************************************************/
sl_status_t aoa_parse_elevation(float *min, float *max, aoa_id_t locator_id)
{
  cJSON *array;
  cJSON *param;
  cJSON *subparam;
  cJSON *locator;
  sl_status_t sc;

  // Check preconditions.
  if (NULL == root) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if ((NULL == min) || (NULL == max)) {
    return SL_STATUS_NULL_POINTER;
  }

  // Check for locator in positioning config
  sc = aoa_parse_find_locator_config(&locator, locator_id);
  if (sc != SL_STATUS_OK) {
    // Try to parse as single locator config
    locator = root;
  }

  // Try parse locator specific config
  array = cJSON_GetObjectItem(locator, "elevationMask");
  if (NULL == array) {
    return SL_STATUS_NOT_FOUND;
  }

  // Check if elevation mask index is valid.
  if (elevation_mask_index >= cJSON_GetArraySize(array)) {
    return SL_STATUS_NOT_FOUND;
  }

  // Get next elevation mask from the array.
  param = cJSON_GetArrayItem(array, elevation_mask_index);

  subparam = cJSON_GetObjectItem(param, "min");
  CHECK_TYPE(subparam, cJSON_Number);
  *min = (float)subparam->valuedouble;
  subparam = cJSON_GetObjectItem(param, "max");
  CHECK_TYPE(subparam, cJSON_Number);
  *max = (float)subparam->valuedouble;

  // Increment index.
  ++elevation_mask_index;

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Parse next item from the allowlist.
 *****************************************************************************/
sl_status_t aoa_parse_allowlist(uint8_t address[ADR_LEN],
                                uint8_t *address_type,
                                aoa_id_t locator_id)
{
  cJSON *array;
  cJSON *param;
  cJSON *locator;
  sl_status_t sc;

  // Check preconditions.
  if (NULL == root) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (NULL == address || NULL == address_type) {
    return SL_STATUS_NULL_POINTER;
  }

  // Check for locator in positioning config
  sc = aoa_parse_find_locator_config(&locator, locator_id);
  if (sc != SL_STATUS_OK) {
    // Try to parse as single locator config
    locator = root;
  }

  // Try parse locator specific config
  array = cJSON_GetObjectItem(locator, "allowlist");
  if (NULL == array) {
    return SL_STATUS_NOT_FOUND;
  }

  // Check if allowlist index is valid.
  if (allowlist_index >= cJSON_GetArraySize(array)) {
    return SL_STATUS_NOT_FOUND;
  }
  // Get next allowlist element from the array.
  param = cJSON_GetArrayItem(array, allowlist_index);
  CHECK_TYPE(param, cJSON_String);
  // Convert the id to address. This will take care about the case.
  aoa_id_to_address(param->valuestring, address, address_type);

  // Increment allowlist index.
  ++allowlist_index;

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Parse antenna array.
 *****************************************************************************/
sl_status_t aoa_parse_antenna_array(uint8_t **antenna_array,
                                    uint8_t *array_length,
                                    aoa_id_t locator_id)
{
  cJSON *array;
  cJSON *param;
  cJSON *locator;
  sl_status_t sc;
  int i = 0;
  int array_size = 0;

  // Check preconditions.
  if (NULL == root) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if ((NULL == antenna_array) || (NULL == array_length)) {
    return SL_STATUS_NULL_POINTER;
  }

  // Check for locator in positioning config
  sc = aoa_parse_find_locator_config(&locator, locator_id);
  if (sc != SL_STATUS_OK) {
    // Try to parse as single locator config
    locator = root;
  }

  // Try parse locator specific config
  array = cJSON_GetObjectItem(locator, "antennaArray");
  if (NULL == array) {
    return SL_STATUS_NOT_FOUND;
  }
  CHECK_TYPE(array, cJSON_Array);

  array_size = cJSON_GetArraySize(array);

  *antenna_array = malloc(array_size * sizeof(uint8_t));

  while (i < array_size) {
    param = cJSON_GetArrayItem(array, i);
    (*antenna_array)[i] = param->valueint;
    i++;
  }
  *array_length = (uint8_t)array_size;

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Deinitialise parser module.
 *****************************************************************************/
sl_status_t aoa_parse_deinit(void)
{
  // Check precondition.
  if (NULL == root) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  cJSON_Delete(root);
  root = NULL;

  return SL_STATUS_OK;
}

// -----------------------------------------------------------------------------
// Private function definitions

/**************************************************************************//**
 * Checking for a locator by id
 *****************************************************************************/
static sl_status_t aoa_parse_find_locator_config(cJSON **locator,
                                                 aoa_id_t locator_id)
{
  aoa_id_t id_json;
  cJSON *param;
  cJSON *array;
  cJSON *item;
  uint32_t i = 0;

  if (locator_id == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  array = cJSON_GetObjectItem(root, "locators");
  CHECK_TYPE(array, cJSON_Array);
  //Check if array of locator configs present
  if (NULL == array) {
    return SL_STATUS_NOT_FOUND;
  }

  //Check for the locator
  while (i <= cJSON_GetArraySize(array)) {
    // Get next locator element from the array.
    item = cJSON_GetArrayItem(array, i);
    CHECK_TYPE(item, cJSON_Object);

    // Parse locator ID.
    param = cJSON_GetObjectItem(item, "id");
    CHECK_TYPE(param, cJSON_String);
    aoa_id_copy(id_json, param->valuestring);

    if (aoa_id_compare(id_json, locator_id) == 0) {
      // Locator found, check for config.
      item = cJSON_GetObjectItem(item, "config");
      CHECK_TYPE(item, cJSON_Object);
      // Check if config for the locator present.
      if (NULL == item) {
        return SL_STATUS_NOT_FOUND;
      }

      // Locator config found.
      *locator = item;
      return SL_STATUS_OK;
    }

    ++i;
  }

  return SL_STATUS_NOT_FOUND;
}

/**************************************************************************//**
 * Parse generic configuration.
 *****************************************************************************/
static sl_status_t aoa_parse_generic_config(cJSON **param,
                                            char *config_name,
                                            aoa_id_t locator_id)
{
  cJSON *source = root;

  // Check preconditions.
  if (root == NULL) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (config_name == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (locator_id != NULL) {
    // Locator is optional, therefore the returned status is don't care.
    (void)aoa_parse_find_locator_config(&source, locator_id);
  }

  *param = cJSON_GetObjectItem(source, config_name);
  if (*param == NULL) {
    return SL_STATUS_NOT_FOUND;
  }
  return SL_STATUS_OK;
}
