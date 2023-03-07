/*
 * Copyright (c) 2012-2020, Texas Instruments Incorporated
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
 *  ======== ThreadSupport.xs ========
 */

var Semaphore = null;
var Task = null;
var ThreadSupport = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    ThreadSupport = this;
    Semaphore = xdc.useModule('ti.sysbios.knl.Semaphore');
    Task = xdc.useModule('ti.sysbios.knl.Task');

    if (Task.numPriorities <= 5) {
        ThreadSupport.$logFatal("The lowest value for Task.numPriorities" +
        "allowed is 6. The current value is " + Task.numPriorities, this);
    }
    ThreadSupport.lowestPriority = 1;
    ThreadSupport.belowNormalPriority =
        Math.ceil((Task.numPriorities * 2/5) - 1);
    ThreadSupport.normalPriority = Math.ceil((Task.numPriorities* 3/5) - 1);
    ThreadSupport.aboveNormalPriority =
        Math.ceil((Task.numPriorities * 4/5) - 1);
    ThreadSupport.highestPriority = (Task.numPriorities) - 1;
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, fxn, params)
{
    var taskParams = new Task.Params();
    taskParams.arg0 = obj;

    if (params.osPriority != ThreadSupport.INVALID_OS_PRIORITY) {
        taskParams.priority = params.osPriority;
    }
    else {
        if (params.priority == ThreadSupport.Priority_LOWEST) {
            taskParams.priority = ThreadSupport.lowestPriority;
        }
        else if (params.priority == ThreadSupport.Priority_BELOW_NORMAL) {
            taskParams.priority = ThreadSupport.belowNormalPriority;
        }
        else if (params.priority == ThreadSupport.Priority_NORMAL) {
            taskParams.priority = ThreadSupport.normalPriority;
        }
        else if (params.priority == ThreadSupport.Priority_ABOVE_NORMAL) {
            taskParams.priority = ThreadSupport.aboveNormalPriority;
        }
        else if (params.priority == ThreadSupport.Priority_HIGHEST) {
            taskParams.priority = ThreadSupport.highestPriority;
        }
        else {
            ThreadSupport.$logFatal("Invalid priority value " + params.priority,
                this, "priority");
        }
    }

    taskParams.stackSize = params.stackSize;
    taskParams.env = obj;

    obj.task = Task.create("&ti_sysbios_xdcruntime_ThreadSupport_runStub",
        taskParams);
    obj.tls = params.tls;
    obj.startFxn = fxn;
    obj.startFxnArg = params.arg;
    Semaphore.construct(obj.join_sem, 0);
}
