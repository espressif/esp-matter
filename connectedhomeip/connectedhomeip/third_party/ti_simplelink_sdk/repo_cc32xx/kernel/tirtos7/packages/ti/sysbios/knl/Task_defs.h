/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 * ======== Task_defs.h ========
 */

/* @cond LONG_NAMES */

#ifdef ti_sysbios_knl_Task_long_names

#define Task_Instance ti_sysbios_knl_Task_Instance
#define Task_Handle ti_sysbios_knl_Task_Handle
#define Task_Module ti_sysbios_knl_Task_Module
#define Task_Object ti_sysbios_knl_Task_Object
#define Task_Struct ti_sysbios_knl_Task_Struct
#define Task_FuncPtr ti_sysbios_knl_Task_FuncPtr
#define Task_AllBlockedFuncPtr ti_sysbios_knl_Task_AllBlockedFuncPtr
#define Task_ModStateCheckValueFuncPtr ti_sysbios_knl_Task_ModStateCheckValueFuncPtr
#define Task_ModStateCheckFuncPtr ti_sysbios_knl_Task_ModStateCheckFuncPtr
#define Task_ObjectCheckValueFuncPtr ti_sysbios_knl_Task_ObjectCheckValueFuncPtr
#define Task_ObjectCheckFuncPtr ti_sysbios_knl_Task_ObjectCheckFuncPtr
#define Task_Mode ti_sysbios_knl_Task_Mode
#define Task_Stat ti_sysbios_knl_Task_Stat
#define Task_HookSet ti_sysbios_knl_Task_HookSet
#define Task_AFFINITY_NONE ti_sysbios_knl_Task_AFFINITY_NONE
#define Task_PendElem ti_sysbios_knl_Task_PendElem
#define Task_Instance_State ti_sysbios_knl_Task_Instance_State
#define Task_Module_State ti_sysbios_knl_Task_Module_State
#define Task_RunQEntry ti_sysbios_knl_Task_RunQEntry
#define Task_Module_StateSmp ti_sysbios_knl_Task_Module_StateSmp
#define Task_Mode_RUNNING ti_sysbios_knl_Task_Mode_RUNNING
#define Task_Mode_READY ti_sysbios_knl_Task_Mode_READY
#define Task_Mode_BLOCKED ti_sysbios_knl_Task_Mode_BLOCKED
#define Task_Mode_TERMINATED ti_sysbios_knl_Task_Mode_TERMINATED
#define Task_Mode_INACTIVE ti_sysbios_knl_Task_Mode_INACTIVE
#define Task_numPriorities ti_sysbios_knl_Task_numPriorities
#define Task_defaultStackSize ti_sysbios_knl_Task_defaultStackSize
#define Task_defaultStackHeap ti_sysbios_knl_Task_defaultStackHeap
#define Task_allBlockedFunc ti_sysbios_knl_Task_allBlockedFunc
#define Task_initStackFlag ti_sysbios_knl_Task_initStackFlag
#define Task_checkStackFlag ti_sysbios_knl_Task_checkStackFlag
#define Task_deleteTerminatedTasks ti_sysbios_knl_Task_deleteTerminatedTasks
#define Task_hooks ti_sysbios_knl_Task_hooks
#define Task_moduleStateCheckFxn ti_sysbios_knl_Task_moduleStateCheckFxn
#define Task_moduleStateCheckValueFxn ti_sysbios_knl_Task_moduleStateCheckValueFxn
#define Task_moduleStateCheckValue ti_sysbios_knl_Task_moduleStateCheckValue
#define Task_moduleStateCheckFlag ti_sysbios_knl_Task_moduleStateCheckFlag
#define Task_objectCheckFxn ti_sysbios_knl_Task_objectCheckFxn
#define Task_objectCheckValueFxn ti_sysbios_knl_Task_objectCheckValueFxn
#define Task_objectCheckFlag ti_sysbios_knl_Task_objectCheckFlag
#define Task_numConstructedTasks ti_sysbios_knl_Task_numConstructedTasks
#define Task_startupHookFunc ti_sysbios_knl_Task_startupHookFunc
#define Task_Module_State_inactiveQ ti_sysbios_knl_Task_Module_State_inactiveQ
#define Task_Module_State_terminatedQ ti_sysbios_knl_Task_Module_State_terminatedQ
#define Task_Params ti_sysbios_knl_Task_Params
#define Task_startup ti_sysbios_knl_Task_startup
#define Task_enabled ti_sysbios_knl_Task_enabled
#define Task_unlockSched ti_sysbios_knl_Task_unlockSched
#define Task_disable ti_sysbios_knl_Task_disable
#define Task_enable ti_sysbios_knl_Task_enable
#define Task_restore ti_sysbios_knl_Task_restore
#define Task_restoreHwi ti_sysbios_knl_Task_restoreHwi
#define Task_self ti_sysbios_knl_Task_self
#define Task_selfMacro ti_sysbios_knl_Task_selfMacro
#define Task_checkStacks ti_sysbios_knl_Task_checkStacks
#define Task_exit ti_sysbios_knl_Task_exit
#define Task_sleep ti_sysbios_knl_Task_sleep
#define Task_yield ti_sysbios_knl_Task_yield
#define Task_getIdleTask ti_sysbios_knl_Task_getIdleTask
#define Task_getIdleTaskHandle ti_sysbios_knl_Task_getIdleTaskHandle
#define Task_startCore ti_sysbios_knl_Task_startCore
#define Task_getArg0 ti_sysbios_knl_Task_getArg0
#define Task_getArg1 ti_sysbios_knl_Task_getArg1
#define Task_getEnv ti_sysbios_knl_Task_getEnv
#define Task_getFunc ti_sysbios_knl_Task_getFunc
#define Task_getHookContext ti_sysbios_knl_Task_getHookContext
#define Task_getName ti_sysbios_knl_Task_getName
#define Task_getPri ti_sysbios_knl_Task_getPri
#define Task_setArg0 ti_sysbios_knl_Task_setArg0
#define Task_setArg1 ti_sysbios_knl_Task_setArg1
#define Task_setEnv ti_sysbios_knl_Task_setEnv
#define Task_setHookContext ti_sysbios_knl_Task_setHookContext
#define Task_setPri ti_sysbios_knl_Task_setPri
#define Task_stat ti_sysbios_knl_Task_stat
#define Task_getMode ti_sysbios_knl_Task_getMode
#define Task_setAffinity ti_sysbios_knl_Task_setAffinity
#define Task_getAffinity ti_sysbios_knl_Task_getAffinity
#define Task_block ti_sysbios_knl_Task_block
#define Task_unblock ti_sysbios_knl_Task_unblock
#define Task_blockI ti_sysbios_knl_Task_blockI
#define Task_unblockI ti_sysbios_knl_Task_unblockI
#define Task_getPrivileged ti_sysbios_knl_Task_getPrivileged
#define Task_Module_startup ti_sysbios_knl_Task_Module_startup
#define Task_Module_startupDone ti_sysbios_knl_Task_Module_startupDone
#define Task_Module_heap ti_sysbios_knl_Task_Module_heap
#define Task_construct ti_sysbios_knl_Task_construct
#define Task_create ti_sysbios_knl_Task_create
#define Task_handle ti_sysbios_knl_Task_handle
#define Task_struct ti_sysbios_knl_Task_struct
#define Task_Handle_name ti_sysbios_knl_Task_Handle_name
#define Task_Instance_init ti_sysbios_knl_Task_Instance_init
#define Task_Object_count ti_sysbios_knl_Task_Object_count
#define Task_Object_get ti_sysbios_knl_Task_Object_get
#define Task_Object_first ti_sysbios_knl_Task_Object_first
#define Task_Object_next ti_sysbios_knl_Task_Object_next
#define Task_Object_sizeof ti_sysbios_knl_Task_Object_sizeof
#define Task_Params_init ti_sysbios_knl_Task_Params_init
#define Task_Instance_finalize ti_sysbios_knl_Task_Instance_finalize
#define Task_delete ti_sysbios_knl_Task_delete
#define Task_destruct ti_sysbios_knl_Task_destruct
#define Task_allBlockedFunction ti_sysbios_knl_Task_allBlockedFunction
#define Task_enter ti_sysbios_knl_Task_enter
#define Task_init ti_sysbios_knl_Task_init
#define Task_Module_state ti_sysbios_knl_Task_Module_state
#define Task_Params_default ti_sysbios_knl_Task_Params_default
#define Task_postInit ti_sysbios_knl_Task_postInit
#define Task_processVitalTaskFlag ti_sysbios_knl_Task_processVitalTaskFlag
#define Task_readyQs ti_sysbios_knl_Task_readyQs
#define Task_schedule ti_sysbios_knl_Task_schedule
#define Task_sleepTimeout ti_sysbios_knl_Task_sleepTimeout
#define Task_deleteTerminatedTasksFunc ti_sysbios_knl_Task_deleteTerminatedTasksFunc
#define Task_getModuleStateCheckValue ti_sysbios_knl_Task_getModuleStateCheckValue
#define Task_getObjectCheckValue ti_sysbios_knl_Task_getObjectCheckValue
#define Task_moduleStateCheck ti_sysbios_knl_Task_moduleStateCheck
#define Task_objectCheck ti_sysbios_knl_Task_objectCheck
#define Task_getTaskFromObjElem ti_sysbios_knl_Task_getTaskFromObjElem

#endif

#if defined(ti_sysbios_knl_Task__nolocalnames) && !defined(ti_sysbios_knl_Task_long_names)

#undef Task_Handle
#undef Task_Module
#undef Task_Object
#undef Task_Struct
#undef Task_FuncPtr
#undef Task_AllBlockedFuncPtr
#undef Task_ModStateCheckValueFuncPtr
#undef Task_ModStateCheckFuncPtr
#undef Task_ObjectCheckValueFuncPtr
#undef Task_ObjectCheckFuncPtr
#undef Task_Mode
#undef Task_Stat
#undef Task_HookSet
#undef Task_AFFINITY_NONE
#undef Task_PendElem
#undef Task_Instance_State
#undef Task_Module_State
#undef Task_RunQEntry
#undef Task_Module_StateSmp
#undef Task_Mode_RUNNING
#undef Task_Mode_READY
#undef Task_Mode_BLOCKED
#undef Task_Mode_TERMINATED
#undef Task_Mode_INACTIVE
#undef Task_numPriorities
#undef Task_defaultStackSize
#undef Task_defaultStackHeap
#undef Task_allBlockedFunc
#undef Task_initStackFlag
#undef Task_checkStackFlag
#undef Task_deleteTerminatedTasks
#undef Task_hooks
#undef Task_moduleStateCheckFxn
#undef Task_moduleStateCheckValueFxn
#undef Task_moduleStateCheckValue
#undef Task_moduleStateCheckFlag
#undef Task_objectCheckFxn
#undef Task_objectCheckValueFxn
#undef Task_objectCheckFlag
#undef Task_numConstructedTasks
#undef Task_startupHookFunc
#undef Task_Module_State_inactiveQ
#undef Task_Module_State_terminatedQ
#undef Task_Params
#undef Task_startup
#undef Task_enabled
#undef Task_unlockSched
#undef Task_disable
#undef Task_enable
#undef Task_restore
#undef Task_restoreHwi
#undef Task_self
#undef Task_selfMacro
#undef Task_checkStacks
#undef Task_exit
#undef Task_sleep
#undef Task_yield
#undef Task_getIdleTask
#undef Task_getIdleTaskHandle
#undef Task_startCore
#undef Task_getArg0
#undef Task_getArg1
#undef Task_getEnv
#undef Task_getFunc
#undef Task_getHookContext
#undef Task_getName
#undef Task_getPri
#undef Task_setArg0
#undef Task_setArg1
#undef Task_setEnv
#undef Task_setHookContext
#undef Task_setPri
#undef Task_stat
#undef Task_getMode
#undef Task_setAffinity
#undef Task_getAffinity
#undef Task_block
#undef Task_unblock
#undef Task_blockI
#undef Task_unblockI
#undef Task_getPrivileged
#undef Task_Module_startup
#undef Task_Module_startupDone
#undef Task_Module_heap
#undef Task_construct
#undef Task_create
#undef Task_handle
#undef Task_struct
#undef Task_Handle_name
#undef Task_Instance_init
#undef Task_Object_count
#undef Task_Object_get
#undef Task_Object_first
#undef Task_Object_next
#undef Task_Object_sizeof
#undef Task_Params_init
#undef Task_Instance_finalize
#undef Task_delete
#undef Task_destruct
#undef Task_enter
#undef Task_init
#undef Task_Module_state
#undef Task_Params_default
#undef Task_postInit
#undef Task_processVitalTaskFlag
#undef Task_readyQs
#undef Task_schedule
#undef Task_sleepTimeout
#undef Task_deleteTerminatedTasksFunc
#undef Task_getModuleStateCheckValue
#undef Task_getObjectCheckValue
#undef Task_moduleStateCheck
#undef Task_objectCheck
#undef Task_getTaskFromObjElem

#endif

/* @endcond */
