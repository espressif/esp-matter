/***************************************************************************//**
 * @file
 * @brief Network DHCP Module
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

#ifndef _DHCP_PRIV_H_
#define _DHCP_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/dhcp.h"

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_path.h>
#include  <net_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                        DHCP MESSAGE DEFINES
 *
 * Note(s) : (1) RFC #2131, section 'Protocol Summary' states that "a DHCP client must be prepared to
 *               receive a message of up to 576 octets".  Hense, the size of the DHCP message buffer is
 *               fixed to this length.
 *
 *           (2) (a) RFC #2131, section 'Introduction' states that "the format of DHCP messages is based
 *                   on the format of BOOTP messages, to capture the BOOTP relay agent behavior described
 *                   as part of the BOOTP specification, and to allow interoperability of existing BOOTP
 *                   clients with DHCP servers".
 *
 *                   Although not explicitly stated, DHCP messages should be at least 300 octets to
 *                   preserve backward compatibility with BOOTP which does define this minimum size (some
 *                   BOOTP relay agents have been known to drop packets of less than this length).
 *
 *               (b) However, NO control may be exercised on received DHCP message, and in order to remain
 *                   compatible with uncomplying DHCP server, smaller incoming DHCP message are NOT
 *                   discarded, provided they are at least 244 bytes (the minimum DHCP message length
 *                   including mandatory "DHCP message type" option).
 *
 *               (c) Thus :
 *
 *                   (1) Transmitted packets will be padded so their length is at least 300 octets.
 *
 *                   (2) Received    packets under 244 octets will be discarded.
 *
 *           (3) Section 'The Client-Server Protocol' of this RFC stipulates that "the first four octets
 *               of the 'options' field of the DHCP message contain the (decimal) values 99, 130, 83 and
 *               99, respectively (this is the same magic cookie as is defined in RFC 1497)".
 *
 *           (4) See also 'DHCP MESSAGE DATA TYPE  Note #2'.
 *
 *           (5) DHCP operation codes are defined in RFC #2131, section 'Protocol Summary'.
 *******************************************************************************************************/

#define  DHCP_MSG_BUF_SIZE                            576   // Buf size                          (see Note #1).

#define  DHCP_MSG_TX_MIN_LEN                          300   // Min tx'd msg len                  (see Note #2).
#define  DHCP_MSG_RX_MIN_LEN                          244   // Min rx'd msg len                  (see Note #2).

#define  DHCP_MAGIC_COOKIE                     0x63825363   // Magic cookie                      (see Note #3).
#define  DHCP_MAGIC_COOKIE_SIZE                         4   // Size of magic cookie (in octets).

#define  DHCP_FLAG_BROADCAST                     (1 << 15)  // Broadcast flag                    (see Note #4).

//                                                                 Operation codes                   (see Note #5).
#define  DHCP_OP_REQUEST                                1
#define  DHCP_OP_REPLY                                  2

#define  DHCP_HTYPE_ETHER                               1   // Ethernet hardware address type.

/********************************************************************************************************
 *                                 DHCP INTERFACE LEASE STATUS DEFINES
 *******************************************************************************************************/

#define  DHCP_STATUS_NONE                                  0

#define  DHCP_STATUS_CFG_IN_PROGRESS                       1
#define  DHCP_STATUS_CFGD                                  2
#define  DHCP_STATUS_CFGD_NO_TMR                           3
#define  DHCP_STATUS_CFGD_LOCAL_LINK                       4

#define  DHCP_STATUS_FAIL                                  5

/********************************************************************************************************
 *                                         DHCP OPTION DEFINES
 *
 * Note(s) : (1) This section defines the DHCP Options and BOOTP Vendor Extensions, as defined in
 *               RFC #2132.  This list is not necessarily exhaustive; please refer to the Internet
 *               Assigned Numbers Authority (www.iana.org) for the complete list.
 *******************************************************************************************************/

#define  DHCP_OPT_FIELD_CODE_LEN                                   1
#define  DHCP_OPT_FIELD_LEN_LEN                                    1
#define  DHCP_OPT_FIELD_HDR_LEN                     (DHCP_OPT_FIELD_CODE_LEN + DHCP_OPT_FIELD_LEN_LEN)

/********************************************************************************************************
 *                                          DHCP TIME DEFINES
 *
 * Note(s) : (1) Minimum retransmitting delay between attempt when rebinding or renewing the lease.
 *
 *               See also 'DHCPc_LeaseTimeUpdate()  Note #2'.
 *
 *           (2) Delay between retried lease negotiation.
 *
 *               See also 'DHCPc_InitStateHandler()  Note #4'.
 *
 *           (3) When a RELEASE message is transmitted, it is possible that the target has to resolve
 *               the destination hardware address through ARP.  We hence have to introduce a slight delay
 *               to allow that address to be resolved and for the RELEASE message to be sent out before
 *               the interface network layer is un-configured.
 *
 *               Not doing so would result in the message to never to transmitted since an un-configured
 *               interface does NOT resolve pending ARP caches.
 *******************************************************************************************************/

#define  DHCP_LEASE_INFINITE                       0xFFFFFFFF   // Permanent lease time.

#define  DHCP_T1_LEASE_NUMERATOR                        1       // 0.5 fraction.
#define  DHCP_T1_LEASE_DENOMINATOR                      2

#define  DHCP_T2_LEASE_NUMERATOR                        7       // 0.875 fraction.
#define  DHCP_T2_LEASE_DENOMINATOR                      8

#define  DHCP_T1_LEASE_FRACTION                        DHCP_T1_LEASE_NUMERATOR / DHCP_T1_LEASE_DENOMINATOR
#define  DHCP_T2_LEASE_FRACTION                        DHCP_T2_LEASE_NUMERATOR / DHCP_T2_LEASE_DENOMINATOR

#define  DHCP_MIN_RETX_TIME_S                           300     // Min re-tx time (see Note #1).

#define  DHCP_INIT_DLY_MS                             10000     // Init       dly (see Note #2).

//                                                                 Release    dly (see Note #3).
#define  DHCP_RELEASE_DLY_S                         NET_ARP_REQ_RETRY_TIMEOUT_DFLT_SEC

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                      DHCP MESSAGE TYPE DATA TYPE
 *******************************************************************************************************/

typedef  enum  dhcp_msg_type {
  DHCP_MSG_NONE = 0,
  DHCP_MSG_DISCOVER = 1,
  DHCP_MSG_OFFER = 2,
  DHCP_MSG_REQUEST = 3,
  DHCP_MSG_DECLINE = 4,
  DHCP_MSG_ACK = 5,
  DHCP_MSG_NAK = 6,
  DHCP_MSG_RELEASE = 7,
  DHCP_MSG_INFORM = 8
} DHCP_MSG_TYPE;

/********************************************************************************************************
 *                                       DHCP MESSAGE DATA TYPE
 *
 * Note(s) : (1) DHCP message format is defined in RFC #2131, section 2 'Protocol Summary'.
 *
 *           (2) RFC #2131, section 2 'Protocol Summary' states that "DHCP servers [...] may not be able
 *               to deliver DHCP messages to clients that cannot accept hardware unicast datagrams before
 *               the TCP/IP software is configured."
 *
 *              "To work around [that], DHCP uses the 'flags' field.  The leftmost bit is defined as the
 *               BRAODCAST (B) flag. [...] The remaining bits of the flags field are reserved for future
 *               use [and] they MUST be set to zero by clients."
 *
 *                   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
 *                   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *                   |B|             MBZ             |
 *                   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *                   B:  BROADCAST flag
 *
 *                   MBZ:  MUST BE ZERO (reserved for future use)
 *
 *           (3) The DHCP message 'header' is immediately followed by the variable length 'option' field.
 *******************************************************************************************************/

//                                                                 ------------------- DHCP MSG HDR -------------------
typedef  struct dhcp_msg_hdr {
  CPU_INT08U Op;                                                // Op code.
  CPU_INT08U Htype;                                             // HW addr type.
  CPU_INT08U Hlen;                                              // HW addr len.
  CPU_INT08U Hops;
  CPU_INT32U Xid;                                               // Transaction ID.
  CPU_INT16U Secs;                                              // Sec elapsed since beginning of addr acquisition.
  CPU_INT16U Flags;                                             // See Note #2.
  CPU_INT32U Ciaddr;                                            // Client IP addr set   by client.
  CPU_INT32U Yiaddr;                                            // Client IP addr rtn'd by server.
  CPU_INT32U Siaddr;
  CPU_INT32U Giaddr;
  CPU_INT08U Chaddr[16];                                        // Client HW addr.
  CPU_INT08U Sname[64];
  CPU_INT08U File[128];
  //                                                               DHCP Opt's (see Note #3).
} DHCP_MSG_HDR;

#define  DHCP_MSG_HDR_SIZE                    (sizeof(DHCP_MSG_HDR))
#define  NETDHCPc_MSG_OPT_SIZE  (DHCP_PKT_BUF_SIZE - DHCP_MSG_HDR_SIZE)

/********************************************************************************************************
 *                                        DHCP MESSAGE DATA TYPE
 *******************************************************************************************************/

typedef  struct  dhcp_msg DHCP_MSG;

struct dhcp_msg {
  CPU_INT32U    TransactionID;                                  // Msg id.
  DHCP_MSG_TYPE Type;
  CPU_INT08U    Buf[DHCP_MSG_BUF_SIZE];                         // Buf for DHCP msg.
  CPU_INT16U    Len;                                            // Tot len of msg (hdr + opt).
  CPU_INT16U    Flags;                                          // Msg flags.
};

/********************************************************************************************************
 *                                        DHCP DICT DATA TYPE
 *******************************************************************************************************/

typedef  struct  dhcp_opt_dict {
  const DHCP_OPT_CODE Code;
  const CPU_INT32U    OptLenMin;
} DHCP_OPT_DICT;

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _DHCP_PRIV_H_
