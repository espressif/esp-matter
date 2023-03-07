/******************************************************************************

 @file  central_display.c

 @brief This file contains the central display sample application for use
        with the CC2650 Bluetooth Low Energy Protocol Stack.

 Group: WCS, BTS
 Target Device: cc13x2_26x2

 ******************************************************************************

 Copyright (c) 2013-2019, Texas Instruments Incorporated
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

#include <stdio.h>

#include "bcomdef.h"

#include "sensor.h"

#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "osal_list.h"
#include <ti_drivers_config.h>

#include "ti_ble_config.h"
#include "ble_user_config.h"

#include "simple_gatt_profile.h"

#include "central_display.h"
#include "cui.h"
#include "ti_dmm_application_policy.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Application events
#define CD_EVT_SCAN_ENABLED        0x01
#define CD_EVT_SCAN_DISABLED       0x02
#define CD_EVT_ADV_REPORT          0x03
#define CD_EVT_SVC_DISC            0x04
#define CD_EVT_READ_RSSI           0x05
#define CD_EVT_PAIR_STATE          0x06
#define CD_EVT_PASSCODE_NEEDED     0x07
#define CD_EVT_READ_RPA            0x08
#define CD_EVT_INSUFFICIENT_MEM    0x09
#define CD_UI_INPUT_EVT            0x0A
#define CD_EVT_TGL_NOTIF           0x0B

// Simple Central Task Events
#define CD_ICALL_EVT                         ICALL_MSG_EVENT_ID  // Event_Id_31
#define CD_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30

#define CD_ALL_EVENTS                        (CD_ICALL_EVT           | \
                                              CD_QUEUE_EVT)

// TRUE to filter discovery results on desired service UUID
#define DEFAULT_DEV_DISC_BY_SVC_UUID          FALSE

// Supervision timeout conversion rate to miliseconds
#define CONN_TIMEOUT_MS_CONVERSION            10

// Task configuration
#define CD_TASK_PRIORITY                     1

#ifndef CD_TASK_STACK_SIZE
#define CD_TASK_STACK_SIZE                   1024
#endif

// Size of string-converted device address ("0xXXXXXXXXXXXX")
#define CD_ADDR_STR_SIZE     15

// Spin if the expression is not true
#define CENTRALDISPLAY_ASSERT(expr) if (!(expr)) CentralDisplay_spin();

// Timeout for the initiator to cancel connection if not successful
#define CONNECTION_TIMEOUT                   5000

// Auto connect chosen group
#define GROUP_NAME_LENGTH                    4

// Max number of scan results to show in ui
#define DEFAULT_MAX_SCAN_RES                 8

// Max number of notifiable characteristics per peripheral connection
#define MAX_CCCD_HANDLES                     5

// Value of connHandleInProgress when auto connect is not currently connecting to a device
#define CONN_NOT_IN_PROGRESS                  0xFFFF

/*********************************************************************
 * TYPEDEFS
 */

// Auto connect availble groups
enum
{
  AUTOCONNECT_DISABLE = 0,              // Disable
  AUTOCONNECT_GROUP_A = 1,              // Group A
  AUTOCONNECT_GROUP_B = 2               // Group B
};

// Discovery states
enum
{
  BLE_DISC_STATE_IDLE,                // Idle
  BLE_DISC_STATE_MTU,                 // Exchange ATT MTU size
  BLE_DISC_STATE_CHAR                 // Characteristic discovery
};

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr; // event header
  uint8_t *pData;  // event data
} cdEvt_t;

// Scanned device information record
typedef struct
{
  uint8_t addrType;         // Peer Device's Address Type
  uint8_t addr[B_ADDR_LEN]; // Peer Device Address
  uint16_t manFacID; // Advertised Manufacturer ID
} scanRec_t;

// Connected device information
typedef struct
{
  uint16_t connHandle;        // Connection Handle
  uint8_t  addr[B_ADDR_LEN];  // Peer Device Address
  Clock_Struct *pRssiClock;   // pointer to clock struct
  uint8_t noCharCCCDHdl;      // Number of notifiable characteristics
  uint16_t charCCCDHdl[MAX_CCCD_HANDLES]; // Notifiable Characteristic handles
  uint16_t charCCCDUUID[MAX_CCCD_HANDLES]; // Notifiable Characteristic UUIDs
  uint8_t notifEnabled; // Notification status (subscribed/not)
  uint16_t manFacID; // Advertised Manufacturer ID
} connRec_t;

// Container to store paring state info when passing from gapbondmgr callback
// to app event. See the pfnPairStateCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct
{
  uint16_t connHandle;
  uint8_t  status;
} cdPairStateData_t;

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
} cdPasscodeData_t;

typedef struct
{
    osal_list_elem elem;
    uint8_t  addr[B_ADDR_LEN];  // member's BDADDR
    uint8_t  addrType;          // member's Address Type
    uint16_t connHandle;        // member's connection handle
    uint8_t  status;            // bitwise status flag
} groupListElem_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle syncEvent;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// Task configuration
Task_Struct cdTask;
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(cdTaskStack, 8)
#else
#pragma data_alignment=8
#endif
uint8_t cdTaskStack[CD_TASK_STACK_SIZE];

// Number of scan results
static uint8_t numScanRes = 0;

// Scan results filtered by Service UUID
static scanRec_t scanList[DEFAULT_MAX_SCAN_RES];

// Number of connected devices
static uint8_t numConn = 0;

// Index into scanList for current connection attempt
static uint8_t scanListIndex = 0;

// List of connections
static connRec_t connList[MAX_NUM_BLE_CONNS];

// Connection handle of current connection
static uint16_t cdConnHandle = LINKDB_CONNHANDLE_INVALID;

// Accept or reject L2CAP connection parameter update request
static bool acceptParamUpdateReq = true;

// Discovery state
static uint8_t discState = BLE_DISC_STATE_IDLE;

// Value to write
static uint8_t charVal = 0;

// Maximum PDU size (default = 27 octets)
static uint16_t cdMaxPduSize;

// Clock instance for RPA read events.
static Clock_Struct clkRpaRead;

// Address mode
static GAP_Addr_Modes_t addrMode = DEFAULT_ADDRESS_MODE;

// Current Random Private Address
static uint8 rpa[B_ADDR_LEN] = {0};

// Auto connect Disabled/Enabled {0 - Disabled, 1- Group A , 2-Group B, ...}
static uint8_t autoConnect = AUTOCONNECT_DISABLE;

// Keep track of current connection handle when autoConnect enabled until
// post connection set up is complete
static uint16_t connHandleInProgress = CONN_NOT_IN_PROGRESS;

// Keep track of current Manufacturer ID when autoConnect is enabled
static uint16_t manFacIdInProgress = 0;

//AutoConnect ADV data filter according to local name short
static uint8_t acGroup[4] = {
  0x03,
  GAP_ADTYPE_LOCAL_NAME_SHORT,
  'G',
  'A'
 };

#if DEFAULT_DEV_DISC_BY_SVC_UUID
//Filter to use for scan results when DEFAULT_DEV_DISC_BY_SVC_UUID is set
static uint16_t SVC_UUID_FILTER = SIMPLEPROFILE_SERV_UUID;
#endif

// Connection Update Params
static uint16_t minConnInt = DEFAULT_UPDATE_MIN_CONN_INTERVAL;
static uint16_t maxConnInt = DEFAULT_UPDATE_MAX_CONN_INTERVAL;
static uint16_t svTimeout = DEFAULT_UPDATE_CONN_TIMEOUT;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void CentralDisplay_init(void);
static void CentralDisplay_taskFxn(uintptr_t a0, uintptr_t a1);

static uint8_t CentralDisplay_isMember(uint8_t *advData , uint8_t *groupName , uint8_t len);
static uint8_t CentralDisplay_processStackMsg(ICall_Hdr *pMsg);
static void CentralDisplay_processGapMsg(gapEventHdr_t *pMsg);
static void CentralDisplay_processGATTMsg(gattMsgEvent_t *pMsg);
static void CentralDisplay_processAppMsg(cdEvt_t *pMsg);
static void CentralDisplay_processGATTDiscEvent(gattMsgEvent_t *pMsg);
static void CentralDisplay_startSvcDiscovery(void);
static void CentralDisplay_toggleNotifications();
#if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
static bool CentralDisplay_findSvcUuid(uint16_t uuid, uint8_t *pData,
                                      uint16_t dataLen);
#endif
static uint16_t CentralDisplay_findManFacID(uint8_t *pAddr, uint16_t dataLen, uint8_t *pData);
static void CentralDisplay_addScanInfo(uint8_t *pAddr, uint8_t addrType, uint16_t dataLen, uint8_t *pData);
static uint8_t CentralDisplay_addConnInfo(uint16_t connHandle, uint8_t *pAddr);
static uint8_t CentralDisplay_removeConnInfo(uint16_t connHandle);
static uint8_t CentralDisplay_getConnIndex(uint16_t connHandle);
static char* CentralDisplay_getConnAddrStr(uint16_t connHandle);
static void CentralDisplay_processPairState(uint8_t state,
                                           cdPairStateData_t* pPairStateData);
static void CentralDisplay_processPasscode(cdPasscodeData_t *pData);

static void CentralDisplay_processCmdCompleteEvt(hciEvt_CmdComplete_t *pMsg);
static status_t CentralDisplay_StartRssi();
static status_t CentralDisplay_CancelRssi(uint16_t connHandle);

static void CentralDisplay_passcodeCb(uint8_t *deviceAddr, uint16_t connHandle,
                                     uint8_t uiInputs, uint8_t uiOutputs,
                                     uint32_t numComparison);
static void CentralDisplay_pairStateCb(uint16_t connHandle, uint8_t state,
                                      uint8_t status);

static void CentralDisplay_clockHandler(UArg arg);

static status_t CentralDisplay_enqueueMsg(uint8_t event, uint8_t status,
                                        uint8_t *pData);

static void CentralDisplay_scanCb(uint32_t evt, void* msg, uintptr_t arg);
static void CentralDisplay_processMenuUpdate(void);
bool CentralDisplay_doSetConnPhy(uint8_t index);
static void CentralDisplay_cyclePhy(const uint32_t _listIndex, char* _lines[3], bool _selected);
static void CentralDisplay_doAutoConnect(const uint32_t _listIndex, char* _lines[3], bool _selected);
static void CentralDisplay_doDiscoverDevices(int32_t menuEntryIndex);
static void CentralDisplay_doSetScanPhy(const uint32_t _listIndex, char* _lines[3], bool _selected);
static void CentralDisplay_doConnect(const uint32_t _listIndex, char* _lines[3], bool _selected);
static void CentralDisplay_doConnUpdate(int32_t menuEntryIndex);
static uint8_t moveCursor(uint8_t col, uint8_t left_boundary, uint8_t right_boundary, uint8_t direction);
static void CentralDisplay_setMinConnInt(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo);
static void CentralDisplay_setMaxConnInt(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo);
static void CentralDisplay_setSvTimeout(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo);
static void CentralDisplay_doDisconnect(int32_t menuEntryIndex);
static void CentralDisplay_doSelectConn(const uint32_t _listIndex, char* _lines[3], bool _selected);
static void CentralDisplay_doTglNotifications(int32_t menuEntryIndex);


// CUI
CUI_clientHandle_t CentralDisplayCuiHndl;
uint32_t rdStatusLineDeviceInfo;
uint32_t rdStatusLineConnInfo1;
uint32_t rdStatusLineConnInfo2;
uint32_t rdStatusLineConnInfo3;
uint32_t rdStatusLinePairInfo;
uint32_t rdStatusLineWorkWith;
uint32_t rdStatusLineStatus;
uint32_t rdStatusLineData;

CUI_SUB_MENU(rdConnUpdateSubMenu, "<  Conn Update   >", 4, rdWorkWithSubMenu)
    CUI_MENU_ITEM_ACTION( "<  Send Update   >", CentralDisplay_doConnUpdate)
    CUI_MENU_ITEM_INT_ACTION( "<  Min Conn Int  >", CentralDisplay_setMinConnInt)
    CUI_MENU_ITEM_INT_ACTION( "<  Max Conn Int  >", CentralDisplay_setMaxConnInt)
    CUI_MENU_ITEM_INT_ACTION( "< Supervision TO >", CentralDisplay_setSvTimeout)
CUI_SUB_MENU_END

CUI_SUB_MENU(rdWorkWithSubMenu, "<   Work With    >", 5, CentralDisplayMainMenu)
    CUI_MENU_ITEM_LIST_ACTION("<   Connection   >", MAX_NUM_BLE_CONNS, CentralDisplay_doSelectConn)
    CUI_MENU_ITEM_ACTION("<  Toggle Notif  >", CentralDisplay_doTglNotifications)
    CUI_MENU_ITEM_LIST_ACTION("<  Set Conn PHY  >", 5, CentralDisplay_cyclePhy)
    CUI_MENU_ITEM_SUBMENU(rdConnUpdateSubMenu)
    CUI_MENU_ITEM_ACTION( "<   Disconnect   >", CentralDisplay_doDisconnect)
CUI_SUB_MENU_END

#define CD_MENU_TITLE " TI Central Display "

CUI_MAIN_MENU(CentralDisplayMainMenu, CD_MENU_TITLE, 5, CentralDisplay_processMenuUpdate)
    CUI_MENU_ITEM_LIST_ACTION("<  Set Scan PHY  >", 2, CentralDisplay_doSetScanPhy)
    CUI_MENU_ITEM_LIST_ACTION("<  Auto Connect  >", 3, CentralDisplay_doAutoConnect)
    CUI_MENU_ITEM_ACTION(     "<    Discover    >", CentralDisplay_doDiscoverDevices)
    CUI_MENU_ITEM_LIST_ACTION("<    Connect     >", DEFAULT_MAX_SCAN_RES, CentralDisplay_doConnect)
    CUI_MENU_ITEM_SUBMENU(rdWorkWithSubMenu)
CUI_MAIN_MENU_END

/*********************************************************************
 * EXTERN FUNCTIONS
 */
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Bond Manager Callbacks
static gapBondCBs_t bondMgrCBs =
{
  CentralDisplay_passcodeCb, // Passcode callback
  CentralDisplay_pairStateCb // Pairing/Bonding state Callback
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/*********************************************************************
 * @fn      CentralDisplay_isMember
 *
 * @brief   Check if Advertiser is part of the group according to its Adv Data
 *
 * @param   advData   - pointer to adv data
 *          groupNmae - group name which need to be compared with
 *          len       - length of the group name
 *
 * @return  TRUE: part of the group
 *          FALSE: not part of the group
 */

static uint8_t CentralDisplay_isMember(uint8_t *advData , uint8_t *groupName , uint8_t len)
{
  if (osal_memcmp((uint8_t *)advData, (uint8_t *)groupName, len))
  {
    return TRUE;
  }
  return FALSE;
}

/*********************************************************************
 * @fn      CentralDisplay_spin
 *
 * @brief   Spin forever
 *
 * @param   none
 */
static void CentralDisplay_spin(void)
{
  volatile uint8_t x;

  while(1)
  {
    x++;
  }
}

/*********************************************************************
 * @fn      CentralDisplay_createTask
 *
 * @brief   Task creation function for the Simple Central.
 *
 * @param   none
 *
 * @return  none
 */
void CentralDisplay_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = cdTaskStack;
  taskParams.stackSize = CD_TASK_STACK_SIZE;
  taskParams.priority = CD_TASK_PRIORITY;

  Task_construct(&cdTask, CentralDisplay_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      CentralDisplay_Init
 *
 * @brief   Initialization function for the Simple Central App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification).
 *
 * @param   none
 *
 * @return  none
 */
static void CentralDisplay_init(void)
{
  uint8_t i;

  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  // Initialize internal data
  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    connList[i].connHandle = LINKDB_CONNHANDLE_INVALID;
    connList[i].pRssiClock = NULL;
  }

  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN,
                   (void *)attDeviceName);

  //Set default values for Data Length Extension
  //Extended Data Length Feature is already enabled by default
  //in build_config.opt in stack project.
  {
    //Change initial values of RX/TX PDU and Time, RX is set to max. by default(251 octets, 2120us)
    #define APP_SUGGESTED_RX_PDU_SIZE 251     //default is 251 octets(RX)
    #define APP_SUGGESTED_RX_TIME     17000   //default is 17000us(RX)
    #define APP_SUGGESTED_TX_PDU_SIZE 27      //default is 27 octets(TX)
    #define APP_SUGGESTED_TX_TIME     328     //default is 328us(TX)

    //This API is documented in hci.h
    //See the LE Data Length Extension section in the BLE5-Stack User's Guide for information on using this command:
    //http://software-dl.ti.com/lprf/ble5stack-latest/
    HCI_EXT_SetMaxDataLenCmd(APP_SUGGESTED_TX_PDU_SIZE, APP_SUGGESTED_TX_TIME, APP_SUGGESTED_RX_PDU_SIZE, APP_SUGGESTED_RX_TIME);
  }

  // Initialize GATT Client
  VOID GATT_InitClient();

  // Register to receive incoming ATT Indications/Notifications
  GATT_RegisterForInd(selfEntity);

  // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);         // GAP
  GATTServApp_AddService(GATT_ALL_SERVICES); // GATT attributes

  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs(selfEntity);

  // Set Bond Manager parameters
  setBondManagerParameters();

  // Start Bond Manager and register callback
  // This must be done before initialing the GAP layer
  VOID GAPBondMgr_Register(&bondMgrCBs);

  // Accept all parameter update requests
  GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, GAP_UPDATE_REQ_ACCEPT_ALL);

  // Register with GAP for HCI/Host messages (for RSSI)
  GAP_RegisterForMsgs(selfEntity);

  // Initialize GAP layer for Central role and register to receive GAP events
  GAP_DeviceInit(GAP_PROFILE_CENTRAL, selfEntity, addrMode, NULL);

  /* Open UI for key and LED */
  CUI_clientParams_t clientParams;
  CUI_clientParamsInit(&clientParams);
  strncpy(clientParams.clientName, "15.4 Sensor Central Display", MAX_CLIENT_NAME_LEN);
  clientParams.maxStatusLines = 8;

  CentralDisplayCuiHndl = CUI_clientOpen(&clientParams);
  CUI_registerMenu(CentralDisplayCuiHndl, &CentralDisplayMainMenu);

  CUI_statusLineResourceRequest(CentralDisplayCuiHndl, "Device Info", false, &rdStatusLineDeviceInfo);
  CUI_statusLineResourceRequest(CentralDisplayCuiHndl, "  Conn Info", false, &rdStatusLineConnInfo1);
  CUI_statusLineResourceRequest(CentralDisplayCuiHndl, "  Conn Info", false, &rdStatusLineConnInfo2);
  CUI_statusLineResourceRequest(CentralDisplayCuiHndl, "  Conn Info", false, &rdStatusLineConnInfo3);
  CUI_statusLineResourceRequest(CentralDisplayCuiHndl, "  Pair Info", false, &rdStatusLinePairInfo);
  CUI_statusLineResourceRequest(CentralDisplayCuiHndl, "  Work With", false, &rdStatusLineWorkWith);
  CUI_statusLineResourceRequest(CentralDisplayCuiHndl, "     Status", false, &rdStatusLineStatus);
  CUI_statusLineResourceRequest(CentralDisplayCuiHndl, "       Data", false, &rdStatusLineData);
}

/*********************************************************************
 * @fn      CentralDisplay_taskFxn
 *
 * @brief   Application task entry point for the Simple Central.
 *
 * @param   none
 *
 * @return  events not processed
 */
static void CentralDisplay_taskFxn(uintptr_t a0, uintptr_t a1)
{
  // Initialize application
  CentralDisplay_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    events = Event_pend(syncEvent, Event_Id_NONE, CD_ALL_EVENTS,
                        ICALL_TIMEOUT_FOREVER);

    if (events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        uint8 safeToDealloc = TRUE;

        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

          // Check for BLE stack events first
          if (pEvt->signature != 0xffff)
          {
            // Process inter-task message
            safeToDealloc = CentralDisplay_processStackMsg((ICall_Hdr *)pMsg);
          }
        }

        if (pMsg && safeToDealloc)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message
      if (events & CD_QUEUE_EVT)
      {
        cdEvt_t *pMsg;
        while (pMsg = (cdEvt_t *)Util_dequeueMsg(appMsgQueue))
        {
          // Process message
          CentralDisplay_processAppMsg(pMsg);

          // Free the space from the message
          ICall_free(pMsg);
        }
      }
    }
  }
}

/*********************************************************************
 * @fn      CentralDisplay_processStackMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t CentralDisplay_processStackMsg(ICall_Hdr *pMsg)
{
  uint8_t safeToDealloc = TRUE;

  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:
      CentralDisplay_processGapMsg((gapEventHdr_t*) pMsg);
      break;

    case GATT_MSG_EVENT:
      CentralDisplay_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    case HCI_GAP_EVENT_EVENT:
    {
      // Process HCI message
      switch (pMsg->status)
      {
        case HCI_COMMAND_COMPLETE_EVENT_CODE:
          CentralDisplay_processCmdCompleteEvt((hciEvt_CmdComplete_t *) pMsg);
          break;

        case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
          AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
          break;

        // HCI Commands Events
        case HCI_COMMAND_STATUS_EVENT_CODE:
          {
            hciEvt_CommandStatus_t *pMyMsg = (hciEvt_CommandStatus_t *)pMsg;
            switch ( pMyMsg->cmdOpcode )
            {
              case HCI_LE_SET_PHY:
                {
                  if (pMyMsg->cmdStatus ==
                      HCI_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE)
                  {
                      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3,
                                           "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                                           "PHY Change failure, peer does not support this");
                  }
                  else
                  {
                      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3,
                                           "PHY Update Status Event: 0x%x", pMyMsg->cmdStatus);
                  }
                }
                break;
              case HCI_DISCONNECT:
                break;

              default:
                {
                  CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3,
                                       "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                                       "Unknown Cmd Status: 0x%04x::0x%02x",
                                       pMyMsg->cmdOpcode, pMyMsg->cmdStatus);
                }
              break;
            }
          }
          break;

        // LE Events
        case HCI_LE_EVENT_CODE:
        {
          hciEvt_BLEPhyUpdateComplete_t *pPUC
            = (hciEvt_BLEPhyUpdateComplete_t*) pMsg;

          if (pPUC->BLEEventCode == HCI_BLE_PHY_UPDATE_COMPLETE_EVENT)
          {
            if (pPUC->status != SUCCESS)
            {
              CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3,
                                   "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                                   "%s: PHY change failure",
                                   CentralDisplay_getConnAddrStr(pPUC->connHandle));
            }
            else
            {
              // Only symmetrical PHY is supported.
              // rxPhy should be equal to txPhy.
              CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3, "PHY Updated to %s",
                                   (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_1M) ? "1M" :
                                   (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_2M) ? "2M" :
                                   (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_CODED) ? "CODED" : "Unexpected PHY Value");
            }
          }

          break;
        }

        default:
          break;
      }

      break;
    }

    case L2CAP_SIGNAL_EVENT:
      // place holder for L2CAP Connection Parameter Reply
      break;

    default:
      break;
  }

  return (safeToDealloc);
}

/*********************************************************************
 * @fn      CentralDisplay_processAppMsg
 *
 * @brief   Scanner application event processing function.
 *
 * @param   pMsg - pointer to event structure
 *
 * @return  none
 */
static void CentralDisplay_processAppMsg(cdEvt_t *pMsg)
{
  bool safeToDealloc = TRUE;

  switch (pMsg->hdr.event)
  {
    case CD_EVT_ADV_REPORT:
    {
      GapScan_Evt_AdvRpt_t* pAdvRpt = (GapScan_Evt_AdvRpt_t*) (pMsg->pData);
      //Auto connect is enabled
      if (autoConnect)
      {
          // Check if already connecting to a device
          if(connHandleInProgress == CONN_NOT_IN_PROGRESS)
          {
              //Check if advertiser is part of the group
              if (CentralDisplay_isMember(pAdvRpt->pData , acGroup, GROUP_NAME_LENGTH))
              {
                  GapScan_disable();
                  uint8_t currentScanPhy;
                  GapScan_getParam(SCAN_PARAM_PRIM_PHYS, &currentScanPhy, NULL);
                  manFacIdInProgress = CentralDisplay_findManFacID(pAdvRpt->addr, pAdvRpt->dataLen, pAdvRpt->pData);
                  CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus, "AutoConnect - Found Device %s", Util_convertBdAddr2Str(pAdvRpt->addr));
                  GapInit_connect(pAdvRpt->addrType & MASK_ADDRTYPE_ID,
                                  pAdvRpt->addr, currentScanPhy, 0);
              }
          }
      }
      else
      {
#if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
          if (CentralDisplay_findSvcUuid(SVC_UUID_FILTER, pAdvRpt->pData, pAdvRpt->dataLen))
          {
              CentralDisplay_addScanInfo(pAdvRpt->addr, pAdvRpt->addrType, pAdvRpt->dataLen, pAdvRpt->pData);
              CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineData, "Discovered: %s",
                                   Util_convertBdAddr2Str(pAdvRpt->addr));
          }
#else
          CentralDisplay_addScanInfo(pAdvRpt->addr, pAdvRpt->addrType, pAdvRpt->dataLen, pAdvRpt->pData);
          CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineData, "Discovered: %s",
                               Util_convertBdAddr2Str(pAdvRpt->addr));
#endif
      }

      // Free report payload data
      if (pAdvRpt->pData != NULL)
      {
        ICall_free(pAdvRpt->pData);
      }
      break;
    }

    case CD_EVT_SCAN_ENABLED:
    {
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo2,
                             "[" CUI_COLOR_CYAN "Auto Connect" CUI_COLOR_RESET "] %s "
                             "[" CUI_COLOR_CYAN "Scan" CUI_COLOR_RESET "] "  CUI_COLOR_GREEN "On" CUI_COLOR_RESET,
                             (autoConnect == AUTOCONNECT_DISABLE) ? "Disabled" :
                             (autoConnect == AUTOCONNECT_GROUP_A) ? "Group A" :
                             (autoConnect == AUTOCONNECT_GROUP_B) ? "Group B" : "Unexpected Auto Connect Value");

        if(!autoConnect)
        {
            CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus, "Discovering..");
        }

        break;
    }

    case CD_EVT_SCAN_DISABLED:
    {
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo2,
                             "[" CUI_COLOR_CYAN "Auto Connect" CUI_COLOR_RESET "] %s "
                             "[" CUI_COLOR_CYAN "Scan" CUI_COLOR_RESET "] "  CUI_COLOR_RED "Off" CUI_COLOR_RESET,
                             (autoConnect == AUTOCONNECT_DISABLE) ? "Disabled" :
                             (autoConnect == AUTOCONNECT_GROUP_A) ? "Group A" :
                             (autoConnect == AUTOCONNECT_GROUP_B) ? "Group B" : "Unexpected Auto Connect Value");

        // Set application state based on connection status
        if(numConn > 0)
        {
            DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_CONNECTED);
        }
        else
        {
            DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_IDLE);
        }

        if(!autoConnect)
        {
            CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus, "%d devices discovered", numScanRes);
        }
        break;
    }
    case CD_EVT_SVC_DISC:
        CentralDisplay_startSvcDiscovery();
        break;

    case CD_EVT_TGL_NOTIF:
        CentralDisplay_toggleNotifications();
        break;

    case CD_EVT_READ_RSSI:
    {
      uint8_t connIndex = pMsg->hdr.state;
      uint16_t connHandle = connList[connIndex].connHandle;

      // If link is still valid
      if (connHandle != LINKDB_CONNHANDLE_INVALID)
      {
        // Restart timer
        Util_startClock(connList[connIndex].pRssiClock);

        // Read RSSI
        VOID HCI_ReadRssiCmd(connHandle);
      }

      break;
    }

    // Pairing event
    case CD_EVT_PAIR_STATE:
    {
      CentralDisplay_processPairState(pMsg->hdr.state,
                                     (cdPairStateData_t*) (pMsg->pData));
      break;
    }

    // Passcode event
    case CD_EVT_PASSCODE_NEEDED:
    {
      CentralDisplay_processPasscode((cdPasscodeData_t *)(pMsg->pData));
      break;
    }

    case CD_EVT_READ_RPA:
    {
      uint8_t* pRpaNew;

      // Read the current RPA.
      pRpaNew = GAP_GetDevAddress(FALSE);

      if (memcmp(pRpaNew, rpa, B_ADDR_LEN))
      {
        uint8_t* pDevNew;
        pDevNew = GAP_GetDevAddress(TRUE);
        // If the RPA has changed, update the display
        memcpy(rpa, pRpaNew, B_ADDR_LEN);
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineDeviceInfo,
                             "[" CUI_COLOR_CYAN "%s" CUI_COLOR_RESET "] %s "
                             "[" CUI_COLOR_CYAN "RP Addr" CUI_COLOR_RESET "] %s",
                             (addrMode <= ADDRMODE_RANDOM) ? "Dev" : "ID", Util_convertBdAddr2Str(pDevNew),
                             Util_convertBdAddr2Str(rpa));
      }
      break;
    }

    // Insufficient memory
    case CD_EVT_INSUFFICIENT_MEM:
    {
      // We are running out of memory.
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                           "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                           "Insufficient Memory");

      // We might be in the middle of scanning, try stopping it.
      GapScan_disable();
      break;
    }

    case CD_UI_INPUT_EVT:
    {
        CUI_processMenuUpdate();
        break;
    }

    default:
      // Do nothing.
      break;
  }

  if ((safeToDealloc == TRUE) && (pMsg->pData != NULL))
  {
    ICall_free(pMsg->pData);
  }
}

/*********************************************************************
 * @fn      CentralDisplay_processGapMsg
 *
 * @brief   GAP message processing function.
 *
 * @param   pMsg - pointer to event message structure
 *
 * @return  none
 */
static void CentralDisplay_processGapMsg(gapEventHdr_t *pMsg)
{
  switch (pMsg->opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
    {
      uint8_t temp8;
      uint16_t temp16;
      gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

      // Setup scanning
      // For more information, see the GAP section in the User's Guide:
      // http://software-dl.ti.com/lprf/ble5stack-latest/

      // Register callback to process Scanner events
      GapScan_registerCb(CentralDisplay_scanCb, NULL);

      // Set Scanner Event Mask
      GapScan_setEventMask(GAP_EVT_SCAN_ENABLED | GAP_EVT_SCAN_DISABLED |
                           GAP_EVT_ADV_REPORT);

      // Set Scan PHY parameters
      GapScan_setPhyParams(SCAN_PRIM_PHY_1M, DEFAULT_SCAN_TYPE,
                           DEFAULT_SCAN_INTERVAL, DEFAULT_SCAN_WINDOW);
      GapScan_setPhyParams(SCAN_PRIM_PHY_CODED, DEFAULT_SCAN_TYPE,
                           DEFAULT_SCAN_INTERVAL, DEFAULT_SCAN_WINDOW);

      // Set Advertising report fields to keep
      temp16 = ADV_RPT_FIELDS;
      GapScan_setParam(SCAN_PARAM_RPT_FIELDS, &temp16);
      // Set Scanning Primary PHY
      temp8 = DEFAULT_SCAN_PHY;
      GapScan_setParam(SCAN_PARAM_PRIM_PHYS, &temp8);
      // Set LL Duplicate Filter
      temp8 = SCAN_FLT_DUP_ENABLE;
      GapScan_setParam(SCAN_PARAM_FLT_DUP, &temp8);

      // Set PDU type filter -
      // Only 'Connectable' and 'Complete' packets are desired.
      // It doesn't matter if received packets are
      // whether Scannable or Non-Scannable, whether Directed or Undirected,
      // whether Scan_Rsp's or Advertisements, and whether Legacy or Extended.
      temp16 = SCAN_FLT_PDU_CONNECTABLE_ONLY | SCAN_FLT_PDU_COMPLETE_ONLY;
      GapScan_setParam(SCAN_PARAM_FLT_PDU_TYPE, &temp16);

      // Set initiating PHY parameters
      GapInit_setPhyParam(DEFAULT_INIT_PHY, INIT_PHYPARAM_CONN_INT_MIN,
                          INIT_PHYPARAM_MIN_CONN_INT);
      GapInit_setPhyParam(DEFAULT_INIT_PHY, INIT_PHYPARAM_CONN_INT_MAX,
                          INIT_PHYPARAM_MAX_CONN_INT);

      cdMaxPduSize = pPkt->dataPktLen;

      // Print Initial Status lines
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo1,
                           "[" CUI_COLOR_CYAN "Role" CUI_COLOR_RESET "] Central "
                           "[" CUI_COLOR_CYAN "Num Conns" CUI_COLOR_RESET "] %d/%d",
                           numConn, MAX_NUM_BLE_CONNS);
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo2,
                           "[" CUI_COLOR_CYAN "Auto Connect" CUI_COLOR_RESET "] %s "
                           "[" CUI_COLOR_CYAN "Scan" CUI_COLOR_RESET "] "  CUI_COLOR_RED "Off" CUI_COLOR_RESET,
                           (autoConnect == AUTOCONNECT_DISABLE) ? "Disabled" :
                           (autoConnect == AUTOCONNECT_GROUP_A) ? "Group A" :
                           (autoConnect == AUTOCONNECT_GROUP_B) ? "Group B" : "Unexpected Auto Connect Value");

      if (addrMode > ADDRMODE_RANDOM)
      {
        // Update the current RPA.
        memcpy(rpa, GAP_GetDevAddress(FALSE), B_ADDR_LEN);

        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineDeviceInfo,
                             "[" CUI_COLOR_CYAN "%s" CUI_COLOR_RESET "] %s "
                             "[" CUI_COLOR_CYAN "RP Addr" CUI_COLOR_RESET "] %s",
                             (addrMode <= ADDRMODE_RANDOM) ? "Dev" : "ID", Util_convertBdAddr2Str(pPkt->devAddr),
                                     Util_convertBdAddr2Str(rpa));

        // Create one-shot clock for RPA check event.
        Util_constructClock(&clkRpaRead, CentralDisplay_clockHandler,
                            READ_RPA_PERIOD, 0, true, CD_EVT_READ_RPA);
      }
      else
      {
          // Display device address
          CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineDeviceInfo,
                               "[" CUI_COLOR_CYAN "%s" CUI_COLOR_RESET "] %s",
                               (addrMode <= ADDRMODE_RANDOM) ? "Dev" : "ID", Util_convertBdAddr2Str(pPkt->devAddr));
      }
      break;
    }

    case GAP_CONNECTING_CANCELLED_EVENT:
    {
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3,
                           "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                           "Connection attempt cancelled");
      if(autoConnect)
      {
          GapScan_enable(0,0,0);
      }
      break;
    }

    case GAP_LINK_ESTABLISHED_EVENT:
    {
      uint16_t connHandle = ((gapEstLinkReqEvent_t*) pMsg)->connectionHandle;
      uint8_t* pAddr = ((gapEstLinkReqEvent_t*) pMsg)->devAddr;
      uint8_t  connIndex;
      uint8_t* pStrAddr;
      uint8_t pairMode = 0;

      // Add this connection info to the list
      connIndex = CentralDisplay_addConnInfo(connHandle, pAddr);

      // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
      CENTRALDISPLAY_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

      // Set manufacture ID for connection
      if(autoConnect)
      {
          // Store connHandle in case link terminated prior to post connection set up
          connHandleInProgress = connHandle;
          connList[connIndex].manFacID = manFacIdInProgress;
      }
      else
      {
          connList[connIndex].manFacID = scanList[scanListIndex].manFacID;
      }

      pStrAddr = (uint8_t*) Util_convertBdAddr2Str(connList[connIndex].addr);

      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3, "Connected to %s", pStrAddr);
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo1,
                           "[" CUI_COLOR_CYAN "Role" CUI_COLOR_RESET "] Central "
                           "[" CUI_COLOR_CYAN "Num Conns" CUI_COLOR_RESET "] %d/%d",
                           numConn, MAX_NUM_BLE_CONNS);

      // Set work with connection handle for new connection
      CentralDisplay_CancelRssi(cdConnHandle);
      cdConnHandle = connHandle;
      CentralDisplay_StartRssi();

      // Start Service Discovery
      CentralDisplay_enqueueMsg(CD_EVT_SVC_DISC, 0, NULL);

      GAPBondMgr_GetParameter(GAPBOND_PAIRING_MODE, &pairMode);
      break;
    }

    case GAP_LINK_TERMINATED_EVENT:
    {
      uint8_t connIndex;
      uint8_t* pStrAddr;
      uint16_t connHandle = ((gapTerminateLinkEvent_t*) pMsg)->connectionHandle;
      uint8_t deleteAddr[B_ADDR_LEN] = {0};

      // Cancel timers
      CentralDisplay_CancelRssi(connHandle);

      // Mark this connection deleted in the connected device list.
      connIndex = CentralDisplay_removeConnInfo(connHandle);

      // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
      CENTRALDISPLAY_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

      // Copy BLE ADDR then remove it from connection info
      pStrAddr = (uint8_t*) Util_convertBdAddr2Str(connList[connIndex].addr);
      memcpy(connList[connIndex].addr, deleteAddr, B_ADDR_LEN);

      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3,
                           "[" CUI_COLOR_YELLOW "Warning" CUI_COLOR_RESET "] "
                           "%s is disconnected", pStrAddr);
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo1,
                           "[" CUI_COLOR_CYAN "Role" CUI_COLOR_RESET "] Central "
                           "[" CUI_COLOR_CYAN "Num Conns" CUI_COLOR_RESET "] %d/%d",
                           numConn, MAX_NUM_BLE_CONNS);

      if (autoConnect)
      {
          // Current connection disconnected prior to completing setup
          if (connHandle == connHandleInProgress)
          {
              connHandleInProgress = CONN_NOT_IN_PROGRESS;
              GapScan_enable(0,0,0);
          }
          // No longer at max number connections && post connection setup is not in progress
          else if ((numConn == (MAX_NUM_BLE_CONNS - 1)) && (connHandleInProgress == CONN_NOT_IN_PROGRESS))
          {
              GapScan_enable(0,0,0);
          }
      }

      break;
    }

    case GAP_UPDATE_LINK_PARAM_REQ_EVENT:
    {
      gapUpdateLinkParamReqReply_t rsp;
      gapUpdateLinkParamReq_t *pReq;

      pReq = &((gapUpdateLinkParamReqEvent_t *)pMsg)->req;

      rsp.connectionHandle = pReq->connectionHandle;
      rsp.signalIdentifier = pReq->signalIdentifier;

      if (acceptParamUpdateReq)
      {
        rsp.intervalMin = pReq->intervalMin;
        rsp.intervalMax = pReq->intervalMax;
        rsp.connLatency = pReq->connLatency;
        rsp.connTimeout = pReq->connTimeout;
        rsp.accepted = TRUE;
      }
      else
      {
        // Reject the request.
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

      if (linkDB_GetInfo(pPkt->connectionHandle, &linkInfo) ==  SUCCESS)
      {
        if(pPkt->status == SUCCESS)
        {
          CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3, "Updated: %s, connTimeout:%d",
                               Util_convertBdAddr2Str(linkInfo.addr),
                               linkInfo.connTimeout*CONN_TIMEOUT_MS_CONVERSION);
        }
        else
        {
          // Display the address of the connection update failure
          CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3,
                               "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                               "Link Param Update Failed 0x%x %s",
                               pPkt->opcode, Util_convertBdAddr2Str(linkInfo.addr));
        }
      }

      break;
    }

    default:
      break;
  }
}

/*********************************************************************
 * @fn      CentralDisplay_processGATTMsg
 *
 * @brief   Process GATT messages and events.
 *
 * @return  none
 */
static void CentralDisplay_processGATTMsg(gattMsgEvent_t *pMsg)
{
  if (linkDB_Up(pMsg->connHandle))
  {
    // See if GATT server was unable to transmit an ATT response
    if (pMsg->hdr.status == blePending)
    {
      // No HCI buffer was available. App can try to retransmit the response
      // on the next connection event. Drop it for now.
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineData,
                           "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                           "ATT Rsp dropped %d", pMsg->method);
    }
    else if (pMsg->method == ATT_HANDLE_VALUE_NOTI)
    {
        Smsgs_bleSensorField_t forwardInfo;
        uint8_t connIndex;
        char notificationString[pMsg->msg.handleValueNoti.len+1];
        char *stringPtr = notificationString;
        uint8_t i;

        // Get index in connect list & transfer info to forwardInfo
        connIndex = CentralDisplay_getConnIndex(pMsg->connHandle);
        memcpy(forwardInfo.bleAddr, connList[connIndex].addr, B_ADDR_LEN);
        forwardInfo.manFacID = connList[connIndex].manFacID;
        forwardInfo.dataLength = pMsg->msg.handleValueNoti.len;

        // Find UUID matching char handle
        forwardInfo.uuid = 0;
        for(i=0; i < connList[connIndex].noCharCCCDHdl; i++)
        {
            if((connList[connIndex].charCCCDHdl[i] - 1) == pMsg->msg.handleValueNoti.handle)
            {
                forwardInfo.uuid = connList[connIndex].charCCCDUUID[i];
                break;
            }
        }

        // Convert Data to string for printing to UART
        for(i = 0; i < pMsg->msg.handleValueNoti.len && i < MAX_BLE_DATA_LEN; i++)
        {
            stringPtr += sprintf(stringPtr, "%02x", pMsg->msg.handleValueNoti.pValue[i]);
            forwardInfo.data[i] = pMsg->msg.handleValueNoti.pValue[i];
        }

        // Display Notification Data & Forward to 15.4 Collector
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineData,
                             "[" CUI_COLOR_CYAN "Msg Type" CUI_COLOR_RESET "] Notif "
                             "[" CUI_COLOR_CYAN "Addr" CUI_COLOR_RESET "] %s "
                             "[" CUI_COLOR_CYAN "UUID" CUI_COLOR_RESET "] 0x%4x "
                             "[" CUI_COLOR_CYAN "Val" CUI_COLOR_RESET "] 0x%s",
                             Util_convertBdAddr2Str(forwardInfo.bleAddr), forwardInfo.uuid, notificationString);

        Sensor_forwardBleData(&forwardInfo);
    }
    else if ((pMsg->method == ATT_READ_RSP)   ||
             ((pMsg->method == ATT_ERROR_RSP) &&
              (pMsg->msg.errorRsp.reqOpcode == ATT_READ_REQ)))
    {
      if (pMsg->method == ATT_ERROR_RSP)
      {
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineData,
                             "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                             "Read Error %d", pMsg->msg.errorRsp.errCode);
      }
      else
      {
        // After a successful read, display the read value
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineData,
                             "[" CUI_COLOR_CYAN "Msg Type" CUI_COLOR_RESET "] Read Rsp "
                             "[" CUI_COLOR_CYAN "Val" CUI_COLOR_RESET "] 0x%02x",
                             pMsg->msg.readRsp.pValue[0]);
      }
    }
    else if ((pMsg->method == ATT_WRITE_RSP)  ||
             ((pMsg->method == ATT_ERROR_RSP) &&
              (pMsg->msg.errorRsp.reqOpcode == ATT_WRITE_REQ)))
    {
      if (pMsg->method == ATT_ERROR_RSP)
      {
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineData,
                             "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                             "Write Error %d", pMsg->msg.errorRsp.errCode);
      }
      else
      {
        // After a successful write, display the value that was written and
        // increment value
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineData,
                             "[" CUI_COLOR_CYAN "Msg Type" CUI_COLOR_RESET "] Write Sent "
                             "[" CUI_COLOR_CYAN "Val" CUI_COLOR_RESET "] 0x%02x",
                             charVal);
      }

    }
    else if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
    {
      // ATT request-response or indication-confirmation flow control is
      // violated. All subsequent ATT requests or indications will be dropped.
      // The app is informed in case it wants to drop the connection.

      // Display the opcode of the message that caused the violation.
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineData,
                           "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                           "FC Violated: %d", pMsg->msg.flowCtrlEvt.opcode);
    }
    else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
    {
      // MTU size updated
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3, "MTU Size: %d", pMsg->msg.mtuEvt.MTU);
    }
    else if (discState != BLE_DISC_STATE_IDLE)
    {
      CentralDisplay_processGATTDiscEvent(pMsg);
    }
  } // else - in case a GATT message came after a connection has dropped, ignore it.

  // Needed only for ATT Protocol messages
  GATT_bm_free(&pMsg->msg, pMsg->method);
}

/*********************************************************************
 * @fn      CentralDisplay_processCmdCompleteEvt
 *
 * @brief   Process an incoming OSAL HCI Command Complete Event.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void CentralDisplay_processCmdCompleteEvt(hciEvt_CmdComplete_t *pMsg)
{
  switch (pMsg->cmdOpcode)
  {
    case HCI_READ_RSSI:
    {
      int8 rssi = (int8)pMsg->pReturnParam[3];

      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineWorkWith,
                           "[" CUI_COLOR_CYAN "Addr" CUI_COLOR_RESET "] %s "
                           "[" CUI_COLOR_CYAN "RSSI" CUI_COLOR_RESET "] %d dBm",
                           CentralDisplay_getConnAddrStr(cdConnHandle), rssi);
      break;
    }
    default:
      break;
  }
}

/*********************************************************************
 * @fn      CentralDisplay_StartRssi
 *
 * @brief   Start periodic RSSI reads on the current link.
 *
 * @return  SUCCESS: RSSI Read timer started
 *          bleIncorrectMode: Aready started
 *          bleNoResources: No resources
 */
static status_t CentralDisplay_StartRssi(void)
{
  uint8_t connIndex = CentralDisplay_getConnIndex(cdConnHandle);

  // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
  CENTRALDISPLAY_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  // If already running
  if (connList[connIndex].pRssiClock != NULL)
  {
    return bleIncorrectMode;
  }

  // Create a clock object and start
  connList[connIndex].pRssiClock
    = (Clock_Struct*) ICall_malloc(sizeof(Clock_Struct));

  if (connList[connIndex].pRssiClock)
  {
    Util_constructClock(connList[connIndex].pRssiClock,
                        CentralDisplay_clockHandler,
                        DEFAULT_RSSI_PERIOD, 0, true,
                        (connIndex << 8) | CD_EVT_READ_RSSI);
  }
  else
  {
    return bleNoResources;
  }

  return SUCCESS;
}

/*********************************************************************
 * @fn      CentralDisplay_CancelRssi
 *
 * @brief   Cancel periodic RSSI reads on a link.
 *
 * @param   connection handle
 *
 * @return  SUCCESS: Operation successful
 *          bleNotConnected: connHandle does not correspond to a existing connection
 *          bleIncorrectMode: Has not started
 */
static status_t CentralDisplay_CancelRssi(uint16_t connHandle)
{
  uint8_t connIndex;
  connIndex = CentralDisplay_getConnIndex(connHandle);

  // Connection has been disconnected timer canceled when processing link terminate
  if (connIndex >= MAX_NUM_BLE_CONNS)
  {
      return bleNotConnected;
  }

  // If already running
  if (connList[connIndex].pRssiClock == NULL)
  {
      return bleIncorrectMode;
  }

  // Stop timer
  Util_stopClock(connList[connIndex].pRssiClock);

  // Destroy the clock object
  Clock_destruct(connList[connIndex].pRssiClock);

  // Free clock struct
  ICall_free(connList[connIndex].pRssiClock);
  connList[connIndex].pRssiClock = NULL;

  return SUCCESS;
}

/*********************************************************************
 * @fn      CentralDisplay_processPairState
 *
 * @brief   Process the new paring state.
 *
 * @return  none
 */
static void CentralDisplay_processPairState(uint8_t state,
                                           cdPairStateData_t* pPairData)
{
  uint8_t status = pPairData->status;
  uint8_t pairMode = 0;

  if (state == GAPBOND_PAIRING_STATE_STARTED)
  {
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLinePairInfo, "Pairing started");
  }
  else if (state == GAPBOND_PAIRING_STATE_COMPLETE)
  {
    if (status == SUCCESS)
    {
      linkDBInfo_t linkInfo;

      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLinePairInfo, "Pairing success");

      if (linkDB_GetInfo(pPairData->connHandle, &linkInfo) == SUCCESS)
      {
        // If the peer was using private address, update with ID address
        if ((linkInfo.addrType == ADDRTYPE_PUBLIC_ID ||
             linkInfo.addrType == ADDRTYPE_RANDOM_ID) &&
             !Util_isBufSet(linkInfo.addrPriv, 0, B_ADDR_LEN))
        {
          // Update the address of the peer to the ID address
          CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLinePairInfo, "Addr updated: %s", Util_convertBdAddr2Str(linkInfo.addr));

          // Update the connection list with the ID address
          uint8_t i = CentralDisplay_getConnIndex(pPairData->connHandle);

          CENTRALDISPLAY_ASSERT(i < MAX_NUM_BLE_CONNS);
          memcpy(connList[i].addr, linkInfo.addr, B_ADDR_LEN);
        }
      }
    }
    else
    {
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLinePairInfo,
                           "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                           "Pairing fail: %d", status);
    }

    GAPBondMgr_GetParameter(GAPBOND_PAIRING_MODE, &pairMode);
  }
  else if (state == GAPBOND_PAIRING_STATE_ENCRYPTED)
  {
    if (status == SUCCESS)
    {
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLinePairInfo, "Encryption success");
    }
    else
    {
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLinePairInfo,
                           "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                           "Encryption failed: %d", status);
    }

    GAPBondMgr_GetParameter(GAPBOND_PAIRING_MODE, &pairMode);
  }
  else if (state == GAPBOND_PAIRING_STATE_BOND_SAVED)
  {
    if (status == SUCCESS)
    {
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLinePairInfo, "Bond save success");
    }
    else
    {
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLinePairInfo,
                           "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                           "Bond save failed: %d", status);
    }
  }
}

/*********************************************************************
 * @fn      CentralDisplay_processPasscode
 *
 * @brief   Process the Passcode request.
 *
 * @return  none
 */
static void CentralDisplay_processPasscode(cdPasscodeData_t *pData)
{
  // Display passcode to user
  if (pData->uiOutputs != 0)
  {
    CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLinePairInfo, "Passcode: %d", B_APP_DEFAULT_PASSCODE);
  }

  // Send passcode response
  GAPBondMgr_PasscodeRsp(pData->connHandle, SUCCESS, B_APP_DEFAULT_PASSCODE);
}

/*********************************************************************
 * @fn      CentralDisplay_startSvcDiscovery
 *
 * @brief   Start service discovery.
 *
 * @return  none
 */
static void CentralDisplay_startSvcDiscovery(void)
{
  CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus, "Start Service Discovery");

  attExchangeMTUReq_t req;

  discState = BLE_DISC_STATE_MTU;

  // Discover GATT Server's Rx MTU size
  req.clientRxMTU = cdMaxPduSize - L2CAP_HDR_SIZE;

  // ATT MTU size should be set to the minimum of the Client Rx MTU
  // and Server Rx MTU values
  VOID GATT_ExchangeMTU(cdConnHandle, &req, selfEntity);
}

/*********************************************************************
 * @fn      CentralDisplay_processGATTDiscEvent
 *
 * @brief   Process GATT discovery event
 *
 * @return  none
 */
static void CentralDisplay_processGATTDiscEvent(gattMsgEvent_t *pMsg)
{
    // Find index into connection list
    uint8_t connIndex;
    connIndex = CentralDisplay_getConnIndex(cdConnHandle);

    if (discState == BLE_DISC_STATE_MTU)
    {
        // Update DMMPolicy for service discovery
        DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_HIGH_BANDWIDTH);

        // MTU size response received, discover simple service
        if (pMsg->method == ATT_EXCHANGE_MTU_RSP)
        {
            // Discover characteristics
            discState = BLE_DISC_STATE_CHAR;
            GATT_DiscAllCharDescs(connList[connIndex].connHandle, 0x0001, 0xFFFF, selfEntity);
        }
    }
    else if (discState == BLE_DISC_STATE_CHAR)
    {
        if ((pMsg->method == ATT_FIND_INFO_RSP) && (pMsg->msg.findInfoRsp.numInfo > 0) )
        {
            uint8_t i;
            static uint16_t lastDscvUUID = 0;

            // For each handle/uuid pair
            for (i = 0; i < pMsg->msg.findInfoRsp.numInfo; i++)
            {
                // If CCCD store characteristic information otherwise keep track of most recent UUID
                if(ATT_BT_PAIR_UUID(pMsg->msg.findInfoRsp.pInfo, i) != GATT_CLIENT_CHAR_CFG_UUID)
                {
                    if(pMsg->msg.findInfoRsp.format == ATT_HANDLE_BT_UUID_TYPE) //16b
                    {
                        lastDscvUUID = ATT_BT_PAIR_UUID(pMsg->msg.findInfoRsp.pInfo, i);
                    }
                    else if(pMsg->msg.findInfoRsp.format == ATT_HANDLE_UUID_TYPE) //128b
                    {
                        // Save 16b UUID alias
                        uint8_t* pChUuid = pMsg->msg.findInfoRsp.pInfo + ATT_PAIR_UUID_IDX(i);
                        lastDscvUUID = BUILD_UINT16(pChUuid[12], pChUuid[13]);
                    }
                }
                else
                {
                    // CCCD found
                    if (connList[connIndex].noCharCCCDHdl < MAX_CCCD_HANDLES)
                    {
                        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus, "Found UUID: 0x%x", lastDscvUUID);
                        connList[connIndex].charCCCDHdl[connList[connIndex].noCharCCCDHdl] = ATT_BT_PAIR_HANDLE(pMsg->msg.findInfoRsp.pInfo, i);
                        connList[connIndex].charCCCDUUID[connList[connIndex].noCharCCCDHdl] = lastDscvUUID;
                        connList[connIndex].noCharCCCDHdl++;
                    }
                }
            }
        }

        // Service Discovery Completed
        if(pMsg->hdr.status == bleProcedureComplete)
        {
            // Update states & enable notifications
            DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_CONNECTED);
            discState = BLE_DISC_STATE_IDLE;
            CentralDisplay_enqueueMsg(CD_EVT_TGL_NOTIF, 0, NULL);
        }
    }
}


/*********************************************************************
 * @fn      CentralDisplay_toggleNotifications
 *
 * @brief   Subscribe to discovered notifiable characteristics
 *
 * @return  none
 */
static void CentralDisplay_toggleNotifications()
{
    attWriteReq_t req;
    uint8_t i;
    bStatus_t status;
    uint8 notificationsOn[2] = {0};
    uint8_t connIndex;

    connIndex = CentralDisplay_getConnIndex(cdConnHandle);

    if (connList[connIndex].notifEnabled == 0)
    {
        // Enable notifications
        notificationsOn[0] = charVal = 0x01;
        connList[connIndex].notifEnabled = 1;
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus, "Enabling Notifications");
    }
    else
    {
        // Disable notifications
        notificationsOn[0] = charVal = 0x00;
        connList[connIndex].notifEnabled = 0;
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus, "Disabling Notifications");
    }

    // For each notifiable characteristic
    for (i = 0; i < connList[connIndex].noCharCCCDHdl; i++)
    {
        req.pValue = GATT_bm_alloc( connList[connIndex].connHandle, ATT_WRITE_REQ, 2, NULL );
        if ( req.pValue != NULL )
        {
            req.handle = connList[connIndex].charCCCDHdl[i];
            req.len = 2;
            memcpy(req.pValue, notificationsOn, 2);
            req.sig = 0;
            req.cmd = 0;

            do
            {
                // The GATT layer processes ATT messages and sends it to the stack.
                // If a new message is queued during this time, blePending is returned.
                status = GATT_WriteCharValue( connList[connIndex].connHandle, &req, selfEntity );
            } while (status == blePending);

            if ( status != SUCCESS )
            {
                GATT_bm_free( (gattMsg_t *)&req, ATT_WRITE_REQ );

                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineData,
                                     "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                                     "Error: Write Failed %d Status %d",
                                     connList[connIndex].charCCCDHdl[i], status);
            }
        }
    }
    // Post connection setup is complete
    if(autoConnect)
    {
        connHandleInProgress = CONN_NOT_IN_PROGRESS;
        // Disable Scan when max connections reached
        if(numConn < MAX_NUM_BLE_CONNS)
        {
            GapScan_enable(0,0,0);
        }
    }
}

#if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
/*********************************************************************
 * @fn      CentralDisplay_findSvcUuid
 *
 * @brief   Find a given UUID in an advertiser's service UUID list.
 *
 * @return  TRUE if service UUID found
 */
static bool CentralDisplay_findSvcUuid(uint16_t uuid, uint8_t *pData,
                                      uint16_t dataLen)
{
  uint8_t adLen;
  uint8_t adType;
  uint8_t *pEnd;

  if (dataLen > 0)
  {
    pEnd = pData + dataLen - 1;

    // While end of data not reached
    while (pData < pEnd)
    {
      // Get length of next AD item
      adLen = *pData++;
      if (adLen > 0)
      {
        adType = *pData;

        // If AD type is for 16-bit service UUID
        if ((adType == GAP_ADTYPE_16BIT_MORE) ||
            (adType == GAP_ADTYPE_16BIT_COMPLETE))
        {
          pData++;
          adLen--;

          // For each UUID in list
          while (adLen >= 2 && pData < pEnd)
          {
            // Check for match
            if ((pData[0] == LO_UINT16(uuid)) && (pData[1] == HI_UINT16(uuid)))
            {
              // Match found
              return TRUE;
            }

            // Go to next
            pData += 2;
            adLen -= 2;
          }

          // Handle possible erroneous extra byte in UUID list
          if (adLen == 1)
          {
            pData++;
          }
        }
        else
        {
          // Go to next item
          pData += adLen;
        }
      }
    }
  }

  // Match not found
  return FALSE;
}
#endif // DEFAULT_DEV_DISC_BY_SVC_UUID

/*********************************************************************
 * @fn      CentralDisplay_findManFacID
 *
 * @brief   Find Manufacturer ID within Advertisement Data
 *
 * @return  Manufacturer ID
 */
static uint16_t CentralDisplay_findManFacID(uint8_t *pAddr, uint16_t dataLen, uint8_t *pData)
{
    // Add Manufacturer ID to scan result list
    uint8_t length;
    uint8_t i = 0;
    uint16_t manFacID = 0;

    // Advertisement Data Structure 1st Byte: Length, 2nd Byte: AD Type, AD Data
    while(i < dataLen)
    {
        length = pData[i];

        // If Manufacturer ID, save 2 byte value
        if((pData[i+1] == GAP_ADTYPE_MANUFACTURER_SPECIFIC) && (length >= 3))
        {
            manFacID = BUILD_UINT16(pData[i+3], pData[i+2]);
            break;
        }
        else
        {
            // Increment index to next AD Structure
            i += (length + 1);
        }
    }

    return manFacID;
}

/*********************************************************************
 * @fn      CentralDisplay_addScanInfo
 *
 * @brief   Add a device to the scanned device list
 *
 * @return  none
 */
static void CentralDisplay_addScanInfo(uint8_t *pAddr, uint8_t addrType, uint16_t dataLen, uint8_t *pData)
{
  uint8_t i;

  // If result count not at max
  if (numScanRes < DEFAULT_MAX_SCAN_RES)
  {
    // Check if device is already in scan results
    for (i = 0; i < numScanRes; i++)
    {
      if (memcmp(pAddr, scanList[i].addr , B_ADDR_LEN) == 0)
      {
        return;
      }
    }

    // Add addr to scan result list
    memcpy(scanList[numScanRes].addr, pAddr, B_ADDR_LEN);
    scanList[numScanRes].addrType = addrType;

    // Check if ManFacID is in advertisement Data
    scanList[numScanRes].manFacID = CentralDisplay_findManFacID(pAddr, dataLen, pData);

    // Increment scan result count
    numScanRes++;
  }
}

/*********************************************************************
 * @fn      CentralDisplay_addConnInfo
 *
 * @brief   Add a device to the connected device list
 *
 * @return  index of the connected device list entry where the new connection
 *          info is put in.
 *          if there is no room, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t CentralDisplay_addConnInfo(uint16_t connHandle, uint8_t *pAddr)
{
  uint8_t i;

  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == LINKDB_CONNHANDLE_INVALID)
    {
      // Found available entry to put a new connection info in
      connList[i].connHandle = connHandle;
      memcpy(connList[i].addr, pAddr, B_ADDR_LEN);
      connList[i].notifEnabled = 0;
      numConn++;
      break;
    }
  }

  return i;
}

/*********************************************************************
 * @fn      CentralDisplay_removeConnInfo
 *
 * @brief   Remove a device from the connected device list
 *
 * @return  index of the connected device list entry where the new connection
 *          info is removed from.
 *          if connHandle is not found, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t CentralDisplay_removeConnInfo(uint16_t connHandle)
{
  uint8_t i;

  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == connHandle)
    {
      // Found the entry to mark as deleted
      connList[i].connHandle = LINKDB_CONNHANDLE_INVALID;
      connList[i].noCharCCCDHdl = 0;
      connList[i].manFacID = 0;
      numConn--;

      break;
    }
  }

  return i;
}

/*********************************************************************
 * @fn      CentralDisplay_getConnIndex
 *
 * @brief   Find index in the connected device list by connHandle
 *
 * @return  the index of the entry that has the given connection handle.
 *          if there is no match, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t CentralDisplay_getConnIndex(uint16_t connHandle)
{
  uint8_t i;

  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == connHandle)
    {
      break;
    }
  }

  return i;
}

/*********************************************************************
 * @fn      CentralDisplay_getConnAddrStr
 *
 * @brief   Return, in string form, the address of the peer associated with
 *          the connHandle.
 *
 * @return  A null-terminated string of the address.
 *          if there is no match, NULL will be returned.
 */
static char* CentralDisplay_getConnAddrStr(uint16_t connHandle)
{
  uint8_t i;

  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == connHandle)
    {
      return Util_convertBdAddr2Str(connList[i].addr);
    }
  }

  return NULL;
}

/*********************************************************************
 * @fn      CentralDisplay_pairStateCb
 *
 * @brief   Pairing state callback.
 *
 * @return  none
 */
static void CentralDisplay_pairStateCb(uint16_t connHandle, uint8_t state,
                                      uint8_t status)
{
  cdPairStateData_t *pData;

  // Allocate space for the event data.
  if ((pData = ICall_malloc(sizeof(cdPairStateData_t))))
  {
    pData->connHandle = connHandle;
    pData->status = status;

    // Queue the event.
    if(CentralDisplay_enqueueMsg(CD_EVT_PAIR_STATE, state, (uint8_t*) pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
* @fn      CentralDisplay_passcodeCb
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
static void CentralDisplay_passcodeCb(uint8_t *deviceAddr, uint16_t connHandle,
                                  uint8_t uiInputs, uint8_t uiOutputs,
                                  uint32_t numComparison)
{
  cdPasscodeData_t *pData = ICall_malloc(sizeof(cdPasscodeData_t));

  // Allocate space for the passcode event.
  if (pData)
  {
    pData->connHandle = connHandle;
    memcpy(pData->deviceAddr, deviceAddr, B_ADDR_LEN);
    pData->uiInputs = uiInputs;
    pData->uiOutputs = uiOutputs;
    pData->numComparison = numComparison;

    // Enqueue the event.
    if (CentralDisplay_enqueueMsg(CD_EVT_PASSCODE_NEEDED, 0,(uint8_t *) pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      CentralDisplay_clockHandler
 *
 * @brief   clock handler function
 *
 * @param   arg - argument from the clock initiator
 *
 * @return  none
 */
void CentralDisplay_clockHandler(UArg arg)
{
  uint8_t evtId = (uint8_t) (arg & 0xFF);

  switch (evtId)
  {
    case CD_EVT_READ_RSSI:
      CentralDisplay_enqueueMsg(CD_EVT_READ_RSSI, (uint8_t) (arg >> 8) , NULL);
      break;

    case CD_EVT_READ_RPA:
      // Restart timer
      Util_startClock(&clkRpaRead);
      // Let the application handle the event
      CentralDisplay_enqueueMsg(CD_EVT_READ_RPA, 0, NULL);
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      CentralDisplay_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 * @param   pData - message data pointer.
 *
 * @return  TRUE or FALSE
 */
static status_t CentralDisplay_enqueueMsg(uint8_t event, uint8_t state,
                                           uint8_t *pData)
{
  uint8_t success;
  cdEvt_t *pMsg = ICall_malloc(sizeof(cdEvt_t));

  // Create dynamic pointer to message.
  if (pMsg)
  {
    pMsg->hdr.event = event;
    pMsg->hdr.state = state;
    pMsg->pData = pData;

    // Enqueue the message.
    success = Util_enqueueMsg(appMsgQueue, syncEvent, (uint8_t *)pMsg);
    return (success) ? SUCCESS : FAILURE;
  }

  return(bleMemAllocError);
}

/*********************************************************************
 * @fn      CentralDisplay_scanCb
 *
 * @brief   Callback called by GapScan module
 *
 * @param   evt - event
 * @param   msg - message coming with the event
 * @param   arg - user argument
 *
 * @return  none
 */
void CentralDisplay_scanCb(uint32_t evt, void* pMsg, uintptr_t arg)
{
  uint8_t event;

  if (evt & GAP_EVT_ADV_REPORT)
  {
    event = CD_EVT_ADV_REPORT;
  }
  else if (evt & GAP_EVT_SCAN_ENABLED)
  {
    event = CD_EVT_SCAN_ENABLED;
  }
  else if (evt & GAP_EVT_SCAN_DISABLED)
  {
    event = CD_EVT_SCAN_DISABLED;
  }
  else if (evt & GAP_EVT_INSUFFICIENT_MEMORY)
  {
    event = CD_EVT_INSUFFICIENT_MEM;
  }
  else
  {
    return;
  }

  if(CentralDisplay_enqueueMsg(event, SUCCESS, pMsg) != SUCCESS)
  {
    ICall_free(pMsg);
  }
}

/*********************************************************************
 * @fn      CentralDisplay_doAutoConnect
 *
 * @brief   Enable/Disable AutoConnect.
 *
 * @param   index - 0 : Disable AutoConnect
 *                  1 : Enable Group A
 *                  2 : Enable Group B
 *
 * @return  always true
 */
static void CentralDisplay_doAutoConnect(const uint32_t _listIndex, char* _lines[3], bool _selected)
{
    static char* autoConnectStrings[] = {"     Disabled",
                                         "     Group A",
                                         "     Group B"};

    strncpy(_lines[1], autoConnectStrings[_listIndex], strlen(autoConnectStrings[_listIndex]));

    if (_selected)
    {
        if(_listIndex == 0)
        {
            autoConnect = AUTOCONNECT_DISABLE;
            if(GapScan_disable() == bleIncorrectMode)
            {
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo2,
                                     "[" CUI_COLOR_CYAN "Auto Connect" CUI_COLOR_RESET "] Disabled "
                                     "[" CUI_COLOR_CYAN "Scan" CUI_COLOR_RESET "] "  CUI_COLOR_RED "Off" CUI_COLOR_RESET);
            }
        }
        else if (_listIndex == 1)
        {
            autoConnect = AUTOCONNECT_GROUP_A;
            acGroup[3] = 'A';
            if(numConn < MAX_NUM_BLE_CONNS)
            {
                GapScan_enable(0, 0, 0);
            }
            else
            {
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo2,
                                     "[" CUI_COLOR_CYAN "Auto Connect" CUI_COLOR_RESET "] Group A "
                                     "[" CUI_COLOR_CYAN "Scan" CUI_COLOR_RESET "] "  CUI_COLOR_RED "Off" CUI_COLOR_RESET);
            }
        }
        else if (_listIndex == 2)
        {
            autoConnect = AUTOCONNECT_GROUP_B;
            acGroup[3] = 'B';
            if(numConn < MAX_NUM_BLE_CONNS)
            {
                GapScan_enable(0, 0, 0);
            }
            else
            {
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo2,
                                     "[" CUI_COLOR_CYAN "Auto Connect" CUI_COLOR_RESET "] Group B "
                                     "[" CUI_COLOR_CYAN "Scan" CUI_COLOR_RESET "] "  CUI_COLOR_RED "Off" CUI_COLOR_RESET);
            }
        }
    }
}

/*********************************************************************
 * @fn      CentralDisplay_doSetScanPhy
 *
 * @brief   Set PHYs for scanning.
 *
 * @param   menuEntryIndex - 0: 1M PHY
 *                           1: CODED PHY (Long range)
 */
static void CentralDisplay_doSetScanPhy(const uint32_t _listIndex, char* _lines[3], bool _selected)
{
    uint8_t temp8;
    static char* phyStrings[] = {"       1M",
                                 "       Coded"};

    strncpy(_lines[1], phyStrings[_listIndex], strlen(phyStrings[_listIndex]));

    if (_selected)
    {
        if (autoConnect)
        {
            CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                                 "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                                 "Disable auto connect to change scanning phy");
        }
        else
        {
            if (_listIndex == 0)
            {
                temp8 = SCAN_PRIM_PHY_1M;
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3, "Primary Scan PHY: 1M");
            }
            else
            {
                temp8 = SCAN_PRIM_PHY_CODED;
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3, "Primary Scan PHY: Coded");
            }

            // Set scanning primary PHY
            GapScan_setParam(SCAN_PARAM_PRIM_PHYS, &temp8);
        }
    }
}

/*********************************************************************
 * @fn      CentralDisplay_doDiscoverDevices
 *
 * @brief   Enables scanning
 *
 * @param   menuEntryIndex - index of CUI menu option
 */
static void CentralDisplay_doDiscoverDevices(int32_t menuEntryIndex)
{
  // Scanning for DEFAULT_SCAN_DURATION x 10 ms.
  // The stack does not need to record advertising reports
  // since the application will filter them by Service UUID and save.
  // Reset number of scan results to 0 before starting scan
  if(autoConnect)
  {
      CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                           "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                           "Auto connect enabled, scanning in background");
  }
  else
  {
      numScanRes = 0;
      DMMPolicy_updateApplicationState(DMMPolicy_StackRole_BlePeripheral, DMMPOLICY_BLE_SCAN);
      GapScan_enable(0, DEFAULT_SCAN_DURATION, 0);
  }
}

/*********************************************************************
 * @fn      CentralDisplay_doEstablishLink
 *
 * @brief   Establish a link to a peer device
 *
 * @param   index - item index from the menu
 *
 */
static void CentralDisplay_doConnect(const uint32_t _listIndex, char* _lines[3], bool _selected)
{
    if(numScanRes <= _listIndex)
    {
        memcpy(_lines[1], "0x000000000000", CD_ADDR_STR_SIZE);
    }
    else
    {
        memcpy(_lines[1], Util_convertBdAddr2Str(scanList[_listIndex].addr), CD_ADDR_STR_SIZE);
    }

    if (_selected)
    {
        if (autoConnect)
        {
            CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                                 "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                                 "Disable auto connect to manually connect to devices");
        }
        else
        {
            if (numConn < MAX_NUM_BLE_CONNS)
            {
                if(numScanRes <= _listIndex)
                {
                    CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                                         "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                                         "Choose a valid BLE ADDR");
                }
                else
                {
                    uint8_t currentScanPhy;
                    GapScan_getParam(SCAN_PARAM_PRIM_PHYS, &currentScanPhy, NULL);
                    scanListIndex = _listIndex;
                    GapInit_connect(scanList[_listIndex].addrType & MASK_ADDRTYPE_ID,
                                    scanList[_listIndex].addr, currentScanPhy, CONNECTION_TIMEOUT);

                    CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3, "Connecting...");
                }
            }
            else
            {
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                                     "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                                     "Maximum number of connections already established");
            }
        }
    }
}

/*********************************************************************
 * @fn      CentralDisplay_doSelectConn
 *
 * @brief   Select a connection to communicate with
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
static void CentralDisplay_doSelectConn(const uint32_t _listIndex, char* _lines[3], bool _selected)
{
  memcpy(_lines[1], Util_convertBdAddr2Str(connList[_listIndex].addr), CD_ADDR_STR_SIZE);

  if (_selected)
  {
    // index cannot be equal to or greater than MAX_NUM_BLE_CONNS
    CENTRALDISPLAY_ASSERT(_listIndex < MAX_NUM_BLE_CONNS);

    if (connList[_listIndex].connHandle == LINKDB_CONNHANDLE_INVALID)
    {
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                             "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                             "Choose a valid connection to work with");
    }
    else
    {
        CentralDisplay_CancelRssi(cdConnHandle);
        cdConnHandle = connList[_listIndex].connHandle;
        CentralDisplay_StartRssi();
    }
  }
}

/*********************************************************************
 * @fn      CentralDisplay_doSetConnPhy
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
bool CentralDisplay_doSetConnPhy(uint8_t index)
{
  bool status = TRUE;

  static uint8_t phy[] = {
    HCI_PHY_1_MBPS, HCI_PHY_2_MBPS, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS,
    HCI_PHY_CODED, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS | HCI_PHY_CODED
  };

  if (cdConnHandle == LINKDB_CONNHANDLE_INVALID)
  {
    return FALSE;
  }

  // Set Phy Preference on the current connection. Apply the same value
  // for RX and TX. For more information, see the LE 2M PHY section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  // Note PHYs are already enabled by default in build_config.opt in stack project.
  if(phy[index] == HCI_PHY_CODED){
    // Send PHY Update
    HCI_LE_SetPhyCmd(cdConnHandle, 0, phy[index], phy[index], LL_PHY_OPT_S2);
  }
  else {
    // Send PHY Update
    HCI_LE_SetPhyCmd(cdConnHandle, 0, phy[index], phy[index], LL_PHY_OPT_NONE);
  }


  return status;
}

/*********************************************************************
 * @fn      CentralDisplay_cyclePhy
 *
 * @brief   Set the BLE5 Connection Phy mode.
 *
 * @param   _listIndex -  index of CUI menu option
 */
static void CentralDisplay_cyclePhy(const uint32_t _listIndex, char* _lines[3], bool _selected)
{
    static char* phyStrings[] = {"       1M",
                                 "       2M",
                                 "      1M|2M",
                                 "      CODED",
                                 "    1M|2M|CODED"};


    strncpy(_lines[1], phyStrings[_listIndex], strlen(phyStrings[_listIndex]));

    if (_selected)
    {
        uint8_t connIndex;
        connIndex = CentralDisplay_getConnIndex(cdConnHandle);

        if (connIndex >= MAX_NUM_BLE_CONNS)
        {
            CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                                 "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                                 "Choose a valid connection to set connection phy");
        }
        else
        {
            CentralDisplay_doSetConnPhy(_listIndex);
        }
    }
}

/*********************************************************************
 * @fn      CentralDisplay_doConnUpdate
 *
 * @brief   Initiate Connection Update procedure
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
static void CentralDisplay_doConnUpdate(int32_t index)
{
    uint8_t connIndex;
    connIndex = CentralDisplay_getConnIndex(cdConnHandle);

    if (connIndex >= MAX_NUM_BLE_CONNS)
    {
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                             "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                             "Choose a valid connection to update connection parameters");
    }
    else
    {
        gapUpdateLinkParamReq_t params;

        (void) index;

        params.connectionHandle = cdConnHandle;
        params.intervalMin = minConnInt;
        params.intervalMax = maxConnInt;
        params.connLatency = DEFAULT_UPDATE_SLAVE_LATENCY;
        params.connTimeout = svTimeout;

        linkDBInfo_t linkInfo;
        if (linkDB_GetInfo(cdConnHandle, &linkInfo) == SUCCESS)
        {
            GAP_UpdateLinkParamReq(&params);

            CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3, "Param update Request:connTimeout =%d",
                                 params.connTimeout*CONN_TIMEOUT_MS_CONVERSION);
        }
        else
        {
            CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineConnInfo3,
                                 "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                                 "update :%s, Unable to find link information",
                                 Util_convertBdAddr2Str(linkInfo.addr));
        }
    }
}

/*********************************************************************
 * @fn      moveCursor
 *
 * @brief moves cursor when setting configurable through the UI
 *
 * @param   col - current column index
 * @param   left_boundary - First column being edited
 * @param   right_boundary - Last column being edited
 * @param   direction - 0: left
 *                      1: right
 *
 * @return  current column index
 */
static uint8_t moveCursor(uint8_t col, uint8_t left_boundary, uint8_t right_boundary, uint8_t direction)
{
    if (direction == 0)
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
    }
    else
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
    }

    return col;
}

/*********************************************************************
 * @fn      CentralDisplay_setMinConnInt
 *
 * @brief Callback to be called when the UI sets min connection interval.
 *
 */
static void CentralDisplay_setMinConnInt(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo)
{
    static CUI_cursorInfo_t cursor = {0, 4};
    static char minConnIntASCII[5] = "1000";
    uint16_t tempConnInt;

    switch (_input) {
        case CUI_ITEM_INTERCEPT_START:
        {
            cursor.col = 4;

            break;
        }
        // Submit the final modified value
        case CUI_ITEM_INTERCEPT_STOP:
        {
            tempConnInt = atoi(minConnIntASCII);
            // Check if within allowable range
            if (tempConnInt < 8)
            {
                minConnInt = 8;
                strcpy(minConnIntASCII, "0008");
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                                     "[" CUI_COLOR_YELLOW "Warning" CUI_COLOR_RESET "] "
                                     "Min Connection Interval must be between 8 and 4000");
            }
            else if (tempConnInt > 4000)
            {
                minConnInt = 4000;
                strcpy(minConnIntASCII, "4000");
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                                     "[" CUI_COLOR_YELLOW "Warning" CUI_COLOR_RESET "] "
                                     "Min Connection Interval must be between 8 and 4000");
            }
            else
            {
                minConnInt = (uint16_t) (tempConnInt / 1.25);
            }

            break;
        }
        // Show the value of this screen w/o making changes
        case CUI_ITEM_PREVIEW:
            break;
        // Move the cursor to the left
        case CUI_INPUT_LEFT:
            cursor.col = moveCursor(cursor.col, 4, 7, 0);
            break;

        // Move the cursor to the right
        case CUI_INPUT_RIGHT:
            cursor.col = moveCursor(cursor.col, 4, 7, 1);
            break;

        case CUI_INPUT_UP:
            break;

        case CUI_INPUT_DOWN:
            break;

        case CUI_INPUT_EXECUTE:
            break;

        case CUI_INPUT_BACK:
            if (4 <= cursor.col)
            {
                // clear the ASCII directly with 0
                minConnIntASCII[cursor.col-4] = '0' ;
                cursor.col = moveCursor(cursor.col, 4, 7, 0);
            }
            break;
        default:
        {
            //is it a number
            if(CUI_IS_INPUT_NUM(_input))
            {
                // directly set the ASCII value because the array
                // is ASCII And so is the input
                minConnIntASCII[cursor.col-4] = _input;
                cursor.col = moveCursor(cursor.col, 4, 7, 1);
            }
        }
    }

    // clear the label.
    strcpy(_pLines[0], "    ");
    strncat(_pLines[0], minConnIntASCII, 4);

    if (_input != CUI_ITEM_PREVIEW) {
        // set the label in the right place.
        strcpy(_pLines[2], " MIN CONN INT (MS) ");
        _pCurInfo->row = 1;
        _pCurInfo->col = cursor.col+1;
    }
}

/*********************************************************************
 * @fn      CentralDisplay_setMaxConnInt
 *
 * @brief Callback to be called when the UI sets max connection interval
 */
static void CentralDisplay_setMaxConnInt(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo)
{
    static CUI_cursorInfo_t cursor = {0, 4};
    static char maxConnIntASCII[5] = "1000";
    uint16_t tempConnInt;

    switch (_input) {
        case CUI_ITEM_INTERCEPT_START:
        {
            cursor.col = 4;

            break;
        }
        // Submit the final modified value
        case CUI_ITEM_INTERCEPT_STOP:
        {
            tempConnInt = atoi(maxConnIntASCII);
            // Check if within allowable range
            if (tempConnInt < 8)
            {
                maxConnInt = 8;
                strcpy(maxConnIntASCII, "0008");
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                                     "[" CUI_COLOR_YELLOW "Warning" CUI_COLOR_RESET "] "
                                     "Max Connection Interval must be between 8 and 4000");
            }
            else if (tempConnInt > 4000)
            {
                maxConnInt = 4000;
                strcpy(maxConnIntASCII, "4000");
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                                     "[" CUI_COLOR_YELLOW "Warning" CUI_COLOR_RESET "] "
                                     "Max Connection Interval must be between 8 and 4000");
            }
            else
            {
                maxConnInt = (uint16_t) (tempConnInt / 1.25);
            }

            break;
        }
        // Show the value of this screen w/o making changes
        case CUI_ITEM_PREVIEW:
            break;
        // Move the cursor to the left
        case CUI_INPUT_LEFT:
            cursor.col = moveCursor(cursor.col, 4, 7, 0);
            break;

        // Move the cursor to the right
        case CUI_INPUT_RIGHT:
            cursor.col = moveCursor(cursor.col, 4, 7, 1);
            break;

        case CUI_INPUT_UP:
            break;

        case CUI_INPUT_DOWN:
            break;

        case CUI_INPUT_EXECUTE:
            break;

        case CUI_INPUT_BACK:
            if (4 <= cursor.col)
            {
                // clear the ASCII directly with 0
                maxConnIntASCII[cursor.col-4] = '0' ;
                cursor.col = moveCursor(cursor.col, 4, 7, 0);
            }
            break;
        default:
        {
            //is it a number
            if(CUI_IS_INPUT_NUM(_input))
            {
                // directly set the ASCII value because the array
                // is ASCII And so is the input
                maxConnIntASCII[cursor.col-4] = _input;
                cursor.col = moveCursor(cursor.col, 4, 7, 1);
            }
        }
    }

    // clear the label.
    strcpy(_pLines[0], "    ");
    strncat(_pLines[0], maxConnIntASCII, 4);

    if (_input != CUI_ITEM_PREVIEW) {
        // set the label in the right place.
        strcpy(_pLines[2], " MAX CONN INT (MS) ");
        _pCurInfo->row = 1;
        _pCurInfo->col = cursor.col+1;
    }
}

/**
 *  @brief Callback to be called when the UI sets supervision timeout
 */
static void CentralDisplay_setSvTimeout(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo)
{
    static CUI_cursorInfo_t cursor = {0, 4};
    static char svTimeoutASCII[6] = "06000";
    uint16_t tempSvTimeout;

    switch (_input) {
        case CUI_ITEM_INTERCEPT_START:
        {
            cursor.col = 4;

            break;
        }
        // Submit the final modified value
        case CUI_ITEM_INTERCEPT_STOP:
        {
            tempSvTimeout = atoi(svTimeoutASCII);
            // Check if within allowable range
            if (tempSvTimeout < 100)
            {
                svTimeout = 10;
                strcpy(svTimeoutASCII, "00100");
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                                     "[" CUI_COLOR_YELLOW "Warning" CUI_COLOR_RESET "] "
                                     "Supervision timeout must be between 100 and 32000");
            }
            else if (tempSvTimeout > 32000)
            {
                svTimeout = 3200;
                strcpy(svTimeoutASCII, "32000");
                CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                                     "[" CUI_COLOR_YELLOW "Warning" CUI_COLOR_RESET "] "
                                     "Supervision timeout must be between 100 and 32000");
            }
            else
            {
                svTimeout = (uint16_t) (tempSvTimeout / 10);
            }

            break;
        }
        // Show the value of this screen w/o making changes
        case CUI_ITEM_PREVIEW:
            break;
        // Move the cursor to the left
        case CUI_INPUT_LEFT:
            cursor.col = moveCursor(cursor.col, 4, 8, 0);
            break;

        // Move the cursor to the right
        case CUI_INPUT_RIGHT:
            cursor.col = moveCursor(cursor.col, 4, 8, 1);
            break;

        case CUI_INPUT_UP:
            break;

        case CUI_INPUT_DOWN:
            break;

        case CUI_INPUT_EXECUTE:
            break;

        case CUI_INPUT_BACK:
            if (4 <= cursor.col)
            {
                // clear the ASCII directly with 0
                svTimeoutASCII[cursor.col-4] = '0' ;
                cursor.col = moveCursor(cursor.col, 4, 8, 0);
            }
            break;
        default:
        {
            //is it a number
            if(CUI_IS_INPUT_NUM(_input))
            {
                // directly set the ASCII value because the array
                // is ASCII And so is the input
                svTimeoutASCII[cursor.col-4] = _input;
                cursor.col = moveCursor(cursor.col, 4, 8, 1);
            }
        }
    }

    // clear the label.
    strcpy(_pLines[0], "    ");
    strncat(_pLines[0], svTimeoutASCII, 5);

    if (_input != CUI_ITEM_PREVIEW) {
        // set the label in the right place.
        strcpy(_pLines[2], " SUPERVISION TIMEOUT (MS) ");
        _pCurInfo->row = 1;
        _pCurInfo->col = cursor.col+1;
    }
}

/*********************************************************************
 * @fn      CentralDisplay_doDisconnect
 *
 * @brief   Disconnect the specified link
 *
 * @param   index - item index from the menu
 *
 * @return  none
 */
static void CentralDisplay_doDisconnect(int32_t index)
{
    uint8_t connIndex;
    connIndex = CentralDisplay_getConnIndex(cdConnHandle);

    if (connIndex >= MAX_NUM_BLE_CONNS)
    {
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                             "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                             "Choose a valid connection to disconnect");
    }
    else
    {
        GAP_TerminateLinkReq(cdConnHandle, HCI_DISCONNECT_REMOTE_USER_TERM);
    }
}

/*********************************************************************
 * @fn      CentralDisplay_tglNotifications
 *
 * @brief   Toggle Notifications for specified link
 *
 * @param   index - item index from the menu
 *
 * @return  none
 */
static void CentralDisplay_doTglNotifications(int32_t menuEntryIndex)
{
    uint8_t connIndex;
    connIndex = CentralDisplay_getConnIndex(cdConnHandle);

    if (connIndex >= MAX_NUM_BLE_CONNS)
    {
        CUI_statusLinePrintf(CentralDisplayCuiHndl, rdStatusLineStatus,
                             "[" CUI_COLOR_RED "Error" CUI_COLOR_RESET "] "
                             "Choose a valid connection to toggle notifications");
    }
    else
    {
        CentralDisplay_enqueueMsg(CD_EVT_TGL_NOTIF, 0, NULL);
    }
}

/**
 *  @brief Send process menu event.
 */
static void CentralDisplay_processMenuUpdate(void)
{
    CentralDisplay_enqueueMsg(CD_UI_INPUT_EVT, 0, NULL);
}

/*********************************************************************
 *  @fn      CentralDisplay_bleFastStateUpdateCb
 *
 * @brief   Callback from BLE link layer to indicate a state change
 */
void CentralDisplay_bleFastStateUpdateCb(uint32_t StackRole, uint32_t stackState)
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
