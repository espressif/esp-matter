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
 *  ======== Task.rov.js ========
 */

var moduleName = "ti.sysbios.knl::Task";

var viewMap = [
    {name: "Basic", fxn: "viewTaskBasic", structName: "BasicInfo"},
    {name: "Detailed", fxn: "viewTaskDetailed", structName: "DetailedInfo"},
    {name: "Call Stack", fxn: "viewInitCallStack", structName: "CallStackFrame"},
    {name: "Module", fxn: "viewTaskModule", structName: "ModuleInfo"}
];

/*
 * Task Object Basic Information
 */
function BasicInfo()
{
    this.address = null;
    this.symbol = null;
    this.name = null;
    this.priority = 0;
    this.mode = null;
    this.fxn = null;
    this.arg0 = null;
    this.arg1 = null;
    this.stackSize = null;
    this.stackBase = null;
}

/*
 * Task Object Detailed Information
 */
function DetailedInfo()
{
    this.address = null;
    this.symbol = null;
    this.name = null;
    this.priority = 0;
    this.mode = null;
    this.fxn = null;
    this.arg0 = null;
    this.arg1 = null;
    this.stackPeak = null;
    this.stackSize = null;
    this.stackBase = null;
    this.blockedOn = null;
}

/*
 * Task Module Information
 */
function ModuleInfo()
{
    this.address = null;
    this.schedulerState = null;
    this.readyQMask = null;
    this.workPending = null;
    this.numVitalTasks = 0;
    this.currentTask = null;
    this.hwiStackPeak = null;
    this.hwiStackSize = null;
    this.hwiStackBase = null;
}

/*
 * CallStackFrame constructor
 */
function CallStackFrame()
{
    this.address = null;
    this.frame = null;
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
 *  ======== viewGetTaskObjectArray ========
 *  returns an array of the current task objects.
 */
function viewGetTaskObjectArray()
{
    taskArray = new Array();

    try {
        var modState = Program.fetchVariable("ti_sysbios_knl_Task_Module_state");
    }
    catch (e) {
        return (taskArray);
    }
    var modStateAddr = Program.lookupSymbolValue("ti_sysbios_knl_Task_Module_state");
    var taskStructType = Program.lookupType("ti_sysbios_knl_Task_Struct");
    var offsetOfObjElem = taskStructType.member.objElem.offset;
    var next = modState.objQ.next;
    var tskAddr;

    while (Number(next) != Number(modStateAddr)) {
        tskAddr = next - offsetOfObjElem;
        var task = new Object();
        task = Program.fetchFromAddr(tskAddr, "ti_sysbios_knl_Task_Struct", 1);
        task.$addr = tskAddr;
        taskArray.push(task);
        next = task.objElem.next;
    }

    return (taskArray);
}

/*
 *  ======== viewInitBasic ========
 */
function viewInitBasic(view, obj)
{
    try {
        var numPriorities = Program.fetchVariable("ti_sysbios_knl_Task_numPriorities");
    }
    catch (e) {
        /* assume default if app doesn't reference Task_numPriorities */
        var numPriorities = 16;
    }

    view.address = toHexString(obj.$addr);
    view.symbol = String(Program.lookupDataSymbol(Number(obj.$addr)));
    /*
     *  In case the dynamically created object was the first thing in the
     *  BIOS_heap.
     */
    if (view.symbol == "BIOS_heap") {
        view.symbol = "";
    }
    if (Number(obj.name) != 0) {
        view.name = Program.fetchString(Number(obj.name));
    }
    view.fxn = String(Program.lookupFuncName(Number(obj.fxn)));
    view.arg0 = toHexString(obj.arg0);
    view.arg1 = toHexString(obj.arg1);
    if (obj.priority < -1 || (obj.priority > numPriorities - 1)) {
        Program.displayError(view, "priority",
        "Corrupted data: Task priority is greater than Task.numPriorities");
    }
    view.priority = obj.priority;
    view.stackBase = toHexString(obj.stack);
    view.stackSize = obj.stackSize;
    viewGetTaskMode(view, obj);
}

/*
 *  ======== viewTaskBasic ========
 */
function viewTaskBasic()
{
    var view = new Array();

    var tasks = viewGetTaskObjectArray();

    for (var i = 0; i < tasks.length; i++) {
        var obj = tasks[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}

/*
 *  ======== viewTaskDetailed ========
 */
function viewTaskDetailed()
{
    var view = new Array();

    var tasks = viewGetTaskObjectArray();

    for (var i = 0; i < tasks.length; i++) {
        var obj = tasks[i];
        var detailedView = new DetailedInfo();
        viewInitBasic(detailedView, obj);
        viewGetStackDepth(detailedView);
        if ((Number(obj.context) < Number(obj.stack)) ||
             (Number(obj.context) > Number(obj.stack) + Number(obj.stackSize))) {
            Program.displayError(detailedView, "stackPeak", "SP outside stack!")            
        }

        viewGetBlockedOnObject(detailedView, obj);
        view.push(detailedView);
    }

    return (view);
}

/*
 *  ======== viewTaskModule ========
 */
function viewTaskModule()
{
    var modView = new ModuleInfo();

    try {
        var modState = Program.fetchVariable("ti_sysbios_knl_Task_Module_state");
    }
    catch (e) {
        return (modView);
    }

    modView.address = toHexString(Program.lookupSymbolValue("ti_sysbios_knl_Task_Module_state"));
    modView.schedulerState = modState.locked ? "Locked" : "Unlocked";
    modView.readyQMask = toPaddedHexString(modState.curSet, 8);
    modView.workPending = modState.workFlag ? "true" : "false";
    modView.numVitalTasks = modState.vitalTasks;
    modView.currentTask = toHexString(modState.curTask);

    var hwiStackInfo = Hwi_viewGetHwiStackInfo();

    modView.hwiStackPeak = hwiStackInfo.hwiStackPeak;
    modView.hwiStackSize = hwiStackInfo.hwiStackSize;
    modView.hwiStackBase = hwiStackInfo.hwiStackBase;

    if (hwiStackInfo.hwiStackPeak == hwiStackInfo.hwiStackSize) {
        Program.displayError(modView, "hwiStackPeak", "Hwi Stack Overrun!");
    }

    return (modView);
}

/* Task Mode
enum Task_Mode {
    Task_Mode_RUNNING,      0
    Task_Mode_READY,        1
    Task_Mode_BLOCKED,      2
    Task_Mode_TERMINATED,   3
    Task_Mode_INACTIVE      4
};
*/

/* BIOS ThreadType
enum BIOS_ThreadType {
    BIOS_ThreadType_Hwi,    0
    BIOS_ThreadType_Swi,    1
    BIOS_ThreadType_Task,   2
    BIOS_ThreadType_Main    3
};
*/

function viewGetCurrentThreadType()
{
    var biosModState = Program.fetchVariable("BIOS_Module_state");

    switch (biosModState.threadType) {
        case 0: return ("Hwi");
        case 1: return ("Swi");
        case 2: return ("Task");
        case 3: return ("Main");
    }
}

/*!
 *  ======== viewGetTaskMode ========
 */
function viewGetTaskMode(view, obj)
{
    var modState = Program.fetchVariable("ti_sysbios_knl_Task_Module_state");
    var biosModState = Program.fetchVariable("BIOS_Module_state");
    var mode = obj.mode;

    if (obj.priority == -1) {
        mode = 4;
    }

    if (Number(modState.curTask) == (view.address)) mode = 0;

    switch (mode) {
        case 0:
            if (biosModState.threadType == 2) {
                view.mode = "Running";
            }
            else {
                view.mode = "Preempted";
            }
            break;
        case 1:
            view.mode = "Ready";
            break;
        case 2:
            view.mode = "Blocked";
            break;
        case 3:
            view.mode = "Terminated";
            break;
        case 4:
            view.mode = "Inactive";
            break;
        default:
            Program.displayError(view, "mode", "Invalid mode: " + mode);
    }
}

/*!
 *  ======== viewGetStackDepth ========
 */
function viewGetStackDepth(view)
{
    var stackData = Program.fetchFromAddr(Number(view.stackBase), "uint32_t", Number(view.stackSize/4));

    var index = 0;

    /*
     * The stack is filled with 0xbe.
     */
    while (stackData[index] == 0xbebebebe) {
        index++;
    }

    view.stackPeak = view.stackSize - index*4;

    if (view.stackPeak == view.stackSize) {
        Program.displayError(view, "stackPeak", "Stack over-flow!");
    }
}

/* pull in TaskSupport ROV functions */
var TaskSupport = xdc.loadCapsule("../family/arm/m3/TaskSupport.rov.js");

/*
 *  ======== viewInitCallStack ========
 *  Generate the Task 'CallStack' view.
 */
function viewInitCallStack()
{
    var view = new Array();

    var tasks = viewGetTaskObjectArray();

    for (var a = 0; a < tasks.length; a++) {
        var obj = tasks[a];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);

        var bts = TaskSupport.viewGetTaskCallStack(obj, basicView.mode, viewGetCurrentThreadType());
        /*
         * Call stack text returned from getCallStack is one long string
         * which must be split it into separate frames and massaged.
         */
        var frames = new Array();
        frames = bts.split("\n");

        /*
         * frames[0] contains diagnostic info from the delegate
         * that is not meant to be displayed except to assist
         * in debugging this view code.
         */

        /*
         * Strip off "Unwind halted ... " from last frame
         *
         * If length = 2, then frames[1] probably says:
         *   "call stack parsing is not supported"
         */
        if (frames.length >= 2) {
            frames.length -= 1;
        }

        for (var i = 1; i < frames.length; i++) {
            var line = frames[i];
            /* separate frame # from frame text a little */
            line = line.replace(" ", "    ");
            var file = line.substr(line.indexOf(" at ") + 4);
            file = file.replace(/\\/g, "/");
            file = file.substr(file.lastIndexOf("/")+1);
            if (file != "") {
                frames[i] = line.substring(0,
                                       line.indexOf(" at ") + 4);
                /* tack on file info */
                frames[i] += file;
            }
            /* halt back trace at TaskSupport_glue() */
            if (frames[i].match(/TaskSupport_glue/)) {
                frames.length = i+1;
            }
        }

        var nickName = basicView.fxn;

        /*
         * Invert the frames[] array so that the strings become the index of a
         * new associative array.
         *
         * This is done because the TREE view renders the array index (field)
         * on the left and the array value on the right.
         *
         * At the same time, extract the "PC = ..." substrings and make them
         * be the value of the array who's index is the beginning of the
         * frame text.
         */
        var invframes = new Array();

        for (var i = 1; i < frames.length; i++) {
            invframes[frames[i].substring(0, frames[i].indexOf("PC")-1)] =
                frames[i].substr(frames[i].indexOf("PC"));
        }

        var header = new CallStackFrame();
        header.address = toHexString(obj.$addr);
        header.frame =  nickName + ",  Task Mode: " +
             basicView.mode;
        view.push(header);

        for (var x in invframes) {
            var stack = new CallStackFrame();
            stack.frame = " " + x + invframes[x];
            view.push(stack);
        }

        var footer = new CallStackFrame();
        footer.frame = " ";
        view.push(footer);
    }

    return (view);
}

/*!
 *  ======== viewGetBlockedOnObject ========
 */
function viewGetBlockedOnObject(view, task)
{
    if (view.mode != "Blocked") return;

    var pendElem = task.pendElem;

    if ((pendElem < task.stack) ||
        (pendElem > (task.stack + task.stackSize))) {
        view.blockedOn = "Internal Error";
        Program.displayError(view, "blockedOn", "Invalid task internal state: "
        + "pend element address (0x"
        + Number(pendElem).toString(16)
        + ") is not within the task's stack");
        return;
    }

    /* Check objects with embedded Semaphores before Semaphores */
    if (checkMailboxes(view, task) == true) return;
    if (checkGateMutexes(view, task) == true ) return;
    if (checkGateMutexPris(view, task) == true ) return;
    if (checkSemaphores(view, task) == true) return;
    if (checkEvents(view, task) == true ) return;
    if (checkTaskSleep(view, task) == true ) return;

    view.blockedOn = "Unknown";
}

/* pull in Mailbox ROV functions */
var Mailbox = xdc.loadCapsule("Mailbox.rov.js");

/*!
 *  ======== checkMailboxes ========
 */
function checkMailboxes(view, task)
{
    var mailboxViews = Mailbox.viewMailboxDetailed();

    for (var i in mailboxViews) {
        if (checkPendQ(mailboxViews[i].pendQueue, task)) {
            view.blockedOn = "Mailbox_pend: 0x" + Number(mailboxViews[i].address).toString(16);
            return (true);
        }
        if (checkPendQ(mailboxViews[i].postQueue, task)) {
            view.blockedOn = "Mailbox_post: 0x" + Number(mailboxViews[i].address).toString(16);
            return (true);
        }
    }
    return (false);
}

/* pull in GateMutex ROV functions */
var GateMutex = xdc.loadCapsule("../gates/GateMutex.rov.js");

/*!
 *  ======== checkGateMutexes ========
 */
function checkGateMutexes(view, task)
{
    var gateMutexViews = GateMutex.viewGateMutexBasic();

    for (var i in gateMutexViews) {
        if (checkPendQ(gateMutexViews[i].pendedTasks, task)) {
            view.blockedOn = "GateMutex: 0x" + Number(gateMutexViews[i].address).toString(16);
            return (true);
        }
    }

    return (false);
}

/* pull in GateMutexPri ROV functions */
var GateMutexPri = xdc.loadCapsule("../gates/GateMutexPri.rov.js");

/*!
 *  ======== checkGateMutexPris ========
 */
function checkGateMutexPris(view, task)
{
    var gateMutexPriViews = GateMutexPri.viewGateMutexPriBasic();

    for (var i in gateMutexPriViews) {
        if (checkPendQ(gateMutexPriViews[i].pendedTasks, task)) {
            view.blockedOn = "GateMutexPri: 0x" + Number(gateMutexPriViews[i].address).toString(16);
            return (true);
        }
    }

    return (false);
}

/* pull in Semaphore ROV functions */
var Semaphore = xdc.loadCapsule("Semaphore.rov.js");

/*!
 *  ======== checkSemaphores ========
 */
function checkSemaphores(view, task)
{
    var semaphoreViews = Semaphore.viewSemaphoreBasic();

    for (var i in semaphoreViews) {
        if (checkPendQ(semaphoreViews[i].pendedTasks, task)) {
            view.blockedOn = "Semaphore: 0x" + Number(semaphoreViews[i].address).toString(16);
            return (true);
        }
    }
    return (false);
}

/*
 *  ======== checkPendQ ========
 *  See if task is blocked in this pendQ
 */
function checkPendQ(pendQ, task)
{
    /* Compare each task in the pendedTasks array with us */
    for (var j = 0; j < pendQ.length; j++) {
        var taskAddr = pendQ[j].substr(6, 10);
        if (Number(taskAddr) == Number(task.$addr)) {
            return (true);
        }
    }
    return (false);
}

/* pull in Event ROV functions */
var Event = xdc.loadCapsule("Event.rov.js");

/*!
 *  ======== checkEvents ========
 */
function checkEvents(view, task)
{
    var eventViews = Event.viewEventBasic();

    for (var i in eventViews) {
        var taskAddr = eventViews[i].pendedTask.substr(6, 10);
        if (Number(taskAddr == Number(task.$addr))) {
            view.blockedOn = "Event: 0x" + Number(eventViews[i].address).toString(16);
            return (true);
        }
    }
    return (false);
}

/* pull in Clock ROV functions */
var Clock = xdc.loadCapsule("Clock.rov.js");

/*!
 *  ======== checkTaskSleep ========
 */
function checkTaskSleep(view, task)
{
    if (task.pendElem == 0) {
        return (false);
    }

    /* Get the element address from the Queue view. */
    var pendElemAddr = task.pendElem;

    /* Fetch the PendElem struct */
    try {
        var pendElem =
            Program.fetchFromAddr(pendElemAddr, "ti_sysbios_knl_Task_PendElem", 1);
    }
    catch (e) {
        Program.displayError(view, "blockedOn",
            "Problem with fetching Task pend element 0x" +
            Number(pendElemAddr).toString(16) + ": " + e.toString());
        return (false);
    }

    /* Check if 'clockHandle' is null. */
    if (Number(pendElem.clockHandle) == 0) {
        return (false);
    }

    try {
        var clockModState = Program.fetchVariable("ti_sysbios_knl_Clock_Module_state");
    }
    catch (e) {
        return (false);
    }
    var clockView = new Clock.BasicInfo;

    /* Otherwise, retrieve the view for the clock handle. */
    try {
        var clockObject =
            Program.fetchFromAddr(pendElem.clockHandle, "ti_sysbios_knl_Clock_Struct");
    }
    catch (e) {
        Program.displayError(view, "blockedOn",
            "Problem scanning pending Clock 0x" +
            Number(pendElem.clockHandle).toString(16) + ": " + e.toString());
        return (false);
    }

    clockObject.$addr = pendElem.clockHandle;
    clockObject.$ticks = clockModState.ticks;

    Clock.viewInitBasic(clockView, clockObject);

    view.blockedOn = "Task_sleep(" + clockView.tRemaining + ")";
    return (true);
}
