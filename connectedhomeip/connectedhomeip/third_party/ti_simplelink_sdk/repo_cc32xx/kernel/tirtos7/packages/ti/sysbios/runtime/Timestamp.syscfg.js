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
 * ======== Timestamp.syscfg.js ========
 */

/* get device specific Settings */
let Settings = system.getScript("/ti/sysbios/family/Settings.js");

/*
 * ======== modules ========
 * Pull in the appropiate dependent modules
 */
function modules(mod)
{
    let modArray = [];

    switch (mod.timestampProvider) {
        case "DWT TimestampProvider":
            modArray.push({
                name      : "DWT TimestampProvider",
                moduleName: "ti/sysbios/family/arm/vxm/TimestampProvider"
            });
            break;
        case "RTC TimestampProvider":
            modArray.push({
                name      : "RTC TimestampProvider",
                moduleName: "ti/sysbios/family/arm/cc26xx/TimestampProvider"
            });
            break;
        case "RTC TimestampProvider CC26X4":
            modArray.push({
                name      : "RTC TimestampProvider CC26X4",
                moduleName: "ti/sysbios/family/arm/v8m/cc26x4/TimestampProvider"
            });
            break;
        case "LM4 TimestampProvider":
            modArray.push({
                name      : "LM4 TimestampProvider",
                moduleName: "ti/sysbios/family/arm/lm4/TimestampProvider"
            });
            break;
        case "SysTick TimestampProvider":
            modArray.push({
                name      : "SysTick TimestampProvider",
                moduleName: "ti/sysbios/family/arm/m3/TimestampProvider"
            });
            break;
    }

    return (modArray);
}

/* global exports, system */

exports = {
    staticOnly: true,
    displayName: "Timestamp",
    moduleStatic: {
        name: "moduleGlobal",
        modules: modules,
        config: [
            {
                name: "timestampProvider",
                displayName: "Timestamp Provider",
                default: Settings.defaultTimestampProvider,
                options: Settings.availableTimestampProviderModules
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/runtime/Timestamp_config.h.xdt"
    },
    getCFiles: () => { return ["ti/sysbios/runtime/Timestamp.c"] }
};
