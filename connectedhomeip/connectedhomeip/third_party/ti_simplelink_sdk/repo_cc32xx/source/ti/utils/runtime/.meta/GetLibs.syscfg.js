/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== GetLibs.syscfg.js ========
 */

"use strict";

/*
 *  ======== modules ========
 */
function modules(inst)
{
    let modules = new Array();

    modules.push({
        name: "genlibs",
        displayName: "GenLibs",
        moduleName: "/ti/utils/build/GenLibs"
    });

    return (modules);
}

/*
 *  ======== getLibs ========
 */
function getLibs(mod)
{
    let GenLibs = system.getScript("/ti/utils/build/GenLibs.syscfg.js");

    var libs = [];
    var deps = [];
    var profile = "_release";

    libs.push(GenLibs.libPath("ti/utils/runtime", "runtime" + profile + ".a"));

    /* create a GenLibs input argument */
    var linkOpts = {
        name: "/ti/utils/runtime",
        deps: deps,
        libs: libs
    };

    return (linkOpts);
}

/*
 *  ======== base ========
 */
let base = {
    displayName: "GetLibs",
    description: "GetLibs module for ti.utils.runtime",

    moduleStatic: {
        modules: modules
    },

    templates: {
        "/ti/utils/build/GenLibs.cmd.xdt":
            { modName: "/ti/utils/runtime/GetLibs", getLibs: getLibs }
    }
};

/* export the module */
exports = base;
