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
 *
 */

/*
 *  ======== LoggerStopMode.xs ========
 */

var LoggerStopMode = null;
var Program = null;

var BIOS = null;

var Timestamp = null;
var Types = null;
var ILoggerSnapshot = null;
var IUIATransfer = null;
var UIAMetaData = null;
var QueueDescriptor = null;
var EventHdr = null;
var Memory = null;

var numInstances = 0;
var isEnableDebugPrintf = false;

/*
 *  ======== module$use ========
 */
function module$use()
{
    LoggerStopMode = this;
    Program = xdc.module('xdc.cfg.Program');

    BIOS = xdc.useModule('ti.sysbios.BIOS');

    Timestamp = xdc.useModule('xdc.runtime.Timestamp');
    Types = xdc.useModule('xdc.runtime.Types');
    ILoggerSnapshot = xdc.useModule('ti.uia.runtime.ILoggerSnapshot');
    IUIATransfer = xdc.useModule('ti.uia.runtime.IUIATransfer');
    UIAMetaData = xdc.useModule('ti.uia.runtime.UIAMetaData');
    QueueDescriptor = xdc.useModule('ti.uia.runtime.QueueDescriptor');
    EventHdr = xdc.useModule('ti.uia.runtime.EventHdr');

    Memory = xdc.module('xdc.runtime.Memory');

    if (LoggerStopMode.statusLogger != null) {
        LoggerStopMode.$logWarning("LoggerStopMode no longer supports a " +
                "statusLogger configuration parameter.  This " +
                "value will be ignored.", LoggerStopMode,
                "statusLogger");
    }

    if (LoggerStopMode.overflowLogger != null) {
        LoggerStopMode.$logWarning("LoggerStopMode no longer supports an " +
                "overflowLogger configuration parameter.  This " +
                "value will be ignored.", LoggerStopMode,
                "overflowLogger");
    }

    if (LoggerStopMode.moduleToRouteToStatusLogger != null) {
        LoggerStopMode.$logWarning("LoggerStopMode no longer supports " +
                "moduleToRouteToStatusLogger configuration.  This " +
                "value will be ignored.", LoggerStopMode,
                "overflowLogger");
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    var customTransportType = "";

    if (!Program.cpu.catalogName.match(/6000/) &&
            (LoggerStopMode.numCores > 1)) {
        LoggerStopMode.$logError("numCores > 1 is only supported on C6000 targets", this);
    }

    if (LoggerStopMode.numCores < 1) {
        LoggerStopMode.$logError("numCores must be > 0", this);
    }

    var transportType = "JTAGSTOPMODE";
    uploadMode = UIAMetaData.UploadMode_JTAGSTOPMODE;

    UIAMetaData.setTransportFields(false,
                                   uploadMode,
                                   transportType,
                                   "UIAPACKET", false,
                                   customTransportType);

    /* Assign the default filtering levels based on the configuration. */
    mod.level1 = params.level1Mask;
    mod.level2 = params.level2Mask;
    mod.level3 = params.level3Mask;

    mod.enabled = true;
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, params)
{
    var bufSize;
    var sizeOfUArg = Program.build.target.stdTypes.t_Ptr.size;

    numInstances += 1;

    if (params.transferBufSize != 0) {
        LoggerStopMode.$logWarning("Config param transferBufSize has been replaced " +
                "bufSize.  This value will be used for bufSize.", LoggerStopMode,
                "transferBufSize");
        bufSize = params.transferBufSize;
    }
    else {
        bufSize = params.bufSize;
    }

    /*
     *  Validate that the buffer size (in MAU) is valid. 16 words is the
     *  minimum size, equal to the maximum size of an 8 word event(12 words)
     *  plus the four word UIA header.  The bufSize must also be a multiple
     *  of words.
     *
     *  The buffer size must also be  >= (maxEventSize + 16 words).
     *  (For testing with small buffer sizes, set maxEventSize = 0 to skip
     *  this test).
     */
    if (params.maxEventSize != 0) {
        if (bufSize < (params.maxEventSize +
                    (16 * Program.build.target.stdTypes.t_Int32.size))) {
            LoggerStopMode.$logError("The bufSize parameter must be maxEventSize + " +
                    "16 words or larger. If you are not using LogSnapshot, " +
                    "you can set maxEventSize to 0.", LoggerStopMode, "bufSize");
        }
    }

    if (bufSize % sizeOfUArg != 0) {
        LoggerStopMode.$logError("The bufSize parameter must be a multiple " +
                "of the size of a pointer", LoggerStopMode, "bufSize");
    }

    obj.bufSize = bufSize;
    obj.enabled = true;
    obj.maxEventSizeUArgs = params.maxEventSize / sizeOfUArg;
    obj.maxEventSize = params.maxEventSize;

    /*
     *  The buffer pointers are initialized to null and set during start up
     *  when the configured prime function is called.
     */
    obj.buffer = null;
    obj.write = null;
    obj.end = null;
    obj.droppedEvents = 0;
    obj.eventSequenceNum = 0;
    obj.pktSequenceNum = 0;
    obj.numBytesInPrevEvent = 0;

    /*
     *  Instance id of 0 is reserved for dynamically created objects,
     *  so start with 1.
     */
    obj.instanceId = getInstanceId(obj);

    obj.packetArray.length = obj.bufSize * LoggerStopMode.numCores;

    /*
     *  The following will place a memory buffer into section name. This
     *  buffer is the memory that this instance will manage.
     */
    Memory.staticPlace(obj.packetArray, LoggerStopMode.cacheLineSizeInMAUs,
            params.bufSection);

    if (isEnableDebugPrintf) {
        print("UIA LoggerStopMode.xs: bufSize = 0x" +
                Number(obj.bufSize).toString(16) +
                ", numCores = " + LoggerStopMode.numCores);
        print("UIA LoggerStopMode.xs: instance$static$init: qdHdr adrs = 0x" +
                Number(obj.hdr.$addrof(0)).toString(16));
    }

    /*
     *  Allocate space for this instance's queue descriptor in the same
     *  memory section as used for the buffer
     */
    obj.hdr.length = QueueDescriptor.Header.$sizeof() * LoggerStopMode.numCores;

    Memory.staticPlace(obj.hdr, LoggerStopMode.cacheLineSizeInMAUs,
            params.bufSection);
}

/*
 *  ======== viewInitModule ========
 *  Initializes the Module view in ROV.
 */
function viewInitModule(view, mod)
{
    /*
     *  Work-around for LoggerStopMode module state being optimized
     *  out.  When that happens, viewInitModule() is called with null
     *  value for mod.
     */
    if (mod == null) {
        view.isEnabled = true;
        view.isTimestampEnabled = true;
        return;
    }

    var LoggerStopMode  = xdc.useModule("ti.uia.loggers.LoggerStopMode");

    /* Populate ROV module view */
    view.isEnabled = mod.enabled;
    view.isTimestampEnabled = mod.isTimestampEnabled;
}

/*
 *  ======== viewInitInstances ========
 *  Initializes the 'Basic' ROV instance view.
 */
function viewInitInstances(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var LoggerStopMode = xdc.useModule('ti.uia.loggers.LoggerStopMode');

    /* Retrieve the LoggerStopMode instance's name */
    view.label = Program.getShortName(obj.$label);

    /*
     * Determine the sequence number of the last record, convert it to a
     * sequential number.
     */
    view.enabled = obj.enabled;
    view.bufferSize = obj.bufSize;
    view.maxEventSize = obj.maxEventSize;
}

/*
 *  ======== viewInitRecords ========
 *  Initializes the Records view in ROV.
 */
function viewInitRecords(view, obj)
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
    var LoggerStopMode = xdc.useModule('ti.uia.loggers.LoggerStopMode');


    /* Populate the Program.InstDataView structure */
    view.label = Program.getShortName(obj.$label);
    if (view.label.equals("")) {
        view.label = String(obj.$addr);
    }

    /* Get the memory reader */
    var memReader = Model.getMemoryImageInst();

    /* Initialize the LoggerStopModeDecoder */
    var decoder;
    try {
        decoder = LoggerStopMode.initDecoder();
    }
    catch (e) {
        print("Caught exception while initiailizing decoder: " + e);

        /* Create a new view element to display the exception. */
        var evtView = Program.newViewStruct('ti.uia.loggers.LoggerStopMode',
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
                obj.buffer,
                obj.write);
    }
    catch (e) {
        print("Caught exception while reading buffer: " + e);

        /* Create a new view element to display the exception. */
        var evtView = Program.newViewStruct('ti.uia.loggers.LoggerStopMode',
                                            'Records');

        /* Dispaly the exception in the 'sequence' field. */
        Program.displayError(evtView, 'sequence',
                             "Caught exception while reading buffer: " + e);

        /* Add the element and return. */
        view.elements = [evtView];

        return;
    }

    if ((buffer == null) || (buffer.length == 0)) {
        /* No records to display */
        //print("***** No records to display *****");
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
        var evtView = Program.newViewStruct('ti.uia.loggers.LoggerStopMode',
                                            'Records');

        /* Dispaly the exception in the 'sequence' field. */
        Program.displayError(evtView, 'sequence',
                "Caught exception while getting number of records: " + e);

        /* Add the element and return. */
        view.elements = [evtView];

        return;
    }

    //print("******* Num records = " + numRecords);
    //print("******* obj.eventSequenceNum = " + obj.eventSequenceNum);
    var firstSeqNum = obj.eventSequenceNum - numRecords - 1;
    var recNum = 0;

    var skipEvent;
    while (offset < buffer.length) {
        recNum++;

        /* Create a new view element. */
        var evtView = Program.newViewStruct('ti.uia.loggers.LoggerStopMode',
                'Records');

        /* Decode the next event. */
        var evt;

        try {
            evt = decoder.decodeEvent(buffer, offset, buffer.length);

            if (evt != null) {
                skipEvent = false;

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
            }
            else {
                /* May be a snapshot event which we can't display in ROV */
                skipEvent = true;
            }

            /* Move the offset to the next record. */
            offset += decoder.getEventLength(buffer, offset);
            //print("Next offset: " + utils.toHex(offset));

            numRecords++;
        }
        catch (e) {
            print("Caught exception decoding event: " + e);
            print("offset = " + utils.toHex(offset) + ", buffer length = " +
                    utils.toHex(buffer.length));

            var msg = String(e);

            /* strip off any useless Java exception class name prefix */
            var k = String(e).lastIndexOf("Exception: ");
            if (k >= 0) {
                msg = msg.substr(k + 11); /* 11 = len of "Exception: " */
            }

            /* Display the exception in the 'sequence' column. */
            Program.displayError(evtView, 'sequence',
                    "Caught exception while decoding event: " + msg
                    + " The logger buffer (" + utils.toHex(Number(obj.buffer))
                    + ") appears corrupted");

            /* Add the element to the view and return. */
            eventViews[eventViews.length] = evtView;

            view.elements = eventViews;

            return;
        }

        if (!skipEvent) {
            /* Add the element to the view. */
            eventViews[eventViews.length] = evtView;
        }
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
    return (getInstanceId(inst.$object));
}

function getInstanceId(obj)
{
    var hashIndex = obj.$name.lastIndexOf('#');

    if (hashIndex > 0) {
        var instId = obj.$name.substring(hashIndex + 1);
        return (Number(instId) + 1);
    }
    return (0);
}


/*
 *  ======== getMetaArgs ========
 *  Returns meta data to support stop-mode RTA.
 */
function getMetaArgs(inst, instNum)
{
    var LoggerStopMode = xdc.module('ti.uia.loggers.LoggerStopMode');

    /* Create a new RtaData structure to populate. */
    var data = new LoggerStopMode.RtaData();
    data.instanceId = instNum + 1;

    return (data);
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


function getNumInstances() {
   return (numInstances);
}

/*
 *  ======== initDecoder ========
 *  Creates an instance of the ti.uia.loggers.LoggerStopModeDecoder class.
 *  The LoggerStopMode 'Records' ROV view uses the LoggerStopModeDecoder class
 *  to decode the raw bytes into xdc.rta.HostEvent objects. This decoder is
 *  shared between this ROV view and UIA.
 */
function initDecoder()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var LoggerStopMode = xdc.useModule('ti.uia.loggers.LoggerStopMode');
    var Model = xdc.useModule('xdc.rov.Model');
    var Log = xdc.useModule('xdc.runtime.Log');

    /* Check if the decoder has already been created. */
    if (LoggerStopMode.$private.decoder != undefined) {
        return (LoggerStopMode.$private.decoder);
    }

    /* Get the IOFReader instance */
    var ofReader = Model.getIOFReaderInst();

    /*
     * In order to decode the events in a LoggerStopMode, the
     * LoggerStopModeDecoder class requires an object which implements the
     * IEventMetaData interface to provide log event information such as the
     * mapping of an event id to the event's message.
     *
     * For ROV, we create a JavaScript object which implements all of the APIs
     * in the IEventMetaData class, then place a Java wrapper on this so that
     * it can be called by the LoggerStopModeDecoder class.
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

    /* Create an instance of the LoggerStopModeDecoder */
    var decoder =
        new Packages.ti.uia.loggers.LoggerStopModeDecoder(endianess,
                bitsPerChar, jEventData);

    /*
     * Store off the LoggerStopMode Decoder instance so we only have to create it
     * once.
     */
    LoggerStopMode.$private.decoder = decoder;

    return (decoder);
}
