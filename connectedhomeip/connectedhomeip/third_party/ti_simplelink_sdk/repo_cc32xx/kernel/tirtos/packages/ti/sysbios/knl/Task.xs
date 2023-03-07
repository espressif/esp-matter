/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 *  ======== Task.xs ========
 */

var Queue = null;
var Task = null;
var Idle = null;
var Program = null;
var BIOS = null;
var Memory = null;
var Settings = null;
var Clock = null;
var Core = null;
var Swi = null;
var Build = null;

/*
 * ======== getCFiles ========
 * return the array of C language files associated
 * with targetName (ie Program.build.target.$name)
 */
function getCFiles(targetName)
{
    if (BIOS.smpEnabled) {
        return (["Task_smp.c"]);
    }
    else {
        return (["Task.c"]);
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

    Task = this;
    Program = xdc.module('xdc.cfg.Program');

    if (Program.build.target.$name.match(/gnu/)) {
        Task.defaultStackSection = ".bss";
    }
    else if (Program.build.target.name.match(/C6.*/)) {
        Task.defaultStackSection = ".far:taskStackSection";
    }
    else if (Program.build.target.name.match(/C28.*/)) {
        if (Program.build.target.$name.match(/elf/)) {
            Task.defaultStackSection = ".bss:taskStackSection";
        }
        else {
            Task.defaultStackSection = ".ebss:taskStackSection";
        }
    }
    else {
        Task.defaultStackSection = ".bss:taskStackSection";
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    BIOS = xdc.module('ti.sysbios.BIOS');
    Queue = xdc.useModule("ti.sysbios.knl.Queue");
    Idle = xdc.useModule("ti.sysbios.knl.Idle");
    Settings = xdc.module("ti.sysbios.family.Settings");
    Build = xdc.module("ti.sysbios.Build");

    if (BIOS.smpEnabled == true) {
        Core = xdc.useModule("ti.sysbios.hal.Core");
    }

    Task.SupportProxy = xdc.module(Settings.getDefaultTaskSupportDelegate());

    /* only useModule(Memory) if needed */
    var Defaults = xdc.module('xdc.runtime.Defaults');
    if (Defaults.common$.memoryPolicy ==
        xdc.module("xdc.runtime.Types").STATIC_POLICY) {
        Memory = xdc.module('xdc.runtime.Memory');
    }
    else {
        Memory = xdc.useModule('xdc.runtime.Memory');
    }

    if (!(BIOS.libType == BIOS.LibType_Custom && BIOS.logsEnabled == false)) {
        xdc.useModule('xdc.runtime.Log');
    }
    if (!(BIOS.libType == BIOS.LibType_Custom
        && BIOS.assertsEnabled == false)) {
        xdc.useModule('xdc.runtime.Assert');
    }

    xdc.useModule("ti.sysbios.knl.Idle");
    xdc.useModule("ti.sysbios.knl.Intrinsics");
    xdc.useModule("ti.sysbios.knl.Task_SupportProxy");
    xdc.useModule("ti.sysbios.hal.Hwi");

    if (BIOS.clockEnabled == true) {
        Clock = xdc.useModule("ti.sysbios.knl.Clock");
    }

    if (BIOS.swiEnabled == true) {
        Swi = xdc.useModule("ti.sysbios.knl.Swi");
    }

    /* Minimize footprint by only building 1 readyQ if Task is disabled */
    if (BIOS.taskEnabled == false) {
        Task.numPriorities = 1;
    }

    /* Initialize default stack sizes from proxy */

    /* undefined defaultStackSize means use proxy's defaultStackSize */
    if (Task.defaultStackSize === undefined) {
        Task.defaultStackSize = Task.SupportProxy.defaultStackSize;
    }

    /* undefined defaultStackHeap means use null */
    if (Task.defaultStackHeap === undefined) {
        Task.defaultStackHeap = null;
    }

    /* undefined idleTaskStackSize means use defaultStackSize */
    if (Task.idleTaskStackSize === undefined) {
        Task.idleTaskStackSize = Task.defaultStackSize;
    }

    /* undefined idleTaskStackSection means use defaultStackSection */
    if (Task.idleTaskStackSection === undefined) {
        Task.idleTaskStackSection = Task.defaultStackSection;
    }

    if (BIOS.smpEnabled == false) {
        Task.defaultAffinity = 0;
    }

    /*
     * Default value of instance params 'affinity' is inherited from
     * Task.defaultAffinity
     */

    Task.PARAMS.affinity = Task.defaultAffinity;
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    mod.locked = true;
    mod.curSet = 0;
    mod.vitalTasks = 0;
    mod.workFlag = 0;
    mod.curTask = null;
    mod.curQ = null;

    Queue.construct(mod.inactiveQ);
    Queue.construct(mod.terminatedQ);

    if (BIOS.smpEnabled == true) {
        mod.smpCurTask.length = Core.numCores;
        mod.smpCurMask.length = Core.numCores;

        for (var coreNum = 0; coreNum < Core.numCores; coreNum++) {
            mod.smpCurTask[coreNum] = null;
            mod.smpCurMask[coreNum] = 0;
        }

        mod.smpCurSet.length = Core.numCores + 1;
        mod.smpReadyQ.length = Core.numCores + 1;

        for (var i = 0; i < (Core.numCores+1); i++) {
            mod.smpCurSet[i] = 0;      /* actual values set in Task_postInit() */
            mod.smpReadyQ[i] = null; /* filled in during Task_startup() */
        }

        mod.idleTask.length   = Core.numCores;

        mod.readyQ.length = Task.numPriorities * (Core.numCores + 1);
        for (var i = 0; i < mod.readyQ.length; i++) {
            Queue.construct(mod.readyQ[i]);
        }
    }
    else {
        mod.smpCurTask.length = 0;
        mod.smpCurMask.length = 0;
        mod.smpCurSet.length  = 0;
        mod.smpReadyQ.length  = 0;

        mod.idleTask.length   = 1;

        mod.readyQ.length = Task.numPriorities;
        for (var i = 0; i < mod.readyQ.length; i++) {
            Queue.construct(mod.readyQ[i]);
        }
    }

    /*
     * Capture deprecated ".taskStackSection" settings
     */
    if ((Program.sectMap[Task.defaultStackSection] === undefined) &&
        !(Program.sectMap[".taskStackSection"] === undefined)) {

        Task.$logWarning("The Task.defaultTaskSection has been " +
                         "changed to \"" + Task.defaultStackSection +
                         "\".  Please review your .cfg file for references " +
                         "to \".taskStackSection\" and replace those " +
                         "references with 'Task.defaultStackSection' or \"" +
                         Task.defaultStackSection + "\".", Task, "defaultTaskSection");

        Program.sectMap[Task.defaultStackSection] = new Program.SectionSpec();

        if (typeof(Program.sectMap[".taskStackSection"]) == "string") {
            /* value of sectMap entry is segment name if its not a new SectionSpec */
            Program.sectMap[Task.defaultStackSection].loadSegment =
                Program.sectMap[".taskStackSection"];
        }
        else {
            /* copy all attrs over */
            for (var attr in Program.sectMap[".taskStackSection"]) {
                Program.sectMap[Task.defaultStackSection][attr] =
                   Program.sectMap[".taskStackSection"][attr];
            }
        }

        /* delete users's Program.sectMap entry. */
        delete Program.sectMap[".taskStackSection"];
    }

    /* minimize footprint by only creating Idle task if Task is enabled */
    if ((BIOS.taskEnabled == true) && (Task.enableIdleTask == true)) {

        /* Create Idle Tasks */

        var tskParams = new Task.Params();
        tskParams.priority = 0;
        tskParams.stackSize = Task.idleTaskStackSize;
        tskParams.stackSection = Task.idleTaskStackSection;

        if (BIOS.smpEnabled == true) {
            for (var i = 0; i < Core.numCores; i++) {
                if (i == 0) {
                    tskParams.vitalTaskFlag = Task.idleTaskVitalTaskFlag;
                }
                else {
                    tskParams.vitalTaskFlag = 0;
                }
                tskParams.affinity = i;
                tskParams.instance.name = "Core " + i + " Idle Task";
                mod.idleTask[i] = Task.create(Idle.loop, tskParams);
            }
        }
        else {
            tskParams.vitalTaskFlag = Task.idleTaskVitalTaskFlag;
            tskParams.instance.name = "ti.sysbios.knl.Task.IdleTask";
            mod.idleTask[0] = Task.create(Idle.loop, tskParams);
        }
    }
    else {
        if (BIOS.smpEnabled == true) {
            for (var i = 0; i < Core.numCores; i++) {
                mod.idleTask[i] = null;
            }
        }
        else {
            mod.idleTask[0] = null;
        }

        if (Task.allBlockedFunc == Idle.run) {
            mod.vitalTasks += 1;
        }
    }

    if (Task.deleteTerminatedTasks == true) {
        Idle.funcList.$add(Task.deleteTerminatedTasksFunc);
    }
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, fxn, params)
{
    var Program = xdc.module('xdc.cfg.Program');
    var mod = this.$module.$object;
    var align;

    /*
     * Handle case when instance$static$init() is invoked before module$use()
     * which initializes the Task.Params.affinity field
     */
    if (params.affinity === undefined) {
        params.affinity = Task.defaultAffinity;
    }

    if (BIOS.taskEnabled == false) {
        Task.$logFatal("Can't create a Task with BIOS.taskEnabled == false.", this);
    }

    /* undefined stackSize means use default which is defaultStackSize */
    if (params.stackSize == 0) {
            params.stackSize = Task.defaultStackSize;
    }

    /* undefined stackSecton means use default which is defaultStackSection */
    if (params.stackSection === undefined) {
            params.stackSection = Task.defaultStackSection;
    }

    /* undefined stackHeap means use default which is defaultStackHeap */
    if (params.stackHeap == null) {
            params.stackHeap = Task.defaultStackHeap;
    }

    obj.priority = params.priority;

    obj.mode = Task.Mode_INACTIVE;

    if (BIOS.smpEnabled == false) {
        obj.curCoreId = 0;
        obj.affinity = 0;
    }
    else if ((params.affinity === undefined) || (params.affinity == ~(0))) {
        obj.curCoreId = Core.numCores;
        obj.affinity = Core.numCores;
    }
    else {
        obj.curCoreId = params.affinity;
        obj.affinity = params.affinity;
    }

    obj.pendElem = null;

    obj.stackHeap = params.stackHeap;

    align = Task.SupportProxy.stackAlignment;

    if (params.stack) {
        Task.$logError("The stack field is not supported for statically defined tasks",
                       this, "stack");
    }
    else {
        if (align != 0) {
            /*
             * round stackSize up to the nearest multiple of the alignment.
             */
            obj.stackSize = (params.stackSize + align - 1) & -align;
            obj.stack.length = obj.stackSize;
        }
        else {
            obj.stackSize = params.stackSize;
            obj.stack.length = obj.stackSize;
        }
        Memory.staticPlace(obj.stack, align, params.stackSection);
    }

    if (obj.stackSize != params.stackSize) {
        Task.$logWarning("stackSize was adjusted to guarantee proper alignment", this, "stackSize");
    }

    obj.fxn = fxn;
    obj.arg0 = params.arg0;
    obj.arg1 = params.arg1;

    obj.env = params.env;

    obj.vitalTaskFlag = params.vitalTaskFlag;
    if (obj.vitalTaskFlag == true) {
        mod.vitalTasks += 1;
    }

    /*
     * the following assignment sets the number of entries in the
     * hookEnv pointer array pointed to by the obj.hookEnv ptr.
     */
    obj.hookEnv.length = Task.hooks.length;

    Queue.elemClearMeta(obj.qElem);
    obj.mask = 1 << params.priority;
    obj.context = null;
    obj.readyQ = null;  /* readyQ filled in by Task_postInit() */

    /* add constructed tasks to constructedTasks array */
    if (this.$category == "Struct") {
        mod.constructedTasks.$add(this);
        Task.numConstructedTasks += 1;
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
    if (hookSet.switchFxn === undefined) {
        hookSet.switchFxn = null;
    }
    if (hookSet.exitFxn === undefined) {
        hookSet.exitFxn = null;
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
    if (Task.numPriorities > BIOS.bitsPerInt) {
        Task.$logError("Task.numPriorities (" +
                        Task.numPriorities +
                        ") can't be greater " +
                        "than the number of " +
                        "bits in an integer (" +
                        BIOS.bitsPerInt + ")",
                        Task, "numPriorities");
    }

    if ((Task.deleteTerminatedTasks == true) &&
        (Task.enableIdleTask == false)) {
        Task.$logError("The automatic deletion of terminated tasks " +
                        "requires the Idle task to be present.",
                        Task, "enableIdleTask");
    }

    if ((BIOS.smpEnabled == true) &&
        (Task.enableIdleTask == false)) {
        Task.$logError("Idle task cannot be disabled in SMP mode.",
                        Task, "enableIdleTask");
    }

    /* validate all "created" instances */
    for(var i = 0; i < Task.$instances.length; i++) {
        instance_validate(Task.$instances[i]);
    }

    /* validate all "constructed" instances */
    for(var i = 0; i < Task.$objects.length; i++) {
        instance_validate(Task.$objects[i]);
    }

    /*
     * Add -D's now that configuration is settled
     */

    var noSwitchHooks = true;
    var noReadyHooks = true;

    for (var i = 0; i < Task.hooks.length; i++) {
        if (Task.hooks[i].switchFxn != null) {
            noSwitchHooks = false;
        }
        if (Task.hooks[i].readyFxn != null) {
            noReadyHooks = false;
        }
    }

    if (noSwitchHooks == false) {
        Build.ccArgs.$add("-Dti_sysbios_knl_Task_ENABLE_SWITCH_HOOKS");
    }
    if (noReadyHooks == false) {
        Build.ccArgs.$add("-Dti_sysbios_knl_Task_ENABLE_READY_HOOKS");
    }

    Build.ccArgs.$add("-Dti_sysbios_knl_Task_minimizeLatency__D=" +
        (Task.minimizeLatency ? "TRUE" : "FALSE"));
}

/*
 *  ======== instance_validate ========
 *  common function to test instance configuration
 */
function instance_validate(instance)
{
    if (instance.$object.fxn == null) {
        Task.$logError("function cannot be null", instance);
    }
    if (instance.priority >= Task.numPriorities) {
        Task.$logError("priority " +
                instance.priority +
                " must be less than Task.numPriorities (" +
                Task.numPriorities + ").", instance, "priority");
    }
    if (Task.enableIdleTask == true) {
        if ((instance.priority == 0) &&
            (instance.$object.fxn != Idle.loop)) {
            Task.$logWarning("Priority 0 reserved for Idle task", instance,
                         "priority");
            }
    }
    if ((instance.priority != 0) &&
        (instance.$object.fxn == Idle.loop)) {
        Task.$logError("Idle task priority must be 0", instance, "priority");
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
 *  ======== scanConstructedTasks ========
 *  Scans statically constructed Tasks to add them to the Task ROV view.
 *
 *  The Task module maintains a table of all statically constructed Tasks so
 *  that it can initialize them at runtime. Constructed objects don't appear
 *  in ROV automatically, so we scan them manually here.
 *
 *  This function does not perform any error handling because it has nowhere
 *  to display an error. If any of the APIs called within this function throw
 *  an exception, it will propogate up and be displayed to the user in ROV.
 */
function scanConstructedTasks(data, viewLevel)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Task = xdc.useModule('ti.sysbios.knl.Task');

    /* Check if the constructed tasks have already been scanned. */
    if (data.scannedConstructedTasks) {
        return;
    }

    /*
     * Set the flag to true now to prevent recursive calls of this function
     * when we scan the constructed tasks.
     */
    data.scannedConstructedTasks = true;

    /* Get the Task module config to get the number of constructed tasks. */
    var modCfg = Program.getModuleConfig('ti.sysbios.knl.Task');

    var numConstrTasks = modCfg.numConstructedTasks;

    /* Just return if there are no statically constructed tasks. */
    if (numConstrTasks == 0) {
        return;
    }

    /*
     * Retrieve the raw view to get at the module state.
     * This should just return, we don't need to catch exceptions.
     */
    var rawView = Program.scanRawView('ti.sysbios.knl.Task');

    var constrTasksAddr = rawView.modState.constructedTasks;
    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');

    /* Retrieve the array of handles to the statically constructed tasks. */
    var taskHandles = Program.fetchArray(ScalarStructs.S_Ptr$fetchDesc,
                                         constrTasksAddr, numConstrTasks);
    /*
     * Scan the instance view for each of the instances.
     * The scanned views will be automatically added to the 'Basic' tab.
     */
    for each (var handle in taskHandles) {
        /* Retrieve the embedded instance */
        var obj = Program.fetchStruct(Task.Instance_State$fetchDesc, handle.elem);

        /*
         * Retrieve the view for the object. This will automatically add the
         * object to the instance list.
         */
        Program.scanObjectView('ti.sysbios.knl.Task', obj, viewLevel);
    }
}

/*
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' Task instance view.
 */
function viewInitBasic(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Task = xdc.useModule('ti.sysbios.knl.Task');
    var BIOS = xdc.useModule('ti.sysbios.BIOS');
    var BIOSCfg = Program.getModuleConfig(BIOS.$name);

    var coreId = obj.curCoreId;

    if (viewCheckForNullObject(Task, obj)) {
        view.label = "Uninitialized Task object";
        return;
    }

    /*
     * First, scan the Task module's array of constructed tasks so that these
     * are added to the view.
     */
    scanConstructedTasks(this, 'Basic');

    view.label = Program.getShortName(obj.$label);
    view.priority = obj.priority;

    if (BIOSCfg.smpEnabled == true) {
        var Core = xdc.useModule('ti.sysbios.hal.Core');
        var CoreCfg = Program.getModuleConfig(Core.$name);

        if (obj.curCoreId == CoreCfg.numCores) {
            view.curCoreId = "X";
            coreId = 0; /* task has not been assigned to a core yet */
        }
        else {
            view.curCoreId = String(obj.curCoreId);
        }
        if (obj.affinity == CoreCfg.numCores) {
            view.affinity = "X";
        }
        else {
            view.affinity = String(obj.affinity);
        }
    }
    else {
        view.curCoreId = "n/a";
        view.affinity = "n/a";
    }

    /* Validate priority */
    var modCfg = Program.getModuleConfig(Task.$name);
    if ((obj.priority < -1) || (obj.priority > (modCfg.numPriorities - 1))) {
        Program.displayError(view, "priority",  "Corrupted data: Task " +
                             "priority is greater than Task.numPriorities");
        return;
    }

    /* Scan the module view to determine the current running Task. */
    try {
        var rawView = Program.scanRawView('ti.sysbios.knl.Task');
    }
    catch (e) {
        Program.displayError(view, "mode", "Error scanning raw view to " +
                             "get current task.");
        return;
    }

    if (obj.priority == -1) {
        view.mode = "Inactive";
    }
    else {
        var biosModView = Program.scanModuleView('ti.sysbios.BIOS', 'Module');
        switch (obj.mode) {
            case Task.Mode_RUNNING:
                if (biosModView.currentThreadType[coreId] == "Task") {
                    view.mode = "Running";
                }
                else {
                    view.mode = "Preempted";
                }
                break;
            case Task.Mode_READY:
                if (BIOSCfg.smpEnabled == true) {
                    view.mode = "Ready";
                }
                else {
                    if ((Number(rawView.modState.curTask) == Number(obj.$addr))) {
                        if (biosModView.currentThreadType[coreId] == "Task") {
                            view.mode = "Running";
                        }
                        else {
                            view.mode = "Preempted";
                        }
                    }
                    else {
                        view.mode = "Ready";
                    }
                }
                break;
            case Task.Mode_BLOCKED:
                view.mode = "Blocked";
                break;
            case Task.Mode_TERMINATED:
                view.mode = "Terminated";
                break;
            default:
                view.mode = "Invalid mode";
                Program.displayError(view, "mode", "Invalid mode: " +
                                     obj.mode);
        }
    }

    view.stackSize = obj.stackSize;

    view.stackBase = "0x" + Number(obj.stack).toString(16);

    /*
     * There may be more than one symbol at this address, so the function
     * name has to be an array.
     */
    view.fxn = Program.lookupFuncName(Number(obj.fxn));

    try {
        /*
         * Special trap for pthreads.  Task arg0 contains the function.
         */
        if (view.fxn[0] == "_pthread_runStub") {
            view.fxn = Program.lookupFuncName(Number(obj.arg0));
        }
    } catch (e) {
        /* Leave function name as _pthread_runStub */
    }

    view.arg0 = obj.arg0;
    view.arg1 = obj.arg1;

    // must check that fxn[] length is > 0 before accessing it for nickName
    var fxnName = (view.fxn.length > 0) ? "" + view.fxn[0] : "";

    view.$private.nickName = view.label != "" ? "Label: " + view.label
                            : fxnName != "" ? "Fxn: " + fxnName
                            : "Handle: " + Number(obj.$addr).toString(16);
}

/*
 *  ======== getNickName ========
 */
function getNickName(tskView)
{
    return(tskView.$private.nickName);
}

/*
 *  ======== viewInitDetailed ========
 *  Initialize the 'Detailed' Task instance view.
 */
function viewInitDetailed(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Task = xdc.useModule('ti.sysbios.knl.Task');
    var TaskCfg = Program.getModuleConfig(Task.$name);
    var Hwi = xdc.useModule('ti.sysbios.hal.Hwi');

    if (viewCheckForNullObject(Task, obj)) {
        view.label = "Uninitialized Task object";
        return;
    }

    /*
     * First, scan the Task module's array of constructed tasks so that these
     * are added to the view.
     */
    scanConstructedTasks(this, 'Detailed');

    /* Detailed view builds off basic view. */
    viewInitBasic(view, obj);

    /* workaround where stack of dummy task points to 0xbebebebe */
    if (obj.stack == 0xbebebebe) {
        return;
    }

    if (TaskCfg.initStackFlag) {
        /* Fetch the Task stack */
        try {
            var stackData = Program.fetchArray(obj.stack$fetchDesc, obj.stack,
                                               obj.stackSize);
        }
        catch (e) {
            Program.displayError(view, "stackPeak", "Error: Problem fetching " +
                                 "Task stack: " + e.toString());
            return;
        }

        /* The function for calculating the stack peak is family-specific. */
        var Support = Program.$modules['ti.sysbios.knl.Task'].SupportProxy;
        var Delegate = xdc.useModule(Support.$name, true);
        view.stackPeak = String(Delegate.stackUsed$view(stackData));
        if (view.stackPeak == view.stackSize) {
            Program.displayError(view, "stackPeak", "Overrun!  ");
            /* (extra spaces to overcome right justify) */
        }
        else if ((obj.context < obj.stack) ||
                 (obj.context > obj.stack + obj.stackSize)) {
            Program.displayError(view, "stackPeak", "SP outside stack!");
        }

        var hwiStackInfo = Hwi.viewGetStackInfo();

        if (hwiStackInfo.hwiStackPeak == hwiStackInfo.hwiStackSize) {
            Program.displayError(view, "stackPeak", "Hwi Stack Overrun!");
        }
    }
    else {
        view.stackPeak = "n/a - set Task.initStackFlag";
    }

    /* fill in blockedOn field if blocked */
    if (view.mode == "Blocked") {
        var pendElem = obj.pendElem;

        if ((pendElem < obj.stack) ||
            (pendElem > (obj.stack + obj.stackSize))) {
            view.blockedOn = "Internal Error";
            Program.displayError(view, "blockedOn", "Invalid task internal state: "
            + "pend element address (0x"
            + Number(pendElem).toString(16)
            + ") is not within the task's stack");
            return;
        }

        if (checkMailboxes(view, obj) == true) return;
        if (checkGateMutexes(view, obj) == true ) return;
        if (checkGateMutexPris(view, obj) == true ) return;
        if (checkSemaphores(view, obj) == true) return;
        if (checkEvents(view, obj) == true ) return;
        if (checkTaskSleep(view, obj) == true ) return;

        view.blockedOn = "Unknown";
    }
}

/*
 *  ======== checkSemPendQ ========
 *  See if task is blocked on this Semaphore
 */
function checkSemPendQ(sem, task)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Semaphore = xdc.useModule('ti.sysbios.knl.Semaphore');

    /* Scan the pendQ to get its elements */
    try {
        var pendQView = Program.scanObjectView('ti.sysbios.knl.Queue', sem.pendQ, 'Basic');
    }
    catch (e) {
        return false;
    }

    /* Compare each task in the pend Q with us */
    for (var j=0; j < pendQView.elems.length; j++) {

        /* Get the element address from the Queue view. */
        var pendElemAddr = pendQView.elems[j];

        /* Fetch the PendElem struct */
        try {
            var pendElem = Program.fetchStruct(Semaphore.PendElem$fetchDesc, pendElemAddr);
        }
        catch (e) {
            return false;
        }
        if (Number(pendElem.tpElem.task) == Number(task.$addr)) {
            return true;
        }
    }
    return false;
}

/*
 *  ======== checkSemaphores ========
 *  See if task is blocked on any Semaphores
 */
function checkSemaphores(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Semaphore = xdc.useModule('ti.sysbios.knl.Semaphore');

    /* check to see if Semaphore is used and grab raw view if so */
    try {
        var semRawView = Program.scanRawView('ti.sysbios.knl.Semaphore');
    }
    catch (e) {
        return false;
    }

    /* Check each instance */
    for (var i in semRawView.instStates) {
        if (checkSemPendQ(semRawView.instStates[i], obj)) {
            view.blockedOn = "Semaphore: 0x" + Number(semRawView.instStates[i].$addr).toString(16);
            return true;
        }
    }
    return false;
}

/*
 *  ======== checkEvents ========
 *  See if task is blocked on any Events
 */
function checkEvents(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Event = xdc.useModule('ti.sysbios.knl.Event');

    /* check to see if Event is used and grab raw view if so */
    try {
        var eventRawView = Program.scanRawView('ti.sysbios.knl.Event');
    }
    catch (e) {
        return false;
    }

    /* Check each instance */
    for (var i in eventRawView.instStates) {

        /* Scan the pendQ to get its elements */
        try {
            var pendQView = Program.scanObjectView('ti.sysbios.knl.Queue', eventRawView.instStates[i].pendQ, 'Basic');
        }
        catch (e) {
            return false;
        }

        /* Compare each task in the pend Q with us */
        for (var j=0; j<pendQView.elems.length; j++) {

            /* Get the element address from the Queue view. */
            var pendElemAddr = pendQView.elems[j];

            /* Fetch the PendElem struct */
            try {
                var pendElem =
                    Program.fetchStruct(Event.PendElem$fetchDesc,
                                        pendElemAddr);
            }
            catch (e) {
                Program.displayError(view, "blockedOn", "Problem with " +
                                     "fetching Event pend element 0x" +
                                     Number(pendElemAddr).toString(16) +
                                     ": " + e.toString());
                return false;
            }
            if (Number(pendElem.tpElem.task) == Number(obj.$addr)) {
                view.blockedOn = "Event: 0x" +
                    Number(eventRawView.instStates[i].$addr).toString(16);
                return true;
            }
        }
    }
    return false;
}

/*
 *  ======== checkMailboxes ========
 *  See if task is blocked on any Mailboxes
 */
function checkMailboxes(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Mailbox = xdc.useModule('ti.sysbios.knl.Mailbox');
    var Semaphore = xdc.useModule('ti.sysbios.knl.Semaphore');

    /* check to see if GateMutexPri is used and grab raw view if so */
    try {
        var mailboxRawView = Program.scanRawView('ti.sysbios.knl.Mailbox');
    }
    catch (e) {
        return false;
    }

    /* Check each instance */
    for (var i in mailboxRawView.instStates) {

        /* get data sem handle */
        var dataSem = mailboxRawView.instStates[i].dataSem;

        if (checkSemPendQ(dataSem, obj)) {
            view.blockedOn = "Mailbox_pend: 0x" +
                Number(mailboxRawView.instStates[i].$addr).toString(16);
            return true;
        }

        /* get free sem handle */
        var freeSem = mailboxRawView.instStates[i].freeSem;

        if (checkSemPendQ(freeSem, obj)) {
            view.blockedOn = "Mailbox_post: 0x" +
                Number(mailboxRawView.instStates[i].$addr).toString(16);
            return true;
        }
    }
    return false;
}

/*
 *  ======== checkTaskSleep ========
 *  See if task is blocked in Task_sleep()
 */
function checkTaskSleep(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Task = xdc.useModule('ti.sysbios.knl.Task');

    if (obj.pendElem == 0) {
            return false;
    }

    /* Get the element address from the Queue view. */
    var pendElemAddr = obj.pendElem;

    /* Fetch the PendElem struct */
    try {
        var pendElem =
            Program.fetchStruct(Task.PendElem$fetchDesc, pendElemAddr);
    }
    catch (e) {
        Program.displayError(view, "blockedOn",
            "Problem with fetching Task pend element 0x" +
            Number(pendElemAddr).toString(16) + ": " + e.toString());
        return false;
    }

    /* Check if 'clock' is null. */
    if (Number(pendElem.clock) == 0) {
        return (false);
    }

    /* Otherwise, retrieve the view for the clock handle. */
    try {
        var clockView =
            Program.scanHandleView('ti.sysbios.knl.Clock',
                                   pendElem.clock, 'Basic');
    }
    catch (e) {
        Program.displayError(view, "blockedOn",
            "Problem scanning pending Clock 0x" +
            Number(pendElem.clock).toString(16) + ": " + e.toString());
        return false;
    }

    view.blockedOn = "Task_sleep(" + clockView.tRemaining + ")";
    return true;
}

/*
 *  ======== checkGateMutexes ========
 *  See if task is blocked on any GateMutexes
 */
function checkGateMutexes(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var GateMutex = xdc.useModule('ti.sysbios.gates.GateMutex');
    var Semaphore = xdc.useModule('ti.sysbios.knl.Semaphore');

    /* check to see if GateMutexPri is used and grab raw view if so */
    try {
        var gateRawView = Program.scanRawView('ti.sysbios.gates.GateMutex');
    }
    catch (e) {
        return false;
    }

    /* Check each instance */
    for (var i in gateRawView.instStates) {

        /* get sem handle */
        var sem = gateRawView.instStates[i].sem;

        if (checkSemPendQ(sem, obj)) {
            view.blockedOn = "GateMutex: 0x" + Number(gateRawView.instStates[i].$addr).toString(16);
            return true;
        }
    }
    return false;
}

/*
 *  ======== checkGateMutexPris ========
 *  See if task is blocked on any GateMutexPris
 */
function checkGateMutexPris(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Task = xdc.useModule('ti.sysbios.knl.Task');
    var GateMutexPri = xdc.useModule('ti.sysbios.gates.GateMutexPri');

    /* check to see if GateMutexPri is used and grab raw view if so */
    try {
        var gateRawView = Program.scanRawView('ti.sysbios.gates.GateMutexPri');
    }
    catch (e) {
        return false;
    }

    /* Check each instance */
    for (var i in gateRawView.instStates) {

        /* Scan the pendQ to get its elements */
        try {
            var pendQView = Program.scanObjectView('ti.sysbios.knl.Queue', gateRawView.instStates[i].pendQ, 'Basic');
        }
        catch (e) {
            return false;
        }

        /* Compare each task in the pend Q with us */
        for (var j = 0; j < pendQView.elems.length; j++) {

            /* Get the pendElem address from the Queue view. */
            var pendElemAddr = pendQView.elems[j];

            /* Fetch the PendElem struct */
            try {
                var pendElem = Program.fetchStruct(Task.PendElem$fetchDesc, pendElemAddr);
            }
            catch (e) {
                return false;
            }

            var currTaskHandle = pendElem.task;

            if (Number(currTaskHandle.$addr) == Number(obj.$addr)) {
                view.blockedOn = "GateMutexPri: 0x" + Number(gateRawView.instStates[i].$addr).toString(16);
                return true;
            }
        }
    }
    return false;
}

/*
 *  ======== viewInitModule ========
 *  Initialize the Task module view.
 */
function viewInitModule(view, mod)
{
    var BIOS = xdc.useModule('ti.sysbios.BIOS');
    var Program = xdc.useModule('xdc.rov.Program');
    var Hwi = xdc.useModule('ti.sysbios.hal.Hwi');
    var HwiCfg = Program.getModuleConfig(Hwi.$name);
    var Task = xdc.useModule('ti.sysbios.knl.Task');

    var swiLock = 0;

    var biosModConfig = Program.getModuleConfig(BIOS.$name);

    if (biosModConfig.swiEnabled == true) {
        try {
            var swiRawView = Program.scanRawView('ti.sysbios.knl.Swi');
            swiLock = swiRawView.modState.locked;
        }
        catch (e) {
            Program.displayError(view, "schedulerState",
                "Swi scheduler state unknown");
            swiLock = 0;
        }
    }

    view.schedulerState = (mod.locked ? "Locked" : swiLock ? "Blocked by Swi scheduler" : "Unlocked");
    view.workPending = mod.workFlag;
    view.numVitalTasks = mod.vitalTasks;

    if (biosModConfig.smpEnabled == true) {
        var Core = xdc.useModule('ti.sysbios.hal.Core');
        var CoreCfg = Program.getModuleConfig(Core.$name);
        var numCores = CoreCfg.numCores;

        var curSet = Program.fetchArray(mod.smpCurSet$fetchDesc, mod.smpCurSet, numCores+1);
        view.readyQMask.length = numCores + 1;
        for (var i=0; i <= numCores; i++) {
            view.readyQMask[i] = "0x" + curSet[i].toString(16);
            view.readyQMask[i] = "0x" + curSet[i].toString(16);
            view.readyQMask[i] = "0x" + curSet[i].toString(16);
        }

        var curTask = Program.fetchArray(mod.smpCurTask$fetchDesc, mod.smpCurTask, numCores);
        view.currentTask.length = numCores;
        for (var i=0; i < numCores; i++) {
            view.currentTask[i] = curTask[i];
            view.currentTask[i] = curTask[i];
        }
    }
    else {
        view.readyQMask.length = 1;
        view.readyQMask[0] = "0x" + mod.curSet.toString(16);

        view.currentTask.length = 1;
        view.currentTask[0] = mod.curTask;
    }

    var stackInfo = Hwi.viewGetStackInfo();

    if (stackInfo.hwiStackSize == 0) {
        var msg = "Error fetching Hwi stack info!";
        Program.displayError(view, "hwiStackPeak", msg);
        Program.displayError(view, "hwiStackSize", msg);
        Program.displayError(view, "hwiStackBase", msg);
    }
    else {
        if (HwiCfg.initStackFlag) {
            view.hwiStackPeak = String(stackInfo.hwiStackPeak);
            if (stackInfo.hwiStackPeak == stackInfo.hwiStackSize) {
                Program.displayError(view, "hwiStackPeak", "Overrun!  ");
                /*                                                  ^^  */
                /* (extra spaces to overcome right justify)             */
            }
        }
        else {
            view.hwiStackPeak = "n/a - set Hwi.initStackFlag";
        }
        view.hwiStackSize = stackInfo.hwiStackSize;
        view.hwiStackBase = "0x"+ stackInfo.hwiStackBase.toString(16);
    }
}

/*
 *  ======== viewInitReadyQs ========
 *  Generate the Task 'ReadyQs' view.
 */
function viewInitReadyQs()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var BIOS = xdc.useModule('ti.sysbios.BIOS');
    var BIOSCfg = Program.getModuleConfig(BIOS.$name);

    var Task = xdc.useModule('ti.sysbios.knl.Task');
    var TaskCfg = Program.getModuleConfig(Task.$name);

    var coreId, qSets, numCores, numPris;

    if (BIOSCfg.smpEnabled == true) {
        var Core = xdc.useModule('ti.sysbios.hal.Core');
        var CoreCfg = Program.getModuleConfig(Core.$name);
        numCores = CoreCfg.numCores;
    }
    else {
        numCores = 1;
    }

    numPris = TaskCfg.numPriorities;

    var viewNode = new Program.TreeNode();

    /* not displayed!!! */
    viewNode.label = "Priority Queues";

    viewNode.children = new Array();

    try {
        var taskRawView = Program.scanRawView('ti.sysbios.knl.Task');
    }
    catch (e) {
        return null;
    }

    var readyQs = taskRawView.modState.readyQ;

    // print("readyQs:", readyQs.$addr);

    if (BIOSCfg.smpEnabled == false) {
        viewGetReadyQs(viewNode, readyQs, 0, numPris, 0);
    }
    else {
        qSets = numCores + 1;

        for (coreId = 0; coreId < qSets; coreId++) {
            viewNode.children[viewNode.children.length++] =
                viewGetCoreNode(readyQs, coreId, numCores, numPris);
        }
    }

    return (viewNode.children);
}

/*
 *  ======== viewGetCoreNode ========
 *  tack on all the readyQs for this core
 */
function viewGetCoreNode(readyQs, coreId, numCores, numPris)
{
    var Program = xdc.useModule('xdc.rov.Program');

    var coreNodeView = new Program.TreeNode();

    var affinity;

    if (coreId != numCores) {
        coreNodeView.label = "core " + coreId + " affinity";
        affinity = coreId.toString();
    }
    else {
        coreNodeView.label = "core X affinity";
        affinity = "X";
    }

    /* add this core's readyQs */
    coreNodeView.children = new Array();

    viewGetReadyQs(coreNodeView, readyQs, coreId, numPris, affinity);

    return (coreNodeView);
}

/*
 *  ======== viewGetReadyQs ========
 *  add all tasks on this readyQ
 */
function viewGetReadyQs(priorityNodes, readyQs, coreId, numPris, affinity)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Task = xdc.useModule('ti.sysbios.knl.Task');
    var Queue = xdc.useModule('ti.sysbios.knl.Queue');
    var BIOS = xdc.useModule('ti.sysbios.BIOS');
    var BIOSCfg = Program.getModuleConfig(BIOS.$name);

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

            var tasks = new Array();

            /*
             * To check for loops, store each address as a key in a map,
             * then with each new address check first if it is already in
             * the map.
             */
            var addrs = {};

            var prevElemAddr;

            while (Number(elem.next) != Number(qAddr)) {

                // print("Task: ", elem.next);

                var taskElemView = Program.newViewStruct('ti.sysbios.knl.Task', 'ReadyQs');
                tasks[tasks.length++] = taskElemView;

                taskElemView.task = elem.next;

                /* get a Basic view of this task */
                try {
                    var taskView = Program.scanHandleView('ti.sysbios.knl.Task', elem.next, 'Basic');
                }
                catch (e) {
                    taskElemView.$status["Address"] = "Bad Task object address: "
                        + Number(elem.next).toString(16) + ": " + e.toString();
                    break;
                }

                /* get this task's raw structure too */
                var task = Program.fetchStruct(Task.Instance_State$fetchDesc, elem.next);

                taskElemView.readyQ = task.readyQ;
                taskElemView.label = taskView.label;
                taskElemView.curCoreId = taskView.curCoreId;
                taskElemView.affinity = taskView.affinity;
                taskElemView.priority = taskView.priority;
                taskElemView.mode = taskView.mode;
                taskElemView.fxn = taskView.fxn;

                if (Number(taskElemView.readyQ) != Number(qAddr)) {
                    taskElemView.$status["readyQ"] = "Task readyQ: " +
                        Number(taskElemView.readyQ).toString(16) +
                        " doesn't match this readyQ address: " +
                        Number(qAddr).toString(16);
                }

                if (taskElemView.priority != priority) {
                    taskElemView.$status["priority"] = "Task priority: " +
                        taskElemView.priority +
                        " doesn't match readyQ priority: " + priority;
                }

                if ((taskElemView.affinity != "n/a")
                     && (taskElemView.affinity != affinity)) {
                    taskElemView.$status["affinity"] = "Task affinity: " +
                        taskElemView.affinity +
                        " doesn't match readyQ affinity: " + affinity;
                }

                if (BIOSCfg.smpEnabled == true) {
                    if (taskElemView.mode != "Ready") {
                        taskElemView.$status["mode"] = "Task mode: " +
                            taskElemView.mode +
                            " should be Ready.";
                    }
                }
                else {
                    if ((taskElemView.mode != "Ready")
                         && (taskElemView.mode != "Running")
                         && (taskElemView.mode != "Preempted")) {
                        taskElemView.$status["mode"] = "Task mode: " +
                            taskElemView.mode +
                            " should be Ready, Running, or Preempted.";
                    }
                }

                prevElemAddr = elem.$addr;

                elem = Program.fetchStruct(Queue.Elem$fetchDesc, elem.next);

                taskElemView.next = elem.next;
                taskElemView.prev = elem.prev;

                if (Number(elem.prev) != Number(prevElemAddr)) {
                    taskElemView.$status["prev"] = "Previous task pointer: " +
                        Number(elem.prev.$addr).toString(16) +
                        " doesn't point to previous task: " +
                        Number(prevElemAddr).toString(16);
                }

                /* Before fetching the next element, verify we're not in a loop. */
                if (elem.next in addrs) {
                    taskElemView.$status["next"] = "Queue contains loop. Element: " +
                        elem.$addr + " points to element: " + elem.next;
                    break;
                }

                prevElemAddr = elem.prev.$addr;

                /*
                 * Add the address to a map so we can check for loops.
                 * The value 'true' is just a placeholder to make sure the
                 * address is in the map.
                 */
                addrs[elem.$addr] = true;
            }

            priorityNodes.children[childNum].properties = tasks;
        }
    }
}

/*
 *  ======== viewInitCallStack ========
 *  Generate the Task 'CallStack' view.
 */
function viewInitCallStack()
{
    var obj = {};

    try {
        var CallStack = xdc.useModule('xdc.rov.CallStack');
        CallStack.clearRegisters();
    }
    catch (e) {
        var nullArray = new Array();
        obj["The CallStacks ROV view is only supported with xdctools "
                        + "version 3.25.01.54 or newer."] = nullArray;
        return (obj);
    }

    var Program = xdc.useModule('xdc.rov.Program');
    var BIOS = xdc.useModule('ti.sysbios.BIOS');
    var BIOSCfg = Program.getModuleConfig(BIOS.$name);
    if (BIOSCfg.smpEnabled == true) {
        Core = xdc.useModule("ti.sysbios.hal.Core");
        var CoreCfg = Program.getModuleConfig(Core.$name);
    }

    var Task = xdc.useModule('ti.sysbios.knl.Task');

    var Support = Program.$modules['ti.sysbios.knl.Task'].SupportProxy;
    var Delegate = xdc.useModule(Support.$name, true);

    try {
        var taskRawView = Program.scanRawView('ti.sysbios.knl.Task');
        var biosModView = Program.scanModuleView('ti.sysbios.BIOS', 'Module');
    }
    catch (e) {
        var nullArray = new Array();
        obj["Error: " + e.toString] = nullArray;
        return (obj);
    }

    /* Process each Task instance */
    for (var i in taskRawView.instStates) {
        var taskView = taskRawView.instStates[i];

        if (viewCheckForNullObject(Task, taskView)) {
            var nullArray = new Array();
            obj["0x" + Number(taskView.$addr).toString(16) +
            ",  Uninitialized Task object"] = nullArray;
            continue;
        }

        /* determine Task state */
        var taskState;
        if (taskView.priority == -1) {
            taskState = Task.Mode_INACTIVE;
        }
        else {
            taskState = taskView.mode;

            if (taskState == Task.Mode_READY) {
                if (BIOSCfg.smpEnabled != true) {
                    if ((Number(taskRawView.modState.curTask)
                         == Number(taskView.$addr))) {
                        taskState = Task.Mode_RUNNING;
                    }
                }
            }
        }

        if (BIOSCfg.smpEnabled == true) {
            var taskCoreId = taskView.curCoreId;
            /* If the curCoreId is "don't care", the task isn't running on any core, pretend we're in Task mode */
            var coreThreadType = (taskCoreId == CoreCfg.numCores) ? "Task" : biosModView.currentThreadType[taskCoreId];
        }
        else {
            var coreThreadType = biosModView.currentThreadType[0];
        }

        /* fetch call stack string from target specific TaskSupport module */
        var bts = Delegate.getCallStack$view(taskView,
                                                taskState,
                                                coreThreadType);

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

        try {
            var taskBasicView = Program.scanHandleView('ti.sysbios.knl.Task',
                                        taskRawView.instStates[i].$addr,
                                        "Basic");
        }
        catch (e) {
            var nullArray = new Array();
            obj["Error: " + e.toString] = nullArray;
            return (obj);
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

        var nickName = Task.getNickName(taskBasicView);
        if (nickName != undefined) {
            nickName = nickName.replace("Label: ti.sysbios.knl.Task.IdleTask",
                                    "Label: IdleTask");
        }
        else {
            nickName = "Label: <N/A>";
        }

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
            invframes[frames[i].substring(0,frames[i].indexOf("PC")-1)] =
                frames[i].substr(frames[i].indexOf("PC"));
        }
        obj["0x" + Number(taskView.$addr).toString(16) +
                ",  " + nickName + ",  Task Mode: " +
                taskBasicView.mode] = invframes;
    }

    return (obj);
}
