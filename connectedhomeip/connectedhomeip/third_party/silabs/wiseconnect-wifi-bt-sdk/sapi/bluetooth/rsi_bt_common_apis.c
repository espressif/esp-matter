/*******************************************************************************
* @file  rsi_bt_common_apis.c
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

#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE))
#include <rsi_driver.h>
#include <rsi_bt_common.h>
#include <rsi_bt.h>

/** @addtogroup BT-BLE 
* @{
*/
/*==============================================*/
/**
 * @fn          int32_t rsi_bt_set_bd_addr(uint8_t *dev_addr)
 * @brief       Set the device BD address. This is blocking API.
 * @pre         needs to be called immediately after @ref rsi_wireless_init() OPERMODE command.
 * @param[in]   dev_addr - public address of the device to be set \n
 * @return      0 		 - Success \n
 *              Non-Zero Value - Failure \n
 *              -3       - Command is given in wrong state(i.e not immediate after opermode)
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_set_bd_addr(uint8_t *dev_addr)
{
  rsi_bt_set_local_bd_addr_t bd_addr;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bd_addr.dev_addr, dev_addr);
#else
  memcpy(bd_addr.dev_addr, dev_addr, RSI_DEV_ADDR_LEN);
#endif
  SL_PRINTF(SL_RSI_BT_SET_BD_ADDRESS, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_SET_BD_ADDR_REQ, &bd_addr, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_ber_enable_or_disable(struct rsi_bt_ber_cmd_s *ber_cmd)
 * @brief      Enable and disable BER. This is blocking API.
 * @param[in]  ber_cmd - pointer address which contains the command structure. Please refer rsi_bt_ber_cmd_s structure for more info \n 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_ber_enable_or_disable(struct rsi_bt_ber_cmd_s *ber_cmd)
{
  if (ber_cmd == NULL) {
    SL_PRINTF(SL_RSI_ERROR_INVALID_PARAMETER, BLUETOOTH, LOG_ERROR);

    return RSI_ERROR_INVALID_PARAM;
  }
  SL_PRINTF(SL_RSI_BT_BER_ENABLE_OR_DISABLE, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_PER_CMD, (void *)ber_cmd, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_set_local_name(uint8_t *local_name)
 * @brief      Set the given name to local device. This is blocking API.
 * @pre        \ref rsi_wireless_init() API need to be called before this API.
 * @param[in]  local_name - Name to be set to the local device.
 * @note       For BLE alone Opermode : When the name of the local device is set to a value with length more than 16 bytes then error is returned with an error code 0x4E66.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *             
 */
int32_t rsi_bt_set_local_name(uint8_t *local_name)
{
  uint8_t name_len;
  rsi_bt_req_set_local_name_t bt_req_set_local_name = { 0 };
  name_len                                          = RSI_MIN(strlen((const char *)local_name), (RSI_DEV_NAME_LEN - 1));

  memcpy(bt_req_set_local_name.name, local_name, name_len);
  bt_req_set_local_name.name[name_len] = 0;
  bt_req_set_local_name.name_len       = name_len;
  SL_PRINTF(SL_RSI_BT_SET_LOCAL_NAME, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_SET_LOCAL_NAME, &bt_req_set_local_name, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_cmd_update_gain_table_offset_or_max_pwr(uint8_t node_id, uint8_t payload_len, uint8_t *payload, uint8_t req_type)
 * @brief      Update gain table offset/max power. This is blocking API.
 * @pre        \ref rsi_wireless_init() API need to be called before this API.
 * @param[in]  node_id     - Node ID (0 - BLE, 1 - BT).
 * @param[in]  payload_len - Length of the payload.
 * @param[in]  payload     - Payload containing table data of gain table offset/max power
 * @param[in]  req_type    - update gain table request type (0 - max power update, 1 - offset update)
 * @return     0		-	Success \n
 *             0x4F01		-	Invalid gain table payload length \n
 *             0x4F02		-	Invalid region. \n
 *             0x4F03		-	Invalid gain table offset request type \n
 *             0x4F04           -       Invalid node id. \n	
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 *             
 */
int32_t rsi_bt_cmd_update_gain_table_offset_or_max_pwr(uint8_t node_id,
                                                       uint8_t payload_len,
                                                       uint8_t *payload,
                                                       uint8_t req_type)
{
  rsi_bt_cmd_update_gain_table_offset_or_maxpower_t bt_gain_table_offset = { 0 };
  bt_gain_table_offset.node_id                                           = node_id;
  bt_gain_table_offset.update_gain_table_type                            = req_type;
  bt_gain_table_offset.payload_len                                       = payload_len;
  memcpy(bt_gain_table_offset.payload, payload, bt_gain_table_offset.payload_len);
  SL_PRINTF(SL_RSI_BT_SET_GAIN_TABLE_OFFSET_OR_MAX_POWER_UPDATE, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_SET_GAIN_TABLE_OFFSET_OR_MAX_POWER_UPDATE, &bt_gain_table_offset, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_get_local_name(rsi_bt_resp_get_local_name_t *bt_resp_get_local_name)
 * @brief      Get the local device name. This is blocking API.
 * @pre        \ref rsi_wireless_init() API need to be called before this API.
 * @param[out] bt_resp_get_local_name - This parameter is the response buffer to hold the response of this API. Please refer rsi_bt_resp_get_local_name_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_get_local_name(rsi_bt_resp_get_local_name_t *bt_resp_get_local_name)
{
  SL_PRINTF(SL_RSI_BT_GET_LOCAL_NAME, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_GET_LOCAL_NAME, NULL, bt_resp_get_local_name);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_get_rssi(uint8_t *dev_addr, int8_t *resp) 
 * @brief      Get the RSSI of the remote device. This is blocking API.
 * @pre        \ref rsi_bt_connect() API need to be called before this API.
 * @param[in]  dev_addr -  Remote device address.
 * @param[out] resp - Parameter to hold the response of this API, rssi is filled in this resp parameter. 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_get_rssi(uint8_t *dev_addr, int8_t *resp)
{
  rsi_bt_get_rssi_t bt_rssi = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_rssi.dev_addr, dev_addr);
#else
  memcpy(bt_rssi.dev_addr, dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_GET_RSSI, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_GET_RSSI, &bt_rssi, resp);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_get_local_device_address(uint8_t *resp)
 * @brief      Get the local device address. This is blocking API.
 * @pre        \ref rsi_wireless_init() API needs to be called before this API.
 * @param[out] resp - Parameter to hold the response of this API, local bd_addr is filled in this resp parameter.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_get_local_device_address(uint8_t *resp)
{
  SL_PRINTF(SL_RSI_BT_GET_LOCAL_DEVICE_ADDRESS, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_GET_LOCAL_DEV_ADDR, NULL, resp);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_get_bt_stack_version(rsi_bt_resp_get_bt_stack_version_t *bt_resp_get_bt_stack_version) 
 * @brief      Get the BT stack version. This is blocking API.
 * @param[out] bt_resp_get_bt_stack_version -  Response buffer to hold the response of this API. Please refer rsi_bt_resp_get_bt_stack_version_s structure for more info
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_get_bt_stack_version(rsi_bt_resp_get_bt_stack_version_t *bt_resp_get_bt_stack_version)
{
  SL_PRINTF(SL_RSI_BT_STACK_VERSION, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_GET_BT_STACK_VERSION, NULL, bt_resp_get_bt_stack_version);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_init(void)
 * @brief      Initialize the BT device. This is blocking API.
 * @pre        \ref rsi_wireless_init() API needs to be called before this API.  \n
 *             If the device is in powersave, get back the device to ACTIVE MODE by using \ref rsi_bt_power_save_profile()
 * @param      None
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_init(void)
{
  SL_PRINTF(SL_RSI_BT_INIT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_INIT, NULL, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t 	rsi_bt_deinit(void)
 * @brief      Deinitialize the BT device. This is blocking API.
 * @pre        \ref rsi_wireless_init() API needs to be called before this API. \n
 *             If the device is in powersave, get back the device to ACTIVE MODE by using \ref rsi_bt_power_save_profile() 
 * @param      None
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_deinit(void)
{
  SL_PRINTF(SL_RSI_BT_DEINIT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_DEINIT, NULL, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_set_antenna(uint8_t antenna_value)
 * @brief      Select either internal / external antenna on the chip. This is blocking API.
 * @pre        \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]  antenna_value - Parameter is used to select either internal or external antenna. Possible values: \n
 *                             0x00 RSI_SEL_INTERNAL_ANTENNA \n 
 *                             0x01 RSI_SEL_EXTERNAL_ANTENNA 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_set_antenna(uint8_t antenna_value)
{
  rsi_ble_set_antenna_t ble_set_antenna = { 0 };

  ble_set_antenna.value = antenna_value;
  SL_PRINTF(SL_RSI_BT_SET_ANTENNA, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_SET_ANTENNA_SELECT, &ble_set_antenna, NULL);
}
/** @} */

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_set_feature_bitmap(uint32_t feature_bit_map)
 * @brief      Enable/Disable the mentioned features. This is blocking API.
 * @note       This API is not supported in the current release.
 * @pre        \ref rsi_sspmode_init() API needs to be called before this API.
 * @param[in]  feature_bit_map - features bit map list \n
 *                               0 This parameter is used for security purposes. \n 
 *                               If this bit is set pairing process occurs, else does not occur. \n
 *                               1 to 31 Reserved for future use. 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_set_feature_bitmap(uint32_t feature_bit_map)
{
  rsi_bt_set_feature_bitmap_t bt_features = { 0 };

  bt_features.bit_map = feature_bit_map;
  SL_PRINTF(SL_RSI_SET_FEATURE_MAP, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_SET_FEATURES_BITMAP, &bt_features, NULL);
}

/** @addtogroup BT-BLE 
* @{
*/
/*==============================================*/
/**
 * @fn         int32_t rsi_bt_set_antenna_tx_power_level(uint8_t protocol_mode, int8_t tx_power)
 * @brief      Enable/Disable the mentioned features. This is blocking API.
 * @pre        \ref rsi_wireless_init() API need to be called before this API
 * @param[in]  protocol_mode - protocol mode \n
 *             1 - BT classic \n
 *			   2 - BT Low Energy 
 * @param[in] tx-power - power value \n
 *             Antenna transmit power level \n
 *             Default Value for BLE Tx Power Index is 31, The range for the BLE Tx Power Index is 1 to 75 (0, 32 indexes are invalid) \n
 *             Default tx power index for BT classic is 14 \n
 *                      1 - 31     0DBM Mode.  \n
 *                     33 - 63     10DBM Mode. \n
 *                     64 - 75     HP Mode.    \n
 * @note       The default value will vary based on country region and board \n 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 */
int32_t rsi_bt_set_antenna_tx_power_level(uint8_t protocol_mode, int8_t tx_power)
{
  rsi_bt_set_antenna_tx_power_level_t bt_set_antenna_tx_power_level = { 0 };

  bt_set_antenna_tx_power_level.protocol_mode = protocol_mode;
#if RSI_BLE_PWR_INX_DBM
  if (protocol_mode == 2 /*BLE_MODE*/) {
    bt_set_antenna_tx_power_level.tx_power = rsi_convert_db_to_powindex(tx_power);
    if (bt_set_antenna_tx_power_level.tx_power == 0) {
      return RSI_ERROR_INVALID_PARAM;
    }
  } else {
    bt_set_antenna_tx_power_level.tx_power = tx_power;
  }
#else
  bt_set_antenna_tx_power_level.tx_power = tx_power;
#endif
  SL_PRINTF(SL_RSI_BT_SET_ANTENNA_TX_POWER_LEVEL, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_SET_ANTENNA_TX_POWER_LEVEL, &bt_set_antenna_tx_power_level, NULL);
}
/*==============================================*/
/**
 * @fn          int32_t rsi_bt_power_save_profile(uint8_t psp_mode, uint8_t psp_type)
 * @brief       Select the power save profile mode for BT / BLE. This is blocking API.
 * @pre         \ref rsi_wireless_init() API needs to be called before this API
 * @param[in]   psp_mode Following psp_mode is defined. \n
 *              0 - RSI_ACTIVE. In this mode module is active and power save is disabled. \n 
 *              1 - RSI_SLEEP_MODE_1. On mode. In this sleep mode, SoC will never turn off, therefore no \n
 *                  handshake is required before sending data to the module. BT/BLE does not support this mode. \n 
 *              2 - RSI_SLEEP_MODE_2. Connected sleep mode. In this sleep mode, SoC will go to sleep based \n
 *                  on GPIO or Message, therefore handshake is required before sending data to the module. \n 
 *              8 - RSI_SLEEP_MODE_8 :Deep sleep mode with RAM RETENTION. \n
 *              10- RSI_SLEEP_MODE_10 : Deep sleep mode without RAM RETENTION. \n
 *                  In this sleep mode, module will turn off the \n
 *                  SoC. Since SoC is turn off, therefore handshake is required before sending data to the module. \n
 * @param[in]   psp_type Following psp_type is defined. \n 
 *              0 - RSI_MAX_PSP. This psp_type will be used for max power saving \n 
 *              1 - Fast PSP \n 
 *              2 - UAPSD  
 * @return      0		-	Success \n
 *              Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for above error codes \ref error-codes .
 * @note        psp_type is only valid in psp_mode 2. \n
 *              BT/BLE doesnot support in RSI_SLEEP_MODE_1. \n
 *              BT/BLE supports only RSI_MAX_PSP mode. Remaining modes are not supported.
 *
 */
int32_t rsi_bt_power_save_profile(uint8_t psp_mode, uint8_t psp_type)
{
  int32_t status = RSI_SUCCESS;

  //Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  //Updating present bt power save type
  rsi_common_cb->power_save.bt_psp_type = psp_type;

  //Updating present bt power save mode
  rsi_common_cb->power_save.bt_psp_mode = psp_mode;

  status = rsi_sleep_mode_decision(rsi_common_cb);

  SL_PRINTF(SL_RSI_BT_POWER_SAVE_PROILE, BLUETOOTH, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */
/** @addtogroup BT-CLASSIC5
* @{
*/
/*==============================================*/
/**
 * @fn          int32_t rsi_bt_per_stats(uint8_t cmd_type, struct rsi_bt_per_stats_s *rsi_bt_per_stats)
 * @brief       Request the local device for BT PER operation.
 * @pre         Call \ref rsi_bt_per_tx() or rsi_bt_per_rx() before calling this API.
 * @param[in]   cmd_type - Parameter to define the command id type for PER operation. \n
 *              BT_PER_STATS_CMD_ID (0x08) - Command id enables PER statistics \n 
 *              BT_TRANSMIT_CMD_ID (0x15) - Command id enables PER transmit \n 
 *              BT_RECEIVE_CMD_ID (0x16) - Command id enables PER receive  
 * @param[in]   rsi_bt_per_stats - reference to the response structure. Please refer to rsi_bt_per_stats_s structure for more info. 
 * @return      0			-	Success \n
 *              Non-Zero Value	-	Failure
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_per_stats(uint8_t cmd_type, struct rsi_bt_per_stats_s *per_stats)
{
  SL_PRINTF(SL_RSI_BT_REQUEST_LOCAL_DEVICE, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_PER_CMD, &cmd_type, per_stats);
}

/*==============================================*/
/**
 * @fn          int32_t rsi_bt_per_cw_mode(struct rsi_bt_per_cw_mode_s *bt_cw_mode)
 * @brief       Keep the device in continuous wave mode. \n
 *              Not necessary to be called if transmit_mode is set to 2 - CONTINUOUS_WAVE_MODE in \ref rsi_bt_tx_per_params_s structure.
 * @pre         Call \ref rsi_bt_per_tx() before calling this API.
 * @param[in]   bt_cw_mode - continuous wave mode information needs to filled in this structure. Please refer \ref rsi_bt_per_cw_mode_s for more info.
 * @return      0			-	Success \n
 *              Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_per_cw_mode(struct rsi_bt_per_cw_mode_s *bt_cw_mode)
{
  if (bt_cw_mode == NULL) {

    SL_PRINTF(SL_RSI_ERROR_INVALID_PARAMETER, BLUETOOTH, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }
  SL_PRINTF(SL_RSI_PER_CW_MODE, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_PER_CMD, bt_cw_mode, NULL);
}

/**
 * @fn         int32_t rsi_bt_vendor_avdtp_stats_enable(uint16_t avdtp_stats_enable, 
 *                                                       uint32_t avdtp_stats_rate)
 * @brief      Issue vendor specific command for setting avdtp stats enable in controller, to receive the Stats. \n 
 *             This is a blocking API. A received event \ref rsi_bt_on_avdtp_stats_t has AVDTP Stats.
 * @pre        Call \ref rsi_wireless_init() before calling this API.
 * @param[in]  avdtp_stats_enable - stats enable, Enables the AVDTP stats in Controller 
 * @param[in]  avdtp_stats_rate -  stats rate, The rate for which the stats received to Host. \n 
 * @note       Presently stats rate is defined with 30000 milisec. This is the minimum time will be fine tunes by running all the possible modes.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_vendor_avdtp_stats_enable(uint16_t avdtp_stats_enable, uint32_t avdtp_stats_rate)
{
  rsi_bt_vendor_avdtp_stats_t rsi_bt_vendor_avdtp_stats;

  rsi_bt_vendor_avdtp_stats.opcode[0]          = (BT_VENDOR_AVDTP_STATS_CMD_OPCODE & 0xFF);
  rsi_bt_vendor_avdtp_stats.opcode[1]          = ((BT_VENDOR_AVDTP_STATS_CMD_OPCODE >> 8) & 0xFF);
  rsi_bt_vendor_avdtp_stats.avdtp_stats_enable = avdtp_stats_enable;
  rsi_bt_vendor_avdtp_stats.avdtp_stats_rate   = avdtp_stats_rate;

  SL_PRINTF(SL_RSI_BT_VENDOR_AVDTP_STATS_ENABLE, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_VENDOR_SPECIFIC, &rsi_bt_vendor_avdtp_stats, NULL);
}

/**
 * @fn         int32_t rsi_bt_vendor_ar_enable(uint16_t enable)
 * @brief      Issue vendor specific command for setting AutoRate in controller on given inputs. \n
 *             This is a blocking API. A received event \ref rsi_bt_on_ar_stats_t has AutoRate Stats.
 * @pre        Call \ref rsi_wireless_init() before calling this API.
 * @param[in]  enable - enables the AutoRate. 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure
 * 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * 
 */
int32_t rsi_bt_vendor_ar_enable(uint16_t enable)
{
  rsi_bt_vendor_ar_cmd_t rsi_bt_vendor_ar_cmd;
  rsi_bt_vendor_ar_cmd.opcode[0] = (BT_VENDOR_AR_CMD_OPCODE & 0xFF);
  rsi_bt_vendor_ar_cmd.opcode[1] = ((BT_VENDOR_AR_CMD_OPCODE >> 8) & 0xFF);

  rsi_bt_vendor_ar_cmd.ar_enable                           = enable;
  rsi_bt_vendor_ar_cmd._2m_state_continuous_pass_threshold = STATE_2M_CONTINUOUS_PASS_THRESHOLD;
  rsi_bt_vendor_ar_cmd._2m_state_pass_threshold            = STATE_2M_PASS_THRESHOLD;
  rsi_bt_vendor_ar_cmd._3m_state_absolute_fail_threshold   = STATE_3M_ABSOLUTE_FAIL_THRESHOLD;
  rsi_bt_vendor_ar_cmd._3m_state_continuous_fail_threshold = STATE_3M_CONTINUOUS_FAIL_THRESHOLD;
  rsi_bt_vendor_ar_cmd._3m_state_relative_fail_threshold   = STATE_3M_RELATIVE_FAIL_THRESHOLD;

  SL_PRINTF(SL_RSI_BT_VENDOR_AR_ENABLE, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_VENDOR_SPECIFIC, &rsi_bt_vendor_ar_cmd, NULL);
}
/**
 * @fn         int32_t rsi_bt_vendor_dynamic_pwr(uint16_t enable,
 *                                  uint8_t *remote_addr,
 *                                  uint8_t power_index_br,
 *                                  uint8_t power_index_2m,
 *                                  uint8_t power_index_3m)
 * @brief      Issue vendor specific command for setting dynamic_tx_power_index in controller on given inputs. \n 
 *             This is a blocking API.
 * @pre        Call \ref rsi_wireless_init() before calling this API.
 * @param[in]  remote_addr - Remote Device Address
 * @param[in]  power_index_br - Power value sets to BR Mode .
 * @param[in]  power_index_2m - Power value sets to 2m Mode data rate
 * @param[in]  power_index_3m - Power value sets to 3m Mode data rate
 * @return     0  		-  Success \n
 *             Non-Zero Value -  Failure
 * 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 *             
 *
 */
int32_t rsi_bt_vendor_dynamic_pwr(uint16_t enable,
                                  uint8_t *remote_addr,
                                  uint8_t power_index_br,
                                  uint8_t power_index_2m,
                                  uint8_t power_index_3m)
{
  rsi_bt_vendor_dynamic_pwr_cmd_t rsi_bt_vendor_dynamic_pwr_cmd = { 0 };
  rsi_bt_vendor_dynamic_pwr_cmd.opcode[0]                       = (BT_VENDOR_DYNAMIC_PWR_OPCODE & 0xFF);
  rsi_bt_vendor_dynamic_pwr_cmd.opcode[1]                       = ((BT_VENDOR_DYNAMIC_PWR_OPCODE >> 8) & 0xFF);

  memcpy(rsi_bt_vendor_dynamic_pwr_cmd.set_dynamic_pwr_index.remote_dev, remote_addr, 6);
  rsi_bt_vendor_dynamic_pwr_cmd.set_dynamic_pwr_index.dynamic_pwr_index.dynamic_power_enable = enable;
  rsi_bt_vendor_dynamic_pwr_cmd.set_dynamic_pwr_index.dynamic_pwr_index.pwr_index_br         = power_index_br;
  rsi_bt_vendor_dynamic_pwr_cmd.set_dynamic_pwr_index.dynamic_pwr_index.pwr_index_2m         = power_index_2m;
  rsi_bt_vendor_dynamic_pwr_cmd.set_dynamic_pwr_index.dynamic_pwr_index.pwr_index_3m         = power_index_3m;

  SL_PRINTF(SL_RSI_BT_VENDOR_DYNAMIC_POWER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_VENDOR_SPECIFIC, &rsi_bt_vendor_dynamic_pwr_cmd, NULL);
}

/** @} */

/**
 * @fn         int32_t rsi_memory_stats_enable(uint8_t protocol, uint8_t memory_stats_enable, uint32_t memory_stats_interval_ms)
 * @brief      Gives vendor specific command to the controller to set memory utilization stats enable.
 * @note       This API is not supported in the current release.
 * @param[in]  protocol - Protocol 
 * @param[in]  memory_stats_enable - Memory status enble 
 * @param[in]  memory_stats_interval_ms - memory stats interval time
 * @return     0  		-  Success \n
 *             Non-Zero Value -  Failure
 *
 */
int32_t rsi_memory_stats_enable(uint8_t protocol, uint8_t memory_stats_enable, uint32_t memory_stats_interval_ms)
{
  rsi_bt_vendor_memory_stats_t rsi_bt_vendor_memory_stats;

  rsi_bt_vendor_memory_stats.opcode[0]                = (BT_VENDOR_MEMORY_STATS_CMD_OPCODE & 0xFF);
  rsi_bt_vendor_memory_stats.opcode[1]                = ((BT_VENDOR_MEMORY_STATS_CMD_OPCODE >> 8) & 0xFF);
  rsi_bt_vendor_memory_stats.protocol                 = protocol;
  rsi_bt_vendor_memory_stats.memory_stats_enable      = memory_stats_enable;
  rsi_bt_vendor_memory_stats.memory_stats_interval_ms = memory_stats_interval_ms;

  SL_PRINTF(SL_RSI_MEMORY_STATS_ENABLE, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_VENDOR_SPECIFIC, &rsi_bt_vendor_memory_stats, NULL);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_bt_l2cap_connect(int8_t *remote_dev_addr, uint16_t psm, uint8_t preferred_mode)
 * @brief      Initiate the l2cap connection request
 * @note       This API is not supported in the current release.
 * @param[in]  remote_dev_addr - remote device address
 * @param[in]  psm - PSM
 * @param[in]  preferred_mode - preferred mode
 * @return     0  		-  Success \n
 *             Non-Zero Value -  Failure
 */
int32_t rsi_bt_l2cap_connect(int8_t *remote_dev_addr, uint16_t psm, uint8_t preferred_mode)
{
  rsi_bt_req_l2cap_connect_t bt_req_l2cap_connect = { 0 };
  memcpy(bt_req_l2cap_connect.dev_addr, remote_dev_addr, 6);
  bt_req_l2cap_connect.psm            = psm;
  bt_req_l2cap_connect.preferred_mode = preferred_mode;

  SL_PRINTF(SL_RSI_BT_L2CAP_CONNECT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_L2CAP_CONNECT, &bt_req_l2cap_connect, NULL);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_bt_l2cap_disconnect(int8_t *remote_dev_addr)
 * @brief      Initiate the l2cap disconnection request
 * @note       This API is not supported in the current release.
 * @param[in]  remote_dev_addr - remote device address
 * @return     0  		-  Success \n
 *             Non-Zero Value - Failure
 */
int32_t rsi_bt_l2cap_disconnect(int8_t *remote_dev_addr)
{
  rsi_bt_req_l2cap_disconnect_t bt_req_l2cap_disconnect = { 0 };
  memcpy(bt_req_l2cap_disconnect.dev_addr, remote_dev_addr, 6);
  SL_PRINTF(SL_RSI_BT_L2CAP_DISCONNECT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_L2CAP_DISCONNECT, &bt_req_l2cap_disconnect, NULL);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_bt_l2cap_init(void)
 * @brief      Initialize the l2cap profile for special data handling
 * @note       This API is not supported in the current release.
 * @param[in]  void
 * @return     0  		-  Success \n
 *             Non-Zero Value - Failure
 *             
 */
int32_t rsi_bt_l2cap_init(void)
{
  rsi_bt_req_profile_mode_t bt_req_l2cap_init = { 0 };
  bt_req_l2cap_init.profile_mode              = 1 << 8;
  SL_PRINTF(SL_RSI_BT_L2CAP_INIT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SET_PROFILE_MODE, &bt_req_l2cap_init, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_l2cap_data(int8_t *remote_dev_addr, uint8_t *data, uint16_t length, uint8_t frame_type)
 * @brief      Send the l2cap data to the remote device
 * @note       This API is not supported in the current release.
 * @param[in]  remote_dev_addr - remote device address
 * @param[in]  data - Data
 * @param[in]  length - Length 
 * @param[in]  frame_type - Frame type 
 * @return     0  		-  Success \n
 *             Non-Zero Value - Failure
 *
 */

int32_t rsi_bt_l2cap_data(int8_t *remote_dev_addr, uint8_t *data, uint16_t length, uint8_t frame_type)
{
  uint16_t xfer_len = 0;

  rsi_bt_req_l2cap_data_t bt_req_l2cap_data = { 0 };
  xfer_len                                  = RSI_MIN(length, RSI_BT_MAX_PAYLOAD_SIZE);
  memcpy(bt_req_l2cap_data.dev_addr, remote_dev_addr, 6);
  bt_req_l2cap_data.data_length[0] = (uint8_t)(xfer_len & 0x00FF);
  bt_req_l2cap_data.data_length[1] = (xfer_len & 0xFF00) >> 8;

  memcpy(bt_req_l2cap_data.data, data, xfer_len);
  bt_req_l2cap_data.frame_type = frame_type;

  //  memset(bt_req_l2cap_data.data,0x0f,10);

  SL_PRINTF(SL_RSI_BT_L2CAP_SEND_DATA_REMOTE_DEVICE_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_L2CAP_PROTOCOL_DATA, &bt_req_l2cap_data, NULL);
}
/*==============================================*/
/**
 * @fn         int32_t rsi_bt_l2cap_create_ertm_channel(rsi_bt_l2cap_ertm_channel_t l2cap_ertm)
 * @brief      Initiate the l2cap ertm channel connection.
 * @note       This API is not supported in the current release.
 * @param[in]  l2cap_ertm - Channel 
 * @return     0  		-  Success \n
 *             Non-Zero Value -  Failure
 *             
 */

int32_t rsi_bt_l2cap_create_ertm_channel(rsi_bt_l2cap_ertm_channel_t l2cap_ertm)
{

  SL_PRINTF(SL_RSI_BT_L2CAP_CREATE_ERTM_CHANNEL_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_L2CAP_ERTM_CONFIGURE, &l2cap_ertm, NULL);
}

#endif
