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
 *  ======== advertisement_set.syscfg.js ========
 */

"use strict";
const config = [
    {
        name: "numOfAdvSet",
        default: 0,
        hidden: true
    },
    {
        name: "meshAdnPeri",
        default: false,
        hidden: true
    }
];
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

    let advParams_args = {
        name: "advParams"+inst.numOfAdvSet
    }
    let advData_args = {
        name: "advData"+inst.numOfAdvSet
    }
    let scanResData_args = {
        name: "scanResData"+inst.numOfAdvSet,
        GAP_ADTYPE_FLAGS: false,
        hideAdvFlags: true
    }

    if(inst.meshAdnPeri == true)
    {
        if(inst.numOfAdvSet == 1)
        {
            advParams_args.advType = "extended";
            advParams_args.primPhy = "GAP_ADV_PRIM_PHY_1_MBPS";
            advParams_args.secPhy  = "GAP_ADV_SEC_PHY_1_MBPS";
            advData_args.GAP_ADTYPE_FLAGS = true;
            advData_args.advertisingFlags = ["GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED","GAP_ADTYPE_FLAGS_GENERAL"];
            advData_args.GAP_ADTYPE_LOCAL_NAME_SHORT = true;
            advData_args.shortenedLocalName = "SP";
            advData_args.GAP_ADTYPE_16BIT_MORE = true;
            advData_args.numOfUUIDs16More = 1;
            advData_args.UUID016More = 0xFFF0;
        }
    }

        dependencyModule.push({
            name: "advParam" + inst.numOfAdvSet,
            displayName: "Advertisement Parameters " + inst.numOfAdvSet,
            moduleName: "/ti/ble5stack/broadcaster/advertisement_params",
            collapsed: true,
            args: advParams_args
        });

        dependencyModule.push({
            name: "advData" + inst.numOfAdvSet,
            displayName: "Advertisement Data " + inst.numOfAdvSet,
            moduleName: "/ti/ble5stack/broadcaster/advertisement_data",
            collapsed: true,
            args: advData_args
        });

        dependencyModule.push({
            name: "scanRes" + inst.numOfAdvSet,
            displayName: "Scan Response Data " + inst.numOfAdvSet,
            moduleName: "/ti/ble5stack/broadcaster/advertisement_data",
            collapsed: true,
            args: scanResData_args
        });

    return(dependencyModule);
}

/*
 *  ======== advSetModule ========
 *  Define the Advertisement Set module properties and methods
 */
const advSetModule = {
    displayName: "Advertisement Set",
    config: config,
    moduleInstances: moduleInstances
};

/*
 *  ======== exports ========
 *  Export the Advertisement Set module
 */
exports = advSetModule;
