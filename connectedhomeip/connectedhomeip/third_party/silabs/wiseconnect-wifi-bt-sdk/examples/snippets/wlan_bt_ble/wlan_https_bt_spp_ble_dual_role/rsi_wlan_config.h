/*******************************************************************************
* @file  rsi_wlan_config.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @file         rsi_wlan_config.h
 * @version      0.1
 * @date         01 Feb 2021
 *
 *
 *
 *  @brief : This file contains user configurable details to configure the device  
 *
 *  @section Description  This file contains user configurable details to configure the device 
 */
#ifndef RSI_WLAN_CONFIG_H
#define RSI_WLAN_CONFIG_H
/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/
#include "rsi_wlan_defines.h"
//#include <rsi_bt.h>
/***********************************************************************************************************************************************/
//! APP CONFIG defines
/**********************************************************************************************************************************************/
#define SSID          "SILABS_AP" //! Access point SSID to connect
#define SECURITY_TYPE RSI_WPA2    //! Security type -  RSI_WPA2 or RSI_OPEN
#define CHANNEL_NO    0
#define PSK           "1234567890" //! Password
#define DHCP_MODE     1            //! DHCP mode 1- Enable 0- Disable
//! If DHCP mode is disabled give IP statically
#if !(DHCP_MODE)
#define DEVICE_IP "192.168.0.101" //0x6500A8C0 //! IP address of the module  - E.g: 0x650AA8C0 == 192.168.0.101
#define GATEWAY   "192.168.0.1"   //0x0100A8C0 //! IP address of Gateway  - E.g: 0x010AA8C0 == 192.168.0.1
#define NETMASK   "255.255.255.0" //0x00FFFFFF //! IP address of netmask - E.g: 0x00FFFFFF == 255.255.255.0
#endif
#define SERVER_PORT          80              //443             //! Server port number
#define SERVER_IP_ADDRESS    "192.168.0.103" //! Server IP address
#define RSI_DNS_CLIENT       0 //! Enable if using DNS client (when using server hostname instead of ip addr)
#define WLAN_SCAN_ONLY       0 //! Enable this for wlan scanning only case
#define WLAN_CONNECTION_ONLY 0 //! Enable this for wlan connection only case

#define HTTPS_DOWNLOAD 0 //! Enable this to test HTTPS download and also set RX_DATA to '1'
#if HTTPS_DOWNLOAD
#define SSL              1 //! Enable SSL or not
#define LOAD_CERTIFICATE 1 //! Load certificate to device flash :
#if RSI_DNS_CLIENT
//! TO-DO: This feature is to be implemeted
#define DOWNLOAD_FILENAME "https://gaana.com"
#endif
#endif
#define DOWNLOAD_FILENAME "5MB.txt"

#define CONTINUOUS_HTTP_DOWNLOAD 1
#define SOCKET_ASYNC_FEATURE     0
#define RSI_APP_BUF_SIZE         1600
#define RECV_BUFFER_SIZE         1500

#define USE_CONNECTION_CLOSE 0 //! 1- Module sends connection close headers to server

//! Enumeration for states in application
typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_INITIAL_STATE = 0,
  RSI_WLAN_UNCONNECTED_STATE,
  RSI_WLAN_SCAN_STATE,
  RSI_WLAN_JOIN_STATE,
  RSI_WLAN_CONNECTED_STATE,
  RSI_WLAN_IPCONFIG_DONE_STATE,
  RSI_WLAN_DISCONNECTED_STATE,
  RSI_WLAN_SOCKET_CONNECTED_STATE,
  RSI_WLAN_DATA_RECEIVE_STATE,
  RSI_POWER_SAVE_STATE
} rsi_wlan_app_state_t;

//! WLAN application control block
typedef struct rsi_wlan_app_cb_s {
  volatile rsi_wlan_app_state_t state; //! WLAN application state
  uint32_t length;                     //! length of buffer to copy
  uint8_t buffer[RSI_APP_BUF_SIZE];    //! application buffer
  uint8_t buf_in_use;                  //! to check application buffer availability
  uint32_t event_map;                  //! application events bit map
} rsi_wlan_app_cb_t;

/*=======================================================================*/
//!	Powersave configurations
/*=======================================================================*/
#define ENABLE_POWER_SAVE 0 //! Set to 1 for powersave mode
#define PSP_MODE          RSI_SLEEP_MODE_2
#define PSP_TYPE          RSI_MAX_PSP

/*=======================================================================*/
//!WMM PS parameters
/*=======================================================================*/

#define RSI_WMM_PS_ENABLE        RSI_DISABLE //! set WMM enable or disable
#define RSI_WMM_PS_TYPE          0           //! set WMM enable or disable  //! 0- TX BASED 1 - PERIODIC
#define RSI_WMM_PS_WAKE_INTERVAL 20          //! set WMM wake up interval
#define RSI_WMM_PS_UAPSD_BITMAP  15          //! set WMM UAPSD bitmap

/*=======================================================================*/
//! Feature frame parameters
/*=======================================================================*/
#if (ENABLE_POWER_SAVE)
#define FEATURE_ENABLES \
  (RSI_FEAT_FRAME_PREAMBLE_DUTY_CYCLE | RSI_FEAT_FRAME_LP_CHAIN | RSI_FEAT_FRAME_IN_PACKET_DUTY_CYCLE)
#else
#define FEATURE_ENABLES 0
#endif

/***********************************************************************************************************************************************/
//! WLAN SAPI CONFIG DEFINES
/***********************************************************************************************************************************************/
#define RSI_ENABLE      1           //! Enable feature
#define RSI_DISABLE     0           //! Disable feature
#define CONCURRENT_MODE RSI_DISABLE //! To enable concurrent mode

/*=======================================================================*/
//! BT power control
/*=======================================================================*/
#define BT_PWR_CTRL_DISABLE 0
#define BT_PWR_CTRL         1
#define BT_PWR_INX          10

/*=======================================================================*/
//! Power save command parameters
/*=======================================================================*/
#define RSI_HAND_SHAKE_TYPE GPIO_BASED //! set handshake type of power mode

/*=======================================================================*/
//! Socket configuration
/*=======================================================================*/
#define TCP_RX_WINDOW_SIZE_CAP   20 //@ TCP RX Window size - 64K (44 * 1460)
#define TCP_RX_WINDOW_DIV_FACTOR 0

/*=======================================================================*/
//! opermode command paramaters
/*=======================================================================*/
//! To set wlan feature select bit map
#define RSI_FEATURE_BIT_MAP (FEAT_ULP_GPIO_BASED_HANDSHAKE | FEAT_DEV_TO_HOST_ULP_GPIO_1 | FEAT_SECURITY_OPEN)

//! TCP IP BYPASS feature check
#define RSI_TCP_IP_BYPASS RSI_DISABLE

#define RSI_TCP_IP_FEATURE_BIT_MAP \
  (TCP_IP_FEAT_DHCPV4_CLIENT | TCP_IP_FEAT_SSL | TCP_IP_FEAT_DNS_CLIENT | TCP_IP_FEAT_EXTENSION_VALID)

#define RSI_EXT_TCPIP_FEATURE_BITMAP                                                                               \
  (EXT_DYNAMIC_COEX_MEMORY | EXT_TCP_IP_WINDOW_DIV | EXT_TCP_IP_TOTAL_SELECTS_4 | EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE \
   | EXT_TCP_IP_BI_DIR_ACK_UPDATE)

//! To set custom feature select bit map
#define RSI_CUSTOM_FEATURE_BIT_MAP FEAT_CUSTOM_FEAT_EXTENTION_VALID

//! To set Extended custom feature select bit map
#if ENABLE_1P8V
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP \
  (EXT_FEAT_LOW_POWER_MODE | EXT_FEAT_XTAL_CLK_ENABLE | EXT_FEAT_384K_MODE | EXT_FEAT_1P8V_SUPPORT)
#else
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (EXT_FEAT_LOW_POWER_MODE | EXT_FEAT_XTAL_CLK_ENABLE | EXT_FEAT_384K_MODE)
#endif
#define RSI_BT_FEATURE_BITMAP (BT_RF_TYPE | ENABLE_BLE_PROTOCOL)

/*=======================================================================*/
//! Rejoin parameters
/*=======================================================================*/

//! RSI_ENABLE or RSI_DISABLE rejoin params
#define RSI_REJOIN_PARAMS_SUPPORT RSI_ENABLE

//! Rejoin retry count. If 0 retries infinity times
#define RSI_REJOIN_MAX_RETRY 10

//! Periodicity of rejoin attempt
#define RSI_REJOIN_SCAN_INTERVAL 4

//! Beacon missed count
#define RSI_REJOIN_BEACON_MISSED_COUNT 40

//! RSI_ENABLE or RSI_DISABLE retry for first time join failure
#define RSI_REJOIN_FIRST_TIME_RETRY RSI_DISABLE

/*=======================================================================*/
//! including the common defines
#include "rsi_wlan_common_config.h"
#endif /* TESTAPPS_WLAN_HTTPS_BT_SPP_BLE_MULTICONNECTION_RSI_WLAN_CONFIG_H_ */
