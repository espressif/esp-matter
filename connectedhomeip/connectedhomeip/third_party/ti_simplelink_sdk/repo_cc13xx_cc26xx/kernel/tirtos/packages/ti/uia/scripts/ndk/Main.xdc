/*!
 *  ======== Main ========
 *  Tool for reading and decoding UIA data over ethernet.
 */
metaonly module Main inherits xdc.tools.ICmd {

    override config String usage[] = [
        ' ',
        'Usage',
        '[-p] [-h] [-m] [-e executable] [-x RTA XML file] [-o output text file] [-c delimter character]',
        '[--help]',
        ' ',
    ];

instance:

    /*!
     *  ======== printToScreen ========
     *  Whether to print the decoded records to the console.
     *
     *  You may use this tool to simply print the decoded records, or you
     *  may have them redirected to a file to be processed elsewhere.
     */
    @CommandOption('p')
    config Bool printToScreen = false;

    /*!
     *  ======== suppressHeader ========
     *  Don't print the header when printing the decoded records.
     */
    @CommandOption('h')
    config Bool suppressHeader = false;

    /*!
     *  ======== more ========
     *  Wait for the user to press enter before displaying the next packet.
     *
     *  Without this flag, the tool will simply print all of the records in the
     *  data file to the screen. If this flag is set, then the tool will wait
     *  the user to press enter in between displaying each packet.
     */
    @CommandOption('m')
    config Bool more = false;

    /*!
     *  ======== executable ========
     *  Path to the executable which was used to generate the data file.
     */
    @CommandOption('e')
    config String executable = "";

    /*!
     *  ======== rtaXml ========
     *  Optional path to the RTA XML file.
     */
    @CommandOption('x')
    config String rtaXml = "";

    /*!
     *  ======== outputFile ========
     *  Optional path to an output file to write the decoded records to.
     *
     *  The tool will write the decoded records out as text to the specified
     *  out file. The record properties will be separated with a delimeter
     *  character so that they can be imported into a spreadsheet such as
     *  Excel.
     */
    @CommandOption('o')
    config String outputFile = "";

    /*!
     *  ======== loadFile ========
     *  // TODO
     *  [-l output file for load data]
     */

    /*!
     *  ======== delimiter ========
     *  Delimiter character to use when writing out the file.
     *
     *  The character specified here will be inserted between each of the
     *  record fields so that the file can be imported into a spreadsheet
     *  such as excel.
     */
    @CommandOption('c')
    config String delimiter = "|";

}
