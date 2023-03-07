/*******************************************************************************
* @file  rsi_common_config.h
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
 * @file         rsi_common_config.h
 * @version      0.1
 * @date         19 Mar 2021*
 *
 *
 *  @brief : This file contains user configurable details to configure the device
 *
 *  @section Description  This file contains user configurable details to configure the device
 */

#ifndef RSI_APP_COMMON_CONFIG_H
#define RSI_APP_COMMON_CONFIG_H
#include <stdio.h>
#include "rsi_ble_config.h"

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/

//! opermode selection
#define RSI_COEX_MODE 13

//! Application Task priorities
#define RSI_BLE_MAIN_TASK_PRIORITY 2
#define RSI_BLE_APP_TASK_PRIORITY  1

//! Application Task sizes
#define RSI_BLE_APP_MAIN_TASK_SIZE (512 * 2)
#define RSI_BLE_APP_TASK_SIZE      (512 * 4)

/*=======================================================================*/
//   ! Data type declarations
/*=======================================================================*/

typedef struct rsi_parsed_conf_s {
  struct rsi_ble_config_t {
    rsi_ble_conn_config_t rsi_ble_conn_config[TOTAL_CONNECTIONS];
  } rsi_ble_config;
} rsi_parsed_conf_t;

/*=======================================================================*/
//   ! Function prototype declarations
/*=======================================================================*/
int32_t rsi_initiate_power_save(void);
void rsi_ble_main_app_task(void);
void rsi_ble_task_on_conn(void *parameters);
void rsi_ble_slave_app_task(void);
void rsi_common_app_task(void);
#endif
