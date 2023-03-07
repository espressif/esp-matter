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
 * ======== Event_defs.h ========
 */

/* @cond LONG_NAMES */

#ifdef ti_sysbios_knl_Event_long_names

#define Event_Instance ti_sysbios_knl_Event_Instance
#define Event_Handle ti_sysbios_knl_Event_Handle
#define Event_Module ti_sysbios_knl_Event_Module
#define Event_Object ti_sysbios_knl_Event_Object
#define Event_Struct ti_sysbios_knl_Event_Struct
#define Event_PendState ti_sysbios_knl_Event_PendState
#define Event_PendElem ti_sysbios_knl_Event_PendElem
#define Event_Instance_State ti_sysbios_knl_Event_Instance_State
#define Event_PendState_TIMEOUT ti_sysbios_knl_Event_PendState_TIMEOUT
#define Event_PendState_POSTED ti_sysbios_knl_Event_PendState_POSTED
#define Event_PendState_CLOCK_WAIT ti_sysbios_knl_Event_PendState_CLOCK_WAIT
#define Event_PendState_WAIT_FOREVER ti_sysbios_knl_Event_PendState_WAIT_FOREVER
#define Event_Instance_State_pendQ ti_sysbios_knl_Event_Instance_State_pendQ
#define Event_Params ti_sysbios_knl_Event_Params
#define Event_pend ti_sysbios_knl_Event_pend
#define Event_post ti_sysbios_knl_Event_post
#define Event_getPostedEvents ti_sysbios_knl_Event_getPostedEvents
#define Event_sync ti_sysbios_knl_Event_sync
#define Event_Module_startup ti_sysbios_knl_Event_Module_startup
#define Event_Module_startupDone ti_sysbios_knl_Event_Module_startupDone
#define Event_construct ti_sysbios_knl_Event_construct
#define Event_create ti_sysbios_knl_Event_create
#define Event_handle ti_sysbios_knl_Event_handle
#define Event_struct ti_sysbios_knl_Event_struct
#define Event_Instance_init ti_sysbios_knl_Event_Instance_init
#define Event_Object_count ti_sysbios_knl_Event_Object_count
#define Event_Object_get ti_sysbios_knl_Event_Object_get
#define Event_Object_first ti_sysbios_knl_Event_Object_first
#define Event_Object_next ti_sysbios_knl_Event_Object_next
#define Event_Object_sizeof ti_sysbios_knl_Event_Object_sizeof
#define Event_Params_init ti_sysbios_knl_Event_Params_init
#define Event_delete ti_sysbios_knl_Event_delete
#define Event_destruct ti_sysbios_knl_Event_destruct
#define Event_checkEvents ti_sysbios_knl_Event_checkEvents
#define Event_Module_state ti_sysbios_knl_Event_state
#define Event_pendTimeout ti_sysbios_knl_Event_pendTimeout
#define Event_getEventFromObjElem ti_sysbios_knl_Event_getEventFromObjElem
#define Event_Params_default ti_sysbios_knl_Event_Params_default

#endif

#if defined(ti_sysbios_knl_Event__nolocalnames) && !defined(ti_sysbios_knl_Event_long_names)

#undef Event_Instance
#undef Event_Handle
#undef Event_Module
#undef Event_Object
#undef Event_Struct
#undef Event_PendState
#undef Event_PendElem
#undef Event_Instance_State
#undef Event_PendState_TIMEOUT
#undef Event_PendState_POSTED
#undef Event_PendState_CLOCK_WAIT
#undef Event_PendState_WAIT_FOREVER
#undef Event_Instance_State_pendQ
#undef Event_Params
#undef Event_pend
#undef Event_post
#undef Event_getPostedEvents
#undef Event_sync
#undef Event_Module_startup
#undef Event_Module_startupDone
#undef Event_construct
#undef Event_create
#undef Event_handle
#undef Event_struct
#undef Event_Instance_init
#undef Event_Object_count
#undef Event_Object_get
#undef Event_Object_first
#undef Event_Object_next
#undef Event_Object_sizeof
#undef Event_Params_init
#undef Event_delete
#undef Event_destruct
#undef Event_checkEvents
#undef Event_Module_state
#undef Event_pendTimeout
#undef Event_getEventFromObjElem
#undef Event_Params_default

#endif

/* @endcond */
