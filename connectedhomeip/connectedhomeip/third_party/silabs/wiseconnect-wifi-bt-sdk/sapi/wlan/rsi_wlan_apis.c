/*******************************************************************************
* @file  rsi_wlan_apis.c
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
#include "rsi_wlan_non_rom.h"
#define GAIN_TABLE_MAX_PAYLOAD_LEN 128

extern struct wpa_scan_results_arr *scan_results_array;

/** @addtogroup WLAN
* @{
*/

#ifdef PROCESS_SCAN_RESULTS_AT_HOST
/*==============================================*/
/**
 * @brief       Sort the scan results in 'scan_results_array' in the order of RSSI.
 *              Application should call this API to get the scan result in sorted order.
 * @pre         \ref rsi_wlan_scan_with_bitmap_options() API needs to be called before this API.
 * @param[in]   scan_results_array     - Contains the array of scan results that need to be sorted.
 * @return      Void
 */
void rsi_sort_scan_results_array_based_on_rssi(struct wpa_scan_results_arr *scan_results_array)
{
  SL_PRINTF(SL_SORT_SCAN_RESULTS_ENTRY, WLAN, LOG_INFO);
  uint16_t i = 0, j = 0;
  struct wpa_scan_res temp;
  memset(&temp, 0, sizeof(struct wpa_scan_res));
  if (scan_results_array != NULL) {
    for (i = 0; i < scan_results_array->num; i++) {
      for (j = 0; j < ((scan_results_array->num) - i - 1); j++) {
        if (scan_results_array->res[j].level < scan_results_array->res[j + 1].level) {
          memcpy(&temp, &(scan_results_array->res[j]), sizeof(struct wpa_scan_res));
          memcpy(&(scan_results_array->res[j]), &(scan_results_array->res[j + 1]), sizeof(struct wpa_scan_res));
          memcpy(&(scan_results_array->res[j + 1]), &temp, sizeof(struct wpa_scan_res));
        }
      }
    }
  }
  SL_PRINTF(SL_SORT_SCAN_RESULTS_EXIT, WLAN, LOG_INFO);
}
#endif

/*==============================================*/
/**
 * @brief       Scan available access points and post scan response to application.
 *              Application must call this API to get the scan results. This is a blocking API.
 * @pre         \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]  ssid        - SSID of an access point to connect. \n SSID should be less than or equal to 32 bytes. \n
 *                             Note:
 *                             SSID is a null terminated string.
 * @param[in]  chno        - Channel number of the access point.
 * @param[in]  result      - Buffer address provided by the application to fill the scan response.
 * @param[in]  length      - Length of the resulting buffer measured in bytes to hold scan results.\n
 *                           Size of structure \ref rsi_rsp_scan_t is 54 bytes.\n
 *                           Length for storing one scan result is sizeof( \ref rsi_scan_info_t),
 *                           which is 44 bytes. \n
 *                           Maximum of 11 scan results will be returned by the
 *                           module, \n in this case, length should be configured
 *                           as  8 + 11*sizeof( \ref rsi_scan_info_t).
 * @param[in]  scan_bitmap    - Scan bitmap options
 * @return     0              -    Success \n
 *             Non-Zero Value -    Failure
 *
 */
int32_t rsi_wlan_scan_with_bitmap_options(int8_t *ssid,
                                          uint8_t chno,
                                          rsi_rsp_scan_t *result,
                                          uint32_t length,
                                          uint32_t scan_bitmap)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_SCAN_BITMAP_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE || wlan_cb->state >= RSI_WLAN_STATE_CONNECTED) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SCAN_BITMAP_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)result;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    // Call async scan to perform actual scan
    status = rsi_wlan_scan_async_with_bitmap_options(ssid, chno, scan_bitmap, NULL);

    if (status != RSI_SUCCESS) {
      // Return status if error in sending command occurs
      SL_PRINTF(SL_WLAN_SCAN_BITMAP_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR);
      return status;
    }

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SCAN_WITH_BITMAP_RESPONSE_WAIT_TIME);

    // Change WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SCAN_BITMAP_WLAN_COMMAND_ERROR, WLAN, LOG_INFO, "status: %4x", status);

    return status;
  }
  // Return status
  SL_PRINTF(SL_WLAN_SCAN_BITMAP_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief               Scan available access points and post scan response to application.
 *                      Application must call this API to get the scan results.
 *                      This is a non-blocking API.
 * @pre                 \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]   ssid     - SSID of an access point to connect. \n
 *                        SSID should be less than or equal to 32 bytes.
 * @param[in]   chno     - Channel number of the access point.
 * @param[in]   bitmap   - Scan feature bitmap
 * @param[out]  status   - Response status
 * @param[out]  buffer   - Buffer address provided by the application to fill the scan response.
 * @param[out]  length   - Length of the response buffer measured
 *                        in bytes to hold scan results. \n Size of structure 
 *                        \ref rsi_rsp_scan_t is 54 bytes.
 *                        Length for storing one scan result is
 *                        sizeof( \ref rsi_scan_info_t), which is 46 bytes. \n
 *                        Maximum of 11 scan results will be returned by the
 *                        module, \n in this case, length should be configured
 *                        as 8 + 11*sizeof( \ref rsi_scan_info_t).
 * @param[in]   scan_bitmap    - Scan feature bitmap
 * @return      0              -   Success \n
 *              Non-Zero Value -   Failure
 *
 */

int32_t rsi_wlan_scan_async_with_bitmap_options(int8_t *ssid,
                                                uint8_t chno,
                                                uint32_t bitmap,
                                                void (*scan_response_handler)(uint16_t status,
                                                                              const uint8_t *buffer,
                                                                              const uint16_t length))

{
  rsi_pkt_t *pkt = NULL;
  rsi_req_scan_t *scan;
  int32_t status = RSI_SUCCESS;
  int32_t allow  = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ENTRY, WLAN, LOG_INFO);

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

#ifdef PROCESS_SCAN_RESULTS_AT_HOST
  if (bitmap & RSI_SCAN_RESULTS_TO_HOST) {
    memset(scan_results_array, 0, sizeof(struct wpa_scan_results_arr));
    rsi_wlan_cb_non_rom->scan_results_to_host = 1;
  }
#endif

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE || wlan_cb->state >= RSI_WLAN_STATE_CONNECTED) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  if (scan_response_handler != NULL) {
    allow = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  }
  if (allow == RSI_SUCCESS) {
    switch (wlan_cb->state) {
      case RSI_WLAN_STATE_OPERMODE_DONE: {
        if (wlan_cb->field_valid_bit_map & RSI_SET_MAC_BIT) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
          // If allocation of packet fails
          if (pkt == NULL) {
            // Change WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }

          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_mac_address_t));

          // Memcpy data
          memcpy(&pkt->data, wlan_cb->mac_address, sizeof(rsi_req_mac_address_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send set MAC command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MAC_ADDRESS, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_MAC_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR);
            return status;
          }
        }
#if HE_PARAMS_SUPPORT
        status = rsi_wlan_11ax_config();
        if (status != RSI_SUCCESS) {
          return status;
        }
#endif

        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_band_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send band command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BAND, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BAND_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR);
          return status;
        }
      }
        //  No break
      // Fall through
      case RSI_WLAN_STATE_BAND_DONE: {

#if RSI_TIMEOUT_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_3, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_timeout_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

        // Send set region command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TIMEOUT, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_TIMEOUT_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_3, WLAN, LOG_ERROR);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

        // Send init command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_INIT, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_INIT_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_4, WLAN, LOG_ERROR);
          return status;
        }
#if RSI_SET_REGION_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_5, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_set_region_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

        // Send set region command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_5, WLAN, LOG_ERROR);
          return status;
        }
#endif
#if RSI_SET_REGION_AP_SUPPORT
        if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE) {
          // allocate command buffer  from wlan pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // return packet allocation failure error
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_set_region_ap_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // send set region AP command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION_AP, pkt);

          // wait on wlan semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_AP_RESPONSE_WAIT_TIME);

          // get wlan/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return the status if error in sending command occurs
            return status;
          }
        }
#endif
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_SCAN_DONE:
      case RSI_WLAN_STATE_INIT_DONE:
      case RSI_WLAN_STATE_CONNECTED: {
#if RSI_WLAN_CONFIG_ENABLE
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_6, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

        // Send Configuration request command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CONFIG, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_CONFIG_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_6, WLAN, LOG_ERROR);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_7, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Fill SCAN parameters
        scan = (rsi_req_scan_t *)pkt->data;

        // Memset the PKT
        memset(&pkt->data, 0, sizeof(rsi_req_scan_t));

        // Copy the channel number
        rsi_uint32_to_4bytes(scan->channel, chno);

        // Copy SSID given by the application
        if (ssid != NULL) {
          rsi_strcpy(scan->ssid, ssid);
        }

        scan->scan_feature_bitmap = bitmap;

        if (scan_response_handler != NULL) {
          // Register scan response handler
          rsi_wlan_cb_non_rom->callback_list.wlan_scan_response_handler = scan_response_handler;
        } else {
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        }

        // Send scan command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SCAN, pkt);
      } break;
      case RSI_WLAN_STATE_NONE:
      case RSI_WLAN_STATE_IP_CONFIG_DONE:
      case RSI_WLAN_STATE_IPV6_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON:
      case RSI_WLAN_STATE_AUTO_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_FAILED:
        break;

      default: {
        // Return status if command given in driver is an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
        if (scan_response_handler == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
        }
      } break;
    }
    if (scan_response_handler != NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
    }
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR);
    return allow;
  }
  SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_EXIT, WLAN, LOG_INFO, "status: %4x", status);

  return status;
}

/*==============================================*/
/**
 * @brief      Scan the surrounding Wi-Fi networks. This is a blocking API. Invoke rsi_wlan_scan_async(), which is an asynchronous call.
 *             Wait on WLAN command semaphore to get the status of the scan results. Return RSI_SUCCESS on success and optionally,
 *             a maximum of 11 scan results if rsi_rsp_scan_t parameter is configured.
 *             In case of failure, an appropriate error code is returned to the application.
 * @pre		     \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]  ssid    - SSID size should be less than or equal to 32 bytes. Note: SSID is a null terminated string.
 * @param[in]  chno    - Channel number to perform scan. If 0, then the module will scan all the channels.
 * @param[in]  result  - Scanned Wi-Fi networks information. \n
 *                         This is an output parameter. This output contains a maximum of 11 scan results \n
 *                         The structure \ref rsi_rsp_scan_t contains members scan_count, which specifies the number of scan \n
 *                         results followed by an array of structure type \ref rsi_scan_info_t, where each array element contains \n
 *                         information about each network scanned.
 * @param[in]  length  - Size that should be allocated to buffer that will store scan results. \n
 *             #### Channels supported in 2.4 GHz Band ####
 *         		 Channel numbers       	 |	Ch no
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 	           All Channels	       	   |       0	
 * 	         	 1			                 |       1	
 * 		         2			                 |       2	
 * 		         3	                     |       3	
 * 		         4			                 |       4	
 * 		         5			                 |       5	
 * 		         6			                 |       6	
 * 	     	     7			                 |       7	
 * 		         8			                 |       8	
 * 		         9			                 |       9	
 * 		         10                      |       10	
 * 		         11			                 |       11	
 * 		         12                      |       12	
 *		         13			                 |       13	
 * 		         14			                 |       14	
 *             #### Channels supported in 5 GHz Band ####
 * 		         Channel numbers         |	Ch no
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         All Channels		         |       0	
 * 		         36	        	           |       36
 * 		         40	        	           |       40
 * 		         44	        	           |       44
 * 		         48	        	           |       48
 *		         149	                   |       149
 * 		         153		                 |       153
 * 		         157		                 |       157
 * 		         161        		         |       161
 *		         165	         	         |       165
 *             #### DFS Channels supported in 5 GHz Band ####
 * 		         Channel numbers         |	Ch no
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         52(DFS)	        	     |       52
 * 		         56(DFS)	        	     |       56
 * 		         60(DFS)	        	     |       60
 * 		         64(DFS)	        	     |       64
 * 		         100(DFS)	               |       100
 * 		         104(DFS)	               |       104
 * 		         108(DFS)	               |       108
 * 		         112(DFS)	               |       112
 * 		         116(DFS)	               |       116
 * 		         120(DFS)	               |       120
 * 		         124(DFS)	               |       124
 * 		         128(DFS)	               |       128
 * 		         132(DFS)                |       132
 * 		         136(DFS)	          	   |       136
 * 		         140(DFS)         		   |       140
 * 		         144(DFS)         		   |       144
 *             #### Channels supported in 4.9 GHz Band ####
 * 		         Channel numbers         |	Ch no
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         All Channels		         |       0	
 * 		         184	        	         |       184
 * 		         188	        	         |       188
 * 		         192	        	         |       192
 * 		         196	        	         |       196
 * 		         8	                     |       8
 * 		         12	                     |       12
 * 		         16	                     |       16
 * @note       To set various timeouts, user should change the following macros in rsi_wlan_config.h \n
 *			       #define RSI_TIMEOUT_SUPPORT RSI_ENABLE \n
 *			       #define RSI_TIMEOUT_BIT_MAP 4 \n
 *			       #define RSI_TIMEOUT_VALUE 300
 *        		timeout_bitmap        	|       Description	
 * 		        :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		        timeout_bitmap[0]     	|	      Set timeout for association and authentication request.timeout_value : timeout value in ms(default 300ms).
 * 		        timeout_bitmap[1]     	|       Sets each channel active scan time in ms (default 100ms)
 * 		        timeout_bitmap[2] 	    |       Used for WLAN keep alive timeout(default value is 30s)
 * 		        timeout_bitmap[31-3]    |       Reserved
 * 	          ####	Scan structure ####
 * 		        Structure Fields        |	Description	
 * 		        :-----------------------|:--------------------------------------------------------------------------------------------------
 * 	 	         rf_channel	            |      Access point channel number
 * 	 	         security_mode	        |      Security mode0 : Open
 *	           ^        	            |    	1 . WPA
 *	           ^        	            |    	2 . WPA2
 *	           ^        	            |    	3 . WEP
 *	           ^        	            |    	4 . WPA Enterprise
 *	           ^        	            |    	5 . WPA2 Enterprise
 *	           ^                        |       7 . WPA3
 * 	 	         rssi_val	              |       RSSI value of the Access Point
 * 	 	         network_type	          |       rsi_wlan_setThis is the type of the network
 * 	 	         ^            	        |       1 . Infrastructure mode	
 * 	 	         ssid		                |       SSID of the access point
 * 	 	         bssid		              |       MAC address of the access point
 * @return     **Success**  - RSI_SUCCESS\n
 *             **Failure**  - Non-Zero values\n
 *
 *			 `If return value is less than 0` \n
 *
 *            		 **RSI_ERROR_INVALID_PARAM**                 - Invalid parameters \n
 *
 *	         	 **RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE**  - Command given in wrong state \n
 *
 *	          	 **RSI_ERROR_PKT_ALLOCATION_FAILURE**        - Buffer not available to serve the command \n
 *
 *             		 `Other expected error codes are :` \n
 *
 * 	         	 **0x0002, 0x0003, 0x0005, 0x000A, 0x0014, 0x0015, 0x001A, 0x0021,0x0024,0x0025,0x0026,0x002C,0x003c**
 * @note       Refer to Error Codes section for above error codes description \ref error-codes.
 *
 *
 */
int32_t rsi_wlan_scan(int8_t *ssid, uint8_t chno, rsi_rsp_scan_t *result, uint32_t length)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_SCAN_ENTRY, WLAN, LOG_INFO);

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE || wlan_cb->state > RSI_WLAN_STATE_IP_CONFIG_DONE) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)result;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    //Ccall async scan to perform actual scan
    status = rsi_wlan_scan_async(ssid, chno, NULL);

    if (status != RSI_SUCCESS) {

      // Return status if error in sending command occurs
      SL_PRINTF(SL_WLAN_SCAN_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
      return status;
    }

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SCAN_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SCAN_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_WLAN_SCAN_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @note	     If status is Success , the argument buffer passed to scan_response_handler \n 
 * 			       holds scan results in \ref rsi_rsp_scan_t structure format.
 * @brief      Scan the surrounding Wi-Fi networks. A scan response handler is registered with it to get the response for scan.This is a non-blocking API.
 * @pre 	     \ref rsi_wireless_init() API needs to be called before this API. 
 * @param[in]  ssid		        			 - SSID to scan. If this input parameter is present, module will scan for that particular SSID only. SSID size should be less than or equal to 32 bytes
 * @param[in]  chno 				         - Channel number to perform scan, if 0, then module will scan in all channels.
 * @param[in]  Scan_response_handler - Callback tht is called when the response for scan is received from the module. \n
 *				       The parameters involved are status, buffer, & length. \n
 *@param[out]	 Status                - Response status.
 *	                                   RSI_SUCCESS - Scan response is stated as Success  \n
 *	                                   Negative -Failure \n
 *@param[out]	 Buffer                - Response buffer \n
 *@param[out]	 Length                - Length of the response buffer measured in bytes to hold scan results. \n Size of structure \ref rsi_rsp_scan_t is 54 bytes. \n
 *					                           Length for storing one scan result is sizeof(rsi_scan_info_t) which is 44 bytes. \n
 *						                         Maximum of 11 scan results will be returned by the module, \n in this case, length must be configured as 8 + 11*sizeof(rsi_scan_info_t).
 * @note       SSID is a null terminated string.
*             #### Channels supported in 2.4 GHz Band ####
 *         		 Channel numbers       	 |	Ch no
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 	           All Channels	       	   |       0	
 * 	         	 1			                 |       1	
 * 		         2			                 |       2	
 * 		         3	                     |       3	
 * 		         4			                 |       4	
 * 		         5			                 |       5	
 * 		         6			                 |       6	
 * 	     	     7			                 |       7	
 * 		         8			                 |       8	
 * 		         9			                 |       9	
 * 		         10                      |       10	
 * 		         11			                 |       11	
 * 		         12                      |       12	
 *		         13			                 |       13	
 * 		         14			                 |       14	
 *             #### Channels supported in 5 GHz Band ####
 * 		         Channel numbers         |	Ch no
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         All Channels		         |       0	
 * 		         36	        	           |       36
 * 		         40	        	           |       40
 * 		         44	        	           |       44
 * 		         48	        	           |       48
 * 		         100	                   |       100
 * 		         104	                   |       104
 * 		         108	                   |       108
 * 		         116	                   |       116
 * 		         132                     |       132
 * 		         136	          	       |       136
 * 		         140         		         |       140
 *		         149	                   |       149
 * 		         153		                 |       153
 * 		         157		                 |       157
 * 		         161        		         |       161
 *		         165	         	         |       165
 * @note       To set various timeouts, user should change the following macros in rsi_wlan_config.h \n
 *			       #define RSI_TIMEOUT_SUPPORT RSI_ENABLE \n
 *			       #define RSI_TIMEOUT_BIT_MAP 4 \n
 *			       #define RSI_TIMEOUT_VALUE 300
 *
 *        	   timeout_bitmap          |       Description	
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         timeout_bitmap[0]     	 |	     Set timeout for association and authentication request.timeout_value : timeout value in ms(default 300ms).
 * 		         timeout_bitmap[1]     	 |       Sets each channel active scan time in ms (default 100ms)
 * 		         timeout_bitmap[2] 	     |       Used for WLAN keep alive timeout(default value is 30s)
 * 		         timeout_bitmap[31-3]    |       Reserved
 * 	           #### Scan Response structure format ####
 * 		         Structure Fields        |	Description	
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 	 	         scan_count	             |      Number of access points scanned.
 * 	 	         ^                       |      scan_count[0] Contains the scan count.
 * 	 	         ^                       |      scan_count[3-1] are reserved.
 * 	 	         scan_info               |      Information about scanned Access points in rsi_scan_info_t structure.
 * 	           ####	Scan structure ####
 * 		         Structure Fields        |	Description	
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 	 	         rf_channel	             |      Access point channel number
 * 	 	         security_mode	         |      Security mode0 : Open
 *	           ^        	             |    	1 . WPA
 *	           ^        	             |    	2 . WPA2
 *	           ^        	             |    	3 . WEP
 *	           ^        	             |    	4 . WPA Enterprise
 *	           ^        	             |    	5 . WPA2 Enterprise
 *	           ^        	             |    	7 . WPA3
 * 	 	         rssi_val	               |       RSSI value of the Access Point
 * 	 	         network_type	           |       Type of network
 * 	 	         ^            	         |       1 . Infrastructure mode	
 * 	 	         ssid		                 |       SSID of the access point
 * 	 	         bssid		               |       MAC address of the access point
 * @return     0              -  Success \n
 *             Non-Zero	Value - Failure \n
 *	           If return value is less than 0 \n
 *             -2             - Invalid parameters \n
 *	           -3             - Command given in wrong state \n
 *	           -4             - Buffer not available to serve the command \n
 *             If return value is greater than 0 \n
 * 	           0x0002, 0x0003, 0x0005, 0x000A, 0x0014, 0x0015, 0x001A, 0x0021,0x0024,0x0025,0x0026,0x002C,0x003c
 * @note       Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_wlan_scan_async(int8_t *ssid,
                            uint8_t chno,
                            void (*scan_response_handler)(uint16_t status,
                                                          const uint8_t *buffer,
                                                          const uint16_t length))

{
  rsi_pkt_t *pkt;
  rsi_req_scan_t *scan;
  int32_t status = RSI_SUCCESS;
  int32_t allow  = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_SCAN_ASYNC_ENTRY, WLAN, LOG_INFO);

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE || wlan_cb->state > RSI_WLAN_STATE_IP_CONFIG_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SCAN_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  if (scan_response_handler != NULL) {
    allow = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  }
  if (allow == RSI_SUCCESS) {

    switch (wlan_cb->state) {
      case RSI_WLAN_STATE_OPERMODE_DONE: {
        if (wlan_cb->field_valid_bit_map & RSI_SET_MAC_BIT) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }

          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_mac_address_t));

          // Memcpy data
          memcpy(&pkt->data, wlan_cb->mac_address, sizeof(rsi_req_mac_address_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send set MAC command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MAC_ADDRESS, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_MAC_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_SCAN_ASYNC_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
            return status;
          }
        }
#if HE_PARAMS_SUPPORT
        status = rsi_wlan_11ax_config();
        if (status != RSI_SUCCESS) {
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_band_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send band command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BAND, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BAND_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_BAND_DONE: {

#if RSI_TIMEOUT_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_timeout_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send set region command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TIMEOUT, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_TIMEOUT_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_ERROR_IN_SENDING_COMMAND_3, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_3, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send init command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_INIT, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_INIT_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          return status;
        }
#if RSI_SET_REGION_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_set_region_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

        // Send set region command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_ERROR_IN_SENDING_COMMAND_4, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#endif
#if RSI_SET_REGION_AP_SUPPORT
        if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE) {
          // allocate command buffer  from wlan pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // return packet allocation failure error
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_set_region_ap_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // send set region AP command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION_AP, pkt);

          // wait on wlan semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_AP_RESPONSE_WAIT_TIME);

          // get wlan/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return the status if error in sending command occurs
            return status;
          }
        }
#endif
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_SCAN_DONE:
      case RSI_WLAN_STATE_INIT_DONE:
      case RSI_WLAN_STATE_CONNECTED:
      case RSI_WLAN_STATE_IP_CONFIG_DONE: {
#if RSI_WLAN_CONFIG_ENABLE
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_5, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send Configuration request command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CONFIG, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_CONFIG_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_ERROR_IN_SENDING_COMMAND_5, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_6, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Fill SCAN parameters
        scan = (rsi_req_scan_t *)pkt->data;

        // Memset the PKT
        memset(&pkt->data, 0, sizeof(rsi_req_scan_t));

        // Copy the channel number
        rsi_uint32_to_4bytes(scan->channel, chno);

        // Copy SSID given by the application
        if (ssid != NULL) {
          rsi_strcpy(scan->ssid, ssid);
        }

        if (scan_response_handler != NULL) {
          // Register scan response handler
          rsi_wlan_cb_non_rom->callback_list.wlan_scan_response_handler = scan_response_handler;
        } else {
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        }
        // Send scan command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SCAN, pkt);
      } break;
      case RSI_WLAN_STATE_NONE:
      case RSI_WLAN_STATE_IPV6_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON:
      case RSI_WLAN_STATE_AUTO_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_FAILED:
        break;
      default: {
        // Return status if command given in driver is an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
        if (scan_response_handler == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
        }
      } break;
    }
    if (scan_response_handler != NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
    }
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SCAN_ASYNC_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return allow;
  }
  SL_PRINTF(SL_WLAN_SCAN_ASYNC_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Connect to the specified Wi-Fi network. This is a blocking API.
 *              Call asynch connect to trigger connect call to AP and return RSI_SUCCESS or error in case of failure.
 * @pre		    \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]   ssid		- SSID of an access point to connect. SSID should be less than or equal to 32 bytes.
 * @param[in]   sec_type 	- Security type of the access point to connect. \n
 * 				0: RSI_OPEN, \n
 * 				1: RSI_WPA, \n
 * 				2: RSI_WPA2, \n
 * 				3: RSI_WEP, \n
 * 				4: RSI_WPA_EAP, \n
 * 				5: RSI_WPA2_EAP, \n
 * 				6: RSI_WPA_WPA2_MIXED, \n
 * 				7: RSI_WPA_PMK, \n
 * 				8: RSI_WPA2_PMK, \n
 * 				9: RSI_WPS_PIN, \n
 * 				10: RSI_USE_GENERATED_WPSPIN, \n
 * 				11: RSI_WPS_PUSH_BUTTON, \n
 * 				12: RSI_WPA_WPA2_MIXED_PMK, \n
 * 				13: RSI_WPA3
 * @param[in]   secret_key	- Pointer to a buffer that contains security information based on sec_type. \n
 * 		Security type(sec_type)	|	Secret key structure format (secret_key)
 * 		:-----------------------|:--------------------------------------------------------------------------------------------------
 * 		RSI_OPEN		|	No secret key in open security mode.
 * 		RSI_WPA			|	PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 * 		RSI_WPA2		|	PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 * 		RSI_WEP			|	WEP keys should be in the following format:
 * 		^			|	typedef struct rsi_wep_keys_s
 * 		^			|	{ 
 * 		^			|	uint8_t index[2];
 * 		^			|	uint8_t key[4][32];
 * 		^			|	} rsi_wep_keys_t; 
 * 		^			|	index: WEP key index to use for TX packet encryption.
 * 		^			|	key: 4 WEP keys, last three WEP keys are optional. If only first WEP key is valid then index should be 0.
 * 		RSI_WPA_EAP		|	Enterprise credentials should be in the following format:
 * 		^			|	typedef struct rsi_eap_credentials_s
 * 		^			|	{ 
 * 		^			|	uint8_t username[64];
 * 		^			|	uint8_t password[128];
 * 		^			|	} rsi_eap_credentials_t;
 *		^			|	username: username to be used in enterprise
 * 		^			|	password: password for the given username
 * 		RSI_WPA2_EAP		|	Enterprise credentials should be in the following format:
 * 		^			|	typedef struct rsi_eap_credentials_s
 * 		^			|	{ 
 * 		^			|	uint8_t username[64];
 * 		^			|	uint8_t password[128];
 * 		^			|	} rsi_eap_credentials_t;
 * 		^			|	username: username to be used in enterprise
 * 		^			|	password: password for a given username.
 * 		RSI_WPA_WPA2_MIXED	|	PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes
 * 		RSI_WPA_PMK		|	PMK string, should be 32 bytes in length
 * 		RSI_WPA2_PMK		|	PMK string, should be 32 bytes in length
 * 		RSI_WPS_PIN		|	8-byte WPS PIN
 * 		RSI_USE_GENERATED_WPSPIN|	NULL string indicate to use PIN generated using rsi_wps_generate_pin API
 * 		RSI_WPS_PUSH_BUTTON	|	NULL string indicate to generate push button event
 * 		RSI_WPA3            |   PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 * @note       To set various timeouts, user should change the following macros in rsi_wlan_config.h \n
 *			   #define RSI_TIMEOUT_SUPPORT RSI_ENABLE \n
 *			   #define RSI_TIMEOUT_BIT_MAP 4 \n
 *			   #define RSI_TIMEOUT_VALUE 300 \n
 *
 *			   timeout_bitmap[0] -	Set timeout for association and authentication request. timeout_value : timeout value in ms(default 300ms). \n
 *			   timeout_bitmap[1] -	Sets each channel active scan time in ms (default 100ms) \n
 *			   timeout_bitmap[2] -	Used for WLAN keep alive timeout(default value is 30s) \n
 *			   timeout_bitmap[31-3] -	reserved \n 
 * @note	To set Rejoin params, user should configure the following macros in rsi_wlan_config.h \n 
 *		            RSI_REJOIN_PARAMS_SUPPORT      : Enable to send rejoin parameters command during Wi-Fi client connection \n
 *		            RSI_REJOIN_MAX_RETRY           : Number of maximum rejoin retries by module\n
 *		            note                           : If Max retries is 0 , retries infinity times \n
 *		            RSI_REJOIN_SCAN_INTERVAL       : Periodicity of rejoin attempt \n
 *		            RSI_REJOIN_BEACON_MISSED_COUNT : Number of consecutive beacon misses after which modules goes to unconnected state \n
 *		            RSI_REJOIN_FIRST_TIME_RETRY    : ENABLE  - Try to rejoin in the first attempt after join failure. \n
 *		                                             DISABLE - Try to rejoin based on maximum rejoin retries configured. \n
 *		When RSI_REJOIN_PARAMS_SUPPORT is enabled in the rsi_wlan_config.h, this rejoin frame will be sent to the firmware after scan done in the SAPI.
 * @note        This API internally handles following commands based on wlan_cb state and sends the next command and finally sends the join command. \n
 *                 •	Set mac address \n
 *                 •	Band \n
 *                 •	Timeout \n
 *                 •	Init \n
 *                 •	Set region \n
 *                 •	WMM parameters \n
 *                 •	Scan \n
 *                 •	EAP config \n
 *                 •	WMM PS parameters \n
 *                 •	WPS method \n
 *                 •	Set WEP keys \n
 *                 •	Host PSK \n
 *                 •	Rejoin params \n
 *                 •	Join \n
 *              For example, \n
 *              After calling rsi_wireless_init(), wlan_cb state is updated to opermode done state. So when we call this API, it will execute band, init, scan and join commands. \n
 *              After calling rsi_wlan_disconnect() or else after rejoin failure, wlan_cb state is updated to band done state. So when we call this API, it will execute init, scan and join commands. \n
 *              After calling rsi_wlan_scan()/ rsi_wlan_scan_with_bitmap_options() API, wlan_cb state is updated to scan done state. So when we call this API, it will execute join command directly. \n
 * @return 	**Success**     - RSI_SUCCESS \n
 *              **Failure**     - Non-Zero Value \n
 *
 *              		 `if return value is less than zero :\n
 *
 *				  **RSI_ERROR_INVALID_PARAM**                - Invalid parameters \n
 *
 *				  **RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE** - Command given in wrong state \n
 *
 *				  **RSI_ERROR_PKT_ALLOCATION_FAILURE**       - Buffer not available to serve the command \n
 *
 *				`Other expected error code are :  \n
 *
 *				  **0x0002,0x0003,0x0005,0x0008,0x0009,0x000A,0x000E,0x0014,** \n
 *
 *				  **0x0015,0x0016,0x0019,0x001A,0x001E,0x0020,0x0021,0x0024,** \n
 *
 *				  **0x0025,0x0026,0x0028,0x0039,0x003C,0x0044,0x0045,0x0046,** \n
 *
 *				  **0x0047,0x0048,0x0049,0xFFF8**
 * @note		Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_wlan_connect(int8_t *ssid, rsi_security_mode_t sec_type, void *secret_key)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_CONNECT_ENTRY, WLAN, LOG_INFO);
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Call async connect to call actual connect
    status = rsi_wlan_connect_async(ssid, sec_type, secret_key, NULL);

    if (status != RSI_SUCCESS) {
      // Return status if error in sending command occurs
      SL_PRINTF(SL_WLAN_CONNECT_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
      return status;
    }

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_CONNECT_RESPONSE_WAIT_TIME);

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    if (status != RSI_SUCCESS) {
      // Return status if error in sending command occurs
      SL_PRINTF(SL_WLAN_CONNECT_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
      return status;
    }

  } else {
    // Return WLAN command error
    return status;
  }
  // Execute post connect commands
  status = rsi_wlan_execute_post_connect_cmds();

  status = rsi_wlan_get_status();
  SL_PRINTF(SL_WLAN_CONNECT_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief       Connect to the specified Wi-Fi network.A join response handler is registered to get the response for join.. This is a non-blocking API.
 * @pre        \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]   ssid - SSID of an access point to connect. SSID should be less than or equal to 32 bytes.
 * @param[in]   sec_type			  - Security type of the access point to connect.
 *                                0: RSI_OPEN, \n
 *                                1: RSI_WPA, \n
 *                                2: RSI_WPA2, \n
 *                                3: RSI_WEP, \n
 *                                4: RSI_WPA_EAP, \n
 *                                5: RSI_WPA2_EAP, \n
 *                                6: RSI_WPA_WPA2_MIXED, \n
 *                                7: RSI_WPA_PMK, \n
 *                                8: RSI_WPA2_PMK, \n
 *                                9: RSI_WPS_PIN, \n
 *                               10: RSI_USE_GENERATED_WPSPIN, \n
 *                               11: RSI_WPS_PUSH_BUTTON, \n
 *                               12: RSI_WPA_WPA2_MIXED_PMK, \n
 *                               13: RSI_WPA3
 * @param[in]   secret_key			  - Pointer to a buffer that contains security information based on sec_type.
 *              Security type(sec_type) |       Secret key structure format (secret_key)
 *              :-----------------------|:-----------------------------------------------------------------------------------------------------------------
 *              RSI_OPEN                |       No secret key in open security mode.
 *              RSI_WPA                 |       PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 *              RSI_WPA2                |       PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 *              RSI_WEP                 |       WEP keys should be in the following format:
 *              ^			|	typedef struct rsi_wep_keys_s 
 *              ^			|	{
 *              ^			|	uint8_t index[2];
 *              ^			|	uint8_t key[4][32];
 *              ^			|	} rsi_wep_keys_t;
 *              ^			|	index: WEP key index to use for TX packet encryption.
 *              ^			|	key: 4 WEP keys, last three WEP keys are optional. If only first WEP key is valid then index should be 0.
 *              RSI_WPA_EAP             |       Enterprise credentials should be in the following format:
 *              ^			|	typedef struct rsi_eap_credentials_s
 *              ^			|	{
 *              ^			|	uint8_t username[64];
 *              ^			|	uint8_t password[128];
 *              ^			|	} rsi_eap_credentials_t;
 *              ^			|	username: username to be used in enterprise
 *              ^			|	password: password for the given username
 *              RSI_WPA2_EAP            |       Enterprise credentials should be in the following format:
 *              ^			|	typedef struct rsi_eap_credentials_s
 *              ^			|	{
 *              ^			|	uint8_t username[64];
 *              ^			|	uint8_t password[128];
 *              ^			|	} rsi_eap_credentials_t;
 *              ^			|	username: username to be used in enterprise
 *              ^			|	password: password for a given username.
 *              RSI_WPA_WPA2_MIXED      |       PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes
 *              RSI_WPA_PMK             |       PMK string, should be 32 bytes in length
 *              RSI_WPA2_PMK            |       PMK string, should be 32 bytes in length
 *              RSI_WPS_PIN             |       8-byte WPS PIN
 *              RSI_USE_GENERATED_WPSPIN|       NULL string indicate to use PIN generated using rsi_wps_generate_pin API
 *              RSI_WPS_PUSH_BUTTON     |       NULL string indicate to generate push button event
 *              RSI_WPA3                |       PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 * @param[in]   join_response_handler - Called when the response for join has been received from the module \n
 *                                        Parameters involved are status, buffer, & length \n
 * @param[out]  Status                - Response status. If status is zero, then the join response is stated as Success  \n
 * @param[out]  Buffer                - Response buffer. On successful execution of the command.  \n GO_Status (1 byte, hex): 0x47 (ASCII "G") If the module becomes a Group Owner (GO) after the GO
 *                                        negotiation stage or becomes an Access Point. \n 0x43 (ASCII "C") If the module does not become a GO after the GO negotiation stage or becomes a client (or station).
 * @param[out] length 		      -	Length of the response buffer.
 * @note         The module gets a default IP of 192.168.100.76 if it becomes a Group Owner or Access Point in case of IPv4. \n and gets a default IP of 2001:db8:0:1:0:0:0:120 in case of IPv6.
 * @note	 To set Rejoin params, user should configure the following macros in rsi_wlan_config.h \n 
 *		            RSI_REJOIN_PARAMS_SUPPORT      : Enable to send rejoin parameters command during Wi-Fi client connection \n
 *		            RSI_REJOIN_MAX_RETRY           : Number of maximum rejoin retries by module\n
 *		            note                           : If Max retries is 0 , retries infinity times \n
 *		            RSI_REJOIN_SCAN_INTERVAL       : Periodicity of rejoin attempt \n
 *		            RSI_REJOIN_BEACON_MISSED_COUNT : Number of consecutive beacon misses after which modules goes to unconnected state \n
 *		            RSI_REJOIN_FIRST_TIME_RETRY    : ENABLE  - Try to rejoin in the first attempt after join failure. \n
 *		                                             DISABLE - Try to rejoin based on maximum rejoin retries configured. \n
 *		When RSI_REJOIN_PARAMS_SUPPORT is enabled in the rsi_wlan_config.h, this rejoin frame will be sent to the firmware after scan done in the SAPI.
 * @note        This API internally handles following commands based on wlan_cb state and sends the next command and finally sends the join command. \n
 *                 •	Set mac address \n
 *                 •	Band \n
 *                 •	Timeout \n
 *                 •	Init \n
 *                 •	Set region \n
 *                 •	WMM parameters \n
 *                 •	Scan \n
 *                 •	EAP config \n
 *                 •	WMM PS parameters \n
 *                 •	WPS method \n
 *                 •	Set WEP keys \n
 *                 •	Host PSK \n
 *                 •	Rejoin params \n
 *                 •	Join \n
 *              For example, \n
 *              After calling rsi_wireless_init(), wlan_cb state is updated to opermode done state. So when we call this API, it will execute band, init, scan and join commands. \n
 *              After calling rsi_wlan_disconnect() or else after rejoin failure, wlan_cb state is updated to band done state. So when we call this API, it will execute init, scan and join commands. \n
 *              After calling rsi_wlan_scan()/ rsi_wlan_scan_with_bitmap_options() API, wlan_cb state is updated to scan done state. So when we call this API, it will execute join command directly. \n
 * @return      0 - Success \n 
 *              Non-Zero - Failure \n
 *              -2 - Invalid parameters \n
 *                -3 - Command given in wrong state \n
 *                -4 - Buffer not availableto serve the command
 * @note        Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_wlan_connect_async(int8_t *ssid,
                               rsi_security_mode_t sec_type,
                               void *secret_key,
                               void (*join_response_handler)(uint16_t status,
                                                             const uint8_t *buffer,
                                                             const uint16_t length))
{
  rsi_eap_credentials_t *credentials;
  rsi_req_wps_method_t *wps_method;
  rsi_req_psk_t *psk;
  rsi_wep_keys_t *wep_keys;
  rsi_pkt_t *pkt;
  rsi_req_scan_t *scan;
  rsi_req_join_t *join;
  rsi_req_eap_config_t *eap_config;
  int32_t status = RSI_SUCCESS;
  int32_t allow  = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
  if (join_response_handler != NULL) {
    allow = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  }

  if (allow == RSI_SUCCESS) {
    // Check whether PSK is provided or not for security modes
    if ((sec_type != 0) && (secret_key == NULL) && (sec_type != RSI_WPS_PUSH_BUTTON)
        && (sec_type != RSI_USE_GENERATED_WPSPIN)) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Throw error in case of invalid parameters
      SL_PRINTF(SL_WLAN_CONNECT_ASYNC_INVALID_PARAM_1, WLAN, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Check whether module is in valid state range or not
    if (wlan_cb->state >= RSI_WLAN_STATE_CONNECTED) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Command given in wrong state
      SL_PRINTF(SL_WLAN_CONNECT_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_1, WLAN, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }

    if ((wlan_cb->opermode == RSI_WLAN_ENTERPRISE_CLIENT_MODE)
        && !((sec_type == RSI_WPA_EAP) || (sec_type == RSI_WPA2_EAP))) {
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Throw error in case of invalid parameters
      SL_PRINTF(SL_WLAN_CONNECT_ASYNC_INVALID_PARAM_2, WLAN, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }

    switch (wlan_cb->state) {
      case RSI_WLAN_STATE_OPERMODE_DONE: {
        if (wlan_cb->field_valid_bit_map & RSI_SET_MAC_BIT) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_mac_address_t));
          // Memcpy data
          memcpy(&pkt->data, wlan_cb->mac_address, sizeof(rsi_req_mac_address_t));
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send set MAC command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MAC_ADDRESS, pkt);
          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_MAC_RESPONSE_WAIT_TIME);
          // Get WLAN/network command response status
          status = rsi_wlan_get_status();
          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR);
            return status;
          }
        }
#if HE_PARAMS_SUPPORT
        status = rsi_wlan_11ax_config();
        if (status != RSI_SUCCESS) {
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_HE_PARAMS_SUPPORT, WLAN, LOG_INFO);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send band command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BAND, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BAND_RESPONSE_WAIT_TIME);
        // Get WLAN/network command response status
        status = rsi_wlan_get_status();
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR);
          return status;
        }
      }
        //  No break
      // Fall through
      case RSI_WLAN_STATE_BAND_DONE: {
#if RSI_TIMEOUT_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_3, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_timeout_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

        // Send set region command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TIMEOUT, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_TIMEOUT_RESPONSE_WAIT_TIME);
        // Get WLAN/network command response status
        status = rsi_wlan_get_status();
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_3, WLAN, LOG_ERROR);
          return status;
        }
#endif
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send init command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_INIT, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_INIT_RESPONSE_WAIT_TIME);
        // Get WLAN/network command response status
        status = rsi_wlan_get_status();
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_WLAN_CMD_STATE_CHANGED_TO_ALLOW, WLAN, LOG_INFO);
          return status;
        }
#if RSI_SET_REGION_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_5, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_set_region_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send set region command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_RESPONSE_WAIT_TIME);
        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_4, WLAN, LOG_ERROR);
          return status;
        }
#endif
#if RSI_SET_REGION_AP_SUPPORT
        if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE) {
          // allocate command buffer  from wlan pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // return packet allocation failure error
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_set_region_ap_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // send set region AP command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION_AP, pkt);

          // wait on wlan semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_AP_RESPONSE_WAIT_TIME);

          // get wlan/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return the status if error in sending command occurs
            return status;
          }
        }
#endif
      }
        //  No break
      // Fall through
      case RSI_WLAN_STATE_INIT_DONE: {
#if RSI_WMM_PS_ENABLE
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_6, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset the packet data
        memset(&pkt->data, 0, sizeof(rsi_wmm_ps_parms_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send WMM parameters
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WMM_PS, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WMM_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_5, WLAN, LOG_ERROR);
          return status;
        }

#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_7, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Give buffer as NULL, no need to give scan results to host
        wlan_cb->app_buffer = NULL;

        // Length of the buffer is zero
        wlan_cb->app_buffer_length = 0;

        // Send scan command
        scan = (rsi_req_scan_t *)pkt->data;

        // Memset the packet data
        memset(&pkt->data, 0, sizeof(rsi_req_scan_t));

        // Copy SSID given by the application
        if (ssid != NULL) {
          rsi_strcpy(scan->ssid, ssid);
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send scan command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SCAN, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SCAN_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_6, WLAN, LOG_ERROR);
          return status;
        }
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_SCAN_DONE: {

        // Send EAP command for EAP security
        if ((wlan_cb->opermode == RSI_WLAN_ENTERPRISE_CLIENT_MODE)) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_8, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }

          credentials = (rsi_eap_credentials_t *)secret_key;

          // Memset the packet data
          memset(&pkt->data, 0, sizeof(rsi_req_eap_config_t));

          eap_config = (rsi_req_eap_config_t *)pkt->data;

          // Copy username
          memcpy(eap_config->user_identity, credentials->username, RSI_EAP_USER_NAME_LENGTH);

          // Copy password
          memcpy(eap_config->password, credentials->password, RSI_EAP_PASSWORD_LENGTH);

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send EAP config command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EAP_CONFIG, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_EAP_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_7, WLAN, LOG_ERROR);
            return status;
          }
        }

#if RSI_WMM_PS_ENABLE
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_9, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset the packet data
        memset(&pkt->data, 0, sizeof(rsi_wmm_ps_parms_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send WMM parameters
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WMM_PS, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WMM_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_8, WLAN, LOG_ERROR);
          return status;
        }

#endif
        if ((sec_type == RSI_WPS_PIN) && (wlan_cb->opermode != RSI_WLAN_ENTERPRISE_CLIENT_MODE)) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_10, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }

          wps_method = (rsi_req_wps_method_t *)pkt->data;

          // Memset buffer
          memset(&pkt->data, 0, sizeof(rsi_req_wps_method_t));

          // Set configuration to validate WPS pin
          rsi_uint16_to_2bytes(wps_method->wps_method, 1);

          // Set configuration to validate WPS pin
          rsi_uint16_to_2bytes(wps_method->generate_pin, 0);

          // Copy WPS pin
          memcpy(wps_method->wps_pin, secret_key, RSI_WPS_PIN_LEN);

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send WPS method command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WPS_METHOD, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WPS_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_9, WLAN, LOG_ERROR);
            return status;
          }
        } else if (sec_type == RSI_WEP) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_11, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }

          // Memset buffer
          memset(&pkt->data, 0, sizeof(rsi_wep_keys_t));

          wep_keys = (rsi_wep_keys_t *)pkt->data;

          // Copy WEP keys
          memcpy(wep_keys, secret_key, sizeof(rsi_wep_keys_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send PSK command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_WEP_KEYS, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WEP_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_10, WLAN, LOG_ERROR);
            return status;
          }

        }
        // Send PSK command for PSK security
        else if (((sec_type == RSI_WPA) || (sec_type == RSI_WPA2) || (sec_type == RSI_WPA3)
                  || (sec_type == RSI_WPA3_TRANSITION) || (sec_type == RSI_WPA_WPA2_MIXED)
                  || (sec_type == RSI_WPA_WPA2_MIXED_PMK) || (sec_type == RSI_WPA_PMK) || (sec_type == RSI_WPA2_PMK))
                 && (wlan_cb->opermode != RSI_WLAN_ENTERPRISE_CLIENT_MODE)) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_12, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          psk = (rsi_req_psk_t *)pkt->data;

          // Memset buffer
          memset(&pkt->data, 0, sizeof(rsi_req_psk_t));

          if ((sec_type == RSI_WPA2_PMK) || (sec_type == RSI_WPA_PMK) || (sec_type == RSI_WPA_WPA2_MIXED_PMK)) {
            if (sec_type == RSI_WPA_WPA2_MIXED_PMK) {
              sec_type = RSI_WPA_WPA2_MIXED;
            }
            // Set type as PMK
            psk->type = 2;

            // Copy PMK
            memcpy(psk->psk_or_pmk, secret_key, RSI_PMK_LEN);

          } else {
            // Set type as PSK
            psk->type = 1;

            // Copy PSK
            memcpy(psk->psk_or_pmk, secret_key, RSI_PSK_LEN);
          }

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send PSK command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HOST_PSK, pkt);
          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WPS_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_11, WLAN, LOG_ERROR);
            return status;
          }
        }

#if RSI_REJOIN_PARAMS_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_13, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_rejoin_params_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send rejoin params command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_REJOIN_PARAMS, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REJOIN_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_12, WLAN, LOG_ERROR);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_14, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset the PKT
        memset(&pkt->data, 0, sizeof(rsi_req_join_t));

        join = (rsi_req_join_t *)pkt->data;

        // Fill security type
        if (sec_type == RSI_WPA2_PMK) {
          join->security_type = RSI_WPA2;
        } else if (sec_type == RSI_WPA_PMK) {
          join->security_type = RSI_WPA;
        } else if ((sec_type == RSI_WPS_PIN) || (sec_type == RSI_USE_GENERATED_WPSPIN)
                   || (sec_type == RSI_WPS_PUSH_BUTTON)) {
          join->security_type = RSI_OPEN;
        } else if (sec_type == RSI_WPA3) {
          join->security_type = SME_WPA3;
        } else if (sec_type == RSI_WPA3_TRANSITION) {
          join->security_type = SME_WPA3_TRANSITION;
        } else {
          join->security_type = sec_type;
        }

        if (ssid != NULL) {
          // Copy SSID and SSID len
          join->ssid_len = rsi_strlen(ssid);

          // Copy Join SSID
          rsi_strcpy(join->ssid, ssid);
        }

        if (join_response_handler != NULL) {
          // Register scan response handler
          rsi_wlan_cb_non_rom->callback_list.wlan_join_response_handler = join_response_handler;
        } else {
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        }
        // Send join command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);
      } break;
      case RSI_WLAN_STATE_NONE:
      case RSI_WLAN_STATE_CONNECTED:
      case RSI_WLAN_STATE_IP_CONFIG_DONE:
      case RSI_WLAN_STATE_IPV6_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON:
      case RSI_WLAN_STATE_AUTO_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_FAILED:
        break;
      default: {
        // Return status if command given in driver is in an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
        if (join_response_handler == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
        }
      } break;
    }
    if (join_response_handler != NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
    }
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_CONNECT_ASYNC_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR);
    return allow;
  }
  // Return status
  SL_PRINTF(SL_WLAN_CONNECT_ASYNC_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief        Get background scan results or stop bgscan. This is a blocking API.
 * @pre             \ref rsi_wlan_connect() API needs to be called before this API.
 * @param[in]	 cmd - Command given by user, to enable or disable bgscan
 *                   0 - disable bgscan
 *                   1 - enable bgscan
 * @param[out]   result - buffer to store bgscan results
 * @param[in]    length - Length of bgscan result buffer
 * @return       0  	  -  Success \n
 *               Non-Zero Value - Failure\n
 *               		-3 - Command given in wrong state \n
 *               		-4 - Buffer not availableto serve the command.
 * @note	Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_wlan_bgscan_profile(uint8_t cmd, rsi_rsp_scan_t *result, uint32_t length)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_BGSCAN_PROFILE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check whether module is in valid state range or not
  if (wlan_cb->state < RSI_WLAN_STATE_CONNECTED) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_BGSCAN_PROFILE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)result;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_BGSCAN_PROFILE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset data
    memset(&pkt->data, 0, sizeof(rsi_req_bg_scan_t));

    // Magic word
    pkt->data[0] = 0xAB;

    // cmd
    pkt->data[1] = cmd;
#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

    // Send BG scan command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BG_SCAN, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BGSCAN_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_BGSCAN_PROFILE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_BGSCAN_PROFILE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief      Enable scan and roaming after connecting to the Access Point. This is a blocking API.
 * @pre        \ref rsi_wlan_connect() API needs to be called before this API.
 * @param[in]  Void
 * @return     0               -  Success \n
 *             Non-Zero  Value - Failure \n
 *             If return value is less than 0 \n
 *		         -3              - Command given in wrong state \n
 *		         -4              - Buffer not available to serve the command \n
 *             If return value is greater than 0 \n
 *             0x0006,0x0021,0x002C,0x004A,0x0025,0x0026 \n
 * @note	   	 Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

// Execute post connect commands
int32_t rsi_wlan_execute_post_connect_cmds(void)
{
#if (RSI_BG_SCAN_SUPPORT || RSI_ROAMING_SUPPORT)
  rsi_pkt_t *pkt;
#endif
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_ENRTY, WLAN, LOG_INFO, "status: %4x", status);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check whether module is in valid state range or not
  if (wlan_cb->state < RSI_WLAN_STATE_CONNECTED) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
#if RSI_BG_SCAN_SUPPORT
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset data
    memset(&pkt->data, 0, sizeof(rsi_req_bg_scan_t));

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send BG scan command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BG_SCAN, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BGSCAN_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    if (status != RSI_SUCCESS) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

      // Return status if error in sending command occurs
      SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
      return status;
    }

#endif
#if RSI_ROAMING_SUPPORT
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    // Memset data
    memset(&pkt->data, 0, sizeof(rsi_req_roam_params_t));

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send roaming parameters command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_ROAM_PARAMS, pkt);

    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_ROAMING_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
#endif
    // Wait on WLAN semaphore

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_NWK_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Start the WPS Push button in AP mode. Must be called after rsi_wlan_ap_start API once it has returned Success. This is a blocking API.
 * @pre         \ref rsi_wlan_ap_start() API needs to be called before this API.
 * @param[in]   ssid -SSID of the Access Point. SSID should be same as that of given in AP start API. Length of the SSID should be less than or equal to 32 bytes.
 * @return      0               - Success \n
 *              Non-Zero  Value - Failure \n
 *              If return value is less than 0 \n
 *		          -4              - Buffer not available to serve the command \n
 *              If return value is greater than 0 \n
 *              0x0021
 * @note        Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_wlan_wps_push_button_event(int8_t *ssid)
{
  rsi_pkt_t *pkt;
  rsi_req_join_t *join;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_WPS_PUSH_BUTTON_EVENT_ENRTY, WLAN, LOG_INFO, "status: %4x", status);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_WPS_PUSH_BUTTON_EVENT_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_WPS_PUSH_BUTTON_EVENT_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_join_t));

    join = (rsi_req_join_t *)pkt->data;

    if (ssid != NULL) {
      // Copy SSID and SSID len
      join->ssid_len = rsi_strlen(ssid);

      // Copy Join SSID
      rsi_strcpy(join->ssid, ssid);
    }

    // Deregister join response handler
    rsi_wlan_cb_non_rom->callback_list.wlan_join_response_handler = NULL;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send join command to start wps
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_JOIN_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_WPS_PUSH_BUTTON_EVENT_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_WPS_PUSH_BUTTON_EVENT_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief        Application to provide feedback of Frequency error in KHz. This is a blocking API.
 * @param[in]    freq_offset_in_khz - Frequency deviation observed in KHz
 * @return       0          - Success \n
 *               Non zero Value - Failure \n
 *               0xFC - Frequency offset sent is zero \n
 *		           0xFB - Frequency offset specified goes beyond the upper limit or lower limit and indicates that frequency offset cannot be changed further. \n
 *        
 * @note        Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 */
int32_t rsi_send_freq_offset(int32_t freq_offset_in_khz)
{
  rsi_pkt_t *pkt;
  rsi_freq_offset_t *freq_offset = NULL;
  int32_t status                 = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_SEND_FREQ_OFFSET_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SEND_FREQ_OFFSET_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_SEND_FREQ_OFFSET_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_freq_offset_t));

    freq_offset = (rsi_freq_offset_t *)pkt->data;

    freq_offset->freq_offset_in_khz = freq_offset_in_khz;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send WPS request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FREQ_OFFSET, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_FREQ_OFFSET_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SEND_FREQ_OFFSET_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_SEND_FREQ_OFFSET_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief             RF calibration process. This API will command the firmware to update the existing Flash/EFuse calibration data. This is a blocking API.
 * @pre    	\ref rsi_transmit_test_start(), \ref rsi_send_freq_offset() API needs to be called before this API.This API is relevant in PER mode only.
 * @param[in]         Target \n
 * 			0 - BURN_INTO_EFUSE (Burns calibration data to EFuse) \n
 * 			1 - BURN_INTO_FLASH (Burns calibration data to Flash) \n
 * @param[in]         Flags - Validate information \n
 *                     Bit |	MACRO 		           |	Description
 *                     :---|:---------------------:|:---------------------------------------------------
 *                     0   |	BURN_GAIN_OFFSET     | 	1 - Update gain offset to calibration data \n	0 - Skip gain offset update
 *                     1   |	BURN_FREQ_OFFSET     |	1 - Update XO Ctune to calibration data \n	0 - Skip XO Ctune update
 *                     2   |	SW_XO_CTUNE_VALID    |	1 - Use XO Ctune provided as argument to update calibration data \n	0 - Use XO Ctune value as read from hardware register
 *                     3   |	BURN_XO_FAST_DISABLE |     Used to apply patch for cold temperature issue(host interface detection) observed on CC0/CC1 modules. \ref appendix
 *                     7-4 |                       |	Reserved
 * @param[in]         gain_offset - gain_offset as observed in dBm
 * @param[in]         xo_ctune - Allow user to directly update xo_ctune value to calibration data bypassing the freq offset loop,
 * 			valid only when BURN_FREQ_OFFSET & SW_XO_CTUNE_VALID of flags is set.
 * @note              To recalibrate gain offset after it has been burnt to flash, the user is required to first reset gain offset and then follow the calibration flow. \n
 *                    e.g.: rsi_calib_write(BURN_INTO_FLASH,BURN_GAIN_OFFSET,0,0); // which resets gain offset \n
 *                    Recalibration is not possible if EFuse is being used instead of flash as calibration data storage            
 * @return            0			- Success \n
 *                    Non-Zero Value	- Failure
 */

int32_t rsi_calib_write(uint8_t target, uint32_t flags, int8_t gain_offset, int32_t xo_ctune)
{
  rsi_pkt_t *pkt;
  rsi_calib_write_t *calib_wr = NULL;
  int32_t status              = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_CALIB_WRITE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_CALIB_WRITE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_CALIB_WRITE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    calib_wr = (rsi_calib_write_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_calib_write_t));

    // Target flash or efuse
    calib_wr->target = target;

    // Flags
    calib_wr->flags = flags;

    // Gain offset
    calib_wr->gain_offset = gain_offset;

    // Freq offset
    calib_wr->xo_ctune = xo_ctune;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send WPS request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CALIB_WRITE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_CALIB_WRITE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_CALIB_WRITE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_CALIB_WRITE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief         Parse and convert the given value in ASCII to the datatype of a given length.. This is a non-blocking API.
 * @param[in]     address - Address
 * @param[in]     length - Length
 * @param[in]     value - Value
 * @return        0         - Success \n
 *                Non-Zero Value  - Failure
 */
int16_t rsi_parse(void *address, uint16_t length, uint8_t *value)
{
  uint8_t temp_buff[15];
  uint16_t temp_offset = 0;
  uint16_t offset;

  for (offset = 0; offset < length; offset++) {
    uint16_t index = 0;
    /* Copy the bytes until '.' ',' '\0'  is reached */
    while ((value[temp_offset] != '.') && (value[temp_offset] != ',') && (value[temp_offset] != '\0')) {
      if (index >= (sizeof(temp_buff) - 1))
        break;
      temp_buff[index++] = value[temp_offset++];
    }
    temp_buff[index] = '\0';
    temp_offset++;
    if (length == RSI_PARSE_2_BYTES) {
      *((uint16_t *)address) = rsi_atoi((int8_t *)temp_buff);
      break;
    } else if (length == RSI_PARSE_4_BYTES) {
      *((uint32_t *)address) = rsi_atoi((int8_t *)temp_buff);
      break;
    } else
      *((uint8_t *)address + offset) = rsi_atoi((int8_t *)temp_buff);
  }

  return temp_offset;
}

/*==============================================*/
/**
 * @brief      Generate WPS pin. This is a blocking API.
 * @pre        \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]  wps_pin - 8-byte WPS pin generated by the device. This is the output parameter.
 * @param[in]  length  -  This is the length of the resulted buffer measured in bytes to hold WPS pin.
 * @param[out] 8-byte WPS pin
 * @return     0               - Success \n
 *             Non-Zero  Value - Failure \n
 *             If return value is less than 0 \n
 *		         -4              - Buffer not available to serve the command \n
 *             If return value is greater than 0 \n
 *             0x0021,0x002C,0x0025,0x0037,0x0038 \n
 * @note       Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_wlan_wps_generate_pin(uint8_t *wps_pin, uint16_t length)
{
  rsi_pkt_t *pkt;
  rsi_req_wps_method_t *wps_method;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_WPS_GENERATE_PIN_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_WPS_GENERATE_PIN_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_WPS_GENERATE_PIN_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    wps_method = (rsi_req_wps_method_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_wps_method_t));

    // Set configuration to generate WPS pin
    rsi_uint16_to_2bytes(wps_method->wps_method, 1);

    // Set configuration to generate WPS pin
    rsi_uint16_to_2bytes(wps_method->generate_pin, 1);

    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)wps_pin;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send WPS request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WPS_METHOD, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WPS_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_WPS_GENERATE_PIN_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_WPS_GENERATE_PIN_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief	    	Validate WPS pin entered. This is a blocking API.
 * @pre   	    \ref rsi_wlan_init() API needs to be called before this API.
 * @param[in]   wps_pin - 8-byte valid wps pin
 * @param[out]  Same WPS pin if command is successful
 * @return		  0 	          	- Success \n
 *				      Non-Zero Value  - Failure
 */

int32_t rsi_wlan_wps_enter_pin(int8_t *wps_pin)
{
  rsi_pkt_t *pkt;
  rsi_req_wps_method_t *wps_method;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_WPS_ENTER_PIN_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_WPS_ENTER_PIN_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_WPS_ENTER_PIN_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    wps_method = (rsi_req_wps_method_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_wps_method_t));

    // Set configuration to validate WPS pin
    rsi_uint16_to_2bytes(wps_method->wps_method, 1);

    // Set configuration to validate WPS pin
    rsi_uint16_to_2bytes(wps_method->generate_pin, 0);

    // Copy WPS pin
    memcpy(wps_method->wps_pin, wps_pin, RSI_WPS_PIN_LEN);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send WPS request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WPS_METHOD, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WPS_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_WPS_ENTER_PIN_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_WPS_ENTER_PIN_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Get random bytes from the device. This is a blocking API.
 * @param[in]   result - Pointer to the buffer in which random data are copied from
 * @param[in]   length - Length of the random data required
 * @return      0         - Success \n
 *              Non-Zero Value  - Failure
 */

int32_t rsi_get_random_bytes(uint8_t *result, uint32_t length)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_GET_RANDOM_BYTES_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_GET_RANDOM_BYTES_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)result;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_GET_RANDOM_BYTES_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_disassoc_t));

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send get random number command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GET_RANDOM, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_GET_RANDOM_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_GET_RANDOM_BYTES_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_GET_RANDOM_BYTES_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Disconnect module from the connected access point. This is a blocking API.
 * @pre    		  \ref rsi_wlan_connect() API needs to be called before this API.
 * @return      Non-Zero Value  - Failure \n
 *              0               - Success \n
 *              If return value is less than 0 \n
 *	       	    -3              - Command given in wrong state \n
 *		          -4              - Buffer not availableto serve the command \n
 *	            If return value is greater than 0 \n
 *		          0x0006,0x0021,0x002C,0x004A,0x0025,0x0026 \n
 * @note        Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_wlan_disconnect(void)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_DISCONNECT_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_CONNECTED) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_DISCONNECT_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_DISCONNECT_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_disassoc_t));

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send disconnect command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DISCONNECT, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_DISCONNECT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_DISCONNECT_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_DISCONNECT_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Disconnect the connected stations in AP mode. This is a blocking API.
 * @pre        \ref rsi_wlan_ap_start() API needs to be called before this API.
 * @param[in]  mac_address - Mac address (6 bytes) of the station to be disconnected.
 * @return     0               - Success \n
 *             Non-Zero Value  - Failure \n
 *             If return value is less than 0 \n
 *	           -4              - Buffer not available to serve the command \n
 *             If return value is greater than 0 \n
 *             0x0013, 0x0021, 0x002C, 0x0015 \n
 * @note       Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_wlan_disconnect_stations(uint8_t *mac_address)
{
  rsi_pkt_t *pkt;
  rsi_req_disassoc_t *disassoc_info;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_DISCONNECT_STATIONS_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_DISCONNECT_STATIONS_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_DISCONNECT_STATIONS_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    disassoc_info = (rsi_req_disassoc_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_disassoc_t));

    // Set mode flag to represent AP mode
    rsi_uint16_to_2bytes(disassoc_info->mode_flag, 1);

    // Copy MAC address of station to be disconnected
    memcpy(disassoc_info->client_mac_address, mac_address, 6);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send disconnect command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DISCONNECT, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_DISCONNECT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_DISCONNECT_STATIONS_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_DISCONNECT_STATIONS_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/** @} */

/** @addtogroup WLAN
* @{
*/

/*==============================================*/
/**
 * @fn             int32_t rsi_config_ipaddress(rsi_ip_version_t version, uint8_t mode, uint8_t *ip_addr,
 *                                           uint8_t *mask, uint8_t *gw, uint8_t *ipconfig_rsp,
 *                                            uint16_t length, uint8_t vap_id);
 * @brief           Configure the IP address to the module. This is a blocking API.
 *                  Send IPconfig command to configure IP address with input parameters (like ipversion, static mode/dhcp mode,vap_id)and return error in case of invalid parameters.
 * @pre            \ref rsi_wlan_connect() API needs to be called before this API.
 * @param[in]    version           - IP version to be used. RSI_IP_VERSION_4 (4) – to select IPv4,RSI_IP_VERSION_6 (6) – to select IPv6
 * @param[in]    mode              - 1 - DHCP mode ; 0 - static mode
 * @param[in]    ip_addr           - Pointer to IP address
 * @param[in]    mask              - Pointer to network mask
 * @param[in]    gw                - Pointer to gateway address
 * @param[in]    ipconfig_rsp     - Hold the IP configuration received using DHCP.
 * @param[in]    length            - Length of ipconfig_rsp buffer
 * @param[in]    vap_id           - VAP id to differentiate between AP and station in concurrent mode. 0 – for station, 1 – for Access point
 * @return       **Success**      - RSI_SUCCESS \n
 *               **Failure**      - Non-Zero Value \n
 *
 *                **RSI_ERROR_INVALID_PARAM**                 - Invalid parameters\n
 *
 *                **RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE**   - Command given in wrong state \n
 *
 *                **RSI_ERROR_PKT_ALLOCATION_FAILURE**         - Buffer not available to serve the command
 * @note        Refer to Error Codes section for the description of other error codes  \ref error-codes.
 *
 */
int32_t rsi_config_ipaddress(rsi_ip_version_t version,
                             uint8_t mode,
                             uint8_t *ip_addr,
                             uint8_t *mask,
                             uint8_t *gw,
                             uint8_t *ipconfig_rsp,
                             uint16_t length,
                             uint8_t vap_id)
{
  rsi_pkt_t *pkt;
  rsi_req_ipv4_parmas_t *ip_params;
  rsi_req_ipv6_parmas_t *ipv6_params;
  int32_t status                 = RSI_SUCCESS;
  int32_t rsi_response_wait_time = 0;
  // Get WLAN CB struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // WLAN radio init
  status = rsi_wlan_radio_init();
  SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_ENTRY, WLAN, LOG_INFO);
  if (status != RSI_SUCCESS) {
    // Return status if error in sending command occurs
    SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)ipconfig_rsp;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    // If ipversion is IPv4
    if (version == RSI_IP_VERSION_4) {
      // Fill all the ipv4 parameters
      ip_params = (rsi_req_ipv4_parmas_t *)pkt->data;

      // Set DHCP mode
      ip_params->dhcp_mode = mode;

      // Fill IP address only if static mode is selected
      if (mode == RSI_STATIC) {
        // Fill IP address
        memcpy(ip_params->ipaddress, ip_addr, RSI_IPV4_ADDRESS_LENGTH);

        // Fill network mask
        memcpy(ip_params->netmask, mask, RSI_IPV4_ADDRESS_LENGTH);

        // Fill gateway
        memcpy(ip_params->gateway, gw, RSI_IPV4_ADDRESS_LENGTH);
      }

      // Check for DHCP HOSTNAME feature
      if ((mode & RSI_DHCP_HOSTNAME) == RSI_DHCP_HOSTNAME) {
        // Copy DHCP client hostname
        rsi_strcpy(ip_params->hostname, RSI_DHCP_HOST_NAME);
      }

      // Set vap_id
      ip_params->vap_id = vap_id;
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

      // Send ipconfig command
      status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_IPCONFV4, pkt);
      rsi_response_wait_time = RSI_IPCONFV4_RESPONSE_WAIT_TIME;

    } else if (version == RSI_IP_VERSION_6) {
      // Fill all the IPv6 params
      ipv6_params = (rsi_req_ipv6_parmas_t *)pkt->data;

      // Fill prefix length
      rsi_uint16_to_2bytes(ipv6_params->prefixLength, 64);

      // Fill DHCP mode
      rsi_uint16_to_2bytes(ipv6_params->mode, mode);

      // Fill IP address only if static mode is selected
      if (mode == RSI_STATIC) {
        // Fill IPv6 address
        memcpy(ipv6_params->ipaddr6, ip_addr, RSI_IPV6_ADDRESS_LENGTH);

        // Fill gateway
        memcpy(ipv6_params->gateway6, gw, RSI_IPV6_ADDRESS_LENGTH);
      }

#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
      // Send ipconfig command
      status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_IPCONFV6, pkt);
      rsi_response_wait_time = RSI_IPCONFV6_RESPONSE_WAIT_TIME;
    } else {

      // Detach buffer
      wlan_cb->app_buffer = NULL;

      // Make length of the buffer to 0
      wlan_cb->app_buffer_length = 0;

      // Update Throw error in case of invalid parameters
      status = RSI_ERROR_INVALID_PARAM;
      // Free the packet
      rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_CHANGE_WLAN_CMD_STATE_TO_ALLOW, WLAN, LOG_ERROR, "status: %4x", status);
      return status;
    }

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Write or erase certificate into module. This is a blocking API.
 * @pre          \ref rsi_wireless_init() API must be called before this API.
 * @param[in]  certificate_type   - Type of certificate
 * @param[in]  cert_inx 		      - Index of certificate
 * @param[in]  buffer 			      - Certificate content
 * @param[in]  certificate_length - Certificate length
 * @return     0                -  Success \n
 *             Non-Zero  Value - Failure \n
 *             If return value is less than 0 \n
 *             -2 Invalid Parameters \n
 *             -3 Command given in wrong state \n
 *		         -4 Buffer not available to serve the command \n
 *             If return value is greater than 0 \n
 *             0x0015,0x0021,0x0025,0x0026,0x002C 
 * @note 		   Index-based certificate loading is valid only for storing certificates on to RAM or flash but not both at the same time. \n
 * 				     Enable BIT(27) in tcp_ip_feature_bit_map to load SSl certificate into RAM. \n
 *				     Enable BIT(31) in tcp_ip_feature_bit_map and BIT (29) in ext_tcp_ip_feature_bit_map to open 3 SSL client sockets. \n
 *				     Three SSL client sockets feature supported only in WLAN mode.
 *
 *
 */
int32_t rsi_wlan_set_certificate_index(uint8_t certificate_type,
                                       uint8_t cert_inx,
                                       uint8_t *buffer,
                                       uint32_t certificate_length)
{
  static uint32_t rem_len;
  uint16_t chunk_size = 0;
  static uint32_t offset;
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  rsi_req_set_certificate_t *rsi_chunk_ptr;
  int32_t rsi_response_wait_time = 0;
  SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check if command is in correct state or not
  if ((wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  if ((buffer == NULL) && (certificate_length)) {
    // Throw error in case of invalid parameters
    SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_INVALID_PARAM, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Get the certificate chunk size
    chunk_size = (RSI_MAX_CERT_SEND_SIZE - sizeof(struct rsi_cert_info_s));

    // Get certificate length
    rem_len = certificate_length;

    // If certificate length is 0, erase certificate
    if (rem_len == 0) {
      // Allocate command buffer from WLAN pool
      pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

      // If allocation of packet fails
      if (pkt == NULL) {
        // Change the WLAN CMD state to allow
        rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
        // Return packet allocation failure error
        SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
        return RSI_ERROR_PKT_ALLOCATION_FAILURE;
      }

      rsi_chunk_ptr = (rsi_req_set_certificate_t *)pkt->data;

      // Memset the PKT
      memset(&pkt->data, 0, RSI_MAX_CERT_SEND_SIZE);

      //Set the total length of certificate
      rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.total_len, certificate_length);

      // Set the certificate type
      rsi_chunk_ptr->cert_info.certificate_type = certificate_type;

      // Set the certificate index
      rsi_chunk_ptr->cert_info.certificate_inx = cert_inx;

      // More chunks to send
      rsi_chunk_ptr->cert_info.more_chunks = 0;

      // Set the length of the certificate chunk
      rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.certificate_length, 0);

      rsi_response_wait_time = RSI_CERTIFICATE_RESPONSE_WAIT_TIME;
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
      // Send the driver command
      status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_CERTIFICATE, pkt);
      // Wait on WLAN semaphore
      rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
      // Get WLAN/network command response status
      status = rsi_wlan_get_status();
      if (status != RSI_SUCCESS) {
        // Change the WLAN CMD state to allow
        rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

        // Return status if error in sending command occurs
        SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
        return status;
      }
    }
    while (rem_len) {
      // Allocate command buffer from WLAN pool
      pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

      // If allocation of packet fails
      if (pkt == NULL) {
        // Change the WLAN CMD state to allow
        rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
        // Return packet allocation failure error
        SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
        return RSI_ERROR_PKT_ALLOCATION_FAILURE;
      }

      rsi_chunk_ptr = (rsi_req_set_certificate_t *)pkt->data;
      // Memset the PKT
      memset(&pkt->data, 0, RSI_MAX_CERT_SEND_SIZE);

      if (rem_len >= chunk_size) {
        // Copy the certificate chunk
        memcpy(rsi_chunk_ptr->certificate, buffer + offset, chunk_size);

        // Move the offset by chunk size
        offset += chunk_size;

        // Subtract the rem_len by the chunk size
        rem_len -= chunk_size;

        //Set the total length of certificate
        rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.total_len, certificate_length);

        // Set the certificate type
        rsi_chunk_ptr->cert_info.certificate_type = certificate_type;

        // Set the certificate index
        rsi_chunk_ptr->cert_info.certificate_inx = cert_inx;

        // More chunks to send
        rsi_chunk_ptr->cert_info.more_chunks = 1;

        // Set the length of the certificate chunk
        rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.certificate_length, chunk_size);

        rsi_response_wait_time = RSI_CERTIFICATE_RESPONSE_WAIT_TIME;
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send the driver command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_CERTIFICATE, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
        // Get WLAN/network command response status
        status = rsi_wlan_get_status();
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }

      } else {

        // Copy the certificate chunk
        memcpy(rsi_chunk_ptr->certificate, buffer + offset, rem_len);

        // Move the offset by chunk size
        offset += rem_len;

        //Set the total length of certificate
        rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.total_len, certificate_length);

        // Set the certificate type
        rsi_chunk_ptr->cert_info.certificate_type = certificate_type;

        // Set the certificate index
        rsi_chunk_ptr->cert_info.certificate_inx = cert_inx;

        // Last chunks to send
        rsi_chunk_ptr->cert_info.more_chunks = 0;

        // Set the length of the certificate chunk
        rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.certificate_length, rem_len);

        rsi_response_wait_time = RSI_CERTIFICATE_RESPONSE_WAIT_TIME;
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send the driver command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_CERTIFICATE, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
        // Get WLAN/network command response status
        status = rsi_wlan_get_status();
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_ERROR_IN_SENDING_COMMAND_3, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }

        // Reset rem_len and offset
        rem_len = 0;
        offset  = 0;
      }
    }

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get WLAN/network command response status
  status = rsi_wlan_get_status();

  // Return status
  SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */

/** @addtogroup NETWORK16
* @{
*/
/*==============================================*/
/**
 * @brief      Load SSL/EAP certificate on WiSeConnect module. This is a blocking API.
 * @pre        \ref rsi_wireless_init() API must be called before this API.
 * @param[in]  certificate_type   - Type of certificate \n
 *                                  These are the certificate types: \n
 *                                  1: TLS client certificate \n
 *                                  2: FAST PAC file          \n
 *                                  3: SSL Client Certificate \n
 *                                  4: SSL Client Private Key \n
 *                                  5: SSL CA Certificate     \n
 *                                  6: SSL Server Certificate \n
 *                                  7: SSL Server Private Key \n
 * @param[in]  buffer 			      - Pointer to a buffer which contains the certificate
 * @param[in]  certificate_length - Certificate length
 * @return     0               - Success \n
 *             Non-Zero  Value - Failure \n
 *             If return value is less than 0 \n
 *             -2 Invalid Parameters \n
 *             -3 Command given in wrong state \n
 *		         -4 Buffer not available to serve the command \n
 *             If return value is greater than 0 \n
 *             0x0015,0x0021,0x0025,0x0026,0x002C 
 * @note       Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */
int32_t rsi_wlan_set_certificate(uint8_t certificate_type, uint8_t *buffer, uint32_t certificate_length)
{
  SL_PRINTF(SL_WLAN_SET_CERTIFICATE_EXIT, NETWORK, LOG_INFO);
  return rsi_wlan_set_certificate_index(certificate_type, 0, buffer, certificate_length);
}
/** @} */

/** @addtogroup WLAN
* @{
*/
/*==============================================*/
/**
 * @brief       Check the status (specific error code) of errors encountered during a call to a WLAN API or BSD sockets
 * 				      functions. User can call this API to check the error code.This is a non-blocking API.
 * @param[in]   Void
 * @return      Return the error code that previously occurred. If no error occurred, return 0.
 * @note        Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 */
int32_t rsi_wlan_get_status(void)
{
  SL_PRINTF(SL_WLAN_GET_STATUS_EXIT, WLAN, LOG_INFO);
  return rsi_driver_cb->wlan_cb->status;
}

/*==============================================*/
/**
 * @brief     Get the required information based on the type of command. This is a blocking API.
 * @pre 		  \ref rsi_wireless_init () API needs to be called before this API.
 * 				    Return WLAN status
 * @param[in] cmd_type  -  Query command type: \n
 *                      	 1 : RSI_FW_VERSION \n
 *              		       2 : RSI_MAC_ADDRESS \n
 *              		       3 : RSI_RSSI \n
 *              		       4 : RSI_WLAN_INFO \n
 *              		       5 : RSI_CONNECTION_STATUS \n
 *              		       6 : RSI_STATIONS_INFO \n
 *              		       7 : RSI_SOCKETS_INFO \n
 *              		       8 : RSI_CFG_GET \n
 *                         9 : RSI_GET_WLAN_STAT
 * @param[in]  response    - Response of the requested command. This is an output parameter.
 * @param[in]  length      - Length of the response buffer in bytes to hold result.
 * @note	     RSI_WLAN_INFO is relevant in both station and AP mode. \n
 *				     RSI_SOCKETS_INFO is relevant in both station mode and AP mode. \n
 *				     RSI_STATIONS_INFO is relevant in AP mode \n
 *				     RSI_GET_WLAN_STATS is relevant in AP and Station mode
 * @return   	 Non-Zero Value  - Failure \n
 *                        0 	 - Success \n
 *				               -3    - Command given in wrong state \n
 *				               -4    - Buffer not availableto serve the command \n
 *				               -6    - Insufficient input buffer given
 * @note       Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 *
 */
int32_t rsi_wlan_get(rsi_wlan_query_cmd_t cmd_type, uint8_t *response, uint16_t length)
{
  int32_t status = RSI_SUCCESS;
  rsi_pkt_t *pkt;
  int32_t rsi_response_wait_time = 0;

  SL_PRINTF(SL_WLAN_GET_ENTRY, WLAN, LOG_INFO);

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get common control block structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  // If state is not in card ready received state
  if (common_cb->state == RSI_COMMON_STATE_NONE) {
    if (cmd_type == RSI_FW_VERSION) {
      while (common_cb->state != RSI_COMMON_CARDREADY) {
#ifndef RSI_WITH_OS
        rsi_scheduler(&rsi_driver_cb->scheduler_cb);
#endif
      }
    } else {
      // Command given in wrong state
      SL_PRINTF(SL_WLAN_GET_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if (cmd_type == RSI_FW_VERSION) {
    // Send firmware version query request
    status = rsi_get_fw_version(response, length);
    SL_PRINTF(SL_WLAN_GET_FW_VERSION_QUERY_REQ, WLAN, LOG_INFO, "status: %4x", status);
    return status;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Copy query CMD type in driver control block
    wlan_cb->query_cmd = cmd_type;

    if (response != NULL) {
      // Attach the buffer given by user
      wlan_cb->app_buffer = response;

      // Length of the buffer provided by user
      wlan_cb->app_buffer_length = length;
    } else {
      // Assign NULL to the app_buffer to avoid junk
      wlan_cb->app_buffer = NULL;

      // Length of the buffer to 0
      wlan_cb->app_buffer_length = 0;
    }
    switch (cmd_type) {
      case RSI_RSSI: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
        rsi_response_wait_time = RSI_WLAN_RSSI_RESPONSE_WAIT_TIME;
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send RSSI query request
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_RSSI, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_MAC_ADDRESS: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send MAC address query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_MAC_ADDRESS, pkt);
        rsi_response_wait_time = RSI_MAC_RESPONSE_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_CONNECTION_STATUS: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_3, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send MAC address query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CONNECTION_STATUS, pkt);
        rsi_response_wait_time = RSI_CONNECT_RESPONSE_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_WLAN_INFO: {
        if (length < sizeof(rsi_rsp_wireless_info_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_1, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }
        if (wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR, "status: %4x", status);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send CMD for WLAN info in AP mode
          status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_GO_PARAMS, pkt);
          rsi_response_wait_time = RSI_WLAN_QUERY_NETWORK_PARAMS_WAIT_TIME;
          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
        } else if ((wlan_cb->opermode == RSI_WLAN_CLIENT_MODE) || (wlan_cb->opermode == RSI_WLAN_ENTERPRISE_CLIENT_MODE)
                   || (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE)) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_5, WLAN, LOG_ERROR, "status: %4x", status);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send CMD for WLAN info in client mode
          status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_NETWORK_PARAMS, pkt);
          rsi_response_wait_time = RSI_WLAN_QUERY_NETWORK_PARAMS_WAIT_TIME;
          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
        } else if (wlan_cb->opermode == RSI_WLAN_WIFI_DIRECT_MODE) {
          // No code here
        }
      } break;
      case RSI_STATIONS_INFO: {
        if (length < sizeof(rsi_rsp_stations_info_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_2, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_6, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send MAC address query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_GO_PARAMS, pkt);
        rsi_response_wait_time = RSI_WLAN_REQ_QUERY_GO_PARAMS;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_SOCKETS_INFO: {
        if (length < sizeof(rsi_rsp_sockets_info_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_3, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }

        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_7, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send MAC address query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_NETWORK_PARAMS, pkt);
        rsi_response_wait_time = RSI_WLAN_QUERY_NETWORK_PARAMS_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_CFG_GET: {
        if (length < sizeof(rsi_cfgGetFrameRcv_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_4, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_8, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send CFG_GET command for getting user store configuration
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GET_CFG, pkt);
        rsi_response_wait_time = RSI_WLAN_REQ_GET_CFG_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_GET_WLAN_STATS: {

        if (length < sizeof(rsi_rsp_wlan_stats_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_5, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_9, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send Wi-fi STATS query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GET_STATS, pkt);
        rsi_response_wait_time = RSI_WLAN_REQ_GET_STATS_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_WLAN_EXT_STATS: {

        if (length < sizeof(rsi_wlan_ext_stats_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          //   SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_5, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          //  SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_9, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send Wi-fi STATS query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EXT_STATS, pkt);
        rsi_response_wait_time = RSI_WLAN_REQ_EXT_STATS_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_FW_VERSION:
        break;
      default:
        // Return status if command given in driver is in an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
        break;
    }
    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_GET_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_WLAN_GET_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Request configuration based on the command type. This is a blocking API.
 * @pre   		  \ref rsi_wireless_init() API needs to be called before this API. \n
 *        		  For setting MAC address, call this API immediately after \n
 *		  		    \ref rsi_wireless_init() and before calling any other API. \n
 *		  		    \ref rsi_config_ipaddress() needs to be call for RSI_CFG_SAVE and RSI_CFG_STORE.
 * @param[in]   cmd_type  - 	Set command type:  : \n
 *                       	1 : RSI_SET_MAC_ADDRESS \n
 *                       	2 : RSI_MULTICAST_FILTER \n
 *                      	3 : RSI_JOIN_BSSID \n
 *                      	4 : RSI_CFG_SAVE  \n
 *                      	5 : RSI_CFG_STORE  \n
 * @param[in]  request    -   Request buffer
 * @param[in]  length     -   Length of the request buffer in bytes
 * 		         cmd type                |	Request structure	
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 	 	         RSI_SET_MAC_ADDRESS     |      uint8_t mac_address[6]
 * 	 	         RSI_MULTICAST_FILTER    |      typedef struct rsi_req_multicast_filter_info_s
 *	           ^        	             |    	{ uint8_t cmd_type;
 *	           ^        	             |    	  uint8_t mac_address[6];
 *	           ^                       |         }rsi_req_multicast_filter_info_t;
 *	           ^                       |      cmd_type are : \n
 *	           ^                       |      1 . RSI_ MULTICAST_MAC_ADD_BIT (To set particular bit in multicast bitmap) \n
 *	           ^        	             |    	2 . RSI_MULTICAST_MAC_CLEAR_BIT (To reset particular bit in multicast bitmap \n
 *	           ^        	             |    	3 . RSI_MULTICAST_MAC_CLEAR_ALL (To clear all the bits in multicast bitmap) \n
 *	           ^        	             |    	4 . RSI_MULTICAST_MAC_SET_ALL (To set all the bits in multicast bitmap) \n
 * 	 	         RSI_JOIN_BSSID          |      uint8_t join_bssid[6]
 * 	 	         RSI_CFG_SAVE            |      This cmd_type is used to save the parameters into non-volatile memory which are used either to join to an Access point (auto-join mode) or to create an Access point(auto-create mode).
 * 	 	         RSI_CFG_STORE           |      This cmd_type is used to give the configuration values which are supposed to be stored in the module's non-volatile memory and that are used in auto-join or auto-create modes. 
 *	           ^        	             |    	User configurations are stored in rsi_user_store_config_t structure. \n
 * @return     0              -  Success \n
 *             Non-Zero	Value - Failure \n
 *	           If return value is less than 0 \n
 *             -2             - Invalid parameters \n
 *	           -3             - Command given in wrong state \n
 *	           -4             - Buffer not available to serve the command \n
 *             If return value is greater than 0 \n
 * 	           0x0002, 0x0003, 0x0005, 0x000A, 0x0014, 0x0015, 0x001A, 0x0021,0x0024,0x0025,0x0026,0x002C,0x003c
 * @note       Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 *
 */
int32_t rsi_wlan_set(rsi_wlan_set_cmd_t cmd_type, uint8_t *request, uint16_t length)
{
  // Statement added to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(length);
  int32_t status = RSI_SUCCESS;

  SL_PRINTF(SL_WLAN_SET_ENTRY, WLAN, LOG_INFO);
  rsi_pkt_t *pkt;
  rsi_user_store_config_t *user_store_ptr = NULL;
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb         = rsi_driver_cb->wlan_cb;
  int32_t rsi_response_wait_time = 0;
  // Get common control block structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  // If state is not in card ready received state
  if (common_cb->state == RSI_COMMON_STATE_NONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SET_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  if (!(cmd_type == 4)) {
    if (request == NULL) {
      // Command given in wrong state
      SL_PRINTF(SL_WLAN_SET_NVALID_PARAM, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_INVALID_PARAM;
    }
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    switch (cmd_type) {
      case RSI_SET_MAC_ADDRESS: {
        // Enable valid bit to ensure setting MAC id before every initilization
        wlan_cb->field_valid_bit_map |= RSI_SET_MAC_BIT;

        // Copy MAC address in to WLAN control block
        memcpy(wlan_cb->mac_address, request, 6);

      } break;

      case RSI_MULTICAST_FILTER: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SET_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_multicast_filter_info_t));

        // Copy MAC address
        memcpy((uint8_t *)&pkt->data, request, sizeof(rsi_req_multicast_filter_info_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Set multicast filter configuration
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MULTICAST_FILTER, pkt);
        rsi_response_wait_time = RSI_MULTICAST_FIL_RESPONSE_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
      } break;

      case RSI_JOIN_BSSID: {

        // Copy MAC address in to WLAN control block
        memcpy(rsi_wlan_cb_non_rom->join_bssid_non_rom, request, 6);

      } break;

      case RSI_CFG_SAVE: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SET_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send CFG_SAVE command for saving user store configuration.
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CFG_SAVE, pkt);
        rsi_response_wait_time = RSI_WLAN_CFG_SAVE_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_CFG_STORE: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SET_PKT_ALLOCATION_FAILURE_3, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
        user_store_ptr = (rsi_user_store_config_t *)pkt->data;
        // Memset buffer
        memset(&pkt->data, 0, sizeof(rsi_user_store_config_t));
        // Copy user store configuration parameter to the packet
        memcpy(user_store_ptr, (rsi_user_store_config_t *)request, sizeof(rsi_user_store_config_t));
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send PSK command
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_USER_STORE_CONFIG, pkt);
        rsi_response_wait_time = RSI_USER_SC_RESPONSE_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;

      default:
        // Return status if command given in driver is in an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
        break;
    }
    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SET_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_WLAN_SET_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @fn       int32_t rsi_wlan_buffer_config(void)
 * @brief    Configure the TX ,RX global buffers ratio. This is a blocking API.
 * @pre    	\ref rsi_wlan_buffer_config() API needs to be called after opermode command only
 * @param[in]	dynamic_tx_pool	- Configure the dynamic tx ratio
 * @param[in]	dynamic_rx_pool	- Configure the dynamic rx ratio
 * @param[in]	dynamic_global_pool - Configure the dynamic global ratio
 * @return 
 * 		Non-Zero Value  - Failure \n
 *              0 		- Success \n
 *              -2 - Invalid parameters \n
 *		-3 - Command given in wrong state \n
 *		-4 - Buffer not availableto serve the command \n
 *		If return value is greater than zero : 0x0021
 * @note        Refer to Error Codes section for the description of the above error codes  \ref error-codes
 * 		Parameters given here are used internally by the API
 *
 *
 */
int32_t rsi_wlan_buffer_config(void)
{
  rsi_pkt_t *pkt;
  rsi_udynamic *dyn_buf;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_BUFFER_CONFIG_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_BUFFER_CONFIG_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_BUFFER_CONFIG_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Take the user provided data and fill it in antenna select structure
    dyn_buf = (rsi_udynamic *)pkt->data;

    // PLL mode value
    dyn_buf->dynamic_tx_pool = TX_POOL_RATIO;

    // RF type
    dyn_buf->dynamic_rx_pool = RX_POOL_RATIO;

    // Wireless mode
    dyn_buf->dynamic_global_pool = GLOBAL_POOL_RATIO;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send antenna select command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DYNAMIC_POOL, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_DYNAMIC_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_BUFFER_CONFIG_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_WLAN_BUFFER_CONFIG_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Start the module in access point mode with the given configuration. This is a blocking API.
 * @pre           \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]  ssid - SSID of the access point. Length of the SSID should be less than or equal to 32 bytes.
 * @param[in]  channel - Channel number. Refer the following channels for the valid channel numbers supported: \n
 *                      2.4GHz Band Channel Mapping, 5GHz Band Channel Mapping \n
 *                      channel number 0 is to enable ACS feature
 *			####The following table maps the channel number to the actual radio frequency in the 2.4 GHz spectrum####
 *			Channel numbers (2.4GHz)|	Center frequencies for 20MHz channel width 
 *			:----------------------:|:-----------------------------------------------:
 *			1			|	2412 
 *			2			|	2417  
 *			3			|	2422 
 *			4			|	2427
 *			5			|	2432 
 *			6			|	2437 
 *			7			|	2442 
 *			8			|	2447 
 *			9			|	2452
 *			10		|	2457  
 *			11		|	2462 
 *			12		|	2467 
 *			13		|	2472 
 *			14		|	2484 
 * 	####The following table maps the channel number to the actual radio frequency in the 5 GHz spectrum####
 * 		Channel Numbers(5GHz) |	Center frequencies for 20MHz channel width 
 * 		:--------------------:|:------------------------------------------:
 *		36		      |5180 
 *		40		      |5200 
 *		44		      |5220 
 *		48		      |5240 
 *		144		      |5700 
 *		149		      |5745 
 *		153		      |5765 
 *		157		      |5785 
 *		161		      |5805 
 *		165		      |5825 
 * @note	DFS channels are not supported in AP mode.
 * @param[in]  security_type - Type of the security modes on which an access point needs to be operated: \n
 *                         1 : RSI_OPEN \n
 *                         2 : RSI_WPA \n
 *                         3 : RSI_WPA2 \n
 *                         4 : RSI_WPA_WPA2_MIXED \n
 *                         5 : RSI_WPS_PUSH_BUTTON
 * @param[in]  encryption_mode - Type of the encryption mode:\n
 *                         0 : RSI_NONE \n
 *                         1 : RSI_TKIP \n
 *                         2 : RSI_CCMP
 * @param[in]  password - PSK to be used in security mode. \n
 *                      Minimum and maximum length of PSK is 8 bytes and 63 bytes respectively
 * @param[in]  beacon_interval - Beacon interval in ms. Allowed values are integers from 100 to 1000 which are multiples of 100.
 * @param[in]  dtim_period - DTIM period. Allowed values are integers between 1 and 255.
 *
 * @return     Non-Zero Value  - Failure \n
 *                    0         - Success\n
 *                -2 - Invalid parameters \n
 *                -3 - Command given in wrong state \n
 *                -4 - Buffer not availableto serve the command
 * @note        Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 *
 */
int32_t rsi_wlan_ap_start(int8_t *ssid,
                          uint8_t channel,
                          rsi_security_mode_t security_type,
                          rsi_encryption_mode_t encryption_mode,
                          uint8_t *password,
                          uint16_t beacon_interval,
                          uint8_t dtim_period)
{

  rsi_pkt_t *pkt;
  rsi_req_join_t *join;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_AP_START_ENTRY, WLAN, LOG_INFO);
  rsi_req_ap_config_t *ap_config;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_AP_START_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    switch (wlan_cb->state) {
      case RSI_WLAN_STATE_OPERMODE_DONE: {
        if (wlan_cb->field_valid_bit_map & RSI_SET_MAC_BIT) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_mac_address_t));

          // Memcpy data
          memcpy(&pkt->data, wlan_cb->mac_address, sizeof(rsi_req_mac_address_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send set MAC command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MAC_ADDRESS, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_MAC_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
            return status;
          }
        }
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_band_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send band command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BAND, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BAND_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // If BAND command fails
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_BAND_DONE: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_3, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send init command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_INIT, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_INIT_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_3, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#if RSI_SET_REGION_AP_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_set_region_ap_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send set region AP command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION_AP, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_AP_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_4, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#endif
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_IP_CONFIG_DONE:
      case RSI_WLAN_STATE_IPV6_CONFIG_DONE:
      case RSI_WLAN_STATE_INIT_DONE: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
        // Memset packet
        memset(&pkt->data, 0, sizeof(rsi_req_ap_config_t));

        ap_config = (rsi_req_ap_config_t *)pkt->data;

        // Copy channel by uint16 to 2 bytes conversion
        rsi_uint16_to_2bytes(ap_config->channel, channel);

        if (ssid != NULL) {
          // Copy Join SSID
          rsi_strcpy(ap_config->ssid, ssid);
        }

        // Security type
        ap_config->security_type = security_type;
        if (ap_config->security_type == RSI_WPA3) {
          ap_config->security_type = SME_WPA3;
        }
        if (ap_config->security_type == RSI_WPA3_TRANSITION) {
          ap_config->security_type = SME_WPA3_TRANSITION;
        }

        // Encryption mode
        ap_config->encryption_mode = encryption_mode;

        // If security is enabled
        if (security_type) {
          // Copy PSK
          rsi_strcpy(ap_config->psk, password);
        }

        // Copy beacon interval by uint16 to 2 bytes conversion
        rsi_uint16_to_2bytes(ap_config->beacon_interval, beacon_interval);

        // Copy DTIM period by uint16 to 2 bytes conversion
        rsi_uint16_to_2bytes(ap_config->dtim_period, dtim_period);

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send AP config command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_AP_CONFIGURATION, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_AP_CONFIG_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        // Check status for AP config command
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_5, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }

#if RSI_MODE_11N_ENABLE
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_5, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset packet
        memset(&pkt->data, 0, sizeof(rsi_req_ap_ht_caps_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send HT capabilities command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HT_CAPABILITIES, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_HT_CAPS_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_6, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_6, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
        // Memset the PKT
        memset(&pkt->data, 0, sizeof(rsi_req_join_t));

        join = (rsi_req_join_t *)pkt->data;

        if (ssid != NULL) {
          // Copy SSID and SSID len
          join->ssid_len = rsi_strlen(ssid);

          // Copy Join SSID
          rsi_strcpy(join->ssid, ssid);
        }

        // vap id of the current mode 0 - station mode, 1 - AP1 mode
        //  Applicable in cocurrent mode only
#if CONCURRENT_MODE
        join->vap_id = 1;
#else
        join->vap_id = 0;
#endif

        // Deregister join response handler
        rsi_wlan_cb_non_rom->callback_list.wlan_join_response_handler = NULL;

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send join command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_JOIN_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_7, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }

      } break;

      case RSI_WLAN_STATE_NONE:
      case RSI_WLAN_STATE_SCAN_DONE:
      case RSI_WLAN_STATE_CONNECTED:
      case RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON:
      case RSI_WLAN_STATE_AUTO_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_FAILED:

        break;
      default: {
        // Return status if command given in driver is in an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
      } break;
    }
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_WLAN_AP_START_NWK_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_8, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_power_save_with_listen_interval(uint8_t psp_mode, uint8_t psp_type,uint16_t listen_interval)
 * @brief       Set the power save profile in WLAN mode with listen interval-based wakeup. This is a blocking API.
 * @pre         \ref rsi_wireless_init() API needs to be called before this API. 
 * @param[in]   psp_mode
 * @param[in]   psp_type
 * 	          	parameter              |  Description	
 * 		          :----------------------|:--------------------------------------------------------------------------------------------------
 *              psp_mode               |   Following psp_mode is defined.
 *              ^                      |   Active(0) : In this mode, module is active and power save is disabled. \n
 *              ^                      |   RSI_SLEEP_MODE_1 (1): Connected sleep mode. \n
 *              ^                      |   In this sleep mode, SoC will never turn off, therefore no handshake is required before sending data to the module. \n
 *              ^                      |   RSI_SLEEP_MODE_2 (2): In this sleep mode, SoC will go to LP/ULP (with/without RAM RETENTION) sleep based on the selected value set for RSI_SELECT_LP_OR_ULP_MODE in rsi_wlan_config.h. \n
 *              ^                      |   Therefore handshake is required before sending data to the module. \n
 * 		          ^                      |   RSI_SLEEP_MODE_8 (8): Deep sleep mode with ULP RAM RETENTION. \n
 *              ^                      |   RSI_SLEEP_MODE_10 (10): Deep sleep mode without ULP RAM RETENTION. \n
 * 		          ^                      |   In deep sleep mode, module will turn off the SoC and a GPIO or Message based handshake is required before sending commands to the module. \n
 * 		          psp_type               |   Follwing psp_type is defined.
 * 		          ^                      |   RSI_MAX_PSP (0): This psp_type will be used for max power saving. \n
 * 		          ^                      |   RSI_FAST_PSP (1): This psp_type allows module to disable power save for any Tx / Rx packet for monitor interval of time \n
 * 		          ^                      |   (monitor interval can be set by RSI_MONITOR_INTERVAL in rsi_wlan_config.h file, default value is 50 ms). \n
 * 		          ^                      |   If there is no data for monitor interval of time then module will again enable power save. \n
 * 		          ^                      |   RSI_UAPSD (2): This psp_type is used to enable WMM power save.
 * 		          listen interval        |   Used to configure sleep duration in power save. \n
 * @note			  Valid only if BIT (7) in join_feature_bit_map is set. This value is given in time units (1024 microsecond). \n
 *							Used to configure sleep duration in power save and should be less than the listen interval configured by RSI_LISTEN_INTERVAL Macro in join command parameters in rsi_wlan_config.h file. \n
 * @note    		1. psp_type is only valid in psp_mode 1 and 2. \n
 * @note			  2. psp_type UAPSD is applicable only if WMM_PS is enabled in \ref rsi_wlan_config.h file. \n
 * @note			  3. In RSI_MAX_PSP mode, Few access points will not aggregate the packets, when power save is enabled from STA. This may cause the drop in throughputs.
 * @note       4. For the power save mode 3, select RSI_SLEEP_MODE_2 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED in rsi_wlan_config.h file. \n
 * @note       5. For the power save mode 9, select RSI_SLEEP_MODE_8 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED in rsi_wlan_config.h file. \n
 * @note       6. For the deep sleep without ram retention case, select RSI_SLEEP_MODE_10 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED for msg_based or GPIO_BASED for gpio_based. \n
 * @note       7. For LP sleep, select RSI_SLEEP_MODE_2 in psp_mode, select RSI_SELECT_LP_OR_ULP_MODE as RSI_LP_MODE and RSI_HAND_SHAKE_TYPE as MSG_BASED/GPIO_BASED in rsi_wlan_config.h file. \n 
 * @return      0               - Success \n
 *              Non-Zero	Value - Failure \n
 *	            If return value is less than 0 \n
 *              -2              - Invalid parameters \n
 *	            -3              - Command given in wrong state \n
 *	            -4              - Buffer not available to serve the command \n
 *
 */

int32_t rsi_wlan_power_save_with_listen_interval(uint8_t psp_mode, uint8_t psp_type, uint16_t listen_interval)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_POWER_SAVE_LISTEN_INTERVAL_ENTRY, WLAN, LOG_INFO);
  // Get commmon CB pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  // Keep backup of power save profile mode and type
  rsi_common_cb->power_save.wlan_psp_type = psp_type;

  // Keep backup of power save profile mode and type
  rsi_common_cb->power_save.wlan_psp_mode = psp_mode;

  rsi_wlan_cb_non_rom->ps_listen_interval = listen_interval;

  status = rsi_sleep_mode_decision(rsi_common_cb);
  SL_PRINTF(SL_WLAN_POWER_SAVE_LISTEN_INTERVAL_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Set the power save profile in WLAN mode. This is a blocking API.
 * @pre         \ref rsi_wireless_init() API needs to be called before this API. 
 * @param[in]   psp_mode
 * @param[in]   psp_type
 * 	          	parameter              |  Description	
 * 		          :----------------------|:--------------------------------------------------------------------------------------------------
 *              psp_mode               |   Following psp_mode is defined.
 *              ^                      |   Active(0) : In this mode, module is active and power save is disabled. \n
 *              ^                      |   RSI_SLEEP_MODE_1 (1): Connected sleep mode. \n
 *              ^                      |   In this sleep mode, SoC will never turn off, therefore no handshake is required before sending data to the module. \n
 *              ^                      |   RSI_SLEEP_MODE_2 (2): In this sleep mode, SoC will go to LP/ULP (with/without RAM RETENTION) sleep based on the selected value set for RSI_SELECT_LP_OR_ULP_MODE in rsi_wlan_config.h. \n
 *              ^                      |   Therefore handshake is required before sending data to the module \n
 * 		          ^                      |   RSI_SLEEP_MODE_8 (8): Deep sleep mode with ULP RAM RETENTION. \n
 *              ^                      |   RSI_SLEEP_MODE_10 (10): Deep sleep mode without ULP RAM RETENTION. \n
 * 		          ^                      |   In deep sleep mode, module will turn off the SoC and a GPIO or Message based handshake is required before sending commands to the module. \n
 * 		          psp_type               |   Follwing psp_type is defined.
 * 		          ^                      |   RSI_MAX_PSP (0): This psp_type will be used for max power saving. \n
 * 		          ^                      |   RSI_FAST_PSP (1): This psp_type allows module to disable power save for any Tx / Rx packet for monitor interval of time \n
 * 		          ^                      |   (monitor interval can be set by RSI_MONITOR_INTERVAL in rsi_wlan_config.h file, default value is 50 ms). \n
 * 		          ^                      |   If there is no data for monitor interval of time then module will again enable power save. \n
 * 		          ^                      |   RSI_UAPSD (2): This psp_type is used to enable WMM power save.
 *             #### Enhanced max psp ####
 *		         Enhanced max PSP is recommended. This essentially a MAX PSP mode but switches to Fast PSP mode if AP does not deliver data within 20ms for PS-Poll. \n
 *		         To enable this mode, follow procedure below: \n
 *             Add ENABLE_ENHANCED_MAX_PSP (BIT(26)) in RSI_CONFIG_FEATURE_BITMAP \n
 *             Set psp_type to RSI_FAST_PSP (1) \n
 *             Configure Monitor interval by RSI_MONITOR_INTERVAL in rsi_wlan_config.h file. (default value is 50 ms)
 * @note	     1. psp_type is only valid in psp_mode 1 and 2. \n
 * @note	     2. psp_type UAPSD is applicable only if WMM_PS is enabled in rsi_wlan_config.h file. \n
 * @note       3. In RSI_MAX_PSP mode, Few Access points won't aggregate the packets, when power save is enabled from STA. This may cause the drop in throughputs.\n
 * @note       4. For the power save mode 3, select RSI_SLEEP_MODE_2 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED in rsi_wlan_config.h file.\n
 * @note       5. For the power save mode 9, select RSI_SLEEP_MODE_8 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED in rsi_wlan_config.h file.\n
 * @note       6. For the deep sleep without ram retention case, select RSI_SLEEP_MODE_10 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED for msg_based or GPIO_BASED for gpio_based.\n
 * @note       7. For LP sleep, select RSI_SLEEP_MODE_2 in psp_mode, select RSI_SELECT_LP_OR_ULP_MODE as RSI_LP_MODE and RSI_HAND_SHAKE_TYPE as MSG_BASED/GPIO_BASED in rsi_wlan_config.h file.\n 
 * @note	     Powersave handshake option: \n
 *					   - When sleep clock source is configured to '32KHz bypass clock on UULP_VBAT_GPIO_3', \n
 *						 use UULP_VBAT_GPIO_0 for SLEEP_IND_FROM_DEV \n
 *						 set RS9116_SILICON_CHIP_VER in 'RS9116.NB0.WC.GENR.OSI.X.X.X\host\sapis\include\rsi_user.h' to 'CHIP_VER_1P4_AND_ABOVE' \n
 *					   - If not using external clock on UULP_VBAT_GPIO_3' as sleep clock source, \n
 *						 use UULP_VBAT_GPIO_3 for SLEEP_IND_FROM_DEV \n
 *						 set RS9116_SILICON_CHIP_VER in 'RS9116.NB0.WC.GENR.OSI.X.X.X\host\sapis\include\rsi_user.h' to 'CHIP_VER_1P3'. \n
 *             EXT_FEAT_LOW_POWER_MODE is not supported for 1.3 version chipset(CHIP_VER_1P3).
 *            #### Power save modes description ####
 *	        + Power Save Mode 0\n
 *            In this mode, module is active and power save is disabled. It can be configured any time, while the module is configured in Power Save mode 2 or 8.\n
 *          + Power save Mode 1 \n
 *            Once the module is configured to power save mode 1, it wakes up periodically based upon the DTIM interval configured in connected AP. \n
 *            In power mode 1, only the RF of the module is in power save while SOC continues to work normally. \n
 *	       	  This command has to be given only when module is in connected state (with the AP). After having configured the module to power save mode, the Host can issue subsequent commands. \n
 *	       	  In power save mode 1 the module can receive data from host at any point of time but it can send/receive the data to/from remote terminal only when it is awake at DTIM intervals. \n
 *          + Power Save Mode 2 \n
 *            Once the module is configured to power save mode 2, it can be woken up either by the Host or periodically during its sleep-wakeup cycle. \n
 *            Power Save mode 2 is GPIO based. \n
 *            In case of GPIO based mode, whenever host wants to send data to module, it gives wakeup request by asserting UULP GPIO #2. \n
 *		        After wakeup, if the module is ready for data transfer, it sends wakeup indication to host by asserting UULP GPIO #3 or UULP GPIO #0. \n
 *		        Host is required to wait until module gives wakeup indication before sending any data to the module. \n
 *            After the completion of data transfer, host can give sleep permission to module by de-asserting UULP GPIO #2. After recognizing sleep permission from host, \n
 *	       	  module gives confirmation to host by de-asserting UULP GPIO #3 or UULP GPIO #0 and again goes back to its sleep-wakeup cycle. \n
 *            Module can send received packets or responses to host at any instant of time. No handshake is required on Rx path. \n
 *          + Power Save mode 3  \n
 *            Power Mode 3 is message based power save. In Power Mode 3, both radio and SOC of RS9116-WiSeConnect are in power save mode. \n
 *		        This mode is significant when module is in associated state with AP. Module wakes up periodically upon every DTIM and gives wakeup message ("WKP") to host. \n
 *		        Module can not be woken up asynchronously. Every time module intends to go to sleep it sends a sleep request message ("SLP") to the host and expects host to send the acknowledgement message ("ACK"). \n
 *		        Host either send acknowledgement ("ACK") or any other pending message. But once ACK is sent, Host should not send any other message unless next wakeup message from module is received. \n
 *            Module shall not go into complete power-save state, if ACK is not received from host for given sleep message. Module can send received packets or responses to host at any instant of time. \n
 *	       	  No handshake is required on Rx path. \n
 *          + Power Save mode 8  \n
 *		        In Power save mode 8, both RF and SOC of the module are in complete power save mode. This mode is significant only when module is in un-connected state. \n
 *		        Power Save mode 8 can be GPIO based. \n
 *            In case of GPIO based, host can wakeup the module from power save by asserting UULP GPIO #2. \n
 *		        After wakeup, if the module is ready for data transfer, it sends wakeup indication to host by asserting UULP GPIO #3 or UULP GPIO #0. \n
 *            Host is required to wait until module gives wakeup indication before sending any data to the module. \n
 *            After the completion of data transfer, host can give sleep permission to module by de-asserting UULP GPIO #2. After recognizing sleep permission from host,
 *	       	  module gives confirmation to host by de-asserting UULP GPIO #3 or UULP GPIO #0 and again goes back to its sleep-wakeup cycle. \n
 *            Module can send received packets or responses to host at any instant of time. No handshake is required on Rx path. \n
 *          + Power save mode 9  \n
 *		        In Power Mode 9 both Radio and SOC of RS9116-WiSeConnect are in complete power save mode. This mode is significant when module is not connected with any AP. \n
 *		        Once power mode 9 command is given, the module sends ("SLP") request to host and wait for the ("ACK") from host and goes to sleep when ACK is given by host. Timer starts when power save command is issued and it can be configured by host using \ref rsi_wlan_set_sleep_timer API. \n
 *	          If host does not set any sleep time, then the timer is configured for 3sec by default. Upon wakeup module sends a wakeup message to the host and expects host to give ACK before \n
 *		        it goes into next sleep cycle. Host either send ACK or any other messages but once ACK is sent no other packet should be sent before receiving next wakeup message. \n
 *            When ulp_mode_enable is set to '2', after waking up from sleep, the module sends WKP FRM SLEEP message to host when RAM retention is not enabled. \n
 *		        After receiving WKP FRM SLEEP message, host needs to start giving commands from beginning (opermode) as module's state is not retained.
 * @return    0               -  Success \n
 *            Non-Zero	Value - Failure \n
 *	          If return value is less than 0 \n
 *            -2              - Invalid parameters \n
 *	          -3              - Command given in wrong state \n
 *	          -4              - Buffer not available to serve the command \n
 *            If return value is greater than 0 \n
 * 	          0x0021,0x0025,0x002C,0xFFF8,0x0015,0x0026,0x0052
 * @note      Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type)
{
  int32_t status = RSI_SUCCESS;
  status         = rsi_wlan_power_save_with_listen_interval(psp_mode, psp_type, 0);
  SL_PRINTF(SL_WLAN_POWER_SAVE_PROFILE_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief 	    Disable or enable the power save feature. This is a blocking API.
 * @pre      \ref rsi_wlan_power_save_profile() API needs to be called before this API.
 * @param[in]   psp_mode        - Power save mode
 * @param[in]   psp_tye         -  Power save type
 * @return      Non-Zero Value  - Failure \n
 *              0 		          - Success \n
 *
 *
 */
int32_t rsi_wlan_power_save_disable_and_enable(uint8_t psp_mode, uint8_t psp_type)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_POWER_SAVE_DISABLE_ENABLE_ENTRY, WLAN, LOG_INFO);
  status = rsi_wlan_power_save_profile(RSI_ACTIVE, psp_type);
  if (status != RSI_SUCCESS) {
    // Return status if error in sending command occurs
    SL_PRINTF(SL_WLAN_POWER_SAVE_DISABLE_ENABLE_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }
  status = rsi_wlan_power_save_profile(psp_mode, psp_type);
  if (status != RSI_SUCCESS) {
    // Return status if error in sending command occurs
    SL_PRINTF(SL_WLAN_POWER_SAVE_DISABLE_ENABLE_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }
  SL_PRINTF(SL_WLAN_POWER_SAVE_DISABLE_ENABLE_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Start the transmit test. This is a blocking API. \n 
 *             This API is relevant in PER mode.
 * @pre        \ref rsi_wlan_radio_init() API needs to be called before this API.
 * @param[in]  power  - Set TX power in dbm. The valid values are from 2dbm to 18dbm for WiSeConnect module.
 * @note                1. User can configure the maximum power level allowed for the given frequncey in the configured region by providing 127 as power level \n
 * @note                2. User should configure a minimum delay (approx. 10 milliseconds) before and after \ref rsi_transmit_test_start API to observe a stable output at requested dBm level. \n
 * @param[in]  rate   - Set transmit data rate
 * @param[in]  length - Configure length of the TX packet. \n
 *                      The valid values are in the range of 24 to 1500 bytes in the burst mode and range of 24 to 260 bytes in the continuous mode. 
 * @param[in]  mode   - 0- Burst Mode \n
 *                      1- Continuous Mode \n
 *                      2- Continuous wave Mode (non modulation) in DC mode \n
 *                      3- Continuous wave Mode (non modulation) in single tone mode (center frequency -2.5MHz) \n
 *                      4- Continuous wave Mode (non modulation) in single tone mode (center frequency +5MHz)
 * @param[in]  channel - Set the channel number in 2.4 GHz / 5GHz.
 * @note                 1. Before starting Continuous Wave mode, user must start Continuous mode with power and channel values that are intended to be used in Continuous Wave mode i.e. \n
 *                          i.  Start Continuous mode with intended power value and channel values - Pass any valid values for rate and length. \n
 *                          ii. Stop Continuous mode \n 
 *                          iii Start Continuous Wave mode \n
 * @note                 2. If user wants to switch continuous wave mode, first need to stop the per mode and again need to give continous wave mode which user wants to switch. 
 *      ### Data Rates ###       
 *			Data rate(Mbps)	|	Value of rate 
 *			:--------------:|:-------------------:
 *			1		            |	0 
 *			2		            |	2 \n
 *			5.5		          |	4 \n
 *			11		          |	6 \n
 *			6		            |	139 \n
 *			9		            |	143 \n
 *			12		          |	138 \n
 *			18		          |	142 \n
 *			24		          |	137 \n
 *			36		          |	141 \n
 *			48		          |	136 \n
 *			54		          |	140 \n
 *			MCS0		        |	256 \n
 *			MCS1		        |	257 \n
 *			MCS2		        |	258 \n
 *			MCS3		        |	259 \n
 *			MCS4		        |	260 \n
 *			MCS5		        |	261 \n
 *			MCS6		        |	262 \n
 *			MCS7		        |	263 \n			
 *			###The following table maps the channel number to the actual radio frequency in the 2.4 GHz spectrum. ### 
 *			Channel numbers (2.4GHz)|	Center frequencies for 20MHz channel width 
 *			:----------------------:|:-----------------------------------------------:
 *			1			|	2412 
 *			2			|	2417  
 *			3			|	2422 
 *			4			|	2427
 *			5			|	2432 
 *			6			|	2437 
 *			7			|	2442 
 *			8			|	2447 
 *			9			|	2452
 *			10			|	2457  
 *			11			|	2462 
 *			12			|	2467 
 *			13			|	2472 
 *			14			|	2484 
 * @note	To start transmit test in 12,13,14 channels, configure set region parameters in rsi_wlan_config.h \n
 *    ###	The following table maps the channel number to the actual radio frequency in the 5 GHz spectrum for 20MHz channel bandwidth. The channel numbers in 5 GHz range is from 36 to 165. ###
 * 		Channel Numbers(5GHz) |	Center frequencies for 20MHz channel width 
 * 		:--------------------:|:------------------------------------------:
 *		36		      |5180 
 *		40		      |5200 
 *		44		      |5220 
 *		48		      |5240 
 *		52		      |5260 
 *		56	        |5280 
 *		60		      |5300 
 *		64		      |5320 
 *		149		      |5745 
 *		153		      |5765 
 *		157		      |5785 
 *		161		      |5805 
 *		165		      |5825 
 * @return      0 		- Success \n
 *              Non-Zero Value  - Failure \n
 *              If less than zero \n
 *				         -4 - Buffer not available to serve the command \n
 *				      If greater than zero \n
 *				         0x000A, 0x0021, 0x0025, 0x002C
 * @note        Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_transmit_test_start(uint16_t power, uint32_t rate, uint16_t length, uint16_t mode, uint16_t channel)
{
  rsi_pkt_t *pkt;
  rsi_req_tx_test_info_t *tx_test_info;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_TX_TEXT_START_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check whether transmit test mode is selected
  if (wlan_cb->opermode != RSI_WLAN_TRANSMIT_TEST_MODE) {
    SL_PRINTF(SL_WLAN_TX_TEXT_START_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  // WLAN radio init
  status = rsi_wlan_radio_init();

  if (status != RSI_SUCCESS) {
    // Return status if error in sending command occurs
    SL_PRINTF(SL_WLAN_TX_TEXT_START_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_TX_TEXT_START_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Fill TX test info parameters
    tx_test_info = (rsi_req_tx_test_info_t *)pkt->data;

    // Memset the PKT
    memset(&pkt->data, 0, sizeof(rsi_req_tx_test_info_t));

    // enable transmit test
    rsi_uint16_to_2bytes(tx_test_info->enable, 1);

    // Configure transmit power of TX test
    rsi_uint16_to_2bytes(tx_test_info->power, power);

    // Configure transmit rate of TX test
    rsi_uint32_to_4bytes(tx_test_info->rate, rate);

    // Configure packet length of TX test
    rsi_uint16_to_2bytes(tx_test_info->length, length);

    // Configure transmit mode of TX test
    rsi_uint16_to_2bytes(tx_test_info->mode, mode);

    // Configure channel of TX test
    rsi_uint16_to_2bytes(tx_test_info->channel, channel);
    rsi_uint16_to_2bytes(tx_test_info->aggr_enable, RSI_TX_TEST_AGGR_ENABLE);
#ifdef CHIP_9117
    tx_test_info->enable_11ax = RSI_11AX_ENABLE;
    if (tx_test_info->enable_11ax) {
      tx_test_info->coding_type   = RSI_CODING_TYPE;
      tx_test_info->nominal_pe    = RSI_NOMINAL_PE;
      tx_test_info->he_ppdu_type  = RSI_HE_PPDU_TYPE;
      tx_test_info->beam_change   = RSI_BEAM_CHANGE;
      tx_test_info->BW            = RSI_BW;
      tx_test_info->UL_DL         = RSI_UL_DL;
      tx_test_info->STBC          = RSI_STBC;
      tx_test_info->Tx_BF         = RSI_TX_BF;
      tx_test_info->GI_LTF        = RSI_GI_LTF;
      tx_test_info->DCM           = RSI_DCM;
      tx_test_info->NSTS_MIDAMBLE = RSI_NSTS_MIDAMBLE;
      tx_test_info->spatial_reuse = RSI_SPATIAL_REUSE;
      tx_test_info->BSS_color     = RSI_BSS_COLOR;
      //tx_test_info->he_conf_params.HE_SIGA2_RESERVED = RSI_HE_SIGA2_RESERVED;
      rsi_uint16_to_2bytes(tx_test_info->HE_SIGA2_RESERVED, RSI_HE_SIGA2_RESERVED);
      tx_test_info->RU_ALLOCATION = RSI_RU_ALLOCATION;
      tx_test_info->N_HELTF_TOT   = RSI_N_HELTF_TOT;
      tx_test_info->SIGB_DCM      = RSI_SIGB_DCM;
      tx_test_info->SIGB_MCS      = RSI_SIGB_MCS;
      //tx_test_info->he_conf_params.USER_STA_ID = RSI_USER_STA_ID;
      rsi_uint16_to_2bytes(tx_test_info->USER_STA_ID, RSI_USER_STA_ID);
      tx_test_info->USER_IDX               = RSI_USER_IDX;
      tx_test_info->SIGB_COMPRESSION_FIELD = RSI_SIGB_COMPRESSION_FIELD;
    }
#endif
#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TX_TEST_MODE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_TRANSMIT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_TX_TEXT_START_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_TX_TEXT_START_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief     Stops the transmit test. This is a blocking API.
 * @note      This API is relevant in PER mode. 
 * @pre       \ref rsi_wlan_radio_init() API needs to be called before this API.
 * @param[in]   void    
 * @return	0		Successful execution of the command
 * 		Non Zero Value	Failure \n
 * 				if return value is less than 0 \n
 * 				-4: Buffer not available to serve the command \n
 * 				If return value is greater than 0 \n
 * 				0x0021, 0x0025, 0x002C      
 * @note       1. User should configure a minimum delay (approx. 10 milliseconds) before and after \ref rsi_transmit_test_start API to observe a stable output at requested dBm level. \n
 * @note       2. Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 *
 *
 */

int32_t rsi_transmit_test_stop(void)
{
  rsi_pkt_t *pkt;
  rsi_req_tx_test_info_t *tx_test_info;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_TX_TEXT_STOP_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_TX_TEXT_STOP_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_TX_TEXT_STOP_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Fill TX test info parameters
    tx_test_info = (rsi_req_tx_test_info_t *)pkt->data;

    // Memset the PKT
    memset(&pkt->data, 0, sizeof(rsi_req_tx_test_info_t));

    // Configure to stop TX test
    rsi_uint16_to_2bytes(tx_test_info->enable, 0);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TX_TEST_MODE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_TRANSMIT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_TX_TEXT_STOP_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_TX_TEXT_STOP_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief     Get the Transmit (TX) & Receive (RX) packets statistics.When this API is called by the host with  valid channel number,
 *            the module gives the statistics to the host for every 1 second asynchronously. If wlan_receive_stats_response_handler()
 *            is registered through rsi_wlan_register_callbacks(), it's a non blocking, otherwise, a blocking call
 * @pre       \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in] channel         - Valid channel number 2.4GHz or 5GHz          
 * @return    0               - Success
 *            Non-Zero Value  - Failure \n
 *            If return value is less than 0 \n
 *            -4: Buffer not available to serve the command \n
 *            If return value is greater than 0 \n
 *            0x0021, 0x0025, 0x002c, 0x000A
 * @note      Refer to Error Codes section for the description of the above error codes \ref error-codes.
 * 
 */

int32_t rsi_wlan_receive_stats_start(uint16_t channel)
{
  rsi_pkt_t *pkt;
  rsi_req_rx_stats_t *rx_stats;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_RX_STATS_START_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_RX_STATS_START_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_RX_STATS_START_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Fill RX stats  parameters
    rx_stats = (rsi_req_rx_stats_t *)pkt->data;

    // Memset the PKT
    memset(&pkt->data, 0, sizeof(rsi_req_rx_stats_t));

    // Configure to start RX stats
    rsi_uint16_to_2bytes(rx_stats->start, 0);

    // Copy the channel number
    rsi_uint16_to_2bytes(rx_stats->channel, channel);

    if (rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler == NULL) {
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    }
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_RX_STATS, pkt);

    if (rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler == NULL) {
      // Wait on WLAN semaphore
      rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_RX_STATS_RESPONSE_WAIT_TIME);
      // Get WLAN/network command response status
      status = rsi_wlan_get_status();
    }
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_RX_STATS_START_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_RX_STATS_START_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief     Stop the Transmit (TX) & Receive(RX) packets statistics.
 *            if wlan_receive_stats_response_handler() is registered through rsi_wlan_register_callbacks(), it's non blocking, otherwise, a blocking call
 * @pre       \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in] Void
 * @return    0               - Success
 *            Non-Zero Value  - Failure \n
 *            If return value is less than 0 \n
 *            -4: Buffer not available to serve the command \n
 *            If return value is greater than 0 \n
 *            0x0021, 0x0025, 0x002c
 * @note      Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_wlan_receive_stats_stop(void)
{
  rsi_pkt_t *pkt;
  rsi_req_rx_stats_t *rx_stats;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_RX_STATS_STOP_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_RX_STATS_STOP_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_RX_STATS_STOP_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Fill RX stats  parameters
    rx_stats = (rsi_req_rx_stats_t *)pkt->data;

    // Memset the PKT
    memset(&pkt->data, 0, sizeof(rsi_req_rx_stats_t));

    // Configure to stop RX stats
    rsi_uint16_to_2bytes(rx_stats->start, 1);

    if (rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler == NULL) {
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    }
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_RX_STATS, pkt);

    if (rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler == NULL) {
      // Wait on WLAN semaphore
      rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_RX_STATS_RESPONSE_WAIT_TIME);

      // Get WLAN/network command response status
      status = rsi_wlan_get_status();
    }

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_RX_STATS_STOP_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_RX_STATS_STOP_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Start discovery in wi-fi direct mode.This is a non-blocking API.
 *              wlan_wfd_discovery_notify_handler() rsi_wlan_wfd_start_discovery() API is registered through rsi_wlan_register_callbacks(), its non blocking, otherwise blocking call.
 * @pre         \ref rsi_wireless_init() API needs to be called before this API.						
 * 	          	Parameter                                  |  Description
 * 	          	:------------------------------------------|:--------------------------------------------------------------------------------------------------
 *               go_intent                                 |   Determine whether the device is intended to form a GO (group owner) or work as a Wi-Fi Direct Peer node. \n
 *               ^                                         |   Value used in the GO negotiation process, when the module negotiates with another Wi-Fi Direct Node on who would become the Group Owner.\n
 *               ^                                         |   Valid range of values for this parameter is: 0 to 16. Higher the number, higher is the willingness of the module to become a GO. \n
 *               ^                                         |   After the module becomes a GO in Wi-Fi Direct mode, it appears as an Access Point to the client devices. \n
 *               ^                                         |   If the number is between 0 and 15, a GO negotiation takes place. If the value is 16, the module forms an Autonomous GO without negotiating with any other device.\n
 *               device_name                               |   Device name for the module. The maximum length of this field is 32 characters and the remaining bytes are filled with 0x00. \n
 *               ^                                         |   Another Wi-Fi Direct device would see this name when it scans for Wi-Fi Direct nodes.  \n
 *               channel                                   |   Operating channel number. The specified channel is used if the device becomes a GO or Autonomous GO \n
 *               ssid_post_fix                             |   Used to add a postfix to the SSID in Wi-Fi Direct GO mode and Autonomous GO mode.\n
 *               psk                                       |   Passphrase of a maximum length of 63 characters (a null character should be supplied to make it 64 bytes in the structure). \n
 *               ^                                         |   PSK used if the module becomes a GO owner. \n
 *               wlan_wfd_discovery_notify_handler         |   Asynchronous message sent from module to the host when module finds any Wi-Fi Direct node. \n
 *               ^                                         |   Parameters involved are status, buffer. & length
 *               status					                           |   Response status. If status is zero, it means that the wfd device response has some device information
 *               buffer    				                         |   Response buffer.
 *               Length                                    |   Response buffer length \n
 *               wlan_wfd_connection_request_notify_handler|   Asynchronous message sent from module to the host when module receives a connection request from any remote Wi-Fi Direct node.\n
 *               ^                                         |   Parameters involved are status, buffer, & length
 *               status 				                           |   Response status. If status is zero, it means that the connection request has come from some device.
 *               buffer 				                           |   Response buffer
 *               length 				                           |   Response buffer length
 * @return      0  -  Success \n
 *              Non-Zero	Value - Failure \n
 *	            If return value is less than 0 \n	
 *	            -3 - Command given in wrong state \n
 *	            -4 - Buffer not available to serve the command \n
 *              If return value is greater than 0 \n
 * 	            0x001D, 0x0021, 0x002C, 0x0015
 *
 */

int32_t rsi_wlan_wfd_start_discovery(
  uint16_t go_intent,
  int8_t *device_name,
  uint16_t channel,
  int8_t *ssid_post_fix,
  uint8_t *psk,
  void (*wlan_wfd_discovery_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length),
  void (*wlan_wfd_connection_request_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length))
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_ENTRY, WLAN, LOG_INFO);

  rsi_req_configure_p2p_t *config_p2p;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check whether Wi-Fi-Direct mode is selected
  if (wlan_cb->opermode != RSI_WLAN_WIFI_DIRECT_MODE) {
    SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  // WLAN radio init
  status = rsi_wlan_radio_init();

  if (status != RSI_SUCCESS) {
    // Return status if error in sending command occurs
    SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    config_p2p = (rsi_req_configure_p2p_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_configure_p2p_t));

    // Configure go intent
    rsi_uint16_to_2bytes(config_p2p->go_intent, go_intent);

    if (device_name != NULL) {
      // Copy device name
      rsi_strcpy(config_p2p->device_name, device_name);
    }

    // Configure operating channel
    rsi_uint16_to_2bytes(config_p2p->operating_channel, channel);

    if (ssid_post_fix != NULL) {
      // Copy SSID post fix
      rsi_strcpy(config_p2p->ssid_post_fix, ssid_post_fix);
    }

    if (psk != NULL) {
      // Copy PSK
      rsi_strcpy(config_p2p->psk, psk);
    }

    if (wlan_wfd_discovery_notify_handler != NULL) {
      rsi_wlan_cb_non_rom->callback_list.wlan_wfd_discovery_notify_handler = wlan_wfd_discovery_notify_handler;
    }

    if (wlan_wfd_connection_request_notify_handler != NULL) {
      rsi_wlan_cb_non_rom->callback_list.wlan_wfd_connection_request_notify_handler =
        wlan_wfd_connection_request_notify_handler;
    }
    if ((wlan_wfd_discovery_notify_handler == NULL) && (wlan_wfd_connection_request_notify_handler == NULL)) {
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    }
    // Send wi-fi direct discovery command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CONFIGURE_P2P, pkt);

    if ((wlan_wfd_discovery_notify_handler == NULL) && (wlan_wfd_connection_request_notify_handler == NULL)) {
      // Wait on WLAN semaphore
      rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_P2P_RESPONSE_WAIT_TIME);
      // Get WLAN/network command response status
      status = rsi_wlan_get_status();
    }

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       A join_response_handler() API that connects to the specified Wi-Fi-Direct device.This is a non-blocking API.
 *              if join_response_handler() is registered through rsi_wlan_register_callbacks(), it's non blocking, otherwise, a blocking call.
 * @pre         \ref rsi_wlan_wfd_start_discovery() API needs to be called before this API.
 * @param[in]   device_name           -  Device name of the Wi-Fi Direct node to connect. \n
 * @param[in]   join_response_handler -  Called when the response for join has come from the module \n
 * @param[in]   device_name           - Device name of the Wi-Fi Direct node to connect. \n
 * @param[in]   join_response_handler - Called when the response for join has come from the module \n
 *                                       Parameters involved are status, buffer, & length.
 * @param[out]  status		      -	 Response status \n
 *                                        If status is zero, join response is stated as success 
 * @param[out]	buffer		      -	 Response buffer. On successful execution of the command. \n
 *       			      	                   GO_Status (1 byte, hex): 0x47 (ASCII "G") � If the module becomes a Group Owner (GO) after the GO negotiation stage, \n
 *       			       	                   or becomes an Access Point. 0x43 (ASCII "C") � If the module does not become a GO after the GO negotiation stage, or becomes a client (or station). \n
 * @param[out] length		      -  Response buffer length
 * @param[out]  <nodetype> \n
 *              'G' - GO \n
 *              'C' - Client
 * @note        The module gets a default IP of 192.168.100.76 if it becomes a Group Owner or Access Point in case of IPv4. and gets a default IP of 2001:db8:0:1:0:0:0:120 in case of IPv6.
 * @return      0               -  Success \n
 *              Non-Zero	Value - Failure \n
 *	            If return value is less than 0 \n
 *	            -4              - Buffer not available to serve the command \n
 *              If return value is greater than 0 \n
 * 	            0x0014, 0x0009, 0x0003, 0x0021, 0x0012c 0x0015
 *
 */

int32_t rsi_wlan_wfd_connect(int8_t *device_name,
                             void (*join_response_handler)(uint16_t status,
                                                           const uint8_t *buffer,
                                                           const uint16_t length))
{
  rsi_pkt_t *pkt;
  rsi_req_join_t *join;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_WFD_CONNECT_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_WFD_CONNECT_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_WFD_CONNECT_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_join_t));

    join = (rsi_req_join_t *)pkt->data;

    if (device_name != NULL) {
      // Copy SSID and SSID len
      join->ssid_len = rsi_strlen(device_name);

      // Copy Join SSID
      rsi_strcpy(join->ssid, device_name);
    }

    if (join_response_handler != NULL) {
      // Register scan response handler
      rsi_wlan_cb_non_rom->callback_list.wlan_join_response_handler = join_response_handler;
    }

    if (join_response_handler == NULL) {
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    }
    // Send join command to start wps
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);

    if (join_response_handler == NULL) {
      // Wait on WLAN semaphore
      rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_JOIN_RESPONSE_WAIT_TIME);
      // Get WLAN/network command response status
      status = rsi_wlan_get_status();
    }

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_WFD_CONNECT_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_WFD_CONNECT_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Send the raw data in TCP/IP bypass mode. This is a blocking API.
 * @param[in]  buffer - Pointer to the buffer to send
 * @param[in]  length - Length of the buffer to send
 * @return     0               -  Success \n
 *             Non-Zero  Value - Failure \n
 *             If return value is less than 0 \n
 *             -2              - Invalid Parameters \n
 *		         -4              - Buffer not available to serve the command \n
 *             If return value is greater than 0 \n
 *             0x0021,0x002C,0x0025
 *
 *
 */

int32_t rsi_wlan_send_data(uint8_t *buffer, uint32_t length)
{
  int32_t status = RSI_SUCCESS;
  uint8_t *host_desc;
  rsi_pkt_t *pkt;
  SL_PRINTF(SL_WLAN_SEND_DATA_ENTRY, WLAN, LOG_INFO);
  // If buffer is not valid
  if ((buffer == NULL) || (length == 0)) {
    // Return packet allocation failure error
    SL_PRINTF(SL_WLAN_SEND_DATA_INVALID_PARAM, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_INVALID_PARAM;
  }

  // Allocate packet to send data
  pkt = rsi_pkt_alloc(&rsi_driver_cb->wlan_cb->wlan_tx_pool);

  if (pkt == NULL) {
    SL_PRINTF(SL_WLAN_SEND_DATA_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  // Get host descriptor pointer
  host_desc = pkt->desc;

  // Memset host descriptor
  memset(host_desc, 0, RSI_HOST_DESC_LENGTH);

  // Fill host descriptor
  rsi_uint16_to_2bytes(host_desc, (length & 0xFFF));

  // Fill packet type
  host_desc[1] |= (RSI_WLAN_DATA_Q << 4);
  host_desc[2] |= 0x01;

  // Copy data to be sent
  memcpy(pkt->data, buffer, length);

  // Enqueue packet to WLAN TX queue
  rsi_enqueue_pkt(&rsi_driver_cb->wlan_tx_q, pkt);

#ifndef RSI_SEND_SEM_BITMAP
  rsi_driver_cb_non_rom->send_wait_bitmap |= BIT(0);
#endif
  // Set TX packet pending event
  rsi_set_event(RSI_TX_EVENT);

  if (rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->send_data_sem, RSI_SEND_DATA_RESPONSE_WAIT_TIME)
      != RSI_ERROR_NONE) {
    SL_PRINTF(SL_WLAN_SEND_DATA_RESPONSE_TIMEOUT, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_RESPONSE_TIMEOUT;
  }
  status = rsi_wlan_get_status();
  // Return status
  SL_PRINTF(SL_WLAN_SEND_DATA_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}
/// @cond DOCS_9117
/**
 * @fn          int32_t rsi_wlan_twt_config(uint8_t twt_enable, uint8_t twt_flow_id, twt_user_params_t *twt_req_params)
 * @brief       Configures TWT parameters. Enables or disables a TWT session. This is blocking API.
 * @pre         needs to be called after @ref rsi_wireless_init() OPERMODE command.
 * @param[in]   twt_enable - TWT session setup or teardown \n 1 - To setup TWT session with given parameters \n
 *              0 - To teardown TWT session with given flow ID.
 * @param[in]   twt_flow_id - TWT session flow ID. \n 0 - 7 are valid flow IDs. \n 0xFF value is used to teardown all active sessions. 
 * @param[in]   twt_req_params Configurable TWT parameters. Refer \ref twt_user_params_s.
 * @return      0      - Success \n
 *              Non-Zero Value - Failure \n
 * @note       Refer Error Codes section for above \ref error-codes .
 */
int32_t rsi_wlan_twt_config(uint8_t twt_enable, uint8_t twt_flow_id, twt_user_params_t *twt_req_params)
{
  int32_t status         = RSI_SUCCESS;
  rsi_pkt_t *pkt         = NULL;
  rsi_twt_req_t *twt_req = NULL;
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  if (twt_enable == 0) {
    if ((twt_flow_id != 0xFF) && (twt_flow_id > 7)) {
      return RSI_ERROR_INVALID_PARAM;
    }
  } else if (twt_enable == 1) {
    if (twt_flow_id > 7) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->wake_int_exp > 31) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->wake_int_exp_tol > 31) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->wake_duration_unit > 1) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->implicit_twt > 1 || twt_req_params->un_announced_twt > 1 || twt_req_params->triggered_twt > 1
        || twt_req_params->restrict_tx_outside_tsp > 1) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->twt_retry_limit > 15) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->twt_retry_interval < 5) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->req_type > 2) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if ((twt_req_params->twt_channel != 0) || (twt_req_params->twt_protection != 0)) {
      return RSI_ERROR_INVALID_PARAM;
    }
  } else {
    return RSI_ERROR_INVALID_PARAM;
  }

  rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_twt_req_t));

    twt_req = (rsi_twt_req_t *)pkt->data;

    /* copying input params into structure*/
    twt_req->twt_enable  = twt_enable;
    twt_req->twt_flow_id = twt_flow_id;
    if (twt_enable) {
      twt_req->wake_duration      = twt_req_params->wake_duration;
      twt_req->wake_duration_tol  = twt_req_params->wake_duration_tol;
      twt_req->wake_int_exp       = twt_req_params->wake_int_exp;
      twt_req->wake_int_exp_tol   = twt_req_params->wake_int_exp_tol;
      twt_req->wake_duration_unit = twt_req_params->wake_duration;
      rsi_uint16_to_2bytes(twt_req->wake_int_mantissa, twt_req_params->wake_int_mantissa);
      rsi_uint16_to_2bytes(twt_req->wake_int_mantissa_tol, twt_req_params->wake_int_mantissa_tol);
      twt_req->implicit_twt            = twt_req_params->implicit_twt;
      twt_req->un_announced_twt        = twt_req_params->un_announced_twt;
      twt_req->triggered_twt           = twt_req_params->triggered_twt;
      twt_req->negotiation_type        = 0; //Only Individual TWT is supported for now. Not taking input from user.
      twt_req->twt_channel             = twt_req_params->twt_channel;
      twt_req->twt_protection          = twt_req_params->twt_protection;
      twt_req->restrict_tx_outside_tsp = twt_req_params->restrict_tx_outside_tsp;
      twt_req->twt_retry_limit         = twt_req_params->twt_retry_limit;
      twt_req->twt_retry_interval      = twt_req_params->twt_retry_interval;
      twt_req->req_type                = twt_req_params->req_type;
    }
#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send twt command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TWT_PARAMS, pkt);
    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_TWT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
  }
  // Return status if error in sending command occurs
  return status;
}
/// @endcond
/*==============================================*/
/**
 * @brief       Send a ping request to the target IP address.
 *              If wlan_ping_response_handler() is registered through rsi_wlan_register_callbacks(), it's non blocking, otherwise, a blocking call
 * @pre           \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]   flags                      - BIT(0) RSI_IPV6 Set this bit to enable IPv6; by default, it is configured to IPv4
 * @param[in]   ip_address                 - Target IP address \n
 *						                              IPv4 address  4 Bytes hexa-decimal, \n
 *   					                              IPv6 address  16 Bytes hexa-decimal
 * @param[in]   size                       - Ping data size to send. Maximum supported is 300 bytes.
 * @param[in]   wlan_ping_response_handler - Called when ping response has been received from the module. \n
 *                                          Parameters involved are status, buffer, & length.
 * @param[out]	status			   - Response status
 * @param[out]  buffer                     - Response buffer
 * @param[out]  length                     - Length of the response buffer
 * @return      0                          -  Success \n
 *              Non-Zero	Value            - Failure \n
 *	            If return value is less than 0 \n
 *              -2                         - Invalid parameters \n
 *	            -4                         - Buffer not available to serve the command \n
 *              If return value is greater than 0 \n
 * 	            0x0015,0xBB21,0xBB4B,0xBB55
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 *
 *
 */

int32_t rsi_wlan_ping_async(uint8_t flags,
                            uint8_t *ip_address,
                            uint16_t size,
                            void (*wlan_ping_response_handler)(uint16_t status,
                                                               const uint8_t *buffer,
                                                               const uint16_t length))
{
  int32_t status = RSI_SUCCESS;
  rsi_pkt_t *pkt;
  rsi_req_ping_t *ping;
  SL_PRINTF(SL_WLAN_PING_ASYNC_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
  if (wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_PING_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  // If IP address is not valid
  if ((ip_address == NULL) || (size == 0)) {
    // Return packet allocation failure error
    SL_PRINTF(SL_WLAN_PING_ASYNC_INVALID_PARAM, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_PING_ASYNC_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_ping_t));

    ping = (rsi_req_ping_t *)pkt->data;

    if (flags & RSI_IPV6) {
      // Copy IP version
      rsi_uint16_to_2bytes(ping->ip_version, (uint16_t)RSI_IP_VERSION_6);

      // Copy IPv6 address
      memcpy(ping->ping_address.ipv6_address, ip_address, RSI_IPV6_ADDRESS_LENGTH);
    } else {
      // Copy IP version
      rsi_uint16_to_2bytes(ping->ip_version, (uint16_t)RSI_IP_VERSION_4);

      // Copy IPv4 address
      memcpy(ping->ping_address.ipv4_address, ip_address, RSI_IPV4_ADDRESS_LENGTH);
    }

    // Copy ping size
    rsi_uint16_to_2bytes(ping->ping_size, size);

    ping->timeout = RSI_PING_REQ_TIMEOUT;

    // Register ping response handler
    if (wlan_ping_response_handler != NULL) {
      rsi_wlan_cb_non_rom->callback_list.wlan_ping_response_handler = wlan_ping_response_handler;
      rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending |= PING_RESPONSE_PENDING;
    }

    if (wlan_ping_response_handler == NULL) {
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    }
    // Send ping command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_PING_PACKET, pkt);

    if (wlan_ping_response_handler == NULL) {
      // Wait on WLAN semaphore
      rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_PING_RESPONSE_WAIT_TIME);
      // Get WLAN/network command response status
      status = rsi_wlan_get_nwk_status();
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
    }
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_PING_ASYNC_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_PING_ASYNC_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Register auto-configuration response handler.. This is a non-blocking API.
 * @param[in]  rsi_auto_config_rsp_handler - Pointer to rsi_auto_config_rsp_handler
 * @param[out] status			   - Response status, 0 if success else failure.
 * @param[out] state			   - BIT(1) RSI_AUTO_CONFIG_FAILED 
 *                             BIT(2) RSI_AUTO_CONFIG_GOING_ON 
 *                             BIT(3) RSI_AUTO_CONFIG_DONE 
 * @return     Void
 */

void rsi_register_auto_config_rsp_handler(void (*rsi_auto_config_rsp_handler)(uint16_t status, uint8_t state))
{

  // Register callback handler
  SL_PRINTF(SL_WLAN_REGISTER_AUTO_CONFIG_RSP_HANDLER_EXIT, WLAN, LOG_INFO);
  rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler = rsi_auto_config_rsp_handler;
}

/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Add profile for auto configuration. This is a blocking API.
 * @pre        \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]  type - profile type. Supported profile types are: \n
 *  							1.RSI_WLAN_PROFILE_AP, \n
 *							2.RSI_WLAN_PROFILE_CLIENT, \n
 *							3.RSI_WLAN_PROFILE_EAP, \n
 *							4.RSI_WLAN_PROFILE_P2P, \n
 *							5.RSI_WLAN_PROFILE_ALL
 * @param[in]  profile - Pointer to config profile and profile structure \n
 * 			ap_profile, \n
 * 			eap_client_profile_t, \n
 * 			client_profile_t, \n
 * 			p2p_profile_t, \n
 * 			rsi_config_profile_t
 * @return      0  -  Success \n
 *              Non-Zero Value - Failure \n
 *				-4 - Buffer not availableto serve the command
 *
 */

int32_t rsi_wlan_add_profile(uint32_t type, uint8_t *profile)
{
  int32_t status = RSI_SUCCESS;
  rsi_pkt_t *pkt;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  rsi_config_profile_t *config_profile;
  SL_PRINTF(SL_WLAN_ADD_PROFILE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_ADD_PROFILE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_ADD_PROFILE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_config_profile_t));

    config_profile = (rsi_config_profile_t *)pkt->data;

    // Fill profile type
    rsi_uint32_to_4bytes(config_profile->profile_type, type);

    // Fill WLAN profile
    if (type == RSI_WLAN_PROFILE_AP) {
      memcpy(&config_profile->wlan_profile_struct.ap_profile, (uint8_t *)profile, sizeof(ap_profile_t));
    } else if (type == RSI_WLAN_PROFILE_CLIENT) {
      memcpy(&config_profile->wlan_profile_struct.client_profile, (uint8_t *)profile, sizeof(client_profile_t));
    } else if (type == RSI_WLAN_PROFILE_EAP) {
      memcpy(&config_profile->wlan_profile_struct.eap_client_profile, (uint8_t *)profile, sizeof(eap_client_profile_t));
    } else if (type == RSI_WLAN_PROFILE_P2P) {
      memcpy(&config_profile->wlan_profile_struct.p2p_profile, (uint8_t *)profile, sizeof(p2p_profile_t));
    } else {
      // No code here
    }

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_config_profile_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send set profile command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_PROFILE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SET_PROFILE_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_ADD_PROFILE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_ADD_PROFILE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Get the current WLAN state.. This is a non-blocking API.
 * @param[in]  Void
 * @return     Return the current WLAN state. \n
               WLAN states are as follows: \n
               RSI_WLAN_STATE_NONE = 0, \n
               RSI_WLAN_STATE_OPERMODE_DONE, \n
               RSI_WLAN_STATE_BAND_DONE, \n
               RSI_WLAN_STATE_INIT_DONE, \n
               RSI_WLAN_STATE_SCAN_DONE, \n
               RSI_WLAN_STATE_CONNECTED, \n
               RSI_WLAN_STATE_IP_CONFIG_DONE, \n
               RSI_WLAN_STATE_IPV6_CONFIG_DONE, \n
               RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON, \n
               RSI_WLAN_STATE_AUTO_CONFIG_DONE, \n
               RSI_WLAN_STATE_AUTO_CONFIG_FAILED
 *	       
 */

uint8_t rsi_wlan_get_state(void)
{
  // Return WLAN state
  SL_PRINTF(SL_WLAN_GET_STATE_EXIT, WLAN, LOG_INFO);
  return rsi_driver_cb->wlan_cb->state;
}

/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Get the stored config profile. This is a blocking API.
 * @pre  	     \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]  type - Config profile type. \n
 *             Supported profile types are as follows: \n
 *  	    	   RSI_WLAN_PROFILE_AP, \n
 *		         RSI_WLAN_PROFILE_CLIENT, \n
 *		         RSI_WLAN_PROFILE_EAP, \n 
 *		         RSI_WLAN_PROFILE_P2P, \n 
 *		         RSI_WLAN_PROFILE_ALL 
 * @param[in]  profile_rsp    - Config profile response in the form of below structure: \n
 *             ap_profile     - eap_client_profile_t, client_profile_t, p2p_profile_t, rsi_config_profile_t
 * @param[in]  length         - Length of the config profile response
 * @return     0              -  Success \n
 *             Non-Zero Value - Failure \n
 *             If return value is less than 0
 *		         -4             - Buffer not available to serve the command
 *
 */

int32_t rsi_wlan_get_profile(uint32_t type, rsi_config_profile_t *profile_rsp, uint16_t length)
{
  int32_t status                 = RSI_SUCCESS;
  rsi_pkt_t *pkt                 = NULL;
  rsi_profile_req_t *profile_req = NULL;
  SL_PRINTF(SL_WLAN_GET_PROFILE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_GET_PROFILE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_GET_PROFILE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_profile_req_t));

    profile_req = (rsi_profile_req_t *)pkt->data;

    // Fill profile type
    rsi_uint32_to_4bytes(profile_req->profile_type, type);

    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)profile_rsp;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send get profile command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GET_PROFILE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_GET_PROFILE_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_GET_PROFILE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_GET_PROFILE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Fill the config profile based on the profile type.. This is a non-blocking API.
 * @pre          \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]  Type           - Profile type
 * @param[in]  Profile_buffer - Pointer to profile buffer
 * @return      profile_buffer
 *
 */
/// @private

uint8_t *rsi_fill_config_profile(uint32_t type, uint8_t *profile_buffer)
{
  SL_PRINTF(SL_WLAN_FILL_CONFIG_PROFILE_ENTRY, WLAN, LOG_INFO);
  int i, j = 0;
  uint32_t ip_addr                             = 0;
  uint32_t mask                                = 0;
  uint32_t gw                                  = 0;
  eap_client_profile_t *eap_client_profile_ptr = NULL;
  network_profile_t *nwk_profile_ptr           = NULL;
  feature_frame_params_t *feature_frame_ptr    = NULL;
  client_profile_t *client_profile_ptr         = NULL;
  ap_profile_t *ap_profile_ptr                 = NULL;
  p2p_profile_t *p2p_profile_ptr               = NULL;

  if (type == RSI_WLAN_PROFILE_AP) {
    ip_addr = RSI_CONFIG_AP_IP_ADDRESS;
    mask    = RSI_CONFIG_AP_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_AP_GATEWAY_ADDRESS;

    ap_profile_ptr = (ap_profile_t *)profile_buffer;

    nwk_profile_ptr = &ap_profile_ptr->network_profile;

    feature_frame_ptr = &ap_profile_ptr->feature_frame_params_s;

    memset(ap_profile_ptr, 0, sizeof(ap_profile_t));

    // Fill AP WLAN feature bitmap
    rsi_uint32_to_4bytes(ap_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_AP_WLAN_FEAT_BIT_MAP);

    // Fill AP TCP_IP feature bitmap
    rsi_uint32_to_4bytes(ap_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_AP_TCP_IP_FEAT_BIT_MAP);

    // Fill AP custom feature bitmap
    rsi_uint32_to_4bytes(ap_profile_ptr->custom_feature_bit_map, RSI_CONFIG_AP_CUSTOM_FEAT_BIT_MAP);

    // Fill data rate
    ap_profile_ptr->data_rate = RSI_CONFIG_AP_DATA_RATE;

    // Fill TX power
    ap_profile_ptr->tx_power = RSI_CONFIG_AP_TX_POWER;

    // Fill AP band
    ap_profile_ptr->band = RSI_CONFIG_AP_BAND;

    // Fill AP channel number
    rsi_uint16_to_2bytes(ap_profile_ptr->channel, RSI_CONFIG_AP_CHANNEL);

    // Fill AP SSID
    rsi_strcpy(ap_profile_ptr->ssid, RSI_CONFIG_AP_SSID);

    // Fill AP security type
    ap_profile_ptr->security_type = RSI_CONFIG_AP_SECURITY_TYPE;

    // Fill AP encryption type
    ap_profile_ptr->encryption_type = RSI_CONFIG_AP_ENCRYPTION_TYPE;

    // Fill AP PSK
    if (RSI_CONFIG_AP_PSK)
      rsi_strcpy(ap_profile_ptr->psk, RSI_CONFIG_AP_PSK);

    // Fill beacon interval
    rsi_uint16_to_2bytes(ap_profile_ptr->beacon_interval, RSI_CONFIG_AP_BEACON_INTERVAL);

    // Fill DTIM period
    rsi_uint16_to_2bytes(ap_profile_ptr->dtim_period, RSI_CONFIG_AP_DTIM);

    // Fill AP keep alive time
    ap_profile_ptr->keep_alive_type = RSI_CONFIG_AP_KEEP_ALIVE_TYPE;

    // Fill AP keep alive counter
    ap_profile_ptr->keep_alive_counter = RSI_CONFIG_AP_KEEP_ALIVE_COUNTER;

    // Fill AP max number of stations
    rsi_uint16_to_2bytes(ap_profile_ptr->max_no_sta, RSI_CONFIG_AP_MAX_STATIONS_COUNT);

    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_AP_TCP_STACK_USED;

    if (nwk_profile_ptr->tcp_stack_used == 1) {
      nwk_profile_ptr->dhcp_enable = 0;

      // Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      // Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      // Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      // Not supported
    }
    // Fill feature frame params
    feature_frame_ptr->pll_mode      = PLL_MODE;
    feature_frame_ptr->rf_type       = RF_TYPE;
    feature_frame_ptr->wireless_mode = WIRELESS_MODE;
    feature_frame_ptr->enable_ppp    = ENABLE_PPP;
    feature_frame_ptr->afe_type      = AFE_TYPE;
    rsi_uint32_to_4bytes(feature_frame_ptr->feature_enables, FEATURE_ENABLES);

  } else if (type == RSI_WLAN_PROFILE_CLIENT) {
    ip_addr = RSI_CONFIG_CLIENT_IP_ADDRESS;
    mask    = RSI_CONFIG_CLIENT_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_CLIENT_GATEWAY_ADDRESS;

    client_profile_ptr = (client_profile_t *)profile_buffer;

    nwk_profile_ptr = &client_profile_ptr->network_profile;

    feature_frame_ptr = &client_profile_ptr->feature_frame_params_s;

    memset(client_profile_ptr, 0, sizeof(client_profile_t));

    // Fill WLAN feature bitmap
    rsi_uint32_to_4bytes(client_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_CLIENT_WLAN_FEAT_BIT_MAP);

    // Fill TCP_IP feature bitmap
    rsi_uint32_to_4bytes(client_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_CLIENT_TCP_IP_FEAT_BIT_MAP);

    // Fill custom feature bitmap
    rsi_uint32_to_4bytes(client_profile_ptr->custom_feature_bit_map, RSI_CONFIG_CLIENT_CUSTOM_FEAT_BIT_MAP);

    // Fill listen interval
    rsi_uint32_to_4bytes(client_profile_ptr->listen_interval, RSI_CONFIG_CLIENT_LISTEN_INTERVAL);

    // Fill data rate
    client_profile_ptr->data_rate = RSI_CONFIG_CLIENT_DATA_RATE;

    // Fill TX power
    client_profile_ptr->tx_power = RSI_CONFIG_CLIENT_TX_POWER;

    // Fill AP band
    client_profile_ptr->band = RSI_CONFIG_CLIENT_BAND;

    // Fill Client SSID
    rsi_strcpy(client_profile_ptr->ssid, RSI_CONFIG_CLIENT_SSID);

    // Fill AP SSID length
    client_profile_ptr->ssid_len = strlen(RSI_CONFIG_CLIENT_SSID);

    // Fill scan feature bitmap
    client_profile_ptr->scan_feature_bitmap = RSI_CONFIG_CLIENT_SCAN_FEAT_BITMAP;

    // Fill client channel number
    rsi_uint16_to_2bytes(client_profile_ptr->channel, RSI_CONFIG_CLIENT_CHANNEL);

    // Scan channel bitmap magic code
    rsi_uint16_to_2bytes(client_profile_ptr->scan_chan_bitmap_magic_code, RSI_CONFIG_CLIENT_MAGIC_CODE);

    // Scan channel bitmap for 2_4 Ghz
    rsi_uint32_to_4bytes(client_profile_ptr->scan_chan_bitmap_2_4_ghz, RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_2_4_GHZ);

    // Scan channel bitmap for 5_0 Ghz
    rsi_uint32_to_4bytes(client_profile_ptr->scan_chan_bitmap_5_0_ghz, RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_5_0_GHZ);

    // Fill client security type
    client_profile_ptr->security_type = RSI_CONFIG_CLIENT_SECURITY_TYPE;

    // Fill AP encryption type
    client_profile_ptr->encryption_type = RSI_CONFIG_CLIENT_ENCRYPTION_TYPE;

    // Fill client PSK
    if (RSI_CONFIG_CLIENT_PSK)
      rsi_strcpy(client_profile_ptr->psk, RSI_CONFIG_CLIENT_PSK);

    // Fill client PMK
    if (strlen(RSI_CONFIG_CLIENT_PMK)) {
      for (i = 0, j = 0; i < 64 && j < 32; i += 2, j++) {
        if (RSI_CONFIG_CLIENT_PMK[i] && RSI_CONFIG_CLIENT_PMK[i + 1]) {
          client_profile_ptr->pmk[j] = ((uint16_t)convert_lower_case_to_upper_case(RSI_CONFIG_CLIENT_PMK[i])) * 16;
          client_profile_ptr->pmk[j] += convert_lower_case_to_upper_case(RSI_CONFIG_CLIENT_PMK[i + 1]);
        } else {
          client_profile_ptr->pmk[j] = 0;
        }
      }
    }

    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_CLIENT_TCP_STACK_USED;

    if (nwk_profile_ptr->tcp_stack_used == 1) {
      nwk_profile_ptr->dhcp_enable = RSI_CONFIG_CLIENT_DHCP_MODE;

      // Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      // Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      // Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      // Not supported
    }

    // Fill feature frame params
    feature_frame_ptr->pll_mode      = PLL_MODE;
    feature_frame_ptr->rf_type       = RF_TYPE;
    feature_frame_ptr->wireless_mode = WIRELESS_MODE;
    feature_frame_ptr->enable_ppp    = ENABLE_PPP;
    feature_frame_ptr->afe_type      = AFE_TYPE;
    rsi_uint32_to_4bytes(feature_frame_ptr->feature_enables, FEATURE_ENABLES);

  } else if (type == RSI_WLAN_PROFILE_EAP) {
    ip_addr = RSI_CONFIG_EAP_IP_ADDRESS;
    mask    = RSI_CONFIG_EAP_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_EAP_GATEWAY_ADDRESS;

    eap_client_profile_ptr = (eap_client_profile_t *)profile_buffer;

    nwk_profile_ptr = &eap_client_profile_ptr->network_profile;

    feature_frame_ptr = &eap_client_profile_ptr->feature_frame_params_s;

    memset(eap_client_profile_ptr, 0, sizeof(eap_client_profile_t));

    // Fill WLAN feature bitmap
    rsi_uint32_to_4bytes(eap_client_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_EAP_WLAN_FEAT_BIT_MAP);

    // Fill TCP_IP feature bitmap
    rsi_uint32_to_4bytes(eap_client_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_EAP_TCP_IP_FEAT_BIT_MAP);

    // Fill custom feature bitmap
    rsi_uint32_to_4bytes(eap_client_profile_ptr->custom_feature_bit_map, RSI_CONFIG_EAP_CUSTOM_FEAT_BIT_MAP);

    // Fill data rate
    eap_client_profile_ptr->data_rate = RSI_CONFIG_EAP_DATA_RATE;

    // Fill TX power
    eap_client_profile_ptr->tx_power = RSI_CONFIG_EAP_TX_POWER;

    // Fill band
    eap_client_profile_ptr->band = RSI_CONFIG_EAP_BAND;

    // Fill Client SSID
    rsi_strcpy(eap_client_profile_ptr->ssid, RSI_CONFIG_EAP_SSID);

    // Fill AP SSID length
    eap_client_profile_ptr->ssid_len = strlen(RSI_CONFIG_EAP_SSID);

    // Fill scan feature bitmap
    eap_client_profile_ptr->scan_feature_bitmap = RSI_CONFIG_EAP_SCAN_FEAT_BITMAP;

    // Fill security type
    eap_client_profile_ptr->security_type = RSI_CONFIG_EAP_SECURITY_TYPE;

    // Fill client channel number
    rsi_uint16_to_2bytes(eap_client_profile_ptr->channel, RSI_CONFIG_EAP_CHANNEL);

    // Scan channel bitmap magic code
    rsi_uint16_to_2bytes(eap_client_profile_ptr->scan_chan_bitmap_magic_code, RSI_CONFIG_EAP_CHAN_MAGIC_CODE);

    // Scan channel bitmap for 2_4 Ghz
    rsi_uint32_to_4bytes(eap_client_profile_ptr->scan_chan_bitmap_2_4_ghz, RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_2_4_GHZ);

    // Scan channel bitmap for 5_0 Ghz
    rsi_uint32_to_4bytes(eap_client_profile_ptr->scan_chan_bitmap_5_0_ghz, RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_5_0_GHZ);

    // Fill EAP method
    rsi_strcpy(eap_client_profile_ptr->eap_method, RSI_CONFIG_EAP_METHOD);

    // Fill inner method
    rsi_strcpy(eap_client_profile_ptr->inner_method, RSI_CONFIG_EAP_INNER_METHOD);

    // Fill user identity
    rsi_strcpy(eap_client_profile_ptr->user_identity, RSI_CONFIG_EAP_USER_IDENTITY);

    // Fill password
    rsi_strcpy(eap_client_profile_ptr->passwd, RSI_CONFIG_EAP_PASSWORD);

    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_EAP_TCP_STACK_USED;

    if (nwk_profile_ptr->tcp_stack_used == 1) {
      nwk_profile_ptr->dhcp_enable = RSI_CONFIG_EAP_DHCP_MODE;

      // Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      // Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      // Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      // Not supported
    }

    // Fill feature frame params
    feature_frame_ptr->pll_mode      = PLL_MODE;
    feature_frame_ptr->rf_type       = RF_TYPE;
    feature_frame_ptr->wireless_mode = WIRELESS_MODE;
    feature_frame_ptr->enable_ppp    = ENABLE_PPP;
    feature_frame_ptr->afe_type      = AFE_TYPE;
    rsi_uint32_to_4bytes(feature_frame_ptr->feature_enables, FEATURE_ENABLES);

  } else if (type == RSI_WLAN_PROFILE_P2P) {
    ip_addr = RSI_CONFIG_P2P_IP_ADDRESS;
    mask    = RSI_CONFIG_P2P_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_P2P_GATEWAY_ADDRESS;

    p2p_profile_ptr = (p2p_profile_t *)profile_buffer;

    nwk_profile_ptr = &p2p_profile_ptr->network_profile;

    // feature_frame_params_t *feature_frame_ptr = NULL;

    feature_frame_ptr = &p2p_profile_ptr->feature_frame_params_s;

    memset(p2p_profile_ptr, 0, sizeof(p2p_profile_t));

    // Fill WLAN feature bitmap
    rsi_uint32_to_4bytes(p2p_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_P2P_WLAN_FEAT_BIT_MAP);

    // Fill TCP_IP feature bitmap
    rsi_uint32_to_4bytes(p2p_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_P2P_TCP_IP_FEAT_BIT_MAP);

    // Fill custom feature bitmap
    rsi_uint32_to_4bytes(p2p_profile_ptr->custom_feature_bit_map, RSI_CONFIG_P2P_CUSTOM_FEAT_BIT_MAP);

    // Fill Join SSID
    rsi_strcpy(p2p_profile_ptr->join_ssid, RSI_CONFIG_P2P_JOIN_SSID);

    // Fill data rate
    p2p_profile_ptr->data_rate = RSI_CONFIG_CLIENT_DATA_RATE;

    // Fill TX power
    p2p_profile_ptr->tx_power = RSI_CONFIG_CLIENT_TX_POWER;

    // Fill AP band
    p2p_profile_ptr->band = RSI_CONFIG_CLIENT_BAND;

    // Fill GO intent
    rsi_uint16_to_2bytes(p2p_profile_ptr->go_intent, RSI_CONFIG_P2P_GO_INTNET);

    // Fill device name
    rsi_strcpy(p2p_profile_ptr->device_name, RSI_CONFIG_P2P_DEVICE_NAME);

    // Fill operating channel
    rsi_uint16_to_2bytes(p2p_profile_ptr->operating_channel, RSI_CONFIG_P2P_OPERATING_CHANNEL);

    // Fill SSID postfix
    rsi_strcpy(p2p_profile_ptr->ssid_postfix, RSI_CONFIG_P2P_SSID_POSTFIX);

    // Fill PSK key
    rsi_strcpy(p2p_profile_ptr->psk_key, RSI_CONFIG_P2P_PSK_KEY);

    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_P2P_TCP_STACK_USED;

    if (nwk_profile_ptr->tcp_stack_used == 1) {
      nwk_profile_ptr->dhcp_enable = RSI_CONFIG_P2P_DHCP_MODE;

      // Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      // Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      // Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      // Not supported
    }

    // Fill feature frame params
    feature_frame_ptr->pll_mode      = PLL_MODE;
    feature_frame_ptr->rf_type       = RF_TYPE;
    feature_frame_ptr->wireless_mode = WIRELESS_MODE;
    feature_frame_ptr->enable_ppp    = ENABLE_PPP;
    feature_frame_ptr->afe_type      = AFE_TYPE;
    rsi_uint32_to_4bytes(feature_frame_ptr->feature_enables, FEATURE_ENABLES);

  } else {
    profile_buffer = NULL;
  }
  SL_PRINTF(SL_WLAN_FILL_CONFIG_PROFILE_EXIT, WLAN, LOG_INFO);
  return profile_buffer;
}

/*==============================================*/
/**
 * @note        This API is not supported in current release.
 * @brief       Delete stored configuration based on profile type. This is a blocking API.
 * @pre         \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]   type           - Profile type
 * @return    	0              - Success \n
 *              Non-Zero Value - Failure \n
 *              If return value is less than 0
 *	            -4             - Buffer not available to serve the command
 */

int32_t rsi_wlan_delete_profile(uint32_t type)
{
  int32_t status                 = RSI_SUCCESS;
  rsi_pkt_t *pkt                 = NULL;
  rsi_profile_req_t *profile_req = NULL;
  SL_PRINTF(SL_WLAN_DELETE_PROFILE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_DELETE_PROFILE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_DELETE_PROFILE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_profile_req_t));

    profile_req = (rsi_profile_req_t *)pkt->data;

    // Fill profile type
    rsi_uint32_to_4bytes(profile_req->profile_type, type);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send delete profile command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DELETE_PROFILE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_DELETE_PROFILE_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_DELETE_PROFILE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_DELETE_PROFILE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief     Enable or disable auto-config with respect to profile. This is a blocking API.
 * @pre  \ref rsi_wlan_set() API needs to be called before this API.
 * @param[in] enable - 0 - Disable the auto configuration feature \n
 *                     1 - Enable the auto configuration feature on power up \n
 *                     2 - Enable the auto configuration for profile based.
 * @note	    Currently Profile based feature is not supported.
 * @param[in] type   - 0
 * <!-- @param[in] type   - Profile type \n
 * @note	    Possible profile types: \n
 *		        // Client profile \n
 *		        #define RSI_WLAN_PROFILE_CLIENT 0 \n
 *		        // P2P profile \n
 *		        #define RSI_WLAN_PROFILE_P2P 1 \n
 *		        // EAP profile \n
 *		        #define RSI_WLAN_PROFILE_EAP 2 \n
 *		        // AP profile \n
 *		        #define RSI_WLAN_PROFILE_AP 6 \n
 *		        // All profiles \n
 *		        #define RSI_WLAN_PROFILE_ALL 0xF -->
 * @return   	0              -  Success \n
 *            Non-Zero Value - Failure \n
 *		        -4             - Buffer not available to serve the command
 * @note      If user tries to give any other command during autojoin, then user gets error 0x002C. \n
 *            To avoid this, user have to disable auto_join feature and give other commands.
 * @note      The parameters of the following APIs are saved when rsi_wlan_enable_auto_config() is called: \n
 *            rsi_wireless_init(), rsi_wlan_scan(), rsi_wlan_scan_with_bitmap_options(), rsi_wlan_connect(), \n
 *            rsi_config_ipaddress(), rsi_wireless_antenna(), rsi_wlan_bgscan_profile(), rsi_radio_caps(), rsi_wlan_ap_start
 *
 *
 */

int32_t rsi_wlan_enable_auto_config(uint8_t enable, uint32_t type)
{
  int32_t status                              = RSI_SUCCESS;
  rsi_pkt_t *pkt                              = NULL;
  rsi_auto_config_enable_t *config_enable_req = NULL;
  SL_PRINTF(SL_WLAN_ENABLE_AUTO_CONFIG_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_ENABLE_AUTO_CONFIG_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_ENABLE_AUTO_CONFIG_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_auto_config_enable_t));

    config_enable_req = (rsi_auto_config_enable_t *)pkt->data;

    // Fill config profile enable/disable
    config_enable_req->config_enable = enable;

    // Fill profile type
    rsi_uint32_to_4bytes(config_enable_req->profile_type, type);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send delete profile command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_AUTO_CONFIG_ENABLE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_AUTO_CONFIG_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_ENABLE_AUTO_CONFIG_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_ENABLE_AUTO_CONFIG_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief   	  Generate PMK if PSK and SSID are provided. This is a blocking API.
 * @pre  		    \ref rsi_wlan_connect() API needs to be called before this API.
 * @param[in]   type   - Possible values of this field are 1, 2, and 3, but we only pass 3 for generation of PMK.
 * @param[in]   psk    - Expected parameters are pre-shared key(PSK) of the access point
 * @param[in]   ssid   - Contain the SSID of the access point, this field will be valid only if TYPE value is 3.
 * @param[in]   pmk    - PMK array
 * @param[in]   length - Length of PMK array
 * @param[out]  32-byte PMK
 * @return	    0              - Successful execution of the command. If TYPE value is 3. \n
 *              Non-Zero Value - Failure \n
 *              If return value is greater than 0 \n
 *              0x0021, 0x0025,0x0026,0x0028,0x002C,0x0039,0x003a, 0x003b
 * @note        Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_wlan_pmk_generate(int8_t type, int8_t *psk, int8_t *ssid, uint8_t *pmk, uint16_t length)
{
  int32_t status         = RSI_SUCCESS;
  rsi_pkt_t *pkt         = NULL;
  rsi_req_psk_t *psk_ptr = NULL;
  SL_PRINTF(SL_WLAN_PMK_GENERATE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_PMK_GENERATE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_PMK_GENERATE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    psk_ptr = (rsi_req_psk_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_psk_t));
    //
    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)pmk;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    // Assign type into structure member
    psk_ptr->type = type;

    // Copy PSK given by user into structure member
    rsi_strcpy(psk_ptr->psk_or_pmk, psk);

    // Copy SSID given by user into structure member
    rsi_strcpy(psk_ptr->ap_ssid, ssid);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send PSK command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HOST_PSK, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_PSK_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_PMK_GENERATE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_PMK_GENERATE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @fn       int32_t rsi_wlan_11ax_config(void)
 * @brief    Configure the 11ax params. This is a blocking API.
 * @pre    	\ref rsi_wlan_11ax_config() API needs to be called after opermode command only
 * @return 
 * 		Non-Zero Value  - Failure \n
 *              0 		- Success \n
 *              -2 - Invalid parameters \n
 *		-3 - Command given in wrong state \n
 *		-4 - Buffer not availableto serve the command \n
 *		If return value is greater than zero : 0x0021
 * @note        Refer to Error Codes section for the description of the above error codes  \ref SP16
 * 		Parameters given here are used internally by the API
 *
 *
 */
int32_t rsi_wlan_11ax_config(void)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
  if (wlan_cb->state >= RSI_WLAN_STATE_INIT_DONE) {
    // Change WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
    // Return command given in wrong state error
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  // Allocate command buffer from WLAN pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
  // If allocation of packet fails
  if (pkt == NULL) {
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
    // Return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
#ifndef RSI_WLAN_SEM_BITMAP
  rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
  // Send antenna select command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_11AX_PARAMS, pkt);

  // Wait on WLAN semaphore
  rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_11AX_WAIT_TIME);

  // Get WLAN/network command response status
  status = rsi_wlan_get_status();
  // Return status
  return status;
}
/*==============================================*/
/**
 * @brief     Configure the sleep timer mode of the module to go into sleep during power save operation. This is a blocking API.
 * @pre   	  Can be issued any time in case of power save mode 9 (MSG_BASED).
 * @param[in] sleep_time     - Sleep Time value in seconds. Minimum value is 1, and maximum value is 2100
 * @return    0              -  Success \n
 *            Non-Zero Value - Failure
 *
 */
int16_t rsi_wlan_set_sleep_timer(uint16_t sleep_time)
{
  int16_t status               = RSI_SUCCESS;
  rsi_pkt_t *pkt               = NULL;
  rsi_set_sleep_timer_t *timer = NULL;
  SL_PRINTF(SL_WLAN_SET_SLEEP_TIMER_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SET_SLEEP_TIMER_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_SET_SLEEP_TIMER_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    timer = (rsi_set_sleep_timer_t *)pkt->data;
    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_set_sleep_timer_t));

    // Copy sleep_timer given by user into structure member
    rsi_uint16_to_2bytes(timer->timeval, sleep_time);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send set sleep timer command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_SLEEP_TIMER, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SET_SLEEP_TIMER_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SET_SLEEP_TIMER_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_SET_SLEEP_TIMER_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*====================================================*/
/**
 * @brief      Register the WLAN callback functions. This is a non-blocking API.
 * @param[in]  callback_id                       - Id of the callback function. Table below displays supported ids
 * @param[in]  void (*callback_handler_ptr)(void - Callback handler
 * @param[in]  status                            - Status of the asynchronous response
 * @param[in]  buffer                            - Payload of the asynchronous response
 * @param[in]  length                            - Length of the payload
 * 	           parameter callback ID                     |   Value	
 *             :-----------------------------------------|:-----------------------------------------------------------
 * 	           RSI_JOIN_FAIL_CB	                         |   0
 * 		         RSI_IP_FAIL_CB                            |   1
 * 		         RSI_REMOTE_SOCKET_TERMINATE_CB            |   2
 * 		         RSI_IP_CHANGE_NOTIFY_CB                   |   3
 * 		         RSI_STATIONS_DISCONNECT_NOTIFY_CB         |   4
 * 		         RSI_STATIONS_CONNECT_NOTIFY_CB            |   5
 * 		         RSI_WLAN_DATA_RECEIVE_NOTIFY_CB           |   6
 * 		         RSI_WLAN_WFD_DISCOVERY_NOTIFY_CB          |   7
 * 		         RSI_WLAN_RECEIVE_STATS_RESPONSE_CB        |   8
 * 		         RSI_WLAN_WFD_CONNECTION_REQUEST_NOTIFY_CB |   9
 * 		         RSI_WLAN_SCAN_RESPONSE_HANDLER            |   10
 * 		         RSI_WLAN_JOIN_RESPONSE_HANDLER            |   11
 * 		         RSI_WLAN_RAW_DATA_RECEIVE_HANDLER         |   12
 * 		         RSI_WLAN_SOCKET_CONNECT_NOTIFY_CB         |   13
 * 		         RSI_WLAN_SERVER_CERT_RECEIVE_NOTIFY_CB    |   14
 * 		         RSI_WLAN_ASYNC_STATS                      |   15
 * 		         RSI_FLASH_WRITE_RESPONSE                  |   16
 * 		         RSI_WLAN_ASSERT_NOTIFY_CB                 |   17
 * 		         RSI_WLAN_MAX_TCP_WINDOW_NOTIFY_CB         |   18
 * @param[in]  (void*)callback_handler_ptr (uint16_t status,uint8_t *buffer, const uint32_t length) – Application callback handler to be registered \n
 * &nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb; **[param] status** - Status of the asynchronous response \n
 * &nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb; **[param] buffer** - Payload of the asynchronous response \n
 * &nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb;&nspb; **[param] length** - Length of the payload \n
 * @note                  Refer to Error Codes section for the description of the above error codes \ref error-codes
 *             #### Prototypes of the callback functions with given callback id ####
 * 	           Callback id                               |   Function Description
 * 	           :-----------------------------------------|:-----------------------------------------------------------
 * 		         RSI_JOIN_FAIL_CB                          |   Called when asynchronous rejoin failure is received from the FW. Application should try to re-join to the AP. \n This is valid in both AP and STA mode \n This call back is triggered when module fails to connect to AP in STA mode or when AP creation fails. @pre  Need to call rsi_scan() API in STA mode, rsi_wlan_ap_start() API in AP mode. @param buffer   NULL\n @param status  Possible Error response codes - 0x0019,0x0045,0x0046,0x0047,0x0048,0x0049
 * 		         RSI_IP_FAIL_CB                            |   Called when asynchronous DHCP renewal failure is received from the FW. Application should retry IP configuration.  \n This is valid in both AP and STA mode \n This call back is triggered when module fails to renew the DHCP. @pre Need to call rsi_wlan_connect() API. @param buffer  NULL \n @param status  Possible Error response codes - 0xFF9C, 0xFF9D
 * 		         RSI_REMOTE_SOCKET_TERMINATE_CB            |   Called when asynchronous remote TCP socket closed is received from the FW. It is an indication given to application that the socket is terminated from remote. \n This is valid in both STA and AP mode \n This call back is triggered when remote socket is terminated or closed @pre  Need to connect to socket. @param buffer rsi_rsp_socket_close_t ( \ref rsi_rsp_socket_close_s) response structure is provided in callback   \n @param status  NA
 *             RSI_IP_CHANGE_NOTIFY_CB                   |   Called when asynchronous IP change notification is received from the FW. It is an indication given to application that the IP has been modified. \n This is valid only in STA mode \n This call back is triggered when AP changes the  ip address. @pre Need to call rsi_wlan_connect(),rsi_config_ipaddress() API. @param buffer NULL \n @param status  RSI_SUCCESS
 *             RSI_STATIONS_DISCONNECT_NOTIFY_CB         |   Called when asynchronous station disconnect notification is received from the FW in AP mode. It is an indication that the AP is disconnect. Application should retry to connect to the AP. \n This is valid when module acts as AP \n This call back is triggered when STA's are disconnected  @pre STA need to connect to the Accesspoint 9916. @param buffer mac address is provided in response structure   \n @param status  RSI_SUCCESS @param length 6
 *             RSI_STATIONS_CONNECT_NOTIFY_CB            |   Called when asynchronous station connect notification is received from the FW in AP mode. It is an indication that the application is connected to the AP \n This is valid when 9116 module acts as AP \n This call back is triggered when STA's are connected @pre  STA need to connect to the accesspoint 9116.  @param buffer mac address is provided in response structure \n @param status RSI_SUCCESS @param length 6
 *             RSI_WLAN_DATA_RECEIVE_NOTIFY_CB           |   Called when asynchronous data is received from the FW in TCP/IP bypass mode. \n This is valid in both AP and STA mode \n This call back is triggered when data is received in TCP/IP bypass mode @pre  Need to connect to socket. @param buffer raw data received \n @param status  RSI_SUCCESS
 * 		         RSI_WLAN_WFD_DISCOVERY_NOTIFY_CB          |   Called when wifi direct device discovery notification received from the FW. \n This is valid in WFD Mode only \n This call back is triggered when a PEER is discovered by the device @pre  Need to call  rsi_wireless_init() ,rsi_wlan_wfd_start_discovery API.  @param buffer  NULL \n @param status  RSI_SUCCESS
 *             RSI_WLAN_RECEIVE_STATS_RESPONSE_CB        |   Called when asynchronous receive statistics from the FW in per or end to end mode. \n This is valid in PER Mode only \n This call back is triggered when module wants to receive stats @pre  Need to call rsi_wlan_radio_init() API. @param buffer rsi_per_stats_rsp_t (\ref rsi_per_stats_rsp_s) response structure is provided in callback.  \n @param status  NA
 * 		         RSI_WLAN_WFD_CONNECTION_REQUEST_NOTIFY_CB |   Called when wi-fi direct connection request from the FW. \n This is valid in WFD Mode only \n This call back is triggered when there is a connection request from other Peer device @pre Need to rsi_wireless_init() ,rsi_wlan_wfd_start_discovery() API. @param buffer  device name \n @param status  RSI_SUCCESS @param length 32
 *             RSI_WLAN_SCAN_RESPONSE_HANDLER            |   Called when a response for scan request is received from the FW. It is an indication to host that the scan is success or failed. \n This is valid only STA mode \n This call back is triggered when module try to scan and receive response of all the available AP's @pre Need to call rsi_scan() API.  @param buffer NULL  \n @param status  **Success** - RSI_SUCCESS \n **Failure**  - 0x0002, 0x0003, 0x0005, 0x000A, 0x0014, 0x0015, 0x001A, 0x0021,0x0024,0x0025,0x0026,0x002C,0x003c
 * 		         RSI_WLAN_JOIN_RESPONSE_HANDLER		         |   Called when a response for join request is received from the FW. It is an indication to application that the Join to AP is success or failed. \n This is valid in STA mode \n This call back is triggered when STA is sucessfully connected  @pre  Need to call rsi_scan API.  @param buffer  NULL \n @param status  **Success** - RSI_SUCCESS\n **Failure** Possible Error response codes -0x0019, 0x0048,0x0045,0x0008
 *             RSI_WLAN_RAW_DATA_RECEIVE_HANDLER         |   Called when raw data packets are received from the FW. \n This is valid in both AP and STA mode \n This call back is triggered when raw data is received in TCP/IP bypass mode @pre Need to connect to socket.  @param buffer raw data  \n @param status  RSI_SUCCESS
 *             RSI_WLAN_SOCKET_CONNECT_NOTIFY_CB         |   Called when a socket connection response comes to the host \n This is valid in both STA and AP mode \n This call back is registerd and triggered when socket connects @pre  Need to create socket. @param buffer rsi_rsp_socket_create_t ( \ref rsi_rsp_socket_create_s)   \n @param status  **Success** - RSI_SUCCESS\n **Failure**  - RSI_FAILURE
 *             RSI_WLAN_SERVER_CERT_RECEIVE_NOTIFY_CB    |   Reserved 
 *             RSI_WLAN_ASYNC_STATS                      |   Called when async response come from the FW to the host. Host can register this callback to get all the information regarding AP connectivity. @param buffer rsi_state_notification_t ( \ref rsi_state_notification_s) response structure is provided in callback   \n @param status NA
 * 		         RSI_WLAN_ASSERT_NOTIFY_CB                 |   Called when WLAN assertion is triggered from FW. It returns the assert value to the application @param buffer   NULL\n @param status   Assert Value
 *             RSI_WLAN_MAX_TCP_WINDOW_NOTIFY_CB         |   Reserved
 */
uint16_t rsi_wlan_register_callbacks(uint32_t callback_id,
                                     void (*callback_handler_ptr)(uint16_t status,
                                                                  uint8_t *buffer,
                                                                  const uint32_t length))
{
  SL_PRINTF(SL_WLAN_REGISTER_CALLBACKS_ENTRY, WLAN, LOG_INFO);
  if (callback_id > RSI_MAX_NUM_CALLBACKS) {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    SL_PRINTF(SL_WLAN_REGISTER_CALLBACKS_NUMBER_EXCEEDS_MAX_NUM_CALLBACKS_OR_CALLBACK_ALREADY_REG, WLAN, LOG_INFO);
    return 1;
  }
  if (callback_id == RSI_JOIN_FAIL_CB) // check for NULL or not
  {
    // Register join fail callback handler
    rsi_wlan_cb_non_rom->callback_list.join_fail_handler = callback_handler_ptr;
  } else if (callback_id == RSI_IP_FAIL_CB) {
    // Register IP renewal fail callback handler
    rsi_wlan_cb_non_rom->callback_list.ip_renewal_fail_handler = callback_handler_ptr;
  } else if (callback_id == RSI_REMOTE_SOCKET_TERMINATE_CB) {
    // Register remote socket terminate callback handler
    rsi_wlan_cb_non_rom->callback_list.remote_socket_terminate_handler = callback_handler_ptr;
  } else if (callback_id == RSI_IP_CHANGE_NOTIFY_CB) {
    // Register IP change notify callback handler
    rsi_wlan_cb_non_rom->callback_list.ip_change_notify_handler = callback_handler_ptr;
  } else if (callback_id == RSI_STATIONS_CONNECT_NOTIFY_CB) {
    // Register station connect notify callback handler
    rsi_wlan_cb_non_rom->callback_list.stations_connect_notify_handler = callback_handler_ptr;
  } else if (callback_id == RSI_STATIONS_DISCONNECT_NOTIFY_CB) {
    // Register station disconnect notify callback handler
    rsi_wlan_cb_non_rom->callback_list.stations_disconnect_notify_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_DATA_RECEIVE_NOTIFY_CB) {
    // Register WLAN data packet receive notify callback handler
    rsi_wlan_cb_non_rom->callback_list.wlan_data_receive_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_RECEIVE_STATS_RESPONSE_CB) {
    // Register WLAN receive stats notify callback handler
    rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_WFD_DISCOVERY_NOTIFY_CB) {
    // Register Wi-Fi direct device discovery notify callback handler
    rsi_wlan_cb_non_rom->callback_list.wlan_wfd_discovery_notify_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_WFD_CONNECTION_REQUEST_NOTIFY_CB) {
    // Register Wi-Fi direct connection request notify callback handler
    rsi_wlan_cb_non_rom->callback_list.wlan_wfd_connection_request_notify_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_RAW_DATA_RECEIVE_HANDLER) {
    // Register raw data receive notify callback handler
    rsi_wlan_cb_non_rom->callback_list.raw_data_receive_handler = callback_handler_ptr;

  }
#ifndef RSI_M4_INTERFACE
  else if (callback_id == RSI_WLAN_SOCKET_CONNECT_NOTIFY_CB) {
    // Register socket connection notify callback handler
    rsi_wlan_cb_non_rom->callback_list.socket_connect_response_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_SERVER_CERT_RECEIVE_NOTIFY_CB) {
    // Register certificate response callback handler
    rsi_wlan_cb_non_rom->callback_list.certificate_response_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_ASYNC_STATS) {
    rsi_wlan_cb_non_rom->callback_list.wlan_async_module_state = callback_handler_ptr;
  }
#endif
  else if (callback_id == RSI_WLAN_ASSERT_NOTIFY_CB) {
    // Register assert notify callback handler
    rsi_wlan_cb_non_rom->callback_list.rsi_assertion_cb = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_MAX_TCP_WINDOW_NOTIFY_CB) {
    if ((RSI_TCP_IP_FEATURE_BIT_MAP & TCP_IP_FEAT_EXTENSION_VALID)
        && (RSI_EXT_TCPIP_FEATURE_BITMAP & EXT_TCP_DYNAMIC_WINDOW_UPDATE_FROM_HOST)) {
      // Register max available RX window callback handler
      rsi_wlan_cb_non_rom->callback_list.rsi_max_available_rx_window = callback_handler_ptr;
    }
  } else if (callback_id == RSI_WLAN_TWT_RESPONSE_CB) {
    rsi_wlan_cb_non_rom->callback_list.twt_response_handler = callback_handler_ptr;
  }
  SL_PRINTF(SL_WLAN_REGISTER_CALLBACKS_EXIT, WLAN, LOG_INFO);
  return 0;
}
/** @} */

/** @addtogroup NETWORK5
* @{
*/
/*==============================================*/
/**
 * @brief	Set the socket configuration parameters. Using this command (optional) is highly recommended. Based on the socket
 * 		configuration, module will use available buffers effectively. \n
 * 		This is a blocking API.
 *
 *  Parameters                      |    Description
 *  :-------------------------------|:--------------------------------------------------------------------------------------------------
 *  total socket                    |    Desired total number of sockets to open
 *  total_tcp_sockets               |    Desired total number of TCP sockets to open
 *  total_udp_sockets               |    Desired total number of UDP sockets to open
 *  tcp_tx_only_sockets             |    Desired total number of TCP sockets to open which are used only for data transmission
 *  tcp_rx_only_sockets             |    Desired total number of TCP sockets to open which are used only for data reception
 *  udp_tx_only_sockets             |    Desired total number of UDP sockets to open which are used only for data transmission
 *  udp_rx_only_sockets             |    Desired total number of UDP sockets to open which are used only for data reception
 *  tcp_rx_high_performance_sockets |    Desired total number of high performance TCP sockets to open. High performance sockets can be \n 
 *  ^                               |    allocated with more buffers based on the buffers availability. This option is valid only for TCP \n 
 *  ^                               |    data receive sockets. Socket can be opened as high performance by setting high performance bit in \n
 *  ^                               |    socket create command.
 *  tcp_rx_window_size_cap          |    Desired total to increase the TCP RX window size
 *  tcp_ack_window_div_factor       |    In case of high latency networks to configure the TCP ACK division factor with respective to the \n 
 *  ^                               |    window size; Increases the ACK frequency for asynchronous sockets \n
 *  ^                               |    Note: \n
 *  ^                               |    Default value is tcp_rx_window_size_cap.
 *
 * 
 * The following conditions must be met: \n
 *     total_sockets <= Maximum allowed sockets(10) \n
 *     (total_tcp_sockets + total_udp_sockets) <= total_sockets \n
 *     (total_tcp_tx_only_sockets + total_tcp_rx_only_sockets) <= total_tcp_sockets \n
 *     (total_udp_tx_only_sockets + total_udp_rx_only_sockets) <= total_udp_sockets \n
 *     total_tcp_rx_high_performance_sockets <= total_tcp_rx_only_sockets \n                                     
 *
 * @pre          Must be given after IP configuration command and before any socket creation.
 * @return    Zero           -  Success \n
 *            Non-Zero Value - Failure \n
 *                           If return value is greater than 0 \n
 *                          0x0021,0x0025,0x002C,0xFF6D \n
 * @note      Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_socket_config(void)
{

  rsi_pkt_t *pkt;
  rsi_socket_config_t *rsi_socket_config;
  int32_t status = RSI_SUCCESS;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Take the user provided data and fill it in antenna select structure
    rsi_socket_config = (rsi_socket_config_t *)pkt->data;

    // Total sockets
    rsi_socket_config->total_sockets = TOTAL_SOCKETS;

    // Total TCP sockets
    rsi_socket_config->total_tcp_sockets = TOTAL_TCP_SOCKETS;

    // Total UDP sockets
    rsi_socket_config->total_udp_sockets = TOTAL_UDP_SOCKETS;

    // Total TCP TX sockets
    rsi_socket_config->tcp_tx_only_sockets = TCP_TX_ONLY_SOCKETS;

    // Total TCP RX sockets
    rsi_socket_config->tcp_rx_only_sockets = TCP_RX_ONLY_SOCKETS;

    // Total UDP TX sockets
    rsi_socket_config->udp_tx_only_sockets = UDP_TX_ONLY_SOCKETS;

    // Total UDP RX sockets
    rsi_socket_config->udp_rx_only_sockets = UDP_RX_ONLY_SOCKETS;

    // TCP RX high performance sockets
    rsi_socket_config->tcp_rx_high_performance_sockets = TCP_RX_HIGH_PERFORMANCE_SOCKETS;

    // TCP RX windows size
    rsi_socket_config->tcp_rx_window_size_cap = TCP_RX_WINDOW_SIZE_CAP;

    // TCP RX windows division factor
    rsi_socket_config->tcp_rx_window_div_factor = TCP_RX_WINDOW_DIV_FACTOR;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send socket configuration command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SOCKET_CONFIG, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SOCK_CONFIG_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    return status;
  }

  // Return status
  return status;
}
/** @} */
/** @addtogroup NETWORK17
* @{
*/

/*==============================================*/
/**
 * @brief      Register and Deregister WLAN radio.
 * @param[in]    enable - To register and Deregister WLAN radio
 * @param[out]   RSI_SUCCESS
 * @return		0  -  Success \n
 *              Non-Zero Value - Failure
 *
 */
/// @private
int32_t rsi_wlan_req_radio(uint8_t enable)
{
  rsi_pkt_t *pkt                   = NULL;
  int32_t status                   = RSI_SUCCESS;
  rsi_wlan_req_radio_t *wlan_radio = NULL;
  SL_PRINTF(SL_WLAN_REQ_RADIO_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_REQ_RADIO_COMMAND_GIVEN_IN_WRONG_STATE, NETWORK, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_REQ_RADIO_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    wlan_radio = (rsi_wlan_req_radio_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_wlan_req_radio_t));

    wlan_radio->radio_req = enable;
    if (wlan_radio->radio_req) {
      rsi_wlan_cb_non_rom->wlan_radio = RSI_WLAN_RADIO_REGISTER;
    } else {
      rsi_wlan_cb_non_rom->wlan_radio = RSI_WLAN_RADIO_DEREGISTER;
    }

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_RADIO, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_RADIO_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_REQ_RADIO_WLAN_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_REQ_RADIO_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Deregister WLAN radio. This is a blocking API.
 * @pre 	      The application should be in radio init and unconnected state before calling this function.
 *			        \ref rsi_wlan_radio_deinit() API is called after \ref rsi_wlan_scan()
 * @param[out]  RSI_SUCCESS      
 * @return    	0              -  Success \n
 *              Non-Zero Value - Failure \n
 *              If the return value is greater than 0 \n
 *              0x0021- Command given in wrong state \n
 *              0x0101-WLAN radio is already deregistered
 *@note         1.If rsi_wlan_radio_deinit() API is called after rsi_wlan_scan().Need to  call rsi_wlan_scan() API again after the rsi_wlan_radio_init(). \n
 *              2.If rsi_wlan_radio_deinit() API is allowed before WLAN connected state only.\n
 *              3.Need to enable powersave before sending any command when the module is in deep sleep power save.
 *              #### Below APIs are allowed after rsi_wlan_radio_deinit() API ####
 *              3.Need to enable powersave before sending any command when the module is in deep sleep power save. \n
 *              Below APIs are allowed after rsi_wlan_radio_deinit() API \n
 *              1. \ref  rsi_wlan_radio_init()
 *              2. \ref rsi_wlan_power_save_profile() 
 *              3. \ref rsi_wlan_get() In this API only RSI_MAC_ADDRESS and RSI_CONNECTION_STATUS cmd_type are allowed.
 *              4. \ref  rsi_wireless_deinit()
 *              5. \ref  rsi_switch_proto()
 *              6. \ref  rsi_get_ram_log()
 *              7. \ref  rsi_get_fw_version()
 *              8. \ref  rsi_common_debug_log()
 *              9. \ref  rsi_bt_power_save_profile()
 *
 */

int32_t rsi_wlan_radio_deinit(void)
{
  int32_t status = RSI_SUCCESS;
  // Req for WLAN radio
  status = rsi_wlan_req_radio(RSI_DISABLE);

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_RADIO_DEINIT_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}
/** @} */
//#ifdef RSI_WAC_MFI_ENABLE

#ifdef RSI_WAC_MFI_ENABLE
/** @addtogroup NETWORK18
* @{
*/

/*==============================================*/
/**
 * @brief      Add the Apple defined IE elements to the beacon command request structure.
 * @param[in]  mfi_ie - Pointer to the IE element
 * @param[in]  ie_len - Length of the IE element
 * @return     0  -  Success \n
 *             Non-Zero Value - Failure \n
 *               -4 - Buffer not available to serve the command
 */
/// @private
int32_t rsi_wlan_add_mfi_ie(int8_t *mfi_ie, uint32_t ie_len)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_ADD_MFI_IE_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;
  rsi_req_add_mfi_ie_t *mfi_ie_req = NULL;

  // Get WLAN CB struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_ADD_MFI_IE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mfi_ie_req = (rsi_req_add_mfi_ie_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_add_mfi_ie_t));

    // Fill IE length
    mfi_ie_req->mfi_ie_len = ie_len;

    // Copy IE
    memcpy(mfi_ie_req->mfi_ie, mfi_ie, ie_len);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send WPS request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_ADD_MFI_IE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_ADD_MFI_IE_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_ADD_MFI_IE_WLAN_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_ADD_MFI_IE_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

#endif

/*==============================================*/
/**
 * @brief      Assign the user configurable channel gain values in different regions to the module from user.This method is used for overwriting default gain tables that are present in firmware. \n
 *             Customer can load all the three gain tables (i.e., 2.4GHz-20Mhz, 5GHz-20Mhz, 5GHz-40Mhz) one after other by changing band and bandwidth values. This is a blocking API.
   @note       1. **This frame has to be used by customers who has done FCC/ETSI/TELEC/KCC certification with their own antenna.  All other customers should not use this. Inappropriate use of this frame may result in violation of FCC/ETSI/TELEC/KCC or any certifications and Silicon labs is not liable for that**\n
 * @note          Internally firmware maintains two tables : Worldwide table & Region based table. Worldwide table is populated by firmware with Max power values that chip can transmit that meets target specs like EVM. Region based table has default gain value set. \n
               2. When certifying with user antenna, Region has to be set to Worldwide and sweep the power from 0 to 21dBm. Arrive at max power level that is passing certification especially band-edge. \n
               3. These FCC/ETSI/TELEC/KCC Max power level should be loaded in end-to-end mode via WLAN User Gain table. This has to be called done every boot-up since this information is not saved inside flash. Region based user gain table sent by application is copied onto Region based table .SoC uses this table in FCC/ETSI/TELEC/KCC to limit power and not to violate allowed limits. \n
                For Worldwide region firmware uses Worldwide table for Tx. For other regions(FCC/ETSI/TELEC/KCC), Firmware uses min value out of Worldwide & Region based table for Tx.  Also there will be part to part variation across chips and offsets are estimated during manufacturing flow which will be applied as correction factor during normal mode of operation. \n
 * @pre   	   \ref rsi_radio_init() API needs to be called before this API
 * @param[in]  band        -  0 ?  2.4GHz \n
 *					                  1 ? 5GHz \n
 *					                  2 ? Dual band (2.4 Ghz and 5 Ghz)
 * @param[in]  bandwidth   -  0 ? 20 MHz \n
 *                            1 ? 40 MHz
 * @param[in]  payload     - Pass channel gain values for different regions in an given array format.
 * @param[in]  payload_len - Max payload length (table size) in 2.4GHz is 128 bytes \n
 *                           Max payload length (table size) in 5GHz is 64 bytes
 *             #### Gain Table Payload Format ####
 *
 *                            1. Gain table Format for 2.4G Band: (Each entry of the table is 1 byte)
 *                               In 2Ghz, Max Gain/Power obtained from certification should be doubled and loaded.
 *                            <TABLE NAME[]>= {
 *                            <NO.of Regions>,
 *                            <REGION NAME 1>, <CHANNEL_CODE_2G>,
 *                            <CHANNEL NUMBER 1>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>, 
 *                            <CHANNEL NUMBER 2>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>, 
 *                            . 
 *                            . 
 *                            .
 *                            .
 *                            .       
 *                            <CHANNEL NUMBER m-1>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>,
 *                            <CHANNEL NUMBER m>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>, 
 *                            <REGION NAME 2>, <CHANNEL_CODE_2G>,
 *                            <CHANNEL NUMBER 1>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>,
 *                            <CHANNEL NUMBER 2>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>,
 *                            . 
 *                            . 
 *                            . 
 *                            . 
 *                            <CHANNEL NUMBER m-1>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>,
 *                            <CHANNEL NUMBER m>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>, 
 *                            }; 
 *    
 *                            Gain table Format for 5G Band: (Each entry of the table is 1 byte)
 *                              In 5Ghz, Max Gain/Power obtained from certification should be loaded.
 *                             <TABLE NAME[]>= { 
 *                                               <NO.of Regions>, 
 *                                               <REGION NAME 1>, <CHANNEL_CODE_5G>, 
 *                                               <CHANNEL NUMBER IN BAND 1 IF ANY>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                                <BAND_NUMBER 1>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <CHANNEL NUMBER IN BAND 2 IF ANY>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <BAND_NUMBER 2>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <CHANNEL NUMBER IN BAND 3 IF ANY>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <BAND_NUMBER 3>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <CHANNEL NUMBER IN BAND 4 IF ANY>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <BAND_NUMBER 4>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               . 
 *                                               . 
 *                                               . 
 *                                               . 
 *                                               . 
 *                                               <REGION NAME y>, <CHANNEL_CODE_5G>, 
 *                                               }; 
 *                            2. Supported Region names:
 *                                                    FCC, ETSI,TELEC, KCC 
 *                                                   The following are the regions and the values to be passed instead of macros in the example.
 *                                                   Region      |          Macro Value
 *                                                   ------------|--------------------
 *                                                   FCC         |            0
 *                                                   ETSI        |            1
 *                                                   TELEC       |            2
 *                                                   KCC         |            4
 *                            3. <CHANNEL_CODE_2G> is a 8 bit value which is encoded as:
 *                               If TX powers of all the channels are same, then use CHANNEL_CODE_2G as 17. In this case, mention channel number as 255.
 *                               Tf TX power is not same for all channels, then indicate CHANNEL_CODE_2G as no-of channels. And specify tx power values for all the channels indicated.
 *                            4. <CHANNEL_CODE_5G> is a 8 bit value encoded as number of rows in a region for 5G band.
 *                                a. 5G is divided into 4 sub bands: 
 *                                      band 1: channel number <= 48 
 *                                      band 2: channel number > 48 and channel number <= 64 
 *                                      band 3: channel number > 64 and channel number <= 144 
 *                                      band 4: channel number > 144 
 *                                b. If any channel in a band has different set of power values, specify the channel number followed by power values. 
 *                                c. If all the channels in a band 1 has same power values, specify the band number as 1 followed by power value. 
 *                                d. If all the channels in a band 2 has same power values, specify the band number as 2 followed by power value. 
 *                                e. If all the channels in a band 3 has same power values, specify the band number as 3 followed by power value. 
 *                                f. If all the channels in a band 4 has same power values, specify the band number as 4 followed by power value.
 *
 *
 * #### Example payload formats ####
 *
 *                            Examples: 
 *                          For 2.4Ghz Band in 20Mhz bandwidth
 *                            {3, //NUM_OF_REGIONS 
 *                                FCC, 13, //NUM_OF_CHANNELS 
 *                            //   rate,  11b, 11g, 11n   
 *                                    1,  34,  20,  20,  
 *                                    2,  34,  28,  28,  
 *                                    3,  34,  32,  32,  
 *                                    4,  34,  36,  36,  
 *                                    5,  34,  38,  38,  
 *                                    6,  34,  40,  40,  
 *                                    7,  34,  38,  38,  
 *                                    8,  34,  36,  36,  
 *                                    9,  34,  32,  32,  
 *                                    10, 34,  32,  32,  
 *                                    11, 34,  24,  24,  
 *                                    12, 34,  16,  24,  
 *                                    13, 34,  12,  12,  
 *                                TELEC, 17, 
 *                                     255, 20,  16, 16, 
 *                                KCC, 17, 
 *                                     255, 26,  20, 20, 
 *                            }; //}}} 
 *
 *                          For 5Ghz band in 20Mhz bandwidth
 *                             {2, 
 *                             FCC, 6, 
 *                                 1,  9, 10, //band 1 
 *                                 2,  8,  9, //band 2 
 *                               100,  4,  4, //band 3 
 *                                 3,  6,  8, //band 3 
 *                               149,  3,  3, //band 4 
 *                             TELEC, 4, 
 *                               1, 9, 10, //band 1 
 *                               2, 8, 10, //band 2   
 *                               3, 6,  8, //band 3   
 *                               4, 6,  7, //band 4 
 *                             };
 *                              
 *                              
 *                          For 5Ghz band in 40Mhz bandwidth
 *                             {2, 
 *                             FCC, 8, 
 *                                 1,  9, 10, //band 1 
 *                                62,  8,  9, //band 2 
 *                                 2,  8,  9, //band 2 
 *                               102,  4,  4, //band 3 
 *                               134,  6,  8, //band 3 
 *                                 3,  6,  8, //band 3 
 *                               151,  3,  3, //band 4    
 *                                 4,  6,  7, //band 4    
 *                             TELEC, 4, 
 *                                1, 9, 10, //band 1 
 *                                2, 8, 10, //band 2  
 *                                3, 6,  8, //band 3  
 *                                4, 6,  7, //band 4 
 *                             }; 
 *                                
 * ####Customers using Certified MARS antenna should use the gain table structures below:####
 *
 *                          For 2.4Ghz Band in 20Mhz bandwidth
 *                           {3,//NUM_OF_REGIONS
 *                               FCC, 0xD,//NUM_OF_CHANNELS
 *                           //   rate,  11b, 11g, 11n
 *                                   1,  28,  32,  30,
 *                                   2,  28,  32,  30,
 *                                   3,  28,  32,  30,
 *                                   4,  30,  28,  34,
 *                                   5,  30,  28,  34,
 *                                   6,  30,  28,  34,
 *                                   7,  30,  28,  34,
 *                                   8,  30,  28,  34,
 *                                   9,  28,  30,  30,
 *                                   10, 28,  30,  30,
 *                                   11, 28,  30,  30,
 *                                   12, 28,  30,  30,
 *                                   13, 28,  30,  30,
 *                               TELEC,0x11, //NA
 *                                    255, 20,  16, 16,
 *                               KCC, 0x11   //NA,
 *                                    255, 26,  20, 20
 *                           };
 *                            
 *                          For 5Ghz band in 20Mhz bandwidth
 *                           {2,
 *                           FCC, 0x6,
 *                               1, 12, 12, //band 1
 *                               2, 11, 11, //band 2   
 *                             100, 10, 12, //band 3   
 *                               3, 13, 13, //band 3   
 *                             140, 10, 11, //band 4   
 *                               4, 13, 13, //band 4   
 *                           TELEC, 0x4, //NA
 *                             1, 9, 10, //band 1
 *                             2, 8, 10, //band 2   
 *                             3, 6,  8, //band 3   
 *                             4, 6,  7, //band 4
 *                           };
 *                            
 *                            
 *                          For 5Ghz band in 40Mhz bandwidth
 *                           {2,
 *                           FCC, 0x8,   
 *                               1,  9,  9, //band 1
 *                              62,  8,  8, //band 2   
 *                               2,  9,  9, //band 2   
 *                             102,  9,  9, //band 3   
 *                             134, 12, 12, //band 3   
 *                               3, 10, 10, //band 3   
 *                             151, 11, 11, //band 4       
 *                               4, 11, 11, //band 4       
 *                           TELEC, 0x4, //NA  
 *                              1, 9, 10, //band 1
 *                              2, 8, 10, //band 2   
 *                              3, 6,  8, //band 3   
 *                              4, 6,  7, //band 4
 *                           };
 * @note		   1. Length of the payload should match with payload_len parameter value. \n
 * @note		   2. In 2.4Ghz band, 40Mhz is not supported
 * @return     0              -  Success \n
 *             Non-Zero	Value - Failure \n
 *	           If return value is less than 0 \n
 *             -2             - Invalid parameters \n
 *	           -3             - Command given in wrong state \n
 *             If return value is greater than 0 \n
 * 	           0x0021, 0x003E
 * @note       Refer to Error Codes section for above error codes \ref error-codes.
 */
int32_t rsi_wlan_update_gain_table(uint8_t band, uint8_t bandwidth, uint8_t *payload, uint16_t payload_len)
{
  int32_t status = 0;
  SL_PRINTF(SL_WLAN_UPDATE_GAIN_TABLE_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt = NULL;

  rsi_gain_table_info_t *gain_table = NULL;

  // Get WLAN CB struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check for gain table max supported payload length
  if (payload_len > GAIN_TABLE_MAX_PAYLOAD_LEN) {
    SL_PRINTF(SL_WLAN_UPDATE_GAIN_TABLE_INVALID_PARAM, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_UPDATE_GAIN_TABLE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    gain_table = (rsi_gain_table_info_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_gain_table_info_t));

    // Fill band
    gain_table->band = band;

    // Fill bandwidth
    gain_table->bandwidth = bandwidth;

    // Fill payload length
    gain_table->size = payload_len;

    // Fill payload content
    memcpy(gain_table->gain_table, payload, payload_len);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GAIN_TABLE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_GAIN_TABLE_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_UPDATE_GAIN_TABLE_WLAN_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_UPDATE_GAIN_TABLE_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}
/** @} */
/** @} */
