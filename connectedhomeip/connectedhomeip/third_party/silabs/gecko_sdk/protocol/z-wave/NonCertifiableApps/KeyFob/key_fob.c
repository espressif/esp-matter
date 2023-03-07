/***************************************************************************//**
 * @file key_fob_app.c
 * @brief key_fob_app.c
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/****************************************************************************/
/* INCLUDES                                                                 */
/****************************************************************************/

// System includes
#include <string.h>

#include "zw_config_rf.h"
#include "config_app.h"

#include <DebugPrintConfig.h>
//#define DEBUGPRINT
#include <DebugPrint.h>

#include <ZAF_Common_interface.h>
#include <ZAF_PM_Wrapper.h>
#include <zaf_event_helper.h>
#include <zaf_job_helper.h>
#include <zaf_config_api.h>
#include <ZW_classcmd.h>
#include <ZW_TransportLayer.h>
#include <ZW_controller_api.h>
#include <ZW_system_startup_api.h>
#include <EventDistributor.h>
#include <SizeOf.h>
#include <AppTimer.h>
#include <KeyFob_hw.h>
#include <board_indicator.h>
#include <em_emu.h>
#include <em_wdog.h>
#include "events.h"

// Command Classes
#include <association_plus.h>
#include <agi.h>

#include <CC_Battery.h>

#include <ZAF_file_ids.h>
#include <ZAF_network_management.h>

#include "network_management.h"
#include "node_storage.h"
#include "zw_region_config.h"
#include "zw_build_no.h"

#include "sl_status.h"
#include "zw_region_config.h"
#include "zw_build_no.h"
#include "zpal_nvm.h"
#include "ZAF_nvm_app.h"
#include <zpal_misc.h>

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * Due to a design choice in PowerManager.c for the implementation of PM_StayAwake(),
 * it is not possible to invoke the function with 0 (forever awake) if the the timer
 * is already running.
 * The below definition overcomes it.
 */
#define PM_STAY_AWAKE_DURATION_ONE_DAY      (1000 * 3600 * 24)  // [ms] 1 day (Used as indefinite keep-awake, but with secu)
#define PM_STAY_AWAKE_DURATION_TEN_MINUTES  (1000 * 60 * 10)
#define PM_STAY_AWAKE_DURATION_REBOOT       (1000 * 4)          // [ms]
#define PM_STAY_AWAKE_DURATION_BTN          (1000 * 15)         // [ms]
#define PM_STAY_AWAKE_DURATION_LEARN_MODE   (1000 * 10)         // [ms]
#define PM_STAY_AWAKE_DURATION_3_SEC        (1000 * 3)
#define PM_STAY_AWAKE_DURATION_REPORT_WAIT  (1000 * 10)         // [ms]

/**
 * Application states. Function AppStateManager(..) includes the state
 * event machine.
 */
typedef enum _STATE_APP_
{
  STATE_APP_STARTUP,            /**< STATE_APP_STARTUP */
  STATE_APP_IDLE,               /**< STATE_APP_IDLE */
  STATE_APP_INCLUDE_EXCLUDE,    /**< STATE_APP_INCLUDE_EXCLUDE */
  STATE_APP_ASSOCIATION,        /**< STATE_APP_ASSOCIATION */
  STATE_APP_TRANSMIT_DATA,      /**< STATE_APP_TRANSMIT_DATA */
  STATE_APP_NETWORK_LEARNMODE,  /**< STATE_APP_NETWORK_LEARNMODE */
  STATE_APP_RESET               /**< STATE_APP_RESET */
} STATE_APP;

/**
 * Note: enum order is important, should be in sync with g_aEventHandlerTable elements
 */
typedef enum EApplicationEvent
{
  EAPPLICATIONEVENT_TIMER = 0,
  EAPPLICATIONEVENT_ZWRX,
  EAPPLICATIONEVENT_ZWCOMMANDSTATUS,
  EAPPLICATIONEVENT_APP,
  EAPPLICATIONEVENT_STATECHANGE,
} EApplicationEvent;

/****************************************************************************/
/*                        STATIC FUNCTION DECLARATION                       */
/****************************************************************************/

/**
 * Task for initialize
 * @param[in] pAppHandles Application handle
 * @return None
*/
static void ApplicationTask(SApplicationHandles* pAppHandles);

/**
 * Task for handling receiving data
 * @param[in] None
 * @return None
*/
static void EventHandlerZwRx(void);

/**
 * Task for processing commands received through Z-Wave
 * @param[in] None
 * @return None
*/
static void EventHandlerZwCommandStatus(void);

/**
 * Handle state change
 * @param[in] None
 * @return None
*/
static void KeyFobStateHandler(void);

/**
 * Set state and notify the Queue
 * @param[in] state is the new state to set
 * @return None
*/
static void ChangeState(STATE_APP newState);

static uint8_t SetRFReceiveMode(uint8_t mode);

static uint8_t GetCommandResponse(SZwaveCommandStatusPackage *pCmdStatus, EZwaveCommandStatusType cmdType);

static void EventHandlerApp(void);

static void EventQueueInit();

static void SendBasicSetDone(void);

static void SupervisionReport(void *pSubscriberContext, void* pRxPackage);

/**
 * Handler for application related tasks, called from button push
 * @param[in] pRxOpt Receive options.
 * @return Result of command parsing.
*/
static void AppStateManager(EVENT_APP event);

static void handle_network_management_states(node_id_t current_node_id);

/****************************************************************************/
/* Application specific button and LED definitions                          */
/****************************************************************************/

static const EventDistributorEventHandler g_aEventHandlerTable[] =
{
  AppTimerNotificationHandler,   // EAPPLICATIONEVENT_TIMER           = 0
  EventHandlerZwRx,              // EAPPLICATIONEVENT_ZWRX            = 1
  EventHandlerZwCommandStatus,   // EAPPLICATIONEVENT_ZWCOMMANDSTATUS = 2
  EventHandlerApp,               // EAPPLICATIONEVENT_APP             = 3
  KeyFobStateHandler             // EAPPLICATIONEVENT_STATECHANGE     = 4
};

static zpal_pm_handle_t m_RadioPowerLock;
static zpal_pm_handle_t m_PeripheralPowerLock;

static TaskHandle_t g_AppTaskHandle;

#ifdef DEBUGPRINT
static uint8_t m_aDebugPrintBuffer[96];
#endif

// Pointer to AppHandles that is passed as input to ApplicationTask(..)
SApplicationHandles* g_pAppHandles;

static const uint8_t SecureKeysRequested = REQUESTED_SECURITY_KEYS;

static SRadioConfig_t RadioConfig = {
  .iListenBeforeTalkThreshold = ELISTENBEFORETALKTRESHOLD_DEFAULT,
  .iTxPowerLevelMax    = APP_MAX_TX_POWER,
  .iTxPowerLevelAdjust = APP_MEASURED_0DBM_TX_POWER,
  .iTxPowerLevelMaxLR  = APP_MAX_TX_POWER_LR,
  .eRegion   = ZW_REGION,
  .radio_debug_enable = ENABLE_RADIO_DEBUG
};

/**
 * Please see the description of app_node_information_t.
 */
static uint8_t cmdClassListNonSecureNotIncluded[] =
{
  COMMAND_CLASS_ZWAVEPLUS_INFO,
  COMMAND_CLASS_ASSOCIATION,
  COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2,
  COMMAND_CLASS_ASSOCIATION_GRP_INFO,
  COMMAND_CLASS_TRANSPORT_SERVICE_V2,
  COMMAND_CLASS_BATTERY,
  COMMAND_CLASS_MANUFACTURER_SPECIFIC,
  COMMAND_CLASS_SECURITY,
  COMMAND_CLASS_SECURITY_2,
  COMMAND_CLASS_DEVICE_RESET_LOCALLY,
  COMMAND_CLASS_SUPERVISION,
  COMMAND_CLASS_VERSION
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
  COMMAND_CLASS_ASSOCIATION,
  COMMAND_CLASS_MULTI_CHANNEL_ASSOCIATION_V2,
  COMMAND_CLASS_ASSOCIATION_GRP_INFO,
  COMMAND_CLASS_BATTERY,
  COMMAND_CLASS_VERSION,
  COMMAND_CLASS_MANUFACTURER_SPECIFIC,
  COMMAND_CLASS_DEVICE_RESET_LOCALLY,
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

static SAppNodeInfo_t AppNodeInfo = {
  .CommandClasses.UnSecureIncludedCC.iListLength = sizeof_array(cmdClassListNonSecureNotIncluded),
  .CommandClasses.UnSecureIncludedCC.pCommandClasses = cmdClassListNonSecureNotIncluded,
  .CommandClasses.SecureIncludedUnSecureCC.iListLength = sizeof_array(cmdClassListNonSecureIncludedSecure),
  .CommandClasses.SecureIncludedUnSecureCC.pCommandClasses = cmdClassListNonSecureIncludedSecure,
  .CommandClasses.SecureIncludedSecureCC.iListLength = sizeof_array(cmdClassListSecure),
  .CommandClasses.SecureIncludedSecureCC.pCommandClasses = cmdClassListSecure,
  .NodeType.generic  = GENERIC_TYPE_GENERIC_CONTROLLER,
  .NodeType.specific = SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLLER,
  .DeviceOptionsMask = APPLICATION_NODEINFO_NOT_LISTENING
};

static const SProtocolConfig_t ProtocolConfig = {
    .pVirtualSlaveNodeInfoTable = NULL,
    .pSecureKeysRequested = &SecureKeysRequested,
    .pNodeInfo    = &AppNodeInfo,
    .pRadioConfig = &RadioConfig
};

static SEventDistributor g_EventDistributor;

/**
 * Application state-machine state.
 */
static STATE_APP currentState = STATE_APP_IDLE;

/**
 * Parameter is used to save wakeup reason after ApplicationInit(..)
 */
static EResetReason_t g_eResetReason;

static SSwTimer JobTimer;

#define APP_EVENT_QUEUE_SIZE          5

/**
 * The following four variables are used for the application event queue.
 */
static SQueueNotifying m_AppEventNotifyingQueue;
static StaticQueue_t m_AppEventQueueObject;
static EVENT_APP eventQueueStorage[APP_EVENT_QUEUE_SIZE];
static QueueHandle_t m_AppEventQueue;

static zpal_nvm_handle_t pFileSystemApplication;

/****************************************************************************/
/* MACRO                                                                    */
/****************************************************************************/

#define STOP_LEARNMODE()              Board_IndicateStatus(BOARD_STATUS_IDLE);  \
                                      ChangeState(STATE_APP_IDLE);              \
                                      zpal_pm_stay_awake(m_RadioPowerLock, 10)

#define PRINT_SUPPORTED_COMMAND_CLASSES();                                                  \
                                      if (CcListLength && pCcList)                                     \
                                      {                                                     \
                                        DPRINT("Supported command classes:\n\t");           \
                                        for (uint8_t cnt = 0; cnt < CcListLength; cnt++)    \
                                        {                                                   \
                                          DPRINTF("%X ", pCcList[cnt]);                     \
                                        }                                                   \
                                        DPRINT("\n");                                       \
                                      }

/****************************************************************************/
/* Static Functions                                                         */
/****************************************************************************/
ZW_APPLICATION_STATUS
ApplicationInit(EResetReason_t eResetReason)
{

  /* Initialize the radio board (SAW filter setup, etc) */
  /* hardware initialization */
  Board_Init();

#ifdef DEBUGPRINT
  zpal_debug_init();
  DebugPrintConfig(m_aDebugPrintBuffer, sizeof(m_aDebugPrintBuffer), zpal_debug_output);
#endif // DEBUGPRINT

  DPRINT("\n\n===ApplicationInit===\n");

  /* g_eResetReason now contains latest System Reset reason */
  g_eResetReason = eResetReason;

  /* Init state machine*/
  currentState = STATE_APP_STARTUP;

  DPRINT("\n-----------------------------\n");
  DPRINT("Z-Wave Sample App: KeyFob\n");
  DPRINTF("\nSDK: %d.%d.%d ZAF: %d.%d.%d.%d\n",
          SDK_VERSION_MAJOR,
          SDK_VERSION_MINOR,
          SDK_VERSION_PATCH,
          zpal_get_app_version_major(),
          zpal_get_app_version_minor(),
          zpal_get_app_version_patch(),
          ZAF_BUILD_NO);

  DPRINTF("ApplicationInit eResetReason = %d (0/7 = PIN/SW, 1 = EM4_WUT)\n", g_eResetReason);

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

  return APPLICATION_RUNNING; /*Return false to enter production test mode*/
}

/**
* Aquire a list of included nodes IDS in the network from protocol
*
* Method requires CommandStatus queue from protocol to be empty.
* Method requires CommandQueue to protocol to be empty.
* Method will cause assert on failure.
*
* @param[out]    node_id_list    Pointer to bitmask list where aquired included nodes IDs saved
*/
static void Get_included_nodes(uint8_t* node_id_list)
{
  const SApplicationHandles *m_pAppHandles = ZAF_getAppHandle();  
  SZwaveCommandPackage GetIncludedNodesCommand = {
      .eCommandType = EZWAVECOMMANDTYPE_ZW_GET_INCLUDED_NODES};

  // Put the Command on queue (and dont wait for it, queue must be empty)
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, (uint8_t *)&GetIncludedNodesCommand, 0);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
  // Wait for protocol to handle command (it shouldnt take long)
  SZwaveCommandStatusPackage includedNodes;
  if (GetCommandResponse(&includedNodes, EZWAVECOMMANDSTATUS_ZW_GET_INCLUDED_NODES))
  {
    memcpy(node_id_list, (uint8_t*)includedNodes.Content.GetIncludedNodes.node_id_list, sizeof(NODE_MASK_TYPE));
    return;
  }
  ASSERT(false);
}

static uint8_t sv_session_id = 0x30;
/* build a supervision get command
 * 
 * @param[out] getFrame   Buffer to store the built supervision get command
 * @param[out] getLen     The lenght of the built supervision get command
 * @param[in]  cmdFrame   The command to be encapsulated into the supervision get command
 * @param[in]  cmdLen     The lenght of the command to be encapsulated into the supervision get command
 */
static void BuildSupervisionGet(uint8_t  *getFrame,
                                uint16_t *getLen,
                                uint8_t  *cmdFrame,
                                uint8_t  cmdLen) 
{
  *getLen = 0;
  if (0 == sv_session_id)
    sv_session_id = 1;
  getFrame[(*getLen)++] = COMMAND_CLASS_SUPERVISION;
  getFrame[(*getLen)++] = SUPERVISION_GET;
  getFrame[(*getLen)++] = sv_session_id++ & SUPERVISION_GET_PROPERTIES1_SESSION_ID_MASK;
  getFrame[(*getLen)++] = cmdLen;
  memcpy(&getFrame[*getLen], cmdFrame, cmdLen);
  *getLen += cmdLen;
}

static uint8_t SendSecureFrame(node_id_t node_id,
                               zwave_keyset_t tx_key,
                               uint8_t number_of_responses,
                               bool isMultiCast,
                               uint8_t *pData,
                               size_t data_length,
                               void (*pCallback)(uint8_t, const TX_STATUS_TYPE*))
{
  SZwaveTransmitPackage TransmitPackage;
  memset(&TransmitPackage, 0, sizeof(TransmitPackage));
  TransmitPackage.eTransmitType = EZWAVETRANSMITTYPE_SECURE;
  SSecureSendData *params = &TransmitPackage.uTransmitParams.SendDataParams;
  if (isMultiCast) {
    node_storage_group_member_nodemask_get((uint8_t*)params->connection.remote.address.nodeList.nodeMask,
                                            sizeof(params->connection.remote.address.nodeList.nodeMask));
    params->connection.remote.is_multicast = isMultiCast;
  } else {
    params->connection.remote.address.node_id = node_id;
  }
  params->tx_options.number_of_responses = number_of_responses;
  params->tx_keys = tx_key;
  params->ptxCompleteCallback = pCallback;
  memcpy(params->data, pData, data_length);
  params->data_length = data_length;

  // Put the package on queue (and don't wait for it)
  return EQUEUENOTIFYING_STATUS_SUCCESS == QueueNotifyingSendToBack(g_pAppHandles->pZwTxQueue, (uint8_t*)&TransmitPackage, 0);
}


/**
 * @brief keyfob is working as secondary then we need to know if the slave node is included with
 * security or not
 * 
 * First we send basic get cmd using s2_unautnticated
 * If we received basic report then we update the security information for the node
 * if not then we try with s0 if we succeeded then we update the security information for the node 
 */
static bool SendBasicGet(node_id_t node_id, zwave_keyset_t tx_key)
{
  DPRINT("\nbasic_get");
  uint8_t basic_get[] = {COMMAND_CLASS_BASIC, BASIC_GET};
  TimerStart(&JobTimer, PM_STAY_AWAKE_DURATION_REPORT_WAIT);
  // Put the package on queue (and don't wait for it)
  return (true == SendSecureFrame(node_id, tx_key, 0, false, basic_get, sizeof(basic_get), NULL));
}

static NODE_MASK_TYPE nodeid_list; 
static node_id_t probed_nodeid = 0;
static void NodeSecurityProbe(bool init);

static void BasicReprotReceived (void *pSubscriberContext, void* pRxPackage) {
  (void)pSubscriberContext;
  (void)pRxPackage;
  TimerStop(&JobTimer);

  SZwaveCommandPackage CommandPackage;
  CommandPackage.eCommandType = EZWAVECOMMANDTYPE_SECURE_NETWORK_MANAGEMENT_SET_SECURITY_FLAGS;
  CommandPackage.uCommandParams.SetSecurityFlags.nodeID = probed_nodeid;

  if (NETWORK_MANAGEMENT_STATE_S2_PROBE == get_current_network_management_state())
  {
    CommandPackage.uCommandParams.SetSecurityFlags.nodeS2Capable      = true;
    CommandPackage.uCommandParams.SetSecurityFlags.nodeS2Included     = true;
    CommandPackage.uCommandParams.SetSecurityFlags.nodeSecureIncluded = true;
    QueueNotifyingSendToBack(g_pAppHandles->pZwCommandQueue, (uint8_t*) &CommandPackage, 500);

    ZW_NODE_MASK_CLEAR_BIT(nodeid_list, probed_nodeid );
    NodeSecurityProbe(false);
  }
  else if (NETWORK_MANAGEMENT_STATE_S0_PROBE == get_current_network_management_state())
  {
    CommandPackage.uCommandParams.SetSecurityFlags.nodeS2Capable      = false;
    CommandPackage.uCommandParams.SetSecurityFlags.nodeS2Included     = false;
    CommandPackage.uCommandParams.SetSecurityFlags.nodeSecureIncluded = true;
    QueueNotifyingSendToBack(g_pAppHandles->pZwCommandQueue, (uint8_t*) &CommandPackage, 500);

    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_S2_PROBE);
    ZW_NODE_MASK_CLEAR_BIT(nodeid_list, probed_nodeid );
    NodeSecurityProbe(false);
  }
}

static void NodeSecurityProbe(bool init)
{
  if (true == init) {
    Get_included_nodes(nodeid_list);
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_S2_PROBE);
    ZAF_CP_SubscribeToCmd(ZAF_getCPHandle(), NULL, BasicReprotReceived, COMMAND_CLASS_BASIC, BASIC_REPORT);
    probed_nodeid = 2;
  }

  while((0 == ZW_NODE_MASK_NODE_IN(nodeid_list, probed_nodeid)) ||
        (probed_nodeid == g_pAppHandles->pNetworkInfo->NodeId))
  {
    probed_nodeid++;
  }
  
  if (probed_nodeid < ZW_MAX_NODES) {
    zpal_pm_stay_awake(m_RadioPowerLock, PM_STAY_AWAKE_DURATION_3_SEC);
    if (NETWORK_MANAGEMENT_STATE_S2_PROBE == get_current_network_management_state()) {
       SendBasicGet(probed_nodeid, SECURITY_KEY_S2_UNAUTHENTICATED_BIT);
    } else {
       SendBasicGet(probed_nodeid, SECURITY_KEY_S0_BIT);
    }
  } else {
    ZAF_CP_UnsubscribeToCmd(ZAF_getCPHandle(), NULL, BasicReprotReceived, COMMAND_CLASS_BASIC, BASIC_REPORT);
    zpal_pm_cancel(m_RadioPowerLock);
    KeyFob_network_learnmode_led_handler(false);      
    ChangeState(STATE_APP_IDLE);
    ZAF_EventHelperEventEnqueue(EVENT_APP_FINISH_EVENT_JOB);
  }

}

static void
ZCB_JobTimeout(SSwTimer *pTimer)
{
  UNUSED(pTimer);
  DPRINT("ZCB_JobTimeout\n");
  if (NETWORK_MANAGEMENT_STATE_ADD_GROUP == get_current_network_management_state() ||
      NETWORK_MANAGEMENT_STATE_REMOVE_GROUP == get_current_network_management_state())
  {
    DPRINT("ZCB: Add/Remove group timeout\n");
    // Timeout
    ZAF_EventHelperEventEnqueue(EVENT_APP_FINISH_EVENT_JOB);
    KeyFob_group_add_Led_handler(false);
    KeyFob_group_remove_Led_handler(false);
  } 
  else if (NETWORK_MANAGEMENT_STATE_START_INCLUSION == get_current_network_management_state() ||
      NETWORK_MANAGEMENT_STATE_START_EXCLUSION == get_current_network_management_state())
  {
    DPRINT("ZCB: Network Add/Remove timeout\n");
    Board_IndicateStatus(BOARD_STATUS_LEARNMODE_INACTIVE);
    handle_network_management_states(0);
  }
  else if (NETWORK_MANAGEMENT_STATE_LEARNMODE == get_current_network_management_state())
  {
    DPRINT("ZCB: Network LearnMode timeout\n");
    handle_network_management_states(0);
  }
  else if (NETWORK_MANAGEMENT_STATE_SECURITY_PROBE == get_current_network_management_state())
  {
    /*We start probing slave nodes for granted security keys*/
    NodeSecurityProbe(true);
  }
  else if (NETWORK_MANAGEMENT_STATE_S2_PROBE == get_current_network_management_state())
  {
    /*probing the current node for s2 failed then try s0*/
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_S0_PROBE);
    NodeSecurityProbe(false);
  }
  else if (NETWORK_MANAGEMENT_STATE_S0_PROBE == get_current_network_management_state())
  {
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_S2_PROBE);
    /*probing the current node for s0 (and s2) failed then removed with from the list*/
    ZW_NODE_MASK_CLEAR_BIT(nodeid_list, probed_nodeid );    
    NodeSecurityProbe(false);
  }
  else if ((NETWORK_MANAGEMENT_STATE_IDLE == get_current_network_management_state()) &&
           STATE_APP_TRANSMIT_DATA== currentState) {
    SendBasicSetDone();
  }
}


static __attribute__((noreturn)) void
ApplicationTask(SApplicationHandles* pAppHandles)
{
  DPRINT("\n\n===========ApplicationTask============\n");

  DPRINT("Enabling watchdog\n");
  zpal_enable_watchdog(true);

  g_AppTaskHandle = xTaskGetCurrentTaskHandle();
  g_pAppHandles = pAppHandles;

  ZAF_Init(g_AppTaskHandle, pAppHandles, &ProtocolConfig, NULL);
  ZAF_setApplicationData(g_AppTaskHandle, pAppHandles,  &ProtocolConfig);
  EventQueueInit(); // Initialize the slew of modules made for event management.
  // Init AppTimer with an app handle
  AppTimerInit(EAPPLICATIONEVENT_TIMER, g_AppTaskHandle);

  AppTimerRegister(&JobTimer, false, ZCB_JobTimeout);

  ZAF_CP_SubscribeToCmd(ZAF_getCPHandle(), NULL, SupervisionReport, COMMAND_CLASS_SUPERVISION, SUPERVISION_REPORT);

  // Setup power management.
  m_RadioPowerLock = zpal_pm_register(ZPAL_PM_TYPE_USE_RADIO);

  if ((ERESETREASON_DEEP_SLEEP_WUT != g_eResetReason) &&
      (ERESETREASON_DEEP_SLEEP_EXT_INT != g_eResetReason))
  {
    zpal_pm_stay_awake(m_RadioPowerLock, PM_STAY_AWAKE_DURATION_REBOOT); // Allowing time for choosing learnmode after reset.
  }

  // Generate event that says the APP needs additional initialization.
  ZAF_EventHelperEventEnqueue(EVENT_APP_INIT); // The state is already set to STATE_APP_STARTUP.


  //Initialize buttons
  KeyFob_hw_init(g_eResetReason);

  Board_IndicatorInit();
  
  EventDistributorConfig(
      &g_EventDistributor,
      sizeof_array(g_aEventHandlerTable),
      g_aEventHandlerTable,
      KeyFobStateHandler);

  DPRINTF("IsWakeupCausedByRtccTimeout = %s\n", (IsWakeupCausedByRtccTimeout()) ? "true" : "false");
  DPRINTF("CompletedSleepDurationMs    = %u\n", GetCompletedSleepDurationMs());

  // This will start additional initialization of the app after the FreeRTOS tasks have started executing.
  ChangeState(STATE_APP_STARTUP);


  /**************************************************************
   * The below approach prevents the system from going to a sleep
   * mode that requires wake up from reset state.
   * (Not supported yet.)
   **************************************************************/
  m_PeripheralPowerLock = zpal_pm_register(ZPAL_PM_TYPE_DEEP_SLEEP);

  zpal_pm_stay_awake(m_PeripheralPowerLock, 0);
  /***********************************************************/


  // Wait for and process events
  DPRINT("KeyFob Event processor Started\n");
  uint32_t iMaxTaskSleep = 0xFFFFFFFF;
  for (;;)
  {
    EventDistributorDistribute(&g_EventDistributor, iMaxTaskSleep, 0);
  }
}


static void
SetProtocolDefault(uint8_t status, const TX_STATUS_TYPE * status_type)
{
  UNUSED(status);
  UNUSED(status_type);
  DPRINTF("%s status=%d\n", __func__, status);

  /* Reset protocol  regardless of TRANSMIT status */
  DPRINT("\nDisabling watchdog during reset\n");
  zpal_enable_watchdog(false);

  DPRINT("Resetting protocol FileSystem to default\n");
  SZwaveCommandPackage CommandPackage;
  CommandPackage.eCommandType = EZWAVECOMMANDTYPE_SET_DEFAULT;

  if (EQUEUENOTIFYING_STATUS_SUCCESS != QueueNotifyingSendToBack(g_pAppHandles->pZwCommandQueue,
                                                                 (uint8_t*) &CommandPackage,
                                                                 500))
  {
    DPRINT("Warning: Failed to perform SetDefault");
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
  zpal_status_t errCode;

  AssociationInit(true, pFileSystemApplication);

  ZAF_Reset();

  uint32_t appVersion = zpal_get_app_version();
  errCode = zpal_nvm_write(pFileSystemApplication, ZAF_FILE_ID_APP_VERSION, &appVersion, ZAF_FILE_SIZE_APP_VERSION);
  ASSERT(ZPAL_STATUS_OK == errCode); // Assert has been kept for debugging , can be removed from production code if this error can only be caused by some internal flash HW failure

  // Set default Basic Set Group - no members
  node_storage_init_group();
}


void AppResetNvm(void)
{
  DPRINT("Resetting application FileSystem to default\n");

  ASSERT(0 != pFileSystemApplication); //Assert has been kept for debugging , can be removed from production code. This error can only be caused by some internal flash HW failure

  zpal_status_t errCode = zpal_nvm_erase_all(pFileSystemApplication);
  ASSERT(ZPAL_STATUS_OK == errCode); //Assert has been kept for debugging , can be removed from production code. This error can only be caused by some internal flash HW failure

  /* Apparently there is no valid configuration in NVM, so load */
  /* default values and save them. */
  SetDefaultConfiguration();
}


/**
 * @brief This function loads the application settings from non-volatile memory.
 * If no settings are found, default values are used and saved.
 */
bool
LoadConfiguration(void)
{
  uint32_t appVersion;
  zpal_status_t versionFileStatus = zpal_nvm_read(pFileSystemApplication, ZAF_FILE_ID_APP_VERSION, &appVersion, ZAF_FILE_SIZE_APP_VERSION);

  if (ZPAL_STATUS_OK == versionFileStatus)
  {
    if (zpal_get_app_version() != appVersion)
    {
      DPRINT("ERROR: App-version did not match the app-version stored on NVM.\n");
      // Add code for migration of file system to higher version here.
    }

    /* Initialize association module */
    AssociationInit(false, pFileSystemApplication);

    // End Device node IDs in Basic Set Association group will be stored in non volatile memory
    node_storage_init_group();

    return true;
  }
  else
  {
    DPRINT("Application FileSystem Verify failed\n");

    pFileSystemApplication = ZAF_GetFileSystemHandle();
    // Reset the file system if ZAF_FILE_ID_APP_VERSION is missing since this indicates
    // corrupt or missing file system.
    AppResetNvm();
    return false;
  }
}

static void stayAwake(void)
{
  /* Don't sleep the next 10 seconds */
  zpal_pm_stay_awake(m_RadioPowerLock, PM_STAY_AWAKE_DURATION_LEARN_MODE);
}

static void doRemainingInitialization()
{
  DPRINT("--> doRemainingInitialization()\n");

  /* Load the application settings from NVM file system */
  bool filesExist = LoadConfiguration();
  UNUSED(filesExist);

  // Setup AGI group lists
  AGI_Init();

  /*
   * Initialize Event Scheduler.
   */
  Transport_OnApplicationInitSW( &m_AppNIF, stayAwake);


  DPRINTF("g_eResetReason: 0x%08x\n", g_eResetReason);
  if (ERESETREASON_DEEP_SLEEP_EXT_INT == g_eResetReason)
  {
    DPRINT("ERESETREASON_DEEP_SLEEP_EXT_INT\n");
    uint32_t em4_wakeup_flags = Board_GetGpioEm4Flags();

    if (0 != em4_wakeup_flags)
    {
      Board_ProcessEm4PinWakeupFlags(em4_wakeup_flags);
    }
    ChangeState(STATE_APP_IDLE);
  }

  if ((ERESETREASON_DEEP_SLEEP_WUT == g_eResetReason) && (STATE_APP_TRANSMIT_DATA != currentState))
  {
    ChangeState(STATE_APP_IDLE);
  }

  if(ERESETREASON_PIN == g_eResetReason ||
     ERESETREASON_BROWNOUT == g_eResetReason ||
     ERESETREASON_POWER_ON == g_eResetReason ||
     ERESETREASON_SOFTWARE == g_eResetReason ||
     ERESETREASON_WATCHDOG == g_eResetReason)
  {
    /* Init state machine*/
    ZAF_EventHelperEventEnqueue(EVENT_EMPTY);
  }
}

uint8_t IsPrimaryController(void)
{
  const SApplicationHandles *m_pAppHandles = ZAF_getAppHandle();
  SZwaveCommandPackage cmdPackage = {
      .eCommandType = EZWAVECOMMANDTYPE_IS_PRIMARY_CTRL};
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, (uint8_t *)&cmdPackage, 500);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
  SZwaveCommandStatusPackage cmdStatus;
  if (GetCommandResponse(&cmdStatus, EZWAVECOMMANDSTATUS_IS_PRIMARY_CTRL))
  {
    return cmdStatus.Content.IsPrimaryCtrlStatus.result;
  }
  ASSERT(false);
  return 0;
}

static void EventHandlerZwRx(void)
{
  DPRINT("EventHandlerZwRx()\n");

  const SApplicationHandles* pAppHandle = ZAF_getAppHandle();
  QueueHandle_t Queue = pAppHandle->ZwRxQueue;
  SZwaveReceivePackage RxPackage;

  // Handle incoming replies
  while (xQueueReceive(Queue, (uint8_t *)(&RxPackage), 0) == pdTRUE)
  {
    DPRINTF("Incoming Rx %x\n", RxPackage.eReceiveType);

    switch (RxPackage.eReceiveType)
    {
      case EZWAVERECEIVETYPE_SINGLE:
        DPRINT("-->EZWAVERECEIVETYPE_SINGLE\n");
        break;
      case EZWAVERECEIVETYPE_SECURE_FRAME_RECEIVED:
        ZAF_CP_CommandPublish(ZAF_getCPHandle(), (void *) &RxPackage);
        TimerStart(&JobTimer, PM_STAY_AWAKE_DURATION_3_SEC);
      break;

      case EZWAVERECEIVETYPE_NODE_UPDATE:
        DPRINT("-->EZWAVERECEIVETYPE_NODE_UPDATE\n");
        if (NETWORK_MANAGEMENT_STATE_ADD_GROUP == get_current_network_management_state())
        {
          DPRINTF("Adding %u to group\n", RxPackage.uReceiveParams.RxNodeUpdate.NodeId);
          node_storage_add_group_member_nodeid(RxPackage.uReceiveParams.RxNodeUpdate.NodeId);
          ZAF_EventHelperEventEnqueue(EVENT_APP_FINISH_EVENT_JOB);
        }
        if (NETWORK_MANAGEMENT_STATE_REMOVE_GROUP == get_current_network_management_state())
        {
          DPRINTF("Removing %u from group\n", RxPackage.uReceiveParams.RxNodeUpdate.NodeId);
          node_storage_remove_group_member_nodeid(RxPackage.uReceiveParams.RxNodeUpdate.NodeId);
          ZAF_EventHelperEventEnqueue(EVENT_APP_FINISH_EVENT_JOB);
        }
        break;

      case EZWAVERECEIVETYPE_STAY_AWAKE:
        DPRINT("-->EZWAVERECEIVETYPE_STAY_AWAKE\n");
        /*
         * Non-application frame was received, that must keep device awake.
         */
        break;

      default:
        DPRINTF("-->Invalid Receive Type: %d\n", RxPackage.eReceiveType);
        break;
    }
  }
}

static void KeyFobStateHandler(void)
{
  DPRINT("KeyFobStateHandler()\n");

  switch (currentState)
  {
    case STATE_APP_STARTUP:
      SetRFReceiveMode(1);
      break;

    case STATE_APP_IDLE:
      break;

    default:
      DPRINT("KeyFobStateHandler - Default case\n\n\n");
      break;
  }
}


static void EventHandlerZwCommandStatus(void)
{
  DPRINT("EventHandlerZwCommandStatus()\n");

  const SApplicationHandles* pAppHandle = ZAF_getAppHandle();
  const QueueHandle_t Queue = pAppHandle->ZwCommandStatusQueue;
  SZwaveCommandStatusPackage Status;
  node_id_t sourceNode;

  // Handle incoming replies
  while (xQueueReceive(Queue, (uint8_t *)(&Status), 0) == pdTRUE)
  {
    DPRINTF("Incoming Status msg 0x%02x\n", Status.eStatusType);

    switch (Status.eStatusType)
    {
      case EZWAVECOMMANDSTATUS_SECURE_ON_NODE_ADDED:
        sourceNode = Status.Content.USecureAppNotification.nodeAddedNotification.node_id;
        DPRINTF("Included node: NodeID: %d\n", sourceNode);

        sl_status_t status = Status.Content.USecureAppNotification.nodeAddedNotification.status;
        if (SL_STATUS_OK != status)
        {
          DPRINTF("Inclusion Failed. Error: 0x%X\n", status);
        }
        else
        {
          uint8_t CcListLength =
              Status.Content.USecureAppNotification.nodeAddedNotification.nif.command_class_list_length;
          const uint16_t * pCcList =
              Status.Content.USecureAppNotification.nodeAddedNotification.nif.command_class_list;

          PRINT_SUPPORTED_COMMAND_CLASSES()
        }
        // This will stop the learnmode process.
        // Also end device is added to KeyFob Basic Set Group Association
        handle_network_management_states(sourceNode);
        DPRINT("Inclusion ended\n");
        break;

      case EZWAVECOMMANDSTATUS_SECURE_ON_NODE_DELETED:
        sourceNode = Status.Content.USecureAppNotification.nodeDeletedNotification.node_id;
        DPRINTF("Excluded Node: %d\n", sourceNode);

        // This will stop the learnmode process.
        handle_network_management_states(sourceNode);
        DPRINT("Exclusion ended\n");
        break;

      case EZWAVECOMMANDSTATUS_TX:
        DPRINT("-->EZWAVECOMMANDSTATUS_TX\n");
        SZWaveTransmitStatus *pTxStatus = &Status.Content.TxStatus;
        if ((pTxStatus->bIsTxFrameLegal) && (pTxStatus->Handle))
        {
          DPRINT("Tx Status received\n");

          void (*pCallback)(uint8_t, const TX_STATUS_TYPE *) = (void (*)(uint8_t,  const TX_STATUS_TYPE *))pTxStatus->Handle;
          pCallback(pTxStatus->TxStatus, &pTxStatus->ExtendedTxStatus);
        }
        else
        {
          // Drop it..
          DPRINT("Tx Status received - dropped\n");
        }
        break;
  
      case EZWAVECOMMANDSTATUS_SECURE_ON_NEW_NETWORK_ENTERED:
        ///< Node entered a new network
        DPRINTF("-->EZWAVECOMMANDSTATUS_SECURE_ON_NEW_NETWORK_ENTERED %08X, %u\n", Status.Content.USecureAppNotification.nodeEnteredNetwork.home_id, Status.Content.USecureAppNotification.nodeEnteredNetwork.node_id);
        handle_network_management_states(Status.Content.USecureAppNotification.nodeEnteredNetwork.node_id);
        break;

      case EZWAVECOMMANDSTATUS_SECURE_ON_NETWORK_MANAGEMENT_STATE_UPDATE:
        ///< Secure network management changed state
        DPRINTF("-->EZWAVECOMMANDSTATUS_SECURE_ON_NETWORK_MANAGEMENT_STATE_UPDATE %u\n", Status.Content.USecureAppNotification.nodeNetworkManagement.state);
        if (0 == Status.Content.USecureAppNotification.nodeNetworkManagement.state)
        {
          handle_network_management_states(0);
        }
        break;

      case EZWAVECOMMANDSTATUS_SECURE_ON_NEW_SUC:
        ///< New SUC NodeID received
        DPRINT("-->EZWAVECOMMANDSTATUS_SECURE_ON_NEW_SUC\n");
        break;

      case EZWAVECOMMANDSTATUS_SECURE_ON_FRAME_TRANSMISSION:
        ///< Frame transmission result
        DPRINT("-->EZWAVECOMMANDSTATUS_SECURE_ON_FRAME_TRANSMISSION\n");
        if ( NETWORK_MANAGEMENT_STATE_SECURITY_PROBE== get_current_network_management_state()) {
          TimerStart(&JobTimer, 2 * 1000);
        }
        break;

      case EZWAVECOMMANDSTATUS_SECURE_ON_RX_FRAME_RECEIVED_INDICATOR:
        ///< Frame received from NodeID indicator
        DPRINT("-->EZWAVECOMMANDSTATUS_SECURE_ON_RX_FRAME_RECEIVED_INDICATOR\n");
        if ( NETWORK_MANAGEMENT_STATE_SECURITY_PROBE== get_current_network_management_state()) {
          TimerStart(&JobTimer, 2 * 1000);
        }
        break;

      case EZWAVECOMMANDSTATUS_LEARN_MODE_STATUS:
        DPRINT("-->EZWAVECOMMANDSTATUS_LEARN_MODE_STATUS\n");
        break;

      case EZWAVECOMMANDSTATUS_SET_DEFAULT:
      {
        DPRINT("-->EZWAVECOMMANDSTATUS_SET_DEFAULT\n");
        DPRINT("Protocol Ready for reset\r\n");
        ZAF_EventHelperEventEnqueue(EVENT_APP_FLUSHMEM_READY);
        break;
      }

      case EZWAVECOMMANDSTATUS_REPLACE_FAILED_NODE_ID:
        DPRINT("-->EZWAVECOMMANDSTATUS_REPLACE_FAILED_NODE_ID\n");
        break;

      case EZWAVECOMMANDSTATUS_REMOVE_FAILED_NODE_ID:
        DPRINT("-->EZWAVECOMMANDSTATUS_REMOVE_FAILED_NODE_ID\n");
        break;

      case EZWAVECOMMANDSTATUS_INVALID_TX_REQUEST:
        DPRINT("-->EZWAVECOMMANDSTATUS_INVALID_TX_REQUEST\n");
        DPRINTF("Invalid TX Request to protocol - %d\n", Status.Content.InvalidTxRequestStatus.InvalidTxRequest);
        break;

      case EZWAVECOMMANDSTATUS_INVALID_COMMAND:
        DPRINT("-->EZWAVECOMMANDSTATUS_INVALID_COMMAND\n");
        DPRINTF("Invalid command to protocol - %d\n", Status.Content.InvalidCommandStatus.InvalidCommand);
        break;
      case EZWAVECOMMANDSTATUS_PM_SET_POWERDOWN_CALLBACK:
        DPRINT("-->EZWAVECOMMANDSTATUS_PM_SET_POWERDOWN_CALLBACK registered\n");
        break;

      default:
        DPRINTF("-->Unknown / Unexpected  CMD - %d\n", Status.eStatusType);
        ASSERT(false);
        break;
    }
  }
}

static void ChangeState(STATE_APP newState)
{
  if (currentState != newState)
  {
    DPRINTF("\nState changed: %d -> %d\n", currentState, newState);
    TimerStop(&JobTimer);
    xTaskNotify(g_AppTaskHandle,
                (1 << EAPPLICATIONEVENT_STATECHANGE),
                eSetBits);
    currentState = newState;
  }
}

static uint8_t nodesInGroup = 0;
static void
SendBasicSetDone(void) {
  ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
  KeyFob_basic_off_Led_handler(false);  
  KeyFob_basic_on_Led_handler(false);  
}

static void SupervisionReport (void *pSubscriberContext, void* pRxPackage) {
  (void)pSubscriberContext;
  SZwaveReceivePackage* myPackage = (SZwaveReceivePackage *)pRxPackage;
  ZW_SUPERVISION_REPORT_FRAME* pReport = (ZW_SUPERVISION_REPORT_FRAME *)&myPackage->uReceiveParams.Rx.Payload.padding;
  if ((SUPERVISION_REPORT == pReport->cmd) && 
      ((pReport->properties1 & SUPERVISION_GET_PROPERTIES1_SESSION_ID_MASK)
        == ((sv_session_id - 1) & SUPERVISION_GET_PROPERTIES1_SESSION_ID_MASK)))
  {
    if (!--nodesInGroup)
    {
      SendBasicSetDone();
    }
  }
}

static uint8_t SendBasicSetToGroup(uint8_t value,  void (*pCallback)(uint8_t, const TX_STATUS_TYPE*))
{
   nodesInGroup = node_storage_group_member_count();
  if (0 < nodesInGroup)
  {
    DPRINTF("\nBasicToGroup (%u nodes) value %u\n", nodesInGroup, value);
    zpal_pm_stay_awake(m_RadioPowerLock, 0);
    TimerStart(&JobTimer, PM_STAY_AWAKE_DURATION_3_SEC);
    /**
     * @attention
     * We will be sending a multicast to a list of nodes on our local group
     * maintained locally and with no dependency towards the ZAF.
     *
     * This group is persistently stored on NVM.
     */
    uint8_t basic_set_cmd[] = {COMMAND_CLASS_BASIC, BASIC_SET, value};
    uint8_t supervision_get[4 + sizeof(basic_set_cmd)];
    uint16_t frame_length;
    // encapsulates basic set command into supervision get command    
    BuildSupervisionGet(supervision_get, &frame_length, basic_set_cmd, sizeof(basic_set_cmd));
    if (SendSecureFrame(0, 0, 1, true, supervision_get, frame_length, pCallback))
    {
      if (0 == value)
      {
        KeyFob_basic_off_Led_handler(true);
      }
      else
      {
        KeyFob_basic_on_Led_handler(true);
      }
      return true;
    }
    return false;
  }
  else
  {
    DPRINTF("Group Empty - Basic Set %s\n", value ? "ON" : "OFF");
    SendBasicSetDone();
    return true;
  }
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


static uint8_t SetRFReceiveMode(uint8_t mode)
{
  DPRINTF("SetRFReceiveMode(%u)\n", mode);

  const SApplicationHandles* pAppHandle = ZAF_getAppHandle();
  SZwaveCommandPackage pCmdPackage = {
      .eCommandType = EZWAVECOMMANDTYPE_SET_RF_RECEIVE_MODE,
      .uCommandParams.SetRfReceiveMode.mode = mode};

  // Put the Command on queue (and dont wait for it, queue must be empty)
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(pAppHandle->pZwCommandQueue, (uint8_t *)&pCmdPackage, 0);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
  SZwaveCommandStatusPackage cmdStatus;
  if (GetCommandResponse(&cmdStatus, EZWAVECOMMANDSTATUS_SET_RF_RECEIVE_MODE))
  {
    return cmdStatus.Content.SetRFReceiveModeStatus.result;
  }

  return 0;
}


static uint8_t checkCommandType(TaskHandle_t pAppTaskHandle, QueueHandle_t Queue, SZwaveCommandStatusPackage *pCmdStatus, EZwaveCommandStatusType cmdType)
{
  if (pCmdStatus->eStatusType == cmdType)
  {
    if (pAppTaskHandle && (0 < uxQueueMessagesWaiting(Queue)))
    {
      /* Only call xTaskNotify if still elements in queue */
      BaseType_t Status = xTaskNotify(pAppTaskHandle, 1 << EAPPLICATIONEVENT_ZWCOMMANDSTATUS, eSetBits);
      ASSERT(pdPASS == Status); // We probably received a bad Task handle
    }
    return true;
  }
  else
  {
    BaseType_t result = xQueueSendToBack(Queue, (uint8_t*)pCmdStatus, 0);
    ASSERT(pdTRUE == result);
  }
  return false;
}


static uint8_t GetCommandResponse(SZwaveCommandStatusPackage *pCmdStatus, EZwaveCommandStatusType cmdType)
{
  const SApplicationHandles * m_pAppHandles = ZAF_getAppHandle();
  TaskHandle_t m_pAppTaskHandle = ZAF_getAppTaskHandle();
  QueueHandle_t Queue = m_pAppHandles->ZwCommandStatusQueue;
  if (!pCmdStatus)
  {
    return false;
  }
  for (uint8_t delayCount = 0; delayCount < 100; delayCount++)
  {
    for (UBaseType_t QueueElmCount = uxQueueMessagesWaiting(Queue);  QueueElmCount > 0; QueueElmCount--)
    {
      pCmdStatus->eStatusType = ~cmdType;
      BaseType_t queueStatus = xQueueReceive(Queue, (uint8_t*)pCmdStatus, 0);
      if ((pdPASS == queueStatus) && (true == checkCommandType(m_pAppTaskHandle, Queue, pCmdStatus, cmdType)))
      {
        // Found the needed cmdType
        return true ;
      }
    }
    vTaskDelay(10);
  }
  if (m_pAppTaskHandle && (0 < uxQueueMessagesWaiting(Queue)))
  {
    /* Only call xTaskNotify if still elements in queue */
    BaseType_t Status = xTaskNotify(m_pAppTaskHandle, 1 << EAPPLICATIONEVENT_ZWCOMMANDSTATUS, eSetBits);
    ASSERT(Status == pdPASS); // We probably received a bad Task handle
  }
  return false;
}


static void EventHandlerApp(void)
{
  uint8_t event;

  while (xQueueReceive(m_AppEventQueue, (uint8_t*)(&event), 0) == pdTRUE)
  {
    AppStateManager((EVENT_APP)event);
  }
}

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
   * the specific task about a pending job
   */
  QueueNotifyingInit(
      &m_AppEventNotifyingQueue,
      m_AppEventQueue,
      g_AppTaskHandle,
      EAPPLICATIONEVENT_APP);

  /*
   * Wraps the QueueNotifying module for simple event generations
   */
  ZAF_EventHelperInit(&m_AppEventNotifyingQueue);

  /*
   * Creates an internal queue to store no more than @ref JOB_QUEUE_BUFFER_SIZE jobs.
   * This module has no notification feature
   */
  ZAF_JobHelperInit();
}


void notAppStateDependentActivity(EVENT_APP event)
{
  UNUSED(event);
  /**
   * Contains actions that are always performed regardless of app-state.
   */
}


static void AppState_StartUp(EVENT_APP event)
{
  if (EVENT_APP_INIT == event)
  {
    /*
     * This approach makes it possible to do less initialization before the scheduler starts.
     * Hence, this was made to reduce the boot-up time.
     */
    doRemainingInitialization();
  }
  else if(EVENT_APP_FLUSHMEM_READY == event)
  {
    AppResetNvm();
  }
  else
  {
    ChangeState(STATE_APP_IDLE);
  }
}

static void AppState_Idle(EVENT_APP event)
{
  /*************************************
   * Button functionalities
   ************************************/

  // Inclusion process starting
  if (EVENT_APP_BUTTON_INCLUDE_DEVICE == event)
  {
    if (IsPrimaryController())
    {
      DPRINT("\n===Inclusion process started===\n");

      bool ret = key_fob_start_inclusion();  // This sets state to NETWORK_MANAGEMENT_STATE_START_INCLUSION
      if (ret == false)
      {
        DPRINT("Error, queue is full\n");
      }
      else
      {
        zpal_pm_stay_awake(m_RadioPowerLock, PM_STAY_AWAKE_DURATION_ONE_DAY);
        Board_IndicateStatus(BOARD_STATUS_LEARNMODE_ACTIVE);
        ChangeState(STATE_APP_INCLUDE_EXCLUDE);
        TimerStart(&JobTimer, 61 * 1000);
      }
    }
    else
    {
      DPRINT("Inclusion not supported when Secondary\n");
    }
  }

  // Exclusion process starting
  if (EVENT_APP_BUTTON_EXCLUDE_DEVICE == event)
  {
    if (IsPrimaryController())
    {
      DPRINT("\n===Exclusion process started===\n");

      bool ret = key_fob_start_exclusion();  // This sets state to NETWORK_MANAGEMENT_STATE_START_EXCLUSION
      if (ret == false)
      {
        DPRINT("Error, queue is full\n");
      }
      else
      {
        zpal_pm_stay_awake(m_RadioPowerLock, PM_STAY_AWAKE_DURATION_ONE_DAY);
        Board_IndicateStatus(BOARD_STATUS_LEARNMODE_ACTIVE);
        ChangeState(STATE_APP_INCLUDE_EXCLUDE);
        TimerStart(&JobTimer, 61 * 1000);
      }
    }
    else
    {
      DPRINT("Exclusion not supported when Secondary\n");
    }
  }

  if(EVENT_APP_FLUSHMEM_READY == event)
  {
    AppResetNvm();
    LoadConfiguration();
  }

  if (EVENT_APP_BUTTON_ASSOCIATION_GROUP_ADD == event)
  {
    DPRINT("\nAdd device to Group - release button and activate device nodeinformation\n");
    KeyFob_group_add_Led_handler(true);
    ChangeState(STATE_APP_ASSOCIATION);
    TimerStart(&JobTimer, 10 * 1000);
  }

  if (EVENT_APP_BUTTON_ASSOCIATION_GROUP_REMOVE == event)
  {
    DPRINT("\nRemove device from Group - release button and activate device nodeinformation\n");
    KeyFob_group_remove_Led_handler(true);
    ChangeState(STATE_APP_ASSOCIATION);
    TimerStart(&JobTimer, 10 * 1000);
  }

  // Send Basic Set ON with multicast
  if (EVENT_APP_BUTTON_BASIC_ON == event)
  {
    /* Basic Set ON to Group requested */
    DPRINT("\nBasic Set ON to Group requested\n");
    ChangeState(STATE_APP_TRANSMIT_DATA);
    zpal_pm_stay_awake(m_RadioPowerLock, PM_STAY_AWAKE_DURATION_BTN);

    if (false == ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB))
    {
      DPRINT("\n** EVENT_APP_NEXT_EVENT_JOB fail\r\n");
    }

    /*Add event's on job-queue*/
    ZAF_JobHelperJobEnqueue(EVENT_APP_SEND_BASIC_ON_JOB);
  }

  // Send Basic Set OFF with multicast
  if (EVENT_APP_BUTTON_BASIC_OFF == event)
  {
    /* Basic Set ON to Group requested */
    DPRINT("\nBasic Set OFF to Group requested\n");
    ChangeState(STATE_APP_TRANSMIT_DATA);
    zpal_pm_stay_awake(m_RadioPowerLock, PM_STAY_AWAKE_DURATION_BTN);

    if (false == ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB))
    {
      DPRINT("\n** EVENT_APP_NEXT_EVENT_JOB fail\r\n");
    }

    /*Add event's on job-queue*/
    ZAF_JobHelperJobEnqueue(EVENT_APP_SEND_BASIC_OFF_JOB);
  }

  if (EVENT_APP_BUTTON_NETWORK_LEARNMODE_NWI == event)
  {
    if (SetRFReceiveMode(true))
    {
      DPRINT("\nSet LearnMode NWI\n");
      if (false == key_fob_start_learnmode_include())
      {
        DPRINT("Error, queue is full\n");
      }
      else
      {
        KeyFob_network_learnmode_led_handler(true);
        ChangeState(STATE_APP_NETWORK_LEARNMODE);
        zpal_pm_stay_awake(m_RadioPowerLock, PM_STAY_AWAKE_DURATION_TEN_MINUTES);
        TimerStart(&JobTimer, 61 * 1000);
      }
    }
    else
    {
        DPRINT("Error, couldn't start radio\n");
    }
}

  if (EVENT_APP_BUTTON_NETWORK_LEARNMODE_NWE == event)
  {
    if (!IsPrimaryController())
    {
      if (SetRFReceiveMode(true))
      {
        DPRINT("\nSet LearnMode NWE\n");
        if (false == key_fob_start_learnmode_exclude())
        {
          DPRINT("Error, queue is full\n");
        }
        else
        {
          KeyFob_network_learnmode_led_handler(true);
          ChangeState(STATE_APP_NETWORK_LEARNMODE);
          zpal_pm_stay_awake(m_RadioPowerLock, PM_STAY_AWAKE_DURATION_TEN_MINUTES);
          TimerStart(&JobTimer, 61 * 1000);
        }
      }
      else
      {
        DPRINT("Error, couldn't start radio\n");
      }
    }
    else
    {
      DPRINT("NWE not supported when Primary Controller\n");
    }
  }
}

static void AppState_IncludeExclude(EVENT_APP event)
{
  if (EVENT_APP_BUTTON_INCLUDE_DEVICE == event)
  {
    handle_network_management_states(0);
    return;
  }

  if (EVENT_APP_BUTTON_EXCLUDE_DEVICE == event)
  {
    handle_network_management_states(0);
    return;
  }

  if(EVENT_APP_FLUSHMEM_READY == event)
  {
    AppResetNvm();
    LoadConfiguration();
  }
}

static void AppState_NetworkLearnMode(EVENT_APP event)
{
  if (EVENT_APP_BUTTON_NETWORK_LEARNMODE_NWI == event)
  {
    handle_network_management_states(0);
    return;
  }

  if(EVENT_APP_FLUSHMEM_READY == event)
  {
    AppResetNvm();
    LoadConfiguration();
  }
}

static void AppState_Association(EVENT_APP event)
{
  DPRINTF("\nSTATE_APP_ASSOCIATION (state: %d)\n", currentState);

  if (EVENT_APP_NEXT_EVENT_JOB == event)
  {
    uint8_t nextevent;
    /*check job-queue*/
    if (true == ZAF_JobHelperJobDequeue(&nextevent))
    {
      /*Let the event scheduler fire the event on state event machine*/
      ZAF_EventHelperEventEnqueue(nextevent);
    }
    else
    {
      DPRINT("  Enqueuing event: EVENT_APP_FINISH_EVENT_JOB\n");
      ZAF_EventHelperEventEnqueue(EVENT_APP_FINISH_EVENT_JOB);
    }
  }

  if (EVENT_APP_BUTTON_UP_ASSOCIATION_GROUP_ADD == event)
  {
    DPRINT("\nAdd Device to Group - Activate Device Nodeinformation\n");
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_ADD_GROUP);
    SetRFReceiveMode(true);
    zpal_pm_stay_awake(m_RadioPowerLock, 11 * 1000);
    Board_IndicatorControl(800, 200, 0, false);
    TimerStart(&JobTimer, 10 * 1000);
  }

  if (EVENT_APP_BUTTON_UP_ASSOCIATION_GROUP_REMOVE == event)
  {
    DPRINT("\nRemove Device from Group - Activate Device Nodeinformation\n");
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_REMOVE_GROUP);
    SetRFReceiveMode(true);
    zpal_pm_stay_awake(m_RadioPowerLock, 11 * 1000);
    Board_IndicatorControl(400, 600, 0, false);
    TimerStart(&JobTimer, 10 * 1000);
  }

  if (EVENT_APP_FINISH_EVENT_JOB == event
      || EVENT_APP_BUTTON_ASSOCIATION_GROUP_ADD    == event
      || EVENT_APP_BUTTON_ASSOCIATION_GROUP_REMOVE == event
	  //Since Association and Basic On/OFF shares buttons in this example application
	  //the Basic On/Off events are allowed to terminate Association.
      || EVENT_APP_BUTTON_BASIC_ON                 == event
      || EVENT_APP_BUTTON_BASIC_OFF                == event
    )
  {
    DPRINTF("\nSTATE_APP_ASSOCIATION done (state: %d)\n", currentState);
    ChangeState(STATE_APP_IDLE);
    SetRFReceiveMode(false);
    KeyFob_group_remove_Led_handler(false);
    KeyFob_group_add_Led_handler(false);
    zpal_pm_cancel(m_RadioPowerLock);
    Board_IndicateStatus(BOARD_STATUS_IDLE);
    set_new_network_management_state(NETWORK_MANAGEMENT_STATE_IDLE);
  }
}


static void AppState_TransmitData(EVENT_APP event)
{
  /**
   * This will take the queued job and put it on the event queue.
   */
  if (EVENT_APP_NEXT_EVENT_JOB == event)
  {
    uint8_t nextevent;
    /*check job-queue*/
    if (true == ZAF_JobHelperJobDequeue(&nextevent))
    {
      /*Let the event scheduler fire the event on state event machine*/
      ZAF_EventHelperEventEnqueue(nextevent);
    }
    else
    {
      DPRINT("  Enqueuing event: EVENT_APP_FINISH_EVENT_JOB\n");
      ZAF_EventHelperEventEnqueue(EVENT_APP_FINISH_EVENT_JOB);
    }
  }

  if(EVENT_APP_FLUSHMEM_READY == event)
  {
    AppResetNvm();
    LoadConfiguration();
  }

  if (EVENT_APP_SEND_BASIC_ON_JOB == event)
  {
    if (true != SendBasicSetToGroup(0xFF, NULL))
    {
      DPRINT("\n*SendBasicSetToGroup ON TX FAILED\n");
      ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
    }
    DPRINT("  EVENT_APP_SEND_BASIC_ON_JOB\n");
  }

  if (EVENT_APP_SEND_BASIC_OFF_JOB == event)
  {
    if (true != SendBasicSetToGroup(0x00, NULL))
    {
      DPRINT("\n*SendBasicSetToGroup OFF TX FAILED\n");
      ZAF_EventHelperEventEnqueue(EVENT_APP_NEXT_EVENT_JOB);
    }
    DPRINT("  EVENT_APP_SEND_BASIC_OFF_JOB\n");
  }

  if (EVENT_APP_FINISH_EVENT_JOB == event)
  {
    ChangeState(STATE_APP_IDLE);
    zpal_pm_stay_awake(m_RadioPowerLock, 10);
  }
  DPRINTF("\nSTATE_APP_TRANSMIT_DATA done (state: %d)\n", currentState);
}


static void AppStateManager(EVENT_APP event)
{
  DPRINTF("AppStateManager() - event: 0x%02x\n", event);

  /*
   * Here we handle events that are not evaluated in the context of the app state.
   */
  notAppStateDependentActivity(event);

  if (EVENT_APP_BUTTON_RESET == event)
  {
    if (IsPrimaryController())
    { 
      DPRINTF("Primary controller. Skip Device Reset Locally Notification.\n");
      SetProtocolDefault(TRANSMIT_COMPLETE_OK, NULL);
    } else {
      /* Send reset notification*/
      CC_DeviceResetLocally_notification_tx();
    }
    /*Force state change to activate system-reset without taking care of current state.*/
    ChangeState(STATE_APP_RESET);
  }

  switch(currentState)
  {
    case STATE_APP_STARTUP:
      AppState_StartUp(event);
      break;

    case STATE_APP_IDLE:  // Ready for input
      AppState_Idle(event);
      break;

    case STATE_APP_INCLUDE_EXCLUDE:
      AppState_IncludeExclude(event);
      break;

    case STATE_APP_ASSOCIATION:
      AppState_Association(event);
      break;

    case STATE_APP_TRANSMIT_DATA:
      AppState_TransmitData(event);
      break;

    case STATE_APP_NETWORK_LEARNMODE:
      AppState_NetworkLearnMode(event);
      break;

    case STATE_APP_RESET:
      if(EVENT_APP_FLUSHMEM_READY == event)
      {
        AppResetNvm();
        /* Soft reset */
        zpal_reboot();
      }
      break;

    default:
      // Do nothing.
      DPRINT("\nAppStateHandler(): Case is not handled\n");
      break;
  }
}


/**
 * Handle the next step in the iteration through all states of the inclusion process.
 * @param current_node_id
 */
static void handle_network_management_states(node_id_t current_node_id)
{
  DPRINTF("handle_network_management_states() - current_node_id: %d\n", current_node_id);

  switch (get_current_network_management_state())
  {
    case NETWORK_MANAGEMENT_STATE_START_INCLUSION:
      DPRINT("-->NETWORK_MANAGEMENT_STATE_START_INCLUSION\n");
      if (false == node_storage_is_nodeid_group_member(current_node_id))
      {
        DPRINTF("incl, node %d not in group\n", current_node_id);
      }
      key_fob_stop_inclusion();
      STOP_LEARNMODE();
      break;

    case NETWORK_MANAGEMENT_STATE_START_EXCLUSION:
      DPRINT("-->NETWORK_MANAGEMENT_STATE_START_EXCLUSION\n");
      if (true == node_storage_is_nodeid_group_member(current_node_id))
      {
        DPRINTF("excl, remove node %d from group\n", current_node_id);
        node_storage_remove_group_member_nodeid(current_node_id);
      }
      key_fob_stop_exclusion();
      STOP_LEARNMODE();
      break;

    case NETWORK_MANAGEMENT_STATE_LEARNMODE:
      DPRINTF("-->NETWORK_MANAGEMENT_STATE_LEARNMODE %u\n", current_node_id);
      zwave_network_management_abort();
      set_new_network_management_state(NETWORK_MANAGEMENT_STATE_SECURITY_PROBE);
      TimerStart(&JobTimer, 2 * 1000);
      zpal_pm_stay_awake(m_RadioPowerLock, 1000 * 10);
      break;

    case NETWORK_MANAGEMENT_STATE_IDLE:
      DPRINT("-->NETWORK_MANAGEMENT_STATE_IDLE\n");
      KeyFob_network_learnmode_led_handler(false);
      STOP_LEARNMODE();
      break;

    default:
      DPRINTF("-->Invalid network management state: %d\n", get_current_network_management_state());
      break;
  }
}

/**
 * Report current battery level to battery command class handler
 *
 * @brief See description for function prototype in CC_Battery.h
 */
uint8_t
CC_Battery_BatteryGet_handler(uint8_t endpoint)
{
  UNUSED(endpoint);
  return KeyFob_hw_get_battery_level();
}

uint32_t portable_setApplicationNodeInformation()
{
#if defined(ZW_CONTROLLER)
  ZW_UpdateCtrlNodeInformation_API_IF();
#endif
  return true;
}

/***********************************************************************
 * SECURE dependent material that was needed from SerialAPI app.
 **********************************************************************/

#define FILE_ID_APPLICATIONCONFIGURATION 104
#define FILE_SIZE_APPLICATIONCONFIGURATION (sizeof(SApplicationConfiguration))

typedef struct SApplicationConfiguration
{
  uint8_t     rfRegion;
  int8_t      iTxPower;
  int8_t      ipower0dbmMeasured;
  uint8_t     enablePTI;
  int16_t     maxTxPower;
} SApplicationConfiguration;


uint8_t
SaveApplicationTxPowerlevel(zpal_tx_power_t ipower, zpal_tx_power_t power0dbmMeasured)
{
  SApplicationConfiguration tApplicationConfiguration;
  uint8_t dataIsWritten = false;
  zpal_status_t tReturnVal;

  tReturnVal = zpal_nvm_read(pFileSystemApplication, FILE_ID_APPLICATIONCONFIGURATION, &tApplicationConfiguration, FILE_SIZE_APPLICATIONCONFIGURATION);
  if (ZPAL_STATUS_OK == tReturnVal)
  {
    tApplicationConfiguration.iTxPower = ipower;
    tApplicationConfiguration.ipower0dbmMeasured = power0dbmMeasured;
    tReturnVal = zpal_nvm_write(pFileSystemApplication, FILE_ID_APPLICATIONCONFIGURATION, &tApplicationConfiguration, FILE_SIZE_APPLICATIONCONFIGURATION);
    if (ZPAL_STATUS_OK == tReturnVal)
    {
      dataIsWritten = true;
    }
  }
  return dataIsWritten;
}

static
bool ObjectExist(zpal_nvm_object_key_t key)
{
  size_t tDataLen;
  return ZPAL_STATUS_OK == zpal_nvm_get_object_size(pFileSystemApplication, key, &tDataLen);
}

uint8_t
ReadApplicationRfRegion(uint8_t * rfRegion)
{
  SApplicationConfiguration tApplicationConfiguration;
  uint8_t dataIsRead = false;
  zpal_status_t tReturnVal;

  if (ObjectExist(FILE_ID_APPLICATIONCONFIGURATION))
  {
    tReturnVal = zpal_nvm_read(pFileSystemApplication, FILE_ID_APPLICATIONCONFIGURATION, &tApplicationConfiguration, FILE_SIZE_APPLICATIONCONFIGURATION);
    if (ZPAL_STATUS_OK == tReturnVal)
    {
      *rfRegion = tApplicationConfiguration.rfRegion;
      dataIsRead = true;
    }
  }
  return dataIsRead;
}

ZW_WEAK void KeyFob_hw_init(EResetReason_t reset_reason)
{
  (void)reset_reason;
}

ZW_WEAK void KeyFob_hw_deep_sleep_wakeup_handler(void)
{
}

ZW_WEAK void KeyFob_basic_on_Led_handler(bool ledOn)
{
  (void)ledOn;
}

ZW_WEAK void KeyFob_basic_off_Led_handler(bool ledOn)
{
  (void)ledOn;
}

ZW_WEAK void KeyFob_group_add_Led_handler(bool ledOn)
{
  (void)ledOn;
}

ZW_WEAK void KeyFob_group_remove_Led_handler(bool ledOn)
{
  (void)ledOn;
}

ZW_WEAK void KeyFob_node_add_remove_Led_handler(bool ledOn)
{
  (void)ledOn;
}

ZW_WEAK void KeyFob_network_learnmode_led_handler(bool ledOn)
{
  (void)ledOn;
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
