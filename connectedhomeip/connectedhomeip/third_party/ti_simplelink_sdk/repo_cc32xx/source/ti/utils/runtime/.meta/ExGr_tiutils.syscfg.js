/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ExGr_tiutils.syscfg.js ========
 */

"use strict";

/*
 *  ======== Logger ========
 *  module: ti.loggers.utils.LoggerBuf
 *  view: Records
 *  fxn(execData, viewData)
 */
let logger_LoggerBuf_Records =
"{\n\
    var filters = execData.filters;\n\
\n\
    for (var i = 0; i < viewData.length; i++) {\n\
        var logs = new Array();\n\
        var buffer = viewData[i].elements;\n\
\n\
        for (var j = 0; j < buffer.length; j++) {\n\
            var rec = buffer[j];\n\
            for (var k = 0; k < filters.length; k++) {\n\
                var f = filters[k];\n\
                if (f.logger == \\\"ti.loggers.utils.LoggerBuf\\\") {\n\
                    if (f.name == rec.Module) {\n\
                        logs.push(f.fxn(rec));\n\
                        break;\n\
                    }\n\
                    else {\n\
                        var re = RegExp(\\\"^\\\" + f.name + \\\"$\\\");\n\
                        if (re.test(rec.Module)) {\n\
                            logs.push(f.fxn(rec));\n\
                            break;\n\
                        }\n\
                    }\n\
                }\n\
            }\n\
        }\n\
\n\
        ExecGraph_collate(execData, logs);\n\
    }\n\
}";

/*
 *  ======== Logger ========
 *  module: ti.loggers.drivers.LoggerITM
 *  view: Records
 *  fxn(execData, viewData)
 */
let logger_LoggerITM_Records =
"{\n\
    var filters = execData.filters;\n\
\n\
    for (var i = 0; i < viewData.length; i++) {\n\
        var logs = new Array();\n\
        var buffer = viewData[i].elements;\n\
\n\
        for (var j = 0; j < buffer.length; j++) {\n\
            var rec = buffer[j];\n\
            for (var k = 0; k < filters.length; k++) {\n\
                var f = filters[k];\n\
                if (f.logger == \\\"ti.loggers.drivers.LoggerITM\\\") {\n\
                    if (f.name == rec.Module) {\n\
                        logs.push(f.fxn(rec));\n\
                        break;\n\
                    }\n\
                    else {\n\
                        var re = RegExp(\\\"^\\\" + f.name + \\\"$\\\");\n\
                        if (re.test(rec.Module)) {\n\
                            logs.push(f.fxn(rec));\n\
                            break;\n\
                        }\n\
                    }\n\
                }\n\
            }\n\
        }\n\
\n\
        ExecGraph_collate(execData, logs);\n\
    }\n\
}";

/*
 *  ======== Filter ========
 *  logger: ti.loggers.utils.LoggerBuf
 *  module: ti_utils_runtime_LogMain
 *  fxn(logEvent)
 */
let filter_LoggerBuf_LogMain =
"{\n\
    var rec = {\n\
        seq: Number(logEvent.Serial),\n\
        ts: Number(logEvent.Timestamp),\n\
        label: logEvent.Message,\n\
        type: \\\"Data\\\",\n\
        y: -1\n\
    };\n\
\n\
    return (rec);\n\
}";

/*
 *  ======== Filter ========
 *  logger: ti.loggers.utils.LoggerBuf
 *  module: ti_drivers_UART
 *  fxn(logEvent)
 */
let filter_LoggerBuf_UART =
"{\n\
    var rec = {\n\
        seq: Number(logEvent.Serial),\n\
        ts: Number(logEvent.Timestamp),\n\
        label: logEvent.Message,\n\
        type: \\\"Data\\\",\n\
        y: -1\n\
    };\n\
\n\
    return (rec);\n\
}";

/*
 *  ======== Filter ========
 *  logger: ti.loggers.drivers.LoggerITM
 *  module: ti_utils_runtime_LogMain
 *  fxn(logEvent)
 */
let filter_LoggerITM_LogMain =
"{\n\
    var rec = {\n\
        seq: Number(logEvent.Serial),\n\
        ts: Number(logEvent.Timestamp),\n\
        label: logEvent.Message,\n\
        type: \\\"Data\\\",\n\
        y: -1\n\
    };\n\
\n\
    return (rec);\n\
}";

/*
 *  ======== Filter ========
 *  logger: ti.loggers.drivers.LoggerITM
 *  module: ti_drivers_UART
 *  fxn(logEvent)
 */
let filter_LoggerITM_UART =
"{\n\
    var rec = {\n\
        seq: Number(logEvent.Serial),\n\
        ts: Number(logEvent.Timestamp),\n\
        label: logEvent.Message,\n\
        type: \\\"Data\\\",\n\
        y: -1\n\
    };\n\
\n\
    return (rec);\n\
}";

/*
 *  ======== moduleInstances ========
 *  Express dependencies for private instances of other modules
 *
 *  This function is invoked on any configuration change to the given
 *  instance.
 */
function moduleInstances(inst)
{
    let modules = new Array();

    modules.push({
        name: "ExecGraphLogger_utils",
        moduleName: "/ti/utils/runtime/ExecGraphLogger",
        args: {
            label: "TI-Utils LoggerBuf",
            module: "ti.loggers.utils.LoggerBuf",
            view: "Records",
            fxn: logger_LoggerBuf_Records
        }
    });

    modules.push({
        name: "ExecGraphLogger_drivers",
        moduleName: "/ti/utils/runtime/ExecGraphLogger",
        args: {
            label: "TI-Drivers LoggerITM",
            module: "ti.loggers.drivers.LoggerITM",
            view: "Records",
            fxn: logger_LoggerITM_Records
        }
    });

    modules.push({
        name: "Filter_LoggerBuf_LogMain",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "CONFIG_LoggerBuf_LogMain",
            logger: "ti.loggers.utils.LoggerBuf",
            module: "ti_utils_runtime_LogMain",
            fxn: filter_LoggerBuf_LogMain
        }
    });

    modules.push({
        name: "Filter_LoggerBuf_UART",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "CONFIG_LoggerBuf_UART",
            logger: "ti.loggers.utils.LoggerBuf",
            module: "ti_drivers_UART",
            fxn: filter_LoggerBuf_UART
        }
    });

    modules.push({
        name: "Filter_LoggerITM_LogMain",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "CONFIG_LoggerITM_LogMain",
            logger: "ti.loggers.drivers.LoggerITM",
            module: "ti_utils_runtime_LogMain",
            fxn: filter_LoggerITM_LogMain
        }
    });

    modules.push({
        name: "Filter_LoggerITM_UART",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "CONFIG_LoggerITM_UART",
            logger: "ti.loggers.drivers.LoggerITM",
            module: "ti_drivers_UART",
            fxn: filter_LoggerITM_UART
        }
    });

    return (modules);
}

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "ExGr_tiutils",
    description: "Execution Graph setup for TI-Utils",
    defaultInstanceName: "CONFIG_ExGr_tiutils_",
    moduleStatic: {
        moduleInstances: moduleInstances,
    }
};

/* export the module */
exports = base;
