/*******************************************************************************
* @file  rsi_bt_common_apis.h
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

#ifndef RSI_BT_COMMON_APIS_H
#define RSI_BT_COMMON_APIS_H

#include <rsi_bt_common.h>
#include <rsi_data_types.h>
#include <rsi_utils.h>

/******************************************************
 * *                      Macros
 * ******************************************************/
// success return value
#define RSI_SUCCESS 0

// failure return value
#define RSI_FAILURE -1

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

/******************************************************
 * *                    Structures
 * ******************************************************/

/******************************************************
 * *                 Global Variables
 * ******************************************************/

/******************************************************
 * *              BT Common API's Declarations
 * ******************************************************/
/*==============================================*/
/**
 * @fn          int32_t rsi_bt_set_bd_addr(uint8_t *dev_addr)
 *              
 */
int32_t rsi_bt_set_bd_addr(uint8_t *dev_addr);

/*==============================================*/
/**
 * @fn          int32_t rsi_bt_ber_enable(struct rsi_bt_ber_cmd_s *rsi_bt_ber_cmd)
 *
 */
int32_t rsi_bt_ber_enable_or_disable(struct rsi_bt_ber_cmd_s *ber_cmd);

/*==============================================*/
/**
 * @fn         rsi_bt_set_local_name
 * 
 */
int32_t rsi_bt_set_local_name(uint8_t *local_name);

/*==============================================*/
/**
 * @fn         rsi_bt_get_local_name      
 * 
 */
int32_t rsi_bt_get_local_name(rsi_bt_resp_get_local_name_t *bt_resp_get_local_name);

/*==============================================*/
/**
 * @fn         rsi_bt_cmd_update_gain_table_offset_or_max_pwr
 *
 *
 */
int32_t rsi_bt_cmd_update_gain_table_offset_or_max_pwr(uint8_t node_id,
                                                       uint8_t payload_len,
                                                       uint8_t *payload,
                                                       uint8_t req_type);

/*==============================================*/
/**
 * @fn         rsi_bt_get_rssi 
 *             
 * 
 */
int32_t rsi_bt_get_rssi(uint8_t *remote_dev_addr, int8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_get_local_device_address
 *              
 */
int32_t rsi_bt_get_local_device_address(uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_init 
 * 
 */
int32_t rsi_bt_init(void);

/*==============================================*/
/**
 * @fn         rsi_bt_deinit
 *             
 * 
 *
 */
int32_t rsi_bt_deinit(void);

/*==============================================*/
/**
 * @fn         rsi_bt_set_antenna
 *            
 *
 */
int32_t rsi_bt_set_antenna(uint8_t antenna_value);

/*==============================================*/
/**
 * @fn         rsi_bt_set_feature_bitmap
 *             
 * 
 */
int32_t rsi_bt_set_feature_bitmap(uint32_t feature_bit_map);

/*==============================================*/
/**
 * @fn         rsi_bt_set_antenna_tx_power_level
 *             
 * 
 */
int32_t rsi_bt_set_antenna_tx_power_level(uint8_t protocol_mode, int8_t tx_power);

/*==============================================*/
/**
 * @fn         rsi_bt_get_stack_version 
 *             
 * 
 */
int32_t rsi_bt_get_bt_stack_version(rsi_bt_resp_get_bt_stack_version_t *bt_resp_get_bt_stack_version);

int32_t rsi_bt_power_save_profile(uint8_t psp_mode, uint8_t psp_type);

#endif
