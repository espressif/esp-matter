/******************************************************************************

 @file  simple_central.c

 @brief This file contains the Simple Central sample application for use
        with the CC2650 Bluetooth Low Energy Protocol Stack.

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

#include "bcomdef.h"

#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "osal_list.h"
#include "board_key.h"
#include <ti_drivers_config.h>

#include "ti_ble_config.h"
#include "ble_user_config.h"

#include "simple_gatt_profile.h"

#include <menu/two_btn_menu.h>
#include "simple_central.h"
#include "simple_central_menu.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Application events
#define SC_EVT_KEY_CHANGE          0x01
#define SC_EVT_SCAN_ENABLED        0x02
#define SC_EVT_SCAN_DISABLED       0x03
#define SC_EVT_ADV_REPORT          0x04
#define SC_EVT_SVC_DISC            0x05
#define SC_EVT_READ_RSSI           0x06
#define SC_EVT_PAIR_STATE          0x07
#define SC_EVT_PASSCODE_NEEDED     0x08
#define SC_EVT_READ_RPA            0x09
#define SC_EVT_INSUFFICIENT_MEM    0x0A

// Simple Central Task Events
#define SC_ICALL_EVT                         ICALL_MSG_EVENT_ID  // Event_Id_31
#define SC_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30

#define SC_ALL_EVENTS                        (SC_ICALL_EVT           | \
                                              SC_QUEUE_EVT)

// Default connection interval when connecting to more then 8 connections and autoconnenct enabled
#define DEFAULT_MULTICON_INTERVAL            200 //250 ms (200 frames of 1.25ms)

// Default connection supervision timeout when connnecting to more then 8 connections and autoconnenct enabled
#define DEFAULT_MULTICON_LSTO                3200 // 32 secs

// Supervision timeout conversion rate to miliseconds
#define CONN_TIMEOUT_MS_CONVERSION            10

// Task configuration
#define SC_TASK_PRIORITY                     1

#ifndef SC_TASK_STACK_SIZE
#define SC_TASK_STACK_SIZE                   1024
#endif

// Size of string-converted device address ("0xXXXXXXXXXXXX")
#define SC_ADDR_STR_SIZE     15

// Row numbers for two-button menu
#define SC_ROW_SEPARATOR     (TBM_ROW_APP + 0)
#define SC_ROW_CUR_CONN      (TBM_ROW_APP + 1)
#define SC_ROW_ANY_CONN      (TBM_ROW_APP + 2)
#define SC_ROW_NON_CONN      (TBM_ROW_APP + 3)
#define SC_ROW_NUM_CONN      (TBM_ROW_APP + 4)
#define SC_ROW_IDA           (TBM_ROW_APP + 5)
#define SC_ROW_RPA           (TBM_ROW_APP + 6)
#define SC_ROW_AC            (TBM_ROW_APP + 8)

// Spin if the expression is not true
#define SIMPLECENTRAL_ASSERT(expr) if (!(expr)) SimpleCentral_spin();

// Timeout for the initiator to cancel connection if not successful
#define CONNECTION_TIMEOUT                   3000

// Auto connect chosen group
#define GROUP_NAME_LENGTH                    4

//Member defalult status when initalized
#define GROUP_MEMBER_INITIALIZED             0x00

//Member connected
#define GROUP_MEMBER_CONNECTED               0x01

//Default connection handle which is set when group member is created
#define GROUP_INITIALIZED_CONNECTION_HANDLE  0xFFFF


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
  BLE_DISC_STATE_SVC,                 // Service discovery
  BLE_DISC_STATE_CHAR                 // Characteristic discovery
};

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr; // event header
  uint8_t *pData;  // event data
} scEvt_t;

// Scanned device information record
typedef struct
{
  uint8_t addrType;         // Peer Device's Address Type
  uint8_t addr[B_ADDR_LEN]; // Peer Device Address
} scanRec_t;

// Connected device information
typedef struct
{
  uint16_t connHandle;        // Connection Handle
  uint16_t charHandle;        // Characteristic Handle
  uint8_t  addr[B_ADDR_LEN];  // Peer Device Address
  Clock_Struct *pRssiClock;   // pointer to clock struct
} connRec_t;

// Container to store paring state info when passing from gapbondmgr callback
// to app event. See the pfnPairStateCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct
{
  uint16_t connHandle;
  uint8_t  status;
} scPairStateData_t;

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
} scPasscodeData_t;

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

// Display Interface
Display_Handle dispHandle = NULL;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

#define APP_EVT_EVENT_MAX 0xA
char *appEventStrings[] = {
  "APP_EVT_ZERO              ",
  "APP_EVT_KEY_CHANGE        ",
  "APP_EVT_SCAN_ENABLED      ",
  "APP_EVT_SCAN_DISABLED     ",
  "APP_EVT_ADV_REPORT        ",
  "APP_EVT_SVC_DISC          ",
  "APP_EVT_READ_RSSI         ",
  "APP_EVT_PAIR_STATE        ",
  "APP_EVT_PASSCODE_NEEDED   ",
  "APP_EVT_READ_RPA          ",
  "APP_EVT_INSUFFICIENT_MEM  ",
};

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
Task_Struct scTask;
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(scTaskStack, 8)
#else
#pragma data_alignment=8
#endif
uint8_t scTaskStack[SC_TASK_STACK_SIZE];

#if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
// Number of scan results filtered by Service UUID
static uint8_t numScanRes = 0;

// Scan results filtered by Service UUID
static scanRec_t scanList[DEFAULT_MAX_SCAN_RES];
#endif // DEFAULT_DEV_DISC_BY_SVC_UUID

// Number of connected devices
static uint8_t numConn = 0;

// List of connections
static connRec_t connList[MAX_NUM_BLE_CONNS];

// Connection handle of current connection
static uint16_t scConnHandle = LINKDB_CONNHANDLE_INVALID;

// Accept or reject L2CAP connection parameter update request
static bool acceptParamUpdateReq = true;

// Discovery state
static uint8_t discState = BLE_DISC_STATE_IDLE;

// Discovered service start and end handle
static uint16_t svcStartHdl = 0;
static uint16_t svcEndHdl = 0;

// Value to write
static uint8_t charVal = 0;

// Maximum PDU size (default = 27 octets)
static uint16_t scMaxPduSize;

// Clock instance for RPA read events.
static Clock_Struct clkRpaRead;

// Address mode
static GAP_Addr_Modes_t addrMode = DEFAULT_ADDRESS_MODE;

// Current Random Private Address
static uint8 rpa[B_ADDR_LEN] = {0};

// Auto connect Disabled/Enabled {0 - Disabled, 1- Group A , 2-Group B, ...}
uint8_t autoConnect = AUTOCONNECT_DISABLE;

//AutoConnect Group list
static osal_list_list groupList;

//AutoConnect ADV data filter according to local name short
static uint8_t acGroup[4] = {
  0x03,
  GAP_ADTYPE_LOCAL_NAME_SHORT,
  'G',
  'A'
 };

//Number of group members found
static uint8_t numGroupMembers = 0;

//Connection in progress to avoid double initiate
static groupListElem_t *memberInProg;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void SimpleCentral_init(void);
static void SimpleCentral_taskFxn(uintptr_t a0, uintptr_t a1);

static uint8_t SimpleCentral_isMember(uint8_t *advData , uint8_t *groupName , uint8_t len);
static void SimpleCentral_autoConnect(void);

static void SimpleCentral_handleKeys(uint8_t keys);
static uint8_t SimpleCentral_processStackMsg(ICall_Hdr *pMsg);
static void SimpleCentral_processGapMsg(gapEventHdr_t *pMsg);
static void SimpleCentral_processGATTMsg(gattMsgEvent_t *pMsg);
static void SimpleCentral_processAppMsg(scEvt_t *pMsg);
static void SimpleCentral_processGATTDiscEvent(gattMsgEvent_t *pMsg);
static void SimpleCentral_startSvcDiscovery(void);
#if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
static bool SimpleCentral_findSvcUuid(uint16_t uuid, uint8_t *pData,
                                      uint16_t dataLen);
static void SimpleCentral_addScanInfo(uint8_t *pAddr, uint8_t addrType);
#endif // DEFAULT_DEV_DISC_BY_SVC_UUID
static uint8_t SimpleCentral_addConnInfo(uint16_t connHandle, uint8_t *pAddr);
static uint8_t SimpleCentral_removeConnInfo(uint16_t connHandle);
static uint8_t SimpleCentral_getConnIndex(uint16_t connHandle);
#ifndef Display_DISABLE_ALL
static char* SimpleCentral_getConnAddrStr(uint16_t connHandle);
#endif
static void SimpleCentral_processPairState(uint8_t state,
                                           scPairStateData_t* pPairStateData);
static void SimpleCentral_processPasscode(scPasscodeData_t *pData);

static void SimpleCentral_processCmdCompleteEvt(hciEvt_CmdComplete_t *pMsg);
static status_t SimpleCentral_StartRssi();
static status_t SimpleCentral_CancelRssi(uint16_t connHandle);

static void SimpleCentral_passcodeCb(uint8_t *deviceAddr, uint16_t connHandle,
                                     uint8_t uiInputs, uint8_t uiOutputs,
                                     uint32_t numComparison);
static void SimpleCentral_pairStateCb(uint16_t connHandle, uint8_t state,
                                      uint8_t status);

static void SimpleCentral_keyChangeHandler(uint8 keys);
static void SimpleCentral_clockHandler(UArg arg);

static status_t SimpleCentral_enqueueMsg(uint8_t event, uint8_t status,
                                        uint8_t *pData);

static void SimpleCentral_scanCb(uint32_t evt, void* msg, uintptr_t arg);
static void SimpleCentral_menuSwitchCb(tbmMenuObj_t* pMenuObjCurr,
                                       tbmMenuObj_t* pMenuObjNext);

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
  SimpleCentral_passcodeCb, // Passcode callback
  SimpleCentral_pairStateCb // Pairing/Bonding state Callback
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/*********************************************************************
 * @fn		SimpleCentral_isMember
 *
 * @brief	Check if Advertiser is part of the group according to its Adv Data
 *
 * @param	advData   - pointer to adv data
 *          groupNmae - group name which need to be compared with
 *          len       - length of the group name
 *
 * @return  TRUE: part of the group
 *          FALSE: not part of the group
 */

static uint8_t SimpleCentral_isMember(uint8_t *advData , uint8_t *groupName , uint8_t len)
{
  if (osal_memcmp((uint8_t *)advData, (uint8_t *)groupName, len))
  {
    return TRUE;
  }
  return FALSE;
}

/*********************************************************************
 * @fn		SimpleCentral_autoConnect
 *
 * @brief	Check if Advertiser is part of the group according to its Adv Data
 *
 * @param   none
 *
 * @return  none
 */

static void SimpleCentral_autoConnect(void) 
{
  status_t status;
  if (memberInProg == NULL)
  {
    if (numConn < MAX_NUM_BLE_CONNS)
    {
	  groupListElem_t *tempMember = (groupListElem_t *)osal_list_head(&groupList);
      //If group member is not connected
      if ((tempMember != NULL) && (!(tempMember->status & GROUP_MEMBER_CONNECTED)))
      {
        //Initiate a connection
        status = GapInit_connect(tempMember->addrType & MASK_ADDRTYPE_ID,tempMember->addr, DEFAULT_INIT_PHY, CONNECTION_TIMEOUT);
        if (status != SUCCESS)
        {
          //Couldn't create connection remove element from list and free the memory.
          osal_list_remove(&groupList, (osal_list_elem *)tempMember);
          ICall_free(tempMember);
        }
    	else
    	{
          //Save pointer to connection in progress untill connection is established.
		  memberInProg = tempMember;
        }
	  }
    }
    else
    {
      Display_printf(dispHandle, SC_ROW_NON_CONN, 0, "AutoConnect turned off: Max connection reached.");
    }
  }
}

/*********************************************************************
 * @fn      SimpleCentral_spin
 *
 * @brief   Spin forever
 *
 * @param   none
 */
static void SimpleCentral_spin(void)
{
  volatile uint8_t x;

  while(1)
  {
    x++;
  }
}

/*********************************************************************
 * @fn      SimpleCentral_createTask
 *
 * @brief   Task creation function for the Simple Central.
 *
 * @param   none
 *
 * @return  none
 */
void SimpleCentral_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = scTaskStack;
  taskParams.stackSize = SC_TASK_STACK_SIZE;
  taskParams.priority = SC_TASK_PRIORITY;

  Task_construct(&scTask, SimpleCentral_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      SimpleCentral_Init
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
static void SimpleCentral_init(void)
{
  uint8_t i;

  BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- init ", SC_TASK_PRIORITY);
  // Create the menu
  SimpleCentral_buildMenu();

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

  Board_initKeys(SimpleCentral_keyChangeHandler);

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
  VOID GATT_InitClient("");

  // Register to receive incoming ATT Indications/Notifications
  GATT_RegisterForInd(selfEntity);

  // Initialize GATT attributes
  GGS_AddService(GAP_SERVICE);               // GAP
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

  BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- call GAP_DeviceInit", GAP_PROFILE_CENTRAL);
  // Initialize GAP layer for Central role and register to receive GAP events
  GAP_DeviceInit(GAP_PROFILE_CENTRAL, selfEntity, addrMode, &pRandomAddress);

  dispHandle = Display_open(Display_Type_ANY, NULL);

  // Disable all items in the main menu
  tbm_setItemStatus(&scMenuMain, SC_ITEM_NONE, SC_ITEM_ALL);
  // Initialize Two-button Menu
  tbm_initTwoBtnMenu(dispHandle, &scMenuMain, 5, SimpleCentral_menuSwitchCb);
  Display_printf(dispHandle, SC_ROW_SEPARATOR, 0, "====================");
}

/*********************************************************************
 * @fn      SimpleCentral_taskFxn
 *
 * @brief   Application task entry point for the Simple Central.
 *
 * @param   none
 *
 * @return  events not processed
 */
static void SimpleCentral_taskFxn(uintptr_t a0, uintptr_t a1)
{
  // Initialize application
  SimpleCentral_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    events = Event_pend(syncEvent, Event_Id_NONE, SC_ALL_EVENTS,
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
            safeToDealloc = SimpleCentral_processStackMsg((ICall_Hdr *)pMsg);
          }
        }

        if (pMsg && safeToDealloc)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message
      if (events & SC_QUEUE_EVT)
      {
        scEvt_t *pMsg;
        while ((pMsg = (scEvt_t *)Util_dequeueMsg(appMsgQueue)))
        {
          // Process message
          SimpleCentral_processAppMsg(pMsg);

          // Free the space from the message
          ICall_free(pMsg);
        }
      }
    }
  }
}

/*********************************************************************
 * @fn      SimpleCentral_processStackMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t SimpleCentral_processStackMsg(ICall_Hdr *pMsg)
{
  uint8_t safeToDealloc = TRUE;

  BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : Stack msg status=%d, event=0x%x\n", pMsg->status, pMsg->event);

  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:
      SimpleCentral_processGapMsg((gapEventHdr_t*) pMsg);
      break;

    case GATT_MSG_EVENT:
      SimpleCentral_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    case HCI_GAP_EVENT_EVENT:
    {
      // Process HCI message
      switch (pMsg->status)
      {
        case HCI_COMMAND_COMPLETE_EVENT_CODE:
          SimpleCentral_processCmdCompleteEvt((hciEvt_CmdComplete_t *) pMsg);
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
                    Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                            "PHY Change failure, peer does not support this");
                  }
                  else
                  {
                    Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                                   "PHY Update Status: 0x%02x",
                                   pMyMsg->cmdStatus);
                  }
                }
                break;
              case HCI_DISCONNECT:
                break;

              default:
                {
                  Display_printf(dispHandle, SC_ROW_NON_CONN, 0,
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
              Display_printf(dispHandle, SC_ROW_ANY_CONN, 0,
                             "%s: PHY change failure",
                             SimpleCentral_getConnAddrStr(pPUC->connHandle));
            }
            else
            {
              Display_printf(dispHandle, SC_ROW_ANY_CONN, 0,
                             "%s: PHY updated to %s",
                             SimpleCentral_getConnAddrStr(pPUC->connHandle),
              // Only symmetrical PHY is supported.
              // rxPhy should be equal to txPhy.
                                (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_1M) ? "1 Mbps" :
                                (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_2M) ? "2 Mbps" :
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
 * @fn      SimpleCentral_processAppMsg
 *
 * @brief   Scanner application event processing function.
 *
 * @param   pMsg - pointer to event structure
 *
 * @return  none
 */
static void SimpleCentral_processAppMsg(scEvt_t *pMsg)
{
  bool safeToDealloc = TRUE;

  if (pMsg->hdr.event <= APP_EVT_EVENT_MAX)
  {
    BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : App msg status=%d, event=%s\n", 0, appEventStrings[pMsg->hdr.event]);
  }
  else
  {
    BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : App msg status=%d, event=0x%x\n", 0, pMsg->hdr.event);
  }

  switch (pMsg->hdr.event)
  {
    case SC_EVT_KEY_CHANGE:
      SimpleCentral_handleKeys(pMsg->hdr.state);
      break;

    case SC_EVT_ADV_REPORT:
    {
      GapScan_Evt_AdvRpt_t* pAdvRpt = (GapScan_Evt_AdvRpt_t*) (pMsg->pData);
      //Auto connect is enabled
      if (autoConnect) 
      {
        if (numGroupMembers == MAX_NUM_BLE_CONNS)
        {
          GapScan_disable("");
          break;
		}
        //Check if advertiser is part of the group
        if (SimpleCentral_isMember(pAdvRpt->pData , acGroup, GROUP_NAME_LENGTH))
	    {
     	  groupListElem_t *tempMember;
     	  //Traverse list to search if advertiser already in list.
  	      for (tempMember = (groupListElem_t *)osal_list_head(&groupList); tempMember != NULL; tempMember = (groupListElem_t *)osal_list_next((osal_list_elem *)tempMember)) 
  	      {
            if (osal_memcmp((uint8_t *)tempMember->addr ,(uint8_t *)pAdvRpt->addr,B_ADDR_LEN))
            {
              break;
            }
          }
          //If tempMemer is NULL this meams advertiser not in list.
          if (tempMember == NULL)
          {
            groupListElem_t *groupMember = (groupListElem_t *)ICall_malloc(sizeof(groupListElem_t));
            if (groupMember != NULL)
            {
              //Copy member's details into Member's list.
              osal_memcpy((uint8_t *)groupMember->addr , (uint8_t *)pAdvRpt->addr,B_ADDR_LEN);
              groupMember->addrType = pAdvRpt->addrType;
              groupMember->status = GROUP_MEMBER_INITIALIZED;
              groupMember->connHandle = GROUP_INITIALIZED_CONNECTION_HANDLE;
              //Add group member into list.
              osal_list_putHead(&groupList,(osal_list_elem *)groupMember);
			  numGroupMembers++;
            }
            else
            {
              Display_printf(dispHandle, SC_ROW_AC, 0, "AutoConnect: Allocation failed!");
              break;
            }
          }
        }
      }
#if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
      if (SimpleCentral_findSvcUuid(SIMPLEPROFILE_SERV_UUID,
                                    pAdvRpt->pData, pAdvRpt->dataLen))
      {
        SimpleCentral_addScanInfo(pAdvRpt->addr, pAdvRpt->addrType);
        Display_printf(dispHandle, SC_ROW_NON_CONN, 0, "Discovered: %s",
                       Util_convertBdAddr2Str(pAdvRpt->addr));
      }
#else // !DEFAULT_DEV_DISC_BY_SVC_UUID
      Display_printf(dispHandle, SC_ROW_NON_CONN, 0, "Discovered: %s",
                     Util_convertBdAddr2Str(pAdvRpt->addr));
#endif // DEFAULT_DEV_DISC_BY_SVC_UUID

      // Free report payload data
      if (pAdvRpt->pData != NULL)
      {
        ICall_free(pAdvRpt->pData);
      }
      break;
    }

    case SC_EVT_SCAN_ENABLED:
      // Disable everything but "Stop Discovering" on the menu
      tbm_setItemStatus(&scMenuMain, SC_ITEM_STOPDISC,
                        (SC_ITEM_ALL & ~SC_ITEM_STOPDISC));
      Display_printf(dispHandle, SC_ROW_NON_CONN, 0, "Discovering...");
      break;

    case SC_EVT_SCAN_DISABLED:
    {
      uint16_t itemsToEnable = SC_ITEM_STARTDISC | SC_ITEM_SCANPHY;
      if (autoConnect)
      {
        itemsToEnable |= SC_ITEM_AUTOCONNECT;
		if (numGroupMembers < MAX_NUM_BLE_CONNS)
        {
           Display_printf(dispHandle, SC_ROW_AC, 0, "AutoConnect: Not all members found, only %d members were found",numGroupMembers);
        }
		else
		{
		  Display_printf(dispHandle, SC_ROW_AC, 0, "AutoConnect: Number of members in the group %d",numGroupMembers);
		  SimpleCentral_autoConnect();
          if (numConn > 0)
	      {
	        // Also enable "Work with"
	        itemsToEnable |= SC_ITEM_SELECTCONN;
	      }
        }
		// Enable "Discover Devices", "Set Scanning PHY", and possibly
	    // "Connect to" and/or "Work with".
	    // Disable "Stop Discovering".
  	    tbm_setItemStatus(&scMenuMain, itemsToEnable, SC_ITEM_STOPDISC);
	  }
      else
      {
	      uint8_t numReport;
	      uint8_t i;
	      static uint8_t* pAddrs = NULL;
	      uint8_t* pAddrTemp;
#if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
	      numReport = numScanRes;
#else // !DEFAULT_DEV_DISC_BY_SVC_UUID
	      GapScan_Evt_AdvRpt_t advRpt;

	      numReport = ((GapScan_Evt_End_t*) (pMsg->pData))->numReport;
#endif // DEFAULT_DEV_DISC_BY_SVC_UUID

	      Display_printf(dispHandle, SC_ROW_NON_CONN, 0,
	                     "%d devices discovered", numReport);

	      if (numReport > 0)
	      {
	        // Also enable "Connect to"
	        itemsToEnable |= SC_ITEM_CONNECT;
	      }

	      if (numConn > 0)
	      {
	        // Also enable "Work with"
	        itemsToEnable |= SC_ITEM_SELECTCONN;
	      }

	      // Enable "Discover Devices", "Set Scanning PHY", and possibly
	      // "Connect to" and/or "Work with".
	      // Disable "Stop Discovering".
	      tbm_setItemStatus(&scMenuMain, itemsToEnable, SC_ITEM_STOPDISC);

	      // Allocate buffer to display addresses
	      if (pAddrs != NULL)
	      {
	        // A scan has been done previously, release the previously allocated buffer
	        ICall_free(pAddrs);
	      }
	      pAddrs = ICall_malloc(numReport * SC_ADDR_STR_SIZE);
	      if (pAddrs == NULL)
	      {
	        numReport = 0;
	      }

	      TBM_SET_NUM_ITEM(&scMenuConnect, numReport);

	      if (pAddrs != NULL)
	      {
	        pAddrTemp = pAddrs;
	        for (i = 0; i < numReport; i++, pAddrTemp += SC_ADDR_STR_SIZE)
	        {
	  #if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
	          // Get the address from the list, convert it to string, and
	          // copy the string to the address buffer
	          memcpy(pAddrTemp, Util_convertBdAddr2Str(scanList[i].addr),
	                 SC_ADDR_STR_SIZE);
	  #else // !DEFAULT_DEV_DISC_BY_SVC_UUID
	          // Get the address from the report, convert it to string, and
	          // copy the string to the address buffer
	          GapScan_getAdvReport(i, &advRpt);
	          memcpy(pAddrTemp, Util_convertBdAddr2Str(advRpt.addr),
	                 SC_ADDR_STR_SIZE);
	  #endif // DEFAULT_DEV_DISC_BY_SVC_UUID

	          // Assign the string to the corresponding action description of the menu
	          TBM_SET_ACTION_DESC(&scMenuConnect, i, pAddrTemp);
	        }

	        // Disable any non-active scan results
	        for (; i < DEFAULT_MAX_SCAN_RES; i++)
	        {
	          tbm_setItemStatus(&scMenuConnect, TBM_ITEM_NONE, (1 << i));
	        }

	        // Note: pAddrs is not freed since it will be used by the two button menu
	        // to display the discovered address.
	        // This implies that at least the last discovered addresses
	        // will be maintained until a new scan is done.
	      }
	      break;
	    }
    }
    case SC_EVT_SVC_DISC:
      SimpleCentral_startSvcDiscovery();
      break;

    case SC_EVT_READ_RSSI:
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
    case SC_EVT_PAIR_STATE:
    {
      SimpleCentral_processPairState(pMsg->hdr.state,
                                     (scPairStateData_t*) (pMsg->pData));
      break;
    }

    // Passcode event
    case SC_EVT_PASSCODE_NEEDED:
    {
      SimpleCentral_processPasscode((scPasscodeData_t *)(pMsg->pData));
      break;
    }

    case SC_EVT_READ_RPA:
    {
      uint8_t* pRpaNew;

      // Read the current RPA.
      pRpaNew = GAP_GetDevAddress(FALSE);

      if (memcmp(pRpaNew, rpa, B_ADDR_LEN))
      {
        // If the RPA has changed, update the display
        Display_printf(dispHandle, SC_ROW_RPA, 0, "RP Addr: %s",
                       Util_convertBdAddr2Str(pRpaNew));
        memcpy(rpa, pRpaNew, B_ADDR_LEN);
      }
      break;
    }

    // Insufficient memory
    case SC_EVT_INSUFFICIENT_MEM:
    {
      // We are running out of memory.
      Display_printf(dispHandle, SC_ROW_ANY_CONN, 0, "Insufficient Memory");

      // We might be in the middle of scanning, try stopping it.
      GapScan_disable("");
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
 * @fn      SimpleCentral_processGapMsg
 *
 * @brief   GAP message processing function.
 *
 * @param   pMsg - pointer to event message structure
 *
 * @return  none
 */
static void SimpleCentral_processGapMsg(gapEventHdr_t *pMsg)
{
  switch (pMsg->opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
    {
      uint8_t temp8;
      uint16_t temp16;
      gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- got GAP_DEVICE_INIT_DONE_EVENT", 0);
      // Setup scanning
      // For more information, see the GAP section in the User's Guide:
      // http://software-dl.ti.com/lprf/ble5stack-latest/

      // Register callback to process Scanner events
      GapScan_registerCb(SimpleCentral_scanCb, NULL);

      // Set Scanner Event Mask
      GapScan_setEventMask(GAP_EVT_SCAN_ENABLED | GAP_EVT_SCAN_DISABLED |
                           GAP_EVT_ADV_REPORT);

      // Set Scan PHY parameters
      GapScan_setPhyParams(DEFAULT_SCAN_PHY, DEFAULT_SCAN_TYPE,
                           DEFAULT_SCAN_INTERVAL, DEFAULT_SCAN_WINDOW);

      // Set Advertising report fields to keep
      temp16 = ADV_RPT_FIELDS;
      GapScan_setParam(SCAN_PARAM_RPT_FIELDS, &temp16);
      // Set Scanning Primary PHY
      temp8 = DEFAULT_SCAN_PHY;
      GapScan_setParam(SCAN_PARAM_PRIM_PHYS, &temp8);
      // Set LL Duplicate Filter
      temp8 = SCANNER_DUPLICATE_FILTER;
      GapScan_setParam(SCAN_PARAM_FLT_DUP, &temp8);

      // Set PDU type filter -
      // Only 'Connectable' and 'Complete' packets are desired.
      // It doesn't matter if received packets are
      // whether Scannable or Non-Scannable, whether Directed or Undirected,
      // whether Scan_Rsp's or Advertisements, and whether Legacy or Extended.
      temp16 = SCAN_FLT_PDU_CONNECTABLE_ONLY | SCAN_FLT_PDU_COMPLETE_ONLY;
      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- GapScan_setParam", 0);
      GapScan_setParam(SCAN_PARAM_FLT_PDU_TYPE, &temp16);

	  // Set initiating PHY parameters
      GapInit_setPhyParam(DEFAULT_INIT_PHY, INIT_PHYPARAM_CONN_INT_MIN,
						  INIT_PHYPARAM_MIN_CONN_INT);
	  GapInit_setPhyParam(DEFAULT_INIT_PHY, INIT_PHYPARAM_CONN_INT_MAX,
						  INIT_PHYPARAM_MAX_CONN_INT);

      scMaxPduSize = pPkt->dataPktLen;

      // Enable "Discover Devices", "Set Scanning PHY", "AutoConnect" , and "Set Address Type"
      // in the main menu
      tbm_setItemStatus(&scMenuMain,
                        SC_ITEM_STARTDISC | SC_ITEM_SCANPHY | SC_ITEM_AUTOCONNECT, SC_ITEM_NONE);

      Display_printf(dispHandle, SC_ROW_NON_CONN, 0, "Initialized");
      Display_printf(dispHandle, SC_ROW_NUM_CONN, 0, "Num Conns: %d", numConn);

      // Display device address
      Display_printf(dispHandle, SC_ROW_IDA, 0, "%s Addr: %s",
                     (addrMode <= ADDRMODE_RANDOM) ? "Dev" : "ID",
                     Util_convertBdAddr2Str(pPkt->devAddr));

      if (addrMode > ADDRMODE_RANDOM)
      {
        // Update the current RPA.
        memcpy(rpa, GAP_GetDevAddress(FALSE), B_ADDR_LEN);

        Display_printf(dispHandle, SC_ROW_RPA, 0, "RP Addr: %s",
                       Util_convertBdAddr2Str(rpa));

        // Create one-shot clock for RPA check event.
        Util_constructClock(&clkRpaRead, SimpleCentral_clockHandler,
                            READ_RPA_PERIOD, 0, true, SC_EVT_READ_RPA);
      }
      break;
    }

    case GAP_CONNECTING_CANCELLED_EVENT:
    {
      uint16_t itemsToEnable = SC_ITEM_SCANPHY | SC_ITEM_STARTDISC |
                               SC_ITEM_CONNECT | SC_ITEM_AUTOCONNECT;
	  if (autoConnect)
      {
        if (memberInProg != NULL)
		{
          //Remove node from member's group and free its memory.
          osal_list_remove(&groupList, (osal_list_elem *)memberInProg);
          ICall_free(memberInProg);  
  		  numGroupMembers--;
          memberInProg = NULL;
	    }
		Display_printf(dispHandle, SC_ROW_AC, 0, "AutoConnect: Number of members in the group %d",numGroupMembers);
		//Keep on connecting to the remaining members in the list
        SimpleCentral_autoConnect();
      }

      if (numConn > 0)
      {
        itemsToEnable |= SC_ITEM_SELECTCONN;
      }

      Display_printf(dispHandle, SC_ROW_NON_CONN, 0,
                     "Conneting attempt cancelled");

      // Enable "Discover Devices", "Connect To", and "Set Scanning PHY"
      // and disable everything else.
      tbm_setItemStatus(&scMenuMain,
                        itemsToEnable, SC_ITEM_ALL & ~itemsToEnable);

      break;
    }

    case GAP_LINK_ESTABLISHED_EVENT:
    {
      uint16_t connHandle = ((gapEstLinkReqEvent_t*) pMsg)->connectionHandle;
      uint8_t* pAddr = ((gapEstLinkReqEvent_t*) pMsg)->devAddr;
      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- got GAP_LINK_ESTABLISHED_EVENT", 0);
      if (autoConnect)
      {
        if (memberInProg != NULL)
		{
  		  if (osal_memcmp((uint8_t *)pAddr, (uint8_t *)memberInProg->addr, B_ADDR_LEN))
  		  {
            //Move the connected member to the tail of the list.
            osal_list_remove(&groupList,(osal_list_elem *)memberInProg);
            osal_list_put(&groupList,(osal_list_elem *)memberInProg);
            //Set the connected bit.;
  		    memberInProg->status |= GROUP_MEMBER_CONNECTED;
            //Store the connection handle.
            memberInProg->connHandle = connHandle;
  		    memberInProg = NULL;
  		  }
  	    }
	  }
      uint8_t  connIndex;
      uint32_t itemsToDisable = SC_ITEM_STOPDISC | SC_ITEM_CANCELCONN;
      uint8_t* pStrAddr;
      uint8_t i;
      uint8_t numConnectable = 0;
      uint8_t pairMode = 0;

      // Add this connection info to the list
      connIndex = SimpleCentral_addConnInfo(connHandle, pAddr);

      // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
      SIMPLECENTRAL_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

      connList[connIndex].charHandle = 0;

      pStrAddr = (uint8_t*) Util_convertBdAddr2Str(connList[connIndex].addr);

      Display_printf(dispHandle, SC_ROW_NON_CONN, 0, "Connected to %s", pStrAddr);
      Display_printf(dispHandle, SC_ROW_NUM_CONN, 0, "Num Conns: %d", numConn);

      // Disable "Connect To" until another discovery is performed
      itemsToDisable |= SC_ITEM_CONNECT;

      // If we already have maximum allowed number of connections,
      // disable device discovery and additional connection making.
      if (numConn >= MAX_NUM_BLE_CONNS)
      {
        itemsToDisable |= SC_ITEM_SCANPHY | SC_ITEM_STARTDISC;
      }

      for (i = 0; i < TBM_GET_NUM_ITEM(&scMenuConnect); i++)
      {
        if (!memcmp(TBM_GET_ACTION_DESC(&scMenuConnect, i), pStrAddr,
            SC_ADDR_STR_SIZE))
        {
          // Disable this device from the connection choices
          tbm_setItemStatus(&scMenuConnect, SC_ITEM_NONE, 1 << i);
        }
        else if (TBM_IS_ITEM_ACTIVE(&scMenuConnect, i))
        {
          numConnectable++;
        }
      }

      // Enable/disable Main menu items properly
      tbm_setItemStatus(&scMenuMain,
                        SC_ITEM_ALL & ~(itemsToDisable), itemsToDisable);

      GAPBondMgr_GetParameter(GAPBOND_PAIRING_MODE, &pairMode);

      if ((autoConnect) && (pairMode != GAPBOND_PAIRING_MODE_INITIATE))
      {
		    SimpleCentral_autoConnect();
      }
      break;
    }

    case GAP_LINK_TERMINATED_EVENT:
    {
	  uint8_t connIndex;
      BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : GAP msg status=%d, opcode=%s\n", 0, "GAP_LINK_TERMINATED_EVENT");
      uint32_t itemsToEnable = SC_ITEM_STARTDISC | SC_ITEM_SCANPHY | SC_ITEM_AUTOCONNECT;
      uint8_t* pStrAddr;
      uint8_t i;
      uint8_t numConnectable = 0;
      uint16_t connHandle = ((gapTerminateLinkEvent_t*) pMsg)->connectionHandle;
      if (autoConnect)
      {
        groupListElem_t *tempMember;
        //Traverse from tail to head because of the sorting which put the connected at the end of the list.
		for (tempMember = (groupListElem_t *)osal_list_tail(&groupList); tempMember != NULL; tempMember = (groupListElem_t *)osal_list_prev((osal_list_elem *)tempMember)) 
        {
          if (tempMember->connHandle == connHandle)
          {
            //Move disconnected member to the head of the list for next connection.
            osal_list_remove(&groupList,(osal_list_elem *)tempMember);
            osal_list_putHead(&groupList,(osal_list_elem *)tempMember);
            //Clear the connected flag.
            tempMember->status &= ~GROUP_MEMBER_CONNECTED;
            //Clear the connnection handle.
            tempMember->connHandle = GROUP_INITIALIZED_CONNECTION_HANDLE;   
          }
        }
      }
      // Cancel timers
      SimpleCentral_CancelRssi(connHandle);
	  
	  // Mark this connection deleted in the connected device list.
      connIndex = SimpleCentral_removeConnInfo(connHandle);
      if (autoConnect)
      {
	    SimpleCentral_autoConnect();
      }
      // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
      SIMPLECENTRAL_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

      pStrAddr = (uint8_t*) Util_convertBdAddr2Str(connList[connIndex].addr);

      Display_printf(dispHandle, SC_ROW_NON_CONN, 0, "%s is disconnected",
                     pStrAddr);
      Display_printf(dispHandle, SC_ROW_NUM_CONN, 0, "Num Conns: %d", numConn);

      for (i = 0; i < TBM_GET_NUM_ITEM(&scMenuConnect); i++)
      {
        if (!memcmp(TBM_GET_ACTION_DESC(&scMenuConnect, i), pStrAddr,
                     SC_ADDR_STR_SIZE))
        {
          // Enable this device in the connection choices
          tbm_setItemStatus(&scMenuConnect, 1 << i, SC_ITEM_NONE);
        }

        if (TBM_IS_ITEM_ACTIVE(&scMenuConnect, i))
        {
          numConnectable++;
        }
      }

      if (numConn > 0)
      {
        // There still is an active connection to select
        itemsToEnable |= SC_ITEM_SELECTCONN;
      }

      // Enable/disable items properly.
      tbm_setItemStatus(&scMenuMain,
                        itemsToEnable, SC_ITEM_ALL & ~itemsToEnable);

      // If we are in the context which the teminated connection was associated
      // with, go to main menu.
      if (connHandle == scConnHandle)
      {
        tbm_goTo(&scMenuMain);
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

	  if (autoConnect)
      {
        SimpleCentral_autoConnect();
      }

      break;
    }

    case GAP_LINK_PARAM_UPDATE_EVENT:
    {
      gapLinkUpdateEvent_t *pPkt = (gapLinkUpdateEvent_t *)pMsg;
      // Get the address from the connection handle
      linkDBInfo_t linkInfo;

      BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : GAP msg status=%d, opcode=%s\n", 0, "GAP_LINK_PARAM_UPDATE_EVENT");
      if (linkDB_GetInfo(pPkt->connectionHandle, &linkInfo) ==  SUCCESS)
      {
        if(pPkt->status == SUCCESS)
        {
          Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                         "Updated: %s, connTimeout:%d",
                         Util_convertBdAddr2Str(linkInfo.addr),
                         linkInfo.connTimeout*CONN_TIMEOUT_MS_CONVERSION);
        }
        else
        {
          // Display the address of the connection update failure
          Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                         "Update Failed 0x%h: %s", pPkt->opcode,
                         Util_convertBdAddr2Str(linkInfo.addr));
        }
      }
      
      if (autoConnect)
      {
        SimpleCentral_autoConnect();
      }

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
      Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                     "Peer Device's Update Request Rejected 0x%h: %s", pPkt->opcode,
                     Util_convertBdAddr2Str(linkInfo.addr));

      break;
    }
#endif

    default:
      break;
  }
}

/*********************************************************************
 * @fn      SimpleCentral_handleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   keys - bit field for key events. Valid entries:
 *                 KEY_LEFT
 *                 KEY_RIGHT
 *
 * @return  none
 */
static void SimpleCentral_handleKeys(uint8_t keys)
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
/*********************************************************************
 * @fn      SimpleCentral_processGATTMsg
 *
 * @brief   Process GATT messages and events.
 *
 * @return  none
 */
static void SimpleCentral_processGATTMsg(gattMsgEvent_t *pMsg)
{
  if (linkDB_Up(pMsg->connHandle))
  {
    // See if GATT server was unable to transmit an ATT response
    if (pMsg->hdr.status == blePending)
    {
      // No HCI buffer was available. App can try to retransmit the response
      // on the next connection event. Drop it for now.
      Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                     "ATT Rsp dropped %d", pMsg->method);
    }
    else if ((pMsg->method == ATT_READ_RSP)   ||
             ((pMsg->method == ATT_ERROR_RSP) &&
              (pMsg->msg.errorRsp.reqOpcode == ATT_READ_REQ)))
    {
      if (pMsg->method == ATT_ERROR_RSP)
      {
        Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                       "Read Error %d", pMsg->msg.errorRsp.errCode);
      }
      else
      {
        // After a successful read, display the read value
        Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                       "Read rsp: 0x%02x", pMsg->msg.readRsp.pValue[0]);
      }
    }
    else if ((pMsg->method == ATT_WRITE_RSP)  ||
             ((pMsg->method == ATT_ERROR_RSP) &&
              (pMsg->msg.errorRsp.reqOpcode == ATT_WRITE_REQ)))
    {
      if (pMsg->method == ATT_ERROR_RSP)
      {
        Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                       "Write Error %d", pMsg->msg.errorRsp.errCode);
      }
      else
      {
        // After a successful write, display the value that was written and
        // increment value
        Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                       "Write sent: 0x%02x", charVal);
      }

      tbm_goTo(&scMenuPerConn);
    }
    else if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
    {
      // ATT request-response or indication-confirmation flow control is
      // violated. All subsequent ATT requests or indications will be dropped.
      // The app is informed in case it wants to drop the connection.

      // Display the opcode of the message that caused the violation.
      Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                     "FC Violated: %d", pMsg->msg.flowCtrlEvt.opcode);
    }
    else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
    {
      // MTU size updated
      Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                     "MTU Size: %d", pMsg->msg.mtuEvt.MTU);
    }
    else if (discState != BLE_DISC_STATE_IDLE)
    {
      SimpleCentral_processGATTDiscEvent(pMsg);
    }
  } // else - in case a GATT message came after a connection has dropped, ignore it.

  // Needed only for ATT Protocol messages
  GATT_bm_free(&pMsg->msg, pMsg->method);
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
static void SimpleCentral_processCmdCompleteEvt(hciEvt_CmdComplete_t *pMsg)
{
  switch (pMsg->cmdOpcode)
  {
    case HCI_READ_RSSI:
    {
#ifndef Display_DISABLE_ALL
      uint16_t connHandle = BUILD_UINT16(pMsg->pReturnParam[1],
                                         pMsg->pReturnParam[2]);
      int8 rssi = (int8)pMsg->pReturnParam[3];
      
	  Display_printf(dispHandle, SC_ROW_ANY_CONN, 0, "%s: RSSI %d dBm",
                   SimpleCentral_getConnAddrStr(connHandle), rssi);

#endif
      break;
    }
    default:
      break;
  }
}

/*********************************************************************
 * @fn      SimpleCentral_StartRssi
 *
 * @brief   Start periodic RSSI reads on the current link.
 *
 * @return  SUCCESS: RSSI Read timer started
 *          bleIncorrectMode: Aready started
 *          bleNoResources: No resources
 */
static status_t SimpleCentral_StartRssi(void)
{
  uint8_t connIndex = SimpleCentral_getConnIndex(scConnHandle);

  // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
  SIMPLECENTRAL_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

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
                        SimpleCentral_clockHandler,
                        DEFAULT_RSSI_PERIOD, 0, true,
                        (connIndex << 8) | SC_EVT_READ_RSSI);
  }
  else
  {
    return bleNoResources;
  }

  return SUCCESS;
}

/*********************************************************************
 * @fn      SimpleCentral_CancelRssi
 *
 * @brief   Cancel periodic RSSI reads on a link.
 *
 * @param   connection handle
 *
 * @return  SUCCESS: Operation successful
 *          bleIncorrectMode: Has not started
 */
static status_t SimpleCentral_CancelRssi(uint16_t connHandle)
{
  uint8_t connIndex = SimpleCentral_getConnIndex(connHandle);

  // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
  SIMPLECENTRAL_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

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

  Display_clearLine(dispHandle, SC_ROW_ANY_CONN);

  return SUCCESS;
}

/*********************************************************************
 * @fn      SimpleCentral_processPairState
 *
 * @brief   Process the new paring state.
 *
 * @return  none
 */
static void SimpleCentral_processPairState(uint8_t state,
                                           scPairStateData_t* pPairData)
{
  uint8_t status = pPairData->status;
  uint8_t pairMode = 0;

  if (state == GAPBOND_PAIRING_STATE_STARTED)
  {
    Display_printf(dispHandle, SC_ROW_CUR_CONN, 0, "Pairing started");
  }
  else if (state == GAPBOND_PAIRING_STATE_COMPLETE)
  {
    if (status == SUCCESS)
    {
      linkDBInfo_t linkInfo;

      Display_printf(dispHandle, SC_ROW_CUR_CONN, 0, "Pairing success");

      if (linkDB_GetInfo(pPairData->connHandle, &linkInfo) == SUCCESS)
      {
        // If the peer was using private address, update with ID address
        if ((linkInfo.addrType == ADDRTYPE_PUBLIC_ID ||
             linkInfo.addrType == ADDRTYPE_RANDOM_ID) &&
             !Util_isBufSet(linkInfo.addrPriv, 0, B_ADDR_LEN))
        {
          // Update the address of the peer to the ID address
          Display_printf(dispHandle, SC_ROW_NON_CONN, 0, "Addr updated: %s",
                         Util_convertBdAddr2Str(linkInfo.addr));

          // Update the connection list with the ID address
          uint8_t i = SimpleCentral_getConnIndex(pPairData->connHandle);

          SIMPLECENTRAL_ASSERT(i < MAX_NUM_BLE_CONNS);
          memcpy(connList[i].addr, linkInfo.addr, B_ADDR_LEN);
        }
      }
    }
    else
    {
      Display_printf(dispHandle, SC_ROW_CUR_CONN, 0, "Pairing fail: %d", status);
    }

    GAPBondMgr_GetParameter(GAPBOND_PAIRING_MODE, &pairMode);

    if ((autoConnect) && (pairMode == GAPBOND_PAIRING_MODE_INITIATE))
    {
	  SimpleCentral_autoConnect();
    }
  }
  else if (state == GAPBOND_PAIRING_STATE_ENCRYPTED)
  {
    if (status == SUCCESS)
    {
      Display_printf(dispHandle, SC_ROW_CUR_CONN, 0, "Encryption success");
    }
    else
    {
      Display_printf(dispHandle, SC_ROW_CUR_CONN, 0, "Encryption failed: %d", status);
    }

    GAPBondMgr_GetParameter(GAPBOND_PAIRING_MODE, &pairMode);

    if ((autoConnect) && (pairMode == GAPBOND_PAIRING_MODE_INITIATE))
    {
      SimpleCentral_autoConnect();
    }
  }
  else if (state == GAPBOND_PAIRING_STATE_BOND_SAVED)
  {
    if (status == SUCCESS)
    {
      Display_printf(dispHandle, SC_ROW_CUR_CONN, 0, "Bond save success");
    }
    else
    {
      Display_printf(dispHandle, SC_ROW_CUR_CONN, 0, "Bond save failed: %d", status);
    }
  }
}

/*********************************************************************
 * @fn      SimpleCentral_processPasscode
 *
 * @brief   Process the Passcode request.
 *
 * @return  none
 */
static void SimpleCentral_processPasscode(scPasscodeData_t *pData)
{
  // Display passcode to user
  if (pData->uiOutputs != 0)
  {
    Display_printf(dispHandle, SC_ROW_CUR_CONN, 0, "Passcode: %d",
                   B_APP_DEFAULT_PASSCODE);
  }

  // Send passcode response
  GAPBondMgr_PasscodeRsp(pData->connHandle, SUCCESS, B_APP_DEFAULT_PASSCODE);
}

/*********************************************************************
 * @fn      SimpleCentral_startSvcDiscovery
 *
 * @brief   Start service discovery.
 *
 * @return  none
 */
static void SimpleCentral_startSvcDiscovery(void)
{
  attExchangeMTUReq_t req;

  // Initialize cached handles
  svcStartHdl = svcEndHdl = 0;

  discState = BLE_DISC_STATE_MTU;

  // Discover GATT Server's Rx MTU size
  req.clientRxMTU = scMaxPduSize - L2CAP_HDR_SIZE;

  // ATT MTU size should be set to the minimum of the Client Rx MTU
  // and Server Rx MTU values
  VOID GATT_ExchangeMTU(scConnHandle, &req, selfEntity);
}

/*********************************************************************
 * @fn      SimpleCentral_processGATTDiscEvent
 *
 * @brief   Process GATT discovery event
 *
 * @return  none
 */
static void SimpleCentral_processGATTDiscEvent(gattMsgEvent_t *pMsg)
{
  if (discState == BLE_DISC_STATE_MTU)
  {
    // MTU size response received, discover simple service
    if (pMsg->method == ATT_EXCHANGE_MTU_RSP)
    {
      uint8_t uuid[ATT_BT_UUID_SIZE] = { LO_UINT16(SIMPLEPROFILE_SERV_UUID),
                                         HI_UINT16(SIMPLEPROFILE_SERV_UUID) };

      discState = BLE_DISC_STATE_SVC;

      // Discovery simple service
      VOID GATT_DiscPrimaryServiceByUUID(pMsg->connHandle, uuid,
                                         ATT_BT_UUID_SIZE, selfEntity);
    }
  }
  else if (discState == BLE_DISC_STATE_SVC)
  {
    // Service found, store handles
    if (pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP &&
        pMsg->msg.findByTypeValueRsp.numInfo > 0)
    {
      svcStartHdl = ATT_ATTR_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0);
      svcEndHdl = ATT_GRP_END_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0);
    }

    // If procedure complete
    if (((pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP) &&
         (pMsg->hdr.status == bleProcedureComplete))  ||
        (pMsg->method == ATT_ERROR_RSP))
    {
      if (svcStartHdl != 0)
      {
        attReadByTypeReq_t req;

        // Discover characteristic
        discState = BLE_DISC_STATE_CHAR;

        req.startHandle = svcStartHdl;
        req.endHandle = svcEndHdl;
        req.type.len = ATT_BT_UUID_SIZE;
        req.type.uuid[0] = LO_UINT16(SIMPLEPROFILE_CHAR1_UUID);
        req.type.uuid[1] = HI_UINT16(SIMPLEPROFILE_CHAR1_UUID);

        VOID GATT_DiscCharsByUUID(pMsg->connHandle, &req, selfEntity);
      }
    }
  }
  else if (discState == BLE_DISC_STATE_CHAR)
  {
    // Characteristic found, store handle
    if ((pMsg->method == ATT_READ_BY_TYPE_RSP) &&
        (pMsg->msg.readByTypeRsp.numPairs > 0))
    {
      uint8_t connIndex = SimpleCentral_getConnIndex(scConnHandle);

      // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
      SIMPLECENTRAL_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

 	  // Store the handle of the simpleprofile characteristic 1 value
      connList[connIndex].charHandle
        = BUILD_UINT16(pMsg->msg.readByTypeRsp.pDataList[3],
                       pMsg->msg.readByTypeRsp.pDataList[4]);

      Display_printf(dispHandle, SC_ROW_CUR_CONN, 0, "Simple Svc Found");

      // Now we can use GATT Read/Write
      tbm_setItemStatus(&scMenuPerConn,
                        SC_ITEM_GATTREAD | SC_ITEM_GATTWRITE, SC_ITEM_NONE);
    }

    discState = BLE_DISC_STATE_IDLE;
  }
}

#if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
/*********************************************************************
 * @fn      SimpleCentral_findSvcUuid
 *
 * @brief   Find a given UUID in an advertiser's service UUID list.
 *
 * @return  TRUE if service UUID found
 */
static bool SimpleCentral_findSvcUuid(uint16_t uuid, uint8_t *pData,
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

/*********************************************************************
 * @fn      SimpleCentral_addScanInfo
 *
 * @brief   Add a device to the scanned device list
 *
 * @return  none
 */
static void SimpleCentral_addScanInfo(uint8_t *pAddr, uint8_t addrType)
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

    // Increment scan result count
    numScanRes++;
  }
}
#endif // DEFAULT_DEV_DISC_BY_SVC_UUID

/*********************************************************************
 * @fn      SimpleCentral_addConnInfo
 *
 * @brief   Add a device to the connected device list
 *
 * @return  index of the connected device list entry where the new connection
 *          info is put in.
 *          if there is no room, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t SimpleCentral_addConnInfo(uint16_t connHandle, uint8_t *pAddr)
{
  uint8_t i;

  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == LINKDB_CONNHANDLE_INVALID)
    {
      // Found available entry to put a new connection info in
      connList[i].connHandle = connHandle;
      memcpy(connList[i].addr, pAddr, B_ADDR_LEN);
      numConn++;

      break;
    }
  }

  return i;
}

/*********************************************************************
 * @fn      SimpleCentral_removeConnInfo
 *
 * @brief   Remove a device from the connected device list
 *
 * @return  index of the connected device list entry where the new connection
 *          info is removed from.
 *          if connHandle is not found, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t SimpleCentral_removeConnInfo(uint16_t connHandle)
{
  uint8_t i;

  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == connHandle)
    {
      // Found the entry to mark as deleted
      connList[i].connHandle = LINKDB_CONNHANDLE_INVALID;
      numConn--;

      break;
    }
  }

  return i;
}

/*********************************************************************
 * @fn      SimpleCentral_getConnIndex
 *
 * @brief   Find index in the connected device list by connHandle
 *
 * @return  the index of the entry that has the given connection handle.
 *          if there is no match, MAX_NUM_BLE_CONNS will be returned.
 */
static uint8_t SimpleCentral_getConnIndex(uint16_t connHandle)
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

#ifndef Display_DISABLE_ALL
/*********************************************************************
 * @fn      SimpleCentral_getConnAddrStr
 *
 * @brief   Return, in string form, the address of the peer associated with
 *          the connHandle.
 *
 * @return  A null-terminated string of the address.
 *          if there is no match, NULL will be returned.
 */
static char* SimpleCentral_getConnAddrStr(uint16_t connHandle)
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
#endif

/*********************************************************************
 * @fn      SimpleCentral_pairStateCb
 *
 * @brief   Pairing state callback.
 *
 * @return  none
 */
static void SimpleCentral_pairStateCb(uint16_t connHandle, uint8_t state,
                                      uint8_t status)
{
  scPairStateData_t *pData;

  // Allocate space for the event data.
  if ((pData = ICall_malloc(sizeof(scPairStateData_t))))
  {
    pData->connHandle = connHandle;
    pData->status = status;

    // Queue the event.
    if(SimpleCentral_enqueueMsg(SC_EVT_PAIR_STATE, state, (uint8_t*) pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
* @fn      SimpleCentral_passcodeCb
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
static void SimpleCentral_passcodeCb(uint8_t *deviceAddr, uint16_t connHandle,
                                  uint8_t uiInputs, uint8_t uiOutputs,
                                  uint32_t numComparison)
{
  scPasscodeData_t *pData = ICall_malloc(sizeof(scPasscodeData_t));

  // Allocate space for the passcode event.
  if (pData)
  {
    pData->connHandle = connHandle;
    memcpy(pData->deviceAddr, deviceAddr, B_ADDR_LEN);
    pData->uiInputs = uiInputs;
    pData->uiOutputs = uiOutputs;
    pData->numComparison = numComparison;

    // Enqueue the event.
    if (SimpleCentral_enqueueMsg(SC_EVT_PASSCODE_NEEDED, 0,(uint8_t *) pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      SimpleCentral_keyChangeHandler
 *
 * @brief   Key event handler function
 *
 * @param   a0 - ignored
 *
 * @return  none
 */
static void SimpleCentral_keyChangeHandler(uint8 keys)
{
  SimpleCentral_enqueueMsg(SC_EVT_KEY_CHANGE, keys, NULL);
}

/*********************************************************************
 * @fn      SimpleCentral_clockHandler
 *
 * @brief   clock handler function
 *
 * @param   arg - argument from the clock initiator
 *
 * @return  none
 */
void SimpleCentral_clockHandler(UArg arg)
{
  uint8_t evtId = (uint8_t) (arg & 0xFF);

  switch (evtId)
  {
    case SC_EVT_READ_RSSI:
      SimpleCentral_enqueueMsg(SC_EVT_READ_RSSI, (uint8_t) (arg >> 8) , NULL);
      break;

    case SC_EVT_READ_RPA:
      // Restart timer
      Util_startClock(&clkRpaRead);
      // Let the application handle the event
      SimpleCentral_enqueueMsg(SC_EVT_READ_RPA, 0, NULL);
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      SimpleCentral_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 * @param   pData - message data pointer.
 *
 * @return  TRUE or FALSE
 */
static status_t SimpleCentral_enqueueMsg(uint8_t event, uint8_t state,
                                           uint8_t *pData)
{
  uint8_t success;
  scEvt_t *pMsg = ICall_malloc(sizeof(scEvt_t));

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
 * @fn      SimpleCentral_scanCb
 *
 * @brief   Callback called by GapScan module
 *
 * @param   evt - event
 * @param   msg - message coming with the event
 * @param   arg - user argument
 *
 * @return  none
 */
void SimpleCentral_scanCb(uint32_t evt, void* pMsg, uintptr_t arg)
{
  uint8_t event;

  if (evt & GAP_EVT_ADV_REPORT)
  {
    event = SC_EVT_ADV_REPORT;
  }
  else if (evt & GAP_EVT_SCAN_ENABLED)
  {
    event = SC_EVT_SCAN_ENABLED;
  }
  else if (evt & GAP_EVT_SCAN_DISABLED)
  {
    event = SC_EVT_SCAN_DISABLED;
  }
  else if (evt & GAP_EVT_INSUFFICIENT_MEMORY)
  {
    event = SC_EVT_INSUFFICIENT_MEM;
  }
  else
  {
    return;
  }

  if(SimpleCentral_enqueueMsg(event, SUCCESS, pMsg) != SUCCESS)
  {
    ICall_free(pMsg);
  }
}

/*********************************************************************
 * @fn      SimpleCentral_doAutoConnect
 *
 * @brief   Enable/Disable AutoConnect.
 *
 * @param   index - 0 : Disable AutoConnect
 *                  1 : Enable Group A
 *                  2 : Enable Group B
 *
 * @return  always true
 */
bool SimpleCentral_doAutoConnect(uint8_t index)
{
    if (index == 1)
    {
      if ((autoConnect) && (autoConnect != AUTOCONNECT_GROUP_A))
      {
        groupListElem_t *tempMember;
        //Traverse list to search if advertiser already in list.
        for (tempMember = (groupListElem_t *)osal_list_head(&groupList); tempMember != NULL; tempMember = (groupListElem_t *)osal_list_next((osal_list_elem *)tempMember)) 
        {
          osal_list_remove(&groupList,(osal_list_elem *)tempMember);
          ICall_free(tempMember);
        }
		numGroupMembers = 0;
      }	
	  Display_printf(dispHandle, SC_ROW_AC, 0, "AutoConnect enabled: Group A");
      autoConnect = AUTOCONNECT_GROUP_A;
      acGroup[3] = 'A';
    }
    else if (index == 2)
    {
      if ((autoConnect) && (autoConnect != AUTOCONNECT_GROUP_B))
      {
        groupListElem_t *tempMember;
        //Traverse list to search if advertiser already in list.
        for (tempMember = (groupListElem_t *)osal_list_head(&groupList); tempMember != NULL; tempMember = (groupListElem_t *)osal_list_next((osal_list_elem *)tempMember)) 
        {
          osal_list_remove(&groupList,(osal_list_elem *)tempMember);
          ICall_free(tempMember);
        }
		numGroupMembers = 0;
      }
      Display_printf(dispHandle, SC_ROW_AC, 0, "AutoConnect enabled: Group B");
      autoConnect = AUTOCONNECT_GROUP_B;
      acGroup[3] = 'B';
    }
    else
    {
      autoConnect = AUTOCONNECT_DISABLE;
      groupListElem_t *tempMember;
      //Traverse list to search if advertiser already in list.
      for (tempMember = (groupListElem_t *)osal_list_head(&groupList); tempMember != NULL; tempMember = (groupListElem_t *)osal_list_next((osal_list_elem *)tempMember)) 
      {
        osal_list_remove(&groupList,(osal_list_elem *)tempMember);
        ICall_free(tempMember);
      }
	  numGroupMembers = 0;
      Display_printf(dispHandle, SC_ROW_AC, 0, "AutoConnect disabled");
    }
    if ((autoConnect) && (MAX_NUM_BLE_CONNS > 8))
    {
	  //Disable accepting L2CAP param upadte request
      acceptParamUpdateReq = false;
	  //Disable all parameter update requests
	  GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, GAP_UPDATE_REQ_DENY_ALL);
	  //Set connection interval and supervision timeout
      GapInit_setPhyParam(INIT_PHY_1M | INIT_PHY_2M | INIT_PHY_CODED,INIT_PHYPARAM_CONN_INT_MAX,DEFAULT_MULTICON_INTERVAL);
      GapInit_setPhyParam(INIT_PHY_1M | INIT_PHY_2M | INIT_PHY_CODED,INIT_PHYPARAM_CONN_INT_MIN,DEFAULT_MULTICON_INTERVAL);
	  GapInit_setPhyParam(INIT_PHY_1M | INIT_PHY_2M | INIT_PHY_CODED,INIT_PHYPARAM_SUP_TIMEOUT,DEFAULT_MULTICON_LSTO);
    }
 
    tbm_goTo(&scMenuMain);
    
    return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_doSetScanPhy
 *
 * @brief   Set PHYs for scanning.
 *
 * @param   index - 0: 1M PHY
 *                  1: CODED PHY (Long range)
 *
 * @return  always true
 */
bool SimpleCentral_doSetScanPhy(uint8_t index)
{
  uint8_t temp8;

  if (index == 0)
  {
    temp8 = SCAN_PRIM_PHY_1M;
  }
  else
  {
    temp8 = SCAN_PRIM_PHY_CODED;
  }

  // Set scanning primary PHY
  GapScan_setParam(SCAN_PARAM_PRIM_PHYS, &temp8);

  Display_printf(dispHandle, SC_ROW_NON_CONN, 0, "Primary Scan PHY: %s",
                 TBM_GET_ACTION_DESC(&scMenuScanPhy, index));

  tbm_goTo(&scMenuMain);

  return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_doDiscoverDevices
 *
 * @brief   Enables scanning
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
bool SimpleCentral_doDiscoverDevices(uint8_t index)
{
  (void) index;

#if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
  // Scanning for DEFAULT_SCAN_DURATION x 10 ms.
  // The stack does not need to record advertising reports
  // since the application will filter them by Service UUID and save.
  // Reset number of scan results to 0 before starting scan
  numScanRes = 0;
  GapScan_enable(0, DEFAULT_SCAN_DURATION, 0);
#else // !DEFAULT_DEV_DISC_BY_SVC_UUID
  // Scanning for DEFAULT_SCAN_DURATION x 10 ms.
  // Let the stack record the advertising reports as many as up to DEFAULT_MAX_SCAN_RES.
  GapScan_enable(0, DEFAULT_SCAN_DURATION, DEFAULT_MAX_SCAN_RES);
#endif // DEFAULT_DEV_DISC_BY_SVC_UUID

  // Enable only "Stop Discovering" and disable all others in the main menu
  tbm_setItemStatus(&scMenuMain, SC_ITEM_STOPDISC,
                    (SC_ITEM_ALL & ~SC_ITEM_STOPDISC));

  return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_doStopDiscovering
 *
 * @brief   Stop on-going scanning
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
bool SimpleCentral_doStopDiscovering(uint8_t index)
{
  (void) index;

  GapScan_disable("");

  return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_doEstablishLink
 *
 * @brief   Establish a link to a peer device
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
bool SimpleCentral_doConnect(uint8_t index)
{
#if (DEFAULT_DEV_DISC_BY_SVC_UUID == TRUE)
  GapInit_connect(scanList[index].addrType & MASK_ADDRTYPE_ID,
                  scanList[index].addr, DEFAULT_INIT_PHY, 0);
#else // !DEFAULT_DEV_DISC_BY_SVC_UUID
  GapScan_Evt_AdvRpt_t advRpt;

  GapScan_getAdvReport(index, &advRpt);

  GapInit_connect(advRpt.addrType & MASK_ADDRTYPE_ID,
                  advRpt.addr, DEFAULT_INIT_PHY, 0);
#endif // DEFAULT_DEV_DISC_BY_SVC_UUID

  // Enable only "Cancel Connecting" and disable all others in the main menu
  tbm_setItemStatus(&scMenuMain, SC_ITEM_CANCELCONN,
                    (SC_ITEM_ALL & ~SC_ITEM_CANCELCONN));

  Display_printf(dispHandle, SC_ROW_NON_CONN, 0, "Connecting...");

  tbm_goTo(&scMenuMain);

  return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_doCancelConnecting
 *
 * @brief   Cancel on-going connection attempt
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
bool SimpleCentral_doCancelConnecting(uint8_t index)
{
  (void) index;

  GapInit_cancelConnect("");

  return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_doSelectConn
 *
 * @brief   Select a connection to communicate with
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
bool SimpleCentral_doSelectConn(uint8_t index)
{
  uint32_t itemsToDisable = SC_ITEM_NONE;

  // index cannot be equal to or greater than MAX_NUM_BLE_CONNS
  SIMPLECENTRAL_ASSERT(index < MAX_NUM_BLE_CONNS);

  scConnHandle = connList[index].connHandle;

  if (connList[index].charHandle == 0)
  {
    // Initiate service discovery
    SimpleCentral_enqueueMsg(SC_EVT_SVC_DISC, 0, NULL);

    // Diable GATT Read/Write until simple service is found
    itemsToDisable = SC_ITEM_GATTREAD | SC_ITEM_GATTWRITE;
  }

  // Set the menu title and go to this connection's context
  TBM_SET_TITLE(&scMenuPerConn, TBM_GET_ACTION_DESC(&scMenuSelectConn, index));

  // Set RSSI items properly depending on current state
  if (connList[index].pRssiClock == NULL)
  {
    tbm_setItemStatus(&scMenuPerConn,
                      SC_ITEM_STRTRSSI, SC_ITEM_STOPRSSI | itemsToDisable);
  }
  else
  {
    tbm_setItemStatus(&scMenuPerConn,
                      SC_ITEM_STOPRSSI, SC_ITEM_STRTRSSI | itemsToDisable);
  }

  // Clear non-connection-related message
  Display_clearLine(dispHandle, SC_ROW_NON_CONN);

  tbm_goTo(&scMenuPerConn);

  return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_doGattRead
 *
 * @brief   GATT Read
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
bool SimpleCentral_doGattRead(uint8_t index)
{
  attReadReq_t req;
  uint8_t connIndex = SimpleCentral_getConnIndex(scConnHandle);

  // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
  SIMPLECENTRAL_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  req.handle = connList[connIndex].charHandle;
  GATT_ReadCharValue(scConnHandle, &req, selfEntity);

  return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_doGattWrite
 *
 * @brief   GATT Write
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
bool SimpleCentral_doGattWrite(uint8_t index)
{
  status_t status;
  uint8_t charVals[4] = { 0x00, 0x55, 0xAA, 0xFF }; // Should be consistent with
                                                    // those in scMenuGattWrite

  attWriteReq_t req;

  req.pValue = GATT_bm_alloc(scConnHandle, ATT_WRITE_REQ, 1, NULL);

  if ( req.pValue != NULL )
  {
    uint8_t connIndex = SimpleCentral_getConnIndex(scConnHandle);

    // connIndex cannot be equal to or greater than MAX_NUM_BLE_CONNS
    SIMPLECENTRAL_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

    req.handle = connList[connIndex].charHandle;
    req.len = 1;
    charVal = charVals[index];    
    req.pValue[0] = charVal;
    req.sig = 0;
    req.cmd = 0;

    status = GATT_WriteCharValue(scConnHandle, &req, selfEntity);
    if ( status != SUCCESS )
    {
      GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ);
    }
  }

  return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_doRssiRead
 *
 * @brief   Toggle RSSI Read
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
bool SimpleCentral_doRssiRead(uint8_t index)
{
  status_t status;

  if ((1 << index) == SC_ITEM_STRTRSSI)
  {
    if ((status = SimpleCentral_StartRssi()) == SUCCESS)
    {
      tbm_setItemStatus(&scMenuPerConn, SC_ITEM_STOPRSSI, SC_ITEM_STRTRSSI);
    }
  }
  else // SC_ITEM_STOP_RSSI
  {
    if ((status = SimpleCentral_CancelRssi(scConnHandle)) == SUCCESS)
    {
      tbm_setItemStatus(&scMenuPerConn, SC_ITEM_STRTRSSI, SC_ITEM_STOPRSSI);
    }
  }

  return ((status == SUCCESS) ? true : false);
}

/*********************************************************************
 * @fn      SimpleCentral_doConnUpdate
 *
 * @brief   Initiate Connection Update procedure
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
bool SimpleCentral_doConnUpdate(uint8_t index)
{
  gapUpdateLinkParamReq_t params;

  (void) index;

  params.connectionHandle = scConnHandle;
  params.intervalMin = DEFAULT_UPDATE_MIN_CONN_INTERVAL;
  params.intervalMax = DEFAULT_UPDATE_MAX_CONN_INTERVAL;
  params.connLatency = DEFAULT_UPDATE_SLAVE_LATENCY;

  linkDBInfo_t linkInfo;
  if (linkDB_GetInfo(scConnHandle, &linkInfo) == SUCCESS)
  {
    if (linkInfo.connTimeout == DEFAULT_UPDATE_CONN_TIMEOUT)
    {
      params.connTimeout = DEFAULT_UPDATE_CONN_TIMEOUT + 200;
    }
    else
    {
      params.connTimeout = DEFAULT_UPDATE_CONN_TIMEOUT;
    }
    GAP_UpdateLinkParamReq(&params);

    Display_printf(dispHandle, SC_ROW_CUR_CONN, 0, "Param update Request:connTimeout =%d",
                    params.connTimeout*CONN_TIMEOUT_MS_CONVERSION);
  }
  else
  {
    Display_printf(dispHandle, SC_ROW_CUR_CONN, 0,
                   "update :%s, Unable to find link information",
                    Util_convertBdAddr2Str(linkInfo.addr));
  }
  return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_doSetConnPhy
 *
 * @brief   Set Connection PHY preference.
 *
 * @param   index - 0: 1M PHY
 *                  1: 2M PHY
 *                  2: 1M + 2M PHY
 *                  3: CODED PHY (Long range)
 *                  4: 1M + 2M + CODED PHY
 *
 * @return  always true
 */
bool SimpleCentral_doSetConnPhy(uint8_t index)
{
  static uint8_t phy[] = {
    HCI_PHY_1_MBPS, HCI_PHY_2_MBPS, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS,
    HCI_PHY_CODED, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS | HCI_PHY_CODED,
  };

  // Set Phy Preference on the current connection. Apply the same value
  // for RX and TX. For more information, see the LE 2M PHY section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  // Note PHYs are already enabled by default in build_config.opt in stack project.
  HCI_LE_SetPhyCmd(scConnHandle, 0, phy[index], phy[index], 0);

  Display_printf(dispHandle, SC_ROW_CUR_CONN, 0, "PHY preference: %s",
                 TBM_GET_ACTION_DESC(&scMenuConnPhy, index));

  return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_doDisconnect
 *
 * @brief   Disconnect the specified link
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
bool SimpleCentral_doDisconnect(uint8_t index)
{
  (void) index;

  GAP_TerminateLinkReq(scConnHandle, HCI_DISCONNECT_REMOTE_USER_TERM);

  return (true);
}

/*********************************************************************
 * @fn      SimpleCentral_menuSwitchCb
 *
 * @brief   Detect menu context switching
 *
 * @param   pMenuObjCurr - the current menu object
 * @param   pMenuObjNext - the menu object the context is about to switch to
 *
 * @return  none
 */
static void SimpleCentral_menuSwitchCb(tbmMenuObj_t* pMenuObjCurr,
                                       tbmMenuObj_t* pMenuObjNext)
{
  uint8_t i, j;

  // interested in only the events of
  // entering scMenuConnect, scMenuSelectConn, and scMenuMain for now
  if (pMenuObjNext == &scMenuConnect)
  {
    
    uint32_t itemsToDisable = SC_ITEM_NONE;

    for (i = 0; i < TBM_GET_NUM_ITEM(&scMenuConnect); i++)
    {
      for (j = 0; j < MAX_NUM_BLE_CONNS; j++)
      {
        if ((connList[j].connHandle != LINKDB_CONNHANDLE_INVALID) &&
            !memcmp(TBM_GET_ACTION_DESC(&scMenuConnect, i),
                    Util_convertBdAddr2Str(connList[j].addr),
                    SC_ADDR_STR_SIZE))
        {
          // Already connected. Add to the set to be disabled.
          itemsToDisable |= (1 << i);
        }
      }
    }

    // Eventually only non-connected device addresses will be displayed.
    tbm_setItemStatus(&scMenuConnect,
                      SC_ITEM_ALL & ~itemsToDisable, itemsToDisable);
  }
  else if (pMenuObjNext == &scMenuSelectConn)
  {
    static uint8_t* pAddrs;
    uint8_t* pAddrTemp;

    if (pAddrs != NULL)
    {
      ICall_free(pAddrs);
    }

    // Allocate buffer to display addresses
    pAddrs = ICall_malloc(numConn * SC_ADDR_STR_SIZE);

    if (pAddrs == NULL)
    {
      TBM_SET_NUM_ITEM(&scMenuSelectConn, 0);
    }
    else
    {
      TBM_SET_NUM_ITEM(&scMenuSelectConn, MAX_NUM_BLE_CONNS);

      pAddrTemp = pAddrs;

      // Add active connection info to the menu object
      for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
      {
        if (connList[i].connHandle != LINKDB_CONNHANDLE_INVALID)
        {
          // This connection is active. Set the corresponding menu item with
          // the address of this connection and enable the item.
          memcpy(pAddrTemp, Util_convertBdAddr2Str(connList[i].addr),
                 SC_ADDR_STR_SIZE);
          TBM_SET_ACTION_DESC(&scMenuSelectConn, i, pAddrTemp);
          tbm_setItemStatus(&scMenuSelectConn, (1 << i), SC_ITEM_NONE);
          pAddrTemp += SC_ADDR_STR_SIZE;
        }
        else
        {
          // This connection is not active. Disable the corresponding menu item.
          tbm_setItemStatus(&scMenuSelectConn, SC_ITEM_NONE, (1 << i));
        }
      }
    }
  }
  else if (pMenuObjNext == &scMenuMain)
  {
    // Now we are not in a specific connection's context
    scConnHandle = LINKDB_CONNHANDLE_INVALID;

    // Clear connection-related message
    Display_clearLine(dispHandle, SC_ROW_CUR_CONN);
  }
}


/*********************************************************************
*********************************************************************/
