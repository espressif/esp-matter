/******************************************************************************

 @file  ugap.c

 @brief This file contains the Micro GAP Initialization and Configuration APIs.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2011-2022, Texas Instruments Incorporated
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
#include <string.h>

#include <icall.h>

#include "bcomdef.h"
#include "util.h"

#include "port.h"
#include "uble.h"
#include "ull.h"
#include "ugap.h"

#if defined(FEATURE_BROADCASTER) && !defined(FEATURE_ADVERTISER)
  #error "FEATURE_ADVERTISER should also be defined if FEATURE_BROADCASTER \
is defined."
#endif /* FEATURE_BROADCASTER && !FEATURE_ADVERTISER */
#if defined(FEATURE_OBSERVER) && !defined(FEATURE_SCANNER)
  #error "FEATURE_SCANNER should also be defined if FEATURE_OBSERVER \
is defined."
#endif /* FEATURE_OBSERVER && !FEATURE_SCANNER */
#if !defined(FEATURE_BROADCASTER) && !defined(FEATURE_OBSERVER) && !defined(FEATURE_CM)
  #error "At least one GAP role feature should be defined. Currently only \
FEATURE_BROADCASTER is available."
#endif /* !FEATURE_BROADCASTER && !FEATURE_OBSERVER */

#if defined(FEATURE_MONITOR)
#if defined(FEATURE_OBSERVER) || defined(FEATURE_SCANNER)
  #error "FEATURE_MONITOR should be stand alone. No other uStack features \
should be defined at the same time."
#endif /* FEATURE_OBSERVER || FEATURE_SCANNER */
#endif /* FEATURE_MONITOR */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define UGAP_CLKEVT_BCAST_DUTY          0x0001
#define UGAP_CLKEVT_SCAN_TIMER          0x0002

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern ubleParams_t ubleParams;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern bStatus_t uble_buildAndPostEvt(ubleEvtDst_t evtDst, ubleEvt_t evt,
                                      ubleMsg_t *pMsg, uint16 len);

/*********************************************************************
 * LOCAL VARIABLES
 */

#if defined(FEATURE_OBSERVER)
/* Micro GAP Observer Role State */
static ugapObserverScan_State_t ugoState = UGAP_SCAN_STATE_INVALID;
#endif /* FEATURE_OBSERVER */

#if defined(FEATURE_MONITOR)
/* Micro GAP Monitor State */
static ugapMonitorState_t ugmState = UGAP_MONITOR_STATE_INVALID;
#endif /* FEATURE_MONITOR */

#if defined(FEATURE_BROADCASTER)
/* Micro GAP Broadcaster Role State */
static ugapBcastState_t ugbState = UGAP_BCAST_STATE_INVALID;

/* Broadcaster Duty Control */
static struct port_timerObject_s *cBcastDuty;

/* Set of application callbacks to be issued by Broadcaster */
static ugapBcastCBs_t ugbAppCBs;

/*********************************************************************
 * Broadcaster Parameters
 */
static uint16 ugbNumAdvEvent;
static uint16 ugbDutyOnTime = 0;
static uint16 ugbDutyOffTime = 0; /* Duty control is off by default */
#endif /* FEATURE_BROADCASTER */

#if defined(FEATURE_OBSERVER)

/* Scan On/Off time Control */
static struct port_timerObject_s *cScanTimer;

/* Set of application callbacks to be issued by Observer */
static ugapObserverScanCBs_t ugoAppCBs;

/*********************************************************************
 * Observer Parameters
 */
static uint16 ugoScanOnTime = 0;
static uint16 ugoScanOffTime = 0;
static ugapObserverScan_State_t ugoStatePrev = UGAP_SCAN_STATE_INVALID;
static uint8 ugoScanChanMap = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void ugap_observerScanWindowCompleteCB(bStatus_t status);

#endif /* FEATURE_OBSERVER */

#if defined(FEATURE_MONITOR)

/* Set of application callbacks to be issued by Monitor */
static ugapMonitorCBs_t ugmAppCBs;

/*********************************************************************
 * Monitor Parameters
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

#endif /* FEATURE_OBSERVER */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

#if defined(FEATURE_BROADCASTER)
/*********************************************************************
 * @fn      ugap_bcastChangeState
 *
 * @brief   Change Broadcaster State. If successful, post UGB_EVT_STATE_CHANGE
 *          event.
 *
 * @param   state - the new state to switch to
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE
 */
static bStatus_t ugap_bcastChangeState(ugapBcastState_t state)
{
  port_key_t key;
  bStatus_t status = SUCCESS;

  key = port_enterCS_SW();

  if (ugbState == state)
  {
    /* Caller is attempting to switch to the same state. Just return
       without posting UGB_EVT_STATE_CHANGE */
    port_exitCS_SW(key);
    return status;
  }

  switch (state)
  {
  case UGAP_BCAST_STATE_INITIALIZED:
    ugbState = state;
    break;

  case UGAP_BCAST_STATE_IDLE:
    port_timerStop( cBcastDuty );

    if (ugbState == UGAP_BCAST_STATE_ADVERTISING)
    {
      ull_advStop();
    }

#if defined(FEATURE_OBSERVER)
    ugap_scanResume();
#endif /* FEATURE_OBSERVER */

    ugbState = state;
    break;

  case UGAP_BCAST_STATE_ADVERTISING:

#if defined(FEATURE_OBSERVER)
    ugap_scanSuspend();
#endif /* FEATURE_OBSERVER */

    status = ull_advStart();
    if (status == SUCCESS)
    {
      /* Set the timeout for duty on time only if duty control is enabled */
      if (ugbDutyOnTime != 0 && ugbDutyOffTime != 0)
      {
        port_timerStart( cBcastDuty,
                         ugbDutyOnTime * UGAP_DUTY_TIME_UNIT * MS_TO_SYSTICK );
      }

      ugbState = state;
    }
    break;

  case UGAP_BCAST_STATE_WAITING:
    ugbState = state;

    /* Set the timeout for duty off time */
    port_timerStart( cBcastDuty,
                     ugbDutyOffTime * UGAP_DUTY_TIME_UNIT * MS_TO_SYSTICK );

    ull_advStop();

#if defined(FEATURE_OBSERVER)
    ugap_scanResume();
#endif /* FEATURE_OBSERVER */

    break;

  case UGAP_BCAST_STATE_SUSPENDED:
    /* TBD */
    break;

  default:
    status = INVALIDPARAMETER;
    break;
  }

  if (status == SUCCESS)
  {
    ugapBcastMsgStateChange_t msgStateChange;

    msgStateChange.state = ugbState;
    status = uble_buildAndPostEvt(UBLE_EVTDST_GAP, UGB_EVT_STATE_CHANGE,
                                  (ubleMsg_t*) &msgStateChange, sizeof(msgStateChange));
  }

  port_exitCS_SW(key);

  return status;
}
#endif /* FEATURE_BROADCASTER */

#if defined(FEATURE_OBSERVER)
/*********************************************************************
 * @fn      ugap_observerChanMapToIndex
 *
 * @brief   Conver the channel map the channel index. Mask out the
 *          channel map for the next call.
 *
 * @param   pChanMap - pointer to current channel map
 *
 * @return  Channel index
 */
static uint8_t ugap_observerChanMapToIndex(uint8_t *pChanMap)
{
  if (*pChanMap == 0)
  {
    /* Reload the scan channel map */
    *pChanMap = ubleParams.scanChanMap;
  }
  if ((*pChanMap) & UBLE_ADV_CHAN_37)
  {
    (*pChanMap) &= ~UBLE_ADV_CHAN_37;
    return 37;
  }
  if ((*pChanMap) & UBLE_ADV_CHAN_38)
  {
    (*pChanMap) &= ~UBLE_ADV_CHAN_38;
    return 38;
  }
  if ((*pChanMap) & UBLE_ADV_CHAN_39)
  {
    (*pChanMap) &= ~UBLE_ADV_CHAN_39;
    return 39;
  }

  /* Should not happen */
  return 0;
}

/*********************************************************************
 * @fn      ugap_observerChangeState
 *
 * @brief   Change Observer State. If successful, post UGS_EVT_STATE_CHANGE
 *          event.
 *
 * @param   state - the new state to switch to
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE
 */
static bStatus_t ugap_observerChangeState(ugapObserverScan_State_t state)
{
  port_key_t key;
  bStatus_t status = SUCCESS;

  key = port_enterCS_SW();

  if (ugoState == state)
  {
    /* Caller is attempting to switch to the same state. Just return
       without posting UGS_EVT_STATE_CHANGE */
    port_exitCS_SW(key);
    return status;
  }

  switch (state)
  {
  case UGAP_SCAN_STATE_INITIALIZED:
    ugoState = state;
    break;

  case UGAP_SCAN_STATE_IDLE:
    port_timerStop(cScanTimer);

    if (ugoState == UGAP_SCAN_STATE_SCANNING)
    {
      ull_scanStop();
    }

    ugoState = state;
    break;

  case UGAP_SCAN_STATE_SCANNING:
    /* Change the scan timer and start scan on next channel. */
    status = ull_scanStart(ugap_observerChanMapToIndex(&ugoScanChanMap));
    if (status == SUCCESS)
    {
      ugoState = state;

      /* Set the timeout for scan if scan on time > 0 */
      if (ugoScanOnTime != 0)
      {
        port_timerStart( cScanTimer, ugoScanOnTime * BLE_TO_SYSTICK );
      }
    }
    break;

  case UGAP_SCAN_STATE_WAITING:
    ugoState = state;

    /* Change the scan timer and stop scan. */
    port_timerStart( cScanTimer, ugoScanOffTime * BLE_TO_SYSTICK );
    ull_scanStop();
    break;

  case UGAP_SCAN_STATE_SUSPENDED:
    if (ugoState == UGAP_SCAN_STATE_SCANNING)
    {
      /* If previous state is SCANNING, stop the ongoing scan timer */
      port_timerStop(cScanTimer);

      /* If previous state is SCANNING, stop the ongoing scan */
      ull_scanStop();
    }

    /* SUSPEND could occur while waiting */
    ugoState = state;
    break;

  default:
    status = INVALIDPARAMETER;
    break;
  }

  if (status == SUCCESS)
  {
    ugapObserverMsgStateChange_t msgStateChange;

    msgStateChange.state = ugoState;
    status = uble_buildAndPostEvt(UBLE_EVTDST_GAP, UGAP_OBSERVER_EVT_STATE_CHANGE,
                                  (ubleMsg_t*) &msgStateChange, sizeof(msgStateChange));
  }

  port_exitCS_SW(key);

  return status;
}
#endif /* FEATURE_OBSERVER */

#if defined(FEATURE_MONITOR)

/*********************************************************************
 * @fn      ugap_monitorChangeState
 *
 * @brief   Change Monitor State. If successful, post UGAP_MONITOR_EVT_STATE_CHANGE
 *          event.
 *
 * @param   state - the new state to switch to
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE
 */
static bStatus_t ugap_monitorChangeState(ugapMonitorState_t state)
{
  port_key_t key;

  bStatus_t status = SUCCESS;

  key = port_enterCS_SW();

  switch (state)
  {
  case UGAP_MONITOR_STATE_INITIALIZED:
    ugmState = state;
    break;

  case UGAP_MONITOR_STATE_IDLE:
    ugmState = state;
    break;

  case UGAP_MONITOR_STATE_MONITORING:
    /* Change start scan on next channel. */
    status = ull_monitorStart(ubleParams.monitorChan);

    if (status == SUCCESS)
    {
      ugmState = state;
    }
    break;

  default:
    status = INVALIDPARAMETER;
    break;
  }

  if (status == SUCCESS)
  {
    ugapMsgStateChange_t msgStateChange;

    msgStateChange.state = ugmState;
    status = uble_buildAndPostEvt(UBLE_EVTDST_GAP, UGAP_MONITOR_EVT_STATE_CHANGE,
                                  (ubleMsg_t*) &msgStateChange, sizeof(msgStateChange));
  }

  port_exitCS_SW(key);

  return status;
}
#endif /* FEATURE_MONITOR */

/*********************************************************************
 * CALLBACKS
 */

/*********************************************************************
 * @fn      ugb_clockHandler
 *
 * @brief   Handles a clock event.
 *
 * @param   a0 - event
 *
 * @return  None
 */
void ugap_clockHandler(uint32_t a0)
{
#if defined(FEATURE_BROADCASTER)
  /* Process Broadcaster Duty Control */
  if (a0 == UGAP_CLKEVT_BCAST_DUTY)
  {
    if (ugbState == UGAP_BCAST_STATE_ADVERTISING)
    {
      /* ugbDutyOffTime could have been changed during advertising state,
         so should be checked here. */
      if (ugbDutyOffTime != 0)
      {
        if (ugap_bcastChangeState(UGAP_BCAST_STATE_WAITING) != SUCCESS)
        {
          /* What to do if failed? */
        }
      }
    }
    else /* if (ugbState == UGAP_BCAST_STATE_WAITING) */
    {
      if (ugap_bcastChangeState(UGAP_BCAST_STATE_ADVERTISING) != SUCCESS)
      {
        /* What to do if failed? */
      }
    }
  }
#endif /* FEATURE_BROADCASTER */

#if defined(FEATURE_OBSERVER)
  /* Process Scan Timer control */
  if (a0 == UGAP_CLKEVT_SCAN_TIMER)
  {
    if (ugoState == UGAP_SCAN_STATE_SCANNING)
    {
      /* ugoScanOffTime could have been changed during scanning state,
         so should be checked here. */
      if (ugoScanOffTime != 0)
      {
        if (ugap_observerChangeState(UGAP_SCAN_STATE_WAITING) != SUCCESS)
        {
          /* What to do if failed? */
        }
      }
    }
    else if (ugoState == UGAP_SCAN_STATE_WAITING)
    {
      if (ugap_observerChangeState(UGAP_SCAN_STATE_SCANNING) != SUCCESS)
      {
        /* What to do if failed? */
      }
    }
  }
#endif /* FEATURE_OBSERVER */
}

#if defined(FEATURE_BROADCASTER)
/*********************************************************************
 * @fn      ugb_advPrepareCB
 *
 * @brief   Do pre-processing of advertising event
 *
 * @param   None
 *
 * @return  None
 */
void ugb_advPrepareCB(void)
{
  port_key_t key;

  key = port_enterCS_SW();
  if (ubleParams.timeToAdv > 0)
  {
    /* Post UGB_EVT_ADV_PREPARE to itself so that it is processed
       in the application task's context */
    uble_buildAndPostEvt(UBLE_EVTDST_GAP, UGB_EVT_ADV_PREPARE, NULL, 0);
  }
  port_exitCS_SW(key);
}

/*********************************************************************
 * @fn      ugb_advPostprocessCB
 *
 * @brief   Do post-processing of advertising event
 *
 * @param   status - result of the last advertising event
 *
 * @return  none
 */
void ugb_advPostprocessCB(bStatus_t status)
{
  ugapBcastMsgAdvPostprocess_t msg;
  port_key_t key;

  key = port_enterCS_SW();
#ifndef RF_MULTIMODE
  if (status == bleNoResources)
  {
    /* Failed to schedule next advertising event. Switch the state to Idle. */
    ugap_bcastChangeState(UGAP_BCAST_STATE_IDLE);
  }
  else
#endif /* !RF_MULTIMODE */
  {
    msg.status = status;
    /* Post UGB_EVT_ADV_POSTPROCESS to itself so that it is processed
       in the application task's context */
    uble_buildAndPostEvt(UBLE_EVTDST_GAP, UGB_EVT_ADV_POSTPROCESS,
                         (ubleMsg_t*) &msg, sizeof(ugapBcastMsgAdvPostprocess_t));

    if (ugbNumAdvEvent > 0)
    {
      if (--ugbNumAdvEvent == 0)
      {
        ugap_bcastChangeState(UGAP_BCAST_STATE_IDLE);
      }
    }
  }
  port_exitCS_SW(key);
}
#endif /* FEATURE_BROADCASTER */

#if defined(FEATURE_OBSERVER)
/*********************************************************************
 * @fn      ugap_observerScanIndicationCB
 *
 * @brief   Do scan indication event
 *
 * @param status status of a scan
 * @param len length of the payload
 * @param pPayload pointer to payload
 *
 * @return  None
 */
void ugap_observerScanIndicationCB(bStatus_t status, uint8_t len, uint8_t *pPayload)
{
  ugapObserverMsgScanIndication_t msg;
  port_key_t key;

  key = port_enterCS_SW();
  msg.status = status;
  msg.len = len;
  msg.pPayload = pPayload;

  /* Post UGAP_OBSERVER_EVT_SCAN_INDICATION to itself so that it is processed
     in the application task's context */
  uble_buildAndPostEvt(UBLE_EVTDST_GAP, UGAP_OBSERVER_EVT_SCAN_INDICATION,
                       (ubleMsg_t*) &msg, sizeof(ugapObserverMsgScanIndication_t));
  port_exitCS_SW(key);
}

/*********************************************************************
 * @fn      ugap_observerScanWindowCompleteCB
 *
 * @brief   Do scan complete event
 *
 * @param   status - result of the scan complete event
 *
 * @return  none
 */
static void ugap_observerScanWindowCompleteCB(bStatus_t status)
{
  ugapObserverMsgScanWindowComplete_t msg;
  port_key_t key;

  key = port_enterCS_SW();
  msg.status = status;
  /* Post UGB_EVT_SCAN_WINDOW_COMPLETE to itself so that it is processed
     in the application task's context */
  uble_buildAndPostEvt(UBLE_EVTDST_GAP, UGAP_OBSERVER_EVT_SCAN_WINDOW_COMPLETE,
                       (ubleMsg_t*) &msg, sizeof(ugapObserverMsgScanWindowComplete_t));
  port_exitCS_SW(key);
}
#endif /* FEATURE_OBSERVER */

#if defined(FEATURE_MONITOR)
/*********************************************************************
 * @fn      upag_monitorIndicationCB
 *
 * @brief   Do monitor indication event
 *
 * @param status status of a monitoring scan
 * @param sessionId
 * @param len length of the payload
 * @param pPayload pointer to payload
 *
 * @return  None
 */
static void upag_monitorIndicationCB(bStatus_t status, uint8_t sessionId, uint8_t len, uint8_t *pPayload)
{
  ugapMsgMonitorIndication_t msg;
  port_key_t key;

  key = port_enterCS_SW();
  msg.status = status;
  msg.sessionId = sessionId;
  msg.len = len;
  msg.pPayload = pPayload;

  /* Post UGAP_MONITOR_EVT_MONITOR_INDICATION to itself so that it is processed
     in the application task's context */
  uble_buildAndPostEvt(UBLE_EVTDST_GAP, UGAP_MONITOR_EVT_MONITOR_INDICATION,
                       (ubleMsg_t*) &msg, sizeof(ugapMsgMonitorIndication_t));
  port_exitCS_SW(key);
}

/*********************************************************************
 * @fn      ugap_monitorCompleteCB
 *
 * @brief   Do monitor complete event
 *
 * @param   status - result of the monitor complete event
 *
 * @return  none
 */
static void ugap_monitorCompleteCB(bStatus_t status, uint8_t sessionId)
{
  ugapMsgMonitorComplete_t msg;
  port_key_t key;

  key = port_enterCS_SW();
  msg.status = status;
  msg.sessionId = sessionId;
  /* Post UGAP_MONITOR_EVT_MONITOR_COMPLETE to itself so that it is processed
     in the application task's context */
  uble_buildAndPostEvt(UBLE_EVTDST_GAP, UGAP_MONITOR_EVT_MONITOR_COMPLETE,
                       (ubleMsg_t*) &msg, sizeof(ugapMsgMonitorComplete_t));
  port_exitCS_SW(key);
}
#endif /* FEATURE_MONITOR */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

#if defined(FEATURE_BROADCASTER)
/*********************************************************************
 * @fn      ugap_bcastInit
 *
 * @brief   Initialize Micro Broadcaster
 *
 * @param   pCBs - a set of application callbacks for Broadcaster
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE
 */
bStatus_t ugap_bcastInit(ugapBcastCBs_t* pCBs)
{
  if (ugbState != UGAP_BCAST_STATE_INVALID &&
      ugbState != UGAP_BCAST_STATE_INITIALIZED &&
      ugbState != UGAP_BCAST_STATE_IDLE)
  {
    /* Cannot re-initialize if the state is one of the active ones */
    return FAILURE;
  }

  cBcastDuty = port_timerCreate(ugap_clockHandler, UGAP_CLKEVT_BCAST_DUTY);

  if (pCBs != NULL)
  {
    /* Register application callbacks to be called by Broadcaster */
    memcpy(&ugbAppCBs, pCBs, sizeof(ugapBcastCBs_t));
  }

  /* Register advertiser-related uGAP callback in the uLL */
  ull_advRegisterCB(ugb_advPrepareCB, ugb_advPostprocessCB);

  return ugap_bcastChangeState(UGAP_BCAST_STATE_INITIALIZED);
}

/*********************************************************************
 * @fn      ugap_bcastSetDuty
 *
 * @brief   Set Broadcaster Duty On/Off time
 *
 * @param   dutyOnTime  - Time period during which the Broadcaster
 *                        is in Advertising state. 100 ms unit. This cannot be
 *                        0 unless dutyOffTime is also 0.
 *          dutyOffTime - Time period during which the Broadcaster
 *                        is in Waiting state. 100 ms unit. If this is 0,
 *                        Duty Control is disabled regardless of dutyOnTime.
 *
 * @return  SUCCESS or INVALIDPARAMETER
 */
bStatus_t ugap_bcastSetDuty(uint16 dutyOnTime, uint16 dutyOffTime)
{
  if (dutyOnTime == 0 && dutyOffTime != 0)
  {
    return INVALIDPARAMETER;
  }

  ugbDutyOnTime = dutyOnTime;

  if (ugbState == UGAP_BCAST_STATE_ADVERTISING &&
      ugbDutyOffTime == 0 && dutyOffTime != 0)
  {
    /* If duty control gets enabled while advertising,
       start from WAITING state */
    ugbDutyOffTime = dutyOffTime;
    ugap_bcastChangeState(UGAP_BCAST_STATE_WAITING);
  }
  else
  {
    ugbDutyOffTime = dutyOffTime;
  }

  return SUCCESS;
}

/*********************************************************************
 * @fn      ugap_bcastStart
 *
 * @brief   Start Broadcaster. Proceed the state from either Initialized or IDLE
 *          to Advertising.
 *
 * @param   numAdvEvent - # of Advertising events to be performed before
 *                        the Broadcaster state goes to IDLE. If this is 0,
 *                        the Broadcaster will keep staying in Advertising or
 *                        Advertising and Waiting alternately unless an error
 *                        happens or the application requests to stop.
 *
 * @return  SUCCESS, FAILURE, or INVALIDPARAMETER
 */
bStatus_t ugap_bcastStart(uint16 numAdvEvent)
{
  /* Micro Broadcaster can start only from UGAP_BCAST_STATE_INITIALIZED state */
  if (ugbState != UGAP_BCAST_STATE_INITIALIZED && ugbState != UGAP_BCAST_STATE_IDLE)
  {
    return FAILURE;
  }

  ugbNumAdvEvent = numAdvEvent;

  return ugap_bcastChangeState(UGAP_BCAST_STATE_ADVERTISING);
}

/*********************************************************************
 * @fn      ugap_bcastStop
 *
 * @brief   Stop Broadcaster. Proceed the state from either Advertising or
 *          Waiting to IDLE.
 *
 * @return  SUCCESS, FAILURE, or INVALIDPARAMETER
 */
bStatus_t ugap_bcastStop(void)
{
  /* Micro Broadcaster should have ever been initialized */
  if (ugbState == UGAP_BCAST_STATE_INVALID ||
      ugbState == UGAP_BCAST_STATE_INITIALIZED)
  {
    return FAILURE;
  }

  return ugap_bcastChangeState(UGAP_BCAST_STATE_IDLE);
}
#endif /* FEATURE_BROADCASTER */

#if defined(FEATURE_OBSERVER)
/*********************************************************************
 * @fn      ugap_scanInit
 *
 * @brief   Initialize Micro Observer
 *
 * @param   pCBs - a set of application callbacks for Ovserver
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE
 */
bStatus_t ugap_scanInit(ugapObserverScanCBs_t* pCBs)
{
  if (ugoState != UGAP_SCAN_STATE_INVALID &&
      ugoState != UGAP_SCAN_STATE_INITIALIZED &&
      ugoState != UGAP_SCAN_STATE_IDLE)
  {
    /* Cannot re-initialize if the state is one of the active ones */
    return FAILURE;
  }

  cScanTimer = port_timerCreate(ugap_clockHandler, UGAP_CLKEVT_SCAN_TIMER);

  if (pCBs != NULL)
  {
    /* Register application callbacks to be called by Broadcaster */
    memcpy(&ugoAppCBs, pCBs, sizeof(ugapObserverScanCBs_t));
  }

  /* Register observer-related uGAP callback in the uLL */
  ull_scanRegisterCB(ugap_observerScanIndicationCB, ugap_observerScanWindowCompleteCB);

  return ugap_observerChangeState(UGAP_SCAN_STATE_INITIALIZED);
}

/*********************************************************************
 * @fn      ugap_scanStart
 *
 * @brief   Start Observer scanning. Proceed the state from either Initialized
 *          or IDLE to Scanning.
 *
 * @return  SUCCESS, FAILURE, or INVALIDPARAMETER
 */
bStatus_t ugap_scanStart(void)
{
  /* Micro Ovserver can start only from UGAP_OBSERVER_STATE_INITIALIZED state */
  if (ugoState != UGAP_SCAN_STATE_INITIALIZED && ugoState != UGAP_SCAN_STATE_IDLE)
  {
    return FAILURE;
  }

  return ugap_observerChangeState(UGAP_SCAN_STATE_SCANNING);
}

/*********************************************************************
 * @fn      ugap_scanStop
 *
 * @brief   Stop Observer scanning. Proceed the state from Scanning to Idle.
 *
 * @return  SUCCESS, FAILURE, or INVALIDPARAMETER
 */
bStatus_t ugap_scanStop(void)
{
  /* Micro Observer should have ever been initialized */
  if (ugoState == UGAP_SCAN_STATE_INVALID || ugoState == UGAP_SCAN_STATE_INITIALIZED)
  {
    return FAILURE;
  }

  return ugap_observerChangeState(UGAP_SCAN_STATE_IDLE);
}

/*********************************************************************
 * @fn      ugap_scanSuspend
 *
 * @brief   Suspend Observer scanning. Proceed the state from either Waiting
 *          or Scanning to Suspended. Suspend the Obesrver to make room
 *          for the Advertiser to transmit.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ugap_scanSuspend(void)
{
  /* Micro Observer should have ever been initialized */
  if (ugoState != UGAP_SCAN_STATE_WAITING && ugoState != UGAP_SCAN_STATE_SCANNING)
  {
    return FAILURE;
  }

  ugoStatePrev = ugoState;
  return ugap_observerChangeState(UGAP_SCAN_STATE_SUSPENDED);
}

/*********************************************************************
 * @fn      ugap_scanResume
 *
 * @brief   Resume a suspended Observer. Proceed the state from Suspended
 *          to Scanning to Waiting.
 *
 * @return  SUCCESS or FAILURE
 */
bStatus_t ugap_scanResume(void)
{
  bStatus_t status = FAILURE;

  /* Micro Observer should have ever been suspended */
  if (ugoState == UGAP_SCAN_STATE_SUSPENDED)
  {
    if (ugoStatePrev == UGAP_SCAN_STATE_WAITING)
    {
      status = ugap_observerChangeState(UGAP_SCAN_STATE_WAITING);
    }
    if (ugoStatePrev == UGAP_SCAN_STATE_SCANNING)
    {
      status = ugap_observerChangeState(UGAP_SCAN_STATE_SCANNING);
    }
  }
  return status;
}

/*********************************************************************
 * @fn      ugap_scanRequest
 *
 * @brief   uGAP scan request
 *
 * @param   scanChanMap - Channels to scan advertising PDUs. One of more
 *                        of channels 37, 38 & 39. If 0, ubParam.scanChanMap is used.
 *          scanWindow - Scan window to listen on a primary advertising
 *                       channel index for the duration of the scan window.
 *                       Unit in 0.625 ms; range from 2.5 ms to 40.959375 s.
 *                       If 0, ubParam.scanWindow is used.
 *          scanInterval - Scan interval. This is defined as the start of
 *                         two consecutive scan windows. Unit in 0.625 ms;
 *                         range from 2.5 ms to 40.959375 s. When scanInterval
 *                         is the same as sacnWindow, a continuous scan
 *                         is requested. If 0, ubParam.scanInterval is used.
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE
 */
bStatus_t ugap_scanRequest(uint8_t scanChanMap, uint16_t scanWindow, uint16_t scanInterval)
{
  if (ugoState == UGAP_SCAN_STATE_SCANNING)
  {
    return FAILURE;
  }

  if (scanWindow > scanInterval)
  {
    return INVALIDPARAMETER;
  }

  if (scanChanMap & UBLE_ADV_CHAN_ALL)
  {
    /* Override ubParam if there is some channel to scan */
    ubleParams.scanChanMap = scanChanMap;
  }

  if (scanWindow > 0)
  {
    if (scanWindow >= UBLE_MIN_SCAN_WINDOW && scanWindow <= UBLE_MAX_SCAN_WINDOW)
    {
      /* Override ubParam if scanWindow is in range */
      ubleParams.scanWindow = scanWindow;
    }
    else
    {
      return INVALIDPARAMETER;
    }
  }

  if (scanInterval > 0)
  {
    if (scanInterval >= UBLE_MIN_SCAN_INTERVAL && scanInterval <= UBLE_MAX_SCAN_INTERVAL)
    {
      /* Override ubParam if scanInterval is in range */
      ubleParams.scanInterval = scanInterval;
    }
    else
    {
      return INVALIDPARAMETER;
    }
  }

  ugoScanOnTime = ubleParams.scanWindow;
  ugoScanOffTime = ubleParams.scanInterval - ubleParams.scanWindow;
  ugoScanChanMap = ubleParams.scanChanMap;

  /* Start scanning */
  return ugap_scanStart();
}
#endif /* FEATURE_OBSERVER */

#if defined(FEATURE_MONITOR)
/*********************************************************************
 * @fn      ugap_monitorInit
 *
 * @brief   Initialize Micro Monitor
 *
 * @param   pCBs - a set of application callbacks for Monitor
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE
 */
bStatus_t ugap_monitorInit(ugapMonitorCBs_t* pCBs)
{
  if (ugmState != UGAP_MONITOR_STATE_INVALID &&
      ugmState != UGAP_MONITOR_STATE_INITIALIZED &&
      ugmState != UGAP_MONITOR_STATE_IDLE)
  {
    /* Cannot re-initialize if the state is one of the active ones */
    return FAILURE;
  }

  if (pCBs != NULL)
  {
    /* Register application callbacks to be called by Broadcaster */
    memcpy(&ugmAppCBs, pCBs, sizeof(ugapMonitorCBs_t));
  }

  /* Register monitor-related uGAP callback in the uLL */
  ull_monitorRegisterCB(upag_monitorIndicationCB, ugap_monitorCompleteCB);

  return ugap_monitorChangeState(UGAP_MONITOR_STATE_INITIALIZED);
}

/*********************************************************************
 * @fn      ugap_monitorStart
 *
 * @brief   Start Monitoring scan. Proceed the state from either Initialized
 *          or IDLE to Monitoring.
 *
 * @return  SUCCESS, FAILURE, or INVALIDPARAMETER
 */
bStatus_t ugap_monitorStart(void)
{
  return ugap_monitorChangeState(UGAP_MONITOR_STATE_MONITORING);
}

/*********************************************************************
 * @fn      ugap_monitorStop
 *
 * @brief   Stop Monitoring scan. Proceed the state from Monitoring to Idle.
 *
 * @return  SUCCESS, FAILURE, or INVALIDPARAMETER
 */
bStatus_t ugap_monitorStop(void)
{
  /* Micro Monitor should have not been initialized */
  if (ugmState == UGAP_MONITOR_STATE_INVALID || ugmState == UGAP_MONITOR_STATE_INITIALIZED)
  {
    return FAILURE;
  }

  return ugap_monitorChangeState(UGAP_MONITOR_STATE_IDLE);
}

/*********************************************************************
 * @fn      ugap_monitorRequest
 *
 * @brief   uGAP monitor request
 *
 * @param   channel - Channels to monitor.
 *          accessAddr - Access address. 0xFFFFFFFF is used as a
 *                       wildcard access address. An error status will
 *                       be returned if the channel is one of 37, 38,
 *                       or 39 but the accessAddr is not 0x8E89BED6.
 *          startTime - Absolute start time in RAT ticks.
 *          duration - Scan window per scanChanIndexchanIndex. A monitor
 *                     complete time-out statuscallback will be sent to
 *                     the application. A zero indicates a continuous
 *                     scan. Unit in 0.625 ms; range from 2.5 ms to
 *                     40.959375 s.
 *
 * @return  SUCCESS or INVALIDPARAMETER
 */
bStatus_t ugap_monitorRequest(uint8_t  channel,
                              uint32_t accessAddr,
                              uint32_t startTime,
                              uint16_t duration,
                              uint32_t crcInit)
{
  /* An error status will be returned if the channel is one of 37, 38,
   * or 39 but the accessAddr is not 0x8E89BED6.
   */
  if (((channel == 37) || (channel == 38) || (channel == 39)) &&
      (accessAddr != 0x8E89BED6))
  {
    return INVALIDPARAMETER;
  }

  /* Override ubParam */
  ubleParams.monitorChan = channel;
  ubleParams.startTime = startTime;
  ubleParams.accessAddr = accessAddr;
  ubleParams.crcInit = crcInit;

  if (duration <= UBLE_MAX_MONITOR_DURATION)
  {
    /* Override ubParam if duration is in range */
    ubleParams.monitorDuration = duration;
  }
  else
  {
    return INVALIDPARAMETER;
  }

  /* Start monitor scan */
  return ugap_monitorStart();
}
#endif /* FEATURE_MONITOR */

/*********************************************************************
 * @fn      ugap_processGAPMsg
 *
 * @brief   Process event messages sent from Micro BLE Stack to uGAP through
 *          the application's Stack Event Proxy and uble_processMsg().
 *
 * @param   pEvtMsg - Pointer to the Micro BLE Stack event message destined to
 *                    uGAP.
 *
 * @return  none
 */
void uble_processGAPMsg(ubleEvtMsg_t *pEvtMsg)
{
  switch (pEvtMsg->hdr.evt)
  {
#if defined(FEATURE_BROADCASTER)
  case UGB_EVT_ADV_PREPARE:
    if (ugbAppCBs.pfnAdvPrepareCB != NULL)
    {
      ugbAppCBs.pfnAdvPrepareCB();
    }
    break;

  case UGB_EVT_ADV_POSTPROCESS:
    if (ugbAppCBs.pfnAdvDoneCB != NULL)
    {
      ugapBcastMsgAdvPostprocess_t *pMsg = (ugapBcastMsgAdvPostprocess_t*) &(pEvtMsg->msg);
      ugbAppCBs.pfnAdvDoneCB(pMsg->status);
    }
    break;

  case UGB_EVT_STATE_CHANGE:
    if (ugbAppCBs.pfnStateChangeCB != NULL)
    {
      ugapBcastMsgStateChange_t *pMsg = (ugapBcastMsgStateChange_t*) &(pEvtMsg->msg);
      ugbAppCBs.pfnStateChangeCB(pMsg->state);
    }
    break;
#endif /* FEATURE_BROADCASTER */

#if defined(FEATURE_OBSERVER)
  case UGAP_OBSERVER_EVT_SCAN_INDICATION:
    if (ugoAppCBs.pfnScanIndicationCB != NULL)
    {
      ugapObserverMsgScanIndication_t *pMsg = (ugapObserverMsgScanIndication_t*) &(pEvtMsg->msg);
      ugoAppCBs.pfnScanIndicationCB(pMsg->status, pMsg->len, pMsg->pPayload);

      /* This will allow the next adv receive */
      Ull_advPktInuse = false;
    }
    break;

  case UGAP_OBSERVER_EVT_SCAN_WINDOW_COMPLETE:
    /* The App scan window complete is called at the end of scan window */
    if (ugoAppCBs.pfnScanWindowCompleteCB != NULL)
    {
      ugapObserverMsgScanWindowComplete_t *pMsg = (ugapObserverMsgScanWindowComplete_t*) &(pEvtMsg->msg);
      ugoAppCBs.pfnScanWindowCompleteCB(pMsg->status);
    }
    break;

  case UGAP_OBSERVER_EVT_STATE_CHANGE:
    if (ugoAppCBs.pfnStateChangeCB != NULL)
    {
      ugapObserverMsgStateChange_t *pMsg = (ugapObserverMsgStateChange_t*) &(pEvtMsg->msg);
      ugoAppCBs.pfnStateChangeCB(pMsg->state);
    }
    break;
#endif /* FEATURE_OVSERVER */

#if defined(FEATURE_MONITOR)
  case UGAP_MONITOR_EVT_MONITOR_INDICATION:
    if (ugmAppCBs.pfnMonitorIndicationCB != NULL)
    {
      ugapMsgMonitorIndication_t *pMsg = (ugapMsgMonitorIndication_t*) &(pEvtMsg->msg);
      ugmAppCBs.pfnMonitorIndicationCB(pMsg->status, pMsg->sessionId, pMsg->len, pMsg->pPayload);
    }
    break;

  case UGAP_MONITOR_EVT_MONITOR_COMPLETE:
    /* The App monitor complete is called at the end of monitor duration */
    if (ugmAppCBs.pfnMonitorCompleteCB != NULL)
    {
      ugapMsgMonitorComplete_t *pMsg = (ugapMsgMonitorComplete_t*) &(pEvtMsg->msg);
      ugmAppCBs.pfnMonitorCompleteCB(pMsg->status, pMsg->sessionId);
    }
    break;

  case UGAP_MONITOR_EVT_STATE_CHANGE:
    if (ugmAppCBs.pfnStateChangeCB != NULL)
    {
      ugapMsgStateChange_t *pMsg = (ugapMsgStateChange_t*) &(pEvtMsg->msg);
      ugmAppCBs.pfnStateChangeCB(pMsg->state);
    }
    break;
#endif /* FEATURE_MONITOR */

  default:
    break;
  }
}


/*********************************************************************
*********************************************************************/
