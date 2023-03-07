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
 *  ======== ble_central.syscfg.js ========
 */

"use strict";

// Get central long descriptions
const Docs = system.getScript("/ti/ble5stack/central/ble_central_docs.js");

// Get common utility functions
const Common = system.getScript("/ti/ble5stack/ble_common.js");

const config = {
    name: "centralConfig",
    displayName: "Central Configuration",
    description: "Configure Central Role Settings",
    config: [
        {
            name: "hideCentralGroup",
            default: true,
            hidden: true
        },
        {
            name: "initiatingPHY",
            displayName: "Initiating PHY",
            default: "INIT_PHY_1M",
            hidden: true,
            longDescription: Docs.initiatingPHYLongDescription,
            options: [
                { displayName: "1M",    name: "INIT_PHY_1M"     },
                { displayName: "2M",    name: "INIT_PHY_2M"     },
                { displayName: "Coded", name: "INIT_PHY_CODED"  }
            ]
        },
        {
            name: "rssiPollingPeriod",
            displayName: "RSSI Polling Period (ms)",
            description: "How often to read the RSSI value (For example: read RSSI value every 3000 ms)",
            default: 3000,
            hidden: true,
            longDescription: Docs.rssiPollingPeriodLongDescription
        },
        {
            name: "connIntMin",
            displayName: "Connection Interval Min (ms)",
            default: 100,
            hidden: true,
            longDescription: Docs.connIntMinLongDescription
        },
        {
            name: "connIntMax",
            displayName: "Connection Interval Max (ms)",
            default: 100,
            hidden: true,
            longDescription: Docs.connIntMaxLongDescription
        }
    ]
};


/*
 * ======== validate ========
 * Validate this inst's configuration
 *
 * @param inst       - Central instance to be validated
 * @param validation - object to hold detected validation issues
 */
function validate(inst, validation)
{
    // Validate connection interval
    Common.validateConnInterval(inst,validation,inst.connIntMin,"connIntMin",inst.connIntMax,"connIntMax");

    if(inst.rssiPollingPeriod < 0 || inst.rssiPollingPeriod > (Math.pow(2,32) - 1))
    {
        validation.logError("RSSI Polling Period range is 0 to sizeof(uint32)", inst, "rssiPollingPeriod");
    }
}

/*
 *  ======== moduleInstances ========
 *  Determines what modules are added as non-static submodules
 *
 *  @param inst  - Module instance containing the config that changed
 *  @returns     - Array containing dependency modules
 */
function moduleInstances(inst)
{
    const dependencyModule = [];

    if(!inst.hideCentralGroup)
    {
        dependencyModule.push({
            name: "connUpdateParamsCentral",
            group: "centralConfig",
            displayName: "Connection Update Request Params",
            moduleName: "/ti/ble5stack/general/ble_conn_update_params",
            collapsed: true,
            args: {
                hideParamUpdateDelay: true
            }
        });
    }

    return(dependencyModule);
}

// Exports to the top level BLE module
exports = {
    config: config,
    validate: validate,
    moduleInstances: moduleInstances
};
