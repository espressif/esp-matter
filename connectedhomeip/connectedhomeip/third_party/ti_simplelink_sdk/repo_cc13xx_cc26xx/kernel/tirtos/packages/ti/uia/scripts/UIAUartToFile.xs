/*
 * Copyright (c) 2013, Texas Instruments Incorporated
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
 *  ======== UIAUartToFile.xs ========
 *
 *  This script collects Log data from a target program that
 *  outputs Log data over the UART.  A cleaned up version of the Log
 *  data with UIA headers is saved to a binary file that can then
 *  be opened in System Analyzer.
 *
 *  The target program needs to be configured with
 *  LoggerIdle.transportType = LoggerIdle.TransportType_UART (see
 *  example).  This script will open the COM port for the target program,
 *  collect Log data from the target,
 *  and output the Log data to a file called systemAnalyzerData.bin.  This file
 *  can then be opened in System Analyzer.
 *
 *  Usage:
 *
 *      xs --cp <class path>
 *              -f UIAUartToFile.xs
 *              -c <com port>
 *              -v verbose
 *
 *  Running on Windows
 *  ==================
 *
 *  1. First, you need to have the RXTX Java library installed. Instructions can
 *     be found here: http://rxtx.qbang.org/wiki/index.php/Installation_for_Windows
 *     and binaries here: http://rxtx.qbang.org/wiki/index.php/Download.
 *
 *     Make sure to add <jdk>/jre/bin to the java library path.  This can be
 *     done by adding it to the Path environment variable (Control Panel ->
 *     System -> Advanced -> Environment Variables).  For example add
 *     c:/Program Files (x86)/Java/jdk1.6.0_26/jre/bin to Path.
 *
 *  2. Start the target program, if not already running.
 *
 *  3. Run the script (we'll assume the script is located in the uartlogging
 *     example project directory). Example:
 *
 *     c:\ti\xdctools_3_24_05_48\xs --cp c:/PROGRA~2/Java/jdk1.6.0_26/jre/lib/ext/RXTXcomm.jar
 *       -f UIAUartToFile.xs
 *       -c COM4
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

/* Windows */
var comPort = "COM4";
var verbose = false;

// When opening a binary file for System Analyzer, the name of the
// is not configurable, only the directory location.
var uiaData = "systemAnalyzerData.bin";
var uiaDir = ".";  // Directory to open uia data file.

var outFile = null; //"uia-output.csv";

/* Set the format of the decoded records when printed. */
var printLevel = Packages.ti.uia.scripts.PacketProcessor.PrintLevel.READABLE;

var comOpt  = "-c";  // Option for name of COM port
var verbOpt = "-v";  // Verbose option
var helpOpt = "-h";  // Help option

var comUsage = comOpt + " com\t - The name of the serial port, eg COM4, to capture Log data.";
var verbUsage = verbOpt + "   \t - Run in verbose mode";
var helpUsage = helpOpt + "   \t - Print this message and exit";

var optsTab = [
    {opt: comOpt,  val : comPort,   usage : comUsage},
    {opt: verbOpt, val : false,     usage : verbUsage},
    {opt: helpOpt, val : false,     usage : helpUsage}
];

run(arguments);


/*
 *  ======== run ========
 */
function run(args)
{
    var eventNum = 0;

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

    /* open UART */
    print("Opening UART connection...");
    var inStream = openUartConnection();

    var outStream = null;
    outStream = new java.io.FileOutputStream(uiaDir + "/" + uiaData);

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
    uiaHeader[0] = -96;  // 0xa0 (HdrType: EventPkt)
    uiaHeader[1] = 0;    // Event length
    uiaHeader[2] = 0;    // Event length
    uiaHeader[3] = 0;    // Event length

    uiaHeader[4] = 0;    // Seq count
    uiaHeader[5] = 0;    // Seq count
    uiaHeader[6] = 0;
    uiaHeader[7] = 1;    // priority

    uiaHeader[8] = 0;      // Instance Id
    uiaHeader[9] = 1;      // Instance Id
    uiaHeader[10] = 0;     // Module Id
    uiaHeader[11] = 0;     // Module Id

    uiaHeader[12] = -1;    // Dest address
    uiaHeader[13] = -1;    // Dest address
    uiaHeader[14] = 0;     // Source address
    uiaHeader[15] = 0;     // Source address

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

        print("Writing event number: " + eventNum++);
        outStream.write(uiaHeader, 0, 16);
        outStream.write(findEvent, firstSeq, totalLength);

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
                timeout = true;
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

            /* Write length to dummy UIA header */
            uiaHeader[3] = eventPacket[2] + 16;

            /* read in event body */
            nBytes = readExactlyTimeout(inStream, eventPacket, 4, eventPacket[2] - 4);
            if (nBytes != eventPacket[2] - 4) {
                timeout = true;
            }

            if (timeout) {
                print("break 7 (timeout)");
                break;
            }

            print("Writing event number: " + eventNum++);
            outStream.write(uiaHeader, 0, 16);
            outStream.write(eventPacket, 0, eventPacket[2]);
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
            case comOpt:
                comPort = optsTab[i].val
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
    serialPortCom.setSerialPortParams(115200,
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
    print("Usage:\n" + "\txs [--cp class-path]\n\t\t" +
            "-f UIAHostFromUART.xs " +
            "[" + comOpt + " com-port] "+
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
    if (comPort == null) {
        print("Error: You must specify a COM port with the " + comOpt + " option!");
        printUsage();
        return (false);
    }

    return (true);
}
