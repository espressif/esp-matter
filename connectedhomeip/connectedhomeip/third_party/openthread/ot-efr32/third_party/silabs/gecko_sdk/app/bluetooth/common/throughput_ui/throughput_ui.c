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

#include "em_types.h"
#include "glib.h"
#include "dmd/dmd.h"
#include <string.h>
#include <stdio.h>
#include "throughput_ui.h"
#include "throughput_ui_types.h"
#include "throughput_ui_config.h"
#include "app_assert.h"
#include "app_log.h"

/*******************************************************************************
 ******************************* DEFINITIONS   *********************************
 ******************************************************************************/
#if defined(THROUGHPUT_UI_LOG_ENABLE) && THROUGHPUT_UI_LOG_ENABLE
#define LOG(...) app_log_info(__VA_ARGS__)
#define LOGLN() app_log_nl()
#else // THROUGHPUT_UI_LOG_ENABLE
#define LOG(...)
#define LOGLN()
#endif // THROUGHPUT_UI_LOG_ENABLE
#define COL_OFFSET                  2
#define ROW_OFFSET                  2
#define ROW_SPACING                 9
#define FONT_TYPE                   ((GLIB_Font_t *)&GLIB_FontNormal8x8)
#define BUFFER_SIZE                 17

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
/// GLIB Context
static GLIB_Context_t glibContext;

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

// Clear single row
static void clear_row(uint8_t row)
{
  GLIB_Rectangle_t rect;
  rect.xMin = 0;
  rect.yMin = ROW_OFFSET + row * ROW_SPACING;
  rect.xMax = glibContext.pDisplayGeometry->xSize - 1;
  rect.yMax = rect.yMin + ROW_SPACING - 1;
  GLIB_setClippingRegion(&glibContext, (const GLIB_Rectangle_t*)&rect);
  GLIB_clearRegion(&glibContext);
  GLIB_resetClippingRegion(&glibContext);
  GLIB_applyClippingRegion(&glibContext);
}

// Write single row
static void write_row(char * str, uint8_t row)
{
  uint16_t y;
  if (strlen(str)) {
    y = ROW_OFFSET + row * ROW_SPACING;
    GLIB_drawString(&glibContext,
                    str,
                    strlen(str) + 1,
                    COL_OFFSET,
                    y,
                    0);
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/**************************************************************************//**
 * Initilize the the UI.
 *****************************************************************************/
void throughput_ui_init(void)
{
  EMSTATUS status;

  // Initialize the DMD module for the DISPLAY device driver.
  status = DMD_init(0);
  app_assert(DMD_OK == status, "Failed to init display driver\n");

  status = GLIB_contextInit(&glibContext);
  app_assert(DMD_OK == status, "Failed to init display context\n");

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  // Set font
  GLIB_setFont(&glibContext, FONT_TYPE);
  GLIB_clear(&glibContext);
}

/**************************************************************************//**
 * Updates the the UI.
 *****************************************************************************/
void throughput_ui_update(void)
{
  DMD_updateDisplay();
}

/**************************************************************************//**
 * Sets the state value on UI.
 *****************************************************************************/
void throughput_ui_set_state(throughput_state_t state)
{
  clear_row(ROW_STATE);
  switch (state) {
    case THROUGHPUT_STATE_CONNECTED:
      write_row(THROUGHPUT_UI_STATE_CONNECTED_TEXT, ROW_STATE);
      LOG(THROUGHPUT_UI_STATE_CONNECTED_TEXT);
      break;
    case THROUGHPUT_STATE_SUBSCRIBED:
      write_row(THROUGHPUT_UI_STATE_SUBSCRIBED_TEXT, ROW_STATE);
      LOG(THROUGHPUT_UI_STATE_SUBSCRIBED_TEXT);
      break;
    case THROUGHPUT_STATE_TEST:
      write_row(THROUGHPUT_UI_STATE_TEST_TEXT, ROW_STATE);
      LOG(THROUGHPUT_UI_STATE_TEST_TEXT);
      break;
    default:
      write_row(THROUGHPUT_UI_STATE_DISCONNECTED_TEXT, ROW_STATE);
      LOG(THROUGHPUT_UI_STATE_DISCONNECTED_TEXT);
      break;
  }
  LOGLN();
}

/**************************************************************************//**
 * Sets the role value on UI.
 *****************************************************************************/
void throughput_ui_set_role(throughput_role_t role)
{
  clear_row(ROW_ROLE);
  if (role == THROUGHPUT_ROLE_PERIPHERAL) {
    write_row(THROUGHPUT_UI_ROLE_PERIPHERAL_TEXT, ROW_ROLE);
    LOG(THROUGHPUT_UI_ROLE_PERIPHERAL_TEXT);
  } else {
    write_row(THROUGHPUT_UI_ROLE_CENTRAL_TEXT, ROW_ROLE);
    LOG(THROUGHPUT_UI_ROLE_CENTRAL_TEXT);
  }
  LOGLN();
}

/**************************************************************************//**
 * Sets the TX Power value on UI.
 *****************************************************************************/
void throughput_ui_set_tx_power(throughput_tx_power_t power)
{
  char buffer[BUFFER_SIZE];
  snprintf(buffer, BUFFER_SIZE, THROUGHPUT_UI_TX_POWER_FORMAT, ((int)power));
  clear_row(ROW_TX_POWER);
  write_row(buffer, ROW_TX_POWER);
  LOG(buffer);
  LOGLN();
}

/**************************************************************************//**
 * Sets the RSSI value on UI.
 *****************************************************************************/
void throughput_ui_set_rssi(throughput_rssi_t rssi)
{
  char buffer[BUFFER_SIZE];
  snprintf(buffer, BUFFER_SIZE, THROUGHPUT_UI_RSSI_FORMAT, (int)rssi);
  clear_row(ROW_RSSI);
  write_row(buffer, ROW_RSSI);
  #if defined(THROUGHPUT_UI_PRINT_RSSI) && THROUGHPUT_UI_PRINT_RSSI == 1
  LOG(buffer);
  LOGLN();
  #endif // THROUGHPUT_UI_PRINT_RSSI
}

/**************************************************************************//**
 * Sets the connection interval value on UI.
 *****************************************************************************/
void throughput_ui_set_connection_interval(throughput_time_t interval)
{
  char buffer[BUFFER_SIZE];
  snprintf(buffer, BUFFER_SIZE, THROUGHPUT_UI_INTERVAL_FORMAT, (int)((float) interval * 1.25) );
  clear_row(ROW_INTERVAL);
  write_row(buffer, ROW_INTERVAL);
  LOG(buffer);
  LOGLN();
}

/**************************************************************************//**
 *   Sets the PDU size value on UI.
 *****************************************************************************/
void throughput_ui_set_pdu_size(throughput_pdu_size_t size)
{
  char buffer[BUFFER_SIZE];
  snprintf(buffer, BUFFER_SIZE, THROUGHPUT_UI_PDU_SIZE_FORMAT, (int)size);
  clear_row(ROW_PDU_SIZE);
  write_row(buffer, ROW_PDU_SIZE);
  LOG(buffer);
  LOGLN();
}

/**************************************************************************//**
 * Sets the MTU size value on UI.
 *****************************************************************************/
void throughput_ui_set_mtu_size(throughput_mtu_size_t size)
{
  char buffer[BUFFER_SIZE];
  snprintf(buffer, BUFFER_SIZE, THROUGHPUT_UI_MTU_SIZE_FORMAT, (int)size);
  clear_row(ROW_MTU_SIZE);
  write_row(buffer, ROW_MTU_SIZE);
  LOG(buffer);
  LOGLN();
}

/**************************************************************************//**
 * Sets the data size value on UI.
 *****************************************************************************/
void throughput_ui_set_data_size(throughput_data_size_t size)
{
  char buffer[BUFFER_SIZE];
  snprintf(buffer, BUFFER_SIZE, THROUGHPUT_UI_DATA_SIZE_FORMAT, (int)size);
  clear_row(ROW_DATA_SIZE);
  write_row(buffer, ROW_DATA_SIZE);
  LOG(buffer);
  LOGLN();
}

/**************************************************************************//**
 * Sets the PHY on UI.
 *****************************************************************************/
void throughput_ui_set_phy(throughput_phy_t phy)
{
  clear_row(ROW_PHY);
  switch (phy) {
    case sl_bt_gap_phy_coding_1m_uncoded:
      write_row(THROUGHPUT_UI_PHY_1M_TEXT, ROW_PHY);
      LOG(THROUGHPUT_UI_PHY_1M_TEXT);
      break;
    case sl_bt_gap_phy_coding_2m_uncoded:
      write_row(THROUGHPUT_UI_PHY_2M_TEXT, ROW_PHY);
      LOG(THROUGHPUT_UI_PHY_2M_TEXT);
      break;
    case sl_bt_gap_phy_coding_125k_coded:
      write_row(THROUGHPUT_UI_PHY_CODED_125K_TEXT, ROW_PHY);
      LOG(THROUGHPUT_UI_PHY_CODED_125K_TEXT);
      break;
    case sl_bt_gap_phy_coding_500k_coded:
      write_row(THROUGHPUT_UI_PHY_CODED_500K_TEXT, ROW_PHY);
      LOG(THROUGHPUT_UI_PHY_CODED_500K_TEXT);
      break;
    default:
      write_row(THROUGHPUT_UI_PHY_UNKNOWN_TEXT, ROW_PHY);
      LOG(THROUGHPUT_UI_PHY_UNKNOWN_TEXT);
      break;
  }
  LOGLN();
}

/**************************************************************************//**
 * Sets the notification state on UI.
 *****************************************************************************/
void throughput_ui_set_notifications(throughput_notification_t notifications)
{
  clear_row(ROW_NOTIFY);
  switch (notifications) {
    case sl_bt_gatt_notification:
      write_row(THROUGHPUT_UI_NOTIFY_YES_TEXT, ROW_NOTIFY);
      LOG(THROUGHPUT_UI_NOTIFY_YES_TEXT);
      break;
    default:
      write_row(THROUGHPUT_UI_NOTIFY_NO_TEXT, ROW_NOTIFY);
      LOG(THROUGHPUT_UI_NOTIFY_NO_TEXT);
      break;
  }
  LOGLN();
}

/**************************************************************************//**
 * Sets the indication state on UI.
 *****************************************************************************/
void throughput_ui_set_indications(throughput_notification_t indications)
{
  clear_row(ROW_INDICATE);
  switch (indications) {
    case sl_bt_gatt_indication:
      write_row(THROUGHPUT_UI_INDICATE_YES_TEXT, ROW_INDICATE);
      LOG(THROUGHPUT_UI_INDICATE_YES_TEXT);
      break;
    default:
      write_row(THROUGHPUT_UI_INDICATE_NO_TEXT, ROW_INDICATE);
      LOG(THROUGHPUT_UI_INDICATE_NO_TEXT);
      break;
  }
  LOGLN();
}

/**************************************************************************//**
 * Sets the throughput value on UI.
 *****************************************************************************/
void throughput_ui_set_throughput(throughput_value_t throughput)
{
  char buffer[BUFFER_SIZE];
  snprintf(buffer, BUFFER_SIZE, THROUGHPUT_UI_TH_FORMAT, (int)throughput);
  clear_row(ROW_THROUGHPUT);
  write_row(buffer, ROW_THROUGHPUT);
  LOG(buffer);
  LOGLN();
}

/**************************************************************************//**
 * Sets the counter value on UI.
 *****************************************************************************/
void throughput_ui_set_count(throughput_count_t count)
{
  char buffer[BUFFER_SIZE];
  snprintf(buffer, BUFFER_SIZE, THROUGHPUT_UI_CNT_FORMAT, (int)count);
  clear_row(ROW_COUNT);
  write_row(buffer, ROW_COUNT);
  LOG(buffer);
  LOGLN();
}
