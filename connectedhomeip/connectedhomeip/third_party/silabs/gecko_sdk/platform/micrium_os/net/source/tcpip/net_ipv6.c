/***************************************************************************//**
 * @file
 * @brief Network Ip Layer Version 6 - (Internet Protocol V6)
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

#ifdef  NET_IPv6_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <net/include/net_util.h>
#include  <net/include/net_if.h>

#include  "net_ipv6_priv.h"
#include  "net_ndp_priv.h"
#include  "net_icmpv6_priv.h"
#include  "net_mldp_priv.h"
#include  "net_dad_priv.h"
#include  "net_buf_priv.h"
#include  "net_conn_priv.h"
#include  "net_tcp_priv.h"
#include  "net_udp_priv.h"
#include  "net_priv.h"
#include  "net_if_priv.h"
#include  "net_ctr_priv.h"

#include  <common/source/kal/kal_priv.h>
#include  <common/include/lib_math.h>
#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (NET)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_NET

#define  NET_IPv6_ADDR_LOCAL_HOST_ADDR                   NET_IPv6_ADDR_ANY_INIT

/********************************************************************************************************
   IPv6 AUTO CFG DEFINES
 *******************************************************************************************************/

#define  NET_IPv6_AUTO_CFG_RAND_RETRY_MAX                3

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const NET_IPv6_ADDR NetIPv6_AddrAny = NET_IPv6_ADDR_ANY;
const NET_IPv6_ADDR NetIPv6_AddrNone = NET_IPv6_ADDR_NONE;
const NET_IPv6_ADDR NetIPv6_AddrWildcard = NET_IPv6_ADDR_WILDCARD;
const NET_IPv6_ADDR NetIPv6_AddrLoopback = NET_IPv6_ADDR_LOOPBACK;
const NET_IPv6_ADDR NetIPv6_AddrLinkLocalAllNodes = NET_IPv6_ADDR_LINKLOCAL_ALLNODES;
const NET_IPv6_ADDR NetIPv6_AddrLinkLocalAllRouters = NET_IPv6_ADDR_LINKLOCAL_ALLROUTERS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       IPv6 POLICY TABLE DATA TYPE
 *
 * Note(s) : (1) The policy table is a longest-matching-prefix lookup table used for source and
 *               destination address selection algorithm.
 *
 *           (2) See the RFC #6724 'Default Address Selection for Internet Protocol Version 6' for more
 *               details.
 *******************************************************************************************************/

static const NET_IPv6_ADDR NET_IPv6_POLICY_MASK_128 = { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } };
static const NET_IPv6_ADDR NET_IPv6_POLICY_MASK_096 = { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0    } };
static const NET_IPv6_ADDR NET_IPv6_POLICY_MASK_016 = { { 0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0    } };
static const NET_IPv6_ADDR NET_IPv6_POLICY_MASK_032 = { { 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0    } };
static const NET_IPv6_ADDR NET_IPv6_POLICY_MASK_007 = { { 0XFE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0    } };
static const NET_IPv6_ADDR NET_IPv6_POLICY_MASK_010 = { { 0XFF, 0XC0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0    } };

typedef  struct  net_ipv6_policy {
  const NET_IPv6_ADDR *PrefixAddrPtr;
  const NET_IPv6_ADDR *PrefixMaskPtr;
  const CPU_INT08U    Precedence;
  const CPU_INT08U    Label;
} NET_IPv6_POLICY;

/*--------------------------------------------------------------------------------------------------------
   -                                              POLICY 01
   -------------------------------------------------------------------------------------------------------*/

#define  NET_IPv6_POLICY_01_PREFIX_ADDR_PTR       &NetIPv6_AddrLoopback
#define  NET_IPv6_POLICY_01_MASK_PTR              &NET_IPv6_POLICY_MASK_128
#define  NET_IPv6_POLICY_01_PRECEDENCE             50
#define  NET_IPv6_POLICY_01_LABEL                   0
static const NET_IPv6_POLICY NetIPv6_Policy_01 = {
  NET_IPv6_POLICY_01_PREFIX_ADDR_PTR,
  NET_IPv6_POLICY_01_MASK_PTR,
  NET_IPv6_POLICY_01_PRECEDENCE,
  NET_IPv6_POLICY_01_LABEL
};

/*--------------------------------------------------------------------------------------------------------
   -                                              POLICY 02
   -------------------------------------------------------------------------------------------------------*/

#define  NET_IPv6_POLICY_02_PREFIX_ADDR_PTR       &NetIPv6_AddrAny
#define  NET_IPv6_POLICY_02_MASK_PTR              &NetIPv6_AddrAny
#define  NET_IPv6_POLICY_02_PRECEDENCE             40
#define  NET_IPv6_POLICY_02_LABEL                   1
static const NET_IPv6_POLICY NetIPv6_Policy_02 = {
  NET_IPv6_POLICY_02_PREFIX_ADDR_PTR,
  NET_IPv6_POLICY_02_MASK_PTR,
  NET_IPv6_POLICY_02_PRECEDENCE,
  NET_IPv6_POLICY_02_LABEL
};

/*--------------------------------------------------------------------------------------------------------
   -                                              POLICY 03
   -------------------------------------------------------------------------------------------------------*/

static const NET_IPv6_ADDR NET_IPv6_POLICY_03_ADDR = { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0, 0 } };

#define  NET_IPv6_POLICY_03_PREFIX_ADDR_PTR       &NET_IPv6_POLICY_03_ADDR
#define  NET_IPv6_POLICY_03_MASK_PTR              &NET_IPv6_POLICY_MASK_096
#define  NET_IPv6_POLICY_03_PRECEDENCE             35
#define  NET_IPv6_POLICY_03_LABEL                   4
static const NET_IPv6_POLICY NetIPv6_Policy_03 = {
  NET_IPv6_POLICY_03_PREFIX_ADDR_PTR,
  NET_IPv6_POLICY_03_MASK_PTR,
  NET_IPv6_POLICY_03_PRECEDENCE,
  NET_IPv6_POLICY_03_LABEL
};

/*--------------------------------------------------------------------------------------------------------
   -                                              POLICY 04
   -------------------------------------------------------------------------------------------------------*/

static const NET_IPv6_ADDR NET_IPv6_POLICY_04_ADDR = { { 0x20, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
#define  NET_IPv6_POLICY_04_PREFIX_ADDR_PTR       &NET_IPv6_POLICY_04_ADDR
#define  NET_IPv6_POLICY_04_MASK_PTR              &NET_IPv6_POLICY_MASK_016
#define  NET_IPv6_POLICY_04_PRECEDENCE             30
#define  NET_IPv6_POLICY_04_LABEL                   2

static const NET_IPv6_POLICY NetIPv6_Policy_04 = {
  NET_IPv6_POLICY_04_PREFIX_ADDR_PTR,
  NET_IPv6_POLICY_04_MASK_PTR,
  NET_IPv6_POLICY_04_PRECEDENCE,
  NET_IPv6_POLICY_04_LABEL
};

/*--------------------------------------------------------------------------------------------------------
   -                                              POLICY 05
   -------------------------------------------------------------------------------------------------------*/

static const NET_IPv6_ADDR NET_IPv6_POLICY_05_ADDR = { { 0x20, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
#define  NET_IPv6_POLICY_05_PREFIX_ADDR_PTR       &NET_IPv6_POLICY_05_ADDR
#define  NET_IPv6_POLICY_05_MASK_PTR              &NET_IPv6_POLICY_MASK_032
#define  NET_IPv6_POLICY_05_PRECEDENCE              5
#define  NET_IPv6_POLICY_05_LABEL                   5

static const NET_IPv6_POLICY NetIPv6_Policy_05 = {
  NET_IPv6_POLICY_05_PREFIX_ADDR_PTR,
  NET_IPv6_POLICY_05_MASK_PTR,
  NET_IPv6_POLICY_05_PRECEDENCE,
  NET_IPv6_POLICY_05_LABEL
};

/*--------------------------------------------------------------------------------------------------------
   -                                              POLICY 06
   -------------------------------------------------------------------------------------------------------*/

static const NET_IPv6_ADDR NET_IPv6_POLICY_06_ADDR = { { 0xFC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
#define  NET_IPv6_POLICY_06_PREFIX_ADDR_PTR       &NET_IPv6_POLICY_06_ADDR
#define  NET_IPv6_POLICY_06_MASK_PTR              &NET_IPv6_POLICY_MASK_007
#define  NET_IPv6_POLICY_06_PRECEDENCE              3
#define  NET_IPv6_POLICY_06_LABEL                  13

static const NET_IPv6_POLICY NetIPv6_Policy_06 = {
  NET_IPv6_POLICY_06_PREFIX_ADDR_PTR,
  NET_IPv6_POLICY_06_MASK_PTR,
  NET_IPv6_POLICY_06_PRECEDENCE,
  NET_IPv6_POLICY_06_LABEL
};

/*--------------------------------------------------------------------------------------------------------
   -                                              POLICY 07
   -------------------------------------------------------------------------------------------------------*/

#define  NET_IPv6_POLICY_07_PREFIX_ADDR_PTR       &NetIPv6_AddrAny
#define  NET_IPv6_POLICY_07_MASK_PTR              &NET_IPv6_POLICY_MASK_096
#define  NET_IPv6_POLICY_07_PRECEDENCE              1
#define  NET_IPv6_POLICY_07_LABEL                   3

static const NET_IPv6_POLICY NetIPv6_Policy_07 = {
  NET_IPv6_POLICY_07_PREFIX_ADDR_PTR,
  NET_IPv6_POLICY_07_MASK_PTR,
  NET_IPv6_POLICY_07_PRECEDENCE,
  NET_IPv6_POLICY_07_LABEL
};

/*--------------------------------------------------------------------------------------------------------
   -                                              POLICY 08
   -------------------------------------------------------------------------------------------------------*/

static const NET_IPv6_ADDR NET_IPv6_POLICY_08_ADDR = { { 0xFE, 0xC0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
#define  NET_IPv6_POLICY_08_PREFIX_ADDR_PTR       &NET_IPv6_POLICY_08_ADDR
#define  NET_IPv6_POLICY_08_MASK_PTR              &NET_IPv6_POLICY_MASK_010
#define  NET_IPv6_POLICY_08_PRECEDENCE              1
#define  NET_IPv6_POLICY_08_LABEL                  11

static const NET_IPv6_POLICY NetIPv6_Policy_08 = {
  NET_IPv6_POLICY_08_PREFIX_ADDR_PTR,
  NET_IPv6_POLICY_08_MASK_PTR,
  NET_IPv6_POLICY_08_PRECEDENCE,
  NET_IPv6_POLICY_08_LABEL
};

/*--------------------------------------------------------------------------------------------------------
   -                                              POLICY 09
   -------------------------------------------------------------------------------------------------------*/

static const NET_IPv6_ADDR NET_IPv6_POLICY_09_ADDR = { { 0x3F, 0xFE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
#define  NET_IPv6_POLICY_09_PREFIX_ADDR_PTR       &NET_IPv6_POLICY_09_ADDR
#define  NET_IPv6_POLICY_09_MASK_PTR              &NET_IPv6_POLICY_MASK_016
#define  NET_IPv6_POLICY_09_PRECEDENCE              1
#define  NET_IPv6_POLICY_09_LABEL                  12

static const NET_IPv6_POLICY NetIPv6_Policy_09 = {
  NET_IPv6_POLICY_09_PREFIX_ADDR_PTR,
  NET_IPv6_POLICY_09_MASK_PTR,
  NET_IPv6_POLICY_09_PRECEDENCE,
  NET_IPv6_POLICY_09_LABEL
};

/*--------------------------------------------------------------------------------------------------------
   -                                            POLICY TABLE
   -------------------------------------------------------------------------------------------------------*/

//                                                                 IPv6 Policy table
static const NET_IPv6_POLICY *NetIPv6_PolicyTbl[] = {
  &NetIPv6_Policy_01,
  &NetIPv6_Policy_02,
  &NetIPv6_Policy_03,
  &NetIPv6_Policy_04,
  &NetIPv6_Policy_05,
  &NetIPv6_Policy_06,
  &NetIPv6_Policy_07,
  &NetIPv6_Policy_08,
  &NetIPv6_Policy_09,
};

#define NET_IPv6_POLICY_TBL_SIZE            (sizeof(NetIPv6_PolicyTbl))

/********************************************************************************************************
 *                                   IPV6 ADDRESS CFG SUBSCRIBE OBJECT
 *******************************************************************************************************/

typedef  struct  net_ipv6_addr_subcribe_obj {
  NET_IPv6_ADDR_HOOK_FNCT Fnct;
  CPU_INT08U              RefCtn;
  SLIST_MEMBER            ListNode;
} NET_IPv6_ADDR_SUBSCRIBE_OBJ;

/********************************************************************************************************
 *                                   IPV6 ADDRESS TO CONFIGURE OBJECT
 *******************************************************************************************************/

typedef  struct  net_ipv6_addr_to_cfg {
  NET_IPv6_ADDR Addr;
  CPU_INT08U    PrefixLen;
  CPU_BOOLEAN   DAD_En;
  SLIST_MEMBER  ListNode;
} NET_IPv6_ADDR_TO_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_BUF *NetIPv6_FragReasmListsHead;                           // Ptr to head of frag reasm lists.
static NET_BUF *NetIPv6_FragReasmListsTail;                           // Ptr to tail of frag reasm lists.

static CPU_INT32U NetIPv6_FragReasmTimeout_ms;                        // IPv6 frag reasm timeout (in ms).

static CPU_INT32U NetIPv6_TxID_Ctr;                                   // Global tx ID field ctr.

#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
static MEM_DYN_POOL NetIPv6_AutoCfgObjPool;
#endif

static MEM_DYN_POOL NetIPv6_AddrPool;

static MEM_DYN_POOL NetIPv6_AddrSubscriberPool;
static SLIST_MEMBER *NetIPv6_AddrSubscriberListHeadPtr;

static MEM_DYN_POOL NetIPv6_AddrToCfgPool;
static SLIST_MEMBER *NetIPv6_AddrToCfgListPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------- IPv6 AUTO CFG FNCTS ----------
#ifdef NET_IPv6_ADDR_AUTO_CFG_MODULE_EN

static void NetIPv6_AddrAutoCfgStop(NET_IF_NBR if_nbr,
                                    RTOS_ERR   *p_err);

static void NetIPv6_AddrAutoCfgHandler(NET_IF_NBR if_nbr,
                                       RTOS_ERR   *p_err);

static NET_IPv6_ADDR_CFG_STATUS NetIPv6_CfgAddrRand(NET_IF_NBR    if_nbr,
                                                    NET_IPv6_ADDR *p_ipv6_id,
                                                    CPU_BOOLEAN   dad_en,
                                                    NET_IPv6_ADDR **p_addr_rtn,
                                                    RTOS_ERR      *p_err);

static void NetIPv6_CfgAddrGlobal(NET_IF_NBR    if_nbr,
                                  NET_IPv6_ADDR *p_ipv6_addr,
                                  RTOS_ERR      *p_err);

#ifdef NET_DAD_MODULE_EN
static void NetIPv6_AddrAutoCfgDAD_Result(NET_IF_NBR               if_nbr,
                                          NET_DAD_OBJ              *p_dad_obj,
                                          NET_IPv6_ADDR_CFG_STATUS status);
#endif
#endif

#ifdef NET_DAD_MODULE_EN
static void NetIPv6_CfgAddrAddDAD_Result(NET_IF_NBR               if_nbr,
                                         NET_DAD_OBJ              *p_dad_obj,
                                         NET_IPv6_ADDR_CFG_STATUS status);
#endif

//                                                                 ------- SRC ADDR SELECTION FNCTS -------
static const NET_IPv6_ADDR_OBJ *NetIPv6_AddrSrcSel(NET_IF_NBR          if_nbr,
                                                   const NET_IPv6_ADDR *p_addr_dest,
                                                   RTOS_ERR            *p_err);

static const NET_IPv6_POLICY *NetIPv6_AddrSelPolicyGet(const NET_IPv6_ADDR *p_addr);

//                                                                 -------------- CFG FNCTS ---------------
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIPv6_CfgAddrValidate(NET_IPv6_ADDR *p_addr,
                                    CPU_INT08U    prefix_len,
                                    RTOS_ERR      *p_err);
#endif

//                                                                 -------- VALIDATE RX DATAGRAMS ---------
static void NetIPv6_RxPktValidate(NET_BUF      *p_buf,
                                  NET_BUF_HDR  *p_buf_hdr,
                                  NET_IPv6_HDR *p_ip_hdr,
                                  RTOS_ERR     *p_err);

static void NetIPv6_RxPktValidateNextHdr(NET_BUF           *p_buf,
                                         NET_BUF_HDR       *p_buf_hdr,
                                         NET_IPv6_NEXT_HDR next_hdr,
                                         CPU_INT16U        protocol_ix,
                                         RTOS_ERR          *p_err);

static void NetIPv6_RxPktProcessExtHdr(NET_BUF  *p_buf,
                                       RTOS_ERR *p_err);

static CPU_INT16U NetIPv6_RxOptHdr(NET_BUF           *p_buf,
                                   NET_IPv6_NEXT_HDR *p_next_hdr,
                                   NET_PROTOCOL_TYPE proto_type,
                                   RTOS_ERR          *p_err);

static CPU_INT16U NetIPv6_RxRoutingHdr(NET_BUF           *p_buf,
                                       NET_IPv6_NEXT_HDR *p_next_hdr,
                                       RTOS_ERR          *p_err);

static CPU_INT16U NetIPv6_RxFragHdr(NET_BUF           *p_buf,
                                    NET_IPv6_NEXT_HDR *p_next_hdr);

static CPU_INT16U NetIPv6_RxESP_Hdr(NET_BUF           *p_buf,
                                    NET_IPv6_NEXT_HDR *p_next_hdr,
                                    RTOS_ERR          *p_err);

static CPU_INT16U NetIPv6_RxAuthHdr(NET_BUF           *p_buf,
                                    NET_IPv6_NEXT_HDR *p_next_hdr);

#if 0
static CPU_INT16U NetIPv6_RxNoneHdr(NET_BUF           *p_buf,
                                    NET_IPv6_NEXT_HDR *p_next_hdr,
                                    RTOS_ERR          *p_err);
#endif

static CPU_INT16U NetIPv6_RxMobilityHdr(NET_BUF           *p_buf,
                                        NET_IPv6_NEXT_HDR *p_next_hdr,
                                        RTOS_ERR          *p_err);

//                                                                 ------------ REASM RX FRAGS ------------
static NET_BUF *NetIPv6_RxPktFragReasm(NET_BUF      *p_buf,
                                       NET_BUF_HDR  *p_buf_hdr,
                                       NET_IPv6_HDR *p_ip_hdr,
                                       CPU_BOOLEAN  *p_is_frag,
                                       RTOS_ERR     *p_err);

static void NetIPv6_RxPktFragListAdd(NET_BUF     *p_buf,
                                     NET_BUF_HDR *p_buf_hdr,
                                     CPU_INT16U  frag_ip_flags,
                                     CPU_INT32U  frag_offset,
                                     CPU_INT32U  frag_size,
                                     RTOS_ERR    *p_err);

static NET_BUF *NetIPv6_RxPktFragListInsert(NET_BUF     *p_buf,
                                            NET_BUF_HDR *p_buf_hdr,
                                            CPU_INT16U  frag_ip_flags,
                                            CPU_INT32U  frag_offset,
                                            CPU_INT32U  frag_size,
                                            NET_BUF     *p_frag_list,
                                            RTOS_ERR    *p_err);

static void NetIPv6_RxPktFragListRemove(NET_BUF     *p_frag_list,
                                        CPU_BOOLEAN tmr_free);

static void NetIPv6_RxPktFragListDiscard(NET_BUF     *p_frag_list,
                                         CPU_BOOLEAN tmr_free);

static void NetIPv6_RxPktFragListUpdate(NET_BUF     *p_frag_list,
                                        NET_BUF_HDR *p_frag_list_buf_hdr,
                                        CPU_INT16U  frag_ip_flags,
                                        CPU_INT32U  frag_offset,
                                        CPU_INT32U  frag_size,
                                        RTOS_ERR    *p_err);

static NET_BUF *NetIPv6_RxPktFragListChkComplete(NET_BUF     *p_frag_list,
                                                 NET_BUF_HDR *p_frag_list_buf_hdr,
                                                 RTOS_ERR    *p_err);

static void NetIPv6_RxPktFragTimeout(void *p_frag_list_timeout);

//                                                                 ---------- DEMUX RX DATAGRAMS ----------
static void NetIPv6_RxPktDemuxDatagram(NET_BUF     *p_buf,
                                       NET_BUF_HDR *p_buf_hdr,
                                       RTOS_ERR    *p_err);

static void NetIPv6_RxPktDiscard(NET_BUF *p_buf);

//                                                                 ----------- VALIDATE TX PKTS -----------
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIPv6_TxPktValidate(const NET_BUF_HDR      *p_buf_hdr,
                                  const NET_IPv6_ADDR    *p_addr_src,
                                  const NET_IPv6_ADDR    *p_addr_dest,
                                  NET_IPv6_TRAFFIC_CLASS traffic_class,
                                  NET_IPv6_FLOW_LABEL    flow_label,
                                  NET_IPv6_HOP_LIM       hop_lim,
                                  RTOS_ERR               *p_err);
#endif

//                                                                 ------------- TX IPv6 PKTS -------------
static void NetIPv6_TxPkt(NET_BUF                *p_buf,
                          NET_BUF_HDR            *p_buf_hdr,
                          NET_IPv6_ADDR          *p_addr_src,
                          NET_IPv6_ADDR          *p_addr_dest,
                          NET_IPv6_EXT_HDR       *p_ext_hdr_list,
                          NET_IPv6_TRAFFIC_CLASS traffic_class,
                          NET_IPv6_FLOW_LABEL    flow_label,
                          NET_IPv6_HOP_LIM       hop_lim,
                          RTOS_ERR               *p_err);

static void NetIPv6_TxPktPrepareHdr(NET_BUF                *p_buf,
                                    NET_BUF_HDR            *p_buf_hdr,
                                    CPU_INT16U             protocol_ix,
                                    NET_IPv6_ADDR          *p_addr_src,
                                    NET_IPv6_ADDR          *p_addr_dest,
                                    NET_IPv6_EXT_HDR       *p_ext_hdr_list,
                                    NET_IPv6_TRAFFIC_CLASS traffic_class,
                                    NET_IPv6_FLOW_LABEL    flow_label,
                                    NET_IPv6_HOP_LIM       hop_lim,
                                    RTOS_ERR               *p_err);

static void NetIPv6_TxPktPrepareExtHdr(NET_BUF          *p_buf,
                                       NET_IPv6_EXT_HDR *p_ext_hdr_list,
                                       RTOS_ERR         *p_err);

#if 0
static void NetIPv6_TxPktPrepareFragHdr(NET_BUF     *p_buf,
                                        NET_BUF_HDR *p_buf_hdr,
                                        CPU_INT16U  *p_protocol_ix,
                                        RTOS_ERR    *p_err);
#endif

//                                                                 ----------- TX IPv6 DATAGRAMS ----------
static void NetIPv6_TxPktDatagram(NET_BUF     *p_buf,
                                  NET_BUF_HDR *p_buf_hdr,
                                  RTOS_ERR    *p_err);

static NET_IP_TX_DEST_STATUS NetIPv6_TxPktDatagramRouteSel(NET_BUF     *p_buf,
                                                           NET_BUF_HDR *p_buf_hdr,
                                                           RTOS_ERR    *p_err);

//                                                                 ----------- RE-TX IPv6 PKTS ------------
static void NetIPv6_ReTxPkt(NET_BUF     *p_buf,
                            NET_BUF_HDR *p_buf_hdr,
                            RTOS_ERR    *p_err);

static void NetIPv6_ReTxPktPrepareHdr(NET_BUF     *p_buf,
                                      NET_BUF_HDR *p_buf_hdr);

#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
static void NetIPv6_TxSolicitationTimeout(void *p_obj);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetIPv6_AddrAutoCfgEn()
 *
 * @brief    Enable IPv6 Auto-configuration process. If the link state is UP, the IPv6 Auto-configuration
 *           will start, else the Auto-configuration will start when the link becomes UP.
 *
 * @param    if_nbr  Network interface number to enable the address auto-configuration on.
 *
 * @param    dad_en  DEF_YES, Do the Duplication Address Detection (DAD).
 *                   DEF_NO , otherwise
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_NOT_AVAIL
 *
 * @return   DEF_OK,   if IPv6 Auto-Configuration is enabled successfully
 *           DEF_FAIL, otherwise.
 *
 * @internal
 * @note     (1) [INTERNAL] This function is called by application function(s) :
 *               - (a) MUST NOT be called with the global network lock already acquired;
 *               - (b) MUST block ALL other network protocol tasks by pending on & acquiring the global
 *                     network lock.
 *               This is required since an application's network protocol suite API function access is
 *               asynchronous to other network protocol tasks.
 *
 * @note     (2) [INTERNAL] RFC #4862 , Section 4 states that "Autoconfiguration is performed only on
 *               multicast-capable links and begins when a multicast-capable interface is enabled,
 *               e.g., during system startup".
 * @endinternal
 *******************************************************************************************************/
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
CPU_BOOLEAN NetIPv6_AddrAutoCfgEn(NET_IF_NBR  if_nbr,
                                  CPU_BOOLEAN dad_en,
                                  RTOS_ERR    *p_err)
{
  CPU_BOOLEAN result = DEF_FAIL;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_cfgd = DEF_NO;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIPv6_AddrAutoCfgEn);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit_release;
  }
#endif

  result = NetIPv6_AddrAutoCfgEnHandler(if_nbr, dad_en, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (result);
}
#endif

/****************************************************************************************************//**
 *                                           NetIPv6_AddrAutoCfgDis()
 *
 * @brief    Disabled the IPv6 Auto-Configuration.
 *
 * @param    if_nbr  Network interface number on which to disabled address auto-configuration.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_OK,   if IPv6 Auto-Configuration was disabled successfully.
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
 * @note     (2) After disabling, in a case of a link status change, the auto-configuration will
 *               not be called. The Hook function after the Auto-configuration completion will also
 *               not occurred.
 *
 * @note     (3) The address previously configured with the auto-configuration will NOT be removed
 *               after disabling.
 *******************************************************************************************************/
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
CPU_BOOLEAN NetIPv6_AddrAutoCfgDis(NET_IF_NBR if_nbr,
                                   RTOS_ERR   *p_err)
{
  CPU_BOOLEAN           result = DEF_FAIL;
  NET_IF                *p_if = DEF_NULL;
  NET_IPv6_AUTO_CFG_OBJ *p_auto_obj = DEF_NULL;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_cfgd = DEF_NO;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIPv6_AddrAutoCfgEn);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ----------------- VALIDATE IF NBR ------------------
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

  p_auto_obj = p_if->IP_Obj->IPv6_AutoCfgObjPtr;
  if (p_auto_obj != DEF_NULL) {
    p_auto_obj->En = DEF_NO;
  }

  NetIPv6_AddrAutoCfgStop(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  result = DEF_OK;

  //                                                               ----------------- RELEASE NET LOCK -----------------
exit_release:
  Net_GlobalLockRelease();

  return (result);
}
#endif

/****************************************************************************************************//**
 *                                           NetIPv6_AddrSubscribe()
 *
 * @brief    Configure the IPv6 Address Configuration hook function. This function will be called
 *           each time a IPv6 address has finished being configured.
 *
 * @param    fnct    Pointer to hook function to call when the IPv6 static address configuration ends.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_POOL_EMPTY
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
void NetIPv6_AddrSubscribe(NET_IPv6_ADDR_HOOK_FNCT fnct,
                           RTOS_ERR                *p_err)
{
  NET_IPv6_ADDR_SUBSCRIBE_OBJ *p_subscriber;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIPv6_AddrSubscribe);

  //                                                               -------- VALIDATE SUBSCRIBER DOESN'T EXIST ---------

  SLIST_FOR_EACH_ENTRY(NetIPv6_AddrSubscriberListHeadPtr, p_subscriber, NET_IPv6_ADDR_SUBSCRIBE_OBJ, ListNode) {
    if (p_subscriber->Fnct == fnct) {
      p_subscriber->RefCtn++;
      goto exit_release;
    }
  }

  //                                                               -------- GET MEMORY TO STORE NEW SUBSCRIBER --------
  p_subscriber = (NET_IPv6_ADDR_SUBSCRIBE_OBJ *)Mem_DynPoolBlkGet(&NetIPv6_AddrSubscriberPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_subscriber->RefCtn = 0u;
  p_subscriber->Fnct = fnct;

  SList_Push(&NetIPv6_AddrSubscriberListHeadPtr, &p_subscriber->ListNode);

  //                                                               ----------------- RELEASE NET LOCK -----------------
exit_release:
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetIPv6_AddrUnsubscribe()
 *
 * @brief    Remove subscribe hook function for IPv6 address configuration.
 *
 * @param    fnct    Pointer to hook function to remove.
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
void NetIPv6_AddrUnsubscribe(NET_IPv6_ADDR_HOOK_FNCT fnct)
{
  NET_IPv6_ADDR_SUBSCRIBE_OBJ *p_subscriber;

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIPv6_AddrUnsubscribe);       // See Note #1b.

  SLIST_FOR_EACH_ENTRY(NetIPv6_AddrSubscriberListHeadPtr, p_subscriber, NET_IPv6_ADDR_SUBSCRIBE_OBJ, ListNode) {
    if (p_subscriber->Fnct == fnct) {
      if (p_subscriber->RefCtn > 0u) {
        p_subscriber->RefCtn--;
      } else {
        RTOS_ERR err;

        SList_Rem(&NetIPv6_AddrSubscriberListHeadPtr, &p_subscriber->ListNode);
        Mem_DynPoolBlkFree(&NetIPv6_AddrSubscriberPool, p_subscriber, &err);
      }
    }
  }

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetIPv6_CfgAddrAdd()
 *
 * @brief    Add a statically-configured IPv6 host address to an interface.
 *
 * @param    if_nbr      Interface number to configure.
 *
 * @param    p_addr      Pointer to desired IPv6 address to add to this interface.
 *
 * @param    prefix_len  Prefix length of the desired IPv6 address to add to this interface.
 *
 * @param    flags       Set of flags to select options for the address configuration:
 *                           - NET_IPv6_FLAG_BLOCK_EN      Enables blocking mode if set.
 *                           - NET_IPv6_FLAG_DAD_EN        Enables DAD if set.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_NOT_AVAIL
 *
 * @return   DEF_OK,   if valid IPv6 address configured.
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
CPU_BOOLEAN NetIPv6_CfgAddrAdd(NET_IF_NBR    if_nbr,
                               NET_IPv6_ADDR *p_addr,
                               CPU_INT08U    prefix_len,
                               NET_FLAGS     flags,
                               RTOS_ERR      *p_err)
{
  NET_IF                 *p_if;
  NET_IPv6_ADDR_CFG_TYPE addr_cfg_type;
  CPU_BOOLEAN            block_en;
  CPU_BOOLEAN            dad_en;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_cfgd;
#endif
  CPU_BOOLEAN              result = DEF_FAIL;
  NET_IPv6_ADDR_CFG_STATUS cfg_status;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetIPv6_CfgAddrAdd);            // See Note #3b.

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit_release;
  }

  //                                                               --------------- VALIDATE IPv6 ADDR -----------------
  NetIPv6_CfgAddrValidate(p_addr, prefix_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }
#endif

  block_en = DEF_BIT_IS_SET(flags, NET_IPv6_FLAG_BLOCK_EN);
  dad_en = DEF_BIT_IS_SET(flags, NET_IPv6_FLAG_DAD_EN);

  //                                                               --------------- SET DAD PARAMETERS -----------------
  if (block_en == DEF_YES) {
    addr_cfg_type = NET_IPv6_ADDR_CFG_TYPE_STATIC_BLOCKING;
  } else {
    addr_cfg_type = NET_IPv6_ADDR_CFG_TYPE_STATIC_NO_BLOCKING;
  }

  //                                                               --------------------- CFG ADDR ---------------------
  cfg_status = NetIPv6_CfgAddrAddHandler(if_nbr,
                                         p_addr,
                                         prefix_len,
                                         0,
                                         0,
                                         NET_IP_ADDR_CFG_MODE_STATIC,
                                         dad_en,
                                         addr_cfg_type,
                                         DEF_NULL,
                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  switch (cfg_status) {
    case NET_IPv6_ADDR_CFG_STATUS_SUCCEED:
    case NET_IPv6_ADDR_CFG_STATUS_IN_PROGRESS:
      result = DEF_OK;
      break;

    case NET_IPv6_ADDR_CFG_STATUS_FAIL:
    case NET_IPv6_ADDR_CFG_STATUS_DUPLICATE:
      result = DEF_FAIL;
      break;

    default:
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_FAIL);
      goto exit_release;
  }

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_if->StartModulesCfgFlags.IPv6_Static = DEF_YES;

exit_release:
  Net_GlobalLockRelease();

  return (result);
}

/****************************************************************************************************//**
 *                                           NetIPv6_CfgAddrRemove()
 *
 * @brief    Remove a configured IPv6 host address & multicast solicited mode address from an
 *           interface.
 *
 * @param    if_nbr          Interface number to remove address configuration.
 *
 * @param    p_addr_host     Pointer to IPv6 address to remove.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_OK,   if IPv6 address removed.
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
CPU_BOOLEAN NetIPv6_CfgAddrRemove(NET_IF_NBR    if_nbr,
                                  NET_IPv6_ADDR *p_addr_host,
                                  RTOS_ERR      *p_err)
{
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_valid;
#endif
  CPU_BOOLEAN result = DEF_FAIL;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               ------------- ACQUIRE NET LOCK -------------
  Net_GlobalLockAcquire((void *)NetIPv6_CfgAddrRemove);                 // See Note #2b.

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ------------- VALIDATE IF NBR --------------
  is_valid = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_valid != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit_release;
  }

  //                                                               ------------ VALIDATE IPv6 ADDR ------------
  is_valid = NetIPv6_IsValidAddrHost(p_addr_host);
  if (is_valid != DEF_YES) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.CfgInvAddrHostCtr);
    Net_GlobalLockRelease();
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG, DEF_FAIL);
  }
#endif

  //                                                               Remove specific IF's cfg'd host addr.
  result = NetIPv6_CfgAddrRemoveHandler(if_nbr, p_addr_host, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit_release:
  //                                                               ------------- RELEASE NET LOCK -------------
  Net_GlobalLockRelease();

  return (result);
}

/****************************************************************************************************//**
 *                                       NetIPv6_CfgAddrRemoveAll()
 *
 * @brief    Remove all configured IPv6 host address(s) from an interface.
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
CPU_BOOLEAN NetIPv6_CfgAddrRemoveAll(NET_IF_NBR if_nbr,
                                     RTOS_ERR   *p_err)
{
  CPU_BOOLEAN addr_remove = DEF_FAIL;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_cfgd;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Net_GlobalLockAcquire((void *)NetIPv6_CfgAddrRemoveAll);      // Acquire net lock (see Note #1b).

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);                   // Validate IF nbr.
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit_release;
  }
#endif

  //                                                               Remove all IF's cfg'd host addr(s).
  addr_remove = NetIPv6_CfgAddrRemoveAllHandler(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit_release:
  Net_GlobalLockRelease();                                      // Release net lock.

  return (addr_remove);
}

/****************************************************************************************************//**
 *                                       NetIPv6_CfgFragReasmTimeout()
 *
 * @brief    Configure IPv6 fragment reassembly timeout.
 *
 * @param    timeout_sec     Desired value for IPv6 fragment reassembly timeout (in seconds).
 *
 * @return   DEF_OK,   IPv6 fragment reassembly timeout configured.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) IPv6 fragment reassembly timeout is the maximum time allowed between received IPv6
 *               fragments from the same IPv6 datagram.
 *
 * @internal
 * @note     (2) [INTERNAL] 'NetIPv6_FragReasmTimeout_ms' variables MUST ALWAYS be accessed exclusively
 *               in critical sections.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_CfgFragReasmTimeout(CPU_INT08U timeout_sec)
{
  CORE_DECLARE_IRQ_STATE;

  if (timeout_sec < NET_IPv6_FRAG_REASM_TIMEOUT_MIN_SEC) {
    return (DEF_FAIL);
  }
  if (timeout_sec > NET_IPv6_FRAG_REASM_TIMEOUT_MAX_SEC) {
    return (DEF_FAIL);
  }

  CORE_ENTER_ATOMIC();
  NetIPv6_FragReasmTimeout_ms = timeout_sec * 1000;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           NetIPv6_GetAddrHost()
 *
 * @brief    Get an interface's IPv6 host address(s) [see Note #3].
 *
 * @param    if_nbr          Interface number to get IPv6 host address(s).
 *
 * @param    p_addr_tbl      Pointer to IPv6 address table that will receive the IPv6 host address(s)
 *                           in host-order for this interface.
 *
 * @param    p_addr_tbl_qty  Pointer to a variable to ... :
 *                               - (a) Pass the size of the address table, in number of IPv6 addresses,
 *                                     pointed to by 'p_addr_tbl'.
 *                               - (b) Return the actual number of IPv6 addresses, if NO error(s);
 *                               - (c) Return 0, otherwise.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_OK,   if interface's IPv6 host address(s) successfully returned.
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
CPU_BOOLEAN NetIPv6_GetAddrHost(NET_IF_NBR       if_nbr,
                                NET_IPv6_ADDR    *p_addr_tbl,
                                NET_IP_ADDRS_QTY *p_addr_tbl_qty,
                                RTOS_ERR         *p_err)
{
  CPU_BOOLEAN addr_avail = DEF_FAIL;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_FAIL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Net_GlobalLockAcquire((void *)NetIPv6_GetAddrHost);           // Acquire net lock (see Note #1b).

  //                                                               Get all IF's host addr(s).
  addr_avail = NetIPv6_GetAddrHostHandler(if_nbr, p_addr_tbl, p_addr_tbl_qty, p_err);

  Net_GlobalLockRelease();                                      // Release net lock.

  return (addr_avail);
}

/****************************************************************************************************//**
 *                                           NetIPv6_GetAddrSrc()
 *
 * @brief    Find the best matched source address in the IPv6 configured host addresses for the
 *           specified destination address.
 *
 * @param    p_if_nbr        Pointer to given interface number if any.
 *                           Variable that will received the interface number if none is given.
 *
 * @param    p_addr_src      Pointer to IPv6 suggested source address if any.
 *                           DEF_NULL if no source address is preferred.
 *
 * @param    p_addr_dest     Pointer to the destination address.
 *
 * @param    p_addr_nexthop  Pointer to Next Hop IPv6 address that the function will found.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_NET_NEXT_HOP
 *
 * @return   Pointer to the IPv6 addresses structure associated with the best source address for the given
 *           destination.
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
const NET_IPv6_ADDR_OBJ *NetIPv6_GetAddrSrc(NET_IF_NBR          *p_if_nbr,
                                            const NET_IPv6_ADDR *p_addr_src,
                                            const NET_IPv6_ADDR *p_addr_dest,
                                            NET_IPv6_ADDR       *p_addr_nexthop,
                                            RTOS_ERR            *p_err)
{
  const NET_IPv6_ADDR_OBJ *p_addr_obj = DEF_NULL;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_if_nbr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_dest != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  Net_GlobalLockAcquire((void *)NetIPv6_GetAddrSrc);

  //                                                               Get source address.
  p_addr_obj = NetIPv6_GetAddrSrcHandler(p_if_nbr,
                                         p_addr_src,
                                         p_addr_dest,
                                         p_addr_nexthop,
                                         p_err);

  Net_GlobalLockRelease();

  return (p_addr_obj);
}

/****************************************************************************************************//**
 *                                       NetIPv6_GetAddrMatchingLen()
 *
 * @brief    Compute the number of identical most significant bits of two IPv6 addresses.
 *
 * @param    p_addr_1    First  IPv6 address for comparison.
 *
 * @param    p_addr_2    Second IPv6 address for comparison.
 *
 * @return   Number of matching bits, if any.
 *           0, otherwise.
 *
 * @note     (1) The returned number is based on the number of matching MSB of both IPv6 addresses:
 *               - (a) Calling the function with the following addresses will return  32 matching bits:
 *                   - p_addr_1 : FE80:ABCD:0000:0000:0000:0000:0000:0000
 *                   - p_addr_2 : FE80:ABCD:F000:0000:0000:0000:0000:0000
 *               - (b) Calling the function with the following addresses will return 127 matching bits:
 *                   - p_addr_1 : FE80:ABCD:0000:0000:0000:0000:0000:0000
 *                   - p_addr_2 : FE80:ABCD:0000:0000:0000:0000:0000:0001
 *               - (c) Calling the function with the following addresses will return 0   matching bits:
 *                   - p_addr_1 : FE80:ABCD:0000:0000:0000:0000:0000:0000
 *                   - p_addr_2 : 7E80:ABCD:0000:0000:0000:0000:0000:0000
 *               - (d) Calling the function with identical addresses will return 128 matching bits.
 *******************************************************************************************************/
CPU_INT08U NetIPv6_GetAddrMatchingLen(const NET_IPv6_ADDR *p_addr_1,
                                      const NET_IPv6_ADDR *p_addr_2,
                                      RTOS_ERR            *p_err)
{
  CPU_INT08U  matching_bit_qty = 0u;
  CPU_INT08U  bit_ix;
  CPU_INT08U  octet_ix;
  CPU_INT08U  octet_diff;
  CPU_BOOLEAN octet_match;
  CPU_BOOLEAN bit_match;
  CPU_BOOLEAN bit_clr;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_1 != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_2 != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  matching_bit_qty = 0u;
  octet_ix = 0u;
  octet_diff = 0u;
  bit_ix = 0u;
  octet_match = DEF_YES;
  bit_match = DEF_YES;
  bit_clr = DEF_YES;

  while ((octet_ix < NET_IPv6_ADDR_SIZE)
         && (octet_match == DEF_YES)) {
    if (p_addr_1->Addr[octet_ix] == p_addr_2->Addr[octet_ix]) {             // If octets are     identical ...
      matching_bit_qty = matching_bit_qty + DEF_OCTET_NBR_BITS;             // ... add 8 bits to matching bit ctr.
    } else {                                                                // If octets are NOT identical ...
      octet_diff = p_addr_1->Addr[octet_ix] ^ p_addr_2->Addr[octet_ix];      // ... determine which bits are identical.
      while ((bit_ix < DEF_OCTET_NBR_BITS)                                  // Calculate number of identical bits ...
             && (bit_match == DEF_YES)) {                                   // ... starting from the MSB of the octet.
        bit_clr = DEF_BIT_IS_CLR(octet_diff, DEF_BIT_07);
        if (bit_clr == DEF_YES) {                                           // If bits are identical .
          matching_bit_qty++;                                               // ... inc maching bits ctr.
          octet_diff <<= 1u;
        } else {
          bit_match = DEF_NO;                                               // Return as soon as a bit is different.
        }
      }
      octet_match = DEF_NO;
    }
    octet_ix++;
  }

  return (matching_bit_qty);
}

/****************************************************************************************************//**
 *                                           NetIPv6_GetAddrScope()
 *
 * @brief    Get the scope of the given IPv6 address.
 *
 * @param    p_addr  Pointer to IPv6 address.
 *
 * @return   Scope of the given IPv6 address.
 *
 * @note     (1) For an unicast address, the scope is given by the 16 first bits of the address. Three
 *               scopes are possible:
 *               - (a) 0xFE80  Link-local
 *               - (b) 0xFEC0  Site-local  -> Deprecated
 *               - (c) others  Global
 *
 * @note     (2) For a mulicast address, the scope is given by a four bits field inside the address.
 *               Current possible scopes are:
 *               - (a) 0x0  reserved
 *               - (b) 0x1  interface-local
 *               - (c) 0x2  link-local
 *               - (d) 0x3  reserved
 *               - (e) 0x4  admin-local
 *               - (f) 0x5  site-local
 *               - (g) 0x6  unassigned
 *               - (h) 0x7  unassigned
 *               - (i) 0x8  organization-local
 *               - (j) 0x9  unassigned
 *               - (k) 0xA  unassigned
 *               - (l) 0xB  unassigned
 *               - (m) 0xC  unassigned
 *               - (n) 0xD  unassigned
 *               - (o) 0xE  global
 *               - (p) 0xF  reserved
 *******************************************************************************************************/
NET_IPv6_SCOPE NetIPv6_GetAddrScope(const NET_IPv6_ADDR *p_addr)
{
  CPU_INT08U     scope;
  CPU_BOOLEAN    is_loopback;
  CPU_BOOLEAN    is_unspecified;
  NET_IPv6_SCOPE scope_rtn = NET_IPv6_ADDR_SCOPE_RESERVED;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (p_addr == DEF_NULL) {
    goto exit;
  }
#endif
  //                                                               ------------------ UNICAST ADDRESS -----------------
  if (p_addr->Addr[0] == 0xFE) {
    scope = p_addr->Addr[1] & 0xC0;

    switch (scope) {
      case 0x80:
        scope_rtn = NET_IPv6_ADDR_SCOPE_LINK_LOCAL;
        goto exit;

      case 0xC0:
        scope_rtn = NET_IPv6_ADDR_SCOPE_SITE_LOCAL;
        goto exit;

      default:
        scope_rtn = NET_IPv6_ADDR_SCOPE_GLOBAL;
        goto exit;
    }
  }

  //                                                               ----------------- MULITCAST ADDRESS ----------------
  if (p_addr->Addr[0] == 0xFF) {
    scope = p_addr->Addr[1] & 0x0F;
    switch (scope) {
      case NET_IPv6_ADDR_SCOPE_RESERVED:
      case NET_IPv6_ADDR_SCOPE_IF_LOCAL:
      case NET_IPv6_ADDR_SCOPE_LINK_LOCAL:
      case NET_IPv6_ADDR_SCOPE_ADMIN_LOCAL:
      case NET_IPv6_ADDR_SCOPE_SITE_LOCAL:
      case NET_IPv6_ADDR_SCOPE_ORG_LOCAL:
      case NET_IPv6_ADDR_SCOPE_GLOBAL:
        scope_rtn = (NET_IPv6_SCOPE)scope;
        goto exit;

      default:
        scope_rtn = NET_IPv6_ADDR_SCOPE_GLOBAL;
        goto exit;
    }
  }

  //                                                               ----------------- LOOPBACK ADDRESS -----------------
  is_loopback = NetIPv6_IsAddrLoopback(p_addr);
  if (is_loopback == DEF_YES) {
    scope_rtn = NET_IPv6_ADDR_SCOPE_LINK_LOCAL;
    goto exit;
  }

  //                                                               ---------------- UNSPECIFIED ADDRESS ---------------
  is_unspecified = NetIPv6_IsAddrUnspecified(p_addr);
  if (is_unspecified == DEF_YES) {
    scope_rtn = NET_IPv6_ADDR_SCOPE_GLOBAL;
    goto exit;
  }

  scope_rtn = NET_IPv6_ADDR_SCOPE_GLOBAL;

exit:
  return (scope_rtn);
}

/****************************************************************************************************//**
 *                                           NetIPv6_IsAddrHostCfgd()
 *
 * @brief    Validate an IPv6 address as a configured IPv6 host address on an enabled interface.
 *
 * @param    p_addr  Pointer to IPv6 address to validate.
 *
 * @return   DEF_YES, if IPv6 address is one of the host's configured IPv6 addresses.
 *           DEF_NO,  otherwise.
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
CPU_BOOLEAN NetIPv6_IsAddrHostCfgd(const NET_IPv6_ADDR *p_addr)
{
  CPU_BOOLEAN addr_host = DEF_FAIL;

  RTOS_ASSERT_DBG((p_addr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  //                                                               Acquire net lock (see Note #1b).
  Net_GlobalLockAcquire((void *)NetIPv6_IsAddrHostCfgd);

  addr_host = NetIPv6_IsAddrHostCfgdHandler(p_addr);            // Chk if any cfg'd host addr.

  Net_GlobalLockRelease();                                      // Release net lock.

  return (addr_host);
}

/****************************************************************************************************//**
 *                                           NetIPv6_IsAddrsCfgdOnIF()
 *
 * @brief    Check if any IPv6 host address(s) configured on an interface.
 *
 * @param    if_nbr  Interface number to check for configured IPv6 host address(s).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_HANDLE
 *
 * @return   DEF_YES, if any IP host address(s) configured on interface.
 *           DEF_NO,  otherwise.
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
CPU_BOOLEAN NetIPv6_IsAddrsCfgdOnIF(NET_IF_NBR if_nbr,
                                    RTOS_ERR   *p_err)
{
  CPU_BOOLEAN addr_avail = DEF_NO;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_cfgd = DEF_NO;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NO);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               Acquire net lock (see Note #1b).
  Net_GlobalLockAcquire((void *)NetIPv6_IsAddrsCfgdOnIF);

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_cfgd != DEF_YES) {
    Net_GlobalLockRelease();
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return (DEF_NO);
  }
#endif

  addr_avail = NetIPv6_IsAddrsCfgdOnIF_Handler(if_nbr, p_err);  // Chk IF for any cfg'd host addr(s).

  Net_GlobalLockRelease();                                      // Release net lock.

  return (addr_avail);
}

/****************************************************************************************************//**
 *                                           NetIPv6_IsValidAddrHost()
 *
 * @brief    (1) Validate an IPv6 host address :
 *              - (a) MUST NOT be one of the following :
 *                  - (1) The unspecified address
 *                  - (2) The loopback    address
 *                  - (3) A Multicast     address
 *
 * @param    p_addr_host     Pointer to IPv6 host address to validate.
 *
 * @return   DEF_YES, if IPv6 host address valid.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsValidAddrHost(const NET_IPv6_ADDR *p_addr_host)
{
  CPU_BOOLEAN is_unspecified;
  CPU_BOOLEAN is_loopback;
  CPU_BOOLEAN is_mcast;

  RTOS_ASSERT_DBG((p_addr_host != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NO);

  is_unspecified = NetIPv6_IsAddrUnspecified(p_addr_host);
  if (is_unspecified == DEF_YES) {
    return (DEF_NO);
  }

  is_loopback = NetIPv6_IsAddrLoopback(p_addr_host);
  if (is_loopback == DEF_YES) {
    return (DEF_NO);
  }

  is_mcast = NetIPv6_IsAddrMcast(p_addr_host);
  if (is_mcast == DEF_YES) {
    return (DEF_NO);
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                           NetIPv6_IsAddrLinkLocal()
 *
 * @brief    (1) Validate an IPv6 address as a link-local IPv6 address.
 *               - (a) In text representation, it corresponds to the following format:
 *                       @verbatim
 *                       FE80:0000:0000:0000:aaaa.bbbb.cccc.dddd  OR:
 *                       FE80::aaaa.bbbb.cccc.dddd                WHERE:
 *
 *                           aaaa.bbbb.cccc.dddd is the interface ID.
 *                       @endverbatim
 * @param    p_addr  Pointer to IPv6 address to validate
 *
 * @return   DEF_YES, if IPv6 address is a link-local IPv6 address.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrLinkLocal(const NET_IPv6_ADDR *p_addr)
{
  CPU_BOOLEAN addr_link_local = DEF_NO;

  //                                                               ---------------- VALIDATE ADDR PTR -----------------
  RTOS_ASSERT_DBG((p_addr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NO);

  //                                                               ------------------ VALIDATE ADDR -------------------
  if ( (p_addr->Addr[0] == 0xFE)
       && ((p_addr->Addr[1] & 0xC0) == 0x80)
       && (p_addr->Addr[2] == DEF_BIT_NONE)
       && (p_addr->Addr[3] == DEF_BIT_NONE)
       && (p_addr->Addr[4] == DEF_BIT_NONE)
       && (p_addr->Addr[5] == DEF_BIT_NONE)
       && (p_addr->Addr[6] == DEF_BIT_NONE)
       && (p_addr->Addr[7] == DEF_BIT_NONE)) {
    addr_link_local = DEF_YES;
  }

  return (addr_link_local);
}

/****************************************************************************************************//**
 *                                           NetIPv6_IsAddrSiteLocal()
 *
 * @brief    (1) Validate an IPv6 address as a site-local address :
 *               - (a) In text representation, it corresponds to the following format:
 *                       @verbatim
 *                       FEC0:AAAA:BBBB:CCCC:DDDD:aaaa.bbbb.cccc.dddd  WHERE:
 *
 *                           AAAA.BBBB.CCCC.DDDD is the subnet    ID AND ...
 *                           aaaa.bbbb.cccc.dddd is the interface ID.
 *                       @endverbatim
 * @param    p_addr  Pointer to IPv6 address to validate.
 *
 * @return   DEF_YES, if IPv6 address is a site-local IPv6 address.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrSiteLocal(const NET_IPv6_ADDR *p_addr)
{
  CPU_BOOLEAN addr_site_local = DEF_NO;

  //                                                               ---------------- VALIDATE ADDR PTR -----------------
  RTOS_ASSERT_DBG((p_addr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NO);

  //                                                               ------------------ VALIDATE ADDR -------------------
  if ((p_addr->Addr[0] == 0xFE)
      && (p_addr->Addr[1] == 0xC0)) {
    addr_site_local = DEF_YES;
  }

  return (addr_site_local);
}

/****************************************************************************************************//**
 *                                           NetIPv6_IsAddrMcast()
 *
 * @brief    Validate an IPv6 address as a multicast address.
 *
 * @param    p_addr  Pointer to IPv6 address to validate.
 *
 * @return   DEF_YES, if IPv6 address is a multicast IPv6 address.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) Refer to RFC #4291, Section 2.6 for the Multicast IPv6 address format.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrMcast(const NET_IPv6_ADDR *p_addr)
{
  CPU_BOOLEAN addr_mcast = DEF_NO;

  //                                                               ---------------- VALIDATE ADDR PTR -----------------
  RTOS_ASSERT_DBG((p_addr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NO);

  //                                                               ------------------ VALIDATE ADDR -------------------
  if (p_addr->Addr[0] == 0xFF) {
    addr_mcast = DEF_YES;
  }

  return (addr_mcast);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsAddrMcastSolNode()
 *
 * @brief    Validate that an IPv6 address is the solicited node multicast address associated with an
 *           IPv6 unicast address.
 *
 * @param    p_addr          Pointer to IPv6 address to validate.
 *
 * @param    p_addr_input    Pointer to input IPv6 unicast address.
 *
 * @return   DEF_YES, if IPv6 address is a solicited node multicast IPv6 address.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) RFC #4291 Section 2.7.1 specifies that "Solicited-Node multicast address are computed
 *               as a function of a node's unicast and anycast addresses.  A Solicited-Node multicast
 *               address is formed by taking the low-order 24 bits of an address (unicast or anycast)
 *               and appending those bits to the prefix FF02:0:0:0:0:1:FF00::/104 resulting in a
 *               multicast address in the range" from :
 *               - (a) FF02:0000:0000:0000:0000:0001:FF00:0000
 *                       to ...
 *               - (b) FF02:0000:0000:0000:0000:0001:FFFF:FFFF"
 *
 * @internal
 * @note     (2) [INTERNAL] As the 24 low-order 24 bits of the input address MUST be compared, the length
 *               argument of the Mem_Cmp() function call is 3 bytes.
 * @endinternal
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrMcastSolNode(const NET_IPv6_ADDR *p_addr,
                                       const NET_IPv6_ADDR *p_addr_input)
{
  CPU_BOOLEAN addr_mcast_sol_node = DEF_NO;

  //                                                               --------------- VALIDATE ADDR PTRS -----------------
  RTOS_ASSERT_DBG((p_addr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NO);
  RTOS_ASSERT_DBG((p_addr_input != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NO);

  //                                                               ------------------ VALIDATE ADDR -------------------
  if ((p_addr->Addr[0] != 0xFF)                                 // See Note #1.
      || (p_addr->Addr[1] != 0x02)
      || (p_addr->Addr[2] != DEF_BIT_NONE)
      || (p_addr->Addr[3] != DEF_BIT_NONE)
      || (p_addr->Addr[4] != DEF_BIT_NONE)
      || (p_addr->Addr[5] != DEF_BIT_NONE)
      || (p_addr->Addr[6] != DEF_BIT_NONE)
      || (p_addr->Addr[7] != DEF_BIT_NONE)
      || (p_addr->Addr[8] != DEF_BIT_NONE)
      || (p_addr->Addr[9] != DEF_BIT_NONE)
      || (p_addr->Addr[10] != DEF_BIT_NONE)
      || (p_addr->Addr[11] != 0x01)
      || (p_addr->Addr[12] != 0xFF)) {
    return (DEF_NO);
  }

  addr_mcast_sol_node = Mem_Cmp(&p_addr->Addr[13],
                                &p_addr_input->Addr[13],
                                3u);                            // See Note #2.

  return (addr_mcast_sol_node);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsAddrMcastAllNodes()
 *
 * @brief    (1) Validate that an IPv6 address is a multicast to all routers IPv6 nodes :
 *               - (a) RFC #4291 Section 2.7.1 specifies that the following IPv6 addresses "identify
 *                       the group of all IPv6 nodes" within their respective scope :
 *                       @verbatim
 *                       FF01:0000:0000:0000:0000:0000:0000:1    Scope 1 -> Interface-local
 *                       FF02:0000:0000:0000:0000:0000:0000:1    Scope 2 -> Link-local
 *                       @endverbatim
 * @param    p_addr  Pointer to IPv6 address to validate (see Note #2).
 *
 *
 * Argument(s) : p_addr      Pointer to IPv6 address to validate (see Note #2).
 *
 * Return(s)   : DEF_YES, if IPv6 address is a mutlicast to all nodes IPv6 address.
 *               DEF_NO,  otherwise.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrMcastAllNodes(const NET_IPv6_ADDR *p_addr)
{
  CPU_BOOLEAN addr_mcast_all_nodes = DEF_NO;

  //                                                               ---------------- VALIDATE ADDR PTR -----------------
  RTOS_ASSERT_DBG((p_addr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NO);

  //                                                               ------------------ VALIDATE ADDR -------------------
  if ((p_addr->Addr[0] != 0xFF)
      || (p_addr->Addr[2] != DEF_BIT_NONE)
      || (p_addr->Addr[3] != DEF_BIT_NONE)
      || (p_addr->Addr[4] != DEF_BIT_NONE)
      || (p_addr->Addr[5] != DEF_BIT_NONE)
      || (p_addr->Addr[6] != DEF_BIT_NONE)
      || (p_addr->Addr[7] != DEF_BIT_NONE)
      || (p_addr->Addr[8] != DEF_BIT_NONE)
      || (p_addr->Addr[9] != DEF_BIT_NONE)
      || (p_addr->Addr[10] != DEF_BIT_NONE)
      || (p_addr->Addr[11] != DEF_BIT_NONE)
      || (p_addr->Addr[12] != DEF_BIT_NONE)
      || (p_addr->Addr[13] != DEF_BIT_NONE)
      || (p_addr->Addr[14] != DEF_BIT_NONE)
      || (p_addr->Addr[15] != 0x01)) {
    return (DEF_NO);
  }

  if ((p_addr->Addr[1] == 0x01)
      || (p_addr->Addr[1] == 0x02)) {
    addr_mcast_all_nodes = DEF_YES;
  }

  return (addr_mcast_all_nodes);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsAddrMcastAllRouters()
 *
 * @brief    (1) Validate that an IPv6 address is a multicast to all routers IPv6 address :
 *               - (a) RFC #4291 Section 2.7.1 specifies that the following IPv6 addresses "identify
 *                       the group of all IPv6 routers" within their respective scope :
 *                   @verbatim
 *                   - (a) FF01:0000:0000:0000:0000:0000:0000:2    Scope 1 -> Interface-local
 *                   - (b) FF02:0000:0000:0000:0000:0000:0000:2    Scope 2 -> Link-local
 *                   - (c) FF05:0000:0000:0000:0000:0000:0000:2    Scope 5 -> Site-local
 *                   @endverbatim
 * @param    p_addr  Pointer to IPv6 address to validate (see Note #2).
 *
 * @return   DEF_YES, if IPv6 address is a mutlicast to all routers IPv6 address.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrMcastAllRouters(const NET_IPv6_ADDR *p_addr)
{
  CPU_BOOLEAN addr_mcast_all_routers = DEF_NO;

  //                                                               ---------------- VALIDATE ADDR PTR -----------------
  RTOS_ASSERT_DBG((p_addr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NO);

  //                                                               ------------------ VALIDATE ADDR -------------------
  if ((p_addr->Addr[0] != 0xFF)
      || (p_addr->Addr[2] != DEF_BIT_NONE)
      || (p_addr->Addr[3] != DEF_BIT_NONE)
      || (p_addr->Addr[4] != DEF_BIT_NONE)
      || (p_addr->Addr[5] != DEF_BIT_NONE)
      || (p_addr->Addr[6] != DEF_BIT_NONE)
      || (p_addr->Addr[7] != DEF_BIT_NONE)
      || (p_addr->Addr[8] != DEF_BIT_NONE)
      || (p_addr->Addr[9] != DEF_BIT_NONE)
      || (p_addr->Addr[10] != DEF_BIT_NONE)
      || (p_addr->Addr[11] != DEF_BIT_NONE)
      || (p_addr->Addr[12] != DEF_BIT_NONE)
      || (p_addr->Addr[13] != DEF_BIT_NONE)
      || (p_addr->Addr[14] != DEF_BIT_NONE)
      || (p_addr->Addr[15] != 0x02)) {
    return (DEF_NO);
  }

  if ((p_addr->Addr[1] == 0x01)
      || (p_addr->Addr[1] == 0x02)
      || (p_addr->Addr[1] == 0x05)) {
    addr_mcast_all_routers = DEF_YES;
  }

  return (addr_mcast_all_routers);
}

/****************************************************************************************************//**
 *                                           NetIPv6_IsAddrMcastRsvd()
 *
 * @brief    (1) Validate that an IPv6 address is a reserved multicast IPv6 address :
 *               - (a) RFC #4291 Section 2.7.1 specifies that the following addresses "are reserved and
 *                     shall never be assigned to any multicast group" :
 *                           FF00:0:0:0:0:0:0:0
 *                           FF01:0:0:0:0:0:0:0
 *                           FF02:0:0:0:0:0:0:0
 *                           FF03:0:0:0:0:0:0:0
 *                           FF04:0:0:0:0:0:0:0
 *                           FF05:0:0:0:0:0:0:0
 *                           FF06:0:0:0:0:0:0:0
 *                           FF07:0:0:0:0:0:0:0
 *                           FF08:0:0:0:0:0:0:0
 *                           FF09:0:0:0:0:0:0:0
 *                           FF0A:0:0:0:0:0:0:0
 *                           FF0B:0:0:0:0:0:0:0
 *                           FF0C:0:0:0:0:0:0:0
 *                           FF0D:0:0:0:0:0:0:0
 *                           FF0E:0:0:0:0:0:0:0
 *                           FF0F:0:0:0:0:0:0:0
 *
 * @param    p_addr  Pointer to IPv6 address to validate (see Note #2).
 *
 * @return   DEF_YES, if IPv6 address is a reserved multicast IPv6 addresss.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrMcastRsvd(const NET_IPv6_ADDR *p_addr)
{
  CPU_BOOLEAN addr_mcast_rsvd = DEF_NO;

  //                                                               ---------------- VALIDATE ADDR PTR -----------------
  RTOS_ASSERT_DBG((p_addr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NO);

  //                                                               ------------------ VALIDATE ADDR -------------------
  if ((p_addr->Addr[0] != 0xFF)
      || (p_addr->Addr[1] != DEF_BIT_NONE)
      || (p_addr->Addr[2] != DEF_BIT_NONE)
      || (p_addr->Addr[3] != DEF_BIT_NONE)
      || (p_addr->Addr[4] != DEF_BIT_NONE)
      || (p_addr->Addr[5] != DEF_BIT_NONE)
      || (p_addr->Addr[6] != DEF_BIT_NONE)
      || (p_addr->Addr[7] != DEF_BIT_NONE)
      || (p_addr->Addr[8] != DEF_BIT_NONE)
      || (p_addr->Addr[9] != DEF_BIT_NONE)
      || (p_addr->Addr[10] != DEF_BIT_NONE)
      || (p_addr->Addr[11] != DEF_BIT_NONE)
      || (p_addr->Addr[12] != DEF_BIT_NONE)
      || (p_addr->Addr[13] != DEF_BIT_NONE)
      || (p_addr->Addr[14] != DEF_BIT_NONE)
      || (p_addr->Addr[15] != DEF_BIT_NONE)) {
    return (DEF_NO);
  }

  if (p_addr->Addr[1] < NET_IPv6_MCAST_RSVD_ADDR_MAX_VAL) {
    addr_mcast_rsvd = DEF_YES;
  }

  return (addr_mcast_rsvd);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsAddrUnspecified()
 *
 * @brief    (1) Validate that an IPv6 address is the unspecified IPv6 address :
 *               - (a) RFC #4291 Section 2.5.2 specifies that the following unicast address "is called
 *                       the unspecified address" :
 *                           0000:0000:0000:0000:0000:0000:0000:0000
 *
 * @param    p_addr  Pointer to IPv6 address to validate.
 *
 * @return   DEF_YES, if IPv6 address is the unspecified address.
 *           DEF_NO,  otherwise.
 *
 * @note     (2) This address indicates the absence of an address and MUST NOT be assigned to any
 *               physical interface. However, it can be used in some cases in the Source Address field
 *               of IPv6 packets sent by a host before it has learned its own address.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrUnspecified(const NET_IPv6_ADDR *p_addr)
{
  CPU_BOOLEAN addr_unspecified = DEF_NO;

  //                                                               ---------------- VALIDATE ADDR PTR -----------------
  RTOS_ASSERT_DBG((p_addr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NO);

  //                                                               ------------------ VALIDATE ADDR -------------------
  if ((p_addr->Addr[0] == DEF_BIT_NONE)
      && (p_addr->Addr[1] == DEF_BIT_NONE)
      && (p_addr->Addr[2] == DEF_BIT_NONE)
      && (p_addr->Addr[3] == DEF_BIT_NONE)
      && (p_addr->Addr[4] == DEF_BIT_NONE)
      && (p_addr->Addr[5] == DEF_BIT_NONE)
      && (p_addr->Addr[6] == DEF_BIT_NONE)
      && (p_addr->Addr[7] == DEF_BIT_NONE)
      && (p_addr->Addr[8] == DEF_BIT_NONE)
      && (p_addr->Addr[9] == DEF_BIT_NONE)
      && (p_addr->Addr[10] == DEF_BIT_NONE)
      && (p_addr->Addr[11] == DEF_BIT_NONE)
      && (p_addr->Addr[12] == DEF_BIT_NONE)
      && (p_addr->Addr[13] == DEF_BIT_NONE)
      && (p_addr->Addr[14] == DEF_BIT_NONE)
      && (p_addr->Addr[15] == DEF_BIT_NONE)) {
    addr_unspecified = DEF_YES;
  }

  return (addr_unspecified);
}

/****************************************************************************************************//**
 *                                           NetIPv6_IsAddrLoopback()
 *
 * @brief    (1) Validate that an IPv6 address is the IPv6 loopback address :
 *               - (a) RFC #4291 Section 2.5.3 specifies that the following unicast address "is called
 *                       the loopback address" :
 *                           0000:0000:0000:0000:0000:0000:0000:0001
 *
 * @param    p_addr  Pointer to IPv6 address to validate (see Note #2).
 *
 * @return   DEF_YES, if IPv6 address is the IPv6 loopback address.
 *           DEF_NO,  otherwise.
 *
 * @note     (2) This address may be used by a node to send an IPv6 packet to itself and MUST NOT
 *               be assigned to any physical interface.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrLoopback(const NET_IPv6_ADDR *p_addr)
{
  CPU_BOOLEAN addr_loopback = DEF_NO;

  //                                                               ---------------- VALIDATE ADDR PTR -----------------
  RTOS_ASSERT_DBG((p_addr != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_NO);

  //                                                               ------------------ VALIDATE ADDR -------------------
  if ((p_addr->Addr[0] == DEF_BIT_NONE)
      && (p_addr->Addr[1] == DEF_BIT_NONE)
      && (p_addr->Addr[2] == DEF_BIT_NONE)
      && (p_addr->Addr[3] == DEF_BIT_NONE)
      && (p_addr->Addr[4] == DEF_BIT_NONE)
      && (p_addr->Addr[5] == DEF_BIT_NONE)
      && (p_addr->Addr[6] == DEF_BIT_NONE)
      && (p_addr->Addr[7] == DEF_BIT_NONE)
      && (p_addr->Addr[8] == DEF_BIT_NONE)
      && (p_addr->Addr[9] == DEF_BIT_NONE)
      && (p_addr->Addr[10] == DEF_BIT_NONE)
      && (p_addr->Addr[11] == DEF_BIT_NONE)
      && (p_addr->Addr[12] == DEF_BIT_NONE)
      && (p_addr->Addr[13] == DEF_BIT_NONE)
      && (p_addr->Addr[14] == DEF_BIT_NONE)
      && (p_addr->Addr[15] == 0x01)) {
    addr_loopback = DEF_YES;
  }

  return (addr_loopback);
}

/****************************************************************************************************//**
 *                                       NetIPv6_AddrTypeValidate()
 *
 * @brief    Validate the type of an IPv6 address.
 *
 * @param    p_addr  Pointer to IPv6 address to validate.
 *
 * @param    if_nbr  Interface number associated to the address.
 *
 * @return   - NET_IPv6_ADDR_TYPE_MCAST,         if IPv6 address is an unknown type muticast     address.
 *           - NET_IPv6_ADDR_TYPE_MCAST_SOL,     if IPv6 address is the multicast solicited node address.
 *           - NET_IPv6_ADDR_TYPE_MCAST_ROUTERS, if IPv6 address is the multicast all routers    address.
 *           - NET_IPv6_ADDR_TYPE_MCAST_NODES,   if IPv6 address is the multicast all nodes      address.
 *           - NET_IPv6_ADDR_TYPE_LINK_LOCAL,    if IPv6 address is a link-local                 address.
 *           - NET_IPv6_ADDR_TYPE_SITE_LOCAL,    if IPv6 address is a site-local                 address.
 *           - NET_IPv6_ADDR_TYPE_UNSPECIFIED,   if IPv6 address is the unspecified              address.
 *           - NET_IPv6_ADDR_TYPE_LOOPBACK,      if IPv6 address is the loopback                 address.
 *           - NET_IPv6_ADDR_TYPE_UNICAST,       otherwise.
 *******************************************************************************************************/
NET_IPv6_ADDR_TYPE NetIPv6_AddrTypeValidate(const NET_IPv6_ADDR *p_addr,
                                            NET_IF_NBR          if_nbr)

{
  CPU_BOOLEAN valid = NET_IPv6_ADDR_TYPE_NONE;

  valid = NetIPv6_IsAddrMcast(p_addr);
  if (valid == DEF_YES) {
    valid = NetIPv6_IsAddrMcastAllRouters(p_addr);
    if (valid == DEF_YES) {
      return (NET_IPv6_ADDR_TYPE_MCAST_ROUTERS);
    }

    valid = NetIPv6_IsAddrMcastAllNodes(p_addr);
    if (valid == DEF_YES) {
      return (NET_IPv6_ADDR_TYPE_MCAST_NODES);
    }

    valid = NetIPv6_IsAddrMcastRsvd(p_addr);
    if (valid == DEF_YES) {
      return (NET_IPv6_ADDR_TYPE_MCAST_RSVD);
    }

    return (NET_IPv6_ADDR_TYPE_MCAST);
  }

  valid = NetIPv6_IsAddrLoopback(p_addr);
  if (valid == DEF_YES) {
    return (NET_IPv6_ADDR_TYPE_LOOPBACK);
  }

  valid = NetIPv6_IsAddrUnspecified(p_addr);
  if (valid == DEF_YES) {
    return (NET_IPv6_ADDR_TYPE_UNSPECIFIED);
  }

  valid = NetIPv6_IsAddrLinkLocal(p_addr);
  if (valid == DEF_YES) {
    return (NET_IPv6_ADDR_TYPE_LINK_LOCAL);
  }

  valid = NetIPv6_IsAddrSiteLocal(p_addr);
  if (valid == DEF_YES) {
    return (NET_IPv6_ADDR_TYPE_SITE_LOCAL);
  }

  PP_UNUSED_PARAM(if_nbr);

  return (NET_IPv6_ADDR_TYPE_UNICAST);
}

/****************************************************************************************************//**
 *                                           NetIPv6_CreateIF_ID()
 *
 * @brief    Create an IPv6 interface identifier.
 *
 * @param    if_nbr      Network interface number to obtain link-layer hardware address.
 *
 * @param    p_addr_id   Pointer to the IPv6 address that will receive the IPv6 interface identifier.
 *
 * @param    id_type     IPv6 interface identifier type:
 *                       NET_IPv6_ADDR_AUTO_CFG_ID_IEEE_48    Universal token from IEEE 802 48-bit MAC
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_HANDLE
 *
 * @note     (1) Universal interface identifier generated from IEEE 802 48-bit MAC address is the
 *               only interface identifier actually supported.
 *
 * @internal
 * @note     (2) [INTENRAL] RFC #4291, Section 2.5.1 states that:
 *               - (a) "For all unicast addresses, except those that start with the binary value 000,
 *                     Interface IDs are required to be 64 bits long and to be constructed in Modified
 *                     EUI-64 format. Modified EUI-64 format-based interface identifiers may have
 *                     universal scope when derived from a universal token (e.g., IEEE 802 48-bit MAC
 *                     or IEEE EUI-64 identifiers [EUI64]) or may have local scope where a global
 *                     token is not available (e.g., serial links, tunnel end-points) or where global
 *                     tokens are undesirable (e.g., temporary tokens for privacy [PRIV])."
 *               - (b) "In the resulting Modified EUI-64 format, the "u" bit is set to one (1) to
 *                     indicate universal scope, and it is set to zero (0) to indicate local scope."
 * @endinternal
 *
 * @note     (3) RFC #4291, Appendix A, states that "[EUI-64] actually defines 0xFF and 0xFF as the
 *               bits to be inserted to create an IEEE EUI-64 identifier from an IEEE MAC-48 identifier.
 *               The 0xFF and 0xFE values are used when starting  with an IEEE EUI-48 identifier."
 *******************************************************************************************************/
CPU_INT08U NetIPv6_CreateIF_ID(NET_IF_NBR            if_nbr,
                               NET_IPv6_ADDR         *p_addr_id,
                               NET_IPv6_ADDR_ID_TYPE id_type,
                               RTOS_ERR              *p_err)
{
  CPU_INT08U hw_addr[NET_IF_HW_ADDR_LEN_MAX];
  CPU_INT08U hw_addr_len;
  CPU_INT08U len_rtn = NET_IPv6_ADDR_AUTO_CFG_ID_LEN_NONE;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, len_rtn);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_id != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, len_rtn);
  RTOS_ASSERT_DBG_ERR_SET((id_type == NET_IPv6_ADDR_AUTO_CFG_ID_IEEE_48), *p_err, RTOS_ERR_INVALID_ARG, len_rtn);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  hw_addr_len = sizeof(hw_addr);

  NetIPv6_AddrUnspecifiedSet(p_addr_id);                        // Clear IF ID.

  NetIF_AddrHW_GetHandler(if_nbr, hw_addr, &hw_addr_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (hw_addr_len >= 3) {
    //                                                             ------------------- CREATE IF ID -------------------
    Mem_Copy(&p_addr_id->Addr[8],                               // Copy first three octets of IF HW addr after ...
             &hw_addr[0],                                       // ... 64 bits of IF ID.
             3u);

    p_addr_id->Addr[8] ^= 0x02;                                 // Set the ID as universal.               See Note #2b.

    p_addr_id->Addr[11] = 0xFF;                                 // Insert the two octets between HW addr. See Note #3.
    p_addr_id->Addr[12] = 0xFE;
  }

  if (hw_addr_len >= 6) {
    Mem_Copy(&p_addr_id->Addr[13],                              // Copy last three octets of IF HW addr after ...
             &hw_addr[3],                                       // ... the two inserted octets.
             3u);
  }

  len_rtn = NET_IPv6_ADDR_AUTO_CFG_ID_LEN_IEEE_48;

exit:
  return (len_rtn);
}

/****************************************************************************************************//**
 *                                       NetIPv6_CreateAddrFromID()
 *
 * @brief    (1) Create an IPv6 addr from a prefix and an identifier.
 *               - (a) Validate prefix length.
 *               - (b) Append address ID to IPv6 address prefix.
 *
 * @param    p_addr_id       Pointer to IPv6 address ID.
 *
 * @param    p_addr_prefix   Pointer to variable that will receive the created IPv6 addr.
 *
 * @param    prefix_type     Prefix type:
 *                           NET_IPv6_ADDR_PREFIX_CUSTOM         Custom     prefix type
 *                           NET_IPv6_ADDR_PREFIX_LINK_LOCAL     Link-local prefix type
 *
 * @param    prefix_len      Prefix len (in bits).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function :
 *                               - RTOS_ERR_NONE
 *
 * @note     (2) If the prefix type is custom, p_addr_prefix SHOULD point on a variable that contain
 *               the prefix address. The address ID will be append to this prefix address. If the
 *               prefix type is link-local, the content of the variable pointed by p_addr_prefix does
 *               not matter and will be overwrited.
 *
 * @note     (3) If the prefix type is link-local, prefix addr SHOULD be initialized to the unspecified
 *               IPv6 addr to make sure resulting addr does not have any uninitialized values (i.e. if
 *               any of the lower 8 octets of the ID address are NOT initialized).
 *******************************************************************************************************/
void NetIPv6_CreateAddrFromID(NET_IPv6_ADDR             *p_addr_id,
                              NET_IPv6_ADDR             *p_addr_prefix,
                              NET_IPv6_ADDR_PREFIX_TYPE prefix_type,
                              CPU_SIZE_T                prefix_len,
                              RTOS_ERR                  *p_err)
{
  CPU_SIZE_T id_len_octets;
  CPU_INT08U prefix_octets_nbr;
  CPU_INT08U prefix_octets_ix;
  CPU_INT08U id_bits_nbr;
  CPU_INT08U prefix_mask;

  //                                                               ---------------- VALIDATE ARGUMENTS ----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_id != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_prefix != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET(((prefix_type == NET_IPv6_ADDR_PREFIX_CUSTOM)
                           || (prefix_type == NET_IPv6_ADDR_PREFIX_LINK_LOCAL)), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET((prefix_len <= NET_IPv6_ADDR_PREFIX_LEN_MAX), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  if (prefix_type == NET_IPv6_ADDR_PREFIX_LINK_LOCAL) {         // If addr prefix type is link-local ...
    RTOS_ASSERT_DBG_ERR_SET((prefix_len == NET_IPv6_ADDR_PREFIX_LINK_LOCAL_LEN), *p_err, RTOS_ERR_INVALID_ARG,; );

    NetIPv6_AddrUnspecifiedSet(p_addr_prefix);                  // ... init addr to unspecified addr. See Note #3.
  }

  prefix_octets_nbr = prefix_len / DEF_OCTET_NBR_BITS;          // Calc nbr of octets that will contain 8 prefix bits.
  prefix_octets_ix = prefix_octets_nbr + 1u;                    // Set ix of the first octet that might NOT contain ...
                                                                // ... 8 prefix bits.
  id_len_octets = NET_IPv6_ADDR_SIZE - prefix_octets_ix;        // Calc nbr of octets that will contain 8 ID bits.

  Mem_Copy(&p_addr_prefix->Addr[prefix_octets_ix],              // Copy ID octets                into prefix addr.
           &p_addr_id->Addr[prefix_octets_ix],
           id_len_octets);

  //                                                               Calc nbr of remaining ID bits to copy.
  id_bits_nbr = DEF_OCTET_NBR_BITS - (prefix_len % DEF_OCTET_NBR_BITS);

  prefix_mask = DEF_OCTET_MASK << id_bits_nbr;                  // Set prefix mask.

  //                                                               Copy        remaining ID bits into prefix addr.
  p_addr_prefix->Addr[prefix_octets_nbr] = ((p_addr_prefix->Addr[prefix_octets_nbr] &  prefix_mask) \
                                            | (p_addr_id->Addr[prefix_octets_nbr]     & ~prefix_mask));

  if (prefix_type == NET_IPv6_ADDR_PREFIX_LINK_LOCAL) {         // If addr prefix type is link-local ...
    p_addr_prefix->Addr[0] = 0xFE;                              // ... set link-local prefix before the ID.
    p_addr_prefix->Addr[1] = 0x80;
    p_addr_prefix->Addr[2] = DEF_BIT_NONE;
    p_addr_prefix->Addr[3] = DEF_BIT_NONE;
    p_addr_prefix->Addr[4] = DEF_BIT_NONE;
    p_addr_prefix->Addr[5] = DEF_BIT_NONE;
    p_addr_prefix->Addr[6] = DEF_BIT_NONE;
    p_addr_prefix->Addr[7] = DEF_BIT_NONE;
  }
}

/****************************************************************************************************//**
 *                                               NetIPv6_MaskGet()
 *
 * @brief    Get an IPv6 mask based on prefix length.
 *
 * @param    p_mask_rtn  Pointer to IPv6 address mask to set.
 *
 * @param    prefix_len  Length of the prefix mask in bits.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
void NetIPv6_MaskGet(NET_IPv6_ADDR *p_mask_rtn,
                     CPU_INT08U    prefix_len,
                     RTOS_ERR      *p_err)
{
  CPU_INT08U mask;
  CPU_INT08U index = 0u;                                        // Index of bit in IPv6 address.
  CPU_INT08U byte_index = 0u;                                   // Number of the current byte in the IPv6 addr.
  CPU_INT08U modulo = 0u;                                       // Modulo 8.

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_mask_rtn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((prefix_len != 0), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET((prefix_len <= NET_IPv6_ADDR_LEN_NBR_BITS), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               Set mask for given prefix.
  while (index < NET_IPv6_ADDR_LEN_NBR_BITS) {
    modulo = index % DEF_OCTET_NBR_BITS;
    if ((modulo == 0)                                           // Increment the byte index when the modulo is null.
        && (index != 0)) {
      byte_index++;
    }

    mask = 1u << modulo;                                        // Set the bit mask for the current byte.

    if (index < prefix_len) {                                   // Apply mask for current address byte.
      DEF_BIT_SET(p_mask_rtn->Addr[byte_index], mask);
    } else {
      DEF_BIT_CLR(p_mask_rtn->Addr[byte_index], mask);
    }

    index++;
  }
}

/****************************************************************************************************//**
 *                                       NetIPv6_AddrMaskByPrefixLen()
 *
 * @brief    Get the IPv6 address masked with the prefix length.
 *
 * @param    p_addr      Pointer to IPv6 address.
 *
 * @param    prefix_len  IPv6 address prefix length.
 *
 * @param    p_addr_rtn  Pointer to IPv6 address that will receive the masked address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *                           - RTOS_ERR_NONE
 *******************************************************************************************************/
void NetIPv6_AddrMaskByPrefixLen(const NET_IPv6_ADDR *p_addr,
                                 CPU_INT08U          prefix_len,
                                 NET_IPv6_ADDR       *p_addr_rtn,
                                 RTOS_ERR            *p_err)
{
  NET_IPv6_ADDR mask;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_rtn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  NetIPv6_MaskGet(&mask, prefix_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  NetIPv6_AddrMask(p_addr,
                   (const NET_IPv6_ADDR *)&mask,
                   p_addr_rtn);
}

/****************************************************************************************************//**
 *                                           NetIPv6_AddrMask()
 *
 * @brief    Apply IPv6 mask on an address.
 *
 * @param    p_addr      Pointer to IPv6 address to be masked.
 *
 * @param    p_mask      Pointer to IPv6 address mask.
 *
 * @param    p_addr_rtn  Pointer to IPv6 address that will received the masked address.
 *******************************************************************************************************/
void NetIPv6_AddrMask(const NET_IPv6_ADDR *p_addr,
                      const NET_IPv6_ADDR *p_mask,
                      NET_IPv6_ADDR       *p_addr_rtn)
{
  CPU_INT08U i;

  RTOS_ASSERT_DBG((p_addr != DEF_NULL), RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG((p_mask != DEF_NULL), RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG((p_addr_rtn != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  for (i = 0u; i < NET_IPv6_ADDR_LEN; i++) {
    p_addr_rtn->Addr[i] = p_addr->Addr[i] & p_mask->Addr[i];
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIPv6_Init()
 *
 * @brief    (1) Initialize Internet Protocol Layer :
 *               - (a) Initialize ALL interfaces' configurable IPv6 addresses.
 *               - (b) Initialize IPv6 fragmentation list pointers and fragmentation timeout.
 *               - (c) Initialize IPv6 identification (ID) counter.
 *
 * @param    p_mem_seg   Pointer to memory segment to use to allocate IPv6 objects.
 *                       DEF_NULL, to allocate from HEAP memory.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Default IPv6 address initialization is invalid & forces the developer or higher-layer
 *               protocol application to configure valid IPv6 address(s).
 *
 * @note     (2) Address configuration state initialized to 'static' by default.
 *               See also 'net_ipv6.h  NETWORK INTERFACES' IPv6 ADDRESS CONFIGURATION DATA TYPE'.
 *******************************************************************************************************/
void NetIPv6_Init(MEM_SEG  *p_mem_seg,
                  RTOS_ERR *p_err)
{
  //                                                               ------------- CREATE IPV6 ADDRESS POOL -------------
  Mem_DynPoolCreate("IPv6 Address Pool",
                    &NetIPv6_AddrPool,
                    p_mem_seg,
                    sizeof(NET_IPv6_ADDR_OBJ),
                    sizeof(CPU_ALIGN),
                    1,
                    NET_IPv6_CFG_IF_MAX_NBR_ADDR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

#ifdef NET_DAD_MODULE_EN
  NetDAD_Init(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

  //                                                               -------------- INIT IPV6 AUTOCFG OBJ ---------------
#ifdef NET_IPv6_ADDR_AUTO_CFG_MODULE_EN

  Mem_DynPoolCreate("IPv6 Auto Cfg Object Pool",
                    &NetIPv6_AutoCfgObjPool,
                    p_mem_seg,
                    sizeof(NET_IPv6_AUTO_CFG_OBJ),
                    sizeof(CPU_ALIGN),
                    NET_IF_CFG_MAX_NBR_IF,
                    NET_IF_CFG_MAX_NBR_IF,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
#endif

  //                                                               --- INITIALIZE SUBSCRIBER LIST FOR IPV6 ADDR CFG ---
  SList_Init(&NetIPv6_AddrSubscriberListHeadPtr);
  Mem_DynPoolCreate("IPv6 Address Subscriber Pool",
                    &NetIPv6_AddrSubscriberPool,
                    p_mem_seg,
                    sizeof(NET_IPv6_ADDR_SUBSCRIBE_OBJ),
                    sizeof(CPU_ALIGN),
                    NET_IF_CFG_MAX_NBR_IF,
                    NET_IF_CFG_MAX_NBR_IF,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ---- INITIALIZE LIST FOR IPV6 ADDR TO CONFIGURE ----
  SList_Init(&NetIPv6_AddrToCfgListPtr);
  Mem_DynPoolCreate("IPv6 Address to cfg Pool",
                    &NetIPv6_AddrToCfgPool,
                    p_mem_seg,
                    sizeof(NET_IPv6_ADDR_TO_CFG),
                    sizeof(CPU_ALIGN),
                    1,
                    NET_IPv6_CFG_IF_MAX_NBR_ADDR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               --------------- INIT IPv6 FRAG LISTS ---------------
  NetIPv6_FragReasmListsHead = DEF_NULL;
  NetIPv6_FragReasmListsTail = DEF_NULL;

  //                                                               -------------- INIT IPv6 FRAG TIMEOUT --------------
  NetIPv6_CfgFragReasmTimeout(NET_IPv6_FRAG_REASM_TIMEOUT_DFLT_SEC);

  //                                                               ----------------- INIT IPv6 ID CTR -----------------
  NetIPv6_TxID_Ctr = NET_IPv6_ID_INIT;

  //                                                               ----------------- INIT NDP MODULE ------------------
  NetNDP_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ----------------- INIT MDLP MODULE -----------------
  NetMLDP_Init(p_mem_seg, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv6_LinkStateSubscriber()
 *
 * @brief    (1) IPv6 subscriber function to link change. When link becomes UP:
 *               - (a) Disabled all configured IPv6 addresses.
 *               - (b) Do IPv6 Address Static Configuration for addresses that where statically configured.
 *               - (c) Do IPv6 Auto-configuration if conditions apply.
 *
 * @param    if_nbr      Network interface number on which link state change occurred.
 *
 * @param    link_state  Current link state of given interface.
 *******************************************************************************************************/
void NetIPv6_LinkStateSubscriber(NET_IF_NBR        if_nbr,
                                 NET_IF_LINK_STATE link_state)
{
  NET_IF               *p_if = DEF_NULL;
  NET_IPv6_ADDR_OBJ    *p_addr_obj = DEF_NULL;
  NET_IPv6_ADDR_TO_CFG *p_addr_to_cfg = DEF_NULL;
  NET_IP_ADDR_CFG_MODE addr_cfg_mode = NET_IP_ADDR_CFG_MODE_NONE;
  RTOS_ERR             local_err;
#ifdef NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
  NET_IPv6_AUTO_CFG_OBJ *p_auto_obj = DEF_NULL;
  CPU_BOOLEAN           auto_en = DEF_NO;
  CORE_DECLARE_IRQ_STATE;
#endif

  //                                                               ----------- ACQUIRE NETWORK GLOBAL LOCK ------------
  Net_GlobalLockAcquire((void *)NetIPv6_LinkStateSubscriber);

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  p_if = NetIF_Get(if_nbr, &local_err);                         // Retrieve Interface object.
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               --------- CHECK IF ADDRESSES ARE CFG ON IF ---------
  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv6_AddrListPtr, p_addr_obj, NET_IPv6_ADDR_OBJ, ListNode) {
    p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_NONE;
    p_addr_obj->IsValid = DEF_NO;
    addr_cfg_mode = p_addr_obj->AddrCfgMode;
    if (link_state == NET_IF_LINK_UP) {
      if (addr_cfg_mode == NET_IP_ADDR_CFG_MODE_STATIC) {
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        (void)NetIPv6_CfgAddrReset(if_nbr,
                                   &p_addr_obj->AddrHost,
                                   &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          goto exit;
        }
      }
    } else {
      if (addr_cfg_mode == NET_IP_ADDR_CFG_MODE_AUTO_CFG) {
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        NetIPv6_CfgAddrRemoveHandler(if_nbr,
                                     &p_addr_obj->AddrHost,
                                     &local_err);
        if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
          goto exit;
        }
      } else {
        //                                                         LEAVE MLDP GROUP OF THE MCAST SOLICITED ADDR
        if (p_addr_obj->AddrMcastSolicitedPtr != DEF_NULL) {
          RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
          NetMLDP_HostGrpLeaveHandler(if_nbr,
                                      p_addr_obj->AddrMcastSolicitedPtr,
                                      &local_err);
        }
      }
    }
  }

  if (link_state == NET_IF_LINK_UP) {
    //                                                             ---------- CHECK FOR ADDRESS TO CONFIGURE ----------
    while (NetIPv6_AddrToCfgListPtr != DEF_NULL) {
      SLIST_MEMBER *p_node;

      p_node = SList_Pop(&NetIPv6_AddrToCfgListPtr);

      p_addr_to_cfg = SLIST_ENTRY(p_node, NET_IPv6_ADDR_TO_CFG, ListNode);

      (void)NetIPv6_CfgAddrAddHandler(if_nbr,
                                      &p_addr_to_cfg->Addr,
                                      p_addr_to_cfg->PrefixLen,
                                      0,
                                      0,
                                      NET_IP_ADDR_CFG_MODE_STATIC,
                                      p_addr_to_cfg->DAD_En,
                                      NET_IPv6_ADDR_CFG_TYPE_STATIC_NO_BLOCKING,
                                      DEF_NULL,
                                      &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
        Mem_DynPoolBlkFree(&NetIPv6_AddrToCfgPool, p_addr_to_cfg, &local_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        goto exit;
      }

      Mem_DynPoolBlkFree(&NetIPv6_AddrToCfgPool, p_addr_to_cfg, &local_err);
      RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }
  }

  //                                                               --------- RESTART IPV6 AUTO-CONFIGURATION ----------
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
  if ((link_state == NET_IF_LINK_UP)
      && (p_if->IP_Obj->IPv6_AutoCfgObjPtr != DEF_NULL)      ) {
    p_auto_obj = p_if->IP_Obj->IPv6_AutoCfgObjPtr;
    CORE_ENTER_ATOMIC();
    auto_en = p_auto_obj->En;
    CORE_EXIT_ATOMIC();

    if (auto_en == DEF_YES) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetIPv6_AddrAutoCfgHandler(if_nbr,
                                 &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.AddrAutoCfgFaultCtr);
      }
    }
  }
#endif

  PP_UNUSED_PARAM(link_state);

  //                                                               ----------------- RELEASE NET LOCK -----------------
exit:
  Net_GlobalLockRelease();
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv6_AddrAutoCfgEnHandler()
 *
 * @brief    Enable IPv6 Auto-configuration process. If the link state is UP, the IPv6 Auto-configuration
 *           will start, else the Auto-configuration will start when the link becomes UP.
 *
 * @param    if_nbr  Network interface number to enable the address auto-configuration on.
 *
 * @param    dad_en  DEF_YES, Do the Duplication Address Detection (DAD).
 *                   DEF_NO , otherwise
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if IPv6 Auto-Configuration is enabled successfully
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
CPU_BOOLEAN NetIPv6_AddrAutoCfgEnHandler(NET_IF_NBR  if_nbr,
                                         CPU_BOOLEAN dad_en,
                                         RTOS_ERR    *p_err)
{
  NET_IF                *p_if = DEF_NULL;
  CPU_BOOLEAN           result = DEF_FAIL;
  NET_IPv6_AUTO_CFG_OBJ *p_auto_obj = DEF_NULL;
  NET_IF_LINK_STATE     link_state = NET_IF_LINK_DOWN;

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  if (p_if->IP_Obj->IPv6_AutoCfgObjPtr != DEF_NULL) {
    p_auto_obj = p_if->IP_Obj->IPv6_AutoCfgObjPtr;
  } else {
    p_auto_obj = (NET_IPv6_AUTO_CFG_OBJ *)Mem_DynPoolBlkGet(&NetIPv6_AutoCfgObjPool, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
      return (DEF_FAIL);
    }

    p_if->IP_Obj->IPv6_AutoCfgObjPtr = p_auto_obj;
  }

  //                                                               ----------- SAVED PREFERENCE FOR AUTOCFG -----------
  p_auto_obj->En = DEF_YES;
  p_auto_obj->DAD_En = (dad_en == DEF_YES) ? DEF_YES : DEF_NO;

  //                                                               ----------------- CHECK LINK STATE -----------------
  link_state = NetIF_LinkStateGetHandler(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  if (link_state == NET_IF_LINK_UP) {
    NetIPv6_AddrAutoCfgHandler(if_nbr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_FAIL);
    }
  }

  p_if->StartModulesCfgFlags.IPv6_AutoCfg = DEF_YES;

  result = DEF_OK;

  return (result);
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv6_CfgAddrAddHandler()
 *
 * @brief    (1) Add a statically-configured IPv6 host address to an interface :
 *               - (a) Validate  address configuration :
 *                   - (1) Check if address is already configured.
 *                   - (2) Validate number of configured IP addresses.
 *               - (b) Configure static IPv6 address
 *               - (c) Join multicast group associated with address.
 *               - (d) Do Duplication address detection (DAD) if enabled.
 *
 * @param    if_nbr      Interface number to configure.
 *
 * @param    p_addr      Pointer to desired IPv6 address to add to this interface (see Note #5).
 *
 * @param    prefix_len  Prefix length of the desired IPv6 address to add to this interface.
 *
 * @param    cfg_mode    Desired value for configuration mode :
 *                       NET_IPv6_ADDR_CFG_MODE_MANUAL   Address is configured manually.
 *                       NET_IPv6_ADDR_CFG_MODE_AUTO     Address is configured using stateless address
 *                                                       auto-configuration.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if valid IPv6 address configured.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
NET_IPv6_ADDR_CFG_STATUS NetIPv6_CfgAddrAddHandler(NET_IF_NBR             if_nbr,
                                                   NET_IPv6_ADDR          *p_addr,
                                                   CPU_INT08U             prefix_len,
                                                   CPU_INT32U             lifetime_valid,
                                                   CPU_INT32U             lifetime_preferred,
                                                   NET_IP_ADDR_CFG_MODE   cfg_mode,
                                                   CPU_BOOLEAN            dad_en,
                                                   NET_IPv6_ADDR_CFG_TYPE addr_cfg_type,
                                                   NET_IPv6_ADDR_OBJ      **p_addr_obj_rtn,
                                                   RTOS_ERR               *p_err)
{
  NET_IF                   *p_if = DEF_NULL;
  NET_IPv6_ADDR_OBJ        *p_addr_obj = DEF_NULL;
  NET_IPv6_ADDR_CFG_STATUS cfg_status = NET_IPv6_ADDR_CFG_STATUS_NONE;
  NET_IF_LINK_STATE        link_state;
  CPU_BOOLEAN              is_addr_cfgd;
  CPU_INT32U               timeout_ms;
  NET_MLDP_HOST_GRP        *p_host_grp;
  NET_IPv6_ADDR            addr_mcast_sol;
#ifdef NET_DAD_MODULE_EN
  NET_DAD_FNCT   dad_hook_fnct = DEF_NULL;
  NET_DAD_STATUS dad_status;
#endif
  RTOS_ERR             local_err;
  NET_IP_ADDR_CFG_MODE addr_cfg_mode = NET_IP_ADDR_CFG_MODE_NONE;

  //                                                               -- VALIDATE THAT ADDRESS DOES NOT ALREADY EXISTS ---
  is_addr_cfgd = NetIPv6_IsAddrHostCfgdHandler(p_addr);
  if (is_addr_cfgd == DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    goto exit;
  }

  //                                                               ------------- VALIDATE THAT LINK IS UP -------------
  link_state = NetIF_LinkStateGetHandler(if_nbr, p_err);
  if (link_state != NET_IF_LINK_UP) {
    NET_IPv6_ADDR_TO_CFG *p_addr_node;

    switch (addr_cfg_type) {
      case NET_IPv6_ADDR_CFG_TYPE_STATIC_BLOCKING:
        RTOS_ERR_SET(*p_err, RTOS_ERR_NET_IF_LINK_DOWN);
        goto exit;

      case NET_IPv6_ADDR_CFG_TYPE_STATIC_NO_BLOCKING:
        p_addr_node = (NET_IPv6_ADDR_TO_CFG *)Mem_DynPoolBlkGet(&NetIPv6_AddrToCfgPool, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }

        Mem_Copy(&p_addr_node->Addr, p_addr, NET_IPv6_ADDR_SIZE);
        p_addr_node->PrefixLen = prefix_len;
        p_addr_node->DAD_En = dad_en;

        SList_Push(&NetIPv6_AddrToCfgListPtr, &p_addr_node->ListNode);
        cfg_status = NET_IPv6_ADDR_CFG_STATUS_NOT_DONE;
        goto exit;

      default:
        cfg_status = NET_IPv6_ADDR_CFG_STATUS_NOT_DONE;
        goto exit;
    }
  }

  //                                                               ------------ RETRIEVE INTERFACE OBJECT -------------
  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               -------------- RETRIVE ADDR MEM BLOCK --------------
  p_addr_obj = (NET_IPv6_ADDR_OBJ *)Mem_DynPoolBlkGet(&NetIPv6_AddrPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------ INITIALIZE ADDRESS FIELDS -------------
  p_addr_obj->DAD_En = dad_en;
  p_addr_obj->IsValid = DEF_NO;
  p_addr_obj->AddrMcastSolicitedPtr = DEF_NULL;
  p_addr_obj->PrefLifetimeTmrPtr = DEF_NULL;
  p_addr_obj->ValidLifetimeTmrPtr = DEF_NULL;

  //                                                               Configure addr cfg state.
  p_addr_obj->AddrCfgMode = cfg_mode;
  p_addr_obj->AddrCfgType = addr_cfg_type;

  Mem_Copy(&p_addr_obj->AddrHost,                               // Configure host address.
           p_addr,
           NET_IPv6_ADDR_SIZE);

  p_addr_obj->AddrHostPrefixLen = prefix_len;                   // Configure address prefix length.
  p_addr_obj->IfNbr = if_nbr;                                   // Configure IF number of address.

  //                                                               ---------- ADD ADDRESS TO IPV6 ADDR LIST -----------
  SList_PushBack(&p_if->IP_Obj->IPv6_AddrListPtr, &p_addr_obj->ListNode);

  //                                                               --------------- JOIN MULTICAST GROUP ---------------
  NetIPv6_AddrMcastSolicitedSet(&addr_mcast_sol,                // Create solicited mcast addr.
                                p_addr,
                                if_nbr,
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetIPv6_CfgAddrRemoveHandler(if_nbr, p_addr, &local_err);
    goto exit;
  }

  p_host_grp = NetMLDP_HostGrpJoinHandler(if_nbr,               // Join  mcast group of the solicited mcast.
                                          &addr_mcast_sol,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetIPv6_CfgAddrRemoveHandler(if_nbr, p_addr, &local_err);
    goto exit;
  }

  p_addr_obj->AddrMcastSolicitedPtr = &p_host_grp->AddrGrp;

  if (lifetime_valid > 0) {                                     // Set addr Valid Lifetime Tmr
    timeout_ms = lifetime_valid * 1000;

    p_addr_obj->ValidLifetimeTmrPtr = NetTmr_Get(NetIPv6_AddrValidLifetimeTimeout,
                                                 p_addr_obj,
                                                 timeout_ms,
                                                 NET_TMR_OPT_NONE,
                                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetIPv6_CfgAddrRemoveHandler(if_nbr, p_addr, &local_err);
      goto exit;
    }
  }

  if (lifetime_preferred > 0) {                                 // Set addr Preferred Lifetime Tmr.
    timeout_ms = lifetime_preferred * 1000;

    p_addr_obj->PrefLifetimeTmrPtr = NetTmr_Get(NetIPv6_AddrPrefLifetimeTimeout,
                                                p_addr_obj,
                                                timeout_ms,
                                                NET_TMR_OPT_NONE,
                                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetIPv6_CfgAddrRemoveHandler(if_nbr, p_addr, &local_err);
      goto exit;
    }
  }

#ifdef NET_DAD_MODULE_EN
  if (dad_en == DEF_YES) {
    p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_TENTATIVE;
    p_addr_obj->IsValid = DEF_NO;

    switch (p_addr_obj->AddrCfgType) {
      case NET_IPv6_ADDR_CFG_TYPE_NONE:
      case NET_IPv6_ADDR_CFG_TYPE_AUTO_CFG_BLOCKING:
      case NET_IPv6_ADDR_CFG_TYPE_STATIC_BLOCKING:
        break;

      case NET_IPv6_ADDR_CFG_TYPE_STATIC_NO_BLOCKING:
      case NET_IPv6_ADDR_CFG_TYPE_RX_PREFIX_INFO:
        dad_hook_fnct = NetIPv6_CfgAddrAddDAD_Result;
        break;

#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
      case NET_IPv6_ADDR_CFG_TYPE_AUTO_CFG_NO_BLOCKING:
        dad_hook_fnct = NetIPv6_AddrAutoCfgDAD_Result;
        break;
#endif

      default:
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        NetIPv6_CfgAddrRemoveHandler(if_nbr, p_addr, &local_err);
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, NET_IPv6_ADDR_CFG_STATUS_FAIL);
    }

    //                                                             Do DAD on address configured and ...
    //                                                             ... configure addr state.
    dad_status = NetDAD_Start(if_nbr, &p_addr_obj->AddrHost, p_addr_obj->AddrCfgType, dad_hook_fnct, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetIPv6_CfgAddrRemoveHandler(if_nbr, p_addr, &local_err);
      cfg_status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
      goto exit;
    }

    switch (dad_status) {
      case NET_DAD_STATUS_SUCCEED:
        cfg_status = NET_IPv6_ADDR_CFG_STATUS_SUCCEED;
        break;

      case NET_DAD_STATUS_IN_PROGRESS:
        cfg_status = NET_IPv6_ADDR_CFG_STATUS_IN_PROGRESS;
        break;

      case NET_DAD_STATUS_FAIL:
        cfg_status = NET_IPv6_ADDR_CFG_STATUS_DUPLICATE;
        break;

      default:
        cfg_status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        NetIPv6_CfgAddrRemoveHandler(if_nbr, p_addr, &local_err);
        RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, NET_IPv6_ADDR_CFG_STATUS_FAIL);
    }
  } else {
    p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_PREFERRED;
    p_addr_obj->IsValid = DEF_YES;
    cfg_status = NET_IPv6_ADDR_CFG_STATUS_SUCCEED;
  }

#else
  p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_PREFERRED;
  p_addr_obj->IsValid = DEF_YES;
  cfg_status = NET_IPv6_ADDR_CFG_STATUS_SUCCEED;
#endif

  //                                                               - ADD PREFIX TO LIST IF ADDRESS IS NOT LINK-LOCAL --
  if (cfg_status == NET_IPv6_ADDR_CFG_STATUS_SUCCEED) {
    NET_IPv6_ADDR prefix;
    CPU_BOOLEAN   is_link_local;
    RTOS_ERR      local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    is_link_local = NetIPv6_IsAddrLinkLocal(&p_addr_obj->AddrHost);
    if (is_link_local != DEF_YES) {
      NetIPv6_AddrMaskByPrefixLen(&p_addr_obj->AddrHost, p_addr_obj->AddrHostPrefixLen, &prefix, &local_err);
      NetNDP_PrefixAddCfg(if_nbr, &prefix, p_addr_obj->AddrHostPrefixLen, DEF_NO, DEF_NULL, 0, &local_err);
    }

    if (p_addr_obj->AddrCfgMode == NET_IP_ADDR_CFG_MODE_STATIC) {
      NET_IPv6_ADDR_SUBSCRIBE_OBJ *p_subscriber;

      SLIST_FOR_EACH_ENTRY(NetIPv6_AddrSubscriberListHeadPtr, p_subscriber, NET_IPv6_ADDR_SUBSCRIBE_OBJ, ListNode) {
        if (p_subscriber->Fnct != DEF_NULL) {
          p_subscriber->Fnct(if_nbr, NET_IPv6_CFG_ADDR_TYPE_STATIC, p_addr, cfg_status);
        }
      }
    }
  }

  if (p_addr_obj_rtn != DEF_NULL) {
    *p_addr_obj_rtn = p_addr_obj;
  }

  PP_UNUSED_PARAM(dad_en);
  PP_UNUSED_PARAM(addr_cfg_type);
  PP_UNUSED_PARAM(addr_cfg_mode);

exit:
  return (cfg_status);
}

/****************************************************************************************************//**
 *                                       NetIPv6_CfgAddrRemoveHandler()
 *
 * @brief    (1) Remove a configured IPv6 host address & multicast solicited mode address from an
 *               interface :
 *               - (a) Validate address to remove :
 *                   - (1) Validate interface
 *                   - (2) Validate IPv6 address
 *               - (b) Remove configured IPv6 address from interface's IPv6 address table :
 *                   - (1) Search table for address to remove
 *                   - (2) Close all connections for address
 *
 * @param    if_nbr          Interface number        to remove address configuration.
 *
 * @param    p_addr_host     Pointer to IPv6 address to remove.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if IPv6 address configuration removed.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) NetIPv6_CfgAddrRemoveHandler() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_CfgAddrRemoveHandler(NET_IF_NBR    if_nbr,
                                         NET_IPv6_ADDR *p_addr_host,
                                         RTOS_ERR      *p_err)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv6_ADDR_OBJ *p_addr_obj;
  CPU_BOOLEAN       found = DEF_NO;
  CPU_BOOLEAN       result = DEF_FAIL;
  RTOS_ERR          local_err;

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (p_if->IP_Obj->IPv6_AddrListPtr == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv6_AddrListPtr, p_addr_obj, NET_IPv6_ADDR_OBJ, ListNode) {
    found = Mem_Cmp(&p_addr_obj->AddrHost, p_addr_host, NET_IPv6_ADDR_SIZE);
    if (found == DEF_YES) {
      break;
    }
  }

  if (found != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);                               // ... rtn err.
    goto exit;
  }

  //                                                               -------- CLOSE ALL IPv6 ADDR CONNS ---------
  //                                                               Close all cfg'd addr's conns.
  NetConn_CloseAllConnsByAddrHandler((CPU_INT08U *)&p_addr_obj->AddrHost, NET_IPv6_ADDR_SIZE);

  //                                                               ----------- FREE ADDRESS TIMERS ------------
  NetTmr_Free(p_addr_obj->PrefLifetimeTmrPtr);
  NetTmr_Free(p_addr_obj->ValidLifetimeTmrPtr);

  p_addr_obj->PrefLifetimeTmrPtr = DEF_NULL;
  p_addr_obj->ValidLifetimeTmrPtr = DEF_NULL;

#ifdef NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
  //                                                               Stop Auto-Cfg Process if address is from auto-cfg.
  if (p_addr_obj->AddrCfgMode == NET_IP_ADDR_CFG_MODE_AUTO_CFG) {
    NetIPv6_AddrAutoCfgStop(if_nbr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }
#endif
  //                                                               LEAVE MLDP GROUP OF THE MCAST SOLICITED ADDR
  if (p_addr_obj->AddrMcastSolicitedPtr != DEF_NULL) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetMLDP_HostGrpLeaveHandler(if_nbr,
                                p_addr_obj->AddrMcastSolicitedPtr,
                                &local_err);
  }

  //                                                               ------ REMOVE IPv6 ADDR FROM ADDR TBL ------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  SList_Rem(&p_if->IP_Obj->IPv6_AddrListPtr, &p_addr_obj->ListNode);
  Mem_DynPoolBlkFree(&NetIPv6_AddrPool, p_addr_obj, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_FAIL);

  result = DEF_OK;

exit:
  return (result);
}

/****************************************************************************************************//**
 *                                       NetIPv6_CfgAddrRemoveAllHandler()
 *
 * @brief    (1) Remove all configured IPv6 host address(s) from an interface :
 *               - (a) Validate IPv6 address configuration state                                   See Note #3b
 *               - (b) Remove ALL configured IPv6 address(s) from interface's IPv6 address table :
 *                   - (1) Close all connections for each address
 *               - (c) Reset    IPv6 address configuration state to static                         See Note #3c
 *
 * @param    if_nbr  Interface number to remove address configuration.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 *
 * Argument(s) : if_nbr      Interface number to remove address configuration.
 *
 *               p_err        Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : DEF_OK,   if ALL interface's configured IPv6 host address(s) successfully removed.
 *               DEF_FAIL, otherwise.
 *
 * Note(s)     : (2) NetIPv6_CfgAddrRemoveAllHandler() is called by network protocol suite function(s) &
 *                   MUST be called with the global network lock already acquired.
 *
 *                   See also 'NetIPv6_CfgAddrRemoveAll()  Note #2'.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_CfgAddrRemoveAllHandler(NET_IF_NBR if_nbr,
                                            RTOS_ERR   *p_err)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv6_ADDR_OBJ *p_addr_obj = DEF_NULL;
  SLIST_MEMBER      *p_node;
  CPU_BOOLEAN       result = DEF_FAIL;
  RTOS_ERR          local_err;

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  while (p_if->IP_Obj->IPv6_AddrListPtr != DEF_NULL) {
    p_node = SList_Pop(&p_if->IP_Obj->IPv6_AddrListPtr);

    p_addr_obj = SLIST_ENTRY(p_node, NET_IPv6_ADDR_OBJ, ListNode);

    //                                                             Close  all cfg'd addr's conns.
    NetConn_CloseAllConnsByAddrHandler((CPU_INT08U *)&p_addr_obj->AddrHost, NET_IPv6_ADDR_SIZE);

#ifdef NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
    //                                                             Stop Auto-Cfg Process if address is from auto-cfg.
    if (p_addr_obj->AddrCfgMode == NET_IP_ADDR_CFG_MODE_AUTO_CFG) {
      NetIPv6_AddrAutoCfgStop(if_nbr, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
    }
#endif

    //                                                             Leave the MLDP grp of the mcast solicited addr.
    if (p_addr_obj->AddrMcastSolicitedPtr != DEF_NULL) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetMLDP_HostGrpLeaveHandler(if_nbr,
                                  p_addr_obj->AddrMcastSolicitedPtr,
                                  &local_err);
    }

    //                                                             Remove addr from tbl.

    NetIPv6_AddrUnspecifiedSet(&p_addr_obj->AddrHost);

    p_addr_obj->AddrMcastSolicitedPtr = DEF_NULL;
    p_addr_obj->AddrCfgMode = NET_IP_ADDR_CFG_MODE_NONE;
    p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_NONE;
    p_addr_obj->AddrHostPrefixLen = 0u;
    p_addr_obj->IfNbr = NET_IF_NBR_NONE;
    p_addr_obj->IsValid = DEF_NO;

    NetTmr_Free(p_addr_obj->PrefLifetimeTmrPtr);
    NetTmr_Free(p_addr_obj->ValidLifetimeTmrPtr);

    p_addr_obj->PrefLifetimeTmrPtr = DEF_NULL;
    p_addr_obj->ValidLifetimeTmrPtr = DEF_NULL;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    Mem_DynPoolBlkFree(&NetIPv6_AddrPool, p_addr_obj, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_FAIL);
  }

  result = DEF_OK;

exit:
  return (result);
}

/****************************************************************************************************//**
 *                                           NetIPv6_CfgAddrReset()
 *
 * @brief    Reset a configured address.
 *
 * @param    if_nbr  Interface number on which the address is configured.
 *
 * @param    p_addr  Pointer to the IPv6 address to reset.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
NET_IPv6_ADDR_CFG_STATUS NetIPv6_CfgAddrReset(NET_IF_NBR    if_nbr,
                                              NET_IPv6_ADDR *p_addr,
                                              RTOS_ERR      *p_err)
{
  NET_IPv6_ADDR_OBJ        *p_addr_obj;
  NET_MLDP_HOST_GRP        *p_host_grp;
  NET_IPv6_ADDR            addr_mcast_sol;
  NET_IF_NBR               if_nbr_tmp;
  NET_IF_LINK_STATE        link_state;
  NET_IPv6_ADDR_CFG_STATUS cfg_status = NET_IPv6_ADDR_CFG_STATUS_NONE;
  CPU_INT32U               timeout_ms;
  CPU_BOOLEAN              is_addr_cfgd;

  //                                                               ------------ FOUND ADDR OBJECT IN LIST -------------
  is_addr_cfgd = NetIPv6_IsAddrHostCfgdHandler(p_addr);
  if (is_addr_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    cfg_status = NET_IPv6_ADDR_CFG_STATUS_NONE;
    goto exit;
  }

  p_addr_obj = NetIPv6_GetAddrsHost(p_addr, &if_nbr_tmp);
  if (if_nbr != if_nbr_tmp) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    cfg_status = NET_IPv6_ADDR_CFG_STATUS_NONE;
    goto exit;
  }

  //                                                               ------------- VALIDATE THAT LINK IS UP -------------
  link_state = NetIF_LinkStateGetHandler(if_nbr, p_err);
  if (link_state != NET_IF_LINK_UP) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_IF_LINK_DOWN);
    cfg_status = NET_IPv6_ADDR_CFG_STATUS_NONE;
    goto exit;
  }

  //                                                               ------------ RE-JOINED MULTICAST GROUP -------------
  NetMLDP_HostGrpLeaveHandler(if_nbr, p_addr_obj->AddrMcastSolicitedPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    cfg_status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
    goto exit_remove;
  }

  NetIPv6_AddrMcastSolicitedSet(&addr_mcast_sol,            // Create solicited mcast addr.
                                p_addr,
                                if_nbr,
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    cfg_status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
    goto exit_remove;
  }

  p_host_grp = NetMLDP_HostGrpJoinHandler(if_nbr,       // Join  mcast group of the solicited mcast.
                                          &addr_mcast_sol,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    cfg_status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
    goto exit_remove;
  }
  p_addr_obj->AddrMcastSolicitedPtr = &p_host_grp->AddrGrp;

  //                                                               -------------- RESET ADDRESS TIMEOUTS --------------
  timeout_ms = 0;
  if (p_addr_obj->ValidLifetimeTmrPtr != DEF_NULL) {
    timeout_ms = p_addr_obj->ValidLifetimeTmrPtr->Val;
  }
  if (timeout_ms > 0) {                                         // Set addr Valid Lifetime Tmr
    NetTmr_Set(p_addr_obj->ValidLifetimeTmrPtr,
               NetIPv6_AddrValidLifetimeTimeout,
               timeout_ms);
  }

  timeout_ms = 0;
  if (p_addr_obj->PrefLifetimeTmrPtr != DEF_NULL) {
    timeout_ms = p_addr_obj->PrefLifetimeTmrPtr->Val;
  }

  if (timeout_ms > 0) {                                         // Set addr Preferred Lifetime Tmr.
    NetTmr_Set(p_addr_obj->PrefLifetimeTmrPtr,
               NetIPv6_AddrPrefLifetimeTimeout,
               timeout_ms);
  }

  //                                                               --------------- REDO DAD IF ENABLED ----------------
#ifdef NET_DAD_MODULE_EN
  if (p_addr_obj->DAD_En == DEF_YES) {
    NET_DAD_FNCT   dad_hook_fnct = DEF_NULL;
    NET_DAD_STATUS dad_status;

    p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_TENTATIVE;
    p_addr_obj->IsValid = DEF_NO;

    switch (p_addr_obj->AddrCfgType) {
      case NET_IPv6_ADDR_CFG_TYPE_NONE:
      case NET_IPv6_ADDR_CFG_TYPE_AUTO_CFG_BLOCKING:
        dad_hook_fnct = DEF_NULL;
        break;

      case NET_IPv6_ADDR_CFG_TYPE_STATIC_BLOCKING:
        p_addr_obj->AddrCfgType = NET_IPv6_ADDR_CFG_TYPE_STATIC_NO_BLOCKING;
        dad_hook_fnct = NetIPv6_CfgAddrAddDAD_Result;
        break;

      case NET_IPv6_ADDR_CFG_TYPE_STATIC_NO_BLOCKING:
      case NET_IPv6_ADDR_CFG_TYPE_RX_PREFIX_INFO:
        dad_hook_fnct = NetIPv6_CfgAddrAddDAD_Result;
        break;

#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
      case NET_IPv6_ADDR_CFG_TYPE_AUTO_CFG_NO_BLOCKING:
        dad_hook_fnct = NetIPv6_AddrAutoCfgDAD_Result;
        break;
#endif

      default:
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, NET_IPv6_ADDR_CFG_STATUS_FAIL);
    }

    //                                                             Do DAD on address configured and ...
    //                                                             ... configure addr state.
    dad_status = NetDAD_Start(if_nbr, &p_addr_obj->AddrHost, p_addr_obj->AddrCfgType, dad_hook_fnct, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_remove;
    }

    switch (dad_status) {
      case NET_DAD_STATUS_SUCCEED:
        cfg_status = NET_IPv6_ADDR_CFG_STATUS_SUCCEED;
        break;

      case NET_DAD_STATUS_IN_PROGRESS:
        cfg_status = NET_IPv6_ADDR_CFG_STATUS_IN_PROGRESS;
        break;

      case NET_DAD_STATUS_FAIL:
        cfg_status = NET_IPv6_ADDR_CFG_STATUS_DUPLICATE;
        break;

      default:
        cfg_status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
        goto exit_remove;
    }
  } else {
    p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_PREFERRED;
    p_addr_obj->IsValid = DEF_YES;
    cfg_status = NET_IPv6_ADDR_CFG_STATUS_SUCCEED;
  }

#else
  p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_PREFERRED;
  p_addr_obj->IsValid = DEF_YES;
  cfg_status = NET_IPv6_ADDR_CFG_STATUS_SUCCEED;
#endif

  if ((p_addr_obj->AddrCfgMode == NET_IP_ADDR_CFG_MODE_STATIC)
      && (cfg_status != NET_IPv6_ADDR_CFG_STATUS_IN_PROGRESS)) {
    NET_IPv6_ADDR_SUBSCRIBE_OBJ *p_subscriber;

    SLIST_FOR_EACH_ENTRY(NetIPv6_AddrSubscriberListHeadPtr, p_subscriber, NET_IPv6_ADDR_SUBSCRIBE_OBJ, ListNode) {
      if (p_subscriber->Fnct != DEF_NULL) {
        p_subscriber->Fnct(if_nbr, NET_IPv6_CFG_ADDR_TYPE_STATIC, p_addr, cfg_status);
      }
    }
  }

  goto exit;

exit_remove:
  {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetIPv6_CfgAddrRemoveHandler(if_nbr, p_addr, &local_err);
    PP_UNUSED_PARAM(local_err);
  }

exit:
  return (cfg_status);
}

/****************************************************************************************************//**
 *                                       NetIPv6_GetAddrHostHandler()
 *
 * @brief    Get an interface's IPv6 host address(s) [see Note #2].
 *
 * @param    if_nbr          Interface number to get IPv6 host address(s).
 *
 * @param    p_addr_tbl      Pointer to IPv6 address table that will receive the IPv6 host address(s)
 *                           in host-order for this interface.
 *
 * @param    p_addr_tbl_qty  Pointer to a variable to ... :
 *                           - (a) Pass the size of the address table, in number of IPv6 addresses,
 *                                 pointed to by 'p_addr_tbl'.
 *                           - (b) Return the actual number of IPv6 addresses, if NO error(s);
 *                           - (c) Return 0,                                   otherwise.
 *                                 ee also Note #3.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if interface's IPv6 host address(s) successfully returned.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) NetIPv6_GetAddrHostHandler() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *               See also 'NetIPv6_GetAddrHost()  Note #1'.
 *
 * @note     (2) IPv6 address(s) returned in host-order.
 *
 * @note     (3) Since 'p_addr_tbl_qty' argument is both an input & output argument
 *               (see 'Argument(s) : p_addr_tbl_qty'), ... :
 *               - (a) Its input value SHOULD be validated prior to use; ...
 *                   - (1) In the case that the 'p_addr_tbl_qty' argument is passed a null pointer,
 *                         NO input value is validated or used.
 *                   - (2) The number of IP addresses of the table that will receive the configured
 *                         IP address(s) MUST be greater than or equal to NET_IPv6_CFG_IF_MAX_NBR_ADDR.
 *               - (b) While its output value MUST be initially configured to return a default value
 *                     PRIOR to all other validation or function handling in case of any error(s).
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_GetAddrHostHandler(NET_IF_NBR       if_nbr,
                                       NET_IPv6_ADDR    *p_addr_tbl,
                                       NET_IP_ADDRS_QTY *p_addr_tbl_qty,
                                       RTOS_ERR         *p_err)
{
  NET_IF            *p_if;
  NET_IPv6_ADDR_OBJ *p_addr_obj;
  NET_IPv6_ADDR     *p_addr;
  NET_IP_ADDRS_QTY  addr_tbl_qty;
  NET_IP_ADDRS_QTY  addr_ix = 0u;
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN is_valid;
#endif
  CPU_BOOLEAN rtn_val = DEF_FAIL;

  //                                                               ---------------- VALIDATE ADDR TBL -----------------
  RTOS_ASSERT_DBG_ERR_SET((p_addr_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_addr_tbl_qty != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);

  addr_tbl_qty = *p_addr_tbl_qty;
  *p_addr_tbl_qty = 0u;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_valid = NetIF_IsValidHandler(if_nbr);
  if (is_valid != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }
#endif

  //                                                               ------------------ GET IPv6 ADDRS ------------------
  p_addr = p_addr_tbl;

  if (if_nbr == NET_IF_NBR_LOOPBACK) {                          // For loopback IF,                  ...
#ifdef NET_IF_LOOPBACK_MODULE_EN
    //                                                             ... get dflt IPv6 localhost addr; ...
    p_addr = (NET_IPv6_ADDR *)&NetIPv6_AddrAny;
    *p_addr_tbl_qty = 1u;
    rtn_val = DEF_OK;
    goto exit;
#else
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_FAIL);
#endif
  }

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (p_if->IP_Obj->IPv6_AddrListPtr == DEF_NULL) {
    goto exit;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv6_AddrListPtr, p_addr_obj, NET_IPv6_ADDR_OBJ, ListNode) {
    if (addr_ix >= addr_tbl_qty) {
      break;
    }

    *p_addr = p_addr_obj->AddrHost;
    addr_ix++;
    p_addr++;
  }

  *p_addr_tbl_qty = addr_ix;                                    // Rtn nbr of cfg'd addrs.

  rtn_val = DEF_OK;

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                       NetIPv6_GetAddrSrcHandler()
 *
 * @brief    Find the best matched source address for the given destination address.
 *
 * @param    p_if_nbr        Pointer to given interface number if any, variable that will received the
 *                           interface number if none is given.
 *
 * @param    p_addr_src      Pointer to IPv6 suggested source address if any.
 *
 * @param    p_addr_dest     Pointer to the destination address.
 *
 * @param    p_addr_nexthop  Pointer to Next Hop IPv6 address that the function will found.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to the IPv6 addresses structure associated with the best source address for the given
 *           destination.
 *
 * @note     (1) NetIPv6_GetAddrSrcHandler() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *
 * @note     (2) If a valid Interface number is given, the function will pull all the addresses
 *               configured on the Interface and found the best source address for the given destination
 *               address according to the Source Address Selection Rules define in RFC#6724.
 *
 * @note     (3) Else, if no Interface number is passed, the function will first call
 *               NetNDP_NextHop() to determine the Next-Hop and therefore the good
 *               Interface to Transmit given the destination address. Afterwards, the function will
 *               continue with the source selection as mentioned in note #1.
 *
 * @note     (4) If a suggested source address is passed, the function will check if it's a address
 *               configured on the outgoing Interface. If it is the case, the suggest source address
 *               will be return and the Source Selection Algorithm will be bypass.
 *******************************************************************************************************/
const NET_IPv6_ADDR_OBJ *NetIPv6_GetAddrSrcHandler(NET_IF_NBR          *p_if_nbr,
                                                   const NET_IPv6_ADDR *p_addr_src,
                                                   const NET_IPv6_ADDR *p_addr_dest,
                                                   NET_IPv6_ADDR       *p_addr_nexthop,
                                                   RTOS_ERR            *p_err)
{
  NET_IF_NBR if_nbr;
  NET_IF_NBR if_nbr_src_addr;
#ifdef  NET_NDP_MODULE_EN
  NET_IPv6_ADDR *p_next_hop;
#endif
  const NET_IPv6_ADDR_OBJ *p_addrs_src = DEF_NULL;
  CPU_BOOLEAN             valid;

  if_nbr = *p_if_nbr;

  //                                                               ----------------- VALIDATE IF NBR ------------------
  valid = NetIF_IsValidCfgdHandler(if_nbr);
  if (valid == DEF_YES) {                                       // A valid IF nbr is given, ...
                                                                // ... find address of Next-Hop.
    (void)NetNDP_NextHopByIF(if_nbr, p_addr_dest, &p_next_hop, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  } else {                                                      // No valid IF nbr is passed to function, ...
                                                                // ... find the outgoing IF and Next-Hop address.
    (void)NetNDP_NextHop(&if_nbr, p_addr_src, p_addr_dest, &p_next_hop, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  //                                                               Set the Next-Hop address to return.
  if (p_addr_nexthop != DEF_NULL) {
    p_addr_nexthop = (NET_IPv6_ADDR *)p_next_hop;
  }

  //                                                               Check if given source addr is configured on IF.
  if (p_addr_src != DEF_NULL) {
    if_nbr_src_addr = NetIPv6_GetAddrHostIF_Nbr(p_addr_src);

    if (if_nbr_src_addr == if_nbr) {                            // Return the given src addr if it's cfgd on the IF.
      p_addrs_src = NetIPv6_GetAddrsHostOnIF(if_nbr, p_addr_src);
      goto exit;
    }
  }

  //                                                               Find best matching source address.
  p_addrs_src = NetIPv6_AddrSrcSel(if_nbr,
                                   p_addr_dest,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (p_addrs_src);
}

/****************************************************************************************************//**
 *                                       NetIPv6_GetAddrHostIF_Nbr()
 *
 * @brief    Get the interface number for a configured IPv6 host address.
 *
 * @param    p_addr  Pointer to configured IPv6 host address to get the interface number (see Note #2).
 *
 * @return   Interface number of a configured  IPv6 host address, if available.
 *           NET_IF_NBR_NONE,                                     otherwise.
 *
 * @note     (1) NetIPv6_GetAddrHostIF_Nbr() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *
 * @note     (2) This function assumes that the TCP-IP stack does not permit two interfaces to setup
 *               the same address.
 *******************************************************************************************************/
NET_IF_NBR NetIPv6_GetAddrHostIF_Nbr(const NET_IPv6_ADDR *p_addr)
{
  CPU_BOOLEAN       addr_unspecified;
  NET_IPv6_ADDR_OBJ *p_ip_addrs;
  NET_IF_NBR        if_nbr;

  addr_unspecified = NetIPv6_IsAddrUnspecified(p_addr);
  if (addr_unspecified == DEF_YES) {
    return (NET_IF_NBR_NONE);
  }

  p_ip_addrs = NetIPv6_GetAddrsHost(p_addr, &if_nbr);
  if (p_ip_addrs == DEF_NULL) {
    return (NET_IF_NBR_NONE);
  }

  return (if_nbr);
}

/****************************************************************************************************//**
 *                                       NetIPv6_GetAddrHostMatchPrefix()
 *
 * @brief    Validate a prefix as being used by an address on the given interface.
 *
 * @param    if_nbr      Interface number of the interface to search on.
 *
 * @param    p_prefix    Pointer to IPv6 prefix.
 *
 * @param    prefix_len  Length of the prefix in bits.
 *
 * @return   Pointer to IPv6 Addresses object matching the prefix.
 *
 * @note     (1) NetIPv6_GetAddrHostMatchPrefix() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
NET_IPv6_ADDR_OBJ *NetIPv6_GetAddrHostMatchPrefix(NET_IF_NBR    if_nbr,
                                                  NET_IPv6_ADDR *p_prefix,
                                                  CPU_INT08U    prefix_len)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv6_ADDR_OBJ *p_addr_obj = DEF_NULL;
  NET_IPv6_ADDR     mask;
  CPU_BOOLEAN       valid;
  CPU_BOOLEAN       found;
  RTOS_ERR          local_err;

  //                                                               Set mask for given prefix.
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetIPv6_MaskGet(&mask, prefix_len, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  p_if = NetIF_Get(if_nbr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv6_AddrListPtr, p_addr_obj, NET_IPv6_ADDR_OBJ, ListNode) {
    valid = NetIPv6_IsAddrAndMaskValid((const NET_IPv6_ADDR *)&p_addr_obj->AddrHost,
                                       (const NET_IPv6_ADDR *) p_prefix);
    if (valid == DEF_YES) {
      if (p_addr_obj->AddrHostPrefixLen == prefix_len) {
        found = DEF_YES;
        break;
      }
    }
  }

  if (found != DEF_YES) {
    p_addr_obj = DEF_NULL;
  }

  PP_UNUSED_PARAM(p_prefix);

exit:
  return (p_addr_obj);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsAddrAndMaskValid()
 *
 * @brief    Validate that an IPv6 address and a mask are valid (match).
 *
 * @param    p_addr  Pointer to IPv6 address to validate.
 *
 * @param    p_mask  Pointer to IPv6 mask to be use.
 *
 * @return   DEF_OK,   if the address and the mask are matching.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrAndMaskValid(const NET_IPv6_ADDR *p_addr,
                                       const NET_IPv6_ADDR *p_mask)
{
  NET_IPv6_ADDR addr_masked;
  CPU_BOOLEAN   cmp;

  NetIPv6_AddrMask(p_addr, p_mask, &addr_masked);
  cmp = NetIPv6_IsAddrsIdentical(p_mask, &addr_masked);

  return (cmp);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsAddAndPrefixLenValid()
 *
 * @brief    Validate that an IPv6 address and the prefix length are valid (match).
 *
 * @param    p_addr      Pointer to IPv6 address to validate.
 *
 * @param    p_prefix    Pointer to IPv6 prefix.
 *
 * @param    prefix_len  Prefix length,
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 *
 * Return(s)   : DEF_OK,   if the address and the prefix mask are matching.
 *               DEF_FAIL, otherwise.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrAndPrefixLenValid(const NET_IPv6_ADDR *p_addr,
                                            const NET_IPv6_ADDR *p_prefix,
                                            CPU_INT08U          prefix_len,
                                            RTOS_ERR            *p_err)
{
  NET_IPv6_ADDR mask;
  NET_IPv6_ADDR addr_masked;
  CPU_BOOLEAN   cmp = DEF_FAIL;

  NetIPv6_MaskGet(&mask, prefix_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  NetIPv6_AddrMask(p_addr,
                   (const  NET_IPv6_ADDR *)&mask,
                   &addr_masked);

  cmp = NetIPv6_IsAddrsIdentical(p_prefix, &addr_masked);

exit:
  return (cmp);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsAddrMaskedValid()
 *
 * @brief    Validate that a masked address is matching the mask.
 *
 * @param    p_addr1     Pointer to IPv6 address mask.
 *
 * @param    p_addr2     Pointer to the masked IPv6 address.
 *
 * @return   DEF_OK,   if the address and the prefix mask are matching.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrsIdentical(const NET_IPv6_ADDR *p_addr1,
                                     const NET_IPv6_ADDR *p_addr2)
{
  CPU_INT08U  i;
  CPU_BOOLEAN cmp;

  for (i = 0u; i < NET_IPv6_ADDR_SIZE; i++) {
    if (p_addr1->Addr[i] != p_addr2->Addr[i]) {
      cmp = DEF_NO;
      goto exit;
    }
  }

  cmp = DEF_YES;

exit:
  return (cmp);
}

/****************************************************************************************************//**
 *                                       NetIPv6_GetAddrLinkLocalCfgd()
 *
 * @brief    Get a link-local IPv6 address configured on a specific interface.
 *
 * @param    if_nbr  Network interface number to search for the link-local address.
 *
 * @return   Pointer on the link-local address, if found.
 *           Pointer to NULL,                   otherwise.
 *
 * @note     (1) NetIPv6_GetAddrLinkLocalCfgd() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
NET_IPv6_ADDR *NetIPv6_GetAddrLinkLocalCfgd(NET_IF_NBR if_nbr)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv6_ADDR_OBJ *p_addr_obj = DEF_NULL;
  NET_IPv6_ADDR     *p_addr = DEF_NULL;
  CPU_BOOLEAN       addr_found = DEF_NO;
  RTOS_ERR          local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  p_if = NetIF_Get(if_nbr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               -------------- SRCH IF FOR IPv6 ADDR ---------------

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv6_AddrListPtr, p_addr_obj, NET_IPv6_ADDR_OBJ, ListNode) {
    addr_found = NetIPv6_IsAddrLinkLocal(&p_addr_obj->AddrHost);
    if (addr_found == DEF_YES) {
      p_addr = &p_addr_obj->AddrHost;
      break;
    }
  }

exit:
  return (p_addr);
}

/****************************************************************************************************//**
 *                                           NetIPv6_GetAddrsHost()
 *
 * @brief    Get interface number & IPv6 addresses object for configured IPv6 address.
 *
 * @param    p_addr      Pointer to configured IPv6 host address to get the interface number & IPv6
 *                       addresses structure (see Note #1).
 *
 * @param    p_if_nbr    Pointer to variable that will receive ... :
 *                           - (a) The interface number for this configured IPv6 address, if available;
 *                           - (b) NET_IF_NBR_NONE,                                       otherwise.
 *
 * @return   Pointer to corresponding IPv6 address structure, if IPv6 address configured on any interface.
 *           Pointer to NULL,                                 otherwise.
 *
 * @note     (1) NetIPv6_GetAddrsHost() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *
 * @note     (2) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
NET_IPv6_ADDR_OBJ *NetIPv6_GetAddrsHost(const NET_IPv6_ADDR *p_addr,
                                        NET_IF_NBR          *p_if_nbr)
{
  NET_IPv6_ADDR_OBJ *p_addr_obj = DEF_NULL;
  NET_IF_NBR        if_nbr = NET_IF_NBR_BASE_CFGD;
  CPU_BOOLEAN       addr_unspecified;

  if (p_if_nbr != DEF_NULL) {                                   // Init IF nbr for err (see Note #2).
    *p_if_nbr = NET_IF_NBR_NONE;
  }

  //                                                               ---------------- VALIDATE IPv6 ADDR ----------------
  addr_unspecified = NetIPv6_IsAddrUnspecified(p_addr);
  if (addr_unspecified == DEF_YES) {
    goto exit;
  }

  //                                                               -------- SRCH ALL CFG'D IF's FOR IPv6 ADDR ---------

  while ((if_nbr < Net_CoreDataPtr->IF_NbrCfgd)                 // Srch all cfg'd IF's ...
         && (p_addr_obj == DEF_NULL)) {                         // ... until addr found.
    p_addr_obj = NetIPv6_GetAddrsHostOnIF(if_nbr, p_addr);
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
 *                                       NetIPv6_GetAddrsHostOnIF()
 *
 * @brief    Get IPv6 address object for an interface's configured IPv6 address.
 *
 * @param    p_addr  Pointer to configured IPv6 host address to get the interface number & IPv6
 *                   addresses structure (see Note #1).
 *
 * @param    if_nbr  Interface number to search for configured IPv6 address.
 *
 * @return   Pointer to corresponding IP address structure, if IPv6 address configured on this interface.
 *           Pointer to NULL,                               otherwise.
 *
 * @note     (1) NetIPv6_GetAddrsHostOnIF() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
NET_IPv6_ADDR_OBJ *NetIPv6_GetAddrsHostOnIF(NET_IF_NBR          if_nbr,
                                            const NET_IPv6_ADDR *p_addr)
{
  NET_IF              *p_if;
  NET_IPv6_ADDR_OBJ   *p_addr_obj = DEF_NULL;
  const NET_IPv6_ADDR *p_mcast_solicited_addr;
  CPU_BOOLEAN         addr_found;
  CPU_BOOLEAN         addr_found_host;
  CPU_BOOLEAN         addr_found_mcast_solicited;
  RTOS_ERR            local_err;

  //                                                               -------------- VALIDATE IPv6 ADDR PTR --------------
  if (p_addr == DEF_NULL) {
    goto exit;
  }

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  p_if = NetIF_Get(if_nbr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               -------------- SRCH IF FOR IPv6 ADDR ---------------
  addr_found = DEF_NO;
  addr_found_host = DEF_NO;
  addr_found_mcast_solicited = DEF_NO;

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv6_AddrListPtr, p_addr_obj, NET_IPv6_ADDR_OBJ, ListNode) {
    p_mcast_solicited_addr = p_addr_obj->AddrMcastSolicitedPtr;

    addr_found_host = Mem_Cmp(p_addr, &p_addr_obj->AddrHost, NET_IPv6_ADDR_SIZE);
    if (addr_found_host == DEF_NO) {
      addr_found_mcast_solicited = Mem_Cmp(p_addr, p_mcast_solicited_addr, NET_IPv6_ADDR_SIZE);
    }

    addr_found = addr_found_host | addr_found_mcast_solicited;

    if (addr_found == DEF_YES) {                                // If addr NOT found, ...
      break;                                                    // ... adv to IF's next addr.
    }
  }

  if (addr_found != DEF_YES) {
    p_addr_obj = DEF_NULL;
    goto exit;
  }

exit:
  return (p_addr_obj);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsAddrHostCfgdHandler()
 *
 * @brief    Validate an IPv6 address as a configured IPv6 host address on an enabled interface.
 *
 * @param    p_addr  Pointer to IPv6 address to validate.
 *
 * @return   DEF_YES, if IPv6 address is one of the host's configured IPv6 addresses.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) NetIPv6_IsAddrHostCfgdHandler() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *               See also 'NetIPv6_IsAddrHostCfgd()  Note #1'.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrHostCfgdHandler(const NET_IPv6_ADDR *p_addr)
{
  NET_IF_NBR  if_nbr;
  CPU_BOOLEAN addr_host;

  if_nbr = NetIPv6_GetAddrHostIF_Nbr(p_addr);
  addr_host = (if_nbr != NET_IF_NBR_NONE) ? DEF_YES : DEF_NO;

  return (addr_host);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsAddrCfgdValidHandler()
 *
 * @brief    Check if IPv6 address configured is valid (CAN be used).
 *
 * @param    p_addr  Pointer to IPv6 address to validate.
 *
 * @return   DEF_YES, if IPv6 address is one of the host's configured IPv6 addresses.*
 *           DEF_NO,  otherwise.
 *
 * @note     (1) NetIPv6_IsAddrCfgdValidHandler() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrCfgdValidHandler(const NET_IPv6_ADDR *p_addr)
{
  NET_IPv6_ADDR_OBJ *p_addr_obj;
  NET_IF_NBR        if_nbr;
  CPU_BOOLEAN       is_valid = DEF_NO;

  p_addr_obj = NetIPv6_GetAddrsHost(p_addr, &if_nbr);
  if (p_addr_obj == DEF_NULL) {
    goto exit;
  }

  is_valid = p_addr_obj->IsValid;

exit:
  return (is_valid);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsAddrsCfgdOnIF_Handler()
 *
 * @brief    Check if any IPv6 address(s) configured on an interface.
 *
 * @param    if_nbr  Interface number to check for configured IPv6 address(s).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if any IPv6 host address(s) configured on interface.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) NetIPv6_IsAddrsCfgdOnIF_Handler() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *               See also 'NetIPv6_IsAddrsCfgdOnIF()  Note #1'.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrsCfgdOnIF_Handler(NET_IF_NBR if_nbr,
                                            RTOS_ERR   *p_err)
{
  NET_IF       *p_if = DEF_NULL;
  SLIST_MEMBER *p_node = DEF_NULL;
  CPU_BOOLEAN  addr_avail = DEF_NO;

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ------------ CHK CFG'D IPv6 ADDRS AVAIL ------------
  p_node = p_if->IP_Obj->IPv6_AddrListPtr;

  addr_avail = (p_node != DEF_NULL) ? DEF_YES : DEF_NO;

exit:
  return (addr_avail);
}

/****************************************************************************************************//**
 *                                           NetIPv6_AddrIsWildcard()
 *
 * @brief    Verify if an NET_IPv6_ADDR address is the IPv6 wildcard address or not.
 *
 * @param    p_addr  Pointer to a NET_IPv6_ADDR address.
 *
 * @return   DEF_YES, if the address is IPv6 wildcard.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrWildcard(NET_IPv6_ADDR *p_addr)
{
  CPU_INT08U  i;
  CPU_BOOLEAN is_wildcard;
  CPU_INT08U  *p_addr_in;
  CPU_INT08U  *p_addr_wildcard;

  is_wildcard = DEF_YES;
  p_addr_in = (CPU_INT08U *) p_addr;
  p_addr_wildcard = (CPU_INT08U *)&NetIPv6_AddrWildcard;

  for (i = 0; i < NET_IPv6_ADDR_SIZE; i++) {
    if (*p_addr_in != *p_addr_wildcard) {
      is_wildcard = DEF_NO;
      break;
    }
    p_addr_in++;
    p_addr_wildcard++;
  }

  return (is_wildcard);
}

/****************************************************************************************************//**
 *                                           NetIPv6_IsValidHopLim()
 *
 * @brief    Verify if an NET_IPv6_HOP_LIM value is valid or not.
 *
 * @param    hop_lim     The IPv6 hop limit to check the validity.
 *
 * @return   DEF_YES, if the IPv6 hop limit is valid.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) Hop limit has to be greater than or equal to 1 to be forwarded by a gateway.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsValidHopLim(NET_IPv6_HOP_LIM hop_lim)
{
  if (hop_lim < 1) {
    return (DEF_NO);
  }

  return (DEF_YES);
}

/****************************************************************************************************//**
 *                                           NetIPv6_AddrHW_McastSet()
 *
 * @brief    Set the multicast Hardware address.
 *
 * @param    p_addr_mac_ascii    Pointer to the Multicast MAC address to configured.
 *
 * @param    p_addr              _mac_ascii    Pointer to the Multicast MAC address to configured.
 *******************************************************************************************************/
void NetIPv6_AddrHW_McastSet(CPU_INT08U    *p_addr_mac_ascii,
                             NET_IPv6_ADDR *p_addr)
{
  CPU_BOOLEAN is_unspecified;

  RTOS_ASSERT_DBG((p_addr_mac_ascii != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  is_unspecified = NetIPv6_IsAddrUnspecified(p_addr);
  RTOS_ASSERT_DBG((is_unspecified != DEF_YES), RTOS_ERR_INVALID_ARG,; );

  p_addr_mac_ascii[0] = 0x33;
  p_addr_mac_ascii[1] = 0x33;

  Mem_Copy(&p_addr_mac_ascii[2],
           &p_addr->Addr[12],
           4u);
}

/****************************************************************************************************//**
 *                                       NetIPv6_SetAddrSolicitedMcast()
 *
 * @brief    Set the solicited node multicast address for a given address.
 *
 * @param    p_addr_result   Pointer to the solicited node address to create
 *
 * @param    p_addr_input    Pointer to the IPv6 address associated with the solicited node address.
 *
 * @param    if_nbr          Interface number to get the HW address if necessary.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) The solicited node multicast address for a specific IPv6 address is formed with the
 *               prefix FF02:0:0:0:0:1:FF00::/104 and the last 3 bytes of the given IPv6 address.
 *
 * @note     (2) If not address is passed as argument, the hardware address of the given Interface is
 *               used to formed the Solicited Node Multicast address.
 *******************************************************************************************************/
void NetIPv6_AddrMcastSolicitedSet(NET_IPv6_ADDR *p_addr_result,
                                   NET_IPv6_ADDR *p_addr_input,
                                   NET_IF_NBR    if_nbr,
                                   RTOS_ERR      *p_err)
{
  CPU_INT08U  addr_hw[NET_IF_HW_ADDR_LEN_MAX];
  CPU_INT08U  addr_hw_len;
  CPU_BOOLEAN is_cfgd;

  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit;
  }

  p_addr_result->Addr[0] = 0xFF;
  p_addr_result->Addr[1] = 0x02;
  p_addr_result->Addr[2] = DEF_BIT_NONE;
  p_addr_result->Addr[3] = DEF_BIT_NONE;
  p_addr_result->Addr[4] = DEF_BIT_NONE;
  p_addr_result->Addr[5] = DEF_BIT_NONE;
  p_addr_result->Addr[6] = DEF_BIT_NONE;
  p_addr_result->Addr[7] = DEF_BIT_NONE;
  p_addr_result->Addr[8] = DEF_BIT_NONE;
  p_addr_result->Addr[9] = DEF_BIT_NONE;
  p_addr_result->Addr[10] = DEF_BIT_NONE;
  p_addr_result->Addr[11] = 0x01;
  p_addr_result->Addr[12] = 0xFF;

  if (p_addr_input == DEF_NULL) {                               // See Note #2.
    addr_hw_len = NET_IF_HW_ADDR_LEN_MAX;

    NetIF_AddrHW_GetHandler(if_nbr, &addr_hw[0], &addr_hw_len, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    Mem_Copy(&p_addr_result->Addr[13],
             &addr_hw[3],
             3u);

    goto exit;
  }

  Mem_Copy(&p_addr_result->Addr[13],                            // See Note #1.
           &p_addr_input->Addr[13],
           3u);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv6_AddrMcastAllRoutersSet()
 *
 * @brief    Set the multicast all-routers IPv6 address.
 *
 * @param    p_addr      Pointer to the IPv6 Multicast All-Routers address to configured.
 *
 * @param    mldp_v2     DEF_YES, Multicast Listiner Discovery Protocol version 2 is used
 *                       DEF_NO,  Multicast Listiner Discovery Protocol version 1 is used
 *
 * @note     (1) The IPv6 Mulitcast All-Routers address is defined as FF02:0:0:0:0:0:0:2 for the link-local
 *               scope.
 *
 * @note     (2) For the MLDv2 protocol, the address FF02:0:0:0:0:0:0:16 is used as the destination
 *               address for sending MDL report messages.
 *******************************************************************************************************/
void NetIPv6_AddrMcastAllRoutersSet(NET_IPv6_ADDR *p_addr,
                                    CPU_BOOLEAN   mldp_v2)
{
  //                                                               --------------------- SET ADDR ---------------------
  p_addr->Addr[0] = 0xFF;
  p_addr->Addr[1] = 0x02;
  p_addr->Addr[2] = DEF_BIT_NONE;
  p_addr->Addr[3] = DEF_BIT_NONE;
  p_addr->Addr[4] = DEF_BIT_NONE;
  p_addr->Addr[5] = DEF_BIT_NONE;
  p_addr->Addr[6] = DEF_BIT_NONE;
  p_addr->Addr[7] = DEF_BIT_NONE;
  p_addr->Addr[8] = DEF_BIT_NONE;
  p_addr->Addr[9] = DEF_BIT_NONE;
  p_addr->Addr[10] = DEF_BIT_NONE;
  p_addr->Addr[11] = DEF_BIT_NONE;
  p_addr->Addr[12] = DEF_BIT_NONE;
  p_addr->Addr[13] = DEF_BIT_NONE;
  p_addr->Addr[14] = DEF_BIT_NONE;

  if (mldp_v2 == DEF_NO) {
    p_addr->Addr[15] = 0x02;
  } else {
    p_addr->Addr[15] = 0x16;
  }
}

/****************************************************************************************************//**
 *                                       NetIPv6_AddrMcastAllNodesSet()
 *
 * @brief    Set the multicast all-nodes IPv6 address.
 *
 * @param    p_addr  Pointer to the IPv6 Multicast All-Nodes address to configured.
 *
 * @note     (1) The IPv6 Multicast All Nodes address is defined as FF02:0:0:0:0:0:0:0:1
 *******************************************************************************************************/
void NetIPv6_AddrMcastAllNodesSet(NET_IPv6_ADDR *p_addr)
{
  p_addr->Addr[0] = 0xFF;
  p_addr->Addr[1] = 0x02;
  p_addr->Addr[2] = DEF_BIT_NONE;
  p_addr->Addr[3] = DEF_BIT_NONE;
  p_addr->Addr[4] = DEF_BIT_NONE;
  p_addr->Addr[5] = DEF_BIT_NONE;
  p_addr->Addr[6] = DEF_BIT_NONE;
  p_addr->Addr[7] = DEF_BIT_NONE;
  p_addr->Addr[8] = DEF_BIT_NONE;
  p_addr->Addr[9] = DEF_BIT_NONE;
  p_addr->Addr[10] = DEF_BIT_NONE;
  p_addr->Addr[11] = DEF_BIT_NONE;
  p_addr->Addr[12] = DEF_BIT_NONE;
  p_addr->Addr[13] = DEF_BIT_NONE;
  p_addr->Addr[14] = DEF_BIT_NONE;
  p_addr->Addr[15] = 0x01;
}

/****************************************************************************************************//**
 *                                           NetIPv6_AddrLoopbackSet()
 *
 * @brief    Set the loopback IPv6 address.
 *
 * @param    p_addr  Pointer to the IPv6 Loopback address to configured.
 *
 * @note     (1) The IPv6 loopback address is defined as 0:0:0:0:0:0:0:1.
 *******************************************************************************************************/
void NetIPv6_AddrLoopbackSet(NET_IPv6_ADDR *p_addr)
{
  p_addr->Addr[0] = DEF_BIT_NONE;
  p_addr->Addr[1] = DEF_BIT_NONE;
  p_addr->Addr[2] = DEF_BIT_NONE;
  p_addr->Addr[3] = DEF_BIT_NONE;
  p_addr->Addr[4] = DEF_BIT_NONE;
  p_addr->Addr[5] = DEF_BIT_NONE;
  p_addr->Addr[6] = DEF_BIT_NONE;
  p_addr->Addr[7] = DEF_BIT_NONE;
  p_addr->Addr[8] = DEF_BIT_NONE;
  p_addr->Addr[9] = DEF_BIT_NONE;
  p_addr->Addr[10] = DEF_BIT_NONE;
  p_addr->Addr[11] = DEF_BIT_NONE;
  p_addr->Addr[12] = DEF_BIT_NONE;
  p_addr->Addr[13] = DEF_BIT_NONE;
  p_addr->Addr[14] = DEF_BIT_NONE;
  p_addr->Addr[15] = 0x01;
}

/****************************************************************************************************//**
 *                                       NetIPv6_AddrUnspecifiedSet()
 *
 * @brief    Set the unspecified IPv6 address.
 *
 * @param    p_addr  Pointer to the IPv6 Unspecified address to configured.
 *
 * @note     (1) The IPv6 Unspecified address is defined as 0:0:0:0:0:0:0:0.
 *******************************************************************************************************/
void NetIPv6_AddrUnspecifiedSet(NET_IPv6_ADDR *p_addr)
{
  p_addr->Addr[0] = DEF_BIT_NONE;
  p_addr->Addr[1] = DEF_BIT_NONE;
  p_addr->Addr[2] = DEF_BIT_NONE;
  p_addr->Addr[3] = DEF_BIT_NONE;
  p_addr->Addr[4] = DEF_BIT_NONE;
  p_addr->Addr[5] = DEF_BIT_NONE;
  p_addr->Addr[6] = DEF_BIT_NONE;
  p_addr->Addr[7] = DEF_BIT_NONE;
  p_addr->Addr[8] = DEF_BIT_NONE;
  p_addr->Addr[9] = DEF_BIT_NONE;
  p_addr->Addr[10] = DEF_BIT_NONE;
  p_addr->Addr[11] = DEF_BIT_NONE;
  p_addr->Addr[12] = DEF_BIT_NONE;
  p_addr->Addr[13] = DEF_BIT_NONE;
  p_addr->Addr[14] = DEF_BIT_NONE;
  p_addr->Addr[15] = DEF_BIT_NONE;
}

/****************************************************************************************************//**
 *                                               NetIPv6_Rx()
 *
 * @brief    (1) Process received datagrams & forward to network protocol layers :
 *               - (a) Validate IPv6 packet & options
 *               - (b) Reassemble fragmented datagrams
 *               - (c) Demultiplex datagram to higher-layer protocols
 *               - (d) Update receive statistics
 *
 * @param    p_buf   Pointer to network buffer that received IPv6 packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Since NetIPv6_RxPktFragReasm() may return a pointer to a different packet buffer (see
 *               'NetIPv6_RxPktFragReasm()  Return(s)', 'p_buf_hdr' MUST be reloaded.
 *
 * @note     (3) For single packet buffer IPv6 datagrams, the datagram length is equal to the IPv6
 *               Total Length minus the IPv6 Header Length.
 *
 * @note     (4) For multiple packet buffer, fragmented IPv6 datagrams, the datagram length is
 *               equal to the previously calculated total fragment size.
 *               - (a) IP datagram length is stored ONLY in the first packet buffer of any
 *                     fragmented packet buffers.
 *
 * @note     (5) Network buffer already freed by higher layer; only increment error counter.
 *******************************************************************************************************/
void NetIPv6_Rx(NET_BUF  *p_buf,
                RTOS_ERR *p_err)
{
  NET_BUF      *p_frag_list;
  NET_BUF_HDR  *p_buf_hdr;
  NET_IPv6_HDR *p_ip_hdr;
  CPU_BOOLEAN  is_frag;

  NET_CTR_STAT_INC(Net_StatCtrs.IPv6.RxPktCtr);

  p_buf_hdr = &p_buf->Hdr;

  p_ip_hdr = (NET_IPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];

  NetIPv6_RxPktValidate(p_buf, p_buf_hdr, p_ip_hdr, p_err);     // Validate rx'd pkt.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               ----------------- PROCESS EXT HDR ------------------
  NetIPv6_RxPktProcessExtHdr(p_buf, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               ------------------- REASM FRAGS --------------------
  p_frag_list = NetIPv6_RxPktFragReasm(p_buf, p_buf_hdr, p_ip_hdr, &is_frag, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               ------------------ DEMUX DATAGRAM ------------------
  if (p_frag_list != DEF_NULL) {
    p_buf_hdr = &p_frag_list->Hdr;

    if (is_frag == DEF_NO) {                                    // If pkt NOT frag'd, calc buf datagram len.
      p_buf_hdr->IP_DatagramLen = (p_buf_hdr->IP_TotLen - p_buf_hdr->IPv6_ExtHdrLen);
    } else {                                                    // Else set tot frag size as datagram len.
      p_buf_hdr->IP_DatagramLen = p_buf_hdr->IP_FragSizeTot;
    }

    NetIPv6_RxPktDemuxDatagram(p_frag_list, p_buf_hdr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit_discard;
    }
  }

  NET_CTR_STAT_INC(Net_StatCtrs.IPv6.RxDgramCompCtr);

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxPktDisCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIPv6_Tx()
 *
 * @brief    (1) Prepare & transmit IPv6 datagram packet(s) :
 *               - (a) Validate  transmit packet
 *               - (b) Prepare & transmit packet datagram
 *               - (c) Update    transmit statistics
 *
 * @param    p_buf           Pointer to network buffer to transmit IPv6 packet.
 *
 * @param    p_addr_src      Pointer to source        IPv6 address.
 *
 * @param    p_addr_dest     Pointer to destination   IPv6 address.
 *
 * @param    traffic_class   Specific traffic class to transmit IPv6 packet (see Note #2a).
 *
 * @param    flow_label      Specific flow label    to transmit IPv6 packet (see Note #x).
 *
 * @param    next_hdr        Next header            to transmit IPv6 packet (see Note #x).
 *
 * @param    hop_limit       Specific hop limit to transmit IPv6 packet.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (2) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
void NetIPv6_Tx(NET_BUF                *p_buf,
                NET_IPv6_ADDR          *p_addr_src,
                NET_IPv6_ADDR          *p_addr_dest,
                NET_IPv6_EXT_HDR       *p_ext_hdr_list,
                NET_IPv6_TRAFFIC_CLASS traffic_class,
                NET_IPv6_FLOW_LABEL    flow_label,
                NET_IPv6_HOP_LIM       hop_lim,
                RTOS_ERR               *p_err)
{
  NET_BUF_HDR *p_buf_hdr;

  p_buf_hdr = &p_buf->Hdr;

  //                                                               --------------- VALIDATE IPv6 TX PKT ---------------
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  NetIPv6_TxPktValidate(p_buf_hdr,
                        p_addr_src,
                        p_addr_dest,
                        traffic_class,
                        flow_label,
                        hop_lim,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }
#endif

  //                                                               ------------------- TX IPv6 PKT --------------------
  NetIPv6_TxPkt(p_buf,
                p_buf_hdr,
                p_addr_src,
                p_addr_dest,
                p_ext_hdr_list,
                traffic_class,
                flow_label,
                hop_lim,
                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.TxPktDisCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIPv6_ReTx()
 *
 * @brief    (1) Prepare & re-transmit packets from transport protocol layers to network interface layer :
 *               - (a) Validate  re-transmit packet
 *               - (b) Prepare & re-transmit packet datagram
 *               - (c) Update    re-transmit statistics
 *
 * @param    p_buf   Pointer to network buffer to re-transmit IPv6 packet.
 *
 *
 * Argument(s) : p_buf       Pointer to network buffer to re-transmit IPv6 packet.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) Network buffer already freed by lower layer; only increment error counter.
 *******************************************************************************************************/
void NetIPv6_ReTx(NET_BUF  *p_buf,
                  RTOS_ERR *p_err)
{
  NET_BUF_HDR *p_buf_hdr;

  p_buf_hdr = &p_buf->Hdr;

  //                                                               ------------------ RE-TX IPv6 PKT ------------------
  NetIPv6_ReTxPkt(p_buf,
                  p_buf_hdr,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.TxPktDisCtr);
    goto exit;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.IPv6.TxDgramCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIPv6_GetTxDataIx()
 *
 * @brief    Get the offset of a buffer at which the IPv6 data CAN be written.
 *
 * @param    if_nbr          Network interface number to transmit data.
 *
 * @param    p_ext_hdr_list  Pointer to IPv6 extension headers list.
 *
 * @param    data_len        IPv6 payload size.
 *
 * @param    mtu             MTU for the upper-layer protocol.
 *
 * @param    p_ix            Pointer to the current protocol index.
 *******************************************************************************************************/
void NetIPv6_GetTxDataIx(NET_IF_NBR       if_nbr,
                         NET_IPv6_EXT_HDR *p_ext_hdr_list,
                         CPU_INT32U       data_len,
                         CPU_INT16U       mtu,
                         CPU_INT16U       *p_ix)
{
  NET_IPv6_EXT_HDR *p_ext_hdr;

  //                                                               Add IPv6 min hdr len to current offset.
  *p_ix += NET_IPv6_HDR_SIZE;

#if 0
  if (data_len > mtu) {
    *p_ix += NET_IPv6_FRAG_HDR_SIZE;                            // Add IPv6 frag ext   hdr len if frag is req'd.
  }
#endif
  //                                                               Add Size of existing extension headers.
  p_ext_hdr = p_ext_hdr_list;
  while (p_ext_hdr != DEF_NULL) {
    *p_ix += p_ext_hdr->Len;
    p_ext_hdr = p_ext_hdr->NextHdrPtr;
  }

  //                                                               Add the lower-level hdr        offsets.
  NetIF_TxIxDataGet(if_nbr, data_len, p_ix);

  PP_UNUSED_PARAM(mtu);
}

/****************************************************************************************************//**
 *                                           NetIPv6_AddExtHdrToList()
 *
 * @brief    Add an Extension Header object into its right place in the extension headers List.
 *
 * @param    p_ext_hdr_head  Pointer on the current top of the extension header list.
 *
 * @param    p_ext_hdr       _head  Pointer on the current top of the extension header list.
 *
 * @param    type            Type of the extension header.
 *
 * @param    len             Length of the extension header.
 *
 * @param    fnct            Pointer to callback function that will be call to fill the ext hdr.
 *
 * @param    sort_key        Key to sort the extension header.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to the new extension header list head.
 *******************************************************************************************************/
NET_IPv6_EXT_HDR *NetIPv6_ExtHdrAddToList(NET_IPv6_EXT_HDR *p_ext_hdr_head,
                                          NET_IPv6_EXT_HDR *p_ext_hdr,
                                          CPU_INT08U       type,
                                          CPU_INT16U       len,
                                          CPU_FNCT_PTR     fnct,
                                          CPU_INT08U       sort_key,
                                          RTOS_ERR         *p_err)
{
  NET_IPv6_EXT_HDR *p_list_head = DEF_NULL;
  NET_IPv6_EXT_HDR *p_ext_hdr_prev;
  NET_IPv6_EXT_HDR *p_ext_hdr_next;
  NET_IPv6_EXT_HDR *p_ext_hdr_item;
  NET_IPv6_EXT_HDR *p_ext_hdr_cache = DEF_NULL;

  //                                                               -------------- VALIDATE EXT HDR TYPE ---------------
  switch (sort_key) {
    case NET_IPv6_EXT_HDR_KEY_HOP_BY_HOP:
    case NET_IPv6_EXT_HDR_KEY_DEST_01:
    case NET_IPv6_EXT_HDR_KEY_ROUTING:
    case NET_IPv6_EXT_HDR_KEY_FRAG:
      break;

    case NET_IPv6_EXT_HDR_KEY_AUTH:
    case NET_IPv6_EXT_HDR_KEY_ESP:
    case NET_IPv6_EXT_HDR_KEY_DEST_02:
      p_list_head = p_ext_hdr_head;
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.ExtHdrNotSupported);
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      goto exit;
  }

  //                                                               --------------- SET EXT HEADER VALUES --------------
  p_ext_hdr->Type = type;
  p_ext_hdr->Len = len;
  p_ext_hdr->SortKey = sort_key;
  p_ext_hdr->Fnct = (void *)fnct;
  p_ext_hdr->Arg = DEF_NULL;

  //                                                               ----------- ADD EXT HDR TO LIST IN ORDER -----------
  if (p_ext_hdr_head == DEF_NULL) {                             // List is empty, ...
    p_ext_hdr->NextHdrPtr = DEF_NULL;                           // ... add ext hdr at top of list.
    p_ext_hdr->PrevHdrPtr = DEF_NULL;
    p_list_head = p_ext_hdr;
  } else {
    p_ext_hdr_item = p_ext_hdr_head;
    while (p_ext_hdr_item != DEF_NULL) {                        // Goto the list to find the right place for the hdr:
      if (p_ext_hdr_item->SortKey > sort_key) {
        break;
      } else if (p_ext_hdr_item->SortKey == sort_key) {         // Ext hdr type is already in list...
        p_list_head = p_ext_hdr_head;
        RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);              // ... return with error.
        goto exit;
      }

      p_ext_hdr_cache = p_ext_hdr_item;
      p_ext_hdr_item = p_ext_hdr_item->NextHdrPtr;
    }

    if (p_ext_hdr_item == DEF_NULL) {                           // Add ext hdr blk to end of list.
      p_ext_hdr_item = p_ext_hdr_cache;
      p_ext_hdr_item->NextHdrPtr = p_ext_hdr;
      p_ext_hdr->PrevHdrPtr = p_ext_hdr_item;
      p_ext_hdr->NextHdrPtr = DEF_NULL;
      p_list_head = p_ext_hdr_head;
    } else {                                                    // Add ext hdr blk inside the list.
      p_ext_hdr_prev = p_ext_hdr_item->PrevHdrPtr;
      p_ext_hdr_next = p_ext_hdr_item;

      if (p_ext_hdr_prev != DEF_NULL) {
        p_ext_hdr_prev->NextHdrPtr = p_ext_hdr;
        p_list_head = p_ext_hdr_head;
      } else {
        p_list_head = p_ext_hdr;                                // Set new list head if hdr blk is added at the top.
      }
      p_ext_hdr->PrevHdrPtr = p_ext_hdr_prev;
      p_ext_hdr->NextHdrPtr = p_ext_hdr_next;
      p_ext_hdr_next->PrevHdrPtr = p_ext_hdr;
    }
  }

exit:
  return (p_list_head);
}

/****************************************************************************************************//**
 *                                           NetIPv6_PrepareFragHdr()
 *
 * @brief    Callback function to fill up the fragment header inside the packet to be send.
 *
 * @param    p_ext_hdr_arg   Pointer to the function arguments.
 *******************************************************************************************************/
void NetIPv6_PrepareFragHdr(void *p_ext_hdr_arg)
{
  NET_IPv6_FRAG_HDR         *p_frag_hdr;
  NET_BUF                   *p_buf;
  NET_IPv6_EXT_HDR_ARG_FRAG *p_frag_hdr_arg;
  CPU_INT16U                frag_hdr_ix;

  p_frag_hdr_arg = (NET_IPv6_EXT_HDR_ARG_FRAG *) p_ext_hdr_arg;

  p_buf = p_frag_hdr_arg->BufPtr;
  frag_hdr_ix = p_frag_hdr_arg->BufIx;

  p_frag_hdr = (NET_IPv6_FRAG_HDR *)&p_buf->DataPtr[frag_hdr_ix];

  Mem_Clr(p_frag_hdr, NET_IPv6_FRAG_HDR_SIZE);

  p_frag_hdr->NextHdr = p_frag_hdr_arg->NextHdr;
  p_frag_hdr->FragOffsetFlag = p_frag_hdr_arg->FragOffset;
  p_frag_hdr->ID = p_frag_hdr_arg->FragID;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   NETWORK MANAGER INTERFACE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetIPv6_GetHostAddrProtocol()
 *
 * @brief    Get an interface's IPv6 protocol address(s) [see Note #1].
 *
 * @param    if_nbr                      Interface number to get IPv6 protocol address(s).
 *
 * @param    p_addr_protocol_tbl         Pointer to a protocol address table that will receive the protocol
 *                                       address(s) in network-order for this interface.
 *
 * @param    p_addr_protocol_tbl_qty     Pointer to a variable to ... :
 *                                           - (a) Pass the size of the protocol address table, in number of
 *                                                 protocol address(s), pointed to by 'p_addr_protocol_tbl'.
 *                                           - (b) Either:
 *                                               - (1) Return the actual number of IPv6 protocol address(s),
 *                                                     if NO error(s);
 *                                               - (2) Return 0, otherwise.
 *                                                     See also Note #1a.
 *
 * @param    p_addr_protocol_len         Pointer to a variable to ... :
 *                                          - (a) Pass the length of the protocol address table address(s),
 *                                                in octets.
 *                                          - (b) Either:
 *                                              - (1) Return the actual length of IPv6 protocol address(s),
 *                                                    in octets, if NO error(s);
 *                                              - (2) Return 0, otherwise.
 *                                                    See also Note #1b.
 *
 * @param    p_err                       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) p_addr_protocol_tbl_qty
 *               - (a) Since 'p_addr_protocol_tbl_qty' argument is both an input & output argument
 *                     (see 'Argument(s) : p_addr_protocol_tbl_qty'), ... :
 *                   - (1) Its input value SHOULD be validated prior to use; ...
 *                       - (A) In the case that the 'p_addr_protocol_tbl_qty' argument is passed a null
 *                             pointer, NO input value is validated or used.
 *                       - (B) The protocol address table's size MUST be greater than or equal to each
 *                             interface's maximum number of IPv6 protocol addresses times the size of
 *                             an IPv6 protocol address :
 *                   - (1) (p_addr_protocol_tbl_qty *  >=  [ NET_IPv6_CFG_IF_MAX_NBR_ADDR *
 *                         p_addr_protocol_len    )         NET_IPv6_ADDR_SIZE      ]
 *                   - (2) While its output value MUST be initially configured to return a default value
 *                         PRIOR to all other validation or function handling in case of any error(s).
 *               - (b) Since 'p_addr_protocol_len' argument is both an input & output argument
 *                     (see 'Argument(s) : p_addr_protocol_len'), ... :
 *                   - (1) Its input value SHOULD be validated prior to use; ...
 *                       - (A) In the case that the 'p_addr_protocol_len' argument is passed a null pointer,
 *                             NO input value is validated or used.
 *                       - (B) The table's protocol address(s) length SHOULD be greater than or equal to the
 *                             size of an IPv6 protocol address :
 *                           - (1) p_addr_protocol_len  >=  NET_IPv6_ADDR_SIZE
 *                           - (2) While its output value MUST be initially configured to return a default value
 *                                 PRIOR to all other validation or function handling in case of any error(s).
 *******************************************************************************************************/
#if 0
void NetIPv6_GetHostAddrProtocol(NET_IF_NBR if_nbr,
                                 CPU_INT08U *p_addr_protocol_tbl,
                                 CPU_INT08U *p_addr_protocol_tbl_qty,
                                 CPU_INT08U *p_addr_protocol_len,
                                 RTOS_ERR   *p_err)
{
  CPU_INT08U       *p_addr_protocol;
  NET_IPv6_ADDR    *p_addr_ip;
  NET_IPv6_ADDR    addr_ip_tbl[NET_IPv6_CFG_IF_MAX_NBR_ADDR];
  NET_IP_ADDRS_QTY addr_ip_tbl_qty;
  NET_IP_ADDRS_QTY addr_ix;
  CPU_INT08U       addr_ip_len;
  CPU_BOOLEAN      result;

  RTOS_ASSERT_DBG_ERR_SET((p_addr_protocol_tbl_qty != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_protocol_len != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_protocol_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  *p_addr_protocol_tbl_qty = 0u;                                    // Cfg dflt tbl qty for err  (see Note #2a2).

  *p_addr_protocol_len = 0u;                                        // Cfg dflt addr len for err (see Note #2b2).
  addr_ip_len = NET_IPv6_ADDR_SIZE;

  //                                                               ----------- GET IPv6 PROTOCOL ADDRS ------------
  addr_ip_tbl_qty = sizeof(addr_ip_tbl) / NET_IPv6_ADDR_SIZE;
  result = NetIPv6_GetAddrHostHandler(if_nbr,
                                      &addr_ip_tbl[0],
                                      &addr_ip_tbl_qty,
                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (result == DEF_FAIL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  addr_ix = 0u;
  p_addr_ip = &addr_ip_tbl[addr_ix];
  p_addr_protocol = &p_addr_protocol_tbl[addr_ix];
  while (addr_ix < addr_ip_tbl_qty) {                               // Rtn all IPv6 protocol addr(s) ...
    NET_UTIL_VAL_COPY_SET_NET_32(p_addr_protocol, p_addr_ip);       // ... in net-order (see Note #1).
    p_addr_protocol += addr_ip_len;
    p_addr_ip++;
    addr_ix++;
  }

  //                                                               Rtn nbr & len of IPv6 protocol addr(s).
  *p_addr_protocol_tbl_qty = (CPU_INT08U)addr_ip_tbl_qty;
  *p_addr_protocol_len = (CPU_INT08U)addr_ip_len;

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv6_GetAddrProtocolIF_Nbr()
 *
 * @brief    (1) Get the interface number for a host's IPv6 protocol address :
 *               - (a) A configured IPv6 host address (on an enabled interface)
 *
 * @param    p_addr_protocol     Pointer to protocol address (see Note #2).
 *
 * @param    addr_protocol_len   Length  of protocol address (in octets).
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @return   Interface number for IPv6 protocol address, if configured on this host.
 *           Interface number of  IPv6 protocol address
 *           in address initialization,              if available.
 *           NET_IF_NBR_LOCAL_HOST,                      for a localhost address.
 *           NET_IF_NBR_NONE,                            otherwise.
 *******************************************************************************************************/
NET_IF_NBR NetIPv6_GetAddrProtocolIF_Nbr(CPU_INT08U *p_addr_protocol,
                                         CPU_INT08U addr_protocol_len,
                                         RTOS_ERR   *p_err)
{
  NET_IPv6_ADDR addr_ip;
  NET_IF_NBR    if_nbr = NET_IF_NBR_NONE;

  PP_UNUSED_PARAM(addr_protocol_len);

  //                                                               ------- GET IPv6 PROTOCOL ADDR's IF NBR --------
  NET_UTIL_VAL_COPY_GET_NET_32(&addr_ip, p_addr_protocol);
  if_nbr = NetIPv6_GetAddrHostIF_Nbr(&addr_ip);

  if (if_nbr == NET_IF_NBR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

exit:
  return (if_nbr);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsValidAddrProtocol()
 *
 * @brief    Validate an IPv6 protocol address.
 *
 * @param    p_addr_protocol     Pointer to protocol address (see Note #1).
 *
 * @param    addr_protocol_len   Length  of protocol address (in octets).
 *
 * @return   DEF_YES, if IPv6 protocol address valid.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) IPv6 protocol address MUST be in network-order.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsValidAddrProtocol(CPU_INT08U *p_addr_protocol,
                                        CPU_INT08U addr_protocol_len)
{
  NET_IPv6_ADDR addr_ip = NET_IPv6_ADDR_NONE;
  CPU_BOOLEAN   valid;

  PP_UNUSED_PARAM(addr_protocol_len);

  //                                                               --------- VALIDATE IPv6 PROTOCOL ADDR ----------
  NET_UTIL_VAL_COPY_GET_NET_32(&addr_ip, p_addr_protocol);
  valid = NetIPv6_IsValidAddrHost(&addr_ip);

  return (valid);
}

/****************************************************************************************************//**
 *                                           NetIPv6_IsAddrInit()
 *
 * @brief    Validate an IPv6 protocol address as the initialization address.
 *
 * @param    p_addr_protocol     Pointer to protocol address (see Note #1).
 *
 * @param    addr_protocol_len   Length  of protocol address (in octets).
 *
 * @return   DEF_YES, if IPv6 protocol address is the initialization address.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIPv6_IsAddrInit(CPU_INT08U *p_addr_protocol,
                               CPU_INT08U addr_protocol_len)
{
  CPU_BOOLEAN addr_init;

  PP_UNUSED_PARAM(addr_protocol_len);

  //                                                               ------- VALIDATE IPv6 PROTOCOL INIT ADDR -------
  addr_init = NetIPv6_IsAddrUnspecified((NET_IPv6_ADDR *)p_addr_protocol);

  return (addr_init);
}

/****************************************************************************************************//**
 *                                       NetIPv6_IsAddrProtocolMulticast()
 *
 * @brief    Validate an IPv6 protocol address as a multicast address.
 *
 * @param    p_addr_protocol     Pointer to protocol address (see Note #1).
 *
 * @param    addr_protocol_len   Length  of protocol address (in octets).
 *
 * @return   DEF_YES, if IPv6 protocol address is a multicast address.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/

#ifdef  NET_MCAST_MODULE_EN
CPU_BOOLEAN NetIPv6_IsAddrProtocolMulticast(CPU_INT08U *p_addr_protocol,
                                            CPU_INT08U addr_protocol_len)
{
  NET_IPv6_ADDR addr_ip;
  CPU_BOOLEAN   addr_multicast;

  PP_UNUSED_PARAM(addr_protocol_len);

  //                                                               ------- VALIDATE IPv6 PROTOCOL INIT ADDR -------
  Mem_Copy(&addr_ip,
           p_addr_protocol,
           NET_IPv6_ADDR_SIZE);
  addr_multicast = NetIPv6_IsAddrMcast(&addr_ip);

  return (addr_multicast);
}
#endif

/****************************************************************************************************//**
 *                                   NetIPv6_AddrValidLifetimeTimeout()
 *
 * @brief    Remove IPv6 address from address list when valid lifetime comes to an end.
 *
 * @param    p_ipv6_addr_timeout     Pointer to ipv6 addrs object that has timed out.
 *******************************************************************************************************/
void NetIPv6_AddrValidLifetimeTimeout(void *p_ipv6_addr_timeout)
{
  NET_IF            *p_if = DEF_NULL;
  NET_IPv6_ADDR_OBJ *p_addr_obj = DEF_NULL;
  NET_IPv6_ADDR     *p_solicit_node_addr;
  RTOS_ERR          local_err;

  p_addr_obj = (NET_IPv6_ADDR_OBJ *)p_ipv6_addr_timeout;

  p_if = NetIF_Get(p_addr_obj->IfNbr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return;
  }

  p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_NONE;
  p_addr_obj->IsValid = DEF_NO;
  p_addr_obj->ValidLifetimeTmrPtr = DEF_NULL;

  NetConn_CloseAllConnsByAddrHandler((CPU_INT08U *)&p_addr_obj->AddrHost, NET_IPv6_ADDR_SIZE);

  Mem_Clr(&p_addr_obj->AddrHost, NET_IPv6_ADDR_SIZE);

#ifdef  NET_MLDP_MODULE_EN
  //                                                               LEAVE MLDP GROUP OF THE MCAST SOLICITED ADDR
  p_solicit_node_addr = p_addr_obj->AddrMcastSolicitedPtr;
  if (p_solicit_node_addr != DEF_NULL) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetMLDP_HostGrpLeaveHandler(p_addr_obj->IfNbr,
                                p_solicit_node_addr,
                                &local_err);
  }
#endif

  //                                                               ------ REMOVE IPv6 ADDR FROM ADDR TBL ------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  SList_Rem(&p_if->IP_Obj->IPv6_AddrListPtr, &p_addr_obj->ListNode);
  Mem_DynPoolBlkFree(&NetIPv6_AddrPool, p_addr_obj, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                       NetIPv6_AddrPrefLifetimeTimeout()
 *
 * @brief    Set the IPv6 address has being deprecated when preferred lifetime comes to an end.
 *
 * @param    p_ipv6_addr_timeout     Pointer to ipv6 addrs object that has timed out.
 *******************************************************************************************************/
void NetIPv6_AddrPrefLifetimeTimeout(void *p_ipv6_addr_timeout)
{
  NET_IPv6_ADDR_OBJ *p_addr_obj;

  p_addr_obj = (NET_IPv6_ADDR_OBJ *)p_ipv6_addr_timeout;

  p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_DEPRECATED;
  p_addr_obj->IsValid = DEF_YES;

  p_addr_obj->PrefLifetimeTmrPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                           NetIPv6_AddrAutoCfgComp()
 *
 * @brief    (1) Complete the IPv6 Auto-Configuration process.
 *               - (a) Update IPv6 Auto-Configuration object for given interface.
 *               - (b) Call Application Hook function.
 *
 * @param    if_nbr              Network interface number on which address auto-configuration occurred.
 *
 *
 * Argument(s) : if_nbr           Network interface number on which address auto-configuration occurred.
 *
 *               auto_cfg_status  Status of the IPv6 Auto-Configuration process.
 *
 * Return(s)   : None.
 *
 * Note(s)     : (2) NetIPv6_AddrAutoCfgComp() is called by network protocol suite function(s) &
 *                   MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
void NetIPv6_AddrAutoCfgComp(NET_IF_NBR               if_nbr,
                             NET_IPv6_ADDR_CFG_STATUS auto_cfg_status)
{
  NET_IF                *p_if;
  NET_IPv6_AUTO_CFG_OBJ *p_auto_obj;
  RTOS_ERR              local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  p_if = NetIF_Get(if_nbr, &local_err);                         // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_auto_obj = p_if->IP_Obj->IPv6_AutoCfgObjPtr;

  switch (p_auto_obj->State) {
    case NET_IPv6_AUTO_CFG_STATE_STARTED_LOCAL:
      p_auto_obj->State = NET_IPv6_AUTO_CFG_STATE_NONE;
      if (p_auto_obj->En == DEF_YES) {
        NET_IPv6_ADDR_SUBSCRIBE_OBJ *p_subscriber;

        SLIST_FOR_EACH_ENTRY(NetIPv6_AddrSubscriberListHeadPtr, p_subscriber, NET_IPv6_ADDR_SUBSCRIBE_OBJ, ListNode) {
          if (p_subscriber->Fnct != DEF_NULL) {
            p_subscriber->Fnct(if_nbr, NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL, p_auto_obj->AddrLocalPtr, auto_cfg_status);
            p_subscriber->Fnct(if_nbr, NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL, p_auto_obj->AddrGlobalPtr, NET_IPv6_ADDR_CFG_STATUS_NOT_DONE);
          }
        }
      }
      break;

    case NET_IPv6_AUTO_CFG_STATE_STARTED_GLOBAL:
      p_auto_obj->State = NET_IPv6_AUTO_CFG_STATE_NONE;
      if (p_auto_obj->En == DEF_YES) {
        NET_IPv6_ADDR_SUBSCRIBE_OBJ *p_subscriber;

        SLIST_FOR_EACH_ENTRY(NetIPv6_AddrSubscriberListHeadPtr, p_subscriber, NET_IPv6_ADDR_SUBSCRIBE_OBJ, ListNode) {
          if (p_subscriber->Fnct != DEF_NULL) {
            p_subscriber->Fnct(if_nbr, NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL, p_auto_obj->AddrLocalPtr, NET_IPv6_ADDR_CFG_STATUS_SUCCEED);
            p_subscriber->Fnct(if_nbr, NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL, p_auto_obj->AddrGlobalPtr, auto_cfg_status);
          }
        }
      }
      break;

    case NET_IPv6_AUTO_CFG_STATE_STOPPED:
      p_auto_obj->State = NET_IPv6_AUTO_CFG_STATE_NONE;
      if (p_auto_obj->En == DEF_YES) {
        NET_IPv6_ADDR_SUBSCRIBE_OBJ *p_subscriber;

        SLIST_FOR_EACH_ENTRY(NetIPv6_AddrSubscriberListHeadPtr, p_subscriber, NET_IPv6_ADDR_SUBSCRIBE_OBJ, ListNode) {
          if (p_subscriber->Fnct != DEF_NULL) {
            if (p_auto_obj->AddrLocalPtr != DEF_NULL) {
              p_subscriber->Fnct(if_nbr, NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL, p_auto_obj->AddrLocalPtr, NET_IPv6_ADDR_CFG_STATUS_SUCCEED);
            } else {
              p_subscriber->Fnct(if_nbr, NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL, p_auto_obj->AddrLocalPtr, NET_IPv6_ADDR_CFG_STATUS_NOT_DONE);
            }

            if (p_auto_obj->AddrGlobalPtr != DEF_NULL) {
              p_subscriber->Fnct(if_nbr, NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL, p_auto_obj->AddrGlobalPtr, NET_IPv6_ADDR_CFG_STATUS_SUCCEED);
            } else {
              p_subscriber->Fnct(if_nbr, NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL, p_auto_obj->AddrGlobalPtr, NET_IPv6_ADDR_CFG_STATUS_NOT_DONE);
            }
          }
        }
      }
      break;

    default:
      p_auto_obj->State = NET_IPv6_AUTO_CFG_STATE_NONE;
      break;
  }

  NetTmr_Free(p_auto_obj->RouterSolRetryTmr);
  p_auto_obj->RouterSolRetryTmr = DEF_NULL;
  p_auto_obj->RouterSolRetryCnt = 0;
  p_auto_obj->RouterSolAddrSrcPtr = DEF_NULL;
  p_auto_obj->AddrLocalPtr = DEF_NULL;
  p_auto_obj->AddrGlobalPtr = DEF_NULL;

exit:
  return;
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetIPv6_AddrAutoCfgStop()
 *
 * @brief    Stop without disabling the IPv6 Auto-Cfg process.
 *
 * @param    if_nbr  Interface number on which to stop the Auto-cfg process.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
static void NetIPv6_AddrAutoCfgStop(NET_IF_NBR if_nbr,
                                    RTOS_ERR   *p_err)
{
  NET_IF                *p_if = DEF_NULL;
  NET_IPv6_AUTO_CFG_OBJ *p_auto_obj = DEF_NULL;

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_auto_obj = p_if->IP_Obj->IPv6_AutoCfgObjPtr;

  if (p_auto_obj != DEF_NULL) {
    switch (p_auto_obj->State) {
      case NET_IPv6_AUTO_CFG_STATE_STARTED_LOCAL:
        p_auto_obj->AddrLocalPtr = DEF_NULL;
        p_auto_obj->AddrGlobalPtr = DEF_NULL;
        p_auto_obj->State = NET_IPv6_AUTO_CFG_STATE_STOPPED;
        break;

      case NET_IPv6_AUTO_CFG_STATE_STARTED_GLOBAL:
        p_auto_obj->AddrGlobalPtr = DEF_NULL;
        p_auto_obj->State = NET_IPv6_AUTO_CFG_STATE_STOPPED;
        break;

      default:
        p_auto_obj->AddrLocalPtr = DEF_NULL;
        p_auto_obj->AddrGlobalPtr = DEF_NULL;
        p_auto_obj->State = NET_IPv6_AUTO_CFG_STATE_NONE;
        break;
    }

    NetTmr_Free(p_auto_obj->RouterSolRetryTmr);
    p_auto_obj->RouterSolRetryTmr = DEF_NULL;
    p_auto_obj->RouterSolRetryCnt = 0;
    p_auto_obj->RouterSolAddrSrcPtr = DEF_NULL;
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv6_AddrAutoCfgHandler()
 *
 * @brief    (1) Perform IPv6 Stateless Address Auto-Configuration:
 *
 *           - (a) Create link-local address.
 *           - (b) Add link-local address to the interface.
 *           - (c) If link-local configuration failed, restart with random address.
 *           - (d) Start global address configuration.
 *
 * @param    if_nbr  Network interface number to perform address auto-configuration.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) NetIPv6_AddrAutoCfgHandler() is called by network protocol suite function(s) &
 *                   MUST be called with the global network lock already acquired.
 *
 * @note     (2) RFC #4862 , Section 4 states that "Auto-configuration is performed only on
 *                   multicast-capable links and begins when a multicast-capable interface is enabled,
 *                   e.g., during system startup".
 *******************************************************************************************************/
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
static void NetIPv6_AddrAutoCfgHandler(NET_IF_NBR if_nbr,
                                       RTOS_ERR   *p_err)
{
  NET_IF                   *p_if;
  NET_IPv6_AUTO_CFG_OBJ    *p_auto_obj;
  NET_IPv6_ADDR_OBJ        *p_addr_obj;
  NET_IPv6_ADDR            *p_addr_cfgd = DEF_NULL;
  NET_IPv6_ADDR            ipv6_id;
  NET_IPv6_ADDR            ipv6_addr;
  NET_IPv6_AUTO_CFG_STATE  state;
  NET_IPv6_ADDR_CFG_STATUS status;
  CPU_BOOLEAN              dad_en;

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  //                                                               --------- ADVERTISE AUTOCONFIG AS STARTED ----------
  p_auto_obj = p_if->IP_Obj->IPv6_AutoCfgObjPtr;

  state = p_auto_obj->State;
  if ((state == NET_IPv6_AUTO_CFG_STATE_STARTED_LOCAL)
      || (state == NET_IPv6_AUTO_CFG_STATE_STARTED_GLOBAL)) {
    goto exit;
  }

  p_auto_obj->State = NET_IPv6_AUTO_CFG_STATE_STARTED_LOCAL;

  //                                                               ------------- SET DAD ENABLE VARIABLE --------------
  dad_en = p_auto_obj->DAD_En;

  //                                                               -------------- CREATE LINK-LOCAL ADDR --------------
  (void)NetIPv6_CreateIF_ID(if_nbr,                             // Create IF ID from HW mac addr.
                            &ipv6_id,
                            NET_IPv6_ADDR_AUTO_CFG_ID_IEEE_48,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_auto_obj->AddrLocalPtr = DEF_NULL;
    p_auto_obj->AddrGlobalPtr = DEF_NULL;
    status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
    goto exit_comp;
  }

  NetIPv6_CreateAddrFromID(&ipv6_id,                            // Create link-local IPv6 addr from IF ID.
                           &ipv6_addr,
                           NET_IPv6_ADDR_PREFIX_LINK_LOCAL,
                           NET_IPv6_ADDR_PREFIX_LINK_LOCAL_LEN,
                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_auto_obj->AddrLocalPtr = DEF_NULL;
    p_auto_obj->AddrGlobalPtr = DEF_NULL;
    status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
    goto exit_comp;
  }

  //                                                               ------------- ADD ADDRESS TO INTERFACE -------------
  status = NetIPv6_CfgAddrAddHandler(if_nbr,
                                     &ipv6_addr,
                                     NET_IPv6_ADDR_PREFIX_LINK_LOCAL_LEN,
                                     0,
                                     0,
                                     NET_IP_ADDR_CFG_MODE_AUTO_CFG,
                                     dad_en,
                                     NET_IPv6_ADDR_CFG_TYPE_AUTO_CFG_NO_BLOCKING,
                                     &p_addr_obj,
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_auto_obj->AddrLocalPtr = DEF_NULL;
    p_auto_obj->AddrGlobalPtr = DEF_NULL;
    status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
    goto exit_comp;
  }

  switch (status) {
    case NET_IPv6_ADDR_CFG_STATUS_SUCCEED:
      p_auto_obj->AddrLocalPtr = &p_addr_obj->AddrHost;
      //                                                           Continue autoconfig with global addr cfg.
      NetIPv6_CfgAddrGlobal(if_nbr,
                            &p_addr_obj->AddrHost,
                            p_err);
      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_TIMEOUT:
          p_auto_obj->AddrGlobalPtr = DEF_NULL;
          status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
          RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
          break;

        default:
          p_auto_obj->AddrGlobalPtr = DEF_NULL;
          status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
        //                                                         fallthrough
        case RTOS_ERR_NONE:
          goto exit;
      }
      break;

    case NET_IPv6_ADDR_CFG_STATUS_IN_PROGRESS:
      goto exit;                                                // Callback function will continue auto-config.

    case NET_IPv6_ADDR_CFG_STATUS_DUPLICATE:
      //                                                           Restart autoconfig with random address.
      status = NetIPv6_CfgAddrRand(if_nbr,
                                   &ipv6_id,
                                   dad_en,
                                   &p_addr_cfgd,
                                   p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }

      switch (status) {
        case NET_IPv6_ADDR_CFG_STATUS_SUCCEED:
          p_auto_obj->AddrLocalPtr = p_addr_cfgd;
          //                                                       Continue autoconfig with global addr cfg.
          NetIPv6_CfgAddrGlobal(if_nbr,
                                &ipv6_addr,
                                p_err);
          switch (RTOS_ERR_CODE_GET(*p_err)) {
            case RTOS_ERR_NONE:
              goto exit;

            case RTOS_ERR_TIMEOUT:
              RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
            //                                                     fallthrough
            default:
              p_auto_obj->AddrGlobalPtr = DEF_NULL;
              status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
              break;
          }
          break;

        case NET_IPv6_ADDR_CFG_STATUS_IN_PROGRESS:
          goto exit;                                            // Callback function will continue autoconfig.

        case NET_IPv6_ADDR_CFG_STATUS_DUPLICATE:
        default:
          p_auto_obj->AddrLocalPtr = DEF_NULL;
          p_auto_obj->AddrGlobalPtr = DEF_NULL;
          status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
          goto exit_comp;
      }
      break;

    default:
      p_auto_obj->AddrLocalPtr = DEF_NULL;
      p_auto_obj->AddrGlobalPtr = DEF_NULL;
      status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
      goto exit_comp;
  }

exit_comp:
  NetIPv6_AddrAutoCfgComp(if_nbr, status);

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                           NetIPv6_CfgAddrRand()
 *
 * @brief    Create Random Link-Local address and start address configuration.
 *
 * @param    if_nbr      Network interface number to perform address auto-configuration.
 *
 * @param    p_ipv6_id   Pointer to IPv6 address Interface ID.
 *
 * @param    dad_en      DEF_YES, Do the Duplication Address Detection (DAD)
 *                       DEF_NO , otherwise
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
static NET_IPv6_ADDR_CFG_STATUS NetIPv6_CfgAddrRand(NET_IF_NBR    if_nbr,
                                                    NET_IPv6_ADDR *p_ipv6_id,
                                                    CPU_BOOLEAN   dad_en,
                                                    NET_IPv6_ADDR **p_addr_rtn,
                                                    RTOS_ERR      *p_err)
{
  NET_IPv6_ADDR_OBJ        *p_addr_obj;
  NET_IPv6_ADDR            *p_addr_return = DEF_NULL;
  NET_IPv6_ADDR            ipv6_addr;
  RAND_NBR                 rand_nbr;
  CPU_INT08U               *p_addr_08;
  CPU_INT08U               retry_cnt;
  CPU_INT32U               ix;
  CPU_BOOLEAN              done = DEF_NO;
  NET_IPv6_ADDR_CFG_STATUS cfg_status = NET_IPv6_ADDR_CFG_STATUS_NONE;

  rand_nbr = ((p_ipv6_id->Addr[3] << 24) | (p_ipv6_id->Addr[2] << 16) | (p_ipv6_id->Addr[1] <<  8) | (p_ipv6_id->Addr[0]))
             ^ ((p_ipv6_id->Addr[7] << 24) | (p_ipv6_id->Addr[6] << 16) | (p_ipv6_id->Addr[5] <<  8) | (p_ipv6_id->Addr[4]))
             ^ ((p_ipv6_id->Addr[11] << 24) | (p_ipv6_id->Addr[10] << 16) | (p_ipv6_id->Addr[9] <<  8) | (p_ipv6_id->Addr[8]))
             ^ ((p_ipv6_id->Addr[15] << 24) | (p_ipv6_id->Addr[14] << 16) | (p_ipv6_id->Addr[13] <<  8) | (p_ipv6_id->Addr[12]));

  Math_RandSetSeed(rand_nbr);

  retry_cnt = 0;

  while ((done != DEF_YES)
         && (retry_cnt < NET_IPv6_AUTO_CFG_RAND_RETRY_MAX)) {
    //                                                             Generate a new ID from a random src.
    rand_nbr = Math_Rand();
    ix = NET_IPv6_ADDR_PREFIX_LINK_LOCAL_LEN / 8u;
    p_addr_08 = (CPU_INT08U *)&p_ipv6_id->Addr[ix];

    while (ix < NET_IPv6_ADDR_SIZE) {
      *p_addr_08 ^= (CPU_INT08U)rand_nbr;                       // Bitwise XOR each byte of the ID with the random nbr.
      p_addr_08++;
      ix++;
    }

    NetIPv6_CreateAddrFromID(p_ipv6_id,                         // Create link-local IPv6 addr from IF ID.
                             &ipv6_addr,
                             NET_IPv6_ADDR_PREFIX_LINK_LOCAL,
                             NET_IPv6_ADDR_PREFIX_LINK_LOCAL_LEN,
                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    //                                                             ------------- ADD ADDRESS TO INTERFACE -------------
    cfg_status = NetIPv6_CfgAddrAddHandler(if_nbr,
                                           &ipv6_addr,
                                           NET_IPv6_ADDR_PREFIX_LINK_LOCAL_LEN,
                                           0,
                                           0,
                                           NET_IP_ADDR_CFG_MODE_AUTO_CFG,
                                           dad_en,
                                           NET_IPv6_ADDR_CFG_TYPE_AUTO_CFG_NO_BLOCKING,
                                           &p_addr_obj,
                                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    switch (cfg_status) {
      case NET_IPv6_ADDR_CFG_STATUS_SUCCEED:
      case NET_IPv6_ADDR_CFG_STATUS_IN_PROGRESS:
        done = DEF_YES;
        break;

      default:
        break;
    }

    retry_cnt++;
  }

  if (retry_cnt == NET_IPv6_AUTO_CFG_RAND_RETRY_MAX) {
    goto exit;
  }

  if (p_addr_obj == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  p_addr_return = &p_addr_obj->AddrHost;

  if (p_addr_rtn != DEF_NULL) {
    *p_addr_rtn = p_addr_return;
  }

exit:
  return (cfg_status);
}
#endif

/****************************************************************************************************//**
 *                                           NetIPv6_CfgAddrGlobal()
 *
 * @brief    (1) Start IPv6 global address configuration:
 *           - (a) Create Signal to be advertise of Router Advertisement reception.
 *           - (b) Send Router Solicitation.
 *           - (c) Wait for a received Router Advertisement.
 *
 * @param    if_nbr          Network interface number to perform address auto-configuration.
 *
 * @param    p_ipv6_addr     Pointer to IPv6 link-local address that was configured.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
static void NetIPv6_CfgAddrGlobal(NET_IF_NBR    if_nbr,
                                  NET_IPv6_ADDR *p_ipv6_addr,
                                  RTOS_ERR      *p_err)
{
  NET_IF                *p_if;
  NET_IPv6_AUTO_CFG_OBJ *p_obj;

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_obj = p_if->IP_Obj->IPv6_AutoCfgObjPtr;

  if (p_obj->State == NET_IPv6_AUTO_CFG_STATE_STOPPED) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    goto exit;
  }

  p_obj->State = NET_IPv6_AUTO_CFG_STATE_STARTED_GLOBAL;

  NetNDP_TxRouterSolicitation(if_nbr,
                              p_ipv6_addr,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_obj->RouterSolAddrSrcPtr = p_ipv6_addr;
  p_obj->RouterSolRetryCnt = 0;
  p_obj->RouterSolRetryTmr = NetTmr_Get(NetIPv6_TxSolicitationTimeout,
                                        p_if,
                                        NET_NDP_RX_ROUTER_ADV_TIMEOUT_MS,
                                        NET_TMR_OPT_ONE_SHOT,
                                        p_err);

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv6_AddrAutoCfgDAD_Result()
 *
 * @brief    IPv6 Auto-configuration callback function when DAD is enabled to continue auto-configuration
 *           process.
 *
 * @param    if_nbr      Network Interface number on which DAD is occurring.
 *
 * @param    p_dad_obj   Pointer to current DAD object.
 *
 * @param    status      Status of the DAD process.
 *******************************************************************************************************/
#if (defined(NET_IPv6_ADDR_AUTO_CFG_MODULE_EN) \
  && (defined(NET_DAD_MODULE_EN))                      )
static void NetIPv6_AddrAutoCfgDAD_Result(NET_IF_NBR               if_nbr,
                                          NET_DAD_OBJ              *p_dad_obj,
                                          NET_IPv6_ADDR_CFG_STATUS status)
{
  NET_IF                   *p_if;
  NET_IPv6_AUTO_CFG_OBJ    *p_auto_cfg_obj;
  NET_IPv6_ADDR_OBJ        *p_addr_obj;
  NET_IPv6_ADDR            *p_ipv6_addr;
  NET_IPv6_ADDR_CFG_STATUS auto_cfg_result;
  CPU_BOOLEAN              local_addr;
  RTOS_ERR                 local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  p_if = NetIF_Get(if_nbr, &local_err);                         // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_auto_cfg_obj = p_if->IP_Obj->IPv6_AutoCfgObjPtr;

  //                                                               Auto-Cfg was stopped during the process.
  if ((p_auto_cfg_obj->State == NET_IPv6_AUTO_CFG_STATE_STOPPED)
      || (p_auto_cfg_obj->State == NET_IPv6_AUTO_CFG_STATE_NONE)   ) {
    auto_cfg_result = NET_IPv6_ADDR_CFG_STATUS_NOT_DONE;
    NetDAD_Stop(if_nbr, p_dad_obj);
    goto exit_comp;
  }

  p_ipv6_addr = &p_dad_obj->Addr;

  p_addr_obj = NetIPv6_GetAddrsHostOnIF(if_nbr, p_ipv6_addr);   // Recover IPv6 addrs object.
  if (p_addr_obj == DEF_NULL) {
    p_auto_cfg_obj->AddrLocalPtr = DEF_NULL;
    p_auto_cfg_obj->AddrGlobalPtr = DEF_NULL;
    auto_cfg_result = NET_IPv6_ADDR_CFG_STATUS_FAIL;
    NetDAD_Stop(if_nbr, p_dad_obj);
    goto exit_comp;
  }

  NetDAD_Stop(if_nbr, p_dad_obj);                               // Stop current DAD process.

  p_ipv6_addr = &p_addr_obj->AddrHost;

  local_addr = NetIPv6_IsAddrLinkLocal(p_ipv6_addr);

  if (local_addr == DEF_YES) {                                  // Cfg of link-local addr has finished.
    if (status != NET_IPv6_ADDR_CFG_STATUS_SUCCEED) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      (void)NetIPv6_CfgAddrRand(if_nbr,
                                p_ipv6_addr,
                                DEF_YES,
                                DEF_NULL,
                                &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
        p_auto_cfg_obj->AddrLocalPtr = DEF_NULL;
        p_auto_cfg_obj->AddrGlobalPtr = DEF_NULL;
        auto_cfg_result = NET_IPv6_ADDR_CFG_STATUS_FAIL;
        goto exit_comp;
      }

      goto exit;
    } else {
      p_auto_cfg_obj->AddrLocalPtr = p_ipv6_addr;

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      NetIPv6_CfgAddrGlobal(if_nbr,
                            p_ipv6_addr,
                            &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
        p_auto_cfg_obj->AddrGlobalPtr = DEF_NULL;
        auto_cfg_result = NET_IPv6_ADDR_CFG_STATUS_FAIL;
        goto exit_comp;
      }

      goto exit;
    }
  } else {                                                      // Cfg of global addr has finished.
    if (status != NET_IPv6_ADDR_CFG_STATUS_SUCCEED) {
      p_auto_cfg_obj->AddrGlobalPtr = DEF_NULL;
      auto_cfg_result = NET_IPv6_ADDR_CFG_STATUS_FAIL;
      goto exit_comp;
    } else {
      p_auto_cfg_obj->AddrGlobalPtr = p_ipv6_addr;
      auto_cfg_result = NET_IPv6_ADDR_CFG_STATUS_SUCCEED;
      goto exit_comp;
    }
  }

exit_comp:
  NetIPv6_AddrAutoCfgComp(if_nbr, auto_cfg_result);

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                       NetIPv6_CfgAddrAddDAD_Result()
 *
 * @brief    Hook function call after IPv6 static address configuration with DAD enabled to release DAD
 *           object.
 *
 * @param    if_nbr      Interface number on which DAD is occurring.
 *
 * @param    p_dad_obj   Pointer to current DAD object.
 *
 * @param    status      Status of the DAD process.
 *******************************************************************************************************/
#ifdef NET_DAD_MODULE_EN
static void NetIPv6_CfgAddrAddDAD_Result(NET_IF_NBR               if_nbr,
                                         NET_DAD_OBJ              *p_dad_obj,
                                         NET_IPv6_ADDR_CFG_STATUS status)
{
  NET_IPv6_ADDR_OBJ           *p_addr_obj;
  NET_IPv6_ADDR               *p_ipv6_addr;
  NET_IPv6_ADDR               *p_addr_return;
  NET_IPv6_ADDR_CFG_STATUS    status_return;
  NET_IPv6_ADDR_SUBSCRIBE_OBJ *p_subscriber;

  p_ipv6_addr = &p_dad_obj->Addr;

  p_addr_obj = NetIPv6_GetAddrsHostOnIF(if_nbr, p_ipv6_addr);
  if (p_addr_obj == DEF_NULL) {
    p_addr_return = DEF_NULL;
    status_return = NET_IPv6_ADDR_CFG_STATUS_FAIL;
  } else {
    NET_IPv6_ADDR prefix;
    CPU_BOOLEAN   is_link_local;
    RTOS_ERR      local_err;

    p_addr_return = &p_addr_obj->AddrHost;
    status_return = status;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    is_link_local = NetIPv6_IsAddrLinkLocal(&p_addr_obj->AddrHost);
    if (is_link_local != DEF_YES) {
      NetIPv6_AddrMaskByPrefixLen(&p_addr_obj->AddrHost, p_addr_obj->AddrHostPrefixLen, &prefix, &local_err);
      NetNDP_PrefixAddCfg(if_nbr, &prefix, p_addr_obj->AddrHostPrefixLen, DEF_NO, DEF_NULL, 0, &local_err);
    }
  }

  SLIST_FOR_EACH_ENTRY(NetIPv6_AddrSubscriberListHeadPtr, p_subscriber, NET_IPv6_ADDR_SUBSCRIBE_OBJ, ListNode) {
    if (p_subscriber->Fnct != DEF_NULL) {
      p_subscriber->Fnct(if_nbr, NET_IPv6_CFG_ADDR_TYPE_STATIC, p_addr_return, status_return);
    }
  }

  NetDAD_Stop(if_nbr, p_dad_obj);
}
#endif

/****************************************************************************************************//**
 *                                           NetIPv6_AddrSrcSel()
 *
 * @brief    Select the best Source address for the given destination address following the rules
 *           given by the Default Source Selection mentioned in RFC #6724.
 *
 * @param    if_nbr          Interface number on which to send packet.
 *
 * @param    p_addr_dest     Pointer to IPv6 destination address.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to IPv6 addresses object to use as source address.
 *
 * @note     (1) If source address selection failed, the first address in the Interface addresses
 *               table is return.
 *
 * @note     (2) RFC #6724 Section 5 - "Source Address Selection" describe selection rules:
 *
 *           - (a) Rule 1: Prefer same address.
 *
 *           - (b) Rule 3: Avoid deprecated addresses.
 *
 *           - (c) Rule 4: Prefer home addresses.
 *
 *               - (i) Not applicable
 *
 *           - (d) Rule 5: Prefer outgoing interface.
 *
 *               - (i) Not applicable
 *
 *           - (e) Rule 5.5: Prefer addresses in a prefix advertised by the next-hop.
 *
 *               - (i) Not applicable
 *
 *           - (f) Rule 6: Prefer matching label.
 *
 *           - (g) Rule 7: Prefer temporary addresses.
 *
 *               - (i) Not applicable
 *
 *           - (h) Rule 8: Use longest matching prefix.
 *******************************************************************************************************/
static const NET_IPv6_ADDR_OBJ *NetIPv6_AddrSrcSel(NET_IF_NBR          if_nbr,
                                                   const NET_IPv6_ADDR *p_addr_dest,
                                                   RTOS_ERR            *p_err)
{
  NET_IF                  *p_if;
  const NET_IPv6_ADDR_OBJ *p_addr_cur;
  const NET_IPv6_ADDR_OBJ *p_addr_sel = DEF_NULL;
  const NET_IPv6_POLICY   *p_policy_dest;
  const NET_IPv6_POLICY   *p_policy_addr;
  CPU_BOOLEAN             valid;
  CPU_INT08U              rule_sel;
  CPU_INT08U              dest_scope;
  CPU_INT08U              addr_scope;
  CPU_INT08U              len_cur;
  CPU_INT08U              len_sel;
  RTOS_ERR                local_err;

  p_if = NetIF_Get(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  dest_scope = NetIPv6_GetAddrScope(p_addr_dest);
  p_policy_dest = NetIPv6_AddrSelPolicyGet(p_addr_dest);
  rule_sel = NET_IPv6_SRC_SEL_RULE_NONE;
  len_sel = 0u;

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv6_AddrListPtr, p_addr_cur, NET_IPv6_ADDR_OBJ, ListNode) {
    //                                                             RULE #1 : Prefer same address.
    valid = NetIPv6_IsAddrsIdentical(&p_addr_cur->AddrHost, p_addr_dest);
    if (valid == DEF_YES) {
      p_addr_sel = p_addr_cur;
      goto exit;
    }

    //                                                             RULE #2 : Prefer appropriate scope.
    addr_scope = NetIPv6_GetAddrScope(&p_addr_cur->AddrHost);
    if (addr_scope == dest_scope) {
      p_addr_sel = p_addr_cur;
      rule_sel = NET_IPv6_SRC_SEL_RULE_02;
      continue;
    }

    if (rule_sel > NET_IPv6_SRC_SEL_RULE_03) {                  // RULE #3 : Avoid deprecated addresses.
      if (p_addr_cur->AddrState == NET_IPv6_ADDR_STATE_PREFERRED) {
        p_addr_sel = p_addr_cur;
        rule_sel = NET_IPv6_SRC_SEL_RULE_03;
        continue;
      }
    } else {
      continue;
    }

    //                                                             RULE #4 :   See Note 2d.
    //                                                             RULE #5 :   See Note 2e.
    //                                                             RULE #5.5 : See Note 2f.

    //                                                             RULE #6 : Prefer matching label.
    if (rule_sel > NET_IPv6_SRC_SEL_RULE_06) {
      p_policy_addr = NetIPv6_AddrSelPolicyGet(&p_addr_cur->AddrHost);
      if (p_policy_addr->Label == p_policy_dest->Label) {
        p_addr_sel = p_addr_cur;
        rule_sel = NET_IPv6_SRC_SEL_RULE_06;
        continue;
      }
    } else {
      continue;
    }

    //                                                             RULE #7 :   See Note 2d.

    //                                                             RULE #8 : Use longest matching prefix.
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    len_cur = NetIPv6_GetAddrMatchingLen(p_addr_dest, &p_addr_cur->AddrHost, &local_err);
    if (len_cur >= len_sel) {
      len_sel = len_cur;
      p_addr_sel = p_addr_cur;
      rule_sel = NET_IPv6_SRC_SEL_RULE_08;
      continue;
    }
  }

  if (rule_sel == NET_IPv6_SRC_SEL_RULE_NONE) {
    p_addr_sel = SLIST_ENTRY(p_if->IP_Obj->IPv6_AddrListPtr, NET_IPv6_ADDR_OBJ, ListNode);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

exit:
  return (p_addr_sel);
}

/****************************************************************************************************//**
 *                                           NetIPv6_LookUpPolicyTbl()
 *
 * @brief    Get address selection policy for a given IPv6 address.
 *
 * @param    p_addr  Pointer to IPv6 address
 *
 * @return   Pointer to address selection policy matching the given address.
 *
 * @note     (1) The Policy table is part of the procedure to select the source and destination
 *               address to Transmit packets.
 *******************************************************************************************************/
static const NET_IPv6_POLICY *NetIPv6_AddrSelPolicyGet(const NET_IPv6_ADDR *p_addr)
{
  const NET_IPv6_POLICY *p_policy_entry;
  const NET_IPv6_POLICY *p_policy_found = DEF_NULL;
  CPU_INT08U            i;
  CPU_BOOLEAN           valid;

  for (i = 0; i < NET_IPv6_POLICY_TBL_SIZE; i++) {
    p_policy_entry = NetIPv6_PolicyTbl[i];
    valid = NetIPv6_IsAddrAndMaskValid(p_addr, p_policy_entry->PrefixMaskPtr);
    if (valid == DEF_YES) {
      p_policy_found = p_policy_entry;
      goto exit;
    }
  }

exit:
  return (p_policy_found);
}

/****************************************************************************************************//**
 *                                           NetIPv6_CfgAddrValidate()
 *
 * @brief    Validate an IPv6 host address and prefix length for configuration on an interface.
 *
 * @param    p_addr      Pointer to desired IPv6 address to configure.
 *
 * @param    prefix_len  Prefix length of the desired IPv6 address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) See function NetIPv6_IsValidAddrHost() for supported IPv6 address host.
 *******************************************************************************************************/

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIPv6_CfgAddrValidate(NET_IPv6_ADDR *p_addr,
                                    CPU_INT08U    prefix_len,
                                    RTOS_ERR      *p_err)
{
  CPU_BOOLEAN addr_valid;

  //                                                               ----------- VALIDATE IPv6 ADDR PREFIX LEN ----------
  if (prefix_len > NET_IPv6_ADDR_PREFIX_LEN_MAX) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.CfgInvAddrHostCtr);
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  //                                                               -------------- VALIDATE HOST ADDRESS ---------------
  addr_valid = NetIPv6_IsValidAddrHost(p_addr);
  if (addr_valid != DEF_YES) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.CfgInvAddrHostCtr);
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }
}
#endif

/****************************************************************************************************//**
 *                                           NetIPv6_RxPktValidate()
 *
 * @brief    (1) Validate received IPv6 packet :
 *
 *           - (a) Validate ...
 *               - (1) the received packet's following IPv6 header fields :
 *
 *                   - (A) Version
 *                   - (B) Traffic Class
 *                   - (C) Flow Label
 *                   - (D) Payload Length
 *                   - (E) Next Header
 *                   - (F) Source      Address
 *                   - (G) Destination Address
 *
 *               - (2) Validation ignores the following IPv6 header fields :
 *
 *                   - (A) Hop Limit
 *
 *           - (b) Convert the following IPv6 header fields from network-order to host-order :
 *
 *               - (1) Version
 *               - (2) Traffic Class
 *               - (3) Flow Label
 *
 *                   - (A) These fields are NOT converted directly in the received packet buffer's
 *                           data area but are converted in local or network buffer variables ONLY.
 *
 *                   - (B) The following IPv6 header fields are converted & stored in network buffer
 *                           variables :
 *
 *                       - (1) Payload Length
 *                       - (4) Source Address
 *                       - (5) Destination Address
 *                       - (6) Next Header
 *
 *           - (c) Update network buffer's protocol controls
 *
 *           - (d) Process IPv6 packet in ICMPv6 Receive Handler
 *
 * @param    p_buf       Pointer to network buffer that received IPv6 packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_ip_hdr    Pointer to received packet's IPv6 header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See 'net_ipv6.h  IP HEADER' for IPv6 header format.
 *
 * @note     (3) The following IPv6 header fields MUST be decoded &/or converted from network-order to host-order
 *                       BEFORE any ICMP Error Messages are transmitted (see 'net_icmp.c  NetICMPv6_TxMsgErr()  Note #2') :
 *
 *                           - (a) Header Length
 *                           - (b) Payload  Length
 *                           - (c) Source      Address
 *                           - (d) Destination Address
 *
 * @note     (4) See RFC #1122, Section 2.3.1
 *                       RFC #894, Section 'Frame Format'
 *                       RFC #1042, Section 'Frame Format and MAC Level Issues : For all hardware types'
 *                       RFC #893, Section 'Introduction'
 *                       RFC #4443, Section 3.4 'Parameter Problem Message'
 *******************************************************************************************************/
static void NetIPv6_RxPktValidate(NET_BUF      *p_buf,
                                  NET_BUF_HDR  *p_buf_hdr,
                                  NET_IPv6_HDR *p_ip_hdr,
                                  RTOS_ERR     *p_err)
{
#if  (NET_IPv6_CFG_TRAFFIC_CLASS_EN == DEF_ENABLED)
  CPU_INT32U ip_traffic_class;
  CPU_INT08U ip_dscp;
  CPU_INT08U ip_ecn;
#endif
#if  (NET_IPv6_CFG_FLOW_LABEL_EN == DEF_ENABLED)
  CPU_INT32U ip_flow_label;
#endif
  const NET_IPv6_ADDR_OBJ *p_ip_addrs;
  NET_IPv6_ADDR_TYPE      addr_type;
  NET_IF_NBR              if_nbr;
  CPU_INT32U              ip_ver_traffic_flow;
  CPU_INT32U              ip_ver;
  CPU_INT16U              protocol_ix;
  CPU_BOOLEAN             rx_remote_host;
#ifdef NET_MLDP_MODULE_EN
  CPU_BOOLEAN grp_joined;
#endif
#ifdef NET_ICMPv6_MODULE_EN
  RTOS_ERR local_err;
#endif

  //                                                               --------------- CONVERT IPv6 FIELDS ----------------
  NET_UTIL_VAL_COPY_GET_NET_32(&ip_ver_traffic_flow, &p_ip_hdr->VerTrafficFlow);
  NET_UTIL_VAL_COPY_GET_NET_16(&p_buf_hdr->IP_TotLen, &p_ip_hdr->PayloadLen);

  Mem_Copy(&p_buf_hdr->IPv6_AddrSrc, &p_ip_hdr->AddrSrc, NET_IPv6_ADDR_SIZE);
  Mem_Copy(&p_buf_hdr->IPv6_AddrDest, &p_ip_hdr->AddrDest, NET_IPv6_ADDR_SIZE);

  //                                                               ---------------- VALIDATE IPv6 VER -----------------
  ip_ver = ip_ver_traffic_flow & NET_IPv6_HDR_VER_MASK;
  ip_ver >>= NET_IPv6_HDR_VER_SHIFT;
  if (ip_ver != NET_IPv6_HDR_VER) {                             // Validate IP ver.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvVerCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

#if  (NET_IPv6_CFG_TRAFFIC_CLASS_EN == DEF_ENABLED)
  //                                                               ----------- VALIDATE IPv6 TRAFFIC CLASS ------------
  //                                                               See 'net_ipv6.h  IPv6 HEADER  Note #x'.

  ip_traffic_class = ip_ver_traffic_flow & NET_IPv6_HDR_TRAFFIC_CLASS_MASK_32;
  ip_traffic_class >>= NET_IPv6_HDR_TRAFFIC_CLASS_SHIFT;

  //                                                               - VALIDATE IPv6 DIFFERENTIATED SERVICES CODEPOINT --

  ip_dscp = ip_traffic_class    & NET_IPv6_HDR_DSCP_MASK_08;
  ip_dscp >>= NET_IPv6_HDR_DSCP_SHIFT;

  //                                                               -- VALIDATE IPv6 EXPLICIT CONGESTION NOTIFICATION --

  ip_ecn = ip_traffic_class    & NET_IPv6_HDR_ECN_MASK_08;

  APP_TRACE_IPv6("           Traffic class = %u\r\n", ip_traffic_class);

  if (ip_traffic_class != NET_IPv6_HDR_TRAFFIC_CLASS) {         // Validate IP traffic class.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvTrafficClassCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

#endif

#if  (NET_IPv6_CFG_FLOW_LABEL_EN == DEF_ENABLED)
  //                                                               ------------ VALIDATE IPv6 FLOW LABEL --------------
  //                                                               See 'net_ipv6.h  IPv6 HEADER  Note #x'.
  ip_flow_label = ip_ver_traffic_flow & NET_IPv6_HDR_FLOW_LABEL_MASK;
  ip_flow_label >>= NET_IPv6_HDR_FLOW_LABEL_SHIFT;
  if (ip_flow_label != NET_IPv6_HDR_FLOW_LABEL) {               // Validate IP flow label.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvFlowLabelCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
#endif

  //                                                               ------------ VALIDATE IPv6 PAYLOAD LEN -------------
  if (p_buf_hdr->IP_TotLen > p_buf_hdr->DataLen) {              // If IPv6 tot len > rem pkt data len, ...
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvTotLenCtr);
#ifdef NET_ICMPv6_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetICMPv6_TxMsgErr(p_buf,
                       NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                       NET_ICMPv6_MSG_CODE_PARAM_PROB_IP_HDR,
                       NET_ICMPv6_PTR_IX_IP_PAYLOAD_LEN,
                       &local_err);
#endif
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                          // ... rtn err (see Note #4).
    goto exit;
  }

  p_buf_hdr->IP_DataLen = (CPU_INT16U)p_buf_hdr->IP_TotLen;
  p_buf_hdr->IP_HdrLen = sizeof(NET_IPv6_HDR);

  //                                                               ------------ VALIDATE IPv6 NEXT HEADER -------------
  switch (p_ip_hdr->NextHdr) {                                  // See 'net_ipv6.h  IP HEADER PROTOCOL FIELD ...
    case NET_IP_HDR_PROTOCOL_ICMPv6:
    case NET_IP_HDR_PROTOCOL_UDP:
    case NET_IP_HDR_PROTOCOL_TCP:
      break;

    case NET_IP_HDR_PROTOCOL_EXT_HOP_BY_HOP:
    case NET_IP_HDR_PROTOCOL_EXT_DEST:
    case NET_IP_HDR_PROTOCOL_EXT_ROUTING:
    case NET_IP_HDR_PROTOCOL_EXT_FRAG:
    case NET_IP_HDR_PROTOCOL_EXT_AUTH:
    case NET_IP_HDR_PROTOCOL_EXT_ESP:
    case NET_IP_HDR_PROTOCOL_EXT_NONE:
    case NET_IP_HDR_PROTOCOL_EXT_MOBILITY:
      break;

    default:                                                    // See Note #x.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvProtocolCtr);
#ifdef  NET_ICMPv6_MODULE_EN
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetICMPv6_TxMsgErr(p_buf,
                         NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                         NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_NEXT_HDR,
                         NET_IPv6_HDR_NEXT_HDR_IX,
                         &local_err);
#endif
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }
  //                                                               --------------- VALIDATE IPv6 ADDRS ----------------
  if_nbr = p_buf_hdr->IF_Nbr;                                   // Get pkt's rx'd IF.

  //                                                               Chk pkt rx'd to cfg'd host addr.
#if 0
  if (if_nbr == NET_IF_NBR_LOCAL_HOST) {
    p_ip_addr = DEF_NULL;
    addr_host_dest = NetIPv6_IsAddrHostCfgdHandler(&p_buf_hdr->IPv6_AddrDest);
  }
#endif
  //                                                               Chk pkt rx'd via local or remote host.
  rx_remote_host = DEF_BIT_IS_SET(p_buf_hdr->Flags, NET_BUF_FLAG_RX_REMOTE);
  if (((if_nbr != NET_IF_NBR_LOCAL_HOST) && (rx_remote_host == DEF_NO))
      || ((if_nbr == NET_IF_NBR_LOCAL_HOST) && (rx_remote_host != DEF_NO))) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvDestCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
  //                                                               -------------- VALIDATE IPv6 SRC ADDR --------------
  addr_type = NetIPv6_AddrTypeValidate(&p_buf_hdr->IPv6_AddrSrc, if_nbr);
  switch (addr_type) {
    case  NET_IPv6_ADDR_TYPE_UNICAST:
    case  NET_IPv6_ADDR_TYPE_LINK_LOCAL:
    case  NET_IPv6_ADDR_TYPE_SITE_LOCAL:
    case  NET_IPv6_ADDR_TYPE_UNSPECIFIED:
    case  NET_IPv6_ADDR_TYPE_NONE:
      break;

    case  NET_IPv6_ADDR_TYPE_MCAST:
    case  NET_IPv6_ADDR_TYPE_MCAST_SOL:
    case  NET_IPv6_ADDR_TYPE_MCAST_ROUTERS:
    case  NET_IPv6_ADDR_TYPE_MCAST_NODES:
    case  NET_IPv6_ADDR_TYPE_LOOPBACK:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }
  //                                                               -------------- VALIDATE IPv6 DEST ADDR -------------
  addr_type = NetIPv6_AddrTypeValidate(&p_buf_hdr->IPv6_AddrDest, if_nbr);
  switch (addr_type) {
    case  NET_IPv6_ADDR_TYPE_MCAST_ROUTERS:
    case  NET_IPv6_ADDR_TYPE_MCAST_NODES:
    case  NET_IPv6_ADDR_TYPE_LOOPBACK:
      break;

    case  NET_IPv6_ADDR_TYPE_MCAST:
    case  NET_IPv6_ADDR_TYPE_MCAST_SOL:
#ifdef  NET_MLDP_MODULE_EN
      grp_joined = NetMLDP_IsGrpJoinedOnIF(if_nbr,
                                           &p_buf_hdr->IPv6_AddrDest);
      if (grp_joined == DEF_NO) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvDestCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
#else
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
#endif
      break;

    case  NET_IPv6_ADDR_TYPE_UNICAST:
    case  NET_IPv6_ADDR_TYPE_SITE_LOCAL:
    case  NET_IPv6_ADDR_TYPE_LINK_LOCAL:
    case  NET_IPv6_ADDR_TYPE_NONE:
    case  NET_IPv6_ADDR_TYPE_UNSPECIFIED:
    default:
      p_ip_addrs = NetIPv6_GetAddrsHostOnIF(if_nbr,
                                            (const NET_IPv6_ADDR *)&p_buf_hdr->IPv6_AddrDest);
      if (p_ip_addrs == DEF_NULL) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvDestCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
  }

  //                                                               ----------------- UPDATE BUF CTRLS -----------------
  //                                                               See Note #3.
  p_buf_hdr->DataLen -= (NET_BUF_SIZE) p_buf_hdr->IP_HdrLen;
  protocol_ix = (CPU_INT16U)(p_buf_hdr->IP_HdrIx + p_buf_hdr->IP_HdrLen);

  //                                                               ----------- PROCESS NEXT HEADER PROTOCOL -----------
  NetIPv6_RxPktValidateNextHdr(p_buf,
                               p_buf_hdr,
                               p_ip_hdr->NextHdr,
                               protocol_ix,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
#ifdef   NET_ICMPv6_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetICMPv6_TxMsgErr(p_buf,
                       NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                       NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_NEXT_HDR,
                       (p_buf_hdr->IP_HdrIx - p_buf_hdr->IP_HdrIx),
                       &local_err);
#endif
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvOptsCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_IPv6_FRAME);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv6_RxPktValidateNextHdr()
 *
 * @brief    (1) Validates the IPv6 Header or Extension Header Next Header.
 *
 *       - (2) Updates the Protocol Header and Protocol Index field of p_buf_hdr.
 *
 * @param    p_buf           Pointer to network buffer that received IPv6 packet.
 *
 * @param    p_buf_hdr       Pointer to network buffer header that received IPv6 packet.
 *
 * @param    next_hdr        Protocol type or Extension Header of the following header.
 *
 * @param    protocol_ix     Index of the Next Header.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (3) If, a node encounters a Next Header value of zero (Hop-by-Hop) in any header other
 *                           than an IPv6 header or an unrecognized Next Header, it must send an ICMP Parameter Problem
 *                           message to the source of the packet, with an ICMP Code value of 1 ("unrecognized Next
 *                           Header type encountered") and the ICMP Pointer field containing the offset of the
 *                           unrecognized value within the original packet. (See RFC #2460, Section 4.0).
 *******************************************************************************************************/
static void NetIPv6_RxPktValidateNextHdr(NET_BUF           *p_buf,
                                         NET_BUF_HDR       *p_buf_hdr,
                                         NET_IPv6_NEXT_HDR next_hdr,
                                         CPU_INT16U        protocol_ix,
                                         RTOS_ERR          *p_err)
{
  switch (next_hdr) {
    case NET_IP_HDR_PROTOCOL_ICMPv6:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_ICMP_V6;
      p_buf_hdr->ProtocolHdrTypeNetSub = NET_PROTOCOL_TYPE_ICMP_V6;
      p_buf_hdr->ICMP_MsgIx = protocol_ix;
      break;

    case NET_IP_HDR_PROTOCOL_UDP:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_UDP_V6;
      p_buf_hdr->ProtocolHdrTypeTransport = NET_PROTOCOL_TYPE_UDP_V6;
      p_buf_hdr->TransportHdrIx = protocol_ix;;
      break;

#ifdef  NET_TCP_MODULE_EN
    case NET_IP_HDR_PROTOCOL_TCP:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_TCP_V6;
      p_buf_hdr->ProtocolHdrTypeTransport = NET_PROTOCOL_TYPE_TCP_V6;
      p_buf_hdr->TransportHdrIx = protocol_ix;
      break;
#endif

    case NET_IP_HDR_PROTOCOL_EXT_HOP_BY_HOP:
      p_buf_hdr->IPv6_HopByHopHdrIx = protocol_ix;
      //                                                           See Note 3.
      if ((p_buf_hdr->IPv6_HopByHopHdrIx - p_buf_hdr->IP_HdrIx) != NET_IPv6_HDR_SIZE) {
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvOptsSeqCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6_EXT_HOP_BY_HOP;
      break;

    case NET_IP_HDR_PROTOCOL_EXT_ROUTING:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6_EXT_ROUTING;
      p_buf_hdr->IPv6_RoutingHdrIx = protocol_ix;
      break;

    case NET_IP_HDR_PROTOCOL_EXT_FRAG:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6_EXT_FRAG;
      p_buf_hdr->IPv6_FragHdrIx = protocol_ix;
      break;

    case NET_IP_HDR_PROTOCOL_EXT_ESP:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6_EXT_ESP;
      p_buf_hdr->IPv6_ESP_HdrIx = protocol_ix;
      break;

    case NET_IP_HDR_PROTOCOL_EXT_AUTH:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6_EXT_AUTH;
      p_buf_hdr->IPv6_AuthHdrIx = protocol_ix;
      break;

    case NET_IP_HDR_PROTOCOL_EXT_NONE:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6_EXT_NONE;
#if 0
      p_buf_hdr->IPv6_NoneHdrIx = protocol_ix;
#endif
      break;

    case NET_IP_HDR_PROTOCOL_EXT_DEST:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6_EXT_DEST;
      p_buf_hdr->IPv6_DestHdrIx = protocol_ix;
      break;

    case NET_IP_HDR_PROTOCOL_EXT_MOBILITY:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6_EXT_MOBILITY;
      p_buf_hdr->IPv6_MobilityHdrIx = protocol_ix;
      break;

    default:                                                    // See Note 3.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvProtocolCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  PP_UNUSED_PARAM(p_buf);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv6_RxPktProcessExtHdr()
 *
 * @brief    Process the Extension Header(s) found in the received IPv6 packet.
 *
 * @param    p_buf   Pointer to a buffer to process the next extension header.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) If, a node encounters a Next Header value of zero (Hop-by-Hop) in any header other
 *                   than an IPv6 header or an unrecognized Next Header, it must send an ICMP Parameter Problem
 *                   message to the source of the packet, with an ICMP Code value of 1 ("unrecognized Next
 *                   Header type encountered") and the ICMP Pointer field containing the offset of the
 *                   unrecognized value within the original packet. (See RFC #2460, Section 4.0).
 *******************************************************************************************************/
static void NetIPv6_RxPktProcessExtHdr(NET_BUF  *p_buf,
                                       RTOS_ERR *p_err)
{
  NET_BUF_HDR *p_buf_hdr;
#ifdef NET_ICMPv6_MODULE_EN
  CPU_INT16U prev_protocol_ix;
#endif
  CPU_INT16U        protocol_ix;
  NET_IPv6_NEXT_HDR next_hdr;
  RTOS_ERR          local_err;

  p_buf_hdr = &p_buf->Hdr;

#ifdef NET_ICMPv6_MODULE_EN
  prev_protocol_ix = p_buf_hdr->IP_HdrIx + NET_IPv6_HDR_SIZE + p_buf_hdr->IPv6_ExtHdrLen;
#endif

  while ((p_buf_hdr->ProtocolHdrType == NET_PROTOCOL_TYPE_IP_V6_EXT_HOP_BY_HOP)
         || (p_buf_hdr->ProtocolHdrType == NET_PROTOCOL_TYPE_IP_V6_EXT_ROUTING)
         || (p_buf_hdr->ProtocolHdrType == NET_PROTOCOL_TYPE_IP_V6_EXT_FRAG)
         || (p_buf_hdr->ProtocolHdrType == NET_PROTOCOL_TYPE_IP_V6_EXT_ESP)
         || (p_buf_hdr->ProtocolHdrType == NET_PROTOCOL_TYPE_IP_V6_EXT_AUTH)
         || (p_buf_hdr->ProtocolHdrType == NET_PROTOCOL_TYPE_IP_V6_EXT_NONE)
         || (p_buf_hdr->ProtocolHdrType == NET_PROTOCOL_TYPE_IP_V6_EXT_DEST)
         || (p_buf_hdr->ProtocolHdrType == NET_PROTOCOL_TYPE_IP_V6_EXT_MOBILITY)) {
#ifdef NET_ICMPv6_MODULE_EN
    prev_protocol_ix = p_buf_hdr->IP_HdrIx + NET_IPv6_HDR_SIZE + p_buf_hdr->IPv6_ExtHdrLen;
#endif
    //                                                             Demux Ext Hdr fct.
    switch (p_buf_hdr->ProtocolHdrType) {
      case NET_PROTOCOL_TYPE_IP_V6_EXT_HOP_BY_HOP:
        protocol_ix = NetIPv6_RxOptHdr(p_buf, &next_hdr, NET_PROTOCOL_TYPE_IP_V6_EXT_HOP_BY_HOP, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
        break;

      case NET_PROTOCOL_TYPE_IP_V6_EXT_DEST:
        protocol_ix = NetIPv6_RxOptHdr(p_buf, &next_hdr, NET_PROTOCOL_TYPE_IP_V6_EXT_DEST, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
        break;

      case NET_PROTOCOL_TYPE_IP_V6_EXT_ROUTING:
        protocol_ix = NetIPv6_RxRoutingHdr(p_buf, &next_hdr, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
        break;

      case NET_PROTOCOL_TYPE_IP_V6_EXT_FRAG:
        protocol_ix = NetIPv6_RxFragHdr(p_buf, &next_hdr);
        break;

      case NET_PROTOCOL_TYPE_IP_V6_EXT_ESP:
        protocol_ix = NetIPv6_RxESP_Hdr(p_buf, &next_hdr, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
        break;

      case NET_PROTOCOL_TYPE_IP_V6_EXT_AUTH:
        protocol_ix = NetIPv6_RxAuthHdr(p_buf, &next_hdr);
        break;

      case NET_PROTOCOL_TYPE_IP_V6_EXT_MOBILITY:
        protocol_ix = NetIPv6_RxMobilityHdr(p_buf, &next_hdr, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
        break;

      case NET_PROTOCOL_TYPE_IP_V6_EXT_NONE:
        goto exit;

      default:
#ifdef NET_ICMPv6_MODULE_EN
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        //                                                         Send ICMP Parameter Problem Msg (See Note 2).
        NetICMPv6_TxMsgErr(p_buf,
                           NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                           NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_NEXT_HDR,
                           (prev_protocol_ix - p_buf_hdr->IP_HdrIx),
                           &local_err);
#endif
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvOptsCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
    }

    NetIPv6_RxPktValidateNextHdr(p_buf,
                                 p_buf_hdr,
                                 next_hdr,
                                 protocol_ix,
                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvProtocolCtr);
#ifdef  NET_ICMPv6_MODULE_EN
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetICMPv6_TxMsgErr(p_buf,
                         NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                         NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_NEXT_HDR,
                         (prev_protocol_ix - p_buf_hdr->IP_HdrIx),
                         &local_err);
#endif
      goto exit;
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIPv6_RxOptHdr()
 *
 * @brief    Validate and process a received Hop-By-Hop or Destination Header.
 *
 * @param    p_buf       Pointer to network buffer that received IPv6 datagram.
 *
 * @param    p_next_hdr  Pointer to variable that will receive the next header from this function.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Index of the next extension header or upper layer protocol, if NO error(s).
 *           0u                                                          otherwise.
 *
 * @note     (1) Hop-by-Hop extension header and Destination header have an Option field based on the
 *               TLV model (Type-Length-Value). See 'net_ipv6.h' EXTENSION HEADER TLV OPTION DATA TYPE.
 *******************************************************************************************************/
static CPU_INT16U NetIPv6_RxOptHdr(NET_BUF           *p_buf,
                                   NET_IPv6_NEXT_HDR *p_next_hdr,
                                   NET_PROTOCOL_TYPE proto_type,
                                   RTOS_ERR          *p_err)
{
#if 0
  CPU_BOOLEAN change_en;
#endif
  CPU_INT08U           *p_data;
  NET_BUF_HDR          *p_buf_hdr;
  NET_IPv6_OPT_HDR     *p_opt_hdr;
  NET_IPv6_EXT_HDR_TLV *p_tlv;
  CPU_INT16U           ext_hdr_ix;
  NET_IPv6_TLV_TYPE    action;
  NET_IPv6_OPT_TYPE    opt_type;
  CPU_INT16U           eh_len;
  CPU_INT16U           next_hdr_offset = 0u;
  CPU_INT16U           next_tlv_offset;
  CPU_BOOLEAN          dest_addr_multicast;
  RTOS_ERR             local_err;

  p_buf_hdr = &p_buf->Hdr;                                      // Get ptr to buf hdr.

  switch (proto_type) {
    case NET_PROTOCOL_TYPE_IP_V6_EXT_HOP_BY_HOP:
      ext_hdr_ix = p_buf_hdr->IPv6_HopByHopHdrIx;
      break;

    case NET_PROTOCOL_TYPE_IP_V6_EXT_DEST:
      ext_hdr_ix = p_buf_hdr->IPv6_DestHdrIx;
      break;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvOptsCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  p_data = p_buf->DataPtr;
  //                                                               Get opt hdr from ext hdr data space.
  p_opt_hdr = (NET_IPv6_OPT_HDR *)&p_data[ext_hdr_ix];

  eh_len = (p_opt_hdr->HdrLen + 1) * NET_IPv6_EH_ALIGN_SIZE;
  p_buf_hdr->DataLen -= eh_len;
  p_buf_hdr->IPv6_ExtHdrLen += eh_len;

  next_hdr_offset = ext_hdr_ix + eh_len;
  next_tlv_offset = 0u;

  while (next_tlv_offset < (eh_len - 2u)) {
    p_tlv = (NET_IPv6_EXT_HDR_TLV *)&p_opt_hdr->Opt[next_tlv_offset];

    action = p_tlv->Type & NET_IPv6_EH_TLV_TYPE_ACT_MASK;
#if 0
    change_en = (p_tlv->Type & NET_IPv6_EH_TLV_TYPE_CHG_MASK) >> NET_IPv6_EH_TLV_TYPE_CHG_SHIFT;
#endif
    opt_type = p_tlv->Type & NET_IPv6_EH_TLV_TYPE_OPT_MASK;

    switch (opt_type) {
      case NET_IPv6_EH_TYPE_PAD1:
        break;

      case NET_IPv6_EH_TYPE_PADN:
        break;

      case NET_IPv6_EH_TYPE_ROUTER_ALERT:
        break;

      default:
        switch (action) {
          //                                                       Skip over opt & continue processing hdr.
          case NET_IPv6_EH_TLV_TYPE_ACT_SKIP:
            break;

          //                                                       Discard pkt.
          case NET_IPv6_EH_TLV_TYPE_ACT_DISCARD:
            NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvOptsCtr);
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;

          //                                                       Discard pkt & send ICMP Param Problem code 2 msg.
          case NET_IPv6_EH_TLV_TYPE_ACT_DISCARD_IPPM:
#ifdef  NET_ICMPv6_MODULE_EN
            RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
            NetICMPv6_TxMsgErr(p_buf,
                               NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                               NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_OPT,
                               ((ext_hdr_ix + 2) - p_buf_hdr->IP_HdrIx),
                               &local_err);
#endif
            NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvOptsCtr);
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
          //                                                       Discard pkt & send ICMP Param Problem code 2 msg ...
          //                                                       ... if pkt dest is not multicast.
          case NET_IPv6_EH_TLV_TYPE_ACT_DISCARD_IPPM_MC:
            p_buf_hdr = &p_buf->Hdr;
            dest_addr_multicast = NetIPv6_IsAddrMcast(&p_buf_hdr->IPv6_AddrDest);
            if (dest_addr_multicast == DEF_NO) {
#ifdef  NET_ICMPv6_MODULE_EN
              RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
              NetICMPv6_TxMsgErr(p_buf,
                                 NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                                 NET_ICMPv6_MSG_CODE_PARAM_PROB_BAD_OPT,
                                 ((ext_hdr_ix + 2) - p_buf_hdr->IP_HdrIx),
                                 &local_err);
#endif
            }
            NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvOptsCtr);
            RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
            goto exit;
        }
        break;
    }

    if (opt_type == NET_IPv6_EH_TYPE_PAD1) {                    // The format of the Pad1 opt is a special case, it ...
      next_tlv_offset++;                                        // ... doesn't have len and value fields.
    } else {
      next_tlv_offset += p_tlv->Len + 2u;
    }
  }

  *p_next_hdr = p_opt_hdr->NextHdr;

exit:
  return (next_hdr_offset);
}

/****************************************************************************************************//**
 *                                           NetIPv6_RxRoutingHdr()
 *
 * @brief    Process the received Routing extension header.
 *
 * @param    p_buf       Pointer to Received buffer.
 *
 * @param    p_next_hdr  Pointer to IPv6 next header object.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Index in buffer of the IPv6 Next header.
 *
 * @note     (1) Routing Extension Header is not yet supported. Therefore the function does not
 *               process the header and only set the pointer to the next header.
 *******************************************************************************************************/
static CPU_INT16U NetIPv6_RxRoutingHdr(NET_BUF           *p_buf,
                                       NET_IPv6_NEXT_HDR *p_next_hdr,
                                       RTOS_ERR          *p_err)
{
  CPU_INT08U           *p_data;
  NET_BUF_HDR          *p_buf_hdr;
  NET_IPv6_ROUTING_HDR *prouting_hdr;
  CPU_INT16U           next_protocol_ix = 0u;
  CPU_INT16U           hdr_len;
  RTOS_ERR             local_err;

  p_buf_hdr = &p_buf->Hdr;                                      // Get ptr to buf hdr.
  p_data = p_buf->DataPtr;

  //                                                               Get routing hdr from pkt data space.
  prouting_hdr = (NET_IPv6_ROUTING_HDR *)&p_data[p_buf_hdr->IPv6_RoutingHdrIx];
  hdr_len = (prouting_hdr->HdrLen + 1) * NET_IPv6_EH_ALIGN_SIZE;

  //                                                               Calculate next hdr ptr.
  next_protocol_ix = p_buf_hdr->IPv6_RoutingHdrIx + hdr_len;
  p_buf_hdr->DataLen -= hdr_len;
  p_buf_hdr->IPv6_ExtHdrLen += hdr_len;

  switch (prouting_hdr->RoutingType) {
    case NET_IPv6_EH_ROUTING_TYPE_0:
    case NET_IPv6_EH_ROUTING_TYPE_1:
    case NET_IPv6_EH_ROUTING_TYPE_2:
      break;                                                    // No processing of Routing Header for now.

    default:
      if (prouting_hdr->SegLeft != 0) {
#ifdef  NET_ICMPv6_MODULE_EN
        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        NetICMPv6_TxMsgErr(p_buf,
                           NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                           NET_ICMPv6_MSG_CODE_PARAM_PROB_IP_HDR,
                           ((p_buf_hdr->IPv6_RoutingHdrIx + 2) - p_buf_hdr->IP_HdrIx),
                           &local_err);
#endif
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvOptsCtr);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }
  }

  *p_next_hdr = prouting_hdr->NextHdr;

exit:
  return (next_protocol_ix);
}

/****************************************************************************************************//**
 *                                           NetIPv6_RxFragHdr()
 *
 * @brief    Process the received Fragment extension header.
 *
 * @param    p_buf       Pointer to network buffer that received IPv6 datagram.
 *
 * @param    p_next_hdr  Pointer to IPv6 next header object.
 *
 * @return   Index in buffer of the IPv6 Next header.
 *******************************************************************************************************/
static CPU_INT16U NetIPv6_RxFragHdr(NET_BUF           *p_buf,
                                    NET_IPv6_NEXT_HDR *p_next_hdr)
{
  CPU_INT08U        *p_data;
  NET_BUF_HDR       *p_buf_hdr;
  NET_IPv6_FRAG_HDR *p_frag_hdr;
  CPU_INT16U        next_protocol_ix;

  p_buf_hdr = &p_buf->Hdr;                                        // Get ptr to buf hdr.
  p_data = p_buf->DataPtr;

  //                                                               Get frag hdr from pkt data space.
  p_frag_hdr = (NET_IPv6_FRAG_HDR *)&p_data[p_buf_hdr->IPv6_FragHdrIx];

  //                                                               Get frag flag & ID.
  p_buf_hdr->IPv6_Flags_FragOffset = NET_UTIL_NET_TO_HOST_16(p_frag_hdr->FragOffsetFlag);
  p_buf_hdr->IPv6_ID = NET_UTIL_NET_TO_HOST_32(p_frag_hdr->ID);

  //                                                               Calculate next hdr ptr.
  next_protocol_ix = p_buf_hdr->IPv6_FragHdrIx + NET_IPv6_FRAG_HDR_SIZE;
  p_buf_hdr->DataLen -= NET_IPv6_FRAG_HDR_SIZE;
  p_buf_hdr->IPv6_ExtHdrLen += NET_IPv6_FRAG_HDR_SIZE;

  *p_next_hdr = p_frag_hdr->NextHdr;

  return (next_protocol_ix);
}

/****************************************************************************************************//**
 *                                           NetIPv6_RxESP_Hdr()
 *
 * @brief    Process the received ESP (Encapsulation Security Payload) extension header.
 *
 * @param    p_buf       Pointer to network buffer that received IPv6 datagram.
 *
 * @param    p_next_hdr  Pointer to IPv6 next header object.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *
 * @return   Index in buffer of the IPv6 Next header.
 *
 * @note     (1) ESP Extension Header is not yet supported.
 *******************************************************************************************************/
static CPU_INT16U NetIPv6_RxESP_Hdr(NET_BUF           *p_buf,
                                    NET_IPv6_NEXT_HDR *p_next_hdr,
                                    RTOS_ERR          *p_err)
{
  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(p_next_hdr);

  NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxOptsUnsupportedCtr);
  RTOS_ERR_SET(*p_err, RTOS_ERR_RX);

  return (0u);
}

/****************************************************************************************************//**
 *                                           NetIPv6_RxAuthHdr()
 *
 * @brief    Process the received Authentication Extension Header.
 *
 * @param    p_buf       Pointer to network buffer that received IPv6 datagram.
 *
 * @param    p_next_hdr  Pointer to IPv6 next header object.
 *
 * @return   Index in buffer of the IPv6 Next header.
 *
 * @note     (1) Authentication Extension Header is not yet supported. Therefore the function does not
 *               process the header and only set the pointer to the next header.
 *******************************************************************************************************/
static CPU_INT16U NetIPv6_RxAuthHdr(NET_BUF           *p_buf,
                                    NET_IPv6_NEXT_HDR *p_next_hdr)
{
  NET_BUF_HDR                 *p_buf_hdr;
  CPU_INT08U                  *p_data;
  NET_IPv6_AUTHENTICATION_HDR *pauth_hdr;
  CPU_INT16U                  eh_len;
  CPU_INT16U                  next_hdr_offset;

  //                                                               Get ptr to buf hdr.
  p_buf_hdr = &p_buf->Hdr;
  p_data = p_buf->DataPtr;

  //                                                               Get Auth hdr from pkt data space.
  pauth_hdr = (NET_IPv6_AUTHENTICATION_HDR *)&p_data[p_buf_hdr->IP_HdrIx];

  eh_len = ((pauth_hdr->HdrLen + 2) >> 1) * NET_IPv6_EH_ALIGN_SIZE;
  p_buf_hdr->DataLen -= eh_len;
  p_buf_hdr->IPv6_ExtHdrLen += eh_len;

  next_hdr_offset = p_buf_hdr->IPv6_AuthHdrIx + eh_len;

  *p_next_hdr = pauth_hdr->NextHdr;

  return (next_hdr_offset);
}

/****************************************************************************************************//**
 *                                           NetIPv6_RxNoneHdr()
 *
 * @brief    Process the received None Extension Header.
 *
 * @param    p_buf       Pointer to network buffer that received IPv6 datagram.
 *
 * @param    p_next_hdr  Pointer to IPv6 next header object.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *
 * @return   Index in buffer of the IPv6 Next header.
 *******************************************************************************************************/
#if 0
static CPU_INT16U NetIPv6_RxNoneHdr(NET_BUF           *p_buf,
                                    NET_CTR_ERR_INC(Net_ErrCtrs.NetIPv6_ErrRxHdrOptsCtr);
                                    NET_IPv6_NEXT_HDR *p_next_hdr,
                                    RTOS_ERR          *p_err)
{
  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(p_next_hdr);
  PP_UNUSED_PARAM(p_err);

  return (0u);
}
#endif

/****************************************************************************************************//**
 *                                           NetIPv6_RxMobilityHdr()
 *
 * @brief    Process the received Mobility Extension Header.
 *
 * @param    p_buf       Pointer to network buffer that received IPv6 datagram.
 *
 * @param    p_next_hdr  Pointer to IPv6 next header object.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function :
 *
 * @return   Index in buffer of the IPv6 Next header.
 *
 * @note     (1) IPv6 Mobility Extension header is not yet supported.
 *******************************************************************************************************/
static CPU_INT16U NetIPv6_RxMobilityHdr(NET_BUF           *p_buf,
                                        NET_IPv6_NEXT_HDR *p_next_hdr,
                                        RTOS_ERR          *p_err)
{
  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(p_next_hdr);

  NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxOptsUnsupportedCtr);
  RTOS_ERR_SET(*p_err, RTOS_ERR_RX);

  return (0u);
}

/****************************************************************************************************//**
 *                                           NetIPv6_RxPktFragReasm()
 *
 * @brief    (1) Reassemble any IPv6 datagram fragments :
 *
 *           - (a) Determine if received IPv6 packet is a fragment
 *           - (b) Reassemble IPv6 fragments, when possible
 *
 * @param    p_buf       Pointer to network buffer that received IPv6 packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_ip_hdr    Pointer to received packet's IPv6 header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to reassembled datagram, if fragment reassembly complete.
 *
 *           Pointer to NULL,                 if fragment reassembly in progress.
 *
 *           Pointer to fragment buffer,      for any fragment discard error.
 *
 * @note     (3) See RFC #2460, Section 4.5 'Fragment Header'
 *
 * @note     (4) Fragment lists are accessed by :
 *
 *           - (a) NetIPv6_RxPktFragReasm()
 *           - (b) Fragment list's 'TMR->Obj' pointer      during execution of NetIPv6_RxPktFragTimeout()
 *
 * @note     (5) Since the primary tasks of the network protocol suite are prevented from running
 *               concurrently (see 'net.h  Note #3'), it is NOT necessary to protect the shared
 *               resources of the fragment lists since no asynchronous access from other network
 *               tasks is possible.
 *******************************************************************************************************/
static NET_BUF *NetIPv6_RxPktFragReasm(NET_BUF      *p_buf,
                                       NET_BUF_HDR  *p_buf_hdr,
                                       NET_IPv6_HDR *p_ip_hdr,
                                       CPU_BOOLEAN  *p_is_frag,
                                       RTOS_ERR     *p_err)
{
#if 0
  NET_IPv6_HDR *p_frag_list_ip_hdr;
#endif
  CPU_BOOLEAN frag;
  CPU_BOOLEAN frag_done;
  CPU_BOOLEAN ip_flag_frags_more;
  CPU_BOOLEAN addr_cmp;
  CPU_INT16U  ip_flags;
  CPU_INT16U  frag_offset;
  CPU_INT16U  frag_size;
  NET_BUF     *p_frag = DEF_NULL;
  NET_BUF     *p_frag_list = DEF_NULL;
  NET_BUF_HDR *p_frag_list_buf_hdr = DEF_NULL;

  //                                                               -------------- CHK FRAG REASM REQUIRED -------------
  frag = DEF_NO;

  ip_flags = p_buf_hdr->IPv6_Flags_FragOffset & NET_IPv6_FRAG_FLAGS_MASK;
  ip_flag_frags_more = DEF_BIT_IS_SET(ip_flags, NET_IPv6_FRAG_FLAG_FRAG_MORE);
  if (ip_flag_frags_more != DEF_NO) {                           // If 'More Frags' set (see Note #3), ...
    frag = DEF_YES;                                             // ... mark as frag.
  }

  frag_offset = p_buf_hdr->IPv6_Flags_FragOffset & NET_IPv6_FRAG_OFFSET_MASK;
  if (frag_offset != NET_IPv6_FRAG_OFFSET_NONE) {               // If frag offset != 0 (see Note #3), ...
    frag = DEF_YES;                                             // ... mark as frag.
  }

  if (frag != DEF_YES) {                                        // If pkt NOT a frag, ...
    *p_is_frag = DEF_NO;
    p_frag = p_buf;                                             // ... rtn non-frag'd datagram (see Note #3).
    goto exit;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.IPv6.RxFragCtr);

  *p_is_frag = DEF_YES;

  //                                                               ------------------- REASM FRAGS --------------------
  frag_size = p_buf_hdr->IP_TotLen - p_buf_hdr->IPv6_ExtHdrLen;
  p_frag_list = NetIPv6_FragReasmListsHead;
  frag_done = DEF_NO;

  while (frag_done == DEF_NO) {                                 // Insert frag into a frag list.
    if (p_frag_list != DEF_NULL) {                              // Srch ALL existing frag lists first.
      p_frag_list_buf_hdr = &p_frag_list->Hdr;
#if 0
      p_frag_list_ip_hdr = (NET_IPv6_HDR *)&p_frag_list->DataPtr[p_frag_list_buf_hdr->IP_HdrIx];
#endif
      //                                                           If frag & this frag list's    ...

      addr_cmp = Mem_Cmp(&p_buf_hdr->IPv6_AddrSrc, &p_frag_list_buf_hdr->IPv6_AddrSrc, NET_IPv6_ADDR_SIZE);

      if (addr_cmp == DEF_YES) {                                                // ... src  addr ...
        addr_cmp = Mem_Cmp(&p_buf_hdr->IPv6_AddrDest, &p_frag_list_buf_hdr->IPv6_AddrDest, NET_IPv6_ADDR_SIZE);
        if (addr_cmp == DEF_YES) {                                              // ... dest addr ...
          if (p_buf_hdr->IPv6_ID == p_frag_list_buf_hdr->IPv6_ID) {             // ... ID ...
#if 0
            if (p_ip_hdr->NextHdr == p_frag_list_ip_hdr->NextHdr) {             // ... next hdr ...
                                                                                // ... fields identical,         ...
#endif
            p_frag = NetIPv6_RxPktFragListInsert(p_buf,                         // ... insert frag into frag list.
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
#if 0
          }
#endif
          }
        }
      }

      if (frag_done != DEF_YES) {                               // If NOT done, adv to next frag list.
        p_frag_list = p_frag_list_buf_hdr->NextPrimListPtr;
      }
    } else {                                                    // Else add new frag list).
      NetIPv6_RxPktFragListAdd(p_buf,
                               p_buf_hdr,
                               ip_flags,
                               frag_offset,
                               frag_size,
                               p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        p_frag = DEF_NULL;
        goto exit;
      }

      frag_done = DEF_YES;
    }
  }

  PP_UNUSED_PARAM(p_ip_hdr);

exit:
  return (p_frag);
}

/****************************************************************************************************//**
 *                                       NetIPv6_RxPktFragListAdd()
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
 * @param    frag_ip_flags   Fragment IPv6 header flags.
 *
 * @param    frag_offset     Fragment offset.
 *
 * @param    frag_size       Fragment size (in octets).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to NULL,            if fragment added as new fragment list.
 *           Pointer to fragment buffer, for any fragment discard error.
 *
 * @note     (2) (a) RFC #2460, Section 4.5 'Fragment Header' states that "if an internet
 *                   datagram is fragmented" :
 *
 *               - (1) (A) "Fragments are counted in units of 8 octets."
 *                   - (B) "The minimum fragment is 8 octets."
 *
 *               - (2) (A) However, this CANNOT apply "if this is the last fragment" ...
 *                       - (1) "(that is the more fragments field is zero)";         ...
 *                   - (B) Which may be of ANY size.
 *
 * @note     (3) During fragment list insertion, some fragment buffer controls were previously initialized
 *               in NetBuf_Get() when the packet was received at the network interface layer.  These buffer
 *               controls do NOT need to be re-initialized but are shown for completeness.
 *******************************************************************************************************/
static void NetIPv6_RxPktFragListAdd(NET_BUF     *p_buf,
                                     NET_BUF_HDR *p_buf_hdr,
                                     CPU_INT16U  frag_ip_flags,
                                     CPU_INT32U  frag_offset,
                                     CPU_INT32U  frag_size,
                                     RTOS_ERR    *p_err)
{
  CPU_BOOLEAN ip_flag_frags_more;
  CPU_INT16U frag_size_min;
  CPU_INT32U timeout_ms;
  NET_BUF_HDR *p_frag_list_tail_buf_hdr;
  RTOS_ERR local_err;
  CORE_DECLARE_IRQ_STATE;

  ip_flag_frags_more = DEF_BIT_IS_SET(frag_ip_flags, NET_IPv6_FRAG_FLAG_FRAG_MORE);
  frag_size_min = (ip_flag_frags_more == DEF_YES) ? NET_IPv6_FRAG_SIZE_MIN_FRAG_MORE
                  : NET_IPv6_FRAG_SIZE_MIN_FRAG_LAST;
  if (frag_size < frag_size_min) {                              // See Note #2a.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragSizeCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (frag_size > NET_IPv6_FRAG_SIZE_MAX) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragSizeCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if ((ip_flag_frags_more == DEF_YES)
      && ((frag_size % NET_IPv6_FRAG_SIZE_UNIT) != 0u)) {
#ifdef  NET_ICMPv6_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetICMPv6_TxMsgErr(p_buf,
                       NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                       NET_ICMPv6_MSG_CODE_PARAM_PROB_IP_HDR,
                       NET_ICMPv6_PTR_IX_IP_PAYLOAD_LEN,
                       &local_err);
#endif
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragSizeCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------------------- GET FRAG TMR -------------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  CORE_ENTER_ATOMIC();
  timeout_ms = NetIPv6_FragReasmTimeout_ms;
  CORE_EXIT_ATOMIC();
  p_buf_hdr->TmrPtr = NetTmr_Get(NetIPv6_RxPktFragTimeout,
                                 p_buf,
                                 timeout_ms,
                                 NET_TMR_OPT_NONE,
                                 &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // If tmr unavail, discard frag.
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               ------------ INSERT FRAG INTO FRAG LISTS -----------
  if (NetIPv6_FragReasmListsTail != DEF_NULL) {                 // If frag lists NOT empty, insert @ tail.
    p_buf_hdr->PrevPrimListPtr = NetIPv6_FragReasmListsTail;
    p_frag_list_tail_buf_hdr = &NetIPv6_FragReasmListsTail->Hdr;
    p_frag_list_tail_buf_hdr->NextPrimListPtr = p_buf;
    NetIPv6_FragReasmListsTail = p_buf;
  } else {                                                      // Else add frag as first frag list.
    NetIPv6_FragReasmListsHead = p_buf;
    NetIPv6_FragReasmListsTail = p_buf;
    p_buf_hdr->PrevPrimListPtr = DEF_NULL;
  }

#if 0                                                           // Init'd in NetBuf_Get() [see Note #3].
  p_buf_hdr->NextPrimListPtr = DEF_NULL;
  p_buf_hdr->PrevBufPtr = DEF_NULL;
  p_buf_hdr->NextBufPtr = DEF_NULL;
  p_buf_hdr->IP_FragSizeTot = NET_IPv6_FRAG_SIZE_NONE;
  p_buf_hdr->IP_FragSizeCur = 0u;
#endif

  //                                                               ----------------- UPDATE FRAG CALCS ----------------
  NetIPv6_RxPktFragListUpdate(p_buf,
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
 *                                       NetIPv6_RxPktFragListInsert()
 *
 * @brief    (1) Insert fragment into corresponding fragment list.
 *
 *           - (a) Fragments are sorted into fragment lists by fragment offset.
 *
 * @param    p_buf           Pointer to network buffer that received fragment.
 *
 * @param    p_buf_hdr       Pointer to network buffer header.
 *
 * @param    frag_ip_flags   Fragment IPv6 header flags.
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
 *
 *           Pointer to NULL,                 if fragment reassembly in progress.
 *
 *           Pointer to fragment buffer,      for any fragment discard error.
 *
 * @note     (2) Assumes ALL fragments in fragment lists have previously been validated for buffer &
 *               IPv6 header fields.
 *
 * @note     (3) During fragment list insertion, some fragment buffer controls were previously
 *               initialized in NetBuf_Get() when the packet was received at the network interface
 *               layer.  These buffer controls do NOT need to be re-initialized but are shown for
 *               completeness.
 *******************************************************************************************************/
static NET_BUF *NetIPv6_RxPktFragListInsert(NET_BUF     *p_buf,
                                            NET_BUF_HDR *p_buf_hdr,
                                            CPU_INT16U  frag_ip_flags,
                                            CPU_INT32U  frag_offset,
                                            CPU_INT32U  frag_size,
                                            NET_BUF     *p_frag_list,
                                            RTOS_ERR    *p_err)
{
#if 0
  CPU_INT16U frag_size_cur;
#endif
  CPU_BOOLEAN ip_flag_frags_more;
  CPU_BOOLEAN frag_insert_done;
  CPU_BOOLEAN frag_list_discard;
  CPU_INT16U frag_offset_actual;
  CPU_INT16U frag_list_cur_frag_offset;
  CPU_INT16U frag_list_cur_frag_offset_actual;
  CPU_INT16U frag_list_prev_frag_offset;
  CPU_INT16U frag_list_prev_frag_offset_actual;
  CPU_INT16U frag_size_min;
  NET_BUF *p_frag;
  NET_BUF *p_frag_list_prev_buf;
  NET_BUF *p_frag_list_cur_buf;
  NET_BUF *p_frag_list_prev_list;
  NET_BUF *p_frag_list_next_list;
  NET_BUF_HDR *p_frag_list_buf_hdr;
  NET_BUF_HDR *p_frag_list_prev_buf_hdr;
  NET_BUF_HDR *p_frag_list_cur_buf_hdr;
  NET_BUF_HDR *p_frag_list_prev_list_buf_hdr;
  NET_BUF_HDR *p_frag_list_next_list_buf_hdr;
  NET_TMR *p_tmr;
  RTOS_ERR local_err;

  ip_flag_frags_more = DEF_BIT_IS_SET(frag_ip_flags, NET_IPv6_FRAG_FLAG_FRAG_MORE);
  frag_size_min = (ip_flag_frags_more == DEF_YES) ? NET_IPv6_FRAG_SIZE_MIN_FRAG_MORE
                  : NET_IPv6_FRAG_SIZE_MIN_FRAG_LAST;
  if (frag_size < frag_size_min) {                              // See Note #2a1.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragSizeCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    p_frag = DEF_NULL;
    goto exit;
  }

  if (frag_size > NET_IPv6_FRAG_SIZE_MAX) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragSizeCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    p_frag = DEF_NULL;
    goto exit;
  }

  if ((ip_flag_frags_more == DEF_YES)
      && ((frag_size % NET_IPv6_FRAG_SIZE_UNIT) != 0u)) {
#ifdef  NET_ICMPv6_MODULE_EN
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    NetICMPv6_TxMsgErr(p_buf,
                       NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                       NET_ICMPv6_MSG_CODE_PARAM_PROB_IP_HDR,
                       NET_ICMPv6_PTR_IX_IP_PAYLOAD_LEN,
                       &local_err);
#endif
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragSizeCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    p_frag = DEF_NULL;
    goto exit;
  }

  //                                                               ------- INSERT FRAG INTO FRAG LISTS --------
  frag_insert_done = DEF_NO;

  p_frag_list_cur_buf = p_frag_list;
  p_frag_list_cur_buf_hdr = &p_frag_list_cur_buf->Hdr;

  while (frag_insert_done == DEF_NO) {
    frag_list_cur_frag_offset = p_frag_list_cur_buf_hdr->IPv6_Flags_FragOffset & NET_IPv6_FRAG_OFFSET_MASK;
    if (frag_offset > frag_list_cur_frag_offset) {                      // While frag offset > cur frag offset, ...
      if (p_frag_list_cur_buf_hdr->NextBufPtr != DEF_NULL) {            // ... adv to next frag in list.
        p_frag_list_cur_buf = p_frag_list_cur_buf_hdr->NextBufPtr;
        p_frag_list_cur_buf_hdr = &p_frag_list_cur_buf->Hdr;
      } else {                                                          // If @ last frag in list, append frag @ end.
        frag_offset_actual = frag_offset;
        frag_list_cur_frag_offset_actual = frag_list_cur_frag_offset
                                           + (p_frag_list_cur_buf_hdr->IP_TotLen - p_frag_list_cur_buf_hdr->IPv6_ExtHdrLen);

        if (frag_offset_actual >= frag_list_cur_frag_offset_actual) {           // If frag does NOT overlap, ...
                                                                                // ... append @ end of frag list.
          p_buf_hdr->PrevBufPtr = p_frag_list_cur_buf;
#if 0                                                                   // Init'd in NetBuf_Get() [see Note #4].
          p_buf_hdr->NextBufPtr = DEF_NULL;
          p_buf_hdr->PrevPrimListPtr = DEF_NULL;
          p_buf_hdr->NextPrimListPtr = DEF_NULL;
          p_buf_hdr->TmrPtr = DEF_NULL;
          p_buf_hdr->IP_FragSizeTot = NET_IPv6_FRAG_SIZE_NONE;
          p_buf_hdr->IP_FragSizeCur = 0u;
#endif

          p_frag_list_cur_buf_hdr->NextBufPtr = p_buf;

          p_frag_list_buf_hdr = &p_frag_list->Hdr;
          NetIPv6_RxPktFragListUpdate(p_frag_list,                      // Update frag list reasm calcs.
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
          p_frag = NetIPv6_RxPktFragListChkComplete(p_frag_list,
                                                    p_frag_list_buf_hdr,
                                                    p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            p_frag = DEF_NULL;
            goto exit;
          }
        } else {                                                        // Else discard overlap frag & datagram.
          NetIPv6_RxPktFragListDiscard(p_frag_list, DEF_YES);
          NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragDisCtr);
          p_frag = DEF_NULL;
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }

        frag_insert_done = DEF_YES;
      }
    } else if (frag_offset < frag_list_cur_frag_offset) {               // If frag offset < cur frag offset, ...
                                                                        // ... insert frag into frag list.
      frag_list_discard = DEF_NO;

      frag_offset_actual = frag_offset + frag_size;
      frag_list_cur_frag_offset_actual = frag_list_cur_frag_offset;

      if (frag_offset_actual > frag_list_cur_frag_offset_actual) {      // If frag overlaps with next frag, ...
        frag_list_discard = DEF_YES;                                    // ... discard frag & datagram (see Note #1b2).
      } else if (p_frag_list_cur_buf_hdr->PrevBufPtr != DEF_NULL) {
        p_frag_list_prev_buf = p_frag_list_cur_buf_hdr->PrevBufPtr;
        p_frag_list_prev_buf_hdr = &p_frag_list_prev_buf->Hdr;

        frag_offset_actual = frag_offset;

        frag_list_prev_frag_offset = p_frag_list_prev_buf_hdr->IPv6_Flags_FragOffset & NET_IPv6_FRAG_OFFSET_MASK;
        frag_list_prev_frag_offset_actual = frag_list_prev_frag_offset
                                            + (p_frag_list_prev_buf_hdr->IP_TotLen - p_frag_list_prev_buf_hdr->IP_HdrLen);
        //                                                         If frag overlaps with prev frag, ...
        if (frag_offset_actual < frag_list_prev_frag_offset_actual) {
          frag_list_discard = DEF_YES;                                  // ... discard frag & datagram (see Note #1b2).
        }
      }

      if (frag_list_discard == DEF_NO) {                                // If frag does NOT overlap, ...
                                                                        // ... insert into frag list.
        p_buf_hdr->PrevBufPtr = p_frag_list_cur_buf_hdr->PrevBufPtr;
        p_buf_hdr->NextBufPtr = p_frag_list_cur_buf;

        if (p_buf_hdr->PrevBufPtr != DEF_NULL) {                        // Insert p_buf between prev & cur bufs.
          p_frag_list_prev_buf = p_buf_hdr->PrevBufPtr;
          p_frag_list_prev_buf_hdr = &p_frag_list_prev_buf->Hdr;

          p_frag_list_prev_buf_hdr->NextBufPtr = p_buf;
          p_frag_list_cur_buf_hdr->PrevBufPtr = p_buf;

#if 0                                                                   // Init'd in NetBuf_Get() [see Note #4].
          p_buf_hdr->PrevPrimListPtr = DEF_NULL;
          p_buf_hdr->NextPrimListPtr = DEF_NULL;
          p_buf_hdr->TmrPtr = DEF_NULL;
          p_buf_hdr->IP_FragSizeTot = NET_IPv6_FRAG_SIZE_NONE;
          p_buf_hdr->IP_FragSizeCur = 0u;
#endif
        } else {                                                        // Else p_buf is new frag list head.
          p_frag_list = p_buf;
          //                                                       Move frag list head info to cur buf ...
          //                                                       ... (see Note #2b1).
          p_buf_hdr->PrevPrimListPtr = p_frag_list_cur_buf_hdr->PrevPrimListPtr;
          p_buf_hdr->NextPrimListPtr = p_frag_list_cur_buf_hdr->NextPrimListPtr;
          p_buf_hdr->TmrPtr = p_frag_list_cur_buf_hdr->TmrPtr;
          p_buf_hdr->IP_FragSizeTot = p_frag_list_cur_buf_hdr->IP_FragSizeTot;
          p_buf_hdr->IP_FragSizeCur = p_frag_list_cur_buf_hdr->IP_FragSizeCur;

          p_frag_list_cur_buf_hdr->PrevBufPtr = p_buf;
          p_frag_list_cur_buf_hdr->PrevPrimListPtr = DEF_NULL;
          p_frag_list_cur_buf_hdr->NextPrimListPtr = DEF_NULL;
          p_frag_list_cur_buf_hdr->TmrPtr = DEF_NULL;
          p_frag_list_cur_buf_hdr->IP_FragSizeTot = NET_IPv6_FRAG_SIZE_NONE;
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
            NetIPv6_FragReasmListsHead = p_buf;
          }

          //                                                       Point next frag list to new frag list head.
          p_frag_list_next_list = p_buf_hdr->NextPrimListPtr;
          if (p_frag_list_next_list != DEF_NULL) {
            p_frag_list_next_list_buf_hdr = &p_frag_list_next_list->Hdr;
            p_frag_list_next_list_buf_hdr->PrevPrimListPtr = p_buf;
          } else {
            NetIPv6_FragReasmListsTail = p_buf;
          }
        }

        p_frag_list_buf_hdr = &p_frag_list->Hdr;
        NetIPv6_RxPktFragListUpdate(p_frag_list,                        // Update frag list reasm calcs.
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
        p_frag = NetIPv6_RxPktFragListChkComplete(p_frag_list,
                                                  p_frag_list_buf_hdr,
                                                  p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          p_frag = DEF_NULL;
          goto exit;
        }
      } else {                                                          // Else discard overlap frag & datagram ...
        NetIPv6_RxPktFragListDiscard(p_frag_list, DEF_YES);             // ... (see Note #1b2).
        NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragDisCtr);
        p_frag = DEF_NULL;
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }

      frag_insert_done = DEF_YES;
    } else {                                                            // Else if frag offset = cur frag offset, ...
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragDisCtr);
      p_frag = DEF_NULL;
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                                    // ... discard duplicate frag (see Note #1b1).

#if 0
      frag_size_cur = p_frag_list_cur_buf_hdr->IP_TotLen - p_frag_list_cur_buf_hdr->IP_HdrLen;
      if (frag_size != frag_size_cur) {                                 // If frag size != cur frag size,    ...
        NetIPv6_RxPktFragListDiscard(p_frag_list, DEF_YES);             // ... discard overlap frag datagram ...
                                                                        // ... (see Note #1b2).
      }
#endif
      frag_insert_done = DEF_YES;

      goto exit;
    }
  }

exit:
  return (p_frag);
}

/****************************************************************************************************//**
 *                                       NetIPv6_RxPktFragListRemove()
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
 *                           via NetIPv6_RxPktFragListDiscard()].
 *******************************************************************************************************/
static void NetIPv6_RxPktFragListRemove(NET_BUF     *p_frag_list,
                                        CPU_BOOLEAN tmr_free)
{
  NET_BUF *p_frag_list_prev_list;
  NET_BUF *p_frag_list_next_list;
  NET_BUF_HDR *p_frag_list_prev_list_buf_hdr;
  NET_BUF_HDR *p_frag_list_next_list_buf_hdr;
  NET_BUF_HDR *p_frag_list_buf_hdr;
  NET_TMR *p_tmr;

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
    NetIPv6_FragReasmListsHead = p_frag_list_next_list;
  }

  //                                                               Point next frag list to prev frag list.
  if (p_frag_list_next_list != DEF_NULL) {
    p_frag_list_next_list_buf_hdr = &p_frag_list_next_list->Hdr;
    p_frag_list_next_list_buf_hdr->PrevPrimListPtr = p_frag_list_prev_list;
  } else {
    NetIPv6_FragReasmListsTail = p_frag_list_prev_list;
  }

  //                                                               ---------------- CLR BUF FRAG PTRS -----------------
  p_frag_list_buf_hdr->PrevPrimListPtr = DEF_NULL;
  p_frag_list_buf_hdr->NextPrimListPtr = DEF_NULL;
  p_frag_list_buf_hdr->TmrPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                       NetIPv6_RxPktFragListDiscard()
 *
 * @brief    Discard fragment list from Fragment Lists.
 *
 * @param    p_frag_list     Pointer to fragment list head buffer.
 *
 * @param    tmr_free        Indicate whether to free network timer :
 *                           DEF_YES            Free network timer for fragment list discard.
 *                           DEF_NO      Do NOT free network timer for fragment list discard
 *                           [Freed by NetTmr_Handler()].
 *******************************************************************************************************/
static void NetIPv6_RxPktFragListDiscard(NET_BUF     *p_frag_list,
                                         CPU_BOOLEAN tmr_free)
{
  NetIPv6_RxPktFragListRemove(p_frag_list, tmr_free);           // Remove frag list from Frag Lists.
  NetIPv6_RxPktDiscard(p_frag_list);                            // Discard every frag buf in frag list.

  NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragDgramDisCtr);          // Inc discarded frag'd datagram ctr.
}

/****************************************************************************************************//**
 *                                       NetIPv6_RxPktFragListUpdate()
 *
 * @brief    Update fragment list reassembly calculations.
 *
 * @param    p_frag_list_buf_hdr     Pointer to fragment list head buffer's header.
 *
 * @param    frag_ip_flags           Fragment IPv6 header flags.
 *
 * @param    frag_offset             Fragment offset.
 *
 * @param    frag_size               Fragment size (in octets).
 *
 * @param    p_err                   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) RFC #2460, Section 4.5 'Fragment Header' states that :
 *
 *                                       - (a) "If insufficient fragments are received to complete reassembly of a packet within
 *                                   60 seconds of the reception of the first-arriving fragment of that packet,
 *                                   reassembly of that packet must be abandoned and all the fragments that have been
 *                                   received for that packet must be discarded."
 *
 *                                       - (b) "If the first fragment (i.e., the one with a Fragment Offset of zero) has been
 *                                   received, an ICMP Time Exceeded -- Fragment Reassembly Time Exceeded message
 *                                   should be sent to the source of that fragment."
 *
 * @note     (2) To avoid possible integer arithmetic overflow, the fragmentation arithmetic result MUST
 *                                   be declared as an integer data type with a greater resolution -- i.e. greater number of
 *                                   bits -- than the fragmentation arithmetic operands' data type(s).
 *******************************************************************************************************/
static void NetIPv6_RxPktFragListUpdate(NET_BUF     *p_frag_list,
                                        NET_BUF_HDR *p_frag_list_buf_hdr,
                                        CPU_INT16U  frag_ip_flags,
                                        CPU_INT32U  frag_offset,
                                        CPU_INT32U  frag_size,
                                        RTOS_ERR    *p_err)
{
  NET_BUF *p_buf_last;
  NET_BUF_HDR *p_buf_hdr_last;
  CPU_INT32U frag_size_tot;                                          // See Note #2.
  CPU_BOOLEAN ip_flag_frags_more;
  RTOS_ERR local_err;

  p_frag_list_buf_hdr->IP_FragSizeCur += frag_size;
  ip_flag_frags_more = DEF_BIT_IS_SET(frag_ip_flags, NET_IPv6_FRAG_FLAG_FRAG_MORE);
  if (ip_flag_frags_more != DEF_YES) {                              // If 'More Frags' NOT set (see Note #1b1A), ...
                                                                    // ... calc frag tot size  (see Note #1b2).
    frag_size_tot = (CPU_INT32U)frag_offset + (CPU_INT32U)frag_size;
    if (frag_size_tot > NET_IPv6_TOT_LEN_MAX) {                     // If frag tot size > IP tot len max, ...
                                                                    // Send ICMPv6 Problem message.
      p_buf_last = p_frag_list;
      p_buf_hdr_last = p_frag_list_buf_hdr;
      while (p_buf_hdr_last->NextBufPtr != DEF_NULL) {
        p_buf_last = p_buf_hdr_last->NextBufPtr;
        p_buf_hdr_last = &p_buf_last->Hdr;
      }
#ifdef  NET_ICMPv6_MODULE_EN
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      NetICMPv6_TxMsgErr(p_buf_last,
                         NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                         NET_ICMPv6_MSG_CODE_PARAM_PROB_IP_HDR,
                         (p_frag_list_buf_hdr->IPv6_FragHdrIx - p_frag_list_buf_hdr->IP_HdrIx) + NET_ICMPv6_PTR_IX_IP_FRAG_OFFSET,
                         &local_err);
#endif
      //                                                           ... discard ovf'd frag datagram (see Note #1a3).
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragDisCtr);
      NetIPv6_RxPktFragListDiscard(p_frag_list, DEF_YES);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    p_frag_list_buf_hdr->IP_FragSizeTot = (CPU_INT16U)frag_size_tot;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                   NetIPv6_RxPktFragListChkComplete()
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
 *
 *           Pointer to NULL,                 if fragment reassembly in progress.
 *                                               OR
 *                                           for any fragment discard error.
 *
 * @note     (1) RFC #2460, Section 4.5 'Fragment Header' states that :
 *
 *           - (a) "If insufficient fragments are received to complete reassembly of a packet within
 *                   60 seconds of the reception of the first-arriving fragment of that packet,
 *                   reassembly of that packet must be abandoned and all the fragments that have been
 *                   received for that packet must be discarded."
 *
 *           - (b) "If the first fragment (i.e., the one with a Fragment Offset of zero) has been
 *                   received, an ICMP Time Exceeded -- Fragment Reassembly Time Exceeded message
 *                   should be sent to the source of that fragment."
 *
 * @note     (2) To avoid possible integer arithmetic overflow, the fragmentation arithmetic result
 *               MUST be declared as an integer data type with a greater resolution -- i.e. greater
 *               number of bits -- than the fragmentation arithmetic operands' data type(s).
 *******************************************************************************************************/
static NET_BUF *NetIPv6_RxPktFragListChkComplete(NET_BUF     *p_frag_list,
                                                 NET_BUF_HDR *p_frag_list_buf_hdr,
                                                 RTOS_ERR    *p_err)
{
  NET_BUF *p_frag;
  CPU_INT32U frag_tot_len;                                          // See Note #2.
#if 0
  CPU_INT32 timeout_ms;
#endif
  RTOS_ERR local_err;

  //                                                               If tot frag size complete, ...
  if (p_frag_list_buf_hdr->IP_FragSizeCur == p_frag_list_buf_hdr->IP_FragSizeTot) {
    //                                                             Calc frag IPv6 tot len (see Note #1a2).
#if 0
    frag_tot_len = (CPU_INT32U)p_frag_list_buf_hdr->IPv6_ExtHdrLen + (CPU_INT32U)p_frag_list_buf_hdr->IP_FragSizeTot;
#else
    frag_tot_len = (CPU_INT32U)p_frag_list_buf_hdr->IP_FragSizeTot;
#endif
    if (frag_tot_len > NET_IPv6_TOT_LEN_MAX) {                      // If tot frag len > IPv6 tot len max, ...
                                                                    // ...discard ovf'd frag datagram (see Note #1a1C).
      NetIPv6_RxPktFragListDiscard(p_frag_list, DEF_YES);

#ifdef  NET_ICMPv6_MODULE_EN
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      //                                                           Send ICMPv6 Problem message.
      NetICMPv6_TxMsgErr(p_frag_list,
                         NET_ICMPv6_MSG_TYPE_PARAM_PROB,
                         NET_ICMPv6_MSG_CODE_PARAM_PROB_IP_HDR,
                         (p_frag_list_buf_hdr->IPv6_FragHdrIx - p_frag_list_buf_hdr->IP_HdrIx) + NET_ICMPv6_PTR_IX_IP_FRAG_OFFSET,
                         &local_err);
#endif
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragTotLenCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      p_frag = DEF_NULL;
      goto exit;
    }

    NetIPv6_RxPktFragListRemove(p_frag_list, DEF_YES);
    NET_CTR_STAT_INC(Net_StatCtrs.IPv6.RxFragDgramReasmCtr);
    p_frag = p_frag_list;                                           // ... rtn reasm'd datagram (see Note #1b1).

#if 0
    //                                                             If cur frag size > tot frag size, ...
  } else if (p_frag_list_buf_hdr->IP_FragSizeCur > p_frag_list_buf_hdr->IP_FragSizeTot) {
    NetIPv6_RxPktFragListDiscard(p_frag_list, DEF_YES);             // ... discard ovf'd frag datagram.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragTotLenCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    p_frag = DEF_NULL;
    goto exit;
#endif
  } else {                                                          // Else reset frag tmr (see Note #1b2A).
#if 0
    CORE_ENTER_ATOMIC();
    timeout_ms = NetIPv6_FragReasmTimeout_ms;
    CORE_EXIT_ATOMIC();
    NetTmr_Set(p_frag_list_buf_hdr->TmrPtr,
               NetIPv6_RxPktFragTimeout,
               timeout_ms);
#endif

    p_frag = DEF_NULL;
  }

exit:
  return (p_frag);
}

/****************************************************************************************************//**
 *                                       NetIPv6_RxPktFragTimeout()
 *
 * @brief    Discard fragment list on fragment reassembly timeout.
 *
 * @param    p_frag_list_timeout     Pointer to network buffer fragment reassembly list (see Note #1b).
 *
 * @note     (2) This function is a network timer callback function :
 *
 *                                       - (a) Clear the timer pointer ... :
 *                                       - (1) Cleared in NetIPv6_RxPktFragListRemove() via NetIPv6_RxPktFragListDiscard().
 *
 *                                       - (b) but do NOT re-free the timer.
 *
 * @note     (3) RFC #2460, Section 4.5 'Fragment Header' states that :
 *
 *                                       - (a) "If insufficient fragments are received to complete reassembly of a packet within
 *                                   60 seconds of the reception of the first-arriving fragment of that packet,
 *                                   reassembly of that packet must be abandoned and all the fragments that have been
 *                                   received for that packet must be discarded."
 *
 *                                       - (b) "If the first fragment (i.e., the one with a Fragment Offset of zero) has been
 *                                   received, an ICMP Time Exceeded -- Fragment Reassembly Time Exceeded message
 *                                   should be sent to the source of that fragment."
 *
 * @note     (4) MUST send ICMP 'Time Exceeded' error message BEFORE NetIPv6_RxPktFragListDiscard()
 *                                   frees fragment buffers.
 *******************************************************************************************************/
static void NetIPv6_RxPktFragTimeout(void *p_frag_list_timeout)
{
  NET_BUF *p_frag_list;
  RTOS_ERR local_err;

  p_frag_list = (NET_BUF *)p_frag_list_timeout;                 // See Note #2b2A.

  p_frag_list->Hdr.TmrPtr = DEF_NULL;

#ifdef  NET_ICMPv6_MODULE_EN
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetICMPv6_TxMsgErr(p_frag_list,                   // Send ICMPV6 'Time Exceeded' err msg (see Note #4).
                     NET_ICMPv6_MSG_TYPE_TIME_EXCEED,
                     NET_ICMPv6_MSG_CODE_TIME_EXCEED_FRAG_REASM,
                     NET_ICMPv6_MSG_PTR_NONE,
                     &local_err);
#endif

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  PP_UNUSED_PARAM(local_err);

  //                                                               Discard frag list (see Note #1b).
  //                                                               Clr but do NOT free tmr (see Note #3).
  NetIPv6_RxPktFragListDiscard(p_frag_list, DEF_NO);

  NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxFragDgramTimeoutCtr);
}

/****************************************************************************************************//**
 *                                       NetIPv6_RxPktDemuxDatagram()
 *
 * @brief    Demultiplex IPv6 datagram to ap_propriate ICMPv6, UDP, or TCP layer.
 *
 * @param    p_buf       Pointer to network buffer that received IPv6 datagram.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) When network buffer is demultiplexed to higher-layer protocol receive, buffer's reference
 *                       counter is NOT incremented since the IPv6 layer does NOT maintain a reference to the
 *                       buffer.
 *
 * @note     (2) Default case already invalidated in NetIPv6_RxPktValidate().  However, the default case
 *                       is included as an extra precaution in case 'ProtocolHdrType' is incorrectly modified.
 *******************************************************************************************************/
static void NetIPv6_RxPktDemuxDatagram(NET_BUF     *p_buf,
                                       NET_BUF_HDR *p_buf_hdr,
                                       RTOS_ERR    *p_err)
{
  NET_BUF_HDR *p_hdr;

  p_hdr = &p_buf->Hdr;
  DEF_BIT_SET(p_hdr->Flags, NET_BUF_FLAG_IPv6_FRAME);           // Set IPv6 Flag.

  switch (p_buf_hdr->ProtocolHdrType) {                         // Demux buf to appropriate protocol (see Note #1).
#ifdef  NET_ICMPv6_MODULE_EN
    case NET_PROTOCOL_TYPE_ICMP_V6:
      NetICMPv6_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

    case NET_PROTOCOL_TYPE_UDP_V6:
      NetUDP_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

#ifdef  NET_TCP_MODULE_EN
    case NET_PROTOCOL_TYPE_TCP_V6:
      NetTCP_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

    case NET_PROTOCOL_TYPE_NONE:
    default:                                                    // See Note #2.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.RxInvProtocolCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIPv6_RxPktDiscard()
 *
 * @brief    On any IPv6 receive error(s), discard IPv6 packet(s) & buffer(s).
 *
 * @param    p_buf   Pointer to network buffer.
 *******************************************************************************************************/
static void NetIPv6_RxPktDiscard(NET_BUF *p_buf)
{
  NET_CTR *p_ctr;

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  p_ctr = &Net_ErrCtrs.IPv6.RxPktDisCtr;
#else
  p_ctr = DEF_NULL;
#endif
  (void)NetBuf_FreeBufList(p_buf, p_ctr);
}

/****************************************************************************************************//**
 *                                           NetIPv6_TxPktValidate()
 *
 * @brief    (1) Validate IPv6 transmit packet parameters :
 *
 *           - (a) Validate the following transmit packet parameters :
 *
 *               - (1) Supported protocols :
 *                   - (A) ICMPv6
 *                   - (B) UDP
 *                   - (C) TCP
 *
 *               - (2) Buffer protocol index
 *               - (3) Total Length
 *               - (4) Hop Limit                                               See Note #2d
 *               - (5) Destination Address                                     See Note #2f
 *               - (6) Source      Address                                     See Note #2e
 *
 * @param    p_buf_hdr       Pointer to network buffer header.
 *
 * @param    p_addr_src      Pointer to source        IPv6 address.
 *
 * @param    p_addr_dest     Pointer to destination   IPv6 address.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See 'net_ipv6.h  IPv6 ADDRESS DEFINES  Notes #2 & #3' for supported IPv6 addresses.
 *******************************************************************************************************/

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
static void NetIPv6_TxPktValidate(const NET_BUF_HDR      *p_buf_hdr,
                                  const NET_IPv6_ADDR    *p_addr_src,
                                  const NET_IPv6_ADDR    *p_addr_dest,
                                  NET_IPv6_TRAFFIC_CLASS traffic_class,
                                  NET_IPv6_FLOW_LABEL    flow_label,
                                  NET_IPv6_HOP_LIM       hop_lim,
                                  RTOS_ERR               *p_err)
{
  NET_IPv6_ADDR_OBJ *p_ip_addrs;
  NET_IF_NBR if_nbr;
  CPU_BOOLEAN addr_unspecified;
  CPU_BOOLEAN is_en;

  //                                                               ----------- VALIDATE IPv6 HOP LIMIT ------------
  if (hop_lim < 1) {                                                // If Hop Limit < 1, rtn err.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.TxInvTTL_Ctr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
    goto exit;
  }

  //                                                               ------------- VALIDATE IPv6 ADDRS --------------
  if_nbr = p_buf_hdr->IF_Nbr;                                       // Get pkt's tx IF.
  is_en = NetIF_IsEnHandler(if_nbr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  if (is_en != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto exit;
  }

  //                                                               Chk pkt's tx cfg'd host addr for src addr.
  addr_unspecified = NetIPv6_IsAddrUnspecified(p_addr_src);
  if (addr_unspecified == DEF_NO) {
    if (if_nbr != NET_IF_NBR_LOCAL_HOST) {
      p_ip_addrs = NetIPv6_GetAddrsHostOnIF(if_nbr, p_addr_src);
    } else {
      p_ip_addrs = NetIPv6_GetAddrsHost(p_addr_src,
                                        DEF_NULL);
    }

    if (p_ip_addrs == DEF_NULL) {
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.TxInvAddrSrcCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
      goto exit;
    }
  }

  PP_UNUSED_PARAM(p_addr_dest);
  PP_UNUSED_PARAM(traffic_class);
  PP_UNUSED_PARAM(flow_label);

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                           NetIPv6_TxPkt()
 *
 * Description : (1) Prepare IPv6 header & transmit IPv6 packet :
 *                   (a) Calculate IPv6 header buffer controls
 *                   (b) Check for transmit fragmentation        See Note #2
 *                   (c) Prepare   IPv6 Extension header(s)
 *                   (d) Prepare   IPv6 header
 *                   (e) Transmit  IPv6 packet datagram
 *
 * Argument(s) : p_buf           Pointer to network buffer to transmit IPv6 packet.
 *
 *               p_buf_hdr       Pointer to network buffer header.
 *
 *               p_addr_src      Pointer to source      IPv6 address.
 *
 *               p_addr_dest     Pointer to destination IPv6 address.
 *
 *               p_ext_hdr_list  Pointer to extension header list to add to IPv6 packet.
 *
 *               traffic_class   Traffic class to add in the IPv6 header to send.
 *
 *               flow_label      Flow label to add in the IPv6 header to send.
 *
 *               hop_lim         Hop limit to add in the IPv6 header of the packet to send.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) IPv6 transmit fragmentation NOT currently supported (see 'net_IPv6.c  Note #1c').
 *                   fragmentation is supported for ICMPv6 packet but not for TCP and UDP packets.
 *
 *               (3) Default case already invalidated in NetIPv6_TxPktValidate().  However, the default case
 *                   is included as an extra precaution in case 'ProtocolHdrType' is incorrectly modified.
 *******************************************************************************************************/
static void NetIPv6_TxPkt(NET_BUF                *p_buf,
                          NET_BUF_HDR            *p_buf_hdr,
                          NET_IPv6_ADDR          *p_addr_src,
                          NET_IPv6_ADDR          *p_addr_dest,
                          NET_IPv6_EXT_HDR       *p_ext_hdr_list,
                          NET_IPv6_TRAFFIC_CLASS traffic_class,
                          NET_IPv6_FLOW_LABEL    flow_label,
                          NET_IPv6_HOP_LIM       hop_lim,
                          RTOS_ERR               *p_err)
{
  CPU_INT16U ip_hdr_len_size;
  CPU_INT16U protocol_ix = 0u;
  CPU_BOOLEAN ip_tx_frag;

  //                                                               ---------------- PREPARE IPv6 OPTS -----------------

  ip_tx_frag = DEF_NO;
  //                                                               --------------- CALC IPv6 HDR CTRLS ----------------
  //                                                               Calc tot IPv6 hdr len (in octets).
  ip_hdr_len_size = (CPU_INT16U)(NET_IPv6_HDR_SIZE);

  switch (p_buf_hdr->ProtocolHdrType) {
    case NET_PROTOCOL_TYPE_ICMP_V6:
      protocol_ix = p_buf_hdr->ICMP_MsgIx;
      break;

    case NET_PROTOCOL_TYPE_UDP_V6:
#ifdef  NET_TCP_MODULE_EN
    case NET_PROTOCOL_TYPE_TCP_V6:
#endif
      protocol_ix = p_buf_hdr->TransportHdrIx;
      break;

    case NET_PROTOCOL_TYPE_NONE:
    default:                                                    // See Note #3.
      NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.TxInvProtocolCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               ----------------- CHK FRAG REQUIRED ----------------
  //                                                               Chained NET_BUF requires frag.
  if (p_buf_hdr->NextBufPtr != DEF_NULL) {
    ip_tx_frag = DEF_YES;
  }

  if (protocol_ix < ip_hdr_len_size) {                          // If hdr len > allowed rem ix, tx frag req'd.
    NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.TxFragNotSupported);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    goto exit;
  }

#if 0
  if (ip_tx_frag == DEF_YES) {                                  // If tx frag NOT required, (see Note #2).
    NetIPv6_TxPktPrepareFragHdr(p_buf,
                                p_buf_hdr,
                                &protocol_ix,
                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }
#endif

  //                                                               ... prepare IPv6 Extension Headers ...
  NetIPv6_TxPktPrepareExtHdr(p_buf,
                             p_ext_hdr_list,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  NetIPv6_TxPktPrepareHdr(p_buf,                                // ... prepare IPv6 hdr     ...
                          p_buf_hdr,
                          protocol_ix,
                          p_addr_src,
                          p_addr_dest,
                          p_ext_hdr_list,
                          traffic_class,
                          flow_label,
                          hop_lim,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  NetIPv6_TxPktDatagram(p_buf, p_buf_hdr, p_err);               // ... & tx IPv6 datagram.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  PP_UNUSED_PARAM(ip_tx_frag);                                  // Prevent 'variable unused' compiler warning.

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIPv6_TxPktPrepareHdr()
 *
 * @brief    (1) Prepare IPv6 header :
 *
 *           - (a) Update network buffer's protocol index & length controls
 *
 *           - (b) Prepare the transmit packet's following IPv6 header fields :
 *
 *               - (1) Version
 *               - (2) Traffic Class
 *               - (3) Flow Label
 *               - (4) Payload Length
 *               - (5) Next Header
 *               - (6) Hop Limit
 *               - (7) Source      Address
 *               - (8) Destination Address
 *
 *           - (c) Convert the following IPv6 header fields from host-order to network-order :
 *
 *               - (1) Version / Traffic Class / Flow Label
 *               - (2) Payload Length
 *
 * @param    p_buf           Pointer to network buffer to transmit IP packet.
 *
 * @param    p_buf_hdr       Pointer to network buffer header.
 *
 * @param    protocol_ix     Index to higher-layer protocol header.
 *
 * @param    p_addr_src      Pointer to source        IPv6 address.
 *
 * @param    p_addr_dest     Pointer to destination   IPv6 address.
 *
 * @param    p_ext_hdr_list  Pointer to IPv6 Extensions headers list.
 *
 * @param    traffic_class   Traffic Class of packet.
 *
 * @param    flow_label      Flow Label of packet.
 *
 * @param    hop_lim         Hop Limit of packet.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) See 'net_ipv6.h  IPv6 HEADER' for IPv6 header format.
 *
 * @note     (3) Supports ONLY the following protocols :
 *
 *                               - (a) ICMPv6
 *                               - (b) UDP
 *                               - (c) TCP
 *
 *                           See also 'net.h  Note #2a'.
 *
 * @note     (4) Default case already invalidated in NetIPv6_TxPktValidate(). However, the default case is
 *                           included as an extra precaution in case 'ProtocolHdrType' is incorrectly modified.
 *******************************************************************************************************/
static void NetIPv6_TxPktPrepareHdr(NET_BUF                *p_buf,
                                    NET_BUF_HDR            *p_buf_hdr,
                                    CPU_INT16U             protocol_ix,
                                    NET_IPv6_ADDR          *p_addr_src,
                                    NET_IPv6_ADDR          *p_addr_dest,
                                    NET_IPv6_EXT_HDR       *p_ext_hdr_list,
                                    NET_IPv6_TRAFFIC_CLASS traffic_class,
                                    NET_IPv6_FLOW_LABEL    flow_label,
                                    NET_IPv6_HOP_LIM       hop_lim,
                                    RTOS_ERR               *p_err)
{
  NET_IPv6_HDR *p_ip_hdr;
  NET_IPv6_EXT_HDR *p_ext_hdr;
  CPU_INT08U ip_ver;
  CPU_INT32U ip_ver_traffic_flow;
  NET_DEV_CFG *p_dev_cfg;
  NET_IF *p_if;

  p_if = NetIF_Get(p_buf_hdr->IF_Nbr, p_err);
  p_dev_cfg = (NET_DEV_CFG *)p_if->Dev_Cfg;

  while (p_buf != DEF_NULL) {
    //                                                             ----------------- UPDATE BUF CTRLS -----------------
    p_buf_hdr->IP_HdrIx = p_dev_cfg->TxBufIxOffset;
    NetIF_TxIxDataGet(p_buf_hdr->IF_Nbr,
                      0,
                      &p_buf_hdr->IP_HdrIx);
    p_buf_hdr->IP_DataLen = (CPU_INT16U) p_buf_hdr->TotLen;
    p_buf_hdr->IP_DatagramLen = (CPU_INT16U) p_buf_hdr->TotLen;

    //                                                             ----------------- PREPARE IPv6 HDR -----------------
    p_ip_hdr = (NET_IPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];

    //                                                             ----- PREPARE IP VER/TRAFFIC CLASS/FLOW LABEL ------
    ip_ver = NET_IPv6_HDR_VER;
    ip_ver_traffic_flow = (ip_ver << NET_IPv6_HDR_VER_SHIFT);

    traffic_class = (traffic_class  & NET_IPv6_HDR_TRAFFIC_CLASS_MASK_16);
    ip_ver_traffic_flow |= (traffic_class << NET_IPv6_HDR_TRAFFIC_CLASS_SHIFT);

    flow_label = (flow_label     & NET_IPv6_HDR_FLOW_LABEL_MASK);
    ip_ver_traffic_flow |= (flow_label << NET_IPv6_HDR_FLOW_LABEL_SHIFT);

    NET_UTIL_VAL_COPY_SET_NET_32(&p_ip_hdr->VerTrafficFlow, &ip_ver_traffic_flow);

    //                                                             ------------- PREPARE IPv6 PAYLOAD LEN -------------
    NET_UTIL_VAL_COPY_SET_NET_16(&p_ip_hdr->PayloadLen, &p_buf_hdr->TotLen);
    p_buf_hdr->TotLen += sizeof(NET_IPv6_HDR);
    p_buf_hdr->IP_TotLen = (CPU_INT16U) p_buf_hdr->TotLen;

    //                                                             -------------- PREPARE IPv6 NEXT HDR ---------------
    p_ext_hdr = p_ext_hdr_list;
    if (p_ext_hdr == DEF_NULL) {
      switch (p_buf_hdr->ProtocolHdrType) {                     // Demux IPv6 protocol (see Note #3).
        case NET_PROTOCOL_TYPE_ICMP_V6:
          p_ip_hdr->NextHdr = NET_IP_HDR_PROTOCOL_ICMPv6;
          break;

        case NET_PROTOCOL_TYPE_UDP_V6:
          p_ip_hdr->NextHdr = NET_IP_HDR_PROTOCOL_UDP;
          break;

    #ifdef  NET_TCP_MODULE_EN
        case NET_PROTOCOL_TYPE_TCP_V6:
          p_ip_hdr->NextHdr = NET_IP_HDR_PROTOCOL_TCP;
          break;
    #endif

        case NET_PROTOCOL_TYPE_IP_V6_EXT_HOP_BY_HOP:
          p_ip_hdr->NextHdr = NET_IP_HDR_PROTOCOL_EXT_HOP_BY_HOP;
          break;

        case NET_PROTOCOL_TYPE_IP_V6_EXT_ROUTING:
          p_ip_hdr->NextHdr = NET_IP_HDR_PROTOCOL_EXT_ROUTING;
          break;

        case NET_PROTOCOL_TYPE_IP_V6_EXT_FRAG:
          p_ip_hdr->NextHdr = NET_IP_HDR_PROTOCOL_EXT_FRAG;
          break;

        case NET_PROTOCOL_TYPE_IP_V6_EXT_ESP:
          p_ip_hdr->NextHdr = NET_IP_HDR_PROTOCOL_EXT_ESP;
          break;

        case NET_PROTOCOL_TYPE_IP_V6_EXT_AUTH:
          p_ip_hdr->NextHdr = NET_IP_HDR_PROTOCOL_EXT_AUTH;
          break;

        case NET_PROTOCOL_TYPE_IP_V6_EXT_NONE:
          p_ip_hdr->NextHdr = NET_IP_HDR_PROTOCOL_EXT_NONE;
          break;

        case NET_PROTOCOL_TYPE_IP_V6_EXT_DEST:
          p_ip_hdr->NextHdr = NET_IP_HDR_PROTOCOL_EXT_DEST;
          break;

        case NET_PROTOCOL_TYPE_IP_V6_EXT_MOBILITY:
          p_ip_hdr->NextHdr = NET_IP_HDR_PROTOCOL_EXT_MOBILITY;
          break;

        default:                                                    // See Note #4.
          NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.TxInvProtocolCtr);
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
    } else {
      p_ip_hdr->NextHdr = p_ext_hdr->Type;
    }

    p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6;        // Update buf protocol for IPv6.
    p_buf_hdr->ProtocolHdrTypeNet = NET_PROTOCOL_TYPE_IP_V6;

    //                                                             -------------- PREPARE IPv6 HOP LIMIT --------------
    if (hop_lim != NET_IPv6_HOP_LIM_NONE) {
      p_ip_hdr->HopLim = hop_lim;
    } else {
      p_ip_hdr->HopLim = NET_IPv6_HOP_LIM_DFLT;
    }

    //                                                             ---------------- PREPARE IPv6 ADDRS ----------------
    Mem_Copy(&p_buf_hdr->IPv6_AddrSrc, p_addr_src, NET_IPv6_ADDR_SIZE);
    Mem_Copy(&p_buf_hdr->IPv6_AddrDest, p_addr_dest, NET_IPv6_ADDR_SIZE);

    Mem_Copy(&p_ip_hdr->AddrSrc, p_addr_src, NET_IPv6_ADDR_SIZE);
    Mem_Copy(&p_ip_hdr->AddrDest, p_addr_dest, NET_IPv6_ADDR_SIZE);

    //                                                             Move to next buffer.
    p_buf = p_buf_hdr->NextBufPtr;
    if (p_buf != DEF_NULL) {
      p_buf_hdr = &p_buf->Hdr;
    }
  }

  PP_UNUSED_PARAM(protocol_ix);
}

/****************************************************************************************************//**
 *                                       NetIPv6_TxPktPrepareExtHdr()
 *
 * @brief    Prepare Extension headers in packets to send and, if necessary, chained fragments
 *           for IPv6 fragmentation.
 *
 * @param    p_buf           Pointer to network buffer to transmit IPv6 packet.
 *
 * @param    p_ext_hdr_list  Pointer to list of extension headers to add.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIPv6_TxPktPrepareExtHdr(NET_BUF          *p_buf,
                                       NET_IPv6_EXT_HDR *p_ext_hdr_list,
                                       RTOS_ERR         *p_err)
{
  NET_BUF_HDR *p_buf_hdr;
  NET_IPv6_EXT_HDR *p_ext_hdr;
  NET_IPv6_EXT_HDR *p_ext_hdr_next;
  NET_IPv6_EXT_HDR *p_ext_hdr_prev;
  CPU_FNCT_PTR fnct;
  NET_IPv6_EXT_HDR_ARG_GENERIC ext_hdr_arg;
  CPU_INT16U data_ix;
  CPU_INT16U data_ix_prev;
  CPU_INT08U next_hdr_type = NET_BUF_TYPE_NONE;
  NET_IPv6_EXT_HDR_ARG_FRAG frag_hdr_arg;
  NET_IPv6_FRAG_FLAGS fragOffsetFlag;
  CPU_INT16U datagram_offset;
  CPU_INT32U ID;
  NET_DEV_CFG *p_dev_cfg;
  NET_IF *p_if;

  datagram_offset = 0u;
  fragOffsetFlag = NET_IPv6_FRAG_OFFSET_NONE;
  NET_IPv6_TX_GET_ID(ID);

  while (p_buf != DEF_NULL) {
    p_buf_hdr = &p_buf->Hdr;
    p_if = NetIF_Get(p_buf_hdr->IF_Nbr, p_err);
    p_dev_cfg = (NET_DEV_CFG *)p_if->Dev_Cfg;

    p_buf_hdr->IP_HdrIx = p_dev_cfg->TxBufIxOffset;
    NetIF_TxIxDataGet(p_buf_hdr->IF_Nbr,
                      0,
                      &p_buf_hdr->IP_HdrIx);

    //                                                             ---------------- SET FRAGMENT INFO -----------------
    //                                                             Get the fragment offset.
    fragOffsetFlag = datagram_offset & NET_IPv6_FRAG_OFFSET_MASK;

    datagram_offset += p_buf_hdr->TotLen;

    //                                                             Determine the more frag flag.
    if (p_buf_hdr->NextBufPtr != DEF_NULL) {
      DEF_BIT_SET(fragOffsetFlag, NET_IPv6_FRAG_FLAG_MORE);
    }

    //                                                             ------------ PREPARE EXTENSION HEADERS -------------
    data_ix = 0;
    data_ix_prev = 0;
    p_ext_hdr = p_ext_hdr_list;
    while (p_ext_hdr != DEF_NULL) {
      p_ext_hdr_next = p_ext_hdr->NextHdrPtr;
      p_ext_hdr_prev = p_ext_hdr->PrevHdrPtr;

      //                                                           Set Type of Next Header.
      if (p_ext_hdr_next != DEF_NULL) {
        next_hdr_type = p_ext_hdr_next->Type;
      } else {
        switch (p_buf_hdr->ProtocolHdrType) {
          case NET_PROTOCOL_TYPE_ICMP_V6:
            next_hdr_type = NET_IP_HDR_PROTOCOL_ICMPv6;
            break;

          case NET_PROTOCOL_TYPE_UDP_V6:
            next_hdr_type = NET_IP_HDR_PROTOCOL_UDP;
            break;

#ifdef  NET_TCP_MODULE_EN
          case NET_PROTOCOL_TYPE_TCP_V6:
            next_hdr_type = NET_IP_HDR_PROTOCOL_TCP;
            break;
#endif

          default:
            NET_CTR_ERR_INC(Net_ErrCtrs.IPv6.TxInvProtocolCtr);
            RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        }
      }

      //                                                           Set Ext Hdr Data Index.
      if (p_ext_hdr_prev != DEF_NULL) {
        data_ix = data_ix_prev + p_ext_hdr_prev->Len;
      } else {
        data_ix = p_buf_hdr->IP_HdrIx + NET_IPv6_HDR_SIZE;
      }

      //                                                           Callback function to write content of ext hdr.
      switch (p_ext_hdr->Type) {
        case NET_IP_HDR_PROTOCOL_EXT_HOP_BY_HOP:
        case NET_IP_HDR_PROTOCOL_EXT_DEST:
        case NET_IP_HDR_PROTOCOL_EXT_ROUTING:
        case NET_IP_HDR_PROTOCOL_EXT_ESP:
        case NET_IP_HDR_PROTOCOL_EXT_AUTH:
        case NET_IP_HDR_PROTOCOL_EXT_MOBILITY:
          ext_hdr_arg.BufIx = data_ix;
          ext_hdr_arg.BufPtr = p_buf;
          ext_hdr_arg.NextHdr = next_hdr_type;
          fnct = (CPU_FNCT_PTR)p_ext_hdr->Fnct;
          fnct((void *) &ext_hdr_arg);
          break;

        case NET_IP_HDR_PROTOCOL_EXT_FRAG:
          frag_hdr_arg.BufIx = data_ix;
          frag_hdr_arg.BufPtr = p_buf;
          frag_hdr_arg.NextHdr = next_hdr_type;
          frag_hdr_arg.FragOffset = NET_UTIL_HOST_TO_NET_16(fragOffsetFlag);
          frag_hdr_arg.FragID = NET_UTIL_HOST_TO_NET_32(ID);
          fnct = (CPU_FNCT_PTR)p_ext_hdr->Fnct;
          fnct((void *) &frag_hdr_arg);
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
          goto exit;
      }

      p_buf_hdr->IPv6_ExtHdrLen += p_ext_hdr->Len;
      p_buf_hdr->TotLen += p_ext_hdr->Len;

      data_ix_prev = data_ix;
      p_ext_hdr = p_ext_hdr->NextHdrPtr;
    }

    p_buf = p_buf_hdr->NextBufPtr;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv6_TxPktPrepareFragHdr()
 *
 * @brief    Prepare packet and chained fragments for IPv6 fragmentation.
 *
 * @param    p_buf           Pointer to network buffer to transmit IPv6 packet.
 *
 * @param    p_buf_hdr       Pointer to network buffer header.
 *
 * @param    p_protocol_ix   Pointer to a protocol header index.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#if 0
static void NetIPv6_TxPktPrepareFragHdr(NET_BUF     *p_buf,
                                        NET_BUF_HDR *p_buf_hdr,
                                        CPU_INT16U  *p_protocol_ix,
                                        RTOS_ERR    *p_err)
{
  NET_IPv6_FRAG_HDR *p_frag_hdr;
  NET_IPv6_FRAG_FLAGS fragOffsetFlag;
  CPU_INT16U datagram_offset;
  CPU_INT32U ID;

  datagram_offset = 0u;
  fragOffsetFlag = NET_IPv6_FRAG_OFFSET_NONE;
  NET_IPv6_TX_GET_ID(ID);

  *p_protocol_ix -= NET_IPv6_FRAG_HDR_SIZE;

  while (p_buf != DEF_NULL) {
    p_buf_hdr = &p_buf->Hdr;
    p_buf_hdr->IPv6_ExtHdrLen += NET_IPv6_FRAG_HDR_SIZE;
    p_frag_hdr = (NET_IPv6_FRAG_HDR *)&p_buf->DataPtr[*p_protocol_ix];
    Mem_Clr(p_frag_hdr, NET_IPv6_FRAG_HDR_SIZE);

    //                                                             Set the Next Hdr field of the Frag Hdr.
    switch (p_buf_hdr->ProtocolHdrType) {
      case NET_PROTOCOL_TYPE_ICMP_V6:
        p_frag_hdr->NextHdr = NET_IP_HDR_PROTOCOL_ICMPv6;
        break;

#ifdef  NET_TCP_MODULE_PRESENT
      case NET_PROTOCOL_TYPE_TCP_V6:
        p_frag_hdr->NextHdr = NET_IP_HDR_PROTOCOL_TCP;
        break;
#endif

      case NET_PROTOCOL_TYPE_UDP_V6:
        p_frag_hdr->NextHdr = NET_IP_HDR_PROTOCOL_UDP;
        break;

      default:
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
        goto exit;
    }

    p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6_EXT_FRAG;

    //                                                             Get the fragment offset.
    fragOffsetFlag = datagram_offset & NET_IPv6_FRAG_OFFSET_MASK;

#if 1
    datagram_offset += p_buf_hdr->TotLen;
    p_buf_hdr->TotLen += NET_IPv6_FRAG_HDR_SIZE;
#else
    datagram_offset += 1448;
#endif
    //                                                             Determine the more frag flag.
    if (p_buf_hdr->NextBufPtr != DEF_NULL) {
      DEF_BIT_SET(fragOffsetFlag, NET_IPv6_FRAG_FLAG_MORE);
    }

    p_frag_hdr->FragOffsetFlag = NET_UTIL_HOST_TO_NET_16(fragOffsetFlag);

    p_buf = p_buf_hdr->NextBufPtr;
  }

exit:
  return;
}
#endif

/****************************************************************************************************//**
 *                                           NetIPv6_TxPktDatagram()
 *
 * @brief    (1) Transmit IPv6 packet datagram :
 *
 *           - (a) Select next-route IPv6 address
 *           - (b) Transmit IPv6 packet datagram via next IPv6 address route :
 *
 *               - (1) Destination is this host                Send to Loopback Interface
 *               - (2) Destination is multicast                Send to Network  Interface Transmit
 *               - (3) Destination is Local  Host              Send to Network  Interface Transmit
 *               - (4) Destination is Remote Host              Send to Network  Interface Transmit
 *
 * @param    p_buf       Pointer to network buffer to transmit IPv6 packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIPv6_TxPktDatagram(NET_BUF     *p_buf,
                                  NET_BUF_HDR *p_buf_hdr,
                                  RTOS_ERR    *p_err)
{
  NET_IP_TX_DEST_STATUS dest_status;

  //                                                               --------------- SEL NEXT-ROUTE ADDR ----------------
  dest_status = NetIPv6_TxPktDatagramRouteSel(p_buf, p_buf_hdr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  switch (dest_status) {
    case NET_IP_TX_DEST_STATUS_LOCAL_HOST:
      p_buf_hdr->IF_NbrTx = NET_IF_NBR_LOCAL_HOST;
      break;

    case NET_IP_TX_DEST_STATUS_MULTICAST:
    case NET_IP_TX_DEST_STATUS_LOCAL_NET:
    case NET_IP_TX_DEST_STATUS_DFLT_GATEWAY:
    case NET_IP_TX_DEST_STATUS_NO_DFLT_GATEWAY:
      p_buf_hdr->IF_NbrTx = p_buf_hdr->IF_Nbr;
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_NEXT_HOP);
      goto exit;
  }

  //                                                               --------------- TX IPv6 PKT DATAGRAM ---------------
  NetIF_Tx(p_buf, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                   NetIPv6_TxPktDatagramRouteSel()
 *
 * Description : (1) Configure next-route IPv6 address for transmit IPv6 packet datagram :
 *
 *                   (a) Select next-route IPv6 address :
 *                       (1) Destination is this host                    See Note #3
 *                       (2) Link-local Host                             See Note #2a
 *                       (3) Remote Host                                 See Note #2a
 *                       (4) Multicast                                   See Note #2c
 *
 *                   (b) Configure next-route IPv6 address for all buffers in buffer list.
 *
 * Argument(s) : p_buf_hdr   Pointer to network buffer header of IPv6 transmit packet.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     :(2) RFC #4861, Section 5.2 Next-hop determination :
 *                   (a) "The sender performs a longest prefix match against the Prefix List to determine
 *                           whether the packet's destination is on- or off-link.  If the destination is on-link,
 *                           the next-hop address is the same as the packet's destination address. Otherwise,
 *                           the sender selects a router from the Default Router List."
 *
 *                   (b) " For efficiency reasons, next-hop determination is not performed on every packet
 *                           that is sent. Instead, the results of next-hop determination computations are saved
 *                           in the Destination Cache (which also contains updates learned from Redirect messages).
 *                           When the sending node has a packet to send, it first examines the Destination Cache.
 *                           If no entry exists for the destination, next-hop determination is invoked to create
 *                           a Destination Cache entry."
 *
 *                   (c) "For multicast packets, the next-hop is always the (multicast) destination address
 *                           and is considered to be on-link.
 *******************************************************************************************************/
static NET_IP_TX_DEST_STATUS NetIPv6_TxPktDatagramRouteSel(NET_BUF     *p_buf,
                                                           NET_BUF_HDR *p_buf_hdr,
                                                           RTOS_ERR    *p_err)
{
  NET_BUF *p_buf_list;
  NET_BUF_HDR *p_buf_list_hdr;
  NET_IF_NBR if_nbr;
  NET_IF_NBR if_nbr_found;
  const NET_IPv6_ADDR *p_addr_next_hop;
  const NET_IPv6_ADDR *p_addr_dest;
  CPU_BOOLEAN addr_mcast;
  NET_IP_TX_DEST_STATUS dest_status = NET_IP_TX_DEST_STATUS_NONE;

  if_nbr = p_buf_hdr->IF_Nbr;
  p_addr_dest = &p_buf_hdr->IPv6_AddrDest;

  //                                                               Multicast Address destination.
  addr_mcast = NetIPv6_IsAddrMcast(p_addr_dest);
  if (addr_mcast == DEF_TRUE) {
    p_addr_next_hop = p_addr_dest;
    dest_status = NET_IP_TX_DEST_STATUS_MULTICAST;
    goto exit;
  }

  //                                                               Verify that addr dest is not an addr on current IF.
  if_nbr_found = NetIPv6_GetAddrHostIF_Nbr(p_addr_dest);
  if (if_nbr == if_nbr_found) {
    p_addr_next_hop = p_addr_dest;
    dest_status = NET_IP_TX_DEST_STATUS_LOCAL_HOST;
    goto exit;
  }

#ifdef  NET_NDP_MODULE_EN
  //                                                               ---------------- FIND NEXT HOP ADDRESS -------------
  dest_status = NetNDP_NextHopByIF(if_nbr,
                                   p_addr_dest,
                                   (NET_IPv6_ADDR **) &p_addr_next_hop,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

#else
  RTOS_ERR_SET(*p_err, RTOS_ERR_NET_NEXT_HOP);
  goto exit;
#endif

exit:
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NET_NEXT_HOP) {
    p_buf_list = p_buf;
    p_buf_list_hdr = p_buf_hdr;
    while (p_buf_list != DEF_NULL) {
      if (p_addr_next_hop != DEF_NULL) {
        p_buf_list_hdr->IPv6_AddrNextRoute = *p_addr_next_hop;
      }
      p_buf_list = p_buf_list_hdr->NextBufPtr;
      p_buf_list_hdr = &p_buf_list->Hdr;
    }
  } else {
    //                                                             ICMPv6 Destination Unreachable Error message should ...
    //                                                             ... be sent. #### NET-781
  }

  return (dest_status);
}

/****************************************************************************************************//**
 *                                               NetIPv6_ReTxPkt()
 *
 * @brief    (1) Prepare & re-transmit IPv6 packet :
 *
 *           - (a) Prepare     IPv6 header
 *           - (b) Re-transmit IPv6 packet datagram
 *
 * @param    p_buf       Pointer to network buffer to re-transmit IPv6 packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIPv6_ReTxPkt(NET_BUF     *p_buf,
                            NET_BUF_HDR *p_buf_hdr,
                            RTOS_ERR    *p_err)
{
  //                                                               ----------------- PREPARE IPv6 HDR -----------------
  NetIPv6_ReTxPktPrepareHdr(p_buf, p_buf_hdr);

  //                                                               ------------- RE-TX IPv6 PKT DATAGRAM --------------
  NetIPv6_TxPktDatagram(p_buf, p_buf_hdr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIPv6_ReTxPktPrepareHdr()
 *
 * @brief    (1) Prepare IPv6 header for re-transmit IPv6 packet :
 *
 *           - (a) Update network buffer's protocol & length controls
 *
 *           - (b) (1) Version
 *               - (2) Traffic Class
 *               - (3) Flow Label
 *               - (4) Payload Length
 *               - (5) Next Header
 *               - (6) Hop Limit
 *               - (7) Source      Address
 *               - (8) Destination Address
 *
 * @param    p_buf       Pointer to network buffer to transmit IPv6 packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *******************************************************************************************************/
static void NetIPv6_ReTxPktPrepareHdr(NET_BUF     *p_buf,
                                      NET_BUF_HDR *p_buf_hdr)
{
#if 0
  NET_IPv6_HDR *p_ip_hdr;
#endif

  //                                                               ----------------- UPDATE BUF CTRLS -----------------
  p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6;         // Update buf protocol for IPv6.
  p_buf_hdr->ProtocolHdrTypeNet = NET_PROTOCOL_TYPE_IP_V6;
  //                                                               Reset tot len for re-tx.
  p_buf_hdr->TotLen = (NET_BUF_SIZE)p_buf_hdr->IP_TotLen;

  //                                                               ----------------- PREPARE IPv6 HDR -----------------
#if 0
  p_ip_hdr = (NET_IPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];
#endif

  PP_UNUSED_PARAM(p_buf);
}

/****************************************************************************************************//**
 *                                       NetIPv6_TxSolicitationTimeout ()
 *
 * @brief    Handle Router Solicitation transmission timeout and retries.
 *
 * @param    p_obj   Pointer to a NET_IF object.
 *******************************************************************************************************/
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
static void NetIPv6_TxSolicitationTimeout(void *p_obj)
{
  NET_IF *p_if;
  NET_IPv6_AUTO_CFG_OBJ *p_auto_obj;
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_if = (NET_IF *)p_obj;

  p_auto_obj = p_if->IP_Obj->IPv6_AutoCfgObjPtr;

  if (p_auto_obj->State == NET_IPv6_AUTO_CFG_STATE_STOPPED) {
    goto exit_fail;
  }

  p_auto_obj->RouterSolRetryTmr = DEF_NULL;

  p_auto_obj->RouterSolRetryCnt++;

  if (p_auto_obj->RouterSolRetryCnt < NET_NDP_TX_ROUTER_SOL_RETRY_MAX) {
    NetNDP_TxRouterSolicitation(p_if->Nbr,
                                p_auto_obj->RouterSolAddrSrcPtr,
                                &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      goto exit_fail;
    }
    p_auto_obj->RouterSolRetryTmr = NetTmr_Get(NetIPv6_TxSolicitationTimeout,
                                               p_if,
                                               NET_NDP_RX_ROUTER_ADV_TIMEOUT_MS,
                                               NET_TMR_OPT_ONE_SHOT,
                                               &local_err);
    goto exit;
  }

exit_fail:
  NetIPv6_AddrAutoCfgComp(p_if->Nbr, NET_IPv6_ADDR_CFG_STATUS_FAIL);

exit:
  return;
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IPv6_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
