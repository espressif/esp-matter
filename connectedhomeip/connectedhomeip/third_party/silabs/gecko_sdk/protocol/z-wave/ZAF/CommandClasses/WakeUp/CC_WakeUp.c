/**
 * @file CC_WakeUp.c
 * @brief Handler for Command Class Wake Up.
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <CC_WakeUp.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "Assert.h"
#include <CC_Common.h>
#include <misc.h>
#include <ZW_TransportSecProtocol.h>
#include <ZW_transport_api.h>
#include <AppTimer.h>
#include <ZAF_Common_interface.h>
#include <ZAF_file_ids.h>
#include "ZAF_tx_mutex.h"

//#define DEBUGPRINT
#include "DebugPrint.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/* [sec] We should avoid sleeping close or less than 10 seconds with a buffer of 1 second.
 * This is due to POST_INCLUSION_STAY_AWAKE_TIME. */
#define MINIMUM_SLEEP_TIME                11     // [sec]

#define POST_INCLUSION_STAY_AWAKE_TIME    10000  // [ms]

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

static uint32_t wakeUpSettings[WAKEUP_PAR_COUNT];

static zpal_nvm_handle_t pFileSystem;
static zpal_pm_handle_t wake_up_cc_power_lock;
static bool wakeUpIsActive              = false;
static bool autoStayAwakeAfterInclusion = false;

static void (*ZCB_WakeUpTxCallback)(uint8_t txStatus, TX_STATUS_TYPE* pExtendedTxStatus);

static SApplicationHandles * m_pAppHandle;

static SWakeupCcData gWakeupCcData;

static SSwTimer WakeUpTimer;


/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

/**
 * Transmits a Wake Up Notification command.
 * @param pCallback Pointer to callback function to be called upon transmission.
 * @return Status of the job.
 */
static bool CmdClassWakeupNotification(void (*pCallback)(uint8_t txStatus, TX_STATUS_TYPE* pExtendedTxStatus));

static void TimerCallback(SSwTimer *pTimer)
{
#if !defined(DEBUGPRINT)
  UNUSED(pTimer);
#endif
  /* It is assumed that an Deep Sleep persistent timer is used with this call back
   * (see AppTimerDeepSleepPersistentRegister()). Otherwise this function will not
   * be called if the timer expires while the device is sleeping in deep sleep
   * hibernate.
   */

  DPRINTF("CC_WakeUp TimerCallback() pTimer->Id=%d\n", pTimer->Id);

  CC_WakeUp_notification_tx(NULL);
}

static void LoadData(void)
{
  const zpal_status_t status = zpal_nvm_read(pFileSystem, ZAF_FILE_ID_WAKEUPCCDATA, &gWakeupCcData, sizeof(SWakeupCcData));
  DPRINTF("\r\nE#%x", status);
  ASSERT(ZPAL_STATUS_OK == status);
}

void CC_WakeUp_init(EResetReason_t resetReason,
                    zpal_nvm_handle_t pFS)
{
  pFileSystem = pFS;
  m_pAppHandle = ZAF_getAppHandle();
  wakeUpIsActive = false;

  AppTimerDeepSleepPersistentRegister(&WakeUpTimer, false, TimerCallback);
  wake_up_cc_power_lock = zpal_pm_register(ZPAL_PM_TYPE_USE_RADIO);

  //Verify that a WAKEUPCCDATA file exists
  size_t   dataLen;
  const zpal_status_t status = zpal_nvm_get_object_size(pFileSystem, ZAF_FILE_ID_WAKEUPCCDATA, &dataLen);
  //If there is no file or there is size mismatch write a default file
  if ((ZPAL_STATUS_OK != status) || (ZAF_FILE_SIZE_WAKEUPCCDATA != dataLen))
  {
    CC_WakeUp_notificationMemorySetDefault(pFileSystem);
  }

  LoadData();

  if ((ERESETREASON_POWER_ON == resetReason) ||
      (ERESETREASON_BROWNOUT == resetReason) ||
      (ERESETREASON_PIN == resetReason) ||
      (ERESETREASON_SOFTWARE == resetReason) ||
      (ERESETREASON_WATCHDOG == resetReason))
  {
    CC_WakeUp_notification_tx(NULL);
  }
}

/*
 * Callback function for sending wakeup notification
 *
 * Called from transport layer (when processing ack) or from
 * CC_WakeUp_notification_tx if tx queue is full.
 */
void
ZCB_WakeUpNotificationCallback( uint8_t txStatus, TX_STATUS_TYPE* pExtendedTxStatus )
{
  DPRINTF("\nWakeUpNotificationCallback() txStatus: %u", txStatus);

  if (NULL != ZCB_WakeUpTxCallback)
  {
    ZCB_WakeUpTxCallback(txStatus, pExtendedTxStatus);
  }
}

/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

void CC_WakeUp_notification_tx(void (*pCallback)(uint8_t txStatus, TX_STATUS_TYPE* pExtendedTxStatus))
{
  /* Only send wakeup notifiers when sensor is node in a network */
  /* and a recovery operation is not in progress */
  if (0 == m_pAppHandle->pNetworkInfo->NodeId)
  {
    if (NULL != pCallback)
    {
      TX_STATUS_TYPE extendedTxStatus;
      memset(&extendedTxStatus, 0, sizeof(TX_STATUS_TYPE));
      uint8_t txStatus = TRANSMIT_COMPLETE_FAIL;
      pCallback(txStatus, &extendedTxStatus);
    }
    return;
  }

  ZCB_WakeUpTxCallback = pCallback;
  wakeUpIsActive = true;
  CC_WakeUp_stayAwakeIfActive();

  if (true != CmdClassWakeupNotification(ZCB_WakeUpNotificationCallback))
  {
    TX_STATUS_TYPE extendedTxStatus;
    memset(&extendedTxStatus, 0, sizeof(TX_STATUS_TYPE));
    uint8_t txStatus = TRANSMIT_COMPLETE_FAIL;
    ZCB_WakeUpNotificationCallback(txStatus, &extendedTxStatus);
  }
}

static bool CmdClassWakeupNotification(void (*pCallback)(uint8_t txStatus, TX_STATUS_TYPE* pExtendedTxStatus))
{
  DPRINTF("\r\nN%d", gWakeupCcData.MasterNodeId);

  uint8_t WakeUpNotificationFrame[] = {
                                       COMMAND_CLASS_WAKE_UP,
                                       WAKE_UP_NOTIFICATION_V2
  };

  SZwaveTransmitPackage FramePackage;

  if ((0x00 < gWakeupCcData.MasterNodeId) && (ZW_MAX_NODES >= gWakeupCcData.MasterNodeId))
  {
    FramePackage.uTransmitParams.SendDataEx.DestNodeId = gWakeupCcData.MasterNodeId;
  }
  else // CC:0084.01.00.32.003
  {
    FramePackage.uTransmitParams.SendDataEx.DestNodeId = 1;
  }
  FramePackage.uTransmitParams.SendDataEx.FrameConfig.Handle = pCallback;
  FramePackage.uTransmitParams.SendDataEx.FrameConfig.TransmitOptions = ZWAVE_PLUS_TX_OPTIONS;
  memcpy(FramePackage.uTransmitParams.SendDataEx.FrameConfig.aFrame,
         WakeUpNotificationFrame,
         sizeof(WakeUpNotificationFrame));
  FramePackage.uTransmitParams.SendDataEx.FrameConfig.iFrameLength = sizeof(WakeUpNotificationFrame);
  FramePackage.uTransmitParams.SendDataEx.SourceNodeId = 0x00;
  FramePackage.uTransmitParams.SendDataEx.TransmitOptions2 = 0x00;
  FramePackage.uTransmitParams.SendDataEx.TransmitSecurityOptions = S2_TXOPTION_VERIFY_DELIVERY;
  FramePackage.uTransmitParams.SendDataEx.eKeyType = GetHighestSecureLevel(m_pAppHandle->pNetworkInfo->SecurityKeys);

  FramePackage.eTransmitType = EZWAVETRANSMITTYPE_EX;

  // Put the package on queue (and dont wait for it)
  DPRINT("\r\nQNSTB");
  if (EQUEUENOTIFYING_STATUS_SUCCESS != QueueNotifyingSendToBack(m_pAppHandle->pZwTxQueue,
                                                                 (uint8_t*)&FramePackage,
                                                                 0))
  {
    return false;
  }
  return true;
}

void
CC_WakeUp_setConfiguration(WAKEUP_PAR type, uint32_t time)
{
  /* Called from the APP (multiple times, once for each wakeup parameter)
   * telling the CC about wakeup parameters */

  if (type >= WAKEUP_PAR_COUNT)
  {
    DPRINT("Invalid parameter\n");
    ASSERT(0);  // Invalid parameter
    return;
  }

  // Set lower bounds to the default sleep time and minimum sleep time.
  if( (WAKEUP_PAR_DEFAULT_SLEEP_TIME == type || WAKEUP_PAR_MIN_SLEEP_TIME == type)
      && (MINIMUM_SLEEP_TIME > time) ) {
    time = MINIMUM_SLEEP_TIME;
  }

  wakeUpSettings[type] = time;
}

void TimerCallbackDummy(void)
{
  // Do nothing for now

}

static received_frame_status_t
CC_WakeUp_handler(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_APPLICATION_TX_BUFFER *pCmd,
  uint8_t cmdLength)
{
  ZAF_TRANSPORT_TX_BUFFER  TxBuf;
  ZW_APPLICATION_TX_BUFFER *pTxBuf = &(TxBuf.appTxBuf);
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX * pTxOptionsEx;

  UNUSED(cmdLength);

  switch(pCmd->ZW_Common.cmd)
  {
    case  WAKE_UP_INTERVAL_SET_V2:
      {
        uint32_t requestedSleepPeriod = 0;
        uint32_t setSleepPeriod = 0;

        // Always AND with FF the right place... Then we don't care whether 1's are shifted in.
        requestedSleepPeriod |= (((uint32_t)pCmd->ZW_WakeUpIntervalSetV2Frame.seconds1) << 16) & 0x00FF0000;
        requestedSleepPeriod |= (((uint32_t)pCmd->ZW_WakeUpIntervalSetV2Frame.seconds2) << 8)  & 0x0000FF00;
        requestedSleepPeriod |= (((uint32_t)pCmd->ZW_WakeUpIntervalSetV2Frame.seconds3) << 0)  & 0x000000FF;

        /* Calculate correct sleep-period dependent of step resolution */
        if (requestedSleepPeriod > 0)
        {
          if (requestedSleepPeriod < wakeUpSettings[WAKEUP_PAR_MIN_SLEEP_TIME])
          {
            setSleepPeriod = wakeUpSettings[WAKEUP_PAR_MIN_SLEEP_TIME];
          }
          else if (requestedSleepPeriod > wakeUpSettings[WAKEUP_PAR_MAX_SLEEP_TIME])
          {
            setSleepPeriod = wakeUpSettings[WAKEUP_PAR_MAX_SLEEP_TIME];
          }
          else if (0 == wakeUpSettings[WAKEUP_PAR_SLEEP_STEP])
          {
            /* If STEP is 0 then only MIN or MAX is allowed.
             * Choose the one closest to the requested sleepPeriod value.
             * (the validations above ensures that sleepPeriod is not outside [MIN; MAX]
             */
            if ( (requestedSleepPeriod - wakeUpSettings[WAKEUP_PAR_MIN_SLEEP_TIME]) <
                 (wakeUpSettings[WAKEUP_PAR_MAX_SLEEP_TIME] - requestedSleepPeriod) )
            {
              setSleepPeriod = wakeUpSettings[WAKEUP_PAR_MIN_SLEEP_TIME];
            }
            else
            {
              setSleepPeriod = wakeUpSettings[WAKEUP_PAR_MAX_SLEEP_TIME];
            }
          }
          else
          {
            /**
             * The following formula ensures that the sleep period will always match a valid step
             * value.
             *
             *                       input - min
             * sleep_period = min + ------------- * step
             *                           step
             */
            setSleepPeriod = wakeUpSettings[WAKEUP_PAR_MIN_SLEEP_TIME] +
                ((requestedSleepPeriod - wakeUpSettings[WAKEUP_PAR_MIN_SLEEP_TIME]) / wakeUpSettings[WAKEUP_PAR_SLEEP_STEP]) * wakeUpSettings[WAKEUP_PAR_SLEEP_STEP];
          }
        }

        /*
         * Save master node ID and sleep period
         *
         * Since we're changing both variables in the struct, we do not need to read the file
         * before changing it. Inserting static assert to make sure changes in the struct size are
         * caught.
         */
        STATIC_ASSERT(sizeof(SWakeupCcData) == 8, STATIC_ASSERT_FAILED_wake_up_cc_struct_wrong_size);
        gWakeupCcData.MasterNodeId = pCmd->ZW_WakeUpIntervalSetV2Frame.nodeid;
        gWakeupCcData.SleepPeriod = setSleepPeriod;

        const zpal_status_t status = zpal_nvm_write(pFileSystem, ZAF_FILE_ID_WAKEUPCCDATA, &gWakeupCcData, sizeof(SWakeupCcData));
        ASSERT(ZPAL_STATUS_OK == status);

        CC_WakeUp_startWakeUpNotificationTimer();

        //Return failure in case requested sleep period could not be set.
        if(requestedSleepPeriod != setSleepPeriod)
        {
          return RECEIVED_FRAME_STATUS_FAIL;
        }
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;
    case WAKE_UP_INTERVAL_GET_V2:
      if(true == Check_not_legal_response_job(rxOpt))
      {
        // The Wake Up CC does not support endpoint bit addressing.
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      RxToTxOptions(rxOpt, &pTxOptionsEx);

      pTxBuf->ZW_WakeUpIntervalReportV2Frame.cmdClass = pCmd->ZW_Common.cmdClass;
      pTxBuf->ZW_WakeUpIntervalReportV2Frame.cmd = WAKE_UP_INTERVAL_REPORT_V2;

      pTxBuf->ZW_WakeUpIntervalReportV2Frame.seconds1 = (uint8_t)(gWakeupCcData.SleepPeriod >> 16); // MSB
      pTxBuf->ZW_WakeUpIntervalReportV2Frame.seconds2 = (uint8_t)(gWakeupCcData.SleepPeriod >> 8);
      pTxBuf->ZW_WakeUpIntervalReportV2Frame.seconds3 = (uint8_t)(gWakeupCcData.SleepPeriod >> 0); // LSB

      // Get wakeup destination node from file (saved by WAKE_UP_INTERVAL_SET_V2 - see above)
      pTxBuf->ZW_WakeUpIntervalReportV2Frame.nodeid = gWakeupCcData.MasterNodeId;

      if(ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
          (uint8_t *)pTxBuf,
          sizeof(ZW_WAKE_UP_INTERVAL_REPORT_FRAME),
          pTxOptionsEx,
          NULL))
      {
        ;
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    case WAKE_UP_NO_MORE_INFORMATION_V2:
      /*
       * The wakeup destination/master node has nothing more to send.
       * From the WakeUp CC perspective we're ready sleep immediately.
       * We signal that to the PM module by releasing our PM lock.
       */
      zpal_pm_cancel(wake_up_cc_power_lock);
      wakeUpIsActive = false;

      autoStayAwakeAfterInclusion = false;

      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;

    case WAKE_UP_INTERVAL_CAPABILITIES_GET_V2:
      if(true == Check_not_legal_response_job(rxOpt))
      {
        // The Wake Up CC does not support endpoint bit addressing.
        return RECEIVED_FRAME_STATUS_FAIL;
      }

      memset((uint8_t*)pTxBuf, 0, sizeof(ZW_APPLICATION_TX_BUFFER) );

      RxToTxOptions(rxOpt, &pTxOptionsEx);

      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.cmdClass = pCmd->ZW_Common.cmdClass;
      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.cmd = WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2;

      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.minimumWakeUpIntervalSeconds1 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_MIN_SLEEP_TIME] >> 16);
      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.minimumWakeUpIntervalSeconds2 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_MIN_SLEEP_TIME] >> 8);
      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.minimumWakeUpIntervalSeconds3 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_MIN_SLEEP_TIME] >> 0);

      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.maximumWakeUpIntervalSeconds1 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_MAX_SLEEP_TIME] >> 16);
      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.maximumWakeUpIntervalSeconds2 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_MAX_SLEEP_TIME] >> 8);
      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.maximumWakeUpIntervalSeconds3 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_MAX_SLEEP_TIME] >> 0);

      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.defaultWakeUpIntervalSeconds1 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_DEFAULT_SLEEP_TIME] >> 16);
      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.defaultWakeUpIntervalSeconds2 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_DEFAULT_SLEEP_TIME] >> 8);
      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.defaultWakeUpIntervalSeconds3 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_DEFAULT_SLEEP_TIME] >> 0);

      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.wakeUpIntervalStepSeconds1 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_SLEEP_STEP] >> 16);
      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.wakeUpIntervalStepSeconds2 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_SLEEP_STEP] >> 8);
      pTxBuf->ZW_WakeUpIntervalCapabilitiesReportV2Frame.wakeUpIntervalStepSeconds3 = (uint8_t)(wakeUpSettings[WAKEUP_PAR_SLEEP_STEP] >> 0);

      if (ZAF_ENQUEUE_STATUS_SUCCESS != Transport_SendResponseEP(
          (uint8_t *)pTxBuf,
          sizeof(ZW_WAKE_UP_INTERVAL_CAPABILITIES_REPORT_V2_FRAME),
          pTxOptionsEx,
          NULL))
      {
        ;
      }
      return RECEIVED_FRAME_STATUS_SUCCESS;
      break;
    default:
      // Do nothing.
      break;
  }
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}


void CC_WakeUp_notificationMemorySetDefault(zpal_nvm_handle_t pFS)
{
  DPRINT("\r\nCCWdef");
  pFileSystem = pFS;

  ASSERT(pFileSystem != 0);

  wakeUpIsActive = false;

  if (WakeUpTimer.pLiaison) // Has the timer been initialized?
  {
    // Ensure the wakeup timer is not running (OK to stop even if not running)
    AppTimerDeepSleepPersistentStop(&WakeUpTimer);
  }

  memset(&gWakeupCcData, 0, sizeof(SWakeupCcData));
  gWakeupCcData.SleepPeriod = wakeUpSettings[WAKEUP_PAR_DEFAULT_SLEEP_TIME];
  const zpal_status_t status = zpal_nvm_write(pFileSystem, ZAF_FILE_ID_WAKEUPCCDATA, &gWakeupCcData, sizeof(SWakeupCcData));
  ASSERT(ZPAL_STATUS_OK == status);

}

void
CC_WakeUp_stayAwakeIfActive(void)
{
  if ((true == wakeUpIsActive) || (true == autoStayAwakeAfterInclusion))
  {
    CC_WakeUp_stayAwake10s();
  }
  if (true == wakeUpIsActive)
  {
    CC_WakeUp_startWakeUpNotificationTimer();
  }
}

void
CC_WakeUp_stayAwake10s(void)
{
  /* Don't sleep the next 10 seconds */
  zpal_pm_stay_awake(wake_up_cc_power_lock, POST_INCLUSION_STAY_AWAKE_TIME);
}

void
CC_WakeUp_startWakeUpNotificationTimer(void)
{
  if (gWakeupCcData.SleepPeriod > 0)
  {
    AppTimerDeepSleepPersistentStart(&WakeUpTimer, gWakeupCcData.SleepPeriod * 1000);
  }
  else
  {
    AppTimerDeepSleepPersistentStop(&WakeUpTimer);
  }
}

void
CC_WakeUp_AutoStayAwakeAfterInclusion(void)
{
  autoStayAwakeAfterInclusion = true;
}

REGISTER_CC(COMMAND_CLASS_WAKE_UP, WAKE_UP_VERSION_V2, CC_WakeUp_handler);
