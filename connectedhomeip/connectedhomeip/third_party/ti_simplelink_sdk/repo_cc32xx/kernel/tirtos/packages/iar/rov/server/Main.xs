/* --COPYRIGHT--,EPL
 *  Copyright (c) 2012-2015 Texas Instruments Incorporated
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * --/COPYRIGHT--*/
/*
 *  ======== run ========
 *  Command based ROV server.
 */
function run(cmdr, args)
{
    var server = xdc.useModule('iar.rov.server.Server');
    var executable = trimStr(this.executable + "");
    var modTab    = trimStr(this.modTab + "");
    var listMod   = this.listMod;

    /* Start the ROV server */
    if (executable) {
        return (server.startModel(executable));
    }
    /* Retrieve Module Tab view */
    else if (modTab) {
        return (server.retrieveData(modTab));
    }
    else if (this.listAll) {
        return (server.retrieveModuleList());
    }
    /* Retrieve a list of Modules and Tabs */
    else if (listMod) {
        return (server.retrieveLimitedList());
    }
    else if (this.clientVersion) {
        server.clientVers = this.clientVersion;
        return (0);
    }
    /* Unknown command */
    else {
        return ("IAR ROV server command not found!");
    }
}

/*
 *  ======== trimStr ========
 *  Trim the string
 */
function trimStr(str)
{
    return (str.replace(/^\s+/g, '').replace(/\s+$/g, ''));
}
