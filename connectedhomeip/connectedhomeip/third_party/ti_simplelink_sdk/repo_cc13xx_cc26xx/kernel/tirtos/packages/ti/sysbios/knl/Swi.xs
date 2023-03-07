/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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
 *  ======== Swi.xs ========
 *
 *
 */

var Queue = null;
var Swi = null;
var BIOS = null;

/*
 * ======== getCFiles ========
 * return the array of C language files associated
 * with targetName (ie Program.build.target.$name)
 */
function getCFiles(targetName)
{
    if (BIOS.smpEnabled) {
        return (["Swi_smp.c"]);
    }
    else {
        return (["Swi.c"]);
    }
}

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    /* provide getCFiles() for Build.getCFiles() */
    this.$private.getCFiles = getCFiles;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Swi = this;

    Queue = xdc.useModule("ti.sysbios.knl.Queue");
    BIOS = xdc.useModule("ti.sysbios.BIOS");

    if (!(BIOS.libType == BIOS.LibType_Custom && BIOS.logsEnabled == false)) {
        xdc.useModule('xdc.runtime.Log');
    }
    if (!(BIOS.libType == BIOS.LibType_Custom
        && BIOS.assertsEnabled == false)) {
        xdc.useModule('xdc.runtime.Assert');
    }
    xdc.useModule("ti.sysbios.hal.Hwi");
    xdc.useModule("ti.sysbios.knl.Intrinsics");

    if (BIOS.taskEnabled == true) {
        xdc.useModule("ti.sysbios.knl.Task");
        Swi.taskDisable = '&ti_sysbios_knl_Task_disable__E';
        Swi.taskRestore = '&ti_sysbios_knl_Task_restore__E';
    }
    else {
        Swi.taskDisable = null;
        Swi.taskRestore = null;
    }

    /*
     * Minimize footprint by only building 1 readyQ if Swi is disabled.
     * 1 is used rather than 0 to prevent various .xs file references to
     * "Swi.numPriorities - 1" from generating warnings/errors.
     */
    if (BIOS.swiEnabled == false) {
        Swi.numPriorities = 1;
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    mod.locked = 1;
    mod.curSet = 0;
    mod.curTrigger = 0;
    mod.curSwi = null;
    mod.curQ = null;

    mod.readyQ.length = Swi.numPriorities;
    for (var i = 0; i < Swi.numPriorities; i++) {
        Queue.construct(mod.readyQ[i]);
    }
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, fxn, params)
{
    var mod = this.$module.$object;

    if (BIOS.swiEnabled == false) {
        Swi.$logFatal("Can't create a Swi with BIOS.swiEnabled == false.", this);
    }

    if (params.priority >= Swi.numPriorities) {
        Swi.$logFatal("Swi priority (" + params.priority + 
                        ") must be less than Swi.numPriorities (" + 
                        Swi.numPriorities + ").", this, "priority");
    }

    Queue.elemClearMeta(obj.qElem);

    obj.fxn = fxn;
    obj.arg0 = params.arg0;
    obj.arg1 = params.arg1;
    if (params.priority == ~0) {
        obj.priority = Swi.numPriorities - 1;
    }
    else {
        obj.priority = params.priority;
    }
    obj.posted = 0;
    obj.mask = 1 << obj.priority;
    obj.readyQ = mod.readyQ[obj.priority];
    obj.initTrigger = obj.trigger = params.trigger;

    /*
     * the following assignment sets the number of entries in the
     * hookEnv pointer array pointed to by the obj.hookEnv ptr.
     */
    obj.hookEnv.length = Swi.hooks.length;

    /* add constructed Swis to constructedSwis array */
    if (this.$category == "Struct") {
        mod.constructedSwis.$add(this);
        Swi.numConstructedSwis += 1;
    }
}

/*
 *  ======== addHookSet ========
 */
function addHookSet(hookSet)
{
    /* use "===" so 'null' is not flagged */
    if (hookSet.registerFxn === undefined) {
        hookSet.registerFxn = null;
    }
    if (hookSet.createFxn === undefined) {
        hookSet.createFxn = null;
    }
    if (hookSet.readyFxn === undefined) {
        hookSet.readyFxn = null;
    }
    if (hookSet.beginFxn === undefined) {
        hookSet.beginFxn = null;
    }
    if (hookSet.endFxn === undefined) {
        hookSet.endFxn = null;
    }
    if (hookSet.deleteFxn === undefined) {
        hookSet.deleteFxn = null;
    }

    this.hooks.$add(hookSet);
}

/*
 *  ======== module$validate ========
 *  some redundant tests are here to catch changes since
 *  module$static$init() and instance$static$init()
 */
function module$validate()
{
    if (Swi.numPriorities > BIOS.bitsPerInt) {
        Swi.$logError("Swi.numPriorities (" + 
                        Swi.numPriorities + 
                        ") can't be greater " +
                        "than the number of " +
                        "bits in an integer (" +
                        BIOS.bitsPerInt + ")",
                        Swi, "numPriorities");
    }

    /* validate all "created" instances */
    for (var i = 0; i < Swi.$instances.length; i++) {
        instance_validate(Swi.$instances[i]);
    }
 
    /* validate all "constructed" instances */
    for (var i = 0; i < Swi.$objects.length; i++) {
        instance_validate(Swi.$objects[i]);
    }
}

/*
 *  ======== instance_validate ========
 *  common function to test instance configuration
 */
function instance_validate(instance)
{
    if (instance.$object.fxn == null) {
        Swi.$logError("function cannot be null", instance);
    }
    if (instance.priority >= Swi.numPriorities) {
        Swi.$logError("Swi priority (" + 
                instance.priority + 
                ") must be less than Swi.numPriorities (" + 
                Swi.numPriorities + ").", instance, "priority");
    }
}

/*
 *  ======== viewCheckForNullObject ========
 *  Returns true if the object is all zeros.
 */
function viewCheckForNullObject(mod, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var objSize = mod.Instance_State.$sizeof();

    /* skip uninitialized objects */
    try {
        var objArray = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt8',
                                    isScalar: true},
                                    Number(obj.$addr),
                                    objSize,
                                    true);
    }
    catch(e) {
        print(e.toString());
    }

    for (var i = 0; i < objSize; i++) {
        if (objArray[i] != 0) return (false);
    }

    return (true);
}

/*
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' Task instance view.
 */
function viewInitBasic(view, obj)
{    
    var Swi = xdc.useModule('ti.sysbios.knl.Swi');
    var Program = xdc.useModule('xdc.rov.Program');

    if (viewCheckForNullObject(Swi, obj)) {
        view.label = "Uninitialized Swi object";
        return;
    }

    view.label = Program.getShortName(obj.$label);
    view.priority = obj.priority;

    /* Validate priority */
    var modCfg = Program.getModuleConfig(Swi.$name);
    if (obj.priority > modCfg.numPriorities) {
        view.$status["priority"] = "Corrupted data: Swi priority is greater" +
                                   "than Swi.numPriorities";
    }

    /* Scan the module view to determine the current running Swi. */
    try {
        var modView = Program.scanModuleView('ti.sysbios.knl.Swi', 'Module');
    }
    catch (e) {
        view.$status["state"] = "Error scanning module view to get current Swi.";
        return;
    }

    /* Set the Swi's mode */
    if ((Number(modView.currentSwi) == Number(obj.$addr))) {
        view.state = "Running";
    }
    else {
        if (obj.posted) {
            view.state = "Posted";
        }
        else {
            view.state = "Idle";
        }
    }

    /*
     * There may be more than one symbol at this address, so the function
     * name has to be an array.
     */
    view.fxn = Program.lookupFuncName(Number(obj.fxn));

    view.arg0 = obj.arg0;
    view.arg1 = obj.arg1;

    view.initTrigger = obj.initTrigger;
    view.curTrigger = obj.trigger;
}

/*
 *  ======== viewInitModule ========
 *  Initialize the Task module view.
 */
function viewInitModule(view, mod)
{
    view.schedulerState = (mod.locked ? "Locked" : "Unlocked");
    view.readyQMask = "0x" + mod.curSet.toString(16);
    view.currentSwi = mod.curSwi;
}

/*
 *  ======== viewInitReadyQs ========
 *  Generate the Swi 'ReadyQs' view.
 */
function viewInitReadyQs()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var BIOS = xdc.useModule('ti.sysbios.BIOS');
    var BIOSCfg = Program.getModuleConfig(BIOS.$name);

    var Swi = xdc.useModule('ti.sysbios.knl.Swi');
    var SwiCfg = Program.getModuleConfig(Swi.$name);

    var qSets, numPris;

    numPris = SwiCfg.numPriorities;

    var viewNode = new Program.TreeNode();

    /* not displayed!!! */
    viewNode.label = "Priorty Queues";

    viewNode.children = new Array();

    try {
        var swiRawView = Program.scanRawView('ti.sysbios.knl.Swi');
    }
    catch (e) {
        return null;
    }

    var readyQs = swiRawView.modState.readyQ;

    viewGetReadyQs(viewNode, readyQs, 0, numPris, 0);

    return (viewNode.children);
}

/*
 *  ======== viewGetReadyQs ========
 *  add all swis on this readyQ
 */
function viewGetReadyQs(priorityNodes, readyQs, coreId, numPris, affinity)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Swi = xdc.useModule('ti.sysbios.knl.Swi');
    var Queue = xdc.useModule('ti.sysbios.knl.Queue');

    var childNum;

    for (var priority = 0; priority < numPris; priority++) {
        /* calculate readyQ head address */
        var qAddr = readyQs.$addr + Queue.Elem.$sizeof() * (coreId*numPris + priority);

        var qObj = Program.fetchStruct(Queue.Instance_State$fetchDesc, qAddr);

        var elem = Program.fetchStruct(Queue.Elem$fetchDesc, qObj.elem.$addr);

        // print(qAddr, qObj.$addr, elem.$addr, elem.next);

        if (Number(elem.next) != Number(qAddr)) {

            childNum = priorityNodes.children.length++;
            priorityNodes.children[childNum].label = "priority " + priority;
            priorityNodes.children[childNum].properties = new Array();

            var swis = new Array();

            /*
             * To check for loops, store each address as a key in a map,
             * then with each new address check first if it is already in
             * the map.
             */
            var addrs = {};

            var prevElemAddr;

            while (Number(elem.next) != Number(qAddr)) {

                // print("Swi: ", elem.next);

                var swiElemView = Program.newViewStruct('ti.sysbios.knl.Swi', 'ReadyQs');
                swis[swis.length++] = swiElemView;

                swiElemView.swi = elem.next;

                /* get a Basic view of this swi */
                try {
                    var swiView = Program.scanHandleView('ti.sysbios.knl.Swi', elem.next, 'Basic');
                }
                catch (e) {
                    swiElemView.$status["Address"] = "Bad Swi object address: " +
                                                Number(elem.next).toString(16) +
                                                ": " + e.toString();
                    break;
                }

                /* get this swi's raw structure too */
                var swi = Program.fetchStruct(Swi.Instance_State$fetchDesc, elem.next);

                swiElemView.readyQ = swi.readyQ;
                swiElemView.label = swiView.label;
                swiElemView.priority = swiView.priority;
                swiElemView.state = swiView.state;
                swiElemView.fxn = swiView.fxn;
                swiElemView.arg0 = swiView.arg0;
                swiElemView.arg1 = swiView.arg1;

                if (Number(swiElemView.readyQ) != Number(qAddr)) {
                    swiElemView.$status["readyQ"] = "Swi readyQ: " +
                        Number(swiElemView.readyQ).toString(16) +
                        " doesn't match this readyQ address: " +
                        Number(qAddr).toString(16);
                }

                if (swiElemView.priority != priority) {
                    swiElemView.$status["priority"] = "Swi priority: " +
                        swiElemView.priority +
                        " doesn't match readyQ priority: " + priority;
                }

                if (swiElemView.state != "Posted") {
                    swiElemView.$status["state"] = "Swi state: " +
                        swiElemView.state +
                        " should be Posted.";
                }

                prevElemAddr = elem.$addr;

                elem = Program.fetchStruct(Queue.Elem$fetchDesc, elem.next);

                swiElemView.next = elem.next;
                swiElemView.prev = elem.prev;

                if (Number(elem.prev) != Number(prevElemAddr)) {
                    swiElemView.$status["prev"] = "Previous swi pointer: " +
                        Number(elem.prev.$addr).toString(16) +
                        " doesn't point to previous swi: " +
                        Number(prevElemAddr).toString(16);
                }

                /* Before fetching the next element, verify we're not in a loop. */
                if (elem.next in addrs) {
                    swiElemView.$status["next"] = "Queue contains loop. Element: " +
                        elem.$addr + " points to element: " + elem.next;
                    break;
                }

                prevElemAddr = elem.prev.$addr;

                /*
                 * Add the address to a map so we can check for loops.
                 * The value 'true' is just a placeholder to make sure the address
                 * is in the map.
                 */
                addrs[elem.$addr] = true;
            }

            priorityNodes.children[childNum].properties = swis;
        }
    }
}
