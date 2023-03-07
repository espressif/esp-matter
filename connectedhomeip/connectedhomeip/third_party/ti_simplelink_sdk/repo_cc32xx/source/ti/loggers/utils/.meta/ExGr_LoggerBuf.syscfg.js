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
 *  ======== ExGr_LoggerBuf.syscfg.js ========
 */

"use strict";

/*
 *  ======== Logger ========
 *  module: ti.loggers.utils.LoggerBuf
 *  view: Records
 *  fxn(execData, viewData)
 */
let logger_Records =
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
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    let reqs = new Array();

    reqs.push({
        name: "ExecGraphLogger",
        moduleName: "/ti/utils/runtime/ExecGraphLogger",
        args: {
            label: "TI-Utils LoggerBuf",
            module: "ti.loggers.utils.LoggerBuf",
            view: "Records",
            fxn: logger_Records
        }
    });

    return (reqs);
}

/*
 *  ======== exports ========
 */
exports = {
    displayName: "ExGr_LoggerBuf",
    description: "Execution Graph setup for ti.loggers.utils.LoggerBuf",
    moduleStatic: {
        moduleInstances: moduleInstances
    }
};
