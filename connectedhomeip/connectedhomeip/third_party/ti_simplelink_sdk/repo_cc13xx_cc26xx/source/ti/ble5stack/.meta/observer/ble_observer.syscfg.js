/*
 * Copyright (c) 2018 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ble_observer.syscfg.js ========
 */

"use strict";

// Get observer long descriptions
const Docs = system.getScript("/ti/ble5stack/observer/ble_observer_docs.js");

// Get common utility functions
const Common = system.getScript("/ti/ble5stack/ble_common.js");

const config = {
    name: "observerConfig",
    displayName: "Observer Configuration",
    description: "Configure Observer Role Settings",
    config: [
        {
            name: "hideObserverGroup",
            default: true,
            hidden: true
        },
        {
            name: "scanPHY",
            displayName: "Scanning PHY",
            default: "SCAN_PRIM_PHY_1M",
            description: "The scanning PHY is the primary PHY",
            hidden: true,
            longDescription: Docs.scanPHYLongDescription,
            options: [
                {
                    displayName: "1M",
                    name: "SCAN_PRIM_PHY_1M"
                },
                {
                    displayName: "Coded",
                    name: "SCAN_PRIM_PHY_CODED"
                }
            ]
        },
        {
            name: "scanType",
            displayName: "Scan Type",
            default: "SCAN_TYPE_ACTIVE",
            hidden: true,
            longDescription: Docs.scanTypeLongDescription,
            options: [
                {
                    displayName: "Active",
                    name: "SCAN_TYPE_ACTIVE",
                    description: "Active Scanning. Scan request PDUs may be sent."
                },
                {
                    displayName: "Passive",
                    name: "SCAN_TYPE_PASSIVE",
                    description: "Passive Scanning. No scan request PDUs shall be sent."
                }
            ]
        },
        {
            name: "scanInt",
            displayName: "Scan Interval (ms)",
            description: "Time interval from when the Controller started its last scan until it "
                         + "begins the subsequent scan on the primary advertising channel.",
            default: 500,
            hidden: true,
            longDescription: Docs.scanIntLongDescription
        },
        {
            name: "scanWin",
            displayName: "Scan Window (ms)",
            description: "Duration of the scan on the primary advertising channel.",
            default: 500,
            hidden: true,
            longDescription: Docs.scanWinLongDescription
        },
        {
            name: "scanDuration",
            displayName: "Scan Duration (ms)",
            description: "Scan duration",
            default: 1000,
            hidden: true,
            longDescription: Docs.scanDurationLongDescription
        },
        {
            name: "advRptFields",
            displayName: "Advertise Report Fields",
            description: "Advertising report fields to keep in the list",
            default: ["SCAN_ADVRPT_FLD_ADDRTYPE", "SCAN_ADVRPT_FLD_ADDRESS"],
            hidden: true,
            longDescription: Docs.advRptFieldsLongDescription,
            options: [
                {
                    displayName: "Event Type",
                    name: "SCAN_ADVRPT_FLD_EVENTTYPE"
                },
                {
                    displayName: "Address Type",
                    name: "SCAN_ADVRPT_FLD_ADDRTYPE"
                },
                {
                    displayName: "Address",
                    name: "SCAN_ADVRPT_FLD_ADDRESS"
                },
                {
                    displayName: "Primary PHY",
                    name: "SCAN_ADVRPT_FLD_PRIMPHY"
                },
                {
                    displayName: "Secondery PHY",
                    name: "SCAN_ADVRPT_FLD_SECPHY"
                },
                {
                    displayName: "Advertise Set Id",
                    name: "SCAN_ADVRPT_FLD_ADVSID"
                },
                {
                    displayName: "TX Power",
                    name: "SCAN_ADVRPT_FLD_TXPOWER"
                },
                {
                    displayName: "RSSI",
                    name: "SCAN_ADVRPT_FLD_RSSI"
                },
                {
                    displayName: "dirAddrType",
                    name: "SCAN_ADVRPT_FLD_DIRADDRTYPE"
                },
                {
                    displayName: "dirAddress",
                    name: "SCAN_ADVRPT_FLD_DIRADDRESS"
                },
                {
                    displayName: "prdAdvInterval",
                    name: "SCAN_ADVRPT_FLD_PRDADVINTERVAL"
                },
                {
                    displayName: "Data Length",
                    name: "SCAN_ADVRPT_FLD_DATALEN"
                }
            ]
        },
        {
            name: "dupFilter",
            displayName: "Duplicate Filter",
            default: "SCAN_FLT_DUP_ENABLE",
            hidden: true,
            options: [
                { displayName: "Disabled", name: "SCAN_FLT_DUP_DISABLE" },
                { displayName: "Enabled", name: "SCAN_FLT_DUP_ENABLE"  },
                { displayName: "Enabled, reset for each scan period", name: "SCAN_FLT_DUP_RESET" }
            ]
        },
        {
            name: "disDevBySerUuid",
            displayName: "Discover Devices By Service UUID",
            description: "Check in order to discover only peripheral devices "
                       + "with a desired service UUID",
            longDescription: Docs.disDevBySerUuidLongDescription,
            default: false,
            hidden: true
        }
    ]
};


/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - Observer instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    if(inst.scanInt < Common.connParamsRanges.scanIntMinValue ||
        inst.scanInt > Common.connParamsRanges.scanIntMaxValue)
    {
        validation.logError("Scan Interval range is " + Common.connParamsRanges.scanIntMinValue 
                            + " ms to " + Common.connParamsRanges.scanIntMaxValue + " ms", inst, "scanInt");
    }
    if(inst.scanWin < Common.connParamsRanges.scanWinMinValue ||
        inst.scanWin > Common.connParamsRanges.scanWinMaxValue)
    {
        validation.logError("Scan Window range is " + Common.connParamsRanges.scanWinMinValue +
                             " ms to " + Common.connParamsRanges.scanWinMaxValue + " ms", inst, "scanWin");
    }
    if((inst.scanDuration < Common.connParamsRanges.scanDurationMinValue ||
        inst.scanDuration > Common.connParamsRanges.scanDurationMaxValue) && inst.scanDuration != 0)
    {
        validation.logError("Scan Duration can be set to 0 or a value in the range of "
                            + Common.connParamsRanges.scanDurationMinValue + " ms to "
                            + Common.connParamsRanges.scanDurationMaxValue + " ms", inst, "scanDuration");
    }

    if(inst.scanWin > inst.scanInt)
    {
        validation.logError("Scan Window shall be less than or equal to Scan Interval", inst, "scanWin");
        validation.logError("Scan Interval shall be greater than or equal to Scan Window", inst, "scanInt");
    }

    if(inst.scanInt >= inst.scanDuration)
    {
        validation.logError("Scan Duration shall be greater than Scan Interval", inst, ["scanDuration","scanInt"]);
    }
}

// Exports to the top level BLE module
exports = {
    config: config,
    validate: validate
};
