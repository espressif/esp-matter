/*******************************************************************************
* @file  rsi_driver.h
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

#ifndef RSI_DRIVER_H
#define RSI_DRIVER_H

#ifndef EFM32_SDIO // This file is not needed for EFM32 board. In order to avoid compilation warnings, we excluded the below code for EFM32
#include "rsi_board_configuration.h"
#endif
#ifdef RSI_M4_INTERFACE
#include "rsi_ccp_user_config.h"
#endif
#include <rsi_data_types.h>
#include <rsi_error.h>
#include <rsi_wlan_defines.h>
//#ifdef WISECONNECT
#if (defined(__CC_ARM) || defined(WISECONNECT))
#ifdef RSI_WLAN_API_ENABLE
#include <rsi_wlan_config.h>
#endif
#elif defined(RSI_WLAN_ENABLE)
#include <rsi_wlan_config.h>
#endif
#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE))
#ifdef RSI_BT_ENABLE
#include <rsi_bt_config.h>
#endif
#ifdef RSI_BLE_ENABLE
#include <rsi_ble_config.h>
#else
#include "rsi_ble_common_config.h"
#endif
#endif
#ifndef RSI_ENABLE_DEMOS
#include <rsi_wlan_common_config.h>
#endif
#include <rsi_user.h>
#include <rsi_utils.h>
#include <rsi_os.h>
#include <rsi_events.h>
#include <rsi_scheduler.h>
#include <rsi_pkt_mgmt.h>
#include <rsi_queue.h>
#include <rsi_common_apis.h>
#include <rsi_common.h>
#include <rsi_wlan_apis.h>
#include <rsi_wlan.h>
#include <rsi_socket.h>
#include <rsi_timer.h>
#ifdef RSI_SPI_INTERFACE
#include <rsi_spi_intf.h>
#include <rsi_spi_cmd.h>
#endif
#ifdef RSI_SDIO_INTERFACE
#include <rsi_sdio.h>
#endif
#ifdef RSI_UART_INTERFACE
#include <rsi_uart.h>
#endif
#include <rsi_hal.h>
#include <rsi_nwk.h>
#include <rsi_setregion_countryinfo.h>
#include <rsi_bootup_config.h>
#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE))
#include <rsi_bt_common.h>
#include <rsi_bt_apis.h>

#include "rsi_bt.h"
#include "rsi_ble.h"
#include "rsi_bt_common_apis.h"
#endif
#ifdef RSI_ZB_ENABLE
#include <rsi_zb_api.h>
#endif
#include <string.h>
#ifdef __GNUC__
#include "strings.h"
#endif
#ifdef RSI_WAC_MFI_ENABLE
#include "rsi_iap.h"
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
#include "rsi_prop_protocol.h"
#endif
#include "rsi_apis_rom.h"
#include "rsi_wlan_non_rom.h"
#ifdef SAPI_LOGGING_ENABLE
#include "debug_auto_gen.h"
#include "sl_app_logging.h"
#endif
#ifdef FW_LOGGING_ENABLE
#include "sl_fw_logging.h"
#endif

//#include <stdlib.h>
/******************************************************
 * *                      Macros
 * ******************************************************/

// string functions
#define rsi_strcpy(x, y)         strcpy((char *)x, (const char *)y)
#define rsi_strcat(x, y)         strcat((char *)x, (const char *)y)
#define rsi_strlen(x)            strlen((const char *)x)
#define rsi_strncasecmp(x, y, z) strncasecmp((const char *)x, (const char *)y, (size_t)z)
// Below macros were used to avoid warnings for unused variables present in the API arguments
#define USED_PARAMETER(x)   x = x /*To avoid gcc/g++ warnings*/
#define UNUSED_PARAMETER(x) x = x /*To avoid gcc/g++ warnings*/
// for unused constant variables, the below assignment is valid than above
#define USED_CONST_PARAMETER(x)   (void)x /*To avoid gcc/g++ warnings*/
#define UNUSED_CONST_PARAMETER(x) (void)x /*To avoid gcc/g++ warnings*/

//! common  type
#define RSI_COMMON_Q 0
// ZB protocol type
#define RSI_ZB_Q 1

// BT protocol type
#define RSI_BT_Q 2
#ifdef SAPIS_BT_STACK_ON_HOST
#define RSI_BT_INT_MGMT_Q 6
#define RSI_BT_HCI_Q      7
#endif
// WLAN mgmt queue type
#define RSI_WLAN_MGMT_Q 4

// WLAN data queue type
#define RSI_WLAN_DATA_Q 5

#ifdef FW_LOGGING_ENABLE
#define RSI_SL_LOG_DATA_Q 8
#define SL_LOG_DATA       0x01
#endif

// frame descriptor length
#define RSI_FRAME_DESC_LEN 16

// status offset in frame descriptor
#define RSI_STATUS_OFFSET 12

// response type offset in frame descriptor
#define RSI_RESP_OFFSET 2

#define RSI_TRUE  1
#define RSI_FALSE 0

//SL_PRINTF logging call
#ifndef SL_PRINTF
#define SL_PRINTF(...)
#endif

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// driver control block structure
typedef struct rsi_driver_cb_s {
  // rx packet pool
  rsi_pkt_pool_t rx_pool;

  // driver common control block structure pointer
  rsi_common_cb_t *common_cb;

  // driver common control block TX queue
  rsi_queue_cb_t common_tx_q;

  // driver WLAN  control block structure pointer
  rsi_wlan_cb_t *wlan_cb;

  // driver WLAN control block TX queue
  rsi_queue_cb_t wlan_tx_q;

  // driver scheduler control block
  rsi_scheduler_cb_t scheduler_cb;

  // driver event list structure
#if (defined(RSI_M4_INTERFACE) && !defined(A11_ROM))
  rsi_event_cb_t event_list[RSI_MAX_NUM_EVENTS];
#else
  rsi_event_cb_t *event_list;
#endif
  rsi_queue_cb_t m4_tx_q;

  rsi_queue_cb_t m4_rx_q;

  void (*rsi_tx_done_handler)(struct rsi_pkt_s *);

#ifdef RSI_ZB_ENABLE
  // driver BT/ZB control block structure pointer
  rsi_zigb_cb_t *zigb_cb;

  // driver BT/ZB control block TX queue
  rsi_queue_cb_t zigb_tx_q;
#endif

#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_PROP_PROTOCOL_ENABLE))
  rsi_bt_global_cb_t *bt_global_cb;

  rsi_bt_cb_t *bt_common_cb;
  rsi_queue_cb_t bt_single_tx_q;
  rsi_queue_cb_t bt_common_tx_q;
#endif
#ifdef RSI_BT_ENABLE
  rsi_bt_cb_t *bt_classic_cb;
  rsi_queue_cb_t bt_classic_tx_q;
#endif
#ifdef RSI_BLE_ENABLE
  rsi_bt_cb_t *ble_cb;
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
  rsi_bt_cb_t *prop_protocol_cb;
  rsi_queue_cb_t prop_protocol_tx_q;
#endif
#ifdef SAPIS_BT_STACK_ON_HOST
  rsi_bt_cb_t *bt_ble_stack_cb;
  // driver BT/BLE stack control block TX queue
  rsi_queue_cb_t bt_ble_stack_tx_q;
#endif

// tx packet state in case of UART interface
#ifdef RSI_UART_INTERFACE
  uint8_t tx_pkt_state;
#endif

  void (*unregistered_event_callback)(uint32_t event_num);

#ifdef FW_LOGGING_ENABLE
  sl_fw_log_cb_t *fw_log_cb;
#endif
} rsi_driver_cb_t;

#define RSI_PKT_ALLOC_RESPONSE_WAIT_TIME 600000
#define RSI_PKT_FREE_RESPONSE_WAIT_TIME  600000

extern rsi_driver_cb_t *rsi_driver_cb;
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_send_ps_mode_to_module(uint8_t selected_ps_mode, uint8_t selected_ps_type);
int32_t rsi_driver_process_bt_zb_recv_cmd(rsi_pkt_t *pkt);
void rsi_clear_sockets(int32_t sockID);
void rsi_interrupt_handler(void);
void rsi_mask_ta_interrupt(void);
void rsi_unmask_ta_interrupt(void);
uint32_t rsi_hal_critical_section_entry(void);
void rsi_hal_critical_section_exit(void);

int32_t rsi_driver_process_recv_data(rsi_pkt_t *pkt);
//int32_t rsi_driver_send_data(int32_t sock_no, uint8_t* buffer, uint32_t length, struct rsi_sockaddr *destAddr);
int32_t rsi_send_raw_data(uint8_t *buffer, uint32_t length);
int32_t rsi_ftp_mode_set(uint8_t mode);

#endif