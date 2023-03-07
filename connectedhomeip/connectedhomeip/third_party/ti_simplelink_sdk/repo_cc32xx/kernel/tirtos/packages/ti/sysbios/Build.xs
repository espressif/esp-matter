/*
 * Copyright (c) 2015-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== Build.xs ========
 */

var BIOS = null;
var Build = null;

var custom28xOpts = " -q -mo ";
var custom6xOpts = " -q -mi10 -mo -pdr -pden -pds=238 -pds=880 -pds1110 ";
var customARP32xOpts = " -q --gen_func_subsections ";
var customArmOpts = " -q -ms --opt_for_speed=2 ";

/*
 * '-Wno-buildin-requires-header' is a workaround to suppress pthread.h
 * warnings. This should be removed. Removal is tracked by TIRTOS-1906.
 */
var customArmClangM33Opts = " ";
var customArmClangM33FOpts = " ";
var customArmClangM3Opts = " ";
var customArmClangM4Opts = " ";
var customArmClangM4FOpts = " ";
var customGnuArmM3Opts = " ";
var customGnuArmM4Opts = " ";
var customGnuArmM4FOpts = " ";
var customGnuArmA9Opts = " ";
var customGnuArmA8Opts = " ";
var customGnuArmA15Opts = " ";
var customGnuArmA53Opts = " ";
var customIarArmOpts = " --silent ";

var ccOptsList = {
    "ti.targets.C28_large"                      : custom28xOpts,
    "ti.targets.C28_float"                      : custom28xOpts,
    "ti.targets.elf.C28_float"                  : custom28xOpts,
    "ti.targets.elf.C674"                       : custom6xOpts,
    "ti.targets.elf.C67P"                       : custom6xOpts,
    "ti.targets.elf.C66"                        : custom6xOpts,
    "ti.targets.elf.C66_big_endian"             : custom6xOpts,
    "ti.targets.arp32.elf.ARP32"                : customARP32xOpts,
    "ti.targets.arp32.elf.ARP32_far"            : customARP32xOpts,
    "ti.targets.arm.elf.Arm9"                   : customArmOpts,
    "ti.targets.arm.elf.A8F"                    : customArmOpts,
    "ti.targets.arm.elf.A8Fnv"                  : customArmOpts,
    "ti.targets.arm.elf.M3"                     : customArmOpts,
    "ti.targets.arm.elf.M4"                     : customArmOpts,
    "ti.targets.arm.elf.M4F"                    : customArmOpts,
    "ti.targets.arm.elf.R4F"                    : customArmOpts,
    "ti.targets.arm.elf.R4Ft"                   : customArmOpts,
    "ti.targets.arm.elf.R5F"                    : customArmOpts,
    "ti.targets.arm.elf.R5F_big_endian"         : customArmOpts,
    "ti.targets.arm.clang.M33"                  : customArmClangM33Opts,
    "ti.targets.arm.clang.M33F"                 : customArmClangM33FOpts,
    "ti.targets.arm.clang.M3"                   : customArmClangM3Opts,
    "ti.targets.arm.clang.M4"                   : customArmClangM4Opts,
    "ti.targets.arm.clang.M4F"                  : customArmClangM4FOpts,
    "gnu.targets.arm.M3"                        : customGnuArmM3Opts,
    "gnu.targets.arm.M4"                        : customGnuArmM4Opts,
    "gnu.targets.arm.M4F"                       : customGnuArmM4FOpts,
    "gnu.targets.arm.A8F"                       : customGnuArmA8Opts,
    "gnu.targets.arm.A9F"                       : customGnuArmA9Opts,
    "gnu.targets.arm.A15F"                      : customGnuArmA15Opts,
    "gnu.targets.arm.A53F"                      : customGnuArmA53Opts,
    "iar.targets.arm.M3"                        : customIarArmOpts,
    "iar.targets.arm.M4"                        : customIarArmOpts,
    "iar.targets.arm.M4F"                       : customIarArmOpts,
};

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    Build = this;

    /*
     * Set default verbose level for custom build flow
     * User can override this in their cfg file.
     */
    var SourceDir = xdc.module("xdc.cfg.SourceDir");
    SourceDir.verbose = 2;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    BIOS = xdc.module("ti.sysbios.BIOS");

    var profile;

    /*
     * Get the profile associated with the ti.sysbios package.
     * The profile can be specified on a per package basis with a line like
     * this in your .cfg script:
     *
     * xdc.loadPackage('ti.sysbios').profile = "release";
     */
    if (this.$package.profile != undefined) {
        profile = this.$package.profile;
    }
    else {
        profile = Program.build.profile;
    }

    /*
     * Handle non-supported whole_program profiles
     */
    if (profile.match(/whole_program/)) {
        BIOS.$logError("The '" + profile +
            "' build profile is no longer supported in SYS/BIOS. " +
            "Use 'release' or 'debug' profiles together with the " +
            "'BIOS.libType' configuration parameter to specify your " +
            "preferred library.  See the compatibility section of " +
            "your SYS/BIOS release notes for more information.",
            "Profile Unsupported Error", BIOS);
    }

    /* 
     * Let the ROM's module$use() handle the BIOS.libType setting
     * if building a ROM application.
     */
    if (Build.buildROMApp == true) {
        return;
    }

    /* inform getLibs() about location of library */
    switch (BIOS.libType) {
        case BIOS.LibType_Instrumented:
            if ((BIOS.$written("assertsEnabled") == true) &&
                (BIOS.assertsEnabled == false)) {
                BIOS.$logWarning(
                    "Asserts are enabled when BIOS.libType " +
                    "is set to BIOS.LibType_Instrumented",
                    BIOS, "assertsEnabled"
                );
            }
            BIOS.assertsEnabled = true;

            if ((BIOS.$written("logsEnabled") == true) &&
                (BIOS.logsEnabled == false)) {
                BIOS.$logWarning(
                    "Logs are enabled when BIOS.libType " +
                    "is set to BIOS.LibType_Instrumented",
                    BIOS, "logsEnabled"
                );
            }
            BIOS.logsEnabled = true;
            break;

        case BIOS.LibType_NonInstrumented:
            if ((BIOS.$written("assertsEnabled") == true) &&
                (BIOS.assertsEnabled == true)) {
                BIOS.$logWarning(
                    "Asserts are disabled when BIOS.libType " +
                    "is set to BIOS.LibType_NonInstrumented",
                    BIOS, "assertsEnabled"
                );
            }
            BIOS.assertsEnabled = false;
            if ((BIOS.$written("logsEnabled") == true) &&
                (BIOS.logsEnabled == true)) {
                BIOS.$logWarning(
                    "Logs are disabled when BIOS.libType " +
                    "is set to BIOS.LibType_NonInstrumented",
                    BIOS, "logsEnabled"
                );
            }
            BIOS.logsEnabled = false;
            break;

        case BIOS.LibType_Debug:
            xdc.useModule('ti.sysbios.knl.Clock'); /* Pull in Clock module so that hard references to Clock APIs are resolved */
            /* this fall through to the custom logic is on purpose */

        case BIOS.LibType_Custom:
            break;
    }

    this.$private.libraryName = "/sysbios.a" + Program.build.target.suffix;
    var SourceDir = xdc.useModule("xdc.cfg.SourceDir");

    /*
     * If BIOS.libDir has been explicitly specified by the user, use it.
     */
    if (BIOS.libDir !== null) {
        SourceDir.outputDir = BIOS.libDir;
    }
    else {
        var curPath = java.io.File(".").getCanonicalPath();
        /* If package.bld world AND building an application OR pre-built lib */
        if (java.io.File(curPath).getName() != "configPkg") {
            var appName = Program.name.substring(0, Program.name.lastIndexOf('.'));
            appName = appName + "_p" + Program.build.target.suffix + ".src";
            SourceDir.outputDir = "package/cfg/" + appName;
            SourceDir.toBuildDir = ".";
        }
        /* Here ONLY if building an application in CCS world */
        else {
            /* request output source directory for generated files */
            SourceDir.toBuildDir = "..";
        }
    }

    var src = SourceDir.create("sysbios");
    src.libraryName = "sysbios" + this.$private.libraryName;
    /*
     * save this directory in our private state (to be read during
     * generation, see Gen.xdt)
     */
    this.$private.outputDir = src.getGenSourceDir();
}

/*
 * Used for trimming the set of C files in the custom BIOS.lib
 * to the minimum required for the application.
 * Only odd case modules are in this list:
 * -More than just Mod.c files
 * -No Mod.c file
 */
var cFiles = {
    "xdc.runtime.Defaults" :
        { cSources: [] },
    "xdc.runtime.Main" :
        { cSources: [] },
    "xdc.runtime.Types" :
        { cSources: [] },
    "xdc.runtime.Core" :
        { cSources: ["Core-mem.c", "Core-smem.c", "Core-label.c", "Core-params.c"] },
};

var biosPackages = [
    "ti.sysbios",
    "ti.sysbios.family.arm",
    "ti.sysbios.family.arm.a8",
    "ti.sysbios.family.arm.a8.intcps",
    "ti.sysbios.family.arm.a8.omap3430",
    "ti.sysbios.family.arm.a8.sim",
    "ti.sysbios.family.arm.a8.ti81xx",
    "ti.sysbios.family.arm.a9",
    "ti.sysbios.family.arm.a9.am437x",
    "ti.sysbios.family.arm.a15",
    "ti.sysbios.family.arm.a15.smp",
    "ti.sysbios.family.arm.a15.tci66xx",
    "ti.sysbios.family.arm.gic",
    "ti.sysbios.family.arm.gicv3",
    "ti.sysbios.family.arm.systimer",
    "ti.sysbios.family.arm.arm9",
    "ti.sysbios.family.arm.cc26xx",
    "ti.sysbios.family.arm.cc32xx",
    "ti.sysbios.family.arm.da830",
    "ti.sysbios.family.arm.ducati",
    "ti.sysbios.family.arm.ducati.dm8148",
    "ti.sysbios.family.arm.ducati.omap4430",
    "ti.sysbios.family.arm.ducati.smp",
    "ti.sysbios.family.arm.exc",
    "ti.sysbios.family.arm.f28m35x",
    "ti.sysbios.family.arm.lm4",
    "ti.sysbios.family.arm.lm4.rtc",
    "ti.sysbios.family.arm.lm3",
    "ti.sysbios.family.arm.m3",
    "ti.sysbios.family.arm.msp432",
    "ti.sysbios.family.arm.msp432.init",
    "ti.sysbios.family.arm.msp432e4.init",
    "ti.sysbios.family.arm.pl192",
    "ti.sysbios.family.arm.v7r",
    "ti.sysbios.family.arm.v7r.keystone3",
    "ti.sysbios.family.arm.v7r.tms570",
    "ti.sysbios.family.arm.v7r.vim",
    "ti.sysbios.family.arm.tms570",
    "ti.sysbios.family.arm.v7a",
    "ti.sysbios.family.arm.v7a.smp",
    "ti.sysbios.family.arm.v8a",
    "ti.sysbios.family.arm.vxm",
    "ti.sysbios.family.c28",
    "ti.sysbios.family.c28.f28m35x",
    "ti.sysbios.family.c28.f2837x",
    "ti.sysbios.family.c62",
    "ti.sysbios.family.c64",
    "ti.sysbios.family.c64p",
    "ti.sysbios.family.c64p.omap3430",
    "ti.sysbios.family.c64p.primus",
    "ti.sysbios.family.c64p.tci6488",
    "ti.sysbios.family.c64p.ti81xx",
    "ti.sysbios.family.c66",
    "ti.sysbios.family.c66.tci66xx",
    "ti.sysbios.family.c66.vayu",
    "ti.sysbios.family.c67p",
    "ti.sysbios.family.c674",
    "ti.sysbios.family.c7x",
    "ti.sysbios.family.arp32",
    "ti.sysbios.family.shared.keystone3",
    "ti.sysbios.family.shared.vayu",
    "ti.sysbios.gates",
    "ti.sysbios.hal",
    "ti.sysbios.hal.ammu",
    "ti.sysbios.hal.unicache",
    "ti.sysbios.heaps",
    "ti.sysbios.io",
    "ti.sysbios.knl",
    "ti.sysbios.misc",
    "ti.sysbios.rts",
    "ti.sysbios.rts.gnu",
    "ti.sysbios.rts.iar",
    "ti.sysbios.rts.ti",
    "ti.sysbios.rom.c28",
    "ti.sysbios.smp",
    "ti.sysbios.syncs",
    "ti.sysbios.timers.dmtimer",
    "ti.sysbios.timers.gptimer",
    "ti.sysbios.timers.rti",
    "ti.sysbios.timers.timer64",
    "ti.sysbios.utils",
    "ti.sysbios.xdcruntime",
];

var xdcPackages = [
    "xdc.runtime",
    "xdc.runtime.knl",
];

function getDefaultCustomCCOpts()
{
    var BIOS = xdc.module('ti.sysbios.BIOS');
    var Build = xdc.module('ti.sysbios.Build');

    /* start with target.cc.opts */
    var customCCOpts = Program.build.target.cc.opts;

    /* add target unique custom ccOpts */
    if (!(ccOptsList[Program.build.target.$name] === undefined)) {
        customCCOpts += ccOptsList[Program.build.target.$name];
    }

    /* Gnu targets need to pick up ccOpts.prefix and suffix */
    if (Program.build.target.$name.match(/gnu/)) {
        var gnuDevice = Program.cpu.deviceName.toUpperCase();
        /* optimize for size with cc13xx and cc26xx devices */
        if (gnuDevice.match(/CC26/) || gnuDevice.match(/CC13/)) {
            customCCOpts += " -Os ";
        }
        else {
            customCCOpts += " -O3 ";
        }
        customCCOpts = Program.build.target.ccOpts.prefix + " " + customCCOpts;
        customCCOpts += Program.build.target.ccOpts.suffix + " ";

        /* Workaround for SDOCM00102122 */
        customCCOpts = customCCOpts.replace("-Wall","");
    }
    else if (Program.build.target.$name.match(/iar/)) {
        var iarDevice = Program.cpu.deviceName.toUpperCase();
        /* optimize for size with cc13xx and cc26xx devices */
        if (iarDevice.match(/CC26/) || iarDevice.match(/CC13/)) {
            customCCOpts += " --mfc -Ohz ";
        }
        else {
            customCCOpts += " --mfc -Ohs ";
        }
    }
    else if (Program.build.target.$name.match(/clang/)) {
        customCCOpts += " -Oz -g ";
        /* add any target unique CC options provided in config.bld */
        customCCOpts = Program.build.target.ccOpts.prefix + " " + customCCOpts;
        customCCOpts += Program.build.target.ccOpts.suffix + " ";
    }
    else {
        /* ti targets do program level compile */
        customCCOpts += " --program_level_compile -o3 -g ";
        var tiDevice = Program.cpu.deviceName.toUpperCase();
        /* optimize for size with cc13xx and cc26xx devices */
        if (tiDevice.match(/CC26/) || tiDevice.match(/CC13/)) {
            customCCOpts = customCCOpts.replace("opt_for_speed=2","opt_for_speed=0");
        }
    }

    /* undo optimizations if this is a Debug build */
    if (BIOS.libType == BIOS.LibType_Debug) {
        if (Program.build.target.$name.match(/gnu/)) {
            customCCOpts = customCCOpts.replace("-O3","");
            /* add in stack frames for stack back trace */
            customCCOpts += " -mapcs ";
        }
        else if (Program.build.target.$name.match(/iar/)) {
            customCCOpts = customCCOpts.replace("-Ohs","--debug");
            customCCOpts = customCCOpts.replace("-Ohz","--debug");
        }
        else if (Program.build.target.$name.match(/clang/)) {
            customCCOpts = customCCOpts.replace(" -Oz","");
        }
        else {
            customCCOpts = customCCOpts.replace(" -o3","");
            if (Program.build.target.$name.match(/arm/)) {
                customCCOpts = customCCOpts.replace(" --opt_for_speed=2","");
            }
        }
    }

    return (customCCOpts);
}

/*
 *  ======== getDefs ========
 */
function getDefs()
{
    var BIOS = xdc.module("ti.sysbios.BIOS");
    var Hwi = xdc.module("ti.sysbios.hal.Hwi");
    var Settings = xdc.module("ti.sysbios.family.Settings");
    var HwiDelegate = Settings.getDefaultHwiDelegate();
    var HwiDelegateName = HwiDelegate.replace(/\./g, "_");

    var Swi = xdc.module("ti.sysbios.knl.Swi");
    var Task = xdc.module("ti.sysbios.knl.Task");
    var Clock = xdc.module("ti.sysbios.knl.Clock");
    var Semaphore = xdc.module("ti.sysbios.knl.Semaphore");
    var Load = xdc.module("ti.sysbios.utils.Load");
    var Defaults = xdc.module('xdc.runtime.Defaults');
    var Diags = xdc.module("xdc.runtime.Diags");

    var defs =   " -Dti_sysbios_BIOS_swiEnabled__D=" + (BIOS.swiEnabled ? "TRUE" : "FALSE")
               + " -Dti_sysbios_BIOS_taskEnabled__D=" + (BIOS.taskEnabled ? "TRUE" : "FALSE")
               + " -Dti_sysbios_BIOS_clockEnabled__D=" + (BIOS.clockEnabled ? "TRUE" : "FALSE")
               + " -Dti_sysbios_BIOS_runtimeCreatesEnabled__D=" + (BIOS.runtimeCreatesEnabled ? "TRUE" : "FALSE")
               + " -Dti_sysbios_knl_Task_moduleStateCheckFlag__D=" + (Task.moduleStateCheckFlag ? "TRUE" : "FALSE")
               + " -Dti_sysbios_knl_Task_objectCheckFlag__D=" + (Task.objectCheckFlag ? "TRUE" : "FALSE");

    if (xdc.module(HwiDelegate).hooks.length == 0) {
        if (!(BIOS.codeCoverageEnabled)) {
            defs += " -Dti_sysbios_hal_Hwi_DISABLE_ALL_HOOKS";
        }
    }

    if (Swi.hooks.length == 0) {
        if (!(BIOS.codeCoverageEnabled)) {
            defs += " -Dti_sysbios_knl_Swi_DISABLE_ALL_HOOKS";
        }
    }

    defs += " -Dti_sysbios_BIOS_smpEnabled__D="
            + (BIOS.smpEnabled ? "TRUE" : "FALSE");

    if (BIOS.smpEnabled == false) {
        defs += " -Dti_sysbios_Build_useHwiMacros";
    }

    if ((BIOS.buildingAppLib == true) && (Build.buildROM == false)) {
        defs += " -Dti_sysbios_Build_useIndirectReferences=FALSE";

        defs += " -Dti_sysbios_knl_Swi_numPriorities__D=" + Swi.numPriorities;
        defs += " -Dti_sysbios_knl_Task_deleteTerminatedTasks__D=" + (Task.deleteTerminatedTasks ? "TRUE" : "FALSE");
        defs += " -Dti_sysbios_knl_Task_numPriorities__D=" + Task.numPriorities;
        defs += " -Dti_sysbios_knl_Task_checkStackFlag__D=" + (Task.checkStackFlag ? "TRUE" : "FALSE");
        defs += " -Dti_sysbios_knl_Task_initStackFlag__D=" + (Task.initStackFlag ? "TRUE" : "FALSE");

        if (BIOS.useSK) {
            defs += " -Dti_sysbios_BIOS_useSK__D=1";
        }

        if (Task.hooks.length == 0) {
            if (!(BIOS.codeCoverageEnabled)) {
                defs += " -Dti_sysbios_knl_Task_DISABLE_ALL_HOOKS";
            }
        }

        /*
         * Add Clock module defs
         */
        switch (Clock.tickSource) {
            case Clock.TickSource_TIMER:
                defs += " -Dti_sysbios_knl_Clock_TICK_SOURCE=ti_sysbios_knl_Clock_TickSource_TIMER";
                break;
            case Clock.TickSource_USER:
                defs += " -Dti_sysbios_knl_Clock_TICK_SOURCE=ti_sysbios_knl_Clock_TickSource_USER";
                break;
            case Clock.TickSource_NULL:
                defs += " -Dti_sysbios_knl_Clock_TICK_SOURCE=ti_sysbios_knl_Clock_TickSource_NULL";
                break;
        }

        if (Clock.tickMode == Clock.TickMode_DYNAMIC) {
            defs += " -Dti_sysbios_knl_Clock_TICK_MODE=ti_sysbios_knl_Clock_TickMode_DYNAMIC";
        }
        else {
            defs += " -Dti_sysbios_knl_Clock_TICK_MODE=ti_sysbios_knl_Clock_TickMode_PERIODIC";
        }

        /*
         * map hal Core module APIs to their delegates
         */

        var coreDelegate;

        if (xdc.module('ti.sysbios.hal.Core').CoreProxy != null) {
            coreDelegate = xdc.module('ti.sysbios.hal.Core').CoreProxy.delegate$;
        }
        else {
            var coreDelegateString = Settings.getDefaultCoreDelegate();
            if (coreDelegateString == null) {
                coreDelegate = xdc.module('ti.sysbios.hal.CoreNull');
            }
            else {
                coreDelegate = xdc.module(coreDelegateString);
            }
        }

        var coreDelegateName = coreDelegate.$name;
        coreDelegateName = coreDelegateName.replace(/\./g, "_");

        defs += " -Dti_sysbios_hal_Core_delegate_getId="
                + coreDelegateName
                + "_getId__E";
        defs += " -Dti_sysbios_hal_Core_delegate_interruptCore="
                + coreDelegateName
                + "_interruptCore__E";
        defs += " -Dti_sysbios_hal_Core_delegate_lock="
                + coreDelegateName
             + "_lock__E";
        defs += " -Dti_sysbios_hal_Core_delegate_unlock="
                + coreDelegateName
             + "_unlock__E";

        defs += " -Dti_sysbios_hal_Core_numCores__D=" + coreDelegate.numCores;

        defs += " -D" + coreDelegateName + "_numCores__D=" + coreDelegate.numCores;

        /*
         * add Load module #defines
         */
        defs += " -Dti_sysbios_utils_Load_taskEnabled__D=" + (Load.taskEnabled ? "TRUE" : "FALSE");
        defs += " -Dti_sysbios_utils_Load_swiEnabled__D=" + (Load.swiEnabled ? "TRUE" : "FALSE");
        defs += " -Dti_sysbios_utils_Load_hwiEnabled__D=" + (Load.hwiEnabled ? "TRUE" : "FALSE");
    }

    if (Hwi.dispatcherSwiSupport) {
        defs += " -D"+HwiDelegateName+"_dispatcherSwiSupport__D=TRUE";
    }
    else {
        defs += " -D"+HwiDelegateName+"_dispatcherSwiSupport__D=FALSE";
    }

    if (Hwi.dispatcherTaskSupport) {
        defs += " -D"+HwiDelegateName+"_dispatcherTaskSupport__D=TRUE";
    }
    else {
        defs += " -D"+HwiDelegateName+"_dispatcherTaskSupport__D=FALSE";
    }

    if (Hwi.dispatcherAutoNestingSupport) {
        defs += " -D"+HwiDelegateName+"_dispatcherAutoNestingSupport__D=TRUE";
    }
    else {
        defs += " -D"+HwiDelegateName+"_dispatcherAutoNestingSupport__D=FALSE";
    }

    if (Hwi.dispatcherIrpTrackingSupport) {
        defs += " -D"+HwiDelegateName+"_dispatcherIrpTrackingSupport__D=TRUE";
    }
    else {
        defs += " -D"+HwiDelegateName+"_dispatcherIrpTrackingSupport__D=FALSE";
    }

    if (Semaphore.supportsEvents) {
        defs += " -Dti_sysbios_knl_Semaphore_supportsEvents__D=TRUE";
    }
    else {
        defs += " -Dti_sysbios_knl_Semaphore_supportsEvents__D=FALSE";
    }

    if (Semaphore.supportsPriority) {
        defs += " -Dti_sysbios_knl_Semaphore_supportsPriority__D=TRUE";
    }
    else {
        defs += " -Dti_sysbios_knl_Semaphore_supportsPriority__D=FALSE";
    }

    if ((BIOS.assertsEnabled == false) ||
        ((Defaults.common$.diags_ASSERT == Diags.ALWAYS_OFF)
            && (Defaults.common$.diags_INTERNAL == Diags.ALWAYS_OFF))) {
        defs += " -Dxdc_runtime_Assert_DISABLE_ALL";
    }

    if (BIOS.logsEnabled == false) {
        defs += " -Dxdc_runtime_Log_DISABLE_ALL";
    }

    defs += Build.getCommandLineDefs();

    return (defs);
}

/*
 *  ======== getCFiles ========
 */
function getCFiles(target)
{
    var biosSources ="";
    var xdcSources = "";
    var cfgBase = "";
    var File = xdc.module("xdc.services.io.File");

    /*
     * determine if mod_config.c files are pre-pended with app name.
     * xdc-A tools do not prepend the app name
     */
    var xdcPkg = xdc.loadPackage("xdc");
    xdcVer = Packages.xdc.services.global.Vers.getXdcString(xdcPkg.packageBase + "/package/package.defs.h");
    if (xdcVer.match("xdc-A")) {
        var longConfigNames = false;
    }
    else {
        var longConfigNames = true;
    }

    
    cfgBase = String(java.io.File(Program.cfgBase + ".c").getCanonicalPath());
    cfgBase = File.getDOSPath(cfgBase);
    cfgBase = cfgBase.replace(/\\/g, "/");
    cfgBase = cfgBase.substring(0, cfgBase.lastIndexOf('/') + 1);
    var appName = Program.name.substring(Program.name.lastIndexOf('/')+1, Program.name.lastIndexOf('.'));
    appName = appName + "_p" + Program.build.target.suffix + "_";

    /*
     * logic to trim the C files down to just what the application needs
     */
    biosSources += "BIOS.c ";
    for each (var mod in Program.targetModules()) {
        var mn = mod.$name;
        var pn = mn.substring(0, mn.lastIndexOf("."));
        var packageMatch = false;

        /* sanity check package path */
        for (var i = 0; i < biosPackages.length; i++) {
            if (pn == biosPackages[i]) {
                packageMatch = true;
                break;
            }
        }

        if (packageMatch && !mn.match(/Proxy/) && !mn.match(/BIOS/)) {
            if (mod.$private.getCFiles !== undefined) {
                var prefix = mn.substr(mn.indexOf("sysbios") + 8);
                var modstr = mn.substr(mn.lastIndexOf(".")+1);
                prefix = prefix.substring(0, prefix.lastIndexOf('.')+1);
                prefix = prefix.replace(/\./g, "/");
                var modCFiles = mod.$private.getCFiles(target);
                for (var j in modCFiles) {
                    biosSources += prefix + modCFiles[j] + " ";
                }
            }
            else if (cFiles[mn] === undefined) {
                var prefix = mn.substr(mn.indexOf("sysbios") + 8);
                var modstr = mn.substr(mn.lastIndexOf(".")+1);
                prefix = prefix.substring(0, prefix.lastIndexOf('.')+1);
                prefix = prefix.replace(/\./g, "/");
                biosSources += prefix + modstr + ".c ";
            }
            else {
                for (i in cFiles[mn].cSources) {
                    var prefix = mn.substr(mn.indexOf("sysbios") + 8);
                    prefix = prefix.substring(0, prefix.lastIndexOf('.')+1);
                    prefix = prefix.replace(/\./g, "/");
                    biosSources += prefix + cFiles[mn].cSources[i] + " ";
                }
            }
        }

        var Types = xdc.module('xdc.runtime.Types');
        if (packageMatch && mod.common$.outPolicy == Types.SEPARATE_FILE) {
            if (longConfigNames == true) {
                biosSources += cfgBase + appName + mn.replace(/\./g,"_") + "_config.c ";
            }
            else {
                biosSources += cfgBase + mn + "_config.c ";
            }
        }
    }
    
    if (BIOS.includeXdcRuntime == true) {
        biosSources += "xdc/runtime/xdc_noinit.c ";
        for each (var mod in Program.targetModules()) {
            var mn = mod.$name;
            var pn = mn.substring(0, mn.lastIndexOf("."));
            var packageMatch = false;

            /* sanity check package path */
            for (var i = 0; i < xdcPackages.length; i++) {
                if (pn == xdcPackages[i]) {
                    packageMatch = true;
                    break;
                }
            }

            if (packageMatch && !mn.match(/Proxy/)) {
                if (cFiles[mn] === undefined) {
                    var prefix = mn.substr(mn.indexOf("xdc"));
                    var modstr = mn.substr(mn.lastIndexOf(".")+1);
                    prefix = prefix.substring(0, prefix.lastIndexOf('.')+1);
                    prefix = prefix.replace(/\./g, "/");
                    biosSources += prefix + modstr + ".c ";
                }
                else {
                    for (i in cFiles[mn].cSources) {
                        var prefix = mn.substr(mn.indexOf("xdc"));
                        prefix = prefix.substring(0, prefix.lastIndexOf('.')+1);
                        prefix = prefix.replace(/\./g, "/");
                        biosSources += prefix + cFiles[mn].cSources[i] + " ";
                    }
                }
            }

            /*
             * pull in file contributions from non ti.sysbios
             * and non xdc.runtime packages
             * if mod.$private.getCFiles() exists
             */
            if ((packageMatch == false) && (mod.$private.getCFiles !== undefined)) {
                /* pull in file contributions from non ti.sysbios and non xdc.runtime packages */
                if (!pn.match(/sysbios/)) {
                    var modCFiles = mod.$private.getCFiles(target);
                    for (var j in modCFiles) {
                        biosSources += modCFiles[j] + " ";
                    }
                }
            }

            var Types = xdc.module('xdc.runtime.Types');
            if (packageMatch && mod.common$.outPolicy == Types.SEPARATE_FILE) {
                if (longConfigNames == true) {
                    biosSources += cfgBase + appName + mn.replace(/\./g,"_") + "_config.c ";
                }
                else {
                    biosSources += cfgBase + mn + "_config.c ";
                }
            }
        }

        /* suppress inclusion of redundant xdc.runtime library */
        var rtsLib = xdc.loadPackage(Program.build.target.$orig.rts);
        if ('Settings' in rtsLib) {
            rtsLib.Settings.bootOnly = true;
        }
        else {
            this.$logError(
                    "Must use xdctools 3.26.00.19 or newer",
                    this);
        }
    }

    /* add annex component source files */
    for (var a = 0; a < Build.annex.length; a++) {
        var annex = Build.annex[a];

        for (var f = 0; f < annex.files.length; f++) {
            biosSources += annex.files[f];
            if ((f + 1) != annex.files.length) {
                biosSources += " ";
            }
        }

        if ((a + 1) != Build.annex.length) {
            biosSources += " ";
        }
    }

    /* remove trailing white-space */
    biosSources = biosSources.replace(/\s+$/, "");

    return (biosSources);
}

/*
 *  ======== getAsmFiles ========
 */
function getAsmFiles(target)
{
    var asmSources = "";
    
    /*
     * logic to trim the C files down to just what the application needs
     */
    for each (var mod in Program.targetModules()) {
        var mn = mod.$name;
        var pn = mn.substring(0, mn.lastIndexOf("."));
        var packageMatch = false;

        /* sanity check package path */
        for (var i = 0; i < biosPackages.length; i++) {
            if (pn == biosPackages[i]) {
                packageMatch = true;
                break;
            }
        }

        if (packageMatch && !mn.match(/Proxy/) && !mn.match(/BIOS/)) {
            if (mod.$private.getAsmFiles !== undefined) {
                var prefix = mn.substr(mn.indexOf("sysbios") + 8);
                var modstr = mn.substr(mn.lastIndexOf(".")+1);
                prefix = prefix.substring(0, prefix.lastIndexOf('.')+1);
                prefix = prefix.replace(/\./g, "/");
                var modAsmFiles = mod.$private.getAsmFiles(target);
                for (var j in modAsmFiles) {
                    asmSources += prefix + modAsmFiles[j] + " ";
                }
            }
        }
    }

    if (asmSources.length != 0) {
        /* remove trailing " " */
        asmSources = asmSources.substring(0, asmSources.length-1);
    }

    return (asmSources.split(' '));
}

/*
 *  ======== getCommandLineDefs ========
 *  get the -D's and --define's from the compiler command line
 */
function getCommandLineDefs()
{
    var defs = " ";
    var type = 0; /* 1: quotes */
    var prefix = Program.build.target.ccOpts.prefix;

    var tokens = prefix.split(" ");

    /*
     * absorb all the "-Dxyz"'s and "--define"'s in the compiler command line
     */
    for (i = 0; i < tokens.length; i++) {
        if (tokens[i].match(/^-D/) || tokens[i].match(/--define/)) {
            defs += tokens[i];
            if (tokens[i] == "-D") {
                defs += tokens[++i];  /* intentionally removes spaces for IAR Assembler */
            }
            defs += " ";
        }
    }

    return (defs);
}

/*
 *  ======== getIncludePaths ========
 *  get the include path from the compiler command line
 */
function getIncludePaths()
{
    var incs = " ";
    var type = 0; /* 1: quotes */
    var prefix = Program.build.target.ccOpts.prefix;

    /*
     * First look for TI compiler's '--include_path=' option.
     * This will be a quoted string which may include embedded
     * spaces.
     */
    var tokens = prefix.split(" ");

    for (i=0; i < tokens.length; i++) {
        if (tokens[i].match(/--include_path/)) {
            incs += tokens[i] + " ";
            if (tokens[i].lastIndexOf('"') == -1) {
                continue;
            }
            /* pull in tokens until last character of token is '"' */
            while (tokens[i].lastIndexOf('"') != tokens[i].length-1) {
                incs += tokens[++i] + " ";
            }
        }
    }


    /*
     * Now look for TI, IAR and gcc's -I options.  There may be 0 or more
     * spaces after the -I.
     */
    for (var i = 0; i < prefix.length; i++) {
        /* Will process only the options */
        if (prefix[i] == '-') {
            i++;
            if (!(i < prefix.length)) {
                break;
            }

            /* Is it an include option? */
            if (prefix[i] == 'I') {
                var opt = prefix[i];

                do {
                   i++;
                   if (!(i < prefix.length)) {
                       /* break while */
                       break;
                   }
                    /* Skip whitespace */
                } while (prefix[i] == ' ');

                if (!(i < prefix.length)) {
                    break;
                }

                /* Quoted strings have to handled differently */
                if (prefix[i] == '"') {
                    type = 1;
                }

                /* Lets not forget the processed characters */
                incs += " -" + opt + prefix[i];
                i++;
                for (;i < prefix.length; i++) {
                     incs += prefix[i];
                     if (((type == 1) && (prefix[i] == '"')) ||
                         ((type == 0) && (prefix[i] == ' '))) {
                         type = 0;
                         incs += " ";
                         break;
                     }
                }
            }
        }
    }

    if (Build.buildROMApp == false) {
        return (incs);
    }

    /* add includes to non BIOS/XDC modules */
    for each (var mod in Program.targetModules()) {
        var mn = mod.$name;
        var pn = mn.substring(0, mn.lastIndexOf("."));
        var packageMatch = false;

        /* sanity check package path */
        for (var i = 0; i < biosPackages.length; i++) {
            if (pn == biosPackages[i]) {
                packageMatch = true;
                break;
            }
        }

        if (packageMatch == true) {
            continue;
        }

        /* sanity check package path */
        for (var i = 0; i < xdcPackages.length; i++) {
            if (pn == xdcPackages[i]) {
                packageMatch = true;
                break;
            }
        }

        var File = xdc.module("xdc.services.io.File");
        if (packageMatch == false) {
            incs += " -I" + File.getDOSPath(mod.$package.packageRepository) + " ";
        }
    }

    return (incs);
}

/*
 *  ======== getCcArgs ========
 *  called by BIOS.getCCOpts() to insert module-provided cc opts into
 *  custom library build compile line.
 */
function getCcArgs()
{
    var Build = xdc.module("ti.sysbios.Build");

    var ccArgs = "";

    for (var i = 0; i < Build.ccArgs.length; i++) {
        ccArgs += " " + Build.ccArgs[i];
    }

    return (ccArgs);
}

/*
 *  ======== getLibs ========
 */
function getLibs(pkg)
{
    return null;
}


/*
 *  ======== getProfiles ========
 *  Determines which profiles to build for.
 *
 *  Any argument in XDCARGS which does not contain platform= is treated
 *  as a profile. This way multiple build profiles can be specified by
 *  separating them with a space.
 */
function getProfiles(xdcArgs)
{
    /*
     * cmdlProf[1] gets matched to "whole_program,debug" if
     * ["abc", "profile=whole_program,debug"] is passed in as xdcArgs
     */
    var cmdlProf = (" " + xdcArgs.join(" ") + " ").match(/ profile=([^ ]+) /);

    if (cmdlProf == null) {
        /* No profile=XYZ found */
        return [];
    }

    /* Split "whole_program,debug" into ["whole_program", "debug"] */
    var profiles = cmdlProf[1].split(',');

    return profiles;
}

/*
 *  ======== buildLibs ========
 *  This function generates the makefile goals for the libraries
 *  produced by a ti.sysbios package.
 */
function buildLibs(objList, relList, filter, xdcArgs, incs)
{
    for (var i = 0; i < xdc.module('xdc.bld.BuildEnvironment').targets.length; i++) {
        var targ = xdc.module('xdc.bld.BuildEnvironment').targets[i];

        /* skip target if not supported */
        if (!supportsTarget(targ, filter)) {
            continue;
        }

        var profile = "debug";
        var libPath = "lib/debug/";

        /*
         * These defines are referenced in a few assembly files. They must be
         * defined when building the sanity libraries or the assembly files will
         * not build. These are defined via the .cfg file for real application
         * builds.
         */
        var asmopts = "";
        var ccopts = "";

        ccopts += " -Dti_sysbios_Build_useIndirectReferences=FALSE"

        asmopts += " -Dti_sysbios_Build_useIndirectReferences=FALSE"
        asmopts += " -Dti_sysbios_BIOS_smpEnabled__D=FALSE";
        asmopts += " -Dti_sysbios_BIOS_mpeEnabled__D=FALSE";
        asmopts += " -Dti_sysbios_hal_Core_numCores__D=1";
        asmopts += " -Dti_sysbios_family_arm_v7r_vim_Hwi_lockstepDevice__D=FALSE";
        asmopts += " -Dti_sysbios_family_arm_a8_intcps_Hwi_enableAsidTagging__D=FALSE";
        asmopts += " -Dti_sysbios_family_arm_m3_TaskSupport_usesMonitors__D=0";

        var lib = Pkg.addLibrary(libPath + Pkg.name,
                            targ, {
                            profile: profile,
                            copts: ccopts,
                            aopts: asmopts,
                            releases: relList,
                            incs: incs,
                            });
        lib.addObjects(objList);

        /* suppress debug libs from exports */
        Pkg.attrs.relScript = "ti/sysbios/libFilter.xs";
    }
}


/*
 *  ======== supportsTarget ========
 *  Returns true if target is in the filter object. If filter
 *  is null or empty, that's taken to mean all targets are supported.
 */
function supportsTarget(target, filter)
{
    var list, field;

    if (filter == null) {
        return true;
    }

    /*
     * For backwards compatibility, we support filter as an array of
     * target names.  The preferred approach is to specify filter as
     * an object with 'field' and 'list' elements.
     *
     * Old form:
     *     var trgFilter = [ "Arm9", "Arm9t", "Arm9t_big_endian" ]
     *
     * New (preferred) form:
     *
     *     var trgFilter = {
     *         field: "isa",
     *         list: [ "v5T", "v7R" ]
     *     };
     *
     */
    if (filter instanceof Array) {
        list = filter;
        field = "name";
    }
    else {
        list = filter.list;
        field = filter.field;
    }

    if (list == null || field == null) {
        throw("invalid filter parameter, must specify list and field!");
    }

    if (field == "noIsa") {
        if (String(','+list.toString()+',').match(','+target["isa"]+',')) {
            return (false);
        }
        return (true);
    }

    /*
     * add ',' at front and and tail of list and field strings to allow
     * use of simple match API.  For example, the string is updated to:
     * ',v5T,v7R,' to allow match of ',v5t,'.
     */
    if (String(','+list.toString()+',').match(','+target[field]+',')) {
        return (true);
    }

    return (false);
}
