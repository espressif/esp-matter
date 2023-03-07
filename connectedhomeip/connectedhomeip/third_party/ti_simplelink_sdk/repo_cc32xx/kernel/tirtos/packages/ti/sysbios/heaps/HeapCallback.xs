/*
 * Copyright (c) 2013, Texas Instruments Incorporated
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
 *  ======== HeapCallback.xs ========
 */
/*
 *  ======== module$use ========
 */
function module$use()
{
    xdc.useModule("xdc.runtime.Startup");
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, prms)
{
    obj.context = 0;
    obj.arg = prms.arg;
}

/*
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' instance view.
 */
function viewInitBasic(view, obj)
{
    view.arg = obj.arg;
    view.context = obj.context;
}

/*
 *  ======== viewInitModule ========
 *  Displays the user supplied callback fxns
 */
function viewInitModule(view, mod)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var HeapCallback = xdc.useModule('ti.sysbios.heaps.HeapCallback');
    var modCfg = Program.getModuleConfig(HeapCallback.$name);

    view.initInstFxn = modCfg.initInstFxn;
    view.createInstFxn = modCfg.createInstFxn;
    view.deleteInstFxn = modCfg.deleteInstFxn;
    view.allocInstFxn = modCfg.allocInstFxn;
    view.freeInstFxn = modCfg.freeInstFxn;
    view.isBlockingInstFxn = modCfg.isBlockingInstFxn;
    view.getStatsInstFxn = modCfg.getStatsInstFxn;
}