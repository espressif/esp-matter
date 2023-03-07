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
 * ======== HeapMem.syscfg.js ========
 */

/* global exports, system */

/*
 * ======== modules ========
 */
function modules(mod)
{
    let modArray = new Array();

    switch (mod.modGateType) {
        case "GateHwi":
            modArray.push({
                name      : "GateHwi",
                moduleName: "ti/sysbios/gates/GateHwi"
            });
            break;
        case "GateSwi":
            modArray.push({
                name      : "GateSwi",
                moduleName: "ti/sysbios/gates/GateSwi"
            });
            break;
        case "GateTask":
            modArray.push({
                name      : "GateTask",
                moduleName: "ti/sysbios/gates/GateTask"
            });
            break;
        case "GateMutex":
            modArray.push({
                name      : "GateMutex",
                moduleName: "ti/sysbios/gates/GateMutex"
            });
            break;
        case "GateMutexPri":
            modArray.push({
                name      : "GateMutexPri",
                moduleName: "ti/sysbios/gates/GateMutexPri"
            });
            break;
    }

    return (modArray);
}

exports = {
    displayName: "HeapMem",

    moduleStatic: {
        name: "moduleGlobal",
        modules: modules,
        config: [
            {
                name: "modGateType", 
                displayName: "Module Gate Type",
                default: "GateMutex",
                options: [
                    { name: "GateHwi", displayName: "Gate Hwi" },
                    { name: "GateSwi", displayName: "Gate Swi" },
                    { name: "GateTask", displayName: "Gate Task" },
                    { name: "GateMutex", displayName: "GateMutex" },
                    { name: "GateMutexPri", displayName: "GateMutexPri" }
                ]
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/heaps/HeapMem_config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt": "/ti/sysbios/heaps/HeapMem.rov.js"
    },
    getCFiles: () => { return ["ti/sysbios/heaps/HeapMem.c"] }
};
