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
 *
 */

/*
 *  ======== dmm.component.js ========
 */

"use strict";

const categoryName = "Multi-Protocol";
const displayName = "DMM";
let topModules = [];
let templates = [];

// Export module for only supported devices
if(system.deviceData.deviceId.match(/CC26.2|CC13.2(R|P)(1|7)|CC2652(R|P)7/))
{
    topModules = [
        {
            displayName: categoryName,
            description: categoryName,
            modules: ["/ti/dmm/dmm"]
        }
    ];

    templates = [
        {
            name: "/ti/dmm/templates/ti_dmm_application_policy.c.xdt",
            outputPath: "ti_dmm_application_policy.c"
        },
        {
            name: "/ti/dmm/templates/ti_dmm_application_policy.h.xdt",
            outputPath: "ti_dmm_application_policy.h"
        },
        {
            name: "/ti/dmm/templates/ti_dmm_custom_activities.h.xdt",
            outputPath: "ti_dmm_custom_activities.h"
        }
    ];
}

exports = {
    displayName: displayName,
    topModules: topModules,
    templates: templates
};
