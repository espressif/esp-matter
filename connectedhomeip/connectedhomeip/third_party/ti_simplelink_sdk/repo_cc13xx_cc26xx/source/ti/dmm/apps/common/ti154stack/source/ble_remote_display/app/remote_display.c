/******************************************************************************

 @file  remote_display.c

 @brief This file contains the 15.4 Remote Display sample application for use
        with the CC13x2 Bluetooth Low Energy Protocol Stack.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2021, Texas Instruments Incorporated
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
#if defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE)
#include <stdio.h>
#endif /* defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE) */

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>

#include <ti/display/Display.h>

#if !(defined __TI_COMPILER_VERSION__) && !(defined __clang__)
#include <intrinsics.h>
#endif

#include <ti/drivers/utils/List.h>

#include <icall.h>
#include "util.h"
#include <bcomdef.h>
/* This Header file contains all BLE API and icall structure definition */
#include <icall_ble_api.h>

#include <devinfoservice.h>
#include <remote_display_gatt_profile.h>
#include <provisioning_gatt_profile.h>

#ifdef USE_RCOSC
#include <rcosc_calibration.h>
#endif //USE_RCOSC

#ifndef CUI_DISABLE
#include "cui.h"
#endif /* CUI_DISABLE */

#include "util.h"
#include "remote_display.h"

#ifdef DMM_SENSOR
#include "ssf.h"
#include "sensor.h"
#ifdef LPSTK
#include "lpstk/lpstk.h"
#endif /* LPSTK */
#elif DMM_COLLECTOR
#include <network_device_gatt_profile.h>
#include "csf.h"
#else
#error "Either DMM_SENSOR or DMM_COLLECTOR must be defined"
#endif

#ifdef DMM_OAD
#include <profiles/oad/cc26xx/oad.h>
#include <ti/common/cc26xx/oad/oad_image_header.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#endif

// Syscfg Generated Files
#include "ti_drivers_config.h"
#include "ti_dmm_application_policy.h"

#ifdef FEATURE_SECURE_COMMISSIONING
#include <sm_commissioning_gatt_profile.h>
#endif /* FEATURE_SECURE_COMMISSIONING */

/*********************************************************************
 * MACROS
 */

// Connection indices map to events by shifting 1 left by the index
#define CONN_INDEX_TO_EVENT(x)             (1 << x)

// Count the trailing zeros to map back from event to index
#if defined __TI_COMPILER_VERSION__
#define CONN_EVENT_TO_INDEX(x) (sizeof(unsigned int) * 8 - 1 - __clz((unsigned int) x))
#else
#define CONN_EVENT_TO_INDEX(x) (sizeof(unsigned int) * 8 - 1 - __CLZ((unsigned int) x))
#endif

/*********************************************************************
 * CONSTANTS
 */
#ifdef DISPLAY_PER_STATS
// How often to perform PER read event (in ms)
#define RD_PER_EVT_PERIOD                    1000
#endif /* DISPLAY_PER_STATS */

// How often to read current current RPA (in ms)
#define RD_READ_RPA_EVT_PERIOD               3000

// Task configuration
#define RD_TASK_PRIORITY                     1

#ifndef RD_TASK_STACK_SIZE
#define RD_TASK_STACK_SIZE                   2048
#endif

// Application events
#define RD_STATE_CHANGE_EVT                  0
#define RD_CHAR_CHANGE_EVT                   1
#define PROV_CHAR_CHANGE_EVT                 2
#define RD_KEY_CHANGE_EVT                    3
#define RD_ADV_EVT                           4
#define RD_PAIR_STATE_EVT                    5
#define RD_PASSCODE_EVT                      6
#define RD_READ_RPA_EVT                      7
#define RD_SEND_PARAM_UPDATE_EVT             8
#define RD_UPDATE_PROV_SENSOR_EVT            9
#define RD_UPDATE_PROV_STATE_EVT             10
#define RD_UPDATE_RD_DATA_EVT                11
#define RD_POLICY_PAUSE_EVT                  12
#define RD_UI_INPUT_EVT                      13
#define NETWORK_DEVICE_CHAR_CHANGE_EVT       14
#define RD_UPDATE_DEVICE_UPDATE_EVT          15
#define RD_CONN_EVT                          16
#define SM_CHAR_CHANGE_EVT                   17
#define SM_SENSOR_STATE_CHANGE_EVT           18

#ifdef DISPLAY_PER_STATS
#define RD_UPDATE_PER_EVT                    19
#define RD_PER_RESET_EVT                     20
#define RD_PER_READ_EVT                      21
#endif /* DISPLAY_PER_STATS */

#if defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE)
#define RD_BLOCK_MODE_EVT                    22
#endif /*  defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE) */

// Internal Events for RTOS application
#define RD_ICALL_EVT                         ICALL_MSG_EVENT_ID // Event_Id_31
#define RD_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30

// Bitwise OR of all RTOS events to pend on
#ifndef DMM_OAD
#define RD_ALL_EVENTS                        (RD_ICALL_EVT             | \
                                              RD_QUEUE_EVT)
#else
#define RD_ALL_EVENTS                        (RD_ICALL_EVT             | \
                                              RD_QUEUE_EVT             | \
                                              OAD_QUEUE_EVT            | \
                                              OAD_DL_COMPLETE_EVT)
#endif /* DMM_OAD */

// Size of string-converted device address ("0xXXXXXXXXXXXX")
#define RD_ADDR_STR_SIZE     15

// Row numbers for display
#define RD_ROW_TITLE            0
#define RD_ROW_SEPARATOR_1      1
#define RD_ROW_STATUS_1         2
#define RD_ROW_STATUS_2         3
#define RD_ROW_CONNECTION       4
#define RD_ROW_ADVSTATE         5
#define RD_ROW_IDA              6
#define RD_ROW_RPA              7
#define RD_ROW_WSN_DATA         8
#define RD_ROW_SENSOR_STATS_1   9
#define RD_ROW_SENSOR_STATS_2   10
#define RD_ROW_SENSOR_STATS_3   11
#define RD_ROW_DEBUG            12

// For storing the active connections
#define RD_RSSI_TRACK_CHNLS        1            // Max possible channels can be GAP_BONDINGS_MAX
#define RD_MAX_RSSI_STORE_DEPTH    5
#define RD_INVALID_HANDLE          0xFFFF
#define RSSI_2M_THRSHLD           -30
#define RSSI_1M_THRSHLD           -40
#define RSSI_S2_THRSHLD           -50
#define RSSI_S8_THRSHLD           -60
#define RD_PHY_NONE                LL_PHY_NONE  // No PHY set
#define AUTO_PHY_UPDATE            0xFF

// Spin if the expression is not true
#define REMOTEDISPLAY_ASSERT(expr) if (!(expr)) remote_display_spin();

#ifdef DMM_OAD
#define PROGRESS_WIDTH 30
/* Must be PROGRESS_WIDTH hashes */
#define HASHES_STR "##############################"
/* Must be PROGRESS_WITDH spaces */
#define SPACES_STR "                              "
#endif

/*********************************************************************
 * TYPEDEFS
 */

// App event passed from stack modules. This type is defined by the application
// since it can queue events to itself however it wants.
typedef struct
{
  uint8_t event;                // event type
  void    *pData;               // pointer to message
} rdEvt_t;

// Container to store passcode data when passing from gapbondmgr callback
// to app event. See the pfnPairStateCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct
{
  uint8_t state;
  uint16_t connHandle;
  uint8_t status;
} rdPairStateData_t;

// Container to store passcode data when passing from gapbondmgr callback
// to app event. See the pfnPasscodeCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct
{
  uint8_t deviceAddr[B_ADDR_LEN];
  uint16_t connHandle;
  uint8_t uiInputs;
  uint8_t uiOutputs;
  uint32_t numComparison;
} rdPasscodeData_t;

// Container to store advertising event data when passing from advertising
// callback to app event. See the respective event in GapAdvScan_Event_IDs
// in gap_advertiser.h for the type that pBuf should be cast to.
typedef struct
{
  uint32_t event;
  void *pBuf;
} rdGapAdvEventData_t;

// Container to store information from clock expiration using a flexible array
// since data is not always needed
typedef struct
{
  uint8_t event;
  uint8_t data[];
} rdClockEventData_t;

// List element for parameter update and PHY command status lists
typedef struct
{
  List_Elem elem;
  uint16_t connHandle;
} rdConnHandleEntry_t;

// Connected device information
typedef struct
{
  uint16_t              connHandle;                        // Connection Handle
  rdClockEventData_t*   pParamUpdateEventData;
  Clock_Struct*         pUpdateClock;                      // pointer to clock struct
  int8_t                rssiArr[RD_MAX_RSSI_STORE_DEPTH];
  uint8_t               rssiCntr;
  int8_t                rssiAvg;
  bool                  phyCngRq;                          // Set to true if PHY change request is in progress
  uint8_t               currPhy;
  uint8_t               rqPhy;
  uint8_t               phyRqFailCnt;                      // PHY change request count
  bool                  isAutoPHYEnable;                   // Flag to indicate auto phy change
#ifdef FEATURE_SECURE_COMMISSIONING
  bool                  bondedConnection;                  // Flag to indicate that the current connection is bonded
  bool                  smAccessAllowed;                   // Flag to determine if the current connection will have access to the SM Service
#endif /* FEATURE_SECURE_COMMISSIONING */
} rdConnRec_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Task configuration
Task_Struct rdTask;
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(rdTaskStack, 8)
#else
#pragma data_alignment=8
#endif
uint8_t rdTaskStack[RD_TASK_STACK_SIZE];

/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent;

// Queue object used for app messages
static Queue_Struct appMsgQueue;
static Queue_Handle appMsgQueueHandle;

// Clock instance for RPA read events.
static Clock_Struct clkRpaRead;
#ifdef DISPLAY_PER_STATS
// Clock instance for PER update events.
static Clock_Struct clkReadPer;
#endif /* DISPLAY_PER_STATS */

#if defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE)
static Clock_Struct clkBlockModeTest;

// Memory to pass PER event ID to clock handler
rdClockEventData_t argBlockModeTest =
{ .event = RD_BLOCK_MODE_EVT };
#endif /* defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE) */

// Memory to pass RPA read event ID to clock handler
rdClockEventData_t argRpaRead =
{ .event = RD_READ_RPA_EVT };

#ifdef DISPLAY_PER_STATS
// Memory to pass PER event ID to clock handler
rdClockEventData_t argReadPer =
{ .event = RD_PER_READ_EVT };
#endif /* DISPLAY_PER_STATS */

// Per-handle connection info
static rdConnRec_t connList[MAX_NUM_BLE_CONNS];

// List to store connection handles for queued param updates
static List_List paramUpdateList;

// Advertising handles
static uint8 advHandleLegacy;

// Address mode
static GAP_Addr_Modes_t addrMode = DEFAULT_ADDRESS_MODE;

// Current Random Private Address
static uint8 rpa[B_ADDR_LEN] = {0};

static RemoteDisplay_clientProvisioningtCbs_t clientProvCbs;
static RemoteDisplayCbs_t remoteDisplayCbs;

#ifdef DMM_COLLECTOR
static RemoteDisplay_networkDeviceCb_t networkDeviceCb;
#endif

static bool dmmPolicyBlePaused = false;

#ifndef CUI_DISABLE
CUI_clientHandle_t remoteDisplayCuiHndl;
uint32_t rdStatusLineStatus1;
uint32_t rdStatusLineStatus2;
uint32_t rdStatusLineConnection;
uint32_t rdStatusLineAdvState;
uint32_t rdStatusLineIda;
uint32_t rdStatusLineRpa;
uint32_t rdStatusLinePhyStatus;
uint32_t rdStatusLineRssiStatus;
uint32_t rdStatusLineCharStats;

#ifdef DISPLAY_PER_STATS
uint32_t rdStatusLinePerStats;
#endif
#ifdef BLOCK_MODE_TEST
uint32_t rdStatusLineBlockModeTestState;

static uint16_t RD_BLOCK_MODE_ON_PERIOD  = 0x01F4;    // Default 500  ms
static uint16_t RD_BLOCK_MODE_OFF_PERIOD = 0x03E8;    // Default 1000 ms
#endif
#endif /* CUI_DISABLE */

#ifdef DMM_OAD
#ifndef CUI_DISABLE
static uint32_t rdStatusLineOad;
#endif /* CUI_DISABLE */
static uint8_t numPendingMsgs = 0;
static bool oadWaitReboot = false;
#endif

#if defined(LPSTK) && defined(DMM_SENSOR)
// Flag to indicate that OAD is in progress
static bool oad_in_progress = FALSE;
#endif /* LPSTK */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void RemoteDisplay_init( void );
static void RemoteDisplay_taskFxn(UArg a0, UArg a1);
static uint8_t RemoteDisplay_processStackMsg(ICall_Hdr *pMsg);
static uint8_t RemoteDisplay_processGATTMsg(gattMsgEvent_t *pMsg);
static void RemoteDisplay_processGapMessage(gapEventHdr_t *pMsg);
static void RemoteDisplay_advCallback(uint32_t event, void *pBuf, uintptr_t arg);
static void  RemoteDisplay_processAdvEvent(rdGapAdvEventData_t *pEventData);
static void RemoteDisplay_processAppMsg(rdEvt_t *pMsg);
static void RemoteDisplay_processRDCharValueChangeEvt(uint8_t paramId);
#ifdef DMM_COLLECTOR
static void RemoteDisplay_processNetworkDeviceCharValueChangeEvt(uint8_t paramId);
#endif
static void RemoteDisplay_processProvCharValueChangeEvt(uint8_t paramId);
static void RemoteDisplay_updateRPA(void);
static void RemoteDisplay_clockHandler(UArg arg);
static void RemoteDisplay_passcodeCb(uint8_t *pDeviceAddr, uint16_t connHandle,
                                        uint8_t uiInputs, uint8_t uiOutputs,
                                        uint32_t numComparison);
static void RemoteDisplay_pairStateCb(uint16_t connHandle, uint8_t state,
                                         uint8_t status);
static void RemoteDisplay_processPairState(rdPairStateData_t *pPairState);
static void RemoteDisplay_processPasscode(rdPasscodeData_t *pPasscodeData);
static void RemoteDisplay_charValueChangeCB(uint8_t paramId);
#ifdef FEATURE_SECURE_COMMISSIONING
static void RemoteDisplay_processSmCharValueChangeEvt(uint8_t paramId);
static void SmCommissioning_charValueChangeCB(uint8_t paramId);
static void RemoteDisplay_updateConnBondState(uint16_t connectionHandle, bool status);
#endif /* FEATURE_SECURE_COMMISSIONING */
#ifdef DMM_COLLECTOR
static void NetworkDevice_charValueChangeCB(uint8_t paramId);
#endif /* DMM_COLLECTOR */
static void Provisioning_charValueChangeCB(uint8_t paramId);
static status_t RemoteDisplay_enqueueMsg(uint8_t event, void *pData);
static uint8_t RemoteDisplay_addConn(uint16_t connHandle);
static uint8_t RemoteDisplay_getConnIndex(uint16_t connHandle);
static void RemoteDisplay_clearPendingParamUpdate(uint16_t connHandle);
static uint8_t RemoteDisplay_removeConn(uint16_t connHandle);
static void RemoteDisplay_processParamUpdate(uint16_t connHandle);
static uint8_t RemoteDisplay_clearConnListEntry(uint16_t connHandle);
static void RemoteDisplay_blePausePolicyCb(uint16_t pause);
static void RemoteDisplay_syncProvAttr(void);
static void RemoteDisplay_syncRDAttr(void);
#ifdef DMM_COLLECTOR
static void RemoteDisplay_syncNetworkDevices(uint16_t devAddr);
#endif
#ifdef DISPLAY_PER_STATS
static void RemoteDisplay_processUpdatePerEvt(void);
static void RemoteDisplay_processVSCmdCompleteEvt(hciEvt_VSCmdComplete_t *pMsg);
#endif /* DISPLAY_PER_STATS */

#ifndef CUI_DISABLE
static void RemoteDisplay_processMenuUpdate(void);
#endif /* CUI_DISABLE */

// BLE5 PHY Switching Functions
static void RemoteDisplay_connEvtCB(Gap_ConnEventRpt_t *pReport);
bool RemoteDisplay_doSetConnPhy(uint8_t index);
static void RemoteDisplay_processCmdCompleteEvt(hciEvt_CmdComplete_t *pMsg);
static void RemoteDisplay_initPHYRSSIArray(void);
static status_t RemoteDisplay_startAutoPhyChange(uint16_t connHandle);
static status_t RemoteDisplay_stopAutoPhyChange(uint16_t connHandle);
static status_t RemoteDisplay_setPhy(uint16_t connHandle, uint8_t allPhys,
                                        uint8_t txPhy, uint8_t rxPhy,
                                        uint16_t phyOpts);
static void RemoteDisplay_updatePHYStat(uint16_t eventCode, uint8_t *pMsg);
#ifndef CUI_DISABLE
static void RemoteDisplay_cyclePhy(int32_t menuEntryIndex);
#endif /* CUI_DISABLE */

#if defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE)
static void RemoteDisplay_blockModeTestOn(int32_t menuEntryIndex);
static void RemoteDisplay_blockModeTestOff(int32_t menuEntryIndex);
static void RemoteDisplay_setBlockModeOnPeriodAction(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo);
static void RemoteDisplay_setBlockModeOffPeriodAction(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo);
static void RemoteDisplay_setBlockModePeriodUiAction(uint16_t* blockModePeriod, const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo);
static uint8_t moveCursorLeft(uint8_t col, uint8_t left_boundary, uint8_t right_boundary, uint8_t skip_space);
static uint8_t moveCursorRight(uint8_t col, uint8_t left_boundary, uint8_t right_boundary, uint8_t skip_space);
#endif /* defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE) */

#ifdef DMM_OAD
static void RemoteDisplay_processOadWriteCB(uint8_t event, uint16_t arg);
static void RemoteDisplay_processL2CAPMsg(l2capSignalEvent_t *pMsg);
#endif
static void RemoteDisplay_processConnEvt(Gap_ConnEventRpt_t *pReport);

// List to store connection handles for set phy command status's
static List_List setPhyCommStatList;

#ifndef CUI_DISABLE
#define RD_MENU_TITLE " TI Remote Display "

CUI_SUB_MENU(rdSetPhySubMenu, "<     SET PHY    >", 6, rdConfigureSubMenu)
    CUI_MENU_ITEM_ACTION(  "<      1M        >", RemoteDisplay_cyclePhy)
    CUI_MENU_ITEM_ACTION(  "<      2M        >", RemoteDisplay_cyclePhy)
    CUI_MENU_ITEM_ACTION(  "<     1M|2M      >", RemoteDisplay_cyclePhy)
    CUI_MENU_ITEM_ACTION(  "<     CODED      >", RemoteDisplay_cyclePhy)
    CUI_MENU_ITEM_ACTION(  "<   1M|2M|CODED  >", RemoteDisplay_cyclePhy)
    CUI_MENU_ITEM_ACTION(  "<      AUTO      >", RemoteDisplay_cyclePhy)
CUI_SUB_MENU_END

#ifdef BLOCK_MODE_TEST
CUI_SUB_MENU(rdBlockModeTestSubMenu,"< BLOCK MODE TEST >", 4, remoteDisplayMainMenu)
    CUI_MENU_ITEM_ACTION(           "<  BM TEST ON    >", RemoteDisplay_blockModeTestOn)
    CUI_MENU_ITEM_ACTION(           "<  BM TEST OFF   >", RemoteDisplay_blockModeTestOff)
    CUI_MENU_ITEM_INT_ACTION(       "<  ON  VAL (ms)  >", RemoteDisplay_setBlockModeOnPeriodAction)
    CUI_MENU_ITEM_INT_ACTION(       "<  OFF VAL (ms)  >", RemoteDisplay_setBlockModeOffPeriodAction)
CUI_SUB_MENU_END
#endif

CUI_SUB_MENU(rdConfigureSubMenu, "<    CONFIGURE   >", 1, remoteDisplayMainMenu)
    CUI_MENU_ITEM_SUBMENU(rdSetPhySubMenu)
CUI_SUB_MENU_END

#ifdef BLOCK_MODE_TEST
CUI_MAIN_MENU(remoteDisplayMainMenu, RD_MENU_TITLE, 2, RemoteDisplay_processMenuUpdate)
#else
CUI_MAIN_MENU(remoteDisplayMainMenu, RD_MENU_TITLE, 1, RemoteDisplay_processMenuUpdate)
#endif
    CUI_MENU_ITEM_SUBMENU(rdConfigureSubMenu)
#ifdef BLOCK_MODE_TEST
    CUI_MENU_ITEM_SUBMENU(rdBlockModeTestSubMenu)
#endif
CUI_MAIN_MENU_END
#endif /* CUI_DISABLE */

/*********************************************************************
 * EXTERN FUNCTIONS
 */
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Bond Manager Callbacks
static gapBondCBs_t RemoteDisplay_BondMgrCBs =
{
  RemoteDisplay_passcodeCb,       // Passcode callback
  RemoteDisplay_pairStateCb       // Pairing/Bonding state Callback
};

// Provisioning Profile Callbacks
static provisioningProfileCBs_t Provisioning_ProfileCBs =
{
  Provisioning_charValueChangeCB // Provisioning GATT Characteristic value change callback
};

// Remote Display GATT Profile Callbacks
static remoteDisplayProfileCBs_t RemoteDisplay_ProfileCBs =
{
  RemoteDisplay_charValueChangeCB // Remote Display GATT Characteristic value change callback
};

#ifdef FEATURE_SECURE_COMMISSIONING
// SM Commissioning GATT Profile Callbacks
static sm_commissioning_profileCBs_t SmCommissioning_ProfileCBs =
{
  SmCommissioning_charValueChangeCB // SM Commissioning GATT Characteristic value change callback
};
#endif /* FEATURE_SECURE_COMMISSIONING */

#ifdef DMM_COLLECTOR
// Network Device Profile Callbacks
static networkDeviceProfileCBs_t networkDeviceProfileCBs =
{
  NetworkDevice_charValueChangeCB // Provisioning GATT Characteristic value change callback
};
#endif /* DMM_COLLECTOR */

#ifdef DMM_OAD
// OAD Service Callbacks
static oadTargetCBs_t RemoteDisplay_oadCBs =
{
  .pfnOadWrite = RemoteDisplay_processOadWriteCB // Write Callback.
};
#endif

/*********************************************************************
 * DMM Policy Callbacks
 */
static DMMPolicy_AppCbs_t dmmPolicyAppCBs =
{
    RemoteDisplay_blePausePolicyCb
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      remote_display_spin
 *
 * @brief   Spin forever
 *
 * @param   none
 */
static void remote_display_spin(void)
{
  volatile uint8_t x = 0;

  while(1)
  {
    x++;
  }
}


/*********************************************************************
 * @fn      RemoteDisplay_registerClientProvCbs
 *
 * @brief   Register the 15.4 provisioning client callbacks
 */
void RemoteDisplay_registerClientProvCbs(RemoteDisplay_clientProvisioningtCbs_t clientCbs)
{
    clientProvCbs = clientCbs;
}

/*********************************************************************
 * @fn      RemoteDisplay_registerRDCbs
 *
 * @brief   Register the 15.4 Remote display callbacks
 */
void RemoteDisplay_registerRDCbs(RemoteDisplayCbs_t rdCbs)
{
    remoteDisplayCbs = rdCbs;
}

#ifdef DMM_COLLECTOR
/*********************************************************************
 * @fn RemoteDisplay_registerNetworkDeviceCb
 * @brief Register the clients network device callback
 */
void RemoteDisplay_registerNetworkDeviceCb(RemoteDisplay_networkDeviceCb_t ndCb)
{
    networkDeviceCb = ndCb;
}
#endif

/*********************************************************************
 * @fn      RemoteDisplay_createTask
 *
 * @brief   Task creation function for the Remote Display.
 */
void RemoteDisplay_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = rdTaskStack;
  taskParams.stackSize = RD_TASK_STACK_SIZE;
  taskParams.priority = RD_TASK_PRIORITY;

  Task_construct(&rdTask, RemoteDisplay_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 *  @fn      RemoteDisplay_updateSensorJoinState/RemoteDisplay_updateCollectorJoinState
 *
 * @brief   Updates the collector state value within the provisioning profile
 */
#ifdef DMM_SENSOR
void RemoteDisplay_updateSensorJoinState(Jdllc_states_t state)
{
    uint8_t *pValue = ICall_malloc(sizeof(Jdllc_states_t));
#elif DMM_COLLECTOR
void RemoteDisplay_updateCollectorJoinState(Cllc_states_t state)
{
    uint8_t *pValue = ICall_malloc(sizeof(Cllc_states_t));
#endif
    if (pValue)
    {
      *pValue = (uint8_t) state;
      if (RemoteDisplay_enqueueMsg(RD_UPDATE_PROV_STATE_EVT, pValue) != SUCCESS)
      {
        ICall_free(pValue);
      }
    }
}

/*********************************************************************
 *  @fn      RemoteDisplay_updateSensorData
 *
 * @brief   Updates the sensor data within the remote display profile
 */
void RemoteDisplay_updateSensorData(void)
{
  RemoteDisplay_enqueueMsg(RD_UPDATE_RD_DATA_EVT, NULL);
}


/*********************************************************************
 *  @fn      RemoteDisplay_syncRDAttr
 *
 * @brief   Helper function to initialize all provisioning profile characteristic
 *          values based on the 15.4 application.
 */
void RemoteDisplay_syncRDAttr(void)
{
    // Callback has not been properly initialized
    if (!remoteDisplayCbs.getRDAttrCb) {
        return;
    }

#ifdef DMM_SENSOR
    uint8_t newSensorData[RDPROFILE_SENSOR_DATA_LEN];
    // Data from 15.4 Application
    remoteDisplayCbs.getRDAttrCb(RemoteDisplayAttr_SensorData, newSensorData,
        RDPROFILE_SENSOR_DATA_LEN);
    // Write data to BLE remote display profile
    RemoteDisplay_SetParameter(RDPROFILE_SENSOR_DATA_CHAR, RDPROFILE_SENSOR_DATA_LEN,
                                   newSensorData);
#elif DMM_COLLECTOR
    uint8_t newSensorAddr[RDPROFILE_SENSOR_ADDR_LEN];
    // Data from 15.4 Application
    remoteDisplayCbs.getRDAttrCb(RemoteDisplayAttr_SensorAddr, newSensorAddr,
        RDPROFILE_SENSOR_ADDR_LEN);
    // Write data to BLE remote display profile
    RemoteDisplay_SetParameter(RDPROFILE_SENSOR_ADDR_CHAR, RDPROFILE_SENSOR_ADDR_LEN,
                                   newSensorAddr);
#endif
#ifndef DMM_COLLECTOR
    uint8_t newReportInterval[RDPROFILE_SENSOR_REPORT_INTERVAL_LEN];
    // Data from 15.4 Application
    remoteDisplayCbs.getRDAttrCb(RemoteDisplayAttr_ReportInterval, newReportInterval,
        RDPROFILE_SENSOR_REPORT_INTERVAL_LEN);
    // Write data to BLE remote display profile
        RemoteDisplay_SetParameter(RDPROFILE_SENSOR_REPORT_INTERVAL_CHAR, RDPROFILE_SENSOR_REPORT_INTERVAL_LEN,
                                   newReportInterval);
#endif
}

#ifdef DMM_COLLECTOR
/*********************************************************************
 *  @fn      RemoteDisplay_deviceUpdate
 *
 * @brief Update device data
 */
void RemoteDisplay_deviceUpdate(uint16_t devAddr)
{
    uint16_t *pValue = ICall_malloc(sizeof(uint16_t));
    if (pValue)
    {
      *pValue = devAddr;
      if (RemoteDisplay_enqueueMsg(RD_UPDATE_DEVICE_UPDATE_EVT, pValue) != SUCCESS)
      {
        ICall_free(pValue);
      }
    }
}

/*********************************************************************
 *  @fn      RemoteDisplay_syncNetworkDevices
 *
 * @brief   Helper function to initialize all provisioning profile characteristic
 *          values based on the 15.4 application.
 */
void RemoteDisplay_syncNetworkDevices(uint16_t devAddr)
{
    uint8_t deviceData[NETDEVPROFILE_NETWORK_DEVICE_CHAR_LEN];
    union RemoteDisplay_DeviceInfo_t devInfo;
    // Data from 15.4 Application
    if(networkDeviceCb)
    {
        networkDeviceCb(devAddr, &devInfo);
    }

    // Only send notification if devAddr is valid
    if(devInfo.ti15_4StackDeviceInfo.devAddr != 0xFFFF)
    {
        deviceData[0] = (devInfo.ti15_4StackDeviceInfo.devAddr & 0xFF00) >> 8;
        deviceData[1] = devInfo.ti15_4StackDeviceInfo.devAddr & 0xFF;
        deviceData[2] = (devInfo.ti15_4StackDeviceInfo.parentAddr & 0xFF00) >> 8;
        deviceData[3] = devInfo.ti15_4StackDeviceInfo.parentAddr & 0xFF;
        deviceData[4] = devInfo.ti15_4StackDeviceInfo.sensorData;
        deviceData[5] = devInfo.ti15_4StackDeviceInfo.rssi;
        deviceData[6] = devInfo.ti15_4StackDeviceInfo.deviceStatus;

        // Write data to BLE device char
        NetworkDeviceProfile_SetParameter(NETDEVPROFILE_DEVICE_CHAR, NETDEVPROFILE_NETWORK_DEVICE_CHAR_LEN,
                                   deviceData);
    }
}
#endif

/*********************************************************************
 *  @fn      RemoteDisplay_syncProvAttr
 *
 * @brief   Helper function to initialize all provisioning profile characteristic
 *          values based on the 15.4 application.
 */
void RemoteDisplay_syncProvAttr(void)
{
    uint8_t newGenericAttr;
    uint8_t newPanId[PROVPROFILE_NTWK_PAN_ID_CHAR_LEN];
    uint8_t newExtPanId[PROVPROFILE_EXT_NTWK_PAN_ID_CHAR_LEN];
    uint8_t newChannMask[PROVPROFILE_SENSOR_CHANNEL_CHAR_LEN];
    uint8_t newFfdAddr[PROVPROFILE_IEEE_FFD_ADDR_CHAR_LEN];
    uint8_t newNtwkKey[PROVPROFILE_NTWK_KEY_CHAR_LEN];

    // Callback has not been properly initialized
    if (!clientProvCbs.getProvisioningAttrCb) {
        return;
    }

    // Data from 15.4 Application
    clientProvCbs.getProvisioningAttrCb(ProvisionAttr_Freq, &newGenericAttr,
        PROVISIONING_GENERIC_CHAR_LEN);
    // Write data to BLE provisioning profile
    ProvisioningProfile_SetParameter(PROVPROFILE_SENSOR_FREQ_CHAR, sizeof(uint8_t),
                                         &newGenericAttr);

    clientProvCbs.getProvisioningAttrCb(ProvisionAttr_PanId, newPanId,
        PROVPROFILE_NTWK_PAN_ID_CHAR_LEN);
    // Write data to BLE provisioning profile
    ProvisioningProfile_SetParameter(PROVPROFILE_NTWK_PAN_ID_CHAR, PROVPROFILE_NTWK_PAN_ID_CHAR_LEN,
                                         newPanId);

    clientProvCbs.getProvisioningAttrCb(ProvisionAttr_ExtPanId, newExtPanId,
        PROVPROFILE_EXT_NTWK_PAN_ID_CHAR_LEN);
    // Write data to BLE provisioning profile
    ProvisioningProfile_SetParameter(PROVPROFILE_EXT_NTWK_PAN_ID_CHAR, PROVPROFILE_EXT_NTWK_PAN_ID_CHAR_LEN,
                                         newExtPanId);

    clientProvCbs.getProvisioningAttrCb(ProvisionAttr_SensorChannelMask, newChannMask,
        PROVPROFILE_SENSOR_CHANNEL_CHAR_LEN);
    // Write data to BLE provisioning profile
    ProvisioningProfile_SetParameter(PROVPROFILE_SENSOR_CHANNEL_CHAR, PROVPROFILE_SENSOR_CHANNEL_CHAR_LEN,
                                         newChannMask);

    clientProvCbs.getProvisioningAttrCb(ProvisionAttr_FFDAddr, newFfdAddr,
        PROVPROFILE_IEEE_FFD_ADDR_CHAR_LEN);
    // Write data to BLE provisioning profile
    ProvisioningProfile_SetParameter(PROVPROFILE_IEEE_FFD_ADDR_CHAR, PROVPROFILE_IEEE_FFD_ADDR_CHAR_LEN,
                                         newFfdAddr);

    clientProvCbs.getProvisioningAttrCb(ProvisionAttr_NtwkKey, newNtwkKey,
        PROVPROFILE_NTWK_KEY_CHAR_LEN);
    // Write data to BLE provisioning profile
    ProvisioningProfile_SetParameter(PROVPROFILE_NTWK_KEY_CHAR, PROVPROFILE_NTWK_KEY_CHAR_LEN,
                                         newNtwkKey);

    clientProvCbs.getProvisioningAttrCb(ProvisionAttr_ProvState, &newGenericAttr,
        PROVISIONING_GENERIC_CHAR_LEN);
    // Write data to BLE provisioning profile
    ProvisioningProfile_SetParameter(PROVPROFILE_PROV_STATE_CHAR, sizeof(uint8_t),
                                         &newGenericAttr);
}

/*********************************************************************
 * @fn      RemoteDisplay_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 */
static void RemoteDisplay_init(void)
{
  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);

#ifdef USE_RCOSC
  RCOSC_enableCalibration();
#endif // USE_RCOSC

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueueHandle = Util_constructQueue(&appMsgQueue);

#ifdef DISPLAY_PER_STATS
  // Construct the clock that controls the PER display period
  Util_constructClock(&clkReadPer, RemoteDisplay_clockHandler,
                      RD_PER_EVT_PERIOD, 0 , false, (UArg)&argReadPer);
#endif /* DISPLAY_PER_STATS */

#if defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE)
  Util_constructClock(&clkBlockModeTest,
                    RemoteDisplay_clockHandler,
                    RD_BLOCK_MODE_OFF_PERIOD, RD_BLOCK_MODE_OFF_PERIOD, false,
                    (UArg)&argBlockModeTest);
#endif /* defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE) */

  // Set the Device Name characteristic in the GAP GATT Service
  // For more information, see the section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

  // Configure GAP
  {
    uint16_t paramUpdateDecision = DEFAULT_PARAM_UPDATE_REQ_DECISION;

    // Pass all parameter update requests to the app for it to decide
    GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, paramUpdateDecision);
  }

  // Setup the GAP Bond Manager. For more information see the GAP Bond Manager
  // section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  setBondManagerParameters();

  // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);           // GAP GATT Service
  GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT Service
  DevInfo_AddService();                        // Device Information Service
  RemoteDisplay_AddService(GATT_ALL_SERVICES); // Remote Display GATT Profile
  #ifdef DMM_COLLECTOR
  NetworkDeviceProfile_AddService(GATT_ALL_SERVICES); // Network DEvice GATT Profile
  #endif
  ProvisioningProfile_AddService(GATT_ALL_SERVICES); // Provisioning GATT Profile
#ifdef FEATURE_SECURE_COMMISSIONING
  SmCommissioning_AddService(GATT_ALL_SERVICES);
#endif /* FEATURE_SECURE_COMMISSIONING */

  // Initialize provisioning profile attributes based on 15.4 application values
  RemoteDisplay_syncProvAttr();

  // Initialize remote display profile attributes based on 15.4 application values
  RemoteDisplay_syncRDAttr();

  // Register callback with Remote Display GATT profile
  RemoteDisplay_RegisterAppCBs(&RemoteDisplay_ProfileCBs);

  #ifdef DMM_COLLECTOR
  // Register callback with Network DEvice GATT profile
  NetworkDeviceProfile_RegisterAppCBs(&networkDeviceProfileCBs);
  #endif
  // Register callback with Provisioning GATT profile
  ProvisioningProfile_RegisterAppCBs(&Provisioning_ProfileCBs);
#ifdef FEATURE_SECURE_COMMISSIONING
  SmCommissioning_RegisterAppCBs(&SmCommissioning_ProfileCBs);
#endif /* FEATURE_SECURE_COMMISSIONING */

  // Start Bond Manager and register callback
  VOID GAPBondMgr_Register(&RemoteDisplay_BondMgrCBs);

  // Register with GAP for HCI/Host messages. This is needed to receive HCI
  // events. For more information, see the HCI section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  GAP_RegisterForMsgs(selfEntity);

  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs(selfEntity);

  // Set default values for Data Length Extension
  // Extended Data Length Feature is already enabled by default
  {
    // Set initial values to maximum, RX is set to max. by default(251 octets, 2120us)
    // Some brand smartphone is essentially needing 251/2120, so we set them here.
    #define APP_SUGGESTED_PDU_SIZE 251 //default is 27 octets(TX)
    #define APP_SUGGESTED_TX_TIME 2120 //default is 328us(TX)

    // This API is documented in hci.h
    // See the LE Data Length Extension section in the BLE5-Stack User's Guide for information on using this command:
    // http://software-dl.ti.com/lprf/ble5stack-latest/
    HCI_LE_WriteSuggestedDefaultDataLenCmd(APP_SUGGESTED_PDU_SIZE, APP_SUGGESTED_TX_TIME);
  }

  // Initialize GATT Client
  GATT_InitClient();

  // Initialize Connection List
  RemoteDisplay_clearConnListEntry(LL_CONNHANDLE_ALL);

  // Initialize GAP layer for Peripheral role and register to receive GAP events
  GAP_DeviceInit(GAP_PROFILE_PERIPHERAL, selfEntity, addrMode, &pRandomAddress);

  // Initialize array to store connection handle and RSSI values
  RemoteDisplay_initPHYRSSIArray();

#ifndef CUI_DISABLE
  CUI_clientParams_t clientParams;
  CUI_clientParamsInit(&clientParams);

  #ifdef DMM_SENSOR
  strncpy(clientParams.clientName, "15.4 Sensor Remote Display", MAX_CLIENT_NAME_LEN);
  #elif DMM_COLLECTOR
  strncpy(clientParams.clientName, "15.4 Collector Remote Display", MAX_CLIENT_NAME_LEN);
  #endif

  clientParams.maxStatusLines = 9;
#ifdef DISPLAY_PER_STATS
  clientParams.maxStatusLines++;
#endif
#ifdef DMM_OAD
  clientParams.maxStatusLines++;
#endif
#ifdef BLOCK_MODE_TEST
  clientParams.maxStatusLines++;
#endif

  /* Open UI for key and LED */
  remoteDisplayCuiHndl = CUI_clientOpen(&clientParams);
  CUI_registerMenu(remoteDisplayCuiHndl, &remoteDisplayMainMenu);

  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "RD STATUS", false, &rdStatusLineStatus1);
  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "BLE STATUS", false, &rdStatusLineStatus2);
  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "BLE CONN STATUS", false, &rdStatusLineConnection);
  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "BLE ADV STATE", false, &rdStatusLineAdvState);
  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "BLE ID", false, &rdStatusLineIda);
  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "BLE RPA", false, &rdStatusLineRpa);
  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "BLE PHY STATUS", false, &rdStatusLinePhyStatus);
  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "BLE RSSI", false, &rdStatusLineRssiStatus);
  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "CHAR STATS", false, &rdStatusLineCharStats);
#ifdef DISPLAY_PER_STATS
  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "BLE PER", false, &rdStatusLinePerStats);
#endif
#ifdef BLOCK_MODE_TEST
  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "BLOCK MODE TEST STATUS", false, &rdStatusLineBlockModeTestState);
#endif
#endif /* CUI_DISABLE */

#ifdef DMM_OAD
#ifndef CUI_DISABLE
  CUI_statusLineResourceRequest(remoteDisplayCuiHndl, "OAD Status", false, &rdStatusLineOad);
#endif /* CUI_DISABLE */
  // Open the OAD module and add the OAD service to the application
  if(OAD_SUCCESS != OAD_open(OAD_DEFAULT_INACTIVITY_TIME))
  {
      /*
       *  OAD cannot be opened, steps must be taken in the application to
       *  handle this gracefully, this can mean an error, assert,
       *  or print statement.
       */
  }
  else
  {
      // Register the OAD callback with the application
      OAD_register(&RemoteDisplay_oadCBs);
  }
#endif
#if defined(LPSTK) && defined(DMM_SENSOR)
    /*
     * NOTE!!
     * OAD_open() must be called before the LPSTK sensors are initialized because there
     * is no arbitration between the Flash and Accelerometer SPI writes.
     * This is handled by halting the LPSTK sensor task until this operation is complete
     */
    Ssf_PostAppSem();
#endif /* LPSTK */

  // register the app callbacks
  DMMPolicy_registerAppCbs(dmmPolicyAppCBs, DMMPolicy_StackRole_BlePeripheral);
}

/*********************************************************************
 * @fn      RemoteDisplay_taskFxn
 *
 * @brief   Application task entry point for the Remote Display.
 *
 * @param   a0, a1 - not used.
 */
static void RemoteDisplay_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  RemoteDisplay_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    // Waits for an event to be posted associated with the calling thread.
    // Note that an event associated with a thread is posted when a
    // message is queued to the message receive queue of the thread
    events = Event_pend(syncEvent, Event_Id_NONE, RD_ALL_EVENTS,
                        ICALL_TIMEOUT_FOREVER);

    if (events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      // Fetch any available messages that might have been sent from the stack
      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        uint8 safeToDealloc = TRUE;

        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

          // Check for non-BLE stack events
          if (pEvt->signature != 0xffff)
          {
            // Process inter-task message
            safeToDealloc = RemoteDisplay_processStackMsg((ICall_Hdr *)pMsg);
          }
        }

        if (pMsg && safeToDealloc)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
      if (events & RD_QUEUE_EVT)
      {
        while (!Queue_empty(appMsgQueueHandle))
        {
          rdEvt_t *pMsg = (rdEvt_t *)Util_dequeueMsg(appMsgQueueHandle);
          if (pMsg)
          {
            // Process message.
            RemoteDisplay_processAppMsg(pMsg);

            // Free the space from the message.
            ICall_free(pMsg);
          }
        }
      }
#ifdef DMM_OAD
      // OAD events
      if(events & OAD_QUEUE_EVT)
      {
#if defined(LPSTK) && defined(DMM_SENSOR)
          if (oad_in_progress == FALSE)
          {
              oad_in_progress = TRUE;

              /*
               * If this is the beginning of an OAD (first event received),
               * disable sensors because:
               *  - the accelerometer SPI and external flash SPI can not
               *    be used simultaneously
               *  - this will maximize over-the-air bandwidth for OAD
               *    as there will be no sensor profile communications
               */
              shutDownSensors(LPSTK_ACCELEROMETER);

              /*
               * Reset the base configuration of the SPI pins. The sensor
               * controller interface files will have modified the config
               * for accelerometer usage.
               */
              GPIO_resetConfig(CONFIG_GPIO_SPI_SS);
              GPIO_resetConfig(CONFIG_PIN_SPI_1_SCLK);
              GPIO_resetConfig(CONFIG_PIN_SPI_1_MOSI);
              GPIO_resetConfig(CONFIG_PIN_SPI_1_MISO);
          }
#endif /* LPSTK */
          // update DMM policy
          DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_OAD);

          // Process the OAD Message Queue
          uint8_t status = OAD_processQueue();

          imgProgressInfo_t dispInfo;
          OAD_getProgressInfo(&dispInfo);

#ifndef CUI_DISABLE
          uint32_t numHashes = 0xFFFFFFFF;

          if (dispInfo.totBlocks != 0xFFFFFFFF)
          {
              numHashes = (dispInfo.currBlock / (dispInfo.totBlocks / PROGRESS_WIDTH));
          }
#endif /* CUI_DISABLE */

          // If the OAD state machine encountered an error, print it
          // Return codes can be found in oad_constants.h
          if(status == OAD_IMG_ID_TIMEOUT)
          {
              // This may be an attack, terminate the link
              GAP_TerminateLinkReq(OAD_getactiveCxnHandle(),
                                   HCI_DISCONNECT_REMOTE_USER_TERM);
          }
          else if (status == OAD_DL_COMPLETE)
          {
#ifndef CUI_DISABLE
              CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineOad, CUI_COLOR_GREEN "Completed" CUI_COLOR_RESET);
#endif /* CUI_DISABLE */
          }
#ifndef CUI_DISABLE
          else if (status == OAD_SUCCESS)
          {
              /* If numHashes is not invalid */
              if (numHashes != 0xFFFFFFFF)
              {
                  CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineOad, "Transferring - [%s%.*s%.*s%s] (%d%%)",
                                       CUI_COLOR_GREEN,
                                       numHashes,                       HASHES_STR,
                                       (PROGRESS_WIDTH - numHashes),    SPACES_STR,
                                       CUI_COLOR_RESET,
                                       (uint32_t)((100 * dispInfo.currBlock) / dispInfo.totBlocks));
              }
          }
#endif /* CUI_DISABLE */
          else
          {
#ifndef CUI_DISABLE
              CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineOad, CUI_COLOR_RED "Stopped" CUI_COLOR_RESET);
#endif /* CUI_DISABLE */
              /* update DMM policy */
              DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_CONNECTED);
          }
      }

      if(events & OAD_DL_COMPLETE_EVT)
      {
          // Register for L2CAP Flow Control Events
          L2CAP_RegisterFlowCtrlTask(selfEntity);

#if defined(LPSTK) && defined(DMM_SENSOR)
          oad_in_progress = FALSE;
#endif /* LPSTK */
      }
#endif /* DMM_OAD */
    }
  }
}

/*********************************************************************
 * @fn      RemoteDisplay_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t RemoteDisplay_processStackMsg(ICall_Hdr *pMsg)
{
  // Always dealloc pMsg unless set otherwise
  uint8_t safeToDealloc = TRUE;

  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:
      RemoteDisplay_processGapMessage((gapEventHdr_t*) pMsg);
      break;

    case GATT_MSG_EVENT:
      // Process GATT message
      safeToDealloc = RemoteDisplay_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

#ifdef DMM_OAD
    case L2CAP_SIGNAL_EVENT:
    {
       // Process L2CAP free buffer notification
        RemoteDisplay_processL2CAPMsg((l2capSignalEvent_t *)pMsg);
       break;
    }
#endif

    case HCI_GAP_EVENT_EVENT:
    {
      // Process HCI message
      switch(pMsg->status)
      {
        case HCI_COMMAND_COMPLETE_EVENT_CODE:
        // Process HCI Command Complete Events here
        {
          RemoteDisplay_processCmdCompleteEvt((hciEvt_CmdComplete_t *) pMsg);
          break;
        }

        case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
          AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
          break;
#ifdef DISPLAY_PER_STATS
        case HCI_VE_EVENT_CODE:
          RemoteDisplay_processVSCmdCompleteEvt((hciEvt_VSCmdComplete_t *) pMsg);
          break;
#endif /* DISPLAY_PER_STATS */

        // HCI Commands Events
        case HCI_COMMAND_STATUS_EVENT_CODE:
        {
            hciEvt_CommandStatus_t *pMyMsg = (hciEvt_CommandStatus_t *)pMsg;
                switch ( pMyMsg->cmdOpcode )
                {
                  case HCI_LE_SET_PHY:
                  {
                    if (pMyMsg->cmdStatus == HCI_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE)
                    {
#ifndef CUI_DISABLE
                      CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLinePhyStatus, "PHY Change failure, peer does not support this");
#endif /* CUI_DISABLE */
                    }
                    else
                    {
#ifndef CUI_DISABLE
                      CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLinePhyStatus, "PHY Update Status Event: 0x%x", pMyMsg->cmdStatus);
#endif /* CUI_DISABLE */
                    }

                    RemoteDisplay_updatePHYStat(HCI_LE_SET_PHY, (uint8_t *)pMsg);
                    break;
                  }

                  default:
                    break;
                }
          break;
        }

        case HCI_LE_EVENT_CODE:
        {
          hciEvt_BLEPhyUpdateComplete_t *pPUC =
            (hciEvt_BLEPhyUpdateComplete_t*) pMsg;

          // A Phy Update Has Completed or Failed
          if (pPUC->BLEEventCode == HCI_BLE_PHY_UPDATE_COMPLETE_EVENT)
          {
            if (pPUC->status != SUCCESS)
            {
#ifndef CUI_DISABLE
              CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLinePhyStatus, "PHY Change failure");
#endif /* CUI_DISABLE */
            }
            else
            {
              // Only symmetrical PHY is supported.
              // rxPhy should be equal to txPhy.
#ifndef CUI_DISABLE
              CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLinePhyStatus, "PHY Updated to %s",
                                         (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_1M) ? "1M" :
                                         (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_2M) ? "2M" :
                                         (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_CODED) ? "CODED" : "Unexpected PHY Value");
#endif /* CUI_DISABLE */
            }

            RemoteDisplay_updatePHYStat(HCI_BLE_PHY_UPDATE_COMPLETE_EVENT, (uint8_t *)pMsg);
          }
          break;
        }

        default:
          break;
      }

      break;
    }

    default:
      // do nothing
      break;
  }

  return (safeToDealloc);
}

/*********************************************************************
 * @fn      RemoteDisplay_processGATTMsg
 *
 * @brief   Process GATT messages and events.
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t RemoteDisplay_processGATTMsg(gattMsgEvent_t *pMsg)
{
  if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
  {
    // ATT request-response or indication-confirmation flow control is
    // violated. All subsequent ATT requests or indications will be dropped.
    // The app is informed in case it wants to drop the connection.

    // Display the opcode of the message that caused the violation.
#ifndef CUI_DISABLE
    CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineStatus1, "FC Violated: %d", pMsg->msg.flowCtrlEvt.opcode);
#endif /* CUI_DISABLE */
  }
  else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
  {
#ifdef DMM_OAD
    OAD_setBlockSize(pMsg->msg.mtuEvt.MTU);
#endif
    // MTU size updated
#ifndef CUI_DISABLE
    CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineStatus1, "MTU Size: %d", pMsg->msg.mtuEvt.MTU);
#endif /* CUI_DISABLE */
  }

  // Free message payload. Needed only for ATT Protocol messages
  GATT_bm_free(&pMsg->msg, pMsg->method);

  // It's safe to free the incoming message
  return (TRUE);
}

/*********************************************************************
 * @fn      RemoteDisplay_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  None.
 */
static void RemoteDisplay_processAppMsg(rdEvt_t *pMsg)
{
  bool dealloc = TRUE;

  switch (pMsg->event)
  {
    case RD_CHAR_CHANGE_EVT:
      RemoteDisplay_processRDCharValueChangeEvt(*(uint8_t*)(pMsg->pData));
      break;
#ifdef DMM_COLLECTOR
    case NETWORK_DEVICE_CHAR_CHANGE_EVT:
      RemoteDisplay_processNetworkDeviceCharValueChangeEvt(*(uint8_t*)(pMsg->pData));
      break;
#endif
    case PROV_CHAR_CHANGE_EVT:
      RemoteDisplay_processProvCharValueChangeEvt(*(uint8_t*)(pMsg->pData));
      break;

#ifdef FEATURE_SECURE_COMMISSIONING
    case SM_CHAR_CHANGE_EVT:
      RemoteDisplay_processSmCharValueChangeEvt(*(uint8_t*)(pMsg->pData));
      break;
#endif /* FEATURE_SECURE_COMMISSIONING */

    case RD_ADV_EVT:
      RemoteDisplay_processAdvEvent((rdGapAdvEventData_t*)(pMsg->pData));
      break;

    case RD_PAIR_STATE_EVT:
      RemoteDisplay_processPairState((rdPairStateData_t*)(pMsg->pData));
      break;

    case RD_PASSCODE_EVT:
      RemoteDisplay_processPasscode((rdPasscodeData_t*)(pMsg->pData));
      break;

    case RD_READ_RPA_EVT:
      RemoteDisplay_updateRPA();
      break;

#ifdef DISPLAY_PER_STATS
    case RD_PER_READ_EVT:
      RemoteDisplay_processUpdatePerEvt();
      break;
#endif /* DISPLAY_PER_STATS */

    case RD_SEND_PARAM_UPDATE_EVT:
    {
      // Extract connection handle from data
      uint16_t connHandle = *(uint16_t *)(((rdClockEventData_t *)pMsg->pData)->data);

      RemoteDisplay_processParamUpdate(connHandle);

      // This data is not dynamically allocated
      dealloc = FALSE;

      /* If we are sending a param update request then the service discovery
       * should have ended. Changed state to connected */
      DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_CONNECTED);

      break;
    }

    // Provisioning state update event
    case RD_UPDATE_PROV_STATE_EVT:
    {
#ifdef DMM_SENSOR
        Jdllc_states_t state = *((Jdllc_states_t*)(pMsg->pData));

        if((state == Jdllc_states_joined) || (state == Jdllc_states_rejoined))
#elif DMM_COLLECTOR
        Cllc_states_t state = *((Cllc_states_t*)(pMsg->pData));

        if((state == Cllc_states_started) || (state == Cllc_states_joiningAllowed))
#endif
        {
            if(dmmPolicyBlePaused)
            {
                //unpause BLE
                dmmPolicyBlePaused = false;
            }
        }

        // Re-sync 15.4 attributes with provisioning profile(15.4->BLE)
        RemoteDisplay_syncProvAttr();
        break;
    }

    // Remote display data update event
    case RD_UPDATE_RD_DATA_EVT:
    {
        // Re-sync 15.4 attributes with remote display profile(15.4->BLE)
        RemoteDisplay_syncRDAttr();
        break;
    }
#ifdef DMM_COLLECTOR
    case RD_UPDATE_DEVICE_UPDATE_EVT:
    {
        // Re-sync 15.4 device info with remote display profile(15.4->BLE)
        uint16_t deviceUpdateAddr = *((uint16_t*)(pMsg->pData));
        RemoteDisplay_syncNetworkDevices(deviceUpdateAddr);
        break;
    }
#endif

    case RD_POLICY_PAUSE_EVT:
    {
        uint8_t numActive = linkDB_NumActive();
        uint16_t pause =  *((uint16_t*)(pMsg->pData));

        if(pause == DMMPOLICY_PAUSED)
        {
            if(numActive < MAX_NUM_BLE_CONNS)
            {
                // Stop advertising
                GapAdv_disable(advHandleLegacy);
            }

            //Stop terminate from starting advertisements
            dmmPolicyBlePaused = true;

            //terminate all connections
            GAP_TerminateLinkReq(LINKDB_CONNHANDLE_ALL, HCI_DISCONNECT_REMOTE_USER_TERM);
        }
        else
        {
            /* We should not have an active connection when resuming from a pause, but just
             * in case check max connections before starting advertising
             */
            if(numActive < MAX_NUM_BLE_CONNS)
            {
                GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
            }

            dmmPolicyBlePaused = false;
        }

        break;
    }

#ifndef CUI_DISABLE
    case RD_UI_INPUT_EVT:
    {
      CUI_processMenuUpdate();
      break;
    }
#endif /* CUI_DISABLE */

    case RD_CONN_EVT:
      RemoteDisplay_processConnEvt((Gap_ConnEventRpt_t *)(pMsg->pData));
      break;

#ifdef FEATURE_SECURE_COMMISSIONING
    case SM_SENSOR_STATE_CHANGE_EVT:
    {
        uint8_t new_state =  *((uint8_t*)(pMsg->pData));
        SmCommissioning_SetParameter(SMCOMMISSIONING_PROFILE_STATECHAR_ID,
                                     SMCOMMISSIONING_PROFILE_STATECHAR_LEN,
                                     &new_state);
        break;
    }
#endif /* FEATURE_SECURE_COMMISSIONING */

    default:
      // Do nothing.
      break;
  }

  // Free message data if it exists and we are to dealloc
  if ((dealloc == TRUE) && (pMsg->pData != NULL))
  {
    ICall_free(pMsg->pData);
  }
}

/*********************************************************************
 * @fn      RemoteDisplay_processGapMessage
 *
 * @brief   Process an incoming GAP event.
 *
 * @param   pMsg - message to process
 */
static void RemoteDisplay_processGapMessage(gapEventHdr_t *pMsg)
{
  switch(pMsg->opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
    {
      bStatus_t status = FAILURE;

      gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

      if(pPkt->hdr.status == SUCCESS)
      {
        // Store the system ID
        uint8_t systemId[DEVINFO_SYSTEM_ID_LEN];

        // use 6 bytes of device address for 8 bytes of system ID value
        systemId[0] = pPkt->devAddr[0];
        systemId[1] = pPkt->devAddr[1];
        systemId[2] = pPkt->devAddr[2];

        // set middle bytes to zero
        systemId[4] = 0x00;
        systemId[3] = 0x00;

        // shift three bytes up
        systemId[7] = pPkt->devAddr[5];
        systemId[6] = pPkt->devAddr[4];
        systemId[5] = pPkt->devAddr[3];

        // Set Device Info Service Parameter
        DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);

#ifndef CUI_DISABLE
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineIda, "Initialized");
#endif /* CUI_DISABLE */

        // Setup and start Advertising
        // For more information, see the GAP section in the User's Guide:
        // http://software-dl.ti.com/lprf/ble5stack-latest/

        // Temporary memory for advertising parameters for set #1. These will be copied
        // by the GapAdv module

        // Create Advertisement set #1 and assign handle
        status = GapAdv_create(&RemoteDisplay_advCallback, &advParams1,
                               &advHandleLegacy);
        REMOTEDISPLAY_ASSERT(status == SUCCESS);

        // Load advertising data for set #1 that is statically allocated by the app
        status = GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_ADV,
                                     sizeof(advData1), advData1);
        REMOTEDISPLAY_ASSERT(status == SUCCESS);

        // Load scan response data for set #1 that is statically allocated by the app
        status = GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_SCAN_RSP,
                                     sizeof(scanResData1), scanResData1);
        REMOTEDISPLAY_ASSERT(status == SUCCESS);

        // Set event mask for set #1
        status = GapAdv_setEventMask(advHandleLegacy,
                                     GAP_ADV_EVT_MASK_START_AFTER_ENABLE |
                                     GAP_ADV_EVT_MASK_END_AFTER_DISABLE |
                                     GAP_ADV_EVT_MASK_SET_TERMINATED);

        // Enable legacy advertising for set #1
        status = GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
        REMOTEDISPLAY_ASSERT(status == SUCCESS);

        // Display device address
#ifndef CUI_DISABLE
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineIda, "%s Addr: %s", (addrMode <= ADDRMODE_RANDOM) ? "Dev" : "ID", Util_convertBdAddr2Str(pPkt->devAddr));
#endif /* CUI_DISABLE */

        if (addrMode > ADDRMODE_RANDOM)
        {
          RemoteDisplay_updateRPA();

          // Create one-shot clock for RPA check event.
          Util_constructClock(&clkRpaRead, RemoteDisplay_clockHandler,
                              RD_READ_RPA_EVT_PERIOD, 0, true,
                              (UArg) &argRpaRead);
        }
      }
      break;
    }

    case GAP_LINK_ESTABLISHED_EVENT:
    {
      gapEstLinkReqEvent_t *pPkt = (gapEstLinkReqEvent_t *)pMsg;

      // Display the amount of current connections
      uint8_t numActive = linkDB_NumActive();
#ifndef CUI_DISABLE
      CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineStatus2, "Num Conns: %d", (uint16_t)numActive);
#endif /* CUI_DISABLE */

      if (pPkt->hdr.status == SUCCESS)
      {
        // Add connection to list and start RSSI
        RemoteDisplay_addConn(pPkt->connectionHandle);

        // Display the address of this connection
#ifndef CUI_DISABLE
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineStatus1, "%s Addr: %s", (addrMode <= ADDRMODE_RANDOM) ? "Dev" : "ID", Util_convertBdAddr2Str(pPkt->devAddr));
#endif /* CUI_DISABLE */

#ifdef DISPLAY_PER_STATS
        // Clear PER stats
        HCI_EXT_PacketErrorRateCmd(0, HCI_EXT_PER_RESET);
        // Start PER Clock.
        Util_startClock(&clkReadPer);
#endif /* DISPLAY_PER_STATS */
      }

      DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_HIGH_BANDWIDTH);

      if (numActive < MAX_NUM_BLE_CONNS)
      {
        // Start advertising since there is room for more connections
        GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
      }
      else
      {
        // Stop advertising since there is no room for more connections
        GapAdv_disable(advHandleLegacy);
      }

      break;
    }

    case GAP_LINK_TERMINATED_EVENT:
    {
      gapTerminateLinkEvent_t *pPkt = (gapTerminateLinkEvent_t *)pMsg;

#ifdef DMM_OAD
      // Cancel the OAD if one is going on
      // A disconnect forces the peer to re-identify
      OAD_cancel();
#endif

      // Display the amount of current connections
      uint8_t numActive = linkDB_NumActive();
#ifndef CUI_DISABLE
      CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineStatus1, "Device Disconnected!");
      CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineStatus2, "Num Conns: %d", (uint16_t)numActive);
#endif /* CUI_DISABLE */

      // Remove the connection from the list and disable RSSI if needed
      RemoteDisplay_removeConn(pPkt->connectionHandle);

      // If no active connections
      if (numActive == 0)
      {
#ifdef DISPLAY_PER_STATS
        // Clear PER stats
        HCI_EXT_PacketErrorRateCmd(0, HCI_EXT_PER_RESET);
        // Stop PER clock
        Util_stopClock(&clkReadPer);
#endif /* DISPLAY_PER_STATS */
      }

      if( (!dmmPolicyBlePaused) && (numActive < MAX_NUM_BLE_CONNS))
      {
        // Start advertising since there is room for more connections and we are not paused
        GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
      }

      break;
    }

    case GAP_UPDATE_LINK_PARAM_REQ_EVENT:
    {
      gapUpdateLinkParamReqReply_t rsp;

      /*During param update, keep BLE in high bandwidth state */
      DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_HIGH_BANDWIDTH);

      gapUpdateLinkParamReqEvent_t *pReq = (gapUpdateLinkParamReqEvent_t *)pMsg;

      rsp.connectionHandle = pReq->req.connectionHandle;
      rsp.signalIdentifier = pReq->req.signalIdentifier;

      // Only accept connection intervals with slave latency of 0
      // This is just an example of how the application can send a response
      if(pReq->req.connLatency == 0)
      {
        rsp.intervalMin = pReq->req.intervalMin;
        rsp.intervalMax = pReq->req.intervalMax;
        rsp.connLatency = pReq->req.connLatency;
        rsp.connTimeout = pReq->req.connTimeout;
        rsp.accepted = TRUE;
      }
      else
      {
        rsp.accepted = FALSE;
      }

      // Send Reply
      VOID GAP_UpdateLinkParamReqReply(&rsp);

      break;
    }

    case GAP_LINK_PARAM_UPDATE_EVENT:
    {
      gapLinkUpdateEvent_t *pPkt = (gapLinkUpdateEvent_t *)pMsg;

      /*Param has been updated, so go back to connected state */
      DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_CONNECTED);

      // Get the address from the connection handle
      linkDBInfo_t linkInfo;
      linkDB_GetInfo(pPkt->connectionHandle, &linkInfo);

#ifndef CUI_DISABLE
      if(pPkt->status == SUCCESS)
      {
        // Display the address of the connection update
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineStatus2, "Link Param Updated: %s", Util_convertBdAddr2Str(linkInfo.addr));
      }
      else
      {
        // Display the address of the connection update failure
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineStatus2, "Link Param Update Failed 0x%x %s", pPkt->opcode, Util_convertBdAddr2Str(linkInfo.addr));
      }
#endif /* CUI_DISABLE */

      // Check if there are any queued parameter updates
      rdConnHandleEntry_t *connHandleEntry = (rdConnHandleEntry_t *)List_get(&paramUpdateList);
      if (connHandleEntry != NULL)
      {
          // Attempt to send queued update now
          RemoteDisplay_processParamUpdate(connHandleEntry->connHandle);

          // Free list element
          ICall_free(connHandleEntry);
      }

      break;
    }

    default:
      break;
  }
}

/*********************************************************************
 *  @fn      RemoteDisplay_bleFastStateUpdateCb
 *
 * @brief   Callback from BLE link layer to indicate a state change
 */
void RemoteDisplay_bleFastStateUpdateCb(uint32_t StackRole, uint32_t stackState)
{
  if(StackRole == DMMPolicy_StackRole_BlePeripheral)
  {
    static uint32_t prevStackState = 0;

    if( !(prevStackState & LL_TASK_ID_SLAVE) && (stackState & LL_TASK_ID_SLAVE))
    {
        /* update DMM policy */
        DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_CONNECTING);
    }

    prevStackState = stackState;
  }
}
/*********************************************************************
 * @fn      RemoteDisplay_blePausePolicyCb
 *
 * @brief   DMM Policy callback to pause BLE Stack
 */
static void RemoteDisplay_blePausePolicyCb(uint16_t pause)
{
    uint16_t *pValue = ICall_malloc(sizeof(uint16_t));

    if (pValue)
    {
      *pValue = pause;

      if (RemoteDisplay_enqueueMsg(RD_POLICY_PAUSE_EVT, pValue) != SUCCESS)
      {
        ICall_free(pValue);
      }
    }
}

/*********************************************************************
 * @fn      RemoteDisplay_charValueChangeCB
 *
 * @brief   Callback from Remote Display Profile indicating a characteristic
 *          value change.
 *
 * @param   paramId - parameter Id of the value that was changed.
 *
 * @return  None.
 */
static void RemoteDisplay_charValueChangeCB(uint8_t paramId)
{
  uint8_t *pValue = ICall_malloc(sizeof(uint8_t));

  if (pValue)
  {
    *pValue = paramId;

    if (RemoteDisplay_enqueueMsg(RD_CHAR_CHANGE_EVT, pValue) != SUCCESS)
    {
      ICall_free(pValue);
    }
  }
}

#ifdef FEATURE_SECURE_COMMISSIONING
/*********************************************************************
 * @fn      SmCommissioning_charValueChangeCB
 *
 * @brief   Callback from SM Commissioning Profile indicating a characteristic
 *          value change.
 *
 * @param   paramId - parameter Id of the value that was changed.
 *
 * @return  None.
 */
static void SmCommissioning_charValueChangeCB(uint8_t paramId)
{
  uint8_t *pValue = ICall_malloc(sizeof(uint8_t));

  if (pValue)
  {
    *pValue = paramId;

    if (RemoteDisplay_enqueueMsg(SM_CHAR_CHANGE_EVT, pValue) != SUCCESS)
    {
      ICall_free(pValue);
    }
  }
}
#endif /* FEATURE_SECURE_COMMISSIONING */

#ifdef DMM_COLLECTOR
/*********************************************************************
 * @fn      NetworkDevice_charValueChangeCB
 *
 * @brief   Callback from Network Device Profile indicating a characteristic
 *          value change.
 *
 * @param   paramId - parameter Id of the value that was changed.
 *
 * @return  None.
 */
static void NetworkDevice_charValueChangeCB(uint8_t paramId)
{
  uint16_t *pValue = ICall_malloc(sizeof(uint16_t));

  if (pValue)
  {
    *pValue = paramId;

    if (RemoteDisplay_enqueueMsg(NETWORK_DEVICE_CHAR_CHANGE_EVT, pValue) != SUCCESS)
    {
      ICall_free(pValue);
    }
  }
}
#endif

/*********************************************************************
 * @fn      Provisioning_charValueChangeCB
 *
 * @brief   Callback from provisioning Profile indicating a characteristic
 *          value change.
 *
 * @param   paramId - parameter Id of the value that was changed.
 *
 * @return  None.
 */
static void Provisioning_charValueChangeCB(uint8_t paramId)
{
  uint8_t *pValue = ICall_malloc(sizeof(uint8_t));

  if (pValue)
  {
    *pValue = paramId;

    if (RemoteDisplay_enqueueMsg(PROV_CHAR_CHANGE_EVT, pValue) != SUCCESS)
    {
      ICall_free(pValue);
    }
  }
}

/*********************************************************************
 * @fn      RemoteDisplay_processRDCharValueChangeEvt
 *
 * @brief   Process a pending Remote Display Profile characteristic value change
 *          event.
 *
 * @param   paramID - parameter ID of the value that was changed.
 */
static void RemoteDisplay_processRDCharValueChangeEvt(uint8_t paramId)
{
    // Remote display profile new parameters
    uint8_t newReportInterval[RDPROFILE_SENSOR_REPORT_INTERVAL_LEN];
    uint8_t newCollLedVal;
#ifdef DMM_COLLECTOR
    uint8_t newSensorAddr[RDPROFILE_SENSOR_ADDR_LEN];
    uint8_t disassociateSensorAddr[RDPROFILE_SENSOR_ADDR_LEN];
#endif

 switch(paramId)
  {
     // Remote display profile characteristics
     case RDPROFILE_SENSOR_REPORT_INTERVAL_CHAR:
       // Obtain current value of parameter from profile
       RemoteDisplay_GetParameter(RDPROFILE_SENSOR_REPORT_INTERVAL_CHAR, &newReportInterval);

#ifndef CUI_DISABLE
#ifdef DMM_SENSOR
       CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineCharStats, "TI 15.4: Sensor Reporting Interval: 0x%02x%02x%02x%02x", newReportInterval[0], newReportInterval[1], newReportInterval[2], newReportInterval[3]);
#elif DMM_COLLECTOR
       CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineCharStats, "TI 15.4: Collector Reporting Interval: 0x%02x%02x%02x%02x", newReportInterval[0], newReportInterval[1], newReportInterval[2], newReportInterval[3]);
#endif
#endif /* CUI_DISABLE */

       // Call stack specific callback function to update parameter
       if(remoteDisplayCbs.setRDAttrCb != NULL)
       {
         remoteDisplayCbs.setRDAttrCb(RemoteDisplayAttr_ReportInterval, newReportInterval, RDPROFILE_SENSOR_REPORT_INTERVAL_LEN);
       }
       break;
     case RDPROFILE_COLL_LED_CHAR:
       // Obtain current value of parameter from profile
       RemoteDisplay_GetParameter(RDPROFILE_COLL_LED_CHAR, &newCollLedVal);
#ifdef DMM_SENSOR
#ifndef CUI_DISABLE
       CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineCharStats, "TI 15.4: Sensor LED toggled: %02x", newCollLedVal);
#endif /* CUI_DISABLE */
#elif DMM_COLLECTOR
       RemoteDisplay_GetParameter(RDPROFILE_SENSOR_ADDR_CHAR, &newSensorAddr);
#ifndef CUI_DISABLE
       CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineCharStats,
                            "TI 15.4: Sensor 0x%02x%02x LED toggled",
                            newSensorAddr[0], newSensorAddr[1]);
#endif /* CUI_DISABLE */
#endif

       // Call stack specific callback function to update parameter
       if(remoteDisplayCbs.setRDAttrCb != NULL)
       {
         remoteDisplayCbs.setRDAttrCb(RemoteDisplayAttr_CollLed, &newCollLedVal, RDPROFILE_COLL_LED_LEN);
       }
       break;

#ifdef DMM_COLLECTOR
     case RDPROFILE_SENSOR_ADDR_CHAR:
        // Obtain current value of parameter from profile
        RemoteDisplay_GetParameter(RDPROFILE_SENSOR_ADDR_CHAR, &newSensorAddr);
#ifndef CUI_DISABLE
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineCharStats,
                             "TI 15.4: Sensor Addr: 0x%02x%02x",
                             newSensorAddr[0], newSensorAddr[1]);
#endif /* CUI_DISABLE */
        // Call stack specific callback function to update parameter
        if (remoteDisplayCbs.setRDAttrCb != NULL)
        {
            remoteDisplayCbs.setRDAttrCb(RemoteDisplayAttr_SensorAddr,
                                         newSensorAddr,
                                         RDPROFILE_SENSOR_ADDR_CHAR);
        }
        break;

    case RDPROFILE_SENSOR_DISASSOCIATE_CHAR:
        // Obtain current value of parameter from profile
        RemoteDisplay_GetParameter(RDPROFILE_SENSOR_DISASSOCIATE_CHAR, &disassociateSensorAddr);
#ifndef CUI_DISABLE
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineCharStats,
                             "TI 15.4: Disassociating Sensor Addr: 0x%02x%02x",
                             disassociateSensorAddr[0], disassociateSensorAddr[1]);
#endif /* CUI_DISABLE */
        // Call stack specific callback function to update parameter
        if (remoteDisplayCbs.setRDAttrCb != NULL)
        {
            remoteDisplayCbs.setRDAttrCb(RemoteDisplayAttr_SensorDisassociate,
                                         disassociateSensorAddr,
                                         RDPROFILE_SENSOR_DISASSOCIATE_CHAR);
        }
        break;
#endif

    default:
      // should not reach here!
      break;
  }
}

#ifdef DMM_COLLECTOR
/*********************************************************************
 * @fn      RemoteDisplay_processNetworkDeviceCharValueChangeEvt
 *
 * @brief   Process a pending Remote Display Profile characteristic value change
 *          event.
 *
 * @param   paramID - parameter ID of the value that was changed.
 */
static void RemoteDisplay_processNetworkDeviceCharValueChangeEvt(uint8_t paramId)
{
  union RemoteDisplay_DeviceInfo_t devInfo;

  switch(paramId)
  {
     // Remote display profile characteristics
     case NETDEVPROFILE_DEVICE_CHAR:
       // Obtain current value of parameter from profile
       NetworkDeviceProfile_GetParameter(NETDEVPROFILE_DEVICE_CHAR, &devInfo);
       // update RD with 154 device info
       RemoteDisplay_deviceUpdate(((0x00FF & devInfo.ti15_4StackDeviceInfo.devAddr) << 8) + ((0xFF00 & devInfo.ti15_4StackDeviceInfo.devAddr) >> 8));

       break;

    default:
      // should not reach here!
      break;
  }
}
#endif

/*********************************************************************
 * @fn      RemoteDisplay_processProvCharValueChangeEvt
 *
 * @brief   Process a pending Remote Display Profile characteristic value change
 *          event.
 *
 * @param   paramID - parameter ID of the value that was changed.
 */
static void RemoteDisplay_processProvCharValueChangeEvt(uint8_t paramId)
{
    // Provisioning profile new parameters
    uint8_t newProvSensor;
    uint8_t newPanId[PROVPROFILE_NTWK_PAN_ID_CHAR_LEN];
    uint8_t newSensorChannel[PROVPROFILE_SENSOR_CHANNEL_CHAR_LEN];
    uint8_t newNtwkKey[PROVPROFILE_NTWK_KEY_CHAR_LEN];

 switch(paramId)
  {
     // Provisioning profile characteristics
     case PROVPROFILE_NTWK_PAN_ID_CHAR:
       // Obtain current value of parameter from profile
       ProvisioningProfile_GetParameter(PROVPROFILE_NTWK_PAN_ID_CHAR, newPanId);

       // Call stack specific callback function to update parameter
       if(clientProvCbs.setProvisioningAttrCb != NULL)
       {
         clientProvCbs.setProvisioningAttrCb(ProvisionAttr_PanId, newPanId, PROVISIONING_NTWK_ID_LEN);
       }
       break;

     case PROVPROFILE_SENSOR_CHANNEL_CHAR:
       // Obtain current value of parameter from profile
       ProvisioningProfile_GetParameter(PROVPROFILE_SENSOR_CHANNEL_CHAR, newSensorChannel);

       // Call stack specific callback function to update parameter
       if(clientProvCbs.setProvisioningAttrCb != NULL)
       {
         clientProvCbs.setProvisioningAttrCb(ProvisionAttr_SensorChannelMask, newSensorChannel, PROVISIONING_NTWK_CHNL_LEN);
       }

       break;

     case PROVPROFILE_NTWK_KEY_CHAR:
       // Obtain current value of parameter from profile
       ProvisioningProfile_GetParameter(PROVPROFILE_NTWK_KEY_CHAR, newNtwkKey);

       // Call stack specific callback function to update parameter
       if(clientProvCbs.setProvisioningAttrCb != NULL)
       {
         clientProvCbs.setProvisioningAttrCb(ProvisionAttr_NtwkKey, newNtwkKey, PROVISIONING_NTWK_KEY_LEN);
       }
       break;

     case PROVPROFILE_PROV_SENSOR_CHAR:
       // Obtain current value of parameter from profile
       ProvisioningProfile_GetParameter(PROVPROFILE_PROV_SENSOR_CHAR, &newProvSensor);

       // Call stack specific callback function to update parameter
       if((clientProvCbs.provisionConnectCb != NULL) && (newProvSensor == PROVPROFILE_PAN_CONNECT))
       {
         clientProvCbs.provisionConnectCb();
       }
#ifdef DMM_COLLECTOR
       else if((clientProvCbs.provisionNtwkOpenCloseCb != NULL) && (newProvSensor == PROVPROFILE_PAN_NETWORKOPEN))
       {
         clientProvCbs.provisionNtwkOpenCloseCb(true);
       }
       else if((clientProvCbs.provisionNtwkOpenCloseCb != NULL) && (newProvSensor == PROVPROFILE_PAN_NETWORKCLOSE))
       {
         clientProvCbs.provisionNtwkOpenCloseCb(false);
       }
#endif
#ifdef DMM_SENSOR
       else if((clientProvCbs.provisionDisconnectCb != NULL) && (newProvSensor == PROVPROFILE_PAN_DISCONNECT))
       {
         clientProvCbs.provisionDisconnectCb();
       }
#endif
       break;

    default:
      // should not reach here!
      break;
  }
}

#ifdef FEATURE_SECURE_COMMISSIONING
/*********************************************************************
 * @fn      RemoteDisplay_processSmCharValueChangeEvt
 *
 * @brief   Process a pending Remote Display SM Profile characteristic value change
 *          event.
 *
 * @param   paramID - parameter ID of the value that was changed.
 */
static void RemoteDisplay_processSmCharValueChangeEvt(uint8_t paramId)
{
#ifndef FEATURE_FULL_FUNCTION_DEVICE
  static SMMsgs_authMethod_t newSmAuthMethod;
#endif
  static uint32_t newPasskey;

  switch(paramId)
  {
#ifndef FEATURE_FULL_FUNCTION_DEVICE
    // SM Set Auth Mode Char Change
    case SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_ID:
      // Obtain current value of parameter from profile
      SmCommissioning_GetParameter(SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_ID, &newSmAuthMethod);
      Sensor_setSmAuthMethod(newSmAuthMethod);
      break;
#endif

    // SM Set Auth Mode Char Change
    case SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_ID:
      // Obtain current value of parameter from profile
      SmCommissioning_GetParameter(SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_ID, &newPasskey);
      RemoteDisplay_updateSmState(SMCOMMISSIONSTATE_COMMISSIONING);
      SM_setPasskey(newPasskey);
      break;

    default:
      // should not reach here!
      break;
  }
}
#endif /* FEATURE_SECURE_COMMISSIONING */

/*********************************************************************
 * @fn      RemoteDisplay_updateRPA
 *
 * @brief   Read the current RPA from the stack and update display
 *          if the RPA has changed.
 *
 * @param   None.
 *
 * @return  None.
 */
static void RemoteDisplay_updateRPA(void)
{
  uint8_t* pRpaNew;

  // Read the current RPA.
  pRpaNew = GAP_GetDevAddress(FALSE);

  if (memcmp(pRpaNew, rpa, B_ADDR_LEN))
  {
    // If the RPA has changed, update the display
#ifndef CUI_DISABLE
    CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineRpa, "RP Addr: %s", Util_convertBdAddr2Str(pRpaNew));
#endif /* CUI_DISABLE */
    memcpy(rpa, pRpaNew, B_ADDR_LEN);
  }
}

/*********************************************************************
 * @fn      RemoteDisplay_clockHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void RemoteDisplay_clockHandler(UArg arg)
{
  rdClockEventData_t *pData = (rdClockEventData_t *)arg;

  if (pData->event == RD_READ_RPA_EVT)
  {
    // Start the next period
    Util_startClock(&clkRpaRead);

    // Post event to read the current RPA
    RemoteDisplay_enqueueMsg(RD_READ_RPA_EVT, NULL);
  }
  else if (pData->event == RD_SEND_PARAM_UPDATE_EVT)
  {
    // Send message to app
    if (RemoteDisplay_enqueueMsg(RD_SEND_PARAM_UPDATE_EVT, pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
#ifdef DISPLAY_PER_STATS
  else if (pData->event == RD_PER_READ_EVT)
  {
    // Satrt the next PER Read Period
    Util_startClock(&clkReadPer);
    // Post event to wake up the application
    RemoteDisplay_enqueueMsg(RD_PER_READ_EVT, NULL);
  }
#endif /* DISPLAY_PER_STATS */

#if defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE)
  else if (pData->event == RD_BLOCK_MODE_EVT)
  {
    if (DMMPolicy_getBlockModeStatus(DMMPolicy_StackRole_BlePeripheral))
    {
      // update the DMM Block Mode status
      DMMPolicy_setBlockModeOff(DMMPolicy_StackRole_BlePeripheral);
      Util_rescheduleClock(&clkBlockModeTest, RD_BLOCK_MODE_OFF_PERIOD);
    }
    else
    {
      DMMPolicy_setBlockModeOn(DMMPolicy_StackRole_BlePeripheral);
      Util_rescheduleClock(&clkBlockModeTest, RD_BLOCK_MODE_ON_PERIOD);
    }
  }
#endif /* defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE) */

}

#ifndef CUI_DISABLE
/*********************************************************************
 * @fn      RemoteDisplay_cyclePhy
 *
 * @brief   Set the BLE5 Phy mode.
 *
 * @param   menuEntryIndex - index of CUI menu option
 */
static void RemoteDisplay_cyclePhy(int32_t menuEntryIndex)
{
  if (menuEntryIndex >= 0 && menuEntryIndex <= (rdSetPhySubMenu.numItems - 2))
  {
    RemoteDisplay_doSetConnPhy(menuEntryIndex);
  }
}
#endif /* CUI_DISABLE */

/*********************************************************************
 * @fn      RemoteDisplay_advCallback
 *
 * @brief   GapAdv module callback
 *
 * @param   pMsg - message to process
 */
static void RemoteDisplay_advCallback(uint32_t event, void *pBuf, uintptr_t arg)
{
  rdGapAdvEventData_t *pData = ICall_malloc(sizeof(rdGapAdvEventData_t));

  if (pData)
  {
    pData->event = event;
    pData->pBuf = pBuf;

    if (RemoteDisplay_enqueueMsg(RD_ADV_EVT, pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      RemoteDisplay_processAdvEvent
 *
 * @brief   Process advertising event in app context
 *
 * @param   pEventData
 */
static void RemoteDisplay_processAdvEvent(rdGapAdvEventData_t *pEventData)
{
  switch (pEventData->event)
  {
    case GAP_EVT_ADV_START_AFTER_ENABLE:
      if (linkDB_NumActive() == 0)
      {
        DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_ADV);
      }
#ifndef CUI_DISABLE
      CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineAdvState, "Adv Set %d Enabled", *(uint8_t *)(pEventData->pBuf));
#endif /* CUI_DISABLE */
      break;

    case GAP_EVT_ADV_END_AFTER_DISABLE:
#ifndef CUI_DISABLE
      CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineAdvState, "Adv Set %d Disabled", *(uint8_t *)(pEventData->pBuf));
#endif /* CUI_DISABLE */
      break;

    case GAP_EVT_ADV_START:
      break;

    case GAP_EVT_ADV_END:
      break;

    case GAP_EVT_ADV_SET_TERMINATED:
    {
#ifndef CUI_DISABLE
      GapAdv_setTerm_t *advSetTerm = (GapAdv_setTerm_t *)(pEventData->pBuf);
      CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineAdvState, "Adv Set %d disabled after conn %d", advSetTerm->handle, advSetTerm->connHandle);
#endif /* CUI_DISABLE */
    }
    break;

    case GAP_EVT_SCAN_REQ_RECEIVED:
      break;

    case GAP_EVT_INSUFFICIENT_MEMORY:
      break;

    default:
      break;
  }

  // All events have associated memory to free except the insufficient memory
  // event
  if (pEventData->event != GAP_EVT_INSUFFICIENT_MEMORY)
  {
    ICall_free(pEventData->pBuf);
  }
}


/*********************************************************************
 * @fn      RemoteDisplay_pairStateCb
 *
 * @brief   Pairing state callback.
 *
 * @return  none
 */
static void RemoteDisplay_pairStateCb(uint16_t connHandle, uint8_t state,
                                         uint8_t status)
{
  rdPairStateData_t *pData = ICall_malloc(sizeof(rdPairStateData_t));

  // Allocate space for the event data.
  if (pData)
  {
    pData->state = state;
    pData->connHandle = connHandle;
    pData->status = status;

    // Queue the event.
    if (RemoteDisplay_enqueueMsg(RD_PAIR_STATE_EVT, pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      RemoteDisplay_passcodeCb
 *
 * @brief   Passcode callback.
 *
 * @return  none
 */
static void RemoteDisplay_passcodeCb(uint8_t *pDeviceAddr,
                                        uint16_t connHandle,
                                        uint8_t uiInputs,
                                        uint8_t uiOutputs,
                                        uint32_t numComparison)
{
  rdPasscodeData_t *pData = ICall_malloc(sizeof(rdPasscodeData_t));

  // Allocate space for the passcode event.
  if (pData )
  {
    pData->connHandle = connHandle;
    memcpy(pData->deviceAddr, pDeviceAddr, B_ADDR_LEN);
    pData->uiInputs = uiInputs;
    pData->uiOutputs = uiOutputs;
    pData->numComparison = numComparison;

    // Enqueue the event.
    if (RemoteDisplay_enqueueMsg(RD_PASSCODE_EVT, pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      RemoteDisplay_processPairState
 *
 * @brief   Process the new paring state.
 *
 * @return  none
 */
static void RemoteDisplay_processPairState(rdPairStateData_t *pPairData)
{
  uint8_t state = pPairData->state;
  uint8_t status = pPairData->status;

  switch (state)
  {
    case GAPBOND_PAIRING_STATE_STARTED:
#ifndef CUI_DISABLE
      CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineConnection, "Pairing started");
      break;

    case GAPBOND_PAIRING_STATE_COMPLETE:
      if (status == SUCCESS)
      {
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineConnection, "Pairing success");
      }
      else
      {
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineConnection, "Pairing fail: %d", status);
      }
      break;
#endif /* CUI_DISABLE */

    case GAPBOND_PAIRING_STATE_ENCRYPTED:
      if (status == SUCCESS)
      {
#ifndef CUI_DISABLE
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineConnection, "Encryption success");
#endif /* CUI_DISABLE */
#ifdef FEATURE_SECURE_COMMISSIONING
        RemoteDisplay_updateConnBondState(pPairData->connHandle, true);
#endif /* FEATURE_SECURE_COMMISSIONING */
      }
      else
      {
#ifndef CUI_DISABLE
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineConnection, "Encryption failed: %d", status);
#endif /* CUI_DISABLE */
#ifdef FEATURE_SECURE_COMMISSIONING
        RemoteDisplay_updateConnBondState(pPairData->connHandle, false);
#endif /* FEATURE_SECURE_COMMISSIONING */
      }
      break;

    case GAPBOND_PAIRING_STATE_BOND_SAVED:
      if (status == SUCCESS)
      {
#ifndef CUI_DISABLE
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineConnection, "Bond save success");
#endif /* CUI_DISABLE */
#ifdef FEATURE_SECURE_COMMISSIONING
        RemoteDisplay_updateConnBondState(pPairData->connHandle, true);
#endif /* FEATURE_SECURE_COMMISSIONING */
      }
      else
      {
#ifndef CUI_DISABLE
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineConnection, "Bond save failed: %d", status);
#endif /* CUI_DISABLE */
#ifdef FEATURE_SECURE_COMMISSIONING
        RemoteDisplay_updateConnBondState(pPairData->connHandle, false);
#endif /* FEATURE_SECURE_COMMISSIONING */
      }
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      RemoteDisplay_processPasscode
 *
 * @brief   Process the Passcode request.
 *
 * @return  none
 */
static void RemoteDisplay_processPasscode(rdPasscodeData_t *pPasscodeData)
{
  // Display passcode to user
  if (pPasscodeData->uiOutputs != 0)
  {
#ifndef CUI_DISABLE
    CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineConnection, "Passcode: %d", B_APP_DEFAULT_PASSCODE);
#endif /* CUI_DISABLE */
  }

  // Send passcode response
  GAPBondMgr_PasscodeRsp(pPasscodeData->connHandle , SUCCESS,
                         B_APP_DEFAULT_PASSCODE);
}

/*********************************************************************
 * @fn      RemoteDisplay_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 */
static status_t RemoteDisplay_enqueueMsg(uint8_t event, void *pData)
{
  uint8_t success;
  rdEvt_t *pMsg = ICall_malloc(sizeof(rdEvt_t));

  // Create dynamic pointer to message.
  if(pMsg)
  {
    pMsg->event = event;
    pMsg->pData = pData;

    // Enqueue the message.
    success = Util_enqueueMsg(appMsgQueueHandle, syncEvent, (uint8_t *)pMsg);
    return (success) ? SUCCESS : FAILURE;
  }

  return(bleMemAllocError);
}

/*********************************************************************
 * @fn      RemoteDisplay_addConn
 *
 * @brief   Add a device to the connected device list
 *
 * @return  index of the connected device list entry where the new connection
 *          info is put in.
 *          if there is no room, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t RemoteDisplay_addConn(uint16_t connHandle)
{
  uint8_t i;
  uint8_t status = bleNoResources;

  // Try to find an available entry
  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == LL_CONNHANDLE_INVALID)
    {
      // Found available entry to put a new connection info in
      connList[i].connHandle = connHandle;

#ifdef FEATURE_SECURE_COMMISSIONING
      // be sure that all sm flags are clear on init of connection
      connList[i].bondedConnection = false;
      connList[i].smAccessAllowed = false;
#endif /* FEATURE_SECURE_COMMISSIONING */

      // Allocate data to send through clock handler
      connList[i].pParamUpdateEventData = ICall_malloc(sizeof(rdClockEventData_t) +
                                          sizeof (uint16_t));
      if(connList[i].pParamUpdateEventData)
      {
          connList[i].pParamUpdateEventData->event = RD_SEND_PARAM_UPDATE_EVT;
        *((uint16_t *)connList[i].pParamUpdateEventData->data) = connHandle;

        // Create a clock object and start
        connList[i].pUpdateClock
          = (Clock_Struct*) ICall_malloc(sizeof(Clock_Struct));

        if (connList[i].pUpdateClock)
        {
          Util_constructClock(connList[i].pUpdateClock,
                            RemoteDisplay_clockHandler,
                            SEND_PARAM_UPDATE_DELAY, 0, true,
                            (UArg) connList[i].pParamUpdateEventData);
        }
        else
        {
            ICall_free(connList[i].pParamUpdateEventData);
        }
      }
      else
      {
        status = bleMemAllocError;
      }

      break;
    }
  }

  return status;
}

/*********************************************************************
 * @fn      RemoteDisplay_getConnIndex
 *
 * @brief   Find index in the connected device list by connHandle
 *
 * @return  the index of the entry that has the given connection handle.
 *          if there is no match, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t RemoteDisplay_getConnIndex(uint16_t connHandle)
{
  uint8_t i;

  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == connHandle)
    {
      return i;
    }
  }

  return(MAX_NUM_BLE_CONNS);
}

/*********************************************************************
 * @fn      RemoteDisplay_clearConnListEntry
 *
 * @brief   Find index in the connected device list by connHandle
 *
 * @return  the index of the entry that has the given connection handle.
 *          if there is no match, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t RemoteDisplay_clearConnListEntry(uint16_t connHandle)
{
  uint8_t i;
  // Set to invalid connection index initially
  uint8_t connIndex = MAX_NUM_BLE_CONNS;

  if(connHandle != LL_CONNHANDLE_ALL)
  {
    // Get connection index from handle
    connIndex = RemoteDisplay_getConnIndex(connHandle);
    if(connIndex >= MAX_NUM_BLE_CONNS)
    {
      return(bleInvalidRange);
    }
  }

  // Clear specific handle or all handles
  for(i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if((connIndex == i) || (connHandle == LL_CONNHANDLE_ALL))
    {
      connList[i].connHandle = LL_CONNHANDLE_INVALID;
      connList[i].currPhy = 0;
      connList[i].phyCngRq = 0;
      connList[i].phyRqFailCnt = 0;
      connList[i].rqPhy = 0;
      memset(connList[i].rssiArr, 0, RD_MAX_RSSI_STORE_DEPTH);
      connList[i].rssiAvg = 0;
      connList[i].rssiCntr = 0;
      connList[i].isAutoPHYEnable = FALSE;
#ifdef FEATURE_SECURE_COMMISSIONING
      connList[i].bondedConnection = false;
      connList[i].smAccessAllowed = false;
#endif /* FEATURE_SECURE_COMMISSIONING */
    }
  }

  return(SUCCESS);
}

/*********************************************************************
 * @fn      RemoteDisplay_clearPendingParamUpdate
 *
 * @brief   clean pending param update request in the paramUpdateList list
 *
 * @param   connHandle - connection handle to clean
 *
 * @return  none
 */
void RemoteDisplay_clearPendingParamUpdate(uint16_t connHandle)
{
  List_Elem *curr;

  for (curr = List_head(&paramUpdateList); curr != NULL; curr = List_next(curr))
  {
    if (((rdConnHandleEntry_t *)curr)->connHandle == connHandle)
    {
      List_remove(&paramUpdateList, curr);
    }
  }
}

/*********************************************************************
 * @fn      RemoteDisplay_removeConn
 *
 * @brief   Remove a device from the connected device list
 *
 * @return  index of the connected device list entry where the new connection
 *          info is removed from.
 *          if connHandle is not found, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t RemoteDisplay_removeConn(uint16_t connHandle)
{
  uint8_t connIndex = RemoteDisplay_getConnIndex(connHandle);

  if(connIndex != MAX_NUM_BLE_CONNS)
  {
    Clock_Struct* pUpdateClock = connList[connIndex].pUpdateClock;

    if (pUpdateClock != NULL)
    {
      // Stop and destruct the RTOS clock if it's still alive
      if (Util_isActive(pUpdateClock))
      {
        Util_stopClock(pUpdateClock);
      }

      // Destruct the clock object
      Clock_destruct(pUpdateClock);
      // Free clock struct
      ICall_free(pUpdateClock);
      // Free ParamUpdateEventData
      ICall_free(connList[connIndex].pParamUpdateEventData);
    }
    // Clear pending update requests from paramUpdateList
    RemoteDisplay_clearPendingParamUpdate(connHandle);
    // Stop Auto PHY Change
    RemoteDisplay_stopAutoPhyChange(connHandle);
    // Clear Connection List Entry
    RemoteDisplay_clearConnListEntry(connHandle);
  }

  return connIndex;
}

/*********************************************************************
 * @fn      RemoteDisplay_processParamUpdate
 *
 * @brief   Process a parameters update request
 *
 * @return  None
 */
static void RemoteDisplay_processParamUpdate(uint16_t connHandle)
{
  gapUpdateLinkParamReq_t req;
  uint8_t connIndex;

  req.connectionHandle = connHandle;
  req.connLatency = DEFAULT_DESIRED_SLAVE_LATENCY;
  req.connTimeout = DEFAULT_DESIRED_CONN_TIMEOUT;
  req.intervalMin = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
  req.intervalMax = DEFAULT_DESIRED_MAX_CONN_INTERVAL;

  connIndex = RemoteDisplay_getConnIndex(connHandle);
  REMOTEDISPLAY_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  // Deconstruct the clock object
  Clock_destruct(connList[connIndex].pUpdateClock);
  // Free clock struct
  if (connList[connIndex].pUpdateClock != NULL)
  {
    ICall_free(connList[connIndex].pUpdateClock);
    connList[connIndex].pUpdateClock = NULL;
  }
  // Free ParamUpdateEventData
  if (connList[connIndex].pParamUpdateEventData != NULL)
  {
    ICall_free(connList[connIndex].pParamUpdateEventData);
  }

  // Send parameter update
  bStatus_t status = GAP_UpdateLinkParamReq(&req);

  // If there is an ongoing update, queue this for when the udpate completes
  if (status == bleAlreadyInRequestedMode)
  {
    rdConnHandleEntry_t *connHandleEntry = ICall_malloc(sizeof(rdConnHandleEntry_t));
    if (connHandleEntry)
    {
        connHandleEntry->connHandle = connHandle;

        List_put(&paramUpdateList, (List_Elem *)connHandleEntry);
    }
  }
}

#ifdef DISPLAY_PER_STATS
/*********************************************************************
 * @fn      RemoteDisplay_processVSCmdCompleteEvt
 *
 * @brief   Process an incoming OSAL HCI EXT Command Complete Event.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void RemoteDisplay_processVSCmdCompleteEvt(hciEvt_VSCmdComplete_t *pMsg)
{
  //Find which command this command complete is for
  switch (pMsg->cmdOpcode)
  {
      case HCI_EXT_PER:
      {
          uint8_t status = pMsg->pEventParam[2];
          if (status == SUCCESS)
          {
              uint8_t cmdVal = pMsg->pEventParam[3];

              if (cmdVal == HCI_EXT_PER_READ)
              {
                uint16_t numPkts = BUILD_UINT16(pMsg->pEventParam[4], pMsg->pEventParam[5]);
                uint16_t numCrcErr = BUILD_UINT16(pMsg->pEventParam[6], pMsg->pEventParam[7]);
                uint16_t numEvents = BUILD_UINT16(pMsg->pEventParam[8], pMsg->pEventParam[9]);
                uint16_t numMissedEvents = BUILD_UINT16(pMsg->pEventParam[10], pMsg->pEventParam[11]);

#ifndef CUI_DISABLE
                CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLinePerStats, "NumPkts = %d, NumCrcErr = %d, NumEvents = %d, NumMissedEvents = %d", numPkts, numCrcErr, numEvents, numMissedEvents);
#endif /* CUI_DISABLE */
              }
          }

          break;
      }
      default:
          break;
  }
}


/*********************************************************************
 * @fn      RemoteDisplay_processUpdatePerEvt
 *
 * @brief   Request a read of the PER counters for a connection.
 *
 * @return  HCI_SUCCESS
 */
static void RemoteDisplay_processUpdatePerEvt(void)
{
    HCI_EXT_PacketErrorRateCmd(0, HCI_EXT_PER_READ);
}
#endif  /* DISPLAY_PER_STATS */

#ifdef DMM_OAD
static void RemoteDisplay_processL2CAPMsg(l2capSignalEvent_t *pMsg)
{
static bool firstRun = TRUE;

switch(pMsg->opcode)
{
  case L2CAP_NUM_CTRL_DATA_PKT_EVT:
  {
    /*
     * We cannot reboot the device immediately after receiving
     * the enable command, we must allow the stack enough time
     * to process and respond to the OAD_EXT_CTRL_ENABLE_IMG
     * command. This command will determine the number of
     * packets currently queued up by the LE controller.
     */
    if(firstRun)
    {
      firstRun = false;

      // We only want to set the numPendingMsgs once
      numPendingMsgs = MAX_NUM_PDU - pMsg->cmd.numCtrlDataPktEvt.numDataPkt;

      // Wait until all PDU have been sent on cxn events
      Gap_RegisterConnEventCb(RemoteDisplay_processConnEvt, GAP_CB_REGISTER,
                              OAD_getactiveCxnHandle());

      /* Set the flag so that the connection event callback will
       * be processed in the context of a pending OAD reboot
       */
      oadWaitReboot = true;
    }
    break;
  }
    default:
      break;
  }
}
#endif

static void RemoteDisplay_processConnEvt(Gap_ConnEventRpt_t *pReport)
{
#ifdef DMM_OAD
  /* If we are waiting for an OAD Reboot, process connection events to ensure
   * that we are not waiting to send data before restarting
   */
  if(oadWaitReboot)
  {
      // Wait until all pending messages are sent
      if(numPendingMsgs == 0)
      {
          // Reset the system
          SysCtrlSystemReset();
      }
      else
      {
        numPendingMsgs--;
      }
  }
  else
  {
    // Process connection events normally
  }
#endif

  // Get index from handle
  uint8_t connIndex = RemoteDisplay_getConnIndex(pReport->handle);

  if (connIndex >= MAX_NUM_BLE_CONNS)
  {
#ifndef CUI_DISABLE
    CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLinePhyStatus, "Connection handle is not in the connList !!!");
#endif /* CUI_DISABLE */
    return;
  }

  // If auto phy change is enabled
  if (connList[connIndex].isAutoPHYEnable == TRUE)
  {
    // Read the RSSI
    HCI_ReadRssiCmd(pReport->handle);
  }
}

#ifdef DMM_OAD
static void RemoteDisplay_processOadWriteCB(uint8_t event, uint16_t arg)
{
  Event_post(syncEvent, event);
}
#endif

/*********************************************************************
 * @fn      RemoteDisplay_connEvtCB
 *
 * @brief   Connection event callback.
 *
 * @param pReport pointer to connection event report
 */
static void RemoteDisplay_connEvtCB(Gap_ConnEventRpt_t *pReport)
{
  // Enqueue the event for processing in the app context.
  if (RemoteDisplay_enqueueMsg(RD_CONN_EVT, pReport) != SUCCESS)
  {
    ICall_free(pReport);
  }
}

#ifndef CUI_DISABLE
/**
 *  @brief Send process menu event.
 */
static void RemoteDisplay_processMenuUpdate(void)
{
  RemoteDisplay_enqueueMsg(RD_UI_INPUT_EVT, NULL);
}
#endif /* CUI_DISABLE */

/*********************************************************************
 * @fn      RemoteDisplay_doSetConnPhy
 *
 * @brief   Set PHY preference.
 *
 * @param   index - 0: 1M PHY
 *                  1: 2M PHY
 *                  2: 1M + 2M PHY
 *                  3: CODED PHY (Long range)
 *                  4: 1M + 2M + CODED PHY
 *
 * @return  always true
 */
bool RemoteDisplay_doSetConnPhy(uint8_t index)
{
  bool status = TRUE;

  static uint8_t phy[] = {
    HCI_PHY_1_MBPS, HCI_PHY_2_MBPS, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS,
    HCI_PHY_CODED, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS | HCI_PHY_CODED,
    AUTO_PHY_UPDATE
  };

  uint8_t connIndex = RemoteDisplay_getConnIndex(connList[0].connHandle);
  if (connIndex >= MAX_NUM_BLE_CONNS)
  {
    return FALSE;
  }

  // Set Phy Preference on the current connection. Apply the same value
  // for RX and TX.
  // If auto PHY update is not selected and if auto PHY update is enabled, then
  // stop auto PHY update
  // Note PHYs are already enabled by default in build_config.opt in stack project.
  if(phy[index] != AUTO_PHY_UPDATE)
  {
      // Cancel RSSI reading  and auto phy changing
      RemoteDisplay_stopAutoPhyChange(connList[connIndex].connHandle);

      if(phy[index] == HCI_PHY_CODED){
          RemoteDisplay_setPhy(connList[connIndex].connHandle, 0, phy[index], phy[index], LL_PHY_OPT_S2);
      }
      else {
          RemoteDisplay_setPhy(connList[connIndex].connHandle, 0, phy[index], phy[index], LL_PHY_OPT_NONE);
      }
  }
  else
  {
      // Start RSSI read for auto PHY update (if it is disabled)
      RemoteDisplay_startAutoPhyChange(connList[connIndex].connHandle);
  }

  return status;
}


/*********************************************************************
 * @fn      RemoteDisplay_processCmdCompleteEvt
 *
 * @brief   Process an incoming OSAL HCI Command Complete Event.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void RemoteDisplay_processCmdCompleteEvt(hciEvt_CmdComplete_t *pMsg)
{
  uint8_t status = pMsg->pReturnParam[0];

  //Find which command this command complete is for
  switch (pMsg->cmdOpcode)
  {
    case HCI_READ_RSSI:
    {
      int8 rssi = (int8)pMsg->pReturnParam[3];

      // Display RSSI value, if RSSI is higher than threshold, change to faster PHY
      if (status == SUCCESS)
      {
        uint16_t handle = BUILD_UINT16(pMsg->pReturnParam[1], pMsg->pReturnParam[2]);

        uint8_t index = RemoteDisplay_getConnIndex(handle);
        if (index >= MAX_NUM_BLE_CONNS)
        {
#ifndef CUI_DISABLE
          CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLinePhyStatus, "Connection handle is not in the connList !!!");
#endif /* CUI_DISABLE */
          return;
        }

        if (rssi != LL_RSSI_NOT_AVAILABLE)
        {
          connList[index].rssiArr[connList[index].rssiCntr++] = rssi;
          connList[index].rssiCntr %= RD_MAX_RSSI_STORE_DEPTH;

          int16_t sum_rssi = 0;
          for(uint8_t cnt=0; cnt<RD_MAX_RSSI_STORE_DEPTH; cnt++)
          {
            sum_rssi += connList[index].rssiArr[cnt];
          }
          connList[index].rssiAvg = (uint32_t)(sum_rssi/RD_MAX_RSSI_STORE_DEPTH);

          uint8_t phyRq = RD_PHY_NONE;
          uint8_t phyRqS = RD_PHY_NONE;
          uint8_t phyOpt = LL_PHY_OPT_NONE;

          if(connList[index].phyCngRq == FALSE)
          {
            if((connList[index].rssiAvg >= RSSI_2M_THRSHLD) &&
            (connList[index].currPhy != HCI_PHY_2_MBPS) &&
                 (connList[index].currPhy != RD_PHY_NONE))
            {
              // try to go to higher data rate
              phyRqS = phyRq = HCI_PHY_2_MBPS;
            }
            else if((connList[index].rssiAvg < RSSI_2M_THRSHLD) &&
                    (connList[index].rssiAvg >= RSSI_1M_THRSHLD) &&
                    (connList[index].currPhy != HCI_PHY_1_MBPS) &&
                    (connList[index].currPhy != RD_PHY_NONE))
            {
              // try to go to legacy regular data rate
              phyRqS = phyRq = HCI_PHY_1_MBPS;
            }
            else if((connList[index].rssiAvg >= RSSI_S2_THRSHLD) &&
                    (connList[index].rssiAvg < RSSI_1M_THRSHLD) &&
                    (connList[index].currPhy != RD_PHY_NONE))
            {
              // try to go to lower data rate S=2(500kb/s)
              phyRqS = HCI_PHY_CODED;
              phyOpt = LL_PHY_OPT_S2;
              phyRq = BLE5_CODED_S2_PHY;
            }
            else if(connList[index].rssiAvg < RSSI_S2_THRSHLD )
            {
              // try to go to lowest data rate S=8(125kb/s)
              phyRqS = HCI_PHY_CODED;
              phyOpt = LL_PHY_OPT_S8;
              phyRq = BLE5_CODED_S8_PHY;
            }
            if((phyRq != RD_PHY_NONE) &&
               // First check if the request for this phy change is already not honored then don't request for change
               (((connList[index].rqPhy == phyRq) &&
                 (connList[index].phyRqFailCnt < 2)) ||
                 (connList[index].rqPhy != phyRq)))
            {
              //Initiate PHY change based on RSSI
              RemoteDisplay_setPhy(connList[index].connHandle, 0,
                                      phyRqS, phyRqS, phyOpt);
              connList[index].phyCngRq = TRUE;

              // If it a request for different phy than failed request, reset the count
              if(connList[index].rqPhy != phyRq)
              {
                // then reset the request phy counter and requested phy
                connList[index].phyRqFailCnt = 0;
              }

              if(phyOpt == LL_PHY_OPT_NONE)
              {
                connList[index].rqPhy = phyRq;
              }
              else if(phyOpt == LL_PHY_OPT_S2)
              {
                connList[index].rqPhy = BLE5_CODED_S2_PHY;
              }
              else
              {
                connList[index].rqPhy = BLE5_CODED_S8_PHY;
              }

            } // end of if ((phyRq != RD_PHY_NONE) && ...
          } // end of if (connList[index].phyCngRq == FALSE)
        } // end of if (rssi != LL_RSSI_NOT_AVAILABLE)

#ifndef CUI_DISABLE
        CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineRssiStatus, "RSSI:%d dBm, AVG RSSI:%d dBm", (uint32_t)(rssi), connList[index].rssiAvg);
#endif /* CUI_DISABLE */
	      } // end of if (status == SUCCESS)
      break;
    }

    default:
      break;
  } // end of switch (pMsg->cmdOpcode)
}


/*********************************************************************
* @fn      RemoteDisplay_initPHYRSSIArray
*
* @brief   Initializes the array of structure/s to store data related
*          RSSI based auto PHy change
*
* @param   connHandle - the connection handle
*
* @param   addr - pointer to device address
*
* @return  index of connection handle
*/
static void RemoteDisplay_initPHYRSSIArray(void)
{
  //Initialize array to store connection handle and RSSI values
  memset(connList, 0, sizeof(connList));
  for (uint8_t index = 0; index < MAX_NUM_BLE_CONNS; index++)
  {
    connList[index].connHandle = RD_INVALID_HANDLE;
  }
}

/*********************************************************************
 * @fn      RemoteDisplay_startAutoPhyChange
 *
 * @brief   Start periodic RSSI reads on a link.
 *
 * @param   connHandle - connection handle of link
 * @param   devAddr - device address
 *
 * @return  SUCCESS: Terminate started
 *          bleIncorrectMode: No link
 *          bleNoResources: No resources
 */
static status_t RemoteDisplay_startAutoPhyChange(uint16_t connHandle)
{
  status_t status = FAILURE;

  // Get connection index from handle
  uint8_t connIndex = RemoteDisplay_getConnIndex(connHandle);
  REMOTEDISPLAY_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  // Start Connection Event notice for RSSI calculation
  status = Gap_RegisterConnEventCb(RemoteDisplay_connEvtCB, GAP_CB_REGISTER, connHandle);

  // Flag in connection info if successful
  if (status == SUCCESS)
  {
    connList[connIndex].isAutoPHYEnable = TRUE;
  }

  return status;
}

/*********************************************************************
 * @fn      RemoteDisplay_stopAutoPhyChange
 *
 * @brief   Cancel periodic RSSI reads on a link.
 *
 * @param   connHandle - connection handle of link
 *
 * @return  SUCCESS: Operation successful
 *          bleIncorrectMode: No link
 */
static status_t RemoteDisplay_stopAutoPhyChange(uint16_t connHandle)
{
  // Get connection index from handle
  uint8_t connIndex = RemoteDisplay_getConnIndex(connHandle);
  REMOTEDISPLAY_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  // Stop connection event notice
  Gap_RegisterConnEventCb(NULL, GAP_CB_UNREGISTER, connHandle);

  // Also update the phychange request status for active RSSI tracking connection
  connList[connIndex].phyCngRq = FALSE;
  connList[connIndex].isAutoPHYEnable = FALSE;

  return SUCCESS;
}

/*********************************************************************
 * @fn      RemoteDisplay_setPhy
 *
 * @brief   Call the HCI set phy API and and add the handle to a
 *          list to match it to an incoming command status event
 */
static status_t RemoteDisplay_setPhy(uint16_t connHandle, uint8_t allPhys,
                                        uint8_t txPhy, uint8_t rxPhy,
                                        uint16_t phyOpts)
{
  // Allocate list entry to store handle for command status
  rdConnHandleEntry_t *connHandleEntry = ICall_malloc(sizeof(rdConnHandleEntry_t));

  if (connHandleEntry)
  {
    connHandleEntry->connHandle = connHandle;

    // Add entry to the phy command status list
    List_put(&setPhyCommStatList, (List_Elem *)connHandleEntry);

    // Send PHY Update
    HCI_LE_SetPhyCmd(connHandle, allPhys, txPhy, rxPhy, phyOpts);
  }

  return SUCCESS;
}

/*********************************************************************
* @fn      RemoteDisplay_updatePHYStat
*
* @brief   Update the auto phy update state machine
*
* @param   connHandle - the connection handle
*
* @return  None
*/
static void RemoteDisplay_updatePHYStat(uint16_t eventCode, uint8_t *pMsg)
{
  uint8_t connIndex;

  switch (eventCode)
  {
    case HCI_LE_SET_PHY:
    {
      // Get connection handle from list
      rdConnHandleEntry_t *connHandleEntry = (rdConnHandleEntry_t *)List_get(&setPhyCommStatList);

      if (connHandleEntry)
      {
        // Get index from connection handle
        connIndex = RemoteDisplay_getConnIndex(connHandleEntry->connHandle);

        ICall_free(connHandleEntry);

        // Is this connection still valid?
        if (connIndex < MAX_NUM_BLE_CONNS)
        {
          hciEvt_CommandStatus_t *pMyMsg = (hciEvt_CommandStatus_t *)pMsg;

          if (pMyMsg->cmdStatus == HCI_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE)
          {
            // Update the phychange request status for active RSSI tracking connection
            connList[connIndex].phyCngRq = FALSE;
            connList[connIndex].phyRqFailCnt++;
          }
        }
      }
      break;
    }

    // LE Event - a Phy update has completed or failed
    case HCI_BLE_PHY_UPDATE_COMPLETE_EVENT:
    {
      hciEvt_BLEPhyUpdateComplete_t *pPUC = (hciEvt_BLEPhyUpdateComplete_t*) pMsg;

      if(pPUC)
      {
        // Get index from connection handle
        connIndex = RemoteDisplay_getConnIndex(pPUC->connHandle);

        // Is this connection still valid?
        if (connIndex < MAX_NUM_BLE_CONNS)
        {
          // Update the phychange request status for active RSSI tracking connection
          connList[connIndex].phyCngRq = FALSE;

          if (pPUC->status == SUCCESS)
          {
            connList[connIndex].currPhy = pPUC->rxPhy;
          }
          if(pPUC->rxPhy != connList[connIndex].rqPhy)
          {
            connList[connIndex].phyRqFailCnt++;
          }
          else
          {
            // Reset the request phy counter and requested phy
            connList[connIndex].phyRqFailCnt = 0;
            connList[connIndex].rqPhy = 0;
          }
        }
      }

      break;
    }

    default:
      break;
  } // end of switch (eventCode)
}

#ifdef FEATURE_SECURE_COMMISSIONING
/*********************************************************************
* @fn      RemoteDisplay_updateSmState
*
* @brief   Update the Remote Display SM state machine
*
* @param   smState - the SM State being set
*
* @return  None
*/
void RemoteDisplay_updateSmState(uint8_t smState){
  uint8_t *pValue = ICall_malloc(sizeof(uint8_t));

  if (pValue)
  {
    *pValue = smState;
    if (RemoteDisplay_enqueueMsg(SM_SENSOR_STATE_CHANGE_EVT, pValue) != SUCCESS)
    {
      ICall_free(pValue);
    }
  }
}

/*********************************************************************
* @fn      RemoteDisplay_getBleAuthConnectionStatus
*
* @brief   Determine if the device currently has a bonded connection
*
* @return  bool - True if at least one bonded connection exists
*/
bool RemoteDisplay_getBleAuthConnectionStatus(void){
  uint8_t i;

  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].bondedConnection == true)
    {
      return true;
    }
  }
  return false;
}

/*********************************************************************
* @fn      RemoteDisplay_updateConnBondState
*
* @brief   update the Connection Bond Status of a specific connection handle
*
* @param   connectionHandle - BLE connection handle to update
* @param   status - boolean to define if the BLE connection is bonded
*
* @return  None
*/
void RemoteDisplay_updateConnBondState(uint16_t connectionHandle, bool status){
  uint8_t i;

  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == connectionHandle)
    {
      connList[i].bondedConnection = status;
      return;
    }
  }
}

/*********************************************************************
* @fn      RemoteDisplay_checkSmAccessAllowed
*
* @brief   update the Connection Bond Status of a specific connection handle
*
* @param   connectionHandle - BLE connection handle to update
*
* @return  bool - True if the BLE connection has access to SM characteristics
*/
bool RemoteDisplay_checkSmAccessAllowed(uint16_t connectionHandle){
  uint8_t i;

  // first check if our connection already has SM access
  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].smAccessAllowed == true)
    {
      if (connList[i].connHandle == connectionHandle)
      {
        // this connection already has access to SM
        return true;
      }
      else
      {
        // found a different connection that already has access to SM
        return false;
      }
    }
  }

  // no connection has access so allow the current connection in question access
  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == connectionHandle && connList[i].bondedConnection == true)
    {
      connList[i].smAccessAllowed = true;
      return true;
     }
  }

  // the connection did not have a boned connection
  return false;
}
#endif /* FEATURE_SECURE_COMMISSIONING */

#if defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE)
/*********************************************************************
 * @fn      RemoteDisplay_blockModeTestOn
 *
 * @brief   Turn the periodic block mode on for BLE.
 *
 * @param   menuEntryIndex - index of CUI menu option
 */
static void RemoteDisplay_blockModeTestOn(int32_t menuEntryIndex)
{
  if (!Util_isActive(&clkBlockModeTest))
  {
    Util_rescheduleClock(&clkBlockModeTest, RD_BLOCK_MODE_ON_PERIOD);
    Util_startClock(&clkBlockModeTest);
    DMMPolicy_setBlockModeOn(DMMPolicy_StackRole_BlePeripheral);
    CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineBlockModeTestState, "Enabled");
  }
}

/*********************************************************************
 * @fn      RemoteDisplay_blockModeTestOff
 *
 * @brief   Turn the periodic block mode off for BLE.
 *
 * @param   menuEntryIndex - index of CUI menu option
 */
static void RemoteDisplay_blockModeTestOff(int32_t menuEntryIndex)
{
  if (Util_isActive(&clkBlockModeTest))
  {
    Util_stopClock(&clkBlockModeTest);
  }
  DMMPolicy_setBlockModeOff(DMMPolicy_StackRole_BlePeripheral);
  CUI_statusLinePrintf(remoteDisplayCuiHndl, rdStatusLineBlockModeTestState, "Disabled");
}


/*********************************************************************
 * @fn      RemoteDisplay_setBlockModeOnPeriodAction
 *
 * @brief   Handle a user input to update the Block Mode On Period value
 *
 * @param   _input - input character
 * @param   _pLines - action menu title
 * @param   _pCurInfo - current cursor info
 */
static void RemoteDisplay_setBlockModeOnPeriodAction(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo)
{
  RemoteDisplay_setBlockModePeriodUiAction(&RD_BLOCK_MODE_ON_PERIOD, _input, _pLines, _pCurInfo);
}

/*********************************************************************
 * @fn      RemoteDisplay_setBlockModeOffPeriodAction
 *
 * @brief   Handle a user input to update the Block Mode Off Period value
 *
 * @param   _input - input character
 * @param   _pLines - action menu title
 * @param   _pCurInfo - current cursor info
 */
static void RemoteDisplay_setBlockModeOffPeriodAction(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo)
{
  RemoteDisplay_setBlockModePeriodUiAction(&RD_BLOCK_MODE_OFF_PERIOD, _input, _pLines, _pCurInfo);
}


/*********************************************************************
 * @fn      RemoteDisplay_setBlockModePeriodUiAction
 *
 * @brief   Handle a user input to update the Block Mode Off Period value
 *
 * @param   blockModePeriod - Block Mode Period
 * @param   _input - input character
 * @param   _pLines - action menu title
 * @param   _pCurInfo - current cursor info
 */
static void RemoteDisplay_setBlockModePeriodUiAction(uint16_t* blockModePeriod, const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo)
{
  static char periodValArr[4] = {};
  static CUI_cursorInfo_t cursor = {0, 4};

  switch (_input) {
    case CUI_ITEM_INTERCEPT_START:
    {
      sprintf(periodValArr, "%04d", *blockModePeriod);
      break;
    }
    // Submit the final modified value
    case CUI_ITEM_INTERCEPT_STOP:
    {
      *blockModePeriod = atoi(periodValArr);
      // Reset the local cursor info
      cursor.col = 4;
      break;
    }
    // Move the cursor to the left
    case CUI_INPUT_LEFT:
    {
      cursor.col = moveCursorLeft(cursor.col, 4, 7, 0);
      break;
    }
    // Move the cursor to the right
    case CUI_INPUT_RIGHT:
    {
      cursor.col = moveCursorRight(cursor.col, 4, 7, 0);
      break;
    }
    default:
    {
      // is the input a number
      if(CUI_IS_INPUT_NUM(_input))
      {
        periodValArr[cursor.col - 4] = _input;
        cursor.col = moveCursorRight(cursor.col, 4, 7, 0);
      }
      else
      {
        sprintf(periodValArr, "%04d", *blockModePeriod);
      }
    }
  }

  snprintf(_pLines[0], 16, "    %04s      ", periodValArr);

  if (_input != CUI_ITEM_PREVIEW)
  {
    if (blockModePeriod == &RD_BLOCK_MODE_ON_PERIOD)
    {
      strcpy(_pLines[2], "BM ON Period (ms)");
    }
    else if (blockModePeriod == &RD_BLOCK_MODE_OFF_PERIOD)
    {
      strcpy(_pLines[2], "BM OFF Period (ms)");
    }
    _pCurInfo->row = 1;
    _pCurInfo->col = cursor.col+1;
  }
}


/**
 *  @brief Callback to be called when the UI move a cursor Left.
 */
static uint8_t moveCursorLeft(uint8_t col, uint8_t left_boundary, uint8_t right_boundary, uint8_t skip_space)
{
  // If you haven't hit the end of left boundary, keep moving cursor left.
  if (left_boundary != col)
  {
    col--;
  }
  else
  {
    col = right_boundary;
  }

  if (0 != skip_space)
  {
    //skip the white space, by continuing to move left over it
    if ((col % 3) == 0)
      col--;
  }
  return col;
}

/**
 *  @brief Callback to be called when the UI moves a cursor right.
 */
static uint8_t moveCursorRight(uint8_t col, uint8_t left_boundary, uint8_t right_boundary, uint8_t skip_space)
{
  // If you haven't hit the end of modifiable lines, keep moving cursor right.
  if (right_boundary != col)
  {
    col++;
  }
  else
  {
    col = left_boundary;
  }

  // if skip_space is true
  if(0 != skip_space)
  {
    //skip the white space, by continuing to move right over it
    if((col % 3) == 0)
      col++;
  }
  return col;
}

#endif /* defined(BLOCK_MODE_TEST) && !defined(CUI_DISABLE) */
