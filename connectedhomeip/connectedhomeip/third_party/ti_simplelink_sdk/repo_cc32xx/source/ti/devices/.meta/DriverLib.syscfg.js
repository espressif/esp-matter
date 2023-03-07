/*
 * Copyright (c) 2019-2021 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== DriverLib.syscfg.js ========
 *  Support use of DriverLib within SysConfig meta-code
 */

"use strict";

/*
 *  ======== config ========
 *  User Config options
 */
let config = [
    {
        name: "path", /* override computed path to driverlib archive */
        displayName: "Library Path",
        description: "Relative path to the driverlib library",
        default: "",
        hidden: true
    }
];

/*
 *  ======== getAttrs ========
 *  Map a SysConfig deviceId to a set of "device family" attrs
 *
 *  @param deviceId  - a pinmux deviceId (system.deviceData)
 *
 *  @returns object - a set of device attributes of the form
 *      {
 *          deviceDir:    "", // name of the directory in /ti/devices/
 *                            // containing driverlib
 *          deviceDefine: ""  // DeviceFamily #define
 *          deviceGroup:  "", // one of "LPRF", "WIFI"
 *      }
 */
function getAttrs(deviceId, part)
{
    var result = {};

    if (deviceId.match(/CC13.1/)) {
        result.deviceDir  = "cc13x1_cc26x1";
        result.deviceGroup = "LPRF";
        result.deviceDefine = "DeviceFamily_CC13X1";
        result.libName = "cc13x1";
    }
    else if (deviceId.match(/CC26.1/)) {
        result.deviceDir = "cc13x1_cc26x1";
        result.deviceGroup = "LPRF";
        result.deviceDefine = "DeviceFamily_CC26X1";
        result.libName = "cc26x1";
    }
    else if (deviceId.match(/CC13.2.7/)) {
        result.deviceDir  = "cc13x2x7_cc26x2x7";
        result.deviceGroup = "LPRF";
        result.deviceDefine = "DeviceFamily_CC13X2X7";
        result.libName = "cc13x2x7";
    }
    else if (deviceId.match(/CC26.2.7/)) {
        result.deviceDir = "cc13x2x7_cc26x2x7";
        result.deviceGroup = "LPRF";
        result.deviceDefine = "DeviceFamily_CC26X2X7";
        result.libName = "cc26x2x7";
    }
    else if (deviceId.match(/CC13.2/)) {
        result.deviceDir  = "cc13x2_cc26x2";
        result.deviceGroup = "LPRF";
        result.deviceDefine = "DeviceFamily_CC13X2";
        result.libName = "cc13x2";
    }
    else if (deviceId.match(/CC26.2/)) {
        result.deviceDir = "cc13x2_cc26x2";
        result.deviceGroup = "LPRF";
        result.deviceDefine = "DeviceFamily_CC26X2";
        result.libName = "cc26x2";
    }
    else if (deviceId.match(/CC26.4/)) {
        result.deviceDir = "cc13x4_cc26x4";
        result.deviceGroup = "LPRF";
        result.deviceDefine = "DeviceFamily_CC26X4";
        result.libName = "cc26x4";
    }
    else if (deviceId.match(/CC2653/)) {
        /* The CC2653 is very similar to CC26X4 from a software point of view,
         * so we use the same deviceDir and libName.
         */
        result.deviceDir = "cc13x4_cc26x4";
        result.deviceGroup = "LPRF";
        result.deviceDefine = "DeviceFamily_CC26X3";
        result.libName = "cc26x4";
    }
    else if (deviceId.match(/CC13.4/)) {
        result.deviceDir = "cc13x4_cc26x4";
        result.deviceGroup = "LPRF";
        result.deviceDefine = "DeviceFamily_CC13X4";
        result.libName = "cc13x4";
    }
    else if (deviceId.match(/CC23.0/)) {
        result.deviceDir = "cc23x0";
        result.deviceGroup = "LPRF";
        result.deviceDefine = "DeviceFamily_CC23X0";
        result.libName = "cc23x0";
    }
    else if (deviceId.match(/CC32/)) {
        result.deviceDir = "cc32xx";
        result.deviceGroup = "WIFI";
        result.deviceDefine = "DeviceFamily_CC3220";
        result.libName = "cc32xx";
    }
    else {
        result.deviceDir = "";
        result.deviceGroup = "";
        result.deviceDefine = "";
        result.libName = "";
    }

    return (result);
}

// /*
//  *  ======== getLibs ========
//  */
// function getLibs(mod)
// {
//     /* get device and compiler toolchain from SysConfig */
//     let devId     = system.deviceData.deviceId;
//     let GenLibs   = system.getScript("/ti/utils/build/GenLibs");
//     let tcDir     = GenLibs.getToolchainDir();

//     /* compute device and toolchain-specific driverlib path */
//     let templates = {
//         LPRF: "ti/devices/{devDir}/driverlib/bin/{tcDir}/driverlib.lib",
//         WIFI: "ti/devices/{devDir}/driverlib/{tcDir}/Release/driverlib.a",
//     };

//     let lib;
//     if (mod.$static.path != "") {
//         lib = mod.$static.path;
//     }
//     else {
//         let attrs = getAttrs(devId);
//         var template = templates[attrs.deviceGroup];
//         if (template == null) {
//             throw Error("device ID '" + devId
//                 + "' isn't from a supported group: '"
//                 + attrs.deviceGroup + "'" );
//         }
//         lib = template
//             .replace("{tcDir}",   tcDir)
//             .replace(/{devDir}/g, attrs.deviceDir);
//     }

//     /* create a GenLibs input argument */
//     var libGroup = {
//         name: "/ti/devices/driverlib",
//         vers: "1.0.0.0",
//         deps: [],
//         libs: [lib]
//     };

//     return (libGroup);
// }

/*
 *  ======== exports ========
 */
exports = {
    staticOnly   : true,
    displayName  : "DriverLib",

    /*
     * The below code has been commented out in order to disable driverlib
     * contributions to genlibs. MCPISWTOOLS-500 is adding genlibs support to
     * ex_gen and placing its generated cmd file BEFORE the kernel on the link
     * line. This will cause problems with agama devices as they need driverlib
     * to be linked AFTER the kernel. The solution, for now, is to have ex_gen
     * continue to get driverlib from board.js (placing it after the kernel)
     * and remove driverlib contributions to genlibs.
     */
    // templates    : {
    //     /* support library option generation for linker */
    //     "/ti/utils/build/GenLibs.cmd.xdt":
    //         {modName: "/ti/devices/DriverLib", getLibs: getLibs}
    // },

    /* enable end-user to override computed library path */
    moduleStatic : {
        config: config
    },

    /* DriverLib-specific exports */
    getAttrs: getAttrs
};
