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
 *  ======== Idle.xs ========
 */

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    /* add user specified functions to funcList array */
    for (var i = 0; i < this.idleFxns.length; i++) {
        var fxn = this.idleFxns[i];
        if (fxn != null) {
            this.addFunc(fxn);
        }
    }
}

/*
 *  ======== module$validate ========
 */
function module$validate()
{
    /* 
     * Idle functions added with Idle.addFunc or Idle.funcList.$add()
     * will have no corresponding coreList[] value.
     * Repair the damage.
     */

    /* first force the two list to have the same size */
    this.coreList.length = this.funcList.length;

    /* then set the undefined coreList[] entries to core 0 */
    for (var i = 0; i < this.funcList.length; i++) {
        if (this.coreList[i] === undefined) {
            this.coreList[i] = 0;
        }
    }
}

/*
 *  ======== addFunc ========
 */
function addFunc(func)
{
    this.funcList.$add(func);
}

/*
 *  ======== addCoreFunc ========
 */
function addCoreFunc(func, coreId)
{
    /* 
     * Idle functions added with Idle.addFunc or Idle.funcList.$add()
     * will have no corresponding coreList[] value.
     * force the two list to have the same size 
     * so the coreId lines up with the func
     */
    this.coreList.length = this.funcList.length;

    this.funcList.$add(func);
    this.coreList.$add(coreId);
}

/*
 *  ======== viewInitModule ========
 *  Initialize the Task module view.
 */
function viewInitModule(view, mod)
{
    var Idle = xdc.useModule('ti.sysbios.knl.Idle');
    var Program = xdc.useModule('xdc.rov.Program');

    var modConfig = Program.getModuleConfig(Idle.$name);

    var elements = new Array();

    for (var i=0; i< modConfig.funcList.length; i++) {
        var elem = Program.newViewStruct('ti.sysbios.knl.Idle', 'Idle.funcList');
        elem.index = i;
        elem.coreId = modConfig.coreList[i]; 
        elem.fxn = modConfig.funcList[i];
        elements[elements.length] = elem;                
    }
    view.elements = elements;
}
