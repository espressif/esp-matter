/*******************************************************************************
* @file  rsi_common_app_DEMO_57.c
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
/**
 * @file    rsi_common_app_DEMO_57.c
 * @version 0.1
 * @date    01 Feb 2020
 *
 *
 *  @section Licenseremote_name
 *  This program should be used on your own responsibility.
 *  Silicon Labs assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains example application for device initialization
 *
 *  @section Description  This application initiates Silabs  device and create tasks.
 *
 */

/*=======================================================================*/
//   ! INCLUDES
/*=======================================================================*/

#include <rsi_common_app.h>
//#if COEX_MAX_APP
#include "rsi_bt_common_apis.h"
#include <stdio.h>
#include <string.h>
#include <rsi_ble.h>
#include "rsi_driver.h"
#include "rsi_common_config.h"

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
//! flag to check bt power save
#if !RUN_TIME_CONFIG_ENABLE
rsi_parsed_conf_t rsi_parsed_conf = { 0 };
#endif
rsi_semaphore_handle_t ble_main_task_sem, ble_slave_conn_sem, bt_app_sem, ant_app_sem, wlan_app_sem, bt_inquiry_sem,
  ble_scan_sem;
#if WLAN_SYNC_REQ
rsi_semaphore_handle_t sync_coex_ble_sem, sync_coex_ant_sem, sync_coex_bt_sem;
bool other_protocol_activity_enabled;
#endif
#if SOCKET_ASYNC_FEATURE
rsi_semaphore_handle_t sock_wait_sem;
#endif
rsi_task_handle_t ble_main_app_task_handle, bt_app_task_handle, ant_app_task_handle, wlan_app_task_handle,
  wlan_task_handle;
rsi_task_handle_t window_reset_notify_task_handle;
bool rsi_ble_running, rsi_bt_running, rsi_ant_running, rsi_wlan_running, wlan_radio_initialized, powersave_cmd_given;
rsi_mutex_handle_t power_cmd_mutex;
rsi_mutex_handle_t window_update_mutex;
bool rsi_window_update_sem_waiting;
rsi_semaphore_handle_t window_reset_notify_sem;
#if (RX_DATA && HTTPS_DOWNLOAD)
rsi_semaphore_handle_t cert_sem, conn_sem;
rsi_task_handle_t cert_bypass_task_handle[SOCKTEST_INSTANCES_MAX];
#endif
#if WLAN_TRANSIENT_CASE
uint32_t disable_factor_count = 0, enable_factor_count = 0;
rsi_semaphore_handle_t wlan_sync_coex_ble_sem, wlan_sync_coex_ant_sem, wlan_sync_coex_bt_sem;
#endif
/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/
extern rsi_task_handle_t common_task_handle;
#if RUN_TIME_CONFIG_ENABLE
extern rsi_semaphore_handle_t common_task_sem;
extern rsi_parsed_conf_t rsi_parsed_conf;
#endif

/*========================================================================*/
//!  CALLBACK FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
int32_t set_power_config(void)
{
  int32_t status = RSI_SUCCESS;

  status = rsi_bt_power_save_profile(RSI_ACTIVE, RSI_MAX_PSP);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to keep in ACTIVE MODE\r\n");
    return status;
  }

  status = rsi_wlan_power_save_profile(RSI_ACTIVE, RSI_MAX_PSP);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to keep in ACTIVE MODE\r\n");
    return status;
  }
  return status;
}

/*==============================================*/
/**
 * @fn         rsi_initiate_power_save
 * @brief      send power save command to RS9116 module
 *
 * @param[out] none
 * @return     status of commands, success-> 0, failure ->-1
 * @section description
 * This function sends command to keep module in power save
 */
int32_t rsi_initiate_power_save(void)
{
  int32_t status = RSI_SUCCESS;
  //! enable wlan radio
  if (!wlan_radio_initialized) {
    status = rsi_wlan_radio_init();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n radio init failed,error = %d\n", status);
      return status;
    } else {
      wlan_radio_initialized = true;
    }
  }
  status = rsi_bt_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed in initiating power save\r\n");
    return status;
  }
#if (WLAN_TRANSIENT_CASE && WLAN_POWER_SAVE_USAGE)
  //! initiating power save in wlan mode
  status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_8, PSP_TYPE);
#else
  //! initiating power save in wlan mode
  status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
#endif
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed in initiating power save\r\n");
    return status;
  }
  return status;
}

#if !RUN_TIME_CONFIG_ENABLE

/**
 * @fn         rsi_ble_initialize_conn_buffer
 * @brief      this function initializes the configurations for each connection
 * @param[out] none
 * @param[out] none
 * @return     none
 * @section description
 */
/*int8_t rsi_ble_initialize_conn_buffer(rsi_ble_conn_config_t *ble_conn_spec_conf)
{
  int8_t status = RSI_SUCCESS;
  if (ble_conn_spec_conf != NULL) {
    if (RSI_BLE_MAX_NBR_SLAVES > 0) {
      //! Initialize slave1 configurations
      ble_conn_spec_conf[SLAVE1].smp_enable        = SMP_ENABLE_S1;
      ble_conn_spec_conf[SLAVE1].add_to_whitelist  = ADD_TO_WHITELIST_S1;
      ble_conn_spec_conf[SLAVE1].profile_discovery = PROFILE_QUERY_S1;
      ble_conn_spec_conf[SLAVE1].data_transfer     = DATA_TRANSFER_S1;
      //ble_conn_spec_conf[SLAVE1].bidir_datatransfer = SMP_ENABLE_S1;
      ble_conn_spec_conf[SLAVE1].rx_notifications                 = RX_NOTIFICATIONS_FROM_S1;
      ble_conn_spec_conf[SLAVE1].rx_indications                   = RX_INDICATIONS_FROM_S1;
      ble_conn_spec_conf[SLAVE1].tx_notifications                 = TX_NOTIFICATIONS_TO_S1;
      ble_conn_spec_conf[SLAVE1].tx_write                         = TX_WRITES_TO_S1;
      ble_conn_spec_conf[SLAVE1].tx_write_no_response             = TX_WRITES_NO_RESP_TO_S1;
      ble_conn_spec_conf[SLAVE1].tx_indications                   = TX_INDICATIONS_TO_S1;
      ble_conn_spec_conf[SLAVE1].conn_param_update.conn_int       = CONN_INTERVAL_S1;
      ble_conn_spec_conf[SLAVE1].conn_param_update.conn_latncy    = CONN_LATENCY_S1;
      ble_conn_spec_conf[SLAVE1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_S1;
      ble_conn_spec_conf[SLAVE1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_S1;
      ble_conn_spec_conf[SLAVE1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_S1;
      ble_conn_spec_conf[SLAVE1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_S1;
    }

    if (RSI_BLE_MAX_NBR_SLAVES > 1) {
      //! Initialize slave2 configurations
      ble_conn_spec_conf[SLAVE2].smp_enable        = SMP_ENABLE_S2;
      ble_conn_spec_conf[SLAVE2].add_to_whitelist  = ADD_TO_WHITELIST_S2;
      ble_conn_spec_conf[SLAVE2].profile_discovery = PROFILE_QUERY_S2;
      ble_conn_spec_conf[SLAVE2].data_transfer     = DATA_TRANSFER_S2;
      //ble_conn_spec_conf[SLAVE2].bidir_datatransfer = SMP_ENABLE_S2;
      ble_conn_spec_conf[SLAVE2].rx_notifications                 = RX_NOTIFICATIONS_FROM_S2;
      ble_conn_spec_conf[SLAVE2].rx_indications                   = RX_INDICATIONS_FROM_S2;
      ble_conn_spec_conf[SLAVE2].tx_notifications                 = TX_NOTIFICATIONS_TO_S2;
      ble_conn_spec_conf[SLAVE2].tx_write                         = TX_WRITES_TO_S2;
      ble_conn_spec_conf[SLAVE2].tx_write_no_response             = TX_WRITES_NO_RESP_TO_S2;
      ble_conn_spec_conf[SLAVE2].tx_indications                   = TX_INDICATIONS_TO_S2;
      ble_conn_spec_conf[SLAVE2].conn_param_update.conn_int       = CONN_INTERVAL_S2;
      ble_conn_spec_conf[SLAVE2].conn_param_update.conn_latncy    = CONN_LATENCY_S2;
      ble_conn_spec_conf[SLAVE2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_S2;
      ble_conn_spec_conf[SLAVE2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_S2;
      ble_conn_spec_conf[SLAVE2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_S2;
      ble_conn_spec_conf[SLAVE2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_S2;
    }

    if (RSI_BLE_MAX_NBR_SLAVES > 2) {
      //! Initialize SLAVE3 configurations
      ble_conn_spec_conf[SLAVE3].smp_enable        = SMP_ENABLE_S3;
      ble_conn_spec_conf[SLAVE3].add_to_whitelist  = ADD_TO_WHITELIST_S3;
      ble_conn_spec_conf[SLAVE3].profile_discovery = PROFILE_QUERY_S3;
      ble_conn_spec_conf[SLAVE3].data_transfer     = DATA_TRANSFER_S3;
      //ble_conn_spec_conf[SLAVE3].bidir_datatransfer = SMP_ENABLE_S3;
      ble_conn_spec_conf[SLAVE3].rx_notifications                 = RX_NOTIFICATIONS_FROM_S3;
      ble_conn_spec_conf[SLAVE3].rx_indications                   = RX_INDICATIONS_FROM_S3;
      ble_conn_spec_conf[SLAVE3].tx_notifications                 = TX_NOTIFICATIONS_TO_S3;
      ble_conn_spec_conf[SLAVE3].tx_write                         = TX_WRITES_TO_S3;
      ble_conn_spec_conf[SLAVE3].tx_write_no_response             = TX_WRITES_NO_RESP_TO_S3;
      ble_conn_spec_conf[SLAVE3].tx_indications                   = TX_INDICATIONS_TO_S3;
      ble_conn_spec_conf[SLAVE3].conn_param_update.conn_int       = CONN_INTERVAL_S3;
      ble_conn_spec_conf[SLAVE3].conn_param_update.conn_latncy    = CONN_LATENCY_S3;
      ble_conn_spec_conf[SLAVE3].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_S3;
      ble_conn_spec_conf[SLAVE3].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_S3;
      ble_conn_spec_conf[SLAVE3].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_S3;
      ble_conn_spec_conf[SLAVE3].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_S3;
    }

    if (RSI_BLE_MAX_NBR_MASTERS > 0) {
      //! Initialize master1 configurations
      ble_conn_spec_conf[MASTER1].smp_enable        = SMP_ENABLE_M1;
      ble_conn_spec_conf[MASTER1].add_to_whitelist  = ADD_TO_WHITELIST_M1;
      ble_conn_spec_conf[MASTER1].profile_discovery = PROFILE_QUERY_M1;
      ble_conn_spec_conf[MASTER1].data_transfer     = DATA_TRANSFER_M1;
      //ble_conn_spec_conf[MASTER1].bidir_datatransfer = SMP_ENABLE_M1;
      ble_conn_spec_conf[MASTER1].rx_notifications                 = RX_NOTIFICATIONS_FROM_M1;
      ble_conn_spec_conf[MASTER1].rx_indications                   = RX_INDICATIONS_FROM_M1;
      ble_conn_spec_conf[MASTER1].tx_notifications                 = TX_NOTIFICATIONS_TO_M1;
      ble_conn_spec_conf[MASTER1].tx_write                         = TX_WRITES_TO_M1;
      ble_conn_spec_conf[MASTER1].tx_write_no_response             = TX_WRITES_NO_RESP_TO_M1;
      ble_conn_spec_conf[MASTER1].tx_indications                   = TX_INDICATIONS_TO_M1;
      ble_conn_spec_conf[MASTER1].conn_param_update.conn_int       = CONN_INTERVAL_M1;
      ble_conn_spec_conf[MASTER1].conn_param_update.conn_latncy    = CONN_LATENCY_M1;
      ble_conn_spec_conf[MASTER1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_M1;
      ble_conn_spec_conf[MASTER1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_M1;
      ble_conn_spec_conf[MASTER1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_M1;
      ble_conn_spec_conf[MASTER1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_M1;
    }

    if (RSI_BLE_MAX_NBR_MASTERS > 1) {
      //! Initialize master2 configurations
      ble_conn_spec_conf[MASTER2].smp_enable        = SMP_ENABLE_M2;
      ble_conn_spec_conf[MASTER2].add_to_whitelist  = ADD_TO_WHITELIST_M2;
      ble_conn_spec_conf[MASTER2].profile_discovery = PROFILE_QUERY_M2;
      ble_conn_spec_conf[MASTER2].data_transfer     = DATA_TRANSFER_M2;
      //ble_conn_spec_conf[MASTER2].bidir_datatransfer = SMP_ENABLE_M2;
      ble_conn_spec_conf[MASTER2].rx_notifications                 = RX_NOTIFICATIONS_FROM_M2;
      ble_conn_spec_conf[MASTER2].rx_indications                   = RX_INDICATIONS_FROM_M2;
      ble_conn_spec_conf[MASTER2].tx_notifications                 = TX_NOTIFICATIONS_TO_M2;
      ble_conn_spec_conf[MASTER2].tx_write                         = TX_WRITES_TO_M2;
      ble_conn_spec_conf[MASTER2].tx_write_no_response             = TX_WRITES_NO_RESP_TO_M2;
      ble_conn_spec_conf[MASTER2].tx_indications                   = TX_INDICATIONS_TO_M2;
      ble_conn_spec_conf[MASTER2].conn_param_update.conn_int       = CONN_INTERVAL_M2;
      ble_conn_spec_conf[MASTER2].conn_param_update.conn_latncy    = CONN_LATENCY_M2;
      ble_conn_spec_conf[MASTER2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_M2;
      ble_conn_spec_conf[MASTER2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_M2;
      ble_conn_spec_conf[MASTER2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_M2;
      ble_conn_spec_conf[MASTER2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_M2;
    }

    // Check the Total Number of Buffers allocated.
    if ((DLE_BUFFER_COUNT_S1 + DLE_BUFFER_COUNT_S2 + DLE_BUFFER_COUNT_S3 + DLE_BUFFER_COUNT_M1 + DLE_BUFFER_COUNT_M2)
        > RSI_BLE_NUM_CONN_EVENTS) {
      LOG_PRINT("\r\n Total number of per connection buffer count is more than the total number alllocated \r\n");
      status = RSI_FAILURE;
    }

  } else {
    LOG_PRINT("\r\n Invalid buffer passed \r\n");
    status = RSI_FAILURE;
  }
  return status;
}*/

/*==============================================*/
/**
 * @fn         rsi_fill_user_config
 * @brief      this function fills the compile time user inputs to local buffer
 * @param[out] none
 * @return     none.
 * @section description
 * this function fills the compile time userinputs to local buffer
 */
int8_t rsi_fill_user_config()
{
  int8_t status = RSI_SUCCESS;
  //! copy protocol selection macros
  //  rsi_parsed_conf.rsi_protocol_sel.is_ble_enabled  = RSI_ENABLE_BLE_TEST;
  rsi_parsed_conf.rsi_protocol_sel.is_bt_enabled   = RSI_ENABLE_BT_TEST;
  rsi_parsed_conf.rsi_protocol_sel.is_ant_enabled  = RSI_ENABLE_ANT_TEST;
  rsi_parsed_conf.rsi_protocol_sel.is_wifi_enabled = RSI_ENABLE_WIFI_TEST;

  //! copy ble connection specific configurations

  if ((RSI_BLE_MAX_NBR_MASTERS > 2) || (RSI_BLE_MAX_NBR_SLAVES > 3)) {
    LOG_PRINT("\r\n number of BLE MASTERS or BLE SLAVES Given wrong declaration\r\n");
    rsi_delay_ms(1000);
    return RSI_FAILURE;
  }

  /* if (rsi_parsed_conf.rsi_protocol_sel.is_ble_enabled) {
    status =
      rsi_ble_initialize_conn_buffer((rsi_ble_conn_config_t *)&rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config);
  }*/

  //! copy bt configurations
  if (rsi_parsed_conf.rsi_protocol_sel.is_bt_enabled) {
    rsi_parsed_conf.rsi_bt_config.rsi_bd_addr               = malloc(sizeof(uint8_t) * RSI_REM_DEV_ADDR_LEN);
    rsi_parsed_conf.rsi_bt_config.rsi_bd_addr               = RSI_BT_REMOTE_BD_ADDR;
    rsi_parsed_conf.rsi_bt_config.rsi_app_avdtp_role        = RSI_APP_AVDTP_ROLE;
    rsi_parsed_conf.rsi_bt_config.rsi_bt_avdtp_stats_enable = RSI_BT_AVDTP_STATS;
    rsi_parsed_conf.rsi_bt_config.rsi_ta_based_encoder      = TA_BASED_ENCODER;
    rsi_parsed_conf.rsi_bt_config.rsi_bt_inquiry_enable     = INQUIRY_ENABLE;
    rsi_parsed_conf.rsi_bt_config.rsi_inq_conn_simultaneous = INQUIRY_CONNECTION_SIMULTANEOUS;
  }
  return status;
}
#endif

/*==============================================*/
/**
 * @fn         rsi_common_app_task
 * @brief      This function creates the main tasks for selected protocol
 * @param[out] none
 * @return     none.
 * @section description
 * This function creates the main tasks for enabled protocols based on local buffer 'rsi_parsed_conf'
 */
#if (PER_TEST_TX_ENABLE || PER_TEST_RX_ENABLE)
//! Variable used to stop receiving stats
volatile uint8_t stop_receiving_stats = 0;
//! Variable used to initialise stats received
uint32_t stats_received = 0;

//! wlan per stats structure
typedef struct rsi_per_stats_rsp_s {
  uint8_t tx_pkts[2];
  uint8_t reserved_1[2];
  uint8_t tx_retries[2];
  uint16_t crc_pass;
  uint16_t crc_fail;
  uint8_t cca_stk[2];
  uint8_t cca_not_stk[2];
  uint8_t pkt_abort[2];
  uint8_t fls_rx_start[2];
  uint8_t cca_idle[2];
  uint8_t reserved_2[26];
  uint8_t rx_retries[2];
  uint8_t reserved_3[2];
  uint16_t cal_rssi;
  uint8_t reserved_4[4];
  uint8_t xretries[2];
  uint8_t max_cons_pkts_dropped[2];
  uint8_t reserved_5[2];
  uint8_t bss_broadcast_pkts[2];
  uint8_t bss_multicast_pkts[2];
  uint8_t bss_filter_matched_multicast_pkts[2];
} rsi_per_stats_rsp_t;

//! call back to receive RX Stats data
void rsi_wlan_stats_receive_handler(uint16_t status, const uint8_t *buffer, const uint16_t length)
{
  rsi_per_stats_rsp_t *per_stats_p = (rsi_per_stats_rsp_t *)buffer;
  stats_received++;
  LOG_PRINT("\n Stats received \n");
}
#endif

void rsi_common_app_task(void)
{
  int8_t status                 = RSI_SUCCESS;
  rsi_ble_running               = false;
  rsi_bt_running                = false;
  rsi_ant_running               = false;
  rsi_wlan_running              = false;
  wlan_radio_initialized        = false;
  powersave_cmd_given           = false;
  ble_main_app_task_handle      = NULL;
  bt_app_task_handle            = NULL;
  ant_app_task_handle           = NULL;
  wlan_app_task_handle          = NULL;
  rsi_window_update_sem_waiting = false;
  uint8_t own_bd_addr[6]        = { 0x66, 0x55, 0x44, 0x33, 0x22, 0x11 };

  while (1) {
#if RUN_TIME_CONFIG_ENABLE
    //! wait untill completion of parsing input file
    rsi_semaphore_wait(&common_task_sem, 0);
#endif

    //! Silabs module initialization
    status = rsi_device_init(LOAD_NWP_FW);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n device init failed \n");
      return;
    }
#if !RUN_TIME_CONFIG_ENABLE
    //! fill the configurations in local structure based on compilation macros
    status = rsi_fill_user_config();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n failed to fill the configurations in local buffer \r\n");
      return;
    }
#endif
    //! WiSeConnect initialization
#if (PER_TEST_TX_ENABLE || PER_TEST_RX_ENABLE)
    status = rsi_wireless_init(RSI_WLAN_TRANSMIT_TEST_MODE, RSI_WLAN_CLIENT_MODE);
#else
    status = rsi_wireless_init(RSI_WLAN_MODE, RSI_COEX_MODE);
#endif
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n wireless init failed \n");
      return;
    }
#if !(PER_TEST_TX_ENABLE || PER_TEST_RX_ENABLE)
    status = rsi_bt_set_bd_addr(own_bd_addr);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\nSET BD ADDR FAILED : 0x%x\n", status);
    } else {
      LOG_PRINT("\nSET BD ADDR SUCCESS : 0x%x\n", status);
    }
#endif
    //! Send Feature frame
    status = rsi_send_feature_frame();
    if (status != RSI_SUCCESS) {
      return;
    }
    //! initialize wlan radio
    status = rsi_wlan_radio_init();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n WLAN radio init failed \n");
      return;
    } else {
      wlan_radio_initialized = true;
    }
    LOG_PRINT("\n Basic Init Done i.e deviceinit=>wirelessinit=>feature=>wlanradioinit \n");

#if (PER_TEST_TX_ENABLE || PER_TEST_RX_ENABLE)
    //! To selct Internal antenna or uFL connector
    status = rsi_wireless_antenna(RSI_ANTENNA, RSI_ANTENNA_GAIN_2G, RSI_ANTENNA_GAIN_5G);
    if (status != RSI_SUCCESS) {
      return;
    }
#endif

#if PER_TEST_TX_ENABLE
    //! transmit test start
    status = rsi_transmit_test_start(RSI_TX_TEST_POWER,
                                     RSI_TX_TEST_RATE,
                                     RSI_TX_TEST_LENGTH,
                                     RSI_TX_TEST_MODE,
                                     RSI_TX_TEST_CHANNEL);
    if (status != RSI_SUCCESS) {
      return;
    } else {
      LOG_PRINT("\nPER TRANSMIT START SUCCESS");
    }
    rsi_delay_ms(PER_TX_TIMEOUT);
    //! Stop TX transmit
    status = rsi_transmit_test_stop();
    if (status != RSI_SUCCESS) {
      return;
    }
    while (1)
      ;
#endif

#if PER_TEST_RX_ENABLE
    //! Register Wlan receive stats call back handler
    rsi_wlan_register_callbacks(RSI_WLAN_RECEIVE_STATS_RESPONSE_CB, rsi_wlan_stats_receive_handler);
    //! Start/Receive publishing RX stats
    status = rsi_wlan_receive_stats_start(RSI_TX_TEST_CHANNEL);
    if (status != RSI_SUCCESS) {
      return;
    } else {
      LOG_PRINT("\nPER RECEIVE START SUCCESS");
    }
    rsi_delay_ms(PER_RX_TIMEOUT);
    //! Stop Receiving RX stats
    status = rsi_wlan_receive_stats_stop();
    if (status != RSI_SUCCESS) {
      return;
    }
    while (1)
      ;
#endif

#if ENABLE_POWER_SAVE
    //! create mutex
    status = rsi_mutex_create(&power_cmd_mutex);
    if (status != RSI_ERROR_NONE) {
      LOG_PRINT("failed to create mutex object, error = %d \r\n", status);
      return;
    }
#endif

    if (rsi_parsed_conf.rsi_protocol_sel.is_wifi_enabled) {
      rsi_wlan_running = true;                //! Making sure wlan got triggered.
      rsi_semaphore_create(&wlan_app_sem, 0); //! This lock will be used from one download complete notification.
#if (RX_DATA && HTTPS_DOWNLOAD)
      rsi_semaphore_create(&cert_sem, 0);
      rsi_semaphore_create(&conn_sem, 0);
      rsi_semaphore_post(&conn_sem);
#endif
#if WLAN_SYNC_REQ
      rsi_semaphore_create(&sync_coex_bt_sem, 0); //! This lock will be used from wlan task to be done.
      rsi_semaphore_create(&sync_coex_ant_sem, 0);
      rsi_semaphore_create(&sync_coex_ble_sem, 0);
#endif
#if WLAN_TRANSIENT_CASE
      rsi_semaphore_create(&wlan_sync_coex_bt_sem, 0);
      rsi_semaphore_create(&wlan_sync_coex_ant_sem, 0);
      rsi_semaphore_create(&wlan_sync_coex_ble_sem, 0);
#endif
#if SOCKET_ASYNC_FEATURE
      rsi_semaphore_create(&sock_wait_sem, 0);
#endif

#if WLAN_STA_TX_CASE
      status = rsi_task_create((void *)rsi_app_task_wifi_tcp_tx_ps,
                               (uint8_t *)"wlan_task",
                               RSI_WLAN_TASK_STACK_SIZE,
                               NULL,
                               RSI_WLAN_APP_TASK_PRIORITY,
                               &wlan_task_handle);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("\r\n rrsi_app_task_wifi_tcp_tx_ps failed to create \r\n");
        break;
      }
#else
      status = rsi_task_create((void *)rsi_wlan_app_task,
                               (uint8_t *)"wlan_task",
                               RSI_WLAN_APP_TASK_SIZE,
                               NULL,
                               RSI_WLAN_APP_TASK_PRIORITY,
                               &wlan_app_task_handle);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("\r\n rsi_wlan_app_task failed to create \r\n");
        break;
      }
#endif
#if WINDOW_UPDATE_FEATURE
      rsi_semaphore_create(&window_reset_notify_sem, 0);
      status = rsi_task_create((void *)rsi_window_reset_notify_app_task,
                               (uint8_t *)"window_reset_notify_task",
                               RSI_WINDOW_RESET_NOTIFY_TASK_STACK_SIZE,
                               NULL,
                               RSI_WINDOW_RESET_NOTIFY_TASK_PRIORITY,
                               &window_reset_notify_task_handle);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("\r\n rsi_window_reset_notify_app_task failed to create \r\n");
        break;
      }
      status = rsi_mutex_create(&window_update_mutex);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("failed to create mutex object, error = %d \r\n", status);
        return;
      }
#endif
    }
    //! create ble main task if ble protocol is selected
    /* if (rsi_parsed_conf.rsi_protocol_sel.is_ble_enabled) {
      rsi_ble_running = 1;
      rsi_semaphore_create(&ble_main_task_sem, 0);
      rsi_semaphore_create(&ble_scan_sem, 0);
      if (RSI_BLE_MAX_NBR_SLAVES > 0) {
        rsi_semaphore_create(&ble_slave_conn_sem, 0);
      }
      status = rsi_task_create((void *)rsi_ble_main_app_task,
                               (uint8_t *)"ble_main_task",
                               RSI_BLE_APP_MAIN_TASK_SIZE,
                               NULL,
                               RSI_BLE_MAIN_TASK_PRIORITY,
                               &ble_main_app_task_handle);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("\r\n ble main task failed to create \r\n");
        return;
      }
    }*/

    //! create bt task if bt protocol is selected
    if (rsi_parsed_conf.rsi_protocol_sel.is_bt_enabled) {
      rsi_bt_running = 1;
      rsi_semaphore_create(&bt_app_sem, 0);
      rsi_semaphore_create(&bt_inquiry_sem, 0);
      status = rsi_task_create((void *)rsi_bt_app_task,
                               (uint8_t *)"bt_task",
                               RSI_BT_APP_TASK_SIZE + RSI_SBC_APP_ENCODE_SIZE,
                               NULL,
                               RSI_BT_APP_TASK_PRIORITY,
                               &bt_app_task_handle);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("\r\n rsi_bt_app_task failed to create \r\n");
        return;
      }
    }

    //! create ant task if ant protocol is selected
    /*   if (rsi_parsed_conf.rsi_protocol_sel.is_ant_enabled) { // To remove errors
      rsi_ant_running = 1;
      rsi_semaphore_create(&ant_app_sem, 0);
      status = rsi_task_create((void *)rsi_ant_app_task,
                               (uint8_t *)"ant_task",
                               RSI_ANT_APP_TASK_SIZE,
                               NULL,
                               RSI_ANT_APP_TASK_PRIORITY,
                               &ant_app_task_handle);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("\r\n rsi_ant_app_task failed to create \r\n");
        return;
      }
    }*/

    //! delete the task as initialization is completed
    rsi_task_destroy(common_task_handle);
  }
}
//#endif
