/***************************************************************************//**
 * @brief Mesh generic model data serialization/deserialization functions
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

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "sl_btmesh_capi_types.h"
#include "sl_btmesh_generic_model_capi_types.h"
#include "sl_btmesh_serdeser.h"

static int16_t int16_from_buf(const uint8_t *ptr)
{
  return ((int16_t)ptr[0]) | ((int16_t)ptr[1] << 8);
}

static void int16_to_buf(uint8_t *ptr, int16_t n)
{
  ptr[0] = n & 0xff;
  ptr[1] = (n >> 8) & 0xff;
}

static uint16_t uint16_from_buf(const uint8_t *ptr)
{
  return ((uint16_t)ptr[0]) | ((uint16_t)ptr[1] << 8);
}

static void uint16_to_buf(uint8_t *ptr, uint16_t n)
{
  ptr[0] = n & 0xff;
  ptr[1] = (n >> 8) & 0xff;
}

static int32_t int32_from_buf(const uint8_t *ptr)
{
  return
    ((int32_t)ptr[0])
    | ((int32_t)ptr[1] << 8)
    | ((int32_t)ptr[2] << 16)
    | ((int32_t)ptr[3] << 24)
  ;
}

static void int32_to_buf(uint8_t *ptr, int32_t n)
{
  ptr[0] = n & 0xff;
  ptr[1] = (n >> 8) & 0xff;
  ptr[2] = (n >> 16) & 0xff;
  ptr[3] = (n >> 24) & 0xff;
}

sl_status_t mesh_lib_serialize_request(const struct mesh_generic_request *req,
                                       uint8_t *msg_buf,
                                       size_t msg_len,
                                       size_t *msg_used)
{
  size_t msg_off = 0;

  switch (req->kind) {
    case mesh_generic_request_on_off:
      if (msg_len < 1) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      msg_buf[msg_off++] = req->on_off;
      *msg_used = msg_off;
      break;

    case mesh_generic_request_on_power_up:
      if (msg_len < 1) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      msg_buf[msg_off++] = req->on_power_up;
      *msg_used = msg_off;
      break;

    case mesh_generic_request_transition_time:
      if (msg_len < 1) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      msg_buf[msg_off++] = req->transition_time;
      *msg_used = msg_off;
      break;

    case mesh_generic_request_level:
    case mesh_generic_request_level_move:
    case mesh_generic_request_level_halt:
      if (msg_len < 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      int16_to_buf(&msg_buf[msg_off], req->level);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_generic_request_level_delta:
      if (msg_len < 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      int32_to_buf(&msg_buf[msg_off], req->delta);
      msg_off += 4;
      *msg_used = msg_off;
      break;

    case mesh_generic_request_location_global:
      if (msg_len < 10) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      int32_to_buf(&msg_buf[msg_off], req->location_global.lat);
      msg_off += 4;
      int32_to_buf(&msg_buf[msg_off], req->location_global.lon);
      msg_off += 4;
      int16_to_buf(&msg_buf[msg_off], req->location_global.alt);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_generic_request_location_local:
      if (msg_len < 9) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      int16_to_buf(&msg_buf[msg_off], req->location_local.north);
      msg_off += 2;
      int16_to_buf(&msg_buf[msg_off], req->location_local.east);
      msg_off += 2;
      int16_to_buf(&msg_buf[msg_off], req->location_local.alt);
      msg_off += 2;
      msg_buf[msg_off++] = req->location_local.floor;
      uint16_to_buf(&msg_buf[msg_off], req->location_local.uncertainty);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_generic_request_power_level:
    case mesh_generic_request_power_level_default:
      if (msg_len < 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], req->power_level);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_generic_request_power_level_range:
      if (msg_len < 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], req->power_range[0]);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], req->power_range[1]);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_generic_request_property_user:
      if (msg_len < 2 + (size_t)req->property.length) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], req->property.id);
      msg_off += 2;
      memcpy(msg_buf + msg_off,
             req->property.buffer + req->property.offset,
             req->property.length);
      msg_off += req->property.length;
      *msg_used = msg_off;
      break;

    case mesh_generic_request_property_admin:
      if (msg_len < 3 + (size_t)req->property.length) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], req->property.id);
      msg_off += 2;
      msg_buf[msg_off++] = req->property.access;
      memcpy(msg_buf + msg_off,
             req->property.buffer + req->property.offset,
             req->property.length);
      msg_off += req->property.length;
      *msg_used = msg_off;
      break;

    case mesh_generic_request_property_manuf:
      if (msg_len < 3) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], req->property.id);
      msg_off += 2;
      msg_buf[msg_off++] = req->property.access;
      *msg_used = msg_off;
      break;

    case mesh_lighting_request_lightness_actual:
    case mesh_lighting_request_lightness_linear:
    case mesh_lighting_request_lightness_default:
      if (msg_len < 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], req->lightness);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_request_lightness_range:
      if (msg_len < 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], req->lightness_range.min);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], req->lightness_range.max);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_request_ctl:
    case mesh_lighting_request_ctl_default:
      if (msg_len < 6) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], req->ctl.lightness);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], req->ctl.temperature);
      msg_off += 2;
      int16_to_buf(&msg_buf[msg_off], req->ctl.deltauv);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_request_ctl_temperature:
      if (msg_len < 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], req->ctl_temperature.temperature);
      msg_off += 2;
      int16_to_buf(&msg_buf[msg_off], req->ctl_temperature.deltauv);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_request_ctl_temperature_range:
      if (msg_len < 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], req->ctl_temperature_range.min);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], req->ctl_temperature_range.max);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_request_hsl:
    case mesh_lighting_request_hsl_default:
      if (msg_len < 6) {
        return -1;
      }
      uint16_to_buf(&msg_buf[msg_off], req->hsl.lightness);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], req->hsl.hue);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], req->hsl.saturation);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_request_hsl_hue:
      if (msg_len < 2) {
        return -1;
      }
      uint16_to_buf(&msg_buf[msg_off], req->hsl_hue);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_request_hsl_saturation:
      if (msg_len < 2) {
        return -1;
      }
      uint16_to_buf(&msg_buf[msg_off], req->hsl_saturation);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_request_hsl_range:
      if (msg_len < 8) {
        return -1;
      }
      uint16_to_buf(&msg_buf[msg_off], req->hsl_range.hue_min);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], req->hsl_range.hue_max);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], req->hsl_range.saturation_min);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], req->hsl_range.saturation_max);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}

sl_status_t mesh_lib_deserialize_request(struct mesh_generic_request *req,
                                         mesh_generic_request_t kind,
                                         const uint8_t *msg_buf,
                                         size_t msg_len)
{
  size_t msg_off = 0;

  switch (kind) {
    case mesh_generic_request_on_off:
      if (msg_len - msg_off != 1) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->on_off = msg_buf[msg_off];
      break;

    case mesh_generic_request_on_power_up:
      if (msg_len - msg_off != 1) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->on_power_up = msg_buf[msg_off];
      break;

    case mesh_generic_request_transition_time:
      if (msg_len - msg_off != 1) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->transition_time = msg_buf[msg_off];
      break;

    case mesh_generic_request_level:
    case mesh_generic_request_level_move:
    case mesh_generic_request_level_halt:
      if (msg_len - msg_off != 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->level = int16_from_buf(&msg_buf[msg_off]);
      break;

    case mesh_generic_request_level_delta:
      if (msg_len - msg_off != 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->delta = int32_from_buf(&msg_buf[msg_off]);
      break;

    case mesh_generic_request_location_global:
      if (msg_len - msg_off != 10) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->location_global.lat = int32_from_buf(&msg_buf[msg_off]);
      msg_off += 4;
      req->location_global.lon = int32_from_buf(&msg_buf[msg_off]);
      msg_off += 4;
      req->location_global.alt = int16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      break;

    case mesh_generic_request_location_local:
      if (msg_len - msg_off != 9) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->location_local.north = int16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      req->location_local.east = int16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      req->location_local.alt = int16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      req->location_local.floor = msg_buf[msg_off++];
      req->location_local.uncertainty = uint16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      break;

    case mesh_generic_request_power_level:
    case mesh_generic_request_power_level_default:
      if (msg_len - msg_off != 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->power_level = uint16_from_buf(&msg_buf[msg_off]);
      break;

    case mesh_generic_request_power_level_range:
      if (msg_len - msg_off != 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->power_range[0] = uint16_from_buf(&msg_buf[msg_off]);
      req->power_range[1] = uint16_from_buf(&msg_buf[msg_off + 2]);
      break;

    case mesh_generic_request_property_user:
      if (msg_len - msg_off < 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->property.id = uint16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      req->property.buffer = msg_buf;
      req->property.offset = msg_off;
      req->property.length = msg_len - msg_off;
      break;

    case mesh_generic_request_property_admin:
      if (msg_len - msg_off < 3) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->property.id = uint16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      req->property.access = msg_buf[msg_off++];
      req->property.buffer = msg_buf;
      req->property.offset = msg_off;
      req->property.length = msg_len - msg_off;
      break;

    case mesh_generic_request_property_manuf:
      if (msg_len - msg_off != 3) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->property.id = uint16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      req->property.access = msg_buf[msg_off++];
      req->property.buffer = NULL;
      req->property.offset = 0;
      req->property.length = 0;
      break;

    case mesh_lighting_request_lightness_actual:
    case mesh_lighting_request_lightness_linear:
    case mesh_lighting_request_lightness_default:
      if (msg_len - msg_off != 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->lightness = uint16_from_buf(&msg_buf[msg_off]);
      break;

    case mesh_lighting_request_lightness_range:
      if (msg_len - msg_off != 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->lightness_range.min = uint16_from_buf(&msg_buf[msg_off]);
      req->lightness_range.max = uint16_from_buf(&msg_buf[msg_off + 2]);
      break;

    case mesh_lighting_request_ctl:
    case mesh_lighting_request_ctl_default:
      if (msg_len - msg_off != 6) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->ctl.lightness = uint16_from_buf(&msg_buf[msg_off]);
      req->ctl.temperature = uint16_from_buf(&msg_buf[msg_off + 2]);
      req->ctl.deltauv = int16_from_buf(&msg_buf[msg_off + 4]);
      break;

    case mesh_lighting_request_ctl_temperature:
      if (msg_len - msg_off != 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->ctl_temperature.temperature = uint16_from_buf(&msg_buf[msg_off]);
      req->ctl_temperature.deltauv = int16_from_buf(&msg_buf[msg_off + 2]);
      break;

    case mesh_lighting_request_ctl_temperature_range:
      if (msg_len - msg_off != 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      req->kind = kind;
      req->ctl_temperature_range.min = uint16_from_buf(&msg_buf[msg_off]);
      req->ctl_temperature_range.max = uint16_from_buf(&msg_buf[msg_off + 2]);
      break;

    case mesh_lighting_request_hsl:
    case mesh_lighting_request_hsl_default:
      if (msg_len - msg_off != 6) {
        return -1;
      }
      req->kind = kind;
      req->hsl.lightness = uint16_from_buf(&msg_buf[msg_off + 0]);
      req->hsl.hue = uint16_from_buf(&msg_buf[msg_off + 2]);
      req->hsl.saturation = uint16_from_buf(&msg_buf[msg_off + 4]);
      break;

    case mesh_lighting_request_hsl_hue:
      if (msg_len - msg_off != 2) {
        return -1;
      }
      req->kind = kind;
      req->hsl_hue = uint16_from_buf(&msg_buf[msg_off]);
      break;

    case mesh_lighting_request_hsl_saturation:
      if (msg_len - msg_off != 2) {
        return -1;
      }
      req->kind = kind;
      req->hsl_saturation = uint16_from_buf(&msg_buf[msg_off]);
      break;

    case mesh_lighting_request_hsl_range:
      if (msg_len - msg_off != 8) {
        return -1;
      }
      req->kind = kind;
      req->hsl_range.hue_min = uint16_from_buf(&msg_buf[msg_off]);
      req->hsl_range.hue_max = uint16_from_buf(&msg_buf[msg_off + 2]);
      req->hsl_range.saturation_min = uint16_from_buf(&msg_buf[msg_off + 4]);
      req->hsl_range.saturation_max = uint16_from_buf(&msg_buf[msg_off + 6]);
      break;

    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}

sl_status_t mesh_lib_serialize_state(const struct mesh_generic_state *current,
                                     const struct mesh_generic_state *target,
                                     uint8_t *msg_buf,
                                     size_t msg_len,
                                     size_t *msg_used)
{
  size_t msg_off = 0;

  switch (current->kind) {
    case mesh_generic_state_on_off:
      if (msg_len < (target ? 2 : 1)) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      msg_buf[msg_off++] = current->on_off.on;
      if (target) {
        msg_buf[msg_off++] = target->on_off.on;
      }
      *msg_used = msg_off;
      break;

    case mesh_generic_state_on_power_up:
      if (msg_len < 1) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      msg_buf[msg_off++] = current->on_power_up.on_power_up;
      *msg_used = msg_off;
      break;

    case mesh_generic_state_transition_time:
      if (msg_len < 1) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      msg_buf[msg_off++] = current->transition_time.time;
      *msg_used = msg_off;
      break;

    case mesh_generic_state_level:
      if (msg_len < (target ? 4 : 2)) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      int16_to_buf(&msg_buf[msg_off], current->level.level);
      msg_off += 2;
      if (target) {
        int16_to_buf(&msg_buf[msg_off], target->level.level);
        msg_off += 2;
      }
      *msg_used = msg_off;
      break;

    case mesh_generic_state_location_global:
      if (msg_len < 10) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      int32_to_buf(&msg_buf[msg_off], current->location_global.lat);
      msg_off += 4;
      int32_to_buf(&msg_buf[msg_off], current->location_global.lon);
      msg_off += 4;
      int16_to_buf(&msg_buf[msg_off], current->location_global.alt);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_generic_state_location_local:
      if (msg_len < 9) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      int16_to_buf(&msg_buf[msg_off], current->location_local.north);
      msg_off += 2;
      int16_to_buf(&msg_buf[msg_off], current->location_local.east);
      msg_off += 2;
      int16_to_buf(&msg_buf[msg_off], current->location_local.alt);
      msg_off += 2;
      msg_buf[msg_off++] = current->location_local.floor;
      uint16_to_buf(&msg_buf[msg_off], current->location_local.uncertainty);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_generic_state_battery:
      if (msg_len < 8) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      msg_buf[msg_off++] = current->battery.level;
      memcpy(msg_buf + msg_off, current->battery.discharge_time, 3);
      msg_off += 3;
      memcpy(msg_buf + msg_off, current->battery.charge_time, 3);
      msg_off += 3;
      msg_buf[msg_off++] = current->battery.flags;
      *msg_used = msg_off;
      break;

    case mesh_generic_state_power_level:
      if (msg_len < (target ? 4 : 2)) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->power_level.level);
      msg_off += 2;
      if (target) {
        uint16_to_buf(&msg_buf[msg_off], target->power_level.level);
        msg_off += 2;
      }
      *msg_used = msg_off;
      break;

    case mesh_generic_state_power_level_last:
      if (msg_len < 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->power_level_last.level);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_generic_state_power_level_default:
      if (msg_len < 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->power_level_default.level);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_generic_state_power_level_range:
      if (msg_len < 5) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      msg_buf[msg_off++] = current->power_level_range.status;
      uint16_to_buf(&msg_buf[msg_off], current->power_level_range.min);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->power_level_range.max);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_generic_state_property_user:
    case mesh_generic_state_property_admin:
    case mesh_generic_state_property_manuf:
      if (msg_len < 3 + (size_t)current->property.length) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->property.id);
      msg_off += 2;
      msg_buf[msg_off++] = current->property.access;
      memcpy(msg_buf + msg_off,
             current->property.buffer + current->property.offset,
             current->property.length);
      msg_off += current->property.length;
      *msg_used = msg_off;
      break;

    case mesh_generic_state_property_list_user:
    case mesh_generic_state_property_list_admin:
    case mesh_generic_state_property_list_manuf:
    case mesh_generic_state_property_list_client:
      if (msg_len < current->property_list.length) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      memcpy(msg_buf + msg_off,
             current->property_list.buffer + current->property_list.offset,
             current->property_list.length);
      msg_off += current->property_list.length;
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_lightness_actual:
    case mesh_lighting_state_lightness_linear:
      if (msg_len < (target ? 4 : 2)) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->lightness.level);
      msg_off += 2;
      if (target) {
        uint16_to_buf(&msg_buf[msg_off], target->lightness.level);
        msg_off += 2;
      }
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_lightness_last:
    case mesh_lighting_state_lightness_default:
      if (msg_len < 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->lightness.level);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_lightness_range_with_status:
      if (msg_len < 5) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      msg_buf[msg_off++] = current->lightness_range.status;
    //Intentional fallthrough
    case mesh_lighting_state_lightness_range:
      if (msg_len < 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->lightness_range.min);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->lightness_range.max);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_ctl:
      if (msg_len < (target ? 12 : 6)) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->ctl.lightness);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->ctl.temperature);
      msg_off += 2;
      int16_to_buf(&msg_buf[msg_off], current->ctl.deltauv);
      msg_off += 2;
      if (target) {
        uint16_to_buf(&msg_buf[msg_off], target->ctl.lightness);
        msg_off += 2;
        uint16_to_buf(&msg_buf[msg_off], target->ctl.temperature);
        msg_off += 2;
        int16_to_buf(&msg_buf[msg_off], target->ctl.deltauv);
        msg_off += 2;
      }
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_ctl_temperature:
      if (msg_len < (target ? 8 : 4)) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->ctl_temperature.temperature);
      msg_off += 2;
      int16_to_buf(&msg_buf[msg_off], current->ctl_temperature.deltauv);
      msg_off += 2;
      if (target) {
        uint16_to_buf(&msg_buf[msg_off], target->ctl_temperature.temperature);
        msg_off += 2;
        int16_to_buf(&msg_buf[msg_off], target->ctl_temperature.deltauv);
        msg_off += 2;
      }
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_ctl_lightness_temperature:
      if (msg_len < (target ? 8 : 4)) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->ctl_lightness_temperature.lightness);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->ctl_lightness_temperature.temperature);
      msg_off += 2;
      if (target) {
        uint16_to_buf(&msg_buf[msg_off], target->ctl_lightness_temperature.lightness);
        msg_off += 2;
        uint16_to_buf(&msg_buf[msg_off], target->ctl_lightness_temperature.temperature);
        msg_off += 2;
      }
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_ctl_default:
      if (msg_len < 6) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->ctl.lightness);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->ctl.temperature);
      msg_off += 2;
      int16_to_buf(&msg_buf[msg_off], current->ctl.deltauv);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_ctl_temperature_range_with_status:
      if (msg_len < 5) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      msg_buf[msg_off++] = current->ctl_temperature_range.status;
    //Intentional fallthrough
    case mesh_lighting_state_ctl_temperature_range:
      if (msg_len < 4) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      uint16_to_buf(&msg_buf[msg_off], current->ctl_temperature_range.min);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->ctl_temperature_range.max);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_hsl:
      if (msg_len < (target ? 12 : 6)) {
        return -1;
      }
      uint16_to_buf(&msg_buf[msg_off], current->hsl.lightness);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->hsl.hue);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->hsl.saturation);
      msg_off += 2;
      if (target) {
        uint16_to_buf(&msg_buf[msg_off], target->hsl.lightness);
        msg_off += 2;
        uint16_to_buf(&msg_buf[msg_off], target->hsl.hue);
        msg_off += 2;
        uint16_to_buf(&msg_buf[msg_off], target->hsl.saturation);
        msg_off += 2;
      }
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_hsl_target:
    case mesh_lighting_state_hsl_default:
      if (msg_len < 6) {
        return -1;
      }
      uint16_to_buf(&msg_buf[msg_off], current->hsl.lightness);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->hsl.hue);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->hsl.saturation);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_hsl_hue:
      if (msg_len < (target ? 4 : 2)) {
        return -1;
      }
      uint16_to_buf(&msg_buf[msg_off], current->hsl_hue.hue);
      msg_off += 2;
      if (target) {
        uint16_to_buf(&msg_buf[msg_off], target->hsl_hue.hue);
        msg_off += 2;
      }
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_hsl_saturation:
      if (msg_len < (target ? 4 : 2)) {
        return -1;
      }
      uint16_to_buf(&msg_buf[msg_off], current->hsl_saturation.saturation);
      msg_off += 2;
      if (target) {
        uint16_to_buf(&msg_buf[msg_off], target->hsl_saturation.saturation);
        msg_off += 2;
      }
      *msg_used = msg_off;
      break;

    case mesh_lighting_state_hsl_range_with_status:
      if (msg_len < 9) {
        return -1;
      }
      msg_buf[msg_off++] = current->hsl_range.status;
    //Intentional fallthrough
    case mesh_lighting_state_hsl_range:
      if (msg_len < 8) {
        return -1;
      }
      uint16_to_buf(&msg_buf[msg_off], current->hsl_range.hue_min);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->hsl_range.hue_max);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->hsl_range.saturation_min);
      msg_off += 2;
      uint16_to_buf(&msg_buf[msg_off], current->hsl_range.saturation_max);
      msg_off += 2;
      *msg_used = msg_off;
      break;

    case mesh_generic_state_last:
    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}

sl_status_t mesh_lib_deserialize_state(struct mesh_generic_state *current,
                                       struct mesh_generic_state *target,
                                       int *has_target,
                                       mesh_generic_state_t kind,
                                       const uint8_t *msg_buf,
                                       size_t msg_len)
{
  size_t msg_off = 0;

  switch (kind) {
    case mesh_generic_state_on_off:
      if (msg_len - msg_off == 1) {
        current->kind = kind;
        current->on_off.on = msg_buf[msg_off++];
        *has_target = 0;
      } else if (msg_len - msg_off == 2) {
        current->kind = kind;
        current->on_off.on = msg_buf[msg_off++];
        target->kind = kind;
        target->on_off.on = msg_buf[msg_off++];
        *has_target = 1;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_generic_state_on_power_up:
      if (msg_len - msg_off == 1) {
        current->kind = kind;
        current->on_power_up.on_power_up = msg_buf[msg_off++];
        *has_target = 0;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_generic_state_transition_time:
      if (msg_len - msg_off == 1) {
        current->kind = kind;
        current->transition_time.time = msg_buf[msg_off++];
        *has_target = 0;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_generic_state_level:
      if (msg_len - msg_off == 2) {
        current->kind = kind;
        current->level.level = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else if (msg_len - msg_off == 4) {
        current->kind = kind;
        current->level.level = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->kind = kind;
        target->level.level = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 1;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_generic_state_location_global:
      if (msg_len - msg_off != 10) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      current->kind = kind;
      current->location_global.lat = int32_from_buf(&msg_buf[msg_off]);
      msg_off += 4;
      current->location_global.lon = int32_from_buf(&msg_buf[msg_off]);
      msg_off += 4;
      current->location_global.alt = int16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      *has_target = 0;
      break;

    case mesh_generic_state_location_local:
      if (msg_len - msg_off != 9) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      current->kind = kind;
      current->location_local.north = int16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      current->location_local.east = int16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      current->location_local.alt = int16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      current->location_local.floor = msg_buf[msg_off++];
      current->location_local.uncertainty = uint16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      *has_target = 0;
      break;

    case mesh_generic_state_battery:
      if (msg_len - msg_off != 8) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      current->kind = kind;
      current->battery.level = msg_buf[msg_off++];
      memcpy(current->battery.discharge_time, msg_buf + msg_off, 3);
      msg_off += 3;
      memcpy(current->battery.charge_time, msg_buf + msg_off, 3);
      msg_off += 3;
      current->battery.flags = msg_buf[msg_off++];
      *has_target = 0;
      break;

    case mesh_generic_state_power_level:
      if (msg_len - msg_off == 2) {
        current->kind = kind;
        current->power_level.level = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else if (msg_len - msg_off == 4) {
        current->kind = kind;
        current->power_level.level = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->kind = kind;
        target->power_level.level = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 1;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_generic_state_power_level_last:
      if (msg_len - msg_off != 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      current->kind = kind;
      current->power_level_last.level = uint16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      *has_target = 0;
      break;

    case mesh_generic_state_power_level_default:
      if (msg_len - msg_off != 2) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      current->kind = kind;
      current->power_level_default.level = uint16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      *has_target = 0;
      break;

    case mesh_generic_state_power_level_range:
      if (msg_len - msg_off != 5) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      current->kind = kind;
      current->power_level_range.status = msg_buf[msg_off++];
      current->power_level_range.min = uint16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      current->power_level_range.max = uint16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      *has_target = 0;
      break;

    case mesh_generic_state_property_user:
    case mesh_generic_state_property_admin:
    case mesh_generic_state_property_manuf:
      if (msg_len - msg_off < 3) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      current->kind = kind;
      current->property.id = uint16_from_buf(&msg_buf[msg_off]);
      msg_off += 2;
      current->property.access = msg_buf[msg_off++];
      current->property.buffer = msg_buf;
      current->property.offset = msg_off;
      current->property.length = msg_len - msg_off;
      *has_target = 0;
      break;

    case mesh_generic_state_property_list_user:
    case mesh_generic_state_property_list_admin:
    case mesh_generic_state_property_list_manuf:
    case mesh_generic_state_property_list_client:
      if ((msg_len - msg_off) & 0x01) {
        return SL_STATUS_INVALID_PARAMETER;
      }
      current->kind = kind;
      current->property_list.buffer = msg_buf;
      current->property_list.offset = msg_off;
      current->property_list.length = msg_len - msg_off;
      *has_target = 0;
      break;

    case mesh_lighting_state_lightness_actual:
    case mesh_lighting_state_lightness_linear:
      if (msg_len - msg_off == 2) {
        current->kind = kind;
        current->lightness.level = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else if (msg_len - msg_off == 4) {
        current->kind = kind;
        current->lightness.level = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->kind = kind;
        target->lightness.level = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 1;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_lighting_state_lightness_last:
    case mesh_lighting_state_lightness_default:
      if (msg_len - msg_off == 2) {
        current->kind = kind;
        current->lightness.level = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_lighting_state_lightness_range_with_status:
      if (msg_len - msg_off == 5) {
        current->lightness_range.status = msg_buf[msg_off++];
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
    //Intentional fallthrough
    case mesh_lighting_state_lightness_range:
      if (msg_len - msg_off == 4) {
        current->kind = kind;
        current->lightness_range.min = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->lightness_range.max = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_lighting_state_ctl:
      if (msg_len - msg_off == 6) {
        current->kind = kind;
        current->ctl.lightness = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->ctl.temperature = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->ctl.deltauv = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else if (msg_len - msg_off == 12) {
        current->kind = kind;
        current->ctl.lightness = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->ctl.temperature = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->ctl.deltauv = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->kind = kind;
        target->ctl.lightness = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->ctl.temperature = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->ctl.deltauv = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 1;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_lighting_state_ctl_temperature:
      if (msg_len - msg_off == 4) {
        current->kind = kind;
        current->ctl_temperature.temperature = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->ctl_temperature.deltauv = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else if (msg_len - msg_off == 8) {
        current->kind = kind;
        current->ctl_temperature.temperature = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->ctl_temperature.deltauv = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->kind = kind;
        target->ctl_temperature.temperature = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->ctl_temperature.deltauv = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 1;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_lighting_state_ctl_lightness_temperature:
      if (msg_len - msg_off == 4) {
        current->kind = kind;
        current->ctl_lightness_temperature.lightness = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->ctl_lightness_temperature.temperature = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else if (msg_len - msg_off == 8) {
        current->kind = kind;
        current->ctl_lightness_temperature.lightness = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->ctl_lightness_temperature.temperature = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->kind = kind;
        target->ctl_lightness_temperature.lightness = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->ctl_lightness_temperature.temperature = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 1;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_lighting_state_ctl_default:
      if (msg_len - msg_off == 6) {
        current->kind = kind;
        current->ctl.lightness = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->ctl.temperature = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->ctl.deltauv = int16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_lighting_state_ctl_temperature_range_with_status:
      if (msg_len - msg_off == 5) {
        current->ctl_temperature_range.status = msg_buf[msg_off++];
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
    // Intentional fallthrough
    case mesh_lighting_state_ctl_temperature_range:
      if (msg_len - msg_off == 4) {
        current->kind = kind;
        current->ctl_temperature_range.min = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->ctl_temperature_range.max = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else {
        return SL_STATUS_INVALID_PARAMETER;
      }
      break;

    case mesh_lighting_state_hsl:
      if (msg_len - msg_off == 6) {
        current->kind = kind;
        current->hsl.lightness = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->hsl.hue = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->hsl.saturation = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else if (msg_len - msg_off == 12) {
        current->kind = kind;
        current->hsl.lightness = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->hsl.hue = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->hsl.saturation = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->hsl.lightness = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->hsl.hue = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->hsl.saturation = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 1;
      } else {
        return -1;
      }
      break;

    case mesh_lighting_state_hsl_target:
    case mesh_lighting_state_hsl_default:
      if (msg_len - msg_off == 6) {
        current->kind = kind;
        current->hsl.lightness = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->hsl.hue = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->hsl.saturation = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else {
        return -1;
      }
      break;

    case mesh_lighting_state_hsl_hue:
      if (msg_len - msg_off == 2) {
        current->kind = kind;
        current->hsl_hue.hue = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else if (msg_len - msg_off == 4) {
        current->kind = kind;
        current->hsl_hue.hue = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->hsl_hue.hue = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 1;
      } else {
        return -1;
      }
      break;

    case mesh_lighting_state_hsl_saturation:
      if (msg_len - msg_off == 2) {
        current->kind = kind;
        current->hsl_saturation.saturation = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else if (msg_len - msg_off == 4) {
        current->kind = kind;
        current->hsl_saturation.saturation = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        target->hsl_saturation.saturation = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 1;
      } else {
        return -1;
      }
      break;

    case mesh_lighting_state_hsl_range_with_status:
      if (msg_len - msg_off == 9) {
        current->hsl_range.status = msg_buf[msg_off++];
      } else {
        return -1;
      }
    // Intentional fallthrough
    case mesh_lighting_state_hsl_range:
      if (msg_len - msg_off == 8) {
        current->kind = kind;
        current->hsl_range.hue_min = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->hsl_range.hue_max = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->hsl_range.saturation_min = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        current->hsl_range.saturation_max = uint16_from_buf(&msg_buf[msg_off]);
        msg_off += 2;
        *has_target = 0;
      } else {
        return -1;
      }
      break;

    case mesh_generic_state_last:
    default:
      return SL_STATUS_INVALID_PARAMETER;
  }

  return SL_STATUS_OK;
}
