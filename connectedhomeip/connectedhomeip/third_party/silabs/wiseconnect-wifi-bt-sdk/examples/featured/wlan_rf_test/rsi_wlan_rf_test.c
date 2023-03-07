/*******************************************************************************
* @file  rsi_wlan_rf_test.c
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
 * Include files
 * */
#include "rsi_driver.h"

// include file to refer data types
#include "rsi_data_types.h"

// COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

// WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"

#include "rsi_bootup_config.h"
// Error include files
#include "rsi_error.h"

// OS include file to refer OS specific functionality
#include "rsi_os.h"

// Transmit test power
#define RSI_TX_TEST_POWER 4

// Transmit test rate
#define RSI_TX_TEST_RATE RSI_RATE_1

// Transmit test length
#define RSI_TX_TEST_LENGTH 30

// Transmit test mode
#define RSI_TX_TEST_MODE RSI_BURST_MODE

// Transmit test channel
#define RSI_TX_TEST_CHANNEL 1

// Select Internal antenna or uFL connector
#define RSI_ANTENNA 1

// Antenna gain in 2.4GHz band
#define RSI_ANTENNA_GAIN_2G 0

// Antenna gain in 5GHz band
#define RSI_ANTENNA_GAIN_5G 0

// Memory length for driver
#define GLOBAL_BUFF_LEN 15000

// Wlan task priority
#define RSI_APPLICATION_TASK_PRIORITY 1

// Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

// Wlan task stack size
#define RSI_APPLICATION_TASK_STACK_SIZE 500

// Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 500

#ifdef CHIP_9117
// User mask shall be in the range 0-31
#define RSI_USER_MASK 0
#endif

// Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

// Variable used to stop receiving stats
volatile uint8_t stop_receiving_stats = 0;

//variable used to initialise stats received
uint32_t stats_received = 0;

// call back to receive RX Stats data
void rsi_wlan_stats_receive_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(buffer);
  UNUSED_CONST_PARAMETER(length);

  stats_received++;

  rsi_per_stats_rsp_t *stats = (rsi_per_stats_rsp_t *)buffer;
  LOG_PRINT("CRC PASS %4d | CRC FAIL %4d | RSSI %4d |\r\n", stats->crc_pass, stats->crc_fail, stats->cal_rssi);
  // For the buffer parameters information is available in SAPI documentation
  if (stats_received == 100) {
    stop_receiving_stats = 1;
  }
}

int32_t application()
{
  int32_t status = RSI_SUCCESS;

#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif

  // Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  // Module initialization
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }

#ifdef RSI_WITH_OS
  // Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif

  // WC initialization
  status = rsi_wireless_init(RSI_WLAN_TRANSMIT_TEST_MODE, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  // Send feature frame
  status = rsi_send_feature_frame();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSend Feature Frame Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nSend Feature Frame Success\r\n");

  // Register WLAN receive stats call back handler
  rsi_wlan_register_callbacks(RSI_WLAN_RECEIVE_STATS_RESPONSE_CB, rsi_wlan_stats_receive_handler);

  // Wlan radio init
  status = rsi_wlan_radio_init();
  if (status != RSI_SUCCESS) {
    // Return the status if error in sending command occurs
    return status;
  }

  // To select Internal antenna or uFL connector
  status = rsi_wireless_antenna(RSI_ANTENNA, RSI_ANTENNA_GAIN_2G, RSI_ANTENNA_GAIN_5G);
  if (status != RSI_SUCCESS) {
    return status;
  }

  // transmit test start
  status = rsi_transmit_test_start(RSI_TX_TEST_POWER,
                                   RSI_TX_TEST_RATE,
                                   RSI_TX_TEST_LENGTH,
                                   RSI_TX_TEST_MODE,
                                   RSI_TX_TEST_CHANNEL);
  if (status != RSI_SUCCESS) {
    return status;
  }

  // Add delay here to see the TX packets on AIR
  rsi_delay_ms(1000);

  // Stop TX transmit
  status = rsi_transmit_test_stop();
  if (status != RSI_SUCCESS) {
    return status;
  }

  ////////////////////////////////////////
  // Transmit data/TX from the peer//////
  ////////////////////////////////////////

  // Start/Receive publishing RX stats
  status = rsi_wlan_receive_stats_start(RSI_TX_TEST_CHANNEL);
  if (status != RSI_SUCCESS) {
    return status;
  }

  while (!stop_receiving_stats) {
#ifndef RSI_WITH_OS
    rsi_wireless_driver_task();
#endif
  }

  // Stop Receiving RX stats
  status = rsi_wlan_receive_stats_stop();
  if (status != RSI_SUCCESS) {
    return status;
  }

  return status;
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

  while (1) {
    rsi_wireless_driver_task();
  }
#endif
}
