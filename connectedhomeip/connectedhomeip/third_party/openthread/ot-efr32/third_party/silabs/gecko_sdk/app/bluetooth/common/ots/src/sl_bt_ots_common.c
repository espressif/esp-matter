/***************************************************************************//**
 * @file
 * @brief Object Transfer Service Object Common source
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
#include "sl_bt_ots_datatypes.h"
#include "sli_bt_ots_datatypes.h"

const uint16_t sl_bt_ots_characteristic_uuids[SL_BT_OTS_CHARACTERISTIC_UUID_COUNT] = {
  SL_BT_OTS_CHARACTERISTIC_UUID_OTS_FEATURE,
  SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_NAME,
  SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_TYPE,
  SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_SIZE,
  SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_FIRST_CREATED,
  SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_LAST_MODIFIED,
  SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_ID,
  SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_PROPERTIES,
  SL_BT_OTS_CHARACTERISTIC_UUID_OACP,
  SL_BT_OTS_CHARACTERISTIC_UUID_OLCP,
  SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_LIST_FILTER,
  SL_BT_OTS_CHARACTERISTIC_UUID_OBJECT_CHANGED,
};
