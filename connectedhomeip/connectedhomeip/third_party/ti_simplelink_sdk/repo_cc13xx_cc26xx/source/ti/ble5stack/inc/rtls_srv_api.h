/******************************************************************************

 @file  rtls_srv_api.h

 @brief This file implements the RTLS Services APIs
 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2018-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/**
 *  @defgroup RTLSSrv RTLSSrv
 *  @brief  This module implements the RTLS Services APIs
 *
 *  @{
 *  @file  rtls_srv_api.h
 *  @brief      RTLS Services
 */

#ifndef RTLSSRVAPI_H
#define RTLSSRVAPI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */

#include "bcomdef.h"
#include <ti/drivers/PIN.h>
#include "ble_user_config.h"
#include "hci.h"
#include "ll.h"

/*-------------------------------------------------------------------
 * CONSTANTS
 */

/**
 * @defgroup RTLSSrv_Constants RTLS Services constants
 *
 * Used to configure RTLS
 * RTLSSrv_setConnectionCteReceiveParams
 * RTLSSrv_setConnectionCteTransmitParams
 *
 * @{
 */

/// @brief CTE Sampling state
#define RTLSSRV_CTE_SAMPLING_NOT_INIT                   LL_CTE_SAMPLING_NOT_INIT       //!< Not Initiated
#define RTLSSRV_CTE_SAMPLING_ENABLE                     LL_CTE_SAMPLING_ENABLE         //!< Enabled
#define RTLSSRV_CTE_SAMPLING_DISABLE                    LL_CTE_SAMPLING_DISABLE        //!< Disabled

/// @brief CTE sample slot type
#define RTLSSRV_CTE_SAMPLE_SLOT_1US                     LL_CTE_SAMPLE_SLOT_1US         //!< 1 &mu;sec
#define RTLSSRV_CTE_SAMPLE_SLOT_2US                     LL_CTE_SAMPLE_SLOT_2US         //!< 2 &mu;sec

/// @brief CTE supported modes, only AoA at this point
#define RTLSSRV_CTE_SAMPLE_RATE_1US_AOD_TX              LL_CTE_SAMPLE_RATE_1US_AOD_TX  //!< Angle of Departure TX
#define RTLSSRV_CTE_SAMPLE_RATE_1US_AOD_RX              LL_CTE_SAMPLE_RATE_1US_AOD_RX  //!< Angle of Departure RX
#define RTLSSRV_CTE_SAMPLE_RATE_1US_AOA_RX              LL_CTE_SAMPLE_RATE_1US_AOA_RX  //!< Angle of Arrival RX

/// @brief CTE types
#define RTLSSRV_CTE_TYPE_AOA                            LL_CTE_TYPE_AOA            //!< Angle of Arrival

/// @brief CTE Sample Rates
#define RTLSSRV_CTE_SAMPLE_RATE_1MHZ                    CTE_SAMPLING_CONFIG_1MHZ   //!< 1 MHz
#define RTLSSRV_CTE_SAMPLE_RATE_2MHZ                    CTE_SAMPLING_CONFIG_2MHZ   //!< 2 MHz
#define RTLSSRV_CTE_SAMPLE_RATE_3MHZ                    CTE_SAMPLING_CONFIG_3MHZ   //!< 3 MHz
#define RTLSSRV_CTE_SAMPLE_RATE_4MHZ                    CTE_SAMPLING_CONFIG_4MHZ   //!< 4 MHz

/// @brief CTE Sample Size
#define RTLSSRV_CTE_SAMPLE_SIZE_8BITS                   LL_CTE_SAMPLE_SIZE_8BITS   //!< 8 bit
#define RTLSSRV_CTE_SAMPLE_SIZE_16BITS                  LL_CTE_SAMPLE_SIZE_16BITS  //!< 16 bit

/// @brief CTE Sample Control
#define RTLSSRV_CTE_SAMPLE_CONTROL_RF_DEFAULT_FILTERING CTE_SAMPLING_CONTROL_DEFAULT_FILTERING
#define RTLSSRV_CTE_SAMPLE_CONTROL_RF_RAW_NO_FILTERING  CTE_SAMPLING_CONTROL_RF_RAW_NO_FILTERING

/// CTE antenna minimum switch length
#define RTLSSRV_CL_CTE_ANTENNA_LIST_MIN_LENGTH          LL_CTE_ANTENNA_LIST_MIN_LENGTH
/// CTE antenna maximum switch length
#define RTLSSRV_CL_CTE_ANTENNA_LIST_MAX_LENGTH          LL_CTE_ANTENNA_LIST_MAX_LENGTH

/// CTE minimun length
#define RTLSSRV_CL_CTE_MIN_LEN                          LL_CTE_MIN_LEN
/// CTE maximum length
#define RTLSSRV_CL_CTE_MAX_LEN                          LL_CTE_MAX_LEN

/// CTE count - minimum value
#define RTLSSRV_CL_CTE_MIN_COUNT                        LL_CTE_COUNT_MIN
/// CTE count - maximum value
#define RTLSSRV_CL_CTE_MAX_COUNT                        LL_CTE_COUNT_MAX

/// Sync handle mask
#define RTLSSRV_SYNC_HANDLE_MASK                        0x1000
/// Reverse sync handle mask
#define RTLSSRV_REVERSE_SYNC_HANDLE                     0x0FFF
/** @} End RTLSSrv_Constants */

/**
 * @defgroup RTLSSrv_Events RTLS Services event types
 *
 * Event types are used to parse various messages produced by RTLS Services
 * Events are passed using @ref RTLSSrv_CBs
 *
 * @{
 */

/**
 * CTE I/Q report is received
 * Used to identify @ref rtlsSrv_connectionIQReport_t
 */
#define RTLSSRV_CONNECTION_CTE_IQ_REPORT_EVT            1

/**
 * Antenna information event is received
 * Used to identify @ref rtlsSrv_antennaInfo_t
 */
#define RTLSSRV_ANTENNA_INFORMATION_EVT                 2

/**
 * Generated when a CTE request fails
 */
#define RTLSSRV_CTE_REQUEST_FAILED_EVT                  3

/**
 * Generated when an async error occurs
 */
#define RTLSSRV_ERROR_EVT                               4

/**
* Genetated when CL CTE cmd complete returns
*/
#define RTLSSRV_CL_CTE_EVT                              5

/**
* Genetated when read adv list size command
* complete returns
*/
#define RTLSSRV_READ_PERIODIC_LIST_SIZE_EVT             6

/**
* Genetated when the controller receives
* the first periodic advertising packet
*/
#define RTLSSRV_SYNC_EST_EVT                            7

/**
* Genetated when the controller has not received
* a periodic advertising packets
*/
#define RTLSSRV_SYNC_LOST_EVT                           8

/**
* Genetated when a periodic advertising packet
* was received
*/
#define RTLSSRV_PERIODIC_ADV_RPT                        9

/**
* Genetated when connectionless IQ report was received
* from the controller
*/
#define RTLSSRV_CL_CTE_IQ_REPORT_EVT                    10

/**
* Generated when termminate sync command complete
* event was received from the controller
*/
#define RTLSSRV_PERIODIC_ADV_TERMINATE_SYNC             11

/**
* Genetated when sampling enable command complete
* event was received from the controller
*/
#define RTLSSRV_CL_AOA_ENABLE_EVT                       12
/** @} End RTLSSrv_Events */


/**
 * @defgroup RTLSSrv_ErrorCodes RTLS Services error codes
 *
 * Error codes used to parse @ref RTLSSRV_ERROR_EVT
 * Errors produced by HCI are mapped to their respective HCI Error values
 *
 * @{
 */

#define RTLSSRV_CONN_HANDLE_INVALID              0x02             //!< Invalid Handle
#define RTLSSRV_OUT_OF_MEMORY                    0x07             //!< Out of Memory
#define RTLSSRV_COMMAND_DISALLOWED               0x0C             //!< Disallowed Command
#define RTLSSRV_ERROR_DUE_TO_LIMITED_RESOURCES   0x0D             //!< Limited Resources
#define RTLSSRV_FEATURE_NOT_SUPPORTED            0x11             //!< Unsupported Feature
#define RTLSSRV_BAD_PARAMETER                    0x12             //!< Invalid parameter
#define RTLSSRV_REMOTE_FEATURE_NOT_SUPPORTED     0x1A             //!< Unsupported Remote Feature
#define RTLSSRV_UNKNOWN_ADVERTISING_IDENTIFIER   0x42             //!< Unknown Advertising Identifier

/** @} End RTLSSrv_ErrorCodes */

/**
 * @defgroup RTLSSrv_Commands RTLS Services Commands
 *
 * Defines for commands that can return errors
 *
 * @{
 */

#define RTLSSRV_SET_CONNECTION_CTE_RECEIVE_PARAMS      0x2054             //!< Receive Parameters
#define RTLSSRV_SET_CONNECTION_CTE_TRANSMIT_PARAMS     0x2055             //!< Transmit Parameters
#define RTLSSRV_SET_CONNECTION_CTE_REQUEST_ENABLE      0x2056             //!< Enable Request
#define RTLSSRV_SET_CONNECTION_CTE_RESPONSE_ENABLE     0x2057             //!< Enable Response
#define RTLSSRV_SET_CL_CTE_TRANSMIT_PARAMS             0x2051             //!< CL CTE transmit params cmd complete event
#define RTLSSRV_SET_CL_CTE_TRANSMIT_ENABLE             0x2052             //!< CL CTE transmit Enable cmd complete event
#define RTLSSRV_SET_CL_IQ_SAMPLING_ENABLE              0x2053             //!< CL CTE iq sampling enable


/** @} End RTLSSrv_Commands */

/*-------------------------------------------------------------------
 * TYPEDEFS
 */


/**
 * @defgroup RTLSSrv_Structs RTLS Services Structures
 * @{
 */

/// @brief CTE Connection IQ Report Event @ref RTLSSRV_CONNECTION_CTE_IQ_REPORT_EVT
typedef struct
{
  uint16_t connHandle;              //!< connection handle
  uint8_t  phy;                     //!< current phy
  uint8_t  dataChIndex;             //!< index of data channel
  uint16_t rssi;                    //!< current rssi
  uint8_t  rssiAntenna;             //!< antenna ID
  uint8_t  cteType;                 //!< cte type
  uint8_t  slotDuration;            //!< sampling slot 1us or 2us
  uint8_t  status;                  //!< packet status (success or CRC error)
  uint16_t connEvent;               //!< connection event
  uint16_t sampleCount;             //!< number of samples
  uint8_t  sampleRate;              //!< sample rate of frontend: 1Mhz, 2Mhz, 3Mhz, 4Mhz
  uint8_t  sampleSize;              //!< sample size: 8 or 16 bit samples
  uint8_t  sampleCtrl;              //!< default filtering or RAW_RF
  uint8_t  numAnt;                  //!< number of antennas we are using
  int8_t   *iqSamples;              //!< list of IQ samples, format is [i,q,i,q,i,q....]
} rtlsSrv_connectionIQReport_t;

/// @brief CTE Connectionless IQ Report Event @ref RTLSSRV_CL_CTE_IQ_REPORT_EVT
typedef struct
{
  uint16_t syncHandle;              //!< sync handle
  uint8_t  channelIndex;            //!< index of data channel
  uint16_t rssi;                    //!< current rssi
  uint8_t  rssiAntenna;             //!< antenna ID
  uint8_t  cteType;                 //!< cte type
  uint8_t  slotDuration;            //!< sampling slot 1us or 2us
  uint8_t  sampleRate;              //!< sample rate of frontend: 1Mhz, 2Mhz, 3Mhz, 4Mhz
  uint8_t  sampleSize;              //!< sample size: 8 or 16 bit samples
  uint8_t  sampleCtrl;              //!< default filtering or RAW_RF
  uint8_t  packetStatus;            //!< packet status (success or CRC error)
  uint16_t eventCounter;            //!< periodic adv event counter
  uint8_t  numAnt;                  //!< number of antennas we are using
  uint16_t sampleCount;             //!< number of samples
  int8_t   *iqSamples;              //!< list of IQ samples, format is [i,q,i,q,i,q....]
} rtlsSrv_clIQReport_t;


/// @brief LE CTE Antenna Information Event @ref RTLSSRV_ANTENNA_INFORMATION_EVT
typedef struct
{
  uint8_t sampleRates;              //!< sample rates supported by the controller
  uint8_t maxNumOfAntennas;         //!< maximum number of antennas supported by the controller
  uint8_t maxSwitchPatternLen;      //!< maximum size of switch pattern supported by the controller
  uint8_t maxCteLen;                //!< maximum cte length supported by the controller
} rtlsSrv_antennaInfo_t;

/// @brief LE CTE Request Failed Event @ref RTLSSRV_CTE_REQUEST_FAILED_EVT
typedef struct
{
  uint8_t status;                   //!< status
  uint16_t connHandle;              //!< connection handle
} rtlsSrv_cteReqFailed_t;

/// @brief RTLS Services error structure @ref RTLSSRV_ERROR_EVT
typedef struct
{
  uint16_t connHandle;              //!< connection handle
  uint16_t errSrc;                  //!< the command that caused the error
  uint16_t errCause;                //!< subEvt @ref RTLSSrv_ErrorCodes
} rtlsSrv_errorEvt_t;

/// CL AoA command complete event
typedef struct
{
  uint16_t opcode;      //!< RTLS event opcode
  uint8_t status;      //!< Event status
} rtlsSrv_ClCmdCompleteEvt_t;

/// CL AoA Enable command complete event
typedef struct
{
  uint16_t opcode;      //!< RTLS event opcode
  uint8_t  status;      //!< Event status
  uint16_t syncHandle;  //!< Handle identifying the periodic advertising train
} rtlsSrv_ClAoAEnableEvt_t;

/// Periodic advertising sync establish event structure
typedef struct
{
  uint8_t  opcode;               //!< RTLS event opcode
  uint8_t  status;               //!< Periodic advertising sync HCI status
  uint16_t syncHandle;           //!< Handle identifying the periodic advertising train
  uint8_t  advSid;               //!< Value of the Advertising SID subfield in the ADI field of the PDU
  uint8_t  advAddrType;          //!< Advertiser address type:\n 0x00 - Public\n 0x01 - Random\n 0x02 - Public Identity Address\n 0x03 - Random Identity Addres
  uint8_t  advAddress[6];        //!< Advertiser address
  uint8_t  advPhy;               //!< Advertiser PHY:\n 0x01 - LE 1M\n 0x02 - LE 2M\n 0x03 - LE Coded
  uint16_t periodicAdvInt;       //!< Periodic advertising interval Range: 0x0006 to 0xFFFF. Time = N * 1.25 ms (Time Range: 7.5 ms to 81.91875 s)
  uint8_t  advClockAccuracy;     //!< Accuracy of the periodic advertiser's clock:\n 0x00 - 500 ppm\n 0x01 - 250 ppm\n 0x02 - 150 ppm\n 0x03 - 100 ppm\n 0x04 - 75 ppm\n 0x05 - 50 ppm\n 0x06 - 30 ppm\n 0x07 - 20 ppm
} rtlsSrv_SyncEstEvt_t;

/// Periodic advertising sync lost event
typedef struct
{
  uint8_t  opcode;          //!< RTLS event opcode
  uint16_t syncHandle;      //!< Handle identifying the periodic advertising train
} rtlsSrv_SyncLostEvt_t;

/// Periodic adverting report event
typedef struct
{
  uint8_t  opcode;            //!< RTLS event opcode
  uint16_t syncHandle;        //!< Handle identifying the periodic advertising train
  int8_t   txPower;           //!< Tx Power information (Range: -127 to +20 dBm)
  int8_t   rssi;              //!< RSSI value for the received packet (Range: -127 to +20 dBm); If the packet contains CTE, this value is not available
  uint8_t  cteType;           //!< 0x00 AoA CTE\n 0x01 - AoD CTE with 1us slots\n 0x02 - AoD CTE with 2us slots\n 0xFF - No CTE
  uint8_t  dataStatus;        //!< 0x00 - Data complete\n 0x01 - Data incomplete, more data to come\n 0x02 - Data incomplete, data truncated, no more to come
  uint8_t  dataLen;           //!< Length of the Data field (Range: 0 to 247)
  uint8_t  *pData;            //!< Data received from a Periodic Advertising packet
} rtlsSrv_PeriodicAdvRpt_t;

/** @} End RTLSSrv_Structs */

/**
 * @defgroup RTLSSrv_CBs RTLS Services callbacks
 * @{
 * These are functions whose pointers are passed from the application
 * to the RTLSSrv module. The module will call the callbacks when the corresponding events
 * are sent via HCI
 */

/// @brief RTLS Services passes messages in this format
typedef struct
{
  uint8_t   evtType;              //!< Type
  uint16_t  evtSize;              //!< Size
  uint8_t   *evtData;             //!< Pointer to Event Data
} rtlsSrv_evt_t;

/**
 * RTLSSrv Callback Structure
 *
 * This must be setup by the application and passed to the RTLSSrv when RTLSSrv_Register is called
 *
 * @param evtType - is used to parse the event according to @ref RTLSSrv_Events
 * @param evtSize - size of evtData
 * @param evtData - pointer to event data structure @ref RTLSSrv_Structs
 */
typedef void (*pfnAppEventHandlerCB_t)(rtlsSrv_evt_t *pEvt);

/** @} End RTLSSrv_CBs */

/// Maximum number of buffers for CL AoA
extern uint8_t maxNumClCteBufs;

/*-------------------------------------------------------------------
 * API's
 */

/**
 * Initialize RTLSSrv module
 *
 * @param numOfRTLSConns - number of connections that RTLS Srv is required to support
 *
 * @return SUCCESS/FAILURE
 */
extern bStatus_t RTLSSrv_init(uint8_t numOfRTLSConns);

/**
 * Register callback functions with RTLS Services
 *
 * @param pCB pointer to callback function structure
 *
 * @return SUCCESS/FAILURE
 */
extern bStatus_t RTLSSrv_register(pfnAppEventHandlerCB_t pCB);

/**
 * Note: RTLSSrv_initAntArray must be called before calling this functions
 *       In order to not limit users GPIO configuration, it is the users responsibility
 *       to either call RTLSSrv_initAntArray or to initialize GPIO's in another manner
 *
 * Used to:
 * 1. Enable or disable sampling received Constant Tone Extension fields on a connection
 * 2. Configure the antenna switching pattern
 * 3. Configure switching and sampling slot durations to be used
 *
 * @param connHandle - Connection handle
 * @param samplingEnable - Sample CTE on a connection and report the samples to the User Application (0 or 1)
 * @param slotDurations - Switching and sampling slots in 1 &mu;s or 2 &mu;s each (1 or 2)
 * @param numAnt - The number of Antenna IDs in the pattern (2 to 75)
 * @param antArray - List of Antenna IDs in the pattern
 *
 * @return bStatus_t - command was acked, async return status in case of an error provided by RTLS_ERROR_EVT
 */
extern bStatus_t RTLSSrv_setConnCteReceiveParams(uint16_t connHandle,
                                                 uint8_t  samplingEnable,
                                                 uint8_t  slotDurations,
                                                 uint8_t  numAnt,
                                                 uint8_t  antArray[]);

/**
 * Used to:
 * 1. Set the antenna switching pattern
 * 2. Set permitted Constant Tone Extension types used for transmitting
 *    Constant Tone Extensions requested by the peer device on a connection
 *
 * @param connHandle - Connection handle
 * @param types - CTE types (0 - Allow AoA CTE Response)
 * @param length - The number of Antenna IDs in the pattern (2 to 75)
 * @param antArray - List of Antenna IDs in the pattern
 *
 * @return bStatus_t - command was acked, async return status in case of an error provided by RTLS_ERROR_EVT
*/
extern bStatus_t RTLSSrv_setConnCteTransmitParams(uint16_t connHandle,
                                                  uint8_t  types,
                                                  uint8_t  length,
                                                  uint8_t  antArray[]);

/**
 * Start or stop initiating the CTE Request procedure on a connection
 * Note that the API enables periodically requesting CTE by using the 'interval' parameter
 *
 * @param connHandle - Connection handle
 * @param enable - Enable or disable CTE Request for a connection (1 or 0)
 * @param interval - Requested interval for initiating the CTE Request procedure in number of connection events (1 to 0xFFFF).\n
 *                   1 means the next CTE Request procedure is initiated at the connection interval following the completion
 *                   of a previous CTE Request procedure.\n
 *                   The first CTE Request procedure is sent as soon as possible no matter the value set for this parameter.
 * @param length - Min length of the CTE being requested in 8 &mu;s units (2 to 20)
 * @param type - Requested CTE type (0 - AoA, 1 - AoD with 1&mu;s slots, 2 - AoD with 2&mu;s slots)
 *
 * @return bStatus_t - command was acked, async return status in case of an error provided by RTLS_ERROR_EVT
*/
extern bStatus_t RTLSSrv_setConnCteRequestEnableCmd(uint16_t connHandle,
                                                    uint8_t  enable,
                                                    uint16_t interval,
                                                    uint8_t  length,
                                                    uint8_t  type);

/**
 * Enable responding to CTE requests
 *
 * @param connHandle - Connection handle
 * @param enable - Enable or disable CTE Response for a connection (1 or 0)
 *
 * @return bStatus_t - command was acked, async return status in case of an error provided by RTLS_ERROR_EVT
*/
extern bStatus_t RTLSSrv_setConnCteResponseEnableCmd(uint16_t connHandle,
                                                     uint8_t  enable);

/**
 * Read Antenna information
 *
 * @return bStatus_t - command was acked, async return status in case of an error provided by @ref RTLSSRV_ANTENNA_INFORMATION_EVT
*/
extern bStatus_t RTLSSrv_readAntennaInformationCmd(void);

/**
 * Set CTE sampling accuracy
 *
 * @param handle Connection/sync handle.
 * @param sampleRate1M sample rate for PHY 1M
 *        range : 1 - least accuracy (as in 5.1 spec) to 4 - most accuracy
 * @param sampleSize1M sample size for PHY 1M
 *        range : 1 - 8 bits (as in 5.1 spec) or 2 - 16 bits (more accurate)
 * @param sampleRate2M sample rate for PHY 2M
 *        range : 1 - least accuracy (as in 5.1 spec) to 4 - most accuracy
 * @param sampleSize2M sample size for PHY 2M
 *        range : 1 - 8 bits (as in 5.1 spec) or 2 - 16 bits (more accurate)
 * @param sampleCtrl - sample control flags
 *        range : range : bit0=0 - Default filtering, bit0=1 - RAW_RF(no filtering), , bit1..7=0 - spare
 *
 * @return bStatus_t - command was acked, async return status in case of an error provided by RTLS_ERROR_EVT
*/
extern bStatus_t RTLSSrv_setCteSampleAccuracy(uint16_t handle,
                                              uint8_t  sampleRate1M,
                                              uint8_t  sampleSize1M,
                                              uint8_t  sampleRate2M,
                                              uint8_t  sampleSize2M,
                                              uint8_t  sampleCtrl);
/**
 * Initialize GPIO's which correspond to Antenna Table configured in sysConfig or in ble_user_config.c (antennaTbl)
 * Initialize one of the antenna ID's(mainAntenna) as the main antenna used to Tx/Rx
 *
 * @param mainAntenna - Antenna ID to be used as main receiving antenna
 *
 * @return PIN_Handle - handle for initialized pins
 */
extern PIN_Handle RTLSSrv_initAntArray(uint8_t mainAntenna);

/**
 * RTLSSrv_processHciEvent
 *
 * This is the HCI event handler for RTLSSrv module
 * Any HCI event related to RTLS should pass through this API
 *
 * @param hciEvt - is used to parse the event according to @ref RTLSSrv_Events
 * @param hciEvtSz - size of evtData
 * @param pEvtData - pointer to event data structure @ref RTLSSrv_Structs
 *
 * @return      TRUE = success, FALSE = failure
 */
extern bStatus_t RTLSSrv_processHciEvent(uint16_t hciEvt, uint16_t hciEvtSz, uint8_t *pEvtData);

/**
 * RTLSSrv_SetCLCteTransmitParams
 *
 * Used to request the advertiser to enable or disable
 * the periodic advertising for the advertising set
 *
 * @design  /ref did_214342976
 *
 * @param   advHandle - Used to identify a periodic advertising train
 * @param   cteLength - CTE length in 8 us units.
 * @param   cteType   - 0x00 - AoA CTE
 *                    - 0x01 - AoD CTE with 1 us slots.
 *                    - 0x02 - AoD CTE with 2 us slots.
 * @param   cteCount  - The number of CTE to transmit in each
 *                      periodic advertising interval. Range: 0x01-0x10
 * @param   switchLen - The number of Antenna IDs in the pattern.
 *                      Range: 0x02-0x4B.
 * @param   pAntenna  - list of Antenna IDs in the pattern.
 *
 * @return  SUCCESS, RTLSSRV_COMMAND_DISALLOWED,
 *          RTLSSRV_FEATURE_NOT_SUPPORTED, RTLSSRV_BAD_PARAMETER
 */
 bStatus_t RTLSSrv_SetCLCteTransmitParams( uint8 advHandle,
                                           uint8 cteLength,
                                           uint8 cteType,
                                           uint8 cteCount,
                                           uint8 switchLen,
                                           uint8 *pAntenna );

/**
 * RTLSSrv_CLCteTransmitEnable
 *
 * Used to request the advertiser to enable or disable
 * the periodic advertising for the advertising set
 *
 * @design  /ref did_214342976
 *
 * @param   advHandle - Used to identify a periodic advertising train
 * @param   enable    - 0x00 - Periodic advertising is disabled (default)
 *                    - 0x01 - Periodic advertising is enabled
 *
 * @return  SUCCESS, RTLSSRV_BAD_PARAMETER, RTLSSRV_COMMAND_DISALLOWED
 */
 bStatus_t RTLSSrv_CLCteTransmitEnable( uint8 advHandle,
                                        uint8 enable );

/**
 * RTLSSrv_setCLCteSamplingEnableCmd
 *
 * Request the Controller to enable or disable capturing
 * IQ samples from the CTE of periodic advertising packets in the periodic
 * advertising train identified by the syncHandle parameter.
 *
 * @design  /ref did_214342976
 *
 * @param   syncHandle     - Handle identifying the periodic advertising train (Range: 0x0000 to 0x0EFF)
 * @param   enable         - 0x00 - Disable CTE sampling
 *                         - 0x01 - Enable CTE sampling
 * @param   slotDuration   - Switching and sampling slots in 1 us or 2 us each (1 or 2).
 * @param   maxSampleCte   - 0 - Sample and report all available CTEs\n
 *                           1-16 - Max number of CTEs to sample and report in each periodic event
 * @param   numAnt         - Number of items in Antenna array (relevant to AoA only)
 * @param   pAntPattern    - Pointer to Antenna array (relevant to AoA only)
 *
 * @return  SUCCESS, RTLSSRV_COMMAND_DISALLOWED, RTLSSRV_BAD_PARAMETER
 *          RTLSSRV_UNKNOWN_ADVERTISING_IDENTIFIER
 */
bStatus_t RTLSSrv_setCLCteSamplingEnableCmd( uint16_t syncHandle,
                                             uint8_t enable,
                                             uint8_t slotDuration,
                                             uint8_t maxSampleCte,
                                             uint8_t numAnt,
                                             uint8_t pAntPattern[] );

#ifdef __cplusplus
}
#endif

#endif /* RTLSSRVAPI_H */

/** @} End RTLSSrv */
