/***************************************************************************//**
 * @file
 * @brief User Interface core logic for throughput test UI.
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

#include <string.h>
#include <stdio.h>
#include "throughput_ui_log_config.h"
#include "throughput_ui.h"
#include "throughput_ui_types.h"
#include "app_assert.h"
#include "app_log.h"

#if defined(THROUGHPUT_UI_LOG_ENABLE) && THROUGHPUT_UI_LOG_ENABLE
#define UI_PRINTF(...) app_log(__VA_ARGS__)
#if defined(THROUGHPUT_UI_LOG_REFRESH_ALL) && THROUGHPUT_UI_LOG_REFRESH_ALL
#define REFRESH_ONE(x)
#define REFRESH_ALL(x) refresh_ui(x)
#if defined(THROUGHPUT_UI_LOG_BOX_ENABLE) && THROUGHPUT_UI_LOG_BOX_ENABLE

#define BOX_T       "_"
#define BOX_S       "|"

#define UI_PRINTBOX(...) app_log(__VA_ARGS__)
#else // THROUGHPUT_UI_LOG_BOX_ENABLE
#define UI_PRINTBOX(...)
#endif // THROUGHPUT_UI_LOG_BOX_ENABLE
#else // THROUGHPUT_UI_LOG_REFRESH_ALL
#define REFRESH_ONE(x) refresh_ui(x)
#define REFRESH_ALL(x)
#define UI_PRINTBOX(...)
#endif // THROUGHPUT_UI_LOG_REFRESH_ALL
#else // THROUGHPUT_UI_LOG_ENABLE
#define UI_PRINTF(...)
#define UI_PRINTBOX(...)
#define REFRESH_ALL(x)
#define REFRESH_ONE(x)
#endif // THROUGHPUT_UI_LOG_ENABLE

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
/// Internal state
static throughput_t ui_state;

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

// Function to a row on UI
static void refresh_ui(uint8_t refresh_row)
{
  uint8_t row;
  uint8_t col;

  uint8_t row_begin = 0;
  uint8_t row_end = THROUGHPUT_UI_ROWS;

  if (refresh_row != ROW_ALL) {
    row_begin = refresh_row;
    row_end = row_begin + 1;
  }

  for (col = 0; col < THROUGHPUT_UI_COLS + 2; col++) {
    UI_PRINTBOX(BOX_T);
  }
  UI_PRINTBOX(APP_LOG_NEW_LINE);

  for (row = row_begin; row < row_end; row++) {
    UI_PRINTBOX(BOX_S);
    switch (row) {
      case ROW_ROLE:
        if (ui_state.role == THROUGHPUT_ROLE_PERIPHERAL) {
          UI_PRINTF(THROUGHPUT_UI_ROLE_PERIPHERAL_TEXT);
          UI_PRINTBOX("");
        } else {
          UI_PRINTF(THROUGHPUT_UI_ROLE_CENTRAL_TEXT);
          UI_PRINTBOX("   ");
        }
        break;
      case ROW_STATE:
        switch (ui_state.state) {
          case THROUGHPUT_STATE_CONNECTED:
            UI_PRINTF(THROUGHPUT_UI_STATE_CONNECTED_TEXT);
            UI_PRINTBOX("   ");
            break;
          case THROUGHPUT_STATE_SUBSCRIBED:
            UI_PRINTF(THROUGHPUT_UI_STATE_SUBSCRIBED_TEXT);
            UI_PRINTBOX("  ");
            break;
          case THROUGHPUT_STATE_TEST:
            UI_PRINTF(THROUGHPUT_UI_STATE_TEST_TEXT);
            UI_PRINTBOX("     ");
            break;
          default:
            UI_PRINTF(THROUGHPUT_UI_STATE_DISCONNECTED_TEXT);
            UI_PRINTBOX("    ");
            break;
        }
        break;
      case ROW_TX_POWER:
        UI_PRINTF(THROUGHPUT_UI_TX_POWER_FORMAT, ((int)(ui_state.tx_power)));
        UI_PRINTBOX("    ");
        break;
      case ROW_RSSI:
        UI_PRINTF(THROUGHPUT_UI_RSSI_FORMAT, (int)ui_state.rssi);
        UI_PRINTBOX("  ");
        break;
      case ROW_INTERVAL:
        UI_PRINTF(THROUGHPUT_UI_INTERVAL_FORMAT, (int)((float) ui_state.interval * 1.25) );
        UI_PRINTBOX("  ");
        break;
      case ROW_PDU_SIZE:
        UI_PRINTF(THROUGHPUT_UI_PDU_SIZE_FORMAT, (int)ui_state.pdu_size);
        UI_PRINTBOX("        ");
        break;
      case ROW_MTU_SIZE:
        UI_PRINTF(THROUGHPUT_UI_MTU_SIZE_FORMAT, (int)ui_state.mtu_size);
        UI_PRINTBOX("        ");
        break;
      case ROW_DATA_SIZE:
        UI_PRINTF(THROUGHPUT_UI_DATA_SIZE_FORMAT, (int)ui_state.data_size);
        UI_PRINTBOX("       ");
        break;
      case ROW_PHY:
        switch (ui_state.phy) {
          case sl_bt_gap_phy_coding_1m_uncoded:
            UI_PRINTF(THROUGHPUT_UI_PHY_1M_TEXT);
            UI_PRINTBOX("         ");
            break;
          case sl_bt_gap_phy_coding_2m_uncoded:
            UI_PRINTF(THROUGHPUT_UI_PHY_2M_TEXT);
            UI_PRINTBOX("         ");
            break;
          case sl_bt_gap_phy_coding_125k_coded:
            UI_PRINTF(THROUGHPUT_UI_PHY_CODED_125K_TEXT);
            UI_PRINTBOX(" ");
            break;
          case sl_bt_gap_phy_coding_500k_coded:
            UI_PRINTF(THROUGHPUT_UI_PHY_CODED_500K_TEXT);
            UI_PRINTBOX(" ");
            break;
          default:
            UI_PRINTF(THROUGHPUT_UI_PHY_UNKNOWN_TEXT);
            UI_PRINTBOX("          ");
            break;
        }
        break;
      case ROW_NOTIFY:
        switch (ui_state.notifications) {
          case sl_bt_gatt_notification:
            UI_PRINTF(THROUGHPUT_UI_NOTIFY_YES_TEXT);
            UI_PRINTBOX("     ");
            break;
          default:
            UI_PRINTF(THROUGHPUT_UI_NOTIFY_NO_TEXT);
            UI_PRINTBOX("      ");
            break;
        }
        break;
      case ROW_INDICATE:
        switch (ui_state.indications) {
          case sl_bt_gatt_indication:
            UI_PRINTF(THROUGHPUT_UI_INDICATE_YES_TEXT);
            UI_PRINTBOX("   ");
            break;
          default:
            UI_PRINTF(THROUGHPUT_UI_INDICATE_NO_TEXT);
            UI_PRINTBOX("    ");
            break;
        }
        break;
      case ROW_THROUGHPUT:
        UI_PRINTF(THROUGHPUT_UI_TH_FORMAT, (int)ui_state.throughput);
        UI_PRINTBOX(" ");
        break;
      case ROW_COUNT:
        UI_PRINTF(THROUGHPUT_UI_CNT_FORMAT, (int)ui_state.count);
        UI_PRINTBOX("  ");
        break;
      default:
        for (col = 0; col < THROUGHPUT_UI_COLS; col++) {
          UI_PRINTBOX(" ");
        }
        break;
    }
    UI_PRINTBOX(BOX_S);
    UI_PRINTF(APP_LOG_NEW_LINE);
  }
  for (col = 0; col < THROUGHPUT_UI_COLS + 2; col++) {
    UI_PRINTBOX(BOX_T);
  }
  UI_PRINTBOX(APP_LOG_NEW_LINE);
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/**************************************************************************//**
 * Initilize the the UI.
 *****************************************************************************/
void throughput_ui_init(void)
{
}

/**************************************************************************//**
 * Updates the the UI.
 *****************************************************************************/
void throughput_ui_update(void)
{
  REFRESH_ALL(ROW_ALL);
}
/**************************************************************************//**
 * Sets the state value on UI.
 *****************************************************************************/
void throughput_ui_set_state(throughput_state_t state)
{
  ui_state.state = state;
  REFRESH_ONE(ROW_STATE);
}

/**************************************************************************//**
 * Sets the role value on UI.
 *****************************************************************************/
void throughput_ui_set_role(throughput_role_t role)
{
  ui_state.role = role;
  REFRESH_ONE(ROW_ROLE);
}

/**************************************************************************//**
 * Sets the TX Power value on UI.
 *****************************************************************************/
void throughput_ui_set_tx_power(throughput_tx_power_t power)
{
  ui_state.tx_power = power;
  REFRESH_ONE(ROW_TX_POWER);
}

/**************************************************************************//**
 * Sets the RSSI value on UI.
 *****************************************************************************/
void throughput_ui_set_rssi(throughput_rssi_t rssi)
{
  ui_state.rssi = rssi;
  REFRESH_ONE(ROW_RSSI);
}

/**************************************************************************//**
 * Sets the connection interval value on UI.
 *****************************************************************************/
void throughput_ui_set_connection_interval(throughput_time_t interval)
{
  ui_state.interval = interval;
  REFRESH_ONE(ROW_INTERVAL);
}

/**************************************************************************//**
 *   Sets the PDU size value on UI.
 *****************************************************************************/
void throughput_ui_set_pdu_size(throughput_pdu_size_t size)
{
  ui_state.pdu_size = size;
  REFRESH_ONE(ROW_PDU_SIZE);
}

/**************************************************************************//**
 * Sets the MTU size value on UI.
 *****************************************************************************/
void throughput_ui_set_mtu_size(throughput_mtu_size_t size)
{
  ui_state.mtu_size = size;
  REFRESH_ONE(ROW_MTU_SIZE);
}

/**************************************************************************//**
 * Sets the data size value on UI.
 *****************************************************************************/
void throughput_ui_set_data_size(throughput_data_size_t size)
{
  ui_state.data_size = size;
  REFRESH_ONE(ROW_DATA_SIZE);
}

/**************************************************************************//**
 * Sets the PHY on UI.
 *****************************************************************************/
void throughput_ui_set_phy(throughput_phy_t phy)
{
  ui_state.phy = phy;
  REFRESH_ONE(ROW_PHY);
}

/**************************************************************************//**
 * Sets the notification state on UI.
 *****************************************************************************/
void throughput_ui_set_notifications(throughput_notification_t notifications)
{
  ui_state.notifications = notifications;
  REFRESH_ONE(ROW_NOTIFY);
}

/**************************************************************************//**
 * Sets the indication state on UI.
 *****************************************************************************/
void throughput_ui_set_indications(throughput_notification_t indications)
{
  ui_state.indications = indications;
  REFRESH_ONE(ROW_INDICATE);
}

/**************************************************************************//**
 * Sets the throughput value on UI.
 *****************************************************************************/
void throughput_ui_set_throughput(throughput_value_t throughput)
{
  ui_state.throughput = throughput;
  REFRESH_ONE(ROW_THROUGHPUT);
}

/**************************************************************************//**
 * Sets the counter value on UI.
 *****************************************************************************/
void throughput_ui_set_count(throughput_count_t count)
{
  ui_state.count = count;
  REFRESH_ONE(ROW_COUNT);
}
