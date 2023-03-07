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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_IPv4_PRIV_H_
#define  _NET_IPv4_PRIV_H_

#include  "../../include/net_cfg_net.h"

#ifdef   NET_IPv4_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_ip.h>
#include  <net/include/net_ipv4.h>

#include  "net_ip_priv.h"
#include  "net_type_priv.h"
#include  "net_tmr_priv.h"

#include  <common/source/collections/slist_priv.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           IPv4 HEADER DEFINES
 *******************************************************************************************************/

#define  NET_IPv4_HDR_SIZE_TOT_MIN                      (NET_IF_HDR_SIZE_TOT_MIN + NET_IPv4_HDR_SIZE_MIN)
#define  NET_IPv4_HDR_SIZE_TOT_MAX                      (NET_IF_HDR_SIZE_TOT_MAX + NET_IPv4_HDR_SIZE_MAX)

#define  NET_IPv4_HDR_SIZE                                20u

#define  NET_IPv4_ID_NONE                                  0u

typedef  CPU_INT32U NET_IPv4_OPT_SIZE;                          // IPv4 opt size data type (see Note #4).

#define  NET_IPv4_HDR_OPT_SIZE_WORD              (sizeof(NET_IPv4_OPT_SIZE))
#define  NET_IPv4_HDR_OPT_SIZE_MAX                      (NET_IPv4_HDR_SIZE_MAX - NET_IPv4_HDR_SIZE_MIN)

#define  NET_IPv4_HDR_OPT_NBR_MIN                          0
#define  NET_IPv4_HDR_OPT_NBR_MAX                       (NET_IPv4_HDR_OPT_SIZE_MAX / NET_IPv4_HDR_OPT_SIZE_WORD)

#define  NET_IPv4_OPT_PARAM_NBR_MAX                        9    // Max nbr of 'max nbr opts'.

/********************************************************************************************************
 *                               IPv4 HEADER TYPE OF SERVICE (TOS) DEFINES
 *
 * Note(s) : (1) (a) See 'IPv4 HEADER  Note #3'                   for TOS            fields.
 *
 *               (b) See RFC # 791, Section 3.1 'Type of Service' for TOS Precedence values.
 *
 *               (c) See RFC #1349, Section 4                     for TOS            values.
 *
 *           (2) RFC #1122, Section 3.2.1.6 states that "the default ... TOS field ... is all zero bits."
 *******************************************************************************************************/

#define  NET_IPv4_HDR_TOS_PRECEDNCE_ROUTINE             0x00u
#define  NET_IPv4_HDR_TOS_PRECEDNCE_PRIO                0x20u
#define  NET_IPv4_HDR_TOS_PRECEDNCE_IMMED               0x40u
#define  NET_IPv4_HDR_TOS_PRECEDNCE_FLASH               0x60u
#define  NET_IPv4_HDR_TOS_PRECEDNCE_FLASH_OVERRIDE      0x80u
#define  NET_IPv4_HDR_TOS_PRECEDNCE_CRITIC_ECP          0xA0u
#define  NET_IPv4_HDR_TOS_PRECEDNCE_INTERNET_CTRL       0xC0u
#define  NET_IPv4_HDR_TOS_PRECEDNCE_NET_CTRL            0xE0u

#define  NET_IPv4_HDR_TOS_PRECEDNCE_MASK                0xE0u
#define  NET_IPv4_HDR_TOS_PRECEDNCE_DFLT                 NET_IPv4_HDR_TOS_PRECEDNCE_ROUTINE

#define  NET_IPv4_HDR_TOS_NONE                          0x00u
#define  NET_IPv4_HDR_TOS_LO_DLY                        0x10u
#define  NET_IPv4_HDR_TOS_HI_THRUPUT                    0x08u
#define  NET_IPv4_HDR_TOS_HI_RELIABILITY                0x04u
#define  NET_IPv4_HDR_TOS_LO_COST                       0x02u

#define  NET_IPv4_HDR_TOS_MASK                          0x1Eu
#define  NET_IPv4_HDR_TOS_DFLT                           NET_IPv4_HDR_TOS_NONE

#define  NET_IPv4_HDR_TOS_MBZ_MASK                      0x01u
#define  NET_IPv4_HDR_TOS_MBZ_DFLT                      0x00u

#define  NET_IPv4_HDR_TOS_RESERVED                       NET_IPv4_HDR_TOS_MBZ_DFLT

//                                                                 See Note #2.
#define  NET_IPv4_TOS_DFLT                              (NET_IPv4_HDR_TOS_PRECEDNCE_DFLT \
                                                         | NET_IPv4_HDR_TOS_DFLT         \
                                                         | NET_IPv4_HDR_TOS_MBZ_DFLT)

#define  NET_IPv4_TOS_NONE                                 NET_IPv4_TOS_DFLT

/********************************************************************************************************
 *                               IPv4 HEADER TIME-TO-LIVE (TTL) DEFINES
 *
 * Note(s) : (1) RFC #1122, Section 3.2.1.7 states that :
 *
 *               (a) "A host MUST NOT send a datagram with a Time-to-Live (TTL) value of zero."
 *
 *               (b) "When a fixed TTL value is used, it MUST be configurable."
 *
 *                   NOT yet implemented. #### NET-817
 *
 *           (2) (a) RFC #1112, Section 6.1 states that "if the upper-layer protocol chooses not to
 *                   specify a time-to-live, it should default to 1 for all multicast IPv4 datagrams,
 *                   so that an explicit choice is required to multicast beyond a single network".
 *
 *               (b) RFC #1112, Appendix I, Section 'Informal Protocol Description' states that
 *                   "queries ... carry an IP time-to-live of 1" & that a "report is sent ... with
 *                   an IPv4 time-to-live of 1".
 *
 *                   Hence, every IGMP message uses a Time-to-Live (TTL) value of 1.
 *******************************************************************************************************/

#define  NET_IPv4_HDR_TTL_NONE                             0    // On IPv4 Tx, subst _DFLT for _NONE (see Note #1a).
#define  NET_IPv4_HDR_TTL_MIN                              1
#define  NET_IPv4_HDR_TTL_MAX                            255
#define  NET_IPv4_HDR_TTL_DFLT                           128    // See Note #1b.

#define  NET_IPv4_TTL_NONE                               NET_IPv4_HDR_TTL_NONE
#define  NET_IPv4_TTL_MIN                                NET_IPv4_HDR_TTL_MIN
#define  NET_IPv4_TTL_MAX                                NET_IPv4_HDR_TTL_MAX
#define  NET_IPv4_TTL_DFLT                               NET_IPv4_HDR_TTL_DFLT

#define  NET_IPv4_TTL_MULTICAST_DFLT                       1    // See Note #2a.
#define  NET_IPv4_TTL_MULTICAST_IGMP                       1    // See Note #2b.

/********************************************************************************************************
 *                                       IPv4 DATA/TOTAL LENGTH DEFINES
 *
 * Note(s) : (1) (a) IPv4 total length #define's (NET_IPv4_TOT_LEN)  relate to the total size of a complete
 *                   IPv4 datagram, including the packet's IPv4 header.  Note that a complete IPv4 datagram
 *                   MAY be fragmented in multiple IPv4 packets.
 *
 *               (b) IPv4 data  length #define's (NET_IPv4_DATA_LEN) relate to the data  size of a complete
 *                   IPv4 datagram, equal to the total IPv4 datagram length minus its IPv4 header size.  Note
 *                   that a complete IPv4 datagram MAY be fragmented in multiple IPv4 packets.
 *
 *           (2) RFC #791, Section 3.1 'Total Length' "recommend[s] that hosts only send datagrams larger
 *               than 576 octets if ... the destination is prepared to accept the larger datagrams"; while
 *               RFC #879, Section 1 requires that "HOSTS MUST NOT SEND DATAGRAMS LARGER THAN 576 OCTETS
 *               UNLESS ... THE DESTINATION HOST IS PREPARED TO ACCEPT LARGER DATAGRAMS".
 *******************************************************************************************************/

//                                                                 See Notes #1a & #1b.
#define  NET_IPv4_DATA_LEN_MIN                             0

#define  NET_IPv4_TOT_LEN_MIN                           (NET_IPv4_HDR_SIZE_MIN + NET_IPv4_DATA_LEN_MIN)
#define  NET_IPv4_TOT_LEN_MAX                            DEF_INT_16U_MAX_VAL

#define  NET_IPv4_DATA_LEN_MAX                          (NET_IPv4_TOT_LEN_MAX  - NET_IPv4_HDR_SIZE_MIN)

#define  NET_IPv4_MAX_DATAGRAM_SIZE_DFLT                 576                    // See Note #2.

/********************************************************************************************************
 *                                       IPv4 HEADER FLAG DEFINES
 *
 * Note(s) : (1) See 'IPv4 HEADER  Note #4' for flag fields.
 *******************************************************************************************************/

#define  NET_IPv4_HDR_FLAG_MASK                       0xE000u

#define  NET_IPv4_HDR_FLAG_NONE                   DEF_BIT_NONE
#define  NET_IPv4_HDR_FLAG_RESERVED               DEF_BIT_15    // MUST be '0'.
#define  NET_IPv4_HDR_FLAG_FRAG_DONT              DEF_BIT_14
#define  NET_IPv4_HDR_FLAG_FRAG_MORE              DEF_BIT_13

/********************************************************************************************************
 *                                       IPv4 FRAGMENTATION DEFINES
 *
 * Note(s) : (1) (a) (1) RFC #791, Section 3.2 'Fragmentation and Reassembly' states that :
 *
 *                       (A) "Fragments are counted in units of 8 octets."
 *                       (B) "The minimum fragment is 8 octets."
 *
 *                   (2) However, this CANNOT apply to the last fragment in a fragmented datagram :
 *
 *                       (A) Which may be of ANY size; ...
 *                       (B) But SHOULD be at least one octet.
 *
 *               (b) RFC #791, Section 3.2 'Fragmentation and Reassembly : An Example Reassembly Procedure'
 *                   states that "the current recommendation for the [IP fragmentation reassembly] timer
 *                   setting is 15 seconds ... [but] this may be changed ... with ... experience".
 *******************************************************************************************************/

#define  NET_IPv4_HDR_FRAG_OFFSET_MASK                0x1FFFu
#define  NET_IPv4_HDR_FRAG_OFFSET_NONE                     0
#define  NET_IPv4_HDR_FRAG_OFFSET_MIN                      0
#define  NET_IPv4_HDR_FRAG_OFFSET_MAX                   8191

#define  NET_IPv4_FRAG_SIZE_UNIT                           8    // Frag size unit = 8 octets (see Note #1a1).

#define  NET_IPv4_FRAG_SIZE_NONE                         DEF_INT_16U_MAX_VAL
#define  NET_IPv4_FRAG_SIZE_MIN_FRAG_MORE                NET_IPv4_FRAG_SIZE_UNIT    // See Note #1a1B.
#define  NET_IPv4_FRAG_SIZE_MIN_FRAG_LAST                  1                        // See Note #1a2B.
#define  NET_IPv4_FRAG_SIZE_MAX                       (((NET_IPv4_TOT_LEN_MAX - NET_IPv4_HDR_SIZE_MIN) / NET_IPv4_FRAG_SIZE_UNIT) \
                                                       * NET_IPv4_FRAG_SIZE_UNIT)

//                                                                 IPv4 frag reasm timeout (see Note #1b) :
#define  NET_IPv4_FRAG_REASM_TIMEOUT_MIN_SEC               1    // IPv4 frag reasm timeout min  =  1 seconds
#define  NET_IPv4_FRAG_REASM_TIMEOUT_MAX_SEC              15    // IPv4 frag reasm timeout max  = 15 seconds
#define  NET_IPv4_FRAG_REASM_TIMEOUT_DFLT_SEC              6    // IPv4 frag reasm timeout dflt =  5 seconds

/********************************************************************************************************
 *                                           IPv4 ADDRESS DEFINES
 *
 * Note(s) : (1) See the following RFC's for IPv4 address summary :
 *
 *               (a) RFC # 791, Section 3.2     'Addressing'
 *               (b) RFC # 950, Section 2.1
 *               (c) RFC #1122, Section 3.2.1.3
 *               (d) RFC #3927, Section 2.1
 *
 *           (2) (a) Supports IPv4 Class A, B, C & D Network addresses ONLY as specified by RFC #791,
 *                   Section 3.2 'Addressing : Address Format' & RFC #1112, Section 4 :
 *
 *                       Class  High Order Bits
 *                       -----  ---------------
 *                           (A)         0
 *                           (B)         10
 *                           (C)         110
 *                           (D)         1110
 *
 *               (b) (1) RFC #1519 states that "the concept of network 'class' needs to be deprecated"
 *                       (Section 4.1) in order to "support classless network destinations" which will
 *                       "permit arbitrary super/subnetting of the remaining class A and class B [and
 *                       class C] address space (the assumption being that classless ... non-contiguous
 *                       subnets ... will be contained within a single ... domain)" (Section 2.2).
 *
 *                       However, despite the aggregated, classless network address space; individual
 *                       class definitions MUST still be used to validate IPv4 addresses as within a
 *                       network address space -- versus multicast, reserved, or experimental addresses.
 *
 *                   (2) RFC #950, Section 2.1 states that "the bits that identify the subnet ... need
 *                       not be adjacent in the address.  However, we recommend that the subnet bits be
 *                       contiguous and located as the most significant bits of the local address".
 *
 *                       #### Therefore, it is assumed that at least the most significant bit of the
 *                       network portion of the subnet address SHOULD be set.
 *
 *           (3) (a) Currently supports 'This Host' initialization address ONLY :
 *
 *                   (1) This      Host                      0.0.0.0         See RFC #1122, Section 3.2.1.3.(a)
 *
 *                       Specified host initialization addresses NOT currently supported :
 *
 *                   (2) Specified Host                      0.<host>        See RFC #1122, Section 3.2.1.3.(b)
 *
 *               (b) Supports 'Localhost' loopback address :
 *
 *                   (1) Localhost                         127.<host>        See RFC #1122, Section 3.2.1.3.(g)
 *
 *                   (2) Stevens, TCP/IP Illustrated, Volume 1, 8th Printing, Section 2.7, Page 28 states
 *                       that "most implementations support a 'loopback interface' that allows a ... host
 *                       to communicate with" itself :
 *
 *                       (A) "The class A network ID 127 is reserved for the loopback interface."
 *                       (B) For "this [loopback] interface ... most systems assign" ... :
 *                           (1) "the IP address of 127.0.0.1 ... and" ...
 *                           (2) "the name 'localhost'."
 *
 *               (c) Supports auto-configured, link-local IPv4 addresses :
 *
 *                   (1) Link-local Hosts                169.254.<host>      See RFC #3927, Section 2.1
 *
 *                   (2) (A) RFC #3927, Section 2.1 specifies the "IPv4 Link-Local address ... range ...
 *                           [as] inclusive" ... :
 *
 *                           (1) "from 169.254.1.0" ...
 *                           (2) "to   169.254.254.255".
 *
 *                       (B) RFC #3927, Section 2.6.2 states that "169.254.255.255 ... is the broadcast
 *                           address for the Link-Local prefix".
 *
 *                       (C) RFC #3927, Section 2.8   states that "the 169.254/16 address prefix MUST
 *                           NOT be subnetted".
 *
 *               (d) Supports multicast host addresses :
 *
 *                   (1) RFC #1112, Section 4 specifies that "class D ... host group addresses range" :
 *
 *                       (A) "from 224.0.0.0" ...
 *                       (B) "to   239.255.255.255".
 *
 *                   (2) However, RFC #1112, Section 4 adds that :
 *
 *                       (A) "address 224.0.0.0 is guaranteed not to be assigned to any group", ...
 *                       (B) "and 224.0.0.1 is assigned to the permanent group of all IP hosts."
 *
 *               (e) Currently supports limited & directed-network broadcasts ONLY :
 *
 *                   (1) Limited          Broadcast      255.255.255.255     See RFC #1122, Section 3.2.1.3.(c)
 *                   (2) Directed-Network Broadcast        <net>.<-1>        See RFC #1122, Section 3.2.1.3.(d)
 *                   (3) Directed-Subnet  Broadcast   <net>.<subnet >.<-1>   See RFC #1122, Section 3.2.1.3.(e)
 *
 *                       Directed-subnets broadcasts NOT currently supported #### NET-801
 *
 *                   (4) Directed-Subnets Broadcast   <net>.<subnets>.<-1>   See RFC #1122, Section 3.2.1.3.(f)
 *
 *           (4) IPv4 addresses expressed in IPv4 dotted-decimal notation, ww.xx.yy.zz, #define'd as :
 *
 *                       (((NET_IPv4_ADDR) ww << (3 * DEF_OCTET_NBR_BITS)) | \
 *                       ((NET_IPv4_ADDR) xx << (2 * DEF_OCTET_NBR_BITS)) | \
 *                       ((NET_IPv4_ADDR) yy << (1 * DEF_OCTET_NBR_BITS)) | \
 *                       ((NET_IPv4_ADDR) zz << (0 * DEF_OCTET_NBR_BITS)))
 *******************************************************************************************************/

#define  NET_IPv4_ADDR_CLASS_A                    0x00000000u           // Class-A IPv4 addr (see Note #2a1A).
#define  NET_IPv4_ADDR_CLASS_A_MASK               0x80000000u
#define  NET_IPv4_ADDR_CLASS_A_MASK_NET           0xFF000000u
#define  NET_IPv4_ADDR_CLASS_A_MASK_HOST          0x00FFFFFFu

#define  NET_IPv4_ADDR_CLASS_B                    0x80000000u           // Class-B IPv4 addr (see Note #2a1B).
#define  NET_IPv4_ADDR_CLASS_B_MASK               0xC0000000u
#define  NET_IPv4_ADDR_CLASS_B_MASK_NET           0xFFFF0000u
#define  NET_IPv4_ADDR_CLASS_B_MASK_HOST          0x0000FFFFu

#define  NET_IPv4_ADDR_CLASS_C                    0xC0000000u           // Class-C IPv4 addr (see Note #2a1C).
#define  NET_IPv4_ADDR_CLASS_C_MASK               0xE0000000u
#define  NET_IPv4_ADDR_CLASS_C_MASK_NET           0xFFFFFF00u
#define  NET_IPv4_ADDR_CLASS_C_MASK_HOST          0x000000FFu

#define  NET_IPv4_ADDR_CLASS_D                    0xE0000000u           // Class-D IPv4 addr (see Note #2a1D).
#define  NET_IPv4_ADDR_CLASS_D_MASK               0xF0000000u

#define  NET_IPv4_ADDR_CLASS_SUBNET_MASK_MIN      0x80000000u           // See Note #2b2.

//                                                                 Localhost net            (see Note #3b1).
#define  NET_IPv4_ADDR_LOCAL_HOST_NET                (((NET_IPv4_ADDR)127uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)  0uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  0uL << (0u * DEF_OCTET_NBR_BITS)))

//                                                                 Localhost typical  addr  (see Note #3b2B1).
#define  NET_IPv4_ADDR_LOCAL_HOST_ADDR               (((NET_IPv4_ADDR)127uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)  0uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  1uL << (0u * DEF_OCTET_NBR_BITS)))

//                                                                 Localhost min host addr.
#define  NET_IPv4_ADDR_LOCAL_HOST_MIN                (((NET_IPv4_ADDR)127uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)  0uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  1uL << (0u * DEF_OCTET_NBR_BITS)))
//                                                                 Localhost max host addr.
#define  NET_IPv4_ADDR_LOCAL_HOST_MAX                (((NET_IPv4_ADDR)127uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)255uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)255uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)254uL << (0u * DEF_OCTET_NBR_BITS)))

#if 0
//                                                                 Private network 10.x.x.x
#define  NET_IPv4_ADDR_PRIVATE_NET_10                (((NET_IPv4_ADDR) 10uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)  0uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  0uL << (0u * DEF_OCTET_NBR_BITS)))

#define  NET_IPv4_ADDR_PRIVATE_NET_10_DFTL_GATEWAY   (((NET_IPv4_ADDR) 10uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)  0uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  1uL << (0u * DEF_OCTET_NBR_BITS)))

//                                                                 Private network 172.16.x.x
#define  NET_IPv4_ADDR_PRIVATE_NET_172               (((NET_IPv4_ADDR) 172uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)  16uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)   0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)   0uL << (0u * DEF_OCTET_NBR_BITS)))

#define  NET_IPv4_ADDR_PRIVATE_NET_172_DFTL_GATEWAY  (((NET_IPv4_ADDR) 172uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)  16uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)   0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)   1uL << (0u * DEF_OCTET_NBR_BITS)))

//                                                                 Private network 192.168.x.x
#define  NET_IPv4_ADDR_PRIVATE_NET_192               (((NET_IPv4_ADDR) 192uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR) 168uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)   0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)   0uL << (0u * DEF_OCTET_NBR_BITS)))

#define  NET_IPv4_ADDR_PRIVATE_NET_192_DFTL_GATEWAY  (((NET_IPv4_ADDR)  192uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)  168uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)    0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)    1uL << (0u * DEF_OCTET_NBR_BITS)))
#endif

//                                                                 Link-local net           (see Note #3c1).
#define  NET_IPv4_ADDR_LOCAL_LINK_NET                (((NET_IPv4_ADDR)169uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)254uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  0uL << (0u * DEF_OCTET_NBR_BITS)))

//                                                                 Link-local broadcast     (see Note #3c2B).
#define  NET_IPv4_ADDR_LOCAL_LINK_BROADCAST          (((NET_IPv4_ADDR)169uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)254uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)255uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)255uL << (0u * DEF_OCTET_NBR_BITS)))

//                                                                 Link-local min host addr (see Note #3c2A1).
#define  NET_IPv4_ADDR_LOCAL_LINK_HOST_MIN           (((NET_IPv4_ADDR)169uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)254uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  1uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)  0uL << (0u * DEF_OCTET_NBR_BITS)))
//                                                                 Link-local max host addr (see Note #3c2A2).
#define  NET_IPv4_ADDR_LOCAL_LINK_HOST_MAX           (((NET_IPv4_ADDR)169uL << (3u * DEF_OCTET_NBR_BITS))   \
                                                      | ((NET_IPv4_ADDR)254uL << (2u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)254uL << (1u * DEF_OCTET_NBR_BITS)) \
                                                      | ((NET_IPv4_ADDR)255uL << (0u * DEF_OCTET_NBR_BITS)))

//                                                                 Multicast min       addr (see Note #3d2).
#define  NET_IPv4_ADDR_MULTICAST_MIN       (((NET_IPv4_ADDR)224uL << (3u * DEF_OCTET_NBR_BITS))   \
                                            | ((NET_IPv4_ADDR)  0uL << (2u * DEF_OCTET_NBR_BITS)) \
                                            | ((NET_IPv4_ADDR)  0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                            | ((NET_IPv4_ADDR)  1uL << (0u * DEF_OCTET_NBR_BITS)))
//                                                                 Multicast max       addr (see Note #3d1B).
#define  NET_IPv4_ADDR_MULTICAST_MAX       (((NET_IPv4_ADDR)239uL << (3u * DEF_OCTET_NBR_BITS))   \
                                            | ((NET_IPv4_ADDR)255uL << (2u * DEF_OCTET_NBR_BITS)) \
                                            | ((NET_IPv4_ADDR)255uL << (1u * DEF_OCTET_NBR_BITS)) \
                                            | ((NET_IPv4_ADDR)255uL << (0u * DEF_OCTET_NBR_BITS)))

//                                                                 Multicast min host  addr (see Note #3d2A).
#define  NET_IPv4_ADDR_MULTICAST_HOST_MIN  (((NET_IPv4_ADDR)224uL << (3u * DEF_OCTET_NBR_BITS))   \
                                            | ((NET_IPv4_ADDR)  0uL << (2u * DEF_OCTET_NBR_BITS)) \
                                            | ((NET_IPv4_ADDR)  0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                            | ((NET_IPv4_ADDR)  2uL << (0u * DEF_OCTET_NBR_BITS)))
//                                                                 Multicast max host  addr (see Note #3d1B).
#define  NET_IPv4_ADDR_MULTICAST_HOST_MAX     NET_IPv4_ADDR_MULTICAST_MAX

//                                                                 Multicast all-hosts addr (see Note #3d2B).
#define  NET_IPv4_ADDR_MULTICAST_ALL_HOSTS (((NET_IPv4_ADDR)224uL << (3u * DEF_OCTET_NBR_BITS))   \
                                            | ((NET_IPv4_ADDR)  0uL << (2u * DEF_OCTET_NBR_BITS)) \
                                            | ((NET_IPv4_ADDR)  0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                            | ((NET_IPv4_ADDR)  1uL << (0u * DEF_OCTET_NBR_BITS)))

#define  NET_IPv4_ADDR_LOCAL_HOST_MASK_NET          0xFF000000u
#define  NET_IPv4_ADDR_LOCAL_HOST_MASK_HOST         0x00FFFFFFu

#define  NET_IPv4_ADDR_LOCAL_LINK_MASK_NET          0xFFFF0000u
#define  NET_IPv4_ADDR_LOCAL_LINK_MASK_HOST         0x0000FFFFu

/********************************************************************************************************
 *                                           IP FLAG DEFINES
 *******************************************************************************************************/

//                                                                 ------------------- NET IP FLAGS -------------------
#define  NET_IPv4_FLAG_NONE                           DEF_BIT_NONE

//                                                                 IP tx flags copied from IP hdr flags.
#define  NET_IPv4_FLAG_TX_DONT_FRAG                   NET_IPv4_HDR_FLAG_FRAG_DONT

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   IPv4 OPTION CONFIGURATION TYPE
 *
 * Note(s) : (1) NET_IPv4_OPT_CFG_TYPE_&&& #define values specifically chosen as ASCII representations of
 *               the IP option configuration types.  Memory displays of IPv4 option configuration buffers
 *               will display the IPv4 option configuration TYPEs with their chosen ASCII names.
 *******************************************************************************************************/

typedef enum net_ipv4_opt_type {
  NET_IPv4_OPT_TYPE_NONE = 0,
  NET_IPv4_OPT_TYPE_ROUTE_STRICT,
  NET_IPv4_OPT_TYPE_ROUTE_LOOSE,
  NET_IPv4_OPT_TYPE_ROUTE_REC,
  NET_IPv4_OPT_TYPE_TS_ONLY,
  NET_IPv4_OPT_TYPE_TS_ROUTE_REC,
  NET_IPv4_OPT_TYPE_TS_ROUTE_SPEC,
  NET_IPv4_OPT_TYPE_SECURITY,
  NET_IPv4_OPT_SECURITY_EXTENDED
} NET_IPv4_OPT_TYPE;

/********************************************************************************************************
 *                                               IPv4 HEADER
 *
 * Note(s) : (1) See RFC #791, Section 3.1 for IPv4 datagram header format.
 *
 *           (2) IPv4 Version Number & Header Length are encoded in the first octet of an IPv4 header as
 *               follows :
 *
 *                           7 6 5 4   3 2 1 0
 *                       ---------------------
 *                       |  V E R  | H L E N |
 *                       ---------------------
 *
 *                   where
 *                           VER         IPv4 version; currently 4 (see 'net_ipv4.h  Note #1')
 *                           HLEN        IPv4 Headers' length in 32-bit words; MUST be at least 5 (20-octet header)
 *                                           & MUST be less than or equal to 15 (60-octet header)
 *
 *           (3) Type of Service (TOS) is encoded in the second octet of an IPv4 header as follows (see 'RFC #1349
 *               Type of Service in the Internet Protocol Suite' for required TOS implementation) :
 *
 *                           7   6   5   4   3   2   1   0
 *                       --------------------------------
 *                       |PRECEDENCE| D | T | R | C | 0 |
 *                       --------------------------------
 *
 *                   where
 *                           PRECEDENCE      Datagram Priority (see 'IPv4 HEADER TYPE OF SERVICE (TOS) DEFINES') :
 *                                               '000' - Lowest  "Routine"         Priority (default)
 *                                               '111' - Highest "Network Control" Priority
 *                           D               Datagram Delay Request :
 *                                               '0' - Normal Delay       requested (default)
 *                                               '1' - Low    Delay       requested
 *                           T               Datagram Throughput Request :
 *                                               '0' - Normal Throughput  requested (default)
 *                                               '1' - High   Throughput  requested
 *                           R               Datagram Reliability Request :
 *                                               '0' - Normal Reliability requested (default)
 *                                               '1' - High   Reliability requested
 *                           C               Datagram Cost Request :
 *                                               '0' - Normal Cost        requested (default)
 *                                               '1' - Low    Cost        requested
 *                           0               MUST be zero; i.e. '0'
 *
 *           (4) Flags & Fragment Offset are encoded in the seventh & eighth octets of an IPv4 header as follows :
 *
 *                           15 14 13  12 11 10 9 8 7 6 5 4 3 2 1 0
 *                       ----------------------------------------
 *                       | 0 DF MF |      FRAGMENT OFFSET       |
 *                       ----------------------------------------
 *
 *                   where
 *                           0                   MUST be zero; i.e. '0'
 *                           DF                  'Do Not Fragment' Flag :
 *                                                   '0' - Datagram fragmentation     allowed
 *                                                   '1' - Datagram fragmentation NOT allowed
 *                           MF                  'More Fragments' Flag :
 *                                                   '0' - No more fragments for datagram; i.e. last fragment
 *                                                   '1' -    More fragments for datagram
 *                           FRAGMENT OFFSET     Offset of fragment in original datagram, measured in units of
 *                                                   8 octets (64 bits)
 *
 *           (5) Supports ONLY a subset of allowed protocol numbers :
 *
 *               (a) ICMP
 *               (b) IGMP
 *               (c) UDP
 *               (d) TCP
 *
 *               See also 'net.h  Note #2a';
 *                   & see 'RFC #1340  Assigned Numbers' for a complete list of protocol numbers.
 *******************************************************************************************************/

//                                                                 ------------------- NET IPv4 HDR -------------------
typedef  struct  net_ipv4_hdr {
  CPU_INT08U         Ver_HdrLen;                                // IPv4 datagram ver nbr/hdr len   (see Note #2).
  NET_IPv4_TOS       TOS;                                       // IPv4 datagram TOS               (see Note #3).
  CPU_INT16U         TotLen;                                    // IPv4 datagram tot len.
  CPU_INT16U         ID;                                        // IPv4 datagram ID.
  NET_IPv4_HDR_FLAGS Flags_FragOffset;                          // IPv4 datagram flags/frag offset (see Note #4).
  NET_IPv4_TTL       TTL;                                       // IPv4 datagram TTL.
  CPU_INT08U         Protocol;                                  // IPv4 datagram protocol          (see Note #5).
  NET_CHK_SUM        ChkSum;                                    // IPv4 datagram chk sum.
  NET_IPv4_ADDR      AddrSrc;                                   // IPv4 datagram src  addr.
  NET_IPv4_ADDR      AddrDest;                                  // IPv4 datagram dest addr.
  NET_IPv4_OPT_SIZE  Opts[NET_IPv4_HDR_OPT_NBR_MAX];            // IPv4 datagram opts (if any).
} NET_IPv4_HDR;

/********************************************************************************************************
 *                               IPv4 HEADER OPTION CONFIGURATION DATA TYPES
 *
 * Note(s) : (1) RFC #1122, Section 3.2.1.8 states that "there MUST be a means ... to specify IPv4 options
 *               to included in transmitted IPv4 datagrams".
 *******************************************************************************************************/

/********************************************************************************************************
 *                   IPv4 ROUTE & INTERNET TIMESTAMP OPTIONS CONFIGURATION DATA TYPE
 *
 * Note(s) : (1) 'NET_IPv4_OPT_CFG_ROUTE_TS' data type used to configure IPv4 Route & Internet Timestamp
 *                   transmit options :
 *
 *               (a) Type        specifies the desired IPv4 option configuration type
 *               (b) Nbr         specifies the desired number of option entries to allocate
 *               (c) Route       specifies the desired IPv4 addresses for Route or Internet Timestamp
 *               (d) TS          specifies the desired Internet Timestamps
 *******************************************************************************************************/

typedef  struct  net_ipv4_opt_cfg_route_ts {
  NET_IPv4_OPT_TYPE Type;                                       // IPv4 opt type.
  CPU_INT08U        Nbr;                                        // IPv4 opt nbr.
  NET_IPv4_ADDR     Route[NET_IPv4_OPT_PARAM_NBR_MAX];          // IPv4 route addrs.
  NET_TS            TS[NET_IPv4_OPT_PARAM_NBR_MAX];             // IPv4 TS's.
  void              *NextOptPtr;                                // Ptr to next IPv4 opt cfg.
} NET_IPv4_OPT_CFG_ROUTE_TS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           NET_IPv4_TX_GET_ID()
 *
 * @brief    Get next IPv4 transmit identification number.
 *
 * @param    id  Variable that will receive the returned IPv4 transmit identification number.
 *
 *           NetIPv4_TxPktPrepareHdr(),
 *           NetIPv4_ReTxPktPrepareHdr().
 *
 *           This macro is an INTERNAL network protocol suite macro & SHOULD NOT be called by
 *           application function(s).
 *
 * @note         (1) Return IPv4 identification number is NOT converted from host-order to network-order.
 *******************************************************************************************************/

#define  NET_IPv4_TX_GET_ID(id)                 do { NET_UTIL_VAL_COPY_16(&(id), &NetIPv4_TxID_Ctr); \
                                                     NetIPv4_TxID_Ctr++;                           } while (0)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetIPv4_Init(MEM_SEG  *p_mem_seg,
                  RTOS_ERR *p_err);

#ifdef NET_IPv4_LINK_LOCAL_MODULE_EN
void NetIPv4_AddrLinkLocalInit(MEM_SEG  *p_mem_seg,
                               RTOS_ERR *p_err);
#endif

CPU_BOOLEAN NetIPv4_CfgAddrAddDynamic(NET_IF_NBR    if_nbr,
                                      NET_IPv4_ADDR addr_host,
                                      NET_IPv4_ADDR addr_subnet_mask,
                                      NET_IPv4_ADDR addr_dflt_gateway,
                                      RTOS_ERR      *p_err);

CPU_BOOLEAN NetIPv4_CfgAddrAddDynamicStart(NET_IF_NBR if_nbr,
                                           RTOS_ERR   *p_err);

CPU_BOOLEAN NetIPv4_CfgAddrAddDynamicStop(NET_IF_NBR if_nbr,
                                          RTOS_ERR   *p_err);

CPU_BOOLEAN NetIPv4_CfgAddrAddHandler(NET_IF_NBR    if_nbr,
                                      NET_IPv4_ADDR addr_host,
                                      NET_IPv4_ADDR addr_subnet_mask,
                                      NET_IPv4_ADDR addr_dflt_gateway,
                                      RTOS_ERR      *p_err);

//                                                                 -------------- GET FNCTS ---------------
CPU_BOOLEAN NetIPv4_GetAddrHostHandler(NET_IF_NBR       if_nbr,
                                       NET_IPv4_ADDR    *p_addr_tbl,
                                       NET_IP_ADDRS_QTY *p_addr_tbl_qty,
                                       RTOS_ERR         *p_err);

NET_IPv4_ADDR NetIPv4_GetAddrSrcHandler(NET_IF_NBR    *p_if_nbr,
                                        NET_IPv4_ADDR addr_remote,
                                        RTOS_ERR      *p_err);

NET_IF_NBR NetIPv4_GetAddrHostIF_Nbr(NET_IPv4_ADDR addr);

NET_IF_NBR NetIPv4_GetAddrHostCfgdIF_Nbr(NET_IPv4_ADDR addr);

//                                                                 ------------- STATUS FNCTS -------------
CPU_BOOLEAN NetIPv4_IsAddrsCfgdOnIF_Handler(NET_IF_NBR if_nbr,
                                            RTOS_ERR   *p_err);

CPU_BOOLEAN NetIPv4_IsValidTOS(NET_IPv4_TOS TOS);

CPU_BOOLEAN NetIPv4_IsValidTTL(NET_IPv4_TTL TTL);

CPU_BOOLEAN NetIPv4_IsValidFlags(NET_IPv4_FLAGS flags);

//                                                                 --------------- RX FNCTS ---------------
void NetIPv4_Rx(NET_BUF  *p_buf,
                RTOS_ERR *p_err);

//                                                                 --------------- TX FNCTS ---------------
void NetIPv4_Tx(NET_BUF        *p_buf,
                NET_IPv4_ADDR  addr_src,
                NET_IPv4_ADDR  addr_dest,
                NET_IPv4_TOS   TOS,
                NET_IPv4_TTL   TTL,
                NET_IPv4_FLAGS flags,
                void           *p_opts,
                RTOS_ERR       *p_err);

void NetIPv4_TxIxDataGet(NET_IF_NBR if_nbr,
                         CPU_INT32U data_len,
                         CPU_INT16U mtu,
                         CPU_INT16U *p_ix);

void NetIPv4_ReTx(NET_BUF  *p_buf,
                  RTOS_ERR *p_err);

//                                                                 ------------ NET MGR FNCTS -------------

NET_IPv4_ADDR_OBJ *NetIPv4_GetAddrObjCfgdOnIF(NET_IF_NBR    if_nbr,
                                              NET_IPv4_ADDR addr);

void NetIPv4_GetHostAddrProtocol(NET_IF_NBR if_nbr,
                                 CPU_INT08U *p_addr_protocol_tbl,
                                 CPU_INT08U *p_addr_protocol_tbl_qty,
                                 CPU_INT08U *p_addr_protocol_len,
                                 RTOS_ERR   *p_err);

NET_IF_NBR NetIPv4_GetAddrProtocolIF_Nbr(CPU_INT08U *p_addr_protocol,
                                         CPU_INT08U addr_protocol_len,
                                         RTOS_ERR   *p_err);

CPU_BOOLEAN NetIPv4_IsValidAddrProtocol(CPU_INT08U *p_addr_protocol,
                                        CPU_INT08U addr_protocol_len);

CPU_BOOLEAN NetIPv4_IsAddrInit(CPU_INT08U *p_addr_protocol,
                               CPU_INT08U addr_protocol_len);

#ifdef  NET_MCAST_MODULE_EN
CPU_BOOLEAN NetIPv4_IsAddrProtocolMulticast(CPU_INT08U *p_addr_protocol,
                                            CPU_INT08U addr_protocol_len);
#endif

CPU_BOOLEAN NetIPv4_IsAddrProtocolConflict(NET_IF_NBR if_nbr);

void NetIPv4_ChkAddrProtocolConflict(NET_IF_NBR if_nbr,
                                     CPU_INT08U *p_addr_protocol,
                                     CPU_INT08U addr_protocol_len,
                                     RTOS_ERR   *p_err);

CPU_BOOLEAN NetIPv4_AddrValidate(NET_IF_NBR    if_nbr,
                                 NET_IPv4_ADDR addr,
                                 RTOS_ERR      *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IPv4_MODULE_EN
#endif // _NET_IPv4_PRIV_H_
