/***************************************************************************//**
 * @file
 * @brief Bluetooth event handler for Silabs CTE mode.
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
#include "sl_bt_api.h"
#include "sl_ncp_evt_filter_common.h"
#include "aoa_cte.h"
#include "aoa_cte_config.h"

// Module shared variables.
extern uint8_t cte_switch_pattern[ANTENNA_ARRAY_MAX_PIN_PATTERN_SIZE];
extern uint8_t cte_switch_pattern_size;

/**************************************************************************//**
 * CTE specific Bluetooth event handler.
 *****************************************************************************/
sl_status_t cte_bt_on_event_silabs(sl_bt_msg_t *evt)
{
  sl_status_t sc = SL_STATUS_OK;
  uint8_t user_data[SL_NCP_EVT_FILTER_CMD_ADD_LEN];
  uint32_t event;
  aoa_db_entry_t *tag = NULL;
  aoa_iq_report_t iq_report;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Config the NCP on the target.
      // Filter out the scan response event
      user_data[0] = SL_NCP_EVT_FILTER_CMD_ADD_ID;
      event = sl_bt_evt_scanner_scan_report_id;
      memcpy(&user_data[1], &event, SL_NCP_EVT_FILTER_CMD_ADD_LEN - 1);

      sc = sl_bt_user_manage_event_filter(SL_NCP_EVT_FILTER_CMD_ADD_LEN,
                                          user_data);
      if (SL_STATUS_OK != sc) {
        break;
      }

      // Set passive scanning on 1M PHY
      sc = sl_bt_scanner_set_mode(sl_bt_gap_phy_1m, AOA_CTE_SCAN_MODE);
      if (SL_STATUS_OK != sc) {
        break;
      }

      // Set scan interval and scan window
      sc = sl_bt_scanner_set_timing(sl_bt_gap_phy_1m,
                                    AOA_CTE_SCAN_INTERVAL,
                                    AOA_CTE_SCAN_WINDOW);
      if (SL_STATUS_OK != sc) {
        break;
      }

      // Start scanning - looking for tags
      sc = sl_bt_scanner_start(sl_bt_gap_phy_1m, sl_bt_scanner_discover_generic);
      if (SL_STATUS_OK != sc) {
        break;
      }

      // Start Silabs CTE
      sc = sl_bt_cte_receiver_enable_silabs_cte(aoa_cte_config.cte_slot_duration,
                                                aoa_cte_config.cte_count,
                                                cte_switch_pattern_size,
                                                cte_switch_pattern);
      break;

    // -------------------------------
    case sl_bt_evt_cte_receiver_silabs_iq_report_id:
    {
      if (evt->data.evt_cte_receiver_silabs_iq_report.samples.len == 0) {
        // Nothing to be processed.
        break;
      }

      // Check if the tag is allowlisted.
      if (SL_STATUS_NOT_FOUND == aoa_db_allowlist_find(evt->data.evt_cte_receiver_silabs_iq_report.address.addr)) {
        // Tag is not on the allowlist, ignoring. Not an error.
        break;
      }

      // Look for this tag.
      sc = aoa_db_get_tag_by_address(&evt->data.evt_cte_receiver_silabs_iq_report.address, &tag);
      // Check if it is a new tag
      if (sc == SL_STATUS_NOT_FOUND) {
        sc = aoa_db_add_tag(0,
                            &evt->data.evt_cte_receiver_silabs_iq_report.address,
                            evt->data.evt_cte_receiver_silabs_iq_report.address_type,
                            &tag);
        if (SL_STATUS_OK != sc) {
          break;
        }
      }

      // Convert event to common IQ report format.
      iq_report.channel = evt->data.evt_cte_receiver_silabs_iq_report.channel;
      iq_report.rssi = evt->data.evt_cte_receiver_silabs_iq_report.rssi;
      iq_report.event_counter = evt->data.evt_cte_receiver_silabs_iq_report.packet_counter;
      iq_report.length = evt->data.evt_cte_receiver_silabs_iq_report.samples.len;
      iq_report.samples = (int8_t *)evt->data.evt_cte_receiver_silabs_iq_report.samples.data;

      aoa_cte_on_iq_report(tag, &iq_report);
    }
    break;

    // -------------------------------
    // Default event handler.
    default:
      break;
  }

  return sc;
}
