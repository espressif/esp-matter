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
 * Silicon Labs Module starts advertising and with BLE Provisioning the Access Point
 * details are fetched.
 * Silicon Labs device is configured as a WiFi station and connects to an Access Point.
 =================================================================================*/

/**
 * Include files
 **/

#include "rsi_driver.h"

// Error include files
#include "rsi_error.h"

#ifdef RSI_WITH_OS
// OS include file to refer OS specific functionality
#include "rsi_os.h"
#endif
#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

//BLE Specific inclusions
#include <rsi_ble_apis.h>
#include <rsi_ble_config.h>

// Memory length for driver
#define GLOBAL_BUFF_LEN 15000

#ifdef RSI_WITH_OS
#define RSI_APPLICATION_TASK_PRIORITY 1
#define RSI_BLE_TASK_PRIORITY         2
#define RSI_DRIVER_TASK_PRIORITY      3

#define RSI_APPLICATION_TASK_STACK_SIZE 1000
#define RSI_BLE_TASK_STACK_SIZE         1000
#define RSI_DRIVER_TASK_STACK_SIZE      2000
#endif

// Wlan client mode
#define RSI_WLAN_CLIENT_MODE 0

// BTLE Coex mode
#define RSI_BTLE_MODE 13

// Parameter to run forever loop
#define RSI_FOREVER 1

// Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

uint8_t ble_connected;

// APP version
#define APP_FW_VERSION "0.4"

// Function prototypes
extern void rsi_wlan_app_task(void);
extern void rsi_ble_configurator_task(void);
void rsi_ble_configurator_init(void);
uint8_t magic_word;

#ifdef RSI_WITH_OS
rsi_task_handle_t ble_task_handle    = NULL;
rsi_task_handle_t driver_task_handle = NULL;

rsi_semaphore_handle_t common_sem;
rsi_semaphore_handle_t wlan_thread_sem;
rsi_semaphore_handle_t ble_thread_sem;
#endif

int32_t application(void)
{
  int32_t status = RSI_SUCCESS;

#ifdef RSI_WITH_OS
  rsi_semaphore_create(&wlan_thread_sem, 0);
  rsi_semaphore_create(&ble_thread_sem, 0);
  rsi_semaphore_create(&common_sem, 0);
#endif

  // Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  // Silicon Labs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nDevice Initialization Success\r\n");

  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

#ifdef RSI_WITH_OS
  rsi_task_create((rsi_task_function_t)rsi_ble_configurator_task,
                  (uint8_t *)"ble_task",
                  RSI_BLE_TASK_STACK_SIZE,
                  NULL,
                  RSI_BLE_TASK_PRIORITY,
                  &ble_task_handle);

  // Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif

  // WiSeConnect initialization
  status = rsi_wireless_init(RSI_WLAN_CLIENT_MODE, RSI_OPERMODE_WLAN_BLE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nWireless Initialization Success\r\n");

  // BLE initialization
  rsi_ble_configurator_init();

#ifdef RSI_WITH_OS
  rsi_wlan_app_task();
#else
  while (RSI_FOREVER) {
    // WLAN application tasks
    rsi_wlan_app_task();

    // BLE application tasks
    rsi_ble_configurator_task();

    // wireless driver tasks
    rsi_wireless_driver_task();
  }
#endif
#ifdef RSI_WITH_OS
  return status;
#endif
}

// main function definition
int main(void)
{
#ifdef RSI_WITH_OS
  rsi_task_handle_t application_handle = NULL;

  // Create application task
  rsi_task_create((rsi_task_function_t)(int32_t)application,
                  (uint8_t *)"application_task",
                  RSI_APPLICATION_TASK_STACK_SIZE,
                  NULL,
                  RSI_APPLICATION_TASK_PRIORITY,
                  &application_handle);

  // Start the scheduler
  rsi_start_os_scheduler();
#else
  application();
#endif
}
