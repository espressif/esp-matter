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
 *  ======== IpcMP.xs ========
 */

var IpcMP = null;

var Clock = null;
var Task = null;
var Gate = null;
var Event = null;
var Hwi = null;
var Semaphore = null;
var Log = null;
var Memory = null;
var MessageQ = null;
var SharedRegion = null;
var Ipc = null;
var MultiProc = null;
var SyncEvent = null;
var ServiceMgr = null;
var UIAPacket = null;
var Settings = null;
var MultiCoreTypes = null;

var numServices = 0;
var plugUserFxn = true;


/*
 *  ======== module$use ========
 */
function doNotPlugIpc()
{
    plugUserFxn = false;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    IpcMP = this;

    /* Bring in all modules used directly by the Agent. */
    Clock        = xdc.useModule('ti.sysbios.knl.Clock');
    Task         = xdc.useModule('ti.sysbios.knl.Task');
    Gate         = xdc.useModule('xdc.runtime.Gate');
    Event        = xdc.useModule('ti.sysbios.knl.Event');
    Hwi          = xdc.useModule('ti.sysbios.hal.Hwi');
    Semaphore    = xdc.useModule('ti.sysbios.knl.Semaphore');
    Log          = xdc.useModule('xdc.runtime.Log');
    Memory       = xdc.useModule('xdc.runtime.Memory');
    MessageQ     = xdc.useModule('ti.sdo.ipc.MessageQ');
    SharedRegion = xdc.useModule('ti.sdo.ipc.SharedRegion');
    Ipc          = xdc.useModule('ti.sdo.ipc.Ipc');
    MultiProc    = xdc.useModule('ti.sdo.utils.MultiProc');
    SyncEvent    = xdc.useModule('ti.sysbios.syncs.SyncEvent');
    ServiceMgr   = xdc.useModule('ti.uia.runtime.ServiceMgr');
    UIAPacket    = xdc.useModule('ti.uia.runtime.UIAPacket');
    Settings     = xdc.useModule("ti.uia.family.Settings");
    MultiCoreTypes = xdc.useModule('ti.uia.runtime.MultiCoreTypes');

    xdc.useModule('xdc.runtime.Error');
    xdc.useModule('ti.sysbios.BIOS');

    /* work-around Syslink's SDOCM00077375 bug */
    if (plugUserFxn == true) {
        try {
            registerFxn = new Ipc.UserFxn;
            registerFxn.attach = '&ti_uia_sysbios_IpcMP_start__E';
            registerFxn.detach = null;
            Ipc.addUserFxn(registerFxn, null);
        }
        catch(err)
        {
            if ((Ipc.userFxn.attach == null) &&  (Ipc.userFxn.detach == null)){
                registerFxn.attach = '&ti_uia_sysbios_IpcMP_startOrig__I';
                registerFxn.detach = null;
                Ipc.userFxn = registerFxn;
            }
            else {
                print("Warning: You must call ti_uia_sysbios_IpcMP_start__E(0,0) after Ipc_attach since the Ipc.userFxn is already used");
            }
        }
    }
    else {
        print("WARNING: Not adding Ipc.userFxn because of SDOCM00077375. Make sure to call \"ti_uia_sysbios_IpcMP_start__E(0, 0);\" in your app.");
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    mod.replyMQ.length = MultiProc.numProcessors;

    for (var i = 0; i < MultiProc.numProcessors; i++) {
        mod.replyMQ[i] = 0xFFFF; //TODO figure out how to use MessageQ.INVALIDMESSAGEQ
    }

    mod.numEventPacketsSent = 0;
    mod.numMSGPacketsSent = 0;
    mod.incomingMsgMQ = null;
    mod.freeEventMQ   = null;
    mod.freeMsgMQ     = null;
    mod.routerMQ      = null;
    mod.startedMQ     = null;
    mod.masterMQ      = ~(0);
    mod.masterRunning = false;
    mod.transportEventHandle = null;
    mod.transportMsgHandle   = null;

    /*
     *  Create the semaphore that is used to release the tasks
     *  once Ipc_start/Ipc_attach is done.
     */
    var semParams         = new Semaphore.Params();
    semParams.instance.name = "IpcMP releaseSem";
    mod.releaseTasksSem = Semaphore.create(0, semParams);

    /* Create Tx Agent Task */
    var taskParams = new Task.Params();
    taskParams.priority = ServiceMgr.transferAgentPriority;
    taskParams.instance.name = "IpcMP Tx";
    taskParams.stackSize = ServiceMgr.transferAgentStackSize;
    if (ServiceMgr.transferAgentStackSection != null) {
        taskParams.stackSection = ServiceMgr.transferAgentStackSection;
    }
    mod.transferAgentHandle = Task.create(
         '&ti_uia_sysbios_IpcMP_transferAgentFxn__E',
         taskParams);

    /*
     *  Three options here:
     *  MultiProc.id is INVALIDID: defer rxTask creation until startup if
     *      ServiceMgr.supportControl = true. Otherwise never create.
     *  MultiProc.id == masterProcId: create rxTask
     *  MultiProc.id != masterProcId: do not create rxTask
     */
    if (MultiProc.id == MultiProc.INVALIDID) {
        if (ServiceMgr.supportControl == true) {
            IpcMP.createRxTask = true;
        }
        mod.master = false; /* Determine during runtime */
    }
    else if ((ServiceMgr.masterProcId == MultiProc.id) ||
             (ServiceMgr.masterProcId == MultiProc.INVALIDID)) {
        /* Create Rx Agent Task */
        mod.master = true;
        taskParams.priority = ServiceMgr.rxTaskPriority;
        taskParams.instance.name = "IpcMP Rx";
        taskParams.stackSize = ServiceMgr.rxTaskStackSize;
        Task.create('&ti_uia_sysbios_IpcMP_rxTaskFxn__E', taskParams);
        IpcMP.createRxTask = false;
    }
    else {
        mod.master = false;
    }

    /*
     * Create a Clock to periodically post the event.
     * Convert the transfer period in ms to Clock ticks. The tickPeriod is in
     * microseconds.
     */
    var clockTicks = Math.floor((ServiceMgr.periodInMs * 1000) / Clock.tickPeriod);

    if ((clockTicks == 0) && (ServiceMgr.periodInMs != 0)) {
        clockTicks = 1;
    }
    var clockParams = new Clock.Params();
    clockParams.period = clockTicks;
    clockParams.startFlag = true;
    mod.clock = Clock.create("&ti_uia_sysbios_IpcMP_clockFxn__E", clockTicks,
                             clockParams);
    mod.clock.instance.name = "IpcMP Clock";

    /* Create the event, which is posted by both MessageQ and the Clock. */
    mod.event = Event.create();
    mod.event.instance.name = "IpcMP Event";

    /*
     *  Create a SyncEvent. The event will be posted a message is
     *  sent to the transfer agent.
     */
    var syncEventPrms = new SyncEvent.Params();
    syncEventPrms.event = mod.event;
    syncEventPrms.eventId = Event.Id_01;
    mod.syncEvent01 = SyncEvent.create(syncEventPrms);
    mod.syncEvent01.instance.name = "IpcMP SyncEvent01";
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

        /* Fill in any gaps created */
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
