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
 * ======== HeapMultiBuf_defs.h ========
 */

#ifdef ti_sysbios_heaps_HeapMultiBuf_long_names

#define HeapMultiBuf_Instance ti_sysbios_heaps_HeapMultiBuf_Instance
#define HeapMultiBuf_Handle ti_sysbios_heaps_HeapMultiBuf_Handle
#define HeapMultiBuf_Module ti_sysbios_heaps_HeapMultiBuf_Module
#define HeapMultiBuf_Object ti_sysbios_heaps_HeapMultiBuf_Object
#define HeapMultiBuf_Struct ti_sysbios_heaps_HeapMultiBuf_Struct
#define HeapMultiBuf_AddrPair ti_sysbios_heaps_HeapMultiBuf_AddrPair
#define HeapMultiBuf_Instance_State ti_sysbios_heaps_HeapMultiBuf_Instance_State
#define HeapMultiBuf_Params ti_sysbios_heaps_HeapMultiBuf_Params
#define HeapMultiBuf_getStats ti_sysbios_heaps_HeapMultiBuf_getStats
#define HeapMultiBuf_alloc ti_sysbios_heaps_HeapMultiBuf_alloc
#define HeapMultiBuf_free ti_sysbios_heaps_HeapMultiBuf_free
#define HeapMultiBuf_isBlocking ti_sysbios_heaps_HeapMultiBuf_isBlocking
#define HeapMultiBuf_Module_name ti_sysbios_heaps_HeapMultiBuf_Module_name
#define HeapMultiBuf_Module_id ti_sysbios_heaps_HeapMultiBuf_Module_id
#define HeapMultiBuf_Module_startup ti_sysbios_heaps_HeapMultiBuf_Module_startup
#define HeapMultiBuf_Module_startupDone ti_sysbios_heaps_HeapMultiBuf_Module_startupDone
#define HeapMultiBuf_Module_hasMask ti_sysbios_heaps_HeapMultiBuf_Module_hasMask
#define HeapMultiBuf_Module_getMask ti_sysbios_heaps_HeapMultiBuf_Module_getMask
#define HeapMultiBuf_Module_setMask ti_sysbios_heaps_HeapMultiBuf_Module_setMask
#define HeapMultiBuf_Object_heap ti_sysbios_heaps_HeapMultiBuf_Object_heap
#define HeapMultiBuf_Module_heap ti_sysbios_heaps_HeapMultiBuf_Module_heap
#define HeapMultiBuf_construct ti_sysbios_heaps_HeapMultiBuf_construct
#define HeapMultiBuf_create ti_sysbios_heaps_HeapMultiBuf_create
#define HeapMultiBuf_handle ti_sysbios_heaps_HeapMultiBuf_handle
#define HeapMultiBuf_struct ti_sysbios_heaps_HeapMultiBuf_struct
#define HeapMultiBuf_Handle_label ti_sysbios_heaps_HeapMultiBuf_Handle_label
#define HeapMultiBuf_Handle_name ti_sysbios_heaps_HeapMultiBuf_Handle_name
#define HeapMultiBuf_Instance_init ti_sysbios_heaps_HeapMultiBuf_Instance_init
#define HeapMultiBuf_Object_count ti_sysbios_heaps_HeapMultiBuf_Object_count
#define HeapMultiBuf_Object_get ti_sysbios_heaps_HeapMultiBuf_Object_get
#define HeapMultiBuf_Object_first ti_sysbios_heaps_HeapMultiBuf_Object_first
#define HeapMultiBuf_Object_next ti_sysbios_heaps_HeapMultiBuf_Object_next
#define HeapMultiBuf_Object_sizeof ti_sysbios_heaps_HeapMultiBuf_Object_sizeof
#define HeapMultiBuf_Params_copy ti_sysbios_heaps_HeapMultiBuf_Params_copy
#define HeapMultiBuf_Params_init ti_sysbios_heaps_HeapMultiBuf_Params_init
#define HeapMultiBuf_Instance_finalize ti_sysbios_heaps_HeapMultiBuf_Instance_finalize
#define HeapMultiBuf_delete ti_sysbios_heaps_HeapMultiBuf_delete
#define HeapMultiBuf_destruct ti_sysbios_heaps_HeapMultiBuf_destruct
#define HeapMultiBuf_Module_upCast ti_sysbios_heaps_HeapMultiBuf_Module_upCast
#define HeapMultiBuf_Module_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapMultiBuf_Module_to_xdc_runtime_IHeap
#define HeapMultiBuf_Handle_upCast ti_sysbios_heaps_HeapMultiBuf_Handle_upCast
#define HeapMultiBuf_Handle_to_xdc_runtime_IHeap ti_sysbios_heaps_HeapMultiBuf_Handle_to_xdc_runtime_IHeap
#define HeapMultiBuf_Handle_downCast ti_sysbios_heaps_HeapMultiBuf_Handle_downCast
#define HeapMultiBuf_Handle_from_xdc_runtime_IHeap ti_sysbios_heaps_HeapMultiBuf_Handle_from_xdc_runtime_IHeap
#define HeapMultiBuf_addrPairCompare ti_sysbios_heaps_HeapMultiBuf_addrPairCompare
#define HeapMultiBuf_borrowBlock ti_sysbios_heaps_HeapMultiBuf_borrowBlock
#define HeapMultiBuf_getHeapMultiBufFromObjElem ti_sysbios_heaps_HeapMultiBuf_getHeapMultiBufFromObjElem
#define HeapMultiBuf_init ti_sysbios_heaps_HeapMultiBuf_init
#define HeapMultiBuf_Module_state ti_sysbios_heaps_HeapMultiBuf_Module_state
#define HeapMultiBuf_Params_default ti_sysbios_heaps_HeapMultiBuf_Params_default
#define HeapMultiBuf_sizeAlignCompare ti_sysbios_heaps_HeapMultiBuf_sizeAlignCompare

#endif /* ti_sysbios_heaps_HeapMultiBuf_long_names */

#if defined(ti_sysbios_heaps_HeapMultiBuf__nolocalnames) && !defined(ti_sysbios_heaps_HeapMultiBuf_long_names)

#undef HeapMultiBuf_Instance
#undef HeapMultiBuf_Handle
#undef HeapMultiBuf_Module
#undef HeapMultiBuf_Object
#undef HeapMultiBuf_Struct
#undef HeapMultiBuf_AddrPair
#undef HeapMultiBuf_Instance_State
#undef HeapMultiBuf_Params
#undef HeapMultiBuf_getStats
#undef HeapMultiBuf_alloc
#undef HeapMultiBuf_free
#undef HeapMultiBuf_isBlocking
#undef HeapMultiBuf_Module_name
#undef HeapMultiBuf_Module_id
#undef HeapMultiBuf_Module_startup
#undef HeapMultiBuf_Module_startupDone
#undef HeapMultiBuf_Module_hasMask
#undef HeapMultiBuf_Module_getMask
#undef HeapMultiBuf_Module_setMask
#undef HeapMultiBuf_Object_heap
#undef HeapMultiBuf_Module_heap
#undef HeapMultiBuf_construct
#undef HeapMultiBuf_create
#undef HeapMultiBuf_handle
#undef HeapMultiBuf_struct
#undef HeapMultiBuf_Handle_label
#undef HeapMultiBuf_Handle_name
#undef HeapMultiBuf_Instance_init
#undef HeapMultiBuf_Object_count
#undef HeapMultiBuf_Object_get
#undef HeapMultiBuf_Object_first
#undef HeapMultiBuf_Object_next
#undef HeapMultiBuf_Object_sizeof
#undef HeapMultiBuf_Params_copy
#undef HeapMultiBuf_Params_init
#undef HeapMultiBuf_Instance_finalize
#undef HeapMultiBuf_delete
#undef HeapMultiBuf_destruct
#undef HeapMultiBuf_Module_upCast
#undef HeapMultiBuf_Module_to_xdc_runtime_IHeap
#undef HeapMultiBuf_Handle_upCast
#undef HeapMultiBuf_Handle_to_xdc_runtime_IHeap
#undef HeapMultiBuf_Handle_downCast
#undef HeapMultiBuf_Handle_from_xdc_runtime_IHeap
#undef HeapMultiBuf_addrPairCompare
#undef HeapMultiBuf_borrowBlock
#undef HeapMultiBuf_getHeapMultiBufFromObjElem
#undef HeapMultiBuf_init
#undef HeapMultiBuf_Module_state
#undef HeapMultiBuf_Params_default
#undef HeapMultiBuf_sizeAlignCompare

#endif

/* @endcond */
