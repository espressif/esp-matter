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

/* @cond LONG_NAMES */

/*
 * ======== GateHwi_defs.h ========
 */

#ifdef ti_sysbios_utils_Load_long_names

#define Load_FuncPtr ti_sysbios_utils_Load_FuncPtr
#define Load_Stat ti_sysbios_utils_Load_Stat
#define Load_HookContext ti_sysbios_utils_Load_HookContext
#define Load_Module_State ti_sysbios_utils_Load_Module_State
#define Load_LS_cpuLoad ti_sysbios_utils_Load_LS_cpuLoad
#define Load_LS_hwiLoad ti_sysbios_utils_Load_LS_hwiLoad
#define Load_LS_swiLoad ti_sysbios_utils_Load_LS_swiLoad
#define Load_LS_taskLoad ti_sysbios_utils_Load_LS_taskLoad
#define Load_postUpdate ti_sysbios_utils_Load_postUpdate
#define Load_updateInIdle ti_sysbios_utils_Load_updateInIdle
#define Load_windowInMs ti_sysbios_utils_Load_windowInMs
#define Load_hwiEnabled ti_sysbios_utils_Load_hwiEnabled
#define Load_swiEnabled ti_sysbios_utils_Load_swiEnabled
#define Load_taskEnabled ti_sysbios_utils_Load_taskEnabled
#define Load_autoAddTasks ti_sysbios_utils_Load_autoAddTasks
#define Load_Module_State_taskList ti_sysbios_utils_Load_Module_State_taskList
#define Load_getTaskLoad ti_sysbios_utils_Load_getTaskLoad
#define Load_update ti_sysbios_utils_Load_update
#define Load_updateCPULoad ti_sysbios_utils_Load_updateCPULoad
#define Load_updateLoads ti_sysbios_utils_Load_updateLoads
#define Load_updateContextsAndPost ti_sysbios_utils_Load_updateContextsAndPost
#define Load_updateCurrentThreadTime ti_sysbios_utils_Load_updateCurrentThreadTime
#define Load_updateThreadContexts ti_sysbios_utils_Load_updateThreadContexts
#define Load_reset ti_sysbios_utils_Load_reset
#define Load_getGlobalSwiLoad ti_sysbios_utils_Load_getGlobalSwiLoad
#define Load_getGlobalHwiLoad ti_sysbios_utils_Load_getGlobalHwiLoad
#define Load_getCPULoad ti_sysbios_utils_Load_getCPULoad
#define Load_calculateLoad ti_sysbios_utils_Load_calculateLoad
#define Load_setMinIdle ti_sysbios_utils_Load_setMinIdle
#define Load_addTask ti_sysbios_utils_Load_addTask
#define Load_removeTask ti_sysbios_utils_Load_removeTask
#define Load_idleFxn ti_sysbios_utils_Load_idleFxn
#define Load_idleFxnPwr ti_sysbios_utils_Load_idleFxnPwr
#define Load_startup ti_sysbios_utils_Load_startup
#define Load_taskCreateHook ti_sysbios_utils_Load_taskCreateHook
#define Load_taskDeleteHook ti_sysbios_utils_Load_taskDeleteHook
#define Load_taskSwitchHook ti_sysbios_utils_Load_taskSwitchHook
#define Load_swiBeginHook ti_sysbios_utils_Load_swiBeginHook
#define Load_swiEndHook ti_sysbios_utils_Load_swiEndHook
#define Load_hwiBeginHook ti_sysbios_utils_Load_hwiBeginHook
#define Load_hwiEndHook ti_sysbios_utils_Load_hwiEndHook
#define Load_taskRegHook ti_sysbios_utils_Load_taskRegHook
#define Load_Module_name ti_sysbios_utils_Load_Module_name
#define Load_Module_id ti_sysbios_utils_Load_Module_id
#define Load_Module_startup ti_sysbios_utils_Load_Module_startup
#define Load_Module_startupDone ti_sysbios_utils_Load_Module_startupDone
#define Load_Module_hasMask ti_sysbios_utils_Load_Module_hasMask
#define Load_Module_getMask ti_sysbios_utils_Load_Module_getMask
#define Load_Module_setMask ti_sysbios_utils_Load_Module_setMask
#define Load_Object_heap ti_sysbios_utils_Load_Object_heap
#define Load_Module_heap ti_sysbios_utils_Load_Module_heap
#define Load_init ti_sysbios_utils_Load_init
#define Load_logCPULoad ti_sysbios_utils_Load_logCPULoad
#define Load_logLoads ti_sysbios_utils_Load_logLoads
#define Load_Module_state ti_sysbios_utils_Load_Module_state

#endif /* ti_sysbios_utils_Load_long_names */

#if defined(ti_sysbios_utils_Load__nolocalnames) && !defined(ti_sysbios_utils_Load_long_names)

#undef Load_FuncPtr
#undef Load_Stat
#undef Load_HookContext
#undef Load_Module_State
#undef Load_LS_cpuLoad
#undef Load_LS_hwiLoad
#undef Load_LS_swiLoad
#undef Load_LS_taskLoad
#undef Load_postUpdate
#undef Load_updateInIdle
#undef Load_windowInMs
#undef Load_hwiEnabled
#undef Load_swiEnabled
#undef Load_taskEnabled
#undef Load_autoAddTasks
#undef Load_Module_State_taskList
#undef Load_getTaskLoad
#undef Load_update
#undef Load_updateCPULoad
#undef Load_updateLoads
#undef Load_updateContextsAndPost
#undef Load_updateCurrentThreadTime
#undef Load_updateThreadContexts
#undef Load_reset
#undef Load_getGlobalSwiLoad
#undef Load_getGlobalHwiLoad
#undef Load_getCPULoad
#undef Load_calculateLoad
#undef Load_setMinIdle
#undef Load_addTask
#undef Load_removeTask
#undef Load_idleFxn
#undef Load_idleFxnPwr
#undef Load_startup
#undef Load_taskCreateHook
#undef Load_taskDeleteHook
#undef Load_taskSwitchHook
#undef Load_swiBeginHook
#undef Load_swiEndHook
#undef Load_hwiBeginHook
#undef Load_hwiEndHook
#undef Load_taskRegHook
#undef Load_Module_name
#undef Load_Module_id
#undef Load_Module_startup
#undef Load_Module_startupDone
#undef Load_Module_hasMask
#undef Load_Module_getMask
#undef Load_Module_setMask
#undef Load_Object_heap
#undef Load_Module_heap
#undef Load_init
#undef Load_logCPULoad
#undef Load_logLoads
#undef Load_Module_state

#endif

/* @endcond */
