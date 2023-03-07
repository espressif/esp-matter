/******************************************************************************

 @file  npi_task.c

 @brief NPI is a TI RTOS Application Thread that provides a common
        Network Processor Interface framework.

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2021-2022, Texas Instruments Incorporated
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

//*****************************************************************************
// Includes
//*****************************************************************************
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <xdc/std.h>
#ifdef ICALL_EVENTS
#include <ti/sysbios/knl/Event.h>
#else //!ICALL_EVENTS
#include <ti/sysbios/knl/Semaphore.h>
#endif //ICALL_EVENTS
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/BIOS.h>
#include <driverlib/ioc.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include "npi_util.h"
#include "npi_task.h"
#include "npi_data.h"
#include "npi_tl.h"


//*****************************************************************************
// Defines
//*****************************************************************************
#ifdef ICALL_EVENTS
#define NPITASK_ICALL_EVENT                 ICALL_MSG_EVENT_ID // Event_Id_31

//! \brief ASYNC Message Received Event (no framing bytes)
#define NPITASK_FRAME_RX_EVENT              Event_Id_00

//! \brief A framed message buffer is ready to be sent to the transport layer.
#define NPITASK_TX_READY_EVENT              Event_Id_01

//! \brief ASYNC Message Received Event (no framing bytes)
#define NPITASK_SYNC_FRAME_RX_EVENT         Event_Id_02

//! \brief Last TX message has been successfully sent
#define NPITASK_TX_DONE_EVENT               Event_Id_03

//! \brief Remote Rdy received Event
#define NPITASK_REM_RDY_EVENT               Event_Id_04

//! \brief NPI assert message
#define NPITASK_ASSERT_MSG_EVENT            Event_Id_05

#define NPITASK_ALL_EVENTS                  (NPITASK_ICALL_EVENT | \
                                             NPITASK_FRAME_RX_EVENT | \
                                             NPITASK_TX_READY_EVENT | \
                                             NPITASK_SYNC_FRAME_RX_EVENT | \
                                             NPITASK_TX_DONE_EVENT | \
                                             NPITASK_REM_RDY_EVENT | \
                                             NPITASK_ASSERT_MSG_EVENT)
#else //!ICALL_EVENTS
//! \brief ASYNC Message Received Event (no framing bytes)
#define NPITASK_FRAME_RX_EVENT              0x0008

//! \brief A framed message buffer is ready to be sent to the transport layer.
#define NPITASK_TX_READY_EVENT              0x0010

//! \brief ASYNC Message Received Event (no framing bytes)
#define NPITASK_SYNC_FRAME_RX_EVENT         0x0020

//! \brief Last TX message has been successfully sent
#define NPITASK_TX_DONE_EVENT               0x0040

//! \brief Remote Rdy received Event
#define NPITASK_REM_RDY_EVENT               0x0080

//! \brief NPI assert message
#define NPITASK_ASSERT_MSG_EVENT            0x0100
#endif //ICALL_EVENTS

//! \brief Task priority for NPI RTOS task
#define NPITASK_PRIORITY                    2

//! \brief NPI allows for NPI_MAX_SUBSTEM number of subsystems to be registered
//         to receive NPI messages from the host.
#define NPI_MAX_SUBSYSTEMS                  4
#define NPI_MAX_SS_ENTRY                    NPI_MAX_SUBSYSTEMS
#define NPI_MAX_ICALL_ENTRY                 NPI_MAX_SUBSYSTEMS

#define REM_RDY_ASSERTED                    0x00
#define REM_RDY_DEASSERTED                  0x01

#define NPI_ASSERT_MSG_LEN                  5

//*****************************************************************************
// Typedefs
//*****************************************************************************
//! \brief When a subsystem registers with NPI Task to receive messages from
//         host, it must provide its subsystem ID along with a CallBack function
//         to handle all messages
typedef struct _npiFromHostTableEntry_t
{
    uint8_t             ssID;
    npiFromHostCBack_t  ssCB;
} _npiFromHostTableEntry_t;

//! \brief When a subsystem registers with NPI Task to receive messages from
//         ICall it must provide its subsystem ID along with a CallBack function
//         to handle all messages
typedef struct _npiFromICallTableEntry_t
{
    uint8_t              icallID;
    npiFromICallCBack_t ssCB;
} _npiFromICallTableEntry_t;

//*****************************************************************************
// Globals
//*****************************************************************************
//! \brief RTOS task handle for NPI task
Task_Handle npiTaskHandle;
uint8_t *npiTaskStack;

//! \brief Handle for the ASYNC TX Queue
static Queue_Handle npiTxQueue;

//! \brief Handle for the ASYNC RX Queue
static Queue_Handle npiRxQueue;

//! \brief Handle for the SYNC TX Queue
static Queue_Handle npiSyncTxQueue;

//! \brief Handle for the SYNC RX Queue
static Queue_Handle npiSyncRxQueue;

//! \brief Flag/Counter indicating a Synchronous REQ/RSP is currently being
//!        processed.
static int8_t syncTransactionInProgress = 0;

//! \brief Pointer to last tx message.  This is freed once confirmation is
//!        is received that the buffer has been transmitted
//!        (ie. NPITASK_TRANSPORT_TX_DONE_EVENT)
//!
static uint8_t *lastQueuedTxMsg;

#ifdef ICALL_EVENTS
static ICall_SyncHandle syncEvent = NULL;
#else //!ICALL_EVENTS
#ifdef USE_ICALL
//! \brief NPI thread ICall Semaphore.
static ICall_Semaphore npiSem = NULL;
#else
static Semaphore_Handle npiSem = NULL;
#endif //USE_ICALL

//! \brief Task pending events
static uint16_t NPITask_events = 0;

//! \brief Event flags for capturing Task-related events from ISR context
static uint16_t tlDoneISRFlag = 0;
static uint16_t remRdyISRFlag = 0;
#endif //ICALL_EVENTS

#ifdef USE_ICALL
//! \brief NPI ICall Application Entity ID.
ICall_EntityID npiAppEntityID = 0;
#endif //USE_ICALL

//! \brief Routing table for translating incoming Host messages to the proper
//!        subsystem callback based on SSID of the message
_npiFromHostTableEntry_t HostToSSTable[NPI_MAX_SS_ENTRY];

#ifdef USE_ICALL
//! \brief Routing table for translating incoming ICall messages to the proper
//!        subsystem callback based on ICall msg source entity ID
_npiFromICallTableEntry_t ICallToSSTable[NPI_MAX_ICALL_ENTRY];
#endif //USE_ICALL

//! \brief Global flag to keep NPI from being opened twice without first closing
static uint8_t taskOpen = 0;

//! \brief Storage space for final assert NPI message
static uint8_t sendAssertMessage[NPI_ASSERT_MSG_LEN] =
{NPI_ASSERT_PAYLOAD_LEN,
 0, RPC_SYS_BLE_SNP+(NPI_MSG_TYPE_ASYNC<<5),
 NPI_ASSERT_CMD1_ID ,0};

//! \brief Save assert type locally for use in NPI assert message
static uint8_t npiTask_assertType;

/* Default NPI parameters structure */
const NPI_Params NPI_defaultParams = {
    .stackSize          = 1024,
    .bufSize            = 530,
    .mrdyPinID          = IOID_UNUSED,
    .srdyPinID          = IOID_UNUSED,
#if defined(NPI_USE_UART)
    .portType           = NPI_SERIAL_TYPE_UART,
    .portBoardID        = 0,                     /* CC2650_UART0 */
#elif defined(NPI_USE_SPI)
    .portType           = NPI_SERIAL_TYPE_SPI,
#if (defined(CC2650DK_5XD) ||  defined(CC2650DK_4XS )) && !defined(TI_DRIVERS_DISPLAY_INCLUDED)
    .portBoardID        = 0,                     /* CC2650_SPI0, conflicts with SRF06 display so both can't be enabled */
#elif (defined(CC2650DK_7ID) || defined(CC2650_LAUNCHXL) || \
    defined(CC2640R2_LAUNCHXL) || defined(CC26X2R1_LAUNCHXL) || defined(CC2652RB_LAUNCHXL) || \
    defined(CC13X2R1_LAUNCHXL) || (defined (CC13X2P1_LAUNCHXL) || defined (CC13X2P_2_LAUNCHXL) || defined (CC13X2P_4_LAUNCHXL) || \
    defined (CC2652PSIP_LP) || defined (CC2652RSIP_LP)))
    .portBoardID        = 1,                     /* CC2650_SPI1 */
#elif (defined(CC2650DK_5XD) || defined(CC2650DK_4XS)) && defined(TI_DRIVERS_DISPLAY_INCLUDED)
#error "WARNING! CC2650_SPI0, is used to drive the SmartRF06 display. Cannot use SPI0 if display is enabled."
#endif

#endif
};

//*****************************************************************************
// Function Prototypes
//*****************************************************************************

//! \brief Callback function registered with Transport Layer
static void NPITask_transportDoneCallBack(uint16_t sizeRx, uint16_t sizeTx);

//! \brief Callback function registered with Transport Layer
static void NPITask_RemRdyEventCB(uint8_t state);

const npiTLCallBacks transportCBs = {
  &NPITask_RemRdyEventCB,
  &NPITask_transportDoneCallBack,
};

//! \brief ASYNC TX Q Processing function.
static void NPITask_ProcessTXQ(Queue_Handle txQ);

//! \brief ASYNC RX Q Processing function.
static void NPITask_processRXQ(void);

//! \brief SYNC RX Q Processing function.
static void NPITask_processSyncRXQ(void);

//! \brief Function to route NPI Message to the appropriate subsystem
static uint8_t NPITask_routeHostToSS(_npiFrame_t *pNPIMsg);

#ifdef USE_ICALL
//! \brief Function to route ICall Message to the appropriate subsystem
static uint8_t NPITask_routeICallToSS(ICall_ServiceEnum src, uint8_t *pGenMsg);
#endif //USE_ICALL

//! \brief Function that transforms NPI Frame struct into a byte array. This is
//         necessary to send data over NPI Transport Layer
static uint8_t * NPITask_SerializeFrame(_npiFrame_t *pNPIMsg);

//! \brief Function that transforms byte contents of NPI RxBuf into an NPI Frame
//!        struct that can be routed.
static _npiFrame_t * NPITask_DeserializeFrame();

// -----------------------------------------------------------------------------
//! \brief      NPI main event processing loop.
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITask_Fxn(UArg a0, UArg a1)
{
    _npiCSKey_t key;
#ifdef USE_ICALL
    uint8_t *pMsg;
    ICall_ServiceEnum stackid;
    ICall_EntityID dest;

#ifdef ICALL_EVENTS
    ICall_enrollService(ICALL_SERVICE_CLASS_NPI, NULL, &npiAppEntityID,
                        &syncEvent);
#else //!ICALL_EVENTS
    ICall_enrollService(ICALL_SERVICE_CLASS_NPI, NULL, &npiAppEntityID, &npiSem);
#endif //ICALL_EVENTS
#endif //USE_ICALL

    /* Forever loop */
    for (;;)
    {
        /* Wait for response message */
#ifdef ICALL_EVENTS
        uint32_t NPITask_events;

        NPITask_events = Event_pend(syncEvent, Event_Id_NONE,
                                    NPITASK_ALL_EVENTS, BIOS_WAIT_FOREVER);

        if (NPITask_events)
#else //!ICALL_EVENTS
        if (Semaphore_pend(npiSem,BIOS_WAIT_FOREVER))
#endif //ICALL_EVENTS
        {
            // First check and Send NPI assert message
            if (NPITask_events & NPITASK_ASSERT_MSG_EVENT)
            {
                NPITask_sendAssertMsg(npiTask_assertType);
            }
            // Capture the ISR events flags now within a critical section.
            // We do this to avoid possible race conditions where the ISR is
            // modifying the event mask while the task is read/writing it.
            key = NPIUtil_EnterCS();

#ifndef ICALL_EVENTS
            NPITask_events = NPITask_events | tlDoneISRFlag |
                             remRdyISRFlag;

            tlDoneISRFlag = 0;
            remRdyISRFlag = 0;
#endif //ICALL_EVENTS

            NPIUtil_ExitCS(key);

            // Remote RDY event
            if (NPITask_events & NPITASK_REM_RDY_EVENT)
            {
#ifndef ICALL_EVENTS
                NPITask_events &= ~NPITASK_REM_RDY_EVENT;
#endif //ICALL_EVENTS
#if (NPI_FLOW_CTRL == 1)
                NPITL_handleRemRdyEvent();
#endif // NPI_FLOW_CTRL = 1
            }
            // TX Frame has been successfully sent
            if (NPITask_events & NPITASK_TX_DONE_EVENT)
            {
              //Deallocate most recent message being transmitted.
              NPIUtil_free(lastQueuedTxMsg);
              lastQueuedTxMsg = NULL;
#ifndef ICALL_EVENTS
              NPITask_events &= ~NPITASK_TX_DONE_EVENT;
#endif //ICALL_EVENTS
            }
            // Frame is ready to send to the Host
            if (NPITask_events & NPITASK_TX_READY_EVENT)
            {
              // Cannot send if NPI Tl is already busy.
              if (!NPITL_checkNpiBusy())
              {
                // Check for outstanding SYNC REQ/RSP transactions.  If so,
                // this ASYNC message must remain Q'd while we wait for the
                // SYNC RSP.
                if (!Queue_empty(npiSyncTxQueue) &&
                        syncTransactionInProgress >= 0)
                {
                    // Prioritize Synchronous traffic
                    NPITask_ProcessTXQ(npiSyncTxQueue);
                }
                else if (!(NPITask_events & NPITASK_SYNC_FRAME_RX_EVENT) &&
                             syncTransactionInProgress == 0 &&
                               !Queue_empty(npiTxQueue))
                {
                    // No outstanding SYNC REQ/RSP transactions, process
                    // ASYNC messages.
                    NPITask_ProcessTXQ(npiTxQueue);
                }
              }

                // The TX READY event flag can be cleared here regardless
                // of the state of the TX queues. The TX done call back
                // will always check the state of the queues and reset
                // the event flag if it discovers more messages to send
                // In this control flow, either a TX message was sent,
                // there is a pending sync tx message to be sent which
                // is blocking any async tx message(s), or NPI was already
                // sending a frame (aka busy). In either case,
                // the TX event will get set again when NPI is done sending (or
                // no longer busy) or after the blocking synchronous message
                // has been sent
#ifndef ICALL_EVENTS
                NPITask_events &= ~NPITASK_TX_READY_EVENT;
#endif //ICALL_EVENTS
            }
#ifdef USE_ICALL
            // ICall Message Event
            if (ICall_fetchServiceMsg(&stackid, &dest, (void **) &pMsg)
                == ICALL_ERRNO_SUCCESS)
            {
                // Route the ICall message to the appropriate subsystem
                if (NPITask_routeICallToSS(stackid,pMsg) != NPI_SUCCESS)
                {
                    // Unable to route message. Subsystem not registered.
                    // Free message
                    ICall_freeMsg(pMsg);
                }
            }
#endif //USE_ICALL
            // Synchronous Frame received from Host
            if (NPITask_events & NPITASK_SYNC_FRAME_RX_EVENT &&
                  syncTransactionInProgress <= 0)
            {
                // Process Queue and clear event flag
                NPITask_processSyncRXQ();
#ifndef ICALL_EVENTS
                NPITask_events &= ~NPITASK_SYNC_FRAME_RX_EVENT;
#endif //ICALL_EVENTS

                if (!Queue_empty(npiSyncRxQueue))
                {
                    // Queue is not empty so reset flag to process remaining
                    // frame(s)
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_SYNC_FRAME_RX_EVENT);
#else //!ICALL_EVENTS
                    NPITask_events |= NPITASK_SYNC_FRAME_RX_EVENT;
                    Semaphore_post(npiSem);
#endif //ICALL_EVENTS
                }
            }
#ifdef ICALL_EVENTS
            else if ((NPITask_events & NPITASK_SYNC_FRAME_RX_EVENT) &&
                     (syncTransactionInProgress > 0))
            {
                // the event needs to be reposted or it will be lost
                Event_post(syncEvent, NPITASK_SYNC_FRAME_RX_EVENT);
            }
#endif


            // A complete frame (msg) has been received and is ready for handling
            if (NPITask_events & NPITASK_FRAME_RX_EVENT &&
                  syncTransactionInProgress == 0)
            {
                // Process the ASYNC message and clear event flag
                NPITask_processRXQ();
#ifndef ICALL_EVENTS
                NPITask_events &= ~NPITASK_FRAME_RX_EVENT;
#endif //ICALL_EVENTS

                if (!Queue_empty(npiRxQueue))
                {
                    // Q is not empty reset flag and process next message
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_FRAME_RX_EVENT);
#else //!ICALL_EVENTS
                    NPITask_events |= NPITASK_FRAME_RX_EVENT;
                    Semaphore_post(npiSem);
#endif //ICALL_EVENTS
                }
            }
        }
    }
}


// -----------------------------------------------------------------------------
// Exported Functions

// -----------------------------------------------------------------------------
//! \brief      Initialize a NPI_Params struct with default values
//!
//! \param[in]  portType  NPI_SERIAL_TYPE_[UART,SPI]
//! \param[in]  params    Pointer to NPI params to be initialized
//!
//! \return     uint8_t   Status NPI_SUCCESS, or NPI_TASK_INVALID_PARAMS
// -----------------------------------------------------------------------------
uint8_t NPITask_Params_init(uint8_t portType, NPI_Params *params)
{
  if (params != NULL)
  {
    *params = NPI_defaultParams;

#if defined(NPI_USE_UART)
    UART_Params_init(&params->portParams.uartParams);
    params->portParams.uartParams.readDataMode = UART_DATA_BINARY;
    params->portParams.uartParams.writeDataMode = UART_DATA_BINARY;
    params->portParams.uartParams.readMode = UART_MODE_CALLBACK;
    params->portParams.uartParams.writeMode = UART_MODE_CALLBACK;
    params->portParams.uartParams.readEcho = UART_ECHO_OFF;
#elif defined(NPI_USE_SPI)
    SPI_Params_init(&params->portParams.spiParams);
    params->portParams.spiParams.mode = SPI_SLAVE;
    params->portParams.spiParams.bitRate = 8000000;
    params->portParams.spiParams.frameFormat = SPI_POL1_PHA1;
#endif //NPI_USE_UART

    return NPI_SUCCESS;
  }

  return NPI_TASK_INVALID_PARAMS;
}

// -----------------------------------------------------------------------------
//! \brief      Task creation function for NPI
//!
//! \param[in]  params    Pointer to NPI params which will be used to
//!                       initialize the NPI Task
//!
//! \return     uint8_t   Status NPI_SUCCESS, or NPI_TASK_FAILURE
// -----------------------------------------------------------------------------
uint8_t NPITask_open(NPI_Params *params)
{
    NPITL_Params transportParams;
    NPI_Params npiParams;
    Task_Params npiTaskParams;

    // Check to see if NPI has already been opened
    if (taskOpen)
    {
      return NPI_TASK_FAILURE;
    }

    taskOpen = 1;

    // If params are NULL use defaults.
    if (params == NULL) {
#if defined(NPI_USE_UART)
        NPITask_Params_init(NPI_SERIAL_TYPE_UART,&npiParams);
#elif defined(NPI_USE_SPI)
        NPITask_Params_init(NPI_SERIAL_TYPE_SPI,&npiParams);
#endif // NPI_USE_UART

        params = &npiParams;
    }

    // Initialize globals
#ifndef ICALL_EVENTS
    NPITask_events = 0;
#endif //ICALL_EVENTS
    lastQueuedTxMsg = NULL;

#ifndef ICALL_EVENTS
#ifndef USE_ICALL
    // create Semaphore instances
    npiSem = Semaphore_create(0, NULL, NULL);
#endif //USE_ICALL
#endif //ICALL_EVENTS

    // create Queue instances
    npiTxQueue = Queue_create(NULL, NULL);
    npiRxQueue = Queue_create(NULL, NULL);
    npiSyncRxQueue = Queue_create(NULL, NULL);
    npiSyncTxQueue = Queue_create(NULL, NULL);

    // Initialize Transport Layer
    transportParams.npiTLBufSize = params->bufSize;
    transportParams.mrdyPinID = params->mrdyPinID;
    transportParams.srdyPinID = params->srdyPinID;
    transportParams.portType = params->portType;
    transportParams.portBoardID = params->portBoardID;
    transportParams.portParams = params->portParams;
    transportParams.npiCallBacks = transportCBs;

    NPITL_openTL(&transportParams);

   // Clear Routing Tables
    memset(HostToSSTable, 0, sizeof(_npiFromHostTableEntry_t)*NPI_MAX_SS_ENTRY);
#ifdef USE_ICALL
    memset(ICallToSSTable, 0, sizeof(_npiFromICallTableEntry_t)*NPI_MAX_ICALL_ENTRY);
#endif //USE_ICALL

    // Configure and create the NPI task.
    Task_Params_init(&npiTaskParams);
    npiTaskStack = NPIUtil_malloc(params->stackSize);
    if (npiTaskStack != NULL)
    {
      npiTaskParams.stack = npiTaskStack;
      npiTaskParams.stackSize = params->stackSize;
      npiTaskParams.priority = NPITASK_PRIORITY;
      npiTaskHandle = Task_create(NPITask_Fxn, &npiTaskParams, NULL);
      if (npiTaskHandle == NULL)
      {
        return NPI_TASK_FAILURE;
      }
    }
    else
    {
      return NPI_TASK_FAILURE;
    }

    return NPI_SUCCESS;
}

// -----------------------------------------------------------------------------
//! \brief      NPI Task close and tear down. Cannot be used with ICall because
//!             ICall service cannot be un-enrolled
//!
//! \return     uint8_t   Status NPI_SUCCESS, or NPI_TASK_FAILURE
// -----------------------------------------------------------------------------
uint8_t NPITask_close(void)
{
#ifdef USE_ICALL
    return NPI_TASK_FAILURE;
#else
    if (!taskOpen)
    {
      return NPI_TASK_FAILURE;
    }

    // Close Tranpsort Layer
    NPITL_closeTL();

    // Delete RTOS allocated structures
#ifndef ICALL_EVENTS
    Semaphore_delete(&npiSem);
#endif //ICALL_EVENTS
    Queue_delete(&npiTxQueue);
    Queue_delete(&npiRxQueue);
    Queue_delete(&npiSyncRxQueue);
    Queue_delete(&npiSyncTxQueue);

    // Free any message buffers for in-flight messages
    if (lastQueuedTxMsg != NULL)
    {
      NPIUtil_free(lastQueuedTxMsg);
    }

    // Delete NPI task
    NPIUtil_free(npiTaskStack);
    Task_delete(&npiTaskHandle);
    taskOpen = 0;

    return NPI_SUCCESS;
#endif //USE_ICALL
}

// -----------------------------------------------------------------------------
//! \brief      API for application task to send a message to the Host.
//!             NOTE: It's assumed all message traffic to the stack will use
//!             other (ICALL) APIs/Interfaces.
//!
//! \param[in]  pMsg    Pointer to "unframed" message buffer.
//!
//! \return     uint8_t Status NPI_SUCCESS, or NPI_SS_NOT_FOUND
// -----------------------------------------------------------------------------
uint8_t NPITask_sendToHost(_npiFrame_t *pMsg)
{
    uint8_t status = NPI_SUCCESS;
    _npiCSKey_t key;

    // Must block task pre-emption so that the higher priority NPI task
    // does not clear the NPITask_events flag before pMsg is properly enqueued.
    key = NPIUtil_EnterCS();

    switch (NPI_GET_MSG_TYPE(pMsg))
    {
        // Enqueue to appropriate NPI Task Q and post correpsonding event.
        case NPI_MSG_TYPE_SYNCRSP:
        case NPI_MSG_TYPE_SYNCREQ:
        {
#ifdef ICALL_EVENTS
            NPIUtil_enqueueMsg(npiSyncTxQueue, syncEvent,
                               NPITASK_TX_READY_EVENT,(unsigned char *) pMsg);
#else //!ICALL_EVENTS
            NPITask_events |= NPITASK_TX_READY_EVENT;
            NPIUtil_enqueueMsg(npiSyncTxQueue, npiSem,(unsigned char *) pMsg);
#endif //ICALL_EVENTS
        }
        break;
        case NPI_MSG_TYPE_ASYNC:
        {
#ifdef ICALL_EVENTS
            NPIUtil_enqueueMsg(npiTxQueue, syncEvent, NPITASK_TX_READY_EVENT,
                               (unsigned char *) pMsg);
#else //!ICALL_EVENTS
            NPITask_events |= NPITASK_TX_READY_EVENT;
            NPIUtil_enqueueMsg(npiTxQueue, npiSem,(unsigned char *) pMsg);
#endif //ICALL_EVENTS
        }
        break;
        default:
            status = NPI_INVALID_PKT;
        break;
    }

    NPIUtil_ExitCS(key);

    return status;
}

// -----------------------------------------------------------------------------
//! \brief      API for subsystems to register for NPI messages received with
//!             the specific ssID. All NPI messages will be passed to callback
//!             provided
//!
//! \param[in]  ssID    The subsystem ID of NPI messages that should be routed
//!                     to pCB
//! \param[in]  pCB     The call back function that will receive NPI messages
//!
//! \return     uint8_t Status NPI_SUCCESS, or NPI_ROUTING_FULL
// -----------------------------------------------------------------------------
uint8_t NPITask_regSSFromHostCB(uint8_t ssID, npiFromHostCBack_t pCB)
{
    uint8_t i;

    for (i = 0; i < NPI_MAX_SS_ENTRY; i++)
    {
        if (HostToSSTable[i].ssCB == NULL)
        {
            HostToSSTable[i].ssID = ssID;
            HostToSSTable[i].ssCB = pCB;
            return NPI_SUCCESS;
        }
    }

    return NPI_ROUTING_FULL;
}

#ifdef USE_ICALL
// -----------------------------------------------------------------------------
//! \brief      API for subsystems to register for ICall messages received from
//!             the specific source entity ID. All ICall messages will be passed
//!             to the callback provided
//!
//! \param[in]  icallID Source entity ID whose messages should be sent to pCB
//!             pCB     The call back function that will receive ICall messages
//!
//! \return     uint8_t Status NPI_SUCCESS, or NPI_ROUTING_FULL
// -----------------------------------------------------------------------------
uint8_t NPITask_regSSFromICallCB(uint8_t icallID, npiFromICallCBack_t pCB)
{
    uint8_t i;

    for (i = 0; i < NPI_MAX_ICALL_ENTRY; i++)
    {
        if (ICallToSSTable[i].ssCB == NULL)
        {
            ICallToSSTable[i].icallID = icallID;
            ICallToSSTable[i].ssCB = pCB;

            return NPI_SUCCESS;
        }
    }

    return NPI_ROUTING_FULL;
}
#endif //USE_ICALL

// -----------------------------------------------------------------------------
// Routing Functions

// -----------------------------------------------------------------------------
//! \brief      Function to route NPI Message to the appropriate subsystem
//!
//! \param[in]  pNPIMsg Pointer to message that will be routed
//!
//! \return     uint8_t Status NPI_SUCCESS, or NPI_SS_NOT_FOUND
// -----------------------------------------------------------------------------
static uint8_t NPITask_routeHostToSS(_npiFrame_t *pNPIMsg)
{
    uint8_t i;
    uint8_t ssIDtoRoute = NPI_GET_SS_ID(pNPIMsg);

    for (i = 0; i < NPI_MAX_SS_ENTRY; i++)
    {
        if (ssIDtoRoute == HostToSSTable[i].ssID)
        {
            HostToSSTable[i].ssCB(pNPIMsg);

            return NPI_SUCCESS;
        }
    }

    return NPI_SS_NOT_FOUND;
}

#ifdef USE_ICALL
// -----------------------------------------------------------------------------
//! \brief      Function to route ICall Message to the appropriate subsystem
//!
//! \param[in]  src     ICall Message source Entity ID
//! \param[in]  pGenMsg Pointer to generic ICall message that will be routed
//!
//! \return     uint8_t Status NPI_SUCCESS, or NPI_SS_NOT_FOUND
// -----------------------------------------------------------------------------
static uint8_t NPITask_routeICallToSS(ICall_ServiceEnum src, uint8_t *pGenMsg)
{
    uint8_t i;

    for (i = 0; i < NPI_MAX_ICALL_ENTRY; i++)
    {
        if (src == ICallToSSTable[i].icallID)
        {
            ICallToSSTable[i].ssCB(pGenMsg);
            return NPI_SUCCESS;
        }
    }
    return NPI_SS_NOT_FOUND;
}
#endif //USE_ICALL


// -----------------------------------------------------------------------------
//! \brief      API to allocate an NPI frame of a given data length
//!
//! \param[in]  len             Length of data field of frame
//!
//! \return     _npiFrame_t *   Pointer to newly allocated frame
// -----------------------------------------------------------------------------
_npiFrame_t * NPITask_mallocFrame(uint16_t len)
{
    _npiFrame_t *pMsg;

    // Allocate memory for NPI Frame
    pMsg = (_npiFrame_t *)NPIUtil_malloc(sizeof(_npiFrame_t) + len);

    if (pMsg != NULL)
    {
        // Assign Data Length of Frame
        pMsg->dataLen = len;

        // Assign pData to first byte of payload
        // Pointer arithmetic of + 1 is equal to sizeof(_npiFrame_t) bytes
        // then cast to unsigned char * for pData
        pMsg->pData = (unsigned char *)(pMsg + 1);
    }
    else
    {
       HAL_ASSERT(HAL_ASSERT_CAUSE_OUT_OF_MEMORY);
    }
    return pMsg;
}

// -----------------------------------------------------------------------------
//! \brief      API to de-allocate an NPI frame
//!
//! \param[in]  frame   Pointer to NPI frame to be de-allocated
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITask_freeFrame(_npiFrame_t *frame)
{
    NPIUtil_free((uint8_t *)frame);
}

// -----------------------------------------------------------------------------
// Serialize and Deserialize functions

// -----------------------------------------------------------------------------
//! \brief      Function creates a byte array from an NPI Frame struct
//!
//!             *** Whoever calls this function is responsible for freeing the
//!             the created byte array
//!
//! \param[in]  pNPIMsg     Pointer to message that will be serialized
//!
//! \return     uint8_t*    Equivalent byte array of the NPI Frame
// -----------------------------------------------------------------------------
static uint8_t * NPITask_SerializeFrame(_npiFrame_t *pNPIMsg)
{
    uint8_t *pSerMsg;

    // Allocate byte array for the entire message
    pSerMsg = NPIUtil_malloc(pNPIMsg->dataLen + NPI_MSG_HDR_LENGTH);

    if (pSerMsg != NULL)
    {
        // Packet Format [ Len1 ][ Len0 ][ Cmd0 ][ Cmd 1 ][ Data Payload ]
        // Fill in Header
        pSerMsg[0] = (uint8)(pNPIMsg->dataLen & 0xFF);
        pSerMsg[1] = (uint8)(pNPIMsg->dataLen >> 8);
        pSerMsg[2] = pNPIMsg->cmd0;
        pSerMsg[3] = pNPIMsg->cmd1;

        // Copy Data Payload
        memcpy(&pSerMsg[4],pNPIMsg->pData,pNPIMsg->dataLen);
    }
    else
    {
      HAL_ASSERT(HAL_ASSERT_CAUSE_OUT_OF_MEMORY);
    }

    return pSerMsg;
}


// -----------------------------------------------------------------------------
//! \brief      Function creates an NPI Frame struct from RxBuf contents
//!
//!             *** Whoever calls this function is responsible for freeing the
//!             the created NPI Frame struct
//!
//! \return     _npiFrame_t*  Equivalent NPI Frame of RxBuf
// -----------------------------------------------------------------------------
static _npiFrame_t * NPITask_DeserializeFrame(void)
{
    _npiFrame_t *pMsg;
    uint16_t datalen;
    uint8_t ch;

    // Function assumes the following packet structure is in RxBuf:
    // [ Len1 ][ Len0 ][ Cmd0 ][ Cmd 1 ][ Data Payload ]
    // It also assumes that npiRxBufHead points to the Len1 byte
    NPITL_readTL(&ch, 1);
    datalen = ch;
    NPITL_readTL(&ch, 1);
    datalen += (ch << 8);

    // Allocate memory for NPI Frame
    pMsg = NPITask_mallocFrame(datalen);

    if (pMsg != NULL)
    {
        // Assign CMD0 and CMD1 bytes
        NPITL_readTL(&(pMsg->cmd0), 1);
        NPITL_readTL(&(pMsg->cmd1), 1);

        // Copy Data payload
        NPITL_readTL(pMsg->pData, pMsg->dataLen);
    }

    return pMsg;
}

// -----------------------------------------------------------------------------
// "Processor" functions

// -----------------------------------------------------------------------------
//! \brief      Dequeue next message in the ASYNC TX Queue and send to serial
//!             interface.
//!
//! \param[in]  txQ    queue handle to be processed
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_ProcessTXQ(Queue_Handle txQ)
{
    _npiFrame_t *pMsg = NULL;
    _npiCSKey_t key;

    // Must block task pre-emption so that the higher priority tasks
    // are not also manipulated txQ at the same time
    key = NPIUtil_EnterCS();
    pMsg = (_npiFrame_t *) NPIUtil_dequeueMsg(txQ);
    NPIUtil_ExitCS(key);

    if (pMsg != NULL)
    {
        // Serialize NPI Frame to be sent over Transport Layer
        if(lastQueuedTxMsg != NULL)
        {
          NPIUtil_free(lastQueuedTxMsg);
          lastQueuedTxMsg = NULL;
        }
        lastQueuedTxMsg = NPITask_SerializeFrame(pMsg);

        if (lastQueuedTxMsg != NULL)
        {
          // Write byte array over Transport Layer
          // We have already checked if TL is busy so we assume write succeeds
          NPITL_writeTL(lastQueuedTxMsg, pMsg->dataLen + NPI_MSG_HDR_LENGTH);
          // If the message is a synchronous response or request
          if (NPI_GET_MSG_TYPE(pMsg) == NPI_MSG_TYPE_SYNCREQ ||
              NPI_GET_MSG_TYPE(pMsg) == NPI_MSG_TYPE_SYNCRSP)
          {
              // Decrement the outstanding Sync REQ/RSP flag.
              syncTransactionInProgress--;
          }
        }

        //Free NPI frame
        NPITask_freeFrame(pMsg);
    }
}

// -----------------------------------------------------------------------------
//! \brief      Dequeue next message in the RX Queue and process it.
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_processRXQ(void)
{
    _npiCSKey_t key;
    _npiFrame_t *pMsg = NULL;

    // Must lock interrupts because RX Queues are accessed from
    // both NPI Task context and ISR
    key = NPIUtil_EnterCS();
    pMsg = (_npiFrame_t *) NPIUtil_dequeueMsg(npiRxQueue);
    NPIUtil_ExitCS(key);

    if (pMsg != NULL)
    {
        // Route to SS based on ID in message
        if (NPITask_routeHostToSS(pMsg) != NPI_SUCCESS)
        {
            // No subsystem registered to handle message. Free NPI Frame
            NPITask_freeFrame(pMsg);
        }
    }
}

// -----------------------------------------------------------------------------
//! \brief      Dequeue next message in the RX Queue and process it.
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_processSyncRXQ(void)
{
    _npiCSKey_t key;
    _npiFrame_t *pMsg = NULL;

    // Sync transaction of 0 means no synchronous transaction is in progress
    // A value of less than zero means that we have sent a sync message and
    // are waiting on the reply
    if (syncTransactionInProgress <= 0)
    {
        // Must lock interrupts because RX Queues are accessed from
        // both NPI Task context and ISR
        key = NPIUtil_EnterCS();
        pMsg = (_npiFrame_t *) NPIUtil_dequeueMsg(npiSyncRxQueue);
        NPIUtil_ExitCS(key);

        if (pMsg != NULL)
        {
            // Route to SS based on ID in message
            if (NPITask_routeHostToSS(pMsg) == NPI_SUCCESS)
            {
              // Increment the outstanding Sync REQ/RSP flag.
              syncTransactionInProgress++;
            }
            else
            {
                // No subsystem registered to handle message. Free NPI Frame
                NPITask_freeFrame(pMsg);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Call Back Functions

// -----------------------------------------------------------------------------
//! \brief      Transaction Done callback provided to Transport Layer for
//!
//! \param[in]  uint16_t    Number of bytes received.
//! \param[in]  uint16_t    Number of bytes transmitted.
//!
//! \return     void
// -----------------------------------------------------------------------------
volatile uint8_t txcomplete = 0;
volatile uint8_t rxcomplete = 0;
static void NPITask_transportDoneCallBack(uint16_t sizeRx, uint16_t sizeTx)
{
    _npiFrame_t *pMsg = NULL;

    if (sizeRx != 0)
    {
        rxcomplete++;
        // De-serialize byte array into NPI frame struct
        pMsg = NPITask_DeserializeFrame();

        if (pMsg)
        {
            switch (NPI_GET_MSG_TYPE(pMsg))
            {
                // Enqueue to appropriate NPI Task Q and post corresponding event.
                case NPI_MSG_TYPE_SYNCREQ:
                case NPI_MSG_TYPE_SYNCRSP:
                {
#ifdef ICALL_EVENTS
                    NPIUtil_enqueueMsg(npiSyncRxQueue, syncEvent,
                                       NPITASK_SYNC_FRAME_RX_EVENT,
                                       (unsigned char *) pMsg);
#else //!ICALL_EVENTS
                    tlDoneISRFlag |= NPITASK_SYNC_FRAME_RX_EVENT;
                    NPIUtil_enqueueMsg(npiSyncRxQueue, npiSem, (unsigned char *) pMsg);
#endif //ICALL_EVENTS
                    break;
                }
                case NPI_MSG_TYPE_ASYNC:
                {
#ifdef ICALL_EVENTS
                    NPIUtil_enqueueMsg(npiRxQueue, syncEvent,
                                       NPITASK_FRAME_RX_EVENT,
                                       (unsigned char *) pMsg);
#else //!ICALL_EVENTS
                    tlDoneISRFlag |= NPITASK_FRAME_RX_EVENT;
                    NPIUtil_enqueueMsg(npiRxQueue, npiSem, (unsigned char *) pMsg);
#endif //ICALL_EVENTS
                    break;
                }
                default:
                {
                    // Unexpected Msg Type. Free Msg
                    NPITask_freeFrame(pMsg);
                    break;
                }
            }
        }
    }

    if (sizeTx && lastQueuedTxMsg)
    {
      txcomplete++;
#ifdef ICALL_EVENTS
      Event_post(syncEvent, NPITASK_TX_DONE_EVENT);
#else //!ICALL_EVENTS
      tlDoneISRFlag |= NPITASK_TX_DONE_EVENT;
      Semaphore_post(npiSem);
#endif //ICALL_EVENTS
    }

    // Check to see if there pending messages waiting to be sent
    // If there are then notify NPI Task by setting TX READY event flag
    if (!Queue_empty(npiSyncTxQueue) || !Queue_empty(npiTxQueue))
    {
        // There are pending SYNC RSP or ASYNC messages waiting to
        // be sent to the host. Set the appropriate flag and post to
        // the semaphore.
#ifdef ICALL_EVENTS
        Event_post(syncEvent, NPITASK_TX_READY_EVENT);
#else //!ICALL_EVENTS
        tlDoneISRFlag |= NPITASK_TX_READY_EVENT;
        Semaphore_post(npiSem);
#endif //ICALL_EVENTS
    }
}

// -----------------------------------------------------------------------------
//! \brief      RX Callback provided to Transport Layer for REM RDY Event
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_RemRdyEventCB(uint8_t state)
{
  if (state == REM_RDY_ASSERTED
#ifdef ICALL_EVENTS
      && syncEvent != NULL)
#else //!ICALL_EVENTS
      && npiSem != NULL)
#endif //ICALL_EVENTS
  {
#ifdef ICALL_EVENTS
    Event_post(syncEvent, NPITASK_REM_RDY_EVENT);
#else //!ICALL_EVENTS
    remRdyISRFlag = NPITASK_REM_RDY_EVENT;
    Semaphore_post(npiSem);
#endif //ICALL_EVENTS
  }
#ifdef NPI_MASTER
  else if (state == REM_RDY_DEASSERTED
#ifdef ICALL_EVENTS
      )
#else //!ICALL_EVENTS
      && npiSem != NULL)
#endif //ICALL_EVENTS
  {
    // There could be pending TX messages that are waiting for Remote Ready
    // signal to be deasserted so that NPI is no longer busy
    if (!Queue_empty(npiSyncTxQueue) || !Queue_empty(npiTxQueue))
    {
#ifdef ICALL_EVENTS
        Event_post(syncEvent, NPITASK_TX_READY_EVENT);
#else //!ICALL_EVENTS
        remRdyISRFlag |= NPITASK_TX_READY_EVENT;
        Semaphore_post(npiSem);
#endif //ICALL_EVENTS
    }
  }
#endif //NPI_MASTER
}

// -----------------------------------------------------------------------------
//! \brief   Send a final out of memory message if there is an allocation
//!          failure to Host.
// -----------------------------------------------------------------------------
void NPITask_sendAssertMsg(uint8_t assertMsg)
{
  // Disable all task switching and interrupts
  Hwi_disable();
  Swi_disable();
  Task_disable();

  // Setup and send final assert message via NPI
  sendAssertMessage[4] = assertMsg;
  NPITL_writeBypassSafeTL(&sendAssertMessage[0], NPI_ASSERT_MSG_LEN);

  // Spinlock doesn't work when power savings enbabled
#ifndef POWER_SAVING
  HAL_ASSERT_SPINLOCK;
#endif
}

// -----------------------------------------------------------------------------
//! \brief   change the NPI header of the pre-defined Assert NPI message
//!          currently hardcoded set to {RPC_SYS_BLE_SNP, NPI_ASSERT_CMD1_ID}.
// -----------------------------------------------------------------------------
void NPITask_chgAssertHdr(uint8_t npi_cmd0, uint8_t npi_cmd1)
{
  _npiCSKey_t key;

  key = NPIUtil_EnterCS();

  sendAssertMessage[2] = npi_cmd0;
  sendAssertMessage[3] = npi_cmd1;

  NPIUtil_ExitCS(key);
}

// -----------------------------------------------------------------------------
//! \brief   Trigger a final NPI message for certain assert events.
// -----------------------------------------------------------------------------
#ifdef ICALL_EVENTS
void NPIData_postAssertNpiMsgEvent(uint8_t assertType)
{
  npiTask_assertType = assertType;
  if (syncEvent == NULL)
  {
    //NPI still not opened
    return;
  }
  Event_post(syncEvent, NPITASK_ASSERT_MSG_EVENT);
}
#else //!ICALL_EVENTS
void NPIData_postAssertNpiMsgEvent(uint8_t assertType)
{
  npiTask_assertType = assertType;
  if (npiSem == NULL)
  {
    //NPI still not opened
    return;
  }
  NPITask_events |= NPITASK_ASSERT_MSG_EVENT;
  Semaphore_post(npiSem);
}
#endif //ICALL_EVENTS

