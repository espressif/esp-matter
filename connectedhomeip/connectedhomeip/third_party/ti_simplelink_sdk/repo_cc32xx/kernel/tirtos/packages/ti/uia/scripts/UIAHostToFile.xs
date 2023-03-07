
xdc.loadPackage('ti.uia.runtime');
xdc.loadPackage('ti.uia.services');
xdc.loadPackage('ti.uia.scripts.ndk');

var outputFile = "C:/temp/UIAReceivedData.bin";

var agent = new Packages.ti.uia.scripts.ndk.NdkAgent();

/* Set the number of cores. */
agent.numProcessors = 1;

/* Set the IP address of the board. */
agent.address = "146.252.161.72";

/* Set the ports for sending and receiving. */
agent.msgUdpPort = 1234;
agent.eventUdpPort = 1235;

/* Specify the number of milliseconds to collect data for. */
var runTime = 10000;

/* If the target does not support control, set to false */
var supportControl = true;

/* The service id of the RTA Service. */
var rtaServiceId = 3;

run();

/*
 *  ======== run ========
 *  Execute within a function context to reduce the scope of any variables.
 */
function run()
{
    print("UIA Client started.");

    /* Create an output stream to write the bytes to a file. */
    var outFile = new xdc.jre.java.io.File(outputFile);
    var fos = new xdc.jre.java.io.FileOutputStream(outFile);

    print("Opening UDP sockets.");
    agent.openSockets(supportControl);

    /* Signal all cores to start sending data. */
    if (supportControl == true) {
        print("Signalling all cores to start sending data.");
        agent.startAllCores();
    }

    var start = xdc.jre.java.lang.System.currentTimeMillis();

    /* Specify how long to run for. */
    var end = start + runTime;

    var packetCnt = 0;

    /* Read data from the target in a loop. */
    print("Receiving data...");
    while (xdc.jre.java.lang.System.currentTimeMillis() <= end) {
        /* Retrieve a packet from the ethernet. */
        var packet = agent.readEvents();

        packetCnt++;

        fos.write(packet.getData(), packet.getOffset(), packet.getLength());
    }

    fos.close();

    print("Ran for " + (runTime / 1000) + " seconds, received " + packetCnt +
          " packets. " + (packetCnt * 1000 / runTime) + " pckts/sec.");
}
