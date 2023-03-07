/******************************************************************************

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

/**
 *  @defgroup GapScan Gap Scanner
 *  @brief This module implements the Host Scanner
 *  @{
 *  @file  gap_scanner.h
 *  @brief      GAP Scanner layer interface
 */

#ifndef GAP_SCANNER_H
#define GAP_SCANNER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */
#include "bcomdef.h"
#include "osal.h"
#include "gap.h"

/*-------------------------------------------------------------------
 * MACROS
 */

/*******************************************************************************
 * Gap Scanner Internal API
 */

/*-------------------------------------------------------------------
 * CONSTANTS
 */

/**
 * @defgroup GapScan_Events GapScan Events
 *
 * Events sent from the GapScan module
 *
 * See @ref GapAdvScan_Event_IDs for the following events which are related to
 * this module:
 * - @ref GAP_EVT_SCAN_ENABLED
 * - @ref GAP_EVT_SCAN_DISABLED
 * - @ref GAP_EVT_SCAN_PRD_ENDED
 * - @ref GAP_EVT_SCAN_DUR_ENDED
 * - @ref GAP_EVT_SCAN_INT_ENDED
 * - @ref GAP_EVT_SCAN_WND_ENDED
 * - @ref GAP_EVT_ADV_REPORT
 * - @ref GAP_EVT_ADV_REPORT_FULL
 * - @ref GAP_EVT_INSUFFICIENT_MEMORY
 *
 * These are set with @ref GapScan_setParam and read with
 * @ref GapScan_getParam
 *
 * @{
 */
/** @} End GapScan_Events */

/**
 * @defgroup GapScan_Callbacks GapScan Callbacks
 *
 * Callbacks used in the GapScan module
 *
 * See @ref pfnGapCB_t for the callbacks used in this module.
 *
 * @{
 */
/** @} End GapScan_Callbacks */

/**
 * @defgroup GapScan_Constants GapScan Constants
 * Other defines used in the GapScan module
 * @{
 */

/// Advertising report event types
enum GapScan_AdvRptTypeNStatus_t {
  /// Connectable
  ADV_RPT_EVT_TYPE_CONNECTABLE = AE_EVT_TYPE_CONN_ADV,
  /// Scannable
  ADV_RPT_EVT_TYPE_SCANNABLE   = AE_EVT_TYPE_SCAN_ADV,
  /// Directed
  ADV_RPT_EVT_TYPE_DIRECTED    = AE_EVT_TYPE_DIR_ADV,
  /// Scan Response
  ADV_RPT_EVT_TYPE_SCAN_RSP    = AE_EVT_TYPE_SCAN_RSP,
  /// Legacy
  ADV_RPT_EVT_TYPE_LEGACY      = AE_EVT_TYPE_LEGACY,
  /// Complete
  ADV_RPT_EVT_STATUS_COMPLETE  = AE_EVT_TYPE_COMPLETE,
  /// More Data To Come
  ADV_RPT_EVT_STATUS_MORE_DATA = AE_EVT_TYPE_INCOMPLETE_MORE_TO_COME,
  /// Truncated
  ADV_RPT_EVT_STATUS_TRUNCATED = AE_EVT_TYPE_INCOMPLETE_NO_MORE_TO_COME,
  /// Reserved for Future Use
  ADV_RPT_EVT_STATUS_RFU       = AE_EVT_TYPE_RFU
};

/// GAP Scanner Primary PHY
enum GapScan_PrimPhy_t {
  SCAN_PRIM_PHY_1M    = LL_PHY_1_MBPS,  //!< Scan on the 1M PHY
  SCAN_PRIM_PHY_CODED = LL_PHY_CODED    //!< Scan on the Coded PHY
};

/// PDU Types for PDU Type Filter
enum GapScan_FilterPduType_t {
  /// Non-connectable only. Mutually exclusive with SCAN_FLT_PDU_CONNECTABLE_ONLY
  SCAN_FLT_PDU_NONCONNECTABLE_ONLY = ADV_RPT_EVT_TYPE_CONNECTABLE,
  /// Connectable only. Mutually exclusive with SCAN_FLT_PDU_NONCONNECTABLE_ONLY
  SCAN_FLT_PDU_CONNECTABLE_ONLY    = ADV_RPT_EVT_TYPE_CONNECTABLE << 1,
  /// Non-scannable only. Mutually exclusive with SCAN_FLT_PDU_SCANNABLE_ONLY
  SCAN_FLT_PDU_NONSCANNABLE_ONLY   = ADV_RPT_EVT_TYPE_SCANNABLE << 1,
   /// Scannable only. Mutually exclusive with SCAN_FLT_PDU_NONSCANNABLE_ONLY
  SCAN_FLT_PDU_SCANNABLE_ONLY      = ADV_RPT_EVT_TYPE_SCANNABLE << 2,
  /// Undirected only. Mutually exclusive with SCAN_FLT_PDU_DIRECTIED_ONLY
  SCAN_FLT_PDU_UNDIRECTED_ONLY     = ADV_RPT_EVT_TYPE_DIRECTED << 2,
  /// Directed only. Mutually exclusive with SCAN_FLT_PDU_UNDIRECTED_ONLY
  SCAN_FLT_PDU_DIRECTED_ONLY       = ADV_RPT_EVT_TYPE_DIRECTED << 3,
  /// Advertisement only. Mutually exclusive with SCAN_FLT_PDU_SCANRSP_ONLY
  SCAN_FLT_PDU_ADV_ONLY            = ADV_RPT_EVT_TYPE_SCAN_RSP << 3,
  /// Scan Response only. Mutually exclusive with SCAN_FLT_PDU_ADV_ONLY
  SCAN_FLT_PDU_SCANRSP_ONLY        = ADV_RPT_EVT_TYPE_SCAN_RSP << 4,
  /// Extended only. Mutually exclusive with SCAN_FLT_PDU_LEGACY_ONLY
  SCAN_FLT_PDU_EXTENDED_ONLY       = ADV_RPT_EVT_TYPE_LEGACY << 4,
  /// Legacy only. Mutually exclusive with SCAN_FLT_PDU_EXTENDED_ONLY
  SCAN_FLT_PDU_LEGACY_ONLY         = ADV_RPT_EVT_TYPE_LEGACY << 5,
  /// Truncated only. Mutually exclusive with SCAN_FLT_PDU_COMPLETE_ONLY
  SCAN_FLT_PDU_TRUNCATED_ONLY      = BV(10),
  /// Complete only. Mutually exclusive with SCAN_FLT_PDU_TRUNCATED_ONLY
  SCAN_FLT_PDU_COMPLETE_ONLY       = BV(11)
};

/// Fields of Adv Report
enum GapScan_AdvRptField_t {
  SCAN_ADVRPT_FLD_EVENTTYPE      = BV( 0), //!< eventType field
  SCAN_ADVRPT_FLD_ADDRTYPE       = BV( 1), //!< addrType field
  SCAN_ADVRPT_FLD_ADDRESS        = BV( 2), //!< address field
  SCAN_ADVRPT_FLD_PRIMPHY        = BV( 3), //!< primPhy field
  SCAN_ADVRPT_FLD_SECPHY         = BV( 4), //!< secPhy field
  SCAN_ADVRPT_FLD_ADVSID         = BV( 5), //!< advSid field
  SCAN_ADVRPT_FLD_TXPOWER        = BV( 6), //!< txPower field
  SCAN_ADVRPT_FLD_RSSI           = BV( 7), //!< RSSI field
  SCAN_ADVRPT_FLD_DIRADDRTYPE    = BV( 8), //!< dirAddrType field
  SCAN_ADVRPT_FLD_DIRADDRESS     = BV( 9), //!< dirAddress field
  SCAN_ADVRPT_FLD_PRDADVINTERVAL = BV(10), //!< prdAdvInterval field
  SCAN_ADVRPT_FLD_DATALEN        = BV(11), //!< dataLen field
};

/// Reason for @ref GapScan_Evt_End_t
enum GapScan_EndReason_t {
  SCAN_END_REASON_USR_REQ = 0, //!< Scanning ended by user request
  SCAN_END_REASON_DUR_EXP      //!< Scanning ended by duration expiration
};

/**
 * @defgroup GAPScan_RSSI GAPScan RSSI Values
 * @{
 */
#define SCAN_RSSI_MAX                      127  //!< Maximum RSSI value. 127 dBm
#define SCAN_RSSI_MIN                      -128 //!< Minimum RSSI value. -128 dBm
/** @} End GAPScan_RSSI */

/**
 * @defgroup GAPScan_Filter_Min_RSSI GAPScan Filter Minimum RSSI
 * @{
 */
#define SCAN_FLT_RSSI_ALL         SCAN_RSSI_MIN //!< -128 dBm meaning 'accept all'
#define SCAN_FLT_RSSI_NONE        SCAN_RSSI_MAX //!< 127 dBm meaning 'don't accept any'
/** @} End GAPScan_Filter_Min_RSSI */

/**
 * @defgroup GAPScan_Default_Param_Value GAPScan Default Parameter Value
 * @{
 */
/// Default scan interval (in 625 us ticks)
#define SCAN_PARAM_DFLT_INTERVAL           800
/// Default scan window   (in 625 us ticks)
#define SCAN_PARAM_DFLT_WINDOW             800
/// Default scan type
#define SCAN_PARAM_DFLT_TYPE               SCAN_TYPE_PASSIVE
/// Default adv report fields to record
#define SCAN_PARAM_DFLT_RPT_FIELDS         (SCAN_ADVRPT_FLD_EVENTTYPE|\
                                            SCAN_ADVRPT_FLD_ADDRTYPE |\
                                            SCAN_ADVRPT_FLD_ADDRESS  |\
                                            SCAN_ADVRPT_FLD_ADVSID   |\
                                            SCAN_ADVRPT_FLD_RSSI)
/// Default phys
#define SCAN_PARAM_DFLT_PHYS               SCAN_PRIM_PHY_1M
/// Default filter policy
#define SCAN_PARAM_DFLT_FLT_POLICY         SCAN_FLT_POLICY_ALL
/// Default setting for PDU type filter
#define SCAN_PARAM_DFLT_FLT_PDU            SCAN_FLT_PDU_COMPLETE_ONLY
/// Default setting for minimum RSSI filter
#define SCAN_PARAM_DFLT_FLT_RSSI           SCAN_FLT_RSSI_ALL
/// Default setting for discoverable mode filter
#define SCAN_PARAM_DFLT_FLT_DISC           SCAN_FLT_DISC_DISABLE
/// Default setting for duplicate filter
#define SCAN_PARAM_DFLT_FLT_DUP            SCAN_FLT_DUP_ENABLE
/// Default advertising report list size
#define SCAN_PARAM_DFLT_NUM_ADV_RPT        0
/** @} End GAPScan_Default_Param_Value */

/**
 * @defgroup GapScan_Periodic_advertising GapScan Periodic Advertising
 * @{
 */
/// Maximum skip value
#define SCAN_PERIODIC_SKIP_MAX                 0x01F3
/// Minimum Synchronization timeout
#define SCAN_PERIODIC_TIMEOUT_MIN              0x000A
/// Maximum Synchronization timeout
#define SCAN_PERIODIC_TIMEOUT_MAX              0x4000
/// Sync with any CTE type
#define SCAN_PERIODIC_CTE_TYPE_ALL             0
/// Do not sync to packets with AoA
#define SCAN_PERIODIC_DO_NOT_SYNC_AOA_CTE      1
/// Do not sync to packets with 1us AoD
#define SCAN_PERIODIC_DO_NOT_SYNC_AOD_CTE_1US  2
/// Do not sync to packets with 2us AoD
#define SCAN_PERIODIC_DO_NOT_SYNC_AOD_CTE_2US  4
/// Do not sync to packets without CTE
#define SCAN_PERIODIC_SYNC_ONLY_WITH_CTE       16
/// Maximum SID value
#define SCAN_PERIODIC_MAX_ADV_SID              0x0F
/// Maximum allowed in option field
#define SCAN_PERIODIC_SYNC_OPTIONS_MAX         0x03
/// Scanner's option - Do not use periodic advertisers list
#define SCAN_PERIODIC_DO_NOT_USE_PERIODIC_ADV_LIST (0 << 0)
/// Scanner's option - Use periodic advertisers list
#define SCAN_PERIODIC_USE_PERIODIC_ADV_LIST        (1 << 0)
/// Scanner's option - Periodic advertising reports initially enabled
#define SCAN_PERIODIC_REPORTING_INITIALLY_ENABLED  (0 << 1)
/// Scanner's option - Periodic advertising reports initially disabled
#define SCAN_PERIODIC_REPORTING_INITIALLY_DISABLED (1 << 1)
/** @} End GapScan_Periodic_advertising */


/** @} End GapScan_Constants */

/**
 * @defgroup GapScan_Params GapScan Params
 * Params used in the GapAdv module
 * @{
 */

/**
 * GAP Scanner Parameters
 *
 * These can be set with @ref GapScan_setParam and read with
 * @ref GapScan_getParam. The default values below refer to the values that are
 * set at initialization.
 */
typedef enum
{
  /**
   * Advertising Report Fields (R/W)
   *
   * Bit mask of which fields in the @ref GapScan_Evt_AdvRpt_t need to be stored
   * in the AdvRptList
   *
   * @note Change of this parameter shall not be attempted while scanning is
   * active
   *
   * size: uint16_t
   *
   * default: @ref SCAN_PARAM_DFLT_RPT_FIELDS
   *
   * range: combination of individual values in @ref GapScan_AdvRptField_t
   */
  SCAN_PARAM_RPT_FIELDS,

  /**
   * Primary Scanning Channel PHYs (R/W)
   *
   * PHYs on which advertisements should be received on the primary
   * advertising channel.
   *
   * @note Change of this parameter will not affect an ongoing scanning.
   * If changed during scanning, it will take effect when the scanning is
   * re-enabled after disabled.
   *
   * size: uint8_t
   *
   * default: @ref SCAN_PARAM_DFLT_PHYS
   *
   * range: @ref GapScan_PrimPhy_t
   */
  SCAN_PARAM_PRIM_PHYS,

  /**
   * Scanning Filter Policy (R/W)
   *
   * Policy of how to apply white list to filter out unwanted packets
   *
   * @note Change of this parameter will not affect an ongoing scanning.
   * If changed during scanning, it will take effect when the scanning is
   * re-enabled after disabled.
   *
   * size: uint8_t
   *
   * default: @ref SCAN_PARAM_DFLT_FLT_POLICY
   *
   * range: @ref GapScan_FilterPolicy_t
   */
  SCAN_PARAM_FLT_POLICY,

  /**
   * Filter by PDU Type (R/W)
   *
   * This filter value specifies packets of which types in 6 different category
   * sets are wanted. The 6 sets include Connectable/Non-Connectable,
   * Scannable/Non-Scannable, Directed/Undirected, ScanRsp/Adv, Legacy/Extended,
   * and Complete/Incomplete category sets. Each set has two mutually exclusive
   * types. Only one type in a set can be* chosen. For example,
   * @ref SCAN_FLT_PDU_NONSCANNABLE_ONLY and @ref SCAN_FLT_PDU_SCANNABLE_ONLY
   * cannot be chosen together. Only either one can be used. If neither type is
   * selected in a set, the filter will not care about the category. For example,
   * if neither @ref SCAN_FLT_PDU_NONCONNECTABLE_ONLY nor
   * @ref SCAN_FLT_PDU_CONNECTABLE_ONLY is set in the parameter value, the
   * scanner will accept both connectable packets and non-connectable packets.
   *
   * @par Usage example:
   * \code{.c}
   * uint16_t temp = ( SCAN_FLT_PDU_CONNECTABLE_ONLY |
   *                   SCAN_FLT_PDU_UNDIRECTED_ONLY  |
   *                   SCAN_FLT_PDU_EXTENDED_ONLY    |
   *                   SCAN_FLT_PDU_COMPLETE_ONLY);
   *
   * GapScan_setParam(SCAN_PARAM_FLT_PDU_TYPE, &temp);
   * \endcode
   *
   * With the call above, the scanner will receive only connectable,
   * undirected, extended(non-legacy), and complete packets. It will not be
   * examined whether the packets are scannable or non-scannable and whether
   * they are scan responses or advertisements.
   *
   * size: uint16_t
   *
   * default: @ref SCAN_PARAM_DFLT_FLT_PDU
   *
   * range: combination of individual values in @ref GapScan_FilterPduType_t
   */
  SCAN_PARAM_FLT_PDU_TYPE,

  /**
   * Filter by Minimum RSSI (R/W)
   *
   * Only packets received with the specified RSSI or above will be accepted.
   * @note -128 means to ignore RSSI (to not filter out any packet by RSSI)
   *
   * size: int8_t
   *
   * default: -128 (ignore)
   *
   * range: -128 - 127 (dBm)
   */
  SCAN_PARAM_FLT_MIN_RSSI,

  /**
   * Filter by Discoverable Mode (R/W)
   *
   * @note Change of this parameter shall not be attempted while scanning is
   * active
   *
   * size: uint8_t
   *
   * default: @ref SCAN_PARAM_DFLT_FLT_DISC
   *
   * range: @ref GapScan_FilterDiscMode_t
   */
  SCAN_PARAM_FLT_DISC_MODE,

  /**
   * Duplicate Filtering (R/W)
   *
   * Specify whether and when to filter out duplicated packets
   * by DID and address
   *
   * @note Change of this parameter will not affect an ongoing scanning.
   * If changed during scanning, it will take effect when the scanning is
   * re-enabled after disabled.
   *
   * size: uint8_t
   *
   * default: @ref SCAN_PARAM_DFLT_FLT_DUP
   *
   * range: @ref GapScan_FilterDuplicate_t
   */
  SCAN_PARAM_FLT_DUP,

/// @cond NODOC
  SCAN_NUM_RW_PARAM,
/// @endcond // NODOC

  /**
   * Number of Reports Recorded (R)
   *
   * Number of advertising reports recorded since the current scanning started.
   * @note This number will be the same as numReport that will come with
   * @ref GAP_EVT_SCAN_DISABLED when the scanning has ended.
   *
   * size: uint8_t
   *
   * range: 0 up to maxNumReport specified when @ref GapScan_enable() is called
   */
  SCAN_PARAM_NUM_ADV_RPT = SCAN_NUM_RW_PARAM,

/// @cond NODOC
  SCAN_NUM_PARAM
/// @endcond //NODOC
} GapScan_ParamId_t;
/** @} End GapScan_Params */

/*-------------------------------------------------------------------
 * Structures
 */

/**
 * @defgroup GapScan_Structs GapScan Structures
 * Data structures used in the GapScan module
 * @{
 */

/// Scanned PHY
typedef enum
{
  SCANNED_PHY_NONE  = 0,               //!< No PHY is used
  SCANNED_PHY_1M    = AE_PHY_1_MBPS,   //!< Scanned on the 1M PHY
  SCANNED_PHY_2M    = AE_PHY_2_MBPS,   //!< Scanned on the 2M PHY
  SCANNED_PHY_CODED = AE_PHY_CODED     //!< Scanned on the Coded PHY
} GapScan_ScannedPhy_t;

/// GAP Scanner Scan Type
typedef enum {
  SCAN_TYPE_PASSIVE = LL_SCAN_PASSIVE, //!< Scan for non-scannable advertisements
  SCAN_TYPE_ACTIVE  = LL_SCAN_ACTIVE   //!< Scan for scannable advertisements
} GapScan_ScanType_t;

/// GAP Scanner Filter Policy
typedef enum {
  /**
   * Accept all advertising packets except directed advertising packets
   * not addressed to this device.
   */
  SCAN_FLT_POLICY_ALL     = 0,
  /**
   * Accept only advertising packets from devices where the advertiser's address
   * is in the whitelist. Directed advertising packets which are not addressed
   * to this device shall be ignored.
   */
  SCAN_FLT_POLICY_WL      = 1,
  /**
   * Accept all advertising packets except directed advertising packets where
   * the TargetA does not addrress this device. Note that directed advdertising
   * packets where the TargetA is a resolvable private address that cannot be
   * resolved are also accepted.
   */
  SCAN_FLT_POLICY_ALL_RPA = 2,
  /**
   * Accept all advertising packets except advertising packets where the
   * advertiser's identity address is not in the whitelist and directed
   * advertising packets where the TargetA does not address this device.
   * Note that directed advertising packets where the TargetA is a resolvable
   * private address that cannot be resolved are also accepted.
   */
  SCAN_FLT_POLICY_WL_RPA  = 3
} GapScan_FilterPolicy_t;

/// Choices for GAP Scanner Discoverable Mode Filter
typedef enum {
  SCAN_FLT_DISC_NONE    = 0, //!< Non-discoverable mode
  SCAN_FLT_DISC_GENERAL = 1, //!< General discoverable mode
  SCAN_FLT_DISC_LIMITED = 2, //!< Limited discoverable mode
  SCAN_FLT_DISC_ALL     = 3, //!< General or Limited discoverable mode
  SCAN_FLT_DISC_DISABLE = 4  //!< Disable discoverable mode filter
} GapScan_FilterDiscMode_t;

/// Choices for GAP Scanner Duplicate Filter
typedef enum {
  SCAN_FLT_DUP_DISABLE = 0, //!< Duplicate filtering disabled
  SCAN_FLT_DUP_ENABLE  = 1, //!< Duplicate filtering enabled
  /// Duplicate filtering enabled, reset for each scan period
  SCAN_FLT_DUP_RESET   = 2
} GapScan_FilterDuplicate_t;

/// Event for scanning end
typedef struct {
  uint8_t    reason;    //!< End reason - @ref GapScan_EndReason_t
  uint8_t    numReport; //!< Number of recorded advertising reports
} GapScan_Evt_End_t;

/// Event for advertising report
typedef struct {
  /**
   * Bits 0 to 4 indicate connectable, scannable, directed, scan response, and
   * legacy respectively
   */
  uint8_t  evtType;
  /// Public, random, public ID, random ID, or anonymous
  GAP_Addr_Types_t addrType;
  /// Address of the advertising device
  uint8_t  addr[B_ADDR_LEN];
  /// PHY of the primary advertising channel
  GapScan_ScannedPhy_t primPhy;
  /// PHY of the secondary advertising channel
  GapScan_ScannedPhy_t secPhy;
  /// SID (0x00-0x0f) of the advertising PDU. 0xFF means no ADI field in the PDU
  uint8_t  advSid;
  /// -127 dBm <= TX power <= 126 dBm
  int8_t   txPower;
  /// -127 dBm <= RSSI <= 20 dBm
  int8_t   rssi;
  /// Type of TargetA address in the directed advertising PDU
  GAP_Addr_Types_t directAddrType;
  /// TargetA address
  uint8_t  directAddr[B_ADDR_LEN];
  /// Periodic advertising interval. 0 means no periodic advertising.
  uint16_t periodicAdvInt;
  /// Length of the data
  uint16_t dataLen;
  /// Pointer to advertising or scan response data
  uint8_t  *pData;
} GapScan_Evt_AdvRpt_t;

/// Event Mask
typedef uint32_t GapScan_EventMask_t;

/// Periodic advertising create sync parameters structure
typedef struct
{
  /**
  * Clear Bit 0 - Use the advSID, advAddrType, and advAddress
  *               parameters to determine which advertiser to listen to\n
  * Set Bit 0   - Use the Periodic Advertiser List to determine which
  *               advertiser to listen to\n
  * Clear Bit 1 - Reporting initially enabled\n
  * Set Bit 1   - Reporting initially disabled\n
  */
  uint8  options;
  uint8  advAddrType;     //!< Advertiser address type - 0x00 - public ; 0x01 - random
  uint8  advAddress[6];   //!< Advertiser address
  uint16 skip;            //!< The maximum number of periodic advertising events that can be skipped after a successful receive (Range: 0x0000 to 0x01F3)
  uint16 syncTimeout;     //!< Synchronization timeout for the periodic advertising train. Range: 0x000A to 0x4000 Time = N*10 ms Time Range: 100 ms to 163.84 s
  /**
  * Set Bit 0 - Do not sync to packets with an AoA CTE\n
  * Set Bit 1 - Do not sync to packets with an AoD CTE with 1 us slots\n
  * Set Bit 2 - Do not sync to packets with an AoD CTE with 2 us slots\n
  * Set Bit 4 - Do not sync to packets without a CTE\n
  */
  uint8  syncCteType;
} GapScan_PeriodicAdvCreateSyncParams_t;

/// Command status and command complete event structure
typedef struct
{
  osal_event_hdr_t hdr;  //!< OSAL Event Header
  uint8_t opcode;        //!< GAP type of command
  uint8_t status;        //!< Event status
} GapScan_PeriodicAdvEvt_t;

/// Read Advertisers list command complete structure
typedef struct
{
  osal_event_hdr_t hdr;  //!< OSAL Event Header
  uint8_t opcode;        //!< GAP type of command
  uint8_t status;        //!< Event status
  uint8_t listSize;      //!< Periodic advertisers list size
} GapScan_ReadAdvSizeListEvt_t;

/// Sync Lost event struct
typedef struct
{
  osal_event_hdr_t hdr;   //!< OSAL Event Header
  uint8_t opcode;         //!< GAP type of command
  uint16_t syncHandle;    //!< Handle identifying the periodic advertising train
} GapScan_PeriodicAdvSyncLostEvt_t;

/// Periodic advertising report event structure
typedef struct
{
  osal_event_hdr_t hdr;  //!< OSAL Event Header
  uint8  opcode;         //!< GAP type of command
  uint8  BLEEventCode;   //!< BLE Event Code
  uint16 syncHandle;     //!< Handle identifying the periodic advertising train
  int8   txPower;        //!< Tx Power information (Range: -127 to +20 dBm)
  int8   rssi;           //!< RSSI value for the received packet (Range: -127 to +20 dBm); If the packet contains CTE, this value is not available
  uint8  cteType;        //!< 0x00 AoA CTE\n 0x01 - AoD CTE with 1us slots\n 0x02 - AoD CTE with 2us slots\n 0xFF - No CTE
  uint8  dataStatus;     //!< 0x00 - Data complete\n 0x01 - Data incomplete, more data to come\n 0x02 - Data incomplete, data truncated, no more to come
  uint8  dataLen;        //!< Length of the Data field (Range: 0 to 247)
  uint8  *pData;         //!< Data received from a Periodic Advertising packet
} GapScan_Evt_PeriodicAdvRpt_t;

/// Periodic advertising sync establish event structure
typedef struct
{
  osal_event_hdr_t hdr;      //!< OSAL Event Header
  uint8  opcode;             //!< GAP type of command
  uint8  BLEEventCode;       //!< BLE Event Code
  uint8  status;             //!< Periodic advertising sync HCI status
  uint16 syncHandle;         //!< Handle identifying the periodic advertising train
  uint8  advSid;             //!< Value of the Advertising SID subfield in the ADI field of the PDU
  uint8  advAddrType;        //!< Advertiser address type:\n 0x00 - Public\n 0x01 - Random\n 0x02 - Public Identity Address\n 0x03 - Random Identity Addres
  uint8  advAddress[6];      //!< Advertiser address
  uint8  advPhy;             //!< Advertiser PHY:\n 0x01 - LE 1M\n 0x02 - LE 2M\n 0x03 - LE Coded
  uint16 periodicAdvInt;     //!< Periodic advertising interval Range: 0x0006 to 0xFFFF. Time = N * 1.25 ms (Time Range: 7.5 ms to 81.91875 s)
  uint8  advClockAccuracy;   //!< Accuracy of the periodic advertiser's clock:\n 0x00 - 500 ppm\n 0x01 - 250 ppm\n 0x02 - 150 ppm\n 0x03 - 100 ppm\n 0x04 - 75 ppm\n 0x05 - 50 ppm\n 0x06 - 30 ppm\n 0x07 - 20 ppm
} GapScan_Evt_PeriodicAdvSyncEst_t;

/** @} End GapScan_Structs */

/*-------------------------------------------------------------------
 * API's
 */

/**
 * Register the callback that will be called when a new GAP_SCAN event occurs.
 *
 * @param cb callback fiction that must be provided by the application
 * @param arg user-defined argument to be passed back through the callback. This
 *        is optional
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 */
status_t GapScan_registerCb(pfnGapCB_t cb, uintptr_t arg);

/**
 * Set parameters dependent on PHY.
 *
 * @note that if primPhys contains more than one PHY, the same parameters of
 * those PHYs will be set with the same values.
 *
 * @note Change of the parameters will not affect an ongoing scanning.
 * If changed during scanning, it will take effect when the scanning is
 * re-enabled after disabled.
 *
 * @param primPhys Primary advertising channel PHY(s). Individual values of
 *         @ref GapScan_PrimPhy_t can be OR'ed.
 * @param type Scanning type
 * @param interval Scanning interval. This shall be equal to or greater than
 *        window
 * @param window Scanning window
 *
 * @return @ref SUCCESS
 * @return @ref bleInvalidRange
 */
status_t GapScan_setPhyParams(uint8_t primPhys, GapScan_ScanType_t type,
                              uint16_t interval, uint16_t window);

/**
 * Get parameters of the specified PHY.
 *
 * @note that primPhy shall indicate only one PHY.
 *
 * @param primPhy Primary advertising channel PHY.Shall be one from
 *        @ref GapScan_PrimPhy_t.
 * @param pType pointer to where type value is to be stored
 * @param pInterval pointer to where scanning interval value is to be stored
 * @param pWindow pointer to where scanning window value is to be stored
 *
 * @return @ref SUCCESS
 * @return @ref bleInvalidRange
 * @return @ref INVALIDPARAMETER
 */
status_t GapScan_getPhyParams(uint8_t primPhy, GapScan_ScanType_t* pType,
                              uint16_t* pInterval, uint16_t* pWindow);

/**
 * Set a parameter.
 *
 * @param paramId parameter ID
 * @param pValue pointer to the value to set the parameter with
 *
 * @return @ref SUCCESS
 * @return @ref bleInvalidRange
 * @return @ref INVALIDPARAMETER
 * @return @ref bleIncorrectMode
 */
status_t GapScan_setParam(GapScan_ParamId_t paramId, void* pValue);

/**
 * Set a parameter.
 *
 * @param paramId parameter ID
 * @param pValue pointer to where the parameter value is to be stored
 * @param pLen output parameter to return the length of the parameter. This is
 *        an optional return parameter. If the caller does not care about this, it
 *        can pass in NULL.
 *
 * @return @ref SUCCESS
 * @return @ref bleInvalidRange
 * @return @ref INVALIDPARAMETER
 */
status_t GapScan_getParam(GapScan_ParamId_t paramId, void* pValue, uint8_t *pLen);

/**
 * Set which events to receive through the callback.
 *
 * One bit per event. If a bit is set to 1, the callback provided by
 * @ref GapScan_registerCb will be called upon corresponding event.
 *
 * @param eventMask bit mask of the events
 */
void GapScan_setEventMask(GapScan_EventMask_t eventMask);

/**
 * Start scanning.
 *
 * If duration is zero period shall be ignored and the scanner will continue
 * scanning until @ref GapScan_disable is called.
 * If period is zero and duration is non-zero, the scanner will scan once
 * until duration has expired or @ref GapScan_disable is called.
 * If both the duration and period are non-zero, the scanner will continue
 * scanning periodically until @ref GapScan_disable() is called.
 *
 * @param period - Scan period. Ignored if duration is zero. 1.28 sec unit.
 *        Range: 0x00-0xffff, where 0x00 is continuously scanning.
 * @param duration - Scan duration. 10 ms unit. The time of duration shall be
 *        greater than the time of scan interval set by @ref GapScan_setPhyParams.
 *        Range: 0x00-0xffff, where 0x00 is continuously scanning.
 * @param maxNumReport - If non-zero, the list of advertising reports (the number
 *        of which is up to maxNumReport) will be generated and come with
 *        @ref GAP_EVT_SCAN_DISABLED.
 *
 * @return @ref SUCCESS
 * @return @ref bleNotReady
 * @return @ref bleInvalidRange
 * @return @ref bleMemAllocError
 * @return @ref bleAlreadyInRequestedMode
 * @return @ref bleIncorrectMode
 */
status_t GapScan_enable(uint16_t period, uint16_t duration,
                        uint8_t maxNumReport);

/**
 * Stop currently running scanning operation.
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 * @return @ref bleIncorrectMode
 */
status_t GapScan_disable(void);

/**
 * Get a specific advertising report from the advertising report list.
 *
 * Only the fields specified by @ref GapScan_setParam (@ref SCAN_PARAM_RPT_FIELDS,
 * etc) will be filled out. All other fields will be filled with 0's.
 *
 * @param rptIdx Index of the advertising report in the list
 * @param pAdvRpt pointer to where the retrieved advertising report is to be
 *        stored
 *
 * @return @ref SUCCESS
 * @return @ref bleInvalidRange
 * @return @ref INVALIDPARAMETER
 */
status_t GapScan_getAdvReport(uint8_t rptIdx, GapScan_Evt_AdvRpt_t* pAdvRpt);

/**
 * Deallocate advertising report list and all its reports created during
 * scanning.
 *
 * If they are not deallocated by the application, they will be deallocated and
 * reallocated when a new scanning starts with @ref GapScan_enable
 *
 * @return @ref SUCCESS
 * @return @ref bleIncorrectMode
 */
status_t GapScan_discardAdvReportList(void);

/**
 * GapScan_PeriodicAdvCreateSync
 *
 * Used a scanner to synchronize with a periodic advertising train from
 * an advertiser and begin receiving periodic advertising packets.
 *
 * @design  /ref did_302932730
 *
 * @param   advSID - Advertising SID subfield in the ADI field used to identify the
 *                   Periodic Advertising (Range: 0x00 to 0x0F)
 * @param   createSyncParams - Pointer to the create sync parameters
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 * @return @ref bleInvalidRange
 */
uint8_t GapScan_PeriodicAdvCreateSync( uint8  advSID,
                                       GapScan_PeriodicAdvCreateSyncParams_t *createSyncParams );

/**
 * GapScan_PeriodicAdvCreateSyncCancel
 *
 * Used a scanner to cancel the HCI_LE_Periodic_Advertising_Create_Sync
 * command while it is pending.
 *
 * @design  /ref did_302932730
 *
 * @return @ref SUCCESS
 */
bStatus_t GapScan_PeriodicAdvCreateSyncCancel( void );

/**
 * GapScan_PeriodicAdvTerminateSync
 *
 * Used a scanner to stop reception of the periodic advertising
 * train identified by the syncHandle parameter.
 *
 * @design  /ref did_302932730
 *
 * @param   syncHandle - Handle identifying the periodic advertising train
 *                       (Range: 0x0000 to 0x0EFF)
 *                       The handle was assigned by the Controller while generating
 *                       the LE Periodic Advertising Sync Established event
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 * @return @ref bleInvalidRange
 */
bStatus_t GapScan_PeriodicAdvTerminateSync( uint16 syncHandle);

/**
 * GapScan_SetPeriodicAdvReceiveEnable
 *
 * Used a scanner to enable or disable reports for the periodic
 * advertising train identified by the syncHandle parameter.
 *
 * @design  /ref did_302932730
 *
 * @param   syncHandle - Handle identifying the periodic advertising train
 *                       (Range: 0x0000 to 0x0EFF)
 *                       The handle was assigned by the Controller while generating
 *                       the LE Periodic Advertising Sync Established event
 * @param   enable     - 0x00 - Reporting disable
 *                       0x01 - Reporting enable
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 * @return @ref bleInvalidRange
 */
bStatus_t GapScan_SetPeriodicAdvReceiveEnable( uint16 syncHandle,
                                               uint8  enable );

/**
 * GapScan_AddDeviceToPeriodicAdvList
 *
 * Used a scanner to add an entry, consisting of a single device address
 * and SID, to the Periodic Advertiser list stored in the Controller.
 *
 * @design  /ref did_302932730
 *
 * @param   advAddrType - Advertiser address type - 0x00 - Public or Public Identity Address
 *                                                  0x01 - Random or Random (static) Identity Address
 * @param   advAddress  - Advertiser address
 * @param   advSID      - Advertising SID subfield in the ADI field used to identify
 *                        the Periodic Advertising (Range: 0x00 to 0x0F)
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 * @return @ref bleInvalidRange
 */
bStatus_t GapScan_AddDeviceToPeriodicAdvList( uint8 advAddrType,
                                              uint8 advAddress[6],
                                              uint8 advSID );

/**
 * GapScan_RemoveDeviceFromPeriodicAdvertiserList
 *
 * Used a scanner to remove one entry from the list of Periodic Advertisers
 * stored in the Controller.
 *
 * @design  /ref did_302932730
 *
 * @param   advAddrType - Advertiser address type -
 *                        0x00 - Public or Public Identity Address
 *                        0x01 - Random or Random (static) Identity Address
 * @param   advAddress  - Advertiser address
 * @param   advSID      - Advertising SID subfield in the ADI field used to identify
 *                        the Periodic Advertising (Range: 0x00 to 0x0F)
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 * @return @ref bleInvalidRange
 */
 bStatus_t GapScan_RemoveDeviceFromPeriodicAdvList( uint8 advAddrType,
                                                    uint8 advAddress[6],
                                                    uint8 advSID );

/**
 * GapScan_ReadPeriodicAdvListSize
 *
 * Used a scanner to read the total number of Periodic Advertiser
 * list entries that can be stored in the Controller.
 *
 * @design  /ref did_302932730
 *
 * @return @ref SUCCESS
 */
hciStatus_t GapScan_ReadPeriodicAdvListSize( void );

/**
 * GapScan_ClearPeriodicAdvList
 *
 * Used a scanner to remove all entries from the list of Periodic
 * Advertisers in the Controller.
 *
 * @design  /ref did_302932730
 *
 * @return @ref SUCCESS
 */
bStatus_t GapScan_ClearPeriodicAdvList( void );

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* GAP_SCANNER_H */

/** @} End GapScan */

