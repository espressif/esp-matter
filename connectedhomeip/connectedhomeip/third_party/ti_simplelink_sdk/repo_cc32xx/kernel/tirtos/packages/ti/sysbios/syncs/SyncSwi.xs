/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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
 *  ======== SyncSwi.xs ========
 *
 */

var Swi = null;
var SyncSwi = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    SyncSwi = this;
    Swi = xdc.useModule("ti.sysbios.knl.Swi");
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, params)
{
    if (!params.swi) {
        SyncSwi.$logFatal("params.swi cannot be null", this);
    }

    obj.swi = params.swi;
}

/*
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' SyncEvent instance view.
 */

function viewInitBasic(view, obj)
{
    view.label = Program.getShortName(obj.$label);
    view.SwiHandle = "0x" + Number(obj.swi).toString(16);

    try {
        var SwiView = Program.scanHandleView('ti.sysbios.knl.Swi', obj.swi,
                'Basic');
    }
    catch (e) {
        print("Error: could not scan view of Swi Handle: " +
                e.toString());
        return;
    }

    view.fxn = SwiView.fxn;
}
