/*
 * Copyright (c) 2014-2020, Texas Instruments Incorporated
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
 *  ======== Settings.xs ========
 *
 */

var settings = {
    device: {
    }
};

var catalogName;
var deviceTable = {
    "ti.catalog.arm": {
        "TMS320DA830": {
            hwiDelegate : "ti.sysbios.family.arm.da830.Hwi",
            timerDelegate : "ti.sysbios.timers.timer64.Timer",
            clockTimerDelegate : "ti.sysbios.timers.timer64.Timer",
            timerSupportDelegate : "ti.sysbios.family.arm.da830.TimerSupport",
            timestampDelegate : "ti.sysbios.timers.timer64.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            bootModule : null,
            cacheDelegate : "ti.sysbios.family.arm.arm9.Cache",
            coreDelegate : null,
            powerDelegate : null,
            secondsDelegate : null,
            mmuModule : "ti.sysbios.family.arm.arm9.Mmu",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.Arm9", "ti.targets.arm.Arm9t",
                        "ti.targets.arm.elf.Arm9", "ti.targets.arm.elf.Arm9t" ]
        },
        "CortexM3": {
            hwiDelegate : "ti.sysbios.family.arm.m3.Hwi",
            timerDelegate : "ti.sysbios.family.arm.m3.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.m3.Timer",
            timestampDelegate : "ti.sysbios.family.arm.m3.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.m3.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.m3.IntrinsicsSupport",
            bootModule : null,
            mmuModule : null,
            cacheDelegate : null,
            powerDelegate : "ti.sysbios.family.arm.m3.Power",
            secondsDelegate : null,
            coreDelegate : null,
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.M3", "ti.targets.arm.M3_big_endian",
                        "ti.targets.arm.elf.M3", "ti.targets.arm.elf.M3_big_endian",
                        "gnu.targets.arm.M3" ]
        },
    },
    "ti.catalog.arm.cortexa53": {
        "SIMFLEMING": {
            hwiDelegate : "ti.sysbios.family.arm.gicv3.Hwi",
            timerDelegate : "ti.sysbios.timers.dmtimer.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.v8a.Timer",
            timerSupportDelegate : "ti.sysbios.family.arm.a15.TimerSupport",
            timestampDelegate : "ti.sysbios.family.arm.v8a.TimestampProvider",
            timestampDelegateSmp : null,
            taskSupportDelegate : "ti.sysbios.family.arm.v8a.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            cacheDelegate : null,
            cacheDelegateSmp : null,
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : null,
            coreDelegateSmp : null,
            mmuModule : "ti.sysbios.family.arm.v8a.Mmu",
            bootModule : null,
            clockTickPeriod : 1000,
            targets : [ "gnu.targets.arm.A53F" ]
        },
    },
    "ti.catalog.arm.cortexa15": {
        "DRA7XX": {
            hwiDelegate : "ti.sysbios.family.arm.gic.Hwi",
            timerDelegate : "ti.sysbios.timers.dmtimer.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.systimer.Timer",
            timerSupportDelegate : "ti.sysbios.family.shared.vayu.TimerSupport",
            timestampDelegate : "ti.sysbios.family.arm.a15.TimestampProvider",
            timestampDelegateSmp : "ti.sysbios.timers.dmtimer.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            cacheDelegate : "ti.sysbios.family.arm.a15.Cache",
            cacheDelegateSmp : "ti.sysbios.family.arm.a15.smp.Cache",
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : "ti.sysbios.family.arm.a15.Core",
            coreDelegateSmp : "ti.sysbios.family.arm.a15.smp.Core",
            mmuModule : "ti.sysbios.family.arm.a15.Mmu",
            bootModule : null,
            clockTickPeriod : 1000,
            targets : [ "gnu.targets.arm.A15", "gnu.targets.arm.A15F" ]
        },
        "OMAP5430": {
            hwiDelegate : "ti.sysbios.family.arm.gic.Hwi",
            timerDelegate : "ti.sysbios.timers.dmtimer.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.systimer.Timer",
            timerSupportDelegate : "ti.sysbios.family.arm.a15.TimerSupport",
            timestampDelegate : "ti.sysbios.family.arm.a15.TimestampProvider",
            timestampDelegateSmp : "ti.sysbios.timers.dmtimer.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            cacheDelegate : "ti.sysbios.family.arm.a15.Cache",
            cacheDelegateSmp : "ti.sysbios.family.arm.a15.smp.Cache",
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : "ti.sysbios.family.arm.a15.Core",
            coreDelegateSmp : "ti.sysbios.family.arm.a15.smp.Core",
            mmuModule : "ti.sysbios.family.arm.a15.Mmu",
            bootModule : null,
            clockTickPeriod : 1000,
            targets : [ "gnu.targets.arm.A15", "gnu.targets.arm.A15F" ]
        },
        "TCI6636K2H": {
            hwiDelegate : "ti.sysbios.family.arm.gic.Hwi",
            timerDelegate : "ti.sysbios.timers.timer64.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.systimer.Timer",
            timerSupportDelegate : "ti.sysbios.family.arm.a15.TimerSupport",
            timestampDelegate : "ti.sysbios.family.arm.a15.TimestampProvider",
            timestampDelegateSmp : "ti.sysbios.timers.timer64.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            cacheDelegate : "ti.sysbios.family.arm.a15.Cache",
            cacheDelegateSmp : "ti.sysbios.family.arm.a15.smp.Cache",
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : "ti.sysbios.family.arm.a15.Core",
            coreDelegateSmp : "ti.sysbios.family.arm.a15.smp.Core",
            mmuModule : "ti.sysbios.family.arm.a15.Mmu",
            bootModule : null,
            clockTickPeriod : 1000,
            targets : [ "gnu.targets.arm.A15", "gnu.targets.arm.A15F" ]
        },
    },
    "ti.catalog.arm.cortexa9": {
        "OMAP4430": {
            hwiDelegate : "ti.sysbios.family.arm.gic.Hwi",
            timerDelegate : "ti.sysbios.timers.dmtimer.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.a9.Timer",
            timerSupportDelegate : "ti.sysbios.family.arm.a9.TimerSupport",
            timestampDelegate : "ti.sysbios.family.arm.a15.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            cacheDelegate : "ti.sysbios.family.arm.a9.Cache",
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : null,
            mmuModule : "ti.sysbios.family.arm.a8.Mmu",
            bootModule : null,
            clockTickPeriod : 1000,
            targets : [ "gnu.targets.arm.A9F" ]
        },
        "AM437X": {
            hwiDelegate : "ti.sysbios.family.arm.gic.Hwi",
            timerDelegate : "ti.sysbios.timers.dmtimer.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.a9.Timer",
            timerSupportDelegate : "ti.sysbios.family.arm.a9.am437x.TimerSupport",
            timestampDelegate : "ti.sysbios.family.arm.a15.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            cacheDelegate : "ti.sysbios.family.arm.a9.Cache",
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : null,
            mmuModule : "ti.sysbios.family.arm.a8.Mmu",
            bootModule : null,
            clockTickPeriod : 1000,
            targets : [ "gnu.targets.arm.A9F" ]
        },
    },
    "ti.catalog.arm.cortexa8": {
        "TI81XX": {
            hwiDelegate : "ti.sysbios.family.arm.a8.intcps.Hwi",
            timerDelegate : "ti.sysbios.timers.dmtimer.Timer",
            clockTimerDelegate : "ti.sysbios.timers.dmtimer.Timer",
            timerSupportDelegate : "ti.sysbios.family.arm.a8.ti81xx.TimerSupport",
            timestampDelegate : "ti.sysbios.family.arm.a8.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            bootModule : null,
            cacheDelegate : "ti.sysbios.family.arm.a8.Cache",
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : null,
            mmuModule : "ti.sysbios.family.arm.a8.Mmu",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.A8Fnv", "gnu.targets.arm.A8F" ]
        },
        "TMS320C3430": {
            hwiDelegate : "ti.sysbios.family.arm.a8.intcps.Hwi",
            timerDelegate : "ti.sysbios.timers.gptimer.Timer",
            clockTimerDelegate : "ti.sysbios.timers.gptimer.Timer",
            timerSupportDelegate : "ti.sysbios.family.arm.a8.omap3430.TimerSupport",
            timestampDelegate : "ti.sysbios.family.arm.a8.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            bootModule : null,
            cacheDelegate : "ti.sysbios.family.arm.a8.Cache",
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : null,
            mmuModule : "ti.sysbios.family.arm.a8.Mmu",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.A8Fnv", "gnu.targets.arm.A8F" ]
        },
    },
    "ti.catalog.arm.cortexm3": {
        "OMAP4430": {
            hwiDelegate : "ti.sysbios.family.arm.m3.Hwi",
            timerDelegate : "ti.sysbios.family.arm.ducati.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.ducati.Timer",
            timerSupportDelegate : "ti.sysbios.family.arm.ducati.TimerSupport",
            timestampDelegate : "ti.sysbios.family.arm.ducati.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.m3.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.m3.IntrinsicsSupport",
            bootModule : null,
            cacheDelegate : "ti.sysbios.hal.unicache.Cache",
            powerDelegate : "ti.sysbios.family.arm.m3.Power",
            secondsDelegate : null,
            coreDelegate : "ti.sysbios.family.arm.ducati.Core",
            mmuModule : "ti.sysbios.hal.ammu.AMMU",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.M3", "ti.targets.arm.M3_big_endian",
                        "ti.targets.arm.elf.M3", "ti.targets.arm.elf.M3_big_endian",
                        "gnu.targets.arm.M3" ]
        },
        "F28M3.*": {
            hwiDelegate : "ti.sysbios.family.arm.m3.Hwi",
            timerDelegate : "ti.sysbios.family.arm.lm3.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.m3.Timer",
            timestampDelegate : "ti.sysbios.family.arm.f28m35x.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.m3.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.m3.IntrinsicsSupport",
            bootModule : "ti.catalog.arm.cortexm3.concertoInit.Boot",
            mmuModule : null,
            cacheDelegate : null,
            powerDelegate : "ti.sysbios.family.arm.m3.Power",
            secondsDelegate : null,
            coreDelegate : null,
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.M3", "ti.targets.arm.elf.M3",
                        "gnu.targets.arm.M3" ]
        },
        "CC2538SF53": {
            hwiDelegate : "ti.sysbios.family.arm.m3.Hwi",
            timerDelegate : "ti.sysbios.family.arm.m3.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.m3.Timer",
            timestampDelegate : "ti.sysbios.family.arm.m3.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.m3.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.m3.IntrinsicsSupport",
            mmuModule : null,
            cacheDelegate : null,
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : null,
            bootModule : null,
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.M3", "iar.targets.arm.M3" ]
        },
        "CC2650": {
            hwiDelegate : "ti.sysbios.family.arm.m3.Hwi",
            timerDelegate : "ti.sysbios.family.arm.m3.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.cc26xx.Timer",
            timestampDelegate : "ti.sysbios.family.arm.cc26xx.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.m3.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.m3.IntrinsicsSupport",
            mmuModule : null,
            cacheDelegate : null,
            powerDelegate : "ti.sysbios.family.arm.cc26xx.Power",
            secondsDelegate : "ti.sysbios.family.arm.cc26xx.Seconds",
            coreDelegate : null,
            bootModule : "ti.sysbios.family.arm.cc26xx.Boot",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.M3", "iar.targets.arm.M3" ]
        },
    },
    "ti.catalog.arm.cortexm4": {
        "TM4C129CNCPDT": {
            hwiDelegate : "ti.sysbios.family.arm.m3.Hwi",
            timerDelegate : "ti.sysbios.family.arm.lm4.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.lm4.Timer",
            timestampDelegate : "ti.sysbios.family.arm.lm4.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.m3.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.m3.IntrinsicsSupport",
            mmuModule : null,
            cacheDelegate : null,
            powerDelegate : "ti.sysbios.family.arm.m3.Power",
            secondsDelegate : "ti.sysbios.family.arm.lm4.Seconds",
            coreDelegate : null,
            bootModule : "ti.catalog.arm.cortexm4.tiva.ce.Boot",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.M4", "ti.targets.arm.elf.M4F",
                        "gnu.targets.arm.M4", "gnu.targets.arm.M4F",
                        "iar.targets.arm.M4", "iar.targets.arm.M4F" ]
        },
        "OMAP5430": {
            hwiDelegate : "ti.sysbios.family.arm.m3.Hwi",
            timerDelegate : "ti.sysbios.family.arm.ducati.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.ducati.Timer",
            timerSupportDelegate : "ti.sysbios.family.arm.ducati.TimerSupport",
            timestampDelegate : "ti.sysbios.family.arm.ducati.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.m3.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.m3.IntrinsicsSupport",
            bootModule : null,
            coreDelegate : "ti.sysbios.family.arm.ducati.Core",
            cacheDelegate : "ti.sysbios.hal.unicache.Cache",
            powerDelegate : "ti.sysbios.family.arm.m3.Power",
            secondsDelegate : null,
            mmuModule : "ti.sysbios.hal.ammu.AMMU",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.M3", "ti.targets.arm.M3_big_endian",
                        "ti.targets.arm.elf.M3", "ti.targets.arm.elf.M3_big_endian",
                        "ti.targets.arm.elf.M4", "gnu.targets.arm.M4" ]
        },
        "DRA7XX": {
            hwiDelegate : "ti.sysbios.family.arm.m3.Hwi",
            timerDelegate : "ti.sysbios.family.arm.ducati.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.ducati.Timer",
            timerSupportDelegate : "ti.sysbios.family.shared.vayu.TimerSupport",
            timestampDelegate : "ti.sysbios.family.arm.ducati.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.m3.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.m3.IntrinsicsSupport",
            bootModule : null,
            coreDelegate : "ti.sysbios.family.arm.ducati.Core",
            cacheDelegate : "ti.sysbios.hal.unicache.Cache",
            powerDelegate : "ti.sysbios.family.arm.m3.Power",
            secondsDelegate : null,
            mmuModule : "ti.sysbios.hal.ammu.AMMU",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.M4", "gnu.targets.arm.elf.M4" ]
        },
        "CC3200": {
            hwiDelegate : "ti.sysbios.family.arm.m3.Hwi",
            timerDelegate : "ti.sysbios.family.arm.lm4.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.m3.Timer",
            timestampDelegate : "ti.sysbios.family.arm.m3.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.m3.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.m3.IntrinsicsSupport",
            mmuModule : null,
            cacheDelegate : null,
            powerDelegate : null,
            secondsDelegate : "ti.sysbios.family.arm.cc32xx.Seconds",
            coreDelegate : null,
            bootModule : null,
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.M4" ]
        },
        "MSP432P401R": {
            hwiDelegate : "ti.sysbios.family.arm.m3.Hwi",
            timerDelegate : "ti.sysbios.family.arm.msp432.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.msp432.Timer",
            timestampDelegate : "ti.sysbios.family.arm.m3.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.m3.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.m3.IntrinsicsSupport",
            mmuModule : null,
            cacheDelegate : null,
            powerDelegate : null,
            secondsDelegate : "ti.sysbios.family.arm.msp432.Seconds",
            coreDelegate : null,
            bootModule : "ti.sysbios.family.arm.msp432.init.Boot",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.M4F" ]
        },
        "MSP432E401Y": {
            hwiDelegate : "ti.sysbios.family.arm.m3.Hwi",
            timerDelegate : "ti.sysbios.family.arm.lm4.Timer",
            clockTimerDelegate : "ti.sysbios.family.arm.lm4.Timer",
            timestampDelegate : "ti.sysbios.family.arm.lm4.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.m3.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.m3.IntrinsicsSupport",
            mmuModule : null,
            cacheDelegate : null,
            powerDelegate : "ti.sysbios.family.arm.m3.Power",
            secondsDelegate : "ti.sysbios.family.arm.lm4.Seconds",
            coreDelegate : null,
            bootModule : "ti.sysbios.family.arm.msp432e4.init.Boot",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.M4F",
                        "gnu.targets.arm.M4F",
                        "iar.targets.arm.M4F" ]
        },
    },
    "ti.catalog.arm.cortexr5": {
        "RM57D8XX": {
            hwiDelegate : "ti.sysbios.family.arm.v7r.vim.Hwi",
            timerDelegate : "ti.sysbios.timers.rti.Timer",
            clockTimerDelegate : "ti.sysbios.timers.rti.Timer",
            timestampDelegate : "ti.sysbios.family.arm.a15.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            bootModule : "ti.sysbios.family.arm.v7r.tms570.Boot",
            mmuModule : "ti.sysbios.family.arm.MPU",
            cacheDelegate : "ti.sysbios.family.arm.v7r.Cache",
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : "ti.sysbios.family.arm.v7r.tms570.Core",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.R5F" ]
        },
        "SIMFLEMING": {
            hwiDelegate : "ti.sysbios.family.arm.v7r.keystone3.Hwi",
            timerDelegate : "ti.sysbios.timers.dmtimer.Timer",
            timerSupportDelegate : "ti.sysbios.family.arm.v7r.keystone3.TimerSupport",
            clockTimerDelegate: "ti.sysbios.timers.dmtimer.Timer",
            timestampDelegate : "ti.sysbios.family.arm.a15.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            bootModule : null,
            mmuModule : null, /*"ti.sysbios.family.arm.MPU",*/
            cacheDelegate : null, /*"ti.sysbios.family.arm.v7r.Cache",*/
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : "ti.sysbios.family.arm.v7r.keystone3.Core",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.R5F" ]
        },
    },
    "ti.catalog.arm.cortexr4": {
        "RM48L5XX": {
            hwiDelegate : "ti.sysbios.family.arm.v7r.vim.Hwi",
            timerDelegate : "ti.sysbios.timers.rti.Timer",
            clockTimerDelegate : "ti.sysbios.timers.rti.Timer",
            timestampDelegate : "ti.sysbios.family.arm.a15.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            bootModule : null,
            mmuModule : "ti.sysbios.family.arm.MPU",
            cacheDelegate : null,
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : "ti.sysbios.family.arm.v7r.tms570.Core",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.R4F", "ti.targets.arm.elf.R4Ft" ]
        },
        "AR14XX": {
            hwiDelegate : "ti.sysbios.family.arm.v7r.vim.Hwi",
            timerDelegate : "ti.sysbios.timers.rti.Timer",
            clockTimerDelegate : "ti.sysbios.timers.rti.Timer",
            timestampDelegate : "ti.sysbios.family.arm.a15.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            bootModule : "ti.sysbios.family.arm.v7r.tms570.Boot",
            mmuModule : "ti.sysbios.family.arm.MPU",
            cacheDelegate : null,
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : "ti.sysbios.family.arm.v7r.tms570.Core",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.R4F", "ti.targets.arm.elf.R4Ft" ]
        },
        "AWR14XX": {
            hwiDelegate : "ti.sysbios.family.arm.v7r.vim.Hwi",
            timerDelegate : "ti.sysbios.timers.rti.Timer",
            clockTimerDelegate : "ti.sysbios.timers.rti.Timer",
            timestampDelegate : "ti.sysbios.family.arm.a15.TimestampProvider",
            taskSupportDelegate : "ti.sysbios.family.arm.TaskSupport",
            intrinsicsSupportDelegate : "ti.sysbios.family.arm.IntrinsicsSupport",
            bootModule : null,
            mmuModule : "ti.sysbios.family.arm.MPU",
            cacheDelegate : null,
            powerDelegate : null,
            secondsDelegate : null,
            coreDelegate : "ti.sysbios.family.arm.v7r.tms570.Core",
            clockTickPeriod : 1000,
            targets : [ "ti.targets.arm.elf.R4F", "ti.targets.arm.elf.R4Ft" ]
        },
    },
};

deviceTable["ti.catalog.arm"]["TMS320DA805"]    = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["TMS320DA807"]    = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["TMS320DA808"]    = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["TMS320DA810"]    = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["TMS320DA828"]    = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["OMAPL137"]       = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["AM1705"]         = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["AM1707"]         = deviceTable["ti.catalog.arm"]["TMS320DA830"];

deviceTable["ti.catalog.arm"]["OMAPL108"]       = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["OMAPL118"]       = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["OMAPL138"]       = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["TMS320DA840"]    = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["TMS320DA850"]    = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["AM1806"]         = deviceTable["ti.catalog.arm"]["TMS320DA830"];
deviceTable["ti.catalog.arm"]["AM1808"]         = deviceTable["ti.catalog.arm"]["TMS320DA830"];

deviceTable["ti.catalog.arm"]["TMS320CDM740"]   = deviceTable["ti.catalog.arm.cortexm3"]["OMAP4430"];
deviceTable["ti.catalog.arm"]["TMS320CDM730"]   = deviceTable["ti.catalog.arm.cortexm3"]["OMAP4430"];
deviceTable["ti.catalog.arm.cortexm3"]["TMS320TI811X"]  = deviceTable["ti.catalog.arm.cortexm3"]["OMAP4430"];
deviceTable["ti.catalog.arm.cortexm3"]["TMS320TI813X"]  = deviceTable["ti.catalog.arm.cortexm3"]["OMAP4430"];
deviceTable["ti.catalog.arm.cortexm3"]["TMS320TI814X"]  = deviceTable["ti.catalog.arm.cortexm3"]["OMAP4430"];
deviceTable["ti.catalog.arm.cortexm3"]["TMS320TI816X"]  = deviceTable["ti.catalog.arm.cortexm3"]["OMAP4430"];
deviceTable["ti.catalog.arm.cortexm3"]["TMS320.*81.8"]  = deviceTable["ti.catalog.arm.cortexm3"]["OMAP4430"];
deviceTable["ti.catalog.arm.cortexm3"]["TMS320C6A8149"]  = deviceTable["ti.catalog.arm.cortexm3"]["OMAP4430"];

deviceTable["ti.catalog.arm.cortexa8"]["TMS320TI811X"]  = deviceTable["ti.catalog.arm.cortexa8"]["TI81XX"];
deviceTable["ti.catalog.arm.cortexa8"]["TMS320TI813X"]  = deviceTable["ti.catalog.arm.cortexa8"]["TI81XX"];
deviceTable["ti.catalog.arm.cortexa8"]["TMS320TI816X"]  = deviceTable["ti.catalog.arm.cortexa8"]["TI81XX"];
deviceTable["ti.catalog.arm.cortexa8"]["TMS320TI814X"]  = deviceTable["ti.catalog.arm.cortexa8"]["TI81XX"];
deviceTable["ti.catalog.arm.cortexa8"]["TMS320C6A8149"]  = deviceTable["ti.catalog.arm.cortexa8"]["TI81XX"];
deviceTable["ti.catalog.arm.cortexa8"]["TMS320.*81.8"]  = deviceTable["ti.catalog.arm.cortexa8"]["TI81XX"];
deviceTable["ti.catalog.arm.cortexa8"]["TMS320C3.*"]    = deviceTable["ti.catalog.arm.cortexa8"]["TMS320C3430"];
deviceTable["ti.catalog.arm.cortexa8"]["OMAP3.*"]       = deviceTable["ti.catalog.arm.cortexa8"]["TMS320C3430"];
deviceTable["ti.catalog.arm.cortexa8"]["AM35.*"]        = deviceTable["ti.catalog.arm.cortexa8"]["TMS320C3430"];
deviceTable["ti.catalog.arm.cortexa8"]["DM37XX"]        = deviceTable["ti.catalog.arm.cortexa8"]["TMS320C3430"];

deviceTable["ti.catalog.arm.cortexa15"]["Vayu"] = deviceTable["ti.catalog.arm.cortexa15"]["DRA7XX"];
deviceTable["ti.catalog.arm.cortexa15"]["C66AK2E05"] = deviceTable["ti.catalog.arm.cortexa15"]["TCI6636K2H"];
deviceTable["ti.catalog.arm.cortexa15"]["TCI6630K2L"] = deviceTable["ti.catalog.arm.cortexa15"]["TCI6636K2H"];
deviceTable["ti.catalog.arm.cortexa15"]["TCI6638K2K"] = deviceTable["ti.catalog.arm.cortexa15"]["TCI6636K2H"];
deviceTable["ti.catalog.arm.cortexa15"]["TCI66AK2G02"] = deviceTable["ti.catalog.arm.cortexa15"]["TCI6636K2H"];

deviceTable["ti.catalog.arm.cortexa53"]["SIMMAXWELL"] = deviceTable["ti.catalog.arm.cortexa53"]["SIMFLEMING"];

deviceTable["ti.catalog.arm.cortexa8"]["AM335.*"]  = deviceTable["ti.catalog.arm.cortexa8"]["TI81XX"];

/* Avoid duplicate entries in delegates.html file */
delete(deviceTable["ti.catalog.arm.cortexa8"]["TMS320C3430"]);

deviceTable["ti.catalog.arm.cortexr5"]["SIMMAXWELL"] = deviceTable["ti.catalog.arm.cortexr5"]["SIMFLEMING"];

deviceTable["ti.catalog.arm.cortexm3"]["CortexM3"] = deviceTable["ti.catalog.arm"]["CortexM3"];
deviceTable["ti.catalog.arm.cortexm3"]["OMAP5430"]  = deviceTable["ti.catalog.arm.cortexm3"]["OMAP4430"];
deviceTable["ti.catalog.arm.cortexm3"]["CC2538.*"]  = deviceTable["ti.catalog.arm.cortexm3"]["CC2538SF53"];
deviceTable["ti.catalog.arm.cortexm3"]["CC26.*"]  = deviceTable["ti.catalog.arm.cortexm3"]["CC2650"];
deviceTable["ti.catalog.arm.cortexm3"]["CC13.*"]  = deviceTable["ti.catalog.arm.cortexm3"]["CC2650"];

/* CC3220 M4 devices */
deviceTable["ti.catalog.arm.cortexm4"]["CC32.*"] = deviceTable["ti.catalog.arm.cortexm4"]["CC3200"];

/* Tiva devices */
deviceTable["ti.catalog.arm.cortexm4"]["TM4.*"]     = deviceTable["ti.catalog.arm.cortexm4"]["TM4C129CNCPDT"];

/* allow M4 devices to be built with M3 target */
deviceTable["ti.catalog.arm.cortexm3"]["TM4.*"]     = deviceTable["ti.catalog.arm.cortexm4"]["TM4C129CNCPDT"];


deviceTable["ti.catalog.arm.cortexm4"]["Vayu"]      = deviceTable["ti.catalog.arm.cortexm4"]["DRA7XX"];
deviceTable["ti.catalog.arm.cortexm4"]["TDA3XX"]    = deviceTable["ti.catalog.arm.cortexm4"]["DRA7XX"];

/* Agama devices */
deviceTable["ti.catalog.arm.cortexm4"]["CC13.*"]    = deviceTable["ti.catalog.arm.cortexm3"]["CC2650"];
deviceTable["ti.catalog.arm.cortexm4"]["CC26.*"]    = deviceTable["ti.catalog.arm.cortexm3"]["CC2650"];

/* Cortex-R5 devices */
deviceTable["ti.catalog.arm.cortexr5"]["RM57D8.*"] = deviceTable["ti.catalog.arm.cortexr5"]["RM57D8XX"];
deviceTable["ti.catalog.arm.cortexr5"]["RM57L8.*"] = deviceTable["ti.catalog.arm.cortexr5"]["RM57D8XX"];
deviceTable["ti.catalog.arm.cortexr4"]["AWR16XX"] = deviceTable["ti.catalog.arm.cortexr4"]["AWR14XX"];
deviceTable["ti.catalog.arm.cortexr4"]["IWR14XX"] = deviceTable["ti.catalog.arm.cortexr4"]["AWR14XX"];
deviceTable["ti.catalog.arm.cortexr4"]["IWR16XX"] = deviceTable["ti.catalog.arm.cortexr4"]["AWR14XX"];

/* MSP432 devices */
deviceTable["ti.catalog.arm.cortexm4"]["MSP432E.*"] = deviceTable["ti.catalog.arm.cortexm4"]["MSP432E401Y"];
deviceTable["ti.catalog.arm.cortexm4"]["MSP432P.*"] = deviceTable["ti.catalog.arm.cortexm4"]["MSP432P401R"];

var armSettings = xdc.loadCapsule("armSettings.xs");

for each (var deviceName in armSettings.m4Devices) {
    settings.device["ti.catalog.arm.cortexm4." + deviceName]
        = deviceTable["ti.catalog.arm.cortexm4"]["TM4C129CNCPDT"];
    deviceTable["ti.catalog.arm.cortexm4"][deviceName]
        = deviceTable["ti.catalog.arm.cortexm4"]["TM4C129CNCPDT"];
}

for (catalogName in deviceTable) {
    for (var deviceName in deviceTable[catalogName]) {
        settings.device[catalogName + "." + deviceName] =
                deviceTable[catalogName][deviceName];
    }
}

/*
 *  ======== deviceSupportCheck ========
 *  Check validity of device
 */
function deviceSupportCheck()
{
    catalogName = Program.cpu.catalogName;
    var deviceName;

    /* look for exact match */
    for (deviceName in deviceTable[Program.cpu.catalogName]) {
        if (deviceName == Program.cpu.deviceName) {
            return deviceName;
        }
    }

    /* now look for wild card match */
    for (deviceName in deviceTable[Program.cpu.catalogName]) {
        if (Program.cpu.deviceName.match(deviceName)) {
            return deviceName;
        }
    }

    /*
     * no match, print all catalog devices supported
     * and then raise an error
     */
    var catalog = xdc.loadPackage(Program.cpu.catalogName);

    var sd = [];

    for (var fullName in settings.device) {
        var deviceTokenized = fullName.split('.');
        deviceName = deviceTokenized.pop();
        var catalogPkg = deviceTokenized.join('.');
        if (catalogPkg == Program.cpu.catalogName &&
            sd.indexOf(deviceName) == -1) {
            sd.push(deviceName);
        }
    }


    for (var i = 0; i < catalog.$modules.length; i++) {
        catalogName = catalog.$modules[i].$name.substring(Program.cpu.catalogName.length+1);
        for (deviceName in deviceTable[Program.cpu.catalogName]) {
            if (catalogName.match(deviceName)) {
                if (sd.indexOf(catalogName) == -1) {
                    sd.push(catalogName);
                }
            }
        }
    }

    /* sort it for a nicer report */
    sd.sort();

    print("The " + Program.cpu.deviceName + " device is not currently supported.");
    print("The following devices are supported in the " +
            Program.cpu.catalogName + " catalog:");

    var numColumns = 5;
    for (i = 0; i < (sd.length % numColumns); i++) {
        sd.push("");
    }
    for (var i=0; i < sd.length; i += numColumns) {
        print("\t" + sd[i] + "  " + sd[i + 1] + "  " + sd[i + 2] +
                "  " + sd[i + 3] + "  " + sd[i + 4]);
    }

    throw new Error ("Unsupported device!");
}

/*
 *  ======== getDefaultCoreDelegate ========
 */
function getDefaultCoreDelegate()
{
    var BIOS = xdc.module('ti.sysbios.BIOS');
    var deviceName = deviceSupportCheck();

    if ((BIOS.smpEnabled == true) &&
        ('coreDelegateSmp' in deviceTable[catalogName][deviceName])) {
        return (deviceTable[catalogName][deviceName].coreDelegateSmp);
    }
    else {
        return (deviceTable[catalogName][deviceName].coreDelegate);
    }
}

/*
 *  ======== getDefaultHwiDelegate ========
 */
function getDefaultHwiDelegate()
{
    var BIOS = xdc.module('ti.sysbios.BIOS');
    var deviceName = deviceSupportCheck();

    if ((BIOS.smpEnabled == true) &&
        ('hwiDelegateSmp' in deviceTable[catalogName][deviceName])) {
        return (deviceTable[catalogName][deviceName].hwiDelegateSmp);
    }
    else {
        return (deviceTable[catalogName][deviceName].hwiDelegate);
    }
}

/*
 *  ======== getDefaultTimerDelegate ========
 */
function getDefaultTimerDelegate()
{
    var deviceName = deviceSupportCheck();
    return (deviceTable[catalogName][deviceName].timerDelegate);
}

/*
 *  ======== getDefaultClockTimerDelegate ========
 */
function getDefaultClockTimerDelegate()
{
    var deviceName = deviceSupportCheck();
    return (deviceTable[catalogName][deviceName].clockTimerDelegate);
}

/*
 *  ======== getDefaultTimerSupportDelegate ========
 */
function getDefaultTimerSupportDelegate()
{
    var deviceName = deviceSupportCheck();
    return (deviceTable[catalogName][deviceName].timerSupportDelegate);
}

/*
 *  ======== getDefaultTimestampDelegate ========
 */
function getDefaultTimestampDelegate()
{
    var BIOS = xdc.module('ti.sysbios.BIOS');
    var deviceName = deviceSupportCheck();

    if ((BIOS.smpEnabled == true) &&
        ('timestampDelegateSmp' in deviceTable[catalogName][deviceName])) {
        return (deviceTable[catalogName][deviceName].timestampDelegateSmp);
    }
    else {
        return (deviceTable[catalogName][deviceName].timestampDelegate);
    }
}

/*
 *  ======== getDefaultTaskSupportDelegate ========
 */
function getDefaultTaskSupportDelegate()
{
    var deviceName = deviceSupportCheck();
    return (deviceTable[catalogName][deviceName].taskSupportDelegate);
}

/*
 *  ======== getDefaultIntrinsicsSupportDelegate ========
 */
function getDefaultIntrinsicsSupportDelegate()
{
    var deviceName = deviceSupportCheck();
    return (deviceTable[catalogName][deviceName].intrinsicsSupportDelegate);
}

/*
 *  ======== getDefaultCacheDelegate ========
 */
function getDefaultCacheDelegate()
{
    var BIOS = xdc.module('ti.sysbios.BIOS');
    var deviceName = deviceSupportCheck();

    if ((BIOS.smpEnabled == true) &&
        ('cacheDelegateSmp' in deviceTable[catalogName][deviceName])) {
        return (deviceTable[catalogName][deviceName].cacheDelegateSmp);
    }
    else {
        if  (deviceTable[catalogName][deviceName].cacheDelegate == null) {
            return ("ti.sysbios.hal.CacheNull");
        }
        else {
            return (deviceTable[catalogName][deviceName].cacheDelegate);
        }
    }
}

/*
 *  ======== getDefaultPowerDelegate ========
 */
function getDefaultPowerDelegate()
{
    var deviceName = deviceSupportCheck();

    if  (deviceTable[catalogName][deviceName].powerDelegate == null) {
        return ("ti.sysbios.hal.PowerNull");
    }
    else {
        return (deviceTable[catalogName][deviceName].powerDelegate);
    }
}

/*
 *  ======== getDefaultSecondsDelegate ========
 */
function getDefaultSecondsDelegate()
{
    var deviceName = deviceSupportCheck();
    return (deviceTable[catalogName][deviceName].secondsDelegate);
}

/*
 *  ======== getDefaultClockTickPeriod ========
 */
function getDefaultClockTickPeriod()
{
    var deviceName = deviceSupportCheck();
    return (deviceTable[catalogName][deviceName].clockTickPeriod);
}

/*
 *  ======== getDefaultBootModule ========
 */
function getDefaultBootModule()
{
    var deviceName = deviceSupportCheck();
    return (deviceTable[catalogName][deviceName].bootModule);
}

/*
 *  ======== getDefaultMmuModule ========
 */
function getDefaultMmuModule()
{
    var deviceName = deviceSupportCheck();
    return (deviceTable[catalogName][deviceName].mmuModule);
}

/*
 *  ======== getFamilySettingsXml ========
 */
function getFamilySettingsXml()
{
    return ("family/arm/Settings.xml");
}
