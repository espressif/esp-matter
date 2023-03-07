/******************************************************************************

 @file  npi_task.c

 @brief NPI is a TI RTOS Application Thread that provides a ! common
        Network Processor Interface framework.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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

// ****************************************************************************
// includes
// ****************************************************************************

#include <xdc/std.h>
#ifdef ICALL_EVENTS
#include <ti/sysbios/knl/Event.h>
#else //!ICALL_EVENTS
#include <ti/sysbios/knl/Semaphore.h>
#endif //ICALL_EVENTS
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/BIOS.h>

#include <string.h>
#include "icall.h"

#include "inc/npi_task.h"
#include "inc/npi_data.h"
#include "inc/npi_frame.h"
#include "inc/npi_rxbuf.h"
#include "inc/npi_tl.h"

// ****************************************************************************
// defines
// ****************************************************************************

#ifdef ICALL_EVENTS
#define NPITASK_ICALL_EVENT ICALL_MSG_EVENT_ID // Event_Id_31

//! \brief Transport layer RX Event (ie. bytes received, RX ISR etc.)
#define NPITASK_TRANSPORT_RX_EVENT Event_Id_00

//! \brief Transmit Complete Event (likely associated with TX ISR etc.)
#define NPITASK_TRANSPORT_TX_DONE_EVENT Event_Id_01

//! \brief ASYNC Message Received Event (no framing bytes)
#define NPITASK_FRAME_RX_EVENT Event_Id_02

//! \brief A framed message buffer is ready to be sent to the transport layer.
#define NPITASK_TX_READY_EVENT Event_Id_03

#if defined(NPI_SREQRSP)
//! \brief ASYNC Message Received Event (no framing bytes)
#define NPITASK_SYNC_FRAME_RX_EVENT Event_Id_04

//! \brief A SYNC framed message buffer is ready to be sent to the transport layer.
#define NPITASK_SYNC_TX_READY_EVENT Event_Id_05

//! \brief SYNC REQ/RSP Watchdog Timer Duration (in ms)
#define NPITASK_WD_TIMEOUT 500
#else // !NPI_SREQRSP
#define NPITASK_SYNC_FRAME_RX_EVENT Event_Id_NONE
#define NPITASK_SYNC_TX_READY_EVENT Event_Id_NONE
#endif // NPI_SREQRSP

//! \brief MRDY Received Event
#define NPITASK_MRDY_EVENT Event_Id_06

#define NPITASK_ALL_EVENTS (NPITASK_ICALL_EVENT | NPITASK_TRANSPORT_RX_EVENT | \
                            NPITASK_TRANSPORT_TX_DONE_EVENT | \
                            NPITASK_FRAME_RX_EVENT | NPITASK_TX_READY_EVENT | \
                            NPITASK_SYNC_FRAME_RX_EVENT | \
                            NPITASK_SYNC_TX_READY_EVENT | NPITASK_MRDY_EVENT)

#else //!ICALL_EVENTS
//! \brief Transport layer RX Event (ie. bytes received, RX ISR etc.)
#define NPITASK_TRANSPORT_RX_EVENT 0x0002

//! \brief Transmit Complete Event (likely associated with TX ISR etc.)
#define NPITASK_TRANSPORT_TX_DONE_EVENT 0x0004

//! \brief ASYNC Message Received Event (no framing bytes)
#define NPITASK_FRAME_RX_EVENT 0x0008

//! \brief A framed message buffer is ready to be sent to the transport layer.
#define NPITASK_TX_READY_EVENT 0x0010

#if defined(NPI_SREQRSP)
//! \brief ASYNC Message Received Event (no framing bytes)
#define NPITASK_SYNC_FRAME_RX_EVENT 0x0020

//! \brief A SYNC framed message buffer is ready to be sent to the transport layer.
#define NPITASK_SYNC_TX_READY_EVENT 0x0040

//! \brief SYNC REQ/RSP Watchdog Timer Duration (in ms)
#define NPITASK_WD_TIMEOUT 500
#endif // NPI_SREQRSP

//! \brief MRDY Received Event
#define NPITASK_MRDY_EVENT 0x0080

#endif //ICALL_EVENTS

#if defined(CC26X2) || defined(CC13X2) || defined(CC13X2P)
#define NPITASK_STACK_SIZE (130*8)     /* in order to optimize memory, this value should be a multiple of 8 bytes */
#else // !CC26X2 && !CC13X2
//! \brief Size of stack created for NPI RTOS task
#ifndef Display_DISABLE_ALL
#ifdef __TI_COMPILER_VERSION__
#define NPITASK_STACK_SIZE 752      /* in order to optimize memory, this value should be a multiple of 8 bytes */
#else // !__TI_COMPILER_VERSION__
#define NPITASK_STACK_SIZE 656      /* in order to optimize memory, this value should be a multiple of 8 bytes */
#endif // __TI_COMPILER_VERSION__
#else // Display_DISABLE_ALL
#ifdef __TI_COMPILER_VERSION__
#define NPITASK_STACK_SIZE 752      /* in order to optimize memory, this value should be a multiple of 8 bytes */
#else // !__TI_COMPILER_VERSION__
#define NPITASK_STACK_SIZE 608      /* in order to optimize memory, this value should be a multiple of 8 bytes */
#endif // __TI_COMPILER_VERSION__
#endif // Display_DISABLE_ALL
#endif // CC26X2 || CC13X2

//! \brief Task priority for NPI RTOS task
#define NPITASK_PRIORITY 2


// ****************************************************************************
// typedefs
// ****************************************************************************

//! \brief Queue record structure
//!
typedef struct NPI_QueueRec_t
{
    Queue_Elem _elem;
    NPIMSG_msg_t *npiMsg;
} NPI_QueueRec;


//*****************************************************************************
// globals
//*****************************************************************************

//! \brief ICall ID for stack which will be sending NPI messages
//!
static uint32_t stackServiceID = 0x0000;

//! \brief RTOS task structure for NPI task
//!
static Task_Struct npiTaskStruct;

//! \brief Allocated memory block for NPI task's stack
//!
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(npiTaskStack, 8)
#elif defined __clang__
__attribute__ ((aligned (8)))
#else
#pragma data_alignment=8
#endif
uint8_t npiTaskStack[NPITASK_STACK_SIZE];

//! \brief Handle for the ASYNC TX Queue
//!
static Queue_Handle npiTxQueue;

//! \brief Handle for the ASYNC RX Queue
//!
static Queue_Handle npiRxQueue;

#if defined(NPI_SREQRSP)
//! \brief Handle for the SYNC TX Queue
//!
static Queue_Handle npiSyncTxQueue;

//! \brief Handle for the SYNC RX Queue
//!
static Queue_Handle npiSyncRxQueue;

//! \brief Flag/Counter indicating a Synchronous REQ/RSP is currently being
//!        processed.
static int8_t syncTransactionInProgress = 0;

//! \brief Clock Struct for Sync REQ/RSP watchdog timer
//!
static Clock_Struct syncReqRspWatchDogClkStruct;
static Clock_Handle syncReqRspWatchDogClkHandle;
#endif // NPI_SREQRSP

//! \brief Pointer to last tx message.  This is free'd once confirmation is
//!        is received that the buffer has been transmitted
//!        (ie. NPITASK_TRANSPORT_TX_DONE_EVENT)
//!
static uint8_t *lastQueuedTxMsg;

//! \brief NPI thread ICall Semaphore.
//!
#define FLE_DEBUG 1

#ifdef ICALL_EVENTS
ICall_SyncHandle syncEvent;
#else //!ICALL_EVENTS
#ifdef FLE_DEBUG
static ICall_Semaphore appSem = NULL;
#else
static Semaphore_Struct structSem;
static Semaphore_Handle appSem;
#endif //FLE_DEBUG
#endif //ICALL_EVENTS

//! \brief NPI ICall Application Entity ID.
//!
ICall_EntityID npiAppEntityID = 0;

#ifndef ICALL_EVENTS
//! \brief Task pending events
//!
static uint16_t NPITask_events = 0;

//! \brief Event flags for capturing Task-related events from ISR context
//!
static uint16_t TX_DONE_ISR_EVENT_FLAG = 0;
static uint16_t MRDY_ISR_EVENT_FLAG = 0;
static uint16_t TRANSPORT_RX_ISR_EVENT_FLAG = 0;
#endif //ICALL_EVENTS

//! \brief Pointer to Application RX event callback function for optional
//!        rerouting of messages to application.
//!
static npiIncomingEventCBack_t incomingRXEventAppCBFunc = NULL;

//! \brief Type of rerouting for RX messages requested by Application
//!
static NPI_IncomingNPIEventRerouteType incomingRXReroute = NONE;


//! \brief Pointer to Application TX event callback function for optional
//!        rerouting of messages to application.
//!
static npiIncomingEventCBack_t incomingTXEventAppCBFunc = NULL;

//! \brief Type of rerouting for TX messages requested by Application
//!
static NPI_IncomingNPIEventRerouteType incomingTXReroute = NONE;

//*****************************************************************************
// function prototypes
//*****************************************************************************

//! \brief      NPI main event processing loop.
//!
static void NPITask_process(void);

//! \brief Callback function registered with Transport Layer
//!
static void NPITask_transportRXCallBack(int size);

//! \brief Callback function registered with Transport Layer
//!
static void NPITask_transportTxDoneCallBack(int size);

//! \brief Callback function registered with Transport Layer
//!
static void NPITask_MRDYEventCB(int size);

//! \brief ASYNC TX Q Processing function.
//!
static void NPITask_ProcessTXQ(void);

#if defined(NPI_SREQRSP)
//! \brief SYNC TX Q Processing function.
//!
static void NPITask_ProcessSyncTXQ(void);

//! \brief SYNC RX Q Processing function.
//!
static void NPITask_processSyncRXQ(void);

//! \brief Sync REQ/RSP Watchdog Timer CB
//!
static void syncReqRspWatchDogTimeoutCB( UArg a0 );
#endif // NPI_SREQRSP

//! \brief ASYNC RX Q Processing function.
//!
static void NPITask_processRXQ(void);

//! \brief Callback function registered with Frame module to handle successful
//!        reception of message from host.
//!
static void NPITask_incomingFrameCB(uint16_t frameSize, uint8_t *pFrame,
                                    NPIMSG_Type msgType);

//! \brief Function to send message buffer to stack task.  Note, message buffer
//!        referenced by NPIMSG_msg_t is "unframed".
//!
static ICall_Errno NPITask_sendBufToStack(ICall_EntityID appEntity,
                                          NPIMSG_msg_t *msg);

//! \brief Function to handle incoming ICall Message Event
//!
static uint_least16_t NPITask_processICallMsgEvent(uint8_t *pMsg,
                                                   ICall_ServiceEnum src,
                                                   ICall_EntityID dest);

//! \brief Function to process incoming Msg from stack task.
//!
static void NPITask_processStackMsg(uint8_t *pMsg);


// -----------------------------------------------------------------------------
//! \brief      Initialization for the NPI Thread
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_inititializeTask(void)
{
#ifndef ICALL_EVENTS
    NPITask_events = 0;
#endif //ICALL_EVENTS

    lastQueuedTxMsg = NULL;

    // create a Tx Queue instance
    npiTxQueue = Queue_create(NULL, NULL);
    // create an Rx Queue instance
    npiRxQueue = Queue_create(NULL, NULL);

#if defined(NPI_SREQRSP)
    // create an Sync RX Queue instance
    npiSyncRxQueue = Queue_create(NULL, NULL);
    // create an Sync TX Queue instance
    npiSyncTxQueue = Queue_create(NULL, NULL);

    // Create clock for SYNC REQ/RSP message watchdog
    Clock_Params clockParams;

    // Convert clockDuration in milliseconds to ticks.
    uint32_t clockTicks = NPITASK_WD_TIMEOUT * (1000 / Clock_tickPeriod);

    // Setup parameters.
    Clock_Params_init(&clockParams);

    // If period is 0, this is a one-shot timer.
    clockParams.period = 0;

    // Starts immediately after construction if true, otherwise wait for a call
    // to start.
    clockParams.startFlag = 0;

    // Initialize clock instance.
    Clock_construct(&syncReqRspWatchDogClkStruct, syncReqRspWatchDogTimeoutCB,
                    clockTicks, &clockParams);

    syncReqRspWatchDogClkHandle = Clock_handle(&syncReqRspWatchDogClkStruct);
#endif // NPI_SREQRSP

    /* Enroll the service that this stack represents */
#ifdef ICALL_EVENTS
    ICall_enrollService ( ICALL_SERVICE_CLASS_NPI, NULL, &npiAppEntityID,
                          &syncEvent );
#else //!ICALL_EVENTS
#ifdef FLE_DEBUG
    ICall_enrollService( ICALL_SERVICE_CLASS_NPI, NULL, &npiAppEntityID, &appSem );
#else
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&structSem, 1, &semParams);
    appSem = Semaphore_handle(&structSem);
#endif //FLE_DEBUG
#endif //ICALL_EVENTS

    // Initialize Frame Module
    NPIFrame_initialize(&NPITask_incomingFrameCB);

    // Initialize Network Processor Interface (NPI) and Transport Layer
    NPITL_initTL( &NPITask_transportTxDoneCallBack,
                  &NPITask_transportRXCallBack,
                  &NPITask_MRDYEventCB );

}


// -----------------------------------------------------------------------------
//! \brief      NPI main event processing loop.
//!
//! \return     void
// -----------------------------------------------------------------------------
#define ITM_Port32(n) (*((volatile unsigned int *)(0xE0000000+4*n)))

static void NPITask_process(void)
{
    ICall_ServiceEnum stackid;
    ICall_EntityID dest;
    uint8_t *pMsg;
    ICall_CSState key;

    /* Forever loop */
    for (;; )
    {
        /* Wait for response message */

#ifdef ICALL_EVENTS
        uint32_t NPITask_events;

        NPITask_events = Event_pend(syncEvent, Event_Id_NONE,
                                    NPITASK_ALL_EVENTS, BIOS_WAIT_FOREVER);
#else //!ICALL_EVENTS
        //if (ICall_wait(ICALL_TIMEOUT_FOREVER) == ICALL_ERRNO_SUCCESS)
        Semaphore_pend(appSem, BIOS_WAIT_FOREVER);
#endif //ICALL_EVENTS
        {
            // Capture the ISR events flags now within a critical section.
            // We do this to avoid possible race conditions where the ISR is
            // modifying the event mask while the task is read/writing it.
            key = ICall_enterCriticalSection();

#ifndef ICALL_EVENTS
            NPITask_events = NPITask_events | TX_DONE_ISR_EVENT_FLAG |
                             MRDY_ISR_EVENT_FLAG | TRANSPORT_RX_ISR_EVENT_FLAG;

            TX_DONE_ISR_EVENT_FLAG = 0;
            MRDY_ISR_EVENT_FLAG = 0;
            TRANSPORT_RX_ISR_EVENT_FLAG = 0;
#endif //ICALL_EVENTS

            ICall_leaveCriticalSection(key);

            // MRDY event
            if (NPITask_events & NPITASK_MRDY_EVENT)
            {
#ifndef ICALL_EVENTS
                NPITask_events &= ~NPITASK_MRDY_EVENT;
#endif //ICALL_EVENTS
#if (NPI_FLOW_CTRL == 1)
                NPITL_handleMrdyEvent();
#endif // NPI_FLOW_CTRL = 1

            }

#if defined(NPI_SREQRSP)
            // Something is ready to send to the Host
            if(NPITask_events & NPITASK_SYNC_TX_READY_EVENT)
            {

                if (syncTransactionInProgress)
                {
                    // Prioritize Synchronous traffic
                    if ((!Queue_empty(npiSyncTxQueue)) && !NPITL_checkNpiBusy())
                    {
                        // Push the pending Sync RSP to the host.
                        NPITask_ProcessSyncTXQ();
                    }
                }
                else
                {
                    // Not expected
                }

                if (Queue_empty(npiSyncTxQueue))
                {
#ifndef ICALL_EVENTS
                    // If the Sync Q is empty now (and it should be) clear the
                    // event.
                    NPITask_events &= ~NPITASK_SYNC_TX_READY_EVENT;
#endif //ICALL_EVENTS
                }
                else
                {
                    // If the Sync Q is not empty now :
                    // - It means we're handling "stacked" SYNC REQ/RSP's
                    //   (which shouldn't be happening).
                    // - Preserve the event flag and repost on the semaphore.
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_SYNC_TX_READY_EVENT);
#else //!ICALL_EVENTS
                    Semaphore_post(appSem);
#endif //ICALL_EVENTS
                }
            }
#endif // NPI_SREQRSP

            // ICall Message Event
            if (ICall_fetchServiceMsg(&stackid, &dest, (void * *) &pMsg)
                == ICALL_ERRNO_SUCCESS)
            {
                NPITask_processICallMsgEvent( pMsg, stackid, dest );
            }

#if defined(NPI_SREQRSP)
            // Synchronous Frame received from Host
            if(NPITask_events & NPITASK_SYNC_FRAME_RX_EVENT)
            {
                // Process it
                NPITask_processSyncRXQ();

                if (Queue_empty(npiSyncRxQueue))
                {
#ifndef ICALL_EVENTS
                    // Q is empty, it's safe to clear the event flag.
                    NPITask_events &= ~NPITASK_SYNC_FRAME_RX_EVENT;
#endif //ICALL_EVENTS
                }
                else
                {
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_SYNC_FRAME_RX_EVENT);
#else //!ICALL_EVENTS
                    // Q is not empty, there's more to handle so preserve the
                    // flag and repost to the task semaphore.
                    Semaphore_post(appSem);
#endif //ICALL_EVENTS
                }
            }
#endif // NPI_SREQRSP

            // An ASYNC message is ready to send to the Host
            if(NPITask_events & NPITASK_TX_READY_EVENT)
            {
#if defined(NPI_SREQRSP)
                // Check for outstanding SYNC REQ/RSP transactions.  If so,
                // this ASYNC message must remain Q'd while we wait for the
                // SYNC RSP.
                if (syncTransactionInProgress  == 0)
                {
                    // No outstanding SYNC REQ/RSP transactions, process
                    // ASYNC messages.
#endif // NPI_SREQRSP
                    if ((!Queue_empty(npiTxQueue)) && !NPITL_checkNpiBusy())
                    {
                        // Push the pending Async Msg to the host.
                        NPITask_ProcessTXQ();
                    }
#if defined(NPI_SREQRSP)
                }
#endif // NPI_SREQRSP

                if (Queue_empty(npiTxQueue))
                {
#ifndef ICALL_EVENTS
                    // Q is empty, it's safe to clear the event flag.
                    NPITask_events &= ~NPITASK_TX_READY_EVENT;
#endif //ICALL_EVENTS
                }
                else
                {
                    // Q is not empty, there's more to handle so preserve the
                    // flag and repost to the task semaphore.
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_TX_READY_EVENT);
#else //!ICALL_EVENTS
                    Semaphore_post(appSem);
#endif //ICALL_EVENTS
                }
            }


            // The Transport Layer has received some bytes
            if(NPITask_events & NPITASK_TRANSPORT_RX_EVENT)
            {
                // Call the packet/frame collector parser.  This function is
                // specific to the supported technology:
                // - HCI for BLE
                // - MT for ZigBee, TIMAC, RF4CE
                // - ? for your favorite technology
                NPIFrame_collectFrameData();

                if (NPIRxBuf_GetRxBufCount() == 0)
                {
#ifndef ICALL_EVENTS
                    // No additional bytes to collect, clear the flag.
                    NPITask_events &= ~NPITASK_TRANSPORT_RX_EVENT;
#endif //ICALL_EVENTS
                }
                else
                {
                    // Additional bytes to collect, preserve the flag and repost
                    // to the semaphore
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_TRANSPORT_RX_EVENT);
#else //!ICALL_EVENTS
                    Semaphore_post(appSem);
#endif //ICALL_EVENTS
                }
            }

            // A complete frame (msg) has been received and is ready for handling
            if(NPITask_events & NPITASK_FRAME_RX_EVENT)
            {
#if defined(NPI_SREQRSP)
                // Check for outstanding SYNC REQ/RSP transactions.  If so,
                // this ASYNC message must remain Q'd while we wait for the
                // SYNC RSP.
                if (syncTransactionInProgress == 0)
                {
#endif // NPI_SREQRSP
                    // Process the ASYNC message
                    NPITask_processRXQ();
                    if (Queue_empty(npiRxQueue))
                    {
#ifndef ICALL_EVENTS
                        // Q is empty, it's safe to clear the event flag.
                        NPITask_events &= ~NPITASK_FRAME_RX_EVENT;
#endif //ICALL_EVENTS
                    }
                    else
                    {
                        // Q is not empty, there's more to handle so preserve the
                        // flag and repost to the task semaphore.
#ifdef ICALL_EVENTS
                        Event_post(syncEvent, NPITASK_FRAME_RX_EVENT);
#else //!ICALL_EVENTS
                        Semaphore_post(appSem);
#endif //ICALL_EVENTS
                    }
#if defined(NPI_SREQRSP)
                }
                else
                {
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_FRAME_RX_EVENT);
#else //!ICALL_EVENTS
                    // Preserve the flag and repost to the task semaphore.
                    Semaphore_post(appSem);
#endif //ICALL_EVENTS
                }
#endif // NPI_SREQRSP
            }

            // The last transmission to the host has completed.
            if(NPITask_events & NPITASK_TRANSPORT_TX_DONE_EVENT)
            {
                // Current TX is done.
                NPITask_events &= ~NPITASK_TRANSPORT_TX_DONE_EVENT;

#if defined(NPI_SREQRSP)
                if (!Queue_empty(npiSyncTxQueue))
                {
                    // There are pending SYNC RSP messages waiting to be sent
                    // to the host. Set the appropriate flag and post to
                    // the semaphore.
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_SYNC_TX_READY_EVENT);
#else //!ICALL_EVENTS
                    NPITask_events |= NPITASK_SYNC_TX_READY_EVENT;
                    Semaphore_post(appSem);
#endif //ICALL_EVENTS
                }
                else
                {
#endif // NPI_SREQRSP
                    if (!Queue_empty(npiTxQueue))
                    {
                        // There are pending ASYNC messages waiting to be sent
                        // to the host. Set the appropriate flag and post to
                        // the semaphore.
#ifdef ICALL_EVENTS
                        Event_post(syncEvent, NPITASK_TX_READY_EVENT);
#else //!ICALL_EVENTS
                        NPITask_events |= NPITASK_TX_READY_EVENT;
                        Semaphore_post(appSem);
#endif //ICALL_EVENTS
                    }
#if defined(NPI_SREQRSP)
                }
#endif // NPI_SREQRSP
            }
        }
    }
}

// -----------------------------------------------------------------------------
//! \brief      NPI Task function called from within NPITask_Fxn
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITask_task(void)
{
    // Initialize application
    NPITask_inititializeTask();

    // No return from TestProfile2 process
    NPITask_process();
}


// -----------------------------------------------------------------------------
// Exported Functions


// -----------------------------------------------------------------------------
//! \brief      NPI task entry point.
//!
//! \return     void
// -----------------------------------------------------------------------------
Void NPITask_Fxn(UArg a0, UArg a1)
{
    NPITask_task();
}

// -----------------------------------------------------------------------------
//! \brief      Task creation function for NPI
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITask_createTask(uint32_t stackID)
{
    // Set stackID for future ICall Messaging
    stackServiceID = stackID;

    memset(&npiTaskStack, 0xBB, sizeof(npiTaskStack));

    // Configure and create the NPI task.
    Task_Params npiTaskParams;
    Task_Params_init(&npiTaskParams);
    npiTaskParams.stack = npiTaskStack;
    npiTaskParams.stackSize = NPITASK_STACK_SIZE;
    npiTaskParams.priority = NPITASK_PRIORITY;

    Task_construct(&npiTaskStruct, NPITask_Fxn, &npiTaskParams, NULL);
}

// -----------------------------------------------------------------------------
//! \brief      Register callback function to reroute incoming (from host)
//!             NPI messages.
//!
//! \param[in]  appRxCB   Callback function.
//! \param[in]  reRouteType Type of re-routing requested
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITask_registerIncomingRXEventAppCB(npiIncomingEventCBack_t appRxCB,
                                          NPI_IncomingNPIEventRerouteType reRouteType)
{
    incomingRXEventAppCBFunc = appRxCB;
    incomingRXReroute = reRouteType;
}

// -----------------------------------------------------------------------------
//! \brief      Register callback function to reroute outgoing (from stack)
//!             NPI messages.
//!
//! \param[in]  appTxCB   Callback function.
//! \param[in]  reRouteType Type of re-routing requested
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITask_registerIncomingTXEventAppCB(npiIncomingEventCBack_t appTxCB,
                                          NPI_IncomingNPIEventRerouteType reRouteType)
{
    incomingTXEventAppCBFunc = appTxCB;
    incomingTXReroute = reRouteType;
}

// -----------------------------------------------------------------------------
//! \brief      API for application task to send a message to the Host.
//!             NOTE: It's assumed all message traffic to the stack will use
//!             other (ICALL) APIs/Interfaces.
//!
//! \param[in]  pMsg    Pointer to "unframed" message buffer.
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITask_sendToHost(uint8_t *pMsg)
{
    ICall_CSState key;
    NPI_QueueRec *recPtr;

    NPIMSG_msg_t *pNPIMsg = NPIFrame_frameMsg(pMsg);

    if(!pNPIMsg)
    {
      return;
    }
    recPtr = ICall_malloc(sizeof(NPI_QueueRec));

    if(!recPtr)
    {
      ICall_free(pNPIMsg);
      return;
    }

    // Enter CS to prevent higher priority tasks
    // from also enqueuing msg at the same time
    key = ICall_enterCriticalSection();

    recPtr->npiMsg = pNPIMsg;

    switch (pNPIMsg->msgType)
    {
        // Enqueue to appropriate NPI Task Q and post corresponding event.
#if defined(NPI_SREQRSP)
        case NPIMSG_Type_SYNCRSP:
        {
            Queue_put(npiSyncTxQueue, &recPtr->_elem);
#ifdef ICALL_EVENTS
            Event_post(syncevent, NPITASK_SYNC_TX_READY_EVENT);
#else //!ICALL_EVENTS
            NPITask_events |= NPITASK_SYNC_TX_READY_EVENT;
            Semaphore_post(appSem);
#endif //ICALL_EVENT
            break;
        }
#endif // NPI_SREQRSP
        case NPIMSG_Type_ASYNC:
        {
            Queue_put(npiTxQueue, &recPtr->_elem);
#ifdef ICALL_EVENTS
            Event_post(syncEvent, NPITASK_TX_READY_EVENT);
#else //!ICALL_EVENTS
            NPITask_events |= NPITASK_TX_READY_EVENT;
            Semaphore_post(appSem);
#endif //ICALL_EVENTS
            break;
        }
        default:
        {
            //error
            break;
        }
    }

    ICall_leaveCriticalSection(key);
}

// -----------------------------------------------------------------------------
// Event Handlers

// -----------------------------------------------------------------------------
//! \brief      Process an ICall message from the Stack
//!
//! \param[in]  pMsg  pointer to an unframed message buffer
//! \param[in]  pMsg  Service ID for  message source
//! \param[in]  pMsg  Entity ID for message destination
//!
//! \return     void
// -----------------------------------------------------------------------------
static uint_least16_t NPITask_processICallMsgEvent(uint8_t *pMsg,
                                                   ICall_ServiceEnum src,
                                                   ICall_EntityID dest)
{
    if (dest == npiAppEntityID)
    {
        // Message received from the Stack.
        NPITask_processStackMsg(pMsg);
    }

    return(0);
}


// -----------------------------------------------------------------------------
// Utility functions

// -----------------------------------------------------------------------------
//! \brief      Forward the message buffer on to the Stack thread.
//!
//! \param[in]  appEntity    ICall Entity ID of the caller.
//! \param[in]  pMtBuf       Pointer to a NPIMSG_msg_t container.
//!
//! \return     void
// -----------------------------------------------------------------------------
static ICall_Errno NPITask_sendBufToStack(ICall_EntityID appEntity,
                                          NPIMSG_msg_t *pMsg)
{

    ICall_Errno errno = ICALL_ERRNO_SUCCESS;


    errno = ICall_sendServiceMsg(appEntity, stackServiceID,
                                 ICALL_MSG_FORMAT_KEEP, pMsg->pBuf);

    ICall_free(pMsg);

    return (errno);
}


// -----------------------------------------------------------------------------
// "Processor" functions

// -----------------------------------------------------------------------------
//! \brief      Process Response from Stack
//!
//! \param[in]  pMsg    Pointer to unframed message buffer
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_processStackMsg(uint8_t *pMsg)
{
    NPIMSG_msg_t *pNPIMsg;

    if(incomingTXEventAppCBFunc != NULL)
    {
        switch(incomingTXReroute)
        {
            case INTERCEPT:
            {
                // Pass the message along to the application and the leave
                // this function with an immediate return.
                // The message needs to be free by the callback.
                incomingTXEventAppCBFunc(pMsg);
                return;
            }
            case ECHO:
            {
                // Pass the message along to the application
                incomingTXEventAppCBFunc(pMsg);
                break;
            }
            case NONE:
            {
                break;
            }
        }
    }

    pNPIMsg = NPIFrame_frameMsg(pMsg);
    if(pNPIMsg != NULL)
    {
        NPI_QueueRec *recPtr;

        recPtr = ICall_malloc(sizeof(NPI_QueueRec));
        if(recPtr != NULL)
        {
            ICall_CSState key;

            // Enter CS to prevent higher priority tasks
            // from also enqueuing msg at the same time
            key = ICall_enterCriticalSection();

            recPtr->npiMsg = pNPIMsg;

            switch(pNPIMsg->msgType)
            {
                // Enqueue to appropriate NPI Task Q and post corresponding event.
#if defined(NPI_SREQRSP)
                case NPIMSG_Type_SYNCRSP:
                {
                    Queue_put(npiSyncTxQueue, &recPtr->_elem);
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_SYNC_TX_READY_EVENT);
#else //!ICALL_EVENTS
                    NPITask_events |= NPITASK_SYNC_TX_READY_EVENT;
                    Semaphore_post(appSem);
#endif //ICALL_EVENTS
                    break;
                }
#endif // NPI_SREQRSP
                case NPIMSG_Type_ASYNC:
                {
                    Queue_put(npiTxQueue, &recPtr->_elem);
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_TX_READY_EVENT);
#else //!ICALL_EVENTS
                    NPITask_events |= NPITASK_TX_READY_EVENT;
                    Semaphore_post(appSem);
#endif //ICALL_EVENTS
                    break;
                }
                default:
                {
                    /* Fail - unsupported message type */
                	ICall_free(recPtr);
                	ICall_free(pNPIMsg);
                    break;
                }
            }

            ICall_leaveCriticalSection(key);
        }
        else
        {
            /* Fail - couldn't get queue record */
        	ICall_free(pNPIMsg);
        }
    }
}

// -----------------------------------------------------------------------------
//! \brief      Dequeue next message in the ASYNC TX Queue and send to serial
//!             interface.
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_ProcessTXQ(void)
{
    ICall_CSState key;
    NPI_QueueRec *recPtr = NULL;

    // Processing of any TX Queue should only be done
    // in a critical section since any application
    // task can enqueue items freely
    key = ICall_enterCriticalSection();

    if (!Queue_empty(npiTxQueue))
    {
      recPtr = Queue_dequeue(npiTxQueue);

      if (recPtr != NULL)
      {
          lastQueuedTxMsg = recPtr->npiMsg->pBuf;

          NPITL_writeTL(recPtr->npiMsg->pBuf, recPtr->npiMsg->pBufSize);

          //free the Queue record
          ICall_free(recPtr->npiMsg);
          ICall_free(recPtr);
      }
    }

    ICall_leaveCriticalSection(key);
}

#if defined(NPI_SREQRSP)
// -----------------------------------------------------------------------------
//! \brief      Dequeue next message in the SYNC TX Queue and send to serial
//!             interface.
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_ProcessSyncTXQ(void)
{
    ICall_CSState key;
    NPI_QueueRec *recPtr = NULL;

    // Processing of any TX Queue should only be done
    // in a critical section since any application
    // task can enqueue items freely
    key = ICall_enterCriticalSection();

    if (!Queue_empty(npiSyncTxQueue))
    {
      recPtr = Queue_dequeue(npiSyncTxQueue);

      if (recPtr != NULL)
      {
          lastQueuedTxMsg = recPtr->npiMsg->pBuf;

          NPITL_writeTL(recPtr->npiMsg->pBuf, recPtr->npiMsg->pBufSize);

          // Decrement the outstanding Sync REQ/RSP flag.
          syncTransactionInProgress--;

          // Stop watchdog clock.
          Clock_stop(syncReqRspWatchDogClkHandle);

          if (syncTransactionInProgress < 0)
          {
              // not expected!
              syncTransactionInProgress = 0;
          }

          ICall_free(recPtr->npiMsg);
          ICall_free(recPtr);
      }
    }

    ICall_leaveCriticalSection(key);
}
#endif // NPI_SREQRSP

// -----------------------------------------------------------------------------
//! \brief      Dequeue next message in the RX Queue and process it.
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_processRXQ(void)
{
    NPI_QueueRec *recPtr = NULL;

    if (!Queue_empty(npiRxQueue))
    {
      recPtr = Queue_get(npiRxQueue);

      if (recPtr != NULL)
      {
          if (incomingRXEventAppCBFunc != NULL)
          {
              switch (incomingRXReroute)
              {
                  case ECHO:
                  {
                      // send to stack and a copy to the application
                      NPITask_sendBufToStack(npiAppEntityID, recPtr->npiMsg);
                      incomingRXEventAppCBFunc((uint8_t *)recPtr->npiMsg);
                      break;
                  }

                  case INTERCEPT:
                  {
                      // send a copy only to the application
                      // npiMsg need to be free in the callback
                      incomingRXEventAppCBFunc((uint8_t *)recPtr->npiMsg);
                      break;
                  }

                  case NONE:
                  {
                      NPITask_sendBufToStack(npiAppEntityID, recPtr->npiMsg);
                      break;
                  }
              }
          }
          else
          {
              // send to stack and a copy to the application
              NPITask_sendBufToStack(npiAppEntityID, recPtr->npiMsg);
          }

          //free the Queue record
          ICall_free(recPtr);

          // DON'T free the referenced npiMsg container.  This will be free'd in the
          // stack task.
      }
    }
}

#if defined(NPI_SREQRSP)
// -----------------------------------------------------------------------------
//! \brief      Dequeue next message in the RX Queue and process it.
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_processSyncRXQ(void)
{
    NPI_QueueRec *recPtr = NULL;

    if (syncTransactionInProgress == 0)
    {
        if (!Queue_empty(npiSyncRxQueue))
        {
          recPtr = Queue_get(npiSyncRxQueue);

          if (recPtr != NULL)
          {

              // Increment the outstanding Sync REQ/RSP flag.
              syncTransactionInProgress++;

              // Start the Sync REQ/RSP watchdog timer
              Clock_start(syncReqRspWatchDogClkHandle);

              if (incomingRXEventAppCBFunc != NULL)
              {
                  switch (incomingRXReroute)
                  {
                      case ECHO:
                      {
                          // send to stack and a copy to the application
                          NPITask_sendBufToStack(npiAppEntityID, recPtr->npiMsg);
                          incomingRXEventAppCBFunc(recPtr->npiMsg->pBuf);
                          break;
                      }

                      case INTERCEPT:
                      {
                          // send a copy only to the application
                          incomingRXEventAppCBFunc(recPtr->npiMsg->pBuf);
                          break;
                      }

                      case NONE:
                      {
                          NPITask_sendBufToStack(npiAppEntityID, recPtr->npiMsg);
                          break;
                      }
                  }
              }
              else
              {
                  // send to stack and a copy to the application
                  NPITask_sendBufToStack(npiAppEntityID, recPtr->npiMsg);
              }

              //free the Queue record
              ICall_free(recPtr);
              // DON'T free the referenced npiMsg container.  This will be free'd in the
              // stack task.
          }
        }
    }
}
#endif // NPI_SREQRSP

// -----------------------------------------------------------------------------
// Call Back Functions

// -----------------------------------------------------------------------------
//! \brief      Call back function for TX Done event from transport layer.
//!
//! \param[in]  size    Number of bytes transmitted.
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_transportTxDoneCallBack(int size)
{

    if(lastQueuedTxMsg)
    {
        //Deallocate most recent message being transmitted.
        ICall_freeMsg(lastQueuedTxMsg);

        lastQueuedTxMsg = NULL;
    }

    // Post the event to the NPI task thread.
#ifdef ICALL_EVENTS
    Event_post(syncEvent, NPITASK_TRANSPORT_TX_DONE_EVENT);
#else //!ICALL_EVENTS
    TX_DONE_ISR_EVENT_FLAG = NPITASK_TRANSPORT_TX_DONE_EVENT;
    Semaphore_post(appSem);
#endif //ICALL_EVENTS
}

// -----------------------------------------------------------------------------
//! \brief      Call back function to handle complete RX frame from Frame module.
//!
//! \param[in]  frameSize       Size of message frame.
//! \param[in]  pFrame          Pointer to message frame.
//! \param[in]  msgType         Message type, SYNC or ASYNC
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_incomingFrameCB(uint16_t frameSize, uint8_t *pFrame,
                                    NPIMSG_Type msgType)
{
    NPI_QueueRec *recPtr = ICall_malloc(sizeof(NPI_QueueRec));

    if (recPtr != NULL)
    {
        // Allocate NPIMSG_msg_t container
        NPIMSG_msg_t *npiMsgPtr = ICall_malloc(sizeof(NPIMSG_msg_t));

        if (npiMsgPtr != NULL)
        {
            npiMsgPtr->pBuf = pFrame;
            npiMsgPtr->pBufSize = frameSize;
            recPtr->npiMsg = npiMsgPtr;

            switch (msgType)
            {

                // Enqueue to appropriate NPI Task Q and post corresponding event.
                case NPIMSG_Type_ASYNC:
                {
                    recPtr->npiMsg->msgType = NPIMSG_Type_ASYNC;
                    Queue_put(npiRxQueue, &recPtr->_elem);
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_FRAME_RX_EVENT);
#else //!ICALL_EVENTS
                    NPITask_events |= NPITASK_FRAME_RX_EVENT;
                    Semaphore_post(appSem);
#endif //ICALL_EVENTS

                    break;
                }

#if defined(NPI_SREQRSP)
                case NPIMSG_Type_SYNCREQ:
                {
                    recPtr->npiMsg->msgType = NPIMSG_Type_SYNCREQ;
                    Queue_put(npiSyncRxQueue, &recPtr->_elem);
#ifdef ICALL_EVENTS
                    Event_post(syncEvent, NPITASK_SYNC_FRAME_RX_EVENT);
#else //!ICALL_EVENTS
                    NPITask_events |= NPITASK_SYNC_FRAME_RX_EVENT;
                    Semaphore_post(appSem);
#endif //ICALL_EVENTS

                    break;
                }
#endif // NPI_SREQRSP

                default:
                {
                    // undefined msgType
                    ICall_free(npiMsgPtr);
                    ICall_free(recPtr);
                    ICall_freeMsg(pFrame);

                    break;
                }
            }
        }
        else
        {
            // Malloc failed - release previous dynamic allocation
            ICall_free(recPtr);
        }
    }
}

// -----------------------------------------------------------------------------
//! \brief      RX Callback provided to Transport Layer for RX Event (ie.Bytes
//!             received).
//!
//! \param[in]  size    Number of bytes received.
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_transportRXCallBack(int size)
{
    // Check for overflow of RxBuf:
    // If the buffer has overflowed there is no way to safely recover. All
    // received bytes can be packet fragments so if a packet fragment is lost
    // the frame parser behavior becomes undefined. The only way to prevent
    // RxBuf overflow is to enable NPI_FLOW_CTRL.
    //
    // If NPI_FLOW_CTRL is not enabled then there is no way to for slave to
    // control the master transfer rate. With NPI_FLOW_CTRL the slave has SRDY
    // to use as a software flow control mechanism.
    // When using NPI_FLOW_CTRL make sure to increase NPI_TL_BUF_SIZE
    // to suit the NPI frame length that is expected to be received.
    if ( size < NPIRxBuf_GetRxBufAvail() )
    {
        NPIRxBuf_Read(size);
#ifdef ICALL_EVENTS
        Event_post(syncEvent, NPITASK_TRANSPORT_RX_EVENT);
#else //!ICALL_EVENTS
        TRANSPORT_RX_ISR_EVENT_FLAG = NPITASK_TRANSPORT_RX_EVENT;
        Semaphore_post(appSem);
#endif //ICALL_EVENTS
    }
    else
    {
        // Trap here for pending buffer overflow. If NPI_FLOW_CTRL is
        // enabled, increase size of RxBuf to handle larger frames from host.
        for(;;);
    }
}

// -----------------------------------------------------------------------------
//! \brief      RX Callback provided to Transport Layer for MRDY Event
//!
//! \param[in]  size    N/A
//!
//! \return     void
// -----------------------------------------------------------------------------
static void NPITask_MRDYEventCB(int size)
{
#ifdef ICALL_EVENTS
    Event_post(syncEvent, NPITASK_MRDY_EVENT);
#else //!ICALL_EVENTS
    MRDY_ISR_EVENT_FLAG = NPITASK_MRDY_EVENT;
    Semaphore_post(appSem);
#endif //ICALL_EVENTS
}


#if defined(NPI_SREQRSP)
// -----------------------------------------------------------------------------
//! \brief      Sync REQ/RSP Watchdog Timer CB
//!
//! \param[in]  a0      UArg
//!
//! \return     void
// -----------------------------------------------------------------------------
static void syncReqRspWatchDogTimeoutCB( UArg a0 )
{
    // Something has happened to the SYNC REQ we're waiting on.
    if (syncTransactionInProgress > 0)
    {
        // reduce the number of transactions outstanding
        syncTransactionInProgress--;

        // check if there are more pending SYNC REQ's
        if (!Queue_empty(npiSyncRxQueue))
        {
#ifdef ICALL_EVENTS
            Event_post(syncEvent, NPITASK_SYNC_FRAME_RX_EVENT);
#else //!ICALL_EVENTS
            NPITask_events |= NPITASK_SYNC_FRAME_RX_EVENT;
#endif //ICALL_EVENTS
        }

#ifndef ICALL_EVENTS
        // re-enter to Task event loop
        Semaphore_post(appSem);
#endif //ICALL_EVENTS
    }
    else
    {
        // not expected
    }
}
#endif // NPI_SREQRSP

