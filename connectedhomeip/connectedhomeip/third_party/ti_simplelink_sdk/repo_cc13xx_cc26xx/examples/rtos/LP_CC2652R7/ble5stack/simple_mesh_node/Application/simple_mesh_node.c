/******************************************************************************
 *
 *

@file  simple_mesh_node.c

@brief This file contains the simple_peripheral_and_mesh sample application for use
with the CC2652 Bluetooth Low Energy Protocol Stack.

Group: WCS, BTS
Target Device: cc13xx_cc26xx

******************************************************************************

 Copyright (c) 2013-2022, Texas Instruments Incorporated
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

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>

#include <ti/display/Display.h>

#include "autoconf.h"

#define ADD_SENSOR_MODELS

#if !(defined __TI_COMPILER_VERSION__) && !(defined __GNUC__)
#include <intrinsics.h>
#endif

#include <ti/drivers/utils/List.h>

#include "util.h"
#include <bcomdef.h>
#include "ble_stack_api.h"

#include <devinfoservice.h>
#include <simple_gatt_profile.h>

#ifdef USE_RCOSC
#include <rcosc_calibration.h>
#endif //USE_RCOSC

#include <ti_drivers_config.h>
#include <board_key.h>

#include <menu/two_btn_menu.h>
#include "simple_mesh_node.h"
#include "simple_mesh_node_menu.h"
#include "ti_ble_config.h"
#include "ti_device_composition.h"
#if defined(OAD_SUPPORT)
#include "oad_app_support.h"
#endif
#include <zephyr.h>
#include <sys/printk.h>
#include <settings/settings.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>
#include <bt_le_porting_layer.h>

#ifdef USE_STATIC_PROVISIONING
#include "static_prov.h"
#endif

#include "generic_models.h"
#include "sensor_models.h"
#include "sensor_temperature8.h"

#include "mesh_erpc_wrapper.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
* CONSTANTS
*/
// How often to perform periodic event (in ms)
#define BLEAPP_PERIODIC_EVT_PERIOD               2000


// Task configuration
#define BLEAPP_TASK_PRIORITY                     3

#ifndef BLEAPP_TASK_STACK_SIZE
#define BLEAPP_TASK_STACK_SIZE                   2248
#endif

// Application events
#define BLEAPP_EVT_CHAR_CHANGE         1
#define BLEAPP_EVT_KEY_CHANGE          2
#define BLEAPP_EVT_ADV                 3
#define BLEAPP_EVT_PAIRING_STATE       4
#define BLEAPP_EVT_PASSCODE_NEEDED     5
#define BLEAPP_EVT_PERIODIC            6
#define BLEAPP_EVT_READ_RPA            7
#define BLEAPP_EVT_SEND_PARAM_UPDATE   8
#define BLEAPP_EVT_CONN                9
#define BLEAPP_EVT_INSUFFICIENT_MEM    10
#define BLEAPP_EVT_ADV_REPORT          11
#define BLEAPP_EVT_SCAN_ENABLED        12
#define BLEAPP_EVT_SCAN_DISABLED       13
#define MESHAPP_EVT_ADV                14
#define MESHAPP_EVT_GATT_WRT           15
#define BLEAPP_EVT_STACK_CALLBACK      16
#define OADAPP_EVT_CALLBACK            17
#define OADAPP_EVT_RESET               18

// For storing the active connections
#define SP_RSSI_TRACK_CHNLS        1            // Max possible channels can be GAP_BONDINGS_MAX
#define SP_MAX_RSSI_STORE_DEPTH    5
#define SP_INVALID_HANDLE          0xFFFF
#define RSSI_2M_THRSHLD           -30
#define RSSI_1M_THRSHLD           -40
#define RSSI_S2_THRSHLD           -50
#define RSSI_S8_THRSHLD           -60
#define SP_PHY_NONE                LL_PHY_NONE  // No PHY set
#define AUTO_PHY_UPDATE            0xFF

// Spin if the expression is not true
#define BLEAPP_ASSERT(expr) if (!(expr)) bleApp_spin();

// address string length is an ascii character for each digit +
#define SMN_ADDR_STR_SIZE     15
#define APP_QUEUE_EVT         UTIL_QUEUE_EVENT_ID // Event_Id_30
#define EVENT_PEND_FOREVER    0xfffffffful

/*********************************************************************
* TYPEDEFS
*/

// App event passed from stack modules. This type is defined by the application
// since it can queue events to itself however it wants.
typedef struct
{
  uint8_t event;                // event type
  void    *pData;               // pointer to message
} appEvt_t;

// Container to store passcode data when passing from gapbondmgr callback
// to app event. See the pfnPairStateCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct
{
  uint8_t state;
  uint16_t connHandle;
  uint8_t status;
} appPairStateData_t;

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
} appPasscodeData_t;

// Container to store advertising event data when passing from advertising
// callback to app event. See the respective event in GapAdvScan_Event_IDs
// in gap_advertiser.h for the type that pBuf should be cast to.
typedef struct
{
  uint32_t event;
  void *pBuf;
} appGapAdvEventData_t;

// Container to store information from clock expiration using a flexible array
// since data is not always needed
typedef struct
{
  uint8_t event;
  uint8_t data[];
} appClockEventData_t;

// Scanned device information record
typedef struct
{
  uint8_t addrType;         // Peer Device's Address Type
  uint8_t addr[B_ADDR_LEN]; // Peer Device Address
} scanRec_t;

// Container to store all scan callback params
typedef struct
{
  uint32_t evt;
  void* pMsg;
  uintptr_t arg;
} scanCbData_t;

#ifdef ADD_SIMPLE_PERIPHERAL

// List element for parameter update and PHY command status lists
typedef struct
{
  List_Elem elem;
  uint16_t  connHandle;
} spConnHandleEntry_t;

// Connected device information
typedef struct
{
  uint16_t              connHandle;                        // Connection Handle
  appClockEventData_t*   pParamUpdateEventData;
  Clock_Struct*         pUpdateClock;                      // pointer to clock struct
  int8_t                rssiArr[SP_MAX_RSSI_STORE_DEPTH];
  uint8_t               rssiCntr;
  int8_t                rssiAvg;
  bool                  phyCngRq;                          // Set to true if PHY change request is in progress
  uint8_t               currPhy;
  uint8_t               rqPhy;
  uint8_t               phyRqFailCnt;                      // PHY change request count
  bool                  isAutoPHYEnable;                   // Flag to indicate auto phy change
} spConnRec_t;
#endif // ADD_SIMPLE_PERIPHERAL

/*********************************************************************
* GLOBAL VARIABLES
*/
// Display Interface
Display_Handle dispHandle = NULL;

// Task configuration
Task_Struct appTask;
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(appTaskStack, 8)
#else
#pragma data_alignment=8
#endif
uint8_t appTaskStack[BLEAPP_TASK_STACK_SIZE];


#define BLEAPP_EVT_EVENT_MAX  19
char *appEventStrings[] = {
  "BLEAPP_ZERO                 ",
  "BLEAPP_EVT_CHAR_CHANGE      ",
  "BLEAPP_EVT_KEY_CHANGE       ",
  "BLEAPP_EVT_ADV              ",
  "BLEAPP_EVT_PAIRING_STATE    ",
  "BLEAPP_EVT_PASSCODE_NEEDED  ",
  "BLEAPP_EVT_PERIODIC         ",
  "BLEAPP_EVT_READ_RPA         ",
  "BLEAPP_EVT_SEND_PARAM_UPDATE",
  "BLEAPP_EVT_CONN             ",
  "BLEAPP_EVT_INSUFFICIENT_MEM ",
  "BLEAPP_EVT_ADV_REPORT       ",
  "BLEAPP_EVT_SCAN_ENABLED     ",
  "BLEAPP_EVT_SCAN_DISABLED    ",
  "MESHAPP_EVT_ADV             ",
  "MESHAPP_EVT_GATT_WRT        ",
  "BLEAPP_EVT_STACK_CALLBACK   ",
  "OADAPP_EVT_CALLBACK         ",
  "OADAPP_EVT_RESET            ",
};

/*********************************************************************
* LOCAL VARIABLES
*/

// Entity ID globally used to check for source and/or destination of messages
bleStack_entityId_t appSelfEntity;
bleStack_entityId_t meshAppSelfEntity;  //extern in bt.c
static Event_Handle appSyncEvent;

// Queue object used for app messages
static Queue_Struct appMsgQueue;
static Queue_Handle appMsgQueueHandle;

// Clock instance for internal periodic events. Only one is needed since
// GattServApp will handle notifying all connected GATT clients
static Clock_Struct clkPeriodic;
// Clock instance for RPA read events.
static Clock_Struct clkRpaRead;

// Memory to pass periodic event ID to clock handler
appClockEventData_t argPeriodic =
{ .event = BLEAPP_EVT_PERIODIC };

// Memory to pass RPA read event ID to clock handler
appClockEventData_t argRpaRead =
{ .event = BLEAPP_EVT_READ_RPA };

#ifdef ADD_SIMPLE_PERIPHERAL
// Per-handle connection info
spConnRec_t connList[MAX_NUM_BLE_CONNS];

// Current connection handle as chosen by menu
static uint16_t menuConnHandle = LINKDB_CONNHANDLE_INVALID;

// List to store connection handles for set phy command status's
static List_List setPhyCommStatList;

// List to store connection handles for queued param updates
static List_List paramUpdateList;

// Auto connect Disabled/Enabled {0 - Disabled, 1- Group A , 2-Group B, ...}
uint8_t autoConnect = AUTOCONNECT_DISABLE;

// Advertising handles
static uint8 advHandle1;
#ifdef SECOND_ADV_SET
static uint8 advHandle2;
#endif /* SECOND_ADV_SET */
#endif // ADD_SIMPLE_PERIPHERAL

// Address mode
static GAP_Addr_Modes_t addrMode = ADDRMODE_PUBLIC;//DEFAULT_ADDRESS_MODE;

#ifdef ADD_SIMPLE_PERIPHERAL
// Current Random Private Address
static uint8 rpa[B_ADDR_LEN] = {0};
#endif // ADD_SIMPLE_PERIPHERAL

#ifdef USE_STATIC_PROVISIONING
uint16_t mesh_own_addr = DEVICE_OWN_ADDRESS;
#else
uint16_t mesh_own_addr = 1;
#endif
int advCb_count = 0;
int scanCb_count = 0;
int scanCb_beaconCount = 0;
int scanCb_msgCount = 0;
int scanCb_provCount = 0;
#ifdef CONFIG_BT_MESH_PROXY
static uint8_t meshScanData[] = "SimpleMeshNode";
#endif

// Used for save and restore the default device UUID
uint8_t defaultUUID[UUID_MAX_LEN];

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void     bleApp_init( void );
static void     bleApp_taskFxn(UArg a0, UArg a1);
static void     bleApp_processAppMsg(appEvt_t *pMsg);
static uint8_t  bleApp_processStackMsg(bleStack_msgHdt_t *pMsg);
static uint8_t  bleApp_processGATTMsg(gattMsgEvent_t *pMsg);
static void     bleApp_processGapMessage(gapEventHdr_t *pMsg);
#ifdef USE_APP_MENU
static void     bleApp_handleKeys(uint8_t keys);
#endif
static void     bleApp_keyChangeHandler(uint8_t keys);
static status_t bleApp_enqueueMsg(uint8_t event, void *pData);
static void     bleApp_performPeriodicTask(void);
static void     bleApp_clockHandler(UArg arg);
static void     bleApp_passcodeCB(uint8_t *pDeviceAddr, uint16_t connHandle,
                                            uint8_t uiInputs, uint8_t uiOutputs,
                                            uint32_t numComparison);
static void     bleApp_pairStateCb(uint16_t connHandle, uint8_t state,
                                             uint8_t status);
static void     meshApp_processAdvEvent(appGapAdvEventData_t *pEventData);

#ifdef ADD_SIMPLE_PERIPHERAL
static void bleApp_processAdvEvent(appGapAdvEventData_t *pEventData);
static void bleApp_charValueChangeCB(uint8_t paramId);
static void bleApp_processCharValueChangeEvt(uint8_t paramId);
static void SimplePeripheral_advCallback(uint32_t event, void *pBuf, uintptr_t arg);
static void SimplePeripheral_updateRPA(void);
static void bleApp_processPairState(appPairStateData_t *pPairState);
static void bleApp_processPasscode(appPasscodeData_t *pPasscodeData);
static void bleApp_processCmdCompleteEvt(hciEvt_CmdComplete_t *pMsg);
static void SimplePeripheral_initPHYRSSIArray(void);
static void SimplePeripheral_updatePHYStat(uint16_t eventCode, uint8_t *pMsg);
static uint8_t SimplePeripheral_addConn(uint16_t connHandle);
static uint8_t SimplePeripheral_getConnIndex(uint16_t connHandle);
static uint8_t SimplePeripheral_removeConn(uint16_t connHandle);
static void bleApp_processParamUpdate(uint16_t connHandle);
static status_t SimplePeripheral_startAutoPhyChange(uint16_t connHandle);
static status_t SimplePeripheral_stopAutoPhyChange(uint16_t connHandle);
static status_t SimplePeripheral_setPhy(uint16_t connHandle, uint8_t allPhys,
                                        uint8_t txPhy, uint8_t rxPhy,
                                        uint16_t phyOpts);
static uint8_t SimplePeripheral_clearConnListEntry(uint16_t connHandle);
void SimplePeripheral_connEvtCB(Gap_ConnEventRpt_t *pReport);
static void bleApp_processConnEvt(Gap_ConnEventRpt_t *pReport);
static bStatus_t SimplePeripheral_initAdvSet(pfnGapCB_t advCallback,
                                             uint8_t * const advHandle, GapAdv_params_t * const advParams,
                                             uint8_t advData[], const int advDataLen,
                                             uint8_t scanRespData[], const int scanRespDataLen);
#ifdef PTM_MODE
void PTM_init(bleStack_entityId_t appSelfEntity);
void PTM_handle_hci_event(uint8_t *pMsg);
#endif // PTM_MODE
#endif // ADD_SIMPLE_PERIPHERAL

uint8_t bleApp_processStackMsgCB(uint8_t event, uint8_t *pMsg);

/* Mesh profile */
int generic_models_extensions(void);
int MeshApp_init(uint16_t addr, uint8_t prov_type);
int MeshApp_unprovisioned_beacon(void);

/* Mesh Callbacks */
#ifdef USE_APP_MENU

#ifdef CONFIG_BT_MESH_LOW_POWER
static void meshApp_lpn_friendshipEstablished(uint16_t net_idx, uint16_t friend_addr,
		    uint8_t queue_size, uint8_t recv_window);
static void meshApp_lpn_friendshipTerminated(uint16_t net_idx, uint16_t friend_addr);
static void meshApp_lpnPolled(uint16_t net_idx, uint16_t friend_addr, bool retry);
#endif /* CONFIG_BT_MESH_LOW_POWER */

static void meshApp_hbRecv(const struct bt_mesh_hb_sub *sub, uint8_t hops,
	     uint16_t feat);
static void meshApp_hb_subEnd(const struct bt_mesh_hb_sub *sub);

#ifdef CONFIG_BT_MESH_FRIEND
static void meshApp_friend_friendshipEstablished(uint16_t net_idx, uint16_t lpn_addr,
			    uint8_t recv_delay, uint32_t polltimeout);
static void meshApp_friend_friendshipTerminated(uint16_t net_idx, uint16_t lpn_addr);
#endif /* CONFIG_BT_MESH_FRIEND */

#endif /* USE_APP_MENU */


/*********************************************************************
 * EXTERN FUNCTIONS
*/
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);
extern void mesh_portingLayer_scanCB(uint32_t evt, void* pMsg, uintptr_t arg);
extern void mesh_portingLayer_advCB(uint32_t event, void *pBuf);
extern int k_sys_work_q_init(void *dev);

#ifdef CONFIG_BT_MESH_PROXY
extern Event_Handle workQThreadSyncEvent;
#endif
#ifdef MESH_ERPC
extern void eRpcMesh_createTask();
// ERPC thread entity ID globally used to check for source and/or
// destination of messages
bleStack_entityId_t erpcThreadSelfEntity;    // eRPC thread
// ERPC thread event globally used to post local events and pend on
// system and local events
Event_Handle erpcThreadSyncEvent;    // eRPC thread
#endif

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Bond Manager Callbacks
static gapBondCBs_t bleApp_bondMgrCBs =
{
  bleApp_passcodeCB,       // Passcode callback
  bleApp_pairStateCb       // Pairing/Bonding state Callback
};

#ifdef ADD_SIMPLE_PERIPHERAL
// Simple GATT Profile Callbacks
static simpleProfileCBs_t bleApp_simpleProfileCBs =
{
  bleApp_charValueChangeCB // Simple GATT Characteristic value change callback
};
#endif

/*********************************************************************
 * Mesh Callbacks Global Variables
 */
#ifdef USE_APP_MENU

BT_MESH_HB_CB_DEFINE(meshApp_hb_cb) = {
  .recv    = meshApp_hbRecv,
  .sub_end = meshApp_hb_subEnd,
};

#ifdef CONFIG_BT_MESH_FRIEND
BT_MESH_FRIEND_CB_DEFINE(meshApp_friend_cb) = {
  .established = meshApp_friend_friendshipEstablished,
  .terminated  = meshApp_friend_friendshipTerminated,
};
#endif /* CONFIG_BT_MESH_FRIEND */

#ifdef CONFIG_BT_MESH_LOW_POWER
BT_MESH_LPN_CB_DEFINE(meshApp_lpn_cb) = {
  .established = meshApp_lpn_friendshipEstablished,
  .terminated  = meshApp_lpn_friendshipTerminated,
  .polled      = meshApp_lpnPolled,
};
#endif /* CONFIG_BT_MESH_LOW_POWER */
#endif /* USE_APP_MENU */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      bleApp_spin
 *
 * @brief   Spin forever
 *
 * @param   none
 */
static void bleApp_spin(void)
{
  volatile uint8_t x = 0;

  while(1)
  {
    x++;
  }
}

#ifdef MESH_ERPC
/*********************************************************************
 * @fn      mesh_erpc_register
 *
 * @brief   Register to ble stack from the eRPC context.
 *
 * @return  none
 */
void mesh_erpc_register(void)
{
    bleStack_errno_t err;

    // When eRPC is enabled, need to register to the ble stack from the eRPC context
    err = bleStack_register(&erpcThreadSelfEntity, 0);
    BLEAPP_ASSERT(BLE_STACK_ERRNO_SUCCESS == err);
}
#endif


#ifdef ADD_SIMPLE_PERIPHERAL
/*********************************************************************
 * @fn      SimplePeripheral_initProfiles
 *
 * @brief   Init simple peripheral profiles
 */
int SimplePeripheral_initProfiles(void)
{
  DevInfo_AddService();                        // Device Information Service

  SimpleProfile_AddService(GATT_ALL_SERVICES); // Simple GATT Profile
#if defined(OAD_SUPPORT_ONCHIP)
  // OAD profile for reset action
  OadApp_addService();
#endif
  // Setup the SimpleProfile Characteristic Values
  // For more information, see the GATT and GATTServApp sections in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  {
    uint8_t charValue1 = 1;
    uint8_t charValue2 = 2;
    uint8_t charValue3 = 3;
    uint8_t charValue4 = 4;
    uint8_t charValue5[SIMPLEPROFILE_CHAR5_LEN] = { 1, 2, 3, 4, 5 };

    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR1, sizeof(uint8_t),
                               &charValue1);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR2, sizeof(uint8_t),
                               &charValue2);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR3, sizeof(uint8_t),
                               &charValue3);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, sizeof(uint8_t),
                               &charValue4);
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR5, SIMPLEPROFILE_CHAR5_LEN,
                               charValue5);
  }

  // Register callback with SimpleGATTprofile
  SimpleProfile_RegisterAppCBs(&bleApp_simpleProfileCBs);

  return 0;
}
#endif // ADD_SIMPLE_PERIPHERAL

/*********************************************************************
 * @fn      bleApp_stackInit
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 */
int bleApp_stackInit(void)
{
  uint8_t profileRole;

  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO bleStack_register
  // ******************************************************************
  // Register the stack messages callback - see ble_stack_api.c - #ifdef ICALL_NO_APP_EVENTS
  bleStack_register(&appSelfEntity, bleApp_processStackMsgCB);
  meshAppSelfEntity = appSelfEntity; // used by bt.c
  // Create the application appSyncEvent
  appSyncEvent = Event_create(NULL, NULL);

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueueHandle = Util_constructQueue(&appMsgQueue);

  // Create one-shot clock for internal periodic events.
  Util_constructClock(&clkPeriodic, bleApp_clockHandler,
                      BLEAPP_PERIODIC_EVT_PERIOD, 0, false, (UArg)&argPeriodic);

#if defined(CONFIG_BT_MESH_PROXY) || defined(ADD_SIMPLE_PERIPHERAL)
  profileRole = GAP_PROFILE_PERIPHERAL | GAP_PROFILE_OBSERVER;
  BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- role PERIPHERAL+OBSERVER", profileRole);
#else
  profileRole = GAP_PROFILE_BROADCASTER | GAP_PROFILE_OBSERVER;
  BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- role BROADCASTER+OBSERVER", profileRole);
#endif // CONFIG_BT_MESH_PROXY || ADD_SIMPLE_PERIPHERAL

#ifdef USE_RCOSC
  // Set device's Sleep Clock Accuracy
#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
  HCI_EXT_SetSCACmd(500);
#endif // (CENTRAL_CFG | PERIPHERAL_CFG)
  RCOSC_enableCalibration();
#endif // USE_RCOSC

#if defined(CONFIG_BT_MESH_PROXY) || defined(ADD_SIMPLE_PERIPHERAL)
  // init GAP
  bleStack_initGap(profileRole, appSelfEntity, DEFAULT_PARAM_UPDATE_REQ_DECISION);

  // init GapBond
  bleStack_initGapBond(0, &bleApp_bondMgrCBs);

  // init GATT
  bleStack_initGatt(profileRole, appSelfEntity, attDeviceName);
#else
  // init GAP
  bleStack_initGap(profileRole, appSelfEntity, 0);

  // init GapBond
  bleStack_initGapBond(0, &bleApp_bondMgrCBs);
#endif

#if defined ( GAP_BOND_MGR )
  // Setup the GAP Bond Manager. For more information see the GAP Bond Manager
  // section in the User's Guide
  // ToDo - remove implementation from SysConfig
  // ToDo - set one call _all instaead of many calls to GAPBondMgr_SetParameter
  setBondManagerParameters();
#endif

#ifdef ADD_SIMPLE_PERIPHERAL
  SimplePeripheral_initProfiles();
#endif

#ifdef ADD_SIMPLE_PERIPHERAL
  // Initialize Connection List
  SimplePeripheral_clearConnListEntry(LINKDB_CONNHANDLE_ALL);

  // Initialize array to store connection handle and RSSI values
  SimplePeripheral_initPHYRSSIArray();
#endif

  BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- call GAP_DeviceInit", profileRole);
  //Initialize GAP layer for Peripheral and Central role and register to receive GAP events
  GAP_DeviceInit(profileRole, appSelfEntity, addrMode, &pRandomAddress);

#ifdef PTM_MODE
  PTM_init(appSelfEntity);
#endif

#ifdef OAD_SUPPORT
#ifdef OAD_DEBUG
  tbm_setItemStatus(&spMenuMain, SP_ITEM_SELECT_OAD_DBG, SP_ITEM_SELECT_CONN);
#endif
  OadApp_init();
#endif
  // Preempting Advertise over Scan
  BLEAPP_ASSERT(HCI_EXT_SetQOSDefaultParameters(LL_QOS_MEDIUM_PRIORITY,
      LL_QOS_TYPE_PRIORITY, LL_QOS_ADV_TASK_TYPE) == HCI_SUCCESS);

  return 0;
}

/*********************************************************************
 * @fn      bleApp_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 */
static void bleApp_init(void)
{
  BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- init ", BLEAPP_TASK_PRIORITY);

  /*
   * Init the menu with input keys and display
   */

  // Init key debouncer
  Board_initKeys(bleApp_keyChangeHandler);

#ifdef USE_APP_MENU
  // preprocessor definitions
  dispHandle = Display_open(Display_Type_ANY, NULL);

  // Create the menu
  SimpleMeshMenu_init(dispHandle);
#endif

  /*
   * Init the ble stack
   */
  bleApp_stackInit();

  if (IS_ENABLED(CONFIG_BT_SETTINGS))
  {
    // Initialize settings submodule
    int err = settings_subsys_init();
    BLEAPP_ASSERT(err == SUCCESS);
    Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "NVS: settings_subsys_init() ret status: %d", err);
  }

  /*
   * Init the mesh stack
   */
#if defined (USE_APP_MENU) || defined (MESH_ERPC)
    // TBMenu will call MeshApp_init
    // eRpc will call bt_mesh_init + bt_mesh_provision
#else
    // bt_ready now, zephyr mesh stack can be initialized
    MeshApp_init(mesh_own_addr, UNPROVISIONED_BEACON);
#endif
}

/*********************************************************************
 * @fn      simple_mesh_and_peripheral_createTask
 *
 * @brief   Task creation function for simple_mesh_and_peripheral.
 */
void simple_mesh_and_peripheral_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = appTaskStack;
  taskParams.stackSize = BLEAPP_TASK_STACK_SIZE;
  taskParams.priority = BLEAPP_TASK_PRIORITY;

  Task_construct(&appTask, bleApp_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      bleApp_createTasks
 *
 * @brief   Task creation function for this ble application.
 */
void bleApp_createTasks(void)
{
  // Init and create ble stack tasks
  bleStack_createTasks();

  // init zephyr mesh worker task
  k_sys_work_q_init(NULL);

#ifdef MESH_ERPC
  // create eRpc task
  eRpcMesh_createTask();
#endif

  // Create local app task
  simple_mesh_and_peripheral_createTask();
}

/*********************************************************************
 * @fn      bleApp_taskFxn
 *
 * @brief   Application task entry point for the Simple Mesh.
 *
 * @param   a0, a1 - not used.
 */
static void bleApp_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  bleApp_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    appEvt_t *pMsg;

    // Waits for an event to be posted
    events = Event_pend(appSyncEvent, Event_Id_NONE, APP_QUEUE_EVT,
                          EVENT_PEND_FOREVER);
    if (events)
    {
      // Check if there are any messages in the queue
      while (!Queue_empty(appMsgQueueHandle))
      {
          pMsg = (appEvt_t *)Util_dequeueMsg(appMsgQueueHandle);

          if (pMsg)
          {
            if (BLEAPP_EVT_STACK_CALLBACK == pMsg->event)
            {
              // Process stack messages - changed from events to callback
              if (bleApp_processStackMsg((bleStack_msgHdt_t*)pMsg->pData))
              {
                bleStack_freeMsg((bleStack_msgHdt_t*)pMsg->pData);
              }
            }
#ifdef OAD_SUPPORT
            else if (OADAPP_EVT_CALLBACK == pMsg->event)
            {
                OadApp_processEvents((uint32_t)(*(uint8_t *)(pMsg->pData)));
                bleStack_free(pMsg->pData);
            }
#endif
            else
            {
              // Process application messages (callbacks after switch context)
              bleApp_processAppMsg(pMsg);
            }

            // Free the space from the message.
            bleStack_free(pMsg);
        }
      }
    }
  }
}

/*********************************************************************
 * @fn      bleApp_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t bleApp_processStackMsg(bleStack_msgHdt_t *pMsg)
{
  // Always dealloc pMsg unless set otherwise
  uint8_t safeToDealloc = TRUE;

  BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : Stack msg status=%d, event=0x%x\n", pMsg->status, pMsg->event);

  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:
      bleApp_processGapMessage((gapEventHdr_t*) pMsg);
      break;

    case GATT_MSG_EVENT:
      // Process GATT message
      safeToDealloc = bleApp_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    case HCI_GAP_EVENT_EVENT:
    {
      // Process HCI message
      switch (pMsg->status)
      {
        case HCI_COMMAND_COMPLETE_EVENT_CODE:
        // Process HCI Command Complete Events here
        {
#ifdef ADD_SIMPLE_PERIPHERAL
          bleApp_processCmdCompleteEvt((hciEvt_CmdComplete_t *) pMsg);
#endif
          break;
        }

        case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
          AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
          break;

#ifdef ADD_SIMPLE_PERIPHERAL
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
                Display_printf(dispHandle, SP_ROW_STATUS_1, 0,
                        "PHY Change failure, peer does not support this");
              }
              else
              {
                Display_printf(dispHandle, SP_ROW_STATUS_1, 0,
                               "PHY Update Status Event: 0x%x",
                               pMyMsg->cmdStatus);
              }

              SimplePeripheral_updatePHYStat(HCI_LE_SET_PHY, (uint8_t *)pMsg);
              break;
            }

            default:
              break;
          }
          break;
        }

        // LE Events
        case HCI_LE_EVENT_CODE:
        {
          hciEvt_BLEPhyUpdateComplete_t *pPUC =
            (hciEvt_BLEPhyUpdateComplete_t*) pMsg;

          // A Phy Update Has Completed or Failed
          if (pPUC->BLEEventCode == HCI_BLE_PHY_UPDATE_COMPLETE_EVENT)
          {
            if (pPUC->status != SUCCESS)
            {
              Display_printf(dispHandle, SP_ROW_STATUS_1, 0,
                             "PHY Change failure");
            }
            else
            {
              // Only symmetrical PHY is supported.
              // rxPhy should be equal to txPhy.
              Display_printf(dispHandle, SP_ROW_STATUS_2, 0,
                             "PHY Updated to %s",
                             (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_1M) ? "1M" :
                             (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_2M) ? "2M" :
                             (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_CODED) ? "CODED" : "Unexpected PHY Value");
            }

            SimplePeripheral_updatePHYStat(HCI_BLE_PHY_UPDATE_COMPLETE_EVENT, (uint8_t *)pMsg);
          }
          break;
        }

#endif // ADD_SIMPLE_PERIPHERAL
        default:
          break;
      }

      break;
    }

#ifdef OAD_SUPPORT
    case L2CAP_SIGNAL_EVENT:
      // Process L2CAP signal
      safeToDealloc = OadApp_processL2CAPMsg((l2capSignalEvent_t *)pMsg);
      break;
#endif

    default:
      // Do nothing
      break;
  }

#ifdef PTM_MODE
  // Check for NPI Messages
  PTM_handle_hci_event(pMsg);
#endif

  return (safeToDealloc);
}

/*********************************************************************
* @fn      bleApp_processGATTMsg
*
* @brief   Process GATT messages and events.
*
* @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
*/
static uint8_t bleApp_processGATTMsg(gattMsgEvent_t *pMsg)
{
#ifdef ADD_SIMPLE_PERIPHERAL
  if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
  {
    // ATT request-response or indication-confirmation flow control is
    // violated. All subsequent ATT requests or indications will be dropped.
    // The app is informed in case it wants to drop the connection.

    // Display the opcode of the message that caused the violation.
    Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "FC Violated: %d", pMsg->msg.flowCtrlEvt.opcode);
  }
  else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
  {
    // MTU size updated
#ifdef OAD_SUPPORT
    OAD_setBlockSize(pMsg->msg.mtuEvt.MTU);
#endif
    Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "MTU Size: %d", pMsg->msg.mtuEvt.MTU);
  }
#endif // ADD_SIMPLE_PERIPHERAL

#ifdef CONFIG_BT_MESH_PROXY
  if (pMsg->method == ATT_MTU_UPDATED_EVENT)
  {
    mesh_portingLayer_updateLinkMTU(pMsg->msg.mtuEvt.MTU);
  }
#endif
  // Free message payload. Needed only for ATT Protocol messages
  GATT_bm_free(&pMsg->msg, pMsg->method);

  // It's safe to free the incoming message
  return (TRUE);
}

/*********************************************************************
 * @fn      bleApp_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  None.
 */
static void bleApp_processAppMsg(appEvt_t *pMsg)
{
  bool dealloc = TRUE;

  if (pMsg->event <= BLEAPP_EVT_EVENT_MAX)
  {
    BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : App msg status=%d, event=%s\n", 0, appEventStrings[pMsg->event]);
  }
  else
  {
    BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : App msg status=%d, event=0x%x\n", 0, pMsg->event);
  }

  switch (pMsg->event)
  {
#ifdef ADD_SIMPLE_PERIPHERAL
    case BLEAPP_EVT_CHAR_CHANGE:
      bleApp_processCharValueChangeEvt(*(uint8_t*)(pMsg->pData));
      break;
#endif

#ifdef USE_APP_MENU
    case BLEAPP_EVT_KEY_CHANGE:
      bleApp_handleKeys(*(uint8_t *)(pMsg->pData));
      break;
#endif

#ifdef ADD_SIMPLE_PERIPHERAL
    case BLEAPP_EVT_ADV:
      advCb_count++;
      bleApp_processAdvEvent((appGapAdvEventData_t*)(pMsg->pData));
      break;
#endif

    case MESHAPP_EVT_ADV:
      advCb_count++;
      meshApp_processAdvEvent((appGapAdvEventData_t*)(pMsg->pData));
      break;

    case BLEAPP_EVT_PERIODIC:
      bleApp_performPeriodicTask();
      break;

#ifdef ADD_SIMPLE_PERIPHERAL
    case BLEAPP_EVT_PAIRING_STATE:
      bleApp_processPairState((appPairStateData_t*)(pMsg->pData));
      break;

    case BLEAPP_EVT_PASSCODE_NEEDED:
      bleApp_processPasscode((appPasscodeData_t*)(pMsg->pData));
      break;

    case BLEAPP_EVT_READ_RPA:
      SimplePeripheral_updateRPA();
      break;

    case BLEAPP_EVT_SEND_PARAM_UPDATE:
    {
      // Extract connection handle from data
      uint16_t connHandle = *(uint16_t *)(((appClockEventData_t *)pMsg->pData)->data);

      bleApp_processParamUpdate(connHandle);

      // This data is not dynamically allocated
      dealloc = FALSE;
      break;
    }

    case BLEAPP_EVT_CONN:
      bleApp_processConnEvt((Gap_ConnEventRpt_t *)(pMsg->pData));
      break;

#if defined(OAD_SUPPORT_ONCHIP)
    case OADAPP_EVT_RESET:
      OadApp_processOadResetEvt((oadResetWrite_t *)(pMsg->pData));
      break;
#endif
#endif // ADD_SIMPLE_PERIPHERAL

    case BLEAPP_EVT_ADV_REPORT:
    {
      scanCbData_t *pData = (scanCbData_t *)pMsg->pData;
      GapScan_Evt_AdvRpt_t* pAdvRpt = (GapScan_Evt_AdvRpt_t*) (pData->pMsg);

      mesh_portingLayer_scanCB(pData->evt, pData->pMsg, pData->arg);
#define BT_DATA_MESH_PROV               0x29 /* Mesh Provisioning PDU */
#define BT_DATA_MESH_MESSAGE            0x2a /* Mesh Networking PDU */
#define BT_DATA_MESH_BEACON             0x2b /* Mesh Beacon */

      scanCb_count++;
      if (pAdvRpt->pData[1] == BT_DATA_MESH_BEACON)
          scanCb_beaconCount++;
      else if (pAdvRpt->pData[1] == BT_DATA_MESH_MESSAGE)
          scanCb_msgCount++;
      else if (pAdvRpt->pData[1] == BT_DATA_MESH_PROV)
          scanCb_provCount++;

      // Free scan payload data
      if (pAdvRpt->pData != NULL)
      {
        bleStack_free(pAdvRpt->pData);
      }
      if (pAdvRpt != NULL)
      {
        bleStack_free(pAdvRpt);
      }
      break;
    }

    case BLEAPP_EVT_SCAN_ENABLED:
    case BLEAPP_EVT_SCAN_DISABLED:
    {
      scanCbData_t *pData = (scanCbData_t *)pMsg->pData;

      mesh_portingLayer_scanCB(pData->evt, pData->pMsg, pData->arg);

      break;
    }

    case BLEAPP_EVT_INSUFFICIENT_MEM:
    {
      // We are running out of memory.
#ifdef USE_APP_MENU
//      Display_printf(dispHandle, SMN_ROW_ANY_CONN, 0, "Insufficient Memory");
#endif
      BLEAPP_ASSERT(FAILURE);
      break;
    }

#ifdef CONFIG_BT_MESH_PROXY
    case MESHAPP_EVT_GATT_WRT:
    {
      writeCbParams_t *writeParams = (writeCbParams_t *)(pMsg->pData);
      // call real writeCB
      mesh_portingLayer_proxyProvAttWriteCB(writeParams);
      break;
    }
#endif

    default:
      // Do nothing.
      break;
  }

  // Free message data if it exists and we are to dealloc
  if ((dealloc == TRUE) && (pMsg->pData != NULL))
  {
    bleStack_free(pMsg->pData);
  }
}

#ifdef ADD_SIMPLE_PERIPHERAL
/*********************************************************************
 * @fn      SimplePeripheral_initAdvSet
 *
 * @brief   Initialize and starts advertise set (legacy or extended)
 *
 * @param advCallback Function pointer to a callback for this advertising set
 * @param advHandle Output parameter to return the created advertising handle
 * @param advParams pointer to structure of adversing parameters
 * @param advData pointer to array containing the advertise data
 * @param advDataLen length (in bytes) of @ref advData
 * @param scanRespData pointer to array containing the scan response data
 * @param scanRespDataLen length (in bytes) of @ref scanRespDataLen
 *
 * @return @ref SUCCESS upon successful initialization,
 *         else, relevant error code upon failure
 */
static bStatus_t SimplePeripheral_initAdvSet(pfnGapCB_t advCallback,
                                             uint8_t * const advHandle, GapAdv_params_t * const advParams,
                                             uint8_t advData[], const int advDataLen,
                                             uint8_t scanRespData[], const int scanRespDataLen)
{
  bStatus_t status;

  BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : ---- call GapAdv_create set=%d,%d\n", 0, 0);
  // Create Advertisement set and assign handle
  status = GapAdv_create(advCallback, advParams,
                         advHandle);
  if (status != SUCCESS)
  {
    return status;
  }


  // Load advertising data for set that is statically allocated by the app
  status = GapAdv_loadByHandle(*advHandle, GAP_ADV_DATA_TYPE_ADV,
                               advDataLen, advData);
  if (status != SUCCESS)
  {
    return status;
  }

  // Load scan response data for set that is statically allocated by the app
  if (scanRespData != NULL)
  {
    status = GapAdv_loadByHandle(*advHandle, GAP_ADV_DATA_TYPE_SCAN_RSP,
                                 scanRespDataLen, scanRespData);
    if (status != SUCCESS)
    {
      return status;
    }
  }

  // Set event mask for set
  status = GapAdv_setEventMask(*advHandle,
                               GAP_ADV_EVT_MASK_START_AFTER_ENABLE |
                               GAP_ADV_EVT_MASK_END_AFTER_DISABLE |
                               GAP_ADV_EVT_MASK_SET_TERMINATED);

  if (status != SUCCESS)
  {
    return status;
  }
  // Enable advertising for set
  status = GapAdv_enable(*advHandle, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);

  return status;
}

/*********************************************************************
 * @fn      SimplePeripheral_startAdvertising
 *
 * @brief   start advertising - legacy and long range
 */
int SimplePeripheral_startAdvertising(uint8_t *devAddr)
{
    bStatus_t status;
    Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "Initialized");

    // Setup and start Advertising
    // For more information, see the GAP section in the User's Guide:
    // http://software-dl.ti.com/lprf/ble5stack-latest/

    // Initialize advertisement handlers

    // In case of extended, initialize advertise set with advertising data,
    // else (Legacy), initialize advertise set with scan response data and advertising data
    // Note: In this example, the default adv set is extended. When changing the set to Legacy,
    // please un-comment the "else" section.
    if ( (advParams1.eventProps & GAP_ADV_PROP_LEGACY) == 0)
    {
      status = SimplePeripheral_initAdvSet(&SimplePeripheral_advCallback, &advHandle1, &advParams1,
                                           advData1, sizeof(advData1) ,NULL, 0);
      BLEAPP_ASSERT(status == SUCCESS);
    }
    // else
    // {
    //   status = SimplePeripheral_initAdvSet(&SimplePeripheral_advCallback, &advHandle1, &advParams1,
    //                                        advData1, sizeof(advData1) ,
    //                                        scanResData1, sizeof(scanResData1));
    //   BLEAPP_ASSERT(status == SUCCESS);
    // }

#ifdef SECOND_ADV_SET
    // In case of extended, initialize advertise set with advertising data,
    // else (Legacy), initialize advertise set with scan response data and advertising data
    // Note: In this example, the default adv set is extended. When changing the set to Legacy,
    // please un-comment the "else" section.
    if (advParams2.eventProps & GAP_ADV_PROP_LEGACY == 0)
    {
      status = SimplePeripheral_initAdvSet(&SimplePeripheral_advCallback, &advHandle2, &advParams2,
                                           advData2, sizeof(advData2) ,NULL, 0);
      BLEAPP_ASSERT(status == SUCCESS);
    }
    // else
    // {
    //   status = SimplePeripheral_initAdvSet(&SimplePeripheral_advCallback, &advHandle2, &advParams2,
    //                                        advData2, sizeof(advData2) ,
    //                                        scanResData2, sizeof(scanResData2));
    //   BLEAPP_ASSERT(status == SUCCESS);
    // }
#endif /* SECOND_ADV_SET */

    // Display device address
    Display_printf(dispHandle, SP_ROW_IDA, 0, "%s Addr: %s",
                   (addrMode <= ADDRMODE_RANDOM) ? "Dev" : "ID",
                   Util_convertBdAddr2Str(devAddr));

    if (addrMode > ADDRMODE_RANDOM)
    {
      SimplePeripheral_updateRPA();

      // Create one-shot clock for RPA check event.
      Util_constructClock(&clkRpaRead, bleApp_clockHandler,
                          READ_RPA_PERIOD, 0, true,
                          (UArg) &argRpaRead);
    }
    tbm_setItemStatus(&spMenuMain, SP_ITEM_AUTOCONNECT, TBM_ITEM_NONE);

    return 0;
}

/*********************************************************************
 * @fn      SimplePeripheral_connectIndication
 *
 * @brief   conection establish indication
 */
int SimplePeripheral_connectIndication(uint16_t connectionHandle, uint8 pkt_status)
{
      // Display the amount of current connections
      uint8_t numActive = linkDB_NumActive();
#ifdef USE_APP_MENU
      Display_printf(dispHandle, SP_ROW_STATUS_2, 0, "Num Conns: %d",
                     (uint16_t)numActive);
#endif

      if (pkt_status == SUCCESS)
      {
        // Add connection to list and start RSSI
        SimplePeripheral_addConn(connectionHandle);
        // Enable connection selection option
        tbm_setItemStatus(&spMenuMain, SP_ITEM_SELECT_CONN,SP_ITEM_AUTOCONNECT);

        // Start Periodic Clock.
        Util_startClock(&clkPeriodic);
      }

      if ((numActive < MAX_NUM_BLE_CONNS) && (autoConnect == AUTOCONNECT_DISABLE))
      {
        // Start advertising since there is room for more connections
        GapAdv_enable(advHandle1, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
#ifdef SECOND_ADV_SET
        GapAdv_enable(advHandle2, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
#endif /* SECOND_ADV_SET */
      }
      else
      {
        GapAdv_disable(advHandle1);
#ifdef SECOND_ADV_SET
        GapAdv_disable(advHandle2);
#endif /* SECOND_ADV_SET */
      }

      return 0;
}
/*********************************************************************
 * @fn      SimplePeripheral_disableAdvertising
 *
 * @brief   disable advertising - legacy and long range
 */
int SimplePeripheral_disconnectIndication(uint16_t connectionHandle)
{
      // Display the amount of current connections
      uint8_t numActive = linkDB_NumActive();
#ifdef USE_APP_MENU
      Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "Device Disconnected!");
      Display_printf(dispHandle, SP_ROW_STATUS_2, 0, "Num Conns: %d",
                     (uint16_t)numActive);
#endif

      // Remove the connection from the list and disable RSSI if needed
      SimplePeripheral_removeConn(connectionHandle);

      // If no active connections
      if (numActive == 0)
      {
        // Stop periodic clock
        Util_stopClock(&clkPeriodic);

        // Disable Connection Selection option
        tbm_setItemStatus(&spMenuMain, SP_ITEM_AUTOCONNECT, SP_ITEM_SELECT_CONN);
      }

      BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : GAP msg: status=%d, opcode=%s\n", 0, "GAP_LINK_TERMINATED_EVENT");
      // Start advertising since there is room for more connections
      GapAdv_enable(advHandle1, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
#ifdef SECOND_ADV_SET
      GapAdv_enable(advHandle2, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
#endif /* SECOND_ADV_SET */

      // Clear remaining lines
      Display_clearLine(dispHandle, SP_ROW_CONNECTION);

#ifdef OAD_SUPPORT
      // Cancel the OAD if one is going on
      // A disconnect forces the peer to re-identify
      OadApp_cancel();
#endif
      return 0;
}
#endif // ADD_SIMPLE_PERIPHERAL
/*********************************************************************
 * @fn      bleApp_processGapMessage
 *
 * @brief   Process an incoming GAP event.
 *
 * @param   pMsg - message to process
 */
static void bleApp_processGapMessage(gapEventHdr_t *pMsg)
{
  switch (pMsg->opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
    {
      gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- got GAP_DEVICE_INIT_DONE_EVENT", 0);
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

        // Register callback to process Scanner events
        GapScan_registerCb(MeshApp_scanCB, NULL);

#ifdef CONFIG_BT_MESH_PROXY
        // Set the device's name in the mesh porting layer
        mesh_portingLayer_setDeviceName(meshScanData);
#endif

#ifdef ADD_SIMPLE_PERIPHERAL
        SimplePeripheral_startAdvertising(pPkt->devAddr);
#endif
      }

      break;
    }

    case GAP_LINK_ESTABLISHED_EVENT:
    {
#if defined(CONFIG_BT_MESH_PROXY) || defined(ADD_SIMPLE_PERIPHERAL)
      gapEstLinkReqEvent_t *pPkt = (gapEstLinkReqEvent_t *)pMsg;

      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- got GAP_LINK_ESTABLISHED_EVENT", 0);
#endif
#ifdef CONFIG_BT_MESH_PROXY
        // Call zephyr cb
        mesh_portingLayer_newConnectionCB(pPkt);
#endif

#ifdef ADD_SIMPLE_PERIPHERAL
        SimplePeripheral_connectIndication(pPkt->connectionHandle, pPkt->hdr.status);
#endif
      break;
    }

    case GAP_LINK_TERMINATED_EVENT:
    {
#if defined(CONFIG_BT_MESH_PROXY) || defined(ADD_SIMPLE_PERIPHERAL)
      gapTerminateLinkEvent_t *pPkt = (gapTerminateLinkEvent_t *)pMsg;
#endif

#ifdef CONFIG_BT_MESH_PROXY
      // Call zephyr cb
      mesh_portingLayer_disconnectCB(pPkt);
#endif

#ifdef ADD_SIMPLE_PERIPHERAL
        SimplePeripheral_disconnectIndication(pPkt->connectionHandle);
#endif
      break;
    }

#if defined(CONFIG_BT_MESH_PROXY) || defined(ADD_SIMPLE_PERIPHERAL)
    case GAP_UPDATE_LINK_PARAM_REQ_EVENT:
    {
      gapUpdateLinkParamReqReply_t rsp;

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

      // Get the address from the connection handle
      linkDBInfo_t linkInfo;
      linkDB_GetInfo(pPkt->connectionHandle, &linkInfo);

#ifdef CONFIG_BT_MESH_PROXY
      mesh_portingLayer_updateLinkMTU(linkInfo.MTU);
#endif

#ifdef ADD_SIMPLE_PERIPHERAL
#ifdef USE_APP_MENU
      if(pPkt->status == SUCCESS)
      {
        // Display the address of the connection update
        Display_printf(dispHandle, SP_ROW_STATUS_2, 0, "Link Param Updated: %s",
                       Util_convertBdAddr2Str(linkInfo.addr));
      }
      else
      {
        // Display the address of the connection update failure
        Display_printf(dispHandle, SP_ROW_STATUS_2, 0,
                       "Link Param Update Failed 0x%x: %s", pPkt->opcode,
                       Util_convertBdAddr2Str(linkInfo.addr));
      }
#endif
      // Check if there are any queued parameter updates
      spConnHandleEntry_t *connHandleEntry = (spConnHandleEntry_t *)List_get(&paramUpdateList);
      if (connHandleEntry != NULL)
      {
        // Attempt to send queued update now
        bleApp_processParamUpdate(connHandleEntry->connHandle);

        // Free list element
        bleStack_free(connHandleEntry);
      }
#endif // ADD_SIMPLE_PERIPHERAL
      break;
    }

#if defined ( NOTIFY_PARAM_UPDATE_RJCT )
     case GAP_LINK_PARAM_UPDATE_REJECT_EVENT:
     {
       linkDBInfo_t linkInfo;
       gapLinkUpdateEvent_t *pPkt = (gapLinkUpdateEvent_t *)pMsg;

       // Get the address from the connection handle
       linkDB_GetInfo(pPkt->connectionHandle, &linkInfo);

       // Display the address of the connection update failure
       Display_printf(dispHandle, SP_ROW_CONNECTION, 0,
                      "Peer Device's Update Request Rejected 0x%h: %s", pPkt->opcode,
                      Util_convertBdAddr2Str(linkInfo.addr));

       break;
     }
#endif
#endif /* defined(CONFIG_BT_MESH_PROXY) || defined(ADD_SIMPLE_PERIPHERAL) */

    default:
#ifdef ADD_SIMPLE_PERIPHERAL
      Display_clearLines(dispHandle, SP_ROW_STATUS_1, SP_ROW_STATUS_2);
#endif // ADD_SIMPLE_PERIPHERAL
      break;
  }
}

/*********************************************************************
* @fn      bleApp_processStackMsgCB
*
* @brief   Callback for application from ble stack
*
* @param   pMessage - message from ble stack
*
* @return  None.
*/
uint8_t bleApp_processStackMsgCB(uint8_t event, uint8_t *pMessage)
{
  // ignore the event
  // Enqueue the msg in order to be excuted in the application context
  bleApp_enqueueMsg(BLEAPP_EVT_STACK_CALLBACK, pMessage);

  // not safe to dealloc, the application layer will free the msg
  return (false);
}

#ifdef OAD_SUPPORT
/*********************************************************************
* @fn      bleApp_processOadEventsInAppContext
*
* @brief   Callback from OAD to handle OAD events
*
* @param   event - event from OAD
*
* @return  None.
*/
uint8_t bleApp_processOadEventsInAppContext(uint8_t *pEventData)
{
  // Enqueue the msg in order to be excuted in the application context
  bleApp_enqueueMsg(OADAPP_EVT_CALLBACK, pEventData);

  // not safe to dealloc, the application layer will free the msg
  return (false);
}
#if defined(OAD_SUPPORT_ONCHIP)
/*********************************************************************
* @fn      bleApp_processOadResetInAppContext
*
* @brief   Callback from OAD to handle OAD reset
*
* @param   event - event from OAD
*
* @return  None.
*/
uint8_t bleApp_processOadResetInAppContext(uint8_t *pEventData)
{
  // Enqueue the msg in order to be excuted in the application context
  bleApp_enqueueMsg(OADAPP_EVT_RESET, pEventData);

  // not safe to dealloc, the application layer will free the msg
  return (false);
}
#endif // OAD_SUPPORT_ONCHIP
#endif // OAD_SUPPORT


#ifdef ADD_SIMPLE_PERIPHERAL
/*********************************************************************
* @fn      bleApp_charValueChangeCB
*
* @brief   Callback from Simple Profile indicating a characteristic
*          value change.
*
 * @param   paramId - parameter Id of the value that was changed.
*
* @return  None.
*/
static void bleApp_charValueChangeCB(uint8_t paramId)
{
  uint8_t *pValue = bleStack_malloc(sizeof(uint8_t));

  if (pValue)
  {
    *pValue = paramId;

    if(bleApp_enqueueMsg(BLEAPP_EVT_CHAR_CHANGE, pValue) != SUCCESS)
    {
      bleStack_free(pValue);
    }
  }
}
#endif // ADD_SIMPLE_PERIPHERAL

#ifdef ADD_SIMPLE_PERIPHERAL
/*********************************************************************
 * @fn      bleApp_processCharValueChangeEvt
 *
 * @brief   Process a pending Simple Profile characteristic value change
 *          event.
 *
 * @param   paramID - parameter ID of the value that was changed.
 */
static void bleApp_processCharValueChangeEvt(uint8_t paramId)
{
  uint8_t newValue;

  switch(paramId)
  {
    case SIMPLEPROFILE_CHAR1:
      SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR1, &newValue);

      Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "Char 1: %d", (uint16_t)newValue);
      break;

    case SIMPLEPROFILE_CHAR3:
      SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR3, &newValue);

      Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "Char 3: %d", (uint16_t)newValue);
      break;

    default:
      // should not reach here!
      break;
  }
}
#endif // ADD_SIMPLE_PERIPHERAL

/*********************************************************************
 * @fn      bleApp_performPeriodicTask
 *
 * @brief   Perform a periodic application task. This function gets called
 *          every two seconds (BLEAPP_PERIODIC_EVT_PERIOD).
 *
 * @param   None.
 *
 * @return  None.
 */
static void bleApp_performPeriodicTask(void)
{
#if defined(USE_APP_MENU) && defined(DEBUG_MESH_APP)
  Display_printf(dispHandle, SMN_ROW_CBCK_ST, 0, "CBCK: advCb count=%04d, scanCb=%04d, beacon=%04x, msg=%04x, prov=%04x",
                                  advCb_count, scanCb_count, scanCb_beaconCount, scanCb_msgCount, scanCb_provCount);
#endif
#ifdef ADD_SIMPLE_PERIPHERAL
  uint8_t valueToCopy;

  // Call to retrieve the value of the third characteristic in the profile
  if (SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR3, &valueToCopy) == SUCCESS)
  {
    // Call to set that value of the fourth characteristic in the profile.
    // Note that if notifications of the fourth characteristic have been
    // enabled by a GATT client device, then a notification will be sent
    // every time this function is called.
    SimpleProfile_SetParameter(SIMPLEPROFILE_CHAR4, sizeof(uint8_t),
                               &valueToCopy);
  }
#endif // ADD_SIMPLE_PERIPHERAL
}

#ifdef ADD_SIMPLE_PERIPHERAL
/*********************************************************************
 * @fn      SimplePeripheral_updateRPA
 *
 * @brief   Read the current RPA from the stack and update display
 *          if the RPA has changed.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SimplePeripheral_updateRPA(void)
{
  uint8_t* pRpaNew;

  // Read the current RPA.
  pRpaNew = GAP_GetDevAddress(FALSE);

  if (memcmp(pRpaNew, rpa, B_ADDR_LEN))
  {
    // If the RPA has changed, update the display
    Display_printf(dispHandle, SP_ROW_RPA, 0, "RP Addr: %s",
                   Util_convertBdAddr2Str(pRpaNew));
    memcpy(rpa, pRpaNew, B_ADDR_LEN);
  }
}
#endif // ADD_SIMPLE_PERIPHERAL

/*********************************************************************
 * @fn      SimpleMeshNode_advCB
 *
 * @brief   GapAdv module callback, called from MESH stack
 *
 * @param   pMsg - message to process
 */
void SimpleMeshNode_advCB(uint32_t event, void *pBuf, uintptr_t arg)
{
  appGapAdvEventData_t *pData = bleStack_malloc(sizeof(appGapAdvEventData_t));

  if (pData)
  {
    pData->event = event;
    pData->pBuf = pBuf;

    if(bleApp_enqueueMsg(MESHAPP_EVT_ADV, pData) != SUCCESS)
    {
      bleStack_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      bleApp_clockHandler
 *
 * @brief   Handler function for clock timeouts.
 *
 * @param   arg - event type
 *
 * @return  None.
 */
static void bleApp_clockHandler(UArg arg)
{
  appClockEventData_t *pData = (appClockEventData_t *)arg;

  if (pData->event == BLEAPP_EVT_PERIODIC)
  {
    // Start the next period
    Util_startClock(&clkPeriodic);

    // Send message to perform periodic task
    bleApp_enqueueMsg(BLEAPP_EVT_PERIODIC, NULL);
  }
  else if (pData->event == BLEAPP_EVT_READ_RPA)
  {
    // Start the next period
    Util_startClock(&clkRpaRead);

    // Send message to read the current RPA
    bleApp_enqueueMsg(BLEAPP_EVT_READ_RPA, NULL);
  }
  else if (pData->event == BLEAPP_EVT_SEND_PARAM_UPDATE)
  {
    // Send message to app
      bleApp_enqueueMsg(BLEAPP_EVT_SEND_PARAM_UPDATE, pData);
  }
}

/*********************************************************************
 * @fn      bleApp_keyChangeHandler
 *
 * @brief   Key event handler function
 *
 * @param   keys - bitmap of pressed keys
 *
 * @return  none
 */
static void bleApp_keyChangeHandler(uint8_t keys)
{
#ifdef USE_APP_MENU
  uint8_t *pValue = bleStack_malloc(sizeof(uint8_t));

  if (pValue)
  {
    *pValue = keys;

    if(bleApp_enqueueMsg(BLEAPP_EVT_KEY_CHANGE, pValue) != SUCCESS)
    {
      bleStack_free(pValue);
    }
  }
#endif /* USE_APP_MENU */
}

#ifdef USE_APP_MENU
/*********************************************************************
 * @fn      bleApp_handleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   keys - bit field for key events. Valid entries:
 *                 KEY_LEFT
 *                 KEY_RIGHT
 */
static void bleApp_handleKeys(uint8_t keys)
{
  if (keys & KEY_LEFT)
  {
    // Check if the key is still pressed. Workaround for possible bouncing.
    if (PIN_getInputValue(CONFIG_GPIO_BTN1) == 0)
    {
      tbm_buttonLeft();
    }
  }
  else if (keys & KEY_RIGHT)
  {
    // Check if the key is still pressed. Workaround for possible bouncing.
    if (PIN_getInputValue(CONFIG_GPIO_BTN2) == 0)
    {
      tbm_buttonRight();
    }
  }
}
#endif // USE_APP_MENU

#ifdef ADD_SIMPLE_PERIPHERAL
/*********************************************************************
 * @fn      SimplePeripheral_advCallback
 *
 * @brief   GapAdv module callback
 *
 * @param   pMsg - message to process
 */
static void SimplePeripheral_advCallback(uint32_t event, void *pBuf, uintptr_t arg)
{
  appGapAdvEventData_t *pData = bleStack_malloc(sizeof(appGapAdvEventData_t));

  if (pData)
  {
    pData->event = event;
    pData->pBuf = pBuf;

    if(bleApp_enqueueMsg(BLEAPP_EVT_ADV, pData) != SUCCESS)
    {
      bleStack_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      bleApp_processAdvEvent
 *
 * @brief   Process advertising event in app context
 *
 * @param   pEventData
 */
static void bleApp_processAdvEvent(appGapAdvEventData_t *pEventData)
{
  switch (pEventData->event)
  {
    case GAP_EVT_ADV_START_AFTER_ENABLE:
      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- GAP_EVT_ADV_START_AFTER_ENABLE", 0);
#ifdef USE_APP_MENU
      Display_printf(dispHandle, SP_ROW_ADVSTATE, 0, "Adv Set %d Enabled",
                     *(uint8_t *)(pEventData->pBuf));
#endif
      break;

    case GAP_EVT_ADV_END_AFTER_DISABLE:
#ifdef USE_APP_MENU
      Display_printf(dispHandle, SP_ROW_ADVSTATE, 0, "Adv Set %d Disabled",
                     *(uint8_t *)(pEventData->pBuf));
#endif
      break;

    case GAP_EVT_ADV_START:
      break;

    case GAP_EVT_ADV_END:
      break;

    case GAP_EVT_ADV_SET_TERMINATED:
    {
#ifndef Display_DISABLE_ALL
      GapAdv_setTerm_t *advSetTerm = (GapAdv_setTerm_t *)(pEventData->pBuf);

      Display_printf(dispHandle, SP_ROW_ADVSTATE, 0, "Adv Set %d disabled after conn %d",
                     advSetTerm->handle, advSetTerm->connHandle );
#endif
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
    bleStack_free(pEventData->pBuf);
  }
}
#endif // ADD_SIMPLE_PERIPHERAL

/*********************************************************************
 * @fn      meshApp_processAdvEvent
 *
 * @brief   Process advertising event in app context
 *
 * @param   pEventData
 */
static void meshApp_processAdvEvent(appGapAdvEventData_t *pEventData)
{
#if defined(USE_APP_MENU) && defined(DEBUG_MESH_APP)
  Display_printf(dispHandle, SMN_ROW_EVNT_ST, 0, "EVNT: AdvEvent=%d", pEventData->event);
#endif

  // Call the porting layer ADV CB
  mesh_portingLayer_advCB(pEventData->event, pEventData->pBuf);

  switch (pEventData->event)
  {
    case GAP_EVT_ADV_START_AFTER_ENABLE:
      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- GAP_EVT_ADV_START_AFTER_ENABLE", 0);
      break;

    default:
      break;
  }

  // All events have associated memory to free except the insufficient memory
  // event
  if (pEventData->event != GAP_EVT_INSUFFICIENT_MEMORY)
  {
    bleStack_free(pEventData->pBuf);
  }
}

/*********************************************************************
* @fn      bleApp_pairStateCb
*
 * @brief   Pairing state callback.
*
* @return  none
*/
static void bleApp_pairStateCb(uint16_t connHandle, uint8_t state,
                                   uint8_t status)
{
  appPairStateData_t *pData = bleStack_malloc(sizeof(appPairStateData_t));

  // Allocate space for the event data.
  if (pData)
  {
    pData->state = state;
    pData->connHandle = connHandle;
    pData->status = status;

    // Queue the event.
    if (bleApp_enqueueMsg(BLEAPP_EVT_PAIRING_STATE, pData) != SUCCESS)
    {
      bleStack_free(pData);
    }
  }
}

/*********************************************************************
* @fn      bleApp_passcodeCB
*
* @brief   Passcode callback.
*
* @param   deviceAddr - pointer to device address
*
* @param   connHandle - the connection handle
*
* @param   uiInputs - pairing User Interface Inputs
*
* @param   uiOutputs - pairing User Interface Outputs
*
* @param   numComparison - numeric Comparison 20 bits
*
* @return  none
*/
static void bleApp_passcodeCB(uint8_t *pDeviceAddr,
                                        uint16_t connHandle,
                                        uint8_t uiInputs,
                                        uint8_t uiOutputs,
                                        uint32_t numComparison)
{
  appPasscodeData_t *pData = bleStack_malloc(sizeof(appPasscodeData_t));

  // Allocate space for the passcode event.
  if (pData)
  {
    pData->connHandle = connHandle;
    memcpy(pData->deviceAddr, pDeviceAddr, B_ADDR_LEN);
    pData->uiInputs = uiInputs;
    pData->uiOutputs = uiOutputs;
    pData->numComparison = numComparison;

    // Enqueue the event.
    if (bleApp_enqueueMsg(BLEAPP_EVT_PASSCODE_NEEDED, pData) != SUCCESS)
    {
      bleStack_free(pData);
    }
  }
}

#ifdef ADD_SIMPLE_PERIPHERAL
/*********************************************************************
 * @fn      bleApp_processPairState
 *
 * @brief   Process the new paring state.
 *
 * @return  none
 */
static void bleApp_processPairState(appPairStateData_t *pPairData)
{
  uint8_t state = pPairData->state;
  uint8_t status = pPairData->status;

  switch (state)
  {
    case GAPBOND_PAIRING_STATE_STARTED:
      Display_printf(dispHandle, SP_ROW_CONNECTION, 0, "Pairing started");
      break;

    case GAPBOND_PAIRING_STATE_COMPLETE:
      if (status == SUCCESS)
      {
        Display_printf(dispHandle, SP_ROW_CONNECTION, 0, "Pairing success");
      }
      else
      {
        Display_printf(dispHandle, SP_ROW_CONNECTION, 0, "Pairing fail: %d", status);
      }
      break;

    case GAPBOND_PAIRING_STATE_ENCRYPTED:
      if (status == SUCCESS)
      {
        Display_printf(dispHandle, SP_ROW_CONNECTION, 0, "Encryption success");
      }
      else
      {
        Display_printf(dispHandle, SP_ROW_CONNECTION, 0, "Encryption failed: %d", status);
      }
      break;

    case GAPBOND_PAIRING_STATE_BOND_SAVED:
      if (status == SUCCESS)
      {
        Display_printf(dispHandle, SP_ROW_CONNECTION, 0, "Bond save success");
      }
      else
      {
        Display_printf(dispHandle, SP_ROW_CONNECTION, 0, "Bond save failed: %d", status);
      }
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      bleApp_processPasscode
 *
 * @brief   Process the Passcode request.
 *
 * @return  none
 */
static void bleApp_processPasscode(appPasscodeData_t *pPasscodeData)
{
  // Display passcode to user
  if (pPasscodeData->uiOutputs != 0)
  {
    Display_printf(dispHandle, SP_ROW_CONNECTION, 0, "Passcode: %d",
                   B_APP_DEFAULT_PASSCODE);
  }

  // Send passcode response
  GAPBondMgr_PasscodeRsp(pPasscodeData->connHandle , SUCCESS,
                         B_APP_DEFAULT_PASSCODE);
}

/*********************************************************************
 * @fn      SimplePeripheral_connEvtCB
 *
 * @brief   Connection event callback.
 *
 * @param pReport pointer to connection event report
 */
void SimplePeripheral_connEvtCB(Gap_ConnEventRpt_t *pReport)
{
  // Enqueue the event for processing in the app context.
  if(bleApp_enqueueMsg(BLEAPP_EVT_CONN, pReport) != SUCCESS)
  {
    bleStack_free(pReport);
  }
}

/*********************************************************************
 * @fn      bleApp_processConnEvt
 *
 * @brief   Process connection event.
 *
 * @param pReport pointer to connection event report
 */
static void bleApp_processConnEvt(Gap_ConnEventRpt_t *pReport)
{
#ifdef OAD_SUPPORT
  /* If we are waiting for an OAD Reboot, process connection events to ensure
   * that we are not waiting to send data before restarting
   */
  if (OadApp_processConnEvt() == TRUE)
  {
      return;
  }
#endif
  // Get index from handle
  uint8_t connIndex = SimplePeripheral_getConnIndex(pReport->handle);

  if (connIndex >= MAX_NUM_BLE_CONNS)
  {
    Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "Connection handle is not in the connList !!!");
    return;
  }

  // If auto phy change is enabled
  if (connList[connIndex].isAutoPHYEnable == TRUE)
  {
    // Read the RSSI
    HCI_ReadRssiCmd(pReport->handle);
  }
}
#endif // ADD_SIMPLE_PERIPHERAL

/*********************************************************************
 * @fn      bleApp_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   pData - message data pointer.
 */
static status_t bleApp_enqueueMsg(uint8_t event, void *pData)
{
  uint8_t success;
  appEvt_t *pMsg = bleStack_malloc(sizeof(appEvt_t));

  // Create dynamic pointer to message.
  if (pMsg)
  {
    pMsg->event = event;
    pMsg->pData = pData;

    // Enqueue the message.
    success = Util_enqueueMsg(appMsgQueueHandle, appSyncEvent, (uint8_t *)pMsg);
    return (success) ? SUCCESS : FAILURE;
  }

  return(bleMemAllocError);
}


/*********************************************************************
 * MESH init structure and callbacks
 * ---------------------------------
 */

// Init comp
// ---------
#define MOD_LF 0x0000

#define GROUP_ADDR 0xc000
#define PUBLISHER_ADDR  0x000f

#define OP_VENDOR_BUTTON BT_MESH_MODEL_OP_3(0x00, CONFIG_BT_COMPANY_ID)
#define OP_VENDOR_GET_STATUS    BT_MESH_MODEL_OP_3(0x01, CONFIG_BT_COMPANY_ID)
#define OP_VENDOR_NOTIFY_STATUS BT_MESH_MODEL_OP_3(0x02, CONFIG_BT_COMPANY_ID)
#define OP_VENDOR_LARGE_MSG     BT_MESH_MODEL_OP_3(0x03, CONFIG_BT_COMPANY_ID)

static const uint16_t app_idx;

uint16_t mesh_btn_target_addr = GROUP_ADDR;
uint16_t mesh_large_msg_target_addr = GROUP_ADDR;
uint16_t mesh_led_target_addr = GROUP_ADDR;
uint16_t mesh_gen_models_target_addr = 1;
uint16_t mesh_sensor_models_target_addr = 1;

uint8_t btn_status = 0;
uint8_t led_status = 0;

void heartbeat(uint8_t hops, uint16_t feat)
{
    //System_printf("heartbeat\n");
}

void vnd_button_pressed_cb(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf)
{
  // check own address
  if (ctx->addr == mesh_own_addr) {
          return;
  }

  led_status = net_buf_simple_pull_u8(buf);

#ifdef USE_APP_MENU
  // button pressed callback
  Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: Button pressed callback from addr=0x%x", ctx->addr);

  // turn on/off LED
  SimpleMeshMenu_turnLed(led_status);
#endif
}

void vnd_get_status_cb(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf)
{
  // check own address
  if (ctx->addr == mesh_own_addr) {
          return;
  }

#ifdef USE_APP_MENU
  // button pressed callback
  Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: Get status callback from addr=0x%x", ctx->addr);
#endif

  // send notify led status as a result of get status request
  {
    NET_BUF_SIMPLE_DEFINE(msg, 3 + 5);

    /* Bind to Health model */
    bt_mesh_model_msg_init(&msg, OP_VENDOR_NOTIFY_STATUS);
    net_buf_simple_add_u8(&msg, led_status);
    if (bt_mesh_model_send_data_wrapper(model->elem_idx, (get_model_info(model)).is_vnd, model->mod_idx, (msg_ctx_raw *)ctx, (buf_simple_raw *)&msg)) {
        Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: Unable to send Vendor Button message");
    }
    Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: Button message sent with OpCode 0x%08x", OP_VENDOR_BUTTON);
  }
}

void vnd_notify_status_cb(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf)
{
#if defined(USE_APP_MENU) && (!defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL))
  uint8_t status;

  // check own address
  if (ctx->addr == mesh_own_addr) {
          return;
  }

  // get status
  status = net_buf_simple_pull_u8(buf);

  // Notify status callback
  Display_printf(dispHandle, SMN_ROW_NTFY_ST, 0, "NTFY: from addr=0x%x, led_status=%d", ctx->addr, status);
#endif /* defined(USE_APP_MENU) && (!defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)) */
}

void get_large_msg_cb(struct bt_mesh_model *model,
                      struct bt_mesh_msg_ctx *ctx,
                      struct net_buf_simple *buf)
{

  // check own address
  if (ctx->addr == mesh_own_addr) {
          return;
  }

#ifdef USE_APP_MENU
  Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: get_large_msg_cb with %d bytes from addr=0x%x",
                 buf->len, ctx->addr);
#endif
}

void prov_complete_cbk(uint16_t net_idx, uint16_t addr)
{
    mesh_own_addr = addr;
    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: prov_complete net_idx=%02d, addr=%02d", net_idx, addr);
}

void prov_reset_cbk()
{
    Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK: device reset complete");

    //Enable provisioning bearers
    MeshApp_unprovisioned_beacon();
}

/*********************************************************************
 * @fn      MeshApp_scanCB
 *
 * @brief   Callback called by GapScan module
 *
 * @param   evt - event
 * @param   msg - message coming with the event
 * @param   arg - user argument
 *
 * @return  none
 */
void MeshApp_scanCB(uint32_t evt, void* pMsg, uintptr_t arg)
{
  uint8_t event;
  scanCbData_t *pData = bleStack_malloc(sizeof(scanCbData_t));

  if (evt & GAP_EVT_ADV_REPORT)
  {
    event = BLEAPP_EVT_ADV_REPORT;
  }
  else if (evt & GAP_EVT_SCAN_ENABLED)
  {
    event = BLEAPP_EVT_SCAN_ENABLED;
  }
  else if (evt & GAP_EVT_SCAN_DISABLED)
  {
    event = BLEAPP_EVT_SCAN_DISABLED;
  }
  else if (evt & GAP_EVT_INSUFFICIENT_MEMORY)
  {
    event = BLEAPP_EVT_INSUFFICIENT_MEM;
  }
  else
  {
    return;
  }

  if (pData)
  {
    pData->evt = evt;
    pData->pMsg = pMsg;
    pData->arg = arg;

    // Queue the event.
    if (bleApp_enqueueMsg(event, pData) != SUCCESS)
    {
      bleStack_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      SimpleMeshNode_gattWriteCB
 *
 * @brief   Callback called by GATT Server module
 *
 * This callback enqueues a message to call in the application context
 *
 * @param   evt - event
 * @param   msg - message coming with the event
 * @param   arg - user argument
 *
 * @return  none
 */
void SimpleMeshNode_gattWriteCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint16 len, uint16 offset,
                                 uint8 method )
{
#ifdef CONFIG_BT_MESH_PROXY
  uint8_t event;
  writeCbParams_t *pData = bleStack_malloc(sizeof(writeCbParams_t));

  event = MESHAPP_EVT_GATT_WRT;

  if (pData)
  {
    pData->connHandle = connHandle;
    pData->len = len;
    pData->method = method;
    pData->offset = offset;
    pData->pAttr = pAttr;

    pData->pValue =  bleStack_malloc(len);
    memcpy(pData->pValue, pValue, len);

    // Queue the event.
    if (bleApp_enqueueMsg(event, pData) != SUCCESS)
    {
      bleStack_free(pData);
    }
  }
#endif // CONFIG_BT_MESH_PROXY
}

/*********************************************************************
 * MESH menu actions
 * -----------------
 */

uint8_t menu_init_called = 0;
int generic_models_extensions(void)
{
    int16_t onoff_model_index = bt_mesh_model_find_wrapper(0, BT_MESH_MODEL_ID_GEN_ONOFF_SRV);
    struct bt_mesh_model *genOnOffSrv = get_model_data(0, 0, onoff_model_index);

    int16_t power_onoff_model_index = bt_mesh_model_find_wrapper(0, BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV);
    struct bt_mesh_model *genPowerOnOffSrv = get_model_data(0, 0, power_onoff_model_index);

    int16_t power_onoff_setup_model_index = bt_mesh_model_find_wrapper(0, BT_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV);
    struct bt_mesh_model *genPowerOnOffSetupSrv = get_model_data(0, 0, power_onoff_setup_model_index);

    int16_t def_trans_time_model_index = bt_mesh_model_find_wrapper(0, BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV);
    struct bt_mesh_model *genTransTimeSrv = get_model_data(0, 0, def_trans_time_model_index);

    // Extend the genOnOffSrv model by the genPowerOnOffSrv model
    if(genOnOffSrv && genPowerOnOffSrv)
    {
        if(bt_mesh_model_extend_wrapper(genPowerOnOffSrv->elem_idx, (get_model_info(genPowerOnOffSrv)).is_vnd, genPowerOnOffSrv->mod_idx,
                                        genOnOffSrv->elem_idx, (get_model_info(genOnOffSrv)).is_vnd, genOnOffSrv->mod_idx) != 0)
        {
            return -1;
        }
    }
    // Extend the genPowerOnOffSrv model by the genPowerOnOffSetupSrv model
    if(genPowerOnOffSrv && genPowerOnOffSetupSrv)
    {
        if(bt_mesh_model_extend_wrapper(genPowerOnOffSetupSrv->elem_idx, (get_model_info(genPowerOnOffSetupSrv)).is_vnd, genPowerOnOffSetupSrv->mod_idx,
                                        genPowerOnOffSrv->elem_idx, (get_model_info(genPowerOnOffSrv)).is_vnd, genPowerOnOffSrv->mod_idx) != 0)
        {
            return -1;
        }
    }
    // Extend the genTransTimeSrv model by the genPowerOnOffSrv model
    if(genTransTimeSrv && genPowerOnOffSetupSrv)
    {
        if(bt_mesh_model_extend_wrapper(genPowerOnOffSetupSrv->elem_idx, (get_model_info(genPowerOnOffSetupSrv)).is_vnd, genPowerOnOffSetupSrv->mod_idx,
                                        genTransTimeSrv->elem_idx, (get_model_info(genTransTimeSrv)).is_vnd, genTransTimeSrv->mod_idx) != 0)
        {
            return -1;
        }
    }
    return 0;
}

/*********************************************************************
 * @fn      MeshApp_saveDefaultUuid
 *
 * @brief   Saves the default device UUID
 *
 * @param   none
 *
 * @return  none
 */
void MeshApp_saveDefaultUuid()
{
    memcpy((void*)defaultUUID, MeshApp_getUuid(), UUID_MAX_LEN);
}

/*********************************************************************
 * @fn      MeshApp_getUuid
 *
 * @brief   Returns the device UUID
 *
 * @param   none
 *
 * @return  device UUID
 */
const uint8_t * MeshApp_getUuid()
{
    return prov.uuid;
}

/*********************************************************************
 * @fn      MeshApp_setUuid
 *
 * @brief   Sets the device UUID chosen by the user
 *
 * @param   index - the index of the chosen UUID in TBM
 *
 * @return  none
 */
void MeshApp_setUuid(uint8_t index)
{
    const uint8_t *uuid;
    static const uint8_t uuid_values[NUM_OF_UUID_ADDRESSES][UUID_MAX_LEN] = { // UUID RFC 4122 format
        {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0xa1,0x11,0x11,0x11,0x11,0x11,0x11,0x11},
        {0x22,0x22,0x22,0x22,0x22,0x22,0x12,0x22,0xa2,0x22,0x22,0x22,0x22,0x22,0x22,0x22},
        {0x33,0x33,0x33,0x33,0x33,0x33,0x13,0x33,0xa3,0x33,0x33,0x33,0x33,0x33,0x33,0x33},
        {0x44,0x44,0x44,0x44,0x44,0x44,0x14,0x44,0xa4,0x44,0x44,0x44,0x44,0x44,0x44,0x44},
        {0x55,0x55,0x55,0x55,0x55,0x55,0x15,0x55,0xa5,0x55,0x55,0x55,0x55,0x55,0x55,0x55},
        {0x66,0x66,0x66,0x66,0x66,0x66,0x16,0x66,0xa6,0x66,0x66,0x66,0x66,0x66,0x66,0x66},
        {0x77,0x77,0x77,0x77,0x77,0x77,0x17,0x77,0xa7,0x77,0x77,0x77,0x77,0x77,0x77,0x77},
        {0x88,0x88,0x88,0x88,0x88,0x88,0x18,0x88,0xa8,0x88,0x88,0x88,0x88,0x88,0x88,0x88},
        {0x99,0x99,0x99,0x99,0x99,0x99,0x19,0x99,0xa9,0x99,0x99,0x99,0x99,0x99,0x99,0x99},
        {0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0x1a,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,0xaa}
    };

    if(index == 0) // Set the UUID to the default UUID value
    {
        uuid = defaultUUID;
    }
    else
    {
        uuid = uuid_values[index - 1];
    }
    memcpy((void *)prov.uuid, uuid, UUID_MAX_LEN);
}

/*********************************************************************
 * @fn      MeshApp_init
 *
 * @brief   init the mesh stack
 *
 * @param   addr - awn address
 *
 * @return  none
 */
int MeshApp_init(uint16_t addr, uint8_t prov_type)
{
  int status = 0;

  // init the mesh stack
  generic_models_extensions();
  status = mesh_init();
  if (status)
  {
      return status;
  }

  if(prov_type == UNPROVISIONED_BEACON)
  {
#ifdef USE_STATIC_PROVISIONING
      MeshApp_static_provisioning(DEVICE_OWN_ADDRESS, app_idx);
#else
      status = MeshApp_unprovisioned_beacon();
#endif

#ifdef USE_APP_MENU
      Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: unprovisioned beacon status=%d", status);
#endif
  }
  else if(prov_type == LOAD_FROM_NV)
  {
#ifdef CONFIG_BT_SETTINGS
    // Try to load settings from NV, if there are no settings saved this will do nothing
    status = settings_load();
#ifdef USE_APP_MENU
    Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "NVS: settings_load ret status: %d", status);
#endif
#endif
  }

  // Init generic models
  MeshModels_init();

  menu_init_called = 1;

  Util_startClock(&clkPeriodic);

#ifdef ADD_SENSOR_MODELS
  sensorTmpr8_register(SAMPLE_APP_MODELS_ELEMENT);
#endif

  return status;
}

/*********************************************************************
 * @fn      MeshApp_unprovisioned_beacon
 *
 * @brief   calls bt_mesh_prov_enable
 *
 * @return  none
 */
int MeshApp_unprovisioned_beacon(void)
{
  int status;
#if defined(CONFIG_BT_MESH_PB_ADV) && defined(CONFIG_BT_MESH_PB_GATT)
  status = bt_mesh_prov_enable_wrapper((bt_mesh_prov_bearer)(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT));
#elif defined(CONFIG_BT_MESH_PB_ADV) && !defined(CONFIG_BT_MESH_PB_GATT)
  status = bt_mesh_prov_enable_wrapper((bt_mesh_prov_bearer)(BT_MESH_PROV_ADV));
#elif !defined(CONFIG_BT_MESH_PB_ADV) && defined(CONFIG_BT_MESH_PB_GATT)
  status = bt_mesh_prov_enable_wrapper((bt_mesh_prov_bearer)(BT_MESH_PROV_GATT));
  BLEAPP_ASSERT(status == SUCCESS);
#else
  // USE_UNPROVISIONED_BEACON most be enabled with an ADV Bearer
  BLEAPP_ASSERT(FAILURE);

#endif

  return status;
}


/*********************************************************************
 * @fn      MeshApp_button_pressed
 *
 * @brief   send mesh message as a result of button presses
 *
 * @param   onoff_state - button on/off
 *
 * @return  none
 */
int MeshApp_button_pressed(uint8_t onoff_state)
{
  struct bt_mesh_model *pModel;
  NET_BUF_SIMPLE_DEFINE(msg, 3 + 5);

  struct bt_mesh_msg_ctx ctx = {
          .app_idx = app_idx,
          .addr = mesh_btn_target_addr,
          .send_ttl = BT_MESH_TTL_DEFAULT,
  };

  int16_t model_index = bt_mesh_model_find_vnd_wrapper(SAMPLE_APP_MODELS_ELEMENT, BT_COMP_ID, 0);
  pModel = get_model_data(SAMPLE_APP_MODELS_ELEMENT, 1, model_index);
  if (pModel == NULL)
  {
      Display_printf(dispHandle, SMN_ROW_NTFY_ST, 0, "The vnd model doesn't exist in the comp data, add it in order to send this msg");
      return -1;
  }

  /* Bind to Health model */
  bt_mesh_model_msg_init(&msg, OP_VENDOR_BUTTON);
  net_buf_simple_add_u8(&msg, onoff_state);
  if (bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg)) {
      return -1;
  }

  return 0;
}

genOnOffSet_t genOnOffSet =
{
     .onOff = 0,
     .tid = 0,
     .transitionTime = 0,
     .delay = 0
};
int genOnOffSet_value = 0;
int genOnOffSet_dataLen = 4;

int MeshApp_genModels_onOff_pressed(uint8_t action_id)
{
  int status = 0;
  struct bt_mesh_model *pModel;

  NET_BUF_SIMPLE_DEFINE(msg, 2 + 10 + 4); // Allocate max client message len

  struct bt_mesh_msg_ctx ctx = {
        .app_idx = app_idx,
        .addr = mesh_gen_models_target_addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
  };

  int16_t model_index = bt_mesh_model_find_wrapper(SAMPLE_APP_MODELS_ELEMENT, BT_MESH_MODEL_ID_GEN_ONOFF_CLI);
  pModel = get_model_data(SAMPLE_APP_MODELS_ELEMENT, 0, model_index);
  if (pModel == NULL)
  {
      return -1;
  }

  switch (action_id)
  {
    case 0:  // cli OnOff get
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_GEN_ONOFF_GET);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 1:  // cli OnOff set
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_GEN_ONOFF_SET);
        genOnOffSet.onOff = genOnOffSet_value;
        net_buf_simple_add_mem(&msg, (void *)&genOnOffSet, genOnOffSet_dataLen);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 2:  // cli OnOff set unack
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK);
        genOnOffSet.onOff = genOnOffSet_value;
        net_buf_simple_add_mem(&msg, (void *)&genOnOffSet, genOnOffSet_dataLen);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 3:  // prepare onOff=0
        genOnOffSet_value = 0;
        break;

    case 4:  // prepare onOff=1
        genOnOffSet_value = 1;
        break;

    case 5:  // prepare onOff timing 0x00,0x0
        genOnOffSet.tid = 0;
        genOnOffSet.transitionTime = 0;
        genOnOffSet.delay = 0;
        genOnOffSet_dataLen = sizeof(genOnOffSet);
        break;

    case 6:  // prepare onOff timing 0x54,0x5
        genOnOffSet.tid = 0;
        genOnOffSet.transitionTime = 0x54;
        genOnOffSet.delay = 0x5;
        genOnOffSet_dataLen = sizeof(genOnOffSet);
        break;

    case 7:  // prepare onOff timing without
        genOnOffSet.tid = 0;
        genOnOffSet_dataLen = 2;  // without timing params
        break;

    default:
        status = -1;
  }

  return status;
}

genLevelSet_t genLevelSet =
{
     .level = 0x01FF,
     .tid = 0,
     .transitionTime = 0,
     .delay = 0
};
int genLevelSet_dataLen = 5;
int genLevelSet_level = 0x01FF;

genDeltaLevelSet_t genDeltaLevelSet =
{
     .deltaLevel = 0x01FF0000,
     .tid = 0,
     .transitionTime = 0,
     .delay = 0
};
int genDeltaLevelSet_dataLen = 7;
int genDeltaLevelSet_level = 0x000001FF;

genMoveSet_t genMoveSet =
{
     .deltaLevel = 0x01FF,
     .tid = 0,
     .transitionTime = 0,
     .delay = 0
};
int genMoveSet_dataLen = 5;
int genMoveSet_deltaLevel = 0x01FF;

int MeshApp_genModels_level_pressed(uint8_t action_id)
{
  int status = 0;
  struct bt_mesh_model *pModel;

  NET_BUF_SIMPLE_DEFINE(msg, 2 + 16 + 4); // Allocate max client message len

  struct bt_mesh_msg_ctx ctx = {
        .app_idx = app_idx,
        .addr = mesh_gen_models_target_addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
  };

  int16_t model_index = bt_mesh_model_find_wrapper(SAMPLE_APP_MODELS_ELEMENT, BT_MESH_MODEL_ID_GEN_LEVEL_CLI);
  pModel = get_model_data(SAMPLE_APP_MODELS_ELEMENT, 0, model_index);
  if (pModel == NULL)
  {
      return -1;
  }

  switch (action_id)
  {
    case 0:  // cli level get
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_GEN_LEVEL_GET);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 1:  // cli level set
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_GEN_LEVEL_SET);
        genLevelSet.level = genLevelSet_level;
        net_buf_simple_add_mem(&msg, (void *)&genLevelSet, genLevelSet_dataLen);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 2:  // cli level set unack
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK);
        genLevelSet.level = genLevelSet_level;
        net_buf_simple_add_mem(&msg, (void *)&genLevelSet, genLevelSet_dataLen);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 3:  // cli delta set
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_GEN_DELTA_SET);
        genDeltaLevelSet.deltaLevel = genDeltaLevelSet_level;
        net_buf_simple_add_mem(&msg, (void *)&genDeltaLevelSet, genDeltaLevelSet_dataLen);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 4:  // cli delta set unack
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_GEN_DELTA_SET_UNACK);
        genDeltaLevelSet.deltaLevel = genDeltaLevelSet_level;
        net_buf_simple_add_mem(&msg, (void *)&genDeltaLevelSet, genDeltaLevelSet_dataLen);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 5:  // cli move set
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_GEN_MOVE_SET);
        genMoveSet.deltaLevel = genMoveSet_deltaLevel;
        net_buf_simple_add_mem(&msg, (void *)&genMoveSet, genMoveSet_dataLen);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 6:  // cli move set unack
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_GEN_MOVE_SET_UNACK);
        genMoveSet.deltaLevel = genMoveSet_deltaLevel;
        net_buf_simple_add_mem(&msg, (void *)&genMoveSet, genMoveSet_dataLen);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 7:  // prepare cli level
        genLevelSet_level = 0x01FF;
        genDeltaLevelSet_level = 0x000001FF;
        genMoveSet_deltaLevel = 0x01FF;
        break;

    case 8:  // prepare cli timing 0xC0, 0
        genLevelSet.tid = 0;
        genLevelSet.transitionTime = 0xC0;
        genLevelSet.delay = 0;
        genLevelSet_dataLen = sizeof(genLevelSet);

        genDeltaLevelSet.tid = 0;
        genDeltaLevelSet.transitionTime = 0xC0;
        genDeltaLevelSet.delay = 0;
        genDeltaLevelSet_dataLen = sizeof(genDeltaLevelSet);

        genMoveSet.tid = 0;
        genMoveSet.transitionTime = 0xC0;
        genMoveSet.delay = 5;
        genMoveSet_dataLen = sizeof(genMoveSet);
        break;

    case 9:  // prepare cli timing 0x54, 5
        genLevelSet.tid = 0;
        genLevelSet.transitionTime = 0x54;
        genLevelSet.delay = 5;
        genLevelSet_dataLen = sizeof(genLevelSet);

        genDeltaLevelSet.tid = 0;
        genDeltaLevelSet.transitionTime = 0x54;
        genDeltaLevelSet.delay = 5;
        genDeltaLevelSet_dataLen = sizeof(genDeltaLevelSet);

        genMoveSet.tid = 0;
        genMoveSet.transitionTime = 0x54;
        genMoveSet.delay = 5;
        genMoveSet_dataLen = sizeof(genMoveSet);
        break;

    case 10:  // prepare cli timing without
        genLevelSet.tid = 0;
        genLevelSet_dataLen = 3;        // without timing params

        genDeltaLevelSet.tid = 0;
        genDeltaLevelSet_dataLen = 5;   // without timing params

        genMoveSet.tid = 0;
        genMoveSet_dataLen = 3;         // without timing params
        break;

    default:
        status = -1;
  }

  return status;
}

genTransTimeSet_t genTransTimeSet =
{
     .transTime = 0,
};
int genTransTimeSet_value = 0;
int genTransTimeSet_dataLen = 1;

int MeshApp_genModels_transTime_pressed(uint8_t action_id)
{
  int status = 0;
  struct bt_mesh_model *pModel;

  NET_BUF_SIMPLE_DEFINE(msg, 2 + 10 + 4); // Allocate max client message len

  struct bt_mesh_msg_ctx ctx = {
        .app_idx = app_idx,
        .addr = mesh_gen_models_target_addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
  };

  int16_t model_index = bt_mesh_model_find_wrapper(SAMPLE_APP_MODELS_ELEMENT, BT_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI);
  pModel = get_model_data(SAMPLE_APP_MODELS_ELEMENT, 0, model_index);
  if (pModel == NULL)
  {
      return -1;
  }

  switch (action_id)
  {
    case 0:  // cli TransTime get
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_GEN_DEF_TRANS_TIME_GET);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 1:  // cli TransTime set
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_GEN_DEF_TRANS_TIME_SET);
        genTransTimeSet.transTime = genTransTimeSet_value;
        net_buf_simple_add_mem(&msg, (void *)&genTransTimeSet, genTransTimeSet_dataLen);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 2:  // cli TransTime set unack
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_GEN_DEF_TRANS_TIME_SET_UNACK);
        genTransTimeSet.transTime = genTransTimeSet_value;
        net_buf_simple_add_mem(&msg, (void *)&genTransTimeSet, genTransTimeSet_dataLen);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 3:  // prepare value=0xCF
        genTransTimeSet_value = 0xCF;
        break;

    case 4:  // prepare value=0x8F
        genTransTimeSet_value = 0x8F;
        break;

    default:
        status = -1;
  }

  return status;
}

int MeshApp_genModels_powerOnOff_pressed(uint8_t action, uint8_t value)
{
  int status = 0;
  struct bt_mesh_model *pModel;

  NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4); // Allocate max client message len

  struct bt_mesh_msg_ctx ctx = {
        .app_idx = app_idx,
        .addr = mesh_gen_models_target_addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
  };

  int16_t model_index = bt_mesh_model_find_wrapper(SAMPLE_APP_MODELS_ELEMENT, BT_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI);
  pModel = get_model_data(SAMPLE_APP_MODELS_ELEMENT, 0, model_index);
  if (pModel == NULL)
  {
      return -1;
  }

  switch (action)
  {
    case 0:  // cli Power OnOff get
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_GEN_ONPOWERUP_GET);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 1:  // cli Power OnOff set
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_GEN_ONPOWERUP_SET);
        net_buf_simple_add_u8(&msg, value);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    case 2:  // cli Power OnOff set unack
        bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_GEN_ONPOWERUP_SET_UNACK);
        net_buf_simple_add_u8(&msg, value);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

    default:
        status = -1;
  }

  return status;
}

int MeshApp_genModels_battery_pressed(uint8_t action)
{
    int status = 0;
    struct bt_mesh_model *pModel;

    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4); // Allocate max client message len

    struct bt_mesh_msg_ctx ctx = {
          .app_idx = app_idx,
          .addr = mesh_gen_models_target_addr,
          .send_ttl = BT_MESH_TTL_DEFAULT,
    };

    int16_t model_index = bt_mesh_model_find_wrapper(SAMPLE_APP_MODELS_ELEMENT, BT_MESH_MODEL_ID_GEN_BATTERY_CLI);
    pModel = get_model_data(SAMPLE_APP_MODELS_ELEMENT, 0, model_index);
    if (pModel == NULL)
    {
        return -1;
    }

    switch (action)
    {
      case 0:  // cli Battery get
        bt_mesh_model_msg_init(&msg, BLE_MESH_MODEL_OP_GEN_BATTERY_GET);
        status = bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg);
        break;

      default:
          status = -1;
    }

    return status;
}
uint16_t selectedPropertyId = TEMPERATURE_SENSOR_PROPERTY_ID;
int MeshApp_sensorModelsClient_pressed(uint8_t action_id, uint8_t value)
{
  int status = 0;
  struct bt_mesh_model *pModel;

  struct bt_mesh_msg_ctx ctx = {
        .app_idx = app_idx,
        .addr = mesh_sensor_models_target_addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
  };

  int16_t model_index = bt_mesh_model_find_wrapper(SAMPLE_APP_MODELS_ELEMENT, BT_MESH_MODEL_ID_SENSOR_CLI);
  pModel = get_model_data(SAMPLE_APP_MODELS_ELEMENT, 0, model_index);
  if (pModel == NULL)
  {
      return -1;
  }

  switch (action_id)
  {
    case 0:  // sensor_desc_get
        sensorModels_send_desc_get(pModel, &ctx, selectedPropertyId);
        break;

    case 1:  // sensor_get
        sensorModels_send_get(pModel, &ctx, selectedPropertyId);
        break;

    case 2:  // sensor_Column_get
        sensorModels_send_Column_get(pModel, &ctx, selectedPropertyId, 0x10);
        break;

    case 3:  // sensor_Series_get
        sensorModels_send_Series_get(pModel, &ctx, selectedPropertyId, 0x10, 0x20);
        break;

    case 4:  // property ID tmpr8
        selectedPropertyId = TEMPERATURE_SENSOR_PROPERTY_ID;
        break;

    case 5:  // property ID unknown
        selectedPropertyId = UNKNOWN_SENSOR_PROPERTY_ID;
        break;

    case 6:  // property ID all
        selectedPropertyId = ALL_SENSOR_PROPERTY_ID;
        break;

    case 7:  // sensor_settings_get
        sensorModels_send_settings_get(pModel, &ctx, selectedPropertyId);
        break;

    case 8:  // sensor_cadence_get
        sensorModels_send_cadence_get(pModel, &ctx, selectedPropertyId);
        break;

    case 9:  // sensor_cadence_set
        sensorModels_send_cadence_set(pModel, &ctx, selectedPropertyId);
        break;

    case 10: // sensor_cadence_set_unack
        sensorModels_send_cadence_set_unack(pModel, &ctx, selectedPropertyId);
        break;

    case 11: // sensor_setting_get
        sensorModels_send_setting_get(pModel, &ctx, selectedPropertyId, 0x10);
        break;

    case 12: // sensor_setting_set
        sensorModels_send_setting_set(pModel, &ctx, selectedPropertyId, 0x10);
        break;

    case 13: // sensor_setting_set_unack
        sensorModels_send_setting_set_unack(pModel, &ctx, selectedPropertyId, 0x10);
        break;

    default:
        status = -1;
  }

  return status;
}

int MeshApp_sensorModelsServer_pressed(uint8_t action_id, uint8_t value)
{
  int status = 0;
  struct bt_mesh_model *pModel;

  struct bt_mesh_msg_ctx ctx = {
        .app_idx = app_idx,
        .addr = mesh_sensor_models_target_addr,
        .send_ttl = BT_MESH_TTL_DEFAULT,
  };

  int16_t model_index = bt_mesh_model_find_wrapper(SAMPLE_APP_MODELS_ELEMENT, BT_MESH_MODEL_ID_SENSOR_SRV);
  pModel = get_model_data(SAMPLE_APP_MODELS_ELEMENT, 0, model_index);
  if (pModel == NULL)
  {
      return -1;
  }


  switch (action_id)
  {
    case 0: // set temperatue 36
        sensorTmpr8_setTemperature((int8_t)(36 * 2));
        break;
    case 1: // set temperatue 40
        sensorTmpr8_setTemperature((int8_t)(40 * 2));
        break;
    case 2: // sensor_setting_status
        sensorModels_send_status_by_id(pModel, &ctx, TEMPERATURE_SENSOR_PROPERTY_ID);
        break;

    default:
        status = -1;
  }

  return status;
}

/*********************************************************************
 * @fn      MeshApp_get_status_pressed
 *
 * @brief   send mesh message as a result get led status presses
 *
 * @param   none
 *
 * @return  none
 */
int MeshApp_get_status_pressed()
{
  struct bt_mesh_model *pModel;
  NET_BUF_SIMPLE_DEFINE(msg, 3 + 5);

  struct bt_mesh_msg_ctx ctx = {
          .app_idx = app_idx,
          .addr = mesh_btn_target_addr,
          .send_ttl = BT_MESH_TTL_DEFAULT,
  };

  int16_t model_index = bt_mesh_model_find_vnd_wrapper(SAMPLE_APP_MODELS_ELEMENT, BT_COMP_ID, 0);
  pModel = get_model_data(SAMPLE_APP_MODELS_ELEMENT, 1, model_index);
  if (pModel == NULL)
  {
      Display_printf(dispHandle, SMN_ROW_NTFY_ST, 0, "The vnd model doesn't exist in the comp data, add it in order to send this msg");
      return -1;
  }

  /* Bind to Health model */
  bt_mesh_model_msg_init(&msg, OP_VENDOR_GET_STATUS);

  if (bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg)) {
      return -1;
  }

  return 0;
}

/*********************************************************************
 * @fn      MeshApp_send_large_msg
 *
 * @brief   send mesh message as a result get led status presses
 *
 * @param   none
 *
 * @return  none
 */
int MeshApp_send_large_msg(uint16 msg_size)
{
  struct bt_mesh_model *pModel;
  NET_BUF_SIMPLE_DEFINE(msg, msg_size + 3 + 4);

  struct bt_mesh_msg_ctx ctx = {
          .app_idx = app_idx,
          .addr = mesh_large_msg_target_addr,
          .send_ttl = BT_MESH_TTL_DEFAULT,
  };

  int16_t model_index = bt_mesh_model_find_vnd_wrapper(SAMPLE_APP_MODELS_ELEMENT, BT_COMP_ID, 0);
  pModel = get_model_data(SAMPLE_APP_MODELS_ELEMENT, 1, model_index);
  if (pModel == NULL)
  {
      Display_printf(dispHandle, SMN_ROW_NTFY_ST, 0, "The vnd model doesn't exist in the comp data, add it in order to send this msg");
      return -1;
  }
  // Create the msg buffer and fill it with the msg opcode + data
  bt_mesh_model_msg_init(&msg, OP_VENDOR_LARGE_MSG);
  for(int i = 0; i < msg_size; i++)
  {
      net_buf_simple_add_u8(&msg, 0xAB);
  }
  if (bt_mesh_model_send(pModel, &ctx, &msg, NULL, NULL)) {
      return -1;
  }

  return 0;
}
/*********************************************************************
 * @fn      MeshApp_notify_status_pressed
 *
 * @brief   send mesh message as a result of notify LED status presses
 *
 * @param   led_status - LEN status on/off
 *
 * @return  none
 */
int MeshApp_notify_status_pressed(uint8_t led_status)
{
  struct bt_mesh_model *pModel;
  NET_BUF_SIMPLE_DEFINE(msg, 3 + 5);

  struct bt_mesh_msg_ctx ctx = {
          .app_idx = app_idx,
          .addr = mesh_led_target_addr,
          .send_ttl = BT_MESH_TTL_DEFAULT,
  };

  int16_t model_index = bt_mesh_model_find_vnd_wrapper(SAMPLE_APP_MODELS_ELEMENT, BT_COMP_ID, 0);
  pModel = get_model_data(SAMPLE_APP_MODELS_ELEMENT, 1, model_index);
  if (pModel == NULL)
  {
      Display_printf(dispHandle, SMN_ROW_NTFY_ST, 0, "The vnd model doesn't exist in the comp data, add it in order to send this msg");
      return -1;
  }

  /* Bind to Health model */
  bt_mesh_model_msg_init(&msg, OP_VENDOR_NOTIFY_STATUS);
  net_buf_simple_add_u8(&msg, led_status);
  if (bt_mesh_model_send_data_wrapper(pModel->elem_idx, (get_model_info(pModel)).is_vnd, pModel->mod_idx, (msg_ctx_raw *)&ctx, (buf_simple_raw *)&msg)) {
      // Unable to send Vendor Button message
      return -1;
  }

  // Button message sent with OpCode OP_VENDOR_BUTTON
  return 0;
}

#ifdef ADD_SIMPLE_PERIPHERAL
/*********************************************************************
 * @fn      SimplePeripheral_addConn
 *
 * @brief   Add a device to the connected device list
 *
 * @return  index of the connected device list entry where the new connection
 *          info is put in.
 *          if there is no room, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t SimplePeripheral_addConn(uint16_t connHandle)
{
  uint8_t i;
  uint8_t status = bleNoResources;

  // Try to find an available entry
  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == LINKDB_CONNHANDLE_INVALID)
    {
      // Found available entry to put a new connection info in
      connList[i].connHandle = connHandle;

#ifdef DEFAULT_SEND_PARAM_UPDATE_REQ
      // Allocate data to send through clock handler
      connList[i].pParamUpdateEventData = bleStack_malloc(sizeof(appClockEventData_t) +
                                                       sizeof (uint16_t));
      if(connList[i].pParamUpdateEventData)
      {
        connList[i].pParamUpdateEventData->event = BLEAPP_EVT_SEND_PARAM_UPDATE;
        *((uint16_t *)connList[i].pParamUpdateEventData->data) = connHandle;

        // Create a clock object and start
        connList[i].pUpdateClock
          = (Clock_Struct*) bleStack_malloc(sizeof(Clock_Struct));

        if (connList[i].pUpdateClock)
        {
          Util_constructClock(connList[i].pUpdateClock,
                              bleApp_clockHandler,
                              SEND_PARAM_UPDATE_DELAY, 0, true,
                              (UArg) (connList[i].pParamUpdateEventData));
        }
        else
        {
            bleStack_free(connList[i].pParamUpdateEventData);
        }
      }
      else
      {
        status = bleMemAllocError;
      }
#endif

      // Set default PHY to 1M
      connList[i].currPhy = HCI_PHY_1_MBPS;

      break;
    }
  }

  return status;
}

/*********************************************************************
 * @fn      SimplePeripheral_getConnIndex
 *
 * @brief   Find index in the connected device list by connHandle
 *
 * @return  the index of the entry that has the given connection handle.
 *          if there is no match, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t SimplePeripheral_getConnIndex(uint16_t connHandle)
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
 * @fn      SimplePeripheral_getConnIndex
 *
 * @brief   Find index in the connected device list by connHandle
 *
 * @return  SUCCESS if connHandle found valid index or bleInvalidRange
 *          if index wasn't found. LINKDB_CONNHANDLE_ALL will always succeed.
 */
static uint8_t SimplePeripheral_clearConnListEntry(uint16_t connHandle)
{
  uint8_t i;
  // Set to invalid connection index initially
  uint8_t connIndex = MAX_NUM_BLE_CONNS;

  if(connHandle != LINKDB_CONNHANDLE_ALL)
  {
    // Get connection index from handle
    connIndex = SimplePeripheral_getConnIndex(connHandle);
    if(connIndex >= MAX_NUM_BLE_CONNS)
    {
      return(bleInvalidRange);
    }
  }

  // Clear specific handle or all handles
  for(i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if((connIndex == i) || (connHandle == LINKDB_CONNHANDLE_ALL))
    {
      connList[i].connHandle = LINKDB_CONNHANDLE_INVALID;
      connList[i].currPhy = 0;
      connList[i].phyCngRq = 0;
      connList[i].phyRqFailCnt = 0;
      connList[i].rqPhy = 0;
      memset(connList[i].rssiArr, 0, SP_MAX_RSSI_STORE_DEPTH);
      connList[i].rssiAvg = 0;
      connList[i].rssiCntr = 0;
      connList[i].isAutoPHYEnable = FALSE;
    }
  }

  return(SUCCESS);
}

/*********************************************************************
 * @fn      SimplePeripheral_clearPendingParamUpdate
 *
 * @brief   clean pending param update request in the paramUpdateList list
 *
 * @param   connHandle - connection handle to clean
 *
 * @return  none
 */
void SimplePeripheral_clearPendingParamUpdate(uint16_t connHandle)
{
  List_Elem *curr;

  for (curr = List_head(&paramUpdateList); curr != NULL; curr = List_next(curr))
  {
    if (((spConnHandleEntry_t *)curr)->connHandle == connHandle)
    {
      List_remove(&paramUpdateList, curr);
    }
  }
}

/*********************************************************************
 * @fn      SimplePeripheral_removeConn
 *
 * @brief   Remove a device from the connected device list
 *
 * @return  index of the connected device list entry where the new connection
 *          info is removed from.
 *          if connHandle is not found, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t SimplePeripheral_removeConn(uint16_t connHandle)
{
  uint8_t connIndex = SimplePeripheral_getConnIndex(connHandle);

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
      bleStack_free(pUpdateClock);
      // Free ParamUpdateEventData
      bleStack_free(connList[connIndex].pParamUpdateEventData);
    }
    // Clear pending update requests from paramUpdateList
    SimplePeripheral_clearPendingParamUpdate(connHandle);
    // Stop Auto PHY Change
    SimplePeripheral_stopAutoPhyChange(connHandle);
    // Clear Connection List Entry
    SimplePeripheral_clearConnListEntry(connHandle);
  }

  return connIndex;
}

/*********************************************************************
 * @fn      bleApp_processParamUpdate
 *
 * @brief   Process a parameters update request
 *
 * @return  None
 */
static void bleApp_processParamUpdate(uint16_t connHandle)
{
  gapUpdateLinkParamReq_t req;
  uint8_t connIndex;

  req.connectionHandle = connHandle;
#ifdef DEFAULT_SEND_PARAM_UPDATE_REQ
  req.connLatency = DEFAULT_DESIRED_SLAVE_LATENCY;
  req.connTimeout = DEFAULT_DESIRED_CONN_TIMEOUT;
  req.intervalMin = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
  req.intervalMax = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
#endif

  connIndex = SimplePeripheral_getConnIndex(connHandle);
  if (connIndex >= MAX_NUM_BLE_CONNS)
  {
    Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "Connection handle is not in the connList !!!");
    return;
  }

  // Deconstruct the clock object
  Clock_destruct(connList[connIndex].pUpdateClock);
  // Free clock struct, only in case it is not NULL
  if (connList[connIndex].pUpdateClock != NULL)
  {
      bleStack_free(connList[connIndex].pUpdateClock);
      connList[connIndex].pUpdateClock = NULL;
  }
  // Free ParamUpdateEventData, only in case it is not NULL
  if (connList[connIndex].pParamUpdateEventData != NULL)
      bleStack_free(connList[connIndex].pParamUpdateEventData);

  // Send parameter update
  bStatus_t status = GAP_UpdateLinkParamReq(&req);

  // If there is an ongoing update, queue this for when the udpate completes
  if (status == bleAlreadyInRequestedMode)
  {
    spConnHandleEntry_t *connHandleEntry = bleStack_malloc(sizeof(spConnHandleEntry_t));
    if (connHandleEntry)
    {
      connHandleEntry->connHandle = connHandle;

      List_put(&paramUpdateList, (List_Elem *)connHandleEntry);
    }
  }
}

/*********************************************************************
 * @fn      SimpleCentral_processCmdCompleteEvt
 *
 * @brief   Process an incoming OSAL HCI Command Complete Event.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void bleApp_processCmdCompleteEvt(hciEvt_CmdComplete_t *pMsg)
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

        uint8_t index = SimplePeripheral_getConnIndex(handle);
        if (index >= MAX_NUM_BLE_CONNS)
        {
          Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "Connection handle is not in the connList !!!");
          return;
        }

        if (rssi != LL_RSSI_NOT_AVAILABLE)
        {
          connList[index].rssiArr[connList[index].rssiCntr++] = rssi;
          connList[index].rssiCntr %= SP_MAX_RSSI_STORE_DEPTH;

          int16_t sum_rssi = 0;
          for(uint8_t cnt=0; cnt<SP_MAX_RSSI_STORE_DEPTH; cnt++)
          {
            sum_rssi += connList[index].rssiArr[cnt];
          }
          connList[index].rssiAvg = (uint32_t)(sum_rssi/SP_MAX_RSSI_STORE_DEPTH);

          uint8_t phyRq = SP_PHY_NONE;
          uint8_t phyRqS = SP_PHY_NONE;
          uint8_t phyOpt = LL_PHY_OPT_NONE;

          if(connList[index].phyCngRq == FALSE)
          {
            if((connList[index].rssiAvg >= RSSI_2M_THRSHLD) &&
            (connList[index].currPhy != HCI_PHY_2_MBPS) &&
                 (connList[index].currPhy != SP_PHY_NONE))
            {
              // try to go to higher data rate
              phyRqS = phyRq = HCI_PHY_2_MBPS;
            }
            else if((connList[index].rssiAvg < RSSI_2M_THRSHLD) &&
                    (connList[index].rssiAvg >= RSSI_1M_THRSHLD) &&
                    (connList[index].currPhy != HCI_PHY_1_MBPS) &&
                    (connList[index].currPhy != SP_PHY_NONE))
            {
              // try to go to legacy regular data rate
              phyRqS = phyRq = HCI_PHY_1_MBPS;
            }
            else if((connList[index].rssiAvg >= RSSI_S2_THRSHLD) &&
                    (connList[index].rssiAvg < RSSI_1M_THRSHLD) &&
                    (connList[index].currPhy != SP_PHY_NONE))
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
            if((phyRq != SP_PHY_NONE) &&
               // First check if the request for this phy change is already not honored then don't request for change
               (((connList[index].rqPhy == phyRq) &&
                 (connList[index].phyRqFailCnt < 2)) ||
                 (connList[index].rqPhy != phyRq)))
            {
              //Initiate PHY change based on RSSI
              SimplePeripheral_setPhy(connList[index].connHandle, 0,
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

            } // end of if ((phyRq != SP_PHY_NONE) && ...
          } // end of if (connList[index].phyCngRq == FALSE)
        } // end of if (rssi != LL_RSSI_NOT_AVAILABLE)

        Display_printf(dispHandle, SP_ROW_RSSI, 0,
                       "RSSI:%d dBm, AVG RSSI:%d dBm",
                       (uint32_t)(rssi),
                       connList[index].rssiAvg);

      } // end of if (status == SUCCESS)
      break;
    }

    case HCI_LE_READ_PHY:
    {
      if (status == SUCCESS)
      {
        Display_printf(dispHandle, SP_ROW_RSSI + 2, 0, "RXPh: %d, TXPh: %d",
                       pMsg->pReturnParam[3], pMsg->pReturnParam[4]);
      }
      break;
    }

    default:
      break;
  } // end of switch (pMsg->cmdOpcode)
}

/*********************************************************************
* @fn      SimplePeripheral_initPHYRSSIArray
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
static void SimplePeripheral_initPHYRSSIArray(void)
{
  //Initialize array to store connection handle and RSSI values
  memset(connList, 0, sizeof(connList));
  for (uint8_t index = 0; index < MAX_NUM_BLE_CONNS; index++)
  {
    connList[index].connHandle = SP_INVALID_HANDLE;
  }
}
/*********************************************************************
      // Set default PHY to 1M
 * @fn      SimplePeripheral_startAutoPhyChange
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
static status_t SimplePeripheral_startAutoPhyChange(uint16_t connHandle)
{
  status_t status = FAILURE;

  // Get connection index from handle
  uint8_t connIndex = SimplePeripheral_getConnIndex(connHandle);
  BLEAPP_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  // Start Connection Event notice for RSSI calculation
  status = Gap_RegisterConnEventCb(SimplePeripheral_connEvtCB, GAP_CB_REGISTER, connHandle);

  // Flag in connection info if successful
  if (status == SUCCESS)
  {
    connList[connIndex].isAutoPHYEnable = TRUE;
  }

  return status;
}

/*********************************************************************
 * @fn      SimplePeripheral_stopAutoPhyChange
 *
 * @brief   Cancel periodic RSSI reads on a link.
 *
 * @param   connHandle - connection handle of link
 *
 * @return  SUCCESS: Operation successful
 *          bleIncorrectMode: No link
 */
static status_t SimplePeripheral_stopAutoPhyChange(uint16_t connHandle)
{
  // Get connection index from handle
  uint8_t connIndex = SimplePeripheral_getConnIndex(connHandle);
  BLEAPP_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  // Stop connection event notice
  Gap_RegisterConnEventCb(NULL, GAP_CB_UNREGISTER, connHandle);

  // Also update the phychange request status for active RSSI tracking connection
  connList[connIndex].phyCngRq = FALSE;
  connList[connIndex].isAutoPHYEnable = FALSE;

  return SUCCESS;
}

/*********************************************************************
 * @fn      SimplePeripheral_setPhy
 *
 * @brief   Call the HCI set phy API and and add the handle to a
 *          list to match it to an incoming command status event
 */
static status_t SimplePeripheral_setPhy(uint16_t connHandle, uint8_t allPhys,
                                        uint8_t txPhy, uint8_t rxPhy,
                                        uint16_t phyOpts)
{
  // Allocate list entry to store handle for command status
  spConnHandleEntry_t *connHandleEntry = bleStack_malloc(sizeof(spConnHandleEntry_t));

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
* @fn      SimplePeripheral_updatePHYStat
*
* @brief   Update the auto phy update state machine
*
* @param   connHandle - the connection handle
*
* @return  None
*/
static void SimplePeripheral_updatePHYStat(uint16_t eventCode, uint8_t *pMsg)
{
  uint8_t connIndex;

  switch (eventCode)
  {
    case HCI_LE_SET_PHY:
    {
      // Get connection handle from list
      spConnHandleEntry_t *connHandleEntry =
                           (spConnHandleEntry_t *)List_get(&setPhyCommStatList);

      if (connHandleEntry)
      {
        // Get index from connection handle
        connIndex = SimplePeripheral_getConnIndex(connHandleEntry->connHandle);

        bleStack_free(connHandleEntry);

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
      hciEvt_BLEPhyUpdateComplete_t *pPUC =
                                     (hciEvt_BLEPhyUpdateComplete_t*) pMsg;

      if(pPUC)
      {
        // Get index from connection handle
        connIndex = SimplePeripheral_getConnIndex(pPUC->connHandle);

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

/*********************************************************************
 * SP menu actions
 * ---------------
 */
/*********************************************************************
 * @fn      SimplePeripheral_setConnPhy
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
bool SimplePeripheral_setConnPhy(uint8 index)
{
  bool status = TRUE;

  static uint8_t phy[] = {
    HCI_PHY_1_MBPS, HCI_PHY_2_MBPS, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS,
    HCI_PHY_CODED, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS | HCI_PHY_CODED,
    AUTO_PHY_UPDATE
  };

  uint8_t connIndex = SimplePeripheral_getConnIndex(menuConnHandle);
  if (connIndex >= MAX_NUM_BLE_CONNS)
  {
    Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "Connection handle is not in the connList !!!");
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
    SimplePeripheral_stopAutoPhyChange(connList[connIndex].connHandle);

    SimplePeripheral_setPhy(menuConnHandle, 0, phy[index], phy[index], 0);

    Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "PHY preference: %s",
                   TBM_GET_ACTION_DESC(&spMenuConnPhy, index));
  }
  else
  {
    // Start RSSI read for auto PHY update (if it is disabled)
    SimplePeripheral_startAutoPhyChange(menuConnHandle);
  }

  return status;
}

/*********************************************************************
* @fn      SimplePeripheral_setConnIndex
*
* @brief   get connection handle by index to connList
*
* @param
*
* @return  None
*/
void SimplePeripheral_setConnIndex(uint8_t index)
{
  menuConnHandle = connList[index].connHandle;
}

uint16_t SimplePeripheral_getConnHandle(uint8_t index)
{
  return connList[index].connHandle;
}


/*********************************************************************
* @fn      SimplePeripheral_setAutoConnect
*
* @brief   change Adv data for auto connect
*
* @param
*
* @return  None
*/
void SimplePeripheral_setAutoConnect(uint8_t newAutoConnect)
{
  GapAdv_disable(advHandle1);
#ifdef SECOND_ADV_SET
  GapAdv_disable(advHandle2);
#endif /* SECOND_ADV_SET */

  if (autoConnect != newAutoConnect)
  {
    if (newAutoConnect == AUTOCONNECT_GROUP_A)
    {
        advData1[2] = 'G';
        advData1[3] = 'A';
#ifdef SECOND_ADV_SET
        advData2[2] = 'G';
        advData2[3] = 'A';
#endif /* SECOND_ADV_SET */
    }
    else if (newAutoConnect == AUTOCONNECT_GROUP_A)
    {
          advData1[2] = 'G';
          advData1[3] = 'B';
#ifdef SECOND_ADV_SET
          advData2[2] = 'G';
          advData2[3] = 'B';
#endif /* SECOND_ADV_SET */
    }
    else if (newAutoConnect == AUTOCONNECT_DISABLE)
    {
          advData1[2] = 'S';
          advData1[3] = 'P';
#ifdef SECOND_ADV_SET
          advData2[2] = 'S';
          advData2[3] = 'P';
#endif /* SECOND_ADV_SET */
    }
  }

  GapAdv_enable(advHandle1, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
#ifdef SECOND_ADV_SET
  GapAdv_enable(advHandle2, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
#endif /* SECOND_ADV_SET */

  autoConnect = newAutoConnect;
}

#endif // ADD_SIMPLE_PERIPHERAL

/*********************************************************************
 * PTM - ToDo move to another module
 * ---------------------------------
 */
#ifdef PTM_MODE

#include "npi_task.h"       // To allow RX event registration
#include "npi_ble.h"        // To enable transmission of messages to UART
#include "icall_hci_tl.h"   // To allow ICall HCI Transport Layer

void PTM_handleNPIRxInterceptEvent(uint8_t *pMsg);
static void PTM_sendToNPI(uint8_t *buf, uint16_t len);

/*********************************************************************
* @fn      PTM_init
*
* @brief   Init PTM activity
*/
void PTM_init(bleStack_entityId_t appSelfEntity)
{
  // Intercept NPI RX events.
  NPITask_registerIncomingRXEventAppCB(PTM_handleNPIRxInterceptEvent, INTERCEPT);

  // Register for Command Status information
  HCI_TL_Init(NULL, (HCI_TL_CommandStatusCB_t) PTM_sendToNPI, NULL, appSelfEntity);

  // Register for Events
  HCI_TL_getCmdResponderID(ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, appSelfEntity));

  // Inform Stack to Initialize PTM
  HCI_EXT_EnablePTMCmd();
}

/*********************************************************************
* @fn      PTM_handle_hci_event
*
* @brief   send HCI CTRL to HOST event to NPI
*/
void PTM_handle_hci_event(uint8_t *pMsg)
{
  // Check for NPI Messages
  hciPacket_t *pBuf = (hciPacket_t *)pMsg;

  // Serialized HCI Event
  if (pBuf->hdr.event == HCI_CTRL_TO_HOST_EVENT)
  {
    uint16_t len = 0;

    // Determine the packet length
    switch(pBuf->pData[0])
    {
      case HCI_EVENT_PACKET:
        len = HCI_EVENT_MIN_LENGTH + pBuf->pData[2];
        break;

      case HCI_ACL_DATA_PACKET:
        len = HCI_DATA_MIN_LENGTH + BUILD_UINT16(pBuf->pData[3], pBuf->pData[4]);
        break;

      default:
        break;
    }

    // Send to Remote Host.
    PTM_sendToNPI(pBuf->pData, len);

    // Free buffers if needed.
    switch (pBuf->pData[0])
    {
      case HCI_ACL_DATA_PACKET:
      case HCI_SCO_DATA_PACKET:
        BM_free(pBuf->pData);
      default:
        break;
    }
  }
}

/*********************************************************************
* @fn      PTM_handleNPIRxInterceptEvent
*
* @brief   Intercept an NPI RX serial message and queue for this application.
*
* @param   pMsg - a NPIMSG_msg_t containing the intercepted message.
*
* @return  none.
*/
void PTM_handleNPIRxInterceptEvent(uint8_t *pMsg)
{
 // Send Command via HCI TL
 HCI_TL_SendToStack(((NPIMSG_msg_t *)pMsg)->pBuf);

 // The data is stored as a message, free this first.
 bleStack_freeMsg(((NPIMSG_msg_t *)pMsg)->pBuf);

 // Free container.
 bleStack_free(pMsg);
}

/*********************************************************************
* @fn      PTM_sendToNPI
*
* @brief   Create an NPI packet and send to NPI to transmit.
*
* @param   buf - pointer HCI event or data.
*
* @param   len - length of buf in bytes.
*
* @return  none
*/
static void PTM_sendToNPI(uint8_t *buf, uint16_t len)
{
 npiPkt_t *pNpiPkt = (npiPkt_t *)ICall_allocMsg(sizeof(npiPkt_t) + len);

 if (pNpiPkt)
 {
   pNpiPkt->hdr.event = buf[0]; //Has the event status code in first byte of payload
   pNpiPkt->hdr.status = 0xFF;
   pNpiPkt->pktLen = len;
   pNpiPkt->pData  = (uint8 *)(pNpiPkt + 1);

   memcpy(pNpiPkt->pData, buf, len);

   // Send to NPI
   // Note: there is no need to free this packet.  NPI will do that itself.
   NPITask_sendToHost((uint8_t *)pNpiPkt);
 }
}
#endif // PTM_MODE

/*********************************************************************
 * Mesh Stack Callbacks
 */
#ifdef USE_APP_MENU
/* Low Power Node Callbacks */
#ifdef CONFIG_BT_MESH_LOW_POWER
/** @brief Friendship established.
 *
 *  This callback notifies the application that friendship has
 *  been successfully established.
 *
 *  @param net_idx  NetKeyIndex used during friendship establishment.
 *  @param friend_addr Friend address.
 *  @param queue_size  Friend queue size.
 *  @param recv_window Low Power Node's listens duration for
 *  Friend response.
 */
static void meshApp_lpn_friendshipEstablished(uint16_t net_idx, uint16_t friend_addr,
		    uint8_t queue_size, uint8_t recv_window)
{
  Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK:LPN friendship established, friend address = 0x%04x", friend_addr);
}

/** @brief Friendship terminated.
 *
 *  This callback notifies the application that friendship has
 *  been terminated.
 *
 *  @param net_idx  NetKeyIndex used during friendship establishment.
 *  @param friend_addr Friend address.
 */
static void meshApp_lpn_friendshipTerminated(uint16_t net_idx, uint16_t friend_addr)
{
  Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK:LPN friendship terminated, friend address = 0x%04x", friend_addr);
}
/** @brief Local Poll Request.
 *
 *  This callback notifies the application that the local node has
 *  polled the friend node.
 *
 *  This callback will be called before @ref bt_mesh_lpn_cb::established
 *  when attempting to establish a friendship.
 *
 *  @param net_idx  NetKeyIndex used during friendship establishment.
 *  @param friend_addr Friend address.
 *  @param retry Retry or first poll request for each transaction.
 */
static void meshApp_lpnPolled(uint16_t net_idx, uint16_t friend_addr, bool retry)
{
  Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK:LPN Poll");
}
#endif /* CONFIG_BT_MESH_LOW_POWER */

/* Heartbeat Callbacks */
/** @brief Receive callback for heartbeats.
 *
 *  Gets called on every received Heartbeat that matches the current
 *  Heartbeat subscription parameters.
 *
 *  @param sub  Current Heartbeat subscription parameters.
 *  @param hops The number of hops the Heartbeat was received
 *              with.
 *  @param feat The feature set of the publishing node. The
 *              value is a bitmap of @ref BT_MESH_FEAT_RELAY,
 *              @ref BT_MESH_FEAT_PROXY,
 *              @ref BT_MESH_FEAT_FRIEND and
 *              @ref BT_MESH_FEAT_LOW_POWER.
 */
static void meshApp_hbRecv(const struct bt_mesh_hb_sub *sub, uint8_t hops,
	     uint16_t feat)
{
  Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK:Heartbeat Received from 0x%04x", sub->src);
}

/** @brief Subscription end callback for heartbeats.
 *
 *  Gets called when the subscription period ends, providing a summary
 *  of the received heartbeat messages.
 *
 *  @param sub Current Heartbeat subscription parameters.
 */
static void meshApp_hb_subEnd(const struct bt_mesh_hb_sub *sub)
{
  Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK:Heartbeat Subscription Ended, Source Address: 0x%04x",sub->src);
}


/* Friend Callbacks */
#ifdef CONFIG_BT_MESH_FRIEND
/** @brief Friendship established.
 *
 *  This callback notifies the application that friendship has
 *  been successfully established.
 *
 *  @param net_idx  NetKeyIndex used during friendship establishment.
 *  @param lpn_addr Low Power Node address.
 *  @param recv_delay Receive Delay in units of 1 millisecond.
 *  @param polltimeout PollTimeout in units of 1 millisecond.
 */
static void meshApp_friend_friendshipEstablished(uint16_t net_idx, uint16_t lpn_addr,
			    uint8_t recv_delay, uint32_t polltimeout)
{
  Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK:Friendship Established, lpn_addr = 0x%04x", lpn_addr);
}

/** @brief Friendship terminated.
 *
 *  This callback notifies the application that friendship has
 *  been terminated.
 *
 *  @param net_idx  NetKeyIndex used during friendship establishment.
 *  @param lpn_addr Low Power Node address.
 */
static void meshApp_friend_friendshipTerminated(uint16_t net_idx, uint16_t lpn_addr)
{
  Display_printf(dispHandle, SMN_ROW_MESH_CB, 0, "MESH CBK:Friendship Terminated, lpn_addr = 0x%04x", lpn_addr);
}
#endif /* CONFIG_BT_MESH_FRIEND */
#endif /* USE_APP_MENU */

/*********************************************************************
*********************************************************************/
