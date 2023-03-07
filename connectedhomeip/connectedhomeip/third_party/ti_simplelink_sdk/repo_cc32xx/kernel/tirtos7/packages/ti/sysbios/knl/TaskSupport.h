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

/*
 * ======== TaskSupport.h ========
 */

#ifndef ti_sysbios_knl_TaskSupport__include
#define ti_sysbios_knl_TaskSupport__include

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ======== INCLUDES ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/runtime/Error.h>

#define ti_sysbios_knl_TaskSupport_long_names
#include <ti/sysbios/knl/TaskSupport_defs.h>

/*
 * ======== TaskSupport APIs ========
 */

/* TaskSupport_FuncPtr */
typedef void (*TaskSupport_FuncPtr)(void);

/* stackAlignment */
extern const unsigned int TaskSupport_stackAlignment;

/* TaskSupport_swap */
extern void TaskSupport_swap(void * prevContext, void * nextContext);

/*
 *  ======== TaskSupport_start ========
 *  Create a task's initial stack image
 */
extern void * TaskSupport_start(void * currTsk, TaskSupport_FuncPtr enter, TaskSupport_FuncPtr exit, Error_Block *eb);

/*
 *  ======== TaskSupport_checkStack ========
 *  Check for stack overflow.
 */
extern bool TaskSupport_checkStack(char *stack, size_t size);

/*
 *  ======== TaskSupport_stackUsed ========
 *  Returns the task stack usage
 *
 *  @param(task)     Task_Object void *
 */
extern size_t TaskSupport_stackUsed(char *stack, size_t size);

/*
 *  ======== TaskSupport_getStackAlignment ========
 */
extern unsigned int TaskSupport_getStackAlignment();

/*
 *  ======== TaskSupport_init ========
 */
extern void TaskSupport_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_knl_TaskSupport__include */

#undef ti_sysbios_knl_TaskSupport_long_names
#include <ti/sysbios/knl/TaskSupport_defs.h>

