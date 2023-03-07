/*******************************************************************************
* @file  rsi_wlan_sta_tx_config_DEMO_57.h
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
 * @file         rsi_wlan_sta_tx_config_DEMO_57.h
 * @version      0.1
 * @date         13 Aug 2018
 *
 *
 *
 *  @brief : This file contains user configurable details to configure the device  
 *
 *  @section Description  This file contains user configurable details to configure the device 
 *
 *
 */
#ifndef SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_CONFIG_WIFI_TCP_TX_POWERSAVE_H
#define SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_CONFIG_WIFI_TCP_TX_POWERSAVE_H
/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/
#include <rsi_common_app.h>

//#if COEX_MAX_APP //WIFI_TCP_TX_POWERSAVE
/***********************************************************************************************************************************************/
//! WIFI_TCP_TX_POWERSAVE APP CONFIG defines
/***********************************************************************************************************************************************/
#define RSI_APP_BUF_SIZE 1600
//#define THROUGHPUT_EN      1
#define PSP_TYPE           RSI_MAX_PSP
#define REQ_THRUPT_IN_MBPS 10 //! 1Mbps - 1; 5Mbps - 5 ; 10Mbps - 10
//#define SSID               "riyaan" //"safoorah"//"Rahamat"//"riyaan" // Enter SSID of AP to connect
#define SECURITY_TYPE RSI_WPA2 //! RSI_WPA2 or RSI_OPEN  //! Security type
//#define PSK                "riyaan@5914"  //"12345678"//"Arsheen5914"//"riyaan@5914"  //! Password

#define DHCP_MODE 1 //! DHCP mode 1- Enable 0- Disable
//! If DHCP mode is disabled give IP statically
#if !(DHCP_MODE)
#define DEVICE_IP 0x6500A8C0 //! IP address of the module   //! E.g: 0x650AA8C0 == 192.168.0.101
#define GATEWAY   0x0100A8C0 //! IP address of Gateway  //! E.g: 0x010AA8C0 == 192.168.0.1
#define NETMASK   0x00FFFFFF //! IP address of netmask //! E.g: 0x00FFFFFF == 255.255.255.0
#endif

//#define SOCKET_ASYNC_FEATURE 1
#define SERVER_PORT 5001 //! Server Port number
//#define SERVER_IP_ADDRESS    "192.168.0.3"//"192.168.43.55" //"192.168.43.34" //"192.168.0.8"//  //! 192.168.2.3 //! Server IP
#define UAPSD_POWER_SAVE 0

#define SD_DEMO                1
#define HTTP_USER_AGENT_HEADER "User-Agent: redpine/1.0.4a"
#define HTTP_ACCEPT_HEADER     "Accept: */*"

/***********************************************************************************************************************************************/
//! WLAN SAPI CONFIG DEFINES
/***********************************************************************************************************************************************/
#define RSI_ENABLE      1           //! Enable feature
#define RSI_DISABLE     0           //! Disable feature
#define CONCURRENT_MODE RSI_DISABLE //! To enable concurrent mode

#define BT_PACKET_TYPE_SEL BT_EDR_2DH3_PACKETS_ONLY
/*=======================================================================*/
//! Opermode command parameters
/*=======================================================================*/
#define RSI_FEATURE_BIT_MAP                                    \
  (FEAT_ULP_GPIO_BASED_HANDSHAKE | FEAT_DEV_TO_HOST_ULP_GPIO_1 \
   | FEAT_SECURITY_OPEN)              //! To set wlan feature select bit map
#define RSI_TCP_IP_BYPASS RSI_DISABLE //! TCP IP BYPASS feature check
#define RSI_TCP_IP_FEATURE_BIT_MAP                                      \
  (TCP_IP_FEAT_DHCPV4_CLIENT | TCP_IP_FEAT_SSL | TCP_IP_FEAT_DNS_CLIENT \
   | TCP_IP_FEAT_EXTENSION_VALID) //! TCP/IP feature select bitmap for selecting TCP
/*#define RSI_EXT_TCPIP_FEATURE_BITMAP                                                                               \
  (EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE | EXT_DYNAMIC_COEX_MEMORY | EXT_TCP_IP_WINDOW_DIV | EXT_TCP_IP_TOTAL_SELECTS_4 \
   | EXT_TCP_IP_BI_DIR_ACK_UPDATE) //! To set custom feature select bit map*/
#if COEX_MAX_APP
#if (WIFI_TCP_TX_POWERSAVE == 1)
#define RSI_BT_FEATURE_BITMAP ((A2DP_SOURCE_PROFILE_ENABLE) | (BT_PACKET_TYPE_SEL))
#endif
#endif
#define RSI_CUSTOM_FEATURE_BIT_MAP FEAT_CUSTOM_FEAT_EXTENTION_VALID //! To set Extended custom feature select bit map

/*=======================================================================*/
//! To set Extended custom feature select bit map
/*=======================================================================*/
#if ENABLE_1P8V
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP \
  (EXT_FEAT_LOW_POWER_MODE | EXT_FEAT_XTAL_CLK_ENABLE | EXT_FEAT_384K_MODE | EXT_FEAT_1P8V_SUPPORT)
#else
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (EXT_FEAT_LOW_POWER_MODE | EXT_FEAT_XTAL_CLK_ENABLE | EXT_FEAT_384K_MODE)
#endif

/*=======================================================================*/
#define FEATURE_ENABLES                                         \
  (RSI_FEAT_FRAME_PREAMBLE_DUTY_CYCLE | RSI_FEAT_FRAME_LP_CHAIN \
   | RSI_FEAT_FRAME_IN_PACKET_DUTY_CYCLE) //49  //! LP chain
//! Power save command parameters
/*=======================================================================*/
#define RSI_HAND_SHAKE_TYPE GPIO_BASED //! set handshake type of power mode
#define RSI_SELECT_LP_OR_ULP_MODE \
  RSI_ULP_WITH_RAM_RET //! 0 - LP, 1- ULP mode with RAM retention and 2 - ULP with Non RAM retention

//! set DTIM alignment required
//! 0 - module wakes up at beacon which is just before or equal to listen_interval
//! 1 - module wakes up at DTIM beacon which is just before or equal to listen_interval
#define RSI_DTIM_ALIGNED_TYPE 0

//! Monitor interval for the FAST PSP mode
//! default is 50 ms, and this parameter is valid for FAST PSP only
#define RSI_MONITOR_INTERVAL 50

/*=======================================================================*/
//!WMM PS parameters
/*=======================================================================*/
#if UAPSD_POWER_SAVE

#define RSI_WMM_PS_ENABLE        RSI_ENABLE //! set WMM enable or disable
#define RSI_WMM_PS_TYPE          1          //! set WMM enable or disable   //! 0- TX BASED 1 - PERIODIC
#define RSI_WMM_PS_WAKE_INTERVAL 30         //! set WMM wake up interval
#define RSI_WMM_PS_UAPSD_BITMAP  15         //! set WMM UAPSD bitmap

#else
//#define RSI_WMM_PS_ENABLE        RSI_DISABLE //! set WMM enable or disable
//#define RSI_WMM_PS_TYPE          0           //! set WMM enable or disable  //! 0- TX BASED 1 - PERIODIC
//#define RSI_WMM_PS_WAKE_INTERVAL 20          //! set WMM wake up interval
#define RSI_WMM_PS_UAPSD_BITMAP 15 //! set WMM UAPSD bitmap
#endif

/*=======================================================================*/
//! Socket configuration
/*=======================================================================*/
#define HIGH_PERFORMANCE_ENABLE RSI_ENABLE //@ RSI_ENABLE or RSI_DISABLE High performance socket
//#define TOTAL_SOCKETS                   1          //@ Total number of sockets. TCP TX + TCP RX + UDP TX + UDP RX
//#define TOTAL_TCP_SOCKETS               1          //@ Total TCP sockets. TCP TX + TCP RX
#define TOTAL_UDP_SOCKETS   0 //@ Total UDP sockets. UDP TX + UDP RX
#define TCP_TX_ONLY_SOCKETS 0 //@ Total TCP TX only sockets. TCP TX
//#define TCP_RX_ONLY_SOCKETS             1          //@ Total TCP RX only sockets. TCP RX
#define UDP_TX_ONLY_SOCKETS             0 //@ Total UDP TX only sockets. UDP TX
#define UDP_RX_ONLY_SOCKETS             0 //@ Total UDP RX only sockets. UDP RX
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS 1 //@ Total TCP RX High Performance sockets
//#define TCP_RX_WINDOW_SIZE_CAP          10         //@ TCP RX Window size - 14k (10 * 1460)
//#define TCP_RX_WINDOW_DIV_FACTOR        44
/*=======================================================================*/

//! including the common defines
#include "rsi_wlan_common_config.h"
//#endif
#endif /* SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_CONFIG_WIFI_TCP_TX_POWERSAVE_H */
