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

#include "rsi_wlan_defines.h"
#include "rsi_os.h"
#include "rsi_common_config.h"

/*=======================================================================================================*/
//!	 APP CONFIG defines
/*=======================================================================================================*/
#define SSID               "SILABS_AP"     //! Access point SSID to connect
#define SECURITY_TYPE      RSI_WPA2        //! Security type -  RSI_WPA2 or RSI_OPEN
#define CHANNEL_NO         0               //! Channel in which device should scan, 0 - scan all channels
#define PSK                "1234567890"    //! Password
#define DEVICE_PORT        5001            //! Module port number
#define SERVER_PORT        5001            //! Server port number
#define SERVER_IP_ADDRESS  "192.168.0.103" //! Server IP address
#define SSL_RX_SERVER_PORT 5002            //! Server port number of SSL client
#define DHCP_MODE          1               //! DHCP mode 1- Enable 0- Disable
#if !(DHCP_MODE)                           //! If DHCP mode is disabled give IP statically
#define DEVICE_IP \
  "192.168.0.101" //0x6500A8C0               //! IP address of the module  - E.g: 0x650AA8C0 == 192.168.0.101
#define GATEWAY "192.168.0.1"   //0x0100A8C0               //! IP address of Gateway  - E.g: 0x010AA8C0 == 192.168.0.1
#define NETMASK "255.255.255.0" //0x00FFFFFF               //! IP address of netmask - E.g: 0x00FFFFFF == 255.255.255.0
#endif

//! Type of throughput
#define UDP_RX 1
#define UDP_TX 2
#define TCP_TX 4
#define TCP_RX 8
#define SSL_TX 16
#define SSL_RX 32
//#define UDP_BIDIRECTIONAL (UDP_TX + UDP_RX)		//! Note: This application won't support bidirectional throughputs
//#define TCP_BIDIRECTIONAL (TCP_TX + TCP_RX)
//#define SSL_BIDIRECTIONAL (SSL_TX + SSL_RX)

#define THROUGHPUT_TYPE TCP_TX //TCP_BIDIRECTIONAL

#define TOTAL_PROTOCOLS_CNT 6
#define TCP_BUFF_SIZE       1460
#define UDP_BUFF_SIZE       1470
#define SSL_BUFF_SIZE       1370
#define DEFAULT_BUFF_SIZE   1460

#define THROUGHPUT_AVG_TIME 60000 //! throughput average time in ms
#define MAX_TX_PKTS         10000 //! Applies in SSL TX and UDP_RX, calculate throughput after transmitting MAX_TX_PKTS
#define CONTINUOUS_THROUGHPUT \
  0                          /* Enable this only while measuring wlan alone throughput
                              1- Measure throughput for every interval of 'THROUGHPUT_AVG_TIME'
                              0- Measure throughput only once for interval of 'THROUGHPUT_AVG_TIME' */
#define RSI_DNS_CLIENT     0 //! Enable if using DNS client (when using server hostname instead of ip addr)
#define TX_RX_RATIO_ENABLE 0
#define RSI_SSL_BIT_ENABLE 0

#define TX_DATA 0 //! Enable this to test TCP transmit
#if ((THROUGHPUT_TYPE == SSL_TX) || (THROUGHPUT_TYPE == SSL_RX) /*|| (THROUGHPUT_TYPE == SSL_BIDIRECTIONAL)*/)
#define SSL 1
//! Load certificate to device flash :
//! Certificate could be loaded once and need not be loaded for every boot up
#define LOAD_CERTIFICATE 1
#endif

#define SOCKTEST_INSTANCES_MAX 1 //! No. of sockets to test
#define SOCKET_ASYNC_FEATURE   1
#define RSI_APP_BUF_SIZE       1600

/*=======================================================================*/
//!	Powersave configurations
/*=======================================================================*/
#define ENABLE_POWER_SAVE 0 //! Set to 1 for powersave mode
#define PSP_MODE          RSI_SLEEP_MODE_2
#define PSP_TYPE          RSI_MAX_PSP

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

#define BT_PACKET_TYPE_SEL BT_EDR_2DH3_PACKETS_ONLY

/*=======================================================================*/
//! opermode command paramaters
/*=======================================================================*/
//! To set wlan feature select bit map
#define RSI_FEATURE_BIT_MAP \
  (FEAT_ULP_GPIO_BASED_HANDSHAKE | FEAT_DEV_TO_HOST_ULP_GPIO_1 | FEAT_SECURITY_OPEN | FEAT_AGGREGATION)

//! TCP IP BYPASS feature check
#define RSI_TCP_IP_BYPASS RSI_DISABLE

#define RSI_TCP_IP_FEATURE_BIT_MAP \
  (TCP_IP_FEAT_DHCPV4_CLIENT | TCP_IP_FEAT_SSL | TCP_IP_FEAT_DNS_CLIENT | TCP_IP_FEAT_EXTENSION_VALID)

#define RSI_EXT_TCPIP_FEATURE_BITMAP                                                      \
  (BIT(16) | EXT_DYNAMIC_COEX_MEMORY | EXT_TCP_IP_WINDOW_DIV | EXT_TCP_IP_TOTAL_SELECTS_4 \
   | EXT_TCP_IP_BI_DIR_ACK_UPDATE)

//! Enable CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ only in Wlan mode
//! To set custom feature select bit map
#if (RSI_ENABLE_BT_TEST || RSI_ENABLE_BLE_TEST)
#define RSI_CUSTOM_FEATURE_BIT_MAP FEAT_CUSTOM_FEAT_EXTENTION_VALID
#else
#define RSI_CUSTOM_FEATURE_BIT_MAP FEAT_CUSTOM_FEAT_EXTENTION_VALID | CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ
#endif
//! To set Extended custom feature select bit map
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (/*EXT_FEAT_LOW_POWER_MODE | */ EXT_FEAT_XTAL_CLK_ENABLE | EXT_FEAT_384K_MODE)

#define RSI_BT_FEATURE_BITMAP (BT_RF_TYPE | ENABLE_BLE_PROTOCOL)

//! Enumeration for states in application
typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_INITIAL_STATE          = 0,
  RSI_WLAN_UNCONNECTED_STATE      = 1,
  RSI_WLAN_SCAN_DONE_STATE        = 2,
  RSI_WLAN_CONNECTED_STATE        = 3,
  RSI_WLAN_IPCONFIG_DONE_STATE    = 4,
  RSI_WLAN_DISCONNECTED_STATE     = 5,
  RSI_WLAN_SOCKET_CONNECTED_STATE = 6,
  RSI_POWER_SAVE_STATE            = 7,
  RSI_WLAN_IDLE_STATE             = 8,

} rsi_wlan_app_state_t;

//! WLAN application control block
typedef struct rsi_wlan_app_cb_s {
  rsi_wlan_app_state_t state;       //! WLAN application state
  uint32_t length;                  //! length of buffer to copy
  uint8_t buffer[RSI_APP_BUF_SIZE]; //! application buffer
  uint8_t buf_in_use;               //! to check application buffer availability
  uint32_t event_map;               //! application events bit map
} rsi_wlan_app_cb_t;

typedef struct wlan_throughput_config_s {
  rsi_semaphore_handle_t wlan_app_sem;
  rsi_semaphore_handle_t wlan_app_sem1;
  uint8_t thread_id;
  uint8_t throughput_type;
  //uint32_t server_port;
  //uint8_t *server_addr;
} wlan_throughput_config_t;

/*=======================================================================*/
//! Feature frame parameters
/*=======================================================================*/
#if (RSI_ENABLE_BT_TEST || RSI_ENABLE_BLE_TEST)
#define PLL_MODE 0
#else
#define PLL_MODE 1
#endif

#define RF_TYPE       1 //! 0 - External RF 1- Internal RF
#define WIRELESS_MODE 0
#define ENABLE_PPP    0
#define AFE_TYPE      1

/*=======================================================================*/
//! High Throughput Capabilies related information
/*=======================================================================*/
//! HT caps supported
//! HT caps bit map.
#define RSI_HT_CAPS_BIT_MAP \
  (RSI_HT_CAPS_NUM_RX_STBC | RSI_HT_CAPS_SHORT_GI_20MHZ | RSI_HT_CAPS_GREENFIELD_EN | RSI_HT_CAPS_SUPPORT_CH_WIDTH)
/*=======================================================================*/
//! Rejoin parameters
/*=======================================================================*/
//! RSI_ENABLE or RSI_DISABLE rejoin params
#define RSI_REJOIN_PARAMS_SUPPORT RSI_ENABLE
//! Rejoin retry count. If 0 retries infinity times
#define RSI_REJOIN_MAX_RETRY 1

#include <rsi_wlan_common_config.h>
#endif
