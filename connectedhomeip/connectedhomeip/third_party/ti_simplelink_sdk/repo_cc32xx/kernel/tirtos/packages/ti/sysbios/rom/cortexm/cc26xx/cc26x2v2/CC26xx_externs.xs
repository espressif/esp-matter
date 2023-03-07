/*
 * Copyright (c) 2017, Texas Instruments Incorporated
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
 * functions referenced in the ROM but not in the ROM
 */

var ROM = xdc.useModule('ti.sysbios.rom.ROM');

/* Group function pointers together into a struct and generate xdcRomFuncsPtr */
ROM.groupFuncPtrs = true;

//ROM.excludeIncludes.$add("string.h");
ROM.excludeIncludes.$add("xdc/runtime/System.h");
ROM.excludeIncludes.$add("xdc/runtime/Startup.h");
ROM.excludeIncludes.$add("ti/sysbios/family/arm/cc26xx/Timer.h");
ROM.excludeIncludes.$add("ti/sysbios/family/arm/cc26xx/TimestampProvider.h");

/* Big.c funcs */
ROM.excludeFuncs.$add({generated: true, type: "xdc_Int", name: "xdc_runtime_Startup_getState__I", args: "xdc_runtime_Types_ModuleId"});
ROM.excludeFuncs.$add({generated: true, type: "xdc_Bool", name: "xdc_runtime_System_SupportProxy_Module__startupDone__S", args: "xdc_Void"});
ROM.excludeFuncs.$add({generated: true, type: "xdc_Void", name: "xdc_runtime_System_SupportProxy_abort__E", args: "xdc_CString"});
ROM.excludeFuncs.$add({generated: true, type: "xdc_Void", name: "xdc_runtime_System_SupportProxy_exit__E", args: " xdc_Int"});
ROM.excludeFuncs.$add({generated: true, type: "xdc_Void", name: "xdc_runtime_System_SupportProxy_flush__E", args: "xdc_Void"});
ROM.excludeFuncs.$add({generated: true, type: "xdc_Void", name: "xdc_runtime_System_SupportProxy_putch__E", args: " xdc_Char"});
ROM.excludeFuncs.$add({generated: true, type: "xdc_Bool", name: "xdc_runtime_System_SupportProxy_ready__E", args: "xdc_Void"});

/* troublesome APIs explicitly excluded from ROM */
ROM.excludeFuncs.$add({generated: false, type: "xdc_Void", name: "ti_sysbios_family_arm_cc26xx_Timer_start__E", args: "ti_sysbios_family_arm_cc26xx_Timer_Handle"}); /* calls driverlib inlined funcs */
ROM.excludeFuncs.$add({generated: false, type: "xdc_Void", name: "ti_sysbios_family_arm_cc26xx_Timer_initDevice__I", args: "ti_sysbios_family_arm_cc26xx_Timer_Object *"}); /* calls driverlib inlined funcs */
ROM.excludeFuncs.$add({generated: false, type: "xdc_Void", name: "ti_sysbios_family_arm_cc26xx_Timer_setThreshold__I", args: "ti_sysbios_family_arm_cc26xx_Timer_Object *, xdc_UInt32, xdc_Bool"}); /* is always undergoing repairs */
ROM.excludeFuncs.$add({generated: false, type: "xdc_Void", name: "ti_sysbios_family_arm_cc26xx_Timer_setNextTick__E", args: "ti_sysbios_family_arm_cc26xx_Timer_Handle, xdc_UInt32"}); /* calls rts lib uldiv */
ROM.excludeFuncs.$add({generated: false, type: "xdc_UInt32", name: "ti_sysbios_family_arm_cc26xx_Timer_getMaxTicks__E", args: "ti_sysbios_family_arm_cc26xx_Timer_Handle"}); /* calls rts lib uldiv */
ROM.excludeFuncs.$add({generated: false, type: "xdc_UInt64", name: "ti_sysbios_family_arm_cc26xx_Timer_getCount64__E", args: "ti_sysbios_family_arm_cc26xx_Timer_Object *"}); /* calls rts lib uldiv */
ROM.excludeFuncs.$add({generated: false, type: "xdc_Void", name: "ti_sysbios_family_arm_cc26xx_Timer_dynamicStub__E", args: "xdc_UArg"}); /* calls rts lib uldiv */
ROM.excludeFuncs.$add({generated: false, type: "xdc_UInt32", name: "ti_sysbios_family_arm_cc26xx_Timer_getCurrentTick__E", args: "ti_sysbios_family_arm_cc26xx_Timer_Handle, xdc_Bool"}); /* calls rts lib uldiv */
ROM.excludeFuncs.$add({generated: false, type: "xdc_UInt32", name: "ti_sysbios_family_arm_cc26xx_TimestampProvider_get32__E", args: "xdc_Void"}); /* calls rts lib uldiv */

ROM.excludeFuncs.$add({generated: false, type: "xdc_Void", name: "xdc_runtime_Error_init__E", args: "xdc_runtime_Error_Block *"}); /* references unspec'd xdc_runtime_Error_IgnoreBlock */
ROM.excludeFuncs.$add({generated: false, type: "xdc_Char *", name: "xdc_runtime_System_formatNum__I", args: "xdc_Char *, xdc_UInt32, xdc_Int, xdc_Int"}); /* calls rts lib uldiv */
