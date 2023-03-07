/*******************************************************************************
* @file  rsi_ota_fw_up.c
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
#include "rsi_firmware_upgradation.h"

/** @addtogroup NETWORK20
* @{
*/
/*==============================================*/
/**
 * @brief       Create an OTAF client. Initialize the client with a given configuration. This is a non-blocking API.
 * @pre         \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]   flags                      - Select the IP version. \n
 *                                 BIT(0)  – RSI_IPV6. Set this bit to enable IPv6 , by default it is configured to IPv4
 * @param[in]   server_ip                  - OTAF server IP address
 * @param[in]   server_port                - OTAF server port number
 * @param[in]   chunk_number               - Firmware content request chunk number
 * @param[in]   timeout                    - TCP receive packet timeout
 * @param[in]   tcp_retry_count            - TCP retransmissions count
 * @param[in]   ota_fw_up_response_handler - Callback when asynchronous response is received from module for firmware update request.
 *                                           Callback parameters: status and chunk_number
 * @param[out]  status                     - Success - RSI_SUCCESS\n
 *                                           Failure - Negative value\n
 *                                           -3 : Command given in wrong state \n
 *                                           -4 : Buffer not available to serve the command
 * @param[out]  chunk_number               - Chunk number of the firmware content
 * @note        For safe firmware upgrade via TCP server, \n
 *              it will take approx. 65 sec duration for upgrading the firmware of 1.5 MB file. 
 * @return      0                          - Success  \n
 *              Negative Value             - Failure \n
 *                          -3             - Command given in wrong state \n
 *                          -4             - Buffer not available to serve the command
 */

int32_t rsi_ota_firmware_upgradation(uint8_t flags,
                                     uint8_t *server_ip,
                                     uint32_t server_port,
                                     uint16_t chunk_number,
                                     uint16_t timeout,
                                     uint16_t tcp_retry_count,
                                     void (*ota_fw_up_response_handler)(uint16_t status, uint16_t chunk_number))
{
  int32_t status = RSI_SUCCESS;

  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_req_ota_fwup_t *otaf_fwup = NULL;

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Register callback
    if (ota_fw_up_response_handler != NULL) {
      // Register SMTP client response notify call back handler
      rsi_wlan_cb_non_rom->nwk_callbacks.rsi_ota_fw_up_response_handler = ota_fw_up_response_handler;
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

    otaf_fwup = (rsi_req_ota_fwup_t *)pkt->data;

    // Check IP version
    if (!(flags & RSI_IPV6)) {
      // Fill the IP version
      otaf_fwup->ip_version = RSI_IP_VERSION_4;

      // Set IP address to localhost
      memcpy(otaf_fwup->server_ip_address.ipv4_address, server_ip, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      // Fill the IP version
      otaf_fwup->ip_version = RSI_IP_VERSION_6;

      // Set IP address to localhost
      memcpy(otaf_fwup->server_ip_address.ipv6_address, server_ip, RSI_IPV6_ADDRESS_LENGTH);
    }

    // Fill server port number
    rsi_uint32_to_4bytes(otaf_fwup->server_port, server_port);

    // Fill chunk number
    rsi_uint16_to_2bytes(otaf_fwup->chunk_number, chunk_number);

    // Fill timeout
    rsi_uint16_to_2bytes(otaf_fwup->timeout, timeout);

    // Fill TCP retry count
    rsi_uint16_to_2bytes(otaf_fwup->retry_count, tcp_retry_count);

    // Send OTAF command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_OTA_FWUP, pkt);

  } else {
    // Return NWK command error
    return status;
  }

  // Return status if error in sending command occurs
  return status;
}
/** @} */
