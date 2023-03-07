/*
 * Copyright (c) 2013-2015, Texas Instruments Incorporated
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
 *
 */

/*
 *  ======== UIAHostFromUART.xs ========
 *
 *  This script collects and interprets Log data for a target program that
 *  outputs Log data over the UART. The target program needs to be configured
 *  with LoggerIdle.transportType = LoggerIdle.TransportType_UART (see
 *  example).  If a com port is specified on the command line, this script will
 *  open the COM port for the target program, collect Log data from the target,
 *  and output the Log data to the console. There are options for saving the
 *  output to a .csv file, or to a binary file that can be opened in CCS's
 *  System Analyzer.  If the raw Log data has already been collected into a
 *  binary file by a different host utility, the data file can be passed on the
 *  command line instead of a com port, and this script will interpret the data.
 *
 *  Usage:
 *
 *      xs --cp <class path> --xdcpath <xdc path>
 *              -f UIAHostFromUART.xs
 *              -p <target program>
 *              -r <target program rta.xml file>
 *              -c <com port, if receiving data from UART>
 *              -b <baud rate, if receiving data from UART>
 *              -d <input data file, if log data is in a file, not com port>
 *              -o <output .csv file>
 *              -s <directory to output data into systemAnalyzerData.bin file>
 *              -v verbose
 *
 *  Example usage in Linux:
 *  ======================
 *  Reading Log data from the COM port:
 *   xs --cp /home/a0xxx/eclipse/org.eclipse.platform_3.7.0_692372347/plugins/gnu.io.rxtx_2.1.7.4_v20071016.jar
 *      -f UIAHostFromUART.xs -p /home/a0xxx/workspace_v5_3/TMDXDOCKH52C1_uartconsole/Debug/TMDXDOCKH52C1_uartlogging.out
 *      -r /home/a0xxx/workspace_v5_3/TMDXDOCKH52C1_uartconsole/Debug/configPkg/package/cfg/uartlogging_pem3.rta.xml
 *      -c /dev/ttyACM0
 *
 *  Reading Log data from a file:
 *   xs -f UIAHostFromUART.xs
 *      -p /home/a0xxx/workspace_v5_3/TMDXDOCKH52C1_uartconsole/Debug/TMDXDOCKH52C1_uartlogging.out
 *      -r /home/a0xxx/workspace_v5_3/TMDXDOCKH52C1_uartconsole/Debug/configPkg/package/cfg/uartlogging_pem3.rta.xml
 *      -f logdata.dat
 *
 *  Running on Windows
 *  ==================
 *
 *  1. First, you need to have the RXTX Java library installed. Instructions can
 *     be found here: http://rxtx.qbang.org/wiki/index.php/Installation_for_Windows
 *     and binaries here: http://rxtx.qbang.org/wiki/index.php/Download.
 *
 *     The two binaries needed are rxtxSerial.dll and RXTXcomm.jar.
 *     The rxtxSerial.dll should be copied to <JRE_DIR>/jre/bin, and the RXTXcomm.jar
 *     goes in the <JRE_DIR>/jre/lib/ext directory, where <JRE_DIR> is the directory
 *     containing the Java runtime.  You can always use the location
 *     of the Java runtime that comes with CCS to place the RXTX binaries.
 *     You will find this in <CCS_INSTALL_DIR>/ccsv6/eclipse/jre.
 *
 *     Make sure to add <JRE_DIR>/jre/bin to the java library path.  This can be
 *     done by adding it to the Path environment variable (Control Panel ->
 *     System -> Advanced -> Environment Variables).  For example add
 *     c:/Program Files (x86)/Java/jdk1.6.0_26/jre/bin to Path.
 *
 *  2. Start the target program, if not already running.
 *
 *  3. Run the script (we'll assume the script is located in the uartlogging
 *     example project directory). Example reading Log data from a com port
 *     and saving the data to .\systemAnalyzerData.bin:
 *
 *     c:\ti\xdctools_3_24_05_48\xs
 *       --cp c:/PROGRA~2/Java/jdk1.6.0_26/jre/lib/ext/RXTXcomm.jar
 *       --xdcpath c:/ti/ccsv5.3.0.00042/uia_1_02_00_03_eng/packages -f UIAHostFromUART.xs
 *           -p Debug\EKS_LM4F232_uartlogging.out
 *           -r Debug\configPkg\package\cfg\uartlogging_pem4f.rta.xml
 *           -c COM4
 *           -s .
 *
 *     Use the uiascript.bat file, after having carefully set its variables,
 *     for ease of use.
 *
 *     Example reading data from a file (NOTE: Since we're not opening the COM
 *     port in this case, we don't need to give the class and library paths.):
 *
 *     c:\ti\xdctools_3_24_05_48\xs
 *       --xdcpath c:/ti/ccsv5.3.0.00042/uia_1_02_00_03_eng/packages -f UIAHostFromUART.xs
 *           -p Debug\EKS_LM4F232_uartlogging.out
 *           -r Debug\configPkg\package\cfg\uartlogging_pem4f.rta.xml
 *           -d logdata.dat
 *
 *     NOTE: If you don't have <jdk>/jre/bin in the java library path, you may
 *     see output like this:
 *
 *     UIA UART Client started.
 *     Opening UART connection...
 *     java.lang.UnsatisfiedLinkError: no rxtxSerial in java.library.path thrown while
 *     loading gnu.io.RXTXCommDriver
 *     Exception in thread "main" java.lang.UnsatisfiedLinkError: no rxtxSerial in
 *     java.library.path at java.lang.ClassLoader.loadLibrary(ClassLoader.java:1738)
 *
 *     An incorrect path to the RXTXcomm.jar file can result in this type of error:
 *
 *     C:\ti\ccsv6.1.0.00104\workspace_2_14_02>.\uiascript.bat
 *     UIA UART Client started.
 *     Opening UART connection...
 *     js: "C:/ti/ccsv6.1.0.00104/tirtos_tivac_2_14_01_20/products/uia_2_00_02_39/packages/ti/uia/scripts/UIAHostFromUART.xs", line 658: TypeError: Cannot call property getPortIdentifier in object [JavaPackage gnu.io.CommPortIdentifier]. It is not
 a function, it is "object".
 *     "C:/ti/ccsv6.1.0.00104/tirtos_tivac_2_14_01_20/products/uia_2_00_02_39/packages/ti/uia/scripts/UIAHostFromUART.xs", line 249
 *     "C:/ti/ccsv6.1.0.00104/tirtos_tivac_2_14_01_20/products/uia_2_00_02_39/packages/ti/uia/scripts/UIAHostFromUART.xs", line 220

 *
 *     NOTE: If you run an older version of xs (eg, from xdctools_3_23), you may
 *     see an error like this:
 *         UIA UART Client started.
 *         Opening UART connection...
 *         Stable Library
 *         =========================================
 *         Native lib Version = RXTX-2.1-7
 *         Java lib Version   = RXTX-2.1-7
 *         COM4 opened!
 *         js: "./UIAHostFromUART.xs", line 74: TypeError: [JavaPackage ti.uia.scripts.PacketProcessor]
 *         is not a function, it is object.
 *             "./UIAHostFromUART.xs", line 55
 *
 *     If this happens, you need to use a newer version of xs.
 *
 *     NOTE:  If there are gaps in the data received from the com port
 *     this script will attempt to recover.  When this happens, you
 *     will see a "BREAK" statement printed out.  This seems to be caused by data
 *     being lost somewhere in transmission to the host, and not by data
 *     corruption on the target.
 *     While testing this script, this type of error has only been observed
 *     to happen a few times each run of the script, and only after the first
 *     few data transfers of the run.
 *     All attempts have been made to recover from data loss, without throwing
 *     an exception, but in the case that an exception is thrown, the script
 *     can just be re-run, without restarting the target program.
 *
 */

/* Load all required packages. */
xdc.loadPackage('ti.targets.omf.elf');
xdc.loadPackage('ti.uia.loggers');
xdc.loadPackage('ti.uia.sysbios');
xdc.loadPackage('ti.uia.runtime');
xdc.loadPackage('ti.uia.services');
xdc.loadPackage('ti.uia.scripts');
xdc.loadPackage('ti.uia.scripts.ndk');

/* Linux */
//var comPort = "/dev/ttyACM0";
//var executable = "/home/a0324034/workspace_v5_2/TMDXDOCKH52C1_uartconsole/Debug/TMDXDOCKH52C1_uartconsole.out";
//var rtaXml = "/home/a0324034/workspace_v5_2/TMDXDOCKH52C1_uartconsole/Debug/configPkg/package/cfg/uartconsole_pem3.rta.xml";

/* Windows */
var comPort = null;
var fileName = null;
var prog = null;
var rtaXml = null;
var verbose = false;
var readDataFromFile = false;
var baudRate = 115200;

// When opening a binary file for System Analyzer, the name of the
// is not configurable, only the directory location.
// When the 'save' option is used, the cleaned up Log data (incomplete
// records removed) with UIA headers is saved to a file.  This file
// can then be opened and viewed in System Analyzer.
var uiaData = "systemAnalyzerData.bin";
var uiaDir = null;  // Directory to open uia data file.

var outFile = null; //"uia-output.csv";

/* Set the format of the decoded records when printed. */
var printLevel = Packages.ti.uia.scripts.PacketProcessor.PrintLevel.READABLE;

var baudOpt = "-b";  // Option for setting the baud rate
var exeOpt  = "-p";  // Option for name of target executable
var dataOpt = "-d";  // Option for name of data file
var outOpt  = "-o";  // Option for name of output csv file
var rtaOpt  = "-r";  // Option for name of rts.xml file
var comOpt  = "-c";  // Option for name of COM port
var saveOpt = "-s";  // Save cleaned up version of Log data with UIA headers
var verbOpt = "-v";  // Verbose option
var helpOpt = "-h";  // Help option

var baudUsage = baudOpt + " baudrate\t - Serial connection baud rate (default is 115200).";
var exeUsage = exeOpt + " prog\t - Name of target executable producing the Log data.";
var rtaUsage = rtaOpt + " rtaXml\t - Name of target executable's rta.xml file.";
var dataUsage = dataOpt + " dataFile\t - Name of file containing captured Log data, if not using\n\t\t" +
                          "   this script to capture the Log data from the serial port.";
var outUsage = outOpt + " csv\t - The name of a csv file to output Log data to.";
var comUsage = comOpt + " com\t - The name of the serial port, eg COM4, to capture Log data.";
var saveUsage = saveOpt + " dir\t - Directory to save file, systamAnalyzerData.bin. This\n\t\t" +
                          "   option saves the Log data to a binary file in a format that\n\t\t" +
                          "   can by used by System Analyzer.";
var verbUsage = verbOpt + "   \t - Run in verbose mode";
var helpUsage = helpOpt + "   \t - Print this message and exit";

var optsTab = [
    {opt: baudOpt, val : baudRate,  usage : baudUsage},
    {opt: exeOpt,  val : prog,      usage : exeUsage},
    {opt: rtaOpt,  val : rtaXml,    usage : rtaUsage},
    {opt: dataOpt, val : fileName,  usage : dataUsage},
    {opt: outOpt,  val : outFile,   usage : outUsage},
    {opt: comOpt,  val : comPort,   usage : comUsage},
    {opt: saveOpt, val : uiaDir,    usage : saveUsage},
    {opt: verbOpt, val : false,     usage : verbUsage},
    {opt: helpOpt, val : false,     usage : helpUsage}
];

run(arguments);


/*
 *  ======== run ========
 */
function run(args)
{
    print("UIA UART Client started.");

    xdc.loadPackage('xdc.rta');
    xdc.loadPackage('xdc.rov');

    var eBadData = -1;
    var eEndOfFile = -2;
    var eTimeout = -3;

    if (!getopts(args)) {
        return;
    }

    /* Validate arguments */
    if (!validate()) {
        return;
    }

    if (comPort != null) {
        /* open UART */
        print("Opening UART connection...");
        var inStream = openUartConnection();
    }
    else {
        try {
            var inStream = new java.io.FileInputStream(fileName);
        }
        catch (e) {
            print("Unable to open file: " + fileName);
            return;
        }
        readDataFromFile = true;
    }

    var outStream = null;
    if (uiaDir != null) {
        outStream = new java.io.FileOutputStream(uiaDir + "/" + uiaData);
    }

    /* Create a PacketProcessor to manage decoding. */
    var processor = new Packages.ti.uia.scripts.PacketProcessor(prog,
            rtaXml, outFile);

    /* Read data from the target in a loop. */
    print("Receiving data...");

    var eventHeader = new xdc.jre.java.lang.reflect.Array.newInstance(
        xdc.jre.java.lang.Byte.TYPE, 4);

    var uiaHeader = xdc.jre.java.lang.reflect.Array.newInstance(
        xdc.jre.java.lang.Byte.TYPE, 16);

    var eventPacket = xdc.jre.java.lang.reflect.Array.newInstance(
        xdc.jre.java.lang.Byte.TYPE, 48);

    var findEvent = xdc.jre.java.lang.reflect.Array.newInstance(
        xdc.jre.java.lang.Byte.TYPE, 240);

    /* Construct dummy UIA header */
    uiaHeader[0] = -96;  // 0xa0
    uiaHeader[1] = 0;
    uiaHeader[2] = 0;
    uiaHeader[3] = 0;

    uiaHeader[4] = 0;
    uiaHeader[5] = 0;
    uiaHeader[6] = 0;
    uiaHeader[7] = 1;

    uiaHeader[8] = 0;
    uiaHeader[9] = 1;
    uiaHeader[10] = 0;
    uiaHeader[11] = 0;

    uiaHeader[12] = -1;
    uiaHeader[13] = -1;
    uiaHeader[14] = 0;
    uiaHeader[15] = 0;

    var firstSeq = 0;
    var secSeq = 0;

    var index = 0;	/* next empty spot in buffer */

    var errorType = 0;
    var nBytes = 0;

    /*
     *  Run forever
     *
     *  This loop will first find a full log event then run forever.
     *  If data stops streaming for more than 500 ms then it will drop
     *  the current log event and find the next full event.
     *  This handles the case of the data stream stopping,
     *  expecting to finish any partial packets is unreasonable.
     */
    while (true) {

        index = 0;
        firstSeq = 0;
        secSeq = 0;

        if ((nBytes = readExactly(inStream, findEvent, index, 8)) != 8) {
            print("(1) End of file reached, exiting...");
            errorType = eEndOfFile;
            break;
        }
        index = 8;
        var match = false;

        /*
         *  Find the first 2 log events in a stream.
         *
         *  Get a new byte and compare it to all previous bytes recieved.
         *  If the new byte = any previous byte + 1 than the two bytes could
         *  be sequence numbers.  To check that they are actually sequence
         *  numbers, get the length of the first event and check
         *  that it is both a reasonable size (greater than or equal to 8)
         *  and equal to the diference of the index of the two sequence
         *  numbers. If this is the case then the two indices are the start
         *  of two events.
         */
        while (!match) {
            /* read byte and check if it equals a previous byte + 1 */
            if ((nBytes = readExactly(inStream, findEvent, index, 1)) != 1) {
                print("(2) End of file reached, exiting...");
                errorType = eEndOfFile;
                break;
            }
            index++;

            for (var i = 0; i < index - 1; i++) {
                if ((findEvent[i] + 1) == findEvent[index - 1]) {
                    /*
                     *  Check if byte where length should be is a reasonable
                     *  event length and the distance between the two sequence
                     *  numbers equals the length. The event type should be
                     *  Event (0), or Event with timestamp (8).
                     *  If these conditions hold, it is almost certainly
                     *  an event.
                     */
                    if ((findEvent[i + 2] > 7 ) &&
                            (findEvent[i + 2] == (index - 1 - i) ) &&
                            ((findEvent[i+3] == 0) || (findEvent[i+3] == 8))) {
                        /*
                         *  i is the index of the start of the first event,
                         *  index - 1 is the start of the second event
                         */
                        firstSeq = i;
                        secSeq = index - 1;
                        match = true;
                        break;
                    }
                }
            }
            if (index > 160) {
                /* reset */
                index = 0;
            }
        }


        /* Match found or end of file reached. */
        if (errorType == eEndOfFile) {
            break;
        }

        /* Read in the rest of the second event */
        if ((nBytes = readExactly(inStream, findEvent, index, 2)) != 2) {
            print("(3) End of file reached, exiting...");
            break;
        }

        index += 2;
        var length = findEvent[index - 1];      /* Length of second event */

        if ((length < 8) || (length > 48)) {
            print("(break 1) Unable to read event due bad length " +
                    "(missing or corrupted data), will restart!!!");
            print("******************* BREAK ********************");
            continue;
        }

        /* Get remainder of second event */
        readExactly(inStream, findEvent, index, (length - 3));
        index += (length - 3);  /* index now equals the end of the second event */

        /* Get the UIA header */
        var totalLength = findEvent[firstSeq + 2] + findEvent[secSeq + 2];
        uiaHeader[3] = totalLength + 16;
        var header = processor.readEventHeader(uiaHeader, false);

        /* Process the first two events */
        try {
            var headerEvent = processor.readPacketBody(header, findEvent,
                    firstSeq, printLevel);
        }
        catch (e) {
            print("(break 2) Unable to interpret event due to missing or " +
                    "corrupted data, will restart!!!");
            print("******************* BREAK ********************");
            continue;
        }

        if (outStream) {
            outStream.write(uiaHeader, 0, 16);
            outStream.write(findEvent, firstSeq, totalLength);
        }

        /*
         *  The next byte will be the first byte of a full log event.
         *  Read in log events until timeout or end of file is reached.
         *  The program will break if there is a delay in a read
         *  so that it can find the next full log event.
         */
        while (true) {
	    var timeout = false;
            /* read in the first 4 bytes, event header */
            if ((nBytes = readExactlyTimeout(inStream, eventPacket, 0, 4)) != 4) {
                if (readDataFromFile) {
                    print("(5) End of file reached, exiting...");
                    errorType = eEndOfFile;
                    break;
		}
		else {
                    timeout = true;
		}
            }

            // Check for valid header type.  This could occur if data was lost.
            var hdrType = eventPacket[3];

            if ((hdrType != 0) && (hdrType != 8)) {
                // Not an event or event with timestamp
                print("(break 3) Got bad event: " + hdrType.toString(16) +
                        " !!! Restarting...");
                errorType = eBadData;
                break;
            }

            if (timeout || (eventPacket[2] < 8) || (eventPacket[2] > 48)) {
                //print("timeout, size = ", +eventPacket[2]);
                if (timeout) {
                    errorType = eTimeout;
                    print("(break 4) TIMEOUT");
                }
                else {
                    errorType = eBadData;
                    print("(break 5) got bad event size: EVENT SIZE = ", +eventPacket[2] + ", restarting...");
                }
                print(+eventPacket[0], " ",
                        +eventPacket[1], " ",
                        +eventPacket[2], " ",
                        +eventPacket[3], " ",
                        +eventPacket[4], " ",
                        +eventPacket[5], " ",
                        +eventPacket[6], " ",
                        +eventPacket[7]);
                break;
            }

            //print("eventPacket[0]: 0x" + eventPacket[0].toString(16),
            //        "eventPacket[1]: ", + eventPacket[1],
            //        "eventPacket[2]: ", + eventPacket[2],
            //        "eventPacket[3]: ", + eventPacket[3]);

            /* add length to dummy UIA header */
            uiaHeader[3] = eventPacket[2] + 16;

            /* get UIA header */
            header = processor.readEventHeader(uiaHeader, false);
            if (header == null) {
                print("break 6");
                break;
            }

            /* read in event body */
            nBytes = readExactlyTimeout(inStream, eventPacket, 4, eventPacket[2] - 4);
            if (nBytes != eventPacket[2] - 4) {
                if (readDataFromFile) {
                    errorType = eEndOfFile;
                    print("(6) End of file reached, exiting...");
                    break;
                }
                else {
                    timeout = true;
                }
            }

            if (timeout) {
                print("break 7 (timeout)");
                break;
            }

            /* Process packet body. */
            try {
                headerEvent = processor.readPacketBody(header, eventPacket, 0,
                        printLevel);
            }
            catch (e) {
                print("(break 8) Missing or corrupted data, will attempt " +
                        "to restart!!!");
                errorType = eBadData;
                break;
            }

            if (outStream) {
                outStream.write(uiaHeader, 0, 16);
                outStream.write(eventPacket, 0, eventPacket[2]);
            }
        }

        print("******************* BREAK ********************");
        if (errorType == eBadData) {
            // If we encounter a bad event header type or size, data
            // may have been lost, so we'll start the process over.
            continue;
        }
        break;
    }
    /* Close the file streams. */
    if (outStream) {
        outStream.close();
    }
    if (inStream) {
        inStream.close();
    }
    processor.close();
}

/*
 *  ======== getopts ========
 */
function getopts(args)
{
    // Remove beginning and ending space, replace space with ',' and
    // then split on comma.
    var opts = (args + '').replace(/^\s+|\s+$/g,'').replace(/[\s,]+/, ',').split(',');

    for (var i = 0; i < opts.length; i++) {
        var opt = opts[i];
        if ((opt == "-help") || (opt == "-h") || (opt == "--help")) {
            printUsage();
            return (false);
        }
        if ((opt == "-v") || (opt == "--verbose")) {
            print("Setting verbose flag.");
            verbose = true;
            continue;
        }
        if (opt[0] == '-') {
            for (var j = 0; j < optsTab.length; j++) {
                if (opt == optsTab[j].opt) {
                    if (opt == saveOpt) {
                        /* This option does not require a directory */
                        if ((i < opts.length -1) && (opts[i + 1][0] != '-')) {
                            optsTab[j].val  = opts[++i];
                        }
                        else {
                            optsTab[j].val = ".";
                        }
                        break;
                    }
                    else {
                        if (i < opts.length - 1) {
                            optsTab[j].val = opts[++i];
                        }
                        else {
                            print("Option: " + opt + " requires an argument");
                            printUsage();
                            return (false);
                        }
                        break;
                    }
                }
            }
            if (j == optsTab.length) {
                print("Unknown option: " + opt);
                printUsage();
                return (false);
            }
        }
        //print("opts[" + i + "] = " + opt);
    }

    /* Set variables according to what's in the opts table */
    for (var i = 0; i < optsTab.length; i++) {
        switch (optsTab[i].opt) {
            case exeOpt:
                prog = optsTab[i].val;
                break;
            case dataOpt:
                fileName = optsTab[i].val
                break;
            case outOpt:
                outFile = optsTab[i].val
                break;
            case rtaOpt:
                rtaXml = optsTab[i].val
                break;
            case comOpt:
                comPort = optsTab[i].val
                break;
            case baudOpt:
                baudRate = optsTab[i].val
                break;
            case saveOpt:
                uiaDir = optsTab[i].val;
                break;
            case verbOpt:
                verbose = optsTab[i].val
                break;
            case helpOpt:
                break;
            default:
                throw ("Unknown option in optsTab: " + optsTab[i].opt + "   Fix this function!!!");
                return (false);
        }
    }

    return (true);
}

/*
 *  ======== openUartConnection ========
 */
function openUartConnection()
{
    /* Get the 'CommPortIdentifier' */
    java.lang.System.setProperty("gnu.io.rxtx.SerialPorts", comPort);
    var portIdCom = Packages.gnu.io.CommPortIdentifier.getPortIdentifier(comPort);

    /*
     * Acquire COM. Specify:
     * - The name of the application asking for the port.
     * - Maximum ms to wait to acquire the port.
     */
    var serialPortCom = portIdCom.open("UART RTA", 2000);

    print(portIdCom.getName() + " opened!");

    /*
     * Get an input stream from the port to read in bytes.
     */
    var inputStream = serialPortCom.getInputStream();

    /*
     * Configure the port.
     */
    serialPortCom.setSerialPortParams(baudRate,
            Packages.gnu.io.SerialPort.DATABITS_8,
            Packages.gnu.io.SerialPort.STOPBITS_1,
            Packages.gnu.io.SerialPort.PARITY_NONE);

    /* Disable flow control. */
    // NOTE:  Disabling flow control caused the script not to work for Stellaris,
    // so this line is commented out.  Uncomment to disable flow control.
    //serialPortCom.setFlowControlMode(Packages.gnu.io.SerialPort.FLOWCONTROL_NONE);

    return (inputStream);
}

/*
 *  ======== printUsage ========
 */
function printUsage()
{
    print("Usage:\n" + "\txs [--cp class-path] [--lp library-path] [--xdcpath path]\n\t\t" +
            "-f UIAHostFromUART.xs " +
            exeOpt + " prog " +
            rtaOpt + " rtaXmlFile\n\t\t" +
            "[" + baudOpt + " baud-rate] " +
            "[" + dataOpt + " log-data-file] " +
            "[" + comOpt + " com-port] "+
            "[" + outOpt + " csv-file] " +
            "[ " + saveOpt + " dir]\n\t\t" +
            "[" + verbOpt + "] " +
            "[" + helpOpt + "]\n"
          );
    for (var i = 0; i < optsTab.length; i++) {
        var opt = optsTab[i];
        print("    " + opt.usage);
    }
    return (true);
}


/*
 *  ======== readExactly ========
 *  Blocks until it has read exactly the number of bytes in 'buffer'.
 *
 *  The java.io.Stream interface will read *up to* 'buffer.length' bytes, but we
 *  need to read exactly 'buffer.length'.
 */
function readExactly(inputStream, buffer, offset, length)
{
    var remaining = length;
    var bytesRead = 0;
    var read = 0;

    read = inputStream.read(buffer, offset, remaining);
    remaining -= read;
    offset += read;
    bytesRead += read;

    if (readDataFromFile) {
        return (bytesRead);
    }

    /*
     * The call to read will read UP TO 'curNumBytes', but may read
     * less than that amount.
     *
     * In this loop, we ensure that exactly the amount requested is read.
     */
    while (remaining > 0) {
        //print("--Reading offset: " + offset + " rem: " + remaining);
        read = inputStream.read(buffer, offset, remaining);

        /* Update the offset and total number of bytes read. */
        offset += read;
        bytesRead += read;

        /* Update the number of bytes to read. */
        remaining -= read;
    }

    return (bytesRead);
}

/*
 *  ======== readExactlyTimeout ========
 *  Blocks until it has read exactly the number of bytes in 'buffer'.
 *  Times out if a byte has not been received in a reasonable ammount of time.
 *
 *  The java.io.Stream interface will read *up to* 'buffer.length' bytes, but we
 *  need to read exactly 'buffer.length'.
 */
function readExactlyTimeout(inputStream, buffer, offset, length)
{
    var remaining = length;
    var read = 0;
    var offset2 = offset;
    var bytesRead = 0;

    var start = xdc.jre.java.lang.System.currentTimeMillis();

    read = inputStream.read(buffer, offset2, remaining);

    if (readDataFromFile) {
        return (read);
    }

    /* Update offset and total number of bytes read */
    offset2 += read;
    bytesRead += read;

    /* Remaining number of bytes to read. */
    remaining = remaining - read;

    /*
     * The call to read will read UP TO 'curNumBytes', but may read
     * less than that amount.
     *
     * In this loop, we ensure that exactly the amount requested is read.
     */
    while (remaining > 0) {
        //print("--Reading offset: " + offset + " rem: " + remaining);
        try {
            read = inputStream.read(buffer, offset2, remaining);
        }
        catch (e) {
            throw "Error reading " + remaining + " bytes from input stream!!!";
        }

        /* Update the offset, total bytes read, and remaining to read. */
        offset2 += read;
        bytesRead += read;
        remaining = remaining - read;

        if ((xdc.jre.java.lang.System.currentTimeMillis() - start) > 1000 ) {
            return (bytesRead);
        }
    }

    return (bytesRead);
}

/*
 *  ======== validate ========
 */
function validate()
{
    if (prog == null) {
        print("Error: You must specify the target executable with the " + exeOpt + " option!");
        printUsage();
        return (false);
    }
    if (rtaXml == null) {
        print("Error: You must specify the target rta.xml file with the " + rtaOpt + " option!");
        printUsage();
        return (false);
    }

    if ((fileName == null) && (comPort == null)) {
        print("Error: You must specify either a COM port with the " + comOpt + " option, or a file\n" +
              "containing Log data captured from the COM port with the " + dataOpt + " option!");
        printUsage();
        return (false);
    }

    return (true);
}
