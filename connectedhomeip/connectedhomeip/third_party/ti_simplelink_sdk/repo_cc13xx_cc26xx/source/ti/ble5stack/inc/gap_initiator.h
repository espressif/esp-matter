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
 *  @defgroup GapInit Gap Initiator
 *  @brief This module implements the Host Initiator
 *  @{
 *  @file  gap_initiator.h
 *  @brief      GAP Initiator layer interface
 */

#ifndef GAP_INITIATOR_H
#define GAP_INITIATOR_H

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
 * CONSTANTS
 */

/**
 * @defgroup GapInit_Constants GapInit Constants
 *
 * Other defines used in the GapInit module
 * @{
 */

/// GAP Initiator Initiating PHYs
enum GapInit_InitPhy_t
{
  INIT_PHY_1M    = LL_PHY_1_MBPS,   //!< 1Mbps PHY. Bit 0
  INIT_PHY_2M    = LL_PHY_2_MBPS,   //!< 2Mbps PHY. Bit 1
  INIT_PHY_CODED = LL_PHY_CODED     //!< Coded PHY. Bit 2
};

/**
 * @defgroup GAPInit_PHY_Param_defaults GAPInit PHY Parameter Default Value
 * @{
 */
#define INIT_PHYPARAM_DFLT_SCAN_INTERVAL   16     //!< Default scan interval (in 625us)
#define INIT_PHYPARAM_DFLT_SCAN_WINDOW     16     //!< Default scan interval (in 625us)
#define INIT_PHYPARAM_DFLT_CONN_INT_MIN    80     //!< Default minimum connection interval (in 1.25ms)
#define INIT_PHYPARAM_DFLT_CONN_INT_MAX    80     //!< Default maximum connection interval (in 1.25ms)
#define INIT_PHYPARAM_DFLT_CONN_LATENCY    0      //!< Default connection latency
#define INIT_PHYPARAM_DFLT_SUP_TIMEOUT     2000   //!< Default supervision timeout (in 10ms)
// TODO: currently minimum/maximum CE lengths are ignored
#define INIT_PHYPARAM_DFLT_MIN_CE_LEN      0      //!< Default minimum connection event length (currently ignored)
#define INIT_PHYPARAM_DFLT_MAX_CE_LEN      0xFFFF //!< Default maximum connection event length (currently ignored)
/** @} End GAPInit_PHY_Param_defaults */

/** @} End GapInit_Constants */

/**
 * @defgroup GapInit_Params GapInit Params
 *
 * Params used in the GapInit module
 * @{
 */

/**
 * GAP Initiator PHY Parameters
 *
 * These can be set with @ref GapInit_setPhyParam and read with
 * @ref GapInit_getPhyParam. The default values below refer to the values
 * that are set at initialization.
 */
typedef enum
{
  /**
   * Scan Interval
   *
   * default: @ref INIT_PHYPARAM_DFLT_SCAN_INTERVAL
   *
   * range: 4 - 16384
   */
  INIT_PHYPARAM_SCAN_INTERVAL,

  /**
   * Scan Window
   *
   * default: @ref INIT_PHYPARAM_DFLT_SCAN_INTERVAL
   *
   * range: 4 - 16384
   */
  INIT_PHYPARAM_SCAN_WINDOW,

  /**
   * Minimum Connection Interval
   *
   * @note This should be equal to or smaller than @ref INIT_PHYPARAM_CONN_INT_MAX
   *
   * default: @ref INIT_PHYPARAM_DFLT_CONN_INT_MIN
   *
   * range: 6 - 3200
   */
  INIT_PHYPARAM_CONN_INT_MIN,

  /**
   * Maximum Connection Interval
   *
   * @note This should be equal to or greater than @ref INIT_PHYPARAM_CONN_INT_MIN
   *
   * default: @ref INIT_PHYPARAM_DFLT_CONN_INT_MAX
   *
   * range: 6 - 3200
   */
  INIT_PHYPARAM_CONN_INT_MAX,

  /**
   * Slave Latency
   *
   * default: @ref INIT_PHYPARAM_DFLT_CONN_LATENCY
   *
   * range: 0 - 499
   */
  INIT_PHYPARAM_CONN_LATENCY,

  /**
   * Supervision Timeout
   *
   * default: @ref INIT_PHYPARAM_DFLT_SUP_TIMEOUT
   *
   * range: 10 - 3200
   */
  INIT_PHYPARAM_SUP_TIMEOUT,

  /**
   * Minimum Length of Connection Event
   *
   * @warning This is not used by the controller
   *
   * default: @ref INIT_PHYPARAM_DFLT_MIN_CE_LEN
   *
   * range: 0 - 0xFFFF
   */
  INIT_PHYPARAM_MIN_CE_LEN,

  /**
   * Maximum Length of Connection Event
   *
   * @note This should be equal to or greater than @ref INIT_PHYPARAM_DFLT_MIN_CE_LEN
   *
   * @warning This is not used by the controller
   *
   * default: @ref INIT_PHYPARAM_DFLT_MAX_CE_LEN
   *
   * range: 0 - 0xFFFF
   */
  INIT_PHYPARAM_MAX_CE_LEN,

/// @cond NODOC
  INIT_NUM_PHYPARAM,
/// @endcond // NODOC
} GapInit_PhyParamId_t;
/** @} End GapInit_Params */

/*-------------------------------------------------------------------
 * Structures
 */

/**
 * @defgroup GapInit_Structs GapInit Structures
 *
 * Data structures used in the GapInit module
 * @{
 */

/**
 * @ref GAP_CONNECTING_CANCELLED_EVENT message format.
 *
 * This message is sent to the app
 * when the CreateConnection request is cancelled.
 */
typedef struct
{
  osal_event_hdr_t  hdr;           //!< @ref GAP_MSG_EVENT and status
  uint8 opcode;                    //!< @ref GAP_CONNECTING_CANCELLED_EVENT
} gapConnCancelledEvent_t;

/** @} End GapInit_Structs */

/*-------------------------------------------------------------------
 * API's
 */

/**
 * Set parameters dependent on PHY.
 *
 * @note that if phys contains more than one PHY, the same parameter of those
 * PHYs will be set with the same value.
 *
 * @param phys connection channel PHY(s): Individual PHY values of
 *        @ref GapInit_InitPhy_t can be OR'ed.
 * @param paramId parameter ID
 * @param value parameter value
 *
 * @return @ref SUCCESS
 * @return @ref bleInvalidRange
 */
status_t GapInit_setPhyParam(uint8_t phys, GapInit_PhyParamId_t paramId,
                             uint16_t value);

/**
 * Get parameters of the specified PHY.
 *
 * @note Phy shall indicate only one PHY.
 *
 * @param phy connection channel PHY: shall be one from @ref GapInit_InitPhy_t.
 * @param paramId parameter ID
 * @param pValue pointer to where the parameter value is to be stored
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 * @return @ref bleInvalidRange
 */
status_t GapInit_getPhyParam(uint8_t phy, GapInit_PhyParamId_t paramId,
                             uint16_t* pValue);

/**
 * Initiate connection with the specified peer device
 *
 * @param peerAddrType peer device's address type.
 * @param pPeerAddress peer device's address
 * @param phys PHY to try making connection on: shall be one from
 *        @ref GapInit_InitPhy_t.
 * @param timeout If there is no chance to initiate a connection within timeout
 *        ms, this connect request will be canceled automatically. if timeout is
 *        0, the initiator will keep trying to get a chance to make a connection
 *        until @ref GapInit_cancelConnect is called.
 *
 * @return @ref SUCCESS
 * @return @ref bleNotReady
 * @return @ref bleInvalidRange
 * @return @ref bleMemAllocError
 * @return @ref bleAlreadyInRequestedMode
 */
status_t GapInit_connect(GAP_Peer_Addr_Types_t peerAddrType,
                         uint8_t* pPeerAddress, uint8_t phys, uint16_t timeout);

/**
 * @param phys PHY to try making connection on: shall be one from
 *        @ref GapInit_InitPhy_t.
 * @param timeout If there is no chance to initiate a connection within timeout
 *        ms, this connect request will be canceled automatically. if timeout is
 *        0, the initiator will keep trying to get a chance to make a connection
 *        until @ref GapInit_cancelConnect is called.
 *
 * @return @ref SUCCESS
 * @return @ref bleNotReady
 * @return @ref bleInvalidRange
 * @return @ref bleMemAllocError
 * @return @ref bleAlreadyInRequestedMode
 */
status_t GapInit_connectWl(uint8_t phys, uint16_t timeout);

/**
 * Cancel the ongoing connection process.
 *
 * @return SUCCESS
 * @return bleNotReady
 * @return bleIncorrectMode
 */
status_t GapInit_cancelConnect(void);

/// @endcond // NODOC

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* GAP_INITIATOR_H */

/** @} End GapInit */

