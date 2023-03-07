/***************************************************************************//**
 * @file
 * @brief Network Interface Layer - Ethernet
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
 * @defgroup NET_CORE_IF_ETHER Ethernet Network Interface API
 * @ingroup  NET_CORE
 * @brief    Ethernet Network Interface API
 *
 * @addtogroup NET_CORE_IF_ETHER
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
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_IF_ETHER_H_
#define  _NET_IF_ETHER_H_

#include  "net_cfg_net.h"

#ifdef  NET_IF_ETHER_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net_cfg.h>

#include  <net/include/net_def.h>
#include  <net/include/net_type.h>
#include  <net/include/net_if.h>
#include  <net/include/dhcp_client_types.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/platform_mgr.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   ETHERNET PHYSICAL LAYER DEFINES
 *******************************************************************************************************/

#define  NET_PHY_SPD_0                                     0u   // Link speed unknown, or link down.
#define  NET_PHY_SPD_10                                   10u   // Link speed = 10mbps.
#define  NET_PHY_SPD_100                                 100u   // Link speed = 100mbps.
#define  NET_PHY_SPD_1000                               1000u   // Link speed = 1000mbps.
#define  NET_PHY_SPD_2500                               2500u
#define  NET_PHY_SPD_5000                               5000u
#define  NET_PHY_SPD_10000                             10000u
#define  NET_PHY_SPD_AUTO                             0xFFFFu   // Initial link spd determined by auto-negotiation.

#define  NET_PHY_DUPLEX_UNKNOWN                            0u   // Duplex uknown or auto-neg incomplete.
#define  NET_PHY_DUPLEX_HALF                               1u   // Duplex = Half Duplex.
#define  NET_PHY_DUPLEX_FULL                               2u   // Duplex = Full Duplex.
#define  NET_PHY_DUPLEX_AUTO                               3u   // Initial duplex   determined by auto-negotiation.

#define  NET_PHY_BUS_MODE_MII                              0u   // Phy attached to MAC via  MII bus.
#define  NET_PHY_BUS_MODE_RMII                             1u   // Phy attached to MAC via RMII bus.
#define  NET_PHY_BUS_MODE_SMII                             2u   // Phy attached to MAC via SMII bus.
#define  NET_PHY_BUS_MODE_GMII                             3u   // Phy attached to MAC via GMII bus.
#define  NET_PHY_BUS_MODE_RGMII                            4u
#define  NET_PHY_BUS_MODE_SGMII                            5u
#define  NET_PHY_BUS_MODE_PCS_PMA                          6u

#define  NET_PHY_TYPE_INT                                  0u   // Internal Phy attached to MAC.
#define  NET_PHY_TYPE_EXT                                  1u   // External Phy attached to MAC.

#define  NET_PHY_ADDR_AUTO                              0xFFu   // Detect   Phy addr automatically.

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                              NETWORK INTERFACE CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef  struct  net_if_ether_cfg {
  const CPU_CHAR  *HW_AddrStr;
  NET_IF_IPv4_CFG IPv4;
  NET_IF_IPv6_CFG IPv6;
} NET_IF_ETHER_CFG;

/********************************************************************************************************
 *                                     ETHERNET DEVICE DATA TYPES
 *
 * Note(s) : (1) The Ethernet interface configuration data type is a specific definition of a network
 *               device configuration data type.  Each specific network device configuration data type
 *               MUST define ALL generic network device configuration parameters, synchronized in both
 *               the sequential order & data type of each parameter.
 *
 *               Thus ANY modification to the sequential order or data types of generic configuration
 *               parameters MUST be appropriately synchronized between the generic network device
 *               configuration data type & the Ethernet interface configuration data type.
 *
 *               See also 'net_if.h  GENERIC NETWORK DEVICE CONFIGURATION DATA TYPE  Note #1'.
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       NET ETHERNET DEVICE CFG
 * @brief Net Ethernet device configuration
 *******************************************************************************************************/

typedef  struct  net_dev_cfg_ether {
  //                                                               ----------------- GENERIC  NET DEV CFG MEMBERS -----------------
  /** Rx buf mem pool type :
      NET_IF_MEM_TYPE_MAIN        bufs alloc'd from main      mem
      NET_IF_MEM_TYPE_DEDICATED   bufs alloc'd from dedicated mem     */
  NET_IF_MEM_TYPE RxBufPoolType;
  NET_BUF_SIZE    RxBufLargeSize;                   ///< Size  of dev rx large buf data areas (in octets).
  NET_BUF_QTY     RxBufLargeNbr;                    ///< Nbr   of dev rx large buf data areas.
  NET_BUF_SIZE    RxBufAlignOctets;                 ///< Align of dev rx       buf data areas (in octets).
  NET_BUF_SIZE    RxBufIxOffset;                    ///< Offset from base ix to rx data into data area (in octets).

  /** Tx buf mem pool type :
      NET_IF_MEM_TYPE_MAIN        bufs alloc'd from main      mem
      NET_IF_MEM_TYPE_DEDICATED   bufs alloc'd from dedicated mem    */
  NET_IF_MEM_TYPE   TxBufPoolType;
  NET_BUF_SIZE      TxBufLargeSize;                 ///< Size  of dev tx large buf data areas (in octets).
  NET_BUF_QTY       TxBufLargeNbr;                  ///< Nbr   of dev tx large buf data areas.
  NET_BUF_SIZE      TxBufSmallSize;                 ///< Size  of dev tx small buf data areas (in octets).
  NET_BUF_QTY       TxBufSmallNbr;                  ///< Nbr   of dev tx small buf data areas.
  NET_BUF_SIZE      TxBufAlignOctets;               ///< Align of dev tx       buf data areas (in octets).
  NET_BUF_SIZE      TxBufIxOffset;                  ///< Offset from base ix to tx data from data area (in octets).

  CPU_ADDR          MemAddr;                        ///< Base addr of (Ether dev's) dedicated mem, if avail.
  CPU_ADDR          MemSize;                        ///< Size      of (Ether dev's) dedicated mem, if avail.

  NET_DEV_CFG_FLAGS Flags;                          ///< Opt'l bit flags.

  //                                                               ----------------- SPECIFIC NET DEV CFG MEMBERS -----------------
  NET_BUF_QTY       RxDescNbr;                      ///< Nbr rx dev desc's.
  NET_BUF_QTY       TxDescNbr;                      ///< Nbr tx dev desc's.

  CPU_ADDR          BaseAddr;                       ///< Base addr of Ether dev hw/regs.

  CPU_DATA          DataBusSizeNbrBits;             ///< Size      of Ether dev's data bus (in bits), if avail.

  CPU_CHAR          HW_AddrStr[NET_IF_802x_ADDR_SIZE_STR];     ///<  Ether IF's dev hw addr str.

  void              *CfgExtPtr;                     ///< Configuration Extension Ptr.
} NET_DEV_CFG_ETHER;

/********************************************************************************************************
 *                                          ETHERNET PHY CFG
 * @brief Ethernet PHY configuration
 *******************************************************************************************************/

typedef  struct  net_phy_cfg_ether {
  CPU_INT08U BusAddr;                                           ///< Phy bus  addr.
  CPU_INT08U BusMode;                                           ///< Phy bus  mode.
  CPU_INT08U Type;                                              ///< Phy      type.
  CPU_INT16U Spd;                                               ///< Phy link spd.
  CPU_INT08U Duplex;                                            ///< Phy link duplex.
} NET_PHY_CFG_ETHER;

/********************************************************************************************************
 *                                        NET ETHERNET IF DATA
 * @brief Ethernet interface
 *******************************************************************************************************/

typedef  struct  net_if_data_ether {
  CPU_INT08U HW_Addr[NET_IF_802x_HW_ADDR_LEN];                  ///<              Ether IF's dev hw addr.
  CPU_INT08U Phy_Addr;                                          ///< Base addr of Ether IF's Phy hw addr.
} NET_IF_DATA_ETHER;

/********************************************************************************************************
 *                                   NET ETHERNET DEVICE LINK STATE
 * @brief Ethernet device link state
 *******************************************************************************************************/

typedef  struct net_dev_link_state_ether {
  CPU_INT16U Spd;                                               ///< Link spd.
  CPU_INT08U Duplex;                                            ///< Link duplex.
} NET_DEV_LINK_ETHER;

/********************************************************************************************************
 *                              ETHERNET DEVICE BSP INTERFACE DATA TYPE
 *
 * @brief Ethernet device BSP Interface
 *
 * @note    (1) The generic Ethernet device board-support package (BSP) interface data type is a template/
 *               subset for all specific Ethernet device BSP interface data types.
 *
 *               (a) Each specific Ethernet device BSP interface data type definition SHOULD define ALL
 *                   generic Ethernet device BSP functions, synchronized in both the sequential order of
 *                   the functions & argument lists for each function.
 *
 *                   Thus ANY modification to the sequential order or argument lists of the generic BSP
 *                   functions SHOULD be appropriately synchronized between the generic Ethernet device
 *                   BSP interface data type & ALL specific Ethernet device BSP interface data type
 *                   definitions/instantiations.
 *
 *                   However, specific Ethernet device BSP interface data type definitions/instantiations
 *                   MAY include additional BSP functions after all generic Ethernet device BSP functions.
 *
 *               (b) (1) A specific Ethernet device BSP interface instantiation MAY define NULL functions
 *                       for any (or all) generic BSP functions provided that the specific Ethernet device
 *                       driver does NOT require those specific generic BSP function(s).
 *
 *                   (2) However, a specific Ethernet device driver that includes additional BSP functions
 *                       into its specific BSP interface data type definition SHOULD NOT then define any
 *                       NULL functions for these additional functions.
 *******************************************************************************************************/

typedef  struct  net_dev_bsp_ether {
  void (*CfgClk)(NET_IF   *p_if,                                ///< Cfg dev clk(s).
                 RTOS_ERR *p_err);

  void (*CfgIntCtrl)(NET_IF   *p_if,                            ///< Cfg dev int ctrl(s).
                     RTOS_ERR *p_err);

  void (*CfgGPIO)(NET_IF   *p_if,                               ///< Cfg dev GPIO.
                  RTOS_ERR *p_err);

  CPU_INT32U (*ClkFreqGet)(NET_IF   *p_if,                      ///< Get dev clk freq.
                           RTOS_ERR *p_err);
} NET_DEV_BSP_ETHER;

/********************************************************************************************************
 *                                   ETHERNET DEVICE API DATA TYPES
 *
 * @brief Ethernet Device API
 *
 * @note    (1) (a) The Ethernet device application programming interface (API) data type is a specific
 *                   network device API data type definition which MUST define ALL generic network device
 *                   API functions, synchronized in both the sequential order of the functions & argument
 *                   lists for each function.
 *
 *                   Thus ANY modification to the sequential order or argument lists of the  generic API
 *                   functions MUST be appropriately synchronized between the generic network device API
 *                   data type & the Ethernet device API data type definition/instantiations.
 *
 *                   However, specific Ethernet device API data type definitions/instantiations MAY include
 *                   additional API functions after all generic Ethernet device API functions.
 *
 *               (b) ALL API functions MUST be defined with NO NULL functions for all specific Ethernet
 *                   device API instantiations.  Any specific Ethernet device API instantiation that does
 *                   NOT require a specific API's functionality MUST define an empty API function which
 *                   may need to return an appropriate error code.
 *
 *               See also 'net_if.h  GENERIC NETWORK DEVICE API DATA TYPE  Note #1'.
 *******************************************************************************************************/

typedef  struct  net_dev_api_ether {
  //                                                               ------- GENERIC  NET DEV API MEMBERS -------
  //                                                                 Init/add
  void (*Init)(NET_IF   *p_if,
               RTOS_ERR *p_err);
  //                                                                 Start
  void (*Start)(NET_IF   *p_if,
                RTOS_ERR *p_err);
  //                                                                 Stop
  void (*Stop)(NET_IF   *p_if,
               RTOS_ERR *p_err);

  //                                                                 Rx
  void (*Rx)(NET_IF     *p_if,
             CPU_INT08U **p_data,
             CPU_INT16U *size,
             RTOS_ERR   *p_err);
  //                                                                 Tx
  void (*Tx)(NET_IF     *p_if,
             CPU_INT08U *p_data,
             CPU_INT16U size,
             RTOS_ERR   *p_err);

  //                                                               ------- SPECIFIC NET DEV API MEMBERS -------
  //                                                                 Multicast addr add
  void (*AddrMulticastAdd)(NET_IF     *p_if,
                           CPU_INT08U *paddr_hw,
                           CPU_INT08U addr_hw_len,
                           RTOS_ERR   *p_err);
  //                                                                 Multicast addr remove
  void (*AddrMulticastRemove)(NET_IF     *p_if,
                              CPU_INT08U *paddr_hw,
                              CPU_INT08U addr_hw_len,
                              RTOS_ERR   *p_err);

  //                                                                 ISR handler
  void (*ISR_Handler)(NET_IF           *p_if,
                      NET_DEV_ISR_TYPE type);

  //                                                                 I/O ctrl
  void (*IO_Ctrl)(NET_IF     *p_if,
                  CPU_INT08U opt,
                  void       *p_data,
                  RTOS_ERR   *p_err);

  //                                                                 Phy reg rd
  void (*Phy_RegRd)(NET_IF     *p_if,
                    CPU_INT08U phy_addr,
                    CPU_INT08U reg_addr,
                    CPU_INT16U *p_data,
                    RTOS_ERR   *p_err);
  //                                                                 Phy reg wr
  void (*Phy_RegWr)(NET_IF     *p_if,
                    CPU_INT08U phy_addr,
                    CPU_INT08U reg_addr,
                    CPU_INT16U reg_data,
                    RTOS_ERR   *p_err);
} NET_DEV_API_ETHER;

/********************************************************************************************************
 *                                        NET ETHERNET PHY API
 * @brief Ethernet PHY API
 *******************************************************************************************************/

typedef  struct  net_phy_api_ether {
  //                                                                 Init
  void (*Init)(NET_IF   *p_if,
               RTOS_ERR *p_err);
  //                                                                 En/dis
  void (*EnDis)(NET_IF      *p_if,
                CPU_BOOLEAN en,
                RTOS_ERR    *p_err);

  //                                                                 Link state get
  void (*LinkStateGet)(NET_IF             *p_if,
                       NET_DEV_LINK_ETHER *p_link_state,
                       RTOS_ERR           *p_err);
  //                                                                 Link state set
  void (*LinkStateSet)(NET_IF             *p_if,
                       NET_DEV_LINK_ETHER *p_link_state,
                       RTOS_ERR           *p_err);

  //                                                                 ISR
  void (*ISR_Handler)(NET_IF *p_if);
} NET_PHY_API_ETHER;

/********************************************************************************************************
 *                                       HARDWARE INFO DATA TYPE
 *******************************************************************************************************/

typedef  struct net_if_ether_hw_info {
  const NET_DEV_API_ETHER *DrvAPI_Ptr;
  const NET_DEV_BSP_ETHER *BSP_API_Ptr;
  const NET_PHY_API_ETHER *PHY_API_Ptr;
  const NET_DEV_CFG_ETHER *DevCfgPtr;
  const NET_PHY_CFG_ETHER *PHY_CfgPtr;
} NET_IF_ETHER_HW_INFO;

struct  _rtos_net_if_ether_hw_info {
  PLATFORM_MGR_ITEM          Item;
  const NET_IF_ETHER_HW_INFO *InfoPtr;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern const NET_IF_API NetIF_API_Ether;                        // Ether IF API fnct ptr(s).

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_CTRLR_ETHER_REG(part_name, ctrlr_info_ptr)             \
  do {                                                              \
    if ((ctrlr_info_ptr)->DrvAPI_Ptr != DEF_NULL) {                 \
      static const struct  _rtos_net_if_ether_hw_info _item = {     \
        .Item.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_NET_ETHER,      \
        .Item.StrID = (part_name),                                  \
        .InfoPtr = (const  NET_IF_ETHER_HW_INFO *)(ctrlr_info_ptr), \
      };                                                            \
      PlatformMgrItemInitAdd((PLATFORM_MGR_ITEM *)&_item.Item);     \
    }                                                               \
  } while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

NET_IF_NBR NetIF_Ether_Add(CPU_CHAR       *p_name,
                           NET_IF_BUF_CFG *p_buf_cfg,
                           MEM_SEG        *p_mem_seg,
                           RTOS_ERR       *p_err);

void NetIF_Ether_Start(NET_IF_NBR       if_nbr,
                       NET_IF_ETHER_CFG *p_cfg,
                       RTOS_ERR         *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IF_ETHER_MODULE_EN
#endif // _NET_IF_ETHER_H_
