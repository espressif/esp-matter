/*******************************************************************************
* @file  rsi_bt_avrcp_apis.c
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
#include "rsi_driver.h"
#ifdef RSI_BT_ENABLE

#include "rsi_bt.h"
#include "rsi_bt_apis.h"
#include "rsi_bt_config.h"
/** @addtogroup BT-CLASSIC2
* @{
*/
/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_init(uint8_t *avrcp_feature)
 * @brief      Set the AVRCP profile mode and also enables the Media Player Selection 
 *             Feature, if required. This is a blocking API.
 * @pre		   \ref rsi_wireless_init() API needs to be called before this API. 
 * @param[in]  avrcp_feature - used for enabling avrcp features \n
			   NULL - Default features used \n
			   1    - MediaPlayerSelection Feature Enable
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       if NULL is passed, AVRCP profile initialize with default stack features.  
 **/
int32_t rsi_bt_avrcp_init(uint8_t *avrcp_feature)
{
  rsi_bt_req_profile_mode_t bt_req_avrcp_init = { 0 };
  bt_req_avrcp_init.profile_mode              = RSI_AVRCP_PROFILE_BIT;
  if (avrcp_feature != NULL) {
    bt_req_avrcp_init.data_len = 1;
    bt_req_avrcp_init.data[0]  = *avrcp_feature;
  }

  SL_PRINTF(SL_RSI_BT_AVRCP_INIT, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SET_PROFILE_MODE, &bt_req_avrcp_init, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_conn(uint8_t *remote_dev_addr)
 * @brief      Initiate avrcp connection This is a blocking API.
 *             A received event \ref rsi_bt_on_avrcp_connect_t indicates that the avrcp connection response.
 * @pre        Call \ref rsi_bt_avrcp_init() before calling this API. Call this API after BT PHY level conn is completed. 
 * @param[in]  remote_dev_addr - remote device address
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure \n
 *             0x4540 ERR_BT_AVRCP_NOT_INITIALISED 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       If A2DP Profile is Initialized, then before calling this API the a2dp connection should be established using this \ref rsi_bt_a2dp_conn() API and once \n 
 *             \ref rsi_bt_on_a2dp_open_t callback is received start the streaming using \ref rsi_bt_stream_start() API. \n 
 *             This is applicable for all AVRCP related API's.
 */
int32_t rsi_bt_avrcp_conn(uint8_t *remote_dev_addr)
{
  rsi_bt_req_avrcp_conn_t bt_req_avrcp_connect = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_avrcp_connect.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_avrcp_connect.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif

  SL_PRINTF(SL_RSI_BT_AVRCP_CONNECT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_CONNECT, &bt_req_avrcp_connect, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_disconn(uint8_t *remote_dev_addr)
 * @brief      Initiate avrcp disconnection. This is a blocking API. 
 *             A received event \ref rsi_bt_on_avrcp_disconnect_t indicates that the avrcp disconnection response.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.   
 * @param[in]  remote_dev_addr - remote device address 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure  
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_disconn(uint8_t *remote_dev_addr)
{
  rsi_bt_req_avrcp_disconnect_t bt_req_avrcp_disconnect = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_avrcp_disconnect.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_avrcp_disconnect.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_AVRCP_DISCONNECT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_DISCONNECT, &bt_req_avrcp_disconnect, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_play(uint8_t *remote_dev_addr)
 * @brief      To play a track, call this API. This is a blocking API.
 *             A received event \ref rsi_bt_on_avrcp_play_t indicates that the avrcp play(Button Pressed/Button Released) response.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.  
 * @param[in]  remote_dev_addr - remote device address
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_play(uint8_t *remote_dev_addr)
{
  rsi_bt_req_avrcp_play_t bt_req_avrcp_play = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_avrcp_play.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_avrcp_play.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_AVRCP_PLAY_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_PLAY, &bt_req_avrcp_play, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_pause(uint8_t *remote_dev_addr)
 * @brief      To Pause a track, call this API. This is a blocking API. 
 *             A received event \ref rsi_bt_on_avrcp_pause_t indicates that the avrcp pause(Button Pressed/Button Released) response.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.  
 * @param[in]  remote_dev_addr - remote device address
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure  
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_pause(uint8_t *remote_dev_addr)
{
  rsi_bt_req_avrcp_pause_t bt_req_avrcp_pause = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_avrcp_pause.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_avrcp_pause.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_AVRCP_PAUSE_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_PAUSE, &bt_req_avrcp_pause, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_stop(uint8_t *remote_dev_addr)
 * @brief      To Stop a track, call this API. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.   
 * @param[in]  remote_dev_addr - remote device address
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_stop(uint8_t *remote_dev_addr)
{
  rsi_bt_req_avrcp_stop_t bt_req_avrcp_stop = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_avrcp_stop.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_avrcp_stop.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_AVRCP_STOP_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_STOP, &bt_req_avrcp_stop, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_next(uint8_t *remote_dev_addr)
 * @brief      To Play the next track, call this API. This is a blocking API.
 *             A received event \ref rsi_bt_on_avrcp_next_t indicates that the avrcp next(Button Pressed/Button Released) response.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.   
 * @param[in]  remote_dev_addr - remote device address
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_next(uint8_t *remote_dev_addr)
{
  rsi_bt_req_avrcp_next_t bt_req_avrcp_next = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_avrcp_next.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_avrcp_next.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_AVRCP_NEXT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_NEXT, &bt_req_avrcp_next, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_previous(uint8_t *remote_dev_addr)
 * @brief      To move to previous track, call this API. This is a blocking API. 
 *             A received event \ref rsi_bt_on_avrcp_previous_t indicates that the avrcp previous(Button Pressed/Button Released) response.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API. 
 * @param[in]  remote_dev_addr - remote device address
 * @return     0	 	-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_previous(uint8_t *remote_dev_addr)
{
  rsi_bt_req_avrcp_previous_t bt_req_avrcp_previous = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_avrcp_previous.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_avrcp_previous.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_AVRCP_PREV_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_PREVIOUS, &bt_req_avrcp_previous, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_vol_up(uint8_t *remote_dev_addr)
 * @brief      To Increase the volume of a track, call this API. This is a blocking API.
 *             A received event \ref rsi_bt_on_avrcp_vol_up_t indicates that the avrcp volume(Button Pressed/Button Released) response.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.   
 * @param[in]  remote_dev_addr - remote device address  
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure  
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_vol_up(uint8_t *remote_dev_addr)
{
  rsi_bt_req_avrcp_vol_up_t bt_req_avrcp_vol_up = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_avrcp_vol_up.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_avrcp_vol_up.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_AVRCP_VOL_UP_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_VOL_UP, &bt_req_avrcp_vol_up, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_vol_down(uint8_t *remote_dev_addr)
 * @brief      To Decrease the volume a track, call this API. This is a blocking API.
 *             A received event \ref rsi_bt_on_avrcp_vol_down_t indicates that the avrcp volume(Button Pressed/Button Released) response.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.   
 * @param[in]  remote_dev_addr - remote device address  
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_vol_down(uint8_t *remote_dev_addr)
{
  rsi_bt_req_avrcp_vol_down_t bt_req_avrcp_vol_down = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_avrcp_vol_down.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_avrcp_vol_down.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_AVRCP_VOL_DOWN_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_VOL_DOWN, &bt_req_avrcp_vol_down, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_get_capabilities(uint8_t *remote_dev_addr,
 *                                                   uint8_t capability_type,
 *                                                   rsi_bt_rsp_avrcp_get_capabilities_t *cap_list)
 * @brief      Gets the capabilities supported by remote device. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API. 
 * @param[in]  remote_dev_addr - remote device address  
 * @param[in]  capability_type - capability type (either 2 or 3) \n 
 *                               2 - Company ID \n
 *                               3 - Events Supported 
 * @param[out] cap_list - get remote capability list. Please refer \ref rsi_bt_rsp_avrcp_get_capabilities_s \n
 *                        structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_get_capabilities(uint8_t *remote_dev_addr,
                                      uint8_t capability_type,
                                      rsi_bt_rsp_avrcp_get_capabilities_t *cap_list)
{
  rsi_bt_req_avrcp_get_capabilities_t bt_req_avrcp_cap;
  memset(&bt_req_avrcp_cap, 0, sizeof(bt_req_avrcp_cap));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_avrcp_cap.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_avrcp_cap.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  bt_req_avrcp_cap.type = capability_type;

  SL_PRINTF(SL_RSI_BT_AVRCP_GET_CAPABILITES_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_CAPABILITES, &bt_req_avrcp_cap, cap_list);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_get_att_list(uint8_t *remote_dev_addr, rsi_bt_rsp_avrcp_get_atts_list_t *att_list)
 * @brief      Requests the TG to provide target supported player application setting attributes. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.   
 * @param[in]  remote_dev_addr - remote device address 
 * @param[out] att_list - attribute list, Please refer \ref rsi_bt_rsp_avrcp_get_atts_list_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_get_att_list(uint8_t *remote_dev_addr, rsi_bt_rsp_avrcp_get_atts_list_t *att_list)
{
  rsi_bt_req_avrcp_get_att_list_t bt_req_avrcp_att = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_avrcp_att.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_avrcp_att.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_GET_ATT_LIST_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_ATTS_LIST, &bt_req_avrcp_att, att_list);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_get_att_vals_list(uint8_t *remote_dev_addr,
 *                                                    uint8_t att_id,
 *                                                    rsi_bt_rsp_avrcp_get_att_vals_list_t *att_vals_list)
 * @brief       Requests the TG to list the set of possible values for the requested player application setting attribute. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.   
 * @param[in]  remote_dev_addr - remote device address   
 * @param[in]  att_id - attribute ID 
 * @param[out] att_vals_list - attribute value list, Please refer \ref rsi_bt_rsp_avrcp_get_att_vals_list_s structure for more info.  
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_get_att_vals_list(uint8_t *remote_dev_addr,
                                       uint8_t att_id,
                                       rsi_bt_rsp_avrcp_get_att_vals_list_t *att_vals_list)
{
  rsi_bt_req_avrcp_get_att_vals_list_t avrcp_att_vals;
  memset(&avrcp_att_vals, 0, sizeof(avrcp_att_vals));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(avrcp_att_vals.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(avrcp_att_vals.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  avrcp_att_vals.att_id = att_id;

  SL_PRINTF(SL_RSI_BT_GET_ATT_VALS_LIST_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_ATT_VALS_LIST, &avrcp_att_vals, att_vals_list);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_get_cur_att_val(uint8_t *remote_dev_addr,
                                     uint8_t *att_list,
                                     uint8_t nbr_atts,
                                     rsi_bt_rsp_avrcp_get_cur_att_val_t *att_vals_list)
 * @brief      Requests the TG to provide the current set values on the target 
 *             for the provided player application setting attributes list. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.  
 * @param[in]  remote_dev_addr - remote device address  
 * @param[in]  att_list - attribute ID 
 * @param[in]  nbr_atts - number of attributes 
 * @param[out] att_vals_list - attribute value list, Please refer \ref rsi_bt_rsp_avrcp_get_cur_att_val_s structure for more info. 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_get_cur_att_val(uint8_t *remote_dev_addr,
                                     uint8_t *att_list,
                                     uint8_t nbr_atts,
                                     rsi_bt_rsp_avrcp_get_cur_att_val_t *att_vals_list)
{
  rsi_bt_req_avrcp_get_cur_att_val_t avrcp_atts;
  memset(&avrcp_atts, 0, sizeof(avrcp_atts));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(avrcp_atts.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(avrcp_atts.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  avrcp_atts.nbr_atts = nbr_atts;
  for (ix = 0; (ix < nbr_atts) && (ix < RSI_MAX_ATT); ix++) {
    avrcp_atts.att_list[ix] = att_list[ix];
  }
  SL_PRINTF(SL_RSI_BT_GET_CURR_ATT_VAL_TRIGGER, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_CUR_ATT_VAL, &avrcp_atts, att_vals_list);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_set_cur_att_val(uint8_t *remote_dev_addr, 
 *                                                  att_val_t *val_list, uint8_t nbr_atts)
 * @brief      requests to set the player application setting list of player 
 *             application setting values on the TG for the corresponding defined 
 *             list of PlayerApplicationSettingAttributes. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.   
 * @param[in]  remote_dev_addr - remote device address 
 * @param[in]  val_list - value list 
 * @param[in]  nbr_atts - number of attributes  
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_set_cur_att_val(uint8_t *remote_dev_addr, att_val_t *val_list, uint8_t nbr_atts)
{
  rsi_bt_req_avrcp_set_cur_att_val_t att_val_list;
  memset(&att_val_list, 0, sizeof(att_val_list));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(att_val_list.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(att_val_list.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  att_val_list.nbr_atts = nbr_atts;
  for (ix = 0; (ix < nbr_atts) && (ix < RSI_MAX_ATT); ix++) {
    att_val_list.att_val_list[ix].att_id = val_list[ix].att_id;
    att_val_list.att_val_list[ix].att_id = val_list[ix].att_val;
  }

  SL_PRINTF(SL_RSI_BT_SET_CURR_ATT_VAL_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_SET_CUR_ATT_VAL, &att_val_list, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_get_element_att(uint8_t *remote_dev_addr,
 *                                                  uint8_t *att_ids,
 *                                                  uint8_t nbr_atts,
 *                                                  rsi_bt_rsp_avrcp_elem_attr_t *att_vals)
 * @brief      Requests the TG to provide the attributes of the element specified in the parameter. 
 *             This shall only be used to retrieve Metadata for the currently playing track from the 
 *             Addressed Player on the Control channel when GetItemAttributes is not supported. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.   
 * @param[in]  remote_dev_addr - Address of remote device    
 * @param[in]  att_ids - attributes ids
 * @param[in]  nbr_atts - number of attributes
 * @param[out] att_vals - attribute values in the list. Please refer \ref rsi_bt_rsp_avrcp_elem_attr_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_get_element_att(uint8_t *remote_dev_addr,
                                     uint8_t *att_ids,
                                     uint8_t nbr_atts,
                                     rsi_bt_rsp_avrcp_elem_attr_t *att_vals)
{
  rsi_bt_req_avrcp_get_ele_att_t att_list;
  memset(&att_list, 0, sizeof(att_list));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(att_list.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(att_list.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  att_list.nbr_atts = nbr_atts;
  for (ix = 0; (ix < nbr_atts) && (ix < RSI_MAX_ATT); ix++) {
    att_list.att_list[ix] = att_ids[ix];
  }

  SL_PRINTF(SL_RSI_BT_GET_ELEM_ATT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_ELEMENT_ATT, &att_list, att_vals);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_get_play_status(uint8_t *remote_dev_addr, rsi_bt_rsp_avrcp_get_player_status_t *play_status)
 * @brief      CT to get the status of the currently playing media at the TG. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.    
 * @param[in]  remote_dev_addr - remote device address 
 * @param[out] play_status - to capture the player status \n
 *             Please refer \ref rsi_bt_rsp_avrcp_get_player_status_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure  
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_get_play_status(uint8_t *remote_dev_addr, rsi_bt_rsp_avrcp_get_player_status_t *play_status)
{
  rsi_bt_req_avrcp_get_player_status_t play_status_req = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(play_status_req.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(play_status_req.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_GET_PLAY_STATUS_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_PLAY_STATUS, &play_status_req, play_status);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_reg_notification(uint8_t *remote_dev_addr, uint8_t event_id, uint8_t *p_resp_val)
 * @brief      Registers with the TG to receive notifications asynchronously based on specific events occurring. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.    
 * @param[in]  remote_dev_addr - remote device address.
 * @param[in]  event_id - event ID.
 * @param[out] p_resp_val - used to capture response for the event which is registered.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_reg_notification(uint8_t *remote_dev_addr, uint8_t event_id, uint8_t *p_resp_val)
{
#ifdef BD_ADDR_IN_ASCII
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(p_resp_val);
#endif
  rsi_bt_req_avrcp_reg_notification_t reg_notify_req;
  memset(&reg_notify_req, 0, sizeof(reg_notify_req));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(reg_notify_req.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(reg_notify_req.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  reg_notify_req.event_id = event_id;
  SL_PRINTF(SL_RSI_BT_REG_NOTIFICATION_TRIGGER, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_REG_NOTIFICATION, &reg_notify_req, p_resp_val);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_get_remote_version(uint8_t *remote_dev_addr,
 *                                                     rsi_bt_rsp_avrcp_remote_version_t *version)
 * @brief      Get the AVRCP profile version from remote device. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.    
 * @param[in]  remote_dev_addr - remote device address. 
 * @param[out] version - version info. Please refer \ref rsi_bt_rsp_avrcp_remote_version_s structure for more info
 * @return     0	 	-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_get_remote_version(uint8_t *remote_dev_addr, rsi_bt_rsp_avrcp_remote_version_t *version)
{
  rsi_bt_req_avrcp_remote_version_t profile_version = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(profile_version.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(profile_version.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_GET_REMOTE_VERSION_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_REMOTE_VERSION, &profile_version, version);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_get_att_text(uint8_t *remote_dev_addr,
 *                                 uint8_t nbr_atts,
 *                                 uint8_t *p_atts,
 *                                 player_att_text_t *p_att_text_resp)
 * @brief      Requests the TG to provide supported player application 
 *             setting attribute displayable text for the provided PlayerApplicationSettingAttributeID. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.    
 * @param[in]  remote_dev_addr - remote device address.  
 * @param[in]  nbr_atts - number of attributes.
 * @param[in]  p_atts - pointer to attributes. 
 * @param[out] p_att_text_resp - to capture response from this API. \n 
 *                               Please refer \ref player_att_text_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_get_att_text(uint8_t *remote_dev_addr,
                                  uint8_t nbr_atts,
                                  uint8_t *p_atts,
                                  player_att_text_t *p_att_text_resp)
{
  rsi_bt_req_avrcp_get_cur_att_val_t att_text;
  memset(&att_text, 0, sizeof(att_text));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(att_text.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(att_text.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  att_text.nbr_atts = nbr_atts;
  for (ix = 0; (ix < nbr_atts) && (ix < RSI_MAX_ATT); ix++) {
    att_text.att_list[ix] = p_atts[ix];
  }
  SL_PRINTF(SL_RSI_BT_GET_ATT_TEXT_TRIGGER, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_ATT_TEXT, &att_text, p_att_text_resp);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_get_att_val_text(uint8_t *remote_dev_addr,
                                      uint8_t att_id,
                                      uint8_t nbr_vals,
                                      uint8_t *p_vals,
                                      player_att_text_t *p_att_text_resp)
 * @brief      Get the AVRCP profile player attribute values text from remote device. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.    
 * @param[in]  remote_dev_addr - remote device address.  
 * @param[in]  att_id - attribute ID. 
 * @param[in]  nbr_vals - number of attribute values.
 * @param[in]  p_vals - pointer to attribute values. 
 * @param[out] p_att_text_resp - to capture response from this API. \n 
 *                               Please refer \ref player_att_text_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_get_att_val_text(uint8_t *remote_dev_addr,
                                      uint8_t att_id,
                                      uint8_t nbr_vals,
                                      uint8_t *p_vals,
                                      player_att_text_t *p_att_text_resp)
{
  rsi_bt_req_avrcp_get_att_val_text_t att_val_text;
  memset(&att_val_text, 0, sizeof(att_val_text));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(att_val_text.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(att_val_text.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  att_val_text.att_id   = att_id;
  att_val_text.nbr_vals = nbr_vals;
  for (ix = 0; (ix < nbr_vals) && (ix < RSI_MAX_ATT); ix++) {
    att_val_text.vals[ix] = p_vals[ix];
  }
  SL_PRINTF(SL_RSI_BT_AVRCP_GET_ATT_VAL_TEXT_TRIGGER, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_ATT_VALS_TEXT, &att_val_text, p_att_text_resp);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_batt_status(uint8_t *remote_dev_addr, uint8_t batt_level)
 * @brief      Send the device battery status to remote device. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.    
 * @param[in]  remote_dev_addr - remote device address.  
 * @param[in]  batt_level - to update battery level. 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_batt_status(uint8_t *remote_dev_addr, uint8_t batt_level)
{
  rsi_bt_req_avrcp_batt_status_t batt_status;
  memset(&batt_status, 0, sizeof(batt_status));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(batt_status.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(batt_status.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  batt_status.batt_status = batt_level;

  SL_PRINTF(SL_RSI_BT_AVRCP_BATT_STATUS_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_BATTERY_STATUS, &batt_status, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_supp_char_sets(uint8_t *remote_dev_addr, uint8_t nbr_sets, uint16_t *p_sets)
 * @brief      Provides the list of character sets supported by CT to the TG. This shall allow the TG to 
 *             send responses with strings in any of the character sets supported by CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.   
 * @param[in]  remote_dev_addr - remote device address.   
 * @param[in]  nbr_sets - Number of displayable character sets provided
 * @param[in]  p_sets - Supported Character Set  
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_supp_char_sets(uint8_t *remote_dev_addr, uint8_t nbr_sets, uint16_t *p_sets)
{
  rsi_bt_req_avrcp_char_sets_t char_sets;
  memset(&char_sets, 0, sizeof(char_sets));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(char_sets.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(char_sets.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  char_sets.char_sets.nbr_sets = nbr_sets;
  for (ix = 0; (ix < nbr_sets) && (ix < MAX_SUPP_VALS); ix++) {
    char_sets.char_sets.supp_vals[ix] = p_sets[ix];
  }

  SL_PRINTF(SL_RSI_BT_AVRCP_SUPP_CHAR_SETS_TRIGGER, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_CHAR_SETS, &char_sets, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_set_abs_vol(uint8_t *remote_dev_addr, uint8_t abs_vol, uint8_t *p_resp_abs_vol)
 * @brief      Send absolute volume information to remote device. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.    
 * @param[in]  remote_dev_addr - remote device address.  
 * @param[in]  abs_vol - absolute volume. 
 * @param[out] p_resp_abs_vol - to capture the response from this API. 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure  
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_set_abs_vol(uint8_t *remote_dev_addr, uint8_t abs_vol, uint8_t *p_resp_abs_vol)
{
  rsi_bt_avrcp_set_abs_vol_t abs_vol_req;
  memset(&abs_vol_req, 0, sizeof(abs_vol_req));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(abs_vol_req.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(abs_vol_req.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  abs_vol_req.abs_vol = abs_vol;

  SL_PRINTF(SL_RSI_BT_AVRCP_SEND_ABS_VOLUME_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_SET_ABS_VOL, &abs_vol_req, p_resp_abs_vol);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_cap_resp(uint8_t *remote_dev_addr, uint8_t cap_type, uint8_t nbr_caps, uint32_t *p_caps)
 * @brief      Sends the device capabilites Supported by TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.    
 * @param[in]  remote_dev_addr - remote device address. 
 * @param[in]  cap_type - capability type. 
 * @param[in]  nbr_caps - number of capabilities   
 * @param[in]  p_caps - capability ID's list.  
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_cap_resp(uint8_t *remote_dev_addr, uint8_t cap_type, uint8_t nbr_caps, uint32_t *p_caps)
{
  rsi_bt_avrcp_cap_resp_t cap_resp;
  memset(&cap_resp, 0, sizeof(cap_resp));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(cap_resp.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(cap_resp.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  cap_resp.cap_type = cap_type;
  cap_resp.nbr_caps = nbr_caps;
  for (ix = 0; (ix < nbr_caps) && (ix < MAX_CAPS); ix++) {
    cap_resp.caps[ix] = p_caps[ix];
  }

  SL_PRINTF(SL_RSI_BT_SEND_DEVICE_CAPABILITES_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_CAPABILITES_RESP, &cap_resp, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_att_list_resp(uint8_t *remote_dev_addr, 
 *                                                uint8_t nbr_atts, uint8_t *p_atts)
 * @brief      Sends the supported attribute lists from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.     
 * @param[in]  remote_dev_addr - remote device address. 
 * @param[in]  nbr_atts - number of attributes. 
 * @param[in]  p_atts - pointer to attributes list.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_att_list_resp(uint8_t *remote_dev_addr, uint8_t nbr_atts, uint8_t *p_atts)
{
  rsi_bt_avrcp_att_list_resp_t att_resp;
  memset(&att_resp, 0, sizeof(att_resp));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(att_resp.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(att_resp.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  att_resp.nbr_atts = nbr_atts;
  for (ix = 0; (ix < nbr_atts) && (ix < MAX_CAPS); ix++) {
    att_resp.atts[ix] = p_atts[ix];
  }

  SL_PRINTF(SL_RSI_BT_AVRCP_ATT_LIST_RESP_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_ATTS_LIST_RESP, &att_resp, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_att_val_list_resp(uint8_t *remote_dev_addr, 
 *                                                    uint8_t nbr_vals, uint8_t *p_vals)
 * @brief      Sends the attributes value list response from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.     
 * @param[in]  remote_dev_addr - remote device address.  
 * @param[in]  nbr_vals - number of values. 
 * @param[in]  p_vals - pointer to values list.  
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure  
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_att_val_list_resp(uint8_t *remote_dev_addr, uint8_t nbr_vals, uint8_t *p_vals)
{
  rsi_bt_avrcp_att_vals_list_resp_t att_vals_resp;
  memset(&att_vals_resp, 0, sizeof(att_vals_resp));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(att_vals_resp.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(att_vals_resp.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  att_vals_resp.nbr_vals = nbr_vals;
  for (ix = 0; (ix < nbr_vals) && (ix < MAX_CAPS); ix++) {
    att_vals_resp.vals[ix] = p_vals[ix];
  }

  SL_PRINTF(SL_RSI_BT_AVRCP_ATT_VAL_LIST_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_ATT_VALS_LIST_RESP, &att_vals_resp, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_cur_att_val_resp(uint8_t *remote_dev_addr,
 *                                                   uint8_t nbr_atts, att_val_t *p_att_vals)
 * @brief      Sends the current attributes values from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.      
 * @param[in]  remote_dev_addr - remote device address.  
 * @param[in]  nbr_atts - number of attributes. 
 * @param[in]  p_att_vals - pointer to attributes values list. 
 *                          Please refer \ref attr_list_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_cur_att_val_resp(uint8_t *remote_dev_addr, uint8_t nbr_atts, att_val_t *p_att_vals)
{
  rsi_bt_avrcp_cur_att_vals_resp_t att_vals_resp;
  memset(&att_vals_resp, 0, sizeof(att_vals_resp));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(att_vals_resp.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(att_vals_resp.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  att_vals_resp.att_list.nbr_atts = nbr_atts;
  for (ix = 0; (ix < nbr_atts) && (ix < MAX_ATT_VALS); ix++) {
    att_vals_resp.att_list.att_vals[ix].att_id  = p_att_vals[ix].att_id;
    att_vals_resp.att_list.att_vals[ix].att_val = p_att_vals[ix].att_val;
  }
  SL_PRINTF(SL_RSI_BT_AVRCP_CURR_ATT_VAL_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_CUR_ATT_VAL_RESP, &att_vals_resp, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_set_cur_att_val_resp(uint8_t *remote_dev_addr, uint8_t status)
 * @brief      Sends the current attributes value status response from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.       
 * @param[in]  remote_dev_addr - remote device address.  
 * @param[in]  status - status to be sent. 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_set_cur_att_val_resp(uint8_t *remote_dev_addr, uint8_t status)
{
  rsi_bt_avrcp_set_att_vals_resp_t set_att_vals_resp;
  memset(&set_att_vals_resp, 0, sizeof(set_att_vals_resp));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(set_att_vals_resp.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(set_att_vals_resp.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  set_att_vals_resp.status = status;

  SL_PRINTF(SL_RSI_BT_AVRCP_SET_CURR_ATT_VAL_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_SET_CUR_ATT_VAL_RESP, &set_att_vals_resp, NULL);
}

#define BT_AVRCP_UTF_8_CHAR_SET 0x006A

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_ele_att_resp(uint8_t *remote_dev_addr,
 *                                               uint8_t num_attrs, attr_list_t *p_attr_list)
 * @brief      Sends the Current Playing Track attributes from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.       
 * @param[in]  remote_dev_addr - remote device address. 
 * @param[in]  num_attrs - number of attributes. 
 * @param[in]  p_attr_list - pointer to attributes list. Please refer \ref attr_list_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_ele_att_resp(uint8_t *remote_dev_addr, uint8_t num_attrs, attr_list_t *p_attr_list)
{
  uint8_t ix;
  rsi_bt_avrcp_elem_attr_resp_t elem_attr;
  memset(&elem_attr, 0, sizeof(elem_attr));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(elem_attr.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(elem_attr.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  elem_attr.elem_attr_list.num_attrs = num_attrs;
  for (ix = 0; (ix < num_attrs) && (ix < MAX_ATT_LIST); ix++) {
    elem_attr.elem_attr_list.attr_list[ix].id          = p_attr_list[ix].id;
    elem_attr.elem_attr_list.attr_list[ix].char_set_id = BT_AVRCP_UTF_8_CHAR_SET;
    elem_attr.elem_attr_list.attr_list[ix].attr_len    = p_attr_list[ix].attr_len;
    memcpy(elem_attr.elem_attr_list.attr_list[ix].attr_val, p_attr_list[ix].attr_val, p_attr_list[ix].attr_len);
  }

  SL_PRINTF(SL_RSI_BT_AVRCP_ELE_ATT_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_ELEMENT_ATT_RESP, &elem_attr, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_play_status_resp(uint8_t *remote_dev_addr,
 *                                                   uint8_t play_status,
 *                                                   uint32_t song_len,
 *                                                   uint32_t song_pos)
 * @brief      Sends the Current Playing Track status from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.       
 * @param[in]  remote_dev_addr - remote device address. 
 * @param[in]  play_status - player status. 
 * @param[in]  song_len - song length. 
 * @param[in]  song_pos - song position. 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_play_status_resp(uint8_t *remote_dev_addr,
                                      uint8_t play_status,
                                      uint32_t song_len,
                                      uint32_t song_pos)
{
  rsi_bt_avrcp_play_status_resp_t player_status;
  memset(&player_status, 0, sizeof(player_status));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(player_status.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(player_status.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  player_status.play_status = play_status;
  player_status.song_len    = song_len;
  player_status.song_pos    = song_pos;

  SL_PRINTF(SL_RSI_BT_AVRCP_PLAY_STATUS_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO);

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_PLAY_STATUS_RESP, &player_status, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_reg_notify_resp(uint8_t *remote_dev_addr,
 *                                                  uint8_t event_id,
 *                                                  uint8_t event_data_len,
 *                                                  uint8_t *event_data)
 * @brief      Register notify Interim response from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.       
 * @param[in]  remote_dev_addr - remote device address.  
 * @param[in]  event_id - event ID. 
 * @param[in]  event_data_len - length of event_data buffer. 
 * @param[in]  event_data - event data. 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_reg_notify_resp(uint8_t *remote_dev_addr,
                                     uint8_t event_id,
                                     uint8_t event_data_len,
                                     uint8_t *event_data)
{
  rsi_bt_avrcp_reg_notify_interim_resp_t reg_event;
  memset(&reg_event, 0, sizeof(reg_event));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(reg_event.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(reg_event.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  reg_event.event_id = event_id;

  if (event_id == 0x02 /* EVENT_TRACK_CHANGED */)
    memcpy(reg_event.reg_notify_val.curr_track_idx, event_data, event_data_len);
  else if (event_id == 0x0d /* EVENT_VOL_CHANGED */)
    memcpy(&reg_event.reg_notify_val.abs_vol, event_data, event_data_len);
  else if (event_id == 0x01 /* AVRCP_EVENT_PLAYBACK_STATUS_CHANGED */)
    memcpy(&reg_event.reg_notify_val.play_status, event_data, event_data_len);
  else if (event_id == 0x0b) {
    reg_event.reg_notify_val.playerid = event_data[0] << 8;
    reg_event.reg_notify_val.playerid += event_data[1];
    reg_event.reg_notify_val.uidcounter = event_data[2] << 8;
    reg_event.reg_notify_val.uidcounter += event_data[3];
  } else {
  }

  SL_PRINTF(SL_RSI_BT_AVRCP_REG_NOTIFY_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_REG_NOTIFICATION_RESP, &reg_event, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_att_text_resp(uint8_t *remote_dev_addr,
 *                                                uint8_t nbr_atts, att_text_t *p_att_text)
 * @brief      Send supporting attribute text response to remote device. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.       
 * @param[in]  remote_dev_addr - remote device address.   
 * @param[in]  nbr_atts - number of attributes. 
 * @param[in]  p_att_text - pointer to attribute text. \n
 *                          Please refer \ref att_text_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_att_text_resp(uint8_t *remote_dev_addr, uint8_t nbr_atts, att_text_t *p_att_text)
{

  SL_PRINTF(SL_RSI_BT_AVRCP_ATT_TEXT_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO);
  rsi_bt_avrcp_att_text_resp_t att_text;
  memset(&att_text, 0, sizeof(att_text));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(att_text.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(att_text.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  att_text.atts_text.nbr_atts = nbr_atts;
  for (ix = 0; (ix < nbr_atts) && (ix < MAX_TEXT_LIST); ix++) {
    att_text.atts_text.list[ix].id = p_att_text[ix].id;
    memcpy(att_text.atts_text.list[ix].att_text, p_att_text[ix].att_text, strlen((char *)p_att_text[ix].att_text));
  }

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_ATT_TEXT_RESP, &att_text, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_att_vals_text_resp(uint8_t *remote_dev_addr,
 *                                                     uint8_t nbr_vals, att_text_t *p_vals_text)
 * @brief      Send supporting attribute values text response to remote device. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.       
 * @param[in]  remote_dev_addr - remote device address.  
 * @param[in]  nbr_vals - number of values. 
 * @param[in]  p_vals_text - pointer to values list. \n
 *                          Please refer \ref att_text_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 */
int32_t rsi_bt_avrcp_att_vals_text_resp(uint8_t *remote_dev_addr, uint8_t nbr_vals, att_text_t *p_vals_text)
{

  SL_PRINTF(SL_RSI_BT_AVRCP_ATT_VALS_TEXT_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO);
  rsi_bt_avrcp_att_text_resp_t vals_text;
  memset(&vals_text, 0, sizeof(vals_text));
  uint8_t ix;
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(vals_text.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(vals_text.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  vals_text.atts_text.nbr_atts = nbr_vals;
  for (ix = 0; (ix < nbr_vals) && (ix < MAX_TEXT_LIST); ix++) {
    vals_text.atts_text.list[ix].id = p_vals_text[ix].id;
    memcpy(vals_text.atts_text.list[ix].att_text, p_vals_text[ix].att_text, strlen((char *)p_vals_text[ix].att_text));
  }

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_ATT_VALS_TEXT_RESP, &vals_text, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_batt_status_resp(uint8_t *remote_dev_addr, uint8_t status)
 * @brief      Sends battery status response from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.  
 * @param[in]  remote_dev_addr - remote device address.
 * @param[in]  status - status to be sent.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_batt_status_resp(uint8_t *remote_dev_addr, uint8_t status)
{

  SL_PRINTF(SL_RSI_BT_AVRCP_BATT_STATUS_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO, "STATUS: %1x", status);
  rsi_bt_avrcp_reg_notify_resp_t batt_status;
  memset(&batt_status, 0, sizeof(batt_status));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(batt_status.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(batt_status.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  batt_status.status = status;

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_BATTERY_STATUS_RESP, &batt_status, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_char_set_status_resp(uint8_t *remote_dev_addr, uint8_t status)
 * @brief      Sends character set repsonse from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API. 
 * @param[in]  remote_dev_addr - remote device address.
 * @param[in]  status - status to be sent.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure  
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 *             
 */
int32_t rsi_bt_avrcp_char_set_status_resp(uint8_t *remote_dev_addr, uint8_t status)
{

  SL_PRINTF(SL_RSI_BT_AVRCP_CHAR_SET_STATUS_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO, "STATUS: %1x", status);
  rsi_bt_avrcp_reg_notify_resp_t char_set;
  memset(&char_set, 0, sizeof(char_set));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(char_set.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(char_set.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  char_set.status = status;

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_CHAR_SETS_RESP, &char_set, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_notify(uint8_t *remote_dev_addr, 
 *                                         uint8_t event_id, notify_val_t *p_notify_val)
 * @brief      Register notify CHANGED response from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API. 
 * @param[in]  remote_dev_addr - remote device address.  
 * @param[in]  event_id - event ID.
 * @param[in]  p_notify_val - pointer to notofication values. please refer \ref notify_val_s structure for more info.
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_notify(uint8_t *remote_dev_addr, uint8_t event_id, notify_val_t *p_notify_val)
{

  SL_PRINTF(SL_RSI_BT_AVRCP_NOTIFY_TRIGGER, BLUETOOTH, LOG_INFO);
  rsi_bt_avrcp_notify_t notify;
  memset(&notify, 0, sizeof(notify));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(notify.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(notify.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  notify.event_id = event_id;
  memcpy(&notify.notify_val, p_notify_val, sizeof(notify_val_t));

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_NOTIFICATION, &notify, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_cmd_reject(uint8_t *remote_dev_addr, uint8_t pdu_id, uint8_t status)
 * @brief      Rejects the received request from CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.  
 * @param[in]  remote_dev_addr - remote device address.  
 * @param[in]  pdu_id - PDU ID. 
 * @param[in]  status - status to be sent.  
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_cmd_reject(uint8_t *remote_dev_addr, uint8_t pdu_id, uint8_t status)
{

  SL_PRINTF(SL_RSI_BT_AVRCP_CMD_REJECT_TRIGGER, BLUETOOTH, LOG_INFO, "PDU_ID: %1x, STATUS: %1x", pdu_id, status);
  rsi_bt_avrcp_cmd_reject_t cmd_reject;
  memset(&cmd_reject, 0, sizeof(cmd_reject));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(cmd_reject.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(cmd_reject.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  cmd_reject.pdu_id = pdu_id;
  cmd_reject.status = status;

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_CMD_REJECT, &cmd_reject, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_set_abs_vol_resp(uint8_t *remote_dev_addr, uint8_t abs_vol)
 * @brief      Sends absolute volume response from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.  
 * @param[in]  remote_dev_addr - remote device address. 
 * @param[in]  abs_vol - absolute volume. 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_set_abs_vol_resp(uint8_t *remote_dev_addr, uint8_t abs_vol)
{

  SL_PRINTF(SL_RSI_BT_AVRCP_SET_ABS_VOL_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO, "VOLUME: %1x", abs_vol);
  rsi_bt_avrcp_set_abs_vol_resp_t abs_vol_resp;
  memset(&abs_vol_resp, 0, sizeof(abs_vol_resp));
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(abs_vol_resp.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(abs_vol_resp.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  abs_vol_resp.abs_vol = abs_vol;

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_SET_ABS_VOL_RESP, &abs_vol_resp, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_set_addr_player_resp(uint8_t *remote_dev_addr, uint8_t status)
 * @brief      Sends Addressed player response from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.  
 * @param[in]  remote_dev_addr - Remote device Address 
 * @param[in]  status - Status 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       This funtion can be accessed only if Media Player Selection Feature is enabled via \ref bt_avrcp_init().
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_set_addr_player_resp(uint8_t *remote_dev_addr, uint8_t status)
{

  SL_PRINTF(SL_RSI_BT_AVRCP_SET_ADDR_PLAYER_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO, "STATUS: %1x", status);
  rsi_bt_avrcp_set_addr_player_resp_t set_addr_player_resp = { 0 };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(set_addr_player_resp.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(set_addr_player_resp.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  set_addr_player_resp.status = status;

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_SET_ADDR_PLAYER_RESP, &set_addr_player_resp, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_get_folder_items_resp(uint8_t *remote_dev_addr,
 *                                                        uint8_t status,
 *                                                        folder_items_resp_t folder_items_resp)
 * @brief      Sends folder items response from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API.  
 * @param[in]  remote_dev_addr - Remote device Address 
 * @param[in]  status - Status 
 * @param[in]  folder_items_resp - Folder items, Please refer \ref folder_items_resp_s structure for more info. 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       The folder_items_resp flag holds the details of Media Player Items. \n
 *             This funtion can be accessed only if Media Player Selection Feature is enabled via \ref bt_avrcp_init().
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_get_folder_items_resp(uint8_t *remote_dev_addr,
                                           uint8_t status,
                                           folder_items_resp_t folder_items_resp)
{

  SL_PRINTF(SL_RSI_BT_AVRCP_GET_FOLDER_ITEMS_RESPONSE_TRIGGER, BLUETOOTH, LOG_INFO, "STATUS: %1x", status);
  rsi_bt_avrcp_get_folder_items_resp_t get_folder_items_resp = { 0 };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(get_folder_items_resp.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(get_folder_items_resp.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  get_folder_items_resp.status = status;
  memcpy(&get_folder_items_resp.fldr_items, &folder_items_resp, sizeof(folder_items_resp_t));
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_FOLDER_ITEMS_RESP, &get_folder_items_resp, NULL);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_bt_avrcp_get_tot_num_items_resp(uint8_t *remote_dev_addr,
 *                                                         uint8_t status,
 *                                                         uint16_t uidcntr,
 *                                                         uint32_t numofitems)
 * @brief      Sends the response for the total number of items from TG to CT. This is a blocking API.
 * @pre        Call \ref rsi_bt_avrcp_init() and \ref rsi_bt_avrcp_conn() before calling this API. 
 * @param[in]  remote_dev_addr - Remote device Address 
 * @param[in]  status - Status 
 * @param[in]  uidcntr - uid counter 
 * @param[in]  numofitems - Number of items 
 * @return     0		-	Success \n
 *             Non-Zero Value	-	Failure 
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 * @note       This funtion can be accessed only if Media Player Selection Feature is enabled via \ref bt_avrcp_init().
 * @note       CT and TG specifies Controller and Target respectively as per BT AVRCP Specification.
 */
int32_t rsi_bt_avrcp_get_tot_num_items_resp(uint8_t *remote_dev_addr,
                                            uint8_t status,
                                            uint16_t uidcntr,
                                            uint32_t numofitems)
{

  SL_PRINTF(SL_RSI_BT_AVRCP_GET_TOTAL_NUM_ITEMS_RESPONSE_TRIGGER,
            BLUETOOTH,
            LOG_INFO,
            "STATUS: %1x, UDICNTR: %2x, NUMBER_OF_ITEMS: %4x",
            status,
            uidcntr,
            numofitems);
  rsi_bt_avrcp_get_tot_num_items_resp_t get_tot_num_items_resp = { 0 };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(get_tot_num_items_resp.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(get_tot_num_items_resp.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  get_tot_num_items_resp.status     = status;
  get_tot_num_items_resp.uidcntr    = uidcntr;
  get_tot_num_items_resp.numofitems = numofitems;

  return rsi_bt_driver_send_cmd(RSI_BT_REQ_AVRCP_GET_TOT_NUM_ITEMS_RESP, &get_tot_num_items_resp, NULL);
}

#endif
/** @} */
