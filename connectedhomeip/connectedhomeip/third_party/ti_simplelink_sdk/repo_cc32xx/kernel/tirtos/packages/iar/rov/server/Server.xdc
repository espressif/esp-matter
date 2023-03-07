/* --COPYRIGHT--,EPL
 *  Copyright (c) 2012-2020 Texas Instruments Incorporated
 *  This program and the accompanying materials are made available under the
 *  terms of the Eclipse Public License v1.0 and Eclipse Distribution License
 *  v. 1.0 which accompanies this distribution. The Eclipse Public License is
 *  available at http://www.eclipse.org/legal/epl-v10.html and the Eclipse
 *  Distribution License is available at
 *  http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * --/COPYRIGHT--*/
package iar.rov.server;

/*!
 *  ======== Server.xdc ========
 */
metaonly module Server {

    config Int modelVers = 5;

    /*!
     *  ======== clientVers ========
     *  The flag that signals client's capabilities
     *
     *  Currently, it's used to determine if Task.CallStacks view is supported.
     *  If `clientVers` is 2 or higher, that view is reported in the list of
     *  views. The versions of iar.rov.server in SYS/BIOS 6.83 and later work
     *  only with DLLs that support Task.CallStacks, so this parameter is not
     *  relevant anymore.
     */
    config UInt clientVers = 3;

    /*
     *  ======== startModel ========
     *  Starts the ROV model given an executable and a coredump file.
     *
     *  This method does all of the following, using the modules and classes
     *  appropriate for a iar server running from the command line.
     *    - Creates the symbol table.
     *    - Creates the memory reader.
     *    - Locates and loads the ROV recap file.
     *    - Passes all of these to xdc.rov.Model.start so that ROV is ready
     *      to be polled.
     *
     *  The returned string is empty or undefined, if there is no error to
     *  report.
     */
    String startModel(String executable);

    /*
     *  ======== retrieveData ========
     *  Retrieve ROV data for the specified module and tab
     *
     *  @params(modTab) String value containing module and tab name seperated
     *                  by a whitespace
     *
     *  Returns a formatted string of ROV data
     *
     *  Format of the string:
     *
     *       COL Column1 SEP Column2 SEP Column3.... COL
     *       ROW Row1    SEP Row2    SEP Row3....    ROW
     *
     *  In case of Error, the format of the string:
     *
     *       ERR ErrData ERR
     *
     *  Where the symbols:
     *       COL = '\x03'
     *       ERR = '\x05'
     *       ROW = '\x12'
     *       SEP = '\x13'
     *
     *  Note: The whitespaces in the above decription of string format is to
     *        decribe the format. The actually string will not contain
     *        these whitespaces
     */
    String retrieveData(String modTab);

    /*
     *  ======== retrieveModuleList ========
     *  Retrieve the list of the modules
     *
     *  Returns a formatted string containing a list of modules and its views.
     *
     *  The format of string:
     *
     *       ROW mod1 SEP view1 TAB view2 TAB ... TAB ROW mod2 SEP view1 TAB \
     *       ... TAB ROW
     *
     *  Where the symbols:
     *       ROW = '\x12'
     *       SEP = '\x13'
     *       TAB = '\x14'
     *
     *  Note: The whitespaces in the above decription of string format is to
     *        decribe the format. The actually string will not contain
     *        these whitespaces
     */
    String retrieveModuleList();

    /*
     *  ======== retrieveLimitedList ========
     *  @_nodoc
     *  Retrieve the list of the modules and the table views only
     *
     *  The purpose of this function is to return a limited list of views for
     *  the version of the IAR ROV plugin that can only handle table views.
     *  Such plugins send '-l' command, which then is forwarded to this
     *  function. Newer plugins are sending nodoc'ed '--list_all', but after we
     *  don't need to support older plugins, both '-l' and '--list_all' will be
     *  forwarded to `retrieveModuleList`.
     */
    String retrieveLimitedList();
}
