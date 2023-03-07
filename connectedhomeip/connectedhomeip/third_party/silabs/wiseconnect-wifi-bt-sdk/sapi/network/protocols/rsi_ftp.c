/*******************************************************************************
* @file  rsi_ftp.c
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

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported
 * @brief      Create FTP objects and connect to the FTP server on the given server port. This should be the first command for accessing FTP server. This is a blocking API.
 * @pre  \ref  rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]   flags      - Network flags. Each bit in the flag has its own significance \n
 *                                  
 *  Flags                  |          Description
 *  :----------------------|:-----------------------------------------------------------------------
 *  BIT(0) - RSI_IPV6      |    Set this bit to enable IPv6. By default, it is configured to IPv4
 *  BIT(1) to BIT(15)      |    Reserved for future use
 *
 * @param[in]  server_ip   - FTP server IP address to connect
 * @param[in]  username    - Username for server authentication
 * @param[in]  password    - Password for server authentication
 * @param[in]  server_port - Port number of FTP server \n
 * @note                     FTP server port is also configurable on a non-standard port
 * @return      0              - Success \n
 *              Non Zero Value - Failure \n
 *                         If return value is less than 0 \n
 *                         -3: Command given in wrong state \n
 *                         -4: Buffer not available to serve the command \n
 *                         If return value is greater than 0 \n
 *
 *                         0x0021,0x002C,0x0015
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_ftp_connect(uint16_t flags, int8_t *server_ip, int8_t *username, int8_t *password, uint32_t server_port)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_CONNECT_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_ops_t *file_ops = NULL;

  rsi_ftp_connect_t *ftp_connect = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_CONNECT_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_CONNECT_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_CONNECT_PKT_ALLOCATION_FAILURE_1, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    file_ops = (rsi_ftp_file_ops_t *)pkt->data;

    // Get command type as FTP Create
    file_ops->command_type = RSI_FTP_CREATE;

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP Create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    if (status != RSI_SUCCESS) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return status if error in sending command occurs
      SL_PRINTF(SL_FTP_CONNECT_ERROR_IN_SENDING_COMMAND_1, NETWORK, LOG_ERROR, "status: %4x", status);
      return status;
    }

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_CONNECT_PKT_ALLOCATION_FAILURE_2, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_connect = (rsi_ftp_connect_t *)pkt->data;

    // Memset the packet
    memset(pkt, 0, sizeof(rsi_ftp_connect_t));

    // Set command type as FTP connect
    ftp_connect->command_type = RSI_FTP_CONNECT;

    if (!(flags & RSI_IPV6)) {
      // Fill IP version
      ftp_connect->ip_version = RSI_IP_VERSION_4;

      // Fill IP address
      memcpy(ftp_connect->server_ip_address.ipv4_address, server_ip, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      // Fill IP version
      ftp_connect->ip_version = RSI_IP_VERSION_6;

      // Fill IPv6 address
      memcpy(ftp_connect->server_ip_address.ipv6_address, server_ip, RSI_IPV6_ADDRESS_LENGTH);
    }
    if (username) {
      // Copy login username
      rsi_strcpy(ftp_connect->username, username);
    }

    if (password) {
      // Copy login password
      rsi_strcpy(ftp_connect->password, password);
    }

    // Copy FTP Server port
    rsi_uint32_to_4bytes(ftp_connect->server_port, server_port);

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP Connect command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_CONNECT_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_CONNECT_ERROR_IN_SENDING_COMMAND_2, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported
 * @brief      Disconnect from the FTP server and destroy the FTP objects. Once the FTP objects are
 *             destroyed, FTP server cannot be accessed. For further accessing, FTP objects should be created again. This is a blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API.
 * @return      0              - Success \n
 *              Non Zero Value - Failure \n
 *                               If return value is less than 0 \n
 *				            -3 - Command given in wrong state \n
 *				            -4 - Buffer not available to serve the command \n
 *		                 If return value is greater than 0 \n
 *
 *		                 0x0021,0x002C,0x0015		             
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 *
 */
int32_t rsi_ftp_disconnect(void)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_DISCONNECT_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_ops_t *ftp_ops = NULL;

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
      SL_PRINTF(SL_FTP_DISCONNECT_COMMAND_GIVEN_IN_WRONG_STATE, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_DISCONNECT_PKT_ALLOCATION_FAILURE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_ops = (rsi_ftp_file_ops_t *)pkt->data;

    // Get command type as FTP Create
    ftp_ops->command_type = RSI_FTP_DISCONNECT;

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP Create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    if (status != RSI_SUCCESS) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return status if error in sending command occurs
      SL_PRINTF(SL_FTP_DISCONNECT_ERROR_IN_SENDING_COMMAND_1, NETWORK, LOG_ERROR, "status: %4x", status);
      return status;
    }
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_DISCONNECT_PKT_ALLOCATION_FAILURE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_ops = (rsi_ftp_file_ops_t *)pkt->data;

    // Memset the packet
    memset(pkt, 0, sizeof(rsi_ftp_file_ops_t));

    // Get command type as FTP Create
    ftp_ops->command_type = RSI_FTP_DESTROY;

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP Create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_DISCONNECT_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_DISCONNECT_ERROR_IN_SENDING_COMMAND_2, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported
 * @brief      Open a file in the specified path on the FTP server. This is a blocking API.
 * @pre   \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  file_name - Filename or filename with the path
 * @return      0              - Success \n
 *              Non Zero Value - Failure \n
 *                               If return value is less than 0 \n
 *                              -3 - Command given in wrong state \n
 *				-4 - Buffer not available to serve the command \n 
 *				If return value is greater than 0 \n
 *
 *				0x0021,0x002C,0x0015
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_ftp_file_write(int8_t *file_name)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_FILE_WRITE_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_ops_t *ftp_ops = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_WRITE_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_WRITE_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_FILE_WRITE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_ops = (rsi_ftp_file_ops_t *)pkt->data;

    // Get command type as FTP file write
    ftp_ops->command_type = RSI_FTP_FILE_WRITE;

    // Copy the filename/path
    rsi_strcpy(ftp_ops->file_name, file_name);

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send FTP command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_FILE_WRITE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_FILE_WRITE_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note        This API is not supported
 * @brief       Write the content into the file that was opened using \ref rsi_ftp_file_write() API. This is a blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]   flags          - Network flags.  \n
 *  
 *  Flags                  |          Description
 *  :----------------------|:-----------------------------------------------------------------------
 *  BIT(0) - RSI_IPV6      |    Set this bit to enable IPv6. By default, it is configured to IPv4
 *  BIT(1) to BIT(15)      |    Reserved for future use
 * 
 * @param[in]   file_content   - Data stream to be written into the file
 * @param[in]   content_length - File content length
 * @param[in]   end_of_file    - Flag that indicates the end of file \n
 *                                1 – Represent the end of content to be written into the file \n
 *                                0 – Extra data that is pending to write into the file \n
 * @note                      (a) This API can be called multiple times to append data into the same file. \n
 *                            (b) File content length should not exceed 1344 bytes in case of IPV4 \n 
 *                                and 1324 bytes in case of IPV6.If exceeds, this API will break the file \n 
 *                                content and send it in multiple packets.
 * @return      0              -  Success \n
 *              Non Zero Value -  Failure \n
 *                                If return value is less than 0 \n
 *                                -3: Command given in wrong state \n
 *                                -4: Buffer not available to serve the command \n
 *                                If return value is greater than 0 \n
 *
 *                                0x0021,0x002C,0x0015 
 */
int32_t rsi_ftp_file_write_content(uint16_t flags, int8_t *file_content, int16_t content_length, uint8_t end_of_file)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_FILE_WRITE_CONTENT_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_write_t *ftp_file_write = NULL;

  rsi_pkt_t *pkt;

  uint32_t file_offset = 0;

  uint32_t chunk_size = 0;

  uint32_t send_size = 0;

  uint8_t head_room = 0;

  uint8_t *host_desc = NULL;

  if (!(flags & RSI_IPV6)) {
    // Headroom for IPv4
    head_room = RSI_TCP_FRAME_HEADER_LEN;
  } else {
    // Headroom for IPv6
    head_room = RSI_TCP_V6_FRAME_HEADER_LEN;
  }

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_WRITE_CONTENT_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_WRITE_CONTENT_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    while (content_length) {
      // Allocate command buffer from WLAN pool
      pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
      // If allocation of packet fails
      if (pkt == NULL) {
        // Change NWK state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        // Return packet allocation failure error
        SL_PRINTF(SL_FTP_FILE_WRITE_CONTENT_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
        return RSI_ERROR_PKT_ALLOCATION_FAILURE;
      }

      ftp_file_write = (rsi_ftp_file_write_t *)pkt->data;

      // Memset the packet
      memset(pkt, 0, (sizeof(pkt) + sizeof(rsi_ftp_file_write_t)));

      // Get command type as file delete
      ftp_file_write->command_type = RSI_FTP_FILE_WRITE_CONTENT;

      if (content_length > RSI_FTP_MAX_CHUNK_LENGTH) {
        chunk_size = RSI_FTP_MAX_CHUNK_LENGTH;
#ifndef RSI_UART_INTERFACE
        rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_ASYNCHRONOUS;
#endif

        // Copy end of file
        ftp_file_write->end_of_file = 0;

      } else {
        chunk_size = content_length;

        // Copy end of file
        ftp_file_write->end_of_file = end_of_file;

#ifndef RSI_UART_INTERFACE
        if (!end_of_file) {
          rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_ASYNCHRONOUS;
        }
#endif
      }
      // Copy file content
      memcpy((((uint8_t *)ftp_file_write) + head_room), (file_content + file_offset), chunk_size);

      // Add headroom to send size
      send_size = chunk_size + head_room;

      // Get the host descriptor
      host_desc = (pkt->desc);

      // Fill data length in the packet host descriptor
      rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

      if (rsi_driver_cb->wlan_cb->expected_response != RSI_WLAN_RSP_ASYNCHRONOUS) {
#ifndef RSI_NWK_SEM_BITMAP
        rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
      }
      // Send set FTP Create command
      status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

      if (rsi_driver_cb->wlan_cb->expected_response != RSI_WLAN_RSP_ASYNCHRONOUS) {
        // Wait on NWK semaphore
        rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_nwk_status();
        // If failed, do not send other chunks
        if (status != RSI_SUCCESS) {
          break;
        }
      }
      // Increase file offset
      file_offset += chunk_size;

      // Decrease file remaining size
      content_length -= chunk_size;
    }

    // Change NWK state to allow
    // When end_of_file is 0, we will need to change the state to allow, as there is no response expected from the firmware
    // When end_of_file is 1, wait_on_nwk_semaphore is already done, so we can allow other nwk commands.
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_FILE_WRITE_CONTENT_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_FILE_WRITE_CONTENT_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note         This API is not supported
 * @brief        Read the content from the specified file on the FTP server. This is a non-blocking API.
 * @pre  \ref    rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]    file_name              - Filename or filename with path
 * @param[in]    call_back_handler_ptr  - Called when asynchronous response is received from module for file read request
 * @param[out]   status                 - Success - RSI_SUCCESS\n
 *                                        Failure - Negative value\n
 *                                                  -2 : Invalid parameter,expects call back handler \n
 *                                                  -3 : Command given in wrong state \n
 *                                                  -4 : Buffer not available to serve the command \n
 *                                                 Other Possible error codes are: 0x0021,0x002C,0x0015
 *                                                 Other parameters of the callback are valid only if status is 0
 * @param[out]   file_content           -  File content
 * @param[out]   content_length         -  Length of file content
 * @param[out]   end_of_file            -  Indicate the end of file \n
 *                                        1 – No more data \n
 *                                        0 – More data present
 * @return      Status of the call_back_handler_ptr is returned
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_ftp_file_read_aysnc(
  int8_t *file_name,
  void (*call_back_handler_ptr)(uint16_t status, uint8_t *file_content, uint16_t content_length, uint8_t end_of_file))
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_FILE_READ_ASYNC_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_ops_t *ftp_ops = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_READ_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_READ_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    if (call_back_handler_ptr != NULL) {
      // Register FTP file read response notify call back handler
      rsi_wlan_cb_non_rom->nwk_callbacks.ftp_file_read_call_back_handler = call_back_handler_ptr;
    } else {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return invalid command error
      SL_PRINTF(SL_FTP_FILE_READ_ASYNC_INVALID_PARAM, NETWORK, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_FILE_READ_ASYNC_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_ops = (rsi_ftp_file_ops_t *)pkt->data;

    // Get command type as FTP file Read
    ftp_ops->command_type = RSI_FTP_FILE_READ;

    // Copy the filename/path
    rsi_strcpy(ftp_ops->file_name, file_name);

    // Send FTP command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_FILE_READ_ASYNC_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_FILE_READ_ASYNC_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}
/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported
 * @brief      Delete the file that is present in the specified path on the FTP server. This is a blocking API.
 * @pre  \ref  rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  file_name - Filename or filename with path to delete
 * @return      0             - Success \n
 *             Non Zero Value - Failure \n
 *                          If return value is less than zero
 *                         -3 - Command given in wrong state \n
 *                         -4 - Buffer not available to serve the command \n
 *                         If return value is greater than 0 \n
 *
 *                         0x0021,0x002C,0x0015
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_ftp_file_delete(int8_t *file_name)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_FILE_DELETE_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_ops_t *ftp_ops = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_DELETE_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_DELETE_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_FILE_DELETE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_ops = (rsi_ftp_file_ops_t *)pkt->data;

    // Get command type as file delete
    ftp_ops->command_type = RSI_FTP_FILE_DELETE;

    // Copy the filename/path to delete
    rsi_strcpy(ftp_ops->file_name, file_name);

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP Create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_FILE_DELETE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_FILE_DELETE_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported
 * @brief      Rename the file with a new name on the FTP server. This is a blocking API.
 * @pre  \ref  rsi_config_ipaddress() API needs to be called before this API 
 * @param[in]  old_file_name - Filename or filename with path to rename
 * @param[in]  new_file_name - New filename
 * @return     0              -  Success  \n
 *             Negative Value - Failure \n
 *                          If return value is less than 0 \n
 *                         -3 - Command given in wrong state \n 
 *                         -4 - Buffer not available to serve the command \n
 *                         If return value is greater than 0 \n
 *
 *                         0x0021,0x002C,0x0015
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_ftp_file_rename(int8_t *old_file_name, int8_t *new_file_name)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_FILE_RENAME_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_rename_t *file_rename = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_RENAME_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_RENAME_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_FILE_RENAME_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    file_rename = (rsi_ftp_file_rename_t *)pkt->data;

    // Get command type as file rename
    file_rename->command_type = RSI_FTP_FILE_RENAME;

    // Copy the filename/path of old filename
    rsi_strcpy(file_rename->old_file_name, old_file_name);

    // Copy the filename/path of new filename
    rsi_strcpy(file_rename->new_file_name, new_file_name);

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP Create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_FILE_RENAME_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_FILE_RENAME_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported
 * @brief      Create a directory on the FTP server. This is a blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  directory_name - Directory name (with path if required) to create
 * @return     0              -  Success  \n
 *             Non Zero Value - Failure \n
 *                         If return value is less than 0 \n
 *                         -3 - Command given in wrong state \n 
 *                         -4 - Buffer not available to serve the command \n
 *                         If return value is greater than 0 \n
 *
 *                         0x0021,0x002C,0x0015
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_ftp_directory_create(int8_t *directory_name)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_DIRECTORY_CREATE_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_ops_t *ftp_ops = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_DIRECTORY_CREATE_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_DIRECTORY_CREATE_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_DIRECTORY_CREATE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_ops = (rsi_ftp_file_ops_t *)pkt->data;

    // Get command type as directory create
    ftp_ops->command_type = RSI_FTP_DIRECTORY_CREATE;

    // Copy the directory name to create
    rsi_strcpy(ftp_ops->file_name, directory_name);

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP Create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_DIRECTORY_CREATE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_DIRECTORY_CREATE_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported
 * @brief      Delete the directory on the FTP server. This is a blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API 
 * @param[in]  directory_name - Directory name (with path if required) to delete
 * @return     0              -  Success  \n
 *             Non Zero Value - Failure \n
 *                              If return value is less than 0 \n
 *                         -3 - Command given in wrong state \n 
 *                         -4 - Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *
 *                              0x0021,0x002C,0x0015
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_ftp_directory_delete(int8_t *directory_name)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_DIRECTORY_DELETE_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_ops_t *ftp_ops = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_DIRECTORY_DELETE_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_DIRECTORY_DELETE_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_DIRECTORY_DELETE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_ops = (rsi_ftp_file_ops_t *)pkt->data;

    // Get command type as directory delete
    ftp_ops->command_type = RSI_FTP_DIRECTORY_DELETE;

    // Copy the filename/path to delete
    rsi_strcpy(ftp_ops->file_name, directory_name);

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // send set FTP  command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_DIRECTORY_DELETE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_DIRECTORY_DELETE_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported
 * @brief      Change the current working directory to the specified directory path on the FTP server. This is a blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  directory_path - Directory name (with path if required) to create
 * @return     0              -  Success  \n
 *             Non Zero Value -  Failure \n
 *                              If return value is less than 0
 *                         -3 - Command given in wrong state \n 
 *                         -4 - Buffer not available to serve the command \n
 *                         If return value is greater than 0 \n
 *
 *                         0x0021,0x002C,0x0015
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_ftp_directory_set(int8_t *directory_path)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_DIRECTORY_SET_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_ops_t *ftp_ops = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_DIRECTORY_SET_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_DIRECTORY_SET_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_DIRECTORY_SET_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_ops = (rsi_ftp_file_ops_t *)pkt->data;

    // Get command type as directory delete
    ftp_ops->command_type = RSI_FTP_DIRECTORY_SET;

    // Copy the filename/path to delete
    rsi_strcpy(ftp_ops->file_name, directory_path);

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP Create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_DIRECTORY_SET_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_DIRECTORY_SET_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note        This API is not supported
 * @brief       Get the list of directories present in the specified directory on the FTP server. This is a non-blocking API.
 * @pre         \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]   directory_path        - Directory path(with path if required) to list
 * @param[in]   call_back_handler_ptr -  Called when asynchronous response is received from module for directory list request
 * @param[out]  status                -  Success - RSI_SUCCESS\n
 *                                       Failure - Negative value\n
 *                                                  -2 : Invalid parameter, expects call back handler \n
 *                                                  -3 : Command given in wrong state \n
 *                                                  -4 : Buffer not available to serve the command \n
 *                                                 Other possible error codes are: 0x0021,0x002C,0x0015
 *                                                 Other parameters of callback are valid only if status is 0
 * @param[out]  directory_list        -  Stream of data with directory list as content
 * @param[out]  length                -  Length of content
 * @param[out]  end_of_list           -  Indicate end of list \n
 *                                      1 – No more data \n
 *                                      0 – More data present
 * @return      Status of the call_back_handler_ptr is returned
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_ftp_directory_list_async(
  int8_t *directory_path,
  void (*call_back_handler_ptr)(uint16_t status, uint8_t *directory_list, uint16_t length, uint8_t end_of_list))
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_DIRECTORY_LIST_ASYNC_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_ops_t *ftp_ops = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_DIRECTORY_LIST_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_DIRECTORY_LIST_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    if (call_back_handler_ptr != NULL) {
      // Register FTP directory list response notify call back handler
      rsi_wlan_cb_non_rom->nwk_callbacks.ftp_directory_list_call_back_handler = call_back_handler_ptr;
    } else {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return invalid command error
      SL_PRINTF(SL_FTP_DIRECTORY_LIST_ASYNC_INVALID_PARAM, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_INVALID_PARAM;
    }
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_DIRECTORY_LIST_ASYNC_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_ops = (rsi_ftp_file_ops_t *)pkt->data;

    // Get command type as directory list
    ftp_ops->command_type = RSI_FTP_DIRECTORY_LIST;

    // copy the directory path to list
    rsi_strcpy(ftp_ops->file_name, directory_path);

    // Send set FTP command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_DIRECTORY_LIST_ASYNC_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_DIRECTORY_LIST_ASYNC_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported
 * @brief      Set the FTP client mode - either in Passive mode or Active Mode.
 *             In active FTP, client establishes the command channel and the server establishes the data channel.
 *             In passive FTP, both the command channel and the data channel are established by the client.
 *             Set the FTP Transfer mode - either in Block transfer mode or Stream transfer Mode.
 *             In stream transfer mode, Data is sent as a continuous stream
 *             In block transfer mode, FTP puts each record (or line) of data into several blocks and sent it on to TCP.
 *             This is a blocking API.
 * @pre   \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  mode - Used to select the mode of FTP client if FTP is enabled \n
 *        In mode variable, BIT(0) refers FTP client mode and BIT(1) refers FTP transfer mode \n
 *		  BIT(0) is 0 then active mode is enabled \n
 *        BIT(0) is 1 then passive mode is enabled \n
 *        BIT(1) is 0 then stream transfer mode is enabled \n
 *        BIT(1) is 1 then block transfer mode is enabled
 * @return     0              -  Success  \n
 *             Negative Value - Failure \n
 *                         -2 - Invalid parameter \n
 *                         -3 - Command given in wrong state \n 
 *                         -4 - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_ftp_mode_set(uint8_t mode)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_MODE_SET_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_mode_params_t *ftp_mode = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_MODE_SET_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_MODE_SET_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_MODE_SET_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_mode = (rsi_ftp_mode_params_t *)pkt->data;

    ftp_mode->command_type = RSI_FTP_COMMAND_MODE_SET;
    ftp_mode->mode_type    = mode;

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP Create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_MODE_SET_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_MODE_SET_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}
/** @} */

/** @addtogroup NETWORK8
* @{
*/
/*==============================================*/
/**
 * @note       This API is supported only in 9117
 *             This API should called before every \ref rsi_ftp_file_write_content() when the Block transfer mode is in use
 * @brief      Set the file size
 * @pre   \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  file_size - represents the size of the file in bytes that is to be transferred. \n
 * @return     0              -  Success  \n
 *             Negative Value - Failure \n
 *                         -2 - Invalid parameter \n
 *                         -3 - Command given in wrong state \n 
 *                         -4 - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
#ifdef CHIP_9117
int32_t rsi_ftp_file_size_set(uint32_t file_size)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_FTP_FILE_SIZE_SET_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_ftp_file_size_set_params_t *ftp_size_set = NULL;

  if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE || wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
    // In concurrent mode or AP mode, state should be in RSI_WLAN_STATE_CONNECTED to accept this command
    if ((wlan_cb->state < RSI_WLAN_STATE_CONNECTED)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_SIZE_SET_COMMAND_GIVEN_IN_WRONG_STATE_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_FTP_FILE_SIZE_SET_COMMAND_GIVEN_IN_WRONG_STATE_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_FTP_FILE_SIZE_SET_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    ftp_size_set = (rsi_ftp_file_size_set_params_t *)pkt->data;

    ftp_size_set->command_type = RSI_FTP_COMMAND_FILE_SIZE_SET;

    rsi_uint32_to_4bytes(ftp_size_set->file_size, file_size);
#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP Create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FTP, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_FTP_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_FTP_FILE_SIZE_SET_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_FTP_FILE_SIZE_SET_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}
#endif
/** @} */
