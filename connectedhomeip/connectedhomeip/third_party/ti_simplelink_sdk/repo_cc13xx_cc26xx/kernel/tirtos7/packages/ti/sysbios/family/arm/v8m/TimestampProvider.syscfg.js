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
 * ======== TimestampProvider.syscfg.js ========
 */

/* global exports, system */

/*
 * ======== modules ========
 * Pull in the appropiate dependent modules
 */
function modules(mod)
{
    let modArray = [];

    modArray.push({
        name      : "Timer",
        moduleName: "ti/sysbios/family/arm/v8m/Timer"
    });

    return (modArray);
}

/*
 * ======== moduleInstances ========
 * Create a 'Timestamp_init' First Function if this
 * module is being used as the Timestamp.
 */
function moduleInstances(mod)
{

    let modInstances = new Array();

    modInstances.push(
        {
            name: "timeStampInit",
            displayName: "Timestamp_init()",
            moduleName: "ti/sysbios/runtime/Startup",
            hidden: true,
            readOnly: true,
            requiredArgs: {
                startupFxn: "ti_sysbios_family_arm_v8m_TimestampProvider_init",
                functionType: "First Function",
                externRequired: true,
                priority: 10, /* call after BIOS_init */
                $name: "ti_sysbios_family_arm_v8m_TimestampProvider_init"
            }
        }
    );

    return (modInstances);
}

exports = {
    staticOnly: true,
    displayName: "SysTick TimestampProvider CC26X4",
    moduleStatic: {
        name: "moduleGlobal",
        modules: modules,
        moduleInstances: moduleInstances
    },
    templates: {
        bios_config_h: "/ti/sysbios/family/arm/v8m/TimestampProvider_config.h.xdt"
    },
    noHeader: true,
    getCFiles: () => { return ["ti/sysbios/family/arm/v8m/TimestampProvider.c"] }
};

