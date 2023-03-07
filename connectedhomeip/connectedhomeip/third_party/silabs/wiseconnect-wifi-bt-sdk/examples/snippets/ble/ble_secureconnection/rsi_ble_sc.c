/*******************************************************************************
* @file  rsi_ble_sc.c
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
 * @brief : This file contains example application for BLE Secure Connections
 * @section Description :
 * This application demonstrates how to configure the SiLabs device in Central
 * mode and connects with remote slave device and how to enable SMP (Security
 * Manager Protocol) pairing and a Secured passkey pairing.
 =================================================================================*/

/**
 * Include files
 * */

//! BLE include file to refer BLE APIs
#include <rsi_ble_apis.h>
#include <rsi_ble_config.h>
#include <rsi_bt_common_apis.h>
#include <rsi_bt_common.h>
#include <stdio.h>
#include "rsi_driver.h"
//! Common include file
#include <rsi_common_apis.h>

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif
#ifdef FW_LOGGING_ENABLE
//! Firmware logging includes
#include "sl_fw_logging.h"
#endif

#ifdef FW_LOGGING_ENABLE
//! Memory length of driver updated for firmware logging
#define BT_GLOBAL_BUFF_LEN (15000 + (FW_LOG_QUEUE_SIZE * MAX_FW_LOG_MSG_LEN))
#else
//! Memory length for driver
#define BT_GLOBAL_BUFF_LEN 15000
#endif

//! Memory to initialize driver
uint8_t global_buf[BT_GLOBAL_BUFF_LEN];

//! local device name
#define RSI_BLE_DEVICE_NAME "BLE_SMP_SC"

//! local device IO capability
#define RSI_BLE_SMP_IO_CAPABILITY 0x00
#define RSI_BLE_SMP_PASSKEY       0

#ifdef RSI_BLE_PING
//! Ping time out
#define RSI_BLE_PING_TIME_OUT 5000
#endif

//! application event list
#define RSI_BLE_CONN_EVENT                0x01
#define RSI_BLE_DISCONN_EVENT             0x02
#define RSI_BLE_SMP_REQ_EVENT             0x03
#define RSI_BLE_SMP_RESP_EVENT            0x04
#define RSI_BLE_SMP_PASSKEY_EVENT         0x05
#define RSI_BLE_SMP_FAILED_EVENT          0x06
#define RSI_BLE_ENCRYPT_STARTED_EVENT     0x07
#define RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT 0x08
#define RSI_BLE_SC_PASSKEY_EVENT          0x09
#define RSI_BLE_LTK_REQ_EVENT             0x0A

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

/*=======================================================================*/
//!    Powersave configurations
/*=======================================================================*/
#define ENABLE_POWER_SAVE 0 //! Set to 1 for powersave mode

#if ENABLE_POWER_SAVE
//! Power Save Profile Mode
#define PSP_MODE RSI_SLEEP_MODE_2
//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP
#endif

//! global parameters list
static uint32_t ble_app_event_map;
static uint8_t str_remote_address[18];
static uint8_t remote_dev_address[6];
static uint32_t numeric_value;
static rsi_bt_event_encryption_enabled_t encrypt_keys;
static rsi_bt_event_le_ltk_request_t ble_ltk_req;
static rsi_bt_event_le_security_keys_t app_ble_sec_keys;
#ifdef RSI_BLE_PING
uint16_t timeout_resp;
#endif
rsi_semaphore_handle_t ble_main_task_sem;
static uint32_t ble_app_event_map;
static uint32_t ble_app_event_map1;
#define MITM_REQ 0x01

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
 * @fn         rsi_ble_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  memcpy(remote_dev_address, resp_enh_conn->dev_addr, 6);
  LOG_PRINT("Enhance connected - str_remote_address : %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, resp_enh_conn->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_CONN_EVENT);
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
  memcpy(remote_dev_address, resp_conn->dev_addr, 6);
  LOG_PRINT("Connected - str_remote_address : %s\r\n",
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
  memcpy(remote_dev_address, resp_disconnect->dev_addr, 6);
  LOG_PRINT("Dis-connected - str_remote_address : %s , reason : %x \r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, resp_disconnect->dev_addr),
            reason);

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
void rsi_ble_on_smp_request(rsi_bt_event_smp_req_t *event_smp_req)
{
  memcpy(remote_dev_address, event_smp_req->dev_addr, 6);
  LOG_PRINT("smp_req - str_remote_address : %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, event_smp_req->dev_addr));
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
void rsi_ble_on_smp_response(rsi_bt_event_smp_resp_t *bt_event_smp)
{
  memcpy(remote_dev_address, bt_event_smp->dev_addr, 6);
  LOG_PRINT("smp_resp - str_remote_address : %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, bt_event_smp->dev_addr));
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
void rsi_ble_on_smp_passkey(rsi_bt_event_smp_passkey_t *event_smp_passkey)
{
  memcpy(remote_dev_address, event_smp_passkey->dev_addr, 6);
  LOG_PRINT("smp_passkey - str_remote_address : %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, event_smp_passkey->dev_addr));
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
  memcpy(remote_dev_address, smp_passkey_display->dev_addr, 6);
  LOG_PRINT("Pass_key display event remote addr: %s, passkey: %s \r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, smp_passkey_display->dev_addr),
            smp_passkey_display->passkey);
  rsi_ble_app_set_event(RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT);
}

void rsi_ble_on_sc_passkey(rsi_bt_event_sc_passkey_t *sc_passkey)
{
  memcpy(remote_dev_address, sc_passkey->dev_addr, 6);
  LOG_PRINT(" Pass_key event remote addr: %s, passkey: %06ld \r\n",
            rsi_6byte_dev_address_to_ascii(str_remote_address, sc_passkey->dev_addr),
            sc_passkey->passkey);
  numeric_value = sc_passkey->passkey;
  rsi_ble_app_set_event(RSI_BLE_SC_PASSKEY_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_le_ltk_req_event
 * @brief      invoked when ltk request event is received
 * @param[in]  le_ltk_req, ltk request parameters.
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
static void rsi_ble_on_le_ltk_req_event(rsi_bt_event_le_ltk_request_t *le_ltk_req)
{
  LOG_PRINT(" \r\n rsi_ble_on_le_ltk_req_event \r\n");
  memcpy(&ble_ltk_req, le_ltk_req, sizeof(rsi_bt_event_le_ltk_request_t));
  rsi_ble_app_set_event(RSI_BLE_LTK_REQ_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_le_security_keys
 * @brief      its invoked when le secure keys events is received.
 * @param[in]  rsi_ble_event_le_security_keys, secure event parameters.
 * @return     none.
 * @section description
 * This callback function is invoked when security keys event is received after encription started. 
 */
void rsi_ble_on_le_security_keys(rsi_bt_event_le_security_keys_t *rsi_ble_event_le_security_keys)
{
  memcpy(&app_ble_sec_keys, rsi_ble_event_le_security_keys, sizeof(rsi_bt_event_le_security_keys_t));
  LOG_PRINT("security keys event remote_ediv: 0x%x\r\n", app_ble_sec_keys.remote_ediv);
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
void rsi_ble_on_smp_failed(uint16_t status, rsi_bt_event_smp_failed_t *event_smp_failed)
{
  memcpy(remote_dev_address, event_smp_failed->dev_addr, 6);
  LOG_PRINT("smp_failed status: 0x%x, str_remote_address: %s\r\n",
            status,
            rsi_6byte_dev_address_to_ascii(str_remote_address, event_smp_failed->dev_addr));
  rsi_ble_app_set_event(RSI_BLE_SMP_FAILED_EVENT);
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
  memcpy(&encrypt_keys, enc_enabled, sizeof(rsi_bt_event_encryption_enabled_t));
  rsi_ble_app_set_event(RSI_BLE_ENCRYPT_STARTED_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_smp_test_app
 * @brief      This application is used for testing the SMP protocol.
 * @param[in]  none. 
 * @return     none.
 * @section description
 * This function is used to test the SMP Protocol.
 */
int32_t rsi_ble_smp_test_app(void)
{
  int32_t status = 0;
  int32_t event_id;
  uint8_t adv[31]                = { 2, 1, 6 };
  uint8_t pairing_info_available = 0;
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
  rsi_ble_gap_register_callbacks(NULL,
                                 rsi_ble_on_connect_event,
                                 rsi_ble_on_disconnect_event,
                                 NULL,
                                 NULL,
                                 NULL,
                                 rsi_ble_on_enhance_conn_status_event,
                                 NULL,
                                 NULL,
                                 NULL);

  //! registering the SMP callback functions
  rsi_ble_smp_register_callbacks(rsi_ble_on_smp_request,
                                 rsi_ble_on_smp_response,
                                 rsi_ble_on_smp_passkey,
                                 rsi_ble_on_smp_failed,
                                 rsi_ble_on_encrypt_started,
                                 rsi_ble_on_smp_passkey_display,
                                 rsi_ble_on_sc_passkey,
                                 rsi_ble_on_le_ltk_req_event,
                                 rsi_ble_on_le_security_keys,
                                 NULL,
                                 NULL);
  //! create ble main task if ble protocol is selected
  rsi_semaphore_create(&ble_main_task_sem, 0);
  //!  initializing the application events map
  rsi_ble_app_init_events();

  //! Set local name
  rsi_bt_set_local_name((uint8_t *)RSI_BLE_DEVICE_NAME);

  //! prepare advertise data //local/device name
  adv[3] = strlen(RSI_BLE_DEVICE_NAME) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_BLE_DEVICE_NAME);

  //! set advertise data
  rsi_ble_set_advertise_data(adv, strlen(RSI_BLE_DEVICE_NAME) + 5);

  //! start addvertising
  rsi_ble_start_advertising();

#if ENABLE_POWER_SAVE

  LOG_PRINT("\r\n Initiate module in to power save \r\n");
  //! enable wlan radio
  status = rsi_wlan_radio_init();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n radio init failed \n");
  }

  //! initiating power save in BLE mode
  status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to initiate power save in BLE mode \r\n");
    return status;
  }

  //! initiating power save in wlan mode
  status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to initiate power save in WLAN mode \r\n");
    return status;
  }

  LOG_PRINT("\r\n Module is in power save \r\n");

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
      case RSI_BLE_CONN_EVENT: {
        //! event invokes when connection was completed

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_CONN_EVENT);
        rsi_6byte_dev_address_to_ascii(str_remote_address, remote_dev_address);
        LOG_PRINT("\r\n Module connected to address : %s \r\n", str_remote_address);

#ifdef RSI_BLE_PING
        //! get ping time out
        rsi_ble_get_le_ping_timeout(remote_dev_address, &timeout_resp);

        //! set le ping time out
        rsi_ble_set_le_ping_timeout(remote_dev_address, RSI_BLE_PING_TIME_OUT);

        //! get ping time out
        rsi_ble_get_le_ping_timeout(remote_dev_address, &timeout_resp);
#endif

        //! initiating the SMP pairing process
        status = rsi_ble_smp_pair_request(remote_dev_address, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\n smp pair req failed with reason = %lx \n", status);
        }

      } break;

      case RSI_BLE_DISCONN_EVENT: {
        //! event invokes when disconnection was completed

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_DISCONN_EVENT);
        LOG_PRINT("\r\nModule got Disconnected\r\n");

#if ENABLE_POWER_SAVE

        LOG_PRINT("\r\n keep module in to active state \r\n");
        //! initiating Active mode in BT mode
        status = rsi_bt_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in ACTIVE mode \r\n");
          return status;
        }

        //! initiating Active mode in WLAN mode
        status = rsi_wlan_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in ACTIVE mode \r\n");
          return status;
        }

#endif
        //! start addvertising
        status = rsi_ble_start_advertising();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\n start adv cmd failed with reason = %lx \n", status);
        }

#if ENABLE_POWER_SAVE

        LOG_PRINT("\r\n keep module in to power save \r\n");
        status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          return status;
        }

        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in power save \r\n");
          return status;
        }
        LOG_PRINT("\r\n Module is in power save \r\n");
#endif
      } break;

      case RSI_BLE_SMP_REQ_EVENT: {
        LOG_PRINT("\r\nIn SMP request event\r\n");
        //! initiate SMP protocol as a Master

        pairing_info_available = 0;

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_REQ_EVENT);

        //! initiating the SMP pairing process
        status = rsi_ble_smp_pair_request(remote_dev_address, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\n smp pair req failed with reason = %lx \n", status);
        }
      } break;

      case RSI_BLE_SMP_RESP_EVENT: {
        LOG_PRINT("\r\nIn SMP response event\r\n");
        //! initiate SMP protocol as a Master

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_RESP_EVENT);

        pairing_info_available = 0;

        //! initiating the SMP pairing process
        status = rsi_ble_smp_pair_response(remote_dev_address, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\n smp pair resp failed with reason = %lx \n", status);
        }
      } break;

      case RSI_BLE_SMP_PASSKEY_EVENT: {
        LOG_PRINT("\r\nIn SMP passkey event\r\n");
        //! initiate SMP protocol as a Master

        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_PASSKEY_EVENT);

        //! initiating the SMP pairing process
        status = rsi_ble_smp_passkey(remote_dev_address, RSI_BLE_SMP_PASSKEY);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\n smp passkey cmd failed with reason = %lx \n", status);
        }
      } break;
      case RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT: {
        LOG_PRINT("\r\nIn SMP passkey display event\r\n");
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_PASSKEY_DISPLAY_EVENT);
      } break;

      case RSI_BLE_SC_PASSKEY_EVENT: {
        LOG_PRINT("\r\nIn SC passkey event\r\n");
        rsi_ble_app_clear_event(RSI_BLE_SC_PASSKEY_EVENT);
        status = rsi_ble_smp_passkey(remote_dev_address, numeric_value);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\n smp passkey cmd failed with reason = %lx \n", status);
        }
      } break;

      case RSI_BLE_LTK_REQ_EVENT: {
        LOG_PRINT("\r\nIn LTK request event\r\n");
        rsi_ble_app_clear_event(RSI_BLE_LTK_REQ_EVENT);
        if (pairing_info_available) {
          status = rsi_ble_ltk_req_reply(remote_dev_address,
                                         (1 | encrypt_keys.enabled | (encrypt_keys.sc_enable << 7)),
                                         encrypt_keys.localltk);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\n ltk req reply cmd failed with reason = %lx \n", status);
          }
        } else {
          rsi_ble_ltk_req_reply(remote_dev_address, 0, NULL);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\n ltk negative req reply cmd failed with reason = %lx \n", status);
          }

          /* restarting the SMP */
          status = rsi_ble_smp_pair_request(remote_dev_address, RSI_BLE_SMP_IO_CAPABILITY, MITM_REQ);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\n smp pair req failed with reason = %lx \n", status);
          } else {
            pairing_info_available = 0;
          }
        }
      } break;

      case RSI_BLE_SMP_FAILED_EVENT: {
        LOG_PRINT("\r\nIn SMP failed event\r\n");
        //! initiate SMP protocol as a Master

        pairing_info_available = 0;
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SMP_FAILED_EVENT);
      } break;

      case RSI_BLE_ENCRYPT_STARTED_EVENT: {
        LOG_PRINT("\r\nIn encrypt started event\r\n");
        //! start the encrypt event

        pairing_info_available = 1;
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_ENCRYPT_STARTED_EVENT);
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
  //! Call Simple SMP protocol test Application
  rsi_ble_smp_test_app();

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
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_ble_smp_test_app,
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
