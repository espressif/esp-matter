/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ExGr_SYSBIOS.syscfg.js ========
 */

"use strict";

/*
 *  ======== Adapter ========
 *  module: ti.sysbios.knl.Task
 *  view: Basic
 *  fxn: (execData, viewData)
 */
let adapter_Task_Basic =
"{\n\
    var label = \\\"\\\";\n\
\n\
    for (var i = 0; i < viewData.length; i++) {\n\
        var task = viewData[i];\n\
\n\
        if (task.label.match(/IdleTask/i)) {\n\
            label = \\\"Idl: \\\" + task.address;\n\
        }\n\
        else if (task.label != \\\"\\\") {\n\
            label = \\\"Tsk: \\\"+ task.label;\n\
        }\n\
        else {\n\
            label = \\\"Tsk: \\\" + task.fxn[0];\n\
        }\n\
\n\
        execData.threads.tasks.push({\n\
            label: label,\n\
            hndl: task.address,\n\
            type: 1,\n\
            pri: Number(task.priority),\n\
            stack: [ task.address ]\n\
        });\n\
    }\n\
}";

/*
 *  ======== Adapter ========
 *  module: ti.sysbios.knl.Swi
 *  view: Basic
 *  fxn: (execData, viewData)
 */
let adapter_Swi_Basic =
"{\n\
    var label = \\\"\\\";\n\
\n\
    for (var i = 0; i < viewData.length; i++) {\n\
        var swi = viewData[i];\n\
\n\
        label = \\\"Swi: \\\" + swi.fxn;\n\
\n\
        execData.threads.tasks.push({\n\
            label: label,\n\
            hndl: swi.address,\n\
            type: 2\n\
        });\n\
    }\n\
}";

/*
 *  ======== Adapter ========
 *  module: ti.sysbios.family.*.Hwi
 *  view: Basic
 *  fxn: (execData, viewData)
 */
let adapter_Hwi_Basic =
"{\n\
    var label = \\\"\\\";\n\
\n\
    for (var i = 0; i < viewData.length; i++) {\n\
        var hwi = viewData[i];\n\
\n\
        label = \\\"Hwi: \\\" + hwi.fxn;\n\
\n\
        execData.threads.tasks.push({\n\
            label: label,\n\
            hndl: hwi.address,\n\
            type: 3\n\
        });\n\
    }\n\
}";

/*  ======== Filter ========
 *  logger: xdc.runtime.LoggerBuf
 *  module: ti.sysbios.knl.Task
 *  fxn(logEvent)
 */
let filter_Task =
"{\n\
    var rec = {\n\
        seq: Number(logEvent.serial),\n\
        ts: Number(logEvent.timestampRaw),\n\
        label: logEvent.text,\n\
        y: -1\n\
    };\n\
\n\
    switch (logEvent.eventName) {\n\
        case \\\"ti.sysbios.knl.Task.LD_block\\\":\n\
            rec.type = \\\"Task_Block\\\";\n\
            rec.hndl = \\\"0x\\\" + Number(logEvent.arg0).toString(16);\n\
            break;\n\
        case \\\"ti.sysbios.knl.Task.LD_ready\\\":\n\
            rec.type = \\\"Task_Ready\\\";\n\
            rec.hndl = \\\"0x\\\" + Number(logEvent.arg0).toString(16);\n\
            break;\n\
        case \\\"ti.sysbios.knl.Task.LM_switch\\\":\n\
            rec.type = \\\"Task_Switch\\\";\n\
            rec.hndl_new = \\\"0x\\\" + Number(logEvent.arg2).toString(16);\n\
            rec.hndl_old = logEvent.arg0 == 0 ? \\\"\\\" :\n\
                \\\"0x\\\" + Number(logEvent.arg0).toString(16);\n\
            break;\n\
        default:\n\
            rec.type = \\\"unknown\\\";\n\
    };\n\
\n\
    return (rec);\n\
}";

/*
 *  ======== Filter ========
 *  logger: xdc.runtime.LoggerBuf
 *  module: ti.sysbios.knl.Swi
 *  fxn(logEvent)
 */
let filter_Swi =
"{\n\
    var rec = {\n\
        seq: Number(logEvent.serial),\n\
        ts: Number(logEvent.timestampRaw),\n\
        label: logEvent.text,\n\
        y: -1\n\
    };\n\
\n\
    switch (logEvent.eventName) {\n\
        case \\\"ti.sysbios.knl.Swi.LM_begin\\\":\n\
            rec.type = \\\"Swi_Begin\\\";\n\
            rec.hndl = \\\"0x\\\" + Number(logEvent.arg0).toString(16);\n\
            break;\n\
        case \\\"ti.sysbios.knl.Swi.LD_end\\\":\n\
            rec.type = \\\"Swi_End\\\";\n\
            rec.hndl = \\\"0x\\\" + Number(logEvent.arg0).toString(16);\n\
            break;\n\
        default:\n\
            rec.type = \\\"unknown\\\";\n\
    };\n\
\n\
    return (rec);\n\
}";

/*
 *  ======== Filter ========
 *  logger: xdc.runtime.LoggerBuf
 *  module: ti.sysbios.family.*.Hwi
 *  fxn(logEvent)
 */
let filter_Hwi =
"{\n\
    var rec = {\n\
        seq: Number(logEvent.serial),\n\
        ts: Number(logEvent.timestampRaw),\n\
        label: logEvent.text,\n\
        y: -1\n\
    };\n\
\n\
    if (logEvent.eventName.match(/Hwi\.LM_begin$/)) {\n\
        rec.type = \\\"Hwi_Begin\\\";\n\
        rec.hndl = \\\"0x\\\" + Number(logEvent.arg0).toString(16);\n\
    }\n\
    else if (logEvent.eventName.match(/Hwi\.LD_end$/)) {\n\
        rec.type = \\\"Hwi_End\\\";\n\
        rec.hndl = \\\"0x\\\" + Number(logEvent.arg0).toString(16);\n\
    }\n\
    else {\n\
        rec.type = \\\"unknown\\\";\n\
    }\n\
\n\
    return (rec);\n\
}";

/*
 *  ======== Filter ========
 *  logger: xdc.runtime.LoggerBuf
 *  module: ti.sysbios.knl.Clock
 *  fxn(logEvent)
 */
let filter_Clock =
"{\n\
    var rec = {\n\
        seq: Number(logEvent.serial),\n\
        ts: Number(logEvent.timestampRaw),\n\
        label: logEvent.text,\n\
        type: \\\"unknown\\\",\n\
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
        name: "ExecGraphAdapter_sysbios_Task",
        moduleName: "/ti/utils/runtime/ExecGraphAdapter",
        args: {
            $name: "CONFIG_ExecGraphAdapter_sysbios_Task",
            label: "SYS/BIOS Task",
            module: "ti.sysbios.knl.Task",
            view: "Basic",
            fxn: adapter_Task_Basic
        }
    });

    modules.push({
        name: "ExecGraphAdapter_sysbios_Swi",
        moduleName: "/ti/utils/runtime/ExecGraphAdapter",
        args: {
            $name: "CONFIG_ExecGraphAdapter_sysbios_Swi",
            label: "SYS/BIOS Swi",
            module: "ti.sysbios.knl.Swi",
            view: "Basic",
            fxn: adapter_Swi_Basic
        }
    });

    modules.push({
        name: "ExecGraphAdapter_sysbios_Hwi",
        moduleName: "/ti/utils/runtime/ExecGraphAdapter",
        args: {
            $name: "CONFIG_ExecGraphAdapter_sysbios_Hwi",
            label: "SYS/BIOS Hwi",
            module: "ti.sysbios.family.*.Hwi",
            view: "Basic",
            fxn: adapter_Hwi_Basic
        }
    });

    modules.push({
        name: "ExecGraphFilter_sysbios_Task",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "CONFIG_ExecGraphFilter_sysbios_Task",
            logger: "xdc.runtime.LoggerBuf",
            module: "ti.sysbios.knl.Task",
            fxn: filter_Task
        }
    });

    modules.push({
        name: "ExecGraphFilter_sysbios_Swi",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "CONFIG_ExecGraphFilter_sysbios_Swi",
            logger: "xdc.runtime.LoggerBuf",
            module: "ti.sysbios.knl.Swi",
            fxn: filter_Swi
        }
    });

    modules.push({
        name: "ExecGraphFilter_sysbios_Hwi",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "CONFIG_ExecGraphFilter_sysbios_Hwi",
            logger: "xdc.runtime.LoggerBuf",
            module: "ti.sysbios.family.*.Hwi",
            fxn: filter_Hwi
        }
    });

    modules.push({
        name: "ExecGraphFilter_sysbios_Clock",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "CONFIG_ExecGraphFilter_sysbios_Clock",
            logger: "xdc.runtime.LoggerBuf",
            module: "ti.sysbios.knl.Clock",
            fxn: filter_Clock
        }
    });

    return (modules);
}

/*
 *  ======== modules ========
 */
function modules(inst)
{
    let modules = new Array();

    modules.push({
        name: "ExGr_XDCtools",
        moduleName: "/ti/utils/runtime/ExGr_XDCtools"
    });

    return (modules);
}

/*
 *  ======== base ========
 *  Module definition object
 */
let base = {
    displayName: "ExGr_SYSBIOS",
    description: "Execution Graph setup for SYS/BIOS",
    moduleStatic: {
        moduleInstances: moduleInstances,
        modules: modules
    }
};

/* export the module */
exports = base;
