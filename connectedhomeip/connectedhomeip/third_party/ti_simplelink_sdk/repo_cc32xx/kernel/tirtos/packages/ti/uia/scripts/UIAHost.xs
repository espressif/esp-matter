/*
 *  ======== UIAHost ========
 *  This script receives UIA data over the ethernet, decodes it, and prints
 *  it to the console.
 *
 *  The top of the file contains a number of variables that must be setup
 *  before running this script for your particular application.
 */

/* Load all required packages. */
xdc.loadPackage('ti.targets.omf.cof');
xdc.loadPackage('ti.targets.omf.elf');
xdc.loadPackage('ti.uia.runtime');
xdc.loadPackage('ti.uia.services');
xdc.loadPackage('ti.uia.scripts');
xdc.loadPackage('ti.uia.scripts.ndk');

var executable = "V:/toddm/uia/src/ti/uia/examples/evm6472/ti_platforms_evm6472_core0/debug/stairstepOneCore.x64P";
var rtaXml = "V:/toddm/uia/src/ti/uia/examples/evm6472/package/cfg/ti_platforms_evm6472_core0/debug/stairstepOneCore_x64P.rta.xml";
var outputFile = null;

var agent = new Packages.ti.uia.scripts.ndk.NdkAgent();

/* Set the number of cores. */
agent.numProcessors = 1;

/* Set the IP address of the board. */
agent.address = "146.252.161.26";

/* If the target does not support control, set to false */
var supportControl = true;

/* Set the format of the decoded records when printed. */
var printLevel = Packages.ti.uia.scripts.PacketProcessor.PrintLevel.READABLE;

run(arguments);

/*
 *  ======== run ========
 *  Execute within a function context to reduce the scope of any variables.
 */
function run()
{
    var processor =
        new Packages.ti.uia.scripts.PacketProcessor(executable, rtaXml,
                                                    outputFile);
    print("UIA Client started.");

    /* Create the PacketProcessor to handle decoding. */
    print("Opening UDP sockets.");
    agent.openSockets(supportControl);

    if (arguments[0][0] != undefined) {

        if (supportControl == false) {
            print("supportControl is set to false in this script.");
            return;
        }

        switch (arguments[0][0][1]) {
            case 'f': {
                agent.sendGetCpuFreqCmd(0);
                while (true) {
                }
                return;
            }
            case 'm': {
                agent.sendReadMaskCmd(0, parseInt(arguments[0][1]));
                return;
            }
            case 'w': {
                agent.sendWriteMaskCmd(0, parseInt(arguments[0][1]), parseInt(arguments[0][2]));
                return;
            }
            case 'p': {
                agent.sendSetPeriodCmd(0, parseInt(arguments[0][1]));
                return;
            }
            case 's': {
                agent.sendStartCmd(0);
                break;
            }
            case 'x': {
                agent.sendStopCmd(0);
                return;
            }
            case 'e': {
                agent.sendLoggerEnableCmd(0, parseInt(arguments[0][1]));
                return;
            }
            case 'E': {
                agent.sendLoggerEnableAllCmd(0);
                while (true) {
                }
                return;
            }
            case 'd': {
                agent.sendLoggerDisableCmd(0, parseInt(arguments[0][1]));
                return;
            }
            case 'D': {
                agent.sendLoggerDisableAllCmd(0);
                while (true) {
                }
                return;
            }
            case 'r': {
                agent.sendLoggerResetCmd(0, parseInt(arguments[0][1]));
                return;
            }
            case 'R': {
                agent.sendLoggerResetAllCmd(0);
                return;
            }
            case 'T': {
                agent.sendSnapshotCmd(0, parseInt(arguments[0][1]), parseInt(arguments[0][2]));
                break;
            }
            case '?':
            case 'h': {
                print("-f : get CPU Frequency");
                print("-m <addr>: read mask specified by address");
                print("-w <addr> <mask>: write mask specified by address");
                print("-p <timeInMs>: Set Rta period");
                print("-s : start Rta");
                print("-x : stop Rta");
                print("-T <resetFlag> <period in ms>: snapshot Rta");
                print("-e <loggerId>: Enable Logger");
                print("-E : Enable all loggers");
                print("-d <loggerId>: Disable Logger");
                print("-D : Disable all loggers");
                print("-r <loggerId>: Reset Logger");
                print("-R : Reset all loggers");
                print("no option: start receiving events from target");
                return;
            }
            default: {
                /* Signal all cores to start sending data. */
                if (supportControl == true) {
                    print("Signalling all cores to start sending data.");
                    agent.startAllCores();
                }
            }
        }
    }

     /* Read data from the target in a loop. */
     print("Receiving data...");
     while (true) {
        /* Retrieve a packet from the ethernet. */
        var packet = agent.readEvents();

        /* Get the byte buffer from the packet. */
        var buffer = packet.getData();

        /* Decode the packet header. */
        var header = processor.readEventHeader(buffer, true);

        /* Read, decode, and print the next packet of records. */
        processor.readPacketBody(header, buffer, 16, printLevel);
     }
}
