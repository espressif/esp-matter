/*
 * Copyright (c) 2012-2016, Texas Instruments Incorporated
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
 *  ======== Adaptor.xs ========
 */

var Adaptor;

var Gate = null;
var Log = null;
var Memory = null;
var Hwi = null;
var Clock = null;
var Task = null;
var Queue = null;
var Event = null;
var Semaphore = null;
var SyncEvent = null;
var Bios = null;
var ServiceMgr = null;
var UIAPacket = null;
var Settings = null;

var numServices = 0;

/*
 *  ======== module$use ========
 */
function module$use()
{
    Adaptor = this;

    /* Bring in all modules used directly by the Agent. */
    Gate         = xdc.useModule('xdc.runtime.Gate');
    Log          = xdc.useModule('xdc.runtime.Log');
    Memory       = xdc.useModule('xdc.runtime.Memory');
    Hwi          = xdc.useModule('ti.sysbios.hal.Hwi');
    Clock        = xdc.useModule('ti.sysbios.knl.Clock');
    Task         = xdc.useModule('ti.sysbios.knl.Task');
    Queue        = xdc.useModule('ti.sysbios.knl.Queue');
    Event        = xdc.useModule('ti.sysbios.knl.Event');
    Semaphore    = xdc.useModule('ti.sysbios.knl.Semaphore');
    SyncEvent    = xdc.useModule('ti.sysbios.syncs.SyncEvent');
    Bios         = xdc.useModule('ti.sysbios.BIOS');
    ServiceMgr   = xdc.useModule('ti.uia.runtime.ServiceMgr');
    UIAPacket    = xdc.useModule('ti.uia.runtime.UIAPacket');
    Settings     = xdc.useModule("ti.uia.family.Settings");
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    /* Create the free event packets semaphore, queue and packets */
    var semParams = new Semaphore.Params();
    semParams.instance.name = "UIA Adaptor's free event semaphore";
    mod.freeEventSem = Semaphore.create(ServiceMgr.numEventPacketBufs,
                                        semParams);

    var queueParams = new Queue.Params();
    queueParams.instance.name = "UIA Adaptor's free event queue";
    mod.freeEventQ = Queue.create(queueParams);

    this.eventBuf.length = ServiceMgr.numEventPacketBufs * (
        ServiceMgr.maxEventPacketSize + Queue.Elem.$sizeof());

    if (params.packetSection === undefined) {
        Memory.staticPlace(this.eventBuf, 0,
            ServiceMgr.transferAgentStackSection);
    }
    else {
        Memory.staticPlace(this.eventBuf, 0, params.packetSection);
    }

    /* Only create if control messages are coming in */
    if (ServiceMgr.supportControl == true) {

        /* Create control messages */
        this.msgBuf.length =
            (ServiceMgr.maxCtrlPacketSize + Queue.Elem.$sizeof()) *
            (ServiceMgr.numIncomingCtrlPacketBufs +
             ServiceMgr.numOutgoingCtrlPacketBufs);

        /* Place the packets */
        if (params.packetSection === undefined) {
            Memory.staticPlace(this.msgBuf, 0, ServiceMgr.rxTaskStackSection);
        }
        else {
            Memory.staticPlace(this.msgBuf, 0, ServiceMgr.packetSection);
        }

        /*
         *  Create the semaphore for the free messages. Initialize the count
         *  to the number of packets in the queue.
         */
        semParams.instance.name = "Adaptor's free msg semaphore";
        mod.freeMsgSem = Semaphore.create(
            (ServiceMgr.numIncomingCtrlPacketBufs +
             ServiceMgr.numOutgoingCtrlPacketBufs),
            semParams);

        /* Create the incoming message semaphore */
        semParams.instance.name = "UIA Adaptor's incoming msg semaphore";
        mod.incomingSem = Semaphore.create(0, semParams);

        /* Create the free message queue */
        queueParams.instance.name = "UIA Adaptor's free msg queue";
        mod.freeMsgQ   = Queue.create(queueParams);

        /* Create the incoming message queue */
        queueParams.instance.name = "UIA Adaptor's incoming queue";
        mod.incomingQ  = Queue.create(queueParams);

        /* Create the outgoing message queue */
        queueParams.instance.name = "UIA Adaptor's outgoing queue";
        mod.outgoingQ  = Queue.create(queueParams);
    }
    else {
        mod.freeMsgSem = null;
        mod.incomingSem = null;
        mod.freeMsgQ = null;
        mod.incomingQ = null;
        mod.outgoingQ = null;
    }

    /* Filled in by the Transport_start function calls */
    mod.transportEventHandle = null;
    mod.transportMsgHandle   = null;

    /* Statistics */
    mod.numEventPacketsSent   = 0;
    mod.numMsgPacketsSent     = 0;
    mod.numMsgPacketsFailed   = 0;
    mod.numEventPacketsFailed = 0;

    /* Create Tx Agent Task */
    var taskParams = new Task.Params();
    taskParams.priority = ServiceMgr.transferAgentPriority;
    taskParams.instance.name = "UIA Adaptor Transfer Agent";
    taskParams.stackSize = ServiceMgr.transferAgentStackSize;
    if (ServiceMgr.transferAgentStackSection != null) {
        taskParams.stackSection = ServiceMgr.transferAgentStackSection;
    }
    mod.transferAgentHandle = Task.create(
         '&ti_uia_sysbios_Adaptor_transferAgentTaskFxn__E',
         taskParams);

    /* Create Rx Task only if supportControl is true */
    if (ServiceMgr.supportControl == true) {

        if (ServiceMgr.rxTaskStackSection != null) {
            taskParams.stackSection = ServiceMgr.rxTaskStackSection;
        }
        taskParams.priority = ServiceMgr.rxTaskPriority;
        taskParams.instance.name = "UIA Adaptor Rx Task";
        taskParams.stackSize = ServiceMgr.rxTaskStackSize;
        Task.create('&ti_uia_sysbios_Adaptor_rxTaskFxn__E',
                    taskParams);
    }

    /*
     * Create a Clock to periodically post the event.
     * Convert the transfer period in ms to Clock ticks. The tickPeriod is in
     * microseconds.
     */
    var clockTicks = Math.floor((ServiceMgr.periodInMs * 1000) /
                                Clock.tickPeriod);

    /* make sure the clock is a least 1 */
    if ((clockTicks == 0) && (ServiceMgr.periodInMs != 0)) {
        clockTicks = 1;
    }
    var clockParams = new Clock.Params();
    clockParams.period = clockTicks;
    clockParams.startFlag = true;
    mod.clock = Clock.create(
                    "&ti_uia_sysbios_Adaptor_clockFxn__E",
                    clockTicks, clockParams);
    mod.clock.instance.name = "UIA Adaptor Clock";

    /* Create the event, which is posted by both MessageQ and the Clock. */
    mod.event = Event.create();
    mod.event.instance.name = "Adaptor Event";

    /*
     *  Create a SyncEvent. The event will be posted a message is
     *  sent to the transfer agent.
     */
    var syncEventPrms = new SyncEvent.Params();
    syncEventPrms.event = mod.event;
    syncEventPrms.eventId = Event.Id_01;
    mod.syncEvent01 = SyncEvent.create(syncEventPrms);
    mod.syncEvent01.instance.name = "UIA Adaptor SyncEvent01";
}

/*
 * ======== newService ========
 */
function newService(id, periodInMs)
{
    /* If new id is larger than numServices, bump up the array lengths */
    if (id >= numServices) {
        this.$object.period.length = id + 1;
        this.$object.scheduled.length = id + 1;
        this.$object.reqEnergy.length = id + 1;

        /* Fill in any gap created */
        for (var i = numServices; i < id; i++) {
            this.$object.period[i]    = 0;
            this.$object.scheduled[i] = 0;
            this.$object.reqEnergy[i] = false;
        }

        numServices = id + 1;
    }

    /* fill in the supplied values */
    this.$object.period[id]    = periodInMs;
    this.$object.scheduled[id] = 0;
    this.$object.reqEnergy[id] = false;
}
