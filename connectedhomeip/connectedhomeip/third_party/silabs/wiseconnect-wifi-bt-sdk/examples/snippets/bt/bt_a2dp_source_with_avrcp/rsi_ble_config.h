/*******************************************************************************
 * @file  rsi_ble_config.h
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
 * @file    rsi_ble_config.h
 * @version 0.1
 * @date    01 Feb 2021
 *
 *
 *
 *  @brief : This file contains user configurable details to configure the device 
 *
 *  @section Description  This file contains user configurable details to configure the device
 */

#ifndef RSI_BLE_CONFIG_H
#define RSI_BLE_CONFIG_H
/***********************************************************************************************************************************************/
//  ! INCLUDES
/***********************************************************************************************************************************************/
#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include <stdlib.h>
#include "rsi_ble_apis.h"

/***********************************************************************************************************************************************/
//! Advertising command parameters
/***********************************************************************************************************************************************/
#define RSI_BLE_ADV_INT_MIN 0x152
#define RSI_BLE_ADV_INT_MAX 0x152

/***********************************************************************************************************************************************/
//! Connection parameters
/***********************************************************************************************************************************************/
#define LE_SCAN_INTERVAL 0x0035
#define LE_SCAN_WINDOW   0x0017

#define CONNECTION_INTERVAL_MIN 0x00C8
#define CONNECTION_INTERVAL_MAX 0x00C8

#define SUPERVISION_TIMEOUT 0x0C80

#define LE_SCAN_INTERVAL_CONN 0x0050
#define LE_SCAN_WINDOW_CONN   0x0050

/***********************************************************************************************************************************************/
//! Connection parameters for RSI as slave to remote device as master connection
/***********************************************************************************************************************************************/
#define S2M_CONNECTION_INTERVAL_MIN 0x00C8
#define S2M_CONNECTION_INTERVAL_MAX 0x00C8

#define S2M_CONNECTION_LATENCY  0x0000
#define S2M_SUPERVISION_TIMEOUT (4 * S2M_CONNECTION_INTERVAL_MAX)

/***********************************************************************************************************************************************/
//! Connection parameters for RSI as master to remote device as slave connection
/***********************************************************************************************************************************************/
#define M2S12_CONNECTION_INTERVAL_MIN 0x00C8
#define M2S12_CONNECTION_INTERVAL_MAX 0x00C8

#define M2S12_CONNECTION_LATENCY  0x0000
#define M2S12_SUPERVISION_TIMEOUT (4 * M2S12_CONNECTION_INTERVAL_MIN)

/***********************************************************************************************************************************************/
//! Connection parameters for RSI as master to remote device as slave connection
/***********************************************************************************************************************************************/
#define M2S34_CONNECTION_INTERVAL_MIN 0x0190
#define M2S34_CONNECTION_INTERVAL_MAX 0x0190

#define M2S34_CONNECTION_LATENCY  0x0000
#define M2S34_SUPERVISION_TIMEOUT (4 * M2S34_CONNECTION_INTERVAL_MIN)

/***********************************************************************************************************************************************/
//! Connection parameters for RSI as master to remote device as slave connection
/***********************************************************************************************************************************************/
#define M2S56_CONNECTION_INTERVAL_MIN 0x0320
#define M2S56_CONNECTION_INTERVAL_MAX 0x0320

#define M2S56_CONNECTION_LATENCY  0x0000
#define M2S56_SUPERVISION_TIMEOUT (4 * M2S56_CONNECTION_INTERVAL_MIN)

/***********************************************************************************************************************************************/

#define RSI_OPERMODE_WLAN_BLE 13

#define BLE_OUTPUT_POWER_FRONT_END_LOSS \
  0 /* This define value is 2 incase of EVK on screen verification, 0 at Antenna. Used in dBm to index conversion, per role Tx power  */

#include "rsi_ble_common_config.h"
#endif /* RSI_BLE_CONFIG_H */
