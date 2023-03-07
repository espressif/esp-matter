/*******************************************************************************
* @file  rsi_common.h
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef RSI_COMMON_H
#define RSI_COMMON_H
#ifndef EFM32_SDIO // This file is not needed for EFM32 board. In order to avoid compilation warnings, we excluded the below code for EFM32
#include "rsi_board_configuration.h"
#endif
#ifdef RSI_M4_INTERFACE
#include "rsi_ipmu.h"
#endif
#include "rsi_wlan_non_rom.h"
#include "rsi_timer.h"
#include "rsi_pkt_mgmt.h"

/******************************************************
 * *                      Macros
 * ******************************************************/

// Power save SLP and WKP frame type
#define RSI_RSP_SLP 0xDE
#define RSI_RSP_WKP 0xDD

// Max packet length of WLAN tx packet
#define RSI_WLAN_CMD_LEN (1600 + SIZE_OF_HEADROOM)

// Max packet length of ZigBee tx packet
#define RSI_ZIGB_CMD_LEN (256 + SIZE_OF_HEADROOM)

// pool size of WLAN tx packets
#define RSI_WLAN_POOL_SIZE                             \
  ((((((uint32_t)(sizeof(rsi_pkt_pool_t))) + 3) & ~3)) \
   + ((RSI_WLAN_CMD_LEN + sizeof(void *)) * RSI_WLAN_TX_POOL_PKT_COUNT))

// pool size of WLAN tx packets
#define RSI_ZIGB_POOL_SIZE                             \
  ((((((uint32_t)(sizeof(rsi_pkt_pool_t))) + 3) & ~3)) \
   + ((RSI_ZIGB_CMD_LEN + sizeof(void *)) * RSI_ZIGB_TX_POOL_PKT_COUNT))

// Max packet length of common command responses
//changed cmd len for crypto and PUF
#if defined(RSI_PUF_ENABLE) || (defined RSI_CRYPTO_ENABLE)
#define RSI_COMMON_CMD_LEN 1600
#else
#define RSI_COMMON_CMD_LEN 100
#endif

// pool size of common command packets
#define RSI_COMMON_POOL_SIZE                           \
  ((((((uint32_t)(sizeof(rsi_pkt_pool_t))) + 3) & ~3)) \
   + ((RSI_COMMON_CMD_LEN + sizeof(void *)) * RSI_COMMON_TX_POOL_PKT_COUNT))

// Max packet length of rx packet
#if !((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
#define RSI_DRIVER_RX_PKT_LEN 1600
#else
#define RSI_DRIVER_RX_PKT_LEN 0
#endif
// pool size of driver rx packets
#define RSI_DRIVER_RX_POOL_SIZE                        \
  ((((((uint32_t)(sizeof(rsi_pkt_pool_t))) + 3) & ~3)) \
   + ((RSI_DRIVER_RX_PKT_LEN + sizeof(void *)) * RSI_DRIVER_RX_POOL_PKT_COUNT))

#define RSI_EVENT_INFO_POOL_SIZE (((((uint32_t)(sizeof(rsi_event_cb_t))) + 3) & ~3) * RSI_MAX_NUM_EVENTS)
// pool size of socket information
#ifdef RSI_WLAN_ENABLE
#define RSI_SOCKET_INFO_POOL_SIZE (((((uint32_t)(sizeof(rsi_socket_info_t))) + 3) & ~3) * RSI_NUMBER_OF_SOCKETS)
#define RSI_SOCKET_INFO_POOL_ROM_SIZE \
  (((((uint32_t)(sizeof(rsi_socket_info_non_rom_t))) + 3) & ~3) * RSI_NUMBER_OF_SOCKETS)
#define RSI_SOCKET_SELECT_INFO_POOL_SIZE \
  (((((uint32_t)(sizeof(rsi_socket_select_info_t))) + 3) & ~3) * RSI_NUMBER_OF_SELECTS)
#else
#define RSI_SOCKET_INFO_POOL_SIZE        0
#define RSI_SOCKET_INFO_POOL_ROM_SIZE    0
#define RSI_SOCKET_SELECT_INFO_POOL_SIZE 0
#endif
// Max packet length of BT COMMON tx packet
#define RSI_BT_COMMON_CMD_LEN (300 + SIZE_OF_HEADROOM)
#if ENCODER_IN_RS9116
#define RSI_BT_CLASSIC_CMD_LEN 4000
#else
#define RSI_BT_CLASSIC_CMD_LEN (1040 + SIZE_OF_HEADROOM)
#endif
#define RSI_BLE_CMD_LEN (300 + SIZE_OF_HEADROOM)
#ifdef RSI_PROP_PROTOCOL_ENABLE
#define RSI_PROP_PROTOCOL_CMD_LEN (300 + SIZE_OF_HEADROOM)
#endif

#ifdef SAPIS_BT_STACK_ON_HOST
#define RSI_OPERMODE_WLAN_BTBLE_DUALMODE_STACK_BYPASS (16 + 8 + 4 + 1)
#define RSI_OPERMODE_WLAN_BTBLE_DUALROLE_STACK_BYPASS (234)
#endif
// pool size of BT COMMON tx packets
#define RSI_BT_COMMON_POOL_SIZE                        \
  ((((((uint32_t)(sizeof(rsi_pkt_pool_t))) + 3) & ~3)) \
   + ((RSI_BT_COMMON_CMD_LEN + sizeof(void *)) * RSI_BT_COMMON_TX_POOL_PKT_COUNT))

// pool size of BT Classic tx packets
#define RSI_BT_CLASSIC_POOL_SIZE                       \
  ((((((uint32_t)(sizeof(rsi_pkt_pool_t))) + 3) & ~3)) \
   + ((RSI_BT_CLASSIC_CMD_LEN + sizeof(void *)) * RSI_BT_CLASSIC_TX_POOL_PKT_COUNT))

// pool size of BLE tx packets
#define RSI_BLE_POOL_SIZE                              \
  ((((((uint32_t)(sizeof(rsi_pkt_pool_t))) + 3) & ~3)) \
   + ((RSI_BLE_CMD_LEN + sizeof(void *)) * RSI_BLE_TX_POOL_PKT_COUNT))
#ifdef RSI_PROP_PROTOCOL_ENABLE
// pool size of PROP_PROTOCOL tx packets
#define RSI_PROP_PROTOCOL_POOL_SIZE                    \
  ((((((uint32_t)(sizeof(rsi_pkt_pool_t))) + 3) & ~3)) \
   + ((RSI_PROP_PROTOCOL_CMD_LEN + sizeof(void *)) * RSI_PROP_PROTOCOL_TX_POOL_PKT_COUNT))
#endif
#ifdef RSI_M4_INTERFACE
#define RSI_M4_MEMORY_POOL_SIZE ((((uint32_t)(sizeof(efuse_ipmu_t))) + 3) & ~3)
#else
#define RSI_M4_MEMORY_POOL_SIZE 0
#endif

#ifdef PROCESS_SCAN_RESULTS_AT_HOST
#define SCAN_RESULTS_MEMORY_POOL_SIZE ((((uint32_t)(sizeof(struct wpa_scan_results_arr))) + 3) & ~3)
#else
#define SCAN_RESULTS_MEMORY_POOL_SIZE 0
#endif

#define RSI_WLAN_CB_NON_ROM_POOL_SIZE ((((uint32_t)(sizeof(rsi_wlan_cb_non_rom_t))) + 3) & ~3)

#ifdef RSI_ZB_ENABLE
#ifdef ZB_MAC_API
#define ZB_GLOBAL_BUF_SIZE ((((uint32_t)(sizeof(rsi_zigb_global_mac_cb_t))) + 3) & ~3)
#else
#define ZB_GLOBAL_BUF_SIZE ((((uint32_t)(sizeof(rsi_zigb_global_cb_t))) + 3) & ~3)
#endif
#endif

#ifdef RSI_ZB_ENABLE
#define RSI_ZB_MEMORY_POOL_SIZE \
  ((((uint32_t)(sizeof(rsi_zigb_cb_t))) + 3) & ~3) + RSI_ZIGB_POOL_SIZE + ZB_GLOBAL_BUF_SIZE
#else
#define RSI_ZB_MEMORY_POOL_SIZE 0
#endif

#if (defined RSI_BT_ENABLE || defined RSI_BLE_ENABLE || defined RSI_PROP_PROTOCOL_ENABLE)
#define RSI_BT_BLE_PROP_PROTOCOL_MEMORY_POOL_SIZE                          \
  ((((uint32_t)(sizeof(rsi_bt_cb_t))) + 3) & ~3) + RSI_BT_COMMON_POOL_SIZE \
    + ((((uint32_t)(sizeof(rsi_bt_global_cb_t))) + 3) & ~3)
#else
#define RSI_BT_BLE_PROP_PROTOCOL_MEMORY_POOL_SIZE 0
#endif

#ifdef RSI_BT_ENABLE
#define RSI_BT_MEMORY_POOL_SIZE                                             \
  ((((uint32_t)(sizeof(rsi_bt_cb_t))) + 3) & ~3) + RSI_BT_CLASSIC_POOL_SIZE \
    + ((((uint32_t)(sizeof(rsi_bt_classic_cb_t))) + 3) & ~3)
#else
#define RSI_BT_MEMORY_POOL_SIZE 0
#endif

#ifdef RSI_BLE_ENABLE
#define RSI_BLE_MEMORY_POOL_SIZE \
  ((((uint32_t)(sizeof(rsi_bt_cb_t))) + 3) & ~3) + RSI_BLE_POOL_SIZE + ((((uint32_t)(sizeof(rsi_ble_cb_t))) + 3) & ~3)
#else
#define RSI_BLE_MEMORY_POOL_SIZE 0
#endif

#ifdef RSI_PROP_PROTOCOL_ENABLE
#define PROP_PROTOCOL_MEMORY_SIZE                                              \
  ((((uint32_t)(sizeof(rsi_bt_cb_t))) + 3) & ~3) + RSI_PROP_PROTOCOL_POOL_SIZE \
    + ((((uint32_t)(sizeof(rsi_prop_protocol_cb_t))) + 3) & ~3)
#else
#define PROP_PROTOCOL_MEMORY_SIZE 0
#endif

#ifdef SAPIS_BT_STACK_ON_HOST
#define BT_STACK_ON_HOST_MEMORY ((((uint32_t)(sizeof(rsi_bt_cb_t))) + 3) & ~3) + RSI_BT_CLASSIC_POOL_SIZE
#else
#define BT_STACK_ON_HOST_MEMORY 0
#endif

#define RSI_DRIVER_POOL_SIZE                                                                                          \
  sizeof(uint32_t) + ((((uint32_t)(sizeof(rsi_driver_cb_t))) + 3) & ~3) + RSI_DRIVER_RX_POOL_SIZE                     \
    + ((((uint32_t)(sizeof(rsi_common_cb_t))) + 3) & ~3) + ((((uint32_t)(sizeof(rsi_driver_cb_non_rom_t))) + 3) & ~3) \
    + RSI_COMMON_POOL_SIZE + ((((uint32_t)(sizeof(rsi_wlan_cb_t))) + 3) & ~3) + RSI_WLAN_POOL_SIZE                    \
    + RSI_M4_MEMORY_POOL_SIZE + RSI_SOCKET_INFO_POOL_SIZE + RSI_SOCKET_INFO_POOL_ROM_SIZE                             \
    + RSI_SOCKET_SELECT_INFO_POOL_SIZE + SCAN_RESULTS_MEMORY_POOL_SIZE + RSI_ZB_MEMORY_POOL_SIZE                      \
    + RSI_BT_BLE_PROP_PROTOCOL_MEMORY_POOL_SIZE + RSI_BT_MEMORY_POOL_SIZE + RSI_BLE_MEMORY_POOL_SIZE                  \
    + PROP_PROTOCOL_MEMORY_SIZE + BT_STACK_ON_HOST_MEMORY + RSI_EVENT_INFO_POOL_SIZE + RSI_WLAN_CB_NON_ROM_POOL_SIZE  \
    + ((((uint32_t)(sizeof(global_cb_t))) + 3) & ~3) + ((((uint32_t)(sizeof(rom_apis_t))) + 3) & ~3)

#define RSI_WAIT_FOREVER            0
#define RSI_ZIGB_RESPONSE_WAIT_TIME RSI_WAIT_FOREVER
#define RSI_WAIT_TIME               RSI_WAIT_FOREVER

#define RSI_TX_EVENT_WAIT_TIME (30000 + TX_WAIT_TIME)
#define DEFAULT_TIMEOUT        RSI_TX_EVENT_WAIT_TIME
#define WAIT_TIMEOOUT          5000
// Internal command timeout defines
#define RSI_OPERMODE_RESPONSE_WAIT_TIME         ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BAND_RESPONSE_WAIT_TIME             ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_MAC_RESPONSE_WAIT_TIME              ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_DISCONNECT_RESPONSE_WAIT_TIME       ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_TRANSMIT_RESPONSE_WAIT_TIME         ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_RX_STATS_RESPONSE_WAIT_TIME         ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WMM_RESPONSE_WAIT_TIME              ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_AUTO_CONFIG_RESPONSE_WAIT_TIME      ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SOCK_CONFIG_RESPONSE_WAIT_TIME      ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_ANTENNA_SEL_RESPONSE_WAIT_TIME      ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_DEBUG_LOG_RESPONSE_WAIT_TIME        ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_GET_RAM_DUMP_RESPONSE_WAIT_TIME     ((1000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_FEATURE_FRAME_RESPONSE_WAIT_TIME    ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_UART_FLOW_CTRL_RESPONSE_WAIT_TIME   ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PWRMODE_RESPONSE_WAIT_TIME          ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PUF_START_RESPONSE_WAIT_TIME        ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PUF_SET_RESPONSE_WAIT_TIME          ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PUF_DIS_SET_RESPONSE_WAIT_TIME      ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PUF_GET_RESPONSE_WAIT_TIME          ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PUF_DIS_GET_RESPONSE_WAIT_TIME      ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PUF_LOAD_RESPONSE_WAIT_TIME         ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_AES_ENCRYPT_RESPONSE_WAIT_TIME      ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_AES_DECRYPT_RESPONSE_WAIT_TIME      ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_AES_MAC_RESPONSE_WAIT_TIME          ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PUF_INTR_RESPONSE_WAIT_TIME         ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SEND_DATA_RESPONSE_WAIT_TIME        ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SWITCH_PROTO_RESPONSE_WAIT_TIME     ((5000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_RAW_DATA_RESPONSE_WAIT_TIME         ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PUF_DIS_ENROLL_RESPONSE_WAIT_TIME   ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PUF_ENROLL_RESPONSE_WAIT_TIME       ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_GAIN_TABLE_RESPONSE_WAIT_TIME       ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_GET_PROFILE_RESPONSE_WAIT_TIME      ((1000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SET_PROFILE_RESPONSE_WAIT_TIME      ((1000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_EAP_RESPONSE_WAIT_TIME              ((5000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_REGION_RESPONSE_WAIT_TIME           ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_REGION_AP_RESPONSE_WAIT_TIME        ((5000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_DYNAMIC_RESPONSE_WAIT_TIME          ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_ROAMING_RESPONSE_WAIT_TIME          ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_MULTICAST_FIL_RESPONSE_WAIT_TIME    ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_HT_CAPS_RESPONSE_WAIT_TIME          ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_MULTICAST_RESPONSE_WAIT_TIME        ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BCAST_RESPONSE_WAIT_TIME            ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_CARD_READY_WAIT_TIME                ((5000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_INIT_RESPONSE_WAIT_TIME             ((3000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_CONFIG_WAIT_TIME               ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_GET_RANDOM_WAIT_TIME           ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_RSSI_RESPONSE_WAIT_TIME        ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_QUERY_NETWORK_PARAMS_WAIT_TIME ((200 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_REQ_GET_CFG_WAIT_TIME          ((200 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_CFG_SAVE_WAIT_TIME             ((1000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BOARD_READY_WAIT_TIME               ((150000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SET_SLEEP_TIMER_RESPONSE_WAIT_TIME  ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_DEINIT_RESPONSE_WAIT_TIME           ((150000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_CERTIFICATE_RESPONSE_WAIT_TIME      ((1000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_REJOIN_RESPONSE_WAIT_TIME           ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_USER_SC_RESPONSE_WAIT_TIME          ((5000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_REQ_GET_STATS_WAIT_TIME        ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_REQ_EXT_STATS_WAIT_TIME        ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_AUTO_JOIN_RESPONSE_WAIT_TIME        ((60000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_DELETE_PROFILE_RESPONSE_WAIT_TIME   ((1000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_IAP_RESPONSE_WAIT_TIME              ((5000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_IAP_GET_CERT_RESPONSE_WAIT_TIME     ((5000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_IAP_GEN_SIG_RESPONSE_WAIT_TIME      ((5000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_TA_M4_COMMAND_RESPONSE_WAIT_TIME    ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_ADD_MFI_IE_RESPONSE_WAIT_TIME       ((500 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_FILTER_BROADCAST_RESPONSE_WAIT_TIME ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SET_RTC_TIMER_RESPONSE_WAIT_TIME    ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#ifdef RSI_ASSERT_API
#define RSI_ASSERT_RESPONSE_WAIT_TIME ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#endif
#define RSI_WLAN_RADIO_RESPONSE_WAIT_TIME      ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_GET_RTC_TIMER_RESPONSE_WAIT_TIME   ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_TCP_WINDOW_RESPONSE_WAIT_TIME ((5000 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_TIMEOUT_RESPONSE_WAIT_TIME         ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SET_CONFIG_RESPONSE_WAIT_TIME      ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#ifdef FW_LOGGING_ENABLE
#define RSI_DEVICE_LOG_RESPONSE_WAIT_TIME ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#endif
// WIFI WAIT timeout defines
#define RSI_SCAN_RESPONSE_WAIT_TIME             ((10000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_JOIN_RESPONSE_WAIT_TIME             ((120000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_P2P_RESPONSE_WAIT_TIME              ((1000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BGSCAN_RESPONSE_WAIT_TIME           ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PING_RESPONSE_WAIT_TIME             ((60000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WEP_RESPONSE_WAIT_TIME              ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_IPCONFV4_RESPONSE_WAIT_TIME         ((120000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_IPCONFV6_RESPONSE_WAIT_TIME         ((120000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_MDNSD_RESPONSE_WAIT_TIME            ((500000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_HTTP_RESPONSE_WAIT_TIME             ((200000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_AP_CONFIG_RESPONSE_WAIT_TIME        ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_FWUP_RESPONSE_WAIT_TIME             ((180000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WP_LOAD_RESPONSE_WAIT_TIME          ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_JSON_LOAD_RESPONSE_WAIT_TIME        ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WP_ERASE_RESPONSE_WAIT_TIME         ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_JSON_ERASE_RESPONSE_WAIT_TIME       ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WP_CLEAR_ALL_RESPONSE_WAIT_TIME     ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_CONNECT_RESPONSE_WAIT_TIME          ((200000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WPS_RESPONSE_WAIT_TIME              ((180000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_FREQ_OFFSET_WAIT_TIME               ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_CALIB_WRITE_WAIT_TIME               ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SMTP_RESPONSE_WAIT_TIME             ((60000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_DNS_SERVER_ADD_RESPONSE_WAIT_TIME   ((150000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_FTP_RESPONSE_WAIT_TIME              ((120000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_OTA_FWUP_RESPONSE_WAIT_TIME         ((100000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_CRYPTO_RESPONSE_WAIT_TIME           ((2000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SOCKET_CREATE_RESPONSE_WAIT_TIME    ((100000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_CERT_RESPONSE_WAIT_TIME             ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SCAN_WITH_BITMAP_RESPONSE_WAIT_TIME ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_DNS_QUERY_RESPONSE_WAIT_TIME        ((150000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_DNS_UPDATE_RESPONSE_WAIT_TIME       ((150000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_HOST_WP_SEND_RESPONSE_WAIT_TIME     ((200000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_POP3_RESPONSE_WAIT_TIME             ((60000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_HTTP_ABORT_RESPONSE_WAIT_TIME       ((200000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_HTTP_CLIENT_PUT_RESPONSE_WAIT_TIME  ((200000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SNTP_RESPONSE_WAIT_TIME             ((100000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_EMB_MQTT_RESPONSE_WAIT_TIME         ((60000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_PSK_RESPONSE_WAIT_TIME              ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SOCKET_CLOSE_RESPONSE_WAIT_TIME     ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_11AX_WAIT_TIME                 ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_TWT_RESPONSE_WAIT_TIME         ((5000 * WIFI_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))

// WIFI BLOCKED timeout defines
#define RSI_ACCEPT_RESPONSE_WAIT_TIME          (RSI_WAIT_FOREVER * WIFI_BLOCKED_TIMEOUT_SF)
#define RSI_SELECT_RESPONSE_WAIT_TIME          ((RSI_WAIT_FOREVER * WIFI_BLOCKED_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_SOCKET_RECVFROM_RESPONSE_WAIT_TIME ((RSI_WAIT_FOREVER * WIFI_BLOCKED_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_GPIO_WAIT_TIME                     10000
#define RSI_RX_EVENT_WAIT_TIME                 DEFAULT_TIMEOUT
#define RSI_COMMON_SEND_CMD_RESPONSE_WAIT_TIME ((250000 * WIFI_BLOCKED_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_WLAN_SEND_CMD_RESPONSE_WAIT_TIME   ((250000 * WIFI_BLOCKED_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_NWK_SEND_CMD_RESPONSE_WAIT_TIME    ((250000 * WIFI_BLOCKED_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_GPIO_CONFIG_RESP_WAIT_TIME         ((100 * WIFI_INTERNAL_TIMEOUT_SF) + (DEFAULT_TIMEOUT))

//BT/BLE command timeouts in ms
#define RSI_BT_COMMON_CMD_RESP_WAIT_TIME    ((500 * BT_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BT_A2DP_CMD_RESP_WAIT_TIME      ((500 * BT_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BT_A2DP_DATA_CMD_RESP_WAIT_TIME ((2000 * BT_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BT_AVRCP_CMD_RESP_WAIT_TIME     ((200 * BT_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BT_HFP_CMD_RESP_WAIT_TIME       ((500 * BT_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BT_PBAP_CMD_RESP_WAIT_TIME      ((500 * BT_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BT_HID_CMD_RESP_WAIT_TIME       ((500 * BT_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BT_SPP_CMD_RESP_WAIT_TIME       ((500 * BT_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))

#define RSI_BLE_GAP_CMD_RESP_WAIT_TIME  ((500 * BLE_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#define RSI_BLE_GATT_CMD_RESP_WAIT_TIME ((200 * BLE_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#ifdef RSI_PROP_PROTOCOL_ENABLE
#define RSI_PROP_PROTOCOL_CMD_TX_PKT_SEND_WAIT_TIME ((2000 * PROP_PROTOCOL_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))
#endif

#define RSI_BT_BLE_CMD_MAX_RESP_WAIT_TIME ((2000 * BT_COMMON_WAIT_TIMEOUT_SF) + (DEFAULT_TIMEOUT))

/********TA GPIO defines ******************************/
//! GPIO address
#define RSI_GPIO_ADDR 0x40200000
//! PAD enable
#define PAD_REN 0x41380000

/******************************************************/

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

// enumeration for states used in common control block in driver
typedef enum rsi_common_state_e {
  RSI_COMMON_STATE_NONE = 0,
  RSI_COMMON_CARDREADY,
  RSI_COMMON_OPERMODE_DONE
} rsi_common_state_t;

// enumeration for states used in initial control block in driver
typedef enum rsi_device_state_e {
  RSI_DEVICE_STATE_NONE = 0,
  RSI_DRIVER_INIT_DONE,
  RSI_DEVICE_INIT_DONE
} rsi_device_state_t;
// enumeration for command responses used in common control block
typedef enum rsi_common_cmd_response_e {
  RSI_COMMON_RSP_CLEAR                 = 0x00,
  RSI_COMMON_RSP_OPERMODE              = 0x10,
  RSI_COMMON_RSP_ANTENNA_SELECT        = 0x1B,
  RSI_COMMON_RSP_FEATURE_FRAME         = 0xC8,
  RSI_COMMON_RSP_SOFT_RESET            = 0x1C,
  RSI_COMMON_RSP_CARDREADY             = 0x89,
  RSI_COMMON_RSP_PWRMODE               = 0x15,
  RSI_COMMON_RSP_ULP_NO_RAM_RETENTION  = 0xCD,
  RSI_COMMON_RSP_ASYNCHRONOUS          = 0xFF,
  RSI_RSP_ENCRYPT_CRYPTO               = 0x76,
  RSI_COMMON_RSP_UART_FLOW_CTRL_ENABLE = 0xA4,
  RSI_COMMON_RSP_TA_M4_COMMANDS        = 0xB0,
  RSI_COMMON_RSP_DEBUG_LOG             = 0x26

#ifdef RSI_PUF_ENABLE
  ,
  RSI_COMMON_RSP_PUF_ENROLL      = 0xD0,
  RSI_COMMON_RSP_PUF_DIS_ENROLL  = 0xD1,
  RSI_COMMON_RSP_PUF_START       = 0xD2,
  RSI_COMMON_RSP_PUF_SET_KEY     = 0xD3,
  RSI_COMMON_RSP_PUF_DIS_SET_KEY = 0xD4,
  RSI_COMMON_RSP_PUF_GET_KEY     = 0xD5,
  RSI_COMMON_RSP_PUF_DIS_GET_KEY = 0xD6,
  RSI_COMMON_RSP_PUF_LOAD_KEY    = 0xD7,
  RSI_COMMON_RSP_AES_ENCRYPT     = 0xD8,
  RSI_COMMON_RSP_AES_DECRYPT     = 0xD9,
  RSI_COMMON_RSP_AES_MAC         = 0xDA,
  RSI_COMMON_RSP_PUF_INTR_KEY    = 0xCE
#endif

#ifdef RSI_WAC_MFI_ENABLE
  ,
  RSI_COMMON_RSP_IAP_GET_CERTIFICATE   = 0xB6,
  RSI_COMMON_RSP_IAP_INIT              = 0xB7,
  RSI_COMMON_RSP_IAP_GENERATE_SIGATURE = 0xB8
#endif
  // Reusing RSI_WLAN_REQ_FW_VERSION as RSI_COMMON_REQ_FW_VERSION
  ,
  RSI_COMMON_RSP_FW_VERSION   = 0x49,
  RSI_COMMON_RSP_SWITCH_PROTO = 0x77,
  RSI_COMMON_RSP_GET_RAM_DUMP = 0x92
#ifdef RSI_ASSERT_API
  ,
  RSI_COMMON_RSP_ASSERT = 0xE1
#endif
  ,
  RSI_COMMON_RSP_SET_RTC_TIMER = 0xE9,
  RSI_COMMON_RSP_GET_RTC_TIMER = 0xF2
#ifdef CONFIGURE_GPIO_FROM_HOST
  ,
  RSI_COMMON_RSP_GPIO_CONFIG = 0x28
#endif
#ifdef FW_LOGGING_ENABLE
  ,
  RSI_COMMON_RSP_DEVICE_LOGGING_INIT = 0x82
#endif
} rsi_common_cmd_response_t;

// enumeration for command request used in common control block
typedef enum rsi_common_cmd_request_e {

  RSI_COMMON_REQ_OPERMODE              = 0x10,
  RSI_COMMON_REQ_ANTENNA_SELECT        = 0x1B,
  RSI_COMMON_REQ_FEATURE_FRAME         = 0xC8,
  RSI_COMMON_REQ_SOFT_RESET            = 0x1C,
  RSI_COMMON_REQ_PWRMODE               = 0x15,
  RSI_COMMON_REQ_ENCRYPT_CRYPTO        = 0x76,
  RSI_COMMON_REQ_UART_FLOW_CTRL_ENABLE = 0xA4,
  RSI_COMMON_REQ_TA_M4_COMMANDS        = 0xB0,
  RSI_COMMON_REQ_DEBUG_LOG             = 0x26

#ifdef RSI_WAC_MFI_ENABLE
  ,
  RSI_COMMON_REQ_IAP_GET_CERTIFICATE   = 0xB6,
  RSI_COMMON_REQ_IAP_INIT              = 0xB7,
  RSI_COMMON_REQ_IAP_GENERATE_SIGATURE = 0xB8
#endif

#ifdef RSI_PUF_ENABLE
  ,
  RSI_COMMON_REQ_PUF_ENROLL      = 0xD0,
  RSI_COMMON_REQ_PUF_DIS_ENROLL  = 0xD1,
  RSI_COMMON_REQ_PUF_START       = 0xD2,
  RSI_COMMON_REQ_PUF_SET_KEY     = 0xD3,
  RSI_COMMON_REQ_PUF_DIS_SET_KEY = 0xD4,
  RSI_COMMON_REQ_PUF_GET_KEY     = 0xD5,
  RSI_COMMON_REQ_PUF_DIS_GET_KEY = 0xD6,
  RSI_COMMON_REQ_PUF_LOAD_KEY    = 0xD7,
  RSI_COMMON_REQ_AES_ENCRYPT     = 0xD8,
  RSI_COMMON_REQ_AES_DECRYPT     = 0xD9,
  RSI_COMMON_REQ_AES_MAC         = 0xDA,
  RSI_COMMON_REQ_PUF_INTR_KEY    = 0xCE
#endif
  // Reusing RSI_WLAN_REQ_FW_VERSION as RSI_COMMON_REQ_FW_VERSION
  ,
  RSI_COMMON_REQ_FW_VERSION   = 0x49,
  RSI_COMMON_REQ_SWITCH_PROTO = 0x77,
  RSI_COMMON_REQ_GET_RAM_DUMP = 0x92
#ifdef RSI_ASSERT_API
  ,
  RSI_COMMON_REQ_ASSERT = 0xE1
#endif
  ,
  RSI_COMMON_REQ_SET_RTC_TIMER = 0xE9,
  RSI_COMMON_REQ_GET_RTC_TIMER = 0xF2,
  RSI_COMMON_REQ_SET_CONFIG    = 0xBA
#ifdef CONFIGURE_GPIO_FROM_HOST
  ,
  RSI_COMMON_REQ_GPIO_CONFIG = 0x28
#endif
#ifdef FW_LOGGING_ENABLE
  ,
  RSI_COMMON_REQ_DEVICE_LOGGING_INIT = 0x82
#endif
} rsi_common_cmd_request_t;

#ifdef RSI_PUF_ENABLE
// enumeration for Common control block for PUF state machine
typedef enum rsi_puf_state_e {
  RSI_PUF_STATE_NONE = 0,
  RSI_PUF_STATE_ENROLLED,
  RSI_PUF_STATE_STARTED,
  RSI_PUF_STATE_SET_KEY,
  RSI_PUF_STATE_KEY_LOADED,
  RSI_PUF_STATE_DISABLED
} rsi_puf_state_t;
#endif

#ifdef RSI_M4_INTERFACE
typedef enum ta_m4_commands_e {
  RSI_TAKE_M4_64K                 = 1,
  RSI_GET_IPMU_PROGRAMMING_VALUES = 2,
  RSI_READ_TA_REGISTER            = 3,
  RSI_WRITE_TA_REGISTER           = 4,
  // This enum varibale added for M4 has to give indication to TA, for Configure the Clock switching between 1.3V to 3.3 .For more details check Jira Ticket RSC-3802.
  RSI_ENABLE_XTAL = 5,
#ifdef CHIP_9117
  RSI_WRITE_TO_COMMON_FLASH = 6,
#endif
} ta_m4_commands_t;
//  M4 and TA secure handshake request structure.
typedef struct ta_m4_handshake_param {
  // sub_cmd form the  enum ta_m4_commands_e(Main command type is RSI_COMMON_REQ_TA_M4_COMMANDS)
  ta_m4_commands_t sub_cmd;
  // length of input_data
  uint8_t input_data_size;
  // Input data. In this input data first byte is reserved for enable(1) or Disable(0) sub_cmd of this structure.
  uint8_t *input_data;
} ta_m4_handshake_param_t;

#ifdef CHIP_9117
#define RSI_MAX_CHUNK_SIZE 1400

// TA2M4 handshake request structure.
typedef struct rsi_req_ta2m4_s {
  // sub_cmd
  uint8_t sub_cmd;

  // TA flash location
  uint32_t addr;

  // total length of input data
  uint16_t in_buf_len;

  // total length of chunk
  uint16_t chunk_len;

  // more chunks or last chunk
  uint8_t more_chunks;

  //data
  uint8_t input_data[RSI_MAX_CHUNK_SIZE];
} __attribute__((__packed__)) rsi_req_ta2m4_t;
#endif
#endif

#define RSI_DMA_VALID      0
#define RSI_SKIP_DMA_VALID 1
#ifndef RSI_M4_INTERFACE
#define DEBUGOUT(...)
#endif

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// opermode command request structure
typedef struct rsi_opermode_s {
  // operating mode 0-client, 1-p2p, 2-EAP, 6-AP, 8-PER
  uint8_t opermode[4];

  // BIT(0)-Open mode security, BIT(1)-PSK security, BIT(2) JSON objects
  uint8_t feature_bit_map[4];

  // BIT(0)-tcp/ip bypass, BIT(1)-HTTP server,BIT(2)-DHCPV4 client,
  uint8_t tcp_ip_feature_bit_map[4];

  // BIT(3)-DHCPV6 client, BIT(4)-DHCPV4 server, BIT(5)-DHCPV6 server
  uint8_t custom_feature_bit_map[4];

  // Extended custom feature bitmap
  uint8_t ext_custom_feature_bit_map[4];

  // BYTE(0): Reserved
  // BYTE(2): (0-3): Reserved, (4-6): BT classic slaves, 7: A2DP profile enable
  // BYTE(3): (0): A2DP proifle role 0- sink, 1 - source, (1) - A2DP Accelerated Mode Enable, (2-5): reserved, 6 - RF type, 7-reserved
  uint8_t bt_feature_bit_map[4];

  // Extended tcp/ip feature bit map
  uint8_t ext_tcp_ip_feature_bit_map[4];

  // BYTE(0) - BLE nbr of attributes,
  // BYTE(1): (0 - 3): BLE Nbr of GATT services
  // BYTE(1): (4 - 7): BLE Nbr of slaves
  // BYTE(2): (0 - 7) BLE powersave index,
  // BYTE(3): (0 - 2)BLE powersave options,
  // BYTE(3): (3: 5) Reserved,
  // BYTE(3): bit 6: BLE new features enable
  // BYTE(3): bit 7: Reserved,
  // BYTE(4): bits (4 - 5):BLE Nbr of masters
  uint8_t ble_feature_bit_map[4];

  // BYTE(0): (0 - 3): BLE number of connection events
  // BYTE(1): (4 - 5): BLE number of connection events
  uint8_t ble_ext_feature_bit_map[4];
  uint8_t config_feature_bit_map[4];
} rsi_opermode_t;

// Antenna select command request structure
typedef struct rsi_antenna_select_s {
  // Antenna value to set
  uint8_t antenna_value;
  // Antenna 2G gain value
  uint8_t gain_2g;
  // Antenna 5G gain value
  uint8_t gain_5g;

} rsi_antenna_select_t;

// UART  debug prints request structure
typedef struct rsi_debug_uart_print_s {
  // assertion_type
  uint32_t assertion_type;
  // assertion_level
  uint32_t assertion_level;
} rsi_debug_uart_print_t;

// RR feature frame command request structure
typedef struct rsi_feature_frame_s {
  // PLL mode val
  uint8_t pll_mode;
  // rf type
  uint8_t rf_type;
  // wireless mode
  uint8_t wireless_mode;
  // enable ppp
  uint8_t enable_ppp;
  // afe type
  uint8_t afe_type;
  // feature enables
  uint32_t feature_enables;

} rsi_feature_frame_t;

/******************************************************
 * *                    Structures
 * ******************************************************/

// driver power save structure
typedef struct rsi_power_save_s {
  // wlan driver power save profile mode
  uint8_t wlan_psp_mode;

  // bt driver power save profile mode
  uint8_t bt_psp_mode;

  uint8_t zb_psp_mode;
  // wlan_driver power save profile type
  uint8_t wlan_psp_type;

  // bt_driver power save profile type
  uint8_t bt_psp_type;

  uint8_t zb_psp_type;
  // module power save state
  volatile uint32_t module_state;

// Module power save State Machine
#define RSI_IDLE          0
#define RSI_SLP_RECEIVED  1
#define RSI_SLP_ACK_SENT  2
#define RSI_WKUP_RECEIVED 3

  // This is used to detect current module power mode is executing
  uint8_t current_ps_mode;

  // This is used to detect current power save mode profile type
  uint8_t power_save_enable;
} rsi_power_save_t;

// Set Sleep Timer structure
typedef struct rsi_set_sleep_timer_s {
  uint8_t timeval[2];
} rsi_set_sleep_timer_t;

// driver common block structure
typedef struct rsi_common_cb_s {
  // driver common block state
  volatile rsi_common_state_t state;

  // driver common block status
  volatile int32_t status;

  // driver common block mutex
  rsi_mutex_handle_t common_mutex;

  // driver common block command expected response
  rsi_common_cmd_response_t expected_response;

  // driver common block semaphore
  rsi_semaphore_handle_t common_sem;

  // driver common block tx pool
  rsi_pkt_pool_t common_tx_pool;

  // power save backups
  rsi_power_save_t power_save;

  // coex mode variable for power save command
  uint16_t ps_coex_mode;

  // buffer pointer given by application to driver
  uint8_t *app_buffer;

  // buffer length given by application to driver
  uint32_t app_buffer_length;

#ifdef RSI_PUF_ENABLE
  // PUF state variable
  volatile rsi_puf_state_t puf_state;
#endif
#ifdef RSI_M4_INTERFACE
  efuse_ipmu_t *ipmu_calib_data_cb;
#endif
#ifdef RSI_WITH_OS
  rsi_semaphore_handle_t common_card_ready_sem;
#endif
#ifdef WAKEUP_GPIO_INTERRUPT_METHOD
  rsi_semaphore_handle_t wakeup_gpio_sem;
#endif
  uint8_t sync_mode;
#ifdef FW_LOGGING_ENABLE
  void (*sl_fw_log_callback)(uint8_t *log_message, uint16_t log_message_length);
#endif
} rsi_common_cb_t;

typedef enum {
  ALLOW  = 0,
  IN_USE = 1,
} command_state;
typedef enum {
  COMMON_CMD   = 0,
  WLAN_CMD     = 1,
  NWK_CMD      = 2,
  SOCKET_CMD   = 3,
  TX_EVENT_CMD = 4,
  RX_EVENT_CMD = 5,
  BT_CMD       = 6,
#ifdef RSI_PROP_PROTOCOL_ENABLE
  PROP_PROTOCOL_CMD = 7
#endif
} command_type;

typedef enum {
  RSI_NWK_CMD_IN_FREE_STATE = 0,
  RSI_NWK_CMD_IN_PROGRESS   = 1,
} nwk_cmd_state;

typedef enum {
  RSI_WLAN_CMD_IN_FREE_STATE = 0,
  RSI_WLAN_CMD_IN_PROGRESS   = 1,
} wlan_cmd_state;

typedef enum {
  RSI_COMMON_CMD_IN_FREE_STATE = 0,
  RSI_COMMON_CMD_IN_PROGRESS   = 1,
} common_cmd_state;

typedef enum {
  RSI_SOCKET_CMD_IN_FREE_STATE = 0,
  RSI_SOCKET_CMD_IN_PROGRESS   = 1,
} socket_state;

#if ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
#define SDIO_BUFFER_LENGTH 2048
#endif
typedef struct rsi_driver_cb_non_rom {
  uint32_t rom_version_info;
  uint32_t tx_mask_event;
  rsi_mutex_handle_t tx_mutex;
#ifdef SAPI_LOGGING_ENABLE
  rsi_mutex_handle_t logging_mutex;
#endif
#if defined(RSI_DEBUG_PRINTS) || defined(FW_LOGGING_ENABLE)
  rsi_mutex_handle_t debug_prints_mutex;
#endif
  rsi_semaphore_handle_t nwk_sem;
  rsi_semaphore_handle_t wlan_cmd_sem;
  rsi_semaphore_handle_t common_cmd_sem;
  rsi_semaphore_handle_t common_cmd_send_sem;
  rsi_semaphore_handle_t wlan_cmd_send_sem;
  rsi_semaphore_handle_t nwk_cmd_send_sem;
  rsi_semaphore_handle_t send_data_sem;
  uint8_t nwk_wait_bitmap;
  uint8_t wlan_wait_bitmap;
  uint8_t send_wait_bitmap;
  uint8_t common_wait_bitmap;
  uint8_t zigb_wait_bitmap;
  uint8_t bt_wait_bitmap;
  uint8_t bt_cmd_wait_bitmap;
  volatile uint8_t nwk_cmd_state;
  volatile int32_t nwk_status;
  volatile uint8_t socket_state;
  volatile int32_t socket_status;
  volatile uint8_t wlan_cmd_state;
  volatile uint8_t common_cmd_state;
  volatile uint32_t timer_counter;
#ifndef RSI_TX_EVENT_HANDLE_TIMER_DISABLE
  volatile uint32_t driver_timer_start;
  rsi_timer_instance_t timer_start;
#endif
#ifndef RSI_RX_EVENT_HANDLE_TIMER_DISABLE
  volatile uint32_t driver_rx_timer_start;
  rsi_timer_instance_t rx_timer_start;
#endif

#ifdef RSI_SPI_INTERFACE
  uint32_t rsi_spiTimer1;
  uint32_t rsi_spiTimer2;
  uint32_t rsi_spiTimer3;
  uint32_t rsi_spiTimer4;
#endif
#ifdef RSI_SDIO_INTERFACE
  uint32_t rsi_sdioTimer;
#endif
#if ((defined RSI_SDIO_INTERFACE) && (!defined LINUX_PLATFORM))
  uint8_t sdio_read_buff[SDIO_BUFFER_LENGTH];
#endif
  //! timer flag
  uint32_t rx_driver_flag;
  volatile rsi_device_state_t device_state;
#ifndef RSI_WAIT_TIMEOUT_EVENT_HANDLE_TIMER_DISABLE
  //error response handler pointer
  void (*rsi_wait_timeout_handler_error_cb)(int32_t status, uint32_t cmd_type);
#endif
  // buffer pointer given by application to driver
  uint8_t *nwk_app_buffer;

  // buffer length given by application to driver
  uint32_t nwk_app_buffer_length;
} rsi_driver_cb_non_rom_t;
typedef struct rsi_set_config_s {
#define XO_CTUNE_FROM_HOST BIT(0)
  uint32_t code;
  union {
    uint8_t xo_ctune;
  } values;
} rsi_set_config_t;
extern rsi_driver_cb_non_rom_t *rsi_driver_cb_non_rom;
extern rsi_wlan_cb_non_rom_t *rsi_wlan_cb_non_rom;
#define COMMON_PKT          1
#define BT_BLE_STACK_PACKET 2
#define BT_PKT              3
#ifdef RSI_PROP_PROTOCOL_ENABLE
#define PROP_PROTOCOL_PKT 4
#endif
#define ZB_PKT   5
#define WLAN_PKT 6
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_driver_process_common_recv_cmd(rsi_pkt_t *pkt);
int32_t rsi_driver_common_send_cmd(rsi_common_cmd_request_t cmd, rsi_pkt_t *pkt);
int8_t rsi_common_cb_init(rsi_common_cb_t *common_cb);
void rsi_common_set_status(int32_t status);
void rsi_handle_slp_wkp(uint8_t frame_type);
int8_t rsi_req_wakeup(void);
int8_t rsi_wait4wakeup(void);
void rsi_allow_sleep(void);
void rsi_powersave_gpio_init(void);
void rsi_common_packet_transfer_done(rsi_pkt_t *pkt);
int32_t rsi_sleep_mode_decision(rsi_common_cb_t *rsi_common_cb);
int32_t rsi_cmd_m4_ta_secure_handshake(uint8_t sub_cmd_type,
                                       uint8_t input_len,
                                       uint8_t *input_data,
                                       uint8_t output_len,
                                       uint8_t *output_data);
void rsi_update_common_cmd_state_to_free_state(void);
void rsi_update_common_cmd_state_to_progress_state(void);
rsi_error_t rsi_wait_on_common_semaphore(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms);
int32_t rsi_check_and_update_cmd_state(uint8_t cmd_type, uint8_t cmd_state);
void rsi_post_waiting_wlan_semaphore(void);
void rsi_post_waiting_common_semaphore(void);
void rsi_post_waiting_bt_semaphore(void);
int32_t rsi_release_waiting_semaphore(void);
void rsi_check_pkt_queue_and_dequeue(void);
void rsi_free_queue_pkt(uint8_t pkt_dequeued, rsi_pkt_t *pkt);
#ifndef RSI_WAIT_TIMEOUT_EVENT_HANDLE_TIMER_DISABLE
void rsi_wait_timeout_handler_error_cb(int32_t status, uint32_t cmd_type);
void rsi_register_wait_timeout_error_callbacks(void (*callback_handler_ptr)(int32_t status, uint32_t cmd_type));
#endif
#if (!defined RSI_TX_EVENT_HANDLE_TIMER_DISABLE || !defined RSI_RX_EVENT_HANDLE_TIMER_DISABLE)
void rsi_error_timeout_and_clear_events(int32_t error, uint32_t cmd_type);
#endif
int32_t rsi_check_waiting_cmds(rsi_rsp_waiting_cmds_t *response);
int32_t rsi_set_config(uint32_t code, uint8_t value);
#ifdef WAKEUP_GPIO_INTERRUPT_METHOD
void rsi_give_wakeup_indication(void);
void rsi_hal_gpio_clear(void);
void rsi_hal_gpio_mask(void);
void rsi_hal_gpio_unmask(void);
#endif
#endif
