/***************************************************************************//**
 * @file
 * @brief Constant Tone Extension GATT Service (Connectionless)
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdbool.h>
#include "sl_bluetooth.h"
#include "app_assert.h"
#include "sli_gatt_service_cte.h"
#include "sli_gatt_service_cte_adv.h"
#include "sl_gatt_service_cte_connectionless_config.h"

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

// Advertising init status.
static bool advertising_initialized;

// Lower limit of the periodic advertising interval. Value in units of 1.25 ms
const adv_cte_interval_t ADV_CTE_INTERVAL_MIN = 0x06; // 7.5 ms

/**************************************************************************//**
 * Initialize advertisement package according to CTE specifications.
 *****************************************************************************/
void adv_cte_init(void)
{
  sl_status_t sc;

  // Set default values.
  advertising_initialized = false;
  adv_cte_min_len = SL_GATT_SERVICE_CTE_CONNECTIONLESS_MIN_LEN;
  adv_cte_min_tx_count = SL_GATT_SERVICE_CTE_CONNECTIONLESS_MIN_TX_COUNT;
  adv_cte_interval = SL_GATT_SERVICE_CTE_CONNECTIONLESS_ADV_INTERVAL;
  adv_cte_phy = SL_GATT_SERVICE_CTE_CONNECTIONLESS_ADV_CTE_PHY;

  // Create an advertising set.
  sc = sl_bt_advertiser_create_set(&advertising_set_handle);
  app_assert_status(sc);

  sc = sl_bt_extended_advertiser_generate_data(advertising_set_handle,
                                               sl_bt_advertiser_general_discoverable);
  app_assert_status(sc);

  // Set advertising interval to 100ms.
  sc = sl_bt_advertiser_set_timing(
    advertising_set_handle,
    160, // min. adv. interval (milliseconds * 1.6)
    160, // max. adv. interval (milliseconds * 1.6)
    0,   // adv. duration
    0);  // max. num. adv. events
  app_assert_status(sc);

  // Start advertising with CTE.
  sc = adv_cte_start();
  app_assert_status(sc);

  advertising_initialized = true;
}

/**************************************************************************//**
 * Start/restart advertising with the preset parameters.
 *****************************************************************************/
sl_status_t adv_cte_start(void)
{
  sl_status_t sc = SL_STATUS_OK;

  // Stop advertising.
  if (advertising_initialized) {
    sc = sl_bt_periodic_advertiser_stop(advertising_set_handle);

    if (sc == SL_STATUS_OK) {
      sc = sl_bt_advertiser_stop(advertising_set_handle);
    }
  }

  // Set PHY.
  if (sc == SL_STATUS_OK) {
    sc = sl_bt_extended_advertiser_set_phy(advertising_set_handle,
                                           sl_bt_gap_phy_1m,
                                           ADV_CTE_PHY_CONVERT(adv_cte_phy));
  }

  // Start advertising and disable connections.
  if (sc == SL_STATUS_OK) {
    sc = sl_bt_extended_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_non_connectable,
                                         SL_BT_EXTENDED_ADVERTISER_INCLUDE_TX_POWER);
  }

  // Start periodic advertisement with 100 ms interval, include tx power in PDU.
  if (sc == SL_STATUS_OK) {
    sc = sl_bt_periodic_advertiser_start(advertising_set_handle,
                                         adv_cte_interval,
                                         adv_cte_interval,
                                         SL_BT_PERIODIC_ADVERTISER_INCLUDE_TX_POWER);
  }

  // Add CTE to periodic advertisements.
  if (sc == SL_STATUS_OK) {
    sc = sl_bt_cte_transmitter_enable_connectionless_cte(
      advertising_set_handle,
      adv_cte_min_len,
      SLI_CTE_TYPE_AOA,
      adv_cte_min_tx_count,
      SLI_CTE_SWITCHING_PATTERN_LENGTH,
      SLI_CTE_SWITCHING_PATTERN);
  }

  return sc;
}
