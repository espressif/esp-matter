/*******************************************************************************
* @file  rsi_smtp_client.c
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

#include "rsi_smtp_client.h"
/** @addtogroup NETWORK11
* @{
*/
/*==============================================*/
/**
 * @note        This API is not supported in current release.
 * @brief       Create an SMTP client. nitialize the client with a given configuration.
 *              This is a blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]   flags         - Select IPV4/IPv6 version  \n
 *   
 *  Flags                  |          Description
 *  :----------------------|:-----------------------------------------------------------------------
 *  BIT(0) - RSI_IPV6      |    Set this bit to enable IPv6. Configured to IPv4 by default
 *  BIT(1) to BIT(7)       |    Reserved for future use
 * 
 * @param[in]   username      - Username for authentication, must be a NULL terminated string
 * @param[in]   password      - Password for authentication, must be a NULL terminated string
 * @param[in]   from_address  - Sender's address, must be a NULL terminated string
 * @param[in]   client_domain - Domain name of the client, must be a NULL terminated string
 * @param[in]   auth_type     - Client authentication type \n
 * 							                 1 - SMTP_CLIENT_AUTH_LOGIN \n
 *                               3 - SMTP_CLIENT_AUTH_PLAIN
 * @param[in]   server_ip     - SMTP server IP address \n
 *                               IPv4 address - 4 Bytes hexa-decimal \n
 *                               IPv6 address - 16 Bytes hexa-decimal
 * @param[in]   port          - SMTP server TCP port
 * @return     0              -  Success \n
 *             Negative Value - Failure \n
 *             -3             - Command given in wrong state \n
 *             -4             - Buffer not available to serve the command
 */

int32_t rsi_smtp_client_create(uint8_t flags,
                               uint8_t *username,
                               uint8_t *password,
                               uint8_t *from_address,
                               uint8_t *client_domain,
                               uint8_t auth_type,
                               uint8_t *server_ip,
                               uint32_t port)
{
  rsi_req_smtp_client_t *smtp_client;
  rsi_pkt_t *pkt;
  int32_t status       = RSI_SUCCESS;
  uint16_t smtp_length = 0;
  uint16_t send_size   = 0;
  uint8_t *host_desc   = NULL;
  SL_PRINTF(SL_SMTP_CLIENT_CREATE_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_SMTP_CLIENT_CREATE_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_SMTP_CLIENT_CREATE_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if ((auth_type != RSI_SMTP_CLIENT_AUTH_LOGIN) && (auth_type != RSI_SMTP_CLIENT_AUTH_PLAIN)) {
    // Return invalid command error
    SL_PRINTF(SL_SMTP_CLIENT_CREATE_INVALID_PARAM, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
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
      SL_PRINTF(SL_SMTP_CLIENT_CREATE_PKT_ALLOCATION_FAILURE_1, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    smtp_client = (rsi_req_smtp_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_smtp_client_t));

    // Fill command type
    smtp_client->command_type = RSI_SMTP_CLIENT_CREATE;

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_req_smtp_client_t) - RSI_SMTP_BUFFER_LENGTH;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send HTTP Get request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SMTP_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_SMTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    if (status != RSI_SUCCESS) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return status
      SL_PRINTF(SL_SMTP_CLIENT_CREATE_EXIT_1, NETWORK, LOG_INFO, "status: %4x", status);
      return status;
    }

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_SMTP_CLIENT_CREATE_PKT_ALLOCATION_FAILURE_2, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_smtp_client_t));

    // Fill command type
    smtp_client->command_type = RSI_SMTP_CLIENT_INIT;

    // Fill the authentication type
    smtp_client->smtp_struct.smtp_client_init.auth_type = auth_type;

    if (!(flags & RSI_IPV6)) {
      // Fill the IP version
      smtp_client->smtp_struct.smtp_client_init.ip_version = RSI_IP_VERSION_4;

      // Set IP address to localhost
      memcpy(smtp_client->smtp_struct.smtp_client_init.server_ip_address.ipv4_address,
             server_ip,
             RSI_IPV4_ADDRESS_LENGTH);
    } else {
      smtp_client->smtp_struct.smtp_client_init.ip_version = RSI_IP_VERSION_6;

      // Set IP address to localhost
      memcpy(smtp_client->smtp_struct.smtp_client_init.server_ip_address.ipv6_address,
             server_ip,
             RSI_IPV6_ADDRESS_LENGTH);
    }

    // Fill the server port
    rsi_uint32_to_4bytes(smtp_client->smtp_struct.smtp_client_init.server_port, port);

    // Copy username
    rsi_strcpy(smtp_client->smtp_buffer, username);
    smtp_length = rsi_strlen(username) + 1;

    // Copy password
    rsi_strcpy((smtp_client->smtp_buffer) + smtp_length, password);
    smtp_length += rsi_strlen(password) + 1;

    // Copy from address
    rsi_strcpy((smtp_client->smtp_buffer) + smtp_length, from_address);
    smtp_length += rsi_strlen(from_address) + 1;

    // Copy client domain
    rsi_strcpy((smtp_client->smtp_buffer) + smtp_length, client_domain);
    smtp_length += rsi_strlen(client_domain) + 1;

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_req_smtp_client_t) - RSI_SMTP_BUFFER_LENGTH + smtp_length;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

    // Send HTTP Get request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SMTP_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_SMTP_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_SMTP_CLIENT_CREATE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_SMTP_CLIENT_CREATE_EXIT_2, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK11
* @{
*/
/*==============================================*/
/**
 * @note        This API is not supported in current release.
 * @brief       Send mail to the recipient from the SMTP client.
 *              This is non-blocking API.
 * @pre		\ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]   mail_recipient_address            - Mail recipient address
 * @param[in]   priority                          - Priority level at which mail is delivered \n
 *                                                  1 - RSI_SMTP_MAIL_PRIORITY_LOW \n
 *                                                  2 - RSI_SMTP_MAIL_PRIORITY_NORMAL \n
 *                                                  4 - RSI_SMTP_MAIL_PRIORITY_HIGH
 * @param[in]   mail_subject                      - Subject line text, a null terminated string.
 * @param[in]   mail_body                         - Mail message
 * @param[in]   mail_body_length                  - Length of the mail body
 * @param[in]   smtp_client_mail_response_handler - Callback when asynchronous response comes from the sent mail \n
 *                                                  Status: status code \n
 *                                                  cmd: sub command type
 * @return     0              -  Success  \n
 *             Negative Value - Failure  \n
 *            -3              - Command given in wrong state \n
 *            -4              - Buffer not available to serve the command
 * 
 * @note      1. The total maximum length of mail_recipient_address, mail_subject & mail_body is 1024 bytes \n
 *            2. If status in callback is non-zero, then sub command type will be in 6th byte of the descriptor
 *   
 */

int32_t rsi_smtp_client_mail_send_async(uint8_t *mail_recipient_address,
                                        uint8_t priority,
                                        uint8_t *mail_subject,
                                        uint8_t *mail_body,
                                        uint16_t mail_body_length,
                                        void (*smtp_client_mail_response_handler)(uint16_t status,
                                                                                  const uint8_t cmd_type))
{
  rsi_req_smtp_client_t *smtp_client;
  rsi_pkt_t *pkt;
  int32_t status       = RSI_SUCCESS;
  uint16_t smtp_length = 0;
  uint16_t send_size   = 0;
  uint8_t *host_desc   = NULL;
  SL_PRINTF(SL_SMTP_CLIENT_MAIL_SEND_ASYNC_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_SMTP_CLIENT_MAIL_SEND_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_SMTP_CLIENT_MAIL_SEND_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if ((priority != RSI_SMTP_MAIL_PRIORITY_LOW) && (priority != RSI_SMTP_MAIL_PRIORITY_NORMAL)
      && (priority != RSI_SMTP_MAIL_PRIORITY_HIGH)) {
    // Return invalid command error
    SL_PRINTF(SL_SMTP_CLIENT_MAIL_SEND_ASYNC_INVALID_PARAM_1, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Register callback
    if (smtp_client_mail_response_handler != NULL) {
      // Register SMTP client response notify call back handler
      rsi_wlan_cb_non_rom->nwk_callbacks.smtp_client_mail_response_handler = smtp_client_mail_response_handler;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return invalid command error
      SL_PRINTF(SL_SMTP_CLIENT_MAIL_SEND_ASYNC_INVALID_PARAM_2, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_SMTP_CLIENT_MAIL_SEND_ASYNC_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    smtp_client = (rsi_req_smtp_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_smtp_client_t));

    // Fill command type
    smtp_client->command_type = RSI_SMTP_CLIENT_MAIL_SEND;

    smtp_client->smtp_struct.smtp_mail_send.smtp_feature = priority;

    // Copy SMTP mail body length
    rsi_uint16_to_2bytes(smtp_client->smtp_struct.smtp_mail_send.mail_body_length, mail_body_length);

    // Copy mail recipient address
    rsi_strcpy(smtp_client->smtp_buffer, mail_recipient_address);
    smtp_length = rsi_strlen(mail_recipient_address) + 1;

    // Copy mail subject
    rsi_strcpy((smtp_client->smtp_buffer) + smtp_length, mail_subject);
    smtp_length += rsi_strlen(mail_subject) + 1;

    // Copy mail message
    memcpy((smtp_client->smtp_buffer) + smtp_length, mail_body, mail_body_length);
    smtp_length += mail_body_length;

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_req_smtp_client_t) - RSI_SMTP_BUFFER_LENGTH + smtp_length;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

    // Send SMTP mail send request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SMTP_CLIENT, pkt);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_SMTP_CLIENT_MAIL_SEND_ASYNC_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_SMTP_CLIENT_MAIL_SEND_ASYNC_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK11
* @{
*/
/*==============================================*/
/**
 * @note        This API is not supported in current release.
 * @brief       Delete SMTP client.
 *              This is a non-blocking API.
 * @pre		\ref rsi_config_ipaddress, rsi_smtp_client_create API needs to be called before this API.
 * @param[in]   smtp_client_delete_response_handler - Callback when asynchronous response comes for the delete request \n
 *                                                    status:Status code \n
 *                                                    cmd   : Sub-command type
 * @return     0              -  Success  \n
 *             Negative Value - Failure \n
 *             -3             - Command given in wrong state \n
 *             -4             - Buffer not available to serve the command
 * @note       If status in callback is non-zero, then sub-command type will be in 6th byte of the descriptor.
 */

int32_t rsi_smtp_client_delete_async(void (*smtp_client_delete_response_handler)(uint16_t status,
                                                                                 const uint8_t cmd_type))
{

  rsi_req_smtp_client_t *smtp_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_SMTP_CLIENT_DELETE_ASYNC_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_SMTP_CLIENT_DELETE_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_SMTP_CLIENT_DELETE_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Register callback
    if (smtp_client_delete_response_handler != NULL) {
      // Register SMTP client  delete response notify call back handler
      rsi_wlan_cb_non_rom->nwk_callbacks.smtp_client_delete_response_handler = smtp_client_delete_response_handler;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return invalid command error
      SL_PRINTF(SL_SMTP_CLIENT_DELETE_ASYNC_INVALID_PARAM, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_INVALID_PARAM;
    }
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_SMTP_CLIENT_DELETE_ASYNC_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    smtp_client = (rsi_req_smtp_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_req_smtp_client_t));

    // Fill command type
    smtp_client->command_type = RSI_SMTP_CLIENT_DEINIT;

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_req_smtp_client_t) - RSI_SMTP_BUFFER_LENGTH;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

    // send SMTP Get request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SMTP_CLIENT, pkt);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_SMTP_CLIENT_DELETE_ASYNC_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_SMTP_CLIENT_DELETE_ASYNC_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */
