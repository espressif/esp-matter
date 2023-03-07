/*
 * Copyright (c) 2020-2021 Texas Instruments Incorporated - http://www.ti.com
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
 *
 */

/*
 *  ======== BIOS.syscfg.js ========
 */


"use strict";

/* use bigInt for zero padding hex numbers */
let bigInt = system.utils.bigInt;

/* get device specific Settings */
let Settings = system.getScript("/ti/sysbios/family/Settings.js");

const device = system.deviceData.deviceId;
const isM33 = device.match(/CC(?:13|26).4/) !== null;

/*
 * ======== moduleInstances ========
 */
function moduleInstances(mod)
{
    let modInstances = new Array();

    modInstances.push({
        name: "startFuncs",
        displayName: "BIOS_start() User Functions",
        moduleName: "ti/sysbios/BIOSStartUserFuncs",
        useArray: true
    });

    modInstances.push({
        name: "biosInit",
        displayName: "BIOS_init()",
        moduleName: "ti/sysbios/runtime/Startup",
        hidden: true,
        readOnly: true,
        requiredArgs: {
            startupFxn: "BIOS_init",
            functionType: "First Function",
            priority: -1, /* call before all other Mod_init() */
            externRequired: false,
            $name: "BIOS_init"
        }
    });

    return (modInstances);
}

/*
 * ======== modules ========
 * Pull in the appropiate dependent modules
 */
function modules(mod)
{
    let modArray = [];

    /* Mandatory core modules */
    modArray.push({name: "Idle", moduleName: "ti/sysbios/knl/Idle"});
    modArray.push({name: "Error", moduleName: "ti/sysbios/runtime/Error"});
    modArray.push({name: "Memory", moduleName: "ti/sysbios/runtime/Memory"});
    modArray.push({name: "MemAlloc", moduleName: "ti/sysbios/rts/MemAlloc"});
    modArray.push({name: "System", moduleName: "ti/sysbios/runtime/System"});
    modArray.push({name: "Startup", moduleName: "ti/sysbios/runtime/Startup"});
    modArray.push({name: "BIOSStartUserFuncs", moduleName: "ti/sysbios/BIOSStartUserFuncs"});

    /* Optional core modules */
    if (!mod.disableClock) {
        modArray.push({name: "Clock", moduleName: "ti/sysbios/knl/Clock"});
    }
    if (!mod.disableTask) {
        modArray.push({name: "Task", moduleName: "ti/sysbios/knl/Task"});
    }
    if (!mod.disableSwi) {
        modArray.push({name: "Swi", moduleName: "ti/sysbios/knl/Swi"});
    }

    /* Heap management */
    if (mod.heapType != "HeapUser") {
        /* HeapMin, HeapMem, HeapCallback can all be used directly like this
         * We need to name it "HeapXXX_module" otherwise there is a naming conflict
         */
        modArray.push({name: mod.heapType + "_module", moduleName: "ti/sysbios/heaps/" + mod.heapType});
    } else {
        /* Deliberate no-op for HeapUser */
    }
    if (mod.heapTrackEnabled) {
        modArray.push({name: "HeapTrack", moduleName: "ti/sysbios/heaps/HeapTrack"});
    }

    /* Gate modules are named BIOS_GateXXX so we need to replace the BIOS_ to get the syscfg module */
    var gateName = mod.rtsGateType.replace("BIOS_", "")
    modArray.push({name: gateName, moduleName: "ti/sysbios/gates/" + gateName});

    /* Configurable extension modules */
    if (mod.assertsEnabled) {
        modArray.push({name: "Assert", moduleName: "ti/sysbios/runtime/Assert"});
    }
    if (mod.psaEnabled) {
        modArray.push({name: "PSA", moduleName: "ti/sysbios/psa/PSA"});
    }

    /* Device-specific modules from Settings */
    if (Settings.secondsModule != null) {
        modArray.push({name: "Seconds", moduleName: Settings.secondsModule});
    }
    if (Settings.bootModule != null) {
        modArray.push({name: "Boot", moduleName: Settings.bootModule});
    }
    modArray.push({name: "Hwi", moduleName: Settings.hwiModule});

    return modArray;
}

/*
 * ======== getLibs ========
 */
function getLibs(mod)
{
    let GenLibs = system.getScript("/ti/utils/build/GenLibs.syscfg.js");
    var toolchain = GenLibs.getToolchainDir();
    var isa = GenLibs.getDeviceIsa();

    var lib_base_name = "ti/sysbios/lib/" + toolchain + "/" + isa + "/"
    var link_info = {
        name: "/ti/sysbios",
        deps: [],
        libs: []
    }

    if (mod.$static.psaEnabled) {
        link_info.libs.push(lib_base_name + "sysbios_psa.a")
    } else {
        link_info.libs.push(lib_base_name + "sysbios.a")
    }

    return link_info;
}

/*
 *  ======== validate ========
 */
function validate(mod, validation)
{
    if (system.getRTOS() != "tirtos7") {
        validation.logError("Please configure sysconfig with --rtos tirtos7 to use this module!", mod);
    }
}

/*
 *  ======== onHeapChange ========
 */
function onHeapChange(mod, ui)
{
    if (mod.heapType == "HeapMem" || mod.heapType == "HeapMin") {
        ui.heapSize.hidden = false;
        ui.heapBaseAddr.hidden = false;
        ui.heapEndAddr.hidden = false;
        if (mod.heapBaseAddr != "" || mod.heapEndAddr != "") {
            ui.heapSize.hidden = true;
        }
    }
    else {
        ui.heapSize.hidden = true;
        ui.heapBaseAddr.hidden = true;
        ui.heapEndAddr.hidden = true;
    }
}

/*
 *  ======== base ========
 */
let base = {
    staticOnly: true,
    displayName: "BIOS",
    moduleStatic: {
        name: "moduleGlobal",
        modules: modules,
        moduleInstances: moduleInstances,
        validate: validate,
        config: [
            {
                name: "cpuFrequency",
                displayName: "CPU Frequency (Hz)",
                default: Settings.defaultCpuFrequency
            },
            {
                name: "assertsEnabled",
                description: `SYS/BIOS Assert checking in Custom SYS/BIOS
                library enable flag`,
                longDescription: `
When set to false, Assert checking code is removed from the final compiled kernel. This option can considerably improve
runtime performance as well as significantly reduce the application's code size.`,
                displayName: "Enable Asserts",
                default: true
            },
            {
                name: "psaEnabled",
                description: `Enables ARM's Platform Security Architecture (PSA) extensions`,
                longDescription: `This functionality is available on only select devices.`,
                displayName: "Enable PSA Extensions",
                hidden: !isM33,
                default: false
            },
            {
                name: "rtsGateType",
                description: `Gate to make sure TI RTS library APIs are re-
                entrant`,
                longDescription: `
The application gets to determine the type of gate (lock) that is used in the TI RTS library. The gate will be used to
guarantee re-entrancy of the RTS APIs.

The type of gate depends on the type of threads that are going to be calling into the RTS library.  For example, if
both Swi and Task threads are going to be calling the RTS library's printf, GateSwi should be used. In this case, Hwi
threads are not impacted (i.e. disabled) during the printf calls from the Swi or Task threads.

If NoLocking is used, the RTS lock is not plugged and re-entrancy for the TI RTS library calls are not guaranteed. The
application can plug the RTS locks directly if it wants.

Numerous gate types are provided by SYS/BIOS.  Each has its advantages and disadvantages.  The following list
summarizes when each type is appropriate for protecting an underlying non-reentrant RTS library.

- GateHwi: Interrupts are disabled and restored to maintain re-entrancy. Use if only making RTS calls from a Hwi, Swi
  and/or Task.

- GateSwi: Swis are disabled and restored to maintain re-entrancy. Use if only making RTS calls from a Swi and/or
  Task.

- GateMutex: A single mutex is used to maintain re-entrancy.  Use if only making RTS calls from a Task.  Blocks only
  Tasks that are also trying to execute critical regions of RTS library.

- GateMutexPri: A priority inheriting mutex is used to maintain re-entrancy. Blocks only Tasks that are also trying to
  execute critical regions of RTS library.  Raises the priority of the Task that is executing the critical region in
  the RTS library to the level of the highest priority Task that is block by the mutex.

The default value of rtsGateType depends on the type of threading model enabled by other configuration parameters. If
taskEnabled is true, GateMutex is used. If swiEnabled is true and taskEnabled is false: GateSwi is used. If both
swiEnabled and taskEnabled are false: xdc.runtime#GateNull is used.

If taskEnabled is false, the user should not select GateMutex (or other Task level gates). Similarly, if taskEnabled
and swiEnabledare false, the user should not select GateSwi or the Task level gates.`,
                displayName: "Runtime Support Library Lock Type",
                default: "BIOS_GateMutex",
                options: [
                    { name: "BIOS_NoLocking", displayName: "No lock" },
                    { name: "BIOS_GateHwi", displayName: "GateHwi" },
                    { name: "BIOS_GateSwi", displayName: "GateSwi" },
                    { name: "BIOS_GateMutex", displayName: "GateMutex" },
                    { name: "BIOS_GateMutexPri", displayName: "GateMutexPri" }
                ]
            },
            /*
             * Default heap settings block (collapsed by default)
             */
            {
                displayName: "Default Heap Settings",
                config: [
                    {
                        name: "heapType",
                        displayName: "Default Memory Heap Type",
                        longDescription: `
Specifies the type of heap to use for the system heap. The system heap is used to allocate instance object state
structures, such as Task objects and their stacks, Semaphore objects, etc.

If set to "HeapCallback" the user must also configure the HeapCallback module with the necessary callback functions.
HeapCallback_create() will be called at initialization time with a NULL HeapCallback_Params argument.

If set to "HeapUser", no system heap will not be created at startup. The user can create a heap at runtime and use the
Memory_setDefaultHeap() API to register the heap with the Memory module. Or, you can use this option for applications
that do not require a heap.`,
                        default: "HeapMem",
                        options: [
                            { name: "HeapMem" },
                            { name: "HeapMin" },
                            { name: "HeapCallback" },
                            { name: "HeapUser", displayName: "HeapUser (None)" }
                        ],
                        onChange: onHeapChange
                    },
                    {
                        name: "heapSize",
                        description: `Size of system heap, units are in MAUs`,
                        longDescription: `
If the BIOS.heapType configuration parameter is set to "HeapMin" or "HeapMem", and the BIOS.heapBaseAddr and
BIOS.heapEndAddr are not set, then the system heap will be created with this size.`,
                        displayName: "Heap Size",
                        displayFormat: { radix: "hex", bitSize: 32 },
                        hidden: false,
                        default: 0x1000,
                        onChange: onHeapChange
                    },
                    {
                        name: "heapBaseAddr",
                        displayName: "Base address of the 'Primary Heap' buffer",
                        description: `Base address of system heap buffer`,
                        longDescription: `
The BIOS.heapBaseAddr configuration parameter defines the base address of the system heap. A symbol from the linker
command file is often used for this parameter.`,
                        hidden: false,
                        default: "",
                        onChange: onHeapChange
                    },
                    {
                        name: "heapEndAddr",
                        displayName: "End address of the 'Primary Heap' buffer, plus one",
                        description: `End address of system heap buffer, plus one`,
                        longDescription: `
The BIOS.heapEndAddr configuration parameter defines the end address
of the system heap. A symbol from the linker command file is often used for
this parameter.`,
                        hidden: false,
                        default: "",
                        onChange: onHeapChange
                    },
                    {
                        name: "heapTrackEnabled",
                        description: `Use HeapTrack with system default heap`,
                        longDescription: `
This configuration parameter will add a HeapTrack instance on top of the
system heap. HeapTrack adds a tracker packet to every allocated buffer and
displays the information in RTOS Object Viewer (ROV). An assert will be
raised on a free if there was a buffer overflow.`,
                        displayName: "Use HeapTrack with system default heap",
                        default: false
                    },
                ]
            },
            /*
             * Core module disable switches
             */
            {
                displayName: "Disable Core Modules",
                description: "Significantly alters system behaviour.",
                config: [
                    {
                        name: "disableTask",
                        description: `Disable SYS/BIOS Task services`,
                        longDescription: `
The following behaviors occur when taskEnabled is set to false:
- Static Task creation will result in a fatal build error.
- The Idle task object is not created. (The Idle functions are invoked within the start() thread.)
- Runtime calls to Task_create will trigger an assertion violation via xdc.runtime.Assert#isTrue.`,
                        displayName: "Disable Task",
                        default: false
                    },
                    {
                        name: "disableSwi",
                        description: `Disable SYS/BIOS Swi services`,
                        longDescription: `
The following behaviors occur when swiEnabled is set to false:
- Static Swi creation will result in a fatal build error.
- Runtime calls to Swi_create will trigger an assertion violation via xdc.runtime.Assert#isTrue.
- See other effects as noted for clockEnabled = false;`,
                        displayName: "Disable Swi",
                        default: false
                    },
                    {
                        name: "disableClock",
                        description: `Disable SYS/BIOS Clock services`,
                        longDescription: `
The following behaviors occur when clockEnabled is set to false:
- Static Clock creation will result in a fatal build error.
- No Clock Swi is created.
- The Clock_tickSource is set to Clock_TickSource_NULL to prevent a Timer object from being created.
- For APIs that take a timeout, values other than NO_WAIT will be equivalent to WAIT_FOREVER.`,
                        displayName: "Disable Clock",
                        default: false
                    }
                ]
            },
            /* Hidden options */
            {
                name: "logsEnabled",
                hidden: true,
                description: `SYS/BIOS Log support in Custom SYS/BIOS library enable flag`,
                longDescription: `
When set to false, all Log code is removed from the final compiled kernel. This option can considerably improve runtime
performance as well as significantly reduce the application's code size.

***Warning:*** When logs are generated, this setting will have the side effect of requiring task stacks to be sized
large enough to absorb two interrupt contexts rather than one. See the discussion on task stacks in Task for more
information.`,
                displayName: "Enable Logs",
                default: false
            },
            {
                name: "runtimeCreatesEnabled",
                hidden: true,
                description: `Runtime instance creation enable flag.`,
                longDescription: `
If set to true, Module_create() and Module_delete() are callable at runtime. Resources for these will be allocated on
the heap. If false, Module_create() and Module_delete() are not callable at runtime`,
                displayName: "Enable Runtime Creates",
                default: true
            },
            {
                name: "compileFiles",
                displayName: "Compile Source Files",
                hidden: true,
                default: true
            },
            /* Provided for backward compatability */
            {
                name: "includeXdcRuntime",
                deprecated: true,
                default: true
            }
        ]
    },
    templates: {
        "/ti/sysbios/BIOS_config.h.xdt": true,
        "/ti/sysbios/BIOS_config.c.xdt": true,
        "/ti/utils/rov/syscfg_c.rov.xs.xdt": "/ti/sysbios/BIOS.rov.js",
        "/ti/utils/build/GenLibs.cmd.xdt":
            { modName: "/ti/sysbios/BIOS", getLibs: getLibs }
    },
    getCFiles: () => { return ["ti/sysbios/BIOS.c"] }
};

exports = base;
