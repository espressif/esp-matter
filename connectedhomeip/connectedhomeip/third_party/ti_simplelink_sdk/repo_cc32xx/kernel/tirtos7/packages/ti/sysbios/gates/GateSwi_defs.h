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
 * ======== GateSwi_defs.h ========
 */

#ifdef ti_sysbios_gates_GateSwi_long_names

#define GateSwi_Instance ti_sysbios_gates_GateSwi_Instance
#define GateSwi_Handle ti_sysbios_gates_GateSwi_Handle
#define GateSwi_Module ti_sysbios_gates_GateSwi_Module
#define GateSwi_Object ti_sysbios_gates_GateSwi_Object
#define GateSwi_Struct ti_sysbios_gates_GateSwi_Struct
#define GateSwi_Q_BLOCKING ti_sysbios_gates_GateSwi_Q_BLOCKING
#define GateSwi_Q_PREEMPTING ti_sysbios_gates_GateSwi_Q_PREEMPTING
#define GateSwi_Instance_State ti_sysbios_gates_GateSwi_Instance_State
#define GateSwi_Params ti_sysbios_gates_GateSwi_Params
#define GateSwi_query ti_sysbios_gates_GateSwi_query
#define GateSwi_enter ti_sysbios_gates_GateSwi_enter
#define GateSwi_leave ti_sysbios_gates_GateSwi_leave
#define GateSwi_Module_name ti_sysbios_gates_GateSwi_Module_name
#define GateSwi_Module_id ti_sysbios_gates_GateSwi_Module_id
#define GateSwi_Module_startup ti_sysbios_gates_GateSwi_Module_startup
#define GateSwi_Module_startupDone ti_sysbios_gates_GateSwi_Module_startupDone
#define GateSwi_Module_hasMask ti_sysbios_gates_GateSwi_Module_hasMask
#define GateSwi_Module_getMask ti_sysbios_gates_GateSwi_Module_getMask
#define GateSwi_Module_setMask ti_sysbios_gates_GateSwi_Module_setMask
#define GateSwi_Object_heap ti_sysbios_gates_GateSwi_Object_heap
#define GateSwi_Module_heap ti_sysbios_gates_GateSwi_Module_heap
#define GateSwi_construct ti_sysbios_gates_GateSwi_construct
#define GateSwi_create ti_sysbios_gates_GateSwi_create
#define GateSwi_handle ti_sysbios_gates_GateSwi_handle
#define GateSwi_struct ti_sysbios_gates_GateSwi_struct
#define GateSwi_Handle_label ti_sysbios_gates_GateSwi_Handle_label
#define GateSwi_Handle_name ti_sysbios_gates_GateSwi_Handle_name
#define GateSwi_Instance_init ti_sysbios_gates_GateSwi_Instance_init
#define GateSwi_Object_count ti_sysbios_gates_GateSwi_Object_count
#define GateSwi_Object_get ti_sysbios_gates_GateSwi_Object_get
#define GateSwi_Object_first ti_sysbios_gates_GateSwi_Object_first
#define GateSwi_Object_next ti_sysbios_gates_GateSwi_Object_next
#define GateSwi_Object_sizeof ti_sysbios_gates_GateSwi_Object_sizeof
#define GateSwi_Params_copy ti_sysbios_gates_GateSwi_Params_copy
#define GateSwi_Params_init ti_sysbios_gates_GateSwi_Params_init
#define GateSwi_delete ti_sysbios_gates_GateSwi_delete
#define GateSwi_destruct ti_sysbios_gates_GateSwi_destruct
#define GateSwi_Module_upCast ti_sysbios_gates_GateSwi_Module_upCast
#define GateSwi_Module_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateSwi_Module_to_xdc_runtime_IGateProvider
#define GateSwi_Handle_upCast ti_sysbios_gates_GateSwi_Handle_upCast
#define GateSwi_Handle_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateSwi_Handle_to_xdc_runtime_IGateProvider
#define GateSwi_Handle_downCast ti_sysbios_gates_GateSwi_Handle_downCast
#define GateSwi_Handle_from_xdc_runtime_IGateProvider ti_sysbios_gates_GateSwi_Handle_from_xdc_runtime_IGateProvider
#define GateSwi_canBePreempted ti_sysbios_gates_GateSwi_canBePreempted
#define GateSwi_canBlock ti_sysbios_gates_GateSwi_canBlock
#define GateSwi_object ti_sysbios_gates_GateSwi_object
#define GateSwi_Params_default ti_sysbios_gates_GateSwi_Params_default

#endif /* ti_sysbios_gates_GateSwi_long_names */

#if defined(ti_sysbios_gates_GateSwi__nolocalnames) && !defined(ti_sysbios_gates_GateSwi_long_names)

#undef GateSwi_Instance
#undef GateSwi_Handle
#undef GateSwi_Module
#undef GateSwi_Object
#undef GateSwi_Struct
#undef GateSwi_Q_BLOCKING
#undef GateSwi_Q_PREEMPTING
#undef GateSwi_Instance_State
#undef GateSwi_Params
#undef GateSwi_query
#undef GateSwi_enter
#undef GateSwi_leave
#undef GateSwi_Module_name
#undef GateSwi_Module_id
#undef GateSwi_Module_startup
#undef GateSwi_Module_startupDone
#undef GateSwi_Module_hasMask
#undef GateSwi_Module_getMask
#undef GateSwi_Module_setMask
#undef GateSwi_Object_heap
#undef GateSwi_Module_heap
#undef GateSwi_construct
#undef GateSwi_create
#undef GateSwi_handle
#undef GateSwi_struct
#undef GateSwi_Handle_label
#undef GateSwi_Handle_name
#undef GateSwi_Instance_init
#undef GateSwi_Object_count
#undef GateSwi_Object_get
#undef GateSwi_Object_first
#undef GateSwi_Object_next
#undef GateSwi_Object_sizeof
#undef GateSwi_Params_copy
#undef GateSwi_Params_init
#undef GateSwi_delete
#undef GateSwi_destruct
#undef GateSwi_Module_upCast
#undef GateSwi_Module_to_xdc_runtime_IGateProvider
#undef GateSwi_Handle_upCast
#undef GateSwi_Handle_to_xdc_runtime_IGateProvider
#undef GateSwi_Handle_downCast
#undef GateSwi_Handle_from_xdc_runtime_IGateProvider
#undef GateSwi_canBePreempted
#undef GateSwi_canBlock
#undef GateSwi_object
#undef GateSwi_Params_default

#endif

/* @endcond */
