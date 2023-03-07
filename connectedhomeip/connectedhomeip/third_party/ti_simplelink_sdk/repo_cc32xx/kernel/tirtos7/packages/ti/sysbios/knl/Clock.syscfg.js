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
 * ======== Clock.syscfg.js ========
 */

/* global exports, system */

/* use bigInt for zero padding hex numbers */
let bigInt = system.utils.bigInt;

/* get device specific Settings */
let Settings = system.getScript("/ti/sysbios/family/Settings.js");

/*
 * ======== moduleInstances ========
 */
function moduleInstances (mod)
{
    let modInstances = new Array();

    modInstances.push({
        name: "clockInit",
        displayName: "Clock_init()",
        moduleName: "ti/sysbios/runtime/Startup",
        hidden: true,
        readOnly: true,
        requiredArgs: { 
            startupFxn: "Clock_init",
            functionType: "Module Init Function",
            externRequired: false,
            priority: 0,
            $name: "Clock_init"
        }
    });

    return (modInstances);
}

/*
 * ======== modules ========
 * Pull in the appropiate dependent modules
 */
function modules(mod)
{
    let modArray = [];

    modArray.push({
        name      : "ClockSupport",
        moduleName: Settings.clockSupportModule
    });

    modArray.push({
        name      : "Queue",
        moduleName: "ti/sysbios/knl/Queue"
    });

    return (modArray);
}

/*
 *  ======== validate ========
 *  Validate this module's configuration
 *
 *  param mod - Clock module
 *  param vo  - Issue reporting object
 *
 */
function validate(mod, vo)
{
    if ((mod.tickMode == "Clock_TickMode_DYNAMIC") &&
        (mod.tickSource != "Clock_TickSource_TIMER")) {
        vo.logError("Clock_TickMode_DYNAMIC mode is only supported with Clock_TickSource_TIMER", mod, "tickSource");
    }
}

exports = {
    staticOnly: true,
    displayName: "Clock",
    moduleStatic: {
        name: "moduleGlobal",
        modules: modules,
        moduleInstances: moduleInstances,
        validate: validate,
        config: [
            {
                name: "tickPeriod",
                displayName: "Clock Tick Period in microseconds",
                description: `Tick period specified in microseconds`,
                longDescription: `
Default value is family dependent. For example, Linux systems often only
support a minimum period of 10000 us and multiples of 10000 us. TI platforms
have a default of 1000 us.

Like most other module configuration parameters, the Clock.tickPeriod config
parameter value is accessible in runtime C code as \"Clock_tickPeriod\".

                `,
                default: Settings.defaultClockTickPeriod,
            },
            {
                name: "tickSource",
                displayName: "Clock Tick Source",
                description: `Source of clock ticks`,
                longDescription: `
"Clock_TickSource_TIMER": The Clock module automatically configures a
a Timer instance to drive the Clock tick.

"Clock_TickSource_USER": The Application is responsible for calling
"Clock_tick()" periodically.

"Clock_TickSource_NULL": The Clock module is disabled.
In this case, it is an error for the application to ever call
Clock_tick().

If this parameter is not set to "Clock_TickSource_TIMER", Clock_tickStart(),
Clock_tickStop(), and Clock_tickReconfig(), have no effect.

The default is "Clock_TickSource_TIMER".
                `,
                default: "Clock_TickSource_TIMER",
                options: [
                    { name: "Clock_TickSource_TIMER", description: "Timer" },
                    { name: "Clock_TickSource_USER", description: "User" },
                    { name: "Clock_TickSource_NULL", description: "None" }
                ]
            },
            {
                name: "tickMode",
                displayName: "Clock Tick Mode",
                description: `Timer tick mode`,
                longDescription: `
This parameter specifies the tick mode to be used by the underlying Timer.

With "Clock_TickMode_PERIODIC" the timer will interrupt the CPU at a fixed rate,
defined by the tickPeriod.

With "Clock_TickMode_DYNAMIC" the timer can be dynamically reprogrammed by Clock, to
interrupt the CPU when the next tick is actually needed for a scheduled
timeout. "Clock_TickMode_DYNAMIC" is not supported on all devices, and may have some
application constraints.
                `,
                default: Settings.defaultClockTickMode,
                options: [
                    { name: "Clock_TickMode_PERIODIC", description: "Periodic" },
                    { name: "Clock_TickMode_DYNAMIC", description: "Dynamic" }
                ]
            },
            {
                name: "swiPriority",
                displayName: "Clock Swi Priority",
                description: `The priority of Swi used by Clock to process
                its instances`,
                longDescription: `
All Clock instances are executed in the context of a single Swi.  This
parameter allows you to control the priority of that Swi.

The default value of this parameter is Swi.numPriorities - 1; i.e., the
maximum Swi priority.
                `,
                default: 15
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/knl/Clock_config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt": "/ti/sysbios/knl/Clock.rov.js"
    },
    getCFiles: () => { return ["ti/sysbios/knl/Clock.c"] }
};

