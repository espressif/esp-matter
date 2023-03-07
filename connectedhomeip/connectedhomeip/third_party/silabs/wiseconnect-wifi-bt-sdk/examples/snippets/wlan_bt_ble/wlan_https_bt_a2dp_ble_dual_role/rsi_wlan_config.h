/*******************************************************************************
* @file  rsi_wlan_config_DEMO_57.h
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
 * @file         rsi_wlan_config_DEMO_57.h
 * @version      0.1
 * @date         01 Feb 2020
 *
 *
 *
 *  @brief : This file contains user configurable details to configure the device  
 *
 *  @section Description  This file contains user configurable details to configure the device 
 */
#ifndef SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_WLAN_CONFIG_COEX_MAX_APP_H
#define SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_WLAN_CONFIG_COEX_MAX_APP_H

/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/
#include <rsi_common_app.h>
//#include "http_client.h" // To remove errors
//#if COEX_MAX_APP
#include <rsi_bt.h>
/***********************************************************************************************************************************************/
//! APP CONFIG defines
/***********************************************************************************************************************************************/
#define SSID          "Tplink" //! Access point SSID to connect
#define SECURITY_TYPE RSI_WPA2 //! Security type -  RSI_WPA2 or RSI_OPEN
#define CHANNEL_NO    0
#define PSK           "12345678" //! Password
#define DHCP_MODE     1          //! DHCP mode 1- Enable 0- Disable
//! If DHCP mode is disabled give IP statically
#if !(DHCP_MODE)
#define DEVICE_IP "192.168.0.101" //0x6500A8C0 //! IP address of the module  - E.g: 0x650AA8C0 == 192.168.0.101
#define GATEWAY   "192.168.0.1"   //0x0100A8C0 //! IP address of Gateway  - E.g: 0x010AA8C0 == 192.168.0.1
#define NETMASK   "255.255.255.0" //0x00FFFFFF //! IP address of netmask - E.g: 0x00FFFFFF == 255.255.255.0
#endif
#define SERVER_PORT       5001            //! Server port number
#define SERVER_IP_ADDRESS "192.168.0.109" //! Server IP address
#define RSI_DNS_CLIENT    0               //! Enable if using DNS client (when using server hostname instead of ip addr)

#define TX_DATA        0 //! Enable this to test TCP transmit
#define RX_DATA        1 //! Enable this to test HTTP/S download
#define HTTPS_DOWNLOAD 0 //! Enable this to test HTTPS download and also set RX_DATA to '1'
#if HTTPS_DOWNLOAD
#define SSL              1 //! Enable SSL or not
#define LOAD_CERTIFICATE 1 //! Load certificate to device flash :
#if RSI_DNS_CLIENT
//! TO-DO: This feature is to be implemeted
#define DOWNLOAD_FILENAME "https://gaana.com"
#else
#define DOWNLOAD_FILENAME "dltest.txt" //! HTTPs resource name
#define BYTES_TO_RECEIVE  6144         //! size of file to download from remote server
#endif
#else
#define DOWNLOAD_FILENAME "dltestdata32.txt" //"15MB.mp3"
#define BYTES_TO_RECEIVE  1048576            //14919768 //! size of DOWNLOAD_FILENAME
#endif
#define CONTINUOUS_HTTP_DOWNLOAD       1
#define SOCKTEST_INSTANCES_MAX         1       //! No. of sockets to test
#define NO_OF_ITERATIONS               1       //! No. of iterations to repeat for each socket
#define BYTES_TO_TRANSMIT              1048576 //! size of file to send to remote server
#define VERIFY_RX_DATA                 0       //! to verify receive data, set this to '1'
#define SELECT_ON_WRITEFDS             0
#define USE_CONNECTION_CLOSE           0
#define TEST_SOCKET_SHUTDOWN           0
#define TLS_SNI_FEATURE                0
#define SOCK_NON_BLOCK                 0
#define BIG_ENDIAN_CONVERSION_REQUIRED 1
#define SOCKET_ASYNC_FEATURE           0
#define WINDOW_UPDATE_FEATURE          0
#define WLAN_THROUGHPUT_ENABLE         0
#define PER_TEST_TX_ENABLE             0 //! TX PER test
#define PER_TEST_RX_ENABLE             0 //! RX PER test

#if (PER_TEST_TX_ENABLE || PER_TEST_RX_ENABLE)
#define PER_TX_TIMEOUT         30000 // PER TX timeout in milliseconds
#define PER_RX_TIMEOUT         30000 // PER RX timeout in milliseconds
#define RSI_TX_TEST_POWER      4
#define RSI_TX_TEST_RATE       RSI_RATE_5_5
#define RSI_TX_TEST_LENGTH     1500 // 24 to 1500 bytes in Burst mode; 24 to 260 bytes in continuous mode
#define RSI_TX_TEST_MODE       RSI_BURST_MODE
#define RSI_TX_TEST_CHANNEL    1
#define RSI_TX_TEST_RATE_FLAGS BIT(2) // RSI_ENABLE_SHORT_GI, RSI_ENABLE_GREEN_FIELD_SUPPORT
#if !RSI_BURST_MODE
#define PER_NO_OF_PKTS          25
#define RSI_TX_TEST_DELAY       0 // In microseconds
#define RSI_TX_TEST_AGGR_ENABLE 0
#endif
#define RSI_ANTENNA         1 //! Select Intenal antenna or uFL connector
#define RSI_ANTENNA_GAIN_2G 0 //! Antenna gain in 2.4GHz band
#define RSI_ANTENNA_GAIN_5G 0 //! Antenna gain in 5GHz band
#endif

#define PING_TEST 0 //! Enable to test PING from Wlan otherwise disable to test https download.
#if PING_TEST
#define REMOTE_IP         0x0101A8C0 //! 192.168.1.1
#define PING_SIZE         20
#define PING_PACKET_COUNT 1000
#endif

#define HTTP_USER_AGENT_HEADER "User-Agent: redpine/1.0.4a"
#define HTTP_ACCEPT_HEADER     "Accept: */*"
#define RSI_APP_BUF_SIZE       1600
#define THROUGHPUT_EN          0  //!Disabled by default
#define STRING_HELPER(x)       #x // turns x into a string
// Two macros are required to convert a macro value to a string
// ex: #define THE_NUMBER_NINE 9
//     STRING_HELPER(THE_NUMBER_NINE) -> "THE_NUMBER_NINE"
//     TOSTRING(THE_NUMBER_NINE) -> STRING_HELPER(9) -> "9"
#define TOSTRING(x)                   STRING_HELPER(x)
#define MAX_DATA_PKTS                 100
#define SD_DEMO                       1
#define MAX_DATA_TO_WRITE_INTO_SDCARD (8 * 1024)
#define SD_WRITE_SZ                   (2 * 1024)
//! By default the app does scan,connect,ipdone,http/https downloads. To restrict the wlan scan only or wlan connect only case need to enable either or manner.
#define WLAN_SCAN_ONLY          0 //! make it 1 for wlan scan in loop.
#define WLAN_CONNECT_ONLY       0 //! make it 1 for be wlan as connected stand-by.
#define COMPUTE_WLAN_THROUGHPUT 1 //! for throughput calculations
/*=======================================================================*/
//!	Powersave configurations
/*=======================================================================*/
#if WLAN_THROUGHPUT_ENABLE
#define ENABLE_POWER_SAVE 0 //! Set to 1 for powersave mode
#else
#define ENABLE_POWER_SAVE 0 //1 //! Set to 1 for powersave mode
#endif
#define PSP_MODE RSI_SLEEP_MODE_2
#define PSP_TYPE RSI_MAX_PSP

/*=======================================================================*/
//!WMM PS parameters
/*=======================================================================*/

#define RSI_WMM_PS_ENABLE        RSI_ENABLE //! set WMM enable or disable
#define RSI_WMM_PS_TYPE          1          //! set WMM enable or disable  //! 0- TX BASED 1 - PERIODIC
#define RSI_WMM_PS_WAKE_INTERVAL 30         //! set WMM wake up interval
#define RSI_WMM_PS_UAPSD_BITMAP  15         //! set WMM UAPSD bitmap

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
#define TOTAL_SOCKETS       SOCKTEST_INSTANCES_MAX //@ Total number of sockets. TCP TX + TCP RX + UDP TX + UDP RX
#define TOTAL_TCP_SOCKETS   SOCKTEST_INSTANCES_MAX //@ Total TCP sockets. TCP TX + TCP R
#define TCP_RX_ONLY_SOCKETS SOCKTEST_INSTANCES_MAX //@ Total TCP RX only sockets. TCP RX
#if WLAN_THROUGHPUT_ENABLE
#define TCP_RX_WINDOW_SIZE_CAP   20 //@ TCP RX Window size - 64K (44 * 1460)
#define TCP_RX_WINDOW_DIV_FACTOR 0
#else
#define TCP_RX_WINDOW_SIZE_CAP   90 //@ TCP RX Window size - 64K (44 * 1460)
#define TCP_RX_WINDOW_DIV_FACTOR 90
#endif
#define BT_PACKET_TYPE_SEL BT_EDR_2DH3_PACKETS_ONLY

/*=======================================================================*/
//! opermode command paramaters
/*=======================================================================*/
//! To set wlan feature select bit map
#define RSI_FEATURE_BIT_MAP (FEAT_ULP_GPIO_BASED_HANDSHAKE | FEAT_DEV_TO_HOST_ULP_GPIO_1 | FEAT_SECURITY_OPEN)

//! TCP IP BYPASS feature check
#define RSI_TCP_IP_BYPASS RSI_DISABLE

//! TCP/IP feature select bitmap for selecting TCP/IP features
#if PING_TEST
#define RSI_TCP_IP_FEATURE_BIT_MAP                                                                    \
  (TCP_IP_FEAT_DHCPV4_CLIENT | TCP_IP_FEAT_SSL | TCP_IP_FEAT_DNS_CLIENT | TCP_IP_FEAT_EXTENSION_VALID \
   | TCP_IP_FEAT_ICMP)
#else
#define RSI_TCP_IP_FEATURE_BIT_MAP \
  (TCP_IP_FEAT_DHCPV4_CLIENT | TCP_IP_FEAT_SSL | TCP_IP_FEAT_DNS_CLIENT | TCP_IP_FEAT_EXTENSION_VALID)
#endif

#if BT_PWR_CTRL_DISABLE
#define RSI_BT_FEATURE_BITMAP                                                                            \
  (A2DP_PROFILE_ENABLE | A2DP_SOURCE_ROLE_ENABLE | BT_PACKET_TYPE_SEL | BT_RF_TYPE | ENABLE_BLE_PROTOCOL \
   | (BT_PWR_CTRL << 2) | (BT_PWR_INX << 3))
#else
#define RSI_BT_FEATURE_BITMAP \
  (A2DP_PROFILE_ENABLE | A2DP_SOURCE_ROLE_ENABLE | BT_PACKET_TYPE_SEL | BT_RF_TYPE | ENABLE_BLE_PROTOCOL)
#endif

#if WINDOW_UPDATE_FEATURE
#define RSI_EXT_TCPIP_FEATURE_BITMAP                                                                               \
  (EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE | EXT_DYNAMIC_COEX_MEMORY | EXT_TCP_IP_WINDOW_DIV | EXT_TCP_IP_TOTAL_SELECTS_4 \
   | EXT_TCP_IP_BI_DIR_ACK_UPDATE | EXT_TCP_DYNAMIC_WINDOW_UPDATE_FROM_HOST)
#elif WLAN_THROUGHPUT_ENABLE
#define RSI_EXT_TCPIP_FEATURE_BITMAP                                                                           \
  (EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE | EXT_TCP_IP_CERT_BYPASS | EXT_DYNAMIC_COEX_MEMORY | EXT_TCP_IP_WINDOW_DIV \
   | EXT_TCP_IP_TOTAL_SELECTS_4 | EXT_TCP_IP_BI_DIR_ACK_UPDATE | EXT_TCP_IP_WINDOW_SCALING                     \
   | EXT_TCP_IP_SSL_16K_RECORD)
#else
#define RSI_EXT_TCPIP_FEATURE_BITMAP                                                                           \
  (EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE | EXT_TCP_IP_CERT_BYPASS | EXT_DYNAMIC_COEX_MEMORY | EXT_TCP_IP_WINDOW_DIV \
   | EXT_TCP_IP_TOTAL_SELECTS_4 | EXT_TCP_IP_BI_DIR_ACK_UPDATE)
#endif

//! To set custom feature select bit map
#define RSI_CUSTOM_FEATURE_BIT_MAP FEAT_CUSTOM_FEAT_EXTENTION_VALID

//! To set Extended custom feature select bit map
#if ENABLE_1P8V
#if (RSI_ENABLE_BT_TEST && TA_BASED_ENCODER)
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (EXT_FEAT_XTAL_CLK_ENABLE | EXT_FEAT_384K_MODE | EXT_FEAT_1P8V_SUPPORT)
#else
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP \
  (EXT_FEAT_LOW_POWER_MODE | EXT_FEAT_XTAL_CLK_ENABLE | EXT_FEAT_384K_MODE | EXT_FEAT_1P8V_SUPPORT)
#endif
#else
#if (RSI_ENABLE_BT_TEST && TA_BASED_ENCODER)
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (EXT_FEAT_XTAL_CLK_ENABLE | EXT_FEAT_384K_MODE)
#else
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (EXT_FEAT_LOW_POWER_MODE | EXT_FEAT_XTAL_CLK_ENABLE | EXT_FEAT_384K_MODE)
#endif
#endif

//! including the common defines
#include "rsi_wlan_common_config.h"
//#endif
#endif /* SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_CONFIG_BLE_DUAL_MODE_BT_A2DP_SOURCE_WIFI_HTTP_S_RX_H */
