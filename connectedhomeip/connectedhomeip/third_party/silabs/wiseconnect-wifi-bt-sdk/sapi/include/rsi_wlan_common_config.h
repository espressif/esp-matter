/*******************************************************************************
* @file  rsi_wlan_common_config.h
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
/**
 * @file         rsi_wlan_common_config.h
 * @version      0.1
 * @date         15 Aug 2018
 *
 *
 *
 *  @brief : This file contains user configurable details to configure the device
 *
 *  @section Description  This file contains user configurable details to configure the device
 *
 *
 */
#ifndef RSI_WLAN_COMMON_CONFIG_H
#define RSI_WLAN_COMMON_CONFIG_H

#include "rsi_wlan_defines.h"
#include "rsi_user.h"
#include <rsi_data_types.h>
/******************************************************
 * *                      WLAN
 * ******************************************************/
// Enable feature
#ifndef RSI_ENABLE
#define RSI_ENABLE 1
#endif

// Disable feature
#ifndef RSI_DISABLE
#define RSI_DISABLE 0
#endif

// To enable concurrent mode
#ifndef CONCURRENT_MODE
#define CONCURRENT_MODE RSI_DISABLE
#endif

#ifndef RSI_FAST_FW_UP
#define RSI_FAST_FW_UP RSI_DISABLE
#endif
// opermode command paramaters
/*=======================================================================*/
// To set wlan feature select bit map
#ifndef RSI_FEATURE_BIT_MAP
#define RSI_FEATURE_BIT_MAP (FEAT_SECURITY_OPEN)
#endif

// TCP IP BYPASS feature check
#ifndef RSI_TCP_IP_BYPASS
#define RSI_TCP_IP_BYPASS RSI_DISABLE
#endif

// TCP/IP feature select bitmap for selecting TCP/IP features
#ifndef RSI_TCP_IP_FEATURE_BIT_MAP
#define RSI_TCP_IP_FEATURE_BIT_MAP (TCP_IP_FEAT_DHCPV4_CLIENT)
#endif

// To set custom feature select bit map
#ifndef RSI_CUSTOM_FEATURE_BIT_MAP
#define RSI_CUSTOM_FEATURE_BIT_MAP 0
#endif

// To set Extended custom feature select bit map
#ifndef RSI_EXT_CUSTOM_FEATURE_BIT_MAP
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP 0
#endif

#ifndef RSI_EXT_TCPIP_FEATURE_BITMAP
#define RSI_EXT_TCPIP_FEATURE_BITMAP 0
#endif

#ifndef RSI_BT_FEATURE_BITMAP
#define RSI_BT_FEATURE_BITMAP BT_RF_TYPE
#endif

#ifndef RSI_SLEEP_IND_FROM_DEV_PIN_SELECT
#define RSI_SLEEP_IND_FROM_DEV_PIN_SELECT 0
#endif

/***************************************************
 * PowerSave Bitmap Configuration
 ***************************************************
 */
#if (RS9116_SILICON_CHIP_VER == CHIP_VER_1P4_AND_ABOVE)
#define PROTOCOL_DEBUG_GPIO_PINS          BIT(1)
#define RSI_APP_TCP_IP_FEATURE_BITMAP     TCP_IP_FEAT_EXTENSION_VALID
#define RSI_APP_EXT_TCP_IP_FEATURE_BITMAP CONFIG_FEAT_EXTENTION_VALID
#define RSI_APP_CONFIG_FEATURE_BITMAP     RSI_FEAT_SLEEP_GPIO_SEL_BITMAP

#else

#define RSI_APP_TCP_IP_FEATURE_BITMAP     0
#define RSI_APP_EXT_TCP_IP_FEATURE_BITMAP 0
#define RSI_APP_CONFIG_FEATURE_BITMAP     0

#endif

#ifndef RSI_CONFIG_FEATURE_BITMAP
#define RSI_CONFIG_FEATURE_BITMAP 0
#endif

/*=======================================================================*/

// Feature frame parameters
/*=======================================================================*/
#ifndef PLL_MODE
#define PLL_MODE 0
#endif

#ifndef RF_TYPE
#define RF_TYPE 1 // 0 - External RF 1- Internal RF
#endif

#ifndef WIRELESS_MODE
#define WIRELESS_MODE 0
#endif

#ifndef ENABLE_PPP
#define ENABLE_PPP 0
#endif

#ifndef AFE_TYPE
#define AFE_TYPE 1
#endif

#ifndef FEATURE_ENABLES
#define FEATURE_ENABLES 0
#endif

/*=======================================================================*/

// Band command paramters
/*=======================================================================*/

// RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#ifndef RSI_BAND
#define RSI_BAND RSI_BAND_2P4GHZ
#endif

/*=======================================================================*/

// set region command paramters
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE Set region support
#ifndef RSI_SET_REGION_SUPPORT
#define RSI_SET_REGION_SUPPORT RSI_DISABLE //@ RSI_ENABLE or RSI_DISABLE set region
#endif

// If 1:region configurations taken from user ;0:region configurations taken from beacon
#ifndef RSI_SET_REGION_FROM_USER_OR_BEACON
#define RSI_SET_REGION_FROM_USER_OR_BEACON 1
#endif

// 0-Default Region domain ,1-US, 2-EUROPE, 3-JAPAN, 4-WORLD, 5-KOREA
#ifndef RSI_REGION_CODE
#define RSI_REGION_CODE 1
#endif

// 0- Without On Board Antenna , 1- With On Board Antenna
#ifndef RSI_MODULE_TYPE
#define RSI_MODULE_TYPE 1
#endif

/*=======================================================================*/

// set region AP command paramters
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE Set region AP support
#ifndef RSI_SET_REGION_AP_SUPPORT
#define RSI_SET_REGION_AP_SUPPORT RSI_DISABLE
#endif

// If 1:region configurations taken from user ;0:region configurations taken from firmware
#ifndef RSI_SET_REGION_AP_FROM_USER
#define RSI_SET_REGION_AP_FROM_USER RSI_DISABLE
#endif

// "US" or "EU" or "JP" or other region codes
#ifndef RSI_COUNTRY_CODE
#define RSI_COUNTRY_CODE "US "
#endif

/*=======================================================================*/

// Rejoin parameters
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE rejoin params
#ifndef RSI_REJOIN_PARAMS_SUPPORT
#define RSI_REJOIN_PARAMS_SUPPORT RSI_DISABLE
#endif

// Rejoin retry count. If 0 retries infinity times
#ifndef RSI_REJOIN_MAX_RETRY
#define RSI_REJOIN_MAX_RETRY 0
#endif

// Periodicity of rejoin attempt
#ifndef RSI_REJOIN_SCAN_INTERVAL
#define RSI_REJOIN_SCAN_INTERVAL 4
#endif

// Beacon missed count
#ifndef RSI_REJOIN_BEACON_MISSED_COUNT
#define RSI_REJOIN_BEACON_MISSED_COUNT 40
#endif

// RSI_ENABLE or RSI_DISABLE retry for first time join failure
#ifndef RSI_REJOIN_FIRST_TIME_RETRY
#define RSI_REJOIN_FIRST_TIME_RETRY RSI_DISABLE
#endif

/*=======================================================================*/

//BG scan command parameters
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE BG Scan support
#ifndef RSI_BG_SCAN_SUPPORT
#define RSI_BG_SCAN_SUPPORT RSI_DISABLE
#endif

// RSI_ENABLE or RSI_DISABLE BG scan
#ifndef RSI_BG_SCAN_ENABLE
#define RSI_BG_SCAN_ENABLE RSI_ENABLE
#endif

// RSI_ENABLE or RSI_DISABLE instant BG scan
#ifndef RSI_INSTANT_BG
#define RSI_INSTANT_BG RSI_DISABLE
#endif

// BG scan threshold value
#ifndef RSI_BG_SCAN_THRESHOLD
#define RSI_BG_SCAN_THRESHOLD 10
#endif

// RSSI tolerance Threshold
#ifndef RSI_RSSI_TOLERANCE_THRESHOLD
#define RSI_RSSI_TOLERANCE_THRESHOLD 4
#endif

// BG scan periodicity
#ifndef RSI_BG_SCAN_PERIODICITY
#define RSI_BG_SCAN_PERIODICITY 10
#endif

// Active scan duration
#ifndef RSI_ACTIVE_SCAN_DURATION
#define RSI_ACTIVE_SCAN_DURATION 15
#endif

// Passive scan duration
#ifndef RSI_PASSIVE_SCAN_DURATION
#define RSI_PASSIVE_SCAN_DURATION 20
#endif

// Multi probe
#ifndef RSI_MULTIPROBE
#define RSI_MULTIPROBE RSI_DISABLE
#endif
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE for RSI_WLAN_CONFIG_ENABLE
#ifndef RSI_WLAN_CONFIG_ENABLE
#define RSI_WLAN_CONFIG_ENABLE RSI_ENABLE
#endif

#ifndef CONFIG_RTSTHRESHOLD
#define CONFIG_RTSTHRESHOLD 1
#endif

#ifndef RSI_RTS_THRESHOLD
#define RSI_RTS_THRESHOLD 2346
#endif

/*=======================================================================*/
//Roaming parameters
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE Roaming support
#ifndef RSI_ROAMING_SUPPORT
#define RSI_ROAMING_SUPPORT RSI_DISABLE
#endif

// roaming threshold value
#ifndef RSI_ROAMING_THRESHOLD
#define RSI_ROAMING_THRESHOLD 10
#endif

// roaming hysterisis value
#ifndef RSI_ROAMING_HYSTERISIS
#define RSI_ROAMING_HYSTERISIS 4
#endif

/*=======================================================================*/

// High Throughput Capabilies related information
/*=======================================================================*/

// RSI_ENABLE or RSI_DISABLE 11n mode in AP mode
#ifndef RSI_MODE_11N_ENABLE
#define RSI_MODE_11N_ENABLE RSI_DISABLE
#endif

// HT caps bit map.
#ifndef RSI_HT_CAPS_BIT_MAP
#define RSI_HT_CAPS_BIT_MAP 2
#endif

/*=======================================================================*/

// Scan command parameters
/*=======================================================================*/

// scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#ifndef RSI_SCAN_CHANNEL_BIT_MAP_2_4
#define RSI_SCAN_CHANNEL_BIT_MAP_2_4 0
#endif

// scan channle bit map in 5GHz band ,valid if given channel to scan is 0
#ifndef RSI_SCAN_CHANNEL_BIT_MAP_5
#define RSI_SCAN_CHANNEL_BIT_MAP_5 0
#endif

// scan_feature_bitmap ,valid only if specific channel to scan and ssid are given
#ifndef RSI_SCAN_FEAT_BITMAP
#define RSI_SCAN_FEAT_BITMAP RSI_ENABLE_QUICK_SCAN
#endif

/*=======================================================================*/

// Enterprise configuration command parameters
/*=======================================================================*/

// Enterprise method ,should be one of among TLS, TTLS, FAST or PEAP
#ifndef RSI_EAP_METHOD
#define RSI_EAP_METHOD "TTLS"
#endif
// This parameter is used to configure the module in Enterprise security mode
#ifndef RSI_EAP_INNER_METHOD
#define RSI_EAP_INNER_METHOD "\"auth=MSCHAPV2\""
#endif

// Private Key Password is required for encrypted private key, format is like "\"12345678\""
#ifndef RSI_PRIVATE_KEY_PASSWORD
#define RSI_PRIVATE_KEY_PASSWORD ""
#endif

/*=======================================================================*/

// AP configuration command parameters
/*=======================================================================*/

// This Macro is used to enable AP keep alive functionality
#ifndef RSI_AP_KEEP_ALIVE_ENABLE
#define RSI_AP_KEEP_ALIVE_ENABLE RSI_ENABLE
#endif

// This parameter is used to configure keep alive type
#ifndef RSI_AP_KEEP_ALIVE_TYPE
#define RSI_AP_KEEP_ALIVE_TYPE RSI_NULL_BASED_KEEP_ALIVE
#endif

// This parameter is used to configure keep alive period
#ifndef RSI_AP_KEEP_ALIVE_PERIOD
#define RSI_AP_KEEP_ALIVE_PERIOD 100
#endif

// This parameter is used to configure maximum stations supported
#ifndef RSI_MAX_STATIONS_SUPPORT
#define RSI_MAX_STATIONS_SUPPORT 4
#endif

/*=======================================================================*/

// Join command parameters
/*=======================================================================*/

// Tx power level
#ifndef RSI_POWER_LEVEL
#define RSI_POWER_LEVEL RSI_POWER_LEVEL_HIGH
#endif

// RSI_JOIN_FEAT_STA_BG_ONLY_MODE_ENABLE or RSI_JOIN_FEAT_LISTEN_INTERVAL_VALID
#ifndef RSI_JOIN_FEAT_BIT_MAP
#define RSI_JOIN_FEAT_BIT_MAP 0
#endif

//
#ifndef RSI_LISTEN_INTERVAL
#define RSI_LISTEN_INTERVAL 0
#endif

// Transmission data rate. Physical rate at which data has to be transmitted.
#ifndef RSI_DATA_RATE
#define RSI_DATA_RATE RSI_DATA_RATE_AUTO
#endif

/*=======================================================================*/

// Ipconf command parameters
/*=======================================================================*/

// DHCP client host name
#ifndef RSI_DHCP_HOST_NAME
#define RSI_DHCP_HOST_NAME "dhcp_client"
#endif

// Transmit test command parameters
/*=======================================================================*/
// TX TEST rate flags
#ifndef RSI_TX_TEST_RATE_FLAGS
#define RSI_TX_TEST_RATE_FLAGS 0
#endif

// TX TEST per channel bandwidth
#ifndef RSI_TX_TEST_PER_CH_BW
#define RSI_TX_TEST_PER_CH_BW 0
#endif

// TX TEST aggregation enable or disable
//If defined in 'rsi_config.h' file, then this won't be used
#ifndef RSI_TX_TEST_AGGR_ENABLE
#define RSI_TX_TEST_AGGR_ENABLE RSI_DISABLE
#endif

// TX TEST delay
#ifndef RSI_TX_TEST_DELAY
#define RSI_TX_TEST_DELAY 0
#endif

// TX TEST Number of packets
#ifndef RSI_TX_TEST_NUM_PKTS
#define RSI_TX_TEST_NUM_PKTS 0
#endif

#ifdef CHIP_9117
#ifndef RSI_11AX_ENABLE
//11AX_ENABLE 0-disable, 1-enable
#define RSI_11AX_ENABLE 0
#endif

#ifndef RSI_CODING_TYPE
// coding_type 0-BCC 1-LDPC
#define RSI_CODING_TYPE 0
#endif

#ifndef RSI_NOMINAL_PE
// nominal_pe 0-0Us 1-8Us 2-16Us
#define RSI_NOMINAL_PE 0
#endif

// he_ppdu_type 0-HE SU PPDU, 1-HE ER SU PPDU, 2-HE TB PPDU, 3-HE MU PPDU
#ifndef RSI_HE_PPDU_TYPE
#define RSI_HE_PPDU_TYPE 0
#endif

//beam_change 0-sameway, 1-differently
#ifndef RSI_BEAM_CHANGE
#define RSI_BEAM_CHANGE 0
#endif

//BW 0-242 tone RU, 1-Upper 106 tone RU
#ifndef RSI_BW
#define RSI_BW 0
#endif

//UL_DL 0-PPDU sent by AP, 1-PPDU sent by STA to AP
#ifndef RSI_UL_DL
#define RSI_UL_DL 0
#endif

//STBC 0-no STBC, 1-STBC if only DCM is set to 0
#ifndef RSI_STBC
#define RSI_STBC 0
#endif

//TX_BF 0-no beamforming matrix, 1- beamforming matrix
#ifndef RSI_TX_BF
#define RSI_TX_BF 0
#endif

//GT_LTF shall be in the range 0-3
#ifndef RSI_GI_LTF
#define RSI_GI_LTF 0
#endif

//DCM 0-No DCM, 1-DCM
#ifndef RSI_DCM
#define RSI_DCM 0
#endif

//NSTS_MIDAMBLE shall be in the range 0-7
#ifndef RSI_NSTS_MIDAMBLE
#define RSI_NSTS_MIDAMBLE 0
#endif

//spatial_reuse shall be in the range 0-15
#ifndef RSI_SPATIAL_REUSE
#define RSI_SPATIAL_REUSE 0
#endif

//BSS_COLOR shall be in the range 0-63
#ifndef RSI_BSS_COLOR
#define RSI_BSS_COLOR 0
#endif

//HE_SIGA2_RESERVED shall be in the range 0-511
#ifndef RSI_HE_SIGA2_RESERVED
#define RSI_HE_SIGA2_RESERVED 0
#endif

//RU_ALLOCATION shall be in the range 0-255
#ifndef RSI_RU_ALLOCATION
#define RSI_RU_ALLOCATION 0
#endif

//N_HELTF_TOT shall be in the range 0-7
#ifndef RSI_N_HELTF_TOT
#define RSI_N_HELTF_TOT 0
#endif

//SIGB_DCM 0-disable, 1-enable
#ifndef RSI_SIGB_DCM
#define RSI_SIGB_DCM 0
#endif

//SIGB_MCS shall be in the range 0-5
#ifndef RSI_SIGB_MCS
#define RSI_SIGB_MCS 0
#endif

//USER_STA_ID shall be in the range 0-2047
#ifndef RSI_USER_STA_ID
#define RSI_USER_STA_ID 0
#endif

//USER_IDX shall be in the range 0-8
#ifndef RSI_USER_IDX
#define RSI_USER_IDX 0
#endif

//SIGB_COMPRESSION_FIELD shall be 0/1
#ifndef RSI_SIGB_COMPRESSION_FIELD
#define RSI_SIGB_COMPRESSION_FIELD 0
#endif
#endif
/*=======================================================================*/

// ssl parameters
/*=======================================================================*/
// ssl version
#ifndef RSI_SSL_VERSION
#define RSI_SSL_VERSION 0
#endif

#define RSI_SSL_RELEASE_2_0 RSI_ENABLE

#ifdef CHIP_9117
#ifndef RSI_SSL_EXT_CIPHERS
#define RSI_SSL_EXT_CIPHERS SSL_TLSV1_3_ALL_CIPHERS
#endif
#endif

// ssl ciphers
#ifndef RSI_SSL_CIPHERS
#if RSI_SSL_RELEASE_2_0
#define RSI_SSL_CIPHERS SSL_RELEASE_2_0_ALL_CIPHERS
#else
#define RSI_SSL_CIPHERS \
  (BIT_DHE_RSA_GCM | BIT_ECDHE_RSA_GCM | BIT_DHE_RSA_CBC | BIT_ECDHE_RSA_CBC | BIT_ECDHE_ECDSA_CBC | SSL_NEW_CIPHERS)
#endif
#endif

// Enable TCP over SSL with TLS version depends on remote side
#ifndef PROTOCOL_DFLT_VERSION
#define PROTOCOL_DFLT_VERSION BIT(0)
#endif

// Enable TCP over SSL with TLS version 1.0
#ifndef PROTOCOL_TLS_1_0
#define PROTOCOL_TLS_1_0 (BIT(0) | BIT(13))
#endif

// Enable TCP over SSL with TLS version 1.1
#ifndef PROTOCOL_TLS_1_1
#define PROTOCOL_TLS_1_1 (BIT(0) | BIT(14))
#endif

// Enable TCP over SSL with TLS version 1.2
#ifndef PROTOCOL_TLS_1_2
#define PROTOCOL_TLS_1_2 (BIT(0) | BIT(15))
#endif
/*=======================================================================*/

// Power save command parameters
/*=======================================================================*/
// set handshake type of power mode
#ifndef RSI_HAND_SHAKE_TYPE
#define RSI_HAND_SHAKE_TYPE MSG_BASED
#endif

// 0 - LP, 1- ULP mode with RAM retention and 2 - ULP with Non RAM retention
#ifndef RSI_SELECT_LP_OR_ULP_MODE
#define RSI_SELECT_LP_OR_ULP_MODE RSI_ULP_WITH_RAM_RET
#endif

// set DTIM aligment required
// 0 - module wakes up at beacon which is just before or equal to listen_interval
// 1 - module wakes up at DTIM beacon which is just before or equal to listen_interval
#ifndef RSI_DTIM_ALIGNED_TYPE
#define RSI_DTIM_ALIGNED_TYPE 0
#endif

// Monitor interval for the FAST PSP mode
// default is 50 ms, and this parameter is valid for FAST PSP only
#ifndef RSI_MONITOR_INTERVAL
#define RSI_MONITOR_INTERVAL 50
#endif

// Number of DTIMs to skip during powersave
#ifndef RSI_NUM_OF_DTIM_SKIP
#define RSI_NUM_OF_DTIM_SKIP 0
#endif

//WMM PS parameters
// set wmm enable or disable
#ifndef RSI_WMM_PS_ENABLE
#define RSI_WMM_PS_ENABLE RSI_DISABLE
#endif

// set wmm enable or disable
// 0- TX BASED 1 - PERIODIC
#ifndef RSI_WMM_PS_TYPE
#define RSI_WMM_PS_TYPE 0
#endif

// set wmm wake up interval
#ifndef RSI_WMM_PS_WAKE_INTERVAL
#define RSI_WMM_PS_WAKE_INTERVAL 20
#endif

// set wmm UAPSD bitmap
#ifndef RSI_WMM_PS_UAPSD_BITMAP
#define RSI_WMM_PS_UAPSD_BITMAP 15
#endif

/*=======================================================================*/

// Socket configuration
/*=======================================================================*/
// RSI_ENABLE or RSI_DISABLE High performance socket
#ifndef HIGH_PERFORMANCE_ENABLE
#define HIGH_PERFORMANCE_ENABLE RSI_ENABLE //@ RSI_ENABLE or RSI_DISABLE High performance socket
#endif

#ifndef TOTAL_SOCKETS
#define TOTAL_SOCKETS 1 //@ Total number of sockets. TCP TX + TCP RX + UDP TX + UDP RX
#endif

#ifndef TOTAL_TCP_SOCKETS
#define TOTAL_TCP_SOCKETS 1 //@ Total TCP sockets. TCP TX + TCP RX
#endif

#ifndef TOTAL_UDP_SOCKETS
#define TOTAL_UDP_SOCKETS 0 //@ Total UDP sockets. UDP TX + UDP RX
#endif

#ifndef TCP_TX_ONLY_SOCKETS
#define TCP_TX_ONLY_SOCKETS 0 //@ Total TCP TX only sockets. TCP TX
#endif

#ifndef TCP_RX_ONLY_SOCKETS
#define TCP_RX_ONLY_SOCKETS 1 //@ Total TCP RX only sockets. TCP RX
#endif

#ifndef UDP_TX_ONLY_SOCKETS
#define UDP_TX_ONLY_SOCKETS 0 //@ Total UDP TX only sockets. UDP TX
#endif

#ifndef UDP_RX_ONLY_SOCKETS
#define UDP_RX_ONLY_SOCKETS 0 //@ Total UDP RX only sockets. UDP RX
#endif

#ifndef TCP_RX_HIGH_PERFORMANCE_SOCKETS
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS 1 //@ Total TCP RX High Performance sockets
#endif

#ifndef TCP_RX_WINDOW_SIZE_CAP
#define TCP_RX_WINDOW_SIZE_CAP \
  10 //@ TCP RX Window size - To scale the window size linearly according to the value (TCP MSS * TCP_RX_WINDOW_SIZE_CAP)
#endif

#ifndef TCP_RX_WINDOW_DIV_FACTOR
#define TCP_RX_WINDOW_DIV_FACTOR \
  10 //@ TCP RX Window division factor - To increase the ACK frequency for asynchronous sockets
#endif

/*=======================================================================*/

// Socket Create parameters
/*=======================================================================*/

// Initial timeout for Socket
#ifndef RSI_SOCKET_KEEPALIVE_TIMEOUT
#define RSI_SOCKET_KEEPALIVE_TIMEOUT 1200
#endif

// VAP ID for Concurrent mode
#ifndef RSI_VAP_ID
#define RSI_VAP_ID 0
#endif

// Timeout for join or scan
/*=======================================================================*/

//RSI_ENABLE or RSI_DISABLE Timeout support
#ifndef RSI_TIMEOUT_SUPPORT
#define RSI_TIMEOUT_SUPPORT RSI_ENABLE
#endif

// roaming threshold value
#ifndef RSI_TIMEOUT_BIT_MAP
#define RSI_TIMEOUT_BIT_MAP 1
#endif

// roaming hysterisis value
#ifndef RSI_TIMEOUT_VALUE
#define RSI_TIMEOUT_VALUE 3000
#endif

// Timeout for ping request
/*=======================================================================*/

//Timeout for PING_REQUEST
#ifndef RSI_PING_REQ_TIMEOUT_MS
#define RSI_PING_REQ_TIMEOUT_MS 10000
#endif

#ifndef RSI_PING_REQ_TIMEOUT
#define RSI_PING_REQ_TIMEOUT (RSI_PING_REQ_TIMEOUT_MS / 100)
#endif

// Store Config Profile parameters
/*=======================================================================*/

// Client profile
#ifndef RSI_WLAN_PROFILE_CLIENT
#define RSI_WLAN_PROFILE_CLIENT 0
#endif

// P2P profile
#ifndef RSI_WLAN_PROFILE_P2P
#define RSI_WLAN_PROFILE_P2P 1
#endif

// EAP profile
#ifndef RSI_WLAN_PROFILE_EAP
#define RSI_WLAN_PROFILE_EAP 2
#endif

// AP profile
#ifndef RSI_WLAN_PROFILE_AP
#define RSI_WLAN_PROFILE_AP 6
#endif

// All profiles
#ifndef RSI_WLAN_PROFILE_ALL
#define RSI_WLAN_PROFILE_ALL 0xFF
#endif

// AP Config Profile Parameters
/*==============================================================================*/

// Transmission data rate. Physical rate at which data has to be transmitted.
#ifndef RSI_CONFIG_AP_DATA_RATE
#define RSI_CONFIG_AP_DATA_RATE RSI_DATA_RATE_AUTO
#endif

// To set wlan feature select bit map
#ifndef RSI_CONFIG_AP_WLAN_FEAT_BIT_MAP
#define RSI_CONFIG_AP_WLAN_FEAT_BIT_MAP (FEAT_SECURITY_PSK)
#endif

// TCP/IP feature select bitmap for selecting TCP/IP features
#ifndef RSI_CONFIG_AP_TCP_IP_FEAT_BIT_MAP
#define RSI_CONFIG_AP_TCP_IP_FEAT_BIT_MAP (TCP_IP_FEAT_DHCPV4_SERVER)
#endif

// To set custom feature select bit map
#ifndef RSI_CONFIG_AP_CUSTOM_FEAT_BIT_MAP
#define RSI_CONFIG_AP_CUSTOM_FEAT_BIT_MAP 0
#endif

// Tx power level
#ifndef RSI_CONFIG_AP_TX_POWER
#define RSI_CONFIG_AP_TX_POWER RSI_POWER_LEVEL_HIGH
#endif

// AP SSID
#ifndef RSI_CONFIG_AP_SSID
#define RSI_CONFIG_AP_SSID "SILABS_AP"
#endif

// RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#ifndef RSI_CONFIG_AP_BAND
#define RSI_CONFIG_AP_BAND RSI_BAND_2P4GHZ
#endif

// To configure AP channle number
#ifndef RSI_CONFIG_AP_CHANNEL
#define RSI_CONFIG_AP_CHANNEL 6
#endif

// To configure security type
#ifndef RSI_CONFIG_AP_SECURITY_TYPE
#define RSI_CONFIG_AP_SECURITY_TYPE RSI_WPA
#endif

// To configure encryption type
#ifndef RSI_CONFIG_AP_ENCRYPTION_TYPE
#define RSI_CONFIG_AP_ENCRYPTION_TYPE 1
#endif

// To configure PSK
#ifndef RSI_CONFIG_AP_PSK
#define RSI_CONFIG_AP_PSK "1234567890"
#endif

// To configure beacon interval
#ifndef RSI_CONFIG_AP_BEACON_INTERVAL
#define RSI_CONFIG_AP_BEACON_INTERVAL 100
#endif

// To configure DTIM period
#ifndef RSI_CONFIG_AP_DTIM
#define RSI_CONFIG_AP_DTIM 2
#endif

// This parameter is used to configure keep alive type
#ifndef RSI_CONFIG_AP_KEEP_ALIVE_TYPE
#define RSI_CONFIG_AP_KEEP_ALIVE_TYPE 0 // RSI_NULL_BASED_KEEP_ALIVE
#endif

#ifndef RSI_CONFIG_AP_KEEP_ALIVE_COUNTER
#define RSI_CONFIG_AP_KEEP_ALIVE_COUNTER 0 // 100
#endif

// This parameter is used to configure keep alive period
#ifndef RSI_CONFIG_AP_KEEP_ALIVE_PERIOD
#define RSI_CONFIG_AP_KEEP_ALIVE_PERIOD 100
#endif

// This parameter is used to configure maximum stations supported
#ifndef RSI_CONFIG_AP_MAX_STATIONS_COUNT
#define RSI_CONFIG_AP_MAX_STATIONS_COUNT 4
#endif

// P2P Network parameters
// TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#ifndef RSI_CONFIG_AP_TCP_STACK_USED
#define RSI_CONFIG_AP_TCP_STACK_USED BIT(0)
#endif

// IP address of the module
// E.g: 0x0A0AA8C0 == 192.168.10.10
#ifndef RSI_CONFIG_AP_IP_ADDRESS
#define RSI_CONFIG_AP_IP_ADDRESS 0x0A0AA8C0
#endif

// IP address of netmask
// E.g: 0x00FFFFFF == 255.255.255.0
#ifndef RSI_CONFIG_AP_SN_MASK_ADDRESS
#define RSI_CONFIG_AP_SN_MASK_ADDRESS 0x00FFFFFF
#endif

// IP address of Gateway
// E.g: 0x0A0AA8C0 == 192.168.10.10
#ifndef RSI_CONFIG_AP_GATEWAY_ADDRESS
#define RSI_CONFIG_AP_GATEWAY_ADDRESS 0x0A0AA8C0
#endif

// Client Profile Parameters
/* ===================================================================================== */

// To configure data rate
#ifndef RSI_CONFIG_CLIENT_DATA_RATE
#define RSI_CONFIG_CLIENT_DATA_RATE RSI_DATA_RATE_AUTO
#endif

// To configure wlan feature bitmap
#ifndef RSI_CONFIG_CLIENT_WLAN_FEAT_BIT_MAP
#define RSI_CONFIG_CLIENT_WLAN_FEAT_BIT_MAP 0
#endif

// To configure tcp/ip feature bitmap
#ifndef RSI_CONFIG_CLIENT_TCP_IP_FEAT_BIT_MAP
#define RSI_CONFIG_CLIENT_TCP_IP_FEAT_BIT_MAP BIT(2)
#endif

// To configure custom feature bit map
#ifndef RSI_CONFIG_CLIENT_CUSTOM_FEAT_BIT_MAP
#define RSI_CONFIG_CLIENT_CUSTOM_FEAT_BIT_MAP 0
#endif

// To configure TX power
#ifndef RSI_CONFIG_CLIENT_TX_POWER
#define RSI_CONFIG_CLIENT_TX_POWER RSI_POWER_LEVEL_HIGH
#endif

// To configure listen interval
#ifndef RSI_CONFIG_CLIENT_LISTEN_INTERVAL
#define RSI_CONFIG_CLIENT_LISTEN_INTERVAL 0
#endif

// To configure SSID
#ifndef RSI_CONFIG_CLIENT_SSID
#define RSI_CONFIG_CLIENT_SSID "SILABS_AP"
#endif

// RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#ifndef RSI_CONFIG_CLIENT_BAND
#define RSI_CONFIG_CLIENT_BAND RSI_BAND_2P4GHZ
#endif

// To configure channel number
#ifndef RSI_CONFIG_CLIENT_CHANNEL
#define RSI_CONFIG_CLIENT_CHANNEL 0
#endif

// To configure security type
#ifndef RSI_CONFIG_CLIENT_SECURITY_TYPE
#define RSI_CONFIG_CLIENT_SECURITY_TYPE RSI_WPA
#endif

// To configure encryption type
#ifndef RSI_CONFIG_CLIENT_ENCRYPTION_TYPE
#define RSI_CONFIG_CLIENT_ENCRYPTION_TYPE 0
#endif

// To configure PSK
#ifndef RSI_CONFIG_CLIENT_PSK
#define RSI_CONFIG_CLIENT_PSK "1234567890"
#endif

// To configure PMK
#ifndef RSI_CONFIG_CLIENT_PMK
#define RSI_CONFIG_CLIENT_PMK ""
#endif

// Client Network parameters
// TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#ifndef RSI_CONFIG_CLIENT_TCP_STACK_USED
#define RSI_CONFIG_CLIENT_TCP_STACK_USED BIT(0)
#endif

// DHCP mode 1- Enable 0- Disable
// If DHCP mode is disabled given IP statically
#ifndef RSI_CONFIG_CLIENT_DHCP_MODE
#define RSI_CONFIG_CLIENT_DHCP_MODE RSI_DHCP
#endif

// IP address of the module
// E.g: 0x0A0AA8C0 == 192.168.10.10
#ifndef RSI_CONFIG_CLIENT_IP_ADDRESS
#define RSI_CONFIG_CLIENT_IP_ADDRESS 0x0A0AA8C0
#endif

// IP address of netmask
// E.g: 0x00FFFFFF == 255.255.255.0
#ifndef RSI_CONFIG_CLIENT_SN_MASK_ADDRESS
#define RSI_CONFIG_CLIENT_SN_MASK_ADDRESS 0x00FFFFFF
#endif

// IP address of Gateway
// E.g: 0x010AA8C0 == 192.168.10.1
#ifndef RSI_CONFIG_CLIENT_GATEWAY_ADDRESS
#define RSI_CONFIG_CLIENT_GATEWAY_ADDRESS 0x010AA8C0
#endif

// scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#ifndef RSI_CONFIG_CLIENT_SCAN_FEAT_BITMAP
#define RSI_CONFIG_CLIENT_SCAN_FEAT_BITMAP 0
#endif

// Scan channel magic code
#ifndef RSI_CONFIG_CLIENT_MAGIC_CODE
#define RSI_CONFIG_CLIENT_MAGIC_CODE 0x4321
#endif

// scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#ifndef RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_2_4_GHZ
#define RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_2_4_GHZ 0
#endif

// scan channle bit map in 5GHz band ,valid if given channel to scan is 0
#ifndef RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_5_0_GHZ
#define RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_5_0_GHZ 0
#endif

// EAP Client Profile Parameters
/* =========================================================================== */

// To configure data rate
#ifndef RSI_CONFIG_EAP_DATA_RATE
#define RSI_CONFIG_EAP_DATA_RATE RSI_DATA_RATE_AUTO
#endif

// To configure wlan feature bitmap
#ifndef RSI_CONFIG_EAP_WLAN_FEAT_BIT_MAP
#define RSI_CONFIG_EAP_WLAN_FEAT_BIT_MAP 0
#endif

// To configure tcp/ip feature bitmap
#ifndef RSI_CONFIG_EAP_TCP_IP_FEAT_BIT_MAP
#define RSI_CONFIG_EAP_TCP_IP_FEAT_BIT_MAP BIT(2)
#endif

// To configure custom feature bit map
#ifndef RSI_CONFIG_EAP_CUSTOM_FEAT_BIT_MAP
#define RSI_CONFIG_EAP_CUSTOM_FEAT_BIT_MAP 0
#endif

// To configure EAP TX power
#ifndef RSI_CONFIG_EAP_TX_POWER
#define RSI_CONFIG_EAP_TX_POWER RSI_POWER_LEVEL_HIGH
#endif

// To Configure scan channel feature bitmap
#ifndef RSI_CONFIG_EAP_SCAN_FEAT_BITMAP
#define RSI_CONFIG_EAP_SCAN_FEAT_BITMAP 0
#endif

// scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#ifndef RSI_CONFIG_EAP_CHAN_MAGIC_CODE
#define RSI_CONFIG_EAP_CHAN_MAGIC_CODE 0 //0x4321
#endif

// scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#ifndef RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_2_4_GHZ
#define RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_2_4_GHZ 0
#endif

// scan channle bit map in 5GHz band ,valid if given channel to scan is 0
#ifndef RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_5_0_GHZ
#define RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_5_0_GHZ 0
#endif

// To configure SSID
#ifndef RSI_CONFIG_EAP_SSID
#define RSI_CONFIG_EAP_SSID "SILABS_AP"
#endif

// RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#ifndef RSI_CONFIG_EAP_BAND
#define RSI_CONFIG_EAP_BAND RSI_BAND_2P4GHZ
#endif

// To set security type
#ifndef RSI_CONFIG_EAP_SECURITY_TYPE
#define RSI_CONFIG_EAP_SECURITY_TYPE RSI_WPA2_EAP
#endif

// To set encryption type
#ifndef RSI_CONFIG_EAP_ENCRYPTION_TYPE
#define RSI_CONFIG_EAP_ENCRYPTION_TYPE 0
#endif

// To set channel number
#ifndef RSI_CONFIG_EAP_CHANNEL
#define RSI_CONFIG_EAP_CHANNEL 0
#endif

// Enterprise method ,should be one of among TLS, TTLS, FAST or PEAP
#ifndef RSI_CONFIG_EAP_METHOD
#define RSI_CONFIG_EAP_METHOD "TLS"
#endif

// This parameter is used to configure the module in Enterprise security mode
#ifndef RSI_CONFIG_EAP_INNER_METHOD
#define RSI_CONFIG_EAP_INNER_METHOD "\"auth=MSCHAPV2\""
#endif

// To configure EAP user identity
#ifndef RSI_CONFIG_EAP_USER_IDENTITY
#define RSI_CONFIG_EAP_USER_IDENTITY "\"user1\""
#endif

// TO configure EAP password
#ifndef RSI_CONFIG_EAP_PASSWORD
#define RSI_CONFIG_EAP_PASSWORD "\"test123\""
#endif

// EAP Network parameters
// TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#ifndef RSI_CONFIG_EAP_TCP_STACK_USED
#define RSI_CONFIG_EAP_TCP_STACK_USED BIT(0)
#endif

// DHCP mode 1- Enable 0- Disable
// If DHCP mode is disabled given IP statically
#ifndef RSI_CONFIG_EAP_DHCP_MODE
#define RSI_CONFIG_EAP_DHCP_MODE RSI_DHCP
#endif

// IP address of the module
// E.g: 0x0A0AA8C0 == 192.168.10.10
#ifndef RSI_CONFIG_EAP_IP_ADDRESS
#define RSI_CONFIG_EAP_IP_ADDRESS 0x0A0AA8C0
#endif

// IP address of netmask
// E.g: 0x00FFFFFF == 255.255.255.0
#ifndef RSI_CONFIG_EAP_SN_MASK_ADDRESS
#define RSI_CONFIG_EAP_SN_MASK_ADDRESS 0x00FFFFFF
#endif

// IP address of Gateway
// E.g: 0x010AA8C0 == 192.168.10.1
#ifndef RSI_CONFIG_EAP_GATEWAY_ADDRESS
#define RSI_CONFIG_EAP_GATEWAY_ADDRESS 0x010AA8C0
#endif

// P2P Profile parameters
/* ================================================================================= */

// To configure data rate
#ifndef RSI_CONFIG_P2P_DATA_RATE
#define RSI_CONFIG_P2P_DATA_RATE RSI_DATA_RATE_AUTO
#endif
// To configure wlan feature bitmap
#ifndef RSI_CONFIG_P2P_WLAN_FEAT_BIT_MAP
#define RSI_CONFIG_P2P_WLAN_FEAT_BIT_MAP 0
#endif

// To configure P2P tcp/ip feature  bitmap
#ifndef RSI_CONFIG_P2P_TCP_IP_FEAT_BIT_MAP
#define RSI_CONFIG_P2P_TCP_IP_FEAT_BIT_MAP BIT(2)
#endif

// To configure P2P custom feature bitmap
#ifndef RSI_CONFIG_P2P_CUSTOM_FEAT_BIT_MAP
#define RSI_CONFIG_P2P_CUSTOM_FEAT_BIT_MAP 0
#endif

// TO configure P2P tx power level
#ifndef RSI_CONFIG_P2P_TX_POWER
#define RSI_CONFIG_P2P_TX_POWER RSI_POWER_LEVEL_HIGH
#endif

// Set P2P go intent
#ifndef RSI_CONFIG_P2P_GO_INTNET
#define RSI_CONFIG_P2P_GO_INTNET 16 // Support only Autonomous GO mode
#endif

// Set device name
#ifndef RSI_CONFIG_P2P_DEVICE_NAME
#define RSI_CONFIG_P2P_DEVICE_NAME "WSC1.1"
#endif

// Set device operating channel
#ifndef RSI_CONFIG_P2P_OPERATING_CHANNEL
#define RSI_CONFIG_P2P_OPERATING_CHANNEL 11
#endif

// Set SSID postfix
#ifndef RSI_CONFIG_P2P_SSID_POSTFIX
#define RSI_CONFIG_P2P_SSID_POSTFIX "WSC_1_0_0"
#endif

// Set P2P join SSID
#ifndef RSI_CONFIG_P2P_JOIN_SSID
#define RSI_CONFIG_P2P_JOIN_SSID "SILABS_AP"
#endif

// Set psk key
#ifndef RSI_CONFIG_P2P_PSK_KEY
#define RSI_CONFIG_P2P_PSK_KEY "12345678"
#endif

// P2P Network parameters
// TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#ifndef RSI_CONFIG_P2P_TCP_STACK_USED
#define RSI_CONFIG_P2P_TCP_STACK_USED BIT(0)
#endif

// DHCP mode 1- Enable 0- Disable
// If DHCP mode is disabled given IP statically
#ifndef RSI_CONFIG_P2P_DHCP_MODE
#define RSI_CONFIG_P2P_DHCP_MODE 1
#endif

// IP address of the module
// E.g: 0x0A0AA8C0 == 192.168.10.10
#ifndef RSI_CONFIG_P2P_IP_ADDRESS
#define RSI_CONFIG_P2P_IP_ADDRESS 0x0A0AA8C0
#endif

// IP address of netmask
// E.g: 0x00FFFFFF == 255.255.255.0
#ifndef RSI_CONFIG_P2P_SN_MASK_ADDRESS
#define RSI_CONFIG_P2P_SN_MASK_ADDRESS 0x00FFFFFF
#endif

// IP address of Gateway
// E.g: 0x010AA8C0 == 192.168.10.1
#ifndef RSI_CONFIG_P2P_GATEWAY_ADDRESS
#define RSI_CONFIG_P2P_GATEWAY_ADDRESS 0x010AA8C0
#endif

/*=======================================================================*/
// buffer allocation parameters
/*=======================================================================*/

#ifndef TX_RX_RATIO_ENABLE
#define TX_RX_RATIO_ENABLE RSI_DISABLE
#endif

#ifndef TX_POOL_RATIO
#define TX_POOL_RATIO 1
#endif

#ifndef RX_POOL_RATIO
#define RX_POOL_RATIO 1
#endif

#ifndef GLOBAL_POOL_RATIO
#define GLOBAL_POOL_RATIO 1
#endif
/*=======================================================================*/

// Unblock call
/*=======================================================================*/
#ifndef BUFFER_FULL_HANDLING
#define BUFFER_FULL_HANDLING RSI_DISABLE
#endif
/*=======================================================================*/

// To enable ASSERT indication from module
/*========================================================================*/
#ifndef RSI_ASSERT_ENABLE
#define RSI_ASSERT_ENABLE 1
#endif
/*========================================================================*/
// 11ax params
/*========================================================================*/
#define HE_PARAMS_SUPPORT  RSI_DISABLE
#define GUARD_INTERVAL     3
#define NOMINAL_PE         2
#define DCM_ENABLE         0
#define LDPC_ENABLE        0
#define NG_CB_ENABLE       0
#define NG_CB_VALUES       0
#define UORA_ENABLE        0
#define TRIGGER_RESP_IND   0xF
#define IPPS_VALID_VALUE   0
#define TX_ONLY_ON_AP_TRIG 0
#define TWT_SUPPORT        RSI_DISABLE
/*========================================================================*/
#endif
