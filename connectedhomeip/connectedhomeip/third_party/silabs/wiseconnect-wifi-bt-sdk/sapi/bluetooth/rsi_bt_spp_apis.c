/*******************************************************************************
* @file  rsi_bt_spp_apis.c
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

/** @addtogroup BT-CLASSIC4
* @{
*/
/*==============================================*/
/**
 * @fn		   int32_t rsi_bt_spp_init(void)
 * @brief	   Set the SPP profile mode. This is a blocking API.
 * @pre		   \ref rsi_wireless_init() API needs to be called before this API. 
 * @param[in]  void
 * @return	   0		-	Success \n
 *			   Non-Zero Value	-	Failure 
 *
 * @note       Refer Error Codes section for common error codes \ref error-codes .
 *             
 */

int32_t rsi_bt_spp_init(void)
{
  rsi_bt_req_profile_mode_t bt_req_spp_init = { 0 };
  bt_req_spp_init.profile_mode              = RSI_SPP_PROFILE_BIT;
  SL_PRINTF(SL_RSI_BT_SPP_INIT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SET_PROFILE_MODE, &bt_req_spp_init, NULL);
}
/*==============================================*/
/**
 * @fn		   int32_t rsi_bt_spp_connect(uint8_t *remote_dev_addr)
 * @brief      Initiate the SPP profile level connection. This is a blocking API.
 *             A received event \ref rsi_bt_on_spp_connect_t indicates that the connection opened successfully.
 * @pre		   \ref rsi_bt_spp_init() API and \ref rsi_bt_connect() API needs to be called before this API
 * @param[in]  remote_dev_addr - Remote device address 
 * @return	   0		- 	Success \n
 *			   Non-Zero Value	-	Failure \n 
 *             0x4501 -  ERR_BT_SPP_NOT_INITIALISED \n
 *             0x4046 -  ERR_BT_INVALID_ARGS \n 
 *             0x4102 -  ERR_SDP_SRV_NOT_FOUND
 *
 *@note       Refer Error Codes section for common error codes \ref error-codes .  
 *
 */

int32_t rsi_bt_spp_connect(uint8_t *remote_dev_addr)
{
  rsi_bt_req_connect_t bt_req_spp_connect = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_spp_connect.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_spp_connect.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_SPP_CONN_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SPP_CONNECT, &bt_req_spp_connect, NULL);
}

/*==============================================*/
/**
 * @fn		   int32_t rsi_bt_spp_disconnect(uint8_t *remote_dev_addr)
 * @brief	   Initiate the SPP Profile level disconnection. This is a blocking API.
 *             A received event \ref rsi_bt_on_spp_disconnect_t indicates that the spp connection is disconnected successfully.
 * @pre		   \ref rsi_bt_spp_connect() API need to be called before this API 
 * @param[in]  remote_dev_addr - This is the remote device address   
 * @return	   0		-	Success \n
 *			   Non-Zero Value	-	Failure \n 
 *             0x4501 -  ERR_BT_SPP_NOT_INITIALISED \n
 *             0x4500 -  ERR_BT_SPP_NOT_CONNECTED
 *         
 *@note       Refer Error Codes section for common error codes \ref error-codes .  
 */

int32_t rsi_bt_spp_disconnect(uint8_t *remote_dev_addr)
{
  //This statement is added only to resolve compilation warning, value is unchanged
  USED_PARAMETER(remote_dev_addr);
  rsi_bt_req_disconnect_t bt_req_spp_disconnect = { { 0 } };
#ifdef BD_ADDR_IN_ASCII
  rsi_ascii_dev_address_to_6bytes_rev(bt_req_spp_disconnect.dev_addr, (int8_t *)remote_dev_addr);
#else
  memcpy(bt_req_spp_disconnect.dev_addr, (int8_t *)remote_dev_addr, 6);
#endif
  SL_PRINTF(SL_RSI_BT_SPP_DISCONNECT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SPP_DISCONNECT, &bt_req_spp_disconnect, NULL);
}

/*==============================================*/
/**
 * @fn		   int32_t rsi_bt_spp_transfer(uint8_t *remote_dev_addr, uint8_t *data, uint16_t length)
 * @brief	   Transfers the data through SPP profile. This is a blocking API.
 * @pre		   \ref rsi_bt_spp_connect() API needs to be called before this API. 
 * @param[in]  remote_dev_addr - This is the remote device address  
 * @param[in]  data - This is the data for transmission 
 * @param[in]  length - This is the data length for transfer, Max length supported upto 1000 bytes   
 * @return	   0		-	Success  \n
 *			   Non Zero Value	-	Failure \n 
 *             0x4501 -  ERR_BT_SPP_NOT_INITIALISED \n
 *             0x4500 -  ERR_BT_SPP_NOT_CONNECTED \n 
 *             0x4201 -  ERR_BT_RFCOMM_DISCONNECTED \n 
 *             0x4049 -  ERR_BT_SOCK_STATE_INVALID
 *
 *@note       Refer Error Codes section for common error codes \ref error-codes .  
 *             
 */

int32_t rsi_bt_spp_transfer(uint8_t *remote_dev_addr, uint8_t *data, uint16_t length)
{
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(remote_dev_addr);
  uint16_t xfer_len = 0;

  rsi_bt_req_spp_transfer_t bt_req_spp_transfer = { 0 };
  xfer_len                                      = RSI_MIN(length, RSI_BT_MAX_PAYLOAD_SIZE);
  bt_req_spp_transfer.data_length               = xfer_len;

  memcpy(bt_req_spp_transfer.data, data, xfer_len);

  SL_PRINTF(SL_RSI_BT_SPP_DISCONNECT_TRIGGER, BLUETOOTH, LOG_INFO);
  return rsi_bt_driver_send_cmd(RSI_BT_REQ_SPP_TRANSFER, &bt_req_spp_transfer, NULL);
}

#endif

/** @} */
