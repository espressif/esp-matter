/**
 * Z-Wave Certified Application Switch On/Off
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include "zw_config_rf.h"
#include <stdbool.h>
#include <stdint.h>
#include "SizeOf.h"
#include "Assert.h"
#include <MfgTokens.h>
#include "DebugPrintConfig.h"
//#define DEBUGPRINT
#include "DebugPrint.h"
#include "config_app.h"
#include <ZAF_file_ids.h>
#include "ZAF_nvm_app.h"
#include <ZW_slave_api.h>
#include <ZW_classcmd.h>
#include <ZW_TransportLayer.h>

#include <string.h>
#include <ev_man.h>

#include <AppTimer.h>
#include <SwTimer.h>
#include <EventDistributor.h>
#include <ZW_system_startup_api.h>
#include <ZW_application_transport_interface.h>

#include <association_plus.h>
#include <agi.h>

#include <CC_Association.h>
#include <CC_AssociationGroupInfo.h>
#include <CC_Basic.h>
#include <CC_BinarySwitch.h>
#include <CC_DeviceResetLocally.h>
#include <CC_Indicator.h>
#include <CC_MultiChanAssociation.h>
#include <CC_Supervision.h>
#include <CC_FirmwareUpdate.h>
#include <CC_ManufacturerSpecific.h>
#include <zaf_config_api.h>

#include <zaf_event_helper.h>
#include <zaf_job_helper.h>

#include <ZAF_Common_helper.h>
#include <ZAF_network_learn.h>
#include <ota_util.h>
#include <ZAF_TSE.h>
#include <ZAF_CmdPublisher.h>
#include <ZAF_network_management.h>
#include "events.h"
#include <zpal_watchdog.h>
#include <zpal_misc.h>
#include <zpal_radio.h>
#include <SwitchOnOff_hw.h>
#include <board_indicator.h>
#include <board_init.h>
#include "zw_region_config.h"
#include "zw_build_no.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * Application states. Function AppStateManager(..) includes the state
 * event machine.
 */
typedef enum _STATE_APP_
{
  STATE_APP_STARTUP,
  STATE_APP_IDLE,
  STATE_APP_LEARN_MODE,
  STATE_APP_RESET,
} STATE_APP;

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

/**
 * Please see the description of app_node_information_t.
 */
static uint8_t cmdClassListNonSecureNotIncluded[] =
{
  COMMAND_CLASS_ZWAVEPLUS_INFO,
  COMMAND_CLASS_SWITCH_BINARY_V2,
  COMMAND_CLASS_ASSOCIATION,
  COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2,
  COMMAND_CLASS_ASSOCIATION_GRP_INFO,
  COMMAND_CLASS_TRANSPORT_SERVICE_V2,
  COMMAND_CLASS_VERSION,
  COMMAND_CLASS_MANUFACTURER_SPECIFIC,
  COMMAND_CLASS_DEVICE_RESET_LOCALLY,
  COMMAND_CLASS_INDICATOR,
  COMMAND_CLASS_POWERLEVEL,
  COMMAND_CLASS_SECURITY,
  COMMAND_CLASS_SECURITY_2,
  COMMAND_CLASS_SUPERVISION,
  COMMAND_CLASS_FIRMWARE_UPDATE_MD_V5
};

/**
 * Please see the description of app_node_information_t.
 */
static uint8_t cmdClassListNonSecureIncludedSecure[] =
{
  COMMAND_CLASS_ZWAVEPLUS_INFO,
  COMMAND_CLASS_TRANSPORT_SERVICE_V2,
  COMMAND_CLASS_SECURITY,
  COMMAND_CLASS_SECURITY_2,
  COMMAND_CLASS_SUPERVISION
};

/**
 * Please see the description of app_node_information_t.
 */
static uint8_t cmdClassListSecure[] =
{
  COMMAND_CLASS_VERSION,
  COMMAND_CLASS_SWITCH_BINARY_V2,
  COMMAND_CLASS_ASSOCIATION,
  COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2,
  COMMAND_CLASS_ASSOCIATION_GRP_INFO,
  COMMAND_CLASS_MANUFACTURER_SPECIFIC,
  COMMAND_CLASS_DEVICE_RESET_LOCALLY,
  COMMAND_CLASS_INDICATOR,
  COMMAND_CLASS_POWERLEVEL,
  COMMAND_CLASS_FIRMWARE_UPDATE_MD_V5
};

/**
 * Structure includes application node information list's and device type.
 */
app_node_information_t m_AppNIF =
{
  cmdClassListNonSecureNotIncluded, sizeof(cmdClassListNonSecureNotIncluded),
  cmdClassListNonSecureIncludedSecure, sizeof(cmdClassListNonSecureIncludedSecure),
  cmdClassListSecure, sizeof(cmdClassListSecure),
  DEVICE_OPTIONS_MASK, {GENERIC_TYPE, SPECIFIC_TYPE}
};


/**
* Set up security keys to request when joining a network.
* These are taken from the config_app.h header file.
*/
static const uint8_t SecureKeysRequested = REQUESTED_SECURITY_KEYS;

static const SAppNodeInfo_t AppNodeInfo =
{
  .DeviceOptionsMask = DEVICE_OPTIONS_MASK,
  .NodeType.generic = GENERIC_TYPE,
  .NodeType.specific = SPECIFIC_TYPE,
  .CommandClasses.UnSecureIncludedCC.iListLength = sizeof_array(cmdClassListNonSecureNotIncluded),
  .CommandClasses.UnSecureIncludedCC.pCommandClasses = cmdClassListNonSecureNotIncluded,
  .CommandClasses.SecureIncludedUnSecureCC.iListLength = sizeof_array(cmdClassListNonSecureIncludedSecure),
  .CommandClasses.SecureIncludedUnSecureCC.pCommandClasses = cmdClassListNonSecureIncludedSecure,
  .CommandClasses.SecureIncludedSecureCC.iListLength = sizeof_array(cmdClassListSecure),
  .CommandClasses.SecureIncludedSecureCC.pCommandClasses = cmdClassListSecure
};

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
  .pVirtualSlaveNodeInfoTable = NULL,
  .pSecureKeysRequested = &SecureKeysRequested,
  .pNodeInfo = &AppNodeInfo,
  .pRadioConfig = &RadioConfig
};

/**
 * Application state-machine state.
 */
static STATE_APP currentState = STATE_APP_IDLE;

/**
 * LED state
 */
static CMD_CLASS_BIN_SW_VAL onOffState = CMD_CLASS_BIN_OFF;

static TaskHandle_t g_AppTaskHandle;

#ifdef DEBUGPRINT
static uint8_t m_aDebugPrintBuffer[96];
#endif

// Pointer to AppHandles that is passed as input to ApplicationTask(..)
SApplicationHandles* g_pAppHandles;

// Prioritized events that can wakeup protocol thread.
typedef enum EApplicationEvent
{
  EAPPLICATIONEVENT_TIMER = 0,
  EAPPLICATIONEVENT_ZWRX,
  EAPPLICATIONEVENT_ZWCOMMANDSTATUS,
  EAPPLICATIONEVENT_APP
} EApplicationEvent;

static void EventHandlerZwRx(void);
static void EventHandlerZwCommandStatus(void);
static void EventHandlerApp(void);

// Event distributor object
static SEventDistributor g_EventDistributor;

// Event distributor event handler table
static const EventDistributorEventHandler g_aEventHandlerTable[4] =
{
  AppTimerNotificationHandler,  // Event 0
  EventHandlerZwRx,
  EventHandlerZwCommandStatus,
  EventHandlerApp
};

// Used by the application data file.
typedef struct SApplicationData
{
  uint8_t onOffState;
} SApplicationData;

#define FILE_SIZE_APPLICATIONDATA     (sizeof(SApplicationData))

static SApplicationData ApplicationData;

#define APP_EVENT_QUEUE_SIZE 5

/**
 * The following four variables are used for the application event queue.
 */
static SQueueNotifying m_AppEventNotifyingQueue;
static StaticQueue_t m_AppEventQueueObject;
static EVENT_APP eventQueueStorage[APP_EVENT_QUEUE_SIZE];
static QueueHandle_t m_AppEventQueue;

/* True Status Engine (TSE) variables */
/* Indicate here which End Points (including root device) support each command class */
s_CC_binarySwitch_data_t ZAF_TSE_BinarySwitchData;
static s_CC_indicator_data_t ZAF_TSE_localActuationIdentifyData = {
  .rxOptions = {
    .rxStatus = 0,          /* rxStatus, verified by the TSE for Multicast */
    .securityKey = 0,       /* securityKey, ignored by the TSE */
    .sourceNode = {0,0},    /* sourceNode (nodeId, endpoint), verified against lifeline destinations by the TSE */
    .destNode = {0,0}       /* destNode (nodeId, endpoint), verified by the TSE for local endpoint */
  },
  .indicatorId = 0x50      /* Identify Indicator*/
};

/**
* TSE simulated RX option for local change addressed to the Root Device
* All applications can use this variable when triggering the TSE after
* a local / non Z-Wave initiated change
*/
static RECEIVE_OPTIONS_TYPE_EX zaf_tse_local_actuation = {
    .rxStatus             = 0,      /* rxStatus, verified by the TSE for Multicast */
    .securityKey          = 0,      /* securityKey, ignored by the TSE */
    .sourceNode           = {0,0},  /* sourceNode (nodeId, endpoint), verified against lifeline destinations by the TSE */
    .destNode             = {0,0},   /* destNode (nodeId, endpoint), verified by the TSE for local endpoint */
    .bSupervisionActive   = 0,       /* true if supervision is active*/
    .sessionId            = 0,       /* Current sessionId*/
    .statusUpdate         = 0        /*Is statusUpdate enabled for current session */
};

static zpal_nvm_handle_t pFileSystemApplication;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/
STATE_APP GetAppState(void);
void AppStateManager(EVENT_APP event);
static void ChangeState(STATE_APP newState);

static void ApplicationTask(SApplicationHandles* pAppHandles);

bool LoadConfiguration(void);
void SetDefaultConfiguration(void);
void ToggleLed(void);
void UpdateSwitch(void);
SApplicationData readAppData(void);
void writeAppData(const SApplicationData* pAppData);

void AppResetNvm(void);

static void indicator_set_handler(uint32_t on_time_ms, uint32_t off_time_ms, uint32_t num_cycles);

/**
* @brief Called when protocol puts a frame on the ZwRxQueue.
*/
static void EventHandlerZwRx(void)
{
  QueueHandle_t Queue = g_pAppHandles->ZwRxQueue;
  SZwaveReceivePackage RxPackage;

  // Handle incoming replies
  while (xQueueReceive(Queue, (uint8_t*)(&RxPackage), 0) == pdTRUE)
  {
    DPRINT("Incoming Rx msg\r\n");

    switch (RxPackage.eReceiveType)
    {
      case EZWAVERECEIVETYPE_SINGLE:
      {
        ZAF_CP_CommandPublish(ZAF_getCPHandle(), (void *) &RxPackage);
        break;
      }

      case EZWAVERECEIVETYPE_NODE_UPDATE:
      {
        /*ApplicationSlaveUpdate() was called from this place in version prior to SDK7*/
        break;
      }

      case EZWAVERECEIVETYPE_SECURITY_EVENT:
      {
        /*ApplicationSecurityEvent() was used to support CSA, not needed in SDK7*/
        break;
      }

      case EZWAVERECEIVETYPE_STAY_AWAKE:
      {
        /*
         * Non-application frame was received, that must keep device awake.
         */
        break;
      }

    default:
      {
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
  while (xQueueReceive(Queue, (uint8_t*)(&Status), 0) == pdTRUE)
  {
    DPRINT("Incoming Status msg\r\n");

    switch (Status.eStatusType)
    {
      case EZWAVECOMMANDSTATUS_TX:
      {
        SZWaveTransmitStatus* pTxStatus = &Status.Content.TxStatus;
        if (!pTxStatus->bIsTxFrameLegal)
        {
          DPRINT("Auch - not sure what to do\r\n");
        }
        else
        {
          DPRINTF("Tx Status received: %#02x\r\n", pTxStatus->TxStatus);
          if (pTxStatus->Handle)
          {
            ZW_TX_Callback_t pCallback = (ZW_TX_Callback_t)pTxStatus->Handle;
            pCallback(pTxStatus->TxStatus, &pTxStatus->ExtendedTxStatus);
          }
        }

        break;
      }

      case EZWAVECOMMANDSTATUS_GENERATE_RANDOM:
      {
        DPRINT("Generate Random status\r\n");
        break;
      }

      case EZWAVECOMMANDSTATUS_LEARN_MODE_STATUS:
      {
        DPRINTF("Learn status %d\r\n", Status.Content.LearnModeStatus.Status);
        if (ELEARNSTATUS_ASSIGN_COMPLETE == Status.Content.LearnModeStatus.Status)
        {
          // When security S0 or higher is set, remove all settings which happen before secure inclusion
          // calling function SetDefaultConfiguration(). The same function is used when there is an
          // EINCLUSIONSTATE_EXCLUDED.
          if ( (EINCLUSIONSTATE_EXCLUDED == ZAF_GetInclusionState()) ||
                      (SECURITY_KEY_NONE != GetHighestSecureLevel(ZAF_GetSecurityKeys())) )
          {
            SetDefaultConfiguration();
          }
          ZAF_EventHelperEventEnqueue((EVENT_APP) EVENT_SYSTEM_LEARNMODE_FINISHED);
          ZAF_Transport_OnLearnCompleted();
        }
        else if(ELEARNSTATUS_SMART_START_IN_PROGRESS == Status.Content.LearnModeStatus.Status)
        {
          ZAF_EventHelperEventEnqueue(EVENT_APP_SMARTSTART_IN_PROGRESS);
        }
        else if(ELEARNSTATUS_LEARN_MODE_COMPLETED_TIMEOUT == Status.Content.LearnModeStatus.Status)
        {
          ZAF_EventHelperEventEnqueue((EVENT_APP) EVENT_SYSTEM_LEARNMODE_FINISHED);
        }
        else if(ELEARNSTATUS_LEARN_MODE_COMPLETED_FAILED == Status.Content.LearnModeStatus.Status)
        {
          //Reformats protocol and application NVM. Then soft reset.
          ZAF_EventHelperEventEnqueue((EVENT_APP) EVENT_SYSTEM_RESET);
        }
        break;
      }

      case EZWAVECOMMANDSTATUS_NETWORK_LEARN_MODE_START:
      {
        break;
      }

      case EZWAVECOMMANDSTATUS_SET_DEFAULT:
      { // Received when protocol is started (not implemented yet), and when SetDefault command is completed
        DPRINT("Protocol Ready\r\n");
        ZAF_EventHelperEventEnqueue(EVENT_APP_FLUSHMEM_READY);

        break;
      }

      case EZWAVECOMMANDSTATUS_INVALID_TX_REQUEST:
      {
        DPRINTF("ERROR: Invalid TX Request to protocol - %d", Status.Content.InvalidTxRequestStatus.InvalidTxRequest);
        break;
      }

      case EZWAVECOMMANDSTATUS_INVALID_COMMAND:
      {
        DPRINTF("ERROR: Invalid command to protocol - %d", Status.Content.InvalidCommandStatus.InvalidCommand);
        break;
      }

      case EZWAVECOMMANDSTATUS_ZW_SET_MAX_INCL_REQ_INTERVALS:
      {
        // Status response from calling the ZAF_SetMaxInclusionRequestIntervals function
        DPRINTF("SetMaxInclusionRequestIntervals status: %s\r\n",
                 Status.Content.NetworkManagementStatus.statusInfo[0] == true ? "SUCCESS" : "FAIL");

        // Add your application code here...
        break;
      }

      case EZWAVECOMMANDSTATUS_ZW_SET_TX_ATTENUATION:
      {
        if (false == Status.Content.SetTxAttenuation.result)
        {
          DPRINT("Not allowed to attenuate tx-power\r\n");
        }
        break;
      }

      default:
        ASSERT(false);
        break;
    }
  }
}

/**
 *
 */
static void EventHandlerApp(void)
{
//  DPRINT("Got event!\r\n");

  uint8_t event;

  while (xQueueReceive(m_AppEventQueue, (uint8_t*)(&event), 0) == pdTRUE)
  {
    DPRINTF("Event: %d\r\n", event);
    AppStateManager((EVENT_APP)event);
  }
}

/*
 * Initialized all modules related to event queuing, task notifying and job registering.
 */
static void EventQueueInit()
{
  // Initialize Queue Notifier for events in the application.
  m_AppEventQueue = xQueueCreateStatic(
    sizeof_array(eventQueueStorage),
    sizeof(eventQueueStorage[0]),
    (uint8_t*)eventQueueStorage,
    &m_AppEventQueueObject
  );

  /*
   * Registers events with associated data, and notifies
   * the specific task about a pending job!
   */
  QueueNotifyingInit(
      &m_AppEventNotifyingQueue,
      m_AppEventQueue,
      g_AppTaskHandle,
      EAPPLICATIONEVENT_APP);

  /*
   * Wraps the QueueNotifying module for simple event generations!
   */
  ZAF_EventHelperInit(&m_AppEventNotifyingQueue);

  /*
   * Creates an internal queue to store no more than @ref JOB_QUEUE_BUFFER_SIZE jobs.
   * This module has no notification feature!
   */
  ZAF_JobHelperInit();
}

/**
 * @brief See description for function prototype in ZW_basis_api.h.
 */
ZW_APPLICATION_STATUS
ApplicationInit(EResetReason_t eResetReason)
{
  UNUSED(eResetReason);

  // NULL - We dont have the Application Task handle yet
  AppTimerInit(EAPPLICATIONEVENT_TIMER, NULL);

  /* hardware initialization */
  Board_Init();

#ifdef DEBUGPRINT
  zpal_debug_init();
  DebugPrintConfig(m_aDebugPrintBuffer, sizeof(m_aDebugPrintBuffer), zpal_debug_output);
#endif

  /* Init state machine*/
  currentState = STATE_APP_STARTUP;

  DPRINT("\n\n--------------------------------\n");
  DPRINT("Z-Wave Sample App: Switch On/Off\n");
  DPRINTF("SDK: %d.%d.%d ZAF: %d.%d.%d.%d\n",
          SDK_VERSION_MAJOR,
          SDK_VERSION_MINOR,
          SDK_VERSION_PATCH,
          zpal_get_app_version_major(),
          zpal_get_app_version_minor(),
          zpal_get_app_version_patch(),
          ZAF_BUILD_NO);


  CC_Indicator_Init(indicator_set_handler);

  // Init file system
  ApplicationFileSystemInit(&pFileSystemApplication);

  // Read Rf region from MFG_ZWAVE_COUNTRY_FREQ
  zpal_radio_region_t regionMfg;
  ZW_GetMfgTokenDataCountryFreq((void*) &regionMfg);
  if (isRfRegionValid(regionMfg)) {
    RadioConfig.eRegion = regionMfg;
  } else {
    ZW_SetMfgTokenDataCountryRegion((void*) &RadioConfig.eRegion);
  }
  DPRINTF("Rf region: %d\n", RadioConfig.eRegion);
  DPRINTF("ApplicationInit eResetReason = %d\n", eResetReason);

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

  return(APPLICATION_RUNNING);
}

/**
 * A pointer to this function is passed to ZW_ApplicationRegisterTask() making it the FreeRTOS
 * application task.
 */
static void
ApplicationTask(SApplicationHandles* pAppHandles)
{
  // Init
  DPRINT("Enabling watchdog\n");
  zpal_enable_watchdog(true);

  g_AppTaskHandle = xTaskGetCurrentTaskHandle();
  g_pAppHandles = pAppHandles;
  AppTimerSetReceiverTask(g_AppTaskHandle);

  ZAF_Init(g_AppTaskHandle, pAppHandles, &ProtocolConfig, NULL);

  /*
   * Create an initialize some of the modules regarding queue and event handling and passing.
   * The UserTask that is dependent on modules initialized here, must be able to detect and wait
   * before using these modules. Specially if it has higher priority than this task!
   *
   * Currently, the UserTask is checking whether zaf_event_helper.h module is ready.
   * This module is the last to be initialized!
   */
  EventQueueInit();

  ZAF_EventHelperEventEnqueue(EVENT_APP_INIT);

  SwitchOnOff_hw_init();

  Board_IndicatorInit();
  Board_IndicateStatus(BOARD_STATUS_IDLE);

  CommandClassSupervisionInit(
      CC_SUPERVISION_STATUS_UPDATES_NOT_SUPPORTED,
      NULL,
      NULL);

  EventDistributorConfig(
    &g_EventDistributor,
    sizeof_array(g_aEventHandlerTable),
    g_aEventHandlerTable,
    NULL
    );

  // Wait for and process events
  DPRINT("SwitchOnOff Event processor Started\r\n");
  uint32_t iMaxTaskSleep = 0xFFFFFFFF;
  for (;;)
  {
    if (0xFFFFFFFF == EventDistributorDistribute(&g_EventDistributor, iMaxTaskSleep, 0))
    {
      return;
    }
  }
}

/**
 * @brief Returns the current state of the application state machine.
 * @return Current state
 */
STATE_APP
GetAppState(void)
{
  return currentState;
}

static void doRemainingInitialization()
{
  /* Load the application settings from NVM file system */
  LoadConfiguration();
  /*it is not the time to wake completely*/

  // Setup AGI group lists
  AGI_Init();

  /*
   * Initialize Event Scheduler.
   */
  Transport_OnApplicationInitSW( &m_AppNIF, NULL);

  /* Enter SmartStart*/
  /* Protocol will commence SmartStart only if the node is NOT already included in the network */
  ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_INCLUSION_SMARTSTART);

  /* Init state machine*/
  ZAF_EventHelperEventEnqueue(EVENT_EMPTY);
}

/**
 * @brief The core state machine of this sample application.
 * @param event The event that triggered the call of AppStateManager.
 */
void
AppStateManager(EVENT_APP event)
{
  DPRINTF("AppStateManager St: %d, Ev: %d\r\n", currentState, event);

  if ((EVENT_APP_BUTTON_LEARN_RESET_LONG_PRESS == event) ||
      (EVENT_SYSTEM_RESET == (EVENT_SYSTEM)event))
  {
    /*Force state change to activate system-reset without taking care of current state.*/
    ChangeState(STATE_APP_RESET);
    /* Send reset notification*/
    CC_DeviceResetLocally_notification_tx();
  }

  switch(currentState)
  {

    case STATE_APP_STARTUP:
      if(EVENT_APP_FLUSHMEM_READY == event)
      {
        AppResetNvm();
      }

      if (EVENT_APP_INIT == event)
      {
        /*
         * This approach makes it possible to do less initialization before the scheduler starts.
         * Hence, this was made to reduce the boot-up time.
         */
        doRemainingInitialization();
        break;
      }
      CC_FirmwareUpdate_Init(NULL, NULL, true);
      ChangeState(STATE_APP_IDLE);
      break;

    case STATE_APP_IDLE:
      if(EVENT_APP_REFRESH_MMI == event)
      {
        Board_IndicateStatus(BOARD_STATUS_IDLE);
      }

      if(EVENT_APP_FLUSHMEM_READY == event)
      {
        AppResetNvm();
        LoadConfiguration();
      }

      if (EVENT_APP_BUTTON_LEARN_RESET_SHORT_PRESS == event ||
          (EVENT_SYSTEM_LEARNMODE_START == (EVENT_SYSTEM)event))
      {
        if (EINCLUSIONSTATE_EXCLUDED != g_pAppHandles->pNetworkInfo->eInclusionState)
        {
          DPRINT("LEARN_MODE_EXCLUSION\r\n");
          ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_EXCLUSION_NWE);
        }
        else
        {
          DPRINT("LEARN_MODE_INCLUSION\r\n");
          ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_INCLUSION);
        }
        ChangeState(STATE_APP_LEARN_MODE);
      }

      if (EVENT_APP_BUTTON_TOGGLE_SHORT_PRESS == event)
      {
        DPRINT("ToggleLed ");
        ToggleLed();
        /* Update the lifeline destinations when the Binary Switch state has changed */
        void * pData = CC_BinarySwitch_prepare_zaf_tse_data(&zaf_tse_local_actuation);
        ZAF_TSE_Trigger(CC_BinarySwitch_report_stx, pData, true);
      }

      if (EVENT_APP_BUTTON_TOGGLE_HOLD == event)
      {
        /*
         * This shows how to force the transmission of an Included NIF. Normally, an INIF is
         * transmitted when the product is power cycled while already being included in a network.
         * If the enduser cannot power cycle the product, e.g. because the battery cannot be
         * removed, the transmission of an INIF must be manually triggered.
         */
        DPRINT("TX INIF");
        ZAF_SendINIF(NULL);
      }

      break;

    case STATE_APP_LEARN_MODE:
      if (EVENT_APP_REFRESH_MMI == event)
      {
        Board_IndicateStatus(BOARD_STATUS_LEARNMODE_ACTIVE);
      }

      if(EVENT_APP_FLUSHMEM_READY == event)
      {
        AppResetNvm();
        LoadConfiguration();
      }

      if (EVENT_APP_BUTTON_LEARN_RESET_SHORT_PRESS == event ||
          (EVENT_SYSTEM_LEARNMODE_STOP == (EVENT_SYSTEM)event))
      {
        ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_DISABLE);
        Board_IndicateStatus(BOARD_STATUS_IDLE);

        //Go back to smart start if the node was never included.
        //Protocol will not commence SmartStart if the node is already included in the network.
        ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_INCLUSION_SMARTSTART);

        ChangeState(STATE_APP_IDLE);

        /* If we are in a network and the Identify LED state was changed to idle due to learn mode, report it to lifeline */
        if (!Board_IsIndicatorActive())
        {
          CC_Indicator_RefreshIndicatorProperties();
        }
        ZAF_TSE_Trigger(CC_Indicator_report_stx,
                        (void *)&ZAF_TSE_localActuationIdentifyData,
                        true);
      }

      if (EVENT_SYSTEM_LEARNMODE_FINISHED == (EVENT_SYSTEM)event)
      {
        //Go back to smart start if the node was excluded.
        //Protocol will not commence SmartStart if the node is already included in the network.
        ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_INCLUSION_SMARTSTART);

        ChangeState(STATE_APP_IDLE);

        /* If we are in a network and the Identify LED state was changed to idle due to learn mode, report it to lifeline */
        if (!Board_IsIndicatorActive())
        {
          CC_Indicator_RefreshIndicatorProperties();
        }
        ZAF_TSE_Trigger(CC_Indicator_report_stx,
                        (void *)&ZAF_TSE_localActuationIdentifyData,
                        true);
      }
      break;

    case STATE_APP_RESET:
      if(EVENT_APP_REFRESH_MMI == event){}

      if(EVENT_APP_FLUSHMEM_READY == event)
      {
        AppResetNvm();
        /* Soft reset */
        zpal_reboot();
      }
      break;

    default:
      // Do nothing.
      break;
  }
}

/**
 * @brief Sets the current state to a new, given state.
 * @param newState New state.
 */
static void
ChangeState(STATE_APP newState)
{
  DPRINTF("State changed from %d to %d\r\n", currentState, newState);

  currentState = newState;

  /**< Pre-action on new state is to refresh MMI */
  ZAF_EventHelperEventEnqueue(EVENT_APP_REFRESH_MMI);
}

/**
 * @brief Transmission callback for Device Reset Locally call.
 * @param pTransmissionResult Result of each transmission.
 */
void
CC_DeviceResetLocally_done(TRANSMISSION_RESULT * pTransmissionResult)
{
  if (TRANSMISSION_RESULT_FINISHED == pTransmissionResult->isFinished)
  {
    /* Reset protocol */
    // Set default command to protocol
    SZwaveCommandPackage CommandPackage;
    CommandPackage.eCommandType = EZWAVECOMMANDTYPE_SET_DEFAULT;

    DPRINT("\nDisabling watchdog during reset\n");
    zpal_enable_watchdog(false);

    EQueueNotifyingStatus Status = QueueNotifyingSendToBack(g_pAppHandles->pZwCommandQueue,
                                                            (uint8_t*)&CommandPackage,
                                                            500);
    ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == Status);
  }
}

/**
 * Report current switch value to binary switch command class handler
 *
 * @brief See description for function prototype in CC_BinarySwitch.h
 */
CMD_CLASS_BIN_SW_VAL
appBinarySwitchGetCurrentValue(uint8_t endpoint)
{
  UNUSED(endpoint);
  return onOffState;
}

/**
 * Report current target value to binary switch command class handler
 *
 * @brief See description for function prototype in CC_BinarySwitch.h
 */
CMD_CLASS_BIN_SW_VAL
appBinarySwitchGetTargetValue(uint8_t endpoint)
{
  /* Always report target value equal to current value  */
  return appBinarySwitchGetCurrentValue(endpoint);
}

/**
 * Report current duration to binary switch command class handler
 *
 * @brief See description for function prototype in CC_BinarySwitch.h
 */
uint8_t
appBinarySwitchGetDuration(uint8_t endpoint)
{
  UNUSED(endpoint);

  /* Always report instant transition; the only reasonable transition for a binary switch. */
  return 0;
}

/**
 * Report current duration to binary switch command class handler
 *
 * @brief See description for function prototype in CC_BinarySwitch.h
 */
e_cmd_handler_return_code_t
appBinarySwitchSet(
  CMD_CLASS_BIN_SW_VAL val,
  uint8_t duration,
  uint8_t endpoint
)
{
  UNUSED(endpoint);
  UNUSED(duration); /* Ignore duration - for a binary switch only instant transitions make sense */

  DPRINTF("\r\nappBinarySwitchSet %u\r\n", val);

  onOffState = val;
  ApplicationData.onOffState = val;
  UpdateSwitch();
  writeAppData(&ApplicationData);

  return E_CMD_HANDLER_RETURN_CODE_HANDLED;
}

/**
 * Report factory default switch transition duration to binary switch command class handler
 *
 * @brief See description for function prototype in CC_BinarySwitch.h
 */
uint8_t
appBinarySwitchGetFactoryDefaultDuration(uint8_t endpoint)
{
  UNUSED(endpoint);
  return 0;
}

/**
 * @brief Function resets configuration to default values.
 *
 * Add application specific functions here to initialize configuration values stored in persistent memory.
 * Will be called at any of the following events:
 *  - Network Exclusion
 *  - Network Secure Inclusion (after S2 bootstrapping complete)
 *  - Device Reset Locally
 */
void
SetDefaultConfiguration(void)
{
  AssociationInit(true, pFileSystemApplication);

  appBinarySwitchSet(CMD_CLASS_BIN_OFF, 0, 0);

  ZAF_Reset();

  uint32_t appVersion = zpal_get_app_version();
  const zpal_status_t status = zpal_nvm_write(pFileSystemApplication, ZAF_FILE_ID_APP_VERSION, &appVersion, ZAF_FILE_SIZE_APP_VERSION);
  ASSERT(ZPAL_STATUS_OK == status);
}

/**
 * @brief This function loads the application settings from file system.
 * If no settings are found, default values are used and saved.
 */
bool
LoadConfiguration(void)
{
  uint32_t appVersion;
  const zpal_status_t status = zpal_nvm_read(pFileSystemApplication, ZAF_FILE_ID_APP_VERSION, &appVersion, ZAF_FILE_SIZE_APP_VERSION);

  if (ZPAL_STATUS_OK == status)
  {
    if (zpal_get_app_version() != appVersion)
    {
      // Add code for migration of file system to higher version here.
    }

    ApplicationData = readAppData();
    onOffState = ApplicationData.onOffState;

    /* Initialize association module */
    AssociationInit(false, pFileSystemApplication);

    return true;
  }
  else
  {
    DPRINT("Application FileSystem Verify failed\r\n");

    // Reset the file system if ZAF_FILE_ID_APP_VERSION is missing since this indicates
    // corrupt or missing file system.
    AppResetNvm();

    return false;
  }
}

void AppResetNvm(void)
{
  DPRINT("Resetting application FileSystem to default\r\n");

  ASSERT(0 != pFileSystemApplication); //Assert has been kept for debugging , can be removed from production code. This error can only be caused by some internal flash HW failure

  const zpal_status_t status = zpal_nvm_erase_all(pFileSystemApplication);
  ASSERT(ZPAL_STATUS_OK == status); //Assert has been kept for debugging , can be removed from production code. This error can only be caused by some internal flash HW failure

  /* Apparently there is no valid configuration in file system, so load */
  /* default values and save them to file system. */
  SetDefaultConfiguration();
}

/**
 * @brief Handler for basic get. Handles received basic get commands.
 */
uint8_t
CC_Basic_GetCurrentValue_handler(uint8_t endpoint)
{
  return appBinarySwitchGetCurrentValue(endpoint);
}

/**
 * @brief Report the target value
 * @return target value.
 */
uint8_t
CC_Basic_GetTargetValue_handler(uint8_t endpoint)
{
  return appBinarySwitchGetTargetValue(endpoint);
}

/**
 * @brief Report transition duration time.
 * @return duration time.
 */
uint8_t
CC_Basic_GetDuration_handler(uint8_t endpoint)
{
  return appBinarySwitchGetDuration(endpoint);
}

/**
 * @brief Toggles LED state variable and refreshes MMI.
 */
void
ToggleLed(void)
{
  if (CMD_CLASS_BIN_ON == onOffState)
  {
    onOffState = CMD_CLASS_BIN_OFF;
  }
  else
  {
    onOffState = CMD_CLASS_BIN_ON;
  }
  ApplicationData.onOffState = onOffState;
  UpdateSwitch();
  writeAppData(&ApplicationData);
}

/**
 * @brief Update switch state.
 */
void
UpdateSwitch(void)
{
  if (CMD_CLASS_BIN_OFF == onOffState)
  {
    SwitchOnOff_hw_binary_switch_handler(false);
    DPRINT("Binary Switch OFF\r\n");
  }
  else if (CMD_CLASS_BIN_ON == onOffState)
  {
    SwitchOnOff_hw_binary_switch_handler(true);
    DPRINT("Binary Switch ON\r\n");
  }
}

/**
 * @brief Reads application data from file system.
 */
SApplicationData
readAppData(void)
{
  ASSERT(0 != pFileSystemApplication); //Assert has been kept for debugging , can be removed from production code. This error can only be caused by some internal flash HW failure

  SApplicationData AppData;
  const zpal_status_t status = zpal_nvm_read(pFileSystemApplication, FILE_ID_APPLICATIONDATA, &AppData, sizeof(SApplicationData));
  ASSERT(ZPAL_STATUS_OK == status); //Assert has been kept for debugging , can be removed from production code. This error hard to occur when a corresponing write is successfull
                                    //Can only happended in case of some hardware failure

  return AppData;
}

/**
 * @brief Writes application data to file system.
 */
void writeAppData(const SApplicationData* pAppData)
{
  ASSERT(0 != pFileSystemApplication); //Assert has been kept for debugging , can be removed from production code. This means nvm3_open failed can only be caused by some internal flash HW failure

  const zpal_status_t status = zpal_nvm_write(pFileSystemApplication, FILE_ID_APPLICATIONDATA, pAppData, sizeof(SApplicationData));
  ASSERT(ZPAL_STATUS_OK == status); //Assert has been kept for debugging , can be removed from production code. This error can only be caused by some internal flash HW failure
}

uint16_t handleFirmWareIdGet(uint8_t n)
{
  if (n == 0)
  {
    // Firmware 0
    return APP_FIRMWARE_ID;
  }
  // Invalid Firmware number.
  return 0;
}


/**
 * This function is used to return a pointer for input data for the TSE trigger.
 * @param rxOption pointer that will be included in the ZAF_TSE input data.
*/
void* CC_Basic_prepare_zaf_tse_data(RECEIVE_OPTIONS_TYPE_EX* pRxOpt)
{
  /* Basic is mapped to Binary Switch for Root device, no endpoint for this app
  Call here different functions for each endpoint (rxOpt->destNode.endpoint)
  if they do not map to the same Command Class & Command */
  return CC_BinarySwitch_prepare_zaf_tse_data(pRxOpt);
}

/**
 * This function is used to return a pointer for input data for the TSE trigger.
 * @param rxOption pointer that will be included in the ZAF_TSE input data.
*/
void CC_Basic_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions, void* pData)
{
  /* Basic Report is mapped to Binary Switch Report for all end points (only root device).
  Call here different functions for each endpoint (pData->rxOpt->destNode.endpoint)
  if they do not map to the same Command Class & Command */
  CC_BinarySwitch_report_stx(txOptions,pData);
}

/**
 * This function is used to notify the Application that the CC Basic Set
 * status is in a WORKING state. The application can subsequently make the TSE Trigger
 * using the information passed on from the rxOptions.
 * @param rxOption pointer used to when triggering the "working state"
*/
void CC_Basic_report_notifyWorking(RECEIVE_OPTIONS_TYPE_EX* pRxOpt)
{
  /* Handle here the case where Basic CC has triggered a working state.
  Look in the Door Lock keypad application for an example. */
  UNUSED(pRxOpt);
}

/**
 * Prepare the data input for the TSE for any Binary Switch CC command based on the pRxOption pointer.
 * @param pRxOpt pointer used to indicate how the frame was received (if any) and what endpoints are affected
*/
void* CC_BinarySwitch_prepare_zaf_tse_data(RECEIVE_OPTIONS_TYPE_EX* pRxOpt)
{
  /* Copy the RxOption in the data and return the pointer */
  ZAF_TSE_BinarySwitchData.rxOptions = *pRxOpt;
  return &ZAF_TSE_BinarySwitchData;
}

void CC_ManufacturerSpecific_ManufacturerSpecificGet_handler(uint16_t * pManufacturerID,
                                                             uint16_t * pProductID)
{
  *pManufacturerID = APP_MANUFACTURER_ID;
  *pProductID = APP_PRODUCT_ID;
}

/*
 * This function will report a serial number in a binary format according to the specification.
 * The serial number is the chip serial number and can be verified using the Simplicity Commander.
 * The choice of reporting can be changed in accordance with the Manufacturer Specific
 * Command Class specification.
 */
void CC_ManufacturerSpecific_DeviceSpecificGet_handler(device_id_type_t * pDeviceIDType,
                                                       device_id_format_t * pDeviceIDDataFormat,
                                                       uint8_t * pDeviceIDDataLength,
                                                       uint8_t * pDeviceIDData)
{
  const size_t serial_length = zpal_get_serial_number_length();

  ASSERT(serial_length <= 0x1F); // Device ID Data Length field size is 5 bits

  *pDeviceIDType = DEVICE_ID_TYPE_SERIAL_NUMBER;
  *pDeviceIDDataFormat = DEVICE_ID_FORMAT_BINARY;
  *pDeviceIDDataLength = (uint8_t)serial_length;
  zpal_get_serial_number(pDeviceIDData);

  DPRINT("\r\nserial number: ");
  for (size_t i = 0; i < serial_length; ++i)
  {
    DPRINTF("%02x", pDeviceIDData[i]);
  }
  DPRINT("\r\n");
}

static void indicator_set_handler(uint32_t on_time_ms, uint32_t off_time_ms, uint32_t num_cycles)
{
  Board_IndicatorControl(on_time_ms, off_time_ms, num_cycles, true);
}

/*
 * The below functions should be implemented as hardware specific functions in a separate source
 * file, e.g. SwitchOnOff_hw.c.
 */
ZW_WEAK void SwitchOnOff_hw_init(void)
{

}

ZW_WEAK void SwitchOnOff_hw_binary_switch_handler(bool on)
{
  UNUSED(on);
}
