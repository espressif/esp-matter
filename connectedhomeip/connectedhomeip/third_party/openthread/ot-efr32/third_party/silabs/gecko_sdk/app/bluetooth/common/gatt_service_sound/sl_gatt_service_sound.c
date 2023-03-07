/***************************************************************************//**
 * @file
 * @brief Sound Level GATT Service
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

#include "em_common.h"
#include "sl_status.h"
#include "gatt_db.h"
#include "app_assert.h"
#include "sl_gatt_service_sound.h"
#include "sl_gatt_service_sound_config.h"

// -----------------------------------------------------------------------------
// Private variables

static int16_t sound_level = 0;

// -----------------------------------------------------------------------------
// Private function declarations

static void sound_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void sound_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  float sl;

  sc = sl_gatt_service_sound_get(&sl);
  // keep previous data if measurement fails
  if (SL_STATUS_OK == sc) {
    /* Sound level from sensor represented in dB, characteristic represented in 0.01 dB */
    sound_level = (int16_t)(sl * 100.0f);
  } else if (SL_STATUS_NOT_INITIALIZED == sc) {
    sound_level = SL_GATT_SERVICE_SOUND_INVALID;
  } else {
    return;
  }

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(sound_level),
    (uint8_t*)&sound_level,
    NULL);
  app_assert_status(sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_sound_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  if (sl_bt_evt_gatt_server_user_read_request_id == SL_BT_MSG_ID(evt->header)) {
    if (gattdb_es_sound_level == evt->data.evt_gatt_server_user_read_request.characteristic) {
      sound_read_cb(&evt->data.evt_gatt_server_user_read_request);
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_sound_get(float *sound_level)
{
  (void)sound_level;
  // keep default value
  return SL_STATUS_FAIL;
}
