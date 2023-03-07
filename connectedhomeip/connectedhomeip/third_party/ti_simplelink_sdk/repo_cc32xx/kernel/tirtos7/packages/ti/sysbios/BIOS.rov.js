/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 */

/*
 *  ======== BIOS.rov.js ========
 */

var moduleName = "ti.sysbios::BIOS";

var viewMap = [
    {name: "Module", fxn: "viewBIOSModule", structName: "ModuleInfo"},
    {name: "Scan for errors...", fxn: "viewInitErrorScan", structName: "ErrorView"}
];

/*
 * BIOS Module Information
 */
function ModuleInfo()
{
    this.currentThreadType = null;
    this.rtsGateType = null;
    this.cpuFreqLow = 0;
    this.cpuFreqHigh = 0;
    this.disableClock = null;
    this.disableSwi = null;
    this.disableTask = null;
    this.startFunc = null;
}

function ErrorView()
{
    this.mod = null;
    this.tab = null;
    this.inst = null;
    this.field = null;
    this.message = null;
}

/*
 * ======== toHexString ========
 * converts a number to a hex string with "0x" prefix.
 */
function toHexString(num) {
    return ("0x" + Number(num).toString(16));
}

/*
 * ======== toPaddedHexString ========
 * converts a number to a hex string with "0x" prefix.
 * inserts up to (len - 1) leading zeros.
 * up to a max of 7 zeros.
 */
function toPaddedHexString(number, len)
{
    return ("0x" + ("0000000" + number.toString(16)).substr(-len));
}

/*
    enum ThreadType {
        ThreadType_Hwi,
        ThreadType_Swi,
        ThreadType_Task,
        ThreadType_Main
    }; */

/*
    enum RtsLockType {
        NoLocking,
        GateHwi,
        GateSwi,
        GateMutex,
        GateMutexPri
    }; */

/*
 *  ======== viewBIOSModule ========
 */
function viewBIOSModule()
{
    var modView = new ModuleInfo();

    var modState = Program.fetchVariable("BIOS_Module_state");

    switch (modState.threadType) {
        case 0:
            modView.currentThreadType = "Hwi";
            break;
        case 1:
            modView.currentThreadType = "Swi";
            break;
        case 2:
            modView.currentThreadType = "Task";
            break;
        case 3:
            modView.currentThreadType = "Main";
            break;
    }

    switch (modState.rtsGateType) {
        case 0:
            modView.rtsGateType = "NoLocking";
            break;
        case 1:
            modView.rtsGateType = "GateHwi";
            break;
        case 2:
            modView.rtsGateType = "GateSwi";
            break;
        case 3:
            modView.rtsGateType = "GateMutex";
            break;
        case 4:
            modView.rtsGateType = "GateMutexPri";
            break;
    }

    modView.cpuFreqLow = modState.cpuFreq.lo;
    modView.cpuFreqHigh = modState.cpuFreq.hi;
    modView.clockEnabled = (modState.enables & 4) ? "true" : "false";
    modView.swiEnabled = (modState.enables & 2) ? "true" : "false";
    modView.taskEnabled = (modState.enables & 1) ? "true" : "false";
    modView.startFunc = String(Program.lookupFuncName(Number(modState.startFunc)));

    return (modView);
}


/*
 *  ======== viewInitErrorScan ========
 */
function viewInitErrorScan()
{
    var view = new Array();

    /* Run every single ROV view to gather any errors. */
    scanAllViews();

    /*
     * Get ROV's table of status messages (built up while the views
     * were run).
     */
    var statusTab = Program.getStatusTable();

    /* Display all of the entries in the status table. */
    for each (var statusElem in statusTab) {
        displayNewStatus(view, statusElem);
    }

    /*
     * If no errors were found, display something positive to let them no
     * it was successful.
     */
    if (view.length == 0) {
        displayNewError(view, "BIOS", "Scan for errors...", "N/A", "N/A",
                        "All ROV views have been run and no errors were " +
                        "encountered. In BIOS, this includes a check of the " +
                        "System stack usage and all Task stacks.");
    }

    return (view);
}

/*
 *  ======== scanAllViews ========
 */
function scanAllViews()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var ViewInfo = xdc.useModule('xdc.rov.ViewInfo');

    /* For every module in the system... */
    for each (var modName in Program.moduleNames) {
        /* Get the module descriptor object. */

        if ((modName == "xdc.rov.runtime.Monitor") ||
            (modName == "xdc.runtime.System")) {
            continue;
        }

        var modDesc = Program.getModuleDesc(modName);

        /*
         * Skip over modules which failed to load (probably missing from
         * package path).
         */
        if (modDesc.loadFailed) {
            continue;
        }

        /* Get the list of views supported by this module. */
        var tabs = Program.getSupportedTabs(modName);

        /* For each of the module's tabs... */
        for each (var tab in tabs) {


            /* Don't scan ourselves or we'll end up in a loop! */
            if (tab.name == "Scan for errors...") {
                continue;
            }

            /*
             * Call the appropriate scan function on the tab and catch any
             * exceptions thrown.
             */
            try {
                switch (String(tab.type)) {
                    case String(ViewInfo.MODULE):
                        Program.scanModuleView(modName, tab.name);
                        break;
                    case String(ViewInfo.INSTANCE):
                        Program.scanInstanceView(modName, tab.name);
                        break;
                    case String(ViewInfo.INSTANCE_DATA):
                        Program.scanInstanceDataView(modName, tab.name);
                        break;
                    case String(ViewInfo.MODULE_DATA):
                        Program.scanModuleDataView(modName, tab.name);
                        break;
                    case String(ViewInfo.RAW):
                        Program.scanRawView(modName);
                        break;
                    case String(ViewInfo.TREE_TABLE):
                        Program.scanTreeTableView(modName, tab.name);
                        break;
                    case String(ViewInfo.TREE):
                        Program.scanTreeView(modName, tab.name);
                        break;
                    default:
                        Program.print("Undefined view type: " + String(tab.type));
                        break;
                }
            }
            catch (e) {
            }
        }
    }
}

/*
 *  ======== displayNewStatus ========
 *  Display a new entry in the ROV 'Scan For Errors' table based on the
 *  provided ROV status object.
 */
function displayNewStatus(elements, status)
{
    displayNewError(elements, status.mod, status.tab, status.inst,
                    status.field, status.message);
}

/*
 *  ======== displayNewError ========
 *  Display a new entry in the ROV 'Scan For Errors' table with the provided
 *  information.
 */
function displayNewError(view, modName, tabName, inst, field, msg)
{
    var Program = xdc.useModule('xdc.rov.Program');

    var elem = new ErrorView();

    elem.mod = modName;
    elem.tab = tabName;
    elem.inst = inst;
    elem.field = field;
    elem.message = msg;
    view.push(elem);
}
