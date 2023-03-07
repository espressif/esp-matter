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
 *  @defgroup GAP GAP
 *  @brief This module implements the Generic Access Protocol (GAP)
 *  @{
 *  @file  gap.h
 *  @brief      GAP layer interface
 */

#ifndef GAP_H
#define GAP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */
#include <stdbool.h>
#include "bcomdef.h"
#include "osal.h"
#include "sm.h"
#include "ll_ae.h"

/*-------------------------------------------------------------------
 * MACROS
 */
/// @cond NODOC
#ifndef status_t
  #define status_t bStatus_t
#endif
/// @endcond // NODOC

/**
 * Is the address random private resolvable (RPA)?
 *
 * @param pAddr pointer to address
 *
 * @return TRUE the address is an RPA
 * @return FALSE the address is not an RPA
 */
#define GAP_IS_ADDR_RPR(pAddr) ((pAddr[B_ADDR_LEN-1] & RANDOM_ADDR_HDR_MASK) \
                                 == PRIVATE_RESOLVE_ADDR_HDR)

/**
 * Is the address random private non-resolvable (NRPA)?
 *
 * @param pAddr pointer to address
 *
 * @return TRUE the address is an NRPA
 * @return FALSE the address is not an NRPA
 */
#define GAP_IS_ADDR_RPN(pAddr) ((pAddr[B_ADDR_LEN-1] & RANDOM_ADDR_HDR_MASK) \
                                 == PRIVATE_NON_RESOLVE_ADDR_HDR)

/**
 * Is the address random static?
 *
 * @param pAddr pointer to address
 *
 * @return TRUE the address is random static
 * @return FALSE the address is not random static
 */
#define GAP_IS_ADDR_RS(pAddr)  ((pAddr[B_ADDR_LEN-1] & RANDOM_ADDR_HDR_MASK) \
                                 == STATIC_ADDR_HDR)

/**
 * Is the address any type of random address?
 */
#define GAP_IS_ADDR_RAND(pAddr) (GAP_IS_ADDR_RPR(pAddr) | \
                                 GAP_IS_ADDR_RPN(pAddr) | \
                                 GAP_IS_ADDR_RS(pAddr))

/*-------------------------------------------------------------------
 * CONSTANTS
 */

/**
 * @defgroup GAP_Events GAP Events
 * Event ID's that can be received from the GAP layer
 * @{
 */

/**
 * @defgroup GAP_Event_IDs GAP Event IDs
 * @{
 *
 * These events are received as @ref GAP_MSG_EVENT
 */
/**
 * Sent after a call to @ref GAP_DeviceInit when the Device Initialization is
 * complete as @ref gapDeviceInitDoneEvent_t.
 */
#define GAP_DEVICE_INIT_DONE_EVENT            0x00
/// @cond NODOC
/**
 * Sent when the Advertising Data or SCAN_RSP Data has been updated. This event
 * is sent as an OSAL message defined as @ref GapAdv_dataUpdateEvent_t.
 * This event sent only when using BLE3_CMD compilation flag.
 */
#define GAP_ADV_DATA_UPDATE_DONE_EVENT        0x02
/**
 * Sent when the GAP_MakeDiscoverable command is being used.
 * This event sent only when using BLE3_CMD compilation flag.
 */
#define GAP_ADV_MAKE_DISCOVERABLE_DONE_EVENT  0x03
/**
 * Sent when the GAP_EndDiscoverable command is being used.
 * This event sent only when using BLE3_CMD compilation flag.
 */
#define GAP_ADV_END_DISCOVERABLE_DONE_EVENT  0x04
/// @endcond NODOC
/**
 * Sent after a link has been established as  @ref gapEstLinkReqEvent_t.
 */
#define GAP_LINK_ESTABLISHED_EVENT            0x05
/**
 * Sent when a connection was terminated as @ref gapTerminateLinkEvent_t.
 */
#define GAP_LINK_TERMINATED_EVENT             0x06
/**
 * Sent after the completion of a parameter update  @ref gapLinkUpdateEvent_t.
 *
 * This same event is received for both the LL and L2CAP procedures.
 */
#define GAP_LINK_PARAM_UPDATE_EVENT           0x07
/**
 * Sent when the peer device's signature counter is updated as
 * @ref gapSignUpdateEvent_t.
 *
 * This event will be consumed by the gapbondmgr if it is present
 */
#define GAP_SIGNATURE_UPDATED_EVENT           0x09
/**
 * Sent when the pairing process is complete as @ref gapAuthCompleteEvent_t.
 *
 * This event will be consumed by the gapbondmgr if it is present
 */
#define GAP_AUTHENTICATION_COMPLETE_EVENT     0x0A
/**
 * Sent when a passkey is needed during pairing as @ref gapPasskeyNeededEvent_t.
 *
 * This event will be consumed by the gapbondmgr if it is present
 */
#define GAP_PASSKEY_NEEDED_EVENT              0x0B
/**
 * Sent when a Slave Security Request is received as
 * @ref gapSlaveSecurityReqEvent_t.
 *
 * This event will be consumed by the gapbondmgr if it is present
 */
#define GAP_SLAVE_REQUESTED_SECURITY_EVENT    0x0C
/**
 * Sent when the bonding process is complete as @ref gapBondCompleteEvent_t.
 *
 * This event will be consumed by the gapbondmgr if it is present
 */
#define GAP_BOND_COMPLETE_EVENT               0x0E
/**
 * Sent when an unexpected Pairing Request is received as
 * @ref gapPairingReqEvent_t.
 *
 * This event will be consumed by the gapbondmgr if it is present
 */
#define GAP_PAIRING_REQ_EVENT                 0x0F
/**
 * Sent when pairing fails due a connection termination before the process
 * completed. This event does not have any payload.
 *
 * This event will be consumed by the gapbondmgr if it is present.
 */
#define GAP_AUTHENTICATION_FAILURE_EVT        0x10
/**
 * Sent when a parameter update request is received. This event is only
 * received when @ref GAP_PARAM_LINK_UPDATE_DECISION is set to
 * @ref GAP_UPDATE_REQ_PASS_TO_APP.  This event is sent
 * as @ref gapUpdateLinkParamReqEvent_t.
 */
#define GAP_UPDATE_LINK_PARAM_REQ_EVENT       0x11
/// Sent when an advertising report session ends. This is an internal event.
#define GAP_SCAN_SESSION_END_EVENT            0x12
/// Sent when an advertising set needs to be removed. This is an internal event.
#define GAP_ADV_REMOVE_SET_EVENT              0x13
/**
 * Sent as @ref gapConnCancelledEvent_t when an a connecting attempt is canceled
 */
#define GAP_CONNECTING_CANCELLED_EVENT        0x15

/**
 * Sent as @ref gapBondLostEvent_t when bond has been removed on peer
 */
#define GAP_BOND_LOST_EVENT                   0x17

/**
 * Sent after a recieved Connection Parameter Update Request was rejected
 * by this device.
 */
#define GAP_LINK_PARAM_UPDATE_REJECT_EVENT    0x18

/**
 * Periodic Advertising Advertiser command complete events.
 */
/// Set periodic advertising parameters command complete event
#define GAP_ADV_SET_PERIODIC_ADV_PARAMS_EVENT 0x19
/// Set periodic advertising data command complete event
#define GAP_ADV_SET_PERIODIC_ADV_DATA_EVENT   0x1A
/// Set periodic advertising enable command complete event
#define GAP_ADV_SET_PERIODIC_ADV_ENABLE_EVENT 0x1B

/**
 * Periodic Advertising Scanner command status event.
 */
#define GAP_SCAN_CREATE_SYNC_EVENT        	    0x1C

/**
 * Periodic Advertising Scanner command complete events.
 */
/// Periodic advertising create sync command complete event
#define GAP_SCAN_SYNC_CANCEL_EVENT              0x1D
/// Periodic advertising terminate sync command complete event
#define GAP_SCAN_TERMINATE_SYNC_EVENT           0x1E
/// Periodic advertising receive enable command complete event
#define GAP_SCAN_PERIODIC_RECEIVE_EVENT         0x1F
/// Add device to period advertisers list command complete event
#define GAP_SCAN_ADD_DEVICE_ADV_LIST_EVENT      0x20
/// Remove device from period advertisers list create sync command complete event
#define GAP_SCAN_REMOVE_DEVICE_ADV_LIST_EVENT   0x21
/// Read period advertisers list size command complete event
#define GAP_SCAN_READ_ADV_LIST_SIZE_EVENT       0x22
/// Clear period advertisers list command complete event
#define GAP_SCAN_CLEAR_ADV_LIST_EVENT           0x23

/**
 * Periodic Advertising Scanner establish event.
 */
#define GAP_SCAN_PERIODIC_ADV_SYNC_EST_EVENT    0x24

/**
 * Periodic Advertising Scanner sync lost event.
 */
#define GAP_SCAN_PERIODIC_ADV_SYNC_LOST_EVENT    0x25

/**
 * Periodic Advertising Scanner report event.
 */
#define GAP_SCAN_PERIODIC_ADV_REPORT_EVENT       0x26
/** @} End GAP_Event_IDs */

/**
 * @defgroup GapAdvScan_Event_IDs GapAdv Event IDs
 * These are received through the @ref pfnGapCB_t registered in the
 * @ref GapAdv_create
 *
 * See the individual event to see how pBuf in the @ref pfnGapCB_t should be
 * cast.
 * @{
 */
/**
 * Sent on the first advertisement after a @ref GapAdv_enable
 *
 * pBuf should be cast to a uint8_t which will contain the advertising handle
 */
#define GAP_EVT_ADV_START_AFTER_ENABLE             (uint32_t)BV(0)
/**
 * Sent after advertising stops due to a @ref GapAdv_disable
 *
 * pBuf should be cast to a uint8_t which will contain the advertising handle
 */
#define GAP_EVT_ADV_END_AFTER_DISABLE              (uint32_t)BV(1)
/**
 * Sent at the beginning of each advertisement (for legacy advertising) or at
 * the beginning of each each advertisement set (for extended advertising)
 *
 * pBuf should be cast to a uint8_t which will contain the advertising handle
 */
#define GAP_EVT_ADV_START                          (uint32_t)BV(2)
/**
 * Sent after each advertisement (for legacy advertising) or at
 * the end of each each advertisement set (for extended advertising)
 *
 * pBuf should be cast to a uint8_t which will contain the advertising handle
 */
#define GAP_EVT_ADV_END                            (uint32_t)BV(3)
/**
 * Sent when an advertisement set is terminated due to a connection
 * establishment
 *
 * pBuf should be cast to @ref GapAdv_setTerm_t
 */
#define GAP_EVT_ADV_SET_TERMINATED                 (uint32_t)BV(4)
/**
 * Sent when a scan request is received
 *
 * pBuf should be cast to @ref GapAdv_scanReqReceived_t
 */
#define GAP_EVT_SCAN_REQ_RECEIVED                  (uint32_t)BV(5)
/**
 * Sent when the advertising data is truncated due to the limited advertisement
 * data length for connectable advertisements.
 *
 * pBuf should be cast to @ref GapAdv_truncData_t
 */
#define GAP_EVT_ADV_DATA_TRUNCATED                 (uint32_t)BV(6)
/// Scanner has been enabled.
#define GAP_EVT_SCAN_ENABLED                       (uint32_t)BV(16)
/**
 * Scanner has been disabled.
 *
 * This event comes with a message of @ref GapScan_Evt_End_t. Application is
 * responsible for freeing the message.
 */
#define GAP_EVT_SCAN_DISABLED                      (uint32_t)BV(17)
/**
 * Scan period has ended.
 *
 * Possibly a new scan period and a new scan duration have started.
 */
#define GAP_EVT_SCAN_PRD_ENDED                     (uint32_t)BV(18)
/// Scan duration has ended.
#define GAP_EVT_SCAN_DUR_ENDED                     (uint32_t)BV(19)
/**
 * Scan interval has ended.
 *
 * Possibly a new scan interval and a new scan window have started.
 */
#define GAP_EVT_SCAN_INT_ENDED                     (uint32_t)BV(20)
/// Scan window has ended.
#define GAP_EVT_SCAN_WND_ENDED                     (uint32_t)BV(21)
/**
 * An Adv or a ScanRsp has been received.
 *
 * This event comes with a message of @ref GapScan_Evt_AdvRpt_t. Application is
 * responsible for freeing both the message and the message data (msg->pData).
 */
#define GAP_EVT_ADV_REPORT                         (uint32_t)BV(22)
/**
 * Maximum number of Adv reports have been recorded.
 *
 * This event comes with a message of @ref GapScan_Evt_AdvRpt_t.
 */
#define GAP_EVT_ADV_REPORT_FULL                    (uint32_t)BV(23)
/// @cond NODOC
/**
 * A Sync with a periodic Adv has been established.
 *
 * This event comes with a message of @ref GapScan_Evt_PrdAdvSyncEst_t.
 * Application is responsible for freeing the message.
 */
#define GAP_EVT_PRD_ADV_SYNC_ESTABLISHED           (uint32_t)BV(24)
/**
 * A periodic Adv has been received.
 *
 * This event comes with a message of @ref GapScan_Evt_PrdAdvRpt_t. Application
 * is responsible for freeing both the message and the message data (msg->pData).
 */
#define GAP_EVT_PRD_ADV_REPORT                     (uint32_t)BV(25)
/**
 * A sync with a periodic has been lost.
 *
 * This event comes with a message of @ref GapScan_Evt_PrdAdvSyncLost_t.
 * Application is responsible for freeing the message.
 */
#define GAP_EVT_PRD_ADV_SYNC_LOST                  (uint32_t)BV(26)
/// @endcond NODOC
/// A memory failure has occurred.
#define GAP_EVT_INSUFFICIENT_MEMORY                (uint32_t)BV(31)

/// Mask for all advertising events
#define GAP_EVT_ADV_EVT_MASK   (GAP_EVT_ADV_START_AFTER_ENABLE |   \
                                GAP_EVT_ADV_END_AFTER_DISABLE |    \
                                GAP_EVT_ADV_START |                \
                                GAP_EVT_ADV_END |                  \
                                GAP_EVT_ADV_SET_TERMINATED |       \
                                GAP_EVT_SCAN_REQ_RECEIVED |        \
                                GAP_EVT_ADV_DATA_TRUNCATED |       \
                                GAP_EVT_INSUFFICIENT_MEMORY)

/// Mask for all scan events
#define GAP_EVT_SCAN_EVT_MASK  (GAP_EVT_SCAN_ENABLED |                    \
                                GAP_EVT_SCAN_DISABLED |                   \
                                GAP_EVT_SCAN_PRD_ENDED |                  \
                                GAP_EVT_SCAN_DUR_ENDED |                  \
                                GAP_EVT_SCAN_INT_ENDED |                  \
                                GAP_EVT_SCAN_WND_ENDED |                  \
                                GAP_EVT_ADV_REPORT |                      \
                                GAP_EVT_ADV_REPORT_FULL |                 \
                                GAP_EVT_PRD_ADV_SYNC_ESTABLISHED |        \
                                GAP_EVT_PRD_ADV_REPORT |                  \
                                GAP_EVT_PRD_ADV_SYNC_LOST |               \
                                GAP_EVT_INSUFFICIENT_MEMORY)

/** @} End GapAdvScan_Event_IDs */

/** @} End GAP_Events */

/**
 * @defgroup GAP_Params GAP Parameters
 *
 * GAP layer parameters
 * @{
 */

/**
 * GAP Configuration Parameters
 *
 * These can be set with @ref GapConfig_SetParameter
 */
typedef enum
{
  /**
   * @brief Can be used by the application to set the IRK
   *
   * It is not necessary to set this parameter. If it is not set, a random IRK
   * will be generated unless there is a valid IRK in NV.
   *
   * @warning This must be set before calling @ref GAP_DeviceInit
   *
   * The priority is:
   * 1. Set manually with GapConfig_SetParameter before @ref GAP_DeviceInit
   * 2. Previously stored in NV by the gapbondmgr
   * 3. Set randomly during @ref GAP_DeviceInit
   *
   * size: 16 bytes
   *
   * @note The IRK can be read with @ref GAP_GetIRK
   */
  GAP_CONFIG_PARAM_IRK,

  /**
   * @brief Can be used by the application to set the SRK
   *
   * It is not necessary to set this parameter. If it is not set, a random SRK
   * will be generated unless there is a valid SRK in NV.
   *
   * @warning This must be set before calling @ref GAP_DeviceInit
   *
   * The priority is:
   * 1. Set manually with GapConfig_SetParameter before @ref GAP_DeviceInit
   * 2. Previously stored in NV by the gapbondmgr
   * 3. Set randomly during @ref GAP_DeviceInit
   *
   * size: 16 bytes
   *
   * @note The SRK can be read with @ref GAP_GetSRK
   */
  GAP_CONFIG_PARAM_SRK,

/// @cond NODOC
  GAP_CONFIG_PARAM_COUNT
/// @endcond //NODOC
} Gap_configParamIds_t;

/**
 * GAP Parameter IDs
 *
 * Parameters set via @ref GAP_SetParamValue
 */
enum Gap_ParamIDs_t
{
  /**
   * Action to take upon receiving a parameter update request.
   *
   * default: @ref GAP_UPDATE_REQ_PASS_TO_APP
   *
   * range: @ref Gap_updateDecision_t
   */
  GAP_PARAM_LINK_UPDATE_DECISION,

  /**
   * Connection Parameter timeout.
   *
   * Minimum time after an L2CAP Connection Parameter Update Response has been
   * received that a L2CAP Connection Parameter Update Request can be sent.
   *
   * See section Version 5.0 Vol 3, Part C, Section 9.3.9.2 of the BT Core Spec
   *
   * default: 30000
   *
   * range: 1-65535
   */
  GAP_PARAM_CONN_PARAM_TIMEOUT,

  /**
   * Minimum Time Interval between private (resolvable) address changes
   * (minutes)
   *
   * @note No event is received when the address changes as per the Core Spec.
   *
   * default: 15
   *
   * range: 1-65535
   */
  GAP_PARAM_PRIVATE_ADDR_INT,

  /**
   * Time to wait for security manager response before returning
   * bleTimeout (ms)
   *
   * default: 30000
   *
   * range: 1-65535
   */
  GAP_PARAM_SM_TIMEOUT,

  /**
   * SM Minimum Key Length supported
   *
   * default: 7
   *
   * range: 1-65535
   */
  GAP_PARAM_SM_MIN_KEY_LEN,

  /**
   * SM Maximum Key Length supported
   *
   * default: 16
   *
   * range: 1-65535
   */
  GAP_PARAM_SM_MAX_KEY_LEN,

/// @cond NODOC
  /**
   * Task ID override for Task Authentication control (for stack internal use
   * only)
   */
  GAP_PARAM_AUTH_TASK_ID,

/**
 * This parameter is deprecated. This value is to avoid modifying the
 * following values.
 */
  GAP_PARAM_DEPRECATED,

  /**
   * Used to set GAP GATT Server (GGS) parameters. This is only used by the
   * transport layer
   *
   * default: 5
   *
   */
  GAP_PARAM_GGS_PARAMS,

#if defined ( TESTMODES )
  /**
   * GAP TestCodes - puts GAP into a test mode
   */
  GAP_PARAM_GAP_TESTCODE,

  /**
   * SM TestCodes - puts SM into a test mode
   */
  GAP_PARAM_SM_TESTCODE,

  /**
   * GATT TestCodes - puts GATT into a test mode (paramValue maintained by GATT)
   */
  GAP_PARAM_GATT_TESTCODE,

  /**
   * ATT TestCodes - puts ATT into a test mode (paramValue maintained by ATT)
   */
  GAP_PARAM_ATT_TESTCODE,

  /**
   * L2CAP TestCodes - puts L2CAP into a test mode (paramValue maintained by
   * L2CAP)
   */
  GAP_PARAM_L2CAP_TESTCODE,

  /**
   * @brief Set the scan channel parameters for flash only
   *
   * default: all channels GAP_ADV_CHAN_ALL
   *
   * values: GAP_ADV_CHAN_37, GAP_ADV_CHAN_38, GAP_ADV_CHAN_39, GAP_ADV_CHAN_37_38
   *         GAP_ADV_CHAN_37_39, GAP_ADV_CHAN_38_39, GAP_ADV_CHAN_ALL
   *         @ref GAP_scanChannels_t.
   */
  GAP_PARAM_SET_SCAN_CHAN_TESTCODE,

#endif // TESTMODES

   /**
   * GAP STUB1 - Not test mode
   * Add a stub in order to have the same
   * Number of commands in Flash-Rom(!TESTMODES) and Flash-Only (TESTMODES)
   * ADD A NEW FLASH-ROM COMMAND HERE.
   */
  GAP_STUB1_NOT_TESTCODE,

   /**
   * GAP STUB2 - Not test mode
   * Add a stub in order to have the same
   * Number of commands in Flash-Rom(!TESTMODES) and Flash-Only (TESTMODES)
   * ADD A NEW FLASH-ROM COMMAND HERE.
   */
  GAP_STUB2_NOT_TESTCODE,

   /**
   * GAP STUB3 - Not test mode
   * Add a stub in order to have the same
   * Number of commands in Flash-Rom(!TESTMODES) and Flash-Only (TESTMODES)
   * ADD A NEW FLASH-ROM COMMAND HERE.
   */
  GAP_STUB3_NOT_TESTCODE,

   /**
   * GAP STUB4 - Not test mode
   * Add a stub in order to have the same
   * Number of commands in Flash-Rom(!TESTMODES) and Flash-Only (TESTMODES)
   * ADD A NEW FLASH-ROM COMMAND HERE.
   */
  GAP_STUB4_NOT_TESTCODE,

   /**
   * GAP STUB5 - Not test mode
   * Add a stub in order to have the same
   * Number of commands in Flash-Rom(!TESTMODES) and Flash-Only (TESTMODES)
   * ADD A NEW FLASH-ROM COMMAND HERE.
   */
  GAP_STUB5_NOT_TESTCODE,

   /**
   * @brief Set the scan channel parameters for flash rom
   *
   * default: all channels GAP_ADV_CHAN_ALL
   *
   * values: GAP_ADV_CHAN_37, GAP_ADV_CHAN_38, GAP_ADV_CHAN_39, GAP_ADV_CHAN_37_38
   *         GAP_ADV_CHAN_37_39, GAP_ADV_CHAN_38_39, GAP_ADV_CHAN_ALL
   *         @ref GAP_scanChannels_t.
   */
  GAP_PARAM_SET_SCAN_CHAN,

  /**
   * ID MAX-valid Parameter ID
   */
  GAP_PARAMID_MAX
/// @endcond //NODOC
};

/** @} End GAP_Params */

/**
 * @defgroup GAP_Constants GAP Constants
 * Other defines used in the GAP layer
 * @{
 */

/**
 * @defgroup GAP_Profile_Roles GAP Profile Roles
 * Bit mask values
 * @{
 */
/// A device that sends advertising events only.
#define GAP_PROFILE_BROADCASTER   0x01
/// A device that receives advertising events only.
#define GAP_PROFILE_OBSERVER      0x02
/**
 * A device that accepts the establishment of an LE physical link using the
 * establishment procedure.
 */
#define GAP_PROFILE_PERIPHERAL    0x04
/**
 * A device that supports the Central role initiates the establishment of a
 * physical connection.
 */
#define GAP_PROFILE_CENTRAL       0x08
/** @} End GAP_Profile_Roles */

/**
 * Options for responding to connection parameter update requests
 *
 * These are used by @ref GAP_PARAM_LINK_UPDATE_DECISION
 */
typedef enum
{
  GAP_UPDATE_REQ_ACCEPT_ALL,     //!< Accept all parameter update requests
  GAP_UPDATE_REQ_DENY_ALL,       //!< Deny all parameter update requests
  /**
   * Pass a @ref GAP_UPDATE_LINK_PARAM_REQ_EVENT to the app for it to decide by
   * responding with @ref GAP_UpdateLinkParamReqReply
   */
  GAP_UPDATE_REQ_PASS_TO_APP
} Gap_updateDecision_t;

/// Address modes to initialize the local device
typedef enum
{
  ADDRMODE_PUBLIC            = 0x00,  //!< Always Use Public Address
  ADDRMODE_RANDOM            = 0x01,  //!< Always Use Random Static Address
  /// Always Use Resolvable Private Address with Public Identity Address
  ADDRMODE_RP_WITH_PUBLIC_ID = 0x02,
  /// Always Use Resolvable Private Address with Random Identity Address
  ADDRMODE_RP_WITH_RANDOM_ID = 0x03,
} GAP_Addr_Modes_t;

/// Address types used for identifying peer address type
typedef enum
{
  ADDRTYPE_PUBLIC    = 0x00,  //!< Public Device Address
  ADDRTYPE_RANDOM    = 0x01,  //!< Random Device Address
  /// Public Identity Address (corresponds to peer's RPA)
  ADDRTYPE_PUBLIC_ID = 0x02,
  /// Random (static) Identity Address (corresponds to peer's RPA)
  ADDRTYPE_RANDOM_ID = 0x03,
  /// Random Device Address (controller unable to resolve)
  ADDRTYPE_RANDOM_NR = 0xFE,
  ADDRTYPE_NONE      = 0xFF   //!< No address provided
} GAP_Addr_Types_t;

/// Address types used for specifying peer address type
typedef enum
{
  PEER_ADDRTYPE_PUBLIC_OR_PUBLIC_ID = 0x00,  //!< Public or Public ID Address
  PEER_ADDRTYPE_RANDOM_OR_RANDOM_ID = 0x01   //!< Random or Random ID Address
} GAP_Peer_Addr_Types_t;

/**
 * @defgroup Address_IDs Masks for setting and getting ID type
 * @{
 */
/// OR with addrtype to change addr type to ID
#define SET_ADDRTYPE_ID               0x02
/// AND with addrtype to remove ID from type
#define MASK_ADDRTYPE_ID              0x01
/** @} End Address_IDs */

/**
 * @defgroup Random_Addr_Bitfields Random Address bit-field mask and types
 * @{
 */
/// Get top 2 bits of address
#define RANDOM_ADDR_HDR_MASK          0xC0
/// Random Static Address (b11)
#define STATIC_ADDR_HDR               0xC0
/// Random Private Non-Resolvable Address (b10)
#define PRIVATE_NON_RESOLVE_ADDR_HDR  0x80
/// Random Private Resolvable Address (b01)
#define PRIVATE_RESOLVE_ADDR_HDR      0x40
/** @} End Random_Addr_Bitfields */

/**
 * @defgroup GAP_ADTypes GAP Advertisement Data Types
 * These are the data type identifiers for the data tokens in the advertisement
 * data field.
 * @{
 */
/// Gap Advertising Flags
#define GAP_ADTYPE_FLAGS                        0x01
/// Service: More 16-bit UUIDs available
#define GAP_ADTYPE_16BIT_MORE                   0x02
/// Service: Complete list of 16-bit UUIDs
#define GAP_ADTYPE_16BIT_COMPLETE               0x03
/// Service: More 32-bit UUIDs available
#define GAP_ADTYPE_32BIT_MORE                   0x04
/// Service: Complete list of 32-bit UUIDs
#define GAP_ADTYPE_32BIT_COMPLETE               0x05
/// Service: More 128-bit UUIDs available
#define GAP_ADTYPE_128BIT_MORE                  0x06
/// Service: Complete list of 128-bit UUIDs
#define GAP_ADTYPE_128BIT_COMPLETE              0x07
/// Shortened local name
#define GAP_ADTYPE_LOCAL_NAME_SHORT             0x08
/// Complete local name
#define GAP_ADTYPE_LOCAL_NAME_COMPLETE          0x09
/// TX Power Level: 0xXX: -127 to +127 dBm
#define GAP_ADTYPE_POWER_LEVEL                  0x0A
/// Simple Pairing OOB Tag: Class of device (3 octets)
#define GAP_ADTYPE_OOB_CLASS_OF_DEVICE          0x0D
/// Simple Pairing OOB Tag: Simple Pairing Hash C (16 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_HASHC     0x0E
/// Simple Pairing OOB Tag: Simple Pairing Randomizer R (16 octets)
#define GAP_ADTYPE_OOB_SIMPLE_PAIRING_RANDR     0x0F
/// Security Manager TK Value
#define GAP_ADTYPE_SM_TK                        0x10
/// Security Manager OOB Flags
#define GAP_ADTYPE_SM_OOB_FLAG                  0x11
/**
 * Min and Max values of the connection interval (2 octets Min, 2 octets Max)
 * (0xFFFF indicates no conn interval min or max)
 */
#define GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE    0x12
/// Signed Data field
#define GAP_ADTYPE_SIGNED_DATA                  0x13
/// Service Solicitation: list of 16-bit Service UUIDs
#define GAP_ADTYPE_SERVICES_LIST_16BIT          0x14
/// Service Solicitation: list of 128-bit Service UUIDs
#define GAP_ADTYPE_SERVICES_LIST_128BIT         0x15
/// Service Data - 16-bit UUID
#define GAP_ADTYPE_SERVICE_DATA                 0x16
/// Public Target Address
#define GAP_ADTYPE_PUBLIC_TARGET_ADDR           0x17
/// Random Target Address
#define GAP_ADTYPE_RANDOM_TARGET_ADDR           0x18
/// Appearance
#define GAP_ADTYPE_APPEARANCE                   0x19
/// Advertising Interval
#define GAP_ADTYPE_ADV_INTERVAL                 0x1A
/// LE Bluetooth Device Address
#define GAP_ADTYPE_LE_BD_ADDR                   0x1B
/// LE Role
#define GAP_ADTYPE_LE_ROLE                      0x1C
/// Simple Pairing Hash C-256
#define GAP_ADTYPE_SIMPLE_PAIRING_HASHC_256     0x1D
/// Simple Pairing Randomizer R-256
#define GAP_ADTYPE_SIMPLE_PAIRING_RANDR_256     0x1E
/// Service Solicitation: list of 32-bit Service UUIDs
#define GAP_ADTYPE_SERVICES_LIST_32BIT          0x1F
/// Service Data - 32-bit UUID
#define GAP_ADTYPE_SERVICE_DATA_32BIT           0x20
/// Service Data - 128-bit UUID
#define GAP_ADTYPE_SERVICE_DATA_128BIT          0x21
/// 3D Information Data
#define GAP_ADTYPE_3D_INFO_DATA                 0x3D
/**
 * Manufacturer Specific Data: first 2 octets contain the Company Identifier
 * Code followed by the additional manufacturer specific data
 */
#define GAP_ADTYPE_MANUFACTURER_SPECIFIC        0xFF
/// Discovery Mode: LE Limited Discoverable Mode
#define GAP_ADTYPE_FLAGS_LIMITED                0x01
/// Discovery Mode: LE General Discoverable Mode
#define GAP_ADTYPE_FLAGS_GENERAL                0x02
/// Discovery Mode: BR/EDR Not Supported
#define GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED    0x04
/** @} End GAP_ADTypes */

/**
 * @defgroup GAP_State_Flags GAP State Flags
 * @{
 */
#define GAP_STATE_IDLE                          0x00 //!< Device is idle
#define GAP_STATE_ADV                           0x01 //!< Device is advertising
#define GAP_STATE_SCAN                          0x02 //!< Device is scanning
#define GAP_STATE_INIT                          0x04 //!< Device is establishing a connection
/** @} End GAP_State_Flags */

/**
 * @defgroup GAP_Appearance_Values GAP Appearance Values
 * @{
 */
#define GAP_APPEARE_UNKNOWN                     0x0000 //!< Unknown
#define GAP_APPEARE_GENERIC_PHONE               0x0040 //!< Generic Phone
#define GAP_APPEARE_GENERIC_COMPUTER            0x0080 //!< Generic Computer
#define GAP_APPEARE_GENERIC_WATCH               0x00C0 //!< Generic Watch
#define GAP_APPEARE_WATCH_SPORTS                0x00C1 //!< Watch: Sports Watch
#define GAP_APPEARE_GENERIC_CLOCK               0x0100 //!< Generic Clock
#define GAP_APPEARE_GENERIC_DISPLAY             0x0140 //!< Generic Display
#define GAP_APPEARE_GENERIC_RC                  0x0180 //!< Generic Remote Control
#define GAP_APPEARE_GENERIC_EYE_GALSSES         0x01C0 //!< Generic Eye-glasses
#define GAP_APPEARE_GENERIC_TAG                 0x0200 //!< Generic Tag
#define GAP_APPEARE_GENERIC_KEYRING             0x0240 //!< Generic Keyring
#define GAP_APPEARE_GENERIC_MEDIA_PLAYER        0x0280 //!< Generic Media Player
#define GAP_APPEARE_GENERIC_BARCODE_SCANNER     0x02C0 //!< Generic Barcode Scanner
#define GAP_APPEARE_GENERIC_THERMOMETER         0x0300 //!< Generic Thermometer
#define GAP_APPEARE_GENERIC_THERMO_EAR          0x0301 //!< Thermometer: Ear
#define GAP_APPEARE_GENERIC_HR_SENSOR           0x0340 //!< Generic Heart rate Sensor
#define GAP_APPEARE_GENERIC_HRS_BELT            0x0341 //!< Heart Rate Sensor: Heart Rate Belt
#define GAP_APPEARE_GENERIC_BLOOD_PRESSURE      0x0380 //!< Generic Blood Pressure
#define GAP_APPEARE_GENERIC_BP_ARM              0x0381 //!< Blood Pressure: Arm
#define GAP_APPEARE_GENERIC_BP_WRIST            0x0382 //!< Blood Pressure: Wrist
#define GAP_APPEARE_GENERIC_HID                 0x03C0 //!< Generic Human Interface Device (HID)
#define GAP_APPEARE_HID_KEYBOARD                0x03C1 //!< HID Keyboard
#define GAP_APPEARE_HID_MOUSE                   0x03C2 //!< HID Mouse
#define GAP_APPEARE_HID_JOYSTIC                 0x03C3 //!< HID Joystick
#define GAP_APPEARE_HID_GAMEPAD                 0x03C4 //!< HID Gamepad
#define GAP_APPEARE_HID_DIGITIZER_TYABLET       0x03C5 //!< HID Digitizer Tablet
#define GAP_APPEARE_HID_DIGITAL_CARDREADER      0x03C6 //!< HID Card Reader
#define GAP_APPEARE_HID_DIGITAL_PEN             0x03C7 //!< HID Digital Pen
#define GAP_APPEARE_HID_BARCODE_SCANNER         0x03C8 //!< HID Barcode Scanner
/** @} End GAP_Appearance_Values */

/**
 * @defgroup GAP_PRIVACY_MODES GAP Privacy Modes
 * @{
 */
#define GAP_PRIVACY_MODE_NETWORK                0 //!< Device Privacy Mode
#define GAP_PRIVACY_MODE_DEVICE                 1 //!< Network Privacy Mode
/** @} End GAP_PRIVACY_MODES */

/**
 * Connection Event Notice PHY's
 */
typedef enum
{
  GAP_CONN_EVT_PHY_1MBPS = 1, //!< 1 MBPS
  GAP_CONN_EVT_PHY_2MBPS = 2, //!< 2 MBPS
  GAP_CONN_EVT_PHY_CODED = 4, //!< Coded-S2 or Coded-S8
} GAP_ConnEvtPhy_t;

/**
 * Status of connection events returned via @ref pfnGapConnEvtCB_t
 */
typedef enum
{
  /// Connection event occurred successfully
  GAP_CONN_EVT_STAT_SUCCESS   = LL_CONN_EVT_STAT_SUCCESS,
  /// Connection event failed because all packets had CRC errors
  GAP_CONN_EVT_STAT_CRC_ERROR = LL_CONN_EVT_STAT_CRC_ERROR,
  /// No data was received during connection event
  GAP_CONN_EVT_STAT_MISSED    = LL_CONN_EVT_STAT_MISSED
} GAP_ConnEvtStat_t;

/**
 * Task type for next scheduled BLE task
 */
typedef enum
{
  /// Advertiser
  GAP_CONN_EVT_TASK_TYPE_ADV    = LL_TASK_ID_ADVERTISER,
  /// Initiating a connection
  GAP_CONN_EVT_TASK_TYPE_INIT   = LL_TASK_ID_INITIATOR,
  /// Connection event in slave role
  GAP_CONN_EVT_TASK_TYPE_SLAVE  = LL_TASK_ID_SLAVE,
  /// Scanner
  GAP_CONN_EVT_TASK_TYPE_SCAN   = LL_TASK_ID_SCANNER,
  /// Connection event in master role
  GAP_CONN_EVT_TASK_TYPE_MASTER = LL_TASK_ID_MASTER,
  // No task
  GAP_CONN_EVT_TASK_TYPE_NONE   = LL_TASK_ID_NONE
} GAP_ConnEvtTaskType_t;

/**
 * Action to take for callback registration API's
 */
typedef enum
{
  GAP_CB_REGISTER, //!< Register a callback
  GAP_CB_UNREGISTER //!> Unregister a callback
} GAP_CB_Action_t;

/** @} End GAP_Constants */

/*-------------------------------------------------------------------
 * TYPEDEFS
 */

/**
 * @defgroup GAP_Structs GAP Structures
 * @{
 */

/// GAP event header format.
typedef struct
{
  osal_event_hdr_t  hdr;           //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;                  //!< GAP type of command. @ref GAP_Event_IDs
} gapEventHdr_t;

/**
 * Peripheral Preferred Connection Parameters.
 *
 * This is used to set the @ref GGS_PERI_CONN_PARAM_ATT param with
 * @ref GGS_SetParameter
 */
typedef struct
{
  /// Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms)
  uint16_t intervalMin;
  /// Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms)
  uint16_t intervalMax;
  /// Number of LL latency connection events (0x0000 - 0x03e8)
  uint16_t latency;
  /// Connection Timeout (0x000A - 0x0C80 * 10 ms)
  uint16_t timeout;
} gapPeriConnectParams_t;

/**
 * @ref GAP_DEVICE_INIT_DONE_EVENT message format.
 *
 * This message is sent to the
 * app when the Device Initialization is done [initiated by calling
 * @ref GAP_DeviceInit ].
 */
typedef struct
{
  osal_event_hdr_t  hdr;                //!< @ref GAP_MSG_EVENT and status
  uint8_t  opcode;                      //!< @ref GAP_DEVICE_INIT_DONE_EVENT
  uint8_t  numDataPkts;                 //!< HC_Total_Num_LE_Data_Packets
  uint16_t dataPktLen;                  //!< HC_LE_Data_Packet_Length
  uint8_t  devAddr[B_ADDR_LEN];         //!< Device's public or random static address
} gapDeviceInitDoneEvent_t;

/**
 * @ref GAP_SIGNATURE_UPDATED_EVENT message format.
 *
 * This message is sent to the
 * app when the signature counter has changed.  This message is to inform the
 * application in case it wants to save it to be restored on reboot or reconnect.
 * This message is sent to update a connection's signature counter and to update
 * this device's signature counter.  If devAddr == BD_ADDR, then this message
 * pertains to this device.
 *
 * This event will be consumed by the gapbondmgr if it exists.
 */
typedef struct
{
  osal_event_hdr_t  hdr;            //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;                   //!< @ref GAP_SIGNATURE_UPDATED_EVENT
  uint8_t addrType;                 //!< Device's address type for devAddr
  uint8_t devAddr[B_ADDR_LEN];      //!< Device's BD_ADDR, could be own address
  uint32_t signCounter;             //!< new Signed Counter
} gapSignUpdateEvent_t;

/**
 * Establish Link Request parameters
 *
 * This is used by @ref GAP_UpdateLinkParamReq
 */
typedef struct
{
  uint16_t connectionHandle; //!< Connection handle of the update
  uint16_t intervalMin;      //!< Minimum Connection Interval
  uint16_t intervalMax;      //!< Maximum Connection Interval
  uint16_t connLatency;      //!< Connection Latency
  uint16_t connTimeout;      //!< Connection Timeout
  uint8_t  signalIdentifier; //!< L2CAP Signal Identifier. Must be 0 for LL Update
} gapUpdateLinkParamReq_t;

/**
 * Update Link Parameters Request Reply parameters
 *
 * This is used by @ref GAP_UpdateLinkParamReqReply
 */
typedef struct
{
  uint16_t connectionHandle; //!< Connection handle of the update
  uint16_t intervalMin;      //!< Minimum Connection Interval
  uint16_t intervalMax;      //!< Maximum Connection Interval
  uint16_t connLatency;      //!< Connection Latency
  uint16_t connTimeout;      //!< Connection Timeout
  uint8_t  signalIdentifier; //!< L2CAP Signal Identifier.
  uint8_t  accepted;         //!< TRUE if host accepts parameter update, FALSE otherwise.
} gapUpdateLinkParamReqReply_t;

/**
 *  @brief @ref GAP_UPDATE_LINK_PARAM_REQ_EVENT message format.
 *
 *  Connection parameters received by the remote device during a connection
 *  update procedure.
 */
typedef struct
{
  osal_event_hdr_t  hdr;              //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;                     //!< @ref GAP_UPDATE_LINK_PARAM_REQ_EVENT
  gapUpdateLinkParamReq_t req;        //!< Remote device's requested parameters
} gapUpdateLinkParamReqEvent_t;

/**
 * @ref GAP_LINK_ESTABLISHED_EVENT message format.
 *
 * This message is sent to the app when a link is established (with status
 * SUCCESS). For a Central, this is after @ref GapInit_connect or
 * @ref GapInit_connectWl completes successfully. For a Peripheral, this message
 * is sent to indicate that a link has been created.
 *
 * A status of something other than SUCCESS is possible in the following cases
 * - LL_STATUS_ERROR_UNKNOWN_CONN_HANDLE (0x02): As a master, connection
 * creation has been canceled.
 * - LL_STATUS_ERROR_DIRECTED_ADV_TIMEOUT (0x3C): As a slave, directed
 * advertising ended without a connection being formed.
 * - LL_STATUS_ERROR_UNACCEPTABLE_CONN_INTERVAL (0x3B): Slave received a
 * connection request with an invalid combination of connection parameters.
 */
typedef struct
{
  osal_event_hdr_t  hdr;       //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;              //!< @ref GAP_LINK_ESTABLISHED_EVENT
  uint8_t devAddrType;         //!< Device address type: @ref GAP_Addr_Types_t
  uint8_t devAddr[B_ADDR_LEN]; //!< Device address of link
  uint16_t connectionHandle;   //!< Connection Handle for this connection
  uint8_t connRole;            //!< Role connection was formed as, @ref GAP_Profile_Roles
  uint16_t connInterval;       //!< Connection Interval
  uint16_t connLatency;        //!< Connection Latency
  uint16_t connTimeout;        //!< Connection Timeout
  uint8_t clockAccuracy;       //!< Clock Accuracy
} gapEstLinkReqEvent_t;

/**
 * @ref GAP_LINK_PARAM_UPDATE_EVENT message format.
 *
 * This message is sent to the app
 * when the connection parameters update request is complete.
 */
typedef struct
{
  osal_event_hdr_t hdr;       //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;             //!< @ref GAP_LINK_PARAM_UPDATE_EVENT
  uint8_t status;             //!< status from link layer, defined in ll.h
  uint16_t connectionHandle;  //!< Connection handle of the update
  uint16_t connInterval;      //!< Requested connection interval
  uint16_t connLatency;       //!< Requested connection latency
  uint16_t connTimeout;       //!< Requested connection timeout
} gapLinkUpdateEvent_t;

/**
 * @ref GAP_LINK_TERMINATED_EVENT message format.
 *
 * This message is sent to the app when connection is terminated.
 */
typedef struct
{
  osal_event_hdr_t  hdr;     //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;            //!< @ref GAP_LINK_TERMINATED_EVENT
  uint16_t connectionHandle; //!< connection Handle
  uint8_t reason;            //!< termination reason from LL, defined in ll.h
} gapTerminateLinkEvent_t;

/**
 * @ref GAP_PASSKEY_NEEDED_EVENT message format.
 *
 * This message is sent to the app when a Passkey is needed from the
 * app's user interface.
 *
 * This event will be consumed by the gapbondmgr if it exists.
 */
typedef struct
{
  osal_event_hdr_t  hdr;          //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;                 //!< @ref GAP_PASSKEY_NEEDED_EVENT
  uint8_t deviceAddr[B_ADDR_LEN]; //!< address of device to pair with, and could be either public or random.
  uint16_t connectionHandle;      //!< Connection handle
  uint8_t uiInputs;               //!< Pairing User Interface Inputs - Ask user to input passcode
  uint8_t uiOutputs;              //!< Pairing User Interface Outputs - Display passcode
  uint32_t numComparison;         //!< Numeric Comparison value to be displayed.
} gapPasskeyNeededEvent_t;

/**
 * @ref GAP_AUTHENTICATION_COMPLETE_EVENT message format.
 *
 * This message is sent to the app when the authentication request is complete.
 *
 * This event will be consumed by the gapbondmgr if it exists.
 */
typedef struct
{
  osal_event_hdr_t  hdr;           //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;                  //!< @ref GAP_AUTHENTICATION_COMPLETE_EVENT
  uint16_t connectionHandle;       //!< Connection Handle from controller used to ref the device
  uint8_t authState;               //!< TRUE if the pairing was authenticated (MITM)
  smSecurityInfo_t *pSecurityInfo; //!< security information from this device
  smSigningInfo_t *pSigningInfo;   //!< Signing information
  smSecurityInfo_t *pDevSecInfo;   //!< security information from connected device
  smIdentityInfo_t *pIdentityInfo; //!< identity information
} gapAuthCompleteEvent_t;

/**
 * Authentication Parameters for @ref GAP_Authenticate which should only be used
 * if the gapbondmgr does not exist
 */
typedef struct
{
  uint16_t connectionHandle;    //!< Connection Handle from controller,
  smLinkSecurityReq_t  secReqs; //!< Pairing Control info
} gapAuthParams_t;

/**
 * @ref GAP_SLAVE_REQUESTED_SECURITY_EVENT message format.
 *
 * This message is sent to the app when a Slave Security Request is received.
 *
 * This event will be consumed by the gapbondmgr if it exists.
 */
typedef struct
{
  osal_event_hdr_t  hdr;          //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;                 //!< @ref GAP_SLAVE_REQUESTED_SECURITY_EVENT
  uint16_t connectionHandle;      //!< Connection Handle
  uint8_t deviceAddr[B_ADDR_LEN]; //!< address of device requesting security
  /**
   *  Authentication Requirements
   *
   *  Bit 2: MITM, Bits 0-1: bonding (0 - no bonding, 1 - bonding)
   */
  uint8_t authReq;
} gapSlaveSecurityReqEvent_t;

/**
 * @ref GAP_BOND_COMPLETE_EVENT message format.
 *
 * This message is sent to the app when a bonding is complete.  This means that
 * a key is loaded and the link is encrypted.
 *
 * This event will be consumed by the gapbondmgr if it exists.
 */
typedef struct
{
  osal_event_hdr_t  hdr;     //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;            //!< @ref GAP_BOND_COMPLETE_EVENT
  uint16_t connectionHandle; //!< connection Handle
} gapBondCompleteEvent_t;

/**
 * Pairing Request fields for @ref GAP_Authenticate which should only be used
 * if the gapbondmgr does not exist
 */
typedef struct
{
  uint8_t ioCap;         //!< Pairing Request ioCap field
  uint8_t oobDataFlag;   //!< Pairing Request OOB Data Flag field
  uint8_t authReq;       //!< Pairing Request Auth Req field
  uint8_t maxEncKeySize; //!< Pairing Request Maximum Encryption Key Size field
  keyDist_t keyDist;     //!< Pairing Request Key Distribution field
} gapPairingReq_t;

/**
 * @ref GAP_PAIRING_REQ_EVENT message format.
 *
 * This message is sent when an unexpected Pairing Request is
 * received and pairing must be initiated with @ref GAP_Authenticate using
 * the pairReq field received here
 *
 * @note This message should only be sent to peripheral devices.
 *
 * This event will be consumed by the gapbondmgr if it exists.
 */
typedef struct
{
  osal_event_hdr_t hdr;      //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;            //!< @ref GAP_PAIRING_REQ_EVENT
  uint16_t connectionHandle; //!< connection Handle
  gapPairingReq_t pairReq;   //!< The Pairing Request fields received.
} gapPairingReqEvent_t;

/**
 * Report describing connection event Returned via a @ref pfnGapConnEvtCB_t.
 */
typedef struct
{
  GAP_ConnEvtStat_t     status;   //!< status of connection event
  uint16_t              handle;   //!< connection handle
  uint8_t               channel;  //!< BLE RF channel index (0-39)
  GAP_ConnEvtPhy_t      phy;      //!< PHY of connection event
  int8_t                lastRssi; //!< RSSI of last packet received
  /// Number of packets received for this connection event
  uint16_t              packets;
  /// Total number of CRC errors for the entire connection
  uint16_t              errors;
  /// Type of next BLE task
  GAP_ConnEvtTaskType_t nextTaskType;
  /// Time to next BLE task (in us). 0xFFFFFFFF if there is no next task.
  uint32_t              nextTaskTime;
} Gap_ConnEventRpt_t;

/**
 * @ref GAP_BOND_LOST_EVENT message format.
 *
 * This message is sent to the app as indication that the bond has been removed on peer.
 *
 * This event will be consumed by the gapbondmgr if it exists.
 */
typedef struct
{
  osal_event_hdr_t  hdr;          //!< @ref GAP_MSG_EVENT and status
  uint8_t opcode;                 //!< @ref GAP_BOND_LOST_EVENT
  uint16_t connectionHandle;      //!< Connection Handle
  uint8_t deviceAddr[B_ADDR_LEN]; //!< address of device requesting pairing
} gapBondLostEvent_t;

/** @} End GAP_Structs */

/*-------------------------------------------------------------------
 * CALLBACKS
 */

/**
 * @defgroup GAP_CBs GAP Callbacks
 * @{
 */

/// Central Address Resolution (CAR) Support Callback Function
typedef uint8_t(*pfnSuppCentAddrRes_t)
(
  uint8_t *deviceAddr,           //!< address of device to check for CAR
  GAP_Peer_Addr_Types_t addrType //!< peer device's address type
);

//TODO try to move these to gap_internal.h

/// GAP Idle Callback Function
typedef void (*pfnGapIdleCB_t)();

/// GAP Device Privacy Mode Callback Function
typedef uint8_t(*pfnGapDevPrivModeCB_t)
(
  GAP_Peer_Addr_Types_t  addrType,    //!< address type of device to check
  uint8_t                *pAddr       //!< address of device to check if Device Privacy mode is permissible
);

/// Callback Registration Structure
typedef struct
{
  pfnSuppCentAddrRes_t   suppCentAddrResCB;  //!< Supports Central Address Resolution
  pfnGapIdleCB_t         gapIdleCB;          //!< GAP Idle callback
  pfnGapDevPrivModeCB_t  gapDevPrivModeCB;   //!< GAP Device Privacy Mode callback
} gapBondMgrCBs_t;

/**
 * GAP Callback function pointer type for the advertising and scan modules.
 *
 * This callback will return @ref GapAdvScan_Event_IDs from the advertising module,
 * some of which can be masked with @ref GapAdv_eventMaskFlags_t. See the
 * respective event in @ref GapAdvScan_Event_IDs for the type that pBuf should be
 * cast to.
 *
 * This callback will also return @ref GapAdvScan_Event_IDs from the advertising
 * module. See the respective event in @ref GapAdvScan_Event_IDs for the type that
 * pBuf should be cast to.
 */
typedef void (*pfnGapCB_t)
(
  uint32_t event,   //!< see @ref GapAdvScan_Event_IDs and GapAdvScan_Event_IDs
  void *pBuf,       //!< data potentially accompanying event
  uintptr_t arg     //!< custom application argument that can be return through this callback
);

/**
 * GAP Callback function pointer type for Connection Event notifications.
 *
 * When registered via @ref Gap_RegisterConnEventCb, this callback will return
 * a pointer to a @ref Gap_ConnEventRpt_t from the controller after each
 * connection event
 *
 * @warning The application owns the memory pointed to by pReport. That is, it
 * is responsible for freeing this memory.
 *
 * @warning This is called from the stack task context. Therefore, processing
 * in this callback should be minimized. Any excessive processing should be
 * done by posting an event to the application task context.
 */
typedef void (*pfnGapConnEvtCB_t)
(
  /// Pointer to report describing the connection event
  Gap_ConnEventRpt_t *pReport
);

/** @} End GAP_CBs */

/*-------------------------------------------------------------------
 * FUNCTIONS - Initialization and Configuration
 */

/**
 * GAP Device Initialization
 *
 * Setup the device. Can only be called once per reset. In order to change
 * the address mode / random address, it is necessary to reset the device
 * and call this API again. In the case where the address mode or random address
 * is different than it was for the last initialization, all bonds and local
 * information stored in NV will be erased.
 *
 * @par Corresponding Events:
 * @ref GAP_DEVICE_INIT_DONE_EVENT of type @ref gapDeviceInitDoneEvent_t
 *
 * @param profileRole GAP Profile Roles: @ref GAP_Profile_Roles
 * @param taskID end application task to receive unprocessed GAP events.
 * @param addrMode Own address mode. If always using Resolvable Private Address,
 *        set this to either @ref ADDRMODE_RP_WITH_PUBLIC_ID or
 *        @ref ADDRMODE_RP_WITH_RANDOM_ID. If always using Identity
 *        Address, set this to either @ref ADDRMODE_PUBLIC or
 *        @ref ADDRMODE_RANDOM.
 * @param pRandomAddr Pointer to 6-byte Random Static Address of this device
 *        that will be copied to the stack. Valid only if addrMode
 *        is @ref ADDRMODE_RANDOM or @ref ADDRMODE_RP_WITH_RANDOM_ID and can
 *        not be NULL in these cases. Ignored for other address types.
 *
 * @return @ref SUCCESS : initialization started
 * @return @ref INVALIDPARAMETER : invalid profile role, role combination,
 *         or invalid Random Static Address,
 * @return @ref bleIncorrectMode : initialization has already occurred
 * @return @ref bleInternalError : error erasing NV
 */
extern bStatus_t GAP_DeviceInit(uint8_t profileRole, uint8_t taskID,
                                GAP_Addr_Modes_t addrMode,
                                uint8_t* pRandomAddr);

/**
 * GAP Device Initialization with no GAP Bond Manager
 *
 * Setup the device. Can only be called once per reset, in case GAPBondMgr is not enabled.
 * In order to change the address mode / random address, it is necessary to reset the device
 * and call this API again. In the case where the address mode or random address
 * is different than it was for the last initialization, all local information
 * stored in NV will be erased.
 *
 * @par Corresponding Events:
 * @ref GAP_DEVICE_INIT_DONE_EVENT of type @ref gapDeviceInitDoneEvent_t
 *
 * @param profileRole GAP Profile Roles: @ref GAP_Profile_Roles
 * @param taskID end application task to receive unprocessed GAP events.
 * @param addrMode Own address mode. If always using Resolvable Private Address,
 *        set this to either @ref ADDRMODE_RP_WITH_PUBLIC_ID or
 *        @ref ADDRMODE_RP_WITH_RANDOM_ID. If always using Identity
 *        Address, set this to either @ref ADDRMODE_PUBLIC or
 *        @ref ADDRMODE_RANDOM.
 * @param pRandomAddr Pointer to 6-byte Random Static Address of this device
 *        that will be copied to the stack. Valid only if addrMode
 *        is @ref ADDRMODE_RANDOM or @ref ADDRMODE_RP_WITH_RANDOM_ID and can
 *        not be NULL in these cases. Ignored for other address types.
 *
 * @return @ref SUCCESS : initialization started
 * @return @ref INVALIDPARAMETER : invalid profile role, role combination,
 *         or invalid Random Static Address,
 * @return @ref bleIncorrectMode : initialization has already occurred
 */

extern bStatus_t GAP_DeviceInit_noGAPBondMgr(uint8_t profileRole, uint8_t taskID,
                                GAP_Addr_Modes_t addrMode,
                                uint8_t* pRandomAddr);

/**
 * Set a GAP Parameter value
 *
 * Use this function to change the default GAP parameter values.
 *
 * @param paramID parameter ID: @ref Gap_ParamIDs_t
 * @param paramValue new param value
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 */
extern bStatus_t GAP_SetParamValue(uint16_t paramID, uint16_t paramValue);

/**
 * Get a GAP Parameter value.
 *
 * @param paramID parameter ID: @ref Gap_ParamIDs_t
 *
 * @return GAP Parameter value
 * @return 0xFFFF if invalid
 */
extern uint16_t GAP_GetParamValue(uint16_t paramID);

/**
 * Register for GAP Messages
 *
 * Register a task ID to receive extra (unprocessed) HCI status and complete,
 * and Host events.
 *
 * @param taskID Default task ID to send events.
 */
extern void GAP_RegisterForMsgs(uint8_t taskID);

/**
 * Register bond manager call backs
 *
 * @param pCBs pointer to Bond Manager Callbacks.
 */
extern void GAP_RegisterBondMgrCBs(gapBondMgrCBs_t *pCBs);

/**
 * Set the Privacy Mode of the requested device to the
 *              desired mode.
 *
 * @warning This should not be called if the bond manager is present as the
 * bond manager will handle this automatically based on the connected device's
 * capabilities and the own device's capabilities.
 *
 * @param addrType remote device's identity address type
 * @param pAddr pointer to the address
 * @param mode @ref GAP_PRIVACY_MODES to use for this device
 *
 * @return @ref SUCCESS : match
 * @return @ref FAILURE : don't match
 * @return @ref INVALIDPARAMETER : parameters invalid
 */
extern bStatus_t GAP_SetPrivacyMode(GAP_Peer_Addr_Types_t addrType,
                                    uint8_t *pAddr, uint8_t mode);

/*-------------------------------------------------------------------
 * FUNCTIONS - Link Establishment
 */

/**
 * Terminate a link connection.
 *
 * @par Corresponding Events:
 * @ref GAP_LINK_TERMINATED_EVENT of type @ref gapTerminateLinkEvent_t
 *
 * @param connectionHandle connection handle of link to terminate
 *        or @ref LINKDB_CONNHANDLE_ALL
 * @param reason terminate reason.
 *
 * @return @ref SUCCESS : termination request sent to stack
 * @return @ref bleIncorrectMode : No Link to terminate
 * @return @ref bleInvalidTaskID : not app that established link
 */
extern bStatus_t GAP_TerminateLinkReq(uint16_t connectionHandle, uint8_t reason);

/**
 * Update the link parameters to a Master or Slave device.
 *
 * As long as LL connection updates are supported on the own device (which is
 * the case by default), an LL Connection Update procedure will be attempted.
 * If this fails, the stack will automatically attempt an L2CAP parameter update
 * request.
 *
 * @par Corresponding Events:
 * After the update procedure is complete, the calling task will receive a
 * @ref GAP_LINK_PARAM_UPDATE_EVENT of type @ref gapLinkUpdateEvent_t regardless
 * of the connection parameter update procedure that occurred.
 *
 * @param pParams link update parameters
 *
 * @return @ref SUCCESS : update request sent to stack
 * @return @ref INVALIDPARAMETER : one of the parameters were invalid
 * @return @ref bleIncorrectMode : invalid profile role
 * @return @ref bleAlreadyInRequestedMode : already updating link parameters
 * @return @ref bleNotConnected : not in a connection
 */
extern bStatus_t GAP_UpdateLinkParamReq(gapUpdateLinkParamReq_t *pParams);

/**
 * Reply to a Connection Parameter Update Request that was received from a
 * remote device.
 *
 * This API should be used in response to a @ref GAP_UPDATE_LINK_PARAM_REQ_EVENT
 * which will only be received when @ref GAP_PARAM_LINK_UPDATE_DECISION is set
 * to @ref GAP_UPDATE_REQ_PASS_TO_APP.
 *
 * @par Corresponding Events:
 * The calling task should call this API in response to a
 * @ref GAP_UPDATE_LINK_PARAM_REQ_EVENT
 * of type @ref gapUpdateLinkParamReqEvent_t <br>
 * After the update procedure is complete, the calling task will receive a
 * @ref GAP_LINK_PARAM_UPDATE_EVENT of type @ref gapLinkUpdateEvent_t
 *
 * @param pParams local device's desired connection parameters.
 *
 * @return @ref SUCCESS : reply sent successfully
 * @return @ref INVALIDPARAMETER : one of the parameters were invalid
 * @return @ref bleIncorrectMode : invalid profile role
 * @return @ref bleAlreadyInRequestedMode : already updating link parameters
 * @return @ref bleNotConnected : not in a connection
 */
extern bStatus_t GAP_UpdateLinkParamReqReply(gapUpdateLinkParamReqReply_t
   *pParams);

/**
 * Returns the number of active connections.
 *
 * @return Number of active connections.
 */
extern uint8_t GAP_NumActiveConnections(void);

/**
* Register/Unregister a connection event callback
*
* It is only possible to register for one connection handle of for all
* connection handles. In the case of unregistering, it does not matter what
* connHandle or cb is passed in as whatever is currently registered will be
* unregistered.
*
* @warning The application owns the memory pointed to by pReport in
* @ref pfnGapConnEvtCB_t. That is, it is responsible for freeing this memory.
*
* @note The callback needs to be registered for each reconnection. It is not
* retained across a disconnect / reconnect.
*
* @param cb Function pointer to a callback.
* @param action Register or unregister the callback.
* @param connHandle if @ref LINKDB_CONNHANDLE_ALL, apply to all connections. <br>
*        else, apply only for a specific connection.
*
* @return @ref SUCCESS
* @return @ref bleGAPNotFound : connection handle not found
* @return @ref bleInvalidRange : the callback function was NULL or action is
*         invalid
* @return @ref bleMemAllocError : there is not enough memory to register the callback.
*/
extern bStatus_t Gap_RegisterConnEventCb(pfnGapConnEvtCB_t cb,
                                         GAP_CB_Action_t action,
                                         uint16_t connHandle);

/*-------------------------------------------------------------------
 * FUNCTIONS - GAP Configuration
 */

/**
 * Set a GAP Configuration Parameter
 *
 * Use this function to write a GAP configuration parameter. These parameters
 * must be set before @ref GAP_DeviceInit
 *
 * @param param parameter ID: @ref Gap_configParamIds_t
 * @param pValue pointer to parameter value. Cast based on the type defined in
 *        @ref Gap_configParamIds_t
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 * @return @ref bleInvalidRange NULL pointer was passed
 * @return @ref bleIncorrectMode Device is already initialized
 */
extern bStatus_t GapConfig_SetParameter(Gap_configParamIds_t param,
                                        void *pValue);

/**
 * Get the address of this device
 *
 * @param wantIA TRUE for Identity Address. FALSE for Resolvable Private
 *        Address (if the device has been initialized with the address mode
 *        @ref ADDRMODE_RP_WITH_PUBLIC_ID or @ref ADDRMODE_RP_WITH_RANDOM_ID)
 *
 * @return pointer to device address.
 */
extern uint8_t *GAP_GetDevAddress(uint8 wantIA);

/**
 * Get the IRK
 *
 * @warning The memory indicated by the returned pointer should not be modified
 * directly if it is desired to update the IRK. Instead, use the
 * @ref GAP_CONFIG_PARAM_IRK
 *
 * @return  pointer to the 16-byte IRK
 */
extern uint8_t *GAP_GetIRK(void);

/**
 * Get the SRK
 *
 * @warning The memory indicated by the returned pointer should not be modified
 * directly if it is desired to update the SRK. Instead, use the
 * @ref GAP_CONFIG_PARAM_SRK
 *
 * @return pointer to the 16-byte SRK
 */
extern uint8_t *GAP_GetSRK(void);

/*-------------------------------------------------------------------
 * FUNCTIONS - Pairing
 */

/**
 *  Start Authentication
 *
 * Start the Authentication process with the requested device.
 * This function is used to Initiate/Allow pairing.
 * Called by both master and slave device (Central and Peripheral).
 *
 * @warning This API should not be called by the application if the
 * gapbondmgr exists as it will be used automatically based on
 * @ref GAPBOND_PAIRING_MODE
 *
 * @par Corresponding Events:
 * After pairing is completed successfully, the calling task will receive a
 * @ref GAP_AUTHENTICATION_COMPLETE_EVENT of type
 * @ref gapAuthCompleteEvent_t <br>
 * After a failed pairing, the calling task will receive a
 * @ref GAP_AUTHENTICATION_FAILURE_EVT
 *
 * @param  pParams Authentication parameters
 * @param  pPairReq Enter these parameters if the Pairing Request was already
 *         received. <br>
 *         NULL, if waiting for Pairing Request or if initiating.
 *
 * @return @ref SUCCESS : authentication request sent to stack
 * @return @ref bleIncorrectMode : Not correct profile role
 * @return @ref INVALIDPARAMETER
 * @return @ref bleNotConnected
 * @return @ref bleAlreadyInRequestedMode
 * @return @ref FAILURE : not workable
 */
extern bStatus_t GAP_Authenticate(gapAuthParams_t *pParams,
                                  gapPairingReq_t *pPairReq);

/**
 * Check if GAP is currently pairing.
 *
 * @note This API is not needed if the gapbondmgr exists as pairing states are
 * returned via the @ref pfnPairStateCB_t callback
 *
 * @return TRUE if pairing
 * @return FALSE otherwise
 */
extern uint8_t GAP_isPairing(void);

/**
 * Terminate Authentication
 *
 * Send a Pairing Failed message and end any existing pairing.
 *
 * @par Corresponding Events:
 * @ref GAP_AUTHENTICATION_FAILURE_EVT
 *
 * @param  connectionHandle connection handle.
 * @param  reason Pairing Failed reason code.
 *
 * @return @ref SUCCESS : function was successful
 * @return @ref bleMemAllocError : memory allocation error
 * @return @ref INVALIDPARAMETER : one of the parameters were invalid
 * @return @ref bleNotConnected : link not found
 * @return @ref bleInvalidRange : one of the parameters were not within range
 */
extern bStatus_t GAP_TerminateAuth(uint16_t connectionHandle, uint8_t reason);

/**
 * Update the passkey in string format.
 *
 * This API should be called in response to receiving a
 * @ref GAP_PASSKEY_NEEDED_EVENT
 *
 * @note This function is the same as @ref GAP_PasscodeUpdate, except that
 * the passkey is passed in as a string format.
 *
 * @warning This API should not be called by the application if the
 * gapbondmgr exists as it is abstracted through @ref GAPBondMgr_PasscodeRsp
 *
 * @param pPasskey new passkey - pointer to numeric string (ie. "019655" )
 *        This string's range is "000000" to "999999"
 * @param connectionHandle connection handle.
 *
 * @return @ref SUCCESS : will start pairing with this entry
 * @return @ref bleIncorrectMode : Link not found
 * @return @ref INVALIDPARAMETER : passkey == NULL or passkey isn't formatted
 * properly
 */
extern bStatus_t GAP_PasskeyUpdate(uint8_t *pPasskey, uint16_t connectionHandle);

/**
 * Update the passkey in a numeric value (not string).
 *
 * This API should be called in response to receiving a
 * @ref GAP_PASSKEY_NEEDED_EVENT
 *
 * @note This function is the same as @ref GAP_PasskeyUpdate, except that
 * the passkey is passed in as a non-string format.
 *
 * @param passcode not string - range: 0 - 999,999.
 * @param connectionHandle connection handle.
 *
 * @return @ref SUCCESS : will start pairing with this entry
 * @return @ref bleIncorrectMode : Link not found
 * @return @ref INVALIDPARAMETER : passkey == NULL or passkey isn't formatted
 * properly
 */
extern bStatus_t GAP_PasscodeUpdate(uint32_t passcode, uint16_t connectionHandle);

/**
 * Generate a Slave Requested Security message to the master.
 *
 * @warning This API should not be called by the application if the
 * gapbondmgr exists as it will be used automatically based on
 * @ref GAPBOND_PAIRING_MODE and the GAP role of the device
 *
 * @param connectionHandle connection handle.
 * @param authReq Authentication Requirements: Bit 2: MITM,
 *        Bits 0-1: bonding (0 - no bonding, 1 - bonding)
 *        Bit 3: Secure Connections
 *
 * @return @ref SUCCESS : will send
 * @return @ref bleNotConnected : Link not found
 * @return @ref bleIncorrectMode : wrong GAP role, must be a Peripheral Role
 */
extern bStatus_t GAP_SendSlaveSecurityRequest(uint16_t connectionHandle,
                                              uint8_t authReq);

/**
 * Set up the connection to accept signed data.
 *
 * @warning This API should not be called by the application if the
 * gapbondmgr exists as it will be used automatically when signing occurs
 *
 * @param connectionHandle connection handle of the signing information
 * @param authenticated TRUE if the signing information is authenticated,
 *        FALSE otherwise
 * @param pParams signing parameters
 *
 * @return @ref SUCCESS
 * @return @ref bleIncorrectMode : Not correct profile role
 * @return @ref INVALIDPARAMETER
 * @return @ref bleNotConnected
 * @return @ref FAILURE : not workable
 */
extern bStatus_t GAP_Signable(uint16_t connectionHandle, uint8_t authenticated,
                              smSigningInfo_t *pParams);

/**
 * Set up the connection's bound parameters.
 *
 * @warning This API should not be called by the application if the
 * gapbondmgr exists as it will be used automatically when a connection is
 * formed to a previously bonded device
 *
 * @par Corresponding Events:
 * @ref GAP_BOND_COMPLETE_EVENT of type @ref gapBondCompleteEvent_t
 *
 * @param connectionHandle connection handle of the signing information
 * @param authenticated TRUE if bond is authenticated.
 * @param secureConnections TRUE if bond has Secure Connections strength.
 * @param pParams the connected device's security parameters
 * @param startEncryption whether or not to start encryption
 *
 * @return @ref SUCCESS
 * @return @ref bleIncorrectMode : Not correct profile role
 * @return @ref INVALIDPARAMETER
 * @return @ref bleNotConnected
 * @return @ref FAILURE : not workable
 */
extern bStatus_t GAP_Bond(uint16_t connectionHandle, uint8_t authenticated,
                          uint8_t secureConnections, smSecurityInfo_t *pParams,
                          uint8_t startEncryption);

/*-------------------------------------------------------------------
 * TASK FUNCTIONS - To only be used in osal_icall_ble.c
 */

/**
 * Initialization for GAP module
 *
 * This should only be used in osal_icall_ble.c in osalInitTasks()
 *
 * @param task_id OSAL task ID
 */
extern void GAP_Init(uint8_t task_id);

/**
 * GAP Module event processing
 *
 * This should only be used in osal_icall_ble.c in tasksArr[]
 *
 * @param  task_id OSAL task ID
 * @param  events  OSAL event received
 *
 * @return bitmask of events that weren't processed
 */
extern uint16_t GAP_ProcessEvent(uint8_t task_id, uint16_t events);
/// @cond NODOC
/**
 * This API is used to disable the RF
 *
 * @param  none
 *
 * @return none
 */
void GAP_DeInit( void );

/**
 * This API is used to re-enable the RF
 *
 * @param  none
 *
 * @return none
 */
void GAP_ReInit( void );
/// @endcond // NODOC


/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* GAP_H */

/** @} End GAP */
