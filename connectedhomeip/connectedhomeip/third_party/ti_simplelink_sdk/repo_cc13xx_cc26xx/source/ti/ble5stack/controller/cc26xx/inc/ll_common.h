/******************************************************************************

 @file  ll_common.h

 @brief This file contains the Link Layer (LL) types, constants,
        API's etc. for the Bluetooth Low Energy (ULE) Controller that are
        internally common to LL routines.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR.
 *
 */


#ifndef LL_COMMON_H
#define LL_COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#include <ti/drivers/rf/RF.h>
#include <ti/drivers/TRNG.h>

#include "rf_hal.h"
#include "osal.h"
#include "ll.h"
#include "ll_scheduler.h"
#include "hal_assert.h"
#if !defined(DeviceFamily_CC26X1)
#include <driverlib/rf_bt5_iq_autocopy.h>
#else
#include <ti/devices/cc13x2_cc26x2/driverlib/rf_bt5_iq_autocopy.h>
#endif

/*******************************************************************************
 * MACROS
 */

// Note: These macros assume the packet header has already been masked with
//       LL_DATA_PDU_HDR_LLID_MASK.
#define LL_DATA_PDU( pktHdr )            ((pktHdr) != LL_DATA_PDU_HDR_LLID_CONTROL_PKT)
#define LL_CTRL_PDU( pktHdr )            ((pktHdr) == LL_DATA_PDU_HDR_LLID_CONTROL_PKT)
#define LL_INVALID_LLID( pktHdr )        ((pktHdr) == LL_DATA_PDU_HDR_LLID_RESERVED)
//
#define LL_SCAN_REQ_PDU( pktHdr )        (((pktHdr) & 0x0F) == LL_PKT_TYPE_SCAN_REQ)
#define LL_SCAN_RSP_PDU( pktHdr )        (((pktHdr) & 0x0F) == LL_PKT_TYPE_SCAN_RSP)
#define LL_CONN_REQ_PDU( pktHdr )        (((pktHdr) & 0x0F) == LL_PKT_TYPE_CONNECT_IND)
#define LL_ADV_IND_PDU( pktHdr )         (((pktHdr) & 0x0F) == LL_PKT_TYPE_ADV_IND)
#define LL_ADV_DIRECT_IND_PDU( pktHdr )  (((pktHdr) & 0x0F) == LL_PKT_TYPE_ADV_DIRECT_IND)
#define LL_ADV_NONCONN_IND_PDU( pktHdr ) (((pktHdr) & 0x0F) == LL_PKT_TYPE_ADV_NONCONN_IND)
#define LL_ADV_SCAN_IND_PDU( pktHdr )    (((pktHdr) & 0x0F) == LL_PKT_TYPE_ADV_SCAN_IND)
#define LL_ADV_EXT_IND_PDU( pktHdr )     (((pktHdr) & 0x0F) == LL_PKT_TYPE_ADV_EXT_IND)
#define LL_AUX_PDU( pktHdr )             (((pktHdr) & 0x0F) == LL_PKT_TYPE_GENERIC_AUX)
#define LL_AUX_CONN_RSP_PDU( pktHdr )    (((pktHdr) & 0x0F) == LL_PKT_TYPE_AUX_CONNECT_RSP)
//
#define LL_ADV_PDU_TYPE( pktHdr )        ((pktHdr) & 0x0F)
#define LL_LEGACY_ADV_PDU( pktHdr )      (LL_ADV_IND_PDU( (pktHdr) )          || \
                                          LL_ADV_DIRECT_IND_PDU( (pktHdr) )   || \
                                          LL_ADV_NONCONN_IND_PDU( (pktHdr) )  || \
                                          LL_SCAN_RSP_PDU( (pktHdr) )         || \
                                          LL_ADV_SCAN_IND_PDU( (pktHdr) ))

#define LL_ADV_HDR_GET_TX_ADD( hdr )     (((hdr) &  0x40) >> 6)
#define LL_ADV_HDR_GET_RX_ADD( hdr )     (((hdr) &  0x80) >> 7)
#define LL_ADV_HDR_SET_TX_ADD( hdr, v )  (((hdr) & ~0x40) | ((v) << 6))
#define LL_ADV_HDR_SET_RX_ADD( hdr, v )  (((hdr) & ~0x80) | ((v) << 7))
//
#define LL_ADV_HDR_GET_CHSEL( hdr )      (((hdr) &  0x20) >> 5)
#define LL_ADV_HDR_SET_CHSEL( hdr, v )   (((hdr) & ~0x20) | ((v) << 5))

// local ASSERT handler
#if defined( DEBUG )
#define LL_ASSERT(cond) {volatile uint8 i = (cond); while(!i);}
#else // !DEBUG
// Note: Use HALNODEBUG to eliminate HAL assert handling (i.e. no assert).
// Note: If HALNODEBUG is not used, use ASSERT_RESET to reset system on assert.
//       Otherwise, evaluation board hazard lights are used.
// Note: Unused input parameter possible when HALNODEBUG; PC-Lint error 715.
#define LL_ASSERT(cond) HAL_ASSERT(cond)
#endif // DEBUG

// RSSI Correction

#define ADI0_TRIM_MASK                           BV(5)

#define GET_RSSI_OFFSET()                                                      \
  ((*((uint32 *)(FCFG1_BASE + LL_RSSI_OFFSET))>>9) & 0xFF)

#define GET_RSSI_CORRECTION( rfConfig )                                        \
  ((rfConfig) & ADI0_TRIM_MASK) ? (int8)GET_RSSI_OFFSET() : (int8)0

// checks if RSSI is valid - returns boolean
#define LL_CHECK_RSSI_VALID( rssi )                                            \
          ((rssi) == LL_RF_RSSI_UNDEFINED || (rssi) == LL_RF_RSSI_INVALID)) ?  \
          TRUE                                                              :  \
          FALSE

// corrects RSSI if valid, otherwise returns not available
// Note: Input is uint8, output int8.
#define LL_CHECK_LAST_RSSI( rssi )                                             \
          ((rssi) == LL_RF_RSSI_UNDEFINED || (rssi) == LL_RF_RSSI_INVALID)  ?  \
          (int8)LL_RSSI_NOT_AVAILABLE                                       :  \
          ((int8)(rssi) - rssiCorrection)

#define CHECK_CRITICAL_SECTION() (__get_BASEPRI() & 0x20 )

#define LL_CMP_BDADDR( dstPtr, srcPtr )                                        \
  ( ((dstPtr)[0] == (srcPtr)[0]) &&                                            \
    ((dstPtr)[1] == (srcPtr)[1]) &&                                            \
    ((dstPtr)[2] == (srcPtr)[2]) &&                                            \
    ((dstPtr)[3] == (srcPtr)[3]) &&                                            \
    ((dstPtr)[4] == (srcPtr)[4]) &&                                            \
    ((dstPtr)[5] == (srcPtr)[5]) )

/*
** Control Procedure Macros
*/
#define SET_FEATURE_FLAG( flags, flag ) ((flags) |= (flag))
#define TST_FEATURE_FLAG( flags, flag ) ((flags) & (flag))
#define CLR_FEATURE_FLAG( flags, flag ) ((flags) &= ~(flag))

#define ONLY_ONE_BIT_SET( x ) (((x) != 0) && !((x) & ((x)-1)))

// Special connection handles (currently only used in the controller for
// connection event reports)
#define LL_CONNHANDLE_ALL                  0xFFFD  //!< Terminates all links
#define LL_CONNHANDLE_LOOPBACK             0xFFFE  //!< Loopback connection handle, used to loopback a message
#define LL_CONNHANDLE_INVALID              0xFFFF  //!< Invalid connection handle, used for no connection handle

// Microsecond to/from RAT Tick Conversion
#define US_TO_RAT_TICKS( us )              ((us) << 2)
#define RAT_TICKS_TO_US( rat )             ((rat) >> 2)

/*******************************************************************************
 * CONSTANTS
 */

// Link Layer State
#define LL_STATE_IDLE                                  0x00
#define LL_STATE_ADV_UNDIRECTED                        0x01
#define LL_STATE_ADV_DIRECTED                          0x02
#define LL_STATE_ADV_SCANNABLE                         0x03
#define LL_STATE_ADV_NONCONN                           0x04
#define LL_STATE_SCAN                                  0x05
#define LL_STATE_INIT                                  0x06
#define LL_STATE_CONN_SLAVE                            0x07
#define LL_STATE_CONN_MASTER                           0x08
#define LL_STATE_DIRECT_TEST_MODE_TX                   0x09
#define LL_STATE_DIRECT_TEST_MODE_RX                   0x0A
#define LL_STATE_MODEM_TEST_TX                         0x0B
#define LL_STATE_MODEM_TEST_RX                         0x0C
#define LL_STATE_MODEM_TEST_TX_FREQ_HOPPING            0x0D
// Extended Advertising
// TEMP: CONSOLIDATE BACK INTO EXISTING STATES
#define LL_STATE_EXT_ADV                               0x11
#define LL_STATE_PERIODIC_ADV                          0x12
#define LL_STATE_PERIODIC_SCAN                         0x13

// LL Events
#define LL_EVT_NONE                                    0x0000
#define LL_EVT_POST_PROCESS_RF                         0x0001
#define LL_EVT_DIRECTED_ADV_FAILED                     0x0002
#define LL_EVT_SLAVE_CONN_CREATED                      0x0004
#define LL_EVT_MASTER_CONN_CREATED                     0x0008
#define LL_EVT_MASTER_CONN_CANCELLED                   0x0010
#define LL_EVT_EXT_SCAN_TIMEOUT                        0x0020
#define LL_EVT_EXT_ADV_TIMEOUT                         0x0040
#define LL_EVT_SLAVE_CONN_CREATED_BAD_PARAM            0x0080
#define LL_EVT_PERIODIC_SCAN_CANCELLED                 0x0100
#define LL_EVT_RESET_SYSTEM_HARD                       0x0200
#define LL_EVT_RESET_SYSTEM_SOFT                       0x0400
#define LL_EVT_CONN_DISCONNECTED_IMMED                 0x0800
#define LL_EVT_ADDRESS_RESOLUTION_TIMEOUT              0x1000
#define LL_EVT_INIT_DONE                               0x2000
#define LL_EVT_OUT_OF_MEMORY                           0x4000
#define SYS_RESERVED                                   SYS_EVENT_MSG

// Hardware Failure Status
#define HW_FAIL_PAST_START_TRIG                        0x80
#define HW_FAIL_OUT_OF_MEMORY                          0x81
#define HW_FAIL_FW_INTERNAL_ERROR                      0x82
#define HW_FAIL_INVAILD_RF_COMMAND                     0x83
#define HW_FAIL_UNKNOWN_RF_STATUS                      0x84
#define HW_FAIL_UNEXPECTED_RF_STATUS                   0x85
#define HW_FAIL_UNKNOWN_LL_STATE                       0x86
#define HW_FAIL_FS_PROG_ERROR                          0x87
#define HW_FAIL_FS_FAIL_TO_START                       0x88
#define HW_FAIL_RF_INIT_ERROR                          0x89
#define HW_FAIL_PDU_SIZE_EXCEEDS_MTU                   0x8A
#define HW_FAIL_PKT_LEN_EXCEEDS_PDU_SIZE               0x8B
#define HW_FAIL_INADEQUATE_PKT_LEN                     0x8C
#define HW_FAIL_DISALLOWED_PHY_CHANGE                  0x8D
#define HW_FAIL_NO_TIMER_AVAILABLE                     0x8E
#define HW_FAIL_EXTENDED_WL_FAULT                      0x8F
#define HW_FAIL_NO_RAT_COMPARE_AVAILABLE               0x90
#define HW_FAIL_RF_DRIVER_ERROR                        0xE0
#define HW_FAIL_UNKNOWN_ERROR                          0xFF

/*
** Air Interface Packets
*/

// Packet Type Mask
#define LL_PKT_EVT_TYPE_MASK                           0x0F

// Advertising Channel PDUs
#define LL_PKT_TYPE_ADV_IND                            0  // primary   - 1M
#define LL_PKT_TYPE_ADV_DIRECT_IND                     1  // primary   - 1M
#define LL_PKT_TYPE_ADV_NONCONN_IND                    2  // primary   - 1M
#define LL_PKT_TYPE_SCAN_REQ                           3  // primary   - 1M
#define LL_PKT_TYPE_AUX_SCAN_REQ                       3  // secondary - 1M/2M/Coded
#define LL_PKT_TYPE_SCAN_RSP                           4  // primary   - 1M
#define LL_PKT_TYPE_CONNECT_IND                        5  // primary   - 1M
#define LL_PKT_TYPE_AUX_CONNECT_IND                    5  // secondary - 1M/2M/Coded
#define LL_PKT_TYPE_ADV_SCAN_IND                       6  // primary   - 1M
#define LL_PKT_TYPE_GENERIC_AUX                        7  // used for any secondary
#define LL_PKT_TYPE_ADV_EXT_IND                        7  // secondary - 1M/Coded
#define LL_PKT_TYPE_AUX_ADV_IND                        7  // secondary - 1M/2M/Coded
#define LL_PKT_TYPE_AUX_SCAN_RSP                       7  // secondary - 1M/2M/Coded
#define LL_PKT_TYPE_AUX_SYNC_IND                       7  // secondary - 1M/2M/Coded
#define LL_PKT_TYPE_AUX_CHAIN_IND                      7  // secondary - 1M/2M/Coded
#define LL_PKT_TYPE_AUX_CONNECT_RSP                    8  // secondary - 1M/2M/Coded
#define LL_PKT_TYPE_RESERVED                           9

// Packet Related Information
#define LL_PKT_PREAMBLE_LEN                            1
#define LL_PKT_SYNCH_LEN                               4
#define LL_PKT_LLID_LEN                                1
#define LL_PKT_HDR_LEN                                 2
#define LL_PKT_MIC_LEN                                 4
#define LL_PKT_CRC_LEN                                 3

// Payload sizes
// Note: For control packets, this is the Opcode + CtrData.
#define LL_CONN_UPDATE_IND_PAYLOAD_LEN                 12
#define LL_CHAN_MAP_IND_PAYLOAD_LEN                    8
#define LL_TERM_IND_PAYLOAD_LEN                        2
#define LL_ENC_REQ_PAYLOAD_LEN                         23
#define LL_ENC_RSP_PAYLOAD_LEN                         13
#define LL_START_ENC_REQ_PAYLOAD_LEN                   1
#define LL_START_ENC_RSP_PAYLOAD_LEN                   1
#define LL_UNKNOWN_RSP_PAYLOAD_LEN                     2
#define LL_FEATURE_REQ_PAYLOAD_LEN                     9
#define LL_FEATURE_RSP_PAYLOAD_LEN                     9
#define LL_PAUSE_ENC_REQ_PAYLOAD_LEN                   1
#define LL_PAUSE_ENC_RSP_PAYLOAD_LEN                   1
#define LL_VERSION_IND_PAYLOAD_LEN                     6
#define LL_REJECT_IND_PAYLOAD_LEN                      2
#define LL_SLAVE_FEATURE_REQ_PAYLOAD_LEN               9
#define LL_CONN_PARAM_REQ_PAYLOAD_LEN                  24
#define LL_CONN_PARAM_RSP_PAYLOAD_LEN                  24
#define LL_REJECT_EXT_IND_PAYLOAD_LEN                  3
#define LL_PING_REQ_PAYLOAD_LEN                        1
#define LL_PING_RSP_PAYLOAD_LEN                        1
#define LL_LENGTH_REQ_PAYLOAD_LEN                      9
#define LL_LENGTH_RSP_PAYLOAD_LEN                      9
#define LL_PHY_REQ_PAYLOAD_LEN                         3
#define LL_PHY_RSP_PAYLOAD_LEN                         3
#define LL_PHY_UPDATE_REQ_PAYLOAD_LEN                  5
#define LL_MIN_USED_CHANNELS_IND_LEN                   3
#define LL_CTE_REQ_PAYLOAD_LEN                         2
#define LL_CTE_RSP_PAYLOAD_LEN                         1
// set to one byte larger than the largest control packet length
#define LL_INVALID_CTRL_LEN                            25

// miscellaneous fields, in bytes
#define LL_CONNECT_IND_LL_DATA_LEN                     22
#define LL_CONNECT_IND_PKT_LEN                         34
#define LL_NUM_BYTES_FOR_CHAN_MAP                      5   //(LL_MAX_NUM_ADV_CHAN+LL_MAX_NUM_DATA_CHAN)/sizeof(uint8)

// Terminate Indication Control Packet
// Note: MD bit is set to 1 to force sending this packet with MD=1 even though
//       the rest of the TX FIFO is empty and the MD configuration is set to
//       use automatic MD insertion based on FIFO contents.
// Note: The bytes are listed in little endian and read right-to-left:
//       0x13: MD=1, LLID=3 (LL control packet)
//       0x02: Length=1 plus one since NR will decrement the length
//       0x02: CtrlType=2 (TERMINATE_IND)
#define LL_TERM_IND_PKT_HDR                            ((1 << LL_DATA_PDU_HDR_MD_BIT) | LL_DATA_PDU_HDR_LLID_CONTROL_PKT)

// max number of sequential NACKS before closing a connection event
#define LL_MAX_NUM_RX_NACKS_ALLOWED                    4

// control procedure timeout in coarse timer ticks
#define LL_MAX_CTRL_PROC_TIMEOUT                       64000 // 40s

// authenticated payload timeout
#define LL_APTO_DEFAULT_VALUE                          30000 // 30s in ms

// connection related timing
#define LL_CONNECTION_T_IFS                            150   // in us
#define LL_CONNECTION_SLOT_TIME                        625   // in us

// max future number of events for an update to parameters or data channel
#define LL_MAX_UPDATE_COUNT_RANGE                      32767

// Connection Setup
#define LL_WINDOW_SIZE                                 2  // 2.5ms in 1.25ms ticks
#define LL_WINDOW_OFFSET                               0  // 1.25ms + 0
#define LL_LINK_SETUP_TIMEOUT                          5  // 6 connection intervals (i.e. 0..5)
#define LL_LINK_MIN_WIN_OFFSET                         2  // in 625us units
//
#define LL_LINK_MIN_WIN_OFFSET_AE_UNCODED              4  // in 625us units
#define LL_LINK_MIN_WIN_OFFSET_AE_CODED                6  // in 625us units

// Adv PDU Header Fields
#define LL_ADV_PDU_HDR_TXADDR                          6
#define LL_ADV_PDU_HDR_RXADDR                          7
//
#define LL_ADV_PDU_HDR_LEN_MASK                        0x3F

// Data PDU Header Fields
#define LL_DATA_PDU_HDR_LLID_MASK                      0x03
//
#define LL_DATA_PDU_HDR_LLID_RESERVED                  0
#define LL_DATA_PDU_HDR_LLID_DATA_PKT_NEXT             1
#define LL_DATA_PDU_HDR_LLID_DATA_PKT_FIRST            2
#define LL_DATA_PDU_HDR_LLID_CONTROL_PKT               3
//
#define LL_DATA_PDU_HDR_NESN_BIT                       2
#define LL_DATA_PDU_HDR_SN_BIT                         3
#define LL_DATA_PDU_HDR_MD_BIT                         4
#define LL_DATA_PDU_HDR_CP_BIT                         5
#define LL_DATA_PDU_HDR_NESN_MASK                      0x04
#define LL_DATA_PDU_HDR_SN_MASK                        0x08
#define LL_DATA_PDU_HDR_MD_MASK                        0x10
#define LL_DATA_PDU_HDR_LEN_MASK                       0x1F

// CTE Info packet fields
#define LL_CTE_INFO_TIME_MASK                          0x1F
#define LL_CTE_INFO_TYPE_MASK                          0xC0
#define LL_CTE_INFO_TYPE_OFFSET                        6     // CTE time (5 bits) + RFU (1 bit)

// Data PDU overhead due to HCI packet type(1), connId(2), and length(2)
// Note: This is temporary until replace by BM alloc/free.
#define LL_DATA_HCI_OVERHEAD_LENGTH                    5

// Data PDU Control Packet Types
#define LL_CTRL_CONNECTION_UPDATE_IND                  0  // M
#define LL_CTRL_CHANNEL_MAP_IND                        1  // M
#define LL_CTRL_TERMINATE_IND                          2  // M, S
#define LL_CTRL_ENC_REQ                                3  // M
#define LL_CTRL_ENC_RSP                                4  //  , S
#define LL_CTRL_START_ENC_REQ                          5  //  , S
#define LL_CTRL_START_ENC_RSP                          6  // M, S
#define LL_CTRL_UNKNOWN_RSP                            7  // M, S
#define LL_CTRL_FEATURE_REQ                            8  // M
#define LL_CTRL_FEATURE_RSP                            9  // M, S
#define LL_CTRL_PAUSE_ENC_REQ                          10 // M
#define LL_CTRL_PAUSE_ENC_RSP                          11 //  , S
#define LL_CTRL_VERSION_IND                            12 // M, S
#define LL_CTRL_REJECT_IND                             13 //  , S
#define LL_CTRL_SLAVE_FEATURE_REQ                      14 //  , S
#define LL_CTRL_CONNECTION_PARAM_REQ                   15 // M, S
#define LL_CTRL_CONNECTION_PARAM_RSP                   16 //  , S
#define LL_CTRL_REJECT_EXT_IND                         17 // M, S
#define LL_CTRL_PING_REQ                               18 // M, S
#define LL_CTRL_PING_RSP                               19 // M, S
#define LL_CTRL_LENGTH_REQ                             20 // M, S
#define LL_CTRL_LENGTH_RSP                             21 // M, S
#define LL_CTRL_PHY_REQ                                22 // M, S
#define LL_CTRL_PHY_RSP                                23 //  , S
#define LL_CTRL_PHY_UPDATE_REQ                         24 // M
#define LL_CTRL_MIN_USED_CHANNELS_IND                  25 //  , S
#define LL_CTRL_CTE_REQ                                26 // M, S
#define LL_CTRL_CTE_RSP                                27 // M, S
//
#define LL_CTRL_INVALID_OPCODE                         200

//
#define LL_CTRL_TERMINATE_RX_WAIT_FOR_TX_ACK           0xF0 // M (internal to LL only)

#define LL_CTRL_BLE_LOG_STRINGS_MAX 27
extern char *llCtrl_BleLogStrings[];
//
// The following "control packet types" are internally defined to assist the
// Link Layer with control procedure processing.
//
// The LL_CTRL_DUMMY_PLACE_HOLDER_TRANSMIT type is used as a place holder at the
// head of the control packet queue so that other control packets cannot
// interleave a currently executing control procedure. This entry would then be
// replaced with the appropriate control packet at a subsequent time. Any new
// control packets would remain queued behind until the current control
// procedure completes.
//
// The LL_CTRL_DUMMY_PLACE_HOLDER_TX_PENDING type is used as a place holder for
// a control packet that has already been queued for Tx, but due to a collision
// caused by a received packet, must be dequeued and restored at a later time
// without being re-queued on the Tx FIFO.
//
// The LL_CTRL_DUMMY_PLACE_HOLDER_RECEIVE type is used to delay the processing
// of a received control packet.
#define LL_NEW_CTRL_DUMMY_PLACE_HOLDER_TX_PENDING      0xFB
#define LL_CTRL_DUMMY_PLACE_HOLDER_TRANSMIT            0xFC
#define LL_CTRL_DUMMY_PLACE_HOLDER_TX_PENDING          0xFD
#define LL_CTRL_DUMMY_PLACE_HOLDER_RECEIVE             0xFE
#define LL_CTRL_UNDEFINED_PKT                          0xFF

// There is only supposed to be at most one control procedure pending, but some
// extra space is allocated here just in case some queueing is required.
#define LL_MAX_NUM_CTRL_PROC_PKTS                      4

// Control Procedure Actions
#define LL_CTRL_PROC_STATUS_SUCCESS                    0
#define LL_CTRL_PROC_STATUS_TERMINATE                  1
#define LL_CTRL_PROC_STATUS_NOT_PROCESSED              2

// Setup Next Slave Procedure Actions
#define LL_SETUP_NEXT_LINK_STATUS_SUCCESS              0
#define LL_SETUP_NEXT_LINK_STATUS_TERMINATE            1

// Receive Flow Control
#define LL_RX_FLOW_CONTROL_DISABLED                    0
#define LL_RX_FLOW_CONTROL_ENABLED                     1

// Advertisement Channels
#define LL_ADV_BASE_CHAN                               37
//
#define LL_SCAN_ADV_CHAN_37                            (LL_ADV_BASE_CHAN+0)
#define LL_SCAN_ADV_CHAN_38                            (LL_ADV_BASE_CHAN+1)
#define LL_SCAN_ADV_CHAN_39                            (LL_ADV_BASE_CHAN+2)

// Advertiser Synchronization Word
#define ADV_SYNCH_WORD                                 0x8E89BED6  // Adv channel sync
#define ADV_CRC_INIT_VALUE                             0x00555555  // not needed; handled by NR hardware automatically

// Connection Related
#define LL_INVALID_CONNECTION_ID                       0xFF
#define LL_RESERVED_CONNECTION_ID                      0x0F00

// Feature Response Flag
#define LL_FEATURE_RSP_INIT                            0
#define LL_FEATURE_RSP_PENDING                         1
#define LL_FEATURE_RSP_DONE                            2
#define LL_FEATURE_RSP_FAILED                          3

// Encryption Related
#define LL_ENC_RAND_LEN                                8
#define LL_ENC_EDIV_LEN                                2
#define LL_ENC_LTK_LEN                                 16
#define LL_ENC_IV_M_LEN                                4
#define LL_ENC_IV_S_LEN                                4
#define LL_ENC_IV_LINK_LEN                             4
#define LL_ENC_IV_LEN                                  (LL_ENC_IV_M_LEN + LL_ENC_IV_S_LEN)
#define LL_ENC_SKD_M_LEN                               8
#define LL_ENC_SKD_S_LEN                               8
#define LL_ENC_SKD_LINK_LEN                            8
#define LL_ENC_SKD_LEN                                 (LL_ENC_SKD_M_LEN + LL_ENC_SKD_S_LEN)
#define LL_ENC_SK_LEN                                  16
#define LL_ENC_NONCE_LEN                               13
#define LL_ENC_NONCE_IV_OFFSET                         5
#define LL_ENC_MIC_LEN                                 LL_PKT_MIC_LEN
//
#define LL_ENC_IV_M_OFFSET                             LL_ENC_IV_S_LEN
#define LL_ENC_IV_S_OFFSET                             0
#define LL_ENC_SKD_M_OFFSET                            LL_ENC_SKD_S_LEN
#define LL_ENC_SKD_S_OFFSET                            0
//
#define LL_ENC_BLOCK_LEN                               16
#define LL_ENC_CCM_BLOCK_LEN                           LL_ENC_BLOCK_LEN
#define LL_ENC_BLOCK_B0_FLAGS                          0x49
#define LL_ENC_BLOCK_A0_FLAGS                          0x01

// Clock Accuracy
#define LL_CA_20_PPM                                   20
#define LL_CA_40_PPM                                   40
#define LL_CA_50_PPM                                   50
#define LL_CA_100_PPM                                  100

// Default SCA
#define LL_SCA_MASTER_DEFAULT                          5 // 50ppm (ordinal)
#define LL_SCA_SLAVE_DEFAULT                           LL_CA_40_PPM

// TX Output Power Related
#define LL_TX_POWER_0_DBM                              0
#define LL_TX_POWER_5_DBM                              5
#define LL_TX_POWER_INVALID                            -128
//
// value used in radio command txPower field to indicate tx20Power is valid
#define TX_POWER_ESCAPE_VALUE                          0xFFFF
//
// value used to mask PA Type from tx20Power (which is really only 22-bits)
#define TX_POWER_HP_PA_MASK                            (~0x80000000)

// TX Data Context
#define LL_TX_DATA_CONTEXT_TX_ISR                      0
#define LL_TX_DATA_CONTEXT_POST_PROCESSING             1

// Direct Test Mode Related
#define LL_DIRECT_TEST_SYNCH_WORD                      0x71764129
#define LL_DIRECT_TEST_CRC_INIT_VALUE                  0x55555500
#define LL_DIRECT_TEST_CRC_LEN                         3
#define LL_DTM_MAX_PAYLOAD_LEN                         37

// Post-Radio Operations
#define LL_POST_RADIO_SET_RX_GAIN_STD                  0x0001
#define LL_POST_RADIO_SET_RX_GAIN_HIGH                 0x0002
#define LL_POST_RADIO_SET_TX_POWER_MINUS_23_DBM        0x0004
#define LL_POST_RADIO_SET_TX_POWER_MINUS_6_DBM         0x0008
#define LL_POST_RADIO_SET_TX_POWER_0_DBM               0x0010
#define LL_POST_RADIO_SET_TX_POWER_4_DBM               0x0020
#define LL_POST_RADIO_GET_TRNG                         0x0040
#define LL_POST_RADIO_CACHE_RANDOM_NUM                 0x0080
#define LL_POST_RADIO_EXTEND_RF_RANGE                  0x0100

/*
** LL RF Frequencies
*/
// BLE RF Values
#define LL_FIRST_RF_CHAN_FREQ                          2402      // MHz
#define LL_LAST_RF_CHAN_FREQ                           (LL_FIRST_RF_CHAN_FREQ+(2*LL_LAST_RF_CHAN))
//
// The BLE RF interface uses a 8 bit channel field to specify the RF channel:
//  0 ..  39: BLE Advertising/Data Channels
// 40 ..  59: Reserved
// 60 .. 207: Customer Frequency given by 2300+channel MHz
//       255: Use existing frequency.
//
#define LL_FIRST_RF_CHAN_FREQ_OFFSET                   2300
#define LL_FIRST_RF_CHAN_ADJ                           (LL_FIRST_RF_CHAN_FREQ - LL_FIRST_RF_CHAN_FREQ_OFFSET)
#define LL_LAST_RF_CHAN_ADJ                            (LL_LAST_RF_CHAN_FREQ - LL_FIRST_RF_CHAN_FREQ_OFFSET)

/*
** FCFG and CCFG Offsets, and some Miscellaneous
*/

// Flash Size
#if !defined(CC26X2) && !defined(CC13X2) && !defined(CC13X2P)
#define LL_FLASH_PAGE_SIZE                             4096      // in bytes
#else //Agama CC26X2 || CC13X2 || CC13X2P
#define LL_FLASH_PAGE_SIZE                             8192      // in bytes
#endif ////Agama CC26X2 || CC13X2 || CC13X2P
#define LL_FLASH_SIZE_OFFSET                           0x2B1     // in FCFG; num of pages

// BADDR Flash Address Offset in CCA (i.e. flash programmer BLE address)
#if !defined(CC26X2) && !defined(CC13X2) && !defined(CC13X2P)
#define LL_BADDR_PAGE_OFFSET                           0xFD0     // in CCFG (CCA); LSB..MSB
#else //Agama CC26X2 || CC13X2 || CC13X2P
#define LL_BADDR_PAGE_OFFSET                           0x1FD0     // in CCFG (CCA); LSB..MSB
#endif ////Agama CC26X2 || CC13X2 || CC13X2P
#define LL_BADDR_PAGE_LEN                              6

// BADDR Address Offset in FCFG1 (i.e. permanent BLE address)
#define LL_BDADDR_OFFSET                               0x2E8     // in FCFG; LSB..MSB

// Chip ID offset in FCFG1
#define LL_INFO_PAGE_CHIP_ID_OFFSET                    0x118     // in FCFG; LSB..MSB, 16 bytes

// RSSI Offset (i.e. correction) in FCFG1 (PG1 only!)
#define LL_RSSI_OFFSET                                 0x380     // in FCFG; bits 16..9, signed 8 bit value?

#ifndef DISABLE_RCOSC_SW_FIX
// MODE_CONF SCLK_LF_OPTION selection for SCLK_LF
#define SCLK_LF_OPTION_OFFSET                          0xFB6     // in CCFG (CCA); LSB..MSB

// SCLK_LF Options
#define SCLK_LF_MASK                                   0xC0
//
#define SCLK_LF_XOSC_HF                                0
#define SCLK_LF_EXTERNAL                               1
#define SCLK_LF_XOSC_LF                                2
#define SCLK_LF_RCOSC_LF                               3
//
#define RCOSC_LF_SCA                                   1500      // possible worst case drift in PPM
#endif // !DISABLE_RCOSC_SW_FIX

// values for pendingParamUpdate
#define PARAM_UPDATE_NOT_PENDING                       0
#define PARAM_UPDATE_PENDING                           1
#define PARAM_UPDATE_APPLIED                           2

#define PHY_UPDATE_APPLIED                             2
#define CHANNEL_MAP_UPDATE_APPLIED                     2

// Update State Values for Slave Latency
#define UPDATE_SL_OKAY                                 0
#define UPDATE_RX_CTRL_ACK_PENDING                     1
#define UPDATE_NEW_TRANS_PENDING                       2

/*
** Bluetooth LE Feature Support
** Core Specification, Vol. 6, Part B, Section 4.6
*/

// Feature Set Related
#define LL_MAX_FEATURE_SET_SIZE                        8         // in bytes
//
#define LL_FEATURE_NONE                                0x00
// Byte 0
#define LL_FEATURE_ENCRYPTION                          0x01
#define LL_FEATURE_CONN_PARAMS_REQ                     0x02
#define LL_FEATURE_REJECT_EXT_IND                      0x04
#define LL_FEATURE_SLV_FEATURES_EXCHANGE               0x08
#define LL_FEATURE_PING                                0x10
#define LL_FEATURE_DATA_PACKET_LENGTH_EXTENSION        0x20
#define LL_FEATURE_PRIVACY                             0x40
#define LL_FEATURE_EXTENDED_SCANNER_FILTER_POLICIES    0x80
// Byte 1
#define LL_FEATURE_2M_PHY                              0x01
#define LL_FEATURE_STABLE_MODULATION_INDEX_TX          0x02
#define LL_FEATURE_STABLE_MODULATION_INDEX_RX          0x04
#define LL_FEATURE_CODED_PHY                           0x08
#define LL_FEATURE_EXTENDED_ADVERTISING                0x10
#define LL_FEATURE_PERIODIC_ADVERTISING                0x20
#define LL_FEATURE_CHAN_ALGO_2                         0x40
#define LL_FEATURE_LE_POWER_CLASS_1                    0x80
// Byte 2
#define LL_FEATURE_MINIMUM_NUMBER_OF_USED_CHANNELS     0x01
#define LL_FEATURE_CONNECTION_CTE_REQUEST              0x02  // support CTE request procedure as initiator
#define LL_FEATURE_CONNECTION_CTE_RESPONSE             0x04  // support CTE request procedure as responder
#define LL_FEATURE_CONNECTIONLESS_CTE_TRANSMITTER      0x08
#define LL_FEATURE_CONNECTIONLESS_CTE_RECEIVER         0x10
#define LL_FEATURE_ANTENNA_SWITCHING_DURING_CTE_TX     0x20  // support LL_FEATURE_RECEIVING_CTE and switching antennas for AoD
#define LL_FEATURE_ANTENNA_SWITCHING_DURING_CTE_RX     0x40  // support LL_FEATURE_RECEIVING_CTE and switching antennas for AoA
#define LL_FEATURE_RECEIVING_CTE                       0x80  // support receiving CTE in data PDUs and IQ sampling
// Byte 3
#define LL_FEATURE_PERIODIC_ADV_SYNC_TRANSFER_SEND     0x01
#define LL_FEATURE_PERIODIC_ADV_SYNC_TRANSFER_RECV     0x02
#define LL_FEATURE_SLEEP_CLOCK_ACCURACY_UPDATES        0x04
#define LL_FEATURE_REMOTE_PUBLIC_KEY_VALIDATION        0x08
#define LL_FEATURE_RESERVED4                           0x10
#define LL_FEATURE_RESERVED5                           0x20
#define LL_FEATURE_RESERVED6                           0x40
#define LL_FEATURE_RESERVED7                           0x80
// Byte 4 - Byte 7
#define LL_FEATURE_RESERVED0                           0x01
#define LL_FEATURE_RESERVED1                           0x02
#define LL_FEATURE_RESERVED2                           0x04
#define LL_FEATURE_RESERVED3                           0x08
#define LL_FEATURE_RESERVED4                           0x10
#define LL_FEATURE_RESERVED5                           0x20
#define LL_FEATURE_RESERVED6                           0x40
#define LL_FEATURE_RESERVED7                           0x80

//
// !!! Update the following defines in case of adding a new feature
//
#define LL_FEATURE_MAX_FEATURES_LSB                    0x0FFFFFFF // number of spec features 0 - 3 bytes (bits 28 - 31 are invalid)
#define LL_FEATURE_MAX_FEATURES_MSB                    0x00000000 // number of spec features 4 - 7 bytes (bits 0 - 31 are invalid)

// Feature Set Masks
// Note: BLE V5.0, Vol.6, Part B, Section 4.6, Table 4.4 indicates which bits
//       are valid between Controllers. Invalid bits are to be ignored upon
//       receipt from the peer Controller. These masks are logically "OR'ed"
//       with the peer's feature set upon exchange to ensure our Controller's
//       behavior remains unaffected by them.
#define LL_FEATURE_MASK_BYTE0                          (LL_FEATURE_PING                               |  \
                                                        LL_FEATURE_PRIVACY                            |  \
                                                        LL_FEATURE_EXTENDED_SCANNER_FILTER_POLICIES)
#define LL_FEATURE_MASK_BYTE1                          (LL_FEATURE_EXTENDED_ADVERTISING               |  \
                                                        LL_FEATURE_PERIODIC_ADVERTISING)
#define LL_FEATURE_MASK_BYTE2                          (LL_FEATURE_CONNECTIONLESS_CTE_TRANSMITTER     |  \
                                                        LL_FEATURE_CONNECTIONLESS_CTE_RECEIVER        |  \
                                                        LL_FEATURE_ANTENNA_SWITCHING_DURING_CTE_TX    |  \
                                                        LL_FEATURE_ANTENNA_SWITCHING_DURING_CTE_RX)
#define LL_FEATURE_MASK_BYTE3                          LL_FEATURE_REMOTE_PUBLIC_KEY_VALIDATION
#define LL_FEATURE_MASK_BYTE4                          LL_FEATURE_NONE
#define LL_FEATURE_MASK_BYTE5                          LL_FEATURE_NONE
#define LL_FEATURE_MASK_BYTE6                          LL_FEATURE_NONE
#define LL_FEATURE_MASK_BYTE7                          LL_FEATURE_NONE

// CM0 FW Parameters
#define CM0_RAM_BASE                                   0x21000028
#define CM0_RAM_EXT_DATA_LEN_ADDR                      (CM0_RAM_BASE + 162) // dataLenMask/maxDatalen
#define CM0_RAM_RX_IFS_TIMEOUT_ADDR                    (CM0_RAM_BASE + 166) // rxIfsTimeout
#define CM0_RAM_START_TO_TX_RAT_OFFSET_ADDR            (CM0_RAM_BASE + 32)  // startToTxRatOffset
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG)
// Note: This is a temporary workaround for CC26xxR2, which has values backwards.
#define LL_AUX_PTR_CA0_CA1                             0x0E49
#if defined(CC26X2) || defined(CC13X2) || defined(CC13X2P)
  // ALT: Use the following override: 0x0E490C83.
  #define CM0_RAM_CA0_CA1_OFFSET_ADDR                (CM0_RAM_BASE + 200) // auxPtrAccCa0/auxPtrAccCa1
#else // CC26XX_R2
  // ALT: Use the following override: 0x0E490823.
  #define CM0_RAM_CA0_CA1_OFFSET_ADDR                (CM0_RAM_BASE + 130) // auxPtrAccCa0/auxPtrAccCa1
#endif // device
#endif // AE_CFG

// Rx Ifs Timeout
#define LL_RF_RX_IFS_TIMEOUT                           0x10A6  // halfword write rxIfsTimeout
#define LL_RF_RX_IFS_DEFAULT_VAL                       0x03C0
#define LL_RF_RX_IFS_TIMEOUT_VAL                       0x02AD

//
// V4.2 - Extended Data length
//
#define LL_RF_DATA_LEN_WRITE_REG                       0x10A2  // halfword write dataLenMask and maxDataLen
#define LL_RF_DATA_LEN_DEFAULT_VAL                     0x1F1F
#define LL_RF_DATA_LEN_MAX_VAL                         0xFFFF
#define LL_RF_ADV_LEN_WRITE_REG                        0x10A4  // halfword write advLenMask and maxAdvLen
#define LL_RF_ADV_LEN_DEFAULT_VAL                      0x253F
#define LL_RF_ADV_LEN_MAX_VAL                          0xFFFF

//
// V5.0 - 2 Mbps and Coded PHY
//
#define LL_PHY_NUMBER_OF_PHYS                          3
#define LL_PHY_BASE_PHY                                LL_PHY_1_MBPS
#define LL_PHY_SLOWEST_PHY                             LL_PHY_1_MBPS //LL_PHY_CODED
#define LL_PHY_FASTEST_PHY                             LL_PHY_2_MBPS
#define LL_PHY_SUPPORTED_PHYS                          (LL_PHY_1_MBPS | LL_PHY_2_MBPS | LL_PHY_CODED)

//
// Control Procedure Flags
//

// Common Control Procedure Flags
#define CLEAR_ALL_FLAGS                                0x00
#define REJECT_EXT_IND_RECEIVED                        0x01
#define UNKNOWN_RSP_RECEIVED                           0x02
#define HOST_INITIATED                                 0x04
#define NOTIFY_HOST                                    0x08

// Use MIC In Payload
#define MIC_NOT_ENABLED                                0
#define MIC_ENABLED                                    1

// V4.1 - Connection Parameter Request Control Procedure Specific Flags
// ALT: REPLACE WITH ONE BYTE, USE MACROS
//uint8 connParamFlags;
//#define CONN_PARAM_CLEAR_ALL_FLAGS                   0x00
//#define CONN_PARAM_UNKNOWN_RSP_RECEIVED_FLAG         0x01
//#define CONN_PARAM_REJECT_EXT_IND_RECEIVED_FLAG      0x02
//#define CONN_PARAM_HOST_INITIATED_FLAG               0x04
//#define CONN_PARAM_REQ_RECEIVED_FLAG                 0x08
//#define CONN_PARAM_RSP_RECEIVED_FLAG                 0x10
//#define CONN_PARAM_UPDATE_ACTIVE_FLAG                0x20

// V4.2 - Extended Data Length Control Procedure Specific Flags
#define DISABLE_LEN_REQUEST                            0x10
#define LEN_RSP_RECEIVED                               0x20
#define UPDATE_LEN_RECEIVED                            0x40

// V4.2 - Privacy V1.2 Control Procedure Specific Flags

// V4.2 - Secure Connections Control Procedure Specific Flags

// V5.0 - 2M and Coded PHY Control Procedure Specific Flags
#define DISABLE_PHY_REQUEST                            0x10
#define PHY_RSP_RECEIVED                               0x20
#define UPDATE_PHY_RECEIVED                            0x40

/*
** Miscellaneous
*/
#define BITS_PER_BYTE                                  8
#define BYTES_PER_WORD                                 4

// HCI Connection Complete Roles
#define HCI_EVT_MASTER_ROLE                            0
#define HCI_EVT_SLAVE_ROLE                             1

// Channel Selection Algorithm
#define LL_CHANNEL_SELECT_ALGO_1                       0
#define LL_CHANNEL_SELECT_ALGO_2                       1

// TX Packet Last Packet Flag Values
// The RFHAL Data Entry specification allots four bits in the Config field for
// Partial Reads. Since BLE only uses Pointer Entries, these bits are ignored
// by the radio, and thus are not used. One bit can therefore be used to
// indicate if a completed fragmented packet is the last packet or not, so that
// packet completion can know when to send the Number of Completed Packets
// event to the Host.
//
// Warning: These bits are owned by radio firmware developers, who can change
//          the definition of the Config field, although unlikely in view of
//          keeping the spec backward compatible.
//
#define DATA_ENTRY_NOT_LAST_PACKET                     0x00
#define DATA_ENTRY_LAST_PACKET                         0x10

// BLE5 PHYs
#define BLE5_1M_PHY                                    0
#define BLE5_2M_PHY                                    1
#define BLE5_CODED_PHY                                 2
#define BLE5_RESERVED_PHY                              3
// Rx Status
#define BLE5_S8_PHY                                    2
#define BLE5_S2_PHY                                    3
//
#define BLE5_CODING_NONE                               0
#define BLE5_CODED_S8_DEFAULT                          0
#define BLE5_CODED_S2_DEFAULT                          1
#define BLE5_CODED_S8_DO_NOT_USE_LAST_PKT              0
#define BLE5_CODED_S8_USE_LAST_PKT                     1
#define BLE5_USE_DEFAULT_RATE_FOR_EMPTY_PKTS           0
#define BLE5_USE_CODDED_S8_FOR_EMPTY_PKTS              1
#define BLE5_USE_DEFAULT_RATE_FOR_RETRAN_PKTS          0
#define BLE5_USE_CODDED_S8_FOR_RETRAN_PKTS             1
//
#define BLE5_CODED_S2_SCHEME                           2
#define BLE5_CODED_S8_SCHEME                           8
//
#define BLE5_CODED_S8_PHY                              (BLE5_CODED_PHY | (BLE5_CODED_S8_DEFAULT << 2))
#define BLE5_CODED_S2_PHY                              (BLE5_CODED_PHY | (BLE5_CODED_S2_DEFAULT << 2))
//
#define BLE5_PHY_MASK                                  0x03

#if defined(CC13X2P)
// Override Registers
#define OVERRIDE_REG_HP_PA_RF_GAIN                     0
#define OVERRIDE_REG_TERMINATION                       0xFFFFFFFF
#endif // CC13X2P

// Connection Event Statuses
#define LL_CONN_EVT_STAT_SUCCESS                       0
#define LL_CONN_EVT_STAT_CRC_ERROR                     1
#define LL_CONN_EVT_STAT_MISSED                        2

#if defined(CC13X2P)
// Override Registers
#define OVERRIDE_REG_HP_PA_RF_GAIN                     0
#define OVERRIDE_REG_TERMINATION                       0xFFFFFFFF
#endif // CC13X2P

// RF FW write param command type
#define RFC_FWPAR_ADDRESS_TYPE_BYTE                    (0x03)
#define RFC_FWPAR_ADDRESS_TYPE_DWORD                   (0x00)

// RF FW write param command CTE address
#define RFC_FWPAR_CTE_CONFIG                           (187)   // 1 byte
#define RFC_FWPAR_CTE_SAMPLING_CONFIG                  (188)   // 1 byte
#define RFC_FWPAR_CTE_OFFSET                           (189)   // 1 byte
#define RFC_FWPAR_CTE_ANT_SWITCH                       (208)   // 4 bytes
#define RFC_FWPAR_CTE_AUTO_COPY                        (212)   // 4 bytes
#define RFC_FWPAR_CTE_INFO_TX_TEST                     (159)   // 1 byte

// RF param command CTE offset
#define RFC_CTE_CONFIG_OFFSET                          (2)
#define RFC_CTE_SAMPLING_CONFIG_OFFSET                 (3)
#define RFC_CTE_ANT_SWITCH_OFFSET                      (5)
#define RFC_CTE_AUTO_COPY_OFFSET                       (6)

// RF force clock command parameters
#define RFC_FORCE_CLK_DIS_RAM                          (0x0000)
#define RFC_FORCE_CLK_ENA_RAM_MCE                      (0x0010)
#define RFC_FORCE_CLK_ENA_RAM_RFE                      (0x0040)

// extFeatureMask in llConnExtraParams_t 
#define EXT_FEATURE_DISCONNECT_ENABLE                   BV(0)
#define EXT_FEATURE_DISCONNECT_DISABLE                 ~BV(0)

// DMM Policy invalid index
#define DMM_POLICY_INVALID_INDEX                       (0xFFFFFFFF)
#define DMM_POLICY_MAX_REPEAT_PRIORITIES               (2)

// Coex type
#define COEX_TYPE_1_WIRE_GRANT                         (0)
#define COEX_TYPE_1_WIRE_REQUEST                       (1)
#define COEX_TYPE_3_WIRE                               (2)

// CTE Samples task ID
#define CTE_TASK_ID_NONE                               (0)
#define CTE_TASK_ID_CONNECTION                         (1)
#define CTE_TASK_ID_CONNECTIONLESS                     (2)
#define CTE_TASK_ID_TEST                               (3)

/*******************************************************************************
 * TYPEDEFS
 */

/*
** Data PDU Control Packets
**
** The following structures are used to represent the various types of control
** packets. They are only used for recasting a data buffer for convenient
** field access
*/

// Connection Parameters
typedef struct
{
  uint8  winSize;                                    // window size
  uint16 winOffset;                                  // window offset
  uint16 connInterval;                               // connection interval
  uint16 slaveLatency;                               // number of connection events the slave can ignore
  uint16 connTimeout;                                // supervision connection timeout
} connParam_t;

// Flags for Connection Parameters Request Control Procedure
typedef struct
{
  uint8 hostInitiated;                               // flag to indicate the Host initiated the Update
  uint8 unknownRspRcved;                             // flag to indicate Unknown Response received
  uint8 rejectIndExtRcved;                           // flag to indicate Reject Ind Extended received
  uint8 connParamReqRcved;                           // flag to indicate Connection Parameter Request received
  //uint8 connParamRspRcved;                           // flag to indicate Connection Parameter Response received
  uint8 connUpdateActive;                            // flag to indicate a Connection Update Parameter or Update Channel procedure active
} connParamFlags_t;

// Connection Parameters Request or Response
typedef struct
{
  uint16 intervalMin;                                // lower connection interval limit
  uint16 intervalMax;                                // upper connection interval limit
  uint16 latency;                                    // slave latency
  uint16 timeout;                                    // connection timeout
  uint8  periodicity;                                // preferred periodicity
  uint16 refConnEvtCount;                            // reference connection event count
  uint16 offset0;                                    // offset 0
  uint16 offset1;                                    // offset 1
  uint16 offset2;                                    // offset 2
  uint16 offset3;                                    // offset 3
  uint16 offset4;                                    // offset 4
  uint16 offset5;                                    // offset 5
} connParamReq_t;

// Channel Map
typedef struct
{
  uint8 chanMap[ LL_NUM_BYTES_FOR_CHAN_MAP ];        // bit map corresponding to the data channels 0..39
} chanMap_t;

// Encryption Request
typedef struct
{
  uint8 RAND[LL_ENC_RAND_LEN];                       // random vector from Master
  uint8 EDIV[LL_ENC_EDIV_LEN];                       // encrypted diversifier from Master
  uint8 SKDm[LL_ENC_SKD_M_LEN];                      // master SKD values concatenated
  uint8 IVm[LL_ENC_IV_M_LEN];                        // master IV values concatenated
} encReq_t;

// Encryption Response
typedef struct
{
  uint8 SKDs[LL_ENC_SKD_S_LEN];                      // slave SKD values concatenated
  uint8 IVs[LL_ENC_IV_S_LEN];                        // slave IV values concatenated
} encRsp_t;

// Unknown Response
typedef struct
{
  uint8 unknownType;                                 // control type of the control
} unknownRsp_t;                                      // packet that caused was unknown

// Feature Request
typedef struct
{
  uint8 featureSet[ LL_MAX_FEATURE_SET_SIZE ];       // features that are used or not
} featureReq_t;

// Feature Response
typedef struct
{
  uint8 featureSet[ LL_MAX_FEATURE_SET_SIZE ];       // features that are used or not
} featureRsp_t;

// Version Information
typedef struct
{
  uint8  verNum;                                     // controller spec version
  uint16 comId;                                      // company identifier
  uint16 subverNum;                                  // implementation version
} verInfo_t;

/*
** Connection Data
**
** The following structures are used to hold the data needed for a LL
** connection.
*/

// Encryption
typedef struct
{
  // Note: IV and SKD provide enough room for the full IV and SKD. When the
  //       Master and Slave values are provided, the result is one combined
  //       (concatenated) value.
  uint8  IV[ LL_ENC_IV_LEN ];                        // combined master and slave IV values concatenated
  uint8  SKD [ LL_ENC_SKD_LEN ];                     // combined master and slave SKD values concatenated
  uint8  RAND[ LL_ENC_RAND_LEN ];                    // random vector from Master
  uint8  EDIV[ LL_ENC_EDIV_LEN ];                    // encrypted diversifier from Master
  uint8  nonce[ LL_ENC_NONCE_LEN ];                  // current nonce with current IV value
  uint8  SK[ LL_ENC_SK_LEN ];                        // session key derived from LTK and SKD
  uint8  LTK[ LL_ENC_LTK_LEN ];                      // Long Term Key from Host
  uint8  SKValid;                                    // flag that indicates the Session Key is valid
  uint8  LTKValid;                                   // Long Term Key is valid
  uint32 txPktCount;                                 // used for nonce formation during encryption (Note: 39 bits!)??
  uint32 rxPktCount;                                 // used for nonce formation during encryption (Note: 39 bits!)??
  // ALT: Could use one variable with one bit for each state.
  uint8  encRestart;                                 // flag to indicate if an encryption key change took place
  uint8  encRejectErrCode;                           // error code for rejecting encryption request
  uint8  startEncRspRcved;                           // flag to indicate the Start Request has been responded to
  uint8  pauseEncRspRcved;                           // flag to indicate the Pause Request has been responded to
  uint8  encReqRcved;                                // flag to indicate an Enc Req was received in a Enc Pause procedure
  uint8  encInProgress;                              // flag used to prevent a enc control procedure while one is already running

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
  uint8  startEncReqRcved;                           // flag to indicate the Start Request has been responded to
  uint8  rejectIndRcved;                             // flag to indicate the Start Encryption needs to be aborted
#endif // INIT_CFG
} encInfo_t;

// Feature Set Data
typedef struct
{
  uint8 featureRspRcved;                             // flag to indicate the Feature Request has been responded to
  uint8 featureSet[ LL_MAX_FEATURE_SET_SIZE ];       // features supported by this device
  uint8 featureSetMask[ LL_MAX_FEATURE_SET_SIZE ];   // peer features to be ignored
} featureSet_t;

// Connection Termination
typedef struct
{
  uint8 connId;                                      // connection ID
  uint8 termIndRcvd;                                 // indicates a TERMINATE_IND was received
  uint8 reason;                                      // reason code to return to Host when connection finally ends
} termInfo_t;

// Version Information Exchange
typedef struct
{
  uint8 peerInfoValid;                               // flag to indicate the peer's version information is valid
  uint8 hostRequest;                                 // flag to indicate the host has requested the peer's version information
  uint8 verInfoSent;                                 // flag to indicate this device's version information has been sent
} verExchange_t;

// Reject Indication Extended
typedef struct
{
  uint8 rejectOpcode;                                // opcode that was rejected
  uint8 errorCode;                                   // error code for rejection
} rejectIndExt_t;

// Control Procedure Information
typedef struct
{
  uint8  ctrlPktActive;                              // control packet at head of queue has been queued for Tx
  uint8  ctrlPkts[ LL_MAX_NUM_CTRL_PROC_PKTS ];      // queue of control packets to be processed
  uint8  ctrlPktCount;                               // number of queued control packets
  uint16 ctrlTimeoutVal;                             // timeout in CI events for control procedure for this connection
  uint16 ctrlTimeout;                                // timeout counter in CI events for control procedure
  uint8  ctrlPktPending;                             // type of received control packet whose processing has been delayed
} ctrlPktInfo_t;

// Packet Error Rate Information - General
typedef struct
{
  uint16 numPkts;                                    // total number of packets received
  uint16 numCrcErr;                                  // number of packets with CRC error
  uint16 numEvents;                                  // number of connection events
  uint16 numMissedEvts;                              // number of missed connection events
} perInfo_t;

// TX Data
typedef struct txData_t
{
  struct txData_t *pNext;                            // pointer to next Tx data entry on queue
  uint8            fragFlag;                         // packet boundary flag
  uint16           len;                              // data length
  uint8           *pData;                            // pointer to data payload
} txData_t;

// Data Packet Queue
typedef struct
{
  txData_t *head;                                    // pointer to the head of the data queue
  txData_t *tail;                                    // pointer to the tail of the data queue
  uint8     numEntries;                              // number of data queue entries
} llDataQ_t;

// Peer Device Information
typedef struct
{
  uint8     peerAddr[ LL_DEVICE_ADDR_LEN ];          // peer device address
  uint8     peerAddrType;                            // peer device address type of public or random
  uint8     reserved;
} peerInfo_t;

// PHY Information
typedef struct
{
  uint8 curPhy;                                      // current PHY
  uint8 updatePhy;                                   // PHY to update to
  uint8 phyFlags;                                    // control procedure flags
  uint8 phyPreference;                               // based on set PHY
  uint8 phyOpts;                                     // Coded phy options
} phyInfo_t;

// Length Information
typedef struct
{
  uint16 connMaxTxOctets;                            // max payload bytes sent by this device
  uint16 connMaxRxOctets;                            // max payload bytes this device can receive
  uint16 connMaxTxTime;                              // max time (us) this device will take to transmit payload
  uint16 connMaxRxTime;                              // max time (us) this device can take to receive payload
  uint16 connRemoteMaxTxOctets;                      // max payload bytes peer device will send
  uint16 connRemoteMaxRxOctets;                      // max payload bytes peer can receive
  uint16 connRemoteMaxTxTime;                        // max time (us) peer will take to transmit
  uint16 connRemoteMaxRxTime;                        // max time (us) peer can take to receive
  uint16 connEffectiveMaxTxOctets;                   // lesser of connMaxTxOctets and connRemoteMaxRxOctets
  uint16 connEffectiveMaxRxOctets;                   // lesser of connMaxRxOctets and connRemoteMaxTxOctets
  uint16 connEffectiveMaxTxTime;                     // lesser of connMaxTxTime and connRemoteMaxRxTime
  uint16 connEffectiveMaxRxTime;                     // lesser of connMaxRxTime and connRemoteMaxTxTime
  //
  uint16 connIntervalPortionAvail;                   // the current connection interval - C (see spec)
  uint16 connEffectiveMaxTxTimeAvail;                // lesser of connEffectiveMaxTxTimeUncoded and connIntervalPortionAvail
  uint16 connEffectiveMaxTxTimeCoded;                // greater of 2704 and connEffectiveMaxTxTimeAvailable
  uint16 connEffectiveMaxTxTimeUncoded;              // lesser of connMaxTxTime and connremoteMaxRxTime
  uint16 connEffectiveMaxRxTimeCoded;                // greater of 2704 and connEffectiveMaxRxTimeUncoded
  uint16 connEffectiveMaxRxTimeUncoded;              // lesser of connMaxRxTime and connRemoteMaxTxTime
  uint8  lenFlags;                                   // control procedure flags
  //
  uint16 connActualMaxTxOctets;                      // max paylod based on max Octets and max Time
  uint8  connSlowestPhy;                             // slowest PHY limits connRemoteMaxTxTime
} lenInfo_t;

// Privacy 1.2
typedef struct
{
  uint8  addrResolution;
  uint32 rpaTimeout;
} privInfo_t;

// Connection Data Type
// Note: Needed for forward reference to llConnState_t from chSelAlgo_t.
typedef struct llConn_t llConnState_t;

// Connection extra parameters type
typedef struct llConnExtraParams_t llConnExtParam_t;

// Data Channel Algorithm Function
typedef uint8 (*chSelAlgo_t)(llConnState_t *);

// Connection event report. Note that this is passed by reference through
// the host callback so the sizes of each element need to match Gap_ConnEventRpt_t
typedef struct
{
  uint8_t  status;   // status of connection event
  uint16_t handle;   // connection handle
  uint8_t  channel;  // BLE RF channel index (0-39)
  uint8_t  phy;      // PHY of connection event
  int8_t   lastRssi; // RSSI of last packet received
  // Number of packets received for this connection event.
  uint16_t packets;
  // Total number of CRC errors for this connection.
  uint16_t errors;
  uint8_t  nextTaskType; // Type of next BLE task
  uint32_t nextTaskTime; // time to next BLE task
} connEvtRpt_t;

// Callback function pointer type for Connection Event notifications
typedef void (*llConnEvtCB_t)
(
  connEvtRpt_t *pReport  // Connection Event Report
);

// Connection event callback reporting information
typedef struct
{
  llConnEvtCB_t cb;  // Host callback
  /// Connection handle to send connection events for. If 0xFFFF, send all.
  uint16_t handle;
} llConnEvtNotice_t;

// Connection extra params which are needed becuase of not being able to
// add new parameters to llConn_t because of the ROM.
struct llConnExtraParams_t
{
  uint16  size;
  uint16  pduSize;
  uint8   state;
  uint8 * pEntry;
  uint8   extFeatureMask;                 //external features can highjack this bitmap which is connection oriented
                                          //|   7..1   |              0               |
                                          //| reserved |  RPA not resolved disconnect |
                                          //|          |  with reason 0x05            |
  uint8   connPriority;                   // connection priority
  uint16  connMissCount;                  // connection miss count
  uint8   connMinTimeExternalUpdateInd:1; // connection external update indication for the minimum connection time.
  uint8   connMaxTimeExternalUpdateInd:1; // connection external update indication for the maximum connection time.
  uint32  connMinTimeLength:31;           // connection minimum time length
  uint32  connMaxTimeLength:31;           // connection maximum time length
  /* Starvation Handling */
  uint8   StarvationMode:1;               // connection starvation mode on/off
  uint8   numLSTORetries:3;               // connection number of retries in LSTO state
  uint8   reserved:4;					  // reserved
}; 

// Connection Data
struct llConn_t
{
  taskInfo_t       *llTask;                             // pointer to associated task block
  // General Connection Data
  uint8             allocConn;                          // flag to indicate if this connection is allocated
  uint8             activeConn;                         // flag to indicate if this connection is active
  uint8             connId;                             // connection ID
  uint16            currentEvent;                       // current event number
  uint16            nextEvent;                          // next active event number
  uint16            expirationEvent;                    // event at which the LSTO has expired
  uint16            expirationValue;                    // number of events to a LSTO expiration
  uint8             firstPacket;                        // flag to indicate when the first packet has been received
  uint16            scaFactor;                          // SCA factor for timer drift calculation
  uint32            timerDrift;                         // saved timer drift adjustment to avoid recalc
  // Connection Parameters
  uint32            lastTimeToNextEvt;                  // the time to next event from the previous connection event
  uint8             slaveLatencyAllowed;                // flag to indicate slave latency is permitted
  uint16            slaveLatency;                       // current slave latency; 0 means inactive
  uint8             lastSlaveLatency;                   // last slave latency value used
  uint16            slaveLatencyValue;                  // current slave latency value (when enabled)
  uint32            accessAddr;                         // saved synchronization word to be used by Slave
  uint32            crcInit;                            // connection CRC initialization value (24 bits)
  uint8             sleepClkAccuracy;                   // peer's sleep clock accuracy; used by own device to determine timer drift
  connParam_t       curParam;                           // current connection parameters
  // Channel Map
  uint8             nextChan;                           // the unmapped channel for the next active connection event
  uint8             currentChan;                        // the current unmapped channel for the completed connection event
  uint8             numUsedChans;                       // count of the number of usable data channels
  uint8             hopLength;                          // used for finding next data channel at next connection event
  uint8             chanMapTable[LL_MAX_NUM_DATA_CHAN]; // current chan map table that is in use for this connection
  chanMap_t         curChanMap;
  // TX Related
  uint8             txDataEnabled;                      // flag that indicates whether data output is allowed
  dataEntryQ_t     *pTxDataEntryQ;
  // RX Related
  uint8             rxDataEnabled;                      // flag that indicates whether data input is allowed
  dataEntryQ_t     *pRxDataEntryQ;
  uint8             lastRssi;                           // last data packet RSSI received on this connection
  // Control Packet Information
  ctrlPktInfo_t     ctrlPktInfo;                        // information for control procedure processing
  // Parameter Update Control Procedure
  uint8             pendingParamUpdate;                 // flag to indicate connection parameter update is pending
  uint16            paramUpdateEvent;                   // event count to indicate when to apply pending param update
  connParam_t       paramUpdate;                        // update parameters
  // Channel Map Update Control Procedure
  uint8             pendingChanUpdate;                  // flag to indicate connection channel map update is pending
  uint16            chanMapUpdateEvent;                 // event count to indicate when to apply pending chan map update
  // Encryption Data Control Procedure
  uint8             encEnabled;                         // flag to indicate that encryption is enabled for this connection
  encInfo_t         encInfo;                            // structure that holds encryption related data
  // Feature Set
  featureSet_t      featureSetInfo;                     // feature set for this connection
  // Version Information
  verExchange_t     verExchange;                        // version information exchange
  verInfo_t         verInfo;                            // peer version information
  // Termination Control Procedure
  termInfo_t        termInfo;                           // structure that holds connection termination data
  // Unknnown Control Packet
  uint8             unknownCtrlType;                    // value of unknown control type
  // Packet Error Rate
  perInfo_t         perInfo;                            // PER
  perByChan_t      *perInfoByChan;                      // PER by Channel
  // Peer Address
  // Note: Address must start on word boundary!
  peerInfo_t        peerInfo;                           // peer device address and address type
  // Connection Event Notification
  uint8             taskID;                             // user task ID to send task event
  uint16            taskEvent;                          // user event to send at end of connection event

  // TEMP: THIS BELONGS IN taskInfo_t, BUT THEN ANOTHER MALLOC IS NEEDED.
  uint32            lastTimeoutTime;

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
  uint8             updateSLPending;                    // flag to monitor Master confirmation of Slave's ACK for update
#endif // ADV_CONN_CFG

#ifndef DISABLE_RCOSC_SW_FIX
  // save off master contribution
  uint16            mstSCA;                             // Master's portion of connection SCA
#endif // !DISABLE_RCOSC_SW_FIX

  // Authenticated Payload Timeout
  uint32            aptoValue;                          // APTO value, in ms
  uint8             pingReqPending;                     // flag to indicate PING Request control procedure in progress
  uint8             numAptoExp;                         // number of 1/2 APTO expirations
  uint8             aptoTimerId;                        // cbTimer timer ID needed to stop the timer

  // Connection Parameter Control Procedure
  connParamFlags_t  connParamReqFlags;                  // flags for handling connection parameter request control procedure
  connParamReq_t    connParams;                         // connection parameters for Request and Response packets
  rejectIndExt_t    rejectIndExt;                       // Reject Indication Extended Sent

  uint16            numEventsLeft;                      // events left before LSTO expiration
  uint16            prevConnInterval;                   // saved curParam CI before overwritten by updateParam CI

  // V4.2
  uint8             pendingLenUpdate;
  lenInfo_t         lenInfo;

#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
  // V5.0
  uint8             pendingPhyUpdate;                   // flag to indicate a PHY update is pending
  uint16            phyUpdateEvent;                     // instant event for PHY update
  phyInfo_t         phyInfo;                            // PHY info for update
#endif // PHY_2MBPS_CFG | PHY_LR_CFG

#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & CHAN_ALGO2_CFG)
  chSelAlgo_t       pChSelAlgo;                         // function for data channel algorithm
#endif // CHAN_ALGO2_CFG
};

// Per BLE LL Connection
typedef struct
{
  uint8         numLLConns;                          // number of allocated connections
  uint8         numActiveConns;                      // number of allocated connections that are active
  uint8         currentConn;                         // the LL connection currently in use
  uint8         nextConn;                            // the next LL connection
  llConnState_t *llConnection;                       // connection state information
} llConns_t;

// Extra parameters per LL Connection
typedef struct
{
  llConnExtParam_t *llConnExtParamEntry;
} llConnsParam_t;

// Advertising Event Parameters
typedef struct
{
  taskInfo_t *llTask;                                // pointer to associated task block
  uint8       paramValid;                            // flag to indicate paraetmers are valid
  uint8       advMode;                               // flag to indicate if currently advertising
  uint16      advInterval;                           // the advertiser interval, based on advIntMin and advIntMax
  uint8       advEvtType;                            // connectable directed, undirected, discoverable, or non-connectable
  uint8       advChanMap;                            // saved Adv channel map; note, only lower three bits used
  uint8       wlPolicy;                              // white list policy for Adv
  uint8       ownAddrType;                           // own device address type of public or random
  // Note: Address must start on word boundary!
  uint8       ownAddr[ LL_DEVICE_ADDR_LEN ];         // own device address
  uint16      scaValue;                              // Slave SCA in PPM
  // Note: Address must start on word boundary!
  uint8       peerAddr[ LL_DEVICE_ADDR_LEN ];        // peer device address
  uint8       peerAddrType;                          // peer device address type of public or random
  // For Connectable Undirected, Discoverable, and Nonconnectable Events only
  uint8       advDataChanged;                        // advertiser data has changed during advertising
  uint8       activeAdvBuf;
  uint8       advDataLen;                            // advertiser data length
  uint8       advData[2][LL_MAX_ADV_DATA_LEN];
  // Scan Response Parameters
  uint8       scanDataChanged;                        // advertiser data has changed during advertising
  uint8       activeScanRspBuf;
  uint8       scanRspLen;                            // scan response data length
  uint8       scanRspData[2][LL_MAX_SCAN_DATA_LEN];
  // Adv Event Notification
  uint8       taskID;                                // user task ID to send task event
  uint16      taskEvent;                             // user event to send at end of Adv event
  //
  uint8       connId;                                // allocated connection ID
} advInfo_t;

// Scanner Event Parameters
typedef struct
{
  taskInfo_t *llTask;                                // pointer to associated task block
  uint8       paramValid;                            // flag to indicate paraetmers are valid
  uint8       scanMode;                              // flag to indicate if currently scanning
  uint16      scanInterval;                          // the interval between scan events
  uint16      scanWindow;                            // the duration of a scan event
  uint8       scanType;                              // passive or active scan
  uint8       ownAddrType;                           // own device address type of public or random
  // Note: Address must start on word boundary!
  uint8       ownAddr[ LL_DEVICE_ADDR_LEN ];         // own device address
  uint8       wlPolicy;                              // white list policy for Scan
  uint8       filterReports;                         // flag to indicate if duplicate Adv packet reports are to be filtered
} scanInfo_t;

// Initiator Event Parameters
typedef struct
{
  taskInfo_t *llTask;                                // pointer to associated task block
  uint8       initPending;                           // flag to indicate if Scan needs to be initialized
  uint8       scanMode;                              // flag to indicate if currently scanning
  uint16      scanInterval;                          // the interval between scan events
  uint16      scanWindow;                            // the duration of a scan event
  uint8       wlPolicy;                              // white list policy for Init
  uint8       ownAddrType;                           // own device address type of public or random
  // Note: Address must start on word boundary!
  uint8       ownAddr[ LL_DEVICE_ADDR_LEN ];         // own device address
  uint8       connId;                                // allocated connection ID
  uint8       scaValue;                              // Master SCA as an ordinal value for PPM
} initInfo_t;

// Direct Test Mode
typedef struct
{
  uint8       rfChan;                                // 0..39
  uint8       packetLen;                             // 0..39 bytes
  uint8       packetType;                            // data pattern
  uint16      numPackets;                            // number of packets received
  uint16      numRxCrcNOK;                           // number of packets received with CRC error
  uint8       lastRssi;                              // RSSI of last packet received
  uint16      txPktCnt;                              // number of tx packets to transmit
} dtmInfo_t;

// RF Patch Compensation
typedef struct
{
  int16       rfTxPathCompParam;                     // RF Tx Path Compensation Parameter
  int16       rfRxPathCompParam;                     // RF Rx Path Compensation Parameter
  int8        rfTxPathCompVal;                       // RF Tx Path Compensation Value (floor of parameter)
  int8        rfRxPathCompVal;                       // RF Rx Path Compensation Value (floor of parameter)
} rfPathComp_t;

// Build Revision
typedef struct
{
  uint16      userRevNum;                            // user revision number
} buildInfo_t;

// Size Info (for dynamic allocation)
typedef struct
{
  uint8  sizeOfTaskInfo;
  uint16 sizeOfLlConnState;
  uint8  sizeOfAdvInfo;
  uint8  sizeOfScanInfo;
  uint8  sizeOfInitInfo;
  uint8  sizeOfAeCBackTbl;
  uint8  sizeOfAeRfCmdSize;
  uint8  sizeOfAeAdvSetTerm;
  uint8  sizeOfAeExtAdvRptEvt;
  uint8  sizeOfDtmInfo;
  uint8  sizeOfConnEvtRpt;
} sizeInfo_t;

// spec 5.1 - CTE structs

//Antenna switch struct
typedef struct
{
  uint8       numEntries;                   //Number of IO value entries in the table. If this is less than the number of slots, the IO value entries are repeated in a circular manner
  uint8       switchTime;                   //1: 1 �s switching and sampling slots 2: 2 �s switching and sampling slots Others: Reserved
  uint16      reserved;
  uint32      ioMask;                       //Bit mask defining the DIOs used for antenna switching. A 1 indicates that the corresponding DIO is used
  uint32      ioEntry[1];                   //Entry defining the values of the DIOs used for the antenna switching for Sample slot #N. Only the bits corresponding to 1's in ioMask are used
} llCteAntSwitch_t;

// received CTE response info struct
typedef struct
{
  uint8       phy;                          // current PHY
  uint8       dataChIndex;                  // CTE response data channel index 
  uint8       packetStatus;                 // CTE response packet status (success or CRC error)
  uint8       rssiAntenna;                  // first antenna which rssi was measured on.
  int16       rssi;                         // rssi measured on received CTE response 
  uint16      connEvent;                    // connection event which CTE response was received
  uint8       length;                       // CTE length received from peer
  uint8       type;                         // CTE type received from peer                 
}llCteRecvInfo_t;

// CTE sample configuration struct
typedef struct
{
  uint8       sampleRate1M;                 // CTE sample rate for 1Mbps phy
  uint8       sampleRate2M;                 // CTE sample rate for 2Mbps phy
  uint8       sampleSize1M;                 // CTE sample size for 1Mbps phy
  uint8       sampleSize2M;                 // CTE sample size for 2Mbps phy
  uint8       sampleCtrl;                   // CTE sample control flags (bit0-RAW_RF(no filtering), ...)
}llCteSampleConfig_t;

//CTE request info struct
typedef struct
{
  uint8       samplingEnable;               // CTE sampling enable received from Host
  uint8       requestEnable;                // CTE request enable received from Host
  uint8       requestLen;                   // CTE request length received from Host
  uint8       requestType;                  // CTE request type received from Host                 
  uint8       sendRequest;                  // flag indicates controller send CTE request to peer
  uint8       recvCte;                      // flag indicates received CTE from peer
  uint16      requestInterval;              // CTE periodic received from Host
  uint32      periodicEvent;                // connection event to send CTE request
  llCteSampleConfig_t sampleConfig;         // CTE sample Host configuration
  llCteRecvInfo_t recvInfo;                 // CTE response info received
  llCteAntSwitch_t *pAntenna;               // antenna switch pattern for AoA
} llCteInitiator_t;

//CTE response info struct
typedef struct
{
  uint8       responseConfig;               // flag indicates that CTE Transmit Params was set by Host
  uint8       responseEnable;               // CTE response enable received from Host
  uint8       supportedTypes;               // supported CTE types received from Host
  uint8       type;                         // CTE type received in CTE request control command
  uint8       len;                          // CTE length received in CTE request control command
  llCteAntSwitch_t *pAntenna;               // antenna switch pattern for AoD
} llCteResponder_t;

// CTE struct
typedef struct
{
  llCteInitiator_t   initiator;             // initiate the CTE request
  llCteResponder_t   responder;             // respond to the CTE request
} llCte_t;

// IQ Samples RF Header struct
typedef struct
{
  uint16                     length;
  uint8                      cteInfo;
  rfc_statusIqSamplesEntry_t status;
  uint8                      rfGain;
  uint8                      rssi;
  uint8                      padding[2];
} llCteSamplesRfHeader_t;

// CTE IQ Samples struct
typedef struct
{
  dataQ_t             queue;               // Auto Copy buffer queue
  rfc_iqAutoCopyDef_t autoCopy;            // Auto Copy RF struct
  dataEntry_t         *pAutoCopyBuffers;   // pointer to the allocated auto copy buffers
  uint8               autoCopyCompleted;   // Counter indicates that RF finished copy the samples
} llCteSamples_t;

// CTE Test struct
typedef struct
{
  uint8       testMode;                     // flag indicates that CTE Test Mode was set by Host
  uint8       inProgress;                   // flag indicates about processing received CTE
  uint8       recvCte;                      // flag indicates received CTE form peer
  uint8       type;                         // CTE type received in RX test HCI command
  uint8       length;                       // CTE length received in RX test HCI command
  llCteAntSwitch_t *pAntenna;               // antenna switch pattern for AoA
} llCteTest_t;

// DMM Policy feature
typedef struct
{
  uint32 time;                                // time passed from last successful transmission
  uint8  aborts;                              // number of consecutive aborted commands
} dmmPolicyManagerThreshold_t;

typedef struct
{
  dmmPolicyManagerThreshold_t  *adv;          // array of advertise handle (maxSupportedAdvSets)
  dmmPolicyManagerThreshold_t  *conn;         // array of connection handle (maxNumConns)
  dmmPolicyManagerThreshold_t  init;          // create connection (1 instant)
  dmmPolicyManagerThreshold_t  scan;          // scan (1 instant)
  uint8                        *advHandle;    // keep the advertise handle per array index
  uint8                        *connRepeatPrio[DMM_POLICY_MAX_REPEAT_PRIORITIES];
                                              // number of consecutive commands per connection with the same priority
                                              //index [0] for high priority and index [1] for urgent priority
} dmmPolicyManager_t;

// Coex Parameters struct
typedef struct
{
  RF_PriorityCoex priority;           // Coex priority (low/high)
  RF_RequestCoex  request;            // Request for Rx operation (yes/no)
} llCoexParams_t;

// Coex Counters struct for debug purpose
typedef struct
{
  uint32  grants;             // count success
  uint32  rejects;            // count number of rejects (no grant)
  uint16  contRejects;        // count continuously rejected requests
  uint16  maxContRejects;     // count max continuously rejected requests
} llCoexCount_t;


// Coex struct
typedef struct
{
  llCoexParams_t  connected;    // master or slave
  llCoexParams_t  initiator;    // create connection or connectable advertiser
  llCoexParams_t  broadcaster;  // non connectable advertiser
  llCoexParams_t  observer;     // scanner
  llCoexCount_t   counter;      // for debug use
  uint8           enable;       // feature enable/disable
  uint8           type;         // Coex 3 wire or 1 wire
} llCoex_t;

// Link Layer Test Mode
#ifdef LL_TEST_MODE

#define LL_TEST_MODE_TP_CON_MAS_BV_19                0
#define LL_TEST_MODE_TP_CON_MAS_BV_26                1
#define LL_TEST_MODE_TP_CON_MAS_BV_28                2
#define LL_TEST_MODE_TP_CON_MAS_BV_31_1              3
#define LL_TEST_MODE_TP_CON_MAS_BV_31_2              4
#define LL_TEST_MODE_TP_CON_MAS_BV_31_3              5
#define LL_TEST_MODE_TP_CON_MAS_BV_32                6
#define LL_TEST_MODE_TP_CON_MAS_BV_33                7
#define LL_TEST_MODE_TP_CON_MAS_BI_02                8
#define LL_TEST_MODE_TP_CON_MAS_BI_04                9
#define LL_TEST_MODE_TP_CON_MAS_BI_06                10
#define LL_TEST_MODE_TP_CON_SLA_BV_26                11
#define LL_TEST_MODE_TP_CON_SLA_BV_28                12
#define LL_TEST_MODE_TP_CON_SLA_BV_30_1              13
#define LL_TEST_MODE_TP_CON_SLA_BV_30_2              14
#define LL_TEST_MODE_TP_CON_SLA_BV_30_3              15
#define LL_TEST_MODE_TP_CON_SLA_BV_31                16
#define LL_TEST_MODE_TP_CON_SLA_BV_32                17
#define LL_TEST_MODE_TP_CON_SLA_BV_33                18
#define LL_TEST_MODE_TP_CON_SLA_BV_34                19
#define LL_TEST_MODE_TP_CON_SLA_BI_02                20
#define LL_TEST_MODE_TP_CON_SLA_BI_04                21
#define LL_TEST_MODE_TP_CON_SLA_BI_05                22
#define LL_TEST_MODE_TP_CON_SLA_BI_06                23
#define LL_TEST_MODE_TP_CON_SLA_BI_08                24
#define LL_TEST_MODE_TP_SEC_MAS_BV_08                25
#define LL_TEST_MODE_TP_SEC_SLA_BV_08                26
#define LL_TEST_MODE_TP_ENC_ADV_BI_02                27
#define LL_TEST_MODE_TP_TIM_SLA_BV_05                28
#define LL_TEST_MODE_TP_SEC_MAS_BV_14                29
#define LL_TEST_MODE_TP_SEC_SLA_BI_05                30
#define LL_TEST_MODE_TP_SEC_MAS_BI_07                31
#define LL_TEST_MODE_TP_SEC_MAS_BV_12                32
#define LL_TEST_MODE_TP_SEC_MAS_BV_13                33
#define LL_TEST_MODE_TP_SEC_MAS_BV_04                34
#define LL_TEST_MODE_TP_SEC_MAS_BI_09                35
#define LL_TEST_MODE_TP_SEC_MAS_BI_05                36
#define LL_TEST_MODE_TP_SEC_MAS_BI_04                37
#define LL_TEST_MODE_TP_SEC_MAS_BI_06                38
// V5.0
#define LL_TEST_MODE_TP_CON_SLA_BV_05                50
#define LL_TEST_MODE_TP_CON_SLA_BV_06                51
#define LL_TEST_MODE_TP_CON_SLA_BV_45                52
#define LL_TEST_MODE_TP_CON_SLA_BV_51                53
#define LL_TEST_MODE_TP_CON_MAS_BV_46                54
#define LL_TEST_MODE_TP_CON_SLA_BI_09                55
#define LL_TEST_MODE_TP_SEC_MAS_BV_03                56
// ESR11
#define LL_TEST_MODE_TP_PAC_SLA_BV01                 57
#define LL_TEST_MODE_TP_PAC_MAS_BV01                 58
#define LL_TEST_MODE_TP_PAC_SLA_BI01                 59
#define LL_TEST_MODE_TP_PAC_MAS_BI01                 60
#define LL_TEST_MODE_TP_SEC_MAS_BV05                 66
//
#define LL_TEST_MODE_TP_CON_MAS_BV03                 61
#define LL_TEST_MODE_TP_CON_MAS_BV04                 62
#define LL_TEST_MODE_TP_CON_MAS_BV05                 63
#define LL_TEST_MODE_TP_CON_SLA_BV04                 64
#define LL_TEST_MODE_TP_CON_SLA_BV05                 65
// TS V5.0.2
#define LL_TEST_MODE_TP_CON_SLA_BV59                 66
// TS V5.0.3
#define LL_TEST_MODE_TP_CON_SLA_BV69                 67
#define LL_TEST_MODE_TP_CON_MAS_BV65                 68
#define LL_TEST_MODE_TP_CON_INI_BV03                 69
#define LL_TEST_MODE_TP_DDI_SCN_BV36                 70
// Tickets
#define LL_TEST_MODE_JIRA_220                        200
#define LL_TEST_MODE_MISSED_SLV_EVT                  201
#define LL_TEST_MODE_JIRA_2756                       202
#define LL_TEST_MODE_JIRA_3478                       203
#define LL_TEST_MODE_JIRA_3646                       204
#define LL_TEST_MODE_INVALID                         0xFF

typedef struct
{
  uint8  testCase;                                   // Core Test Spec Test Case
  uint32 counter;                                    // General purpose counter.
} llTestMode_t;

#endif // LL_TEST_MODE

// Invalid System Boot Message
#define INVALID_SYSBOOTMSG (uint8 *)0xFFFFFFFF

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

// RF open parameter to specify PRCM Mode and pointers to CPE/MCE/RFE patches
extern RF_Mode      rfMode;

// RF Setup
#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)))
extern rfOpCmd_RadioSetup_t rfSetup;
#endif // !PHY_2MBPS_CFG & !PHY_LR_CFG


// FW Parameter structure (for Extended Data Length)
extern rfOpImmedCmd_RW_FwParam_t fwParCmd;
extern rfOpCmd_runImmedCmd_t     runFwParCmd;

// Device Addresses
extern uint8 ownPublicAddr[];
extern uint8 ownRandomAddr[];
//
extern uint8         LL_TaskID;                       // link layer task ID
extern uint8         llState;                         // link layer state
extern llConns_t     llConns;                         // link layer connection table
extern llConnsParam_t llConnExtParam;                  // link layer conn extra parameters table
extern verInfo_t     verInfo;                         // own version information
//extern peerInfo_t    peerInfo;                        // peer device related data
extern advInfo_t     *advInfo;                        // advertiser data
extern scanInfo_t    *scanInfo;                       // scan data
extern initInfo_t    *initInfo;                       // initiator data
extern dtmInfo_t     *dtmInfo;                        // direct test mode data
extern sizeInfo_t    sizeInfo;                        // size info of various data structs
extern buildInfo_t   buildInfo;                       // build revision data
extern featureSet_t  deviceFeatureSet;                // device feature set
extern uint8         curTxPowerVal;                   // current Tx Power value
extern uint8         maxTxPwrForDTM;                  // max power override for DTM
extern rfPathComp_t *pRfPathComp;                     // RF Tx Path Compensation data
extern uint16        taskEndStatus;                   // radio task end status
extern uint16        postRfOperations;                // flags for post-RF operations
extern int8          rssiCorrection;                  // RSSI correction
extern uint8         onePktPerEvt;                    // one packet per event enable flag
extern uint8         fastTxRespTime;                  // fast TX response time enable flag
extern uint8         rxFifoFlowCtrl;                  // flag to indicate if RX Flow Control is enabled
extern uint8         slOverride;                      // flag for user suspension of SL
extern uint8         numComplPkts;                    // number of completed Tx buffers
extern uint8         numComplPktsLimit;               // minimum number of completed Tx buffers before event
extern uint8         numComplPktsFlush;               // flag to indicate send number of completed buffers at end of event
#if defined( CC26XX ) || defined( CC13XX )
extern uint16        rfCfgAdiVal;                     // RF Config Value for ADI init
#endif // CC26XX/CC13XX
extern uint8         scanReqRpt;                      // Scan Request Report flag
#if defined(CC13X2P)
extern uint8         txPwrRfGainReg;                  // index into common override register table for HP PA RF Gain
#endif // CC13X2P

#ifndef DISABLE_RCOSC_SW_FIX
// pointer to the sclkSrc, as defined in the CCFG (for RCOSC workaround)
extern uint8 *sclkSrc;
#endif // !DISABLE_RCOSC_SW_FIX

// V4.1 - LL Topology
extern uint8 *activeConns;

extern uint16 *LSTORecoveryArr;

// System Boot Message
extern uint8 *SysBootMsg;

// V4.2 - Extended Data Length
extern uint16 connInitialMaxTxOctets;
extern uint16 connInitialMaxTxTime;
extern uint16 connInitialMaxTxTimeUncoded;
extern uint16 connInitialMaxTxTimeCoded;
//
extern uint16 supportedMaxTxOctets;
extern uint16 supportedMaxTxTime;
extern uint16 supportedMaxRxOctets;
extern uint16 supportedMaxRxTime;

// V5.0 - 2M and Coded PHY
extern uint8 defaultPhy;

#ifdef LL_TEST_MODE
extern llTestMode_t llTestMode;                      // LL Test Mode test cases
//
extern volatile uint8 firstTx;
extern volatile uint8 timSlvBv05Done;
extern volatile uint8 numSets;
extern volatile uint8 numTxPkts;
extern volatile uint8 nomCI;
extern volatile uint8 numTxEvts;
extern volatile uint8 setFailed;
extern volatile uint8 numFailedSets;
extern volatile uint8 numFailedTx;
#endif // LL_TEST_MODE

// Host Connection Event Notice Callback
extern llConnEvtNotice_t llConnEvtNotice;

// TRNG handle
extern TRNG_Handle trngHandle;

//CTE struct
extern llCte_t *llCte;
extern llCteSamples_t llCteSamples;
extern llCteTest_t llCteTest;


// DMM Policy feature
extern dmmPolicyManager_t dmmPolicyManager;

// Remote feature set cache
#ifdef QUAL_TEST
extern featureRsp_t *remoteFeatureSet;
#endif

// Current Mapped Channel
extern uint8 *llCurrentMappedChan;

// Coex feature
extern llCoex_t llCoex;

// QOS PARAMETERS
//***************
// Qos default parameters
extern uint8  qosDefaultPriorityConnParameter;
extern uint8  qosDefaultPriorityAdvParameter;
extern uint8  qosDefaultPriorityScnParameter;
extern uint8  qosDefaultPriorityInitParameter;
extern uint8  qosDefaultPriorityPerAdvParameter;
extern uint8  qosDefaultPriorityPerScnParameter;
extern uint8  defaultChannelMap[LL_NUM_BYTES_FOR_CHAN_MAP];

/*******************************************************************************
 * FUNCTIONS
 */

// Taskend Jump Tables
extern void (*taskEndAction)( void );

/*
** Link Layer Common
*/

// RF Management
extern void                 llRfSetup( uint8 );
extern void                 llRfInit( void );
extern void                 llResetRadio( void );
extern void                 llHaltRadio( uint16 );
extern void                 llRfStartFS( uint8, uint16 );
extern void                 llSetFreqTune( uint8 );
extern void                 llProcessPostRfOps( void );
extern void                 llSetTxPower( uint8 );
extern void                 llSetTxPwrLegacy( uint8 );
extern int8                 llGetTxPower( void );
extern uint8                llTxPwrPoutLU( int8 );
extern uint8                llTxPwrLU( uint16 );
extern void                 llTxPwrSetRfGainIndex( uint32 *);
extern void                 llTxPwrSwitchPA( uint8, uint32 *);
extern void                 llTxPwrSetRfCmdType(uint8 *, bool);
extern void                 llExtendRfRange( void );
extern void                 llGetTimeToStableXOSC( void );
extern void                 llRfSetupFwParamCmd( uint8, uint8, uint32, rfOpCmd_t *);
extern void                 llRfOverrideCteValue(uint32, uint16 , uint8 );
extern void                 llRfOverrideCommonValue(uint32,uint8);
//
extern RF_EventMask         rfEvent;
extern RF_Handle            rfHandle;
extern RF_CmdHandle         rfCmdHandle;

// Control Procedure Setup
extern uint8                llSetupUpdateParamReq( llConnState_t * );           // M
extern uint8                llSetupUpdateChanReq( llConnState_t * );            // M
extern uint8                llSetupEncReq( llConnState_t * );                   // M
extern uint8                llSetupEncRsp( llConnState_t * );                   // S
extern uint8                llSetupStartEncReq( llConnState_t * );              // S
extern uint8                llSetupStartEncRsp( llConnState_t * );              // M, S
extern uint8                llSetupPauseEncReq( llConnState_t * );              // M
extern uint8                llSetupPauseEncRsp( llConnState_t * );              // S
extern uint8                llSetupRejectInd( llConnState_t * );                // S
extern uint8                llSetupFeatureSetReq( llConnState_t * );            // M, S
extern uint8                llSetupFeatureSetRsp( llConnState_t * );            // M, S
extern uint8                llSetupVersionIndReq( llConnState_t * );            // M
extern uint8                llSetupTermInd( llConnState_t * );                  // M, S
extern uint8                llSetupUnknownRsp( llConnState_t * );               // M, S
extern uint8                llSetupPingReq( llConnState_t * );                  // M, S
extern uint8                llSetupPingRsp( llConnState_t * );                  // M, S
extern uint8                llSetupConnParamReq( llConnState_t * );             // M, S
extern uint8                llSetupConnParamRsp( llConnState_t * );             //  , S
extern uint8                llSetupRejectIndExt( llConnState_t * );             // M, S
//
extern uint8                llSetupPhyCtrlPkt( llConnState_t *, uint8 );        // M, S
extern uint8                llSetupLenCtrlPkt( llConnState_t *, uint8 );        // M, S
extern uint8                llSetupCte( llConnState_t *, uint8 );               // M, S

// Control Procedure Management
extern void                 llEnqueueCtrlPkt( llConnState_t *, uint8 );
extern void                 llDequeueCtrlPkt( llConnState_t * );
extern void                 llReplaceCtrlPkt( llConnState_t *, uint8, uint8);
extern uint8                llMoveCtrlPkt( llConnState_t *, uint8 *, uint8 * );
extern uint8                llMoveBackCtrlPkt( llConnState_t *, uint8 *, uint8 );
extern void                 llSendReject( llConnState_t *, uint8, uint8 );
extern uint8                llPendingUpdateParam( void );
extern void                 llInitFeatureSet( void );
extern void                 llRemoveFromFeatureSet( uint8 feature );
extern void                 llConvertCtrlProcTimeoutToEvent( llConnState_t * );
extern uint8                llVerifyConnParamReqParams( uint16, uint16, uint16, uint8, uint16, uint16 *);
extern uint8                llValidateConnParams( llConnState_t *, uint16, uint16, uint16, uint16, uint16, uint8, uint16, uint16 *);
extern void                 llUpdateCteState( llConnState_t *);
extern uint8                llGetCteInfo( uint8, void * );
extern uint8                llSetCteAntennaArray(llCteAntSwitch_t *, uint8 *, uint8 , uint8);
extern void                 llApplyParamUpdate( llConnState_t * );

// Data Channel Management
extern void                 llProcessChanMap( llConnState_t *, uint8 * );
extern uint8                llGetNextDataChan( llConnState_t *, uint16 );
extern uint8                llGetNextDataChanAlgo1( llConnState_t * );
extern uint8                llGetNextDataChanAlgo2( llConnState_t * );
extern uint8                llReverseBits( uint8 );
extern uint16               llGenPrnE( uint16, uint16 );
extern void                 llSetNextDataChan( llConnState_t * );
extern uint8                llAtLeastTwoChans( uint8 * );
extern uint8                llGetSlowestPhy( uint8 );

// Connection Management
extern llConnState_t        *llAllocConnId( void );
extern void                 llReleaseConnId( llConnState_t * );
extern void                 llReleaseAllConnId( void );
extern uint16               llGetMinCI( uint16  );
extern uint16               llFindNextActiveConnId( uint16 );
extern uint8                llGetNextConn( void );
extern uint16               llGetLstoNumOfEventsLeftMargin( uint16 );
extern uint8                llSetStarvationMode( uint16 , uint8 );
extern void                 llRealignConn( llConnState_t *, uint32 );
extern void                 llSortActiveConns( uint8 *, uint8 );
extern void                 llShellSortActiveConns(uint8 *activeConns, uint8 numActiveConns);
extern void                 llConnCheckCleanRxQueue( llConnState_t * );
extern void                 llConnCleanup( llConnState_t * );
extern void                 llConnTerminate( llConnState_t *, uint8  );
extern uint8                llConnExists( uint8, uint8 *, uint8 );
extern uint32               llGenerateCRC( void );
extern uint8                llEventInRange( uint16 , uint16 , uint16  );
extern uint16               llEventDelta( uint16 , uint16  );
extern void                 llConvertLstoToEvent( llConnState_t *, connParam_t * );
extern uint8                llAdjustForMissedEvent( llConnState_t *, uint32  );
extern void                 llAlignToNextEvent( llConnState_t *connPtr );
extern void                 llGetAdvChanPDU( uint8 *, uint8 *, uint8 *, uint8 *, uint8 *, int8 * );
// Access Address
extern uint32               llGenerateValidAccessAddr( void );
extern uint8                llValidAccessAddr( uint32 );
extern uint8                llGtSixConsecZerosOrOnes( uint32 );
extern uint8                llEqSynchWord( uint32 );
extern uint8                llOneBitSynchWordDiffer( uint32 );
extern uint8                llEqualBytes( uint32 );
extern uint8                llGtTwentyFourTransitions( uint32 );
extern uint8                llLtTwoChangesInLastSixBits( uint32 );
extern uint8                llEqAlreadyValidAddr( uint32  );
extern uint8                llLtThreeOnesInLsb( uint32 );
extern uint8                llGtElevenTransitionsInLsh( uint32 );
// Data Management
extern uint8                llEnqueueDataQ( llDataQ_t *, txData_t * );
extern uint8                llEnqueueHeadDataQ( llDataQ_t *, txData_t * );
extern uint8                llDequeueDataQ( llDataQ_t *, txData_t ** );
extern uint8                llDataQFull( llDataQ_t * );
extern uint8                llDataQEmpty( llDataQ_t * );
extern void                 llProcessTxData( llConnState_t *, uint8 );
extern uint8                llWriteTxData( llConnState_t *, uint8 *, uint8 , uint8, uint8 );
extern void                 llCombinePDU( uint16, uint8 *, uint16, uint8 );
extern uint8                llFragmentPDU( llConnState_t *, uint8 *, uint16 );
extern uint8                *llMemCopySrc( uint8 *, uint8 *, uint8 );
extern uint8                *llMemCopyDst( uint8 *, uint8 *, uint8 );
extern void                 llCreateRxBuffer( llConnState_t *, dataEntry_t *);
extern void                 llCheckRxBuffers( llConnState_t *connPtr );
extern void                 llReplaceRxBuffers( llConnState_t * );
extern uint16               llTime2Octets( uint8, uint8, uint16, uint8 );
extern uint32               llOctets2Time( uint8, uint8, uint16, uint8 );
extern uint16               llSetCodedMaxTxTime( llConnState_t * );
extern uint8                llVerifyCodedConnInterval( llConnState_t *, uint16 );
extern llConnState_t        *llDataGetConnPtr( uint8 );
extern llConnExtParam_t     *llGetConExtPtr( uint8 );

extern llStatus_t           llDynamicAlloc( void );
extern void                 llDynamicFree( void );

// Failure Management
extern void                 llHardwareError( uint8 );

// Advertising Task End Cause
extern void                 llDirAdv_TaskEnd( void );
extern void                 llAdv_TaskEnd( void );
extern void                 llAdv_TaskConnect( void );
extern void                 llAdv_TaskAbort( void );

// Scanner Task End Cause
extern void                 llScan_TaskEnd( void );
extern void                 llProcessScanRxFIFO( uint8 scanStatus );

// Initiator Task End Cause
extern void                 llInit_TaskConnect( void );
extern void                 llInit_TaskEnd( void );

// Master Task End Cause
extern void                 llMaster_TaskEnd( void );
extern uint8                llProcessMasterControlProcedures( llConnState_t *connPtr );
extern uint8                llSetupNextMasterEvent( void );

// Slave Task End Cause
extern void                 llSlave_TaskEnd( void );
extern uint8                llSetupNextSlaveEvent( void );
extern uint8                llProcessSlaveControlProcedures( llConnState_t * );
extern uint8                llCheckForLstoDuringSL( llConnState_t * );

// Error Related End Cause
extern void                 llTaskError( void );

// White List Related
extern llStatus_t           llCheckWhiteListUsage( void );

// Timer Related Management
extern void                 llCBTimer_AptoExpiredCback( uint8 * );

// Connection Event Notice
extern void                 llRegisterConnEvtCallback( llConnEvtCB_t cb, uint16_t connHandle );

// DMM policy
extern void                 llDmmSetThreshold(uint8 state, uint8 handle, uint8 reset);
extern uint32               llDmmGetActivityIndex(uint16 cmdNum);
extern uint8                llDmmSetAdvHandle(uint8 handle, uint8 clear);
extern void                 llDmmDynamicFree(void);
extern llStatus_t           llDmmDynamicAlloc(void);

#ifdef __cplusplus
}
#endif

#endif /* LL_COMMON_H */
