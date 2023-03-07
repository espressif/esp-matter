/*******************************************************************************
* @file  main.c
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
 * @file    main.c
 * @version 0.1
 * @date    8 May  2019
 *
 *
 *
 *  @brief : This file contains driver, module initialization and application execution
 *
 *  @section Description  This file contains driver, module initializations
 *
 *
 */

#include "rsi_common_app.h"
#include "rsi_driver.h"
#include "rsi_wlan.h"
#ifdef RSI_WITH_OS
//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#ifdef FW_LOGGING_ENABLE
//! Firmware logging includes
#include "sl_fw_logging.h"
#endif

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY 1
#define RSI_DNS_TASK_PRIORITY  1

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 4

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE 1024
#define RSI_DNS_TASK_STACK_SIZE  512

#define RSI_SOCKET1_TASK_STACK_SIZE 768

#define RSI_SOCKET2_TASK_STACK_SIZE 768

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 1024

#define RSI_BT_TASK_STACK_SIZE 1024

#define RSI_SBC_ENCODE_STACK_SIZE 1024

#define RSI_ANT_TASK_STACK_SIZE 1024

#define RSI_BT_TASK_PRIORITY 1

#define RSI_SOCKET1_TASK_PRIORITY 2
#define RSI_SOCKET2_TASK_PRIORITY 2

#define RSI_ANT_TASK_PRIORITY 1

#define RSI_BLE_TASK_STACK_SIZE 1024

//! BT task priority
#define RSI_UI_TASK_STACK_SIZE 512 * 4
rsi_task_handle_t ui_app_task_handle = NULL;

#define RSI_UI_TASK_PRIORITY 3

rsi_task_handle_t common_task_handle = NULL;
#define RSI_COMMON_TASK_PRIORITY   1
#define RSI_COMMON_TASK_STACK_SIZE (512 * 2)

rsi_semaphore_handle_t common_task_sem;
rsi_semaphore_handle_t ant_coex_sem;
rsi_semaphore_handle_t ant_sem;
rsi_semaphore_handle_t coex_sem;
rsi_semaphore_handle_t coex_sem1;
rsi_semaphore_handle_t socket_wait_sem1;
rsi_semaphore_handle_t socket_wait_sem2;

rsi_semaphore_handle_t event_sem;

rsi_semaphore_handle_t suspend_sem;

rsi_semaphore_handle_t wifi_task_sem;

rsi_semaphore_handle_t ui_task_sem;

rsi_semaphore_handle_t bt_sem;

rsi_task_handle_t ui_task_handle = NULL;
#endif

#ifdef FW_LOGGING_ENABLE
//! Memory length of driver updated for firmware logging
#define GLOBAL_BUFF_LEN (15000 + (FW_LOG_QUEUE_SIZE * MAX_FW_LOG_MSG_LEN))
#else
#define GLOBAL_BUFF_LEN 50000 //16900
#endif

//! Flag for infinite loop
#define RSI_FOREVER 1

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN] = { 0 };
uint8_t powersave_d = 0, ble_powersave_d = 0, wifi_powersave_d = 0;
uint8_t tx_rx_Completed = 0;
uint8_t loopback_done   = 0;
#if (BLE_DUAL_MODE_BT_A2DP_SOURCE_WIFI_HTTP_S_RX || BLE_DUAL_MODE_BT_SPP_SLAVE)
extern uint16_t num_of_conn_slaves;
extern uint8_t num_of_conn_masters;
#endif

#ifdef RSI_WITH_OS
rsi_task_handle_t driver_task_handle = NULL;
#endif

int main(void)
{
#ifdef RSI_WITH_OS

  rsi_task_handle_t wlan_task_handle = NULL;

  rsi_task_handle_t dns_task_handle = NULL;

  rsi_task_handle_t wlan_get_task_handle = NULL;

  rsi_task_handle_t socket1_task_handle = NULL;

  rsi_task_handle_t socket2_task_handle = NULL;

  rsi_task_handle_t bt_task_handle = NULL;

  rsi_task_handle_t ble_task_handle = NULL;

  rsi_task_handle_t ant_task_handle = NULL;

  rsi_task_handle_t ui_task_handle = NULL;

  rsi_task_handle_t cert_task_handle = NULL;

  (void)wlan_task_handle;
  (void)dns_task_handle;
  (void)wlan_get_task_handle;
  (void)socket1_task_handle;
  (void)socket2_task_handle;
  (void)bt_task_handle;
  (void)ble_task_handle;
  (void)ant_task_handle;
  (void)driver_task_handle;
  (void)ui_task_handle;
  (void)cert_task_handle;
#endif

  int32_t status = RSI_SUCCESS;

  //! Board Initialization
  rsi_hal_board_init();

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for WLAN task

//#if COEX_MAX_APP
#if RUN_TIME_CONFIG_ENABLE
  rsi_semaphore_create(&common_task_sem, 0);
  rsi_semaphore_create(&ui_task_sem, 0);
  rsi_task_create((void *)rsi_ui_app_task,
                  (uint8_t *)"ui_task",
                  RSI_UI_TASK_STACK_SIZE,
                  NULL,
                  RSI_UI_TASK_PRIORITY,
                  &ui_app_task_handle);
#endif
  rsi_task_create((void *)rsi_common_app_task,
                  (uint8_t *)"common_task",
                  RSI_COMMON_TASK_STACK_SIZE,
                  NULL,
                  RSI_COMMON_TASK_PRIORITY,
                  &common_task_handle);
#endif
  //! Task created for Driver task
  rsi_task_create((void *)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();
  /*#else
  while (RSI_FOREVER) {
    //! Execute demo
    rsi_demo_app();

    //! wireless driver task
    rsi_wireless_driver_task();
  }
#endif*/
  return 0;
}

#ifdef RSI_WITH_OS
void rsi_wireless_driver_task_create()
{
  //! Task created for Driver task
  rsi_task_create((void *)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
}
#endif
