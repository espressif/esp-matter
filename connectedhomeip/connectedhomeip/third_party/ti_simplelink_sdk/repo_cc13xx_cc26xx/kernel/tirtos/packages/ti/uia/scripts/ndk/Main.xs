/*
 *  ======== run ========
 */
function run(cmdr, args)
{
    print("ti.uia.scripts.ndk started.");

    /* Make sure they at least passed -p or -o */
    if ((!this.printToScreen) && this.outputFile.equals("")) {
        print("\nPass -p to print to the screen or specify an output file " +
              "with -o, or both.");
        return;
    }

    /* Verify the arguments. */
    if (!fileExists(this.executable)) {
        print("Executable " + this.executable + " does not exist.");
        return;
    }
    if (!this.rtaXml.equals("") && !fileExists(this.rtaXml)) {
        print("RTA XML file " + this.rtaXml + " does not exist.");
        return;
    }

    /* Load the classes in the RTA package for decoding the records. */
    xdc.loadPackage('xdc.rta');

    /* Load the .out file parser packages. */
    xdc.loadPackage('ti.targets.omf.cof');
    xdc.loadPackage('ti.targets.omf.elf');

    /* Create the MetaData object. */
    var meta = new Packages.xdc.rta.MetaData();

    /* Initialize the MetaData. */
    if (!this.rtaXml.equals("")) {
        meta.parse(this.rtaXml, this.executable);
    }
    else {
        meta.parseFromExec(this.executable);
    }

    var NdkAgent = xdc.useModule('ti.uia.scripts.ndk.NdkAgent');

    /* Set the IP address of the board. */
    NdkAgent.address = "146.252.161.93";

    /* Set the ports for sending and receiving. */
    NdkAgent.msgUdpPort = 1234;
    NdkAgent.eventUdpPort = 1235;

    /* Set the number of cores. */
    NdkAgent.numProcessors = 6;

    print("Opening UDP sockets.");
    NdkAgent.openSockets();

    /* Signal all cores to start sending data. */
    print("Signalling all cores to start sending data.");
    NdkAgent.startAllCores();

    print("All cores started, reading data.");

    /* Create an output stream to write the bytes to a file. */
    //var outFile = new xdc.jre.java.io.File("C:/UIAReceivedData_2.bin");
    //var fos = new xdc.jre.java.io.FileOutputStream(outFile);

    var writeToFile = !this.outputFile.equals("");

    if (this.more) {
        print("-m : The script will wait for you to press 'enter' before " +
              "displaying the next buffer...");
        cmdr.read();
    }

    /* Create the file writer for the output file. */
    var fw;
    var d = this.delimiter;
    if (writeToFile) {
        var f = xdc.jre.java.io.File(this.outputFile);
        fw = xdc.jre.java.io.FileWriter(f);
        fw.write("Timestamp" + d + "Sequence Num" + d + "Module" + d +
                 "Message" + d + "Logger\n");
    }

    var EventPacketDecoder = xdc.useModule('ti.uia.scripts.EventPacketDecoder');
    var LoggerBuf2Decoder = xdc.useModule('ti.uia.scripts.LoggerBuf2Decoder');

    // TODO - These should be passed the MetaData object
    EventPacketDecoder.initialize();
    LoggerBuf2Decoder.initialize(meta, 6, 2);

    /* Read data from the target in a loop. */
    while (true) {
        /* Get a packet of data. */
        var packet = NdkAgent.readEvents();

        /* Decode the packet header. */
        var header = EventPacketDecoder.decodePacketHeader(packet.getData());

        var instanceId = header.instanceId;
        var payloadBytes = header.eventLength - 16;

        if (packet.getOffset() != 0) {
            throw ("ERROR! packet.getOffset() != 0");
        }

        /* Print the header */
        if (this.printToScreen && !this.suppressHeader) {
            print("\n=============================================");
            print("Core: " + header.srcAddr);
            print(meta.getLoggerNames()[instanceId] + " - " +
                  payloadBytes + " bytes");
            print("=============================================\n");
        }

        var buffer = packet.getData();
        var offset = 16;
        var evt;

        while (true) {
            /* Decode the record. */
            evt = LoggerBuf2Decoder.decodeEvent(header, buffer, offset,
                                                header.eventLength,
                                                header.payloadEndian);

            if (evt.len == 0) {
                break;
            }
            else {
                offset += evt.len;

                /* Write the decoded record. */
                if (writeToFile) {
                    fw.write(evt.timestamp + d + evt.seqNum + d +
                             meta.lookupModuleName(evt.moduleID) + d +
                             LoggerBuf2Decoder.getFormattedMsg(evt) + d +
                             meta.getLoggerNames()[instanceId] + "\n");
                }

                /* Print the decoded record. */
                if (this.printToScreen) {
                    //print(LoggerBuf2Decoder.getDebugMsg(evt));
                    print("[" + evt.timestamp + "] #" + evt.seqNum + ": " +
                          getShortModName(meta.lookupModuleName(evt.moduleID)) +
                          " " + LoggerBuf2Decoder.getFormattedMsg(evt));
                }

                if (offset >= header.eventLength) {
                    break;
                }
            }
        }

        /* Wait for the user to press enter before moving to the next packet.*/
        if (this.more) {
            cmdr.read();
        }
    }

    /* Close the file writer. */
    if (writeToFile) {
        fw.close();
    }

    print("ti.uia.scripts.ndk done.");
}

/*
 *  ======== fileExists ========
 *  Returns whether or not the file at the given path exists.
 */
function fileExists(filePath) {
    return (xdc.jre.java.io.File(filePath).exists());
}

/*
 *  ======== getShortModName ========
 *  Takes an event, retrieves its module name, then strips off the package
 *  name leaving just the module name.
 */
function getShortModName(name)
{
    var shortName = name;
    try {
        shortName = name.substring(name.lastIndexOf('.') + 1, name.length());
    }
    catch (e) {
        print("ERROR: " + e);
    }

    return (shortName);
}
