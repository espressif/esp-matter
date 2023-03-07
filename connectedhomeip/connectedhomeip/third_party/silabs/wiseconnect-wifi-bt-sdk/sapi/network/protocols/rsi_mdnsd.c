/*******************************************************************************
* @file  rsi_mdnsd.c
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
#include "rsi_mdnsd.h"
/** @addtogroup NETWORK15
* @{
*/
/*==============================================*/
/**
 * @brief            Initialize the MDNSD service in WiSeConnect device.It creates MDNS daemon. This is a blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]        ip_version - Select the IP version. 4 – to select IPv4 \n 6 – to select IPv6
 * @param[in]        ttl        - Time to live in seconds for which service should be active
 * @param[in]        host_name  - Host name that is used as host name in Type A record.
 * @return      0               - Success \n
 *              Negative value  - Failure
 */
int32_t rsi_mdnsd_init(uint8_t ip_version, uint16_t ttl, uint8_t *host_name)
{
  rsi_req_mdnsd_t *mdnsd;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_MSNSD_INIT_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if ((rsi_strlen(host_name) + 1) > MDNSD_BUFFER_SIZE) {
    SL_PRINTF(SL_MSNSD_INIT_INSUFFICIENT_BUFFER, NETWORK, LOG_ERROR);
    return RSI_ERROR_INSUFFICIENT_BUFFER;
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
      SL_PRINTF(SL_MSNSD_INIT_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mdnsd = (rsi_req_mdnsd_t *)pkt->data;

    // Memset the packet data to insert NULL between fields
    memset(&pkt->data, 0, sizeof(rsi_req_mdnsd_t));

    // Fill command type
    mdnsd->command_type = RSI_MDNSD_INIT;

    // Fill IP version
    mdnsd->mdnsd_struct.mdnsd_init.ip_version = ip_version;

    // Fill time to live
    rsi_uint16_to_2bytes(mdnsd->mdnsd_struct.mdnsd_init.ttl, ttl);

    // Copy host name
    rsi_strcpy(mdnsd->buffer, host_name);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_req_mdnsd_t) - MDNSD_BUFFER_SIZE + rsi_strlen(mdnsd->buffer) + 1;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

    // Send MDNSD request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_MDNSD, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_MDNSD_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_MSNSD_INIT_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_MSNSD_INIT_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK15
* @{
*/
#ifndef CHIP_9117
/*==============================================*/
/**
 * @brief      Add a service/start service discovery.This is a blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API
 * @param[in]  port             - Port number on which service should be added.
 * @param[in]  ttl              - Time to live. This is time in seconds for which service should be active
 * @param[in]  more             - This byte should be set to '1' when there are more services to add \n
 *                                 0 - This is last service, starts MDNS service. \n
 *                                 1 - Still more services will be added.
 * @param[in]  service_ptr_name - Name to be added in Type-PTR record
 * @param[in]  service_name     - Name to be added in Type-SRV record (service name)
 * @param[in]  service_text     - Text field to be added in Type-TXT record
 * @return      0               - Success \n
 *              Negative value  - Failure
 * @note        Currently, only one service registration is supported
 */

int32_t rsi_mdnsd_register_service(uint16_t port,
                                   uint16_t ttl,
                                   uint8_t more,
                                   uint8_t *service_ptr_name,
                                   uint8_t *service_name,
                                   uint8_t *service_text)
{
  rsi_req_mdnsd_t *mdnsd;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_MSNSD_REGISTER_SERVICE_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if ((rsi_strlen(service_ptr_name) + rsi_strlen(service_name) + rsi_strlen(service_text) + 3) > MDNSD_BUFFER_SIZE) {
    SL_PRINTF(SL_MSNSD_REGISTER_SERVICE_INSUFFICIENT_BUFFER, NETWORK, LOG_ERROR);
    return RSI_ERROR_INSUFFICIENT_BUFFER;
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
      SL_PRINTF(SL_MSNSD_REGISTER_SERVICE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mdnsd = (rsi_req_mdnsd_t *)pkt->data;

    // Memset the packet data to insert NULL between fields
    memset(&pkt->data, 0, sizeof(rsi_req_mdnsd_t));

    // Fill command type
    mdnsd->command_type = RSI_MDNSD_REGISTER_SERVICE;

    // Fill port number
    rsi_uint16_to_2bytes(mdnsd->mdnsd_struct.mdnsd_register_service.port, port);

    // Fill time to live
    rsi_uint16_to_2bytes(mdnsd->mdnsd_struct.mdnsd_register_service.ttl, ttl);

    // More
    mdnsd->mdnsd_struct.mdnsd_register_service.more = more;

    // Copy service pointer name
    rsi_strcpy(mdnsd->buffer, service_ptr_name);
    send_size = rsi_strlen(service_ptr_name) + 1;

    // Copy service name
    rsi_strcpy((mdnsd->buffer) + send_size, service_name);
    send_size += rsi_strlen(service_name) + 1;

    // Copy service text
    rsi_strcpy((mdnsd->buffer) + send_size, service_text);
    send_size += rsi_strlen(service_text) + 1;

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_req_mdnsd_t) - MDNSD_BUFFER_SIZE + send_size;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

    // Send MDNSD request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_MDNSD, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_MDNSD_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_MSNSD_REGISTER_SERVICE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_MSNSD_REGISTER_SERVICE_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}
#else

/*==============================================*/
/**
 * @brief      Add a service/start service discovery.This is a blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API
 * @param[in]  port             - Port number on which service should be added.
 * @param[in]  ttl              - Time to live. This is time in seconds for which service should be active
 * @param[in]  more             - This byte should be set to '1' when there are more services to add \n
 *                                 0 - This is last service, starts MDNS service. \n
 *                                 1 - Still more services will be added.
 * @param[in]  type             - Name to be added in Type field
 * @param[in]  sub_type             - Name to be added in SubType field
 * @param[in]  service_name     - Name to be added in Type-SRV record (service name)
 * @param[in]  service_text     - Text field to be added in Type-TXT record
 * @return      0               - Success \n
 *              Negative value  - Failure
 * @note        Currently, only one service registration is supported
 */

int32_t rsi_mdnsd_register_service(uint16_t port,
                                   uint16_t ttl,
                                   uint8_t *type,
                                   uint8_t *sub_type,
                                   uint8_t *service_name,
                                   uint8_t *service_text)
{
  rsi_req_mdnsd_t *mdnsd;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_MSNSD_REGISTER_SERVICE_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if ((rsi_strlen(type) + rsi_strlen(sub_type) + rsi_strlen(service_name) + rsi_strlen(service_text) + 3)
      > MDNSD_BUFFER_SIZE) {
    SL_PRINTF(SL_MSNSD_REGISTER_SERVICE_INSUFFICIENT_BUFFER, NETWORK, LOG_ERROR);
    return RSI_ERROR_INSUFFICIENT_BUFFER;
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
      SL_PRINTF(SL_MSNSD_REGISTER_SERVICE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mdnsd = (rsi_req_mdnsd_t *)pkt->data;

    // Memset the packet data to insert NULL between fields
    memset(&pkt->data, 0, sizeof(rsi_req_mdnsd_t));

    // Fill command type
    mdnsd->command_type = RSI_MDNSD_REGISTER_SERVICE;

    // Fill port number
    rsi_uint16_to_2bytes(mdnsd->mdnsd_struct.mdnsd_register_service.port, port);

    // Fill time to live
    rsi_uint16_to_2bytes(mdnsd->mdnsd_struct.mdnsd_register_service.ttl, ttl);

    //Copy Type
    rsi_strcpy(mdnsd->buffer, type);
    send_size = rsi_strlen(type) + 1;

    //Copy subtype
    // Copy service pointer name
    rsi_strcpy(mdnsd->buffer, sub_type);
    send_size += rsi_strlen(sub_type) + 1;

    // Copy service name
    rsi_strcpy((mdnsd->buffer) + send_size, service_name);
    send_size += rsi_strlen(service_name) + 1;

    // Copy service text
    rsi_strcpy((mdnsd->buffer) + send_size, service_text);
    send_size += rsi_strlen(service_text) + 1;

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_req_mdnsd_t) - MDNSD_BUFFER_SIZE + send_size;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

    // Send MDNSD request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_MDNSD, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_MDNSD_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_MSNSD_REGISTER_SERVICE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_MSNSD_REGISTER_SERVICE_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

#endif
/** @} */

/** @addtogroup NETWORK15
* @{
*/
/*==============================================*/
/**
 * @brief      Delete the MDNSD service. This is a blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  void  
 * @return     0              - Success \n
 *             Negative value - Failure
 */

int32_t rsi_mdnsd_deinit(void)
{
  rsi_req_mdnsd_t *mdnsd;
  rsi_pkt_t *pkt;
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  SL_PRINTF(SL_MSNSD_DEINIT_ENTRY, NETWORK, LOG_INFO);
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
      SL_PRINTF(SL_MSNSD_DEINIT_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mdnsd = (rsi_req_mdnsd_t *)pkt->data;

    // Memset the packet data to insert NULL between fields
    memset(&pkt->data, 0, sizeof(rsi_req_mdnsd_t));

    // Fill command type
    mdnsd->command_type = RSI_MDNSD_DEINIT;

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_req_mdnsd_t) - MDNSD_BUFFER_SIZE;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif

    // Send MDNSD request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_MDNSD, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_MDNSD_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();

    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_MSNSD_INIT_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_MSNSD_INIT_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */
