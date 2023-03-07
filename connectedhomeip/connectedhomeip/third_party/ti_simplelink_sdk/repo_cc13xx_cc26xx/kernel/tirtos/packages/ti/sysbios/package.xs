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
 *  ======== Package.getLibs ========
 *  This function is called when a program's configuration files are
 *  being generated and it returns the name of a library appropriate
 *  for the program's configuration.
 */

function getLibs(prog)
{
    var name = this.$name + ".a" + prog.build.target.suffix;
    var lib = "";

    var BIOS = xdc.module("ti.sysbios.BIOS");
    var Build = xdc.module("ti.sysbios.Build");

    switch (BIOS.libType) {
        case BIOS.LibType_Instrumented:
        case BIOS.LibType_NonInstrumented:
        case BIOS.LibType_Custom:
        case BIOS.LibType_Debug:
        default:
            if (BIOS.$used == false) return (null);
            if (Build.buildROM == true) return (null);
            lib = Build.$private.outputDir + Build.$private.libraryName;
            return ("!" + String(java.io.File(lib).getCanonicalPath()));
            break;
    }

    /* could not find any library, throw exception */
    throw Error("Library not found: " + name);
}

/*
 *  ======== package.close ========
 */
function close()
{
    if (xdc.om.$name != 'cfg') {
        return;
    }

    /*
     * Force the Build module to get used if any module
     * in this package is used
     */
    for (var mod in this.$modules) {
        if (this.$modules[mod].$used == true) {
            xdc.useModule('ti.sysbios.Build');
            break;
        }
    }

    /* add default xdc.runtime.Timestamp.SupportProxy binding */
    var Timestamp = xdc.module('xdc.runtime.Timestamp');

    if (!Timestamp.$written("SupportProxy")) {
        var Settings = xdc.module("ti.sysbios.family.Settings");
        /* if Timestamp is used, make sure the delegate is also used */
        if (Timestamp.$used == true) {
            /* make sure the delegate gets used as well */
            Timestamp.SupportProxy =
                xdc.useModule(Settings.getDefaultTimestampDelegate());
        }
        else {
            Timestamp.SupportProxy =
                xdc.module(Settings.getDefaultTimestampDelegate());
        }
    }
}
