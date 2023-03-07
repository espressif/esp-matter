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
 * ======== HeapTrack_defs.h ========
 */

#ifdef ti_sysbios_heaps_HeapTrack_long_names

#define HeapTrack_Instance ti_sysbios_heaps_HeapTrack_Instance
#define HeapTrack_Handle ti_sysbios_heaps_HeapTrack_Handle
#define HeapTrack_Module ti_sysbios_heaps_HeapTrack_Module
#define HeapTrack_Object ti_sysbios_heaps_HeapTrack_Object
#define HeapTrack_Struct ti_sysbios_heaps_HeapTrack_Struct
#define HeapTrack_Tracker ti_sysbios_heaps_HeapTrack_Tracker
#define HeapTrack_STARTSCRIBBLE ti_sysbios_heaps_HeapTrack_STARTSCRIBBLE
#define HeapTrack_NOSCRIBBLE ti_sysbios_heaps_HeapTrack_NOSCRIBBLE
#define HeapTrack_Instance_State ti_sysbios_heaps_HeapTrack_Instance_State
#define HeapTrack_Instance_State_trackQueue ti_sysbios_heaps_HeapTrack_Instance_State_trackQueue
#define HeapTrack_Params ti_sysbios_heaps_HeapTrack_Params
#define HeapTrack_alloc ti_sysbios_heaps_HeapTrack_alloc
#define HeapTrack_free ti_sysbios_heaps_HeapTrack_free
#define HeapTrack_isBlocking ti_sysbios_heaps_HeapTrack_isBlocking
#define HeapTrack_getStats ti_sysbios_heaps_HeapTrack_getStats
#define HeapTrack_printTask ti_sysbios_heaps_HeapTrack_printTask
#define HeapTrack_printHeap ti_sysbios_heaps_HeapTrack_printHeap
#define HeapTrack_Module_name ti_sysbios_heaps_HeapTrack_Module_name
#define HeapTrack_Module_id ti_sysbios_heaps_HeapTrack_Module_id
#define HeapTrack_Module_startup ti_sysbios_heaps_HeapTrack_Module_startup
#define HeapTrack_Module_startupDone ti_sysbios_heaps_HeapTrack_Module_startupDone
#define HeapTrack_Module_hasMask ti_sysbios_heaps_HeapTrack_Module_hasMask
#define HeapTrack_Module_getMask ti_sysbios_heaps_HeapTrack_Module_getMask
#define HeapTrack_Module_setMask ti_sysbios_heaps_HeapTrack_Module_setMask
#define HeapTrack_Object_heap ti_sysbios_heaps_HeapTrack_Object_heap
#define HeapTrack_Module_heap ti_sysbios_heaps_HeapTrack_Module_heap
#define HeapTrack_construct ti_sysbios_heaps_HeapTrack_construct
#define HeapTrack_create ti_sysbios_heaps_HeapTrack_create
#define HeapTrack_handle ti_sysbios_heaps_HeapTrack_handle
#define HeapTrack_struct ti_sysbios_heaps_HeapTrack_struct
#define HeapTrack_Handle_label ti_sysbios_heaps_HeapTrack_Handle_label
#define HeapTrack_Handle_name ti_sysbios_heaps_HeapTrack_Handle_name
#define HeapTrack_Instance_init ti_sysbios_heaps_HeapTrack_Instance_init
#define HeapTrack_Object_count ti_sysbios_heaps_HeapTrack_Object_count
#define HeapTrack_Object_get ti_sysbios_heaps_HeapTrack_Object_get
#define HeapTrack_Object_first ti_sysbios_heaps_HeapTrack_Object_first
#define HeapTrack_Object_next ti_sysbios_heaps_HeapTrack_Object_next
#define HeapTrack_Object_sizeof ti_sysbios_heaps_HeapTrack_Object_sizeof
#define HeapTrack_Params_copy ti_sysbios_heaps_HeapTrack_Params_copy
#define HeapTrack_Params_init ti_sysbios_heaps_HeapTrack_Params_init
#define HeapTrack_Instance_finalize ti_sysbios_heaps_HeapTrack_Instance_finalize
#define HeapTrack_delete ti_sysbios_heaps_HeapTrack_delete
#define HeapTrack_destruct ti_sysbios_heaps_HeapTrack_destruct
#define HeapTrack_Module_upCast ti_sysbios_heaps_HeapTrack_Module_upCast
#define HeapTrack_Module_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapTrack_Module_to_xdc_runtime_IHeap
#define HeapTrack_Handle_upCast ti_sysbios_heaps_HeapTrack_Handle_upCast
#define HeapTrack_Handle_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapTrack_Handle_to_xdc_runtime_IHeap
#define HeapTrack_Handle_downCast ti_sysbios_heaps_HeapTrack_Handle_downCast
#define HeapTrack_Handle_from_xdc_runtime_IHeap ti_sysbios_heaps_HeapTrack_Handle_from_xdc_runtime_IHeap
#define HeapTrack_getHeapTrackFromObjElem ti_sysbios_heaps_HeapTrack_getHeapTrackFromObjElem
#define HeapTrack_init ti_sysbios_heaps_HeapTrack_init
#define HeapTrack_Module_state ti_sysbios_heaps_HeapTrack_Module_state
#define HeapTrack_Params_default ti_sysbios_heaps_HeapTrack_Params_default
#define HeapTrack_printTrack ti_sysbios_heaps_HeapTrack_printTrack

#endif /* ti_sysbios_heaps_HeapTrack_long_names */

#if defined(ti_sysbios_heaps_HeapTrack__nolocalnames) && !defined(ti_sysbios_heaps_HeapTrack_long_names)

#undef HeapTrack_Instance
#undef HeapTrack_Handle
#undef HeapTrack_Module
#undef HeapTrack_Object
#undef HeapTrack_Struct
#undef HeapTrack_Tracker
#undef HeapTrack_STARTSCRIBBLE
#undef HeapTrack_NOSCRIBBLE
#undef HeapTrack_Instance_State
#undef HeapTrack_Instance_State_trackQueue
#undef HeapTrack_Params
#undef HeapTrack_alloc
#undef HeapTrack_free
#undef HeapTrack_isBlocking
#undef HeapTrack_getStats
#undef HeapTrack_printTask
#undef HeapTrack_printHeap
#undef HeapTrack_Module_name
#undef HeapTrack_Module_id
#undef HeapTrack_Module_startup
#undef HeapTrack_Module_startupDone
#undef HeapTrack_Module_hasMask
#undef HeapTrack_Module_getMask
#undef HeapTrack_Module_setMask
#undef HeapTrack_Object_heap
#undef HeapTrack_Module_heap
#undef HeapTrack_construct
#undef HeapTrack_create
#undef HeapTrack_handle
#undef HeapTrack_struct
#undef HeapTrack_Handle_label
#undef HeapTrack_Handle_name
#undef HeapTrack_Instance_init
#undef HeapTrack_Object_count
#undef HeapTrack_Object_get
#undef HeapTrack_Object_first
#undef HeapTrack_Object_next
#undef HeapTrack_Object_sizeof
#undef HeapTrack_Params_copy
#undef HeapTrack_Params_init
#undef HeapTrack_Instance_finalize
#undef HeapTrack_delete
#undef HeapTrack_destruct
#undef HeapTrack_Module_upCast
#undef HeapTrack_Module_to_xdc_runtime_IHeap
#undef HeapTrack_Handle_upCast
#undef HeapTrack_Handle_to_xdc_runtime_IHeap
#undef HeapTrack_Handle_downCast
#undef HeapTrack_Handle_from_xdc_runtime_IHeap
#undef HeapTrack_getHeapTrackFromObjElem
#undef HeapTrack_init
#undef HeapTrack_Module_state
#undef HeapTrack_Params_default
#undef HeapTrack_printTrack

#endif

/* @endcond */
