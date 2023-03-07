/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
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
 *
 */
 
/*
 * ======== Boot.syscfg.js ========
 */

/* global exports, system */

/*
 * ======== moduleInstances ========
 * Create a Startup Reset Func for Boot_trimDevice()
 */
function moduleInstances(mod)
{
    if (!mod.trimDevice) return ([]);
    
    let startupInstance = new Array();
    
    startupInstance.push(
        {
            name: "trimDeviceFunc",
            displayName: "Boot trimDevice",
            moduleName: "ti/sysbios/runtime/Startup",
            hidden: true,
            readOnly: true,
            requiredArgs: {
                startupFxn: "Boot_trimDevice",
                functionType: "Reset Function",
                externRequired: false,
                $name: "Boot_trimDevice"
            }
        }
    );
    
    return (startupInstance);
}

exports = {
    staticOnly: true,
    displayName: "Boot",
    defaultInstanceName: "Boot",
    moduleStatic: {
        name: "moduleGlobal",
        moduleInstances: moduleInstances,
        config: [
            {
                name: "trimDevice",
                description: `Trim device flag; determines if
                SetupTrimDevice() is called at boot, default is true.`,
                longDescription: `
Set to false to disable the Boot module from trimming the device. The device
*must* be trimmed for proper operation. If the Boot module doesn't do this,
the application must explicitly call SetupTrimDevice().

                `,
                displayName: "Trim Device Flag",
                default: true
            },
            {
                name: "customerConfig",
                description: `Include a default customer configuration (CCFG)
                structure, default is false.`,
                longDescription: `
Set to true to link in the default CCFG data structure in driverlib.

                `,
                displayName: "Use default CCFG from driverlib",
                default: false
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/family/arm/cc26xx/Boot_config.h.xdt"
    },
    getCFiles: () => { return ["ti/sysbios/family/arm/cc26xx/Boot.c"] }
};

