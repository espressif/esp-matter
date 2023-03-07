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
 *  ======== BIOS.xs ========
 */

var BIOS = null;
var Build = null;
var Core = null;
var MemAlloc = null;
var Memory = null;
var Program = null;
var Timestamp = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    BIOS = this;
    Build = xdc.module('ti.sysbios.Build');
    Program = xdc.module('xdc.cfg.Program');

    var GetSet = xdc.module("xdc.services.getset.GetSet");
    GetSet.onSet(this, "libType", _setLibType);
    GetSet.onSet(this, "assertsEnabled", _setLibOption);
    GetSet.onSet(this, "logsEnabled", _setLibOption);
    GetSet.onSet(this, "runtimeCreatesEnabled", _setMemoryPolicy);
    GetSet.onSet(this, "customCCOpts", _setCustomCCOpts);

    /*
     * Need to initialize GetSet on the 'cpuFreq' structure so that Grace is
     * updated when the fields of 'cpuFreq' are changed.
     */
    GetSet.init(BIOS.cpuFreq);

    var freq =   Program.cpu.clockRate * 1000000;
    BIOS.cpuFreq.lo = freq & 0xffffffff;
    BIOS.cpuFreq.hi =  Number((freq / Math.pow(2,32)).toFixed(0));

    /*
     * On Concerto devices register BIOS with the M3-side Boot module to listen
     * for changes to the CPU frequency and PLL and divider configuration.
     */
    if (Program.cpu.deviceName.match(/F28M3/)) {
        if (Program.build.target.name.match(/M3/)) {
            var BootM3 =xdc.module('ti.catalog.arm.cortexm3.concertoInit.Boot');

            if ('registerFreqListener' in BootM3) {
                BootM3.registerFreqListener(this);
            }
        }
    }
    else if (Program.cpu.deviceName.match(/F2807/) ||
             Program.cpu.deviceName.match(/F2837/) ||
             Program.cpu.deviceName.match(/F28004/)) {
        var Boot = xdc.module('ti.catalog.c2800.initF2837x.Boot');

        if ('registerFreqListener' in Boot) {
            Boot.registerFreqListener(this);
        }
    }
    /*
     * On 28x devices, register BIOS with the 28x Boot module to listen
     * for changes to the CPU frequency from the PLL configuration.
     */
    else if (Program.build.target.name.match(/28/)) {
        var Boot28 = xdc.module('ti.catalog.c2800.init.Boot');

        if ('registerFreqListener' in Boot28) {
            Boot28.registerFreqListener(this);
        }
    }
    /* register frequency listener for MSP432E */
    else if (Program.cpu.deviceName.match(/MSP432E/)) {
        var Boot = xdc.useModule('ti.sysbios.family.arm.msp432e4.init.Boot');
        if ('registerFreqListener' in Boot) {
            Boot.registerFreqListener(this);
        }
    }
    /* register frequency listener for MSP432 */
    else if (Program.cpu.deviceName.match(/MSP432/)) {
        var Boot = xdc.module('ti.sysbios.family.arm.msp432.init.Boot');
        if ('registerFreqListener' in Boot) {
            Boot.registerFreqListener(this);
        }
    }
    /*
     * register frequency listener for Cortex-R5 based devices
     */
    else if (Program.build.target.name.match(/R5F/)) {
        var settings = xdc.module('ti.sysbios.family.Settings');
        var defaultBootModule = settings.getDefaultBootModule();
        if (defaultBootModule != null) {
            var BootModule = xdc.module(defaultBootModule);
            if ('registerFreqListener' in BootModule) {
                BootModule.registerFreqListener(this);
            }
        }
    }

    /*
     * For Stellaris devices, listen to appropriate Boot module
     * for changes to the CPU frequency from the PLL configuration.
     */
    if (Program.cpu.deviceName.match(/LM3.*/)
        || Program.cpu.deviceName.match(/LM4.*/)
        || Program.cpu.deviceName.match(/TM4.*/)) {
        var settings = xdc.module('ti.sysbios.family.Settings');
        var BootModule = xdc.module(settings.getDefaultBootModule());
        if ('registerFreqListener' in BootModule) {
            BootModule.registerFreqListener(this);
        }
    }

    // Have to bind in a default proxy
    BIOS.RtsGateProxy = xdc.module('xdc.runtime.GateNull');

    /* used by Task, Swi, Event modules for error checking */
    BIOS.bitsPerInt = Program.build.target.bitsPerChar * BIOS.intSize.$sizeof();

    var targName = Program.build.target.$name;

    if (targName.indexOf("C28") != -1) {
        /*
         * 28x targets have limited memory and require Task stacks to
         * be placed in memory entirely below the address 0x10000.
         * Setting 'sectionName' to '.ebss:taskStackSection' (or
         * '.bss:taskStackSection' for ELF) accomplishes this.
         */
        if (Program.build.target.$name.match(/elf/)) {
            BIOS.heapSection =".bss:taskStackSection";
        }
        else {
            BIOS.heapSection =".ebss:taskStackSection";
        }
    }

    Memory = xdc.module("xdc.runtime.Memory");

    /*
     * Construct default BIOS.customCCOpts
     * User can override this in their cfg file.
     */
    BIOS.customCCOpts = Build.getDefaultCustomCCOpts();
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    /* Pull in ti.sysbios.rts.MemAlloc module */
    MemAlloc = xdc.useModule('ti.sysbios.rts.MemAlloc');

    if ((BIOS.smpEnabled == true) || (BIOS.libType == BIOS.LibType_Debug)) {
        Core = xdc.useModule('ti.sysbios.hal.Core');
    }

    /* If app config has not specified an argSize, set it to zero */
    if (Program.$written("argSize") == false) {
        Program.argSize = 0x0;
    }

    /*
     * For C28: Check to see if BIOS.cpuFreq is zero, indicating that
     * the tms320x28 generic platform is being used, and the application has
     * not defined the CPU frequency.
     */
    if ((Program.build.target.name.match(/28/)) &&
        (BIOS.cpuFreq.lo == 0) && (BIOS.cpuFreq.hi == 0)) {
        this.$logError("BIOS.cpuFreq is undefined.  You must define the CPU frequency in your application configuration.  For example, if the PLL is being configured for 100MHz, add this to your application configuration script: BIOS.cpuFreq.lo = 100000000;", this, "cpuFreq.lo");
    }

    /*
     * For 28x, if not using the generic platform, the stacksMemory is defined
     * and should be used.  If using generic platform, we have to rely on the
     * placement of .ebss (or .bss for ELF) to be at a low address so task
     * stacks fall below the 0xffff page boundary.  If stack is above 0x10000,
     * we get a runtime error.
     */
    if ( (Program.build.target.name.match(/28/)) &&
            (Program.platform.stackMemory != undefined) ) {

        if (Program.build.target.$name.match(/elf/)) {
           sect = ".bss";
        }
        else {
           sect = ".ebss";
        }

        if (Program.sectMap[sect + ":taskStackSection"] === undefined) {
            Program.sectMap[sect + ":taskStackSection"] =
                new Program.SectionSpec();
            Program.sectMap[sect + ":taskStackSection"].loadSegment =
                Program.platform.stackMemory;
        }
    }

    /* Bring in Settings module for bios 6 osal */
    var Settings = xdc.useModule('ti.sysbios.xdcruntime.Settings', true);
    xdc.useModule('ti.sysbios.knl.Idle', true);

    /*
     *  Only use the types of gates that support nesting by not need the
     *  matching leaves and enters (e.g. its ok not to call the leave if
     *  it was nested) as long as at least one leave is called with
     *  the first returned key.
     */
    if (this.rtsGateType === undefined) {
        if (this.taskEnabled == true) {
            this.rtsGateType = BIOS.GateMutex;
        }
        else if (this.swiEnabled == true) {
            this.rtsGateType = BIOS.GateSwi;
        }
    }

    /* Make sure the correct gate module is used and that it makes sense. */
    if (this.rtsGateType == BIOS.GateMutexPri) {
        if (this.taskEnabled == true) {
            this.RtsGateProxy = xdc.useModule('ti.sysbios.gates.GateMutexPri', true);
        }
        else {
            this.$logError("Cannot use GateMutexPri when taskEnable is false.", this, "rtsGateType");
        }
    }
    else if (this.rtsGateType == BIOS.GateMutex) {
        if (this.taskEnabled == true) {
            this.RtsGateProxy = xdc.useModule('ti.sysbios.gates.GateMutex', true);
        }
        else {
            this.$logError("Cannot use GateMutex when taskEnable is false.", this, "rtsGateType");
        }
    }
    else if (this.rtsGateType == BIOS.GateSwi) {
        if (this.swiEnabled == true) {
            this.RtsGateProxy = xdc.useModule('ti.sysbios.gates.GateSwi', true);
        }
        else {
            this.$logError("Cannot use GateSwi when swiEnable is false.", this, "rtsGateType");
        }
    }
    else if (this.rtsGateType == BIOS.GateHwi) {
        this.RtsGateProxy = xdc.useModule('ti.sysbios.gates.GateHwi', true);
    }

    /* Plug in a gate to be used by rts. */
    if (BIOS.rtsGateType != BIOS.NoLocking) {
        if (Program.build.target.$name.match(/ti/)) {
            BIOS.addUserStartupFunction('&ti_sysbios_BIOS_registerRTSLock');
        }
    }

    /* Setting default heap section */
    var Memory = xdc.module('xdc.runtime.Memory');
    if (Memory.$written("defaultHeapInstance") == true) {
        /* cfg has written defaultHeapInstance, so use it! */
        if (BIOS.$written("heapSize") == true) {
            this.$logWarning("BIOS.heapSize and Memory.defaultHeapInstance " +
                "have both been set.  BIOS.heapSize ignored.  " +
                "Using Memory.defaultHeapInstance.", this, "heapSize");
        }
    }
    else {
        if (BIOS.$written("heapSize")) {
            if (Memory.$written("defaultHeapSize") &&
                    Memory.defaultHeapSize != BIOS.heapSize) {
                this.$logWarning("BIOS.heapSize and Memory.defaultHeapSize " +
                "have both been set.  Memory.defaultHeapSize ignored.  " +
                "Using BIOS.heapSize.", this, "heapSize");
            }
            if (Program.$written("heap") && Program.heap != BIOS.heapSize) {
                this.$logWarning("BIOS.heapSize and Program.heap " +
                "have both been set.  Program.heap ignored.  " +
                "Using BIOS.heapSize.", this, "heapSize");
            }
        }
        else if (Memory.$written("defaultHeapSize")) {
            /* app .cfg set Memory.defaultHeapSize, but not BIOS.heapSize */
            BIOS.heapSize = Memory.defaultHeapSize;

            if (Program.$written("heap") &&
                    Program.heap != Memory.defaultHeapSize) {
                this.$logWarning("Memory.defaultHeapSize and Program.heap " +
                "have both been set.  Program.heap ignored.  " +
                "Using Memory.defaultHeapSize.", Memory, "defaultHeapSize");
            }
        }
        else if (Program.$written("heap")) {
            /* app .cfg only set Program.heap, so we use that. */
            BIOS.heapSize = Program.heap;
        }

        if (BIOS.heapSize != 0) {
            Program.exportModule('xdc.runtime.Memory');

            var HeapMem = xdc.useModule('ti.sysbios.heaps.HeapMem', true);
            HeapMem.common$.fxntab = true;
            var heapMemParams = new HeapMem.Params;
            heapMemParams.size = BIOS.heapSize;
            heapMemParams.sectionName = BIOS.heapSection;
            var heap0 = HeapMem.create(heapMemParams);

            if (BIOS.heapTrackEnabled) {
                var HeapTrack = xdc.useModule('ti.sysbios.heaps.HeapTrack', true);
                var heapTrackParams = new HeapTrack.Params;
                heapTrackParams.heap = heap0;
                var heap1 = HeapTrack.create(heapTrackParams);

                Memory.defaultHeapInstance = heap1;
            }
            else {
                Memory.defaultHeapInstance = heap0;
            }
        }
    }

    /* Set Program.heap accordingly */
    MemAlloc.configureProgramHeap();

    /* Hijack Error.raiseHook and insert ours in before it */
    var Error = xdc.module('xdc.runtime.Error');
    BIOS.installedErrorHook = Error.raiseHook;
    Error.raiseHook = BIOS.errorRaiseHook;

    if (xdc.module('ti.sysbios.family.Settings').bootModule ==
        "ti.sysbios.family.arm.cc26xx.Boot") {
        /* force cc26xx/driverlib to be after sysbios lib in .xdl file */
        xdc.useModule(xdc.module('ti.sysbios.family.Settings').bootModule);
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    var numCores = 1;

    mod.cpuFreq = params.cpuFreq;
    mod.rtsGateCount = 0;
    mod.rtsGateKey   = 0;

    mod.threadType   = BIOS.ThreadType_Main;

    if (BIOS.smpEnabled == true) {
        numCores = Core.numCores;
        mod.smpThreadType.length = numCores;
        for (var coreNum = 0; coreNum < numCores; coreNum++) {
            mod.smpThreadType[coreNum]   = BIOS.ThreadType_Main;
        }
    }
    else {
        mod.smpThreadType.length = 0;
    }

    /* Plug in a gate to be used by rts. */
    if (BIOS.rtsGateType != BIOS.NoLocking) {
        mod.rtsGate = BIOS.RtsGateProxy.create();
    }
    else {
        mod.rtsGate = null;
    }

    /* Assign BIOS_start to it's symbol for ROM */
    mod.startFunc = '&ti_sysbios_BIOS_startFunc';

    /* Assign BIOS_exit to it's symbol for ROM */
    mod.exitFunc = '&ti_sysbios_BIOS_exitFunc';
}

/*
 *  ======== getCCOpts ========
 */
function getCCOpts(target)
{
    return (BIOS.customCCOpts + Build.getCcArgs());
}

/*
 *  ======== getEnumString ========
 *  Get the enum value string name, not 0, 1, 2 or 3, etc.  For an enumeration
 *  type property.
 *
 *  Example usage:
 *  if obj contains an enumeration type property "Enum enumProp"
 *
 *  view.enumString = getEnumString(obj.enumProp);
 *
 */
function getEnumString(enumProperty)
{
    /*
     *  Split the string into tokens in order to get rid of the huge package
     *  path that precedes the enum string name. Return the last 2 tokens
     *  concatenated with "_"
     */
    var enumStrArray = String(enumProperty).split(".");
    var len = enumStrArray.length;
    return (enumStrArray[len - 1]);
}

/*
 *  ======== module$validate ========
 *  some redundant tests are here to catch changes since
 *  module$static$init() and instance$static$init()
 */
function module$validate()
{
    var Settings = xdc.module("ti.sysbios.family.Settings");
    var HwiDelegate = xdc.module(Settings.getDefaultHwiDelegate());
    var Swi = xdc.module("ti.sysbios.knl.Swi");
    var Semaphore = xdc.module("ti.sysbios.knl.Semaphore");
    var Load = xdc.module("ti.sysbios.utils.Load");
    var Defaults = xdc.module('xdc.runtime.Defaults');
    var Diags = xdc.module("xdc.runtime.Diags");
    var libType = getEnumString(BIOS.libType);

    /*
     * Check that user did not write Program.cpu.clockRate; if they did,
     * tell them to write BIOS.cpuFreq instead.
     */
    if (Program.cpu.$written("clockRate") == true) {
        BIOS.$logError("Program.cpu.clockRate should not be modified.  " +
            "Set BIOS.cpuFreq to specify a new CPU frequency.",
                                 Program, "cpu.clockRate");
    }

    /*
     * Verify that the user did not set a BIOS cpuFreq which is different from
     * what's configured in the Boot module.
     *
     * For 28x devices...
     */
    if (Program.build.target.name.match(/28/)) {
        var Boot28 = xdc.module('ti.catalog.c2800.init.Boot');

        /* Only do this check if the Boot module is configuring the PLL. */
        if (Boot28.configurePll &&
            (Boot28.getFrequency() != BIOS.cpuFreq.lo)) {
            BIOS.$logWarning("BIOS.cpuFreq does not match the frequency " +
                             "computed from the PLL configuration in the " +
                             "Boot module. Remove the BIOS.cpuFreq line " +
                             "from your .cfg script to allow the Boot " +
                             "module to set the frequency.", BIOS,
                             "cpuFreq.lo");
        }
    }

    if ((Build.buildROM == true) || (Build.buildROMApp == true)) {
        return;
    }

    switch (BIOS.libType) {
        case BIOS.LibType_Instrumented:
            if (BIOS.assertsEnabled == false) {
                BIOS.$logWarning("BIOS.assertsEnabled should be set to 'true' when " +
                                 "BIOS.libType == BIOS." + libType + ".  " +
                                 "Set 'BIOS.libType = BIOS.LibType_Custom' " +
                                 "to build a custom library or update your configuration.",
                                 BIOS, "assertsEnabled");
            }
            if (BIOS.logsEnabled == false) {
                BIOS.$logWarning("BIOS.logsEnabled should be set to 'true' when " +
                                 "BIOS.libType == BIOS." + libType + ".  " +
                                 "Set 'BIOS.libType = BIOS.LibType_Custom' " +
                                 "to build a custom library or update your configuration.",
                                 BIOS, "logsEnabled");
            }
            break;

        case BIOS.LibType_NonInstrumented:
            if ((BIOS.assertsEnabled == true) && BIOS.$written("assertsEnabled")) {
                BIOS.$logWarning("BIOS.assertsEnabled should be set to 'false' when " +
                                 "BIOS.libType == BIOS." + libType + ".  " +
                                 "Set 'BIOS.libType = BIOS.LibType_Custom' " +
                                 "to build a custom library or update your configuration.",
                                 BIOS, "assertsEnabled");
            }
            if ((BIOS.logsEnabled == true) && BIOS.$written("logsEnabled")) {
                BIOS.$logWarning("BIOS.logsEnabled should be set to 'false' when " +
                                 "BIOS.libType == BIOS." + libType + ".  " +
                                 "Set 'BIOS.libType = BIOS.LibType_Custom' " +
                                 "to build a custom library or update your configuration.",
                                 BIOS, "logsEnabled");
            }
            break;

        case BIOS.LibType_Custom:
            if ((BIOS.assertsEnabled == true)
                && (Defaults.common$.diags_ASSERT == Diags.ALWAYS_OFF)
                && (Defaults.common$.diags_INTERNAL == Diags.ALWAYS_OFF)) {
                BIOS.$logWarning("BIOS.assertsEnabled should be set to 'false' when " +
                                 "Defaults.common$.diags_ASSERT == Diags.ALWAYS_OFF.",
                                 BIOS, "assertsEnabled");
            }
            break;

        case BIOS.LibType_Debug:
        default:
            break;
    }
}

/*
 *  ======== addUserStartupFunction ========
 */
function addUserStartupFunction (func)
{
    this.startupFxns.$add(func);
}

/*
 *  ======== viewInitModule ========
 *  Display the module properties in ROV
 */
function viewInitModule(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var BIOS = xdc.useModule('ti.sysbios.BIOS');
    var biosModConfig = Program.getModuleConfig(BIOS.$name);

    var numCores;

    view.rtsGateType  = "" + biosModConfig.rtsGateType;
    view.cpuFreqLow   = obj.cpuFreq.lo;
    view.cpuFreqHigh  = obj.cpuFreq.hi;
    view.clockEnabled = biosModConfig.clockEnabled;
    view.swiEnabled   = biosModConfig.swiEnabled;
    view.taskEnabled  = biosModConfig.taskEnabled;
    view.startFunc    = "0x" + Number(obj.startFunc).toString(16);

    if (biosModConfig.smpEnabled == true) {
        var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');
        var Core = xdc.useModule('ti.sysbios.hal.Core');
        var coreModConfig = Program.getModuleConfig(Core.$name);

        numCores = coreModConfig.numCores;

        /*
         * Sadly, the following commented out line doesn't work
         * because an enum isn't a supported core data type needed
         * by Program.fetchArray()
         *
         *  var threadType = Program.fetchArray(obj.smpThreadType$fetchDesc, obj.smpThreadType, numCores);
         *
         * Instead, the following presumptive code is provided as a solution:
         */
        if ((Program.build.target.name == "M3") ||
            (Program.build.target.name == "M4") ||
            (Program.build.target.name == "M4F") ||
            (Program.build.target.name == "A9F") ||
            (Program.build.target.name == "A15F")) {
            var threadType = Program.fetchArray({type:'xdc.rov.support.ScalarStructs.S_UChar', isScalar:true},
                            obj.smpThreadType,
                            numCores);
        }
        else {
            var threadType = Program.fetchArray({type:'xdc.rov.support.ScalarStructs.S_UInt', isScalar:true},
                            obj.smpThreadType,
                            numCores, false);
        }
    }
    else {
        numCores = 1;
        var threadType = new Array();
        threadType.length = 1;
        threadType[0] = obj.threadType;
    }

    view.currentThreadType.length = numCores;

    for (var i = 0; i < numCores; i++) {
        if (threadType[i] == BIOS.ThreadType_Hwi) {
            view.currentThreadType[i] = "Hwi";
        }
        else if (threadType[i] == BIOS.ThreadType_Swi) {
            view.currentThreadType[i] = "Swi";
        }
        else if (threadType[i] == BIOS.ThreadType_Task) {
            view.currentThreadType[i] = "Task";
        }
        else if (threadType[i] == BIOS.ThreadType_Main) {
            view.currentThreadType[i] = "Main";
        }
        else {
            view.currentThreadType[i] = "undefined";
            view.$status["currentThreadType"] =
                    "Error: unknown thread type for current thread.";
        }
    }
}

/*
 *  ======== viewInitErrorScan ========
 */
function viewInitErrorScan(view)
{
    var Program = xdc.useModule('xdc.rov.Program');

    var elements = new Array();

    /*
     * Verify that the current XDCtools contains the Program.getStatusTable
     * API (available in 3.22.01).
     */
    if (!('getStatusTable' in Program)) {
        displayNewError(elements, "BIOS", "Scan for errors...", "N/A", "N/A",
                        "This ROV view is only supported by XDCtools " +
                        "3.22.01 and later.");

        view.elements = elements;
        return;
    }

    /* Run every single ROV view to gather any errors. */
    scanAllViews();

    /*
     * Get ROV's table of status messages (built up while the views
     * were run).
     */
    var statusTab = Program.getStatusTable();

    /* Display all of the entries in the status table. */
    for each (var statusElem in statusTab) {
        displayNewStatus(elements, statusElem);
    }

    /* Check for any Log.L_error events in the LoggerBufs. */
    scanLoggerBufsForErrors(elements);

    /*
     * If no errors were found, display something positive to let them no
     * it was successful.
     */
    if (elements.length == 0) {
        displayNewError(elements, "BIOS", "Scan for errors...", "N/A", "N/A",
                        "All ROV views have been run and no errors were " +
                        "encountered. In BIOS, this includes a check of the " +
                        "System stack usage and all Task stacks.");
    }

    view.elements = elements;
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
                        print("Undefined view type: " + viewType);
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
function displayNewError(elements, modName, tabName, inst, field, msg)
{
    var Program = xdc.useModule('xdc.rov.Program');

    var elem = Program.newViewStruct('ti.sysbios.BIOS', 'Scan for errors...');

    elem.mod = modName;
    elem.tab = tabName;
    elem.inst = inst;
    elem.field = field;
    elem.message = msg;

    elements[elements.length] = elem;
}

/*
 *  ======== getCpuFrequency ========
 */
function getCpuFrequency()
{
    return (BIOS.cpuFreq.lo);
}

/*
 *  ======== getCpuFreqMeta ========
 */
function getCpuFreqMeta()
{
    return (BIOS.cpuFreq);
}

/*
 *  ======== getTimestampFrequency ========
 */
function getTimestampFrequency()
{
    var Timestamp = xdc.module('xdc.runtime.Timestamp');
    var TimestampProvider = xdc.module(Timestamp.SupportProxy.delegate$.$name);

    try {
        var freq = TimestampProvider.getFreqMeta();
        return (freq.lo);
    }
    catch (e) {
        BIOS.$logWarning("WARNING: Timestamp provider " +
                Timestamp.SupportProxy.delegate$.$name +
                " does not implement getFreqMeta(). " +
                "BIOS.getTimestampFrequency() is returning 0!", BIOS,
                "getTimestampFrequency");
    }

    /* If we can't determine the timestamp frequency, return 0. */
    return (0);
}

/*
 *  ======== getTimestampFreqMeta ========
 */
function getTimestampFreqMeta()
{
    var Timestamp = xdc.module('xdc.runtime.Timestamp');
    var TimestampProvider = xdc.module(Timestamp.SupportProxy.delegate$.$name);

    try {
        var freq = TimestampProvider.getFreqMeta();
        return (freq);
    }
    catch (e) {
        BIOS.$logWarning("WARNING: Timestamp provider " +
                Timestamp.SupportProxy.delegate$.$name +
                " does not implement getFreqMeta(). " +
                "BIOS.getTimestampFreqMeta() is returning 0!", BIOS,
                "getTimestampFreqMeta");
    }

    /* If we can't determine the timestamp frequency, return 0. */
    return ({lo : 0, hi : 0});
}

/*
 *  ======== getDefaultTimestampProvider ========
 */
function getDefaultTimestampProvider()
{
    var Settings = xdc.module("ti.sysbios.family.Settings");
    return (Settings.getDefaultTimestampDelegate());
}

/*
 *  ======== getLabel ========
 *  Get a string to represent the specified instance.
 */
function getLabel(view)
{
    /* If a 'label' was specified, just return that. */
    if ((view.label != null) &&
        (view.label != undefined) &&
        (view.label != "")) {
        return (view.label);
    }

    /* Otherwise, return the handle address in parentheses. */
    return ("(0x" + Number(view.address).toString(16) + ")");
}

/*
 *  ======== scanLoggerBufsForErrors ========
 *  Look through all of the LoggerBuf instances for any Log.L_error events.
 */
function scanLoggerBufsForErrors(elements)
{
    var Program = xdc.useModule('xdc.rov.Program');

    /* Verify LoggerBuf is in this app. */
    try {
        Program.getModuleDesc('xdc.runtime.LoggerBuf');
    }
    catch (e) {
        /* If not, just bail. */
        return;
    }

    /* Retrieve the LoggerBuf 'Records' view. */
    var dataViews = Program.scanInstanceDataView('xdc.runtime.LoggerBuf',
                                                 'Records');

    /* Look through each of the instances. */
    for each (var view in dataViews) {
        for each (var record in view.elements) {
            if (record.eventName == "xdc.runtime.Log.L_error") {
                displayNewError(elements, "xdc.runtime.LoggerBuf", "Records",
                                view.label, "Record #" + record.serial,
                                record.text);
            }
        }
    }
}

/*
 *  ======== _setLibType ========
 *  The "real-time" setter setLibType function
 *
 *  This function is called whenever libType changes.
 */
function _setLibType(field, val)
{
    var BIOS = this;

    /*
     * Set BIOS.assertsEnabled and BIOS.logsEnabled according to libType
     */
    switch (val) {
        case BIOS.LibType_Instrumented:
        case BIOS.LibType_Custom:
        case BIOS.LibType_Debug:
            /*
             * do nothing, default value for assertsEnabled
             * and logsEnabled is true
             */
            break;

        case BIOS.LibType_NonInstrumented:
            /* override default value of true */
            BIOS.assertsEnabled = false;
            BIOS.logsEnabled = false;
            break;

        default:
            print(BIOS.$name + ": unknown libType setting: " + val);
            break;
    }

    /*
     * Re-construct default BIOS.customCCOpts
     */
    BIOS.customCCOpts = Build.getDefaultCustomCCOpts();
}

/*
 *  ======== _setMemoryPolicy ========
 *  The "real-time" memoryPolicy setter function
 *
 *  This function is called whenever runtimeCreatesEnabled changes.
 */
function _setMemoryPolicy(field, val)
{
    var Defaults = xdc.module('xdc.runtime.Defaults');

    if (val == true) {
        Defaults.common$.memoryPolicy = xdc.module("xdc.runtime.Types").DELETE_POLICY;
    }
    else {
        Defaults.common$.memoryPolicy = xdc.module("xdc.runtime.Types").STATIC_POLICY;
    }
}

/*
 *  ======== _setLibOption ========
 */
function _setLibOption(field, val)
{
    var BIOS = this;

    /* nothing to do (yet) */
}

function _setCustomCCOpts(field, val)
{
}

/* Flag used by 'fireFrequencyUpdate'. */
var freqFromBoot = false;

/*
 *  ======== fireFrequencyUpdate ========
 *  Called from a Boot module to notify BIOS of an update in configured
 *  frequency as a result of PLL configuration.
 *
 *  Updates the BIOS cpuFreq whenever one of the PLL parameters changes.
 */
function fireFrequencyUpdate(newFreq)
{
    /* Don't do anything if the new frequency is the same as the current. */
    if (BIOS.cpuFreq.lo == newFreq) {
        return;
    }

    /*
     *  If the frequency has been previously modified by the user (not by the
     *  Boot module's PLL configuration), then warn them that we are
     *  overwriting their previous setting.
     */
    if ((freqFromBoot == false) &&
        (BIOS.cpuFreq.$written("lo") || BIOS.cpuFreq.$written("hi"))) {

        BIOS.$logWarning("PLL configuration is overwriting previously " +
                         "set BIOS.cpuFreq.",
                         BIOS, "cpuFreq");
    }

    /*
     * Set the 'freqFromBoot' flag to indicate that the frequency has been
     * modified by the PLL configuration.
     */
    freqFromBoot = true;

    /* Update the BIOS frequency. */
    BIOS.cpuFreq.lo = newFreq;
}
