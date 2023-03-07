/*******************************************************************************
* @file  rsi_http_client.c
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

// Include Driver header file
#include "rsi_driver.h"
#include "rsi_http_client.h"
/*
 * Global Variables
 * */
extern rsi_driver_cb_t *rsi_driver_cb;
uint8_t *rsi_itoa(uint32_t val, uint8_t *str);
/** @addtogroup NETWORK9
* @{
*/
/*==============================================*/
/**
 * @brief      Send HTTP get request to remote HTTP server. This is a non-blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  flags            -  Select version and security \n
 *  
 *   Flags |     Macro       |          Description
 *  :------|:----------------|:-----------------------------------------------------------------------
 *  BIT(0) | RSI_IPV6        |    Set this bit to enable IPv6, by default it is configured to IPv4
 *  BIT(1) | RSI_SSL_ENABLE  |    Set this bit to enable SSL featuree
 *  BIT(2) | RSI_SSL_V_1_0   |    Set this bit to support SSL TLS Version 1.0 if HTTPS is enabled
 *  BIT(3) | RSI_SSL_V_1_2   |    Set this bit to support SSL_TLS Version 1.2 if HTTPS is enabled
 *  BIT(4) | RSI_SSL_V_1_1   |    Set this bit to support SSL_TLS Version 1.1 if HTTPS is enabled
 *  BIT(5) | HTTP_POST_DATA  |    Set this bit to enable Http_post large data feature
 *  BIT(6) | HTTP_V_1_1      |    Set this bit  to use HTTP version 1.1
 *
 * @param[in]   ip_address                       - Server IP address
 * @param[in]   port                             - Port number of HTTP server
 * @param[in]   resource                         - URL string for requested resource
 * @param[in]   host_name                        - Host name
 * @param[in]   extended_header                  - User-defined extended header, each member header should end by \r\c \n
 * @param[in]   username                         - Username for server authentication
 * @param[in]   password                         - Password for server authentication
 * @param[in]   http_client_get_response_handler - Callback when asynchronous response comes for the request
 * @param[out]  status                           - Status of response from module. This will return failure upon an internal error only.
 * @param[out]  buffer                           - Buffer pointer
 * @param[out]  length                           - Length of data
 * @param[out]  more_data                        - 1  No more data, 0  More data present
 * @param[out]  status_code          HTTP response code as returned by server in HTTP header. e.g., 200, 201, 404, etc.
 *                                   This field is valid only when status field (first argument) is successful, indicating
 *                                   a response is received from HTTP server. A status_code equal to 0 indicates that
 *                                   there was no HTTP header in the received packet, probably a continuation of the frame
 *                                   body received in the previous chunk.
 *                                   This field is available, if the feature  RSI_HTTP_STATUS_INDICATION_EN
 *                                   is enabled in rsi_wlan_config.h
 * @return      0                 -  Success  \n
 *              Negative value    - Failure \n
 *              -2                - Invalid parameters \n
 *				      -3                - Command given in wrong state \n
 *				      -4                - Buffer not available to serve the command
 * @note        HTTP server port is configurable on non-standard port also \n 
 *              Refer to Error Codes section for the description of the above error codes \ref error-codes.
 *
 */

int32_t rsi_http_client_get_async(uint8_t flags,
                                  uint8_t *ip_address,
                                  uint16_t port,
                                  uint8_t *resource,
                                  uint8_t *host_name,
                                  uint8_t *extended_header,
                                  uint8_t *user_name,
                                  uint8_t *password,
#if RSI_HTTP_STATUS_INDICATION_EN
                                  void (*http_client_response_handler)(uint16_t status,
                                                                       const uint8_t *buffer,
                                                                       const uint16_t length,
                                                                       const uint32_t moredata,
                                                                       uint16_t status_code)
#else
                                  void (*http_client_response_handler)(uint16_t status,
                                                                       const uint8_t *buffer,
                                                                       const uint16_t length,
                                                                       const uint32_t moredata)
#endif
)
{
  return rsi_http_client_async(RSI_HTTP_GET,
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
                               http_client_response_handler);
}

/** @} */

/** @addtogroup NETWORK9
* @{
*/
/*==============================================*/
/**
 * @brief      Send HTTP post request to remote HTTP server. This is a non-blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  flags            -  Select version and security \n
 * 
 *   Flags |     Macro       |          Description
 *  :------|:----------------|:-----------------------------------------------------------------------
 *  BIT(0) | RSI_IPV6        |    Set this bit to enable IPv6, by default, it is configured to IPv4
 *  BIT(1) | RSI_SSL_ENABLE  |    Set this bit to enable SSL feature
 *  BIT(2) | RSI_SSL_V_1_0   |    Set this bit to support SSL TLS Version 1.0 if HTTPS is enabled
 *  BIT(3) | RSI_SSL_V_1_2   |    Set this bit to support SSL_TLS Version 1.2 if HTTPS is enabled
 *  BIT(4) | RSI_SSL_V_1_1   |    Set this bit to support SSL_TLS Version 1.1 if HTTPS is enabled
 *  BIT(5) | HTTP_POST_DATA  |    Set this bit to enable Http_post large data feature
 *  BIT(6) | HTTP_V_1_1      |    Set this bit  to use HTTP version 1.1
 *
 * @param[in]  ip_address                        - Server IP address
 * @param[in]  port                              - Port number of HTTP server
 * @param[in]  resource                          - URL string for requested resource
 * @param[in]  host_name                         - Host name
 * @param[in]  extended_header                   - User-defined extended header, each member header should end by \r\c \n
 * @param[in]  username                          - Username for server authentication
 * @param[in]  password                          - Password for server authentication
 * @param[in]  post_data                         - HTTP data to be posted to server
 * @param[in]  post_data_length                  - Post data length
 * @param[in]  http_client_post_response_handler - Callback when asynchronous response comes for the request
 * @param[in]  status                            - Status of response from module. This will return failure upon an internal error only.
 * @param[in]  buffer                            - Buffer pointer
 * @param[in]  length                            - Length of data
 * @param[in]  moredata                          - 1  No more data, 0  More data present, 2  HTTP post success response
 * @param[in]  status_code                        HTTP response code as returned by server in HTTP header. e.g., 200, 201, 404, etc.
 *                                                This field is valid only when status field (first argument) is successful, indicating
 *                                                a response is received from HTTP server. A status_code equal to 0 indicates that
 *                                                there was no HTTP header in the received packet, probably a continuation of the frame
 *                                                body received in the previous chunk.
 *                                                This field is available, if the feature  RSI_HTTP_STATUS_INDICATION_EN
 *                                                is enabled in rsi_wlan_config.h
 * @return      0                                -  Success  \n
 *              Negative Value                   - Failure \n
 *              -2                               - Invalid parameters \n
 *				      -3                               - Command given in wrong state \n
 *				      -4                               - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 *
 */
int32_t rsi_http_client_post_async(uint8_t flags,
                                   uint8_t *ip_address,
                                   uint16_t port,
                                   uint8_t *resource,
                                   uint8_t *host_name,
                                   uint8_t *extended_header,
                                   uint8_t *user_name,
                                   uint8_t *password,
                                   uint8_t *post_data,
                                   uint32_t post_data_length,
#if RSI_HTTP_STATUS_INDICATION_EN
                                   void (*http_client_response_handler)(uint16_t status,
                                                                        const uint8_t *buffer,
                                                                        const uint16_t length,
                                                                        const uint32_t moredata,
                                                                        uint16_t status_code)
#else
                                   void (*http_client_response_handler)(uint16_t status,
                                                                        const uint8_t *buffer,
                                                                        const uint16_t length,
                                                                        const uint32_t moredata)
#endif
)
{
  return rsi_http_client_async(RSI_HTTP_POST,
                               flags,
                               ip_address,
                               port,
                               resource,
                               host_name,
                               extended_header,
                               user_name,
                               password,
                               post_data,
                               post_data_length,
                               http_client_response_handler);
}

/** @} */

/** @addtogroup NETWORK9
* @{
*/
/*==============================================*/
/**
 * @brief      Send HTTP get request/HTTP post request to remote HTTP server based on the type selected. This is a non-blocking API.
 * @pre   \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  type             - 0  RSI_HTTP_GET, 1  RSI_HTTP_POST 
 * @param[in]  flags            - Select version and security \n
 * 
 *   Flags |     Macro       |          Description
 *  :------|:----------------|:-----------------------------------------------------------------------
 *  BIT(0) | RSI_IPV6        |    Set this bit to enable IPv6, by default, it is configured to IPv4
 *  BIT(1) | RSI_SSL_ENABLE  |    Set this bit to enable SSL feature
 *  BIT(2) | RSI_SSL_V_1_0   |    Set this bit to support SSL TLS Version 1.0 if HTTPS is enabled
 *  BIT(3) | RSI_SSL_V_1_2   |    Set this bit to support SSL_TLS Version 1.2 if HTTPS is enabled
 *  BIT(4) | RSI_SSL_V_1_1   |    Set this bit to support SSL_TLS Version 1.1 if HTTPS is enabled
 *  BIT(5) | HTTP_POST_DATA  |    Set this bit to enable Http_post large data feature
 *  BIT(6) | HTTP_V_1_1      |    Set this bit  to use HTTP version 1.1
 *
 * 
 * @param[in]   ip_address       - Server IP address
 * @param[in]   port             - Port number of HTTP server
 * @param[in]   resource         - URL string for requested resource
 *						                    Note: HTTP server port is also configurable on a non-standard port
 * @param[in]   host_name        - Host name
 * @param[in]   extended_header  - User-defined extended header, each member header should end by \r\c \n
 * @param[in]   username         - Username for server authentication
 * @param[in]   password         - Password for server authentication
 * @param[in]   post_data        - HTTP data to be posted to server
 * @param[in]   post_data_length - This is the post data length 
 * @param[in]   callback         - Callback when asyncronous response comes for the request
 * @param[out]  status           - Status of response from module. This will return failure upon an internal error only.
 * @param[out]  buffer           - Buffer pointer
 * @param[out]  length           - Length of data
 * @param[out]  moredata         - 1   No more data, 0   More data present
 * @param[in]   status_code       HTTP response code as returned by server in HTTP header. e.g., 200, 201, 404, etc.
 *                                This field is valid only when status field (first argument) is successful, indicating
 *                                a response is received from HTTP server. A status_code equal to 0 indicates that
 *                                there was no HTTP header in the received packet, probably a continuation of the frame
 *                                body received in the previous chunk.
 *                                This field is available, if the feature  RSI_HTTP_STATUS_INDICATION_EN
 *                                is enabled in rsi_wlan_config.h
 * @return      0               -  Success  \n
 *              Negative Value  - Failure \n
 *              -2              - Invalid parameters \n
 *				      -3              - Command given in wrong state \n
 *				      -4              - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 *
 */

int32_t rsi_http_client_async(uint8_t type,
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
#if RSI_HTTP_STATUS_INDICATION_EN
                              void (*callback)(uint16_t status,
                                               const uint8_t *buffer,
                                               const uint16_t length,
                                               const uint32_t moredata,
                                               uint16_t status_code)
#else
                              void (*callback)(uint16_t status,
                                               const uint8_t *buffer,
                                               const uint16_t length,
                                               const uint32_t moredata)
#endif
)
{
  rsi_req_http_client_t *http_client;
  rsi_pkt_t *pkt;
  int32_t status       = RSI_SUCCESS;
  uint8_t https_enable = 0;
  uint16_t http_length = 0;
  uint32_t send_size   = 0;
  uint8_t *host_desc   = NULL;
  uint8_t tmp_str[7]   = { 0 };
  SL_PRINTF(SL_HTTP_CLIENT_ASYNC_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Register HTTP client response notify call back handler to NULL
  rsi_wlan_cb_non_rom->nwk_callbacks.http_client_response_handler = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode and AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // command given in wrong state
      SL_PRINTF(SL_HTTP_CLIENT_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_HTTP_CLIENT_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Register callback
    if (callback != NULL) {
      // Register HTTP client response notify call back handler
      rsi_wlan_cb_non_rom->nwk_callbacks.http_client_response_handler = callback;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return invalid command error
      SL_PRINTF(SL_HTTP_CLIENT_ASYNC_NVALID_PARAM, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_HTTP_CLIENT_ASYNC_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    http_client = (rsi_req_http_client_t *)pkt->data;

    // Memset the packet data to insert NULL between fields
    memset(&pkt->data, 0, sizeof(rsi_req_http_client_t));

    // Fill ipversion
    if (flags & RSI_IPV6) {
      // Set IPv6 version
      rsi_uint16_to_2bytes(http_client->ip_version, RSI_IP_VERSION_6);
    } else {
      // Set IPv4 version
      rsi_uint16_to_2bytes(http_client->ip_version, RSI_IP_VERSION_4);
    }

    if (flags & RSI_SSL_ENABLE) {
      // Set HTTPS feature
      https_enable = 1;
    }

    // Set default by NULL delimiter
    https_enable |= BIT(1);

    // SSL versions

    if (flags & RSI_SSL_V_1_0) {
      https_enable |= RSI_SSL_V_1_0;
    }
    if (flags & RSI_SSL_V_1_1) {
      https_enable |= RSI_SSL_V_1_1;
    }

    if (flags & RSI_SUPPORT_HTTP_POST_DATA) {
      // Set HTTP post big data feature bit
      https_enable |= RSI_SUPPORT_HTTP_POST_DATA;
    }

    if (flags & RSI_SUPPORT_HTTP_V_1_1) {
      // Set HTTP version 1.1 feature bit
      https_enable |= RSI_SUPPORT_HTTP_V_1_1;
    }

    // Fill HTTPS features parameters
    rsi_uint16_to_2bytes(http_client->https_enable, https_enable);

    // Fill port number
    http_client->port = port;

    // Copy username
    rsi_strcpy(http_client->buffer, user_name);
    http_length = rsi_strlen(user_name) + 1;

    // Copy password
    rsi_strcpy((http_client->buffer) + http_length, password);
    http_length += rsi_strlen(password) + 1;

    // Check for HTTP_V_1.1 and Empty host name
    if ((flags & RSI_SUPPORT_HTTP_V_1_1) && (rsi_strlen(host_name) == 0)) {
      host_name = ip_address;
    }

    // Copy host name
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
      // Check for HTTP post data feature
      if (flags & RSI_SUPPORT_HTTP_POST_DATA) {
        post_data = rsi_itoa(post_data_length, tmp_str);

        post_data_length = rsi_strlen(post_data);
      }

      // Copy HTTP post data
      memcpy((http_client->buffer) + http_length + 1, post_data, post_data_length);

      http_length += (post_data_length + 1);
    }

    // Use host descriptor to set payload length
    send_size = sizeof(rsi_req_http_client_t) - RSI_HTTP_BUFFER_LEN + http_length;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint32_to_4bytes(host_desc, (send_size & 0xFFF));

    if (type) {
      // Send HTTP post request command
      status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_CLIENT_POST, pkt);
    } else {
      // Send HTTP Get request command
      status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_CLIENT_GET, pkt);
    }

  } else {
    // Return NWK command error
    SL_PRINTF(SL_HTTP_CLIENT_ASYNC_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_HTTP_CLIENT_ASYNC_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK9
* @{
*/
/*==============================================*/
/**
 * @brief      Abort any ongoing HTTP request from the client. This is a blocking API.
 * @pre   \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  Void
 * @return      0              -  Success  \n
 *              Negative Value - Failure \n
 *				      -3             - Command given in wrong state \n
 *				      -4             - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_http_client_abort(void)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_HTTP_CLIENT_ABORT_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_HTTP_CLIENT_ABORT_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

    // Send join command to start WPS
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_ABORT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_HTTP_ABORT_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_HTTP_CLIENT_ABORT_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_HTTP_CLIENT_ABORT_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK9
* @{
*/
/*==============================================*/
/**
 * @brief      Create the HTTP put client. This is a blocking API.
 * @pre   \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  Void
 * @return      0              -  Success  \n
 *              Negative Value - Failure \n
 *				      -3             - Command given in wrong state \n
 *				      -4             - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_http_client_put_create(void)
{
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_HTTP_CLIENT_PUT_CREATE_ENTRY, NETWORK, LOG_INFO);
  rsi_http_client_put_req_t *http_put_req;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Register HTTP client response notify call back handler to NULL
  rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_put_response_handler = NULL;

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_HTTP_CLIENT_PUT_CREATE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    http_put_req = (rsi_http_client_put_req_t *)pkt->data;

    // Memset the packet data to insert NULL between fields
    memset(&pkt->data, 0, sizeof(rsi_http_client_put_req_t));

    // Fill command type
    http_put_req->command_type = HTTP_CLIENT_PUT_CREATE;

    // Use host descriptor to set payload length
    send_size = sizeof(rsi_http_client_put_req_t) - HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

    // Send HTTP put command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_CLIENT_PUT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_HTTP_CLIENT_PUT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_HTTP_CLIENT_PUT_CREATE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_HTTP_CLIENT_PUT_CREATE_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK9
* @{
*/
/*==============================================*/
/**
 * @brief      Delete the created HTTP put client. This is a non-blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API
 * @param[in]  Void
 * @return      0              -  Success  \n
 *              Negative Value - Failure \n
 *				      -3             - Command given in wrong state \n
 *				      -4             - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_http_client_put_delete(void)
{
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_HTTP_CLIENT_PUT_DELETE_ENTRY, NETWORK, LOG_INFO);
  rsi_http_client_put_req_t *http_put_req;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_HTTP_CLIENT_PUT_DELETE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    http_put_req = (rsi_http_client_put_req_t *)pkt->data;

    // Memset the packet data to insert NULL between fields
    memset(&pkt->data, 0, sizeof(rsi_http_client_put_req_t));

    // Fill command type
    http_put_req->command_type = HTTP_CLIENT_PUT_DELETE;

    // Use host descriptor to set payload length
    send_size = sizeof(rsi_http_client_put_req_t) - HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

    // Send HTTP put command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_CLIENT_PUT, pkt);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_HTTP_CLIENT_PUT_DELETE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_HTTP_CLIENT_PUT_DELETE_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK9
* @{
*/
/*==============================================*/
/**
 * @brief 	   Start the HTTP client put process. This is a non-blocking API.
 * @pre  \ref API needs to be called before this API  \n
 *            rsi_config_ipaddress  \n
 *            rsi_http_client_put_create.
 * @param[in]  flags            -  Select version and security \n
 *    
 *   Flags |     Macro                      |          Description
 *  :------|:-------------------------------|:-----------------------------------------------------------------------
 *  BIT(0) | RSI_IPV6                       |    Set this bit to enable IPv6, by default, it is configured to IPv4
 *  BIT(1) | RSI_SSL_ENABLE                 |    Set this bit to enable SSL feature
 *  BIT(2) | RSI_SSL_V_1_0                  |    Set this bit to support SSL TLS Version 1.0 if HTTPS is enabled
 *  BIT(3) | RSI_SSL_V_1_2                  |    Set this bit to support SSL_TLS Version 1.2 if HTTPS is enabled
 *  BIT(4) | RSI_SSL_V_1_1                  |    Set this bit to support SSL_TLS Version 1.1 if HTTPS is enabled
 *  BIT(5) | HTTP_POST_DATA                 |    Set this bit to enable Http_post large data feature
 *  BIT(6) | HTTP_V_1_1                     |    Set this bit  to use HTTP version 1.1
 *  BIT(7) | HTTP_USER_DEFINED_CONTENT_TYPE |    Set  this bit to enable user defined http_content type
 *
 * @param[in]  ip_address       - Server IP address
 * @param[in]  port_number      - Port number of HTTP server
 * @param[in]  resource         - URL string for requested resource
 * @param[in]  host_name        - Host name
 * @param[in]  extended_header  - User-defined extended header, each member header should end by \r\c \n
 * @param[in]  user_name        - Username for server authentication
 * @param[in]  password         - Password for server authentication
 * @param[in]  content_length   - Total length of HTTP data
 * @param[in]  post_data_length - HTTP data length to be posted to server
 * @param[in]  callback         - Callback when asyncronous response comes for the request
 * @param[in]  status           - Status code
 * @param[in]  type             - HTTP Client PUT command type \n
 *                                  2 - Put start response \n
 *                                  3 - Put packet response \n
 *                                  4 - Put delete response \n
 *                                  5 - Data from HTTP server
 * @param[in]  buffer           - Buffer pointer
 * @param[in]  length           - Length of data
 * @param[in]  end_of_put_pkt   - End of file or HTTP resource content \n
 *                                 0 - More data is pending from host  \n
 *                                 1 - End of HTTP file/resource content  \n
 *                                 8 - More data pending from server  \n
 *                                 9 - End of HTTP file from server
 * @return      0               -  Success  \n
 *              Negative Value  - Failure \n
 *				      -3              - Command given in wrong state \n
 *				      -4              - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_http_client_put_start(
  uint8_t flags,
  uint8_t *ip_address,
  uint32_t port_number,
  uint8_t *resource,
  uint8_t *host_name,
  uint8_t *extended_header,
  uint8_t *user_name,
  uint8_t *password,
  uint32_t content_length,
  void (*callback)(uint16_t status, uint8_t type, const uint8_t *buffer, uint16_t length, const uint8_t end_of_put_pkt))
{
  rsi_pkt_t *pkt;
  int32_t status       = RSI_SUCCESS;
  uint8_t https_enable = 0;
  uint16_t http_length = 0;
  uint16_t send_size   = 0;
  uint8_t *host_desc   = NULL;
  rsi_http_client_put_req_t *http_put_req;
  rsi_http_client_put_start_t *http_put_start;
  SL_PRINTF(SL_HTTP_CLIENT_PUT_START_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Register callback
    if (callback != NULL) {
      // Register HTTP client response notify call back handler
      rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_put_response_handler = callback;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return invalid command error
      SL_PRINTF(SL_HTTP_CLIENT_PUT_START_INVALID_PARAM, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_HTTP_CLIENT_PUT_START_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    http_put_req = (rsi_http_client_put_req_t *)pkt->data;

    // Memset the packet data to insert NULL between fields
    memset(&pkt->data, 0, sizeof(rsi_http_client_put_req_t));

    http_put_start = &http_put_req->http_client_put_struct.http_client_put_start;

    // Fill command type
    http_put_req->command_type = HTTP_CLIENT_PUT_START;

    // Fill ipversion
    if (flags & RSI_IPV6) {
      // Set IPv6 version
      http_put_start->ip_version = RSI_IP_VERSION_6;
    } else {
      // Set IPv4 version
      http_put_start->ip_version = RSI_IP_VERSION_4;
    }

    if (flags & RSI_SSL_ENABLE) {
      // Set HTTPS feature
      https_enable = 1;

      // Set default by NULL delimiter
      https_enable |= BIT(1);
    }
    if (flags & RSI_HTTP_USER_DEFINED_CONTENT_TYPE) {
      // Enable user given content type in extended header
      https_enable |= RSI_HTTP_USER_DEFINED_CONTENT_TYPE;
    }
    // Fill HTTPS features parameters
    rsi_uint16_to_2bytes(http_put_start->https_enable, https_enable);
    // Fill HTTP server port number
    rsi_uint32_to_4bytes(http_put_start->port_number, port_number);

    // Fill Total resource content length
    rsi_uint32_to_4bytes(http_put_start->content_length, content_length);

    // Copy username
    rsi_strcpy(http_put_req->http_put_buffer, user_name);
    http_length = rsi_strlen(user_name) + 1;

    // Copy password
    rsi_strcpy((http_put_req->http_put_buffer) + http_length, password);
    http_length += rsi_strlen(password) + 1;

    // Copy host name
    rsi_strcpy((http_put_req->http_put_buffer) + http_length, host_name);
    http_length += rsi_strlen(host_name) + 1;

    // Copy IP address
    rsi_strcpy((http_put_req->http_put_buffer) + http_length, ip_address);
    http_length += rsi_strlen(ip_address) + 1;

    // Copy URL resource
    rsi_strcpy((http_put_req->http_put_buffer) + http_length, resource);
    http_length += rsi_strlen(resource) + 1;

    if (extended_header != NULL) {
      // Copy Extended header
      rsi_strcpy((http_put_req->http_put_buffer) + http_length, extended_header);
      http_length += rsi_strlen(extended_header);
    }

    // Use host descriptor to set payload length
    send_size = sizeof(rsi_http_client_put_req_t) - HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH + http_length;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

    // Send HTTP put command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_CLIENT_PUT, pkt);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_HTTP_CLIENT_PUT_START_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_HTTP_CLIENT_PUT_START_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK9
* @{
*/
/*==============================================*/
/**
 * @brief      Send HTTP data to HTTP server for the created URL resource. This is a blocking API.
 * @pre   \ref API needs to be called before this API  \n
 *             rsi_config_ipaddress  \n
 *             rsi_http_client_put_create  \n
 *             rsi_http_client_put_start.
 * @param[in]  file_content         - HTTP data content
 * @param[in]  current_chunk_length - HTTP data current chunk length
 * @return      0                   -  Success  \n
 *              Negative Value      - Failure \n
 *				     -3                   - Command given in wrong state \n
 *				     -4                   - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_http_client_put_pkt(uint8_t *file_content, uint16_t current_chunk_length)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  rsi_http_client_put_req_t *http_put_req;
  rsi_http_client_put_data_req_t *http_put_data;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_HTTP_CLIENT_PUT_PKT_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_HTTP_CLIENT_PUT_PKT_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    http_put_req = (rsi_http_client_put_req_t *)pkt->data;

    // Memset the packet data to insert NULL between fields
    memset(&pkt->data, 0, sizeof(rsi_http_client_put_req_t));

    http_put_data = &http_put_req->http_client_put_struct.http_client_put_data_req;

    // Fill command type
    http_put_req->command_type = HTTP_CLIENT_PUT_PKT;

    // Fill HTTP put current_chunk_length
    rsi_uint16_to_2bytes(http_put_data->current_length, current_chunk_length);

    // Fill resource content
    memcpy((uint8_t *)http_put_req->http_put_buffer, file_content, current_chunk_length);

    // Use host descriptor to set payload length
    send_size = sizeof(rsi_http_client_put_req_t) - HTTP_CLIENT_PUT_MAX_BUFFER_LENGTH + current_chunk_length;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

    // Send HTTP put command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_CLIENT_PUT, pkt);

    if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_put_response_handler == NULL) {
#ifndef RSI_NWK_SEM_BITMAP
      rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
      // Wait on NWK semaphore
      rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_HTTP_CLIENT_PUT_RESPONSE_WAIT_TIME);
      // Get WLAN/network command response status
      status = rsi_wlan_get_nwk_status();

      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
    }
  } else {
    // Return NWK command error
    SL_PRINTF(SL_HTTP_CLIENT_PUT_PKT_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_HTTP_CLIENT_PUT_PKT_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK9
* @{
*/
/*==============================================*/
/**
 * @brief      Send the HTTP post data packet to remote HTTP server. This is a non-blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  file_content         - User given http file content
 * @param[in]  current_chunk_length - Length of the current HTTP data
 * @param[in]  callback             - Callback when asynchronous response comes for the request.
 * @param[in]  status               - Status of response from module. This will return failure upon an internal error only.
 * @param[in]  buffer               - Buffer pointer
 * @param[in]  length               - Length of data
 * @param[in]  more_data            -  4- More data is pending from host \n
 *                                     5-End of HTTP data from host(Host sent total data content length) \n
 *                                     8–More data is pending from module to host. Further interrupts may be \n
 *                                       raised by the module till all the data is transferred to the Host. \n
 *                                     9– End of HTTP data from module to host.
 * @param[in]  status_code          - HTTP response code as returned by server in HTTP header. e.g., 200, 201, 404, etc.
 *                                    This field is valid only when status field (first argument) is successful, indicating
 *                                    a response is received from HTTP server. A status_code equal to 0 indicates that
 *                                    there was no HTTP header in the received packet, probably a continuation of the frame
 *                                    body received in the previous chunk.
 *                                    This field is available, if the feature  RSI_HTTP_STATUS_INDICATION_EN
 *                                    is enabled in rsi_wlan_config.h
 * @return       0                  - Success  \n
 *               Negative Value     - Failure \n
 *               -2                 - Invalid parameters \n
 *				       -3                 - Command given in wrong state \n
 *				       -4                 - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_http_client_post_data(uint8_t *file_content,
                                  uint16_t current_chunk_length,
#if RSI_HTTP_STATUS_INDICATION_EN
                                  void (*rsi_http_post_data_response_handler)(uint16_t status,
                                                                              const uint8_t *buffer,
                                                                              const uint16_t length,
                                                                              const uint32_t moredata,
                                                                              uint16_t status_code)
#else
                                  void (*rsi_http_post_data_response_handler)(uint16_t status,
                                                                              const uint8_t *buffer,
                                                                              const uint16_t length,
                                                                              const uint32_t moredata)
#endif
)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  rsi_http_client_post_data_req_t *http_post_data;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_HTTP_CLIENT_POST_DATA_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Register callback
    if (rsi_http_post_data_response_handler != NULL) {
      // Register HTTP client response notify call back handler
      rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_post_data_response_handler =
        rsi_http_post_data_response_handler;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return invalid command error
      SL_PRINTF(SL_HTTP_CLIENT_POST_DATA_INVALID_PARAM, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_HTTP_CLIENT_POST_DATA_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    http_post_data = (rsi_http_client_post_data_req_t *)pkt->data;

    // Memset the packet data to insert NULL between fields
    memset(&pkt->data, 0, sizeof(rsi_http_client_post_data_req_t));

    // Fill HTTP post data current_chunk_length
    rsi_uint16_to_2bytes(http_post_data->current_length, current_chunk_length);

    // Fill resource content
    memcpy((uint8_t *)http_post_data->http_post_data_buffer, file_content, current_chunk_length);

    // Use host descriptor to set payload length
    send_size =
      sizeof(rsi_http_client_post_data_req_t) - HTTP_CLIENT_POST_DATA_MAX_BUFFER_LENGTH + current_chunk_length;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

    // Send HTTP put command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HTTP_CLIENT_POST_DATA, pkt);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_HTTP_CLIENT_POST_DATA_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_HTTP_CLIENT_POST_DATA_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */
