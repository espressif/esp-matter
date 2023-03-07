/******************************************************************************

 @file  ull.c

 @brief This file contains the Micro Link Layer (uLL) API for the Micro
        BLE Stack.

        This API is mostly based on the Bluetooth Core Specification,
        V4.2, Vol. 6.

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
 * INCLUDES
 */
#include <stdlib.h>

#include <driverlib/ioc.h>

#include <icall.h>

#include <driverlib/rf_mailbox.h>
#include <driverlib/rf_ble_mailbox.h>
#include <ti/drivers/rf/RF.h>
#include <ll_common.h>
#include <bcomdef.h>
#include <util.h>

#include <port.h>
#include <uble.h>
#include <urfi.h>
#include <ull.h>

#if defined(RTLS_CTE)
#if !defined(DeviceFamily_CC26X1)
#include <driverlib/rf_bt5_iq_autocopy.h>
#else
#include <ti/devices/cc13x2_cc26x2/driverlib/rf_bt5_iq_autocopy.h>
#endif
#include <urtls.h>
#endif /* RTLS_CTE */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/* uLL State */
#define ULL_STATE_STANDBY               0
#define ULL_STATE_ADVERTISING           1
#define ULL_STATE_SCANNING              2
#define ULL_STATE_MONITORING            3

/* Clock Event for about to advertising */
#define ULL_CLKEVT_ADV_ABOUT_TO         1
/* Clock event for advertisement interval */
#define ULL_CLKEVT_ADV_INT_EXPIRED      2

/* Time gap in RAT ticks between adv commands in the same command chain */
#define ULL_ADV_GAP_IN_CHAIN  ((uint32) (374.5 * US_TO_RAT))

/* uLL adv tx status */
#define ULL_ADV_TX_DONE                 1
#define ULL_ADV_TX_SCHEDULED            2
#define ULL_ADV_TX_NO_RF_RESOURCE       3
#define ULL_ADV_TX_FAILED               4

/* uLL advertisement modes */
#define ULL_ADV_MODE_START              1
#define ULL_ADV_MODE_IMMEDIATE          2
#define ULL_ADV_MODE_PERIODIC           3
#define ULL_ADV_MODE_PERIODIC_RELAXED   4
#define ULL_ADV_MODE_RESCHEDULE         5

/* uLL scan rx status */
#define ULL_SCAN_RX_SCHEDULED           1
#define ULL_SCAN_RX_NO_RF_RESOURCE      2
#define ULL_SCAN_RX_FAILED              3

/* uLL scanning modes */
#define ULL_SCAN_MODE_START             1
#define ULL_SCAN_MODE_RESCHEDULE        2

/* uLL monitor rx status */
#define ULL_MONITOR_RX_SCHEDULED        1
#define ULL_MONITOR_RX_NO_RF_RESOURCE   2
#define ULL_MONITOR_RX_FAILED           3

/* uLL moniotr modes */
#define ULL_MONITOR_MODE_START          1
#define ULL_MONITOR_MODE_RESCHEDULE     2

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
/* Micro Link Layer State */
uint8 ulState = ULL_STATE_STANDBY;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern bStatus_t uble_buildAndPostEvt(ubleEvtDst_t evtDst, ubleEvt_t evt,
                                      ubleMsg_t *pMsg, uint16 len);

/*********************************************************************
 * EXTERNAL VARIABLES
 */

extern ubleParams_t ubleParams;
extern rfc_CMD_BLE_ADV_COMMON_t urfiAdvCmd[];
extern uint8 rfTimeCrit;
extern rfc_CMD_BLE_SCANNER_t urfiScanCmd;
extern rfc_CMD_BLE_GENERIC_RX_t urfiGenericRxCmd;

/*********************************************************************
 * LOCAL VARIABLES
 */

#if defined(FEATURE_ADVERTISER)
/* Advertiser callbacks */
static pfnAdvAboutToCB_t ull_notifyAdvAboutTo;
static pfnAdvDoneCB_t    ull_notifyAdvDone;
/* Clock object used for "about to advertise" notification */
static struct port_timerObject_s *cAdvAboutTo;

#if defined(RF_MULTIMODE)
/* Timer object used to track the advertisement interval */
static struct port_timerObject_s *cAdvInt;
#endif /* RF_MULTIMODE */

#endif /* FEATURE_ADVERTISER */

/* Advertisement status */
static uint8 advTxStatus = ULL_ADV_TX_DONE;

#if defined(FEATURE_SCANNER)
/* Scanner callbacks */
static pfnScanIndCB_t ull_notifyScanIndication;
static pfnScanWindowCompCB_t ull_notifyScanWindowComplete;

/* Scan status */
static uint8 scanRxStatus = ULL_SCAN_RX_SCHEDULED;

/* Static Data Entries */
#if defined __TI_COMPILER_VERSION || defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN (ull_scanDataEntry , 4)
#endif
struct
{
  dataEntry_t entry;
  uint8       data[ ULL_ADV_ALIGNED_BUFFER_SIZE ];
} ull_scanDataEntry[ ULL_NUM_RX_SCAN_ENTRIES ];

/* Scan Data Queue */
dataQ_t       ull_scanDataQueue;

uint8 advPkt[ ULL_PKT_HDR_LEN + ULL_MAX_BLE_ADV_PKT_SIZE + ULL_SUFFIX_MAX_SIZE - ULL_SUFFIX_CRC_SIZE];

/**
 * Flow control to allow one RX packet at a time
 */
bool Ull_advPktInuse = false;
#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
/* Monitor callbacks */
static pfnMonitorIndCB_t ull_notifyMonitorIndication;
static pfnMonitorCompCB_t ull_notifyMonitorComplete;

/* Monitor status */
static uint8 monitorRxStatus = ULL_MONITOR_RX_SCHEDULED;

/* Monitor Session ID */
static uint8_t ull_sessionId;

/* Static Data Entries */
#if defined __TI_COMPILER_VERSION || defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN (ull_monitorDataEntry , 4)
#endif
struct
{
  dataEntry_t entry;
  uint8       data[ ULL_BLE_ALIGNED_BUFFER_SIZE ];
} ull_monitorDataEntry[ ULL_NUM_RX_MONITOR_ENTRIES ];

/* Scan Data Queue */
dataQ_t       ull_monitorDataQueue;

uint8 monitorPkt[ ULL_PKT_HDR_LEN + ULL_MAX_BLE_PKT_SIZE + ULL_SUFFIX_MAX_SIZE - ULL_SUFFIX_CRC_SIZE ];

#endif /* FEATURE_MONITOR */

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
bStatus_t ull_advSchedule(uint8 mode);

#if defined(FEATURE_SCANNER)
/*******************************************************************************
 * @fn          ull_setupScanDataEntryQueue
 *
 * @brief       This routine is used to setup a static ring buffer.
 *
 * input parameters
 *
 * @param
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Pointer to the Rx Data Entry Queue.
 */
dataEntryQ_t *ull_setupScanDataEntryQueue( void )
{
  /* init data entry */
  for ( uint8 i=0; i<ULL_NUM_RX_SCAN_ENTRIES; i++ )
  {
    ull_scanDataEntry[i].entry.status     = 0;
    ull_scanDataEntry[i].entry.config     = 0;
    ull_scanDataEntry[i].entry.length     = ULL_ADV_ALIGNED_BUFFER_SIZE;
    ull_scanDataEntry[i].entry.pNextEntry = &ull_scanDataEntry[i+1].entry;
  }

  /* adjust last entry to wrap */
  ull_scanDataEntry[ULL_NUM_RX_SCAN_ENTRIES-1].entry.pNextEntry = &ull_scanDataEntry[0].entry;

  /* init data queue */
  ull_scanDataQueue.dataEntryQ.pCurEntry  = &ull_scanDataEntry[0].entry;
  ull_scanDataQueue.dataEntryQ.pLastEntry = NULL;
  ull_scanDataQueue.pNextDataEntry        = &ull_scanDataEntry[0].entry;
  ull_scanDataQueue.pTempDataEntry        = NULL;

  return( &ull_scanDataQueue.dataEntryQ );
}
#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
/*******************************************************************************
 * @fn          ull_setupMonitorDataEntryQueue
 *
 * @brief       This routine is used to setup a static ring buffer.
 *
 * input parameters
 *
 * @param
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Pointer to the Rx Data Entry Queue.
 */
dataEntryQ_t *ull_setupMonitorDataEntryQueue( void )
{
  /* init data entry */
  for ( uint8 i=0; i<ULL_NUM_RX_MONITOR_ENTRIES; i++ )
  {
    ull_monitorDataEntry[i].entry.status     = 0;
    ull_monitorDataEntry[i].entry.config     = 0;
    ull_monitorDataEntry[i].entry.length     = ULL_BLE_ALIGNED_BUFFER_SIZE;

    if (i != ULL_NUM_RX_MONITOR_ENTRIES - 1)
    {
      ull_monitorDataEntry[i].entry.pNextEntry = &ull_monitorDataEntry[i+1].entry;
    }
    else
    {
      /* adjust last entry to wrap */
      ull_monitorDataEntry[ULL_NUM_RX_MONITOR_ENTRIES-1].entry.pNextEntry = &ull_monitorDataEntry[0].entry;
    }
  }

  /* init data queue */
  ull_monitorDataQueue.dataEntryQ.pCurEntry  = &ull_monitorDataEntry[0].entry;
  ull_monitorDataQueue.dataEntryQ.pLastEntry = NULL;
  ull_monitorDataQueue.pNextDataEntry        = &ull_monitorDataEntry[0].entry;
  ull_monitorDataQueue.pTempDataEntry        = NULL;

  return( &ull_monitorDataQueue.dataEntryQ );
}
#endif /* FEATURE_MONITOR */

#if defined(FEATURE_SCANNER) || defined(FEATURE_MONITOR)
/*******************************************************************************
 * @fn          ull_getNextDataEntry
 *
 * @brief       This function is used to return a pointer to the next data
 *              entry in the data entry queue that is available for System
 *              processing. Note that this does not necessarily mean the data
 *              entry has be Finished by the radio - to determine this, the
 *              data entry status would have to be first checked. This is only
 *              the data entry to would be processed next by System software.
 *
 * input parameters
 *
 * @param       dataEntryQueue_t - Pointer to data entry queue.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      Pointer to next data entry of a data queue to be processed.
 */
dataEntry_t *ull_getNextDataEntry( dataEntryQ_t *pDataEntryQ )
{
  /* return next data entry to may be processed by System software */
  return( (dataEntry_t *)((dataQ_t *)pDataEntryQ)->pNextDataEntry );
}

/*******************************************************************************
 * @fn          ull_nextDataEntryDone
 *
 * @brief       This function is used to mark the next System data entry on a
 *              data entry queue as Pending so that the radio can once again
 *              use it. It should be called after the user has processed the
 *              data entry.
 *
 * input parameters
 *
 * @param       dataEntryQueue_t - Pointer to data entry queue.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void ull_nextDataEntryDone( dataEntryQ_t *pDataEntryQ )
{
  dataQ_t *pDataQueue;
  port_key_t key;

  key = port_enterCS_HW();

  /* point to data queue */
  pDataQueue = (dataQ_t *)pDataEntryQ;

  if ( pDataQueue->pNextDataEntry != NULL )
  {
    /* mark the next System data entry as Pending */
    pDataQueue->pNextDataEntry->status = DATA_ENTRY_PENDING;

    /* advance to the next data entry in the data entry queue */
    pDataQueue->pNextDataEntry = pDataQueue->pNextDataEntry->pNextEntry;

    port_exitCS_HW(key);

    /* return pointer to next entry, or NULL if there isn't one
     * Note: For a ring buffer, there is always another.
     */
    return;
  }

  port_exitCS_HW(key);

  /* return next data entry to may be processed by System software */
  return;
}

#endif /* FEATURE_SCANNER || FEATURE_MONITOR */

/*********************************************************************
 * CALLBACKS
 */

/*********************************************************************
 * @fn      ull_clockHandler
 *
 * @brief   Clock handler function
 *
 * @param   a0 - event
 *
 * @return  none
 */
void ull_clockHandler(UArg a0)
{
#if defined(FEATURE_ADVERTISER)
  /* Process AboutToAdv */
  if (a0 == ULL_CLKEVT_ADV_ABOUT_TO)
  {
    ull_notifyAdvAboutTo();
  }
#ifdef RF_MULTIMODE
  /* Process advertise interval expiry */
  if (a0 == ULL_CLKEVT_ADV_INT_EXPIRED)
  {
    port_key_t key;

    key = port_enterCS_SW();
    uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_ADV_TX_TIMER_EXPIRED, NULL, 0);
    port_exitCS_SW(key);
  }
#endif /* RF_MULTIMODE */
#endif  /* FEATURE_ADVERTISER */
}

/*********************************************************************
 * @fn      ull_advDoneCb
 *
 * @brief   Callback function to be invoked by RF driver
 *
 * @param   rfHandle - RF client handle
 *
 * @param   cmdHandle - RF command handle
 *
 * @param   events - RF events
 *
 * @return  none
 */
#if defined(RF_MULTIMODE)
void ull_advDoneCb(RF_Handle rfHandle, RF_CmdHandle cmdHandle,
                   RF_EventMask events)
{
  port_key_t key;
  uint16_t status;

  key = port_enterCS_SW();

  status = urfiAdvCmd[0].status | urfiAdvCmd[1].status | urfiAdvCmd[2].status;
  if ((events & RF_EventLastCmdDone) && (status == BLE_DONE_OK))
  {
    uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_ADV_TX_SUCCESS, NULL, 0);
  }
  else if (events &
          (RF_EventCmdAborted | RF_EventCmdStopped |
           RF_EventCmdPreempted | RF_EventCmdCancelled))
  {
    if (advTxStatus == ULL_ADV_TX_SCHEDULED)
    {
      uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_ADV_TX_FAILED, NULL, 0);
    }
  }

  port_exitCS_SW(key);
}
#else
void ull_advDoneCb(RF_Handle rfHandle, RF_CmdHandle cmdHandle,
                   RF_EventMask events)
{
  port_key_t key;

  key = port_enterCS_SW();

  if (events & RF_EventLastCmdDone)
  {
    /* Notify uGAP that the last ADV was done successfully. */
    ull_notifyAdvDone(SUCCESS);

    if (ulState == ULL_STATE_ADVERTISING)
    {
      if (SUCCESS != ull_advSchedule(ULL_ADV_MODE_PERIODIC))
      {
        /* Switch to StadnBy state */
        ulState = ULL_STATE_STANDBY;

        port_exitCS_SW(key);

        /* No more scheduling is available */
        ull_notifyAdvDone(bleNoResources);
      }
      else
      {
        port_exitCS_SW(key);
      }
    }
    else
    {
      port_exitCS_SW(key);
    }
  }
  else if (events &
           (RF_EventCmdAborted | RF_EventCmdStopped | RF_EventCmdCancelled))
  {
    /* Switch to StadnBy state */
    ulState = ULL_STATE_STANDBY;

    port_exitCS_SW(key);
  }
  else
  {
    port_exitCS_SW(key);
  }
}
#endif /* RF_MULTIMODE */

#if defined(FEATURE_SCANNER)
/*********************************************************************
 * @fn      ull_scanDoneCb
 *
 * @brief   Callback function to be invoked by RF driver
 *
 * @param   rfHandle - RF client handle
 *
 * @param   cmdHandle - RF command handle
 *
 * @param   events - RF events
 *
 * @return  none
 */
void ull_scanDoneCb(RF_Handle rfHandle, RF_CmdHandle cmdHandle,
                    RF_EventMask events)
{
  port_key_t key;

  key = port_enterCS_SW();

  if (events & RF_EventRxEntryDone)
  {
    if (scanRxStatus == ULL_SCAN_RX_SCHEDULED)
    {
      /* Radio has received a packet */
      uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_SCAN_RX_SUCCESS, NULL, 0);
    }
  }

  /* RF_EventRxEntryDone and RF_EventLastCmdDone can be set at the same time */
  if (events & RF_EventLastCmdDone)
  {
    if (scanRxStatus == ULL_SCAN_RX_SCHEDULED)
    {
      switch(urfiScanCmd.status)
      {
        case BLE_DONE_OK:
        case BLE_DONE_RXTIMEOUT:
        case BLE_DONE_NOSYNC:
        case BLE_DONE_RXERR:
        case BLE_DONE_ENDED:
        case BLE_DONE_ABORT:
        case BLE_DONE_STOPPED:
          /* Operation finished normally */
          break;

        case BLE_ERROR_RXBUF:
          /* No available Rx buffer or received overflowed */
          uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_SCAN_RX_BUF_FULL, NULL, 0);
          break;

        default:
          /* Do not allow further scans */
          ulState = ULL_STATE_STANDBY;

          /* Synth error or other fatal errors */
          uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_SCAN_RX_FAILED, NULL, 0);
          break;
      }
    }
  }
  else if (events & RF_EventInternalError)
  {
    /* Do not allow further scans */
    ulState = ULL_STATE_STANDBY;

    /* Internal fatal errors */
    uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_SCAN_RX_FAILED, NULL, 0);
  }

  port_exitCS_SW(key);
}
#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
/*********************************************************************
 * @fn      ull_monitorDoneCb
 *
 * @brief   Callback function to be invoked by RF driver
 *
 * @param   rfHandle - RF client handle
 *
 * @param   cmdHandle - RF command handle
 *
 * @param   events - RF events
 *
 * @return  none
 */
void ull_monitorDoneCb(RF_Handle rfHandle, RF_CmdHandle cmdHandle,
                       RF_EventMask events)
{
  port_key_t key;

  key = port_enterCS_SW();

  if (events & RF_EventRxEntryDone)
  {
    if (monitorRxStatus == ULL_MONITOR_RX_SCHEDULED)
    {
      /* Radio has received a packet */
      uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_MONITOR_RX_SUCCESS, NULL, 0);
    }
  }

  /* RF_EventRxEntryDone and RF_EventLastCmdDone can be set at the same time */
  if (events & RF_EventLastCmdDone)
  {
    if (monitorRxStatus == ULL_MONITOR_RX_SCHEDULED)
    {
      switch(urfiGenericRxCmd.status)
      {
        case BLE_DONE_OK:
        case BLE_DONE_RXTIMEOUT:
        case BLE_DONE_NOSYNC:
        case BLE_DONE_RXERR:
        case BLE_DONE_ENDED:
        case BLE_DONE_ABORT:
        case BLE_DONE_STOPPED:
        case ERROR_PAST_START:
          /* Operation finished normally or past start trigger */
          uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_MONITOR_RX_WINDOW_COMPLETE, NULL, 0);
          break;

        case BLE_ERROR_RXBUF:
          /* No available Rx buffer */
          uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_MONITOR_RX_BUF_FULL, NULL, 0);
          break;

        default:
          /* Do not allow further monitoring scans */
          ulState = ULL_STATE_STANDBY;

          /* Synth error or other fatal errors */
          uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_MONITOR_RX_FAILED, NULL, 0);
          break;
      }
    }
  }
  else if (events & RF_EventInternalError)
  {
    /* Do not allow further monitoring scans */
    ulState = ULL_STATE_STANDBY;

    /* Internal fatal errors */
    uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_MONITOR_RX_FAILED, NULL, 0);
  }

#if defined(RTLS_CTE)
  if (events & RF_EventSamplesEntryDone)
  {
    urtls_cteSamples.autoCopyCompleted ++;
  }
#endif /* RTLS_CTE */

  port_exitCS_SW(key);
}
#endif /* FEATURE_MONITOR */

/*********************************************************************
 * PUBLIC FUNCTIONS
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
bStatus_t ull_init(void)
{
  if (ulState != ULL_STATE_STANDBY)
  {
    /* Cannot re-initialize if running */
    return FAILURE;
  }

  /* Initialize RF Interface */
  if (urfi_init() != SUCCESS)
  {
    return FAILURE;
  }

#if defined(FEATURE_ADVERTISER)
  cAdvAboutTo = port_timerCreate(ull_clockHandler, ULL_CLKEVT_ADV_ABOUT_TO);
#if defined(RF_MULTIMODE)
  cAdvInt = port_timerCreate(ull_clockHandler, ULL_CLKEVT_ADV_INT_EXPIRED);
#endif /* RF_MULTIMODE */
#endif /* FEATURE_ADVERTISER */

  return SUCCESS;
}

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
                       pfnAdvDoneCB_t pfnAdvDoneCB)
{
  ull_notifyAdvAboutTo = pfnAdvAboutToCB;
  ull_notifyAdvDone = pfnAdvDoneCB;
}
#endif /* FEATURE_ADVERTISER */

#if defined(FEATURE_SCANNER)
/*********************************************************************
 * @fn     ull_scanRegisterCB
 *
 * @brief  Register callbacks supposed to be called by Observer.
 *
 * @param  pfnScanIndicationCB - callback to nofity the application that a
 *                               packet has been received.
 * @param  pfnScanWindowCompleteCB - callback to post-process scan interval complete
 *
 * @return  none
 */
void ull_scanRegisterCB(pfnScanIndCB_t pfnScanIndicationCB,
                        pfnScanWindowCompCB_t pfnScanWindowCompleteCB)
{
  ull_notifyScanIndication = pfnScanIndicationCB;
  ull_notifyScanWindowComplete = pfnScanWindowCompleteCB;
}
#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
/*********************************************************************
 * @fn     ull_monitorRegisterCB
 *
 * @brief  Register callbacks supposed to be called by Monitor.
 *
 * @param  pfnMonitorIndicationCB - callback to nofity the application that a
 *                                  packet has been received.
 * @param  pfnMonitorCompleteCB - callback to post-process monitor duration complete
 *
 * @return  none
 */
void ull_monitorRegisterCB(pfnMonitorIndCB_t pfnMonitorIndicationCB,
                           pfnMonitorCompCB_t pfnMonitorCompleteCB)
{
  ull_notifyMonitorIndication = pfnMonitorIndicationCB;
  ull_notifyMonitorComplete = pfnMonitorCompleteCB;
}
#endif /* FEATURE_MONITOR */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

#if defined(FEATURE_ADVERTISER)
/*********************************************************************
 * @fn      ull_advSchedule
 *
 * @brief   Update ADV command with latest parameters and schedule
 *          the radio operation
 *
 * @param   mode   -  if ULL_ADV_MODE_START, start the advertisement and schedule
 *                    the operation for 1 ms later. If set to
 *                    ULL_ADV_MODE_IMMEDIATE schedules the operation 1ms later.
 *                    ULL_ADV_MODE_PERIODIC and ULL_ADV_MODE_PERIODIC_RELAXED,
 *                    sechedule the operation, according to the advInterval and
 *                    the random delay for time critical and time relaxed
 *                    operation respectively. ULL_ADV_MODE_RESCHEDULE,
 *                    reschedules the operation at the same previous schedule.
 *
 * @return  SUCCESS - the radio operation has been successfully scheduled
 *          FAILURE - failed to schedule the radio operation due to RF driver
 *                    error
 */
bStatus_t ull_advSchedule(uint8 mode)
{
  uint16 commandNo;
#if defined(RF_MULTIMODE)
  RF_ScheduleCmdParams cmdParams = {
    0,
    RF_StartNotSpecified,
    RF_AllowDelayAny,
    0,
    RF_EndNotSpecified,
    0,
    0,
    RF_PriorityCoexDefault
  };
  uint8  numChan; /* # of adv channels */
  port_key_t key;
#endif /* RF_MULTIMODE */

#if defined(FEATURE_SCAN_RESPONSE)
  if (ubleParams.advType == UBLE_ADVTYPE_ADV_SCAN)
  {
    commandNo = CMD_BLE_ADV_SCAN;
  }
  else
#endif /* FEATURE_SCAN_RESPONSE */
  /* UBLE_ADVTYPE_ADV_NC */
  {
    commandNo = CMD_BLE_ADV_NC;
  }

  for (uint8 i = 0; i < 3; i++)
  {
    urfiAdvCmd[i].commandNo = (ubleParams.advChanMap & (1 << i)) ?
                               commandNo : CMD_NOP;
  }

  if ((ULL_ADV_MODE_START == mode) || (ULL_ADV_MODE_IMMEDIATE == mode))
  {
    /* Schedule the first adv event for 1 ms from now */
    urfiAdvCmd[0].startTime = RF_getCurrentTime() + 1 * MS_TO_RAT;
  }
  else if (ULL_ADV_MODE_PERIODIC == mode)
  {
    /* Add a random delay up to 10ms */
    urfiAdvCmd[0].startTime += ubleParams.advInterval * BLE_TO_RAT +
                               (uint32) rand() % (MS_TO_RAT * 10);
  }
  else if (ULL_ADV_MODE_PERIODIC_RELAXED == mode)
  {
    /* Anchor to the current time*/
    urfiAdvCmd[0].startTime = RF_getCurrentTime();
    /* Add a random delay up to 10ms to the interval */
    urfiAdvCmd[0].startTime += ubleParams.advInterval * BLE_TO_RAT +
                               (uint32) rand() % (MS_TO_RAT * 10);

  }
  // clear command status values
  urfiAdvCmd[0].status = IDLE;
  urfiAdvCmd[1].status = IDLE;
  urfiAdvCmd[2].status = IDLE;

#if defined(RF_MULTIMODE)
    /* Calculate the end time */
    numChan = ubleParams.advChanMap & 0x01 + ((ubleParams.advChanMap & 0x02) >> 1) +
              ((ubleParams.advChanMap & 0x04) >> 2);
    cmdParams.endTime =
      urfiAdvCmd[0].startTime
      + numChan * (((urfiAdvParams.advLen + 16) * BYTE_TO_RAT) + ULL_ADV_GAP_IN_CHAIN)
      - ULL_ADV_GAP_IN_CHAIN;

  if ((mode != ULL_ADV_MODE_RESCHEDULE) && (mode != ULL_ADV_MODE_IMMEDIATE))
  {
    uint32 timerTicks;

    if (RF_TIME_CRITICAL == rfTimeCrit)
    {
      if (ULL_ADV_MODE_START == mode)
      {
        timerTicks = ubleParams.advInterval * BLE_TO_RAT;
      }
      else
      {
        timerTicks = cmdParams.endTime - RF_getCurrentTime();
      }
      /* Set timeout equal to advertisement interval plus 1 ms */
      timerTicks = (timerTicks)/SYSTICK_TO_RAT;
      timerTicks += MS_TO_SYSTICK;

    }
    else //relaxed operation.
    {
      if (ULL_ADV_MODE_START == mode)
      {
        timerTicks = ubleParams.advInterval * BLE_TO_RAT;
      }
      else
      {
        timerTicks = cmdParams.endTime - RF_getCurrentTime();
      }

      /* Set timeout equal to two times the advertisement interval minus 1 ms */
      timerTicks = (timerTicks * 2)/SYSTICK_TO_RAT;
      timerTicks -= MS_TO_SYSTICK;
    }
    port_timerStop( cAdvInt );
    port_timerStart( cAdvInt, timerTicks );
  }

  key = port_enterCS_HW();
  urfiAdvHandle = RF_scheduleCmd(urfiHandle, (RF_Op*) &urfiAdvCmd[0],
                                 &cmdParams, ull_advDoneCb,
                                 RF_EventInternalError);
  port_exitCS_HW(key);

  if (urfiAdvHandle >= 0)
  {
    advTxStatus = ULL_ADV_TX_SCHEDULED;
  }
  else
  {
    //ToDo: Once RF driver add the error status, use that instead
    //and one for failure case.
    advTxStatus = ULL_ADV_TX_NO_RF_RESOURCE;
  }


#else /* RF Single Mode */
  urfiAdvHandle = RF_postCmd(urfiHandle, (RF_Op*) &urfiAdvCmd[0],
                             (RF_Priority) ubleParams.rfPriority, ull_advDoneCb,
                             RF_EventInternalError);
#endif

  if (urfiAdvHandle >= 0)
  {
    if ((mode != ULL_ADV_MODE_START) && ubleParams.timeToAdv)
    {
      /* # of RAT ticks to the next Adv */
      uint32 ticksToAdv = urfiAdvCmd[0].startTime - RF_getCurrentTime();

      /* Is there enough time to issue "about to advertise" notification
       * before the next ADV happens?
       * Count 1 ms additionally to the required time for safety.
       */
      if (ticksToAdv >= ((uint32) ubleParams.timeToAdv + 1) * MS_TO_RAT)
      {
        /* Stop timer first */
        port_timerStop( cAdvAboutTo );

        /* Convert RAT ticks to system ticks. */
        ticksToAdv /= SYSTICK_TO_RAT;

        /* Set the timeout and start the timer */
        port_timerStart( cAdvAboutTo,
                         ticksToAdv - ubleParams.timeToAdv * MS_TO_SYSTICK );
      }
    }

    return SUCCESS;
  }

#if defined(RF_MULTIMODE)
  return SUCCESS;
#else
  return FAILURE;
#endif
}

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
bStatus_t ull_advStart(void)
{
  port_key_t key;
  bStatus_t status = FAILURE;

  key = port_enterCS_SW();
  /* Possible to enter ULL_STATE_ADVERTISING from only ULL_STATE_STANDBY */
  if (ulState != ULL_STATE_STANDBY)
  {
    port_exitCS_SW(key);
    return status;
  }
  status = uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_ADV_TX_STARTED, NULL, 0);

  if (status == SUCCESS)
  {
    ulState = ULL_STATE_ADVERTISING;
  }
  port_exitCS_SW(key);
  return status;
}

/*********************************************************************
 * @fn      ull_advStop
 *
 * @brief   Exit ULL_STATE_ADVERTISING
 *
 * @param   none
 *
 * @return  none
 */
void ull_advStop(void)
{
  port_key_t key;

  key = port_enterCS_SW();

  if (ulState == ULL_STATE_ADVERTISING)
  {
    /* Cancel or stop ADV command */
    if (urfiAdvHandle > 0)
    {
      RF_cancelCmd(urfiHandle, urfiAdvHandle, 0);
      urfiAdvHandle = URFI_CMD_HANDLE_INVALID;
    }

    /* Cancel AdvAboutTo timer */
    port_timerStop( cAdvAboutTo );

#if defined(RF_MULTIMODE)
    /* cancel the interval timer */
    port_timerStop( cAdvInt );
#endif /* RF_MULTIMODE */

    ulState = ULL_STATE_STANDBY;
  }

  port_exitCS_SW(key);
}
#endif /* FEATURE_ADVERTISER */

#if defined(FEATURE_SCANNER)
/*********************************************************************
 * @fn      ull_scanSchedule
 *
 * @brief   Update SCAN command with latest parameters and schedule
 *          the radio operation
 *
 * @param   mode   -  if ULL_SCAN_MODE_START, start the scanning and schedule
 *                    the operation immediately. ULL_SCAN_MODE_RESCHEDULE,
 *                    reschedules the operation at the same previous schedule.
 *
 * @return  SUCCESS - the radio operation has been successfully scheduled
 *          FAILURE - failed to schedule the radio operation due to RF driver
 *                    error
 */
bStatus_t ull_scanSchedule(uint8 mode)
{
#if defined(RF_MULTIMODE)
  RF_ScheduleCmdParams cmdParams = {
    0,
    RF_StartNotSpecified,
    RF_AllowDelayAny,
    0,
    RF_EndNotSpecified,
    0,
    0,
    RF_PriorityCoexDefault
  };
  RF_EventMask bmEvent;
  port_key_t key;
#endif /* RF_MULTIMODE */

  /* The ULL_SCAN_MODE_START and ULL_SCAN_MODE_RESCHEDULE
   * are treated the same.
   */
  (void) mode;

  /* clear command status values */
  urfiScanCmd.status = IDLE;

#if defined(RF_MULTIMODE)
  bmEvent = (RF_EventInternalError | RF_EventLastCmdDone | RF_EventRxEntryDone);

  /* uGAP controls the start of scan timing. The end time is unknown.
   * ADV always has equal or higher priority.
   * The end time feature is not required.
   */
  cmdParams.endTime = 0;

  key = port_enterCS_HW();
  urfiScanHandle = RF_scheduleCmd(urfiHandle, (RF_Op*) &urfiScanCmd,
                                  &cmdParams, ull_scanDoneCb, bmEvent);
  port_exitCS_HW(key);

  if (urfiScanHandle >= 0)
  {
    scanRxStatus = ULL_SCAN_RX_SCHEDULED;
  }
  else
  {
    //ToDo: Once RF driver add the error status, use that instead
    //and one for failure case.
    scanRxStatus = ULL_SCAN_RX_NO_RF_RESOURCE;
  }

  return SUCCESS;
#else /* RF Single Mode */
  urfiScanHandle = RF_postCmd(urfiHandle, (RF_Op*) &urfiScanCmd,
                              (RF_Priority) ubleParams.rfPriority, ull_scanDoneCb,
                              RF_EventInternalError);

  if (urfiScanHandle >= 0)
  {
    return SUCCESS;
  }

  return FAILURE;
#endif /* RF_MULTIMODE */
}

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
bStatus_t ull_scanStart(uint8_t scanChanIndex)
{
  port_key_t key;
  bStatus_t status = FAILURE;

  key = port_enterCS_SW();
  /* Possible to enter ULL_STATE_SCANNING from only ULL_STATE_STANDBY */
  if (ulState != ULL_STATE_STANDBY)
  {
    port_exitCS_SW(key);
    return status;
  }

  /* Update scan parameters */
  urfiScanCmd.channel = scanChanIndex;

  /* TBD: Update other scan parameters */

  /* Post an ul event */
  status = uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_SCAN_RX_STARTED, NULL, 0);

  if (status == SUCCESS)
  {
    ulState = ULL_STATE_SCANNING;
  }

  port_exitCS_SW(key);
  return(status);
}

/*********************************************************************
 * @fn      ull_scanStop
 *
 * @brief   Exit ULL_STATE_SCANNING
 *
 * @param   none
 *
 * @return  none
 */
void ull_scanStop(void)
{
  port_key_t key;

  key = port_enterCS_SW();

  if (ulState == ULL_STATE_SCANNING)
  {
    ulState = ULL_STATE_STANDBY;

    /* Cancel or stop Scan command */
    if (urfiScanHandle > 0)
    {
      RF_cancelCmd(urfiHandle, urfiScanHandle, 0);
      urfiScanHandle = URFI_CMD_HANDLE_INVALID;
    }

    ull_notifyScanWindowComplete(SUCCESS);
  }

  port_exitCS_SW(key);
}

/*******************************************************************************
 * @fn          ull_getAdvChanPDU
 *
 * @brief       This function is used to retrieve the Advertising Channel PDU,
 *              and parse its data for either a Advertising Packet Report or a
 *              Scan Packet Report.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       len      - Pointer to payload length (sans the Adv's address).
 * @param       payload  - Pointer to payload (sans the Adv's address).
 *
 * @return      None.
 */
void ull_getAdvChanPDU( uint8 *len, uint8 *payload )
{
  uint8       *pPdu;
  dataEntry_t *pDataEntry;

  /* get pointer to packet */
  pDataEntry = ull_getNextDataEntry( (dataEntryQ_t *)urfiScanCmd.pParams->pRxQ );

  if (pDataEntry != NULL)
  {
    /* get pointer to BLE PDU packet */
    pPdu = (uint8 *)(pDataEntry + 1);

    /* read length, add postfix length, exclude CRC length  */
    *len = pPdu[1] + ULL_PKT_HDR_LEN + ULL_SUFFIX_MAX_SIZE - ULL_SUFFIX_CRC_SIZE;

    if (*len < ULL_PKT_HDR_LEN + ULL_MAX_BLE_ADV_PKT_SIZE + ULL_SUFFIX_MAX_SIZE - ULL_SUFFIX_CRC_SIZE)
    {
      /* copy PDU to payload */
      memcpy(payload, pPdu, *len);
    }
    else
    {
      /* packet length error */
      *len = 0;
    }
  }
  else
  {
    /* data entry error */
    *len = 0;
  }
}

/*********************************************************************
 * @fn      ull_rxEntryDoneCback
 *
 * @brief   Callback to send the received packet up.
 *
 * @param   none
 *
 * @return  none
 */
void ull_rxEntryDoneCback(void)
{
  uint8 dataLen = 0;
  port_key_t key;
  port_key_t key_s;

  key = port_enterCS_HW();
  key_s = port_enterCS_SW();

  dataEntry_t *pDataEntry = ull_getNextDataEntry( (dataEntryQ_t *)urfiScanCmd.pParams->pRxQ );

  /* get pointer to packet */
  if ( (pDataEntry == NULL) || (pDataEntry->status != DATA_ENTRY_FINISHED) )
  {
    port_exitCS_SW(key_s);
    port_exitCS_HW(key);

    /* nothing to do here */
    return;
  }

  /* The callback will be called only if the previous adv packet is
   * processed by uGAP.
   */
  if (Ull_advPktInuse == false)
  {
    Ull_advPktInuse = true;

    /* process RX FIFO data */
    ull_getAdvChanPDU( &dataLen, advPkt );

    /* in all cases, mark the RX queue data entry as free
     * Note: Even if there isn't any heap to copy to, this packet is considered
     *       lost, and the queue entry is marked free for radio use.
     */
    ull_nextDataEntryDone( (dataEntryQ_t *)urfiScanCmd.pParams->pRxQ );
  }

  /* TBD: handle filtering and white list */
  /* TBD: advPkt needs to be dynamically allocated and application freed */

  /* We have an advertisment packet:
   *
   * | Preamble  | Access Addr | PDU         | CRC     |
   * | 1-2 bytes | 4 bytes     | 2-257 bytes | 3 bytes |
   *
   * The PDU is expended to:
   * | Header  | Payload     |
   * | 2 bytes | 1-255 bytes |
   *
   * The Header is expended to:
   * | PDU Type...RxAdd | Length |
   * | 1 byte           | 1 byte |
   *
   * The Payload is expended to:
   * | AdvA    | AdvData    |
   * | 6 bytes | 0-31 bytes |
   *
   * The radio stripps the CRC and replaces it with the postfix.
   *
   * The Postfix is expended to:
   * | RSSI   | Status | TimeStamp |
   * | 1 byte | 1 byte | 4 bytes   |
   *
   * The Status is expended to:
   * | bCrcErr | bIgnore | channel  |
   * | bit 7   | bit 6   | bit 5..0 |
   *
   * Note the advPkt is the beginning of PDU; the dataLen includes
   * the postfix length.
   *
   */

  port_exitCS_SW(key_s);
  port_exitCS_HW(key);

  if (dataLen != 0)
  {
    ull_notifyScanIndication( SUCCESS, dataLen, advPkt );
  }
  return;
}
#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
/*********************************************************************
 * @fn      ull_monitorSchedule
 *
 * @brief   Update Generic RX command with latest parameters and schedule
 *          the radio operation
 *
 * @param   mode   -  if ULL_MONITOR_MODE_START, start the scanning and schedule
 *                    the operation immediately. ULL_MONITOR_MODE_RESCHEDULE,
 *                    reschedules the operation at the same previous schedule.
 *
 * @return  SUCCESS - the radio operation has been successfully scheduled
 *          FAILURE - failed to schedule the radio operation due to RF driver
 *                    error
 */
bStatus_t ull_monitorSchedule(uint8 mode)
{
#if defined(RF_MULTIMODE)
  RF_ScheduleCmdParams cmdParams = {
    0,
    RF_StartNotSpecified,
    RF_AllowDelayAny,
    0,
    RF_EndNotSpecified,
    0,
    0,
    RF_PriorityCoexDefault
  };
  RF_EventMask bmEvent;
  port_key_t key;
#endif /* RF_MULTIMODE */

  /* clear command status values */
  urfiGenericRxCmd.status = IDLE;

  /* Note: cannot do wildcard access address */
  urfiGenericRxParams.accessAddress = ubleParams.accessAddr;

  /* Note: for crc check and rssi readings to work we must set a valid crcInit value */
  urfiGenericRxParams.crcInit0 = (ubleParams.crcInit >> 0) & 0xFF;
  urfiGenericRxParams.crcInit1 = (ubleParams.crcInit >> 8) & 0xFF;
  urfiGenericRxParams.crcInit2 = (ubleParams.crcInit >> 16) & 0xFF;


#if defined(RF_MULTIMODE)
  bmEvent = (RF_EventInternalError | RF_EventLastCmdDone | RF_EventRxEntryDone);

  if (mode == ULL_MONITOR_MODE_START)
  {
    urfiGenericRxCmd.startTrigger.triggerType = TRIG_ABSTIME;
    urfiGenericRxCmd.startTime = ubleParams.startTime;
    urfiGenericRxCmd.startTrigger.pastTrig = 1;

    if (ubleParams.monitorDuration > 0)
    {
      urfiGenericRxParams.endTrigger.triggerType = TRIG_REL_START;
      urfiGenericRxParams.endTime = ubleParams.monitorDuration * BLE_TO_RAT;
      cmdParams.endTime = urfiGenericRxParams.endTime;
    }
    else
    {
      /* Continuous monitoring */
      urfiGenericRxParams.endTrigger.triggerType = TRIG_NEVER;
      urfiGenericRxParams.endTime = 0;
      cmdParams.endTime = 0;
    }
  }
  else
  {
    /* ULL_MONITOR_MODE_RESCHEDULE */
    urfiGenericRxCmd.startTrigger.triggerType = TRIG_NOW;
    urfiGenericRxCmd.startTime = 0;

    if (ubleParams.monitorDuration > 0)
    {
      if (urfiGenericRxParams.endTime > ubleParams.monitorDuration * BLE_TO_RAT)
      {
        /* shorten the end time */
        urfiGenericRxParams.endTime -= ubleParams.monitorDuration * BLE_TO_RAT;
      }
      else
      {
        /* past the end time */
        urfiGenericRxParams.endTrigger.triggerType = TRIG_REL_START;
        urfiGenericRxParams.endTime = 100; /* End ASAP */
      }
      cmdParams.endTime = urfiGenericRxParams.endTime;
    }
    else
    {
      /* Continuous monitoring */
      urfiGenericRxParams.endTrigger.triggerType = TRIG_NEVER;
      urfiGenericRxParams.endTime = 0;
      cmdParams.endTime = 0;
    }
  }

#if defined(RTLS_CTE)
  if (urtls_cteSamples.pAutoCopyBuffers != NULL)
  {
    // in case the sampling is enable
    if (urtls_cteInfo[ubleParams.monitorHandle-1].samplingEnable == URTLS_CTE_SAMPLING_ENABLE)
    {
      // Enable the Packet received with CRC error interrupt and copy samples interrupt
      bmEvent |= (RF_EventSamplesEntryDone);
      // Set the antenna switch
      urtls_rfOverrideCteValue((uint32)(urtls_cteInfo[ubleParams.monitorHandle-1].pAntenna), RFC_FWPAR_CTE_ANT_SWITCH, RFC_CTE_ANT_SWITCH_OFFSET);
      // set the auto copy struct pointer in RF memory
      urtls_rfOverrideCteValue((uint32)(&urtls_cteSamples.autoCopy),RFC_FWPAR_CTE_AUTO_COPY, RFC_CTE_AUTO_COPY_OFFSET);
      // reset the auto copy counter
      urtls_cteSamples.autoCopyCompleted = 0;
      // set the CTE max count as 1 CTE (relevant only to periodic scan)
      urtls_cteSamples.autoCopy.cteCopyLimitCount = 1;
    }
    else
    {
      // Disable the Packet received with CRC error interrupt and copy samples interrupt
      bmEvent &= ~(RF_EventSamplesEntryDone);
      // disable the antenna switch
      urtls_rfOverrideCteValue(0, RFC_FWPAR_CTE_ANT_SWITCH, RFC_CTE_ANT_SWITCH_OFFSET);
      // disable the auto copy
      urtls_rfOverrideCteValue(0, RFC_FWPAR_CTE_AUTO_COPY, RFC_CTE_AUTO_COPY_OFFSET);
    }
  }
#endif /* RTLS_CTE */

  key = port_enterCS_HW();

  /* Save local session Id */
  ull_sessionId = ubleParams.monitorHandle;
  urfiGenericRxHandle = RF_scheduleCmd(urfiHandle, (RF_Op*) &urfiGenericRxCmd,
                                       &cmdParams, ull_monitorDoneCb, bmEvent);
  port_exitCS_HW(key);

  if (urfiGenericRxHandle >= 0)
  {
    monitorRxStatus = ULL_MONITOR_RX_SCHEDULED;
  }
  else
  {
    //ToDo: Once RF driver add the error status, use that instead
    //and one for failure case.
    monitorRxStatus = ULL_MONITOR_RX_NO_RF_RESOURCE;
  }

  return SUCCESS;
#else /* RF Single Mode */
  urfiGenericRxHandle = RF_postCmd(urfiHandle, (RF_Op*) &urfiGenericRxParams,
                                   (RF_Priority) ubleParams.rfPriority, ull_monitorDoneCb,
                                   RF_EventInternalError);

  if (urfiGenericRxHandle >= 0)
  {
    return SUCCESS;
  }

  return FAILURE;
#endif /* RF_MULTIMODE */
}

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
bStatus_t ull_monitorStart(uint8_t channel)
{
  port_key_t key;
  port_key_t hwKey;

  bStatus_t status = FAILURE;

  hwKey = port_enterCS_HW();
  key = port_enterCS_SW();

  /* Update scan parameters */
  urfiGenericRxCmd.channel = ubleParams.monitorChan = channel;

  ulState = ULL_STATE_MONITORING;

  /* Post an ul event */
  status = uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_MONITOR_RX_STARTED, NULL, 0);

  port_exitCS_SW(key);
  port_exitCS_HW(hwKey);

  return (status);
}

/*******************************************************************************
 * @fn          ull_getPDU
 *
 * @brief       This function is used to retrieve the Rx PDU.
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       len      - Pointer to payload length.
 * @param       payload  - Pointer to payload.
 *
 * @return      None.
 */
void ull_getPDU( uint8 *len, uint8 *payload )
{
  uint8       *pPdu;
  dataEntry_t *pDataEntry;

  /* get pointer to packet */
  pDataEntry = ull_getNextDataEntry( (dataEntryQ_t *)urfiGenericRxCmd.pParams->pRxQ );

  /* get pointer to BLE PDU packet */
  pPdu = (uint8 *)(pDataEntry + 1);

  /* read length, add postfix length, exclude CRC length  */
  *len = pPdu[1] + ULL_PKT_HDR_LEN + ULL_SUFFIX_MAX_SIZE - ULL_SUFFIX_CRC_SIZE;

  // If CTE Info is found
  if ((pPdu[0] & 0x20) == 0x20)
  {
      *len += 1;
#if defined(RTLS_CTE)
      if(urtls_cteInfo[ull_sessionId-1].requestEnable == TRUE)
      {
        uint8_t cteInfo = pPdu[2];
        //save the CTE info received from peer
        urtls_cteInfo[ull_sessionId-1].recvCte = TRUE;
        urtls_cteInfo[ull_sessionId-1].recvInfo.length = cteInfo & URTLS_CTE_INFO_TIME_MASK;
        urtls_cteInfo[ull_sessionId-1].recvInfo.type = (cteInfo & URTLS_CTE_INFO_TYPE_MASK) >> URTLS_CTE_INFO_TYPE_OFFSET;
      }
#endif /* RTLS_CTE */
  }

  /* The maximum length is ULL_PKT_HDR_LEN + ULL_MAX_BLE_PKT_SIZE +
   * ULL_SUFFIX_MAX_SIZE - ULL_SUFFIX_CRC_SIZE. This is greater than 255.
   * There is no need to do a length check here since it will always pass.
   */

  /* copy PDU to payload */
  memcpy(payload, pPdu, *len);
}

/*********************************************************************
 * @fn      ull_rxEntryDoneCback
 *
 * @brief   Callback to send the received packet up.
 *
 * @param   none
 *
 * @return  none
 */
void ull_rxEntryDoneCback(void)
{
  uint8 dataLen;
  port_key_t key;
  port_key_t key_s;

  key = port_enterCS_HW();
  key_s = port_enterCS_SW();

  dataEntry_t *pDataEntry = ull_getNextDataEntry( (dataEntryQ_t *)urfiGenericRxCmd.pParams->pRxQ );

  /* get pointer to packet */
  if ( (pDataEntry == NULL) || (pDataEntry->status != DATA_ENTRY_FINISHED) )
  {
    port_exitCS_HW(key);
    port_exitCS_SW(key_s);

    /* nothing to do here */
    return;
  }

  /* process RX FIFO data */
  ull_getPDU( &dataLen, monitorPkt );

  /* in all cases, mark the RX queue data entry as free
   * Note: Even if there isn't any heap to copy to, this packet is considered
   *       lost, and the queue entry is marked free for radio use.
   */
  ull_nextDataEntryDone( (dataEntryQ_t *)urfiGenericRxCmd.pParams->pRxQ );

  port_exitCS_HW(key);
  port_exitCS_SW(key_s);

  /* TBD: monitorPkt can be dynamically allocated and application freed */

  /* We have a packet:
   *
   * | Preamble  | Access Addr | PDU         | CRC     |
   * | 1-2 bytes | 4 bytes     | 2-257 bytes | 3 bytes |
   *
   * The PDU is expended to:
   * | Header  | Payload     |
   * | 2 bytes | 1-255 bytes |
   *
   * The Header is expended to:
   * | PDU Type...RxAdd | Length |
   * | 1 byte           | 1 byte |
   *
   * The radio stripps the CRC and replaces it with the postfix.
   *
   * The Postfix is expended to:
   * | RSSI   | Status | TimeStamp |
   * | 1 byte | 1 byte | 4 bytes   |
   *
   * The Status is expended to:
   * | bCrcErr | bIgnore | channel  |
   * | bit 7   | bit 6   | bit 5..0 |
   *
   * Note the monitorPkt is the beginning of PDU; the dataLen includes
   * the postfix length.
   *
   */
  if (dataLen != 0)
  {
    ull_notifyMonitorIndication( SUCCESS, ull_sessionId, dataLen, monitorPkt );
  }
  return;
}
#endif /* FEATURE_MONITOR */
/*********************************************************************
 * @fn      uble_processLLMsg
 *
 * @brief   Process event messages sent from Micro BLE Stack to uLL through
 *          the application's Stack Event Proxy and uble_processMsg().
 *
 * @param   pEvtMsg - Pointer to the Micro BLE Stack event message destined to
 *                    uLL.
 *
 * @return  none
 */
void uble_processLLMsg(ubleEvtMsg_t *pEvtMsg)
{
  switch (pEvtMsg->hdr.evt)
  {
#if defined(FEATURE_ADVERTISER)
  case ULL_EVT_ADV_TX_SUCCESS:
    /* Adv tx was successful */
    advTxStatus = ULL_ADV_TX_DONE;

#if defined(RF_MULTIMODE)
    port_timerStop( cAdvInt );
#endif /* RF_MULTIMODE */

    ull_notifyAdvDone(SUCCESS);
    if (ulState == ULL_STATE_ADVERTISING)
    {
      /* set the next advertisement */
      if (RF_TIME_CRITICAL == rfTimeCrit)
      {
        ull_advSchedule(ULL_ADV_MODE_PERIODIC);
      }
      else
      {
        ull_advSchedule(ULL_ADV_MODE_PERIODIC_RELAXED);
      }
    }
    break;

  case ULL_EVT_ADV_TX_FAILED:
    /* adv rf tx failure */
    advTxStatus = ULL_ADV_TX_NO_RF_RESOURCE;
    ull_notifyAdvDone(bleNoResources);
    break;

  case ULL_EVT_ADV_TX_TIMER_EXPIRED:
    /* adv interval timer expired */
    advTxStatus = ULL_ADV_TX_FAILED;
    ull_notifyAdvDone(FAILURE);
    if (ulState == ULL_STATE_ADVERTISING)
    {
      if (RF_TIME_CRITICAL == rfTimeCrit)
      {
        ull_advSchedule(ULL_ADV_MODE_PERIODIC);
      }
      else
      {
        /* Cancel or stop ADV command */
        if (urfiAdvHandle > 0)
        {
          RF_cancelCmd(urfiHandle, urfiAdvHandle, 0);
          urfiAdvHandle = URFI_CMD_HANDLE_INVALID;
        }

        ull_advSchedule(ULL_ADV_MODE_PERIODIC_RELAXED);
      }
    }
    break;

  case ULL_EVT_ADV_TX_RADIO_AVAILABLE:
    /* rf radio resource available */
    if (ULL_ADV_TX_NO_RF_RESOURCE == advTxStatus)
    {
      /* check if there is time to reschedule the advertisement. */
      uint32_t currentTime = RF_getCurrentTime();
      if ((currentTime + 1 * MS_TO_RAT) < urfiAdvCmd[0].startTime)
      {
        if (ulState == ULL_STATE_ADVERTISING)
        {
          /* reschedule the advertisement.*/
          ull_advSchedule(ULL_ADV_MODE_RESCHEDULE);
        }
      }
      else
      {
        if (RF_TIME_CRITICAL == rfTimeCrit)
        {
          advTxStatus = ULL_ADV_TX_FAILED;
          ull_notifyAdvDone(FAILURE);
#if defined(RF_MULTIMODE)
          port_timerStop( cAdvInt );
#endif /* RF_MULTIMODE */
          if (ulState == ULL_STATE_ADVERTISING)
          {
            ull_advSchedule(ULL_ADV_MODE_PERIODIC);
          }
        }
        else /* relaxed operation */
        {
          /* past due, send it immediate */
          if (ulState == ULL_STATE_ADVERTISING)
          {
            ull_advSchedule(ULL_ADV_MODE_IMMEDIATE);
          }
        }
      }
    }
    break;

  case ULL_EVT_ADV_TX_STARTED:
    if (ulState == ULL_STATE_ADVERTISING)
    {
      ull_advSchedule(ULL_ADV_MODE_START);
    }
    break;
#endif /* FEATURE_ADVERTISER */

#if defined(FEATURE_SCANNER)
  case ULL_EVT_SCAN_RX_SUCCESS:
    /* Scan Rx has received a packet */
    ull_rxEntryDoneCback();
    break;

  case ULL_EVT_SCAN_RX_FAILED:
    /* Scan rx failure. This is caused by radio failures,
     * not preemption eventhough the scanRxStatus is the same.
     */
    scanRxStatus = ULL_SCAN_RX_NO_RF_RESOURCE;
    ull_notifyScanWindowComplete(bleNoResources);
    break;

  case ULL_EVT_SCAN_RX_BUF_FULL:
    /* Scan Rx buffer full */
    scanRxStatus = ULL_SCAN_RX_NO_RF_RESOURCE;
    ull_notifyScanIndication( MSG_BUFFER_NOT_AVAIL, 0, NULL );

    if (ulState == ULL_STATE_SCANNING)
    {
      /* reschedule the scanning */
      ull_scanSchedule(ULL_SCAN_MODE_RESCHEDULE);
    }
    break;

  case ULL_EVT_SCAN_RX_RADIO_AVAILABLE:
    /* Rf radio resource available. This is caused by PHY preemption.
     */
    if (scanRxStatus == ULL_SCAN_RX_NO_RF_RESOURCE && ulState == ULL_STATE_SCANNING)
    {
      /* reschedule the scanning */
      ull_scanSchedule(ULL_SCAN_MODE_RESCHEDULE);
    }
    break;

  case ULL_EVT_SCAN_RX_STARTED:
    if (ulState == ULL_STATE_SCANNING)
    {
      ull_scanSchedule(ULL_SCAN_MODE_START);
    }
    break;
#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
  case ULL_EVT_MONITOR_RX_SUCCESS:
    /* Monitoring scan Rx has received a packet */
    ull_rxEntryDoneCback();
    break;

  case ULL_EVT_MONITOR_RX_FAILED:
    /* Monitoring scan rx failure. This is caused by radio failures,
     * not preemption eventhough the monitorRxStatus is the same.
     */
    monitorRxStatus = ULL_MONITOR_RX_NO_RF_RESOURCE;
    ull_notifyMonitorComplete(bleNoResources, ull_sessionId);
    break;

  case ULL_EVT_MONITOR_RX_BUF_FULL:
    /* Monitoring scan Rx buffer full */
    monitorRxStatus = ULL_MONITOR_RX_NO_RF_RESOURCE;
    ull_notifyMonitorIndication(MSG_BUFFER_NOT_AVAIL, ull_sessionId, 0, NULL);

    /* reschedule the monitoring scan */
    ull_monitorSchedule(ULL_MONITOR_MODE_RESCHEDULE);
    break;

  case ULL_EVT_MONITOR_RX_WINDOW_COMPLETE:
  {
#if defined(RTLS_CTE)
     // get the CTE information in case received CTE response packet
    if ((urtls_cteSamples.autoCopyCompleted > 0) && (urtls_cteSamples.pAutoCopyBuffers != NULL))
    {
      dataEntry_t *pDataEntry;
      port_key_t key;
      port_key_t key_s;

      key = port_enterCS_HW();
      key_s = port_enterCS_SW();

      // get the samples buffer
      pDataEntry = ull_getNextDataEntry((dataEntryQ_t *)urtls_cteSamples.autoCopy.pSamplesQueue);

      urtls_getCteInfo(pDataEntry, ull_sessionId, (uint8_t)ubleParams.monitorChan);

      /* in all cases, mark the RX queue data entry as free
       * Note: Even if there isn't any heap to copy to, this packet is considered
       *       lost, and the queue entry is marked free for radio use.
       */
      ull_nextDataEntryDone((dataEntryQ_t *)urtls_cteSamples.autoCopy.pSamplesQueue);

      port_exitCS_SW(key_s);
      port_exitCS_HW(key);
    }
#endif /* RTLS_CTE */

    /* Monitoring scan rx window complete. */
      ull_notifyMonitorComplete(SUCCESS, ull_sessionId);
  }
  break;

  case ULL_EVT_MONITOR_RX_RADIO_AVAILABLE:
    /* Rf radio resource available. This is caused by PHY preemption.
     */
      /* reschedule the monitoring scan */
      ull_monitorSchedule(ULL_MONITOR_MODE_RESCHEDULE);
    break;

  case ULL_EVT_MONITOR_RX_STARTED:
      ull_monitorSchedule(ULL_MONITOR_MODE_START);
    break;
#endif /* FEATURE_MONITOR */

  default:
    break;
  }
}

/*********************************************************************
*********************************************************************/
