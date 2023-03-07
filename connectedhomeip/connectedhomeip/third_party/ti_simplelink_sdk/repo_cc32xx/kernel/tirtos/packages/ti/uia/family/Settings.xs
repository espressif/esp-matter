/*
 * Copyright (c) 2012, Texas Instruments Incorporated
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
 *  ======== Settings.xs ========
 */

/* NDK Transport */
var ndkTransport = {
    initFxn: '&TransportNdk_init',
    startFxn: '&TransportNdk_start',
    recvFxn: '&TransportNdk_recv',
    sendFxn: '&TransportNdk_send',
    stopFxn: '&TransportNdk_stop',
    exitFxn: '&TransportNdk_exit',
};

/* File Transport */
var fileTransport = {
    initFxn: '&TransportFile_init',
    startFxn: '&TransportFile_start',
    recvFxn: '&TransportFile_recv',
    sendFxn: '&TransportFile_send',
    stopFxn: '&TransportFile_stop',
    exitFxn: '&TransportFile_exit',
};

/* NULL Transport */
var nullTransport = {
    initFxn: null,
    startFxn: null,
    recvFxn: null,
    sendFxn: null,
    stopFxn: null,
    exitFxn: null,
}

/* Table of settings for non-NDK supported devices. */
var devices = {
    "TMS320TI816X": {
        transport:       nullTransport
    },
    "TMS320TI814X": {
        transport:       nullTransport
    },
};

/*
 *  ======== getDefaultTransport ========
 *  returns the default transport to use on this device.
 */
function getDefaultTransport()
{
    var ServiceMgr = xdc.useModule('ti.uia.runtime.ServiceMgr');

    var localDevices = {
        "TMS320TI816X": {
            transport: ServiceMgr.TransportType_NULL
        },
        "TMS320TI814X": {
            transport: ServiceMgr.TransportType_NULL
        },
    };

    var transport;
    if (localDevices[Program.cpu.deviceName] == undefined) {
        transport = ServiceMgr.TransportType_ETHERNET;
    }
    else {
        transport = localDevices[Program.cpu.deviceName].transport;
    }

    /* If SYS/BIOS is on the CortexA8 default to Ethernet */
    if ((Program.cpu.deviceName == "TMS320TI814X") &&
        (Program.cpu.catalogName == "ti.catalog.arm.cortexa8")) {
        transport = ServiceMgr.TransportType_ETHERNET;
    }

    return (transport);
}

/*
 *  ======== getServiceMgrEventPacketSize ========
 */
function getServiceMgrEventPacketSize()
{
    var ServiceMgr = xdc.useModule('ti.uia.runtime.ServiceMgr');

    if (ServiceMgr.transportType == ServiceMgr.TransportType_ETHERNET) {
        return (1472);
    }
    else  {
        return (1024);
    }
}

/*
 *  ======== getServiceMgrMsgPacketSize ========
 *  The default msg packet size is 128 for all transports and devices.
 */
function getServiceMgrMsgPacketSize()
{
    return (128);
}

/*
 *  ======== getSupportControl ========
 *  Returns whether the configured transport supports control on this device.
 */
function getSupportControl()
{
    var ServiceMgr = xdc.useModule('ti.uia.runtime.ServiceMgr');

    var device = Program.cpu.deviceName;

    if (ServiceMgr.transportType == ServiceMgr.TransportType_ETHERNET) {
        return (true);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_FILE) {
        /* File does not support control. */
        return (false);
    }
    else {
        // TODO What to do here?
        return (false);
    }
}

/*
 *  ======== initFxn ========
 */
function initFxn()
{
    var ServiceMgr = xdc.useModule('ti.uia.runtime.ServiceMgr');
    var settings = devices[Program.cpu.deviceName];

    if (ServiceMgr.transportType == ServiceMgr.TransportType_FILE) {
        return (fileTransport.initFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_NULL) {
        return (nullTransport.initFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_ETHERNET) {
        return (ndkTransport.initFxn);
    }
    else if (settings === undefined) {
        return (ndkTransport.initFxn);
    }

    return (settings.transport.initFxn);
}

/*
 *  ======== startFxn ========
 */
function startFxn()
{
    var ServiceMgr = xdc.useModule('ti.uia.runtime.ServiceMgr');
    var settings = devices[Program.cpu.deviceName];

    if (ServiceMgr.transportType == ServiceMgr.TransportType_FILE) {
        return (fileTransport.startFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_NULL) {
        return (nullTransport.startFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_ETHERNET) {
        return (ndkTransport.startFxn);
    }
    else if (settings === undefined) {
        return (ndkTransport.startFxn);
    }

    return (settings.transport.startFxn);
}

/*
 *  ======== recvFxn ========
 */
function recvFxn()
{
    var ServiceMgr = xdc.useModule('ti.uia.runtime.ServiceMgr');
    var settings = devices[Program.cpu.deviceName];

    if (ServiceMgr.transportType == ServiceMgr.TransportType_FILE) {
        return (fileTransport.recvFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_NULL) {
        return (nullTransport.recvFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_ETHERNET) {
        return (ndkTransport.recvFxn);
    }
    else if (settings === undefined) {
        return (ndkTransport.recvFxn);
    }

    return (settings.transport.recvFxn);
}

/*
 *  ======== sendFxn ========
 */
function sendFxn()
{
    var ServiceMgr = xdc.useModule('ti.uia.runtime.ServiceMgr');
    var settings = devices[Program.cpu.deviceName];

    if (ServiceMgr.transportType == ServiceMgr.TransportType_FILE) {
        return (fileTransport.sendFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_NULL) {
        return (nullTransport.sendFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_ETHERNET) {
        return (ndkTransport.sendFxn);
    }
    else if (settings === undefined) {
        return (ndkTransport.sendFxn);
    }

    return (settings.transport.sendFxn);
}

/*
 *  ======== stopFxn ========
 */
function stopFxn()
{
    var ServiceMgr = xdc.useModule('ti.uia.runtime.ServiceMgr');
    var settings = devices[Program.cpu.deviceName];

    if (ServiceMgr.transportType == ServiceMgr.TransportType_FILE) {
        return (fileTransport.stopFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_NULL) {
        return (nullTransport.stopFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_ETHERNET) {
        return (ndkTransport.stopFxn);
    }
    else if (settings === undefined) {
        return (ndkTransport.stopFxn);
    }

    return (settings.transport.stopFxn);
}

/*
 *  ======== exitFxn ========
 */
function exitFxn()
{
    var ServiceMgr = xdc.useModule('ti.uia.runtime.ServiceMgr');
    var settings = devices[Program.cpu.deviceName];

    if (ServiceMgr.transportType == ServiceMgr.TransportType_FILE) {
        return (fileTransport.exitFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_NULL) {
        return (nullTransport.exitFxn);
    }
    else if (ServiceMgr.transportType == ServiceMgr.TransportType_ETHERNET) {
        return (ndkTransport.exitFxn);
    }
    else if (settings === undefined) {
        return (ndkTransport.exitFxn);
    }

    return (settings.transport.exitFxn);
}
