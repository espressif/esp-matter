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
 * ======== Hwi.syscfg.js ========
 */

/* global exports, system */

/* use bigInt for zero padding hex numbers */
let bigInt = system.utils.bigInt;

/* get device specific Settings */
let Settings = system.getScript("/ti/sysbios/family/Settings.js");

/*
 * ======== moduleInstances ========
 */
function moduleInstances(mod)
{
    let modInstances = new Array();

    modInstances.push({
        name: "hwiInit",
        displayName: "Hwi_init()",
        moduleName: "ti/sysbios/runtime/Startup",
        hidden: true,
        readOnly: true,
        requiredArgs: {
            startupFxn: "Hwi_init",
            functionType: "Module Init Function",
            externRequired: false,
            priority: 4,
            $name: "Hwi_init"
        }
    });

    modInstances.push({
        name: "hwiHooks",
        displayName: "Hwi Hooks",
        moduleName: "ti/sysbios/hal/HwiHooks.syscfg.js",
        useArray: true
    });

    modInstances.push(
        {
            name: "initNVIC",
            description: `initialize everything but leave ints disabled`,
            displayName: "Hwi_initNVIC()",
            moduleName: "ti/sysbios/runtime/Startup",
            hidden: true,
            readOnly: true,
            requiredArgs: {
                startupFxn: "Hwi_initNVIC",
                functionType: "First Function",
                externRequired: false,
                priority: -3,  /* must be higher priority than Hwi_initStack */
                $name: "Hwi_initNVIC"
            }
        }
    );

    if (mod.initStackFlag || mod.checkStackFlag) {
        modInstances.push(
            {
                name: "initStack",
                displayName: "Hwi_initStack()",
                moduleName: "ti/sysbios/runtime/Startup",
                hidden: true,
                readOnly: true,
                requiredArgs: {
                    startupFxn: "Hwi_initStack",
                    functionType: "First Function",
                    externRequired: false,
                    priority: -2,  /* must be lower priority than Hwi_initNVIC */
                    $name: "Hwi_initStack"
                }
            }
        );
    };

    if (mod.checkStackFlag) {
        modInstances.push(
            {
                name: "checkStackIdleFunc",
                displayName: "Idle Function for Hwi stack check",
                moduleName: "ti/sysbios/knl/Idle",
                hidden: true,
                readOnly: true,
                requiredArgs: {
                    idleFxn: "Hwi_checkStack",
                    externRequired: false,
                    $name: "Hwi_checkStack"
                }
            }
        );
    }

    return (modInstances);
}

/*
 * ======== modules ========
 * Pull in Idle module if Hwi.checkStackFlag is true
 */
function modules(mod)
{
//    console.log(Object.keys(mod));

    let modArray = new Array(); 

    modArray.push({
        name      : "HwiHooks",
        moduleName: "ti/sysbios/hal/HwiHooks",
        hidden    : true
    });

    if (mod.checkStackFlag) {
        modArray.push({
            name      : "Idle",
            moduleName: "ti/sysbios/knl/Idle",
            hidden    : false
        });
    }

    return modArray;
}

/*
 * ======== enableExceptionChange ========
 */
function enableExceptionChange(inst, ui)
{
    if (inst.enableException) {
        inst.excHandlerFunc = "Hwi_excHandlerMax";
    }
    else {
        inst.excHandlerFunc = "Hwi_excHandlerMin";
    }
}

exports = {
    staticOnly: true,
    displayName: "Hwi",
    moduleStatic: {
        name: "moduleGlobal",
        modules: modules,
        moduleInstances: moduleInstances,
        config: [
            {
                name: "numInterrupts",
                displayName: "Number of Interrupts",
                default: Settings.defaultNumInterrupts
            },
            {
                name: "dispatchTableSize",
                hidden: true,
                default: Settings.defaultNumInterrupts
            },
            {
                name: "disablePriority",
                displayName: "Hwi_disable() priority",
                description: `The priority that BASEPRI is set to by
                Hwi_disable().`,
                longDescription: `
All interrupts configured with equal or less priority (equal or higher
number) than disablePriority are disabled by Hwi_disable. Interrupts
configured with higher priority (smaller number) than Hwi.disablePriority are
non-maskable (ie zero-latency).

The default setting is the second highest interrupt priority defined for the
device (typically '0x20' for devices which support 8 priority values). This
results in priority 0 (and all other values in the same priority group, ie
0x00 thru 0x1f) being the zero-latency, non-maskable interrupt priority. All
other priorities are disabled with Hwi_disable().

                `,
                default: "1",
                options: [
                    { name: "7", displayName: "7 - Lowest Priority" },
                    { name: "6" },
                    { name: "5" },
                    { name: "4" },
                    { name: "3" },
                    { name: "2" },
                    { name: "1", displayName: "1 - Highest Priority" }
                ]
            },
            {
                name: "priGroup",
                displayName: "Priority Group",
                description: "Priority Group Setting",
                longDescription: `
This value will be written to the PRIGROUP field within the NVIC's
Application Interrupt and Reset Control Register (Hwi_nvic.AIRCR). It defines
how the 8 bit priority values are interpreted by the hardware.
                `,
                default: "0",
                options: [
                    { name: "0" }, { name: "1" }, { name: "2" }, { name: "3" },
                    { name: "4" }, { name: "5" }, { name: "6" }, { name: "7" }
                ]
            },
            {
                name: "nvicCCR",
                displayName: "NVIC CCR Register Settings",
                longDescription: `
These setting are written to Hwi_nvic.CCR at startup time.
                `,
                minSelections: 0,
                default: ["STKALIGN"],
                options: [
                    { name: "STKALIGN" },
                    { name: "BFHFNMIGN" },
                    { name: "DIV_0_TRP" },
                    { name: "UNALIGN_TRP" },
                    { name: "USERSETMPEND" },
                    { name: "NONEBASETHRDENA" }
                ]
            },
            {
                name: "initStackFlag",
                displayName: "Initialize Hwi Stack",
                description: "Initialize ISR stack with known value for stack checking at runtime",
                longDescription: `
This is useful for inspection of the stack in the debugger or core
dump utilities for stack overflow and depth.

Default is true.
                `,
                default: true
            },
            {
                name: "checkStackFlag",
                displayName: "Check Hwi Stack In Idle Task",
                description: "Check for Hwi stack overrun during Idle loop.",
                longDescription: `
When set to 'true', an idle function is added to the idle loop
that checks for a Hwi stack overrun condition and raises
an Error if one is detected.

The check consists of testing the top of stack value against
its initial value. If it is no
longer at this value, the assumption is that the ISR stack
has been overrun. If the test fails, then the
"Hwi_E_stackOverflow" error is raised.
                `,
                default: false
            },
            {
                name: "dispatcherAutoNestingSupport",
                displayName: "Dispatcher Auto Nesting Support",
                description: `Include interrupt nesting logic in interrupt
                dispatcher?`,
                longDescription: `
Default is true.

This option provides the user with the ability to optimize interrupt
dispatcher performance when support for interrupt nesting is not required.

***warning:*** Setting this parameter to false will disable interrupts during the
execution of your Hwi functions.

                `,
                default: true
            },
            {
                name: "dispatcherSwiSupport",
                displayName: "Dispatcher Swi Support",
                description: `Include Swi scheduling logic in interrupt
                dispatcher?`,
                longDescription: `
This option provides the user with the ability to optimize interrupt
dispatcher performance when it is known that Swis will not be posted from any
of their Hwi threads.

***warning:*** Setting this parameter to false will disable the logic in the
interrupt dispatcher that invokes the Swi scheduler prior to returning from
an interrupt. With this setting, Swis MUST NOT be posted from Hwi functions!

The default is 'true'.
                `,
                default: true
            },
            {
                name: "dispatcherTaskSupport",
                displayName: "Dispatcher Task Support",
                description: `Include Task scheduling logic in interrupt
                dispatcher?`,
                longDescription: `
This option provides the user with the ability to optimize interrupt
dispatcher performance when it is known that no Task scheduling APIs (ie
Semaphore_post()) will be executed from any of their Hwi threads.

***warning:*** Setting this parameter to false will disable the logic in the interrupt
dispatcher that invokes the Task scheduler prior to returning from an
interrupt.

Default is 'true'.
                `,
                default: true
            },
            {
                name: "enableException",
                displayName: "Enable Exception Decoding at runtime",
                description: `Enable full exception decoding`,
                longDescription: `
When this is enabled, the exception handler will fully decode an exception
and dump the registers to the system console.

When disabled, the exception context is only available using ROV.
                `,
                onChange: enableExceptionChange,
                default: true
            },
            {
                name: "excHandlerFunc",
                displayName: "Exception Handler",
                description: `Exception handler function pointer.`,
                longDescription: `
The default is determined by the value of Hwi.enableException.

If the user does NOT set this parameter, then the following default behavior
is followed:

If Hwi.enableException is true, then the internal 'Hwi_excHandlerMax'
function is used. This exception handler saves the exception context then
does a complete exception decode and dump to the console, then raises an
Error. The exception context can be viewed within CCS in the ROV Hwi module's
Exception view.

If Hwi.enableException is false, then the internal 'Hwi_excHandlerMin'
function is used. This exception handler saves the exception context then
raises an Error. The exception context can be viewed within CCS in the ROV
Hwi module's Exception view.

If the user sets this parameter to their own function, then the user's
function will be invoked with the following arguments:

Void myExceptionHandler(UInt *excStack, UInt lr);

Where 'excStack' is the address of the stack containing the register context
at the time of the exception, and 'lr' is the link register value when the
low-level-assembly-coded exception handler was vectored to.

If this parameter is set to 'null', then an infinite while loop is entered
when an exception occurs. This setting minimizes code and data footprint but
provides no automatic exception decoding.

                `,
                hidden: true,
                default: "Hwi_excHandlerMax"
            },
            {
                /* provided for script compatibiility */
                name: "resetVectorAddress",
                displayName: "Reset Vector Table Address",
                description: `Reset vector table address. Default is
                0x00000000.`,
                longDescription: `
This parameter is the address of the vector table used at system reset time.
Typically this is placed at 0x00000000.

If the Hwi.resetVectorAddress has a different value than the
Hwi.vectorTableAddress then two vector tables are generated, one at the
Hwi.resetVectorAddress and another at the Hwi.vectorTableAddress.

After the initial boot code has been executed at startup, the NVIC's Vector
Table Offset Register will be programmed to point to the vector table at the
Hwi.vectorTableAddress.

is created and placed in the \".resetVecs\" section.

                `,
                displayFormat: { radix: "hex", bitSize: 32 },
                hidden: true,
                default: bigInt(Settings.defaultResetVectorAddress)
            },
            {
                /* provided for script compatibiility */
                name: "vectorTableAddress",
                displayName: "Runtime Vector Table Address",
                description: `Location of the Runtime Interrupt Vector Table.
                Default is device dependent.`,
                longDescription: `
This parameter allows the user to override the default placement of the
runtime interrupt vector table. The NVIC's Vector Table Offset Register
(VTOR) is also programmed to this value.

Some systems require the runtime vector table to be placed at an address
other than 0 but still need a copy of the two V8M boot vectors (SP and reset
PC), located there. To achieve this, a separate parameter resetVectorAdress
is provided. If the resetVectorAddress has a different value then the
vectorTableAddress then a separate vector table is generated and placed at
that address.

The vector table must be placed at an address at or lower than 0x3FFFFC00 and
must be aligned on an even 64 word boundary.

                `,
                displayFormat: { radix: "hex", bitSize: 32 },
                hidden: true,
                default: bigInt(Settings.defaultVectorTableAddress)
            },
            {
                /* provided for script compatibiility, always enabled */
                name: "dispatcherIrpTrackingSupport",
                hidden: true,
                default: true
            },
            {
                name: "coreVectors",
                displayName: "Core Exception Handlers",
                config: [
                    {
                        name: "resetFunc",
                        description: `Reset Handler (ID/vector #1). Default
                        is c_int00`,
                        displayName: "Reset Handler (vector #1)",
                        default: "_c_int00"
                    },
                    {
                        name: "nmiFunc",
                        description: `NMI Handler (ID/vector #2). Default is
                        set to an internal exception handler`,
                        displayName: "NMI Handler (vector #2)",
                        default: "Hwi_excHandlerAsm"
                    },
                    {
                        name: "hardFaultFunc",
                        description: `Hard Fault Handler (ID/vector #3).
                        Default is set to an internal exception handler`,
                        displayName: "Hard Fault Handler (vector #3)",
                        default: "Hwi_excHandlerAsm"
                    },
                    {
                        name: "memFaultFunc",
                        description: `Mem Fault Handler (ID/vector #4).
                        Default is set to an internal exception handler`,
                        displayName: "Mem Fault Handler (vector #4)",
                        default: "Hwi_excHandlerAsm"
                    },
                    {
                        name: "busFaultFunc",
                        description: `Bus Fault Handler (ID/vector #5).
                        Default is set to an internal exception handler`,
                        displayName: "Bus Fault Handler (vector #5)",
                        default: "Hwi_excHandlerAsm"
                    },
                    {
                        name: "usageFaultFunc",
                        description: `Usage Fault Handler (ID/vector #6).
                        Default is set to an internal exception handler`,
                        displayName: "Usage Fault Handler (vector #6)",
                        default: "Hwi_excHandlerAsm"
                    },
                    {
                        name: "svCallFunc",
                        description: `SVCall Handler (ID/vector #11). Default
                        is set to an internal exception handler`,
                        displayName: "SVCall Handler (vector #11)",
                        default: "Hwi_excHandlerAsm"
                    },
                    {
                        name: "debugMonFunc",
                        description: `Debug Mon Handler (ID/vector #12).
                        Default is set to an internal exception handler`,
                        displayName: "Debug Mon Handler (vector #12)",
                        default: "Hwi_excHandlerAsm"
                    },
                    {
                        name: "reservedFunc",
                        description: `Reserved Exception Handler (ID/vector
                        #13). Default is set to an internal exception
                        handler`,
                        displayName: "Reserved Vectors Handler",
                        default: "Hwi_excHandlerAsm"
                    }
                ]
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/family/arm/v8m/Hwi_config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt": "/ti/sysbios/family/arm/v8m/Hwi.rov.js"
    },
    getCFiles: () => { return ["ti/sysbios/family/arm/v8m/Hwi.c"] }
};

