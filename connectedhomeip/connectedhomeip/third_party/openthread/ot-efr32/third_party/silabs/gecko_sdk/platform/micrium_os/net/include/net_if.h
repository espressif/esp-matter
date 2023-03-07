/***************************************************************************//**
 * @file
 * @brief Network Interface Management
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
 * @defgroup NET_CORE_IF Network Interface API
 * @ingroup  NET_CORE
 * @brief    Network Interface API
 *
 * @addtogroup NET_CORE_IF
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_IF_H_
#define  _NET_IF_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_cfg_net.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_type.h>
#include  <net/include/net_stat.h>
#include  <net/include/dhcp_client_types.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                  NETWORK INTERFACE / 802x DEFINES
 *******************************************************************************************************/

#define  NET_IF_802x_ADDR_SIZE_STR                    NET_ASCII_LEN_MAX_ADDR_MAC

/********************************************************************************************************
 *                                   NETWORK INTERFACE I/O CONTROL DEFINES
 *******************************************************************************************************/

#define  NET_IF_IO_CTRL_NONE                               0u
#define  NET_IF_IO_CTRL_LINK_STATE_GET                    10u   // Get        link state.
#define  NET_IF_IO_CTRL_LINK_STATE_GET_INFO               11u   // Get        link state info.
#define  NET_IF_IO_CTRL_LINK_STATE_UPDATE                 12u   // Update dev link state regs.
#define  NET_IF_IO_CTRL_EEE                               20u   // Control EEE feature.
#define  NET_IF_IO_CTRL_EEE_GET_INFO                      21u   // Get EEE info.

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                              NETWORK INTERFACE CONFIGURATION DATA TYPE
 *******************************************************************************************************/

//                                                                 --------- NET INTERFACE IPV4 CONFIGURATION ---------
typedef  struct net_if_dhcp_client_cfg {
  CPU_BOOLEAN            En;
  DHCPc_CFG              Cfg;
  DHCPc_ON_COMPLETE_HOOK OnCompleteHook;
} NET_IF_DHCP_CLIENT_CFG;

typedef  struct  net_if_ipv4_static_cfg {
  const CPU_CHAR *Addr;
  const CPU_CHAR *Mask;
  const CPU_CHAR *Gateway;
} NET_IF_IPv4_STATIC_CFG;

typedef  struct  net_if_ipv4_link_local_cfg {
  CPU_BOOLEAN                       En;
  NET_IPv4_LINK_LOCAL_COMPLETE_HOOK OnCompleteHook;
} NET_IF_IPv4_LINK_LOCAL_CFG;

typedef  struct  net_if_ipv4_cfg {
  NET_IF_IPv4_STATIC_CFG     Static;
  NET_IF_DHCP_CLIENT_CFG     DHCPc;
  NET_IF_IPv4_LINK_LOCAL_CFG LinkLocal;
} NET_IF_IPv4_CFG;

//                                                                 --------- NET INTERFACE IPV6 CONFIGURATION ---------
typedef  struct  net_if_ipv6_static_cfg {
  const CPU_CHAR *Addr;
  CPU_INT08U     PrefixLen;
  CPU_BOOLEAN    DAD_En;
} NET_IF_IPv6_STATIC_CFG;

typedef  struct  net_if_ipv6_slaac_cfg {
  CPU_BOOLEAN En;
  CPU_BOOLEAN DAD_En;
} NET_IF_IPv6_SLAAC_CFG;

typedef  struct  net_if_ipv6_cfg {
  NET_IF_IPv6_STATIC_CFG  Static;
  NET_IF_IPv6_SLAAC_CFG   AutoCfg;
  NET_IPv6_ADDR_HOOK_FNCT Hook;
} NET_IF_IPv6_CFG;

typedef  struct  net_if_api NET_IF_API;

typedef  struct  net_if NET_IF;

typedef  enum  net_if_type {
  NET_IF_TYPE_NONE,
  NET_IF_TYPE_LOOPBACK,
  NET_IF_TYPE_SERIAL,
  NET_IF_TYPE_PPP,
  NET_IF_TYPE_ETHER,
  NET_IF_TYPE_WIFI
} NET_IF_TYPE;

typedef  struct  net_ip_if_obj NET_IP_IF_OBJ;

/********************************************************************************************************
 *                     NETWORK DEVICE INTERRUPT SERVICE ROUTINE (ISR) TYPE DATA TYPE
 * @brief Network device ISR
 *
 * @note    (1) The following network device interrupt service routine (ISR) types are currently supported.
 *
 *               However, this may NOT be a complete or exhaustive list of device ISR type(s).  Therefore,
 *               ANY addition, modification, or removal of network device ISR types SHOULD be appropriately
 *               synchronized &/or updated with (ALL) device driver ISR handlers.
 *******************************************************************************************************/

typedef  enum  net_dev_isr_type {
  NET_DEV_ISR_TYPE_NONE,
  NET_DEV_ISR_TYPE_UNKNOWN,                                     ///< Dev                     ISR unknown.

  NET_DEV_ISR_TYPE_RX,                                          ///< Dev rx                  ISR.
  NET_DEV_ISR_TYPE_RX_RUNT,                                     ///< Dev rx runt             ISR.
  NET_DEV_ISR_TYPE_RX_OVERRUN,                                  ///< Dev rx overrun          ISR.

  NET_DEV_ISR_TYPE_TX_RDY,                                      ///< Dev tx rdy              ISR.
  NET_DEV_ISR_TYPE_TX_COMPLETE,                                 ///< Dev tx complete         ISR.
  NET_DEV_ISR_TYPE_TX_COLLISION_LATE,                           ///< Dev tx late   collision ISR.
  NET_DEV_ISR_TYPE_TX_COLLISION_EXCESS,                         ///< Dev tx excess collision ISR.

  NET_DEV_ISR_TYPE_JABBER,                                      ///< Dev jabber              ISR.
  NET_DEV_ISR_TYPE_BABBLE,                                      ///< Dev babble              ISR.

  NET_DEV_ISR_TYPE_PHY,                                         ///< Dev phy                 ISR.

  NET_DEV_ISR_TYPE_TX_DONE = NET_DEV_ISR_TYPE_TX_COMPLETE
} NET_DEV_ISR_TYPE;

/********************************************************************************************************
 *                              NETWORK DEVICE CONFIGURATION FLAG DATA TYPE
 *
 * Note(s) : (1) The following network device configuration flags are currently supported :
 *******************************************************************************************************/

typedef  CPU_INT32U NET_DEV_CFG_FLAGS;

//                                                                 See Note #1.
#define  NET_DEV_CFG_FLAG_NONE                           DEF_BIT_NONE
#define  NET_DEV_CFG_FLAG_SWAP_OCTETS                    DEF_BIT_00

#define  NET_DEV_CFG_FLAG_MASK                          (NET_DEV_CFG_FLAG_NONE \
                                                         | NET_DEV_CFG_FLAG_SWAP_OCTETS)

/********************************************************************************************************
 *                                   NETWORK INTERFACE TYPE DEFINES
 *
 * @note    (1) NET_IF_TYPE_&&& #define values specifically chosen as ASCII representations of the network
 *               interface types.  Memory displays of network interfaces will display the network interface
 *               TYPEs with their chosen ASCII names.
 *******************************************************************************************************/

typedef  enum  net_if_mem_type {
  NET_IF_MEM_TYPE_NONE = 0,
  NET_IF_MEM_TYPE_MAIN,                         ///< Create dev's net bufs in main      mem.
  NET_IF_MEM_TYPE_DEDICATED                     ///< Create dev's net bufs in dedicated mem.
} NET_IF_MEM_TYPE;

/********************************************************************************************************
 *                              NETWORK INTERFACE LINK STATE DATAT TYPE
 *******************************************************************************************************/

typedef  enum net_if_link_sate {
  NET_IF_LINK_UP,
  NET_IF_LINK_DOWN
} NET_IF_LINK_STATE;

/********************************************************************************************************
 *                         NETWORK INTERFACE LINK CHANGE HOOK FNCT DATA TYPE
 *******************************************************************************************************/

typedef  void (*NET_IF_LINK_SUBSCRIBER_FNCT)(NET_IF_NBR        if_nbr,
                                             NET_IF_LINK_STATE link_state);

typedef  struct  net_if_link_subscriber_obj NET_IF_LINK_SUBSCRIBER_OBJ;

/********************************************************************************************************
 *                        NETWORK INTERFACE PERFORMANCE MONITOR STATE DATA TYPE
 *******************************************************************************************************/

typedef  enum net_if_perf_mon_state {
  NET_IF_PERF_MON_STATE_NONE,
  NET_IF_PERF_MON_STATE_STOP,
  NET_IF_PERF_MON_STATE_START,
  NET_IF_PERF_MON_STATE_RUN
} NET_IF_PERF_MON_STATE;

/********************************************************************************************************
 *                                  INTERFACE START OBJECT DATA TYPE
 *******************************************************************************************************/

typedef  struct  net_if_start_obj NET_IF_START_OBJ;

typedef  struct  net_if_app_info {
  NET_IPv4_LINK_LOCAL_STATUS StatusIPv4LinkLocal;
  NET_IPv4_ADDR              AddrLinkLocalIPv4;
  DHCPc_STATUS               StatusDHCP;
  NET_IPv4_ADDR              AddrDynIPv4;
  NET_IPv4_ADDR              AddrDynGatewayIPv4;
  NET_IPv4_ADDR              AddrDynMaskIPv4;
  NET_IPv6_ADDR_CFG_STATUS   StatusLinkLocalIPv6;
  NET_IPv6_ADDR              AddrLinkLocalIPv6;
  NET_IPv6_ADDR_CFG_STATUS   StatusGlobalIPv6;
  NET_IPv6_ADDR              AddrGlobalIPv6;
  NET_IPv6_ADDR_CFG_STATUS   StatusStaticIPv6;
  NET_IPv6_ADDR              AddrStaticIPv6;
} NET_IF_APP_INFO;

/********************************************************************************************************
 *                                   NETWORK CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef  struct  net_if_buf_cfg {
  NET_BUF_QTY RxBufLargeNbr;
  NET_BUF_QTY TxBufLargeNbr;
  NET_BUF_QTY TxBufSmallNbr;
} NET_IF_BUF_CFG;

/********************************************************************************************************
 *                               INTERFACE START MODULES FLAGS DATA TYPE
 *******************************************************************************************************/

typedef  struct net_if_start_cfg_flags {
  CPU_BOOLEAN IPv4_LinkLocal  : 1;
  CPU_BOOLEAN IPv6_AutoCfg    : 1;
  CPU_BOOLEAN IPv6_Static     : 1;
  CPU_BOOLEAN DHCPc           : 1;
} NET_IF_START_CFG_FLAGS;

/********************************************************************************************************
 *                                     NETWORK INTERFACE DATA TYPE
 * @brief Network interface
 *
 * @note    (1) A network interface's hardware MTU is computed as the minimum of the largest buffer size
 *               configured for a specific interface & the configured MTU for the interface device.
 *
 *           (2) Network interface initialization flag set when an interface has been successfully added
 *               & initialized to the interface table.  Once set, this flag is never cleared since the
 *               removal of interfaces is currently not allowed.
 *
 *           (3) Network interface enable/disable independent of physical hardware link state of the
 *               interface's associated device.
 *******************************************************************************************************/
//                                                                 -------------------------- NET IF --------------------------
struct  net_if {
  NET_IF_TYPE                Type;                      ///< IF type (Loopback, Ethernet, PPP, Serial device, etc.).
  NET_IF_NBR                 Nbr;                       ///< IF nbr.

  CPU_CHAR                   *Name;

  CPU_BOOLEAN                Init;                      ///< IF init     status (see Note #2).
  CPU_BOOLEAN                En;                        ///< IF en/dis   status (see Note #3).

  NET_IF_LINK_STATE          LinkPrev;
  NET_IF_LINK_STATE          Link;                      ///< IF current  Phy link status.
  MEM_DYN_POOL               LinkSubscriberPool;

  NET_IF_LINK_SUBSCRIBER_OBJ *LinkSubscriberListHeadPtr;
  NET_IF_LINK_SUBSCRIBER_OBJ *LinkSubscriberListEndPtr;

  NET_IF_START_CFG_FLAGS     StartModulesCfgFlags;
  NET_IF_START_OBJ           *StartObj;

  NET_MTU                    MTU;                       ///< IF MTU             (see Note #1).

  void                       *IF_API;                   ///< Ptr to IF's     API  fnct tbl.
  void                       *IF_Data;                  ///< Ptr to IF's     data area.
  void                       *Dev_API;                  ///< Ptr to IF's dev API  fnct tbl.
  void                       *Dev_BSP;                  ///< Ptr to IF's dev BSP  fnct tbl.
  void                       *Dev_Cfg;                  ///< Ptr to IF's dev cfg       tbl.
  void                       *Dev_Data;                 ///< Ptr to IF's dev data area.
  void                       *Ext_API;                  ///< Ptr to IF's phy API  fnct tbl.
  void                       *Ext_Cfg;                  ///< Ptr to IF's phy cfg       tbl.
  void                       *Ext_Data;                 ///< Ptr to IF's phy data area.

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  NET_IF_PERF_MON_STATE      PerfMonState;              ///< Perf mon state.
  NET_TS_MS                  PerfMonTS_Prev_ms;         ///< Perf mon prev TS (in ms).
#endif

#ifdef  NET_LOAD_BAL_MODULE_EN
  NET_STAT_CTR  RxPktCtr;                               ///< Indicates nbr of rx pkts q'd to IF but NOT yet handled.

  void          *TxSuspendSignalObj;
  CPU_INT32U    TxSuspendTimeout_ms;
  NET_STAT_CTR  TxSuspendCtr;                           ///< Indicates nbr of tx conn's for  IF currently suspended.
#endif

  NET_IP_IF_OBJ *IP_Obj;

  void          *DevTxRdySignalObj;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//                                                                 =========================================== CTRL FNCTS ============================================

NET_IF_NBR NetIF_NbrGetFromName(CPU_CHAR *p_name);

#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
void NetIF_WaitSetupReady(NET_IF_NBR      if_nbr,
                          NET_IF_APP_INFO *p_info,
                          CPU_INT32U      timeout_ms,
                          RTOS_ERR        *p_err);
#endif

NET_IF_NBR NetIF_Add(void     *p_if_api,
                     void     *p_dev_api,
                     void     *p_dev_bsp,
                     void     *p_dev_cfg,
                     void     *p_ext_api,
                     void     *p_ext_cfg,
                     RTOS_ERR *p_err);

void NetIF_Start(NET_IF_NBR if_nbr,
                 RTOS_ERR   *p_err);

void NetIF_Stop(NET_IF_NBR if_nbr,
                RTOS_ERR   *p_err);

//                                                                 ============================================ CFG FNCTS ============================================

CPU_BOOLEAN NetIF_CfgPhyLinkPeriod(CPU_INT16U time_ms);

CPU_BOOLEAN NetIF_CfgPerfMonPeriod(CPU_INT16U time_ms);

void *NetIF_GetRxDataAlignPtr(NET_IF_NBR if_nbr,
                              void       *p_data,
                              RTOS_ERR   *p_err);

void *NetIF_GetTxDataAlignPtr(NET_IF_NBR if_nbr,
                              void       *p_data,
                              RTOS_ERR   *p_err);

//                                                                 ========================================== STATUS FNCTS ===========================================

CPU_BOOLEAN NetIF_IsValid(NET_IF_NBR if_nbr,
                          RTOS_ERR   *p_err);

CPU_BOOLEAN NetIF_IsValidCfgd(NET_IF_NBR if_nbr,
                              RTOS_ERR   *p_err);

CPU_BOOLEAN NetIF_IsEn(NET_IF_NBR if_nbr,
                       RTOS_ERR   *p_err);

CPU_BOOLEAN NetIF_IsEnCfgd(NET_IF_NBR if_nbr,
                           RTOS_ERR   *p_err);

CPU_INT08U NetIF_GetExtAvailCtr(RTOS_ERR *p_err);

NET_IF_NBR NetIF_GetNbrBaseCfgd(void);

NET_IF_TYPE NetIF_TypeGet(NET_IF_NBR if_nbr,
                          RTOS_ERR   *p_err);

//                                                                 =========================================== MGMT FNCTS ============================================

void NetIF_AddrHW_Get(NET_IF_NBR if_nbr,
                      CPU_INT08U *p_addr_hw,
                      CPU_INT08U *p_addr_len,
                      RTOS_ERR   *p_err);

void NetIF_AddrHW_Set(NET_IF_NBR if_nbr,
                      CPU_INT08U *p_addr_hw,
                      CPU_INT08U addr_len,
                      RTOS_ERR   *p_err);

CPU_BOOLEAN NetIF_AddrHW_IsValid(NET_IF_NBR if_nbr,
                                 CPU_INT08U *p_addr_hw,
                                 RTOS_ERR   *p_err);

NET_MTU NetIF_MTU_Get(NET_IF_NBR if_nbr,
                      RTOS_ERR   *p_err);

void NetIF_MTU_Set(NET_IF_NBR if_nbr,
                   NET_MTU    mtu,
                   RTOS_ERR   *p_err);

NET_IF_LINK_STATE NetIF_LinkStateGet(NET_IF_NBR if_nbr,
                                     RTOS_ERR   *p_err);

CPU_BOOLEAN NetIF_LinkStateWaitUntilUp(NET_IF_NBR if_nbr,
                                       CPU_INT16U retry_max,
                                       CPU_INT32U time_dly_ms,
                                       RTOS_ERR   *p_err);

void NetIF_LinkStateSubscribe(NET_IF_NBR                  if_nbr,
                              NET_IF_LINK_SUBSCRIBER_FNCT fcnt,
                              RTOS_ERR                    *p_err);

void NetIF_LinkStateUnsubscribe(NET_IF_NBR                  if_nbr,
                                NET_IF_LINK_SUBSCRIBER_FNCT fcnt,
                                RTOS_ERR                    *p_err);

void NetIF_IO_Ctrl(NET_IF_NBR if_nbr,
                   CPU_INT08U opt,
                   void       *p_data,
                   RTOS_ERR   *p_err);

#ifdef  NET_LOAD_BAL_MODULE_EN
void NetIF_TxSuspendTimeoutSet(NET_IF_NBR if_nbr,
                               CPU_INT32U timeout_ms,
                               RTOS_ERR   *p_err);

CPU_INT32U NetIF_TxSuspendTimeoutGet_ms(NET_IF_NBR if_nbr,
                                        RTOS_ERR   *p_err);
#endif

/********************************************************************************************************
 *                                               BSP API
 *******************************************************************************************************/

void NetIF_ISR_Handler(NET_IF_NBR       if_nbr,
                       NET_DEV_ISR_TYPE type,
                       RTOS_ERR         *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_IF_H_
