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
 *  @defgroup GapAdv Gap Advertiser
 *  @brief This module implements the Host Advertiser
 *
 *  @{
 *  @file  gap_advertiser.h
 *  @brief      GAP Advertiser layer interface
 */

#ifndef GAP_ADVERTISER_H
#define GAP_ADVERTISER_H

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

// This is a common file for the legacy and sysconfig examples,
// the parameters under ifndef SYSCFG are defined in this file for
// the legacy examples and generated using the sysconfig tool for
// the sysconfig examples
#ifndef SYSCFG
/**
 * @defgroup GapAdv_Constants GapAdv Constants
 * Other defines used in the GapAdv module
 * @{
 */

/// Default parameters for legacy, scannable, connectable advertising
#define GAPADV_PARAMS_LEGACY_SCANN_CONN {                                  \
  .eventProps = GAP_ADV_PROP_CONNECTABLE | GAP_ADV_PROP_SCANNABLE |        \
                GAP_ADV_PROP_LEGACY,                                       \
  .primIntMin = 160,                                                       \
  .primIntMax = 160,                                                       \
  .primChanMap = GAP_ADV_CHAN_ALL,                                         \
  .peerAddrType = PEER_ADDRTYPE_PUBLIC_OR_PUBLIC_ID,                       \
  .peerAddr = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa },                      \
  .filterPolicy = GAP_ADV_WL_POLICY_ANY_REQ,                               \
  .txPower = GAP_ADV_TX_POWER_NO_PREFERENCE,                               \
  .primPhy = GAP_ADV_PRIM_PHY_1_MBPS,                                      \
  .secPhy = GAP_ADV_SEC_PHY_1_MBPS,                                        \
  .sid = 0                                                                 \
}

/// Default parameters for long range, connectable, advertising extension
#define GAPADV_PARAMS_AE_LONG_RANGE_CONN {                                 \
  .eventProps = GAP_ADV_PROP_CONNECTABLE,                                  \
  .primIntMin = 160,                                                       \
  .primIntMax = 160,                                                       \
  .primChanMap = GAP_ADV_CHAN_ALL,                                         \
  .peerAddrType = PEER_ADDRTYPE_PUBLIC_OR_PUBLIC_ID,                       \
  .peerAddr = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa },                      \
  .filterPolicy = GAP_ADV_WL_POLICY_ANY_REQ,                               \
  .txPower = GAP_ADV_TX_POWER_NO_PREFERENCE,                               \
  .primPhy = GAP_ADV_PRIM_PHY_CODED_S2,                                    \
  .secPhy = GAP_ADV_SEC_PHY_CODED_S2,                                      \
  .sid = 0                                                                 \
}

/// Non-Connectable & Non-Scannable advertising set
#define GAPADV_PARAMS_AE_NC_NS {                                           \
  .eventProps = 0,                                                         \
  .primIntMin = 160,                                                       \
  .primIntMax = 160,                                                       \
  .primChanMap = GAP_ADV_CHAN_ALL,                                         \
  .peerAddrType = PEER_ADDRTYPE_PUBLIC_OR_PUBLIC_ID,                       \
  .peerAddr = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa },                      \
  .filterPolicy = GAP_ADV_WL_POLICY_ANY_REQ,                               \
  .txPower = GAP_ADV_TX_POWER_NO_PREFERENCE,                               \
  .primPhy = GAP_ADV_PRIM_PHY_1_MBPS,                                      \
  .secPhy = GAP_ADV_SEC_PHY_1_MBPS,                                        \
  .sid = 1                                                                 \
}
/** @} End GapAdv_Constants */
#endif

/*-------------------------------------------------------------------
 * CONSTANTS
 */

/**
 * @defgroup GapAdv_Events GapAdv Events
 *
 * Events sent from the GapAdv module
 *
 * See @ref GapAdvScan_Event_IDs for the following events which are related to
 * this module:
 * - @ref GAP_EVT_ADV_START_AFTER_ENABLE
 * - @ref GAP_EVT_ADV_END_AFTER_DISABLE
 * - @ref GAP_EVT_ADV_START
 * - @ref GAP_EVT_ADV_END
 * - @ref GAP_EVT_ADV_SET_TERMINATED
 * - @ref GAP_EVT_SCAN_REQ_RECEIVED
 * - @ref GAP_EVT_ADV_DATA_TRUNCATED
 * - @ref GAP_EVT_INSUFFICIENT_MEMORY
 *
 * These are set with @ref GapAdv_setParam and read with
 * @ref GapAdv_getParam
 *
 * @{
 */
/** @} End GapAdv_Events */

/**
 * @defgroup GapAdv_Callbacks GapAdv Callbacks
 *
 * Callbacks used in the GapAdv module
 *
 * See @ref pfnGapCB_t for the callbacks used in this module.
 *
 * These are registered per-advertising handle with @ref GapAdv_create
 *
 * @{
 */
/** @} End GapAdv_Callbacks */

/**
 * @defgroup GapAdv_Params GapAdv Params
 * Params used in the GapAdv module
 * @{
 */

/**
 * GAP Advertising Parameters
 *
 * These can be set with @ref GapAdv_setParam and read with
 * @ref GapAdv_getParam. They do not have default values since it is required to
 * pass a set of parameters into @ref GapAdv_create
 *
 * Each advertising handle can have a different set of parameters.
 *
 * All parameters are readable and writable
 */
typedef enum
{
  /**
   * Advertising Event Properties
   *
   * size: uint16_t
   *
   * range: @ref GapAdv_eventProps_t
   */
  GAP_ADV_PARAM_PROPS,

  /**
   * Minimum advertising interval (n * 0.625 ms)
   *
   * This is for undirected and low duty cycle directed advertising
   *
   * size: uint32_t
   *
   * range: 20 ms to 10,485.759375 s
   */
  GAP_ADV_PARAM_PRIMARY_INTERVAL_MIN,

  /**
   * Maximum advertising interval (n * 0.625 ms
   *
   * This is for undirected and low duty cycle directed advertising
   *
   * @note This will be ignored as the @ref GAP_ADV_PARAM_PRIMARY_INTERVAL_MIN
   * is always used
   *
   * size: uint32_t
   *
   * range: 20 ms to 10,485.759375 s
   */
  GAP_ADV_PARAM_PRIMARY_INTERVAL_MAX,

  /**
   * Primary Channel Map
   *
   * A bit field that indicates the advertising channels that shall be used when
   * transmitting advertising packets
   *
   * size: uint8_t
   *
   * range: @ref GapAdv_advChannels_t
   */
  GAP_ADV_PARAM_PRIMARY_CHANNEL_MAP,

  /**
   * Peer Address Type for Directed Advertising
   *
   * size: uint8_t
   *
   * range: @ref GAP_Peer_Addr_Types_t
   */
  GAP_ADV_PARAM_PEER_ADDRESS_TYPE,

  /**
   * Peer Address for Directed Advertising
   *
   * size: uint8_t[6]
   *
   * range: 0x000000000000 - 0xFFFFFFFFFFFE
   */
  GAP_ADV_PARAM_PEER_ADDRESS,

  /**
   * Advertising Filter Policy
   *
   * size: uint8_t
   *
   * range: @ref GapAdv_filterPolicy_t
   */
  GAP_ADV_PARAM_FILTER_POLICY,

  /**
   * Advertising Tx Power
   *
   * This is the maximum power level at which the advertising packets are to be
   * transmitted on the advertising channels. The Controller will choose a power
   * level lower than or equal to this.
   *
   * @note When setting this parameter, the host will overwrite the value with
   * the new chosen value if it is set successfully.
   *
   * @warning In order to set this parameter, @ref GAP_ADV_PARAM_PROPS must
   * be set to include @ref GAP_ADV_PROP_TX_POWER
   *
   * size: int8_t
   *
   * units: dBm
   * 
   * range: -127 to +20 or @ref GAP_ADV_TX_POWER_NO_PREFERENCE
   */
  GAP_ADV_PARAM_TX_POWER,

  /**
   * Primary PHY
   *
   * The PHY on which the advertising packets are transmitted on the primary
   * advertising channel.
   *
   * size: uint8_t
   *
   * range: @ref GapAdv_primaryPHY_t
   */
  GAP_ADV_PARAM_PRIMARY_PHY,

  /**
   * Secondary PHY
   *
   * The PHY on which the advertising packets are be transmitted on the
   * secondary advertising channel.
   *
   * size: uint8_t
   *
   * range: @ref GapAdv_secondaryPHY_t
   */
  GAP_ADV_PARAM_SECONDARY_PHY,

  /**
   * Advertising SID (set identity)
   *
   * This is the value to be transmitted in the Advertising SID subfield of the
   * ADI field of the Extended Header of those advertising channel PDUs that
   * have an ADI field. If the advertising set only uses PDUs that do not
   * contain an ADI field, this parameter is ignored.
   *
   * size: uint8_t
   *
   * range: 0-15
   */
  GAP_ADV_PARAM_SID,

/// @cond NODOC
  GAP_ADV_PARAM_COUNT
/// @endcond //NODOC
} GapAdv_ParamId_t;
/** @} End GapAdv_Params */

/**
 *  @addtogroup GapAdv_Constants
 *  @{
 */

/// Invalid Advertising Handle for GAP Advertising API's
#define GAP_ADV_INVALID_HANDLE        0xFF

/// GAP Advertising Event Param Properties
enum GapAdv_eventProps_t
{
  GAP_ADV_PROP_CONNECTABLE =  BV(0),  //!< Connectable advertising
  GAP_ADV_PROP_SCANNABLE =    BV(1),  //!< Scannable advertising
  GAP_ADV_PROP_DIRECTED =     BV(2),  //!< Directed advertising
  GAP_ADV_PROP_HDC =          BV(3),  //!< High Duty Cycle Directed Advertising
  GAP_ADV_PROP_LEGACY =       BV(4),  //!< Use legacy advertising PDU's
  GAP_ADV_PROP_ANONYMOUS =    BV(5),  //!< Omit advertiser's address from PDU's
  GAP_ADV_PROP_TX_POWER =     BV(6)   //!< Include TxPower in the extended header
                                      //!< of the advertising PDU
};

/// Gap Advertising Data Types
typedef enum
{
  GAP_ADV_DATA_TYPE_ADV,        //!< Advertising data
  GAP_ADV_DATA_TYPE_SCAN_RSP    //!< Scan response data
} GapAdv_dataTypes_t;

/// Possible free options for Gap Advertiser API's
typedef enum
{
  GAP_ADV_FREE_OPTION_DONT_FREE,       //!< Don't free anything
  GAP_ADV_FREE_OPTION_ADV_DATA,        //!< Free the advertising data
  GAP_ADV_FREE_OPTION_SCAN_RESP_DATA,  //!< Free the scan response data
} GapAdv_freeBufferOptions_t;

/// Enable options for @ref GapAdv_enable
typedef enum
{
  /**
   * Use the maximum possible value. This is the spec-defined maximum for
   * directed advertising and infinite advertising for all other types
   */
  GAP_ADV_ENABLE_OPTIONS_USE_MAX,
  /**
   * Use the user-specified duration
   */
  GAP_ADV_ENABLE_OPTIONS_USE_DURATION,
  /**
   * Use the user-specified maximum number of events
   */
  GAP_ADV_ENABLE_OPTIONS_USE_MAX_EVENTS,
} GapAdv_enableOptions_t;

/// GAP Advertiser Defines for Advertiser Channels
typedef enum
{
  GAP_ADV_CHAN_37       = BV(0),  //!< Channel 37
  GAP_ADV_CHAN_38       = BV(1),  //!< Channel 38
  GAP_ADV_CHAN_39       = BV(2),  //!< Channel 39
  GAP_ADV_CHAN_37_38    = (GAP_ADV_CHAN_37 |
                           GAP_ADV_CHAN_38),  //!< Channels 37 & 38
  GAP_ADV_CHAN_37_39    = (GAP_ADV_CHAN_37 |
                           GAP_ADV_CHAN_39),  //!< Channels 37 & 39
  GAP_ADV_CHAN_38_39    = (GAP_ADV_CHAN_38 |
                           GAP_ADV_CHAN_39),  //!< Channels 38 & 39
  GAP_ADV_CHAN_ALL      = (GAP_ADV_CHAN_37 |
                           GAP_ADV_CHAN_38 |
                           GAP_ADV_CHAN_39)  //!< All Channels
} GapAdv_advChannels_t;

/// GAP Advertiser White List Policy
typedef enum
{
  /**
   * Process scan and connection requests from all devices (i.e., the White List
   * is not in use)
   */
  GAP_ADV_WL_POLICY_ANY_REQ,
  /**
   * Process connection requests from all devices and only scan requests
   * from devices that are in the White List
   */
  GAP_ADV_WL_POLICY_WL_SCAN_REQ,
  /**
   * Process scan requests from all devices and only connection requests
   * from devices that are in the White List.
   */
  GAP_ADV_WL_POLICY_WL_CONNECT_IND,
  /**
   * Process scan and connection requests only from devices in the White List.
   */
  GAP_ADV_WL_POLICY_WL_ALL_REQ
} GapAdv_filterPolicy_t;

/// Allow the controller to choose the Tx power
#define GAP_ADV_TX_POWER_NO_PREFERENCE      127

/// Gap Advertiser Options for advertising on the primary PHY
typedef enum
{
  GAP_ADV_PRIM_PHY_1_MBPS     = 0x01,  //!< 1 mbps
  GAP_ADV_PRIM_PHY_CODED_S8   = 0x03,  //!< coded-s8
  GAP_ADV_PRIM_PHY_CODED_S2   = 0x83   //!< coded-s2
} GapAdv_primaryPHY_t;

/// Gap Advertiser Options for advertising on the secondary PHY
typedef enum
{
  GAP_ADV_SEC_PHY_1_MBPS     = 0x01,  //!< 1 mbps
  GAP_ADV_SEC_PHY_2_MBPS     = 0x02,  //!< 2 mbps
  GAP_ADV_SEC_PHY_CODED_S8   = 0x03,  //!< coded-s8
  GAP_ADV_SEC_PHY_CODED_S2   = 0x83   //!< coded-s2
} GapAdv_secondaryPHY_t;

/**
 * GAP Advertiser bitfields to enable / disable callback events
 *
 * These are used in @ref GapAdv_setEventMask
 * The events that that these flags control are defined in
 * @ref GapAdvScan_Event_IDs
 */
typedef enum
{
  /**
   * Enables / disables the @ref GAP_EVT_SCAN_REQ_RECEIVED event
   */
  GAP_ADV_EVT_MASK_SCAN_REQ_NOTI       = AE_NOTIFY_ENABLE_SCAN_REQUEST,
  /**
   * Enables / disables the @ref GAP_EVT_ADV_SET_TERMINATED event
   */
  GAP_ADV_EVT_MASK_SET_TERMINATED      = BV(1),
  /**
   * Enables / disables the @ref GAP_EVT_ADV_START_AFTER_ENABLE event
   */
  GAP_ADV_EVT_MASK_START_AFTER_ENABLE  = AE_NOTIFY_ENABLE_ADV_SET_START,
  /**
   * Enables / disables the @ref GAP_EVT_ADV_START event
   */
  GAP_ADV_EVT_MASK_START               = AE_NOTIFY_ENABLE_ADV_START,
  /**
   * Enables / disables the @ref GAP_EVT_ADV_END event
   */
  GAP_ADV_EVT_MASK_END                 = AE_NOTIFY_ENABLE_ADV_END,
  /**
   * Enables / disables the @ref GAP_EVT_ADV_END_AFTER_DISABLE event
   */
  GAP_ADV_EVT_MASK_END_AFTER_DISABLE   = AE_NOTIFY_ENABLE_ADV_SET_END,
  /**
   * Mask to enables / disable all advertising events
   */
  GAP_ADV_EVT_MASK_ALL                 = GAP_ADV_EVT_MASK_SCAN_REQ_NOTI |
                                         GAP_ADV_EVT_MASK_START_AFTER_ENABLE |
                                         GAP_ADV_EVT_MASK_START |
                                         GAP_ADV_EVT_MASK_END |
                                         GAP_ADV_EVT_MASK_END_AFTER_DISABLE |
                                         GAP_ADV_EVT_MASK_SET_TERMINATED,
/// @cond NODOC
  /**
   * Used to set this to 16 bits for future events
   */
  GAP_ADV_EVT_MASK_RESERVED            = BV(15)
/// @endcond // NODOC
} GapAdv_eventMaskFlags_t;

/**
 * @defgroup GapAdv_Periodic_Advertising GapAdv Periodic Advertising Values
 * @{
 */
/// Periodic Advertising - Minimum interval
#define GAPADV_PERIODIC_ADV_INTERVAL_MIN                 0x0006
/// Periodic Advertising - Maximum interval
#define GAPADV_PERIODIC_ADV_INTERVAL_MAX                 0xFFFF
/// Periodic Advertising - Enable TX power property
#define GAPADV_PERIODIC_ADV_ENABLE_TX_POWER              0x0040
/// Periodic Advertising - Disable TX power property
#define GAPADV_PERIODIC_ADV_DISABLE_TX_POWER             0x0000
/// Periodic Advertising operation options - Intermidiate fragment
#define GAPADV_PERIODIC_ADV_DATA_INTERMIDIATE_FRAG       0x00
/// Periodic Advertising operation options - First fragment
#define GAPADV_PERIODIC_ADV_DATA_FIRST_FRAG              0x01
/// Periodic Advertising operation options - Last fragment
#define GAPADV_PERIODIC_ADV_DATA_LAST_FRAG               0x02
/// Periodic Advertising operation options - Complete data
#define GAPADV_PERIODIC_ADV_DATA_COMPLETE                0x03
/// Periodic Advertising - Maximum fragment/complete data length
#define GAPADV_PERIODIC_ADV_MAX_DATA_LEN                 252
/** @} End GapAdv_Periodic_Advertising */

/** @} End GapAdv_Constants */

/*-------------------------------------------------------------------
 * Structures
 */

/**
 * @defgroup GapAdv_Structs GapAdv Structures
 * Data structures used in the GapAdv module
 * @{
 */

/**
 * Per-advertising set parameters
 *
 * These can be set with @ref GapAdv_setParam
 * See the specific parameter linked to each element for more information.
 */
typedef struct
{
  uint16_t eventProps;                   //!< @ref GAP_ADV_PARAM_PROPS
  uint32_t primIntMin;                   //!< @ref GAP_ADV_PARAM_PRIMARY_INTERVAL_MIN
  uint32_t primIntMax;                   //!< @ref GAP_ADV_PARAM_PRIMARY_INTERVAL_MAX
  GapAdv_advChannels_t primChanMap;      //!< @ref GAP_ADV_PARAM_PRIMARY_CHANNEL_MAP
  GAP_Peer_Addr_Types_t peerAddrType;    //!< @ref GAP_ADV_PARAM_PEER_ADDRESS_TYPE
  uint8_t peerAddr[B_ADDR_LEN];          //!< @ref GAP_ADV_PARAM_PEER_ADDRESS
  GapAdv_filterPolicy_t filterPolicy;    //!< @ref GAP_ADV_PARAM_FILTER_POLICY
  int8_t txPower;                        //!< NOT CURRENTLY IMPLEMENTED
  GapAdv_primaryPHY_t primPhy;           //!< @ref GAP_ADV_PARAM_PRIMARY_PHY
  GapAdv_secondaryPHY_t secPhy;          //!< @ref GAP_ADV_PARAM_SECONDARY_PHY
  uint8_t sid;                           //!< @ref GAP_ADV_PARAM_SID
} GapAdv_params_t;


/// Data type returned with @ref GAP_EVT_ADV_SET_TERMINATED event
typedef struct
{
  uint8  handle;         //!< Advertising handle
  uint8  status;         //!< TODO
  uint16 connHandle;     //!< Connection handle that ended advertising
  uint8  numCompAdvEvts; //!< Number of completed advertising events
} GapAdv_setTerm_t;

/// Data type returned with @ref GAP_EVT_SCAN_REQ_RECEIVED event
typedef struct
{
  uint8  handle;                 //!< Advertising handle
  uint8  scanAddrType;           //!< Address type of scanning device
  uint8  scanAddr[B_ADDR_LEN];   //!< Address of scanning device
  uint8  channel;                //!< Channel when the scan req was received
  int8   rssi;                   //!< RSSI of the scan req
} GapAdv_scanReqReceived_t;

/// Data type returned with @ref GAP_EVT_ADV_DATA_TRUNCATED event
typedef struct
{
  uint8  handle;           //!< Advertising handle
  uint8  availAdvDataLen;  //!< How many bytes of the advertising data were sent
  /**
   * Total length of the advertising data that was attempted to send
   */
  uint16 advDataLen;
} GapAdv_truncData_t;

/// Periodic advertising parameters structure
typedef struct
{
  uint16 periodicAdvIntervalMin;   //!< Minimum periodic advertising interval; Range: 0x0006 to 0xFFFF Time = N * 1.25 ms Time Range: 7.5ms to 81.91875s
  uint16 periodicAdvIntervalMax;   //!< Maximum periodic advertising interval; Range: 0x0006 to 0xFFFF Time = N * 1.25 ms Time Range: 7.5ms to 81.91875s
  uint16 periodicAdvProp;          //!< Periodic advertising properties - set bit 6 for include TxPower in the advertising PDU
} GapAdv_periodicAdvParams_t;

/// Periodic advertising data structure
typedef struct
{
  /**
   * operation  - 0x00 - Intermediate fragment of fragmented periodic advertising data;
   *			  0x01 - First fragment of fragmented periodic advertising data;
   *			  0x02 - Last fragment of fragmented periodic advertising data;
   *			  0x03 - Complete periodic advertising data
   */
  uint8 operation;
  uint8 dataLength;   //!< The number of bytes in the Advertising Data parameter
  uint8 *pData;       //!< Periodic advertising data
} GapAdv_periodicAdvData_t;

typedef struct
{
  osal_event_hdr_t hdr;   //!< OSAL Event Header
  uint8_t opcode;         //!< GAP type of command
  uint8_t status;         //!< Event status
} GapAdv_periodicAdvEvt_t;

/// @cond NODOC
typedef struct
{
  osal_event_hdr_t hdr;
  uint8_t opcode;
  uint8_t dataType;
} GapAdv_dataUpdateEvent_t;

typedef struct
{
  osal_event_hdr_t hdr;
  uint8_t opcode;
} GapAv_setAdvStatusDoneEvent_t;
/// @endcond //NODOC

/** @} End GapAdv_Structs */

/*-------------------------------------------------------------------
 * FUNCTIONS - Advertising
 */

/**
 * Create an advertising set.
 *
 * This will allocate all required memory for the advertising set except for the
 * advertising data. A pointer to a structure of advertising parameters
 * (@ref GapAdv_params_t) must be passed with advParam. This will be copied to
 * the newly allocated memory for the advertising set and can then be freed if
 * desired. Parameters can be changed individually later with
 * @ref GapAdv_setParam
 *
 * @warning It is required to pass a callback into this function in order to
 * receive advertising events. The various events that are passed into this
 * callback can be disabled / enabled with  @ref GapAdv_setEventMask
 *
 * @param cb Function pointer to a callback for this advertising set
 * @param advParam pointer to structure of adversing parameters
 * @param advHandle Output parameter to return the created advertising handle
 *
 * @return @ref bleIncorrectMode : incorrect profile role or an update / prepare
 *         is in process
 * @return @ref bleInvalidRange : the callback function was NULL
 * @return @ref bleNoResources : there are no available advertising handles
 * @return @ref bleMemAllocError : there is not enough memory to allocate the
 *         advertising set
 * @return @ref SUCCESS
 */
extern bStatus_t GapAdv_create(pfnGapCB_t *cb, GapAdv_params_t *advParam,
                               uint8 *advHandle);

/**
 *  Delete an advertising set.
 *
 * This will first disable advertising then free all of the memory that was
 * allocated for the advertising set from @ref GapAdv_create. It will also
 * attempt to free the advertising / scan response data based on freeOptions.
 *
 * @note The advertising / scan response data can only be freed if the
 * respective buffer is not used elsewhere by another advertising set.
 *
 * @param handle Handle of advertising set to destroy
 * @param freeOptions @ref GapAdv_freeBufferOptions_t
 *
 * @return @ref bleIncorrectMode : incorrect profile role or an update / prepare
 *         is in process
 * @return @ref bleGAPNotFound : advertising set does not exist
 * @return @ref bleGAPBufferInUse : can't free since this buffer is used
 *         elsewhere
 * @return @ref SUCCESS
 */
extern bStatus_t GapAdv_destroy(uint8 handle,
                                GapAdv_freeBufferOptions_t freeOptions);

/**
 * Enable Advertising.
 *
 * This will attempt to enable advertising for a set identified by the handle.
 * The advertising set must first be created with @ref GapAdv_create.
 *
 * @par Corresponding Events:
 * @ref GAP_EVT_ADV_START_AFTER_ENABLE after advertising is enabled <br>
 * @ref GAP_EVT_ADV_START after each advertising set starts<br>
 * @ref GAP_EVT_ADV_END after each advertising set ends<br>
 * @ref GAP_EVT_SCAN_REQ_RECEIVED after received scan requests<br>
 *
 * @param handle Handle of advertising set to enable
 * @param enableOptions whether to advertise for the max possible time, for
 *        a user-specified duration, or for a user-specified number of
 *        advertising events
 * @param durationOrMaxEvents If enableOptions is set to
 *        @ref GAP_ADV_ENABLE_OPTIONS_USE_DURATION, this is the time
 *        (in 10 ms ticks) to advertise before stopping where the range is 10
 *        ms - 655,540 ms <br>
 *        If enableOptions is set to @ref GAP_ADV_ENABLE_OPTIONS_USE_MAX_EVENTS,
 *        this is the maximum number of advertisements to send before stopping,
 *        where the range is 1-256 <br>
 *        If enableOptions is set to @ref GAP_ADV_ENABLE_OPTIONS_USE_MAX, this
 *        parameter is not used
 *
 * @return @ref bleIncorrectMode : incorrect profile role or an update / prepare
 *         is in process
 * @return @ref bleGAPNotFound : advertising set does not exist
 * @return @ref bleNotReady : the advertising set has not yet been loaded with
 *         advertising data
 * @return @ref bleAlreadyInRequestedMode : device is already advertising
 * @return @ref SUCCESS
 */
extern bStatus_t GapAdv_enable(uint8 handle,
                               GapAdv_enableOptions_t enableOptions,
                               uint16 durationOrMaxEvents);

/**
* @brief Disable an active advertising set.
*
* The set still exists and can be re-enabled with @ref GapAdv_enable .
*
* @par Corresponding Events:
* @@ref GAP_EVT_ADV_END_AFTER_DISABLE after advertising is disabled <br>
*
* @param handle Handle of advertising set to disable
*
* @return @ref bleIncorrectMode : incorrect profile role or an update / prepare
*         is in process
* @return @ref bleGAPNotFound : advertising set does not exist
* @return @ref bleAlreadyInRequestedMode : advertising set is not currently
*         advertising
* @return @ref SUCCESS
*/
extern bStatus_t GapAdv_disable(uint8 handle);

/**
 * Set the per-advertising set event mask to select which events are sent
 *
 * A value of 1 enables the event and a value of 0 disables the event
 *
 * @param  handle handle of advertising set to mask events for
 * @param  mask   A bitfield to enable / disable events returned to the
 *                per-advertising set callback function (@ref pfnGapCB_t ). See
 *                @ref GapAdv_eventMaskFlags_t
 *
 * @return @ref bleIncorrectMode : incorrect profile role
 * @return @ref bleInvalidRange : invalid event mask
 * @return @ref bleGAPNotFound : advertising set does not exist
 * @return @ref bleNotReady : advertising needs to be disabled
 * @return @ref SUCCESS
 */
extern bStatus_t GapAdv_setEventMask(uint8 handle, GapAdv_eventMaskFlags_t mask);

/**
 * Set a parameter for an advertising set.
 *
 * The input pointer (pValue) should point to a type based on
 * the paramID. See the respective parameter in @ref GapAdv_ParamId_t for the
 * type to pass in as pValue
 *
 * @note Advertising must be disabled in order to update a parameter.
 *
 * @param handle Handle of advertising set to modify
 * @param paramID parameter to update. See @ref GapAdv_ParamId_t
 * @param pValue input pointer
 *
 * @return @ref bleIncorrectMode : incorrect profile role
 * @return @ref bleInvalidRange : paramID out of range
 * @return @ref bleGAPNotFound : advertising set does not exist
 * @return @ref SUCCESS
 */
extern bStatus_t GapAdv_setParam(uint8 handle, GapAdv_ParamId_t paramID,
                                 void *pValue);

/**
 * Get a parameter for an advertising set.
 *
 * @note The data that the return pointer ( pValue) points to should be
 * cast to a type based on the paramID . See the respective parameter in
 * @ref GapAdv_ParamId_t for the type to cast pValue to
 *
 * @param handle Handle of advertising set to modify
 * @param paramID parameter to read. See @ref GapAdv_ParamId_t
 * @param pValue return pointer
 * @param pLen output parameter to return the length of the parameter. This is
 * an optional return parameter. If the caller does not care about this, it can
 * pass in NULL.
 *
 * @return @ref bleIncorrectMode : incorrect profile role
 * @return @ref bleInvalidRange : paramID out of range
 * @return @ref bleGAPNotFound : advertising set does not exist
 * @return @ref SUCCESS
 */
extern bStatus_t GapAdv_getParam(uint8 handle, GapAdv_ParamId_t paramID,
                                 void *pValue, uint8_t *pLen);

/**
 * Find the address of the advertising or scan response data that an
 * advertising set is using.
 *
 * @warning The ppBuf parameter is a pointer-to-a-pointer so the address to a
 * pointer must be passed in.
 *
 * @param handle Handle of advertising set to search.
 * @param dataType @ref GapAdv_dataTypes_t
 * @param pLen output parameter to return length of data
 * @param ppBuf output parameter to return pointer to data
 *
 * @return @ref bleIncorrectMode : incorrect profile role
 * @return @ref bleInvalidRange : dataType out of range
 * @return @ref bleGAPNotFound : advertising set does not exist
 * @return @ref SUCCESS
 */
extern bStatus_t GapAdv_getBuffer(uint8 handle, GapAdv_dataTypes_t dataType,
                                  uint16 *pLen, uint8 **ppBuf);

/**
 * Prepares advertising for a given handle and optionally frees the old data
 * that was being used for advertisement / scan response data.
 *
 * This function must be used in the following cases:
 * -# It is desired to update the contents of a data buffer that is currently
 * being used for an enabled advertising handle
 * -# It is desired to load a new buffer to an advertising handle
 *
 * This function is used in conjunction with @ref GapAdv_loadByHandle to support
 * either of the cases stated above.  This function is called first before
 * @ref GapAdv_loadByHandle and will disable advertising on the handle if it is
 * currently enabled. This will result in the @ref GAP_EVT_ADV_END_AFTER_DISABLE
 * being sent
 *
 * For case 1, the buffer can be updated as desired after this function returns
 * successfully, then reloaded with @ref GapAdv_loadByHandle
 *
 * For case 2, in order to prevent double-copying, the data that was being used
 * by the advertising set should be freed before calling
 * @ref GapAdv_loadByHandle. It is recommended that this is done by setting
 * the freeOptions of @ref GapAdv_prepareLoadByHandle . This will
 * prompt the host to search through other advertising sets to ensure that the
 * data buffer is not being used elsewhere.  If the data is being used elsewhere,
 * the free will not occur and @ref bleGAPBufferInUse will be returned. In this
 * case, there are now two options to update this advertising data.
 * -# If it is desired to only update this handle, the only way to do this is
 *  to: destroy this handle with @ref GapAdv_destroy, recreate this handle with
 *  @ref GapAdv_create, and load it using @ref GapAdv_loadByHandle
 * -# Update the buffer (which will affect all handles using this buffer) using
 *  @ref GapAdv_prepareLoadByBuffer and @ref GapAdv_loadByBuffer
 *
 * Both of these options require first aborting this prepare operation with
 * @ref GapAdv_abortLoad
 *
 * @note It is not possible to free the advertising data and the scan response
 * at the same time. A prepare / load operation must run to
 * completion before performing another prepare operation, an advertising
 * disable, enable, create or destroy. It is possible to abort the
 * prepare / load with @ref GapAdv_abortLoad .
 *
 * @par Corresponding events
 * @ref GAP_EVT_ADV_END_AFTER_DISABLE
 *
 * @param handle Handle of advertising set to prepare
 * @param freeOptions Whether to free the original buffer and, if so, whether
 *        it is advertising data or scan response data
 *
 * @return @ref bleIncorrectMode : incorrect profile role or there is already
 *         an ongoing prepare or prepare / load
 * @return @ref bleInvalidRange : freeOptions out of range
 * @return @ref bleGAPNotFound : advertising set does not exist
 * @return @ref bleAlreadyInRequestedMode : advertising set is not currently
 *         advertising
 * @return @ref bleGAPBufferInUse : can't free because this buffer is used
 *         elsewhere
 * @return @ref SUCCESS
 */
bStatus_t GapAdv_prepareLoadByHandle(uint8 handle,
                                     GapAdv_freeBufferOptions_t freeOptions);

/**
 * Load advertising / scan response data to an advertising set.
 *
 * The first use for this function is to load a new advertising / scan response
 * buffer to the advertising set initially after the set has been created with
 * @ref GapAdv_create.
 *
 * A second use for this function is in conjunction with
 * @ref GapAdv_prepareLoadByHandle in order to prevent double-copying of data
 * when updating the advertising / scan response data for a single handle.
 * Also, advertising will be automatically re-enabled by the host if it was
 * disabled during @ref GapAdv_prepareLoadByHandle. This will result in the
 * application receiving the @ref GAP_EVT_ADV_START_AFTER_ENABLE event.
 *
 * Since this function accepts any pointer, it is possible to pass in a new
 * buffer or re-use the original buffer (assuming it wasn't freed with
 * @ref GapAdv_prepareLoadByHandle).  It is also possible to modify the length
 * of the original buffer (assuming the buffer / length still point to valid
 * memory).
 *
 * @note The prepare / load operation must run to completion before another
 * prepare operation or advertising disable, enable, create, or destroy. It is
 * possible to abort the prepare / load with @ref GapAdv_abortLoad.
 *
 * @warning It is the application's responsibility to ensure that it is passing
 * a len / pBuf to valid memory.
 *
 * @par Corresponding Events
 * @ref GAP_EVT_ADV_START_AFTER_ENABLE
 *
 * @param handle Handle of advertising to (re)load advertising / scan response
 *        data
 * @param dataType see @ref GapAdv_dataTypes_t
 * @param len Length of new data
 * @param pBuf pointer to new data
 *
 * @return @ref bleIncorrectMode : incorrect profile role or
 *         @ref GapAdv_prepareLoadByHandle has not been called first
 * @return @ref bleInvalidRange : dataType out of range or len is too large
 * @return @ref bleGAPNotFound : advertising set does not exist
 * @return @ref bleGAPBufferInUse : can't free because this buffer is used
 *         elsewhere
 * @return @ref SUCCESS
 */
bStatus_t GapAdv_loadByHandle(uint8 handle, GapAdv_dataTypes_t dataType,
                              uint16 len, uint8 *pBuf);

/**
 * Used in conjunction with @ref GapAdv_loadByBuffer to update an advertising /
 * scan response buffer that is used by multiple advertising handles.
 *
 * This function must be used in the following cases:
 * -# It is desired to update the contents of a data buffer that is currently
 * being used by multiple advertising sets
 * -# It is desired to load a new buffer to multiple advertising sets without
 * double-copying
 *
 * This function is called first to search through all active advertising sets.
 * If the advertising or scan response data of an advertising set matches
 * the input buffer, this data will be marked as used.  If the advertising
 * set is currently advertising, it will be disabled and marked for re-enabling.
 * This re-enabling will occur when @ref GapAdv_loadByBuffer is called.
 *
 * For case 1, the buffer can be updated as desired after this function returns
 * successfully, then reloaded with @ref GapAdv_loadByBuffer
 *
 * For case 2, in order to prevent double-copying, the data that was being used
 * by the advertising sets should be freed before calling
 * @ref GapAdv_loadByBuffer. It is recommended that this is done by setting
 * the freeOldData of @ref GapAdv_prepareLoadByBuffer.  The new buffer can
 * then be loaded with @ref GapAdv_loadByBuffer
 *
 * @note The prepare / load operation must  run to completion before another
 * prepare operation, an advertising disable, enable, create, or destroy
 * can begin. It is possible to abort the prepare / load with
 * @ref GapAdv_abortLoad.
 *
 * @param pBuf pointer to buffer to prepare
 * @param freeOldData whether or not free the old pointer
 *
 * @return @ref bleIncorrectMode : incorrect profile role or there is already
 * an ongoing prepare or prepare / load
 * @return @ref bleGAPNotFound : no advertising sets are using this buffer
 * @return @ref SUCCESS
 */
extern bStatus_t GapAdv_prepareLoadByBuffer(uint8 *pBuf, bool freeOldData);

/**
 * Used in conjunction with @ref GapAdv_prepareLoadByBuffer to
 * update advertising / scan response buffers.
 *
 * This function is called after @ref GapAdv_prepareLoadByBuffer.  It will
 * search through all advertising sets and update any advertising data / scan
 * response buffers that were marked as used by @ref GapAdv_prepareLoadByBuffer.
 * It will also re-enabling any advertising sets that were disabled by
 * @ref GapAdv_prepareLoadByBuffer.
 *
 * Since this function accepts any pointer, it is possible to pass in a new
 * buffer or re-use the original buffer (assuming it wasn't freed with
 * @ref GapAdv_prepareLoadByBuffer).  It is also possible to modify the length
 * of the original buffer (assuming the buffer / length still point to valid
 * memory).
 *
 * @note The prepare / load operation must be run to completion before another
 * prepare operation, an advertising disable, enable, create, or destroy can
 * begin. It is possible to abort the prepare / load with @ref GapAdv_abortLoad
 *
 * @warning It is the application's responsibility to ensure that it is passing
 * a len / pBuf to valid memory.
 *
 * @param len length of buffer to load
 * @param pBuf pointer to buffer to load
 *
 * @return @ref bleIncorrectMode : incorrect profile role or
 * @ref GapAdv_prepareLoadByBuffer has not been called first
 * @return @ref SUCCESS
 */
extern bStatus_t GapAdv_loadByBuffer(uint16 len, uint8 *pBuf);


bStatus_t GapAdv_loadByBuffer_hook(uint16 len, uint8 *pBuf);

/**
 * Abort an advertising load operation.
 *
 * Can be used to abort a prepare or prepare / load operation after a call to
 * @ref GapAdv_prepareLoadByHandle or @ref GapAdv_prepareLoadByBuffer.
 *
 * @return @ref SUCCESS
 */
extern bStatus_t GapAdv_abortLoad(void);

/**
 * Set the advertiser's virtual public address.
 *
 * This API should be used after a handle was created using GapAdv_create
 * and before enabling the advertise set using GapAdv_enable.
 * Setting virtual address is allowed only for Legacy Non-Connectable and Non-Scanable
 * PDUs.
 * The virtual public address is not associated with the device's assigned public address.
 *
 * @design /ref 239346186
 *
 * @param  advHandle handle of the relevant advertise set.
 * @param  bdAddr    public virtual address which need to be assigned to the handle.
 *
 * @return INVALIDPARAMETER  bdAddr pointer is NULL
 * @return bleGAPNotFound advertising set does not exist
 * @return bleIncorrectMode advertise set is already active or wrong PDU was used
 * @return SUCCESS
 */
extern bStatus_t GapAdv_setVirtualAdvAddr(uint8 advHandle, uint8 *bdAddr);

/**
 * GapAdv_SetPeriodicAdvParams
 *
 * Set the advertiser parameters for the periodic advertising
 *
 * @design  /ref did_302932730
 *
 * @param   advHandle           - Used to identify a periodic advertising train
 *                                Created after creating extended advertising using GapAdv_create
 * @param   periodicAdvParams   - Pointer to periodic advertising parameters
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 * @return @ref bleInvalidRange
 * @return @ref bleGAPNotFound
 */
 uint8_t GapAdv_SetPeriodicAdvParams( uint8 advHandle,
                                      GapAdv_periodicAdvParams_t *periodicAdvParams );

/**
 * GapAdv_SetPeriodicAdvData
 *
 * Used to set the advertiser data used in periodic advertising PDUs.
 * This command may be issued at any time after the advertising set identified by
 * the Advertising_Handle parameter has been configured for periodic advertising
 *
 * @design  /ref did_302932730
 *
 * @param   advHandle           - Used to identify a periodic advertising train
 * @param   periodicAdvData     - Pointer to periodic advertising data
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 * @return @ref bleInvalidRange
 */
 bStatus_t GapAdv_SetPeriodicAdvData( uint8 advHandle,
                                      GapAdv_periodicAdvData_t *periodicAdvData );

/**
 * GapAdv_SetPeriodicAdvEnable
 *
 * Used to request the advertiser to enable or disable
 * the periodic advertising for the advertising set
 *
 * @design  /ref did_302932730
 *
 * @param   enable    - 0x00 - Periodic advertising is disabled (default)
 *                      0x01 - Periodic advertising is enabled
 * @param   advHandle - Used to identify a periodic advertising train
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 * @return @ref bleInvalidRange
 */
 bStatus_t GapAdv_SetPeriodicAdvEnable( uint8 enable,
                                        uint8 advHandle );

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* GAP_ADVERTISER_H */

/** @} End GapAdv */
