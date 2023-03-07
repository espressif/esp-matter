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
 *  ======== ExecGraphSettings.rov.js ========
 */

var Program = xdc.module('xdc.rov.Program');
var Monitor = xdc.module('xdc.rov.runtime.Monitor');

var moduleName = "ExecGraphSettings";
var ExGr_configFileName = "ti_utils_runtime_execgraph.js.xs";

var viewMap = [
    {
        name: "Module",
        fxn: "viewInitModule",
        structName: "ModuleView"
    }
];

function ModuleView()
{
    this.TimestampHZ = 0;
}

function viewInitModule()
{
//  Monitor.println("ExecGraphSettings: viewInitModule");

    var table = new Array();
    var data = ExGrAdap_fetchData();
    if (data == null) return (table);

    var view = new ModuleView();
    view.TimestampHZ = data.tsHz;
    table.push(view);

    return (table);
}

function ExGrAdap_fetchData()
{
//  Monitor.println("ExecGraphSettings: ExGrAdap_fetchData");

    /* make sure configuration file exists */
    var cfgFileName = gen_dir() + "/" + ExGr_configFileName;
    var exGrCfg = new java.io.File(cfgFileName);

    if (!exGrCfg.exists()) {
        Monitor.println("Error: ExecGraph configuration file not found.");
        return (null);
    }

    /* load ExecGraphSettings generated configuration file */
    var cap = xdc.loadCapsule(cfgFileName);
    var state = cap.ExecGraphSettings_state;

    /* state information */
    var data = {};
    data.tsHz = state.timestampHz;

    return (data);
}

/*
 *  ======== gen_dir ========
 *  Find the code generation directory used by SysConfig
 *
 *  XDCtools exports utils as global object from the file xdc/utils.js.
 */
function gen_dir()
{

    /* look for the file syscfg_c.rov.xs in the hash of all loaded files */
    var genDir = null;
    var keys = Object.keys(utils.loadedFiles);

    for (var i = 0; i < keys.length; i++) {
        var tokens = keys[i].match(/(.*)[\\/]syscfg_c\.rov\.xs$/);
        if (tokens) {
            genDir = tokens[1].replace(/\\/g, "/");
            break;
        }
    }

    return (genDir);
}
