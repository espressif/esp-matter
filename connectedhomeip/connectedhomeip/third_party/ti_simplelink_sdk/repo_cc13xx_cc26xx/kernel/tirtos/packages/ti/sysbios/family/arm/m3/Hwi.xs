/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 *  ======== Hwi.xs ========
 */

var Hwi = null;
var Memory = null;
var Startup = null;
var BIOS = null;
var Build = null;
var Core = null;
var deviceTable;
var deviceName;

/*
 * ======== getAsmFiles ========
 * return the array of assembly language files associated
 * with targetName (ie Program.build.target.$name)
 */
function getAsmFiles(targetName)
{
    switch(targetName) {
        case "ti.targets.arm.elf.M3":
        case "ti.targets.arm.elf.M4":
        case "ti.targets.arm.elf.M4F":
            return (["Hwi_asm.sv7M", "Hwi_asm_switch.sv7M"]);
            break;

        case "gnu.targets.arm.M3":
        case "gnu.targets.arm.M4":
        case "gnu.targets.arm.M4F":
        case "ti.targets.arm.clang.M3":
        case "ti.targets.arm.clang.M4":
        case "ti.targets.arm.clang.M4F":
            return (["Hwi_asm_gnu.sv7M", "Hwi_asm_switch_gnu.sv7M"]);
            break;

        case "iar.targets.arm.M3":
        case "iar.targets.arm.M4":
        case "iar.targets.arm.M4F":
            return (["Hwi_asm_iar.sv7M", "Hwi_asm_switch_iar.sv7M"]);
            break;

    default:
        return (null);
        break;
    }
}

/*
 * ======== getCFiles ========
 * return the array of C language files associated
 * with targetName (ie Program.build.target.$name)
 */
function getCFiles(targetName)
{
    if (BIOS.smpEnabled) {
        return (["Hwi_smp.c"]);
    }
    else {
        return (["Hwi.c"]);
    }
}

if (xdc.om.$name == "cfg") {
    deviceTable = {
        "CortexM3": {
            numInterrupts : 16 + 64,
            numPriorities : 8,
            resetVectorAddress : 0,
            vectorTableAddress : 0,
        },
        "OMAP4430": {
            numInterrupts : 16 + 64,
            numPriorities : 16,
            resetVectorAddress : 0,
            vectorTableAddress : 0,
        },
        "TDA3XX": {
            numInterrupts : 16 + 70,
            numPriorities : 16,
            resetVectorAddress : 0,
            vectorTableAddress : 0,
        },
        "LM3.*": {
            numInterrupts : 16 + 64,
            numPriorities : 8,
            resetVectorAddress : 0,
            vectorTableAddress : 0x20000000,
        },
        "LM4.*": {
            numInterrupts : 16 + 200,   /* up to 216 for Flurry devices */
            numPriorities : 8,
            resetVectorAddress : 0,
            vectorTableAddress : 0x20000000,
        },
        "TM4.*": {
            numInterrupts : 16 + 200,   /* up to 216 for Flurry devices */
            numPriorities : 8,
            resetVectorAddress : 0,
            vectorTableAddress : 0x20000000,
        },
        "MSP432P.*": {
            numInterrupts : 16 + 64,
            numPriorities : 8,
            resetVectorAddress : 0,
            vectorTableAddress : 0x20000000,
        },
        "MSP432E.*": {
            numInterrupts : 16 + 200,
            numPriorities : 8,
            resetVectorAddress : 0,
            vectorTableAddress : 0x20000000,
        },
        "SC.*": {
            numInterrupts : 16 + 200,   /* up to 216 for Blizzard devices */
            numPriorities : 8,
            resetVectorAddress : 0,
            vectorTableAddress : 0x20000000,
        },
        "F28M35.*": {
            numInterrupts : 16 + 92,            /* suppports 108 interrupts */
            numPriorities : 8,
            resetVectorAddress : 0x00200040,    /* placed low in flash */
            vectorTableAddress : 0x20000000,
        },
        "F28M36.*": {
            numInterrupts : 16 + 134,           /* supports 150 interrupts */
            numPriorities : 8,
            resetVectorAddress : 0x00200040,    /* placed low in flash */
            vectorTableAddress : 0x20000000,
        },
        "CC2538.*": {
            numInterrupts : 16 + 147,           /* supports 163 interrupts */
            numPriorities : 8,
            resetVectorAddress : 0x00200000,    /* placed low in flash */
            vectorTableAddress : 0x20000000,
        },
        "CC26.2.*": {
            numInterrupts : 16 + 38,            /* supports 54 interrupts */
            numPriorities : 8,
            resetVectorAddress : 0x0,           /* placed low in flash */
            vectorTableAddress : 0x20000000,
        },
        "CC26.1.*": {
            numInterrupts : 16 + 38,            /* supports 54 interrupts */
            numPriorities : 8,
            resetVectorAddress : 0x0,           /* placed low in flash */
            vectorTableAddress : 0x20000000,
        },
        "CC26.*": {
            numInterrupts : 16 + 34,            /* supports 50 interrupts */
            numPriorities : 8,
            resetVectorAddress : 0x0,           /* placed low in flash */
            vectorTableAddress : 0x20000000,
        },
        "CC3200": {
            numInterrupts : 16 + 179,
            numPriorities : 8,
            resetVectorAddress : 0x20004000,
            vectorTableAddress : 0x20000000,
        },
        "CC3220SF": {
            numInterrupts : 16 + 179,
            numPriorities : 8,
            resetVectorAddress : 0x01000800,
            vectorTableAddress : 0x20000000,
        },
        /*
         * CC32XX vector table placement must be done in the linker.cmd file.
         * Hwi.placeVectorTables config parameter must be 'false'!
         */
        "CC32XX": {
            numInterrupts : 16 + 179,
            numPriorities : 8,
            resetVectorAddress : 0x0,
            vectorTableAddress : 0x0,
        }
    }

    deviceTable["TMS470R10"]    = deviceTable["CortexM3"];

    deviceTable["TMS320CDM740"]  = deviceTable["OMAP4430"];
    deviceTable["Ducati"]        = deviceTable["OMAP4430"];
    deviceTable["TMS320TI811X"]  = deviceTable["OMAP4430"];
    deviceTable["TMS320TI813X"]  = deviceTable["OMAP4430"];
    deviceTable["TMS320TI814X"]  = deviceTable["OMAP4430"];
    deviceTable["TMS320TI816X"]  = deviceTable["OMAP4430"];
    deviceTable["TMS320DM385"]   = deviceTable["OMAP4430"];
    deviceTable["TMS320DM8168"]  = deviceTable["OMAP4430"];
    deviceTable["TMS320C6A8168"] = deviceTable["OMAP4430"];
    deviceTable["TMS320C6A8149"] = deviceTable["OMAP4430"];
    deviceTable["TMS320DM8148"]  = deviceTable["OMAP4430"];
    deviceTable["OMAP5430"]      = deviceTable["OMAP4430"];
    deviceTable["Vayu"]          = deviceTable["OMAP4430"];
    deviceTable["DRA7XX"]        = deviceTable["OMAP4430"];
    deviceTable["CC13.1.*"]      = deviceTable["CC26.1.*"];
    deviceTable["CC13.2.*"]      = deviceTable["CC26.2.*"];
    deviceTable["CC13.*"]        = deviceTable["CC26.*"];
    deviceTable["CC3235S"]       = deviceTable["CC3200"];
    deviceTable["CC3235SF"]      = deviceTable["CC3220SF"];
    deviceTable["CC32.*S"]       = deviceTable["CC3200"];
    deviceTable["CC32.*SF"]      = deviceTable["CC3220SF"];
}

/*
 *  ======== deviceSupportCheck ========
 *  Check validity of device
 */
function deviceSupportCheck()
{
    var deviceName;

    for (deviceName in deviceTable) {
        if (deviceName == Program.cpu.deviceName) {
            return deviceName;
        }
    }

    /* now look for wild card match */
    for (deviceName in deviceTable) {
        if (Program.cpu.deviceName.match(deviceName)) {
            return deviceName;
        }
    }

    Hwi.$logError("Unsupported device!", Program.cpu.deviceName);
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

    /* provide getAsmFiles() for Build.getAsmFiles() */
    this.$private.getAsmFiles = getAsmFiles;

    /* provide getCFiles() for Build.getCFiles() */
    this.$private.getCFiles = getCFiles;

    Hwi = this;

    /* set fxntab default */
    Hwi.common$.fxntab = false;

    var GetSet = xdc.module("xdc.services.getset.GetSet");
    GetSet.onSet(this, "enableException", _enableExceptionSet);
    GetSet.onSet(this, "excHandlerFunc", _excHandlerFuncSet);

    deviceName = deviceSupportCheck();

    /*
     * Most tiva derivative GNU linker cmd files require definitions for
     *  _intvecs_base_address and _vtable_base_address.
     * "isTiva = true" informs Hwi.xdt and Hwi_link.xdt to generate the required
     * linker cmd content for these devices.
     */
    if (Program.platformName.match(/ti\.platforms\.tiva/) ||
        Program.platformName.match(/ti\.platforms\.cc26xx/) ||
        Program.platformName.match(/ti\.platforms\.msp432/) ||
        Program.platformName.match(/ti\.platforms\.simplelink/) ||
        Program.platformName.match(/ti\.platforms\.cortexM/) ||
        Program.cpu.deviceName.match(/CC26/) ||
        Program.cpu.deviceName.match(/CC13/) ||
        Program.cpu.deviceName.match(/CC25/) ||
        Program.cpu.deviceName.match(/CC32/) ||
        Program.cpu.deviceName.match(/TM4/)) {
        Hwi.isTiva = true;
    }
    else {
        Hwi.isTiva = false;
    }

    Hwi.NUM_INTERRUPTS = deviceTable[deviceName].numInterrupts;
    Hwi.NUM_PRIORITIES = deviceTable[deviceName].numPriorities;
    Hwi.dispatchTableSize = deviceTable[deviceName].numInterrupts;
    Hwi.resetVectorAddress = deviceTable[deviceName].resetVectorAddress;
    Hwi.vectorTableAddress = deviceTable[deviceName].vectorTableAddress;

    Hwi.intAffinity.length = 256;

    /* by default, exception context is saved on local ISR stack */
    Hwi.excContextBuffer = 0;
    Hwi.excContextBuffers.length = 2;
    Hwi.excContextBuffers[0] = 0;
    Hwi.excContextBuffers[1] = 0;

    /* by default, the exception thread stack is NOT copied */
    Hwi.excStackBuffer = null;
    Hwi.excStackBuffers.length = 2;
    Hwi.excStackBuffers[0] = null;
    Hwi.excStackBuffers[1] = null;

    Hwi.excHookFuncs.length = 2;
    Hwi.excHookFuncs[0] = null;
    Hwi.excHookFuncs[1] = null;

    /* kill xdc runtime's .bootVecs */
    Program.sectMap[".bootVecs"] = new Program.SectionSpec();
    Program.sectMap[".bootVecs"].type = "DSECT";

    /*
     * Initialize meta-only Hwi object array
     */

    /*
     * Set the meta array size to support the max number of interrupts.
     * The actual number is defined by NUM_INTERRUPTS
     */
    Hwi.interrupt.length = 256;

    for (var intNum = 0; intNum < Hwi.interrupt.length; intNum++) {
        Hwi.interrupt[intNum].used = false;
        Hwi.interrupt[intNum].useDispatcher = false;
        Hwi.interrupt[intNum].fxn = null;
        Hwi.interrupt[intNum].name = "";

        /* all ints are mapped to core 0 by default */
        Hwi.intAffinity[intNum] = 0;
    }

    if (Program.build.target.$name.match(/iar\./)) {
        /* Don't get the vector table from rts library */
        var VectorTable = xdc.module('iar.targets.arm.rts.VectorTable');
        VectorTable.getVectorTableLib = false;

        Hwi.resetFunc = '&__iar_program_start';
    }
    else {
        Hwi.resetFunc = '&_c_int00';
    }

    switch (Hwi.NUM_PRIORITIES) {
        case 2:
            Hwi.disablePriority = 0x80;
            break;
        case 4:
            Hwi.disablePriority = 0x40;
            break;
        case 8:
            Hwi.disablePriority = 0x20;
            break;
        case 16:
            Hwi.disablePriority = 0x10;
            break;
        case 32:
            Hwi.disablePriority = 0x08;
            break;
        case 64:
            Hwi.disablePriority = 0x04;
            break;
        case 128:
            Hwi.disablePriority = 0x02;
            break;
        case 256:
        default:
            Hwi.disablePriority = 0x01;
            break;
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Startup = xdc.useModule('xdc.runtime.Startup');

    BIOS = xdc.useModule("ti.sysbios.BIOS");
    Build = xdc.module("ti.sysbios.Build");

    if (!(BIOS.libType == BIOS.LibType_Custom && BIOS.logsEnabled == false)) {
        xdc.useModule('xdc.runtime.Log');
    }

    if (BIOS.smpEnabled == true) {
        Core = xdc.module("ti.sysbios.hal.Core");
    }

    /* only useModule(Memory) if needed */
    var Defaults = xdc.module('xdc.runtime.Defaults');
    if (Defaults.common$.memoryPolicy ==
        xdc.module("xdc.runtime.Types").STATIC_POLICY) {
        Memory = xdc.module('xdc.runtime.Memory');
    }
    else {
        Memory = xdc.useModule('xdc.runtime.Memory');
    }

    if (Hwi.placeVectorTables && !Program.build.target.$name.match(/gnu/)) {
        /* create our .vecs & .resetVecs SectionSpecs */
        Program.sectMap[".resetVecs"] = new Program.SectionSpec();
        Program.sectMap[".vecs"] = new Program.SectionSpec();
        Program.sectMap[".vecs"].type = "NOLOAD";
    }

    if (Hwi.dispatcherSwiSupport == undefined) {
        Hwi.dispatcherSwiSupport = BIOS.swiEnabled;
    }
    if (Hwi.dispatcherTaskSupport == undefined) {
        Hwi.dispatcherTaskSupport = BIOS.taskEnabled;
    }
    if (Hwi.dispatcherSwiSupport) {
        if (BIOS.swiEnabled) {
            xdc.useModule("ti.sysbios.knl.Swi");
            Hwi.swiDisable = '&ti_sysbios_knl_Swi_disable__E';
            Hwi.swiRestore = '&ti_sysbios_knl_Swi_restore__E';
            Hwi.swiRestoreHwi = '&ti_sysbios_knl_Swi_restoreHwi__E';
        }
        else {
            Hwi.$logError("Dispatcher Swi support can't be enabled if ti.sysbios.BIOS.swiEnabled is false.", Hwi, "dispatcherSwiSupport");
        }
    }
    else {
        Hwi.swiDisable = '&ti_sysbios_family_arm_m3_Hwi_swiDisableNull';
        Hwi.swiRestore = '&ti_sysbios_family_arm_m3_Hwi_swiRestoreNull';
        Hwi.swiRestoreHwi = null;
    }

    if (Hwi.dispatcherTaskSupport) {
        if (BIOS.taskEnabled) {
            xdc.useModule("ti.sysbios.knl.Task");
            Hwi.taskDisable = '&ti_sysbios_knl_Task_disable__E';
            Hwi.taskRestoreHwi = '&ti_sysbios_knl_Task_restoreHwi__E';
        }
        else {
            Hwi.$logError ("Dispatcher Task support can't be enabled if ti.sysbios.BIOS.taskEnabled is false.", Hwi, "dispatcherTaskSupport");
        }
    }
    else {
        Hwi.taskDisable = null;
        Hwi.taskRestoreHwi = null;
    }

    /* Plug all non user-plugged exception handlers */
    if (Hwi.nmiFunc === undefined) {
        Hwi.nmiFunc = Hwi.excHandlerAsm;
    }
    if (Hwi.hardFaultFunc === undefined) {
        Hwi.hardFaultFunc = Hwi.excHandlerAsm;
    }
    if (Hwi.memFaultFunc === undefined) {
        Hwi.memFaultFunc = Hwi.excHandlerAsm;
    }
    if (Hwi.busFaultFunc === undefined) {
        Hwi.busFaultFunc = Hwi.excHandlerAsm;
    }
    if (Hwi.usageFaultFunc === undefined) {
        Hwi.usageFaultFunc = Hwi.excHandlerAsm;
    }
    if (Hwi.svCallFunc === undefined) {
        Hwi.svCallFunc = Hwi.excHandlerAsm;
    }
    if (Hwi.debugMonFunc === undefined) {
        Hwi.debugMonFunc = Hwi.excHandlerAsm;
    }
    if (Hwi.reservedFunc === undefined) {
        Hwi.reservedFunc = Hwi.excHandlerAsm;
    }
    if (Hwi.nullIsrFunc === undefined) {
        Hwi.nullIsrFunc = Hwi.excHandlerAsm;
    }


    if (Hwi.enableWA1_1 == true) {
        Hwi.$logError ("The Hwi.enableWA1_1 flag is not supported in this version of BIOS", Hwi, "enableWA1_1");
    }

    /*
     * After config file is parsed, use the final value
     * of Hwi.NUM_INTERRUPTS to correct the meta interrupt
     * table size which is used to generate the vector
     * table in Hwi.xdt.
     */
    Hwi.interrupt.length = Hwi.NUM_INTERRUPTS;

    /*
     * plug full exception decode func if enabled and excHandlerFunc
     * hasn't been set by the user
     */
    if (Hwi.$written("excHandlerFunc") == false) {
        if (Hwi.enableException) {
            Hwi.excHandlerFunc = Hwi.excHandlerMax;
        }
        else {
            Hwi.excHandlerFunc = Hwi.excHandlerMin;
        }
    }

    /*
     *  Register Hwi_initIsrStackSize as a first function. It will be called
     *  after cinit and initialize the ISR stack size before the stack size
     *  is used by Hwi module startup code.
     */
    if (Program.build.target.$name.match(/iar/)) {
        var len = Startup.firstFxns.length
        Startup.firstFxns.length++;
        Startup.firstFxns[len] =
            '&ti_sysbios_family_arm_m3_Hwi_initIsrStackSize';
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{

    if (BIOS.smpEnabled == true) {
        var numCores = Core.numCores;
        mod.intAffinity.length = Hwi.NUM_INTERRUPTS;
        mod.intAffinityMasks.length = Core.numCores;

        for (var i = 0; i < Core.numCores; i++) {
            mod.intAffinityMasks[i].length = (Hwi.interrupt.length + 15) / 32;
        }

        for (var i = 0; i < mod.intAffinityMasks.length; i++) {
            for (var j = 0; j < mod.intAffinityMasks[i].length; j++) {
                mod.intAffinityMasks[i][j] = 0;
            }
        }

        for (var i = 0; i < mod.intAffinity.length; i++) {
            if (Hwi.intAffinity[i] < Core.numCores) {
                mod.intAffinity[i] = Hwi.intAffinity[i];
                if (i >= 16) {
                    var index = (i-16) >> 5;
                    var mask = 1 << ((i-16) & 0x1f);
                    mod.intAffinityMasks[Hwi.intAffinity[i]][index] |= mask;
                }
            }
            else {
                Hwi.$logError("Invalid core Id (" + Hwi.intAffinity[i] +
                ") for Hwi " + i + ". Core ids must be less than " +
                Core.numCores + ".", this, "intAffinity");
            }
        }
    }
    else {
        var numCores = 1;
        mod.intAffinity.length = 0;
        mod.intAffinityMasks.length = 0;
    }

    mod.swiTaskKeys = 0x00000101;

    mod.vnvicFlushRequired = false;

    mod.taskSP = null;

    mod.excActive.length = numCores;
    mod.excContext.length = numCores;
    mod.excStack.length = numCores;

    if (Hwi.excContextBuffer != 0) {
        Hwi.excContextBuffers[0] = Hwi.excContextBuffer;
    }

    if (Hwi.excStackBuffer != null) {
        Hwi.excStackBuffers[0] = Hwi.excStackBuffer;
    }

    if (Hwi.excHookFunc != null) {
        Hwi.excHookFuncs[0] = Hwi.excHookFunc;
    }

    for (var i = 0; i < numCores; i++) {

        mod.excActive[i] = false;

        if (Hwi.excContextBuffers[i] != 0) {
            mod.excContext[i] = $addr(Hwi.excContextBuffers[i]);
        }
        else {
            mod.excContext[i] = null;
        }
        if (Hwi.excStackBuffers[i] != null) {
            mod.excStack[i] = Hwi.excStackBuffers[i];
        }
        else {
            mod.excStack[i] = null;
        }
    }

    mod.isrStack = null;
    /* Overriden by Hwi_initIsrStackSize() if IAR */
    if (Program.build.target.$name.match(/iar/)) {
        mod.isrStackBase = null;
        mod.isrStackSize = null;
    }
    else {
        mod.isrStackBase = $externPtr('__TI_STACK_BASE');
        mod.isrStackSize = $externPtr('__TI_STACK_SIZE');
    }

    mod.dispatchTable = $externPtr('ti_sysbios_family_arm_m3_Hwi_dispatchTable[0]');

    Hwi.ccr = (params.nvicCCR.STKALIGN << 9) |
              (params.nvicCCR.BFHFNMIGN << 8) |
              (params.nvicCCR.DIV_0_TRP << 4) |
              (params.nvicCCR.UNALIGN_TRP << 3) |
              (params.nvicCCR.USERSETMPEND << 1) |
              (params.nvicCCR.NONEBASETHRDENA << 0);

    /* Validate Hwi.priGroup */
    if (Hwi.priGroup > 7) {
        Hwi.$logError("Hwi.priGroup = " + Hwi.priGroup + " but must be between 0 - 7", this, "priGroup");
    }

    /*
     * Non GNU targets have to deal with legacy config files
     */
    if (Hwi.placeVectorTables && !Program.build.target.$name.match(/gnu/)) {

        /*
         * Some legacy config files explicitly place the vector table sections
         * rather than setting Hwi.vectorTableAddress and Hwi.resetVectorAddress.
         * The ugly logic below attempts to achieve the desired config goal and
         * end up with a coherent configuration.
         */

        if (Program.sectMap[".resetVecs"].loadAddress === undefined) {
            Program.sectMap[".resetVecs"].loadAddress = Hwi.resetVectorAddress;
        }
        else {
            if (Hwi.resetVectorAddress !=
                        Program.sectMap[".resetVecs"].loadAddress) {
                /* force config to match section placement */
                Hwi.$unseal("resetVectorAddress");
                Hwi.resetVectorAddress = Program.sectMap[".resetVecs"].loadAddress;
            }
        }

        if (Program.sectMap[".vecs"].loadAddress === undefined) {
            Program.sectMap[".vecs"].loadAddress = Hwi.vectorTableAddress;
        }
        else {
            if (Hwi.vectorTableAddress
                        != Program.sectMap[".vecs"].loadAddress) {
                /* force config match section placement */
                Hwi.$unseal("vectorTableAddress");
                Hwi.vectorTableAddress = Program.sectMap[".vecs"].loadAddress;
            }
        }

        /* remove empty .vecs section if it isn't used */
        if (Hwi.resetVectorAddress == Hwi.vectorTableAddress) {
            Program.sectMap[".vecs"].type = "DSECT";
        }
    }

    /* Initialize the NVIC early */
    if ((Hwi.resetVectorAddress != 0) && (Build.buildROMApp == true)) {
        /* Fix for SDOCM00114681: broken Hwi_initNVIC() function. */
        Startup.firstFxns.$add(Hwi.romInitNVIC);
    }
    else {
        Startup.firstFxns.$add(Hwi.initNVIC);
    }

    if ((Hwi.vectorTableAddress != Hwi.resetVectorAddress) ||
            (Hwi.placeVectorTables == false)) {
        mod.vectorTableBase = $externPtr('ti_sysbios_family_arm_m3_Hwi_ramVectors[0]');
    }
    else {
        mod.vectorTableBase = Hwi.vectorTableAddress;
    }

    if (Hwi.vectorTableAddress > 0x3fffc000) {
        Hwi.$logError("Vector Table must be placed at or below 0x3FFFFC00",
                    this);
    }

    if (Hwi.dispatchTableSize < Hwi.NUM_INTERRUPTS) {
        Hwi.numSparseInterrupts = Hwi.dispatchTableSize;
        if (Hwi.dispatchTableSize > Hwi.NUM_INTERRUPTS) {
            Hwi.$logError("(" + Hwi.dispatchTableSize + ") " +
            "must be less than or equal to Hwi.NUM_INTERRUPTS: (" +
            Hwi.NUM_INTERRUPTS + ")",
             this, "dispatchTableSize");
        }
        else if (Hwi.dispatchTableSize > Hwi.NUM_INTERRUPTS / 3) {
            Hwi.$logWarning("(" + Hwi.dispatchTableSize + ") " +
            "should only be set to a value less than 1/3 " +
            "Hwi.NUM_INTERRUPTS: (" + Hwi.NUM_INTERRUPTS + ")",
             this, "dispatchTableSize");
        }

        /* place msp432's sparse dispatchTable in SRAM_CODE */
        if (Program.platformName.match(/ti\.platforms\.msp432/)) {
            if (!(Program.cpu.deviceName.match(/MSP432E/))) {
            Build.ccArgs.$add("-Dti_sysbios_family_arm_m3_Hwi_FIX_MSP432_DISPATCH_TABLE_ADDRESS");
            }
        }
    }

    /* add -D to compile line to optimize exception code */
    Build.ccArgs.$add("-Dti_sysbios_family_arm_m3_Hwi_enableException__D=" +
        (Hwi.enableException ? "TRUE" : "FALSE"));

    if (BIOS.buildingAppLib == true) {
        /* add -D to compile line to optimize exception code */
        Build.ccArgs.$add("-Dti_sysbios_family_arm_m3_Hwi_disablePriority__D=" +
            Hwi.disablePriority + "U");

        if (Build.buildROM == false) {
            /* add -D to compile line to optimize sparse interrupt handling code */
            Build.ccArgs.$add("-Dti_sysbios_family_arm_m3_Hwi_numSparseInterrupts__D=" +
                Hwi.numSparseInterrupts + "U");
        }
    }
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, intNum, fxn, params)
{
    var mod = this.$module.$object;

    if (intNum >= Hwi.NUM_INTERRUPTS) {
        Hwi.$logError("intnum " + intNum + " is out of range!", this, intNum);
    }

    if (intNum < 15) {
        Hwi.$logError("Only intNums > = 15 can be created.", this, intNum);
    }

    if (Hwi.interrupt[intNum].used == true) {
        Hwi.$logError("Hwi " + intNum + " already in use (by " +
                Hwi.interrupt[intNum].fxn + ").", this);
    }

    Hwi.interrupt[intNum].used = true;
    Hwi.interrupt[intNum].fxn = fxn;

    obj.arg = params.arg;
    obj.fxn = fxn;
    obj.intNum = intNum;

    if (params.priority != -1) {
        obj.priority = params.priority;
    }
    else {
        obj.priority = 255;
    }

    Hwi.interrupt[intNum].priority = obj.priority;

    /*
     * obj.irp field is overloaded during initialization
     * to reduce Hwi Object footprint
     *
     * for postInit(), encode irp with enableInt
     * and useDispatcher info.
     */

    obj.irp = 0;

    if (params.enableInt == true) {
        obj.irp |= 0x1;
    }

    if (params.useDispatcher == true) {
        obj.irp |= 0x2;
    }

    /* Zero latency interrupts don't use the dispatcher */
    if ((params.useDispatcher == false) ||
        (obj.priority < Hwi.disablePriority)) {
        Hwi.interrupt[intNum].useDispatcher = false;
    }
    else {
        Hwi.interrupt[intNum].useDispatcher = true;
        Hwi.interrupt[intNum].hwi = this;
    }

    /* Quietly allow any maskSetting to go through */
//    if (params.maskSetting != Hwi.MaskingOption_LOWER) {
//        Hwi.$logWarning("For all Cortex M devices, BIOS only supports "
//                         + "Hwi.MaskingOption_LOWER. All other maskSettings "
//                         + "are ignored.",
//                        this);
//    }

    obj.hookEnv.length = Hwi.hooks.length;
}

/*
 *  ======== inUseMeta ========
 */
function inUseMeta(intNum)
{
    return Hwi.interrupt[intNum].used;
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
 */
function module$validate()
{
    if (Hwi.placeVectorTables && Program.cpu.deviceName == "CC32XX") {
        Hwi.$logError("Hwi.placeVectorTables must be 'false' for CC32XX.", Hwi, "placeVectorTables");
    }

    /* validate all "created" instances */
    for (var i = 0; i < Hwi.$instances.length; i++) {
        instance_validate(Hwi.$instances[i]);
    }

    /* validate all "constructed" instances */
    for (var i = 0; i < Hwi.$objects.length; i++) {
        instance_validate(Hwi.$objects[i]);
    }
}

/*
 *  ======== instance_validate ========
 *  common function to test instance configuration
 */
function instance_validate(instance)
{
    if (instance.$object.fxn == null) {
        Hwi.$logError("function cannot be null", instance);
    }
}

/*
 *  ======== viewScanDispatchTable ========
 *  Scans dispatch table for constructed Hwis to add them to the Hwi ROV view.
 *
 *  The Hwi dispatch table is scanned for handles that are not in
 *  the raw instance view. These Hwi objects are then manually added to
 *   ROV's scanned object list.
 *
 *  This function does not perform any error handling because it has nowhere
 *  to display an error. If any of the APIs called within this function throw
 *  an exception, it will propagate up and be displayed to the user in ROV.
 */
function viewScanDispatchTable(data, viewLevel)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Hwi = xdc.useModule('ti.sysbios.family.arm.m3.Hwi');

    /* Check if the constructed tasks have already been scanned. */
    if (data.scannedConstructedHwis) {
        return;
    }

    /*
     * Set the flag to true now to prevent recursive calls of this function
     * when we scan the constructed tasks.
     */
    data.scannedConstructedHwis = true;

    /* Get the Task module config to get the number of constructed tasks. */
    var modCfg = Program.getModuleConfig('ti.sysbios.family.arm.m3.Hwi');

    /* sparse dispatchTable not supported yet */
    if (modCfg.numSparseInterrupts != 0) return;

    var numHwis = modCfg.NUM_INTERRUPTS;

    /*
     * Retrieve the raw view to get at the module state.
     * This should just return, we don't need to catch exceptions.
     */
    var rawView = Program.scanRawView('ti.sysbios.family.arm.m3.Hwi');

    var dispatchTableAddr = rawView.modState.dispatchTable;

    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');

    /* Retrieve the dispatchTable array of handles */
    var hwiHandles = Program.fetchArray(ScalarStructs.S_Ptr$fetchDesc,
                                         dispatchTableAddr, numHwis);

    /*
     * Scan the dispatchTable for non-zero Hwi handles
     */
    for (var i=15; i < numHwis; i++) {
        var hwiHandle = hwiHandles[i];
        if (Number(hwiHandle.elem) != 0) {
            var alreadyScanned = false;
            /* skip Hwis that are already known to ROV */
            for (var j in rawView.instStates) {
                var rawInstance = rawView.instStates[j];
                if (Number(rawInstance.$addr) == Number(hwiHandle.elem)) {
                    alreadyScanned = true;
                    break;
                }
            }
            if (alreadyScanned == false) {
                /* Retrieve the embedded instance */
                var obj = Program.fetchStruct(Hwi.Instance_State$fetchDesc, hwiHandle.elem);
                /*
                * Retrieve the view for the object. This will automatically add the
                * object to the instance list.
                */
                Program.scanObjectView('ti.sysbios.family.arm.m3.Hwi', obj, viewLevel);
            }
        }
    }
}

var modView = null;

/*
 *  ======== viewGetPriority ========
 */
function viewGetPriority(view, that, intNum)
{
    var priority = 0;
    var registerBaseAddr;

    try {
        registerBaseAddr = 0xe000e400;
        that.IPR = Program.fetchArray(
            {
                type: 'xdc.rov.support.ScalarStructs.S_UInt8',
                isScalar: true
            },
            registerBaseAddr,
            240,
            false);

        registerBaseAddr = 0xe000ed18;
        that.SHPR = Program.fetchArray(
            {
                type: 'xdc.rov.support.ScalarStructs.S_UInt8',
                isScalar: true
            },
            registerBaseAddr,
            12,
            false);

        if (intNum >= 16) {
            priority = that.IPR[intNum-16];
        }
        else if (intNum >= 4) {
            priority = that.SHPR[intNum-4];
        }
    }
    catch (e) {
        print("Error: Problem fetching priorities: " + e.toString());

        Program.displayError(view, "priority",  "Unable to read Hwi priority " +
            "registers at 0x" + registerBaseAddr.toString(16));
    }

    return priority;
}

/*
 *  ======== viewNvicFetch ========
 *  Called from viewInitModule()
 */
function viewNvicFetch(that)
{
    try {
        that.ISER = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt', isScalar: true}, 0xe000e100, 8, false);
        that.ISPR = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt', isScalar: true}, 0xe000e200, 8, false);
        that.IABR = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt', isScalar: true}, 0xe000e300, 8, false);
        that.ICSR = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt', isScalar: true}, 0xe000ed04, 1, false);
        that.STCSR = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt', isScalar: true}, 0xe000e010, 1, false);
        that.SHCSR = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt', isScalar: true}, 0xe000ed24, 1, false);
        that.VTOR = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt', isScalar: true}, 0xe000ed08, 1, false);
    }
    catch (e) {
        print("Error: Problem fetching NVIC: " + e.toString());
    }
}

/* used by ROV view Code */
var subPriMasks = [0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff];

/* used by ROV view Code */
var numPriTable = {
    "2" : {
        mask : 0x80,
        shift : 7
    },
    "4" : {
        mask : 0xc0,
        shift : 6
    },
    "8" : {
        mask : 0xe0,
        shift : 5
    },
    "16" : {
        mask : 0xf0,
        shift : 4
    },
    "32" : {
        mask : 0xf8,
        shift : 3
    },
    "64" : {
        mask : 0xfc,
        shift : 2
    },
    "128" : {
        mask : 0xfe,
        shift : 1
    },
    "256" : {
        mask : 0xff,
        shift : 0
    }
}

/*
 *  ======== viewFillBasicInfo ========
 *  Fill in the 'Basic' Task instance view.
 */
function viewFillBasicInfo(view, obj)
{
    var Hwi = xdc.useModule('ti.sysbios.family.arm.m3.Hwi');
    var Program = xdc.useModule('xdc.rov.Program');
    var halHwi = xdc.useModule('ti.sysbios.hal.Hwi');
    var hwiModCfg = Program.getModuleConfig('ti.sysbios.family.arm.m3.Hwi');

    var pri = viewGetPriority(view, this, Math.abs(obj.intNum));

    var mask = numPriTable[hwiModCfg.NUM_PRIORITIES].mask;

    var shift = numPriTable[hwiModCfg.NUM_PRIORITIES].shift;

    view.priority = pri;

    if (hwiModCfg.priGroup + 1 > shift) {
        view.group = pri >> (hwiModCfg.priGroup + 1);
    }
    else {
        view.group = pri >> shift;
    }

    view.subPriority = (pri & subPriMasks[hwiModCfg.priGroup]) >> shift;
    view.halHwiHandle =  halHwi.viewGetHandle(obj.$addr);
    if (view.halHwiHandle != null) {
        view.label = Program.getShortName(halHwi.viewGetLabel(obj.$addr));
    }
    else {
        view.label = Program.getShortName(obj.$label);
    }
    view.intNum = Math.abs(obj.intNum);

    if (obj.intNum >= 0) {
        view.type = "Dispatched";
        if (view.priority < hwiModCfg.disablePriority) {
            view.$status["type"] = view.$status["priority"] =
                "Unsafe! This dispatched interrupt " +
                "has a zero latency interrupt priority and is " +
                "not disabled by Hwi_disable()!";
        }
    }
    else {
        if (view.priority < hwiModCfg.disablePriority) {
            view.type = "Zero Latency";
        }
        else {
            view.type = "Non Dispatched";
        }
    }

    var fxn = Program.lookupFuncName(Number(obj.fxn));
    if (fxn.length > 1) {
        view.fxn = fxn[1]; /* blow off FUNC16/32 */
    }
    else {
        view.fxn = fxn[0];
    }
    view.arg = obj.arg;
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
    var Hwi = xdc.useModule('ti.sysbios.family.arm.m3.Hwi');

    if (viewCheckForNullObject(Hwi, obj)) {
        view.halHwiHandle = "Uninitialized Hwi object";
        return;
    }

    /* Add constructed Hwis to ROV object list */
    viewScanDispatchTable(this, 'Basic');

    viewFillBasicInfo(view, obj);
}

/*
 *  ======== viewInitDetailed ========
 *  Initialize the 'Detailed' Task instance view.
 */
function viewInitDetailed(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Hwi = xdc.useModule('ti.sysbios.family.arm.m3.Hwi');
    var hwiModConfig = Program.getModuleConfig(Hwi.$name);
    var BIOS = xdc.useModule('ti.sysbios.BIOS');
    var biosModConfig = Program.getModuleConfig(BIOS.$name);

    /* Add constructed Hwis to ROV object list */
    viewScanDispatchTable(this, 'Detailed');

    if (viewCheckForNullObject(Hwi, obj)) {
        view.halHwiHandle = "Uninitialized Hwi object";
        return;
    }

    /* Detailed view builds off basic view. */
    viewFillBasicInfo(view, obj);

    view.irp = obj.irp;

    viewNvicFetch(this);

    var enabled = false;
    var active = false;
    var pending = false;

    if (view.intNum >= 16) {
        var index = (view.intNum-16) >> 5;
        var mask = 1 << ((view.intNum-16) & 0x1f);
        enabled = this.ISER[index] & mask;
        active = this.IABR[index] & mask;
        pending = this.ISPR[index] & mask;
    }
    else {
        switch(view.intNum) {
            case 15: /* SysTick */
                pending = this.ICSR & 0x100000000;
                enabled = this.STCSR & 0x00000002;
                active = this.SHCSR & 0x00000800;
                break;
            default:
                view.status = "unknown";
                return;
                break;
        }
    }

    if (enabled) {
        view.status = "Enabled";
    }
    else {
        view.status = "Disabled";
    }

    if (active) {
        view.status += ", Active";
    }

    if (pending) {
        view.status += ", Pending";
    }

    if (biosModConfig.smpEnabled == true) {
        view.coreId = hwiModConfig.intAffinity[view.intNum];
    }
    else {
        view.coreId = 0;
    }
}

/*!
 *  ======== viewGetStackInfo ========
 */
function viewGetStackInfo()
{
    var IHwi = xdc.useModule('ti.sysbios.interfaces.IHwi');
    var Program = xdc.useModule('xdc.rov.Program');
    var Hwi = xdc.useModule('ti.sysbios.family.arm.m3.Hwi');

    var stackInfo = new IHwi.StackInfo();

    /* Fetch needed info from Hwi module state */
    try {
        var hwiRawView = Program.scanRawView('ti.sysbios.family.arm.m3.Hwi');
        var size = Number(hwiRawView.modState.isrStackSize);
        var stackBase = hwiRawView.modState.isrStackBase;
        var stackData = Program.fetchArray(
            {
                type: 'xdc.rov.support.ScalarStructs.S_UChar',
                isScalar: true
            }, stackBase, size);
    }
    catch (e) {
        stackInfo.hwiStackSize = 0;     /* signal error to caller */
        return (stackInfo);
    }

    var index = 0;

    /*
     * The stack is filled with 0xbe.
     */
    while (stackData[index] == 0xbe) {
        index++;
    }

    stackInfo.hwiStackPeak = size - index;
    stackInfo.hwiStackSize = size;
    stackInfo.hwiStackBase = Number(stackBase);

    return (stackInfo);
}

/*
 *  ======== viewDecodeNMI ========
 */
function viewDecodeNMI(excContext)
{
    return("NMI Exception");
}

/*
 *  ======== viewDecodeHardFault ========
 */
function viewDecodeHardFault(excContext)
{
    var fault = "Hard Fault: ";

    if (excContext.HFSR & 0x40000000) {
        fault += "FORCED: ";
        fault += viewDecodeUsageFault(excContext);
        fault += viewDecodeBusFault(excContext);
        fault += viewDecodeMemFault(excContext);
        return (fault);
    }
    else if (excContext.HFSR & 0x80000000) {
        fault += "DEBUGEVT: ";
        fault += viewDecodeDebugMon(excContext);
        return (fault);
    }
    else if (excContext.HFSR & 0x00000002) {
        fault += "VECTBL";
    }
    else {
        fault += "Unknown";
    }

    return (fault);
}

/*
 *  ======== viewDecodeMemFault ========
 */
function viewDecodeMemFault(excContext)
{
    var fault = ""

    if (excContext.MMFSR != 0) {

        fault = "MEMFAULT: ";
        if (excContext.MMFSR & 0x10) {
            fault += "MSTKERR";
        }
        else if (excContext.MMFSR & 0x08) {
            fault += "MUNSTKERR";
        }
        else if (excContext.MMFSR & 0x02) {
            fault += "DACCVIOL ";
            fault += "Data Access Error. Address = 0x" + Number(excContext.MMAR).toString(16);
        }
        else if (excContext.MMFSR & 0x01) {
            fault += "IACCVIOL ";
            fault += "Instruction Fetch Error. Address = 0x" + Number(excContext.MMAR).toString(16);
        }
        else {
            fault += "Unknown";
        }
    }
    return (fault);
}

/*
 *  ======== viewDecodeBusFault ========
 */
function viewDecodeBusFault(excContext)
{
    var fault = ""

    if (excContext.BFSR != 0) {

        fault = "BUSFAULT: ";

        if (excContext.BFSR & 0x10) {
            fault += "STKERR";
        }
        else if (excContext.BFSR & 0x08) {
            fault += "UNSTKERR";
        }
        else if (excContext.BFSR & 0x04) {
            fault += "IMPRECISERR";
        }
        else if (excContext.BFSR & 0x02) {
            fault += "PRECISERR.";
            fault += "Data Access Error. Address = 0x" + Number(excContext.BFAR).toString(16);
        }
        else if (excContext.BFSR & 0x01) {
            fault += "IBUSERR";
        }
        else {
            fault += "Unknown";
        }
    }
    return (fault);
}

/*
 *  ======== viewDecodeUsageFault ========
 */
function viewDecodeUsageFault(excContext)
{
    var fault = ""

    if (excContext.UFSR != 0) {
        fault = "USAGE: ";
        if (excContext.UFSR & 0x0001) {
            fault += "UNDEFINSTR";
        }
        else if (excContext.UFSR & 0x0002) {
            fault += "INVSTATE";
        }
        else if (excContext.UFSR & 0x0004) {
            fault += "INVPC";
        }
        else if (excContext.UFSR & 0x0008) {
            fault += "NOCP";
        }
        else if (excContext.UFSR & 0x0100) {
            fault += "UNALIGNED";
        }
        else if (excContext.UFSR & 0x0200) {
            fault += "DIVBYZERO";
        }
        else {
            fault += "Unknown";
        }
    }
    return (fault);
}

/*
 *  ======== viewDecodeSvCall ========
 */
function viewDecodeSvCall(excContext)
{
    return("SV Call Exception, pc = " + Number(excContext.pc).toString(16));
}

/*
 *  ======== viewDecodeDebugMon ========
 */
function viewDecodeDebugMon(excContext)
{
    var fault = "";

    if (excContext.DFSR != 0) {

        fault = "DEBUG: ";

        if (excContext.DFSR & 0x00000010) {
            fault += "EXTERNAL";
        }
        else if (excContext.DFSR & 0x00000008) {
            fault += "VCATCH";
        }
        else if (excContext.DFSR & 0x00000004) {
            fault += "DWTTRAP";
        }
        else if (excContext.DFSR & 0x00000002) {
            fault += "BKPT";
        }
        else if (excContext.DFSR & 0x00000001) {
            fault += "HALTED";
        }
        else {
            fault += "Unknown";
        }
    }
    return (fault);
}

/*
 *  ======== viewDecodeReserved ========
 */
function viewDecodeReserved(excContext, excNum)
{
    return ("Reserved vector: " + String(excNum));
}

/*
 *  ======== viewDecodeNoIsr ========
 */
function viewDecodeNoIsr(excContext, excNum)
{
    return ("Undefined Hwi: " + String(excNum));
}

/*
 *  ======== viewDecodeException ========
 */
function viewDecodeException(excContext)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Hwi = xdc.useModule('ti.sysbios.family.arm.m3.Hwi');

    var excNum = String(excContext.ICSR & 0xff);

    switch (excNum) {
        case "2":
            return (viewDecodeNMI(excContext));         /* NMI */
            break;
        case "3":
            return (viewDecodeHardFault(excContext));   /* Hard Fault */
            break;
        case "4":
            return (viewDecodeMemFault(excContext));    /* Mem Fault */
            break;
        case "5":
            return (viewDecodeBusFault(excContext));    /* Bus Fault */
            break;
        case "6":
            return (viewDecodeUsageFault(excContext));  /* Usage Fault */
            break;
        case "11":
            return (viewDecodeSvCall(excContext));      /* SVCall */
            break;
        case "12":
            return (viewDecodeDebugMon(excContext));    /* Debug Mon */
            break;
        case "7":
        case "8":
        case "9":
        case "10":
        case "13":
            return (viewDecodeReserved(excContext, excNum)); /* reserved */
            break;
        default:
            return (viewDecodeNoIsr(excContext, excNum));       /* no ISR */
            break;
    }
    return (null);
}

/*
 *  ======== viewCallStack ========
 */
function viewCallStack(excContext) {
    try {
        var CallStack = xdc.useModule('xdc.rov.CallStack');
        CallStack.clearRegisters();
    }
    catch (e) {
        return (null);
    }

    CallStack.setRegister("SP", Number(excContext["sp"]));
    CallStack.setRegister("R13", Number(excContext["sp"]));
    CallStack.setRegister("R14", Number(excContext["lr"]));
    CallStack.setRegister("PC", Number(excContext["pc"]));

    CallStack.setRegister("R0", Number(excContext["r0"]));
    CallStack.setRegister("R1", Number(excContext["r1"]));
    CallStack.setRegister("R2", Number(excContext["r2"]));
    CallStack.setRegister("R3", Number(excContext["r3"]));
    CallStack.setRegister("R4", Number(excContext["r4"]));
    CallStack.setRegister("R5", Number(excContext["r5"]));
    CallStack.setRegister("R6", Number(excContext["r6"]));
    CallStack.setRegister("R7", Number(excContext["r7"]));
    CallStack.setRegister("R8", Number(excContext["r8"]));
    CallStack.setRegister("R9", Number(excContext["r9"]));
    CallStack.setRegister("R10", Number(excContext["r10"]));
    CallStack.setRegister("R11", Number(excContext["r11"]));
    CallStack.setRegister("R12", Number(excContext["r12"]));

    /* fetch back trace string */
    var frames = CallStack.toText();

    /* break up into separate lines */
    frames = frames.split("\n");

    /*
     * Strip off "Unwind halted ... " from last frame
     */
    frames.length -= 1;

    for (var i = 0; i < frames.length; i++) {
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

    for (var i = 0; i < frames.length; i++) {
        invframes[frames[i].substring(0,frames[i].indexOf("PC")-1)] =
            frames[i].substr(frames[i].indexOf("PC"));
    }

    return (invframes);
}

/*
 *  ======== viewFillExceptionContext ========
 */
function viewFillExceptionContext(excContext)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var CallStack = xdc.useModule('xdc.rov.CallStack');
    var excReg, lrReg;

    CallStack.fetchRegisters(["R0", "R1", "R2"]);

    /* Use address of Hwi_excHandlerFunc to determine which register contains excContext address */
    var excHandlerAddress = Program.lookupSymbolValue("ti_sysbios_family_arm_m3_Hwi_excHandler__I");
    excHandlerAddress &= 0xfffffffe; /* mask off LSB to avoid any tool chain differences */

    /* determine which registers are pointing to the excContext and LR */
    switch (excHandlerAddress) {
        case 0x1003ab6c:    /* CC13X2/CC26X2 ROM */
        case 0x1002ddf4:    /* CC13X2V2/CC26X2V2 ROM */
            excReg = "R2";
            lrReg = "R1";
            break;
        default:            /* all other ROMs and non-ROM builds */
            excReg = "R0";
            lrReg = "R1";
            break;
    }

    excContext.$addr = "N/A"; /* excContext is not in physical memory */

    /* use $addr() to force ROV to render in HEX */
    excContext.sp = $addr(CallStack.getRegister(excReg) + 16);

    /* excReg points to registerContext saved by ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I */
    var registerContext = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_Ptr', isScalar: true}, CallStack.getRegister(excReg), 16, false);


    /* r4-r11 registers were programmatically pushed */
    /* untouched S_Ptr types render as HEX */
    excContext.r4 = registerContext[0];
    excContext.r5 = registerContext[1];
    excContext.r6 = registerContext[2];
    excContext.r7 = registerContext[3];
    excContext.r8 = registerContext[4];
    excContext.r9 = registerContext[5];
    excContext.r10 = registerContext[6];
    excContext.r11 = registerContext[7];

    /* first 8 words are exception frame saved by M3/M4 during exception */
    excContext.r0 = registerContext[8];
    excContext.r1 = registerContext[9];
    excContext.r2 = registerContext[10];
    excContext.r3 = registerContext[11];
    excContext.r12 = registerContext[12];
    excContext.lr = registerContext[13];
    excContext.pc = registerContext[14];
    excContext.psr = registerContext[15];

    var ICSR = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_Ptr', isScalar: true}, 0xe000ed04, 1, false);
    excContext.ICSR = ICSR[0];

    /* fetch 6 words starting at MMFSR */
    var nvicRegs = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_Ptr', isScalar: true}, 0xe000ed28, 6, false);

    /* use $addr() to force ROV to render in HEX after math is performed on the S_Ptr */
    excContext.MMFSR = $addr(nvicRegs[0] & 0xff);
    excContext.BFSR = $addr((nvicRegs[0] & 0x0000ff00) >> 8);
    excContext.UFSR = $addr((nvicRegs[0] & 0xffff0000) >> 16);

    /* untouched S_Ptr types render as HEX */
    excContext.HFSR = nvicRegs[1];
    excContext.DFSR = nvicRegs[2];
    excContext.MMAR = nvicRegs[3];
    excContext.BFAR = nvicRegs[4];
    excContext.AFSR = nvicRegs[5];

    /* get BIOS module view to retrieve BIOS ThreadType */
    var biosModView = Program.scanModuleView('ti.sysbios.BIOS', 'Module');

    switch(biosModView.currentThreadType[0]) {
        case "Task":
            var taskModView = Program.scanModuleView('ti.sysbios.knl.Task', 'Module');
            excContext.threadType = "Task";
            excContext.threadHandle = taskModView.currentTask[0];
            var taskRawView = Program.scanRawView('ti.sysbios.knl.Task');
            for (var i in taskRawView.instStates) {
                if (Number(taskRawView.instStates[i].$addr) == Number(taskModView.currentTask[0])) {
                    excContext.threadStack = taskRawView.instStates[i].stack;
                    excContext.threadStackSize = taskRawView.instStates[i].stackSize;
                    break;
                }
            }
            break;

        case "Swi":
            excContext.threadType = "Swi";
            var swiModView = Program.scanModuleView('ti.sysbios.knl.Swi', 'Module');
            excContext.threadHandle = swiModView.currentSwi;
            break;

        case "Hwi":
            excContext.threadType = "Hwi";
            excContext.threadHandle = "unknown";
            var hwiRawView = Program.scanRawView('ti.sysbios.family.arm.m3.Hwi');
            for (var i in hwiRawView.instStates) {
                if (Number(hwiRawView.instStates[i].intNum) == (excContext.psr & 0xff)) {
                    excContext.threadHandle = hwiRawView.instStates[i].$addr;
                    break;
                }
            }
            break;

        case "Main":
            excContext.threadType = "Main";
            excContext.threadHandle = $addr(0);
            break;
    }

    switch(biosModView.currentThreadType[0]) {
        case "Swi":
        case "Hwi":
        case "Main":
            var hwiRawView = Program.scanRawView('ti.sysbios.family.arm.m3.Hwi');
            excContext.threadStack = hwiRawView.modState.isrStackBase;
            excContext.threadStackSize = hwiRawView.modState.isrStackSize;
            break;
    }
}

/*
 *  ======== viewInitException ========
 */
function viewInitException()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Hwi = xdc.useModule('ti.sysbios.family.arm.m3.Hwi');
    var hwiModConfig = Program.getModuleConfig(Hwi.$name);

    var BIOS = xdc.useModule('ti.sysbios.BIOS');
    var biosModConfig = Program.getModuleConfig(BIOS.$name);

    if (biosModConfig.smpEnabled == true) {
        var Core = xdc.useModule('ti.sysbios.hal.Core');
        var coreModConfig = Program.getModuleConfig(Core.$name);
        var numCores = coreModConfig.numCores;
    }
    else {
        var numCores = 1;
    }

    var coreId;
    var excActive = false;

    try {
        var hwiRawView = Program.scanRawView('ti.sysbios.family.arm.m3.Hwi');
    }
    catch (e) {
        return null;
    }

    var excActive  = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt16', isScalar: true}, hwiRawView.modState.excActive, 2, false);

    try {
        var excContexts  = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt', isScalar: true}, hwiRawView.modState.excContext, 2, false);
    }
    catch (e) {
        return null;
    }

    var obj = {};

    for (coreId = 0; coreId < numCores; coreId++) {
        if (excActive[coreId] != 0) {
            if (hwiModConfig.excHandlerFunc == null) {
                var excContext = Program.fetchStruct(Hwi.ExcContext$fetchDesc,
                                     hwiModConfig.vectorTableAddress, false);
                viewFillExceptionContext(excContext);
            }
            else {
                    try {
                        var excContext = Program.fetchStruct(Hwi.ExcContext$fetchDesc,
                                 excContexts[coreId], false);
                    }
                    catch (e) {
                        print(e);
                        return null;
                    }
            }

            var excDecode = {};
            excDecode["Decoded"] = {};
            excDecode.Decoded = viewDecodeException(excContext);

            var excCallStack = viewCallStack(excContext);

            if (biosModConfig.smpEnabled == true) {
                obj["Decoded exception, core " + String(coreId)] = excDecode;
                obj["Exception context " + String(coreId)] = excContext;
            }
            else {
                obj["Decoded exception"] = excDecode;
                obj["Exception context"] = excContext;
            }

            obj["Exception call stack"] = excCallStack;
        }
    }

    return (obj);
}

/*
 *  ======== viewCheckVector ========
 *  Depending on ROV/ROV2 and the toolchain used to build the
 * .out file, the vale for a function symbol address may or may
 *  not have its LSB set. This function attempts to normalize
 *  the ROV view behavior for ALL permutations of toolchains
 *  and ROV/ROV2.
 */
function viewCheckVector(vectorObj, expected, errorPrefix)
{
    var val = Program.lookupSymbolValue(expected);
    var vector = vectorObj.vector;
    var match = Number(vector&~1) == Number(val&~1);
    if (match) {
        vectorObj.vectorLabel = expected;
    }
    else {
        vectorObj.vectorLabel = "error";
        if (errorPrefix) {
            vectorObj.$status["vector"] =
            vectorObj.$status["vectorLabel"] =
                errorPrefix + " vector is not as configured!\n" +
                "Should be: " + expected;
        }
    }
    return (match);
}

/*
 *  ======== viewInitVectorTable ========
 */
function viewInitVectorTable(view)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Hwi = xdc.useModule('ti.sysbios.family.arm.m3.Hwi');
    var halHwi = xdc.useModule('ti.sysbios.hal.Hwi');
    var hwiModCfg = Program.getModuleConfig(Hwi.$name);
    var correctLabel;

    var numInts = hwiModCfg.NUM_INTERRUPTS;

    viewNvicFetch(this);

    var vtor = Number(this.VTOR);

    var vectorTable = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt', isScalar: true}, vtor, numInts, false);

    try {
        var rawView = Program.scanRawView('ti.sysbios.family.arm.m3.Hwi');
    }
    catch (e) {
        return null;
    }

    var dispatchTableAddr = rawView.modState.dispatchTable;

    var ScalarStructs = xdc.useModule('xdc.rov.support.ScalarStructs');

    /* Retrieve the dispatchTable array of handles */
    var hwiHandles = Program.fetchArray(ScalarStructs.S_Ptr$fetchDesc,
                                         dispatchTableAddr, numInts);
    var vectors = new Array();

    for (var i = 0; i < numInts; i++) {
        var vector = Program.newViewStruct('ti.sysbios.family.arm.m3.Hwi', 'Vector Table');
        vector.vectorNum = i;
        vector.vector = "0x" + Number(vectorTable[i]).toString(16);

        /* Tag priority info */
        if (i >= 4) {
            var pri = viewGetPriority(view, this, i);
            var mask = numPriTable[hwiModCfg.NUM_PRIORITIES].mask;
            var shift = numPriTable[hwiModCfg.NUM_PRIORITIES].shift;
            vector.priority = "0x" + Number(pri).toString(16);
            if (hwiModCfg.priGroup + 1 > shift) {
                vector.preemptPriority = pri >> (hwiModCfg.priGroup + 1);
            }
            else {
                vector.preemptPriority = pri >> shift;
            }

            vector.subPriority = (pri & subPriMasks[hwiModCfg.priGroup]) >> shift;
        }

        /* Hwi handles only exist for interrupts 15 thru NUM_INTERRUPTS */
        if (i > 14) {
            var hwiHandle = hwiHandles[i];

            /* If a Hwi object exists for this vector */
            if (Number(hwiHandle.elem) != 0) {
                vector.type = "Dispatched";
                vector.hwiHandle = '0x' + Number(hwiHandle.elem).toString(16);
                /* fetch the Hwi object */
                var hwi = Program.fetchStruct(Hwi.Instance_State$fetchDesc, hwiHandle.elem, false);

                vector.hwiFxn = Program.lookupFuncName(Number(hwi.fxn))[0];
                vector.hwiArg = hwi.arg;
                vector.hwiIrp = hwi.irp;

                if (!viewCheckVector(vector, "ti_sysbios_family_arm_m3_Hwi_dispatch__I")) {
                    vector.$status["vector"] = vector.$status["vectorLabel"] =
                        "The vector for this dispatched interrupt is not correct!\n" +
                        "Should be: \"ti_sysbios_family_arm_m3_Hwi_dispatch__I\"";
                }

                if (vector.priority < hwiModCfg.disablePriority) {
                    vector.$status["type"] = vector.$status["priority"] =
                        "Unsafe! This dispatched interrupt " +
                        "has a zero latency interrupt priority and is " +
                        "not disabled by Hwi_disable()!";
                }
            }
            else {
                if (viewCheckVector(vector, "ti_sysbios_family_arm_m3_Hwi_excHandlerAsm__I")) {
                    vector.type = "Unused";
                }
                else {
                    vector.type = "Unmanaged";
                    /*
                     * Attempt to find a matching symbol for the vector
                     * try with the LSB set to 1 first (ROV2 likes this)
                     */
                    var vectorLabel = Program.lookupFuncName(Number(vectorTable[i]));
                    if (vectorLabel.length == 0) {
                        /* clear LSB if label not found with LSB set (legacy ROV likes this) */
                        vectorLabel = Program.lookupFuncName(Number(vectorTable[i]&0xfffffffe));
                    }
                    if (vectorLabel.length == 0) {
                        vector.vectorLabel = "";
                    }
                    else {
                        vector.vectorLabel = vectorLabel[0];
                    }
                    /* check for non-dispatched interrupts created with Hwi.create */
                    for (var j in rawView.instStates) {
                        /* non-dispatched interrupts are encoded with 2's complemented intNums */
                        if (-(rawView.instStates[j].intNum) == i) {
                            if (vector.priority < hwiModCfg.disablePriority) {
                                vector.type = "Zero Latency";
                            }
                            else {
                                vector.type = "Non Dispatched";
                            }
                            vector.hwiHandle = "0x" + Number(rawView.instStates[j].$addr).toString(16);
                            vector.hwiFxn = Program.lookupFuncName(Number(rawView.instStates[j].fxn))[0];
                            vector.hwiArg = "N/A";
                            vector.hwiIrp = "N/A";
                            if (rawView.instStates[j].fxn != vector.vector) {
                                vector.$status["vector"] =
                                vector.$status["vectorLabel"] =
                                vector.$status["hwiFxn"] = "Vector does not match Hwi function!";
                            }
                        }
                    }
                }
            }

            var enabled = false;
            var active = false;
            var pending = false;

            if (i > 15) {
                var index = (i-16) >> 5;
                var mask = 1 << ((i-16) & 0x1f);
                enabled = this.ISER[index] & mask;
                active = this.IABR[index] & mask;
                pending = this.ISPR[index] & mask;
            }
            else {
                switch(i) {
                    case 15: /* SysTick */
                        pending = this.ICSR & 0x100000000;
                        enabled = this.STCSR & 0x00000002;
                        active = this.SHCSR & 0x00000800;
                        break;
                    default:
                        view.status = "unknown";
                        return;
                        break;
                }
            }

            if (enabled) {
                vector.status = "Enabled";
            }
            else {
                vector.status = "Disabled";
            }

            if (active) {
                vector.status += ", Active";
            }

            if (pending) {
                vector.status += ", Pending";
            }
        }
        vectors[vectors.length] = vector;
    }

    vectors[0].type = "MSP";
    vectors[0].vectorLabel = "";

    /* check exception handlers */

    vectors[1].type = "Reset";
    vectors[1].preemptPriority = -3;
    correctLabel = String(hwiModCfg.resetFunc).substring(1);
    viewCheckVector(vectors[1], correctLabel, "Reset");

    vectors[2].type = "NMI";
    vectors[2].preemptPriority = -2;
    correctLabel = String(hwiModCfg.nmiFunc).substring(1);
    viewCheckVector(vectors[2], correctLabel, "NMI");

    vectors[3].type = "HardFault";
    vectors[3].preemptPriority = -1;
    correctLabel = String(hwiModCfg.hardFaultFunc).substring(1);
    viewCheckVector(vectors[3], correctLabel, "Hard Fault");

    vectors[4].type = "MemFault";
    correctLabel = String(hwiModCfg.memFaultFunc).substring(1);
    viewCheckVector(vectors[4], correctLabel, "Mem Fault");

    vectors[5].type = "BusFault";
    correctLabel = String(hwiModCfg.busFaultFunc).substring(1);
    viewCheckVector(vectors[5], correctLabel, "Bus Fault");

    vectors[6].type = "UsageFault";
    correctLabel = String(hwiModCfg.usageFaultFunc).substring(1);
    viewCheckVector(vectors[6], correctLabel, "Usage Fault");

    vectors[7].type = "Reserved";
    vectors[7].vectorLabel = "";
    vectors[8].type = "Reserved";
    vectors[8].vectorLabel = "";
    vectors[9].type = "Reserved";
    vectors[9].vectorLabel = "";
    vectors[10].type = "Reserved";
    vectors[10].vectorLabel = "";

    vectors[11].type = "SVCall";
    correctLabel = String(hwiModCfg.svCallFunc).substring(1);
    viewCheckVector(vectors[11], correctLabel, "SVCall");

    vectors[12].type = "DebugMon";
    correctLabel = String(hwiModCfg.debugMonFunc).substring(1);
    viewCheckVector(vectors[12], correctLabel, "Debug Mon");

    vectors[13].type = "Reserved";
    vectors[13].vectorLabel = "";

    vectors[14].type = "PendSV";
    correctLabel = "ti_sysbios_family_arm_m3_Hwi_pendSV__I";
    viewCheckVector(vectors[14], correctLabel, "PendSV");

    view.elements = vectors;
}

/*
 *  ======== viewInitModule ========
 */
function viewInitModule(view, mod)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var CallStack = xdc.useModule('xdc.rov.CallStack');

    CallStack.fetchRegisters(["CTRL_FAULT_BASE_PRI"]);
    var ctrlFaultBasePri = CallStack.getRegister("CTRL_FAULT_BASE_PRI");

    var halHwiModCfg = Program.getModuleConfig('ti.sysbios.hal.Hwi');
    var hwiModCfg = Program.getModuleConfig('ti.sysbios.family.arm.m3.Hwi');

    viewNvicFetch(this);

    view.activeInterrupt = String(this.ICSR & 0xff);
    view.pendingInterrupt = String((this.ICSR & 0xff000) >> 12);
    if (view.activeInterrupt != "0") {
        view.processorState = "Handler";
    }
    else if (ctrlFaultBasePri & 0x01000000) {
        view.processorState = "Unpriv, Thread";
    }
    else {
        view.processorState = "Priv, Thread";
    }

    if ((view.activeInterrupt > 0) && (view.activeInterrupt < 14)) {
        view.exception = "Yes";
        Program.displayError(view, "exception", "An exception has occurred!");
    }
    else {
        view.exception = "none";
    }

    view.options[0] = "Hwi.autoNestingSupport = ";
    view.options[1] = "Hwi.swiSupport = ";
    view.options[2] = "Hwi.taskSupport = ";
    view.options[3] = "Hwi.irpSupport = ";

    view.options[0] += hwiModCfg.dispatcherAutoNestingSupport ? "true" : "false";
    view.options[1] += hwiModCfg.dispatcherSwiSupport ? "true" : "false";
    view.options[2] += hwiModCfg.dispatcherTaskSupport ? "true" : "false";
    view.options[3] += hwiModCfg.dispatcherIrpTrackingSupport ? "true" : "false";

    var stackInfo = viewGetStackInfo();

    if (stackInfo.hwiStackSize == 0) {
        view.$status["hwiStackPeak"] =
        view.$status["hwiStackSize"] =
        view.$status["hwiStackBase"] = "Error fetching Hwi stack info!";
    }
    else {
        if (halHwiModCfg.initStackFlag) {
            view.hwiStackPeak = String(stackInfo.hwiStackPeak);
            view.hwiStackSize = stackInfo.hwiStackSize;
            view.hwiStackBase = "0x"+ stackInfo.hwiStackBase.toString(16);

            if (stackInfo.hwiStackPeak == stackInfo.hwiStackSize) {
                view.$status["hwiStackPeak"] = "Overrun!  ";
                /*                                  ^^  */
                /* (extra spaces to overcome right justify) */
            }
        }
        else {
            view.hwiStackPeak = "n/a - set Hwi.initStackFlag";
            view.hwiStackSize = stackInfo.hwiStackSize;
            view.hwiStackBase = "0x"+ stackInfo.hwiStackBase.toString(16);
        }
    }
}

/*
 *  ======== _enableExceptionSet ========
 *
 *  This function is called whenever enableException changes.
 */
function _enableExceptionSet(field, val)
{
    if (val == true) {
        if (Hwi.excHandlerFunc == Hwi.excHandlerMin) {
            Hwi.excHandlerFunc = Hwi.excHandlerMax;
        }
    }
    else {
        if (Hwi.excHandlerFunc == Hwi.excHandlerMax) {
            Hwi.excHandlerFunc = Hwi.excHandlerMin;
        }
    }
}

/*
 *  ======== _excHandlerFuncSet ========
 *
 *  This function is called whenever excHandlerFunc changes.
 *
 *  This "on set" function is here just to make xgconf update the
 *  excHandlerFunc field immediately whenever enableException is
 *  checked or unchecked.
 */
function _excHandlerFuncSet(field, val)
{
}
