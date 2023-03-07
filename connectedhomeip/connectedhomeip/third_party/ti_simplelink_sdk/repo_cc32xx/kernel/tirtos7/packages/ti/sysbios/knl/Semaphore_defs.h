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
 * ======== Semaphore_defs.h ========
 */

/* @cond LONG_NAMES */

#ifdef ti_sysbios_knl_Semaphore_long_names

#define Semaphore_Instance ti_sysbios_knl_Semaphore_Instance
#define Semaphore_Handle ti_sysbios_knl_Semaphore_Handle
#define Semaphore_Module ti_sysbios_knl_Semaphore_Module
#define Semaphore_Object ti_sysbios_knl_Semaphore_Object
#define Semaphore_Struct ti_sysbios_knl_Semaphore_Struct
#define Semaphore_Mode ti_sysbios_knl_Semaphore_Mode
#define Semaphore_PendState ti_sysbios_knl_Semaphore_PendState
#define Semaphore_PendElem ti_sysbios_knl_Semaphore_PendElem
#define Semaphore_Instance_State ti_sysbios_knl_Semaphore_Instance_State
#define Semaphore_Mode_COUNTING ti_sysbios_knl_Semaphore_Mode_COUNTING
#define Semaphore_Mode_BINARY ti_sysbios_knl_Semaphore_Mode_BINARY
#define Semaphore_Mode_COUNTING_PRIORITY ti_sysbios_knl_Semaphore_Mode_COUNTING_PRIORITY
#define Semaphore_Mode_BINARY_PRIORITY ti_sysbios_knl_Semaphore_Mode_BINARY_PRIORITY
#define Semaphore_PendState_TIMEOUT ti_sysbios_knl_Semaphore_PendState_TIMEOUT
#define Semaphore_PendState_POSTED ti_sysbios_knl_Semaphore_PendState_POSTED
#define Semaphore_PendState_CLOCK_WAIT ti_sysbios_knl_Semaphore_PendState_CLOCK_WAIT
#define Semaphore_PendState_WAIT_FOREVER ti_sysbios_knl_Semaphore_PendState_WAIT_FOREVER
#define Semaphore_supportsEvents ti_sysbios_knl_Semaphore_supportsEvents
#define Semaphore_supportsPriority ti_sysbios_knl_Semaphore_supportsPriority
#define Semaphore_eventPost ti_sysbios_knl_Semaphore_eventPost
#define Semaphore_eventSync ti_sysbios_knl_Semaphore_eventSync
#define Semaphore_Instance_State_pendQ ti_sysbios_knl_Semaphore_Instance_State_pendQ
#define Semaphore_Params ti_sysbios_knl_Semaphore_Params
#define Semaphore_getCount ti_sysbios_knl_Semaphore_getCount
#define Semaphore_pend ti_sysbios_knl_Semaphore_pend
#define Semaphore_post ti_sysbios_knl_Semaphore_post
#define Semaphore_registerEvent ti_sysbios_knl_Semaphore_registerEvent
#define Semaphore_reset ti_sysbios_knl_Semaphore_reset
#define Semaphore_Module_startup ti_sysbios_knl_Semaphore_Module_startup
#define Semaphore_Module_startupDone ti_sysbios_knl_Semaphore_Module_startupDone
#define Semaphore_construct ti_sysbios_knl_Semaphore_construct
#define Semaphore_create ti_sysbios_knl_Semaphore_create
#define Semaphore_handle ti_sysbios_knl_Semaphore_handle
#define Semaphore_struct ti_sysbios_knl_Semaphore_struct
#define Semaphore_Instance_init ti_sysbios_knl_Semaphore_Instance_init
#define Semaphore_Object_count ti_sysbios_knl_Semaphore_Object_count
#define Semaphore_Object_get ti_sysbios_knl_Semaphore_Object_get
#define Semaphore_Object_first ti_sysbios_knl_Semaphore_Object_first
#define Semaphore_Object_next ti_sysbios_knl_Semaphore_Object_next
#define Semaphore_Object_sizeof ti_sysbios_knl_Semaphore_Object_sizeof
#define Semaphore_Params_init ti_sysbios_knl_Semaphore_Params_init
#define Semaphore_Instance_finalize ti_sysbios_knl_Semaphore_Instance_finalize
#define Semaphore_delete ti_sysbios_knl_Semaphore_delete
#define Semaphore_destruct ti_sysbios_knl_Semaphore_destruct
#define Semaphore_Module_state ti_sysbios_knl_Semaphore_Module_state
#define Semaphore_Params_default ti_sysbios_knl_Semaphore_Params_default
#define Semaphore_pendTimeout ti_sysbios_knl_Semaphore_pendTimeout
#define Semaphore_getSemaphoreFromObjElem ti_sysbios_knl_Semaphore_getSemaphoreFromObjElem
#define Semaphore_testStaticInlines ti_sysbios_knl_Semaphore_testStaticInlines

#endif

#if defined(ti_sysbios_knl_Semaphore__nolocalnames) && !defined(ti_sysbios_knl_Semaphore_long_names)

#undef Semaphore_Instance
#undef Semaphore_Handle
#undef Semaphore_Module
#undef Semaphore_Object
#undef Semaphore_Struct
#undef Semaphore_Mode
#undef Semaphore_PendState
#undef Semaphore_PendElem
#undef Semaphore_Instance_State
#undef Semaphore_Mode_COUNTING
#undef Semaphore_Mode_BINARY
#undef Semaphore_Mode_COUNTING_PRIORITY
#undef Semaphore_Mode_BINARY_PRIORITY
#undef Semaphore_PendState_TIMEOUT
#undef Semaphore_PendState_POSTED
#undef Semaphore_PendState_CLOCK_WAIT
#undef Semaphore_PendState_WAIT_FOREVER
#undef Semaphore_supportsEvents
#undef Semaphore_supportsPriority
#undef Semaphore_eventPost
#undef Semaphore_eventSync
#undef Semaphore_Instance_State_pendQ
#undef Semaphore_Params
#undef Semaphore_getCount
#undef Semaphore_pend
#undef Semaphore_post
#undef Semaphore_registerEvent
#undef Semaphore_reset
#undef Semaphore_Module_startup
#undef Semaphore_Module_startupDone
#undef Semaphore_construct
#undef Semaphore_create
#undef Semaphore_handle
#undef Semaphore_struct
#undef Semaphore_Instance_init
#undef Semaphore_Object_count
#undef Semaphore_Object_get
#undef Semaphore_Object_first
#undef Semaphore_Object_next
#undef Semaphore_Object_sizeof
#undef Semaphore_Params_init
#undef Semaphore_Instance_finalize
#undef Semaphore_delete
#undef Semaphore_destruct
#undef Semaphore_Module_state
#undef Semaphore_Params_default
#undef Semaphore_pendTimeout
#undef Semaphore_getSemaphoreFromObjElem
#undef Semaphore_testStaticInlines

#endif

/* @endcond */
