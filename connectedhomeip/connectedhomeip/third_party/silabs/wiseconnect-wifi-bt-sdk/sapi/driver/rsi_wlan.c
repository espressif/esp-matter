/*******************************************************************************
* @file  rsi_wlan.c
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
/*
  Global Variables
 * */
extern rsi_driver_cb_t *rsi_driver_cb;
extern rsi_socket_info_non_rom_t *rsi_socket_pool_non_rom;
extern rsi_socket_select_info_t *rsi_socket_select_info;
uint32_t scan_bitmap                            = RSI_SCAN_FEAT_BITMAP;
struct wpa_scan_results_arr *scan_results_array = NULL;
#ifdef PROCESS_SCAN_RESULTS_AT_HOST
int process_scan_results(uint8_t *buf, uint16_t len, int8_t rssi, uint8_t channel, uint16_t freq);
#endif
/** @addtogroup DRIVER13
* @{
*/
/*==============================================*/
/**
 * @fn         int8_t rsi_wlan_cb_init(rsi_wlan_cb_t *wlan_cb)
 * @brief      Initialize the WLAN control block structure. 
 * @param[in]  wlan_cb - Pointer to WLAN cb structure 
 * @return     0              - Success \n
 *             Non Zero Value - Failure 
 */
/// @private
int8_t rsi_wlan_cb_init(rsi_wlan_cb_t *wlan_cb)
{
  int8_t retval = RSI_ERR_NONE;

  // validate input parameter
  if (wlan_cb == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  // initialize wlan control block with default values
  wlan_cb->state  = RSI_WLAN_STATE_NONE;
  wlan_cb->status = 0;

  // Create wlan mutex
  rsi_mutex_create(&wlan_cb->wlan_mutex);
  // Create wlan mutex
  wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;

  retval = rsi_semaphore_create(&rsi_driver_cb_non_rom->nwk_cmd_send_sem, 0);
  if (retval != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
  retval = rsi_semaphore_create(&rsi_driver_cb_non_rom->wlan_cmd_send_sem, 0);
  if (retval != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
  retval = rsi_semaphore_create(&rsi_driver_cb_non_rom->send_data_sem, 0);
  if (retval != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
  rsi_semaphore_post(&rsi_driver_cb_non_rom->nwk_cmd_send_sem);
  rsi_semaphore_post(&rsi_driver_cb_non_rom->wlan_cmd_send_sem);
  // Create wlan semaphore
  retval = rsi_semaphore_create(&wlan_cb->wlan_sem, 0);
  if (retval != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
  retval = rsi_semaphore_create(&rsi_driver_cb_non_rom->wlan_cmd_sem, 0);
  if (retval != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
  retval = rsi_semaphore_create(&rsi_driver_cb_non_rom->nwk_sem, 0);
  if (retval != RSI_ERROR_NONE) {
    return RSI_ERROR_SEMAPHORE_CREATE_FAILED;
  }
  wlan_cb->app_buffer = 0;

  return retval;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_driver_wlan_send_cmd(rsi_wlan_cmd_request_t cmd, rsi_pkt_t *pkt)
 * @brief       Fills command info and places it into wlan TX queue
 * @param[in]   cmd - Type of the command to send 
 * @param[in]   pkt - Pointer of the packet to send 
 * @return      0              - Success \n
 *              Non Zero Value - Failure             
 *
 */

/// @private
int32_t rsi_driver_wlan_send_cmd(rsi_wlan_cmd_request_t cmd, rsi_pkt_t *pkt)
{
  uint16_t payload_size     = 0;
  int32_t status            = RSI_SUCCESS;
  uint8_t cmd_type          = 0;
  uint16_t multicast_bitmap = 0;
  uint32_t mqtt_command_type;
  // Get host descriptor pointer
  uint8_t *host_desc = (pkt->desc);
  switch (cmd) {

    case RSI_WLAN_REQ_BAND: {
      rsi_req_band_t *rsi_band = (rsi_req_band_t *)pkt->data;

      // fill band value
      rsi_band->band_value = RSI_BAND;

      // fill payload size
      payload_size = sizeof(rsi_req_band_t);

    } break;
    case RSI_WLAN_REQ_SELECT_REQUEST: {
      payload_size = sizeof(rsi_req_socket_select_t);
    } break;
    case RSI_WLAN_REQ_UPDATE_TCP_WINDOW: {
      payload_size = sizeof(rsi_req_tcp_window_update_t);
    } break;
#if RSI_WLAN_CONFIG_ENABLE
    case RSI_WLAN_REQ_CONFIG: {
      rsi_req_config_t *rsi_config = (rsi_req_config_t *)pkt->data;

      // fill configuration type
      rsi_config->config_type = CONFIG_RTSTHRESHOLD;

      // fill rtsthreshold value
      rsi_config->value = RSI_RTS_THRESHOLD;

      // fill payload size
      payload_size = sizeof(rsi_req_config_t);
    } break;
#endif

    case RSI_WLAN_REQ_SET_MAC_ADDRESS: {
      rsi_req_mac_address_t *rsi_set_mac = (rsi_req_mac_address_t *)pkt->data;

      // fill mac address
      memcpy(rsi_set_mac->mac_address, rsi_driver_cb->wlan_cb->mac_address, 6);

      // fill payload size
      payload_size = sizeof(rsi_req_mac_address_t);

    } break;
    case RSI_WLAN_REQ_SET_MULTICAST_FILTER: {

      rsi_req_multicast_filter_info_t *multicast_filter;

      multicast_filter = (rsi_req_multicast_filter_info_t *)pkt->data;

      cmd_type = multicast_filter->cmd_type;

      multicast_bitmap = (uint16_t)cmd_type;

      if ((cmd_type == RSI_MULTICAST_MAC_ADD_BIT) || (cmd_type == RSI_MULTICAST_MAC_CLEAR_BIT)) {
        multicast_bitmap |= (multicast_mac_hash((uint8_t *)multicast_filter->mac_address) << 8);
      }

      // memset the pkt
      memset(&pkt->data, 0, 2);

      // copy processed payload in to buffer
      rsi_uint16_to_2bytes((uint8_t *)&pkt->data, multicast_bitmap);

      // fill payload size
      payload_size                              = 2;
      rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_SET_MULTICAST_FILTER;

    } break;
    case RSI_WLAN_REQ_SET_REGION: {
      rsi_req_set_region_t *rsi_set_region = (rsi_req_set_region_t *)pkt->data;

      // select set region code from user or from beacon
      rsi_set_region->set_region_code_from_user_cmd = RSI_SET_REGION_FROM_USER_OR_BEACON;

      // fill region code
      rsi_set_region->region_code = RSI_REGION_CODE;

      // fill module type
      rsi_uint16_to_2bytes(rsi_set_region->module_type, RSI_MODULE_TYPE);

      // fill payload size
      payload_size = sizeof(rsi_req_set_region_t);

    } break;
    case RSI_WLAN_REQ_SET_REGION_AP: {
      rsi_req_set_region_ap_t *rsi_set_region_ap = (rsi_req_set_region_ap_t *)pkt->data;

      // select set region code from user or flash
      rsi_set_region_ap->set_region_code_from_user_cmd = RSI_SET_REGION_AP_FROM_USER;

      // fill region code
      rsi_strcpy(rsi_set_region_ap->country_code, RSI_COUNTRY_CODE);

#if RSI_SET_REGION_AP_FROM_USER
      // extract set region ap information
      extract_setregionap_country_info(rsi_set_region_ap);
#endif

      // fill payload size
      payload_size = sizeof(rsi_req_set_region_ap_t);

    } break;
    case RSI_WLAN_REQ_REJOIN_PARAMS: {
      rsi_req_rejoin_params_t *rsi_rejoin = (rsi_req_rejoin_params_t *)pkt->data;

      // fill rejoin retry count
      rsi_uint32_to_4bytes(rsi_rejoin->max_retry, RSI_REJOIN_MAX_RETRY);

      // fill rejoin retry scan interval
      rsi_uint32_to_4bytes(rsi_rejoin->scan_interval, RSI_REJOIN_SCAN_INTERVAL);

      // fill beacon missed count
      rsi_uint32_to_4bytes(rsi_rejoin->beacon_missed_count, RSI_REJOIN_BEACON_MISSED_COUNT);

      // fill rejoin first time retry enable or disable
      rsi_uint32_to_4bytes(rsi_rejoin->first_time_retry_enable, RSI_REJOIN_FIRST_TIME_RETRY);

      // fill payload size
      payload_size = sizeof(rsi_req_rejoin_params_t);

    } break;
    case RSI_WLAN_REQ_11AX_PARAMS: {
      wlan_11ax_config_params_t *config = (wlan_11ax_config_params_t *)pkt->data;
      config->guard_interval            = GUARD_INTERVAL;
      config->nominal_pe                = NOMINAL_PE;
      config->dcm_enable                = DCM_ENABLE;
      config->ldpc_enable               = LDPC_ENABLE;
      config->ng_cb_enable              = NG_CB_ENABLE;
      config->ng_cb_values              = NG_CB_VALUES;
      config->uora_enable               = UORA_ENABLE;
      config->trigger_rsp_ind           = TRIGGER_RESP_IND;
      config->ipps_valid_value          = IPPS_VALID_VALUE;
      config->tx_only_on_ap_trig        = TX_ONLY_ON_AP_TRIG;
      config->twt_support               = TWT_SUPPORT;

      // fill payload size
      payload_size = sizeof(wlan_11ax_config_params_t);
    } break;
    case RSI_WLAN_REQ_TWT_PARAMS: {
      // fill payload size
      payload_size = sizeof(rsi_twt_req_t);
    } break;
    case RSI_WLAN_REQ_EAP_CONFIG: {
      rsi_req_eap_config_t *rsi_eap_req = (rsi_req_eap_config_t *)pkt->data;

      // copy enterprise configuratiomn data
      rsi_strcpy((int8_t *)rsi_eap_req->eap_method, RSI_EAP_METHOD);
      rsi_strcpy((int8_t *)rsi_eap_req->inner_method, RSI_EAP_INNER_METHOD);
      rsi_uint32_to_4bytes((uint8_t *)rsi_eap_req->okc_enable, 0);
      rsi_strcpy((int8_t *)rsi_eap_req->private_key_password, RSI_PRIVATE_KEY_PASSWORD);

      // fill payload size
      payload_size = sizeof(rsi_req_eap_config_t);

    } break;
    case RSI_WLAN_REQ_SCAN: {
      // Scan Parameters
      rsi_req_scan_t *rsi_scan = (rsi_req_scan_t *)pkt->data;

      // Sets channel bitmap for 2.4GHz
      if (RSI_BAND == RSI_DUAL_BAND) {
        rsi_uint16_to_2bytes(rsi_scan->channel_bit_map_2_4, RSI_SCAN_CHANNEL_BIT_MAP_2_4);
        rsi_uint32_to_4bytes(rsi_scan->channel_bit_map_5, RSI_SCAN_CHANNEL_BIT_MAP_5);
      } else if (RSI_BAND == RSI_BAND_5GHZ) {
        // Set channel bitmap for 5GHz
        rsi_uint32_to_4bytes(rsi_scan->channel_bit_map_5, RSI_SCAN_CHANNEL_BIT_MAP_5);
      } else {
        rsi_uint16_to_2bytes(rsi_scan->channel_bit_map_2_4, RSI_SCAN_CHANNEL_BIT_MAP_2_4);
      }

      if (rsi_scan->scan_feature_bitmap == 0)
        rsi_scan->scan_feature_bitmap = RSI_SCAN_FEAT_BITMAP;

#ifdef PROCESS_SCAN_RESULTS_AT_HOST
      if (rsi_scan->scan_feature_bitmap == 0)
        rsi_scan->scan_feature_bitmap = RSI_SCAN_FEAT_BITMAP;
#else
      rsi_scan->scan_feature_bitmap = RSI_SCAN_FEAT_BITMAP;
#endif
      payload_size = sizeof(rsi_req_scan_t);
    } break;
    case RSI_WLAN_REQ_JOIN: {
      // Join Parameters
      rsi_req_join_t *rsi_join = (rsi_req_join_t *)pkt->data;

      rsi_join->data_rate           = RSI_DATA_RATE;
      rsi_join->power_level         = RSI_POWER_LEVEL;
      rsi_join->join_feature_bitmap = RSI_JOIN_FEAT_BIT_MAP;
      rsi_uint32_to_4bytes(rsi_join->listen_interval, RSI_LISTEN_INTERVAL);
      memcpy(rsi_join->join_bssid, rsi_wlan_cb_non_rom->join_bssid_non_rom, 6);

      payload_size = sizeof(rsi_req_join_t);
      /* In concurrent mode, no need to update the expected response */
      if (rsi_driver_cb->wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE) {
        rsi_driver_cb->wlan_cb->expected_response = (rsi_wlan_cmd_response_t)RSI_WLAN_REQ_JOIN;
      }

    } break;

    case RSI_WLAN_REQ_GAIN_TABLE: {
      rsi_gain_table_info_t *gain_table = (rsi_gain_table_info_t *)pkt->data;
      // payload of gain table parameters
      payload_size = sizeof(rsi_gain_table_info_t) + (gain_table->size) - 1; // -1 is for gain_table->gain_table
    } break;

    case RSI_WLAN_REQ_CONFIGURE_P2P: {
      // payload of p2p parameters
      payload_size = sizeof(rsi_req_configure_p2p_t);
    } break;
    case RSI_WLAN_REQ_HOST_PSK: {
      // PSK and PMK parameters
      payload_size = sizeof(rsi_req_psk_t);
    } break;
    case RSI_WLAN_REQ_BG_SCAN: {
      rsi_req_bg_scan_t *rsi_bg_scan = (rsi_req_bg_scan_t *)pkt->data;

      if (pkt->data[0] == 0xAB) {
        if (pkt->data[1] == 1) {
          // Enable or Disable bg scan
          rsi_uint16_to_2bytes(rsi_bg_scan->bgscan_enable, 1);

          // Enable or Disable instant bg scan
          rsi_uint16_to_2bytes(rsi_bg_scan->enable_instant_bgscan, 1);
        }

      } else {
        // Enable or Disable bg scan
        rsi_uint16_to_2bytes(rsi_bg_scan->bgscan_enable, RSI_BG_SCAN_ENABLE);

        // Enable or Disable instant bg scan
        rsi_uint16_to_2bytes(rsi_bg_scan->enable_instant_bgscan, RSI_INSTANT_BG);
      }
      // bg scan threshold value
      rsi_uint16_to_2bytes(rsi_bg_scan->bgscan_threshold, RSI_BG_SCAN_THRESHOLD);

      // tolerance threshold
      rsi_uint16_to_2bytes(rsi_bg_scan->rssi_tolerance_threshold, RSI_RSSI_TOLERANCE_THRESHOLD);

      // periodicity
      rsi_uint16_to_2bytes(rsi_bg_scan->bgscan_periodicity, RSI_BG_SCAN_PERIODICITY);

      // active scan duration
      rsi_uint16_to_2bytes(rsi_bg_scan->active_scan_duration, RSI_ACTIVE_SCAN_DURATION);

      // passive scan duration
      rsi_uint16_to_2bytes(rsi_bg_scan->passive_scan_duration, RSI_PASSIVE_SCAN_DURATION);

      // Enable multi probe
      rsi_bg_scan->multi_probe = RSI_MULTIPROBE;

      // fill payload size
      payload_size = sizeof(rsi_req_bg_scan_t);

      rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_BG_SCAN;
    } break;
    case RSI_WLAN_REQ_TIMEOUT: {

      rsi_req_timeout_t *rsi_timeout = (rsi_req_timeout_t *)pkt->data;

      // Timeout Bitmap
      rsi_uint32_to_4bytes(rsi_timeout->timeout_bitmap, RSI_TIMEOUT_BIT_MAP);

      // Timeout value
      rsi_uint16_to_2bytes(rsi_timeout->timeout_value, RSI_TIMEOUT_VALUE);

      // fill payload size
      payload_size = sizeof(rsi_req_timeout_t);
    } break;
    case RSI_WLAN_REQ_ROAM_PARAMS: {
      rsi_req_roam_params_t *rsi_roam_params = (rsi_req_roam_params_t *)pkt->data;

      // Enable or Disable bg scan
      rsi_uint32_to_4bytes(rsi_roam_params->roam_enable, RSI_ROAMING_SUPPORT);

      // Enable or Disable instant bg scan
      rsi_uint32_to_4bytes(rsi_roam_params->roam_threshold, RSI_ROAMING_THRESHOLD);

      // bg scan threshold value
      rsi_uint32_to_4bytes(rsi_roam_params->roam_hysteresis, RSI_ROAMING_HYSTERISIS);

      // fill payload size
      payload_size                              = sizeof(rsi_req_roam_params_t);
      rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_ROAM_PARAMS;

    } break;
    case RSI_WLAN_REQ_WPS_METHOD: {
      // fill payload size for wps method
      payload_size = sizeof(rsi_req_wps_method_t);

    } break;
    case RSI_WLAN_REQ_DISCONNECT: {
      // fill payload size for disconnect parameters
      payload_size                              = sizeof(rsi_req_disassoc_t);
      rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_DISCONNECT;

    } break;
    case RSI_WLAN_REQ_TX_TEST_MODE: {
      rsi_req_tx_test_info_t *rsi_tx_test_info = (rsi_req_tx_test_info_t *)pkt->data;

      // Enable or Disable transmit test mode
      rsi_uint16_to_2bytes(rsi_tx_test_info->rate_flags, RSI_TX_TEST_RATE_FLAGS);

      // Enable or Disable transmit test mode
      rsi_uint16_to_2bytes(rsi_tx_test_info->channel_bw, RSI_TX_TEST_PER_CH_BW);

      // Enable or Disable transmit test mode
      rsi_uint16_to_2bytes(rsi_tx_test_info->aggr_enable, RSI_TX_TEST_AGGR_ENABLE);

      // Enable or Disable transmit test mode
      rsi_uint16_to_2bytes(rsi_tx_test_info->reserved, 0);

      // Enable or Disable transmit test mode
      rsi_uint16_to_2bytes(rsi_tx_test_info->no_of_pkts, RSI_TX_TEST_NUM_PKTS);

      // Enable or Disable transmit test mode
      rsi_uint16_to_2bytes(rsi_tx_test_info->delay, RSI_TX_TEST_DELAY);

      //  fill payload size tx test mode parameters
      payload_size = sizeof(rsi_req_tx_test_info_t);

    } break;
    case RSI_WLAN_REQ_RX_STATS: {
      //  fill payload size rx stats parameters
      payload_size = sizeof(rsi_req_rx_stats_t);

    } break;
    case RSI_WLAN_REQ_PING_PACKET: {
      // PING parameters
      payload_size = sizeof(rsi_req_ping_t);
    } break;

    case RSI_WLAN_REQ_DYNAMIC_POOL: {
      // fill payload size
      payload_size                              = sizeof(rsi_udynamic);
      rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_DYNAMIC_POOL;
    } break;

    case RSI_WLAN_REQ_SOCKET_CREATE: {
      payload_size = sizeof(rsi_req_socket_t);

    } break;
    case RSI_WLAN_REQ_SOCKET_READ_DATA: {
      payload_size = sizeof(rsi_req_socket_read_t);
    } break;
    case RSI_WLAN_REQ_SET_CERTIFICATE: {
      rsi_req_set_certificate_t *set_certificate = (rsi_req_set_certificate_t *)pkt->data;

      if (set_certificate->cert_info.more_chunks) {
        payload_size = RSI_MAX_CERT_SEND_SIZE;
      } else {
        payload_size =
          rsi_bytes2R_to_uint16(set_certificate->cert_info.certificate_length) + sizeof(struct rsi_cert_info_s);
      }
    } break;
    case RSI_WLAN_REQ_SET_WEP_KEYS: {
      payload_size = sizeof(rsi_wep_keys_t);
    } break;
    case RSI_WLAN_REQ_IPCONFV4: {
      payload_size                              = sizeof(rsi_req_ipv4_parmas_t);
      rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_IPCONFV4;
    } break;
    case RSI_WLAN_REQ_IPCONFV6: {
      payload_size                              = sizeof(rsi_req_ipv6_parmas_t);
      rsi_driver_cb->wlan_cb->expected_response = RSI_WLAN_RSP_IPCONFV6;
    } break;
    case RSI_WLAN_REQ_MDNSD:
    case RSI_WLAN_REQ_HTTP_OTAF:
    case RSI_WLAN_REQ_HTTP_CLIENT_POST:
    case RSI_WLAN_REQ_HTTP_CLIENT_GET:
    case RSI_WLAN_REQ_HTTP_CLIENT_POST_DATA: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_WLAN_REQ_HTTP_CREDENTIALS: {
      payload_size = sizeof(rsi_req_http_credentials_t);
    } break;
    case RSI_WLAN_REQ_SMTP_CLIENT: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_WLAN_REQ_SOCKET_ACCEPT: {
      payload_size = sizeof(rsi_req_socket_accept_t);
    } break;
#ifndef RSI_M4_INTERFACE
    case RSI_WLAN_REQ_CERT_VALID: {
      payload_size = sizeof(rsi_req_cert_valid_t);
    } break;
#endif
    case RSI_WLAN_REQ_DNS_QUERY: {
      payload_size = sizeof(rsi_req_dns_query_t);
    } break;
    case RSI_WLAN_REQ_DNS_UPDATE: {
      payload_size = sizeof(rsi_req_dns_update_t);
    } break;
    case RSI_WLAN_REQ_DNS_SERVER_ADD: {
      payload_size = sizeof(rsi_req_dns_server_add_t);
    } break;
    case RSI_WLAN_REQ_DHCP_USER_CLASS: {
      payload_size = sizeof(rsi_dhcp_user_class_t);
    } break;
    case RSI_WLAN_REQ_SOCKET_CLOSE: {
      payload_size = sizeof(rsi_req_socket_close_t);
    } break;

    case RSI_WLAN_REQ_AP_CONFIGURATION: {
      rsi_req_ap_config_t *ap_config = (rsi_req_ap_config_t *)pkt->data;
#if RSI_AP_KEEP_ALIVE_ENABLE
      // copy AP keep alive type
      ap_config->ap_keepalive_type = RSI_AP_KEEP_ALIVE_TYPE;

      // copy AP keep alive period
      ap_config->ap_keepalive_period = RSI_AP_KEEP_ALIVE_PERIOD;
#endif
      // copy max station supported  by uint16 to 2 bytes conversion
      rsi_uint16_to_2bytes(ap_config->max_sta_support, RSI_MAX_STATIONS_SUPPORT);

      payload_size = sizeof(rsi_req_ap_config_t);

    } break;
    case RSI_WLAN_REQ_HT_CAPABILITIES: {
      rsi_req_ap_ht_caps_t *ap_ht_caps = (rsi_req_ap_ht_caps_t *)pkt->data;

      rsi_uint16_to_2bytes(ap_ht_caps->mode_11n_enable, RSI_MODE_11N_ENABLE);

      rsi_uint16_to_2bytes(ap_ht_caps->ht_caps_bitmap, RSI_HT_CAPS_BIT_MAP);

      payload_size = sizeof(rsi_req_ap_ht_caps_t);

    } break;
    case RSI_WLAN_REQ_FTP: {
      rsi_ftp_file_ops_t *file_ops = (rsi_ftp_file_ops_t *)pkt->data;

      switch (file_ops->command_type) {
        case RSI_FTP_CREATE:
        case RSI_FTP_DISCONNECT:
        case RSI_FTP_DESTROY:
        case RSI_FTP_FILE_WRITE:
        case RSI_FTP_FILE_READ:
        case RSI_FTP_FILE_DELETE:
        case RSI_FTP_DIRECTORY_SET:
        case RSI_FTP_DIRECTORY_DELETE:
        case RSI_FTP_DIRECTORY_LIST:
        case RSI_FTP_DIRECTORY_CREATE: {
          payload_size = sizeof(rsi_ftp_file_ops_t);
        } break;
        case RSI_FTP_COMMAND_MODE_SET: {
          payload_size = sizeof(rsi_ftp_mode_params_t);
        } break;
#ifdef CHIP_9117
        case RSI_FTP_COMMAND_FILE_SIZE_SET: {
          payload_size = sizeof(rsi_ftp_file_size_set_params_t);
        } break;
#endif
        case RSI_FTP_CONNECT: {
          payload_size = sizeof(rsi_ftp_connect_t);
        } break;
        case RSI_FTP_FILE_RENAME: {
          payload_size = sizeof(rsi_ftp_file_rename_t);
        } break;
        case RSI_FTP_FILE_WRITE_CONTENT: {
          payload_size = rsi_bytes2R_to_uint16(host_desc);
        } break;
        default: {
        }
      }

    } break;
    case RSI_WLAN_REQ_WMM_PS: {
      // wmmps parameters
      rsi_wmm_ps_parms_t *wmm_ps = (rsi_wmm_ps_parms_t *)pkt->data;

      // set wmm enable or disable
      rsi_uint16_to_2bytes(wmm_ps->wmm_ps_enable, RSI_WMM_PS_ENABLE);

      // set wmm enable or disable
      // 0- TX BASED 1 - PERIODIC
      rsi_uint16_to_2bytes(wmm_ps->wmm_ps_type, RSI_WMM_PS_TYPE);

      // set wmm wake up interval
      rsi_uint32_to_4bytes(wmm_ps->wmm_ps_wakeup_interval, RSI_WMM_PS_WAKE_INTERVAL);

      // set wmm UAPSD bitmap
      // Resetting this bit as it is affecting throughput in COEX scenarios
      wmm_ps->wmm_ps_uapsd_bitmap = 0; //RSI_WMM_PS_UAPSD_BITMAP;

      //fill payload size wmm ps parameters
      payload_size = sizeof(rsi_wmm_ps_parms_t);

    } break;
    case RSI_WLAN_REQ_MULTICAST: {
      payload_size = sizeof(rsi_req_multicast_t);
    } break;
    case RSI_WLAN_REQ_FWUP: {
      rsi_req_fwup_t *fwup = (rsi_req_fwup_t *)pkt->data;

      // Get packet length
      uint16_t length = rsi_bytes2R_to_uint16(fwup->length);

      // Content length + 2bytes type + 2bytes length
      payload_size = (length + 4);
    } break;
    case RSI_WLAN_REQ_WEBPAGE_LOAD: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_WLAN_REQ_JSON_LOAD: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_WLAN_REQ_WEBPAGE_ERASE: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_WLAN_REQ_JSON_OBJECT_ERASE: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_WLAN_REQ_WEBPAGE_CLEAR_ALL: {

      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_WLAN_REQ_HOST_WEBPAGE_SEND: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_WLAN_REQ_POP3_CLIENT: {
      payload_size = sizeof(rsi_req_pop3_client_t);
    } break;
    case RSI_WLAN_REQ_HTTP_CLIENT_PUT: {

      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_WLAN_REQ_OTA_FWUP: {
      payload_size = sizeof(rsi_req_ota_fwup_t);
    } break;
    case RSI_WLAN_REQ_SET_PROFILE: {
      payload_size = rsi_bytes2R_to_uint16(host_desc);
    } break;
    case RSI_WLAN_REQ_GET_PROFILE:
    case RSI_WLAN_REQ_DELETE_PROFILE: {
      payload_size = sizeof(rsi_profile_req_t);
    } break;
    case RSI_WLAN_REQ_AUTO_CONFIG_ENABLE: {
      payload_size = sizeof(rsi_auto_config_enable_t);
    } break;
    case RSI_WLAN_REQ_SOCKET_CONFIG: {
      // fill payload size
      payload_size = sizeof(rsi_socket_config_t);
    } break;
#ifdef RSI_WAC_MFI_ENABLE
    case RSI_WLAN_REQ_ADD_MFI_IE: {
      rsi_req_add_mfi_ie_t *rsi_mfi_ie = (rsi_req_add_mfi_ie_t *)pkt->data;
      // fill payload size
      payload_size = rsi_mfi_ie->mfi_ie_len + 1;
    } break;
#endif
    case RSI_WLAN_REQ_FILTER_BCAST_PACKETS: {
      // fill payload size
      payload_size = sizeof(rsi_req_wlan_filter_bcast_t);

    } break;
    case RSI_WLAN_REQ_SNTP_CLIENT: {
      // fill payload size
      payload_size = sizeof(rsi_sntp_client_t);
    } break;

    case RSI_WLAN_REQ_USER_STORE_CONFIG: {
      // fill payload size
      payload_size = sizeof(rsi_user_store_config_t);

    } break;
    case RSI_WLAN_REQ_SET_SLEEP_TIMER: {
      // fill payload size
      payload_size = sizeof(rsi_set_sleep_timer_t);

    } break;
    case RSI_WLAN_REQ_RADIO: {
      // fill payload size
      payload_size = sizeof(rsi_wlan_req_radio_t);

    } break;
    case RSI_WLAN_REQ_FREQ_OFFSET: {
      payload_size = sizeof(rsi_freq_offset_t);
    } break;
    case RSI_WLAN_REQ_CALIB_WRITE: {
      payload_size = sizeof(rsi_calib_write_t);
    } break;
    case RSI_WLAN_REQ_EMB_MQTT_CLIENT: {
      rsi_req_emb_mqtt_command_t *mqtt_cmd = (rsi_req_emb_mqtt_command_t *)pkt->data;
      mqtt_command_type                    = rsi_bytes4R_to_uint32(mqtt_cmd->command_type);
      switch (mqtt_command_type) {
        case RSI_EMB_MQTT_CLIENT_INIT: {
          payload_size = sizeof(rsi_emb_mqtt_client_init_t);
        } break;
        case RSI_EMB_MQTT_CONNECT: {
          payload_size = sizeof(rsi_emb_mqtt_connect_t);
        } break;
        case RSI_EMB_MQTT_SUBSCRIBE: {
          payload_size = sizeof(rsi_emb_mqtt_sub_t);
        } break;
        case RSI_EMB_MQTT_SND_PUB_PKT: {

          rsi_emb_mqtt_snd_pub_t *mqtt_pub_cmd = (rsi_emb_mqtt_snd_pub_t *)pkt->data;
          uint16_t length                      = rsi_bytes2R_to_uint16(mqtt_pub_cmd->msg_len);
          payload_size                         = sizeof(rsi_emb_mqtt_snd_pub_t) + length;
        } break;
        case RSI_EMB_MQTT_UNSUBSCRIBE: {
          payload_size = sizeof(rsi_emb_mqtt_unsub_t);
        } break;
        case RSI_EMB_MQTT_DISCONNECT:
        case RSI_EMB_MQTT_COMMAND_DESTROY: {
          payload_size = sizeof(rsi_req_emb_mqtt_command_t);
        } break;
        default: {
        }
      }
    } break;
    case RSI_WLAN_REQ_INIT:
    case RSI_WLAN_REQ_QUERY_NETWORK_PARAMS:
    case RSI_WLAN_REQ_CFG_SAVE:
    case RSI_WLAN_REQ_GET_CFG:
    case RSI_WLAN_REQ_RSSI:
    case RSI_WLAN_REQ_FW_VERSION:
    case RSI_WLAN_REQ_MAC_ADDRESS:
    case RSI_WLAN_REQ_QUERY_GO_PARAMS:
    case RSI_WLAN_REQ_CONNECTION_STATUS:
    case RSI_WLAN_REQ_WIRELESS_FWUP:
    case RSI_WLAN_REQ_HTTP_ABORT:
    case RSI_WLAN_REQ_FTP_FILE_WRITE:
    case RSI_WLAN_REQ_GET_RANDOM:
    case RSI_WLAN_REQ_GET_STATS:
    case RSI_WLAN_REQ_EXT_STATS:
      break;

    default: {
      break;
    }
  }
  // Memset host descriptor
  memset(host_desc, 0, RSI_HOST_DESC_LENGTH);

  // Fill payload length
  rsi_uint16_to_2bytes(host_desc, (payload_size & 0xFFF));

  // Fill frame type
  host_desc[1] |= (RSI_WLAN_MGMT_Q << 4);

  // Fill frame type
  host_desc[2] = cmd;

  // Enqueue packet to WLAN TX queue
  rsi_enqueue_pkt(&rsi_driver_cb->wlan_tx_q, pkt);

  // Set TX packet pending event
  rsi_set_event(RSI_TX_EVENT);

  // Return status
  return status;
}

/*==============================================*/
/**
 * @fn          uint8_t *rsi_extract_filename(uint8_t *json, uint8_t *buffer)
 * @brief       Extract filename out of the received json update data.
 * @param[in]   json   - json object data string 
 * @param[in]   buffer - contains file name 
 * @return      Returns file name extracted 
 *
 *
 */

/* This function is used to extract filename out of the received json update data. */
uint8_t *rsi_extract_filename(uint8_t *json, uint8_t *buffer)
{
  uint8_t c;
  uint8_t i = 0;

  while (i < 24) {

    /* Get character */
    c = json[i];

    if (c != ',')
      buffer[i] = c;
    else
      break;

    ++i;
  }

  buffer[i] = '\0';

  return &json[i + 1];
}

/*==============================================*/
/**
 * @fn          int32_t rsi_driver_process_wlan_recv_cmd(rsi_pkt_t *pkt)
 * @brief       Process WLAN receive commands. 
 * @param[in]   pkt - Pointer to received RX packet  
 * @return      0              - Success \n
 *              Non-Zero Value - Failure
 *              
 * @note      Memory allocation for the pointer is from receive handler, after process it will be freed. 
 *
 */
/// @private
int32_t rsi_driver_process_wlan_recv_cmd(rsi_pkt_t *pkt)
{
  uint8_t cmd_type   = 0;
  uint8_t ip_version = 0, sock_id = 0, accept_sock_id = 0, socket_type = 0;
  int32_t sockID                             = 0;
  int32_t status                             = RSI_SUCCESS;
  uint16_t copy_length                       = 0;
  uint8_t *host_desc                         = NULL;
  uint8_t *payload                           = NULL;
  uint16_t payload_length                    = 0;
  uint16_t length                            = 0;
  uint8_t buffers_freed                      = 0;
  int8_t wlan_pkt_pending                    = 0;
  uint8_t type                               = 0;
  uint8_t i                                  = 0;
  uint8_t j                                  = 0;
  rsi_rsp_socket_select_t *socket_select_rsp = NULL;
  uint16_t status_code                       = 0;
#ifdef PROCESS_SCAN_RESULTS_AT_HOST
  uint16_t recv_freq = 0;
  int8_t rssi        = 0;
  uint8_t channel    = 0;
#endif
  // Get wlan cb struct pointer
  rsi_wlan_cb_t *rsi_wlan_cb = rsi_driver_cb->wlan_cb;

  // Get common cb pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  // Get host descriptor
  host_desc = pkt->desc;

  // Get command type
  cmd_type = pkt->desc[2];

  // Get payload pointer
  payload = pkt->data;

  // Get payoad length
  payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);

  // check status
  status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);

  // check for packet pending in wlan queue
  wlan_pkt_pending = rsi_check_queue_status(&rsi_driver_cb->wlan_tx_q);

  switch (cmd_type) {
    case RSI_WLAN_RSP_DNS_UPDATE: {
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_dns_update_rsp_handler != NULL) {
        rsi_wlan_cb_non_rom->nwk_callbacks.rsi_dns_update_rsp_handler(status);
        rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending &= ~(DNS_RESPONSE_PENDING);
      }
    } break;
    case RSI_WLAN_RSP_BAND: {
      // if success, update state  wlan_cb state to band done
      if (status == RSI_SUCCESS) {
        rsi_wlan_cb->state = RSI_WLAN_STATE_BAND_DONE;
      }
    } break;
    case RSI_WLAN_RSP_SELECT_REQUEST: {
      rsi_fd_set *fd_read                        = NULL;
      rsi_fd_set *fd_write                       = NULL;
      rsi_fd_set *fd_except                      = NULL;
      rsi_rsp_socket_select_t *socket_select_rsp = (rsi_rsp_socket_select_t *)payload;
      j                                          = socket_select_rsp->select_id;
      if (rsi_socket_select_info[j].sock_select_callback == NULL) {
        fd_read  = (rsi_fd_set *)rsi_socket_select_info[j].rsi_sel_read_fds;
        fd_write = (rsi_fd_set *)rsi_socket_select_info[j].rsi_sel_write_fds;
      } else {
        fd_read   = &rsi_socket_select_info[j].rsi_sel_read_fds_var;
        fd_write  = &rsi_socket_select_info[j].rsi_sel_write_fds_var;
        fd_except = &rsi_socket_select_info[j].rsi_sel_exception_fds_var;
      }
      if (fd_read != NULL) {
        //clear the read fd set
        RSI_FD_ZERO(fd_read);
        fd_read->fd_count = 0;
      }
      if (fd_write != NULL) {
        //clear the write fd set
        RSI_FD_ZERO(fd_write);
        fd_write->fd_count = 0;
      }
      if (fd_except != NULL) {
        //clear the except fd set
        RSI_FD_ZERO(fd_except);
        fd_except->fd_count = 0;
      }
      if (payload_length > 0) {
        //set the readfd and writefd with the updated value from firmware response
        for (i = 0; i < NUMBER_OF_SOCKETS; i++) {
          //set the readfd
          if ((socket_select_rsp->rsi_read_fds.fd_array[0])) {
            if ((socket_select_rsp->rsi_read_fds.fd_array[0]) & BIT(rsi_socket_pool[i].sock_id)) {
              fd_read->fd_count++;
              RSI_FD_SET(i, fd_read);
            }
          }
          //set the writefd
          if ((socket_select_rsp->rsi_write_fds.fd_array[0])) {
            if ((socket_select_rsp->rsi_write_fds.fd_array[0]) & BIT(rsi_socket_pool[i].sock_id)) {
              fd_write->fd_count++;
              RSI_FD_SET(i, fd_write);
            }
          }
          //set the terminated bit in socket pool
          if ((socket_select_rsp->socket_terminate_bitmap) & BIT(rsi_socket_pool[i].sock_id)) {
            rsi_socket_pool_non_rom[i].socket_terminate_indication = 1;
          }
        }
      }
      //call back
      if (rsi_socket_select_info[j].sock_select_callback != NULL) {
        rsi_socket_select_info[j].sock_select_callback(
          (rsi_fd_set *)&rsi_socket_select_info[j].rsi_sel_read_fds_var,
          (rsi_fd_set *)&rsi_socket_select_info[j].rsi_sel_write_fds_var,
          (rsi_fd_set *)&rsi_socket_select_info[j].rsi_sel_exception_fds_var,
          status);
        rsi_socket_select_info[j].sock_select_callback = NULL;
        rsi_socket_select_info[j].select_state         = RSI_SOCKET_SELECT_STATE_INIT;
        return RSI_SUCCESS;
      }

    } break;

    case RSI_WLAN_RSP_INIT: {

      // update state  wlan_cb state to init done
      if (status == RSI_SUCCESS) {
        rsi_wlan_cb->state      = RSI_WLAN_STATE_INIT_DONE;
        common_cb->ps_coex_mode = (common_cb->ps_coex_mode | BIT(0));
      }

    } break;
    case RSI_WLAN_RSP_WPS_METHOD: {

      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy generated wps pin
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                          : (rsi_wlan_cb->app_buffer_length);
          memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
          rsi_wlan_cb->app_buffer = NULL;
        }
      }

    } break;
    case RSI_WLAN_RSP_CONNECTION_STATUS: {

      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy connection status
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                          : (rsi_wlan_cb->app_buffer_length);
          memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
          rsi_wlan_cb->app_buffer = NULL;
        }
      }

    } break;
    case RSI_WLAN_RSP_RSSI: {

      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy rssi value
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                          : (rsi_wlan_cb->app_buffer_length);
          memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
          rsi_wlan_cb->app_buffer = NULL;
        }
      }

    } break;
    case RSI_WLAN_RSP_FW_VERSION: {

      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy firmware version
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                          : (rsi_wlan_cb->app_buffer_length);
          memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
          rsi_wlan_cb->app_buffer = NULL;
        }
      }

    } break;
    case RSI_WLAN_RSP_MAC_ADDRESS: {

      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy mac address
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                          : (rsi_wlan_cb->app_buffer_length);
          memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
          rsi_wlan_cb->app_buffer = NULL;
        }
      }

    } break;
    case RSI_WLAN_RSP_QUERY_NETWORK_PARAMS: {

      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy scan results
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {

          if (rsi_wlan_cb->query_cmd == RSI_WLAN_INFO) {
            // copy wlan related information in to output buffer
            if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
              copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? sizeof(rsi_rsp_wireless_info_t)
                                                                              : (rsi_wlan_cb->app_buffer_length);

              ((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->wlan_state =
                (uint16_t)((rsi_rsp_nw_params_t *)payload)->wlan_state;
              ((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->channel_number =
                (uint16_t)((rsi_rsp_nw_params_t *)payload)->channel_number;
              memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ssid,
                     ((rsi_rsp_nw_params_t *)payload)->ssid,
                     RSI_SSID_LEN);
              memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->mac_address,
                     ((rsi_rsp_nw_params_t *)payload)->mac_address,
                     6);
              ((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->sec_type =
                ((rsi_rsp_nw_params_t *)payload)->sec_type;
              memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->psk,
                     ((rsi_rsp_nw_params_t *)payload)->psk,
                     64);
              memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ipv4_address,
                     ((rsi_rsp_nw_params_t *)payload)->ipv4_address,
                     4);
              memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ipv6_address,
                     ((rsi_rsp_nw_params_t *)payload)->ipv6_address,
                     16);
            }

          } else if (rsi_wlan_cb->query_cmd == RSI_SOCKETS_INFO) {
            if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
              copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? sizeof(rsi_rsp_sockets_info_t)
                                                                              : (rsi_wlan_cb->app_buffer_length);

              memcpy(((rsi_rsp_sockets_info_t *)rsi_wlan_cb->app_buffer)->num_open_socks,
                     ((rsi_rsp_nw_params_t *)payload)->num_open_socks,
                     2);
              memcpy(((rsi_rsp_sockets_info_t *)rsi_wlan_cb->app_buffer)->socket_info,
                     ((rsi_rsp_nw_params_t *)payload)->socket_info,
                     sizeof(rsi_sock_info_query_t) * RSI_MN_NUM_SOCKETS);
            }
          }
          rsi_wlan_cb->app_buffer = NULL;
        }
      }
    } break;
    case RSI_WLAN_RSP_GET_STATS: {
      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy scan results
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {

          if (rsi_wlan_cb->query_cmd == RSI_GET_WLAN_STATS) {
            // copy wlan related information in to output buffer
            if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
              copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? sizeof(rsi_rsp_wireless_info_t)
                                                                              : (rsi_wlan_cb->app_buffer_length);

              memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
            }
          }
        }
        rsi_wlan_cb->app_buffer = NULL;
      }
    } break;
    case RSI_WLAN_RSP_EXT_STATS: {
      if (status == RSI_SUCCESS) {
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {

          if (rsi_wlan_cb->query_cmd == RSI_WLAN_EXT_STATS) {
            // copy wlan related information in to output buffer
            if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
              copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? sizeof(rsi_wlan_ext_stats_t)
                                                                              : (rsi_wlan_cb->app_buffer_length);

              memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
            }
          }
        }
        rsi_wlan_cb->app_buffer = NULL;
      }
    } break;
    case RSI_WLAN_RSP_QUERY_GO_PARAMS: {

      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy scan results
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {

          if (rsi_wlan_cb->query_cmd == RSI_WLAN_INFO) {

            if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
              copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? sizeof(rsi_rsp_wireless_info_t)
                                                                              : (rsi_wlan_cb->app_buffer_length);

              ((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->wlan_state =
                rsi_bytes2R_to_uint16(((rsi_rsp_go_params_t *)payload)->sta_count);
              ((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->channel_number =
                rsi_bytes2R_to_uint16(((rsi_rsp_go_params_t *)payload)->channel_number);
              memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ssid,
                     ((rsi_rsp_go_params_t *)payload)->ssid,
                     RSI_SSID_LEN);
              memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->mac_address,
                     ((rsi_rsp_go_params_t *)payload)->mac_address,
                     6);
              memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->psk,
                     ((rsi_rsp_go_params_t *)payload)->psk,
                     64);
              memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ipv4_address,
                     ((rsi_rsp_go_params_t *)payload)->ipv4_address,
                     4);
              memcpy(((rsi_rsp_wireless_info_t *)rsi_wlan_cb->app_buffer)->ipv6_address,
                     ((rsi_rsp_go_params_t *)payload)->ipv6_address,
                     16);
            }
          } else if (rsi_wlan_cb->query_cmd == RSI_STATIONS_INFO) {

            if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
              copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? sizeof(rsi_rsp_wireless_info_t)
                                                                              : (rsi_wlan_cb->app_buffer_length);

              memcpy(((rsi_rsp_stations_info_t *)rsi_wlan_cb->app_buffer)->sta_count,
                     ((rsi_rsp_go_params_t *)payload)->sta_count,
                     2);
              memcpy(((rsi_rsp_stations_info_t *)rsi_wlan_cb->app_buffer)->sta_info,
                     ((rsi_rsp_go_params_t *)payload)->sta_info,
                     sizeof(rsi_go_sta_info_t) * RSI_MAX_STATIONS);
            }
          }
          rsi_wlan_cb->app_buffer = NULL;
        }
      }
    } break;
    case RSI_WLAN_RSP_SMTP_CLIENT: {
      //Changing the nwk state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

      if ((host_desc[5] == RSI_SMTP_CLIENT_MAIL_SEND)
          && (rsi_wlan_cb_non_rom->nwk_callbacks.smtp_client_mail_response_handler != NULL)) {
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->nwk_callbacks.smtp_client_mail_response_handler(status, host_desc[5]);
      } else if ((host_desc[5] == RSI_SMTP_CLIENT_DEINIT)
                 && (rsi_wlan_cb_non_rom->nwk_callbacks.smtp_client_delete_response_handler != NULL)) {
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->nwk_callbacks.smtp_client_delete_response_handler(status, host_desc[5]);
      }
      rsi_wlan_set_nwk_status(status);
      return RSI_SUCCESS;
    }
      // no break
    case RSI_WLAN_RSP_P2P_CONNECTION_REQUEST: {
      if (rsi_wlan_cb_non_rom->callback_list.wlan_wfd_connection_request_notify_handler != NULL) {
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.wlan_wfd_connection_request_notify_handler(status, payload, payload_length);
        return RSI_SUCCESS;
      }
    } break;
    case RSI_WLAN_RSP_WFD_DEVICE: {
      if (rsi_wlan_cb_non_rom->callback_list.wlan_wfd_discovery_notify_handler != NULL) {
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.wlan_wfd_discovery_notify_handler(status, payload, payload_length);
        return RSI_SUCCESS;
      }
    } break;
    case RSI_WLAN_RSP_CLIENT_CONNECTED: {
      if (rsi_wlan_cb_non_rom->callback_list.stations_connect_notify_handler != NULL) {
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.stations_connect_notify_handler(status, payload, payload_length);
        return RSI_SUCCESS;
      }
    } break;
    case RSI_WLAN_RSP_CLIENT_DISCONNECTED: {
      if (rsi_wlan_cb_non_rom->callback_list.stations_disconnect_notify_handler != NULL) {
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.stations_disconnect_notify_handler(status, payload, payload_length);
        return RSI_SUCCESS;
      }
    } break;
    case RSI_WLAN_RSP_SCAN: {
      if (rsi_wlan_cb_non_rom->scan_results_to_host) {
        rsi_wlan_cb_non_rom->scan_results_to_host = 0;
        if (scan_results_array->num == 0) {
          status = RSI_AP_NOT_FOUND;
        }
      }
      // update state  wlan_cb state to scan done
      if (status == RSI_SUCCESS) {
        // Check for AP mode, and don't update scan done state
        if (rsi_wlan_cb->opermode != RSI_WLAN_ACCESS_POINT_MODE) {
          rsi_wlan_cb->state = RSI_WLAN_STATE_SCAN_DONE;
        }

        if (rsi_wlan_cb_non_rom->callback_list.wlan_scan_response_handler != NULL) {

          // Call asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->callback_list.wlan_scan_response_handler(status, payload, payload_length);
          rsi_wlan_cb_non_rom->callback_list.wlan_scan_response_handler = NULL;

          return RSI_SUCCESS;
        } else {
          // check the length of application buffer and copy scan results
          if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
            copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                            : (rsi_wlan_cb->app_buffer_length);
            memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
            rsi_wlan_cb->app_buffer = NULL;
          }
        }
      } else {
        if (rsi_wlan_cb_non_rom->callback_list.wlan_scan_response_handler != NULL) {
          // Call asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->callback_list.wlan_scan_response_handler(status, NULL, 0);
          rsi_wlan_cb_non_rom->callback_list.wlan_scan_response_handler = NULL;
          return RSI_SUCCESS;
        }
      }
      // Check for Auto config state
      if (rsi_wlan_cb->auto_config_state == RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON) {
        // Set auto config state to failed
        rsi_driver_cb->wlan_cb->auto_config_state = RSI_WLAN_STATE_AUTO_CONFIG_FAILED;
        rsi_wlan_cb->state                        = RSI_WLAN_STATE_SCAN_DONE;

        if (rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler) {
          rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler(status, RSI_AUTO_CONFIG_FAILED);
        }
      }
    } break;
    case RSI_WLAN_RSP_BG_SCAN: {
      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy bg scan results
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                          : (rsi_wlan_cb->app_buffer_length);
          memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
          rsi_wlan_cb->app_buffer = NULL;
        }
      }
    } break;
    case RSI_WLAN_RSP_JOIN: {
      // update state  wlan_cb state to connected
      if (status == RSI_SUCCESS) {
        if (rsi_wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
          rsi_wlan_cb->state = RSI_WLAN_STATE_IP_CONFIG_DONE;
        } else {
          rsi_wlan_cb->state = RSI_WLAN_STATE_CONNECTED;
        }
        // Check for Auto config done
        if (host_desc[15] & RSI_AUTO_CONFIG_DONE) {
          // Set auto config state to done
          rsi_driver_cb->wlan_cb->auto_config_state = RSI_WLAN_STATE_AUTO_CONFIG_DONE;

          if (rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler) {
            rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler(status, RSI_AUTO_CONFIG_DONE);
          }
        }

      }
      // Check for Auto config state
      else if (rsi_wlan_cb->auto_config_state == RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON) {
        rsi_driver_cb->wlan_cb->state = RSI_WLAN_STATE_BAND_DONE;
        // Set auto config state to failed
        rsi_driver_cb->wlan_cb->auto_config_state = RSI_WLAN_STATE_AUTO_CONFIG_FAILED;

        if (rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler) {
          rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler(status, RSI_AUTO_CONFIG_FAILED);
        }
      } else {

        //Reset powe save mode as join fails
        common_cb->power_save.current_ps_mode = 0;

        // Reset module state as join fails
        common_cb->power_save.module_state = 0;

        if (status != RSI_WPS_NOT_SUPPORTED) {
          rsi_wlan_cb->state = RSI_WLAN_STATE_BAND_DONE;
          if (rsi_wlan_cb->expected_response != cmd_type) {
            if (rsi_wlan_cb_non_rom->callback_list.join_fail_handler != NULL) {
              // Call asynchronous response handler to indicate to host
              rsi_wlan_cb_non_rom->callback_list.join_fail_handler(status, payload, payload_length);
              rsi_wlan_cb_non_rom->callback_list.join_fail_handler = NULL;
            }

            if ((wlan_pkt_pending == 0) && rsi_wlan_check_waiting_wlan_cmd()) {
#ifndef RSI_WLAN_SEM_BITMAP
              rsi_driver_cb_non_rom->wlan_wait_bitmap &= ~BIT(1);
#endif
              rsi_semaphore_post(&rsi_driver_cb_non_rom->wlan_cmd_send_sem);
            }
            if (rsi_driver_cb_non_rom->wlan_wait_bitmap & BIT(0)) {
#ifndef RSI_WLAN_SEM_BITMAP
              rsi_driver_cb_non_rom->wlan_wait_bitmap &= ~BIT(0);
#endif
              rsi_semaphore_post(&rsi_driver_cb_non_rom->wlan_cmd_sem);
            }
            rsi_post_waiting_semaphore();
            rsi_wlan_set_status(status);
            return RSI_SUCCESS;
          }
        }
      }
      if (rsi_wlan_cb_non_rom->callback_list.wlan_join_response_handler != NULL) {
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.wlan_join_response_handler(status, payload, payload_length);
        rsi_wlan_cb_non_rom->callback_list.wlan_join_response_handler = NULL;

        // Clear expected response
        rsi_wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;
        rsi_wlan_set_status(status);
        return RSI_SUCCESS;
      }
    } break;
    case RSI_WLAN_RSP_IPCONFV6:
    case RSI_WLAN_RSP_IPCONFV4: {
      // if success, update state  wlan_cb state to scan done
      if (status == RSI_SUCCESS) {
        if (cmd_type == RSI_WLAN_RSP_IPCONFV4) {
          rsi_wlan_cb->state = RSI_WLAN_STATE_IP_CONFIG_DONE;
        } else {
          rsi_wlan_cb->state = RSI_WLAN_STATE_IPV6_CONFIG_DONE;
        }

        // Check for Auto config done
        if (host_desc[15] & RSI_AUTO_CONFIG_DONE) {
          // Set auto config state to done
          rsi_wlan_cb->auto_config_state = RSI_WLAN_STATE_AUTO_CONFIG_DONE;

          // Call auto config response handler
          if (rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler) {
            rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler(status, RSI_AUTO_CONFIG_DONE);
          }
        }

        // check the length of application buffer and copy scan results
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                          : (rsi_wlan_cb->app_buffer_length);
          memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
          rsi_wlan_cb->app_buffer = NULL;
        }

        // Clear expected response
        rsi_wlan_cb->expected_response = RSI_WLAN_RSP_CLEAR;
      }
      // Check for Auto config state
      else if (rsi_wlan_cb->auto_config_state == RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON) {
        // Set auto config state to failed
        rsi_driver_cb->wlan_cb->auto_config_state = RSI_WLAN_STATE_AUTO_CONFIG_FAILED;

        if (rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler) {
          rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler(status, RSI_AUTO_CONFIG_FAILED);
        }
      } else {
        if (rsi_wlan_cb->expected_response != cmd_type) {
          if (rsi_wlan_cb_non_rom->callback_list.ip_renewal_fail_handler != NULL) {
            // Call asynchronous response handler to indicate to host
            rsi_wlan_cb_non_rom->callback_list.ip_renewal_fail_handler(status, payload, payload_length);
          }

          if (wlan_pkt_pending == 0 && rsi_wlan_check_waiting_wlan_cmd()) {
#ifndef RSI_WLAN_SEM_BITMAP
            rsi_driver_cb_non_rom->wlan_wait_bitmap &= ~BIT(1);
#endif
            rsi_semaphore_post(&rsi_driver_cb_non_rom->wlan_cmd_send_sem);
          }
          // Post the semaphore which is waiting on socket/wlan/nwk
          rsi_post_waiting_semaphore();

          return RSI_SUCCESS;
        }
      }
    } break;
    case RSI_WLAN_RSP_GET_RANDOM: {
      // if success, update state  wlan_cb state to scan done
      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy scan results
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                          : (rsi_wlan_cb->app_buffer_length);
          memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
        }
      }
      rsi_wlan_cb->app_buffer = NULL;
    } break;
    case RSI_WLAN_RSP_PING_PACKET: {
      if (rsi_wlan_cb_non_rom->callback_list.wlan_ping_response_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.wlan_ping_response_handler(status, payload, payload_length);
        rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending &= ~(PING_RESPONSE_PENDING);
        return RSI_SUCCESS;
      }
    } break;
    case RSI_WLAN_RSP_RX_STATS: {
      if (rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler != NULL) {
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler(status, payload, payload_length);
        return RSI_SUCCESS;
      }
    } break;
    case RSI_WLAN_RSP_DISCONNECT: {
      // if success, update state  wlan_cb state to band done
      if (status == RSI_SUCCESS) {
#if (RSI_HAND_SHAKE_TYPE == MSG_BASED)
        if (common_cb->power_save.module_state == RSI_SLP_ACK_SENT) {
          rsi_unmask_event(RSI_TX_EVENT);
        }
#endif
#ifdef CONC_STA_AP_DYN_SWITCH_EN
        if (rsi_wlan_cb->opermode != RSI_WLAN_CONCURRENT_MODE)
          rsi_wlan_cb->state = RSI_WLAN_STATE_BAND_DONE;
        else
          rsi_wlan_cb->state = RSI_WLAN_STATE_INIT_DONE;
#else
        rsi_wlan_cb->state = RSI_WLAN_STATE_BAND_DONE;
#endif
        //Reset powe save mode as Disconnect received
        common_cb->power_save.current_ps_mode = 0;

        // Reset module state as Disconnect received
        common_cb->power_save.module_state = 0;

        // Post the semaphore which is waiting on socket/wlan/nwk
        rsi_post_waiting_semaphore();
      }
    } break;
    case RSI_WLAN_RSP_DYNAMIC_POOL: {
      // check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);
    } break;

    case RSI_WLAN_RSP_SOCKET_CREATE: {
      rsi_rsp_socket_create_t *socket_rsp = (rsi_rsp_socket_create_t *)payload;

      if (rsi_bytes2R_to_uint16(socket_rsp->socket_type) == RSI_SOCKET_TCP_SERVER) {
        // Get waiting socket ID
        sockID = rsi_get_app_socket_descriptor(socket_rsp->module_port);
      } else {
        // Get IP version
        ip_version = rsi_bytes2R_to_uint16(socket_rsp->ip_version);

        if (ip_version == 4) {
          sockID = rsi_get_socket_descriptor(socket_rsp->module_port,
                                             socket_rsp->dst_port,
                                             socket_rsp->dest_ip_addr.ipv4_addr,
                                             ip_version,
                                             rsi_bytes2R_to_uint16(socket_rsp->socket_id));
        } else {
          sockID = rsi_get_socket_descriptor(socket_rsp->module_port,
                                             socket_rsp->dst_port,
                                             socket_rsp->dest_ip_addr.ipv6_addr,
                                             ip_version,
                                             rsi_bytes2R_to_uint16(socket_rsp->socket_id));
        }
      }
      if (sockID < 0 || (sockID >= RSI_NUMBER_OF_SOCKETS)) {
        break;
      }
      if (status == RSI_SUCCESS) {
        sock_id     = rsi_bytes2R_to_uint16(socket_rsp->socket_id);
        socket_type = rsi_bytes2R_to_uint16(socket_rsp->socket_type);
        ip_version  = rsi_bytes2R_to_uint16(socket_rsp->ip_version);

        rsi_socket_pool[sockID].source_port      = rsi_bytes2R_to_uint16(socket_rsp->module_port);
        rsi_socket_pool[sockID].destination_port = rsi_bytes2R_to_uint16(socket_rsp->dst_port);
        rsi_socket_pool_non_rom[sockID].mss      = rsi_bytes2R_to_uint16(socket_rsp->mss);

        // Save socket id
        if (socket_type != RSI_SOCKET_TCP_SERVER) {
          rsi_socket_pool[sockID].sock_id = sock_id;
        }
        if (socket_type == RSI_SOCKET_TCP_SERVER) {
          // Update state to listen is success
          rsi_socket_pool[sockID].sock_state = RSI_SOCKET_STATE_LISTEN;

        } else {
          if (!rsi_socket_pool_non_rom[sockID].wait_to_connect) {
            rsi_socket_pool[sockID].sock_state = RSI_SOCKET_STATE_CONNECTED;
          } else {
            rsi_socket_pool_non_rom[sockID].wait_to_connect = 0;
#ifndef RSI_SOCK_SEM_BITMAP
            rsi_socket_pool_non_rom[sockID].socket_wait_bitmap &= ~BIT(0);
#endif
            // Wait on select semaphore
            rsi_semaphore_post(&rsi_socket_pool_non_rom[sockID].socket_sem);
          }
        }
      }
#ifndef RSI_M4_INTERFACE
      if (rsi_wlan_cb_non_rom->callback_list.socket_connect_response_handler != NULL) {
        // Changing the callback response f/w socket id to app socket id.
        rsi_uint16_to_2bytes(socket_rsp->socket_id, sockID);
        rsi_wlan_cb_non_rom->socket_cmd_rsp_pending &= ~BIT(sockID);
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.socket_connect_response_handler(status, payload, payload_length);
      }
#endif

    } break;
    case RSI_WLAN_RSP_UPDATE_TCP_WINDOW: {
      rsi_rsp_tcp_window_update_t *tcp_window = (rsi_rsp_tcp_window_update_t *)payload;
      sockID                                  = rsi_get_application_socket_descriptor(tcp_window->socket_id);
      if ((sockID < 0) || (sockID >= RSI_NUMBER_OF_SOCKETS)) {
        break;
      }
      rsi_socket_pool_non_rom[sockID].window_size = tcp_window->window_size;
      rsi_wlan_socket_set_status(status, sockID);
      if (rsi_socket_pool_non_rom[sockID].socket_wait_bitmap & BIT(0)) {
#ifndef RSI_SOCK_SEM_BITMAP
        rsi_socket_pool_non_rom[sockID].socket_wait_bitmap &= ~BIT(0);
#endif
        // Wait on select semaphore
        rsi_semaphore_post(&rsi_socket_pool_non_rom[sockID].socket_sem);
      }
    } break;
    case RSI_WLAN_RSP_CONN_ESTABLISH: {
      if (status == RSI_SUCCESS) {
        rsi_rsp_ltcp_est_t *ltcp = (rsi_rsp_ltcp_est_t *)payload;

        // Get IP version
        ip_version = rsi_bytes2R_to_uint16(ltcp->ip_version);

        // Get socket descriptor from response
        sock_id = rsi_bytes2R_to_uint16(ltcp->socket_id);

        // Get waiting socket ID
        sockID = rsi_get_app_socket_descriptor(ltcp->src_port_num);
        if (sockID < 0 || (sockID >= RSI_NUMBER_OF_SOCKETS)) {
          break;
        }
        rsi_socket_pool_non_rom[sockID].mss = rsi_bytes2R_to_uint16(ltcp->mss);
        if ((sockID >= 0) && (sockID < NUMBER_OF_SOCKETS)) {
          if (rsi_socket_pool_non_rom[sockID].accept_call_back_handler != NULL) {
            // Get the actual source socket descriptor
            accept_sock_id = rsi_get_primary_socket_id(ltcp->src_port_num);

            // Increase backlog current count
            rsi_socket_pool[accept_sock_id].backlog_current_count++;
          }
        }

        // Update the state as connected
        rsi_socket_pool[sockID].sock_state = RSI_SOCKET_STATE_CONNECTED;
        // Save socket id
        rsi_socket_pool[sockID].sock_id = sock_id;

        // Save destination port number
        rsi_socket_pool[sockID].destination_port = rsi_bytes2R_to_uint16(ltcp->dest_port);

        // Save destination IP addess
        if (ip_version == 4) {
          memcpy(rsi_socket_pool[sockID].destination_ip_addr.ipv4,
                 ltcp->dest_ip_addr.ipv4_address,
                 RSI_IPV4_ADDRESS_LENGTH);
        } else {
          memcpy(rsi_socket_pool[sockID].destination_ip_addr.ipv6,
                 ltcp->dest_ip_addr.ipv6_address,
                 RSI_IPV6_ADDRESS_LENGTH);
        }

        // In case of asynchronous accept call the callback
        if (rsi_socket_pool_non_rom[sockID].accept_call_back_handler != NULL) {
          rsi_socket_pool_non_rom[sockID].accept_call_back_handler(sockID,
                                                                   rsi_socket_pool[sockID].destination_port,
                                                                   rsi_socket_pool[sockID].destination_ip_addr.ipv4,
                                                                   ip_version);
          rsi_wlan_cb_non_rom->socket_cmd_rsp_pending &= ~BIT(sockID);
        }
      }
    } break;
    case RSI_WLAN_RSP_REMOTE_TERMINATE: {
      rsi_rsp_socket_close_t *close = (rsi_rsp_socket_close_t *)payload;

      // Get application socket ID
      sockID = rsi_get_application_socket_descriptor(rsi_bytes2R_to_uint16(close->socket_id));

      // Need to set reason for remote terminate

      if (sockID < 0 || (sockID >= RSI_NUMBER_OF_SOCKETS)) {
        break;
      }
      // Clear socket info
      if (rsi_wlan_cb_non_rom->callback_list.remote_socket_terminate_handler != NULL) {
        // Changing the callback response f/w socket id to app socket id.
        rsi_uint16_to_2bytes(close->socket_id, sockID);
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.remote_socket_terminate_handler(status, payload, payload_length);
      }

      if ((RSI_TCP_IP_FEATURE_BIT_MAP & TCP_IP_FEAT_EXTENSION_VALID)
          && (RSI_EXT_TCPIP_FEATURE_BITMAP & EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE)) {
        rsi_socket_pool_non_rom[sockID].socket_terminate_indication = 1;
        //Do not clear the sock info, RSI_WLAN_RSP_SOCKET_CLOSE will handle
      } else
        rsi_clear_sockets(sockID);

      rsi_wlan_socket_set_status(status, sockID);
      // post on  semaphore
      rsi_post_waiting_socket_semaphore(sockID);
    } break;
    case RSI_WLAN_RSP_SOCKET_CLOSE: {
      rsi_rsp_socket_close_t *close = (rsi_rsp_socket_close_t *)payload;
      if (rsi_bytes2R_to_uint16(close->socket_id)) {
        // Normal socket close
        // Get socket descriptor
        sockID = rsi_get_application_socket_descriptor(rsi_bytes2R_to_uint16(close->socket_id));
      } else {
        sockID = rsi_get_socket_id(rsi_bytes2R_to_uint16(close->port_number), 0);
      }

      if (sockID < 0 || (sockID >= RSI_NUMBER_OF_SOCKETS)) {
        break;
      }

      rsi_wlan_socket_set_status(status, sockID);
      // post on  semaphore
#ifndef RSI_SOCK_SEM_BITMAP
      rsi_post_waiting_socket_semaphore(sockID);
#else
      rsi_semaphore_post(&rsi_socket_pool_non_rom[sockID].socket_sem);
#endif
    } break;

    case RSI_WLAN_RSP_IPV4_CHANGE: {
      if (rsi_wlan_cb_non_rom->callback_list.ip_change_notify_handler != NULL) {
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.ip_change_notify_handler(status, payload, payload_length);
      }

      if (wlan_pkt_pending == 0 && rsi_wlan_check_waiting_wlan_cmd()) {
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap &= ~BIT(1);
#endif
        rsi_semaphore_post(&rsi_driver_cb_non_rom->wlan_cmd_send_sem);
      }
      // Post the semaphore which is waiting on socket/wlan/nwk
      rsi_post_waiting_semaphore();

      return RSI_SUCCESS;
    }
      // no break
    case RSI_WLAN_RSP_HTTP_CLIENT_POST:
    case RSI_WLAN_RSP_HTTP_CLIENT_GET: {
      if (rsi_wlan_cb_non_rom->nwk_callbacks.http_client_response_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        // more data
        uint16_t moredata = rsi_bytes2R_to_uint16(payload);
#if RSI_HTTP_STATUS_INDICATION_EN
        // HTTP status code
        uint16_t status_code = rsi_bytes2R_to_uint16(payload + 2);
#else
        USED_PARAMETER(status_code);
#endif

        // Adjust payload length
        payload_length -= RSI_HTTP_OFFSET;

        if (status == RSI_SUCCESS) {
          // Call asynchronous response handler to indicate to host
#if RSI_HTTP_STATUS_INDICATION_EN
          rsi_wlan_cb_non_rom->nwk_callbacks.http_client_response_handler(status,
                                                                          (payload + RSI_HTTP_OFFSET),
                                                                          payload_length,
                                                                          (uint32_t)moredata,
                                                                          status_code);
#else
          rsi_wlan_cb_non_rom->nwk_callbacks.http_client_response_handler(status,
                                                                          (payload + RSI_HTTP_OFFSET),
                                                                          payload_length,
                                                                          moredata);
#endif
        } else {
          // Call asynchronous response handler to indicate to host
#if RSI_HTTP_STATUS_INDICATION_EN
          rsi_wlan_cb_non_rom->nwk_callbacks.http_client_response_handler(status,
                                                                          (payload + RSI_HTTP_OFFSET),
                                                                          payload_length,
                                                                          0,
                                                                          0);
#else
          rsi_wlan_cb_non_rom->nwk_callbacks.http_client_response_handler(status,
                                                                          (payload + RSI_HTTP_OFFSET),
                                                                          payload_length,
                                                                          0);
#endif
        }
      }
      rsi_wlan_set_nwk_status(status);
      return RSI_SUCCESS;
    }

    case RSI_WLAN_RSP_HTTP_CLIENT_POST_DATA: {
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_post_data_response_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        if (status == RSI_SUCCESS) {
          // more data
          uint16_t moredata = rsi_bytes2R_to_uint16(payload);
          // HTTP status code
#if RSI_HTTP_STATUS_INDICATION_EN
          // HTTP status code
          uint16_t status_code = rsi_bytes2R_to_uint16(payload + 2);
#else
          USED_PARAMETER(status_code);
#endif

          // Adjust payload length
          payload_length -= RSI_HTTP_OFFSET;

          // Call asynchronous response handler to indicate to host
#if RSI_HTTP_STATUS_INDICATION_EN
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_post_data_response_handler(status,
                                                                                        (payload + RSI_HTTP_OFFSET),
                                                                                        payload_length,
                                                                                        (uint32_t)moredata,
                                                                                        status_code);
#else
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_post_data_response_handler(status,
                                                                                        (payload + RSI_HTTP_OFFSET),
                                                                                        payload_length,
                                                                                        moredata);
#endif
        } else {
          // Call asynchronous response handler to indicate to host
#if RSI_HTTP_STATUS_INDICATION_EN
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_post_data_response_handler(status,
                                                                                        payload,
                                                                                        payload_length,
                                                                                        0,
                                                                                        0);
#else
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_post_data_response_handler(status,
                                                                                        payload,
                                                                                        payload_length,
                                                                                        0);
#endif
        }
      }
      rsi_wlan_set_nwk_status(status);
      return RSI_SUCCESS;
    }
    case RSI_WLAN_RSP_HTTP_OTAF: {

      if (rsi_wlan_cb_non_rom->nwk_callbacks.http_otaf_response_handler != NULL) {
        // more data
        //  uint16_t moredata = rsi_bytes2R_to_uint16(payload);
        // HTTP status code
        //uint16_t status_code = rsi_bytes2R_to_uint16(payload + 2);

        // Adjust payload length
        payload_length -= RSI_HTTP_OFFSET;
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        if (status == RSI_SUCCESS) {
          // Call asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.http_otaf_response_handler(status, (payload + RSI_HTTP_OFFSET));
        } else {
          // Call asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.http_otaf_response_handler(status, (payload + RSI_HTTP_OFFSET));
        }
      }
      return RSI_SUCCESS;
    }
      // no break
    case RSI_WLAN_RSP_URL_REQUEST: {
      rsi_urlReqFrameRcv *postcontent = (rsi_urlReqFrameRcv *)payload;
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_webpage_request_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        if (status == RSI_SUCCESS) {
          // Call asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_webpage_request_handler(type,
                                                                         postcontent->url_name,
                                                                         postcontent->post_data,
                                                                         (uintptr_t)postcontent->post_content_length,
                                                                         status);
          // to avoid compiler warning replace uint32_t with uintptr_t
        }
      }
      return RSI_SUCCESS;
    }
      // no break
    case RSI_WLAN_RSP_DNS_QUERY: {

      if (status == RSI_SUCCESS) {
        if ((rsi_driver_cb_non_rom->nwk_app_buffer != NULL) && (rsi_driver_cb_non_rom->nwk_app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_driver_cb_non_rom->nwk_app_buffer_length)
                          ? (payload_length)
                          : (rsi_driver_cb_non_rom->nwk_app_buffer_length);
          memcpy(rsi_driver_cb_non_rom->nwk_app_buffer, payload, copy_length);
        }
      }
      rsi_driver_cb_non_rom->nwk_app_buffer = NULL;

    } break;
    case RSI_WLAN_RSP_FTP: {
      rsi_ftp_file_rsp_t *ftp_file_rsp = (rsi_ftp_file_rsp_t *)payload;

      if (ftp_file_rsp->command_type == RSI_FTP_FILE_READ) {
        if (status == RSI_SUCCESS) {
          // Call asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.ftp_file_read_call_back_handler(status,
                                                                             ftp_file_rsp->data_content,
                                                                             ftp_file_rsp->data_length,
                                                                             !(ftp_file_rsp->more));
          if (!ftp_file_rsp->more) {
            //Changing the nwk state to allow
            rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
          }
        } else {
          //Changing the nwk state to allow
          rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
          // Call asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.ftp_file_read_call_back_handler(status, NULL, 0, 1);
        }
        return RSI_SUCCESS;
      } else if (ftp_file_rsp->command_type == RSI_FTP_DIRECTORY_LIST) {
        if (status == RSI_SUCCESS) {
          // Call asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.ftp_directory_list_call_back_handler(status,
                                                                                  ftp_file_rsp->data_content,
                                                                                  ftp_file_rsp->data_length,
                                                                                  !(ftp_file_rsp->more));
          if (!ftp_file_rsp->more) {
            //Changing the nwk state to allow
            rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
          }
        } else {
          //Changing the nwk state to allow
          rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
          // Call asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.ftp_directory_list_call_back_handler(status, NULL, 0, 1);
        }
        return RSI_SUCCESS;
      }

    } break;

    case RSI_WLAN_RSP_TCP_ACK_INDICATION: {
      rsi_rsp_tcp_ack_t *tcp_ack = (rsi_rsp_tcp_ack_t *)payload;

      // Get the socket ID from response and get application sockID from it
      sockID = rsi_get_application_socket_descriptor(tcp_ack->socket_id);

      if (sockID < 0 || (sockID >= RSI_NUMBER_OF_SOCKETS)) {
        break;
      }

      // Get the length from response
      length = rsi_bytes2R_to_uint16(tcp_ack->length);

      // Calculate the buffers can be freed with this length
      buffers_freed = calculate_buffers_required(rsi_socket_pool[sockID].sock_type, length);

      // Increase the current available buffer count
      rsi_socket_pool[sockID].current_available_buffer_count += buffers_freed;

      // If current count exceeds max count
      if (rsi_socket_pool[sockID].current_available_buffer_count > rsi_socket_pool[sockID].max_available_buffer_count) {
        rsi_socket_pool[sockID].current_available_buffer_count = rsi_socket_pool[sockID].max_available_buffer_count;
      }

      // Call registered callback
      if (rsi_wlan_cb_non_rom->nwk_callbacks.data_transfer_complete_handler) {
        rsi_wlan_cb_non_rom->nwk_callbacks.data_transfer_complete_handler(sockID, length);
      }

      rsi_wlan_socket_set_status(status, sockID);
#ifndef RSI_SOCK_SEM_BITMAP
      rsi_socket_pool_non_rom[sockID].socket_wait_bitmap &= ~BIT(2);
#endif
      // Wait on select semaphore
      rsi_semaphore_post(&rsi_socket_pool_non_rom[sockID].sock_send_sem);

    } break;
    case RSI_WLAN_RSP_WIRELESS_FWUP_OK:
    case RSI_WLAN_RSP_WIRELESS_FWUP_DONE: {
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_wireless_fw_upgrade_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        rsi_wlan_cb_non_rom->nwk_callbacks.rsi_wireless_fw_upgrade_handler(cmd_type, status);

        return RSI_SUCCESS;
      }
    } break;

    case RSI_WLAN_RSP_DHCP_USER_CLASS: {
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_dhcp_usr_cls_rsp_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        rsi_wlan_cb_non_rom->nwk_callbacks.rsi_dhcp_usr_cls_rsp_handler(status);
      }
    } break;

    case RSI_WLAN_RSP_JSON_UPDATE: {
      uint8_t filename[24] = { '\0' };

      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_json_object_update_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

        // Extract the filename from the received json object data payload
        rsi_extract_filename(payload, filename);

        rsi_wlan_cb_non_rom->nwk_callbacks.rsi_json_object_update_handler(filename,
                                                                          payload,
                                                                          strlen((const char *)payload),
                                                                          RSI_SUCCESS);
      }
    } break;
    case RSI_WLAN_RSP_SOCKET_READ_DATA: {
      // we get the socket id in the response payload if it is error
      sockID = rsi_get_application_socket_descriptor(payload[0]);
      if (sockID < 0 || (sockID >= RSI_NUMBER_OF_SOCKETS)) {
        break;
      }
#ifdef RSI_PROCESS_MAX_RX_DATA
      if ((rsi_socket_pool[sockID].recv_buffer_length) && (rsi_socket_pool[sockID].sock_type & SOCK_STREAM)) {
        rsi_wlan_socket_set_status(0, sockID);
      } else {
        rsi_wlan_socket_set_status(status, sockID);
      }
#else
      rsi_wlan_socket_set_status(status, sockID);
#endif
#ifndef RSI_SOCK_SEM_BITMAP
      rsi_socket_pool_non_rom[sockID].socket_wait_bitmap &= ~BIT(1);
#endif
      // Wait on select semaphore
      rsi_semaphore_post(&rsi_socket_pool_non_rom[sockID].sock_recv_sem);
    } break;
    case RSI_WLAN_RSP_JSON_EVENT: {
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_json_object_event_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        rsi_wlan_cb_non_rom->nwk_callbacks.rsi_json_object_event_handler(RSI_SUCCESS,
                                                                         payload,
                                                                         strlen((const char *)payload));
      }
    } break;
    case RSI_WLAN_RSP_OTA_FWUP: {
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_ota_fw_up_response_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        if (status == RSI_SUCCESS) {
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_ota_fw_up_response_handler(status, 0);
        } else {
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_ota_fw_up_response_handler(status, *(uint16_t *)payload);
        }

        return RSI_SUCCESS;
      }

    } break;
    case RSI_WLAN_RSP_HTTP_CLIENT_PUT: {
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_put_response_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        if (status == RSI_SUCCESS) {
          uint8_t end_of_file                        = 0;
          uint8_t http_cmd_type                      = *payload;
          rsi_http_client_put_pkt_rsp_t *put_pkt_rsp = (rsi_http_client_put_pkt_rsp_t *)payload;
          http_Put_Data_t *http_put_pkt              = (http_Put_Data_t *)payload;

          // Check for HTTP_CLIENT_PUT_PKT command
          if (http_cmd_type == HTTP_CLIENT_PUT_PKT) {
            payload_length = 0;
            end_of_file    = put_pkt_rsp->end_of_file;
          }
          // Check for HTTP Client PUT response from server
          else if (http_cmd_type == HTTP_CLIENT_PUT_OFFSET_PKT) {
            payload_length = http_put_pkt->data_len;
            end_of_file    = *(payload + 4);
          }
          // Call HTTP client PUT asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_put_response_handler(status,
                                                                                  (uint8_t)*payload,
                                                                                  payload + RSI_HTTP_PUT_OFFSET,
                                                                                  payload_length,
                                                                                  end_of_file);
        } else {
          // Call POP3 client asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_http_client_put_response_handler(status,
                                                                                  host_desc[5],
                                                                                  payload + RSI_HTTP_PUT_OFFSET,
                                                                                  payload_length,
                                                                                  0);
        }
        rsi_wlan_set_nwk_status(status);
        return RSI_SUCCESS;
      }
    } break;
    case RSI_WLAN_RSP_GET_PROFILE: {
      // check the length of application buffer and copy get profile response
      if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
        copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                        : (rsi_wlan_cb->app_buffer_length);
        memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
        rsi_wlan_cb->app_buffer = NULL;
      }
    } break;
    case RSI_WLAN_RSP_POP3_CLIENT: {
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_pop3_client_mail_response_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        if (status == RSI_SUCCESS) {
          // Call POP3 client asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_pop3_client_mail_response_handler(status, (uint8_t)*payload, payload);
        } else {
          // Call POP3 client asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_pop3_client_mail_response_handler(status, host_desc[5], payload);
        }
        rsi_wlan_set_nwk_status(status);
        return RSI_SUCCESS;
      }
    } break;
#ifdef RSI_UART_INTERFACE
    case RSI_WLAN_RSP_UART_DATA_ACK: {
      if (!(rsi_socket_pool[rsi_wlan_cb_non_rom->rsi_uart_data_ack_check].sock_bitmap
            & RSI_SOCKET_FEAT_TCP_ACK_INDICATION)) {
        // check wlan_cb for any task is waiting for response
        if (rsi_wlan_cb->expected_response != cmd_type) {
          // Previous TX packet has dropped in module
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap &= ~BIT(2);
#endif
          // signal the wlan semaphore
          rsi_semaphore_post(&rsi_wlan_cb->wlan_sem);
        }
      }
#ifndef RSI_SEND_SEM_BITMAP
      rsi_driver_cb_non_rom->send_wait_bitmap &= ~BIT(0);
#endif
      rsi_semaphore_post(&rsi_driver_cb_non_rom->send_data_sem);
    } break;
#endif
    case RSI_WLAN_RSP_SOCKET_CONFIG: {
      // check status
      status = rsi_bytes2R_to_uint16(host_desc + RSI_STATUS_OFFSET);
      if (rsi_wlan_cb_non_rom->callback_list.rsi_max_available_rx_window) {
        rsi_wlan_cb_non_rom->callback_list.rsi_max_available_rx_window(status, payload, payload_length);
      }
    } break;
#ifdef RSI_WAC_MFI_ENABLE
    case RSI_WLAN_RSP_ADD_MFI_IE: {
      if (status == RSI_SUCCESS) {
        rsi_wlan_cb->state = RSI_WLAN_STATE_BAND_DONE;
      }
    } break;
#endif
    case RSI_WLAN_RSP_HOST_PSK: {

      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy generated wps pin
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                          : (rsi_wlan_cb->app_buffer_length);
          memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
          rsi_wlan_cb->app_buffer = NULL;
        }
      }

    } break;
    case RSI_WLAN_RSP_SNTP_SERVER:
    case RSI_WLAN_RSP_SNTP_CLIENT: {
      if (status == RSI_SUCCESS) {
        if ((rsi_driver_cb_non_rom->nwk_app_buffer != NULL) && (rsi_driver_cb_non_rom->nwk_app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_driver_cb_non_rom->nwk_app_buffer_length)
                          ? (payload_length)
                          : (rsi_driver_cb_non_rom->nwk_app_buffer_length);
          memcpy(rsi_driver_cb_non_rom->nwk_app_buffer, payload, copy_length);
        }
      }
      rsi_driver_cb_non_rom->nwk_app_buffer = NULL;
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_sntp_client_create_response_handler != NULL) {
        if (((uint8_t)*payload == RSI_SNTP_CREATE) || ((uint8_t)*payload == RSI_SNTP_DELETE)
            || ((uint8_t)*payload == RSI_SNTP_SERVER_ASYNC_RSP) || ((uint8_t)*payload == RSI_SNTP_GETSERVER_ADDRESS)) {
          //Changing the nwk state to allow
          rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
          // Call SNTP client asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_sntp_client_create_response_handler(status,
                                                                                     (uint8_t)*payload,
                                                                                     payload);
          rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending &= ~(SNTP_RESPONSE_PENDING);
          rsi_wlan_set_nwk_status(status);
          return RSI_SUCCESS;
        }
      }
    } break;
    case RSI_WLAN_RSP_RADIO: {
      if (status == RSI_SUCCESS) {
        if (rsi_wlan_cb->state == RSI_WLAN_STATE_SCAN_DONE
            && rsi_wlan_cb_non_rom->wlan_radio == RSI_WLAN_RADIO_DEREGISTER) {
          rsi_wlan_cb->state = RSI_WLAN_STATE_INIT_DONE;
        }
      }

    } break;

      // this switch case copy for response buffer to user space.
    case RSI_WLAN_RSP_GET_CFG: {

      if (status == RSI_SUCCESS) {
        // check the length of application buffer and copy firmware version
        if ((rsi_wlan_cb->app_buffer != NULL) && (rsi_wlan_cb->app_buffer_length != 0)) {
          copy_length = (payload_length < rsi_wlan_cb->app_buffer_length) ? (payload_length)
                                                                          : (rsi_wlan_cb->app_buffer_length);
          memcpy(rsi_wlan_cb->app_buffer, payload, copy_length);
          rsi_wlan_cb->app_buffer = NULL;
        }
      }

    } break;
#ifndef RSI_M4_INTERFACE
    case RSI_WLAN_RSP_CERT_VALID: {
      if (status == RSI_SUCCESS) {

        rsi_cert_recv_t *cert_recev = (rsi_cert_recv_t *)payload;

        // Get IP version
        ip_version = rsi_bytes2R_to_uint16(cert_recev->ip_version);

        if (ip_version == 4) {
          sockID = rsi_get_socket_descriptor(cert_recev->src_port,
                                             cert_recev->dst_port,
                                             cert_recev->ip_address.ipv4_address,
                                             ip_version,
                                             rsi_bytes2R_to_uint16(cert_recev->sock_desc));
        } else {
          sockID = rsi_get_socket_descriptor(cert_recev->src_port,
                                             cert_recev->dst_port,
                                             cert_recev->ip_address.ipv6_address,
                                             ip_version,
                                             rsi_bytes2R_to_uint16(cert_recev->sock_desc));
        }
        if (sockID < 0 || (sockID >= RSI_NUMBER_OF_SOCKETS)) {
          break;
        }

        if ((rsi_socket_pool[sockID].recv_buffer_length) < (*(uint16_t *)(cert_recev->total_len))) {
          status = RSI_ERROR_INSUFFICIENT_BUFFER;
          rsi_wlan_socket_set_status(status, sockID);
        }
      }
      if (rsi_wlan_cb_non_rom->callback_list.certificate_response_handler != NULL) {
        //Changing the nwk state to allow
        rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
        // Call asynchronous response handler to indicate to host
        rsi_wlan_cb_non_rom->callback_list.certificate_response_handler(status, payload, payload_length);
      }
    } break;
    case RSI_WLAN_RSP_MODULE_STATE: {
      if (status == RSI_SUCCESS) {
        if (rsi_wlan_cb_non_rom->callback_list.wlan_async_module_state != NULL) {
          rsi_wlan_cb_non_rom->callback_list.wlan_async_module_state(status, payload, payload_length);
        }
      }
    } break;
    case RSI_WLAN_RSP_TWT_ASYNC: {
      if (rsi_wlan_cb_non_rom->callback_list.twt_response_handler != NULL) {
        rsi_wlan_cb_non_rom->callback_list.twt_response_handler(status, payload, payload_length);
      }
    } break;
#endif
#ifdef PROCESS_SCAN_RESULTS_AT_HOST
    case RSI_WLAN_RSP_SCAN_RESULTS: {

      if (status == RSI_SUCCESS) {

        recv_freq = pkt->desc[9];
        recv_freq = recv_freq << 8 | pkt->desc[8];
        rssi      = pkt->desc[10];
        channel   = pkt->desc[11];

        process_scan_results(payload, payload_length, rssi, channel, recv_freq);
        return RSI_SUCCESS;
      }

    } break;
#endif
    case RSI_WLAN_RSP_EMB_MQTT_CLIENT: {
      if (status == RSI_ERROR_MQTT_PING_TIMEOUT) {
        if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_keep_alive_timeout_callback != NULL) {
          // This should not post semaphore
          rsi_wlan_set_nwk_status(status);
          rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_keep_alive_timeout_callback(status,
                                                                                      pkt->data,
                                                                                      payload_length);
          return RSI_SUCCESS;
        }
      }
    } break;
    case RSI_WLAN_RSP_MQTT_REMOTE_TERMINATE: {
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_remote_terminate_handler != NULL) {
        rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_remote_terminate_handler(status, pkt->data, payload_length);
      }
    } break;
    case RSI_WLAN_RSP_EMB_MQTT_PUBLISH_PKT: {
      if (rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_publish_message_callback != NULL) {
        rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_publish_message_callback(status, pkt->data, payload_length);
      }
    } break;

    default:
      break;
  }
  if (cmd_type == RSI_WLAN_REQ_SELECT_REQUEST) {

    socket_select_rsp = (rsi_rsp_socket_select_t *)payload;
    j                 = socket_select_rsp->select_id;
    // update the status in wlan_cb
    rsi_select_set_status(status, j);
    if (rsi_socket_select_info[j].select_state == RSI_SOCKET_SELECT_STATE_CREATE) {
      // Wait on select semaphore
      rsi_semaphore_post(&rsi_socket_select_info[j].select_sem);
    }
  } else if ((cmd_type == RSI_WLAN_REQ_HTTP_CLIENT_POST) || (cmd_type == RSI_WLAN_REQ_DNS_SERVER_ADD)
             || (cmd_type == RSI_WLAN_REQ_DHCP_USER_CLASS) || (cmd_type == RSI_WLAN_REQ_HTTP_CLIENT_GET)
             || (cmd_type == RSI_WLAN_REQ_POP3_CLIENT) || (cmd_type == RSI_WLAN_REQ_WEBPAGE_LOAD)
             || (cmd_type == RSI_WLAN_REQ_HTTP_ABORT) || (cmd_type == RSI_WLAN_REQ_MULTICAST)
             || (cmd_type == RSI_WLAN_REQ_JSON_LOAD) || (cmd_type == RSI_WLAN_REQ_HTTP_CLIENT_PUT)
             || (cmd_type == RSI_WLAN_REQ_WIRELESS_FWUP) || (cmd_type == RSI_WLAN_REQ_JSON_OBJECT_ERASE)
             || (cmd_type == RSI_WLAN_REQ_HTTP_CLIENT_POST_DATA) || (cmd_type == RSI_WLAN_REQ_HTTP_CREDENTIALS)
             || (cmd_type == RSI_WLAN_REQ_HOST_WEBPAGE_SEND) || (cmd_type == RSI_WLAN_REQ_SMTP_CLIENT)
             || (cmd_type == RSI_WLAN_RSP_DNS_SERVER_ADD) || (cmd_type == RSI_WLAN_REQ_DNS_QUERY)
             || (cmd_type == RSI_WLAN_REQ_FTP) || (cmd_type == RSI_WLAN_REQ_MDNSD)
             || (cmd_type == RSI_WLAN_REQ_DNS_UPDATE) || (cmd_type == RSI_WLAN_REQ_SNTP_CLIENT)
             || (cmd_type == RSI_WLAN_REQ_OTA_FWUP) || (cmd_type == RSI_WLAN_REQ_WEBPAGE_CLEAR_ALL)
             || (cmd_type == RSI_WLAN_REQ_FWUP) || (cmd_type == RSI_WLAN_RSP_WIRELESS_FWUP_OK)
             || (cmd_type == RSI_WLAN_RSP_WIRELESS_FWUP_DONE) || (cmd_type == RSI_WLAN_REQ_EMB_MQTT_CLIENT)
             || (cmd_type == RSI_WLAN_REQ_PING_PACKET)) {
    rsi_wlan_set_nwk_status(status);
#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap &= ~BIT(0);
#endif
    // post on nwk semaphore
    rsi_semaphore_post(&rsi_driver_cb_non_rom->nwk_sem);

  } else if ((cmd_type == RSI_WLAN_REQ_SET_MAC_ADDRESS) || (cmd_type == RSI_WLAN_REQ_BAND)
             || (cmd_type == RSI_WLAN_REQ_TIMEOUT) || (cmd_type == RSI_WLAN_REQ_INIT)
             || (cmd_type == RSI_WLAN_REQ_SET_WEP_KEYS) || (cmd_type == RSI_WLAN_REQ_GET_RANDOM)
             || (cmd_type == RSI_WLAN_REQ_SET_REGION) || (cmd_type == RSI_WLAN_REQ_HOST_PSK)
             || (cmd_type == RSI_WLAN_REQ_DISCONNECT) || (cmd_type == RSI_WLAN_REQ_CONFIG)
             || (cmd_type == RSI_WLAN_REQ_REJOIN_PARAMS) || (cmd_type == RSI_WLAN_REQ_IPCONFV4)
             || (cmd_type == RSI_WLAN_REQ_SCAN) || (cmd_type == RSI_WLAN_REQ_JOIN)
             || (cmd_type == RSI_WLAN_REQ_SET_CERTIFICATE) || (cmd_type == RSI_WLAN_REQ_WMM_PS)
             || (cmd_type == RSI_WLAN_REQ_BG_SCAN) || (cmd_type == RSI_WLAN_REQ_RSSI)
             || (cmd_type == RSI_WLAN_REQ_EAP_CONFIG) || (cmd_type == RSI_WLAN_REQ_ROAM_PARAMS)
             || (cmd_type == RSI_WLAN_REQ_MAC_ADDRESS) || (cmd_type == RSI_WLAN_REQ_WPS_METHOD)
             || (cmd_type == RSI_WLAN_REQ_GET_CFG) || (cmd_type == RSI_WLAN_REQ_CONNECTION_STATUS)
             || (cmd_type == RSI_WLAN_REQ_QUERY_GO_PARAMS) || (cmd_type == RSI_WLAN_REQ_USER_STORE_CONFIG)
             || (cmd_type == RSI_WLAN_REQ_CONFIGURE_P2P) || (cmd_type == RSI_WLAN_REQ_QUERY_NETWORK_PARAMS)
             || (cmd_type == RSI_WLAN_REQ_AP_CONFIGURATION) || (cmd_type == RSI_WLAN_REQ_SET_MULTICAST_FILTER)
             || (cmd_type == RSI_WLAN_REQ_HT_CAPABILITIES) || (cmd_type == RSI_WLAN_REQ_SET_PROFILE)
             || (cmd_type == RSI_WLAN_REQ_CFG_SAVE) || (cmd_type == RSI_WLAN_REQ_TX_TEST_MODE)
             || (cmd_type == RSI_WLAN_REQ_GET_PROFILE) || (cmd_type == RSI_WLAN_REQ_FREQ_OFFSET)
             || (cmd_type == RSI_WLAN_REQ_CALIB_WRITE) || (cmd_type == RSI_WLAN_REQ_DELETE_PROFILE)
             || (cmd_type == RSI_WLAN_REQ_SET_SLEEP_TIMER) || (cmd_type == RSI_WLAN_REQ_FILTER_BCAST_PACKETS)
             || (cmd_type == RSI_WLAN_REQ_AUTO_CONFIG_ENABLE) || (cmd_type == RSI_WLAN_REQ_SOCKET_CONFIG)
             || (cmd_type == RSI_WLAN_RSP_IPCONFV6) || (cmd_type == RSI_WLAN_REQ_SET_REGION_AP)
             || (cmd_type == RSI_WLAN_REQ_DYNAMIC_POOL) || (cmd_type == RSI_WLAN_REQ_GAIN_TABLE)
             || (cmd_type == RSI_WLAN_REQ_RX_STATS) || (cmd_type == RSI_WLAN_REQ_RADIO)
             || (cmd_type == RSI_WLAN_REQ_GET_STATS) || (cmd_type == RSI_WLAN_REQ_11AX_PARAMS)
             || (cmd_type == RSI_WLAN_REQ_TWT_PARAMS) || (cmd_type == RSI_WLAN_REQ_EXT_STATS)
#ifdef RSI_WAC_MFI_ENABLE
             || (cmd_type == RSI_WLAN_REQ_ADD_MFI_IE)
#endif
  ) {
#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap &= ~BIT(0);
#endif
    rsi_wlan_set_status(status);
    // post on nwk semaphore
    rsi_semaphore_post(&rsi_driver_cb_non_rom->wlan_cmd_sem);

  } else if (cmd_type == RSI_WLAN_RSP_SOCKET_CREATE || cmd_type == RSI_WLAN_RSP_CONN_ESTABLISH) {
    if (sockID >= 0) {
      rsi_wlan_socket_set_status(status, sockID);
      if (rsi_socket_pool_non_rom[sockID].socket_wait_bitmap & BIT(0)) {
#ifndef RSI_SOCK_SEM_BITMAP
        rsi_socket_pool_non_rom[sockID].socket_wait_bitmap &= ~BIT(0);
#endif
        // Wait on select semaphore
        rsi_semaphore_post(&rsi_socket_pool_non_rom[sockID].socket_sem);
      }
    }
  }

  return RSI_SUCCESS;
}

/** @} */
/** @addtogroup WLAN
* @{
*/
/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_radio_init(void)
 * @brief       This API initializes wlan radio parameters and WLAN supplicant parameters. This API returns the WLAN MAC address of the module to the host.
 * @pre         \ref rsi_wireless_init() API must be called before this \ref rsi_wlan_radio_init() API. 
 * @param[in]    void  
 * @return       0              - Success  \n
 *               Non-Zero Value - Failure \n
 *                                If return value is greater than 0 \n
 *                                0x0021- Command given in wrong state
 * @note        rsi_wlan_radio_init() is used if user wants to configure any other parameters which are supposed to be given before rsi_wlan_scan() or rsi_wlan_scan_async() API
 */

int32_t rsi_wlan_radio_init(void)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  int32_t allow  = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_RADIO_INIT_ENTRY, WLAN, LOG_INFO);
  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state <= RSI_WLAN_STATE_BAND_DONE) {
    allow = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  }
  if (allow == RSI_SUCCESS) {
    switch (wlan_cb->state) {
      case RSI_WLAN_STATE_OPERMODE_DONE: {
        if (wlan_cb->field_valid_bit_map & RSI_SET_MAC_BIT) {
          // allocate command buffer  from wlan pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // return packet allocation failure error
            SL_PRINTF(SL_WLAN_RADIO_INIT_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_mac_address_t));

          // Memcpy data
          memcpy(&pkt->data, wlan_cb->mac_address, sizeof(rsi_req_mac_address_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // send set mac command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MAC_ADDRESS, pkt);

          // wait on wlan semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_MAC_RESPONSE_WAIT_TIME);

          // get wlan/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return the status if error in sending command occurs
            SL_PRINTF(SL_WLAN_RADIO_INIT_EXIT_1, WLAN, LOG_ERROR, "status: %4x", status);
            return status;
          }
        }

#if HE_PARAMS_SUPPORT
        status = rsi_wlan_11ax_config();
        if (status != RSI_SUCCESS) {
          SL_PRINTF(SL_WLAN_RADIO_INIT_EXIT_2, WLAN, LOG_INFO, "status: %4x", status);
          return status;
        }
#endif
        // allocate command buffer  from wlan pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          //Changing the wlan cmd state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // return packet allocation failure error
          SL_PRINTF(SL_WLAN_RADIO_INIT_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_band_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // send band command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BAND, pkt);

        // wait on wlan semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BAND_RESPONSE_WAIT_TIME);

        // get wlan/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // If BAND command fails
          //Changing the wlan cmd state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return the status if error in sending command occurs
          SL_PRINTF(SL_WLAN_RADIO_INIT_EXIT_3, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
      }
      //no break
      //fall through
      case RSI_WLAN_STATE_BAND_DONE: {
#if RSI_TIMEOUT_SUPPORT
        // allocate command buffer  from wlan pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          //Changing the wlan cmd state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // return packet allocation failure error
          SL_PRINTF(SL_WLAN_RADIO_INIT_PKT_ALLOCATION_FAILURE_3, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_timeout_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

        // send timeout command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TIMEOUT, pkt);

        // wait on wlan semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_TIMEOUT_RESPONSE_WAIT_TIME);

        // get wlan/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          //Changing the wlan cmd state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return the status if error in sending command occurs
          SL_PRINTF(SL_WLAN_RADIO_INIT_EXIT_4, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#endif
        // allocate command buffer  from wlan pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          //Changing the wlan cmd state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // return packet allocation failure error
          SL_PRINTF(SL_WLAN_RADIO_INIT_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // send init command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_INIT, pkt);

        // wait on wlan semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_INIT_RESPONSE_WAIT_TIME);

        // get wlan/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          //Changing the wlan cmd state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return the status if error in sending command occurs
          SL_PRINTF(SL_WLAN_RADIO_INIT_EXIT_5, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#if RSI_SET_REGION_AP_SUPPORT
        if (wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
          // allocate command buffer  from wlan pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // return packet allocation failure error
            SL_PRINTF(SL_WLAN_RADIO_INIT_PKT_ALLOCATION_FAILURE_5, WLAN, LOG_ERROR, "status: %4x", status);
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
            SL_PRINTF(SL_WLAN_RADIO_INIT_EXIT_6, WLAN, LOG_ERROR, "status: %4x", status);
            return status;
          }
        }
#endif
#if RSI_SET_REGION_SUPPORT
        if (wlan_cb->opermode != RSI_WLAN_ACCESS_POINT_MODE) {
          // allocate command buffer  from wlan pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // return packet allocation failure error
            SL_PRINTF(SL_WLAN_RADIO_INIT_PKT_ALLOCATION_FAILURE_6, WLAN, LOG_ERROR, "status: %4x", status);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_set_region_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // send set region command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION, pkt);

          // wait on wlan semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_RESPONSE_WAIT_TIME);

          // get wlan/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return the status if error in sending command occurs
            SL_PRINTF(SL_WLAN_RADIO_INIT_EXIT_7, WLAN, LOG_ERROR, "status: %4x", status);
            return status;
          }
        }
#endif
      } break;
      /* These cases are added to resolve compilation warning*/
      case RSI_WLAN_STATE_NONE:
      case RSI_WLAN_STATE_INIT_DONE:
      case RSI_WLAN_STATE_SCAN_DONE:
      case RSI_WLAN_STATE_CONNECTED:
      case RSI_WLAN_STATE_IP_CONFIG_DONE:
      case RSI_WLAN_STATE_IPV6_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON:
      case RSI_WLAN_STATE_AUTO_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_FAILED:
        //fall through

      default: {
        if (rsi_wlan_cb_non_rom->wlan_radio == RSI_WLAN_RADIO_DEREGISTER) {
          // req for wlan radio
          status = rsi_wlan_req_radio(RSI_ENABLE);
        }
        // Return the status if error in sending command occurs
        SL_PRINTF(SL_WLAN_RADIO_INIT_EXIT_8, WLAN, LOG_ERROR, "status: %4x", status);
        return status;
      }
        // no break
    }
    //Changing the wlan cmd state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // return wlan command error
    SL_PRINTF(SL_WLAN_RADIO_INIT_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "Allow:%4x", allow);
    return allow;
  }

  // Return the status if error in sending command occurs
  SL_PRINTF(SL_WLAN_RADIO_INIT_EXIT_9, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_filter_broadcast(uint16_t beacon_drop_threshold,
                                  uint8_t filter_bcast_in_tim,
                                  uint8_t filter_bcast_tim_till_next_cmd)
 * @brief       Program the ignoring broad cast packet threshold levels when station is in powersave mode and is used to achieve low currents in standby associated mode. \n
                This is blocking API.
 * @param[in]   beacon_drop_threshold               - LMAC beacon drop threshold(ms): The amount of time that FW waits to receive full beacon.Default value is 5000ms. 
 * @param[in]   filter_bcast_in_tim                 - If this bit is set, then from the next dtim any broadcast data pending bit in TIM indicated will be ignored valid values: 0 - 1 
 * @param[in]   filter_bcast_tim_till_next_cmd      -  0 - filter_bcast_in_tim is valid till disconnect of the STA \n
 *                                                     1 - filter_bcast_in_tim is valid till next update by giving the same command 
 * @note        Validity of this bit is dependent on the  filter_bcast_tim_till_next_cmd
 * @return       0              - Success   \n
 *               Non-Zero Value - If return value is less than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                               return value is greater than 0 \n
 *                               0x0021 
 */
int32_t rsi_wlan_filter_broadcast(uint16_t beacon_drop_threshold,
                                  uint8_t filter_bcast_in_tim,
                                  uint8_t filter_bcast_tim_till_next_cmd)
{
  rsi_pkt_t *pkt;

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_FILTER_BROADCAST_ENTRY, WLAN, LOG_INFO);
  rsi_req_wlan_filter_bcast_t *rsi_filter_bcast;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // return packet allocation failure error
      SL_PRINTF(SL_WLAN_FILTER_BROADCAST_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    // Memset data
    memset(&pkt->data, 0, sizeof(rsi_req_wlan_filter_bcast_t));

    rsi_filter_bcast = (rsi_req_wlan_filter_bcast_t *)pkt->data;

    rsi_uint16_to_2bytes(rsi_filter_bcast->beacon_drop_threshold, beacon_drop_threshold);

    rsi_filter_bcast->filter_bcast_in_tim = filter_bcast_in_tim;

    rsi_filter_bcast->filter_bcast_tim_till_next_cmd = filter_bcast_tim_till_next_cmd;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // send band command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FILTER_BCAST_PACKETS, pkt);

    // wait on wlan semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_FILTER_BROADCAST_RESPONSE_WAIT_TIME);

    // get wlan/network command response status
    status = rsi_wlan_get_status();

    //Changing the wlan cmd state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // return nwk command error
    SL_PRINTF(SL_WLAN_FILTER_BROADCAST_NWK_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return the status if error in sending command occurs
  SL_PRINTF(SL_WLAN_FILTER_BROADCAST_COMMAND_SEN_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/** @} */
/** @addtogroup DRIVER13
* @{
*/
/*==============================================*/
/**
 *
 * @fn         uint32_t rsi_check_wlan_state(void)
 * @brief      Get WLAN status. 
 * @param[in]  void  
 * @return     Wlan block status \n
 *             0 -  RSI_WLAN_STATE_NONE  \n
 *             1 -  RSI_WLAN_STATE_OPERMODE_DONE \n
 *             2 -  RSI_WLAN_STATE_BAND_DONE \n
 *             3 -  RSI_WLAN_STATE_INIT_DONE \n 
 *             4 -  RSI_WLAN_STATE_SCAN_DONE \n
 *             5 -  RSI_WLAN_STATE_CONNECTED \n 
 *             6 -  RSI_WLAN_STATE_IP_CONFIG_DONE \n
 *             7 -  RSI_WLAN_STATE_IPV6_CONFIG_DONE \n
 *             8 -  RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON \n
 *             9 -  RSI_WLAN_STATE_AUTO_CONFIG_DONE \n
 *             10-  RSI_WLAN_STATE_AUTO_CONFIG_FAILED 
 *
 *
 *
 */
/// @private
uint32_t rsi_check_wlan_state(void)
{
  return rsi_driver_cb->wlan_cb->state;
}

/*==============================================*/
/**
 * @fn          void rsi_wlan_set_status(int32_t status)
 * @brief       Set the WLAN status.  
 * @param[in]   status - Status value to be set 
 * @return      void
 *
 *
 */
/// @private
void rsi_wlan_set_status(int32_t status)
{
  rsi_driver_cb->wlan_cb->status = status;
}
/*==============================================*/
/**
 * @fn          int32_t rsi_post_waiting_semaphore(void)
 * @brief       Post on a waiting semaphore. 
 * @param[in]   void  
 * @return      0	           -  Success \n
 *              Non-Zero Value - Failure
 *          
 */
/// @private
int32_t rsi_post_waiting_semaphore(void)
{
  int i = 0;
  rsi_rsp_socket_create_t socket_rsp;
  rsi_wlan_set_nwk_status(RSI_ERROR_IN_WLAN_CMD);
  rsi_call_asynchronous_callback();
  rsi_post_waiting_nwk_semaphore();
  for (i = 0; i < NUMBER_OF_SOCKETS; i++) {
    // post socket semaphores
    if (rsi_socket_pool[i].sock_state > RSI_SOCKET_STATE_INIT) {
      // Set error
      rsi_wlan_socket_set_status(RSI_ERROR_IN_WLAN_CMD, i);

#ifdef SOCKET_CLOSE_WAIT
      rsi_socket_pool_non_rom[i].close_pending = 1;
#endif
      if (rsi_wlan_cb_non_rom->socket_cmd_rsp_pending & BIT(i)) {
        memset(&socket_rsp, 0, sizeof(rsi_rsp_socket_create_t));
        if (rsi_wlan_cb_non_rom->callback_list.socket_connect_response_handler != NULL) {
          // Changing the callback response f/w socket id to app socket id.
          rsi_uint16_to_2bytes(socket_rsp.socket_id, i);
          // Call asynchronous response handler to indicate to host
          rsi_wlan_cb_non_rom->callback_list.socket_connect_response_handler((uint16_t)RSI_ERROR_IN_WLAN_CMD,
                                                                             (uint8_t *)&socket_rsp,
                                                                             sizeof(rsi_rsp_socket_create_t));
          rsi_wlan_cb_non_rom->socket_cmd_rsp_pending &= ~BIT(i);
        }
        if (rsi_socket_pool_non_rom[i].accept_call_back_handler != NULL) {
          rsi_socket_pool_non_rom[i].accept_call_back_handler(i, 0, 0, 0);
          rsi_wlan_cb_non_rom->socket_cmd_rsp_pending &= ~BIT(i);
        }
      }
      // post on  semaphore
      rsi_post_waiting_socket_semaphore(i);
    }
  }
#ifndef SOCKET_CLOSE_WAIT
  // Clear all sockets info
  rsi_clear_sockets(RSI_CLEAR_ALL_SOCKETS);
#endif
  for (i = 0; i < RSI_NUMBER_OF_SELECTS; i++) {
    if (rsi_socket_select_info[i].select_state == RSI_SOCKET_SELECT_STATE_CREATE) {
      if (rsi_socket_select_info[i].sock_select_callback == NULL) {
        // update the status in wlan_cb
        rsi_select_set_status(RSI_ERROR_IN_WLAN_CMD, i);
        rsi_semaphore_post(&rsi_socket_select_info[i].select_sem);
      } else {
        //clear the read fd set
        RSI_FD_ZERO(&rsi_socket_select_info[i].rsi_sel_read_fds_var);
        rsi_socket_select_info[i].rsi_sel_read_fds_var.fd_count = 0;

        //clear the write fd set
        RSI_FD_ZERO(&rsi_socket_select_info[i].rsi_sel_write_fds_var);
        rsi_socket_select_info[i].rsi_sel_write_fds_var.fd_count = 0;

        //clear the exception fd set
        RSI_FD_ZERO(&rsi_socket_select_info[i].rsi_sel_exception_fds_var);
        rsi_socket_select_info[i].rsi_sel_exception_fds_var.fd_count = 0;

        rsi_socket_select_info[i].sock_select_callback(
          (rsi_fd_set *)&rsi_socket_select_info[i].rsi_sel_read_fds_var,
          (rsi_fd_set *)&rsi_socket_select_info[i].rsi_sel_write_fds_var,
          (rsi_fd_set *)&rsi_socket_select_info[i].rsi_sel_exception_fds_var,
          -1);

        rsi_socket_select_info[i].sock_select_callback = NULL;

        rsi_socket_select_info[i].select_state = RSI_SOCKET_SELECT_STATE_INIT;
      }
    }
  }
  rsi_wlan_cb_non_rom->socket_cmd_rsp_pending = 0;
  return RSI_SUCCESS;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_check_waiting_socket_cmd(void)
 * @brief       Check if any socket command is in waiting state.
 * @param[in]   void  
 * @return      0 - No socket command is in waiting state \n
 *              1 - socket command is in waiting state 
 *              
 */
/// @private
int32_t rsi_wlan_check_waiting_socket_cmd(void)
{

  // Get wlan cb struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if ((wlan_cb->expected_response == RSI_WLAN_RSP_SOCKET_ACCEPT)
      || (wlan_cb->expected_response == RSI_WLAN_RSP_SOCKET_CREATE)
      || (wlan_cb->expected_response == RSI_WLAN_RSP_SOCKET_READ_DATA)
      || (wlan_cb->expected_response == RSI_WLAN_RSP_SOCKET_CLOSE)
      || (wlan_cb->expected_response == RSI_WLAN_RSP_CONN_ESTABLISH)
      || (wlan_cb->expected_response == RSI_WLAN_RSP_TCP_ACK_INDICATION)) {
    return 1;
  } else {
    return 0;
  }
}

/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_check_waiting_wlan_cmd(void)
 * @brief       Check if any WLAN command is in waiting state.
 * @param[in]   void  
 * @return      0	           -  Success \n
 *              Non-Zero Value - Failure
 *
 *
 */
/// @private
int32_t rsi_wlan_check_waiting_wlan_cmd(void)
{
  // Get wlan cb struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if ((wlan_cb->expected_response == RSI_WLAN_RSP_IPCONFV4) || (wlan_cb->expected_response == RSI_WLAN_RSP_DISCONNECT)
      || (wlan_cb->expected_response == RSI_WLAN_RSP_IPCONFV6)
      || (wlan_cb->expected_response == RSI_WLAN_RSP_ROAM_PARAMS)
      || (wlan_cb->expected_response == RSI_WLAN_RSP_BG_SCAN)
      || (wlan_cb->expected_response == RSI_WLAN_RSP_DYNAMIC_POOL)
      || (wlan_cb->expected_response == RSI_WLAN_RSP_SET_MULTICAST_FILTER))

  {
    return 1;
  } else {
    return RSI_SUCCESS;
  }
}
/*==============================================*/
/**
 * @fn         void rsi_wlan_process_raw_data(rsi_pkt_t *pkt)
 * @brief      Receive raw data packet from module. 
 * @param[in]  pkt - pointer to rx pkt
 * @note       Memory allocation for the pointer is from receive handler  ,after process it will be freed . 
 * @return     void
 *
 *
 */

void rsi_wlan_process_raw_data(rsi_pkt_t *pkt)
{
  uint8_t *host_desc;
  uint8_t *payload;
  uint16_t payload_length;

  // Get wlan cb struct pointer
  //  rsi_wlan_cb_t *rsi_wlan_cb = rsi_driver_cb->wlan_cb;

  // Get host descriptor
  host_desc = pkt->desc;

  // Get payload pointer
  payload = pkt->data;

  // Get payoad length
  payload_length = (rsi_bytes2R_to_uint16(host_desc) & 0xFFF);

  if (rsi_wlan_cb_non_rom->callback_list.wlan_data_receive_handler != NULL) {
    // Call asynchronous data receive handler to indicate to host
    rsi_wlan_cb_non_rom->callback_list.wlan_data_receive_handler(0, payload, payload_length);
  }
}

/*==============================================*/
/**
 * @fn          void rsi_wlan_packet_transfer_done(rsi_pkt_t *pkt)
 * @brief       Handle packet transfer completion.  
 * @param[in]   pkt - pointer to packet 
 * @return      void
 *
 */
/// @private
void rsi_wlan_packet_transfer_done(rsi_pkt_t *pkt)
{
  // uint8_t queueno = 0xff;
#ifndef RSI_UART_INTERFACE
  uint8_t queueno; // To avoid warning for unused variable 'queueno' added the macros.
  uint8_t frame_type;
  uint8_t *buf_ptr;
  rsi_req_socket_send_t *send;
  int32_t sockID;
#endif

#ifndef RSI_UART_INTERFACE
  buf_ptr = (uint8_t *)pkt->desc;

  // Get Frame type
  frame_type = buf_ptr[2];

  // Get data pointer
  send = (rsi_req_socket_send_t *)pkt->data;

  // Get protocol type
  queueno = (buf_ptr[1] & 0xf0) >> 4;
#endif

  // Set wlan status as success
  rsi_wlan_set_status(RSI_SUCCESS);
#ifndef RSI_UART_INTERFACE
  if (queueno == RSI_WLAN_DATA_Q) {
    if (frame_type == 0x1) {
#ifndef RSI_SEND_SEM_BITMAP
      rsi_driver_cb_non_rom->send_wait_bitmap &= ~BIT(0);
#endif
      rsi_semaphore_post(&rsi_driver_cb_non_rom->send_data_sem);
    } else if (rsi_driver_cb->wlan_cb->expected_response != RSI_WLAN_RSP_TCP_ACK_INDICATION) {
      sockID = rsi_get_application_socket_descriptor(send->socket_id[0]);
      if ((sockID >= 0) && (sockID < RSI_NUMBER_OF_SOCKETS)) {
        rsi_wlan_socket_set_status(RSI_SUCCESS, sockID);
#ifndef RSI_SOCK_SEM_BITMAP
        rsi_socket_pool_non_rom[sockID].socket_wait_bitmap &= ~BIT(2);
        RSI_SEMAPHORE_POST(&rsi_socket_pool_non_rom[sockID].sock_send_sem);
#else
        RSI_SEMAPHORE_POST(&rsi_socket_pool_non_rom[sockID].sock_send_sem);
#endif
      }
    }
  }
#endif
  // Free the wlan tx pkt
  rsi_pkt_free(&rsi_driver_cb->wlan_cb->wlan_tx_pool, pkt);
}

/*==============================================*/
/**
 * @fn          void rsi_check_wlan_buffer_full(rsi_pkt_t *pkt)
 * @brief       Will clear TX packet, if TX buffer is full. 
 * @param[in]   pkt - Buffer pointer 
 * @note        BUFFER_FULL_HANDLING macro should be enabled.
 * @return      void
 *
 */
/// @private
void rsi_check_wlan_buffer_full(rsi_pkt_t *pkt)
{
  uint8_t queueno = 0xff;
  uint8_t *buf_ptr;
  int32_t sockID;
  uint8_t frame_type;
  rsi_req_socket_send_t *send;

  buf_ptr = (uint8_t *)pkt->desc;
  // Get Frame type
  frame_type = buf_ptr[2];

  // Get data pointer
  send = (rsi_req_socket_send_t *)pkt->data;
  // Get protocol type
  queueno = (buf_ptr[1] & 0xf0) >> 4;
  // Set wlan status as TX buffer full
  rsi_wlan_set_status(RSI_TX_BUFFER_FULL);
  if (queueno == RSI_WLAN_DATA_Q) {
    if (frame_type == 0x1) {
#ifndef RSI_SEND_SEM_BITMAP
      rsi_driver_cb_non_rom->send_wait_bitmap &= ~BIT(0);
#endif
      rsi_semaphore_post(&rsi_driver_cb_non_rom->send_data_sem);
    } else if (rsi_driver_cb->wlan_cb->expected_response != RSI_WLAN_RSP_TCP_ACK_INDICATION) {
      sockID = rsi_get_application_socket_descriptor(send->socket_id[0]);
      if (sockID >= 0) {
        rsi_wlan_socket_set_status(RSI_SUCCESS, sockID);
#ifndef RSI_SOCK_SEM_BITMAP
        rsi_socket_pool_non_rom[sockID].socket_wait_bitmap &= ~BIT(2);
#endif
        RSI_SEMAPHORE_POST(&rsi_socket_pool_non_rom[sockID].sock_send_sem);
      }
    }
  }
  // Free the wlan tx pkt
  rsi_pkt_free(&rsi_driver_cb->wlan_cb->wlan_tx_pool, pkt);
}

/*==============================================*/
/**
 * @fn         void rsi_check_common_buffer_full(rsi_pkt_t *pkt)
 * @brief      Check if the common buffer is full. 
 * @param[in]  pkt - Buffer pointer  
 * @return     void
 *
 */
/// @private
void rsi_check_common_buffer_full(rsi_pkt_t *pkt)
{
  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(pkt);
  // Set wlan status as TX buffer full
  rsi_wlan_set_status(RSI_TX_BUFFER_FULL);
#ifndef RSI_WLAN_SEM_BITMAP
  rsi_driver_cb_non_rom->wlan_wait_bitmap &= ~BIT(2);
#endif

  // Post the semaphore which is waiting on socket send
  rsi_semaphore_post(&rsi_driver_cb->wlan_cb->wlan_sem);
}
/*==============================================*/
/**
 * @fn          rsi_error_t rsi_wait_on_wlan_semaphore(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms ) 
 * @brief       By wireless library to acquire or wait for wlan semaphore.
 * @param[in]   Semaphore  - Semaphore handle pointer 
 * @param[in]   timeout_ms - Maximum time to wait to acquire semaphore. \n If timeout_ms is 0, then wait till semaphore is acquired
 * @return      0              - Success \n
 *              Negative Value - failure
 */
/// @private
rsi_error_t rsi_wait_on_wlan_semaphore(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms)
{
  // Wait on wlan semaphore
  if (rsi_semaphore_wait(semaphore, timeout_ms) != RSI_ERROR_NONE) {
    rsi_wlan_set_status(RSI_ERROR_RESPONSE_TIMEOUT);
#ifndef RSI_WAIT_TIMEOUT_EVENT_HANDLE_TIMER_DISABLE
    if (rsi_driver_cb_non_rom->rsi_wait_timeout_handler_error_cb != NULL) {
      rsi_driver_cb_non_rom->rsi_wait_timeout_handler_error_cb(RSI_ERROR_RESPONSE_TIMEOUT, WLAN_CMD);
    }
#endif
    return RSI_ERROR_RESPONSE_TIMEOUT;
  }
  return RSI_ERROR_NONE;
}

/*==============================================*/
/**
 * @fn          void rsi_update_wlan_cmd_state_to_free_state(void) 
 * @brief       By wireless library to update the WLAN command state to free state. 
 * @param[in]   void  
 * @return      void
 *
 */

void rsi_update_wlan_cmd_state_to_free_state(void)
{
  rsi_driver_cb_non_rom->wlan_cmd_state = RSI_WLAN_CMD_IN_FREE_STATE;
}

/*==============================================*/
/**
 * @fn          void rsi_update_wlan_cmd_state_to_progress_state(void)
 * @brief       By wireless library to update the wlan cmd state 
 *              to progress state.
 * @param[in]  void  
 * @return     void
 */
/// @private
void rsi_update_wlan_cmd_state_to_progress_state(void)
{
  rsi_driver_cb_non_rom->wlan_cmd_state = RSI_WLAN_CMD_IN_PROGRESS;
}
/*==============================================*/
/**
 * @fn          int32_t rsi_post_waiting_wlan_semaphore(void)
 * @brief       Posts all waiting WLAN semaphores. This API is typically used to reset the semaphore states.
 * @param[in]   void  
 * @return      0	           -  Success \n
 *              Non-Zero Value - Failure
 *
 *
 */
/// @private
#ifndef RSI_WLAN_SEM_BITMAP
void rsi_post_waiting_wlan_semaphore(void)
{
  if (rsi_driver_cb_non_rom->wlan_wait_bitmap & BIT(0)) {
    rsi_semaphore_post(&rsi_driver_cb_non_rom->wlan_cmd_sem);
  }
  if (rsi_driver_cb_non_rom->wlan_wait_bitmap & BIT(1)) {
    rsi_semaphore_post(&rsi_driver_cb_non_rom->wlan_cmd_send_sem);
  }
  if (rsi_driver_cb_non_rom->wlan_wait_bitmap & BIT(2)) {
    rsi_semaphore_post(&rsi_driver_cb->wlan_cb->wlan_sem);
  }
  rsi_driver_cb_non_rom->wlan_wait_bitmap = 0;
}
#endif
void rsi_call_asynchronous_callback()
{
  if ((rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending & (PING_RESPONSE_PENDING))
      && rsi_wlan_cb_non_rom->callback_list.wlan_ping_response_handler != NULL) {
    // Call asynchronous response handler to indicate to host
    rsi_wlan_cb_non_rom->callback_list.wlan_ping_response_handler((uint16_t)RSI_ERROR_IN_WLAN_CMD, NULL, 0);
    rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending &= ~(PING_RESPONSE_PENDING);
  }

  if ((rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending & (DNS_RESPONSE_PENDING))
      && rsi_wlan_cb_non_rom->nwk_callbacks.rsi_dns_update_rsp_handler != NULL) {
    rsi_wlan_cb_non_rom->nwk_callbacks.rsi_dns_update_rsp_handler((uint16_t)RSI_ERROR_IN_WLAN_CMD);
    rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending &= ~(DNS_RESPONSE_PENDING);
  }
  if ((rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending & (SNTP_RESPONSE_PENDING))
      && rsi_wlan_cb_non_rom->nwk_callbacks.rsi_sntp_client_create_response_handler != NULL) {
    rsi_wlan_cb_non_rom->nwk_callbacks.rsi_sntp_client_create_response_handler((uint16_t)RSI_ERROR_IN_WLAN_CMD,
                                                                               0,
                                                                               NULL);
    rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending &= ~(SNTP_RESPONSE_PENDING);
  }
  rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending = 0;
}
/** @} */
#ifdef PROCESS_SCAN_RESULTS_AT_HOST

/** @addtogroup DRIVER13
* @{
*/
/*==============================================*/
/**
 *
 * @fn         void sort_index_based_on_rssi(struct wpa_scan_results_arr *scan_results_array)
 * @brief      Sort the scan list based on rssi value. 
 * @param[in]  scan_results_array - Pointer to scan results array  
 * @return     void
 */
/// @private
void sort_index_based_on_rssi(struct wpa_scan_results_arr *scan_results_array)
{
  uint16_t scan[MAX_SCAN_COUNT];
  struct wpa_scan_res *sc = &scan_results_array->res[0];
  uint16_t i, j;
  uint16_t min;
  for (i = 0; i < scan_results_array->num; i++, sc++) {
    scan[i] = sc->rssiVal;
  }
  for (i = 0; i < scan_results_array->num; i++) {
    min                               = scan[0];
    scan_results_array->sort_index[i] = 0;
    for (j = 1; j < scan_results_array->num; j++) {
      if (min > scan[j]) {
        scan_results_array->sort_index[i] = j;
        min                               = scan[j];
      }
    }
    scan[scan_results_array->sort_index[i]] = 0xFF;
  }
}

/*==============================================*/
/**
 *
 * @fn         int process_scan_results(uint8_t *buf, uint16_t len, int8_t rssi, uint8_t channel, uint16_t freq)
 * @brief      Process received beacons and probe responses. 
 * @param[in]  buf     - Received frame. 
 * @param[in]  len     - length of the buffer. 
 * @param[in]  rssi    - RSSI value 
 * @param[in]  channel - Channel in which the frame is received 
 * @param[in]  freq    - Frequency of the channel  
 * @return     0               - Success \n 
 *             Non-Zero Value  - failure
 *              
 *
 *
 *
 */
/// @private
int process_scan_results(uint8_t *buf, uint16_t len, int8_t rssi, uint8_t channel, uint16_t freq)
{
  uint8_t subtype;
  uint16_t i;
  uint16_t ies_len;
  uint8_t *bssid;
  uint16_t pairwise_cipher_count = 0;
  const uint8_t *ssid_;
  uint8_t ssid_len;
  uint8_t *bss = NULL;
  uint8_t *end = NULL;
  uint16_t caps;
  uint8_t WLAN_WIFI_OUI_RSN[3];
  uint8_t WLAN_WIFI_OUI[3];

  struct wpa_scan_results_arr *scan_res = scan_results_array;
  struct wpa_scan_res *result           = NULL;
  uint16_t *index                       = NULL;

  WLAN_WIFI_OUI_RSN[0] = 0x00;
  WLAN_WIFI_OUI_RSN[1] = 0x0F;
  WLAN_WIFI_OUI_RSN[2] = 0xAC;

  WLAN_WIFI_OUI[0] = 0x00;
  WLAN_WIFI_OUI[1] = 0x50;
  WLAN_WIFI_OUI[2] = 0xF2;

  /* if memory is not allocated to hold scan results break */
  if (scan_res == NULL) {
    return RSI_FAILURE;
  }
  if ((len == 0)) {
    return RSI_FAILURE;
  }

  index = &scan_results_array->sort_index[0];

  bssid = &buf[BSSID_OFFSET];

  subtype = buf[0] & IEEE80211_FC0_SUBTYPE_MASK;

  switch (subtype) {
    case IEEE80211_FC0_SUBTYPE_BEACON:
    case IEEE80211_FC0_SUBTYPE_PROBE_RESP: {
      if (len <= MINIMUM_FRAME_LENGTH)
        return RSI_FAILURE;

      ies_len = len - MINIMUM_FRAME_LENGTH;

      /* Find if there is an entry already with same bssid */
      for (i = 0; i < scan_res->num; i++) {
        if ((memcmp(scan_res->res[i].bssid, bssid, ETH_ALEN) == 0)
            && (strncmp((const char *)scan_res->res[i].ssid, (const char *)&buf[SSID_OFFSET], scan_res->res[i].ssid_len)
                == 0)) {
          /* entry exist with same bssid, need to replace with new information*/
          result = &scan_res->res[i];
          break;
        }
      }

      if (result == NULL) {
        /* Try to get entry to hold new information from beacon & probe response */
        if (scan_res->num >= MAX_SCAN_COUNT) {
          sort_index_based_on_rssi(scan_results_array);

          /* if already maximum limit reached create space by deleting existing entry with low rssi */
          if ((scan_res->res[index[scan_res->num - 1]].level < rssi)) {
            result = &scan_res->res[scan_res->sort_index[scan_res->num - 1]];
          }

          if (result == NULL) {
            break;
          }
        } else {
          result = &scan_res->res[scan_res->num];
          scan_res->num++;
        }
      }

      if (freq == 0)
        return RSI_FAILURE;
      /* copy bssid */
      memcpy((uint8_t *)&result->bssid, bssid, ETH_ALEN);

      result->freq       = freq;
      result->channel_no = channel;

      caps            = *(uint16_t *)&buf[CAPABILITIES_OFFSET];
      result->level   = rssi;
      result->rssiVal = -rssi;
      if (((caps & BIT(0)) == 1) && ((caps & BIT(1)) == 0))
        result->uNetworkType = 1;
      else
        result->uNetworkType = 0;
      if ((caps & BIT(4))) {
        result->security_mode = SME_WEP;
      } else {
        result->security_mode = SME_OPEN;
      }
      bss      = &buf[IES_OFFSET];
      ssid_    = (uint8_t *)"";
      ssid_len = 0;
      memset((uint8_t *)&result->ssid, 0, RSI_SSID_LEN);
      end = bss + ies_len;
      while (bss + 1 < end) {

        if (bss + IE_POS + bss[IE_LEN] > end)
          break;

        switch (bss[IE_TYPE]) {
          case WLAN_EID_SSID:
            ssid_len = bss[IE_LEN];
            ssid_    = bss + IE_POS;
            memcpy((uint8_t *)&result->ssid, ssid_, ssid_len);
            result->ssid_len = ssid_len;
            break;

          case WLAN_EID_VENDOR_SPECIFIC:
            if (((!memcmp(&bss[IE_POS], WLAN_WIFI_OUI, 3)) && bss[IE_POS + 3] == 0x01)
                && (result->security_mode == SME_OPEN || result->security_mode == SME_WEP)) {
              result->security_mode = SME_WPA;
              pairwise_cipher_count = *(uint16_t *)&bss[12];
              if (bss[19 + pairwise_cipher_count * 4] == 1) {
                result->security_mode = SME_WPA_ENTERPRISE;
              }
            }
            break;
          case WLAN_EID_RSN:
            if (!memcmp((uint8_t *)&bss[WIFI_OUI_RSN], WLAN_WIFI_OUI_RSN, 3)) {
              result->security_mode = SME_WPA2;
              pairwise_cipher_count = *(uint16_t *)&bss[8];
              if (bss[15 + pairwise_cipher_count * 4] == 1) {
                result->security_mode = SME_WPA2_ENTERPRISE;
              }
            }
            break;
          default:
            break;
        }
        bss += IE_POS + bss[IE_LEN];
      }

      break;
    }

    default:

      break;
  }
  return 0;
}
#endif
/** @} */
