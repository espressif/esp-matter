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
 * ======== GateTask_defs.h ========
 */

#ifdef ti_sysbios_gates_GateTask_long_names

#define GateTask_Instance ti_sysbios_gates_GateTask_Instance
#define GateTask_Handle ti_sysbios_gates_GateTask_Handle
#define GateTask_Module ti_sysbios_gates_GateTask_Module
#define GateTask_Object ti_sysbios_gates_GateTask_Object
#define GateTask_Struct ti_sysbios_gates_GateTask_Struct
#define GateTask_Q_BLOCKING ti_sysbios_gates_GateTask_Q_BLOCKING
#define GateTask_Q_PREEMPTING ti_sysbios_gates_GateTask_Q_PREEMPTING
#define GateTask_Instance_State ti_sysbios_gates_GateTask_Instance_State
#define GateTask_Params ti_sysbios_gates_GateTask_Params
#define GateTask_query ti_sysbios_gates_GateTask_query
#define GateTask_enter ti_sysbios_gates_GateTask_enter
#define GateTask_leave ti_sysbios_gates_GateTask_leave
#define GateTask_Module_name ti_sysbios_gates_GateTask_Module_name
#define GateTask_Module_id ti_sysbios_gates_GateTask_Module_id
#define GateTask_Module_startup ti_sysbios_gates_GateTask_Module_startup
#define GateTask_Module_startupDone ti_sysbios_gates_GateTask_Module_startupDone
#define GateTask_Module_hasMask ti_sysbios_gates_GateTask_Module_hasMask
#define GateTask_Module_getMask ti_sysbios_gates_GateTask_Module_getMask
#define GateTask_Module_setMask ti_sysbios_gates_GateTask_Module_setMask
#define GateTask_Object_heap ti_sysbios_gates_GateTask_Object_heap
#define GateTask_Module_heap ti_sysbios_gates_GateTask_Module_heap
#define GateTask_construct ti_sysbios_gates_GateTask_construct
#define GateTask_create ti_sysbios_gates_GateTask_create
#define GateTask_handle ti_sysbios_gates_GateTask_handle
#define GateTask_struct ti_sysbios_gates_GateTask_struct
#define GateTask_Handle_label ti_sysbios_gates_GateTask_Handle_label
#define GateTask_Handle_name ti_sysbios_gates_GateTask_Handle_name
#define GateTask_Instance_init ti_sysbios_gates_GateTask_Instance_init
#define GateTask_Object_count ti_sysbios_gates_GateTask_Object_count
#define GateTask_Object_get ti_sysbios_gates_GateTask_Object_get
#define GateTask_Object_first ti_sysbios_gates_GateTask_Object_first
#define GateTask_Object_next ti_sysbios_gates_GateTask_Object_next
#define GateTask_Object_sizeof ti_sysbios_gates_GateTask_Object_sizeof
#define GateTask_Params_copy ti_sysbios_gates_GateTask_Params_copy
#define GateTask_Params_init ti_sysbios_gates_GateTask_Params_init
#define GateTask_delete ti_sysbios_gates_GateTask_delete
#define GateTask_destruct ti_sysbios_gates_GateTask_destruct
#define GateTask_Module_upCast ti_sysbios_gates_GateTask_Module_upCast
#define GateTask_Module_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateTask_Module_to_xdc_runtime_IGateProvider
#define GateTask_Handle_upCast ti_sysbios_gates_GateTask_Handle_upCast
#define GateTask_Handle_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateTask_Handle_to_xdc_runtime_IGateProvider
#define GateTask_Handle_downCast ti_sysbios_gates_GateTask_Handle_downCast
#define GateTask_Handle_from_xdc_runtime_IGateProvider ti_sysbios_gates_GateTask_Handle_from_xdc_runtime_IGateProvider
#define GateTask_canBePreempted ti_sysbios_gates_GateTask_canBePreempted
#define GateTask_canBlock ti_sysbios_gates_GateTask_canBlock
#define GateTask_object ti_sysbios_gates_GateTask_object
#define GateTask_Params_default ti_sysbios_gates_GateTask_Params_default

#endif /* ti_sysbios_gates_GateTask_long_names */

#if defined(ti_sysbios_gates_GateTask__nolocalnames) && !defined(ti_sysbios_gates_GateTask_long_names)

#undef GateTask_Instance
#undef GateTask_Handle
#undef GateTask_Module
#undef GateTask_Object
#undef GateTask_Struct
#undef GateTask_Q_BLOCKING
#undef GateTask_Q_PREEMPTING
#undef GateTask_Instance_State
#undef GateTask_Params
#undef GateTask_query
#undef GateTask_enter
#undef GateTask_leave
#undef GateTask_Module_name
#undef GateTask_Module_id
#undef GateTask_Module_startup
#undef GateTask_Module_startupDone
#undef GateTask_Module_hasMask
#undef GateTask_Module_getMask
#undef GateTask_Module_setMask
#undef GateTask_Object_heap
#undef GateTask_Module_heap
#undef GateTask_construct
#undef GateTask_create
#undef GateTask_handle
#undef GateTask_struct
#undef GateTask_Handle_label
#undef GateTask_Handle_name
#undef GateTask_Instance_init
#undef GateTask_Object_count
#undef GateTask_Object_get
#undef GateTask_Object_first
#undef GateTask_Object_next
#undef GateTask_Object_sizeof
#undef GateTask_Params_copy
#undef GateTask_Params_init
#undef GateTask_delete
#undef GateTask_destruct
#undef GateTask_Module_upCast
#undef GateTask_Module_to_xdc_runtime_IGateProvider
#undef GateTask_Handle_upCast
#undef GateTask_Handle_to_xdc_runtime_IGateProvider
#undef GateTask_Handle_downCast
#undef GateTask_Handle_from_xdc_runtime_IGateProvider
#undef GateTask_canBePreempted
#undef GateTask_canBlock
#undef GateTask_object
#undef GateTask_Params_default

#endif

/* @endcond */
