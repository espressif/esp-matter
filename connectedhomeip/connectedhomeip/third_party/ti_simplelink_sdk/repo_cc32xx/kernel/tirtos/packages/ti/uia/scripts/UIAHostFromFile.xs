/*
 *  ======== UIAHostFromFile.xs ========
 *  Script for reading and decoding UIA data from a binary data file.
 *
 *  There are a number of variables at the top of the script which should
 *  be set before using it.
 */

/* Load all required packages. */
xdc.loadPackage('ti.targets.omf.cof');
xdc.loadPackage('ti.targets.omf.elf');
xdc.loadPackage('ti.uia.runtime');
xdc.loadPackage('ti.uia.services');
xdc.loadPackage('ti.uia.scripts');
xdc.loadPackage('ti.uia.scripts.ndk');

var executable = "v:/toddm/uia/src/ti/uia/examples/evmti816x/ti_uia_examples_platforms_evmTI816X_video/debug/videoM3LoggerSM.xem3";
var rtaXml = "v:/toddm/uia/src/ti/uia/examples/evmti816x/package/cfg/ti_uia_examples_platforms_evmTI816X_video/debug/videoM3LoggerSM_xem3.rta.xml";
var inputFile = "C:/temp/UIAReceivedData.bin";

var outputFile = "C:/temp/DecodedData.csv";

/* Set the format of the decoded records when printed. */
var printLevel = Packages.ti.uia.scripts.PacketProcessor.PrintLevel.READABLE;

run();

/*
 *  ======== run ========
 *  Execute within a function context to reduce the scope of any variables.
 */
function run()
{
    print("UIA Host From File started.");

    /* Create a PacketProcessor to manage decoding. */
    var processor =
        new Packages.ti.uia.scripts.FileProcessor(executable, rtaXml,
                                                  inputFile, outputFile);

    /* Loop over all of the packets in the file. */
    while (true) {

        /* Read the next packet header and print it. */
        var header = processor.readNextEventHeader(true);

        /* A null header signifies we've reached the end of the file. */
        if (header == null) {
            break;
        }

        /* Read, decode, and print the next packet of records. */
        processor.readPacketBody(header, printLevel);
    }

    /* Close the file streams. */
    processor.close();
}
