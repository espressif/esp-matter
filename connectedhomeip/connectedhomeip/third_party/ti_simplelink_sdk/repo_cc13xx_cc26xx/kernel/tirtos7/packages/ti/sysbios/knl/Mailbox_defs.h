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
 * ======== Mailbox_defs.h ========
 */

/* @cond LONG_NAMES */

#ifdef ti_sysbios_knl_Mailbox_long_names

#define Mailbox_Instance ti_sysbios_knl_Mailbox_Instance
#define Mailbox_Handle ti_sysbios_knl_Mailbox_Handle
#define Mailbox_Module ti_sysbios_knl_Mailbox_Module
#define Mailbox_Object ti_sysbios_knl_Mailbox_Object
#define Mailbox_Struct ti_sysbios_knl_Mailbox_Struct
#define Mailbox_MbxElem ti_sysbios_knl_Mailbox_MbxElem
#define Mailbox_Instance_State ti_sysbios_knl_Mailbox_Instance_State
#define Mailbox_Instance_State_dataQue ti_sysbios_knl_Mailbox_Instance_State_dataQue
#define Mailbox_Instance_State_freeQue ti_sysbios_knl_Mailbox_Instance_State_freeQue
#define Mailbox_Instance_State_dataSem ti_sysbios_knl_Mailbox_Instance_State_dataSem
#define Mailbox_Instance_State_freeSem ti_sysbios_knl_Mailbox_Instance_State_freeSem
#define Mailbox_Params ti_sysbios_knl_Mailbox_Params
#define Mailbox_getMsgSize ti_sysbios_knl_Mailbox_getMsgSize
#define Mailbox_getNumFreeMsgs ti_sysbios_knl_Mailbox_getNumFreeMsgs
#define Mailbox_getNumPendingMsgs ti_sysbios_knl_Mailbox_getNumPendingMsgs
#define Mailbox_pend ti_sysbios_knl_Mailbox_pend
#define Mailbox_post ti_sysbios_knl_Mailbox_post
#define Mailbox_Module_startup ti_sysbios_knl_Mailbox_Module_startup
#define Mailbox_Module_startupDone ti_sysbios_knl_Mailbox_Module_startupDone
#define Mailbox_construct ti_sysbios_knl_Mailbox_construct
#define Mailbox_create ti_sysbios_knl_Mailbox_create
#define Mailbox_handle ti_sysbios_knl_Mailbox_handle
#define Mailbox_struct ti_sysbios_knl_Mailbox_struct
#define Mailbox_Instance_init ti_sysbios_knl_Mailbox_Instance_init
#define Mailbox_Object_count ti_sysbios_knl_Mailbox_Object_count
#define Mailbox_Object_get ti_sysbios_knl_Mailbox_Object_get
#define Mailbox_Object_first ti_sysbios_knl_Mailbox_Object_first
#define Mailbox_Object_next ti_sysbios_knl_Mailbox_Object_next
#define Mailbox_Object_sizeof ti_sysbios_knl_Mailbox_Object_sizeof
#define Mailbox_Params_init ti_sysbios_knl_Mailbox_Params_init
#define Mailbox_Instance_finalize ti_sysbios_knl_Mailbox_Instance_finalize
#define Mailbox_delete ti_sysbios_knl_Mailbox_delete
#define Mailbox_destruct ti_sysbios_knl_Mailbox_destruct
#define Mailbox_Module_state ti_sysbios_knl_Mailbox_Module_state
#define Mailbox_Params_default ti_sysbios_knl_Mailbox_Params_default
#define Mailbox_postInit ti_sysbios_knl_Mailbox_postInit
#define Mailbox_getMailboxFromObjElem ti_sysbios_knl_Mailbox_getMailboxFromObjElem

#endif

#if defined(ti_sysbios_knl_Mailbox__nolocalnames) && !defined(ti_sysbios_knl_Mailbox_long_names)

#undef Mailbox_Instance
#undef Mailbox_Handle
#undef Mailbox_Module
#undef Mailbox_Object
#undef Mailbox_Struct
#undef Mailbox_MbxElem
#undef Mailbox_Instance_State
#undef Mailbox_Instance_State_dataQue
#undef Mailbox_Instance_State_freeQue
#undef Mailbox_Instance_State_dataSem
#undef Mailbox_Instance_State_freeSem
#undef Mailbox_Params
#undef Mailbox_getMsgSize
#undef Mailbox_getNumFreeMsgs
#undef Mailbox_getNumPendingMsgs
#undef Mailbox_pend
#undef Mailbox_post
#undef Mailbox_Module_startup
#undef Mailbox_Module_startupDone
#undef Mailbox_construct
#undef Mailbox_create
#undef Mailbox_handle
#undef Mailbox_struct
#undef Mailbox_Instance_init
#undef Mailbox_Object_count
#undef Mailbox_Object_get
#undef Mailbox_Object_first
#undef Mailbox_Object_next
#undef Mailbox_Object_sizeof
#undef Mailbox_Params_init
#undef Mailbox_Instance_finalize
#undef Mailbox_delete
#undef Mailbox_destruct
#undef Mailbox_Module_state
#undef Mailbox_Params_default
#undef Mailbox_postInit
#undef Mailbox_getMailboxFromObjElem

#endif

/* @endcond */
