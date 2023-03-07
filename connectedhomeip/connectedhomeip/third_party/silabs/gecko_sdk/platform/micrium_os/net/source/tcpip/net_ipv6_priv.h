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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_IPv6_PRIV_H_
#define  _NET_IPv6_PRIV_H_

#include  "../../include/net_cfg_net.h"

#ifdef   NET_IPv6_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_ip.h"
#include  "../../include/net_ipv6.h"

#include  "../../include/net_cfg_net.h"
#include  "net_ip_priv.h"
#include  "net_type_priv.h"
#include  "net_tmr_priv.h"
#include  "net_if_priv.h"

#include  <common/source/kal/kal_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           DAD DEFINES
 *******************************************************************************************************/

#define  NET_IPv6_DAD_SIGNAL_TIMEOUT_MS    (NET_NDP_SOLICIT_NBR_MAX * NET_NDP_RETRANS_TIMEOUT_MAX_SEC * 1000u)

/********************************************************************************************************
 *                                       IP MULTICAST SELECT DEFINES
 *
 * Note(s) : (1) The following IP values MUST be pre-#define'd in 'net_def.h' PRIOR to 'net_cfg.h'
 *               so that the developer can configure IP for the desired IP multicast selection (see
 *               'net_def.h  IP LAYER DEFINES  Note #1') :
 *
 *                   NET_IP_MULTICAST_SEL_NONE
 *                   NET_IP_MULTICAST_SEL_TX
 *                   NET_IP_MULTICAST_SEL_TX_RX
 *******************************************************************************************************/

#if 0                                                           // See Note #1.
#define  NET_IP_MULTICAST_SEL_NONE                         0u
#define  NET_IP_MULTICAST_SEL_TX                           1u
#define  NET_IP_MULTICAST_SEL_TX_RX                        2u
#endif

/********************************************************************************************************
 *                                           IPv6 HEADER DEFINES
 *
 * Note(s) : (1) The following IPv6 value MUST be pre-#define'd in 'net_def.h' PRIOR to 'net_buf.h' so that
 *               the Network Buffer Module can configure maximum buffer header size (see 'net_def.h  IPv6
 *               LAYER DEFINES' & 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1') :
 *
 *               (a) NET_IPv6_HDR_SIZE_MAX                 40        (NET_IPv6_HDR_LEN_MAX
 *                                                                   * NET_IPv6_HDR_LEN_WORD_SIZE)
 *******************************************************************************************************/

#define  NET_IPv6_HDR_VER_MASK                    0xF0000000u
#define  NET_IPv6_HDR_VER_SHIFT                           28u
#define  NET_IPv6_HDR_VER                                  6u

#define  NET_IPv6_HDR_TRAFFIC_CLASS_MASK_32       0x0FF00000u
#define  NET_IPv6_HDR_TRAFFIC_CLASS_MASK_16           0x00FFu
#define  NET_IPv6_HDR_TRAFFIC_CLASS_SHIFT                 20u
#define  NET_IPv6_HDR_TRAFFIC_CLASS               0x00000000u

#define  NET_IPv6_HDR_DSCP_MASK_08                      0xFCu
#define  NET_IPv6_HDR_DSCP_SHIFT                        0x02u

#define  NET_IPv6_HDR_ECN_MASK_08                       0x03u

#define  NET_IPv6_HDR_FLOW_LABEL_MASK             0x000FFFFFu
#define  NET_IPv6_HDR_FLOW_LABEL_SHIFT                     0u
#define  NET_IPv6_HDR_FLOW_LABEL                           0u

#define  NET_IPv6_HDR_LEN_MASK                          0x0Fu
#define  NET_IPv6_HDR_LEN_MIN                             10
#define  NET_IPv6_HDR_LEN_MAX                             15
#define  NET_IPv6_HDR_LEN_WORD_SIZE                      CPU_WORD_SIZE_32

#if 0
#define  NET_IPv6_HDR_SIZE                               40u
#endif

#define  NET_IPv6_HDR_SIZE_TOT_MIN                      (NET_IF_HDR_SIZE_TOT_MIN + NET_IPv6_HDR_SIZE)
#define  NET_IPv6_HDR_SIZE_TOT_MAX                      (NET_IF_HDR_SIZE_TOT_MAX + NET_IPv6_HDR_SIZE_MAX)

#define  NET_IPv6_ID_NONE                                  0u
#define  NET_IPv6_ID_INIT                                NET_IPv6_ID_NONE

/********************************************************************************************************
 *                               IPv6 EXTENSION HEADER SORT KEY DEFINES
 *
 * Note(s) : (1) Used to sort the extensions headers list for the appropriate order in the IPv6 packet.
 *******************************************************************************************************/

#define  NET_IPv6_EXT_HDR_KEY_HOP_BY_HOP                   0u
#define  NET_IPv6_EXT_HDR_KEY_DEST_01                      1u
#define  NET_IPv6_EXT_HDR_KEY_ROUTING                      2u
#define  NET_IPv6_EXT_HDR_KEY_FRAG                         3u
#define  NET_IPv6_EXT_HDR_KEY_AUTH                         4u
#define  NET_IPv6_EXT_HDR_KEY_ESP                          5u
#define  NET_IPv6_EXT_HDR_KEY_DEST_02                      6u

/********************************************************************************************************
 *                                   IPv6 HEADER TRAFFIC CLASS (TC) DEFINES
 *
 * Note(s) : (1) (a) See 'IPv6 HEADER  Note #3'                   for TC             fields.
 *
 *               (b) See RFC # 791, Section 3.1 'Type of Service' for TC  Precedence values.
 *
 *               (c) See RFC #1349, Section 4                     for TOS            values.
 *
 *           (2) RFC #1122, Section 3.2.1.6 states that "the default ... TOS field ... is all zero bits."
 *******************************************************************************************************/

#define  NET_IPv6_HDR_TRAFFIC_CLASS_NONE                0x00u

#define  NET_IPv6_HDR_TRAFFIC_CLASS_DFLT                 NET_IPv6_HDR_TRAFFIC_CLASS_NONE

#define  NET_IPv6_TRAFFIC_CLASS_DFLT                     NET_IPv6_HDR_TRAFFIC_CLASS_DFLT

/********************************************************************************************************
 *                                       IPv6 HEADER HOP LIMIT DEFINES
 *
 * Note(s) : (1) (a) IPv6 packets cannot be sent with Hop Limit of zero. Therefore a Hop Limit of zero
 *                   will be changed to the default Hop Limit when transmitting.
 *
 *               (b) TCP-IP stack configured the IPv6 default Hop Limit to a value of 128.
 *
 *           (2) Multicast packet must always have a Hop Limit of 1 since they should not leave the local
 *               network.
 *******************************************************************************************************/

#define  NET_IPv6_HDR_HOP_LIM_NONE                         0    // On IPv6 Tx, subst _DFLT for _NONE (see Note #1a).
#define  NET_IPv6_HDR_HOP_LIM_MIN                          1
#define  NET_IPv6_HDR_HOP_LIM_MAX                        255
#define  NET_IPv6_HDR_HOP_LIM_DFLT                       128

#define  NET_IPv6_HOP_LIM_NONE                           NET_IPv6_HDR_HOP_LIM_NONE
#define  NET_IPv6_HOP_LIM_MIN                            NET_IPv6_HDR_HOP_LIM_MIN
#define  NET_IPv6_HOP_LIM_MAX                            NET_IPv6_HDR_HOP_LIM_MAX
#define  NET_IPv6_HOP_LIM_DFLT                           NET_IPv6_HDR_HOP_LIM_DFLT

#define  NET_IPv6_HOP_LIM_MULTICAST_DFLT                   1    // See Note #2.

/********************************************************************************************************
 *                               IPv6 HEADER FLOW LABEL DEFINES
 *******************************************************************************************************/

#define  NET_IPv6_HDR_FLOW_LABEL_NONE                      0
#define  NET_IPv6_HDR_FLOW_LABEL_MIN                       0
#define  NET_IPv6_HDR_FLOW_LABEL_MAX              0x000FFFFF
#define  NET_IPv6_HDR_FLOW_LABEL_DFLT                    NET_IPv6_HDR_FLOW_LABEL_NONE

#define  NET_IPv6_FLOW_LABEL_NONE                        NET_IPv6_HDR_FLOW_LABEL_NONE
#define  NET_IPv6_FLOW_LABEL_MIN                         NET_IPv6_HDR_FLOW_LABEL_MIN
#define  NET_IPv6_FLOW_LABEL_MAX                         NET_IPv6_HDR_FLOW_LABEL_MAX
#define  NET_IPv6_FLOW_LABEL_DFLT                        NET_IPv6_HDR_FLOW_LABEL_DFLT

/********************************************************************************************************
 *                                       IPv6 DATA/TOTAL LENGTH DEFINES
 *
 * Note(s) : (1) (a) IPv6 total length #define's (NET_IPv6_TOT_LEN)  relate to the total size of a complete
 *                   IPv6 datagram, including the packet's IPv6 header.  Note that a complete IPv6 datagram
 *                   MAY be fragmented in multiple IPv6 packets.
 *
 *               (b) IPv6 data  length #define's (NET_IPv6_DATA_LEN) relate to the data  size of a complete
 *                   IPv6 datagram, equal to the total IPv6 datagram length minus its IPv6 header size.  Note
 *                   that a complete IPv6 datagram MAY be fragmented in multiple IPv6 packets.
 *           (2) RFC #2460, Section 5 'Packet Size Issues' required that "every link in the internet have
 *               an MTU of 1280 octets or greater."
 *******************************************************************************************************/

//                                                                 See Notes #1a & #1b.
#define  NET_IPv6_DATA_LEN_MIN                             0

#define  NET_IPv6_TOT_LEN_MIN                           (NET_IPv6_HDR_SIZE + NET_IPv6_DATA_LEN_MIN)
#define  NET_IPv6_TOT_LEN_MAX                            DEF_INT_16U_MAX_VAL

#define  NET_IPv6_DATA_LEN_MAX                          (NET_IPv6_TOT_LEN_MAX  - NET_IPv6_HDR_SIZE)

#define  NET_IPv6_MAX_DATAGRAM_SIZE_DFLT                1280                    // See Note #2.

/********************************************************************************************************
 *                                   IPv6 FRAGMENTATION & FLAG DEFINES
 *
 * Note(s) : (1) See 'IPv6 HEADER  Note #4' for flag fields.
 *******************************************************************************************************/

#define  NET_IPv6_FRAG_FLAGS_MASK                     0x0007u

#define  NET_IPv6_FRAG_NONE                              DEF_BIT_NONE
#define  NET_IPv6_FRAG_FLAG_FRAG_MORE                    DEF_BIT_00

/********************************************************************************************************
 *                                       IPv6 FRAGMENTATION DEFINES
 *
 * Note(s) : (1) The fragment offset field of the Fragment Header is located at bits [15-3]. It is a value
 *               representing a 8-byte multiple offset. Since the field starts at bit 3, masking the lower
 *               3 bits of the field provides a byte multiple offset of the fragment.
 *
 *           (2) If insufficient fragments are received to complete reassembly of a packet within
 *                   60 seconds of the reception of the first-arriving fragment of that packet,
 *                   reassembly of that packet must be abandoned and all the fragments that have been
 *                   received for that packet must be discarded.
 *******************************************************************************************************/

#define  NET_IPv6_FRAG_OFFSET_MASK                    0xFFF8u
#define  NET_IPv6_FRAG_OFFSET_NONE                         0u
#define  NET_IPv6_FRAG_OFFSET_MIN                          0u
#define  NET_IPv6_FRAG_OFFSET_MAX                     0xFFF8u
#define  NET_IPv6_FRAG_FLAG_MORE                         DEF_BIT_00

#define  NET_IPv6_FRAG_SIZE_UNIT                           8u   // Frag size unit = 8 octets (see Note #1).

#define  NET_IPv6_FRAG_SIZE_NONE                         DEF_INT_16U_MAX_VAL
#define  NET_IPv6_FRAG_SIZE_MIN_FRAG_MORE                NET_IPv6_FRAG_SIZE_UNIT
#define  NET_IPv6_FRAG_SIZE_MIN_FRAG_LAST                  1u

#define  NET_IPv6_FRAG_SIZE_MAX                        65535u

//                                                                 IPv6 frag reasm timeout (see Note #2) :
#define  NET_IPv6_FRAG_REASM_TIMEOUT_MIN_SEC               1u   // IPv6 frag reasm timeout min  =  1 seconds
#define  NET_IPv6_FRAG_REASM_TIMEOUT_MAX_SEC             120u   // IPv6 frag reasm timeout max  = 10 seconds
#define  NET_IPv6_FRAG_REASM_TIMEOUT_DFLT_SEC             60u   // IPv6 frag reasm timeout dflt = 60 seconds

/********************************************************************************************************
 *                                           IPv6 ADDRESS DEFINES
 *
 * Note(s) : (1) See the RFC #4291 'IPv6 Addressing Architecture' for IPv6 address summary.
 *******************************************************************************************************/

#define  NET_IPv6_MCAST_RSVD_ADDR_MAX_VAL               (DEF_INT_08U_MAX_VAL & DEF_NIBBLE_MASK)

/********************************************************************************************************
 *                                   IPv6 SOURCE ADDR SELECTION DEFINES
 *
 * Note(s) : (1) Priority order of rules to select source address.
 *******************************************************************************************************/

#define  NET_IPv6_SRC_SEL_RULE_MIN                      1u
#define  NET_IPv6_SRC_SEL_RULE_MAX                    255u

#define  NET_IPv6_SRC_SEL_RULE_NONE                     NET_IPv6_SRC_SEL_RULE_MAX

#define  NET_IPv6_SRC_SEL_RULE_01                       1u
#define  NET_IPv6_SRC_SEL_RULE_02                       2u
#define  NET_IPv6_SRC_SEL_RULE_03                       3u
#define  NET_IPv6_SRC_SEL_RULE_04                       4u
#define  NET_IPv6_SRC_SEL_RULE_05                       5u
#define  NET_IPv6_SRC_SEL_RULE_06                       6u
#define  NET_IPv6_SRC_SEL_RULE_07                       7u
#define  NET_IPv6_SRC_SEL_RULE_08                       8u

/********************************************************************************************************
 *                                       IPv6 ADDRESS TYPE DEFINES
 *******************************************************************************************************/

#define  NET_IPv6_ADDR_TYPE_NONE                           0u
#define  NET_IPv6_ADDR_TYPE_UNSPECIFIED                    1u
#define  NET_IPv6_ADDR_TYPE_LOOPBACK                       2u
#define  NET_IPv6_ADDR_TYPE_UNICAST                        3u
#define  NET_IPv6_ADDR_TYPE_LINK_LOCAL                     4u
#define  NET_IPv6_ADDR_TYPE_SITE_LOCAL                     5u
#define  NET_IPv6_ADDR_TYPE_MCAST                          6u
#define  NET_IPv6_ADDR_TYPE_MCAST_SOL                      7u
#define  NET_IPv6_ADDR_TYPE_MCAST_ROUTERS                  8u
#define  NET_IPv6_ADDR_TYPE_MCAST_ROUTERS_MLDV2            9u
#define  NET_IPv6_ADDR_TYPE_MCAST_NODES                   10u
#define  NET_IPv6_ADDR_TYPE_MCAST_RSVD                    12u

/********************************************************************************************************
 *                                       IPv6 ADDRESS PREFIX DEFINES
 *******************************************************************************************************/

#define  NET_IPv6_ADDR_PREFIX_CUSTOM                       0u
#define  NET_IPv6_ADDR_PREFIX_LINK_LOCAL                   1u
#define  NET_IPv6_ADDR_PREFIX_SITE_LOCAL                   2u
#define  NET_IPv6_ADDR_PREFIX_MCAST                        3u

#define  NET_IPv6_ADDR_PREFIX_LINK_LOCAL_LEN              64u

#define  NET_IPv6_ADDR_PREFIX_LEN_MIN                      0u
#define  NET_IPv6_ADDR_PREFIX_LEN_MAX                    128u

/********************************************************************************************************
 *                               IPv6 ADDRESS AUTO CONFIGURATION ID DEFINES
 *******************************************************************************************************/

#define  NET_IPv6_ADDR_AUTO_CFG_ID_NONE                    0u
#define  NET_IPv6_ADDR_AUTO_CFG_ID_IEEE_48                 1u

#define  NET_IPv6_ADDR_AUTO_CFG_ID_LEN_NONE                0u
#define  NET_IPv6_ADDR_AUTO_CFG_ID_LEN_IEEE_48            64u

/********************************************************************************************************
 *                       IPv6 HOP-BY-HOP & DESTINATION OPTION EXTENSION HEADER DEFINES
 *******************************************************************************************************/

#define  NET_IPv6_EH_TLV_TYPE_ACT_MASK                  0xC0
#define  NET_IPv6_EH_TLV_TYPE_CHG_MASK                  0x20
#define  NET_IPv6_EH_TLV_TYPE_OPT_MASK                  0x1F

#define  NET_IPv6_EH_TLV_TYPE_ACT_SKIP                  0x00
#define  NET_IPv6_EH_TLV_TYPE_ACT_DISCARD               0x40
#define  NET_IPv6_EH_TLV_TYPE_ACT_DISCARD_IPPM          0x80
#define  NET_IPv6_EH_TLV_TYPE_ACT_DISCARD_IPPM_MC       0xC0

#define  NET_IPv6_EH_TLV_TYPE_CHG_SHIFT                    6u

#define  NET_IPv6_EH_TYPE_PAD1                          0x00
#define  NET_IPv6_EH_TYPE_PADN                          0x01
#define  NET_IPv6_EH_TYPE_ROUTER_ALERT                  0x05

/********************************************************************************************************
 *                                       IPv6 Router Alert Option Values
 *
 *   Reference
 *           [RFC2711]
 *
 *       Value                          Description                                        Reference
 *           0      Datagram contains a Multicast Listener Discovery message                 [RFC2710]
 *           1      Datagram contains RSVP message                                           [RFC2711]
 *           2      Datagram contains an Active Networks message                             [RFC2711]
 *           3      Reserved                                                                 [RFC5350]
 *       4-35     Aggregated Reservation Nesting Level                                     [RFC3175]
 *       36-67    QoS NSLP Aggregation Levels 0-31                                         [RFC5974]
 *       68      NSIS NATFW NSLP                                                          [RFC5973]
 *       69-65502   Unassigned
 *   65503-65534 Reserved for experimental use                                            [RFC5350]
 *       65535    Reserved                                                                 [IANA   ]
 *******************************************************************************************************/

#define  NET_IPv6_EH_TYPE_ROUTER_ALERT_MLDP           0x0000
#define  NET_IPv6_EH_TYPE_ROUTER_ALERT_RSVP           0x0001
#define  NET_IPv6_EH_TYPE_ROUTER_ALERT_AN             0x0002

/********************************************************************************************************
 *                               IPv6 EXTENSION HEADER SIZE ALIGNMENT DEFINES
 *******************************************************************************************************/

#define  NET_IPv6_EH_ALIGN_SIZE                            8u

/********************************************************************************************************
 *                                       IPv6 NEXT HEADER FIELD OFFSET
 *******************************************************************************************************/

#define  NET_IPv6_HDR_NEXT_HDR_IX                          6u

/********************************************************************************************************
 *                               IPv6 ROUTING EXTENSION HEADER ROUTING TYPE
 *******************************************************************************************************/

#define  NET_IPv6_EH_ROUTING_TYPE_0                        0u
#define  NET_IPv6_EH_ROUTING_TYPE_1                        1u
#define  NET_IPv6_EH_ROUTING_TYPE_2                        2u

/********************************************************************************************************
 *                                   IPv6 INTERNET TIMESTAMP DEFINES
 *******************************************************************************************************/

#define  NET_IPv6_TS_NONE                         ((NET_TS)0u)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       IPv6 NEXT HDR DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IPv6_NEXT_HDR;

/********************************************************************************************************
 *                                       IPv6 TLV TYPE DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IPv6_TLV_TYPE;

/********************************************************************************************************
 *                                       IPv6 TLV LENGTH DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IPv6_TLV_LEN;

/********************************************************************************************************
 *                                       IPv6 HOP BY HOP OPTION DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IPv6_OPT_TYPE;

/********************************************************************************************************
 *                                       IPv6 ADDRESS DATA TYPES
 *
 * Note(s) : (1) 'NET_IPv6_ADDR' pre-defined in 'net_type.h' PRIOR to all other network modules that
 *                   require IPv6 address data type.
 *******************************************************************************************************/

typedef enum net_ipv6_addr_cfg_type {
  NET_IPv6_ADDR_CFG_TYPE_NONE,
  NET_IPv6_ADDR_CFG_TYPE_STATIC_BLOCKING,
  NET_IPv6_ADDR_CFG_TYPE_STATIC_NO_BLOCKING,
  NET_IPv6_ADDR_CFG_TYPE_AUTO_CFG_BLOCKING,
  NET_IPv6_ADDR_CFG_TYPE_AUTO_CFG_NO_BLOCKING,
  NET_IPv6_ADDR_CFG_TYPE_RX_PREFIX_INFO,
} NET_IPv6_ADDR_CFG_TYPE;

typedef  enum {
  NET_IPv6_ADDR_STATE_NONE,
  NET_IPv6_ADDR_STATE_PREFERRED,
  NET_IPv6_ADDR_STATE_DEPRECATED,
  NET_IPv6_ADDR_STATE_TENTATIVE,
  NET_IPv6_ADDR_STATE_DUPLICATED
} NET_IPv6_ADDR_STATE;

struct  net_ipv6_addr_obj {
  NET_IF_NBR             IfNbr;
  NET_IPv6_ADDR          AddrHost;                              // IPv6 host                addr.
  NET_IPv6_ADDR          *AddrMcastSolicitedPtr;
  NET_IP_ADDR_CFG_MODE   AddrCfgMode;                           // IPv6                     addr auto cfg state.
  NET_IPv6_ADDR_CFG_TYPE AddrCfgType;
  CPU_INT08U             AddrHostPrefixLen;                     // IPv6 host                addr prefix len (in bytes).
  NET_IPv6_ADDR_STATE    AddrState;
  CPU_BOOLEAN            IsValid;
  CPU_BOOLEAN            DAD_En;
  NET_TMR                *PrefLifetimeTmrPtr;
  NET_TMR                *ValidLifetimeTmrPtr;
  SLIST_MEMBER           ListNode;
};

/********************************************************************************************************
 *                                               IPv6 HEADER
 *
 * Note(s) : (1) See RFC #2460, Section 3 for IPv6 datagram header format.
 *
 *           (2) (a) IPv6 Version Number, Traffic Class & Flow Label are encoded in the first word of an IPv6
 *                   header as follows :
 *
 *                                           31 28 27     20 19   0
 *                                       ------------------------
 *                                       | VER | TRAFFIC | FLOW |
 *                                       ------------------------
 *
 *                       where
 *                               VER         IPv6 version; currently 6 (see 'net_ipv6.h  Note #1')
 *                               TRAFFIC     Traffic Class; classes or priorities of IPv6 packets.
 *                               FLOW        Flow Label; label of sequences of packets.
 *
 *               (b) See RFC #2474 for Traffic Class field detailed description.
 *
 *               (c) See RFC #6437 for Flow Label    field detailed description.
 *
 *           (3) (a) IPv6 Payload Length, Next Header and Hop Limit are encoded in the seconrd word of an IPv6
 *                   header as follows :
 *
 *                                       31        16 15      8 7      0
 *                                   ---------------------------------
 *                                   | PAYLOADLEN | NEXTHDR | HOPLIM |
 *                                   ---------------------------------
 *
 *                       where
 *                               PAYLOADLEN  Payload Len; Length of the IPv6 payload following this IPv6 header.
 *                               NEXTHDR     Next Header; Identifies the type of header following the IPv6 header.
 *                               HOPLIM      Hop Limit; Decremented by 1 by each node that forwards the packet.
 *
 *           (4) (a) IPv6 Source Address is a 128-bit addresses specifying the originator of the packet
 *                   described as follow:
 *
 *                   127                                                                 0
 *                   -----------------------------------------------------------------------
 *                   |                              ADDR SRC                               |
 *                   -----------------------------------------------------------------------
 *
 *                       where
 *                               ADDR SCR    Source Address; 128-bit address of the originator of the packet.
 *
 *               (b) IPv6 Destination Address is a 128-bit addresses specifying the recipient of the packet
 *                   described as follow:
 *
 *                   127                                                                 0
 *                   -----------------------------------------------------------------------
 *                   |                              ADDR DEST                              |
 *                   -----------------------------------------------------------------------
 *
 *                       where
 *                               ADDR DEST   Destination Address; 128-bit address of the recipient of the packet.
 *
 *               (c) See RFC #4291 for more details on IPv6 addressing architecture.
 *******************************************************************************************************/

//                                                                 ------------------- NET IPv6 HDR -------------------
typedef  struct  net_IPv6_hdr {
  CPU_INT32U        VerTrafficFlow;                             // IPv6 ver nbr/traffic class/flow label (see Note #2).
  CPU_INT16U        PayloadLen;                                 // IPv6 payload len.
  NET_IPv6_NEXT_HDR NextHdr;                                    // IPv6 next hdr.
  CPU_INT08U        HopLim;                                     // IPv6 hop lim.
  NET_IPv6_ADDR     AddrSrc;                                    // IPv6 src  addr.
  NET_IPv6_ADDR     AddrDest;                                   // IPv6 dest addr.
} NET_IPv6_HDR;

/********************************************************************************************************
 *                                           IPv6 PSEUDO-HEADER
 *
 * Note(s) : (1) See RFC #2460, Section 8.1 'Upper-Layer Checksums' for IPv6 pseudo-header format.
 *******************************************************************************************************/

//                                                                 --------------- NET IPv6 PSEUDO-HDR ----------------
typedef  struct  net_ipv6_pseudo_hdr {
  NET_IPv6_ADDR AddrSrc;                                        // IPv6 dataram  src  addr.
  NET_IPv6_ADDR AddrDest;                                       // IPv6 datagram dest addr.
  CPU_INT32U    UpperLayerPktLen;                               // Len of the upper-layer hdr & data.
  CPU_INT16U    Zero;                                           // Field MUST be zero'd; i.e. ALL bits clr'd.
  CPU_INT16U    NextHdr;                                        // Next hdr. First 8-bits MUST be zero'd.
} NET_IPv6_PSEUDO_HDR;

#define  NET_IPv6_PSEUDO_HDR_SIZE                (sizeof(NET_IPv6_PSEUDO_HDR))

/********************************************************************************************************
 *                                       IPv6 EXTENTION HEADERS
 *******************************************************************************************************/

typedef struct net_ipv6_ext_hdr NET_IPv6_EXT_HDR;

struct  net_ipv6_ext_hdr {
  NET_IPv6_EXT_HDR *NextHdrPtr;
  NET_IPv6_EXT_HDR *PrevHdrPtr;
  CPU_INT08U       Type;
  CPU_INT16U       Len;
  CPU_INT08U       SortKey;
  void             *Fnct;
  void             *Arg;
};

/********************************************************************************************************
 *                                   IPv6 EXTENTION HEADERS ARGUMENTS
 *******************************************************************************************************/

typedef  struct  net_ipv6_ext_hdr_arg_generic {
  CPU_INT08U NextHdr;
  NET_BUF    *BufPtr;
  CPU_INT16U BufIx;
} NET_IPv6_EXT_HDR_ARG_GENERIC;

typedef  struct  net_ipv6_ext_hdr_arg_frag {
  CPU_INT08U NextHdr;
  NET_BUF    *BufPtr;
  CPU_INT16U BufIx;
  CPU_INT16U FragOffset;
  CPU_INT32U FragID;
} NET_IPv6_EXT_HDR_ARG_FRAG;

/********************************************************************************************************
 *                               IPv6 HOP-BY-HOP & DESTINATION OPTION HEADER
 *
 * Note(s) : (1) See RFC #2460
 *******************************************************************************************************/

//                                                                 -------- NET IPv6 HOP-BY-HOP & DEST OPT HDR --------
typedef  struct  net_ipv6_opt_hdr {
  NET_IPv6_NEXT_HDR NextHdr;                                    // Next hdr.
  CPU_INT08U        HdrLen;                                     // Size of hdr in octets, excluding 1st octet.
  CPU_INT08U        Opt[1];                                     // Variable option field.
} NET_IPv6_OPT_HDR;

/********************************************************************************************************
 *                                           IPv6 ROUTING HEADER
 *
 * Note(s) : (1) See RFC #2460
 *******************************************************************************************************/

//                                                                 --------------- NET IPV6 ROUTING HDR ---------------
typedef  struct  net_ipv6_routing_hdr {
  NET_IPv6_NEXT_HDR NextHdr;                                    // Next hdr.
  CPU_INT08U        HdrLen;                                     // Size of hdr in octets, excluding 1st octet.
  CPU_INT08U        RoutingType;                                // Routing type field.
  CPU_INT08U        SegLeft;                                    // Segment left field.
  CPU_INT32U        Rsvd;                                       // Reserved     field.
  NET_IPv6_ADDR     Addr[1];                                    // Address routing table.
} NET_IPv6_ROUTING_HDR;

/********************************************************************************************************
 *                                           IPv6 FRAGMENT HEADER
 *
 * Note(s) : (1) See RFC #2460
 *******************************************************************************************************/

//                                                                 ---------------- NET IPV6 FRAG HDR -----------------
typedef  struct  net_ipv6_frag_hdr {
  NET_IPv6_NEXT_HDR   NextHdr;                                   // Next hdr.
  CPU_INT08U          Rsvd;                                      // Reserved                 field.
  NET_IPv6_FRAG_FLAGS FragOffsetFlag;                            // Frag offset & frag flags field.
  CPU_INT32U          ID;                                        // ID                       field.
} NET_IPv6_FRAG_HDR;

#define  NET_IPv6_FRAG_HDR_SIZE  sizeof(NET_IPv6_FRAG_HDR)

/********************************************************************************************************
 *                                       IPv6 AUTHENTICATION HEADER
 *
 * Note(s) : (1) See RFC #4302
 *******************************************************************************************************/

//                                                                 ---------------- NET IPV6 FRAG HDR -----------------
typedef  struct  net_ipv6_authentication_hdr {
  NET_IPv6_NEXT_HDR NextHdr;                                    // Next hdr.
  CPU_INT08U        HdrLen;                                     // Hdr Len.
  CPU_INT16U        Rsvd;                                       // Reserved        field.
  CPU_INT32U        SPI;                                        // Security parameters index.
  CPU_INT32U        SeqNum;                                     // Sequence number field.
  CPU_INT32U        ICV[1];                                     // Integrety check value. Variable len.
} NET_IPv6_AUTHENTICATION_HDR;

/********************************************************************************************************
 *                               IPv6 ENCAPSULATING SECURITY PAYLOAD HEADER
 *
 * Note(s) : (1) See RFC #4303
 *******************************************************************************************************/

//                                                                 ---------------- NET IPV6 FRAG HDR -----------------
typedef  struct  net_ipv6_esp_hdr {
  CPU_INT32U SPI;                                               // Security parameters index.
  CPU_INT32U SeqNum;                                            // Sequence number field.
  CPU_INT08U Payload[4];                                        // Payload, padding, next header & ICV. Variable len.
} NET_IPv6_ESP_HDR;

/********************************************************************************************************
 *                                   EXTENSION HEADER TLV OPTION DATA TYPE
 *
 * Notes : (1) TLV (Type-Length-Value) Option format :
 *
 *               0               7               15
 *               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- - - - - - - - -
 *               |  Option Type  |  Opt Data Len |  Option Data
 *               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+- - - - - - - - -
 *******************************************************************************************************/

typedef  struct  net_ipv6_ext_hdr_tlv {
  NET_IPv6_TLV_TYPE Type;
  NET_IPv6_TLV_LEN  Len;
  CPU_INT08U        Val[2];
} NET_IPv6_EXT_HDR_TLV;

/********************************************************************************************************
 *                                   IPv6 AUTOCONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef enum net_ipv6_auto_cfg_state {
  NET_IPv6_AUTO_CFG_STATE_NONE,
  NET_IPv6_AUTO_CFG_STATE_STARTED_LOCAL,
  NET_IPv6_AUTO_CFG_STATE_STARTED_GLOBAL,
  NET_IPv6_AUTO_CFG_STATE_STOPPED,
} NET_IPv6_AUTO_CFG_STATE;

struct net_ipv6_auto_cfg_obj {
  CPU_BOOLEAN             En;
  CPU_BOOLEAN             DAD_En;
  NET_IPv6_AUTO_CFG_STATE State;
  NET_IPv6_ADDR           *AddrLocalPtr;
  NET_IPv6_ADDR           *AddrGlobalPtr;
  NET_TMR                 *RouterSolRetryTmr;
  NET_IPv6_ADDR           *RouterSolAddrSrcPtr;
  CPU_INT08U              RouterSolRetryCnt;
  SLIST_MEMBER            ListNode;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NET_IPv6_TX_GET_ID()
 *
 * @brief    Get next IPv6 transmit identification number.
 *
 * @param    id  Variable that will receive the returned IPv6 transmit identification number.
 *
 *           NetIPv6_TxPktPrepareHdr(),
 *           NetIPv6_ReTxPktPrepareHdr(),
 *           NetIPv6_TxPktPrepareFragHdr().
 *
 *           This macro is an INTERNAL network protocol suite macro & SHOULD NOT be called by
 *           application function(s).
 *
 * @note         (1) Return IPv6 identification number is NOT converted from host-order to network-order.
 *******************************************************************************************************/

#define  NET_IPv6_TX_GET_ID(id)                 do { NET_UTIL_VAL_COPY_32(&(id), &NetIPv6_TxID_Ctr); \
                                                     NetIPv6_TxID_Ctr++;                           } while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetIPv6_Init(MEM_SEG  *p_mem_seg,
                  RTOS_ERR *p_err);

void NetIPv6_LinkStateSubscriber(NET_IF_NBR        if_nbr,
                                 NET_IF_LINK_STATE link_state);

CPU_BOOLEAN NetIPv6_AddrAutoCfgEnHandler(NET_IF_NBR  if_nbr,
                                         CPU_BOOLEAN dad_en,
                                         RTOS_ERR    *p_err);

//                                                                 -------------- CFG FNCTS ---------------
NET_IPv6_ADDR_CFG_STATUS NetIPv6_CfgAddrAddHandler(NET_IF_NBR             if_nbr,
                                                   NET_IPv6_ADDR          *p_addr,
                                                   CPU_INT08U             prefix_len,
                                                   CPU_INT32U             lifetime_valid,
                                                   CPU_INT32U             lifetime_preferred,
                                                   NET_IP_ADDR_CFG_MODE   cfg_mode,
                                                   CPU_BOOLEAN            dad_en,
                                                   NET_IPv6_ADDR_CFG_TYPE addr_cfg_type,
                                                   NET_IPv6_ADDR_OBJ      **p_addr_obj_rtn,
                                                   RTOS_ERR               *p_err);

CPU_BOOLEAN NetIPv6_CfgAddrRemoveHandler(NET_IF_NBR    if_nbr,
                                         NET_IPv6_ADDR *p_addr_host,
                                         RTOS_ERR      *p_err);

CPU_BOOLEAN NetIPv6_CfgAddrRemoveAllHandler(NET_IF_NBR if_nbr,
                                            RTOS_ERR   *p_err);

NET_IPv6_ADDR_CFG_STATUS NetIPv6_CfgAddrReset(NET_IF_NBR    if_nbr,
                                              NET_IPv6_ADDR *p_addr,
                                              RTOS_ERR      *p_err);

//                                                                 -------------- GET FNCTS ---------------
CPU_BOOLEAN NetIPv6_GetAddrHostHandler(NET_IF_NBR       if_nbr,
                                       NET_IPv6_ADDR    *p_addr_tbl,
                                       NET_IP_ADDRS_QTY *p_addr_tbl_qty,
                                       RTOS_ERR         *p_err);

const NET_IPv6_ADDR_OBJ *NetIPv6_GetAddrSrcHandler(NET_IF_NBR          *p_if_nbr,
                                                   const NET_IPv6_ADDR *p_addr_src,
                                                   const NET_IPv6_ADDR *p_addr_dest,
                                                   NET_IPv6_ADDR       *p_addr_nexthop,
                                                   RTOS_ERR            *p_err);

NET_IF_NBR NetIPv6_GetAddrHostIF_Nbr(const NET_IPv6_ADDR *p_addr);

NET_IPv6_ADDR_OBJ *NetIPv6_GetAddrHostMatchPrefix(NET_IF_NBR    if_nbr,
                                                  NET_IPv6_ADDR *p_prefix,
                                                  CPU_INT08U    prefix_len);

NET_IPv6_ADDR *NetIPv6_GetAddrLinkLocalCfgd(NET_IF_NBR if_nbr);

NET_IPv6_ADDR_OBJ *NetIPv6_GetAddrsHost(const NET_IPv6_ADDR *p_addr,
                                        NET_IF_NBR          *p_if_nbr);

NET_IPv6_ADDR_OBJ *NetIPv6_GetAddrsHostOnIF(NET_IF_NBR          if_nbr,
                                            const NET_IPv6_ADDR *p_addr);

//                                                                 ------------- STATUS FNCTS -------------
CPU_BOOLEAN NetIPv6_IsAddrHostCfgdHandler(const NET_IPv6_ADDR *p_addr);

CPU_BOOLEAN NetIPv6_IsAddrCfgdValidHandler(const NET_IPv6_ADDR *p_addr);

CPU_BOOLEAN NetIPv6_IsAddrsCfgdOnIF_Handler(NET_IF_NBR if_nbr,
                                            RTOS_ERR   *p_err);

CPU_BOOLEAN NetIPv6_IsAddrWildcard(NET_IPv6_ADDR *p_addr);

CPU_BOOLEAN NetIPv6_IsValidHopLim(NET_IPv6_HOP_LIM hop_lim);

CPU_BOOLEAN NetIPv6_IsAddrAndMaskValid(const NET_IPv6_ADDR *p_addr,
                                       const NET_IPv6_ADDR *p_mask);

CPU_BOOLEAN NetIPv6_IsAddrAndPrefixLenValid(const NET_IPv6_ADDR *p_addr,
                                            const NET_IPv6_ADDR *p_prefix,
                                            CPU_INT08U          prefix_len,
                                            RTOS_ERR            *p_err);

CPU_BOOLEAN NetIPv6_IsAddrsIdentical(const NET_IPv6_ADDR *p_addr1,
                                     const NET_IPv6_ADDR *p_addr2);

//                                                                 ------------- SETUP FNCTS --------------
void NetIPv6_AddrHW_McastSet(CPU_INT08U    *p_addr_mac_ascii,
                             NET_IPv6_ADDR *p_addr);

void NetIPv6_AddrMcastSolicitedSet(NET_IPv6_ADDR *p_addr_result,
                                   NET_IPv6_ADDR *p_addr_input,
                                   NET_IF_NBR    if_nbr,
                                   RTOS_ERR      *p_err);

void NetIPv6_AddrMcastAllRoutersSet(NET_IPv6_ADDR *p_addr,
                                    CPU_BOOLEAN   mldp_v2);

void NetIPv6_AddrMcastAllNodesSet(NET_IPv6_ADDR *p_addr);

void NetIPv6_AddrLoopbackSet(NET_IPv6_ADDR *p_addr);

void NetIPv6_AddrUnspecifiedSet(NET_IPv6_ADDR *p_addr);

//                                                                 --------------- RX FNCTS ---------------
void NetIPv6_Rx(NET_BUF  *p_buf,
                RTOS_ERR *p_err);

//                                                                 --------------- TX FNCTS ---------------
//                                                                 Prepare & tx IPv6 pkts.
void NetIPv6_Tx(NET_BUF                *p_buf,
                NET_IPv6_ADDR          *p_addr_src,
                NET_IPv6_ADDR          *p_addr_dest,
                NET_IPv6_EXT_HDR       *p_ext_hdr_list,
                NET_IPv6_TRAFFIC_CLASS traffic_class,
                NET_IPv6_FLOW_LABEL    flow_label,
                NET_IPv6_HOP_LIM       hop_lim,
                RTOS_ERR               *p_err);

//                                                                 Prepare & re-tx IPv6 pkts.
void NetIPv6_ReTx(NET_BUF  *p_buf,
                  RTOS_ERR *p_err);

void NetIPv6_GetTxDataIx(NET_IF_NBR       if_nbr,
                         NET_IPv6_EXT_HDR *p_ext_hdr_list,
                         CPU_INT32U       data_len,
                         CPU_INT16U       mtu,
                         CPU_INT16U       *p_ix);

//                                                                 Prepare Extension Header to Tx.
NET_IPv6_EXT_HDR *NetIPv6_ExtHdrAddToList(NET_IPv6_EXT_HDR *p_ext_hdr_head,
                                          NET_IPv6_EXT_HDR *p_ext_hdr,
                                          CPU_INT08U       type,
                                          CPU_INT16U       len,
                                          CPU_FNCT_PTR     fnct,
                                          CPU_INT08U       sort_key,
                                          RTOS_ERR         *p_err);

void NetIPv6_PrepareFragHdr(void *p_ext_hdr_arg);

//                                                                 ------------ NET MGR FNCTS -------------
void NetIPv6_GetHostAddrProtocol(NET_IF_NBR if_nbr,
                                 CPU_INT08U *p_addr_protocol_tbl,
                                 CPU_INT08U *p_addr_protocol_tbl_qty,
                                 CPU_INT08U *p_addr_protocol_len,
                                 RTOS_ERR   *p_err);

NET_IF_NBR NetIPv6_GetAddrProtocolIF_Nbr(CPU_INT08U *p_addr_protocol,
                                         CPU_INT08U addr_protocol_len,
                                         RTOS_ERR   *p_err);

CPU_BOOLEAN NetIPv6_IsValidAddrProtocol(CPU_INT08U *p_addr_protocol,
                                        CPU_INT08U addr_protocol_len);

CPU_BOOLEAN NetIPv6_IsAddrInit(CPU_INT08U *p_addr_protocol,
                               CPU_INT08U addr_protocol_len);

#ifdef  NET_MCAST_MODULE_EN
CPU_BOOLEAN NetIPv6_IsAddrProtocolMulticast(CPU_INT08U *p_addr_protocol,
                                            CPU_INT08U addr_protocol_len);
#endif

//                                                                 --------- IPv6 ADDRESS TIMERS ----------
void NetIPv6_AddrValidLifetimeTimeout(void *p_ipv6_addr_timeout);

void NetIPv6_AddrPrefLifetimeTimeout(void *p_ipv6_addr_timeout);

#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
void NetIPv6_AddrAutoCfgComp(NET_IF_NBR               if_nbr,
                             NET_IPv6_ADDR_CFG_STATUS auto_cfg_status);

NET_IPv6_AUTO_CFG_OBJ *NetIPv6_GetAddrAutoCfgObj(NET_IF_NBR if_nbr);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IPv6_MODULE_EN
#endif // _NET_IPv6_PRIV_H_
