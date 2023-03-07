/*
 * Copyright (c) 2012-2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
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
 *  ======== LoggerStreamer.xs ========
 */

var LoggerStreamer  = null;
var Timestamp = null;
var Types = null;
var UIAMetaData = null;
var ILoggerSnapshot = null;
var IUIATransfer = null;
var UIAMetaData = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    LoggerStreamer = this;
    Timestamp = xdc.useModule('xdc.runtime.Timestamp');
    Types = xdc.useModule('xdc.runtime.Types');
    ILoggerSnapshot = xdc.useModule('ti.uia.runtime.ILoggerSnapshot');
    IUIATransfer = xdc.useModule('ti.uia.runtime.IUIATransfer');
    UIAMetaData = xdc.useModule('ti.uia.runtime.UIAMetaData');
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    var Program = xdc.module('xdc.cfg.Program');
    var sizeOfInt32 = Program.build.target.stdTypes.t_Int32.size;

    var setTransport = true;
    var LoggingSetup = xdc.module('ti.uia.sysbios.LoggingSetup');

    if (LoggingSetup.$used) {
        /*
         *  If LoggingSetup was used, don't configure the tansport
         *  in the UIA metadata.  LoggingSetup's logger will do this.
         */
        setTransport = false;
    }

    /*
     *  Validate that the buffer size (in MAU) is valid. 16 words is the minimum
     *  size, equal to the maximum size of an 8 word event(12 words) plus the four word
     *  UIA header.  The bufSize must also be a multiple of words.
     *
     *  The buffer size must also be  >= (maxEventSize + 16 words).
     *  (For testing with small buffer sizes, set maxEventSize = 0 to skip this test).
     */
    if (params.maxEventSize != 0) {
        if (params.bufSize <(params.maxEventSize + (16 * Program.build.target.stdTypes.t_Int32.size))) {
            this.$logError("The bufSize parameter must be maxEventSize + 16 words or larger", this);
        }
    }
    if (params.bufSize % Program.build.target.stdTypes.t_Int32.size != 0) {
        this.$logError("The bufSize parameter must be a multiple of a word", this);
    }

    /* Check for prime and exchance functions */
    if (params.primeFxn == null) {
        // Removed this warning as the prime may be done at
        // run-time.  See SDOCM00114209
        //this.$logWarning("The primeFxn parameter is not set, you must call prime yourself", this);
    }
    if (params.exchangeFxn == null) {
        this.$logError("The exchangeFxn parameter must be set", this);
    }

    /*
     *  The buffer pointers are initialized to null and set during start up
     *    when the configured prime function is called.
     */
    mod.enabled = true;
    mod.buffer = null;
    mod.write = null;
    mod.end = null;
    mod.droppedEvents = 0;
    var customTransportType = "";

    if (params.customTransportType != null){
        customTransportType = params.customTransportType;
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
            //TODO build error/warning?
            break;
    }

    if (setTransport) {
        UIAMetaData.setTransportFields(false,
                UIAMetaData.UploadMode_NONJTAGTRANSPORT,
                transportType,
                "UIAPACKET", false,
                customTransportType);
    }

    /* Assign the default filtering levels based on the configuration. */
    mod.level1 = params.level1Mask;
    mod.level2 = params.level2Mask;
    mod.level3 = params.level3Mask;

    if ((params.moduleToRouteToStatusLogger != null) &&
            (params.statusLogger != null)){
        var toRoute = xdc.module(params.moduleToRouteToStatusLogger);
        mod.moduleIdToRouteToStatusLogger = toRoute.Module__id;
    }
    else {
        mod.moduleIdToRouteToStatusLogger = 65535;
    }
    mod.maxEventSizeInBits32 = params.maxEventSize / sizeOfInt32;
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
    var LoggerStreamer  = xdc.useModule("ti.uia.sysbios.LoggerStreamer");
    var LoggerStreamerModConfig = Program.getModuleConfig(LoggerStreamer.$name);

    /* Populate ROV module view */
    view.isEnabled = mod.enabled;
    view.isTimestampEnabled = LoggerStreamerModConfig.isTimestampEnabled;
    view.bufferSize = LoggerStreamerModConfig.bufSize;
    view.primeFunc = LoggerStreamerModConfig.primeFxn;
    view.exchangeFunc = LoggerStreamerModConfig.exchangeFxn;
    view.transportType = LoggerStreamerModConfig.transportType;
    if (LoggerStreamerModConfig.transportType ==
            LoggerStreamer.TransportType_CUSTOM) {
        view.customTransport = LoggerStreamerModConfig.customTransport;
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
    var LoggerCircBuf = xdc.useModule('ti.uia.runtime.LoggerCircBuf');
    var uiaHeaderOffset = 128 / Program.build.target.bitsPerChar;

    /*
     * Retrieve the module's state.
     * If this throws an exception, just allow it to propogate up.
     */
    var rawView = Program.scanRawView('ti.uia.sysbios.LoggerStreamer');

    /* Get the memory reader */
    var memReader = Model.getMemoryImageInst();

    /* Initialize the LoggerCircBufDecoder */
    var decoder;
    try {
        decoder = LoggerCircBuf.initDecoder();
    }
    catch (e) {
        print("Caught exception while initiailizing decoder: " + e);
    }

    /* Read the buffer. */
    var buffer;
    if(rawView.modState.write != 0) {
        // Get log events from memory
        try {
            buffer = decoder.readTargetBuffer(memReader,
                    rawView.modState.buffer + uiaHeaderOffset,
                    rawView.modState.write
                    - (rawView.modState.buffer + uiaHeaderOffset),
                    rawView.modState.buffer + uiaHeaderOffset,
                    rawView.modState.write);
        }
        catch (e) {
            print("Caught exception while reading buffer: " + e);

            /* Create a new view element to display the exception. */
            var evtView = Program.newViewStruct('ti.uia.sysbios.LoggerStreamer',
                    'Records');

            /* Dispaly the exception in the 'sequence' field. */
            Program.displayError(evtView, 'sequence',
                    "Caught exception while reading buffer: " + e);

            /* Add the element and return. */
            view.elements = [evtView];

            return;
        }
    }
    else {
        // Read UIA header to get length
        var uiaHeader;
        try {
            uiaHeader = decoder.readTargetBuffer(memReader,
                    rawView.modState.buffer,
                    uiaHeaderOffset,
                    rawView.modState.buffer,
                    rawView.modState.buffer + uiaHeaderOffset);
        }
        catch (e) {
            print("Caught exception while reading UIA header: " + e);

            /* Create a new view element to display the exception. */
            var evtView = Program.newViewStruct('ti.uia.sysbios.LoggerStreamer',
                    'Records');

            /* Dispaly the exception in the 'sequence' field. */
            Program.displayError(evtView, 'sequence',
                    "Caught exception while reading UIA header: " + e);

            /* Add the element and return. */
            view.elements = [evtView];

            return;
        }

        /* Get log events from memory */
        try {
            buffer = decoder.readTargetBuffer(memReader,
                    rawView.modState.buffer,
                    uiaHeader[3]*(8/Program.build.target.bitsPerChar),
                    rawView.modState.buffer + uiaHeaderOffset,
                    rawView.modState.buffer + uiaHeader[3]*(8/Program.build.target.bitsPerChar));
        }
        catch (e) {
            print("Caught exception while reading buffer: " + e);

            /* Create a new view element to display the exception. */
            var evtView = Program.newViewStruct('ti.uia.sysbios.LoggerStreamer',
                    'Records');

            /* Dispaly the exception in the 'sequence' field. */
            Program.displayError(evtView, 'sequence',
                    "Caught exception while reading buffer: " + e);

            /* Add the element and return. */
            view.elements = [evtView];

            return;
        }
    }

    /* Decode the buffer and display the records. */
    var eventViews = new Array();
    var offset = 0;             /* Offset */

    /* process rest of events */
    while (offset < buffer.length) {
        /* Create a new view element. */
        var evtView =
                Program.newViewStruct('ti.uia.sysbios.LoggerStreamer', 'Records');

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
            print("offset: ", +offset);

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
 *  ======== getLoggerInstanceId ========
 *  Parses the string returned from the instance (e.g. Mod.Instance#1)
 *  Returns the trailing number +1 as the instance ID.  This can then
 *  be used to look up the instance using mod.$instances[instanceId].
 *  It is also used as a unique identifier in the Queue Descriptor header
 *  and UIA Metadata to identify which instance of a logger is being used
 *  for a specific purpose within UIA. Note that a LoggerInstanceId of 0
 *  is reserved for dynamically created instances of the logger - the
 *  target code can test to see if the logger's queue descriptor header
 *  has an ID field of 0, and scan through the list of queue descriptors
 *  to determine what the maximum logger ID that is in use is in order
 *  to initialize the instance ID to a unique value for that logger module.
 */
function getLoggerInstanceId(inst){
    return (1);
}

/*
 *  ======== getLoggerPriority ========
 *  returns the priority of this logger instance.
 */
function getLoggerPriority(inst){
    var IUIATransfer = xdc.module('ti.uia.runtime.IUIATransfer');
    return (IUIATransfer.Priority_STANDARD);
}

/*
 *  ======== setLoggerPriority ========
 *  sets the priority of this logger instance.
 */
function setLoggerPriority(inst, priority){
}
