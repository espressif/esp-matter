/*
 * Copyright (c) 2020-2021 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== Settings.js ========
 */

let deviceSettingsTable = {
    "cc13x2_cc26x2" : {
        bootModule : "/ti/sysbios/family/arm/cc26xx/Boot",
        hwiModule : "/ti/sysbios/family/arm/m3/Hwi",
        timerModule : "/ti/sysbios/family/arm/m3/Timer",
        defaultClockTickPeriod : 1000,
        defaultClockTickMode : "Clock_TickMode_DYNAMIC",
        clockSupportModule : "/ti/sysbios/family/arm/cc26xx/ClockSupport",
        taskSupportModule : "/ti/sysbios/family/arm/m3/TaskSupport",
        secondsModule : "/ti/sysbios/family/arm/cc26xx/Seconds",
        defaultTaskStackSize : 1024,
        availableTimestampProviderModules : [
            { name: "RTC TimestampProvider", displayName: "RTC TimestampProvider" },
            { name: "DWT TimestampProvider", displayName: "DWT TimestampProvider" },
            { name: "LM4 TimestampProvider", displayName: "LM4 TimestampProvider" },
            { name: "SysTick TimestampProvider", displayName: "SysTick TimestampProvider" }
        ],
        defaultTimestampProvider : "RTC TimestampProvider",
        defaultNumInterrupts : 16 + 38,
        defaultResetVectorAddress : 0x00000000,
        defaultVectorTableAddress : 0x20000000,
        defaultCpuFrequency : 48000000
    },
    "cc13x4_cc26x4" : {
        bootModule : "/ti/sysbios/family/arm/cc26xx/Boot",
        hwiModule : "/ti/sysbios/family/arm/v8m/Hwi",
        timerModule : "/ti/sysbios/family/arm/v8m/Timer",
        defaultClockTickPeriod : 1000,
        defaultClockTickMode : "Clock_TickMode_DYNAMIC",
        clockSupportModule : "/ti/sysbios/family/arm/v8m/cc26x4/ClockSupport",
        taskSupportModule : "/ti/sysbios/family/arm/v8m/TaskSupport",
        secondsModule : "/ti/sysbios/family/arm/v8m/cc26x4/Seconds",
        defaultTaskStackSize : 1024,
        availableTimestampProviderModules : [
            { name: "RTC TimestampProvider CC26X4", displayName: "RTC TimestampProvider CC26X4" },
            { name: "DWT TimestampProvider", displayName: "DWT TimestampProvider" },
            { name: "LM4 TimestampProvider", displayName: "LM4 TimestampProvider" },
            { name: "SysTick TimestampProvider CC26X4", displayName: "SysTick TimestampProvider CC26X4" }
        ],
        defaultTimestampProvider : "RTC TimestampProvider CC26X4",
        defaultNumInterrupts : 16 + 47,
        defaultResetVectorAddress : 0x00000000,
        defaultVectorTableAddress : 0x20000000,
        defaultCpuFrequency : 48000000
    },
    "cc32xx" : {
        bootModule : null,
        hwiModule : "/ti/sysbios/family/arm/m3/Hwi",
        timerModule : "/ti/sysbios/family/arm/lm4/Timer",
        defaultClockTickPeriod : 1000,
        defaultClockTickMode : "Clock_TickMode_PERIODIC",
        clockSupportModule : "/ti/sysbios/family/arm/m3/ClockSupport",
        taskSupportModule : "/ti/sysbios/family/arm/m3/TaskSupport",
        secondsModule : "/ti/sysbios/family/arm/cc32xx/Seconds",
        defaultTaskStackSize : 1024,
        availableTimestampProviderModules : [
            { name: "SysTick TimestampProvider", displayName: "SysTick TimestampProvider" },
            { name: "DWT TimestampProvider", displayName: "DWT TimestampProvider" },
            { name: "LM4 TimestampProvider", displayName: "LM4 TimestampProvider" }
        ],
        defaultTimestampProvider : "SysTick TimestampProvider",
        defaultNumInterrupts : 16 + 179,
        defaultResetVectorAddress : 0x20004000,
        defaultVectorTableAddress : 0x20000000,
        defaultCpuFrequency : 80000000
    },
    "cc23x0" : {
        bootModule : "/ti/sysbios/family/arm/cc26xx/Boot",
        hwiModule : "/ti/sysbios/family/arm/v6m/Hwi",
        timerModule : "/ti/sysbios/family/arm/v6m/Timer",
        defaultClockTickPeriod : 1000,
        defaultClockTickMode : "Clock_TickMode_PERIODIC",
        clockSupportModule : "/ti/sysbios/family/arm/cc26xx/ClockSupport",
        taskSupportModule : "/ti/sysbios/family/arm/v6m/TaskSupport",
        secondsModule : "/ti/sysbios/family/arm/cc26xx/Seconds",
        defaultTaskStackSize : 1024,
        availableTimestampProviderModules : [
            { name: "RTC TimestampProvider", displayName: "RTC TimestampProvider" },
            { name: "DWT TimestampProvider", displayName: "DWT TimestampProvider" },
            { name: "LM4 TimestampProvider", displayName: "LM4 TimestampProvider" },
            { name: "SysTick TimestampProvider M0", displayName: "SysTick TimestampProvider M0" }
        ],
        defaultTimestampProvider : "RTC TimestampProvider",
        defaultNumInterrupts : 16 + 38,
        defaultResetVectorAddress : 0x00000000,
        defaultVectorTableAddress : 0x20000000,
        defaultCpuFrequency : 48000000
    }
};

/* Add the CC32XXSF derivative */
deviceSettingsTable["cc32xxsf"] = Object.assign({}, deviceSettingsTable.cc32xx);
deviceSettingsTable["cc32xxsf"].defaultResetVectorAddress = 0x01000800;

/* Add the CC13x1/CC26x1 derivative */
deviceSettingsTable["cc13x1_cc26x1"] = Object.assign({}, deviceSettingsTable.cc13x2_cc26x2);

let deviceId = system.deviceData.deviceId;

let deviceSettings;

if (deviceId.match(/CC23.0/)) {
    deviceSettings = deviceSettingsTable.cc23x0;
} else if (deviceId.match(/CC13.1.*|CC26.1.*/)) {
    /* CC13X1/CC26X1 */
    deviceSettings = deviceSettingsTable.cc13x1_cc26x1;
} else if (deviceId.match(/CC13.2.*|CC26.2.*/)) {
    /* CC13X2/CC26X2 */
    deviceSettings = deviceSettingsTable.cc13x2_cc26x2;
} else if (deviceId.match(/CC13.4.*|CC26.4.*|CC2653.*/)) {
    /* CC13X4/CC26X4 */
    deviceSettings = deviceSettingsTable.cc13x4_cc26x4;
} else if (deviceId.match(/CC32..SF.*/)) {
    /* CC32XXSF */
    deviceSettings = deviceSettingsTable.cc32xxsf;
} else if (deviceId.match(/CC32.*/)) {
    /* CC32XX */
    deviceSettings = deviceSettingsTable.cc32xx;
} else if (deviceId.match(/MSP432E.*/)) {
    /* MSP432E */
    deviceSettings = deviceSettingsTable.msp432e;
} else {
    /* default to CC13X2/CC26X2 */
    deviceSettings = deviceSettingsTable.cc13x2_cc26x2;
}

exports = {
    /* BIOS module settings */
    bootModule : deviceSettings.bootModule,
    hwiModule : deviceSettings.hwiModule,
    timerModule : deviceSettings.timerModule,
    defaultCpuFrequency : deviceSettings.defaultCpuFrequency,

    /* Clock module settings */
    defaultClockTickPeriod : deviceSettings.defaultClockTickPeriod,
    defaultClockTickMode : deviceSettings.defaultClockTickMode,
    clockSupportModule : deviceSettings.clockSupportModule,

    /* Task module settings */
    taskSupportModule : deviceSettings.taskSupportModule,
    defaultTaskStackSize : deviceSettings.defaultTaskStackSize,

    secondsModule : deviceSettings.secondsModule,

    /* Hwi module settings */
    defaultNumInterrupts : deviceSettings.defaultNumInterrupts,
    defaultResetVectorAddress : deviceSettings.defaultResetVectorAddress,
    defaultVectorTableAddress : deviceSettings.defaultVectorTableAddress,

    /* Timestamp module settings */
    availableTimestampProviderModules : deviceSettings.availableTimestampProviderModules,
    defaultTimestampProvider : deviceSettings.defaultTimestampProvider
};
