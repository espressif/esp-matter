/******************************************************************************

 @file  urtls.c

 @brief This file contains configuration, initialization,
        and parameter management of RTLS. It also contains
        interfacing between Micro BLE Stack and the application.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2012-2022, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */

#include <stdlib.h>

/* Header files required to enable instruction fetch cache */
#include <driverlib/vims.h>

#include <port.h>
#include <urtls.h>
#include <urfi.h>
#include <urfc.h>

#if defined(AOA_SYSCFG)
#include "ti_aoa_config.h"
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// sampling control flags
#define URTLS_CTE_SAMPLING_CONTROL_DEFAULT_FILTERING   (0x00)      // skip antenna switching samples
#define URTLS_CTE_SAMPLING_CONTROL_RF_RAW_NO_FILTERING (0x01)      // copy i/q samples as is from RF RAM
#define URTLS_CTE_SAMPLING_CONTROL_DEFAULT             (URTLS_CTE_SAMPLING_CONTROL_DEFAULT_FILTERING)

// CTE length
#define URTLS_CTE_MIN_LEN                              (2)         // 16us
#define URTLS_CTE_MAX_LEN                              (20)        // 160us
#define URTLS_CTE_NUM_RF_SAMPLES(cteLen)               (4 *((cteLen * 8) - CTE_OFFSET))

// CTE sample size
#define URTLS_CTE_SAMPLE_SIZE_8BITS                    (1)
#define URTLS_CTE_SAMPLE_SIZE_16BITS                   (2)

// CTE count
#define URTLS_CTE_COUNT_ALL_AVAILABLE                  (0)
#define URTLS_CTE_COUNT_MIN                            (1)
#define URTLS_CTE_COUNT_MAX                            (16)

// CTE sample slot type
#define URTLS_CTE_SAMPLE_SLOT_1US                      (1)
#define URTLS_CTE_SAMPLE_SLOT_2US                      (2)

// CTE antenna switch length
#define URTLS_CTE_ANTENNA_LIST_MIN_LENGTH              (2)
#define URTLS_CTE_ANTENNA_LIST_MAX_LENGTH              (75)
#define URTLS_CTE_MAX_ANTENNAS                         (75)

// CTE report event samples indexing
#define URTLS_CTE_SAMPLES_COUNT_REF_PERIOD             (8)    //number of samples in referece period according to spec
#define URTLS_CTE_FIRST_SAMPLE_IDX_REF_PERIOD          (1)    //reference period start samples index for sample rate 1Mhz,2Mhz and 3Mhz
#define URTLS_CTE_FIRST_SAMPLE_IDX_REF_PERIOD_4MHZ     (0)    //reference period start samples index for sample rate 4Mhz
#define URTLS_CTE_FIRST_SAMPLE_IDX_SLOT_1US            (37)   //1us start samples index for sample rate 1Mhz,2Mhz and 3Mhz
#define URTLS_CTE_FIRST_SAMPLE_IDX_SLOT_2US            (45)   //2us start samples index for sample rate 1Mhz,2Mhz and 3Mhz
#define URTLS_CTE_FIRST_SAMPLE_IDX_SLOT_1US_4MHZ       (36)   //1us start samples index for sample rate 4Mhz
#define URTLS_CTE_FIRST_SAMPLE_IDX_SLOT_2US_4MHZ       (44)   //2us start samples index for sample rate 4Mhz

// CTE report event samples offsets
#define URTLS_CTE_SAMPLE_JUMP_REF_PERIOD               (4)    //peek 1 sample every 4 samples
#define URTLS_CTE_SAMPLE_JUMP_SLOT_1US                 (8)    //peek 1 sample every 8 samples
#define URTLS_CTE_SAMPLE_JUMP_SLOT_2US                 (16)   //peek 1 sample every 16 samples

#define URTLS_CTE_MAX_SAMPLES_PER_EVENT                (96)   //max samples data length in one event
#define URTLS_CTE_MAX_RF_BUFFER_SIZE                   (512)  //first buffer size (MCE RAM)
#define URTLS_CTE_MAX_RF_EXT_BUFFER_SIZE               (512)  //second buffer size (RFE RAM)
#define URTLS_CTE_SAMPLE_RATE_4MHZ                     (4)
#define URTLS_CTE_SAMPLE_RATE_1MHZ                     (1)

// CM0 FW Parameters
#define CM0_RAM_BASE                                   (0x21000028)

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

  //CTE struct
  urtls_cteSamples_t urtls_cteSamples = {0};
  urtls_cteInfo_t    urtls_cteInfo[UBLE_MAX_MONITOR_HANDLE];

/*********************************************************************
 * LOCAL VARIABLES
 */

  // User Application callback
  pfnAppEventHandlerCB_t gAppCB = NULL;

  // Pin Handle
  PIN_Handle gPinHandle = NULL;

  // Number of CTE Sampling Buffers
  uint8_t gMaxNumBuffers = MAX_NUM_CTE_BUFS;

#if !defined(AOA_SYSCFG)

  // Antenna board configurations (example for a 12-antenna board)
  // Maximum number of antennas
  #define ANTENNA_TABLE_SIZE                             (12)
  // BitMask of all the relevant GPIOs which needed for the antennas
  #define ANTENNA_IO_MASK                                BV(27)|BV(28)|BV(29)|BV(30)

  // Antenna GPIO configuration (should be adapted to the antenna board design)
  uint32_t antennaTbl[] =
  {
   0,                        // antenna 0 GPIO configuration (all GPIOs in ANTENNA_IO_MASK are LOW)
   BV(28),                   // antenna 1
   BV(29),                   // antenna 2
   BV(28) | BV(29),          // antenna 3
   BV(30),                   // antenna 4
   BV(28) | BV(30),          // antenna 5
   BV(27),                   // antenna 6
   BV(27) | BV(28),          // antenna 7
   BV(27) | BV(29),          // antenna 8
   BV(27) | BV(28) | BV(29), // antenna 9
   BV(27) | BV(30),          // antenna 10
   BV(27) | BV(28) | BV(30)  // antenna 11
  };
#endif // !defined(AOA_SYSCFG)

  // Antenna properties passes to the stack
  cteAntProp_t cteAntProp = {ANTENNA_IO_MASK,
                             ANTENNA_TABLE_SIZE,
                             antennaTbl};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

bStatus_t urtls_setupCteSamplesEntryQueue( uint8_t numBuffers );
bStatus_t urtls_setCteAntennaArray(urtls_cteAntSwitch_t *pAntenna, uint8_t *pPattern, uint8_t lenPattern, uint8_t slotDurations);
void urtls_connectionIqReportEvent(uint8_t  sessionId,
                                          uint8_t  dataChIndex,
                                          uint16_t rssi,
                                          uint8_t  rssiAntenna,
                                          uint8_t  cteType,
                                          uint8_t  slotDuration,
                                          uint8_t  status,
                                          uint16_t sampleCount,
                                          uint8_t  sampleRate,
                                          uint8_t  sampleSize,
                                          uint8_t  sampleCtrl,
                                          uint32_t *cteData);
void urtls_setCteSamples(uint16_t  sampleCount,
                           uint8_t  sampleSlot,
                           uint8_t  sampleRate,
                           uint8_t  sampleSize,
                           uint8_t  sampleCtrl,
                           uint32_t *src,
                           int8_t   *iqSamples);


/*********************************************************************
 * @fn      urtls_register
 *
 * Register callback functions with urtls Services
 *
 * @param   pCB pointer to callback function structure
 *
 * @return  SUCCESS/FAILURE
 */
bStatus_t urtls_register(pfnAppEventHandlerCB_t pCB)
{
  if (pCB != NULL)
  {
    gAppCB = pCB;

    return SUCCESS;
  }
  else
  {
    return FAILURE;
  }
}

/*********************************************************************
 * @fn          urtls_callAppCb
 *
 * @brief       Allocate and send a message to the application
 *
 * @param       evtType - opcode of the event
 * @param       evtSize - size of evtData
 * @param       evtData - actual information to pass
 *
 * @return      TRUE = success, FALSE = failure
 */
bStatus_t urtls_callAppCb(uint8_t evtType, uint16_t evtSize, uint8_t *pEvtData)
{
  urtls_evt_t *pEvt;

  // No point in doing anything if no user callback
  if (gAppCB == NULL)
  {
    return FALSE;
  }

  // Allocate the event - will be freed by the user application
  pEvt = (urtls_evt_t *)urtls_malloc(sizeof(urtls_evt_t));

  if (pEvt == NULL)
  {
    return FALSE;
  }

  // Load event info
  pEvt->evtType = evtType;
  pEvt->evtSize = evtSize;
  pEvt->evtData = pEvtData;

  // Call user registered callback
  gAppCB((uint8_t *)pEvt);

  return TRUE;
}

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
void* urtls_malloc(uint32_t sz)
{
  void *pPointer;

  URTLS_MALLOC(pPointer, sz);

  if (pPointer == NULL)
  {
    return NULL;
  }

  return pPointer;
}

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
void urtls_rfOverrideCteValue(uint32_t val, uint16_t address, uint8_t offset)
{
  uint32 *pOverride;

  if ((uint32 *)ubRfRegOverride != NULL)
  {
    // Get pointer to CTE overrides
    pOverride = (uint32 *)ubRfRegOverride;

    // Write the override (will be applied when CM0 powercycles)
    *(pOverride + offset)  = val;

    // Handle the case where CM0 might never powercycle (overrides won't be re-read)
    // Just write directly to CM0 memory (has no effect if CM0 is asleep, which is fine)
    HWREG(CM0_RAM_BASE + address) = val;
  }
}

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
void urtls_freeConn(uint8_t sessionId, uint8_t numHandles)
{
  // Free allocated memory of Antenna switch pattern struct
  if (urtls_cteInfo[sessionId-1].pAntenna != NULL)
  {
    URTLS_FREE(urtls_cteInfo[sessionId-1].pAntenna);
  }

  // Free allocated memory of auto copy buffers
  if((numHandles == 0) && (urtls_cteSamples.pAutoCopyBuffers != NULL))
  {
    URTLS_FREE(urtls_cteSamples.pAutoCopyBuffers);
  }
}

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
PIN_Handle urtls_initAntArray(uint8_t mainAntenna)
{
  uint32_t enAntMask;
  uint32_t pinCfg;
  PIN_State pinState;
  uint8_t maskCounter = 0;
  uint32_t mainIoEntry;

  // If we already have a handle, just return it
  if (gPinHandle != NULL)
  {
    return gPinHandle;
  }

  pinCfg = PIN_TERMINATE;

  // Check that mainAntenna is one of the antennas configured in the board configuration
  if (mainAntenna > cteAntProp.antPropTblSize)
  {
    return NULL;
  }
  else
  {
    mainIoEntry = cteAntProp.antPropTbl[mainAntenna];
  }

  // Get antenna mask configured by the user
  enAntMask = cteAntProp.antMask;

  // Check if PIN handle already exists
  if (gPinHandle == NULL)
  {
    gPinHandle = PIN_open(&pinState, &pinCfg);
  }

  // Setup pins in Antenna Mask
  while (enAntMask)
  {
    if (enAntMask & 0x1)
    {
      pinCfg = maskCounter | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_INPUT_DIS | PIN_DRVSTR_MED;

      if (PIN_add(gPinHandle, pinCfg) != PIN_SUCCESS)
      {
        PIN_close(gPinHandle);
        return NULL;
      }
    }

    // Setup main antenna (switch relevant pins to high)
    if (mainIoEntry & 0x1)
    {
      PIN_setOutputValue(gPinHandle, maskCounter, 1);
    }

    maskCounter++;
    enAntMask>>=1;
    mainIoEntry>>=1;
  }

  return gPinHandle;
}

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
                                        uint8_t *pAntenna)
{
  // check parameter
  if (((samplingEnable != FALSE) && (samplingEnable != TRUE)) ||
      ((samplingEnable == TRUE) &&
     (((slotDurations != URTLS_CTE_SAMPLE_SLOT_1US) && (slotDurations != URTLS_CTE_SAMPLE_SLOT_2US)) ||
       (length < URTLS_CTE_ANTENNA_LIST_MIN_LENGTH) ||
       (length > URTLS_CTE_ANTENNA_LIST_MAX_LENGTH))))
  {
    return URTLS_FAIL;
  }

  if (samplingEnable == TRUE)
  {
    //Set the iq auto copy
    if (urtls_cteSamples.pAutoCopyBuffers == NULL)
    {
      // Set the queue pointer
      if (urtls_setupCteSamplesEntryQueue(gMaxNumBuffers) != URTLS_SUCCESS)
      {
        return URTLS_FAIL;
      }
    }

    // Delete old antenna list
    if (urtls_cteInfo[sessionId-1].pAntenna != NULL)
    {
      URTLS_FREE(urtls_cteInfo[sessionId-1].pAntenna);
    }

    // Allocate the antenna switch pattern struct
    urtls_cteInfo[sessionId-1].pAntenna = (urtls_cteAntSwitch_t *)urtls_malloc( sizeof(urtls_cteAntSwitch_t) + (sizeof(uint32_t) * (length - 1)) );

    if (urtls_cteInfo[sessionId-1].pAntenna )
    {
      // Set antenna array
      if (urtls_setCteAntennaArray(urtls_cteInfo[sessionId-1].pAntenna, pAntenna, length, slotDurations) != URTLS_SUCCESS)
      {
        return URTLS_FAIL;
      }
      urtls_cteInfo[sessionId-1].recvInfo.rssiAntenna = pAntenna[0];
    }
    else
    {
      return URTLS_FAIL;
    }
    urtls_cteInfo[sessionId-1].samplingEnable = URTLS_CTE_SAMPLING_ENABLE;
  }
  else if (urtls_cteInfo[sessionId-1].samplingEnable == URTLS_CTE_SAMPLING_ENABLE)
  {
    urtls_cteInfo[sessionId-1].samplingEnable = URTLS_CTE_SAMPLING_DISABLE;
  }

  return URTLS_SUCCESS;
}

/*******************************************************************************
 * @fn          urtls_setupCteSamplesEntryQueue
 *
 * @brief       This routine is used to setup a queue with single buffer queue
 *              dynamically to receive a CTE samples packet.
 *
 * @param       numBuffers - number of auto copy buffers to allocate (between 1 to 16)
 *                           in connection mode this value is 1.
 *
 * @return      TRUE or FALSE
 */
bStatus_t urtls_setupCteSamplesEntryQueue( uint8_t numBuffers )
{
  dataEntry_t *pEntryQ;
  uint8_t *pBuffer;
  uint16_t bufSize;

  if (numBuffers > URTLS_CTE_COUNT_MAX)
  {
    return URTLS_FAIL;
  }

  urtls_cteSamples.autoCopyCompleted = 0;
  //Report samples regardless of CRC result
  urtls_cteSamples.autoCopy.samplesConfig.bFlushCrcErr = 0;
  //Do not report samples from packets with invalid CTEInfo
  urtls_cteSamples.autoCopy.samplesConfig.bFlushCteInfoErr = 1;
  //Report samples from AoA packets
  urtls_cteSamples.autoCopy.samplesConfig.bFlushAoa = 0;
  //Do not Report samples from AoD packets - default setting
  //will be overwrite in case of CTE test mode
  urtls_cteSamples.autoCopy.samplesConfig.bFlushAod1us = 1;
  urtls_cteSamples.autoCopy.samplesConfig.bFlushAod2us = 1;
  //Report gain as single-bit value in status field only
  urtls_cteSamples.autoCopy.samplesConfig.bIncludeRfGain = 1;
  //Report RSSI in status field
  urtls_cteSamples.autoCopy.samplesConfig.bIncludeRssi = 1;
  //Minimum value of CTETime for packets to report
  urtls_cteSamples.autoCopy.minReportSize = URTLS_CTE_MIN_LEN;
  //Maximum value of CTETime for packets to report
  urtls_cteSamples.autoCopy.maxReportSize = URTLS_CTE_MAX_LEN;
  //Disable the CTE limit counter
  urtls_cteSamples.autoCopy.cteCopyLimitCount = 0xFF;
  //allocate the RF IQ samples buffer = ~2.5kb
  // buffer size = 32 bit size * max sample rate *((max cte Length * 8) - CTE_OFFSET)
  bufSize = (sizeof(dataEntry_t) + sizeof(urtls_cteSamplesRfHeader_t) + (sizeof(uint32_t) * URTLS_CTE_NUM_RF_SAMPLES(URTLS_CTE_MAX_LEN)));
  // allocate the complete memory - theoretically could be up to ~2.5KB * 16 = ~40KB
  urtls_cteSamples.pAutoCopyBuffers = (dataEntry_t *)urtls_malloc( numBuffers * bufSize );

  if (urtls_cteSamples.pAutoCopyBuffers == NULL)
  {
    return URTLS_FAIL;
  }
  pEntryQ = urtls_cteSamples.pAutoCopyBuffers;
  pBuffer = (uint8_t *)urtls_cteSamples.pAutoCopyBuffers;
  for (uint8_t i=0; i < numBuffers; i++)
  {
    pEntryQ->status     = DATASTAT_PENDING;
    pEntryQ->config     = DATA_ENTRY_TYPE_GENERAL | DATA_ENTRY_LEN_SIZE_2;
    pEntryQ->length     = bufSize;
    // last entry will points to the first - in case of one entry, it will point to itself.
    pEntryQ->pNextEntry = (i+1 < numBuffers)?(dataEntry_t *)(pBuffer + bufSize):urtls_cteSamples.pAutoCopyBuffers;
    pBuffer += bufSize;
    pEntryQ = (dataEntry_t *)pBuffer;
  }
  // initialize data queue
  urtls_cteSamples.queue.dataEntryQ.pCurEntry  = urtls_cteSamples.pAutoCopyBuffers;
  urtls_cteSamples.queue.dataEntryQ.pLastEntry = NULL;
  urtls_cteSamples.queue.pNextDataEntry        = urtls_cteSamples.pAutoCopyBuffers;
  urtls_cteSamples.queue.pTempDataEntry        = NULL;

  // set the queue pointer
  urtls_cteSamples.autoCopy.pSamplesQueue = (uint32_t *)&urtls_cteSamples.queue.dataEntryQ;

  return URTLS_SUCCESS;
}

/*******************************************************************************
 * @fn          urtls_setCteAntennaArray
 *
 * @brief       This function sets the CTE antenna array which will be used by the RF Core
 *
 * @param       pAntenna - pointer to antenna array which will be used by the RF
 * @param       pPattern - pointer to antenna switching pattern received from Host
 * @param       lenPattern - length of antenna switching pattern
 * @param       slotDurations - switching and sampling slots in 1 us or 2 us
 *
 * @return      URTLS_SUCCESS or URTLS_FAIL
 */
bStatus_t urtls_setCteAntennaArray(urtls_cteAntSwitch_t *pAntenna, uint8_t *pPattern, uint8_t lenPattern, uint8_t slotDurations)
{
  uint8_t i;

  memset( pAntenna, 0, sizeof(urtls_cteAntSwitch_t) + (sizeof(uint32_t) * (lenPattern - 1)) );
  pAntenna->numEntries = lenPattern;
  pAntenna->switchTime = slotDurations;
  pAntenna->ioMask     = cteAntProp.antMask;
  for (i = 0; i < lenPattern; i++)
  {
    // if antenna id is invalid an error should be returned
    if (pPattern[i] > (cteAntProp.antPropTblSize - 1))
    {
      return URTLS_FAIL;
    }
    // configure antenna according to id
    pAntenna->ioEntry[i] = cteAntProp.antPropTbl[pPattern[i]];
  }
  return URTLS_SUCCESS;
}

/*******************************************************************************
 * @fn          urtls_setCteSampleAccuracy
 *
 * @brief       This function used to set CTE accuracy for PHY 1M and 2M
 *
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
 * @return      URTLS_SUCCESS or URTLS_FAIL
 */
bStatus_t urtls_setCteSampleAccuracy(uint8_t  sessionId,
                                     uint8_t  sampleRate1M,
                                     uint8_t  sampleSize1M,
                                     uint8_t  sampleRate2M,
                                     uint8_t  sampleSize2M,
                                     uint8_t  sampleCtrl)
{
  urtls_cteSampleConfig_t *pConfig = NULL;

  // check parameter
  if ((sampleRate1M > CTE_SAMPLING_CONFIG_1MBPS)     ||
      (sampleRate2M > CTE_SAMPLING_CONFIG_2MBPS)     ||
      (sampleSize1M > URTLS_CTE_SAMPLE_SIZE_16BITS)  ||
      (sampleSize2M > URTLS_CTE_SAMPLE_SIZE_16BITS))
  {
    return URTLS_FAIL;
  }

  // check that CTE request is active
  if (urtls_cteInfo[sessionId-1].requestEnable == TRUE)
  {
    return URTLS_FAIL;
  }
  pConfig = &urtls_cteInfo[sessionId-1].sampleConfig;

  // Special setting when CTE_SAMPLING_CONTROL_RF_RAW_NO_FILTERING is set
  if (sampleCtrl & URTLS_CTE_SAMPLING_CONTROL_RF_RAW_NO_FILTERING)
  {
    // Force sampleRate=4 and sampleSize=2
    sampleRate1M = CTE_SAMPLING_CONFIG_4MHZ;
    sampleSize1M = URTLS_CTE_SAMPLE_SIZE_16BITS;
    sampleRate2M = CTE_SAMPLING_CONFIG_4MHZ;
    sampleSize2M = URTLS_CTE_SAMPLE_SIZE_16BITS;
  }

  //set the accuracy
  if (sampleRate1M > 0)
  {
    pConfig->sampleRate1M = sampleRate1M;
  }
  if (sampleRate2M > 0)
  {
    pConfig->sampleRate2M = sampleRate2M;
  }
  if (sampleSize1M > 0)
  {
    pConfig->sampleSize1M = sampleSize1M;
  }
  if (sampleSize2M > 0)
  {
    pConfig->sampleSize2M = sampleSize2M;
  }

  pConfig->sampleCtrl = sampleCtrl;

  return URTLS_SUCCESS;
}

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
                                           uint8_t type)
{
  // we do not support AoD yet!
  if ((type == URTLS_CTE_TYPE_AOD_1US) || (type == URTLS_CTE_TYPE_AOD_2US))
  {
    return URTLS_FAIL;
  }

  // check parameter
  if ( ((enable != FALSE) && (enable != TRUE)) ||
       ((length < URTLS_CTE_MIN_LEN) || (length > URTLS_CTE_MAX_LEN))  ||
       (type > URTLS_CTE_TYPE_AOD_2US))
  {
    return URTLS_FAIL;
  }

  // check that already enabled
  if ((urtls_cteInfo[sessionId-1].requestEnable == TRUE) && (enable == TRUE))
  {
    return URTLS_FAIL;
  }

  // check if the Host sets the CTE receive params
  if (urtls_cteInfo[sessionId-1].samplingEnable == URTLS_CTE_SAMPLING_NOT_INIT)
  {
    return URTLS_FAIL;
  }

  // check that the interval is a non-zero value
  if ((enable == TRUE) && (interval == 0))
  {
    return URTLS_FAIL;
  }

  urtls_cteInfo[sessionId-1].requestEnable = enable;

  if (enable == TRUE)
  {
    urtls_cteInfo[sessionId-1].requestInterval = interval;
    urtls_cteInfo[sessionId-1].requestLen = length;
    urtls_cteInfo[sessionId-1].requestType = type;
  }

  return URTLS_SUCCESS;
}

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

bStatus_t urtls_getCteInfo(dataEntry_t *pDataEntry, uint8_t sessionId, uint8_t channel)
{
  uint16_t sampleCount;
  uint8_t sampleRate;
  uint8_t sampleSize;
  uint8_t rawRfMode;
  uint8_t switchTime;
  urtls_cteSamplesRfHeader_t *samplesHdr;
  uint8_t *samplesData;
  uint8_t cteLen;
  uint8_t cteType;

  if ((urtls_cteSamples.autoCopyCompleted == 0) || (urtls_cteSamples.pAutoCopyBuffers == NULL) ||
      (pDataEntry == NULL) || (pDataEntry->status != DATASTAT_FINISHED))
  {
    if (urtls_cteInfo[sessionId-1].recvCte == TRUE)
    {
      urtls_cteInfo[sessionId-1].recvCte = FALSE;
    }
   return URTLS_FAIL;
  }

  // decrease number of completed buffers
  urtls_cteSamples.autoCopyCompleted--;
  // set the samples pointers
  samplesData = (uint8_t *)(pDataEntry + 1);
  samplesHdr = (urtls_cteSamplesRfHeader_t *)(samplesData);
  samplesData += sizeof(urtls_cteSamplesRfHeader_t);
  cteLen = (samplesHdr->cteInfo & URTLS_CTE_INFO_TIME_MASK);
  cteType = (samplesHdr->cteInfo & URTLS_CTE_INFO_TYPE_MASK) >> URTLS_CTE_INFO_TYPE_OFFSET;

  if (urtls_cteInfo[sessionId-1].recvCte == TRUE)
  {
    // validate the received CTE information
    if ((urtls_cteInfo[sessionId-1].samplingEnable != URTLS_CTE_SAMPLING_ENABLE) ||
        (urtls_cteInfo[sessionId-1].recvInfo.length < URTLS_CTE_MIN_LEN) ||
        (urtls_cteInfo[sessionId-1].recvInfo.length > URTLS_CTE_MAX_LEN))
    {
      return URTLS_FAIL;
    }

    // validate the received CTE information against the RF information
    if ((urtls_cteInfo[sessionId-1].recvInfo.length != cteLen)       ||
        (urtls_cteInfo[sessionId-1].recvInfo.type != cteType)        ||
        (sizeof(uint32) * URTLS_CTE_NUM_RF_SAMPLES(cteLen) != samplesHdr->length))
    {
      return URTLS_FAIL;
    }
    // setup the report information and prepare the RAMs for reading
    urtls_cteInfo[sessionId-1].recvInfo.rssi = (int16_t)(samplesHdr->rssi | 0xFF00);
    urtls_cteInfo[sessionId-1].recvInfo.packetStatus = samplesHdr->status.status.bCrcErr;
    urtls_cteInfo[sessionId-1].recvInfo.dataChIndex = channel;

    // sample rate is default 1Mhz or defined by command
    sampleRate = urtls_cteInfo[sessionId-1].sampleConfig.sampleRate1M;
    // sample size is default 8 bits or defined by command
    sampleSize = urtls_cteInfo[sessionId-1].sampleConfig.sampleSize1M;

    // if we are in RF RAW mode then we need to also take the switching period samples into account
    if (urtls_cteInfo[sessionId-1].sampleConfig.sampleCtrl & URTLS_CTE_SAMPLING_CONTROL_RF_RAW_NO_FILTERING)
    {
      // take length and switching into account due to inclusion of switching period samples
      // we want ALL RF samples so sampleCount needs to match accordingly
      rawRfMode = 2;
      switchTime = URTLS_CTE_SAMPLE_SLOT_1US;
    }
    else if (cteType == URTLS_CTE_TYPE_AOA)
    {
      rawRfMode = 1;
      switchTime = urtls_cteInfo[sessionId-1].pAntenna->switchTime;
    }
    else
    {
      return URTLS_FAIL;
    }

    // calculate the number of samples
    // if rawRfMode is enabled then:
    // 1. the length is doubled since we are also outputting the switching period samples
    // 2. switchTime is normalized since it does not play a factor in the amount of samples
    sampleCount = CTE_REFERENCE_PERIOD + rawRfMode*(((urtls_cteInfo[sessionId-1].recvInfo.length * 8) -
                 (CTE_OFFSET + CTE_REFERENCE_PERIOD)) / (switchTime * 2));

   urtls_connectionIqReportEvent(sessionId,
                                 urtls_cteInfo[sessionId-1].recvInfo.dataChIndex,
                                 urtls_cteInfo[sessionId-1].recvInfo.rssi,
                                 urtls_cteInfo[sessionId-1].recvInfo.rssiAntenna,
                                 urtls_cteInfo[sessionId-1].recvInfo.type,
                                 switchTime,
                                 urtls_cteInfo[sessionId-1].recvInfo.packetStatus,
                                 sampleCount,
                                 sampleRate,
                                 sampleSize,
                                 urtls_cteInfo[sessionId-1].sampleConfig.sampleCtrl,
                                 (uint32_t *)samplesData);

  }
  return URTLS_SUCCESS;
}

/*******************************************************************************
 * @fn          urtls_connectionIqReportEvent
 *
 * @brief       This function is used to generate a I/Q CTE report event.
 *
 * @param       sessionId     - Session ID.
 * @param       dataChIndex   - Index of the data channel.
 * @param       rssi          - RSSI value of the packet
 * @param       rssiAntenna   - ID of the antenna on which the RSSI was measured
 * @param       cteType       - CTE type (0-AoA, 1-AoD with 1us, 2-AoD with 2us)
 * @param       slotDuration  - Switching and sampling slots (1 - 1us, 2 - 2us)
 * @param       status        - packet status:
 *                              0 - CRC was correct
 *                              1 - CRC was incorrect
 * @param       sampleCount   - number of samples including the 8 reference period
 * @param       sampleRate    - number of samples per 1us represent CTE accuracy
 *                              range : 1 - least accuracy (as in 5.1 spec) to 4 - most accuracy
 * @param       sampleSize    - sample size represent CTE accuracy
 *                              range : 1 - 8 bit (as in 5.1 spec) or 2 - 16 bits (most accurate)
 * @param       sampleCtrl    - 1 : RF RAW mode 2: Filtered mode (switching period omitted)
 * @param       cteData       - RF buffer which hold the samples
 *
 * @return      None.
 */
void urtls_connectionIqReportEvent(uint8_t  sessionId,
                                        uint8_t  dataChIndex,
                                        uint16_t rssi,
                                        uint8_t  rssiAntenna,
                                        uint8_t  cteType,
                                        uint8_t  slotDuration,
                                        uint8_t  status,
                                        uint16_t sampleCount,
                                        uint8_t  sampleRate,
                                        uint8_t  sampleSize,
                                        uint8_t  sampleCtrl,
                                        uint32_t *cteData)
{
  urtls_connectionIQReport_t *pUrtlsEvt;
  uint8_t evtStatus = FALSE;

  // case the source buffers are NULLs - set the samples count to proper value
  if (cteData == NULL)
  {
    sampleCount = 0;
  }

  // set the total samples includes over sampling
  sampleCount *= (sampleRate * sampleSize);

  pUrtlsEvt = (urtls_connectionIQReport_t *)urtls_malloc(sizeof(urtls_connectionIQReport_t));

  if (pUrtlsEvt  == NULL)
  {
    // We could not allocate, return that we failed
    return;
  }
  pUrtlsEvt->iqSamples = (int8_t *)urtls_malloc(sampleCount*2);

  if (pUrtlsEvt->iqSamples == NULL)
  {
    // We could not allocate, return that we failed and free previously allocated data
    URTLS_FREE(pUrtlsEvt);
    return;
  }

  // Set parameters for urtls I/Q report event
  pUrtlsEvt->sessionId    = sessionId;
  pUrtlsEvt->cteType      = cteType;
  pUrtlsEvt->dataChIndex  = dataChIndex;
  pUrtlsEvt->rssi         = rssi;
  pUrtlsEvt->rssiAntenna  = rssiAntenna;
  pUrtlsEvt->sampleCount  = sampleCount;
  pUrtlsEvt->sampleCtrl   = sampleCtrl;
  pUrtlsEvt->sampleRate   = sampleRate;
  pUrtlsEvt->sampleSize   = sampleSize;
  pUrtlsEvt->slotDuration = slotDuration;
  pUrtlsEvt->status       = status;

  // Adjust amount of samples in case sample size is 16 bit
  if (sampleSize == URTLS_CTE_SAMPLE_SIZE_16BITS)
  {
    pUrtlsEvt->sampleCount /= 2;
  }

  // copy IQ samples
  urtls_setCteSamples(sampleCount,
                      slotDuration,
                      sampleRate,
                      sampleSize,
                      sampleCtrl,
                      cteData,
                      pUrtlsEvt->iqSamples);

  // Send event to user registered callback
  evtStatus = urtls_callAppCb(URTLS_CONNECTION_CTE_IQ_REPORT_EVT, sizeof(urtls_connectionIQReport_t), (uint8_t *)pUrtlsEvt);
  if (evtStatus == FALSE)
  {
    // We could not allocate, return that we failed and free previously allocated data
    URTLS_FREE(pUrtlsEvt->iqSamples);
    URTLS_FREE(pUrtlsEvt);
  }

  return;
}

/*******************************************************************************
 * @fn          urtls_setCteSamples
 *
 * @brief       This function is used to truncate and copy CTE samples
 *
 * @param       sampleCount -Number of samples
 * @param       sampleSlot - 1 us or 2 us
 * @param       sampleRate - 1Mhz to 4Mhz
 * @param       sampleSize - 8 bits or 16 bits
 * @param       sampleCtrl - Default filtering or RAW_RF(no_filtering)
 * @param       src -        Source which keep the samples in 16 bits per I sample and
 *                           16 bits per Q sample (32 bits per sample)
 * @param       iqSamples -  Destination buffer which will keep the IQ samples
 *
 *              Each destination buffer size should be (sizeof (int8) * sampleCount)
 *
 * @return      None
 */
void urtls_setCteSamples(uint16_t  sampleCount,
                         uint8_t  sampleSlot,
                         uint8_t  sampleRate,
                         uint8_t  sampleSize,
                         uint8_t  sampleCtrl,
                         uint32_t *src,
                         int8_t   *iqSamples)
{
  int16_t  sample[2];  //divide the sample for I sample and Q sample
  uint16_t maxVal = 0;
  int32_t  absValQ = 0;
  int32_t  absValI = 0;
  uint16_t sampleNum;  // antenna sample number
  uint16_t sampleIdx;  // sample index in the src buffer
  uint8_t  shift = 0;
  uint8_t  firstSampleIndex;
  uint8_t  firstRefSampleIndex;
  uint8_t  iterate,j;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///// RAW_RF mode - copy samples without filtering, sampleRate/sampleSize/slotDuration are forced to 4/2/1  /////
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // handle RF_RAW mode
  if (sampleCtrl & URTLS_CTE_SAMPLING_CONTROL_RF_RAW_NO_FILTERING)
  {
    int16_t *pIQ = (int16_t *)iqSamples;

    // go over the antenna samples (divide count by 2 to compensate for 16bit size)
    for (sampleNum = 0; sampleNum < sampleCount/2; sampleNum++)
    {
      // find sample index in src buffer according to the samples offset without skipping
      sampleIdx = sampleNum;

      // find in which RAM the sample located and get it in 16 bits
      *(uint32_t *)sample = src[sampleIdx];

      pIQ[sampleNum * 2] = sample[1];
      pIQ[sampleNum  * 2 + 1] = sample[0];
    }
    // return updated samples offset
    return;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///// Not RAW mode - switching period/idle period samples are filtered out according to the BT5.1 Spec /////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // normalize the number of samples
  // each sample can consist of up to 4 samples and 2 byes size
  // sample count should be aligned to sample rate and sample size
  sampleCount /= (sampleRate * sampleSize);

  //find the first sample index
  if (sampleRate == URTLS_CTE_SAMPLE_RATE_4MHZ)
  {
    firstRefSampleIndex = URTLS_CTE_FIRST_SAMPLE_IDX_REF_PERIOD_4MHZ;
    firstSampleIndex = (sampleSlot == URTLS_CTE_SAMPLE_SLOT_1US)?
                        URTLS_CTE_FIRST_SAMPLE_IDX_SLOT_1US_4MHZ:
                        URTLS_CTE_FIRST_SAMPLE_IDX_SLOT_2US_4MHZ;
  }
  else
  {
    firstRefSampleIndex = URTLS_CTE_FIRST_SAMPLE_IDX_REF_PERIOD;
    firstSampleIndex = (sampleSlot == URTLS_CTE_SAMPLE_SLOT_1US)?
                        URTLS_CTE_FIRST_SAMPLE_IDX_SLOT_1US:
                        URTLS_CTE_FIRST_SAMPLE_IDX_SLOT_2US;
  }

  // in case sample size is 8 bits - execute the loop twice:
  // 1) find the max value
  // 2) normalize the samples and copy
  // in case the sample size is 16 bits - run only the second iteration, no need to normalize (RF samples are represented as 16 bits)
  for (iterate = (sampleSize - 1); iterate < 2; iterate++)
  {
    // go over the antenna samples
    for (sampleNum = 0; sampleNum < sampleCount ; sampleNum++)
    {
      // find sample index in src buffer according to the samples offset
      if (sampleNum < URTLS_CTE_SAMPLES_COUNT_REF_PERIOD)
      {
       sampleIdx = (sampleNum * URTLS_CTE_SAMPLE_JUMP_REF_PERIOD) + firstRefSampleIndex;
      }
      else
      {
        if (sampleSlot == URTLS_CTE_SAMPLE_SLOT_1US)
        {
          sampleIdx = ((sampleNum - URTLS_CTE_SAMPLES_COUNT_REF_PERIOD) * URTLS_CTE_SAMPLE_JUMP_SLOT_1US) + firstSampleIndex;
        }
        else
        {
          sampleIdx = ((sampleNum - URTLS_CTE_SAMPLES_COUNT_REF_PERIOD) * URTLS_CTE_SAMPLE_JUMP_SLOT_2US) + firstSampleIndex;
        }
      }

      // each antenna sample can consist of up to 4 samples - depend on the sample rate
      for (j = 0; j < sampleRate; j++)
      {
        // find in which RAM the sample located and get it in 16 bits
        *(uint32 *)sample = src[sampleIdx + j];

        // first iteration - find the max value
        // relevant only when sample size is 8 bits
        if (iterate == 0)
        {
          // Get abs of Q
          absValQ = sample[0];

          if (absValQ < 0)
          {
            absValQ = (-1)*absValQ;
          }

          // Get abs of I
          absValI = sample[1];

          if (absValI < 0)
          {
            absValI = (-1)*absValI;
          }

          // Check if abs of I is bigger than the max value we found
          if (absValI > maxVal)
          {
            maxVal = absValI;
          }

          // Check if abs of Q is bigger than the max value we found
          if (absValQ > maxVal)
          {
            maxVal = absValQ;
          }
        }
        else // normalize the sample and copy it
        {
          // case of 8 bits sample size as 5.1 spec definition
          if (sampleSize == URTLS_CTE_SAMPLE_SIZE_8BITS)
          {
            // copy the Q sample
            iqSamples[(sampleNum * sampleRate * 2) + j + 1] = (int8)(sample[0] / (1<<shift));

            // copy the I sample
            iqSamples[(sampleNum * sampleRate * 2) + j] = (int8)(sample[1] / (1<<shift));
          }
          else // case of 16 bits sample size
          {
            int16 *pIQ = (int16 *)iqSamples;

            // copy Q sample
            pIQ[(sampleNum * sampleRate * 2) + (2 * j) + 1] = sample[0];

            // copy I sample
            pIQ[(sampleNum * sampleRate * 2) + (2 * j)] = sample[1];
          }
        }
      }
    }

    // first iteration - find the shift value
    // relevant only when sample size is 8 bits
    if (iterate == 0)
    {
      // find shifting value according to the max value sample
      if (maxVal < 0x80)
      {
        shift = 0;
      }
      else
      {
        shift = 8;
        maxVal >>= 8;
        if (maxVal < 0x08)
        {
          shift -= 4;
        }
        else
        {
          maxVal >>= 4;
        }
        if (maxVal < 0x02)
        {
          shift -= 2;
        }
        else
        {
          maxVal >>= 2;
        }
        if (maxVal < 0x01)
        {
          shift -= 1;
        }
      }
    }
  }
  return;
}

/*********************************************************************
*********************************************************************/
