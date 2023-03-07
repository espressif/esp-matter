/******************************************************************************

 @file  ll_ae.h

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

#ifndef LL_AE_H
#define LL_AE_H

/*******************************************************************************
 * INCLUDES
 */

#include "rf_hal.h"
#include "ble.h"
#include "ll_wl.h"
#include "ll_common.h"
#include "ll_config.h"

// generic remap for Host
#define MAP_LL_SearchAdvSet( h )           MAP_LL_GetAdvSet( (h), LE_SEARCH_ADV_SET )

/*******************************************************************************
 * HCI AE API
 */

/*******************************************************************************
 * CONSTANTS
 */

/*
** LE API Parameters
*/

// Extended Advertising Tx Power
#define AE_TX_POWER_NO_PREFERENCE                           127       // valid values: -127..+126

// Extended Advertising Interval
#define AE_INTERVAL_MIN                                     32        // 20ms in 625us
#define AE_INTERVAL_MAX                                     768000    // 480,000ms in 625us (=480000/0.625).

// Extended Advertising Max Skip and Duration
#define AE_MAX_SKIP_DEFAULT                                 0
#define AE_DURATION_DISABLED                                0

// Extended Advertising Primary PHY
// TI Enhancement - Use as the MSB of the PHY parameter!
#define AE_PHY_NONE                                         0x00
#define AE_PHY_1_MBPS                                       0x01
#define AE_PHY_2_MBPS                                       0x02
#define AE_PHY_CODED                                        0x03
#define AE_PHY_CODED_S8                                     0x03
#define AE_PHY_CODED_S2                                     0x83
//
#define AE_PHY_CODED_SCHEME_MASK                            0x7F

// Secondary Advertising Max Skip
#define AE_MAX_AUX_SKIP_NONE                                0x00

// Advertising SID
#define AE_SID_0                                            0
#define AE_SID_1                                            1
#define AE_SID_2                                            2
#define AE_SID_3                                            3
#define AE_SID_4                                            4
#define AE_SID_5                                            5
#define AE_SID_6                                            6
#define AE_SID_7                                            7
#define AE_SID_8                                            8
#define AE_SID_9                                            9
#define AE_SID_10                                           10
#define AE_SID_11                                           11
#define AE_SID_12                                           12
#define AE_SID_13                                           13
#define AE_SID_14                                           14
#define AE_SID_15                                           15

// Advertising Scan Request Notification Flag
#define AE_NOTIFY_DISABLE_SCAN_REQUEST                      ~BV(0)
#define AE_NOTIFY_ENABLE_SCAN_REQUEST                        BV(0)

// Vendor Specific Notify Flags
#define AE_NOTIFY
#define AE_NOTIFY_DISABLE_ADV_SET_START                     ~BV(4)
#define AE_NOTIFY_ENABLE_ADV_SET_START                       BV(4)
#define AE_NOTIFY_DISABLE_ADV_START                         ~BV(5)
#define AE_NOTIFY_ENABLE_ADV_START                           BV(5)
#define AE_NOTIFY_DISABLE_ADV_END                           ~BV(6)
#define AE_NOTIFY_ENABLE_ADV_END                             BV(6)
#define AE_NOTIFY_DISABLE_ADV_SET_END                       ~BV(7)
#define AE_NOTIFY_ENABLE_ADV_SET_END                         BV(7)

// Advertising Halt Flags
#define AE_HALTED_FALSE                                     ~BV(0)
#define AE_HALTED_TRUE                                       BV(0)
#define AE_DISABLE_DISCONNECT_AUTH_FAIL                     ~BV(1)
#define AE_ENABLE_DISCONNECT_AUTH_FAIL                       BV(1)
/*
** Extended Advertiser
*/

#define AE_MAX_NUM_SID                                      16
#define AE_MAX_ADV_SETS                                     254
#define AE_MAX_ADV_SETS_TO_ENABLE_DISABLE                   63

#if defined( CC26XX ) || defined( CC13XX )
  #if defined( CC26X2 ) || defined( CC13X2 ) || defined( CC13X2P )
    #define AE_MAX_ADV_DATA_LEN                             1650
    #define AE_MAX_SCAN_RSP_DATA_LEN                        1650
    #define AE_MAX_NUM_ADV_SETS                             20
  #else // CC26XX_R2 || CC1350LP_7XD ...
    #define AE_MAX_ADV_DATA_LEN                             300
    #define AE_MAX_SCAN_RSP_DATA_LEN                        300
    #define AE_MAX_NUM_ADV_SETS                             6
  #endif // CC26X2 || CC13X2
#endif // CC26XX || CC13XX

#define AE_MAX_ADV_PAYLOAD_LEN                              255
#define AE_DEFAULT_ADV_DATA_LEN                             AE_MAX_ADV_DATA_LEN
#define AE_DEFAULT_SCAN_RSP_DATA_LEN                        AE_MAX_SCAN_RSP_DATA_LEN
#define AE_DEFAULT_NUM_ADV_SETS                             AE_MAX_NUM_ADV_SETS

// Advertising Event Properties Bit Number
#define AE_PROP_CONNECTABLE_ADVERTISING                     0
#define AE_PROP_SCANNABLE_ADVERTISING                       1
#define AE_PROP_DIRECTED_ADVERTISING                        2
#define AE_PROP_HDC_DIRECTED_CONNECTABLE_ADVERTISING        3 // <=3.75ms Adv Interval
#define AE_PROP_LEGACY_PDU                                  4
#define AE_PROP_OMIT_ADVERTISERS_ADDRESS_FROM_ALL_PDUS      5 // anonymous advertising
#define AE_PROP_INCLUDE_TX_POWER_IN_EXTENDED_HEADER         6

// Advertising Event Properties
#define AE_PROPS_CONN_ADV                                   BV(AE_PROP_CONNECTABLE_ADVERTISING)
#define AE_PROPS_SCAN_ADV                                   BV(AE_PROP_SCANNABLE_ADVERTISING)
#define AE_PROPS_DIR_ADV                                    BV(AE_PROP_DIRECTED_ADVERTISING)
#define AE_PROPS_HDC_DIR_ADV                                BV(AE_PROP_HDC_DIRECTED_CONNECTABLE_ADVERTISING)
#define AE_PROPS_LEGACY                                     BV(AE_PROP_LEGACY_PDU)
#define AE_PROPS_OMIT_ADVA                                  BV(AE_PROP_OMIT_ADVERTISERS_ADDRESS_FROM_ALL_PDUS)
#define AE_PROPS_TX_POWER                                   BV(AE_PROP_INCLUDE_TX_POWER_IN_EXTENDED_HEADER)

// Advertising Event Properties for Legacy PDUs
#define AE_EVT_TYPE_CONN_SCAN_ADV_IND                       AE_PROPS_LEGACY      | \
                                                            AE_PROPS_SCAN_ADV    | \
                                                            AE_PROPS_CONN_ADV    // 0x13: Supported if there's Advertising Data (31 bytes max)
#define AE_EVT_TYPE_CONN_DIR_LDC_ADV_DIRECT_IND             AE_PROPS_LEGACY      | \
                                                            AE_PROPS_DIR_ADV     | \
                                                            AE_PROPS_CONN_ADV    // 0x15: Not supported if there's Advertising Data
#define AE_EVT_TYPE_CONN_DIR_HDC_ADV_DIRECT_IND             AE_PROPS_LEGACY      | \
                                                            AE_PROPS_HDC_DIR_ADV | \
                                                            AE_PROPS_DIR_ADV     | \
                                                            AE_PROPS_CONN_ADV    // 0x1D: Not supported if there's Advertising Data
#define AE_EVT_TYPE_SCAN_UNDIR_ADV_SCAN_IND                 AE_PROPS_LEGACY      | \
                                                            AE_PROPS_SCAN_ADV    // 0x12: Supported if there's Advertising Data (31 bytes max)
#define AE_EVT_TYPE_NONCONN_NONSCAN_ADV_NONCONN_IND         AE_PROPS_LEGACY      // 0x10: Supported if there's Advertising Data (31 bytes max)

// Advertising Mode
#define AE_ADV_MODE_NONCONN_NONSCAN                         0
#define AE_ADV_MODE_CONNECTABLE                             1
#define AE_ADV_MODE_SCANNABLE                               2
#define AE_ADV_MODE_RESERVED                                3
//
#define ADV_MODE_MASK                                       0xC0
#define ADV_MODE_BIT_OFFSET                                 6

// Extended Header Flag Bits
#define EXTHDR_FLAG_ADVA_BIT                                0
#define EXTHDR_FLAG_TARGETA_BIT                             1
#define EXTHDR_FLAG_CTEINFO_BIT                             2
#define EXTHDR_FLAG_ADI_BIT                                 3
#define EXTHDR_FLAG_AUXPTR_BIT                              4
#define EXTHDR_FLAG_SYNCINFO_BIT                            5
#define EXTHDR_FLAG_TXPWR_BIT                               6
#define EXTHDR_FLAG_RFU2_BIT                                7
//
#define EXTHDR_INFO_SIZE                                    1
#define EXTHDR_FLAGS_SIZE                                   1
//
#define EXTHDR_FLAG_ADVA_SIZE                               6
#define EXTHDR_FLAG_TARGETA_SIZE                            6
#define EXTHDR_FLAG_CTEINFO_SIZE                            1
#define EXTHDR_FLAG_ADI_SIZE                                2
#define EXTHDR_FLAG_AUXPTR_SIZE                             3
#define EXTHDR_FLAG_SYNCINFO_SIZE                           18
#define EXTHDR_FLAG_TXPWR_SIZE                              1
#define EXTHDR_FLAG_RFU2_SIZE                               1
#define EXTHDR_FLAG_ACAD_SIZE                               9  //for periodic channel map update indication
//
#define EXTHDR_FLAG_ADVA                                    BV(EXTHDR_FLAG_ADVA_BIT)
#define EXTHDR_FLAG_TARGETA                                 BV(EXTHDR_FLAG_TARGETA_BIT)
#define EXTHDR_FLAG_CTEINFO                                 BV(EXTHDR_FLAG_CTEINFO_BIT)
#define EXTHDR_FLAG_ADI                                     BV(EXTHDR_FLAG_ADI_BIT)
#define EXTHDR_FLAG_AUXPTR                                  BV(EXTHDR_FLAG_AUXPTR_BIT)
#define EXTHDR_FLAG_SYNCINFO                                BV(EXTHDR_FLAG_SYNCINFO_BIT)
#define EXTHDR_FLAG_TXPWR                                   BV(EXTHDR_FLAG_TXPWR_BIT)
#define EXTHDR_FLAG_RFU2                                    BV(EXTHDR_FLAG_RFU2_BIT)
//
#define EXTHDR_MAX_LEN                                      63
#define EXTHDR_LEN_MASK                                     0x3F
#define EXTHDR_LEN_OFFSET                                   0
//
#define EXTHDR_DID_SIZE                                     12 // in bits
#define EXTHDR_DID_MASK                                     0x0FFF
//
#define EXTHDR_ACAD_CHANMAP_UPDATE_SIZE                    0x08
#define EXTHDR_ACAD_CHANMAP_UPDATE_TYPE                    0x28
//
#define EXTHDR_TOTAL_BUF_SIZE                               (EXTHDR_FLAG_ADI_SIZE      +  \
                                                             EXTHDR_FLAG_AUXPTR_SIZE   +  \
                                                             EXTHDR_FLAG_SYNCINFO_SIZE +  \
                                                             EXTHDR_FLAG_TXPWR_SIZE)

#define PERIODIC_ADV_HDR_TOTAL_BUF_SIZE                     (EXTHDR_FLAG_CTEINFO_SIZE  +  \
                                                             EXTHDR_FLAG_AUXPTR_SIZE   +  \
                                                             EXTHDR_FLAG_TXPWR_SIZE)   +  \
                                                             EXTHDR_FLAG_ACAD_SIZE

// Auxilary Offset Units
#define AE_AUX_OFFSET_UNITS_30_US                           0
#define AE_AUX_OFFSET_UNITS_300_US                          1
//
#define AE_AUX_OFFSET_30_US_UNIT_VALUE                      30
#define AE_AUX_OFFSET_300_US_UNIT_VALUE                     300
#define AE_AUX_OFFSET_UNITS_VALUE_DIFF                      (AE_AUX_OFFSET_300_US_UNIT_VALUE - AE_AUX_OFFSET_30_US_UNIT_VALUE)
#define AE_AUX_OFFSET_UNITS_OFFSET                          7
#define AE_AUX_OFFSET_UNIT_CUTOFF_TIME                      0x0003BFC4 // 245,700 us
#define AE_AUX_OFFSET_SIZE                                  13         // in bits
#define AE_AUX_OFFSET_MASK                                  0x1FFF
#define AE_AUX_OFFSET_AUTO_INSERT                           0

// Auxiliary Channel Index
#define AE_CHAN_INDEX_MASK                                  0x3F

// Auxiliary PHY mask
#define AE_PHY_MASK                                         0x3

// Auxiliary syncinfo only mask
#define AE_SYNCINFO_ONLY_MASK                               0x4

// Auxilary Clock Accuracy
#define AE_AUX_CA_51_500_PPM                                0
#define AE_AUX_CA_0_50_PPM                                  1

// Auxilary PHY
#define AE_AUX_1M_PHY                                       BLE5_1M_PHY
#define AE_AUX_2M_PHY                                       BLE5_2M_PHY
#define AE_AUX_CODED_PHY                                    BLE5_CODED_PHY

/*
** Common Extended Advertisnig Payload Format
*/
#define AE_EXT_HDR_LEN_SIZE                                 1
#define AE_EXT_HDR_FLAGS_SIZE                               1

/*
** Advertising and Scan Response Data
*/

// Operation
#define AE_DATA_OP_NEXT_FRAG                                0x00
#define AE_DATA_OP_FIRST_FRAG                               0x01
#define AE_DATA_OP_LAST_FRAG                                0x02
#define AE_DATA_OP_COMPLETE                                 0x03
#define AE_DATA_OP_UNCHANGED                                0x04
//
#define AE_DATA_OP_NO_DATA                                  0xFF

// Fragmentation Preference
#define AE_DATA_FRAG_PREF_CTRL_MAY_FRAG                     0
#define AE_DATA_FRAG_PREF_CTRL_MAY_NOT_FRAG                 1

// Length
#define AE_MAX_ADV_DATA_PARAM_LEN                           251
#define AE_DATA_LEN_IN_PAYLOAD_SIZE                         2     // in bytes

/*
** Extended Scanner
*/

#define AE_EXT_SCAN_MIN_TIME                                4      // in 625us = 2.5ms
#define AE_EXT_SCAN_MAX_TIME                                0xFFFF // in 625us units = 40.959375s

#define AE_EXT_SCAN_DURATION_DISABLED                       0
#define AE_EXT_SCAN_MIN_DURATION                            1      // in 10ms units = 10ms
#define AE_EXT_SCAN_MAX_DURATION                            0xFFFF // in 10ms units = 655.35s

#define AE_EXT_SCAN_PERIOD_DISABLED                         0
#define AE_EXT_SCAN_MIN_PERIOD                              1      // in 1.28s units = 1.28s
#define AE_EXT_SCAN_MAX_PERIOD                              0xFFFF // in 1.28s units = 83,884.8s

// Extended Advertising Report Event Type
#define AE_EVT_TYPE_CONNECTABLE_ADVERTISING                 0
#define AE_EVT_TYPE_SCANNABLE_ADVERTISING                   1
#define AE_EVT_TYPE_DIRECTED_ADVERTISING                    2
#define AE_EVT_TYPE_SCAN_RESPONSE                           3
#define AE_EVT_TYPE_LEGACY_PDU                              4
//
#define AE_EVT_TYPE_CONN_ADV                                BV(AE_EVT_TYPE_CONNECTABLE_ADVERTISING)
#define AE_EVT_TYPE_SCAN_ADV                                BV(AE_EVT_TYPE_SCANNABLE_ADVERTISING)
#define AE_EVT_TYPE_DIR_ADV                                 BV(AE_EVT_TYPE_DIRECTED_ADVERTISING)
#define AE_EVT_TYPE_SCAN_RSP                                BV(AE_EVT_TYPE_SCAN_RESPONSE)
#define AE_EVT_TYPE_LEGACY                                  BV(AE_EVT_TYPE_LEGACY_PDU)

// Advertising Event Types for Legacy PDUs
#define AE_EXT_ADV_RPT_EVT_TYPE_ADV_IND                     AE_EVT_TYPE_LEGACY    | \
                                                            AE_EVT_TYPE_SCAN_ADV  | \
                                                            AE_EVT_TYPE_CONN_ADV  // 0x13
#define AE_EXT_ADV_RPT_EVT_TYPE_DIRECT_IND                  AE_EVT_TYPE_LEGACY    | \
                                                            AE_EVT_TYPE_DIR_ADV   | \
                                                            AE_EVT_TYPE_CONN_ADV  // 0x15
#define AE_EXT_ADV_RPT_EVT_TYPE_SCAN_IND                    AE_EVT_TYPE_LEGACY    | \
                                                            AE_EVT_TYPE_SCAN_ADV  // 0x12
#define AE_EXT_ADV_RPT_EVT_TYPE_NONCONN_IND                 AE_EVT_TYPE_LEGACY    // 0x10
#define AE_EXT_ADV_RPT_EVT_TYPE_SCAN_RSP_ADV_IND            AE_EVT_TYPE_LEGACY    | \
                                                            AE_EVT_TYPE_SCAN_RSP  | \
                                                            AE_EVT_TYPE_SCAN_ADV  | \
                                                            AE_EVT_TYPE_CONN_ADV  // 0x1B
#define AE_EXT_ADV_RPT_EVT_TYPE_SCAN_RSP_ADV_SCAN_IND       AE_EVT_TYPE_LEGACY    | \
                                                            AE_EVT_TYPE_SCAN_RSP  | \
                                                            AE_EVT_TYPE_SCAN_ADV  // 0x1A
// special error case
#define AE_EXT_ADV_RPT_EVT_TYPE_SCAN_RSP                    AE_EVT_TYPE_LEGACY    | \
                                                            AE_EVT_TYPE_SCAN_RSP  // 0x18

#define AE_EXT_ADV_RPT_DIR_ADDR_TYPE_UNRESOLVED_RPA         0xFE

//
#define AE_EVT_TYPE_COMPLETE_MASK                           (~(3 << 5))
#define AE_EVT_TYPE_COMPLETE                                (0 << 5)
#define AE_EVT_TYPE_INCOMPLETE_MORE_TO_COME                 (1 << 5)
#define AE_EVT_TYPE_INCOMPLETE_NO_MORE_TO_COME              (2 << 5)
#define AE_EVT_TYPE_RFU                                     (3 << 5)
//
#define AE_EVNT_INTERNAL_TYPE_PREV_TRUNCATED                (1 << 7)

// LE Extended Advertising Report Event

#define AE_NO_ADDRESS_PROVIDED                              0xFF

/*
** LL API Parameters
*/

// flags to indicate whether to allocate an Adv Set if not found, or return it
#define LE_SEARCH_ADV_SET                                   0
#define LE_ALLOCATE_ADV_SET                                 1

#define LE_COUNT_ALL_ADV_SETS                               0
#define LE_COUNT_ENABLED_ADV_SETS                           1

/*
** Callbacks
*/

// TEMP: THIS IS HCI TERRITORY, BUT EASIER FOR NOW IF DONE IN LL.
#define AE_SCAN_HCI_BLE_EXTENDED_ADV_REPORT_EVENT           0x0D      //!< Extended Adv Report
#define AE_ADV_HCI_BLE_ADV_SET_TERMINATED_EVENT             0x12      //!< LE Advertising Set Terminated Event
#define AE_ADV_HCI_BLE_SCAN_REQUEST_RECEIVED_EVENT          0x13      //!< Scan Request Received

// timing constants
// ALT: Make these values configurable from ll_config? Allow user to set?
#define AE_T_IFS_US                                         150
#define AE_T_MAFS_IN_US                                     300
#define AE_MIN_OFFSET_UNIT_IN_US                             30
#define AE_MAX_OFFSET_UNIT_IN_US                            300
#define AE_1M_OR_CODED_TO_2M_TIME_COMPENSATION_IN_TICKS     7

// constants which are used to determine the time of adv over the air.
#define LEGACY_ADV_MAX_TIME_CONSUME                        3500
#define AE_CONSUME_OVERHEAD                                1500
#define AE_NUM_BYTES_OVERHAED_27_BYTES                       27

// adv sorted list node start time error code
#define AE_INVALID_START_TIME                                 0

#if defined(CC26X2) || defined(CC13X2) || defined(CC13X2P)
  // TEMP: Define substitute for StartSynthToRatOffset, a radio parameter(?).
  #define START_SYNTH_TO_RAT_OFFSET                         166
  #define PRIMARY_CMD_START_SYNTH_TO_RAT_OFFSET             180
  //
  #define AE_MIN_CMD_TIME_IN_US                             300
  #define AE_MIN_T_MAFS_IN_US                               310
#else // !CC26XX && !CC13XX
  // TEMP: Define substitute for StartSynthToRatOffset, a radio parameter(?).
  #define START_SYNTH_TO_RAT_OFFSET                         256
  //
  #define AE_MIN_CMD_TIME_IN_US                             500
  #define AE_MIN_T_MAFS_IN_US                               500
#endif // CC26X2 || CC13X2 || CC13X2P

#define AE_MIN_CMD_TIME_IN_RAT_TICKS                        (4*AE_MIN_CMD_TIME_IN_US)
#define AE_MIN_T_MAFS_IN_RAT_TICKS                          (4*AE_MIN_T_MAFS_IN_US)

// scan timeout flags to track duration/period
#define AE_SCAN_DURATION_TIMEOUT                            0
#define AE_SCAN_PERIOD_TIMEOUT                              1

// scan start state
#define AE_SCAN_START_STATE_FIRST                           0
#define AE_SCAN_START_STATE_SECOND                          1
#define AE_SCAN_START_STATE_NEXT                            2

// RAT Channels
// Note: The Controller will always use Channel 0.
#define RF_RAT_CHAN_0                                       0
#define RF_RAT_CHAN_1                                       1
#define RF_RAT_CHAN_2                                       2

//periodic advertiser state
#define PERIODIC_ADV_STATE_DISABLE                          0
#define PERIODIC_ADV_STATE_PENDING_ENABLE                   1
#define PERIODIC_ADV_STATE_PENDING_TRIGGER                  2
#define PERIODIC_ADV_STATE_ENABLE                           3

//periodic scanner state
#define PERIODIC_SCAN_STATE_IDLE                            0
#define PERIODIC_SCAN_STATE_SYNCING_PENDING                 1
#define PERIODIC_SCAN_STATE_SYNCING_ACTIVE                  2
#define PERIODIC_SCAN_STATE_SYNCED                          3

// Margin time is rf and controller process time in rat ticks (empirical calculation)
#define PERIODIC_ADV_MARGIN_TIME_RAT_TICKS                  2400 + RAT_TICKS_FOR_SCHED_PROCESS_TIME // 800 usec
#define PERIODIC_SCAN_MARGIN_TIME_RAT_TICKS                 1600 + RAT_TICKS_FOR_SCHED_PROCESS_TIME // 600 usec
#define EXT_ADV_MARGIN_TIME_RAT_TICKS                       2800 // 700 usec
#define EXT_SCAN_MARGIN_TIME_RAT_TICKS                      2400 // 350 usec
#define EXT_INIT_MARGIN_TIME_RAT_TICKS                      2400 // 350 usec
#define PERIODIC_SCAN_MAX_MARGIN_TIME_RAT_TICKS             (PERIODIC_SCAN_MARGIN_TIME_RAT_TICKS + LL_JITTER_CORRECTION + LL_RX_RAMP_OVERHEAD + RAT_TICKS_FOR_PERIODIC_SCAN_WIN_SIZE + LL_RX_SYNCH_OVERHEAD) // 2.576ms

#define PERIODIC_SYNCING_LIMIT_NUM_EVENTS                  6
#define PERIODIC_SCAN_MAX_HANDLES                          0x0EFF
#define PERIODIC_SCAN_TERMINATE_LIST_MAX_HANDLES           4
#define PERIODIC_SCAN_TERMINATE_LIST_CREATE_SYNC_INDEX     0
#define PERIODIC_SCAN_TERMINATE_LIST_INVALID_HANDLE        0xFF

/*******************************************************************************
 * @fn          LE Extended Advertising Start After Enable Event Callback
 *
 * @brief       This callback is used to handle the Advertising Start After
 *              Enable event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - Pointer to the advertisement set handle for
 *                        this event, specified by uint8.
 *
 *                        Note: The pointer pParams MUST NOT be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_ADV_START_AFTER_ENABLE                     0

/*******************************************************************************
 * @fn          LE Extended Advertising End After Disable Event Callback
 *
 * @brief       This callback is used to handle the Advertising End After
 *              Disable event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - Pointer to the advertisement set handle for
 *                        this event, specified by uint8.
 *
 *                        Note: The pointer pParams MUST NOT be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_ADV_END_AFTER_DISABLE                      1

/*******************************************************************************
 * @fn          LE Extended Advertising Start Event Callback
 *
 * @brief       This callback is used to handle the Advertising Start event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - Pointer to the advertisement set handle for
 *                        this event, specified by uint8.
 *
 *                        Note: The pointer pParams MUST NOT be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_ADV_START                                  2

/*******************************************************************************
 * @fn          LE Extended Advertising End Event Callback
 *
 * @brief       This callback is used to handle the Advertising End event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - Pointer to the advertisement set handle for
 *                        this event, specified by uint8.
 *
 *                        Note: The pointer pParams MUST NOT be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_ADV_END                                    3

/*******************************************************************************
 * @fn          LE Extended Advertising Report Event Callback
 *
 * @brief       This callback is used to handle the Extended Advertising
 *              Report event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - Pointer aeExtAdvRptEvt to the advertising report
 *                        event data specified by aeExtAdvRptEvt_t.
 *
 *                        Note: The pointer pParams MUST be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_EXT_ADV_REPORT                             4

/*******************************************************************************
 * @fn          LE Advertising Set Terminated Callback
 *
 * @brief       This callback is used to handle the LE Advertising Set
 *              Terminated event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - Pointer aeAdvSetTerm to the advertising set terminated
 *                        event data specified by aeAdvSetTerm_t.
 *
 *                        Note: The pointer pParams MUST be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_ADV_SET_TERMINATED                         5

/*******************************************************************************
 * @fn          LE Extended Scan Request Received Event Callback
 *
 * @brief       This callback is used to handle the Extended Scan Request
 *              Received event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - Pointer aeScanReqReceived to the scan request received
 *                        event, data specified by aeScanReqReceived_t.
 *
 *                        Note: The pointer pParams MUST be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_EXT_SCAN_REQ_RECEIVED                      6

/*******************************************************************************
 * @fn          LE Extended Scan Timeout Event Callback
 *
 * @brief       This callback is used to handle the Extended Scan Timeout
 *              event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - NULL.
 *
 *                        Note: No parameter to be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_EXT_SCAN_TIMEOUT                           7

/*******************************************************************************
 * @fn          LE Extended Scan Start Event Callback
 *
 * @brief       This callback is used to handle the Extended Scan Start event.
 *              This event occurs only once when the Scan starts after it is
 *              enabled.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - NULL.
 *
 *                        Note: No parameter to be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_EXT_SCAN_START                             8

/*******************************************************************************
 * @fn          LE Extended Scan End Event Callback
 *
 * @brief       This callback is used to handle the Extended Scan End event.
 *              This event occurs only once when the Scan ends after it is
 *              disabled.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - NULL.
 *
 *                        Note: No parameter to be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_EXT_SCAN_END                               9

/*******************************************************************************
 * @fn          LE Extended Scan Window End Event Callback
 *
 * @brief       This callback is used to handle the Extended Scan Window End
 *              event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - NULL.
 *
 *                        Note: No parameter to be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_EXT_SCAN_WINDOW_END                        10

/*******************************************************************************
 * @fn          LE Extended Scan Interval End Event Callback
 *
 * @brief       This callback is used to handle the Extended Scan Interval End
 *              event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - NULL.
 *
 *                        Note: No parameter to be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_EXT_SCAN_INTERVAL_END                      11

/*******************************************************************************
 * @fn          LE Extended Scan Duration End Event Callback
 *
 * @brief       This callback is used to handle the Extended Scan Duration End
 *              event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - NULL.
 *
 *                        Note: No parameter to be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_EXT_SCAN_DURATION_END                      12

/*******************************************************************************
 * @fn          LE Extended Scan Period End Event Callback
 *
 * @brief       This callback is used to handle the Extended Scan Period End
 *              event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - NULL.
 *
 *                        Note: No parameter to be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_EXT_SCAN_PERIOD_END                        13

/*******************************************************************************
 * @fn          LE Out of Memory Event Callback
 *
 * @brief       This callback is used to handle an out of heap memory event.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - NULL.
 *
 *                        Note: No parameter to be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_OUT_OF_MEMORY                              14

/*******************************************************************************
 * @fn          LE Extended Advertisment Data Truncated Event Callback
 *
 * @brief       This callback is used to inform the Host that the Extended
 *              Advertising Data was truncated. This can happen when there's
 *              a connectable extended advertisement, and the the data has
 *              been set to a length that exceeds the max available data
 *              possible in the AUX_ADV_IND.
 *
 * @prototype   void <function name>( uint8 eventId, (void *)pParams );
 *
 * input parameters
 *
 * @param       eventId - Specified by this define.
 * @param       pParams - Pointer truncData to the extended advertisment data
 *                        truncated event, data specified by aeAdvTrucData_t.
 *
 *                        Note: The pointer pParams MUST NOT be freed!
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
#define LL_CBACK_EXT_ADV_DATA_TRUNCATED                     15

//
// Register callback with this index clears the callback tables.
//
#define LL_CBACK_CLEAR                                      0xFF

/*******************************************************************************
 * MACROS
 */

// Advertising Event Properties
#define TST_AE_PROPS_CONN( advEvtProps )      ((advEvtProps)[0] & AE_PROPS_CONN_ADV)
#define TST_AE_PROPS_SCAN( advEvtProps )      ((advEvtProps)[0] & AE_PROPS_SCAN_ADV)
#define TST_AE_PROPS_DIR( advEvtProps )       ((advEvtProps)[0] & AE_PROPS_DIR_ADV)
#define TST_AE_PROPS_HDC_DIR( advEvtProps )   ((advEvtProps)[0] & AE_PROPS_HDC_DIR_ADV)
#define TST_AE_PROPS_LEGACY( advEvtProps )    ((advEvtProps)[0] & AE_PROPS_LEGACY)
#define TST_AE_PROPS_OMIT_ADVA( advEvtProps ) ((advEvtProps)[0] & AE_PROPS_OMIT_ADVA)
#define TST_AE_PROPS_TX_PWR( advEvtProps )    ((advEvtProps)[0] & AE_PROPS_TX_POWER)

// Common Extended Advertising Payload Format
#define LL_ADV_MODE( advHdr )                 ((advHdr) >> 6)

// Advertising Event Properties
#define SET_AE_PROPS_FLAG( flags, flag ) ((flags) |= (flag))
#define TST_AE_PROPS_FLAG( flags, flag ) ((flags) & (flag))
#define CLR_AE_PROPS_FLAG( flags, flag ) ((flags) &= ~(flag))

// Extended Header Flags
#define SET_EXTHDR_FLAG( flags, flag ) ((flags) |= (flag))
#define TST_EXTHDR_FLAG( flags, flag ) ((flags) & (flag))
#define CLR_EXTHDR_FLAG( flags, flag ) ((flags) &= ~(flag))

// Extended Header Length and Advertising Mode
#define SET_EXTHDR_LEN( hdr, len ) ( (hdr) = ((hdr) & ~EXTHDR_LEN_MASK) |      \
                                             ((len) &  EXTHDR_LEN_MASK))

#define GET_EXT_HDR_INFO( len, mode ) (((mode) << ADV_MODE_BIT_OFFSET) |       \
                                       ((len) & EXTHDR_LEN_MASK))

#define GET_EXT_HDR_LEN( hdr ) ((hdr) & EXTHDR_LEN_MASK)

#define GET_ADV_MODE( hdr ) ((hdr) >> ADV_MODE_BIT_OFFSET)

#define SET_ADV_MODE( hdr, mode ) ((hdr) = ((hdr) & ~ADV_MODE_MASK) |          \
                                           ((mode) << ADV_MODE_BIT_OFFSET) )

#define CLR_ADV_MODE( hdr ) ((hdr) &= ~ADV_MODE_MASK )

// Advertising Data Information Field
#define GET_SID( adi ) ((adi) >> EXTHDR_DID_SIZE)
#define GET_DID( adi ) ((adi) & EXTHDR_DID_MASK)

//
#define SET_EXT_ADV_HDR_CFG_SKIP_ADVA( c )                                     \
  (c) |= BV(0)

#define SET_EXT_ADV_HDR_CFG_SKIP_TGTA( c )                                     \
  (c) |= BV(1)

#define CLR_EXT_ADV_HDR_CFG_SKIP_TGTA( c )                                     \
  (c) &= ~BV(1)

#define SETVAR_EXT_ADV_HDR_CFG_DEV_ADDR_TYPE( c, v )                           \
  (c) = ((c) & ~BV(2)) | (((v) & 0x01) << 2)

#define SETVAR_EXT_ADV_HDR_CFG_TGT_ADDR_TYPE( c, v )                           \
  (c) = ((c) & ~BV(3)) | (((v) & 0x01) << 3)

// Periodic Scan options
#define GET_PERIODIC_SCAN_OPTIONS_LIST_USE(c)       (((c) >> 0) & 0x01)
#define GET_PERIODIC_SCAN_OPTIONS_REPORT_DISABLE(c) (((c) >> 1) & 0x01)

//Periodic Scan Sync CTE types
#define GET_PERIODIC_CTE_TYPE_SYNC_NO_AOA(c)       (((c) >> 0) & 0x01)
#define GET_PERIODIC_CTE_TYPE_SYNC_NO_1U_AOD(c)    (((c) >> 1) & 0x01)
#define GET_PERIODIC_CTE_TYPE_SYNC_NO_2U_AOD(c)    (((c) >> 2) & 0x01)
#define GET_PERIODIC_CTE_TYPE_SYNC_NO_TYPE_3(c)    (((c) >> 3) & 0x01)
#define GET_PERIODIC_CTE_TYPE_SYNC_ONLY_CTE(c)     (((c) >> 4) & 0x01)


/*******************************************************************************
 * TYPEDEFS
 */


////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

//
// LE Set Advertising Set Random Address Command
//
PACKED_TYPEDEF_STRUCT
{
  uint8 handle;
  uint8 randAddr[B_ADDR_LEN];
} aeRandAddrCmd_t;

//
// LE Set Extended Advertising Parameters Command
//
PACKED_TYPEDEF_STRUCT
{
  // Command Parameters
  uint8 handle;
  uint8 eventProps[2];
  uint8 primIntMin[3];
  uint8 primIntMax[3];
  uint8 primChanMap;
  uint8 ownAddrType;
  uint8 peerAddrType;
  uint8 peerAddr[B_ADDR_LEN];               // must be word aligned!
  uint8 filterPolicy;
  int8  txPower;
  uint8 primPhy;
  uint8 secMaxSkip;
  uint8 secPhy;
  uint8 sid;
  uint8 notifyEnableFlags;                  // scan request notification
} aeSetParamCmd_t;

//
// LE Set Extended Advertising Parameters Return Parameters
//
PACKED_TYPEDEF_STRUCT
{
  int8 txPower;
} aeSetParamRtn_t;

//
// LE Set Extended Advertising and Scan Response Data Command
//
PACKED_TYPEDEF_STRUCT
{
  // Command Parameters
  uint8  handle;
  uint8  operation;                         // AE_DATA_OP_COMPLETE or AE_DATA_OP_UNCHANGED
  uint8  fragPref;                          // AE_DATA_FRAG_PREF_CTRL_MAY_FRAG or AE_DATA_FRAG_PREF_CTRL_MAY_NOT_FRAG
  uint16 dataLen;                           // length of Adv or Scan Rsp data
  uint8 *pData;                             // ptr to Host data, or NULL
} aeSetDataCmd_t;

//
// LE Set Extended Advertising Enable Command
//
PACKED_TYPEDEF_STRUCT
{
  // Command Parameters
  uint8  enable;
  uint8  numSets;
  uint8  handle;
  uint16 duration;                           // advertising duration
  uint8  maxEvents;                          // number of events allowed; 0=infinite
} aeEnableCmd_t;

//
// LE Set Extended Scan Parameters Command
//
PACKED_TYPEDEF_STRUCT
{
  uint8  scanType;                          // active or passive
  uint16 scanInterval;                      // 2.5ms..40.959375s (625us units)
  uint16 scanWindow;                        // 2.5ms..40.959375s (625us units)
} extScan_t;

PACKED_TYPEDEF_STRUCT
{
  uint8     ownAddrType;
  uint8     scanFilterPolicy;
  uint8     scanPhys;                       // bit 0: 1M, bit 2: Coded
  extScan_t extScanParam[2];
} aeSetScanParamCmd_t;

//
// LE Set Extended Scan Enable Command
//
PACKED_TYPEDEF_STRUCT
{
  uint8  enable;
  uint8  dupFiltering;
  uint16 duration;                          // 10ms..655.35s (10ms units)
  uint16 period;                            // 1.28s..83,884s (1.28s units)
} aeEnableScanCmd_t;

//
// LE Extended Create Connection Command
//
PACKED_TYPEDEF_STRUCT
{
  uint16 scanInterval;                      // 2.5ms..40.959375s (625us units)
  uint16 scanWindow;                        // 2.5ms..40.959375s (625us units)
  uint16 connIntMin;                        // 7.5ms..4s (1.25ms units)
  uint16 connIntMax;                        // 7.5ms..4s (1.25ms units)
  uint16 connLatency;                       // 0..499
  uint16 connTimeout;                       // 100ms..32s (10ms units)
  uint16 minLength;                         // 0ms..40.959375s (625us units)
  uint16 maxLength;                         // 0ms..40.959375s (625us units)
} extInit_t;

PACKED_TYPEDEF_STRUCT
{
  uint8     initFilterPolicy;
  uint8     ownAddrType;
  uint8     peerAddrType;
  uint8     peerAddr[B_ADDR_LEN];
  uint8     initPhys;                       // bit 0: 1M, bit 1: 2M, bit 2: Coded
  extInit_t extInitParam[LL_PHY_NUMBER_OF_PHYS];
} aeCreateConnCmd_t;

//
// LE Extended Advertising Report Event
//
PACKED_TYPEDEF_STRUCT
{
  uint8  subCode;                           // HCI_BLE_EXTENDED_ADV_REPORT_EVENT
  uint8  numRpts;                           // only a value of 1 is supported
  uint16 evtType;
  uint8  addrType;
  uint8  addr[B_ADDR_LEN];
  uint8  primPhy;                           // 0: 1M, 3: Coded
  uint8  secPhy;                            // 0: 1M, 2: 2M, 3: Coded
  uint8  advSid;                            // 0..15
  int8   txPower;                           // -127..126, or 127: no Tx Power
  int8   rssi;                              // -127..126, or 127: no RSSI
  uint16 periodicAdvInt;                    // 7.5ms..81,918.75s (1.25ms units), 0=No periodic advertising
  uint8  directAddrType;
  uint8  directAddr[B_ADDR_LEN];
  uint8  dataLen;                           // 0..229 bytes
  uint8  *pData;
  // Vendor Specific Information
  uint16 advDid;
} aeExtAdvRptEvt_t;

//
// LE Advertising Set Terminated Event
//
PACKED_TYPEDEF_STRUCT
{
  uint8  subCode;                           // HCI_BLE_ADV_SET_TERMINATED_EVENT
  uint8  status;
  uint8  handle;
  uint16 connHandle;
  uint8  numCompAdvEvts;
} aeAdvSetTerm_t;

//
// LE Scan Request Received Event
//
PACKED_TYPEDEF_STRUCT
{
  uint8  subCode;                           // HCI_BLE_SCAN_REQUEST_RECEIVED_EVENT
  uint8  handle;
  uint8  scanAddrType;
  uint8  scanAddr[B_ADDR_LEN];
  uint8  channel;
  int8   rssi;
} aeScanReqReceived_t;

//
// LE Scan Timeout Event
//
PACKED_TYPEDEF_STRUCT
{
  uint8  subCode;                           // HCI_BLE_SCAN_TIMEOUT_EVENT
} aeScanTimeout_t;

//
// Vendor Specific LE Advertising Set Adv Truncated Data Event
//
PACKED_TYPEDEF_STRUCT
{
  uint8   handle;
  uint16  advDataLen;
  uint8   availAdvDataLen;
} aeAdvTrucData_t;

//
// Register Callback Command
//
PACKED_TYPEDEF_STRUCT
{
  uint8 cBackID;
  void *pCBack;
} aeCBack_t;

/*
** BLE Input Command Parameter Structures
*/

// Extended Advertiser Command Parameters
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  extAdvCfg_t   advCfg;                // W:  advertiser configuration
  uint8         reserved[2];           // unused
  uint8         auxPtrTgtType;         // W:  reference for auxPtrTgtTime
  uint32        auxPtrTgtTime;         // W:  start time of Aux Pkt
  uint8        *pAdvPkt;               // W:  ptr to Ext Adv Pkt (ADV_EXT_IND)
  uint8        *pDeviceAddr;           // W:  ptr to device BLE address
} extAdvCmd_t;

// Periodic Advertiser Command Parameters
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint8         reserved[3];           // unused
  uint8         auxPtrTgtType;         // W:  reference for auxPtrTgtTime
  uint32        auxPtrTgtTime;         // W:  start time of Aux Pkt
  uint8        *pAdvPkt;               // W:  ptr to Ext Adv Pkt (ADV_EXT_IND)
  uint32        accessAddress;         // W:  Access address used on the periodic advertisement
  uint8         crcInit0;              // W:  CRC initialization value used on the periodic advertisement least significant byte
  uint8         crcInit1;              // W:  CRC initialization value used on the periodic advertisement middle byte
  uint8         crcInit2;              // W:  CRC initialization value used on the periodic advertisement most significant byte
  uint8         reserved2;             // unused
} periodicAdvCmd_t;

// Secondary Channel Advertiser Command Parameters
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  dataEntryQ_t *pRXQ;                  // W:  ptr to Rx queue
  rxQCfg_t      rxCfg;                 // W:  rx queue configuration
  advCfg_t      advCfg;                // W:  advertiser configuration
  uint8         reserved;              // unused
  uint8         auxPtrTgtType;         // W:  reference for auxPtrTgtTime
  uint32        auxPtrTgtTime;         // W:  start time of Aux Pkt
  uint8        *pAdvPkt;               // W:  ptr to Ext Adv Pkt (ADV_AUX_IND)
  uint8        *pRspPkt;               // W:  ptr to Ext Adv Pkt (AUX_SCAN_RSP, AUX_CONNECT_RSP)
  uint8        *pDeviceAddr;           // W:  ptr to device BLE address
  wlEntry_t    *pWhiteList;            // W:  ptr to white list
} secChanAdvCmd_t;

// Common Extended Packet Entry Format
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint8         extHdrInfo;            // W:  advMode(7..6), lenth(5..0)
  uint8         extHdrFlags;           // W:  ext hdr flags per spec
  uint8         extHdrConfig;          // W:  ext hdr configuration
  uint8         advDataLen;            // W:  size of Adv data
  uint8        *pExtHeader;            // W:  ptr to buffer with ext hdr
  uint8        *pAdvData;              // W:  ptr to adv data
} comExtPktFormat_t;

// Extended Advertiser Output Structure
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  uint16        nTxAdv;                // RW: num ADV*_IND Tx pkts
  uint8         nTxScanRsp;            // RW: num *SCAN_RSP Tx pkts
  uint8         nRxScanReq;            // RW: num *SCAN_REQ okay Rx pkts
  uint8         nRxConnReq;            // RW: num *CONNECT_IND okay Rx pkts
  uint8         nTxConnRsp;            // RW: num Tx AUX_CONNECT_RSP
  uint16        nRxNok;                // RW: num not okay Rx pkts
  uint16        nRxIgn;                // RW: num okay Rx pkts ignored
  uint8         nRxBufFull;            // RW: num pkts discarded
  uint8         lastRssi;              // R:  RSSI of last Rx pkt
  uint32        timeStamp;             // R:  timestamp of last Rx pkt
} extAdvOut_t;

// Total AE Memory Allocation for RF
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  ble5OpCmd_t         extRfCmd;
  rfOpCmd_cntBranch_t countBCmd;
  ble5OpCmd_t         auxRfCmd;
  rfOpCmd_Count_t     countCmd;
  extAdvCmd_t         extRfParam;
  secChanAdvCmd_t     auxRfParam;
  comExtPktFormat_t   comPkt;
  extAdvOut_t         comOutput;
  uint8               extHdr[EXTHDR_TOTAL_BUF_SIZE];
} aeRf_t;

// Total AE Legacy Memory Allocation for RF
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  bleOpCmd_t          advCmd[LL_MAX_NUM_ADV_CHAN];
  advParam_t          advParam;
  advOut_t            advOutput;
} aeLegacyRf_t;

typedef union
{
  aeLegacyRf_t aeRfLegacyCmd;
  aeRf_t       aeRfCmd;
} aeRfCmdSize_t;

// Periodic Scanner Command Parameters
PACKED_ALIGNED_TYPEDEF_STRUCT
{
  dataEntryQ_t *pRXQ;                  // W:  ptr to Rx queue
  rxQCfg_t      rxCfg;                 // W:  rx queue configuration
  scanCfg_t     scanCfg;               // W:  advertiser configuration
  uint8         reserved[10];          // unused
  uint8        *pDeviceAddr;           // W:  ptr to device address
  uint32        accessAddress;         // W:  Access address used on the periodic advertisement
  uint8         crcInit0;              // W:  CRC initialization value used on the periodic advertisement – least significant byte
  uint8         crcInit1;              // W:  CRC initialization value used on the periodic advertisement – middle byte
  uint8         crcInit2;              // W:  CRC initialization value used on the periodic advertisement – most significant byte
  uint8          __dummy3;             // unused
  uint16        maxWaitForAux;         // W:  max wait time to secondary channel
  trig_t        timeoutTrig;           // W:  timeout trig for first Rx operation
  trig_t        endTrig;               // W:  end trig for connection event
  uint32        timeoutTime;           // W:  time for timeout trigger
  uint32        endTime;               // W:  time for end trigger
  uint32        rxStartTime;           // R:  time needed to start Rx
  uint16        rxListenTime;          // R:  time needed to listen
  uint8         chan;                  // R:  secondary channel number
  uint8         phyMode;               // R:  secondary channel PHY
}periodicScanParam_t;

/*
** Controller Extended Advertising Set
*/

typedef struct advSet_t advSet_t;

// Extended Advertising Set Entry
struct advSet_t
{
  // Controller Internal
  advSet_t       *next;                           // ptr to next handle in list, if any
  taskInfo_t     *llTask;                         // pointer to associated BLE task block
  uint8           advMode;                        // flag to indicate if currently advertising
  uint8           paramValid;                     // flag to indicate paraetmers are valid
  uint8           connId;                         // allocated connection ID
  uint8           firstAdvEvt;                    // flag indicating first advertisement event
  uint8           maxAdvEvts;                     // maximum number of AE events, or continuous                                  
  uint8           advHalted;                      // in order to save memory (avoid patching) advHalted was transformed from boolean to bitmap
                                                  // | reserved | disconnect, reason 0x05 | radio was halted |
                                                  // |   2..7   |          1              |         0        |
  uint8           advEvtType;                     // legacy Adv Event Type based on properties
  uint32          advStartTime;                   // start time of advertisement event
  // Primary Channel - ADV_EXT_IND
  uint8           extHdrInfo;                     // Adv Mode (7..6), Ext Hdr Len (5..0)
  uint8           extHdrFlags;                    // extended header flags
  uint8           extHdrSize;                     // extended header length
  uint8           numPrimChans;                   // number of primary adv channels
  uint8           firstPrimChan;                  // first primary channel
  uint16          otaTimeExtAdv;                  // total OTA time in us per length and PHY
  uint16          otaTimeAdjust;                  // OTA time of one length + interpackt time (us)
  // Secondary Channel - AUX_ADV_IND, AUX_CHAIN_IND, AUX_SCAN_RSP, AUX_SYNC_IND
  uint8           auxHdrInfo;                     // Adv Mode (7..6), Ext Hdr Len (5..0)
  uint8           auxHdrFlags;                    // secondary channel header flags
  uint8           auxExtHdrSize;                  // aux extended header length
  uint8           auxChanIndex;                   // aux channel index
  uint16          auxChanCounter;                 // channelization algo 2 counter
  uint16          otaTimeAuxAdv;                  // total OTA time in us per length and PHY
  uint16          otaTimeAuxAdvScan;              // total OTA time in us per length and PHY for first Scannable Aux
  uint8           minTimeAdj;                     // total OTA time modulo min offset
  // Common Packet
  uint8           txPowerIndex;                   // tx power table index
  uint16          scaValue;                       // Slave SCA in PPM
  uint8          *pOwnRandAddr;                   // Host defined random address (for LE_SetAdvSetRandAddr)
  // Note: Address must start on word boundary!
  uint8           ownAddr[ LL_DEVICE_ADDR_LEN ];  // own device address
  uint16          durationLeft;                   // remaining duration
  // Note: Address must start on word boundary!
  uint8           peerAddr[ LL_DEVICE_ADDR_LEN ]; // peer device address
  uint8           ownAddrType;                    // own device address type of public or random
  uint8           peerAddrType;                   // peer device address type of public or random
  // Advertising Data
  uint16          adi;                            // Advertising Information (ADI)
  uint8           maxAvailData;                   // max space available for adv data in pkt
  uint8           fragLen;                        // length of fragment
  uint8           lastFragLen;                    // length of last fragment
  uint8           numFrags;                       // number of advertising data fragments
  uint16          dataLen;                        // original length of data
  uint8          *pData;                          // pointer to raw data
  // ISR Related
  uint8           txCount;                        // track number of Tx Done interrupts
  // RF Related
  // Note: Pointer to aeRf_t for AE, and aeLegacyRf_t for AE Legacy.
  uint8          *pRfCmds;                        // ptr to allocated RF command memory
  //
  // Host Extended Advertising Parameters
  aeSetParamCmd_t *pAdvParam;                     // ptr to Host provided params
  aeSetDataCmd_t  *pAdvData;                      // ptr to Host provided adv data params
  aeSetDataCmd_t  *pScanRspData;                  // ptr to Host provided scan response data params
  aeEnableCmd_t   *pEnable;                       // ptr to Host provided enable params

  // extended adv rf commands for the primary channels
  ble5OpCmd_t     extAdvPrimaryRfCmd[2];          // exist 1 rf command so we need 2 to complete the max primary channels.

  bleOpCmd_t      advHdcCmd;                      // high duty cycle first rf command
  uint32          durationExpireTime;             // duration end time in RF ticks

  // Extended advertise Priority
  uint8           priority;                       // Extended Advertise Priority.
};

typedef struct sortedAdv_t sortedAdv_t;
// Structure to represent each node in Adv sorted list
struct sortedAdv_t
{
  advSet_t    *AdvEntry;
  uint16       timeConsume;
  uint32       timeScheduled;                 // last successful scheduled start time
  sortedAdv_t *next;
};

// Extended Scanner Information
typedef struct
{
  taskInfo_t *llTask;                         // pointer to associated task block
  // Note: Address must start on word boundary!
  uint8        ownAddr[ LL_DEVICE_ADDR_LEN ]; // own device address
  uint8        ownAddrType;                   // own device address type of public or random
  uint8        paramValid;                    // flag to indicate paraetmers are valid
  uint8        scanMode;                      // flag to indicate if currently scanning
  //
  uint32       scanStartTime;                 // start time of scanner event
  uint32       scanPeriodLeft;                // Period - Duration
  uint8        scanStartState;                // flag to indicate start state
  uint8        timingFlag;                    // flag to indicate duration or period end
  uint8        lastLegacyAdv;                 // used to track last received legacy Adv PDU
  //
  // Host Extended Scanner Parameters
  aeSetScanParamCmd_t *pScanParam;            // ptr to Host provided params
  aeEnableScanCmd_t   *pEnable;               // ptr to Host provided enable params
  //
  // Extended Scan Priority
  uint8                priority;              // Extended Scan Priority as a Secondary Task.
} extScanInfo_t;

// Extended Initiator Event Information
typedef struct
{
  taskInfo_t *llTask;                         // pointer to associated task block
  // Note: Address must start on word boundary!
  uint8       ownAddr[ LL_DEVICE_ADDR_LEN ];  // own device address
  uint8       ownAddrType;                    // own device address type of public or random
  uint8       scanMode;                       // flag to indicate if currently scanning
  uint32      initStartTime;                  // start time of initiator event
  uint8       connId;                         // allocated connection ID
  uint8       scaValue;                       // Master SCA as an ordinal value for PPM
  //
  // Host Extended Initiator Parameters
  aeCreateConnCmd_t *pCreateConn;             // ptr to Host provided enable params
  //
  // Extender Initiator Priority
  uint8             priority;                // Extender Initiator Priority as a Secondary Task.
} extInitInfo_t;

// Callback Table
// WARNING: Do not rearrange the order of these pointers without changing their
//          corresponding indices!
typedef struct
{
  union
  {
    uint8  numCBack;
    uint32 reserved;
  };
  void *pCBackAdvStartAfterEnable;    // LL_CBACK_ADV_START_AFTER_ENABLE
  void *pCBackAdvEndAfterDisble;      // LL_CBACK_ADV_END_AFTER_DISABLE
  void *pCBackAdvStart;               // LL_CBACK_ADV_START
  void *pCBackAdvEnd;                 // LL_CBACK_ADV_END
  //
  void *pCBackExtAdvReport;           // LL_CBACK_EXT_ADV_REPORT
  void *pCBackAdvSetTerminated;       // LL_CBACK_ADV_SET_TERMINATED
  void *pCBackExtScanReqReceived;     // LL_CBACK_EXT_SCAN_REQ_RECEIVED
  void *pCBackExtScanTimeout;         // LL_CBACK_EXT_SCAN_TIMEOUT
  //
  void *pCBackExtScanStart;           // LL_CBACK_EXT_SCAN_START
  void *pCBackExtScanEnd;             // LL_CBACK_EXT_SCAN_END
  void *pCBackExtScanWindowEnd;       // LL_CBACK_EXT_SCAN_WINDOW_END
  void *pCBackExtScanIntervalEnd;     // LL_CBACK_EXT_SCAN_INTERVAL_END
  void *pCBackExtDurationEnd;         // LL_CBACK_EXT_SCAN_DURATION_END
  void *pCBackExtPeriodEnd;           // LL_CBACK_EXT_SCAN_PERIOD_END
  //
  void *pCBackOutOfMemory;            // LL_CBACK_OUT_OF_MEMORY
  void *pCBackAdvDataTruncated;       // LL_CBACK_EXT_ADV_DATA_TRUNCATED
  //
} aeCBackTbl_t;

// Generic Callback Function Type
typedef void (*pCBack_t)(uint8 cBackID, void *pParams);


////////////////////////////////////////////////////////
//// Periodic Advertising structures ///////////////////
////////////////////////////////////////////////////////

typedef struct
{
  uint16                            intervalMin;           // Minimum advertising interval for periodic advertising. (Range: 0x0006 to 0xFFFF) Time = N * 1.25 ms
  uint16                            intervalMax;           // Maximum advertising interval for periodic advertising. (Range: 0x0006 to 0xFFFF) Time = N * 1.25 ms
  uint8                             props[2];              // Set bit number 6 for include TxPower in the advertising PDU
} llPeriodicAdvParamsCmd_t;


typedef struct
{
  uint32                            packetOffset;          // Time from a reference point to the start of the AUX_SYNC_IND in usec
  uint32                            accessAddr;            // access address
  uint16                            eventCounter;          // AUX_SYNC_IND counter in sync info
  uint8                             offsetUnit;            // packet offset unit - 0 = 30 usec; 1 = 300 usec
  uint8                             crcInit[LL_PKT_CRC_LEN];// CRC init value
  uint8                             sca;                   // worst case sleep clock accuracy
} llPeriodicAdvSyncInfo_t;

//Connectionless CTE struct
typedef struct
{
  uint8                             enable;                // CTE enable or disable received from Host
  uint8                             pending;               // pending to enable or disable the CTE after received from Host
  uint8                             type;                  // CTE type and use as flag indicates that CTE Params was set by Host
  uint8                             len;                   // CTE length
  uint8                             count;                 // number of CTE's to transmit/receive in the same periodic event
  llCteAntSwitch_t                  *pAntenna;             // antenna switch pattern
} llConnectionlessCte_t;

typedef struct
{
  uint8                             bitmap[LL_NUM_BYTES_FOR_CHAN_MAP];     // channel map bits
  uint8                             numUsedChans;                          // count of the number of usable data channels
} llPeriodicChanMap_t;

typedef struct llPeriodicAdvSet_t
{
  struct llPeriodicAdvSet_t         *next;                 // Pointer to next handle in list, if any
  ble5OpCmd_t                       rfCmd;                 // Periodic Advertiser Command
  rfOpCmd_Count_t                   rfCount;               // set the counter for rf chain commands
  periodicAdvCmd_t                  rfParam;               // Periodic Advertiser Command Parameters
  comExtPktFormat_t                 rfPkt;                 // set packet format for rf command
  extAdvOut_t                       rfOutput;              // hold return info from rf
  llPeriodicAdvParamsCmd_t          paramsCmd;             // Pointer to periodic params command
  aeSetDataCmd_t                    dataCmd;               // Pointer to periodic data command
  llPeriodicAdvSyncInfo_t           syncInfo;              // Sync information in AUX_ADV_IND
  llConnectionlessCte_t             cteInfo;               // transmit CTE info struct
  uint32                            startTime;             // start of periodic event
  uint32                            totalOtaTime;          // total OTA time in RAT ticks per length and PHY
  llPeriodicChanMap_t               *pChanMap;             // pointer to current channel map
  uint8                             *pData;                // pointer to periodic data
  uint16                            otaTime;               // OTA time of single packet in us per length and PHY
  uint16                            interval;              // Periodic advertising interval. (Range: 7.5 ms to 81918 ms)
  uint16                            numMissed;             // number of missed adv events
  uint16                            eventCounter;          // incremental counter for each AUX_SYNC_IND
  uint16                            chanMapUpdateEvent;    // channel map update instant
  uint16                            dataLen;               // length of periodic data
  uint8                             dataUpdated;           // Flag indication for updated periodic data by Host
  uint8                             maxAvailData;          // max space available for adv data in pkt
  uint8                             txCount;               // hold number of tx in current periodic event
  uint8                             fragLen;               // length of fragment
  uint8                             lastFragLen;           // length of last fragment
  uint8                             numFrags;              // number of advertising data fragments
  uint8                             numChains;             // number of advertising fragments (max of data or cte count)
  uint8                             handle;                // Used to identify a periodic advertising train (Range: 0x00 to 0xEF)
  uint8                             state;                 // Enable, disable or pending the periodic advertising
  uint8                             currentChan;           // the current unmapped channel for the periodic adv
  uint8                             txPowerIndex;          // tx power table index
  uint8                             phy;                   // phy used in periodic train
  uint8                             pendingDisable;        // disable periodic adv flag
  uint8                             pendingChanUpdate;     // flag to indicate channel map update is pending
  uint8                             intPriority;           // internal priority: priority scale to use in periodic adv selection procedure
  uint8                             extHdrSize;            // periodic adv extended header size
  uint8                             extHdr[PERIODIC_ADV_HDR_TOTAL_BUF_SIZE]; // buffer for periodic header
  uint8                             priority;              // priority as a secondary task.

} llPeriodicAdvSet_t;

typedef struct
{
  llPeriodicChanMap_t               current;               // current channel map
  llPeriodicChanMap_t               next;                  // new channel map
  uint8                             updated;               // channel map was updated
} llPeriodicAdvChanMap_t;

typedef struct
{
  llPeriodicAdvSet_t                *advList;              // periodic advertisers list
  llPeriodicAdvSet_t                *currentAdv;           // current active periodic set
  taskInfo_t                        *llTask;               // Pointer to associated BLE task block (define new task with ID LL_TASK_ID_PERIODIC_ADV 0x08)
  llPeriodicAdvChanMap_t            chanMap;               // channel map struct
  uint8                             advNumActive;          // current number of active periodic sets
} llPeriodicAdv_t;

////////////////////////////////////////////////////////
//// Periodic Advertising Scanner structures ///////////
////////////////////////////////////////////////////////

typedef struct
{
  uint8                             addr[LL_DEVICE_ADDR_LEN]; // Advertiser address
  uint8                             addrType;                 // Advertiser address type
  uint8                             sid;                      // Advertiser SID retrieved from the ADI field
  uint8                             options;                  // Bitwise options - use list or/and disable reports
  uint8                             cteType;                  // Bitwise packet filter according to CTE type
  uint16                            skip;                     // Max number of events that can be skipped after a successful receive
  uint16                            timeout;                  // Synchronization timeout = N*10 ms
} llPeriodicScanSyncCmd_t;

typedef struct
{
  dataEntry_t entry;
  union
  {
    uint8  data[ LL_PKT_HDR_LEN + MAX_BLE_ADV_PKT_SIZE + SUFFIX_MAX_SIZE ];
    uint32 reserved;
  };
} llPeriodicScanDataEntry_t;

typedef struct llPeriodicScanSet_t
{
  struct llPeriodicScanSet_t        *next;                 // Pointer to next handle in list, if any
  ble5OpCmd_t                       rfCmd;                 // Periodic Scanner RF Command
  periodicScanParam_t               rfParam;               // Periodic Scanner RF Command Parameters
  llPeriodicScanSyncCmd_t           syncCmd;               // current periodic syncing params command
  llPeriodicAdvSyncInfo_t           syncInfo;              // Sync information in AUX_ADV_IND
  llPeriodicAdvChanMap_t            chanMap;               // channel map received from advertiser
  llConnectionlessCte_t             cteInfo;               // CTE parameters setup by host
  llCteSampleConfig_t               cteConfig;             // CTE sample Host configuration
  uint32                            startTime;             // scanning start time in ticks
  uint32                            totalOtaTime;          // total OTA time in RAT ticks per length and PHY
  uint16                            handle;                // identify a periodic advertising train (Range: 0x00 to 0xEF)
  uint16                            interval;              // Periodic advertising interval. (Range: 7.5 ms to 81918 ms)
  uint16                            eventCounter;          // incremental counter for each periodic event
  uint16                            numMissed;             // number of consecutive missed periodic events
  uint16                            chanMapUpdateEvent;    // channel map update instant
  int16                             driftFactor;           // drift time in RAT ticks per event (negative or positive value depends on the drift direction)
  uint8                             state;                 // periodic advertising scanner status
  uint8                             phy;                   // advertiser phy
  uint8                             reportEnable;          // send scan report to host flag
  uint8                             terminate;             // flag hold the reason for the termination if there is
  uint8                             ownAddrType;           // periodic scanner address type (public or random)
  uint8                             rxCount;               // number of received packets in current periodic event
  uint8                             cteRssiAntenna;        // first antenna which rssi was measured on while CTE sampling.
  uint8                             intPriority;           // internal priority: priority scale to use in periodic scan selection procedure
  uint8                             driftLearnCounter;     // counts number of valid events used for drift calculation (range 0 to PERIODIC_SCAN_DRIFT_LEARNING_MAX_NUM)
  uint8                             priority;              // priority as a secondary task.

} llPeriodicScanSet_t;

typedef struct llPeriodicWhiteListItem_t
{
  uint8                             addr[B_ADDR_LEN];     // Peer address
  uint8                             addrType;             // Peer address type
  uint8                             sid;                  // Peer SID
  struct llPeriodicWhiteListItem_t  *next;                // Pointer to next item or NULL
} llPeriodicWhiteListItem_t;

typedef struct
{
  uint8                             numItems;             // number of items in the white list
  llPeriodicWhiteListItem_t         *itemList;            // white list head
} llPeriodicWhiteList_t;

typedef struct
{
  llPeriodicScanSet_t               *scanList;             // periodic advertising scanners list
  llPeriodicScanSet_t               *currentScan;          // current active periodic advertising scanner
  llPeriodicScanSet_t               *createSync;           // pointer to periodic scan set which in create sync process active periodic advertiser
  taskInfo_t                        *llTask;               // Pointer to associated BLE task block (define new task with ID LL_TASK_ID_PERIODIC_SCANNER 0x10)
  dataQ_t                           queue;                 // shared queue for all periodic scanners
  llPeriodicScanDataEntry_t         rxBuf[ NUM_RX_SCAN_ENTRIES ];// shared buffers for all periodic scanners
  extScanOut_t                      rfOutput;              // shared Periodic Scanner RF Command Output
  llPeriodicWhiteList_t             whiteList;             // Periodic Scanner white list
  uint8                             scanNumActive;         // current number of active periodic scanners
  uint8                             terminateList[PERIODIC_SCAN_TERMINATE_LIST_MAX_HANDLES]; // terminate handle array (first index reserved for create sync cancel)
} llPeriodicScan_t;


/*******************************************************************************
 * LOCAL VARIABLES
*/

/*******************************************************************************
 * GLOBAL VARIABLES
 */

extern aeCBackTbl_t    aeCBackTbl;

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
extern advSet_t       *advSetList;
extern uint8           aeCurHandle;
extern uint8           aeCurAdvEnableHandle;
extern uint8           aeCurConnHandle;
extern uint8           aeSlaveSCA;
#endif // ADV_NCONN_CFG | ADV_CONN_CFG

#if (defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)) || (FLASH_ROM_BUILD) 
extern extScanInfo_t  *extScanInfo;
#endif // SCAN_CFG || FLASH_ROM_BUILD

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
extern uint8           extScanIndex;
extern uint16          extScanNumMissed;
extern uint8           extScanPriority;
//
extern ble5OpCmd_t     extScanCmd;
extern adiList_t       adiList[AE_MAX_NUM_SID];
extern extScanParam_t  extScanParam;
extern extScanOut_t    extScanOutput;
extern uint8           extScanChanMap;
#endif // SCAN_CFG

#if (defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)) || (FLASH_ROM_BUILD) 
extern extInitInfo_t  *extInitInfo;
#endif // INIT_CFG || FLASH_ROM_BUILD

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
extern uint8           extInitIndex;
//
extern ble5OpCmd_t     extInitCmd;
extern extInitParam_t  extInitParam;
extern extScanOut_t    extInitOutput;
//
#endif // INIT_CFG

/*******************************************************************************
 * HCI AE API
 */

extern llStatus_t    LE_SetAdvSetRandAddr( aeRandAddrCmd_t * );
extern llStatus_t    LE_SetExtAdvParams( aeSetParamCmd_t *, aeSetParamRtn_t * );
extern llStatus_t    LE_SetExtAdvData( aeSetDataCmd_t * );
extern llStatus_t    LE_SetExtScanRspData( aeSetDataCmd_t * );
extern llStatus_t    LE_SetExtAdvEnable( aeEnableCmd_t * );
extern llStatus_t    LE_RemoveAdvSet( uint8 );
extern llStatus_t    LE_ClearAdvSets( void );
extern uint16        LE_ReadMaxAdvDataLen( void );
extern uint8         LE_ReadNumSupportedAdvSets( void );
extern llStatus_t    LE_AE_SetData( aeSetDataCmd_t *, uint8 );
extern llStatus_t    LE_SetExtScanParams( aeSetScanParamCmd_t * );
extern llStatus_t    LE_SetExtScanEnable( aeEnableScanCmd_t * );
extern llStatus_t    LE_ExtCreateConn( aeCreateConnCmd_t * );

/*******************************************************************************
 * LL AE API
 */
extern llStatus_t    LL_AE_Init( void );
extern llStatus_t    LL_AE_RegCBack( uint8 cBackId, void * );
extern advSet_t     *LL_GetAdvSet( uint8, uint8 );
extern uint8         LL_CountAdvSets( uint8 );
extern void          LL_DisableAdvSets( void );
extern llPeriodicAdvSet_t *llGetPeriodicAdv( uint8 handle );
extern llPeriodicAdvSet_t *llGetCurrentPeriodicAdv( void );
extern void          llUpdatePeriodicAdvChainPacket( void );
extern void          llSetPeriodicAdvChmapUpdate( uint8 set );
extern llPeriodicScanSet_t *llGetPeriodicScan( uint16 handle );
extern llPeriodicScanSet_t *llGetCurrentPeriodicScan( uint8 state );
extern uint8         llGetPeriodicScanCteTasks( void );
extern void          llClearPeriodicAdvSets( void );
extern void          llClearPeriodicScanSets( void );
extern void          llSetRfCmdPreemptionParams( uint32 );
extern uint8         llGetRfCmdPreemptionEnable( void );
extern void          llTerminatePeriodicScan(void);

/*******************************************************************************
 * LL Internal API
 */
extern void 		 llUpdateSortedAdvList( void );
extern void 		 llSetAETimeConsume( sortedAdv_t *aeNode );
extern sortedAdv_t	*llDetachNode( sortedAdv_t *aeNode );
extern uint32		 llAddAdvSortedEntry( advSet_t *pAdvSet, sortedAdv_t** newNode );
extern void          llAllocRfMem( advSet_t * );
extern llStatus_t    llSetupExtAdv( advSet_t * );
extern llStatus_t    llSetupPeriodicAdv( advSet_t * );
extern llStatus_t    llSetupExtAdvLegacy( advSet_t * );
extern llStatus_t    llGetNextOrPreviousExtScanChannelIndex( uint8 );
extern llStatus_t    llGetFirstExtScanChannelIndex( void );
extern llStatus_t    llSetupExtScan( void );
extern llStatus_t    llSetupExtScan_sPatch( void );
extern void          llSetupExtInit( uint8 );
extern void          llSetupExtInit_sPatch( uint8 );
extern void          llSetupExtData( advSet_t * );
extern void          llSetupExtHdr( advSet_t *, uint8, uint16 );
extern uint8         llGetExtHdrLen( uint8 );
extern uint8         llNextChanIndex( uint16 );
extern void          llExtAdvCBack( uint8, void * );
extern uint8         llCheckCBack( uint8 );
extern void          llEndExtAdvTask( advSet_t * );
extern void          llEndExtScanTask( void );
extern void          llEndExtInitTask( void );
extern uint8         llGetRandChannelMapIndex( uint8 );
extern uint8         llSetPeriodicHdrFlags( llPeriodicAdvSet_t * );
extern void          llSetupPeriodicHdr( llPeriodicAdvSet_t * );
extern llStatus_t    llTrigPeriodicAdv( advSet_t *, llPeriodicAdvSet_t * );
extern void          llPeriodicAdv_Config( advSet_t *pAdvSet );
extern void          llEndPeriodicAdvTask( llPeriodicAdvSet_t * );
extern void          llSetPeriodicSyncInfo( advSet_t *, uint8 * );
extern llStatus_t    llSetupPeriodicScan( llPeriodicScanSet_t * );
extern llStatus_t    llTrigPeriodicScan( llPeriodicScanSet_t * );
extern void          llEndPeriodicScanTask( llPeriodicScanSet_t * );
extern void          llSetPeriodicChanMap( llPeriodicChanMap_t *, uint8 * );
extern uint32        llGetSecondaryTaskEndTime( taskInfo_t *, uint32 , llConnState_t *);
extern uint8         llCompareSecondaryPrimaryTasksQoSParam( uint8 , taskInfo_t *, llConnState_t * );
// RF Post Processing
extern void          llExtAdv_PostProcess( void );
extern void          llPeriodicAdv_PostProcess( void );
extern void          llPeriodicScan_PostProcess( void );
extern void          llExtScan_PostProcess( void );
extern void          llExtInit_PostProcess( void );
// RF Event Processing
extern void          llProcessExtScanRxFIFO( uint8 );
extern void          llSendAdvSetTermEvent( advSet_t *, uint8, uint8 );
extern void          llSendAdvSetEndEvent( advSet_t * );
extern llStatus_t    llStartDurationTimer( uint16, uint32 );
extern void          llProcessPeriodicScanRxFIFO( void );
extern uint8         llCheckDuplicateFilteringLimit( void );
// Scheduler Related
extern ble5OpCmd_t  *llFindNextAdvSet( void );
extern ble5OpCmd_t  *llFindNextSecCmd( taskInfo_t *llTask );
extern ble5OpCmd_t  *llFindNextPeriodicAdv( void );

// Other
extern dataEntryQ_t *llSetupExtScanDataEntryQueue( void );
extern dataEntryQ_t *llSetupPeriodicScanDataEntryQueue( void );

/*******************************************************************************
 */

#endif /* LL_AE_H */
