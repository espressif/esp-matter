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
 *  ======== Tiva.xs ========
 *
 */

var peripheralsTable = {};

peripheralsTable["2TIMERS"] = [
    {
        factory: "ti.catalog.arm.peripherals.timers.Timer",
        params: {name: "Timer0", baseAddr: 0x40030000, intNum: 35}
    },
    {
        factory: "ti.catalog.arm.peripherals.timers.Timer",
        params: {name: "Timer1", baseAddr: 0x40031000, intNum: 37}
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
    params: {name: "Timer2", baseAddr: 0x40032000, intNum: 39}
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
    params: {name: "Timer3", baseAddr: 0x40033000, intNum: 51}
};

/*
 *  ======== instance$meta$init ========
 */
function instance$meta$init(revision)
{
    this.$private.realDevice = revision;

    //var Boot = xdc.useModule('ti.catalog.arm.cortexm3.lm3init.Boot');

    var tableEntry;
    /*
     * Define which actual devices correspond to the above defined
     * descriptor tables. Also, additional configurations important
     * to SYS/BIOS are made here.
     */
    if (this.$private.realDevice.match(/^LM3S\d\d76/)
        || this.$private.realDevice.match(/^LM3S\d110/)
        || this.$private.realDevice.match(/^LM3S\d\d52/)
        || this.$private.realDevice.match(/^LM3S\d4\d\d/)
        || this.$private.realDevice.match(/^LM3S1\D1\d/)
        || this.$private.realDevice.match(/^LM3S3\w26/)
        || this.$private.realDevice.match(/^LM3S(1|2)\d39/)) {
        tableEntry = "3TIMERS";
    }
    else if (this.$private.realDevice.match(/^LM3S5\D3(1|6)/)
        && !this.$private.realDevice.match(/^LM3S5\Dr\w/)) {
        tableEntry = "3TIMERS";
    }
    else if (this.$private.realDevice.match(/^LM3S\d\d\d$/)
        && !this.$private.realDevice.match(/^LM3S(1|3)0(1|2)$/)) {
        tableEntry = "3TIMERS";
    }
    else if (this.$private.realDevice.match(/^LM3S(1|3)0(1|2)$/)
        || this.$private.realDevice == "LM3S2016") {
        tableEntry = "2TIMERS";
    }
    else if (this.$private.realDevice == "LM3S1620"
        || this.$private.realDevice == "LM3S1751"
        || this.$private.realDevice == "LM3S1850"
        || this.$private.realDevice == "LM3S1937"
        || this.$private.realDevice == "LM3S2730"
        || this.$private.realDevice == "LM3S3634"
        || this.$private.realDevice == "LM3S5632"
        || this.$private.realDevice == "LM3S5662"
        || this.$private.realDevice == "LM3S5732"
        || this.$private.realDevice == "LM3S5737"
        || this.$private.realDevice == "LM3S5747"
        || this.$private.realDevice == "LM3S5762"
        || this.$private.realDevice == "LM3S6100"
        || this.$private.realDevice == "LM3S6730") {
        tableEntry = "3TIMERS";
    }
    else {
        tableEntry = "4TIMERS";
    }

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
 *  ======== Tiva.getMemoryMap ========
 *  Returns the memory map as seen by a program running on this device.
 */
function getMemoryMap(regs)
{
    return (null);
}
