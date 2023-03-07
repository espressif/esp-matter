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
 * ======== GateMutexPri_defs.h ========
 */

#ifdef ti_sysbios_gates_GateMutexPri_long_names

#define GateMutexPri_Instance ti_sysbios_gates_GateMutexPri_Instance
#define GateMutexPri_Handle ti_sysbios_gates_GateMutexPri_Handle
#define GateMutexPri_Module ti_sysbios_gates_GateMutexPri_Module
#define GateMutexPri_Object ti_sysbios_gates_GateMutexPri_Object
#define GateMutexPri_Struct ti_sysbios_gates_GateMutexPri_Struct
#define GateMutexPri_Q_BLOCKING ti_sysbios_gates_GateMutexPri_Q_BLOCKING
#define GateMutexPri_Q_PREEMPTING ti_sysbios_gates_GateMutexPri_Q_PREEMPTING
#define GateMutexPri_Instance_State ti_sysbios_gates_GateMutexPri_Instance_State
#define GateMutexPri_Instance_State_pendQ ti_sysbios_gates_GateMutexPri_Instance_State_pendQ
#define GateMutexPri_Params ti_sysbios_gates_GateMutexPri_Params
#define GateMutexPri_query ti_sysbios_gates_GateMutexPri_query
#define GateMutexPri_enter ti_sysbios_gates_GateMutexPri_enter
#define GateMutexPri_leave ti_sysbios_gates_GateMutexPri_leave
#define GateMutexPri_Module_name ti_sysbios_gates_GateMutexPri_Module_name
#define GateMutexPri_Module_id ti_sysbios_gates_GateMutexPri_Module_id
#define GateMutexPri_Module_startup ti_sysbios_gates_GateMutexPri_Module_startup
#define GateMutexPri_Module_startupDone ti_sysbios_gates_GateMutexPri_Module_startupDone
#define GateMutexPri_Module_hasMask ti_sysbios_gates_GateMutexPri_Module_hasMask
#define GateMutexPri_Module_getMask ti_sysbios_gates_GateMutexPri_Module_getMask
#define GateMutexPri_Module_setMask ti_sysbios_gates_GateMutexPri_Module_setMask
#define GateMutexPri_Object_heap ti_sysbios_gates_GateMutexPri_Object_heap
#define GateMutexPri_Module_heap ti_sysbios_gates_GateMutexPri_Module_heap
#define GateMutexPri_construct ti_sysbios_gates_GateMutexPri_construct
#define GateMutexPri_create ti_sysbios_gates_GateMutexPri_create
#define GateMutexPri_handle ti_sysbios_gates_GateMutexPri_handle
#define GateMutexPri_struct ti_sysbios_gates_GateMutexPri_struct
#define GateMutexPri_Handle_label ti_sysbios_gates_GateMutexPri_Handle_label
#define GateMutexPri_Handle_name ti_sysbios_gates_GateMutexPri_Handle_name
#define GateMutexPri_Instance_init ti_sysbios_gates_GateMutexPri_Instance_init
#define GateMutexPri_Object_count ti_sysbios_gates_GateMutexPri_Object_count
#define GateMutexPri_Object_get ti_sysbios_gates_GateMutexPri_Object_get
#define GateMutexPri_Object_first ti_sysbios_gates_GateMutexPri_Object_first
#define GateMutexPri_Object_next ti_sysbios_gates_GateMutexPri_Object_next
#define GateMutexPri_Object_sizeof ti_sysbios_gates_GateMutexPri_Object_sizeof
#define GateMutexPri_Params_copy ti_sysbios_gates_GateMutexPri_Params_copy
#define GateMutexPri_Params_init ti_sysbios_gates_GateMutexPri_Params_init
#define GateMutexPri_Instance_finalize ti_sysbios_gates_GateMutexPri_Instance_finalize
#define GateMutexPri_delete ti_sysbios_gates_GateMutexPri_delete
#define GateMutexPri_destruct ti_sysbios_gates_GateMutexPri_destruct
#define GateMutexPri_Module_upCast ti_sysbios_gates_GateMutexPri_Module_upCast
#define GateMutexPri_Module_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateMutexPri_Module_to_xdc_runtime_IGateProvider
#define GateMutexPri_Handle_upCast ti_sysbios_gates_GateMutexPri_Handle_upCast
#define GateMutexPri_Handle_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateMutexPri_Handle_to_xdc_runtime_IGateProvider
#define GateMutexPri_Handle_downCast ti_sysbios_gates_GateMutexPri_Handle_downCast
#define GateMutexPri_Handle_from_xdc_runtime_IGateProvider ti_sysbios_gates_GateMutexPri_Handle_from_xdc_runtime_IGateProvider
#define GateMutexPri_canBePreempted ti_sysbios_gates_GateMutexPri_canBePreempted
#define GateMutexPri_canBlock ti_sysbios_gates_GateMutexPri_canBlock
#define GateMutexPri_object ti_sysbios_gates_GateMutexPri_object
#define GateMutexPri_Params_default ti_sysbios_gates_GateMutexPri_Params_default
#define GateMutexPri_getGateMutexPriFromObjElem ti_sysbios_gates_GateMutexPri_getGateMutexPriFromObjElem
#define GateMutexPri_insertPri ti_sysbios_gates_GateMutexPri_insertPri
#define GateMutexPri_Module_state ti_sysbios_gates_GateMutexPri_Module_state

#endif /* ti_sysbios_gates_GateMutexPri_long_names */

#if defined(ti_sysbios_gates_GateMutexPri__nolocalnames) && !defined(ti_sysbios_gates_GateMutexPri_long_names)

#undef GateMutexPri_Instance
#undef GateMutexPri_Handle
#undef GateMutexPri_Module
#undef GateMutexPri_Object
#undef GateMutexPri_Struct
#undef GateMutexPri_Q_BLOCKING
#undef GateMutexPri_Q_PREEMPTING
#undef GateMutexPri_Instance_State
#undef GateMutexPri_Instance_State_pendQ
#undef GateMutexPri_Params
#undef GateMutexPri_query
#undef GateMutexPri_enter
#undef GateMutexPri_leave
#undef GateMutexPri_Module_name
#undef GateMutexPri_Module_id
#undef GateMutexPri_Module_startup
#undef GateMutexPri_Module_startupDone
#undef GateMutexPri_Module_hasMask
#undef GateMutexPri_Module_getMask
#undef GateMutexPri_Module_setMask
#undef GateMutexPri_Object_heap
#undef GateMutexPri_Module_heap
#undef GateMutexPri_construct
#undef GateMutexPri_create
#undef GateMutexPri_handle
#undef GateMutexPri_struct
#undef GateMutexPri_Handle_label
#undef GateMutexPri_Handle_name
#undef GateMutexPri_Instance_init
#undef GateMutexPri_Object_count
#undef GateMutexPri_Object_get
#undef GateMutexPri_Object_first
#undef GateMutexPri_Object_next
#undef GateMutexPri_Object_sizeof
#undef GateMutexPri_Params_copy
#undef GateMutexPri_Params_init
#undef GateMutexPri_Instance_finalize
#undef GateMutexPri_delete
#undef GateMutexPri_destruct
#undef GateMutexPri_Module_upCast
#undef GateMutexPri_Module_to_xdc_runtime_IGateProvider
#undef GateMutexPri_Handle_upCast
#undef GateMutexPri_Handle_to_xdc_runtime_IGateProvider
#undef GateMutexPri_Handle_downCast
#undef GateMutexPri_Handle_from_xdc_runtime_IGateProvider
#undef GateMutexPri_canBePreempted
#undef GateMutexPri_canBlock
#undef GateMutexPri_object
#undef GateMutexPri_Params_default
#undef GateMutexPri_getGateMutexPriFromObjElem
#undef GateMutexPri_insertPri
#undef GateMutexPri_Module_state

#endif

/* @endcond */
