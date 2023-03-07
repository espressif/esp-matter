/**
 * Z-Wave Certified Application Power Strip
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include "zw_config_rf.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
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
#include <CC_BinarySwitch.h>
#include <CC_DeviceResetLocally.h>
#include <CC_Indicator.h>
#include <CC_MultiChan.h>
#include <CC_MultiChanAssociation.h>
#include <CC_MultilevelSwitch_Support.h>
#include <CC_Notification.h>
#include <CC_Supervision.h>
#include <CC_FirmwareUpdate.h>
#include <CC_ManufacturerSpecific.h>

#include <notification.h>

#include "zaf_event_helper.h"
#include "zaf_job_helper.h"
#include <ZAF_Common_helper.h>
#include <ZAF_network_learn.h>
#include "ZAF_TSE.h"
#include <ota_util.h>
#include <ZAF_CmdPublisher.h>
#include "events.h"
#include <zpal_watchdog.h>
#include <zpal_misc.h>
#include <PowerStrip_hw.h>
#include <board_indicator.h>
#include <board_init.h>

#include "zw_region_config.h"

#include "zw_build_no.h"
#include <zaf_config_api.h>

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

typedef enum _STATE_APP_
{
  STATE_APP_STARTUP,
  STATE_APP_IDLE,
  STATE_APP_LEARN_MODE,
  STATE_APP_RESET,
  STATE_APP_TRANSMIT_DATA
} STATE_APP;

#define SWITCH_ON           (0x01)
#define SWITCH_OFF          (0x00)
#define BIN_SWITCH_1        (ENDPOINT_1 - 1)

/**
 * The default dimming duration is used if a received CC Multilevel Switch Set or Start Level Change
 * command contains no duration. This typically applies for older devices that do not support
 * the latest Multilevel Switch command class.
 */
#define CC_MULTILEVEL_SWITCH_DEFAULT_DIMMING_DURATION 1 // 1 second

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
  COMMAND_CLASS_NOTIFICATION_V3,
  COMMAND_CLASS_TRANSPORT_SERVICE_V2,
  COMMAND_CLASS_VERSION,
  COMMAND_CLASS_MANUFACTURER_SPECIFIC,
  COMMAND_CLASS_DEVICE_RESET_LOCALLY,
  COMMAND_CLASS_INDICATOR,
  COMMAND_CLASS_POWERLEVEL,
  COMMAND_CLASS_SECURITY,
  COMMAND_CLASS_SECURITY_2,
  COMMAND_CLASS_MULTI_CHANNEL_V4,
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
  COMMAND_CLASS_SWITCH_BINARY_V2,
  COMMAND_CLASS_ASSOCIATION,
  COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2,
  COMMAND_CLASS_ASSOCIATION_GRP_INFO,
  COMMAND_CLASS_NOTIFICATION_V3,
  COMMAND_CLASS_VERSION,
  COMMAND_CLASS_MANUFACTURER_SPECIFIC,
  COMMAND_CLASS_DEVICE_RESET_LOCALLY,
  COMMAND_CLASS_INDICATOR,
  COMMAND_CLASS_POWERLEVEL,
  COMMAND_CLASS_MULTI_CHANNEL_V4,
  COMMAND_CLASS_FIRMWARE_UPDATE_MD_V5
};

/**
 * Structure includes application node information list's and device type.
 */
static app_node_information_t m_AppNIF =
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

static SAppNodeInfo_t AppNodeInfo =
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

/****************************************************************************/
/* Endpoint 1 command class lists                                           */
/****************************************************************************/

/**
 * Please see the description of app_node_information_t.
 */
static uint8_t ep1_noSec_InclNonSecure[] =
{
  COMMAND_CLASS_ZWAVEPLUS_INFO,
  COMMAND_CLASS_SWITCH_BINARY_V2,
  COMMAND_CLASS_ASSOCIATION,
  COMMAND_CLASS_ASSOCIATION_GRP_INFO,
  COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2,
  COMMAND_CLASS_NOTIFICATION_V3,
  COMMAND_CLASS_SUPERVISION,
  COMMAND_CLASS_SECURITY,
  COMMAND_CLASS_SECURITY_2
};

/**
 * Please see the description of app_node_information_t.
 */
static uint8_t ep1_noSec_InclSecure[] =
{
  COMMAND_CLASS_ZWAVEPLUS_INFO,
  COMMAND_CLASS_SUPERVISION,
  COMMAND_CLASS_SECURITY,
  COMMAND_CLASS_SECURITY_2
};

/**
 * Please see the description of app_node_information_t.
 */
static uint8_t ep1_sec_InclSecure[] =
{
  COMMAND_CLASS_SWITCH_BINARY_V2,
  COMMAND_CLASS_ASSOCIATION,
  COMMAND_CLASS_ASSOCIATION_GRP_INFO,
  COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2,
  COMMAND_CLASS_NOTIFICATION_V3
};

/****************************************************************************/
/* Endpoint 2 command class lists                                           */
/****************************************************************************/

/**
 * Please see the description of app_node_information_t.
 */
static uint8_t ep2_noSec_InclNonSecure[] =
{
  COMMAND_CLASS_ZWAVEPLUS_INFO,
  COMMAND_CLASS_SWITCH_MULTILEVEL,
  COMMAND_CLASS_ASSOCIATION,
  COMMAND_CLASS_ASSOCIATION_GRP_INFO,
  COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2,
  COMMAND_CLASS_NOTIFICATION_V3,
  COMMAND_CLASS_SUPERVISION,
  COMMAND_CLASS_SECURITY,
  COMMAND_CLASS_SECURITY_2
};

/**
 * Please see the description of app_node_information_t.
 */
static uint8_t ep2_noSec_InclSecure[] =
{
  COMMAND_CLASS_ZWAVEPLUS_INFO,
  COMMAND_CLASS_SUPERVISION,
  COMMAND_CLASS_SECURITY,
  COMMAND_CLASS_SECURITY_2
};

/**
 * Please see the description of app_node_information_t.
 */
static uint8_t ep2_sec_InclSecure[] =
{
  COMMAND_CLASS_SWITCH_MULTILEVEL,
  COMMAND_CLASS_ASSOCIATION,
  COMMAND_CLASS_ASSOCIATION_GRP_INFO,
  COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2,
  COMMAND_CLASS_NOTIFICATION_V3
};

static EP_NIF endpointsNIF[NUMBER_OF_ENDPOINTS] =
{
 /*
  * Endpoint 1
  */
  {
    GENERIC_TYPE_SWITCH_BINARY,
    SPECIFIC_TYPE_NOT_USED,
    {
      {ep1_noSec_InclNonSecure, sizeof(ep1_noSec_InclNonSecure)},
      {{ep1_noSec_InclSecure, sizeof(ep1_noSec_InclSecure)}, {ep1_sec_InclSecure, sizeof(ep1_sec_InclSecure)}}
    }
  },
  /*
   * Endpoint 2
   */
  {
    GENERIC_TYPE_SWITCH_MULTILEVEL,
    SPECIFIC_TYPE_NOT_USED,
    {
      {ep2_noSec_InclNonSecure, sizeof(ep2_noSec_InclNonSecure)},
      {{ep2_noSec_InclSecure, sizeof(ep2_noSec_InclSecure)}, {ep2_sec_InclSecure, sizeof(ep2_sec_InclSecure)}}
    }
  }
};

static EP_FUNCTIONALITY_DATA endPointFunctionality =
{
  {
    NUMBER_OF_INDIVIDUAL_ENDPOINTS,     /**< nbrIndividualEndpoints 7 bit*/
    RES_ZERO,                           /**< resIndZeorBit 1 bit*/
    NUMBER_OF_AGGREGATED_ENDPOINTS,     /**< nbrAggregatedEndpoints 7 bit*/
    RES_ZERO,                           /**< resAggZeorBit 1 bit*/
    RES_ZERO,                           /**< resZero 6 bit*/
    ENDPOINT_IDENTICAL_DEVICE_CLASS_NO,/**< identical 1 bit*/
    ENDPOINT_DYNAMIC_NO                /**< dynamic 1 bit*/
  }
};

/**
 * Setup AGI root device groups table from config_app.h
 */
static const AGI_GROUP agiTableRootDeviceGroups[] = {AGITABLE_ROOTDEVICE_GROUPS};
static const AGI_GROUP agiTableEndpoint1Groups[] = {AGITABLE_ENDPOINT_1_GROUPS};
static const AGI_GROUP agiTableEndpoint2Groups[] = {AGITABLE_ENDPOINT_2_GROUPS};

static const AGI_PROFILE endpointProfile =
{
  ASSOCIATION_GROUP_INFO_REPORT_PROFILE_NOTIFICATION,
  NOTIFICATION_REPORT_POWER_MANAGEMENT_V4
};

#define ENDPOINT_PROFILE  &endpointProfile


/**
 * Application state-machine state.
 */
static STATE_APP currentState = STATE_APP_IDLE;

typedef struct _BIN_SWITCH_{
  uint8_t switchStatus;
}BIN_SWITCH;

typedef struct  _MULTILEVEL_SWITCH_{
  uint8_t switchStatus;
  uint8_t level;
}MULTILEVEL_SWITCH;

typedef struct _POWER_STRIP_{
  BIN_SWITCH binSwitch;
  MULTILEVEL_SWITCH dimmer;
}POWER_STRIP;

POWER_STRIP powerStrip;

static ESwTimerStatus notificationOverLoadTimerStatus = ESWTIMER_STATUS_FAILED;
// Timer
static SSwTimer NotificationTimer;

static bool notificationOverLoadActiveState =  false;
static uint8_t notificationOverLoadendpoint = 0;

static cc_multilevel_switch_t switches[] = {
                                            {
                                             .endpoint = 2
                                            }
};

uint8_t supportedEvents = NOTIFICATION_EVENT_POWER_MANAGEMENT_OVERLOADED_DETECTED;

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

#define APP_EVENT_QUEUE_SIZE 5

/**
 * The following four variables are used for the application event queue.
 */
static SQueueNotifying m_AppEventNotifyingQueue;
static StaticQueue_t m_AppEventQueueObject;
static EVENT_APP eventQueueStorage[APP_EVENT_QUEUE_SIZE];
static QueueHandle_t m_AppEventQueue;

void AppResetNvm(void);

/* True Status Engine (TSE) variables */
/* TSE simulated RX option for local change addressed to End Point 1 */
static RECEIVE_OPTIONS_TYPE_EX zaf_tse_local_ep1_actuation = {
    .rxStatus = 0,        /* rxStatus, verified by the TSE for Multicast */
    .securityKey = 0,     /* securityKey, ignored by the TSE */
    .sourceNode = {0,0},  /* sourceNode (nodeId, endpoint), verified against lifeline destinations by the TSE */
    .destNode = {0,1}     /* destNode (nodeId, endpoint), verified by the TSE for local endpoint */
};

/* Indicate here which End Points (NOT including root device) support each command class */
static const uint8_t endpoints_Supporting_BinarySwitch_CC[] = {1};
s_CC_binarySwitch_data_t ZAF_TSE_BinarySwitchData[sizeof_array(endpoints_Supporting_BinarySwitch_CC)];
static s_CC_indicator_data_t ZAF_TSE_localActuationIdentifyData = {
  .rxOptions = {
    .rxStatus = 0,          /* rxStatus, verified by the TSE for Multicast */
    .securityKey = 0,       /* securityKey, ignored by the TSE */
    .sourceNode = {0,0},    /* sourceNode (nodeId, endpoint), verified against lifeline destinations by the TSE */
    .destNode = {0,0}       /* destNode (nodeId, endpoint), verified by the TSE for local endpoint */
  },
  .indicatorId = 0x50      /* Identify Indicator*/
};

s_CC_notification_data_t ZAF_TSE_NotificationData;

static zpal_nvm_handle_t pFileSystemApplication;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/
STATE_APP GetAppState(void);
void AppStateManager(EVENT_APP event);
static void ChangeState(STATE_APP newState);

uint8_t CC_BinarySwitch_getEndpointIndex(uint8_t endpoint);

void ApplicationTask(SApplicationHandles* pAppHandles);

bool LoadConfiguration(void);
void SetDefaultConfiguration(void);
void ZCB_JobStatus(TRANSMISSION_RESULT * pTransmissionResult);
void ZCB_NotificationTimerCallback(SSwTimer *pTimer);
static void ToggleSwitch(uint8_t switchID);
static void notificationToggle(void);
static void cc_multilevel_handler(cc_multilevel_switch_t * p_switch);

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
          DPRINTF("Tx Status received: %u\r\n", pTxStatus->TxStatus);
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
          if((EINCLUSIONSTATE_EXCLUDED == ZAF_GetInclusionState()))
          {
            if(ESWTIMER_STATUS_FAILED != notificationOverLoadTimerStatus)
            {
              TimerStop(&NotificationTimer);
              notificationOverLoadTimerStatus = ESWTIMER_STATUS_FAILED;
            }
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

static void EventHandlerApp(void)
{
  DPRINT("Got event!\r\n");

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

/*
 * See description for function prototype in ZW_basis_api.h.
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


  DPRINT("\n\n------------------------------\n");
  DPRINT("Z-Wave Sample App: Power Strip\n");
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

  cc_multilevel_switch_init(switches,
                            sizeof_array(switches),
                            CC_MULTILEVEL_SWITCH_DEFAULT_DIMMING_DURATION,
                            cc_multilevel_handler);

  powerStrip.binSwitch.switchStatus = SWITCH_OFF;

  Transport_AddEndpointSupport( &endPointFunctionality, endpointsNIF, NUMBER_OF_ENDPOINTS);

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
 *
 * The function must be treated as a static function, but is left non-static for the purpose
 * of unit testing.
 */
void
ApplicationTask(SApplicationHandles* pAppHandles)
{
  // Init
  DPRINT("Enabling watchdog\n");
  zpal_enable_watchdog(true);

  g_AppTaskHandle = xTaskGetCurrentTaskHandle();
  g_pAppHandles = pAppHandles;
  AppTimerSetReceiverTask(g_AppTaskHandle);
  AppTimerRegister(&NotificationTimer, true, ZCB_NotificationTimerCallback);

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

  PowerStrip_hw_init();

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
  DPRINT("PowerStrip Event processor Started\r\n");
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

/*
 * The function must be treated as a static function, but is left non-static for the purpose
 * of unit testing.
 */
void doRemainingInitialization()
{
  InitNotification(pFileSystemApplication);  // This is needed by LoadConfiguration() in case of error.

  /* Load the application settings from NVM file system */
  bool filesExist = LoadConfiguration();
  
    // Initialize AGI and set up groups.
  AGI_Init();

  // Root device
  AGI_ResourceGroupSetup(agiTableRootDeviceGroups,
                         sizeof_array(agiTableRootDeviceGroups),
                         ENDPOINT_ROOT);
  // Endpoint 1
  AGI_ResourceGroupSetup(agiTableEndpoint1Groups,
                         sizeof_array(agiTableEndpoint1Groups),
                         ENDPOINT_1);

  // Endpoint 2
  AGI_ResourceGroupSetup(agiTableEndpoint2Groups,
                         sizeof_array(agiTableEndpoint2Groups),
                         ENDPOINT_2);

  {
    AddNotification(ENDPOINT_PROFILE,
                    NOTIFICATION_TYPE_POWER_MANAGEMENT,
                    &supportedEvents,
                    1,
                    false,
                    ENDPOINT_1,
                    NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED,
                    filesExist);

    AddNotification(ENDPOINT_PROFILE,
                    NOTIFICATION_TYPE_POWER_MANAGEMENT,
                    &supportedEvents,
                    1,
                    false,
                    ENDPOINT_2,
                    NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED,
                    filesExist);
  }

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

      if (EVENT_APP_REFRESH_MMI == event)
      {
        Board_IndicateStatus(BOARD_STATUS_IDLE);
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

      if ((EVENT_APP_BUTTON_LEARN_RESET_SHORT_PRESS == event) ||
          (EVENT_SYSTEM_LEARNMODE_START == (EVENT_SYSTEM)event))
      {
        DPRINT("APP_BUTTON_LEARN_RESET SHORT_PRESS\n");

        if (EINCLUSIONSTATE_EXCLUDED != g_pAppHandles->pNetworkInfo->eInclusionState)
        {
          DPRINT("LEARN_MODE_EXCLUSION\n");
          ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_EXCLUSION_NWE);
        }
        else
        {
          DPRINT("LEARN_MODE_INCLUSION\n");
          ZAF_setNetworkLearnMode(E_NETWORK_LEARN_MODE_INCLUSION);
        }
        ChangeState(STATE_APP_LEARN_MODE);
      }

      if (EVENT_APP_BUTTON_OUTLET1_TOGGLE == event)
      {
        DPRINT("+Toggle SW 1");
        ToggleSwitch(BIN_SWITCH_1);
      }

      if (EVENT_APP_BUTTON_OUTLET2_DIMMER_SHORT_PRESS == event)
      {
        DPRINT("\nDimmer press");
        cc_multilevel_switch_stop_level_change(&switches[0]);

        if (cc_multilevel_switch_get_current_value(&switches[0]) > 0) {
          DPRINT("\nTurn off");
          cc_multilevel_switch_set(&switches[0], 0, 0); // Turn it off
        } else {
          uint8_t value = cc_multilevel_switch_get_last_on_value(&switches[0]);
          DPRINTF("\nTurn on: %u", value);
          value = (0 == value ? 99 : value);
          cc_multilevel_switch_set(&switches[0], value, 0); // Set to last on value.
        }
      }

      if (EVENT_APP_BUTTON_OUTLET2_DIMMER_RELEASE == event)
      {
        DPRINT("\nDimmer up");
        cc_multilevel_switch_stop_level_change(&switches[0]);
      }

      if (EVENT_APP_BUTTON_OUTLET2_DIMMER_HOLD == event)
      {
        DPRINT("\nDimmer hold");
        static int32_t direction = 1; // 1 = increase the value because we always boot with off.
        cc_multilevel_switch_start_level_change(&switches[0], (direction > 0 ? false : true), true, 0, 10);
        if (-1 == direction) {
          direction = 1;
        } else if (1 == direction) {
          direction = -1;
        }
        DPRINTF("\ndir: %d", direction);
      }

      if (EVENT_APP_BUTTON_NOTIFICATION_TOGGLE == event)
      {
        /*
         * Pressing the NOTIFICATION_TOGGLE_BTN key will toggle the overload timer.
         * This timer will transmit a notification every 30th second.
         */
        notificationToggle();
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

      if(EVENT_APP_FLUSHMEM_READY == event)
      {
        AppResetNvm();
        LoadConfiguration();
      }

      if (EVENT_APP_FINISH_EVENT_JOB == event)
      {
        ChangeState(STATE_APP_IDLE);
      }

      if (EVENT_APP_BUTTON_NOTIFICATION_TOGGLE == event)
      {
        /*
         * Short press on notification key will toggle the overload timer.
         * This timer will transmit a notification every 30th second.
         */
        notificationToggle();
      }
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

  DefaultNotificationStatus(NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED);

  ZAF_Reset();

  cc_multilevel_switch_init(switches,
                            sizeof_array(switches),
                            CC_MULTILEVEL_SWITCH_DEFAULT_DIMMING_DURATION,
                            cc_multilevel_handler);

  appBinarySwitchSet(CMD_CLASS_BIN_OFF, 0, 0);

  uint32_t appVersion = zpal_get_app_version();
  const zpal_status_t status = zpal_nvm_write(pFileSystemApplication, ZAF_FILE_ID_APP_VERSION, &appVersion, ZAF_FILE_SIZE_APP_VERSION);
  ASSERT(ZPAL_STATUS_OK == status); //Assert has been kept for debugging , can be removed from production code if this error can only be caused by some internal flash HW failure
}

/**
 * @brief This function loads the application settings from non-volatile memory.
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

  /* Apparently there is no valid configuration in the NVM file system, so load */
  /* default values and save them. */
  SetDefaultConfiguration();
}

/*
 * This function will be invoked when a Basic Get command is received.
 *
 * See prototype for more information.
 */
uint8_t CC_Basic_GetCurrentValue_handler(uint8_t endpoint)
{
  if (2 == endpoint) {
    return cc_multilevel_switch_get_current_value_handler(endpoint);
  }
  return appBinarySwitchGetCurrentValue(endpoint);
}

/*
 * This function will be invoked when a Basic Get command is received.
 *
 * See prototype for more information.
 */
uint8_t CC_Basic_GetTargetValue_handler(uint8_t endpoint)
{
  if (2 == endpoint) {
    return cc_multilevel_switch_get_target_value_handler(endpoint);
  }
  return appBinarySwitchGetTargetValue(endpoint);
}

/*
 * This function will be invoked when a Basic Get command is received.
 *
 * See prototype for more information.
 */
uint8_t CC_Basic_GetDuration_handler(uint8_t endpoint)
{
  if (2 == endpoint) {
    return cc_multilevel_switch_get_duration_handler(endpoint);
  }
  return appBinarySwitchGetDuration(endpoint);
}

/*
 * This function will be invoked when a Binary Switch Get command is received by the root
 * device or by the endpoint that supports the Binary Switch CC.
 *
 * See prototype for more information.
 */
CMD_CLASS_BIN_SW_VAL appBinarySwitchGetCurrentValue(uint8_t endpoint)
{
  UNUSED(endpoint);

  if (powerStrip.binSwitch.switchStatus & SWITCH_ON)
  {
    return CMD_CLASS_BIN_ON;
  }
  else
  {
    return CMD_CLASS_BIN_OFF;
  }
}

/*
 * This function will be invoked when a Binary Switch Get command is received by the root
 * device or by the endpoint that supports the Binary Switch CC.
 *
 * See prototype for more information.
 */
CMD_CLASS_BIN_SW_VAL appBinarySwitchGetTargetValue(uint8_t endpoint)
{
  /* Always report target value equal to current value  */
  return appBinarySwitchGetCurrentValue(endpoint);
}

/*
 * This function will be invoked when a Binary Switch Get command is received by the root
 * device or by the endpoint that supports the Binary Switch CC.
 *
 * See prototype for more information.
 */
uint8_t appBinarySwitchGetDuration(uint8_t endpoint)
{
  UNUSED(endpoint);

  /* Always report instant transition; the only reasonable transition for a binary switch. */
  return 0;
}

/*
 * This function will be invoked when a Binary Switch Set command is received by the root
 * device or by the endpoint that supports the Binary Switch CC.
 *
 * See prototype for more information.
 */
e_cmd_handler_return_code_t appBinarySwitchSet(
  CMD_CLASS_BIN_SW_VAL val,
  uint8_t duration,
  uint8_t endpoint
)
{
  UNUSED(duration); /* Ignore duration - for a binary switch only instant transitions make sense */
  UNUSED(endpoint);

  DPRINTF("\r\nApp: handleApplBinarySwitchSet %u %u\r\n", endpoint, val);

  if(CMD_CLASS_BIN_OFF == val)
  {
    /*
     * Ignore bitfield conversion warnings as there is no good solution other than stop
     * using bitfields.
     */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    powerStrip.binSwitch.switchStatus &= ~SWITCH_ON;
#pragma GCC diagnostic pop
    PowerStrip_hw_binary_switch_handler(false);
  }
  else if(CMD_CLASS_BIN_ON == val)
  {
    powerStrip.binSwitch.switchStatus |= SWITCH_ON;
    PowerStrip_hw_binary_switch_handler(true);
  }

  return E_CMD_HANDLER_RETURN_CODE_HANDLED;
}

/*
 * See prototype.
 */
uint8_t appBinarySwitchGetFactoryDefaultDuration(uint8_t endpoint)
{
  UNUSED(endpoint);
  return 0;
}

static void cc_multilevel_handler(cc_multilevel_switch_t * p_switch)
{
#ifdef DEBUGPRINT
  uint8_t level = ZAF_Actuator_GetCurrentValue(&p_switch->actuator);
  DPRINTF("\nValue: %u", level);
#endif

  PowerStrip_hw_multilevel_switch_handler(p_switch);
}

void
ZCB_JobStatus(TRANSMISSION_RESULT * pTransmissionResult)
{
  DPRINTF("\r\nTX CB for N %u", pTransmissionResult->nodeId);

  if (TRANSMISSION_RESULT_FINISHED == pTransmissionResult->isFinished)
  {
    ZAF_EventHelperEventEnqueue(EVENT_APP_FINISH_EVENT_JOB);
  }
}

/**
 * Prepare the data input for the TSE for Notification events.
*/
void* CC_Notification_prepare_zaf_tse_data(notification_type_t type, uint8_t event, uint8_t endpoint)
{
  static RECEIVE_OPTIONS_TYPE_EX pRxOpt = {
      .rxStatus = 0,        /* rxStatus, verified by the TSE for Multicast */
      .securityKey = 0,     /* securityKey, ignored by the TSE */
      .sourceNode = {0,0},  /* sourceNode (nodeId, endpoint), verified against lifeline destinations by the TSE */
      .destNode = {0,0}     /* destNode (nodeId, endpoint), verified by the TSE for local endpoint */
  };

  /* Sanity check the endpoint value is valid */
  if (endpoint > NUMBER_OF_ENDPOINTS)
  {
    return NULL;
  }

  /*
   * Ignore bitfield conversion warnings as there is no good solution other than stop
   * using bitfields.
   */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
  /* Specify which endpoint triggered this state change  */
  pRxOpt.destNode.endpoint = endpoint;
#pragma GCC diagnostic pop
  /* And store the data and return the pointer */
  ZAF_TSE_NotificationData.rxOptions = pRxOpt;
  ZAF_TSE_NotificationData.notificationType = type;
  ZAF_TSE_NotificationData.notificationEvent = event;
  ZAF_TSE_NotificationData.sourceEndpoint = endpoint;

  return &ZAF_TSE_NotificationData;
}

/**
 * @brief Called every time the notification timer triggers.
 */
void
ZCB_NotificationTimerCallback(SSwTimer *pTimer)
{
  UNUSED(pTimer);

  JOB_STATUS jobStatus;
  void       *pData;

  DPRINT("\r\nNtfctn timer");

  if (false == notificationOverLoadActiveState)
  {
    /*Find new endpoint to send notification*/
    notificationOverLoadendpoint++;
    if (notificationOverLoadendpoint > NUMBER_OF_ENDPOINTS)
    {
      notificationOverLoadendpoint = 1;
    }

    DPRINTF("\r\nNtfctn enable EP %u", notificationOverLoadendpoint);

    notificationOverLoadActiveState = true;
    NotificationEventTrigger(
        ENDPOINT_PROFILE,
        NOTIFICATION_TYPE_POWER_MANAGEMENT,
        NOTIFICATION_EVENT_POWER_MANAGEMENT_OVERLOADED_DETECTED,
        NULL,
        0,
        notificationOverLoadendpoint);

    /* Tell the lifeline destinations that an Endpoint state has been modified */
    pData = CC_Notification_prepare_zaf_tse_data(NOTIFICATION_TYPE_POWER_MANAGEMENT, NOTIFICATION_EVENT_POWER_MANAGEMENT_OVERLOADED_DETECTED, notificationOverLoadendpoint);
  }
  else
  {
    DPRINTF("\r\nNtfctn disable EP %u", notificationOverLoadendpoint);

    notificationOverLoadActiveState = false;
    NotificationEventTrigger(
        ENDPOINT_PROFILE,
        NOTIFICATION_TYPE_POWER_MANAGEMENT,
        NOTIFICATION_EVENT_POWER_MANAGEMENT_NO_EVENT,
        &supportedEvents,
        1,
        notificationOverLoadendpoint);

    /* Tell the lifeline destinations that an Endpoint state has been modified */
    pData = CC_Notification_prepare_zaf_tse_data(NOTIFICATION_TYPE_POWER_MANAGEMENT, NOTIFICATION_EVENT_POWER_MANAGEMENT_NO_EVENT, notificationOverLoadendpoint);
  }

  //@ [NOTIFICATION_TRANSMIT]
  jobStatus = UnsolicitedNotificationAction(
      ENDPOINT_PROFILE,
      notificationOverLoadendpoint,
      ZCB_JobStatus);
  //@ [NOTIFICATION_TRANSMIT]

  if (JOB_STATUS_SUCCESS != jobStatus)
  {
    TRANSMISSION_RESULT transmissionResult;

    DPRINTF("\r\nX%u", jobStatus);

    transmissionResult.status = false;
    transmissionResult.nodeId = 0;
    transmissionResult.isFinished = TRANSMISSION_RESULT_FINISHED;

    ZCB_JobStatus(&transmissionResult);
  }

  NOTIFICATION_STATUS notification_enable = CmdClassNotificationGetNotificationStatus(NOTIFICATION_TYPE_POWER_MANAGEMENT, notificationOverLoadendpoint);
  if (pData && (NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED == notification_enable))
  {
    /* Trigger TSE */
    ZAF_TSE_Trigger(CC_Notification_report_stx, pData, true);
    ChangeState(STATE_APP_TRANSMIT_DATA);
  }

}

static void ToggleSwitch(uint8_t switchID)
{
  switch (switchID)
  {
    case BIN_SWITCH_1:
      if (powerStrip.binSwitch.switchStatus & SWITCH_ON)
      {
        DPRINT(" OFF\n"); /* Continuation of DPRINT in AppStateManager() */

        /*
         * Ignore bitfield conversion warnings as there is no good solution other than stop
         * using bitfields.
         */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
        powerStrip.binSwitch.switchStatus &= ~SWITCH_ON;
#pragma GCC diagnostic pop
        PowerStrip_hw_binary_switch_handler(false);
      }
      else
      {
        DPRINT(" ON\n"); /* Continuation of DPRINT in AppStateManager() */
        powerStrip.binSwitch.switchStatus |= SWITCH_ON;
        PowerStrip_hw_binary_switch_handler(true);
      }
      /* Tell the lifeline destination that EP1 state has been modified */
      void* pDataEp1 = CC_BinarySwitch_prepare_zaf_tse_data(&zaf_tse_local_ep1_actuation);
      ZAF_TSE_Trigger(CC_BinarySwitch_report_stx, pDataEp1, true);

      break;
    default:
      /*this is expection*/
      break;
  }
}

/**
 * @brief Toggles the notification timer.
 */
static void notificationToggle(void)
{
  DPRINT("\r\nNtfctn toggle");


  if (ESWTIMER_STATUS_FAILED == notificationOverLoadTimerStatus)
  {
    DPRINT("\r\nNtfctn start");

    notificationOverLoadActiveState = false;

    notificationOverLoadTimerStatus = TimerStart(&NotificationTimer, 30*1000);

    /*
     * The timer will transmit the first notification in 30 seconds. We
     * call the callback function directly to transmit the first
     * notification right now.
     */
    ZCB_NotificationTimerCallback(NULL);
  }
  else
  {
    DPRINT("\r\nNtfctn stop");
    /* Deactivate overload timer */
    TimerStop(&NotificationTimer);
    notificationOverLoadTimerStatus = ESWTIMER_STATUS_FAILED;
  }
}

/*
 * Please see description in CC_Basic.h.
*/
void* CC_Basic_prepare_zaf_tse_data(RECEIVE_OPTIONS_TYPE_EX* pRxOpt)
{
  /*
   * CC Basic is mapped to CC Binary Switch for Root and EP1
   * CC Basic is mapped to CC Multilevel Switch for EP2.
   *
   * Since CC Multilevel Switch will take care of True Status itself, only CC Binary Switch must
   * be handled here.
   */
  if (0 == pRxOpt->destNode.endpoint || 1 == pRxOpt->destNode.endpoint)
  {
    return CC_BinarySwitch_prepare_zaf_tse_data(pRxOpt);
  }
  else
  {
    return NULL;
  }
}

/*
 * Please see description in CC_Basic.h.
 */
void CC_Basic_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions, void* pData)
{
  /*
   * CC Basic is mapped to CC Binary Switch for endpoint 1.
   * CC Basic is mapped to CC Multilevel Switch for endpoint 2.
   *
   * Since CC Multilevel Switch will take care of True Status itself, only CC Binary Switch must
   * be handled here.
   *
   * Call here different functions for each endpoint (pData->rxOpt->destNode.endpoint) if they do
   * not map to the same Command Class & Command
   */
  s_zaf_tse_data_input_template_t* pDataInput = (s_zaf_tse_data_input_template_t*)(pData);
  RECEIVE_OPTIONS_TYPE_EX RxOptions = pDataInput->rxOptions;

  if (0 == RxOptions.destNode.endpoint || 1 == RxOptions.destNode.endpoint)
  {
    CC_BinarySwitch_report_stx(txOptions,pData);
  }
}

/*
 * See description in CC_Basic.h.
 */
void CC_Basic_report_notifyWorking(RECEIVE_OPTIONS_TYPE_EX* pRxOpt)
{
  UNUSED(pRxOpt);
}

/**
 * Considering several endpoints supporting the same Command Class,
 * this function finds the index of the end point if they were stored in an array.
 * e.g. if Endpoints 0, 3, 5 and 12 support the Command Class, their index would
 * be respectively 0, 1, 2 and 3.
 * @param endpoint Indicating the endpoint identifier to search for.
*/
uint8_t CC_BinarySwitch_getEndpointIndex(uint8_t endpoint)
{
  for (uint8_t i=0; i<sizeof(endpoints_Supporting_BinarySwitch_CC); i++)
  {
    if (endpoint == endpoints_Supporting_BinarySwitch_CC[i]){
      return i;
    }
  }
  /*End point was not found, this should not happen but in this case, just return the first End Point */
  return 0;
}

/**
 * Prepare the data input for the TSE for any Binary Switch CC command based on the pRxOption pointer.
 * @param pRxOpt pointer used to indicate how the frame was received (if any) and what endpoints are affected
*/
void* CC_BinarySwitch_prepare_zaf_tse_data(RECEIVE_OPTIONS_TYPE_EX* pRxOpt)
{
  uint8_t endpoint_index = CC_BinarySwitch_getEndpointIndex(pRxOpt->destNode.endpoint);
  memset(&ZAF_TSE_BinarySwitchData[endpoint_index], 0, sizeof(s_CC_binarySwitch_data_t));
  ZAF_TSE_BinarySwitchData[endpoint_index].rxOptions = *pRxOpt;

  /* Apply here a endpoint mapping, endpoint 0 state maps to endpoint 1.
  Note: in order to map the root device to more than one end point,
  it is recommended to modify the Command handler to be called for each mapped End point
  e.g. call CommandClassBinarySwitchSupportSet() for every mapped
  end point in handleCommandClassBinarySwitch() */
  if (0 == ZAF_TSE_BinarySwitchData[endpoint_index].rxOptions.destNode.endpoint)
  {
    ZAF_TSE_BinarySwitchData[endpoint_index].rxOptions.destNode.endpoint = 1;
  }

  return &ZAF_TSE_BinarySwitchData[endpoint_index];
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
 * file, e.g. PowerStrip_hw.c.
 */
ZW_WEAK void PowerStrip_hw_init(void)
{

}

ZW_WEAK void PowerStrip_hw_binary_switch_handler(bool on)
{
  UNUSED(on);
}

ZW_WEAK void PowerStrip_hw_multilevel_switch_handler(cc_multilevel_switch_t * p_switch)
{
  UNUSED(p_switch);
}
