/*******************************************************************************
* @file  rsi_ble_privacy.c
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
/*************************************************************************
 *
 */

/*================================================================================
 * @brief : This file contains example application for Bluetooth LE Privacy
 * @section Description :
 * This application demonstrates how to configure device with privacy feature by
 * organizing resolving list and resolution process and how to connect to remote
 * Peripheral device.
 =================================================================================*/

/**
 * Include files
 * */

//! BLE include file to refer BLE APIs
#include <rsi_ble_apis.h>
#include <rsi_ble_config.h>
#include <rsi_bt_common_apis.h>
#include <rsi_bt_common.h>
#include <rsi_ble.h>
#include "rsi_driver.h"
//! Common include file
#include <rsi_common_apis.h>
#include <stdio.h>
#include <string.h>

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif
#ifdef FW_LOGGING_ENABLE
//! Firmware logging includes
#include "sl_fw_logging.h"
#endif

//! Remote Device Name to connect
#define RSI_REMOTE_DEVICE_NAME "BLE_SIMPLE_PRIVACY"

#ifdef FW_LOGGING_ENABLE
//! Memory length of driver updated for firmware logging
#define BT_GLOBAL_BUFF_LEN (15000 + (FW_LOG_QUEUE_SIZE * MAX_FW_LOG_MSG_LEN))
#else
//! Memory length for the driver
#define BT_GLOBAL_BUFF_LEN 15000
#endif

//! Memory to initialize driver
uint8_t global_buf[BT_GLOBAL_BUFF_LEN];

#define RSI_SLAVE  0
#define RSI_MASTER 1

//!local device role
#define RSI_DEVICE_ROLE RSI_SLAVE

//! local device name
#define RSI_BLE_DEVICE_NAME "SIMPLE_PRIVACY"

//! local device IO capability
#define RSI_BLE_SMP_IO_CAPABILITY 0x00
#define RSI_BLE_SMP_PASSKEY       0

//! application event list
#define RSI_APP_EVENT_ADV_REPORT            0x00
#define RSI_BLE_CONN_EVENT                  0x01
#define RSI_BLE_DISCONN_EVENT               0x02
#define RSI_BLE_SMP_REQ_EVENT               0x03
#define RSI_BLE_SMP_RESP_EVENT              0x04
#define RSI_BLE_SMP_PASSKEY_EVENT           0x05
#define RSI_BLE_SMP_FAILED_EVENT            0x06
#define RSI_BLE_ENCRYPT_STARTED_EVENT       0x07
#define RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT   0x08
#define RSI_BLE_SC_PASSKEY_EVENT            0x09
#define RSI_BLE_LTK_REQ_EVENT               0x0A
#define RSI_BLE_SECURITY_KEYS_EVENT         0x0B
#define RSI_BLE_ENHANCE_CONNECTED_EVENT     0x0C
#define RSI_APP_EVENT_DATA_LENGTH_CHANGE    0x0D
#define RSI_APP_EVENT_PHY_UPDATE_COMPLETE   0x0E
#define RSI_APP_EVENT_CONN_UPDATE_COMPLETE  0x0F
#define RSI_BLE_RECEIVE_REMOTE_FEATURES     0x10
#define RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ 0x11

//! Address type of the device to connect
#define RSI_BLE_REMOTE_ADDR_TYPE LE_PUBLIC_ADDRESS

//! Address of the device to connect
#define RSI_BLE_REMOTE_ADDR "00:23:A7:56:77:77"

//!address resolution enable
#define RSI_BLE_SET_RESOLVABLE_PRIV_ADDR_TOUT 120

//! process type 1-add device to resolvlist, 2-remove device from resolvlist, 3-clear the resolvlist
#define RSI_BLE_ADD_TO_RESOLVE_LIST      1
#define RSI_BLE_REMOVE_FROM_RESOLVE_LIST 2
#define RSI_BLE_CLEAR_RESOLVE_LIST       3

//! privacy mode 0-Network privacy mode 1-Device privacy mode
#define RSI_BLE_NETWORK_PRIVACY_MODE 0
#define RSI_BLE_DEVICE_PRIVACY_MODE  1

#define RSI_BLE_PRIVACY_MODE RSI_BLE_DEVICE_PRIVACY_MODE

#define RSI_BLE_RESOLVING_LIST_SIZE 5

#ifdef RSI_WITH_OS
//! BLE task stack size
#define RSI_BT_TASK_STACK_SIZE 1000

//! BT task priority
#define RSI_BT_TASK_PRIORITY 1

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 3000

#ifdef FW_LOGGING_ENABLE
/*=======================================================================*/
//!    Firmware logging configurations
/*=======================================================================*/
//! Firmware logging task defines
#define RSI_FW_TASK_STACK_SIZE (512 * 2)
#define RSI_FW_TASK_PRIORITY   2
//! Firmware logging variables
extern rsi_semaphore_handle_t fw_log_app_sem;
rsi_task_handle_t fw_log_task_handle = NULL;
//! Firmware logging prototypes
void sl_fw_log_callback(uint8_t *log_message, uint16_t log_message_length);
void sl_fw_log_task(void);
#endif

void rsi_wireless_driver_task(void);

#endif

#define MITM_REQ          0x01
#define RSI_MAX_LIST_SIZE 0x05

//! user defined structure
//LE resolvlist group.
typedef struct rsi_ble_resolvlist_group_s {
  uint8_t used;
  uint8_t remote_dev_addr_type;
  uint8_t remote_dev_addr[18];
  uint8_t peer_irk[16];
  uint8_t local_irk[16];
  uint8_t Identity_addr_type;
  uint8_t Identity_addr[18];
} rsi_ble_resolvlist_group_t;

//LE resolvlist.
typedef struct rsi_ble_resolve_key_s {
  uint8_t remote_dev_addr_type;
  uint8_t remote_dev_addr[18];
  uint8_t peer_irk[16];
  uint8_t local_irk[16];
  uint16_t remote_ediv;
  uint8_t remote_rand[16];
  uint8_t remote_ltk[16];
  uint8_t Identity_addr_type;
  uint8_t Identity_addr[18];
} rsi_ble_resolve_key_t;

//LE resolvlist.
typedef struct rsi_ble_dev_ltk_list_s {
  uint8_t enc_enable;
  uint8_t sc_enable;
  uint8_t remote_dev_addr_type;
  uint8_t remote_dev_addr[6];
  uint8_t peer_irk[16];
  uint8_t local_irk[16];
  uint16_t remote_ediv;
  uint16_t local_ediv;
  uint8_t remote_rand[8];
  uint8_t localrand[8];
  uint8_t remote_ltk[16];
  uint8_t localltk[16];
  uint8_t Identity_addr_type;
  uint8_t Identity_addr[6];
  uint8_t used;
} rsi_ble_dev_ltk_list_t;

//! global parameters list
static uint32_t ble_app_event_map;
static uint32_t passkey;
static uint8_t dev_address_type;
static uint8_t str_remote_address[18];
static uint8_t remote_addr_type = 0;
static uint8_t remote_name[31];
static uint8_t remote_dev_addr[18]          = { 0 };
static uint8_t remote_dev_bd_addr[6]        = { 0 };
static uint8_t device_found                 = 0;
static uint8_t rsi_app_resp_resolvlist_size = 0;

static rsi_bt_event_le_ltk_request_t temp_le_ltk_req;
static rsi_bt_event_le_security_keys_t temp_le_sec_keys;
static rsi_ble_event_enhance_conn_status_t rsi_app_enhance_connected_device = { 0 };
static rsi_ble_resolvlist_group_t resolvlist[5];
static rsi_ble_resolve_key_t resolve_key;
rsi_ble_dev_ltk_list_t ble_dev_ltk_list[RSI_MAX_LIST_SIZE];

static rsi_ble_event_remote_features_t remote_dev_feature;
static rsi_ble_event_phy_update_t rsi_app_phy_update_complete;
static rsi_ble_event_remote_conn_param_req_t rsi_app_remote_device_conn_params;
static rsi_ble_event_conn_status_t rsi_app_connected_device;
uint8_t str_remote_dev_address[18] = { '\0' };
rsi_semaphore_handle_t ble_main_task_sem;
static uint32_t ble_app_event_map;
static uint32_t ble_app_event_map1;

int8_t add_device_to_ltk_key_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list,
                                  rsi_bt_event_encryption_enabled_t *enc_enabled);
int8_t add_security_keys_to_device_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list,
                                        rsi_bt_event_le_security_keys_t *le_sec_keys);
int8_t rsi_get_ltk_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list, rsi_bt_event_le_ltk_request_t *le_ltk_req);

/*==============================================*/
/**
 * @fn         rsi_ble_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BLE initialization.
 */
static void rsi_ble_app_init_events()
{
  ble_app_event_map  = 0;
  ble_app_event_map1 = 0;
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_event
 * @brief      set the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
void rsi_ble_app_set_event(uint32_t event_num)
{

  if (event_num < 32) {
    ble_app_event_map |= BIT(event_num);
  } else {
    ble_app_event_map1 |= BIT((event_num - 32));
  }
  rsi_semaphore_post(&ble_main_task_sem);

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_ble_app_clear_event(uint32_t event_num)
{

  if (event_num < 32) {
    ble_app_event_map &= ~BIT(event_num);
  } else {
    ble_app_event_map1 &= ~BIT((event_num - 32));
  }

  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_ble_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 64; ix++) {
    if (ix < 32) {
      if (ble_app_event_map & (1 << ix)) {
        return ix;
      }
    } else {
      if (ble_app_event_map1 & (1 << (ix - 32))) {
        return ix;
      }
    }
  }

  return (-1);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_central_on_adv_report_event
 * @brief      invoked when advertise report event is received
 * @param[in]  adv_report, pointer to the received advertising report
 * @return     none.
 * @section description
 * This callback function updates the scanned remote devices list
 */
void rsi_ble_simple_central_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report)
{

  if (device_found == 1) {
    return;
  }

  memset(remote_name, 0, 31);
  BT_LE_ADPacketExtract(remote_name, adv_report->adv_data, adv_report->adv_data_len);

  remote_addr_type = adv_report->dev_addr_type;
  rsi_6byte_dev_address_to_ascii(remote_dev_addr, (uint8_t *)adv_report->dev_addr);
  memcpy(remote_dev_bd_addr, (uint8_t *)adv_report->dev_addr, 6);
  if (((device_found == 0) && ((strcmp((const char *)remote_name, RSI_REMOTE_DEVICE_NAME)) == 0))
      || ((remote_addr_type == RSI_BLE_REMOTE_ADDR_TYPE)
          && ((strcmp((const char *)remote_dev_addr, RSI_BLE_REMOTE_ADDR) == 0)))) {
    device_found = 1;
    rsi_ble_app_set_event(RSI_APP_EVENT_ADV_REPORT);
  }
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_connect_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
static void rsi_ble_on_connect_event(rsi_ble_event_conn_status_t *resp_conn)
{
  dev_address_type = resp_conn->dev_addr_type;
  memcpy(remote_dev_bd_addr, resp_conn->dev_addr, 6);
  LOG_PRINT("connect - str_remote_address : %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, resp_conn->dev_addr));

  rsi_ble_app_set_event(RSI_BLE_CONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
static void rsi_ble_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  UNUSED_PARAMETER(reason); //This statement is added only to resolve compilation warning, value is unchanged
  memcpy(remote_dev_bd_addr, resp_disconnect->dev_addr, 6);
  LOG_PRINT("connect - str_remote_address : %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, resp_disconnect->dev_addr));

  rsi_ble_app_set_event(RSI_BLE_DISCONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_request 
 * @brief      its invoked when smp request event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP request events is received(we are in Master mode)
 * Note: slave requested to start SMP request, we have to send SMP request command
 */
void rsi_ble_on_smp_request(rsi_bt_event_smp_req_t *remote_dev_address)
{
  memcpy(remote_dev_bd_addr, remote_dev_address->dev_addr, 6);
  LOG_PRINT("smp_req - str_remote_address : %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, remote_dev_address->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_SMP_REQ_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_response 
 * @brief      its invoked when smp response event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP response events is received(we are in slave mode) 
 * Note: Master initiated SMP protocol, we have to send SMP response command
 */
void rsi_ble_on_smp_response(rsi_bt_event_smp_resp_t *remote_dev_address)
{
  memcpy(remote_dev_bd_addr, remote_dev_address->dev_addr, 6);
  LOG_PRINT("smp_resp - str_remote_address : %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, remote_dev_address->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_SMP_RESP_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_passkey 
 * @brief      its invoked when smp passkey event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP passkey events is received
 * Note: We have to send SMP passkey command
 */
void rsi_ble_on_smp_passkey(rsi_bt_event_smp_passkey_t *remote_dev_address)
{
  memcpy(remote_dev_bd_addr, remote_dev_address->dev_addr, 6);
  LOG_PRINT("smp_passkey - str_remote_address : %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, remote_dev_address->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_SMP_PASSKEY_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_passkey_display 
 * @brief      its invoked when smp passkey event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP passkey events is received
 * Note: We have to send SMP passkey command
 */
void rsi_ble_on_smp_passkey_display(rsi_bt_event_smp_passkey_display_t *smp_passkey_display)
{
  memcpy(remote_dev_bd_addr, smp_passkey_display->dev_addr, 6);
  LOG_PRINT("remote addr: %s, passkey: %s \r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, smp_passkey_display->dev_addr),
            smp_passkey_display->passkey);
  rsi_ble_app_set_event(RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT);
}
/*==============================================*/
/**
 * @fn         rsi_ble_on_sc_passkey 
 * @brief      its invoked when smp passkey event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP passkey events is received
 * Note: We have to send SMP passkey command
 */
void rsi_ble_on_sc_passkey(rsi_bt_event_sc_passkey_t *sc_passkey)
{
  memcpy(remote_dev_bd_addr, sc_passkey->dev_addr, 6);
  LOG_PRINT("remote addr: %s, passkey: %06ld \r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, sc_passkey->dev_addr),
            sc_passkey->passkey);
  passkey = sc_passkey->passkey;
  rsi_ble_app_set_event(RSI_BLE_SC_PASSKEY_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_smp_failed 
 * @brief      its invoked when smp failed event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when SMP failed events is received
 */
void rsi_ble_on_smp_failed(uint16_t status, rsi_bt_event_smp_failed_t *remote_dev_address)
{
  memcpy(remote_dev_bd_addr, remote_dev_address->dev_addr, 6);
  LOG_PRINT("smp_failed status: 0x%x, str_remote_address: %s\r\n",
            status,
            rsi_6byte_dev_address_to_ascii(str_remote_address, remote_dev_address->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_SMP_FAILED_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_le_ltk_req_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
static void rsi_ble_on_le_ltk_req_event(rsi_bt_event_le_ltk_request_t *le_ltk_req)
{
  LOG_PRINT(" \r\n rsi_ble_on_le_ltk_req_event \r\n");
  memcpy(&temp_le_ltk_req, le_ltk_req, sizeof(rsi_bt_event_le_ltk_request_t));
  rsi_ble_app_set_event(RSI_BLE_LTK_REQ_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_le_security_keys_event
 * @brief      invoked when security keys event is received
 * @param[in]  rsi_bt_event_le_security_keys_t, security keys information
 * @return     none.
 * @section description
 * This callback function indicates security keys information
 */
static void rsi_ble_on_le_security_keys_event(rsi_bt_event_le_security_keys_t *le_sec_keys)
{
  LOG_PRINT(" \r\n rsi_ble_on_le_security_keys_event \r\n");
  add_security_keys_to_device_list(ble_dev_ltk_list, le_sec_keys);
  memcpy(&temp_le_sec_keys, le_sec_keys, sizeof(rsi_bt_event_le_security_keys_t));
  rsi_ble_app_set_event(RSI_BLE_SECURITY_KEYS_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_enh_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the enhanced connection
 */
void rsi_ble_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  memcpy(&rsi_app_enhance_connected_device, resp_enh_conn, sizeof(rsi_ble_event_enhance_conn_status_t));
  dev_address_type = resp_enh_conn->dev_addr_type;
  memcpy(remote_dev_bd_addr, resp_enh_conn->dev_addr, 6);

  rsi_ble_app_set_event(RSI_BLE_ENHANCE_CONNECTED_EVENT);
}
/*==============================================*/
/**
 * @fn         rsi_ble_data_length_change_event
 * @brief      invoked when data length is set
  * @section description
 * This Callback function indicates data length is set
 */
void rsi_ble_data_length_change_event(rsi_ble_event_data_length_update_t *rsi_ble_data_length_update)
{
  LOG_PRINT("Max_tx_octets: %d \r\n", rsi_ble_data_length_update->MaxTxOctets);
  LOG_PRINT("Max_tx_time: %d \r\n", rsi_ble_data_length_update->MaxTxTime);
  LOG_PRINT("Max_rx_octets: %d \r\n", rsi_ble_data_length_update->MaxRxOctets);
  LOG_PRINT("Max_rx_time: %d \r\n", rsi_ble_data_length_update->MaxRxTime);
  rsi_ble_app_set_event(RSI_APP_EVENT_DATA_LENGTH_CHANGE);
}
/*==============================================*/
/**
 * @fn         rsi_ble_phy_update_complete_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This Callback function indicates disconnected device information and status
 */
void rsi_ble_phy_update_complete_event(rsi_ble_event_phy_update_t *rsi_ble_event_phy_update_complete)
{
  LOG_PRINT("\n PHY UPDATE COMPLETE\n");
  memcpy(&rsi_app_phy_update_complete, rsi_ble_event_phy_update_complete, sizeof(rsi_ble_event_phy_update_t));
  rsi_ble_app_set_event(RSI_APP_EVENT_PHY_UPDATE_COMPLETE);
}

void rsi_ble_on_conn_update_complete_event(rsi_ble_event_conn_update_t *rsi_ble_event_conn_update_complete,
                                           uint16_t resp_status)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(
    rsi_ble_event_conn_update_complete); //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("\n CONN UPDATE COMPLETE\n");
  rsi_ble_app_set_event(RSI_APP_EVENT_CONN_UPDATE_COMPLETE);
}
/*==============================================*/
/**
* @fn         rsi_ble_on_remote_features_event
* @brief      invoked when LE remote features event is received.
* @param[in] resp_conn, connected remote device information
* @return     none.
* @section description
* This callback function indicates the status of the connection
*/
void rsi_ble_on_remote_features_event(rsi_ble_event_remote_features_t *rsi_ble_event_remote_features)
{

  LOG_PRINT("\nFeature received is %s\n", rsi_ble_event_remote_features->remote_features);
  memcpy(&remote_dev_feature, rsi_ble_event_remote_features, sizeof(rsi_ble_event_remote_features_t));
  rsi_ble_app_set_event(RSI_BLE_RECEIVE_REMOTE_FEATURES);
}
/*==============================================*/
/**
* @fn         rsi_ble_on_remote_conn_params_request_event
* @brief      invoked when remote conn params request is received
* @param[out] remote_conn_param, emote conn params request information
* @return     none.
* @section description
* This callback function indicates the status of the connection
*/
static void rsi_ble_on_remote_conn_params_request_event(rsi_ble_event_remote_conn_param_req_t *remote_conn_param,
                                                        uint16_t status)
{
  UNUSED_PARAMETER(status); //This statement is added only to resolve compilation warning, value is unchanged
  uint8_t str_remote_address[18] = { 0 };
  memcpy(&rsi_app_remote_device_conn_params, remote_conn_param, sizeof(rsi_ble_event_remote_conn_param_req_t));
  LOG_PRINT("\tREMOTE CONN PARAMS REQUEST\n");
  LOG_PRINT("\tREMOTE DEVICE ADDRESS:%s\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_app_remote_device_conn_params.dev_addr));
  LOG_PRINT("\tCONN PARAMS INFORMATION:\n");
  LOG_PRINT("\tCONN INTERVAL MIN:0x%04x\n", rsi_app_remote_device_conn_params.conn_interval_min);
  LOG_PRINT("\tCONN INTERVAL MAX:0x%04x\n", rsi_app_remote_device_conn_params.conn_interval_max);
  LOG_PRINT("\tCONN LATENCY:0x%04x\n", rsi_app_remote_device_conn_params.conn_latency);
  LOG_PRINT("\tSUPERVISION TIMEOUT:0x%04x\n", rsi_app_remote_device_conn_params.timeout);
  rsi_ble_app_set_event(RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ);
}
/*==============================================*/
/**
 * @fn         rsi_ble_on_encrypt_started 
 * @brief      its invoked when encryption started event is received.
 * @param[in]  remote_dev_address, it indicates remote bd address.
 * @return     none.
 * @section description
 * This callback function is invoked when encryption started events is received
 */
void rsi_ble_on_encrypt_started(uint16_t status, rsi_bt_event_encryption_enabled_t *enc_enabled)
{
  LOG_PRINT("start encrypt status: %d \r\n", status);
  add_device_to_ltk_key_list(ble_dev_ltk_list, enc_enabled);
  rsi_ble_app_set_event(RSI_BLE_ENCRYPT_STARTED_EVENT);
}

/*==============================================*/
/**
 * @fn         add_device_to_ltk_key_list
 * @brief      this function will add device to the ltk key list
 * @param[in]  ltk device list pointer
 * @param[in]  encrypt start event pointer
 * @param[out] status
 * @section description
 * add device to ltk key list
 * */
int8_t add_device_to_ltk_key_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list,
                                  rsi_bt_event_encryption_enabled_t *enc_enabled)
{
  int8_t status = 0;
  uint8_t ix;

  for (ix = 0; ix < RSI_MAX_LIST_SIZE; ix++) {

    if (ble_dev_ltk_list[ix].used == 1) {
      if ((enc_enabled->dev_addr_type > 1) && (!memcmp(enc_enabled->dev_addr, ble_dev_ltk_list[ix].Identity_addr, 6))) {
        break;
      }

      if ((enc_enabled->dev_addr_type <= 1)
          && (!memcmp(enc_enabled->dev_addr, ble_dev_ltk_list[ix].remote_dev_addr, 6))) {
        break;
      }
    }

    if (ble_dev_ltk_list[ix].used == 0) {
      ble_dev_ltk_list[ix].used       = 1;
      ble_dev_ltk_list[ix].enc_enable = enc_enabled->enabled;
      ble_dev_ltk_list[ix].sc_enable  = enc_enabled->sc_enable;
      memcpy(ble_dev_ltk_list[ix].remote_dev_addr, enc_enabled->dev_addr, 6);
      memcpy(ble_dev_ltk_list[ix].localltk, enc_enabled->localltk, 16);
      memcpy(ble_dev_ltk_list[ix].localrand, enc_enabled->localrand, 8);
      ble_dev_ltk_list[ix].local_ediv = enc_enabled->localediv;
      break;
    }
  }
  if (ix >= RSI_MAX_LIST_SIZE) {
    return -1;
  }
  return status;
}

/*==============================================*/
/**
 * @fn         add_device_to_resolvlist
 * @brief      this function will add device to resolvlistwith updated irks 
 * @param[out] ix, index of resolvlist
 * @return     none.
 * @section description
 * add device to resolvlistwith updated irks 
 * */
int8_t add_security_keys_to_device_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list,
                                        rsi_bt_event_le_security_keys_t *le_sec_keys)
{
  int8_t status = 0;
  uint8_t ix;
  for (ix = 0; ix < RSI_MAX_LIST_SIZE; ix++) {

    if ((ble_dev_ltk_list[ix].used == 1) && (!memcmp(ble_dev_ltk_list[ix].remote_dev_addr, le_sec_keys->dev_addr, 6))
        && (ble_dev_ltk_list[ix].remote_dev_addr_type == le_sec_keys->dev_addr_type)) {
      memcpy(ble_dev_ltk_list[ix].local_irk, le_sec_keys->local_irk, 16);
      memcpy(ble_dev_ltk_list[ix].peer_irk, le_sec_keys->remote_irk, 16);
      memcpy(ble_dev_ltk_list[ix].remote_rand, le_sec_keys->remote_rand, 8);
      memcpy(ble_dev_ltk_list[ix].remote_ltk, le_sec_keys->remote_ltk, 16);
      memcpy(ble_dev_ltk_list[ix].Identity_addr, le_sec_keys->Identity_addr, 6);
      ble_dev_ltk_list[ix].remote_ediv        = le_sec_keys->remote_ediv;
      ble_dev_ltk_list[ix].Identity_addr_type = le_sec_keys->Identity_addr_type;
      break;
    }
  }
  if (ix >= RSI_MAX_LIST_SIZE) {
    return -1;
  }

  return status;
}

/*==============================================*/
/**
 * @fn         add_device_to_resolvlist
 * @brief      this function will add device to resolvlistwith updated irks 
 * @param[out] ix, index of resolvlist
 * @return     none.
 * @section description
 * add device to resolvlistwith updated irks 
 * */
int32_t add_device_to_resolvlist(rsi_ble_resolvlist_group_t *resolvlist_p, rsi_ble_resolve_key_t *resolve_key_p)
{
  uint32_t status = 0;
  uint8_t ix;

  for (ix = 0; ix < RSI_BLE_RESOLVING_LIST_SIZE; ix++) {
    if (resolvlist_p[ix].used == 0) {
      resolvlist_p[ix].used               = 1;
      resolvlist_p[ix].Identity_addr_type = resolve_key_p->Identity_addr_type;
      memcpy(resolvlist_p[ix].Identity_addr, resolve_key_p->Identity_addr, sizeof(resolve_key_p->Identity_addr));
      memcpy(resolvlist_p[ix].peer_irk, resolve_key_p->peer_irk, sizeof(resolve_key_p->peer_irk));
      memcpy(resolvlist_p[ix].local_irk, resolve_key_p->local_irk, sizeof(resolve_key_p->local_irk));
      break;
    }
  }
  if (ix >= RSI_BLE_RESOLVING_LIST_SIZE) {
    return -1;
  }

  //add device to resolvlist or remove from resolvlist or clear the resolvlist
  status = rsi_ble_resolvlist(RSI_BLE_ADD_TO_RESOLVE_LIST,
                              resolve_key_p->Identity_addr_type,
                              temp_le_sec_keys.Identity_addr,
                              resolve_key_p->peer_irk,
                              resolve_key_p->local_irk);
  if (status != RSI_SUCCESS) {
    return status;
  }
  return status;
}

int8_t rsi_get_ltk_list(rsi_ble_dev_ltk_list_t *ble_dev_ltk_list, rsi_bt_event_le_ltk_request_t *le_ltk_req)
{
  uint8_t ix;

  for (ix = 0; ix < RSI_MAX_LIST_SIZE; ix++) {

    if (ble_dev_ltk_list[ix].used == 1) {
      if ((le_ltk_req->dev_addr_type > 1) && (!(memcmp(le_ltk_req->dev_addr, ble_dev_ltk_list[ix].Identity_addr, 6)))) {
        return ix;
      }

      if ((le_ltk_req->dev_addr_type <= 1)
          && (!(memcmp(le_ltk_req->dev_addr, ble_dev_ltk_list[ix].remote_dev_addr, 6)))) {
        return ix;
      }
    }
  }
  return -1;
}

/*==============================================*/
/**
 * @fn         update_resolvlist
 * @brief      this function will update the resolvlist if different remote address or different irks for same address received
 * @param[out] none.
 * @return     none.
 * @section description
 * update the resolvlist if different remote address or different irks for same address received
 * */
void update_resolvlist(rsi_ble_resolvlist_group_t *resolvlist_p, rsi_ble_resolve_key_t *resolve_key_p)
{
  uint8_t ix = 0;
  while (ix < RSI_BLE_RESOLVING_LIST_SIZE) {
    if (!strcmp((const char *)resolvlist_p[ix].Identity_addr, (const char *)resolve_key_p->Identity_addr)) {
      if (memcmp(resolvlist_p[ix].peer_irk, resolve_key_p->peer_irk, sizeof(resolve_key_p->peer_irk))) {
        add_device_to_resolvlist(resolvlist_p, resolve_key_p);
        break;
      } else {
        //LOG_PRINT("\nPeer IRK is not modified\n");
        break;
      }
    } else {
      strcpy((char *)resolvlist_p[ix].Identity_addr, (char *)resolve_key_p->Identity_addr);
      add_device_to_resolvlist(resolvlist_p, resolve_key_p);
      break;
    }
    ix++;
  }
}

/*==============================================*/
/**
 * @fn         rsi_ble_privacy_app
 * @brief      This application is used for testing the Privacy feature.
 * @param[in]  none. 
 * @return     none.
 * @section description
 * This function is used to test the LL Privacy Protocol.
 */
int32_t rsi_ble_privacy_app(void)
{
  int32_t status = 0, ix;
  int32_t event_id;
  uint8_t first_connect               = 0;
  rsi_ble_dev_ltk_list_t *ble_dev_ltk = NULL;
#if (RSI_DEVICE_ROLE == RSI_SLAVE)
  uint8_t adv[31] = { 2, 1, 6 };
#endif
#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif
#ifdef FW_LOGGING_ENABLE
  //Fw log component level
  sl_fw_log_level_t fw_component_log_level;
#endif
#ifndef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, BT_GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > BT_GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
#endif

#ifdef RSI_WITH_OS
  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif
  //! WC initialization
  status = rsi_wireless_init(0, RSI_OPERMODE_WLAN_BLE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }
#ifdef FW_LOGGING_ENABLE
  //! Set log levels for firmware components
  sl_set_fw_component_log_levels(&fw_component_log_level);

  //! Configure firmware logging
  status = sl_fw_log_configure(FW_LOG_ENABLE,
                               FW_TSF_GRANULARITY_US,
                               &fw_component_log_level,
                               FW_LOG_BUFFER_SIZE,
                               sl_fw_log_callback);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Firmware Logging Init Failed\r\n");
  }
#ifdef RSI_WITH_OS
  //! Create firmware logging semaphore
  rsi_semaphore_create(&fw_log_app_sem, 0);
  //! Create firmware logging task
  rsi_task_create((rsi_task_function_t)sl_fw_log_task,
                  (uint8_t *)"fw_log_task",
                  RSI_FW_TASK_STACK_SIZE,
                  NULL,
                  RSI_FW_TASK_PRIORITY,
                  &fw_log_task_handle);
#endif
#endif
  //! registering the GAP callback functions
  rsi_ble_gap_register_callbacks(rsi_ble_simple_central_on_adv_report_event,
                                 rsi_ble_on_connect_event,
                                 rsi_ble_on_disconnect_event,
                                 NULL,
                                 rsi_ble_phy_update_complete_event,
                                 rsi_ble_data_length_change_event,
                                 rsi_ble_on_enhance_conn_status_event,
                                 NULL,
                                 rsi_ble_on_conn_update_complete_event,
                                 rsi_ble_on_remote_conn_params_request_event);

  rsi_ble_gap_extended_register_callbacks(rsi_ble_on_remote_features_event, NULL);

  //! registering the SMP callback functions
  rsi_ble_smp_register_callbacks(rsi_ble_on_smp_request,
                                 rsi_ble_on_smp_response,
                                 rsi_ble_on_smp_passkey,
                                 rsi_ble_on_smp_failed,
                                 rsi_ble_on_encrypt_started,
                                 rsi_ble_on_smp_passkey_display,
                                 rsi_ble_on_sc_passkey,
                                 rsi_ble_on_le_ltk_req_event,
                                 rsi_ble_on_le_security_keys_event,
                                 NULL,
                                 NULL);
  //! create ble main task if ble protocol is selected
  rsi_semaphore_create(&ble_main_task_sem, 0);
  //!  initializing the application events map
  rsi_ble_app_init_events();

  //! Set local name
  status = rsi_bt_set_local_name((uint8_t *)RSI_BLE_DEVICE_NAME);
  if (status != RSI_SUCCESS) {
    return status;
  }

#if (RSI_DEVICE_ROLE == RSI_SLAVE)
  //!preparing scan response data
  adv[3] = strlen(RSI_BLE_DEVICE_NAME) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_BLE_DEVICE_NAME);

  //!set scan response data
  status = rsi_ble_set_scan_response_data(adv, strlen(RSI_BLE_DEVICE_NAME) + 5);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSet Scan Response Data Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nSet Scan Response Data Success\r\n");
  }

  //! set advertise data
  status = rsi_ble_set_advertise_data(adv, strlen(RSI_BLE_DEVICE_NAME) + 5);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSet Advertise Data Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nSet Advertise Data Success\r\n");
  }

  //! start the advertising
  status = rsi_ble_start_advertising();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nStart Advertising Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nStart Advertising Success\r\n");
  }
#else
  status = rsi_ble_start_scanning();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nStart Scanning Failed, Error Code : 0x%X\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nStart Scanning Success\r\n");
  }

#endif

  //! waiting for events from controller.
  while (1) {

    //! Application main loop
#ifndef RSI_WITH_OS
    rsi_wireless_driver_task();
#endif
    //! checking for events list
    event_id = rsi_ble_app_get_event();

    if (event_id == -1) {
      rsi_semaphore_wait(&ble_main_task_sem, 0);
      continue;
    }

    switch (event_id) {
      case RSI_APP_EVENT_ADV_REPORT: {
        //! advertise report event.
        //! clear the advertise report event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_ADV_REPORT);
        LOG_PRINT("\r\nIn Advertising Event\r\n");
        status = rsi_ble_connect(remote_addr_type, (int8_t *)remote_dev_bd_addr);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("connect status: 0x%lX\r\n", status);
        }
      } break;
      case RSI_BLE_CONN_EVENT: {
        //! event invokes when connection was completed

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_CONN_EVENT);
        rsi_6byte_dev_address_to_ascii(str_remote_dev_address, rsi_app_connected_device.dev_addr);
        LOG_PRINT("\r\n Module connected to address : %s \r\n", str_remote_dev_address);

        status = rsi_ble_smp_pair_request(remote_dev_bd_addr, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
      } break;

      case RSI_BLE_DISCONN_EVENT: {
        //! event invokes when disconnection was completed

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_DISCONN_EVENT);
        LOG_PRINT("\r\nModule got Disconnected\r\n");
        device_found  = 0;
        first_connect = 0;
#if (RSI_DEVICE_ROLE == RSI_MASTER)

        status = rsi_ble_connect(
          resolve_key.Identity_addr_type + 2,
          (int8_t *)rsi_ascii_dev_address_to_6bytes_rev(remote_dev_bd_addr, (int8_t *)resolve_key.Identity_addr));
        if (status != RSI_SUCCESS) {
          LOG_PRINT("connect status: 0x%X\r\n", status);
        }

#else
        //! start the advertising
        LOG_PRINT("\n Start Advertising\n");
        status = rsi_ble_start_advertising();
        if (status != RSI_SUCCESS) {
          return status;
        }
#endif
      } break;

      case RSI_BLE_SMP_REQ_EVENT: {
        LOG_PRINT("\r\nIn SMP request event\r\n");
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_REQ_EVENT);
        if (first_connect == 0) {
          //! initiating the SMP pairing process
          status        = rsi_ble_smp_pair_request(remote_dev_bd_addr, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
          first_connect = 1;
        }
      } break;

      case RSI_BLE_SMP_RESP_EVENT: {
        LOG_PRINT("\r\nIn SMP response event\r\n");
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_RESP_EVENT);

        //! initiating the SMP pairing process
        status = rsi_ble_smp_pair_response(remote_dev_bd_addr, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
      } break;

      case RSI_BLE_SMP_PASSKEY_EVENT: {
        LOG_PRINT("\r\nIn SMP passkey event\r\n");
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_PASSKEY_EVENT);

        //! initiating the SMP pairing process
        status = rsi_ble_smp_passkey(remote_dev_bd_addr, RSI_BLE_SMP_PASSKEY);
      } break;
      case RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT: {
        LOG_PRINT("\r\nIn SMP passkey display event\r\n");
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT);
      } break;
      case RSI_BLE_LTK_REQ_EVENT: {
        LOG_PRINT("\r\nIn LTK request event\r\n");
        //! event invokes when disconnection was completed

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_LTK_REQ_EVENT);
        ix = rsi_get_ltk_list(ble_dev_ltk_list, &temp_le_ltk_req);

        ble_dev_ltk = &ble_dev_ltk_list[ix];
        if ((ix != -1) && (ble_dev_ltk != NULL)) {
          LOG_PRINT("\n positive reply\n");
          //!  give le ltk req reply cmd with positive reply
          status = rsi_ble_ltk_req_reply(temp_le_ltk_req.dev_addr,
                                         (1 | (ble_dev_ltk->enc_enable) | (ble_dev_ltk->sc_enable << 7)),
                                         ble_dev_ltk->localltk);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n failed to restart smp pairing with status: 0x%x \r\n", status);
          }
        }

        else {
          LOG_PRINT("\n negative reply\n");
          //! give le ltk req reply cmd with negative reply
          status = rsi_ble_ltk_req_reply(temp_le_ltk_req.dev_addr, 0, NULL);

          LOG_PRINT("\n Re-initiating SMP\n");
          status = rsi_ble_smp_pair_request(remote_dev_bd_addr, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
        }
      } break;

      case RSI_BLE_SC_PASSKEY_EVENT: {
        LOG_PRINT("\r\nIn SC passkey event\r\n");
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SC_PASSKEY_EVENT);
        rsi_ble_smp_passkey(remote_dev_bd_addr, passkey);
      } break;

      case RSI_BLE_SECURITY_KEYS_EVENT: {
        LOG_PRINT("\r\nIn Sequrity keys event\r\n");
        //! event invokes when security keys are received
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SECURITY_KEYS_EVENT);
        resolve_key.remote_dev_addr_type = dev_address_type;

        resolve_key.remote_ediv = temp_le_sec_keys.remote_ediv;
        memcpy(resolve_key.remote_rand, temp_le_sec_keys.remote_rand, sizeof(temp_le_sec_keys.remote_rand));
        memcpy(resolve_key.remote_ltk, temp_le_sec_keys.remote_ltk, sizeof(temp_le_sec_keys.remote_ltk));

        rsi_6byte_dev_address_to_ascii(resolve_key.remote_dev_addr, temp_le_sec_keys.dev_addr);
        memcpy(resolve_key.peer_irk, temp_le_sec_keys.remote_irk, sizeof(temp_le_sec_keys.remote_irk));
        memcpy(resolve_key.local_irk, temp_le_sec_keys.local_irk, sizeof(temp_le_sec_keys.local_irk));

        resolve_key.Identity_addr_type = temp_le_sec_keys.Identity_addr_type;
        rsi_6byte_dev_address_to_ascii(resolve_key.Identity_addr, temp_le_sec_keys.Identity_addr);

        update_resolvlist((rsi_ble_resolvlist_group_t *)&resolvlist, &resolve_key);

        //get resolvlist size
        status = rsi_ble_get_resolving_list_size(&rsi_app_resp_resolvlist_size);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("line %d -> status: 0x%lx\r\n", __LINE__, status);
          return status;
        }

        //set address resolution enable and resolvable private address timeout
        status =
          rsi_ble_set_addr_resolution_enable(RSI_BLE_DEV_ADDR_RESOLUTION_ENABLE, RSI_BLE_SET_RESOLVABLE_PRIV_ADDR_TOUT);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("line %d -> status: 0x%lx\r\n", __LINE__, status);
          return status;
        }

        //set privacy mode
        status = rsi_ble_set_privacy_mode(
          resolve_key.Identity_addr_type,
          rsi_ascii_dev_address_to_6bytes_rev(remote_dev_bd_addr, (int8_t *)resolve_key.Identity_addr),
          RSI_BLE_PRIVACY_MODE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("line %d -> status: 0x%lx\r\n", __LINE__, status);
          return status;
        }
      } break;

      case RSI_BLE_SMP_FAILED_EVENT: {
        LOG_PRINT("\r\nIn SMP failed event\r\n");
        //! initiate SMP protocol as a Master

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_FAILED_EVENT);
      } break;

      case RSI_BLE_ENCRYPT_STARTED_EVENT: {
        LOG_PRINT("\r\nIn encrypt start event\r\n");
        //! start the encrypt event

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_ENCRYPT_STARTED_EVENT);
      } break;
      case RSI_BLE_ENHANCE_CONNECTED_EVENT: {
        LOG_PRINT("\r\nIn enhance connect event\r\n");
        //! remote device enhnace connected event
        //! clear the enhance connected event.
        rsi_ble_app_clear_event(RSI_BLE_ENHANCE_CONNECTED_EVENT);

      } break;
      case RSI_APP_EVENT_DATA_LENGTH_CHANGE: {
        LOG_PRINT("\r\nIn data length change event\r\n");
        //! remote device disconnected event

        //! clear the disconnected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_DATA_LENGTH_CHANGE);
      } break;
      case RSI_APP_EVENT_PHY_UPDATE_COMPLETE: {
        LOG_PRINT("\r\nIn physical update complete event\r\n");
        //! phy update complete event

        //! clear the phy updare complete event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_PHY_UPDATE_COMPLETE);
      } break;
      case RSI_APP_EVENT_CONN_UPDATE_COMPLETE: {
        LOG_PRINT("\r\nIn connection update complete event\r\n");
        //! conn update complete event

        LOG_PRINT("\n RSI_APP_EVENT_CONN_UPDATE_COMPLETE\n");
        //! clear the conn updare complete event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_CONN_UPDATE_COMPLETE);
      } break;
      case RSI_BLE_RECEIVE_REMOTE_FEATURES: {
        LOG_PRINT("\r\nIn BLE recieve remote features event\r\n");
        rsi_ble_app_clear_event(RSI_BLE_RECEIVE_REMOTE_FEATURES);

        if (first_connect == 0) {
          //! initiating the SMP pairing process
          status        = rsi_ble_smp_pair_request(remote_dev_bd_addr, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
          first_connect = 1;
        }

      } break;
      case RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ: {
        LOG_PRINT("\r\nIn Remote connection parameters request event\r\n");
        //! clear the remote conn params event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_REMOTE_CONN_PARAM_REQ);
        //! Default ACCEPT the remote conn params request(0-ACCEPT, 1-REJECT)
        status = rsi_ble_conn_param_resp(rsi_app_remote_device_conn_params.dev_addr, 0);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\n conn param resp status: 0x%lx\r\n", status);
        }
      } break;
      default: {
      }
    }
  }
}

/*==============================================*/
/**
 * @fn         main_loop
 * @brief      Schedules the Driver task.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function schedules the Driver task.
 */
void main_loop(void)
{
  while (1) {
    rsi_wireless_driver_task();
  }
}

/*==============================================*/
/**
 * @fn         main
 * @brief      main function
 * @param[in]  none.
 * @return     none.
 * @section description
 * This is the main function to call Application
 */
int main(void)
{
#ifdef RSI_WITH_OS
  int32_t status;
  rsi_task_handle_t bt_task_handle = NULL;
#endif

#ifndef RSI_WITH_OS

  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);
  //! Call LL Privacy protocol test Application
  rsi_ble_privacy_app();

  //! Application main loop
  main_loop();

  return 0;
#endif
#ifdef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, BT_GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > BT_GLOBAL_BUFF_LEN)) {
    return status;
  }

  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

  //! OS case
  //! Task created for BLE task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_ble_privacy_app,
                  (uint8_t *)"ble_task",
                  RSI_BT_TASK_STACK_SIZE,
                  NULL,
                  RSI_BT_TASK_PRIORITY,
                  &bt_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

  return status;
#endif
}
