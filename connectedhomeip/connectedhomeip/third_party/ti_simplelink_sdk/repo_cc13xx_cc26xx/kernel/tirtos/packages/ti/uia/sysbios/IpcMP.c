/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */

/*
 *  ======== IpcMP.c ========
 */

/* XDC include files */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/System.h>

/* UIA include files */
#include <ti/uia/runtime/ServiceMgr.h>
#include <ti/uia/runtime/UIAPacket.h>
#include <ti/uia/runtime/Transport.h>
#include <ti/uia/runtime/MultiCoreTypes.h>

/* IPC include files */
#define ti_sdo_ipc_MessageQ__nolocalnames
#include <ti/sdo/ipc/MessageQ.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/MessageQ.h>

/* SYSBIOS include files */
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/BIOS.h>

/* internal */
#include "package/internal/IpcMP.xdc.h"

#include <string.h>  /* For memcpy */

/* Must be unsigned values passed in! */
#define EXPIRED(a, b) ((Int32)(b - a) >= 0)

/*
 *************************************************************************
 *                      Module functions
 *************************************************************************
 */

/*
 *  ======== IpcMP_Module_startup ========
 */
Int IpcMP_Module_startup(Int phase)
{
    Task_Params params;
    MessageQ_Params messageqParams;
    Char name[16];

    if (ti_sdo_ipc_MessageQ_Module_startupDone() == FALSE) {
        return (Startup_NOTDONE);
    }

    /* Setup the transfer agent's message queue */
    MessageQ_Params_init(&messageqParams);
    messageqParams.synchronizer = IpcMP_module->syncEvent01;
    if ((ServiceMgr_masterProcId == MultiProc_self()) ||
        (ServiceMgr_masterProcId == MultiProc_INVALIDID)) {
        System_sprintf(name, "%s", MultiCoreTypes_MASTERSTARTED);
        IpcMP_module->startedMQ = MessageQ_create(name, &messageqParams);
        System_printf("MessageQ_open MultiCoreTypes_MASTERSTARTED\n");
        System_sprintf(name, "%s", MultiCoreTypes_MASTERNAME);
        /* Default is false in the xs file. */
        IpcMP_module->master = TRUE;
        IpcMP_module->masterRunning = TRUE;

    }
    else {
        System_sprintf(name, "%s%02d", MultiCoreTypes_SLAVENAME, MultiProc_self());
    }
    IpcMP_module->routerMQ = MessageQ_create(name, &messageqParams);

    /* The slave will do an open and set masterMQ later */
    IpcMP_module->masterMQ = MessageQ_getQueueId(IpcMP_module->routerMQ);

    /* These queues have the standard MessageQ synchronizer */
    IpcMP_module->freeEventMQ   = MessageQ_create("freeEvents", NULL);
    IpcMP_module->freeMsgMQ     = MessageQ_create("freeMsgs", NULL);
    if (ServiceMgr_supportControl == TRUE) {
        IpcMP_module->incomingMsgMQ = MessageQ_create("incomingMsgs", NULL);

        /* The master needs to create the rxTask and release it */
        if (IpcMP_module->master == TRUE) {

            /* Create the rxTask if it was not statically created */
            if (IpcMP_createRxTask == TRUE) {
                Task_Params_init(&params);
                params.priority = ServiceMgr_rxTaskPriority;
                params.stackSize = ServiceMgr_rxTaskStackSize;
                Task_create(IpcMP_rxTaskFxn, &params, NULL);
            }
       }
    }

    if (ServiceMgr_transportFxns.initFxn != NULL) {
        ServiceMgr_transportFxns.initFxn();
    }

    return (Startup_DONE);
}

/*
 *  ======== IpcMP_clockFxn ========
 */
Void IpcMP_clockFxn(UArg arg0)
{
    /*
     *  Post the Event to allow the transfer agent to run
     *  and determine which services should send events.
     */
    Event_post(IpcMP_module->event, Event_Id_00);
}

/*
 *  ======== IpcMP_freePacket ========
 *  Function called by a service to "free" a packet.
 */
Void IpcMP_freePacket(UIAPacket_Hdr *packet)
{
    Int status;
    MessageQ_Msg msg;

    /* Adjust pointer to point to msg */
    msg = (MessageQ_Msg)((Char *)packet - sizeof(MessageQ_MsgHeader));

    /* Get the message from the free msg message queue */
    status = MessageQ_put(MessageQ_getReplyQueue(msg), msg);
    Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);
}

/*
 *  ======== IpcMP_getFreePacket ========
 *  Function called by a service to "allocate" a msg packet.
 */
UIAPacket_Hdr *IpcMP_getFreePacket(UIAPacket_HdrType type, UInt timeout)
{
    Int status;
    MessageQ_Msg msg;
    UIAPacket_Hdr *packet;
    MessageQ_Handle freeHandle;

    /* Determine which message queue to get the packet from */
    if (type == UIAPacket_HdrType_EventPkt) {
        freeHandle = (MessageQ_Handle)(IpcMP_module->freeEventMQ);
    }
    else if (type == UIAPacket_HdrType_Msg) {
        freeHandle = (MessageQ_Handle)(IpcMP_module->freeMsgMQ);
    }
    else {
        Assert_isTrue(FALSE, MultiCoreTypes_A_invalidHdrType);
    }

    /* Get the message from the specified free message queue */
    status = MessageQ_get(freeHandle, &msg, timeout);
    if (status != MessageQ_S_SUCCESS) {
        return (NULL);
    }

    /* Allows the router to return the msg to the free msg queue */
    MessageQ_setReplyQueue(freeHandle, msg);

    /* Adjust pointer to point to UIAPacket */
    packet = (UIAPacket_Hdr *)((Char *)msg + sizeof(MessageQ_MsgHeader));

    /*
     *  Make sure the HdrType is set. This allows this module to
     *  place the packet back on the correct free queue when done processing.
     */
    if (type == UIAPacket_HdrType_EventPkt) {
        UIAPacket_setHdrType(packet, UIAPacket_HdrType_EventPkt);
    }
    else {
        UIAPacket_setHdrType(packet, UIAPacket_HdrType_Msg);
    }

    return (packet);
}

/*
 *  ======== IpcMP_requestEnergy ========
 */
Void IpcMP_requestEnergy(Int id)
{
    UInt key;

    /* Must protect against the IpcMP transferAgent */
    key = Hwi_disable();

    /* Give the service energy to run. */
    IpcMP_module->reqEnergy[id] = TRUE;

    /* Leave the gate */
    Hwi_restore(key);

    /* Wake up the transfer agent */
    Event_post(IpcMP_module->event, Event_Id_02);
}

/*
 *  ======== IpcMP_rxTaskFxn ========
 *  Task that receives incoming messages from the host.
 *  These messages are then sent to the transfer agent.
 */
Void IpcMP_rxTaskFxn(UArg arg, UArg unused)
{
    Int status;
    MessageQ_Msg msg;
    UIAPacket_Hdr *packet;
    MessageQ_Handle freeMQ;

    /* Wait until ServiceMgr_start is called */
    Semaphore_pend(IpcMP_module->releaseTasksSem, BIOS_WAIT_FOREVER);

    /*
     *  This queue maintains the "free" buffers that will be
     *  filled in with messages from the instrumentation host.
     *  These are will be routed to the destination processors.
     *  The remote processors are responsible for sending this
     *  back to the free queue (via the getReplyQueue feature
     *  of MessageQ).
     */
    freeMQ = (MessageQ_Handle)(IpcMP_module->incomingMsgMQ);

    /* Prime all the incoming packets */
    IpcMP_prime(freeMQ, ServiceMgr_maxCtrlPacketSize,
                ServiceMgr_numIncomingCtrlPacketBufs);

    /* Make sure the transport is set to go */
    if (ServiceMgr_transportFxns.startFxn != NULL) {
        IpcMP_module->transportMsgHandle =
            ServiceMgr_transportFxns.startFxn(UIAPacket_HdrType_Msg);
    }

    /*
     *  Loop to receive msgs from the instrumentation host
     */
    while (TRUE) {

        /* Grab a free incomingMsg buffer */
        status = MessageQ_get(freeMQ, &msg, MessageQ_FOREVER);
        Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);

        /* Adjust to point to the packet */
        packet = (UIAPacket_Hdr *)((Char *)msg + sizeof(MessageQ_MsgHeader));

        /* Receive the packet. */
        status = ServiceMgr_transportFxns.recvFxn(IpcMP_module->transportMsgHandle,
                                             &packet, ServiceMgr_maxCtrlPacketSize);
        /* Put onto router's message queue */
        if ((status > 0) &&
            (UIAPacket_getHdrType(packet) == UIAPacket_HdrType_Msg)) {

            /* To allow receiver to know where to send it to */
            MessageQ_setReplyQueue(freeMQ, msg);

            /* To inform the remote processor of the action */
            MessageQ_setMsgId(msg, MultiCoreTypes_Action_FROMHOST);

            status = MessageQ_put(IpcMP_module->masterMQ, msg);
            Assert_isTrue((status == MessageQ_S_SUCCESS),
                          MultiCoreTypes_A_ipcFailed);
        }
        else {

            /* Put the msg back on the free list */
            status = MessageQ_put(MessageQ_getQueueId(freeMQ), msg);
            Assert_isTrue((status == MessageQ_S_SUCCESS),
                          MultiCoreTypes_A_ipcFailed);

            /* Reset the transport with a stop/start */
            if (ServiceMgr_transportFxns.stopFxn != NULL) {
                ServiceMgr_transportFxns.stopFxn(IpcMP_module->transportMsgHandle);
            }

            if (ServiceMgr_transportFxns.startFxn != NULL) {
                IpcMP_module->transportMsgHandle =
                    ServiceMgr_transportFxns.startFxn(UIAPacket_HdrType_Msg);
            }
        }
    }
}

/*
 *  ======== IpcMP_sendPacket ========
 *  Function called by a service to send a packet.
 */
Bool IpcMP_sendPacket(UIAPacket_Hdr *packet)
{
    Int status;
    MessageQ_Msg msg;

    /* Set the src fields */
    UIAPacket_setSenderAdrs(packet, (Bits16)MultiProc_self());

    /* Adjust to point to the message header */
    msg = (MessageQ_Msg)((Char *)packet - sizeof(MessageQ_MsgHeader));

    if (IpcMP_module->masterRunning == FALSE) {
        status = MessageQ_put(MessageQ_getReplyQueue(msg), msg);
        return (TRUE);
    }

    /*
     *  If on the master processor and the call is being made
     *  in the context of the transferAgent, just call the IpcMP directly.
     */
    if ((IpcMP_module->master == TRUE) &&
        (IpcMP_module->transferAgentHandle == Task_self())) {

        if (UIAPacket_getHdrType(packet) == UIAPacket_HdrType_Msg) {
            status = ServiceMgr_transportFxns.sendFxn(
                IpcMP_module->transportMsgHandle, &packet);
            if (status >= 0) {
                IpcMP_module->numMSGPacketsSent++;
            }
        }
        else {
            status = ServiceMgr_transportFxns.sendFxn(
                IpcMP_module->transportEventHandle, &packet);
            if (status >= 0) {
                IpcMP_module->numEventPacketsSent++;
            }
        }

        /* Give the message back */
        status = MessageQ_put(MessageQ_getReplyQueue(msg), msg);
        Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);
    }
    else {

        /* Tell the router what type of message */
        MessageQ_setMsgId(msg, MultiCoreTypes_Action_TOHOST);

        /* Send the message to the router */
        status = MessageQ_put(IpcMP_module->masterMQ, msg);
        if (status != MessageQ_S_SUCCESS) {
            return (FALSE);
        }
    }

    return (TRUE);
}

/*
 *  ======== IpcMP_setPeriod ========
 */
Void IpcMP_setPeriod(Int id, UInt32 periodInMs)
{
    UInt key;
    UInt adjPeriod;

    /*
     *  Adjust as need:
     *   - 0 stays 0 (not going to send events)
     *   - less than this module's period, set to this module's period
     *   - make sure it is a multiple of this module's period
     */
    if (periodInMs == 0) {
        adjPeriod = periodInMs;
    }
    else if (periodInMs < (UInt32)ServiceMgr_periodInMs) {
        adjPeriod = ServiceMgr_periodInMs;
    }
    else {
        adjPeriod = (periodInMs / ServiceMgr_periodInMs) * ServiceMgr_periodInMs;
    }

    /* Must protect against the IpcMP Thread */
    key = Hwi_disable();

    /* Set the new period and scheduled time */
    IpcMP_module->period[id] = adjPeriod * 1000 / Clock_tickPeriod;
    IpcMP_module->scheduled[id] = Clock_getTicks() + IpcMP_module->period[id];

    /* Leave the gate */
    Hwi_restore(key);
}

/*
 *  ======== IpcMP_start ========
 */
Int IpcMP_start(UArg arg, UInt16 value)
{
    /*
     *  The master and supportControl is TRUE, post the semaphore
     *  twice to release rxTask and Transfer Tasks. Otherwise,
     *  just once to release the Transfer Task.
     */
    Semaphore_post(IpcMP_module->releaseTasksSem);

    if ((IpcMP_module->master == TRUE) &&
        (ServiceMgr_supportControl == TRUE)) {

        Semaphore_post(IpcMP_module->releaseTasksSem);
    }
    return (0);
}

/*
 *  ======== IpcMP_startOrig ========
 */
Int IpcMP_startOrig(Ptr *ptr, UInt16 value)
{
    return (IpcMP_start(0, value));
}

/*
 *  ======== IpcMP_transferAgentFxn ========
 */
Void IpcMP_transferAgentFxn(UArg arg, UArg unused)
{
    Bits32 mask;
    MessageQ_Msg msg;

    /* Wait until Service_start is called */
    Semaphore_pend(IpcMP_module->releaseTasksSem, BIOS_WAIT_FOREVER);

    /* Prime the different outgoing event and msg buffers into "free" queues */
    IpcMP_prime(IpcMP_module->freeMsgMQ, ServiceMgr_maxCtrlPacketSize,
              ServiceMgr_numOutgoingCtrlPacketBufs);
    IpcMP_prime(IpcMP_module->freeEventMQ, ServiceMgr_maxEventPacketSize,
              ServiceMgr_numEventPacketBufs);

    /*
     * If the master, call the transport start function for events.
     * If the slave, open the master message queue and send a register msg.
     */
    if (IpcMP_module->master == TRUE) {
        /* Prime some more for broadcast */
        IpcMP_prime(IpcMP_module->freeMsgMQ, ServiceMgr_maxCtrlPacketSize,
              4);
        if (ServiceMgr_transportFxns.startFxn != NULL) {
            IpcMP_module->transportEventHandle =
                ServiceMgr_transportFxns.startFxn(UIAPacket_HdrType_EventPkt);
        }
    }
    else {
        IpcMP_registerWithMaster();
    }

    /* Run in a loop. */
    while (TRUE) {
        /* Block on the clock, incoming message or energy request */
        mask = Event_pend(IpcMP_module->event, Event_Id_NONE,
                          Event_Id_00 | Event_Id_01 | Event_Id_02,
                          BIOS_WAIT_FOREVER);

        /* Service requested energy */
        if (mask & Event_Id_02) {
            IpcMP_giveEnergy();
        }

        /* A Command has been sent down, handle it */
        if (mask & Event_Id_01) {
            /* Make sure to process all incoming messages */
            while (MessageQ_get(IpcMP_module->routerMQ, &msg, 0) == MessageQ_S_SUCCESS) {
                IpcMP_handleMsg(msg);
            }
        }

        /* If the Clock expired, gather data */
        if (mask & Event_Id_00) {
            IpcMP_requestEvents();
        }
    }
}

/*
 *************************************************************************
 *                       Internal functions
 *************************************************************************
 */

/*
 *  ======== IpcMP_giveEnergy ========
 */
Void IpcMP_giveEnergy()
{
    Int id;
    UInt key;

    for (id = 0; id < ServiceMgr_getNumServices(); id++) {

        /* Must protect against the IpcMP transferAgent */
        key = Hwi_disable();

        if (IpcMP_module->reqEnergy[id] == TRUE) {

            IpcMP_module->reqEnergy[id] = FALSE;

            /* Leave the gate */
            Hwi_restore(key);

            /* Call the service's callback */
            ServiceMgr_processCallback(id, ServiceMgr_Reason_REQUESTENERGY, NULL);
        }
        else {
            /* Leave the gate */
            Hwi_restore(key);
        }
    }
}

/*
 *  ======== IpcMP_handleMsg ========
 */
Void IpcMP_handleMsg(Ptr buf)
{
    Int i;
    UInt key;
    UInt serviceId;
    UInt16 dstProcId;
    UIAPacket_Hdr *packet;
    UIAPacket_Hdr *remotePacket;
    UInt32 *nackCode;
    Int status = MessageQ_E_FAIL;
    MessageQ_Msg msg = (MessageQ_Msg)buf;
    MessageQ_Msg remoteMsg;
    MessageQ_QueueId returnMQ;


    switch (MessageQ_getMsgId(msg)) {
        case MultiCoreTypes_Action_TOHOST: /* Send the packet to the host */
            /* Point the the packet and send */
            packet = (UIAPacket_Hdr *)((Char *)msg + sizeof(MessageQ_MsgHeader));

            if (UIAPacket_getHdrType(packet) == UIAPacket_HdrType_Msg) {
                ServiceMgr_transportFxns.sendFxn(IpcMP_module->transportMsgHandle, &packet);
            }
            else {
                ServiceMgr_transportFxns.sendFxn(IpcMP_module->transportEventHandle, &packet);
            }

            /* Give the message back */
            status = MessageQ_put(MessageQ_getReplyQueue(msg), msg);
            Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);
            break;

        case MultiCoreTypes_Action_FROMHOST: /* Packet from the host */

            /* Determine whether to process or forward to dst processor */
            packet = (UIAPacket_Hdr *)((Char *)msg + sizeof(MessageQ_MsgHeader));
            dstProcId = UIAPacket_getDestAdrs(packet);

            /* Check to see if this is a broadcast */
            if (dstProcId == UIAPacket_BROADCAST) {

                /* If a master, send to the registered slaves */
                if (IpcMP_module->master == TRUE) {
                    for (i = 0; i < MultiProc_getNumProcessors(); i++) {
                        if (IpcMP_module->replyMQ[i] !=
                            MessageQ_INVALIDMESSAGEQ) {

                            /* Get a free message and send to the remote core */
                            remotePacket = IpcMP_getFreePacket(UIAPacket_HdrType_Msg,
                                                ServiceMgr_WAIT_FOREVER);

                            /* todo Handle error?? */
                            memcpy(remotePacket, packet, UIAPacket_getMsgLength(packet));
                            remoteMsg = (MessageQ_Msg)((Char *)remotePacket - sizeof(MessageQ_MsgHeader));
                            MessageQ_setMsgId(remoteMsg, MultiCoreTypes_Action_FROMHOST);
                            status = MessageQ_put(IpcMP_module->replyMQ[i], remoteMsg);
                            Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);
                        }
                    }
                }
                dstProcId = MultiProc_self();
            }

            if (dstProcId == MultiProc_self()) {

                serviceId = UIAPacket_getServiceId(packet);

                /* Process the message */
                ServiceMgr_processCallback(serviceId, ServiceMgr_Reason_INCOMINGMSG,
                                        packet);

                /* Send it back to the rx Task */
                status = MessageQ_put(MessageQ_getReplyQueue(msg), msg);
                Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);
            }
            else {
                /* Send to the remote processor */
                if (IpcMP_module->replyMQ[dstProcId] != MessageQ_INVALIDMESSAGEQ) {
                    status = MessageQ_put(IpcMP_module->replyMQ[dstProcId], msg);
                    Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);
                }
                else {
                    /* send NACK back. Re-use existing message. */
                    UIAPacket_setMsgType(packet,
                        UIAPacket_MsgType_NACK_WITH_ERROR_CODE);
                    UIAPacket_setMsgLength(packet, sizeof(UIAPacket_Hdr) +
                        sizeof(UIAPacket_NACKErrorCode));
                    UIAPacket_setDestAdrs(packet, UIAPacket_HOST);
                    UIAPacket_setSenderAdrs(packet, dstProcId);
                    nackCode = (UInt32 *)((Char *)packet + sizeof(UIAPacket_Hdr));
                    *nackCode = UIAPacket_swizzle(UIAPacket_NACKErrorCode_BAD_ENDPOINT_ADDRESS);

                    ServiceMgr_transportFxns.sendFxn(IpcMP_module->transportMsgHandle, &packet);
                }
            }
            break;

        case MultiCoreTypes_Action_REGISTER:

            /* Start events and msgs from going to the master */
            IpcMP_module->replyMQ[msg->srcProc] = ((MultiCoreTypes_RegisterMsg *)msg)->remoteMQ;
            System_printf("Setting IpcMP_module->replyMQ 0x%x, 0x%x\n", msg->srcProc, ((MultiCoreTypes_RegisterMsg *)msg)->remoteMQ);

            /* Send it back to the free queue */
            returnMQ = MessageQ_getReplyQueue(msg);
            status = MessageQ_put(returnMQ, msg);
            Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);
            break;

        case MultiCoreTypes_Action_STOP:

            /* Stop events and msgs from going to the master */
            key = Hwi_disable();
            IpcMP_module->masterRunning = FALSE;
            Hwi_restore(key);

            /* Send it back to the sender */
            MessageQ_setMsgId(msg, MultiCoreTypes_Action_STOPACK);
            returnMQ = IpcMP_module->masterMQ;
            status = MessageQ_put(returnMQ, msg);
            Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);
            IpcMP_registerWithMaster();
            break;

        default:
            /* Send it back to the free queue */
            returnMQ = MessageQ_getReplyQueue(msg);
            status = MessageQ_put(returnMQ, msg);
            Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);
            break;
    }

}

/*
 *  ======== IpcMP_prime ========
 *  Prime the the MessageQ.
 */
Void IpcMP_prime(Ptr handle, Int size, Int count)
{
    Int i;
    Int allocSize = size + sizeof(MessageQ_MsgHeader);
    MessageQ_Msg msg;
    IHeap_Handle heap;
    Int status;

    /* Get the SharedRegion's heap */
    heap = (IHeap_Handle)SharedRegion_getHeap(IpcMP_sharedRegionId);
    Assert_isTrue((heap != NULL), MultiCoreTypes_A_ipcFailed);

    for (i = 0; i < count; i++) {

        /* Allocate the shared memory from the shared memory heap */
        msg = Memory_alloc(heap, allocSize, 0, NULL);

        /* No calls to MessageQ_alloc/free so we can use the staticMsgInit. */
        MessageQ_staticMsgInit(msg, allocSize);

        /* Place onto the "free" message queue */
        status = MessageQ_put(MessageQ_getQueueId(handle), msg);
        Assert_isTrue((status == MessageQ_S_SUCCESS),
                       MultiCoreTypes_A_ipcFailed);
    }
}

/*
 *  ======== IpcMP_registerWithMaster ========
 */
Void IpcMP_registerWithMaster(Void)
{
    Int status = MessageQ_E_FAIL;
    MultiCoreTypes_RegisterMsg *regMsg;
    MessageQ_QueueId masterStarted;

    /* Make sure the master has been created */
    while (status != MessageQ_S_SUCCESS) {
        status = MessageQ_open(MultiCoreTypes_MASTERSTARTED, &masterStarted);
        if (status != MessageQ_S_SUCCESS) {
            Task_sleep(100);
        }
    }
    System_printf("MessageQ_open MultiCoreTypes_MASTERSTARTED 0x%x\n", masterStarted);

    status = MessageQ_E_FAIL;
    while (status != MessageQ_S_SUCCESS) {
        status = MessageQ_open(MultiCoreTypes_MASTERNAME,
                               &(IpcMP_module->masterMQ));
        if (status != MessageQ_S_SUCCESS) {
            Task_sleep(100);
        }
    }

    /* Not needed anymore */
    MessageQ_close(&masterStarted);

    /*
     *  Send a register message to the master. The only thread that gets
     *  from this queue is this one. So no single reader issues.
     */
    status = MessageQ_get(IpcMP_module->freeMsgMQ, (MessageQ_Msg *)&regMsg,
                          MessageQ_FOREVER);
    Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);

    /* Setup and send the register request to the master */
    MessageQ_setMsgId(regMsg, MultiCoreTypes_Action_REGISTER);
    MessageQ_setReplyQueue(IpcMP_module->freeMsgMQ, (MessageQ_Msg)regMsg);
    regMsg->remoteMQ = MessageQ_getQueueId(IpcMP_module->routerMQ);
    System_printf("regMsg->remoteMQ = 0x%x IpcMP_module->masterMQ = 0x%x\n", regMsg->remoteMQ, IpcMP_module->masterMQ);

    status = MessageQ_put(IpcMP_module->masterMQ, (MessageQ_Msg)regMsg);

    Assert_isTrue((status == MessageQ_S_SUCCESS), MultiCoreTypes_A_ipcFailed);

    IpcMP_module->masterRunning = TRUE;
}

/*
 *  ======== IpcMP_requestEvents ========
 */
Void IpcMP_requestEvents(Void)
{
    Int id;
    UInt key;
    UInt32 currentTime;

    /* Get the current time to determine who should be called */
    currentTime = Clock_getTicks();

    /* Query each service */
    for (id = 0; id < ServiceMgr_getNumServices(); id++) {

        /* To protect against a change via IpcMP_setPeriod */
        key = Hwi_disable();

        /* if the service wants to be called and the time has arrived */
        if ((IpcMP_module->period[id] != 0) &&
            EXPIRED(IpcMP_module->scheduled[id], currentTime)) {

            /* Set the new scheduled time */
            IpcMP_module->scheduled[id] = currentTime +
                    IpcMP_module->period[id];

            if (IpcMP_module->masterRunning == TRUE) {
                Hwi_restore(key);

                /* Call the service's function to let it send events */
                ServiceMgr_processCallback(id, ServiceMgr_Reason_PERIODEXPIRED,
                        NULL);
            }
            else {
                Hwi_restore(key);
            }
        }
    }
}
