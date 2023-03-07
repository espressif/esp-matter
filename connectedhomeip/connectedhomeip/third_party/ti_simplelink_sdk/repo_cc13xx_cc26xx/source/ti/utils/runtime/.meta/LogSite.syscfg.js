/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== LogSite.syscfg.js ========
 */

"use strict";

const ILogger = system.getScript("/ti/utils/runtime/ILogger");

/*
 *  ======== config_instance ========
 *  Define the config params of the module instance
 */
let config_instance = [
    {
        name: "module",
        displayName: "Module",
        default: "name_space_Module",
        onChange: onChangeName_instance,
        description: "Specify a namespace qualified module name."
    },
    {
        name: "enable_INFO1",
        displayName: "Enable Level INFO1",
        default: false
    },
    {
        name: "enable_INFO2",
        displayName: "Enable Level INFO2",
        default: false
    },
    {
        name: "enable_INFO3",
        displayName: "Enable Level INFO3",
        default: false
    },
    {
        name: "enable_INFO4",
        displayName: "Enable Level INFO4",
        default: false
    },
    {
        name: "enable_INFO5",
        displayName: "Enable Level INFO5",
        default: false
    },
    {
        name: "enable_INFO6",
        displayName: "Enable Level INFO6",
        default: false
    },
    {
        name: "enable_WARN",
        displayName: "Enable Level WARN",
        default: false
    },
    {
        name: "enable_ERROR",
        displayName: "Enable Level ERROR",
        default: false
    },
    {
        name: "loggerMod",
        displayName: "Logger Modules",
        default: "/ti/loggers/utils/LoggerBuf",
        options: ILogger.providers()
    }
];

/*
 *  ======== onChangeName_instance ========
 *  Compute the instance name
 *
 *  <module>_LogSite
 */
function onChangeName_instance(inst, ui)
{
    inst.$name = [ inst.module, "LogSite" ].join("_");
}

/*
 *  ======== modules ========
 *  Express dependencies for other modules
 */
function modules(inst)
{
    let modules = new Array();

    modules.push({
        name: "getlibs",
        displayName: "GetLibs",
        moduleName: "/ti/utils/runtime/GetLibs"
    });

    return (modules);
}

/*
 *  ======== sharedModuleInstances ========
 *  Express dependencies for shared instances of other modules
 */
function sharedModuleInstances(inst)
{
    let modules = new Array();

    modules.push({
        name: "logger",
        displayName: "Logger instance",
        moduleName: inst.loggerMod
    });

    return (modules);
}

/*
 *  ======== getSyms ========
 *  Contribute the symbol map file to the generated linker command file
 */
function getSyms(mod)
{
    let GenLibs = system.getScript("/ti/utils/build/GenLibs.syscfg.js");
    let scc = GenLibs.getToolchainDir();

    var libs = [];
    var deps = [];

    /* add linker script which contains all LogSite symbol mappings */
    if ("/ti/utils/runtime/LogSite" in system.modules) {
        if (scc != "iar") {
            libs.push("ti_utils_runtime_LogSite.cmd.genlibs");
        }
    }

    /* must come before all logger implementations */
    const loggers = ILogger.providers();

    for (let i = 0; i < loggers.length; i++) {
        if (system.modules[loggers[i].name] != null) {
            /* extract the module's namespace for the dependency declaration */
            deps.push(loggers[i].name.match(/^(.+)\/\w+$/)[1]);
        }
    }

    /* create a GenLibs input argument */
    var linkOpts = {
        name: "/ti/utils/runtime/LogSite",
        deps: deps,
        libs: libs
    };

    return (linkOpts);
}

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "LogSite",
    description: "Log Site",
    config: config_instance,
    defaultInstanceName: "ti_utils_runtime_LogSite_",
    modules: modules,
    sharedModuleInstances: sharedModuleInstances,
    templates: {
        "/ti/utils/runtime/Config.c.xdt":
            "/ti/utils/runtime/LogSite.Config.c.xdt",
        "/ti/utils/runtime/Config.h.xdt":
            "/ti/utils/runtime/LogSite.Config.h.xdt",
        "/ti/utils/runtime/LogSite.cmd.xdt":
            "/ti/utils/runtime/LogSite.cmd.xdt",
        "/ti/utils/build/GenLibs.cmd.xdt":
            { modName: "/ti/utils/runtime/LogSite", getLibs: getSyms }
    }
};

/* export the module */
exports = base;
