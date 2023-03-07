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
 * ======== HeapMem_defs.h ========
 */

#ifdef ti_sysbios_heaps_HeapMem_long_names

#define HeapMem_Instance ti_sysbios_heaps_HeapMem_Instance
#define HeapMem_Handle ti_sysbios_heaps_HeapMem_Handle
#define HeapMem_Module ti_sysbios_heaps_HeapMem_Module
#define HeapMem_Object ti_sysbios_heaps_HeapMem_Object
#define HeapMem_Struct ti_sysbios_heaps_HeapMem_Struct
#define HeapMem_ExtendedStats ti_sysbios_heaps_HeapMem_ExtendedStats
#define HeapMem_Header ti_sysbios_heaps_HeapMem_Header
#define HeapMem_Instance_State ti_sysbios_heaps_HeapMem_Instance_State
#define HeapMem_primaryHeapBaseAddr ti_sysbios_heaps_HeapMem_primaryHeapBaseAddr
#define HeapMem_primaryHeapEndAddr ti_sysbios_heaps_HeapMem_primaryHeapEndAddr
#define HeapMem_Params ti_sysbios_heaps_HeapMem_Params
#define HeapMem_enter ti_sysbios_heaps_HeapMem_enter
#define HeapMem_leave ti_sysbios_heaps_HeapMem_leave
#define HeapMem_alloc ti_sysbios_heaps_HeapMem_alloc
#define HeapMem_allocUnprotected ti_sysbios_heaps_HeapMem_allocUnprotected
#define HeapMem_free ti_sysbios_heaps_HeapMem_free
#define HeapMem_freeUnprotected ti_sysbios_heaps_HeapMem_freeUnprotected
#define HeapMem_isBlocking ti_sysbios_heaps_HeapMem_isBlocking
#define HeapMem_getStats ti_sysbios_heaps_HeapMem_getStats
#define HeapMem_restore ti_sysbios_heaps_HeapMem_restore
#define HeapMem_getExtendedStats ti_sysbios_heaps_HeapMem_getExtendedStats
#define HeapMem_Module_name ti_sysbios_heaps_HeapMem_Module_name
#define HeapMem_Module_id ti_sysbios_heaps_HeapMem_Module_id
#define HeapMem_Module_startup ti_sysbios_heaps_HeapMem_Module_startup
#define HeapMem_Module_startupDone ti_sysbios_heaps_HeapMem_Module_startupDone
#define HeapMem_Module_hasMask ti_sysbios_heaps_HeapMem_Module_hasMask
#define HeapMem_Module_getMask ti_sysbios_heaps_HeapMem_Module_getMask
#define HeapMem_Module_setMask ti_sysbios_heaps_HeapMem_Module_setMask
#define HeapMem_Object_heap ti_sysbios_heaps_HeapMem_Object_heap
#define HeapMem_Module_heap ti_sysbios_heaps_HeapMem_Module_heap
#define HeapMem_construct ti_sysbios_heaps_HeapMem_construct
#define HeapMem_create ti_sysbios_heaps_HeapMem_create
#define HeapMem_handle ti_sysbios_heaps_HeapMem_handle
#define HeapMem_struct ti_sysbios_heaps_HeapMem_struct
#define HeapMem_Handle_label ti_sysbios_heaps_HeapMem_Handle_label
#define HeapMem_Handle_name ti_sysbios_heaps_HeapMem_Handle_name
#define HeapMem_Instance_init ti_sysbios_heaps_HeapMem_Instance_init
#define HeapMem_Object_count ti_sysbios_heaps_HeapMem_Object_count
#define HeapMem_Object_get ti_sysbios_heaps_HeapMem_Object_get
#define HeapMem_Object_first ti_sysbios_heaps_HeapMem_Object_first
#define HeapMem_Object_next ti_sysbios_heaps_HeapMem_Object_next
#define HeapMem_Object_sizeof ti_sysbios_heaps_HeapMem_Object_sizeof
#define HeapMem_Params_copy ti_sysbios_heaps_HeapMem_Params_copy
#define HeapMem_Params_init ti_sysbios_heaps_HeapMem_Params_init
#define HeapMem_delete ti_sysbios_heaps_HeapMem_delete
#define HeapMem_destruct ti_sysbios_heaps_HeapMem_destruct
#define HeapMem_Module_upCast ti_sysbios_heaps_HeapMem_Module_upCast
#define HeapMem_Module_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapMem_Module_to_xdc_runtime_IHeap
#define HeapMem_Handle_upCast ti_sysbios_heaps_HeapMem_Handle_upCast
#define HeapMem_Handle_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapMem_Handle_to_xdc_runtime_IHeap
#define HeapMem_Handle_downCast ti_sysbios_heaps_HeapMem_Handle_downCast
#define HeapMem_Handle_from_xdc_runtime_IHeap ti_sysbios_heaps_HeapMem_Handle_from_xdc_runtime_IHeap
#define HeapMem_gate ti_sysbios_heaps_HeapMem_gate
#define HeapMem_getHeapMemFromObjElem ti_sysbios_heaps_HeapMem_getHeapMemFromObjElem
#define HeapMem_init ti_sysbios_heaps_HeapMem_init
#define HeapMem_Module_state ti_sysbios_heaps_HeapMem_Module_state
#define HeapMem_Params_default ti_sysbios_heaps_HeapMem_Params_default

#endif /* ti_sysbios_heaps_HeapMem_long_names */

#if defined(ti_sysbios_heaps_HeapMem__nolocalnames) && !defined(ti_sysbios_heaps_HeapMem_long_names)

#undef HeapMem_Instance
#undef HeapMem_Handle
#undef HeapMem_Module
#undef HeapMem_Object
#undef HeapMem_Struct
#undef HeapMem_ExtendedStats
#undef HeapMem_Header
#undef HeapMem_Instance_State
#undef HeapMem_primaryHeapBaseAddr
#undef HeapMem_primaryHeapEndAddr
#undef HeapMem_Params
#undef HeapMem_enter
#undef HeapMem_leave
#undef HeapMem_alloc
#undef HeapMem_allocUnprotected
#undef HeapMem_free
#undef HeapMem_freeUnprotected
#undef HeapMem_isBlocking
#undef HeapMem_getStats
#undef HeapMem_restore
#undef HeapMem_getExtendedStats
#undef HeapMem_Module_name
#undef HeapMem_Module_id
#undef HeapMem_Module_startup
#undef HeapMem_Module_startupDone
#undef HeapMem_Module_hasMask
#undef HeapMem_Module_getMask
#undef HeapMem_Module_setMask
#undef HeapMem_Object_heap
#undef HeapMem_Module_heap
#undef HeapMem_construct
#undef HeapMem_create
#undef HeapMem_handle
#undef HeapMem_struct
#undef HeapMem_Handle_label
#undef HeapMem_Handle_name
#undef HeapMem_Instance_init
#undef HeapMem_Object_count
#undef HeapMem_Object_get
#undef HeapMem_Object_first
#undef HeapMem_Object_next
#undef HeapMem_Object_sizeof
#undef HeapMem_Params_copy
#undef HeapMem_Params_init
#undef HeapMem_delete
#undef HeapMem_destruct
#undef HeapMem_Module_upCast
#undef HeapMem_Module_to_xdc_runtime_IHeap
#undef HeapMem_Handle_upCast
#undef HeapMem_Handle_to_xdc_runtime_IHeap
#undef HeapMem_Handle_downCast
#undef HeapMem_Handle_from_xdc_runtime_IHeap
#undef HeapMem_gate
#undef HeapMem_getHeapMemFromObjElem
#undef HeapMem_init
#undef HeapMem_Module_state
#undef HeapMem_Params_default

#endif

/* @endcond */
