/*******************************************************************************
* @file  rsi_nwk.c
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

/*
  Include files
  */
#include "rsi_driver.h"
#include "rsi_wlan_non_rom.h"
#ifdef RSI_WLAN_ENABLE

extern rsi_socket_info_non_rom_t *rsi_socket_pool_non_rom;
/** @addtogroup WLAN
* @{
*/
/*==============================================*/
/**
 * @fn         int32_t rsi_driver_send_data(uint32_t sockID, uint8_t *buffer, uint32_t length, struct rsi_sockaddr *destAddr)
 * @brief      Send data packet. This is a blocking API.
 * @param[in]  sockID   - socket descriptor on which data needs to be send
 * @param[in]  buffer   - pointer to data 
 * @param[in]  length   - data length
 * @param[in]  destAddr - pointer to destination socket details 
 * @return     0              - Success \n
 *             Negative Value - Failure
 *
 *
 */
/// @private
int32_t rsi_driver_send_data(uint32_t sockID, uint8_t *buffer, uint32_t length, struct rsi_sockaddr *destAddr)
{
  return rsi_driver_send_data_non_rom(sockID, buffer, length, destAddr);
}

/*==============================================*/
/**
 * @fn         int32_t rsi_driver_process_recv_data(rsi_pkt_t *pkt)
 * @brief      This API processes received data packet. This is a non-blocking API.
 * @param[in]  pkt - pointer to data packet  
 * @return     0              - Success \n
 *             Negative Value - Failure
 */
/// @private
int32_t rsi_driver_process_recv_data(rsi_pkt_t *pkt)
{
  return rsi_driver_process_recv_data_non_rom(pkt);
}

/*====================================================*/
/**
 * @fn          int16_t rsi_nwk_register_callbacks(uint32_t callback_id,
 *                           void (*callback_handler_ptr)(uint8_t command_type, uint32_t status, const uint8_t *buffer, const uint32_t length))
 * @brief       Register the call backs. This is a non-blocking API.
 * @param[in]   callback_id          - This is the Id of the call back function \n
 *                                      Following ids are supported: \n
 *                                      0-RSI_NWK_ERROR_CB \n
 *                                      1-RSI_WLAN_NWK_URL_REQ_CB \n
 *                                      2-RSI_WLAN_NWK_JSON_UPDATE_CB \n
 *                                      3-RSI_WLAN_NWK_FW_UPGRADE_CB \n
 *                                      4-RSI_WLAN_NWK_JSON_EVENT_CB 
 * @param[in]   callback_handler_ptr - call back handler which needs to be registered for a given call back
 * @param[out]  status		     - This is the response status. For success it returns 0 and for failure it returns negative value. For possible error codes from FW refer to Error Codes section for the description of the above error codes \ref error-codes.
 * @param[out]  buffer		     - This is the response buffer.
 * @param[out]  length    	     - This is the length of the response buffer.
 * @return      0                    - Success  \n
 *              Non-Zero Value       - Failure \n
 *                                     -3 Command given in wrong state \n 
 *                                     If call_back_id is greater than the maximum callbacks to register, returns 1                
 * #### prototypes of the call back functions ####
 * callback id                  |    Prototype                            | Description         |   Parameters
 * :----------------------------|:----------------------------------------|:--------------------|:-------------------------------------------------------------
 * RSI_NWK_ERROR_CB             | void (*nwk_error_call_back_handler)(uint8_t command_type, uint32_t status, const uint8_t *buffer, const uint32_t length); | This callback is used to Register join fail| command_type => command_type of the response\n status => status of the response\n buffer => payload of the response\n length => length of the payload
 * RSI_WLAN_NWK_URL_REQ_CB      | void (*rsi_webpage_request_handler)(uint8_t type, uint8_t *url_name,uint8_t *post_content_buffer, uint32_t post_content_length, uint32_t status); | This callback is used to Register webpage request| type => type of the handler\n url_name =>url name\n post_content_buffer => Webpage content\n post_content_lenght => length of webpage content\n status => status of the response
 * RSI_WLAN_NWK_JSON_UPDATE_CB  | void (*rsi_json_object_update_handler)(uint8_t *file_name, uint8_t *json_object, uint32_t length, uint32_t status); | This callback is used to Register json update | file_name => File name \n json_object => Json object\n length: length of the json object\n status => status of the response
 * RSI_WLAN_NWK_FW_UPGRADE_CB   | void (*rsi_wireless_fw_upgrade_handler)(uint8_t type, uint32_t status); | This callback is used to Register wireless firmware upgrade | type => type of the handler\n status => status of the response
 * RSI_WLAN_NWK_JSON_EVENT_CB   | void (*rsi_json_object_event_handler)(uint32_t status, uint8_t *json_object_str, uint32_t length); | This callback is used to Register json update | status => status of the response\n json_object_str => json object string\n length => length of the string
 */

int16_t rsi_nwk_register_callbacks(
  uint32_t callback_id,
  void (*callback_handler_ptr)(uint8_t command_type, uint32_t status, const uint8_t *buffer, const uint32_t length))
{
  SL_PRINTF(SL_NWK_REGISTER_CALLBACKS_ENTRY, NETWORK, LOG_INFO);
  if (callback_id > RSI_MAX_NUM_CALLBACKS) {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    SL_PRINTF(SL_NWK_REGISTER_CALLBACKS_EXCEEDS_MAX_CALLBACKS, NETWORK, LOG_INFO);
    return RSI_ERROR_EXCEEDS_MAX_CALLBACKS;
  }
  if (callback_id == RSI_NWK_ERROR_CB) // check for NULL or not
  {
    // Register join fail call back handler
    rsi_wlan_cb_non_rom->nwk_callbacks.nwk_error_call_back_handler = callback_handler_ptr;
  }
  SL_PRINTF(SL_NWK_REGISTER_CALLBACKS_EXIT, NETWORK, LOG_INFO);
  return RSI_SUCCESS;
}
/** @} */

/** @addtogroup NETWORK14
* @{
*/
/*====================================================*/
/**
 * @fn          int16_t rsi_wlan_nwk_register_json_update_cb(uint32_t callback_id,
 *                                void (*callback_handler_ptr)(uint8_t *filename, uint8_t *json_object, uint32_t length, uint32_t status))
 * @brief       Register the call backs.This is non blocking API.
 * @param[in]   callback_id          - call back number which needs to be registered
 * @param[in]   callback_handler_ptr - call back handler which needs to be registered for a given call back
 * @param[out]	filename	     - json object filename
 * @param[out]	json_object	     - json object data
 * @param[out]	length 		     - This is the length of the response buffer.
 * @param[out]	status		     - This is the response status.
 * @return      0              - Success \n
 *              Negative Value - Failure
 *
 * @note        Please refer to Error Codes section for the description of the above error codes  \ref error-codes.
 */

int16_t rsi_wlan_nwk_register_json_update_cb(
  uint32_t callback_id,
  void (*callback_handler_ptr)(uint8_t *filename, uint8_t *json_object, uint32_t length, uint32_t status))
{
  if (callback_id > RSI_MAX_NUM_CALLBACKS) {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    return RSI_ERROR_EXCEEDS_MAX_CALLBACKS;
  }

  // Register json update handler
  rsi_wlan_cb_non_rom->nwk_callbacks.rsi_json_object_update_handler = callback_handler_ptr;

  return RSI_SUCCESS;
}
/*====================================================*/
/**
 * @fn          int16_t rsi_wlan_nwk_register_json_event_cb(uint32_t callback_id,
                                            void (*callback_handler_ptr)(uint32_t status,
                                                                         uint8_t *json_object,
                                                                         uint32_t length))
 * @brief       Register the event call backs.This is non blocking API.
 * @param[in]   callback_id          - call back number which needs to be registered
 * @param[in]   callback_handler_ptr - call back handler which needs to be registered for a given call back
 * @param[out]	status		     - This is the response status.
 * @param[out]	json_object	     - json object data 
 * @param[out]	length		     - This is the length of the response buffer.
 * @return      0              - Success \n
 *              Positive Value - Failure
 */

int16_t rsi_wlan_nwk_register_json_event_cb(uint32_t callback_id,
                                            void (*callback_handler_ptr)(uint32_t status,
                                                                         uint8_t *json_object,
                                                                         uint32_t length))
{
  if (callback_id > RSI_MAX_NUM_CALLBACKS) {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    return RSI_ERROR_EXCEEDS_MAX_CALLBACKS;
  }

  // Register json update handler
  rsi_wlan_cb_non_rom->nwk_callbacks.rsi_json_object_event_handler = callback_handler_ptr;

  return RSI_SUCCESS;
}
/*====================================================*/
/**
 * @fn          int16_t rsi_wlan_nwk_register_webpage_req_cb(uint32_t callback_id,
                                             void (*callback_handler_ptr)(uint8_t type,
                                                                          uint8_t *url_name,
                                                                          uint8_t *post_content_buffer,
                                                                          uint32_t post_content_length,
                                                                          uint32_t status))
 * @brief       Register webpage request callbacks.This is non blocking API.
 * @param[in]   callback_id          - call back number which needs to be registered
 * @param[in]   callback_handler_ptr - call back handler which needs to be registered for a given call back
 * @param[out]	type		     - command type 
 * @param[out]	url_name   	     - URL address of the domain
 * @param[out]	post_content_buffer  - This is the response buffer.
 * @param[out]	post_content_length  - This is the length of the response buffer.
 * @param[out]	status		     - This is the response status.
 * @return      0              - Success \n
 *              Positive Value - Failure
 */

int16_t rsi_wlan_nwk_register_webpage_req_cb(uint32_t callback_id,
                                             void (*callback_handler_ptr)(uint8_t type,
                                                                          uint8_t *url_name,
                                                                          uint8_t *post_content_buffer,
                                                                          uint32_t post_content_length,
                                                                          uint32_t status))
{
  if (callback_id > RSI_MAX_NUM_CALLBACKS) {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    return RSI_ERROR_EXCEEDS_MAX_CALLBACKS;
  }

  // Register webpage request handler
  rsi_wlan_cb_non_rom->nwk_callbacks.rsi_webpage_request_handler = callback_handler_ptr;

  return RSI_SUCCESS;
}
/*====================================================*/
/**
 * @fn          int16_t rsi_wlan_nwk_register_wireless_fw_upgrade_cb(uint32_t callback_id,
                                                     void (*callback_handler_ptr)(uint8_t type, uint32_t status))
 * @brief       Register wireless firmware upgrade callbacks.This is non blocking API.
 * @param[in]   callback_id          - call back number which needs to be registered
 * @param[in]   callback_handler_ptr - call back handler which needs to be registered for a given call back
 * @param[out]	type		          - command type 
 * @param[out]	status		          - This is the response status.
 * @return      0                     - Success \n
 *              Positive Value        - Failure
 */
int16_t rsi_wlan_nwk_register_wireless_fw_upgrade_cb(uint32_t callback_id,
                                                     void (*callback_handler_ptr)(uint8_t type, uint32_t status))
{
  if (callback_id > RSI_MAX_NUM_CALLBACKS) {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    return RSI_ERROR_EXCEEDS_MAX_CALLBACKS;
  }

  // Register wireless firmware upgrade call back handler
  rsi_wlan_cb_non_rom->nwk_callbacks.rsi_wireless_fw_upgrade_handler = callback_handler_ptr;

  return RSI_SUCCESS;
}
/** @} */
/** @addtogroup DRIVER9
* @{
*/
/*==============================================*/
/**
 * @fn          rsi_error_t rsi_wait_on_nwk_semaphore(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms) 
 * @brief       Wireless library to acquire or wait for nwk semaphore.
 * @param[in]   semaphore  - Semaphore handle pointer  
 * @param[in]   timeout_ms - Maximum time to wait to acquire semaphore. If timeout_ms is 0 then wait \n
                             till acquire semaphore.
 * @return      0              - Success \n
 *              Negative Value - Failure
 */
/// @private
rsi_error_t rsi_wait_on_nwk_semaphore(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms)
{
  // Wait on wlan semaphore
  if (rsi_semaphore_wait(semaphore, timeout_ms) != RSI_ERROR_NONE) {
    rsi_wlan_set_nwk_status(RSI_ERROR_RESPONSE_TIMEOUT);
#ifndef RSI_WAIT_TIMEOUT_EVENT_HANDLE_TIMER_DISABLE
    if (rsi_driver_cb_non_rom->rsi_wait_timeout_handler_error_cb != NULL) {
      rsi_driver_cb_non_rom->rsi_wait_timeout_handler_error_cb(RSI_ERROR_RESPONSE_TIMEOUT, NWK_CMD);
    }
#endif
    return RSI_ERROR_RESPONSE_TIMEOUT;
  }
  return RSI_ERROR_NONE;
}
/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_get_nwk_status(void)
 * @brief       Return network status.
 * @param[in]   void 
 * @return      0              - Success \n
 *              Non-Zero Value - Failure
 *
 */

int32_t rsi_wlan_get_nwk_status(void)
{
  return rsi_driver_cb_non_rom->nwk_status;
}

/*==============================================*/
/**
 * @fn          void rsi_wlan_set_nwk_status(int32_t status)
 * @brief       Set network status
 * @param[in]   status - status value to be set 
 * @return      void
 */
/// @private
void rsi_wlan_set_nwk_status(int32_t status)
{
  rsi_driver_cb_non_rom->nwk_status = status;
#ifndef RSI_WLAN_STATUS
  rsi_wlan_set_status(status);
#endif
}

/*==============================================*/
/**
 * @fn         int32_t rsi_driver_send_data_non_rom(uint32_t sockID, uint8_t *buffer, uint32_t length, struct rsi_sockaddr *destAddr)
 * @brief      Send data packet
 * @param[in]  sockID   - socket descriptor on which data needs to be send
 * @param[in]  buffer   - pointer to data 
 * @param[in]  length   - data length
 * @param[in]  destAddr - pointer to destination socket details 
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 *
 */
/// @private
int32_t rsi_driver_send_data_non_rom(uint32_t sockID, uint8_t *buffer, uint32_t length, struct rsi_sockaddr *destAddr)
{
  int32_t status = RSI_SUCCESS;
  uint8_t *host_desc;
  uint32_t total_send_size = 0;
  uint16_t headerLen;
  rsi_req_socket_send_t *send;
  rsi_pkt_t *pkt;
  rsi_driver_cb_t *rsi_driver_cb     = global_cb_p->rsi_driver_cb;
  rsi_socket_info_t *rsi_socket_pool = global_cb_p->rsi_socket_pool;

  // Allocate packet to send data
  pkt = rsi_pkt_alloc(&rsi_driver_cb->wlan_cb->wlan_tx_pool);

  if (pkt == NULL) {
    // Set error
    rsi_wlan_socket_set_status(RSI_ERROR_PKT_ALLOCATION_FAILURE, sockID);
    return RSI_SOCK_ERROR;
  }

  if (rsi_socket_pool[sockID].sock_state != RSI_SOCKET_STATE_CONNECTED) {
    rsi_pkt_free(&rsi_driver_cb->wlan_cb->wlan_tx_pool, pkt);
    // Set error
    rsi_wlan_socket_set_status(RSI_ERROR_EBADF, sockID);
    return RSI_SOCK_ERROR;
  }

  // Get data pointer
  send = (rsi_req_socket_send_t *)pkt->data;

  // Get host descriptor pointer
  host_desc = pkt->desc;

  // Memset host descriptor
  memset(host_desc, 0, RSI_HOST_DESC_LENGTH);

  // Memset before filling
  memset(send, 0, (sizeof(rsi_req_socket_send_t) - sizeof(send->send_buffer)));

  if (((rsi_socket_pool[sockID].sock_type >> 4) & 0xF) == AF_INET) {
    // Fill IP version as 4
    rsi_uint16_to_2bytes(send->ip_version, 4);

    // Fill destination IPv4 address
    memcpy((uint8_t *)send->dest_ip_addr.ipv4_address,
           (uint8_t *)ntohl(&(((struct rsi_sockaddr_in *)destAddr)->sin_addr.s_addr)),
           RSI_IPV4_ADDRESS_LENGTH);

    // Fill destination port
    rsi_uint16_to_2bytes(send->dest_port, ntohs(((struct rsi_sockaddr_in *)destAddr)->sin_port));

    // Calculate header length
    if ((rsi_socket_pool[sockID].sock_type & 0xF) == SOCK_STREAM) {
      // TCP IPv4 header
      headerLen = RSI_TCP_FRAME_HEADER_LEN;
    } else {
      // UDP IPv4 header
      headerLen = RSI_UDP_FRAME_HEADER_LEN;
    }
  } else {
    // Fill IP version as 6
    rsi_uint16_to_2bytes(send->ip_version, 6);

    // Fill destination IPv6 address
    memcpy(send->dest_ip_addr.ipv6_address,
           (uint8_t *)ntohl(&((struct rsi_sockaddr_in6 *)destAddr)->sin6_addr._S6_un._S6_u8),
           RSI_IPV6_ADDRESS_LENGTH);

    // Fill destination port
    rsi_uint16_to_2bytes(send->dest_port, ntohs(((struct rsi_sockaddr_in6 *)destAddr)->sin6_port));

    // Calculate header length
    if ((rsi_socket_pool[sockID].sock_type & 0xF) == SOCK_STREAM) {
      // TCP IPv6 header length
      headerLen = RSI_TCP_V6_FRAME_HEADER_LEN;
    } else {
      // UDP IPv6 header length
      headerLen = RSI_UDP_V6_FRAME_HEADER_LEN;
    }
  }

  // if it is a web socket, setting web socket data header length
  if (rsi_socket_pool[sockID].sock_bitmap & RSI_SOCKET_FEAT_WEBS_SUPPORT) {
    // If websocket is enabled
    headerLen += ((length < 126) ? 6 : 8);
  }

  // Fill socket desriptor
  send->socket_id[0] = rsi_socket_pool[sockID].sock_id;

  // Fill opcode used in websocket
  send->socket_id[1] = rsi_socket_pool[sockID].opcode;

  // Fill send buffer length
  rsi_uint16_to_2bytes(send->length, length);

  // Fill data offset
  rsi_uint16_to_2bytes(send->data_offset, headerLen);

  // Total send length includes header length and payload length
  total_send_size = (headerLen + length);

  headerLen -= (sizeof(rsi_req_socket_send_t) - sizeof(send->send_buffer));

  // Copy data to be sent
  memcpy(&send->send_buffer[headerLen], buffer, length);

  // Fill host descriptor
  rsi_uint16_to_2bytes(host_desc, (total_send_size & 0xFFF));

  // Fill packet type
  host_desc[1] |= (RSI_WLAN_DATA_Q << 4);

  // Enqueue packet to WLAN TX queue
  rsi_enqueue_pkt(&rsi_driver_cb->wlan_tx_q, pkt);

  // Set TX packet pending event
  rsi_set_event(RSI_TX_EVENT);

#ifdef RSI_UART_INTERFACE
#ifndef RSI_SEND_SEM_BITMAP
  rsi_driver_cb_non_rom->send_wait_bitmap |= BIT(0);
#endif
  if (rsi_wait_on_socket_semaphore(&rsi_driver_cb_non_rom->send_data_sem, RSI_SEND_DATA_RESPONSE_WAIT_TIME)
      != RSI_ERROR_NONE) {
    rsi_wlan_socket_set_status(RSI_ERROR_RESPONSE_TIMEOUT, sockID);

    return RSI_SOCK_ERROR;
  }
#else
#ifndef RSI_SOCK_SEM_BITMAP
  rsi_socket_pool_non_rom[sockID].socket_wait_bitmap |= BIT(2);
#endif
  status =
    rsi_wait_on_socket_semaphore(&rsi_socket_pool_non_rom[sockID].sock_send_sem, RSI_SEND_DATA_RESPONSE_WAIT_TIME);
  if (status != RSI_ERROR_NONE) {
    // get wlan/network command response status
    rsi_wlan_socket_set_status(status, sockID);
    return RSI_SOCK_ERROR;
  }
#endif

  status = rsi_wlan_socket_get_status(sockID);

  // Return status
  return status;
}
/*==============================================*/
/**
 * @fn         int32_t rsi_driver_process_recv_data_non_rom(rsi_pkt_t *pkt)
 * @brief      Process data receive packet. 
 * @param[in]  pkt - pointer to data packet  
 * @return     0              - Success \n
 *             Non-Zero Value - Failure
 */

int32_t rsi_driver_process_recv_data_non_rom(rsi_pkt_t *pkt)
{
  uint32_t data_length, data_offset;
  int32_t sockID;
  uint8_t websocket_info = 0;
  rsi_socket_info_t *sock_info;
  rsi_rsp_socket_recv_t *recv;
  uint8_t *host_desc;
  uint16_t payload_length;
  int32_t frame_type = 0;
  int32_t status     = RSI_SUCCESS;

  //  rsi_driver_cb_t   *rsi_driver_cb   = global_cb_p->rsi_driver_cb;
  rsi_socket_info_t *rsi_socket_pool = global_cb_p->rsi_socket_pool;

  // Get payload pointer
  recv = (rsi_rsp_socket_recv_t *)pkt->data;

  // Get host descriptor
  host_desc = pkt->desc;

  frame_type = host_desc[2];

  // Get payoad length
  payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);

  // check status
  status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

  if (frame_type == 0x01) {
    rsi_wlan_cb_non_rom->callback_list.raw_data_receive_handler(0, pkt->data, payload_length);
  } else {
    // Get socket descriptor
    sockID = rsi_application_socket_descriptor(recv->socket_id[0]);

    // update the status in wlan_cb
    rsi_wlan_socket_set_status(status, sockID);

    // Get web socket information
    websocket_info = recv->socket_id[1];

    if ((sockID < 0) || (sockID >= NUMBER_OF_SOCKETS)) {
      return RSI_SOCK_ERROR;
    }

    // Get socket info
    sock_info = &rsi_socket_pool[sockID];

    // Get received data length
    data_length = rsi_bytes2R_to_uint16(recv->length);

    // Get actual data offset
    data_offset = rsi_bytes2R_to_uint16(recv->offset);

    if ((sock_info->sock_type & 0xF) == SOCK_DGRAM) {
      // Save destination port number
      sock_info->destination_port = rsi_bytes2R_to_uint16(recv->dest_port);

      // Save destination IP address
      if ((sock_info->sock_type >> 4) == AF_INET) {
        memcpy(sock_info->destination_ip_addr.ipv4, recv->dest_ip_addr.ipv4_address, RSI_IPV4_ADDRESS_LENGTH);
      } else {
        memcpy(sock_info->destination_ip_addr.ipv6, recv->dest_ip_addr.ipv6_address, RSI_IPV6_ADDRESS_LENGTH);
      }
    }

    if (!(sock_info->sock_bitmap & RSI_SOCKET_FEAT_SYNCHRONOUS)) {

      if (sock_info->sock_receive_callback != NULL) {
        if ((sock_info->sock_bitmap & RSI_SOCKET_FEAT_WEBS_SUPPORT)) {
          *(uint8_t *)(recv + data_offset - 1) = websocket_info;

          // Call registered callback if asynchronous callback
          sock_info->sock_receive_callback(sockID, ((uint8_t *)recv + data_offset - 1), data_length + 1);
        } else {
          // Call registered callback if asynchronous callback
          sock_info->sock_receive_callback(sockID, ((uint8_t *)recv + data_offset), data_length);
        }
      }
      return RSI_SUCCESS;
    }

    if (global_cb_p->rx_buffer_mem_copy == 1) {
      // If socket receive available length is still present
      if (sock_info->sock_recv_available_length != 0) {
        return RSI_SOCK_ERROR;
      }

      // Memcopy to socket receive buffer
      sock_info->sock_recv_buffer = ((uint8_t *)recv + data_offset);

      // Increase socket recieve available length
      sock_info->sock_recv_available_length = data_length;
    } else {
      if (sock_info->recv_buffer) {
#ifdef RSI_PROCESS_MAX_RX_DATA
        if (sock_info->sock_type & SOCK_STREAM) {
          // Memcopy to socket receive buffer
          memcpy((sock_info->recv_buffer + sock_info->recv_buffer_length),
                 ((uint8_t *)recv + data_offset),
                 data_length);
          sock_info->recv_buffer_length += data_length;
        } else {
          // Memcopy to socket receive buffer
          memcpy(sock_info->recv_buffer, ((uint8_t *)recv + data_offset), data_length);
          sock_info->recv_buffer_length = data_length;
        }
#else
        // Memcopy to socket receive buffer
        memcpy(sock_info->recv_buffer, ((uint8_t *)recv + data_offset), data_length);
        sock_info->recv_buffer_length = data_length;
#endif
      } else {
        sock_info->recv_buffer_length = 0;
      }
    }

#ifdef RSI_PROCESS_MAX_RX_DATA
    if (sock_info->sock_type & SOCK_STREAM) {
      if (recv->more_data_pending) {
        rsi_socket_pool_non_rom[sockID].more_rx_data_pending = 1;
      }
    }
    if (recv->end_of_the_data || !(sock_info->sock_type & SOCK_STREAM))
#endif
    {
#ifndef RSI_SOCK_SEM_BITMAP
      rsi_socket_pool_non_rom[sockID].socket_wait_bitmap &= ~BIT(1);
#endif
      rsi_socket_pool_non_rom[sockID].recv_pending_bit |= BIT(0);
      // signal the wlan semaphore
      RSI_SEMAPHORE_POST(&rsi_socket_pool_non_rom[sockID].sock_recv_sem);
    }
  }
  return RSI_SUCCESS;
}
/*==============================================*/
/**
 * @fn          void rsi_post_waiting_nwk_semaphore()
 * @brief       Posting of the network semaphores which are on wait
 * @param[in]   void  
 * @return      void
 */
/// @private
#ifndef RSI_NWK_SEM_BITMAP
void rsi_post_waiting_nwk_semaphore()
{
  if (rsi_driver_cb_non_rom->nwk_wait_bitmap & BIT(0)) {
    rsi_semaphore_post(&rsi_driver_cb_non_rom->nwk_sem);
  }
  if (rsi_driver_cb_non_rom->nwk_wait_bitmap & BIT(1)) {
    rsi_semaphore_post(&rsi_driver_cb_non_rom->nwk_cmd_send_sem);
  }
  rsi_driver_cb_non_rom->nwk_wait_bitmap = 0;
}
#else
void rsi_post_waiting_nwk_semaphore()
{
  rsi_semaphore_post(&rsi_driver_cb_non_rom->nwk_sem);
  rsi_semaphore_post(&rsi_driver_cb_non_rom->nwk_cmd_send_sem);
}
#endif
#endif
/** @} */
