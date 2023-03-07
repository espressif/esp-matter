/******************************************************************************

 @file  host_test_app.c

 @brief This file contains the HostTest sample application for use with the
        CC2650 Bluetooth Low Energy Protocol Stack.

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

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/display/Display.h>

#if defined(USE_FPGA) || defined(DEBUG_SW_TRACE)
#include <driverlib/ioc.h>
#endif /* USE_FPGA | DEBUG_SW_TRACE */

#include <string.h>

#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"


#include "inc/npi_ble.h"
#include "inc/npi_task.h"
#include <inc/hw_types.h>

#if defined (USE_RCOSC)
#include "rcosc_calibration.h"
#endif /* USE_RCOSC */

#include "host_test_app.h"

#ifdef ICALL_LITE
#include "icall_hci_tl.h"
#endif /* ICALL_LITE */

/*********************************************************************
 * CONSTANTS
 */

// LE Event Lengths
#define HCI_CMD_COMPLETE_EVENT_LEN              3
#define HCI_CMD_VS_COMPLETE_EVENT_LEN           2
#define HCI_CMD_STATUS_EVENT_LEN                4
#define HCI_PHY_UPDATE_COMPLETE_EVENT_LEN       6
#define HCI_SCAN_REQ_REPORT_EVENT_LEN           11
#define HCI_BLE_CHANNEL_MAP_UPDATE_EVENT_LEN    9

// Task configuration
#define HTA_TASK_PRIORITY                     1
#define HTA_TASK_STACK_SIZE                   1024

#define HTA_ICALL_EVT                         ICALL_MSG_EVENT_ID // Event_Id_31
#define HTA_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30
#define HOST_TL_CALLBACK_EVENT                 Event_Id_00

#define HTA_ALL_EVENTS                        (HTA_ICALL_EVT | HTA_QUEUE_EVT | \
                                               HOST_TL_CALLBACK_EVENT)

#ifdef ICALL_LITE
#define HTA_NO_OPCODE_SENT                      0xFFFFFFFF
#else /* !ICALL_LITE */
#define HTA_NO_OPCODE_SENT                      0xFFFF
#endif /* ICALL_LITE */

/*********************************************************************
 * TYPEDEFS
 */
// Queue record structure
typedef struct  Host_TestAPP_QueueRec_t
{
    Queue_Elem _elem;
    aeExtAdvRptEvt_t *pData;
    void* callbackFctPtr;
} Host_TestAPP_QueueRec;


/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
Display_Handle dispHandle = NULL;

#ifdef ICALL_LITE
extern uint32 lastAppOpcodeIdxSent;
#else /* !ICALL_LITE */
extern uint16 lastAppOpcodeSent;
#endif /* ICALL_LITE */

/*********************************************************************
 * LOCAL VARIABLES
 */
// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.
static ICall_SyncHandle Host_TestApp_syncEvent;

// Task configuration
Task_Struct htaTask;
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(htaTaskStack, 8)
#elif defined __clang__
__attribute__ ((aligned (8)))
#else
#pragma data_alignment=8
#endif
uint8_t htaTaskStack[HTA_TASK_STACK_SIZE];

#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
#if !defined (GATT_DB_OFF_CHIP)
  static uint8 deviceName[GAP_DEVICE_NAME_LEN] = { 0 };
  static uint16 appearance = 17;
#endif
#endif // (CENTRAL_CFG | PERIPHERAL_CFG)

// Stack build revision
ICall_BuildRevision buildRev;


// Handle for the Callback Event Queue
static Queue_Handle Host_testApp_Queue;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void HostTestApp_init(void);
static void HostTestApp_taskFxn(UArg a0, UArg a1);

static void HostTestApp_processGapEvent(ICall_HciExtEvt *pMsg);
#ifdef RTLS_CTE_TEST
static void HostTestApp_processTestEvent(ICall_HciExtEvt *pMsg);
#endif
static void HostTestApp_processBLEEvent(ICall_HciExtEvt *pMsg);

static void sendCommandCompleteEvent(uint8 eventCode, uint16 opcode,
                                     uint8 numParam, uint8 *param);
static void sendCommandStatusEvent(uint8_t eventCode, uint16_t status,
                                   uint16_t opcode);
static void sendBLECompleteEvent(uint8 eventLen, uint8 *pEvent);

#ifdef ICALL_LITE
void HostTestApp_handleNPIRxInterceptEvent(uint8_t *pMsg);
uint8_t HostTestApp_processStackMsg(hciPacket_t *pBuf);
static void HostTestApp_sendToNPI(uint8_t *buf, uint16_t len);
#endif /* ICALL_LITE */

static uint8_t  Host_TestApp_Queue_ProcessCbkEvent(void);
static uint8_t  Host_TestApp_postCallbackEvent(void *pData, void* callbackFctPtr);
/*********************************************************************
 * EXTERN FUNCTIONS
 */
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      HostTestApp_createTask
 *
 * @brief   Task creation function for the Host Test App.
 *
 * @param   none
 *
 * @return  none
 */
void HostTestApp_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = htaTaskStack;
  taskParams.stackSize = HTA_TASK_STACK_SIZE;
  taskParams.priority = HTA_TASK_PRIORITY;

  Task_construct(&htaTask, HostTestApp_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      HostTestApp_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 *
 * @param   none
 *
 * @return  none
 */
static void HostTestApp_init(void)
{
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &Host_TestApp_syncEvent);

#ifdef USE_FPGA
  #if defined(CC26X2)
    // configure RF Core SMI Data Link
    IOCPortConfigureSet(IOID_20, IOC_PORT_RFC_GPO0, IOC_STD_OUTPUT);
    IOCPortConfigureSet(IOID_18, IOC_PORT_RFC_GPI0, IOC_STD_INPUT);

    // configure RF Core SMI Command Link
    IOCPortConfigureSet(IOID_22, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_OUT, IOC_STD_OUTPUT);
    IOCPortConfigureSet(IOID_21, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_IN, IOC_STD_INPUT);

    // configure RF Core tracer IO
    // Maps to SmartRF06EB RF1.10.
    IOCPortConfigureSet(IOID_19, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT | IOC_CURRENT_4MA | IOC_SLEW_ENABLE);
  #else
    // configure RF Core SMI Data Link
    IOCPortConfigureSet(IOID_12, IOC_PORT_RFC_GPO0, IOC_STD_OUTPUT);
    IOCPortConfigureSet(IOID_11, IOC_PORT_RFC_GPI0, IOC_STD_INPUT);

    // configure RF Core SMI Command Link
    IOCPortConfigureSet(IOID_10, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_OUT, IOC_STD_OUTPUT);
    IOCPortConfigureSet(IOID_9, IOC_IOCFG0_PORT_ID_RFC_SMI_CL_IN, IOC_STD_INPUT);

    // configure RF Core tracer IO
    // Maps to J9 Header.
    IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT);
  #endif /* CC26X2 */
#else /* !USE_FPGA */
  #if defined(DEBUG_SW_TRACE)
    // configure RF Core tracer IO
    // Note: Pleaes see board files for additional IO mapping information.

    // SmartRF06EB: Maps to RF1.20
    // Note: Must disable LCD Display!
    //IOCPortConfigureSet(IOID_8, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT | IOC_CURRENT_4MA | IOC_SLEW_ENABLE);

    // SmartRF06EB: Maps to RF2.10
    // No conflict with LCD.
    //IOCPortConfigureSet(IOID_24, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT | IOC_CURRENT_4MA | IOC_SLEW_ENABLE);

    // SmartRF06EB: Maps to RF2.12
    // No conflict with LCD.
    IOCPortConfigureSet(IOID_30, IOC_PORT_RFC_TRC, IOC_STD_OUTPUT | IOC_CURRENT_4MA | IOC_SLEW_ENABLE);
  #endif /* DEBUG_SW_TRACE */
#endif /* USE_FPGA */

  // Set device's Sleep Clock Accuracy
  //HCI_EXT_SetSCACmd(40);

#if defined(USE_RCOSC)
  // Set device's Sleep Clock Accuracy
#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
  HCI_EXT_SetSCACmd(500);
#endif // (CENTRAL_CFG | PERIPHERAL_CFG)
  RCOSC_enableCalibration();
#endif /* USE_RCOSC */

#ifdef ICALL_LITE
  // Intercept NPI RX events.
  NPITask_registerIncomingRXEventAppCB(HostTestApp_handleNPIRxInterceptEvent,
                                       INTERCEPT);

  HCI_TL_Init(NULL, (HCI_TL_CommandStatusCB_t) HostTestApp_sendToNPI,
               Host_TestApp_postCallbackEvent, selfEntity);

  HCI_TL_getCmdResponderID(ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG,
                                                     selfEntity));
#endif /* ICALL_LITE */

  dispHandle = Display_open(Display_Type_LCD, NULL);

  // Register for unprocessed HCI/Host event messages
#if defined(HOST_CONFIG)
  GAP_RegisterForMsgs(selfEntity);

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG) )
  // Initialize GATT Client
  VOID GATT_InitClient();
#endif /* (CENTRAL_CFG | PERIPHERAL_CFG) */
#endif // HOST_CONFIG

  // Get build revision
#ifdef ICALL_LITE
  buildRevision(&buildRev);
#else /* !ICALL_LITE */
  VOID Util_buildRevision(&buildRev);
#endif /* ICALL_LITE */

#if defined(HOST_CONFIG)
#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
#if !defined (GATT_DB_OFF_CHIP)
  #if defined (GATT_QUAL)
  VOID GATTQual_AddService(GATT_ALL_SERVICES); // Includes GAP and GATT Services

  // Set device name
  memcpy(deviceName, "Test Database", 10);

  {
    gapPeriConnectParams_t connParam = { 100, 200, 0, 2000 };

    // Set preferred connection parameters
    VOID GGS_SetParameter(GGS_PERI_CONN_PARAM_ATT, sizeof(gapPeriConnectParams_t), (void*) &connParam);
  }
  #else /* !GATT_QUAL */
  // Add our services to GATT Server
  VOID GGS_AddService(GAP_SERVICE);
  VOID GATTServApp_AddService(GATT_ALL_SERVICES);
    #if defined (GATT_TEST)
  VOID GATTTest_AddService(GATT_ALL_SERVICES);
    #endif /* GATT_TEST */

  // Set device name
  if ((buildRev.hostInfo & CENTRAL_CFG) && (buildRev.hostInfo & PERIPHERAL_CFG))
  {
    memcpy(deviceName, "TI BLE All", 10);
  }
  else if (buildRev.hostInfo & CENTRAL_CFG)
  {
    memcpy(deviceName, "TI BLE Central", 14);
  }
  else if (buildRev.hostInfo & PERIPHERAL_CFG)
  {
    memcpy(deviceName, "TI BLE Peripheral",  17);
  }
  else
  {
    memcpy(deviceName, "TI BLE Unknown",  14);
  }
  #endif /* GATT_QUAL */

  VOID GGS_SetParameter(GGS_DEVICE_NAME_ATT, strlen((char *)deviceName), deviceName);
  VOID GGS_SetParameter(GGS_APPEARANCE_ATT, sizeof(uint16), (void*)&appearance);

#endif /* GATT_DB_OFF_CHIP */
#endif /* (CENTRAL_CFG | PERIPHERAL_CFG) */

#ifdef CC26XX_R2
  Display_print0(dispHandle, 0, 0, "TI BLEv5.x");
#else /* !CC26XX_R2 */
  Display_print0(dispHandle, 0, 0, "TI BLEv2.0");
#endif /* CC26XX_R2 */
  Display_print0(dispHandle, 1, 0, "HostTestApp");

  // Display Host build configuration
  if ((buildRev.hostInfo & CENTRAL_CFG) && (buildRev.hostInfo & PERIPHERAL_CFG))
  {
    Display_print0(dispHandle, 2, 0, "All");
  }
  else if ((buildRev.hostInfo & CENTRAL_CFG) &&
           (buildRev.hostInfo & BROADCASTER_CFG))
  {
    Display_print0(dispHandle, 2, 0, "Cent+Bcast");
  }
  else if ((buildRev.hostInfo & PERIPHERAL_CFG) &&
           (buildRev.hostInfo & OBSERVER_CFG))
  {
    Display_print0(dispHandle, 2, 0, "Peri+Observ");
  }
  else if (buildRev.hostInfo & CENTRAL_CFG)
  {
    Display_print0(dispHandle, 2, 0, "Central");
  }
  else if (buildRev.hostInfo & PERIPHERAL_CFG)
  {
    Display_print0(dispHandle, 2, 0, "Peripheral");
  }
  else
  {
    Display_print1(dispHandle, 2, 0, "Unknown build cfg %d", buildRev.hostInfo);
  }
#endif // HOST_CONFIG

  // Create a Tx Queue instance
  Host_testApp_Queue = Queue_create(NULL, NULL);

#if defined(GAP_BOND_MGR) && defined (EXCLUDE_SM)
  uint8_t pairMode = GAPBOND_PAIRING_MODE_NO_PAIRING;
  GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
#endif
}

/*********************************************************************
 * @fn      HostTestApp_taskFxn
 *
 * @brief   Application task entry point for the Host Test App.
 *
 * @param   none
 *
 * @return  none
 */
static void HostTestApp_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  HostTestApp_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    events = Event_pend(Host_TestApp_syncEvent, Event_Id_NONE, HTA_ALL_EVENTS,
                        ICALL_TIMEOUT_FOREVER);

    if (events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
                                   (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        bool dealloc = true;

        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
#ifdef ICALL_LITE
          ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

          if (pEvt->signature != 0xFFFF)
          {
            // Message
            dealloc = HostTestApp_processStackMsg((hciPacket_t *)pMsg);
          }
#else /* !ICALL_LITE */

          // Process incoming messages
          switch (pMsg->hdr.event)
          {
            case HCI_GAP_EVENT_EVENT:
              HostTestApp_processGapEvent(pMsg);
              break;

            default:
              break;
          }
#endif /* ICALL_LITE */
        }

        if (dealloc == true)
        {
          ICall_freeMsg(pMsg);
        }
      }

      if( events & HOST_TL_CALLBACK_EVENT)
      {
        if (!Queue_empty(Host_testApp_Queue))
        {
          // Push the pending Async Msg to the host.
          if ( Host_TestApp_Queue_ProcessCbkEvent())
          {
            // Q was not empty, there's could be more to handle so preserve the
            // flag and repost to the task.
            Event_post(Host_TestApp_syncEvent, HOST_TL_CALLBACK_EVENT);
          }
        }
      }
    }
  }
}

#ifdef ICALL_LITE
/*********************************************************************
 * @fn      HostTestApp_processStackMsg
 *
 * @brief   Process incoming HCI Event or Data Message.
 *
 * @param   pEvt - pointer to event structure.
 *
 * @return  TRUE to deallocate, FALSE otherwise
 */
uint8_t HostTestApp_processStackMsg(hciPacket_t *pBuf)
{
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
    HostTestApp_sendToNPI(pBuf->pData, len);

    // Free buffers if needed.
    switch (pBuf->pData[0])
    {
      case HCI_ACL_DATA_PACKET:
      case HCI_SCO_DATA_PACKET:
        // This API is link directly with the Stack library. which is fine since
        // it is thread safe (critical section inside)
        BM_free(pBuf->pData);
      default:
        break;
    }

    return(TRUE);
  }
  else if (pBuf->hdr.event == HCI_GAP_EVENT_EVENT)
  {
    // Structured Host Event from HCI that Host did not process.
    // There should only be few of these.
    HostTestApp_processGapEvent((ICall_HciExtEvt *) pBuf);

    return(TRUE);
  }
  else
  {
#if (defined(HCI_TL_FULL) && (defined HOST_CONFIG))
     // Structured Host Event from Host Layer, serialize it first.  Then it will be passed back
     // as a HCI_CTRL_TO_HOST_EVENT so that it can be sent out.
    return(HCI_TL_processStructuredEvent((ICall_Hdr *)pBuf));
#else
    return(TRUE);
#endif
  }
}
#endif /* ICALL_LITE */

/*********************************************************************
 * @fn      HostTestApp_processGapEvent
 *
 * @brief   Process an incoming GAP Event.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void HostTestApp_processGapEvent(ICall_HciExtEvt *pMsg)
{
  switch(pMsg->hdr.status)
  {
    case HCI_COMMAND_COMPLETE_EVENT_CODE:
      {
        hciEvt_CmdComplete_t *pkt = (hciEvt_CmdComplete_t *)pMsg;

#ifdef ICALL_LITE
        if (HCI_TL_compareAppLastOpcodeSent(pkt->cmdOpcode))
#else /* !ICALL_LITE */
        if (lastAppOpcodeSent == pkt->cmdOpcode)
#endif /* ICALL_LITE */
        {
          // app processes this as it was embedded msg to stack

          // Reset last opcode sent
#ifdef ICALL_LITE
          lastAppOpcodeIdxSent = HTA_NO_OPCODE_SENT;
#else /* !ICALL_LITE */
          lastAppOpcodeSent = HTA_NO_OPCODE_SENT;
#endif /* ICALL_LITE */
        }
        else
        {
          ICall_MsgHdr *msgHdr;
          uint16_t len;

          msgHdr = (ICall_MsgHdr *)pMsg;
          msgHdr--; // Backup to the msg header

          len = (uint16_t)(msgHdr->len - sizeof (hciEvt_CmdComplete_t));

          sendCommandCompleteEvent(HCI_COMMAND_COMPLETE_EVENT_CODE,
                                   pkt->cmdOpcode, len, pkt->pReturnParam);
        }
      }
      break;

    case HCI_DISCONNECTION_COMPLETE_EVENT_CODE:
      break;

    case HCI_COMMAND_STATUS_EVENT_CODE:
      {
        hciEvt_CommandStatus_t *pkt = (hciEvt_CommandStatus_t *)pMsg;

#ifdef ICALL_LITE
        if (HCI_TL_compareAppLastOpcodeSent(pkt->cmdOpcode))
#else /* !ICALL_LITE */
        if (lastAppOpcodeSent == pkt->cmdOpcode)
#endif /* ICALL_LITE */
        {
          // app processes this as it was embedded msg to stack

          // Reset last opcode sent
#ifdef ICALL_LITE
          lastAppOpcodeIdxSent = HTA_NO_OPCODE_SENT;
#else /* !ICALL_LITE */
          lastAppOpcodeSent = HTA_NO_OPCODE_SENT;
#endif /* ICALL_LITE */
        }
        else if (pkt->cmdOpcode == HCI_LE_SET_PHY)
        {
          sendCommandStatusEvent(HCI_COMMAND_STATUS_EVENT_CODE, pkt->cmdStatus,
                                 pkt->cmdOpcode);
        }
      }
      break;

    case HCI_LE_EVENT_CODE:
      HostTestApp_processBLEEvent(pMsg);
      break;

    case HCI_VE_EVENT_CODE:
      {
        hciEvt_VSCmdComplete_t *pkt = (hciEvt_VSCmdComplete_t *)pMsg;

#ifdef ICALL_LITE
        if (HCI_TL_compareAppLastOpcodeSent(pkt->cmdOpcode))
#else /* !ICALL_LITE */
        if (lastAppOpcodeSent == pkt->cmdOpcode)
#endif /* ICALL_LITE */
        {
          // app processes this as it was embedded msg to stack

          // Reset last opcode sent
#ifdef ICALL_LITE
          lastAppOpcodeIdxSent = HTA_NO_OPCODE_SENT;
#else /* !ICALL_LITE */
          lastAppOpcodeSent = HTA_NO_OPCODE_SENT;
#endif /* ICALL_LITE */
        }
        else
        {
          sendCommandCompleteEvent(HCI_VE_EVENT_CODE, pkt->cmdOpcode,
                                   pkt->length, pkt->pEventParam);
        }
      }
      break;

    case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
      AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
      break;

    case HCI_TEST_EVENT_CODE:
#ifdef RTLS_CTE_TEST
	  HostTestApp_processTestEvent(pMsg);
#endif
      break;

    default:
      break;
  }
}
#ifdef RTLS_CTE_TEST
/*********************************************************************
 * @fn      HostTestApp_processTestEvent
 *
 * @brief   Process an incoming BLE Event for testing.

 * @param   pMsg - message to process
 *
 * @return  None.
 */
static void HostTestApp_processTestEvent(ICall_HciExtEvt *pMsg)
{
  uint8 *pEvtTest = (uint8 *)pMsg + 8;
  uint8 eventLen;
  switch (*pEvtTest)
  {
    case HCI_BLE_CONNECTIONLESS_IQ_REPORT_EVENT:
      {
        eventLen = HCI_CONNECTIONLESS_IQ_REPORT_EVENT_LEN + (2 * pEvtTest[12]); // adding the number of samples count from the msg
      }
      break;
    default:
      eventLen = 0;
      break;
  }
  if (eventLen > 0)
  {
    // Send BLE Complete Event
	sendBLECompleteEvent(eventLen, pEvtTest);
  }
}
#endif
/*********************************************************************
 * @fn      HostTestApp_processBLEEvent
 *
 * @brief   Process an incoming BLE Event.

 * @param   pMsg - message to process
 *
 * @return  None.
 */
static void HostTestApp_processBLEEvent(ICall_HciExtEvt *pMsg)
{
  hciEvt_BLEPhyUpdateComplete_t *pEvt = (hciEvt_BLEPhyUpdateComplete_t *)pMsg;
  uint8 event[HCI_SCAN_REQ_REPORT_EVENT_LEN];
  uint8 eventLen;

  switch (pEvt->BLEEventCode)
  {
    case HCI_BLE_PHY_UPDATE_COMPLETE_EVENT:
      {
        event[0] = HCI_BLE_PHY_UPDATE_COMPLETE_EVENT; // event code
        event[1] = pEvt->status;                      // status
        event[2] = LO_UINT16(pEvt->connHandle);       // connection handle (LSB)
        event[3] = HI_UINT16(pEvt->connHandle);       // connection handle (MSB)
        event[4] = pEvt->txPhy;                       // TX PHY
        event[5] = pEvt->rxPhy;                       // RX PHY

        eventLen = HCI_PHY_UPDATE_COMPLETE_EVENT_LEN;
      }
      break;

    case HCI_BLE_SCAN_REQ_REPORT_EVENT:
      {
        hciEvt_BLEScanReqReport_t *pkt = (hciEvt_BLEScanReqReport_t *) pEvt;

        event[0] = pkt->BLEEventCode;
        event[1] = pkt->eventType;
        event[2] = pkt->peerAddrType;
        memcpy(&event[3], pkt->peerAddr, B_ADDR_LEN);
        event[9] = pkt->bleChan;
        event[10] = pkt->rssi;

        eventLen = HCI_SCAN_REQ_REPORT_EVENT_LEN;
      }
      break;

    case HCI_BLE_CHANNEL_MAP_UPDATE_EVENT:
      {
        hciEvt_BLEChanMapUpdate_t *pkt = (hciEvt_BLEChanMapUpdate_t *) pEvt;

        event[0] = pkt->BLEEventCode;
        memcpy(&event[1], &pkt->connHandle, sizeof(pkt->connHandle));
        event[3] = pkt->nextDataChan;
        memcpy(&event[4], pkt->newChanMap, LL_NUM_BYTES_FOR_CHAN_MAP);

        eventLen = HCI_BLE_CHANNEL_MAP_UPDATE_EVENT_LEN;
      }
      break;

    default:
      eventLen = 0;
      break;
  }

  if (eventLen > 0)
  {
    // Send BLE Complete Event
    sendBLECompleteEvent(eventLen, event);
  }
}

//*****************************************************************************
// the function prototypes

/*******************************************************************************
 * This generic function sends a Command Complete or a Vendor Specific Command
 * Complete Event to the Host.
 *
 */
static void sendCommandCompleteEvent(uint8_t eventCode, uint16_t opcode,
                                     uint8_t numParam, uint8_t *param)
{
  npiPkt_t *msg;
  uint16_t   totalLength;
  uint16_t   txLen = 0; // Length to transmit

  // The initial length will be:
  // Icall message header(4) - not part of packet sent to HCI Host!
  // Minimum Event Data: Packet Type(1) + Event Code(1) + Length(1)
  // Return Parameters (0..N)
  totalLength = sizeof(npiPkt_t) + HCI_EVENT_MIN_LENGTH + numParam;

  // adjust the size of the event packet based on event code
  // Note: If not a vendor specific event, then the event includes:
  //       Command Complete Data: Number of HCI Commands Allowed(1) + Command Opcode(2)
  // Note: If a vendor specific event, then the event includes:
  //       Vendor Specific Command Complete Data: Vendor Specific Event Opcode(2)
  totalLength += ((eventCode != HCI_VE_EVENT_CODE)  ?
                   HCI_CMD_COMPLETE_EVENT_LEN        :
                   HCI_CMD_VS_COMPLETE_EVENT_LEN);

  // allocate memory for Icall hdr + packet
  msg = (npiPkt_t *)ICall_allocMsg(totalLength);
  if (msg)
  {
    // Icall message event, status, and pointer to packet
    msg->hdr.event  = HCI_EVENT_PACKET;
    msg->hdr.status = 0xFF;
    msg->pData      = (uint8*)(msg+1);

    // fill in Command Complete Event data
    msg->pData[0] = HCI_EVENT_PACKET;
    msg->pData[1] = eventCode;

    txLen += 2;

    // check if this isn't a vendor specific event
    if (eventCode != HCI_VE_EVENT_CODE)
    {
      msg->pData[2] = numParam + HCI_CMD_COMPLETE_EVENT_LEN;
      msg->pData[3] = 1;// hciCtrlCmdToken;     // event parameter 1
      msg->pData[4] = LO_UINT16(opcode); // event parameter 2
      msg->pData[5] = HI_UINT16(opcode); // event parameter 2

      txLen += 4;

      // remaining event parameters
      (void)memcpy(&msg->pData[6], param, numParam);

      txLen += numParam;
    }
    else // it is a vendor specific event
    {
      // less one byte as number of complete packets not used in vendor specific event
      msg->pData[2] = numParam + HCI_CMD_VS_COMPLETE_EVENT_LEN;
      msg->pData[3] = param[0];            // event parameter 0: event opcode LSB
      msg->pData[4] = param[1];            // event parameter 1: event opcode MSB
      msg->pData[5] = param[2];            // event parameter 2: status
      msg->pData[6] = LO_UINT16(opcode); // event parameter 3: command opcode LSB
      msg->pData[7] = HI_UINT16(opcode); // event parameter 3: command opcode MSB

      txLen += 6;

      // remaining event parameters
      // Note: The event opcode and status were already placed in the msg packet.
      (void)memcpy(&msg->pData[8], &param[3], numParam-HCI_EVENT_MIN_LENGTH);

      txLen += (numParam-HCI_EVENT_MIN_LENGTH);
    }

    msg->pktLen = txLen;

    NPITask_sendToHost((uint8_t *)msg);
  }
}

/*******************************************************************************
 * This generic function sends a Command Complete or a Vendor Specific Command
 * Complete Event to the Host.
 *
 */
static void sendCommandStatusEvent(uint8_t eventCode, uint16_t status,
                                   uint16_t opcode)
{
  npiPkt_t *msg;
  uint16_t   totalLength;

  // The initial length will be:
  // Icall message header(4) - not part of packet sent to HCI Host!
  // Minimum Event Data: Packet Type(1) + Event Code(1) + Length(1)
  // Command Status Event Data: Status (1) + Num HCI Cmd Pkt (1) + Cmd Opcode (2)
  totalLength = sizeof(npiPkt_t)     +
                HCI_EVENT_MIN_LENGTH +
                HCI_CMD_STATUS_EVENT_LEN;

  // allocate memory for Icall hdr + packet
  msg = (npiPkt_t *)ICall_allocMsg(totalLength);
  if (msg)
  {
    // Icall message event, status, and pointer to packet
    msg->hdr.event  = HCI_EVENT_PACKET;
    msg->hdr.status = 0xFF;

    // fill in length and data pointer
    msg->pktLen = HCI_EVENT_MIN_LENGTH + HCI_CMD_STATUS_EVENT_LEN;
    msg->pData  = (uint8*)(msg+1);

    // fill in Command Complete Event data
    msg->pData[0] = HCI_EVENT_PACKET;
    msg->pData[1] = eventCode;
    msg->pData[2] = HCI_CMD_STATUS_EVENT_LEN;
    msg->pData[3] = status;
    msg->pData[4] = 1;                 // number of HCI command packets
    msg->pData[5] = LO_UINT16(opcode); // opcode (LSB)
    msg->pData[6] = HI_UINT16(opcode); // opcode (MSB)

    NPITask_sendToHost((uint8_t *)msg);
  }
}

/*******************************************************************************
 * This is a generic function used to send BLE Complete Event to the
 * Host processor.
 *
 */
static void sendBLECompleteEvent(uint8_t eventLen, uint8 *pEvent)
{
  npiPkt_t *msg;
  uint16_t   totalLength;

  // The initial length will be:
  // Icall message header(4) - not part of packet sent to HCI Host!
  // Minimum Event Data: Packet Type(1) + Event Code(1) + Length(1)
  // Event Data: eventLen
  totalLength = sizeof(npiPkt_t) + HCI_EVENT_MIN_LENGTH + eventLen;

  // allocate memory for Icall hdr + packet
  msg = (npiPkt_t *)ICall_allocMsg(totalLength);
  if (msg)
  {
    // Icall message event, status, and pointer to packet
    msg->hdr.event  = HCI_EVENT_PACKET;
    msg->hdr.status = 0xFF;

    // fill in length and data pointer
    msg->pktLen = HCI_EVENT_MIN_LENGTH + eventLen;
    msg->pData  = (uint8*)(msg+1);

    // fill in BLE Complete Event data
    msg->pData[0] = HCI_EVENT_PACKET;
    msg->pData[1] = HCI_LE_EVENT_CODE;
    msg->pData[2] = eventLen;

    // populate event data
    if (eventLen > 0)
    {
      memcpy(&msg->pData[3], pEvent, eventLen);
    }

    NPITask_sendToHost((uint8_t *)msg);
  }
}

#ifdef ICALL_LITE
/*********************************************************************
 * @fn      HostTestApp_handleNPIRxInterceptEvent
 *
 * @brief   Intercept an NPI RX serial message and queue for this application.
 *
 * @param   pMsg - a NPIMSG_msg_t containing the intercepted message.
 *
 * @return  none.
 */
void HostTestApp_handleNPIRxInterceptEvent(uint8_t *pMsg)
{
  HCI_TL_SendToStack(((NPIMSG_msg_t *)pMsg)->pBuf);

  // The data is stored as a message, free this first.
  ICall_freeMsg(((NPIMSG_msg_t *)pMsg)->pBuf);

  // Free container.
  ICall_free(pMsg);
}

/*********************************************************************
 * @fn      HostTestApp_sendToNPI
 *
 * @brief   Create an NPI packet and send to NPI to transmit.
 *
 * @param   buf - pointer HCI event or data.
 *
 * @param   len - length of buf in bytes.
 *
 * @return  none
 */
static void HostTestApp_sendToNPI(uint8_t *buf, uint16_t len)
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

/*********************************************************************
 * @fn      Host_TestApp_postCallbackEvent
 *
 * @brief   Post a event to the task so it can be parse out of the Swi context.
 *
 * @param   extAdvRpt - a pointer to the data to parse.
 *          callbackFctPtr - function pointer that will parse the message.
 *
 * @return  status:
 *            true if allocation and post succeed.
 *            false if allocation of event failed.
 */
uint8_t Host_TestApp_postCallbackEvent(void *pData, void* callbackFctPtr)
{
  Host_TestAPP_QueueRec *recPtr;

  recPtr = ICall_malloc(sizeof(Host_TestAPP_QueueRec));
  if (recPtr)
  {
    recPtr->pData = pData;
    recPtr->callbackFctPtr = callbackFctPtr;
    Queue_put(Host_testApp_Queue, &recPtr->_elem);
    Event_post(Host_TestApp_syncEvent, HOST_TL_CALLBACK_EVENT);
    return(true);
  }
  else
  {
    return(false);
  }
}

/*********************************************************************
 * @fn      Host_TestApp_Queue_ProcessCbkEvent
 *
 * @brief   Process the queue of Callback from HCI transport Layer.
 *
 * @param   None
 *
 * @return  TRUE: The list was not empty.
 *          FALSE: The list was empty.
 */
static uint8_t Host_TestApp_Queue_ProcessCbkEvent(void)
{
  //Get the Queue elem atomicaly:
  Host_TestAPP_QueueRec *pRec = Queue_get(Host_testApp_Queue);

  if (pRec != (Host_TestAPP_QueueRec *)Host_testApp_Queue)
  {
    //List not Empty
    ((void (*)(void*))(pRec->callbackFctPtr))(pRec->pData);
    ICall_free(pRec);
  }
  else
  {
    return(false);
  }
  return(true);
}

#endif /* ICALL_LITE */
/*********************************************************************
*********************************************************************/
