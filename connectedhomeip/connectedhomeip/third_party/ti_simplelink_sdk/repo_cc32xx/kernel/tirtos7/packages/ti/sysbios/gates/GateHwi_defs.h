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

#ifdef ti_sysbios_gates_GateHwi_long_names

#define GateHwi_Instance ti_sysbios_gates_GateHwi_Instance
#define GateHwi_Handle ti_sysbios_gates_GateHwi_Handle
#define GateHwi_Module ti_sysbios_gates_GateHwi_Module
#define GateHwi_Object ti_sysbios_gates_GateHwi_Object
#define GateHwi_Struct ti_sysbios_gates_GateHwi_Struct
#define GateHwi_Q_BLOCKING ti_sysbios_gates_GateHwi_Q_BLOCKING
#define GateHwi_Q_PREEMPTING ti_sysbios_gates_GateHwi_Q_PREEMPTING
#define GateHwi_Instance_State ti_sysbios_gates_GateHwi_Instance_State
#define GateHwi_Params ti_sysbios_gates_GateHwi_Params
#define GateHwi_query ti_sysbios_gates_GateHwi_query
#define GateHwi_enter ti_sysbios_gates_GateHwi_enter
#define GateHwi_leave ti_sysbios_gates_GateHwi_leave
#define GateHwi_Module_name ti_sysbios_gates_GateHwi_Module_name
#define GateHwi_Module_id ti_sysbios_gates_GateHwi_Module_id
#define GateHwi_Module_startup ti_sysbios_gates_GateHwi_Module_startup
#define GateHwi_Module_startupDone ti_sysbios_gates_GateHwi_Module_startupDone
#define GateHwi_Module_hasMask ti_sysbios_gates_GateHwi_Module_hasMask
#define GateHwi_Module_getMask ti_sysbios_gates_GateHwi_Module_getMask
#define GateHwi_Module_setMask ti_sysbios_gates_GateHwi_Module_setMask
#define GateHwi_Object_heap ti_sysbios_gates_GateHwi_Object_heap
#define GateHwi_Module_heap ti_sysbios_gates_GateHwi_Module_heap
#define GateHwi_construct ti_sysbios_gates_GateHwi_construct
#define GateHwi_create ti_sysbios_gates_GateHwi_create
#define GateHwi_handle ti_sysbios_gates_GateHwi_handle
#define GateHwi_struct ti_sysbios_gates_GateHwi_struct
#define GateHwi_Handle_label ti_sysbios_gates_GateHwi_Handle_label
#define GateHwi_Handle_name ti_sysbios_gates_GateHwi_Handle_name
#define GateHwi_Instance_init ti_sysbios_gates_GateHwi_Instance_init
#define GateHwi_Object_count ti_sysbios_gates_GateHwi_Object_count
#define GateHwi_Object_get ti_sysbios_gates_GateHwi_Object_get
#define GateHwi_Object_first ti_sysbios_gates_GateHwi_Object_first
#define GateHwi_Object_next ti_sysbios_gates_GateHwi_Object_next
#define GateHwi_Object_sizeof ti_sysbios_gates_GateHwi_Object_sizeof
#define GateHwi_Params_copy ti_sysbios_gates_GateHwi_Params_copy
#define GateHwi_Params_init ti_sysbios_gates_GateHwi_Params_init
#define GateHwi_delete ti_sysbios_gates_GateHwi_delete
#define GateHwi_destruct ti_sysbios_gates_GateHwi_destruct
#define GateHwi_Module_upCast ti_sysbios_gates_GateHwi_Module_upCast
#define GateHwi_Module_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateHwi_Module_to_xdc_runtime_IGateProvider
#define GateHwi_Handle_upCast ti_sysbios_gates_GateHwi_Handle_upCast
#define GateHwi_Handle_to_xdc_runtime_IGateProvider ti_sysbios_gates_GateHwi_Handle_to_xdc_runtime_IGateProvider
#define GateHwi_Handle_downCast ti_sysbios_gates_GateHwi_Handle_downCast
#define GateHwi_Handle_from_xdc_runtime_IGateProvider ti_sysbios_gates_GateHwi_Handle_from_xdc_runtime_IGateProvider
#define GateHwi_canBePreempted ti_sysbios_gates_GateHwi_canBePreempted
#define GateHwi_canBlock ti_sysbios_gates_GateHwi_canBlock
#define GateHwi_object ti_sysbios_gates_GateHwi_object
#define GateHwi_Params_default ti_sysbios_gates_GateHwi_Params_default

#endif /* ti_sysbios_gates_GateHwi_long_names */

#if defined(ti_sysbios_gates_GateHwi__nolocalnames) && !defined(ti_sysbios_gates_GateHwi_long_names)

#undef GateHwi_Instance
#undef GateHwi_Handle
#undef GateHwi_Module
#undef GateHwi_Object
#undef GateHwi_Struct
#undef GateHwi_Q_BLOCKING
#undef GateHwi_Q_PREEMPTING
#undef GateHwi_Instance_State
#undef GateHwi_Params
#undef GateHwi_query
#undef GateHwi_enter
#undef GateHwi_leave
#undef GateHwi_Module_name
#undef GateHwi_Module_id
#undef GateHwi_Module_startup
#undef GateHwi_Module_startupDone
#undef GateHwi_Module_hasMask
#undef GateHwi_Module_getMask
#undef GateHwi_Module_setMask
#undef GateHwi_Object_heap
#undef GateHwi_Module_heap
#undef GateHwi_construct
#undef GateHwi_create
#undef GateHwi_handle
#undef GateHwi_struct
#undef GateHwi_Handle_label
#undef GateHwi_Handle_name
#undef GateHwi_Instance_init
#undef GateHwi_Object_count
#undef GateHwi_Object_get
#undef GateHwi_Object_first
#undef GateHwi_Object_next
#undef GateHwi_Object_sizeof
#undef GateHwi_Params_copy
#undef GateHwi_Params_init
#undef GateHwi_delete
#undef GateHwi_destruct
#undef GateHwi_Module_upCast
#undef GateHwi_Module_to_xdc_runtime_IGateProvider
#undef GateHwi_Handle_upCast
#undef GateHwi_Handle_to_xdc_runtime_IGateProvider
#undef GateHwi_Handle_downCast
#undef GateHwi_Handle_from_xdc_runtime_IGateProvider
#undef GateHwi_canBePreempted
#undef GateHwi_canBlock
#undef GateHwi_object
#undef GateHwi_Params_default

#endif

/* @endcond */
