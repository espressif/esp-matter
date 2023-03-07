/*
 * Copyright (c) 2013-2019, Texas Instruments Incorporated
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
 *  ======== Adaptor.c ========
 */

/* XDC include files */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Startup.h>

/* UIA include files */
#include <ti/uia/runtime/ServiceMgr.h>
#include <ti/uia/runtime/UIAPacket.h>
#include <ti/uia/runtime/Transport.h>

/* SYSBIOS include files */
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/BIOS.h>

/* internal */
#include "package/internal/Adaptor.xdc.h"

/* Must be unsigned values passed in! */
#define EXPIRED(a, b) ((Int32)(b - a) >= 0)

/*
 *************************************************************************
 *                      Module functions
 *************************************************************************
 */

/*
 *  ======== Adaptor_Module_startup ========
 */
Int Adaptor_Module_startup(Int phase)
{
    Int i;
    Queue_Elem *elem;

    if (Queue_Module_startupDone() == FALSE) {
        return (Startup_NOTDONE);
    }

    /*
     *  If there are control messages, put the packets onto the free msg queue
     */
    if (ServiceMgr_supportControl == TRUE) {
        elem = (Queue_Elem *)Adaptor_msgBuf;
        for (i = 0; i < ServiceMgr_numIncomingCtrlPacketBufs +
             ServiceMgr_numOutgoingCtrlPacketBufs; i++) {
            Queue_put(Adaptor_module->freeMsgQ, elem);
            elem = (Queue_Elem *)((Char *)elem + ServiceMgr_maxCtrlPacketSize +
                                  sizeof(Queue_Elem));
        }
    }

    /* Put the packets onto the free event queue */
    elem = (Queue_Elem *)Adaptor_eventBuf;
    for (i = 0; i < ServiceMgr_numEventPacketBufs; i++) {
        Queue_put(Adaptor_module->freeEventQ, elem);
        elem = (Queue_Elem *)((Char *)elem + ServiceMgr_maxEventPacketSize +
                              sizeof(Queue_Elem));
    }

    /* Call the transport init function */
    if (ServiceMgr_transportFxns.initFxn != NULL) {
        ServiceMgr_transportFxns.initFxn();
    }

    return (Startup_DONE);
}

/*
 *  ======== Adaptor_clockFxn ========
 */
Void Adaptor_clockFxn(UArg arg0)
{
    /*
     *  Post the Event to allow the transfer agent to run
     *  and determine which services should send events.
     */
    Event_post(Adaptor_module->event, Event_Id_00);
}

/*
 *  ======== Adaptor_freePacket ========
 *  Function called by a service to "free" a packet.
 */
Void Adaptor_freePacket(UIAPacket_Hdr *packet)
{
    Queue_Elem *elem;
    UIAPacket_HdrType type = UIAPacket_getHdrType(packet);

    elem = (Queue_Elem *)((Char *)packet - sizeof(Queue_Elem));

    /* Determine the type of header to place on the correct queue */
    if (type == UIAPacket_HdrType_EventPkt) {
        Queue_put(Adaptor_module->freeEventQ, elem);
        Semaphore_post(Adaptor_module->freeEventSem);
    }
    else {
        Queue_put(Adaptor_module->freeMsgQ, elem);
        Semaphore_post(Adaptor_module->freeMsgSem);
    }
}

/*
 *  ======== Adaptor_getFreePacket ========
 *  Function called by a service to "allocate" a msg packet.
 */
UIAPacket_Hdr *Adaptor_getFreePacket(UIAPacket_HdrType type, UInt timeout)
{
    Adaptor_Entry *entry;

    /* Get the free packet off the correct queue */
    if (type == UIAPacket_HdrType_EventPkt) {
        Semaphore_pend(Adaptor_module->freeEventSem, timeout);
        entry = Queue_get(Adaptor_module->freeEventQ);
        if ((Queue_Elem *)entry == (Queue_Elem *)Adaptor_module->freeEventQ) {
            return (NULL);
        }

    }
    else {
        Semaphore_pend(Adaptor_module->freeMsgSem, timeout);
        entry = Queue_get(Adaptor_module->freeMsgQ);
        if ((Queue_Elem *)entry == (Queue_Elem *)Adaptor_module->freeMsgQ) {
            return (NULL);
        }
    }

    /*
     *  Make sure the HdrType is set. This allows this module to
     *  place the packet back on the correct free queue when done processing.
     */
    UIAPacket_setHdrType(&(entry->packet), type);

    return (&(entry->packet));
}

/*
 *  ======== Adaptor_requestEnergy ========
 */
Void Adaptor_requestEnergy(Int id)
{
    UInt key;

    /* Must protect against the Adaptor transferAgent */
    key = Hwi_disable();

    /* Give the service energy to run. */
    Adaptor_module->reqEnergy[id] = TRUE;

    /* Leave the gate */
    Hwi_restore(key);

    /* Wake up the transfer agent */
    Event_post(Adaptor_module->event, Event_Id_02);
}

/*
 *  ======== Adaptor_rxTaskFxn ========
 *  Task that receives incoming messages from the host.
 *  These messages are then sent to the transfer agent.
 */
Void Adaptor_rxTaskFxn(UArg arg, UArg unused)
{
    Int status;
    Adaptor_Entry *entry;
    UIAPacket_Hdr *packet;

    /* Make sure the transport is set to go */
    if (ServiceMgr_transportFxns.startFxn != NULL) {
        Adaptor_module->transportMsgHandle =
            ServiceMgr_transportFxns.startFxn(UIAPacket_HdrType_Msg);
    }

    /*
     *  Loop to receive msgs from the instrumentation host
     */
    while (TRUE) {

        /* Grab a free incomingMsg buffer */
        packet = Adaptor_getFreePacket(UIAPacket_HdrType_Msg,
                                       BIOS_WAIT_FOREVER);

        /* Receive the packet. */
        status = ServiceMgr_transportFxns.recvFxn(
                     Adaptor_module->transportMsgHandle, &packet,
                     ServiceMgr_maxCtrlPacketSize);

        /* Put onto router's message queue */
        if ((status > 0) &&
            (UIAPacket_getHdrType(packet) == UIAPacket_HdrType_Msg)) {

            /* The Queue elem is just above the packet */
            entry = (Adaptor_Entry *)((Char *)packet - sizeof(Queue_Elem));

            Queue_put(Adaptor_module->incomingQ, (Queue_Elem *)entry);
            Event_post(Adaptor_module->event, Event_Id_01);
        }
        else {
            /* Return the packet */
            Adaptor_freePacket(packet);

            /* Reset the transport with a stop/start */
            if (ServiceMgr_transportFxns.stopFxn != NULL) {
                ServiceMgr_transportFxns.stopFxn(
                    Adaptor_module->transportMsgHandle);
            }

            if (ServiceMgr_transportFxns.startFxn != NULL) {
                Adaptor_module->transportMsgHandle =
                    ServiceMgr_transportFxns.startFxn(UIAPacket_HdrType_Msg);
            }
        }
    }
}

/*
 *  ======== Adaptor_sendPacket ========
 *  Function called by a service to send a packet.
 */
Bool Adaptor_sendPacket(UIAPacket_Hdr *packet)
{
    Bool status;
    Adaptor_Entry *entry;

    /* Set the src fields */
    UIAPacket_setSenderAdrs(packet, 0);

    /*
     *  If the call is being made in the context of the transferAgent,
     *  just call the Adaptor directly.
     */
    if (Adaptor_module->transferAgentHandle == Task_self()) {
        status = Adaptor_sendToHost(packet);
    }
    else {

        /* Not in the transfer agent's context. Put it on the outgoing queue */
        entry = (Adaptor_Entry *)((Char *)packet - sizeof(Queue_Elem));

        Queue_put(Adaptor_module->outgoingQ, (Queue_Elem *)entry);
        Event_post(Adaptor_module->event, Event_Id_03);
        status = TRUE;
    }

    return (status);
}

/*
 *  ======== Adaptor_setPeriod ========
 */
Void Adaptor_setPeriod(Int id, UInt32 periodInMs)
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
        adjPeriod = (periodInMs / ServiceMgr_periodInMs) *
                    ServiceMgr_periodInMs;
    }

    /* Must protect against the Adaptor Thread */
    key = Hwi_disable();

    /* Set the new period and scheduled time */
    Adaptor_module->period[id] = adjPeriod * 1000 / Clock_tickPeriod;
    Adaptor_module->scheduled[id] = Clock_getTicks() +
            Adaptor_module->period[id];

    /* Leave the gate */
    Hwi_restore(key);
}

/*
 *  ======== Adaptor_transferAgentTaskFxn ========
 */
Void Adaptor_transferAgentTaskFxn(UArg arg, UArg unused)
{
    Adaptor_Entry *entry;
    Bool status;
    Bits32 mask;

    if (ServiceMgr_transportFxns.startFxn != NULL) {
        Adaptor_module->transportEventHandle =
                ServiceMgr_transportFxns.startFxn(UIAPacket_HdrType_EventPkt);
    }

    /* Run in a loop. */
    while (TRUE) {
        /*  Block on
         *  - the clock:        request events to be sent.
         *  - incoming message: call Adaptor_sendToService
         *  - outgoing message: call Adaptor_sendToHost
         *  - energy request:   call Adaptor_giveEnergy
         */
        mask = Event_pend(Adaptor_module->event, Event_Id_NONE,
                Event_Id_00 | Event_Id_01 | Event_Id_02 | Event_Id_03,
                BIOS_WAIT_FOREVER);

        /* If the Clock expired, gather data */
        if (mask & Event_Id_00) {
            Adaptor_runScheduledServices();
        }

        /* A msg has been sent from the host, handle all of them */
        if (mask & Event_Id_01) {
            entry = (Adaptor_Entry *)Queue_get(Adaptor_module->incomingQ);
            while ((Queue_Elem *)entry !=
                   (Queue_Elem *)Adaptor_module->incomingQ) {
                Adaptor_sendToService(entry);

                /* Get next one */
                entry = (Adaptor_Entry *)Queue_get(Adaptor_module->incomingQ);
            }
        }

        /* Service requested energy */
        if (mask & Event_Id_02) {
            Adaptor_giveEnergy();
        }

        /* A msg need to be sent to the host, handle all of them */
        if (mask & Event_Id_03) {
            entry = (Adaptor_Entry *)Queue_get(Adaptor_module->outgoingQ);
            while ((Queue_Elem *)entry !=
                   (Queue_Elem *)Adaptor_module->outgoingQ) {

                /* Send message to host */
                status = Adaptor_sendToHost((UIAPacket_Hdr *)&(entry->packet));
                if (status == FALSE) {
                    Adaptor_freePacket((UIAPacket_Hdr *)&(entry->packet));
                }

                /* Get next one */
                entry = (Adaptor_Entry *)Queue_get(Adaptor_module->outgoingQ);
            }
        }
    }
}

/*
 *************************************************************************
 *                       Internal functions
 *************************************************************************
 */

/*
 *  ======== Adaptor_giveEnergy ========
 */
Void Adaptor_giveEnergy()
{
    Int id;
    UInt key;

    for (id = 0; id < ServiceMgr_getNumServices(); id++) {

        /* Must protect against the Adaptor transferAgent */
        key = Hwi_disable();

        if (Adaptor_module->reqEnergy[id] == TRUE) {

            Adaptor_module->reqEnergy[id] = FALSE;

            /* Leave the gate */
            Hwi_restore(key);

            /* Call the service's callback */
            ServiceMgr_processCallback(id, ServiceMgr_Reason_REQUESTENERGY,
                                       NULL);
        }
        else {
            /* Leave the gate */
            Hwi_restore(key);
        }
    }
}

/*
 *  ======== Adaptor_sendToHost ========
 *  Do not free the packet in case of failure since
 *  the service might want to resend or do something with the data.
 */
Bool Adaptor_sendToHost(UIAPacket_Hdr *packet)
{
    Bool status;

    if (UIAPacket_getHdrType(packet) == UIAPacket_HdrType_Msg) {
        status = ServiceMgr_transportFxns.sendFxn(
                     Adaptor_module->transportMsgHandle, &packet);
        if (status == TRUE) {
            Adaptor_module->numMsgPacketsSent++;

            /* Free the packet */
            Adaptor_freePacket(packet);
        }
        else {
            Adaptor_module->numMsgPacketsFailed++;
        }
    }
    else {
        status = ServiceMgr_transportFxns.sendFxn(
                     Adaptor_module->transportEventHandle, &packet);
        if (status == TRUE) {
            Adaptor_module->numEventPacketsSent++;

            /* Free the packet */
            Adaptor_freePacket(packet);
        }
        else {
            Adaptor_module->numEventPacketsFailed++;
        }
    }
    return (status);
}

/*
 *  ======== Adaptor_sendToService ========
 */
Void Adaptor_sendToService(Adaptor_Entry *entry)
{
    UInt serviceId;
    UIAPacket_Hdr *packet = (UIAPacket_Hdr *)&(entry->packet);

    /* Get the service id from the packet and send the msg to the service */
    serviceId = UIAPacket_getServiceId(packet);

    ServiceMgr_processCallback(serviceId, ServiceMgr_Reason_INCOMINGMSG,
                               packet);
    /* Free the packet */
    Adaptor_freePacket(packet);
}

/*
 *  ======== Adaptor_runScheduledServices ========
 */
Void Adaptor_runScheduledServices(Void)
{
    Int id;
    UInt key;
    UInt32 currentTime;

    /* Get the current time to determine who should be called */
    currentTime = Clock_getTicks();

    /* Query each service */
    for (id = 0; id < ServiceMgr_getNumServices(); id++) {

        /* To protect against a change via Adaptor_setPeriod */
        key = Hwi_disable();

        /*
         *  The service must have a non-zero period to be processed.
         *  If scheduled time has past, call the process function.
         *  Also handle the case where the clock wraps.
         */
        if ((Adaptor_module->period[id] != 0) &&
             EXPIRED(Adaptor_module->scheduled[id], currentTime)) {

            /* Set the new scheduled time */
            Adaptor_module->scheduled[id] = currentTime +
                    Adaptor_module->period[id];

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
