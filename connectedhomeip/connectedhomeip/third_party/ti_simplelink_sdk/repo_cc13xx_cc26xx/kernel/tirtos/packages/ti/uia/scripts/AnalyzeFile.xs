/*
 *  ======== AnalyzeFile.xs ========
 *  Script for analyzing UIA performance based on a binary data file.
 *
 *  There are a number of variables at the top of the script which should
 *  be set before using it.
 */

/* Load all required packages. */
//xdc.loadPackage('ti.targets.omf.cof');
xdc.loadPackage('ti.targets.omf.elf');
xdc.loadPackage('ti.uia.loggers');
xdc.loadPackage('ti.uia.sysbios');
xdc.loadPackage('ti.uia.runtime');
xdc.loadPackage('ti.uia.services');
xdc.loadPackage('ti.uia.scripts');
xdc.loadPackage('ti.uia.scripts.ndk');

/* The .out file loaded on the target. */
//var executable = "../packages/ti/uia/examples/evm6472/ti_platforms_evm6472_core0/debug/message_single.x64P";
//var rtaXml = "../packages/ti/uia/examples/evm6472/package/cfg/ti_platforms_evm6472_core0/debug/message_single_x64P.rta.xml";

var executable = "Z:/cmcc/UIA_Examples/evm6472/stairstepOneCore/stairstepOneCore.x64P";
var rtaXml = "Z:/cmcc/UIA_Examples/evm6472/stairstepOneCore/stairstepOneCore_x64P.rta.xml";
var inputFile = "C:/temp/UIAReceivedData.bin";

/* Skip over the first 'skipCnt' packets received. */
var skipCnt = 0;

/* Set the format of the decoded records when printed. */
var printLevel = Packages.ti.uia.scripts.PacketProcessor.PrintLevel.NONE;
//var printLevel = Packages.ti.uia.scripts.PacketProcessor.PrintLevel.DEBUG;

var meta;

var cores = new Array();
var numCores = 8;

/* Track how many events come from each module. */
var modules = {};
var modArray;

/* Track how many times we receive each event. */
var events = {};
var evtArray;

var tskLoadEvtId;
var tskLoadEvts = new Array();
var curPacket = 0;

run();

function run()
{
    print("AnalyzeFile started.");

    /* Create a PacketProcessor to manage decoding. */
    var processor =
        new Packages.ti.uia.scripts.FileProcessor(executable, rtaXml,
                                                  inputFile, null);

    meta = processor.getMetaData();

    tskLoadEvtId = meta.lookupEventId("ti.sysbios.utils.Load.LS_taskLoad");
    print("Task load event id = " + tskLoadEvtId);

    /* Initialize the statistics structure for each core. */
    for (var i = 0; i < numCores; i++) {
        cores[i] = {
            numPcktsRcvd: 0,

            firstSeqNum: -1,
            lastSeqNum: -1,

            numBreaks: 0,

            largestBreak: 0,

            loggers: new Array()
        }

        /* TODO - We should get logger count from RTA XML file. */
        for (var j = 0; j < 2; j++) {
            cores[i].loggers[j] = {
                numRecsRcvd: 0,

                firstSeqNum: -1,
                lastSeqNum: -1,

                firstTime: -1,
                lastTime: -1,

                numBreaks: 0,

                largestBreak: 0
            }
        }
    }


    /* Read from the file until we reach the end. */
    print("Skipping the first " + skipCnt + " packets...");
    while (processNextPacket(processor)) {
        /*
         * Print the number of the current packet being analyzed. This can be
         * compared to the total number of packets received when the binary
         * data file was captured to get a sense of the scripts progress.
         */
        print(curPacket++);
    }

    /* Close the file streams. */
    processor.close();

    /* Handle data that needs to be sorted before reporting. */
    sortData();

    printStatistics();

    printModsAndEvents();

    writeOutStatistics();
}

/*
 *  ======== processNextPacket ========
 */
function processNextPacket(processor)
{
    /* Skip over the first 'skipCnt' packets. */
    if (skipCnt-- > 0) {
        // TODO - Discard the packet
        return;
    }

    /* Read the next packet header and print it. */
    var header = processor.readNextEventHeader(false);

    /* If header is null, we've reached the end of the file. */
    if (header == null) {
        /* Return false to signal that we should stop reading. */
        return (false);
    }

    /* Update the core's statistics based on this packet. */
    recordCore(header);

    /* Read, decode, and print the next packet of records. */
    var events = processor.readPacketBody(header, printLevel);

    /* Process each record. */
    for each (var evt in events) {
        /* Check for out of order record. */
        if (evt.sequenceNum < cores[header.getSenderAddr()].loggers[header.getInstanceId()].lastSeqNum) {
            print("Received out of order record!!!");
            print("Current: " + evt.sequenceNum + " Previous: " + cores[header.getSenderAddr()].loggers[header.getInstanceId()].lastSeqNum);
            printPacket(processor, header, events);
        }

        /* Update the logger's statistics based on this event. */
        recordEvent(header, evt);
    }

    /* Return true to signal to keep reading. */
    return (true);
}

/*
 *  ======== recordCore ========
 */
function recordCore(header)
{
    var core = cores[header.getSenderAddr()];

    /* Increment number of packets received. */
    core.numPcktsRcvd++;

    /* Record the first packet sequence number received from this core. */
    if (core.firstSeqNum == -1) {
        core.firstSeqNum = header.getPacketSeqNum();
        core.lastSeqNum = header.getPacketSeqNum();
        return;
    }

    /* Check for out of order packets. */
    if (header.getPacketSeqNum() < core.lastSeqNum) {
        print("Received out of order packet!!!");
    }

    /*
     * Check if this packet is consecutive with the last one.
     * TODO - This count is bogus if we get out of order packets.
     */
    if (header.getPacketSeqNum() != (core.lastSeqNum + 1)) {
        core.numBreaks++;

        /* Track the largest gap in packets we've seen. */
        var breakSize = header.getPacketSeqNum() - core.lastSeqNum - 1;

        if (breakSize > core.largestBreak) {
            core.largestBreak = breakSize;
        }
    }

    /* Update the last sequence number received from this core. */
    core.lastSeqNum = header.getPacketSeqNum();
}

/*
 *  ======== recordEvent ========
 */
function recordEvent(header, event)
{
    /* Track how many events come from each module. */
    if (modules[event.moduleId] == undefined) {
        modules[event.moduleId] = 1;
    }
    else {
        modules[event.moduleId]++;
    }

    /* Track how many records there are of each event. */
    if (events[event.eventId] == undefined) {
        events[event.eventId] = 1;
    }
    else {
        events[event.eventId]++;
    }

    /* Track the thread load events. */
    /*
    if (event.eventId = tskLoadEvtId) {
        tskLoadEvts[tskLoadEvts.length] = event;
    }
    */

    var logger = cores[header.getSenderAddr()].loggers[header.getInstanceId()];

    /* Update total number of records received from this logger. */
    logger.numRecsRcvd++;

    /* Record the first event received from this logger. */
    if (logger.firstSeqNum == -1) {
        logger.firstSeqNum = event.sequenceNum;
        logger.lastSeqNum = event.sequenceNum;
        return;
    }

    /* Check for out of order records. */
    if (event.sequenceNum < logger.lastSeqNum) {
        print("Received out of order record!!!");
    }

    /* Check for breaks in the events. */
    if (event.sequenceNum != (logger.lastSeqNum + 1)) {
        logger.numBreaks++;

        /* Track the largest gap in records we've seen. */
        var breakSize = event.sequenceNum - logger.lastSeqNum - 1;

        if (breakSize > logger.largestBreak) {
            logger.largestBreak = breakSize;
        }
    }

    /* Update the last sequence number received from this logger. */
    logger.lastSeqNum = event.sequenceNum;

    if (logger.firstTime == -1) {
        logger.firstTime = event.timestamp;
        logger.lastTime = event.timestamp;
        return;
    }

    /* Ensure the timestamp never drops in value. */
    if (event.timestamp < logger.lastTime) {
        print("\n************TIMESTAMP DROP!!!************\n");
        print("core: " + header.getSenderAddr() + " logger: " + header.getInstanceId() +
              " old = " + logger.lastTime + " new = " + event.timestamp);
    }

    logger.lastTime = event.timestamp;
}

/*
 *  ======== printPacket ========
 */
function printPacket(processor, header, events)
{
    /* Print out the header and its values. */
    header.printHeader();

    /* Retrieve the the LoggerCircBufDecoder. */
    var loggerDec = processor.getDecoder();

    /* Print out each of the events. */
    for each (var evt in events) {
        print(loggerDec.getReadableEvent(evt));
    }
}

/*
 *  ======== sortData ========
 *
 */
function sortData()
{
    /* Sort the modules by the number of events from that module. */
    modArray = new Array();

    /* Copy all of the module statistics into an array. */
    for (var p in modules) {
        modArray[modArray.length] = {
            id: p,
            count: modules[p]
        };
    }

    /* Perform the sort. */
    modArray.sort(sortFunc);

    /* Sort the events by the number we've received. */
    evtArray = new Array();

    /* Copy all of the event statistics into an array. */
    for (var p in events) {
        evtArray[evtArray.length] = {
            id: p,
            count: events[p]
        };
    }

    /* Perform the sort. */
    evtArray.sort(sortFunc);
}

/*
 *  ======== sortFunc ========
 *  Function for sorting events and modules by the number of times we've
 *  received them.
 *
 *  Sort them into descending order.
 */
function sortFunc(a, b) {
    return (b.count - a.count);
}

/*
 *  ======== printStatistics ========
 */
function printStatistics()
{
    /* System wide statistics. */
    var totalPacketsSent = 0;
    var totalPacketsDropped = 0;

    var totalRecordsLogged = 0;
    var totalRecordsDropped = 0;

    /* For each core */
    for (var i = 0; i < cores.length; i++) {
        print("\n=========================");
        print("======== Core " + i + " =========");
        print("=========================");

        var core = cores[i];

        print("Packets received: " + core.numPcktsRcvd + "    from " +
              core.firstSeqNum + " to " + core.lastSeqNum);

        if (core.numPcktsRcvd == 0) {
            continue;
        }

        core.totalSent = core.lastSeqNum - core.firstSeqNum + 1;
        core.totalDropped = core.totalSent - core.numPcktsRcvd;

        totalPacketsSent += core.totalSent;
        totalPacketsDropped += core.totalDropped;

        print("Total dropped packets: " + core.totalDropped);

        core.rcvdPercent = ((core.numPcktsRcvd / core.totalSent) * 100).toFixed(2);

        print("Received " + core.rcvdPercent + "% of sent packets.");

        print("There were " + core.numBreaks + " gaps in packets.");

        print("The largest gap was " + core.largestBreak + " packets.");

        /* For each logger */
        for (var j = 0; j < core.loggers.length; j++) {
            print("\n    ======================");
            print("    ====== Logger " + j + " ======");
            print("    ======================");

            var logger = core.loggers[j];

            print("    Records received: " + logger.numRecsRcvd + "    from " +
                  logger.firstSeqNum + " to " + logger.lastSeqNum);

            logger.totalRecs = logger.lastSeqNum - logger.firstSeqNum + 1;
            logger.totalDropped = logger.totalRecs - logger.numRecsRcvd;

            totalRecordsLogged += logger.totalRecs;
            totalRecordsDropped += logger.totalDropped;

            print("    Total dropped records: " + logger.totalDropped);

            logger.rcvdPercent = ((logger.numRecsRcvd / logger.totalRecs) * 100).toFixed(2);

            print("    Received " + logger.rcvdPercent + "% of logged records.");

            print("    There were " + logger.numBreaks + " gaps in records.");
            print("    The largest gap was " + logger.largestBreak + " records.");

            logger.fragPercent = (logger.numBreaks / (logger.totalRecs / 2) * 100).toFixed(2);

            print("    Record fragmentation: " + logger.fragPercent + "%");
        }
    }

    /* Print system wide statistics. */
    print("\n=========================");
    print("======== System =========");
    print("=========================");
    print("Total packets sent: " + totalPacketsSent);
    print("Total packets dropped: " + totalPacketsDropped);
    print("Percentage packets rcvd: " + ((1 - (totalPacketsDropped / totalPacketsSent)) * 100).toFixed(2) + "%");
    print("\nTotal records logged: " + totalRecordsLogged);
    print("Total records dropped: " + totalRecordsDropped);
    print("Percentage records rcvd: " + ((1 - (totalRecordsDropped / totalRecordsLogged)) * 100).toFixed(2) + "%");
}

function printModsAndEvents()
{
    print("\nDistribution of records by module:");
    for each (var mod in modArray) {
        var modName = meta.lookupModuleName(mod.id);

        if (modName == null) {
            modName = String(mod.id);
        }

        print("  " + strPad(modName, 35) +
              mod.count);
    }

    print("\nDistribution of records by event:");
    for each (var evt in evtArray) {
        var evtName = meta.lookupEventName(evt.id);

        if (evtName == null) {
            evtName = String(evt.id);
        }

        print("  " + strPad(evtName, 40) +
              evt.count);
    }
}

/*
 *  ======== strPad ========
 *  This function helps to format the results file by padding
 *  the given string with whitespace to be 'pad' characters
 *  wide.
 */
function strPad(str, pad)
{
    for (i = String(str).length; i < pad; i++) {
        str += " ";
    }

    return(str);
}

/*
 *  ======== writeOutStatistics ========
 */
function writeOutStatistics()
{
    var file = new xdc.jre.java.io.File("C:/temp/UIAStatistics.csv");
    var fw = new xdc.jre.java.io.FileWriter(file);

    /* Table of packets received and dropped */
    fw.write("Total Packets Received and Dropped\n");
    fw.write("Core,Total Sent,Total Rcvd,Total Dropped\n");
    for (var i = 0; i < cores.length; i++) {
        var core = cores[i];
        fw.write(i + "," + core.totalSent + "," + core.numPcktsRcvd + "," +
                 core.totalDropped + "\n");
    }

    /* Table of percentage of packets received per core. */
    fw.write("\nPercentage of Packets Received\n");
    fw.write("Core,% Rcvd\n");
    for (var i = 0; i < cores.length; i++) {
        var core = cores[i];
        fw.write(i + "," + core.rcvdPercent + "\n");
    }

    /* Table of records received and dropped, one table per core. */
    for (var i = 0; i < cores.length; i++) {
        var core = cores[i];
        fw.write("\nCore " + i + " Records Received\n");
        fw.write("Logger,Total Records,Total Received,Total Dropped\n");

        /* For each logger */
        for (var j = 0; j < core.loggers.length; j++) {
            logger = core.loggers[j];
            fw.write(j + "," + logger.totalRecs + "," + logger.numRecsRcvd +
                     "," + logger.totalDropped + "\n");
        }
    }

    /* Table of percentage of records received per core. */
    fw.write("\nPercentage of Records Received\n");
    fw.write("Core,% Rcvd\n");

    for (var i = 0; i < cores.length; i++) {
        var core = cores[i];

        fw.write(i + "," + cores[i].loggers[1].rcvdPercent + "\n");
    }

    fw.write("\nDistribution of Records by Module\n");
    for each (var mod in modArray) {
        fw.write(meta.lookupModuleName(mod.id) + "," +
                 mod.count + "\n");
    }

    fw.write("\nDistribution of Records by Event\n");
    for each (var evt in evtArray) {
        fw.write(meta.lookupEventName(evt.id) + "," +
                 evt.count + "\n");
    }

    fw.close();
}
