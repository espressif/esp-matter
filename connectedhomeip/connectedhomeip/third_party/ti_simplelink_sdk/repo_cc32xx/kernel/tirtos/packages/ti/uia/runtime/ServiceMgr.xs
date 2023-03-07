/*
 * Copyright (c) 2012-2014, Texas Instruments Incorporated
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
 *  ======== ServiceMgr.xs ========
 */

var ServiceMgr   = null;
var UAPacket     = null;
var Settings     = null;
var UIAMetaData  = null;

/*
 *  Reserve the first three ids for
 *    0 - Discovery service
 *    1 - RPC service
 *    2 - TCF service
 */
var serviceId = 3;

/*
 *  ======== module$use ========
 */
function module$use()
{
    ServiceMgr  = this;
    UIAPacket   = xdc.useModule("ti.uia.runtime.UIAPacket");
    Settings    = xdc.useModule("ti.uia.family.Settings");
    UIAMetaData = xdc.useModule('ti.uia.runtime.UIAMetaData');

    /*
     * Set the SupportProxy based on the 'multicore' config param.
     * The SupportProxy config is nodoc'd, but it's still possible to set it,
     * so we don't want to overwrite it if it has been set explicitly.
     *
     * TODO - Warn if multicore selected on single core device.
     */
    if (ServiceMgr.SupportProxy == null) {
        if (ServiceMgr.topology == ServiceMgr.Topology_SINGLECORE) {
            ServiceMgr.SupportProxy =
                xdc.useModule('ti.uia.sysbios.Adaptor');
        }
        else {
            ServiceMgr.SupportProxy =
                xdc.useModule('ti.uia.sysbios.IpcMP');
        }
    }

    /*
     * If the transport has not been specified, get the default transport from
     * the family Settings module.
     */
    if (ServiceMgr.transportType == null) {
        ServiceMgr.transportType = Settings.getDefaultTransport();
    }

    /*
     * If the transport is not user-configured, then get the default packet
     * sizes. Only use the default size if the user has not explicitly
     * specified it already.
     */
    if (ServiceMgr.transportType != ServiceMgr.TransportType_USER) {
        /* Get the default Event Packet size if not set */
        if (ServiceMgr.maxEventPacketSize == null) {
            ServiceMgr.maxEventPacketSize =
                Settings.getServiceMgrEventPacketSize();
        }

        /* Get the default Control Msg Packet size if not set */
        if (ServiceMgr.maxCtrlPacketSize == null) {
            ServiceMgr.maxCtrlPacketSize =
                Settings.getServiceMgrMsgPacketSize();
        }

        /* Get the default value for supportControl */
        if (ServiceMgr.supportControl == null) {
            ServiceMgr.supportControl = Settings.getSupportControl();
        }
    }

    if (ServiceMgr.transportFxns.initFxn === undefined) {
        ServiceMgr.transportFxns.initFxn = Settings.initFxn();
    }
    if (ServiceMgr.transportFxns.startFxn === undefined) {
        ServiceMgr.transportFxns.startFxn = Settings.startFxn();
    }
    if (ServiceMgr.transportFxns.recvFxn === undefined) {
        ServiceMgr.transportFxns.recvFxn = Settings.recvFxn();
    }
    if (ServiceMgr.transportFxns.sendFxn === undefined) {
        ServiceMgr.transportFxns.sendFxn = Settings.sendFxn();
    }
    if (ServiceMgr.transportFxns.stopFxn === undefined) {
        ServiceMgr.transportFxns.stopFxn = Settings.stopFxn();
    }
    if (ServiceMgr.transportFxns.exitFxn === undefined) {
        ServiceMgr.transportFxns.exitFxn = Settings.exitFxn();
    }


    /* To save space */
    this.SupportProxy$proxy.common$.fxntab = false;
    this.SupportProxy.common$.fxntab = false;
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    if (xdc.om.$name != "cfg") {
        return;
    }

    mod.runCount = 0;

    /* for every module in the system find and assign the service ids */
    for (var i = 0; i < xdc.om.$modules.length; i++) {
        var curMod = xdc.om.$modules[i];

        if (curMod.$used) {
            for each (var prop in curMod) {
                if (prop instanceof this.ServiceId) {
                    prop.val = serviceId++;
                }
            }
        }
    }

    mod.numServices = serviceId;
    this.processCallbackFxn.length = serviceId;

    /* Default the callback functions to NULL */
    for (i = 0; i < mod.numServices; i++) {
        this.processCallbackFxn[i] = null;
    }

    var multicore;
    if (ServiceMgr.topology == ServiceMgr.Topology_MULTICORE) {
        multicore = true;
    }
    else {
        multicore = false;
    }

    var transportType;
    switch(ServiceMgr.transportType){
        case ServiceMgr.TransportType_FILE:
            transportType = "FILE";
            break;
        case ServiceMgr.TransportType_USER:
            transportType = "CUSTOM";
            break;
        case ServiceMgr.TransportType_ETHERNET:
            transportType = "ETHERNET";
            break;
        default:
            transportType = "NONE";
            break;
    }

    UIAMetaData.setTransportFields(multicore,
                                   UIAMetaData.UploadMode_NONJTAGTRANSPORT,
                                   transportType, "UIAPACKET",
                                   ServiceMgr.supportControl,
                                   ServiceMgr.customTransportType);
}

/*
 *  ======== register ========
 */
function register(id, processCallback, periodInMs)
{
    /* Make sure the id is in range */
    if (id.val > serviceId) {
        ServiceMgr.$logError("Invalid Service id (" + id + ")", this,
                             "register");
    }

    /* Cannot register null function */
    if (processCallback == null) {
        ServiceMgr.$logError("Cannot register a null function", this,
                             "register");
    }

    /* If a function specified, plug it in */
    this.processCallbackFxn[id.val] = processCallback;

    /* Call the proxy's newService function */
    this.SupportProxy.delegate$.newService(id.val, periodInMs);
}

/*
 * ======== ServiceId$sizeof ========
 */
function ServiceId$sizeof()
{
    return Program.build.target.stdTypes.t_Bits16.size;
}

/*
 * ======== ServiceId$alignof ========
 */
function ServiceId$alignof()
{
    return Program.build.target.stdTypes.t_Bits16.align;
}

/*
 * ======== ServiceId$encode ========
 */
function ServiceId$encode( desc )
{
    return !desc ? "0" : ("((ti_uia_runtime_ServiceMgr_ServiceId)" + desc.val +
                          ")");
}

/*
 *  ======== viewInitModule ========
 */
function viewInitModule(view, mod)
{
    var ServiceMgr  = xdc.useModule("ti.uia.runtime.ServiceMgr");

    var serviceMgrModConfig = Program.getModuleConfig(ServiceMgr.$name);

    view.periodInMs     = serviceMgrModConfig.periodInMs;
    view.supportControl = serviceMgrModConfig.supportControl;
    if (serviceMgrModConfig.topology.equals(String(ServiceMgr.Topology_SINGLECORE))) {
        view.topology       = "Topology_SINGLECORE";
    }
    else {
        view.topology       = "Topology_MULTICORE";
    }
    view.masterProcId   = serviceMgrModConfig.masterProcId;
    view.numServices    = mod.numServices;
    view.runCount       = mod.runCount;
    // TODO: SDOCM00077324 ServiceMgr's ROV should obtain information from proxies
    //view.masterRunning = "true";
}

/*
 *  ======== viewInitPacket ========
 */
function viewInitPacket(view, mod)
{
    var ServiceMgr  = xdc.useModule("ti.uia.runtime.ServiceMgr");

    var serviceMgrModConfig = Program.getModuleConfig(ServiceMgr.$name);

    view.maxEventPacketSize = serviceMgrModConfig.maxEventPacketSize;
    view.numEventPacketBufs = serviceMgrModConfig.numEventPacketBufs;
    view.maxCtrlPacketSize  = serviceMgrModConfig.maxCtrlPacketSize;
    view.numOutgoingCtrlPacketBufs =
        serviceMgrModConfig.numOutgoingCtrlPacketBufs;
    view.numIncomingCtrlPacketBufs =
        serviceMgrModConfig.numIncomingCtrlPacketBufs;
}

/*
 *  ======== viewInitStats ========
 *  Initializes the transport view in ROV.
 */
function viewInitStats(view, mod)
{
    // TODO: SDOCM00077324
    // ServiceMgr's ROV should obtain information from proxies
}

/*
 *  ======== viewInitTransport ========
 *  Initializes the transport view in ROV.
 */
function viewInitTransport(view, mod)
{
    var ServiceMgr  = xdc.useModule("ti.uia.runtime.ServiceMgr");
    var serviceMgrModConfig = Program.getModuleConfig(ServiceMgr.$name);

    if (serviceMgrModConfig.transportFxns.initFxn != "") {
        view.initFxn  = serviceMgrModConfig.transportFxns.initFxn;
    }
    else {
        view.initFxn  = "N/A";
    }
    if (serviceMgrModConfig.transportFxns.startFxn != "") {
        view.startFxn  = serviceMgrModConfig.transportFxns.startFxn;
    }
    else {
        view.startFxn  = "N/A";
    }

    if (serviceMgrModConfig.transportFxns.recvFxn != "") {
        view.recvFxn  = serviceMgrModConfig.transportFxns.recvFxn;
    }
    else {
        view.recvFxn  = "N/A";
    }

    if (serviceMgrModConfig.transportFxns.sendFxn != "") {
        view.sendFxn  = serviceMgrModConfig.transportFxns.sendFxn;
    }
    else {
        view.sendFxn  = "N/A";
    }

    if (serviceMgrModConfig.transportFxns.stopFxn != "") {
        view.stopFxn  = serviceMgrModConfig.transportFxns.stopFxn;
    }
    else {
        view.stopFxn  = "N/A";
    }

    if (serviceMgrModConfig.transportFxns.exitFxn != "") {
        view.exitFxn  = serviceMgrModConfig.transportFxns.exitFxn;
    }
    else {
        view.exitFxn  = "N/A";
    }
}
