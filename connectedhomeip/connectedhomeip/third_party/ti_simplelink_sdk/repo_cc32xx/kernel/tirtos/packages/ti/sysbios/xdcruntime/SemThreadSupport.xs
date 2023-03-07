/*
 * Copyright (c) 2012, Texas Instruments Incorporated
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
 *  ======== SemThread.xs ========
 */

var ISemaphore = null;
var Semaphore = null;
var Clock = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    ISemaphore = xdc.useModule('xdc.runtime.knl.ISemaphore');
    Semaphore = xdc.useModule('ti.sysbios.knl.Semaphore');
    Clock = xdc.useModule('ti.sysbios.knl.Clock');
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, count, params)
{
    var semParams = new Semaphore.Params();
    if (params.mode == ISemaphore.Mode_COUNTING) {
        semParams.mode = Semaphore.Mode_COUNTING;
    }
    else {
        semParams.mode = Semaphore.Mode_BINARY;
    }
    Semaphore.construct(obj.sem, count, semParams);
}

/*
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' instance view.
 */
function viewInitBasic(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    
    view.label = Program.getShortName(obj.$label);

    view.SemaphoreHandle = "0x" + Number(obj.sem.$addr).toString(16);

    try {
        var semView = 
            Program.scanObjectView('ti.sysbios.knl.Semaphore', obj.sem, 'Basic');
    }
    catch (e) {
        Program.displayError(view, "pendedTasks",
            "Error: could not scan view of Event Handle: " + e.toString());
        return;
    }

    /* Label the Semaphore so that it's clear it belongs to this object. */
    var semLabel;
    
    /* 
     * If the SemThreadSupport instance has not been named, use the full label,
     * e.g., ti.sysbios.xdcruntime.SemThreadSupport@800268d4.
     */
    if (view.label == "") {
        semLabel = obj.$label;
    }
    else {
        semLabel = view.label;
    }
    
    /* 
     * Add 'Semaphore in ' to the beginning, giving us:
     * Semaphore in ti.sysbios.xdcruntime.SemThreadSupport@800268d4.
     */
    semView.label = "Semaphore in " + semLabel;
    
    /* 
     * Copy over the 'pendedTasks' field to display all tasks pended on this
     * SemThreadSupport object.
     */
    view.pendedTasks = semView.pendedTasks;
}
