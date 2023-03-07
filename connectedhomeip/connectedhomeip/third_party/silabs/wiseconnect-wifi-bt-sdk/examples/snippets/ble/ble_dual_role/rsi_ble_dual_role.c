/*******************************************************************************
* @file  rsi_ble_dual_role.c
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

/*================================================================================
 * @brief : This file contains example application for BLE Dual Role
 * @section Description :
 * This application demonstrates how to connect with multiple(6) slaves as silabs
 * module in central mode and connect with multiple(2) masters as silabs module
 * in peripheral mode.
 =================================================================================*/

/**
 * Include files
 * */

//! BLE include file to refer BLE APIs
#include <rsi_ble_apis.h>
#include <rsi_ble_config.h>
#include <rsi_bt_common_apis.h>
#include <rsi_bt_common.h>

//! Common include file
#include <rsi_common_apis.h>

#include <string.h>
#include "rsi_driver.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

#define RSI_BLE_LOCAL_NAME "BLE_PERIPHERAL"

#define RSI_SEL_ANTENNA    RSI_SEL_INTERNAL_ANTENNA //RSI_SEL_EXTERNAL_ANTENNA
#define RSI_SCAN_RESP_DATA "SILABS"

//! Address type of the device to connect
#define RSI_BLE_DEV_ADDR_TYPE LE_PUBLIC_ADDRESS

//! Address of the devices to connect
#define RSI_BLE_DEV_1_ADDR "00:1A:7D:DA:71:22"
#define RSI_BLE_DEV_2_ADDR "00:1A:7D:DA:71:73"
#define RSI_BLE_DEV_3_ADDR "00:1A:7D:DA:71:63"
#define RSI_BLE_DEV_4_ADDR "00:1A:7D:34:54:66"
#define RSI_BLE_DEV_5_ADDR "00:1A:7D:DA:71:48"
#define RSI_BLE_DEV_6_ADDR "00:1A:7D:DA:71:22"
#define RSI_BLE_DEV_7_ADDR "00:1A:7D:DB:71:21"
#define RSI_BLE_DEV_8_ADDR "00:1A:7D:DA:71:0C"

#define RSI_BLE_DEV_ADDR "00:00:00:00:00:00" //dummy address

#define RSI_BLE_DEV_M1_ADDR "00:1A:7D:DA:71:14"
#define RSI_BLE_DEV_M2_ADDR "00:1A:7D:DA:71:00"

//! application events list
#define RSI_BLE_ADV_REPORT_EVENT   0x01
#define RSI_BLE_CONN_EVENT         0x02
#define RSI_BLE_DISCONN_EVENT      0x03
#define RSI_BLE_SCAN_RESTART_EVENT 0x04

//! Maximum number of advertise reports to hold
#define NO_OF_ADV_REPORTS 8

//! Maximum number of multiple slaves supported.
#define MAX_NUM_OF_SLAVES 6

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

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

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN] = { 0 };

//! Application global parameters.
static rsi_bt_resp_get_local_name_t rsi_app_resp_get_local_name = { 0 };
static uint8_t rsi_app_resp_get_dev_addr[RSI_DEV_ADDR_LEN]      = { 0 };
static uint16_t rsi_scan_in_progress;
static uint16_t rsi_app_no_of_adv_reports_rcvd = 0;
//! global variables
uint8_t conn_dev_addr[18]   = { 0 };
uint8_t remote_dev_addr[6]  = { 0 };
uint16_t num_of_conn_slaves = 0;
uint8_t num_of_conn_masters = 0;
uint16_t conn_req_pending   = 0;
static rsi_ble_event_adv_report_t rsi_app_adv_reports_to_app[NO_OF_ADV_REPORTS];
static rsi_ble_event_conn_status_t conn_event_to_app[10];
static rsi_ble_event_disconnect_t disconn_event_to_app;

rsi_semaphore_handle_t ble_main_task_sem;
static uint32_t ble_app_event_map;
static uint32_t ble_app_event_map1;
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
 * @fn         rsi_ble_on_adv_report_event
 * @brief      invoked when advertise report event is received
 * @param[in]  adv_report, pointer to the received advertising report
 * @return     none.
 * @section description
 * This callback function updates the scanned remote devices list
 */
void rsi_ble_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report)
{
  uint8_t str_adv_addr[18] = { 0 };

  memcpy(&rsi_app_adv_reports_to_app[(rsi_app_no_of_adv_reports_rcvd) % (NO_OF_ADV_REPORTS)],
         adv_report,
         sizeof(rsi_ble_event_adv_report_t));
  rsi_app_no_of_adv_reports_rcvd++;
  if (rsi_app_no_of_adv_reports_rcvd == NO_OF_ADV_REPORTS) {
    rsi_app_no_of_adv_reports_rcvd = 0;
    LOG_PRINT("restart scan\r\n");
    rsi_ble_app_set_event(RSI_BLE_SCAN_RESTART_EVENT);
  }
  rsi_6byte_dev_address_to_ascii(str_adv_addr, (uint8_t *)adv_report->dev_addr);
  LOG_PRINT("\n DEV_ADDR: %s\r\n", str_adv_addr);
  if ((!strcmp(RSI_BLE_DEV_1_ADDR, (char *)str_adv_addr)) || (!strcmp(RSI_BLE_DEV_2_ADDR, (char *)str_adv_addr))
      || (!strcmp(RSI_BLE_DEV_3_ADDR, (char *)str_adv_addr)) || (!strcmp(RSI_BLE_DEV_4_ADDR, (char *)str_adv_addr))
      || (!strcmp(RSI_BLE_DEV_5_ADDR, (char *)str_adv_addr)) || (!strcmp(RSI_BLE_DEV_6_ADDR, (char *)str_adv_addr))
      || (!strcmp(RSI_BLE_DEV_7_ADDR, (char *)str_adv_addr)) || (!strcmp(RSI_BLE_DEV_8_ADDR, (char *)str_adv_addr))) {
    memcpy(conn_dev_addr, str_adv_addr, sizeof(str_adv_addr));
    memcpy((int8_t *)remote_dev_addr, (uint8_t *)adv_report->dev_addr, 6);
    rsi_app_no_of_adv_reports_rcvd = 0;
    LOG_PRINT("\n matched \n");
    rsi_ble_app_set_event(RSI_BLE_ADV_REPORT_EVENT);
  }
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

  uint8_t str_conn_device[18] = { 0 };
  uint8_t ix                  = 0;

  rsi_6byte_dev_address_to_ascii(str_conn_device, (uint8_t *)resp_enh_conn->dev_addr);
  if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_1_ADDR)) {
    conn_event_to_app[0].dev_addr_type = resp_enh_conn->dev_addr_type;
    memcpy(conn_event_to_app[0].dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
    conn_event_to_app[0].status = resp_enh_conn->status;
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_2_ADDR)) {
    conn_event_to_app[1].dev_addr_type = resp_enh_conn->dev_addr_type;
    memcpy(conn_event_to_app[1].dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
    conn_event_to_app[1].status = resp_enh_conn->status;
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_3_ADDR)) {
    conn_event_to_app[2].dev_addr_type = resp_enh_conn->dev_addr_type;
    memcpy(conn_event_to_app[2].dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
    conn_event_to_app[2].status = resp_enh_conn->status;
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_4_ADDR)) {
    conn_event_to_app[3].dev_addr_type = resp_enh_conn->dev_addr_type;
    memcpy(conn_event_to_app[3].dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
    conn_event_to_app[3].status = resp_enh_conn->status;
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_5_ADDR)) {
    conn_event_to_app[4].dev_addr_type = resp_enh_conn->dev_addr_type;
    memcpy(conn_event_to_app[4].dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
    conn_event_to_app[4].status = resp_enh_conn->status;
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_6_ADDR)) {
    conn_event_to_app[5].dev_addr_type = resp_enh_conn->dev_addr_type;
    memcpy(conn_event_to_app[5].dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
    conn_event_to_app[5].status = resp_enh_conn->status;
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_7_ADDR)) {
    conn_event_to_app[6].dev_addr_type = resp_enh_conn->dev_addr_type;
    memcpy(conn_event_to_app[6].dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
    conn_event_to_app[6].status = resp_enh_conn->status;
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_8_ADDR)) {
    conn_event_to_app[7].dev_addr_type = resp_enh_conn->dev_addr_type;
    memcpy(conn_event_to_app[7].dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
    conn_event_to_app[7].status = resp_enh_conn->status;
  } else {
    for (ix = 8; ix <= 9; ix++) {
      rsi_6byte_dev_address_to_ascii(str_conn_device, (uint8_t *)conn_event_to_app[ix].dev_addr);
      if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_ADDR)) {
        conn_event_to_app[ix].dev_addr_type = resp_enh_conn->dev_addr_type;
        memcpy(conn_event_to_app[ix].dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
        conn_event_to_app[ix].status = resp_enh_conn->status;

        num_of_conn_masters++;
        break;
      }
    }
  }

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
  uint8_t str_conn_device[18] = { 0 };
  uint8_t ix                  = 0;

  rsi_6byte_dev_address_to_ascii(str_conn_device, (uint8_t *)resp_conn->dev_addr);
  if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_1_ADDR)) {
    memcpy(&conn_event_to_app[0], resp_conn, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_2_ADDR)) {
    memcpy(&conn_event_to_app[1], resp_conn, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_3_ADDR)) {
    memcpy(&conn_event_to_app[2], resp_conn, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_4_ADDR)) {
    memcpy(&conn_event_to_app[3], resp_conn, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_5_ADDR)) {
    memcpy(&conn_event_to_app[4], resp_conn, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_6_ADDR)) {
    memcpy(&conn_event_to_app[5], resp_conn, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_7_ADDR)) {
    memcpy(&conn_event_to_app[6], resp_conn, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_8_ADDR)) {
    memcpy(&conn_event_to_app[7], resp_conn, sizeof(rsi_ble_event_conn_status_t));
  } else {
    for (ix = 8; ix <= 9; ix++) {
      rsi_6byte_dev_address_to_ascii(str_conn_device, (uint8_t *)conn_event_to_app[ix].dev_addr);
      if (!strcmp((char *)str_conn_device, RSI_BLE_DEV_ADDR)) {
        memcpy(&conn_event_to_app[ix], resp_conn, sizeof(rsi_ble_event_conn_status_t));
        num_of_conn_masters++;
        break;
      }
    }
  }

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
 * This Callback function indicates disconnected device information and status
 */
static void rsi_ble_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  UNUSED_PARAMETER(reason); //This statement is added only to resolve compilation warning, value is unchanged
  uint8_t str_disconn_device[18] = { 0 };
  uint8_t str_conn_device[18]    = { 0 };
  uint8_t ix                     = 0;

  rsi_6byte_dev_address_to_ascii(str_disconn_device, (uint8_t *)resp_disconnect->dev_addr);
  if (!strcmp((char *)str_disconn_device, RSI_BLE_DEV_1_ADDR)) {
    memset(&conn_event_to_app[0], 0, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_disconn_device, RSI_BLE_DEV_2_ADDR)) {
    memset(&conn_event_to_app[1], 0, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_disconn_device, RSI_BLE_DEV_3_ADDR)) {
    memset(&conn_event_to_app[2], 0, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_disconn_device, RSI_BLE_DEV_4_ADDR)) {
    memset(&conn_event_to_app[3], 0, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_disconn_device, RSI_BLE_DEV_5_ADDR)) {
    memset(&conn_event_to_app[4], 0, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_disconn_device, RSI_BLE_DEV_6_ADDR)) {
    memset(&conn_event_to_app[5], 0, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_disconn_device, RSI_BLE_DEV_7_ADDR)) {
    memset(&conn_event_to_app[6], 0, sizeof(rsi_ble_event_conn_status_t));
  } else if (!strcmp((char *)str_disconn_device, RSI_BLE_DEV_8_ADDR)) {
    memset(&conn_event_to_app[7], 0, sizeof(rsi_ble_event_conn_status_t));
  } else {
    for (ix = 8; ix <= 9; ix++) {
      rsi_6byte_dev_address_to_ascii(str_conn_device, (uint8_t *)conn_event_to_app[ix].dev_addr);
      if (!strcmp((char *)str_conn_device, (const char *)str_disconn_device)) {
        memset(&conn_event_to_app[ix], 0, sizeof(rsi_ble_event_conn_status_t));
        num_of_conn_masters--;
        break;
      }
    }
  }

  memcpy(&disconn_event_to_app, resp_disconnect, sizeof(rsi_ble_event_disconnect_t));
  rsi_ble_app_set_event(RSI_BLE_DISCONN_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_ble_dual_role
 * @brief      Tests the BLE GAP peripheral role.
 * @param[in]  none
  * @return    none.
 * @section description
 * This function is used to test the BLE peripheral role and simple GAP API's.
 */
int32_t rsi_ble_dual_role(void)
{
  int32_t status         = 0;
  int32_t temp_event_map = 0;
  uint8_t adv[31]        = { 2, 1, 6 };
#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif
#ifndef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif
#ifdef RSI_WITH_OS
  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
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
    return status;
  }

  //! registering the GAP callback functions
  rsi_ble_gap_register_callbacks(rsi_ble_on_adv_report_event,
                                 rsi_ble_on_connect_event,
                                 rsi_ble_on_disconnect_event,
                                 NULL,
                                 NULL,
                                 NULL,
                                 rsi_ble_on_enhance_conn_status_event,
                                 NULL,
                                 NULL,
                                 NULL);
  //! create ble main task if ble protocol is selected
  rsi_semaphore_create(&ble_main_task_sem, 0);

  //! initialize the event map
  rsi_ble_app_init_events();

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(rsi_app_resp_get_dev_addr);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! set the local device name
  status = rsi_bt_set_local_name((uint8_t *)RSI_BLE_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&rsi_app_resp_get_local_name);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! select/set the antenna type(internal/external)
  /*	status = rsi_bt_set_antenna(RSI_SEL_ANTENNA);
      if(status != RSI_SUCCESS)
      {
      return status;
      }
      */
  //!preparing scan response data
  adv[3] = strlen(RSI_SCAN_RESP_DATA) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_SCAN_RESP_DATA);

  //!set scan response data
  rsi_ble_set_scan_response_data(adv, strlen(RSI_SCAN_RESP_DATA) + 5);

  //! prepare advertise data //local/device name
  adv[3] = strlen(RSI_BLE_LOCAL_NAME) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_BLE_LOCAL_NAME);

  //! set advertise data
  rsi_ble_set_advertise_data(adv, strlen(RSI_BLE_LOCAL_NAME) + 5);

#if 1
  //! start the advertising
  status = rsi_ble_start_advertising();
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif

  LOG_PRINT("\n Start scanning \n");
  rsi_app_no_of_adv_reports_rcvd = 0;
  rsi_ble_start_scanning();
  rsi_scan_in_progress = 1;
  if (status != RSI_SUCCESS) {
    return status;
  }

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

  while (1) {
    //! Application main loop
#ifndef RSI_WITH_OS
    //! run the non OS scheduler
    rsi_wireless_driver_task();
#endif
    //! checking for received events
    temp_event_map = rsi_ble_app_get_event();
    if (temp_event_map == RSI_FAILURE) {
      //! if events are not received loop will be continued.
      rsi_semaphore_wait(&ble_main_task_sem, 0);
      continue;
    }

    //! if any event is received, it will be served.
    LOG_PRINT("\n\ntemp event map is = 0X%lx\n\n", temp_event_map);
    switch (temp_event_map) {
      case RSI_BLE_ADV_REPORT_EVENT: {
        //! clear the advertise report event.
        rsi_ble_app_clear_event(RSI_BLE_ADV_REPORT_EVENT);

        LOG_PRINT("\n Advertise report received \n");
        if (conn_req_pending == 0) {
          if ((num_of_conn_slaves - 2) < MAX_NUM_OF_SLAVES) {
            if ((!strcmp((char *)conn_dev_addr, RSI_BLE_DEV_1_ADDR))
                || (!strcmp((char *)conn_dev_addr, RSI_BLE_DEV_2_ADDR))
                || (!strcmp((char *)conn_dev_addr, RSI_BLE_DEV_3_ADDR))
                || (!strcmp((char *)conn_dev_addr, RSI_BLE_DEV_4_ADDR))
                || (!strcmp((char *)conn_dev_addr, RSI_BLE_DEV_5_ADDR))
                || (!strcmp((char *)conn_dev_addr, RSI_BLE_DEV_6_ADDR))
                || (!strcmp((char *)conn_dev_addr, RSI_BLE_DEV_7_ADDR))
                || (!strcmp((char *)conn_dev_addr, RSI_BLE_DEV_8_ADDR))) {
              LOG_PRINT("\n Device found. Stop scanning \n");
              status               = rsi_ble_stop_scanning();
              rsi_scan_in_progress = 0;
              if (status != RSI_SUCCESS) {
                LOG_PRINT("\n status is = %lx \n ", status);
              }
              LOG_PRINT("\n Connect command \n");
              status = rsi_ble_connect(RSI_BLE_DEV_ADDR_TYPE, (int8_t *)remote_dev_addr);
              if (status != RSI_SUCCESS) {
                LOG_PRINT("\r\n Connecting failed with status : 0x%lx \n", status);
                rsi_ble_app_set_event(RSI_BLE_SCAN_RESTART_EVENT);
              } else {
                conn_req_pending = 1;
              }
            }
          }
        }
      } break;

      case RSI_BLE_CONN_EVENT: {
        num_of_conn_slaves++;

        LOG_PRINT("\n Device connected \n ");
        LOG_PRINT("\n Number of devices connected:%d \n", num_of_conn_slaves);

        conn_req_pending = 0;

        rsi_ble_app_clear_event(RSI_BLE_CONN_EVENT);
        if (num_of_conn_masters < 2) {
          status = rsi_ble_start_advertising();
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\n status is = %lx \n ", status);
          }
        }
        if ((num_of_conn_slaves - 2) < MAX_NUM_OF_SLAVES) {
          LOG_PRINT("\n Start scanning \n");
          rsi_app_no_of_adv_reports_rcvd = 0;
          status                         = rsi_ble_start_scanning();
          rsi_scan_in_progress           = 1;
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\n status is = %lx \n ", status);
          }
        }
      } break;

      case RSI_BLE_DISCONN_EVENT: {
        num_of_conn_slaves--;

        LOG_PRINT("\n Device disconnected\n ");
        LOG_PRINT("\n Number of connected devices:%d\n", num_of_conn_slaves);

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
        if (num_of_conn_masters < 2) {
          status = rsi_ble_start_advertising();
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\n status is = %lx \n ", status);
          }
        }
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_DISCONN_EVENT);

        if (!rsi_scan_in_progress) {
          rsi_app_no_of_adv_reports_rcvd = 0;
          status                         = rsi_ble_start_scanning();
          rsi_scan_in_progress           = 1;
          if ((status != RSI_SUCCESS)) {
            LOG_PRINT("\n status is = %lx \n ", status);
          }
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

      case RSI_BLE_SCAN_RESTART_EVENT: {
        //! clear the served event
        rsi_ble_app_clear_event(RSI_BLE_SCAN_RESTART_EVENT);

        LOG_PRINT("\n Device found. Stop scanning \n");
        status               = rsi_ble_stop_scanning();
        rsi_scan_in_progress = 0;
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\n status is = %lx \n ", status);
        }

        LOG_PRINT("\n Start scanning \n");
        rsi_app_no_of_adv_reports_rcvd = 0;
        status                         = rsi_ble_start_scanning();
        rsi_scan_in_progress           = 1;
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\n status is = %lx \n ", status);
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
  int32_t status;
#ifdef RSI_WITH_OS
  rsi_task_handle_t bt_task_handle = NULL;
#endif

#ifndef RSI_SAMPLE_HAL
  //! Board Initialization
  Board_init();
#endif

#ifndef RSI_WITH_OS

  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

  //! Call BLE Peripheral application
  status = rsi_ble_dual_role();

  //! Application main loop
  main_loop();

  return status;
#endif
#ifdef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

  //! OS case
  //! Task created for BLE task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_ble_dual_role,
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
