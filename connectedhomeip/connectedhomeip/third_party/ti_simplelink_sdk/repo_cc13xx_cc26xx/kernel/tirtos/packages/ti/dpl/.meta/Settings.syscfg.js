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
 * ======== Settings.syscfg.js ========
 * This module's main purpose is to provide a list of source files to
 * include in the BIOS 7.x's ti_sysbios_config.c file. The BIOS module
 * queries all modules in the system for a getCFiles function which provides
 * an array of files to include in the ti_sysbios_config.c file. This could
 * be generalized and move to ti.utils.build.GetCFiles or some such module.
 */

var dplFiles = [
    "ti/dpl/ClockP_tirtos.c",
    "ti/dpl/DebugP_tirtos.c",
    "ti/dpl/HwiP_tirtos.c",
    "ti/dpl/MutexP_tirtos.c",
    "ti/dpl/SemaphoreP_tirtos.c",
    "ti/dpl/SwiP_tirtos.c",
    "ti/dpl/SystemP_tirtos.c"
];

function getCFiles()
{
    if (system.deviceData.deviceId.match(/CC13.2|CC26.2/)) {
        dplFiles.push("ti/dpl/PowerCC26X2_tirtos.c");
    } else if (system.deviceData.deviceId.match(/CC32.*/)) {
        dplFiles.push("ti/dpl/PowerCC32XX_tirtos.c");
    } else /* default to cc13x2/cc26x2 */ {
        dplFiles.push("ti/dpl/PowerCC26X2_tirtos.c");
    }

    return (dplFiles);
}

exports = {
    staticOnly: true,
    displayName: "Settings",
    getCFiles: getCFiles
};
