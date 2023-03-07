/*******************************************************************************
* @file  main.c
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
 * SiLabs Module starts advertising and with BLE Provisioning the Access Point
 * details are fetched.
 * SiLabs device is configured as a WiFi station and connects to an Access Point.
 =================================================================================*/

/**
 * Include files
 **/

#include "rsi_driver.h"

//! Error include files
#include "rsi_error.h"

#ifdef RSI_WITH_OS
//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#endif
#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

//!BLE Specific inclusions
#include <rsi_ble_apis.h>
#include <rsi_ble_config.h>

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

#ifdef RSI_WITH_OS
//!Wlan BLE task priority
#define RSI_WLAN_BLE_TASK_PRIORITY 1

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY 2

//! Ble task priority
#define RSI_BLE_TASK_PRIORITY 3

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 4

//! Wlan BT task stack size
#define RSI_WLAN_BLE_TASK_STACK_SIZE 1000

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE 1000

//! BT task stack size
#define RSI_BLE_TASK_STACK_SIZE 1000

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 2000

#endif

//! Wlan client mode
#define RSI_WLAN_CLIENT_MODE 0

//! BTLE Coex mode
#define RSI_BTLE_MODE 13

//! Parameter to run forever loop
#define RSI_FOREVER 1

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

uint8_t ble_connected;
extern uint8_t wlan_connected;

//! APP version
#define APP_FW_VERSION "0.4"

//! Function prototypes
extern void rsi_wlan_app_task(void);
extern void rsi_ble_configurator_task(void);

void rsi_wlan_mqtt_task(void);

int32_t rsi_wlan_mqtt_certs_init(void);
void rsi_ble_configurator_init(void);
int32_t rsi_wlan_ble_app(void);

uint8_t magic_word;

#ifdef RSI_WITH_OS
rsi_semaphore_handle_t commonsem, wlan_thread_sem, ble_thread_sem;

int32_t rsi_wlan_ble_app_init(void)
{
  int32_t status                     = RSI_SUCCESS;
  rsi_task_handle_t wlan_task_handle = NULL;
  rsi_task_handle_t ble_task_handle  = NULL;
#ifdef BLE_CONFIGURATOR
  //! WiSeConnect initialization
  status = rsi_wireless_init(RSI_WLAN_CLIENT_MODE, RSI_OPERMODE_WLAN_BLE);
#else
  status = rsi_wireless_init(RSI_WLAN_CLIENT_MODE, RSI_WLAN_ONLY);
#endif
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%X\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  status = rsi_wlan_mqtt_certs_init();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nset certificate issue, Error Code : 0x%X\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nset certificate Success\r\n");
  }
#ifdef BLE_CONFIGURATOR
  //! BLE initialization
  rsi_ble_configurator_init();
#endif
  rsi_wlan_app_task();
}

#endif

int32_t rsi_wlan_ble_app(void)
{
  int32_t status = RSI_SUCCESS;
#ifdef BLE_CONFIGURATOR
  //! WiSeConnect initialization
  status = rsi_wireless_init(RSI_WLAN_CLIENT_MODE, RSI_OPERMODE_WLAN_BLE);
#else
  status = rsi_wireless_init(RSI_WLAN_CLIENT_MODE, RSI_WLAN_ONLY);
#endif
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  status = rsi_wlan_mqtt_certs_init();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSet Certificate Issue, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nSet Certificate Success\r\n");
  }
#ifdef BLE_CONFIGURATOR
  rsi_ble_configurator_init();
#endif

  while (RSI_FOREVER) {
    //! WLAN application tasks
    rsi_wlan_app_task();
#ifdef BLE_CONFIGURATOR
    //! BLE application tasks
    rsi_ble_configurator_task();
#endif
    if (wlan_connected) {
      rsi_wlan_mqtt_task();
    }

    //! wireless driver tasks
    rsi_wireless_driver_task();
  }
}

//! Forever in wireless driver task
void main_loop()
{
  while (RSI_FOREVER) {
    rsi_wireless_driver_task();
  }
}

//! main function definition
int main(void)
{
  int32_t status;

#ifdef RSI_WITH_OS
  rsi_task_handle_t wlan_ble_task_handle = NULL;
  rsi_task_handle_t driver_task_handle   = NULL;
  rsi_task_handle_t ble_task_handle      = NULL;
#endif

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! Silabs module initialization
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
#ifdef BLE_CONFIGURATOR
  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);
#endif

#ifdef RSI_WITH_OS

  rsi_semaphore_create(&wlan_thread_sem, 0);
#ifdef BLE_CONFIGURATOR
  rsi_semaphore_create(&ble_thread_sem, 0);
#endif
  rsi_semaphore_create(&commonsem, 0);
  //! OS case

  //! Task created for COMMON task
  rsi_task_create(rsi_wlan_ble_app_init,
                  "common_task",
                  RSI_WLAN_BLE_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_BLE_TASK_PRIORITY,
                  &wlan_ble_task_handle);
#ifdef BLE_CONFIGURATOR
  rsi_task_create(rsi_ble_configurator_task,
                  "ble_task",
                  RSI_BLE_TASK_STACK_SIZE,
                  NULL,
                  RSI_BLE_TASK_PRIORITY,
                  &ble_task_handle);
#endif
  //! Task created for Driver task
  rsi_task_create(rsi_wireless_driver_task,
                  "driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! Call WLAN BLE application
  status = rsi_wlan_ble_app();

  //! Application main loop
  main_loop();
#endif
  return status;
}
