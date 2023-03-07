/*
 * Copyright (c) 2013-2016, Texas Instruments Incorporated
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
 *  ======== LoggerIdle.xs ========
 */

var LoggerIdle = null;

var Idle = null;
var Types = null;
var Timestamp = null;
var EventHdr = null;
var UIAMetaData = null;

/*
 *  Size of temp buffer to copy Event into.  Must be large
 *  enough to hold (WRITE8_IN_BITS32 + 2) = 12 words.  Make
 *  it 16 just to be safe.
 */
var tempBufferSize = 16;


/*
 *  ======== module$use ========
 */
function module$use()
{
    LoggerIdle = this;

    Idle = xdc.useModule("ti.sysbios.knl.Idle");
    Types = xdc.useModule('xdc.runtime.Types');
    Timestamp = xdc.useModule('xdc.runtime.Timestamp');
    EventHdr = xdc.useModule('ti.uia.runtime.EventHdr');
    UIAMetaData = xdc.useModule('ti.uia.runtime.UIAMetaData');

}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    if (LoggerIdle.$written("writeWhenFull")) {
        LoggerIdle.$logWarning("The LoggerIdle.writeWhenFull configuration " +
                               "parameter has been deprecated.  If the " +
                               "buffer is full, old data will be overwritten.",
                LoggerIdle, "writeWhenFull");
    }
    Idle.addFunc(LoggerIdle.idleWriteEvent);

    if (params.transportFxn == null) {
        this.$logError("LoggerIdle cannot have a null transport function", this);
    }
    mod.loggerFxn = params.transportFxn;
    mod.enabled = true;
    mod.empty = true;
    mod.eventSequenceNum = 0;
    mod.bufferSize = params.bufferSize;
    mod.idleBuffer.length = params.bufferSize;
    mod.tempBuffer.length = tempBufferSize;
    mod.bufferRead = mod.idleBuffer.$addrof(0);
    mod.bufferWrite = mod.idleBuffer.$addrof(0);
    mod.bufferPad = mod.idleBuffer.$addrof(params.bufferSize - 1);
    if (params.isTimestampEnabled) {
        mod.bufferEnd = mod.idleBuffer.$addrof(params.bufferSize - 12);
    }
    else {
        mod.bufferEnd = mod.idleBuffer.$addrof(params.bufferSize - 10);
    }

    var transportType;
    switch(params.transportType){
        case this.TransportType_UART:
            transportType = "UART";
            break;
        case this.TransportType_USB:
            transportType = "USB";
            break;
        case this.TransportType_ETHERNET:
            transportType = "ETHERNET";
            break;
        case this.TransportType_CUSTOM:
            transportType = "CUSTOM";
            break;
        default:
            this.$logError("Unkown transport type", this);
            break;
    }
    UIAMetaData.setTransportFields(false,
                                   UIAMetaData.UploadMode_NONJTAGTRANSPORT,
                                   transportType,
                                   "UIAEVENTHDR", false,
                                   params.customTransportType);
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, params)
{
}

/*
 *  ======== viewInitModule ========
 *  Initializes the Module view in ROV.
 */
function viewInitModule(view, mod)
{
    var LoggerIdle  = xdc.useModule("ti.uia.sysbios.LoggerIdle");
    var loggerIdleModConfig = Program.getModuleConfig(LoggerIdle.$name);

    /* Populate ROV module view */
    view.isEnabled = mod.enabled;
    view.isTimestampEnabled = loggerIdleModConfig.isTimestampEnabled;
    view.bufferSize = loggerIdleModConfig.bufferSize;
    view.sequenceNumber = mod.eventSequenceNum;
    view.transportType = loggerIdleModConfig.transportType;
    if (loggerIdleModConfig.transportType ==
        LoggerIdle.TransportType_CUSTOM) {
        view.customTransport = loggerIdleModConfig.customTransport;
    }
    else {
        view.customTransport = "N/A"
    }
}

/*
 *  ======== viewInitRecords ========
 *  Initializes the Records view in ROV.
 */
function viewInitRecords(view)
{
    xdc.loadPackage('ti.uia.runtime');
    xdc.loadPackage('ti.uia.sysbios');
    var Model = xdc.useModule('xdc.rov.Model');
    var Program = xdc.useModule('xdc.rov.Program');
    var LoggerIdle = xdc.useModule('ti.uia.sysbios.LoggerIdle');
    var loggerIdleModConfig = Program.getModuleConfig(LoggerIdle.$name);
    var timestamp = loggerIdleModConfig.isTimestampEnabled;

    /*
     * Retrieve the module's state.
     * If this throws an exception, just allow it to propogate up.
     */
    var rawView = Program.scanRawView('ti.uia.sysbios.LoggerIdle');

    if (rawView.modState.empty) {
        /* Buffer is empty */
        return;
    }

    /* Get the memory reader */
    var memReader = Model.getMemoryImageInst();

    /* Initialize the LoggerIdle decoder */
    var decoder;
    try {
        decoder = LoggerIdle.initDecoder();
    }
    catch (e) {
        print("Caught exception while initiailizing decoder: " + e);
        /* Create a new view element to display the exception. */
        var evtView = Program.newViewStruct('ti.uia.sysbios.LoggerIdle',
                'Records');

        /* Dispaly the exception in the 'sequence' field. */
        Program.displayError(evtView, 'sequence',
                "Caught exception while initiailizing decoder: " + e);

        /* Add the element and return. */
        view.elements = [evtView];

        return;
    }

    if (rawView.modState.empty) {
        return;
    }

    /* Read the buffer. */
    var buffer;
    try {
        buffer = decoder.readTargetBuffer(memReader, rawView.modState.idleBuffer,
                rawView.modState.bufferRead, rawView.modState.bufferWrite,
                rawView.modState.bufferPad);
    }
    catch (e) {
        print("Caught exception while reading buffer: " + e);

        /* Create a new view element to display the exception. */
        var evtView = Program.newViewStruct('ti.uia.sysbios.LoggerIdle',
                                            'Records');

        /* Dispaly the exception in the 'sequence' field. */
        Program.displayError(evtView, 'sequence',
                             "Caught exception while reading buffer: " + e);

        /* Add the element and return. */
        view.elements = [evtView];

        return;
    }

    /* Decode the buffer and display the records. */
    var eventViews = new Array();
    var offset = 0;             /* Offset */

    /*
     * Loop over all of the records in the buffer.
     * 'buffer' contains only valid, contiguous records.
     */
    while (offset < buffer.length) {
        /* Create a new view element. */
        var evtView =
            Program.newViewStruct('ti.uia.sysbios.LoggerIdle', 'Records');

        /* Decode the next event. */
        var evt;
        try {
            evt = decoder.decodeEvent(buffer, offset, buffer.length);

            /* Fill in the element. */
            evtView.sequence = evt.sequenceNum;
            evtView.timestampRaw = evt.timestamp;
            evtView.modName = evt.getModuleName();
            evtView.text = evt.getEventMsg();
            evtView.eventId = evt.eventId;
            evtView.eventName = evt.getEventName();
            evtView.arg0 = evt.args[0];
            evtView.arg1 = evt.args[1];
            evtView.arg2 = evt.args[2];
            evtView.arg3 = evt.args[3];
            evtView.arg4 = evt.args[4];
            evtView.arg5 = evt.args[5];
            evtView.arg6 = evt.args[6];
            evtView.arg7 = evt.args[7];

            /* Move the offset to the next record. */
            offset += decoder.getEventLength(buffer, offset);
        }
        catch (e) {
            print("Caught exception decoding event: " + e);

            /* Display the exception in the 'sequence' column. */
            Program.displayError(evtView, 'sequence',
                    "Caught exception while decoding event: " + e);

            /* Add the element to the view and return. */
            eventViews[eventViews.length] = evtView;

            view.elements = eventViews;

            return;
        }

        /* Add the element to the view. */
        eventViews[eventViews.length] = evtView;
    }

    /* Assign the view elements to the view object before returning. */
    view.elements = eventViews;
}

/*
 *  ======== getMetaArgs ========
 *  Returns meta data to support stop-mode RTA.
 */
function getMetaArgs(inst, instNum)
{
    var LoggerIdle = xdc.module('ti.uia.sysbios.LoggerIdle');

    /* Create a new StopModeData structure to populate. */
    var data = new LoggerIdle.RtaData();
    data.instanceId = 0;
    return (data);
}

/*
 *  ======== initDecoder ========
 *  Creates an instance of the ti.uia.sysbios.LoggerIdle class.
 *  The LoggerIdle 'Records' ROV view uses the LoggerIdleDecoder class
 *  to decode the raw bytes into xdc.rta.HostEvent objects. This decoder is
 *  shared between this ROV view and UIA.
 */
function initDecoder()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var LoggerIdle = xdc.useModule('ti.uia.sysbios.LoggerIdle');
    var Model = xdc.useModule('xdc.rov.Model');
    var Log = xdc.useModule('xdc.runtime.Log');

    /* Check if the decoder has already been created. */
    if (LoggerIdle.$private.decoder != undefined) {
        return (LoggerIdle.$private.decoder);
    }

    /* Get the IOFReader instance */
    var ofReader = Model.getIOFReaderInst();

    /*
     * In order to decode the events in a LoggerIdle, the
     * LoggerIdleDecoder class requires an object which implements the
     * IEventMetaData interface to provide log event information such as the
     * mapping of an event id to the event's message.
     *
     * For ROV, we create a JavaScript object which implements all of the APIs
     * in the IEventMetaData class, then place a Java wrapper on this so that
     * it can be called by the LoggerIdleDecoder class.
     */
    var eventData = {
        getOFReader:
            function() {
                return (ofReader);
            },
        lookupEventMessage:
            function (eventId) {
                return (Log.lookupEventMessage(eventId));
            },
        getTargetArgSize:
            function() {
                return (Program.build.target.stdTypes.t_IArg.size *
                        Program.build.target.bitsPerChar / 8);
            },
        lookupEventName:
            function(eventId) {
                return (Log.lookupEventName(eventId));
            },
        lookupModuleName:
            function(modId) {
                return (Log.lookupModuleName(modId));
            },
        getTargetEventRecSize:
            function() {
                return (Log.EventRec.$sizeof() *
                        Program.build.target.bitsPerChar / 8);
            }
    };

    /* Load the Java classes in the xdc.rta package. */
    xdc.loadPackage('xdc.rta');

    /* Create the Java wrapper for the IEventMetaData object. */
    var jEventData = new Packages.xdc.rta.IEventMetaData(eventData);

    /* Required target information */
    var bitsPerChar = Program.build.target.bitsPerChar;
    var endianStr = Program.build.target.model.endian;
    var endianess = xdc.jre.xdc.rov.TargetType.strToEndianess(endianStr);

    /* Create an instance of the LoggerIdleDecoder */
    var decoder =
        new Packages.ti.uia.sysbios.LoggerIdleDecoder(endianess,
                bitsPerChar, jEventData);

    /*
     * Store off the LoggerIdle Decoder instance so we only have to create it
     * once.
     */
    LoggerIdle.$private.decoder = decoder;

    return (decoder);
}
