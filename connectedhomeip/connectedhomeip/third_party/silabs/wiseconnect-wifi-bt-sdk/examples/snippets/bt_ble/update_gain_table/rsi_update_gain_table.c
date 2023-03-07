/*******************************************************************************
* @file  rsi_update_gain_table_demo_68.c
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
/**
 * @file    rsi_update_gain_table_demo_68.c
 * @version 0.1
 * @date    xx Oct 2015
 *
 *
 *
 *  @brief : This file contains example application for Updating Gain table.
 *
 *  @section Description  This application serves as test for Updating Gain table.
 *
 */

/*=======================================================================*/
//   ! INCLUDES
/*=======================================================================*/

//! BT include file to refer BT APIs
#include <rsi_driver.h>
#include <rsi_bt_common_apis.h>
#include <rsi_ble_apis.h>
#include <rsi_bt.h>
#include <stdio.h>
#include <rsi_ble.h>

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/
#define RSI_BT_BLE_MODE             9
#define GLOBAL_BUFF_LEN             15000
#define UPDATE_GAIN_TABLE_MAX_POWER 0
#define UPDATE_GAIN_TABLE_OFFSET    1
#define BLE_NODE                    0
#define BT_NODE                     1
/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];
int rsi_app_task_update_gain_table(void)
{
  int32_t status = RSI_SUCCESS;

  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }
  //! SiLabs module initialization
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }
  //! WiSeConnect initialization
  status = rsi_wireless_init(RSI_WLAN_CLIENT_MODE, RSI_BT_BLE_MODE);
  if (status != RSI_SUCCESS) {
    return status;
  }

  uint8_t _RS9116_BT_REGION_BASED_MAXPOWER_VS_OFFSET_XX[] = { 5, 0,  4, 255, 0, 0,   0, 39, 0, 78,  2, 1,  4, 255,
                                                              0, 0,  0, 39,  0, 78,  0, 2,  4, 255, 3, 12, 0, 15,
                                                              5, 26, 1, 3,   6, 255, 0, 0,  0, 3,   0, 39, 1, 75,
                                                              2, 78, 0, 4,   4, 255, 3, 2,  2, 5,   5, 7,  1 };

  uint8_t _RS9116_BT_REGION_BASED_MAXPOWER_XX[] = { 0, 16, 1, 10, 2, 14, 3, 10, 4, 12 };

  uint8_t _RS9116_BLE_REGION_BASED_MAXPOWER_VS_OFFSET_XX[] = { 5, 0,  4, 255, 0, 0,   0, 39, 0, 78,  2, 1,  4, 255,
                                                               0, 0,  0, 39,  0, 78,  0, 2,  4, 255, 3, 12, 0, 15,
                                                               5, 26, 1, 3,   6, 255, 0, 0,  0, 3,   0, 39, 1, 75,
                                                               2, 78, 0, 4,   4, 255, 3, 2,  2, 5,   5, 7,  1 };

  uint8_t _RS9116_BLE_REGION_BASED_MAXPOWER_XX[] = { 0, 17, 1, 10, 2, 14, 3, 20, 4, 12 };

#if (RSI_BT_BLE_MODE == 9 || RSI_BT_BLE_MODE == 12)
  status = rsi_bt_cmd_update_gain_table_offset_or_max_pwr(BLE_NODE,
                                                          sizeof(_RS9116_BLE_REGION_BASED_MAXPOWER_XX),
                                                          _RS9116_BLE_REGION_BASED_MAXPOWER_XX,
                                                          UPDATE_GAIN_TABLE_MAX_POWER);
  LOG_PRINT("status : 0x%lx\n", status);
  if (status != 0) {
    return status;
  }
  status = rsi_bt_cmd_update_gain_table_offset_or_max_pwr(BLE_NODE,
                                                          sizeof(_RS9116_BLE_REGION_BASED_MAXPOWER_VS_OFFSET_XX),
                                                          _RS9116_BLE_REGION_BASED_MAXPOWER_VS_OFFSET_XX,
                                                          UPDATE_GAIN_TABLE_OFFSET);
  LOG_PRINT("status : 0x%lx\n", status);
  if (status != 0) {
    return status;
  }
#endif
#if (RSI_BT_BLE_MODE == 9 || RSI_BT_BLE_MODE == 4)
  status = rsi_bt_cmd_update_gain_table_offset_or_max_pwr(BT_NODE,
                                                          sizeof(_RS9116_BT_REGION_BASED_MAXPOWER_XX),
                                                          _RS9116_BT_REGION_BASED_MAXPOWER_XX,
                                                          UPDATE_GAIN_TABLE_MAX_POWER);

  LOG_PRINT("status : 0x%lx\n", status);
  if (status != 0) {
    return status;
  }
  status = rsi_bt_cmd_update_gain_table_offset_or_max_pwr(BT_NODE,
                                                          sizeof(_RS9116_BT_REGION_BASED_MAXPOWER_VS_OFFSET_XX),
                                                          _RS9116_BT_REGION_BASED_MAXPOWER_VS_OFFSET_XX,
                                                          UPDATE_GAIN_TABLE_OFFSET);
  LOG_PRINT("status : 0x%lx\n", status);
  if (status != 0) {
    return status;
  }
#endif
  return status;
}
//! Forever in wireless driver task
void main_loop()
{
  while (1) {
    rsi_wireless_driver_task();
  }
}
int main(void)
{
  int32_t status;

  status = rsi_app_task_update_gain_table();

  //! Application main loop
  main_loop();

  return status;
}
