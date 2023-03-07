/*
 * Copyright (c) 2016, Texas Instruments Incorporated
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
 */

/*
 *  ======== CC13xx.xs ========
 *
 */

var peripheralsTable = {};

peripheralsTable["2TIMERS"] = [
    {
        factory: "ti.catalog.arm.peripherals.timers.Timer",
        params: {name: "Timer0", baseAddr: 0x40010000, intNum: 31}
    },
    {
        factory: "ti.catalog.arm.peripherals.timers.Timer",
        params: {name: "Timer1", baseAddr: 0x40011000, intNum: 33}
    }
];

peripheralsTable["3TIMERS"] = [];
for (var i = 0; i < peripheralsTable["2TIMERS"].length; i++) {
    peripheralsTable["3TIMERS"][i] = {};
    for (var prop in peripheralsTable["2TIMERS"][i]) {
        peripheralsTable["3TIMERS"][i][prop]
            = peripheralsTable["2TIMERS"][i][prop];
    }
}
peripheralsTable["3TIMERS"][i] = {
    factory: "ti.catalog.arm.peripherals.timers.Timer",
    params: {name: "Timer2", baseAddr: 0x40012000, intNum: 35}
};

peripheralsTable["4TIMERS"] = [];
for (var i = 0; i < peripheralsTable["3TIMERS"].length; i++) {
    peripheralsTable["4TIMERS"][i] = {};
    for (var prop in peripheralsTable["3TIMERS"][i]) {
        peripheralsTable["4TIMERS"][i][prop]
            = peripheralsTable["3TIMERS"][i][prop];
    }
}
peripheralsTable["4TIMERS"][i] = {
    factory: "ti.catalog.arm.peripherals.timers.Timer",
    params: {name: "Timer3", baseAddr: 0x40013000, intNum: 37}
};

/*
 *  ======== instance$meta$init ========
 */
function instance$meta$init(revision)
{
    this.$private.realDevice = revision;

    var tableEntry = "4TIMERS";

    if (peripheralsTable[tableEntry] != null) {
        for (var i = 0; i < peripheralsTable[tableEntry].length; i++) {
            var entry = peripheralsTable[tableEntry][i];
            var modName = entry.factory;
            var mod = xdc.useModule(modName);
            var params = entry.params;

            /* construct the create arguments */
            var args = [];
            args.push(params);

            /* construct the peripheral */
            this.peripherals[params.name] = mod.create.$apply(mod, args);
        }
    }
}

/*
 *  ======== CC13xx.getMemoryMap ========
 *  Returns the memory map as seen by a program running on this device.
 */
function getMemoryMap(regs)
{
    return (null);
}
