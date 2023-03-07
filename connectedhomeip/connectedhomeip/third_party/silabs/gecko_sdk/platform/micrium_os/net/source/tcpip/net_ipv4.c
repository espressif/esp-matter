/***************************************************************************//**
 * @file
 * @brief Network Ip Layer Version 4 - (Internet Protocol V4)
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

#include  <net/include/net_cfg_net.h>
#ifdef  NET_IPv4_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <net/include/net_util.h>
#include  <net/include/net_arp.h>
#include  <net/include/net_type.h>

#include  "net_ipv4_priv.h"
#include  "net_icmpv4_priv.h"
#include  "net_igmp_priv.h"
#include  "net_buf_priv.h"
#include  "net_util_priv.h"
#include  "net_priv.h"
#include  "net_udp_priv.h"
#include  "net_tcp_priv.h"
#include  "net_conn_priv.h"
#include  "net_if_priv.h"
#include  "net_cache_priv.h"
#include  "net_arp_priv.h"

#include  <net/source/util/net_svc_task_priv.h>
#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 *                                           IPv4 HEADER DEFINES
 *******************************************************************************************************/

#define  NET_IPv4_HDR_VER_MASK                          0xF0u
#define  NET_IPv4_HDR_VER_SHIFT                            4u
#define  NET_IPv4_HDR_VER                                  4u   // Supports IPv4 ONLY (see 'net_ipv4.h  Note #1').

#define  NET_IPv4_HDR_LEN_MASK                          0x0Fu

#define  NET_IPv4_ID_INIT                                NET_IPv4_ID_NONE

/********************************************************************************************************
 *                                       IPv4 HEADER OPTIONS DEFINES
 *
 * Note(s) : (1) See the following RFC's for IPv4 options summary :
 *
 *               (a) RFC # 791, Section 3.1     'Options'
 *               (b) RFC #1122, Section 3.2.1.8
 *               (c) RFC #1108
 *
 *           (2) IPv4 option types are encoded in the first octet for each IP option as follows :
 *
 *                           7   6 5  4 3 2 1 0
 *                       ---------------------
 *                       |CPY|CLASS|  N B R  |
 *                       ---------------------
 *
 *                   where
 *                           CPY         Indicates whether option is copied into all fragments :
 *                                           '0' - IP option NOT copied into fragments
 *                                           '1' - IP option     copied into fragments
 *                           CLASS       Indicates options class :
 *                                           '00' - Control
 *                                           '01' - Reserved
 *                                           '10' - Debug / Measurement
 *                                           '11' - Reserved
 *                           NBR         Option Number :
 *                                           '00000' - End of Options List
 *                                           '00001' - No Operation
 *                                           '00010' - Security
 *                                           '00011' - Loose  Source Routing
 *                                           '00100' - Internet Timestamp
 *                                           '00111' - Record Route
 *                                           '01001' - Strict Source Routing
 *
 *           (3) IPv4 header allows for a maximum option list length of ten (10) 32-bit options :
 *
 *                   NET_IPv4_HDR_OPT_SIZE_MAX = (NET_IPv4_HDR_SIZE_MAX - NET_IPv4_HDR_SIZE_MIN) / NET_IPv4_HDR_OPT_SIZE_WORD
 *
 *                                           = (60 - 20) / (32-bits)
 *
 *                                           =  Ten (10) 32-bit options
 *
 *           (4) 'NET_IPv4_OPT_SIZE'  MUST be pre-defined PRIOR to all definitions that require IPv4 option
 *                   size data type.
 *******************************************************************************************************/

#define  NET_IPv4_HDR_OPT_COPY_FLAG               DEF_BIT_07

#define  NET_IPv4_HDR_OPT_CLASS_MASK                    0x60u
#define  NET_IPv4_HDR_OPT_CLASS_CTRL                    0x00u
#define  NET_IPv4_HDR_OPT_CLASS_RESERVED_1              0x20u
#define  NET_IPv4_HDR_OPT_CLASS_DBG                     0x40u
#define  NET_IPv4_HDR_OPT_CLASS_RESERVED_2              0x60u

#define  NET_IPv4_HDR_OPT_NBR_MASK                      0x1Fu
#define  NET_IPv4_HDR_OPT_NBR_END_LIST                  0x00u
#define  NET_IPv4_HDR_OPT_NBR_NOP                       0x01u
#define  NET_IPv4_HDR_OPT_NBR_SECURITY                  0x02u   // See 'net_ipv4.h  Note #1d'.
#define  NET_IPv4_HDR_OPT_NBR_ROUTE_SRC_LOOSE           0x03u
#define  NET_IPv4_HDR_OPT_NBR_SECURITY_EXTENDED         0x05u   // See 'net_ipv4.h  Note #1d'.
#define  NET_IPv4_HDR_OPT_NBR_TS                        0x04u
#define  NET_IPv4_HDR_OPT_NBR_ROUTE_REC                 0x07u
#define  NET_IPv4_HDR_OPT_NBR_ROUTE_SRC_STRICT          0x09u

#define  NET_IPv4_HDR_OPT_END_LIST             (NET_IPv4_HDR_OPT_CLASS_CTRL | NET_IPv4_HDR_OPT_NBR_END_LIST)
#define  NET_IPv4_HDR_OPT_NOP                  (NET_IPv4_HDR_OPT_CLASS_CTRL | NET_IPv4_HDR_OPT_NBR_NOP)
#define  NET_IPv4_HDR_OPT_SECURITY             (NET_IPv4_HDR_OPT_COPY_FLAG | NET_IPv4_HDR_OPT_CLASS_CTRL | NET_IPv4_HDR_OPT_NBR_SECURITY)
#define  NET_IPv4_HDR_OPT_ROUTE_SRC_LOOSE      (NET_IPv4_HDR_OPT_COPY_FLAG | NET_IPv4_HDR_OPT_CLASS_CTRL | NET_IPv4_HDR_OPT_NBR_ROUTE_SRC_LOOSE)
#define  NET_IPv4_HDR_OPT_SECURITY_EXTENDED    (NET_IPv4_HDR_OPT_COPY_FLAG | NET_IPv4_HDR_OPT_CLASS_CTRL | NET_IPv4_HDR_OPT_NBR_SECURITY_EXTENDED)
#define  NET_IPv4_HDR_OPT_TS                   (NET_IPv4_HDR_OPT_CLASS_DBG  | NET_IPv4_HDR_OPT_NBR_TS)
#define  NET_IPv4_HDR_OPT_ROUTE_REC            (NET_IPv4_HDR_OPT_CLASS_CTRL | NET_IPv4_HDR_OPT_NBR_ROUTE_REC)
#define  NET_IPv4_HDR_OPT_ROUTE_SRC_STRICT     (NET_IPv4_HDR_OPT_COPY_FLAG | NET_IPv4_HDR_OPT_CLASS_CTRL | NET_IPv4_HDR_OPT_NBR_ROUTE_SRC_STRICT)

#define  NET_IPv4_HDR_OPT_PAD                           0x00u

//                                                                 ---------------- SRC/REC ROUTE OPTS ----------------
#define  NET_IPv4_OPT_ROUTE_PTR_OPT                        0    // Ptr ix to       route opt itself.
#define  NET_IPv4_OPT_ROUTE_PTR_ROUTE                      4    // Ptr ix to first route (min legal ptr val).

//                                                                 --------------------- TS OPTS ----------------------
#define  NET_IPv4_OPT_TS_PTR_OPT                           0    // Ptr ix to       TS    opt itself.
#define  NET_IPv4_OPT_TS_PTR_TS                            4    // Ptr ix to first TS    (min legal ptr val).

#define  NET_IPv4_OPT_TS_OVF_MASK                       0xF0u
#define  NET_IPv4_OPT_TS_OVF_SHIFT                         4u
#define  NET_IPv4_OPT_TS_OVF_MAX                          15u

#define  NET_IPv4_OPT_TS_FLAG_MASK                      0x0Fu
#define  NET_IPv4_OPT_TS_FLAG_TS_ONLY                      0u
#define  NET_IPv4_OPT_TS_FLAG_TS_ROUTE_REC                 1u
#define  NET_IPv4_OPT_TS_FLAG_TS_ROUTE_SPEC                3u

#define  NET_IPv4_HDR_OPT_SIZE_ROUTE                     NET_IPv4_HDR_OPT_SIZE_WORD
#define  NET_IPv4_HDR_OPT_SIZE_TS                        NET_IPv4_HDR_OPT_SIZE_WORD
#define  NET_IPv4_HDR_OPT_SIZE_SECURITY                    3

#define  NET_IPv4_OPT_PARAM_NBR_MIN                        1

#define  NET_IPv4_OPT_PARAM_NBR_MAX_ROUTE                  9
#define  NET_IPv4_OPT_PARAM_NBR_MAX_TS_ONLY                9
#define  NET_IPv4_OPT_PARAM_NBR_MAX_TS_ROUTE               4

#define  NET_IPv4_HDR_OPT_IX                             NET_IPv4_HDR_SIZE_MIN
#define  NET_IPv4_OPT_IX_RX                                0

/********************************************************************************************************
 *                                           LINK-LOCAL ADDRESSES DEFINES
 *******************************************************************************************************/

#define  NET_IPv4_LOCAL_LINK_PROBE_WAIT_S                    1
#define  NET_IPv4_LOCAL_LINK_ANNOUNCE_WAIT_S                 2

#define  NET_IPv4_CFG_LOCAL_LINK_MAX_RETRY                   2

#define  NET_IPv4_LOCAL_LINK_ANNOUNCE_NUM                    2
#define  NET_IPv4_LOCAL_LINK_ANNOUNCE_INTERVAL_S             2
#define  NET_IPv4_LOCAL_LINK_MAX_CONFLICTS                   5
#define  NET_IPv4_LOCAL_LINK_RATE_LIMIT_INTERVAL_S          60

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   IPv4 SOURCE ROUTE OPTION DATA TYPE
 *
 * Note(s) : (1) See the following RFC's for Source Route options summary :
 *
 *               (a) RFC # 791, Section 3.1         'Options : Loose/Strict Source & Record Route'
 *               (b) RFC #1122, Section 3.2.1.8.(c)
 *
 *           (2) Used for both Source Route options & Record Route options :
 *
 *               (a) NET_IPv4_HDR_OPT_ROUTE_SRC_LOOSE
 *               (b) NET_IPv4_HDR_OPT_ROUTE_SRC_STRICT
 *               (c) NET_IPv4_HDR_OPT_ROUTE_REC
 *
 *           (3) 'Route' declared with 1 entry; prevents removal by compiler optimization.
 *******************************************************************************************************/

typedef  struct  net_ipv4_opt_src_route {
  CPU_INT08U    Type;                                           // Src route type (see Note #2).
  CPU_INT08U    Len;                                            // Len of   src route opt (in octets).
  CPU_INT08U    Ptr;                                            // Ptr into src route opt (octet-ix'd).
  CPU_INT08U    Pad;                                            // Forced word-alignment pad octet.
  NET_IPv4_ADDR Route[1];                                       // Src route IPv4 addrs (see Note #3).
} NET_IPv4_OPT_SRC_ROUTE;

/********************************************************************************************************
 *                               IPv4 INTERNET TIMESTAMP OPTION DATA TYPE
 *
 * Note(s) : (1) See the following RFC's for Internet Timestamp option summary :
 *
 *               (a) RFC # 791, Section 3.1         'Options : Internet Timestamp'
 *               (b) RFC #1122, Section 3.2.1.8.(e)
 *
 *           (2) 'TS'/'Route'/'Route_TS' declared with 1 entry; prevents removal by compiler optimization.
 *******************************************************************************************************/

typedef  struct  net_ipv4_opt_ts {
  CPU_INT08U Type;                                              // TS type.
  CPU_INT08U Len;                                               // Len of   src route opt (in octets).
  CPU_INT08U Ptr;                                               // Ptr into src route opt (octet-ix'd).
  CPU_INT08U Ovf_Flags;                                         // Ovf/Flags.
  NET_TS     TS[1];                                             // Timestamps (see Note #2).
} NET_IPv4_OPT_TS;

typedef  struct  net_ipv4_route_ts {
  NET_IPv4_ADDR Route[1];                                       // Route IPv4 addrs (see Note #2).
  NET_TS        TS[1];                                          // Timestamps     (see Note #2).
} NET_IPv4_ROUTE_TS;

#define  NET_IPv4_OPT_TS_ROUTE_SIZE              (sizeof(NET_IPv4_ROUTE_TS))

typedef  struct  net_ipv4_opt_ts_route {
  CPU_INT08U        Type;                                       // TS type.
  CPU_INT08U        Len;                                        // Len of   src route opt (in octets).
  CPU_INT08U        Ptr;                                        // Ptr into src route opt (octet-ix'd).
  CPU_INT08U        Ovf_Flags;                                  // Ovf/Flags.
  NET_IPv4_ROUTE_TS Route_TS[1];                                // Route IPv4 addrs / TS (see Note #2).
} NET_IPv4_OPT_TS_ROUTE;

/********************************************************************************************************
 *                                   IPV4 LINK LOCAL ADDR MODULE DATA TYPE
 *******************************************************************************************************/

#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
typedef  enum net_ipv4_link_local_msg_type {
  NET_IPv4_LINK_LOCAL_MSG_TYPE_NEW_IF,
  NET_IPv4_LINK_LOCAL_MSG_TYPE_START_IF,
  NET_IPv4_LINK_LOCAL_MSG_TYPE_REBOOT_IF,
  NET_IPv4_LINK_LOCAL_MSG_TYPE_STOP_IF
} NET_IPv4_LINK_LOCAL_MSG_TYPE;

typedef  enum  net_ipv4_link_local_state {
  NET_IPv4_LINK_LOCAL_STATE_NONE,
  NET_IPv4_LINK_LOCAL_STATE_INIT,
  NET_IPv4_LINK_LOCAL_STATE_VALIDATE_ADDR,
  NET_IPv4_LINK_LOCAL_STATE_ANNOUNCE_ADDR
} NET_IPv4_LINK_LOCAL_STATE;

typedef  struct  net_ipv4_link_local_obj {
  NET_IF_NBR                        IF_Nbr;
  NET_IPv4_LINK_LOCAL_STATE         State;
  NET_IPv4_ADDR                     Addr;
  CPU_INT08U                        AnnounceNbr;
  NET_SVC_TASK_TMR_HANDLE           SvcTaskTmr;
  NET_IPv4_LINK_LOCAL_COMPLETE_HOOK CompleteHook;
  SLIST_MEMBER                      ListNode;
} NET_IPv4_LINK_LOCAL_OBJ;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_BUF *NetIPv4_FragReasmListsHead;               // Ptr to head of frag reasm lists.
static NET_BUF *NetIPv4_FragReasmListsTail;               // Ptr to tail of frag reasm lists.

static CPU_INT32U NetIPv4_FragReasmTimeout_ms;            // IPv4 frag reasm timeout (in ms).

static CPU_INT16U NetIPv4_TxID_Ctr;                       // Global tx ID field ctr.

static MEM_DYN_POOL NetIPv4_AddrPool;

#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static MEM_DYN_POOL           NetIPv4_LinkLocalObjPool;
static SLIST_MEMBER           *NetIPv4_LinkLocalObjListPtr;
static NET_SVC_TASK_CHILD     *NetIPv4_LinkLocalSvcTaskChildPtr;
static NET_SVC_TASK_CHILD_CFG NetIPv4_LinkLocalSvcTaskChildCfg;
static NET_SVC_TASK_HOOKS     NetIPv4_LinkLocalSvcHooks;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------------- CFG FNCTS ---------------
static CPU_BOOLEAN NetIPv4_CfgAddrRemoveAllHandler(NET_IF_NBR if_nbr,
                                                   RTOS_ERR   *p_err);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIPv4_CfgAddrValidate(NET_IPv4_ADDR addr_host,
                                    NET_IPv4_ADDR addr_subnet_mask,
                                    NET_IPv4_ADDR addr_dflt_gateway,
                                    RTOS_ERR      *p_err);
#endif

//                                                                 -------------- GET FNCTS ---------------

static NET_IPv4_ADDR_OBJ *NetIPv4_GetAddrObjCfgd(NET_IF_NBR    *p_if_nbr,
                                                 NET_IPv4_ADDR addr);

//                                                                 -------- VALIDATE RX DATAGRAMS ---------
static void NetIPv4_RxPktValidate(NET_BUF      *p_buf,
                                  NET_BUF_HDR  *p_buf_hdr,
                                  NET_IPv4_HDR *p_ip_hdr,
                                  RTOS_ERR     *p_err);

static void NetIPv4_RxPktValidateOpt(NET_BUF      *p_buf,
                                     NET_BUF_HDR  *p_buf_hdr,
                                     NET_IPv4_HDR *p_ip_hdr,
                                     CPU_INT08U   ip_hdr_len_size,
                                     RTOS_ERR     *p_err);

static CPU_BOOLEAN NetIPv4_RxPktValidateOptRoute(NET_BUF_HDR *p_buf_hdr,
                                                 CPU_INT08U  *p_opts,
                                                 CPU_INT08U  opt_list_len_rem,
                                                 CPU_INT08U  *p_opt_len,
                                                 RTOS_ERR    *p_err);

static CPU_BOOLEAN NetIPv4_RxPktValidateOptTS(NET_BUF_HDR *p_buf_hdr,
                                              CPU_INT08U  *p_opts,
                                              CPU_INT08U  opt_list_len_rem,
                                              CPU_INT08U  *p_opt_len,
                                              RTOS_ERR    *p_err);

//                                                                 -------- REASM RX FRAGS --------

static NET_BUF *NetIPv4_RxPktFragReasm(NET_BUF      *p_buf,
                                       NET_BUF_HDR  *p_buf_hdr,
                                       NET_IPv4_HDR *p_ip_hdr,
                                       CPU_BOOLEAN  *p_is_frag,
                                       RTOS_ERR     *p_err);

static void NetIPv4_RxPktFragListAdd(NET_BUF     *p_buf,
                                     NET_BUF_HDR *p_buf_hdr,
                                     CPU_INT16U  frag_ip_flags,
                                     CPU_INT16U  frag_offset,
                                     CPU_INT16U  frag_size,
                                     RTOS_ERR    *p_err);

static NET_BUF *NetIPv4_RxPktFragListInsert(NET_BUF     *p_buf,
                                            NET_BUF_HDR *p_buf_hdr,
                                            CPU_INT16U  frag_ip_flags,
                                            CPU_INT16U  frag_offset,
                                            CPU_INT16U  frag_size,
                                            NET_BUF     *p_frag_list,
                                            RTOS_ERR    *p_err);

static void NetIPv4_RxPktFragListRemove(NET_BUF     *p_frag_list,
                                        CPU_BOOLEAN tmr_free);

static void NetIPv4_RxPktFragListDiscard(NET_BUF     *p_frag_list,
                                         CPU_BOOLEAN tmr_free,
                                         RTOS_ERR    *p_err);

static void NetIPv4_RxPktFragListUpdate(NET_BUF     *p_frag_list,
                                        NET_BUF_HDR *p_frag_list_buf_hdr,
                                        CPU_INT16U  frag_ip_flags,
                                        CPU_INT16U  frag_offset,
                                        CPU_INT16U  frag_size,
                                        RTOS_ERR    *p_err);

static NET_BUF *NetIPv4_RxPktFragListChkComplete(NET_BUF     *p_frag_list,
                                                 NET_BUF_HDR *p_frag_list_buf_hdr,
                                                 RTOS_ERR    *p_err);

static void NetIPv4_RxPktFragTimeout(void *p_frag_list_timeout);

//                                                                 ---------- DEMUX RX DATAGRAMS ----------

static void NetIPv4_RxPktDemuxDatagram(NET_BUF     *p_buf,
                                       NET_BUF_HDR *p_buf_hdr,
                                       RTOS_ERR    *p_err);

//                                                                 ------- VALIDATE TX PKTS -------
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIPv4_TxPktValidate(NET_BUF_HDR   *p_buf_hdr,
                                  NET_IPv4_ADDR addr_src,
                                  NET_IPv4_ADDR addr_dest,
                                  NET_IPv4_TOS  TOS,
                                  NET_IPv4_TTL  TTL,
                                  CPU_INT16U    flags,
                                  void          *p_opts,
                                  RTOS_ERR      *p_err);
#endif

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIPv4_TxPktValidateOpt(void     *p_opts,
                                     RTOS_ERR *p_err);

static void NetIPv4_TxPktValidateOptRouteTS(void       *p_opt_route_ts,
                                            CPU_INT08U *p_opt_len,
                                            void       **p_opt_next,
                                            RTOS_ERR   *p_err);
#endif

//                                                                 ------------- TX IPv4 PKTS -------------

static void NetIPv4_TxPkt(NET_BUF       *p_buf,
                          NET_BUF_HDR   *p_buf_hdr,
                          NET_IPv4_ADDR addr_src,
                          NET_IPv4_ADDR addr_dest,
                          NET_IPv4_TOS  TOS,
                          NET_IPv4_TTL  TTL,
                          CPU_INT16U    flags,
                          void          *p_opts,
                          RTOS_ERR      *p_err);

static CPU_INT08U NetIPv4_TxPktPrepareOpt(void       *p_opts,
                                          CPU_INT08U *p_opt_hdr,
                                          RTOS_ERR   *p_err);

static void NetIPv4_TxPktPrepareOptRoute(void       *p_opts,
                                         CPU_INT08U *p_opt_hdr,
                                         CPU_INT08U *p_opt_len,
                                         void       **p_opt_next);

static void NetIPv4_TxPktPrepareOptTS(void       *p_opts,
                                      CPU_INT08U *p_opt_hdr,
                                      CPU_INT08U *p_opt_len,
                                      void       **p_opt_next);

static void NetIPv4_TxPktPrepareOptTSRoute(void       *p_opts,
                                           CPU_INT08U *p_opt_hdr,
                                           CPU_INT08U *p_opt_len,
                                           void       **p_opt_next);

static void NetIPv4_TxPktPrepareHdr(NET_BUF       *p_buf,
                                    NET_BUF_HDR   *p_buf_hdr,
                                    CPU_INT16U    ip_hdr_len_tot,
                                    CPU_INT08U    ip_opt_len_tot,
                                    CPU_INT16U    protocol_ix,
                                    NET_IPv4_ADDR addr_src,
                                    NET_IPv4_ADDR addr_dest,
                                    NET_IPv4_TOS  TOS,
                                    NET_IPv4_TTL  TTL,
                                    CPU_INT16U    flags,
                                    CPU_INT32U    *p_ip_hdr_opts);

//                                                                 -------- TX IP DATAGRAMS -------

static void NetIPv4_TxPktDatagram(NET_BUF     *p_buf,
                                  NET_BUF_HDR *p_buf_hdr,
                                  RTOS_ERR    *p_err);

static NET_IP_TX_DEST_STATUS NetIPv4_TxPktDatagramRouteSel(NET_BUF_HDR *p_buf_hdr,
                                                           RTOS_ERR    *p_err);

//                                                                 ----------- RE-TX IPv4 PKTS ------------

static void NetIPv4_ReTxPkt(NET_BUF     *p_buf,
                            NET_BUF_HDR *p_buf_hdr,
                            RTOS_ERR    *p_err);

static void NetIPv4_ReTxPktPrepareHdr(NET_BUF     *p_buf,
                                      NET_BUF_HDR *p_buf_hdr);

static CPU_BOOLEAN NetIPv4_IsAddrHostHandler(NET_IPv4_ADDR addr);

static CPU_BOOLEAN NetIPv4_IsAddrHostCfgdHandler(NET_IPv4_ADDR addr);

//                                                                 --------- IPV4 LINK-LOCAL MODULE FUNCTIONS ---------
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static void NetIPv4_LinkSubscribeHook(NET_IF_NBR        if_nbr,
                                      NET_IF_LINK_STATE link_state);

static void NetIPv4_AddrLinkLocalHookOnMsg(NET_SVC_TASK_CHILD *p_child,
                                           CPU_INT32U         msg_type,
                                           void               *p_msg_arg,
                                           RTOS_ERR           *p_err);

static void NetIPv4_AddrLinkLocalHookOnTmrTimeout(NET_SVC_TASK_CHILD *p_child,
                                                  void               *p_obj,
                                                  RTOS_ERR           *p_err);

static NET_IPv4_LINK_LOCAL_STATUS NetIPv4_AddrLinkLocalCfgStart(NET_IPv4_LINK_LOCAL_OBJ *p_obj,
                                                                RTOS_ERR                *p_err);

static NET_IPv4_LINK_LOCAL_STATUS NetIPv4_AddrLinkLocalCfgReboot(NET_IPv4_LINK_LOCAL_OBJ *p_obj,
                                                                 RTOS_ERR                *p_err);

static void NetIPv4_AddrLinkLocalCfgStop(NET_IPv4_LINK_LOCAL_OBJ    *p_obj,
                                         NET_IPv4_LINK_LOCAL_STATUS status,
                                         RTOS_ERR                   *p_err);

static void NetIPv4_AddrLinkLocalCreate(NET_IPv4_LINK_LOCAL_OBJ *p_obj,
                                        RTOS_ERR                *p_err);

static void NetIPv4_AddrLinkLocalAdd(NET_IPv4_LINK_LOCAL_OBJ *p_obj,
                                     RTOS_ERR                *p_err);

static void NetIPv4_AddrLinkLocalAnnounce(NET_IPv4_LINK_LOCAL_OBJ *p_obj,
                                          RTOS_ERR                *p_err);

static NET_IPv4_ADDR NetIPv4_AddrLocalLinkGet(CPU_INT08U *p_addr_hw,
                                              CPU_INT08U addr_hw_len);

static CPU_BOOLEAN NetIPv4_CfgAddrAutoCfgStart(NET_IF_NBR if_nbr,
                                               RTOS_ERR   *p_err);

static CPU_BOOLEAN NetIPv4_CfgAddrAddAutoCfg(NET_IF_NBR    if_nbr,
                                             NET_IPv4_ADDR addr_host,
                                             RTOS_ERR      *p_err);

static CPU_BOOLEAN NetIPv4_CfgAddrLinkLocalStop(NET_IF_NBR if_nbr,
                                                RTOS_ERR   *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetIPv4_CfgAddrAdd()
 *
 * @brief    Add a statically-configured IPv4 host address, subnet mask, & default gateway to an interface.
 *
 * @param    if_nbr              Interface number to configure.
 *
 * @param    addr_host           Desired IPv4 address to add to this interface.
 *
 * @param    addr_subnet_mask    Desired IPv4 address subnet mask to configure.
 *
 * @param    addr_dflt_gateway   Desired IPv4 default gateway address to configure.
 *                               DEF_NULL or empty string if no gateway to configure.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_ALREADY_EXISTS
 *                                   - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_OK,   if valid IPv4 address, subnet mask, & default gateway configured.
 *           DEF_FAIL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) IPv4 addresses MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_CfgAddrAdd(NET_IF_NBR    if_nbr,
                               NET_IPv4_ADDR addr_host,
                               NET_IPv4_ADDR addr_subnet_mask,
                               NET_IPv4_ADDR addr_dflt_gateway,
                               RTOS_ERR      *p_err)
{
  CPU_BOOLEAN is_cfgd = DEF_NO;
  CPU_BOOLEAN result = DEF_FAIL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIPv4_CfgAddrAdd);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit_release;
  }

  //                                                               ---------------- VALIDATE IP ADDRS -----------------
  NetIPv4_CfgAddrValidate(addr_host, addr_subnet_mask, addr_dflt_gateway, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }
#endif

  result = NetIPv4_CfgAddrAddHandler(if_nbr, addr_host, addr_subnet_mask, addr_dflt_gateway, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  PP_UNUSED_PARAM(is_cfgd);

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (result);
}

/****************************************************************************************************//**
 *                                           NetIPv4_CfgAddrRemove()
 *
 * @brief    Remove a configured IPv4 host address, subnet mask, & default gateway from an interface
 *
 * @param    if_nbr      Interface number to remove address configuration.
 *
 * @param    addr_host   IPv4 address to remove.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_OK,   if IPv4 address configuration removed.
 *           DEF_FAIL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_CfgAddrRemove(NET_IF_NBR    if_nbr,
                                  NET_IPv4_ADDR addr_host,
                                  RTOS_ERR      *p_err)
{
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN addr_valid;
#endif
  NET_IF            *p_if;
  NET_IPv4_ADDR_OBJ *p_addr_obj;
  NET_IPv4_ADDR     addr_cfgd;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_cfgd;
#endif
  CPU_BOOLEAN found;
  CPU_BOOLEAN result;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIPv4_CfgAddrRemove);         // See Note #2b.

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ------------- VALIDATE IF NBR --------------
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit_release;
  }

  //                                                               ------------ VALIDATE IPv4 ADDR ------------
  addr_valid = NetIPv4_IsValidAddrHost(addr_host);
  if (addr_valid != DEF_YES) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.CfgInvAddrHostCtr);
    Net_GlobalLockRelease();
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);
  }
#endif

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  if (p_if->IP_Obj->IPv4_AddrListPtr == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit_release;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
    if (p_addr_obj->AddrHost == addr_host) {
      found = DEF_YES;
      break;
    }
  }

  if (found != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);                               // ... rtn err.
    goto exit_release;
  }

  //                                                               -------- CLOSE ALL IPv4 ADDR CONNS ---------
  //                                                               Close all cfg'd addr's conns.
  NET_UTIL_VAL_SET_NET_32(&addr_cfgd, p_addr_obj->AddrHost);
  NetConn_CloseAllConnsByAddrHandler((CPU_INT08U *)      &addr_cfgd,
                                     (NET_CONN_ADDR_LEN)sizeof(addr_cfgd));

  //                                                               ----- REMOVE ADDRESS FROM LIST AND FREE BLOCK ------
  SList_Rem(&p_if->IP_Obj->IPv4_AddrListPtr, &p_addr_obj->ListNode);
  Mem_DynPoolBlkFree(&NetIPv4_AddrPool, p_addr_obj, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_FAIL);

  result = DEF_OK;

  //                                                               ------------- RELEASE NET LOCK -------------
exit_release:
  Net_GlobalLockRelease();

  return (result);
}

/****************************************************************************************************//**
 *                                       NetIPv4_CfgAddrRemoveAll()
 *
 * @brief    Remove all configured IPv4 host address(s) from an interface.
 *
 * @param    if_nbr  Interface number to remove address configuration.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_OK,   if ALL interface's configured IP host address(s) successfully removed.
 *           DEF_FAIL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_CfgAddrRemoveAll(NET_IF_NBR if_nbr,
                                     RTOS_ERR   *p_err)
{
  CPU_BOOLEAN addr_remove = DEF_FAIL;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_cfgd = DEF_NO;
#endif

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Net_GlobalLockAcquire((void *)NetIPv4_CfgAddrRemoveAll);      // Acquire net lock (see Note #1b).

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);                   // Validate IF nbr.
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit_release;
  }
#endif
  //                                                               Remove all IF's cfg'd host addr(s).
  addr_remove = NetIPv4_CfgAddrRemoveAllHandler(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit_release:
  Net_GlobalLockRelease();                                      // Release net lock.

  return (addr_remove);
}

/****************************************************************************************************//**
 *                                       NetIPv4_AddrLinkLocalCfg()
 *
 * @brief    Start the IPv4 Link Local process on the given interface.
 *
 * @param    if_nbr  Interface number on which the IPv4 Link Local process will be started.
 *
 * @param    hook    Hook function to be notified when the process is completed.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_IS_OWNER
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @note     (1) An IPv4 link local address will only be configured if no other IPv4 addresses are
 *               configured on the interface.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
void NetIPv4_AddrLinkLocalCfg(NET_IF_NBR                        if_nbr,
                              NET_IPv4_LINK_LOCAL_COMPLETE_HOOK hook,
                              RTOS_ERR                          *p_err)
{
  NET_IF                  *p_if;
  NET_IPv4_LINK_LOCAL_OBJ *p_obj;
  NET_IF_LINK_STATE       link_state;
  CPU_BOOLEAN             found = DEF_NO;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  SLIST_FOR_EACH_ENTRY(NetIPv4_LinkLocalObjListPtr, p_obj, NET_IPv4_LINK_LOCAL_OBJ, ListNode) {
    if (p_obj->IF_Nbr == if_nbr) {
      found = DEF_YES;
      break;
    }
  }

  if (found == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    return;
  }

  p_obj = (NET_IPv4_LINK_LOCAL_OBJ *)Mem_DynPoolBlkGet(&NetIPv4_LinkLocalObjPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_obj->IF_Nbr = if_nbr;
  p_obj->State = NET_IPv4_LINK_LOCAL_STATE_INIT;
  p_obj->CompleteHook = hook;

  //                                                               ------ SEND MESSAGE TO START LINK LOCAK ON IF ------
  NetSvcTask_ChildMsgPost(NetIPv4_LinkLocalSvcTaskChildPtr,
                          NET_IPv4_LINK_LOCAL_MSG_TYPE_NEW_IF,
                          p_obj,
                          DEF_NO,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  link_state = NetIF_LinkStateGet(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  if (link_state == NET_IF_LINK_UP) {
    NetSvcTask_ChildMsgPost(NetIPv4_LinkLocalSvcTaskChildPtr,
                            NET_IPv4_LINK_LOCAL_MSG_TYPE_START_IF,
                            p_obj,
                            DEF_NO,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_release;
    }
  }

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_if->StartModulesCfgFlags.IPv4_LinkLocal = DEF_YES;

  return;

exit_release:
  {
    RTOS_ERR local_err;

    Mem_DynPoolBlkFree(&NetIPv4_LinkLocalObjPool, p_obj, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_AddrLinkLocalCfgRemove()
 *
 * @brief    Stop the IPv4 Link Local process on the given interface and remove the link local address
 *           if one have already been configured.
 *
 * @param    if_nbr  Interface number.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_IS_OWNER
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
void NetIPv4_AddrLinkLocalCfgRemove(NET_IF_NBR if_nbr,
                                    RTOS_ERR   *p_err)
{
  NET_IPv4_LINK_LOCAL_OBJ *p_obj = DEF_NULL;
  CPU_BOOLEAN             found = DEF_NO;

  SLIST_FOR_EACH_ENTRY(NetIPv4_LinkLocalObjListPtr, p_obj, NET_IPv4_LINK_LOCAL_OBJ, ListNode) {
    if (p_obj->IF_Nbr == if_nbr) {
      found = DEF_YES;
      break;
    }
  }

  if (found != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    return;
  }

  NetSvcTask_ChildMsgPost(NetIPv4_LinkLocalSvcTaskChildPtr,
                          NET_IPv4_LINK_LOCAL_MSG_TYPE_STOP_IF,
                          p_obj,
                          DEF_NO,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_CfgFragReasmTimeout()
 *
 * @brief    (1) Configure IPv4 fragment reassembly timeout.
 *               - (a) IPv4 fragment reassembly timeout is the maximum time allowed between received IPv4
 *                     fragments from the same IPv4 datagram.
 *
 * @param    timeout_sec     Desired value for IPv4 fragment reassembly timeout (in seconds).
 *
 * @return   DEF_OK,   IPv4 fragment reassembly timeout configured.
 *           DEF_FAIL, otherwise.
 *
 * @note     (2) Configured timeout does NOT reschedule any current IP fragment reassembly timeout in
 *               progress but becomes effective the next time IP fragments reassemble with timeout.
 *
 * @note     (3) [INTERNAL] 'NetIPv4_FragReasmTimeout_ms' variables MUST ALWAYS be accessed exclusively
 *               in critical sections.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_CfgFragReasmTimeout(CPU_INT08U timeout_sec)
{
  CORE_DECLARE_IRQ_STATE;

  if (timeout_sec < NET_IPv4_FRAG_REASM_TIMEOUT_MIN_SEC) {
    return (DEF_FAIL);
  }
  if (timeout_sec > NET_IPv4_FRAG_REASM_TIMEOUT_MAX_SEC) {
    return (DEF_FAIL);
  }

  CORE_ENTER_ATOMIC();
  NetIPv4_FragReasmTimeout_ms = timeout_sec * 1000;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           NetIPv4_GetAddrHost()
 *
 * @brief    Get an interface's IPv4 host address(s).
 *
 * @param    if_nbr          Interface number to get IPv4 host address(s).
 *
 * @param    p_addr_tbl      Pointer to IPv4 address table that will receive the IPv4 host address(s).
 *
 * @param    p_addr_tbl_qty  Pointer to a variable to pass the table size and will return the number of
 *                           address added to the table.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_OK,   if interface's IPv4 host address(s) successfully returned.
 *           DEF_FAIL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) IPv4 address(s) returned in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_GetAddrHost(NET_IF_NBR       if_nbr,
                                NET_IPv4_ADDR    *p_addr_tbl,
                                NET_IP_ADDRS_QTY *p_addr_tbl_qty,
                                RTOS_ERR         *p_err)
{
  CPU_BOOLEAN addr_avail = DEF_FAIL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Net_GlobalLockAcquire((void *)NetIPv4_GetAddrHost);           // Acquire net lock (see Note #1b).

  //                                                               Get all IF's host addr(s).
  addr_avail = NetIPv4_GetAddrHostHandler(if_nbr, p_addr_tbl, p_addr_tbl_qty, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  if (addr_avail == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit_release;
  }

exit_release:
  Net_GlobalLockRelease();                                      // Release net lock.

  return (addr_avail);
}

/****************************************************************************************************//**
 *                                           NetIPv4_GetAddrSrc()
 *
 * @brief    Get corresponding configured IPv4 host address for a remote IPv4 address (to use as source
 *           address).
 *
 * @param    addr_remote     Remote address to get configured IPv4 host address.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_INVALID_HANDLE
 *
 * @return   Configured IPv4 host address, if available.
 *           NET_IPv4_ADDR_NONE, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) IPv4 addresses MUST be in host-order.
 *******************************************************************************************************/
NET_IPv4_ADDR NetIPv4_GetAddrSrc(NET_IPv4_ADDR addr_remote,
                                 RTOS_ERR      *p_err)
{
  NET_IPv4_ADDR addr_host = NET_IPv4_ADDR_NONE;
  NET_IF_NBR    if_nbr = NET_IF_NBR_NONE;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               Acquire net lock (see Note #1b).
  Net_GlobalLockAcquire((void *)NetIPv4_GetAddrSrc);

  addr_host = NetIPv4_GetAddrSrcHandler(&if_nbr, addr_remote, p_err);    // Get cfg'd host addr.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  if (addr_host == NET_IPv4_ADDR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit_release;
  }

exit_release:
  Net_GlobalLockRelease();                                      // Release net lock.

  return (addr_host);
}

/****************************************************************************************************//**
 *                                       NetIPv4_GetAddrSubnetMask()
 *
 * @brief    Get the IPv4 address subnet mask for a configured IPv4 host address.
 *
 * @param    addr    Configured IPv4 host address to get the subnet mask.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *
 * @return   Configured IPv4 host address's subnet mask in host-order, if NO error(s).
 *           NET_IPv4_ADDR_NONE, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) IPv4 address returned in host-order.
 *******************************************************************************************************/
NET_IPv4_ADDR NetIPv4_GetAddrSubnetMask(NET_IPv4_ADDR addr,
                                        RTOS_ERR      *p_err)
{
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
  NET_IPv4_ADDR     addr_subnet = NET_IPv4_ADDR_NONE;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #1b.
  Net_GlobalLockAcquire((void *)NetIPv4_GetAddrSubnetMask);

  //                                                               ----------- GET IPv4 ADDR'S SUBNET MASK ------------
  p_addr_obj = NetIPv4_GetAddrObjCfgd(DEF_NULL, addr);
  if (p_addr_obj == DEF_NULL) {                                 // If addr NOT found, ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);                       // ... rtn err.
    goto exit_release;
  }

  addr_subnet = p_addr_obj->AddrHostSubnetMask;                 // Get IPv4 addr subnet mask.

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (addr_subnet);
}

/****************************************************************************************************//**
 *                                       NetIPv4_GetAddrDfltGateway()
 *
 * @brief    Get the default gateway IPv4 address for a configured IPv4 host address.
 *
 * @param    addr    Configured IPv4 host address to get the default gateway.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_FOUND
 *
 * @return   Configured IPv4 host address's default gateway in host-order, if NO error(s).
 *           NET_IPv4_ADDR_NONE, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) IPv4 address returned in host-order.
 *******************************************************************************************************/
NET_IPv4_ADDR NetIPv4_GetAddrDfltGateway(NET_IPv4_ADDR addr,
                                         RTOS_ERR      *p_err)
{
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
  NET_IPv4_ADDR     addr_dflt_gateway = NET_IPv4_ADDR_NONE;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  //                                                               See Note #1b.
  Net_GlobalLockAcquire((void *)NetIPv4_GetAddrDfltGateway);

  //                                                               ----------- GET IPv4 ADDR'S DFLT GATEWAY -----------
  p_addr_obj = NetIPv4_GetAddrObjCfgd(DEF_NULL, addr);
  if (p_addr_obj == DEF_NULL) {                                 // If addr NOT found, ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);                       // ... rtn err.
    goto exit_release;
  }

  addr_dflt_gateway = p_addr_obj->AddrDfltGateway;              // Get IPv4 addr dflt gateway.

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (addr_dflt_gateway);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrClassA()
 *
 * @brief    Validate an IPv4 address as a Class-A IPv4 address.
 *
 * @param    addr    IPv4 address to validate.
 *
 * @return   DEF_YES, if IPv4 address is a Class-A IPv4 address.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrClassA(NET_IPv4_ADDR addr)
{
  CPU_BOOLEAN addr_class_a;

  addr_class_a = ((addr & NET_IPv4_ADDR_CLASS_A_MASK) == NET_IPv4_ADDR_CLASS_A) ? DEF_YES : DEF_NO;

  return (addr_class_a);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrClassB()
 *
 * @brief    Validate an IPv4 address as a Class-B IPv4 address.
 *
 * @param    addr    IPv4 address to validate.
 *
 * @return   DEF_YES, if IPv4 address is a Class-B IPv4 address.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrClassB(NET_IPv4_ADDR addr)
{
  CPU_BOOLEAN addr_class_b;

  addr_class_b = ((addr & NET_IPv4_ADDR_CLASS_B_MASK) == NET_IPv4_ADDR_CLASS_B) ? DEF_YES : DEF_NO;

  return (addr_class_b);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrClassC()
 *
 * @brief    Validate an IPv4 address as a Class-C IPv4 address.
 *
 * @param    addr    IPv4 address to validate.
 *
 * @return   DEF_YES, if IPv4 address is a Class-C IPv4 address.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrClassC(NET_IPv4_ADDR addr)
{
  CPU_BOOLEAN addr_class_c;

  addr_class_c = ((addr & NET_IPv4_ADDR_CLASS_C_MASK) == NET_IPv4_ADDR_CLASS_C) ? DEF_YES : DEF_NO;

  return (addr_class_c);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrClassD()
 *
 * @brief    Validate an IPv4 address as a Class-D IPv4 address.
 *
 * @param    addr    IPv4 address to validate.
 *
 * @return   DEF_YES, if IPv4 address is a Class-D IPv4 address.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrClassD(NET_IPv4_ADDR addr)
{
  CPU_BOOLEAN addr_class_d;

  addr_class_d = ((addr & NET_IPv4_ADDR_CLASS_D_MASK) == NET_IPv4_ADDR_CLASS_D) ? DEF_YES : DEF_NO;

  return (addr_class_d);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrThisHost()
 *
 * @brief    Validate an IPv4 address as a 'This Host' initialization IPv4 address.
 *
 *       - (1) RFC #1122, Section 3.2.1.3.(a) specifies the IPv4 'This Host' initialization address
 *             as : 0.0.0.0
 *
 * @param    addr    IPv4 address to validate.
 *
 * @return   DEF_YES, if IPv4 address is a 'This Host' initialization IPv4 address.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrThisHost(NET_IPv4_ADDR addr)
{
  CPU_BOOLEAN addr_this_host;

  addr_this_host = (addr == NET_IPv4_ADDR_THIS_HOST) ? DEF_YES : DEF_NO;

  return (addr_this_host);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrLocalHost()
 *
 * @brief    Validate an IPv4 address as a 'Localhost' IPv4 address.
 *
 *       - (1) RFC #1122, Section 3.2.1.3.(g) specifies the IPv4 'Localhost' address as :
 *             127.<host>
 *
 * @param    addr    IPv4 address to validate.
 *
 * @return   DEF_YES, if IPv4 address is a 'Localhost' IPv4 address.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrLocalHost(NET_IPv4_ADDR addr)
{
  CPU_BOOLEAN addr_local_host;

  addr_local_host = ((addr >= NET_IPv4_ADDR_LOCAL_HOST_MIN)
                     && (addr <= NET_IPv4_ADDR_LOCAL_HOST_MAX)) ? DEF_YES : DEF_NO;

  return (addr_local_host);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrLocalLink()
 *
 * @brief    Validate an IPv4 address as a link-local IPv4 address.
 *           - (1) RFC #3927, Section 2.1 specifies the "IPv4 Link-Local address ... range ... [as]
 *                 inclusive" ... :
 *               - (a) "from 169.254.1.0" ...
 *               - (b) "to   169.254.254.255".
 *
 * @param    addr    IPv4 address to validate.
 *
 *                   (a) "from 169.254.1.0" ...
 *                   (b) "to   169.254.254.255".
 *
 *
 * Argument(s) : addr        IPv4 address to validate.
 *
 * Return(s)   : DEF_YES, if IPv4 address is a link-local IPv4 address.
 *               DEF_NO,  otherwise.
 *
 * Note(s)     : (1) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrLocalLink(NET_IPv4_ADDR addr)
{
  CPU_BOOLEAN addr_local_link;

  addr_local_link = ((addr >= NET_IPv4_ADDR_LOCAL_LINK_HOST_MIN)
                     && (addr <= NET_IPv4_ADDR_LOCAL_LINK_HOST_MAX)) ? DEF_YES : DEF_NO;

  return (addr_local_link);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrBroadcast()
 *
 * @brief    Validate an IPv4 address as a limited broadcast IPv4 address.
 *
 *       - (1) RFC #1122, Section 3.2.1.3.(c) specifies the IPv4 limited broadcast address as :
 *             255.255.255.255
 *
 * @param    addr    IPv4 address to validate.
 *
 *
 * Argument(s) : addr        IPv4 address to validate.
 *
 * Return(s)   : DEF_YES, if IPv4 address is a limited broadcast IP address.
 *               DEF_NO,  otherwise.
 *
 * Note(s)     : (1) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrBroadcast(NET_IPv4_ADDR addr)
{
  CPU_BOOLEAN addr_broadcast;

  addr_broadcast = (addr == NET_IPv4_ADDR_BROADCAST) ? DEF_YES : DEF_NO;

  return (addr_broadcast);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrMulticast()
 *
 * @brief    Validate an IPv4 address as a multicast IP address.
 *
 * @param    addr    IPv4 address to validate.
 *
 * @return   DEF_YES, if IPv4 address is a multicast IP address.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrMulticast(NET_IPv4_ADDR addr)
{
  CPU_BOOLEAN addr_multicast;

  addr_multicast = NetIPv4_IsAddrClassD(addr);

  return (addr_multicast);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrHost()
 *
 * @brief    (1) Validate an IPv4 address as an IPv4 host address :
 *               - (a) A configured  IPv4 host address (on an enabled interface)
 *               - (b) A 'Localhost' IPv4 address
 *
 * @param    addr    IPv4 address to validate.
 *
 * @return   DEF_YES, if IPv4 address is one of the host's IPv4 addresses.
 *           DEF_NO,  otherwise.
 *
 * @internal
 * @note     (2) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (3) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrHost(NET_IPv4_ADDR addr)
{
  CPU_BOOLEAN addr_host;

  Net_GlobalLockAcquire((void *)NetIPv4_IsAddrHost);            // Acquire net lock (see Note #2b).

  addr_host = NetIPv4_IsAddrHostHandler(addr);                  // Chk if any host addr.

  Net_GlobalLockRelease();                                      // Release net lock.

  return (addr_host);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrHostCfgd()
 *
 * @brief    Validate an IPv4 address as a configured IPv4 host address on an enabled interface.
 *
 * @param    addr    IPv4 address to validate.
 *
 * @return   DEF_YES, if IPv4 address is one of the host's configured IPv4 addresses.
 *           DEF_NO,  otherwise.
 *
 * @internal
 * @note     (1)  [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrHostCfgd(NET_IPv4_ADDR addr)
{
  CPU_BOOLEAN addr_host = DEF_FAIL;

  //                                                               Acquire net lock (see Note #1b).
  Net_GlobalLockAcquire((void *)NetIPv4_IsAddrHostCfgd);

  addr_host = NetIPv4_IsAddrHostCfgdHandler(addr);              // Chk if any cfg'd host addr.

  Net_GlobalLockRelease();                                      // Release net lock.

  return (addr_host);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrsCfgdOnIF()
 *
 * @brief    Check if any IPv4 host address(s) configured on an interface.
 *
 * @param    if_nbr  Interface number to check for configured IPv4 host address(s).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_YES, if any IP host address(s) configured on interface.
 *           DEF_NO,  otherwise.
 *
 * @internal
 * @note     (1)  [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrsCfgdOnIF(NET_IF_NBR if_nbr,
                                    RTOS_ERR   *p_err)
{
  CPU_BOOLEAN addr_avail = DEF_NO;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               Acquire net lock (see Note #1b).
  Net_GlobalLockAcquire((void *)NetIPv4_IsAddrsCfgdOnIF);

  addr_avail = NetIPv4_IsAddrsCfgdOnIF_Handler(if_nbr, p_err);  // Chk IF for any cfg'd host addr(s).

  Net_GlobalLockRelease();                                      // Release net lock.

  return (addr_avail);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsValidAddrHost()
 *
 * @brief    (1) Validate an IPv4 host address :
 *               - (a) MUST NOT be one of the following :
 *                   - (1) This      Host                              RFC #1122, Section 3.2.1.3.(a)
 *                   - (2) Specified Host                              RFC #1122, Section 3.2.1.3.(b)
 *                   - (3) Limited   Broadcast                         RFC #1122, Section 3.2.1.3.(c)
 *                   - (4) Directed  Broadcast                         RFC #1122, Section 3.2.1.3.(d)
 *                   - (5) Localhost                                   RFC #1122, Section 3.2.1.3.(g)
 *                   - (6) Multicast host address                      RFC #1112, Section 7.2
 *               - (b) RFC #3927, Section 2.1 specifies the "IPv4 Link-Local address" :
 *                   - (1) "Range ... inclusive"  ...
 *                       - (a) "from 169.254.1.0" ...
 *                       - (b) "to   169.254.254.255".
 *           - (2) ONLY validates typical IPv4 host addresses, since 'This Host' & 'Specified Host' IPv4
 *                 host addresses are ONLY valid during a host's initialization.
 *                 This function CANNOT be used to validate any 'This Host' or 'Specified Host' host
 *                 addresses.
 *
 * @param    addr_host   IPv4 host address to validate (see Note #4).
 *
 * @return   DEF_YES, if IPv4 host address valid.
 *           DEF_NO,  otherwise.
 *
 * @note     (3) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsValidAddrHost(NET_IPv4_ADDR addr_host)
{
  CPU_BOOLEAN valid;

  valid = DEF_YES;
  //                                                               ---------------- VALIDATE HOST ADDR ----------------
  //                                                               Chk invalid 'This Host'         (see Note #1a1).
  if (addr_host == NET_IPv4_ADDR_THIS_HOST) {
    valid = DEF_NO;

    //                                                             Chk invalid lim'd broadcast     (see Note #1a3).
  } else if (addr_host == NET_IPv4_ADDR_BROADCAST) {
    valid = DEF_NO;

    //                                                             Chk invalid localhost           (see Note #1a5).
  } else if ((addr_host & NET_IPv4_ADDR_LOCAL_HOST_MASK_NET)
             == NET_IPv4_ADDR_LOCAL_HOST_NET     ) {
    valid = DEF_NO;

    //                                                             Chk         link-local addrs    (see Note #1b1).
  } else if ((addr_host & NET_IPv4_ADDR_LOCAL_LINK_MASK_NET)
             == NET_IPv4_ADDR_LOCAL_LINK_NET     ) {
    //                                                             Chk invalid link-local addr     (see Note #1b1A).
    if ((addr_host < NET_IPv4_ADDR_LOCAL_LINK_HOST_MIN)
        || (addr_host > NET_IPv4_ADDR_LOCAL_LINK_HOST_MAX)) {
      valid = DEF_NO;
    }
  } else if ((addr_host & NET_IPv4_ADDR_CLASS_A_MASK) == NET_IPv4_ADDR_CLASS_A) {
    //                                                             Chk invalid Class-A 'This Host' (see Note #1a2).
    if ((addr_host               & NET_IPv4_ADDR_CLASS_A_MASK_HOST)
        == (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_CLASS_A_MASK_HOST)) {
      valid = DEF_NO;
    }
    //                                                             Chk invalid Class-A broadcast   (see Note #1a4).
    if ((addr_host               & NET_IPv4_ADDR_CLASS_A_MASK_HOST)
        == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_CLASS_A_MASK_HOST)) {
      valid = DEF_NO;
    }
  } else if ((addr_host & NET_IPv4_ADDR_CLASS_B_MASK) == NET_IPv4_ADDR_CLASS_B) {
    //                                                             Chk invalid Class-B 'This Host' (see Note #1a2).
    if ((addr_host               & NET_IPv4_ADDR_CLASS_B_MASK_HOST)
        == (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_CLASS_B_MASK_HOST)) {
      valid = DEF_NO;
    }
    //                                                             Chk invalid Class-B broadcast   (see Note #1a4).
    if ((addr_host               & NET_IPv4_ADDR_CLASS_B_MASK_HOST)
        == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_CLASS_B_MASK_HOST)) {
      valid = DEF_NO;
    }
  } else if ((addr_host & NET_IPv4_ADDR_CLASS_C_MASK) == NET_IPv4_ADDR_CLASS_C) {
    //                                                             Chk invalid Class-C 'This Host' (see Note #1a2).
    if ((addr_host               & NET_IPv4_ADDR_CLASS_C_MASK_HOST)
        == (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_CLASS_C_MASK_HOST)) {
      valid = DEF_NO;
    }
    //                                                             Chk invalid Class-C broadcast   (see Note #1a4).
    if ((addr_host               & NET_IPv4_ADDR_CLASS_C_MASK_HOST)
        == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_CLASS_C_MASK_HOST)) {
      valid = DEF_NO;
    }
  } else if ((addr_host & NET_IPv4_ADDR_CLASS_D_MASK) == NET_IPv4_ADDR_CLASS_D) {
    //                                                             Chk invalid Class-D multicast   (see Note #1a6).
    valid = DEF_NO;
  } else {                                                      // Invalid addr class (see Note #3).
    valid = DEF_NO;
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                       NetIPv4_IsValidAddrHostCfgd()
 *
 * @brief    - (1) Validate an IPv4 address for a configured IPv4 host address :
 *               - (a) MUST NOT be one of the following :
 *                   - (1) This      Host                              RFC #1122, Section 3.2.1.3.(a)
 *                   - (2) Specified Host                              RFC #1122, Section 3.2.1.3.(b)
 *                   - (3) Limited   Broadcast                         RFC #1122, Section 3.2.1.3.(c)
 *                   - (4) Directed  Broadcast                         RFC #1122, Section 3.2.1.3.(d)
 *                   - (5) Subnet    Broadcast                         RFC #1122, Section 3.2.1.3.(e)
 *                   - (6) Localhost                                   RFC #1122, Section 3.2.1.3.(g)
 *                   - (7) Multicast host address                      RFC #1112, Section 7.2
 *           - (2) ONLY validates this host's IPv4 address, since 'This Host' & 'Specified Host' IPv4 host
 *                 addresses are ONLY valid during a host's initialization (see Notes #1a1 & #1a4).  This
 *                 function CANNOT be used to validate any 'This Host' or 'Specified Host' host addresses.
 *
 * @param    addr_host           IPv4 host address to validate.
 *
 * @param    addr_subnet_mask    IPv4      address subnet mask.
 *
 * @return   DEF_YES, if this host's IPv4 address valid.
 *           DEF_NO,  otherwise.
 *
 * @note     (3) IPv4 addresses MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsValidAddrHostCfgd(NET_IPv4_ADDR addr_host,
                                        NET_IPv4_ADDR addr_subnet_mask)
{
  CPU_BOOLEAN valid_host;
  CPU_BOOLEAN valid_mask;
  CPU_BOOLEAN valid;

  if ((addr_host == NET_IPv4_ADDR_NONE)                             // Chk invalid NULL addr(s).
      || (addr_subnet_mask == NET_IPv4_ADDR_NONE)) {
    return (DEF_NO);
  }
  //                                                               Chk invalid subnet 'This Host' (see Note #1a2).
  if ((addr_host               & ~addr_subnet_mask)
      == (NET_IPv4_ADDR_THIS_HOST & ~addr_subnet_mask)) {
    return (DEF_NO);
  }
  //                                                               Chk invalid subnet broadcast   (see Note #1a5).
  if ((addr_host               & ~addr_subnet_mask)
      == (NET_IPv4_ADDR_BROADCAST & ~addr_subnet_mask)) {
    return (DEF_NO);
  }

  valid_host = NetIPv4_IsValidAddrHost(addr_host);
  valid_mask = NetIPv4_IsValidAddrSubnetMask(addr_subnet_mask);

  valid = ((valid_host == DEF_YES)
           && (valid_mask == DEF_YES)) ? DEF_YES : DEF_NO;

  return (valid);
}

/****************************************************************************************************//**
 *                                       NetIPv4_IsValidAddrSubnetMask()
 *
 * @brief    (1) Validate an IPv4 address subnet mask :
 *               - (a) RFC #1122, Section 3.2.1.3 states that :
 *                   - (1) "IP addresses are not permitted to have the value 0 or -1 for any of the ...
 *                           <Subnet-number> fields" ...
 *                   - (2) "This implies that each of these fields will be at least two bits long."
 *               - (b) RFC #950, Section 2.1 'Special Addresses' reiterates that "the values of all
 *                     zeros and all ones in the subnet field should not be assigned to actual
 *                     (physical) subnets".
 *               - (c) RFC #950, Section 2.1 also states that "the bits that identify the subnet ...
 *                     need not be adjacent in the address.  However, we recommend that the subnet
 *                     bits be contiguous and located as the most significant bits of the local
 *                     address".
 *                     #### Therefore, it is assumed that at least the most significant bit of the
 *                     network portion of the subnet address SHOULD be set.
 *
 * @param    addr_subnet_mask    IPv4 address subnet mask to validate.
 *
 * @return   DEF_YES, if IPv4 address subnet mask valid.
 *           DEF_NO,  otherwise.
 *
 * @note     (2) IPv4 addresses MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsValidAddrSubnetMask(NET_IPv4_ADDR addr_subnet_mask)
{
  CPU_BOOLEAN   valid;
  NET_IPv4_ADDR mask;
  CPU_INT08U    mask_size;
  CPU_INT08U    mask_nbr_one_bits;
  CPU_INT08U    mask_nbr_one_bits_min;
  CPU_INT08U    mask_nbr_one_bits_max;
  CPU_INT08U    i;

  //                                                               ------------- VALIDATE SUBNET MASK -------------
  //                                                               Chk invalid subnet class (see Note #1c).
  if ((addr_subnet_mask & NET_IPv4_ADDR_CLASS_SUBNET_MASK_MIN) == NET_IPv4_ADDR_NONE) {
    valid = DEF_NO;
  } else {                                                          // Chk invalid subnet mask (see Notes #1a & #1b).
    mask_size = sizeof(addr_subnet_mask) * DEF_OCTET_NBR_BITS;
    mask = DEF_BIT_00;
    mask_nbr_one_bits = 0u;
    for (i = 0u; i < mask_size; i++) {                              // Calc nbr subnet bits.
      if (addr_subnet_mask & mask) {
        mask_nbr_one_bits++;
      }
      mask <<= 1u;
    }

    mask_nbr_one_bits_min = 2u;                                     // See Note #1a2.
    mask_nbr_one_bits_max = mask_size - mask_nbr_one_bits_min;
    //                                                             Chk invalid nbr subnet bits (see Note #1a2).
    if (mask_nbr_one_bits < mask_nbr_one_bits_min) {
      valid = DEF_NO;
    } else if (mask_nbr_one_bits > mask_nbr_one_bits_max) {
      valid = DEF_NO;
    } else {
      valid = DEF_YES;
    }
  }

  return (valid);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIPv4_Init()
 *
 * @brief    (1) Initialize Internet Protocol Layer :
 *               - (a) Initialize ALL interfaces' configurable IPv4 addresses
 *               - (b) Initialize IPv4 fragmentation list pointers
 *               - (c) Initialize IPv4 identification (ID) counter
 *
 * @note     (2) Default IPv4 address initialization is invalid & forces the developer or higher-layer
 *               protocol application to configure valid IPv4 address(s).
 *
 * @note     (3) Address configuration state initialized to 'static' by default.
 *******************************************************************************************************/
void NetIPv4_Init(MEM_SEG  *p_mem_seg,
                  RTOS_ERR *p_err)
{
  //                                                               ------------- CREATE IPV4 ADDRESS POOL -------------
  Mem_DynPoolCreate("IPv4 Address Pool",
                    &NetIPv4_AddrPool,
                    p_mem_seg,
                    sizeof(NET_IPv4_ADDR_OBJ),
                    sizeof(CPU_ALIGN),
                    1,
                    NET_IPv4_CFG_IF_MAX_NBR_ADDR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------- INIT IPv4 FRAG LISTS -------------
  NetIPv4_FragReasmListsHead = DEF_NULL;
  NetIPv4_FragReasmListsTail = DEF_NULL;

  //                                                               --------------- INIT IPv4 ID CTR ---------------
  NetIPv4_TxID_Ctr = NET_IPv4_ID_INIT;

#ifdef  NET_IGMP_MODULE_EN
  NetIGMP_Init(p_mem_seg, p_err);
#endif

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv4_AddrLinkLocalInit()
 *
 * @brief    Initialize the IPv4 link local module.
 *
 * @param    p_mem_seg   Pointer to memory segment to use for dynamic allocation.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
void NetIPv4_AddrLinkLocalInit(MEM_SEG  *p_mem_seg,
                               RTOS_ERR *p_err)
{
  NET_SVC_TASK_CHILD_CFG *p_child_cfg;

  Mem_DynPoolCreate("IPv4 Link Local Object Pool",
                    &NetIPv4_LinkLocalObjPool,
                    p_mem_seg,
                    sizeof(NET_IPv4_LINK_LOCAL_OBJ),
                    sizeof(CPU_ALIGN),
                    NET_IF_CFG_MAX_NBR_IF,
                    NET_IF_CFG_MAX_NBR_IF,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------ INITIALIZE IPv4 LINK LOCAL OBJECT LIST ------
  SList_Init(&NetIPv4_LinkLocalObjListPtr);

  //                                                               -- CREATE SERVICE TASK CHILD FOR IPv4 LINK LOCAL ---
  p_child_cfg = &NetIPv4_LinkLocalSvcTaskChildCfg;
  p_child_cfg->MemSegPtr = Net_CoreDataPtr->CoreMemSegPtr;
  p_child_cfg->TmrNbrMax = NET_IF_CFG_MAX_NBR_IF * 2u;
  p_child_cfg->SockNbrMax = 0u;
  p_child_cfg->MsgNbrMax = NET_IF_CFG_MAX_NBR_IF * 5u;

  NetIPv4_LinkLocalSvcHooks.OnStart = DEF_NULL;
  NetIPv4_LinkLocalSvcHooks.OnStop = DEF_NULL;
  NetIPv4_LinkLocalSvcHooks.OnMsg = NetIPv4_AddrLinkLocalHookOnMsg;
  NetIPv4_LinkLocalSvcHooks.OnErr = DEF_NULL;

  NetIPv4_LinkLocalSvcTaskChildPtr = NetSvcTask_ChildStart(Net_CoreDataPtr->SvcTaskHandle,
                                                           p_child_cfg,
                                                           DEF_NULL,
                                                           &NetIPv4_LinkLocalSvcHooks,
                                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_CfgAddrAddDynamic()
 *
 * @brief    Add a dynamically-configured IPv4 host address, subnet mask, & default gateway to
 *           an interface.
 *
 * @param    if_nbr              Interface number to configure.
 *
 * @param    addr_host           Desired IPv4 address to add to this interface.
 *
 * @param    addr_subnet_mask    Desired IPv4 address subnet mask to configure.
 *
 * @param    addr_dflt_gateway   Desired IPv4 default gateway address to configure.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function :
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_ALREADY_EXISTS
 *                                   - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_OK,   if valid IPv4 address, subnet mask, & default gateway configured.
 *           DEF_FAIL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) Application calls to dynamic address configuration functions MUST be sequenced as follows :
 *               - (a) NetIPv4_CfgAddrAddDynamicStart() MUST precede NetIPv4_CfgAddrAddDynamic().
 *
 * @note     (3) IPv4 addresses MUST be in host-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_CfgAddrAddDynamic(NET_IF_NBR    if_nbr,
                                      NET_IPv4_ADDR addr_host,
                                      NET_IPv4_ADDR addr_subnet_mask,
                                      NET_IPv4_ADDR addr_dflt_gateway,
                                      RTOS_ERR      *p_err)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
  CPU_BOOLEAN       addr_cfgd = DEF_NO;
  CPU_BOOLEAN       found = DEF_NO;
  CPU_BOOLEAN       result = DEF_FAIL;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_en = DEF_NO;
#endif

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIPv4_CfgAddrAddDynamic);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_en = NetIF_IsEnCfgdHandler(if_nbr, p_err);
  if (is_en != DEF_YES) {
    goto exit_release;
  }

  //                                                               --------------- VALIDATE IPv4 ADDRS ----------------
  NetIPv4_CfgAddrValidate(addr_host, addr_subnet_mask, addr_dflt_gateway, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }
#endif

  //                                                               Validate host addr is not already configured.
  addr_cfgd = NetIPv4_IsAddrHostCfgdHandler(addr_host);
  if (addr_cfgd != DEF_NO) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.CfgInvAddrInUseCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    goto exit;
  }

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
    if (p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_DYN_INIT) {
      p_addr_obj->CfgMode = NET_IP_ADDR_CFG_MODE_DYN;
      p_addr_obj->AddrHost = addr_host;
      p_addr_obj->AddrHostSubnetMask = addr_subnet_mask;
      p_addr_obj->AddrHostSubnetMaskHost = ~addr_subnet_mask;
      p_addr_obj->AddrHostSubnetNet = addr_host & addr_subnet_mask;
      p_addr_obj->AddrDfltGateway = addr_dflt_gateway;
      p_addr_obj->IsValid = DEF_YES;
      found = DEF_YES;
      break;
    }
  }

  if (found != DEF_YES) {
    goto exit_release;
  }

  result = DEF_OK;

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
exit:
  return (result);
}

/****************************************************************************************************//**
 *                                       NetIPv4_CfgAddrAddDynamicStart()
 *
 * @brief    Start the dynamic address configuration for an interface.
 *
 * @param    if_nbr  Interface number to start dynamic address configuration.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_OK,   if dynamic configuration successfully started.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) Application calls to dynamic address configuration functions MUST be sequenced as follows:
 *               - (a) NetIPv4_CfgAddrAddDynamicStart() MUST precede NetIPv4_CfgAddrAddDynamic()
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_CfgAddrAddDynamicStart(NET_IF_NBR if_nbr,
                                           RTOS_ERR   *p_err)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
  CPU_BOOLEAN       found = DEF_NO;
  CPU_BOOLEAN       result = DEF_FAIL;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_en = DEF_NO;
#endif

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------- ACQUIRE NET LOCK -------------
  Net_GlobalLockAcquire((void *)NetIPv4_CfgAddrAddDynamicStart);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ------------- VALIDATE IF NBR --------------
  is_en = NetIF_IsEnCfgdHandler(if_nbr, p_err);
  if (is_en != DEF_YES) {
    goto exit_release;
  }
#endif

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
    if ((p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_DYN)
        || (p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_DYN_INIT)) {
      found = DEF_YES;
      break;
    }
  }

  if (found != DEF_YES) {
    //                                                             ------------- RETRIEVE ADDR MEM BLOCK --------------
    p_addr_obj = (NET_IPv4_ADDR_OBJ *)Mem_DynPoolBlkGet(&NetIPv4_AddrPool, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_release;
    }

    //                                                             ---------- ADD ADDRESS TO IPV4 ADDR LIST -----------
    SList_Push(&p_if->IP_Obj->IPv4_AddrListPtr, &p_addr_obj->ListNode);
  }

  p_addr_obj->CfgMode = NET_IP_ADDR_CFG_MODE_DYN_INIT;
  p_addr_obj->AddrHost = NET_IPv4_ADDR_NONE;
  p_addr_obj->AddrHostSubnetMask = NET_IPv4_ADDR_NONE;
  p_addr_obj->AddrHostSubnetMaskHost = NET_IPv4_ADDR_NONE;
  p_addr_obj->AddrHostSubnetNet = NET_IPv4_ADDR_NONE;
  p_addr_obj->AddrDfltGateway = NET_IPv4_ADDR_NONE;
  p_addr_obj->IsValid = DEF_NO;

  result = DEF_OK;

exit_release:
  //                                                               ------------- RELEASE NET LOCK -------------
  Net_GlobalLockRelease();

  return (result);
}

/****************************************************************************************************//**
 *                                       NetIPv4_CfgAddrAddDynamicStop()
 *
 * @brief    Stop the dynamic address configuration for an interface.
 *
 * @param    if_nbr  Interface number to stop dynamic address configuration.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_POOL_FULL
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_OK,   if dynamic configuration successfully stopped.
 *           DEF_FAIL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 * @endinternal
 *
 * @note     (2) Application calls to dynamic address configuration functions MUST be sequenced as follows :
 *
 *           - (a) NetIPv4_CfgAddrAddDynamicStop() MUST follow NetIPv4_CfgAddrAddDynamicStart() --
 *                   if & ONLY if dynamic address initialization fails.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_CfgAddrAddDynamicStop(NET_IF_NBR if_nbr,
                                          RTOS_ERR   *p_err)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_cfgd = DEF_NO;
#endif
  CPU_BOOLEAN found = DEF_NO;
  CPU_BOOLEAN result = DEF_FAIL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------- ACQUIRE NET LOCK -------------
  //                                                               See Note #2b.
  Net_GlobalLockAcquire((void *)NetIPv4_CfgAddrAddDynamicStop);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ------------- VALIDATE IF NBR --------------
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit_release;
  }
#endif

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
    if ((p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_DYN)
        || (p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_DYN_INIT)) {
      found = DEF_YES;
      break;
    }
  }

  if (found == DEF_YES) {
    if (p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_DYN) {
      NET_IPv4_ADDR addr_cfgd;

      //                                                           -------- CLOSE ALL IPv4 ADDR CONNS ---------
      //                                                           Close all cfg'd addr's conns.
      NET_UTIL_VAL_SET_NET_32(&addr_cfgd, p_addr_obj->AddrHost);
      NetConn_CloseAllConnsByAddrHandler((CPU_INT08U *)&addr_cfgd,
                                         sizeof(addr_cfgd));
    }

    SList_Rem(&p_if->IP_Obj->IPv4_AddrListPtr, &p_addr_obj->ListNode);
    Mem_DynPoolBlkFree(&NetIPv4_AddrPool, p_addr_obj, p_err);
    Net_GlobalLockRelease();
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_FAIL);
    result = DEF_OK;
    goto exit;
  }

  result = DEF_OK;

exit_release:
  //                                                               ------------- RELEASE NET LOCK -------------
  Net_GlobalLockRelease();

exit:
  return (result);
}

/****************************************************************************************************//**
 *                                       NetIPv4_CfgAddrAddHandler()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    if_nbr              $$$$ Add description for 'if_nbr'
 *
 * @param    addr_host           $$$$ Add description for 'addr_host'
 *
 * @param    addr_subnet_mask    $$$$ Add description for 'addr_subnet_mask'
 *
 * @param    addr_dflt_gateway   $$$$ Add description for 'addr_dflt_gateway'
 *
 * @param    p_err               $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_CfgAddrAddHandler(NET_IF_NBR    if_nbr,
                                      NET_IPv4_ADDR addr_host,
                                      NET_IPv4_ADDR addr_subnet_mask,
                                      NET_IPv4_ADDR addr_dflt_gateway,
                                      RTOS_ERR      *p_err)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
  CPU_BOOLEAN       is_cfgd = DEF_NO;
  CPU_BOOLEAN       result = DEF_FAIL;

  //                                                               Validate host addr not already configured.
  is_cfgd = NetIPv4_IsAddrHostCfgdHandler(addr_host);
  if (is_cfgd == DEF_YES) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.CfgInvAddrInUseCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    return (DEF_FAIL);
  }

  p_if = NetIF_Get(if_nbr, p_err);                              // Retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  //                                                               -------------- RETRIVE ADDR MEM BLOCK --------------
  p_addr_obj = (NET_IPv4_ADDR_OBJ *)Mem_DynPoolBlkGet(&NetIPv4_AddrPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  //                                                               ------------ INITIALIZE ADDRESS FIELDS -------------
  p_addr_obj->CfgMode = NET_IP_ADDR_CFG_MODE_STATIC;
  p_addr_obj->AddrHost = addr_host;
  p_addr_obj->AddrHostSubnetMask = addr_subnet_mask;
  p_addr_obj->AddrHostSubnetMaskHost = ~addr_subnet_mask;
  p_addr_obj->AddrHostSubnetNet = addr_host & addr_subnet_mask;
  p_addr_obj->AddrDfltGateway = addr_dflt_gateway;
  p_addr_obj->IsValid = DEF_YES;

  //                                                               ---------- ADD ADDRESS TO IPV4 ADDR LIST -----------
  SList_PushBack(&p_if->IP_Obj->IPv4_AddrListPtr, &p_addr_obj->ListNode);

  result = DEF_OK;

  return (result);
}

/****************************************************************************************************//**
 *                                       NetIPv4_GetAddrHostHandler()
 *
 * @brief    Get an interface's IPv4 host address(s) [see Note #2].
 *
 * @param    if_nbr          Interface number to get IPv4 host address(s).
 *
 * @param    p_addr_tbl      Pointer to IPv4 address table that will receive the IPv4 host address(s)
 *                           in host-order for this interface.
 *
 * @param    p_addr_tbl_qty  Pointer to a variable to ... :
 *                               - (a) Pass the size of the address table, in number of IPv4 addresses,
 *                                     pointed to by 'p_addr_tbl'.
 *                               - (b) Return the actual number of IPv4 addresses, if NO error(s);
 *                               - (c) Return 0, otherwise.
 *                                     See also Note #3.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if interface's IPv4 host address(s) successfully returned.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) NetIPv4_GetAddrHostHandler() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *
 *               See also 'NetIPv4_GetAddrHost()  Note #1'.
 *
 * @note     (2) IPv4 address(s) returned in host-order.
 *
 * @note     (3) Since 'p_addr_tbl_qty' argument is both an input & output argument
 *               (see 'Argument(s) : p_addr_tbl_qty'), ... :
 *               - (a) Its input value SHOULD be validated prior to use; ...
 *                   - (1) In the case that the 'p_addr_tbl_qty' argument is passed a null pointer,
 *                         NO input value is validated or used.
 *                   - (2) The number of IP addresses of the table that will receive the configured
 *                         IP address(s) MUST be greater than or equal to NET_IPv4_CFG_IF_MAX_NBR_ADDR.
 *               - (b) While its output value MUST be initially configured to return a default value
 *                     PRIOR to all other validation or function handling in case of any error(s).
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_GetAddrHostHandler(NET_IF_NBR       if_nbr,
                                       NET_IPv4_ADDR    *p_addr_tbl,
                                       NET_IP_ADDRS_QTY *p_addr_tbl_qty,
                                       RTOS_ERR         *p_err)
{
  NET_IF            *p_if;
  NET_IPv4_ADDR_OBJ *p_addr_obj;
  NET_IPv4_ADDR     *p_addr;
  NET_IP_ADDRS_QTY  addr_tbl_qty;
  NET_IP_ADDRS_QTY  addr_ix = 0;
  CPU_BOOLEAN       result = DEF_FAIL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_SET((p_addr_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_tbl_qty != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);

  addr_tbl_qty = *p_addr_tbl_qty;
  *p_addr_tbl_qty = 0u;                                         // Cfg rtn addr tbl qty for err  (see Note #3b).

  //                                                               ------------------ GET IPv4 ADDRS ------------------
  p_addr = p_addr_tbl;

  if (if_nbr == NET_IF_NBR_LOOPBACK) {                          // For loopback IF,                  ...
#ifdef NET_IF_LOOPBACK_MODULE_EN
    *p_addr = NET_IPv4_ADDR_LOCAL_HOST_ADDR;                    // ... get dflt IPv4 localhost addr; ...
    *p_addr_tbl_qty = 1u;
    result = DEF_OK;
    goto exit;
#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_FAIL);
#endif
  }

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (p_if->IP_Obj->IPv4_AddrListPtr == DEF_NULL) {
    goto exit;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
    if (addr_ix >= addr_tbl_qty) {
      break;
    }

    *p_addr = p_addr_obj->AddrHost;
    addr_ix++;
    p_addr++;
  }

  result = DEF_OK;

  *p_addr_tbl_qty = addr_ix;                                    // Rtn nbr of cfg'd addrs.

exit:
  return (result);
}

/****************************************************************************************************//**
 *                                       NetIPv4_GetAddrSrcHandler()
 *
 * @brief    (1) Get corresponding configured IPv4 host address for a remote IPv4 address :
 *               - (a) Search Remote IPv4 Address cache for corresponding  configured IPv4 host address
 *                     that recently communicated with the remote IPv4 address
 *                   - (1) NOT yet implemented if IPv4 routing table to be implemented. #### NET-816
 *               - (b) Search configured IP host addresses structure for configured IPv4 host address with
 *                     same local network as the remote IPv4 address
 *
 * @param    p_if_nbr        Pointer to variable containing the preferred Interface number.
 *                           Pointer that will received the Interface number of the selected IP address
 *                           otherwise.
 *
 * @param    addr_remote     Remote address to get configured IPv4 host address (see Note #3).
 *
 * @param    p_err           Error pointer.
 *
 *
 * Argument(s) : p_if_nbr        Pointer to variable containing the preferred Interface number.
 *                                   Pointer that will received the Interface number of the selected IP address
 *                                   otherwise.
 *
 *               addr_remote     Remote address to get configured IPv4 host address (see Note #3).
 *
 *               p_err           Error pointer.
 *
 * Return(s)   : Configured IPv4 host address (see Note #3), if available.
 *               NET_IPv4_ADDR_NONE,                         otherwise.
 *
 * Note(s)     : (2) NetIPv4_GetAddrSrcHandler() is called by network protocol suite function(s) &
 *                   MUST be called with the global network lock already acquired.
 *
 *               (3) IPv4 addresses MUST be in host-order.
 *******************************************************************************************************/
NET_IPv4_ADDR NetIPv4_GetAddrSrcHandler(NET_IF_NBR    *p_if_nbr,
                                        NET_IPv4_ADDR addr_remote,
                                        RTOS_ERR      *p_err)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
  NET_IPv4_ADDR     addr_host = NET_IPv4_ADDR_NONE;
  NET_IPv4_ADDR     addr_backup = NET_IPv4_ADDR_NONE;
  NET_IF_NBR        if_nbr = NET_IF_NBR_BASE_CFGD;
  CPU_BOOLEAN       valid = DEF_NO;

  //                                                               ---------------- VALIDATE IPv4 ADDR ----------------
  RTOS_ASSERT_DBG_ERR_SET((addr_remote != NET_IPv4_ADDR_NONE), *p_err, RTOS_ERR_INVALID_ARG, NET_IPv4_ADDR_NONE);

  //                                                               -------------- SRCH REMOTE ADDR CACHE --------------
  //                                                               See Note #1a1.

  valid = NetIF_IsValidCfgdHandler(*p_if_nbr);
  if (valid == DEF_YES) {                                       // Found address on given Interface.
    p_if = NetIF_Get(*p_if_nbr, p_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_IPv4_ADDR_NONE);

    //                                                             Find address on IF with matching subnet.
    SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
      if ((addr_remote & p_addr_obj->AddrHostSubnetMask)
          == p_addr_obj->AddrHostSubnetNet ) {
        if (p_addr_obj->IsValid == DEF_YES) {
          addr_host = p_addr_obj->AddrHost;
          break;
        }
      }
      //                                                           Save a backup address that has a valid gateway ...
      //                                                           ... in case addr destination is outside local net.
      if ((p_addr_obj->AddrDfltGateway != NET_IPv4_ADDR_NONE)
          && (p_addr_obj->AddrHost != NET_IPv4_ADDR_NONE)
          && (p_addr_obj->IsValid == DEF_YES)
          && (addr_backup == NET_IPv4_ADDR_NONE)) {
        addr_backup = p_addr_obj->AddrHost;
      }
    }
    //                                                             Return address found.
    if (addr_host == NET_IPv4_ADDR_NONE) {
      addr_host = addr_backup;
    }
  } else {                                                      // If no valid Interface was given...
                                                                // ... search address on all configured Interfaces.
    while ((if_nbr < Net_CoreDataPtr->IF_NbrCfgd)
           && (addr_host == NET_IPv4_ADDR_NONE)) {
      p_if = NetIF_Get(if_nbr, p_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_IPv4_ADDR_NONE);

      SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
        if ((addr_remote & p_addr_obj->AddrHostSubnetMask)
            == p_addr_obj->AddrHostSubnetNet ) {
          if (p_addr_obj->IsValid == DEF_YES) {
            addr_host = p_addr_obj->AddrHost;
            *p_if_nbr = if_nbr;
            break;
          }
        }
      }

      if (addr_host == NET_IPv4_ADDR_NONE) {
        if_nbr++;
      }
    }

    if (addr_host == NET_IPv4_ADDR_NONE) {
      if_nbr = NetIF_GetDflt();

      p_if = NetIF_Get(if_nbr, p_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_IPv4_ADDR_NONE);

      SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
        if ((p_addr_obj->AddrDfltGateway != NET_IPv4_ADDR_NONE)
            && (p_addr_obj->AddrHost != NET_IPv4_ADDR_NONE)
            && (p_addr_obj->IsValid == DEF_YES)           ) {
          addr_host = p_addr_obj->AddrHost;
          *p_if_nbr = if_nbr;
          break;
        }
      }
    }
  }

  return (addr_host);
}

/****************************************************************************************************//**
 *                                       NetIPv4_GetAddrHostIF_Nbr()
 *
 * @brief    (1) Get the interface number for an IPv4 host address :
 *               - (a) A configured IPv4 host address (on an enabled interface)
 *               - (b) A 'Localhost'                address
 *               - (c) A 'This host' initialization address                        See Note #4
 *
 * @param    addr    Configured IPv4 host address to get the interface number (see Note #3).
 *
 * @return   Interface number of a configured IPv4 host address,    if available.
 *           Interface number of              IPv4 host address
 *           in dynamic address initialization (see Note #4), if available.
 *           NET_IF_NBR_LOCAL_HOST,                               for a localhost address.
 *           NET_IF_NBR_NONE,                                     otherwise.
 *
 * @note     (2) NetIPv4_GetAddrHostIF_Nbr() is called by network protocol suite function(s) & MUST
 *               be called with the global network lock already acquired.
 *
 * @note     (3) IPv4 address MUST be in host-order.
 *
 * @note     (4) For the 'This Host' initialization address, the interface in the dynamic address
 *               initialization state (if any) is returned (see 'NetIPv4_CfgAddrAddDynamicStart()
 *               Note #4b2').  This allows higher layers to select an interface in dynamic address
 *               initialization & transmit the 'This Host' initialization address in order to
 *               negotiate & configure a dynamic address for the interface.
 *******************************************************************************************************/
NET_IF_NBR NetIPv4_GetAddrHostIF_Nbr(NET_IPv4_ADDR addr)
{
  NET_IF            *p_if;
  NET_IPv4_ADDR_OBJ *p_addr_obj;
  NET_IF_NBR        if_nbr = NET_IF_NBR_BASE_CFGD;
  CPU_BOOLEAN       addr_this_host;
  CPU_BOOLEAN       addr_local_host;
  CPU_BOOLEAN       addr_init = DEF_NO;

  addr_this_host = NetIPv4_IsAddrThisHost(addr);                // Chk 'This Host' addr  (see Note #1c).
  if (addr_this_host == DEF_YES) {
    while ((if_nbr < Net_CoreDataPtr->IF_NbrCfgd)               // Srch ALL cfg'd IF's ...
           && (addr_init == DEF_NO)) {
      RTOS_ERR local_err;

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      p_if = NetIF_Get(if_nbr, &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
        if_nbr = NET_IF_NBR_NONE;
        goto exit;
      }

      SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
        if ((p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_DYN_INIT)
            || (p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_AUTO_CFG)) {
          addr_init = DEF_YES;
          break;
        }
      }

      if (addr_init != DEF_YES) {
        if_nbr++;
      }
    }

    if (addr_init != DEF_YES) {                                 // If NO dynamic addr init found, ...
      if_nbr = NET_IF_NBR_NONE;                                 // ... rtn NO IF.
    }
  } else {
    //                                                             Chk localhost  addrs (see Note #1b).
    addr_local_host = NetIPv4_IsAddrLocalHost(addr);
    if (addr_local_host == DEF_YES) {
      if_nbr = NET_IF_NBR_LOCAL_HOST;
      //                                                           Chk cfg'd host addrs (see Note #1a).
    } else {
      if_nbr = NetIPv4_GetAddrHostCfgdIF_Nbr(addr);
    }
  }

exit:
  return (if_nbr);
}

/****************************************************************************************************//**
 *                                       NetIPv4_GetAddrHostCfgdIF_Nbr()
 *
 * @brief    Get the interface number for a configured IPv4 host address.
 *
 * @param    addr    Configured IPv4 host address to get the interface number (see Note #2).
 *
 * @return   Interface number of a configured IPv4 host address, if available.
 *           NET_IF_NBR_NONE,                                    otherwise.
 *
 * @note     (1) NetIPv4_GetAddrHostCfgdIF_Nbr() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *
 * @note     (2) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
NET_IF_NBR NetIPv4_GetAddrHostCfgdIF_Nbr(NET_IPv4_ADDR addr)
{
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
  NET_IF_NBR        if_nbr = NET_IF_NBR_NONE;

  p_addr_obj = NetIPv4_GetAddrObjCfgd(&if_nbr, addr);
  if (p_addr_obj == DEF_NULL) {
    goto exit;
  }

exit:
  return (if_nbr);
}

/****************************************************************************************************//**
 *                                       NetIPv4_IsAddrsCfgdOnIF_Handler()
 *
 * @brief    Check if any IPv4 address(s) configured on an interface.
 *
 * @param    if_nbr  Interface number to check for configured IPv4 address(s).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if any IPv4 host address(s) configured on interface.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) NetIPv4_IsAddrsCfgdOnIF_Handler() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *               See also 'NetIPv4_IsAddrsCfgdOnIF()  Note #1'.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrsCfgdOnIF_Handler(NET_IF_NBR if_nbr,
                                            RTOS_ERR   *p_err)
{
  NET_IF      *p_if = DEF_NULL;
  CPU_BOOLEAN addr_avail = DEF_NO;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_cfgd = DEF_NO;
#endif

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }
#endif

  //                                                               ------------ CHK CFG'D IPv4 ADDRS AVAIL ------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  addr_avail = (p_if->IP_Obj->IPv4_AddrListPtr != DEF_NULL) ? DEF_YES : DEF_NO;

exit:
  return (addr_avail);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsValidTOS()
 *
 * @brief    Validate an IPv4 TOS.
 *
 * @param    TOS     IPv4 TOS to validate.
 *
 * @return   DEF_YES, if IPv4 TOS valid.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) See 'net_ipv4.h  IPv4 HEADER TYPE OF SERVICE (TOS) DEFINES  Note #1'
 *               & 'net_ipv4.h  IPv4 HEADER  Note #3'.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsValidTOS(NET_IPv4_TOS TOS)
{
  CPU_BOOLEAN tos_mbz;

  tos_mbz = DEF_BIT_IS_SET(TOS, NET_IPv4_HDR_TOS_MBZ_MASK);       // Chk for invalid TOS bit(s).
  if (tos_mbz != DEF_NO) {
    return (DEF_NO);
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsValidTTL()
 *
 * @brief    Validate an IPv4 TTL.
 *
 * @param    TTL     IPv4 TTL to validate.
 *
 * @return   DEF_YES, if IPv4 TTL valid.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) RFC #1122, Section 3.2.1.7 states that "a host MUST NOT send a datagram with a
 *               Time-to-Live (TTL) value of zero".
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsValidTTL(NET_IPv4_TTL TTL)
{
  if (TTL < 1) {                                                // Chk TTL < 1 (see Note #1).
    return (DEF_NO);
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsValidFlags()
 *
 * @brief    Validate IPv4 flags.
 *
 * @param    flags   IPv4 flags to select options; bit-field flags logically OR'd :
 *                   NET_IPv4_FLAG_NONE              No  IPv4 flags selected.
 *                   NET_IPv4_FLAG_TX_DONT_FRAG      Set IPv4 'Don't Frag' flag.
 *
 * @return   DEF_YES, if IPv4 flags valid.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsValidFlags(NET_IPv4_FLAGS flags)
{
  NET_IPv4_FLAGS flag_mask;

  flag_mask = NET_IPv4_FLAG_NONE
              | NET_IPv4_FLAG_TX_DONT_FRAG;
  //                                                               Chk for any invalid flags req'd.
  if ((flags & (NET_IPv4_FLAGS) ~flag_mask) != NET_IPv4_FLAG_NONE) {
    return (DEF_NO);
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                               NetIPv4_Rx()
 *
 * @brief    (1) Process received datagrams & forward to network protocol layers :
 *               - (a) Validate IPv4 packet & options
 *               - (b) Reassemble fragmented datagrams
 *               - (c) Demultiplex datagram to higher-layer protocols
 *               - (d) Update receive statistics
 *           - (2) Although IPv4 data units are typically referred to as 'datagrams' (see RFC #791, Section 1.1),
 *                 the term 'IP packet' (see RFC #1983, 'packet') is used for IPv4 Receive until the packet is
 *                 validated, & possibly reassembled, as an IPv4 datagram.
 *
 * @param    p_buf   Pointer to network buffer that received IPv4 packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (3) This function is a network protocol suite to network interface (IF) function & SHOULD be
 *               called only by appropriate network interface function(s).
 *
 * @note     (4) Since NetIPv4_RxPktFragReasm() may return a pointer to a different packet buffer (see
 *               'NetIPv4_RxPktFragReasm()  Return(s)', 'p_buf_hdr' MUST be reloaded.
 *
 * @note     (5) Datagram length:
 *               - (a) For single packet buffer IPv4 datagrams, the datagram length is equal to the IPv4
 *                     Total Length minus the IPv4 Header Length.
 *               - (b) For multiple packet buffer, fragmented IPv4 datagrams, the datagram length is
 *                     equal to the previously calculated total fragment size.
 *               - (1) IP datagram length is stored ONLY in the first packet buffer of any
 *                     fragmented packet buffers.
 *
 * @note     (6) Network buffer already freed by higher layer; only increment error counter.
 *******************************************************************************************************/
void NetIPv4_Rx(NET_BUF  *p_buf,
                RTOS_ERR *p_err)
{
  NET_BUF      *p_frag_list;
  NET_BUF_HDR  *p_buf_hdr;
  NET_IPv4_HDR *p_ip_hdr;
  CPU_BOOLEAN  is_frag;

  NET_CTR_STAT_INC(Net_StatCtrs.IPv4.RxPktCtr);

  //                                                               -------------- VALIDATE RX'D IPv4 PKT --------------
  p_buf_hdr = &p_buf->Hdr;

  p_ip_hdr = (NET_IPv4_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];

  NetIPv4_RxPktValidate(p_buf, p_buf_hdr, p_ip_hdr, p_err);     // Validate rx'd pkt.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               ------------------- REASM FRAGS --------------------
  p_frag_list = NetIPv4_RxPktFragReasm(p_buf, p_buf_hdr, p_ip_hdr, &is_frag, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               ------------ DEMUX DATAGRAM ------------
  if (p_frag_list != DEF_NULL) {
    p_buf_hdr = &p_frag_list->Hdr;

    if (is_frag == DEF_NO) {
      p_buf_hdr->IP_DatagramLen = p_buf_hdr->IP_TotLen                // ... calc buf datagram len (see Note #4a).
                                  - p_buf_hdr->IP_HdrLen;
    } else {
      p_buf_hdr->IP_DatagramLen = p_buf_hdr->IP_FragSizeTot;          // ...       as datagram len (see Note #4b).
    }

    NetIPv4_RxPktDemuxDatagram(p_frag_list, p_buf_hdr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_discard;
    }
  }

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxPktDisCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIPv4_Tx()
 *
 * @brief    (1) Prepare & transmit IPv4 datagram packet(s) :
 *               - (a) Validate  transmit packet
 *               - (b) Prepare & transmit packet datagram
 *               - (c) Update    transmit statistics
 *
 * @param    p_buf       Pointer to network buffer to transmit IPv4 packet.
 *
 * @param    addr_src    Source      IPv4 address.
 *
 * @param    addr_dest   Destination IPv4 address.
 *
 * @param    TOS         Specific TOS to transmit IPv4 packet (see Note #2a).
 *
 * @param    TTL         Specific TTL to transmit IPv4 packet (see Note #2b) :
 *                               - NET_IPv4_TTL_MIN                minimum TTL transmit value   (1)
 *                               - NET_IPv4_TTL_MAX                maximum TTL transmit value (255)
 *                               - NET_IPv4_TTL_DFLT               default TTL transmit value (128)
 *                               - NET_IPv4_TTL_NONE               replace with default TTL
 *
 * @param    flags       Flags to select transmit options; bit-field flags logically OR'd :
 *                               - NET_IPv4_FLAG_NONE              No  IPv4 transmit flags selected.
 *                               - NET_IPv4_FLAG_TX_DONT_FRAG      Set IPv4 'Don't Frag' flag.
 *
 * @param    p_opts      Pointer to one or more IPv4 options configuration data structures (see Note #2c) :
 *                               - NULL                            NO IP transmit options configuration.
 *                               - NET_IPv4_OPT_CFG_ROUTE_TS       Route &/or Internet Timestamp options configuration.
 *                               - NET_IPv4_OPT_CFG_SECURITY       Security options configuration
 *                                                                                                 (see 'net_ipv4.c  Note #1d').
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See RFC #1122, Section 3.2.1.6
 *                   RFC #1122, Section 3.2.1.7
 *                   RFC #1122, Section 3.2.1.8
 *
 * @note     (3) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
void NetIPv4_Tx(NET_BUF       *p_buf,
                NET_IPv4_ADDR addr_src,
                NET_IPv4_ADDR addr_dest,
                NET_IPv4_TOS  TOS,
                NET_IPv4_TTL  TTL,
                CPU_INT16U    flags,
                void          *p_opts,
                RTOS_ERR      *p_err)
{
  NET_BUF_HDR *p_buf_hdr;

  p_buf_hdr = &p_buf->Hdr;

  //                                                               --------------- VALIDATE IPv4 TX PKT ---------------
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  NetIPv4_TxPktValidate(p_buf_hdr,
                        addr_src,
                        addr_dest,
                        TOS,
                        TTL,
                        flags,
                        p_opts,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }
#endif

  //                                                               ------------------- TX IPv4 PKT --------------------
  NetIPv4_TxPkt(p_buf,
                p_buf_hdr,
                addr_src,
                addr_dest,
                TOS,
                TTL,
                flags,
                p_opts,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               ----------------- UPDATE TX STATS ------------------
  NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDgramCtr);

  goto exit;

exit_discard:
#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxPktDisCtr);
#endif

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIPv4_ReTx()
 *
 * @brief    (1) Prepare & re-transmit packets from transport protocol layers to network interface layer :
 *               - (a) Validate  re-transmit packet
 *               - (b) Prepare & re-transmit packet datagram
 *               - (c) Update    re-transmit statistics
 *
 * @param    p_buf   Pointer to network buffer to re-transmit IPv4 packet.
 *
 *
 * Argument(s) : p_buf       Pointer to network buffer to re-transmit IPv4 packet.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
void NetIPv4_ReTx(NET_BUF  *p_buf,
                  RTOS_ERR *p_err)
{
  NET_BUF_HDR *p_buf_hdr;

  p_buf_hdr = &p_buf->Hdr;

  //                                                               ------------------ RE-TX IPv4 PKT ------------------
  NetIPv4_ReTxPkt(p_buf,
                  p_buf_hdr,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxPktDisCtr);
    return;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDgramCtr);
}

/****************************************************************************************************//**
 *                                       NetIPv4_GetAddrObjCfgdOnIF()
 *
 * @brief    Get IPv4 addresses structure for an interface's configured IPv4 address.
 *
 * @param    addr    Configured IPv4 host address to get the interface number & IPv4 addresses
 *                   structure (see Note #1).
 *
 * @param    if_nbr  Interface number to search for configured IPv4 address.
 *
 * @return   Pointer to corresponding IP address structure, if IPv4 address configured on this interface.
 *           Pointer to NULL,                                 otherwise.
 *
 * @note     (1) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
NET_IPv4_ADDR_OBJ *NetIPv4_GetAddrObjCfgdOnIF(NET_IF_NBR    if_nbr,
                                              NET_IPv4_ADDR addr)
{
  NET_IF            *p_if;
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
  CPU_BOOLEAN       found = DEF_NO;
  RTOS_ERR          local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_if = NetIF_Get(if_nbr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------- SEARCH IF FOR IPv4 ADDR --------------
  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
    if (p_addr_obj->AddrHost == addr) {
      found = DEF_YES;
      break;
    }
  }

  if (found != DEF_YES) {
    p_addr_obj = DEF_NULL;
  }

exit:
  return (p_addr_obj);
}

/****************************************************************************************************//**
 *                                           NetIPv4_TxIxDataGet()
 *
 * @brief    Get the offset of a buffer at which the IPv4 data CAN be written.
 *
 * @param    if_nbr      Network interface number to transmit data.
 *
 * @param    data_len    IPv4 payload size.
 *
 * @param    mtu         MTU for the upper-layer protocol.
 *
 * @param    p_ix        Pointer to the current protocol index.
 *******************************************************************************************************/
void NetIPv4_TxIxDataGet(NET_IF_NBR if_nbr,
                         CPU_INT32U data_len,
                         CPU_INT16U mtu,
                         CPU_INT16U *p_ix)
{
  //                                                               Add IPv4 min hdr len to current offset.
  *p_ix += NET_IPv4_HDR_SIZE;

  //                                                               Add the lower-level hdr        offsets.
  NetIF_TxIxDataGet(if_nbr, data_len, p_ix);

  PP_UNUSED_PARAM(mtu);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   NETWORK MANAGER INTERFACE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetIPv4_GetHostAddrProtocol()
 *
 * @brief    Get an interface's IPv4 protocol address(s) [see Note #1].
 *
 * @param    if_nbr                      Interface number to get IPv4 protocol address(s).
 *
 * @param    p_addr_protocol_tbl         Pointer to a protocol address table that will receive the protocol
 *                                       address(s) in network-order for this interface.
 *
 * @param    p_addr_protocol_tbl_qty     Pointer to a variable to ... :
 *                                           - (a) Pass the size of the protocol address table, in number of
 *                                                 protocol address(s), pointed to by 'p_addr_protocol_tbl'.
 *                                           - (b) Either:
 *                                               - (1) Return the actual number of IPv4 protocol address(s),
 *                                                     if NO error(s);
 *                                               - (2) Return 0, otherwise.
 *                                                     See also Note #2a.
 *
 * @param    p_addr_protocol_len         Pointer to a variable to ... :
 *                                           - (a) Pass the length of the protocol address table address(s),
 *                                                 in octets.
 *                                           - (b) Either:
 *                                               - (1) Return the actual length of IPv4 protocol address(s),
 *                                                     in octets, if NO error(s);
 *                                               - (2) Return 0, otherwise.
 *                                       See also Note #2b.
 *
 * @param    p_err                       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) IPv4 protocol address(s) returned in network-order.
 *******************************************************************************************************/
#if 0
void NetIPv4_GetHostAddrProtocol(NET_IF_NBR if_nbr,
                                 CPU_INT08U *p_addr_protocol_tbl,
                                 CPU_INT08U *p_addr_protocol_tbl_qty,
                                 CPU_INT08U *p_addr_protocol_len,
                                 RTOS_ERR   *p_err)
{
  CPU_INT08U       *p_addr_protocol;
  NET_IPv4_ADDR    *p_addr_ip;
  NET_IPv4_ADDR    addr_ip_tbl[NET_IPv4_CFG_IF_MAX_NBR_ADDR];
  NET_IP_ADDRS_QTY addr_ip_tbl_qty;
  NET_IP_ADDRS_QTY addr_ix;
  CPU_INT08U       addr_ip_len;

  RTOS_ASSERT_DBG_ERR_SET((p_addr_protocol_tbl_qty != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_protocol_len != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_protocol_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  *p_addr_protocol_tbl_qty = 0u;                                // Cfg dflt tbl qty for err

  *p_addr_protocol_len = 0u;                                    // Cfg dflt addr len for err
  addr_ip_len = sizeof(NET_IPv4_ADDR);

  //                                                               ------------- GET IPv4 PROTOCOL ADDRS --------------
  addr_ip_tbl_qty = sizeof(addr_ip_tbl) / sizeof(NET_IPv4_ADDR);
  (void)NetIPv4_GetAddrHostHandler(if_nbr,
                                   &addr_ip_tbl[0],
                                   &addr_ip_tbl_qty,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  addr_ix = 0u;
  p_addr_ip = &addr_ip_tbl[addr_ix];
  p_addr_protocol = &p_addr_protocol_tbl[addr_ix];
  while (addr_ix < addr_ip_tbl_qty) {                           // Rtn all IPv4 protocol addr(s) ...
    NET_UTIL_VAL_COPY_SET_NET_32(p_addr_protocol, p_addr_ip);     // ... in net-order (see Note #1).
    p_addr_protocol += addr_ip_len;
    p_addr_ip++;
    addr_ix++;
  }

  //                                                               Rtn nbr & len of IPv4 protocol addr(s).
  *p_addr_protocol_tbl_qty = (CPU_INT08U)addr_ip_tbl_qty;
  *p_addr_protocol_len = (CPU_INT08U)addr_ip_len;

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_GetAddrProtocolIF_Nbr()
 *
 * @brief    (1) Get the interface number for a host's IPv4 protocol address :
 *               - (a) A configured IPv4 host address (on an enabled interface)
 *               - (b) A 'Localhost'          address
 *               - (c) An           IPv4 host initialization address
 *
 * @param    p_addr_protocol     Pointer to protocol address (see Note #2).
 *
 * @param    addr_protocol_len   Length  of protocol address (in octets).
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @return   Interface number for IPv4 protocol address, if configured on this host.
 *           Interface number of  IPv4 protocol address
 *           in address initialization,              if available.
 *           NET_IF_NBR_LOCAL_HOST,                      for a localhost address.
 *           NET_IF_NBR_NONE,                            otherwise.
 *
 * @note     (2) Protocol address MUST be in network-order.
 *******************************************************************************************************/
NET_IF_NBR NetIPv4_GetAddrProtocolIF_Nbr(CPU_INT08U *p_addr_protocol,
                                         CPU_INT08U addr_protocol_len,
                                         RTOS_ERR   *p_err)
{
  NET_IPv4_ADDR addr_ip;
  NET_IF_NBR    if_nbr = NET_IF_NBR_NONE;

  //                                                               ------- GET IPv4 PROTOCOL ADDR's IF NBR --------
  NET_UTIL_VAL_COPY_GET_NET_32(&addr_ip, p_addr_protocol);
  if_nbr = NetIPv4_GetAddrHostIF_Nbr(addr_ip);
  if (if_nbr == NET_IF_NBR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  PP_UNUSED_PARAM(addr_protocol_len);

exit:
  return (if_nbr);
}

/****************************************************************************************************//**
 *                                       NetIPv4_IsValidAddrProtocol()
 *
 * @brief    Validate an IPv4 protocol address.
 *
 * @param    p_addr_protocol     Pointer to protocol address (see Note #1).
 *
 * @param    addr_protocol_len   Length  of protocol address (in octets).
 *
 * @return   DEF_YES, if IPv4 protocol address valid.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) IPv4 protocol address MUST be in network-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsValidAddrProtocol(CPU_INT08U *p_addr_protocol,
                                        CPU_INT08U addr_protocol_len)
{
  NET_IPv4_ADDR addr_ip;
  CPU_BOOLEAN   valid = DEF_NO;

  //                                                               --------- VALIDATE IPv4 PROTOCOL ADDR ----------
  NET_UTIL_VAL_COPY_GET_NET_32(&addr_ip, p_addr_protocol);
  valid = NetIPv4_IsValidAddrHost(addr_ip);

  PP_UNUSED_PARAM(addr_protocol_len);

  return (valid);
}

/****************************************************************************************************//**
 *                                           NetIPv4_IsAddrInit()
 *
 * @brief    Validate an IPv4 protocol address as the initialization address.
 *
 * @param    p_addr_protocol     Pointer to protocol address (see Note #1).
 *
 * @param    addr_protocol_len   Length  of protocol address (in octets).
 *
 * @return   DEF_YES, if IPv4 protocol address is the initialization address.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) IPv4 protocol address MUST be in network-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrInit(CPU_INT08U *p_addr_protocol,
                               CPU_INT08U addr_protocol_len)
{
  NET_IPv4_ADDR addr_ip;
  CPU_BOOLEAN   addr_init = DEF_NO;

  PP_UNUSED_PARAM(addr_protocol_len);

  //                                                               ------- VALIDATE IPv4 PROTOCOL INIT ADDR -------
  NET_UTIL_VAL_COPY_GET_NET_32(&addr_ip, p_addr_protocol);
  addr_init = NetIPv4_IsAddrThisHost(addr_ip);

  return (addr_init);
}

/****************************************************************************************************//**
 *                                       NetIPv4_IsAddrProtocolMulticast()
 *
 * @brief    Validate an IPv4 protocol address as a multicast address.
 *
 * @param    p_addr_protocol     Pointer to protocol address (see Note #1).
 *
 * @param    addr_protocol_len   Length  of protocol address (in octets).
 *
 * @return   DEF_YES, if IPv4 protocol address is a multicast address.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) IPv4 protocol address MUST be in network-order.
 *******************************************************************************************************/

#ifdef  NET_MCAST_TX_MODULE_EN
CPU_BOOLEAN NetIPv4_IsAddrProtocolMulticast(CPU_INT08U *p_addr_protocol,
                                            CPU_INT08U addr_protocol_len)
{
  NET_IPv4_ADDR addr_ip;
  CPU_BOOLEAN   addr_multicast = DEF_NO;

  PP_UNUSED_PARAM(addr_protocol_len);

  //                                                               ------- VALIDATE IPv4 PROTOCOL INIT ADDR -------
  NET_UTIL_VAL_COPY_GET_NET_32(&addr_ip, p_addr_protocol);
  addr_multicast = NetIPv4_IsAddrMulticast(addr_ip);

  return (addr_multicast);
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_IsAddrProtocolConflict()
 *
 * @brief    Get interface's IPv4 protocol address conflict status.
 *
 * @param    if_nbr  Interface number to get IPv4 protocol address conflict status.
 *
 * @return   DEF_YES, if IPv4 protocol address conflict detected.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) See RFC #3927, Section 2.5
 *
 * @note     (2) Interfaces' IPv4 address configuration 'AddrProtocolConflict' variables MUST ALWAYS
 *               be accessed exclusively in critical sections.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv4_IsAddrProtocolConflict(NET_IF_NBR if_nbr)
{
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  NET_IF_NBR valid;
#endif
  NET_IF            *p_if;
  NET_IPv4_ADDR_OBJ *p_addr_obj;
  CPU_BOOLEAN       addr_conflict = DEF_NO;
  RTOS_ERR          local_err;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               --------------- VALIDATE IF NBR ----------------
  valid = NetIF_IsValidHandler(if_nbr);
  if (valid != DEF_YES) {
    goto exit;
  }
#endif

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  p_if = NetIF_Get(if_nbr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------- CHK IPv4 PROTOCOL ADDR CONFLICT --------
  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
    if (p_addr_obj->IsValid == DEF_NO) {
      addr_conflict = DEF_YES;
      break;
    }
  }

exit:
  return (addr_conflict);
}

/****************************************************************************************************//**
 *                                       NetIPv4_ChkAddrProtocolConflict()
 *
 * @brief    Check for any IPv4 protocol address conflict between this interface's IPv4 host address(s)
 *           & other host(s) on the local network.
 *
 * @param    if_nbr              Interface number to get IPv4 protocol address conflict status.
 *
 * @param    p_addr_protocol     Pointer to protocol address (see Note #1).
 *
 * @param    addr_protocol_len   Length of  protocol address (in octets).
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) IPv4 protocol address MUST be in network-order.
 *
 * @note     (2) See RFC #3927, Section 2.5
 *
 * @note     (3) Network layer manager SHOULD eventually be responsible for maintaining each
 *               interface's network address(s)' configuration. #### NET-809
 *******************************************************************************************************/
void NetIPv4_ChkAddrProtocolConflict(NET_IF_NBR if_nbr,
                                     CPU_INT08U *p_addr_protocol,
                                     CPU_INT08U addr_protocol_len,
                                     RTOS_ERR   *p_err)
{
  NET_IF            *p_if;
  NET_IPv4_ADDR_OBJ *p_addr_obj;
  NET_IPv4_ADDR     addr_ip;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_valid;
#endif
  CPU_BOOLEAN addr_conflict;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               --------------- VALIDATE IF NBR ----------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }
#endif

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
    NET_UTIL_VAL_SET_NET_32(&addr_ip, p_addr_obj->AddrHost);
    addr_conflict = Mem_Cmp(p_addr_protocol, &addr_ip, addr_protocol_len);

    if (addr_conflict == DEF_YES) {
      p_addr_obj->IsValid = DEF_NO;
      break;
    }
  }

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetIPv4_CfgAddrRemoveAllHandler()
 *
 * @brief    (1) Remove all configured IPv4 host address(s) from an interface :
 *
 *           - (a) Validate IPv4 address configuration state                                   See Note #3b
 *           - (b) Remove ALL configured IPv4 address(s) from interface's IPv4 address table :
 *               - (1) Close all connections for each address
 *           - (c) Reset    IPv4 address configuration state to static                         See Note #3c
 *
 * @param    if_nbr  Interface number to remove address configuration.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if ALL interface's configured IPv4 host address(s) successfully removed.
 *           DEF_FAIL, otherwise.
 *
 * @note     (2) NetIPv4_CfgAddrRemoveAllHandler() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *
 * @note     (3) Interface configuration:
 *           - (a) An interface may be configured with either :
 *
 *               - (1) One or more statically- configured IPv4 addresses (default configuration)
 *                           OR
 *               - (2) Exactly one dynamically-configured IPv4 address
 *
 *           - (b) If an interface's IPv4 host address(s) are NOT currently statically- or dynamically-
 *                   configured, then NO address(s) may NOT be removed.  However, an interface in the
 *                   dynamic-init may call this function which effect will solely put that interface
 *                   back in the default statically-configured mode.
 *
 *           - (c) When NO address(s) are configured on an interface after ALL address(s) are removed,
 *                   the interface's address configuration is defaulted back to statically-configured.
 *******************************************************************************************************/
static CPU_BOOLEAN NetIPv4_CfgAddrRemoveAllHandler(NET_IF_NBR if_nbr,
                                                   RTOS_ERR   *p_err)
{
  NET_IF            *p_if;
  NET_IPv4_ADDR_OBJ *p_addr_obj;
  SLIST_MEMBER      *p_node;
  CPU_BOOLEAN       result = DEF_FAIL;

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  while (p_if->IP_Obj->IPv4_AddrListPtr != DEF_NULL) {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    p_node = SList_Pop(&p_if->IP_Obj->IPv4_AddrListPtr);

    p_addr_obj = SLIST_ENTRY(p_node, NET_IPv4_ADDR_OBJ, ListNode);

    Mem_DynPoolBlkFree(&NetIPv4_AddrPool, p_addr_obj, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_FAIL);
  }

  result = DEF_OK;

exit:
  return (result);
}

/****************************************************************************************************//**
 *                                           NetIPv4_CfgAddrValidate()
 *
 * @brief    Validate an IPv4 host address, subnet mask, & default gateway for configuration on an
 *           interface.
 *
 * @param    addr_host           Desired IPv4 address                 to configure (see Note #1).
 *
 * @param    addr_subnet_mask    Desired IPv4 address subnet mask     to configure (see Note #1).
 *
 * @param    addr_dflt_gateway   Desired IPv4 default gateway address to configure (see Note #1).
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) IPv4 addresses MUST be in host-order.
 *
 *                               See also RFC #1122, Section 3.3.1.1
 *******************************************************************************************************/

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIPv4_CfgAddrValidate(NET_IPv4_ADDR addr_host,
                                    NET_IPv4_ADDR addr_subnet_mask,
                                    NET_IPv4_ADDR addr_dflt_gateway,
                                    RTOS_ERR      *p_err)
{
  CPU_BOOLEAN addr_valid;

  //                                                               Validate host addr & subnet mask.
  addr_valid = NetIPv4_IsValidAddrHostCfgd(addr_host, addr_subnet_mask);
  if (addr_valid != DEF_YES) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.CfgInvAddrHostCtr);
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }
  //                                                               Validate dflt gateway (see Note #2).
  if (addr_dflt_gateway != NET_IPv4_ADDR_NONE) {
    addr_valid = NetIPv4_IsValidAddrHost(addr_dflt_gateway);
    if (addr_valid != DEF_YES) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.CfgInvGatewayCtr);
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
    }
    //                                                             Validate dflt gateway subnet (see Note #2b).
    if ((addr_dflt_gateway & addr_subnet_mask)
        != (addr_host         & addr_subnet_mask)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.CfgInvGatewayCtr);
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                           NetIPv4_GetAddrObjCfgd()
 *
 * @brief    Get interface number & IPv4 addresses structure for configured IPv4 address.
 *
 * @param    addr        Configured IPv4 host address to get the interface number & IPv4 addresses
 *                       structure (see Note #1).
 *
 * @param    p_if_nbr    Pointer to variable that will receive ... :
 *                       (a) The interface number for this configured IPv4 address, if available;
 *                       (b) NET_IF_NBR_NONE,                                       otherwise.
 *
 * @return   Pointer to corresponding IPv4 address structure, if IPv4 address configured on any interface.
 *           Pointer to NULL,                                 otherwise.
 *
 * @note     (1) IPv4 address MUST be in host-order.
 *
 * @note     (2) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
static NET_IPv4_ADDR_OBJ *NetIPv4_GetAddrObjCfgd(NET_IF_NBR    *p_if_nbr,
                                                 NET_IPv4_ADDR addr)
{
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
  NET_IF_NBR        if_nbr = NET_IF_NBR_BASE_CFGD;

  if (p_if_nbr != DEF_NULL) {                                   // Init IF nbr for err (see Note #2).
    *p_if_nbr = NET_IF_NBR_NONE;
  }

  //                                                               ---------------- VALIDATE IPv4 ADDR ----------------
  if (addr == NET_IPv4_ADDR_NONE) {
    goto exit;
  }

  //                                                               -------- SRCH ALL CFG'D IF's FOR IPv4 ADDR ---------

  while ((if_nbr < Net_CoreDataPtr->IF_NbrCfgd)                 // Srch all cfg'd IF's ...
         && (p_addr_obj == DEF_NULL)         ) {                // ... until addr found.
    p_addr_obj = NetIPv4_GetAddrObjCfgdOnIF(if_nbr, addr);
    if (p_addr_obj == DEF_NULL) {                               // If addr NOT found, ...
      if_nbr++;                                                 // ... adv to next IF nbr.
    }
  }

  if (p_addr_obj != DEF_NULL) {                                 // If addr avail, ...
    if (p_if_nbr != DEF_NULL) {
      *p_if_nbr = if_nbr;                                       // ... rtn IF nbr.
    }
  }

exit:
  return (p_addr_obj);
}

/****************************************************************************************************//**
 *                                           NetIPv4_RxPktValidate()
 *
 * Description : (1) Validate received IPv4 packet :
 *
 *                   (a) (1) Validate the received packet's following IPv4 header fields :
 *
 *                           (A) Version
 *                           (B) Header Length
 *                           (C) Total  Length                               See Note #4
 *                           (D) Flags
 *                           (E) Fragment Offset
 *                           (F) Protocol
 *                           (G) Check-Sum                                   See Note #5
 *                           (H) Source      Address                         See Note #9c
 *                           (I) Destination Address                         See Note #9d
 *                           (J) Options
 *
 *                       (2) Validation ignores the following IPv4 header fields :
 *
 *                           (A) Type of Service (TOS)
 *                           (B) Identification  (ID)
 *                           (C) Time-to-Live    (TTL)
 *
 *                   (b) Convert the following IPv4 header fields from network-order to host-order :
 *
 *                       (1) Total Length                                    See Notes #1bB1 & #3b
 *                       (2) Identification (ID)                             See Note  #1bB2
 *                       (3) Flags/Fragment Offset                           See Note  #1bB3
 *                       (4) Check-Sum                                       See Note  #5d
 *                       (5) Source      Address                             See Notes #1bB4 & #3c
 *                       (6) Destination Address                             See Notes #1bB5 & #3d
 *                       (7) All Options' multi-octet words                  See Notes #1bB6 & #1bC
 *
 *                           (A) These fields are NOT converted directly in the received packet buffer's
 *                               data area but are converted in local or network buffer variables ONLY.
 *
 *                           (B) The following IPv4 header fields are converted & stored in network buffer
 *                               variables :
 *
 *                               (1) Total Length
 *                               (2) Identification (ID)
 *                               (3) Flags/Fragment Offset
 *                               (4) Source      Address
 *                               (5) Destination Address
 *                               (6) IPv4 Options' multi-octet words
 *
 *                           (C) Since any IPv4 packet may receive a number of various IPv4 options that may
 *                               require conversion from network-order to host-order, IPv4 options are copied
 *                               into a separate network buffer for validation, conversion, & demultiplexing.
 *
 *                   (c) Update network buffer's protocol controls
 *
 *                   (d) Process IPv4 packet in ICMP Receive Handler
 *
 * Argument(s) : p_buf       Pointer to network buffer that received IPv4 packet.
 *
 *               p_buf_hdr   Pointer to network buffer header.
 *
 *               p_ip_hdr    Pointer to received packet's IPv4 header.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) See 'net_ipv4.h  IP HEADER' for IPv4 header format.
 *
 *               (3) The following IPv4 header fields MUST be decoded &/or converted from network-order to host-order
 *                   BEFORE any ICMP Error Messages are transmitted (see 'net_icmp.c  NetICMPv4_TxMsgErr()  Note #2') :
 *
 *                   (a) Header Length
 *                   (b) Total  Length
 *                   (c) Source      Address
 *                   (d) Destination Address
 *
 *               (4) (a) In addition to validating that the IPv4 header Total Length is greater than or equal to the
 *                       IPv4 header Header Length, the IPv4 total length SHOULD be compared to the remaining packet
 *                       data length which should be identical.
 *
 *                   (b) (1) However, some network interfaces MAY append octets to their frames :
 *
 *                           (A) 'pad' octets, if the frame length does NOT meet the frame's required minimum size :
 *
 *                               (1) RFC #894, Section 'Frame Format' states that "the minimum length of  the data
 *                                   field of a packet sent over an Ethernet is 46 octets.  If necessary, the data
 *                                   field should be padded (with octets of zero) to meet the Ethernet minimum frame
 *                                   size.  This padding is not part of the IPv4 packet and is not included in the
 *                                   total length field of the IPv4 header".
 *
 *                               (2) RFC #1042, Section 'Frame Format and MAC Level Issues : For all hardware types'
 *                                   states that "IEEE 802 packets may have a minimum size restriction.  When
 *                                   necessary, the data field should be padded (with octets of zero) to meet the
 *                                   IEEE 802 minimum frame size requirements.  This padding is not part of the IPv4
 *                                   datagram and is not included in the total length field of the IPv4 header".
 *
 *                           (B) Trailer octets, to improve overall throughput :
 *
 *                               (1) RFC #893, Section 'Introduction' specifies "a link-level ... trailer
 *                                   encapsulation, or 'trailer' ... to minimize the number and size of memory-
 *                                   to-memory copy operations performed by a receiving host when processing a
 *                                   data packet".
 *
 *                               (2) RFC #1122, Section 2.3.1 states that "trailer encapsulations[s] ... rearrange
 *                                   the data contents of packets ... [to] improve the throughput of higher layer
 *                                   protocols".
 *
 *                           (C) CRC or checksum values, optionally copied from a device.
 *
 *                       (2) Therefore, if ANY octets are appended to the total frame length, then the packet's
 *                           remaining data length MAY be greater than the IPv4 total length :
 *
 *                           (A) Thus,    the IPv4 total length & the packet's remaining data length CANNOT be
 *                               compared for equality.
 *
 *                               (1) Unfortunately, this eliminates the possibility to validate the IPv4 total
 *                                   length to the packet's remaining data length.
 *
 *                           (B) And      the IPv4 total length MAY    be less    than the packet's remaining
 *                               data length.
 *
 *                               (1) However, the packet's remaining data length MUST be reset to the IPv4
 *                                   total length to correctly calculate higher-layer application data
 *                                   length.
 *
 *                           (C) However, the IPv4 total length CANNOT be greater than the packet's remaining
 *                               data length.
 *
 *               (5) (a) IPv4 header Check-Sum field MUST be validated BEFORE (or AFTER) any multi-octet words
 *                       are converted from network-order to host-order since "the sum of 16-bit integers can
 *                       be computed in either byte order" [RFC #1071, Section 2.(B)].
 *
 *                       In other words, the IPv4 header Check-Sum CANNOT be validated AFTER SOME but NOT ALL
 *                       multi-octet words have been converted from network-order to host-order.
 *
 *                   (b) However, ALL received packets' multi-octet words are converted in local or network
 *                       buffer variables ONLY (see Note #1bA).  Therefore, IPv4 header Check-Sum may be validated
 *                       at any point.
 *
 *                   (c) For convenience, the IPv4 header Check-Sum is validated AFTER IPv4 Version, Header Length,
 *                       & Total Length fields have been validated.  Thus, invalid IPv4 version or length packets
 *                       are quickly discarded (see Notes #9a, #8a, & #8b) & the total IPv4 header length
 *                       (in octets) will already be calculated for the IPv4 Check-Sum calculation.
 *
 *                   (d) After the IPv4 header Check-Sum is validated, it is NOT necessary to convert the Check-
 *                       Sum from network-order to host-order since    it is NOT required for further processing.
 *
 *               (6) (a) RFC #791, Section 3.2 'Fragmentation and Reassembly' states that "if an internet datagram
 *                       is fragmented" :
 *
 *                       (1) "Fragments are counted in units of 8 octets."
 *                       (2) "The minimum fragment is 8 octets."
 *
 *                   (b) (1) However, this CANNOT apply "if this is the last fragment" ...
 *                           (A) "(that is the more fragments field is zero)";         ...
 *                       (2) Which may be of ANY size.
 *
 *                   See also 'net_ipv4.h  IPv4 FRAGMENTATION DEFINES  Note #1a'.
 *
 *               (7) (a) RFC #792, Section 'Destination Unreachable Message : Description' states that "if, in
 *                       the destination host, the IPv4 module cannot deliver the datagram because the indicated
 *                       protocol module ... is not active, the destination host may send a destination unreachable
 *                       message to the source host".
 *
 *                   (b) Default case already invalidated earlier in this function.  However, the default case
 *                       is included as an extra precaution in case 'Protocol' is incorrectly modified.
 *
 *               (8) ICMP Error Messages are sent if any of the following IP header fields are invalid :
 *
 *                   (a) Header Length                               ICMP 'Parameter   Problem'  Error Message
 *                   (b) Total  Length                               ICMP 'Parameter   Problem'  Error Message
 *                   (c) Flags                                       ICMP 'Parameter   Problem'  Error Message
 *                   (d) Fragment Offset                             ICMP 'Parameter   Problem'  Error Message
 *                   (e) Protocol                                    ICMP 'Unreachable Protocol' Error Message
 *                   (f) Options                                     ICMP 'Parameter   Problem'  Error Messages
 *                                                                           [see NetIPv4_RxPktValidateOpt()]
 *
 *               (9) RFC #1122, Section 3.2.1 requires that IPv4 packets with the following invalid IPv4 header
 *                   fields be "silently discarded" :
 *
 *                   (a) Version                                             RFC #1122, Section 3.2.1.1
 *                   (b) Check-Sum                                           RFC #1122, Section 3.2.1.2
 *
 *                   (c) Source Address
 *
 *                       (1) (A) RFC #1122, Section 3.2.1.3 states that "a host MUST silently discard
 *                               an incoming datagram containing an IPv4 source address that is invalid
 *                               by the rules of this section".
 *
 *                           (B) (1) MAY      be one of the following :
 *                                   (a) Configured host address             RFC #1122, Section 3.2.1.3.(1)
 *                                   (b) Localhost       address             RFC #1122, Section 3.2.1.3.(g)
 *                                                                               See also Note #9c2A
 *                                   (c) Link-local host address             RFC #3927, Section 2.1
 *                                                                               See also Note #9c2B
 *                                   (d) This       Host                     RFC #1122, Section 3.2.1.3.(a)
 *                                   (e) Specified  Host                     RFC #1122, Section 3.2.1.3.(b)
 *
 *                               (2) MUST NOT be one of the following :
 *                                   (a) Multicast  host address             RFC #1112, Section 7.2
 *                                   (b) Limited    Broadcast                RFC #1122, Section 3.2.1.3.(c)
 *                                   (c) Directed   Broadcast                RFC #1122, Section 3.2.1.3.(d)
 *                                   (d) Subnet     Broadcast                RFC #1122, Section 3.2.1.3.(e)
 *                                                                               See also Note #9c2C
 *
 *                       (2) (A) RFC #1122, Section 3.2.1.3.(g) states that the "internal host loopback
 *                               address ... MUST NOT appear outside a host".
 *
 *                               (1) However, this does NOT prevent the host loopback address from being
 *                                   used as an IPv4 packet's source address as long as BOTH the packet's
 *                                   source AND destination addresses are internal host addresses, either
 *                                   a configured host IP address or any host loopback address.
 *
 *                           (B) RFC #3927, Section 2.1 specifies the "IPv4 Link-Local address ... range
 *                               ... [as] inclusive" ... :
 *                               (1) "from 169.254.1.0" ...
 *                               (2) "to   169.254.254.255".
 *
 *                           (C) Although received packets' IPv4 source addresses SHOULD be checked for
 *                               invalid subnet broadcasts (see Note #9c1B2d), since multiple IPv4 host
 *                               addresses MAY be configured on any single network interface & since
 *                               each of these IPv4 host addresses may be configured on various networks
 *                               with various subnet masks, it is NOT possible to absolutely determine
 *                               if a received packet is a subnet broadcast for any specific network
 *                               on the network interface.
 *
 *                   (d) Destination Address
 *
 *                       (1) (A) RFC #1122, Section 3.2.1.3 states that "a host MUST silently discard
 *                               an incoming datagram that is not destined for" :
 *
 *                               (1) "(one of) the host's IPv4 address(es); or" ...
 *                               (2) "an IPv4 broadcast address valid for the connected network; or"
 *                               (3) "the address for a multicast group of which the host is a member
 *                                       on the incoming physical interface."
 *
 *                           (B) (1) MUST     be one of the following :
 *                                   (a) Configured host address             RFC #1122, Section 3.2.1.3.(1)
 *                                   (b) Multicast  host address             RFC #1122, Section 3.2.1.3.(3)
 *                                                                               See also Note #9d2A
 *                                   (c) Localhost                           RFC #1122, Section 3.2.1.3.(g)
 *                                                                               See also Note #9d2B
 *                                   (d) Limited    Broadcast                RFC #1122, Section 3.2.1.3.(c)
 *                                   (e) Directed   Broadcast                RFC #1122, Section 3.2.1.3.(d)
 *                                   (f) Subnet     Broadcast                RFC #1122, Section 3.2.1.3.(e)
 *
 *                               (2) MUST NOT be one of the following :
 *                                   (a) This       Host                     RFC #1122, Section 3.2.1.3.(a)
 *                                   (b) Specified  Host                     RFC #1122, Section 3.2.1.3.(b)
 *
 *                       (2) (A) RFC #1122, Section 3.2.1.3 states that "for most purposes, a datagram
 *                               addressed to a ... multicast destination is processed as if it had been
 *                               addressed to one of the host's IP addresses".
 *
 *                           (B) RFC #1122, Section 3.2.1.3.(g) states that the "internal host loopback
 *                               address ... MUST NOT appear outside a host".
 *
 *                           (C) RFC #3927, Section 2.8 states that "the 169.254/16 address prefix MUST
 *                               NOT be subnetted".  Therefore, link-local broadcast packets may ONLY be
 *                               received via directed broadcast (see Note #9d1B1e).
 *
 *                       (3) (A) RFC #1122, Section 3.3.6 states that :
 *
 *                               (1) "When a host sends a datagram to a link-layer broadcast address, the IP
 *                                       destination address MUST be a legal IP broadcast or IP multicast address."
 *
 *                               (2) "A host SHOULD silently discard a datagram that is received via a link-
 *                                       layer broadcast ... but does not specify an IP multicast or broadcast
 *                                       destination address."
 *
 *                           (B) (1) Therefore, any packet received as ... :
 *
 *                                   (a) ... an IPv4 broadcast destination address MUST also have been received
 *                                           as a link-layer broadcast.
 *
 *                                   (b) ... a link-layer broadcast MUST also be received as an IP broadcast or
 *                                           as an IPv4 multicast.
 *
 *                               (2) Thus, the following packets MUST be silently discarded if received as ... :
 *
 *                                   (a) ... a link-layer broadcast but not as an IPv4 broadcast or multicast; ...
 *                                   (b) ... a link-layer unicast   but     as an IPv4 broadcast.
 *
 *               (10) See 'net_ipv4.h  IPv4 ADDRESS DEFINES  Notes #2 & #3' for supported IPv4 addresses.
 *
 *               (11) (a) RFC #1122, Section 3.2.1.6 states that "the IP layer SHOULD pass received TOS values
 *                       up to the transport layer".
 *
 *                       NOT currently implemented. #### NET-812
 *
 *                   (b) RFC #1122, Section 3.2.1.8 states that "all IP options ... received in datagrams MUST
 *                       be passed to the transport layer (or to ICMP processing when the datagram is an ICMP
 *                       message).  The IPv4 and transport layer MUST each interpret those IPv4 options that they
 *                       understand and silently ignore the others".
 *
 *                       NOT currently implemented. #### NET-813
 *******************************************************************************************************/
static void NetIPv4_RxPktValidate(NET_BUF      *p_buf,
                                  NET_BUF_HDR  *p_buf_hdr,
                                  NET_IPv4_HDR *p_ip_hdr,
                                  RTOS_ERR     *p_err)
{
#ifdef  NET_IGMP_MODULE_EN
  CPU_BOOLEAN addr_host_grp_joined;
#endif
  NET_IF            *p_if;
  NET_IPv4_ADDR_OBJ *p_addr_obj;
  NET_IF_NBR        if_nbr;
  CPU_INT08U        ip_ver;
  CPU_INT08U        ip_hdr_len;
  CPU_INT16U        ip_hdr_len_tot;
  CPU_INT16U        ip_flags;
  CPU_INT16U        ip_frag_offset;
  CPU_INT16U        protocol_ix;
  CPU_BOOLEAN       ip_flag_reserved;
  CPU_BOOLEAN       ip_flag_dont_frag;
  CPU_BOOLEAN       ip_flag_frags_more;
  CPU_BOOLEAN       ip_chk_sum_valid;
  CPU_BOOLEAN       addr_host_src;
  CPU_BOOLEAN       addr_host_dest;
  CPU_BOOLEAN       rx_remote_host;
  CPU_BOOLEAN       rx_broadcast;
  CPU_BOOLEAN       ip_broadcast;
  CPU_BOOLEAN       ip_multicast;
#ifdef  NET_ICMPv4_MODULE_EN
  RTOS_ERR local_err;
#endif

  //                                                               --------------- CONVERT IPv4 FIELDS ----------------
  //                                                               See Note #3.
  NET_UTIL_VAL_COPY_GET_NET_16(&p_buf_hdr->IP_TotLen, &p_ip_hdr->TotLen);
  NET_UTIL_VAL_COPY_GET_NET_32(&p_buf_hdr->IP_AddrSrc, &p_ip_hdr->AddrSrc);
  NET_UTIL_VAL_COPY_GET_NET_32(&p_buf_hdr->IP_AddrDest, &p_ip_hdr->AddrDest);

  //                                                               ---------------- VALIDATE IPv4 VER -----------------
  ip_ver = p_ip_hdr->Ver_HdrLen & NET_IPv4_HDR_VER_MASK;        // See 'net_ipv4.h  IPv4 HEADER  Note #2'.
  ip_ver >>= NET_IPv4_HDR_VER_SHIFT;
  if (ip_ver != NET_IPv4_HDR_VER) {                             // Validate IP ver.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvVerCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               -------------- VALIDATE IPv4 HDR LEN ---------------
  //                                                               See 'net_ipv4.h  IPv4 HEADER  Note #2'.
  ip_hdr_len = p_ip_hdr->Ver_HdrLen   & NET_IPv4_HDR_LEN_MASK;
  ip_hdr_len_tot = (CPU_INT16U)ip_hdr_len * NET_IPv4_HDR_LEN_WORD_SIZE;
  p_buf_hdr->IP_HdrLen = (CPU_INT16U)ip_hdr_len_tot;            // See Note #3a.

  if (ip_hdr_len < NET_IPv4_HDR_LEN_MIN) {                      // If hdr len < min hdr len, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvLenCtr);
#ifdef  NET_ICMPv4_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetICMPv4_TxMsgErr(p_buf,
                       NET_ICMPv4_MSG_TYPE_PARAM_PROB,
                       NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR,
                       NET_ICMPv4_PTR_IX_IP_HDR_LEN,
                       &local_err);
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (ip_hdr_len > NET_IPv4_HDR_LEN_MAX) {                      // If hdr len > max hdr len, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvLenCtr);
#ifdef  NET_ICMPv4_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetICMPv4_TxMsgErr(p_buf,
                       NET_ICMPv4_MSG_TYPE_PARAM_PROB,
                       NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR,
                       NET_ICMPv4_PTR_IX_IP_HDR_LEN,
                       &local_err);
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               -------------- VALIDATE IPv4 TOT LEN ---------------
#if 0                                                           // See Note #3b.
  NET_UTIL_VAL_COPY_GET_NET_16(&p_buf_hdr->IP_TotLen, &p_ip_hdr->TotLen);
#endif
  if (p_buf_hdr->IP_TotLen < ip_hdr_len_tot) {                  // If IPv4 tot len < hdr len, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvTotLenCtr);
#ifdef  NET_ICMPv4_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetICMPv4_TxMsgErr(p_buf,
                       NET_ICMPv4_MSG_TYPE_PARAM_PROB,
                       NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR,
                       NET_ICMPv4_PTR_IX_IP_TOT_LEN,
                       &local_err);
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (p_buf_hdr->IP_TotLen > p_buf_hdr->DataLen) {              // If IPv4 tot len > rem pkt data len, ...
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvTotLenCtr);
#ifdef  NET_ICMPv4_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetICMPv4_TxMsgErr(p_buf,
                       NET_ICMPv4_MSG_TYPE_PARAM_PROB,
                       NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR,
                       NET_ICMPv4_PTR_IX_IP_TOT_LEN,
                       &local_err);
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                              // ... rtn err (see Note #4b2C).
    goto exit;
  }

  p_buf_hdr->DataLen = (NET_BUF_SIZE) p_buf_hdr->IP_TotLen;         // Trunc data len to IP tot len (see Note #4b2B1).
  p_buf_hdr->IP_DataLen = (CPU_INT16U)(p_buf_hdr->IP_TotLen - p_buf_hdr->IP_HdrLen);

  //                                                               --------------- VALIDATE IPv4 CHK SUM --------------
#ifdef  NET_IPV4_CHK_SUM_OFFLOAD_RX
  ip_chk_sum_valid = DEF_OK;
#else
  //                                                               See Note #5.
  ip_chk_sum_valid = NetUtil_16BitOnesCplChkSumHdrVerify(p_ip_hdr,
                                                         ip_hdr_len_tot);
#endif
  if (ip_chk_sum_valid != DEF_OK) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvChkSumCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
#if 0                                                           // Conv to host-order NOT necessary (see Note #5d).
  (void)NET_UTIL_VAL_GET_NET_16(&p_ip_hdr->ChkSum);
#endif

  //                                                               ------------------ CONVERT IP ID -------------------
  NET_UTIL_VAL_COPY_GET_NET_16(&p_buf_hdr->IP_ID, &p_ip_hdr->ID);

  //                                                               --------------- VALIDATE IPv4 FLAGS ----------------
  //                                                               See 'net_ipv4.h  IPv4 HEADER  Note #4'.
  NET_UTIL_VAL_COPY_GET_NET_16(&p_buf_hdr->IP_Flags_FragOffset, &p_ip_hdr->Flags_FragOffset);
  ip_flags = p_buf_hdr->IP_Flags_FragOffset & NET_IPv4_HDR_FLAG_MASK;
#if 1                                                           // Allow invalid reserved flag for rx'd datagrams.
  ip_flag_reserved = DEF_BIT_IS_SET_ANY(ip_flags, NET_IPv4_HDR_FLAG_RESERVED);
  if (ip_flag_reserved != DEF_NO) {                             // If reserved flag bit set, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvFlagsCtr);
#ifdef  NET_ICMPv4_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetICMPv4_TxMsgErr(p_buf,
                       NET_ICMPv4_MSG_TYPE_PARAM_PROB,
                       NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR,
                       NET_ICMPv4_PTR_IX_IP_FLAGS,
                       &local_err);
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
#endif

  //                                                               ---------------- VALIDATE IPv4 FRAG ----------------
  //                                                               See 'net_ipv4.h  IPv4 HEADER  Note #4'.
  ip_flag_dont_frag = DEF_BIT_IS_SET(ip_flags, NET_IPv4_HDR_FLAG_FRAG_DONT);
  ip_flag_frags_more = DEF_BIT_IS_SET(ip_flags, NET_IPv4_HDR_FLAG_FRAG_MORE);
  if (ip_flag_dont_frag != DEF_NO) {                            // If  'Don't Frag' flag set & ...
    if (ip_flag_frags_more != DEF_NO) {                         // ... 'More Frags' flag set, rtn err.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvFragCtr);
#ifdef  NET_ICMPv4_MODULE_EN
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetICMPv4_TxMsgErr(p_buf,
                         NET_ICMPv4_MSG_TYPE_PARAM_PROB,
                         NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR,
                         NET_ICMPv4_PTR_IX_IP_FRAG_OFFSET,
                         &local_err);
#endif
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    ip_frag_offset = p_buf_hdr->IP_Flags_FragOffset & NET_IPv4_HDR_FRAG_OFFSET_MASK;
    if (ip_frag_offset != NET_IPv4_HDR_FRAG_OFFSET_NONE) {      // ... frag offset != 0,      rtn err.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvFragCtr);
#ifdef  NET_ICMPv4_MODULE_EN
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetICMPv4_TxMsgErr(p_buf,
                         NET_ICMPv4_MSG_TYPE_PARAM_PROB,
                         NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR,
                         NET_ICMPv4_PTR_IX_IP_FRAG_OFFSET,
                         &local_err);
#endif
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  }

  if (ip_flag_frags_more != DEF_NO) {                           // If 'More Frags' set (see Note #6b1A)   ...
                                                                // ... & IPv4 data len NOT multiple of    ...
    if ((p_buf_hdr->IP_DataLen % NET_IPv4_FRAG_SIZE_UNIT) != 0u) {     // ... frag size units (see Note #6a), rtn err.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvFragCtr);
#ifdef  NET_ICMPv4_MODULE_EN
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetICMPv4_TxMsgErr(p_buf,
                         NET_ICMPv4_MSG_TYPE_PARAM_PROB,
                         NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR,
                         NET_ICMPv4_PTR_IX_IP_TOT_LEN,
                         &local_err);
#endif
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  }

  //                                                               -------------- VALIDATE IPv4 PROTOCOL --------------
  switch (p_ip_hdr->Protocol) {                                 // See 'net_ip.h  IP HEADER PROTOCOL FIELD DEFINES ...
    case NET_IP_HDR_PROTOCOL_ICMP:                              // ... Note #1.
    case NET_IP_HDR_PROTOCOL_IGMP:
    case NET_IP_HDR_PROTOCOL_UDP:
    case NET_IP_HDR_PROTOCOL_TCP:
      break;

    default:                                                    // See Note #7a.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvProtocolCtr);
#ifdef  NET_ICMPv4_MODULE_EN
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetICMPv4_TxMsgErr(p_buf,
                         NET_ICMPv4_MSG_TYPE_DEST_UNREACH,
                         NET_ICMPv4_MSG_CODE_DEST_PROTOCOL,
                         NET_ICMPv4_MSG_PTR_NONE,
                         &local_err);
#endif
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  //                                                               ---------------- VALIDATE IP ADDRS -----------------
#if 0                                                           // See Notes #3c & #3d.
  NET_UTIL_VAL_COPY_GET_NET_32(&p_buf_hdr->IP_AddrSrc, &p_ip_hdr->AddrSrc);
  NET_UTIL_VAL_COPY_GET_NET_32(&p_buf_hdr->IP_AddrDest, &p_ip_hdr->AddrDest);
#endif

  if_nbr = p_buf_hdr->IF_Nbr;                                   // Get pkt's rx'd IF.

  //                                                               Chk pkt rx'd to cfg'd host addr.
  if (if_nbr != NET_IF_NBR_LOCAL_HOST) {
    p_addr_obj = NetIPv4_GetAddrObjCfgdOnIF(if_nbr, p_buf_hdr->IP_AddrDest);
    addr_host_dest = (p_addr_obj != DEF_NULL) ? DEF_YES : DEF_NO;
  } else {
    p_addr_obj = DEF_NULL;
    addr_host_dest = NetIPv4_IsAddrHostCfgdHandler(p_buf_hdr->IP_AddrDest);
  }

  //                                                               Chk pkt rx'd via local or remote host.
  rx_remote_host = DEF_BIT_IS_SET(p_buf_hdr->Flags, NET_BUF_FLAG_RX_REMOTE);
  if (((if_nbr != NET_IF_NBR_LOCAL_HOST) && (rx_remote_host == DEF_NO))
      || ((if_nbr == NET_IF_NBR_LOCAL_HOST) && (rx_remote_host != DEF_NO))) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               -------------- VALIDATE IPv4 SRC ADDR --------------
  //                                                               See Note #9c.
  if (p_buf_hdr->IP_AddrSrc == NET_IPv4_ADDR_THIS_HOST) {       // Chk 'This Host' addr        (see Note #9c1B1d).
                                                                // Chk         localhost addrs (see Note #9c1B1b).
  } else if ((p_buf_hdr->IP_AddrSrc & NET_IPv4_ADDR_LOCAL_HOST_MASK_NET)
             == NET_IPv4_ADDR_LOCAL_HOST_NET     ) {
    //                                                             Chk invalid localhost addrs.
    if ((p_buf_hdr->IP_AddrSrc    & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)
        == (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
    if ((p_buf_hdr->IP_AddrSrc    & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)
        == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    if (rx_remote_host != DEF_NO) {                             // If localhost addr rx'd via remote host, ...
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                            // ... rtn err / discard pkt   (see Note #9c2A1).
      goto exit;
    }

    //                                                             Chk link-local addrs        (see Note #9c1B1c).
  } else if ((p_buf_hdr->IP_AddrSrc & NET_IPv4_ADDR_LOCAL_LINK_MASK_NET)
             == NET_IPv4_ADDR_LOCAL_LINK_NET     ) {
    //                                                             Chk invalid link-local addr (see Note #9c2B1).
    if ((p_buf_hdr->IP_AddrSrc < NET_IPv4_ADDR_LOCAL_LINK_HOST_MIN)
        || (p_buf_hdr->IP_AddrSrc > NET_IPv4_ADDR_LOCAL_LINK_HOST_MAX)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    //                                                             Chk invalid lim'd broadcast (see Note #9c1B2b).
  } else if (p_buf_hdr->IP_AddrSrc == NET_IPv4_ADDR_BROADCAST) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvAddrSrcCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  } else if ((p_buf_hdr->IP_AddrSrc   & NET_IPv4_ADDR_CLASS_A_MASK) == NET_IPv4_ADDR_CLASS_A) {
    //                                                             Chk Class-A broadcast       (see Note #9c1B2c).
    if ((p_buf_hdr->IP_AddrSrc      & NET_IPv4_ADDR_CLASS_A_MASK_HOST)
        == (NET_IPv4_ADDR_BROADCAST   & NET_IPv4_ADDR_CLASS_A_MASK_HOST)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  } else if ((p_buf_hdr->IP_AddrSrc & NET_IPv4_ADDR_CLASS_B_MASK) == NET_IPv4_ADDR_CLASS_B) {
    //                                                             Chk Class-B broadcast       (see Note #9c1B2c).
    if ((p_buf_hdr->IP_AddrSrc    & NET_IPv4_ADDR_CLASS_B_MASK_HOST)
        == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_CLASS_B_MASK_HOST)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  } else if ((p_buf_hdr->IP_AddrSrc & NET_IPv4_ADDR_CLASS_C_MASK) == NET_IPv4_ADDR_CLASS_C) {
    //                                                             Chk Class-C broadcast       (see Note #9c1B2c).
    if ((p_buf_hdr->IP_AddrSrc    & NET_IPv4_ADDR_CLASS_C_MASK_HOST)
        == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_CLASS_C_MASK_HOST)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  } else if ((p_buf_hdr->IP_AddrSrc & NET_IPv4_ADDR_CLASS_D_MASK) == NET_IPv4_ADDR_CLASS_D) {
    //                                                             Chk Class-D multicast       (see Note #9c1B2a).
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvAddrSrcCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  } else {                                                      // Discard invalid addr class  (see Note #10).
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvAddrSrcCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               Chk subnet  broadcast       (see Note #9c1B2d).
#if 0                                                           // See Note #9c2C.
  if (p_addr_obj != DEF_NULL) {
    if ((p_buf_hdr->IP_AddrSrc & p_addr_obj->AddrHostSubnetMask)
        == p_addr_obj->AddrHostSubnetNet ) {
      if ((p_buf_hdr->IP_AddrSrc    & p_addr_obj->AddrHostSubnetMaskHost)
          == (NET_IPv4_ADDR_BROADCAST  & p_addr_obj->AddrHostSubnetMaskHost)) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvAddrSrcCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
    }
  }
#endif

  //                                                               ------------- VALIDATE IP DEST ADDR ------------
  //                                                               See Note #9d.
  ip_broadcast = DEF_NO;
  ip_multicast = DEF_NO;

  //                                                               Chk this host's cfg'd addr    (see Note #9d1B1a).
  if (p_addr_obj != DEF_NULL) {
    if (p_addr_obj->AddrHost == NET_IPv4_ADDR_NONE) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    if (p_buf_hdr->IP_AddrDest != p_addr_obj->AddrHost) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    //                                                             Chk this host's cfg'd addr(s) [see Note #9d1B1a].
  } else if (addr_host_dest == DEF_YES) {
    addr_host_src = NetIPv4_IsAddrHostHandler(p_buf_hdr->IP_AddrSrc);
    if (addr_host_src != DEF_YES) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    //                                                             Chk         localhost         (see Note #9d1B1c).
  } else if ((p_buf_hdr->IP_AddrDest & NET_IPv4_ADDR_LOCAL_HOST_MASK_NET)
             == NET_IPv4_ADDR_LOCAL_HOST_NET     ) {
    //                                                             Chk invalid localhost addrs.
    if ((p_buf_hdr->IP_AddrDest    & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)
        == (NET_IPv4_ADDR_THIS_HOST  & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    if (rx_remote_host != DEF_NO) {                                 // If localhost addr rx'd via remote host, ...
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                                // ... rtn err / discard pkt     (see Note #9d2B).
      goto exit;
    }

    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.RxDestLocalHostCtr);

    //                                                             Chk invalid 'This Host'       (see Note #9d1B2a).
  } else if (p_buf_hdr->IP_AddrDest == NET_IPv4_ADDR_THIS_HOST) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;

#ifdef  NET_IGMP_MODULE_EN
    //                                                             Chk joined multicast addr(s)  [see Note #9d1B1b].
  } else if ((p_buf_hdr->IP_AddrDest & NET_IPv4_ADDR_CLASS_D_MASK) == NET_IPv4_ADDR_CLASS_D) {
    addr_host_grp_joined = NetIGMP_IsGrpJoinedOnIF(if_nbr, p_buf_hdr->IP_AddrDest);
    if (addr_host_grp_joined != DEF_YES) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.RxDestMcastCtr);
    ip_multicast = DEF_YES;
#endif
  } else {
    //                                                             Chk lim'd broadcast           (see Note #9d1B1d).
    if (p_buf_hdr->IP_AddrDest == NET_IPv4_ADDR_BROADCAST) {
      ip_broadcast = DEF_YES;
    } else if ((p_buf_hdr->IP_AddrDest & NET_IPv4_ADDR_LOCAL_LINK_MASK_NET)
               == NET_IPv4_ADDR_LOCAL_LINK_NET     ) {
      //                                                           Chk link-local broadcast      (see Note #9d2C).
      if ((p_buf_hdr->IP_AddrDest & NET_IPv4_ADDR_LOCAL_LINK_MASK_HOST)
          != (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_LOCAL_LINK_MASK_HOST)) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      ip_broadcast = DEF_YES;
    } else if ((p_buf_hdr->IP_AddrDest & NET_IPv4_ADDR_CLASS_A_MASK) == NET_IPv4_ADDR_CLASS_A) {
      //                                                           Chk Class-A 'This Host'       (see Note #9d1B2b).
      if ((p_buf_hdr->IP_AddrDest   & NET_IPv4_ADDR_CLASS_A_MASK_HOST)
          == (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_CLASS_A_MASK_HOST)) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      //                                                           Chk Class-A broadcast         (see Note #9d1B1e).
      if ((p_buf_hdr->IP_AddrDest   & NET_IPv4_ADDR_CLASS_A_MASK_HOST)
          == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_CLASS_A_MASK_HOST)) {
        ip_broadcast = DEF_YES;
      }
    } else if ((p_buf_hdr->IP_AddrDest & NET_IPv4_ADDR_CLASS_B_MASK) == NET_IPv4_ADDR_CLASS_B) {
      //                                                           Chk Class-B 'This Host'       (see Note #9d1B2b).
      if ((p_buf_hdr->IP_AddrDest    & NET_IPv4_ADDR_CLASS_B_MASK_HOST)
          == (NET_IPv4_ADDR_THIS_HOST  & NET_IPv4_ADDR_CLASS_B_MASK_HOST)) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      //                                                           Chk Class-B broadcast         (see Note #9d1B1e).
      if ((p_buf_hdr->IP_AddrDest   & NET_IPv4_ADDR_CLASS_B_MASK_HOST)
          == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_CLASS_B_MASK_HOST)) {
        ip_broadcast = DEF_YES;
      }
    } else if ((p_buf_hdr->IP_AddrDest & NET_IPv4_ADDR_CLASS_C_MASK) == NET_IPv4_ADDR_CLASS_C) {
      //                                                           Chk Class-C 'This Host'       (see Note #9d1B2b).
      if ((p_buf_hdr->IP_AddrDest    & NET_IPv4_ADDR_CLASS_C_MASK_HOST)
          == (NET_IPv4_ADDR_THIS_HOST  & NET_IPv4_ADDR_CLASS_C_MASK_HOST)) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      //                                                           Chk Class-C broadcast         (see Note #9d1B1e).
      if ((p_buf_hdr->IP_AddrDest   & NET_IPv4_ADDR_CLASS_C_MASK_HOST)
          == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_CLASS_C_MASK_HOST)) {
        ip_broadcast = DEF_YES;
      }
    } else {                                                        // Discard invalid addr class    (see Note #10).
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    //                                                             Chk subnet broadcast          (see Note #9d1B1f).
    if (if_nbr != NET_IF_NBR_LOCAL_HOST) {                          // If pkt rx'd via remote host, ...
      p_if = NetIF_Get(if_nbr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }

      SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
        if ((p_buf_hdr->IP_AddrDest & p_addr_obj->AddrHostSubnetMask)
            == p_addr_obj->AddrHostSubnetNet ) {
          if ((p_buf_hdr->IP_AddrDest   & p_addr_obj->AddrHostSubnetMaskHost)
              == (NET_IPv4_ADDR_BROADCAST  & p_addr_obj->AddrHostSubnetMaskHost)) {
            ip_broadcast = DEF_YES;
            break;
          }
        }
      }
    }

    //                                                             If NOT any this host's addrs (see Note #9d1A1) &
    if (ip_broadcast != DEF_YES) {                                  // .. NOT any broadcast   addrs (see Note #9d1A2);
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                                // .. rtn err / discard pkt     (see Note #9d1A).
      goto exit;
    }

    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.RxDestBcastCtr);
  }

  //                                                               Chk invalid    broadcast     (see Note #9d3).
  rx_broadcast = DEF_BIT_IS_SET(p_buf_hdr->Flags, NET_BUF_FLAG_RX_BROADCAST);
  if (rx_broadcast == DEF_YES) {                                    // If          IF   broadcast rx'd, ...
    if ((ip_broadcast != DEF_YES)                                   // ... BUT NOT IPv4 broadcast rx'd  ...
        && (ip_multicast != DEF_YES)) {                             // ... AND NOT IPv4 multicast rx'd; ...
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxDestBcastCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                                // ... rtn err / discard pkt    (see Note #9d3B2a).
      goto exit;
    }
  }

  //                                                               ---------------- VALIDATE IPv4 OPTS ----------------
  if (ip_hdr_len_tot > NET_IPv4_HDR_SIZE_MIN) {                 // If hdr len > min, ...
                                                                // ... validate/process IPv4 opts (see Note #11b).
    NetIPv4_RxPktValidateOpt(p_buf,
                             p_buf_hdr,
                             p_ip_hdr,
                             ip_hdr_len_tot,
                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  //                                                               ----------------- UPDATE BUF CTRLS -----------------
#if 0                                                           // See Note #3a.
  p_buf_hdr->IP_HdrLen = ip_hdr_len_tot;
#endif

  p_buf_hdr->DataLen -= (NET_BUF_SIZE) p_buf_hdr->IP_HdrLen;
  protocol_ix = (CPU_INT16U)(p_buf_hdr->IP_HdrIx + p_buf_hdr->IP_HdrLen);
  switch (p_ip_hdr->Protocol) {
    case NET_IP_HDR_PROTOCOL_ICMP:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_ICMP_V4;
      p_buf_hdr->ProtocolHdrTypeNetSub = NET_PROTOCOL_TYPE_ICMP_V4;
      p_buf_hdr->ICMP_MsgIx = protocol_ix;
      break;

#ifdef  NET_IGMP_MODULE_EN
    case NET_IP_HDR_PROTOCOL_IGMP:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IGMP;
      p_buf_hdr->ProtocolHdrTypeNetSub = NET_PROTOCOL_TYPE_IGMP;
      p_buf_hdr->IGMP_MsgIx = protocol_ix;
      break;
#endif

    case NET_IP_HDR_PROTOCOL_UDP:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_UDP_V4;
      p_buf_hdr->ProtocolHdrTypeTransport = NET_PROTOCOL_TYPE_UDP_V4;
      p_buf_hdr->TransportHdrIx = protocol_ix;
      break;

#ifdef  NET_TCP_MODULE_EN
    case NET_IP_HDR_PROTOCOL_TCP:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_TCP_V4;
      p_buf_hdr->ProtocolHdrTypeTransport = NET_PROTOCOL_TYPE_TCP_V4;
      p_buf_hdr->TransportHdrIx = protocol_ix;
      break;
#endif

    default:                                                    // See Note #7b.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvProtocolCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv4_RxPktValidateOpt()
 *
 * @brief    (1) Validate & process received packet's IPv4 options :
 *
 *           - (a) Copy            IPv4 options into new buffer    See 'NetIPv4_RxPktValidate()  Note #1bC'
 *           - (b) Decode/validate IPv4 options
 *
 * @param    p_buf               Pointer to network buffer that received IPv4 packet.
 *
 * @param    p_buf_hdr           Pointer to network buffer header.
 *
 * @param    p_ip_hdr            Pointer to received packet's IPv4 header.
 *
 * @param    ip_hdr_len_size     Length  of received packet's IPv4 header.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @note     (3) RFC #1122, Section 3.2.1.8 lists the processing of the following IPv4 options as optional :
 *
 *                                   - (a) Record Route                            RFC #1122, Section 3.2.1.8.(d)
 *                                   - (b) Internet Timestamp                      RFC #1122, Section 3.2.1.8.(e)
 *
 * @note     (4) Each option's length MUST be multiples of NET_IP_HDR_OPT_SIZE_WORD octets so that "the
 *                               beginning of a subsequent option [aligns] on a 32-bit boundary" (RFC #791, Section 3.1
 *                               'Options : No Operation').
 *
 * @note     (5) RFC #1122, Section 3.2.1.8.(c) prohibits "an IP header" from transmitting with "more
 *                               than one Source Route option".
 *******************************************************************************************************/
static void NetIPv4_RxPktValidateOpt(NET_BUF      *p_buf,
                                     NET_BUF_HDR  *p_buf_hdr,
                                     NET_IPv4_HDR *p_ip_hdr,
                                     CPU_INT08U   ip_hdr_len_size,
                                     RTOS_ERR     *p_err)
{
  NET_IF      *p_if = DEF_NULL;
  NET_BUF     *p_opt_buf = DEF_NULL;
  NET_BUF_HDR *p_opt_buf_hdr = DEF_NULL;
  CPU_INT08U  *p_opts = DEF_NULL;
  CPU_INT08U  opt_list_len_size = 0u;
  CPU_INT08U  opt_list_len_rem = 0u;
  CPU_INT08U  opt_len = 0u;
  CPU_INT08U  opt_nbr_src_routes = 0u;
#ifdef  NET_ICMPv4_MODULE_EN
  CPU_INT08U opt_ix_err = 0u;
#endif
  CPU_INT16U   opt_ix = 0u;
  NET_BUF_SIZE opt_buf_size = 0u;
  CPU_BOOLEAN  opt_err = DEF_NO;
  CPU_BOOLEAN  opt_list_end = DEF_NO;
  NET_IF_NBR   if_nbr = NET_IF_NBR_NONE;
  RTOS_ERR     local_err;
  RTOS_ERR     err_rtn;

  opt_list_len_size = ip_hdr_len_size - NET_IPv4_HDR_SIZE_MIN;  // Calc opt list len size.

  //                                                               ---------- VALIDATE IPv4 HDR OPT LIST SIZE ---------

  if (opt_list_len_size > NET_IPv4_HDR_OPT_SIZE_MAX) {          // If tot opt len > max opt size,           rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
#ifdef  NET_ICMPv4_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetICMPv4_TxMsgErr(p_buf,
                       NET_ICMPv4_MSG_TYPE_PARAM_PROB,
                       NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR,
                       NET_ICMPv4_PTR_IX_IP_OPTS,
                       &local_err);
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
  //                                                               If tot opt len NOT multiple of opt size, rtn err.
  if ((opt_list_len_size % NET_IPv4_HDR_OPT_SIZE_WORD) != 0u) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
#ifdef  NET_ICMPv4_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetICMPv4_TxMsgErr(p_buf,
                       NET_ICMPv4_MSG_TYPE_PARAM_PROB,
                       NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR,
                       NET_ICMPv4_PTR_IX_IP_OPTS,
                       &local_err);
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------------- COPY IPv4 OPTS INTO BUF --------------
  if_nbr = p_buf_hdr->IF_Nbr;
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  //                                                               Get IPv4 opt rx buf.
  opt_ix = NET_IPv4_OPT_IX_RX;
  p_opt_buf = NetBuf_Get(if_nbr,
                         NET_TRANSACTION_RX,
                         opt_list_len_size,
                         opt_ix,
                         0,
                         NET_BUF_FLAG_NONE,
                         &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxOptsBufNoneAvailCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               Get IPv4 opt rx buf data area.
  p_opt_buf->DataPtr = NetBuf_GetDataPtr(p_if,
                                         NET_TRANSACTION_RX,
                                         opt_list_len_size,
                                         opt_ix,
                                         0,
                                         &opt_buf_size,
                                         0,
                                         &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    NetBuf_Free(p_opt_buf);
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxOptsBufNoneAvailCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  NetBuf_DataWr(p_opt_buf,                                       // Copy IP opts from rx'd pkt to IP opt buf.
                opt_ix,
                opt_list_len_size,
                (CPU_INT08U *)&p_ip_hdr->Opts[0]);

  //                                                               Init IPv4 opt buf ctrls.
  p_buf_hdr->IP_OptPtr = p_opt_buf;
  p_opt_buf_hdr = &p_opt_buf->Hdr;
  p_opt_buf_hdr->IP_HdrIx = opt_ix;
  p_opt_buf_hdr->IP_HdrLen = opt_list_len_size;
  p_opt_buf_hdr->TotLen = p_opt_buf_hdr->IP_HdrLen;
  p_opt_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V4_OPT;

  //                                                               ------------ DECODE/VALIDATE IPv4 OPTS -------------
  opt_err = DEF_NO;
  opt_list_end = DEF_NO;
  opt_nbr_src_routes = 0u;

  p_opts = (CPU_INT08U *)&p_opt_buf->DataPtr[opt_ix];
  opt_list_len_rem = opt_list_len_size;

  while (opt_list_len_rem > 0) {                                // Process each opt in list (see Note #4).
    switch (*p_opts) {
      case NET_IPv4_HDR_OPT_END_LIST:                           // ------------------- END OPT LIST -------------------
        opt_list_end = DEF_YES;                                 // Mark end of opt list.
        opt_len = NET_IPv4_HDR_OPT_SIZE_WORD;
        break;

      case NET_IPv4_HDR_OPT_NOP:                                // --------------------- NOP OPT ----------------------
        opt_len = NET_IPv4_HDR_OPT_SIZE_WORD;
        break;

      case NET_IPv4_HDR_OPT_ROUTE_SRC_LOOSE:                    // ---------------- SRC/REC ROUTE OPTS ----------------
      case NET_IPv4_HDR_OPT_ROUTE_SRC_STRICT:
      case NET_IPv4_HDR_OPT_ROUTE_REC:
        if (opt_list_end == DEF_NO) {
          if (opt_nbr_src_routes < 1) {
            opt_nbr_src_routes++;
            opt_err = NetIPv4_RxPktValidateOptRoute(p_buf_hdr, p_opts, opt_list_len_rem, &opt_len, &err_rtn);
          } else {                                              // If > 1 src route opt, rtn err (see Note #5).
            RTOS_ERR_SET(err_rtn, RTOS_ERR_RX);
            opt_err = DEF_YES;
          }
        } else {                                                // If opt found AFTER end of opt list, rtn err.
          RTOS_ERR_SET(err_rtn, RTOS_ERR_RX);
          opt_err = DEF_YES;
        }
        break;

      case NET_IPv4_HDR_OPT_TS:                                 // --------------------- TS OPTS ----------------------
        if (opt_list_end == DEF_NO) {
          opt_err = NetIPv4_RxPktValidateOptTS(p_buf_hdr, p_opts, opt_list_len_rem, &opt_len, &err_rtn);
        } else {                                                // If opt found AFTER end of opt list, rtn err.
          RTOS_ERR_SET(err_rtn, RTOS_ERR_RX);
          opt_err = DEF_YES;
        }
        break;
      //                                                           --------------- UNSUPPORTED IP OPTS ----------------
      case NET_IPv4_HDR_OPT_SECURITY:
      case NET_IPv4_HDR_OPT_SECURITY_EXTENDED:
      default:                                                  // ------------------- INVALID OPTS -------------------
        opt_len = *(p_opts + 1);                                // Ignore unknown opts.
        break;
    }

    if (opt_err == DEF_NO) {
      if (opt_list_len_rem >= opt_len) {
        opt_list_len_rem -= opt_len;
        p_opts += opt_len;
      } else {                                                  // If rem opt list len NOT multiple of opt size, ...
        RTOS_ERR_SET(err_rtn, RTOS_ERR_RX);                     // ... rtn err.
        opt_err = DEF_YES;
      }
    }

    if (opt_err == DEF_YES) {                                   // If ANY opt errs, tx ICMP err msg.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
#ifdef  NET_ICMPv4_MODULE_EN
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      opt_ix_err = NET_ICMPv4_PTR_IX_IP_OPTS + (opt_list_len_size - opt_list_len_rem);
      NetICMPv4_TxMsgErr(p_buf,
                         NET_ICMPv4_MSG_TYPE_PARAM_PROB,
                         NET_ICMPv4_MSG_CODE_PARAM_PROB_IP_HDR,
                         opt_ix_err,
                         &local_err);
#endif
      RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(err_rtn));
      goto exit;
    }
  }

  PP_UNUSED_PARAM(p_buf);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv4_RxPktValidateOptRoute()
 *
 * @brief    (1) Validate & process Source Route options :
 *
 *           - (a) Convert ALL Source Route IPv4 addresses from network-order to host-order
 *           - (b) Add this host's IPv4 address to Source Route
 *
 *       - (2) See 'net_ipv4.h  IPv4 SOURCE ROUTE OPTION DATA TYPE' for Source Route options summary.
 *
 * @param    p_buf_hdr           Pointer to network buffer header.
 *
 * @param    p_opts              Pointer to Source Route option.
 *
 * @param    opt_list_len_rem    Remaining option list length (in octets).
 *
 * @param    p_opt_len           Pointer to variable that will receive the Source Route option length
 *                               (in octets).
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : DEF_NO,  NO Source Route option error.
 *               DEF_YES, otherwise.
 *
 * Note(s)     : (3) If Source Route option appends this host's IPv4 address to the source route, the IPv4
 *                   header check-sum is NOT re-calculated since the check-sum was previously validated
 *                   in NetIPv4_RxPktValidate() & is NOT required for further validation or processing.
 *
 *               (4) Default case already invalidated earlier in this function.  However, the default
 *                   case is included as an extra precaution in case any of the IPv4 receive options is
 *                   incorrectly modified.
 *******************************************************************************************************/
static CPU_BOOLEAN NetIPv4_RxPktValidateOptRoute(NET_BUF_HDR *p_buf_hdr,
                                                 CPU_INT08U  *p_opts,
                                                 CPU_INT08U  opt_list_len_rem,
                                                 CPU_INT08U  *p_opt_len,
                                                 RTOS_ERR    *p_err)
{
  NET_IPv4_OPT_SRC_ROUTE *p_opt_route;
  NET_IPv4_ADDR          opt_addr;
  CPU_INT08U             opt_ptr;
  CPU_INT08U             opt_ix;
  CPU_BOOLEAN            is_err = DEF_YES;

  p_opt_route = (NET_IPv4_OPT_SRC_ROUTE *)p_opts;

  //                                                               ----------------- VALIDATE OPT LEN -----------------
  if (p_opt_route->Ptr > p_opt_route->Len) {                    // If ptr exceeds opt len,              rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (p_opt_route->Len > opt_list_len_rem) {                    // If opt len exceeds rem opt len,      rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if ((p_opt_route->Len % NET_IPv4_HDR_OPT_SIZE_WORD) != 0u) {      // If opt len NOT multiple of opt size, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  opt_ptr = NET_IPv4_OPT_ROUTE_PTR_ROUTE;
  opt_ix = 0u;

  //                                                               --------------- CONVERT TO HOST-ORDER --------------
  while (opt_ptr < p_opt_route->Ptr) {                          // Convert ALL src route addrs to host-order.
    NET_UTIL_VAL_COPY_GET_NET_32(&opt_addr, &p_opt_route->Route[opt_ix]);
    NET_UTIL_VAL_COPY_32(&p_opt_route->Route[opt_ix], &opt_addr);
    opt_ptr += NET_IPv4_HDR_OPT_SIZE_WORD;
    opt_ix++;
  }

  //                                                               ------------------- INSERT ROUTE -------------------
  if (p_opt_route->Ptr < p_opt_route->Len) {                    // If ptr < len, append this host addr to src route.
    switch (*p_opts) {
      case NET_IPv4_HDR_OPT_ROUTE_SRC_LOOSE:
      case NET_IPv4_HDR_OPT_ROUTE_REC:
        opt_addr = p_buf_hdr->IP_AddrDest;
        NET_UTIL_VAL_COPY_SET_HOST_32(&p_opt_route->Route[opt_ix], &opt_addr);
        p_opt_route->Ptr += NET_IPv4_HDR_OPT_SIZE_WORD;
        break;

      case NET_IPv4_HDR_OPT_ROUTE_SRC_STRICT:
        break;

      default:                                                  // See Note #4.
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
    }
  }

  *p_opt_len = p_opt_route->Len;                                // Rtn src route opt len.

  is_err = DEF_NO;

exit:
  return (is_err);
}

/****************************************************************************************************//**
 *                                       NetIPv4_RxPktValidateOptTS()
 *
 * @brief    (1) Validate & process Internet Timestamp options :
 *
 *           - (a) Convert ALL Internet Timestamps & Source Route IPv4 addresses from network-order
 *                       to host-order
 *           - (b) Add current Internet Timestamp & this host's   IPv4 address to Internet Timestamp
 *
 *       - (2) See 'net_ipv4.h  IPv4 INTERNET TIMESTAMP OPTION DATA TYPE' for Internet Timestamp options
 *                   summary.
 *
 * @param    p_buf_hdr           Pointer to network buffer header.
 *
 * @param    p_opts              Pointer to Internet Timestamp option.
 *
 * @param    opt_list_len_rem    Remaining option list length (in octets).
 *
 * @param    p_opt_len           Pointer to variable that will return the Internet Timestamp option length
 *
 * @param    ---------         (in octets).
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_NO,  NO Internet Timestamp option error.
 *           DEF_YES, otherwise.
 *
 * @note     (3) If Internet Timestamp option appends the current Internet Timestamp &/or this host's
 *               IP address to the Internet Timestamp, the IPv4 header check-sum is NOT re-calculated
 *               since the check-sum was previously validated in NetIPv4_RxPktValidate() & is NOT
 *               required for further validation or processing.
 *******************************************************************************************************/
static CPU_BOOLEAN NetIPv4_RxPktValidateOptTS(NET_BUF_HDR *p_buf_hdr,
                                              CPU_INT08U  *p_opts,
                                              CPU_INT08U  opt_list_len_rem,
                                              CPU_INT08U  *p_opt_len,
                                              RTOS_ERR    *p_err)
{
  NET_IPv4_OPT_TS       *p_opt_ts;
  NET_IPv4_OPT_TS_ROUTE *p_opt_ts_route;
  NET_IPv4_ROUTE_TS     *p_route_ts;
  CPU_INT08U            opt_ptr;
  CPU_INT08U            opt_ix;
  CPU_INT08U            opt_ts_flags;
  CPU_INT08U            opt_ts_ovf;
  NET_TS                opt_ts;
  NET_IPv4_ADDR         opt_addr;
  CPU_BOOLEAN           is_err = DEF_YES;

  p_opt_ts = (NET_IPv4_OPT_TS *)p_opts;

  //                                                               ----------------- VALIDATE OPT LEN -----------------
  if (p_opt_ts->Ptr > p_opt_ts->Len) {                          // If ptr exceeds opt len,         rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (p_opt_ts->Len > opt_list_len_rem) {                       // If opt len exceeds rem opt len, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  opt_ptr = NET_IPv4_OPT_TS_PTR_TS;
  opt_ix = 0u;
  opt_ts_flags = p_opt_ts->Ovf_Flags & NET_IPv4_OPT_TS_FLAG_MASK;

  switch (opt_ts_flags) {
    case NET_IPv4_OPT_TS_FLAG_TS_ONLY:
      if ((p_opt_ts->Len % NET_IPv4_HDR_OPT_SIZE_WORD) != 0u) {        // If opt len NOT multiple of opt size, rtn err.
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }

      //                                                           --------------- CONVERT TO HOST-ORDER --------------
      while (opt_ptr < p_opt_ts->Ptr) {                         // Convert ALL TS's to host-order.
        NET_UTIL_VAL_COPY_GET_NET_32(&opt_ts, &p_opt_ts->TS[opt_ix]);
        NET_UTIL_VAL_COPY_32(&p_opt_ts->TS[opt_ix], &opt_ts);
        opt_ptr += NET_IPv4_HDR_OPT_SIZE_WORD;
        opt_ix++;
      }

      //                                                           --------------------- INSERT TS --------------------
      if (p_opt_ts->Ptr < p_opt_ts->Len) {                      // If ptr < len, append ts to list.
        opt_ts = NetUtil_TS_Get();
        NET_UTIL_VAL_COPY_SET_HOST_32(&p_opt_ts->TS[opt_ix], &opt_ts);
        p_opt_ts->Ptr += NET_IPv4_HDR_OPT_SIZE_WORD;
      } else {                                                  // Else inc & chk ovf ctr.
        opt_ts_ovf = p_opt_ts->Ovf_Flags & NET_IPv4_OPT_TS_OVF_MASK;
        opt_ts_ovf >>= NET_IPv4_OPT_TS_OVF_SHIFT;
        opt_ts_ovf++;

        if (opt_ts_ovf > NET_IPv4_OPT_TS_OVF_MAX) {             // If ovf ctr ovfs, rtn err.
          NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }
      }
      break;

    case NET_IPv4_OPT_TS_FLAG_TS_ROUTE_REC:
    case NET_IPv4_OPT_TS_FLAG_TS_ROUTE_SPEC:
      //                                                           If opt len NOT multiple of opt size, rtn err.
      if ((p_opt_ts->Len % NET_IPv4_OPT_TS_ROUTE_SIZE) != NET_IPv4_HDR_OPT_SIZE_WORD) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }

      p_opt_ts_route = (NET_IPv4_OPT_TS_ROUTE *)p_opts;
      p_route_ts = &p_opt_ts_route->Route_TS[0];

      //                                                           --------------- CONVERT TO HOST-ORDER --------------
      while (opt_ptr < p_opt_ts_route->Ptr) {                   // Convert ALL src route addrs & ts's to host-order.
        NET_UTIL_VAL_COPY_GET_NET_32(&opt_addr, &p_route_ts->Route[opt_ix]);
        NET_UTIL_VAL_COPY_GET_NET_32(&opt_ts, &p_route_ts->TS[opt_ix]);
        NET_UTIL_VAL_COPY_32(&p_route_ts->Route[opt_ix], &opt_addr);
        NET_UTIL_VAL_COPY_32(&p_route_ts->TS[opt_ix], &opt_ts);
        opt_ptr += NET_IPv4_OPT_TS_ROUTE_SIZE;
        opt_ix++;
      }

      //                                                           ---------------- INSERT SRC ROUTE/TS ---------------
      if (p_opt_ts_route->Ptr < p_opt_ts_route->Len) {          // If ptr < len, append src route addr & ts to list.
        opt_addr = p_buf_hdr->IP_AddrDest;
        opt_ts = NetUtil_TS_Get();
        NET_UTIL_VAL_COPY_SET_HOST_32(&p_route_ts->Route[opt_ix], &opt_addr);
        NET_UTIL_VAL_COPY_SET_HOST_32(&p_route_ts->TS[opt_ix], &opt_ts);
        p_opt_ts_route->Ptr += NET_IPv4_OPT_TS_ROUTE_SIZE;
      } else {                                                  // Else inc & chk ovf ctr.
        opt_ts_ovf = p_opt_ts->Ovf_Flags & NET_IPv4_OPT_TS_OVF_MASK;
        opt_ts_ovf >>= NET_IPv4_OPT_TS_OVF_SHIFT;
        opt_ts_ovf++;

        if (opt_ts_ovf > NET_IPv4_OPT_TS_OVF_MAX) {             // If ovf ctr ovfs, rtn err.
          NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }
      }
      break;

    default:                                                    // If invalid/unknown TS flag, rtn err.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvOptsCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  *p_opt_len = p_opt_ts->Len;                                   // Rtn TS opt len.

  is_err = DEF_NO;

exit:
  return (is_err);
}

/****************************************************************************************************//**
 *                                           NetIPv4_RxPktFragReasm()
 *
 * @brief    (1) Reassemble any IPv4 datagram fragments :
 *
 *           - (a) Determine if received IPv4 packet is a fragment
 *           - (b) Reassemble IPv4 fragments, when possible
 *
 *       - (2) (a) Received fragments are reassembled by sorting datagram fragments into fragment lists
 *                   (also known as 'Fragmented Datagrams') grouped by the following IPv4 header fields :
 *
 *               - (1) Source      Address
 *               - (2) Destination Address
 *               - (3) Identification
 *               - (4) Protocol
 *
 *                   See also Note #3a.
 *
 *           - (b) Fragment lists are linked to form a list of Fragment Lists/Fragmented Datagrams.
 *
 *               - (1) In the diagram below, ... :
 *
 *                   - (A) The top horizontal row  represents the list of fragment lists.
 *
 *                   - (B) Each    vertical column represents the fragments in the same fragment list/
 *                           Fragmented Datagram.
 *
 *                   - (C) (1) 'NetIPv4_FragReasmListsHead' points to the head of the Fragment Lists;
 *                       - (2) 'NetIPv4_FragReasmListsTail' points to the tail of the Fragment Lists.
 *
 *                   - (D) Fragment buffers' 'PrevPrimListPtr' & 'NextPrimListPtr' doubly-link each fragment
 *                           list's head buffer to form the list of Fragment Lists.
 *
 *                   - (E) Fragment buffer's 'PrevBufPtr'      & 'NextBufPtr'      doubly-link each fragment
 *                           in a fragment list.
 *
 *               - (2) (A) For each received fragment, all fragment lists are searched in order to insert the
 *                           fragment into the appropriate fragment list--i.e. the fragment list with identical
 *                           fragment list IPv4 header field values (see Note #2a).
 *
 *                   - (B) If a received fragment is the first fragment with its specific fragment list IPv4
 *                           header field values, the received fragment starts a new fragment list which is
 *                           added at the tail of the Fragment Lists.
 *
 *                           See also Note #3b2.
 *
 *                   - (C) To expedite faster fragment list searches :
 *
 *                       - (1) (a) Fragment lists are added             at the tail of the Fragment Lists;
 *                           - (b) Fragment lists are searched starting at the head of the Fragment Lists.
 *
 *                       - (2) As fragments are received & processed into fragment lists, older fragment
 *                               lists migrate to the head of the Fragment Lists.  Once a fragment list is
 *                               reassembled or discarded, it is removed from the Fragment Lists.
 *
 *               - (3) Fragment buffer size is irrelevant & ignored in the fragment reassembly procedure--
 *                       i.e. the procedure functions correctly regardless of the buffer sizes used for any &
 *                       all received fragments.
 *
 *                                   |                   List of                     |
 *                                   |<----------- Fragmented Datagrams ------------>|
 *                                   |               (see Note #2b1A)                |
 *
 *
 *                                       |        NextPrimListPtr                  |
 *                                       |       (see Note #2b1D)                  |
 *                                       v                   |                     v
 *                                                           |
 *                     Head of        -------       -------  v    -------       -------  (see Note #2b1C2)
 *                     Fragment  ---->|     |------>|     |------>|     |------>|     |
 *                      Lists         |     |       |     |       |     |       |     |       Tail of
 *                                    |     |<------|     |<------|     |<------|     |<----  Fragment
 *                (see Note #2b1C1)   |     |       |     |  ^    |     |       |     |        Lists
 *                                    |     |       |     |  |    |     |       |     |
 *                                    -------       -------  |    -------       -------
 *                                       | ^                 |       | ^
 *                                       | |       PrevPrimListPtr   | |
 *                                       v |      (see Note #2b1D)   v |
 *                                     -------                     -------
 *                                     |     |                     |     |
 *                                     |     |                     |     |
 *                                     |     |                     |     |
 *                                     |     |                     |     |
 *                                     |     |                     |     |        Fragments in a fragment
 *                                     -------                     -------        list may use different
 *                                       | ^                         | ^           size network buffers
 *                       NextBufPtr ---> | | <--- PrevBufPtr         | |             (see Note #2b3)
 *                    (see Note #2b1E)   v |   (see Note #2b1E)      v |
 *                                     -------                     -------        The last fragment in a
 *                                     |     |                     |     |  <--- fragment list may likely
 *                                     |     |                     |     |       use a smaller buffer size
 *                                     |     |                     -------
 *                                     |     |
 *                                     |     |
 *                                     -------
 *
 * @param    p_buf       Pointer to network buffer that received IPv4 packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_ip_hdr    Pointer to received packet's IPv4 header.
 *
 *
 * @return   Pointer to reassembled datagram, if fragment reassembly complete.
 *           Pointer to NULL,                 if fragment reassembly in progress.
 *           Pointer to fragment buffer,      for any fragment discard error.
 *
 * @note     (3) See RFC #791, Section 3.2
 *                   RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly Procedure'
 *
 * @note     (4) Fragment lists are accessed by :
 *
 *           - (a) NetIPv4_RxPktFragReasm()
 *           - (b) Fragment list's 'TMR->Obj' pointer      during execution of NetIPv4_RxPktFragTimeout()
 *
 * @note     (5) Since the primary tasks of the network protocol suite are prevented from running
 *               concurrently (see 'net.h  Note #3'), it is NOT necessary to protect the shared
 *               resources of the fragment lists since no asynchronous access from other network
 *               tasks is possible.
 *******************************************************************************************************/
static NET_BUF *NetIPv4_RxPktFragReasm(NET_BUF      *p_buf,
                                       NET_BUF_HDR  *p_buf_hdr,
                                       NET_IPv4_HDR *p_ip_hdr,
                                       CPU_BOOLEAN  *p_is_frag,
                                       RTOS_ERR     *p_err)
{
  CPU_BOOLEAN  frag;
  CPU_BOOLEAN  frag_done;
  CPU_BOOLEAN  ip_flag_frags_more;
  CPU_INT16U   ip_flags;
  CPU_INT16U   frag_offset;
  CPU_INT16U   frag_size;
  NET_BUF      *p_frag;
  NET_BUF      *p_frag_list;
  NET_BUF_HDR  *p_frag_list_buf_hdr;
  NET_IPv4_HDR *p_frag_list_ip_hdr;

  //                                                               -------------- CHK FRAG REASM REQUIRED -------------
  frag = DEF_NO;

  ip_flags = p_buf_hdr->IP_Flags_FragOffset & NET_IPv4_HDR_FLAG_MASK;
  ip_flag_frags_more = DEF_BIT_IS_SET(ip_flags, NET_IPv4_HDR_FLAG_FRAG_MORE);
  if (ip_flag_frags_more != DEF_NO) {                           // If 'More Frags' set (see Note #3), ...
    frag = DEF_YES;                                             // ... mark as frag.
  }

  frag_offset = (CPU_INT16U)(p_buf_hdr->IP_Flags_FragOffset & NET_IPv4_HDR_FRAG_OFFSET_MASK);
  if (frag_offset != NET_IPv4_HDR_FRAG_OFFSET_NONE) {           // If frag offset != 0 (see Note #3), ...
    frag = DEF_YES;                                             // ... mark as frag.
  }

  if (frag != DEF_YES) {                                        // If pkt NOT a frag, ...
    *p_is_frag = DEF_NO;
    p_frag = p_buf;
    goto exit;                                                  // ... rtn non-frag'd datagram (see Note #3).
  }

  NET_CTR_STAT_INC(Net_StatCtrs.IPv4.RxFragCtr);

  *p_is_frag = DEF_YES;

  //                                                               ------------------- REASM FRAGS --------------------
  frag_size = p_buf_hdr->IP_TotLen - p_buf_hdr->IP_HdrLen;
  p_frag_list = NetIPv4_FragReasmListsHead;
  frag_done = DEF_NO;

  while (frag_done == DEF_NO) {                                 // Insert frag into a frag list.
    if (p_frag_list != DEF_NULL) {                              // Srch ALL existing frag lists first (see Note #2b2A).
      p_frag_list_buf_hdr = &p_frag_list->Hdr;
      p_frag_list_ip_hdr = (NET_IPv4_HDR *)&p_frag_list->DataPtr[p_frag_list_buf_hdr->IP_HdrIx];

      //                                                           If frag & this frag list's    ...
      if (p_buf_hdr->IP_AddrSrc == p_frag_list_buf_hdr->IP_AddrSrc) {             // ... src  addr (see Note #2a1) ...
        if (p_buf_hdr->IP_AddrDest == p_frag_list_buf_hdr->IP_AddrDest) {         // ... dest addr (see Note #2a2) ...
          if (p_buf_hdr->IP_ID == p_frag_list_buf_hdr->IP_ID) {                   // ... ID        (see Note #2a3) ...
            if (p_ip_hdr->Protocol == p_frag_list_ip_hdr->Protocol) {             // ... protocol  (see Note #2a4) ...
                                                                                  // ... fields identical,         ...
              p_frag = NetIPv4_RxPktFragListInsert(p_buf,                         // ... insert frag into frag list.
                                                   p_buf_hdr,
                                                   ip_flags,
                                                   frag_offset,
                                                   frag_size,
                                                   p_frag_list,
                                                   p_err);
              if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
                p_frag = DEF_NULL;
                goto exit;
              }

              frag_done = DEF_YES;
            }
          }
        }
      }

      if (frag_done != DEF_YES) {                               // If NOT done, adv to next frag list.
        p_frag_list = p_frag_list_buf_hdr->NextPrimListPtr;
      }
    } else {                                                    // Else add new frag list (see Note #2b2B).
      NetIPv4_RxPktFragListAdd(p_buf,
                               p_buf_hdr,
                               ip_flags,
                               frag_offset,
                               frag_size,
                               p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        p_frag = DEF_NULL;
        goto exit;
      }

      p_frag = DEF_NULL;

      frag_done = DEF_YES;
    }
  }

exit:
  return (p_frag);
}

/****************************************************************************************************//**
 *                                       NetIPv4_RxPktFragListAdd()
 *
 * @brief    (1) Add fragment as new fragment list at end of Fragment Lists :
 *
 *           - (a) Get    fragment reassembly timer
 *           - (b) Insert fragment into Fragment Lists
 *           - (c) Update fragment list reassembly calculations
 *
 * @param    p_buf           Pointer to network buffer that received fragment.
 *
 * @param    p_buf_hdr       Pointer to network buffer header.
 *
 * @param    frag_ip_flags   Fragment IPv4 header flags.
 *
 * @param    frag_offset     Fragment offset.
 *
 * @param    frag_size       Fragment size (in octets).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *
 * @note     (2) See RFC #791, Section 3.2 'Fragmentation and Reassembly'
 *
 * @note     (3) During fragment list insertion, some fragment buffer controls were previously initialized
 *                           in NetBuf_Get() when the packet was received at the network interface layer.  These buffer
 *                           controls do NOT need to be re-initialized but are shown for completeness.
 *******************************************************************************************************/
static void NetIPv4_RxPktFragListAdd(NET_BUF            *p_buf,
                                     NET_BUF_HDR        *p_buf_hdr,
                                     NET_IPv4_HDR_FLAGS frag_ip_flags,
                                     CPU_INT16U         frag_offset,
                                     CPU_INT16U         frag_size,
                                     RTOS_ERR           *p_err)
{
  CPU_BOOLEAN ip_flag_frags_more;
  CPU_INT16U  frag_size_min;
  CPU_INT32U  timeout_ms;
  NET_BUF_HDR *p_frag_list_tail_buf_hdr;
  CORE_DECLARE_IRQ_STATE;

  ip_flag_frags_more = DEF_BIT_IS_SET(frag_ip_flags, NET_IPv4_HDR_FLAG_FRAG_MORE);
  frag_size_min = (ip_flag_frags_more == DEF_YES) ? NET_IPv4_FRAG_SIZE_MIN_FRAG_MORE
                  : NET_IPv4_FRAG_SIZE_MIN_FRAG_LAST;
  if (frag_size < frag_size_min) {                              // See Note #2.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxFragSizeCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (frag_size > NET_IPv4_FRAG_SIZE_MAX) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxFragSizeCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------------------- GET FRAG TMR -------------------
  CORE_ENTER_ATOMIC();
  timeout_ms = NetIPv4_FragReasmTimeout_ms;
  CORE_EXIT_ATOMIC();
  p_buf_hdr->TmrPtr = NetTmr_Get(NetIPv4_RxPktFragTimeout,
                                 p_buf,
                                 timeout_ms,
                                 NET_TMR_OPT_NONE,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;                                                  // If tmr unavail, discard frag.
  }

  //                                                               ------------ INSERT FRAG INTO FRAG LISTS -----------
  if (NetIPv4_FragReasmListsTail != DEF_NULL) {                 // If frag lists NOT empty, insert @ tail.
    p_buf_hdr->PrevPrimListPtr = NetIPv4_FragReasmListsTail;
    p_frag_list_tail_buf_hdr = &NetIPv4_FragReasmListsTail->Hdr;
    p_frag_list_tail_buf_hdr->NextPrimListPtr = p_buf;
    NetIPv4_FragReasmListsTail = p_buf;
  } else {                                                      // Else add frag as first frag list.
    NetIPv4_FragReasmListsHead = p_buf;
    NetIPv4_FragReasmListsTail = p_buf;
    p_buf_hdr->PrevPrimListPtr = DEF_NULL;
  }

#if 0                                                           // Init'd in NetBuf_Get() [see Note #3].
  p_buf_hdr->NextPrimListPtr = DEF_NULL;
  p_buf_hdr->PrevBufPtr = DEF_NULL;
  p_buf_hdr->NextBufPtr = DEF_NULL;
  p_buf_hdr->IP_FragSizeTot = NET_IPv4_FRAG_SIZE_NONE;
  p_buf_hdr->IP_FragSizeCur = 0u;
#endif

  //                                                               ----------------- UPDATE FRAG CALCS ----------------
  NetIPv4_RxPktFragListUpdate(p_buf,
                              p_buf_hdr,
                              frag_ip_flags,
                              frag_offset,
                              frag_size,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv4_RxPktFragListInsert()
 *
 * @brief    Insert fragment into corresponding fragment list.
 *
 *       - (1) Fragments are sorted into fragment lists by fragment offset.
 *
 *       - (2) Although RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly
 *               Procedure' states that "in the case that two or more fragments contain the same data
 *               either identically or through a partial overlap ... [the IP fragmentation reassembly
 *               algorithm should] use the more recently arrived copy in the data buffer and datagram
 *               delivered"; in order to avoid the complexity of sequencing received fragments with
 *               duplicate data that overlap multiple previously-received fragments' data, duplicate
 *               & overlap fragments are discarded :
 *
 *           - (a) Duplicate fragments are discarded.  A fragment is a duplicate of a fragment already
 *                   in the fragment list if both fragments have identical fragment offset & size values.
 *
 *           - (b) Overlap fragments are discarded & the entire Fragmented Datagram is also discarded.
 *                   A fragment is an overlap fragment if any portion of its fragment data overlaps any
 *                   other fragment's data :
 *
 *               - (1) [Fragment offset]  <  [(Any other fragment offset * FRAG_OFFSET_SIZE) +
 *                                                   Any other fragment size                       ]
 *
 *               - (2) [(Fragment offset * FRAG_OFFSET_SIZE) + Fragment size]  >  [Any other fragment offset]
 *
 * @param    p_buf           Pointer to network buffer that received fragment.
 *
 * @param    p_buf_hdr       Pointer to network buffer header.
 *
 * @param    frag_ip_flags   Fragment IPv4 header flags.
 *
 * @param    frag_offset     Fragment offset.
 *
 * @param    frag_size       Fragment size (in octets).
 *
 * @param    p_frag_list     Pointer to fragment list head buffer.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to reassembled datagram, if fragment reassembly complete.
 *           Pointer to NULL,                 if fragment reassembly in progress.
 *           Pointer to NULL,                 for any fragment discard error.
 *
 * @note     (3) See RFC #791, Section 3.2 'Fragmentation and Reassembly'
 *
 * @note     (4) Assumes ALL fragments in fragment lists have previously been validated for buffer &
 *               IPv4 header fields.
 *
 * @note     (5) During fragment list insertion, some fragment buffer controls were previously
 *               initialized in NetBuf_Get() when the packet was received at the network interface
 *               layer.  These buffer controls do NOT need to be re-initialized but are shown for
 *               completeness.
 *******************************************************************************************************/
static NET_BUF *NetIPv4_RxPktFragListInsert(NET_BUF            *p_buf,
                                            NET_BUF_HDR        *p_buf_hdr,
                                            NET_IPv4_HDR_FLAGS frag_ip_flags,
                                            CPU_INT16U         frag_offset,
                                            CPU_INT16U         frag_size,
                                            NET_BUF            *p_frag_list,
                                            RTOS_ERR           *p_err)
{
  CPU_BOOLEAN ip_flag_frags_more;
  CPU_BOOLEAN frag_insert_done;
  CPU_BOOLEAN frag_list_discard;
  CPU_INT16U  frag_offset_actual;
  CPU_INT16U  frag_list_cur_frag_offset;
  CPU_INT16U  frag_list_cur_frag_offset_actual;
  CPU_INT16U  frag_list_prev_frag_offset;
  CPU_INT16U  frag_list_prev_frag_offset_actual;
  CPU_INT16U  frag_size_min;
  CPU_INT16U  frag_size_cur;
  NET_BUF     *p_frag;
  NET_BUF     *p_frag_list_prev_buf;
  NET_BUF     *p_frag_list_cur_buf;
  NET_BUF     *p_frag_list_prev_list;
  NET_BUF     *p_frag_list_next_list;
  NET_BUF_HDR *p_frag_list_buf_hdr;
  NET_BUF_HDR *p_frag_list_prev_buf_hdr;
  NET_BUF_HDR *p_frag_list_cur_buf_hdr;
  NET_BUF_HDR *p_frag_list_prev_list_buf_hdr;
  NET_BUF_HDR *p_frag_list_next_list_buf_hdr;
  NET_TMR     *p_tmr;
  RTOS_ERR    local_err;

  ip_flag_frags_more = DEF_BIT_IS_SET(frag_ip_flags, NET_IPv4_HDR_FLAG_FRAG_MORE);
  frag_size_min = (ip_flag_frags_more == DEF_YES) ? NET_IPv4_FRAG_SIZE_MIN_FRAG_MORE
                  : NET_IPv4_FRAG_SIZE_MIN_FRAG_LAST;
  if (frag_size < frag_size_min) {                              // See Note #2a1.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxFragSizeCtr);
    p_frag = DEF_NULL;
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (frag_size > NET_IPv4_FRAG_SIZE_MAX) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxFragSizeCtr);
    p_frag = DEF_NULL;
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------- INSERT FRAG INTO FRAG LISTS --------
  frag_insert_done = DEF_NO;

  p_frag_list_cur_buf = p_frag_list;
  p_frag_list_cur_buf_hdr = &p_frag_list_cur_buf->Hdr;

  while (frag_insert_done == DEF_NO) {
    frag_list_cur_frag_offset = (CPU_INT16U)(p_frag_list_cur_buf_hdr->IP_Flags_FragOffset & NET_IPv4_HDR_FRAG_OFFSET_MASK);
    if (frag_offset > frag_list_cur_frag_offset) {                      // While frag offset > cur frag offset, ...
      if (p_frag_list_cur_buf_hdr->NextBufPtr != DEF_NULL) {            // ... adv to next frag in list.
        p_frag_list_cur_buf = p_frag_list_cur_buf_hdr->NextBufPtr;
        p_frag_list_cur_buf_hdr = &p_frag_list_cur_buf->Hdr;
      } else {                                                          // If @ last frag in list, append frag @ end.
        frag_offset_actual = frag_offset                        * NET_IPv4_FRAG_SIZE_UNIT;
        frag_list_cur_frag_offset_actual = (frag_list_cur_frag_offset          * NET_IPv4_FRAG_SIZE_UNIT)
                                           + (p_frag_list_cur_buf_hdr->IP_TotLen - p_frag_list_cur_buf_hdr->IP_HdrLen);

        if (frag_offset_actual >= frag_list_cur_frag_offset_actual) {           // If frag does NOT overlap, ...
                                                                                // ... append @ end of frag list.
          p_buf_hdr->PrevBufPtr = p_frag_list_cur_buf;
#if 0                                                                   // Init'd in NetBuf_Get() [see Note #5].
          p_buf_hdr->NextBufPtr = DEF_NULL;
          p_buf_hdr->PrevPrimListPtr = DEF_NULL;
          p_buf_hdr->NextPrimListPtr = DEF_NULL;
          p_buf_hdr->TmrPtr = DEF_NULL;
          p_buf_hdr->IP_FragSizeTot = NET_IPv4_FRAG_SIZE_NONE;
          p_buf_hdr->IP_FragSizeCur = 0u;
#endif

          p_frag_list_cur_buf_hdr->NextBufPtr = p_buf;

          p_frag_list_buf_hdr = &p_frag_list->Hdr;
          NetIPv4_RxPktFragListUpdate(p_frag_list,                       // Update frag list reasm calcs.
                                      p_frag_list_buf_hdr,
                                      frag_ip_flags,
                                      frag_offset,
                                      frag_size,
                                      p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            p_frag = DEF_NULL;
            goto exit;
          }
          //                                                       Chk    frag list reasm complete.
          p_frag = NetIPv4_RxPktFragListChkComplete(p_frag_list,
                                                    p_frag_list_buf_hdr,
                                                    p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            p_frag = DEF_NULL;
            goto exit;
          }
        } else {                                                        // Else discard overlap frag & datagram.
          RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
          NetIPv4_RxPktFragListDiscard(p_frag_list, DEF_YES, &local_err);
          NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxFragDisCtr);
          p_frag = DEF_NULL;
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }

        frag_insert_done = DEF_YES;
      }
    } else if (frag_offset < frag_list_cur_frag_offset) {               // If frag offset < cur frag offset, ...
                                                                        // ... insert frag into frag list.
      frag_list_discard = DEF_NO;

      frag_offset_actual = (frag_offset               * NET_IPv4_FRAG_SIZE_UNIT) + frag_size;
      frag_list_cur_frag_offset_actual = frag_list_cur_frag_offset * NET_IPv4_FRAG_SIZE_UNIT;

      if (frag_offset_actual > frag_list_cur_frag_offset_actual) {      // If frag overlaps with next frag, ...
        frag_list_discard = DEF_YES;                                    // ... discard frag & datagram (see Note #2).
      } else if (p_frag_list_cur_buf_hdr->PrevBufPtr != DEF_NULL) {
        p_frag_list_prev_buf = p_frag_list_cur_buf_hdr->PrevBufPtr;
        p_frag_list_prev_buf_hdr = &p_frag_list_prev_buf->Hdr;

        frag_offset_actual = frag_offset * NET_IPv4_FRAG_SIZE_UNIT;

        frag_list_prev_frag_offset = p_frag_list_prev_buf_hdr->IP_Flags_FragOffset & NET_IPv4_HDR_FRAG_OFFSET_MASK;
        frag_list_prev_frag_offset_actual = (frag_list_prev_frag_offset        * NET_IPv4_FRAG_SIZE_UNIT)
                                            + (p_frag_list_prev_buf_hdr->IP_TotLen - p_frag_list_prev_buf_hdr->IP_HdrLen);
        //                                                         If frag overlaps with prev frag, ...
        if (frag_offset_actual < frag_list_prev_frag_offset_actual) {
          frag_list_discard = DEF_YES;                                  // ... discard frag & datagram (see Note #2).
        }
      } else {
        ;
      }

      if (frag_list_discard == DEF_NO) {                                // If frag does NOT overlap, ...
                                                                        // ... insert into frag list.
        p_buf_hdr->PrevBufPtr = p_frag_list_cur_buf_hdr->PrevBufPtr;
        p_buf_hdr->NextBufPtr = p_frag_list_cur_buf;

        if (p_buf_hdr->PrevBufPtr != DEF_NULL) {                   // Insert p_buf between prev & cur bufs.
          p_frag_list_prev_buf = p_buf_hdr->PrevBufPtr;
          p_frag_list_prev_buf_hdr = &p_frag_list_prev_buf->Hdr;

          p_frag_list_prev_buf_hdr->NextBufPtr = p_buf;
          p_frag_list_cur_buf_hdr->PrevBufPtr = p_buf;

#if 0                                                                   // Init'd in NetBuf_Get() [see Note #4].
          p_buf_hdr->PrevPrimListPtr = DEF_NULL;
          p_buf_hdr->NextPrimListPtr = DEF_NULL;
          p_buf_hdr->TmrPtr = DEF_NULL;
          p_buf_hdr->IP_FragSizeTot = NET_IPv4_FRAG_SIZE_NONE;
          p_buf_hdr->IP_FragSizeCur = 0u;
#endif
        } else {                                                        // Else p_buf is new frag list head.
          p_frag_list = p_buf;
          //                                                       Move frag list head info to cur buf ...
          //                                                       ... (see Note #3).
          p_buf_hdr->PrevPrimListPtr = p_frag_list_cur_buf_hdr->PrevPrimListPtr;
          p_buf_hdr->NextPrimListPtr = p_frag_list_cur_buf_hdr->NextPrimListPtr;
          p_buf_hdr->TmrPtr = p_frag_list_cur_buf_hdr->TmrPtr;
          p_buf_hdr->IP_FragSizeTot = p_frag_list_cur_buf_hdr->IP_FragSizeTot;
          p_buf_hdr->IP_FragSizeCur = p_frag_list_cur_buf_hdr->IP_FragSizeCur;

          p_frag_list_cur_buf_hdr->PrevBufPtr = p_buf;
          p_frag_list_cur_buf_hdr->PrevPrimListPtr = DEF_NULL;
          p_frag_list_cur_buf_hdr->NextPrimListPtr = DEF_NULL;
          p_frag_list_cur_buf_hdr->TmrPtr = DEF_NULL;
          p_frag_list_cur_buf_hdr->IP_FragSizeTot = NET_IPv4_FRAG_SIZE_NONE;
          p_frag_list_cur_buf_hdr->IP_FragSizeCur = 0u;

          //                                                       Point tmr            to new frag list head.
          p_tmr = p_buf_hdr->TmrPtr;
          p_tmr->Obj = p_buf;

          //                                                       Point prev frag list to new frag list head.
          p_frag_list_prev_list = p_buf_hdr->PrevPrimListPtr;
          if (p_frag_list_prev_list != DEF_NULL) {
            p_frag_list_prev_list_buf_hdr = &p_frag_list_prev_list->Hdr;
            p_frag_list_prev_list_buf_hdr->NextPrimListPtr = p_buf;
          } else {
            NetIPv4_FragReasmListsHead = p_buf;
          }

          //                                                       Point next frag list to new frag list head.
          p_frag_list_next_list = p_buf_hdr->NextPrimListPtr;
          if (p_frag_list_next_list != DEF_NULL) {
            p_frag_list_next_list_buf_hdr = &p_frag_list_next_list->Hdr;
            p_frag_list_next_list_buf_hdr->PrevPrimListPtr = p_buf;
          } else {
            NetIPv4_FragReasmListsTail = p_buf;
          }
        }

        p_frag_list_buf_hdr = &p_frag_list->Hdr;
        NetIPv4_RxPktFragListUpdate(p_frag_list,                         // Update frag list reasm calcs.
                                    p_frag_list_buf_hdr,
                                    frag_ip_flags,
                                    frag_offset,
                                    frag_size,
                                    p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          p_frag = DEF_NULL;
          goto exit;
        }
        //                                                         Chk    frag list reasm complete.
        p_frag = NetIPv4_RxPktFragListChkComplete(p_frag_list,
                                                  p_frag_list_buf_hdr,
                                                  p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          p_frag = DEF_NULL;
          goto exit;
        }
      } else {                                                           // Else discard overlap frag & datagram ...
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        NetIPv4_RxPktFragListDiscard(p_frag_list, DEF_YES, &local_err);         // ... (see Note #1).
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxFragDisCtr);
        p_frag = DEF_NULL;
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }

      frag_insert_done = DEF_YES;
    } else {                                                             // Else if frag offset = cur frag offset, ...
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxFragDisCtr);
      p_frag = DEF_NULL;
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                                 // ... discard duplicate frag (see Note #1).

      frag_size_cur = p_frag_list_cur_buf_hdr->IP_TotLen - p_frag_list_cur_buf_hdr->IP_HdrLen;
      if (frag_size != frag_size_cur) {                                  // If frag size != cur frag size,    ...
                                                                         // ... discard overlap frag datagram ...
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        NetIPv4_RxPktFragListDiscard(p_frag_list, DEF_YES, &local_err);
        //                                                         ... (see Note #1).
      }

      frag_insert_done = DEF_YES;

      goto exit;
    }
  }

exit:
  return (p_frag);
}

/****************************************************************************************************//**
 *                                       NetIPv4_RxPktFragListRemove()
 *
 * @brief    (1) Remove fragment list from Fragment Lists :
 *
 *           - (a) Free   fragment reassembly timer
 *           - (b) Remove fragment list from Fragment Lists
 *           - (c) Clear  buffer's fragment pointers
 *
 * @param    p_frag_list     Pointer to fragment list head buffer.
 *
 * @param    tmr_free        Indicate whether to free network timer :
 *                           DEF_YES            Free network timer for fragment list discard.
 *                           DEF_NO      Do NOT free network timer for fragment list discard
 *                           [Freed by  NetTmr_Handler()
 *                           via NetIPv4_RxPktFragListDiscard()].
 *******************************************************************************************************/
static void NetIPv4_RxPktFragListRemove(NET_BUF     *p_frag_list,
                                        CPU_BOOLEAN tmr_free)
{
  NET_BUF     *p_frag_list_prev_list;
  NET_BUF     *p_frag_list_next_list;
  NET_BUF_HDR *p_frag_list_prev_list_buf_hdr;
  NET_BUF_HDR *p_frag_list_next_list_buf_hdr;
  NET_BUF_HDR *p_frag_list_buf_hdr;
  NET_TMR     *p_tmr;

  p_frag_list_buf_hdr = &p_frag_list->Hdr;

  //                                                               ------------------ FREE FRAG TMR -------------------
  if (tmr_free == DEF_YES) {
    p_tmr = p_frag_list_buf_hdr->TmrPtr;
    if (p_tmr != DEF_NULL) {
      NetTmr_Free(p_tmr);
      p_frag_list_buf_hdr->TmrPtr = DEF_NULL;
    }
  }

  //                                                               --------- REMOVE FRAG LIST FROM FRAG LISTS ---------
  p_frag_list_prev_list = p_frag_list_buf_hdr->PrevPrimListPtr;
  p_frag_list_next_list = p_frag_list_buf_hdr->NextPrimListPtr;

  //                                                               Point prev frag list to next frag list.
  if (p_frag_list_prev_list != DEF_NULL) {
    p_frag_list_prev_list_buf_hdr = &p_frag_list_prev_list->Hdr;
    p_frag_list_prev_list_buf_hdr->NextPrimListPtr = p_frag_list_next_list;
  } else {
    NetIPv4_FragReasmListsHead = p_frag_list_next_list;
  }

  //                                                               Point next frag list to prev frag list.
  if (p_frag_list_next_list != DEF_NULL) {
    p_frag_list_next_list_buf_hdr = &p_frag_list_next_list->Hdr;
    p_frag_list_next_list_buf_hdr->PrevPrimListPtr = p_frag_list_prev_list;
  } else {
    NetIPv4_FragReasmListsTail = p_frag_list_prev_list;
  }

  //                                                               ---------------- CLR BUF FRAG PTRS -----------------
  p_frag_list_buf_hdr->PrevPrimListPtr = DEF_NULL;
  p_frag_list_buf_hdr->NextPrimListPtr = DEF_NULL;
  p_frag_list_buf_hdr->TmrPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                       NetIPv4_RxPktFragListDiscard()
 *
 * @brief    Discard fragment list from Fragment Lists.
 *
 * @param    p_frag_list     Pointer to fragment list head buffer.
 *
 * @param    tmr_free        Indicate whether to free network timer :
 *                           DEF_YES            Free network timer for fragment list discard.
 *                           DEF_NO      Do NOT free network timer for fragment list discard
 *                           [Freed by NetTmr_Handler()].
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIPv4_RxPktFragListDiscard(NET_BUF     *p_frag_list,
                                         CPU_BOOLEAN tmr_free,
                                         RTOS_ERR    *p_err)
{
  NET_CTR *p_ctr;

  NetIPv4_RxPktFragListRemove(p_frag_list, tmr_free);                    // Remove frag list from Frag Lists.

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  p_ctr = (NET_CTR *)&Net_ErrCtrs.IPv4.RxPktDisCtr;
#else
  p_ctr = DEF_NULL;
#endif
  (void)NetBuf_FreeBufList(p_frag_list, p_ctr);

  NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxFragDgramDisCtr);   // Inc discarded frag'd datagram ctr.

  RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
}

/****************************************************************************************************//**
 *                                       NetIPv4_RxPktFragListUpdate()
 *
 * @brief    Update fragment list reassembly calculations.
 *
 * @param    p_frag_list_buf_hdr     Pointer to fragment list head buffer's header.
 *
 * @param    frag_ip_flags           Fragment IPv4 header flags.
 *
 * @param    frag_offset             Fragment offset.
 *
 * @param    frag_size               Fragment size (in octets).
 *
 * @param    p_err                   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) See RFC #791, Section 3.2 'Fragmentation and Reassembly'
 *
 * @note     (2) To avoid possible integer arithmetic overflow, the fragmentation arithmetic result MUST
 *                                   be declared as an integer data type with a greater resolution -- i.e. greater number of
 *                                   bits -- than the fragmentation arithmetic operands' data type(s).
 *******************************************************************************************************/
static void NetIPv4_RxPktFragListUpdate(NET_BUF            *p_frag_list,
                                        NET_BUF_HDR        *p_frag_list_buf_hdr,
                                        NET_IPv4_HDR_FLAGS frag_ip_flags,
                                        CPU_INT16U         frag_offset,
                                        CPU_INT16U         frag_size,
                                        RTOS_ERR           *p_err)
{
  CPU_INT32U  frag_size_tot;                                        // See Note #2.
  CPU_BOOLEAN ip_flag_frags_more;
  RTOS_ERR    local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_frag_list_buf_hdr->IP_FragSizeCur += frag_size;
  ip_flag_frags_more = DEF_BIT_IS_SET(frag_ip_flags, NET_IPv4_HDR_FLAG_FRAG_MORE);
  if (ip_flag_frags_more != DEF_YES) {                              // If 'More Frags' NOT set (see Note #1), ...
                                                                    // ... calc frag tot size  (see Note #1).
    frag_size_tot = ((CPU_INT32U)frag_offset * NET_IPv4_FRAG_SIZE_UNIT) + (CPU_INT32U)frag_size;
    if (frag_size_tot > NET_IPv4_TOT_LEN_MAX) {                     // If frag tot size > IP tot len max, ...
                                                                    // ... discard ovf'd frag datagram (see Note #1).
      NetIPv4_RxPktFragListDiscard(p_frag_list, DEF_YES, &local_err);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    p_frag_list_buf_hdr->IP_FragSizeTot = (CPU_INT16U)frag_size_tot;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                   NetIPv4_RxPktFragListChkComplete()
 *
 * @brief    Check if fragment list complete; i.e. fragmented datagram reassembled.
 *
 * @param    p_frag_list             Pointer to fragment list head buffer.
 *
 * @param    p_frag_list_buf_hdr     Pointer to fragment list head buffer's header.
 *
 * @param    p_err                   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to reassembled datagram, if fragment reassembly complete.
 *           Pointer to NULL,                 if fragment reassembly in progress.
 *                                               OR
 *                                           for any fragment discard error.
 *
 * @note     (1) See RFC #791, Section 3.2 'Fragmentation and Reassembly'
 *
 * @note     (2) To avoid possible integer arithmetic overflow, the fragmentation arithmetic result
 *               MUST be declared as an integer data type with a greater resolution -- i.e. greater
 *               number of bits -- than the fragmentation arithmetic operands' data type(s).
 *******************************************************************************************************/
static NET_BUF *NetIPv4_RxPktFragListChkComplete(NET_BUF     *p_frag_list,
                                                 NET_BUF_HDR *p_frag_list_buf_hdr,
                                                 RTOS_ERR    *p_err)
{
  NET_BUF    *p_frag;
  CPU_INT32U frag_tot_len;                                           // See Note #2.
  CPU_INT32U timeout_ms;
  RTOS_ERR   local_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               If tot frag size complete, ...
  if (p_frag_list_buf_hdr->IP_FragSizeCur == p_frag_list_buf_hdr->IP_FragSizeTot) {
    //                                                             Calc frag IPv4 tot len (see Note #1a2).
    frag_tot_len = (CPU_INT32U)p_frag_list_buf_hdr->IP_HdrLen + (CPU_INT32U)p_frag_list_buf_hdr->IP_FragSizeTot;
    if (frag_tot_len > NET_IPv4_TOT_LEN_MAX) {                       // If tot frag len > IPv4 tot len max, ...
                                                                     // ... discard ovf'd frag datagram (see Note #1).
      NetIPv4_RxPktFragListDiscard(p_frag_list, DEF_YES, &local_err);
      p_frag = DEF_NULL;
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    NetIPv4_RxPktFragListRemove(p_frag_list, DEF_YES);
    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.RxFragDgramReasmCtr);
    p_frag = p_frag_list;                                            // ... rtn reasm'd datagram (see Note #1).

    //                                                             If cur frag size > tot frag size, ...
  } else if (p_frag_list_buf_hdr->IP_FragSizeCur > p_frag_list_buf_hdr->IP_FragSizeTot) {
    NetIPv4_RxPktFragListDiscard(p_frag_list, DEF_YES, p_err);       // ... discard ovf'd frag datagram.
    p_frag = DEF_NULL;
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  } else {                                                           // Else reset frag tmr (see Note #1).
    CORE_ENTER_ATOMIC();
    timeout_ms = NetIPv4_FragReasmTimeout_ms;
    CORE_EXIT_ATOMIC();
    NetTmr_Set(p_frag_list_buf_hdr->TmrPtr,
               NetIPv4_RxPktFragTimeout,
               timeout_ms);

    p_frag = DEF_NULL;
  }

exit:
  return (p_frag);
}

/****************************************************************************************************//**
 *                                       NetIPv4_RxPktFragTimeout()
 *
 * @brief    Discard fragment list on fragment reassembly timeout.
 *
 * @param    p_frag_list_timeout     Pointer to network buffer fragment reassembly list (see Note #1b).
 *
 * @note     (1) RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly Procedure'
 *                                   states that :
 *
 *                                       - (a) "If the [IP fragments' reassembly] timer runs out," ...
 *                                       - (b) "the [sic] all reassembly resources for this buffer identifier are released."
 *
 * @note     (3) This function is a network timer callback function :
 *
 *                                       - (a) Clear the timer pointer ... :
 *                                       - (1) Cleared in NetIPv4_RxPktFragListRemove() via NetIPv4_RxPktFragListDiscard().
 *
 *                                       - (b) but do NOT re-free the timer.
 *
 * @note     (4) See RFC #792, Section 'Time Exceeded Message'
 *******************************************************************************************************/
static void NetIPv4_RxPktFragTimeout(void *p_frag_list_timeout)
{
  NET_BUF  *p_frag_list;
  RTOS_ERR local_err;

  p_frag_list = (NET_BUF *)p_frag_list_timeout;

#ifdef  NET_ICMPv4_MODULE_EN
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetICMPv4_TxMsgErr(p_frag_list,                               // Send ICMPv4 'Time Exceeded' err msg (see Note #4).
                     NET_ICMPv4_MSG_TYPE_TIME_EXCEED,
                     NET_ICMPv4_MSG_CODE_TIME_EXCEED_FRAG_REASM,
                     NET_ICMPv4_MSG_PTR_NONE,
                     &local_err);
#endif

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  //                                                               Discard frag list (see Note #1b).
  NetIPv4_RxPktFragListDiscard(p_frag_list,
                               DEF_NO,                          // Clr but do NOT free tmr (see Note #3).
                               &local_err);

  NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxFragDgramTimeoutCtr);
}

/****************************************************************************************************//**
 *                                       NetIPv4_RxPktDemuxDatagram()
 *
 * @brief    Demultiplex IPv4 datagram to appropriate ICMP, IGMP, UDP, or TCP layer.
 *
 * @param    p_buf       Pointer to network buffer that received IPv4 datagram.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) When network buffer is demultiplexed to higher-layer protocol receive, buffer's reference
 *                       counter is NOT incremented since the IPv4 layer does NOT maintain a reference to the
 *                       buffer.
 *
 * @note     (2) Default case already invalidated in NetIPv4_RxPktValidate().  However, the default case
 *                       is included as an extra precaution in case 'ProtocolHdrType' is incorrectly modified.
 *******************************************************************************************************/
static void NetIPv4_RxPktDemuxDatagram(NET_BUF     *p_buf,
                                       NET_BUF_HDR *p_buf_hdr,
                                       RTOS_ERR    *p_err)
{
  switch (p_buf_hdr->ProtocolHdrType) {                          // Demux buf to appropriate protocol (see Note #1).
#ifdef NET_ICMPv4_MODULE_EN
    case NET_PROTOCOL_TYPE_ICMP_V4:
      NetICMPv4_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

#ifdef  NET_IGMP_MODULE_EN
    case NET_PROTOCOL_TYPE_IGMP:
      NetIGMP_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

    case NET_PROTOCOL_TYPE_UDP_V4:
      NetUDP_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

#ifdef  NET_TCP_MODULE_EN
    case NET_PROTOCOL_TYPE_TCP_V4:
      NetTCP_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

    case NET_PROTOCOL_TYPE_NONE:
    default:                                                    // See Note #2.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.RxInvProtocolCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIPv4_TxPktValidate()
 *
 * @brief    (1) Validate IPv4 transmit packet parameters & options :
 *
 *           - (a) Validate the following transmit packet parameters :
 *
 *               - (1) Supported protocols :
 *                   - (A) ICMP
 *                   - (B) IGMP
 *                   - (C) UDP
 *                   - (D) TCP
 *
 *               - (2) Buffer protocol index
 *               - (3) Total Length
 *               - (4) Type of Service (TOS)                                   See Note #2c
 *               - (5) Flags
 *               - (6) Time-to-Live    (TTL)                                   See Note #2d
 *               - (7) Destination Address                                     See Note #2f
 *               - (8) Source      Address                                     See Note #2e
 *               - (9) Options
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    addr_src    Source      IPv4 address.
 *
 * @param    addr_dest   Destination IPv4 address.
 *
 * @param    TOS         Specific TOS to transmit IPv4 packet
 *                       (see 'net_ipv4.h  IPv4 HEADER TYPE OF SERVICE (TOS) DEFINES').
 *
 * @param    TTL         Specific TTL to transmit IPv4 packet
 *                       (see 'net_ipv4.h  IPv4 HEADER TIME-TO-LIVE (TTL) DEFINES') :
 *                       NET_IPv4_TTL_MIN                minimum TTL transmit value   (1)
 *                       NET_IPv4_TTL_MAX                maximum TTL transmit value (255)
 *                       NET_IPv4_TTL_DFLT               default TTL transmit value (128)
 *                       NET_IPv4_TTL_NONE               replace with default TTL
 *
 * @param    flags       Flags to select transmit options; bit-field flags logically OR'd :
 *                       NET_IPv4_FLAG_NONE              No  IPv4 transmit flags selected.
 *                       NET_IPv4_FLAG_TX_DONT_FRAG      Set IPv4 'Don't Frag' flag.
 *
 * @param    p_opts      Pointer to one or more IPv4 options configuration data structures
 *                       (see 'net_ipv4.h  IPv4 HEADER OPTION CONFIGURATION DATA TYPES') :
 *                       NULL                            NO IPv4 transmit options configuration.
 *                       NET_IPv4_OPT_CFG_ROUTE_TS       Route &/or Internet Timestamp options configuration.
 *                       NET_IPv4_OPT_CFG_SECURITY       Security options configuration
 *                       (see 'net_ipv4.h  Note #1d').
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) RFC #1122, Section 3.2.1 requires that IPv4 packets be transmitted with the following
 *                       valid IPv4 header fields :
 *
 *                           - (a) Version                                             RFC #1122, Section 3.2.1.1
 *                           - (b) Check-Sum                                           RFC #1122, Section 3.2.1.2
 *                           - (c) Type of Service (TOS)                               RFC #1122, Section 3.2.1.6
 *                           - (d) Time-to-Live    (TTL)                               RFC #1122, Section 3.2.1.7
 *
 *                           - (1) RFC #1122, Section 3.2.1.7 states that "a host MUST NOT send a datagram with
 *                       a Time-to-Live (TTL) value of zero".
 *
 *                           - (e) Source      Address
 *
 *                           - (1) RFC #1122, Section 3.2.1.3 states that "when a host sends any datagram,
 *                       the IP source address MUST be one of its own IP addresses (but not a
 *                       broadcast or multicast address)".
 *
 *                           - (2) RFC #1122, Section 3.2.1.3.(g) states that the "internal host loopback
 *                       address ... MUST NOT appear outside a host".
 *
 *                           - (a) However, this does NOT prevent the host loopback address from being
 *                       used as an IPv4 packet's source address as long as BOTH the packet's
 *                       source AND destination addresses are internal host addresses, either
 *                       a configured host IP address or any host loopback address.
 *
 *                           - (f) Destination Address
 *
 *                           - (1) See RFC #1112, Section 4
 *                       RFC #3927, Section 2.1
 *                       RFC #3927, Section 2.6.2
 *******************************************************************************************************/

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIPv4_TxPktValidate(NET_BUF_HDR    *p_buf_hdr,
                                  NET_IPv4_ADDR  addr_src,
                                  NET_IPv4_ADDR  addr_dest,
                                  NET_IPv4_TOS   TOS,
                                  NET_IPv4_TTL   TTL,
                                  NET_IPv4_FLAGS flags,
                                  void           *p_opts,
                                  RTOS_ERR       *p_err)
{
  NET_IPv4_ADDR_OBJ *p_addr_obj;
  NET_IF_NBR        if_nbr;
  CPU_BOOLEAN       addr_host;
  CPU_BOOLEAN       tx_remote_host;
  CPU_BOOLEAN       is_en;

  PP_UNUSED_PARAM(TOS);
  PP_UNUSED_PARAM(TTL);
  PP_UNUSED_PARAM(flags);

  //                                                               ------------- VALIDATE IPv4 ADDRS --------------
  if_nbr = p_buf_hdr->IF_Nbr;                                       // Get pkt's tx IF.
  is_en = NetIF_IsEnHandler(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  if (is_en != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto exit;
  }

  //                                                               Chk pkt's tx cfg'd host addr.
  if (if_nbr != NET_IF_NBR_LOCAL_HOST) {
    p_addr_obj = NetIPv4_GetAddrObjCfgdOnIF(if_nbr, addr_src);
  } else {
    p_addr_obj = NetIPv4_GetAddrObjCfgd(DEF_NULL, addr_src);
  }

  if (p_addr_obj != DEF_NULL) {
    if ( (p_addr_obj->CfgMode != NET_IP_ADDR_CFG_MODE_DYN_INIT)
         && ((p_addr_obj->AddrHost == NET_IPv4_ADDR_NONE)
             || (p_addr_obj->AddrHostSubnetMask == NET_IPv4_ADDR_NONE)
             || (p_addr_obj->AddrHostSubnetMaskHost == NET_IPv4_ADDR_NONE)
             || (p_addr_obj->AddrHostSubnetNet == NET_IPv4_ADDR_NONE))) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }
  }

  //                                                               ----------- VALIDATE IPv4 DEST ADDR ------------
  //                                                               See Note #2e.
  addr_host = NetIPv4_IsAddrHostCfgdHandler(addr_dest);             // Chk this host's cfg'd addr(s) [see Note #2f].
  if (addr_host == DEF_YES) {
    tx_remote_host = DEF_NO;

    //                                                             Chk invalid 'This Host'       (see Note #2f).
  } else if (addr_dest == NET_IPv4_ADDR_THIS_HOST) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrDestCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
    goto exit;

    //                                                             Chk localhost addrs           (see Note #2f).
  } else if ((addr_dest & NET_IPv4_ADDR_LOCAL_HOST_MASK_NET)
             == NET_IPv4_ADDR_LOCAL_HOST_NET     ) {
    //                                                             Chk localhost 'This Host'     (see Note #2f).
    if ((addr_dest               & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)
        == (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }

    tx_remote_host = DEF_NO;

    //                                                             Chk link-local addrs          (see Note #2f).
  } else if ((addr_dest & NET_IPv4_ADDR_LOCAL_LINK_MASK_NET)
             == NET_IPv4_ADDR_LOCAL_LINK_NET     ) {
    //                                                             Chk link-local broadcast      (see Note #2f).
    if (addr_dest == NET_IPv4_ADDR_LOCAL_LINK_BROADCAST) {
      ;
      //                                                           Chk invalid link-local addrs  (see Note #2f).
    } else if ((addr_dest < NET_IPv4_ADDR_LOCAL_LINK_HOST_MIN)
               || (addr_dest > NET_IPv4_ADDR_LOCAL_LINK_HOST_MAX)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }

    tx_remote_host = DEF_YES;

    //                                                             Chk lim'd broadcast           (see Note #2f).
  } else if (addr_dest == NET_IPv4_ADDR_BROADCAST) {
    tx_remote_host = DEF_YES;

#ifdef  NET_MCAST_MODULE_EN
    //                                                             Chk Class-D multicast         (see Note #2f).
  } else if ((addr_dest & NET_IPv4_ADDR_CLASS_D_MASK) == NET_IPv4_ADDR_CLASS_D) {
    //                                                             Chk invalid multicast addrs   (see Note #2f).
    if ((addr_dest < NET_IPv4_ADDR_MULTICAST_MIN)
        || (addr_dest > NET_IPv4_ADDR_MULTICAST_MAX)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }

    tx_remote_host = DEF_YES;
#endif

    //                                                             Chk remote hosts :
  } else if (p_addr_obj == DEF_NULL) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrDestCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
    goto exit;

    //                                                             Check addr in dynamic process.
  } else if (p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_DYN_INIT) {
    tx_remote_host = DEF_YES;
    //                                                             Chk local  subnet.
  } else if ((addr_dest & p_addr_obj->AddrHostSubnetMask)
             == p_addr_obj->AddrHostSubnetNet ) {
    //                                                             Chk local  subnet 'This Host' (see Note #2f).
    if ((addr_dest               & p_addr_obj->AddrHostSubnetMaskHost)
        == (NET_IPv4_ADDR_THIS_HOST & p_addr_obj->AddrHostSubnetMaskHost)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }

    tx_remote_host = DEF_YES;
  } else {
    //                                                             Chk remote subnet.
    if ((addr_dest & NET_IPv4_ADDR_CLASS_A_MASK) == NET_IPv4_ADDR_CLASS_A) {
      //                                                           Chk Class-A 'This Host'       (see Note #2f).
      if ((addr_dest               & NET_IPv4_ADDR_CLASS_A_MASK_HOST)
          == (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_CLASS_A_MASK_HOST)) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrDestCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
        goto exit;
      }
    } else if ((addr_dest & NET_IPv4_ADDR_CLASS_B_MASK) == NET_IPv4_ADDR_CLASS_B) {
      //                                                           Chk Class-B 'This Host'       (see Note #2f).
      if ((addr_dest               & NET_IPv4_ADDR_CLASS_B_MASK_HOST)
          == (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_CLASS_B_MASK_HOST)) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrDestCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
        goto exit;
      }
    } else if ((addr_dest & NET_IPv4_ADDR_CLASS_C_MASK) == NET_IPv4_ADDR_CLASS_C) {
      //                                                           Chk Class-C 'This Host'       (see Note #2f).
      if ((addr_dest               & NET_IPv4_ADDR_CLASS_C_MASK_HOST)
          == (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_CLASS_C_MASK_HOST)) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrDestCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
        goto exit;
      }
    } else {                                                        // Discard invalid addr class    (see Note #3).
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }

    //                                                             Chk dflt gateway cfg'd.
    if (p_addr_obj->AddrDfltGateway == NET_IPv4_ADDR_NONE) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.CfgInvGatewayCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_NEXT_HOP);
      goto exit;
    }

    tx_remote_host = DEF_YES;
  }

  //                                                               ------------- VALIDATE IP SRC ADDR -------------
  //                                                               See Note #2d.
  //                                                               Chk this host's cfg'd addr  (see Note #2e).
  if (p_addr_obj != DEF_NULL) {
#if 0                                                               // Chk'd in 'VALIDATE IPv4 ADDRS'.
    if (p_ip_addrs->AddrHost == NET_IPv4_ADDR_NONE) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }
#endif
    if (addr_src != p_addr_obj->AddrHost) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }

    //                                                             Chk 'This Host'             (see Note #2e).
  } else if (addr_src == NET_IPv4_ADDR_THIS_HOST) {
    //                                                             Chk         localhost addrs (see Note #2e).
  } else if ((addr_src & NET_IPv4_ADDR_LOCAL_HOST_MASK_NET)
             == NET_IPv4_ADDR_LOCAL_HOST_NET     ) {
    //                                                             Chk invalid localhost addrs.
    if ((addr_src                & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)
        == (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }
    if ((addr_src                & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)
        == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST)) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }

    if (tx_remote_host != DEF_NO) {                                 // If localhost addr tx'd to remote host, ...
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);                            // ... rtn err / discard pkt   (see Note #2e).
      goto exit;
    }
  } else {
    if ((addr_src & NET_IPv4_ADDR_CLASS_A_MASK) == NET_IPv4_ADDR_CLASS_A) {
      //                                                           Chk Class-A 'This Host'     (see Note #2e).
      if ((addr_src                & NET_IPv4_ADDR_CLASS_A_MASK_HOST)
          != (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_CLASS_A_MASK_HOST)) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrSrcCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
        goto exit;
      }
    } else if ((addr_src & NET_IPv4_ADDR_CLASS_B_MASK) == NET_IPv4_ADDR_CLASS_B) {
      //                                                           Chk Class-B 'This Host'     (see Note #2e).
      if ((addr_src                & NET_IPv4_ADDR_CLASS_B_MASK_HOST)
          != (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_CLASS_B_MASK_HOST)) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrSrcCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
        goto exit;
      }
    } else if ((addr_src & NET_IPv4_ADDR_CLASS_C_MASK) == NET_IPv4_ADDR_CLASS_C) {
      //                                                           Chk Class-C 'This Host'     (see Note #2e).
      if ((addr_src                & NET_IPv4_ADDR_CLASS_C_MASK_HOST)
          != (NET_IPv4_ADDR_THIS_HOST & NET_IPv4_ADDR_CLASS_C_MASK_HOST)) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrSrcCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
        goto exit;
      }
    } else {                                                        // Discard invalid addr class
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }
  }

  //                                                               --------------- VALIDATE IP OPTS ---------------
  if (p_opts != DEF_NULL) {
    NetIPv4_TxPktValidateOpt(p_opts, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_TxPktValidateOpt()
 *
 * @brief    (1) Validate IPv4 transmit option configurations :
 *
 *           - (a) IPv4 transmit options MUST be configured by appropriate transmit options configuration
 *                   data structure(s) passed via 'p_opts';
 *
 *           - (b) IPv4 header allows for a maximum option size of 40 octets
 *
 * @param    p_opts  Pointer to one or more IPv4 options configuration data structures (see Note #1a) :
 *                   NET_IPv4_OPT_CFG_ROUTE_TS           IPv4 Route &/or Internet Timestamp options
 *                   configuration.
 *                   NET_IPv4_OPT_CFG_SECURITY           Security options configuration
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (3) The following IPv4 transmit options MUST be configured exclusively--i.e. only a single
 *                   IPv4 Route or Internet Timestamp option may be configured for any one IPv4 datagram :
 *
 *                       - (a) NET_IPv4_OPT_TYPE_ROUTE_STRICT
 *                       - (b) NET_IPv4_OPT_TYPE_ROUTE_LOOSE
 *                       - (c) NET_IPv4_OPT_TYPE_ROUTE_REC
 *                       - (d) NET_IPv4_OPT_TYPE_TS_ONLY
 *                       - (e) NET_IPv4_OPT_TYPE_TS_ROUTE_REC
 *                       - (f) NET_IPv4_OPT_TYPE_TS_ROUTE_SPEC
 *
 *                       - (A) RFC #1122, Section 3.2.1.8.(c) prohibits "an IP header" from transmitting
 *                   with "more than one Source Route option".
 *
 * @note     (4) RFC #791, Section 3.1 'Options : Internet Timestamp' states that "each timestamp"
 *                   may be "preceded with [an] internet address".
 *******************************************************************************************************/

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIPv4_TxPktValidateOpt(void     *p_opts,
                                     RTOS_ERR *p_err)
{
  CPU_INT08U        opt_len_size;
  CPU_INT08U        opt_len = 0u;
  CPU_INT08U        opt_nbr_route_ts;
  NET_IPv4_OPT_TYPE *p_opt_cfg_type;
  void              *p_opt_cfg;
  void              *p_opt_next = DEF_NULL;

  opt_len_size = 0u;
  opt_nbr_route_ts = 0u;
  p_opt_cfg = p_opts;

  while (p_opt_cfg != DEF_NULL) {
    p_opt_cfg_type = (NET_IPv4_OPT_TYPE *)p_opt_cfg;
    switch (*p_opt_cfg_type) {
      case NET_IPv4_OPT_TYPE_ROUTE_STRICT:
      case NET_IPv4_OPT_TYPE_ROUTE_LOOSE:
      case NET_IPv4_OPT_TYPE_ROUTE_REC:
      case NET_IPv4_OPT_TYPE_TS_ONLY:
      case NET_IPv4_OPT_TYPE_TS_ROUTE_REC:
      case NET_IPv4_OPT_TYPE_TS_ROUTE_SPEC:
        if (opt_nbr_route_ts > 0) {                             // If > 1 exclusive IPv4 opt, rtn err (see Note #3A).
          NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptCfgCtr);
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
        }
        opt_nbr_route_ts++;

        NetIPv4_TxPktValidateOptRouteTS(p_opt_cfg, &opt_len, &p_opt_next, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
        break;

      //                                                           -------------- UNSUPPORTED IPv4 OPTS ---------------
      case NET_IPv4_OPT_TYPE_SECURITY:
      case NET_IPv4_OPT_SECURITY_EXTENDED:
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.HdrOptNotSupportedCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        goto exit;

      case NET_IPv4_OPT_TYPE_NONE:                              // ---------------- INVALID IPv4 OPTS -----------------
      default:
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptTypeCtr);
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
    }

    opt_len_size += opt_len;
    if (opt_len_size > NET_IPv4_HDR_OPT_SIZE_MAX) {             // If tot opt len exceeds max opt len, rtn err.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptLenCtr);
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
    }

    p_opt_cfg = p_opt_next;                                     // Validate next cfg opt.
  }

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_TxPktValidateOptRouteTS()
 *
 * @brief    (1) Validate IPv4 Route &/or Internet Timestamp option configuration :
 *
 *           - (a) See 'net_ipv4.h  IPv4 ROUTE & INTERNET TIMESTAMP OPTIONS CONFIGURATION DATA TYPE' for
 *                   valid IPv4 Route &/or Internet Timestamp option configuration.
 *
 *           - (b) Validate the following options' configuration parameters :
 *
 *               - (1) Type
 *               - (2) Number
 *                       * Less    than minimum
 *                       * Greater than maximum
 *               - (3) IPv4 Route addresses
 *                       * MUST be IPv4 Class A, B, or C address   See 'net_ipv4.h  IPv4 ADDRESS DEFINES  Note #2a'
 *               - (4) Internet Timestamps
 *                       * Timestamp values are NOT validated
 *
 *           - (c) Return option values.
 *
 * @param    p_opt_route_ts  Pointer to IPv4 Route &/or Internet Timestamp option configuration data structure.
 *
 * @param    p_opt_len       Pointer to variable that will receive the Route/Internet Timestamp option length
 *
 * @param    ---------       (in octets).
 *
 * @param    p_opt_next      Pointer to variable that will receive the pointer to the next IPv4 transmit option.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIPv4_TxPktValidateOptRouteTS(void       *p_opt_route_ts,
                                            CPU_INT08U *p_opt_len,
                                            void       **p_opt_next,
                                            RTOS_ERR   *p_err)
{
  NET_IPv4_OPT_CFG_ROUTE_TS *p_opt_cfg_route_ts;
  CPU_INT08U                opt_nbr_min;
  CPU_INT08U                opt_nbr_max = 0u;
  CPU_INT08U                opt_len;
  CPU_INT08U                opt_len_opt;
  CPU_INT08U                opt_len_param = 0u;
  CPU_INT08U                opt_route_ix;
  CPU_BOOLEAN               opt_route_spec = DEF_NO;
  NET_IPv4_ADDR             opt_route_addr;

  p_opt_cfg_route_ts = (NET_IPv4_OPT_CFG_ROUTE_TS *)p_opt_route_ts;

  //                                                               ------------------ VALIDATE TYPE -------------------
  switch (p_opt_cfg_route_ts->Type) {
    case NET_IPv4_OPT_TYPE_ROUTE_STRICT:
      opt_nbr_min = NET_IPv4_OPT_PARAM_NBR_MIN;
      opt_nbr_max = NET_IPv4_OPT_PARAM_NBR_MAX_ROUTE;
      opt_len_opt = NET_IPv4_HDR_OPT_SIZE_ROUTE;
      opt_len_param = sizeof(NET_IPv4_ADDR);
      opt_route_spec = DEF_YES;
      break;

    case NET_IPv4_OPT_TYPE_ROUTE_LOOSE:
      opt_nbr_min = NET_IPv4_OPT_PARAM_NBR_MIN;
      opt_nbr_max = NET_IPv4_OPT_PARAM_NBR_MAX_ROUTE;
      opt_len_opt = NET_IPv4_HDR_OPT_SIZE_ROUTE;
      opt_len_param = sizeof(NET_IPv4_ADDR);
      opt_route_spec = DEF_YES;
      break;

    case NET_IPv4_OPT_TYPE_ROUTE_REC:
      opt_nbr_min = NET_IPv4_OPT_PARAM_NBR_MIN;
      opt_nbr_max = NET_IPv4_OPT_PARAM_NBR_MAX_ROUTE;
      opt_len_opt = NET_IPv4_HDR_OPT_SIZE_ROUTE;
      opt_len_param = sizeof(NET_IPv4_ADDR);
      opt_route_spec = DEF_NO;
      break;

    case NET_IPv4_OPT_TYPE_TS_ONLY:
      opt_nbr_min = NET_IPv4_OPT_PARAM_NBR_MIN;
      opt_nbr_max = NET_IPv4_OPT_PARAM_NBR_MAX_TS_ONLY;
      opt_len_opt = NET_IPv4_HDR_OPT_SIZE_TS;
      opt_len_param = sizeof(NET_TS);
      opt_route_spec = DEF_NO;
      break;

    case NET_IPv4_OPT_TYPE_TS_ROUTE_REC:
      opt_nbr_min = NET_IPv4_OPT_PARAM_NBR_MIN;
      opt_nbr_max = NET_IPv4_OPT_PARAM_NBR_MAX_TS_ROUTE;
      opt_len_opt = NET_IPv4_HDR_OPT_SIZE_TS;
      opt_len_param = sizeof(NET_IPv4_ADDR) + sizeof(NET_TS);
      opt_route_spec = DEF_NO;
      break;

    case NET_IPv4_OPT_TYPE_TS_ROUTE_SPEC:
      opt_nbr_min = NET_IPv4_OPT_PARAM_NBR_MIN;
      opt_nbr_max = NET_IPv4_OPT_PARAM_NBR_MAX_TS_ROUTE;
      opt_len_opt = NET_IPv4_HDR_OPT_SIZE_TS;
      opt_len_param = sizeof(NET_IPv4_ADDR) + sizeof(NET_TS);
      opt_route_spec = DEF_YES;
      break;

    case NET_IPv4_OPT_TYPE_NONE:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptTypeCtr);
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  //                                                               ------------------- VALIDATE NBR -------------------
  if (p_opt_cfg_route_ts->Nbr < opt_nbr_min) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptLenCtr);
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }
  if (p_opt_cfg_route_ts->Nbr > opt_nbr_max) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptLenCtr);
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  //                                                               ------------------ VALIDATE ROUTE ------------------
  if (opt_route_spec == DEF_YES) {                              // For specified routes ...
                                                                // ... validate all route addrs (see Note #1b3).
    for (opt_route_ix = 0u; opt_route_ix < p_opt_cfg_route_ts->Nbr; opt_route_ix++) {
      opt_route_addr = p_opt_cfg_route_ts->Route[opt_route_ix];

      if ((opt_route_addr & NET_IPv4_ADDR_CLASS_A_MASK) != NET_IPv4_ADDR_CLASS_A) {
        if ((opt_route_addr & NET_IPv4_ADDR_CLASS_B_MASK) != NET_IPv4_ADDR_CLASS_B) {
          if ((opt_route_addr & NET_IPv4_ADDR_CLASS_C_MASK) != NET_IPv4_ADDR_CLASS_C) {
            NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptCfgCtr);
            RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
          }
        }
      }
    }
  }

  //                                                               ------------------- VALIDATE TS --------------------
  //                                                               See Note #1b4.

  //                                                               ------------------- RTN OPT VALS -------------------
  opt_len = opt_len_opt + (p_opt_cfg_route_ts->Nbr * opt_len_param);
  *p_opt_len = opt_len;
  *p_opt_next = p_opt_cfg_route_ts->NextOptPtr;
}
#endif

/****************************************************************************************************//**
 *                                               NetIPv4_TxPkt()
 *
 * @brief    (1) Prepare IPv4 header & transmit IPv4 packet :
 *
 *           - (a) Prepare   IPv4 options (if any)
 *           - (b) Calculate IPv4 header buffer controls
 *           - (c) Check for transmit fragmentation        See Note #2
 *           - (d) Prepare   IPv4 header
 *           - (e) Transmit  IPv4 packet datagram
 *
 * @param    p_buf       Pointer to network buffer to transmit IPv4 packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    addr_src    Source      IPv4 address.
 *
 * @param    addr_dest   Destination IPv4 address.
 *
 * @param    TOS         Specific TOS to transmit IPv4 packet
 *
 * @param    TTL         Specific TTL to transmit IPv4 packet
 *                       NET_IPv4_TTL_MIN                minimum TTL transmit value   (1)
 *                       NET_IPv4_TTL_MAX                maximum TTL transmit value (255)
 *                       NET_IPv4_TTL_DFLT               default TTL transmit value (128)
 *                       NET_IPv4_TTL_NONE               replace with default TTL
 *
 * @param    flags       Flags to select transmit options; bit-field flags logically OR'd :
 *                       NET_IPv4_FLAG_NONE              No  IPv4 transmit flags selected.
 *                       NET_IPv4_FLAG_TX_DONT_FRAG      Set IPv4 'Don't Frag' flag.
 *
 * @param    p_opts      Pointer to one or more IPv4 options configuration data structures
 *                       NULL                            NO IPv4 transmit options configuration.
 *                       NET_IPv4_OPT_CFG_ROUTE_TS       Route &/or Internet Timestamp options configuration.
 *                       NET_IPv4_OPT_CFG_SECURITY       Security options configuration
 *
 *
 *               flags       Flags to select transmit options; bit-field flags logically OR'd :
 *                               NET_IPv4_FLAG_NONE              No  IPv4 transmit flags selected.
 *                               NET_IPv4_FLAG_TX_DONT_FRAG      Set IPv4 'Don't Frag' flag.
 *
 *               p_opts      Pointer to one or more IPv4 options configuration data structures
 *
 *                               NULL                            NO IPv4 transmit options configuration.
 *                               NET_IPv4_OPT_CFG_ROUTE_TS       Route &/or Internet Timestamp options configuration.
 *                               NET_IPv4_OPT_CFG_SECURITY       Security options configuration
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) IPv4 transmit fragmentation NOT currently supported (see 'net_ipv4.c  Note #1c'). #### NET-810
 *
 *               (3) Default case already invalidated in NetIPv4_TxPktValidate().  However, the default case
 *                   is included as an extra precaution in case 'ProtocolHdrType' is incorrectly modified.
 *******************************************************************************************************/
static void NetIPv4_TxPkt(NET_BUF        *p_buf,
                          NET_BUF_HDR    *p_buf_hdr,
                          NET_IPv4_ADDR  addr_src,
                          NET_IPv4_ADDR  addr_dest,
                          NET_IPv4_TOS   TOS,
                          NET_IPv4_TTL   TTL,
                          NET_IPv4_FLAGS flags,
                          void           *p_opts,
                          RTOS_ERR       *p_err)
{
#if 0                                                           // NOT currently implemented (see Note #2).
  CPU_BOOLEAN flag_dont_frag;
#endif
  CPU_INT08U        ip_opt_len_size;
  CPU_INT16U        ip_hdr_len_size;
  CPU_INT16U        protocol_ix = 0u;
  NET_MTU           ip_mtu;
  NET_IPv4_OPT_SIZE ip_hdr_opts[NET_IPv4_HDR_OPT_NBR_MAX];
  CPU_BOOLEAN       ip_tx_frag;

  //                                                               ---------------- PREPARE IPv4 OPTS -----------------
  if (p_opts != DEF_NULL) {
    ip_opt_len_size = NetIPv4_TxPktPrepareOpt(p_opts,
                                              (CPU_INT08U *)&ip_hdr_opts[0],
                                              p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  } else {
    ip_opt_len_size = 0u;
  }

  //                                                               --------------- CALC IPv4 HDR CTRLS ----------------
  //                                                               Calc tot IPv4 hdr len (in octets).
  ip_hdr_len_size = (CPU_INT16U)(NET_IPv4_HDR_SIZE_MIN + ip_opt_len_size);

  switch (p_buf_hdr->ProtocolHdrType) {
    case NET_PROTOCOL_TYPE_ICMP_V4:
      protocol_ix = p_buf_hdr->ICMP_MsgIx;
      break;

#ifdef  NET_IGMP_MODULE_EN
    case NET_PROTOCOL_TYPE_IGMP:
      protocol_ix = p_buf_hdr->IGMP_MsgIx;
      break;
#endif

    case NET_PROTOCOL_TYPE_UDP_V4:
#ifdef  NET_TCP_MODULE_EN
    case NET_PROTOCOL_TYPE_TCP_V4:
#endif
      protocol_ix = p_buf_hdr->TransportHdrIx;
      break;

    case NET_PROTOCOL_TYPE_NONE:
    default:                                                    // See Note #3.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvProtocolCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               ----------------- CHK FRAG REQUIRED ----------------
  ip_tx_frag = DEF_NO;
  if (protocol_ix < ip_hdr_len_size) {                          // If hdr len > allowed rem ix, tx frag req'd.
    ip_tx_frag = DEF_YES;
  }

  ip_mtu = NetIF_MTU_GetProtocol(p_buf_hdr->IF_Nbr, NET_PROTOCOL_TYPE_IP_V4, NET_IF_FLAG_NONE);

  ip_mtu -= ip_hdr_len_size - NET_IPv4_HDR_SIZE_MIN;
  if (p_buf_hdr->TotLen > ip_mtu) {                              // If tot len > MTU,            tx frag req'd.
    ip_tx_frag = DEF_YES;
  }

  if (ip_tx_frag == DEF_NO) {                                   // If tx frag NOT required, ...
    NetIPv4_TxPktPrepareHdr(p_buf,                              // ... prepare IPv4 hdr     ...
                            p_buf_hdr,
                            ip_hdr_len_size,
                            ip_opt_len_size,
                            protocol_ix,
                            addr_src,
                            addr_dest,
                            TOS,
                            TTL,
                            flags,
                            &ip_hdr_opts[0]);

    NetIPv4_TxPktDatagram(p_buf, p_buf_hdr, p_err);             // ... & tx IPv4 datagram.
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  } else {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxFragNotSupported);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIPv4_TxPktPrepareOpt()
 *
 * @brief    (1) Prepare IPv4 header with IPv4 transmit options :
 *
 *           - (a) Prepare ALL IPv4 options from configuration
 *                       data structure(s)
 *           - (b) Pad remaining IPv4 header octets            See RFC #791, Section 3.1 'Padding'
 *
 *       - (2) IP transmit options MUST be configured by appropriate options configuration data structure(s)
 *               passed via 'p_opts'; see 'net_ipv4.h  IPv4 HEADER OPTION CONFIGURATION DATA TYPES' for IPv4
 *               options configuration.
 *
 *       - (3) Convert ALL IPv4 options' multi-octet words from host-order to network-order.
 *
 * @param    p_opts      Pointer to one or more IPv4 options configuration data structures (see Note #2) :
 *
 * @param    ------     NULL                            NO IPv4 transmit options configuration.
 *                               NET_IPv4_OPT_CFG_ROUTE_TS       Route &/or Internet Timestamp options configuration.
 *                               NET_IPv4_OPT_CFG_SECURITY       Security options configuration
 *
 * @param    p_opt_hdr   Pointer to IPv4 transmit option buffer to prepare IPv4 options.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Total IPv4 option length (in octets), if NO error(s).
 *           0,                                    otherwise.
 *
 * @note     (5) Transmit arguments & options validated in NetIPv4_TxPktValidate()/NetIPv4_TxPktValidateOpt() :
 *
 *           - (a) Assumes ALL   transmit arguments & options are valid
 *           - (b) Assumes total transmit options' lengths    are valid
 *
 * @note     (6) IP header allows for a maximum option size of 40 octets (see 'net_ipv4.h  IPv4 HEADER
 *               OPTIONS DEFINES  Note #3').
 *
 * @note     (7) Default case already invalidated in NetIPv4_TxPktValidateOpt().  However, the default
 *               case is included as an extra precaution in case any of the IPv4 transmit options types
 *               are incorrectly modified.
 *******************************************************************************************************/
static CPU_INT08U NetIPv4_TxPktPrepareOpt(void       *p_opts,
                                          CPU_INT08U *p_opt_hdr,
                                          RTOS_ERR   *p_err)
{
  CPU_INT08U        ip_opt_len_tot = 0u;
  CPU_INT08U        ip_opt_len = 0u;
  CPU_INT08U        *p_opt_cfg_hdr = DEF_NULL;
  NET_IPv4_OPT_TYPE *p_opt_cfg_type = DEF_NULL;
  void              *p_opt_next = DEF_NULL;
  void              *p_opt_cfg = DEF_NULL;

  ip_opt_len_tot = 0u;
  p_opt_cfg = p_opts;
  p_opt_cfg_hdr = p_opt_hdr;
  //                                                               ---------------- PREPARE IPv4 OPTS -----------------
  while (p_opt_cfg != DEF_NULL) {                              // Prepare ALL cfg'd IPv4 opts (see Note #1a).
    p_opt_cfg_type = (NET_IPv4_OPT_TYPE *)p_opt_cfg;
    switch (*p_opt_cfg_type) {
      case NET_IPv4_OPT_TYPE_ROUTE_STRICT:
      case NET_IPv4_OPT_TYPE_ROUTE_LOOSE:
      case NET_IPv4_OPT_TYPE_ROUTE_REC:
        NetIPv4_TxPktPrepareOptRoute(p_opt_cfg, p_opt_cfg_hdr, &ip_opt_len, &p_opt_next);
        break;

      case NET_IPv4_OPT_TYPE_TS_ONLY:
        NetIPv4_TxPktPrepareOptTS(p_opt_cfg, p_opt_cfg_hdr, &ip_opt_len, &p_opt_next);
        break;

      case NET_IPv4_OPT_TYPE_TS_ROUTE_REC:
      case NET_IPv4_OPT_TYPE_TS_ROUTE_SPEC:
        NetIPv4_TxPktPrepareOptTSRoute(p_opt_cfg, p_opt_cfg_hdr, &ip_opt_len, &p_opt_next);
        break;

      //                                                           -------------- UNSUPPORTED IPv4 OPTS ---------------
      case NET_IPv4_OPT_TYPE_SECURITY:
      case NET_IPv4_OPT_SECURITY_EXTENDED:
        break;

      //                                                           ---------------- INVALID IPv4 OPTS -----------------
      case NET_IPv4_OPT_TYPE_NONE:
      default:                                                  // See Note #6.
        ip_opt_len_tot = 0u;
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptTypeCtr);
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);
    }

    if (ip_opt_len_tot > NET_IPv4_HDR_OPT_SIZE_MAX) {
      ip_opt_len_tot = 0u;
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptLenCtr);
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, 0u);
    }

    ip_opt_len_tot += ip_opt_len;
    p_opt_cfg_hdr += ip_opt_len;

    p_opt_cfg = p_opt_next;                                    // Prepare next cfg opt.
  }

  //                                                               ------------------- PAD IPv4 HDR -------------------
  if (ip_opt_len_tot > 0u) {
    //                                                             Pad rem'ing IPv4 hdr octets (see Note #1b).
    while ((ip_opt_len_tot %  NET_IPv4_HDR_OPT_SIZE_WORD)
           && (ip_opt_len_tot <= NET_IPv4_HDR_OPT_SIZE_MAX)) {
      *p_opt_cfg_hdr = NET_IPv4_HDR_OPT_PAD;
      p_opt_cfg_hdr++;
      ip_opt_len_tot++;
    }

    if (ip_opt_len_tot > NET_IPv4_HDR_OPT_SIZE_MAX) {
      ip_opt_len_tot = 0u;
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptLenCtr);
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, 0u);
    }
  }

  return (ip_opt_len_tot);
}

/****************************************************************************************************//**
 *                                       NetIPv4_TxPktPrepareOptRoute()
 *
 * @brief    (1) Prepare IPv4 header with IPv4 Route transmit options :
 *
 *           - (a) Prepare IPv4 Route option header
 *           - (b) Prepare IPv4 Route
 *
 *       - (2) See RFC #791, Section 3.1 'Options : Loose/Strict Source & Record Route'.
 *
 * @param    p_opts      Pointer to IPv4 Route option configuration data structure.
 *
 * @param    p_opt_hdr   Pointer to IPv4 transmit option buffer to prepare IPv4 Route option.
 *
 * @param    p_opt_len   Pointer to variable that will receive the Route option length (in octets).
 *
 * @param    p_opt_next  Pointer to variable that will receive the pointer to the next IPv4 transmit option.
 *
 * @note     (3) Transmit arguments & options validated in NetIPv4_TxPktValidate()/NetIPv4_TxPktValidateOpt() :
 *
 *                           - (a) Assumes ALL   transmit arguments & options are valid
 *                           - (b) Assumes total transmit options' lengths    are valid
 *
 * @note     (4) Default case already invalidated in NetIPv4_TxPktValidateOpt().  However, the default
 *                       case is included as an extra precaution in case any of the IPv4 transmit options types
 *                       are incorrectly modified.
 *******************************************************************************************************/
static void NetIPv4_TxPktPrepareOptRoute(void       *p_opts,
                                         CPU_INT08U *p_opt_hdr,
                                         CPU_INT08U *p_opt_len,
                                         void       **p_opt_next)
{
  NET_IPv4_OPT_CFG_ROUTE_TS *p_opt_cfg_route_ts;
  NET_IPv4_OPT_SRC_ROUTE    *p_opt_route;
  CPU_BOOLEAN               opt_route_spec = DEF_NO;
  CPU_INT08U                opt_route_ix;
  NET_IPv4_ADDR             opt_route_addr;

  //                                                               -------------- PREPARE ROUTE OPT HDR ---------------
  p_opt_cfg_route_ts = (NET_IPv4_OPT_CFG_ROUTE_TS *)p_opts;
  p_opt_route = (NET_IPv4_OPT_SRC_ROUTE *)p_opt_hdr;
  p_opt_route->Len = NET_IPv4_HDR_OPT_SIZE_ROUTE + (p_opt_cfg_route_ts->Nbr * sizeof(NET_IPv4_ADDR));
  p_opt_route->Ptr = NET_IPv4_OPT_ROUTE_PTR_ROUTE;
  p_opt_route->Pad = NET_IPv4_HDR_OPT_PAD;

  switch (p_opt_cfg_route_ts->Type) {
    case NET_IPv4_OPT_TYPE_ROUTE_STRICT:
      p_opt_route->Type = NET_IPv4_HDR_OPT_ROUTE_SRC_STRICT;
      opt_route_spec = DEF_YES;
      break;

    case NET_IPv4_OPT_TYPE_ROUTE_LOOSE:
      p_opt_route->Type = NET_IPv4_HDR_OPT_ROUTE_SRC_LOOSE;
      opt_route_spec = DEF_YES;
      break;

    case NET_IPv4_OPT_TYPE_ROUTE_REC:
      p_opt_route->Type = NET_IPv4_HDR_OPT_ROUTE_REC;
      opt_route_spec = DEF_NO;
      break;

    case NET_IPv4_OPT_TYPE_NONE:
    default:                                                    // See Note #4.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptTypeCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               ------------------ PREPARE ROUTE -------------------
  for (opt_route_ix = 0u; opt_route_ix < p_opt_cfg_route_ts->Nbr; opt_route_ix++) {
    //                                                             Cfg specified or rec route addrs.
    if (opt_route_spec == DEF_YES) {
      opt_route_addr = p_opt_cfg_route_ts->Route[opt_route_ix];
    } else {
      opt_route_addr = (NET_IPv4_ADDR)NET_IPv4_ADDR_NONE;
    }
    NET_UTIL_VAL_COPY_SET_NET_32(&p_opt_route->Route[opt_route_ix], &opt_route_addr);
  }

  *p_opt_len = p_opt_route->Len;
  *p_opt_next = p_opt_cfg_route_ts->NextOptPtr;
}

/****************************************************************************************************//**
 *                                       NetIPv4_TxPktPrepareOptTS()
 *
 * @brief    (1) Prepare IPv4 header with Internet Timestamp option :
 *
 *           - (a) Prepare Internet Timestamp option header
 *           - (b) Prepare Internet Timestamps
 *
 *       - (2) See RFC #791, Section 3.1 'Options : Internet Timestamp'.
 *
 * @param    p_opts      Pointer to Internet Timestamp option configuration data structure.
 *
 * @param    p_opt_hdr   Pointer to IP transmit option buffer to prepare Internet Timestamp option.
 *
 * @param    p_opt_len   Pointer to variable that will receive the Internet Timestamp option length
 *
 * @param    ---------   (in octets).
 *
 *
 *               p_opt_next  Pointer to variable that will receive the pointer to the next IPv4 transmit option.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (3) Transmit arguments & options validated in NetIPv4_TxPktValidate()/NetIPv4_TxPktValidateOpt() :
 *
 *                   (a) Assumes ALL   transmit arguments & options are valid
 *                   (b) Assumes total transmit options' lengths    are valid
 *
 *               (4) Default case already invalidated in NetIPv4_TxPktValidateOpt().  However, the default
 *                   case is included as an extra precaution in case any of the IPv4 transmit options types
 *                   are incorrectly modified.
 *******************************************************************************************************/
static void NetIPv4_TxPktPrepareOptTS(void       *p_opts,
                                      CPU_INT08U *p_opt_hdr,
                                      CPU_INT08U *p_opt_len,
                                      void       **p_opt_next)
{
  NET_IPv4_OPT_CFG_ROUTE_TS *p_opt_cfg_route_ts;
  NET_IPv4_OPT_TS           *p_opt_ts = DEF_NULL;
  CPU_INT08U                opt_ts_ovf;
  CPU_INT08U                opt_ts_flags;
  CPU_INT08U                opt_ts_ix;
  NET_TS                    opt_ts;

  //                                                               ---------------- PREPARE TS OPT HDR ----------------
  p_opt_cfg_route_ts = (NET_IPv4_OPT_CFG_ROUTE_TS *)p_opts;

  switch (p_opt_cfg_route_ts->Type) {
    case NET_IPv4_OPT_TYPE_TS_ONLY:
      p_opt_ts = (NET_IPv4_OPT_TS *)p_opt_hdr;
      p_opt_ts->Type = NET_IPv4_HDR_OPT_TS;
      p_opt_ts->Len = NET_IPv4_HDR_OPT_SIZE_TS + (p_opt_cfg_route_ts->Nbr * sizeof(NET_TS));
      p_opt_ts->Ptr = NET_IPv4_OPT_TS_PTR_TS;

      opt_ts_ovf = 0u;
      opt_ts_flags = NET_IPv4_OPT_TS_FLAG_TS_ONLY;
      p_opt_ts->Ovf_Flags = opt_ts_ovf | opt_ts_flags;
      break;

    case NET_IPv4_OPT_TYPE_NONE:
    default:                                                    // See Note #4.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptTypeCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               -------------------- PREPARE TS --------------------
  for (opt_ts_ix = 0u; opt_ts_ix < p_opt_cfg_route_ts->Nbr; opt_ts_ix++) {
    opt_ts = p_opt_cfg_route_ts->TS[opt_ts_ix];
    NET_UTIL_VAL_COPY_SET_NET_32(&p_opt_ts->TS[opt_ts_ix], &opt_ts);
  }

  *p_opt_len = p_opt_ts->Len;
  *p_opt_next = p_opt_cfg_route_ts->NextOptPtr;
}

/****************************************************************************************************//**
 *                                       NetIPv4_TxPktPrepareOptTSRoute()
 *
 * @brief    (1) Prepare IPv4 header with Internet Timestamp with IPv4 Route option :
 *
 *           - (a) Prepare Internet Timestamp option header
 *           - (b) Prepare Internet Timestamps
 *
 *       - (2) See RFC #791, Section 3.1 'Options : Internet Timestamp'.
 *
 * @param    p_opts      Pointer to Internet Timestamp option configuration data structure.
 *
 * @param    p_opt_hdr   Pointer to IP transmit option buffer to prepare Internet Timestamp option.
 *
 * @param    p_opt_len   Pointer to variable that will receive the Internet Timestamp option length
 *                       (in octets).
 *
 * @param    p_opt_next  Pointer to variable that will receive the pointer to the next IPv4 transmit option.
 *
 * @note     (3) Transmit arguments & options validated in NetIPv4_TxPktValidate()/NetIPv4_TxPktValidateOpt() :
 *
 *                           - (a) Assumes ALL   transmit arguments & options are valid
 *                           - (b) Assumes total transmit options' lengths    are valid
 *
 * @note     (4) Default case already invalidated in NetIPv4_TxPktValidateOpt().  However, the default
 *                       case is included as an extra precaution in case any of the IPv4 transmit options types
 *                       are incorrectly modified.
 *******************************************************************************************************/
static void NetIPv4_TxPktPrepareOptTSRoute(void       *p_opts,
                                           CPU_INT08U *p_opt_hdr,
                                           CPU_INT08U *p_opt_len,
                                           void       **p_opt_next)
{
  NET_IPv4_OPT_CFG_ROUTE_TS *p_opt_cfg_route_ts;
  NET_IPv4_OPT_TS_ROUTE     *p_opt_ts_route;
  NET_IPv4_ROUTE_TS         *p_route_ts;
  CPU_INT08U                opt_ts_ovf;
  CPU_INT08U                opt_ts_flags;
  CPU_INT08U                opt_ts_ix;
  NET_TS                    opt_ts;
  CPU_BOOLEAN               opt_route_spec = DEF_NO;
  NET_IPv4_ADDR             opt_route_addr;

  //                                                               ---------------- PREPARE TS OPT HDR ----------------
  p_opt_cfg_route_ts = (NET_IPv4_OPT_CFG_ROUTE_TS *)p_opts;
  p_opt_ts_route = (NET_IPv4_OPT_TS_ROUTE *)p_opt_hdr;
  p_opt_ts_route->Type = NET_IPv4_HDR_OPT_TS;
  p_opt_ts_route->Len = NET_IPv4_HDR_OPT_SIZE_TS + (p_opt_cfg_route_ts->Nbr * (sizeof(NET_IPv4_ADDR) + sizeof(NET_TS)));
  p_opt_ts_route->Ptr = NET_IPv4_OPT_TS_PTR_TS;
  opt_ts_ovf = 0u;

  switch (p_opt_cfg_route_ts->Type) {
    case NET_IPv4_OPT_TYPE_TS_ROUTE_REC:
      opt_ts_flags = NET_IPv4_OPT_TS_FLAG_TS_ROUTE_REC;
      p_opt_ts_route->Ovf_Flags = opt_ts_ovf | opt_ts_flags;
      opt_route_spec = DEF_NO;
      break;

    case NET_IPv4_OPT_TYPE_TS_ROUTE_SPEC:
      opt_ts_flags = NET_IPv4_OPT_TS_FLAG_TS_ROUTE_SPEC;
      p_opt_ts_route->Ovf_Flags = opt_ts_ovf | opt_ts_flags;
      opt_route_spec = DEF_YES;
      break;

    case NET_IPv4_OPT_TYPE_NONE:
    default:                                                    // See Note #4.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvOptTypeCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               ----------------- PREPARE ROUTE/TS -----------------
  p_route_ts = &p_opt_ts_route->Route_TS[0];
  for (opt_ts_ix = 0u; opt_ts_ix < p_opt_cfg_route_ts->Nbr; opt_ts_ix++) {
    //                                                             Cfg specified or rec route addrs.
    if (opt_route_spec == DEF_YES) {
      opt_route_addr = p_opt_cfg_route_ts->Route[opt_ts_ix];
    } else {
      opt_route_addr = (NET_IPv4_ADDR)NET_IPv4_ADDR_NONE;
    }
    NET_UTIL_VAL_COPY_SET_NET_32(&p_route_ts->Route[opt_ts_ix], &opt_route_addr);

    opt_ts = p_opt_cfg_route_ts->TS[opt_ts_ix];
    NET_UTIL_VAL_COPY_SET_NET_32(&p_route_ts->TS[opt_ts_ix], &opt_ts);
  }

  *p_opt_len = p_opt_ts_route->Len;
  *p_opt_next = p_opt_cfg_route_ts->NextOptPtr;
}

/****************************************************************************************************//**
 *                                           NetIPv4_TxPktPrepareHdr()
 *
 * @brief    (1) Prepare IP header :
 *
 *           - (a) Update network buffer's protocol index & length controls
 *
 *           - (b) Prepare the transmit packet's following IP header fields :
 *
 *               - (1) Version
 *               - (2) Header Length
 *               - (3) Type of Service (TOS)
 *               - (4) Total  Length
 *               - (5) Identification  (ID)
 *               - (6) Flags
 *               - (7) Fragment Offset
 *               - (8) Time-to-Live    (TTL)
 *               - (9) Protocol
 *                   (10) Check-Sum                                   See Note #6
 *                   (11) Source      Address
 *                   (12) Destination Address
 *                   (13) Options
 *
 *           - (c) Convert the following IP header fields from host-order to network-order :
 *
 *               - (1) Total Length
 *               - (2) Identification (ID)
 *               - (3) Flags/Fragment Offset
 *               - (4) Source      Address
 *               - (5) Destination Address
 *               - (6) Check-Sum                                   See Note #6c
 *               - (7) Options                                     See Note #5
 *
 * @param    p_buf           Pointer to network buffer to transmit IP packet.
 *
 * @param    p_buf_hdr       Pointer to network buffer header.
 *
 * @param    ip_hdr_len_tot  Total IPv4 header length.
 *
 * @param    ip_opt_len_tot  Total IPv4 header options' length.
 *
 * @param    protocol_ix     Index to higher-layer protocol header.
 *
 * @param    addr_src        Source      IP address.
 *
 * @param    addr_dest       Destination IPv4 address.
 *
 * @param    TOS             Specific TOS to transmit IPv4 packet
 *
 * @param    TTL             Specific TTL to transmit IPv4 packet
 *                           NET_IPv4_TTL_MIN                minimum TTL transmit value   (1)
 *                           NET_IPv4_TTL_MAX                maximum TTL transmit value (255)
 *                           NET_IPv4_TTL_DFLT               default TTL transmit value (128)
 *                           NET_IPv4_TTL_NONE               replace with default TTL
 *
 * @param    flags           Flags to select transmit options; bit-field flags logically OR'd :
 *                           NET_IPv4_FLAG_NONE              No  IPv4 transmit flags selected.
 *                           NET_IPv4_FLAG_TX_DONT_FRAG      Set IPv4 'Don't Frag' flag.
 *
 * @param    p_ip_hdr_opts   Pointer to IPv4 options buffer.
 *
 * @note     (2) See 'net_ipv4.h  IPv4 HEADER' for IPv4 header format.
 *
 * @note     (3) Supports ONLY the following protocols :
 *
 *                               - (a) ICMP
 *                               - (b) IGMP
 *                               - (c) UDP
 *                               - (d) TCP
 *
 *                           See also 'net.h  Note #2a'.
 *
 * @note     (4) Default case already invalidated in NetIPv4_TxPktValidate().  However, the default case is
 *                           included as an extra precaution in case 'ProtocolHdrType' is incorrectly modified.
 *
 * @note     (5) Assumes ALL IPv4 options' multi-octet words previously converted from host-order to
 *                           network-order.
 *
 * @note     (6) IPv4 header Check-Sum:
 *
 *                               - (a) IPv4 header Check-Sum MUST be calculated AFTER the entire IPv4 header has been prepared.
 *                           In addition, ALL multi-octet words are converted from host-order to network-order
 *                           since "the sum of 16-bit integers can be computed in either byte order" [RFC #1071,
 *                           Section 2.(B)].
 *
 *                               - (b) IPv4 header Check-Sum field MUST be cleared to '0' BEFORE the IP header Check-Sum is
 *                           calculated (see RFC #791, Section 3.1 'Header Checksum').
 *
 *                               - (c) The IPv4 header Check-Sum field is returned in network-order & MUST NOT be re-converted
 *                           back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()  Note #3b').
 *******************************************************************************************************/
static void NetIPv4_TxPktPrepareHdr(NET_BUF       *p_buf,
                                    NET_BUF_HDR   *p_buf_hdr,
                                    CPU_INT16U    ip_hdr_len_tot,
                                    CPU_INT08U    ip_opt_len_tot,
                                    CPU_INT16U    protocol_ix,
                                    NET_IPv4_ADDR addr_src,
                                    NET_IPv4_ADDR addr_dest,
                                    NET_IPv4_TOS  TOS,
                                    NET_IPv4_TTL  TTL,
                                    CPU_INT16U    flags,
                                    CPU_INT32U    *p_ip_hdr_opts)
{
  NET_IPv4_HDR *p_ip_hdr;
  CPU_INT08U   ip_ver;
  CPU_INT08U   ip_hdr_len;
  CPU_INT16U   ip_id;
  CPU_INT16U   ip_flags;
  CPU_INT16U   ip_frag_offset;
  CPU_INT16U   ip_opt_ix;
  CPU_INT16U   ip_flags_frag_offset;
  CPU_INT16U   ip_chk_sum;
  CPU_BOOLEAN  addr_dest_multicast;

  //                                                               ----------------- UPDATE BUF CTRLS -----------------
  p_buf_hdr->IP_HdrLen = ip_hdr_len_tot;
  p_buf_hdr->IP_HdrIx = protocol_ix - p_buf_hdr->IP_HdrLen;

  p_buf_hdr->IP_DataLen = (CPU_INT16U) p_buf_hdr->TotLen;
  p_buf_hdr->IP_DatagramLen = (CPU_INT16U) p_buf_hdr->TotLen;
  p_buf_hdr->TotLen += (NET_BUF_SIZE) p_buf_hdr->IP_HdrLen;
  p_buf_hdr->IP_TotLen = (CPU_INT16U) p_buf_hdr->TotLen;

  //                                                               ----------------- PREPARE IPv4 HDR -----------------
  p_ip_hdr = (NET_IPv4_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];

  //                                                               -------------- PREPARE IP VER/HDR LEN --------------
  ip_ver = NET_IPv4_HDR_VER;
  ip_ver <<= NET_IPv4_HDR_VER_SHIFT;

  ip_hdr_len = p_buf_hdr->IP_HdrLen / NET_IPv4_HDR_LEN_WORD_SIZE;
  ip_hdr_len &= NET_IPv4_HDR_LEN_MASK;

  p_ip_hdr->Ver_HdrLen = ip_ver | ip_hdr_len;

  //                                                               ------------------ PREPARE IP TOS ------------------
  p_ip_hdr->TOS = TOS;

  //                                                               --------------- PREPARE IPv4 TOT LEN ---------------
  NET_UTIL_VAL_COPY_SET_NET_16(&p_ip_hdr->TotLen, &p_buf_hdr->TotLen);

  //                                                               ----------------- PREPARE IPv4 ID ------------------
  NET_IPv4_TX_GET_ID(ip_id);
  NET_UTIL_VAL_COPY_SET_NET_16(&p_ip_hdr->ID, &ip_id);

  //                                                               -------------- PREPARE IPv4 FLAGS/FRAG -------------
  ip_flags = NET_IPv4_HDR_FLAG_NONE;
  ip_flags |= flags;
  ip_flags &= NET_IPv4_HDR_FLAG_MASK;

  ip_frag_offset = NET_IPv4_HDR_FRAG_OFFSET_NONE;

  ip_flags_frag_offset = ip_flags | ip_frag_offset;
  NET_UTIL_VAL_COPY_SET_NET_16(&p_ip_hdr->Flags_FragOffset, &ip_flags_frag_offset);

  //                                                               ----------------- PREPARE IPv4 TTL -----------------
  if (TTL != NET_IPv4_TTL_NONE) {
    p_ip_hdr->TTL = TTL;
  } else {
#ifdef  NET_MCAST_TX_MODULE_EN
    addr_dest_multicast = NetIPv4_IsAddrMulticast(addr_dest);
#else
    addr_dest_multicast = DEF_NO;
#endif
    //                                                             ... set dflt multicast TTL for multicast dest addr.
    p_ip_hdr->TTL = (addr_dest_multicast != DEF_YES) ? NET_IPv4_TTL_DFLT
                    : NET_IPv4_TTL_MULTICAST_DFLT;
  }

  //                                                               -------------- PREPARE IPv4 PROTOCOL ---------------
  switch (p_buf_hdr->ProtocolHdrType) {                         // Demux IPv4 protocol (see Note #3).
    case NET_PROTOCOL_TYPE_ICMP_V4:
      p_ip_hdr->Protocol = NET_IP_HDR_PROTOCOL_ICMP;
      break;

#ifdef  NET_IGMP_MODULE_EN
    case NET_PROTOCOL_TYPE_IGMP:
      p_ip_hdr->Protocol = NET_IP_HDR_PROTOCOL_IGMP;
      break;
#endif

    case NET_PROTOCOL_TYPE_UDP_V4:
      p_ip_hdr->Protocol = NET_IP_HDR_PROTOCOL_UDP;
      break;

#ifdef  NET_TCP_MODULE_EN
    case NET_PROTOCOL_TYPE_TCP_V4:
      p_ip_hdr->Protocol = NET_IP_HDR_PROTOCOL_TCP;
      break;
#endif

    case NET_PROTOCOL_TYPE_NONE:
    default:                                                    // See Note #4.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvProtocolCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V4;         // Update buf protocol for IPv4.
  p_buf_hdr->ProtocolHdrTypeNet = NET_PROTOCOL_TYPE_IP_V4;

  //                                                               ---------------- PREPARE IPv4 ADDRS ----------------
  p_buf_hdr->IP_AddrSrc = addr_src;
  p_buf_hdr->IP_AddrDest = addr_dest;

  NET_UTIL_VAL_COPY_SET_NET_32(&p_ip_hdr->AddrSrc, &addr_src);
  NET_UTIL_VAL_COPY_SET_NET_32(&p_ip_hdr->AddrDest, &addr_dest);

  //                                                               ---------------- PREPARE IPv4 OPTS -----------------
  if (ip_opt_len_tot > 0) {
    ip_opt_ix = p_buf_hdr->IP_HdrIx + NET_IPv4_HDR_OPT_IX;
    NetBuf_DataWr(p_buf,                                        // See Note #5.
                  ip_opt_ix,
                  ip_opt_len_tot,
                  (CPU_INT08U *)p_ip_hdr_opts);
  }

  //                                                               --------------- PREPARE IPv4 CHK SUM ---------------
  //                                                               See Note #6.
  NET_UTIL_VAL_SET_NET_16(&p_ip_hdr->ChkSum, 0x0000u);          // Clr  chk sum (see Note #6b).
                                                                // Calc chk sum.
#ifdef  NET_IPV4_CHK_SUM_OFFLOAD_TX
  ip_chk_sum = 0u;
#else
  ip_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc(p_ip_hdr,
                                                 ip_hdr_len_tot);
#endif

  NET_UTIL_VAL_COPY_16(&p_ip_hdr->ChkSum, &ip_chk_sum);         // Copy chk sum in net order (see Note #6c).
}

/****************************************************************************************************//**
 *                                           NetIPv4_TxPktDatagram()
 *
 * @brief    (1) Transmit IPv4 packet datagram :
 *
 *           - (a) Select next-route IPv4 address
 *           - (b) Transmit IPv4 packet datagram via next IPv4 address route :
 *
 *               - (1) Destination is this host                Send to Loopback Interface
 *                   - (A) Configured host address
 *                   - (B) Localhost       address
 *
 *               - (2) Limited Broadcast                       Send to Network  Interface Transmit
 *               - (3) Local   Host                            Send to Network  Interface Transmit
 *               - (4) Remote  Host                            Send to Network  Interface Transmit
 *
 * @param    p_buf       Pointer to network buffer to transmit IPv4 packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIPv4_TxPktDatagram(NET_BUF     *p_buf,
                                  NET_BUF_HDR *p_buf_hdr,
                                  RTOS_ERR    *p_err)
{
  NET_IP_TX_DEST_STATUS dest_status;

  //                                                               --------------- SEL NEXT-ROUTE ADDR ----------------
  dest_status = NetIPv4_TxPktDatagramRouteSel(p_buf_hdr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  switch (dest_status) {
    case NET_IP_TX_DEST_STATUS_LOCAL_HOST:
      p_buf_hdr->IF_NbrTx = NET_IF_NBR_LOCAL_HOST;
      break;

    case NET_IP_TX_DEST_STATUS_BROADCAST:
    case NET_IP_TX_DEST_STATUS_MULTICAST:
    case NET_IP_TX_DEST_STATUS_LOCAL_NET:
    case NET_IP_TX_DEST_STATUS_DFLT_GATEWAY:
      p_buf_hdr->IF_NbrTx = p_buf_hdr->IF_Nbr;
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_NEXT_HOP);
      goto exit;
  }

  //                                                               --------------- TX IPv4 PKT DATAGRAM ---------------
  NetIF_Tx(p_buf, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv4_TxPktDatagramRouteSel()
 *
 * @brief    (1) Configure  next-route IPv4 address for transmit IPv4 packet datagram :
 *
 *           - (a) Select next-route IPv4 address :                See Note #3
 *               - (1) Destination is this host :
 *                   - (A) Configured host address
 *                   - (B) Localhost       address                 See RFC #1122, Section 3.2.1.3.(g)
 *               - (2) Link-local                                  See Note #3c
 *               - (3) Limited Broadcast                           See Note #3b2a
 *               - (4) Multicast                                   See Note #3b2a
 *               - (5) Local  Net Host                             See Note #3b1b
 *               - (6) Remote Net Host                             See Note #3b1c
 *
 *           - (b) Configure next-route IPv4 address in network-order.
 *
 * @param    p_buf_hdr   Pointer to network buffer header of IPv4 transmit packet.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See 'net_ipv4.h  IPv4 ADDRESS DEFINES  Notes #2 & #3' for supported IPv4 addresses.
 *
 * @note     (3) See RFC #1122, Section 3.3.1
 *                       RFC #950, Section 2.2
 *                       RFC #1122, Section 3.2.1.3.(g)
 *                       RFC #1122, Section 3.3.1.1
 *                       RFC #950, Section 2.1
 *                       RFC #3927, Section 2.6.2
 *                       RFC #3927, Section 2.8
 *******************************************************************************************************/
static NET_IP_TX_DEST_STATUS NetIPv4_TxPktDatagramRouteSel(NET_BUF_HDR *p_buf_hdr,
                                                           RTOS_ERR    *p_err)
{
  NET_IPv4_ADDR_OBJ     *p_addr_obj;
  NET_IF_NBR            if_nbr;
  NET_IPv4_ADDR         addr_src;
  NET_IPv4_ADDR         addr_dest;
  CPU_BOOLEAN           addr_cfgd;
  NET_IP_TX_DEST_STATUS dest_status = NET_IP_TX_DEST_STATUS_NONE;

  //                                                               ----------- GET IPv4 TX PKT ADDRS ----------
  addr_src = p_buf_hdr->IP_AddrSrc;
  addr_dest = p_buf_hdr->IP_AddrDest;

  if_nbr = p_buf_hdr->IF_Nbr;
  p_addr_obj = NetIPv4_GetAddrObjCfgdOnIF(if_nbr, addr_src);

  //                                                               Perform IPv4 routing/fwd'ing alg here?

  //                                                               ---------- CHK CFG'D HOST ADDR(S) ----------
  //                                                               Chk cfg'd host addr(s)   [see Note #3].
  addr_cfgd = NetIPv4_IsAddrHostCfgdHandler(addr_dest);
  if (addr_cfgd == DEF_YES) {
    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestThisHostCtr);
    p_buf_hdr->IP_AddrNextRoute = addr_dest;
    dest_status = NET_IP_TX_DEST_STATUS_LOCAL_HOST;

    //                                                             ----------- CHK LOCALHOST ADDRS ------------
    //                                                             Chk localhost src   addr (see Note #3).
  } else if ((addr_src  & NET_IPv4_ADDR_LOCAL_HOST_MASK_NET)
             == NET_IPv4_ADDR_LOCAL_HOST_NET     ) {
    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestLocalHostCtr);
    p_buf_hdr->IP_AddrNextRoute = addr_dest;
    dest_status = NET_IP_TX_DEST_STATUS_LOCAL_HOST;
    //                                                             Chk localhost dest  addr (see Note #3).
  } else if ((addr_dest & NET_IPv4_ADDR_LOCAL_HOST_MASK_NET)
             == NET_IPv4_ADDR_LOCAL_HOST_NET     ) {
    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestLocalHostCtr);
    p_buf_hdr->IP_AddrNextRoute = addr_dest;
    dest_status = NET_IP_TX_DEST_STATUS_LOCAL_HOST;

    //                                                             ----------- CHK LINK-LOCAL ADDRS -----------
  } else if (((addr_src  & NET_IPv4_ADDR_LOCAL_LINK_MASK_NET)           // Chk link-local src  addr (see Note #3)    OR
              == NET_IPv4_ADDR_LOCAL_LINK_NET)
             || ((addr_dest & NET_IPv4_ADDR_LOCAL_LINK_MASK_NET)        // ... link-local dest addr (see Note #3).
                 == NET_IPv4_ADDR_LOCAL_LINK_NET)) {
    //                                                             Chk link-local broadcast (see Note #3).
    if ((addr_dest               & NET_IPv4_ADDR_LOCAL_LINK_MASK_HOST)
        == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_LOCAL_LINK_MASK_HOST)) {
      NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestBcastCtr);
      DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_TX_BROADCAST);
      p_buf_hdr->IP_AddrNextRoute = addr_dest;
      dest_status = NET_IP_TX_DEST_STATUS_BROADCAST;
    } else {
      p_buf_hdr->IP_AddrNextRoute = addr_dest;
      dest_status = NET_IP_TX_DEST_STATUS_LOCAL_NET;
    }

    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestLocalLinkCtr);
    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestLocalNetCtr);

    //                                                             ----------- CHK LIM'D BROADCAST ------------
  } else if (addr_dest == NET_IPv4_ADDR_BROADCAST) {                    // Chk lim'd broadcast (see Note #3).
    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestBcastCtr);
    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestLocalNetCtr);
    DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_TX_BROADCAST);
    p_buf_hdr->IP_AddrNextRoute = addr_dest;
    dest_status = NET_IP_TX_DEST_STATUS_BROADCAST;

#ifdef  NET_MCAST_MODULE_EN                                             // -------------- CHK MULTICAST ---------------
                                                                        // Chk multicast       (see Note #3).
  } else if ((addr_dest & NET_IPv4_ADDR_CLASS_D_MASK) == NET_IPv4_ADDR_CLASS_D) {
    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestMcastCtr);
    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestLocalNetCtr);
    DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_TX_MULTICAST);
    p_buf_hdr->IP_AddrNextRoute = addr_dest;
    dest_status = NET_IP_TX_DEST_STATUS_MULTICAST;
#endif

    //                                                             ------------- CHK REMOTE HOST --------------
  } else if (p_addr_obj == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_INVALID_ADDR_SRC);
    goto exit;

    //                                                             ------------- CHK LOCAL  NET ---------------
    //                                                             Chk local subnet           (see Note #3).
  } else if ((addr_dest & p_addr_obj->AddrHostSubnetMask)
             == p_addr_obj->AddrHostSubnetNet ) {
    //                                                             Chk local subnet broadcast (see Note #3).
    if ((addr_dest               & p_addr_obj->AddrHostSubnetMaskHost)
        == (NET_IPv4_ADDR_BROADCAST & p_addr_obj->AddrHostSubnetMaskHost)) {
      NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestBcastCtr);
      DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_TX_BROADCAST);
      p_buf_hdr->IP_AddrNextRoute = addr_dest;
      dest_status = NET_IP_TX_DEST_STATUS_BROADCAST;
    } else {
      p_buf_hdr->IP_AddrNextRoute = addr_dest;
      dest_status = NET_IP_TX_DEST_STATUS_LOCAL_NET;
    }

    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestLocalNetCtr);

    //                                                             ------------- CHK REMOTE  NET --------------
  } else {
    //                                                             Tx to remote net (see Note #3) ...
    if ((addr_dest & NET_IPv4_ADDR_CLASS_A_MASK) == NET_IPv4_ADDR_CLASS_A) {
      if ((addr_dest             & NET_IPv4_ADDR_CLASS_A_MASK_HOST)
          == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_CLASS_A_MASK_HOST)) {
        NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestBcastCtr);
      }
    } else if ((addr_dest & NET_IPv4_ADDR_CLASS_B_MASK) == NET_IPv4_ADDR_CLASS_B) {
      if ((addr_dest             & NET_IPv4_ADDR_CLASS_B_MASK_HOST)
          == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_CLASS_B_MASK_HOST)) {
        NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestBcastCtr);
      }
    } else if ((addr_dest & NET_IPv4_ADDR_CLASS_C_MASK) == NET_IPv4_ADDR_CLASS_C) {
      if ((addr_dest             & NET_IPv4_ADDR_CLASS_C_MASK_HOST)
          == (NET_IPv4_ADDR_BROADCAST & NET_IPv4_ADDR_CLASS_C_MASK_HOST)) {
        NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestBcastCtr);
      }
    } else {                                                            // Discard invalid addr class (see Note #2).
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.TxInvDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }

    if (p_addr_obj != DEF_NULL) {
      if (p_addr_obj->AddrDfltGateway == NET_IPv4_ADDR_NONE) {           // If dflt gateway NOT cfg'd, ...
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_NEXT_HOP);                     // ... rtn err (see Note #3).
        goto exit;
      }
    }

    NET_CTR_STAT_INC(Net_StatCtrs.IPv4.TxDestRemoteNetCtr);
    p_buf_hdr->IP_AddrNextRoute = p_addr_obj->AddrDfltGateway;          // ... via dflt gateway (see Note #3).
    dest_status = NET_IP_TX_DEST_STATUS_DFLT_GATEWAY;
  }

  //                                                               ---- CFG IPv4 NEXT ROUTE NET-ORDER ADDR ----
  p_buf_hdr->IP_AddrNextRouteNetOrder = NET_UTIL_HOST_TO_NET_32(p_buf_hdr->IP_AddrNextRoute);

exit:
  return (dest_status);
}

/****************************************************************************************************//**
 *                                               NetIPv4_ReTxPkt()
 *
 * @brief    (1) Prepare & re-transmit IPv4 packet :
 *
 *           - (a) Prepare     IPv4 header
 *           - (b) Re-transmit IPv4 packet datagram
 *
 * @param    p_buf       Pointer to network buffer to re-transmit IPv4 packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIPv4_ReTxPkt(NET_BUF     *p_buf,
                            NET_BUF_HDR *p_buf_hdr,
                            RTOS_ERR    *p_err)
{
  //                                                               ----------------- PREPARE IPv4 HDR -----------------
  NetIPv4_ReTxPktPrepareHdr(p_buf,
                            p_buf_hdr);

  //                                                               ------------- RE-TX IPv4 PKT DATAGRAM --------------
  NetIPv4_TxPktDatagram(p_buf, p_buf_hdr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv4_ReTxPktPrepareHdr()
 *
 * @brief    (1) Prepare IPv4 header for re-transmit IPv4 packet :
 *
 *           - (a) Update network buffer's protocol & length controls
 *
 *           - (b) Prepare the re-transmit packet's following IPv4 header fields :
 *
 *               - (1) Identification  (ID)                        See Note #2
 *               - (2) Check-Sum                                   See Note #3
 *
 *           - (c) Assumes the following IP header fields are already validated/prepared &
 *                   have NOT been modified :
 *
 *               - (1)  Version
 *               - (2)  Header Length
 *               - (3)  Type of Service (TOS)
 *               - (4)  Total  Length
 *               - (5)  Flags
 *               - (6)  Fragment Offset
 *               - (7)  Time-to-Live    (TTL)
 *               - (8)  Protocol
 *               - (9)  Source      Address
 *                   (10) Destination Address
 *                   (11) Options
 *
 *           - (d) Convert the following IPv4 header fields from host-order to network-order :
 *
 *               - (1) Identification (ID)
 *               - (2) Check-Sum                                       See Note #3c
 *
 * @param    p_buf       Pointer to network buffer to transmit IPv4 packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @note     (2) RFC #1122, Section 3.2.1.5 states that "some Internet protocol experts have maintained
 *                       that when a host sends an identical copy of an earlier datagram, the new copy should
 *                       contain the same Identification value as the original ... However, ... we believe that
 *                       retransmitting the same Identification field is not useful".
 *
 * @note     (3) IP header Check-Sum:
 *                           - (a) MUST be calculated AFTER the entire IPv4 header has been prepared.
 *                       In addition, ALL multi-octet words are converted from host-order to network-order
 *                       since "the sum of 16-bit integers can be computed in either byte order" [RFC #1071,
 *                       Section 2.(B)].
 *
 *                           - (b) IPv4 header Check-Sum field MUST be cleared to '0' BEFORE the IPv4 header Check-Sum is
 *                       calculated (see RFC #791, Section 3.1 'Header Checksum').
 *
 *                           - (c) The IPv4 header Check-Sum field is returned in network-order & MUST NOT be re-converted
 *                       back to host-order (see 'net_util.c  NetUtil_16BitOnesCplChkSumHdrCalc()  Note #3b').
 *******************************************************************************************************/
static void NetIPv4_ReTxPktPrepareHdr(NET_BUF     *p_buf,
                                      NET_BUF_HDR *p_buf_hdr)
{
  NET_IPv4_HDR *p_ip_hdr;
  CPU_INT16U   ip_id;
  CPU_INT16U   ip_hdr_len_tot;
  CPU_INT16U   ip_chk_sum;

  //                                                               ----------------- UPDATE BUF CTRLS -----------------
  p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V4;         // Update buf protocol for IPv4.
  p_buf_hdr->ProtocolHdrTypeNet = NET_PROTOCOL_TYPE_IP_V4;
  //                                                               Reset tot len for re-tx.
  p_buf_hdr->TotLen = (NET_BUF_SIZE)p_buf_hdr->IP_TotLen;

  //                                                               ----------------- PREPARE IPv4 HDR -----------------
  p_ip_hdr = (NET_IPv4_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];

  //                                                               ----------------- PREPARE IPv4 ID ------------------
  NET_IPv4_TX_GET_ID(ip_id);                                    // Get new IP ID (see Note #2).
  NET_UTIL_VAL_COPY_SET_NET_16(&p_ip_hdr->ID, &ip_id);

  //                                                               --------------- PREPARE IPv4 CHK SUM ---------------
  //                                                               See Note #3.
  NET_UTIL_VAL_SET_NET_16(&p_ip_hdr->ChkSum, 0x0000u);          // Clr  chk sum (see Note #3b).
  ip_hdr_len_tot = p_buf_hdr->IP_HdrLen;
#ifdef  NET_IPV4_CHK_SUM_OFFLOAD_TX                          // Calc chk sum.
  ip_chk_sum = 0u;
#else
  ip_chk_sum = NetUtil_16BitOnesCplChkSumHdrCalc(p_ip_hdr,
                                                 ip_hdr_len_tot);
#endif

  NET_UTIL_VAL_COPY_16(&p_ip_hdr->ChkSum, &ip_chk_sum);         // Copy chk sum in net order (see Note #3c).

  PP_UNUSED_PARAM(ip_hdr_len_tot);
}

/****************************************************************************************************//**
 *                                       NetIPv4_IsAddrHostHandler()
 *
 * @brief    (1) Validate an IPv4 address as an IPv4 host address :
 *
 *           - (a) A configured IPv4 host address (on an enabled interface)
 *           - (b) A 'Localhost'     IPv4 address
 *
 * @param    addr    IPv4 address to validate (see Note #3).
 *
 * @return   DEF_YES, if IPv4 address is one of the host's IP addresses.
 *           DEF_NO,  otherwise.
 *
 * @note     (2) NetIPv4_IsAddrHostHandler() is called by network protocol suite function(s) & MUST
 *               be called with the global network lock already acquired.
 *
 *               See also 'NetIPv4_IsAddrHost()  Note #2'.
 *
 * @note     (3) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
static CPU_BOOLEAN NetIPv4_IsAddrHostHandler(NET_IPv4_ADDR addr)
{
  NET_IF_NBR  if_nbr;
  CPU_BOOLEAN addr_host;

  if_nbr = NetIPv4_GetAddrHostIF_Nbr(addr);
  addr_host = (if_nbr != NET_IF_NBR_NONE) ? DEF_YES : DEF_NO;

  return (addr_host);
}

/****************************************************************************************************//**
 *                                       NetIPv4_IsAddrHostCfgdHandler()
 *
 * @brief    Validate an IPv4 address as a configured IPv4 host address on an enabled interface.
 *
 * @param    addr    IPv4 address to validate (see Note #2).
 *
 * @return   DEF_YES, if IPv4 address is one of the host's configured IPv4 addresses.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) NetIPv4_IsAddrHostCfgdHandler() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *
 * @note     (2) IPv4 address MUST be in host-order.
 *******************************************************************************************************/
static CPU_BOOLEAN NetIPv4_IsAddrHostCfgdHandler(NET_IPv4_ADDR addr)
{
  NET_IF_NBR  if_nbr;
  CPU_BOOLEAN addr_host;

  if_nbr = NetIPv4_GetAddrHostCfgdIF_Nbr(addr);
  addr_host = (if_nbr != NET_IF_NBR_NONE) ? DEF_YES : DEF_NO;

  return (addr_host);
}

/****************************************************************************************************//**
 *                                       NetIPv4_LinkSubscribeHook()
 *
 * @brief    IPv4 link local module's hook function for the interface link change.
 *
 * @param    if_nbr      Interface number.
 *
 * @param    link_state  Interface link state:
 *                       NET_IF_LINK_UP
 *                       NET_IF_LINK_DOWN
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static void NetIPv4_LinkSubscribeHook(NET_IF_NBR        if_nbr,
                                      NET_IF_LINK_STATE link_state)
{
  NET_IPv4_LINK_LOCAL_OBJ *p_obj;
  NET_IF                  *p_if;
  RTOS_ERR                local_err;

  p_if = NetIF_Get(if_nbr, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_INVALID_HANDLE,; );

  if (p_if->StartModulesCfgFlags.DHCPc == DEF_YES) {
    return;
  }

  SLIST_FOR_EACH_ENTRY(NetIPv4_LinkLocalObjListPtr, p_obj, NET_IPv4_LINK_LOCAL_OBJ, ListNode) {
    if (p_obj->IF_Nbr == if_nbr) {
      if (link_state == NET_IF_LINK_UP) {
        NetSvcTask_ChildMsgPost(NetIPv4_LinkLocalSvcTaskChildPtr,
                                NET_IPv4_LINK_LOCAL_MSG_TYPE_REBOOT_IF,
                                p_obj,
                                DEF_YES,
                                &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          return;
        }
      }
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_AddrLinkLocalHookOnMsg()
 *
 * @brief    The hook function for a message received by the Network Service Task and to be process
 *           by the IPv4 link local module child.
 *
 * @param    p_child     Pointer to the IPv4 Link local module Network Service Task Child.
 *
 * @param    msg_type    The message type.
 *
 * @param    p_msg_arg   Pointer to the message arguments.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static void NetIPv4_AddrLinkLocalHookOnMsg(NET_SVC_TASK_CHILD *p_child,
                                           CPU_INT32U         msg_type,
                                           void               *p_msg_arg,
                                           RTOS_ERR           *p_err)
{
  NET_IPv4_LINK_LOCAL_OBJ    *p_obj = DEF_NULL;
  NET_IPv4_LINK_LOCAL_STATUS status = NET_IPv4_LINK_LOCAL_STATUS_NONE;

  PP_UNUSED_PARAM(p_child);

  switch (msg_type) {
    case NET_IPv4_LINK_LOCAL_MSG_TYPE_NEW_IF:
      p_obj = (NET_IPv4_LINK_LOCAL_OBJ *)p_msg_arg;
      RTOS_ASSERT_CRITICAL_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

      SList_Push(&NetIPv4_LinkLocalObjListPtr, &p_obj->ListNode);

      NetIF_LinkStateSubscribe(p_obj->IF_Nbr,
                               NetIPv4_LinkSubscribeHook,
                               p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        status = NET_IPv4_LINK_LOCAL_STATUS_FAILED;
        goto exit_err;
      }
      break;

    case NET_IPv4_LINK_LOCAL_MSG_TYPE_START_IF:
      p_obj = (NET_IPv4_LINK_LOCAL_OBJ *)p_msg_arg;
      RTOS_ASSERT_CRITICAL_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

      status = NetIPv4_AddrLinkLocalCfgStart(p_obj, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_err;
      }
      break;

    case NET_IPv4_LINK_LOCAL_MSG_TYPE_REBOOT_IF:
      p_obj = (NET_IPv4_LINK_LOCAL_OBJ *)p_msg_arg;
      RTOS_ASSERT_CRITICAL_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

      status = NetIPv4_AddrLinkLocalCfgReboot(p_obj, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_err;
      }
      break;

    case NET_IPv4_LINK_LOCAL_MSG_TYPE_STOP_IF:
      p_obj = (NET_IPv4_LINK_LOCAL_OBJ *)p_msg_arg;
      RTOS_ASSERT_CRITICAL_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

      NetIPv4_AddrLinkLocalCfgStop(p_obj, NET_IPv4_LINK_LOCAL_STATUS_NONE, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
      break;

    default:
      break;
  }

  return;

exit_err:
  NetIPv4_AddrLinkLocalCfgStop(p_obj, status, p_err);
}
#endif

/****************************************************************************************************//**
 *                                   NetIPv4_AddrLinkLocalHookOnTmrTimeout()
 *
 * @brief    The hook function for a timed out timer by the Network Service Task and to be process
 *           by the IPv4 Link local module child.
 *
 * @param    p_child     Pointer to the IPv4 Link local module Network Service Task Child.
 *
 * @param    p_obj       Pointer to current IPv4 link local object.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static void NetIPv4_AddrLinkLocalHookOnTmrTimeout(NET_SVC_TASK_CHILD *p_child,
                                                  void               *p_obj,
                                                  RTOS_ERR           *p_err)
{
  NET_IPv4_LINK_LOCAL_OBJ *p_link_local_obj;
  NET_IPv4_ADDR           addr_net;
  CPU_INT08U              rtn_len;
  CPU_INT08U              hw_addr_sender[NET_CACHE_HW_ADDR_LEN_ETHER];

  PP_UNUSED_PARAM(p_child);

  p_link_local_obj = (NET_IPv4_LINK_LOCAL_OBJ *)p_obj;

  p_link_local_obj->SvcTaskTmr = DEF_NULL;

  switch (p_link_local_obj->State) {
    case NET_IPv4_LINK_LOCAL_STATE_VALIDATE_ADDR:

      addr_net = (NET_IPv4_ADDR)NET_UTIL_HOST_TO_NET_32(p_link_local_obj->Addr);

      //                                                           ------------ GET HW ADDR FROM ARP CACHE ------------
      rtn_len = NetARP_CacheGetAddrHW(p_link_local_obj->IF_Nbr,
                                      &hw_addr_sender[0],
                                      NET_CACHE_HW_ADDR_LEN_ETHER,
                                      (CPU_INT08U *)&addr_net,
                                      sizeof(addr_net),
                                      p_err);
      if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
          && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NET_ADDR_UNRESOLVED)
          && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NOT_FOUND)) {
        goto exit_err;
      }

      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

      if (rtn_len == 0) {                                       // ... addr NOT used.
        NetIPv4_AddrLinkLocalAdd(p_link_local_obj, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit_err;
        }
      } else {                                                  // ... addr is used.
        NetIPv4_AddrLinkLocalCreate(p_link_local_obj, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit_err;
        }
        p_link_local_obj->State = NET_IPv4_LINK_LOCAL_STATE_VALIDATE_ADDR;
      }
      break;

    case NET_IPv4_LINK_LOCAL_STATE_ANNOUNCE_ADDR:
      NetIPv4_AddrLinkLocalAnnounce(p_link_local_obj, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_err;
      }
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      goto exit_err;
  }

  return;

exit_err:
  NetIPv4_AddrLinkLocalCfgStop(p_obj, NET_IPv4_LINK_LOCAL_STATUS_FAILED, p_err);
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_AddrLinkLocalCfgStart()
 *
 * @brief    Start the IPv4 link local process.
 *
 * @param    p_obj   Pointer to the IPv4 link local object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Process status :
 *
 *               NET_IPv4_LINK_LOCAL_STATUS_NONE
 *               NET_IPv4_LINK_LOCAL_STATUS_SUCCEEDED
 *               NET_IPv4_LINK_LOCAL_STATUS_FAILED
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static NET_IPv4_LINK_LOCAL_STATUS NetIPv4_AddrLinkLocalCfgStart(NET_IPv4_LINK_LOCAL_OBJ *p_obj,
                                                                RTOS_ERR                *p_err)
{
  CPU_BOOLEAN is_cfgd = DEF_NO;

  switch (p_obj->State) {
    case NET_IPv4_LINK_LOCAL_STATE_INIT:
      //                                                           ------- CHECK IS ADDRESS ALREADY CFGD ON IF --------
      is_cfgd = NetIPv4_IsAddrsCfgdOnIF(p_obj->IF_Nbr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (NET_IPv4_LINK_LOCAL_STATUS_FAILED);
      }

      if (is_cfgd == DEF_OK) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
        return (NET_IPv4_LINK_LOCAL_STATUS_NONE);
      }

      NetIPv4_AddrLinkLocalCreate(p_obj, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return (NET_IPv4_LINK_LOCAL_STATUS_FAILED);
      }

      p_obj->State = NET_IPv4_LINK_LOCAL_STATE_VALIDATE_ADDR;
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
  }

  return (NET_IPv4_LINK_LOCAL_STATUS_NONE);
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_AddrLinkLocalCfgReboot()
 *
 * @brief    Reboot the IPv4 link local process.
 *
 * @param    p_obj   Pointer to the IPv4 link local object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Process status :
 *
 *               NET_IPv4_LINK_LOCAL_STATUS_NONE
 *               NET_IPv4_LINK_LOCAL_STATUS_SUCCEEDED
 *               NET_IPv4_LINK_LOCAL_STATUS_FAILED
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static NET_IPv4_LINK_LOCAL_STATUS NetIPv4_AddrLinkLocalCfgReboot(NET_IPv4_LINK_LOCAL_OBJ *p_obj,
                                                                 RTOS_ERR                *p_err)
{
  NET_IPv4_LINK_LOCAL_STATUS status = NET_IPv4_LINK_LOCAL_STATUS_NONE;

  NetIPv4_CfgAddrLinkLocalStop(p_obj->IF_Nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (NET_IPv4_LINK_LOCAL_STATUS_FAILED);
  }

  p_obj->State = NET_IPv4_LINK_LOCAL_STATE_INIT;

  status = NetIPv4_AddrLinkLocalCfgStart(p_obj, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (status);
  }

  return (status);
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_AddrLinkLocalCfgStop()
 *
 * @brief    Stop the IPv4 link local process.
 *
 * @param    p_obj   Pointer to the IPv4 link local object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static void NetIPv4_AddrLinkLocalCfgStop(NET_IPv4_LINK_LOCAL_OBJ    *p_obj,
                                         NET_IPv4_LINK_LOCAL_STATUS status,
                                         RTOS_ERR                   *p_err)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  if (p_obj->SvcTaskTmr != DEF_NULL) {
    NetSvcTask_TmrDel(p_obj->SvcTaskTmr, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      goto exit_err;
    }
  }

  NetIPv4_CfgAddrLinkLocalStop(p_obj->IF_Nbr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit_err;
  }

  NetIF_LinkStateUnsubscribe(p_obj->IF_Nbr,
                             NetIPv4_LinkSubscribeHook,
                             &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit_err;
  }

  p_obj->State = NET_IPv4_LINK_LOCAL_STATE_INIT;

exit_err:
  if (p_obj->CompleteHook != DEF_NULL) {
    RTOS_ERR rtn_err;

    rtn_err = *p_err;

    p_obj->CompleteHook(p_obj->IF_Nbr, p_obj->Addr, status, rtn_err);
  }

  Mem_DynPoolBlkFree(&NetIPv4_LinkLocalObjPool, p_obj, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  SList_Rem(&NetIPv4_LinkLocalObjListPtr, &p_obj->ListNode);
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_AddrLinkLocalCreate()
 *
 * @brief    Create the IPv4 link local address and probe the network for the address.
 *
 * @param    p_obj   Pointer to the IPv4 link local object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static void NetIPv4_AddrLinkLocalCreate(NET_IPv4_LINK_LOCAL_OBJ *p_obj,
                                        RTOS_ERR                *p_err)
{
  NET_IPv4_ADDR addr_host;
  NET_IPv4_ADDR addr_net;
  NET_IPv4_ADDR addr_this_host;
  CPU_INT08U    addr_hw_len = NET_IF_802x_HW_ADDR_LEN;
  CPU_INT08U    hw_addr[NET_IF_802x_HW_ADDR_LEN];
  CPU_INT32U    delay_ms;

  NetIF_AddrHW_Get(p_obj->IF_Nbr, &hw_addr[0], &addr_hw_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  //                                                               ---------- CREATE IPV4 LINK-LOCAL ADDRESS ----------
  addr_host = NetIPv4_AddrLocalLinkGet(&hw_addr[0], addr_hw_len);
  addr_net = (NET_IPv4_ADDR)NET_UTIL_HOST_TO_NET_32(addr_host);

  p_obj->Addr = addr_host;

  NetIPv4_CfgAddrAutoCfgStart(p_obj->IF_Nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  addr_this_host = NET_IPv4_ADDR_NONE;

  //                                                               ---------------- PROBE ADDR ON NET -----------------
  NetARP_CacheProbeAddrOnNet(NET_PROTOCOL_TYPE_IP_V4,
                             (CPU_INT08U *)&addr_this_host,
                             (CPU_INT08U *)&addr_net,
                             sizeof(addr_net),
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  delay_ms = (NET_ARP_REQ_RETRY_TIMEOUT_DFLT_SEC * NET_ARP_REQ_RETRY_DFLT + 1) * DEF_TIME_NBR_mS_PER_SEC;

  if (p_obj->SvcTaskTmr == DEF_NULL) {
    p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(NetIPv4_LinkLocalSvcTaskChildPtr,
                                             NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                             NetIPv4_AddrLinkLocalHookOnTmrTimeout,
                                             p_obj,
                                             delay_ms,
                                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  } else {
    NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                      NetIPv4_AddrLinkLocalHookOnTmrTimeout,
                      p_obj,
                      delay_ms,
                      p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_AddrLinkLocalAdd()
 *
 * @brief    Add the IPv4 link local address on the interface address list.
 *
 * @param    p_obj   Pointer to the IPv4 link local object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static void NetIPv4_AddrLinkLocalAdd(NET_IPv4_LINK_LOCAL_OBJ *p_obj,
                                     RTOS_ERR                *p_err)
{
  CPU_BOOLEAN is_cfgd;

  is_cfgd = NetIPv4_CfgAddrAddAutoCfg(p_obj->IF_Nbr,
                                      p_obj->Addr,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (is_cfgd != DEF_OK) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return;
  }

  p_obj->AnnounceNbr = 0u;

  NetIPv4_AddrLinkLocalAnnounce(p_obj, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_AddrLinkLocalAnnounce()
 *
 * @brief    Announce the new IPv4 link local address that has been configured on the interface.
 *
 * @param    p_obj   Pointer to the IPv4 link local object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static void NetIPv4_AddrLinkLocalAnnounce(NET_IPv4_LINK_LOCAL_OBJ *p_obj,
                                          RTOS_ERR                *p_err)
{
  NET_IPv4_ADDR addr_net;
  CPU_INT32U    delay_ms;

  addr_net = (NET_IPv4_ADDR)NET_UTIL_HOST_TO_NET_32(p_obj->Addr);

  if (p_obj->AnnounceNbr < NET_IPv4_LOCAL_LINK_ANNOUNCE_NUM) {
    NetARP_TxReqGratuitous(NET_PROTOCOL_TYPE_IP_V4,
                           (CPU_INT08U *)&addr_net,
                           sizeof(NET_IPv4_ADDR),
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_obj->AnnounceNbr++;

    delay_ms = NET_IPv4_LOCAL_LINK_ANNOUNCE_INTERVAL_S * DEF_TIME_NBR_mS_PER_SEC;

    if (p_obj->SvcTaskTmr == DEF_NULL) {
      p_obj->SvcTaskTmr = NetSvcTask_TmrCreate(NetIPv4_LinkLocalSvcTaskChildPtr,
                                               NET_SVC_TASK_TMR_TYPE_ONE_SHOT,
                                               NetIPv4_AddrLinkLocalHookOnTmrTimeout,
                                               p_obj,
                                               delay_ms,
                                               p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    } else {
      NetSvcTask_TmrSet(p_obj->SvcTaskTmr,
                        NetIPv4_AddrLinkLocalHookOnTmrTimeout,
                        p_obj,
                        delay_ms,
                        p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }
    }

    p_obj->State = NET_IPv4_LINK_LOCAL_STATE_ANNOUNCE_ADDR;
  } else {
    if (p_obj->CompleteHook != DEF_NULL) {
      RTOS_ERR local_err;

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      p_obj->CompleteHook(p_obj->IF_Nbr, p_obj->Addr, NET_IPv4_LINK_LOCAL_STATUS_SUCCEEDED, local_err);
    }
    p_obj->State = NET_IPv4_LINK_LOCAL_STATE_INIT;
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_AddrLocalLinkGet()
 *
 * @brief    Create a IPv4 Link local address from the 169.254.0.0/16 prefix, the harware address and
 *           a random parameter.
 *
 * @param    p_addr_hw       Pointer to the interface hardware address.
 *
 * @param    addr_hw_len     Hardware address length.
 *
 * @return   IPv4 link local address.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static NET_IPv4_ADDR NetIPv4_AddrLocalLinkGet(CPU_INT08U *p_addr_hw,
                                              CPU_INT08U addr_hw_len)
{
  KAL_TICK      time_cur;
  CPU_INT32U    seed_hw_addr;
  CPU_INT32U    seed_time;
  CPU_INT32U    random;
  NET_IPv4_ADDR addr;
  RTOS_ERR      local_err;

  //                                                               ------------------ GENERATE SEEDS ------------------
  seed_hw_addr = 0;
  seed_hw_addr = ((((CPU_INT32U) p_addr_hw + (addr_hw_len - 1)) << 24)
                  | (((CPU_INT32U) p_addr_hw + (addr_hw_len)) << 16));

  time_cur = KAL_TickGet(&local_err);
  seed_time = (time_cur & 0x0000FFFF);

  //                                                               ---------------- GET PSEUDO-RAND NBR ---------------
  random = (seed_time | seed_hw_addr);                          // OR the two seeds.

  //                                                               ------------------- GENERATE ADDR ------------------
  addr = NET_IPv4_ADDR_LOCAL_LINK_HOST_MIN
         + (random % (NET_IPv4_ADDR_LOCAL_LINK_HOST_MAX - NET_IPv4_ADDR_LOCAL_LINK_HOST_MIN + 1));

  return (addr);
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_CfgAddrAutoCfgStart()
 *
 * @brief    Start the IPv4 Link local address configuration process on the given interface.
 *
 * @param    if_nbr  Interface number on which to start the IPv4 link local process.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK, if process started successfully.
 *           DEF_FAIl, otherwise.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static CPU_BOOLEAN NetIPv4_CfgAddrAutoCfgStart(NET_IF_NBR if_nbr,
                                               RTOS_ERR   *p_err)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
  CPU_BOOLEAN       found = DEF_NO;
  CPU_BOOLEAN       result = DEF_FAIL;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_en = DEF_NO;
#endif

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------- ACQUIRE NET LOCK -------------
  Net_GlobalLockAcquire((void *)NetIPv4_CfgAddrAutoCfgStart);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ------------- VALIDATE IF NBR --------------
  is_en = NetIF_IsEnCfgdHandler(if_nbr, p_err);
  if (is_en != DEF_YES) {
    goto exit_release;
  }
#endif

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
    if (p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_AUTO_CFG) {
      found = DEF_YES;
      break;
    }
  }

  if (found != DEF_YES) {
    //                                                             ------------- RETRIEVE ADDR MEM BLOCK --------------
    p_addr_obj = (NET_IPv4_ADDR_OBJ *)Mem_DynPoolBlkGet(&NetIPv4_AddrPool, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_release;
    }

    //                                                             ---------- ADD ADDRESS TO IPV4 ADDR LIST -----------
    SList_Push(&p_if->IP_Obj->IPv4_AddrListPtr, &p_addr_obj->ListNode);
  }

  p_addr_obj->CfgMode = NET_IP_ADDR_CFG_MODE_AUTO_CFG;
  p_addr_obj->AddrHost = NET_IPv4_ADDR_NONE;
  p_addr_obj->AddrHostSubnetMask = NET_IPv4_ADDR_NONE;
  p_addr_obj->AddrHostSubnetMaskHost = NET_IPv4_ADDR_NONE;
  p_addr_obj->AddrHostSubnetNet = NET_IPv4_ADDR_NONE;
  p_addr_obj->AddrDfltGateway = NET_IPv4_ADDR_NONE;
  p_addr_obj->IsValid = DEF_NO;

  result = DEF_OK;

exit_release:
  //                                                               ------------- RELEASE NET LOCK -------------
  Net_GlobalLockRelease();

  return (result);
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_CfgAddrAddAutoCfg()
 *
 * @brief    Add the IPv4 Link local address on the given interface.
 *
 * @param    if_nbr      Interface number on which to add the Ipv4 link local address.
 *
 * @param    addr_host   IPv4 link local address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK, if address added successfully.
 *           DEF_FAIl, otherwise.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static CPU_BOOLEAN NetIPv4_CfgAddrAddAutoCfg(NET_IF_NBR    if_nbr,
                                             NET_IPv4_ADDR addr_host,
                                             RTOS_ERR      *p_err)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
  CPU_BOOLEAN       addr_cfgd = DEF_NO;
  CPU_BOOLEAN       found = DEF_NO;
  CPU_BOOLEAN       result = DEF_FAIL;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_en = DEF_NO;
#endif

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIPv4_CfgAddrAddAutoCfg);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_en = NetIF_IsEnCfgdHandler(if_nbr, p_err);
  if (is_en != DEF_YES) {
    goto exit_release;
  }

  //                                                               --------------- VALIDATE IPv4 ADDRS ----------------
  NetIPv4_CfgAddrValidate(addr_host, NET_IPv4_ADDR_LOCAL_LINK_NET, NET_IPv4_ADDR_NONE, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }
#endif

  //                                                               Validate host addr is not already configured.
  addr_cfgd = NetIPv4_IsAddrHostCfgdHandler(addr_host);
  if (addr_cfgd != DEF_NO) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv4.CfgInvAddrInUseCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    goto exit;
  }

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
    if (p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_AUTO_CFG) {
      p_addr_obj->CfgMode = NET_IP_ADDR_CFG_MODE_AUTO_CFG;
      p_addr_obj->AddrHost = addr_host;
      p_addr_obj->AddrHostSubnetMask = NET_IPv4_ADDR_LOCAL_LINK_NET;
      p_addr_obj->AddrHostSubnetMaskHost = ~NET_IPv4_ADDR_LOCAL_LINK_NET;
      p_addr_obj->AddrHostSubnetNet = addr_host & NET_IPv4_ADDR_LOCAL_LINK_NET;
      p_addr_obj->AddrDfltGateway = NET_IPv4_ADDR_NONE;
      p_addr_obj->IsValid = DEF_YES;
      found = DEF_YES;
      break;
    }
  }

  if (found != DEF_YES) {
    goto exit_release;
  }

  result = DEF_OK;

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
exit:
  return (result);
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv4_CfgAddrLinkLocalStop()
 *
 * @brief    Stop the IPv4 Link local address configuration process on the given interface.
 *
 * @param    if_nbr  Interface number on which to remove the link local address.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK, if process stopped successfully.
 *           DEF_FAIl, otherwise.
 *******************************************************************************************************/
#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
static CPU_BOOLEAN NetIPv4_CfgAddrLinkLocalStop(NET_IF_NBR if_nbr,
                                                RTOS_ERR   *p_err)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv4_ADDR_OBJ *p_addr_obj = DEF_NULL;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_cfgd = DEF_NO;
#endif
  CPU_BOOLEAN found = DEF_NO;
  CPU_BOOLEAN result = DEF_FAIL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------- ACQUIRE NET LOCK -------------
  //                                                               See Note #2b.
  Net_GlobalLockAcquire((void *)NetIPv4_CfgAddrLinkLocalStop);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ------------- VALIDATE IF NBR --------------
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit_release;
  }
#endif

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
    if (p_addr_obj->CfgMode == NET_IP_ADDR_CFG_MODE_AUTO_CFG) {
      found = DEF_YES;
      break;
    }
  }

  if (found == DEF_YES) {
    NET_IPv4_ADDR addr_cfgd;

    //                                                             -------- CLOSE ALL IPv4 ADDR CONNS ---------
    //                                                             Close all cfg'd addr's conns.
    NET_UTIL_VAL_SET_NET_32(&addr_cfgd, p_addr_obj->AddrHost);
    NetConn_CloseAllConnsByAddrHandler((CPU_INT08U *)&addr_cfgd,
                                       sizeof(addr_cfgd));

    SList_Rem(&p_if->IP_Obj->IPv4_AddrListPtr, &p_addr_obj->ListNode);
    Mem_DynPoolBlkFree(&NetIPv4_AddrPool, p_addr_obj, p_err);
    Net_GlobalLockRelease();
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_FAIL);
    result = DEF_OK;
    goto exit;
  }

  result = DEF_OK;

exit_release:
  //                                                               ------------- RELEASE NET LOCK -------------
  Net_GlobalLockRelease();

exit:
  return (result);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IPv4_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
