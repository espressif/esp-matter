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
 * ======== Swi_defs.h ========
 */

/* @cond LONG_NAMES */

#ifdef ti_sysbios_knl_Swi_long_names

#define Swi_Instance ti_sysbios_knl_Swi_Instance
#define Swi_Handle ti_sysbios_knl_Swi_Handle
#define Swi_Module ti_sysbios_knl_Swi_Module
#define Swi_Object ti_sysbios_knl_Swi_Object
#define Swi_Struct ti_sysbios_knl_Swi_Struct
#define Swi_FuncPtr ti_sysbios_knl_Swi_FuncPtr
#define Swi_HookSet ti_sysbios_knl_Swi_HookSet
#define Swi_Struct2 ti_sysbios_knl_Swi_Struct2
#define Swi_Instance_State ti_sysbios_knl_Swi_Instance_State
#define Swi_Module_State ti_sysbios_knl_Swi_Module_State
#define Swi_numPriorities ti_sysbios_knl_Swi_numPriorities
#define Swi_hooks ti_sysbios_knl_Swi_hooks
#define Swi_numConstructedSwis ti_sysbios_knl_Swi_numConstructedSwis
#define Swi_Params ti_sysbios_knl_Swi_Params
#define Swi_construct2 ti_sysbios_knl_Swi_construct2
#define Swi_startup ti_sysbios_knl_Swi_startup
#define Swi_enabled ti_sysbios_knl_Swi_enabled
#define Swi_unlockSched ti_sysbios_knl_Swi_unlockSched
#define Swi_disable ti_sysbios_knl_Swi_disable
#define Swi_enable ti_sysbios_knl_Swi_enable
#define Swi_restore ti_sysbios_knl_Swi_restore
#define Swi_restoreHwi ti_sysbios_knl_Swi_restoreHwi
#define Swi_self ti_sysbios_knl_Swi_self
#define Swi_getTrigger ti_sysbios_knl_Swi_getTrigger
#define Swi_raisePri ti_sysbios_knl_Swi_raisePri
#define Swi_restorePri ti_sysbios_knl_Swi_restorePri
#define Swi_andn ti_sysbios_knl_Swi_andn
#define Swi_dec ti_sysbios_knl_Swi_dec
#define Swi_getHookContext ti_sysbios_knl_Swi_getHookContext
#define Swi_setHookContext ti_sysbios_knl_Swi_setHookContext
#define Swi_getPri ti_sysbios_knl_Swi_getPri
#define Swi_getFunc ti_sysbios_knl_Swi_getFunc
#define Swi_getAttrs ti_sysbios_knl_Swi_getAttrs
#define Swi_setAttrs ti_sysbios_knl_Swi_setAttrs
#define Swi_setPri ti_sysbios_knl_Swi_setPri
#define Swi_inc ti_sysbios_knl_Swi_inc
#define Swi_or ti_sysbios_knl_Swi_or
#define Swi_post ti_sysbios_knl_Swi_post
#define Swi_Module_startup ti_sysbios_knl_Swi_Module_startup
#define Swi_Module_startupDone ti_sysbios_knl_Swi_Module_startupDone
#define Swi_construct ti_sysbios_knl_Swi_construct
#define Swi_create ti_sysbios_knl_Swi_create
#define Swi_handle ti_sysbios_knl_Swi_handle
#define Swi_struct ti_sysbios_knl_Swi_struct
#define Swi_Handle_label ti_sysbios_knl_Swi_Handle_label
#define Swi_Handle_name ti_sysbios_knl_Swi_Handle_name
#define Swi_Instance_init ti_sysbios_knl_Swi_Instance_init
#define Swi_Object_count ti_sysbios_knl_Swi_Object_count
#define Swi_Object_get ti_sysbios_knl_Swi_Object_get
#define Swi_Object_first ti_sysbios_knl_Swi_Object_first
#define Swi_Object_next ti_sysbios_knl_Swi_Object_next
#define Swi_Object_sizeof ti_sysbios_knl_Swi_Object_sizeof
#define Swi_Params_init ti_sysbios_knl_Swi_Params_init
#define Swi_Instance_finalize ti_sysbios_knl_Swi_Instance_finalize
#define Swi_delete ti_sysbios_knl_Swi_delete
#define Swi_destruct ti_sysbios_knl_Swi_destruct
#define Swi_init ti_sysbios_knl_Swi_init
#define Swi_Module_state ti_sysbios_knl_Swi_Module_state
#define Swi_Params_default ti_sysbios_knl_Swi_Params_default
#define Swi_postInit ti_sysbios_knl_Swi_postInit
#define Swi_readyQs ti_sysbios_knl_Swi_readyQs
#define Swi_run ti_sysbios_knl_Swi_run
#define Swi_runLoop ti_sysbios_knl_Swi_runLoop
#define Swi_schedule ti_sysbios_knl_Swi_schedule
#define Swi_getName ti_sysbios_knl_Swi_getName
#define Swi_getSwiFromObjElem ti_sysbios_knl_Swi_getSwiFromObjElem

#endif

#if defined(ti_sysbios_knl_Swi__nolocalnames) && !defined(ti_sysbios_knl_Swi_long_names)

#undef Swi_Instance
#undef Swi_Handle
#undef Swi_Module
#undef Swi_Object
#undef Swi_Struct
#undef Swi_FuncPtr
#undef Swi_HookSet
#undef Swi_Struct2
#undef Swi_Instance_State
#undef Swi_Module_State
#undef Swi_numPriorities
#undef Swi_hooks
#undef Swi_numConstructedSwis
#undef Swi_Params
#undef Swi_construct2
#undef Swi_startup
#undef Swi_enabled
#undef Swi_unlockSched
#undef Swi_disable
#undef Swi_enable
#undef Swi_restore
#undef Swi_restoreHwi
#undef Swi_self
#undef Swi_getTrigger
#undef Swi_raisePri
#undef Swi_restorePri
#undef Swi_andn
#undef Swi_dec
#undef Swi_getHookContext
#undef Swi_setHookContext
#undef Swi_getPri
#undef Swi_getFunc
#undef Swi_getAttrs
#undef Swi_setAttrs
#undef Swi_setPri
#undef Swi_inc
#undef Swi_or
#undef Swi_post
#undef Swi_Module_startup
#undef Swi_Module_startupDone
#undef Swi_construct
#undef Swi_create
#undef Swi_handle
#undef Swi_struct
#undef Swi_Handle_label
#undef Swi_Handle_name
#undef Swi_Instance_init
#undef Swi_Object_count
#undef Swi_Object_get
#undef Swi_Object_first
#undef Swi_Object_next
#undef Swi_Object_sizeof
#undef Swi_Params_init
#undef Swi_Instance_finalize
#undef Swi_delete
#undef Swi_destruct
#undef Swi_init
#undef Swi_Module_state
#undef Swi_Params_default
#undef Swi_postInit
#undef Swi_readyQs
#undef Swi_run
#undef Swi_runLoop
#undef Swi_schedule
#undef Swi_getName
#undef Swi_getSwiFromObjElem

#endif

/* @endcond */
