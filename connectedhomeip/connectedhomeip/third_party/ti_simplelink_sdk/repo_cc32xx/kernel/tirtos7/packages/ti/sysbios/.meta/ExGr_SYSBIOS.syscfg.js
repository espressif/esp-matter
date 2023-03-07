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
 *  ======== ExGr_SYSBIOS.syscfg.js ========
 *  Helper module to create execution graph data adapters and filters
 *
 *  Note:
 *
 *  The adapter and filter function definitions will be stringified and
 *  used in module instance requirements. Only the function bodies are
 *  actually used. Later, in ROV, the function bodies will be used in a
 *  function constructor along with a pre-defined signature. Therefore,
 *  you cannot change the signature. Also, do not invoke these functions
 *  in this script.
 *
 *  The stringification method (toString()) is provided by the JavaScript
 *  run-time implementation, it is not provided by the language. It does
 *  not handle double-quote (") characters in the function body.
 */

"use strict";

/*
 *  ======== Adapter ========
 *  Execution Graph adapter function for ROV view
 *
 *  module: ti.sysbios.family.*.Hwi
 *  view: Basic
 *  signature: fxn(execData, viewData)
 *
 *  See note above.
 */
function adapter_Hwi_Basic(execData, viewData)
{
    for (var i = 0; i < viewData.elements.length; i++) {
        var hwi = viewData.elements[i];
        var label = 'Hwi: ' + hwi.fxn;
        execData.threads.tasks.push({
            label: label,
            hndl: hwi.handle,
            type: 3
        });
    }
}

/*
 *  ======== Adapter ========
 *  Execution Graph adapter function for ROV view
 *
 *  module: ti.sysbios.knl.Swi
 *  view: Basic
 *  signature: fxn(execData, viewData)
 *
 *  See note above.
 */
function adapter_Swi_Basic(execData, viewData)
{
    for (var i = 0; i < viewData.elements.length; i++) {
        var swi = viewData.elements[i];
        var label = 'Swi: ' + swi.fxn;
        execData.threads.tasks.push({
            label: label,
            hndl: swi.handle,
            type: 2
        });
    }
}

/*
 *  ======== Adapter ========
 *  Execution Graph adapter function for ROV view
 *
 *  module: ti.sysbios.knl.Task
 *  view: Basic
 *  signature: fxn(execData, viewData)
 *
 *  See note above.
 */
function adapter_Task_Basic(execData, viewData)
{
    for (var i = 0; i < viewData.elements.length; i++) {
        var task = viewData.elements[i];
        var label = '';
        if (task.name.match(/Idle_taskStruct/i)) {
            label = 'Idl: ' + task.handle;
        }
        else if (task.name != '') {
            label = 'Tsk: ' + task.name;
        }
        else {
            label = 'Tsk: ' + task.fxn;
        }
        execData.threads.tasks.push({
            label: label,
            hndl: task.handle,
            type: 1,
            pri: Number(task.priority),
            stack: [ task.handle ]
        });
    }
}

/*
 *  ======== Filter ========
 *  Execution Graph filter function for ROV view
 *
 *  logger: ti.loggers.utils.LoggerBuf
 *  module: ti.sysbios.knl.Clock
 *  signature: fxn(logEvent)
 *
 *  See note above.
 */
function filter_Clock(logEvent)
{
    var rec = {
        seq: Number(logEvent.Serial),
        ts: Number(logEvent.Timestamp),
        label: logEvent.Message,
        type: 'Data',
        y: -1
    };

    return (rec);
};

/*
 *  ======== Filter ========
 *  Execution Graph filter function for ROV view
 *
 *  logger: ti.loggers.utils.LoggerBuf
 *  module: ti.sysbios.family.*.Hwi
 *  signature: fxn(logEvent)
 *
 *  See note above.
 */
function filter_Hwi(logEvent)
{
    var rec = {
        seq: Number(logEvent.Serial),
        ts: Number(logEvent.Timestamp),
        label: logEvent.Message,
        y: -1
    };

    switch (logEvent.Type) {
        case 'Log_Type_HwiBegin':
            rec.type = 'Hwi_Begin';
            rec.hndl = '0x' + Number(logEvent.Arg0).toString(16);
            break;
        case 'Log_Type_HwiEnd':
            rec.type = 'Hwi_End';
            rec.hndl = '0x' + Number(logEvent.Arg0).toString(16);
            break;
        default:
            rec.type = 'unknown';
    }

    return (rec);
}

/*
 *  ======== Filter ========
 *  Execution Graph filter function for ROV view
 *
 *  logger: ti.loggers.utils.LoggerBuf
 *  module: ti.sysbios.knl.Swi
 *  signature: fxn(logEvent)
 *
 *  See note above.
 */
function filter_Swi(logEvent)
{
    var rec = {
        seq: Number(logEvent.Serial),
        ts: Number(logEvent.Timestamp),
        label: logEvent.Message,
        y: -1
    };

    switch (logEvent.Type) {
        case 'Log_Type_SwiBegin':
            rec.type = 'Swi_Begin';
            rec.hndl = '0x' + Number(logEvent.Arg0).toString(16);
            break;
        case 'Log_Type_SwiEnd':
            rec.type = 'Swi_End';
            rec.hndl = '0x' + Number(logEvent.Arg0).toString(16);
            break;
        default:
            rec.type = 'unknown';
    }

    return (rec);
};

/*
 *  ======== Filter ========
 *  Execution Graph filter function for ROV view
 *
 *  logger: ti.loggers.utils.LoggerBuf
 *  module: ti.sysbios.knl.Task
 *  signature: fxn(logEvent)
 *
 *  See note above.
 */
function filter_Task(logEvent)
{
    var rec = {
        seq: Number(logEvent.Serial),
        ts: Number(logEvent.Timestamp),
        label: logEvent.Message,
        y: -1
    };

    switch (logEvent.Type) {
        case 'Log_Type_TaskBlock':
            rec.type = 'Task_Block';
            rec.hndl = '0x' + Number(logEvent.Arg0).toString(16);
            break;
        case 'Log_Type_TaskReady':
            rec.type = 'Task_Ready';
            rec.hndl = '0x' + Number(logEvent.Arg0).toString(16);
            break;
        case 'Log_Type_TaskSwitch':
            rec.type = 'Task_Switch';
            rec.hndl_new = '0x' + Number(logEvent.Arg2).toString(16);
            rec.hndl_old = logEvent.arg0 == 0 ? '' :
                '0x' + Number(logEvent.Arg0).toString(16);
            break;
        default:
            rec.type = 'unknown';
    }

    return (rec);
}

/*
 *  ======== fsig ========
 *  A regular expression to match a function signature
 *
 *  Used below in the replace() method to remove the function signature
 *  at the beginning of a stingified function, leaving only the function
 *  body.
 */
let fsig = new RegExp(/^\s*function[^(]*\([^)]*\)[^{]*/);

/*
 *  ======== moduleInstances ========
 */
function moduleInstances(inst)
{
    let reqs = new Array();

    reqs.push({
        name: "ExecGraphAdapter_Hwi",
        moduleName: "/ti/utils/runtime/ExecGraphAdapter",
        args: {
            $name: "ti_sysbios_hal_Hwi_ExecGraphAdapter",
            label: "SYS/BIOS Hwi",
            module: "ti.sysbios.family.*.Hwi",
            view: "Basic",
            fxn: adapter_Hwi_Basic.toString().replace(fsig, "")
        }
    });

    reqs.push({
        name: "ExecGraphAdapter_Swi",
        moduleName: "/ti/utils/runtime/ExecGraphAdapter",
        args: {
            $name: "ti_sysbios_knl_Swi_ExecGraphAdapter",
            label: "SYS/BIOS Swi",
            module: "ti.sysbios.knl.Swi",
            view: "Basic",
            fxn: adapter_Swi_Basic.toString().replace(fsig, "")
        }
    });

    reqs.push({
        name: "ExecGraphAdapter_Task",
        moduleName: "/ti/utils/runtime/ExecGraphAdapter",
        args: {
            $name: "ti_sysbios_knl_Task_ExecGraphAdapter",
            label: "SYS/BIOS Task",
            module: "ti.sysbios.knl.Task",
            view: "Basic",
            fxn: adapter_Task_Basic.toString().replace(fsig, "")
        }
    });

    reqs.push({
        name: "ExecGraphFilter_Clock",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "ti_sysbios_knl_Clock_ExecGraphFilter",
            logger: "ti.loggers.utils.LoggerBuf",
            module: "ti.sysbios.knl.Clock",
            fxn: filter_Clock.toString().replace(fsig, "")
        }
    });

    reqs.push({
        name: "ExecGraphFilter_Hwi",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "ti_sysbios_hal_Hwi_ExecGraphFilter",
            logger: "ti.loggers.utils.LoggerBuf",
            module: "ti.sysbios.family.*.Hwi",
            fxn: filter_Hwi.toString().replace(fsig, "")
        }
    });

    reqs.push({
        name: "ExecGraphFilter_Swi",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "ti_sysbios_knl_Swi_ExecGraphFilter",
            logger: "ti.loggers.utils.LoggerBuf",
            module: "ti.sysbios.knl.Swi",
            fxn: filter_Swi.toString().replace(fsig, "")
        }
    });

    reqs.push({
        name: "ExecGraphFilter_Task",
        moduleName: "/ti/utils/runtime/ExecGraphFilter",
        args: {
            $name: "ti_sysbios_knl_Task_ExecGraphFilter",
            logger: "ti.loggers.utils.LoggerBuf",
            module: "ti.sysbios.knl.Task",
            fxn: filter_Task.toString().replace(fsig, "")
        }
    });

    return (reqs);
}

/*
 *  ======== modules ========
 */
function modules(inst)
{
    let modules = new Array();

    modules.push({
        name: "ExGr_LoggerBuf",
        moduleName: "/ti/loggers/utils/ExGr_LoggerBuf"
    });

    return (modules);
}

/*
 *  ======== exports ========
 */
exports = {
    displayName: "ExGr_SYSBIOS",
    description: "Execution Graph setup for SYS/BIOS",
    moduleStatic: {
        moduleInstances: moduleInstances,
        modules: modules
    },
    noTargetCode: true
};
