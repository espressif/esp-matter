/*******************************************************************************
* @file  rsi_pop3_client.c
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

#include "rsi_pop3_client.h"

/** @addtogroup NETWORK20
* @{
*/
/*==============================================*/
/**
 * @note        Currently API is not supported.
 * @brief       Create POP3 client session. This is a non-blocking API.
 * @pre         \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]   flags                                 - Select the IP version. \n
 *                          BIT(0)                    – RSI_IPV6. Set this bit to enable IPv6, by default it is configured to IPv4
 * @param[in]   server_ip_address                     - POP3 server IP address
 * @param[in]   server_port_number                    - POP3 server TCP port
 * @param[in]   auth_type                             - Client authentication type
 * @param[in]   username                              - Username for authentication. It must be a NULL terminated string
 * @param[in]   password                              - Password for authentication. It must be a NULL terminated string
 * @param[in]  rsi_pop3_client_mail_response_handler  - Callback when asynchronous response comes for the session create. \n
 *                                                      The callback parameters are: status,type, and buffer \n
 *@param[out]   status                                - Status code
 *@param[out]   type                                  - Sub-command type
 *@param[out]   buffer                                - Buffer pointer
 *@return       Zero                                  - Success \n
 *              Non-Zero Value                        -  If return value is less than 0 \n
 *                              -3                    : Command given in wrong state \n
 *                              -4                    : Buffer not available to serve the command \n
 * \n                             
 *                                                      If return value is greater than 0 \n
 *                                                      0x0021,0x0015,0xBB87,0xff74
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_pop3_session_create_async(uint8_t flags,
                                      uint8_t *server_ip_address,
                                      uint16_t server_port_number,
                                      uint8_t auth_type,
                                      uint8_t *username,
                                      uint8_t *password,
                                      void (*rsi_pop3_client_mail_response_handler)(uint16_t status,
                                                                                    uint8_t type,
                                                                                    const uint8_t *buffer))
{
  rsi_req_pop3_client_t *pop3_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint8_t *host_desc = NULL;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Register callback
    if (rsi_pop3_client_mail_response_handler != NULL) {
      // Register POP3 client response, notify callback handler
      rsi_wlan_cb_non_rom->nwk_callbacks.rsi_pop3_client_mail_response_handler = rsi_pop3_client_mail_response_handler;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return invalid command error
      return RSI_ERROR_INVALID_PARAM;
    }
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    pop3_client = (rsi_req_pop3_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_pop3_client_t));

    // Fill command type
    pop3_client->command_type = POP3_CLIENT_SESSION_CREATE;

    if (!(flags & RSI_IPV6)) {
      // Fill the IP version
      pop3_client->pop3_struct.pop3_client_session_create.ip_version = RSI_IP_VERSION_4;

      // Set IP address to localhost
      memcpy(pop3_client->pop3_struct.pop3_client_session_create.server_ip_address.ipv4_address,
             server_ip_address,
             RSI_IPV4_ADDRESS_LENGTH);
    } else {
      pop3_client->pop3_struct.pop3_client_session_create.ip_version = RSI_IP_VERSION_6;

      // Set IP address to localhost
      memcpy(pop3_client->pop3_struct.pop3_client_session_create.server_ip_address.ipv6_address,
             server_ip_address,
             RSI_IPV6_ADDRESS_LENGTH);
    }

    // Fill POP3 server port number
    rsi_uint16_to_2bytes(pop3_client->pop3_struct.pop3_client_session_create.server_port_number, server_port_number);

    // Fill authentication type
    pop3_client->pop3_struct.pop3_client_session_create.auth_type = auth_type;

    // Fill username
    rsi_strcpy(pop3_client->pop3_struct.pop3_client_session_create.username, username);

    // Fill password
    rsi_strcpy(pop3_client->pop3_struct.pop3_client_session_create.password, password);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (sizeof(rsi_req_pop3_client_t) & 0xFFF));

    // Send POP3 client session create request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_POP3_CLIENT, pkt);

  } else {
    // Return NWK command error
    return status;
  }

  // Return status
  return status;
}

/*==============================================*/
/**
 * @note       Currently API is not supported.
 * @brief      Delete POP3 client session. This is a non-blocking API.
 * @pre        \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  Void
 * @return     Zero            -  Success \n
 *             Non-Zero Value  -  If return value is less than 0 \n
 *                                -3 : Command given in wrong state \n
 *                                -4 : Buffer not available to serve the command \n
 *\n                                
 *                                     If return value is greater than 0 \n
 *                                     0x0021,0xFF74,0xBB87
 * @note       Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_pop3_session_delete(void)
{

  rsi_req_pop3_client_t *pop3_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint8_t *host_desc = NULL;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    pop3_client = (rsi_req_pop3_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_pop3_client_t));

    // Fill command type
    pop3_client->command_type = POP3_CLIENT_SESSION_DELETE;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (sizeof(rsi_req_pop3_client_t) & 0xFFF));

    // Send POP3 client session create request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_POP3_CLIENT, pkt);

  } else {
    // Return NWK command error
    return status;
  }

  // Return status
  return status;
}

/*==============================================*/
/**
 * @note        Currently API is not supported.
 * @brief       Get mail stats. This is a non-blocking API.
 * @pre         \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]   Void
 * @return      Zero              -  Success \n
 *              Non-Zero Value    -  If return value is less than 0 \n
 *                             -3 : Command given in wrong state \n
 *                             -4 : Buffer not available to serve the command \n
 *\n                             
 *                                  If return value is greater than 0 \n
 *                                  0x0021,0xFF74,0xBB87
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_pop3_get_mail_stats(void)
{

  rsi_req_pop3_client_t *pop3_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint8_t *host_desc = NULL;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    pop3_client = (rsi_req_pop3_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_pop3_client_t));

    // Fill command type
    pop3_client->command_type = POP3_CLIENT_GET_MAIL_STATS;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (sizeof(rsi_req_pop3_client_t) & 0xFFF));

    // Send POP3 client session create request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_POP3_CLIENT, pkt);

  } else {
    // Return NWK command error
    return status;
  }

  // Return status
  return status;
}

/*==============================================*/
/**
 * @note        Currently API is not supported. 
 * @brief       Get the size of the mail for the passed mail index. This is a blocking API.
 * @pre         \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]   mail_index       - Mail index to get the size of the mail
 * @return      Zero             - Success \n
 *              Non-Zero Value   - If return value is less than 0 \n
 *                            -3 : Command given in wrong state \n
 *                            -4 : Buffer not available to serve the command \n
 *\n                            
 *                                 If return value is greater than 0 \n
 *                                 0x0021,0xBB87,0xFF74,0xBBFF
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes. \n
 *              
 */

int32_t rsi_pop3_get_mail_list(uint16_t mail_index)
{

  rsi_req_pop3_client_t *pop3_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint8_t *host_desc = NULL;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    pop3_client = (rsi_req_pop3_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_pop3_client_t));

    // Fill command type
    pop3_client->command_type = POP3_CLIENT_GET_MAIL_LIST;

    // Fill POP3 mail index number
    rsi_uint16_to_2bytes(pop3_client->pop3_struct.pop3_client_mail_index, mail_index);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (sizeof(rsi_req_pop3_client_t) & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

    // Send POP3 client session create request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_POP3_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_POP3_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    return status;
  }

  // Return status
  return status;
}

/*==============================================*/
/**
 * @note          Currently API is not supported.
 * @brief         Retrieve mail content for the passed mail index. This is a non-blocking API.
 * @pre           \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]     mail_index       - Mail index to get the mail content for the passed index
 * @return        Zero             - Success \n
 *                Non-Zero Value   - If return value is less than 0 \n
 *                              -3 : Command given in wrong state \n
 *                              -4 : Buffer not available to serve the command \n
 *\n                              
 *                                   If return value is greater than 0 \n
 *                                   0x0021,0xBB87,0xFF74,0xBBFF,0xBBC5
 * @note          Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_pop3_retrive_mail(uint16_t mail_index)
{

  rsi_req_pop3_client_t *pop3_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint8_t *host_desc = NULL;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    pop3_client = (rsi_req_pop3_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_pop3_client_t));

    // Fill command type
    pop3_client->command_type = POP3_CLIENT_RETR_MAIL;

    // Fill POP3 mail index number
    rsi_uint16_to_2bytes(pop3_client->pop3_struct.pop3_client_mail_index, mail_index);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (sizeof(rsi_req_pop3_client_t) & 0xFFF));

    // Send POP3 client session create request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_POP3_CLIENT, pkt);

  } else {
    // Return NWK command error
    return status;
  }

  // Return status
  return status;
}

/*==============================================*/
/**
 * @note        Currently API is not supported.
 * @brief       Mark a mail as deleted for the passed mail index.This is a non-blocking API.
 * @pre         \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]   mail_index       - Mail index to mark the mail as deleted
 * @return      Zero             - Success \n
 *              Non-Zero Value   - If return value is less than 0 \n
 *                            -3 : Command given in wrong state \n
 *                            -4 : Buffer not available to serve the command \n
 *\n                            
 *                                 If return value is greater than 0 \n
 *                                 0x0021,0xFF74,0xBB87,0xBBFF
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_pop3_mark_mail(uint16_t mail_index)
{

  rsi_req_pop3_client_t *pop3_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint8_t *host_desc = NULL;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    pop3_client = (rsi_req_pop3_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_pop3_client_t));

    // Fill command type
    pop3_client->command_type = POP3_CLIENT_MARK_MAIL;

    // Fill POP3 mail index number
    rsi_uint16_to_2bytes(pop3_client->pop3_struct.pop3_client_mail_index, mail_index);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (sizeof(rsi_req_pop3_client_t) & 0xFFF));

    // Send POP3 client session create request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_POP3_CLIENT, pkt);

  } else {
    // Return NWK command error
    return status;
  }

  // Return status
  return status;
}

/*==============================================*/
/**
 * @note       Currently API is not supported.
 * @brief      Unmark all the marked (deleted) mails in the current session. This is a non-blocking API.
 * @pre        \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  void 
 * @return     Zero             -  Success \n
 *             Non-Zero Value   -  If return value is less than 0 \n
 *                           -3 : Command given in wrong state \n
 *                           -4 : Buffer not available to serve the command \n
 *\n                           
 *                                If return value is greater than 0 \n
 *                                0x0021,0xFF74,0xBB87	
 * @note       Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_pop3_unmark_mail(void)
{

  rsi_req_pop3_client_t *pop3_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint8_t *host_desc = NULL;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    pop3_client = (rsi_req_pop3_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_pop3_client_t));

    // Fill command type
    pop3_client->command_type = POP3_CLIENT_UNMARK_MAIL;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (sizeof(rsi_req_pop3_client_t) & 0xFFF));

    // Send POP3 client session create request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_POP3_CLIENT, pkt);

  } else {
    // Return NWK command error
    return status;
  }

  // Return status
  return status;
}

/*==============================================*/
/**
 * @note        Currently API is not supported.
 * @brief        Get the POP3 server status. This is a non-blocking API.
 * @pre          \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]    void 
 * @return       Zero              -  Success \n
 *               Non-Zero Value    -  If return value is less than 0 \n
 *                              -3 : Command given in wrong state \n
 *                              -4 : Buffer not available to serve the command \n
 *\n                              
 *                                   If return value is greater than 0 \n
 *                                   0x0021,0xBB87,0xFF74
 * @note         Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_pop3_get_server_status(void)
{

  rsi_req_pop3_client_t *pop3_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint8_t *host_desc = NULL;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    pop3_client = (rsi_req_pop3_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_pop3_client_t));

    // Fill command type
    pop3_client->command_type = POP3_CLIENT_GET_SERVER_STATUS;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (sizeof(rsi_req_pop3_client_t) & 0xFFF));

    // Send POP3 client session create request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_POP3_CLIENT, pkt);

  } else {
    // Return NWK command error
    return status;
  }

  // Return status
  return status;
}
/** @} */
