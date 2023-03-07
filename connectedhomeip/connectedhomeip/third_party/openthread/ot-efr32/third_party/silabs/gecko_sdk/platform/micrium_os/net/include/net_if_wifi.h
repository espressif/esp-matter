/***************************************************************************//**
 * @file
 * @brief Network Interface Layer - Wireless
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @defgroup NET_CORE_IF_WIFI Wireless Network Interface API
 * @ingroup  NET_CORE
 * @brief    Wireless Network Interface API
 *
 * @addtogroup NET_CORE_IF_WIFI
 * @{
 ********************************************************************************************************
 * @note     (1) Supports following network interface layers:
 *             - (a) Ethernet
 *             - (b) IEEE 802
 *
 * @note     (2) Ethernet implementation conforms to RFC #1122, Section 2.3.3, bullets (a) & (b), but
 *               does NOT implement bullet (c) :
 *               RFC #1122                  LINK LAYER                  October 1989
 *               2.3.3  ETHERNET (RFC-894) and IEEE 802 (RFC-1042) ENCAPSULATION
 *               Every Internet host connected to a 10Mbps Ethernet cable :
 *                 - (a) MUST be able to send and receive packets using RFC-894 encapsulation;
 *                 - (b) SHOULD be able to receive RFC-1042 packets, intermixed with RFC-894 packets; and
 *                 - (c) MAY be able to send packets using RFC-1042 encapsulation.
 *
 * @note     (3) Wireless implementation doesn't supports wireless supplicant and/or IEEE 802.11. The
 *               wireless module-hardware must provide the wireless supplicant and all relevant IEEE 802.11
 *               supports.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_IF_WIFI_H_
#define  _NET_IF_WIFI_H_

#include  "net_cfg_net.h"

#ifdef  NET_IF_WIFI_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_type.h>
#include  <net/include/net_def.h>
#include  <net/include/net_if.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/platform_mgr.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   DEVICE SPI CFG VALUE DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_DEV_BAND;

#define  NET_DEV_BAND_NONE                                 0u
#define  NET_DEV_BAND_2_4_GHZ                              1u
#define  NET_DEV_BAND_5_0_GHZ                              2u
#define  NET_DEV_BAND_DUAL                                 3u

/********************************************************************************************************
 *                                       WIRELESS BSS MAC ADDR
 *******************************************************************************************************/

typedef  struct  net_if_wifi_bssid {
  CPU_INT08U BSSID[NET_IF_HW_ADDR_LEN_MAX];                             // WiFi BSSID
} NET_IF_WIFI_BSSID;

/********************************************************************************************************
 *                                       WIRELESS SSID DATA TYPE
 *******************************************************************************************************/

#define  NET_IF_WIFI_STR_LEN_MAX_SSID                     32u

typedef  struct  net_if_wifi_ssid {
  CPU_CHAR SSID[NET_IF_WIFI_STR_LEN_MAX_SSID];                          // WiFi SSID.
} NET_IF_WIFI_SSID;

/********************************************************************************************************
 *                                    WIRELESS CHANNEL DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IF_WIFI_CH;

//                                                                 Defines WiFi ch.
#define  NET_IF_WIFI_CH_ALL                                0u
#define  NET_IF_WIFI_CH_1                                  1u
#define  NET_IF_WIFI_CH_2                                  2u
#define  NET_IF_WIFI_CH_3                                  3u
#define  NET_IF_WIFI_CH_4                                  4u
#define  NET_IF_WIFI_CH_5                                  5u
#define  NET_IF_WIFI_CH_6                                  6u
#define  NET_IF_WIFI_CH_7                                  7u
#define  NET_IF_WIFI_CH_8                                  8u
#define  NET_IF_WIFI_CH_9                                  9u
#define  NET_IF_WIFI_CH_10                                10u
#define  NET_IF_WIFI_CH_11                                11u
#define  NET_IF_WIFI_CH_12                                12u
#define  NET_IF_WIFI_CH_13                                13u
#define  NET_IF_WIFI_CH_14                                14u

#define  NET_IF_WIFI_CH_MIN             NET_IF_WIFI_CH_ALL
#define  NET_IF_WIFI_CH_MAX             NET_IF_WIFI_CH_14

/********************************************************************************************************
 *                                   WIRELESS NETWORK TYPE DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IF_WIFI_NET_TYPE;

#define  NET_IF_WIFI_NET_TYPE_UNKNOWN                      0u
#define  NET_IF_WIFI_NET_TYPE_INFRASTRUCTURE               1u
#define  NET_IF_WIFI_NET_TYPE_ADHOC                        2u

/********************************************************************************************************
 *                                  WIRELESS SECURITY TYPE DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IF_WIFI_SECURITY_TYPE;                          // Define WiFi security type.

#define  NET_IF_WIFI_SECURITY_OPEN                         1u
#define  NET_IF_WIFI_SECURITY_WEP                          2u
#define  NET_IF_WIFI_SECURITY_WPA                          3u
#define  NET_IF_WIFI_SECURITY_WPA2                         4u

/********************************************************************************************************
 *                                 WIRELESS POWER & STRENGHT DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IF_WIFI_SIGNAL_STRENGTH;                        // WiFi IF's signal strength.

/********************************************************************************************************
 *                                 WIRELESS APs SCAN RESULT DATA TYPE
 * @brief Interface Wifi AP
 *******************************************************************************************************/

typedef  struct  net_if_wifi_ap {
  NET_IF_WIFI_BSSID           BSSID;                                    ///< WiFi AP SSID.
  NET_IF_WIFI_SSID            SSID;                                     ///< WiFi AP SSID.
  NET_IF_WIFI_CH              Ch;                                       ///< WiFi AP Ch.
  NET_IF_WIFI_NET_TYPE        NetType;                                  ///< Wifi AP net type.
  NET_IF_WIFI_SECURITY_TYPE   SecurityType;                             ///< WiFi AP security type.
  NET_IF_WIFI_SIGNAL_STRENGTH SignalStrength;                           ///< WiFi AP Signal Strength.
} NET_IF_WIFI_AP;

/********************************************************************************************************
 *                                    WIRELESS DATA RATE DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IF_WIFI_DATA_RATE;                              // Define WiFi data rate.

#define  NET_IF_WIFI_DATA_RATE_AUTO                        0u
#define  NET_IF_WIFI_DATA_RATE_1_MBPS                      1u
#define  NET_IF_WIFI_DATA_RATE_2_MBPS                      2u
#define  NET_IF_WIFI_DATA_RATE_5_5_MBPS                    5u
#define  NET_IF_WIFI_DATA_RATE_6_MBPS                      6u
#define  NET_IF_WIFI_DATA_RATE_9_MBPS                      9u
#define  NET_IF_WIFI_DATA_RATE_11_MBPS                    11u
#define  NET_IF_WIFI_DATA_RATE_12_MBPS                    12u
#define  NET_IF_WIFI_DATA_RATE_18_MBPS                    18u
#define  NET_IF_WIFI_DATA_RATE_24_MBPS                    24u
#define  NET_IF_WIFI_DATA_RATE_36_MBPS                    36u
#define  NET_IF_WIFI_DATA_RATE_48_MBPS                    48u
#define  NET_IF_WIFI_DATA_RATE_54_MBPS                    54u
#define  NET_IF_WIFI_DATA_RATE_MCS0                      100u
#define  NET_IF_WIFI_DATA_RATE_MCS1                      101u
#define  NET_IF_WIFI_DATA_RATE_MCS2                      102u
#define  NET_IF_WIFI_DATA_RATE_MCS3                      103u
#define  NET_IF_WIFI_DATA_RATE_MCS4                      104u
#define  NET_IF_WIFI_DATA_RATE_MCS5                      105u
#define  NET_IF_WIFI_DATA_RATE_MCS6                      106u
#define  NET_IF_WIFI_DATA_RATE_MCS7                      107u
#define  NET_IF_WIFI_DATA_RATE_MCS8                      108u
#define  NET_IF_WIFI_DATA_RATE_MCS9                      109u
#define  NET_IF_WIFI_DATA_RATE_MCS10                     110u
#define  NET_IF_WIFI_DATA_RATE_MCS11                     111u
#define  NET_IF_WIFI_DATA_RATE_MCS12                     112u
#define  NET_IF_WIFI_DATA_RATE_MCS13                     113u
#define  NET_IF_WIFI_DATA_RATE_MCS14                     114u
#define  NET_IF_WIFI_DATA_RATE_MCS15                     115u

/********************************************************************************************************
 *                                 WIRELESS POWER & STRENGHT DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IF_WIFI_PWR_LEVEL;                              // Define WiFi pwr level.

#define  NET_IF_WIFI_PWR_LEVEL_LO                          0u
#define  NET_IF_WIFI_PWR_LEVEL_MED                         1u
#define  NET_IF_WIFI_PWR_LEVEL_HI                          2u

/********************************************************************************************************
 *                                       WIRELESS PSK DATA TYPE
 *******************************************************************************************************/

#define  NET_IF_WIFI_STR_LEN_MAX_PSK                      32u

typedef  struct  net_if_wifi_psk {
  CPU_CHAR PSK[NET_IF_WIFI_STR_LEN_MAX_PSK];                            // WiFi PSK.
} NET_IF_WIFI_PSK;

/********************************************************************************************************
 *                                     WIRELESS PEER INFO DATA TYPE
 *******************************************************************************************************/

typedef  struct  net_if_wifi_peer {
  CPU_CHAR HW_Addr[NET_IF_HW_ADDR_LEN_MAX];                             // WiFi peer MAC addr.
} NET_IF_WIFI_PEER;

/********************************************************************************************************
 *                                       WIRELESS CMD DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IF_WIFI_CMD;                                    // Define WiFi cmd.

#define  NET_IF_WIFI_CMD_SCAN                              1u
#define  NET_IF_WIFI_CMD_JOIN                              2u
#define  NET_IF_WIFI_CMD_LEAVE                             3u
#define  NET_IF_WIFI_CMD_LINK_STATE_GET                    4u
#define  NET_IF_WIFI_CMD_LINK_STATE_GET_INFO               5u
#define  NET_IF_WIFI_CMD_LINK_STATE_UPDATE                 6u
#define  NET_IF_WIFI_CMD_CREATE                            7u
#define  NET_IF_WIFI_CMD_GET_PEER_INFO                     8u

/********************************************************************************************************
 *                                     WIRELESS AP CONFIG DATA TYPE
 * @brief Wireless AP configuration
 *******************************************************************************************************/

typedef  struct  net_if_wifi_ap_cfg {
  NET_IF_WIFI_NET_TYPE      NetType;                                    ///< WiFi AP Config net type.
  NET_IF_WIFI_DATA_RATE     DataRate;                                   ///< WiFi AP Config data rate.
  NET_IF_WIFI_SECURITY_TYPE SecurityType;                               ///< WiFi AP Config security type.
  NET_IF_WIFI_PWR_LEVEL     PwrLevel;                                   ///< WiFi AP Config pwr level.
  NET_IF_WIFI_SSID          SSID;                                       ///< WiFi AP Config ssid.
  NET_IF_WIFI_PSK           PSK;                                        ///< WiFi AP Config psk.
  NET_IF_WIFI_CH            Ch;                                         ///< WiFi AP Config ch.
} NET_IF_WIFI_AP_CFG;

/********************************************************************************************************
 *                                     WIRELESS DEVICE DATA TYPES
 *
 * Note(s) : (1) The Wireless interface configuration data type is a specific definition of a network
 *               device configuration data type.  Each specific network device configuration data type
 *               MUST define ALL generic network device configuration parameters, synchronized in both
 *               the sequential order & data type of each parameter.
 *
 *               Thus, ANY modification to the sequential order or data types of configuration parameters
 *               MUST be appropriately synchronized between the generic network device configuration data
 *               type & the Ethernet interface configuration data type.
 *
 *               See also 'net_if.h  GENERIC NETWORK DEVICE CONFIGURATION DATA TYPE  Note #1'.
 *******************************************************************************************************/

typedef  struct  net_if_wifi_cfg {
  const CPU_CHAR  *HW_AddrStr;
  NET_IF_IPv4_CFG IPv4;
  NET_IF_IPv6_CFG IPv6;
  NET_DEV_BAND    Band;
} NET_IF_WIFI_CFG;

/********************************************************************************************************
 *                                   WIRELESS DEVICE API DATA TYPES
 *
 * Note(s) : (1) (a) The Wireless device application programming interface (API) data type is a specific
 *                   network device API data type definition which MUST define ALL generic network device
 *                   API functions, synchronized in both the sequential order of the functions & argument
 *                   lists for each function.
 *
 *                   Thus, ANY modification to the sequential order or argument lists of the API functions
 *                   MUST be appropriately synchronized between the generic network device API data type &
 *                   the Wireless device API data type definition/instantiations.
 *
 *                   However, specific Wireless device API data type definitions/instantiations MAY include
 *                   additional API functions after all generic Ethernet device API functions.
 *
 *               (b) ALL API functions MUST be defined with NO NULL functions for all specific Ethernet
 *                   device API instantiations.  Any specific Ethernet device API instantiation that does
 *                   NOT require a specific API's functionality MUST define an empty API function.
 *
 *               See also 'net_if.h  GENERIC NETWORK DEVICE API DATA TYPE  Note #1'.
 *******************************************************************************************************/

typedef  struct  net_wifi_mgr_ctx NET_WIFI_MGR_CTX;

//                                                                 ---------- NET WIFI DEV API ------------
//                                                                 Net wifi dev API fnct ptrs :
typedef  struct  net_dev_api_wifi {
  void (*Init)(NET_IF   *p_if,                                              // Init/add.
               RTOS_ERR *p_err);

  void (*Start)(NET_IF   *p_if,                                             // Start.
                RTOS_ERR *p_err);

  void (*Stop)(NET_IF   *p_if,                                              // Stop.
               RTOS_ERR *p_err);

  void (*Rx)(NET_IF     *p_if,                                              // Rx.
             CPU_INT08U **p_data,
             CPU_INT16U *size,
             RTOS_ERR   *p_err);

  void (*Tx)(NET_IF     *p_if,                                              // Tx.
             CPU_INT08U *p_data,
             CPU_INT16U size,
             RTOS_ERR   *p_err);

  void (*AddrMulticastAdd)(NET_IF     *p_if,                                // Multicast addr add.
                           CPU_INT08U *p_addr_hw,
                           CPU_INT08U addr_hw_len,
                           RTOS_ERR   *p_err);

  void (*AddrMulticastRemove)(NET_IF     *p_if,                             // Multicast addr remove.
                              CPU_INT08U *p_addr_hw,
                              CPU_INT08U addr_hw_len,
                              RTOS_ERR   *p_err);

  void (*ISR_Handler)(NET_IF           *p_if,                               // ISR handler.
                      NET_DEV_ISR_TYPE type);

  void (*MgmtDemux)(NET_IF   *p_if,                                         // Demux mgmt frame.
                    NET_BUF  *p_buf,
                    RTOS_ERR *p_err);

  CPU_INT32U (*MgmtExecuteCmd)(NET_IF           *p_if,                      // Execute mgmt cmd.
                               NET_IF_WIFI_CMD  cmd,
                               NET_WIFI_MGR_CTX *p_ctx,
                               void             *p_cmd_data,
                               CPU_INT16U       cmd_data_len,
                               CPU_INT08U       *p_buf_rtn,
                               CPU_INT08U       buf_rtn_len_max,
                               RTOS_ERR         *p_err);

  CPU_INT32U (*MgmtProcessResp)(NET_IF           *p_if,                     // Process mgmt frame.
                                NET_IF_WIFI_CMD  cmd,
                                NET_WIFI_MGR_CTX *p_ctx,
                                CPU_INT08U       *p_buf_rxd,
                                CPU_INT16U       buf_rxd_len,
                                CPU_INT08U       *p_buf_rtn,
                                CPU_INT16U       buf_rtn_len_max,
                                RTOS_ERR         *p_err);
} NET_DEV_API_WIFI;

/********************************************************************************************************
 *                              WIRELESS DEVICE BSP INTERFACE DATA TYPE
 *
 * Note(s) : (1) (a) The Wireless device board-support package (BSP) interface data type is a specific
 *                   network device BSP interface data type definition which SHOULD define ALL generic
 *                   network device BSP functions, synchronized in both the sequential order of the
 *                   functions & argument lists for each function.
 *
 *                   Thus, ANY modification to the sequential order or argument lists of the BSP functions
 *                   SHOULD be appropriately synchronized between the generic network device BSP interface
 *                   data type & the Wireless device BSP interface data type definition/instantiations.
 *
 *                   However, specific Wireless device BSP interface data type definitions/instantiations
 *                   MAY include additional BSP functions after all generic Wireless device BSP functions.
 *
 *               (b) A specific Wireless device BSP interface instantiation MAY define NULL functions for
 *                   any (or all) BSP functions provided that the specific Ethernet device driver does NOT
 *                   require those specific BSP function(s).
 *******************************************************************************************************/

//                                                                 ----------------- NET WIFI DEV BSP -----------------
//                                                                 Net WiFi dev BSP fnct ptrs :
typedef  struct  net_dev_bsp_wifi {
  //                                                               Start
  void (*Start)(NET_IF   *p_if,
                RTOS_ERR *p_err);
  //                                                               Stop
  void (*Stop)(NET_IF   *p_if,
               RTOS_ERR *p_err);
  //                                                               Cfg GPIO
  void (*CfgGPIO)(NET_IF   *p_if,
                  RTOS_ERR *p_err);
  //                                                               Cfg ISR
  void (*CfgIntCtrl)(NET_IF   *p_if,
                     RTOS_ERR *p_err);
  //                                                               Enable/Disable ISR
  void (*IntCtrl)(NET_IF      *p_if,
                  CPU_BOOLEAN en,
                  RTOS_ERR    *p_err);
} NET_DEV_BSP_WIFI;

/********************************************************************************************************
 *                                        NET WIFI MANAGER API
 *******************************************************************************************************/

typedef  struct  net_wifi_mgr_api {
  //                                                               Init
  void (*Init)(NET_IF   *p_if,
               RTOS_ERR *p_err);
  //                                                               Start
  void (*Start)(NET_IF   *p_if,
                RTOS_ERR *p_err);
  //                                                               Stop
  void (*Stop)(NET_IF   *p_if,
               RTOS_ERR *p_err);
  //                                                               Scan
  CPU_INT16U (*AP_Scan)(NET_IF                 *p_if,
                        NET_IF_WIFI_AP         *p_buf_scan,
                        CPU_INT16U             buf_scan_len_max,
                        const NET_IF_WIFI_SSID *p_ssid,
                        NET_IF_WIFI_CH         ch,
                        RTOS_ERR               *p_err);
  //                                                               Join
  void (*AP_Join)(NET_IF                   *p_if,
                  const NET_IF_WIFI_AP_CFG *p_ap_cfg,
                  RTOS_ERR                 *p_err);
  //                                                               Leave
  void (*AP_Leave)(NET_IF   *p_if,
                   RTOS_ERR *p_err);
  //                                                               I/O Control
  void (*IO_Ctrl)(NET_IF     *p_if,
                  CPU_INT08U opt,
                  void       *p_data,
                  RTOS_ERR   *p_err);
  //                                                               Mgmt handler
  CPU_INT32U (*Mgmt)(NET_IF          *p_if,
                     NET_IF_WIFI_CMD cmd,
                     CPU_INT08U      *p_buf_cmd,
                     CPU_INT16U      buf_cmd_len,
                     CPU_INT08U      *p_buf_rtn,
                     CPU_INT16U      buf_rtn_len_max,
                     RTOS_ERR        *p_err);
  //                                                               Mgr signal
  void (*Signal)(NET_IF   *p_if,
                 NET_BUF  *p_buf,
                 RTOS_ERR *p_err);
  //                                                               Create
  void (*AP_Create)(NET_IF                   *p_if,
                    const NET_IF_WIFI_AP_CFG *p_cfg,
                    RTOS_ERR                 *p_err);
  //                                                               Get Peer Info
  CPU_INT16U (*AP_GetPeerInfo)(NET_IF                 *p_if,
                               const NET_IF_WIFI_PEER *p_buf_peer,
                               CPU_INT16U             peer_info_len_max,
                               RTOS_ERR               *p_err);
} NET_WIFI_MGR_API;

/********************************************************************************************************
 *                                       HARDWARE INFO DATA TYPE
 *******************************************************************************************************/

typedef  struct net_if_wifi_part_info {
  const NET_DEV_API_WIFI *DrvAPI_Ptr;
  CPU_INT16U             RxBufIxOffset;
  CPU_INT16U             TxBufIxOffset;
} NET_IF_WIFI_PART_INFO;

typedef  struct  net_if_wifi_hw_info {
  PLATFORM_MGR_ITEM           Item;                             // Platform manager standard item structure.
  CPU_CHAR                    *IO_BusNamePtr;
  CPU_INT16U                  IO_SlaveID;
  const NET_IF_WIFI_PART_INFO *PartInfoPtr;
  const NET_DEV_BSP_WIFI      *BSP_API_Ptr;
} NET_IF_WIFI_HW_INFO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern const NET_IF_API NetIF_API_WiFi;                         // Wireless IF API fnct ptr(s).

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_CTRLR_WIFI_SPI_REG(part_name, spi_name, part_info_ptr, bsp_api_ptr, slave_id) \
  do {                                                                                     \
    if ((part_info_ptr)->DrvAPI_Ptr != DEF_NULL) {                                         \
      static const struct  net_if_wifi_hw_info _item = {                                   \
        .Item.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_NET_WIFI,                              \
        .Item.StrID = (part_name),                                                         \
        .IO_BusNamePtr = (spi_name),                                                       \
        .IO_SlaveID = (slave_id),                                                          \
        .PartInfoPtr = (const  NET_IF_WIFI_PART_INFO *)(part_info_ptr),                    \
        .BSP_API_Ptr = (const  NET_DEV_BSP_WIFI *)(bsp_api_ptr)                            \
      };                                                                                   \
      PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_item.Item);                            \
    }                                                                                      \
  } while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

NET_IF_NBR NetIF_WiFi_Add(CPU_CHAR             *p_name,
                          const NET_IF_BUF_CFG *p_buf_cfg,
                          void                 *p_ext_cfg,
                          MEM_SEG              *p_mem_seg,
                          RTOS_ERR             *p_err);

void NetIF_WiFi_Start(NET_IF_NBR      if_nbr,
                      NET_IF_WIFI_CFG *p_cfg,
                      RTOS_ERR        *p_err);

CPU_INT16U NetIF_WiFi_Scan(NET_IF_NBR             if_nbr,
                           NET_IF_WIFI_AP         *p_buf_scan,
                           CPU_INT16U             buf_scan_len_max,
                           const NET_IF_WIFI_SSID *p_ssid,
                           NET_IF_WIFI_CH         ch,
                           RTOS_ERR               *p_err);

void NetIF_WiFi_Join(NET_IF_NBR                if_nbr,
                     NET_IF_WIFI_NET_TYPE      net_type,
                     NET_IF_WIFI_DATA_RATE     data_rate,
                     NET_IF_WIFI_SECURITY_TYPE security_type,
                     NET_IF_WIFI_PWR_LEVEL     pwr_level,
                     NET_IF_WIFI_SSID          ssid,
                     NET_IF_WIFI_PSK           psk,
                     RTOS_ERR                  *p_err);

void NetIF_WiFi_CreateAP(NET_IF_NBR                if_nbr,
                         NET_IF_WIFI_NET_TYPE      net_type,
                         NET_IF_WIFI_DATA_RATE     data_rate,
                         NET_IF_WIFI_SECURITY_TYPE security_type,
                         NET_IF_WIFI_PWR_LEVEL     pwr_level,
                         NET_IF_WIFI_CH            ch,
                         NET_IF_WIFI_SSID          ssid,
                         NET_IF_WIFI_PSK           psk,
                         RTOS_ERR                  *p_err);

void NetIF_WiFi_Leave(NET_IF_NBR if_nbr,
                      RTOS_ERR   *p_err);

CPU_INT16U NetIF_WiFi_GetPeerInfo(NET_IF_NBR       if_nbr,
                                  NET_IF_WIFI_PEER *p_buf_peer,
                                  CPU_INT16U       buf_peer_info_len_max,
                                  RTOS_ERR         *p_err);

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IF_WIFI_MODULE_EN
#endif // _NET_IF_WIFI_H_
