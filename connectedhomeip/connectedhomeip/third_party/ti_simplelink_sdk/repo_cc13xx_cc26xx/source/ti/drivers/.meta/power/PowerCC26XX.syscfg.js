/*
 * Copyright (c) 2018-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== PowerCC26XX.syscfg.js ========
 */

"use strict";

/* get Common /ti/drivers utility functions */
let Common   = system.getScript("/ti/drivers/Common.js");
let logError = Common.logError;
let isCName  = Common.isCName;
let logWarning = Common.logWarning;

/* Array of power configurables for this device family */
let config = [
    {
        name        : "enablePolicy",
        displayName : "Enable Policy",
        description : "Enable the power policy to run when the CPU is idle.",
        longDescription:`
If enabled, the policy function will be invoked once for each pass of the
idle loop.

In addition to this static setting, the Power Policy can be dynamically
enabled and disabled at runtime, via the Power_enablePolicy() and
Power_disablePolicy() APIs, respectively.
`,
        onChange    : onChangeEnablePolicy,
        default     : true
    },

    /* Policy function handling */
    {
        name        : "policyFunction",
        displayName : "Policy Function",
        description : "Power policy function called from the idle loop.",
        longDescription:`
When enabled, this function is invoked in the idle loop, to opportunistically
select and activate sleep states.

Two reference policies are provided:

* __PowerCC26XX_standbyPolicy__
* __PowerCC26XX_doWFI__

In addition to this static selection, the Power Policy can be
dynamically changed at runtime, via the Power_setPolicy() API.
`,
        default     : "PowerCC26XX_standbyPolicy",
        onChange    : onChangePolicyFxn,
        options     :
        [
            {
                name: "PowerCC26XX_standbyPolicy",
                description: "An agressive policy that checks constraints and"
                  + " time until next scheduled work, and optionally chooses"
                  + " STANDBY, IDLE (power down), or WFI, in that"
                  + " order of preference."
            },
            {
                name: "PowerCC26XX_doWFI",
                description: "A simple policy that will invoke CPU wait for"
                    + " interrupt (WFI)."
            },
            {
                name: "Custom",
                description: "Custom policies can be written and specified"
                    + " via configuration of a Custom Policy Function."
            }
        ]
    },

    {
        name        : "policyCustomFunction",
        displayName : "Custom Policy Function",
        description : "User-provided Custom Policy Function.",
        default     : "customPolicyFxn",
        hidden      : true
    },

    {
        name        : "policyInitFunction",
        displayName : "Policy Init Function",
        default     : "Not Required",
        onChange    : onChangePolicyInitFxn,
        description : "The initialization function for the Power policy.",
        options     :
        [
            {
                name: "Not Required",
                description: "The selected Policy Function does not require an"
                  + " Init function."
            },
            {
                name: "Custom",
                description: "A custom initialization function can be written"
                  + " and specified via configuration of a Custom Policy Init"
                  + " Function."
            }
        ]
    },

    {
        name        : "policyInitCustomFunction",
        displayName : "Custom Policy Init Function",
        description : "User-provided Custom Policy Init Function. " +
                      "Usage not typical.",
        default     : "customPolicyInitFxn",
        hidden      : true
    },

    {
        name        : "calibrateFunction",
        displayName : "RCOSC Calibration Function",
        description : "Function to be used for RC Oscillator (RCOSC) calibration.",
        longDescription:`
The High Frequency RC Oscillator (RCOSC_HF) and Low Frequency RC Oscillator
 (RCOSC_LF), are typically re-calibrated on each device wakeup
 from STANDBY.  This calibration enables high accuracy RCOSC operation, and
 faster High Frequency crystal oscillator (XOSC_HF) startups.  The name of
 the calibration function to be used is specified with this configuration
 parameter.
`,
        default     : "PowerCC26XX_calibrate",
        options     :
        [
            {
              name: "PowerCC26XX_calibrate",
              description : "The typical function used for performing"
                + " RCOSC calibration."
            },
            {
              name: "PowerCC26XX_noCalibrate",
              description : "No calibration. This function can be"
                + " used to disable RCOSC calibration, and remove the"
                + " corresponding code and data from the program image."
            }
        ]
    },

    {
        name        : "calibrateRCOSC_LF",
        displayName : "Calibrate RCOSC_LF",
        description : "Enable calibration of RCOSC_LF.",
        default     : true
    },

    {
        name        : "calibrateRCOSC_HF",
        displayName : "Calibrate RCOSC_HF",
        description : "Enable calibration of RCOSC_HF.",
        default     : true
    },

    {
        name        : "vddrRechargeMargin",
        displayName : "VDDR Recharge Margin",
        description : "Margin to adjust initial VDDR recharge period (in"
          + " units of SCLK_LF periods).",
        longDescription:`
While in STANDBY the VDDR rail will be adaptively recharged to maintain the
 STANDBY state of the device.  Before the next entry to STANDBY, the initial
 recharge period can be reduced (with this margin) from the maximum recharge
 period used during the previous STANDBY interval.  This margin may be useful
 in high temperature environments, but is typically left as zero, and no
 margin is used.
`,
        default     : 0
    },

    {
        name        : "enableTCXOFunction",
        displayName : "Enable TCXO Function",
        description : "User-provided function for enable/disable of TCXO.",
        longDescription:`
When a connected TCXO xtal is supported on the board and CCFG is configured
 to use TCXO as the high frequency oscillator this function must be able to
 both power up and down the TCXO. CCFG configures the startup time of the
 TCXO and also the TCXO type.
`,
        placeholder : "Enter a function name to enable",
        default     : ""
    }
];

/*
 *  ======== devSpecific ========
 *  Device-specific extensions to be added to base Power configuration
 */
let devSpecific = {
    getClockFrequencies : getClockFrequencies,
    moduleStatic        : {
        config   : config,
        validate : validate,
        modules: Common.autoForceModules(["Board", "ti/devices/CCFG"])
    },
    templates           : {
        boardc      : "/ti/drivers/power/PowerCC26XX.Board.c.xdt",
        board_initc : "/ti/drivers/power/PowerCC26XX.Board_init.c.xdt"
    }
};

/*
 *  ======== onChangeEnablePolicy ========
 *  onChange callback function for the enablePolicy config
 */
function onChangeEnablePolicy(inst, ui)
{
    let subState = (!inst.enablePolicy);
    ui.policyInitFunction.hidden  = subState;
    ui.policyFunction.hidden = subState;

    onChangePolicyInitFxn(inst,ui);
    onChangePolicyFxn(inst,ui);
}

/*
 *  ======== onChangePolicyInitFxn ========
 *  onChange callback function for the policyInitFunction config
 */
function onChangePolicyInitFxn(inst, ui)
{
    let subState = (inst.policyInitFunction !== 'Custom');
    ui.policyInitCustomFunction.hidden = subState;
}

/*
 *  ======== onChangePolicyFxn ========
 *  onChange callback function for the policyFunction config
 */
function onChangePolicyFxn(inst, ui)
{
    let subState = (inst.policyFunction !== 'Custom');
    ui.policyCustomFunction.hidden = subState;
}

/*
 *  ======== getClockFrequencies ========
 *  Return the value of the CC26XX main CPU clock frequency
 */
function getClockFrequencies(clock)
{
    return [ 48000000 ];
}

/*
 *  ======== isExtTcxoSelected ========
 *  Check the CCFG module to see if an External TCXO has been
 *  selected.
 */
function isExtTcxoSelected()
{
    const CCFG = system.modules["/ti/devices/CCFG"];
    if (CCFG != undefined && CCFG.$static.srcClkHF === "External 48Mhz TCXO") {
        return (true);
    }
    return (false);
}

/*
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  param inst - Power instance to be validated
 *  param vo   - Issue reporting object
 *
 */
function validate(inst, vo)
{
    if (inst.enablePolicy) {
        if (inst.policyInitFunction === 'Custom') {
            if (!isCName(inst.policyInitCustomFunction)) {
                logError(vo, inst, "policyInitCustomFunction",
                    "Not a valid C identifier");
            }
            if (inst.policyInitCustomFunction === '') {
                logError(vo, inst, "policyInitCustomFunction",
                    "Must contain a valid function name if the " +
                    "Policy Init Function field == 'Custom'");
            }
        }
        if (inst.policyFunction === 'Custom') {
            if (!isCName(inst.policyCustomFunction)) {
                logError(vo, inst, "policyCustomFunction",
                    "is not a valid C identifier");
            }
            if (inst.policyCustomFunction === '') {
                logError(vo, inst, "policyCustomFunction",
                    "Must contain a valid function name if the " +
                    "Policy Function field == 'Custom'");
            }
        }
    }
    if (!isCName(inst.enableTCXOFunction)) {
        logError(vo, inst, "enableTCXOFunction",
                 "Must contain a valid C function name or NULL");
    }
    if ((isExtTcxoSelected() === true) && (inst.enableTCXOFunction === '')){
        logWarning(vo, inst, "enableTCXOFunction",
                 "Must contain a valid C function name because an External " +
                 "TCXO has been chosen as HF Clock Source in the Device " +
                 "Configuration module");
    }
}

/*
 *  ======== extend ========
 *  Extends a base exports object to include any device specifics
 *
 *  This function is invoked by the generic Power module to
 *  allow us to augment/override as needed for the CC26XX
 */
function extend(base)
{
    /* display which driver implementation can be used */
    devSpecific = Common.addImplementationConfig(devSpecific, "Power", null,
        [{name: "PowerCC26X2"}], null);

    return (Object.assign({}, base, devSpecific));
}

/*
 *  ======== exports ========
 *  Export device-specific extensions to base exports
 */
exports = {
    /* required function, called by base ADCBuf module */
    extend: extend,
    getClockFrequencies: getClockFrequencies
};
