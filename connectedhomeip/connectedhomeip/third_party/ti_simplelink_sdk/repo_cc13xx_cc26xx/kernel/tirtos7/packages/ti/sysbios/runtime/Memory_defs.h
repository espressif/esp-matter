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
 * ======== Memory_defs.h ========
 */

/* @cond LONG_NAMES */

#ifdef ti_sysbios_runtime_Memory_long_names

#define IHeap_AllocFxn ti_sysbios_runtime_IHeap_AllocFxn
#define IHeap_FreeFxn ti_sysbios_runtime_IHeap_FreeFxn
#define IHeap_IsBlockingFxn ti_sysbios_runtime_IHeap_IsBlockingFxn
#define IHeap_GetStatsFxn ti_sysbios_runtime_IHeap_GetStatsFxn
#define IHeap_Object ti_sysbios_runtime_IHeap_Object

#define Memory_Stats ti_sysbios_runtime_Memory_Stats
#define Memory_defaultHeapInstance ti_sysbios_runtime_Memory_defaultHeapInstance
#define Memory_alloc ti_sysbios_runtime_Memory_alloc
#define Memory_calloc ti_sysbios_runtime_Memory_calloc
#define Memory_free ti_sysbios_runtime_Memory_free
#define Memory_getMaxDefaultTypeAlign ti_sysbios_runtime_Memory_getMaxDefaultTypeAlign
#define Memory_getStats ti_sysbios_runtime_Memory_getStats
#define Memory_query ti_sysbios_runtime_Memory_query
#define Memory_valloc ti_sysbios_runtime_Memory_valloc
#define Memory_getDefaultHeap ti_sysbios_runtime_Memory_getDefaultHeap
#define Memory_setDefaultHeap ti_sysbios_runtime_Memory_setDefaultHeap

#endif

#if defined(ti_sysbios_runtime_Memory__nolocalnames) && !defined(ti_sysbios_runtime_Memory_long_names)

#undef IHeap_FreeFxn
#undef IHeap_IsBlockingFxn
#undef IHeap_GetStatsFxn
#undef IHeap_Object
#undef IHeap_Object

#undef Memory_Stats
#undef Memory_defaultHeapInstance
#undef Memory_alloc
#undef Memory_calloc
#undef Memory_free
#undef Memory_getMaxDefaultTypeAlign
#undef Memory_getStats
#undef Memory_query
#undef Memory_valloc
#undef Memory_getDefaultHeap
#undef Memory_setDefaultHeap

#endif

/* @endcond */
