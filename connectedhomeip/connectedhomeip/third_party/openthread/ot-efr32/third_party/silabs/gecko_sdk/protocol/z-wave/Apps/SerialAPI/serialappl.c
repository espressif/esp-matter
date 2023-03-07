/**
 * @file
 * Serial API implementation for Enhanced Z-Wave module
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "zw_config_rf.h"
#include <string.h>
#include "SizeOf.h"
#include "Assert.h"
#include "EventDistributor.h"

#include <stdio.h>
#include "Min2Max2.h"

#include "SyncEvent.h"

#ifdef ZW_CONTROLLER
#include <ZW_controller_api.h>
#endif /* ZW_CONTROLLER */

#include "comm_interface.h"
#include <AppTimer.h>

#include <ZW_system_startup_api.h>

/* Include serialappl header file - containing version and */
/* SerialAPI functionality support definitions */
#include <serialappl.h>
#ifdef ZW_SECURITY_PROTOCOL
#include <ZW_security_api.h>
#include <ZW_TransportSecProtocol.h>
#endif

#include <ZW_application_transport_interface.h>
#include "DebugPrintConfig.h"
// SerialAPI uses SWO for debug output.
// For example SWO Terminal in  Studio commander can be used to get the output.
//#define DEBUGPRINT
#include <DebugPrint.h>
#include <app_node_info.h>
#include <virtual_slave_node_info.h>
#include <serialapi_file.h>
#include <cmd_handlers.h>
#include <cmds_management.h>
#include <cmds_security.h>
#include <nvm_backup_restore.h>
#include <cmds_rf.h>
#include <ZAF_Common_interface.h>
#include <ZAF_types.h>
#include <utils.h>
#include <zpal_watchdog.h>
#include <zpal_misc.h>
#include <SerialAPI_hw.h>
#include <board_indicator.h>

#include "zw_region_config.h"

#include "zw_build_no.h"

/* Basic level definitions */
#define BASIC_ON 0xFF
#define BASIC_OFF 0x00

#define TX_POWER_LR_20_DBM    200
#define TX_POWER_LR_14_DBM    140
/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/
#ifdef JP_DK
/* Define RSSI threshold so JP can be tested in DK */
#define JP_DK_RSSI_THRESHOLD 52
#endif

/**
 *
 */
typedef struct _S_TRANSPORT_REQUESTED_SECURITY_SETTINGS_
{
  uint8_t requestedSecurityKeysBits;
} S_TRANSPORT_REQUESTED_SECURITY_SETTINGS;

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/
static TaskHandle_t g_AppTaskHandle;

static void EventHandlerZwRx(void);
static void EventHandlerZwCommandStatus(void);
static void SerialAPIStateHandler(void);
// Event distributor object
static SEventDistributor g_EventDistributor;

// Event distributor event handler table
static const EventDistributorEventHandler g_aEventHandlerTable[6] =
{
  EventHandlerZwRx,              // EAPPLICATIONEVENT_ZWRX = 0
  EventHandlerZwCommandStatus,   // EAPPLICATIONEVENT_ZWCOMMANDSTATUS = 1
  SerialAPIStateHandler,         // EAPPLICATIONEVENT_STATECHANGE  = 2
  SerialAPIStateHandler,         // EAPPLICATIONEVENT_SERIALDATARX = 3
  SerialAPIStateHandler,         // EAPPLICATIONEVENT_SERIALTIMEOUT = 4
  AppTimerNotificationHandler
};

/**
* Set up default security keys to request when joining a network.
* These are taken from the config_app.h header file.
*/
#ifdef ZW_SECURITY_PROTOCOL
static const uint8_t SecureKeysRequested = REQUESTED_SECURITY_KEYS;
#else  // ifdef ZW_SECURITY_PROTOCOL
static const uint8_t SecureKeysRequested = 0;
#endif // #ifdef ZW_SECURITY_PROTOCOL

static SRadioConfig_t RadioConfig =
{
  .iListenBeforeTalkThreshold = ELISTENBEFORETALKTRESHOLD_DEFAULT,
  .iTxPowerLevelMax = APP_MAX_TX_POWER,
  .iTxPowerLevelAdjust = APP_MEASURED_0DBM_TX_POWER,
  .iTxPowerLevelMaxLR = APP_MAX_TX_POWER_LR,
  .eRegion = ZW_REGION,
  .radio_debug_enable = ENABLE_RADIO_DEBUG
};

static const SProtocolConfig_t ProtocolConfig = {
#ifdef ZW_CONTROLLER_BRIDGE
    .pVirtualSlaveNodeInfoTable = &VirtualSlaveNodeInfoTable,
#else  // ifdef ZW_CONTROLLER_BRIDGE
    .pVirtualSlaveNodeInfoTable = NULL,
#endif // ifdef ZW_CONTROLLER_BRIDGE
    .pSecureKeysRequested = &SecureKeysRequested,
    .pNodeInfo = &AppNodeInfo,
    .pRadioConfig = &RadioConfig};

extern SSyncEvent SetDefaultCB;
extern SSyncEventArg1 LearnModeStatusCb;

/* State vars for ApplicationPoll */
static uint8_t state = 0xff;
static uint8_t retry = 0;

uint8_t compl_workbuf[BUF_SIZE_TX]; /* Used for frames send to remote side. */

/* Queue for frames transmitted to PC - callback, ApplicationCommandHandler, ApplicationControllerUpdate... */
#if !defined(MAX_CALLBACK_QUEUE)
#define MAX_CALLBACK_QUEUE  8
#endif /* !defined(MAX_CALLBACK_QUEUE) */

#if !defined(MAX_UNSOLICITED_QUEUE)
#define MAX_UNSOLICITED_QUEUE 8
#endif /* !defined(MAX_UNSOLICITED_QUEUE) */

typedef struct _callback_element_
{
  uint8_t wCmd;
  uint8_t wLen;
  uint8_t wBuf[BUF_SIZE_TX];
} CALLBACK_ELEMENT;

typedef struct _request_queue_
{
  uint8_t requestOut;
  uint8_t requestIn;
  uint8_t requestCnt;
  CALLBACK_ELEMENT requestQueue[MAX_CALLBACK_QUEUE];
} REQUEST_QUEUE;

REQUEST_QUEUE callbackQueue;

typedef struct _request_unsolicited_queue_
{
  uint8_t requestOut;
  uint8_t requestIn;
  uint8_t requestCnt;
  CALLBACK_ELEMENT requestQueue[MAX_UNSOLICITED_QUEUE];
} REQUEST_UNSOLICITED_QUEUE;

REQUEST_UNSOLICITED_QUEUE commandQueue;

eSerialAPISetupNodeIdBaseType nodeIdBaseType = SERIAL_API_SETUP_NODEID_BASE_TYPE_DEFAULT;

#if SUPPORT_ZW_WATCHDOG_START | SUPPORT_ZW_WATCHDOG_STOP
extern uint8_t bWatchdogStarted;
#endif

/* Last system wakeup reason - is set in ApplicationInit */
EResetReason_t g_eApplResetReason;

#ifdef APPL_PROD_TEST
#define PRODTEST_NR_1 1
#define PRODTEST_NR_2 2
#define PRODTEST_NR_3 3
#define PRODTEST_NR_4 4
#define PRODTEST_NR_5 5
#define PRODTEST_NR_6 6
#define PRODTEST_NR_7 7

#define POWERON_MAGIC_VALUE 0x4242
extern uint8_t bProdtestState;
/* Production test pin definition */
static uint8_t testRun;
#define SET_PRODUCTIONTEST_PIN PIN_IN(P21, 1)
#define IN_PRODUCTIONTEST (!PIN_GET(P21))
#endif /* APPL_PROD_TEST */


/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/
zpal_pm_handle_t radio_power_lock;
zpal_pm_handle_t io_power_lock;
SSwTimer mWakeupTimer;
SApplicationHandles* g_pAppHandles;
bool bTxStatusReportEnabled;

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/
static void ApplicationInitSW(void);
static void ApplicationTask(SApplicationHandles *pAppHandles);

#ifdef ZW_CONTROLLER_BRIDGE
static void ApplicationCommandHandler_Bridge(SReceiveMulti *pReciveMulti);
#else
void ApplicationCommandHandler(void *pSubscriberContext, SZwaveReceivePackage* pRxPackage);
#endif

void ApplicationNodeUpdate(uint8_t bStatus, uint16_t nodeID, uint8_t *pCmd, uint8_t bLen);

#if SUPPORT_ZW_SET_PROMISCUOUS_MODE
static void SetPromiscuousMode(uint8_t mode);
#endif

#if SUPPORT_ZW_REMOVE_FAILED_NODE_ID
extern void ZCB_ComplHandler_ZW_RemoveFailedNodeID(uint8_t bStatus);
#endif

#if SUPPORT_ZW_REPLACE_FAILED_NODE
extern void ZCB_ComplHandler_ZW_ReplaceFailedNode(uint8_t bStatus);
#endif

#if SUPPORT_ZW_SET_SLAVE_LEARN_MODE
extern void ZCB_ComplHandler_ZW_SetSlaveLearnMode(uint8_t bStatus, uint8_t orgID, uint8_t newID);
#endif

#if SUPPORT_ZW_SET_RF_RECEIVE_MODE
extern uint8_t SetRFReceiveMode(uint8_t mode);
#endif

#ifdef UZB

#if 1 // 0 - test UZB on ZDP03A, 1 - normal mode (UZB on UZB :)
#define LEDxPort P0
#define LEDxSHADOW P0Shadow
#define LEDxSHADOWDIR P0ShadowDIR
#define LEDxDIR P0DIR
#define LEDxDIR_PAGE P0DIR_PAGE
#define LEDx 4
#else // 0 - test UZB on ZDP03A, 1 - normal mode (UZB on UZB :)
#define LEDxPort P0
#define LEDxSHADOW P0Shadow
#define LEDxSHADOWDIR P0ShadowDIR
#define LEDxDIR P0DIR
#define LEDxDIR_PAGE P0DIR_PAGE
#define LEDx 7
#endif // 0 - test UZB on ZDP03A, 1 - normal mode (UZB on UZB :)

void /*RET  Nothing                  */
set_state_and_notify(uint8_t st)
{

  if (state != st)
  {
    xTaskNotify(g_AppTaskHandle,
                1<<EAPPLICATIONEVENT_STATECHANGE,
                eSetBits);
    if (st == stateIdle)
    {
      PIN_HIGH(LEDx);
    }
    else if (state == stateIdle)
    {
      PIN_LOW(LEDx);
    }
    state = st;
  }
}

#else // UZB

void set_state_and_notify(uint8_t st)
{
  if (state != st)
  {
    xTaskNotify(g_AppTaskHandle,
                1<<EAPPLICATIONEVENT_STATECHANGE,
                eSetBits );
    state = st;
  }
}


#endif // UZB

void set_state(uint8_t st)
{
  state = st;
}

/*===============================   Request   ================================
**    Queues request (callback) to be transmitted to remote side
**
**--------------------------------------------------------------------------*/
bool /*RET  queue status (false queue full)*/
Request(
    uint8_t cmd,       /*IN   Command                  */
    uint8_t *pData, /*IN   pointer to data          */
    uint8_t len        /*IN   Length of data           */
)
{
#if SUPPORT_SERIAL_API_READY
  /* Only queue Request frame for HOST if SERIAL LINK has been established or we need to send the WakeUp Frame */
  if (((SERIAL_LINK_DETACHED != serialLinkState) || (wakeUpOnRF)) && (callbackQueue.requestCnt < MAX_CALLBACK_QUEUE))
#else
  if (callbackQueue.requestCnt < MAX_CALLBACK_QUEUE)
#endif
  {
    callbackQueue.requestCnt++;
    callbackQueue.requestQueue[callbackQueue.requestIn].wCmd = cmd;
    if (len > (uint8_t)BUF_SIZE_TX)
    {
      len = (uint8_t)BUF_SIZE_TX;
    }
    callbackQueue.requestQueue[callbackQueue.requestIn].wLen = len;
    memcpy(&callbackQueue.requestQueue[callbackQueue.requestIn].wBuf[0], pData, len);
    if (++callbackQueue.requestIn >= MAX_CALLBACK_QUEUE)
    {
      callbackQueue.requestIn = 0;
    }
    xTaskNotify(g_AppTaskHandle,
                1<<EAPPLICATIONEVENT_STATECHANGE,
                eSetBits);

    return true;
  }
  return false;
}

/*=========================   RequestUnsolicited   ===========================
**    Queues request (command) to be transmitted to remote side
**
**--------------------------------------------------------------------------*/
bool /*RET  queue status (false queue full)*/
RequestUnsolicited(
    uint8_t cmd,       /*IN   Command                  */
    uint8_t *pData, /*IN   pointer to data          */
    uint8_t len        /*IN   Length of data           */
)
{
  taskENTER_CRITICAL();
#if SUPPORT_SERIAL_API_READY
  /* Only queue Request frame for HOST if SERIAL LINK has been established or we need to send the WakeUp Frame */
  if (((SERIAL_LINK_DETACHED != serialLinkState) || (wakeUpOnRF)) && (commandQueue.requestCnt < MAX_UNSOLICITED_QUEUE))
#else
  if (commandQueue.requestCnt < MAX_UNSOLICITED_QUEUE)
#endif
  {
    commandQueue.requestCnt++;
    commandQueue.requestQueue[commandQueue.requestIn].wCmd = cmd;
    if (len > (uint8_t)BUF_SIZE_TX)
    {
      len = (uint8_t)BUF_SIZE_TX;
    }
    commandQueue.requestQueue[commandQueue.requestIn].wLen = len;
    memcpy(&commandQueue.requestQueue[commandQueue.requestIn].wBuf[0], pData, len);
    if (++commandQueue.requestIn >= MAX_UNSOLICITED_QUEUE)
    {
      commandQueue.requestIn = 0;
    }
    taskEXIT_CRITICAL();
    xTaskNotify(g_AppTaskHandle,
                1<<EAPPLICATIONEVENT_STATECHANGE,
                eSetBits);
    return true;
  }
  taskEXIT_CRITICAL();
  return false;
}

void PurgeCallbackQueue(void)
{
  callbackQueue.requestOut = callbackQueue.requestIn = callbackQueue.requestCnt = 0;
}

void PurgeCommandQueue(void)
{
  taskENTER_CRITICAL();
  commandQueue.requestOut = commandQueue.requestIn = commandQueue.requestCnt = 0;
  taskEXIT_CRITICAL();
}

/*===============================   Respond   ===============================
**    Send immediate respons to remote side
**
**    Side effects: Sets state variable to stateTxSerial (wait for ack)
**
**--------------------------------------------------------------------------*/
void /*RET  Nothing                 */
Respond(
    uint8_t cmd,             /*IN   Command                  */
    uint8_t const *pData, /*IN   pointer to data          */
    uint8_t len              /*IN   Length of data           */
)
{
  /* If there are no data; pData == NULL and len == 0 we must set the data pointer */
  /* to some dummy data. comm_interface_transmit_frame interprets NULL pointer as retransmit indication */
  if (len == 0)
  {
    pData = (uint8_t *)0x7ff; /* Just something is not used anyway */
  }
  comm_interface_transmit_frame(cmd, RESPONSE, pData, len, NULL);

  set_state_and_notify(stateTxSerial); /* We want ACK/NAK...*/
}

void
DoRespond(uint8_t retVal)
{
  Respond(serial_frame->cmd, &retVal, 1);
}

void
DoRespond_workbuf(
  uint8_t cnt
)
{
  Respond(serial_frame->cmd, compl_workbuf, cnt);
}

static void EventHandlerZwRx(void)
{
  QueueHandle_t Queue = g_pAppHandles->ZwRxQueue;
  SZwaveReceivePackage RxPackage;

  // Handle incoming replies
  while (xQueueReceive(Queue, (uint8_t *)(&RxPackage), 0) == pdTRUE)
  {
    DPRINTF("Incoming Rx %x \r\n", RxPackage.eReceiveType);

    switch (RxPackage.eReceiveType)
    {
      case EZWAVERECEIVETYPE_SINGLE:
      {
#ifndef ZW_CONTROLLER_BRIDGE
        ApplicationCommandHandler(NULL, &RxPackage);
#endif
        break;
      }

#ifdef ZW_CONTROLLER_BRIDGE
      case EZWAVERECEIVETYPE_MULTI:
      {
        ApplicationCommandHandler_Bridge(&RxPackage.uReceiveParams.RxMulti);
        break;
      }
#endif // #ifdef ZW_CONTROLLER_BRIDGE

      case EZWAVERECEIVETYPE_NODE_UPDATE:
      {
        ApplicationNodeUpdate(
            RxPackage.uReceiveParams.RxNodeUpdate.Status,
            RxPackage.uReceiveParams.RxNodeUpdate.NodeId,
            RxPackage.uReceiveParams.RxNodeUpdate.aPayload,
            RxPackage.uReceiveParams.RxNodeUpdate.iLength);
        break;
      }

#ifdef ZW_SLAVE
      case EZWAVERECEIVETYPE_SECURITY_EVENT:
      {
        /*ApplicationSecurityEvent() was used to support CSA, not needed in SDK7*/
        break;
      }
#endif

      case EZWAVERECEIVETYPE_STAY_AWAKE:
      {
        /*
         * Non-application frame was received, that must keep device awake.
         */
        break;
      }

      default:
      {
        DPRINTF("Invalid Receive Type: %d", RxPackage.eReceiveType);
        ASSERT(false);
        break;
      }
    }
  }
}

/**
* @brief Triggered when protocol puts a message on the ZwCommandStatusQueue.
*/
static void EventHandlerZwCommandStatus(void)
{
  QueueHandle_t Queue = g_pAppHandles->ZwCommandStatusQueue;
  SZwaveCommandStatusPackage Status;

  // Handle incoming replies
  while (xQueueReceive(Queue, (uint8_t *)(&Status), 0) == pdTRUE)
  {
    DPRINTF("Incoming Status msg %x\r\n", Status.eStatusType);

    switch (Status.eStatusType)
    {
    case EZWAVECOMMANDSTATUS_TX:
    {
      SZWaveTransmitStatus *pTxStatus = &Status.Content.TxStatus;
      if (!pTxStatus->bIsTxFrameLegal)
      {
        DPRINT("Auch - not sure what to do\r\n");
      }
      else
      {
        DPRINT("Tx Status received\r\n");
        if (pTxStatus->Handle)
        {
          ZW_TX_Callback_t pCallback = (ZW_TX_Callback_t)pTxStatus->Handle;
          pCallback(pTxStatus->TxStatus, &pTxStatus->ExtendedTxStatus);
        }
      }

      break;
    }

    case EZWAVECOMMANDSTATUS_LEARN_MODE_STATUS:
    {
      SyncEventArg1Invoke(&LearnModeStatusCb, Status.Content.LearnModeStatus.Status);
      break;
    }

    case EZWAVECOMMANDSTATUS_SET_DEFAULT:
      // Received when protocol is started (not implemented yet), and when SetDefault command is completed
      SyncEventInvoke(&SetDefaultCB);
      break;

#ifdef ZW_CONTROLLER
    case EZWAVECOMMANDSTATUS_REPLACE_FAILED_NODE_ID:
    {
      ZCB_ComplHandler_ZW_ReplaceFailedNode(Status.Content.FailedNodeIDStatus.result);
      break;
    }
    case EZWAVECOMMANDSTATUS_REMOVE_FAILED_NODE_ID:
    {
      ZCB_ComplHandler_ZW_RemoveFailedNodeID(Status.Content.FailedNodeIDStatus.result);
      break;
    }
    case EZWAVECOMMANDSTATUS_NETWORK_MANAGEMENT:
    {
      LEARN_INFO_T mLearnInfo;
      mLearnInfo.bStatus = Status.Content.NetworkManagementStatus.statusInfo[0];
      mLearnInfo.bSource = (uint16_t)(((uint16_t)Status.Content.NetworkManagementStatus.statusInfo[1] << 8) | // nodeID MSB
                                      (uint16_t)Status.Content.NetworkManagementStatus.statusInfo[2]);         // nodeID LSB
      mLearnInfo.bLen = Status.Content.NetworkManagementStatus.statusInfo[3];
      mLearnInfo.pCmd = &Status.Content.NetworkManagementStatus.statusInfo[4];
      ZCB_ComplHandler_ZW_NodeManagement(&mLearnInfo);
      break;
    }
#if SUPPORT_ZW_SET_SLAVE_LEARN_MODE
    case EZWAVECOMMANDSTATUS_SET_SLAVE_LEARN_MODE:
    {
      uint8_t bStatus;
      uint16_t orgID;
      uint16_t newID;
      bStatus = Status.Content.NetworkManagementStatus.statusInfo[0];
      orgID = (uint16_t)((uint16_t)(Status.Content.NetworkManagementStatus.statusInfo[1] << 8) |  // org nodeID MSB
              Status.Content.NetworkManagementStatus.statusInfo[2]);                               // org nodeID LSB
      newID = (uint16_t)((uint16_t)(Status.Content.NetworkManagementStatus.statusInfo[3] << 8) |   // new nodeID MSB
              Status.Content.NetworkManagementStatus.statusInfo[4]);                               // new nodeID LSB
      ZCB_ComplHandler_ZW_SetSlaveLearnMode(bStatus, (uint8_t)orgID, (uint8_t)newID);              // orgID and newID are always (8-bit) IDs
      break;
    }
#endif
#endif
    case EZWAVECOMMANDSTATUS_INVALID_TX_REQUEST:
    {
      DPRINTF("Invalid TX Request to protocol - %d\r\n", Status.Content.InvalidTxRequestStatus.InvalidTxRequest);
      break;
    }
    case EZWAVECOMMANDSTATUS_INVALID_COMMAND:
    {
      DPRINTF("Invalid command to protocol - %d\r\n", Status.Content.InvalidCommandStatus.InvalidCommand);
      break;
    }
    case EZWAVECOMMANDSTATUS_PM_SET_POWERDOWN_CALLBACK:
    {
      // Status response from calling the ZAF_PM_SetPowerDownCallback function
      if(false == Status.Content.SetPowerDownCallbackStatus.result)
      {
        DPRINT("Failed to register PowerDown Callback function\r\n");
        ASSERT(false);
      }
      break;
    }

    default:
      DPRINTF("Unknown / Unexpected  CMD - %d\r\n", Status.eStatusType);
      ASSERT(false);
      break;
    }
  }
}

static void 
appFileSystemInit(void)
{
  /* 
   * Handle file system init inside Application Task
   * This reduces the default stack needed during initialization
   */
  if (SerialApiFileInit())
  {
    ReadApplicationSettings(&AppNodeInfo.DeviceOptionsMask, &AppNodeInfo.NodeType.generic, &AppNodeInfo.NodeType.specific);
    ReadApplicationCCInfo(&AppNodeInfo.CommandClasses.UnSecureIncludedCC.iListLength,
                          (uint8_t*)AppNodeInfo.CommandClasses.UnSecureIncludedCC.pCommandClasses,
                          &AppNodeInfo.CommandClasses.SecureIncludedUnSecureCC.iListLength,
                          (uint8_t*)AppNodeInfo.CommandClasses.SecureIncludedUnSecureCC.pCommandClasses,
                          &AppNodeInfo.CommandClasses.SecureIncludedSecureCC.iListLength,
                          (uint8_t*)AppNodeInfo.CommandClasses.SecureIncludedSecureCC.pCommandClasses);
    ReadApplicationRfRegion(&RadioConfig.eRegion);
    ReadApplicationTxPowerlevel(&RadioConfig.iTxPowerLevelMax, &RadioConfig.iTxPowerLevelAdjust);
    ReadApplicationMaxLRTxPwr(&RadioConfig.iTxPowerLevelMaxLR);
    ReadApplicationEnablePTI(&RadioConfig.radio_debug_enable);
  }
  else
  {
    /*
     * We end up here on the first boot after initializing the flash file system
     */
 
    zpal_radio_region_t mfgRegionConfig;
    // Check for a valid RF Region value in the manufacturing user data configuration
    ZW_GetMfgTokenDataCountryFreq(&mfgRegionConfig);
    if ( (mfgRegionConfig <= REGION_US_LR) || (mfgRegionConfig == REGION_JP) || (mfgRegionConfig == REGION_KR) )
    {
      // Valid RF Region configuration found. Use instead of the app default setting
      RadioConfig.eRegion = mfgRegionConfig;
    }

    // Save the setting to flash
    SaveApplicationRfRegion(RadioConfig.eRegion);
    // Save the default Tx powerlevel
    SaveApplicationTxPowerlevel(RadioConfig.iTxPowerLevelMax, RadioConfig.iTxPowerLevelAdjust);
    // write defualt values
    SaveApplicationSettings(AppNodeInfo.DeviceOptionsMask, AppNodeInfo.NodeType.generic, AppNodeInfo.NodeType.specific);
    // change the 20dBm tx power setting according to the application configuration
    SaveApplicationMaxLRTxPwr(RadioConfig.iTxPowerLevelMaxLR);

    SaveApplicationEnablePTI(RadioConfig.radio_debug_enable);
  }  
}
/*
 * The below function must be implemented as hardware specific function in a separate source
 * file if required.
 */
ZW_WEAK void SerialAPI_hw_psu_init(void)
{
  // Do nothing
}

/*===============================   ApplicationPoll   =======================
**    Application poll function, handling the receiving and transmitting
**    communication with the PC.
**
**--------------------------------------------------------------------------*/
static void           /*RET  Nothing                  */
ApplicationTask(SApplicationHandles* pAppHandles)
{

  SerialAPI_hw_psu_init(); // Must be invoked after the file system is initialized.
  
  // Init
  g_AppTaskHandle = xTaskGetCurrentTaskHandle();

  g_pAppHandles = pAppHandles;

  ZAF_setApplicationData(g_AppTaskHandle, pAppHandles,  &ProtocolConfig);
  AppTimerInit(EAPPLICATIONEVENT_TIMER, (void *) g_AppTaskHandle);
  radio_power_lock = zpal_pm_register(ZPAL_PM_TYPE_USE_RADIO);
  zpal_pm_stay_awake(radio_power_lock, 0);
  io_power_lock = zpal_pm_register(ZPAL_PM_TYPE_DEEP_SLEEP);
  zpal_pm_stay_awake(io_power_lock, 0);

  EventDistributorConfig(
      &g_EventDistributor,
      sizeof_array(g_aEventHandlerTable),
      g_aEventHandlerTable,
      SerialAPIStateHandler);

  set_state_and_notify(stateStartup);
  // Wait for and process events
  DPRINT("SerialApi Event processor Started\r\n");
  uint32_t iMaxTaskSleep = 0xFFFFFFFF;//10;
  for (;;)
  {
    EventDistributorDistribute(&g_EventDistributor, iMaxTaskSleep, 0);
  }
}

static void SerialAPICommandHandler(void)
{
  const bool handler_invoked = invoke_cmd_handler(serial_frame);
  if (!handler_invoked)
  {
    /* TODO - send a "Not Supported" respond frame */
    /* UNKNOWN - just drop it */
    set_state_and_notify(stateIdle);
  }
}


static void SerialAPIStateHandler(void)
{
  comm_interface_parse_result_t conVal;

  /* ApplicationPoll is controlled by a statemachine with the four states:
      stateIdle, stateFrameParse, stateTxSerial, stateCbTxSerial.

      stateIdle: If there is anything to transmit do so. -> stateCbTxSerial
                 If not, check if anything is received. -> stateFrameParse
                 If neither, stay in the state
                 Note: frames received while we are transmitting are lost
                 and must be retransmitted by PC

      stateFrameParse: Parse received frame.
                 If the request has no response -> stateIdle
                 If there is an immediate response send it. -> stateTxSerial

      stateTxSerial:  Waits for ack on responses send in stateFrameParse.
                 Retransmit frame as needed.
                 -> stateIdle

      stateCbTxSerial:  Waits for ack on requests send in stateIdle
                  (callback, ApplicationCommandHandler etc).
                 Retransmit frame as needed and remove from callbackqueue when done.
                 -> stateIdle

	  stateAppSuspend: Added for the uzb suspend function. The resume is through the suspend signal goes high in UZB stick
	                   The wakeup from deep sleep suspend causes system reboot

  */

  {
    switch (state)
    {
      case stateStartup:
        {
          ApplicationInitSW();
          SetRFReceiveMode(1);
          set_state_and_notify(stateIdle);
        }
        break;

      case stateIdle:
        {
#if SUPPORT_SERIAL_API_READY
          /* Only empty callback queue for HOST if SERIAL LINK has been established  */
          if (callbackQueue.requestCnt && (SERIAL_LINK_DETACHED != serialLinkState))
#else
          /* Check if there is anything to transmit. If so do it */
          if (callbackQueue.requestCnt)
#endif
          {
            comm_interface_transmit_frame(
              callbackQueue.requestQueue[callbackQueue.requestOut].wCmd,
              REQUEST,
              (uint8_t *)callbackQueue.requestQueue[callbackQueue.requestOut].wBuf,
              callbackQueue.requestQueue[callbackQueue.requestOut].wLen,
              NULL
            );
            set_state_and_notify(stateCallbackTxSerial);
            /* callbackCnt decremented when frame is acknowledged from PC - or timed out after retries */
          }
          else
          {
#if SUPPORT_SERIAL_API_READY
            /* Only empty command queue for HOST if SERIAL LINK has been established  */
            if (commandQueue.requestCnt && (SERIAL_LINK_DETACHED != serialLinkState))
#else
            /* Check if there is anything to transmit. If so do it */
            if (commandQueue.requestCnt)
#endif
            {
              comm_interface_transmit_frame(
                commandQueue.requestQueue[commandQueue.requestOut].wCmd,
                REQUEST,
                (uint8_t *)commandQueue.requestQueue[commandQueue.requestOut].wBuf,
                commandQueue.requestQueue[commandQueue.requestOut].wLen,
                NULL
              );
              set_state_and_notify(stateCommandTxSerial);
              /* commandCnt decremented when frame is acknowledged from PC - or timed out after retries */
            }
            else
            {
              /* Nothing to transmit. Check if we received anything */
              if (comm_interface_parse_data(true) == PARSE_FRAME_RECEIVED)
              {
#if SUPPORT_SERIAL_API_READY
                /* We have received a frame from HOST so we must assume we are connected */
                serialLinkState = SERIAL_LINK_CONNECTED;
#endif
                /* We got a frame... */
                set_state_and_notify(stateFrameParse);
              }
            }
          }
        }
        break;

    case stateFrameParse:
      {
        SerialAPICommandHandler();
      }
      break;

    case stateTxSerial:
    {
      /* Wait for ACK on send respond. Retransmit as needed */
      if ((conVal = comm_interface_parse_data(false)) == PARSE_FRAME_SENT)
      {
        /* One more RES transmitted succesfully */
        retry = 0;
        set_state_and_notify(stateIdle);
      }
      else if (conVal == PARSE_TX_TIMEOUT)
      {
        /* Either a NAK has been received or we timed out waiting for ACK */
        if (retry++ < MAX_SERIAL_RETRY)
        {
          comm_interface_transmit_frame(0, REQUEST, NULL, 0, NULL); /* Retry... */
        }
        else
        {
          /* Drop RES as HOST could not be reached */
          retry = 0;
          set_state_and_notify(stateIdle);
        }
      }
      /* All other states are ignored, as for now the only thing we are looking for is ACK/NAK! */
    }
    break;

    case stateCallbackTxSerial:
    {
      /* Wait for ack on unsolicited event (callback etc.) */
      /* Retransmit as needed. Remove frame from callbackQueue when done */
      if ((conVal = comm_interface_parse_data(false)) == PARSE_FRAME_SENT)
      {
        /* One more REQ transmitted succesfully */
        PopCallBackQueue();
      }
      else if (conVal == PARSE_TX_TIMEOUT)
      {
        /* Either a NAK has been received or we timed out waiting for ACK */
        if (retry++ < MAX_SERIAL_RETRY)
        {
          comm_interface_transmit_frame(0, REQUEST, NULL, 0, NULL); /* Retry... */
        }
        else
        {
          /* Drop REQ as HOST could not be reached */
          PopCallBackQueue();
        }
      }
      /* All other states are ignored, as for now the only thing we are looking for is ACK/NAK! */
    }
    break;

    case stateCommandTxSerial:
    {
      /* Wait for ack on unsolicited ApplicationCommandHandler event */
      /* Retransmit as needed. Remove frame from comamndQueue when done */
      if ((conVal = comm_interface_parse_data(false)) == PARSE_FRAME_SENT)
      {
        /* One more REQ transmitted succesfully */
        PopCommandQueue();
      }
      else if (conVal == PARSE_TX_TIMEOUT)
      {
        /* Either a NAK has been received or we timed out waiting for ACK */
        if (retry++ < MAX_SERIAL_RETRY)
        {
          comm_interface_transmit_frame(0, REQUEST, NULL, 0, NULL); /* Retry... */
        }
        else
        {
          /* Drop REQ as HOST could not be reached */
          PopCommandQueue();
        }
      }
      /* All other states are ignored, as for now the only thing we are looking for is ACK/NAK! */
    }
    break;
 #ifdef USB_SUSPEND_SUPPORT
	case stateAppSuspend:
		SerialAPI_hw_usb_suspend_handler();
	break;
#endif
    default:
      set_state_and_notify(stateIdle);
      break;
    }

  } // For loop - task loop
}


void
PopCallBackQueue(void)
{
  if (callbackQueue.requestCnt)
  {
    callbackQueue.requestCnt--;
    if (++callbackQueue.requestOut >= MAX_CALLBACK_QUEUE)
    {
      callbackQueue.requestOut = 0;
    }
  }
  else
  {
    callbackQueue.requestOut = callbackQueue.requestIn;
  }
  retry = 0;
  set_state_and_notify(stateIdle);
}


void
PopCommandQueue(void)
{
  if (commandQueue.requestCnt)
  {
    commandQueue.requestCnt--;
    if (++commandQueue.requestOut >= MAX_UNSOLICITED_QUEUE)
    {
      commandQueue.requestOut = 0;
    }
  }
  else
  {
    commandQueue.requestOut = commandQueue.requestIn;
  }
  retry = 0;
  set_state_and_notify(stateIdle);
}

/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * @brief wakeup after sleep timeout event
 *
 * @param pTimer Timer connected to this method
 */
void
ZCB_WakeupTimeout(SSwTimer *pTimer)
{
  UNUSED(pTimer);
  DPRINT("ZCB_WakeupTimeout\n");
}

/*==============================   ApplicationInitSW   ======================
**    Initialization of the Application Software
**
**--------------------------------------------------------------------------*/
void
ApplicationInitSW(void)
{
  comm_interface_init();

  // FIXME load any saved node configuration and prepare to feed it to protocol
/* Do we together with the bTxStatus uint8_t also transmit a sTxStatusReport struct on ZW_SendData callback to HOST */
#if SUPPORT_SEND_DATA_TIMING
  bTxStatusReportEnabled = true;
#else
  bTxStatusReportEnabled = false;
#endif

#if SUPPORT_SERIAL_API_STARTUP_NOTIFICATION
  /* ZW->HOST: bWakeupReason | bWatchdogStarted | deviceOptionMask | */
  /*           nodeType_generic | nodeType_specific | cmdClassLength | cmdClass[] */

  // FIXME send startup notification via serial port if we are supposed to
  SCommandClassList_t *const apCCLists[3] =
  {
    &AppNodeInfo.CommandClasses.UnSecureIncludedCC,
    &AppNodeInfo.CommandClasses.SecureIncludedUnSecureCC,
    &AppNodeInfo.CommandClasses.SecureIncludedSecureCC
  };

  compl_workbuf[0] = g_eApplResetReason;
#if SUPPORT_ZW_WATCHDOG_START || SUPPORT_ZW_WATCHDOG_STOP
  compl_workbuf[1] = bWatchdogStarted;
#else
  compl_workbuf[1] = false;
#endif
  compl_workbuf[2] = AppNodeInfo.DeviceOptionsMask;
  compl_workbuf[3] = AppNodeInfo.NodeType.generic;
  compl_workbuf[4] = AppNodeInfo.NodeType.specific;
  compl_workbuf[5] = apCCLists[0]->iListLength;
  uint8_t i = 0;
  if (0 < apCCLists[0]->iListLength)
  {
    for (i = 0; i < apCCLists[0]->iListLength; i++)
    {
      compl_workbuf[6 + i] = apCCLists[0]->pCommandClasses[i];
    }
  }
  eSerialAPIStartedCapabilities capabilities = (RadioConfig.eRegion == REGION_US_LR) ? SERIAL_API_STARTED_CAPABILITIES_L0NG_RANGE : 0;
  compl_workbuf[6 + i] = capabilities;

  Request(FUNC_ID_SERIAL_API_STARTED, compl_workbuf, 7 + i);

#endif /* #if SUPPORT_STARTUP_NOTIFICATION */
   AppTimerDeepSleepPersistentRegister(&mWakeupTimer, false, ZCB_WakeupTimeout);  // register for event jobs timeout event
}

#ifdef USB_SUSPEND_SUPPORT
void UsbSuspendCallback(void)
{
  set_state(stateAppSuspend);
}
#endif

/*==============================   ApplicationInit   ======================
**    Init UART and setup port pins for LEDs
**
**--------------------------------------------------------------------------*/
ZW_APPLICATION_STATUS
ApplicationInit(
  EResetReason_t eResetReason)
{
  SerialAPI_hw_init();

#ifdef USB_SUSPEND_SUPPORT
  SerialAPI_set_usb_supend_callback(UsbSuspendCallback);
#endif

#if SUPPORT_SERIAL_API_READY
	if (ERESETREASON_SLEEP == eResetReason)
	{
	  /* We have been waken from sleep by timer or external pin event - we must assume we are connected. */
	  serialLinkState = SERIAL_LINK_CONNECTED;
	}
	else
	{
		/* We have been waken either by ERESETREASON_POWER_ON or  ERESETREASON_PIN or similar. Initially we are DETACHED */
	  serialLinkState = SERIAL_LINK_CONNECTED;
	}
#endif
  /* g_eApplResetReason now contains lastest System Reset reason */
  g_eApplResetReason = eResetReason;

#ifdef DEBUGPRINT
  static uint8_t m_aDebugPrintBuffer[96];
  zpal_debug_init();
  DebugPrintConfig(m_aDebugPrintBuffer, sizeof(m_aDebugPrintBuffer), zpal_debug_output);
#endif

  DPRINT("\n\n--------------------------------\n");
  DPRINT("Z-Wave Sample App: Serial API ");
#if defined(ZW_SLAVE)
  DPRINT("Slave\n");
#else /* defined(ZW_SLAVE) */
  DPRINT("Controller\n");
#endif /* defined(ZW_SLAVE) */
  DPRINTF("SDK: %d.%d.%d ZAF: %d.%d.%d.%d\n",
          SDK_VERSION_MAJOR,
          SDK_VERSION_MINOR,
          SDK_VERSION_PATCH,
          zpal_get_app_version_major(),
          zpal_get_app_version_minor(),
          zpal_get_app_version_patch(),
          ZAF_BUILD_NO);

  DPRINTF("ApplicationInit eResetReason = %d\n", eResetReason);

  appFileSystemInit();
  /*************************************************************************************
   * CREATE USER TASKS  -  ZW_ApplicationRegisterTask() and ZW_UserTask_CreateTask()
   *************************************************************************************
   * Register the main APP task function.
   *
   * ATTENTION: This function is the only task that can call ZAF aPI functions!!!
   * Failure to follow guidelines will result in undefined behavior.
   *
   * This function further is the only way to register Event Notification Bit Numbers
   * for associating to given event handlers.
   *
   * ZW_UserTask_CreateTask() can be used to create additional tasks.
   * @see SensorPIR_MultiThread example for more info.
   *************************************************************************************/
  bool bWasTaskCreated = ZW_ApplicationRegisterTask(
                                                    ApplicationTask,
                                                    EAPPLICATIONEVENT_ZWRX,
                                                    EAPPLICATIONEVENT_ZWCOMMANDSTATUS,
                                                    &ProtocolConfig
                                                    );
  ASSERT(bWasTaskCreated);

  return (APPLICATION_RUNNING); /*Return false to enter production test mode*/
}


#ifndef ZW_CONTROLLER_BRIDGE
/*==========================   ApplicationCommandHandler   ==================
**    Handling of received application commands and requests
**
**--------------------------------------------------------------------------*/
void /*RET Nothing                  */
ApplicationCommandHandler(void *pSubscriberContext, SZwaveReceivePackage* pRxPackage)
{
  UNUSED(pSubscriberContext);
  ZW_APPLICATION_TX_BUFFER *pCmd = (ZW_APPLICATION_TX_BUFFER *)&pRxPackage->uReceiveParams.Rx.Payload;
  uint8_t cmdLength = pRxPackage->uReceiveParams.Rx.iLength;
  RECEIVE_OPTIONS_TYPE *rxOpt = &pRxPackage->uReceiveParams.Rx.RxOptions;
  /* ZW->PC: REQ | 0x04 | rxStatus | sourceNode | cmdLength | pCmd[] | rssiVal | securityKey */
  uint8_t offset = 0;
  BYTE_IN_AR(compl_workbuf, 0) = rxOpt->rxStatus;
  if (SERIAL_API_SETUP_NODEID_BASE_TYPE_16_BIT == nodeIdBaseType)
  {
    BYTE_IN_AR(compl_workbuf, 1) = (uint8_t)(rxOpt->sourceNode >> 8);     // MSB
    BYTE_IN_AR(compl_workbuf, 2) = (uint8_t)(rxOpt->sourceNode & 0xFF);   // LSB
    offset++;  // 16 bit nodeID means the command fields that follow are offset by one byte
  }
  else
  {
    BYTE_IN_AR(compl_workbuf, 1) = (uint8_t)(rxOpt->sourceNode & 0xFF);       // Legacy 8 bit nodeID
  }
#if defined(ZW_CONTROLLER) && !defined(ZW_CONTROLLER_STATIC)
  if (cmdLength > (uint8_t)(BUF_SIZE_TX - (offset + (rxOpt->rxStatus & RECEIVE_STATUS_FOREIGN_FRAME ? 5 : 4))))
  {
    cmdLength = (uint8_t)(BUF_SIZE_TX - (offset + (rxOpt->rxStatus & RECEIVE_STATUS_FOREIGN_FRAME ? 5 : 4)));
  }
#else
  if (cmdLength > (uint8_t)(BUF_SIZE_TX - (offset + 5)))
  {
    cmdLength = (uint8_t)(BUF_SIZE_TX - (offset + 5));
  }
#endif
  BYTE_IN_AR(compl_workbuf, offset + 2) = cmdLength;
  for (uint8_t i = 0; i < cmdLength; i++)
  {
    BYTE_IN_AR(compl_workbuf, offset + 3 + i) = *((uint8_t*)pCmd + i);
  }
  /* Syntax when a promiscuous frame is received (i.e. RECEIVE_STATUS_FOREIGN_FRAME is set): */
  /* ZW->PC: REQ | 0xD1 | rxStatus | sourceNode | cmdLength | pCmd[] | destNode | rssiVal
   * | securityKey | bSourceTxPower | bSourceNoiseFloor */
#if defined(ZW_CONTROLLER) && !defined(ZW_CONTROLLER_STATIC)
  /* For libraries supporting promiscuous mode... */
  BYTE_IN_AR(compl_workbuf, offset + 3 + cmdLength) = (uint8_t)(rxOpt->destNode & 0xFF);
  uint8_t index = (uint8_t)(offset + 3 + ((rxOpt->rxStatus & RECEIVE_STATUS_FOREIGN_FRAME) ? 1 : 0) + cmdLength);
  BYTE_IN_AR(compl_workbuf, index++) = (uint8_t)rxOpt->rxRSSIVal;
  BYTE_IN_AR(compl_workbuf, index++) = rxOpt->securityKey;
  BYTE_IN_AR(compl_workbuf, index++) = (uint8_t)rxOpt->bSourceTxPower;
  BYTE_IN_AR(compl_workbuf, index) = (uint8_t)rxOpt->bSourceNoiseFloor;
  RequestUnsolicited((rxOpt->rxStatus & RECEIVE_STATUS_FOREIGN_FRAME ?
                        FUNC_ID_PROMISCUOUS_APPLICATION_COMMAND_HANDLER : FUNC_ID_APPLICATION_COMMAND_HANDLER),
                     compl_workbuf,
                     index);
#else
  BYTE_IN_AR(compl_workbuf, offset + 3 + cmdLength) = (uint8_t)rxOpt->rxRSSIVal;
  BYTE_IN_AR(compl_workbuf, offset + 4 + cmdLength) = rxOpt->securityKey;
  BYTE_IN_AR(compl_workbuf, offset + 5 + cmdLength) = (uint8_t)rxOpt->bSourceTxPower;
  BYTE_IN_AR(compl_workbuf, offset + 6 + cmdLength) = (uint8_t)rxOpt->bSourceNoiseFloor;

  /* Less code space-consuming version for libraries without promiscuous support */
  RequestUnsolicited(FUNC_ID_APPLICATION_COMMAND_HANDLER, compl_workbuf, (uint8_t)(offset + 7 + cmdLength));
#endif
}
#endif

#ifdef ZW_CONTROLLER_BRIDGE

// Struct describing Multicast nodemask header for SerialAPI
typedef struct SMultiCastNodeMaskHeaderSerial
{
  uint8_t iNodemaskLength : 5; // Bits 0-4 is length. Length of Nodemask in bytes - Valid values [0-29]
  uint8_t iNodeMaskOffset : 3; // Bits 5-7 is offset. Denotes which node the first bit in the nodemask describes
                               // First node in nodemask is (Value * 32) + 1 - e.g. 2 -> first node is 65
                               // In reality - we always give a full nodemask -> length 29, offset 0.
} SMultiCastNodeMaskHeaderSerial;

/*======================   ApplicationCommandHandler_Bridge   ================
**    Handling of received application commands and requests
**
**--------------------------------------------------------------------------*/
static void                       /*RET Nothing                  */
ApplicationCommandHandler_Bridge(SReceiveMulti* pReceiveMulti)
{
#if SUPPORT_APPLICATION_COMMAND_HANDLER_BRIDGE
  /* ZW->HOST: REQ | 0xA8 | rxStatus | destNode | sourceNode | cmdLength
   *          | pCmd[] | multiDestsOffset_NodeMaskLen | multiDestsNodeMask[] | rssiVal
   *          | securityKey | bSourceTxPower | bSourceNoiseFloor */
  uint8_t offset = 0;
  BYTE_IN_AR(compl_workbuf, 0) = pReceiveMulti->RxOptions.rxStatus;
  if (SERIAL_API_SETUP_NODEID_BASE_TYPE_16_BIT == nodeIdBaseType)
  {
    BYTE_IN_AR(compl_workbuf, 1) = (uint8_t)(pReceiveMulti->RxOptions.destNode >> 8);      // MSB
    BYTE_IN_AR(compl_workbuf, 2) = (uint8_t)(pReceiveMulti->RxOptions.destNode & 0xFF);    // LSB
    BYTE_IN_AR(compl_workbuf, 3) = (uint8_t)(pReceiveMulti->RxOptions.sourceNode >> 8);    // MSB
    BYTE_IN_AR(compl_workbuf, 4) = (uint8_t)(pReceiveMulti->RxOptions.sourceNode & 0xFF);  // LSB
    offset = 6;  // 16 bit nodeIDs means the command fields that follow are offset by two bytes
  }
  else
  {
    // Legacy 8 bit nodeIDs
    BYTE_IN_AR(compl_workbuf, 1) = (uint8_t)pReceiveMulti->RxOptions.destNode;
    BYTE_IN_AR(compl_workbuf, 2) = (uint8_t)pReceiveMulti->RxOptions.sourceNode;
    offset = 4;
  }

  uint32_t cmdLength = pReceiveMulti->iCommandLength;
  uint8_t i;

  if (cmdLength > sizeof(pReceiveMulti->Payload))
  {
    cmdLength = sizeof(pReceiveMulti->Payload);
  }
  if (cmdLength > (uint8_t)(BUF_SIZE_TX - offset) )
  {
    cmdLength = (uint8_t)(BUF_SIZE_TX - offset) ;
  }
  BYTE_IN_AR(compl_workbuf, offset - 1 ) = (uint8_t)cmdLength;

  memcpy(compl_workbuf + offset, (uint8_t*)&pReceiveMulti->Payload, cmdLength);

  if (pReceiveMulti->RxOptions.rxStatus & RECEIVE_STATUS_TYPE_MULTI)
  {
    /* Its a Multicast frame */

    // Create NodeMaskHeader to comply with SerialAPI
    const SMultiCastNodeMaskHeaderSerial NodeMaskHeader = {
        .iNodemaskLength = 29, // Always offset full nodemask. Hardwired to 29 (and not
                               // nodemask define) since SerialAPI is not supposed to change.
        .iNodeMaskOffset = 0   // Always full nodemask -> No offset
    };

    i = NodeMaskHeader.iNodemaskLength + 1; // + 1 for node mask headers own size.
    if (i > (uint8_t)(BUF_SIZE_TX - (offset + cmdLength)))
    {
      i = (uint8_t)(BUF_SIZE_TX - (offset + cmdLength + 1));
    }
    if (i > 0)
    {
      *(compl_workbuf + offset + cmdLength) = i - 1;
      memcpy(compl_workbuf + offset + 1 + cmdLength, (uint8_t*)pReceiveMulti->NodeMask, i - 1); // +- 1 as we already copied node mask header
      i += (uint8_t)cmdLength;
    }
  }
  else
  {
    if (cmdLength >= (uint8_t)(BUF_SIZE_TX - offset) )
    {
      cmdLength = (uint8_t)(BUF_SIZE_TX - offset -1);
      i = (uint8_t)cmdLength;
    }
    else
    {
      i = (uint8_t)(cmdLength + 1);
    }
    BYTE_IN_AR(compl_workbuf, (uint8_t)(offset + cmdLength)) = 0;
  }
  BYTE_IN_AR(compl_workbuf, offset + i) = (uint8_t)pReceiveMulti->RxOptions.rxRSSIVal;
  if (SERIAL_API_SETUP_NODEID_BASE_TYPE_16_BIT == nodeIdBaseType)
  {
    BYTE_IN_AR(compl_workbuf, offset + ++i) = pReceiveMulti->RxOptions.securityKey; //inclusion fails without this
    BYTE_IN_AR(compl_workbuf, offset + ++i) = (uint8_t)pReceiveMulti->RxOptions.bSourceTxPower;
    BYTE_IN_AR(compl_workbuf, offset + ++i) = (uint8_t)pReceiveMulti->RxOptions.bSourceNoiseFloor;
  }
  /* Unified Application Command Handler for Bridge and Virtual nodes */
  RequestUnsolicited(FUNC_ID_APPLICATION_COMMAND_HANDLER_BRIDGE, compl_workbuf, (uint8_t)(offset + 1 + i));
#else
  /* Simulate old split Application Command Handlers */
  uint8_t offset = 0;
  if (!IsNodeVirtual(destNode))
  {
    /* ZW->PC: REQ | 0x04 | rxStatus | sourceNode | cmdLength | Payload */
    BYTE_IN_AR(compl_workbuf, 0) = pReceiveMulti->RxOptions.rxStatus;
    if (SERIAL_API_SETUP_NODEID_BASE_TYPE_16_BIT == nodeIdBaseType)
    {
      BYTE_IN_AR(compl_workbuf, offset + 1) = (uint8_t)(pReceiveMulti->RxOptions.sourceNode >> 8);     // MSB
      BYTE_IN_AR(compl_workbuf, offset + 2) = (uint8_t)(pReceiveMulti->RxOptions.sourceNode & 0xFF);   // LSB
      offset = 4;  // 16 bit nodeID means the command fields that follow are offset by one byte
    }
    else
    {
      BYTE_IN_AR(compl_workbuf, 1) = (uint8_t)pReceiveMulti->RxOptions.sourceNode;  // Legacy 8 bit
      offset = 3;
    }
    if (cmdLength > (uint8_t)(BUF_SIZE_TX - offset) )
    {
      cmdLength = (uint8_t)(BUF_SIZE_TX - offset) ;
    }
    BYTE_IN_AR(compl_workbuf, offset - 1 ) = (uint8_t)cmdLength;
    for (i = 0; i < cmdLength; i++)
    {
      BYTE_IN_AR(compl_workbuf, offset + i) = *((uint8_t*)&pReceiveMulti->Payload + i);
    }
    RequestUnsolicited(FUNC_ID_APPLICATION_COMMAND_HANDLER, compl_workbuf, offset + cmdLength);
  }
  else
  {
    /* ZW->PC: REQ | 0xA1 | rxStatus | destNode | sourceNode | cmdLength | Payload */
    BYTE_IN_AR(compl_workbuf, 0) = pReceiveMulti->RxOptions.rxStatus;
    if (SERIAL_API_SETUP_NODEID_BASE_TYPE_16_BIT == nodeIdBaseType)
    {
      BYTE_IN_AR(compl_workbuf, 1) = (uint8_t)(pReceiveMulti->RxOptions.destNode >> 8);       // MSB
      BYTE_IN_AR(compl_workbuf, 2) = (uint8_t)(pReceiveMulti->RxOptions.destNode & 0xFF);     // LSB
      BYTE_IN_AR(compl_workbuf, 3) = (uint8_t)(pReceiveMulti->RxOptions.sourceNode >> 8);     // MSB
      BYTE_IN_AR(compl_workbuf, 4) = (uint8_t)(pReceiveMulti->RxOptions.sourceNode & 0xFF);   // LSB
      offset = 6;  // 16 bit nodeIDs means the command fields that follow are offset by two bytes
    }
    else
    {
      // Legacy 8 bit nodeIDs
      BYTE_IN_AR(compl_workbuf, 1) = (uint8_t)pReceiveMulti->RxOptions.destNode;
      BYTE_IN_AR(compl_workbuf, 2) = (uint8_t)pReceiveMulti->RxOptions.sourceNode;
      offset = 4;
    }
    if (cmdLength > (uint8_t)(BUF_SIZE_TX - offset))
    {
      cmdLength = (uint8_t)(BUF_SIZE_TX - offset) ;
    }
    BYTE_IN_AR(compl_workbuf, offset - 1) = cmdLength;
    for (i = 0; i < cmdLength; i++)
    {
      BYTE_IN_AR(compl_workbuf, offset + i) = *((uint8_t*)&pReceiveMulti->Payload + i);
    }

    RequestUnsolicited(FUNC_ID_APPLICATION_SLAVE_COMMAND_HANDLER, compl_workbuf, offset + cmdLength);
  }
#endif
}
#endif


/*======================   ApplicationNodeUpdate   =========================
**    Inform the static controller/slave of node information received
**
**--------------------------------------------------------------------------*/
void                                /* RET  Nothing                         */
ApplicationNodeUpdate(
  uint8_t bStatus,                     /* IN   Status of learn mode            */
  uint16_t nodeID,                    /* IN   Node id of node sending nodeinfo*/
  uint8_t *pCmd,                       /* IN   Pointer to appl. node info      */
  uint8_t bLen                         /* IN   Node info length                */
)
{
  uint8_t offset = 0;
  BYTE_IN_AR(compl_workbuf, 0) = bStatus;
  if (SERIAL_API_SETUP_NODEID_BASE_TYPE_16_BIT == nodeIdBaseType)
  {
    BYTE_IN_AR(compl_workbuf, 1) = (uint8_t)(nodeID >> 8);     // MSB
    BYTE_IN_AR(compl_workbuf, 2) = (uint8_t)(nodeID & 0xFF);   // LSB
    offset++;  // 16 bit nodeID means the command fields that follow are offset by one byte
  }
  else
  {
    BYTE_IN_AR(compl_workbuf, 1) = (uint8_t)(nodeID & 0xFF);      // Legacy 8 bit nodeID
  }

  /*  - Buffer boundary check */
  if (bLen > (uint8_t)(BUF_SIZE_TX - (offset + 3)))
  {
    bLen = (uint8_t)(BUF_SIZE_TX - (offset + 3));
  }
  BYTE_IN_AR(compl_workbuf, offset + 2) = bLen;
  for (uint8_t i = 0; i < bLen; i++)
  {
    BYTE_IN_AR(compl_workbuf, offset + 3 + i) = *(pCmd + i);
  }
  RequestUnsolicited(FUNC_ID_ZW_APPLICATION_UPDATE, compl_workbuf, (uint8_t)(offset + bLen + 3));
}

/*
 * The below functions should be implemented as hardware specific functions in a separate source
 * file, e.g. SerialAPI_hw.c.
 */
ZW_WEAK void SerialAPI_hw_init(void)
{

}

#ifdef USB_SUSPEND_SUPPORT

ZW_WEAK void SerialAPI_set_usb_supend_callback(SerialAPI_hw_usb_suspend_callback_t callback)
{
  UNUSED(callback);
}

ZW_WEAK void SerialAPI_hw_usb_suspend_handler(void)
{

}

#endif

