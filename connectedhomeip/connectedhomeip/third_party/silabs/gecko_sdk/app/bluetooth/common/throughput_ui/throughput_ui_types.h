/***************************************************************************//**
 * @file
 * @brief User Interface type definitions for throughput test UI.
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

/*******************************************************************************
 ******************************   DEFINITIONS   ********************************
 ******************************************************************************/
#define THROUGHPUT_UI_ROWS        13
#define THROUGHPUT_UI_COLS        16

/*******************************************************************************
 ********************************  TYPEDEFS  ***********************************
 ******************************************************************************/

// Enum type for UI rows
typedef enum {
  ROW_ROLE        = 0,
  ROW_STATE       = 1,
  ROW_TX_POWER    = 2,
  ROW_RSSI        = 3,
  ROW_PHY         = 4,
  ROW_INTERVAL    = 5,
  ROW_PDU_SIZE    = 6,
  ROW_MTU_SIZE    = 7,
  ROW_DATA_SIZE   = 8,
  ROW_NOTIFY      = 9,
  ROW_INDICATE    = 10,
  ROW_THROUGHPUT  = 11,
  ROW_COUNT       = 12,
  ROW_ALL         = THROUGHPUT_UI_ROWS
} throughput_ui_row;

#define THROUGHPUT_UI_YESNO(x)                   ((x) ? ("Yes") : ("No"))

#define THROUGHPUT_UI_ROLE_PERIPHERAL_TEXT       "ROLE: PERIPHERAL"
#define THROUGHPUT_UI_ROLE_CENTRAL_TEXT          "ROLE: CENTRAL"

#define THROUGHPUT_UI_STATE_CONNECTED_TEXT       "ST: Connected"
#define THROUGHPUT_UI_STATE_DISCONNECTED_TEXT    "ST: Disconn."
#define THROUGHPUT_UI_STATE_SUBSCRIBED_TEXT      "ST: Subscribed"
#define THROUGHPUT_UI_STATE_TEST_TEXT            "ST: Testing"
#define THROUGHPUT_UI_STATE_UNKNOWN_TEXT         "ST: Unknown"

#define THROUGHPUT_UI_TX_POWER_FORMAT    "TX: %+4d dBm"
#define THROUGHPUT_UI_RSSI_FORMAT                "RSSI: %+4d dBm"
#define THROUGHPUT_UI_INTERVAL_FORMAT            "INTERVAL: %04d"
#define THROUGHPUT_UI_LATENCY_FORMAT             "LATENCY: %04d"
#define THROUGHPUT_UI_TIMEOUT_FORMAT             "TIMEOUT: %04d"

#define THROUGHPUT_UI_PDU_SIZE_FORMAT            "PDU: %3d"
#define THROUGHPUT_UI_MTU_SIZE_FORMAT            "MTU: %3d"
#define THROUGHPUT_UI_DATA_SIZE_FORMAT           "DATA: %3d"

#define THROUGHPUT_UI_PHY_1M_TEXT                "PHY: 1M"
#define THROUGHPUT_UI_PHY_2M_TEXT                "PHY: 2M"
#define THROUGHPUT_UI_PHY_CODED_125K_TEXT        "PHY: CODED 125K"
#define THROUGHPUT_UI_PHY_CODED_500K_TEXT        "PHY: CODED 500K"
#define THROUGHPUT_UI_PHY_UNKNOWN_TEXT           "PHY: -"

#define THROUGHPUT_UI_NOTIFY_YES_TEXT            "NOTIFY: Yes"
#define THROUGHPUT_UI_NOTIFY_NO_TEXT             "NOTIFY: No"

#define THROUGHPUT_UI_INDICATE_YES_TEXT          "INDICATE: Yes"
#define THROUGHPUT_UI_INDICATE_NO_TEXT           "INDICATE: No"

#define THROUGHPUT_UI_RESULT_YES_TEXT            "RESULT: Yes"
#define THROUGHPUT_UI_RESULT_NO_TEXT             "RESULT: No"

#define THROUGHPUT_UI_TH_FORMAT                  "TH: %07d bps"
#define THROUGHPUT_UI_CNT_FORMAT                 "CNT: %09d"
#define THROUGHPUT_UI_LOST_FORMAT                "LOST: %09d"
#define THROUGHPUT_UI_ERROR_FORMAT               "ERR: %09d"
#define THROUGHPUT_UI_TIME_FORMAT                "TIME: %09d"

#define THROUGHPUT_UI_DISCOVERY_STATE_IDLE_TEXT            "DISCOVERY: IDLE"
#define THROUGHPUT_UI_DISCOVERY_STATE_CONN_TEXT            "DISCOVERY: CONNECTING"
#define THROUGHPUT_UI_DISCOVERY_STATE_SCAN_TEXT            "DISCOVERY: SCAN"
#define THROUGHPUT_UI_DISCOVERY_STATE_SERVICE_TEXT         "DISCOVERY: SERVICE"
#define THROUGHPUT_UI_DISCOVERY_STATE_CHARACTERISTICS_TEXT "DISCOVERY: CHARACTERISTICS"
#define THROUGHPUT_UI_DISCOVERY_STATE_FINISHED_TEXT        "DISCOVERY: FINISHED"
#define THROUGHPUT_UI_DISCOVERY_STATE_UNKNOWN_TEXT         "DISCOVERY: UNKNOWN"

#define THROUGHPUT_UI_CHARACTERISTICS_FOUND_FORMAT \
  "CHARACTERISTICS: "                              \
  "NOTIFY: %s INDICATE %s TRANSMISSION %s RESULT: %s"
