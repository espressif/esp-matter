/*******************************************************************************
* @file  rsi_http_ota_fw_up.c
* @brief This file contains API's to get HTTP data and post HTTP data for requested URL
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
/** @addtogroup FIRMWARE
* @{
*/
/*==============================================*/
/**
 * @brief      Initiate firmware update from an HTTP server. This is non-blocking API.
 * @param[in]  flags            -  Select version and security \n
 * 
 *   Flags |     Macro       |          Description
 *  :------|:----------------|:-----------------------------------------------------------------------
 *  BIT(0) | RSI_IPV6        |    Set this bit to enable IPv6 , by default it is configured to IPv4
 *  BIT(1) | RSI_SSL_ENABLE  |    Set this bit to enable SSL feature
 *  BIT(2) | RSI_SSL_V_1_0   |    Set this bit to support SSL TLS Version 1.0 if HTTPS is enabled
 *  BIT(3) | RSI_SSL_V_1_2   |    Set this bit to support SSL_TLS Version 1.2 if HTTPS is enabled
 *  BIT(4) | RSI_SSL_V_1_1   |    Set this bit to support SSL_TLS Version 1.1 if HTTPS is enabled
 *  BIT(5) | HTTP_POST_DATA  |    Set this bit to enable Http_post large data feature
 *  BIT(6) | HTTP_V_1_1      |    Set this bit  to use HTTP version 1.1
 *
 * @param[in]  ip_address                 - Server IP address
 * @param[in]  port                       - Port number, default : 80 - HTTP, 443 - HTTPS
 * @param[in]  resource                   - URL string for requested resource
 * @param[in]  host_name                  - Host name
 * @param[in]  extended_header            - Extender header if present
 * @param[in]  username                   - Username for server Authentication
 * @param[in]  password                   - Password for server Authentication
 * @param[in]  http_otaf_response_handler - Callback is called when asynchronous response is received from module for HTTP firmware update request \n
 * @param[out] status                     - Status code represent
 *                                          RSI_SUCCESS                : Firmware update success
 *                                          oOther expected error codes : BBED,BB40,BB38,BBD2,FF74,FFF4
 * @param[out] buffer                     - NULL
 * @return      0                         - Success \n
 *              Negative value            - Failure
 *
 */
int32_t rsi_http_fw_update(uint8_t flags,
                           uint8_t *ip_address,
                           uint16_t port,
                           uint8_t *resource,
                           uint8_t *host_name,
                           uint8_t *extended_header,
                           uint8_t *user_name,
                           uint8_t *password,
                           void (*http_otaf_response_handler)(uint16_t status, const uint8_t *buffer))
{
  return rsi_http_otaf_async(RSI_HTTP_OTAF,
                             flags,
                             ip_address,
                             port,
                             resource,
                             host_name,
                             extended_header,
                             user_name,
                             password,
                             NULL,
                             0,
                             http_otaf_response_handler);
}
/** @} */

/** @addtogroup NETWORK9
* @{
*/
/*==============================================*/
/**
 * @brief      Post the HTTP data for the requested URL to HTTP server. This is a non-blocking API.
 * @param[in]  type             -  0 - HTTPGET \n 1 - HTTPPOST
 * @param[in]  flags            -  Select version and security:\n
 * 
 *   Flags |     Macro       |          Description
 *  :------|:----------------|:-----------------------------------------------------------------------
 *  BIT(0) | RSI_IPV6        |    Set this bit to enable IPv6 , by default it is configured to IPv4
 *  BIT(1) | RSI_SSL_ENABLE  |    Set this bit to enable SSL feature
 *  BIT(2) | RSI_SSL_V_1_0   |    Set this bit to support SSL TLS Version 1.0 if HTTPS is enabled
 *  BIT(3) | RSI_SSL_V_1_2   |    Set this bit to support SSL_TLS Version 1.2 if HTTPS is enabled
 *  BIT(4) | RSI_SSL_V_1_1   |    Set this bit to support SSL_TLS Version 1.1 if HTTPS is enabled
 *  BIT(6) | HTTP_V_1_1      |    Set this bit  to use HTTP version 1.1
 *
 * @param[in]  ip_address       - Server IP address
 * @param[in]  port             - Port number, default : 80 - HTTP, 443 - HTTPS
 * @param[in]  resource         - URL string for requested resource
 * @param[in]  host_name        - Post name
 * @param[in]  extended_header  - Extender header if present, after each header member append \r\c \n
 * @param[in]  username         - Username for server Authentication
 * @param[in]  password         - Password for server Authentication
 * @param[in]  post_data        - HTTP data to be posted to server
 * @param[in]  post_data_length - HTTP data length to be posted to server
 * @param[in]  callback         - callback is called when asynchronous response is received from module for HTTP firmware update request
 * @param[out] buffer           - NULL
 * @param[out] status           - This is the status response from module. This returns failure upon an internal error only.
 * @return      0               - Success  \n
 *              Negative Value  - Failure \n
 *              -2              - Invalid parameters \n
 *              -3              - Command given in wrong state \n
 *              -4              - Buffer not available to serve command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_http_otaf_async(uint8_t type,
                            uint8_t flags,
                            uint8_t *ip_address,
                            uint16_t port,
                            uint8_t *resource,
                            uint8_t *host_name,
                            uint8_t *extended_header,
                            uint8_t *user_name,
                            uint8_t *password,
                            uint8_t *post_data,
                            uint32_t post_data_length,
                            void (*callback)(uint16_t status, const uint8_t *buffer))
{
  rsi_req_http_client_t *http_client = NULL;
  rsi_pkt_t *pkt                     = NULL;
  int32_t status                     = RSI_SUCCESS;
  uint8_t https_enable               = 0;
  uint16_t http_length               = 0;
  uint32_t send_size                 = 0;
  uint8_t *host_desc                 = NULL;
  SL_PRINTF(SL_HTTP_OTAF_ASYNC_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Register HTTP client response, notify callback handler to NULL
  rsi_wlan_cb_non_rom->nwk_callbacks.http_otaf_response_handler = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode and AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_HTTP_OTAF_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_HTTP_OTAF_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if (rsi_check_and_update_cmd_state(NWK_CMD, IN_USE) == RSI_SUCCESS) {
    // register callback
    if (callback != NULL) {
      // Register HTTP client response, notify call back handler
      rsi_wlan_cb_non_rom->nwk_callbacks.http_otaf_response_handler = callback;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return invalid command error
      SL_PRINTF(SL_HTTP_OTAF_ASYNC_INVALID_PARAM, NETWORK, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_HTTP_OTAF_ASYNC_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    http_client = (rsi_req_http_client_t *)pkt->data;

    // Memset the packet data to insert NULL between fields
    memset(&pkt->data, 0, sizeof(rsi_req_http_client_t));

    // Fill IP version
    if (flags & RSI_IPV6) {
      // Set ipv6 version
      rsi_uint16_to_2bytes(http_client->ip_version, RSI_IP_VERSION_6);
    } else {
      // Set ipv4 version
      rsi_uint16_to_2bytes(http_client->ip_version, RSI_IP_VERSION_4);
    }

    if (flags & RSI_SSL_ENABLE) {
      // Set https feature
      https_enable = 1;
    }

    // Set default by NULL delimiter
    https_enable |= BIT(1);

    //ssl versions

    if (flags & RSI_SSL_V_1_0) {
      https_enable |= RSI_SSL_V_1_0;
    }
    if (flags & RSI_SSL_V_1_1) {
      https_enable |= RSI_SSL_V_1_1;
    }

    if (flags & RSI_SUPPORT_HTTP_V_1_1) {
      // Set HTTP version 1.1 feature bit
      https_enable |= RSI_SUPPORT_HTTP_V_1_1;
    }

    // Fill https features parameters
    rsi_uint16_to_2bytes(http_client->https_enable, https_enable);

    // Fill port no
    http_client->port = port;

    // Copy username
    rsi_strcpy(http_client->buffer, user_name);
    http_length = rsi_strlen(user_name) + 1;

    // Copy  password
    rsi_strcpy((http_client->buffer) + http_length, password);
    http_length += rsi_strlen(password) + 1;

    // Check for HTTP_V_1.1 and Empty host name
    if ((flags & RSI_SUPPORT_HTTP_V_1_1) && (rsi_strlen(host_name) == 0)) {
      host_name = ip_address;
    }

    // Copy  Host name
    rsi_strcpy((http_client->buffer) + http_length, host_name);
    http_length += rsi_strlen(host_name) + 1;

    // Copy IP address
    rsi_strcpy((http_client->buffer) + http_length, ip_address);
    http_length += rsi_strlen(ip_address) + 1;

    // Copy URL resource
    rsi_strcpy((http_client->buffer) + http_length, resource);
    http_length += rsi_strlen(resource) + 1;

    if (extended_header != NULL) {

      // Copy Extended header
      rsi_strcpy((http_client->buffer) + http_length, extended_header);
      http_length += rsi_strlen(extended_header);
    }

    if (type) {
      // Copy Httppost data
      memcpy((http_client->buffer) + http_length + 1, post_data, post_data_length);

      http_length += (post_data_length + 1);
    }

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_req_http_client_t) - RSI_HTTP_BUFFER_LEN + http_length;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint32_to_4bytes(host_desc, (send_size & 0xFFF));

    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_OTAF, pkt);
  } else {
    // Return NWK command error
    SL_PRINTF(SL_HTTP_OTAF_ASYNC_NWK_CMD_IN_PROGRESS, NETWORK, LOG_ERROR);
    return RSI_ERROR_NWK_CMD_IN_PROGRESS;
  }

  // Return status
  SL_PRINTF(SL_HTTP_OTAF_ASYNC_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */
