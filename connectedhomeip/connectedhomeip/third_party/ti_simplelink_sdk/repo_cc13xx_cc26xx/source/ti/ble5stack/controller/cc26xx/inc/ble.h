/******************************************************************************

 @file  ble.h

 @brief This file contains the data structures and APIs for CC26xx
        RF Core Firmware Specification for Bluetooth Low Energy.

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

#ifndef BLE_H
#define BLE_H

/*******************************************************************************
 * INCLUDES
 */

#include "rf_hal.h"
#include "ll_wl.h"
#include "ll_common.h"
#include "ll_config.h"

/*******************************************************************************
 * CONSTANTS
 */

// Mailbox CPE Interrupts for BLE
#define MB_NO_INT                      0
#define MB_TX_DONE_INT                 BV(4)
#define MB_TX_ACK_INT                  BV(5)
#define MB_TX_CTRL_INT                 BV(6)
#define MB_TX_CTRL_ACK_INT             BV(7)
#define MB_TX_CTRL_ACK_ACK_INT         BV(8)
#define MB_TX_RETRANS_INT              BV(9)
#define MB_TX_ENTRY_DONE_INT           BV(10)
#define MB_TX_BUF_CHANGED_INT          BV(11)
#define MB_RX_OK_INT                   BV(16)
#define MB_RX_NOK_INT                  BV(17)
#define MB_RX_IGNORED_INT              BV(18)
#define MB_RX_EMPTY_INT                BV(19)
#define MB_RX_CTRL_INT                 BV(20)
#define MB_RX_CTRL_ACK_INT             BV(21)
#define MB_RX_BUF_FULL_INT             BV(22)
#define MB_RX_ENTRY_DONE_INT           BV(23)
#if defined( CC26XX ) || defined( CC13XX )
#define MB_RX_DATA_WRITTEN             BV(24)
#define MB_RX_N_DATA_WRITTEN           BV(25)
#define MB_RX_ABORTED                  BV(26)
#endif // CC26XX/CC13XX

// Mailbox Hardware Interrupts for BLE
#define MB_HW_RAT_CHAN_5_INT           RAT_CHAN_5_IRQ
#define MB_HW_RAT_CHAN_6_INT           RAT_CHAN_6_IRQ
#define MB_HW_RAT_CHAN_7_INT           RAT_CHAN_7_IRQ

// Mailbox BLE Immediate Commands
#define CMD_BLE_ADV_PAYLOAD            0x1001

// Rf Command Legacy vs Rf Command BLE5 for the Tx power configurations
#define TX_PWR_CMD_BLE5                 0x0  // For all BLE5 Rf commands
#define TX_PWR_CMD_LEGACY               0x1  // For all Legacy Rf commands
#define TX_PWR_CMD_POS_BIT              0x7  // Bit 7 would indicate the type of Rf command

// Mailbox BLE Direct Commands

// Mailbox BLE Radio Commands
#define CMD_BLE_SLAVE                  0x1801
#define CMD_BLE_MASTER                 0x1802
#define CMD_BLE_ADV                    0x1803
#define CMD_BLE_ADV_DIR                0x1804
#define CMD_BLE_ADV_NC                 0x1805
#define CMD_BLE_ADV_SCAN               0x1806
#define CMD_BLE_SCANNER                0x1807
#define CMD_BLE_INITIATOR              0x1808
#define CMD_BLE_RX_TEST                0x1809
#define CMD_BLE_TX_TEST                0x180A

// Mailbox BLE5 Radio Commands
#define CMD_BLE5_RADIO_SETUP           0x1820
#define CMD_BLE5_SLAVE                 0x1821
#define CMD_BLE5_MASTER                0x1822
#define CMD_BLE5_ADV_EXT               0x1823
#define CMD_BLE5_ADV_AUX               0x1824
#define CMD_BLE5_SCANNER               0x1827
#define CMD_BLE5_INITIATOR             0x1828
#define CMD_BLE5_RX_TEST               0x1829
#define CMD_BLE5_TX_TEST               0x182A

// BLE Radio Operation Command Status
#define BLESTAT_IDLE                   RFSTAT_IDLE
#define BLESTAT_PENDING                RFSTAT_PENDING
#define BLESTAT_ACTIVE                 RFSTAT_ACTIVE
#define BLESTAT_SKIPPED                RFSTAT_SKIPPED
//
#define BLESTAT_DONE_OK                0x1400  // result True
#define BLESTAT_DONE_RXTIMEOUT         0x1401  // result False
#define BLESTAT_DONE_NOSYNC            0x1402  // result True
#define BLESTAT_DONE_RXERR             0x1403  // result True
#define BLESTAT_DONE_CONNECT           0x1404  // result True (False for Slave)
#define BLESTAT_DONE_MAXNACK           0x1405  // result True
#define BLESTAT_DONE_ENDED             0x1406  // result False
#define BLESTAT_DONE_ABORT             0x1407  // result Abort
#define BLESTAT_DONE_STOPPED           0x1408  // result False
#define BLESTAT_DONE_AUX               0x1409  // result False
#define BLESTAT_DONE_CONNECT_CHSEL0    0x140A  // result False
//
#define BLESTAT_ERROR_PAR              0x1800  // result Abort
#define BLESTAT_ERROR_RXBUF            0x1801  // result False
#define BLESTAT_ERROR_NO_SETUP         0x1802  // result Abort
#define BLEBTAT_ERROR_NO_FS            0x1803  // result Abort
#define BLESTAT_ERROR_SYNTH_PROG       0x1804  // result Abort
#define BLESTAT_ERROR_RX_OVERFLOW      0x1805  // result Abort
#define BLESTAT_ERROR_TX_UNDERFLOW     0x1806  // result Abort
#define BLESTAT_ERROR_AUX              0x1807  // result Abort
#define BLESTAT_ERROR_NO_GRANT         0x1808  // result Abort

// Advertisement Data Type
#define BLE_ADV_DATA_TYPE              0
#define BLE_SCAN_RSP_DATA_TYPE         1

// Advertising Configuration
#define ADV_CFG_INCLUDE_BAD_LEN_MSG    0
#define ADV_CFG_DISCARD_BAD_LEN_MSG    1
#define ADV_CFG_DISABLE_CHAN_SELECT    0
#define ADV_CFG_ENABLE_CHAN_SELECT     1
#define ADV_CFG_DISABLE_PRIV_IGN_MODE  0
#define ADV_CFG_ENABLE_PRIV_IGN_MODE   1
#define ADV_CFG_DISABLE_RPA_MODE       0
#define ADV_CFG_ENABLE_RPA_MODE        1

// Scan Configuration
#define SCAN_CFG_WL_POLICY_ANY         0
#define SCAN_CFG_WL_POLICY_USE_WL      1
#define SCAN_CFG_PASSIVE_SCAN          0
#define SCAN_CFG_ACTIVE_SCAN           1
#define SCAN_CFG_DISABLE_RPA_FILTER    0
#define SCAN_CFG_ENABLE_RPA_FILTER     1
#define SCAN_CFG_ALLOW_ILLEGAL_LEN     0
#define SCAN_CFG_DISCARD_ILLEGAL_LEN   1
#define SCAN_CFG_NO_SET_IGNORE_BIT     0
#define SCAN_CFG_AUTO_SET_IGNORE_BIT   1
#define SCAN_CFG_NO_END_ON_REPORT      0
#define SCAN_CFG_END_ON_REPORT         1
#define SCAN_CFG_DISABLE_RPA_MODE      0
#define SCAN_CFG_ENABLE_RPA_MODE       1

// Extended Scan Configuration
#define EXT_SCAN_CFG_CHECK_ADI         BV(0)
#define EXT_SCAN_CFG_AUTO_ADI_UPDATE   BV(1)
#define EXT_SCAN_CFG_DUP_FILTERING     BV(2)
#define EXT_SCAN_CFG_AUTO_WL_IGNORE    BV(3)
#define EXT_SCAN_CFG_AUTO_ADI_PROCESS  BV(4)
#define EXT_SCAN_CFG_EXCLUSIVE_SID     BV(5)
#define EXT_SCAN_CFG_ACCEPT_SYNCINFO   BV(6)

// Init Configuration
#define INIT_CFG_USE_PEER_ADDR         0
#define INIT_CFG_WL_POLICY_USE_WL      1
#define INIT_CFG_NO_DYN_WINOFFSET      0
#define INIT_CFG_USE_DYN_WINOFFSET     1
#define INIT_CFG_ALLOW_ILLEGAL_LEN     0
#define INIT_CFG_DISCARD_ILLEGAL_LEN   1

// RX Queue Configuration Bits
#define RXQ_CFG_CLEAR                  0
#define RXQ_CFG_AUTOFLUSH_IGNORED_PKT  BV(0)
#define RXQ_CFG_AUTOFLUSH_CRC_ERR_PKT  BV(1)
#define RXQ_CFG_AUTOFLUSH_EMPTY_PKT    BV(2)
#define RXQ_CFG_INCLUDE_PKT_LEN_BYTE   BV(3)
#define RXQ_CFG_INCLUDE_CRC            BV(4)
#define RXQ_CFG_APPEND_RSSI            BV(5)
#define RXQ_CFG_APPEND_STATUS          BV(6)
#define RXQ_CFG_APPEND_TIMESTAMP       BV(7)

// Sequence Number Status Bits
#define SEQ_NUM_CFG_CLEAR              0
#define SEQ_NUM_CFG_LAST_RX_SN         BV(0)
#define SEQ_NUM_CFG_LAST_TX_SN         BV(1)
#define SEQ_NUM_CFG_NEXT_TX_SN         BV(2)
#define SEQ_NUM_CFG_FIRST_PKT          BV(3)
#define SEQ_NUM_CFG_AUTO_EMPTY         BV(4)
#define SEQ_NUM_CFG_CTRL_TX            BV(5)
#define SEQ_NUM_CFG_CTRL_ACK_RX        BV(6)
#define SEQ_NUM_CFG_CTRL_ACK_PENDING   BV(7)

// RX Test
#define RX_TEST_END_AFTER_RX_PKT       0
#define RX_TEST_REPEAT_AFTER_RX_PKT    1

// Radio Timer (RAT) Times (in 250ns ticks)
#define RAT_TICKS_IN_1US               4         // Connection Jitter
#define RAT_TICKS_IN_3US               12        // Aux Start Time Workaround for 2M
#define RAT_TICKS_IN_4US               16        // Max Range Delay (1km)
#define RAT_TICKS_IN_6US               24        // Timestamp correction
#define RAT_TICKS_IN_10US              40        // Connection Jitter
#define RAT_TICKS_IN_15_5US            62        // TP/TIM/SLA/BV-05
#define RAT_TICKS_IN_16US              64        // Connection Jitter
#define RAT_TICKS_IN_40US              160       // Primary Channel Branch Delay
#define RAT_TICKS_IN_64US              256       // Radio Rx Settle Time
#define RAT_TICKS_IN_72US              288       // Agama AP timing adjustment due to pilot tone
#define RAT_TICKS_IN_85US              340       // Radio Rx Synch Time
#define RAT_TICKS_IN_90US              360       // Agama CC13X2P AP timing adjustment due to pilot tone
#define RAT_TICKS_IN_100US             400       // 1M / 2500 RAT ticks (SCA PPM)
#define RAT_TICKS_IN_140US             560       // Rx Back-end Time
#define RAT_TICKS_IN_150US             600       // T_IFS
#define RAT_TICKS_IN_180US             720       // AUX_CONNECT_REQ in 2M
#define RAT_TICKS_IN_200US             800       // LL Topology margin
#define RAT_TICKS_IN_700US             2800      // LL_TEST_MODE JIRA-2756
#define RAT_TICKS_IN_256US             1024      // Radio Overhead + FS Calibration
#define RAT_TICKS_IN_280US             1120      // Radio Overhead + FS Calibration
#define RAT_TICKS_IN_300US             1200      // T_MAFS (AE)
#define RAT_TICKS_IN_352US             1408      // CONNECT_IND is 44 bytes
#define RAT_TICKS_IN_500US             2000      // Periodic Adv addition process command
#define RAT_TICKS_IN_625US             2500      // Fundamental BLE Time Slot
#define RAT_TICKS_IN_900US             3600      // Additional Rx Synch overhead for Coded S8
#define RAT_TICKS_IN_1MS               4000      // Multiple of Adv Random Delay
#define RAT_TICKS_IN_1_006MS           4024      // AUX_CONNECT_REQ in Coded S2
#define RAT_TICKS_IN_1_225MS           4900      // Adv HDC offset time from rf count command in case ch.39 excluded
#define RAT_TICKS_IN_1_25MS            5000      // Fundamental BLE Time Unit
#define RAT_TICKS_IN_1_5MS             6000      // max RX window size for periodic scan
#define RAT_TICKS_IN_1_875MS           7500      // DTM Packet Interval
#define RAT_TICKS_IN_2MS               8000      // Max time in 1M phy for fragments periodic adv
#define RAT_TICKS_IN_2_5MS             10000     // DTM Packet Interval
#define RAT_TICKS_IN_2_896MS           11584     // AUX_CONNECT_REQ in Coded S8
#define RAT_TICKS_IN_10MS              40000     // General Purpose Delay
#define RAT_TICKS_IN_12_5MS            50000     // DTM T(l) Compare
#define RAT_TICKS_IN_16MS              64000     // Max time in Coded phy for fragments periodic adv
#define RAT_TICKS_IN_20MS              80000
#define RAT_TICKS_IN_1_28S             5120000   // Directed Advertising Timeout
#define RAT_TICKS_IN_32S               128000000 // Max LSTO
//
#define RAT_TICKS_FOR_CONNECT_IND             RAT_TICKS_IN_352US
#define RAT_TICKS_FOR_AUX_CONN_REQ_1M         RAT_TICKS_IN_352US
#define RAT_TICKS_FOR_AUX_CONN_REQ_2M         RAT_TICKS_IN_180US
#define RAT_TICKS_FOR_AUX_CONN_REQ_S2         RAT_TICKS_IN_1_006MS
#define RAT_TICKS_FOR_AUX_CONN_REQ_S8         RAT_TICKS_IN_2_896MS
#define RAT_TICKS_FOR_PERIODIC_SCAN_WIN_SIZE  RAT_TICKS_IN_1_5MS
#define RAT_TICKS_FOR_SCHED_PROCESS_TIME      RAT_TICKS_IN_200US

// Radio Timer (RAT) Channels
#define RAT_CHAN_5                     5
#define RAT_CHAN_6                     6
#define RAT_CHAN_7                     7

// Miscellaneous
#define BLE_BDADDR_SIZE                6
#define MAX_BLE_CONNECT_IND_SIZE       34 // init addr + own addr + payload
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG)
#define MAX_BLE_ADV_PKT_SIZE          255 // payload
#else // !AE_CFG
#define MAX_BLE_ADV_PKT_SIZE           37 // payload
#endif // AE_CFG

// HCI Rx Packet Header
// | Packet Type (1) | Handler(2) | Length(2) |
// Note: This is the same define as what's in hci_tl.h: HCI_DATA_MIN_LENGTH!
#define HCI_RX_PKT_HDR_SIZE            5
//
#define BLE_CRC_LEN                    LL_PKT_CRC_LEN
#define BLE_CHAN_MAP_LEN               LL_NUM_BYTES_FOR_CHAN_MAP

/*******************************************************************************
 * MACROS
 */

//
// Receive Queue Entry Configuration
//
#define SET_RXQ_ENTRY_CFG( d, b )                                              \
  (d) |= BV(b)

#define CLR_RXQ_ENTRY_CFG( d, b )                                              \
  (d) &= ~BV(b)

// Sequence Number Status
#define SET_ENTRY_CFG( d, b )                                                  \
  (d) |= BV(b)

#define CLR_ENTRY_CFG( d, b )                                                  \
  (d) &= ~BV(b)

// Whitening Configuration
#define CLR_WHITENING( x )                                                     \
  (x) = 0x80

#define SET_WHITENING_BLE( x )                                                 \
  (x) = 0x00;

#define SET_WHITENING_INIT( x, v )                                             \
  (x) = 0x80 | ((v) & ~0x80)

//
// Advertising Configuration
//
#define CLR_ADV_CFG( c )                                                       \
  (c) = 0

#define SETBIT_ADV_CFG( c, b )                                                 \
  (c) = ((c) & ~BV(b)) | BV(b)

#define SETVAR_ADV_CFG_FILTER_POLICY( c, v )                                   \
  (c) = ((c) & ~0x03) | ((v) & 0x03)

#define SETVAR_ADV_CFG_DEV_ADDR_TYPE( c, v )                                   \
  (c) = ((c) & ~BV(2)) | (((v) & 0x01) << 2)

#define GET_ADV_CFG_DEV_ADDR_TYPE( c )                                         \
  (((c) & BV(2)) >> 2)

#define SETVAR_ADV_CFG_PEER_ADDR_TYPE( c, v )                                  \
  (c) = ((c) & ~BV(3)) | (((v) & 0x01) << 3)

#define SETVAR_ADV_CFG_STRICT_LEN_FILTER( c, v )                               \
  (c) = ((c) & ~BV(4)) | (((v) & 0x01) << 4)

#define SET_ADV_CFG_CHAN_SELECT( c )                                           \
  (c) |= BV(5)

#define CLR_ADV_CFG_CHAN_SELECT( c )                                           \
  (c) &= ~BV(5)

#define SETVAR_ADV_CFG_CHAN_SELECT( c, v )                                     \
  (c) = ((c) & ~BV(5)) | (((v) & 0x01) << 5)

#define SET_ADV_CFG_PRIV_IGN_MODE( c )                                         \
  (c) |= BV(6)

#define CLR_ADV_CFG_PRIV_IGN_MODE( c )                                         \
  (c) &= ~BV(6)

#define SETVAR_ADV_CFG_PRIV_IGN_MODE( c, v )                                   \
  (c) = ((c) & ~BV(6)) | (((v) & 0x01) << 6)

#define SET_ADV_CFG_RPA_MODE( c )                                              \
  (c) |= BV(7)

#define CLR_ADV_CFG_RPA_MODE( c )                                              \
  (c) &= ~BV(7)

#define SETVAR_ADV_CFG_RPA_MODE( c, v )                                        \
  (c) = ((c) & ~BV(7)) | (((v) & 0x01) << 7)


//
// Extended Advvertising Configuration
//

// Note: Bit 5 is bDirected instead of Channel Selection Algo!

#define SET_ADV_CFG_DIRECTED( c )                                              \
  (c) |= BV(5)

#define CLR_ADV_CFG_DIRECTED( c )                                              \
  (c) &= ~BV(5)

#define SETVAR_ADV_CFG_DIRECTED( c, v )                                        \
  (c) = ((c) & ~BV(5)) | (((v) & 0x01) << 5)


//
// Scan Configuration
//
#define CLR_SCAN_CFG( c )                                                      \
  (c) = 0

#define SETBIT_SCAN_CFG( c, b )                                                \
  (c) = ((c) & ~BV(b)) | BV(b)

#define SETVAR_SCAN_CFG_FILTER_POLICY( c, v )                                  \
  (c) = ((c) & ~BV(0)) | ((v) & 0x01);

#define SETVAR_SCAN_CFG_ACTIVE_SCAN( c, v )                                    \
  (c) = ((c) & ~BV(1)) | (((v) & 0x01) << 1)

#define SETVAR_SCAN_CFG_DEV_ADDR_TYPE( c, v )                                  \
  (c) = ((c) & ~BV(2)) | (((v) & 0x01) << 2)

#define GET_SCAN_CFG_DEV_ADDR_TYPE( c )                                        \
  (((c) & BV(2)) >> 2)

#define SETVAR_SCAN_CFG_RPA_FILTER_POLICY( c, v )                              \
  (c) = ((c) & ~BV(3)) | (((v) & 0x01) << 3)

#define SETVAR_SCAN_CFG_STRICT_LEN_FILTER( c, v )                              \
  (c) = ((c) & ~BV(4)) | (((v) & 0x01) << 4)

#define SET_SCAN_CFG_AUTO_SET_WL_IGNORE( c )                                   \
  (c) |= BV(5)

#define CLR_SCAN_CFG_AUTO_SET_WL_IGNORE( c )                                   \
  (c) &= ~BV(5)

#define SET_SCAN_CFG_AUTO_SET_WL_IGNORE( c )                                   \
  (c) |= BV(5)

#define CLR_SCAN_CFG_AUTO_SET_WL_IGNORE( c )                                   \
  (c) &= ~BV(5)

#define SETVAR_SCAN_CFG_AUTO_SET_WL_IGNORE( c, v )                             \
  (c) = ((c) & ~BV(5)) | (((v) & 0x01) << 5)

#define SETVAR_SCAN_CFG_END_ON_REPORT( c, v )                                  \
  (c) = ((c) & ~BV(6)) | (((v) & 0x01) << 6)

#define SET_SCAN_CFG_RPA_MODE( c )                                             \
  (c) |= BV(7)

#define CLR_SCAN_CFG_RPA_MODE( c )                                             \
  (c) &= ~BV(7)

#define SETVAR_SCAN_CFG_RPA_MODE( c, v )                                       \
  (c) = ((c) & ~BV(7)) | (((v) & 0x01) << 7)

//
// Scan Backoff Configuration
//
#define SETVAR_SCAN_BACKOFF_CFG_UL( c, v )                                     \
  (c) = ((c) & ~0x0F) | ((v) & 0x0F)

#define SETVAR_SCAN_BACKOFF_CFG_LAST_SUCCCEEDED( c, v )                        \
  (c) = ((c) & ~BV(4)) | (((v) & 0x01) << 4)

#define SETVAR_SCAN_BACKOFF_CFG_LAST_FAILED( c, v )                            \
  (c) = ((c) & ~BV(5)) | (((v) & 0x01) << 5)

//
// Scan Extended Filter Configuration
//
#define CLR_EXT_SCAN_FILTER_CFG( c )                                           \
  (c) = 0

#define SETVAR_EXT_SCAN_FILTER_CFG(c, v)                                       \
  (c) = ((c) & ~0x3F) | ((v) & 0x3F)

#define CLR_EXT_SCAN_FILTER_CFG_EXCLUSIVE_SID( c )                             \
  (c) &= ~EXT_SCAN_CFG_EXCLUSIVE_SID

#define SET_EXT_SCAN_FILTER_CFG_EXCLUSIVE_SID( c )                             \
  (c) |= EXT_SCAN_CFG_EXCLUSIVE_SID

#define CLR_EXT_SCAN_FILTER_CFG_ACCEPT_SYNCINFO( c )                           \
  (c) &= ~EXT_SCAN_CFG_ACCEPT_SYNCINFO

#define SET_EXT_SCAN_FILTER_CFG_ACCEPT_SYNCINFO( c )                           \
  (c) |= EXT_SCAN_CFG_ACCEPT_SYNCINFO

//
// Adi Status Configuration
//
#define CLR_ADI_STATUS( c )                                                \
  (c) = 0

#define SETVAR_ADI_STATUS(c, v)                                            \
  (c) = ((c) & ~0x7F) | ((v) & 0x7F)

#define SETVAR_ADI_STATUS_LAST_ACCEPTED_SID(c, v)                          \
  (c) = ((c) & ~0x0F) | ((v) & 0x0F)

#define SETVAR_ADI_STATUS_STATE(c, v)                                      \
  (c) = ((c) & ~0x70) | (((v) & 0x07) << 4)

//
// Init Configuration
//
#define CLR_INIT_CFG( c )                                                      \
  (c) = 0

#define SETVAR_INIT_CFG_FILTER_POLICY( c, v )                                  \
  (c) = ((c) & ~BV(0)) | ((v) & 0x01);

#define SETVAR_INIT_CFG_DYN_WINOFFSET( c, v )                                  \
  (c) = ((c) & ~BV(1)) | (((v) & 0x01) << 1)

#define SETVAR_INIT_CFG_DEV_ADDR_TYPE( c, v )                                  \
  (c) = ((c) & ~BV(2)) | (((v) & 0x01) << 2)

#define SETVAR_INIT_CFG_PEER_ADDR_TYPE( c, v )                                 \
  (c) = ((c) & ~BV(3)) | (((v) & 0x01) << 3)

#define SETVAR_INIT_CFG_STRICT_LEN_FILTER( c, v )                              \
  (c) = ((c) & ~BV(4)) | (((v) & 0x01) << 4)

#define SET_INIT_CFG_CHAN_SELECT( c )                                          \
  (c) |= BV(5)

#define CLR_INIT_CFG_CHAN_SELECT( c )                                          \
  (c) &= ~BV(5)

#define SETVAR_INIT_CFG_CHAN_SELECT( c, v )                                    \
  (c) = ((c) & ~BV(5)) | (((v) & 0x01) << 5)

//
// Transmit Test Configuration
//
#define SET_TX_TEST_CFG_OVERRIDE( c )                                          \
  (c) |= BV(0)

#define CLR_TX_TEST_CFG_OVERRIDE( c )                                          \
  (c) &= ~BV(0)

#define SET_TX_TEST_CFG_USE_PRBS9( c )                                         \
  (c) = ((c) & 0x06) | BV(1);

#define SET_TX_TEST_CFG_USE_PRBS15( c )                                        \
  (c) = ((c) & 0x06) | BV(2);

//
// Receive Status Byte Bit Field
//
#define VALID_TIMESTAMP( s )                                                   \
  ((s) & BV(0))

#define LAST_RX_PKT_CRC_ERROR( s )                                             \
  ((s) & BV(1))

#define LAST_RX_PKT_IGNORED( s )                                               \
  ((s) & BV(2))

#define LAST_RX_PKT_EMPTY( s )                                                 \
  ((s) & BV(3))

#define LAST_RX_PKT_CONTROL( s )                                               \
  ((s) & BV(4))

#define LAST_RX_PKT_MD( s )                                                    \
  ((s) & BV(5))

#define LAST_RX_PKT_ACK( s )                                                   \
  ((s) & BV(6))


//
// Adi Status
//
#define ADI_STATUS_LAST_ACCEPTED_SID( s )                                      \
  ((s) & 0x0F)

#define ADI_STATUS_STATE( s )                                                  \
  (((s) >> 4 ) & 0x03)


#define WAIT( t ) {volatile uint32 time = (t); while( time-- );}

/*******************************************************************************
 * TYPEDEFS
 */

// BLE Radio Operation Whitening Configuration
// |     7    |       6..0     |
// | Override | Whitening Init |
//
typedef uint8 whitenCfg_t;

// BLE Radio Operation Receive Queue Configuration
// |     7     |   6    |   5  |  4  |      3      |       2        |         1           |         0         |
// | Timestamp | Status | RSSI | CRC | Length Byte | AutoFlushEmpty | AutoFlush CRC Error | AutoFlush Ignored |
//
typedef uint8 rxQCfg_t;

// BLE Radio Operation Sequence Number Status
// |         7        |      6      |    5    |      4       |      3       |     2      |      1     |     0      |
// | Ctrl Ack Pending | Ctrl Ack Rx | Ctrl Tx | Tx AutoEmpty | First Packet | Next Tx SN | Last Tx SN | Last Rx SN |
//
typedef uint8 seqNumStat_t;

// BLE Advertiser Configuration
// |     7    |           6         |        5      |         4         |        3       |        2         |     1..0      |
// | RPA Mode | Privacy Ignore Mode | Chan Algo Sel | Strict Len Filter | Peer Addr Type | Device Addr Type | Filter Policy |
typedef uint8 advCfg_t;

// BLE Extended Advertiser Configuration
// |     7 ..3   |        2         |   1..0   |
// |   Reserved  | Device Addr Type | Reserved |
typedef uint8 extAdvCfg_t;

// BLE Scanner Configuration
// |     7    |       6       |         5         |       4       |      3     |        2         |      1              0
// | RPA Mode | End On Report | AutoSet WL Ignore | Length Filter | RPA Filter | Device Addr Type | Active Scan | Filter Policy |
//
typedef uint8 scanCfg_t;

// BLE Backoff Parameters
// | 7..6 |      5      |      4       |     3..0        |
// |  N/A | Last Failed | Last Success | Log Upper Limit |
//
typedef uint8 bkOff_t;

// BLE Initiator Configuration
// | 7..6 |   5   |      4        |       3        |        2         |         1          |       0        |
// |  N/A | ChSel | Length Filter | Peer Addr Type | Device Addr Type | Dyn. Window Offset | Use White list |
//
typedef uint8 initCfg_t;

// BLE Transmit Test Override Configuration
// | 7..3 |      2     |     1     |         0         |
// |  N/A | Use PRBS15 | Use PRBS9 | Encoding Override |
//
typedef uint8 txTestCfg_t;

// Receive Status Byte Bit Field
// |  7  |    6    |    5   |     4    |     3     |    2    |    1    |        0        |
// | N/A | lastACK | lastMD | lastCTRL | lastEmpty | lastIgn | lastErr | Timestamp Valid |
//
typedef uint8 pktStat_t;

// Extended Scanner Filter Configuration
// |   7..6   |       5       |        4        |       3       |          2         |        1       |     0     |
// | Reserved | bExclusiveSid | bAutoAdiProcess | bAutoWlIgnore | bApplyDupFiltering | bAutoAdiUpdate | bCheckAdi |
//
typedef uint8 extFltrCfg_t;

// Extended Scanner Filter Configuration
// |    7     | 6..4  |      3..0       |
// | Reserved | state | lastAcceptedSid |
//
typedef uint8 adiStatus_t;

// Extended Scanner ADI List
// |  15..14  | 13..12 |   11..0   |
// | Reserved |  mode  | advDataId |
//
typedef uint16 adiList_t;

/*
** BLE Data Entry Structures
*/

// IDEA:
// USE OVERLAY TO MAKE THIS WORK.
// E.G. SIZE = sizeof( dataEntry_t ) + sizeof( dataEntryPrefix_t ) + dataSize + sizeof( dataEntrySuffix_t )
//      THEN CREATE A BUFFER OF THAT SIZE: uint8 buf[ SIZE ]
//      THEN MAP DATA ENTRY POINTER: dataQueue->pCurEntry = &buf[ sizeof( dataEntryPrefix_t ) ]
//      THEN BEGIN AT: &(dataQueue->pCurEntry->length+sizeof(uint16))?

// Data Entry Prefix
// Note: Not formally part of the radio data entry definition.
// ALT: Could move to BLE file.
PACKED_TYPEDEF_STRUCT
{
  uint8  bleStateRole;                 // could be combined wtih connection ID
  uint8  reserved;                     // for packing
  uint16 bleConnID;                    // could be combined with task state NOT NEEDED IF QUEUE PER CONNECTION
  uint32 rxPktCount;                   // used for building nonce for encryption
} dataEntryPrefix_t;

/*
** BLE Radio Commands
**
** R  = System CPU reads; Radio CPU will not read.
** W  = System CPU writes; Radio CPU reads but will not modify.
** RW = System CPU writes initially; Radio CPU reads and may modify.
*/

// BLE Radio Operation Command Common Structure
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t     rfOpCmd;               // radio command common structure
  uint8         chan;                  // W:  channel number
  whitenCfg_t   whitening;             // W:  whitening configuration
  uint8        *pParams;               // W:  ptr to cmd specific parameters
  uint8        *pOutput;               // W:  ptr to cmd specific results, or NULL
} bleOpCmd_t;

// BLE5 Radio Operation Command Common Structure
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t     rfOpCmd;               // radio command common structure
  uint8         chan;                  // W:  channel number
  whitenCfg_t   whitening;             // W:  whitening configuration
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
  uint8         phyMode;               // W:  mainMode and LR coding
  uint8         rangeDelay;            // W:  extended T_IFS in RAT ticks
  uint16        txPower;               // W:  Tx power for this command
#endif // PHY_2MBPS_CFG | PHY_LR_CFG
  uint8        *pParams;               // W:  ptr to cmd specific parameters
  uint8        *pOutput;               // W:  ptr to cmd specific results, or NULL
#if defined(CC13X2P)
  uint32       *tx20Power;             // W:  Tx power for HP PA
#endif // CC13X2P
} ble5OpCmd_t;


/*
** BLE Input Command Parameter Structures
*/

// Advertiser Command Parameters
PACKED_TYPEDEF_STRUCT
{
  dataEntryQ_t *pRXQ;                  // W:  ptr to Rx queue
  rxQCfg_t      rxCfg;                 // W:  rx queue configuration
  advCfg_t      advCfg;                // W:  advertiser configuration
  uint8         advLen;                // W:  size of Adv data
  uint8         scanRspLen;            // W:  size of Scan Response data
  uint8        *pAdvData;              // W:  ptr to Adv data
  uint8        *pScanRspData;          // W:  ptr to Scan Response data
  uint8        *pDeviceAddr;           // W:  ptr to device BLE address
  wlEntry_t    *pWhiteList;            // W:  ptr to white list
  uint8         reserved[3];           // unused
  trig_t        endTrig;               // W:  end trig for adv event
  uint32        endTime;               // W:  time for end trigger
} advParam_t;

// Scanner Command Parameters
PACKED_TYPEDEF_STRUCT
{
  dataEntryQ_t *pRXQ;                  // W:  ptr to Rx queue
  rxQCfg_t      rxCfg;                 // W:  rx queue configuration
  scanCfg_t     scanCfg;               // W:  advertiser configuration
  uint16        randState;             // RW: a pseudo-random number
  uint16        backoffCount;          // RW: backoff count
  bkOff_t       backoffParam;          // RW: backoff parameters
  uint8         scanReqLen;            // W:  size of Scan Request data
  uint8        *pScanReqData;          // W:  ptr to Scan Request data
  uint8        *pDeviceAddr;           // W:  ptr to device address
  wlEntry_t    *pWhiteList;            // W:  ptr to white list
  uint16        reserved1;             // unused
  trig_t        timeoutTrig;           // W:  timeout trig for first Rx operation
  trig_t        endTrig;               // W:  end trig for connection event
  uint32        timeoutTime;           // W:  time for timeout trigger
  uint32        endTime;               // W:  time for end trigger
} scanParam_t;

// Extended Scanner Command Parameters
PACKED_TYPEDEF_STRUCT
{
  dataEntryQ_t *pRXQ;                  // W:  ptr to Rx queue
  rxQCfg_t      rxCfg;                 // W:  rx queue configuration
  scanCfg_t     scanCfg;               // W:  advertiser configuration
  uint16        randState;             // RW: a pseudo-random number
  uint16        backoffCount;          // RW: backoff count
  bkOff_t       backoffParam;          // RW: backoff parameters
  extFltrCfg_t  extFltrCfg;            // W:  extended filter configuration
  adiStatus_t   adiStatus;             // RW: adi status
  uint8         reserved[3];           // unused
  uint8        *pDeviceAddr;           // W:  ptr to device address
  wlEntry_t    *pWhiteList;            // W:  ptr to white list
  adiList_t    *pAdiList;              // W:  ptr to ADI list
  uint16        maxWaitForAux;         // W:  max wait time to secondary channel
  trig_t        timeoutTrig;           // W:  timeout trig for first Rx operation
  trig_t        endTrig;               // W:  end trig for connection event
  uint32        timeoutTime;           // W:  time for timeout trigger
  uint32        endTime;               // W:  time for end trigger
  uint32        rxStartTime;           // R:  time needed to start Rx
  uint16        rxListenTime;          // R:  time needed to listen
  uint8         chan;                  // R:  secondary channel number
  uint8         phyMode;               // R:  secondary channel PHY
} extScanParam_t;

// Initiator Command Parameters
PACKED_TYPEDEF_STRUCT
{
  dataEntryQ_t *pRXQ;                  // W:  ptr to Rx queue
  rxQCfg_t      rxCfg;                 // W:  rx queue configuration
  initCfg_t     initCfg;               // W:  initiator configuration
  uint8         reserved1;             // unused
  uint8         connReqLen;            // W:  size of Connect Request data
  uint8        *pConnReqData;          // W:  ptr to Connect Request data
  uint8        *pDeviceAddr;           // W:  ptr to device address
  wlEntry_t    *pWhiteList;            // W:  ptr to white list or peer address
  uint32        connectTime;           // RW: time of first connection event
  uint16        reserved2;             // unused
  trig_t        timeoutTrig;           // W:  timeout trig for first Rx operation
  trig_t        endTrig;               // W:  end trig for connection event
  uint32        timeoutTime;           // W:  time for timeout trigger
  uint32        endTime;               // W:  time for end trigger
} initParam_t;

// Extended Initiator Command Parameters
PACKED_TYPEDEF_STRUCT
{
  dataEntryQ_t *pRXQ;                  // W:  ptr to Rx queue
  rxQCfg_t      rxCfg;                 // W:  rx queue configuration
  initCfg_t     initCfg;               // W:  initiator configuration
  uint16        randState;             // RW: a pseudo-random number
  uint16        backoffCount;          // RW: backoff count
  bkOff_t       backoffParam;          // RW: backoff parameters
  uint8         connReqLen;            // W:  size of Connect Request data
  uint8        *pConnReqData;          // W:  ptr to Connect Request data
  uint8        *pDeviceAddr;           // W:  ptr to device address
  wlEntry_t    *pWhiteList;            // W:  ptr to white list or peer address
  uint32        connectTime;           // RW: time of first connection event
  uint16        maxWaitForAux;         // W:  max wait time to secondary channel
  trig_t        timeoutTrig;           // W:  timeout trig for first Rx operation
  trig_t        endTrig;               // W:  end trig for connection event
  uint32        timeoutTime;           // W:  time for timeout trigger
  uint32        endTime;               // W:  time for end trigger
  uint32        rxStartTime;           // R:  time needed to start Rx
  uint16        rxListenTime;          // R:  time needed to listen
  uint8         chan;                  // R:  secondary channel number
  uint8         phyMode;               // R:  secondary channel PHY
} extInitParam_t;

// Initiator Command CONNECT_IND LL_Data
PACKED_TYPEDEF_STRUCT
{
  uint32        accessAddress;         // W:  access address used in connection
  uint8         crcInit[BLE_CRC_LEN];  // W:  CRC init value
  uint8         winSize;
  uint16        winOffset;
  uint16        connInterval;
  uint16        latency;
  uint16        timeout;
  uint8         chanMap[BLE_CHAN_MAP_LEN];
  uint8         hopSca;
} connReqData_t;

// Master Command Parameters
PACKED_TYPEDEF_STRUCT
{
  dataEntryQ_t *pRXQ;                  // W:  ptr to Rx queue
  dataEntryQ_t *pTXQ;                  // W:  ptr to Tx queue
  rxQCfg_t      rxCfg;                 // W:  rx queue configuration
  seqNumStat_t  seqStat;               // RW: sequence status bit field
  uint8         maxNAck;               // W:  max number of NACKs allowed
  uint8         maxTxPkt;              // W:  max number of Tx pkts allowed
  uint32        accessAddress;         // W:  access address used in connection
  uint8         crcInit[BLE_CRC_LEN];  // W:  CRC init value
  trig_t        endTrig;               // W:  end trig for connection event
  uint32        endTime;               // W:  time for end trigger
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
  uint8         maxRxPktLen;           // W:  BLE5 max Rx pkt len allowed
  uint8         maxTxLenForLR;         // W:  BLE5 max Rx pkt len for S=8 LR
  uint8         reserved[2];           // unused
#endif // PHY_2MBPS_CFG | PHY_LR_CFG
} masterParam_t;

// Slave Command Parameters
PACKED_TYPEDEF_STRUCT
{
  dataEntryQ_t *pRXQ;                  // W:  ptr to Rx queue
  dataEntryQ_t *pTXQ;                  // W:  ptr to Tx queue
  rxQCfg_t      rxCfg;                 // W:  rx queue configuration
  seqNumStat_t  seqStat;               // RW: sequence status bit field
  uint8         maxNAck;               // W:  max number of NACKs allowed
  uint8         maxTxPkt;              // W:  max number of Tx pkts allowed
  uint32        accessAddress;         // W:  access address used in connection
  uint8         crcInit[BLE_CRC_LEN];  // W:  CRC init value
  trig_t        timeoutTrig;           // W:  timeout trig for first Rx operation
  uint32        timeoutTime;           // W:  time for timeout trigger
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
  uint8         maxRxPktLen;           // W:  BLE5 max Rx pkt len allowed
  uint8         maxTxLenForLR;         // W:  BLE5 max Rx pkt len for S=8 LR
  uint8         reserved[1];           // unused
#else // !PHY_2MBPS_CFG & !PHY_LR_CFG
  uint8         reserved[3];           // unused
#endif // PHY_2MBPS_CFG | PHY_LR_CFG
  trig_t        endTrig;               // W:  end trig for connection event
  uint32        endTime;               // W:  time for end trigger
} slaveParam_t;

// Connection Command Parameters
PACKED_TYPEDEF_STRUCT
{
  dataEntryQ_t *pRXQ;                  // W:  ptr to Rx queue
  dataEntryQ_t *pTXQ;                  // W:  ptr to Tx queue
  rxQCfg_t      rxCfg;                 // W:  rx queue configuration
  seqNumStat_t  seqStat;               // RW: sequence status bit field
  uint8         maxNAck;               // W:  max number of NACKs allowed
  uint8         maxTxPkt;              // W:  max number of Tx pkts allowed
  uint32        accessAddress;         // W:  access address used in connection
  uint8         crcInit[BLE_CRC_LEN];  // W:  CRC init value
  trig_t        timeoutTrig;           // W:  timeout trig for first Rx operation
  uint32        timeoutTime;           // W:  time for timeout trigger
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
  uint8         maxRxPktLen;           // W:  BLE5 max Rx pkt len allowed
  uint8         maxTxLenForLR;         // W:  BLE5 max Rx pkt len for S=8 LR
  uint8         reserved[1];           // unused
#else // !PHY_2MBPS_CFG & !PHY_LR_CFG
  uint8         reserved[3];           // unused
#endif // PHY_2MBPS_CFG | PHY_LR_CFG
  trig_t        endTrig;               // W:  end trig for connection event
  uint32        endTime;               // W:  time for end trigger
} linkParam_t;

// Generic Rx Command Parameters
PACKED_TYPEDEF_STRUCT
{
  dataEntryQ_t *pRXQ;                  // W:  ptr to Rx queue, or NULL
  rxQCfg_t      rxCfg;                 // W:  rx queue configuration
  uint8         repeatMode;            // W:  end/restart after pkt Rx
  uint16        reserved;              // unused
  uint32        accessAddress;         // W:  access address used in connection
  uint8         crcInit[BLE_CRC_LEN];  // W:  CRC init value
  trig_t        endTrig;               // W:  end trig for Rx
  uint32        endTime;               // W:  time for end trigger
} rxTestParam_t;

// Test Tx Command Parameters
PACKED_TYPEDEF_STRUCT
{
  uint16        numPkts;               // W:  number of pkts to Tx
  uint8         payloadLen;            // W:  size of Tx pkt
  uint8         pktType;               // W:  packet type
  uint32        period;                // W:  inter-pkt time, in radio cycles
  txTestCfg_t   config;                // W:  override packet encoding
  uint8         byteVal;               // W:  override byte to Tx
  uint8         reserved;              // unused
  trig_t        endTrig;               // W:  end trig for Tx
  uint32        endTime;               // W:  time for end trigger
} txTestParam_t;

/*
** BLE Output Command Structures
*/

// Advertiser Command
PACKED_TYPEDEF_STRUCT
{
  uint16        nTxAdv;                // RW: num ADV*_IND Tx pkts
  uint8         nTxScanRsp;            // RW: num SCAN_RSP Tx pkts
  uint8         nRxScanReq;            // RW: num SCAN_REQ okay Rx pkts
  uint8         nRxConnReq;            // RW: num CONNECT_IND okay Rx pkts
  uint8         reserved;              // unused
  uint16        nRxNok;                // RW: num not okay Rx pkts
  uint16        nRxIgn;                // RW: num okay Rx pkts ignored
  uint8         nRxBufFull;            // RW: num pkts discarded
  uint8         lastRssi;              // R:  RSSI of last Rx pkt
  uint32        timeStamp;             // R:  timestamp of last Rx pkt
} advOut_t;

// Scanner Command
PACKED_TYPEDEF_STRUCT
{
  uint16        nTxScanReq;            // RW: num SCAN_REQ Tx pkts
  uint16        nBoffScanReq;          // RW: num SCAN_REQ pkts not sent due to backoff
  uint16        nRxAdvOk;              // RW: num ADV*_IND okay Rx pkts
  uint16        nRxAdvIgn;             // RW: num ADV*_IND okay Rx pkts ignored
  uint16        nRxAdvNok;             // RW: num ADV*_IND not okay Rx pkts
  uint16        nRxScanRspOk;          // RW: num SCAN_RSP okay Rx pkts
  uint16        nRxScanRspIgn;         // RW: num SCAN_RSP okay Rx pkts ignored
  uint16        nRxScanRspNok;         // RW: num SCAN_RSP not okay Rx pkts
  uint8         nRxAdvBufFull;         // RW: num ADV*_IND pkts discarded
  uint8         nRxScanRspBufFull;     // RW: num SCAN_RSP pkts discarded
  uint8         lastRssi;              // R:  RSSI of last Rx pkt
  uint8         reserved;              // unused
  uint32        timeStamp;             // R: timestamp of last Rx ADV*_IND
} scanOut_t;

// Extended Scanner Command
PACKED_TYPEDEF_STRUCT
{
  uint16        nTxReq;                // RW: num _REQ Tx
  uint16        nBoffScanReq;          // RW: num _REQ not sent due to backoff
  uint16        nRxAdvOk;              // RW: num ADV*_IND okay Rx pkts
  uint16        nRxAdvIgn;             // RW: num ADV*_IND okay Rx pkts ignored
  uint16        nRxAdvNok;             // RW: num ADV*_IND not okay Rx pkts
  uint16        nRxScanRspOk;          // RW: num SCAN_RSP okay Rx pkts
  uint16        nRxScanRspIgn;         // RW: num SCAN_RSP okay Rx pkts ignored
  uint16        nRxScanRspNok;         // RW: num SCAN_RSP not okay Rx pkts
  uint8         nRxAdvBufFull;         // RW: num ADV*_IND pkts discarded
  uint8         nRxScanRspBufFull;     // RW: num SCAN_RSP pkts discarded
  uint8         lastRssi;              // R:  RSSI of last Rx pkt
  uint8         reserved;              // unused
  uint32        timeStamp;             // R:  timestamp of last Rx *ADV*_IND
} extScanOut_t;

// Initiator Command
PACKED_TYPEDEF_STRUCT
{
  uint8         nTxConnReq;            // RW: num CONN_REQ Tx pkts
  uint8         nRxAdvOk;              // RW: num ADV*_IND okay Rx pkts
  uint16        nRxAdvIgn;             // RW: num ADV*_IND okay Rx pkts ignored
  uint16        nRxAdvNok;             // RW: num ADV*_IND not okay Rx pkts
  uint8         nRxAdvBufFull;         // RW: num ADV*_IND pkts discarded
  uint8         lastRssi;              // R:  RSSI of last Rx pkt
  uint32        timeStamp;             // R:  timestamp of last Rx ADV*_IND
} initOut_t;

// Extended Initiator Command
PACKED_TYPEDEF_STRUCT
{
  uint16        nTxReq;                // RW: num _REQ Tx
  uint16        nBoffScanReq;          // RW: num _REQ not sent due to backoff
  uint16        nRxAdvOk;              // RW: num ADV*_IND okay Rx pkts
  uint16        nRxAdvIgn;             // RW: num ADV*_IND okay Rx pkts ignored
  uint16        nRxAdvNok;             // RW: num ADV*_IND not okay Rx pkts
  uint16        nRxScanRspOk;          // RW: num SCAN_RSP okay Rx pkts
  uint16        nRxScanRspIgn;         // RW: num SCAN_RSP okay Rx pkts ignored
  uint16        nRxScanRspNok;         // RW: num SCAN_RSP not okay Rx pkts
  uint8         nRxAdvBufFull;         // RW: num ADV*_IND pkts discarded
  uint8         nRxScanRspBufFull;     // RW: num SCAN_RSP pkts discarded
  uint8         lastRssi;              // R:  RSSI of last Rx pkt
  uint8         reserved;              // unused
  uint32        timeStamp;             // R:  timestamp of last Rx ADV*_IND
} extInitOut_t;

// Master or Slave Command
PACKED_TYPEDEF_STRUCT
{
  uint8         nTx;                   // RW: num Tx pkts
  uint8         nTxAck;                // RW: num Tx pkts Acked
  uint8         nTxCtrl;               // RW: num Tx ctrl pkts
  uint8         nTxCtrlAck;            // RW: num Tx ctrl pkts Acked
  uint8         nTxCtrlAckAck;         // RW: num Tx ctrl pkts Acked that were Acked
  uint8         nTxRetrans;            // RW: num retransmissions
  uint8         nTxEntryDone;          // RW: num pkts on Tx queue that are finished
  uint8         nRxOk;                 // RW: num okay Rx pkts
  uint8         nRxCtrl;               // RW: num okay Rx ctrl pkts
  uint8         nRxCtrlAck;            // RW: num okay Rx ctrl pkts Acked
  uint8         nRxNok;                // RW: num not okay Rx pkts
  uint8         nRxIgn;                // RW: num okay Rx pkts ignored
  uint8         nRxEmpty;              // RW: num okay Rx pkts with no payload
  uint8         nRxBufFull;            // RW: num pkts discarded
  uint8         lastRssi;              // R:  RSSI of last Rx pkt
  pktStat_t     pktStatus;             // RW: last pkt status and timestamp
  uint32        timeStamp;             // R:  timestamp of first Rx pkt (Slave Only)
} connOut_t;

// Generic Rx Command
PACKED_TYPEDEF_STRUCT
{
  uint16        nRxOk;                 // RW: num okay Rx pkts
  uint16        nRxNok;                // RW: num not okay Rx pkts
  uint16        nRxBufFull;            // RW: num ADV*_IND pkts discarded
  uint8         lastRssi;              // R:  RSSI of last Rx pkt
  uint8         reserved;              // unused
  uint32        timeStamp;             // R:  timestamp of first Rx pkt (Slave Only)
} rxOut_t;

// Test Tx Command
PACKED_TYPEDEF_STRUCT
{
  uint16        nTx;                   // RW: num Tx pkts
  uint16        reserved;              // unused - for alignment only
} txOut_t;

/*
** BLE Radio Immediate Commands
*/

// Update Advertising Data Command
PACKED_TYPEDEF_STRUCT
{
  uint16        cmdNum;                // W:  radio command number
  uint8         dataType;              // W:  Adv or Scan Rsp data
  uint8         dataLen;               // W:  length of update
  uint8        *pData;                 // W:  ptr to update data
  uint8        *pParams;               // W:  ptr to update parameters
} bleUpdateAdvData_t;


/*
** BLE5
*/

// CMD_BLE5_RADIO_SETUP: BLE5 Radio Setup Command
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  rfOpCmd_t      rfOpCmd;              // radio command common structure
  uint8          defaultPhy;           // for non-BLE commands only
  uint8          reserved;             // unused
  uint16         config;               // bias, trim, FS power up, etc.
  uint16         txPower;              // IB, GC, tempCoeff
  regOverride_t *pRegOverrideCommon;   // ptr to list of common overrides
  regOverride_t *pRegOverride1M;       // ptr to list of 1M overrides
  regOverride_t *pRegOverride2M;       // ptr to list of 2M overrides
  regOverride_t *pRegOverrideCoded;    // ptr to list of Coded overrides
#if defined(CC13X2P)
  regOverride_t *pRegOverrideTxStd;
  regOverride_t *pRegOverrideTx20;     // ptr to list of High Gain overrides (only applicable on CC1352P)
#endif
} rfOpCmd_Ble5RadioSetup_t;


/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

// Receive Queue
extern dataQ_t *rxDataQ;
extern dataQ_t *txDataQ;
extern dataEntryPtr_t (*rxRingBuf)[NUM_RX_DATA_ENTRIES];

// Advertising Data Structures
extern bleOpCmd_t       advCmd[];
extern advParam_t       advParam;
extern advOut_t         advOutput;
extern const uint8      advChan[];
extern const uint16     advEvt2Cmd[];
extern const uint8      advEvt2State[];

// Scan Data Structures
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
extern bleOpCmd_t       scanCmd;
extern scanParam_t      scanParam;
extern scanOut_t        scanOutput;
#endif // SCAN_CFG

// Init Data Structures
extern bleOpCmd_t       initCmd;
extern initParam_t      initParam;
extern initOut_t        initOutput;
extern connReqData_t    connReqData[];

// Connection Data Structures
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
extern ble5OpCmd_t      *linkCmd;
#else // !PHY_2MBPS_CFG & !PHY_LR_CFG
extern bleOpCmd_t       *linkCmd;
#endif // PHY_2MBPS_CFG | PHY_LR_CFG
extern linkParam_t      *linkParam;

// Connection Output
extern connOut_t        connOutput;

// Direct Test Mode Data Structures
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG))
extern ble5OpCmd_t      trxTestCmd;
#else // !PHY_2MBPS_CFG & !PHY_LR_CFG
extern bleOpCmd_t       trxTestCmd;
#endif // PHY_2MBPS_CFG | PHY_LR_CFG
extern txTestParam_t    txTestParam;
extern txOut_t          txTestOut;
extern rxTestParam_t    rxTestParam;
extern rxOut_t          rxTestOut;

// Modem Tests (TELECO)
extern rfOpCmd_TxTest_t txModemTestCmd;
extern rfOpCmd_RxTest_t rxModemTestCmd;

/*******************************************************************************
 * APIs
 */

// ISR Callbacks
extern void cpe0IntCback( void );
extern void cpe1IntCback( void );
extern void hwIntCback( void );

// Mailbox
extern void llSetupMailbox( void );

// RF HAL
extern void llSetupRfHal( void );
extern void llPatchCM0( void );

// RAT Channel
extern void llSetupRATChanCompare( uint8 ratChan, uint32 compareTime );
extern void llRatChanCBack_A( void );
extern void llRatChanCBack_B( void );
extern void llRatChanCBack_C( void );
extern void llRatChanCBack_D( void );

// Data Processing
extern dataEntryQ_t *llSetupScanDataEntryQueue( void );
extern dataEntryQ_t *llSetupInitDataEntryQueue( void );
extern dataEntryQ_t *llSetupAdvDataEntryQueue( void );
extern dataEntryQ_t *llSetupDataEntryQueue( void );
extern dataEntryQ_t *llSetupOneDataEntryQueueDyn( void );
extern dataEntryQ_t *llSetupConnRxDataEntryQueue( uint8 connId );
extern uint8         llSetupCteSamplesEntryQueue( uint8 numBuffers );
extern uint8         llFreeCteSamplesEntryQueue( void );
extern void          llAddTxDataEntry( dataEntryQ_t *, dataEntry_t * );
extern void          llProcessSlaveControlPacket( llConnState_t *, uint8 * );
extern void          llProcessMasterControlPacket( llConnState_t *, uint8 * );
extern void          llMoveTempTxDataEntries( llConnState_t * );
extern uint8         HOOK_llProcessSlaveControlPacket( llConnState_t *, uint8 * );
extern uint8         HOOK_llProcessMasterControlPacket( llConnState_t *, uint8 * );
extern void          llSetRfReportAodPackets( void );

/*******************************************************************************
 */

#endif /* BLE_H */
