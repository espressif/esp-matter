/***************************************************************************//**
 * @file
 * @brief AoA serializer and deserializer functions.
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
#include "cJSON.h"
#include "aoa_serdes.h"

// Helper macro.
#define CHECK_TYPE(x, t)  if (((x) == NULL) || ((x)->type != (t))) return SL_STATUS_FAIL

/***************************************************************************//**
 * Serialize IQ report data structure into string.
 ******************************************************************************/
sl_status_t aoa_serialize_iq_report(aoa_iq_report_t *iq_report, char** str)
{
  if ((iq_report == NULL) || (str == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  cJSON *root = cJSON_CreateObject();
  cJSON *samples = cJSON_CreateArray();
  cJSON_AddIntegerToObject(root, "channel", (int)iq_report->channel);
  cJSON_AddIntegerToObject(root, "rssi", (int)iq_report->rssi);
  cJSON_AddIntegerToObject(root, "sequence", (int)iq_report->event_counter);
  for (int i = 0; i < iq_report->length; i++) {
    cJSON_AddItemToArray(samples, cJSON_CreateInteger(iq_report->samples[i]));
  }
  cJSON_AddItemToObject(root, "samples", samples);
  *str = cJSON_Print(root);
  cJSON_Delete(root);
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Deserialize IQ report data structure from string.
 ******************************************************************************/
sl_status_t aoa_deserialize_iq_report(char* str, aoa_iq_report_t *iq_report)
{
  if ((iq_report == NULL) || (str == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  cJSON *root = cJSON_Parse(str);
  CHECK_TYPE(root, cJSON_Object);
  cJSON *samples = cJSON_GetObjectItem(root, "samples");
  CHECK_TYPE(samples, cJSON_Array);
  cJSON *param = NULL;
  uint8_t length = 0;
  param = cJSON_GetObjectItem(root, "channel");
  CHECK_TYPE(param, cJSON_Number);
  iq_report->channel = (uint8_t)param->valueint;
  param = cJSON_GetObjectItem(root, "rssi");
  CHECK_TYPE(param, cJSON_Number);
  iq_report->rssi = (int8_t)param->valueint;
  param = cJSON_GetObjectItem(root, "sequence");
  CHECK_TYPE(param, cJSON_Number);
  iq_report->event_counter = (uint16_t)param->valueint;
  cJSON_ArrayForEach(param, samples) {
    CHECK_TYPE(param, cJSON_Number);
    iq_report->samples[length] = (int8_t)param->valueint;
    ++length;
  }
  iq_report->length = length;
  cJSON_Delete(root);
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Serialize angle data structure into string.
 ******************************************************************************/
sl_status_t aoa_serialize_angle(aoa_angle_t *angle, char** str)
{
  if ((angle == NULL) || (str == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  cJSON *root = cJSON_CreateObject();
  cJSON_AddDoubleToObject(root, "azimuth", (double)angle->azimuth);
  cJSON_AddDoubleToObject(root, "azimuth_stdev", (double)angle->azimuth_stdev);
  cJSON_AddDoubleToObject(root, "elevation", (double)angle->elevation);
  cJSON_AddDoubleToObject(root, "elevation_stdev", (double)angle->elevation_stdev);
  cJSON_AddDoubleToObject(root, "distance", (double)angle->distance);
  cJSON_AddDoubleToObject(root, "distance_stdev", (double)angle->distance_stdev);
  cJSON_AddIntegerToObject(root, "sequence", (int)angle->sequence);
  *str = cJSON_Print(root);
  cJSON_Delete(root);
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Deserialize angle data structure from string.
 ******************************************************************************/
sl_status_t aoa_deserialize_angle(char* str, aoa_angle_t *angle)
{
  if ((angle == NULL) || (str == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  cJSON *root = cJSON_Parse(str);
  CHECK_TYPE(root, cJSON_Object);
  cJSON *param = cJSON_GetObjectItem(root, "azimuth");
  CHECK_TYPE(param, cJSON_Number);
  angle->azimuth = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "azimuth_stdev");
  CHECK_TYPE(param, cJSON_Number);
  angle->azimuth_stdev = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "elevation");
  CHECK_TYPE(param, cJSON_Number);
  angle->elevation = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "elevation_stdev");
  CHECK_TYPE(param, cJSON_Number);
  angle->elevation_stdev = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "distance");
  CHECK_TYPE(param, cJSON_Number);
  angle->distance = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "distance_stdev");
  CHECK_TYPE(param, cJSON_Number);
  angle->distance_stdev = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "sequence");
  CHECK_TYPE(param, cJSON_Number);
  angle->sequence = (int32_t)param->valueint;
  cJSON_Delete(root);
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Serialize position data structure into string.
 ******************************************************************************/
sl_status_t aoa_serialize_position(aoa_position_t *position, char** str)
{
  if ((position == NULL) || (str == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  cJSON *root = cJSON_CreateObject();
  cJSON_AddDoubleToObject(root, "x", position->x);
  cJSON_AddDoubleToObject(root, "x_stdev", position->x_stdev);
  cJSON_AddDoubleToObject(root, "y", position->y);
  cJSON_AddDoubleToObject(root, "y_stdev", position->y_stdev);
  cJSON_AddDoubleToObject(root, "z", position->z);
  cJSON_AddDoubleToObject(root, "z_stdev", position->z_stdev);
  cJSON_AddIntegerToObject(root, "sequence", (int)position->sequence);
  *str = cJSON_Print(root);
  cJSON_Delete(root);
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Deserialize position data structure from string.
 ******************************************************************************/
sl_status_t aoa_deserialize_position(char* str, aoa_position_t *position)
{
  if ((position == NULL) || (str == NULL)) {
    return SL_STATUS_NULL_POINTER;
  }
  cJSON *root = cJSON_Parse(str);
  CHECK_TYPE(root, cJSON_Object);
  cJSON *param = cJSON_GetObjectItem(root, "x");
  CHECK_TYPE(param, cJSON_Number);
  position->x = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "x_stdev");
  CHECK_TYPE(param, cJSON_Number);
  position->x_stdev = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "y");
  CHECK_TYPE(param, cJSON_Number);
  position->y = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "y_stdev");
  CHECK_TYPE(param, cJSON_Number);
  position->y_stdev = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "z");
  CHECK_TYPE(param, cJSON_Number);
  position->z = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "z_stdev");
  CHECK_TYPE(param, cJSON_Number);
  position->z_stdev = (float)param->valuedouble;
  param = cJSON_GetObjectItem(root, "sequence");
  CHECK_TYPE(param, cJSON_Number);
  position->sequence = (int32_t)param->valueint;
  cJSON_Delete(root);
  return SL_STATUS_OK;
}
