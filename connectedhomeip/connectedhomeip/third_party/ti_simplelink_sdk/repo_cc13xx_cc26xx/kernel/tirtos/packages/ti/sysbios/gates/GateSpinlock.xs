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
 *  ======== GateSpinlock.xs ========
 */

/*
 *  ======== module$use ========
 */
function module$use()
{
    xdc.useModule("ti.sysbios.knl.Task");
    var BIOS = xdc.useModule("ti.sysbios.BIOS");
    if (!(BIOS.libType == BIOS.LibType_Custom
        && BIOS.assertsEnabled == false)) {
        xdc.useModule('xdc.runtime.Assert');
    }
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, params)
{
    obj.owner = null;
    obj.hwiKey = 0;
}

/*
 *  ======== queryMeta ========
 */
function queryMeta(qual)
{
    var rc = false;
    var IGateProvider = xdc.module('xdc.runtime.IGateProvider');

    switch (qual) {
        case IGateProvider.Q_BLOCKING:
            rc = false;
            break;
        case IGateProvider.Q_PREEMPTING:
            rc = true;
            break;
        default:
            this.$logWarning("Invalid quality. Returning false", this, qual);
            break;
    }

    return (rc);
}

/*
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' Task instance view.
 */
function viewInitBasic(view, obj)
{
    /* Get owner task. Note: if NULL, there is no owner. */
    if (obj.owner == 0) {
        view.owner  = "N/A";
        view.status = "Not entered";
    }
    else {
        try {
            var taskView = Program.scanHandleView('ti.sysbios.knl.Task',
                obj.owner, 'Basic');
        }
        catch (e) {
            view.$status["owner"] = "Error: Problem getting owning Task: " + e;
            return;
        }

        // if this Task has no label, use the function name
        if (taskView.label == "" && taskView.fxn[0] != "") {
            view.owner = "taskfxn: " + taskView.fxn[0];
        }
        else {
            view.owner = taskView.label;
        }

        view.status = "Entered";
    }
}
