/***************************************************************************//**
 * @file
 * @brief Network Interface Layer - 802X
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

#ifdef  NET_IF_802x_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  NET_IPv4_MODULE_EN
#include  "net_ipv4_priv.h"
#include  "net_icmpv4_priv.h"
#include  "net_arp_priv.h"
#endif

#ifdef  NET_IPv6_MODULE_EN
#include  "net_ndp_priv.h"
#include  "net_ipv6_priv.h"
#endif

#include  "net_if_802x_priv.h"
#include  "net_if_priv.h"
#include  "net_priv.h"
#include  "net_ctr_priv.h"

#include  <net/include/net_util.h>

#include  <common/include/lib_utils.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 *                                               802x DEFINES
 *******************************************************************************************************/

//                                                                 ---------------- ETHER FRAME TYPES -----------------
#define  NET_IF_802x_FRAME_TYPE_IPv4                  0x0800u
#define  NET_IF_802x_FRAME_TYPE_IPv6                  0x86DDu
#define  NET_IF_802x_FRAME_TYPE_ARP                   0x0806u
#define  NET_IF_802x_FRAME_TYPE_RARP                  0x8035u   // See 'net_def.h  NETWORK PROTOCOL TYPES  Note #1'.

/********************************************************************************************************
 *                                           IEEE 802 DEFINES
 *
 * Note(s) : (1) SNAP 'Organizational Unique Identifier' (OUI) abbreviated to 'SNAP' for some SNAP OUI
 *               codes to enforce ANSI-compliance of 31-character symbol length uniqueness.
 *
 *           (2) Default SNAP 'Organizational Unique Identifier' (OUI) IEEE 802.2 frame type is ALWAYS
 *               Ethernet frame type (see 'IEEE 802 HEADER / FRAME  Note #1').
 *******************************************************************************************************/

#define  NET_IF_IEEE_802_FRAME_LEN_MAX                  (NET_IF_MTU_IEEE_802 + NET_IF_HDR_SIZE_BASE_IEEE_802)

//                                                                 ------- IEEE 802.2 LOGICAL LINK CONTROL (LLC) ------
#define  NET_IF_IEEE_802_LLC_DSAP                       0xAAu
#define  NET_IF_IEEE_802_LLC_SSAP                       0xAAu
#define  NET_IF_IEEE_802_LLC_CTRL                       0x03u

//                                                                 --- IEEE 802.2 SUB-NETWORK ACCESS PROTOCOL (SNAP) --
#define  NET_IF_IEEE_802_SNAP_CODE_ETHER            0x000000u   // Dflt  SNAP org code (Ether) [see Note #2].
#define  NET_IF_IEEE_802_SNAP_CODE_00                   0x00u   // Dflt  SNAP org code, octet #00.
#define  NET_IF_IEEE_802_SNAP_CODE_01                   0x00u   // Dflt  SNAP org code, octet #01.
#define  NET_IF_IEEE_802_SNAP_CODE_02                   0x00u   // Dflt  SNAP org code, octet #02.

#define  NET_IF_IEEE_802_SNAP_TYPE_IPv4                  NET_IF_802x_FRAME_TYPE_IPv4
#define  NET_IF_IEEE_802_SNAP_TYPE_IPv6                  NET_IF_802x_FRAME_TYPE_IPv6
#define  NET_IF_IEEE_802_SNAP_TYPE_ARP                   NET_IF_802x_FRAME_TYPE_ARP
#define  NET_IF_IEEE_802_SNAP_TYPE_RARP                  NET_IF_802x_FRAME_TYPE_RARP

/********************************************************************************************************
 *                                   NETWORK INTERFACE HEADER DEFINES
 *
 * Note(s) : (1) NET_IF_HDR_SIZE_ETHER_MAX's ideal #define'tion :
 *
 *                   (A) max( Ether Hdr, IEEE 802 Hdr )
 *
 *               (a) However, since NET_IF_HDR_SIZE_ETHER_MAX is used ONLY for network transmit & IEEE 802
 *                   is NEVER transmitted (see 'net_if_ether.h  Note #2'), NET_IF_HDR_SIZE_ETHER_MAX  MUST
 *                   be #define'd with hard-coded knowledge that Ethernet is the only supported frame
 *                   encapsulation for network transmit.
 *
 *           (2) The following network interface value MUST be pre-#define'd in 'net_def.h' PRIOR to
 *               'net_cfg.h' so that the developer can configure the network interface for the correct
 *               network interface link layer values (see 'net_def.h  NETWORK INTERFACE LAYER DEFINES'
 *               & 'net_cfg_net.h  NETWORK INTERFACE LAYER CONFIGURATION  Note #4') :
 *
 *               (a) NET_IF_HDR_SIZE_ETHER                 14
 *******************************************************************************************************/

#define  NET_IF_HDR_SIZE_BASE_ETHER                       14    // Ethernet base hdr size.
#define  NET_IF_HDR_SIZE_BASE_IEEE_802                     8    // IEEE 802 base hdr size.

#if 0                                                           // See Note #2a.
#define  NET_IF_HDR_SIZE_ETHER                           NET_IF_HDR_SIZE_BASE_ETHER
#endif

#define  NET_IF_HDR_SIZE_IEEE_802                       (NET_IF_HDR_SIZE_BASE_ETHER + NET_IF_HDR_SIZE_BASE_IEEE_802)

#define  NET_IF_HDR_SIZE_ETHER_MIN              (DEF_MIN(NET_IF_HDR_SIZE_ETHER, NET_IF_HDR_SIZE_IEEE_802))
#define  NET_IF_HDR_SIZE_ETHER_MAX                       NET_IF_HDR_SIZE_ETHER

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ 802x ADDRs ------------
static const CPU_INT08U NetIF_802x_AddrNull[NET_IF_802x_ADDR_SIZE] = {
  0x00u,
  0x00u,
  0x00u,
  0x00u,
  0x00u,
  0x00u
};

static const CPU_INT08U NetIF_802x_AddrBroadcast[NET_IF_802x_ADDR_SIZE] = {
  0xFFu,
  0xFFu,
  0xFFu,
  0xFFu,
  0xFFu,
  0xFFu
};

#ifdef  NET_MCAST_RX_MODULE_EN
static const CPU_INT08U NetIF_802x_AddrMulticastMask[NET_IF_802x_ADDR_SIZE] = {
  0x01u,
  0x00u,
  0x00u,
  0x00u,
  0x00u,
  0x00u
};
#endif

#ifdef  NET_MCAST_TX_MODULE_EN
#ifdef  NET_IPv4_MODULE_EN
static const CPU_INT08U NetIF_802x_AddrMulticastBaseIPv4[NET_IF_802x_ADDR_SIZE] = {
  0x01u,
  0x00u,
  0x5Eu,
  0x00u,
  0x00u,
  0x00u
};
#endif
#endif

#ifdef  NET_MCAST_MODULE_EN
#ifdef  NET_IPv6_MODULE_EN
static const CPU_INT08U NetIF_802x_AddrMulticastBaseIPv6[NET_IF_802x_ADDR_SIZE] = {
  0x33u,
  0x33u,
  0x00u,
  0x00u,
  0x00u,
  0x00u
};
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               NETWORK INTERFACE HEADER / FRAME DATA TYPES
 *******************************************************************************************************/

//                                                                 ----------------- NET 802x IF DATA -----------------
typedef  struct  net_if_data_802x {
  CPU_INT08U HW_Addr[NET_IF_802x_ADDR_SIZE];                    // 802x IF's dev hw addr.
} NET_IF_DATA_802x;

/********************************************************************************************************
 *                                   ETHERNET HEADER / FRAME DATA TYPES
 *
 * Note(s) : (1) Frame 'Data' buffer CANNOT be declared to force word-alignment.  'Data' buffer MUST immediately
 *               follow frame 'Hdr' since Ethernet frames are contiguous, non-aligned data packets.
 *
 *           (2) 'Data' declared with 1 entry; prevents removal by compiler optimization.
 *
 *           (3) Frame CRC's are computed/validated by an Ethernet device.  NO software CRC is handled for
 *               receive or transmit.
 *******************************************************************************************************/

//                                                                 ----------------- NET IF ETHER HDR -----------------
typedef  struct  net_if_hdr_ether {
  CPU_INT08U AddrDest[NET_IF_802x_ADDR_SIZE];                   // MAC dest addr.
  CPU_INT08U AddrSrc[NET_IF_802x_ADDR_SIZE];                    // MAC src  addr.
  CPU_INT16U FrameType;                                         // Frame type.
} NET_IF_HDR_ETHER;

/********************************************************************************************************
 *                                   IEEE 802 HEADER / FRAME DATA TYPES
 *
 * Note(s) : (1) Header 'SNAP_OrgCode' defines the SNAP 'Organizational Unique Identifier' (OUI).  The OUI
 *               indicates the various organization/vendor/manufacturer with each organization then defining
 *               their respective frame types.
 *
 *               However, the default SNAP OUI indicates Ethernet frame types & is ALWAYS used.  ALL other
 *               OUI's are discarded as invalid.
 *
 *               See also 'IEEE 802 DEFINES  Notes #1 & #2'.
 *
 *           (2) Frame 'Data' buffer CANNOT be declared to force word-alignment.  'Data' buffer MUST immediately
 *               follow frame 'Hdr' since Ethernet frames are contiguous, non-aligned data packets.
 *
 *           (3) 'Data' declared with 1 entry; prevents removal by compiler optimization.
 *
 *           (4) Frame CRC's are computed/validated by an Ethernet device.  NO software CRC is handled for
 *               receive or transmit.
 *******************************************************************************************************/

//                                                                 --------------- NET IF IEEE 802 HDR ----------------
typedef  struct  net_if_hdr_ieee_802 {
  CPU_INT08U AddrDest[NET_IF_802x_ADDR_SIZE];                   // IEEE 802.3 dest  addr.
  CPU_INT08U AddrSrc[NET_IF_802x_ADDR_SIZE];                    // IEEE 802.3 src   addr.
  CPU_INT16U FrameLen;                                          // IEEE 802.3 frame len.

  //                                                               ------ IEEE 802.2 LOGICAL LINK CONTROL (LLC) -------
  CPU_INT08U LLC_DSAP;                                          // Dest Serv Access Pt.
  CPU_INT08U LLC_SSAP;                                          // Src  Serv Access Pt.
  CPU_INT08U LLC_Ctrl;                                          // Ctrl Field.

  //                                                               -- IEEE 802.2 SUB-NETWORK ACCESS PROTOCOL (SNAP) ---
  CPU_INT08U SNAP_OrgCode[NET_IF_IEEE_802_SNAP_CODE_SIZE];              // Org code (see Note #1).
  CPU_INT16U SNAP_FrameType;                                            // IEEE 802.2 frame type.
} NET_IF_HDR_IEEE_802;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------- RX FNCTS -----------
static void NetIF_802x_RxPktFrameDemux(NET_IF                *p_if,
                                       NET_BUF               *p_buf,
                                       NET_BUF_HDR           *p_buf_hdr,
                                       NET_IF_HDR_802x       *p_if_hdr,
                                       NET_CTR_IF_802x_STATS *p_ctrs_stat,
                                       NET_CTR_IF_802x_ERRS  *p_ctrs_err,
                                       RTOS_ERR              *p_err);

static void NetIF_802x_RxPktFrameDemuxEther(NET_IF               *p_if,
                                            NET_BUF_HDR          *p_buf_hdr,
                                            NET_IF_HDR_802x      *p_if_hdr,
                                            NET_CTR_IF_802x_ERRS *p_ctrs_err,
                                            RTOS_ERR             *p_err);

static void NetIF_802x_RxPktFrameDemuxIEEE802(NET_IF               *p_if,
                                              NET_BUF_HDR          *p_buf_hdr,
                                              NET_IF_HDR_802x      *p_if_hdr,
                                              NET_CTR_IF_802x_ERRS *p_ctrs_err,
                                              RTOS_ERR             *p_err);

//                                                                 ----------- TX FNCTS -----------
static NET_IF_TX_RDY_STATUS NetIF_802x_TxPktPrepareFrame(NET_IF                *p_if,
                                                         NET_BUF               *p_buf,
                                                         NET_BUF_HDR           *p_buf_hdr,
                                                         NET_CTR_IF_802x_STATS *p_ctrs_stat,
                                                         NET_CTR_IF_802x_ERRS  *p_ctrs_err,
                                                         RTOS_ERR              *p_err);

static void NetIF_802x_TxIxDataGet(CPU_INT16U *p_ix);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetIF_802x_Init()
 *
 * @brief    (1) Initialize 802x Module :
 *
 *               Module initialization NOT yet required/implemented
 *
 *
 * Argument(s) : p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
void NetIF_802x_Init(RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);
}

/****************************************************************************************************//**
 *                                               NetIF_802x_Rx()
 *
 * @brief    (1) Process received data packets & forward to network protocol layers :
 *               - (a) Update link status
 *               - (b) Validate packet received
 *               - (c) Demultiplex packet to higher-layer protocols
 *
 * @param    p_if            Pointer to an 802x network interface.
 *
 * @param    p_buf           Pointer to a network buffer that received a packet.
 *
 * @param    p_ctrs_stat     Pointer to an 802x network interface statistic counters.
 *
 * @param    p_ctrs_err      Pointer to an 802x network interface error     counters.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) If a network interface receives a packet, its physical link must be 'UP' & the
 *               interface's physical link state is set accordingly.
 *               - (a) An attempt to check for link state is made after an interface has been started.
 *                     However, many physical layer devices, such as Ethernet physical layers require
 *                     several seconds for Auto-Negotiation to complete before the link becomes
 *                     established.  Thus the interface link flag is not updated until the link state
 *                     timer expires & one or more attempts to check for link state have been completed.
 *
 * @note     (3) Network buffer already freed by higher layer; only increment error counter.
 *******************************************************************************************************/
void NetIF_802x_Rx(NET_IF                *p_if,
                   NET_BUF               *p_buf,
                   NET_CTR_IF_802x_STATS *p_ctrs_stat,
                   NET_CTR_IF_802x_ERRS  *p_ctrs_err,
                   RTOS_ERR              *p_err)
{
  NET_BUF_HDR     *p_buf_hdr;
  NET_IF_HDR_802x *p_if_hdr;
  CPU_BOOLEAN     size_valid;

  //                                                               ---------------- UPDATE LINK STATUS ----------------
  if (p_if->En != DEF_YES) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxIFDisableCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit_discard;
  }

  p_if->Link = NET_IF_LINK_UP;                                  // See Note #2.

  //                                                               ------------ VALIDATE RX'D 802x IF PKT -------------
  p_buf_hdr = &p_buf->Hdr;
  size_valid = NetIF_802x_PktSizeIsValid(p_buf_hdr->TotLen);
  if (size_valid != DEF_YES) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvFrameCtr);
    NET_CTR_ERR_INC(p_ctrs_err->RxInvFrameCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit_discard;
  }

  NET_CTR_STAT_INC(Net_StatCtrs.IFs.IFs_802xCtrs.RxPktCtr);
  NET_CTR_STAT_INC(p_ctrs_stat->RxPktCtr);

  //                                                               -------------- DEMUX RX'D 802x IF PKT --------------
  p_if_hdr = (NET_IF_HDR_802x *)&p_buf->DataPtr[p_buf_hdr->IF_HdrIx];
  NetIF_802x_RxPktFrameDemux(p_if,                              // Demux pkt to appropriate net protocol.
                             p_buf,
                             p_buf_hdr,
                             p_if_hdr,
                             p_ctrs_stat,
                             p_ctrs_err,
                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(p_ctrs_err->RxPktDisCtr);
  NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxPktDisCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetIF_802x_Tx()
 *
 * @brief    (1) Prepare data packets from network protocol layers for transmit :
 *               - (a) Validate packet to transmit
 *               - (b) Prepare data packets with appropriate Ethernet frame format
 *
 * @param    p_if            Pointer to an 802x network interface.
 *
 * @param    p_buf           Pointer to a network buffer with data packet to transmit.
 *
 * @param    p_ctrs_stat     Pointer to an 802x network interface statistic counters.
 *
 * @param    p_ctrs_err      Pointer to an 802x network interface error     counters.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetIF_802x_Tx(NET_IF                *p_if,
                   NET_BUF               *p_buf,
                   NET_CTR_IF_802x_STATS *p_ctrs_stat,
                   NET_CTR_IF_802x_ERRS  *p_ctrs_err,
                   RTOS_ERR              *p_err)
{
  NET_IF_HDR_ETHER *p_if_hdr_ether;
  NET_BUF_HDR      *p_buf_hdr;
#ifdef NET_CACHE_MODULE_EN
  NET_CACHE_STATE cache_state = NET_CACHE_STATE_NONE;
#endif
  NET_IF_TX_RDY_STATUS tx_status;
  CPU_INT16U           frame_type;

  p_buf_hdr = &p_buf->Hdr;

  //                                                               -------------- PREPARE 802x TX FRAME ---------------
  tx_status = NetIF_802x_TxPktPrepareFrame(p_if,
                                           p_buf,
                                           p_buf_hdr,
                                           p_ctrs_stat,
                                           p_ctrs_err,
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  switch (tx_status) {
    case NET_IF_TX_RDY_STATUS_UNICAST:
    case NET_IF_TX_RDY_STATUS_BROADCAST:
      NET_CTR_STAT_INC(Net_StatCtrs.IFs.IFs_802xCtrs.TxPktCtr);
      NET_CTR_STAT_INC(p_ctrs_stat->TxPktCtr);
      goto exit;

    case NET_IF_TX_RDY_STATUS_MULTICAST:
    case NET_IF_TX_RDY_STATUS_ADDR_REQ:
      p_if_hdr_ether = (NET_IF_HDR_ETHER *)&p_buf->DataPtr[p_buf_hdr->IF_HdrIx];
      frame_type = NET_UTIL_VAL_GET_NET_16(&p_if_hdr_ether->FrameType);
      switch (frame_type) {
#ifdef  NET_IPv4_MODULE_EN
        case NET_IF_802x_FRAME_TYPE_ARP:
        case NET_IF_802x_FRAME_TYPE_IPv4:
#ifdef  NET_ARP_MODULE_EN
          cache_state = NetARP_CacheHandler(p_buf, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit_discard;
          }
#else
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
          break;
#endif

#ifdef  NET_IPv6_MODULE_EN
        case NET_IF_802x_FRAME_TYPE_IPv6:
#ifdef  NET_NDP_MODULE_EN
          cache_state = NetNDP_NeighborCacheHandler(p_buf, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit_discard;
          }
#else
          RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
          break;
#endif

        default:
          RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
      break;

    default:
      goto exit_discard;
  }
#ifdef NET_CACHE_MODULE_EN
  switch (cache_state) {
    case NET_CACHE_STATE_RESOLVED:
      NET_CTR_STAT_INC(Net_StatCtrs.IFs.IFs_802xCtrs.TxPktCtr);
      NET_CTR_STAT_INC(p_ctrs_stat->TxPktCtr);
      break;

    case NET_CACHE_STATE_PEND:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_OP_IN_PROGRESS);
      goto exit;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_STATE,; );
  }
#endif
  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.TxPktDisCtr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIF_802x_AddrHW_Get()
 *
 * @brief    Get an 802x interface's hardware address.
 *
 * @param    p_if        Pointer to an 802x network interface.
 *
 * @param    p_addr_hw   Pointer to variable that will receive the hardware address (see Note #1).
 *
 * @param    p_addr_len  Pointer to a variable to ... :
 *                           - (a) Pass the length of the address buffer pointed to by 'paddr_hw'.
 *                           - (b) (1) Return the actual size of the protocol address, if NO error(s);
 *                               - (2) Return 0,                                       otherwise.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) The hardware address is returned in network-order; i.e. the pointer to the hardware
 *               address points to the highest-order octet.
 *
 * @note     (2) Pointers to variables that return values MUST be initialized PRIOR to all other
 *               validation or function handling in case of any error(s).
 *******************************************************************************************************/
void NetIF_802x_AddrHW_Get(NET_IF     *p_if,
                           CPU_INT08U *p_addr_hw,
                           CPU_INT08U *p_addr_len,
                           RTOS_ERR   *p_err)
{
  NET_IF_DATA_802x *p_if_data;
  CPU_INT08U       addr_len;

  addr_len = *p_addr_len;
  *p_addr_len = 0u;                                             // Init len for err (see Note #2).

  //                                                               ------------ VALIDATE 802x HW ADDR LEN -------------
  if (addr_len < NET_IF_802x_ADDR_SIZE) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  //                                                               ----------------- GET 802x HW ADDR -----------------
  p_if_data = (NET_IF_DATA_802x *)p_if->IF_Data;
  NET_UTIL_VAL_COPY(p_addr_hw,
                    &p_if_data->HW_Addr[0],
                    NET_IF_802x_ADDR_SIZE);

  *p_addr_len = NET_IF_802x_ADDR_SIZE;
}

/****************************************************************************************************//**
 *                                           NetIF_802x_AddrHW_Set()
 *
 * @brief    Set an 802x interface's hardware address.
 *
 * @param    p_if        Pointer to an 802x network interface.
 *
 * @param    p_addr_hw   Pointer to hardware address (see Note #1).
 *
 * @param    addr_len    Length  of hardware address.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) The hardware address MUST be in network-order; i.e. the pointer to the hardware
 *               address MUST point to the highest-order octet.
 *
 * @note     (2) The interface MUST be stopped BEFORE setting a new hardware address which does NOT
 *               take effect until the interface is re-started.
 *******************************************************************************************************/
void NetIF_802x_AddrHW_Set(NET_IF     *p_if,
                           CPU_INT08U *p_addr_hw,
                           CPU_INT08U addr_len,
                           RTOS_ERR   *p_err)
{
  NET_IF_DATA_802x *p_if_data;
  CPU_BOOLEAN      valid;

  //                                                               -------------- VALIDATE 802x HW ADDR ---------------
  RTOS_ASSERT_DBG_ERR_SET((p_if != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_addr_hw != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((addr_len == NET_IF_802x_ADDR_SIZE), *p_err, RTOS_ERR_INVALID_ARG,; );

  valid = NetIF_802x_AddrHW_IsValid(p_if, p_addr_hw);
  if (valid != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  //                                                               ----------------- SET 802x HW ADDR -----------------
  p_if_data = (NET_IF_DATA_802x *)p_if->IF_Data;
  NET_UTIL_VAL_COPY(&p_if_data->HW_Addr[0],                     // Set new hw addr (see Note #2).
                    p_addr_hw,
                    NET_IF_802x_ADDR_SIZE);
}

/****************************************************************************************************//**
 *                                       NetIF_802x_AddrHW_IsValid()
 *
 * @brief    (1) Validate an 802x hardware address which MUST NOT be one of the following :
 *           - (a) 802x broadcast address               See RFC #894, Section 'Address Mappings :
 *                                                       Broadcast Address'
 *
 * @param    p_if        Pointer to an 802x network interface.
 *
 * @param    p_addr_hw   Pointer to an 802x hardware address (see Note #1).
 *
 * @return   DEF_YES, if 802x hardware address valid.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) The hardware address MUST be in network-order; i.e. the pointer to the hardware
 *               address MUST point to the highest-order octet.
 *
 * @note     (2) The size of the memory buffer that contains the 802x hardware address MUST be
 *               greater than or equal to NET_IF_802x_ADDR_SIZE.
 *
 * @note     (3) 802x hardware address memory buffer array accessed by octets.
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_802x_AddrHW_IsValid(NET_IF     *p_if,
                                      CPU_INT08U *p_addr_hw)
{
  CPU_BOOLEAN addr_null;
  CPU_BOOLEAN addr_broadcast;
  CPU_BOOLEAN addr_valid;

  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warning.

  //                                                               ------------------- VALIDATE PTR -------------------
  if (p_addr_hw == DEF_NULL) {
    switch (p_if->Type) {
#ifdef  NET_IF_ETHER_MODULE_EN
      case NET_IF_TYPE_ETHER:
        NET_CTR_ERR_INC(Net_ErrCtrs.IFs.Ether.IF_802xCtrs.NullPtrCtr);
        break;
#endif

#ifdef  NET_IF_WIFI_MODULE_EN
      case NET_IF_TYPE_WIFI:
        NET_CTR_ERR_INC(Net_ErrCtrs.IFs.WiFi.IF_802xCtrs.NullPtrCtr);
        break;
#endif

      default:
        break;
    }

    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.NullPtrCtr);
    return (DEF_NO);
  }

  //                                                               -------------- VALIDATE 802x HW ADDR ---------------
  addr_null = Mem_Cmp(p_addr_hw,
                      &NetIF_802x_AddrNull[0],
                      NET_IF_802x_ADDR_SIZE);

  addr_broadcast = Mem_Cmp(p_addr_hw,
                           &NetIF_802x_AddrBroadcast[0],
                           NET_IF_802x_ADDR_SIZE);

  addr_valid = ((addr_null == DEF_YES)
                || (addr_broadcast == DEF_YES)) ? DEF_NO : DEF_YES;

  return (addr_valid);
}

/****************************************************************************************************//**
 *                                       NetIF_802x_AddrMulticastAdd()
 *
 * @brief    Add a multicast address to an 802x interface.
 *
 * @param    p_if                Pointer to an 802x network interface to add address.
 *
 * @param    p_addr_protocol     Pointer to a multicast protocol address to add (see Note #1).
 *
 * @param    addr_protocol_len   Length of the protocol address, in octets.
 *
 * @param    addr_protocol_type  Protocol address type.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) The multicast protocol address MUST be in network-order.
 *******************************************************************************************************/
void NetIF_802x_AddrMulticastAdd(NET_IF            *p_if,
                                 CPU_INT08U        *p_addr_protocol,
                                 CPU_INT08U        addr_protocol_len,
                                 NET_PROTOCOL_TYPE addr_protocol_type,
                                 RTOS_ERR          *p_err)
{
#ifdef  NET_MCAST_MODULE_EN
  NET_DEV_API *p_dev_api;
  CPU_INT08U  addr_hw[NET_IF_802x_ADDR_SIZE];
  CPU_INT08U  addr_hw_len;

  switch (addr_protocol_type) {
    case NET_PROTOCOL_TYPE_IP_V4:
      addr_hw_len = sizeof(addr_hw);
      NetIF_802x_AddrMulticastProtocolToHW(p_if,
                                           p_addr_protocol,
                                           addr_protocol_len,
                                           addr_protocol_type,
                                           &addr_hw[0],
                                           &addr_hw_len,
                                           p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

    case NET_PROTOCOL_TYPE_IP_V6:
      addr_hw_len = sizeof(addr_hw);
      NetIF_802x_AddrMulticastProtocolToHW(p_if,
                                           p_addr_protocol,
                                           addr_protocol_len,
                                           addr_protocol_type,
                                           &addr_hw[0],
                                           &addr_hw_len,
                                           p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  p_dev_api = (NET_DEV_API *)p_if->Dev_API;

  //                                                               Add multicast addr to dev.
  p_dev_api->AddrMulticastAdd(p_if,
                              &addr_hw[0],
                              addr_hw_len,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
#else
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(p_addr_protocol);
  PP_UNUSED_PARAM(addr_protocol_len);
  PP_UNUSED_PARAM(addr_protocol_type);

  RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
}

/****************************************************************************************************//**
 *                                       NetIF_802x_AddrMulticastRemove()
 *
 * @brief    Remove a multicast address from an 802x interface.
 *
 * @param    p_if                Pointer to an 802x network interface to remove address.
 *
 * @param    p_addr_protocol     Pointer to a multicast protocol address to remove (see Note #1).
 *
 * @param    addr_protocol_len   Length of the protocol address, in octets.
 *
 * @param    addr_protocol_type  Protocol address type.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) The multicast protocol address MUST be in network-order.
 *******************************************************************************************************/
void NetIF_802x_AddrMulticastRemove(NET_IF            *p_if,
                                    CPU_INT08U        *p_addr_protocol,
                                    CPU_INT08U        addr_protocol_len,
                                    NET_PROTOCOL_TYPE addr_protocol_type,
                                    RTOS_ERR          *p_err)
{
#ifdef  NET_MCAST_MODULE_EN
  NET_DEV_API *p_dev_api;
  CPU_INT08U  addr_hw[NET_IF_802x_ADDR_SIZE];
  CPU_INT08U  addr_hw_len;

  switch (addr_protocol_type) {
    case NET_PROTOCOL_TYPE_IP_V4:
    case NET_PROTOCOL_TYPE_IP_V6:
      addr_hw_len = sizeof(addr_hw);
      NetIF_802x_AddrMulticastProtocolToHW(p_if,
                                           p_addr_protocol,
                                           addr_protocol_len,
                                           addr_protocol_type,
                                           &addr_hw[0],
                                           &addr_hw_len,
                                           p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }

  p_dev_api = (NET_DEV_API *)p_if->Dev_API;
  //                                                               Remove multicast addr from dev.
  p_dev_api->AddrMulticastRemove(p_if,
                                 &addr_hw[0],
                                 addr_hw_len,
                                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;

#else
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(p_addr_protocol);
  PP_UNUSED_PARAM(addr_protocol_len);
  PP_UNUSED_PARAM(addr_protocol_type);

  RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
}

/****************************************************************************************************//**
 *                                   NetIF_802x_AddrMulticastProtocolToHW()
 *
 * @brief    Convert a multicast protocol address into an 802x address.
 *
 * @param    p_if                Pointer to an 802x network interface to transmit the packet.
 *
 * @param    p_addr_protocol     Pointer to a multicast protocol address to convert
 *
 * @param    ---------------     (see Note #1a).
 *
 * @param    addr_protocol_len   Length of the protocol address, in octets.
 *
 * @param    addr_protocol_type  Protocol address type.
 *
 * @param    p_addr_hw           Pointer to a variable that will receive the hardware address
 *
 * @param    ---------           (see Note #1a).
 *
 * @param    p_addr_hw_len       Pointer to a variable to ... :
 *
 * @param    -------------       (see Note #1a).
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Octet order:
 *               - (a) The multicast protocol address MUST be in network-order.
 *               - (b) The 802x hardware address is returned in network-order; i.e. the pointer to
 *                     the hardware address points to the highest-order octet.
 *
 * @note     (2) Since 'paddr_hw_len' argument is both an input & output argument (see 'Argument(s) :
 *               paddr_hw_len'), ... :
 *               - (a) Its input value SHOULD be validated prior to use, and
 *               - (b) Its output value MUST be initially configured to return a default value
 *                     PRIOR to all other validation or function handling in case of any error(s).
 *******************************************************************************************************/
void NetIF_802x_AddrMulticastProtocolToHW(NET_IF            *p_if,
                                          CPU_INT08U        *p_addr_protocol,
                                          CPU_INT08U        addr_protocol_len,
                                          NET_PROTOCOL_TYPE addr_protocol_type,
                                          CPU_INT08U        *p_addr_hw,
                                          CPU_INT08U        *p_addr_hw_len,
                                          RTOS_ERR          *p_err)
{
#ifdef  NET_MCAST_TX_MODULE_EN
#ifdef NET_IPv4_MODULE_EN
  CPU_INT08U addr_protocol_v4[NET_IPv4_ADDR_LEN];
#endif
#ifdef NET_IPv6_MODULE_EN
  NET_IPv6_ADDR addr_protocol_v6;
#endif
#endif

  PP_UNUSED_PARAM(p_err);

  *p_addr_hw_len = 0u;                                          // Cfg dflt addr len for err (see Note #2b).

#ifdef  NET_MCAST_TX_MODULE_EN
  switch (addr_protocol_type) {
#ifdef NET_IPv4_MODULE_EN
    case NET_PROTOCOL_TYPE_IP_V4:
      NET_UTIL_VAL_COPY(&addr_protocol_v4[0],
                        p_addr_protocol,
                        addr_protocol_len);

      NET_UTIL_VAL_COPY(&p_addr_hw[0],
                        &NetIF_802x_AddrMulticastBaseIPv4[0],
                        NET_IF_802x_ADDR_SIZE);
      p_addr_hw[3] = addr_protocol_v4[1] & 0x7Fu;
      p_addr_hw[4] = addr_protocol_v4[2];
      p_addr_hw[5] = addr_protocol_v4[3];
      break;
#endif

#ifdef NET_IPv6_MODULE_EN
    case NET_PROTOCOL_TYPE_IP_V6:
      Mem_Copy(&addr_protocol_v6.Addr[0],
               p_addr_protocol,
               addr_protocol_len);

      Mem_Copy(&p_addr_hw[0],
               &NetIF_802x_AddrMulticastBaseIPv6[0],
               NET_IF_802x_ADDR_SIZE);
      p_addr_hw[2] = addr_protocol_v6.Addr[12];
      p_addr_hw[3] = addr_protocol_v6.Addr[13];
      p_addr_hw[4] = addr_protocol_v6.Addr[14];
      p_addr_hw[5] = addr_protocol_v6.Addr[15];
      break;
#endif

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
  }

  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warning.

  *p_addr_hw_len = NET_IF_802x_ADDR_SIZE;

#else
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(p_addr_protocol);
  PP_UNUSED_PARAM(addr_protocol_len);
  PP_UNUSED_PARAM(addr_protocol_type);
  PP_UNUSED_PARAM(p_addr_hw);

  *p_addr_hw_len = 0u;                                          // Cfg dflt addr len for err (see Note #2b).

  RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
}

/****************************************************************************************************//**
 *                                       NetIF_802x_BufPoolCfgValidate()
 *
 * @brief    (1) Validate 802x network buffer pool configuration :
 *               - (a) Validate configured size of network buffers
 *
 * @param    p_if    Pointer to an 802x network interface.
 *
 *
 * Argument(s) : p_if        Pointer to an 802x network interface.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) All 802x buffer data area sizes MUST be configured greater than or equal to
 *                   NET_IF_802x_BUF_SIZE_MIN.
 *******************************************************************************************************/
void NetIF_802x_BufPoolCfgValidate(NET_IF   *p_if,
                                   RTOS_ERR *p_err)
{
  NET_DEV_CFG *p_dev_cfg;
  CPU_INT16U  net_if_802x_buf_size_min;

  p_dev_cfg = (NET_DEV_CFG *)p_if->Dev_Cfg;

  net_if_802x_buf_size_min = 0u;

  NetIF_802x_TxIxDataGet(&net_if_802x_buf_size_min);

  //                                                               ----------- VALIDATE BUF DATA SIZES ------------
  //                                                               Validate large rx buf size (see Note #2).
  RTOS_ASSERT_DBG_ERR_SET((p_dev_cfg->RxBufLargeSize >= net_if_802x_buf_size_min), *p_err, RTOS_ERR_INVALID_CFG,; );

  if (p_dev_cfg->TxBufLargeNbr > 0) {                               // If any large tx bufs cfg'd, ...
                                                                    // ... validate large tx buf size (see Note #2).
    RTOS_ASSERT_DBG_ERR_SET((p_dev_cfg->TxBufLargeSize >= net_if_802x_buf_size_min), *p_err, RTOS_ERR_INVALID_CFG,; );
  }

  if (p_dev_cfg->TxBufSmallNbr > 0) {                               // If any small tx bufs cfg'd, ...
                                                                    // ... validate small tx buf size (see Note #2).
    RTOS_ASSERT_DBG_ERR_SET((p_dev_cfg->TxBufSmallSize >= net_if_802x_buf_size_min), *p_err, RTOS_ERR_INVALID_CFG,; );
  }
}

/****************************************************************************************************//**
 *                                           NetIF_802x_MTU_Set()
 *
 * @brief    Set 802x interface's MTU.
 *
 * @param    p_if    Pointer to an 802x network interface.
 *
 * @param    mtu     Desired maximum transmission unit (MTU) size to configure (in octets).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetIF_802x_MTU_Set(NET_IF   *p_if,
                        NET_MTU  mtu,
                        RTOS_ERR *p_err)
{
  NET_DEV_CFG  *p_dev_cfg;
  NET_BUF_SIZE buf_size_max;
  NET_MTU      mtu_max;

  p_dev_cfg = (NET_DEV_CFG *)p_if->Dev_Cfg;

  buf_size_max = DEF_MAX((p_dev_cfg->TxBufLargeSize - p_dev_cfg->TxBufIxOffset),
                         (p_dev_cfg->TxBufSmallSize - p_dev_cfg->TxBufIxOffset));

  mtu_max = DEF_MIN(mtu, buf_size_max);

  RTOS_ASSERT_DBG_ERR_SET((mtu <= mtu_max), *p_err, RTOS_ERR_INVALID_CFG,; );

  p_if->MTU = mtu;
}

/****************************************************************************************************//**
 *                                       NetIF_802x_GetPktSizeHdr()
 *
 * @brief    Get 802x packet header size.
 *
 * @param    p_if    Pointer to an 802x network interface.
 *
 * @return   The 802x packet header size.
 *******************************************************************************************************/
CPU_INT16U NetIF_802x_GetPktSizeHdr(NET_IF *p_if)
{
  CPU_INT16U pkt_size;

  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warning.
  pkt_size = (CPU_INT16U)NET_IF_HDR_SIZE_ETHER;

  return (pkt_size);
}

/****************************************************************************************************//**
 *                                       NetIF_802x_GetPktSizeMin()
 *
 * @brief    Get minimum allowable 802x packet size.
 *
 * @param    p_if    Pointer to an 802x network interface.
 *
 * @return   The minimum allowable 802x packet size.
 *******************************************************************************************************/
CPU_INT16U NetIF_802x_GetPktSizeMin(NET_IF *p_if)
{
  CPU_INT16U pkt_size;

  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warning.
  pkt_size = (CPU_INT16U)NET_IF_802x_FRAME_MIN_SIZE;

  return (pkt_size);
}

/****************************************************************************************************//**
 *                                       NetIF_802x_GetPktSizeMax()
 *
 * @brief    Get maximum allowable 802x packet size.
 *
 * @param    p_if    Pointer to an 802x network interface.
 *
 * @return   The maximum allowable 802x packet size.
 *******************************************************************************************************/
CPU_INT16U NetIF_802x_GetPktSizeMax(NET_IF *p_if)
{
  CPU_INT16U pkt_size;

  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warning.
  pkt_size = (CPU_INT16U)NET_IF_ETHER_FRAME_MAX_SIZE;

  return (pkt_size);
}

/****************************************************************************************************//**
 *                                       NetIF_802x_PktSizeIsValid()
 *
 * @brief    Validate an 802x packet size.
 *
 * @param    size    Size of 802x packet frame (in octets).
 *
 * @return   DEF_YES, if 802x packet size valid.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetIF_802x_PktSizeIsValid(CPU_INT16U size)
{
  CPU_BOOLEAN valid;

  valid = DEF_YES;

  if (size < NET_IF_802x_FRAME_MIN_SIZE) {
    valid = DEF_NO;
  }

  return (valid);
}

/****************************************************************************************************//**
 *                                           NetIF_802x_ISR_Handler()
 *
 * @brief    Handle Wireless device's interrupt service routine (ISR) function(s).
 *
 * @param    p_if    Pointer to an 802x network interface.
 *
 * @param    type    Device interrupt type(s) to handle :
 *                       - NET_DEV_ISR_TYPE_UNKNOWN                Handle unknown device           ISR(s).
 *                       - NET_DEV_ISR_TYPE_RX                     Handle device receive           ISR(s).
 *                       - NET_DEV_ISR_TYPE_RX_RUNT                Handle device runt              ISR(s).
 *                       - NET_DEV_ISR_TYPE_RX_OVERRUN             Handle device receive overrun   ISR(s).
 *                       - NET_DEV_ISR_TYPE_TX_RDY                 Handle device transmit ready    ISR(s).
 *                       - NET_DEV_ISR_TYPE_TX_COMPLETE            Handle device transmit complete ISR(s).
 *                       - NET_DEV_ISR_TYPE_TX_COLLISION_LATE      Handle device late   collision  ISR(s).
 *                       - NET_DEV_ISR_TYPE_TX_COLLISION_EXCESS    Handle device excess collision  ISR(s).
 *                       - NET_DEV_ISR_TYPE_JABBER                 Handle device jabber            ISR(s).
 *                       - NET_DEV_ISR_TYPE_BABBLE                 Handle device late babble       ISR(s).
 *                       - NET_DEV_ISR_TYPE_PHY                    Handle device physical layer    ISR(s).
 *                   See specific network device(s) for
 *                   additional device ISR(s).
 *                   See also 'net_if.h  NETWORK DEVICE INTERRUPT SERVICE ROUTINE (ISR) TYPE DEFINES'
 *                   for other available & supported network device ISR types.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) NetIF_WiFi_ISR_Handler() is called within the context of an ISR & ... :
 *               - (a) MUST NOT be called with the global network lock already acquired; ...
 *               - (b) MUST NOT block by pending on & acquiring the global network lock.
 *                   - (1) Although blocking on the global network lock is typically required since any
 *                         external API function access is asynchronous to other network protocol tasks;
 *                         interrupt service routines (ISRs) are (typically) prohibited from pending on
 *                         OS objects & therefore can NOT acquire the global network lock.
 *                   - (2) Therefore, ALL network interface & network device driver functions called by
 *                         NetIF_WiFi_ISR_Handler() MUST be able to be asynchronously accessed without
 *                         the global network lock & without corrupting any network data or task.
 *
 * @note     (2) Network device interrupt service routines (ISR) handler(s) SHOULD be able to correctly
 *               function regardless of whether their corresponding network interface(s) are enabled.
 *               See also Note #1b2.
 *******************************************************************************************************/
void NetIF_802x_ISR_Handler(NET_IF           *p_if,
                            NET_DEV_ISR_TYPE type,
                            RTOS_ERR         *p_err)
{
  NET_DEV_API *p_dev_api;

  //                                                               ---------------- VALIDATE ISR TYPE ----------------
  switch (type) {
    case NET_DEV_ISR_TYPE_UNKNOWN:
    case NET_DEV_ISR_TYPE_RX:
    case NET_DEV_ISR_TYPE_RX_RUNT:
    case NET_DEV_ISR_TYPE_RX_OVERRUN:
    case NET_DEV_ISR_TYPE_TX_RDY:
    case NET_DEV_ISR_TYPE_TX_COMPLETE:
    case NET_DEV_ISR_TYPE_TX_COLLISION_LATE:
    case NET_DEV_ISR_TYPE_TX_COLLISION_EXCESS:
      p_dev_api = (NET_DEV_API *)p_if->Dev_API;
      RTOS_ASSERT_DBG_ERR_SET((p_dev_api->ISR_Handler != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
      p_dev_api->ISR_Handler(p_if, type);
      break;

    case NET_DEV_ISR_TYPE_PHY:
    case NET_DEV_ISR_TYPE_JABBER:
    case NET_DEV_ISR_TYPE_BABBLE:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_SUPPORTED,; );
    //                                                             Fallthrough

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetIF_802x_RxPktFrameDemux()
 *
 * @brief    (1) Validate received packet frame & demultiplex to appropriate protocol layer :
 *               - (a) Validate destination address :
 *                   - (1) Check for broadcast address                 See RFC #1122, Section 2.4
 *                   - (2) Check for multicast address                 See RFC #1112, Section 6.4
 *                   - (3) Check for this host's hardware address
 *               - (b) Validate source address                         See 'NetIF_802x_IsValidAddrSrc()  Note #1'
 *               - (c) Demultiplex & validate frame :
 *                   - (1) Ethernet   frame type
 *                   - (2) IEEE 802.3 frame length
 *               - (d) Demultiplex packet to appropriate protocol layer :
 *                   - (1) IP  receive
 *                   - (2) ARP receive
 *
 * @param    p_if            Pointer to an 802x network interface that received a packet.
 *
 * @param    p_buf           Pointer to a       network buffer    that received a packet.
 *
 * @param    p_buf_hdr       Pointer to received packet frame's network buffer header.
 *
 * @param    p_if_hdr        Pointer to received packet frame's header.
 *
 * @param    p_ctrs_stat     Pointer to an 802x network interface statistic counters.
 *
 * @param    p_ctrs_err      Pointer to an 802x network interface error     counters.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) When network buffer is demultiplexed to higher-layer protocol receive, the buffer's
 *               reference counter is NOT incremented since the network interface layer does NOT
 *               maintain a reference to the buffer.
 *******************************************************************************************************/
static void NetIF_802x_RxPktFrameDemux(NET_IF                *p_if,
                                       NET_BUF               *p_buf,
                                       NET_BUF_HDR           *p_buf_hdr,
                                       NET_IF_HDR_802x       *p_if_hdr,
                                       NET_CTR_IF_802x_STATS *p_ctrs_stat,
                                       NET_CTR_IF_802x_ERRS  *p_ctrs_err,
                                       RTOS_ERR              *p_err)
{
  NET_IF_DATA_802x *p_if_data;
  CPU_BOOLEAN      valid;
  CPU_BOOLEAN      dest_this_host;
#ifdef  NET_MCAST_RX_MODULE_EN
  CPU_BOOLEAN dest_multicast;
#endif
  CPU_BOOLEAN dest_broadcast;
  CPU_INT16U  frame_type_len;

  PP_UNUSED_PARAM(p_ctrs_stat);                                 // Prevent possible 'variable unused' warnings.

  //                                                               ---------------- VALIDATE DEST ADDR ----------------
  dest_broadcast = Mem_Cmp(&p_if_hdr->AddrDest[0],
                           &NetIF_802x_AddrBroadcast[0],
                           NET_IF_802x_ADDR_SIZE);

#ifdef  NET_MCAST_RX_MODULE_EN
  dest_multicast = ((p_if_hdr->AddrDest[0] & NetIF_802x_AddrMulticastMask[0])
                    == NetIF_802x_AddrMulticastMask[0]) ? DEF_YES : DEF_NO;
#endif

  if (dest_broadcast == DEF_YES) {
    NET_CTR_STAT_INC(Net_StatCtrs.IFs.IFs_802xCtrs.RxPktBcastCtr);
    NET_CTR_STAT_INC(p_ctrs_stat->RxPktBcastCtr);
    DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_RX_BROADCAST);       // Flag rx'd multicast pkt (see Note #1a1).

#ifdef  NET_MCAST_RX_MODULE_EN
  } else if (dest_multicast == DEF_YES) {
    NET_CTR_STAT_INC(Net_StatCtrs.IFs.IFs_802xCtrs.RxPktMcastCtr);
    NET_CTR_STAT_INC(p_ctrs_stat->RxPktMcastCtr);
    DEF_BIT_SET(p_buf_hdr->Flags, NET_BUF_FLAG_RX_MULTICAST);       // Flag rx'd broadcast pkt (see Note #1a2).
#endif
  } else {
    p_if_data = (NET_IF_DATA_802x *)p_if->IF_Data;
    dest_this_host = Mem_Cmp(&p_if_hdr->AddrDest[0],
                             &p_if_data->HW_Addr[0],
                             NET_IF_802x_ADDR_SIZE);
    if (dest_this_host != DEF_YES) {                            // Discard invalid dest addr (see Note #1a3).
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvAddrDestCtr);
      NET_CTR_ERR_INC(p_ctrs_err->RxInvAddrDestCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  }

  //                                                               ---------------- VALIDATE SRC  ADDR ----------------
  valid = NetIF_802x_AddrHW_IsValid(p_if, &p_if_hdr->AddrSrc[0]);

  if (valid != DEF_YES) {                                       // Discard invalid src addr (see Note #1b).
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvAddrSrcCtr);
    NET_CTR_ERR_INC(p_ctrs_err->RxInvAddrSrcCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

#ifdef  NET_DAD_MODULE_EN
  //                                                               --------------- DEMUX RX IF HW ADDR ----------------
  p_buf_hdr->IF_HW_AddrLen = NET_IF_802x_ADDR_SIZE;
  p_buf_hdr->IF_HW_AddrSrcPtr = &p_if_hdr->AddrSrc[0];
  p_buf_hdr->IF_HW_AddrDestPtr = &p_if_hdr->AddrDest[0];
#endif

  //                                                               --------------- DEMUX/VALIDATE FRAME ---------------
  NET_UTIL_VAL_COPY_GET_NET_16(&frame_type_len, &p_if_hdr->FrameType_Len);
  if (frame_type_len <= NET_IF_IEEE_802_FRAME_LEN_MAX) {
    NetIF_802x_RxPktFrameDemuxIEEE802(p_if,
                                      p_buf_hdr,
                                      p_if_hdr,
                                      p_ctrs_err,
                                      p_err);
  } else {
    NetIF_802x_RxPktFrameDemuxEther(p_if,
                                    p_buf_hdr,
                                    p_if_hdr,
                                    p_ctrs_err,
                                    p_err);
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }
  //                                                               -------------------- DEMUX PKT ---------------------
  //                                                               See Note #2.

  switch (p_buf_hdr->ProtocolHdrType) {                         // Demux buf to appropriate protocol.
#ifdef  NET_ARP_MODULE_EN
    case NET_PROTOCOL_TYPE_ARP:
      NetARP_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

#ifdef  NET_IPv4_MODULE_EN
    case NET_PROTOCOL_TYPE_IP_V4:
      NetIPv4_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case  NET_PROTOCOL_TYPE_IP_V6:
      NetIPv6_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

    case NET_PROTOCOL_TYPE_NONE:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvProtocolCtr);
      NET_CTR_ERR_INC(p_ctrs_err->RxInvProtocolCtr);
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IF[p_if->Nbr].RxInvProtocolCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIF_802x_RxPktFrameDemuxEther()
 *
 * @brief    (1) Validate & demultiplex Ethernet packet frame :
 *               - (a) Validate & demultiplex Ethernet packet frame
 *               - (b) Update buffer controls
 *
 * @param    p_buf_hdr   Pointer to received packet frame's network buffer header.
 *
 * @param    p_if_hdr    Pointer to received packet frame's header.
 *
 * @param    p_ctrs_err  Pointer to an 802x network interface error counters.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIF_802x_RxPktFrameDemuxEther(NET_IF               *p_if,
                                            NET_BUF_HDR          *p_buf_hdr,
                                            NET_IF_HDR_802x      *p_if_hdr,
                                            NET_CTR_IF_802x_ERRS *p_ctrs_err,
                                            RTOS_ERR             *p_err)
{
  NET_IF_HDR_ETHER *p_if_hdr_ether;
  CPU_INT16U       frame_type;
  CPU_INT16U       ix;

  PP_UNUSED_PARAM(p_ctrs_err);                                  // Prevent possible 'variable unused' warnings.
  PP_UNUSED_PARAM(p_if);

  p_if_hdr_ether = (NET_IF_HDR_ETHER *)p_if_hdr;

  //                                                               -------------- VALIDATE / DEMUX FRAME --------------
  NET_UTIL_VAL_COPY_GET_NET_16(&frame_type, &p_if_hdr_ether->FrameType);
  ix = p_buf_hdr->IF_HdrIx + NET_IF_HDR_SIZE_ETHER;
  switch (frame_type) {                                         // Validate & demux Ether frame type.
#ifdef  NET_IPv4_MODULE_EN
    case NET_IF_802x_FRAME_TYPE_IPv4:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V4;
      p_buf_hdr->ProtocolHdrTypeNet = NET_PROTOCOL_TYPE_IP_V4;
      p_buf_hdr->IP_HdrIx = (NET_BUF_SIZE)ix;
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_IF_802x_FRAME_TYPE_IPv6:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6;
      p_buf_hdr->ProtocolHdrTypeNet = NET_PROTOCOL_TYPE_IP_V6;
      p_buf_hdr->IP_HdrIx = (NET_BUF_SIZE)ix;
      break;
#endif

#ifdef  NET_ARP_MODULE_EN
    case NET_IF_802x_FRAME_TYPE_ARP:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_ARP;
      p_buf_hdr->ProtocolHdrTypeIF_Sub = NET_PROTOCOL_TYPE_ARP;
      p_buf_hdr->ARP_MsgIx = (NET_BUF_SIZE)ix;
      break;
#endif

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvFrameCtr);
      NET_CTR_ERR_INC(p_ctrs_err->RxInvFrameCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  //                                                               ----------------- UPDATE BUF CTRLS -----------------
  p_buf_hdr->IF_HdrLen = NET_IF_HDR_SIZE_ETHER;
  p_buf_hdr->DataLen -= (NET_BUF_SIZE)p_buf_hdr->IF_HdrLen;

  p_buf_hdr->ProtocolHdrTypeIF = NET_PROTOCOL_TYPE_IF_ETHER;

exit:
  return;
}

/****************************************************************************************************//**
 *                                   NetIF_802x_RxPktFrameDemuxIEEE802()
 *
 * @brief    (1) Validate & demultiplex IEEE 802 packet frame :
 *              - (a) Validate & demultiplex IEEE 802 packet frame
 *                  - (1) IEEE 802.2 LLC
 *                  - (2) IEEE 802.2 SNAP Organization Code
 *                  - (3) IEEE 802.2 SNAP Frame Type
 *              - (b) Update buffer controls
 *
 * @param    p_buf_hdr   Pointer to received packet frame's network buffer header.
 *
 * @param    p_if_hdr    Pointer to received packet frame's header.
 *
 * @param    p_ctrs_err  Pointer to an 802x network interface error counters.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) The IEEE 802.3 Frame Length field specifies the number of frame data octets & does NOT
 *               include the trailing frame CRC field octets.  However, since some Ethernet devices MAY
 *               append the CRC field as part of a received packet frame, any validation of the minimum
 *               frame size MUST assume that the CRC field may be present.  Therefore, the minimum frame
 *               packet size for comparison MUST include the number of CRC field octets.
 *******************************************************************************************************/
static void NetIF_802x_RxPktFrameDemuxIEEE802(NET_IF               *p_if,
                                              NET_BUF_HDR          *p_buf_hdr,
                                              NET_IF_HDR_802x      *p_if_hdr,
                                              NET_CTR_IF_802x_ERRS *p_ctrs_err,
                                              RTOS_ERR             *p_err)
{
  NET_IF_HDR_IEEE_802 *p_if_hdr_ieee_802;
  CPU_INT16U          frame_len;
  CPU_INT16U          frame_len_actual;
  CPU_INT16U          frame_type;
  CPU_INT16U          ix;

  PP_UNUSED_PARAM(p_ctrs_err);                                  // Prevent possible 'variable unused' warnings.
  PP_UNUSED_PARAM(p_if);

  p_if_hdr_ieee_802 = (NET_IF_HDR_IEEE_802 *)p_if_hdr;

  //                                                               ------------- VALIDATE FRAME SIZE --------------
  if (p_buf_hdr->TotLen >= NET_IF_802x_FRAME_MIN_CRC_SIZE) {        // If pkt size >= min frame pkt size (see Note #2)
    NET_UTIL_VAL_COPY_GET_NET_16(&frame_len, &p_if_hdr_ieee_802->FrameLen);
    frame_len_actual = (CPU_INT16U)(p_buf_hdr->TotLen - NET_IF_HDR_SIZE_ETHER - NET_IF_802x_FRAME_CRC_SIZE);
    if (frame_len != frame_len_actual) {                            // ... & frame len != rem pkt len, rtn err.
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvFrameCtr);
      NET_CTR_ERR_INC(p_ctrs_err->RxInvFrameCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  }

  //                                                               ------------ VALIDATE IEEE 802.2 LLC -----------
  if (p_if_hdr_ieee_802->LLC_DSAP != NET_IF_IEEE_802_LLC_DSAP) {    // Validate IEEE 802.2 LLC DSAP.
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvFrameCtr);
    NET_CTR_ERR_INC(p_ctrs_err->RxInvFrameCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (p_if_hdr_ieee_802->LLC_SSAP != NET_IF_IEEE_802_LLC_SSAP) {    // Validate IEEE 802.2 LLC SSAP.
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvFrameCtr);
    NET_CTR_ERR_INC(p_ctrs_err->RxInvFrameCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  if (p_if_hdr_ieee_802->LLC_Ctrl != NET_IF_IEEE_802_LLC_CTRL) {    // Validate IEEE 802.2 LLC Ctrl.
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvFrameCtr);
    NET_CTR_ERR_INC(p_ctrs_err->RxInvFrameCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
  //                                                               ----------- VALIDATE IEEE 802.2 SNAP -----------
  //                                                               Validate IEEE 802.2 SNAP OUI.
  if (p_if_hdr_ieee_802->SNAP_OrgCode[0] != NET_IF_IEEE_802_SNAP_CODE_00) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvFrameCtr);
    NET_CTR_ERR_INC(p_ctrs_err->RxInvFrameCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
  if (p_if_hdr_ieee_802->SNAP_OrgCode[1] != NET_IF_IEEE_802_SNAP_CODE_01) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvFrameCtr);
    NET_CTR_ERR_INC(p_ctrs_err->RxInvFrameCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }
  if (p_if_hdr_ieee_802->SNAP_OrgCode[2] != NET_IF_IEEE_802_SNAP_CODE_02) {
    NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvFrameCtr);
    NET_CTR_ERR_INC(p_ctrs_err->RxInvFrameCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  NET_UTIL_VAL_COPY_GET_NET_16(&frame_type, &p_if_hdr_ieee_802->SNAP_FrameType);
  ix = p_buf_hdr->IF_HdrIx + NET_IF_HDR_SIZE_IEEE_802;
  switch (frame_type) {                                             // Validate & demux IEEE 802.2 SNAP Frame Type.
#ifdef  NET_IPv4_MODULE_EN
    case NET_IF_IEEE_802_SNAP_TYPE_IPv4:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V4;
      p_buf_hdr->ProtocolHdrTypeNet = NET_PROTOCOL_TYPE_IP_V4;
      p_buf_hdr->IP_HdrIx = (NET_BUF_SIZE)ix;
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_IF_IEEE_802_SNAP_TYPE_IPv6:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IP_V6;
      p_buf_hdr->ProtocolHdrTypeNet = NET_PROTOCOL_TYPE_IP_V6;
      p_buf_hdr->IP_HdrIx = (NET_BUF_SIZE)ix;
      break;
#endif

#ifdef  NET_ARP_MODULE_EN
    case NET_IF_IEEE_802_SNAP_TYPE_ARP:
      p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_ARP;
      p_buf_hdr->ProtocolHdrTypeIF_Sub = NET_PROTOCOL_TYPE_ARP;
      p_buf_hdr->ARP_MsgIx = (NET_BUF_SIZE)ix;
      break;
#endif

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.RxInvFrameCtr);
      NET_CTR_ERR_INC(p_ctrs_err->RxInvFrameCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

  //                                                               --------------- UPDATE BUF CTRLS ---------------
  p_buf_hdr->IF_HdrLen = NET_IF_HDR_SIZE_IEEE_802;
  p_buf_hdr->DataLen -= (NET_BUF_SIZE)p_buf_hdr->IF_HdrLen;

  p_buf_hdr->ProtocolHdrTypeIF = NET_PROTOCOL_TYPE_IF_IEEE_802;

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIF_802x_TxPktPrepareFrame()
 *
 * @brief    (1) Prepare data packet with 802x frame format :
 *               - (a) Demultiplex Ethernet frame type
 *               - (b) Update buffer controls
 *               - (c) Write Ethernet values into packet frame
 *                   - (1) Ethernet destination broadcast address, if necessary
 *                   - (2) Ethernet source      MAC       address
 *                   - (3) Ethernet frame type
 *               - (d) Clear Ethernet frame pad octets, if any
 *
 * @param    p_if            Pointer to an 802x network interface.
 *
 * @param    p_buf           Pointer to network buffer with data packet to encapsulate.
 *
 * @param    p_buf_hdr       Pointer to network buffer header.
 *
 * @param    p_ctrs_stat     Pointer to an 802x network interface error counters.
 *
 * @param    p_ctrs_err      Pointer to an 802x network interface error counters.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Supports ONLY Ethernet frame format for network transmit (see 'net_if_802x.c  Note #2a').
 *
 * @note     (3) Supports ONLY ARP & IP protocols (see 'net.h  Note #2a').
 *
 * @note     (4) To prepare the packet buffer for ARP resolution, the buffer's ARP protocol address
 *               pointer needs to be configured to the appropriate outbound address :
 *               - (a) For ARP packets, the ARP layer will configure the ARP protocol address pointer
 *                     (see 'net_arp.c  NetARP_TxPktPrepareHdr()  Note #1d').
 *               - (b) For IP  packets, configure the ARP protocol address pointer to the IP's next-
 *                     route address.
 *
 * @note     (5) See also: RFC #894, Section 'Frame Format'.
 *******************************************************************************************************/
static NET_IF_TX_RDY_STATUS NetIF_802x_TxPktPrepareFrame(NET_IF                *p_if,
                                                         NET_BUF               *p_buf,
                                                         NET_BUF_HDR           *p_buf_hdr,
                                                         NET_CTR_IF_802x_STATS *p_ctrs_stat,
                                                         NET_CTR_IF_802x_ERRS  *p_ctrs_err,
                                                         RTOS_ERR              *p_err)
{
#ifdef  NET_MCAST_TX_MODULE_EN
  CPU_BOOLEAN tx_multicast;
#endif
  NET_IF_DATA_802x     *p_if_data;
  NET_IF_HDR_ETHER     *p_if_hdr_ether;
  CPU_INT16U           protocol_ix = 0u;
  CPU_INT16U           frame_type = NET_IF_802x_FRAME_TYPE_IPv4;
  CPU_INT16U           clr_ix;
  CPU_INT16U           clr_len;
  CPU_INT16U           clr_size;
  CPU_BOOLEAN          clr_buf_mem;
  CPU_BOOLEAN          tx_broadcast;
  NET_IF_TX_RDY_STATUS tx_status = NET_IF_TX_RDY_STATUS_NONE;

  PP_UNUSED_PARAM(p_ctrs_stat);                                 // Prevent possible 'variable unused' warnings.
  PP_UNUSED_PARAM(p_ctrs_err);

  //                                                               ----------------- DEMUX FRAME TYPE -----------------
  switch (p_buf_hdr->ProtocolHdrType) {                         // Demux protocol for frame type (see Note #3).
    case NET_PROTOCOL_TYPE_IF_ETHER:
      tx_status = NET_IF_TX_RDY_STATUS_UNICAST;
      goto exit;

#ifdef  NET_ARP_MODULE_EN
    case NET_PROTOCOL_TYPE_ARP:
      protocol_ix = p_buf_hdr->ARP_MsgIx;
      frame_type = NET_IF_802x_FRAME_TYPE_ARP;
      break;
#endif

#ifdef  NET_IPv4_MODULE_EN
    case NET_PROTOCOL_TYPE_IP_V4:
      protocol_ix = p_buf_hdr->IP_HdrIx;
      frame_type = NET_IF_802x_FRAME_TYPE_IPv4;
#ifdef  NET_ARP_MODULE_EN
      //                                                           Cfg ARP addr ptr (see Note #4b).
      p_buf_hdr->ARP_AddrProtocolPtr = (CPU_INT08U *)&p_buf_hdr->IP_AddrNextRouteNetOrder;
#endif
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_PROTOCOL_TYPE_IP_V6:
      protocol_ix = p_buf_hdr->IP_HdrIx;
      frame_type = NET_IF_802x_FRAME_TYPE_IPv6;
      //                                                           Cfg NDP addr ptr (see Note #4b).
#ifdef  NET_ARP_MODULE_EN
      p_buf_hdr->ARP_AddrProtocolPtr = (CPU_INT08U *)&p_buf_hdr->IPv6_AddrNextRoute;
#endif
#ifdef  NET_NDP_MODULE_EN
      p_buf_hdr->NDP_AddrProtocolPtr = (CPU_INT08U *)&p_buf_hdr->IPv6_AddrNextRoute;
#endif
      break;
#endif

    case NET_PROTOCOL_TYPE_NONE:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IFs_802x.TxInvProtocolCtr);
      NET_CTR_ERR_INC(p_ctrs_err->TxInvProtocolCtr);
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL, tx_status);
  }

  //                                                               ----------- UPDATE BUF CTRLS -----------
  p_buf_hdr->IF_HdrLen = NET_IF_HDR_SIZE_ETHER;
  p_buf_hdr->IF_HdrIx = protocol_ix - p_buf_hdr->IF_HdrLen;
  p_buf_hdr->TotLen += (NET_BUF_SIZE) p_buf_hdr->IF_HdrLen;

  p_buf_hdr->ProtocolHdrType = NET_PROTOCOL_TYPE_IF_ETHER;
  p_buf_hdr->ProtocolHdrTypeIF = NET_PROTOCOL_TYPE_IF_ETHER;

  //                                                               ---------- PREPARE 802x FRAME ----------
  p_if_hdr_ether = (NET_IF_HDR_ETHER *)&p_buf->DataPtr[p_buf_hdr->IF_HdrIx];

  //                                                               --------- PREPARE FRAME ADDRS ----------
  tx_broadcast = DEF_BIT_IS_SET(p_buf_hdr->Flags, NET_BUF_FLAG_TX_BROADCAST);
#ifdef  NET_MCAST_TX_MODULE_EN
  tx_multicast = DEF_BIT_IS_SET(p_buf_hdr->Flags, NET_BUF_FLAG_TX_MULTICAST);
#endif

  if (tx_broadcast == DEF_YES) {                                            // If dest addr broadcast,      ...
    NET_UTIL_VAL_COPY(&p_if_hdr_ether->AddrDest[0],                         // ... wr broadcast addr into frame.
                      &NetIF_802x_AddrBroadcast[0],
                      NET_IF_802x_ADDR_SIZE);
    NET_CTR_STAT_INC(Net_StatCtrs.IFs.IFs_802xCtrs.TxPktBcastCtr);
    NET_CTR_STAT_INC(p_ctrs_stat->TxPktBcastCtr);
    tx_status = NET_IF_TX_RDY_STATUS_BROADCAST;

#ifdef  NET_MCAST_TX_MODULE_EN
  } else if (tx_multicast == DEF_YES) {
    //                                                             If dest addr multicast, ...
    if (frame_type != NET_IF_802x_FRAME_TYPE_IPv6) {
      NET_CTR_STAT_INC(Net_StatCtrs.IFs.IFs_802xCtrs.TxPktMcastCtr);
      NET_CTR_STAT_INC(p_ctrs_stat->TxPktMcastCtr);

#ifdef  NET_ARP_MODULE_EN
      p_buf_hdr->ARP_AddrHW_Ptr = &p_if_hdr_ether->AddrDest[0];                 // ...  req hw addr binding.
#endif
    } else {
#ifdef  NET_IPv6_MODULE_EN
      NetIPv6_AddrHW_McastSet(&p_if_hdr_ether->AddrDest[0], &p_buf_hdr->IPv6_AddrDest);
#ifdef  NET_NDP_MODULE_EN
      p_buf_hdr->NDP_AddrHW_Ptr = &p_if_hdr_ether->AddrDest[0];
#endif

#endif
    }
    tx_status = NET_IF_TX_RDY_STATUS_MULTICAST;
#endif
  } else {                                                                  // Else req hw addr binding.
#ifdef  NET_ARP_MODULE_EN
    p_buf_hdr->ARP_AddrHW_Ptr = &p_if_hdr_ether->AddrDest[0];
#endif
#ifdef  NET_NDP_MODULE_EN
    p_buf_hdr->NDP_AddrHW_Ptr = &p_if_hdr_ether->AddrDest[0];
#endif
    tx_status = NET_IF_TX_RDY_STATUS_ADDR_REQ;
  }

  p_if_data = (NET_IF_DATA_802x *)p_if->IF_Data;
  NET_UTIL_VAL_COPY(&p_if_hdr_ether->AddrSrc[0],                            // Wr src addr into frame.
                    &p_if_data->HW_Addr[0],
                    NET_IF_802x_ADDR_SIZE);

  //                                                               ---------- PREPARE FRAME TYPE ----------
  NET_UTIL_VAL_COPY_SET_NET_16(&p_if_hdr_ether->FrameType, &frame_type);

  //                                                               --------- CLR/PAD FRAME OCTETS ---------
  if (p_buf_hdr->TotLen < NET_IF_802x_FRAME_MIN_SIZE) {                     // If tot len < min frame len (see Note #5)
    clr_buf_mem = DEF_BIT_IS_SET(p_buf_hdr->Flags, NET_BUF_FLAG_CLR_MEM);
    if (clr_buf_mem != DEF_YES) {                                           // ... & buf mem NOT clr,               ...
      clr_ix = p_buf_hdr->IF_HdrIx        + (CPU_INT16U)p_buf_hdr->TotLen;
      clr_len = NET_IF_802x_FRAME_MIN_SIZE - (CPU_INT16U)p_buf_hdr->TotLen;
      clr_size = clr_ix + clr_len;
      if (clr_size > p_buf_hdr->Size) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_TX);
        goto exit;
      }
      Mem_Clr((void *)&p_buf->DataPtr[clr_ix],                              // ... clr rem'ing octets (see Note #5).
              (CPU_SIZE_T) clr_len);
    }
    p_buf_hdr->TotLen = NET_IF_802x_FRAME_MIN_SIZE;                         // Update tot frame len.
  }

exit:
  return (tx_status);
}

/****************************************************************************************************//**
 *                                           NetIF_802x_TxIxDataGet()
 *
 * @brief    Get the offset of a buffer at which the IP data CAN be written.
 *
 * @param    p_ix    Pointer to the current protocol index.
 *******************************************************************************************************/
static void NetIF_802x_TxIxDataGet(CPU_INT16U *p_ix)
{
  *p_ix += NET_IF_HDR_SIZE_ETHER_MIN;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IF_802x_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
