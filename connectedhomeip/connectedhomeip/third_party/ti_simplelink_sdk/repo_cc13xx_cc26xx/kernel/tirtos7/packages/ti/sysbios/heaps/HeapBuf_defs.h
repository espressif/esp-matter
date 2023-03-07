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
 * ======== HeapBuf_defs.h ========
 */

#ifdef ti_sysbios_heaps_HeapBuf_long_names

#define HeapBuf_Instance ti_sysbios_heaps_HeapBuf_Instance
#define HeapBuf_Handle ti_sysbios_heaps_HeapBuf_Handle
#define HeapBuf_Module ti_sysbios_heaps_HeapBuf_Module
#define HeapBuf_Object ti_sysbios_heaps_HeapBuf_Object
#define HeapBuf_Struct ti_sysbios_heaps_HeapBuf_Struct
#define HeapBuf_ExtendedStats ti_sysbios_heaps_HeapBuf_ExtendedStats
#define HeapBuf_Header ti_sysbios_heaps_HeapBuf_Header
#define HeapBuf_Instance_State ti_sysbios_heaps_HeapBuf_Instance_State
#define HeapBuf_primaryHeapBaseAddr ti_sysbios_heaps_HeapBuf_primaryHeapBaseAddr
#define HeapBuf_primaryHeapEndAddr ti_sysbios_heaps_HeapBuf_primaryHeapEndAddr
#define HeapBuf_Params ti_sysbios_heaps_HeapBuf_Params
#define HeapBuf_enter ti_sysbios_heaps_HeapBuf_enter
#define HeapBuf_leave ti_sysbios_heaps_HeapBuf_leave
#define HeapBuf_alloc ti_sysbios_heaps_HeapBuf_alloc
#define HeapBuf_allocUnprotected ti_sysbios_heaps_HeapBuf_allocUnprotected
#define HeapBuf_free ti_sysbios_heaps_HeapBuf_free
#define HeapBuf_freeUnprotected ti_sysbios_heaps_HeapBuf_freeUnprotected
#define HeapBuf_isBlocking ti_sysbios_heaps_HeapBuf_isBlocking
#define HeapBuf_getStats ti_sysbios_heaps_HeapBuf_getStats
#define HeapBuf_restore ti_sysbios_heaps_HeapBuf_restore
#define HeapBuf_getExtendedStats ti_sysbios_heaps_HeapBuf_getExtendedStats
#define HeapBuf_Module_name ti_sysbios_heaps_HeapBuf_Module_name
#define HeapBuf_Module_id ti_sysbios_heaps_HeapBuf_Module_id
#define HeapBuf_Module_startup ti_sysbios_heaps_HeapBuf_Module_startup
#define HeapBuf_Module_startupDone ti_sysbios_heaps_HeapBuf_Module_startupDone
#define HeapBuf_Module_hasMask ti_sysbios_heaps_HeapBuf_Module_hasMask
#define HeapBuf_Module_getMask ti_sysbios_heaps_HeapBuf_Module_getMask
#define HeapBuf_Module_setMask ti_sysbios_heaps_HeapBuf_Module_setMask
#define HeapBuf_Object_heap ti_sysbios_heaps_HeapBuf_Object_heap
#define HeapBuf_Module_heap ti_sysbios_heaps_HeapBuf_Module_heap
#define HeapBuf_construct ti_sysbios_heaps_HeapBuf_construct
#define HeapBuf_create ti_sysbios_heaps_HeapBuf_create
#define HeapBuf_handle ti_sysbios_heaps_HeapBuf_handle
#define HeapBuf_struct ti_sysbios_heaps_HeapBuf_struct
#define HeapBuf_Handle_label ti_sysbios_heaps_HeapBuf_Handle_label
#define HeapBuf_Handle_name ti_sysbios_heaps_HeapBuf_Handle_name
#define HeapBuf_Instance_init ti_sysbios_heaps_HeapBuf_Instance_init
#define HeapBuf_Object_count ti_sysbios_heaps_HeapBuf_Object_count
#define HeapBuf_Object_get ti_sysbios_heaps_HeapBuf_Object_get
#define HeapBuf_Object_first ti_sysbios_heaps_HeapBuf_Object_first
#define HeapBuf_Object_next ti_sysbios_heaps_HeapBuf_Object_next
#define HeapBuf_Object_sizeof ti_sysbios_heaps_HeapBuf_Object_sizeof
#define HeapBuf_Params_copy ti_sysbios_heaps_HeapBuf_Params_copy
#define HeapBuf_Params_init ti_sysbios_heaps_HeapBuf_Params_init
#define HeapBuf_delete ti_sysbios_heaps_HeapBuf_delete
#define HeapBuf_destruct ti_sysbios_heaps_HeapBuf_destruct
#define HeapBuf_Module_upCast ti_sysbios_heaps_HeapBuf_Module_upCast
#define HeapBuf_Module_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapBuf_Module_to_xdc_runtime_IHeap
#define HeapBuf_Handle_upCast ti_sysbios_heaps_HeapBuf_Handle_upCast
#define HeapBuf_Handle_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapBuf_Handle_to_xdc_runtime_IHeap
#define HeapBuf_Handle_downCast ti_sysbios_heaps_HeapBuf_Handle_downCast
#define HeapBuf_Handle_from_xdc_runtime_IHeap ti_sysbios_heaps_HeapBuf_Handle_from_xdc_runtime_IHeap
#define HeapBuf_gate ti_sysbios_heaps_HeapBuf_gate
#define HeapBuf_getHeapBufFromObjElem ti_sysbios_heaps_HeapBuf_getHeapBufFromObjElem
#define HeapBuf_init ti_sysbios_heaps_HeapBuf_init
#define HeapBuf_Module_state ti_sysbios_heaps_HeapBuf_Module_state
#define HeapBuf_Params_default ti_sysbios_heaps_HeapBuf_Params_default
#define HeapBuf_getAlign ti_sysbios_heaps_HeapBuf_getAlign
#define HeapBuf_getBlockSize ti_sysbios_heaps_HeapBuf_getBlockSize
#define HeapBuf_getEndAddr ti_sysbios_heaps_HeapBuf_getEndAddr
#define HeapBuf_Instance_finalize ti_sysbios_heaps_HeapBuf_Instance_finalize
#define HeapBuf_mergeHeapBufs ti_sysbios_heaps_HeapBuf_mergeHeapBufs
#define HeapBuf_postInit ti_sysbios_heaps_HeapBuf_postInit
#define HeapBuf_trackMaxAllocs ti_sysbios_heaps_HeapBuf_trackMaxAllocs

#endif /* ti_sysbios_heaps_HeapBuf_long_names */

#if defined(ti_sysbios_heaps_HeapBuf__nolocalnames) && !defined(ti_sysbios_heaps_HeapBuf_long_names)

#undef HeapBuf_Instance
#undef HeapBuf_Handle
#undef HeapBuf_Module
#undef HeapBuf_Object
#undef HeapBuf_Struct
#undef HeapBuf_ExtendedStats
#undef HeapBuf_Header
#undef HeapBuf_Instance_State
#undef HeapBuf_primaryHeapBaseAddr
#undef HeapBuf_primaryHeapEndAddr
#undef HeapBuf_Params
#undef HeapBuf_enter
#undef HeapBuf_leave
#undef HeapBuf_alloc
#undef HeapBuf_allocUnprotected
#undef HeapBuf_free
#undef HeapBuf_freeUnprotected
#undef HeapBuf_isBlocking
#undef HeapBuf_getStats
#undef HeapBuf_restore
#undef HeapBuf_getExtendedStats
#undef HeapBuf_Module_name
#undef HeapBuf_Module_id
#undef HeapBuf_Module_startup
#undef HeapBuf_Module_startupDone
#undef HeapBuf_Module_hasMask
#undef HeapBuf_Module_getMask
#undef HeapBuf_Module_setMask
#undef HeapBuf_Object_heap
#undef HeapBuf_Module_heap
#undef HeapBuf_construct
#undef HeapBuf_create
#undef HeapBuf_handle
#undef HeapBuf_struct
#undef HeapBuf_Handle_label
#undef HeapBuf_Handle_name
#undef HeapBuf_Instance_init
#undef HeapBuf_Object_count
#undef HeapBuf_Object_get
#undef HeapBuf_Object_first
#undef HeapBuf_Object_next
#undef HeapBuf_Object_sizeof
#undef HeapBuf_Params_copy
#undef HeapBuf_Params_init
#undef HeapBuf_delete
#undef HeapBuf_destruct
#undef HeapBuf_Module_upCast
#undef HeapBuf_Module_to_xdc_runtime_IHeap
#undef HeapBuf_Handle_upCast
#undef HeapBuf_Handle_to_xdc_runtime_IHeap
#undef HeapBuf_Handle_downCast
#undef HeapBuf_Handle_from_xdc_runtime_IHeap
#undef HeapBuf_gate
#undef HeapBuf_getHeapBufFromObjElem
#undef HeapBuf_init
#undef HeapBuf_Module_state
#undef HeapBuf_Params_default
#undef HeapBuf_getAlign
#undef HeapBuf_getBlockSize
#undef HeapBuf_getEndAddr
#undef HeapBuf_Instance_finalize
#undef HeapBuf_mergeHeapBufs
#undef HeapBuf_postInit
#undef HeapBuf_trackMaxAllocs

#endif

/* @endcond */
