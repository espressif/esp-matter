/***************************************************************************//**
 * @file
 * @brief Configuration parser.
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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "sl_status.h"
#include "app_log.h"
#include "throughput_central.h"

// Helper macro.
#define CHECK_TYPE(x, t)  if (((x) == NULL) || ((x)->type != (t))) return SL_STATUS_FAIL

// Module internal variables.
static cJSON *root = NULL;
static int allowlist_index;

/**************************************************************************//**
 * Load file into memory.
 *****************************************************************************/
char* cf_parse_load_file(const char *filename)
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
sl_status_t cf_parse_init(char *config)
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

  allowlist_index = 0;

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Parse next item from the allowlist.
 *****************************************************************************/
sl_status_t cf_parse_allowlist(uint8_t *address)
{
  cJSON *array;
  cJSON *param;

  // Check preconditions.
  if (NULL == root) {
    return SL_STATUS_NOT_INITIALIZED;
  }
  if (NULL == address) {
    return SL_STATUS_NULL_POINTER;
  }

  array = cJSON_GetObjectItem(root, "allowlist");
  if (NULL == array) {
    // Allowlist configuration is optional.
    return SL_STATUS_NOT_FOUND;
  }
  // Check if allowlist index is valid.
  if (allowlist_index >= cJSON_GetArraySize(array)) {
    return SL_STATUS_NOT_FOUND;
  }
  // Get next allowlist element from the array.
  param = cJSON_GetArrayItem(array, allowlist_index);
  CHECK_TYPE(param, cJSON_String);

  throughput_central_decode_address(param->valuestring, address);

  // Increment allowlist index.
  ++allowlist_index;

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Deinitialize parser module.
 *****************************************************************************/
sl_status_t cf_parse_deinit(void)
{
  // Check precondition.
  if (NULL == root) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  cJSON_Delete(root);
  root = NULL;

  return SL_STATUS_OK;
}
