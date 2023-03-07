/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 *  ======== Idle.rov.js ========
 */

var moduleName = "ti.sysbios.knl::Idle";

var viewMap = [
    {name: "Idle.funcList", fxn: "viewIdleFuncList", structName: "FuncListInfo"}
];

function FuncListInfo()
{
    this.index  = null;
    this.fxn    = null;
}

function viewGetIdleFuncList()
{
    idleArray = new Array();

    try {
        var funcList = Program.fetchVariable("ti_sysbios_knl_Idle_funcList");
    }
    catch (e) {
        return (idleArray)
    }

    for (var i = 0; i < funcList.length; i++) {
        var idleFuncObj = new Object();
        idleFuncObj.index = i;
        idleFuncObj.fxn = String(Program.lookupFuncName(Number(funcList[i])));
        idleArray.push(idleFuncObj);
    }

    return (idleArray);
}

function viewInitFuncList(view, obj)
{
    view.index  = obj.index;
    view.fxn    = obj.fxn;
}

function viewIdleFuncList()
{
    view = new Array();

    var idleFxns = viewGetIdleFuncList();
    for (var i = 0; i < idleFxns.length; i++) {
        var obj = idleFxns[i];
        var funcListView = new FuncListInfo();
        viewInitFuncList(funcListView, obj);
        view.push(funcListView);
    }

    return (view);
}
