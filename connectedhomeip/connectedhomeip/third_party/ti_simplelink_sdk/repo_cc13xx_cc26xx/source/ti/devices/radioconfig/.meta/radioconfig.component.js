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
 *
 */

/*
 *  ======== radioconfig.component.js ========
 */

"use strict";

// Common functionality
const Common = system.getScript("/ti/devices/radioconfig/radioconfig_common.js");

// Other dependencies
const DevInfo = Common.getScript("device_info.js");

let topModules;

const BasePath = Common.basePath;
const displayName = "RF Stacks";
const hasProp = Common.HAS_PROP;
const hasBle = Common.HAS_BLE;
const hasIeee = Common.HAS_IEEE_15_4;

if ((hasProp || hasBle || hasIeee) && DevInfo.isDeviceSupported()) {
    // Add PHY groups
    if (hasProp) {
        DevInfo.addPhyGroup(Common.PHY_PROP);
    }

    if (hasBle) {
        DevInfo.addPhyGroup(Common.PHY_BLE);
    }

    if (hasIeee) {
        DevInfo.addPhyGroup(Common.PHY_IEEE_15_4);
    }

    topModules = [
        {
            displayName: displayName,
            description: "Radio Configuration for SimpleLink devices",
            modules: [
                BasePath + "custom"
            ]
        },
        {
            displayName: "RF Design",
            description: "RF Design for SimpleLink devices",
            modules: [
                BasePath + "rfdesign"
            ]
        }

    ];
}

const templates = [
    {
        name: BasePath + "templates/rf_settings.c.xdt",
        outputPath: "ti_radio_config.c"
    },
    {
        name: BasePath + "templates/rf_settings.h.xdt",
        outputPath: "ti_radio_config.h"
    }
];

exports = {
    templates: templates,
    displayName: displayName,
    topModules: topModules
};
