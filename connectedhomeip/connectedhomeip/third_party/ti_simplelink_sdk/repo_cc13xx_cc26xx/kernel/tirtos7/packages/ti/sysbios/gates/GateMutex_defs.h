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
 * ======== GateMutex_defs.h ========
 */

#ifdef ti_sysbios_gates_GateMutex_long_names

#define GateMutex_Instance ti_sysbios_gates_GateMutex_Instance
#define GateMutex_Handle ti_sysbios_gates_GateMutex_Handle
#define GateMutex_Module ti_sysbios_gates_GateMutex_Module
#define GateMutex_Object ti_sysbios_gates_GateMutex_Object
#define GateMutex_Struct ti_sysbios_gates_GateMutex_Struct
#define GateMutex_Q_BLOCKING ti_sysbios_gates_GateMutex_Q_BLOCKING
#define GateMutex_Q_PREEMPTING ti_sysbios_gates_GateMutex_Q_PREEMPTING
#define GateMutex_Instance_State ti_sysbios_gates_GateMutex_Instance_State
#define GateMutex_Instance_State_pendQ ti_sysbios_gates_GateMutex_Instance_State_pendQ
#define GateMutex_Params ti_sysbios_gates_GateMutex_Params
#define GateMutex_query ti_sysbios_gates_GateMutex_query
#define GateMutex_enter ti_sysbios_gates_GateMutex_enter
#define GateMutex_leave ti_sysbios_gates_GateMutex_leave
#define GateMutex_Module_name ti_sysbios_gates_GateMutex_Module_name
#define GateMutex_Module_id ti_sysbios_gates_GateMutex_Module_id
#define GateMutex_Module_startup ti_sysbios_gates_GateMutex_Module_startup
#define GateMutex_Module_startupDone ti_sysbios_gates_GateMutex_Module_startupDone
#define GateMutex_Module_hasMask ti_sysbios_gates_GateMutex_Module_hasMask
#define GateMutex_Module_getMask ti_sysbios_gates_GateMutex_Module_getMask
#define GateMutex_Module_setMask ti_sysbios_gates_GateMutex_Module_setMask
#define GateMutex_Object_heap ti_sysbios_gates_GateMutex_Object_heap
#define GateMutex_Module_heap ti_sysbios_gates_GateMutex_Module_heap
#define GateMutex_construct ti_sysbios_gates_GateMutex_construct
#define GateMutex_create ti_sysbios_gates_GateMutex_create
#define GateMutex_handle ti_sysbios_gates_GateMutex_handle
#define GateMutex_struct ti_sysbios_gates_GateMutex_struct
#define GateMutex_Handle_label ti_sysbios_gates_GateMutex_Handle_label
#define GateMutex_Handle_name ti_sysbios_gates_GateMutex_Handle_name
#define GateMutex_Instance_init ti_sysbios_gates_GateMutex_Instance_init
#define GateMutex_Object_count ti_sysbios_gates_GateMutex_Object_count
#define GateMutex_Object_get ti_sysbios_gates_GateMutex_Object_get
#define GateMutex_Object_first ti_sysbios_gates_GateMutex_Object_first
#define GateMutex_Object_next ti_sysbios_gates_GateMutex_Object_next
#define GateMutex_Object_sizeof ti_sysbios_gates_GateMutex_Object_sizeof
#define GateMutex_Params_copy ti_sysbios_gates_GateMutex_Params_copy
#define GateMutex_Params_init ti_sysbios_gates_GateMutex_Params_init
#define GateMutex_Instance_finalize ti_sysbios_gates_GateMutex_Instance_finalize
#define GateMutex_delete ti_sysbios_gates_GateMutex_delete
#define GateMutex_destruct ti_sysbios_gates_GateMutex_destruct
#define GateMutex_Module_upCast ti_sysbios_gates_GateMutex_Module_upCast
#define GateMutex_Module_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateMutex_Module_to_xdc_runtime_IGateProvider
#define GateMutex_Handle_upCast ti_sysbios_gates_GateMutex_Handle_upCast
#define GateMutex_Handle_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateMutex_Handle_to_xdc_runtime_IGateProvider
#define GateMutex_Handle_downCast ti_sysbios_gates_GateMutex_Handle_downCast
#define GateMutex_Handle_from_xdc_runtime_IGateProvider ti_sysbios_gates_GateMutex_Handle_from_xdc_runtime_IGateProvider
#define GateMutex_canBePreempted ti_sysbios_gates_GateMutex_canBePreempted
#define GateMutex_canBlock ti_sysbios_gates_GateMutex_canBlock
#define GateMutex_object ti_sysbios_gates_GateMutex_object
#define GateMutex_Params_default ti_sysbios_gates_GateMutex_Params_default
#define GateMutex_getGateMutexFromObjElem ti_sysbios_gates_GateMutex_getGateMutexFromObjElem
#define GateMutex_Module_state ti_sysbios_gates_GateMutex_Module_state

#endif /* ti_sysbios_gates_GateMutex_long_names */

#if defined(ti_sysbios_gates_GateMutex__nolocalnames) && !defined(ti_sysbios_gates_GateMutex_long_names)

#undef GateMutex_Instance
#undef GateMutex_Handle
#undef GateMutex_Module
#undef GateMutex_Object
#undef GateMutex_Struct
#undef GateMutex_Q_BLOCKING
#undef GateMutex_Q_PREEMPTING
#undef GateMutex_Instance_State
#undef GateMutex_Instance_State_pendQ
#undef GateMutex_Params
#undef GateMutex_query
#undef GateMutex_enter
#undef GateMutex_leave
#undef GateMutex_Module_name
#undef GateMutex_Module_id
#undef GateMutex_Module_startup
#undef GateMutex_Module_startupDone
#undef GateMutex_Module_hasMask
#undef GateMutex_Module_getMask
#undef GateMutex_Module_setMask
#undef GateMutex_Object_heap
#undef GateMutex_Module_heap
#undef GateMutex_construct
#undef GateMutex_create
#undef GateMutex_handle
#undef GateMutex_struct
#undef GateMutex_Handle_label
#undef GateMutex_Handle_name
#undef GateMutex_Instance_init
#undef GateMutex_Object_count
#undef GateMutex_Object_get
#undef GateMutex_Object_first
#undef GateMutex_Object_next
#undef GateMutex_Object_sizeof
#undef GateMutex_Params_copy
#undef GateMutex_Params_init
#undef GateMutex_Instance_finalize
#undef GateMutex_delete
#undef GateMutex_destruct
#undef GateMutex_Module_upCast
#undef GateMutex_Module_to_xdc_runtime_IGateProvider
#undef GateMutex_Handle_upCast
#undef GateMutex_Handle_to_xdc_runtime_IGateProvider
#undef GateMutex_Handle_downCast
#undef GateMutex_Handle_from_xdc_runtime_IGateProvider
#undef GateMutex_canBePreempted
#undef GateMutex_canBlock
#undef GateMutex_object
#undef GateMutex_Params_default
#undef GateMutex_getGateMutexFromObjElem
#undef GateMutex_Module_state

#endif

/* @endcond */
