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
 *  ======== Exception.xs ========
 *
 */

var Hwi = null;
var Exception = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    Exception = this;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Exception.$logWarning("The ti.sysbios.family.arm.m3.Exception module " +
                          "is being deprecated.  " +
                          "Its functionality has been integrated into the " +
                          "ti.sysbios.family.arm.m3.Hwi module.  " +
                          "Please remove any references to this module from " +
                          "your application.", this);

    Hwi = xdc.useModule('ti.sysbios.family.arm.m3.Hwi');

    if (Exception.doPrint == true) {
        Hwi.enableException = true;
    }
}

/*
 *  ======== viewInitModule ========
 */
function viewInitModule(view, mod)
{
    var Program = xdc.useModule('xdc.rov.Program');

    if (mod.excContext == 0) {
        view.context[0] =  "No Exception Context to Display.";
        return;
    }

    var context = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt32', isScalar: true}, mod.excContext, 24, false);

    view.context[0] =  "r0 = " +  Number(context[0]).toString(16);
    view.context[1] =  "r1 = " +  Number(context[1]).toString(16);
    view.context[2] =  "r2 = " +  Number(context[2]).toString(16);
    view.context[3] =  "r3 = " +  Number(context[3]).toString(16);
    view.context[4] =  "r4 = " +  Number(context[4]).toString(16);
    view.context[5] =  "r5 = " +  Number(context[5]).toString(16);
    view.context[6] =  "r6 = " +  Number(context[6]).toString(16);
    view.context[7] =  "r7 = " +  Number(context[7]).toString(16);
    view.context[8] =  "r8 = " +  Number(context[8]).toString(16);
    view.context[9] =  "r9 = " +  Number(context[9]).toString(16);
    view.context[10] = "r10 = " + Number(context[10]).toString(16);
    view.context[11] = "r11 = " + Number(context[11]).toString(16);
    view.context[12] = "r12 = " + Number(context[12]).toString(16);
    view.context[13] = "sp = " +  Number(context[13]).toString(16);
    view.context[14] = "lr = " +  Number(context[14]).toString(16);
    view.context[15] = "pc = " +  Number(context[15]).toString(16);
    view.context[16] = "psr = " + Number(context[16]).toString(16);

    view.context[17] = "MMFSR = "+ Number(context[17]).toString(16);
    view.context[18] = "BFSR = " + Number(context[18]).toString(16);
    view.context[19] = "UFSR = " + Number(context[19]).toString(16);
    view.context[20] = "HFSR = " + Number(context[20]).toString(16);
    view.context[21] = "DFSR = " + Number(context[21]).toString(16);
    view.context[22] = "BFAR = " + Number(context[22]).toString(16);
    view.context[23] = "AFSR = " + Number(context[23]).toString(16);
}

/*
 *  ======== viewInitContext ========
 */
function viewInitContext(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Exception = xdc.useModule('ti.sysbios.family.arm.m3.Exception');

    var viewArr = new Array();

    var view = new Program.InstDataView();


    try {
        var excRawView = Program.scanRawView('ti.sysbios.family.arm.m3.Exception');
    }
    catch (e) {
        return null;
    }

    var excContext = excRawView.modState.excContext;

    if (excContext == 0) {
        view.label = "No Exception Context";
        viewArr[0] = view;
        return (viewArr);
    }

    try {
        var context = Program.fetchStruct(Exception.Context$fetchDesc, excContext);
    }
    catch (e) {
        return;
    }

    var fields = [
        "r0","r1","r2","r3","r4","r5","r6","r7",
        "r8","r9","r10","r11","r12","sp","lr","pc",
        "psr","MMFSR","BFSR","UFSR","HFSR","DFSR","MMAR","BFAR","AFSR"
    ];

    for (var x in fields) {
        var p = fields[x];
        var fieldElem = Program.newViewStruct('ti.sysbios.family.arm.m3.Exception', 'Context');
        fieldElem.register = p;
        fieldElem.value = "0x" + context[p].toString(16);
        view.elements.$add(fieldElem);
    }

    view.label = "Exception Context";

    viewArr[0] = view;

    return (viewArr);
}
