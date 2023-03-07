/***************************************************************************//**
 * @file
 * @brief Network Loopback Interface Layer
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

#ifdef  NET_IF_LOOPBACK_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "net_if_loopback_priv.h"
#include  "net_if_priv.h"
#include  "net_type_priv.h"

#ifdef NET_IPv4_MODULE_EN
#include  "net_ipv4_priv.h"
#endif

#ifdef NET_IPv6_MODULE_EN
#include  "net_ipv6_priv.h"
#endif

#include  <net/include/net_if.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_BUF *NetIF_Loopback_RxQ_Head;           // Ptr to loopback IF rx Q head.
static NET_BUF *NetIF_Loopback_RxQ_Tail;           // Ptr to loopback IF rx Q tail.

static NET_STAT_CTR NetIF_Loopback_RxQ_PktCtr;     // Net loopback IF rx pkts ctr.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------- RX FNCTS -----------

static void NetIF_Loopback_RxQ_Add(NET_BUF *p_buf);

static NET_BUF *NetIF_Loopback_RxQ_Get(void);

static void NetIF_Loopback_RxQ_Remove(NET_BUF *p_buf);

static void NetIF_Loopback_RxQ_Unlink(NET_BUF *p_buf);

static void NetIF_Loopback_RxPktDemux(NET_BUF  *p_buf,
                                      RTOS_ERR *p_err);

//                                                                 ----------- TX FNCTS -----------
static void NetIF_Loopback_TxPktFree(NET_BUF *p_buf);

static void NetIF_Loopback_TxPktDiscard(NET_BUF *p_buf);

//                                                                 ---------- API FNCTS -----------

static void NetIF_Loopback_IF_Add(NET_IF   *p_if,
                                  RTOS_ERR *p_err);

static void NetIF_Loopback_IF_Start(NET_IF   *p_if,
                                    RTOS_ERR *p_err);

static void NetIF_Loopback_IF_Stop(NET_IF   *p_if,
                                   RTOS_ERR *p_err);

//                                                                 ---------- MGMT FNCTS ----------

static void NetIF_Loopback_AddrHW_Get(NET_IF     *p_if,
                                      CPU_INT08U *p_addr_hw,
                                      CPU_INT08U *p_addr_len,
                                      RTOS_ERR   *p_err);

static void NetIF_Loopback_AddrHW_Set(NET_IF     *p_if,
                                      CPU_INT08U *p_addr_hw,
                                      CPU_INT08U addr_len,
                                      RTOS_ERR   *p_err);

static CPU_BOOLEAN NetIF_Loopback_AddrHW_IsValid(NET_IF     *p_if,
                                                 CPU_INT08U *p_addr_hw);

static void NetIF_Loopback_AddrMulticastAdd(NET_IF            *p_if,
                                            CPU_INT08U        *p_addr_protocol,
                                            CPU_INT08U        addr_protocol_len,
                                            NET_PROTOCOL_TYPE addr_protocol_type,
                                            RTOS_ERR          *p_err);

static void NetIF_Loopback_AddrMulticastRemove(NET_IF            *p_if,
                                               CPU_INT08U        *p_addr_protocol,
                                               CPU_INT08U        addr_protocol_len,
                                               NET_PROTOCOL_TYPE addr_protocol_type,
                                               RTOS_ERR          *p_err);

static void NetIF_Loopback_AddrMulticastProtocolToHW(NET_IF            *p_if,
                                                     CPU_INT08U        *p_addr_protocol,
                                                     CPU_INT08U        addr_protocol_len,
                                                     NET_PROTOCOL_TYPE addr_protocol_type,
                                                     CPU_INT08U        *p_addr_hw,
                                                     CPU_INT08U        *p_addr_hw_len,
                                                     RTOS_ERR          *p_err);

static void NetIF_Loopback_BufPoolCfgValidate(NET_IF   *p_if,
                                              RTOS_ERR *p_err);

static void NetIF_Loopback_MTU_Set(NET_IF   *p_if,
                                   NET_MTU  mtu,
                                   RTOS_ERR *p_err);

static CPU_INT16U NetIF_Loopback_GetPktSizeHdr(NET_IF *p_if);

static CPU_INT16U NetIF_Loopback_GetPktSizeMin(NET_IF *p_if);

static CPU_INT16U NetIF_Loopback_GetPktSizeMax(NET_IF *p_if);

static void NetIF_Loopback_ISR_Handler(NET_IF           *p_if,
                                       NET_DEV_ISR_TYPE type,
                                       RTOS_ERR         *p_err);

static void NetIF_Loopback_IO_CtrlHandler(NET_IF     *p_if,
                                          CPU_INT08U opt,
                                          void       *p_data,
                                          RTOS_ERR   *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const NET_IF_API NetIF_API_Loopback = {                                             // Loopback IF API fnct ptrs :
  &NetIF_Loopback_IF_Add,                                                           // Init/add
  &NetIF_Loopback_IF_Start,                                                         // Start
  &NetIF_Loopback_IF_Stop,                                                          // Stop
  DEF_NULL,                                                                         // Rx
  DEF_NULL,                                                                         // Tx
  &NetIF_Loopback_AddrHW_Get,                                                       // Hw        addr get
  &NetIF_Loopback_AddrHW_Set,                                                       // Hw        addr set
  &NetIF_Loopback_AddrHW_IsValid,                                                   // Hw        addr valid
  &NetIF_Loopback_AddrMulticastAdd,                                                 // Multicast addr add
  &NetIF_Loopback_AddrMulticastRemove,                                              // Multicast addr remove
  &NetIF_Loopback_AddrMulticastProtocolToHW,                                        // Multicast addr protocol-to-hw
  &NetIF_Loopback_BufPoolCfgValidate,                                               // Buf cfg validation
  &NetIF_Loopback_MTU_Set,                                                          // MTU set
  &NetIF_Loopback_GetPktSizeHdr,                                                    // Get pkt hdr size
  &NetIF_Loopback_GetPktSizeMin,                                                    // Get pkt min size
  &NetIF_Loopback_GetPktSizeMax,                                                    // Get pkt max size
  &NetIF_Loopback_ISR_Handler,                                                      // ISR handler
  &NetIF_Loopback_IO_CtrlHandler                                                    // I/O ctrl
};

const NET_IF_CFG_LOOPBACK NetIF_CfgLoopbackDflt = {
  NET_IF_MEM_TYPE_MAIN,         // Desired receive  buffer memory pool type :
                                //                                 NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory
                                //                                 NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (loopback's) dedicated memory
  1500u,                        // Desired size      of interface's large receive  buffers (in octets) [see Note #2].
  10u,                          // Desired number    of interface's large receive  buffers             [see Note #3a].
  4u,                           // Desired alignment of interface's       receive  buffers (in octets) [see Note #4].
  0u,                           // Desired offset from base receive  index, if needed      (in octets) [see Note #5a1].

  NET_IF_MEM_TYPE_MAIN,         // Desired transmit buffer memory pool type :
                                //                                 NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory
                                //                                 NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (loopback's) dedicated memory
  1500u,                        // Desired size      of interface's large transmit buffers (in octets) [see Note #2].
  10u,                          // Desired number    of interface's large transmit buffers             [see Note #3b].
  60u,                          // Desired size      of interface's small transmit buffers (in octets) [see Note #2].
  10u,                          // Desired number    of interface's small transmit buffers             [see Note #3b].
  4u,                           // Desired alignment of interface's       transmit buffers (in octets) [see Note #4].
  0u,                           // Desired offset from base transmit index, if needed      (in octets) [see Note #5a2].

  0x00000000u,                  // Base address   of dedicated memory, if available.
  0u,                           // Size           of dedicated memory, if available (in octets).

  NET_DEV_CFG_FLAG_NONE,        // Desired option flags, if any (see Note #6).
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetIF_Loopback_Init()
 *
 * @brief    (1) Initialize Network Loopback Interface Module :
 *              - (a) Initialize network loopback interface counter(s)
 *              - (b) Initialize network loopback interface receive queue pointers
 *              - (c) Add        network loopback interface
 *              - (d) Start      network loopback interface
 *
 * @param    p_mem_seg   Pointer to memory segment to use for allocation.
 *
 *
 * Argument(s) : p_mem_seg   Pointer to memory segment to use for allocation.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (2) The following network loopback interface initialization functions MUST be sequenced
 *                   as follows :
 *
 *                   (a) NetIF_Loopback_Init() MUST precede ALL other network loopback interface
 *                           initialization functions
 *
 *                   (b) NetIF_Add() & NetIF_Start() MUST :
 *
 *                       (1) Follow  NetIF_Init()'s initialization       of the network global lock
 *                               (see also 'net_if.c  NetIF_Init()   Note #2b')
 *                       (2) Precede any network or application task access to the network global lock
 *                               (see also 'net_if.c  NetIF_Add()    Note #2'
 *                                       & 'net_if.c  NetIF_Start()  Note #2')
 *******************************************************************************************************/
void NetIF_Loopback_Init(MEM_SEG  *p_mem_seg,
                         RTOS_ERR *p_err)
{
  NET_IF_NBR if_nbr;

  //                                                               ----------- INIT NET LOOPBACK IF CTR(s) ------------
  NetStat_CtrInit(&NetIF_Loopback_RxQ_PktCtr);

  //                                                               -------------- INIT NET LOOPBACK RX Q --------------
  NetIF_Loopback_RxQ_Head = DEF_NULL;
  NetIF_Loopback_RxQ_Tail = DEF_NULL;

  //                                                               ----------------- INIT LOOPBACK IF -----------------
  //                                                               Start Loopback IF (see Note #2b).
  if_nbr = NetIF_Add((void *)&NetIF_API_Loopback,               // Loopback IF's                      API.
                     DEF_NULL,                                  // Loopback IF   does NOT support dev API.
                     DEF_NULL,                                  // Loopback IF   does NOT support dev BSP.
                     (void *)&NetIF_CfgLoopbackDflt,            // Loopback IF's                  dev cfg.
                     DEF_NULL,                                  // Loopback IF   does NOT support Phy API.
                     DEF_NULL,                                  // Loopback IF   does NOT support Phy cfg.
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  NetIF_Start(if_nbr, p_err);                         // Start Loopback IF.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  PP_UNUSED_PARAM(p_mem_seg);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_Loopback_Rx()
 *
 * @brief    (1) Receive & handle packets from the network loopback interface :
 *               - (a) Receive packet from network loopback interface :
 *                   - (1) Update  receive packet counters
 *                   - (2) Get     receive packet from loopback receive queue
 *               - (b) Validate    receive packet
 *               - (c) Demultiplex receive packet to network layer protocols
 *               - (d) Update      receive statistics
 *
 * @param    p_if    Pointer to network loopback interface that received a packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Size of received packet, if NO error(s).
 *           0, otherwise.
 *
 * @note     (2) If a network interface receives a packet, its physical link must be 'UP' & the
 *               interface's physical link state is set accordingly.
 *               - (a) An attempt to check for link state is made after an interface has been started.
 *                     However, many physical layer devices, such as Ethernet physical layers require
 *                     several seconds for Auto-Negotiation to complete before the link becomes
 *                     established.  Thus the interface link flag is not updated until the link state
 *                     timer expires & one or more attempts to check for link state have been completed.
 *
 * @note     (3) When network buffer is demultiplexed to the network layer, the buffer's reference
 *               counter is NOT incremented since the network loopback interface receive does NOT
 *               maintain a reference to the buffer.
 *
 * @note     (4) Network buffer already freed by higher layer; only increment error counter.
 *******************************************************************************************************/
NET_BUF_SIZE NetIF_Loopback_Rx(NET_IF   *p_if,
                               RTOS_ERR *p_err)
{
  NET_BUF      *p_buf = DEF_NULL;
  NET_BUF_HDR  *p_buf_hdr = DEF_NULL;
  NET_BUF_SIZE size = 0u;

  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warning.

  //                                                               ---------------- UPDATE LINK STATUS ----------------
  p_if->Link = NET_IF_LINK_UP;                                  // See Note #2.

  //                                                               -------------------- GET RX PKT --------------------
  p_buf = NetIF_Loopback_RxQ_Get();                             // Get pkt from loopback rx Q.
  if (p_buf == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    goto exit_discard;
  }

  NetStat_CtrDec(&NetIF_Loopback_RxQ_PktCtr);                   // Dec loopback IF's nbr q'd rx pkts avail.

  NET_CTR_STAT_INC(Net_StatCtrs.IFs.Loopback.RxPktCtr);

  p_buf_hdr = &p_buf->Hdr;

  //                                                               ------------------- DEMUX RX PKT -------------------
  size = p_buf_hdr->TotLen;                                     // Rtn pkt tot len/size.
                                                                // See Note #3.
  NetIF_Loopback_RxPktDemux(p_buf, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               ----------------- UPDATE RX STATS ------------------
  NET_CTR_STAT_INC(Net_StatCtrs.IFs.Loopback.RxPktCompCtr);

  goto exit;

exit_discard:
  NET_CTR_ERR_INC(Net_ErrCtrs.IFs.Loopback.RxPktDisCtr);

exit:
  return (size);
}

/****************************************************************************************************//**
 *                                           NetIF_Loopback_Tx()
 *
 * @brief    (1) Transmit packets to the network loopback interface :
 *               - (a) Validate loopback transmit packet
 *               - (b) Get      loopback receive  buffer
 *               - (c) Copy     loopback transmit packet to loopback receive buffer
 *               - (d) Post     loopback receive  packet to loopback receive queue
 *               - (e) Signal   network interface receive task
 *               - (f) Free     loopback transmit packet                                   See Note #4
 *               - (g) Update   loopback transmit statistics
 *
 * @param    p_if        Pointer to network loopback interface.
 *
 * @param    p_buf_tx    Pointer to network buffer to transmit.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Size of transmitted packet, if NO error(s).
 *           0, otherwise.
 *
 * @note     (2) Loopback buffer flag value to clear was previously initialized in NetBuf_Get() when
 *               the buffer was allocated.  This buffer flag value does NOT need to be re-cleared but
 *               is shown for completeness.
 *
 * @note     (3) On ANY error(s), network resources MUST be appropriately freed :
 *               - (a) For loopback receive buffers NOT yet posted to the network loopback interface
 *                     receive queue, the buffer MUST be freed by NetBuf_Free().
 *               - (b) Loopback receive buffer data areas that have been linked to loopback receive
 *                     buffers are inherently freed    by NetBuf_Free().
 *               - (c) Loopback receive buffers that have been queued to the loopback receive queue
 *                     are inherently unlinked by NetBuf_Free().
 *
 * @note     (4) Since loopback transmit packets are NOT asynchronously transmitted from network
 *               devices, they do NOT need to be asynchronously deallocated by the network interface
 *               transmit deallocation task (see 'net_if.c  NetIF_TxDeallocTaskHandler()  Note #1a').
 *******************************************************************************************************/
NET_BUF_SIZE NetIF_Loopback_Tx(NET_IF   *p_if,
                               NET_BUF  *p_buf_tx,
                               RTOS_ERR *p_err)
{
  NET_BUF      *p_buf_rx;
  NET_BUF_HDR  *p_buf_hdr_rx;
  NET_BUF_HDR  *p_buf_hdr_tx;
  NET_BUF_SIZE buf_data_ix_tx;
  NET_BUF_SIZE buf_data_ix_rx;
  NET_BUF_SIZE buf_data_ix_rx_offset;
  NET_BUF_SIZE buf_data_size_rx;
  NET_BUF_SIZE buf_data_len_tx = 0u;

  p_buf_hdr_tx = &p_buf_tx->Hdr;

  //                                                               ----------- GET LOOPBACK RX BUF ------------
  //                                                               Get rx buf.
  buf_data_len_tx = p_buf_hdr_tx->TotLen;
  buf_data_ix_rx = NET_BUF_DATA_IX_RX;
  p_buf_rx = NetBuf_Get(NET_IF_NBR_LOOPBACK,
                        NET_TRANSACTION_RX,
                        buf_data_len_tx,
                        buf_data_ix_rx,
                        &buf_data_ix_rx_offset,
                        NET_BUF_FLAG_NONE,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_discard;
  }

  //                                                               Get rx buf data area.
  p_buf_rx->DataPtr = NetBuf_GetDataPtr(p_if,
                                        NET_TRANSACTION_RX,
                                        buf_data_len_tx,
                                        buf_data_ix_rx,
                                        &buf_data_ix_rx_offset,
                                        &buf_data_size_rx,
                                        0,
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NetBuf_Free(p_buf_rx);                                              // See Note #3a.
    goto exit_discard;
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (buf_data_size_rx < buf_data_len_tx) {
    NetBuf_Free(p_buf_rx);                                              // See Note #3b.
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    goto exit_discard;
  }
#else
  PP_UNUSED_PARAM(buf_data_size_rx);                                    // Prevent 'variable unused' compiler warning.
#endif

  buf_data_ix_rx += buf_data_ix_rx_offset;

  //                                                               ------ COPY TX PKT TO LOOPBACK RX BUF ------
  //                                                               Cfg rx loopback buf ctrls.
  p_buf_hdr_rx = &p_buf_rx->Hdr;
  p_buf_hdr_rx->TotLen = buf_data_len_tx;
  p_buf_hdr_rx->DataLen = buf_data_len_tx;
  p_buf_hdr_rx->ProtocolHdrType = p_buf_hdr_tx->ProtocolHdrType;
#if 0                                                                   // Init'd in NetBuf_Get() [see Note #2].
  DEF_BIT_CLR(p_buf_hdr_rx->Flags, NET_BUF_FLAG_RX_REMOTE);
#endif

  switch (p_buf_hdr_tx->ProtocolHdrType) {
    case NET_PROTOCOL_TYPE_IP_V4:
      p_buf_hdr_rx->IP_HdrIx = (CPU_INT16U)buf_data_ix_rx;
      buf_data_ix_tx = (NET_BUF_SIZE)p_buf_hdr_tx->IP_HdrIx;
      break;

    //                                                             TODO_NET IPv6

    case NET_PROTOCOL_TYPE_NONE:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IF[NET_IF_NBR_LOOPBACK].TxInvProtocolCtr);
      NetBuf_Free(p_buf_rx);                                            // See Note #3b.
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      goto exit_discard;
  }

  //                                                               Copy tx loopback pkt into rx loopback buf.
  NetBuf_DataCopy(p_buf_rx,
                  p_buf_tx,
                  buf_data_ix_rx,
                  buf_data_ix_tx,
                  buf_data_len_tx,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NetBuf_Free(p_buf_rx);                                              // See Note #3b.
    goto exit_discard;
  }

  //                                                               ------- POST RX PKT TO LOOPBACK RX Q -------
  NetIF_Loopback_RxQ_Add(p_buf_rx);                                     // Post pkt to loopback rx q.
  NetStat_CtrInc(&NetIF_Loopback_RxQ_PktCtr);                           // Inc loopback IF's nbr q'd rx pkts avail.

  //                                                               ------------ SIGNAL IF RX TASK -------------
  NetIF_RxQPost(NET_IF_NBR_LOOPBACK, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NetBuf_Free(p_buf_rx);                                              // See Note #3c.
    goto exit_discard;
  }

  //                                                               ------ FREE TX PKT / UPDATE TX STATS -------
  NetIF_Loopback_TxPktFree(p_buf_tx);                                   // See Note #4.

  NET_CTR_STAT_INC(Net_StatCtrs.IFs.Loopback.TxPktCtr);

  goto exit;

exit_discard:
  NetIF_Loopback_TxPktDiscard(p_buf_tx);

exit:
  return (buf_data_len_tx);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NetIF_Loopback_RxQ_Add()
 *
 * @brief    Add a network packet into the Network Loopback Interface Receive Queue.
 *
 *       - (1) Network packets that have been received via the Network Loopback Interface are queued
 *               to await processing by the Network Interface Receive Task handler (see 'net_if.c
 *               NetIF_RxTaskHandler()  Note #1').
 *
 *           - (a) Received network packet buffers are linked to form a Network Loopback Interface
 *                   Receive Queue.
 *
 *                   In the diagram below, ... :
 *
 *               - (1) The horizontal row represents the list of received network packet buffers.
 *
 *               - (2) (A) 'NetIF_Loopback_RxQ_Head' points to the head of the Network Loopback Interface
 *                               Receive Queue;
 *                   - (B) 'NetIF_Loopback_RxQ_Tail' points to the tail of the Network Loopback Interface
 *                               Receive Queue.
 *
 *               - (3) Network buffers' 'PrevSecListPtr' & 'NextSecListPtr' doubly-link each network
 *                       packet  buffer to form the Network Loopback Interface Receive Queue.
 *
 *           - (b) The Network Loopback Interface Receive Queue is a FIFO Q :
 *
 *               - (1) Network packet buffers are added at     the tail of the Network Loopback Interface
 *                           Receive Queue;
 *               - (2) Network packet buffers are removed from the head of the Network Loopback Interface
 *                           Receive Queue.
 *
 *                                   |                                               |
 *                                   |<- Network Loopback Interface Receive Queue -->|
 *                                   |                (see Note #1)                  |
 *
 *                           Packets removed from                        Packets added at
 *                           Receive Queue head                        Receive Queue tail
 *                               (see Note #1b2)                           (see Note #1b1)
 *
 *                                     |              NextSecListPtr             |
 *                                     |             (see Note #1a3)             |
 *                                     v                    |                    v
 *                                                          |
 *                   Head of        -------       -------   v   -------       -------  (see Note #1a2B)
 *                   Receive   ---->|     |------>|     |------>|     |------>|     |
 *                    Queue         |     |       |     |       |     |       |     |       Tail of
 *                                  |     |<------|     |<------|     |<------|     |<----  Receive
 *              (see Note #1a2A)    |     |       |     |   ^   |     |       |     |        Queue
 *                                  |     |       |     |   |   |     |       |     |
 *                                  -------       -------   |   -------       -------
 *                                                          |
 *                                                   PrevSecListPtr
 *                                                   (see Note #1a3)
 *
 * @param    p_buf   Pointer to a network buffer.
 *
 * @note     (2) Some buffer controls were previously initialized in NetBuf_Get() when the buffer was
 *                   allocated.  These buffer controls do NOT need to be re-initialized but are shown for
 *                   completeness.
 *******************************************************************************************************/
static void NetIF_Loopback_RxQ_Add(NET_BUF *p_buf)
{
  NET_BUF_HDR *p_buf_hdr;
  NET_BUF_HDR *p_buf_hdr_tail;

  //                                                               ----------------- CFG NET BUF PTRS -----------------
  p_buf_hdr = &p_buf->Hdr;
  p_buf_hdr->PrevSecListPtr = NetIF_Loopback_RxQ_Tail;
  p_buf_hdr->NextSecListPtr = DEF_NULL;
  //                                                               Cfg buf's unlink fnct/obj to loopback rx Q.
  p_buf_hdr->UnlinkFnctPtr = &NetIF_Loopback_RxQ_Unlink;
#if 0                                                           // Init'd in NetBuf_Get() [see Note #2].
  p_buf_hdr->UnlinkObjPtr = DEF_NULL;
#endif

  //                                                               ---------- ADD PKT BUF INTO LOOPBACK RX Q ----------
  if (NetIF_Loopback_RxQ_Tail != DEF_NULL) {                    // If Q NOT empty, add after tail.
    p_buf_hdr_tail = &NetIF_Loopback_RxQ_Tail->Hdr;
    p_buf_hdr_tail->NextSecListPtr = p_buf;
  } else {                                                      // Else add first pkt buf into Q.
    NetIF_Loopback_RxQ_Head = p_buf;
  }

  NetIF_Loopback_RxQ_Tail = p_buf;                              // Add pkt buf @ Q tail (see Note #1b1).
}

/****************************************************************************************************//**
 *                                           NetIF_Loopback_RxQ_Get()
 *
 * @brief    Get a network packet buffer from the Network Loopback Interface Receive Queue.
 *
 * @return   Pointer to received network packet buffer, if available.
 *           Pointer to NULL, otherwise.
 *******************************************************************************************************/
static NET_BUF *NetIF_Loopback_RxQ_Get(void)
{
  NET_BUF *p_buf = DEF_NULL;

  //                                                               ---------- GET PKT BUF FROM LOOPBACK RX Q ----------
  p_buf = NetIF_Loopback_RxQ_Head;
  if (p_buf == DEF_NULL) {
    goto exit;
  }
  //                                                               -------- REMOVE PKT BUF FROM LOOPBACK RX Q ---------
  NetIF_Loopback_RxQ_Remove(p_buf);

exit:
  return (p_buf);
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_RxQ_Remove()
 *
 * @brief    Remove a network packet buffer from the Network Loopback Interface Receive Queue.
 *
 * @param    p_buf   Pointer to a network buffer.
 *******************************************************************************************************/
static void NetIF_Loopback_RxQ_Remove(NET_BUF *p_buf)
{
  NET_BUF     *p_buf_list_prev;
  NET_BUF     *p_buf_list_next;
  NET_BUF_HDR *p_buf_hdr;
  NET_BUF_HDR *p_buf_list_prev_hdr;
  NET_BUF_HDR *p_buf_list_next_hdr;

  //                                                               -------- REMOVE PKT BUF FROM LOOPBACK RX Q ---------
  p_buf_hdr = &p_buf->Hdr;
  p_buf_list_prev = p_buf_hdr->PrevSecListPtr;
  p_buf_list_next = p_buf_hdr->NextSecListPtr;

  //                                                               Point prev pkt buf to next pkt buf.
  if (p_buf_list_prev != DEF_NULL) {
    p_buf_list_prev_hdr = &p_buf_list_prev->Hdr;
    p_buf_list_prev_hdr->NextSecListPtr = p_buf_list_next;
  } else {
    NetIF_Loopback_RxQ_Head = p_buf_list_next;
  }
  //                                                               Point next pkt buf to prev pkt buf.
  if (p_buf_list_next != DEF_NULL) {
    p_buf_list_next_hdr = &p_buf_list_next->Hdr;
    p_buf_list_next_hdr->PrevSecListPtr = p_buf_list_prev;
  } else {
    NetIF_Loopback_RxQ_Tail = p_buf_list_prev;
  }

  //                                                               ----------------- CLR NET BUF PTRS -----------------
  p_buf_hdr->PrevSecListPtr = DEF_NULL;                         // Clr buf sec list ptrs.
  p_buf_hdr->NextSecListPtr = DEF_NULL;

  p_buf_hdr->UnlinkFnctPtr = DEF_NULL;                          // Clr unlink ptrs.
  p_buf_hdr->UnlinkObjPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_RxQ_Unlink()
 *
 * @brief    Unlink a network packet buffer from the Network Loopback Interface Receive Queue.
 *
 * @param    p_buf   Pointer to network buffer enqueued on Network Loopback Interface Receive Queue.
 *******************************************************************************************************/
static void NetIF_Loopback_RxQ_Unlink(NET_BUF *p_buf)
{
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  CPU_BOOLEAN used;
#endif

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  //                                                               ------------------- VALIDATE BUF -------------------
  used = NetBuf_IsUsed(p_buf);
  if (used != DEF_YES) {
    return;
  }
#endif

  //                                                               ---------- UNLINK BUF FROM LOOPBACK RX Q -----------
  NetIF_Loopback_RxQ_Remove(p_buf);
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_RxPktDemux()
 *
 * @brief    Demultiplex received loopback packet to appropriate network protocol.
 *
 * @param    p_buf   Pointer to network buffer that received loopback packet.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIF_Loopback_RxPktDemux(NET_BUF  *p_buf,
                                      RTOS_ERR *p_err)
{
  NET_BUF_HDR *p_buf_hdr = &p_buf->Hdr;

  switch (p_buf_hdr->ProtocolHdrType) {                         // Demux buf to appropriate protocol.
#ifdef  NET_IPv4_MODULE_EN
    case NET_PROTOCOL_TYPE_IP_V4:
      NetIPv4_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_PROTOCOL_TYPE_IP_V6:
      NetIPv6_Rx(p_buf, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;
#endif

    case NET_PROTOCOL_TYPE_NONE:
    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.IFs.IF[NET_IF_NBR_LOOPBACK].RxInvProtocolCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_TxPktFree()
 *
 * @brief    Free network buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *******************************************************************************************************/
static void NetIF_Loopback_TxPktFree(NET_BUF *p_buf)
{
  (void)NetBuf_FreeBuf(p_buf, DEF_NULL);
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_TxPktDiscard()
 *
 * @brief    On any loopback transmit packet error(s), discard packet & buffer.
 *
 * @param    p_buf   Pointer to network buffer.
 *******************************************************************************************************/
static void NetIF_Loopback_TxPktDiscard(NET_BUF *p_buf)
{
  NET_CTR *p_ctr;

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  p_ctr = &Net_ErrCtrs.IFs.Loopback.TxPktDisCtr;
#else
  p_ctr = DEF_NULL;
#endif

  (void)NetBuf_FreeBuf(p_buf, p_ctr);
}

/****************************************************************************************************//**
 *                                           NetIF_Loopback_IF_Add()
 *
 * @brief    (1) Add & initialize the Network Loopback Interface :
 *
 *           - (a) Initialize Loopback MTU
 *           - (b) Configure  Loopback interface
 *
 * @param    p_if    Pointer to network loopback interface.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) Upon adding the loopback interface, the highest possible MTU is configured.  If this
 *                   value needs to be changed, either prior to starting the interface or during run-time,
 *                   it may be reconfigured by calling NetIF_MTU_Set() from the application.
 *******************************************************************************************************/
static void NetIF_Loopback_IF_Add(NET_IF   *p_if,
                                  RTOS_ERR *p_err)
{
  NET_IF_CFG_LOOPBACK *pcfg_loopback;
  NET_BUF_SIZE        buf_size_max;
  NET_MTU             mtu_loopback;
  NET_MTU             mtu_loopback_dflt;

  pcfg_loopback = (NET_IF_CFG_LOOPBACK *)p_if->Dev_Cfg;

  //                                                               ------------------ CFG LOOPBACK IF -----------------
  p_if->Type = NET_IF_TYPE_LOOPBACK;                            // Set IF type to loopback.

  NetIF_BufPoolInit(p_if, p_err);                               // Init IF's buf pools.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               --------------------- INIT MTU ---------------------
  buf_size_max = DEF_MAX(pcfg_loopback->TxBufLargeSize, pcfg_loopback->TxBufSmallSize);
  mtu_loopback_dflt = NET_IF_MTU_LOOPBACK;
  mtu_loopback = DEF_MIN(mtu_loopback_dflt, buf_size_max);
  p_if->MTU = mtu_loopback;                                     // Set loopback MTU (see Note #2).

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetIF_Loopback_IF_Start()
 *
 * @brief    Start Network Loopback Interface.
 *
 * @param    p_if    Pointer to network loopback interface.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIF_Loopback_IF_Start(NET_IF   *p_if,
                                    RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);

  p_if->Link = NET_IF_LINK_UP;
}

/****************************************************************************************************//**
 *                                           NetIF_Loopback_IF_Stop()
 *
 * @brief    Stop Network Loopback Interface.
 *
 * @param    p_if    Pointer to network loopback interface.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIF_Loopback_IF_Stop(NET_IF   *p_if,
                                   RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);

  p_if->Link = NET_IF_LINK_DOWN;
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_AddrHW_Get()
 *
 * @brief    Get the loopback interface's hardware address.
 *
 * @param    p_if        Pointer to loopback interface.
 *
 * @param    p_addr_hw   Pointer to variable that will receive the hardware address.
 *
 * @param    p_addr_len  Pointer to a variable to ... :
 *
 * @param    ----------  (a) Pass the length of the address buffer pointed to by 'p_addr_hw'.
 *                           (b) (1) Return the actual size of the protocol address, if NO error(s);
 *                               (2) Return 0,                                       otherwise.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) The loopback interface is NOT linked to, associated with, or handled by any physical
 *                       network device(s) & therefore has NO physical hardware address.
 *
 *                       See also 'net_if_loopback.c  Note #1a'.
 *******************************************************************************************************/
static void NetIF_Loopback_AddrHW_Get(NET_IF     *p_if,
                                      CPU_INT08U *p_addr_hw,
                                      CPU_INT08U *p_addr_len,
                                      RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(p_addr_hw);

  *p_addr_len = 0u;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);                         // See Note #1.
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_AddrHW_Set()
 *
 * @brief    Set the loopback interface's hardware address.
 *
 * @param    p_if        Pointer to loopback interface.
 *
 * @param    p_addr_hw   Pointer to a memory that contains the hardware address (see Note #1).
 *
 * @param    addr_len    Hardware address length.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) The loopback interface is NOT linked to, associated with, or handled by any physical
 *                       network device(s) & therefore has NO physical hardware address.
 *
 *                       See also 'net_if_loopback.c  Note #1a'.
 *******************************************************************************************************/
static void NetIF_Loopback_AddrHW_Set(NET_IF     *p_if,
                                      CPU_INT08U *p_addr_hw,
                                      CPU_INT08U addr_len,
                                      RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(p_addr_hw);
  PP_UNUSED_PARAM(addr_len);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);                         // See Note #1.
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_AddrHW_IsValid()
 *
 * @brief    Validate a loopback interface hardware address.
 *
 * @param    p_if        Pointer to   loopback interface.
 *
 * @param    p_addr_hw   Pointer to a loopback interface hardware address (see Note #1).
 *
 * @return   DEF_NO, loopback hardware address NOT valid (see Note #1).
 *
 * @note     (1) The loopback interface is NOT linked to, associated with, or handled by any physical
 *               network device(s) & therefore has NO physical hardware address.
 *
 *               See also 'net_if_loopback.c  Note #1a'.
 *******************************************************************************************************/
static CPU_BOOLEAN NetIF_Loopback_AddrHW_IsValid(NET_IF     *p_if,
                                                 CPU_INT08U *p_addr_hw)
{
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(p_addr_hw);

  return (DEF_NO);                                              // See Note #1.
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_AddrMulticastAdd()
 *
 * @brief    Add a multicast address to the loopback interface.
 *
 * @param    p_if                Pointer to network loopback interface to add address.
 *
 * @param    p_addr_protocol     Pointer to a multicast protocol address to add (see Note #1).
 *
 * @param    addr_protocol_len   Length of the protocol address, in octets.
 *
 * @param    addr_protocol_type  Protocol address type.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Multicast addresses are available ONLY for configured interface(s); NOT for any
 *                               loopback interface(s).
 *
 * @note     (2) The multicast protocol address MUST be in network-order.
 *******************************************************************************************************/
static void NetIF_Loopback_AddrMulticastAdd(NET_IF            *p_if,
                                            CPU_INT08U        *p_addr_protocol,
                                            CPU_INT08U        addr_protocol_len,
                                            NET_PROTOCOL_TYPE addr_protocol_type,
                                            RTOS_ERR          *p_err)
{
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(p_addr_protocol);
  PP_UNUSED_PARAM(addr_protocol_len);
  PP_UNUSED_PARAM(addr_protocol_type);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
}

/****************************************************************************************************//**
 *                                   NetIF_Loopback_AddrMulticastRemove()
 *
 * @brief    Remove a multicast address from the loopback interface.
 *
 * @param    p_if                Pointer to network loopback interface to remove address.
 *
 * @param    p_addr_protocol     Pointer to a multicast protocol address to remove (see Note #1).
 *
 * @param    addr_protocol_len   Length of the protocol address, in octets.
 *
 * @param    addr_protocol_type  Protocol address type.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Multicast addresses are available ONLY for configured interface(s); NOT for any
 *                               loopback interface(s).
 *
 * @note     (2) The multicast protocol address MUST be in network-order.
 *******************************************************************************************************/
static void NetIF_Loopback_AddrMulticastRemove(NET_IF            *p_if,
                                               CPU_INT08U        *p_addr_protocol,
                                               CPU_INT08U        addr_protocol_len,
                                               NET_PROTOCOL_TYPE addr_protocol_type,
                                               RTOS_ERR          *p_err)
{
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(p_addr_protocol);
  PP_UNUSED_PARAM(addr_protocol_len);
  PP_UNUSED_PARAM(addr_protocol_type);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
}

/****************************************************************************************************//**
 *                               NetIF_Loopback_AddrMulticastProtocolToHW()
 *
 * @brief    Convert a multicast protocol address into a loopback interface address.
 *
 * @param    p_if                Pointer to network loopback interface to transmit the packet.
 *
 * @param    p_addr_protocol     Pointer to a multicast protocol address to convert,
 *
 * @param    ---------------     in network-order.
 *
 * @param    addr_protocol_len   Length of the protocol address, in octets.
 *
 * @param    addr_protocol_type  Protocol address type.
 *
 * @param    p_addr_hw           Pointer to a variable that will receive the hardware address
 *
 * @param    ---------           ------             in network-order.
 *
 * @param    p_addr_hw_len       Pointer to a variable to ... :
 *
 * @param    -------------       --             in network-order.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Multicast addresses are available ONLY for configured interface(s); NOT for any
 *                               loopback interface(s).
 *
 * @note     (2) (a) The multicast protocol address MUST be in network-order.
 *
 *                                   - (b) The loopback hardware address is returned in network-order; i.e. the pointer to
 *                               the hardware address points to the highest-order octet.
 *
 * @note     (3) Since 'p_addr_hw_len' argument is both an input & output argument (see 'Argument(s) :
 *                               p_addr_hw_len'), ... :
 *
 *                                   - (a) Its input value SHOULD be validated prior to use; ...
 *                                   - (b) While its output value MUST be initially configured to return a default value
 *                               PRIOR to all other validation or function handling in case of any error(s).
 *******************************************************************************************************/
static void NetIF_Loopback_AddrMulticastProtocolToHW(NET_IF            *p_if,
                                                     CPU_INT08U        *p_addr_protocol,
                                                     CPU_INT08U        addr_protocol_len,
                                                     NET_PROTOCOL_TYPE addr_protocol_type,
                                                     CPU_INT08U        *p_addr_hw,
                                                     CPU_INT08U        *p_addr_hw_len,
                                                     RTOS_ERR          *p_err)
{
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(p_addr_protocol);
  PP_UNUSED_PARAM(addr_protocol_len);
  PP_UNUSED_PARAM(addr_protocol_type);
  PP_UNUSED_PARAM(p_addr_hw);

  *p_addr_hw_len = 0u;                                           // Cfg dflt addr len for err (see Note #3b).

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
}

/****************************************************************************************************//**
 *                                   NetIF_Loopback_BufPoolCfgValidate()
 *
 * @brief    Validate loopback interface network buffer pool configuration.
 *
 * @param    p_if    Pointer to loopback interface.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIF_Loopback_BufPoolCfgValidate(NET_IF   *p_if,
                                              RTOS_ERR *p_err)
{
  //                                                               Prevent 'variable unused' compiler warning.
  PP_UNUSED_PARAM(p_if);
  PP_UNUSED_PARAM(p_err);
}

/****************************************************************************************************//**
 *                                           NetIF_Loopback_MTU_Set()
 *
 * @brief    Set the loopback interface's MTU.
 *
 * @param    p_if    Pointer to loopback interface.
 *
 * @param    mtu     Desired maximum transmission unit (MTU) size to configure (in octets).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIF_Loopback_MTU_Set(NET_IF   *p_if,
                                   NET_MTU  mtu,
                                   RTOS_ERR *p_err)
{
  NET_IF_CFG_LOOPBACK *pcfg_loopback;
  NET_BUF_SIZE        buf_size_max;
  NET_MTU             mtu_max;

  pcfg_loopback = (NET_IF_CFG_LOOPBACK *)p_if->Dev_Cfg;
  buf_size_max = DEF_MAX(pcfg_loopback->TxBufLargeSize, pcfg_loopback->TxBufSmallSize);
  mtu_max = DEF_MIN(mtu, buf_size_max);

  RTOS_ASSERT_DBG_ERR_SET((mtu <= mtu_max), *p_err, RTOS_ERR_INVALID_CFG,; );

  p_if->MTU = mtu;
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_GetPktSizeHdr()
 *
 * @brief    Get loopback interface header size.
 *
 * @param    p_if    Pointer to loopback interface.
 *
 * @return   The loopback interface header size.
 *******************************************************************************************************/
static CPU_INT16U NetIF_Loopback_GetPktSizeHdr(NET_IF *p_if)
{
  CPU_INT16U pkt_size;

  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warning.
  pkt_size = (CPU_INT16U)NET_IF_HDR_SIZE_LOOPBACK;

  return (pkt_size);
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_GetPktSizeMin()
 *
 * @brief    Get maximum allowable loopback interface packet size.
 *
 * @param    p_if    Pointer to loopback interface.
 *
 * @return   The maximum allowable loopback interface packet size.
 *******************************************************************************************************/
static CPU_INT16U NetIF_Loopback_GetPktSizeMin(NET_IF *p_if)
{
  CPU_INT16U pkt_size;

  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warning.
  pkt_size = (CPU_INT16U)NET_IF_LOOPBACK_SIZE_MIN;

  return (pkt_size);
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_GetPktSizeMin()
 *
 * @brief    Get maximum allowable loopback interface packet size.
 *
 * @param    p_if    Pointer to loopback interface.
 *
 * @return   The maximum allowable loopback interface packet size.
 *******************************************************************************************************/
static CPU_INT16U NetIF_Loopback_GetPktSizeMax(NET_IF *p_if)
{
  CPU_INT16U  pkt_size;
  NET_DEV_CFG *p_cfg = (NET_DEV_CFG *)p_if->Dev_Cfg;

  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warning.
  pkt_size = (CPU_INT16U)p_cfg->RxBufLargeSize;

  return (pkt_size);
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_ISR_Handler()
 *
 * @brief    Handle loopback interface interrupt service routine (ISR) function(s).
 *
 *       - (1) Interrupt service routines are available ONLY for configured interface(s)/device(s);
 *               NOT for any loopback interface(s).
 *
 * @param    p_if    Pointer to network loopback interface.
 *
 * @param    type    Device interrupt type(s) to handle.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetIF_Loopback_ISR_Handler(NET_IF           *p_if,
                                       NET_DEV_ISR_TYPE type,
                                       RTOS_ERR         *p_err)
{
  PP_UNUSED_PARAM(p_if);                                        // Prevent 'variable unused' compiler warnings.
  PP_UNUSED_PARAM(type);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
}

/****************************************************************************************************//**
 *                                       NetIF_Loopback_IO_CtrlHandler()
 *
 * @brief    Handle loopback interface specific control(s).
 *
 * @param    p_if    Pointer to loopback interface.
 *
 * @param    opt     Desired I/O control option code to perform :
 *                   NET_IF_IO_CTRL_LINK_STATE_GET           Get loopback interface's link state,
 *                   'UP' or 'DOWN'.
 *
 * @param    p_data  Pointer to variable that will receive possible I/O control data (see Note #1).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) 'p_data' MUST point to a variable (or memory) that is sufficiently sized AND aligned
 *                   to receive any return data.
 *******************************************************************************************************/
static void NetIF_Loopback_IO_CtrlHandler(NET_IF     *p_if,
                                          CPU_INT08U opt,
                                          void       *p_data,
                                          RTOS_ERR   *p_err)
{
  CPU_BOOLEAN *p_link_state;

  switch (opt) {
    case NET_IF_IO_CTRL_LINK_STATE_GET:
      RTOS_ASSERT_DBG_ERR_SET((p_data != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

      p_link_state = (CPU_BOOLEAN *)p_data;                     // See Note #1.
      *p_link_state = (CPU_BOOLEAN)p_if->Link;
      break;

    case NET_IF_IO_CTRL_NONE:
    case NET_IF_IO_CTRL_LINK_STATE_GET_INFO:
    case NET_IF_IO_CTRL_LINK_STATE_UPDATE:
    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_ARG,; );
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IF_LOOPBACK_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
