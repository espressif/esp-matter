/******************************************************************************

 @file  ull.h

 @brief This file contains the Micro Link Layer (uLL) API for the Micro
        BLE Stack.

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

#ifndef ULL_H
#define ULL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
* INCLUDES
*/

/*-------------------------------------------------------------------
 * CONSTANTS
 */

/**
 * Link layer Advertiser Events
 */
#define ULL_EVT_ADV_TX_NO_RADIO_RESOURCE    1 //!< Adv event no radio resource
#define ULL_EVT_ADV_TX_FAILED               2 //!< Adv event tx failed
#define ULL_EVT_ADV_TX_SUCCESS              3 //!< Adv event tx success
#define ULL_EVT_ADV_TX_RADIO_AVAILABLE      4 //!< Adv event radio available
#define ULL_EVT_ADV_TX_TIMER_EXPIRED        5 //!< Adv event interval timer expired
#define ULL_EVT_ADV_TX_STARTED              6 //!< Adv event tx started

/**
 * Link layer Scanner Events
 */
#define ULL_EVT_SCAN_RX_NO_RADIO_RESOURCE   7 //!< Scan event rx no radio resource
#define ULL_EVT_SCAN_RX_FAILED              8 //!< Scan event rx failed
#define ULL_EVT_SCAN_RX_BUF_FULL            9 //!< Scan event rx buffer full
#define ULL_EVT_SCAN_RX_SUCCESS            10 //!< Scan event rx success
#define ULL_EVT_SCAN_RX_RADIO_AVAILABLE    11 //!< Scan event radio available
#define ULL_EVT_SCAN_RX_STARTED            12 //!< Scan event rx started

/**
 * Link layer Monitor Events
 */
#define ULL_EVT_MONITOR_RX_NO_RADIO_RESOURCE  13 //!< Monitor event rx no radio resource
#define ULL_EVT_MONITOR_RX_FAILED             14 //!< Monitor event rx failed
#define ULL_EVT_MONITOR_RX_BUF_FULL           15 //!< Monitor event rx buffer full
#define ULL_EVT_MONITOR_RX_SUCCESS            16 //!< Monitor event rx success
#define ULL_EVT_MONITOR_RX_WINDOW_COMPLETE    17 //!< Monitor event rx window complete
#define ULL_EVT_MONITOR_RX_RADIO_AVAILABLE    18 //!< Monitor event radio available
#define ULL_EVT_MONITOR_RX_STARTED            19 //!< Monitor event rx started

/**
 * Link layer Scanner number of rx entries
 */
#define ULL_NUM_RX_SCAN_ENTRIES             6 //!< Number of scan rx entries
#define ULL_PKT_HDR_LEN                     2 //!< Packet hearde length
#define ULL_MAX_BLE_ADV_PKT_SIZE           37 //!< Payload size

/**
 * Link layer Monitor number of rx entries
 */
#define ULL_NUM_RX_MONITOR_ENTRIES          6 //!< Number of monitoring scan rx entries
#define ULL_PKT_HDR_LEN                     2 //!< Packet header length
#define ULL_MAX_BLE_PKT_SIZE              255 //!< Payload size.


/**
 * Link layer Receive Suffix Data Sizes
 */
#define ULL_SUFFIX_CRC_SIZE             3
#define ULL_SUFFIX_RSSI_SIZE            1
#define ULL_SUFFIX_STATUS_SIZE          1
#define ULL_SUFFIX_TIMESTAMP_SIZE       4
#define ULL_SUFFIX_MAX_SIZE             (ULL_SUFFIX_CRC_SIZE    +              \
                                         ULL_SUFFIX_RSSI_SIZE   +              \
                                         ULL_SUFFIX_STATUS_SIZE +              \
                                         ULL_SUFFIX_TIMESTAMP_SIZE)

#define ULL_ADV_UNALIGNED_BUFFER_SIZE   (ULL_PKT_HDR_LEN + ULL_MAX_BLE_ADV_PKT_SIZE + ULL_SUFFIX_MAX_SIZE)
#define ULL_ADV_ALIGNED_BUFFER_SIZE     (ULL_ADV_UNALIGNED_BUFFER_SIZE + 4 - \
                                         (ULL_ADV_UNALIGNED_BUFFER_SIZE % 4))

#define ULL_BLE_UNALIGNED_BUFFER_SIZE   (ULL_PKT_HDR_LEN + ULL_MAX_BLE_PKT_SIZE + ULL_SUFFIX_MAX_SIZE)
#define ULL_BLE_ALIGNED_BUFFER_SIZE     (ULL_BLE_UNALIGNED_BUFFER_SIZE + 4 - \
                                         (ULL_BLE_UNALIGNED_BUFFER_SIZE % 4))

/**
 * Scanner White List Policy
 */
#define ULL_SCAN_WL_POLICY_ANY_ADV_PKTS                0
#define ULL_SCAN_WL_POLICY_USE_WHITE_LIST              1


/******************************************************************************
* TYPEDEFS
*/

#if defined(FEATURE_ADVERTISER)

/**
 * Callback of when an Adv event is about to start.
 */
typedef void (*pfnAdvAboutToCB_t)(void);

/**
 * Callback of when events come from RF driver regarding Adv command.
 */
typedef void (*pfnAdvDoneCB_t)(bStatus_t status);

#endif /* FEATURE_ADVERTISER */

#if defined(FEATURE_SCANNER)

/**
 * Callback of when a scan has received a packet.
 */
typedef void (*pfnScanIndCB_t)(bStatus_t status, uint8_t len, uint8_t *pPayload);

/**
 * Callback of when events when a scan wondow is complete.
 */
typedef void (*pfnScanWindowCompCB_t)(bStatus_t status);

/**
 * Flow control to allow one RX packet at a time
 */
extern bool Ull_advPktInuse;

#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)

/**
 * Callback of when a monitoring scan has received a packet.
 */
typedef void (*pfnMonitorIndCB_t)(bStatus_t status, uint8_t sessionId, uint8_t len, uint8_t *pPayload);

/**
 * Callback of when a monitor duration is complete.
 */
typedef void (*pfnMonitorCompCB_t)(bStatus_t status, uint8_t sessionId);

#endif /* FEATURE_MONITOR */


/*****************************************************************************
 * FUNCTION PROTOTYPES
 */

/*********************************************************************
 * @fn     ull_init
 *
 * @brief  Initialization function for the Micro Link Layer.
 *
 * @param  none
 *
 * @return SUCCESS or FAILURE
 */
bStatus_t ull_init(void);

#if defined(FEATURE_ADVERTISER)
/*********************************************************************
 * @fn     ull_advRegisterCB
 *
 * @brief  Register callbacks supposed to be called by Advertiser.
 *
 * @param  pfnAdvAboutToCB - callback to nofity the application of time to
 *                           update the advertising packet payload.
 * @param  pfnAdvDoneCB    - callback to post-process Advertising Event
 *
 * @return  none
 */
void ull_advRegisterCB(pfnAdvAboutToCB_t pfnAdvAboutToCB,
                       pfnAdvDoneCB_t pfnAdvDoneCB);

/*********************************************************************
 * @fn      ull_advStart
 *
 * @brief   Enter ULL_STATE_ADVERTISING
 *
 * @param   none
 *
 * @return  SUCCESS - Successfully entered ULL_STATE_ADVERTISING
 *          FAILURE - Failed to enter ULL_STATE_ADVERSISING
 */
bStatus_t ull_advStart(void);

/*********************************************************************
 * @fn      ull_advStop
 *
 * @brief   Exit ULL_STATE_ADVERTISING
 *
 * @param   none
 *
 * @return  none
 */
void      ull_advStop(void);

#endif /* FEATURE_ADVERTISER */

#if defined(FEATURE_SCANNER)
/*********************************************************************
 * @fn     ull_scanRegisterCB
 *
 * @brief  Register callbacks supposed to be called by Scanner.
 *
 * @param  pfnScanIndicationCB - callback to nofity the application that a packet is
 *                               received.
 * @param  pfnScanWindowCompleteCB - callback to post-process scan window complete Event
 *
 * @return  none
 */
void ull_scanRegisterCB(pfnScanIndCB_t pfnScanIndicationCB,
                        pfnScanWindowCompCB_t pfnScanWindowCompleteCB);

/*********************************************************************
 * @fn      ull_scanStart
 *
 * @brief   Enter ULL_STATE_SCANNING
 *
 * @param   scanChanIndex - scan channel index
 *
 * @return  SUCCESS - Successfully entered ULL_STATE_SCANNING
 *          FAILURE - Failed to enter ULL_STATE_SCANNING
 */
bStatus_t ull_scanStart(uint8_t scanChanIndexMap);

/*********************************************************************
 * @fn      ull_scanStop
 *
 * @brief   Exit ULL_STATE_SCANNING
 *
 * @param   none
 *
 * @return  none
 */
void      ull_scanStop(void);

#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
/*********************************************************************
 * @fn     ull_monitorRegisterCB
 *
 * @brief  Register callbacks supposed to be called by Monitor.
 *
 * @param  pfnMonitorIndicationCB - callback to nofity the application that
 *                                  a packet is received.
 * @param  pfnMonitorCompleteCB - callback to post-process monitor duration complete Event
 *
 * @return  none
 */
void ull_monitorRegisterCB(pfnMonitorIndCB_t pfnMonitorIndicationCB,
                           pfnMonitorCompCB_t pfnMonitorCompleteCB);

/*********************************************************************
 * @fn      ull_monitorStart
 *
 * @brief   Enter ULL_STATE_MONITORING
 *
 * @param   channel - monitoring scan channel
 *
 * @return  SUCCESS - Successfully entered ULL_STATE_MONITORING
 *          FAILURE - Failed to enter ULL_STATE_MONITORING
 */
bStatus_t ull_monitorStart(uint8_t channel);

#endif /* FEATURE_MONITOR */

#ifdef __cplusplus
}
#endif

#endif /* ULL_H */
