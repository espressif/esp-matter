/*******************************************************************************
* @file  rsi_sntp_client.c
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

#include "rsi_sntp_client.h"

#include "rsi_nwk.h"

/** @addtogroup NETWORK12
* @{
*/
/*==============================================*/
/**
 * @brief      Create SNTP client.
 *             This is a non-blocking API.
 * @pre   \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  flags	- Select IP version and security \n
 * 
 *  Flags                  |          Description
 *  :----------------------|:-----------------------------------------------------------------------
 *  BIT(0) - RSI_IPV6      |    Set this bit to enable IPv6. Configured to IPv4 by default.
 *  BIT(1) - RSI_SSL_ENABLE|    Set this bit to enable SSL feature
 *  BIT(2) to BIT(7)       |    Reserved for future use
 *    
 * @param[in]  server_ip                           - Server IP address
 * @param[in]  sntp_method                         - SNTP methods to use \n
 *                                                   1-For Broadcast Method, 2-For Unicast Method 
 * @param[in]  sntp_timeout                        - SNTP timeout value
 * @param[in]  sntp_client_create_response_handler - Callback function when asynchronous response comes for the request. \n
 *                                                   status: Expected error codes are : 0xBB0A, 0xFF5F, 0xBB0B, 0xBB15, 0xBB10\n
 *                                                   cmd_type: Command type \n
 *                                                   buffer: Buffer pointer
 * @return     0              -  Success  \n
 *             Negative Value - Failure \n
 *            -2 - Invalid parameters, call back not registered  \n
 *            -3 - Command given in wrong state \n
 *            -4 - Buffer not available to serve the command
 * @note        SNTP broadcast method is currently not supported. 
 * @note        Refer to Error Codes section for more error codes \ref error-codes.
 *
 */

int32_t rsi_sntp_client_create_async(uint8_t flags,
                                     uint8_t *server_ip,
                                     uint8_t sntp_method,
                                     uint16_t sntp_timeout,
                                     void (*rsi_sntp_client_create_response_handler)(uint16_t status,
                                                                                     const uint8_t cmd_type,
                                                                                     const uint8_t *buffer))

{
  rsi_sntp_client_t *sntp_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_SNTP_CLIENT_CREATE_ASYNC_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_SNTP_CLIENT_CREATE_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_SNTP_CLIENT_CREATE_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Register callback
    if (rsi_sntp_client_create_response_handler != NULL) {
      // Register POP3 client response notify callback handler
      rsi_wlan_cb_non_rom->nwk_callbacks.rsi_sntp_client_create_response_handler =
        rsi_sntp_client_create_response_handler;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return invalid command error
      SL_PRINTF(SL_SNTP_CLIENT_CREATE_ASYNC_INVALID_PARAM, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_SNTP_CLIENT_CREATE_ASYNC_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    sntp_client = (rsi_sntp_client_t *)pkt->data;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_sntp_client_t));

    // Fill command type
    sntp_client->command_type = RSI_SNTP_CREATE;

    if ((sntp_method != RSI_SNTP_BROADCAST_MODE) && (sntp_method != RSI_SNTP_UNICAST_MODE)) {
      // Default SNTP is in UNICAST mode
      sntp_method = RSI_SNTP_UNICAST_MODE;
    }

    // Fill SNTP method
    sntp_client->sntp_method = sntp_method;

    // SNTP time out
    rsi_uint16_to_2bytes(sntp_client->sntp_timeout, sntp_timeout);

    // Check for IP version
    if (!(flags & RSI_IPV6)) {
      sntp_client->ip_version = RSI_IP_VERSION_4;
      memcpy(sntp_client->server_ip_address.ipv4_address, server_ip, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      sntp_client->ip_version = RSI_IP_VERSION_6;
      memcpy(sntp_client->server_ip_address.ipv4_address, server_ip, RSI_IPV6_ADDRESS_LENGTH);
    }

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_sntp_client_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

    rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending |= SNTP_RESPONSE_PENDING;
    // send SNTP Get request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SNTP_CLIENT, pkt);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_SNTP_CLIENT_CREATE_ASYNC_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_SNTP_CLIENT_CREATE_ASYNC_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK12
* @{
*/
/*==============================================*/
/**
 * @brief       Get the current time parameters (NTP epoch time).
 * 		This is a blocking API.
 * @pre   \ref rsi_sntp_client_create_async() API needs to be called before this API. 
 * @param[in]   length        - Length of the buffer
 * @param[in]   sntp_time_rsp - Current time response
 * @return     0              -  Success  \n
 *             Negative Value - Failure \n
 *             -2             - Invalid parameters, callback not registered \n
 *             -3             - Command given in wrong state \n
 *             -4             - Buffer not available to serve the command 
 * @note        Refer to Error Codes section for more error codes \ref error-codes.
 */

int32_t rsi_sntp_client_gettime(uint16_t length, uint8_t *sntp_time_rsp)
{

  rsi_sntp_client_t *sntp_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_SNTP_CLIENT_GETTIME_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_SNTP_CLIENT_GETTIME_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_SNTP_CLIENT_GETTIME_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
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
      SL_PRINTF(SL_SNTP_CLIENT_GETTIME_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Attach the buffer given by user
    rsi_driver_cb_non_rom->nwk_app_buffer = (uint8_t *)sntp_time_rsp;

    // Length of the buffer provided by user
    rsi_driver_cb_non_rom->nwk_app_buffer_length = length;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_sntp_client_t));

    sntp_client = (rsi_sntp_client_t *)pkt->data;

    // Fill command type
    sntp_client->command_type = RSI_SNTP_GETTIME;

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_sntp_client_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

    // Send SNTP Get request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SNTP_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_SNTP_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_SNTP_CLIENT_GETTIME_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_SNTP_CLIENT_GETTIME_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK12
* @{
*/
/*==============================================*/
/**
 * @brief       Get current time in time date format parameters.
 *              This is a blocking API.
 * @pre   \ref rsi_sntp_client_create_async() API needs to be called before this API.
 * @param[in]   length             - Length of the buffer
 * @param[in]   sntp_time_date_rsp - Current time and date response
 * @return     0              -  Success  \n
 *             Negative Value - Failure \n
 *            -2              - Invalid parameters, callback not registered \n
 *            -3              - Command given in wrong state \n
 *            -4              - Buffer not available to serve the command
 * @note      Refer to Error Codes section for more error codes \ref error-codes.
 */

int32_t rsi_sntp_client_gettime_date(uint16_t length, uint8_t *sntp_time_date_rsp)
{
  rsi_sntp_client_t *sntp_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_SNTP_CLIENT_GETTIME_DATE_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_SNTP_CLIENT_GETTIME_DATE_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_SNTP_CLIENT_GETTIME_DATE_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
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
      SL_PRINTF(SL_SNTP_CLIENT_GETTIME_DATE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Attach the buffer given by user
    rsi_driver_cb_non_rom->nwk_app_buffer = (uint8_t *)sntp_time_date_rsp;

    // Length of the buffer provided by user
    rsi_driver_cb_non_rom->nwk_app_buffer_length = length;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_sntp_client_t));

    sntp_client = (rsi_sntp_client_t *)pkt->data;

    // Fill command type
    sntp_client->command_type = RSI_SNTP_GETTIME_DATE;

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_sntp_client_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

    // Send SNTP Get request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SNTP_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_SNTP_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_SNTP_CLIENT_GETTIME_DATE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_SNTP_CLIENT_GETTIME_DATE_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK12
* @{
*/
/*==============================================*/
/**
 * @brief       Get SNTP server info.
 *              This is a blocking API.
 * @pre   \ref rsi_sntp_client_create_async() API needs to be called before this API.
 * @param[in]   length               - Reponse buffer length
 * @param[in]   sntp_server_response - Pointer to the SNTP Reponse buffer
 *  @return     0              -  Success  \n
 *              Negative Value - Failure \n
 *              -2             - Invalid parameters, call back not registered \n
 *              -3             - Command given in wrong state \n
 *              -4             - Buffer not available to serve the command 
 *  @note       Refer to Error Codes section for more error codes \ref error-codes.
 */

int32_t rsi_sntp_client_server_info(uint16_t length, uint8_t *sntp_server_response)
{

  rsi_sntp_client_t *sntp_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_SNTP_CLIENT_SERVER_INFO_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // if state is not in card ready received state
  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_SNTP_CLIENT_SERVER_INFO_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_SNTP_CLIENT_SERVER_INFO_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
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
      SL_PRINTF(SL_SNTP_CLIENT_SERVER_INFO_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Attach the buffer given by user
    rsi_driver_cb_non_rom->nwk_app_buffer = (uint8_t *)sntp_server_response;

    // Length of the buffer provided by user
    rsi_driver_cb_non_rom->nwk_app_buffer_length = length;

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_sntp_client_t));

    sntp_client = (rsi_sntp_client_t *)pkt->data;

    // Fill command type
    sntp_client->command_type = RSI_SNTP_GET_SERVER_INFO;

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_sntp_client_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

    // Send SNTP Get request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SNTP_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_SNTP_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_SNTP_CLIENT_SERVER_INFO_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_SNTP_CLIENT_SERVER_INFO_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK12
* @{
*/
/*==============================================*/
/**
 * @brief       Delete SNTP client.
 *              This is a non-blocking API.
 * @pre   \ref rsi_sntp_client_create_async() API needs to be called before this API.
 * @param[in]   Void
 * @return     0              -  Success  \n
 *             Negative Value - Failure \n
 *             -2             - Invalid parameters, callback not registered \n
 *             -3             - Command given in wrong state \n
 *             -4             - Buffer not available to serve the command
 * @note      Refer to Error Codes section for more error codes \ref error-codes.
 */

int32_t rsi_sntp_client_delete_async(void)
{
  rsi_sntp_client_t *sntp_client;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_SNTP_CLIENT_DELETE_ASYNC_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_SNTP_CLIENT_DELETE_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_SNTP_CLIENT_DELETE_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
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
      SL_PRINTF(SL_SNTP_CLIENT_DELETE_ASYNC_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset the packet data
    memset(&pkt->data, 0, sizeof(rsi_sntp_client_t));

    sntp_client = (rsi_sntp_client_t *)pkt->data;

    // Fill command type
    sntp_client->command_type = RSI_SNTP_DELETE;

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_sntp_client_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

    rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending |= SNTP_RESPONSE_PENDING;
    // send SNTP Get request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SNTP_CLIENT, pkt);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_SNTP_CLIENT_DELETE_ASYNC_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_SNTP_CLIENT_DELETE_ASYNC_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */
