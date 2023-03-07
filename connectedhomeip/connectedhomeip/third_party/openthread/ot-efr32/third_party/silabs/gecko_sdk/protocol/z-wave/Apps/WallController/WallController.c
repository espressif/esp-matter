/**
 * Z-Wave Certified Application Wall Controller
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
/* During development of your device, you may add features using command    */
/* classes which are not already included. Remember to include relevant     */
/* classes and utilities and add them in your make file.                    */
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
#include <CC_CentralScene.h>
#include <CC_DeviceResetLocally.h>
#include <CC_Indicator.h>
#include <CC_ManufacturerSpecific.h>
#include <CC_MultiChanAssociation.h>
#include <CC_MultilevelSwitch_Control.h>
#include <CC_Supervision.h>
#include <CC_FirmwareUpdate.h>
#include <zaf_config_api.h>
#include <ZW_TransportMulticast.h>
#include "zaf_event_helper.h"
#include "zaf_job_helper.h"
#include <ZAF_Common_helper.h>
#include <ZAF_network_learn.h>
#include <ZAF_TSE.h>
#include <ota_util.h>
#include <ZAF_CmdPublisher.h>
#include "events.h"
#include <zpal_watchdog.h>
#include <zpal_misc.h>
#include <WallController_hw.h>
#include <board_indicator.h>
#include <board_init.h>
#include "zw_region_config.h"
#include "zw_build_no.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

typedef enum {
  KEY01,
  KEY02,
  KEY03,
  NUMBER_OF_KEYS
} key_id_t;


typedef enum
{
  KEY_EVENT_SHORT_PRESS,
  KEY_EVENT_HOLD,
  KEY_EVENT_UP
} key_event_t;


#define EVENT_APP_CC_NO_JOB 0xFF

/**
 * Application states. Function AppStateManager(..) includes the state
 * event machine.
 */
typedef enum
{
  STATE_APP_IDLE,
  STATE_APP_STARTUP,
  STATE_APP_AWAIT_KEYPRESS,          // Unused
  STATE_APP_LEARN_MODE,
  STATE_APP_GET_NEXT_NODE,           // Unused
  STATE_APP_INITIATE_TRANSMISSION,   // Unused
  STATE_APP_AWAIT_TRANSMIT_CALLBACK, // Unused
  STATE_APP_RESET,
  STATE_APP_TRANSMIT_DATA
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
  COMMAND_CLASS_ASSOCIATION_V2,
  COMMAND_CLASS_ASSOCIATION_GRP_INFO,
  COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2,
  COMMAND_CLASS_TRANSPORT_SERVICE_V2,
  COMMAND_CLASS_VERSION,
  COMMAND_CLASS_MANUFACTURER_SPECIFIC,
  COMMAND_CLASS_DEVICE_RESET_LOCALLY,
  COMMAND_CLASS_INDICATOR,
  COMMAND_CLASS_POWERLEVEL,
  COMMAND_CLASS_SECURITY,
  COMMAND_CLASS_SECURITY_2,
  COMMAND_CLASS_SUPERVISION,
  COMMAND_CLASS_CENTRAL_SCENE_V2,
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
  COMMAND_CLASS_ASSOCIATION_V2,
  COMMAND_CLASS_ASSOCIATION_GRP_INFO,
  COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2,
  COMMAND_CLASS_MANUFACTURER_SPECIFIC,
  COMMAND_CLASS_DEVICE_RESET_LOCALLY,
  COMMAND_CLASS_INDICATOR,
  COMMAND_CLASS_POWERLEVEL,
  COMMAND_CLASS_CENTRAL_SCENE_V2,
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
 * @var agiTableRootDeviceGroups
 * AGI table defining the groups for root device.
 */
//@ [wall_controller_agi_other_groups]
AGI_GROUP agiTableRootDeviceGroups[] = {AGITABLE_ROOTDEVICE_GROUPS};
//@ [wall_controller_agi_other_groups]

/*
 * Validate that MAX_ASSOCIATION_GROUPS (defined in config_app.h) match the
 * number of elements in AGITABLE_ROOTDEVICE_GROUPS (also defined in
 * config_app.h) plus one (for Lifeline)
 */
STATIC_ASSERT((sizeof_array(agiTableRootDeviceGroups) + 1) == MAX_ASSOCIATION_GROUPS, MAX_ASSOCIATION_GROUPS_value_is_invalid);

/**
 * Application state-machine state.
 */
static STATE_APP currentState;

typedef struct _NEXT_JOB_Q_
{
  agi_profile_t profile;
} NEXT_JOB_Q;

/**
 * Holds the latest button action.
 * The action is used to distinguish between which action to perform:
 * - Press    => Switch on/off
 * - Hold     => Start dimming
 * - Release  => Stop dimming
 */
static key_event_t keyEventGlobal;
static uint8_t centralSceneNumberHold;
static uint8_t centralSceneKeyAttributeHold;

static ESwTimerStatus centralSceneHoldTimerStatus = ESWTIMER_STATUS_FAILED;
// Timer
static SSwTimer CentralSceneHoldTimer;

static CCMLS_PRIMARY_SWITCH_T multiLevelDirection[NUMBER_OF_KEYS] = {CCMLS_PRIMARY_SWITCH_DOWN, CCMLS_PRIMARY_SWITCH_DOWN, CCMLS_PRIMARY_SWITCH_DOWN};
static uint8_t buttonStates[NUMBER_OF_KEYS] = {0, 0, 0};
static key_id_t m_button;
static NEXT_JOB_Q nextJob;

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
  uint8_t slowRefresh;
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
static s_CC_indicator_data_t ZAF_TSE_localActuationIdentifyData = {
  .rxOptions = {
    .rxStatus = 0,          /* rxStatus, verified by the TSE for Multicast */
    .securityKey = 0,       /* securityKey, ignored by the TSE */
    .sourceNode = {0,0},    /* sourceNode (nodeId, endpoint), verified against lifeline destinations by the TSE */
    .destNode = {0,0}       /* destNode (nodeId, endpoint), verified by the TSE for local endpoint */
  },
  .indicatorId = 0x50      /* Identify Indicator*/
};

static zpal_nvm_handle_t pFileSystemApplication;

static EVENT_APP pendingKeyPressEvent = EVENT_APP_CC_NO_JOB;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// No exported data.

/****************************************************************************/
/* PRIVATE FUNCTION PROTOTYPES                                              */
/****************************************************************************/
STATE_APP GetAppState(void);
void AppStateManager(EVENT_APP event);
static void ChangeState(STATE_APP newState);

static void ApplicationTask(SApplicationHandles* pAppHandles);
static void PrepareAGITransmission(uint8_t profile, key_id_t nextActiveButton);
static JOB_STATUS InitiateTransmission(void);
void ZCB_TransmitCallback(TRANSMISSION_RESULT * pTransmissionResult);
static void InitiateCentralSceneTX(uint8_t keyAttribute, uint8_t sceneNumber);
static void ZCB_CentralSceneHoldTimerCallback(SSwTimer *pTimer);
bool LoadConfiguration(void);
void SetDefaultConfiguration(void);

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
          DPRINT("Tx Status received\r\n");
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
          ZAF_EventHelperEventEnqueue(EVENT_SYSTEM_LEARNMODE_FINISHED);
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


  DPRINT("\n\n----------------------------------\n");
  DPRINT("Z-Wave Sample App: Wall Controller\n");
  DPRINTF("SDK: %d.%d.%d ZAF: %d.%d.%d.%d\n",
          SDK_VERSION_MAJOR,
          SDK_VERSION_MINOR,
          SDK_VERSION_PATCH,
          zpal_get_app_version_major(),
          zpal_get_app_version_minor(),
          zpal_get_app_version_patch(),
          ZAF_BUILD_NO);

  DPRINTF("ApplicationInit eResetReason = %d\n", eResetReason);

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
  AppTimerRegister(&CentralSceneHoldTimer, true, ZCB_CentralSceneHoldTimerCallback);

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

  WallController_hw_init();

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
  DPRINT("WallController Event processor Started\r\n");
  uint32_t iMaxTaskSleep = 10;
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

  /*
   * Initialize AGI.
   */
  //@ [WALL_CONTROLLER_AGI_INIT]
  AGI_Init();
  AGI_ResourceGroupSetup(agiTableRootDeviceGroups, sizeof_array(agiTableRootDeviceGroups), ENDPOINT_ROOT);
  //@ [WALL_CONTROLLER_AGI_INIT]

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

      if(EVENT_APP_INIT == event)
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
        break;
      }

      if(EVENT_APP_FLUSHMEM_READY == event)
      {
        AppResetNvm();
        LoadConfiguration();
      }

      if(EVENT_APP_SMARTSTART_IN_PROGRESS == event)
      {
        ChangeState(STATE_APP_LEARN_MODE);
      }

      /**************************************************************************************
       * Learn/Reset button
       **************************************************************************************
       */
      if ((EVENT_APP_BUTTON_LEARN_RESET_SHORT_PRESS == event) ||
          (EVENT_SYSTEM_LEARNMODE_START == (EVENT_SYSTEM)event))
      {
        if (EINCLUSIONSTATE_EXCLUDED != g_pAppHandles->pNetworkInfo->eInclusionState)
        {
          DPRINT("LEARN_MODE_EXCLUSION");
          ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_EXCLUSION_NWE);
        }
        else{
          DPRINT("LEARN_MODE_INCLUSION");
          ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_INCLUSION);
        }
        ChangeState(STATE_APP_LEARN_MODE);
        break;
      }

      /**************************************************************************************
       * KEY 1
       *************************************************************************************/

      if (EVENT_APP_BUTTON_KEY01_SHORT_PRESS == event)
      {
        DPRINT("\r\nK1SHORT_PRESS\r\n");
        keyEventGlobal = KEY_EVENT_SHORT_PRESS;
        centralSceneNumberHold = 1;
        centralSceneKeyAttributeHold = CENTRAL_SCENE_NOTIFICATION_KEY_ATTRIBUTES_KEY_PRESSED_1_TIME_V2;

        //ZAF_jobEnqueue(EVENT_APP_CENTRAL_SCENE_JOB);
        ZAF_JobHelperJobEnqueue(EVENT_APP_CENTRAL_SCENE_JOB);

        PrepareAGITransmission(ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY01, KEY01);
        ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
        ChangeState(STATE_APP_TRANSMIT_DATA);
      }

      if (EVENT_APP_BUTTON_KEY01_HOLD == event)
      {
        DPRINT("\r\nK1HOLD\r\n");
        keyEventGlobal = KEY_EVENT_HOLD;
        centralSceneNumberHold = 1;
        centralSceneKeyAttributeHold = CENTRAL_SCENE_NOTIFICATION_KEY_ATTRIBUTES_KEY_HELD_DOWN_V2;

        ZAF_JobHelperJobEnqueue(EVENT_APP_CENTRAL_SCENE_JOB);

        PrepareAGITransmission(ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY01, KEY01);
        ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
        ChangeState(STATE_APP_TRANSMIT_DATA);
      }

      if ((EVENT_APP_BUTTON_KEY01_RELEASE == event) &&
          (keyEventGlobal == KEY_EVENT_HOLD))
      {
        DPRINT("\r\nK1UP\r\n");
        keyEventGlobal = KEY_EVENT_UP;
        centralSceneNumberHold = 1;
        centralSceneKeyAttributeHold = CENTRAL_SCENE_NOTIFICATION_KEY_ATTRIBUTES_KEY_RELEASED_V2;

        ZAF_JobHelperJobEnqueue(EVENT_APP_CENTRAL_SCENE_JOB);

        PrepareAGITransmission(ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY01, KEY01);
        ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
        ChangeState(STATE_APP_TRANSMIT_DATA);
      }

      /**************************************************************************************
       * KEY 2
       *************************************************************************************/

      if (EVENT_APP_BUTTON_KEY02_SHORT_PRESS == event)
      {
        DPRINT("\r\nK2SHORT_PRESS\r\n");
        keyEventGlobal = KEY_EVENT_SHORT_PRESS;
        centralSceneNumberHold = 2;
        centralSceneKeyAttributeHold = CENTRAL_SCENE_NOTIFICATION_KEY_ATTRIBUTES_KEY_PRESSED_1_TIME_V2;

        ZAF_JobHelperJobEnqueue(EVENT_APP_CENTRAL_SCENE_JOB);

        PrepareAGITransmission(ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY02, KEY02);
        ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
        ChangeState(STATE_APP_TRANSMIT_DATA);
      }

      if (EVENT_APP_BUTTON_KEY02_HOLD == event)
      {
        DPRINT("\r\nK2HOLD\r\n");
        keyEventGlobal = KEY_EVENT_HOLD;
        centralSceneNumberHold = 2;
        centralSceneKeyAttributeHold = CENTRAL_SCENE_NOTIFICATION_KEY_ATTRIBUTES_KEY_HELD_DOWN_V2;

        ZAF_JobHelperJobEnqueue(EVENT_APP_CENTRAL_SCENE_JOB);

        PrepareAGITransmission(ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY02, KEY02);
        ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
        ChangeState(STATE_APP_TRANSMIT_DATA);
      }

      if ((EVENT_APP_BUTTON_KEY02_RELEASE == event) &&
          (keyEventGlobal == KEY_EVENT_HOLD))
      {
        DPRINT("\r\nK2UP\r\n");
        keyEventGlobal = KEY_EVENT_UP;
        centralSceneNumberHold = 2;
        centralSceneKeyAttributeHold = CENTRAL_SCENE_NOTIFICATION_KEY_ATTRIBUTES_KEY_RELEASED_V2;

        ZAF_JobHelperJobEnqueue(EVENT_APP_CENTRAL_SCENE_JOB);

        PrepareAGITransmission(ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY02, KEY02);
        ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
        ChangeState(STATE_APP_TRANSMIT_DATA);
      }

      /**************************************************************************************
       * KEY 3
       *************************************************************************************/

      if (EVENT_APP_BUTTON_KEY03_SHORT_PRESS == event)
      {
        DPRINT("\r\nK3SHORT_PRESS\r\n");
        keyEventGlobal = KEY_EVENT_SHORT_PRESS;
        centralSceneNumberHold = 3;
        centralSceneKeyAttributeHold = CENTRAL_SCENE_NOTIFICATION_KEY_ATTRIBUTES_KEY_PRESSED_1_TIME_V2;

        ZAF_JobHelperJobEnqueue(EVENT_APP_CENTRAL_SCENE_JOB);

        PrepareAGITransmission(ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY03, KEY03);
        ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
        ChangeState(STATE_APP_TRANSMIT_DATA);
      }

      if (EVENT_APP_BUTTON_KEY03_HOLD == event)
      {
        DPRINT("\r\nK3HOLD\r\n");
        keyEventGlobal = KEY_EVENT_HOLD;
        centralSceneNumberHold = 3;
        centralSceneKeyAttributeHold = CENTRAL_SCENE_NOTIFICATION_KEY_ATTRIBUTES_KEY_HELD_DOWN_V2;

        ZAF_JobHelperJobEnqueue(EVENT_APP_CENTRAL_SCENE_JOB);

        PrepareAGITransmission(ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY03, KEY03);
        ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
        ChangeState(STATE_APP_TRANSMIT_DATA);
      }

      if ((EVENT_APP_BUTTON_KEY03_RELEASE == event) &&
          (keyEventGlobal == KEY_EVENT_HOLD))
      {
        DPRINT("\r\nK3UP\r\n");
        keyEventGlobal = KEY_EVENT_UP;
        centralSceneNumberHold = 3;
        centralSceneKeyAttributeHold = CENTRAL_SCENE_NOTIFICATION_KEY_ATTRIBUTES_KEY_RELEASED_V2;

        ZAF_JobHelperJobEnqueue(EVENT_APP_CENTRAL_SCENE_JOB);

        PrepareAGITransmission(ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY03, KEY03);
        ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
        ChangeState(STATE_APP_TRANSMIT_DATA);
      }
      break;


    case STATE_APP_LEARN_MODE:
      if(EVENT_APP_REFRESH_MMI == event)
      {
        Board_IndicateStatus(BOARD_STATUS_LEARNMODE_ACTIVE);
      }

      if(EVENT_APP_FLUSHMEM_READY == event)
      {
        AppResetNvm();
        LoadConfiguration();
      }

      if ((EVENT_APP_BUTTON_LEARN_RESET_SHORT_PRESS == event) ||
          (EVENT_SYSTEM_LEARNMODE_STOP == (EVENT_SYSTEM)event))
      {
        DPRINT("\r\nLEARN MODE DISABLE\r\n");
        ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_DISABLE);

        //Go back to smart start if the node was never included.
        //Protocol will not commence SmartStart if the node is already included in the network.
        ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_INCLUSION_SMARTSTART);

        Board_IndicateStatus(BOARD_STATUS_IDLE);
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

      if(EVENT_SYSTEM_LEARNMODE_FINISHED == (EVENT_SYSTEM)event)
      {
        //Go back to smart start if the node was excluded.
        //Protocol will not commence SmartStart if the node is already included in the network.
        ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_INCLUSION_SMARTSTART);

        DPRINT("\r\nLEARN MODE FINISH\r\n");
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

    case STATE_APP_TRANSMIT_DATA:
      if(EVENT_APP_REFRESH_MMI == event)
      {
        // Nothing here.
      }

      if(EVENT_APP_FLUSHMEM_READY == event)
      {
        AppResetNvm();
        LoadConfiguration();
      }

      if (EVENT_APP_NEXT_EVENT_JOB == event)
      {
        uint8_t event;
        if (true == ZAF_JobHelperJobDequeue(&event))
        {
          /*
           * If we were able to dequeue an event from the job queue, let's process it right away
           * by adding it to the event queue.
           */
          ZAF_EventHelperEventEnqueue(event);
        }
        else
        {
          // If there are no more events, we'll finish the job handling.
          ZAF_EventHelperEventEnqueue(EVENT_APP_FINISH_EVENT_JOB);
        }
      }

      if (EVENT_APP_CENTRAL_SCENE_JOB == event)
      {
        if (KEY_EVENT_HOLD == keyEventGlobal)
        {
          if (ESWTIMER_STATUS_FAILED == centralSceneHoldTimerStatus)
          {
            // Start the timer only if it's not started already.
            centralSceneHoldTimerStatus = TimerStart(&CentralSceneHoldTimer, ( 0 == ApplicationData.slowRefresh ) ? 200 : 55000);
          }
        }
        else if (KEY_EVENT_UP == keyEventGlobal)
        {
          if (ESWTIMER_STATUS_FAILED != centralSceneHoldTimerStatus)
          {
            TimerStop(&CentralSceneHoldTimer);
            centralSceneHoldTimerStatus = ESWTIMER_STATUS_FAILED;
          }
        }

        InitiateCentralSceneTX(centralSceneKeyAttributeHold, centralSceneNumberHold);
      }

      if (EVENT_APP_CC_BASIC_JOB == event)
      {
        DPRINT("\r\nEVENT_APP_CC_BASIC_JOB");
        if(JOB_STATUS_SUCCESS != CC_Basic_Set_tx( &nextJob.profile, ENDPOINT_ROOT, buttonStates[m_button], ZCB_TransmitCallback))
        {
          DPRINT("\r\nEVENT_APP_CC_BASIC_JOB failed!\r\n");
          ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
        }
      }

      if(EVENT_APP_CC_SWITCH_MULTILEVEL_JOB == event)
      {
        JOB_STATUS jobStatus = InitiateTransmission();
        DPRINT("\r\nEVENT_APP_CC_SWITCH_MULTILEVEL_JOB");
        if (JOB_STATUS_SUCCESS != jobStatus)
        {
          TRANSMISSION_RESULT transmissionResult = {0, 0, TRANSMISSION_RESULT_FINISHED};
          DPRINT("\r\nCC_SWITCH_MULTILEVEL ERROR");
          ZCB_TransmitCallback(&transmissionResult);
        }
      }

      /*
       * Catch important button press/release events and save it for when the transmit is done.
       * Important events are "Key Up" and "Key Long Press", which both are button release events
       * and signify the end to an action that was initiated earlier and therefore must be matched.
       */
      if ( (EVENT_APP_BUTTON_KEY01_RELEASE == event) ||
           (EVENT_APP_BUTTON_KEY02_RELEASE == event) ||
           (EVENT_APP_BUTTON_KEY03_RELEASE == event) )
      {
        DPRINTF("\r\nReceived key press event (%d) in state STATE_APP_TRANSMIT_DATA", event);
        pendingKeyPressEvent = event;
      }

      if (EVENT_APP_FINISH_EVENT_JOB == event)
      {
        DPRINT("\r\nTransmitDone.");
        if (EVENT_APP_CC_NO_JOB != pendingKeyPressEvent)
        {
          // Button press/release event received earlier during transmit. Re-post it to the event queue now.
          DPRINTF("\r\nRe-posting pending key event (%d) in state STATE_APP_TRANSMIT_DATA)", pendingKeyPressEvent);
          ZAF_EventHelperEventEnqueue(pendingKeyPressEvent);
          pendingKeyPressEvent = EVENT_APP_CC_NO_JOB;
        }
        ChangeState(STATE_APP_IDLE);
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
  DPRINTF("\r\nState changed: %d -> %d\r\n", currentState, newState);

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

    EQueueNotifyingStatus Status = QueueNotifyingSendToBack(g_pAppHandles->pZwCommandQueue, (uint8_t*)&CommandPackage, 500);
    ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == Status);
  }
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

  ApplicationData.slowRefresh = 1;

  writeAppData(&ApplicationData);

  ZAF_Reset();

  uint32_t appVersion = zpal_get_app_version();
  zpal_nvm_write(pFileSystemApplication, ZAF_FILE_ID_APP_VERSION, &appVersion, ZAF_FILE_SIZE_APP_VERSION);
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
  ASSERT(ZPAL_STATUS_OK == status);  //Assert has been kept for debugging , can be removed from production code. This error can only be caused by some internal flash HW failure

  /* Apparently there is no valid configuration in file system, so load */
  /* default values and save them to file system. */
  SetDefaultConfiguration();
}

/**
 * @brief Prepares the transmission of commands stored in the AGI table.
 *
 * @param profile The profile key.
 * @param srcEndpoint The source endpoint.
 */
static void
PrepareAGITransmission(
        uint8_t profile,
        key_id_t nextActiveButton)
{
  DPRINTF("\r\nPrepareAGITransmission %d", keyEventGlobal);

  nextJob.profile.profile_MS = ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL;
  nextJob.profile.profile_LS = profile;

  m_button = nextActiveButton;
  if (KEY_EVENT_SHORT_PRESS == keyEventGlobal)
  {
    ZAF_JobHelperJobEnqueue(EVENT_APP_CC_BASIC_JOB);
    if (0xFF == buttonStates[m_button])
    {
      /*
       * If button is on, turn device off.
       */
      buttonStates[m_button] = 0x00;
      DPRINT("\r\nBasic OFF");
    }
    else
    {
      /*
       * If button is off, turn device on.
       */
      buttonStates[m_button] = 0xFF;
      DPRINT("\r\nBasic ON");
    }
  }
  else if (KEY_EVENT_HOLD == keyEventGlobal)
  {
    DPRINT("\r\npre EVENT_APP_CC_SWITCH_MULTILEVEL_JOB");
    ZAF_JobHelperJobEnqueue(EVENT_APP_CC_SWITCH_MULTILEVEL_JOB);
    if (CCMLS_PRIMARY_SWITCH_UP == multiLevelDirection[m_button])
    {
      multiLevelDirection[m_button] = CCMLS_PRIMARY_SWITCH_DOWN;
    }
    else
    {
      multiLevelDirection[m_button] = CCMLS_PRIMARY_SWITCH_UP;
    }
  }
  else if (KEY_EVENT_UP == keyEventGlobal)
  {
    DPRINT("\r\npre BUTTON_UP EVENT_APP_CC_SWITCH_MULTILEVEL_JOB");
    ZAF_JobHelperJobEnqueue(EVENT_APP_CC_SWITCH_MULTILEVEL_JOB);
  }
}

/**
 * Initiates a Central Scene Notification to the lifeline.
 * We don't care about the result, since we have to proceed no matter what.
 * Therefore a callback function is called in any case.
 * @param keyAttribute The key attribute in action.
 * @param sceneNumber The scene in action.
 */
static void InitiateCentralSceneTX(uint8_t keyAttribute, uint8_t sceneNumber)
{
  agi_profile_t lifelineProfile = {
      ASSOCIATION_GROUP_INFO_REPORT_PROFILE_GENERAL_NA_V2,
      ASSOCIATION_GROUP_INFO_REPORT_PROFILE_GENERAL_LIFELINE
  };

  JOB_STATUS jobStatus = CommandClassCentralSceneNotificationTransmit(
          &lifelineProfile,
          ENDPOINT_ROOT,
          keyAttribute,
          sceneNumber,
          ZCB_TransmitCallback);

  if (JOB_STATUS_SUCCESS != jobStatus)
  {
    TRANSMISSION_RESULT transmissionResult;
    transmissionResult.nodeId = 0;
    transmissionResult.status = TRANSMIT_COMPLETE_FAIL;
    transmissionResult.isFinished = TRANSMISSION_RESULT_FINISHED;
    DPRINT("\r\nLL failure");
    ZCB_TransmitCallback(&transmissionResult);
  }
  else
  {
    DPRINT("\r\nLL success");
  }
}


/**
 * @brief Processes the transmission to related nodes.
 * @return Status of the job.
 */
static JOB_STATUS
InitiateTransmission(void)
{
  DPRINTF("\r\n### ITrans %d", keyEventGlobal);
  if (KEY_EVENT_HOLD == keyEventGlobal)
  {
    DPRINT("\r\n### Multilevel TX.Change");

    return CmdClassMultilevelSwitchStartLevelChange(
                &nextJob.profile,
                ENDPOINT_ROOT,
                ZCB_TransmitCallback,
                multiLevelDirection[m_button],
                CCMLS_IGNORE_START_LEVEL_TRUE,
                CCMLS_SECONDARY_SWITCH_NO_INC_DEC,
                0,
                2,
                0);
  }
  else if (KEY_EVENT_UP == keyEventGlobal)
  {
    DPRINT("\r\n### Multilevel Stop level change");
    return CmdClassMultilevelSwitchStopLevelChange(
                &nextJob.profile,
                ENDPOINT_ROOT,
                ZCB_TransmitCallback);
  }
  return JOB_STATUS_BUSY;
}

/**
 * @brief Callback function setting the application state.
 * @details Sets the application state when done transmitting.
 * @param pTransmissionResult Result of each transmission.
 */
void
ZCB_TransmitCallback(TRANSMISSION_RESULT * pTransmissionResult)
{
  DPRINTF("\r\nTX CB for N %d: %d", pTransmissionResult->nodeId, pTransmissionResult->status);
  if (EVENT_APP_CC_NO_JOB != pendingKeyPressEvent) {
    ZAF_EventHelperEventEnqueue(EVENT_APP_FINISH_EVENT_JOB);
  } else if (TRANSMISSION_RESULT_FINISHED == pTransmissionResult->isFinished) {
    ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
  }
}

/**
 * @brief See description for function prototype in CC_CentralScene.h.
 */
uint8_t getAppCentralSceneReportData(ZW_CENTRAL_SCENE_SUPPORTED_REPORT_1BYTE_V3_FRAME * pData)
{
  pData->supportedScenes = 3; // Number of buttons
  pData->properties1 = (1 << 1) | 1; // 1 bit mask byte & All keys are identical.
  pData->variantgroup1.supportedKeyAttributesForScene1 = 0x07; // 0b00000111.
  return 1;
}

/**
 * @brief See description for function prototype in CC_CentralScene.h.
 */
void getAppCentralSceneConfiguration(central_scene_configuration_t * pConfiguration)
{
  pConfiguration->slowRefresh = readAppData().slowRefresh;
}

/**
 * @brief See description for function prototype in CC_CentralScene.h.
 */
e_cmd_handler_return_code_t
setAppCentralSceneConfiguration(central_scene_configuration_t * pConfiguration)
{
  ApplicationData.slowRefresh = pConfiguration->slowRefresh;
  writeAppData(&ApplicationData);

  // just return success
  return E_CMD_HANDLER_RETURN_CODE_HANDLED;
}

void
ZCB_CentralSceneHoldTimerCallback(SSwTimer *pTimer)
{
  ZAF_JobHelperJobEnqueue(EVENT_APP_CENTRAL_SCENE_JOB);
  if (STATE_APP_TRANSMIT_DATA != currentState)
  {
    ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
    ChangeState(STATE_APP_TRANSMIT_DATA);
  }
  UNUSED(pTimer);
}

/**
 * @brief Reads application data from file system.
 */
SApplicationData
readAppData(void)
{
  SApplicationData AppData;

  const zpal_status_t status = zpal_nvm_read(pFileSystemApplication, FILE_ID_APPLICATIONDATA, &AppData, sizeof(SApplicationData));
  ASSERT(ZPAL_STATUS_OK == status);//Assert has been kept for debugging , can be removed from production code. This error hard to occur when a corresponing write is successfull
                                    //Can only happended in case of some hardware failure

  return AppData;
}

/**
 * @brief Writes application data to file system.
 */
void writeAppData(const SApplicationData* pAppData)
{
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
  // Invalid Firmware number
  return 0;
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
 * file, e.g. WallController_hw.c.
 */
ZW_WEAK void WallController_hw_init(void)
{

}
