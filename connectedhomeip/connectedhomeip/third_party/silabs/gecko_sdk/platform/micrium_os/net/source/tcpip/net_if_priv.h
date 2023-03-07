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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_IF_PRIV_H_
#define  _NET_IF_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_def.h"
#include  "../../include/net_if.h"
#include  "../../include/net_cfg_net.h"
#include  "net_priv.h"
#include  "net_type_priv.h"
#include  "net_buf_priv.h"
#include  "net_def_priv.h"

#ifdef NET_DHCP_CLIENT_MODULE_EN
#include  <net/include/dhcp_client_types.h>
#endif

#include  <common/source/kal/kal_priv.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_IF_PHY_LINK_TIME_MIN_MS                      50
#define  NET_IF_PHY_LINK_TIME_MAX_MS                   60000
#define  NET_IF_PHY_LINK_TIME_DFLT_MS                    250

#define  NET_IF_PERF_MON_TIME_MIN_MS                      50
#define  NET_IF_PERF_MON_TIME_MAX_MS                   60000
#define  NET_IF_PERF_MON_TIME_DFLT_MS                    250

//                                                                 ----- CFG BUF DATA PROTOCOL MIN/MAX HDR SIZES ------

#if  (defined(NET_IF_ETHER_MODULE_EN) \
  || defined(NET_IF_WIFI_MODULE_EN))

    #define  NET_IF_HDR_SIZE_MIN                NET_IF_HDR_SIZE_ETHER
    #define  NET_IF_HDR_SIZE_MAX                NET_IF_HDR_SIZE_ETHER

#elif  (defined(NET_IF_LOOPBACK_MODULE_EN))

    #define  NET_IF_HDR_SIZE_MIN                NET_IF_HDR_SIZE_LOOPBACK
    #define  NET_IF_HDR_SIZE_MAX                NET_IF_HDR_SIZE_LOOPBACK

#else
    #define  NET_IF_HDR_SIZE_MIN                0
    #define  NET_IF_HDR_SIZE_MAX                0
#endif

/********************************************************************************************************
 *                                   NETWORK INTERFACE INDEX DEFINES
 *
 * Note(s) : (1) Since network data value macro's appropriately convert data values from any CPU addresses,
 *               word-aligned or not; network receive & transmit packets are NOT required to ensure that
 *               network packet headers (ARP/IP/UDP/TCP/etc.) & header members will locate on CPU word-
 *               aligned addresses.  Therefore, network interface packets are NOT required to start on
 *               any specific buffer indices.
 *
 *               See also 'net_util.h  NETWORK DATA VALUE MACRO'S           Note #2b'
 *                       & 'net_buf.h   NETWORK BUFFER INDEX & SIZE DEFINES  Note #2'.
 *******************************************************************************************************/
//                                                                 See Note #1.
#define  NET_IF_IX_RX                                    NET_BUF_DATA_IX_RX
#define  NET_IF_RX_IX                                    NET_IF_IX_RX       // Req'd for backwards-compatibility.

/********************************************************************************************************
 *                                   NETWORK INTERFACE NUMBER DATA TYPE
 *******************************************************************************************************/

#define  NET_IF_NBR_BASE                                   0
#define  NET_IF_NBR_BASE_CFGD                           (NET_IF_NBR_BASE       + NET_IF_NBR_IF_RESERVED)

#define  NET_IF_NBR_MIN                                  NET_IF_NBR_IF_RESERVED
#define  NET_IF_NBR_MAX                                 (NET_IF_NBR_NONE - 1)

//                                                                 Reserved net IF nbrs :
#define  NET_IF_NBR_LOOPBACK                            (NET_IF_NBR_BASE + 0)
#define  NET_IF_NBR_LOCAL_HOST                           NET_IF_NBR_LOOPBACK
#define  NET_IF_NBR_WILDCARD                             NET_IF_NBR_NONE

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               NETWORK INTERFACE CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef  struct  net_if_cfg {
  const CPU_CHAR  *HW_AddrStr;
  NET_IF_IPv4_CFG IPv4;
  NET_IF_IPv6_CFG IPv6;
} NET_IF_CFG;

/********************************************************************************************************
 *                               NETWORK INTERFACE TRANSFER READY STATUS
 *******************************************************************************************************/

typedef  enum  net_if_tx_rdy_status {
  NET_IF_TX_RDY_STATUS_NONE,
  NET_IF_TX_RDY_STATUS_UNICAST,
  NET_IF_TX_RDY_STATUS_MULTICAST,
  NET_IF_TX_RDY_STATUS_BROADCAST,
  NET_IF_TX_RDY_STATUS_ADDR_REQ
} NET_IF_TX_RDY_STATUS;

struct  net_if_link_subscriber_obj {
  NET_IF_LINK_SUBSCRIBER_FNCT Fnct;
  CPU_INT32U                  RefCtn;
  NET_IF_LINK_SUBSCRIBER_OBJ  *NextPtr;
};

/********************************************************************************************************
 *                                   INTERFACE START OBJECT DATA TYPE
 *******************************************************************************************************/

#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
typedef  enum  net_if_start_msg_type {
  NET_IF_START_MSG_TYPE_LINK_UP,
  NET_IF_START_MSG_TYPE_IPv4_LINK_LOCAL,
  NET_IF_START_MSG_TYPE_DHCP,
  NET_IF_START_MSG_TYPE_IPv6_STATIC,
  NET_IF_START_MSG_TYPE_IPv6_LINK_LOCAL,
  NET_IF_START_MSG_TYPE_IPv6_GLOBAL
} NET_IF_START_MSG_TYPE;

typedef  struct  net_if_start_msg {
  NET_IF_START_MSG_TYPE Type;
  NET_IF_NBR            IF_Nbr;
  SLIST_MEMBER          ListNode;
} NET_IF_START_MSG;

typedef  struct net_if_start_rdy_flags {
  CPU_BOOLEAN IF_LinkUpRdy      : 1;
  CPU_BOOLEAN IPv4_LinkLocalRdy : 1;
  CPU_BOOLEAN IPv6_LinkLocalRdy : 1;
  CPU_BOOLEAN IPv6_GlobalRdy    : 1;
  CPU_BOOLEAN IPv6_StaticRdy    : 1;
  CPU_BOOLEAN DHCPc_Rdy         : 1;
} NET_IF_START_RDY_FLAGS;
#endif

struct  net_if_start_obj {
#if (NET_IF_CFG_WAIT_SETUP_READY_EN == DEF_ENABLED)
  MEM_DYN_POOL                      StartMsgPool;
  KAL_SEM_HANDLE                    SemHandle;
  SLIST_MEMBER                      *MsgListPtr;
  NET_IF_START_RDY_FLAGS            RdyFlags;
  NET_IF_APP_INFO                   *AppInfoPtr;
#endif
  CPU_BOOLEAN                       IPv4_LinkLocalEn;
  NET_IPv4_LINK_LOCAL_COMPLETE_HOOK IPv4_LinkLocalHook;
  NET_IPv6_ADDR_HOOK_FNCT           IPv6_Hook;
  DHCPc_ON_COMPLETE_HOOK            DHCPc_Hook;
};

/********************************************************************************************************
 *                           GENERIC NETWORK DEVICE CONFIGURATION DATA TYPE
 *
 * Note(s) : (1) The generic network device configuration data type is a template/subset for all specific
 *               network device configuration data types.  Each specific network device configuration
 *               data type MUST define ALL generic network device configuration parameters, synchronized
 *               in both the sequential order & data type of each parameter.
 *
 *               Thus ANY modification to the sequential order or data types of generic configuration
 *               parameters MUST be appropriately synchronized between the generic network device
 *               configuration data type & ALL specific network device configuration data types.
 *******************************************************************************************************/

//                                                                 ------------------------- NET DEV CFG --------------------------
struct  net_dev_cfg {
  NET_IF_MEM_TYPE   RxBufPoolType;                  // Rx buf mem pool type :
                                                    //             NET_IF_MEM_TYPE_MAIN        bufs alloc'd from main      mem
                                                    //             NET_IF_MEM_TYPE_DEDICATED   bufs alloc'd from dedicated mem
  NET_BUF_SIZE      RxBufLargeSize;                 // Size  of dev rx large buf data areas (in octets).
  NET_BUF_QTY       RxBufLargeNbr;                  // Nbr   of dev rx large buf data areas.
  NET_BUF_SIZE      RxBufAlignOctets;               // Align of dev rx       buf data areas (in octets).
  NET_BUF_SIZE      RxBufIxOffset;                  // Offset from base ix to rx data into data area (in octets).

  NET_IF_MEM_TYPE   TxBufPoolType;                  // Tx buf mem pool type :
                                                    //             NET_IF_MEM_TYPE_MAIN        bufs alloc'd from main      mem
                                                    //             NET_IF_MEM_TYPE_DEDICATED   bufs alloc'd from dedicated mem
  NET_BUF_SIZE      TxBufLargeSize;                 // Size  of dev tx large buf data areas (in octets).
  NET_BUF_QTY       TxBufLargeNbr;                  // Nbr   of dev tx large buf data areas.
  NET_BUF_SIZE      TxBufSmallSize;                 // Size  of dev tx small buf data areas (in octets).
  NET_BUF_QTY       TxBufSmallNbr;                  // Nbr   of dev tx small buf data areas.
  NET_BUF_SIZE      TxBufAlignOctets;               // Align of dev tx       buf data areas (in octets).
  NET_BUF_SIZE      TxBufIxOffset;                  // Offset from base ix to tx data from data area (in octets).

  CPU_ADDR          MemAddr;                        // Base addr of (dev's) dedicated mem, if avail.
  CPU_ADDR          MemSize;                        // Size      of (dev's) dedicated mem, if avail.

  NET_DEV_CFG_FLAGS Flags;                          // Opt'l bit flags.
};

/********************************************************************************************************
 *                               GENERIC NETWORK INTERFACE API DATA TYPE
 *
 * Note(s) : (1) The generic network interface application programming interface (API) data type is a
 *               template/subset for all specific network interface API data types.
 *
 *               (a) Each specific network interface API data type definition MUST define ALL generic
 *                   network interface API functions, synchronized in both the sequential order of the
 *                   functions & argument lists for each function.
 *
 *                   Thus ANY modification to the sequential order or argument lists of the generic API
 *                   functions MUST be appropriately synchronized between the generic network interface
 *                   API data type & ALL specific network interface API data type definitions/instantiations.
 *
 *               (b) ALL API functions SHOULD be defined with NO NULL functions for all specific network
 *                   interface API instantiations.  Any specific network interface API instantiation that
 *                   does define any NULL API functions MUST ensure that NO NULL API functions are called
 *                   for the specific network interface.
 *
 *                   Instead of  NULL functions, a specific network interface API instantiation COULD
 *                   define empty API functions that return error code 'RTOS_ERR_NOT_AVAIL'.
 *******************************************************************************************************/

//                                                                 ---------- NET IF API ----------
//                                                                 Net IF API fnct ptrs :
struct  net_if_api {
  //                                                               Init/add
  void (*Add)                      (NET_IF   *p_if,
                                    RTOS_ERR *p_err);

  //                                                               Start
  void (*Start)                    (NET_IF   *p_if,
                                    RTOS_ERR *p_err);

  //                                                               Stop
  void (*Stop)                     (NET_IF   *p_if,
                                    RTOS_ERR *p_err);

  //                                                               Rx
  void (*Rx)                       (NET_IF   *p_if,
                                    NET_BUF  *p_buf,
                                    RTOS_ERR *p_err);

  //                                                               Tx
  void (*Tx)                       (NET_IF   *p_if,
                                    NET_BUF  *p_buf,
                                    RTOS_ERR *p_err);

  //                                                               Hw addr get
  void (*AddrHW_Get)               (NET_IF     *p_if,
                                    CPU_INT08U *p_addr,
                                    CPU_INT08U *addr_len,
                                    RTOS_ERR   *p_err);

  //                                                               Hw addr set
  void (*AddrHW_Set)               (NET_IF     *p_if,
                                    CPU_INT08U *p_addr,
                                    CPU_INT08U addr_len,
                                    RTOS_ERR   *p_err);

  //                                                               Hw addr valid
  CPU_BOOLEAN (*AddrHW_IsValid)           (NET_IF     *p_if,
                                           CPU_INT08U *p_addr_hw);

  //                                                               Multicast addr add
  void (*AddrMulticastAdd)         (NET_IF            *p_if,
                                    CPU_INT08U        *p_addr_protocol,
                                    CPU_INT08U        paddr_protocol_len,
                                    NET_PROTOCOL_TYPE addr_protocol_type,
                                    RTOS_ERR          *p_err);

  //                                                               Multicast addr remove
  void (*AddrMulticastRemove)      (NET_IF            *p_if,
                                    CPU_INT08U        *p_addr_protocol,
                                    CPU_INT08U        paddr_protocol_len,
                                    NET_PROTOCOL_TYPE addr_protocol_type,
                                    RTOS_ERR          *p_err);

  //                                                               Multicast addr protocol-to-hw
  void (*AddrMulticastProtocolToHW)(NET_IF            *p_if,
                                    CPU_INT08U        *p_addr_protocol,
                                    CPU_INT08U        addr_protocol_len,
                                    NET_PROTOCOL_TYPE addr_protocol_type,
                                    CPU_INT08U        *p_addr_hw,
                                    CPU_INT08U        *p_addr_hw_len,
                                    RTOS_ERR          *p_err);

  //                                                               Buf cfg validation
  void (*BufPoolCfgValidate)       (NET_IF   *p_if,
                                    RTOS_ERR *p_err);

  //                                                               MTU set
  void (*MTU_Set)                  (NET_IF   *p_if,
                                    NET_MTU  mtu,
                                    RTOS_ERR *p_err);

  //                                                               Get pkt hdr size
  CPU_INT16U (*GetPktSizeHdr)            (NET_IF *p_if);

  //                                                               Get pkt min size
  CPU_INT16U (*GetPktSizeMin)            (NET_IF *p_if);

  //                                                               Get pkt min size
  CPU_INT16U (*GetPktSizeMax)            (NET_IF *p_if);

  //                                                               ISR handler
  void       (*ISR_Handler)              (NET_IF           *p_if,
                                          NET_DEV_ISR_TYPE type,
                                          RTOS_ERR         *p_err);

  //                                                               I/O ctrl
  void (*IO_Ctrl)                  (NET_IF     *p_if,
                                    CPU_INT08U opt,
                                    void       *p_data,
                                    RTOS_ERR   *p_err);
};

/********************************************************************************************************
 *                                   GENERIC NETWORK DEVICE API DATA TYPE
 *
 * Note(s) : (1) The generic network device application programming interface (API) data type is a template/
 *               subset for all specific network device API data types.
 *
 *               (a) Each specific network device API data type definition MUST define ALL generic network
 *                   device API functions, synchronized in both the sequential order of the functions &
 *                   argument lists for each function.
 *
 *                   Thus ANY modification to the sequential order or argument lists of the  generic API
 *                   functions MUST be appropriately synchronized between the generic network device API
 *                   data type & ALL specific network device API data type definitions/instantiations.
 *
 *                   However, specific network device API data type definitions/instantiations MAY include
 *                   additional API functions after all generic network device API functions.
 *
 *               (b) ALL API functions MUST be defined with NO NULL functions for all specific network
 *                   device API instantiations.  Any specific network device API instantiation that does
 *                   NOT require a specific API's functionality MUST define an empty API function which
 *                   may need to return an appropriate error code.
 *******************************************************************************************************/

//                                                                 -------------------- NET DEV API -------------------
//                                                                 Net dev API fnct ptrs :
typedef  struct  net_dev_api {
  //                                                               Init
  void (*Init)(NET_IF   *p_if,
               RTOS_ERR *p_err);
  //                                                               Start
  void (*Start)(NET_IF   *p_if,
                RTOS_ERR *p_err);
  //                                                               Stop
  void (*Stop)(NET_IF   *p_if,
               RTOS_ERR *p_err);
  //                                                               Rx
  void (*Rx)(NET_IF     *p_if,
             CPU_INT08U **p_data,
             CPU_INT16U *size,
             RTOS_ERR   *p_err);
  //                                                               Tx
  void (*Tx)(NET_IF     *p_if,
             CPU_INT08U *p_data,
             CPU_INT16U size,
             RTOS_ERR   *p_err);
  //                                                               Multicast addr add
  void (*AddrMulticastAdd)(NET_IF     *p_if,
                           CPU_INT08U *p_addr_hw,
                           CPU_INT08U addr_hw_len,
                           RTOS_ERR   *p_err);
  //                                                               Multicast addr remove
  void (*AddrMulticastRemove)(NET_IF     *p_if,
                              CPU_INT08U *p_addr_hw,
                              CPU_INT08U addr_hw_len,
                              RTOS_ERR   *p_err);
  //                                                               ISR handler
  void (*ISR_Handler)(NET_IF           *p_if,
                      NET_DEV_ISR_TYPE type);
} NET_DEV_API;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               DRIVER API
 *******************************************************************************************************/

void NetIF_RxQPost(NET_IF_NBR if_nbr,                          // Signal IF rx rdy    from dev rx ISR(s).
                   RTOS_ERR   *p_err);

NET_IF_NBR NetIF_RxQPend(RTOS_ERR *p_err);

void NetIF_RxHandler(NET_IF_NBR if_nbr);

void NetIF_DevCfgTxRdySignal(NET_IF     *p_if,
                             CPU_INT16U cnt);

void NetIF_DevTxRdySignal(NET_IF *p_if);

void NetIF_TxDeallocQPost(CPU_INT08U *p_buf_data,               // Post to tx dealloc Q.
                          RTOS_ERR   *p_err);

CPU_INT08U *NetIF_TxDeallocQPend(RTOS_ERR *p_err);

/********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 *******************************************************************************************************/

void NetIF_Init(MEM_SEG  *p_mem_seg,
                RTOS_ERR *p_err);

void NetIF_StartInternal(NET_IF_NBR if_nbr,
                         NET_IF_CFG *p_cfg,
                         RTOS_ERR   *p_err);

void NetIF_BufPoolInit(NET_IF   *p_if,
                       RTOS_ERR *p_err);

NET_IF *NetIF_Get(NET_IF_NBR if_nbr,
                  RTOS_ERR   *p_err);

NET_IF_NBR NetIF_GetDflt(void);

CPU_BOOLEAN NetIF_IsValidHandler(NET_IF_NBR if_nbr);

CPU_BOOLEAN NetIF_IsValidCfgdHandler(NET_IF_NBR if_nbr);

CPU_BOOLEAN NetIF_IsEnHandler(NET_IF_NBR if_nbr,
                              RTOS_ERR   *p_err);

CPU_BOOLEAN NetIF_IsEnCfgdHandler(NET_IF_NBR if_nbr,
                                  RTOS_ERR   *p_err);

void NetIF_AddrHW_GetHandler(NET_IF_NBR if_nbr,
                             CPU_INT08U *p_addr_hw,
                             CPU_INT08U *p_addr_len,
                             RTOS_ERR   *p_err);

void NetIF_AddrHW_SetHandler(NET_IF_NBR if_nbr,
                             CPU_INT08U *p_addr_hw,
                             CPU_INT08U addr_len,
                             RTOS_ERR   *p_err);

CPU_BOOLEAN NetIF_AddrHW_IsValidHandler(NET_IF_NBR if_nbr,
                                        CPU_INT08U *p_addr_hw,
                                        RTOS_ERR   *p_err);

#ifdef  NET_MCAST_MODULE_EN
void NetIF_AddrMulticastAdd(NET_IF_NBR        if_nbr,
                            CPU_INT08U        *p_addr_protocol,
                            CPU_INT08U        addr_protocol_len,
                            NET_PROTOCOL_TYPE addr_protocol_type,
                            RTOS_ERR          *p_err);

void NetIF_AddrMulticastRemove(NET_IF_NBR        if_nbr,
                               CPU_INT08U        *p_addr_protocol,
                               CPU_INT08U        addr_protocol_len,
                               NET_PROTOCOL_TYPE addr_protocol_type,
                               RTOS_ERR          *p_err);
#endif
#ifdef  NET_MCAST_TX_MODULE_EN
void NetIF_AddrMulticastProtocolToHW(NET_IF_NBR        if_nbr,
                                     CPU_INT08U        *p_addr_protocol,
                                     CPU_INT08U        addr_protocol_len,
                                     NET_PROTOCOL_TYPE addr_protocol_type,
                                     CPU_INT08U        *p_addr_hw,
                                     CPU_INT08U        *p_addr_hw_len,
                                     RTOS_ERR          *p_err);
#endif

NET_MTU NetIF_MTU_GetProtocol(NET_IF_NBR        if_nbr,
                              NET_PROTOCOL_TYPE protocol,
                              NET_IF_FLAG       opt);

void NetIF_MTU_SetHandler(NET_IF_NBR if_nbr,
                          NET_MTU    mtu,
                          RTOS_ERR   *p_err);

CPU_INT16U NetIF_GetPayloadRxMax(NET_IF_NBR        if_nbr,
                                 NET_PROTOCOL_TYPE protocol);

CPU_INT16U NetIF_GetPayloadTxMax(NET_IF_NBR        if_nbr,
                                 NET_PROTOCOL_TYPE protocol);

CPU_INT16U NetIF_GetPktSizeMin(NET_IF_NBR if_nbr);

CPU_INT16U NetIF_GetPktSizeMax(NET_IF_NBR if_nbr);

void NetIF_RxPktInc(NET_IF_NBR if_nbr);

CPU_BOOLEAN NetIF_RxPktIsAvail(NET_IF_NBR if_nbr,
                               NET_CTR    rx_chk_nbr);

void NetIF_Tx(NET_BUF  *p_buf_list,
              RTOS_ERR *p_err);

void NetIF_TxSuspend(NET_IF_NBR if_nbr);

void NetIF_TxIxDataGet(NET_IF_NBR if_nbr,
                       CPU_INT32U data_size,
                       CPU_INT16U *p_ix);

void NetIF_TxPktListDealloc(CPU_INT08U *p_buf_data);

NET_IF_LINK_STATE NetIF_LinkStateGetHandler(NET_IF_NBR if_nbr,
                                            RTOS_ERR   *p_err);

void NetIF_LinkStateSubscribeHandler(NET_IF_NBR                  if_nbr,
                                     NET_IF_LINK_SUBSCRIBER_FNCT fcnt,
                                     RTOS_ERR                    *p_err);

void NetIF_LinkStateUnSubscribeHandler(NET_IF_NBR                  if_nbr,
                                       NET_IF_LINK_SUBSCRIBER_FNCT fcnt,
                                       RTOS_ERR                    *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_IF_CFG_MAX_NBR_IF
#error  "NET_IF_CFG_MAX_NBR_IF                   not #define'd in 'net_cfg.h'"
#error  "                                  [MUST be  >= NET_IF_NBR_MIN_VAL]  "

#elif   (DEF_CHK_VAL_MIN(NET_IF_CFG_MAX_NBR_IF, \
                         NET_IF_NBR_MIN_VAL) != DEF_OK)
#error  "NET_IF_CFG_MAX_NBR_IF             illegally #define'd in 'net_cfg.h'"
#error  "                                  [MUST be  >= NET_IF_NBR_MIN_VAL]  "
#endif

//                                                                 Correctly configured in 'net_cfg_net.h'; DO NOT MODIFY.
#ifndef  NET_IF_NBR_IF_TOT
#error  "NET_IF_NBR_IF_TOT                       not #define'd in 'net_cfg_net.h'"
#error  "                                  [MUST be  >= NET_IF_NBR_MIN]          "
#error  "                                  [     &&  <= NET_IF_NBR_MAX]          "

#elif   (DEF_CHK_VAL(NET_IF_NBR_IF_TOT, \
                     NET_IF_NBR_MIN,    \
                     NET_IF_NBR_MAX) != DEF_OK)
#error  "NET_IF_NBR_IF_TOT                 illegally #define'd in 'net_cfg_net.h'"
#error  "                                  [MUST be  >= NET_IF_NBR_MIN]          "
#error  "                                  [     &&  <= NET_IF_NBR_MAX]          "
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_IF_PRIV_H_
