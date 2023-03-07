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
 *  ======== TaskSupport_defs.h ========
 */

#ifdef ti_sysbios_knl_TaskSupport_long_names
#define TaskSupport_FuncPtr ti_sysbios_knl_TaskSupport_FuncPtr
#define TaskSupport_stackAlignment ti_sysbios_knl_TaskSupport_stackAlignment
#define TaskSupport_swap ti_sysbios_knl_TaskSupport_swap
#define TaskSupport_start ti_sysbios_knl_TaskSupport_start
#define TaskSupport_checkStack ti_sysbios_knl_TaskSupport_checkStack
#define TaskSupport_stackUsed ti_sysbios_knl_TaskSupport_stackUsed
#define TaskSupport_getStackAlignment ti_sysbios_knl_TaskSupport_getStackAlignment
#define TaskSupport_init ti_sysbios_knl_TaskSupport_init
#define TaskSupport_buildTaskStack ti_sysbios_knl_TaskSupport_buildTaskStack
#define TaskSupport_glue ti_sysbios_knl_TaskSupport_glue
#endif

#if defined(ti_sysbios_knl_TaskSupport__nolocalnames) && !defined(ti_sysbios_knl_TaskSupport_long_names)
#undef TaskSupport_FuncPtr
#undef TaskSupport_stackAlignment
#undef TaskSupport_swap
#undef TaskSupport_start
#undef TaskSupport_checkStack
#undef TaskSupport_stackUsed
#undef TaskSupport_getStackAlignment
#undef TaskSupport_init
#undef TaskSupport_buildTaskStack
#undef TaskSupport_glue
#endif

/* @endcond */
