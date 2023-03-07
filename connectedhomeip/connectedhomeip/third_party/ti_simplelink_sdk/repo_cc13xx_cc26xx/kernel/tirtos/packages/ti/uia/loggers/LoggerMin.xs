/*
 * Copyright (c) 2013-2014, Texas Instruments Incorporated
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
 *  ======== LoggerMin.xs ========
 */

var LoggerMin = null;

var BIOS = null;
var Timestamp = null;
var Types = null;
var ILoggerSnapshot = null;
var IUIATransfer = null;
var UIAMetaData = null;
var EventHdr = null;
var Idle = null;


/*
 *  ======== module$use ========
 */
function module$use()
{
    LoggerMin = this;

    BIOS = xdc.useModule('ti.sysbios.BIOS');

    Timestamp = xdc.useModule('xdc.runtime.Timestamp');
    Types = xdc.useModule('xdc.runtime.Types');
    ILoggerSnapshot = xdc.useModule('ti.uia.runtime.ILoggerSnapshot');
    IUIATransfer = xdc.useModule('ti.uia.runtime.IUIATransfer');
    UIAMetaData = xdc.useModule('ti.uia.runtime.UIAMetaData');
    EventHdr = xdc.useModule('ti.uia.runtime.EventHdr');
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    var Program = xdc.module('xdc.cfg.Program');
    var sizeOfInt32 = Program.build.target.stdTypes.t_Int32.size;
    var write8Size = 10 * 4; // (EventHdr + mod/evt Ids + 8 args) * 4 bytes

    if (params.bufSize % Program.build.target.stdTypes.t_Int32.size != 0) {
        LoggerMin.$logError("The bufSize parameter must be a multiple of a word", this);
    }

    /*
     *  The buffer pointers are initialized to null and set during start up
     *  when the configured prime function is called.
     */
    mod.enabled = true;
    mod.empty = true;
//    mod.start = null;
//    mod.write = null;
//    mod.end = null;
    mod.droppedEvents = 0;
    mod.eventSequenceNum = 0;
    mod.pktSequenceNum = 0;
    mod.numBytesInPrevEvent = 0;

    /*
     *  Set the eventType based on timestamp used, so we
     *  don't have to do the check at runtime. Adjust the
     *  write8 size also, if necessary.
     */
    if (params.timestampSize == LoggerMin.TimestampSize_64b) {
        mod.eventType = EventHdr.HdrType_EventWithPrevLenWith64bTimestamp;
        write8Size += 8;
    }
    else if (params.timestampSize == LoggerMin.TimestampSize_32b) {
        mod.eventType = EventHdr.HdrType_EventWithPrevLenWith32bTimestamp;
        write8Size += 4;
    }
    else {
        mod.eventType = EventHdr.HdrType_EventWithPrevLenWithNoTimestamp;
    }

    var transportType = "JTAGSTOPMODE";
    var uploadMode = UIAMetaData.UploadMode_JTAGSTOPMODE;
    var customTransportType = "";

    UIAMetaData.setTransportFields(false,
                                   uploadMode,
                                   transportType,
                                   "UIAPACKET", false,
                                   customTransportType);

    /*
     *  The following will place a memory buffer into section name. This
     *  buffer is the memory that this instance will manage.
     */
    var adjustedSize = params.bufSize;

    /*
     *  The following will place a memory buffer into section name. This
     *  buffer is the memory that this instance will manage.
     */
    if ((Program.cpu.catalogName.match(/6000/)) && (LoggerMin.numCores > 1)) {
        adjustedSize = adjustedSize * LoggerMin.numCores;
    }

    mod.packetBuffer.length = adjustedSize;
    var Memory = xdc.module('xdc.runtime.Memory');
    Memory.staticPlace(mod.packetBuffer, params.memoryAlignmentInMAUs, params.bufSection);

    /* Allow UIA packet header at beginning of buffer */
    mod.write = mod.packetBuffer.$addrof(0 + sizeOfInt32);
    mod.start = mod.packetBuffer.$addrof(0);
    mod.end = mod.packetBuffer.$addrof(params.bufSize - sizeOfInt32);
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, params)
{
}

/*
 *  ======== getLoggerInstanceId ========
 *  Returns the 1 as the instance ID.  This can then
 *  be used to look up the instance using mod.$instances[instanceId].
 *  It is also used by the UIA Metadata to identify which instance of a
 *  logger is being used for a specific purpose within UIA.
 */
function getLoggerInstanceId(inst){
    return (1);
}
/*
 *  ======== viewInitModule ========
 *  Initializes the Module view in ROV.
 */
function viewInitModule(view, mod)
{
    var LoggerMin  = xdc.useModule("ti.uia.loggers.LoggerMin");
    var modConfig = Program.getModuleConfig(LoggerMin.$name);

    /* Populate ROV module view */
    view.isEnabled = mod.enabled;

    view.timestampSize = modConfig.timestampSize;
    view.bufferSize = modConfig.bufSize;
}

/*
 *  ======== viewInitRecords ========
 *  Initializes the Records view in ROV.
 */
function viewInitRecords(view)
{
    xdc.loadPackage('ti.uia.loggers');
    xdc.loadPackage('ti.uia.runtime');

    // Causes exception from ROV server:
    // java.lang.ClassCastException:
    // org.mozilla.javascript.DefaultErrorReporter
    // cannot be cast to org.mozilla.javascript.tools.ToolErrorReporter.
    //xdc.loadPackage('ti.uia.sysbios');

    xdc.loadPackage('xdc.runtime');
    xdc.loadPackage('xdc.rov');
    var Model = xdc.useModule('xdc.rov.Model');
    var Log = xdc.useModule('xdc.runtime.Log');
    var Program = xdc.useModule('xdc.rov.Program');
    var LoggerMin = xdc.useModule('ti.uia.loggers.LoggerMin');

    /*
     * Retrieve the module's state.
     * If this throws an exception, just allow it to propogate up.
     */
    var rawView = Program.scanRawView('ti.uia.loggers.LoggerMin');

    /* Get the memory reader */
    var memReader = Model.getMemoryImageInst();

    /* Initialize the LoggerMin decoder */
    var decoder;
    try {
        decoder = LoggerMin.initDecoder();
    }
    catch (e) {
        print("Caught exception while initiailizing decoder: " + e);

        /* Create a new view element to display the exception. */
        var evtView = Program.newViewStruct('ti.uia.loggers.LoggerMin',
                'Records');

        /* Dispaly the exception in the 'sequence' field. */
        Program.displayError(evtView, 'sequence',
                "Caught exception while initiailizing decoder: " + e);

        /* Add the element and return. */
        view.elements = [evtView];

        return;
    }

    /* Read the buffer. */
    var buffer;
    try {
        buffer = decoder.readTargetBuffer(memReader,
                rawView.modState.packetBuffer,
                rawView.modState.write);
    }
    catch (e) {
        print("Caught exception while reading buffer: " + e);

        /* Create a new view element to display the exception. */
        var evtView = Program.newViewStruct('ti.uia.loggers.LoggerMin',
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

    /* Process records from readPtrOffset to end of buffer */
    var offset = 0;
    var numRecords;

    try {
        numRecords = decoder.getNumRecords(buffer, buffer.length);
    }
    catch (e) {
        print("Caught exception while getting number of records: " + e);

        /* Create a new view element to display the exception. */
        var evtView = Program.newViewStruct('ti.uia.loggers.LoggerMin',
                                            'Records');

        /* Dispaly the exception in the 'sequence' field. */
        Program.displayError(evtView, 'sequence',
                "Caught exception while getting number of records: " + e);

        /* Add the element and return. */
        view.elements = [evtView];

        return;
    }

    var firstSeqNum = rawView.modState.eventSequenceNum - numRecords - 1;
    var recNum = 0;

    while (offset < buffer.length) {
        recNum++;

        /* Create a new view element. */
        var evtView = Program.newViewStruct('ti.uia.loggers.LoggerMin',
	        'Records');

        /* Decode the next event. */
        var evt;

        try {
            evt = decoder.decodeEvent(buffer, offset, buffer.length);

            /* Fill in the element. */
            evtView.sequence = firstSeqNum + recNum;
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
            print("Next offset: " + utils.toHex(offset));

	    numRecords++;
        }
        catch (e) {
            print("Caught exception decoding event: " + e);
            print("offset = " + utils.toHex(offset) + ", buffer length = " +
                  utils.toHex(buffer.length));

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
    var LoggerMin = xdc.module('ti.uia.loggers.LoggerMin');

    /* Create a new StopModeData structure to populate. */
    var data = new LoggerMin.RtaData();
    data.instanceId = 1;
    return (data);
}

/*
 *  ======== initDecoder ========
 *  Creates an instance of the ti.uia.loggers.LoggerMinDecoder class.
 *  The LoggerMin 'Records' ROV view uses the LoggerMinDecoder class
 *  to decode the raw bytes into xdc.rta.HostEvent objects. This decoder is
 *  shared between this ROV view and UIA.
 */
function initDecoder()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var LoggerMin = xdc.useModule('ti.uia.loggers.LoggerMin');
    var Model = xdc.useModule('xdc.rov.Model');
    var Log = xdc.useModule('xdc.runtime.Log');

    /* Check if the decoder has already been created. */
    if (LoggerMin.$private.decoder != undefined) {
        return (LoggerMin.$private.decoder);
    }

    /* Get the IOFReader instance */
    var ofReader = Model.getIOFReaderInst();

    /*
     * In order to decode the events in a LoggerMin, the
     * LoggerMinDecoder class requires an object which implements the
     * IEventMetaData interface to provide log event information such as the
     * mapping of an event id to the event's message.
     *
     * For ROV, we create a JavaScript object which implements all of the APIs
     * in the IEventMetaData class, then place a Java wrapper on this so that
     * it can be called by the LoggerMinDecoder class.
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

    /* Create an instance of the LoggerMinDecoder */
    var decoder =
        new Packages.ti.uia.loggers.LoggerMinDecoder(endianess,
                bitsPerChar, jEventData);

    /*
     * Store off the LoggerMin Decoder instance so we only have to create it
     * once.
     */
    LoggerMin.$private.decoder = decoder;

    return (decoder);
}
