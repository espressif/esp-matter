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
 *  @defgroup URTLS
 *  @brief This module implements configuration, initialization,
 *   and parameter management of RTLS. It also contains
 *   interfacing between Micro BLE Stack and the application.
 *
 */

#ifndef URTLS_H
#define URTLS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */
#include <bcomdef.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/drivers/PIN.h>
#if !defined(DeviceFamily_CC26X1)
#include <driverlib/rf_bt5_iq_autocopy.h>
#else
#include <ti/devices/cc13x2_cc26x2/driverlib/rf_bt5_iq_autocopy.h>
#endif

#include <uble.h>

/*-------------------------------------------------------------------
 * DEFINITIONS & CONSTANTS
 */

#define URTLS_SUCCESS                                  (0)
#define URTLS_FAIL                                     (1)

/**
 * CTE I/Q report is received
 * Used to identify @ref rtlsSrv_connectionIQReport_t
 */
#define URTLS_CONNECTION_CTE_IQ_REPORT_EVT             (1)

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

// sampling control flags
#define URTLS_CTE_SAMPLING_CONTROL_DEFAULT_FILTERING   (0x00)      // skip antenna switching samples
#define URTLS_CTE_SAMPLING_CONTROL_RF_RAW_NO_FILTERING (0x01)      // copy i/q samples as is from RF RAM
#define URTLS_CTE_SAMPLING_CONTROL_DEFAULT             (URTLS_CTE_SAMPLING_CONTROL_DEFAULT_FILTERING)
// set the CTE sampling rate 4Mhz (4 samples in 1us)
#define URTLS_CTE_SAMPLING_CONFIG_1MBPS                (CTE_SAMPLING_CONFIG_4MHZ)
#define URTLS_CTE_SAMPLING_CONFIG_2MBPS                (CTE_SAMPLING_CONFIG_4MHZ)

// CTE Sampling state
#define URTLS_CTE_SAMPLING_NOT_INIT                    (0)
#define URTLS_CTE_SAMPLING_ENABLE                      (1)
#define URTLS_CTE_SAMPLING_DISABLE                     (2)

// CTE Info packet fields
#define URTLS_CTE_INFO_TIME_MASK                       (0x1F)
#define URTLS_CTE_INFO_TYPE_MASK                       (0xC0)
#define URTLS_CTE_INFO_TYPE_OFFSET                     (6)     // CTE time (5 bits) + RFU (1 bit)

// CTE types
#define URTLS_CTE_TYPE_AOA                             (0)
#define URTLS_CTE_TYPE_AOD_1US                         (1)
#define URTLS_CTE_TYPE_AOD_2US                         (2)
#define URTLS_CTE_TYPE_NONE                            (0xFF)

// Data PDU Header Fields
#define URTLS_DATA_PDU_HDR_CP_BIT                      (5)

// Number of CTE Sampling Buffers
#ifndef MAX_NUM_CTE_BUFS
#define MAX_NUM_CTE_BUFS 1
#endif

// URTLS_MALLOC memory allocation without icall
#define URTLS_MALLOC(pAlloc, size) {                           \
                                        volatile uint32_t keyHwi; \
                                        volatile uint32_t keySwi; \
                                        keyHwi = Hwi_disable();   \
                                        keySwi = Swi_disable();   \
                                        pAlloc = malloc(size);    \
                                        Swi_restore(keySwi);      \
                                        Hwi_restore(keyHwi);      \
                                      }
// URTLS_MALLOC memory free without icall
#define URTLS_FREE(pFree)          {                           \
                                        volatile uint32_t keyHwi; \
                                        volatile uint32_t keySwi; \
                                        keyHwi = Hwi_disable();   \
                                        keySwi = Swi_disable();   \
                                        free(pFree);              \
                                        pFree = NULL;             \
                                        Swi_restore(keySwi);      \
                                        Hwi_restore(keyHwi);      \
                                      }
/*-------------------------------------------------------------------
 * TYPEDEFS
 */

// IQ Samples RF Header struct
typedef struct
{
  uint16_t                     length;
  uint8_t                      cteInfo;
  rfc_statusIqSamplesEntry_t   status;
  uint8_t                      rfGain;
  uint8_t                      rssi;
  uint8_t                      padding[2];
} urtls_cteSamplesRfHeader_t;

typedef const uint32 antennaIOEntry_t;

typedef struct
{
  uint32_t      antMask;
  uint8_t       antPropTblSize;
  antennaIOEntry_t      *antPropTbl;
} cteAntProp_t;

//Antenna switch struct
typedef struct
{
  uint8_t       numEntries;                //Number of IO value entries in the table. If this is less than the number of slots, the IO value entries are repeated in a circular manner
  uint8_t       switchTime;                //1: 1 �s switching and sampling slots 2: 2 �s switching and sampling slots Others: Reserved
  uint16_t      reserved;
  uint32_t      ioMask;                    //Bit mask defining the DIOs used for antenna switching. A 1 indicates that the corresponding DIO is used
  uint32_t      ioEntry[1];                //Entry defining the values of the DIOs used for the antenna switching for Sample slot #N. Only the bits corresponding to 1's in ioMask are used
} urtls_cteAntSwitch_t;

// received CTE response info struct
typedef struct
{
  uint8_t       dataChIndex;               // CTE response data channel index
  uint8_t       packetStatus;              // CTE response packet status (success or CRC error)
  uint8_t       rssiAntenna;               // First antenna which RSSI was measured on.
  int16_t       rssi;                      // RSSI measured on received CTE response
  uint8_t       length;                    // CTE length received from peer
  uint8_t       type;                      // CTE type received from peer
}urtls_cteRecvInfo_t;

// CTE sample configuration struct
typedef struct
{
  uint8_t       sampleRate1M;               // CTE sample rate for 1Mbps phy
  uint8_t       sampleRate2M;               // CTE sample rate for 2Mbps phy
  uint8_t       sampleSize1M;               // CTE sample size for 1Mbps phy
  uint8_t       sampleSize2M;               // CTE sample size for 2Mbps phy
  uint8_t       sampleCtrl;                 // CTE sample control flags (bit0-RAW_RF(no filtering), ...)
}urtls_cteSampleConfig_t;

//CTE request info struct
typedef struct
{
  uint8_t                   samplingEnable;  // CTE sampling enable received from Host
  uint8_t                   requestEnable;   // CTE request enable received from Host
  uint8_t                   requestLen;      // CTE request length received from Host
  uint8_t                   requestType;     // CTE request type received from Host
  uint8_t                   sendRequest;     // flag indicates controller send CTE request to peer
  uint8_t                   recvCte;         // flag indicates received CTE from peer
  uint16_t                  requestInterval; // CTE periodic received from Host
  uint32_t                  periodicEvent;   // Connection event to send CTE request
  urtls_cteSampleConfig_t   sampleConfig;    // CTE sample Host configuration
  urtls_cteRecvInfo_t       recvInfo;        // CTE response info received
  urtls_cteAntSwitch_t      *pAntenna;       // Antenna switch pattern for AoA
} urtls_cteInfo_t;

// CTE IQ Samples struct
typedef struct
{
  dataQ_t             queue;                 // Auto Copy buffer queue
  rfc_iqAutoCopyDef_t autoCopy;              // Auto Copy RF struct
  dataEntry_t         *pAutoCopyBuffers;     // Pointer to the allocated auto copy buffers
  uint8_t             autoCopyCompleted;     // Counter indicates that RF finished copy the samples
} urtls_cteSamples_t;

// CTE Connection IQ Report Event
typedef struct
{
  uint8_t  sessionId;               //!< SessionId
  uint8_t  dataChIndex;             //!< index of data channel
  uint16_t rssi;                    //!< current rssi
  uint8_t  rssiAntenna;             //!< antenna ID
  uint8_t  cteType;                 //!< cte type
  uint8_t  slotDuration;            //!< sampling slot 1us or 2us
  uint8_t  status;                  //!< packet status (success or CRC error)
  uint16_t sampleCount;             //!< number of samples
  uint8_t  sampleRate;              //!< sample rate of frontend: 1Mhz, 2Mhz, 3Mhz, 4Mhz
  uint8_t  sampleSize;              //!< sample size: 8 or 16 bit samples
  uint8_t  sampleCtrl;              //!< default filtering or RAW_RF
  uint8_t  numAnt;                  //!< number of antennas we are using
  int8_t   *iqSamples;              //!< list of IQ samples, format is [i,q,i,q,i,q....]
} urtls_connectionIQReport_t;

/// urtls passes messages in this format
typedef struct
{
  uint8_t   evtType;          //!< Type
  uint16_t  evtSize;          //!< Size
  uint8_t   *evtData;         //!< Pointer to Event Data
} urtls_evt_t;

/**
 * urtls Callback Structure
 *
 * This must be setup by the application and passed to the urtls when urtls_Register is called
 *
 * @param evtType - is used to parse the event according to @ref RTLSSrv_Events
 * @param evtSize - size of evtData
 * @param evtData - pointer to event data structure @ref RTLSSrv_Structs
 */
typedef void (*pfnAppEventHandlerCB_t)(uint8_t *pEvt);

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */
  //CTE struct
extern urtls_cteSamples_t urtls_cteSamples;
extern urtls_cteInfo_t    urtls_cteInfo[UBLE_MAX_MONITOR_HANDLE];

/*-------------------------------------------------------------------
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      urtls_register
 *
 * Register callback functions with urtls Services
 *
 * @param   pCB pointer to callback function structure
 *
 * @return  SUCCESS/FAILURE
 */
bStatus_t urtls_register(pfnAppEventHandlerCB_t pCB);

/*********************************************************************
 * @fn          urtls_callAppCb
 *
 * @brief       Allocate and send a message to the application
 *
 * @param       evtType - opcode of the event
 * @param       evtSize - size of evtData
 * @param       pEvtData - actual information to pass
 *
 * @return      TRUE = success, FALSE = failure
 */
bStatus_t urtls_callAppCb(uint8_t evtType, uint16_t evtSize, uint8_t *pEvtData);

/*********************************************************************
* @fn      urtls_malloc
*
* @brief   This function will allocate memory, if we were unable to allocate
*          we will return NULL
*
* @param   Allocated pointer - has to be cast
*
* @return  none
*/
void* urtls_malloc(uint32_t sz);

/*******************************************************************************
 * @fn          urtls_rfOverrideCteValue
 *
 * @brief       This function sets CTE value array for the RF Core
 *
 * @param       val      - data to updated
 * @param       address  - Hardware address
 * @param       offset   - field offset from CTE overrides
 *
 * @return      None
 */
void urtls_rfOverrideCteValue(uint32_t val, uint16_t address, uint8_t offset);

/*********************************************************************
 * @fn      urtls_freeConn
 *
 * @brief   Free allocated memory
 *
 * @param   sessionId  - Session ID
 * @param   numHandles - Number of active handles
 *
 * @return  None.
 */
void urtls_freeConn(uint8_t sessionId, uint8_t numHandles);

/*********************************************************************
 * @fn      urtls_initAntArray
 *
 * @brief   Initialize antenna array
 *          To initialize a single pin, use array of length 1
 *
 * @param   mainAntenna - Antenna ID to be used as main receiving antenna
 *
 * @return  PIN_Handle - handle for initialized pins
 */
PIN_Handle urtls_initAntArray(uint8_t mainAntenna);

/*********************************************************************
 * @fn      urtls_setConnCteReceiveParams
 *
 * @brief   Used to enable or disable sampling received Constant Tone Extension fields on a
 *          connection and to set the antenna switching pattern and switching and sampling slot
 *          durations to be used.
 *
 * @param   sessionId - Session Id.
 * @param   samplingEnable - Sample CTE on a connection and report the samples to the Host (0 or 1).
 * @param   slotDurations - Switching and sampling slots in 1 us or 2 us each (1 or 2).
 * @param   length - The number of Antenna IDs in the pattern (2 to 75).
 * @param   pAntenna - List of Antenna IDs in the pattern.
 *
 * @return  URTLS_SUCCESS or URTLS_FAIL
 */
bStatus_t urtls_setConnCteReceiveParams(uint8_t sessionId,
                                        uint8_t samplingEnable,
                                        uint8_t slotDurations,
                                        uint8_t length,
                                        uint8_t *pAntenna);

/*******************************************************************************
 * @fn          urtls_setCteSampleAccuracy
 *
 * @brief       This function used to set CTE accuracy for PHY 1M and 2M
 * *
 * @param       sessionId -    Session ID.
 * @param       sampleRate1M - sample rate for PHY 1M
 *                             range : 1 - least accuracy (as in 5.1 spec) to 4 - most accuracy
 * @param       sampleSize1M - sample size for PHY 1M
 *                             range : 1 - 8 bits (as in 5.1 spec) or 2 - 16 bits (more accurate)
 * @param       sampleRate2M - sample rate for PHY 2M
 *                             range : 1 - least accuracy (as in 5.1 spec) to 4 - most accuracy
 * @param       sampleSize2M - sample size for PHY 2M
 *                             range : 1 - 8 bits (as in 5.1 spec) or 2 - 16 bits (more accurate)
 * @param       sampleCtrl   - sample control flags
 *                             range : bit0=0 - Default filtering, bit0=1 - RAW_RF(no filtering), , bit1..7=0 - spare
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      URTLS_SUCCESS or URTLS_FAIL
 */
bStatus_t urtls_setCteSampleAccuracy(uint8_t  sessionId,
                                     uint8_t  sampleRate1M,
                                     uint8_t  sampleSize1M,
                                     uint8_t  sampleRate2M,
                                     uint8_t  sampleSize2M,
                                     uint8_t  sampleCtrl);

/*******************************************************************************
 * @fn          urtls_setConnCteRequestEnableCmd
 *
 * @brief       This API is used to enable or disable CTE for a connection.
 *
 * @param       sessionId - Session ID.
 * @param       enable -    Enable or disable CTE for a connection (1 or 0).
 * @param       interval -  Requested interval for initiating the CTE Request procedure
 *                          in number of connection events (1 to 0xFFFF)
 * @param       length -    Min length of the CTE being requested in 8 us units (2 to 20).
 * @param       type -      Requested CTE type (0 - AoA, 1 - AoD with 1us slots,
 *                          2 - AoD with 2us slots).
 *
 * @return      URTLS_SUCCESS or URTLS_FAIL
 */
bStatus_t urtls_setConnCteRequestEnableCmd(uint8_t sessionId,
                                           uint8_t enable,
                                           uint16_t interval,
                                           uint8_t length,
                                           uint8_t type);

/*******************************************************************************
 * @fn          urtls_getCteInfo
 *
 * @brief       This function is used to get the CTE info from received CTE packet
 *              and forward the event to the application
 *
 * @param       pDataEntry - Pointer to the queue of copying samples.
 * @param       sessionId -  Session Id.
 * @param       channel -    Data channel index.
 *
 * @return      URTLS_SUCCESS or URTLS_FAIL
 */
bStatus_t urtls_getCteInfo(dataEntry_t *pDataEntry, uint8_t sessionId, uint8_t channel);


#ifdef __cplusplus
}
#endif

#endif /* URTLS_H */

/** @} End URTLS */

/** @} End Micro_BLE_Stack */
