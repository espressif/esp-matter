/*******************************************************************************
* @file  rsi_wlan_app.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
/*************************************************************************
 *
 */

/*================================================================================
 * @brief : This file contains example application for Wlan Station BLE
 * Provisioning
 * @section Description :
 * This application explains how to get the WLAN connection functionality using
 * BLE provisioning.
 * Silicon Labs Module starts advertising and with BLE Provisioning the Access Point
 * details are fetched.
 * Silicon Labs device is configured as a WiFi station and connects to an Access Point.
 =================================================================================*/

/**
 * Include files
 * */
#include <rsi_driver.h>

// include file to refer data types
#include "rsi_data_types.h"

// COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

// WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"

// WLAN include file for configuration
#include <rsi_wlan_config.h>

// socket include file to refer socket APIs
#include "rsi_socket.h"

#include "rsi_bootup_config.h"
// Error include files
#include "rsi_error.h"

#include "rsi_wlan_non_rom.h"

// OS include file to refer OS specific functionality
#include "rsi_os.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#include "rsi_chip.h"
#endif

// Type of throughput
#define UDP_TX 0
#define UDP_RX 1
#define TCP_TX 2
#define TCP_RX 3

#define SOCKET_ASYNC_FEATURE 1
// Power measurement type
#define THROUGHPUT_TYPE UDP_TX

// Memory to initialize driver
uint32_t total_tx_bytes = 0;
uint32_t total_rx_bytes;
uint32_t secs;
#ifdef RSI_WITH_OS
extern rsi_semaphore_handle_t wlan_thread_sem;
#endif
/*
 *********************************************************************************************************
 *                                         LOCAL GLOBAL VARIABLES
 *********************************************************************************************************
 */

rsi_rsp_scan_t scan_result;
uint16_t scanbuf_size;
uint8_t connected = 0, timeout = 0;
uint8_t disconnected = 0, disassosiated = 0;
uint8_t a = 0;
uint8_t rsp_buf[18];
static uint32_t wlan_app_event_map;

/*
 *********************************************************************************************************
 *                                               DATA TYPES
 *********************************************************************************************************
 */

typedef enum rsi_app_cmd_e {
  RSI_DATA                = 0,
  RSI_WLAN_SCAN_RESP      = 1,
  RSI_WLAN_CONN_STATUS    = 2,
  RSI_WLAN_DISCONN_STATUS = 3,
  RSI_WLAN_DISCONN_NOTIFY = 4,
  RSI_WLAN_TIMEOUT_NOTIFY = 5

} rsi_app_cmd_t;

extern void rsi_wlan_app_send_to_ble(uint16_t msg_type, uint8_t *data, uint16_t data_len);
extern uint8_t coex_ssid[50], pwd[34], sec_type;
uint8_t retry = 1;

uint8_t conn_status;

extern uint8_t magic_word;

#define RSI_APP_BUF_SIZE 1600

uint32_t received_length = 0;
/*==============================================*/
/**
 * @fn         rsi_wlan_app_set_event
 * @brief      sets the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void rsi_wlan_app_set_event(uint32_t event_num)
{
  wlan_app_event_map |= BIT(event_num);
#ifdef RSI_WITH_OS
  rsi_semaphore_post(&wlan_thread_sem);
#endif
  return;
}

/*==============================================*/
/**
 * @fn         rsi_wlan_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
void rsi_wlan_app_clear_event(uint32_t event_num)
{
  wlan_app_event_map &= ~BIT(event_num);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_wlan_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
int32_t rsi_wlan_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 32; ix++) {
    if (wlan_app_event_map & (1 << ix)) {
      return ix;
    }
  }

  return (-1);
}

// Call back for Socket Async
void socket_async_recive(uint32_t sock_no, uint8_t *buffer, uint32_t length)
{
  UNUSED_PARAMETER(sock_no);
  UNUSED_PARAMETER(buffer);
  received_length += length;
}

// rejoin failure call back handler in station mode
void rsi_join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(buffer);
  UNUSED_CONST_PARAMETER(length);
  // update wlan application state
  disconnected = 1;
  connected    = 0;
  rsi_wlan_app_set_event(RSI_WLAN_DISCONNECTED_STATE);
}

void rsi_wlan_app_call_backs_init(void)
{
  // Initialize join fail call back
  rsi_wlan_register_callbacks(RSI_JOIN_FAIL_CB, rsi_join_fail_handler);
}

void rsi_wlan_app_task()
{
  int32_t status   = RSI_SUCCESS;
  int32_t event_id = 0;
#ifdef RSI_WITH_OS
  while (1) {
#endif
    // checking for events list
    event_id = rsi_wlan_app_get_event();
    if (event_id == -1) {
#ifdef RSI_WITH_OS
      rsi_semaphore_wait(&wlan_thread_sem, 0);
      // if events are not received loop will be continued.
      continue;
#else
    // if events are not received loop will not be continued.
    return;
#endif
    }
    switch (event_id) {
      case RSI_WLAN_INITIAL_STATE: {
        rsi_wlan_app_clear_event(RSI_WLAN_INITIAL_STATE);
        // update wlan application state
#if (WYZBEE_CONFIGURATOR || BLE_SCANNER)
        rsi_wlan_app_call_backs_init();

        if (magic_word) {
          // clear the served event
          rsi_wlan_app_set_event(RSI_WLAN_FLASH_STATE);

        } else {
          rsi_wlan_app_set_event(RSI_WLAN_SCAN_STATE);
        }

#elif (STATIC_AP_OPEN || STATIC_AP_WPA2_SECURE || STATIC_AP_WPA_SECURE || STATIC_AP_MIXED)
      rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
#endif
      } break;

      case RSI_WLAN_UNCONNECTED_STATE: {
        rsi_wlan_app_clear_event(RSI_WLAN_UNCONNECTED_STATE);
        //Any additional code if required
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&wlan_thread_sem);
#endif
      } break;

      case RSI_WLAN_SCAN_STATE: {
        rsi_wlan_app_clear_event(RSI_WLAN_SCAN_STATE);
        scanbuf_size = sizeof(scan_result);
        memset(&scan_result, 0, sizeof(scan_result));

        status = rsi_wlan_scan(0, 0, &scan_result, scanbuf_size);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWLAN Scan Failed, Error Code : 0x%lX\r\n", status);
          rsi_wlan_app_set_event(RSI_WLAN_SCAN_STATE);

        } else {
          // update wlan application state

#if WYZBEE_CONFIGURATOR
          rsi_wlan_app_send_to_ble(RSI_WLAN_SCAN_RESP, (uint8_t *)&scan_result, scanbuf_size);

#elif (STATIC_AP_OPEN || STATIC_AP_WPA2_SECURE || STATIC_AP_WPA_SECURE || STATIC_AP_MIXED)
        rsi_wlan_app_cb.state = RSI_WLAN_JOIN_STATE;
#endif
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&wlan_thread_sem);
#endif
      } break;

      case RSI_WLAN_JOIN_STATE: {
#if WYZBEE_CONFIGURATOR
        if (sec_type == 0 || sec_type == '0') {
          status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_OPEN, NULL);
        } else if (sec_type == 1 || sec_type == '1') {
          status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA, pwd);
        } else if (sec_type == 2 || sec_type == '2') {
          status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA2, pwd);
        } else if (sec_type == 6 || sec_type == '6') {
          status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA_WPA2_MIXED, pwd);
        }
#endif

#if STATIC_AP_OPEN
        status = rsi_wlan_connect((int8_t *)static_ap_ssid, RSI_OPEN, NULL);
#endif

#if STATIC_AP_WPA2_SECURE
        status = rsi_wlan_connect((int8_t *)static_ap_ssid, RSI_WPA2, static_ap_pwd);
#endif

#if STATIC_AP_WPA_SECURE
        status = rsi_wlan_connect((int8_t *)static_ap_ssid, RSI_WPA, static_ap_pwd);
#endif

#if STATIC_AP_MIXED
        status = rsi_wlan_connect((int8_t *)static_ap_ssid, RSI_WPA_WPA2_MIXED, static_ap_pwd);
#endif

        if (status != RSI_SUCCESS) {
#if WYZBEE_CONFIGURATOR
          timeout = 1;
          rsi_wlan_app_send_to_ble(RSI_WLAN_TIMEOUT_NOTIFY, (uint8_t *)&timeout, 1);
          rsi_wlan_app_clear_event(RSI_WLAN_JOIN_STATE);
#endif
          LOG_PRINT("\r\nWLAN Connect Failed, Error Code : 0x%lX\r\n", status);
        } else {
          // update wlan application state
          rsi_wlan_app_clear_event(RSI_WLAN_JOIN_STATE);
          rsi_wlan_app_set_event(RSI_WLAN_CONNECTED_STATE);
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&wlan_thread_sem);
#endif
      } break;

      case RSI_WLAN_FLASH_STATE: {
        rsi_wlan_app_clear_event(RSI_WLAN_FLASH_STATE);
        if (retry) {
#if WYZBEE_CONFIGURATOR
          if (sec_type == 0 || sec_type == '0') {
            status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_OPEN, NULL);
          } else if (sec_type == 1 || sec_type == '1') {
            status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA, pwd);
          } else if (sec_type == 2 || sec_type == '2') {
            status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA2, pwd);
          } else if (sec_type == 6 || sec_type == '6') {
            status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA_WPA2_MIXED, pwd);
          }
#endif
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\nWLAN Connect Failed, Error Code : 0x%lX\r\n", status);
            break;
          } else {
            rsi_wlan_app_set_event(RSI_WLAN_CONNECTED_STATE);
          }
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&wlan_thread_sem);
#endif
      } break;

      case RSI_WLAN_CONNECTED_STATE: {
        rsi_wlan_app_clear_event(RSI_WLAN_CONNECTED_STATE);
        // Configure IP
        status = rsi_config_ipaddress(RSI_IP_VERSION_4, RSI_DHCP, 0, 0, 0, rsp_buf, sizeof(rsp_buf), 0);
        if (status != RSI_SUCCESS) {
#if WYZBEE_CONFIGURATOR
          a++;
          if (a == 3) {
            a       = 0;
            timeout = 1;
            status  = rsi_wlan_disconnect();
            if (status == RSI_SUCCESS) {
              connected     = 0;
              disassosiated = 1;
              rsi_wlan_app_send_to_ble(RSI_WLAN_TIMEOUT_NOTIFY, (uint8_t *)&timeout, 1);
              rsi_wlan_app_set_event(RSI_WLAN_ERROR_STATE);
            }
          }
#endif
          LOG_PRINT("\r\nIP Config Failed, Error Code : 0x%lX\r\n", status);
          break;
        } else {
          a             = 0;
          connected     = 1;
          conn_status   = 1;
          disconnected  = 0;
          disassosiated = 0;
          // update wlan application state
          rsi_wlan_app_set_event(RSI_WLAN_IPCONFIG_DONE_STATE);

#if WYZBEE_CONFIGURATOR
          rsi_wlan_app_send_to_ble(RSI_WLAN_CONN_STATUS, (uint8_t *)&connected, 1);
#endif
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&wlan_thread_sem);
#endif
      } break;

      case RSI_WLAN_IPCONFIG_DONE_STATE: {
        rsi_wlan_app_clear_event(RSI_WLAN_IPCONFIG_DONE_STATE);
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&wlan_thread_sem);
#endif
      } break;

      case RSI_WLAN_ERROR_STATE: {

      } break;

      case RSI_WLAN_DISCONNECTED_STATE: {
        rsi_wlan_app_clear_event(RSI_WLAN_DISCONNECTED_STATE);
        retry = 1;
        rsi_wlan_app_send_to_ble(RSI_WLAN_DISCONN_STATUS, (uint8_t *)&disconnected, 1);
        rsi_wlan_app_set_event(RSI_WLAN_FLASH_STATE);
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&wlan_thread_sem);
#endif
      } break;

      case RSI_WLAN_DISCONN_NOTIFY_STATE: {
        rsi_wlan_app_clear_event(RSI_WLAN_DISCONN_NOTIFY_STATE);
        status = rsi_wlan_disconnect();
        if (status == RSI_SUCCESS) {
#if RSI_WISE_MCU_ENABLE
          rsi_flash_erase((uint32_t)FLASH_ADDR_TO_STORE_AP_DETAILS);
#endif
          LOG_PRINT("\r\nWLAN Disconnected\r\n");
          disassosiated = 1;
          connected     = 0;
          rsi_wlan_app_send_to_ble(RSI_WLAN_DISCONN_NOTIFY, (uint8_t *)&disassosiated, 1);
          rsi_wlan_app_set_event(RSI_WLAN_UNCONNECTED_STATE);
        } else {
          LOG_PRINT("\r\nWLAN Disconnect Failed, Error Code : 0x%lX\r\n", status);
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&wlan_thread_sem);
#endif
      } break;
      case RSI_WLAN_SOCKET_RECEIVE_STATE:
        break;
      case RSI_WLAN_MQTT_INIT_DONE_STATE:
        break;
      case RSI_WLAN_MQTT_SUBSCRIBE_DONE_STATE:
        break;
      case RSI_BLE_GATT_WRITE_EVENT:
        break;
      case RSI_WLAN_DATA_RECEIVE_STATE:
        break;
      case RSI_SD_WRITE_STATE:
        break;
      case RSI_WLAN_DEMO_COMPLETE_STATE:
        break;
      default:
        break;
    }
#ifdef RSI_WITH_OS
  }
#endif
}
