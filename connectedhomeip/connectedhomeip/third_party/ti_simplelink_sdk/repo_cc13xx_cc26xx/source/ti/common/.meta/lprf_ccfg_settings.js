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
 *  ======== lp_common_ccfg_settings.js ========
 */

/*
 * TO USE THIS FILE:
 *    Your stack MUST export a ccfgSettings object.
 *
 *    In your .syscfg file, you can use the following code snippet to set the
 *    CCFG settings:
 *     var device = scripting.addModule("ti/devices/CCFG");
 *     const ccfgSettings = system.getScript("/ti/common/lprf_ccfg_settings.js").ccfgSettings;
 *     for(var setting in ccfgSettings)
 *     {
 *         device[setting] = ccfgSettings[setting];
 *     }
 */

"use strict";

// Get common LPRF utility functions
const Common = system.getScript("/ti/common/lprf_common.js");

let ccfgSettings = {};

// Settings for ti/devices/CCFG module
const boardSpecificCCFGSettings = {
    CC1312R1_LAUNCHXL_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 13,
        levelBootloaderBackdoor: "Active low"
    },
    CC1352R1_LAUNCHXL_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    CC1352P1_LAUNCHXL_CCFG_SETTINGS: {
        xoscCapArray: true,
        xoscCapArrayDelta: 0xC1,
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    CC1352P_2_LAUNCHXL_CCFG_SETTINGS: {
        xoscCapArray: true,
        xoscCapArrayDelta: 0xC1,
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    CC1352P_4_LAUNCHXL_CCFG_SETTINGS: {
        xoscCapArray: true,
        xoscCapArrayDelta: 0xC1,
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    CC26X2R1_LAUNCHXL_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 13,
        levelBootloaderBackdoor: "Active low"
    },
    LP_CC2652RB_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 13,
        levelBootloaderBackdoor: "Active low",
        srcClkLF: "LF RCOSC"
    },
    LP_CC2652RSIP_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    LP_CC2652PSIP_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    LP_CC1312R7_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 13,
        levelBootloaderBackdoor: "Active low"
    },
    LP_CC1352P7_1_CCFG_SETTINGS: {
        xoscCapArray: true,
        xoscCapArrayDelta: 0xC1,
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    LP_CC1352P7_4_CCFG_SETTINGS: {
        xoscCapArray: true,
        xoscCapArrayDelta: 0xC1,
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    LP_CC1311P3_CCFG_SETTINGS: {
        xoscCapArray: true,
        xoscCapArrayDelta: 0xC1,
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    LP_CC2652R7_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 13,
        levelBootloaderBackdoor: "Active low"
    },
    LP_CC2651R3_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 13,
        levelBootloaderBackdoor: "Active low"
    },
    LP_CC2651P3_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 13,
        levelBootloaderBackdoor: "Active low"
    },
    LP_CC1354R10_RGZ_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    LP_CC2674R10_FPGA_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 13,
        levelBootloaderBackdoor: "Active low"
    },
    LP_EM_CC1354P10_1_CCFG_SETTINGS: {
        xoscCapArray: true,
        xoscCapArrayDelta: 0xC1,
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    LP_EM_CC1354P10_6_CCFG_SETTINGS: {
        xoscCapArray: true,
        xoscCapArrayDelta: 0xC1,
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    LP_EM_CC1314R10_CCFG_SETTINGS: {
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 15,
        levelBootloaderBackdoor: "Active low"
    },
    LP_EM_CC2653P10_CCFG_SETTINGS: {
        xoscCapArray: true,
        xoscCapArrayDelta: 0xC1,
        enableBootloader: true,
        enableBootloaderBackdoor: true,
        dioBootloaderBackdoor: 13,
        levelBootloaderBackdoor: "Active low"
    },

};

// Get the LaunchPad specific CCFG Settings
if(system.deviceData.board)
{
    const boardName = Common.getDeviceOrLaunchPadName(false);
    ccfgSettings = Object.assign(ccfgSettings,
        boardSpecificCCFGSettings[boardName + "_CCFG_SETTINGS"]);
}

// Get the stack specific CCFG settings
let stack = "";
for(stack of Common.stacks)
{
    let stackPath = stack.path;
    if(system.modules[stackPath])
    {
        // Workaround for rf driver examples provided by EasyLink
        if(stackPath === "/ti/devices/radioconfig/custom")
        {
            stackPath = "/ti/easylink/easylink";
        }

        const stackCommon = system.getScript(stackPath + "_common.js");

        // Verify that the stack has ccfgSettings to provide before setting them
        if(stackCommon.ccfgSettings)
        {
            ccfgSettings = Object.assign(ccfgSettings,
                stackCommon.ccfgSettings);
        }
    }
}

exports = {
    ccfgSettings: ccfgSettings
};
