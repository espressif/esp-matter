/***************************************************************************//**
 * @file
 * @brief Network Buffer Management
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

#ifndef  _NET_BUF_PRIV_H_
#define  _NET_BUF_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_cfg_net.h"
#include  "../../include/net_if.h"

#include  "net_type_priv.h"
#include  "net_stat_priv.h"
#include  "net_tmr_priv.h"

#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_BUF_ID_NONE                                   0u
#define  NET_BUF_ID_INIT                     NET_BUF_ID_NONE

/********************************************************************************************************
 *                                       NETWORK BUFFER FLAG DEFINES
 *******************************************************************************************************/

//                                                                 ------------------- NET BUF FLAGS ------------------
#define  NET_BUF_FLAG_NONE                        DEF_BIT_NONE
#define  NET_BUF_FLAG_USED                        DEF_BIT_00    // Buf cur used; i.e. NOT in free buf pool.
#define  NET_BUF_FLAG_CLR_MEM                     DEF_BIT_01    // Buf mem clr'd.

#define  NET_BUF_FLAG_RX_BROADCAST                DEF_BIT_04    // Indicates pkts rx'd via  broadcast.
#define  NET_BUF_FLAG_RX_MULTICAST                DEF_BIT_05    // Indicates pkts rx'd via  multicast.
#define  NET_BUF_FLAG_RX_REMOTE                   DEF_BIT_06    // Indicates pkts rx'd from remote host.
#define  NET_BUF_FLAG_RX_UDP_CHK_SUM_VALID        DEF_BIT_07    // Indicates UDP  rx chk sum valid.

#define  NET_BUF_FLAG_TX_BROADCAST                DEF_BIT_12    // Indicates pkts to tx via broadcast.
#define  NET_BUF_FLAG_TX_MULTICAST                DEF_BIT_13    // Indicates pkts to tx via multicast.

#define  NET_BUF_FLAG_TX_LOCK                     DEF_BIT_14    // Protects  pkts from concurrent dev/hw tx.

#define  NET_BUF_FLAG_IPv6_FRAME                  DEF_BIT_15    // Indicates IPv6 frame.

/********************************************************************************************************
 *                                   NETWORK BUFFER INDEX & SIZE DEFINES
 *
 * Note(s) : (1) (a) (1) NET_BUF_DATA_PROTOCOL_HDR_SIZE_MIN's  value is pre-#define'd in 'net_cfg_net.h' :
 *
 *                       (A) NET_BUF_DATA_PROTOCOL_HDR_SIZE_MIN's ideal #define'tion :
 *
 *                               (1) min(IF Headers)  +  min(Protocol Headers)
 *
 *                       (B) NET_BUF_DATA_PROTOCOL_HDR_SIZE_MIN  #define'd with hard-coded knowledge that IF &/or
 *                           ARP, IP/ICMP, IP/IGMP, IP/UDP headers have the smallest combined minimum size of all
 *                           the protocol headers :
 *
 *                               ARP Hdr   28       IP Hdr   20      IP Hdr   20      IP Hdr   20     IP Hdr   20
 *                                               ICMP Hdr    8    IGMP Hdr    8     UDP Hdr    8    TCP Hdr   20
 *                               ------------     -------------    -------------     ------------    ------------
 *                               Total     28     Total      28    Total      28     Total     28    Total     40
 *
 *                   (2) NET_BUF_DATA_PROTOCOL_HDR_SIZE_MAX's  value is pre-#define'd in 'net_cfg_net.h' :
 *
 *                       (A) NET_BUF_DATA_PROTOCOL_HDR_SIZE_MAX's ideal #define'tion :
 *
 *                               (1) max(IF Headers)  +  max(Protocol Headers)
 *
 *                       (B) NET_BUF_DATA_PROTOCOL_HDR_SIZE_MAX  #define'd with hard-coded knowledge that IF, IP,
 *                           & TCP headers have the largest combined maximum size of all the protocol headers :
 *
 *                               ARP Hdr   68       IP Hdr   60      IP Hdr   60      IP Hdr   60     IP Hdr   60
 *                                               ICMP Hdr   20    IGMP Hdr    8     UDP Hdr    8    TCP Hdr   60
 *                               ------------     -------------    -------------     ------------    ------------
 *                               Total     68     Total      80    Total      68     Total     68    Total    120
 *
 *               (b) (1) Assumes minimum ARP header size of 28 octets based on Ethernet hardware & IP protocol
 *                       addresses.  Actual ARP header size depends on actual hardware & protocol address lengths.
 *
 *                   (2) Assumes maximum ARP header size of 68 octets based on maximum length hardware & protocol
 *                       addresses.  Actual ARP header size depends on actual hardware & protocol address lengths.
 *
 *                   See 'net_arp.h  Note #2' for supported hardware & protocol types.
 *
 *               (c) The minimum network buffer size MUST be configured greater than the maximum network packet
 *                   header sizes so that the first of any fragmented packets always contains a complete receipt
 *                   of all frame & network packet headers.
 *
 *           (2) (a) Since network data value macro's appropriately convert data values from any CPU addresses,
 *                   word-aligned or not; network receive & transmit packets are NOT required to ensure that
 *                   network packet headers (ARP/IP/UDP/TCP/etc.) & header members will locate on CPU word-
 *                   aligned addresses.  Therefore, network receive & transmit packets are NOT required to
 *                   start on any specific network buffer indices.
 *
 *                   See also 'net_util.h  NETWORK DATA VALUE MACRO'S  Note #2b'.
 *
 *               (b) However, many processors & network devices may be more efficient & may even REQUIRE that
 *                   memory transfers occur on CPU word-aligned addresses or on device-specific word-aligned
 *                   addresses [e.g. processors or devices with direct memory access (DMA) capability].
 *                   Therefore, network receive & transmit packets SHOULD start at the device layer on CPU
 *                   word-aligned or device-specific word-aligned indices(/addresses) in network buffer data
 *                   areas.
 *
 *                   (1) (A) Receive packet   index SHOULD be configured to ensure that the device layer receive
 *                           packet is word-aligned, either CPU word-aligned or device-specific word-aligned.
 *
 *                       (B) Network transmit index SHOULD be configured to ensure that the device layer transmit
 *                           packet is word-aligned, either CPU word-aligned or device-specific word-aligned.
 *
 *                           (1) However, this assumes that a single data index may be configured that can handle
 *                               all possible combinations of network packet header lengths while still ensuring
 *                               that each device's transmit packets are word-aligned.
 *
 *                               This assumption is valid if & only if all combinations of network packet header
 *                               lengths are multiples of the CPU's data word size &/or specific device's word
 *                               aligment.
 *
 *                   (2) These data indices also assume that each interface's network buffer data area(s) are
 *                       configured to be aligned to at least either the CPU's data word size &/or the device-
 *                       specific word alignment.
 *
 *                   See also 'net_dev_cfg.c  EXAMPLE NETWORK DEVICE CONFIGURATION  Note #4'
 *                           & 'lib_mem.h  MEMORY DATA VALUE MACRO'S  Note #1a'.
 *******************************************************************************************************/

#if  (defined(NET_IPv6_MODULE_EN))
    #define  NET_BUF_IP_HDR_SIZE    NET_IPv6_HDR_SIZE

#elif (defined  NET_IPv4_MODULE_EN)
    #define  NET_BUF_IP_HDR_SIZE    NET_IPv4_HDR_SIZE

#else
    #define  NET_BUF_IP_HDR_SIZE    0
#endif

#define  NET_BUF_PROTOCOL_HDR_SIZE_MIN        NET_UDP_HDR_SIZE_MIN

#if (defined(NET_TCP_MODULE_EN))
    #define  NET_BUF_PROTOCOL_HDR_SIZE_MAX    NET_TCP_HDR_SIZE_MAX
#else
    #define  NET_BUF_PROTOCOL_HDR_SIZE_MAX    NET_UDP_HDR_SIZE_MAX
#endif

#define  NET_BUF_DATA_PROTOCOL_HDR_SIZE_MIN             (NET_IF_HDR_SIZE_MIN   \
                                                         + NET_BUF_IP_HDR_SIZE \
                                                         + NET_BUF_PROTOCOL_HDR_SIZE_MIN)

#define  NET_BUF_DATA_PROTOCOL_HDR_SIZE_MAX             (NET_IF_HDR_SIZE_MAX   \
                                                         + NET_BUF_IP_HDR_SIZE \
                                                         + NET_BUF_PROTOCOL_HDR_SIZE_MAX)

//                                                                 ------------ NET BUF HDR/DATA IX's -------------
#define  NET_BUF_DATA_BASE                         0
#define  NET_BUF_DATA_PROTOCOL_HDR_BASE          NET_BUF_DATA_BASE
//                                                                 Data ix/sizes based on max pkt hdr sizes ...
//                                                                 ... (see Note #1a2).
#define  NET_BUF_DATA_IX                        (NET_BUF_DATA_PROTOCOL_HDR_BASE + NET_BUF_DATA_PROTOCOL_HDR_SIZE_MAX)
#define  NET_BUF_DATA_IX_RX                      NET_BUF_DATA_BASE  // See Note #2b1A.
#define  NET_BUF_DATA_IX_TX                      NET_BUF_DATA_IX    // See Note #2b1B.

#define  NET_BUF_DATA_SIZE_MIN                   NET_BUF_DATA_IX
#define  NET_BUF_DATA_PROTOCOL_HDR_SIZE          NET_BUF_DATA_IX

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   NETWORK BUFFER QUANTITY DATA TYPE
 *
 * Note(s) : (1) (a) 'NET_BUF_QTY' pre-defined in 'net_type.h' PRIOR to all other network modules that
 *                       require network interface queue size data type.
 *
 *               (b) The following network interface queue size values are pre-#define'd in 'net_type.h'
 *                   PRIOR to all other network modules that require network interface numbers values :
 *
 *                   (1) NET_BUF_NBR_MIN
 *                   (2) NET_BUF_NBR_MAX
 *
 *                       (A) NET_BUF_NBR_MAX  SHOULD be #define'd based on 'NET_BUF_QTY' data type declared.
 *******************************************************************************************************/

#if 0                                                           // See Note #1.
typedef  CPU_INT16U NET_BUF_QTY;                                // Defines max qty of net bufs to support.

#define  NET_BUF_NBR_MIN                                   1
#define  NET_BUF_NBR_MAX                 DEF_INT_16U_MAX_VAL    // See Note #1b2A.
#endif

#define  NET_BUF_NBR_RX_LARGE_MIN            NET_BUF_NBR_MIN
#define  NET_BUF_NBR_RX_LARGE_MAX            NET_BUF_NBR_MAX

#define  NET_BUF_NBR_TX_MIN                  NET_BUF_NBR_MIN
#define  NET_BUF_NBR_TX_MAX                  NET_BUF_NBR_MAX

#define  NET_BUF_NBR_TX_LARGE_MIN        DEF_INT_16U_MIN_VAL
#define  NET_BUF_NBR_TX_LARGE_MAX            NET_BUF_NBR_MAX

#define  NET_BUF_NBR_TX_SMALL_MIN        DEF_INT_16U_MIN_VAL
#define  NET_BUF_NBR_TX_SMALL_MAX            NET_BUF_NBR_MAX

/********************************************************************************************************
 *                               NETWORK BUFFER FUNCTION POINTER DATA TYPE
 *******************************************************************************************************/

typedef  void (*NET_BUF_FNCT)(NET_BUF *p_buf);

/********************************************************************************************************
 *                                   NETWORK BUFFER FLAGS DATA TYPE
 *******************************************************************************************************/

typedef  NET_FLAGS NET_BUF_FLAGS;

/********************************************************************************************************
 *                                       NETWORK BUFFER TYPE DEFINES
 *
 * Note(s) : (1) NET_BUF_TYPE_&&& #define values specifically chosen as ASCII representations of the network
 *               buffer types.  Memory displays of network buffers will display the network buffer TYPEs with
 *               their chosen ASCII names.
 *******************************************************************************************************/

typedef  enum  net_buf_type {
  NET_BUF_TYPE_NONE = 0,
  NET_BUF_TYPE_BUF,
  NET_BUF_TYPE_RX_LARGE,
  NET_BUF_TYPE_TX_LARGE,
  NET_BUF_TYPE_TX_SMALL
} NET_BUF_TYPE;

/********************************************************************************************************
 *                                   NETWORK BUFFER HEADER DATA TYPE
 *
 *                                            NET_BUF_HDR
 *                                          |-------------|
 *                                          | Buffer Type |
 *                                          |-------------|
 *                                          | Buffer Size |
 *                                          |-------------|
 *                                          |    Flags    |
 *                              Previous    |-------------|
 *                               Buffer <----------O      |
 *                                Lists     |-------------|      Next
 *                                          |      O----------> Buffer
 *                              Previous    |-------------|      Lists
 *                               Buffer <----------O      |
 *                                          |-------------|      Next
 *                                          |      O----------> Buffer
 *                                          |-------------|
 *                                          |      O----------> Buffer
 *                                          |-------------|      Timer
 *                                          |             |
 *                                          |      O----------> Unlink Fnct
 *                                          |             |      & Obj Ptrs
 *                                          |-------------|
 *                                          |  Reference  |
 *                                          |   Counter   |
 *                                          |-------------|
 *                                          |             |
 *                                          |             |
 *                                          |  Protocol   |
 *                                          |   Header    |
 *                                          |  Indices/   |
 *                                          |  Lengths    |
 *                                          |             |
 *                                          |             |
 *                                          |-------------|
 *                                          |             |
 *                                          |  Protocol   |
 *                                          |  Controls   |
 *                                          |             |
 *                                          | ----------- |
 *                                          |     ARP     |
 *                                          | ----------- |
 *                                          |     IP      |
 *                                          | ----------- |
 *                                          |   TCP/UDP   |
 *                                          | ----------- |
 *                                          |     TCP     |
 *                                          | ----------- |
 *                                          |    Conn     |
 *                                          |-------------|
 *
 *
 * Note(s) : (1) Protocol Header & Data indices into DATA area declared as 'CPU_INT16U' because current
 *               TCP/IPv4 header sizes do NOT exceed 'CPU_INT16U' index space.
 *
 *           (2) 'TotLen'/'DataLen' calculate total buffer data length & 'Application' data length.
 *
 *               For received buffers, total buffer data length is set to the incoming packet length;
 *               'Application' data length (& index) is calculated as total buffer data lengths minus
 *               all protocol header lengths found in the buffer.
 *
 *               For transmitted buffers, 'Application' data length (& index) is set by higher-layer
 *               network protocols or the application layer; total buffer data length is calculated
 *               as the 'Application' data length plus all protocol header lengths inserted into the
 *               buffer.
 *
 *           (3) The following variables ideally declared as specific TCP data types; declared as CPU
 *               data types because TCP data types NOT defined until 'net_tcp.h' (see 'net.h  NETWORK
 *               INCLUDE FILES') :
 *
 *               (a) TCP transmit acknowledgement code variables ideally declared as 'NET_TCP_ACK_CODE';
 *                       declared as 'CPU_INT08U'.
 *******************************************************************************************************/

//                                                                 ----------------------- NET BUF HDR ------------------------
typedef  struct  net_buf_hdr {
  NET_BUF_TYPE      Type;                               // Buf type cfg'd @ init.
  NET_BUF_SIZE      Size;                               // Buf size cfg'd @ init.
  NET_BUF_FLAGS     Flags;                              // Buf flags.

  NET_BUF_QTY       ID;                                 // Buf id.
  CPU_INT08U        RefCtr;                             // Nbr of ext refs pointing to this buf.

  NET_IF_NBR        IF_Nbr;                             // Buf's rx or base IF nbr.
  NET_IF_NBR        IF_NbrTx;                           // Buf's tx         IF nbr.

  NET_BUF           *PrevPrimListPtr;                   // Ptr to PREV prim list.
  NET_BUF           *NextPrimListPtr;                   // Ptr to NEXT prim list.

  NET_BUF           *PrevSecListPtr;                    // Ptr to PREV sec  list.
  NET_BUF           *NextSecListPtr;                    // Ptr to NEXT sec  list.

  NET_BUF           *PrevTxListPtr;                     // Ptr to NEXT tx   list buf.
  NET_BUF           *NextTxListPtr;                     // Ptr to PREV tx   list buf.

  NET_BUF           *PrevBufPtr;                        // Ptr to PREV buf.
  NET_BUF           *NextBufPtr;                        // Ptr to NEXT buf.

  NET_TMR           *TmrPtr;                            // Ptr to buf TMR.

  NET_BUF_FNCT      UnlinkFnctPtr;                      // Ptr to fnct used to unlink buf from multiple refs.
  void              *UnlinkObjPtr;                      // Ptr to obj       to unlink buf from.

  NET_PROTOCOL_TYPE ProtocolHdrType;                    // Cur       hdr protocol type.
  NET_PROTOCOL_TYPE ProtocolHdrTypeIF;                  // IF        hdr protocol type.
  NET_PROTOCOL_TYPE ProtocolHdrTypeIF_Sub;              // IF        sub-protocol type.
  NET_PROTOCOL_TYPE ProtocolHdrTypeNet;                 // Net       hdr protocol type.
  NET_PROTOCOL_TYPE ProtocolHdrTypeNetSub;              // Net       sub-protocol type.
  NET_PROTOCOL_TYPE ProtocolHdrTypeTransport;           // Transport hdr protocol type.

  CPU_INT16U        IF_HdrIx;                           // IF                   hdr  ix  (in DATA area).
  CPU_INT16U        IF_HdrLen;                          // IF                   hdr  len (in octets   ).

#ifdef  NET_DAD_MODULE_EN
  void              *IF_HW_AddrSrcPtr;                  // Pointer to HW IF src  address.
  void              *IF_HW_AddrDestPtr;                 // Pointer to HW IF dest address.
  CPU_INT08U        IF_HW_AddrLen;                      // Length of HW IF address.
#endif

#ifdef  NET_ARP_MODULE_EN
  CPU_INT16U         ARP_MsgIx;                         // ARP                  msg  ix  (in DATA area).
  CPU_INT16U         ARP_MsgLen;                        // ARP                  msg  len (in octets   ).
#endif

  CPU_INT16U         IP_HdrIx;                          // IP                   hdr  ix  (in DATA area).
  CPU_INT16U         IP_HdrLen;                         // IP                   hdr  len (in octets   ).
  CPU_INT16U         IP_TotLen;                         // IP                   tot  len (in octets   ).
  CPU_INT16U         IP_DataLen;                        // IP pkt/frag          data len (in octets   ).
  CPU_INT16U         IP_DatagramLen;                    // IP complete datagram data len (in octets   ).

  CPU_INT16U         ICMP_MsgIx;                        // ICMP                 msg  ix  (in DATA area).
  CPU_INT16U         ICMP_MsgLen;                       // ICMP                 msg  len (in octets   ).
  CPU_INT16U         ICMP_HdrLen;                       // ICMP                 hdr  len (in octets   ).

#ifdef  NET_IGMP_MODULE_EN
  CPU_INT16U         IGMP_MsgIx;                        // IGMP                 msg  ix  (in DATA area).
  CPU_INT16U         IGMP_MsgLen;                       // IGMP                 msg  len (in octets   ).
#endif

  CPU_INT16U         TransportHdrIx;                    // Transport            hdr  ix  (in DATA area).
  CPU_INT16U         TransportHdrLen;                   // Transport            hdr  len (in octets   ).
  CPU_INT16U         TransportTotLen;                   // Transport        tot      len (in octets   ).
  CPU_INT16U         TransportDataLen;                  // Transport        tot data len (in octets   ).

  CPU_INT16U         DataIx;                            // App                  DATA ix  (in DATA area).
  NET_BUF_SIZE       DataLen;                           // App                  DATA len (in octets   ).

  NET_BUF_SIZE       TotLen;                            // ALL                  DATA len (in octets   ).

#ifdef  NET_ARP_MODULE_EN
  CPU_INT08U         *ARP_AddrHW_Ptr;                   // Ptr to ARP hw       addr.
  CPU_INT08U         *ARP_AddrProtocolPtr;              // Ptr to ARP protocol addr.
#endif

  CPU_INT32U         IP_FragSizeTot;                    // Tot IP rx frag size.
  CPU_INT32U         IP_FragSizeCur;                    // Cur IP rx frag size.

#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_HDR_FLAGS IP_Flags_FragOffset;               // IP rx flags/frag offset.
  CPU_INT16U         IP_ID;                             // IP datagram id.
  NET_IPv4_ADDR      IP_AddrSrc;                        // IP src             addr.
  NET_IPv4_ADDR      IP_AddrDest;                       // IP dest            addr.
  NET_IPv4_ADDR      IP_AddrNextRoute;                  // IP tx 'Next-Route' addr.
  NET_IPv4_ADDR      IP_AddrNextRouteNetOrder;          // IP tx 'Next-Route' addr in net-order.
  NET_BUF            *IP_OptPtr;                        // Ptr to IP rx opts.
#endif

#ifdef  NET_IPv6_MODULE_EN
  NET_IPv6_FRAG_FLAGS IPv6_Flags_FragOffset;
  NET_IPv6_ADDR       IPv6_AddrSrc;                     // IPv6 src             addr.
  NET_IPv6_ADDR       IPv6_AddrDest;                    // IPv6 dest            addr.
  NET_IPv6_ADDR       IPv6_AddrNextRoute;               // IPv6 tx 'Next-Route' addr.

  CPU_INT16U          IPv6_ExtHdrLen;                   // IP                   hdr  len (in octets   ).

  CPU_INT16U          IPv6_HopByHopHdrIx;               // IPv6 HopByHop        hdr  ix. (in DATA area).
  CPU_INT16U          IPv6_RoutingHdrIx;                // IPv6 Routing         hdr  ix. (in DATA area).
  CPU_INT16U          IPv6_FragHdrIx;                   // IPv6 Fragmentation   hdr  ix. (in DATA area).
  CPU_INT16U          IPv6_ESP_HdrIx;                   // IPv6 ESP             hdr  ix. (in DATA area).
  CPU_INT16U          IPv6_AuthHdrIx;                   // IPv6 Authentication  hdr  ix. (in DATA area).
  CPU_INT16U          IPv6_DestHdrIx;                   // IPv6 Destination     hdr  ix. (in DATA area).
  CPU_INT16U          IPv6_MobilityHdrIx;               // IPv6 Mobility        hdr  ix. (in DATA area).

  CPU_INT32U          IPv6_ID;                          // IPv6 ID from frag    hdr.
#endif

#ifdef  NET_NDP_MODULE_EN
  CPU_INT08U *NDP_AddrHW_Ptr;                           // Ptr to NDP hw       addr.
  CPU_INT08U *NDP_AddrProtocolPtr;                      // Ptr to NDP protocol addr.
#endif

#ifdef  NET_MLDP_MODULE_EN
  CPU_INT16U           MLDP_MsgIx;                      // MLDP                 msg  ix  (in DATA area).
  CPU_INT16U           MLDP_MsgLen;                     // MDLP                 msg  len (in octets   ).
#endif

  NET_PORT_NBR         TransportPortSrc;                // Transport src  port.
  NET_PORT_NBR         TransportPortDest;               // Transport dest port.

#ifdef  NET_TCP_MODULE_EN
  NET_TCP_HDR_FLAGS    TCP_HdrLen_Flags;                // TCP hdr len/flags.

  NET_TCP_SEG_SIZE     TCP_SegLenInit;                  // TCP init  seg len.
  NET_TCP_SEG_SIZE     TCP_SegLen;                      // TCP       seg len [i.e. data len &/or sync/close ctrl(s)].
  NET_TCP_SEG_SIZE     TCP_SegLenLast;                  // TCP last  seg len [i.e. last len tx'd].
  NET_TCP_SEG_SIZE     TCP_SegLenData;                  // TCP data  seg len [i.e. data len].
  NET_PKT_CTR          TCP_SegReTxCtr;                  // Indicates nbr seg re-tx's.
  CPU_BOOLEAN          TCP_SegSync;                     // Indicates TCP sync  seg.
  CPU_BOOLEAN          TCP_SegClose;                    // Indicates TCP close seg.
  CPU_BOOLEAN          TCP_SegReset;                    // Indicates TCP reset seg.
  CPU_BOOLEAN          TCP_SegAck;                      // Indicates TCP ack   seg.
  CPU_BOOLEAN          TCP_SegAckTxd;                   // Indicates TCP ack tx'd        for rx'd seg.
  CPU_INT08U           TCP_SegAckTxReqCode;             // Indicates TCP ack tx req code for rx'd seg (see Note #3a).

  NET_TCP_SEQ_NBR      TCP_SeqNbrInit;                  // TCP init  seq nbr.
  NET_TCP_SEQ_NBR      TCP_SeqNbrLast;                  // TCP last  seq nbr
  NET_TCP_SEQ_NBR      TCP_SeqNbr;                      // TCP       seq nbr
  NET_TCP_SEQ_NBR      TCP_AckNbr;                      // TCP       ack nbr
  NET_TCP_SEQ_NBR      TCP_AckNbrLast;                  // TCP last  ack nbr
  NET_TCP_SEG_SIZE     TCP_MaxSegSize;                  // TCP max   seg size.
  NET_TCP_WIN_SIZE     TCP_WinSize;                     // TCP       win size.
  NET_TCP_WIN_SIZE     TCP_WinSizeLast;                 // TCP last  win size.

  NET_TCP_TX_RTT_TS_MS TCP_RTT_TS_Rxd_ms;               // TCP RTT TS @ seg rx'd (in ms).
  NET_TCP_TX_RTT_TS_MS TCP_RTT_TS_Txd_ms;               // TCP RTT TS @ seg tx'd (in ms).

  NET_TCP_FLAGS        TCP_Flags;                       // TCP tx flags.
#endif

  NET_CONN_ID          Conn_ID;                         // Net             conn id.
  NET_CONN_ID          Conn_ID_Transport;               // Transport layer conn id.
  NET_CONN_ID          Conn_ID_App;                     // App       layer conn id.

  CPU_INT08U           ConnType;                        // Conn type.
} NET_BUF_HDR;

/********************************************************************************************************
 *                                       NETWORK BUFFER DATA TYPE
 *
 *                                               NET_BUF
 *                                           |-------------|
 *                                           |             |
 *                                           |   Buffer    |
 *                                           |   Header    |
 *                                           |             |
 *                                           |-------------|
 *                                           |             |
 *                                           |   Buffer    |
 *                                           |    Data     |
 *                                           |   Pointer   |
 *                                           |             |
 *                                           |-------------|
 *
 * Note(s) : (1) (a) 'DataPtr' buffer data area is storage for both Protocol Headers & Application data.
 *
 *               (b) 'DataPtr' buffer data area SHOULD be declared with an additional CPU data word size
 *                       so that devices may efficiently & safely read or write data without overflowing the
 *                       data area.
 *
 *                   This additional CPU data word size does NOT increase the overall useable network
 *                   buffer 'Data' size (see also 'net_buf.c  NetBuf_GetMaxSize()  Note #1').
 *******************************************************************************************************/

//                                                                 --------------------- NET BUF ----------------------
struct  net_buf {
  NET_BUF_HDR Hdr;                                              // Net buf hdr.
  CPU_INT08U  *DataPtr;                                         // Ptr to net buf data area (see Note #1).
};

/********************************************************************************************************
 *                                   NETWORK BUFFER POOLS DATA TYPE
 *
 * Note(s) : (1) Each network interface & device configures & maintains its own network buffer pools.
 *******************************************************************************************************/

//                                                                 ------------------ NET BUF POOLS -------------------
typedef  struct  net_buf_pools {
  MEM_DYN_POOL  NetBufPool;                                     // Net buf                         pool.
  MEM_DYN_POOL  RxBufLargePool;                                 // Net buf rx large data area      pool.
  MEM_DYN_POOL  TxBufLargePool;                                 // Net buf tx large data area      pool.
  MEM_DYN_POOL  TxBufSmallPool;                                 // Net buf tx small data area      pool.

#if (NET_STAT_POOL_BUF_EN == DEF_ENABLED)
  NET_STAT_POOL NetBufStatPool;                                 // Net buf                    stat pool.
  NET_STAT_POOL RxBufLargeStatPool;                             // Net buf rx large data area stat pool.
  NET_STAT_POOL TxBufLargeStatPool;                             // Net buf tx large data area stat pool.
  NET_STAT_POOL TxBufSmallStatPool;                             // Net buf tx small data area stat pool.
#endif
} NET_BUF_POOLS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NET_BUF_GET_ID()
 *
 * @brief    Get next network buffer identification number.
 *
 * @param    id  Variable that will receive the returned identification number.
 *
 *               @note         (1) Network buffer identification number is returned in host-order.
 *******************************************************************************************************/

#define  NET_BUF_GET_ID(id)                     do { NET_UTIL_VAL_COPY_16(&(id), &NetBuf_ID_Ctr); \
                                                     NetBuf_ID_Ctr++;                             } while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetBuf_Init(MEM_SEG  *p_mem_seg,
                 RTOS_ERR *p_err);

//                                                                 ----------- BUF ALLOC FNCTS ------------
void NetBuf_PoolInit(NET_IF_NBR   if_nbr,
                     NET_BUF_TYPE type,
                     CPU_CHAR     *pool_name,
                     void         *p_mem_base_addr,
                     CPU_SIZE_T   mem_size,
                     CPU_SIZE_T   blk_nbr,
                     CPU_SIZE_T   blk_size,
                     CPU_SIZE_T   blk_align,
                     RTOS_ERR     *p_err);

void NetBuf_PoolCfgValidate(NET_IF_TYPE if_type,
                            NET_DEV_CFG *p_dev_cfg);

NET_BUF *NetBuf_Get(NET_IF_NBR      if_nbr,
                    NET_TRANSACTION transaction,
                    NET_BUF_SIZE    size,
                    NET_BUF_SIZE    ix,
                    NET_BUF_SIZE    *p_ix_offset,
                    NET_BUF_FLAGS   flags,
                    RTOS_ERR        *p_err);

CPU_INT08U *NetBuf_GetDataPtr(NET_IF          *p_if,
                              NET_TRANSACTION transaction,
                              NET_BUF_SIZE    size,
                              NET_BUF_SIZE    ix_start,
                              NET_BUF_SIZE    *p_ix_offset,
                              NET_BUF_SIZE    *p__data_size,
                              NET_BUF_TYPE    *p_type,
                              RTOS_ERR        *p_err);

NET_BUF_SIZE NetBuf_GetMaxSize(NET_IF_NBR      if_nbr,
                               NET_TRANSACTION transaction,
                               NET_BUF         *p_buf,
                               NET_BUF_SIZE    ix_start);

void NetBuf_Free(NET_BUF *p_buf);

NET_BUF_QTY NetBuf_FreeBuf(NET_BUF *p_buf,
                           NET_CTR *p_ctr);

NET_BUF_QTY NetBuf_FreeBufList(NET_BUF *p_buf_list,
                               NET_CTR *p_ctr);

NET_BUF_QTY NetBuf_FreeBufQ_PrimList(NET_BUF *p_buf_q,
                                     NET_CTR *p_ctr);

NET_BUF_QTY NetBuf_FreeBufQ_SecList(NET_BUF      *p_buf_q,
                                    NET_CTR      *p_ctr,
                                    NET_BUF_FNCT fnct_unlink);

void NetBuf_FreeBufDataAreaRx(NET_IF_NBR if_nbr,
                              CPU_INT08U *p_buf_data);

//                                                                 ------------ BUF API FNCTS -------------
void NetBuf_DataRd(NET_BUF      *p_buf,
                   NET_BUF_SIZE ix,
                   NET_BUF_SIZE len,
                   CPU_INT08U   *p_dest);

void NetBuf_DataWr(NET_BUF      *p_buf,
                   NET_BUF_SIZE ix,
                   NET_BUF_SIZE len,
                   CPU_INT08U   *p_src);

void NetBuf_DataCopy(NET_BUF      *p_buf_dest,
                     NET_BUF      *p_buf_src,
                     NET_BUF_SIZE ix_dest,
                     NET_BUF_SIZE ix_src,
                     NET_BUF_SIZE len,
                     RTOS_ERR     *p_err);

//                                                                 ----------- BUF STATUS FNCTS -----------
CPU_BOOLEAN NetBuf_IsUsed(NET_BUF *p_buf);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_BUF_PRIV_H_
