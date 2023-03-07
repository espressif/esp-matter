/*******************************************************************************
* @file  rsi_wlan_apis.h
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

#ifndef RSI_WLAN_APIS_H
#define RSI_WLAN_APIS_H

#include <rsi_data_types.h>
#include "rsi_user.h"
#ifdef RSI_M4_INTERFACE
#include "base_types.h"
#endif
/******************************************************
 * *                      Macros
 * ******************************************************/
// To enable A2DP profile
#define A2DP_SOURCE_PROFILE_ENABLE BIT(24) // Added to avoid compilation errors

// Enable feature
#define RSI_ENABLE 1
// Disable feature
#define RSI_DISABLE 0

// Enable profile config
#define RSI_ENABLE_CFG_PROFILE 2

// eap username length
#define RSI_EAP_USER_NAME_LENGTH 64

// eap password length
#define RSI_EAP_PASSWORD_LENGTH 128

// total no of sockets
#define RSI_MN_NUM_SOCKETS 10

// max no of stations
#define RSI_MAX_STATIONS 8

// Maximum length of SSID
#define RSI_SSID_LEN 34

#define RSI_PSK_LEN 64
#define RSI_PMK_LEN 32

// Maximum Acccess points that can be scanned
#define RSI_AP_SCANNED_MAX 11

// MAC address length
#define RSI_MAC_ADDR_LEN 6

// Maximum wifi direct device count
#define RSI_MAX_WFD_DEVICE_COUNT 10

#define RSI_INSTANT_BGSCAN 1

#define RSI_BGSCAN_STOP 0

// To provide additional scan results to host
#define RSI_SCAN_RESULTS_TO_HOST BIT(1)
// feature bit map
/*=========================================================================*/
// Securtity type open
#define FEAT_SECURITY_OPEN BIT(0)

// Securtity type WPA/WPA2
#define FEAT_SECURITY_PSK BIT(1)

// Aggregation support
#define FEAT_AGGREGATION BIT(2)

// LP mode GPIO handshake
#define FEAT_LP_GPIO_BASED_HANDSHAKE BIT(3)

// ULP mode GPIO based handshake
#define FEAT_ULP_GPIO_BASED_HANDSHAKE BIT(4)

// To select ULP GPIO 1 for wake up indication
#define FEAT_DEV_TO_HOST_ULP_GPIO_1 BIT(5)

// To supply 3.3 volt supply
#define FEAT_RF_SUPPY_VOL_3_3_VOLT BIT(6)

// To Disable WPS in AP mode
#define FEAT_WPS_DISABLE BIT(7)

// To enable EAP-LEAP mode
#define FEAT_EAP_LEAP_IN_COEX BIT(8)
/*=========================================================================*/

// BT feature bit map
/*=========================================================================*/
#define BT_BDR_MODE_ENABLE          BIT(0)
#define BT_BDR_MODE_LP_CHAIN_ENABLE BIT(1)
//!TA based encoder
#define TA_BASED_ENCODER_ENABLE BIT(14)
//! To enable HFP profile
#define HFP_PROFILE_ENABLE BIT(15)
//! To enable A2DP profile
#define A2DP_PROFILE_ENABLE BIT(23)
//! To enable A2DP Profile role as source/sink
#define A2DP_SOURCE_ROLE_ENABLE BIT(24)
//! To enable A2DP Accelerator mode
#define A2DP_ACCELERATOR_MODE_ENABLE BIT(25)
//! To enable Buffer Alignment for Test Mode
#define BT_BLE_CP_BUFF_SIZE BIT(27)
//! To bt rf type
#define BT_RF_TYPE BIT(30)
//! To enable ble protocol
#define ENABLE_BLE_PROTOCOL BIT(31)
/*=========================================================================*/

// TCP/IP feature bit map parameters description !//
/*=========================================================================*/
// TCP/IP bypass feature
#define TCP_IP_FEAT_BYPASS BIT(0)

// Enable HTTP server feature
#define TCP_IP_FEAT_HTTP_SERVER BIT(1)

// Enable DHCPv4 client feature
#define TCP_IP_FEAT_DHCPV4_CLIENT BIT(2)

// Enable DHCPv6 client feature
#define TCP_IP_FEAT_DHCPV6_CLIENT BIT(3)

// Enable DHCPv4 server feature
#define TCP_IP_FEAT_DHCPV4_SERVER BIT(4)

// Enable DHCPv6 server feature
#define TCP_IP_FEAT_DHCPV6_SERVER BIT(5)

// Enable JSON objects
#define TCP_IP_FEAT_JSON_OBJECTS BIT(6)

// Enable HTTP client
#define TCP_IP_FEAT_HTTP_CLIENT BIT(7)

// Enable DNS client
#define TCP_IP_FEAT_DNS_CLIENT BIT(8)

// Enable SNMP client
#define TCP_IP_FEAT_SNMP_AGENT BIT(9)

// Enable SSL feature
#define TCP_IP_FEAT_SSL BIT(10)

// Enable ICMP feature(ping)
#define TCP_IP_FEAT_ICMP BIT(11)

// Enable HTTP client
#define TCP_IP_FEAT_HTTPS_SERVER BIT(12)

// Enable sending web page configuration to host from wireless config page
#define TCP_IP_FEAT_SEND_CONFIGS_TO_HOST BIT(14)

// Enable FTP client
#define TCP_IP_FEAT_FTP_CLIENT BIT(15)

// Enable SNTP client
#define TCP_IP_FEAT_SNTP_CLIENT BIT(16)

// Enable  IPV6 support
#define TCP_IP_FEAT_IPV6 BIT(17)

// Enable  Raw data support
#define TCP_IP_FEAT_RAW_DATA BIT(18)

// Enable MDNSD
#define TCP_IP_FEAT_MDNSD BIT(19)

// Enable SMTP client
#define TCP_IP_FEAT_SMTP_CLIENT BIT(20)

// Enable Single SSL socket
#define TCP_IP_FEAT_SINGLE_SSL_SOCKET BIT(25)

// Enable to Load public and private keys for TLS and SSL handshake
#define TCP_IP_FEAT_LOAD_PUBLIC_PRIVATE_CERTS BIT(26)

// Enable to Load SSL certificates in to RAM
#define TCP_IP_FEAT_LOAD_CERTS_INTO_RAM BIT(27)

// Enable POP3 client
#define TCP_IP_FEAT_POP3_CLIENT BIT(29)

// Enable OTAF client
#define TCP_IP_FEAT_OTAF BIT(30)

#define TCP_IP_FEAT_EXTENSION_VALID BIT(31)

// Extended TCP/IP feature bit map parameters description !//
/*=========================================================================*/
#define EXT_TCP_FEAT_DHCP_OPT77 BIT(1)

// Enable HTTP server root path (Default configuration page) bypass
#define EXT_TCP_IP_HTTP_SERVER_BYPASS BIT(2)

// TCP bi-dir ack update
#define EXT_TCP_IP_BI_DIR_ACK_UPDATE BIT(3)

// TCP RX window division
#define EXT_TCP_IP_WINDOW_DIV BIT(4)

// SSL server certificate bypass, validation from the host
#define EXT_TCP_IP_CERT_BYPASS BIT(5)

// SSL 16K record size support
#define EXT_TCP_IP_SSL_16K_RECORD BIT(6)

// Enable DNS client byass
#define EXT_TCP_IP_DNS_CLIENT_BYPASS BIT(7)

// Enable TCP window scaling feature
#define EXT_TCP_IP_WINDOW_SCALING BIT(8)

// Enables both TCP/IP bypass mode & embedded modes
#define EXT_TCP_IP_DUAL_MODE_ENABLE BIT(9)

// Enables Ethernet to WIFI bridge
#define EXT_TCP_IP_ETH_WIFI_BRIDGE BIT(10)

// Enables the Dynamic coex memory
#define EXT_DYNAMIC_COEX_MEMORY          BIT(11)
#define EXT_TCP_IP_WAIT_FOR_SOCKET_CLOSE BIT(16)
// Enable Embedded/internal MQTT
#define EXT_EMB_MQTT_ENABLE         BIT(17)
#define EXT_TCP_IP_TOTAL_SELECTS_1  (1 << 12)
#define EXT_TCP_IP_TOTAL_SELECTS_2  (2 << 12)
#define EXT_TCP_IP_TOTAL_SELECTS_3  (3 << 12)
#define EXT_TCP_IP_TOTAL_SELECTS_4  (4 << 12)
#define EXT_TCP_IP_TOTAL_SELECTS_5  (5 << 12)
#define EXT_TCP_IP_TOTAL_SELECTS_6  (6 << 12)
#define EXT_TCP_IP_TOTAL_SELECTS_7  (7 << 12)
#define EXT_TCP_IP_TOTAL_SELECTS_8  (8 << 12)
#define EXT_TCP_IP_TOTAL_SELECTS_9  (9 << 12)
#define EXT_TCP_IP_TOTAL_SELECTS_10 (10 << 12)

// Enables the SSL_HIGH_PERFORMANCE
#define EXT_TCP_IP_FEAT_SSL_HIGH_PERFORMANCE BIT(18)

// Enabled to update TCP window from host
#define EXT_TCP_DYNAMIC_WINDOW_UPDATE_FROM_HOST BIT(19)
#define EXT_TCP_MAX_RECV_LENGTH                 BIT(20)

#define EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS BIT(29)
#define EXT_TCP_IP_FEAT_SSL_MEMORY_CLOUD  BIT(30)
#define CONFIG_FEAT_EXTENTION_VALID       BIT(31)

// Select No of sockets
#define TCP_IP_TOTAL_SOCKETS_1  (1 << 21)
#define TCP_IP_TOTAL_SOCKETS_2  (2 << 21)
#define TCP_IP_TOTAL_SOCKETS_3  (3 << 21)
#define TCP_IP_TOTAL_SOCKETS_4  (4 << 21)
#define TCP_IP_TOTAL_SOCKETS_5  (5 << 21)
#define TCP_IP_TOTAL_SOCKETS_6  (6 << 21)
#define TCP_IP_TOTAL_SOCKETS_7  (7 << 21)
#define TCP_IP_TOTAL_SOCKETS_8  (8 << 21)
#define TCP_IP_TOTAL_SOCKETS_9  (9 << 21)
#define TCP_IP_TOTAL_SOCKETS_10 (10 << 21)
/*=========================================================================*/

// Custom feature bit map paramters description !//
/*=========================================================================*/

// Disables gateway config sent to STA from RSI AP
#define CUSTOM_FEAT_DISABLE_GATEWAY_IN_RSI_AP BIT(2)

// To configure the clock for NWP SOC 160Mhz
// If higher performance is needed(like High throughput)
// then this configuration is needed
#define CUSTOM_FEAT_SOC_CLK_CONFIG_160MHZ BIT(4)
// If this bit is set, AP is created in hidden mode
// This bit is valid only in case of AP mode
#define CUSTOM_FEAT_AP_IN_HIDDEN_MODE BIT(5)

// DNS server IP address in DHCP offer response in AP mode
#define CUSTOM_FEAT_DNS_SERVER_IN_DHCP_OFFER BIT(6)

// Support for scanning in DFS channels() in 5GHZ band
// This bit is valid in WiFi client mode
#define CUSTOM_FEAT_DFS_CHANNEL_SUPPORT BIT(8)

// If this bit is set it enables the LED blinking feature
// after module initialisation.LED (GPIO_16) is used for
// this feature and blinks when any TX Packet on air happens
// and when we get a unicast packets addressing to our MAC
#define CUSTOM_FEAT_LED_FEATURE BIT(9)

// If this bit is enabled,module indicates the host
// the wlan connection status asynchronously
// This bit is valid in case of Wifi client mode
#define CUSTOM_FEAT_ASYNC_CONNECTION_STATUS BIT(10)

// Wake on wireless indication in UART mode
#define CUSTOM_FEAT_WAKE_ON_WIRELESS BIT(11)

// Enables AP blacklisting in STA mode
#define CUSTOM_FEAT_ENABLE_AP_BLACKLIST BIT(12)

// Number of clients to support in AP/WFD mode
#define MAX_NUM_OF_CLIENTS             1
#define CUSTOM_FEAT_MAX_NUM_OF_CLIENTS ((MAX_NUM_OF_CLIENTS & 0xF) << 13)

// select between de-authentication or null data (with power management bit set) based roaming.
#define CUSTOM_FEAT_ROAM_WITH_DEAUTH_OR_NULLDATA BIT(17)

// Trigger Auto Configuration
#define CUSTOM_FEAT_TRIGGER_AUTO_CONFIG BIT(20)

// In AP mode, If set only two packets per STA will be buffered when STA is in PS
#define CUSTOM_FEAT_LIMIT_PACKETS_PER_STA BIT(22)

#define CUSTOM_FEAT_HTTP_HTTPS_AUTH BIT(23)

// To configure the clock for NWP SOC 120Mhz
// If higher performance is needed(like High throughput)
// then this configuration is needed
#define CUSTOM_FEAT_SOC_CLK_CONFIG_120MHZ BIT(24)

// HTTP server credentials to host in get configuration command
#define CUSTOM_FEAT_HTTP_SERVER_CRED_TO_HOST BIT(25)

// For a LTCP socket when maximum clients are connected if a new connection request is received, then this connection request will be rejected immediately
#define CUSTOM_FEAT_REJECT_CONNECT_REQ_IMMEDIATELY BIT(26)

// Enables Dual band roaming and vcsafd feature
#define CUSTOM_FEAT_DUAL_BAND_ROAM_VCSAFD BIT(27)

// Real time clock from host
#define CUSTOM_FEAT_RTC_FROM_HOST BIT(28)

#define CUSTOM_FEAT_BT_IAP BIT(29)

// Extention valid to use Extended custom feature bitmap
#define FEAT_CUSTOM_FEAT_EXTENTION_VALID BIT(31)

/*=========================================================================*/

// Extended custom feature bitmap !//
/*=========================================================================*/
// To support 4096 size RSA KEY certificate
#define EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT BIT(1)

// Extended custom bitmap to support TELEC
#define EXT_FEAT_TELEC_SUPPORT BIT(2)

// To support 4096 size KEY SSL certificate
#define EXT_FEAT_SSL_CERT_WITH_4096_KEY_SUPPORT BIT(3)

// Extended custom bitmap for AP Broadcast customization
#define EXT_FEAT_AP_BROADCAST_PKT_SND_B4_DTIM BIT(4)

// Extended custom bitmap to support FCC
#define EXT_FEAT_FCC_LOW_PWR BIT(5)

// To enable PUF
#define EXT_FEAT_PUF BIT(7)

// Nokia Spectral mask extended custom bitmap
#define EXT_FEAT_SPECTRAL_MASK_NOKIA BIT(8)

// Extended feature bit map to skip default leading character '\' in HTTP header
#define EXT_HTTP_SKIP_DEFAULT_LEADING_CHARACTER BIT(9)

// To enable PUF private key
#define EXT_FEAT_PUF_PRIVATE_KEY BIT(10)

// To enable 802.11R Over The Air Roaming
#define EXT_FEAT_ENABLE_11R_OTA BIT(11)

// To enable 802.11J protocol
#define EXT_FEAT_IEEE_80211J BIT(12)

// To enable 802.11W protocol
#define EXT_FEAT_IEEE_80211W BIT(13)

// To enable the Multiverion TCP over SSL support
#define EXT_FEAT_SSL_VERSIONS_SUPPORT BIT(14)

// To Enable 16 client support
#define EXT_FEAT_16th_STATION_IN_AP_MODE BIT(15)

// To enable 802.11R Over the Distribution System Roaming
#define EXT_FEAT_ENABLE_11R_ODS BIT(16)

// To enable low power mode in Wlan
#define EXT_FEAT_LOW_POWER_MODE BIT(19)

// To enable http otaf support
#define EXT_FEAT_HTTP_OTAF_SUPPORT BIT(18)

#ifdef CHIP_9117

// To enable 448K memory for TA
#define EXT_FEAT_448K_M4SS_256K BIT(21)
// To enable 512K memory for TA
#define EXT_FEAT_512K_M4SS_192K BIT(20)
// To enable 704K memory for TA
#define EXT_FEAT_704K_M4SS_0K (BIT(20) | BIT(21))

#define EXT_FEAT_256K_MODE 0
#else //defaults

// To enable 320K memory for TA
#define EXT_FEAT_320K_MODE BIT(20)
// To enable 256K memory for TA
#define EXT_FEAT_256K_MODE BIT(21)
// To enable 384K memory for TA
#define EXT_FEAT_384K_MODE (BIT(20) | BIT(21))

#endif

// To enable CRYSTAL for TA
#ifdef CHIP_9117
#define EXT_FEAT_XTAL_CLK_ENABLE BIT(22)
#else
#if (RS9116_SILICON_CHIP_VER == CHIP_VER_1P4_AND_ABOVE)
#define EXT_FEAT_XTAL_CLK_ENABLE BIT(23)
#else
#define EXT_FEAT_XTAL_CLK_ENABLE BIT(22)
#endif
#endif

// To intimate FW not to modify MDNS text record
#define EXT_FEAT_HOMEKIT_WAC_ENABLED BIT(24)

// To enable 1.8v support for TA
#define EXT_FEAT_1P8V_SUPPORT BIT(25)

// To select UART debug prints pin selection
// If BIT(27) is enabled,Debug prints are supported on UART1
// If BIT(27) is disabled,Debug prints are supported on UART2
#define EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS BIT(27)

// If this bit is enabled,NWP disables Debug prints support
#define EXT_FEAT_DISABLE_DEBUG_PRINTS BIT(28)

// config feature bitmap
/*=========================================================================*/
#define RSI_FEAT_SLEEP_GPIO_SEL_BITMAP BIT(0)
// DVS Dynamic Voltage Selection
#define RSI_FEAT_DVS_SEL_CONFIG_1 BIT(2)
#define RSI_FEAT_DVS_SEL_CONFIG_2 BIT(3)
#define RSI_FEAT_DVS_SEL_CONFIG_3 BIT(4)
#define RSI_FEAT_DVS_SEL_CONFIG_4 BIT(5)
// External PMU Selection
#define RSI_EXTERNAL_PMU_GOOD_TIME_100us  BIT(6)
#define RSI_EXTERNAL_PMU_GOOD_TIME_200us  BIT(7)
#define RSI_EXTERNAL_PMU_GOOD_TIME_300us  (BIT(6) | BIT(7))
#define RSI_EXTERNAL_PMU_GOOD_TIME_400us  BIT(8)
#define RSI_EXTERNAL_PMU_GOOD_TIME_500us  (BIT(6) | BIT(8))
#define RSI_EXTERNAL_PMU_GOOD_TIME_600us  (BIT(7) | BIT(8))
#define RSI_EXTERNAL_PMU_GOOD_TIME_700us  (BIT(6) | BIT(7) | BIT(8))
#define RSI_EXTERNAL_PMU_GOOD_TIME_800us  BIT(9)
#define RSI_EXTERNAL_PMU_GOOD_TIME_900us  (BIT(6) | BIT(9))
#define RSI_EXTERNAL_PMU_GOOD_TIME_1000us (BIT(7) | BIT(9))
#define RSI_EXTERNAL_PMU_GOOD_TIME_1100us (BIT(6) | BIT(7) | BIT(9))
#define RSI_EXTERNAL_PMU_GOOD_TIME_1200us (BIT(8) | BIT(9))
#define RSI_EXTERNAL_PMU_GOOD_TIME_1300us (BIT(6) | BIT(8) | BIT(9))
#define RSI_EXTERNAL_PMU_GOOD_TIME_1400us (BIT(7) | BIT(8) | BIT(9))
#define RSI_EXTERNAL_PMU_GOOD_TIME_1500us (BIT(6) | BIT(7) | BIT(8) | BIT(9))
// Extrenal LDO voltage selection
#define RSI_FEAT_EXTERNAL_LDO_SEL BIT(10)
// This field valid only if RSI_FEAT_EXTERNAL_LDO_SEL is enabled i.e BIT(10) is set
// if this bit set means 1.0V selected else 1.1V selected
#define RSI_FEAT_EXTERNAL_LDO_VOL BIT(11)
// TLS version selection
#define RSI_FEAT_EAP_TLS_V1P0                BIT(14)
#define RSI_FEAT_EAP_TLS_V1P2                BIT(15)
#define FEAT_CONC_STA_AP_DYN_SWITCH_SEL      BIT(17)
#define ULP_GPIO9_FOR_UART2_TX               BIT(18)
#define RSI_FEAT_DISABLE_MCS_5_6_7_DATARATES BIT(19)
#define RSI_FEAT_DISABLE_SHORT_GI            BIT(20)
//! XTAL goodtime configurations
#define RSI_XTAL_GOODTIME_1000us 0
#define RSI_XTAL_GOODTIME_2000us BIT(24)
#define RSI_XTAL_GOODTIME_3000us BIT(25)
#define RSI_XTAL_GOODTIME_600us  (BIT(24) | BIT(25))

#define ENABLE_ENHANCED_MAX_PSP    BIT(26)
#define ENABLE_DEBUG_BBP_TEST_PINS BIT(27)
/*=========================================================================*/

/*=========================================================================*/
// Feature enable paramters
#define RSI_FEAT_FRAME_PREAMBLE_DUTY_CYCLE  BIT(0)
#define RSI_FEAT_FRAME_LP_CHAIN             BIT(4)
#define RSI_FEAT_FRAME_IN_PACKET_DUTY_CYCLE BIT(5)
/*=========================================================================*/

// BAND COMMAND PARAMETERS OPTIONS
/*=========================================================================*/

// module operates in 2.4GHz band
#define RSI_BAND_2P4GHZ 0

// Module operates in 5GHz band
#define RSI_BAND_5GHZ 1

// Module operates in 4.9GHz band
#define RSI_BAND_4_9GHZ 1

// Module opertes in both 2.4GHz and 5GHz  band
// This option is valid only in case of Wifi-Direct
// and WiFi client mode
#define RSI_DUAL_BAND 2

/*=========================================================================*/

// SCAN COMMAND PARAMETERS OPTIONS
/*=========================================================================*/

// Scan feature bitmap paramters !//

// If this bit is set,module scans for the AP given in scan API
// and posts the scan results immediately to the host after finding
// one Accesspoint.This bit is valid only if specific channel and
// ssid to scan is given.
#define RSI_ENABLE_QUICK_SCAN BIT(0)

/*=========================================================================*/

// ACCESS POINT CONFIGURATION PARAMETERS DESCRIPTIONS
/*=========================================================================*/

// AP keep alive type bit !//

// If this bit is enabled, AP performs keep alive functionality
// by sending NULL DATA packet to the station. If no ACK is received
// from the station after specific no of retries, AP discards the station
#define RSI_NULL_BASED_KEEP_ALIVE (BIT(0) | BIT(1))

// If this bit is enabled, AP performs keep alive functionality
// based on the rx packets received from its stations. If no packet is
// received from the station with in time out, AP discards it
#define RSI_DEAUTH_BASED_KEEP_ALIVE BIT(0)

// HT capabilities bitmap paramters !//
// Bit map corresponding to high throughput capabilities
// HT capabilities bitmap is valid only if HT capabilites RSI_ENABLE_HT_CAPABILITIES
// macro is enabled.

// If this bit is set,Short GI support for 20Mhz band width
#define RSI_ENABLE_SHORT_GI BIT(5)

// If these bit is set, RX STBC support is enabled in AP mode
#define RSI_ENABLE_RX_STBC_SUPPORT (BIT(8) | ~BIT(9))

// If these bit is set, Green field support is enabled in AP mode
#define RSI_ENABLE_GREEN_FIELD_SUPPORT BIT(4)

/*=========================================================================*/

// JOIN COMMAND PARAMETERS OPTIONS
/*=========================================================================*/

// Tx Power level !//
// Low Tx power level
// (7+/-1)dBm in 2.4GHz band
// (5+/-1)dBm in 5GHz band
#define RSI_POWER_LEVEL_LOW 0

// Medium Tx power level !//
// (5+/-1)dBm in 2.4GHz band
// (7+/-1)dBm in 5GHz band
#define RSI_POWER_LEVEL_MEDIUM 1

// High Tx power level !//
// Highest power that can be
// used in that channel
#define RSI_POWER_LEVEL_HIGH 2

// To enable b/g only mode in station mode
#define RSI_JOIN_FEAT_STA_BG_ONLY_MODE_ENABLE BIT(0)

// To take listen interval from join command.
#define RSI_JOIN_FEAT_LISTEN_INTERVAL_VALID BIT(1)

// To enable quick join feature
#define RSI_JOIN_FEAT_QUICK_JOIN BIT(2)

//To enable CCXV2 feature
#define RSI_JOIN_FEAT_CCXV2_FEATURE BIT(3)

//To connect to AP based on BSSID together with configured SSID
#define RSI_JOIN_FEAT_BSSID_BASED BIT(4)

// MFP Type
#define RSI_JOIN_FEAT_MFP_CAPABLE_ONLY     BIT(5)
#define RSI_JOIN_FEAT_MFP_CAPABLE_REQUIRED (BIT(5) | BIT(6))

// listen interval from power save command
#define RSI_JOIN_FEAT_PS_CMD_LISTEN_INTERVAL_VALID BIT(7)

// DATA Rates used //
#define RSI_DATA_RATE_AUTO    0
#define RSI_DATA_RATE_1       1
#define RSI_DATA_RATE_2       2
#define RSI_DATA_RATE_5P5     3
#define RSI_DATA_RATE_11      4
#define RSI_DATA_RATE_6       5
#define RSI_DATA_RATE_9       6
#define RSI_DATA_RATE_12      7
#define RSI_DATA_RATE_18      8
#define RSI_DATA_RATE_24      9
#define RSI_DATA_RATE_36      10
#define RSI_DATA_RATE_48      11
#define RSI_DATA_RATE_54      12
#define RSI_DATA_RATE_MCS0    13
#define RSI_DATA_RATE_MCS1    14
#define RSI_DATA_RATE_MCS2    15
#define RSI_DATA_RATE_MCS3    16
#define RSI_DATA_RATE_MCS4    17
#define RSI_DATA_RATE_MCS5    18
#define RSI_DATA_RATE_MCS6    19
#define RSI_DATA_RATE_MCS7    20
#define RSI_DATA_RATE_HE_MCS0 21
#define RSI_DATA_RATE_HE_MCS1 22
#define RSI_DATA_RATE_HE_MCS2 23
#define RSI_DATA_RATE_HE_MCS3 24
#define RSI_DATA_RATE_HE_MCS4 25
#define RSI_DATA_RATE_HE_MCS5 26
#define RSI_DATA_RATE_HE_MCS6 27
#define RSI_DATA_RATE_HE_MCS7 28
/*=========================================================================*/

// Multicast filter cmds
/*=========================================================================*/
#define RSI_MULTICAST_MAC_ADD_BIT   0
#define RSI_MULTICAST_MAC_CLEAR_BIT 1
#define RSI_MULTICAST_MAC_CLEAR_ALL 2
#define RSI_MULTICAST_MAC_SET_ALL   3

/*=========================================================================*/

// SSL features
/*=========================================================================*/

//Release 2.0 default ciphers
#define SSL_DEFAULT_CIPHERS                                                                    \
  (BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 | BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256           \
   | BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384 | BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256     \
   | BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 | BIT_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 \
   | BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA | BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA           \
   | BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA | BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA               \
   | BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA | BIT_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA          \
   | BIT_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA | BIT_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA)
//Release 2.0 all configurable ciphers
#define SSL_RELEASE_2_0_ALL_CIPHERS                                                                       \
  (SSL_DEFAULT_CIPHERS | BIT_TLS_RSA_WITH_AES_256_CBC_SHA256 | BIT_TLS_RSA_WITH_AES_128_CBC_SHA256        \
   | BIT_TLS_RSA_WITH_AES_256_CBC_SHA | BIT_TLS_RSA_WITH_AES_128_CBC_SHA | BIT_TLS_RSA_WITH_AES_128_CCM_8 \
   | BIT_TLS_RSA_WITH_AES_256_CCM_8)

#ifdef CHIP_9117
#define RSI_SSL_EXT_CIPHERS SSL_TLSV1_3_ALL_CIPHERS
#endif

#define SSL_ALL_CIPHERS SSL_RELEASE_2_0_ALL_CIPHERS

//TLSv1.3 configurable ciphers
#ifdef CHIP_9117
#define SSL_TLSV1_3_ALL_CIPHERS                                                                     \
  (BIT_TLS13_AES_128_GCM_SHA256 | BIT_TLS13_AES_256_GCM_SHA384 | BIT_TLS13_CHACHA20_POLY1305_SHA256 \
   | BIT_TLS13_AES_128_CCM_SHA256 | BIT_TLS13_AES_128_CCM_8_SHA256)
#endif

//DHE_RSA in combination with CBC secure ciphers
#define BIT_DHE_RSA_CBC (BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 | BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256)
//ECDHE_RSA in combination with CBC secure ciphers
#define BIT_ECDHE_RSA_CBC (BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384 | BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256)
//ECDHE_ECDSA in combination with CBC secure ciphers
#define BIT_ECDHE_ECDSA_CBC (BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 | BIT_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256)
//DHE_RSA in combination with GCM secure ciphers
#define BIT_DHE_RSA_GCM (BIT_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 | BIT_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384)
//ECDHE_RSA in combination with GCM secure ciphers
#define BIT_ECDHE_RSA_GCM (BIT_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 | BIT_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384)

#define BIT_TLS_RSA_WITH_AES_256_CBC_SHA256         BIT(0)
#define BIT_TLS_RSA_WITH_AES_128_CBC_SHA256         BIT(1)
#define BIT_TLS_RSA_WITH_AES_256_CBC_SHA            BIT(2)
#define BIT_TLS_RSA_WITH_AES_128_CBC_SHA            BIT(3)
#define BIT_TLS_RSA_WITH_AES_128_CCM_8              BIT(4)
#define BIT_TLS_RSA_WITH_AES_256_CCM_8              BIT(5)
#define BIT_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256     BIT(8)
#define BIT_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384     BIT(9)
#define BIT_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256   BIT(10)
#define BIT_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384   BIT(11)
#define BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256     BIT(14)
#define BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256     BIT(15)
#define BIT_TLS_DHE_RSA_WITH_AES_256_CBC_SHA        BIT(16)
#define BIT_TLS_DHE_RSA_WITH_AES_128_CBC_SHA        BIT(17)
#define BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384   BIT(18)
#define BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256   BIT(19)
#define BIT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA      BIT(20)
#define BIT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA      BIT(21)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 BIT(22)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 BIT(23)
#define BIT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA    BIT(24)
#define BIT_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA       BIT(25)
#define BIT_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA     BIT(26)
#define BIT_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA   BIT(27)
#define SSL_NEW_CIPHERS                             BIT(31)

// TLSv1.3 supported ciphers
#ifdef CHIP_9117
#define BIT_TLS13_AES_128_GCM_SHA256       BIT(0)
#define BIT_TLS13_AES_256_GCM_SHA384       BIT(1)
#define BIT_TLS13_CHACHA20_POLY1305_SHA256 BIT(2)
#define BIT_TLS13_AES_128_CCM_SHA256       BIT(3)
#define BIT_TLS13_AES_128_CCM_8_SHA256     BIT(4)
#endif

/*=========================================================================*/

// ssl version
#define RSI_SSL_V_1_0 BIT(2)
#define RSI_SSL_V_1_2 BIT(3)
#define RSI_SSL_V_1_1 BIT(4)
#ifdef CHIP_9117
#define RSI_SSL_V_1_3 BIT(8)
#endif

// socket feature
/*=======================================================================*/

// Type of service. possible values are 0 to 7
#define RSI_TOS 0

// power save handshake types
/*=======================================================================*/
// Type of service. possible values are 0 to 7
#define MSG_BASED  1
#define GPIO_BASED 2
#define M4_BASED   3

/*=======================================================================*/

// PER mode values
/*=======================================================================*/
// Possible data rates
#define RSI_RATE_1       0x0
#define RSI_RATE_2       0x2
#define RSI_RATE_5_5     0x4
#define RSI_RATE_11      0x6
#define RSI_RATE_6       0x8b
#define RSI_RATE_9       0x8f
#define RSI_RATE_12      0x8a
#define RSI_RATE_18      0x8e
#define RSI_RATE_24      0x89
#define RSI_RATE_36      0x8d
#define RSI_RATE_48      0x88
#define RSI_RATE_54      0x8c
#define RSI_RATE_MCS0    0x100
#define RSI_RATE_MCS1    0x101
#define RSI_RATE_MCS2    0x102
#define RSI_RATE_MCS3    0x103
#define RSI_RATE_MCS4    0x104
#define RSI_RATE_MCS5    0x105
#define RSI_RATE_MCS6    0x106
#define RSI_RATE_MCS7    0x107
#define RSI_RATE_MCS7_SG 0x307

/*=========================================================================*/

/*=========================================================================*/

// Transmit test supported modes
#define RSI_BURST_MODE             0
#define RSI_CONTINUOUS_MODE        1
#define RSI_CONTINUOUS_WAVE_MODE_2 2
#define RSI_CONTINUOUS_WAVE_MODE_3 3
#define RSI_CONTINUOUS_WAVE_MODE_4 4

/*=======================================================================*/

// Power save LP/ULP sleep type
/*=======================================================================*/
#define RSI_LP_MODE             0
#define RSI_ULP_WITH_RAM_RET    1
#define RSI_ULP_WITHOUT_RAM_RET 2

/*=======================================================================*/

/*=======================================================================*/

// HT caps supported
#define RSI_HT_CAPS_NUM_RX_STBC      (1 << 8)
#define RSI_HT_CAPS_SHORT_GI_20MHZ   BIT(5)
#define RSI_HT_CAPS_GREENFIELD_EN    BIT(4)
#define RSI_HT_CAPS_SUPPORT_CH_WIDTH BIT(1)

/*=======================================================================*/

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

// enumerations for security type of access point to join
typedef enum rsi_security_mode_e {
  // open mode
  RSI_OPEN = 0,
  // WPA security with PSK
  RSI_WPA,
  // WPA2 security with PSK
  RSI_WPA2,
  // WEP security
  RSI_WEP,
  // Enterprise WPA security
  RSI_WPA_EAP,
  // Enterprise WPA2 security
  RSI_WPA2_EAP,
  // WPA/WPA2 security with PSK
  RSI_WPA_WPA2_MIXED,
  // WPA security with PMK
  RSI_WPA_PMK,
  // WPA2 security with PMK
  RSI_WPA2_PMK,
  // WPS pin method
  RSI_WPS_PIN,
  // WPS generated pin method
  RSI_USE_GENERATED_WPSPIN,
  // WPS push button method
  RSI_WPS_PUSH_BUTTON,
  // WPA/WPA2 security with PMK
  RSI_WPA_WPA2_MIXED_PMK,
  // WPA3 security with PSK
  RSI_WPA3,
  // WPA3 TRANSITION
  RSI_WPA3_TRANSITION,

} rsi_security_mode_t;

// enumerations for encryption types
typedef enum rsi_encryption_mode_e { RSI_NONE, RSI_TKIP, RSI_CCMP } rsi_encryption_mode_t;

// enumerations for certificates types
typedef enum rsi_certificate_type_e {
  RSI_EAP_CLIENT             = 1,
  RSI_FAST_PAC_FILE          = 2,
  RSI_SSL_CLIENT             = 3,
  RSI_SSL_CLIENT_PRIVATE_KEY = 4,
  RSI_SSL_CA_CERTIFICATE     = 5,
  RSI_SSL_SERVER_CERTIFICATE = 6,
  RSI_SSL_SERVER_PRIVATE_KEY = 7,
  RSI_EAP_PRIVATE_KEY        = 17,
  RSI_EAP_PUBLIC_KEY         = 33,
  RSI_EAP_CA_CERTIFICATE     = 49

} rsi_certificate_type_t;

// enumerations for set commands
typedef enum rsi_wlan_set_cmd_e {
  RSI_SET_MAC_ADDRESS  = 1,
  RSI_MULTICAST_FILTER = 2,
  RSI_JOIN_BSSID       = 3,
  RSI_CFG_SAVE         = 4,
  RSI_CFG_STORE        = 5
} rsi_wlan_set_cmd_t;

// enumerations for query commands
typedef enum rsi_wlan_query_cmd_e {
  RSI_FW_VERSION        = 1,
  RSI_MAC_ADDRESS       = 2,
  RSI_RSSI              = 3,
  RSI_WLAN_INFO         = 4,
  RSI_CONNECTION_STATUS = 5,
  RSI_STATIONS_INFO     = 6,
  RSI_SOCKETS_INFO      = 7,
  RSI_CFG_GET           = 8,
  RSI_GET_WLAN_STATS    = 9,
  RSI_WLAN_EXT_STATS    = 10
} rsi_wlan_query_cmd_t;

/******************************************************
 * *                 Type Definitions
 * ******************************************************/
// wifi direct device info structure
typedef struct rsi_wfd_device_info_s {
  // If New device  1; Device left 0
  uint8_t device_state;

  // Name the device found or left 32 bytes
  uint8_t device_name[32];

  // Mac address of the device
  uint8_t mac_address[6];

  // Type of the device 1st byte inidcates primary device type;
  // 2nd byte indicates sub catagory
  uint8_t device_type[2];

} rsi_wfd_device_info_t;

// wifi direct device info response structure
typedef struct rsi_rsp_wfd_device_info_s {
  // device count
  uint8_t device_count;

  // 32 maximum responses from scan command
  rsi_wfd_device_info_t wfd_dev_info[RSI_MAX_WFD_DEVICE_COUNT];

} rsi_rsp_wfd_device_info_t;

/*P2P connection request from wi-fi device*/
typedef struct rsi_rsp_p2p_connection_request_s {
  // device name
  uint8_t device_name[32];

} rsi_rsp_p2p_connection_request_t;

// Scan information response structure
typedef struct rsi_scan_info_s {
  // channel number of the scanned AP
  uint8_t rf_channel;

  // security mode of the scanned AP
  uint8_t security_mode;

  // rssi value of the scanned AP
  uint8_t rssi_val;

  // network type of the scanned AP
  uint8_t network_type;

  // SSID of the scanned AP
  uint8_t ssid[RSI_SSID_LEN];

  // BSSID of the scanned AP
  uint8_t bssid[RSI_MAC_ADDR_LEN];

  // reserved
  uint8_t reserved[2];
} rsi_scan_info_t;

// multicast command request structure
typedef struct rsi_req_multicast_filter_info_s {
  // cmd_type
  uint8_t cmd_type;

  // MAC address to apply filter
  uint8_t mac_address[6];

} rsi_req_multicast_filter_info_t;

// wireless information
typedef struct rsi_rsp_wireless_info_s {
  // wlan state: connected or disconnected in station mode
  // wlan state: no of stations connected in AP mode
  uint16_t wlan_state;

  // channel number of connected AP
  uint16_t channel_number;

  // uint8[32], SSID of connected access point
  uint8_t ssid[RSI_SSID_LEN];

  // Mac address
  uint8_t mac_address[6];

  // security type
  uint8_t sec_type;

  // PSK
  uint8_t psk[64];

  // uint8[4], Module IP Address
  uint8_t ipv4_address[4];

  // uint8[16], Module IPv6 Address
  uint8_t ipv6_address[16];

  // reserved1
  uint8_t reserved1[2];

  // reserved2
  uint8_t reserved2[2];

} rsi_rsp_wireless_info_t;

typedef struct rsi_go_sta_info_s {
  // IP version if the connected client
  uint8_t ip_version[2];

  // Mac Address of the connected client
  uint8_t mac[6];
  union {
    // IPv4 Address of the Connected client
    uint8_t ipv4_address[4];

    // IPv6 Address of the Connected client
    uint8_t ipv6_address[16];
  } ip_address;

} rsi_go_sta_info_t;

// stations information
typedef struct rsi_rsp_stations_info_s {
  // Number of stations Connected to GO
  uint8_t sta_count[2];

  rsi_go_sta_info_t sta_info[RSI_MAX_STATIONS];

} rsi_rsp_stations_info_t;

typedef struct wep_key_ds_s {
  uint8_t index[2];
  uint8_t key[4][32];
} wep_key_ds_t;

// Store config profile structure

typedef struct network_profile {
  uint8_t tcp_stack_used;

  uint8_t dhcp_enable;
  uint8_t ip_address[4];
  uint8_t sn_mask[4];
  uint8_t default_gw[4];

  uint8_t dhcp6_enable;
  uint8_t prefix_length[2];
  uint8_t ip6_address[16];
  uint8_t deafault_gw[16];

} network_profile_t;

typedef struct feature_frame_params_s {
  uint8_t pll_mode;
  uint8_t rf_type;
  uint8_t wireless_mode;
  uint8_t enable_ppp;
  uint8_t afe_type;
  uint8_t feature_enables[4];
} feature_frame_params_t;

typedef struct ap_profile {
  uint8_t ap_profile_magic_word[4];
  uint8_t wlan_feature_bit_map[4];
  uint8_t tcp_ip_feature_bit_map[4];
  uint8_t custom_feature_bit_map[4];
  uint8_t data_rate;
  uint8_t tx_power;
  uint8_t band;
  uint8_t channel[2];
  uint8_t ssid[RSI_SSID_LEN];
  uint8_t security_type;
  uint8_t encryption_type;
  uint8_t psk[RSI_PSK_LEN];
  uint8_t beacon_interval[2];
  uint8_t dtim_period[2];
  uint8_t keep_alive_type;
  uint8_t keep_alive_counter;
  uint8_t max_no_sta[2];

  // Network config profie
  network_profile_t network_profile;

  feature_frame_params_t feature_frame_params_s;
} ap_profile_t;

typedef struct client_profile {
  uint8_t client_profile_magic_word[4];
  uint8_t wlan_feature_bit_map[4];
  uint8_t tcp_ip_feature_bit_map[4];
  uint8_t custom_feature_bit_map[4];
  uint8_t listen_interval[4];
  uint8_t data_rate;
  uint8_t tx_power;
  uint8_t band;
  uint8_t ssid[RSI_SSID_LEN];
  uint8_t ssid_len;
  uint8_t channel[2];
  uint8_t scan_feature_bitmap;
  uint8_t scan_chan_bitmap_magic_code[2];
  uint8_t scan_chan_bitmap_2_4_ghz[4];
  uint8_t scan_chan_bitmap_5_0_ghz[4];
  uint8_t security_type;
  uint8_t encryption_type;
  uint8_t psk[RSI_PSK_LEN];
  uint8_t pmk[RSI_PMK_LEN];
  wep_key_ds_t wep_key;

  // Network config profie
  network_profile_t network_profile;
  feature_frame_params_t feature_frame_params_s;

} client_profile_t;

typedef struct eap_client_profile {
  uint8_t eap_profile_magic_word[4];
  uint8_t wlan_feature_bit_map[4];
  uint8_t tcp_ip_feature_bit_map[4];
  uint8_t custom_feature_bit_map[4];
  uint8_t listen_interval;
  uint8_t data_rate;
  uint8_t tx_power;
  uint8_t band;
  uint8_t ssid[RSI_SSID_LEN];
  uint8_t ssid_len;
  uint8_t channel[2];
  uint8_t scan_feature_bitmap;
  uint8_t scan_chan_bitmap_magic_code[2];
  uint8_t scan_chan_bitmap_2_4_ghz[4];
  uint8_t scan_chan_bitmap_5_0_ghz[4];
  uint8_t security_type;
  uint8_t eap_method[32];
  uint8_t inner_method[32];
  uint8_t user_identity[64];
  uint8_t passwd[128];

  // Network config profie
  network_profile_t network_profile;
  feature_frame_params_t feature_frame_params_s;
} eap_client_profile_t;

typedef struct p2p_profile {

  uint8_t p2p_profile_magic_word[4];
  uint8_t wlan_feature_bit_map[4];
  uint8_t tcp_ip_feature_bit_map[4];
  uint8_t custom_feature_bit_map[4];
  uint8_t data_rate;
  uint8_t tx_power;
  uint8_t band;
  uint8_t join_ssid[RSI_SSID_LEN];
  uint8_t go_intent[2];
  uint8_t device_name[64];
  uint8_t operating_channel[2];
  uint8_t ssid_postfix[64];
  uint8_t psk_key[64];

  // Network config profie
  network_profile_t network_profile;
  feature_frame_params_t feature_frame_params_s;

} p2p_profile_t;

typedef struct rsi_config_profile_s {
  uint8_t profile_type[4];

  union {
    // AP config profile
    ap_profile_t ap_profile;

    // Client config profile
    client_profile_t client_profile;

    // EAP client config profile
    eap_client_profile_t eap_client_profile;

    // P2P config profile
    p2p_profile_t p2p_profile;

  } wlan_profile_struct;

} rsi_config_profile_t;

typedef struct rsi_profile_req_s {
  uint8_t profile_type[4];

} rsi_profile_req_t;

typedef struct rsi_auto_config_enable_s {
  uint8_t config_enable;
  uint8_t profile_type[4];

} rsi_auto_config_enable_t;

// socket information structure
typedef struct rsi_sock_info_query_s {
  // 2 bytes, socket id
  uint8_t sock_id[2];

  // 2 bytes, socket type
  uint8_t sock_type[2];

  // 2 bytes, source port number
  uint8_t source_port[2];

  // 2 bytes, remote port number
  uint8_t dest_port[2];

  union {
    //  remote IPv4 Address
    uint8_t ipv4_address[4];

    //  remote IPv6 Address
    uint8_t ipv6_address[16];

  } dest_ip_address;

} rsi_sock_info_query_t;

// sockets information
typedef struct rsi_rsp_sockets_info_s {
  // number of sockets opened
  uint8_t num_open_socks[2];

  //sockets information array
  rsi_sock_info_query_t socket_info[RSI_MN_NUM_SOCKETS];

} rsi_rsp_sockets_info_t;

// Scan command response structure
typedef struct rsi_rsp_scan_s {
  // number of access points scanned
  uint8_t scan_count[4];

  // reserved
  uint8_t reserved[4];

  // scanned access points information
  rsi_scan_info_t scan_info[RSI_AP_SCANNED_MAX];

} rsi_rsp_scan_t;

// Response structure for Query firmware version command
typedef struct rsi_rsp_fw_version_s {

  // Firmware version
  uint8_t firmwre_version[20];

} rsi_rsp_fw_version_t;

// Response structure for Query RSSI command
typedef struct rsi_rsp_rssi_s {

  // RSSI value of the accesspoint to which station is connected
  uint8_t rssi_value[2];

} rsi_rsp_rssi_t;

// Response structure for Query MAC address command
typedef struct rsi_rsp_mac_s {

  // MAC address of the module
  uint8_t mac_address[6];

} rsi_rsp_mac_t;

// Response structure rx stats command
typedef struct rsi_rsp_rx_stats_s {
  // no. of tx pkts
  uint8_t tx_pkts[2];

  // no. of rx pkts
  uint8_t reserved_1[2];

  // no. of tx retries
  uint8_t tx_retries[2];

  // no. of pkts that pass crc
  uint8_t crc_pass[2];

  // no. of pkts failing crc chk
  uint8_t crc_fail[2];

  // no. of times cca got stuck
  uint8_t cca_stk[2];

  // no of times cca didn't get stuck
  uint8_t cca_not_stk[2];

  // no. of pkt aborts
  uint8_t pkt_abort[2];

  // no. of false rx starts
  uint8_t fls_rx_start[2];

  // cca idle time
  uint8_t cca_idle[2];

  // Reserved fields
  uint8_t reserved_2[26];

  // no. of rx retries
  uint8_t rx_retries[2];

  // rssi value
  uint8_t reserved_3[2];

  // cal_rssi
  uint8_t cal_rssi[2];

  // lna_gain bb_gain
  uint8_t reserved_4[4];

  // number of tx packets dropped after maximum retries
  uint8_t xretries[2];

  // consecutive pkts dropped
  uint8_t max_cons_pkts_dropped[2];

  // Reserved fields
  uint8_t reserved_5[2];

  // BSSID matched broadcast packets count
  uint8_t bss_broadcast_pkts[2];

  // BSSID matched multicast packets count
  uint8_t bss_multicast_pkts[2];

  // BSSID & multicast filter matched packets count
  uint8_t bss_filter_matched_multicast_pkts[2];

} rsi_rsp_rx_stats_t;

// IPV4 ipconfig command response  structure
typedef struct rsi_rsp_ipv4_parmas_s {
  // MAC address of this module
  uint8_t macAddr[6];

  // Configured IP address
  uint8_t ipaddr[4];

  // Configured netmask
  uint8_t netmask[4];

  // Configured default gateway
  uint8_t gateway[4];

} rsi_rsp_ipv4_parmas_t;

// IPV6 ipconfig command response structure
typedef struct rsi_rsp_ipv6_parmas_s {
  // prefix length
  uint8_t prefixLength[2];

  // Configured IPv address
  uint8_t ipaddr6[16];

  // Router IPv6 address
  uint8_t defaultgw6[16];

} rsi_rsp_ipv6_parmas_t;

// Set wep keys command request structure
typedef struct rsi_wep_keys_s {
  // select the wep key to use
  uint8_t index[2];

  // 4 wep keys
  uint8_t key[4][32];

} rsi_wep_keys_t;

typedef struct rsi_eap_credentials_s {
  // Username
  uint8_t username[RSI_EAP_USER_NAME_LENGTH];

  // Password
  uint8_t password[RSI_EAP_PASSWORD_LENGTH];

} rsi_eap_credentials_t;

/*Structure for module stats notification*/
typedef struct rsi_rsp_wlan_stats_s {
  uint8_t operating_mode;
  uint8_t dtim_period;
  uint8_t ideal_beacon_info[2];
  uint8_t busy_beacon_info[2];
  uint8_t beacon_interval[2];
} rsi_rsp_wlan_stats_t;

/*Structure for module stats notification*/
typedef struct rsi_wlan_ext_stats_s {
  uint32_t beacon_lost_count;
  uint32_t beacon_rx_count;
  uint32_t mcast_rx_count;
  uint32_t mcast_tx_count;
  uint32_t ucast_rx_count;
  uint32_t ucast_tx_count;
  uint32_t overrun_count;
} rsi_wlan_ext_stats_t;

#define RSI_PARSE_1_BYTES 1
#define RSI_PARSE_2_BYTES 2
#define RSI_PARSE_4_BYTES 4

#define IEEE80211_FC0_SUBTYPE_MASK       0xf0
#define IEEE80211_FC0_SUBTYPE_PROBE_RESP 0x50
#define IEEE80211_FC0_SUBTYPE_BEACON     0x80

/* Information Element IDs */
#define WLAN_EID_SSID            0
#define WLAN_EID_RSN             48
#define WLAN_EID_VENDOR_SPECIFIC 221
#define PAIR_WISE_CIPHER_COUNT   12
#define WIFI_OUI_RSN             4
#define IE_TYPE                  0
#define IE_LEN                   1
#define IE_POS                   2

#define SME_OPEN            0
#define SME_WPA             1
#define SME_WPA2            2
#define SME_WEP             3
#define SME_WPA_ENTERPRISE  4
#define SME_WPA2_ENTERPRISE 5
#define SME_WPA3            7
#define SME_WPA3_TRANSITION 8

#define BSSID_OFFSET         16
#define SSID_OFFSET          38
#define CAPABILITIES_OFFSET  34
#define IES_OFFSET           36
#define MINIMUM_FRAME_LENGTH 36

#define MAX_SCAN_COUNT 48

#define ETH_ALEN 6

struct wpa_scan_res {
  int freq;
  int level;
  uint8_t channel_no;
  uint8_t security_mode;
  uint8_t rssiVal;
  uint8_t uNetworkType;
  uint8_t ssid[RSI_SSID_LEN];
  uint8_t ssid_len;
  uint8_t bssid[ETH_ALEN];
};

struct wpa_scan_results_arr {

  uint16_t num;
  uint16_t sort_index[MAX_SCAN_COUNT];
  struct wpa_scan_res res[MAX_SCAN_COUNT];
};

//wlan per stats structure
typedef struct rsi_per_stats_rsp_s {
  uint16_t tx_pkts;
  uint8_t reserved_1[2];
  uint16_t tx_retries;
  uint16_t crc_pass;
  uint16_t crc_fail;
  uint16_t cca_stk;
  uint16_t cca_not_stk;
  uint16_t pkt_abort;
  uint16_t fls_rx_start;
  uint16_t cca_idle;
  uint8_t reserved_2[26];
  uint16_t rx_retries;
  uint8_t reserved_3[2];
  uint16_t cal_rssi;
  uint8_t reserved_4[4];
  uint16_t xretries;
  uint16_t max_cons_pkts_dropped;
  uint8_t reserved_5[2];
  uint16_t bss_broadcast_pkts;
  uint16_t bss_multicast_pkts;
  uint16_t bss_filter_matched_multicast_pkts;
  uint8_t eof_pkt_drop_count[4];
  uint8_t mask_pkt_drop_count[4];
  uint8_t ack_sent[4];
  //!No.of pkts rcvd with 48M
  uint16_t pkt_rcvd_with_48M;
  //!No.of pkts rcvd with 24M
  uint16_t pkt_rcvd_with_24M;
  //!No.of pkts rcvd with 12M
  uint16_t pkt_rcvd_with_12M;
  //!No.of pkts rcvd with 6M
  uint16_t pkt_rcvd_with_6M;
  //!No.of pkts rcvd with 54M
  uint16_t pkt_rcvd_with_54M;
  //!No.of pkts rcvd with 36M
  uint16_t pkt_rcvd_with_36M;
  //!No.of pkts rcvd with 18M
  uint16_t pkt_rcvd_with_18M;
  //!No.of pkts rcvd with 9M
  uint16_t pkt_rcvd_with_9M;
  //!No.of pkts rcvd with 11M
  uint16_t pkt_rcvd_with_11M;
  //!No.of pkts rcvd with 5.5M
  uint16_t pkt_rcvd_with_5M;
  //!No.of pkts rcvd with 2M
  uint16_t pkt_rcvd_with_2M;
  //!No.of pkts rcvd with 1M
  uint16_t pkt_rcvd_with_1M;
  //!No.of pkts rcvd with mcs0
  uint16_t pkt_rcvd_with_mcs0;
  //!No.of pkts rcvd with mcs1
  uint16_t pkt_rcvd_with_mcs1;
  //!No.of pkts rcvd with mcs2
  uint16_t pkt_rcvd_with_mcs2;
  //!No.of pkts rcvd with mcs3
  uint16_t pkt_rcvd_with_mcs3;
  //!No.of pkts rcvd with mcs4
  uint16_t pkt_rcvd_with_mcs4;
  //!No.of pkts rcvd with mcs5
  uint16_t pkt_rcvd_with_mcs5;
  //!No.of pkts rcvd with mcs6
  uint16_t pkt_rcvd_with_mcs6;
  //!No.of pkts rcvd with mcs7
  uint16_t pkt_rcvd_with_mcs7;
#ifdef CHIP_9117
  //!No.of pkts rcvd with 11ax SU PPDU Type
  uint16_t pkt_count_HE_SU_PPDU;
  uint16_t pkt_count_HE_ER_SU_PPDU;
  uint16_t pkt_count_HE_TRIG_PPDU;
  uint16_t pkt_count_HE_MU_PPDU;
  uint16_t pkt_count_11AC;
  uint16_t tx_done;
#endif
  uint32_t reserved_8[9];
  uint16_t noise_rssi;
  int32_t stop_per;
} rsi_per_stats_rsp_t;

// TWT User configurable parameters.
typedef struct twt_user_params_s {
  // Nominal minimum wake duration. Range : 0 - 255
  uint8_t wake_duration;
  // Wake duration tolerance. Range : 0 - 255
  uint8_t wake_duration_tol;
  // Wake interval Exponent. Range : 0 - 31
  uint8_t wake_int_exp;
  // Wake interval exponent tolerance. Range : 0 - 31
  uint8_t wake_int_exp_tol;
  // Wake interval mantissa. Range : 0 - 65535
  uint16_t wake_int_mantissa;
  // Wake interval mantissa tolerance.  Range : 0 - 65535
  uint16_t wake_int_mantissa_tol;
  // Implicit TWT : 0 or 1
  uint8_t implicit_twt;
  // Un-announced TWT : 0 or 1
  uint8_t un_announced_twt;
  // Triggered TWT : 0 or 1
  uint8_t triggered_twt;
  // Wake duration unit. 0 : 1TU = 256uSec; 1 : 1TU 1024uSec
  uint8_t wake_duration_unit;
  // TWT Channel. Range : 0 - 7 valid channels. Ignored by firmware.
  uint8_t twt_channel;
  // TWT protection : 0 or 1. Ignored by firmware.
  uint8_t twt_protection;
  // Restrict TX outside TSP : 0 or 1
  uint8_t restrict_tx_outside_tsp;
  // TWT Retry limit. Range : 0 - 15
  uint8_t twt_retry_limit;
  // TWT retry interval in seconds. Range : 5 - 255.
  uint8_t twt_retry_interval;
  // TWT Request type. 0 - Request TWT; 1 - Suggest TWT; 2 - Demand TWT
  uint8_t req_type;
} twt_user_params_t;
// TWT request structure to configure a session
typedef struct rsi_twt_req_s {
  // Nominal minimum wake duration. Range : 0 - 255
  uint8_t wake_duration;
  // Wake duration tolerance. Range : 0 - 255
  uint8_t wake_duration_tol;
  // Wake interval exponent tolerance. Range : 0 - 31
  uint8_t wake_int_exp;
  // Wake interval exponent tolerance. Range : 0 - 31
  uint8_t wake_int_exp_tol;
  // Wake interval mantissa. Range : 0 - 65535
  uint8_t wake_int_mantissa[2];
  // Wake interval mantissa tolerance.  Range : 0 - 65535
  uint8_t wake_int_mantissa_tol[2];
  // Implicit TWT : 0 or 1
  uint8_t implicit_twt;
  // Un-announced TWT : 0 or 1
  uint8_t un_announced_twt;
  // Triggered TWT : 0 or 1
  uint8_t triggered_twt;
  // Negotiation Type : 0 - Individual TWT; 1 - Broadcast TWT
  uint8_t negotiation_type;
  // TWT Channel. Range : 0 - 7
  uint8_t twt_channel;
  // TWT protection : 0 or 1
  uint8_t twt_protection;
  // TWT Session Flow ID. 0 - 7 valid. 0xFF to disable all active TWT sessions.
  uint8_t twt_flow_id;
  // Restrict TX outside TSP : 0 or 1
  uint8_t restrict_tx_outside_tsp;
  // TWT Retry limit. Range : 0 - 15
  uint8_t twt_retry_limit;
  // TWT retry interval. Range : 5 - 255
  uint8_t twt_retry_interval;
  // TWT Request type. 0 - Request TWT; 1 - Suggest TWT; 2 - Demand TWT
  uint8_t req_type;
  //TWT Enable. 0 - TWT session teardown; 1 - TWT session setup
  uint8_t twt_enable;
  // Wake duration unit. 0 - 256 TU; 1 - 1024 TU
  uint8_t wake_duration_unit;
} rsi_twt_req_t;
// TWT Response structure. This structure contains response parameters from AP.
typedef struct rsi_twt_rsp_s {
  uint8_t wake_duration;
  uint8_t wake_duration_unit;
  uint8_t wake_int_exp;
  uint8_t negotiation_type;
  uint16_t wake_int_mantissa;
  uint8_t implicit_twt;
  uint8_t un_announced_twt;
  uint8_t triggered_twt;
  uint8_t twt_channel;
  uint8_t twt_protection;
  uint8_t twt_flow_id;
} rsi_twt_rsp_t;

/******************************************************
 * *                    Structures
 * ******************************************************/

/******************************************************
 * *                 Global Variables
 * ******************************************************/

/******************************************************
 * *               Function Declarations
 * ******************************************************/
extern int32_t rsi_wlan_scan(int8_t *ssid, uint8_t chno, rsi_rsp_scan_t *result, uint32_t length);
extern int32_t rsi_wlan_scan_async(int8_t *ssid,
                                   uint8_t chno,
                                   void (*callback)(uint16_t status, const uint8_t *buffer, const uint16_t length));
extern int32_t rsi_wlan_connect(int8_t *ssid, rsi_security_mode_t sec_type, void *secret_key);
extern int32_t rsi_wlan_connect_async(int8_t *ssid,
                                      rsi_security_mode_t sec_type,
                                      void *secret_key,
                                      void (*scan_response_handler)(uint16_t status,
                                                                    const uint8_t *buffer,
                                                                    const uint16_t length));
extern int32_t rsi_wlan_ap_start(int8_t *ssid,
                                 uint8_t channel,
                                 rsi_security_mode_t security_type,
                                 rsi_encryption_mode_t encryption_mode,
                                 uint8_t *password,
                                 uint16_t beacon_interval,
                                 uint8_t dtim_period);
extern int32_t rsi_wlan_execute_post_connect_cmds(void);
extern int32_t rsi_wlan_disconnect(void);
extern int32_t rsi_wlan_disconnect_stations(uint8_t *mac_address);
extern int32_t rsi_wlan_set_certificate_index(uint8_t certificate_type,
                                              uint8_t cert_inx,
                                              uint8_t *buffer,
                                              uint32_t certificate_length);
extern int32_t rsi_wlan_set_certificate(uint8_t certificate_type, uint8_t *buffer, uint32_t certificate_length);
extern int32_t rsi_wlan_get_status(void);
extern int32_t rsi_wlan_wps_generate_pin(uint8_t *response, uint16_t length);
extern int32_t rsi_wlan_wps_enter_pin(int8_t *wps_pin);
extern int32_t rsi_wlan_wps_push_button_event(int8_t *ssid);
extern int32_t rsi_transmit_test_start(uint16_t power, uint32_t rate, uint16_t length, uint16_t mode, uint16_t channel);
extern int32_t rsi_transmit_test_stop(void);
extern int32_t rsi_wlan_receive_stats_start(uint16_t channel);
extern int32_t rsi_wlan_receive_stats_stop(void);
extern int32_t rsi_wlan_get(rsi_wlan_query_cmd_t cmd_type, uint8_t *response, uint16_t length);
extern int32_t rsi_wlan_set(rsi_wlan_set_cmd_t cmd_type, uint8_t *request, uint16_t length);
extern uint16_t rsi_wlan_register_callbacks(uint32_t callback_id,
                                            void (*callback_handler_ptr)(uint16_t status,
                                                                         uint8_t *buffer,
                                                                         const uint32_t length));
extern int32_t rsi_wlan_send_data(uint8_t *buffer, uint32_t length);
extern int32_t rsi_wlan_wfd_start_discovery(
  uint16_t go_intent,
  int8_t *device_name,
  uint16_t channel,
  int8_t *ssid_post_fix,
  uint8_t *psk,
  void (*wlan_wfd_discovery_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length),
  void (*wlan_wfd_connection_request_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length));
extern int32_t rsi_wlan_wfd_connect(int8_t *device_name,
                                    void (*join_response_handler)(uint16_t status,
                                                                  const uint8_t *buffer,
                                                                  const uint16_t length));
extern int32_t rsi_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
extern int32_t rsi_send_raw_data(uint8_t *buffer, uint32_t length);
extern uint8_t *rsi_fill_config_profile(uint32_t type, uint8_t *profile_buffer);
extern int32_t rsi_wlan_add_profile(uint32_t type, uint8_t *profile);
extern int32_t rsi_wlan_get_profile(uint32_t type, rsi_config_profile_t *profile_rsp, uint16_t length);
extern int32_t rsi_wlan_delete_profile(uint32_t type);
extern int32_t rsi_wlan_enable_auto_config(uint8_t enable, uint32_t type);
extern int32_t rsi_wlan_bgscan_profile(uint8_t cmd, rsi_rsp_scan_t *result, uint32_t length);

extern int32_t rsi_wlan_twt_config(uint8_t twt_enable, uint8_t twt_flow_id, twt_user_params_t *twt_req_params);

extern int32_t rsi_wlan_update_gain_table(uint8_t band, uint8_t bandwidth, uint8_t *payload, uint16_t payload_len);
extern int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
extern int32_t rsi_wlan_power_save_with_listen_interval(uint8_t psp_mode, uint8_t psp_type, uint16_t listen_interval);
extern int32_t rsi_wlan_power_save_disable_and_enable(uint8_t psp_mode, uint8_t psp_type);
extern int32_t rsi_wlan_scan_with_bitmap_options(int8_t *ssid,
                                                 uint8_t chno,
                                                 rsi_rsp_scan_t *result,
                                                 uint32_t length,
                                                 uint32_t scan_bitmap);
extern void rsi_sort_scan_results_array_based_on_rssi(struct wpa_scan_results_arr *scan_results_array);
extern int32_t rsi_wlan_scan_async_with_bitmap_options(int8_t *ssid,
                                                       uint8_t chno,
                                                       uint32_t bitmap,
                                                       void (*scan_response_handler)(uint16_t status,
                                                                                     const uint8_t *buffer,
                                                                                     const uint16_t length));
int32_t rsi_wlan_ping_async(uint8_t flags,
                            uint8_t *ip_address,
                            uint16_t size,
                            void (*wlan_ping_response_handler)(uint16_t status,
                                                               const uint8_t *buffer,
                                                               const uint16_t length));
int32_t rsi_send_freq_offset(int32_t freq_offset_in_khz);
int32_t rsi_calib_write(uint8_t target, uint32_t flags, int8_t gain_offset, int32_t freq_offset);
int16_t rsi_parse(void *address, uint16_t length, uint8_t *value);
int32_t rsi_wlan_11ax_config(void);

void rsi_register_auto_config_rsp_handler(void (*rsi_auto_config_rsp_handler)(uint16_t status, uint8_t state));
STATIC INLINE void set_option(uint32_t *parameter, uint32_t flag)
{
  *parameter |= flag;
}

STATIC INLINE void clear_option(uint32_t *parameter, uint32_t flag)
{
  *parameter &= ~flag;
}

#endif
