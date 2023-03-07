/* --COPYRIGHT--,EPL
 *  Copyright (c) 2012-2020 Texas Instruments Incorporated
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * --/COPYRIGHT--*/
package iar.rov.server;

/*!
 *  ======== Main.xdc ========
 *  The iar.rov.server command line utility provides ROV information
 *
 *  Commands:
 *
 *  Start ROV -
 *      xs iar.rov.server -e "executable"
 *
 *      xs iar.rov.server -e "/path/to/file/App.exe"
 *
 *  Get a list of Modules and Tabs
 *      xs iar.rov.server -l
 *
 *  Get a Module Tab View -
 *
 *      xs iar.rov.server -m "Module Tab"
 *
 *      xs iar.rov.server -m "ti.sysbios.knl.Task Basic"
 *
 */
metaonly module Main inherits xdc.tools.ICmd {

    override config String usage[] = [
        ' ',
        'Usage',
        '[-e executable]',
        '[--clientVersion <N>]',
        '[-m Module Tab]',
        '[-l]',
        '[--help]',
        ' ',
    ];

instance:

    //!Module name followed by Tab name
    @CommandOption('m')
    config String modTab = "";

    //!Executable
    @CommandOption('e')
    config String executable = "";

    //!List Modules
    @CommandOption('l')
    config Bool listMod = false;

    /*!
     *  ======== list_all ========
     *  @_nodoc
     *  List all modules and views
     */
    @CommandOption('list_all')
    config Bool listAll = false;

    /*!
     *  ======== clientVersion ========
     *  The flag that signals client's capabilities
     *
     *  Older DLLs weren't able to properly display Task.CallStacks view. The
     *  clients of the version 2 and higher can display that view.
     */
    @CommandOption('clientVersion')
    config UInt clientVersion = 3;
}
