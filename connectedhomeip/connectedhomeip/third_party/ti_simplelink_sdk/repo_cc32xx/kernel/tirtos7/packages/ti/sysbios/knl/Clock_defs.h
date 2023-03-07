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
 * ======== Clock_defs.h ========
 */

/* @cond LONG_NAMES */

#ifdef ti_sysbios_knl_Clock_long_names

#define Clock_Instance ti_sysbios_knl_Clock_Instance
#define Clock_Handle ti_sysbios_knl_Clock_Handle
#define Clock_Module ti_sysbios_knl_Clock_Module
#define Clock_Object ti_sysbios_knl_Clock_Object
#define Clock_Struct ti_sysbios_knl_Clock_Struct
#define Clock_TickSource ti_sysbios_knl_Clock_TickSource
#define Clock_TickMode ti_sysbios_knl_Clock_TickMode
#define Clock_FuncPtr ti_sysbios_knl_Clock_FuncPtr
#define Clock_Instance_State ti_sysbios_knl_Clock_Instance_State
#define Clock_Module_State ti_sysbios_knl_Clock_Module_State
#define Clock_TickSource_TIMER ti_sysbios_knl_Clock_TickSource_TIMER
#define Clock_TickSource_USER ti_sysbios_knl_Clock_TickSource_USER
#define Clock_TickSource_NULL ti_sysbios_knl_Clock_TickSource_NULL
#define Clock_TickMode_PERIODIC ti_sysbios_knl_Clock_TickMode_PERIODIC
#define Clock_TickMode_DYNAMIC ti_sysbios_knl_Clock_TickMode_DYNAMIC
#define Clock_serviceMargin ti_sysbios_knl_Clock_serviceMargin
#define Clock_tickSource ti_sysbios_knl_Clock_tickSource
#define Clock_tickMode ti_sysbios_knl_Clock_tickMode
#define Clock_timerId ti_sysbios_knl_Clock_timerId
#define Clock_tickPeriod ti_sysbios_knl_Clock_tickPeriod
#define Clock_doTickFunc ti_sysbios_knl_Clock_doTickFunc
#define Clock_triggerClock ti_sysbios_knl_Clock_triggerClock
#define Clock_Module_State_clockQ ti_sysbios_knl_Clock_Module_State_clockQ
#define Clock_Params ti_sysbios_knl_Clock_Params
#define Clock_getTicks ti_sysbios_knl_Clock_getTicks
#define Clock_getTimerHandle ti_sysbios_knl_Clock_getTimerHandle
#define Clock_setTicks ti_sysbios_knl_Clock_setTicks
#define Clock_tickStop ti_sysbios_knl_Clock_tickStop
#define Clock_tickReconfig ti_sysbios_knl_Clock_tickReconfig
#define Clock_tickStart ti_sysbios_knl_Clock_tickStart
#define Clock_tick ti_sysbios_knl_Clock_tick
#define Clock_workFunc ti_sysbios_knl_Clock_workFunc
#define Clock_workFuncDynamic ti_sysbios_knl_Clock_workFuncDynamic
#define Clock_logTick ti_sysbios_knl_Clock_logTick
#define Clock_getCompletedTicks ti_sysbios_knl_Clock_getCompletedTicks
#define Clock_getTickPeriod ti_sysbios_knl_Clock_getTickPeriod
#define Clock_getTicksUntilInterrupt ti_sysbios_knl_Clock_getTicksUntilInterrupt
#define Clock_getTicksUntilTimeout ti_sysbios_knl_Clock_getTicksUntilTimeout
#define Clock_walkQueueDynamic ti_sysbios_knl_Clock_walkQueueDynamic
#define Clock_walkQueuePeriodic ti_sysbios_knl_Clock_walkQueuePeriodic
#define Clock_scheduleNextTick ti_sysbios_knl_Clock_scheduleNextTick
#define Clock_addI ti_sysbios_knl_Clock_addI
#define Clock_removeI ti_sysbios_knl_Clock_removeI
#define Clock_start ti_sysbios_knl_Clock_start
#define Clock_startI ti_sysbios_knl_Clock_startI
#define Clock_stop ti_sysbios_knl_Clock_stop
#define Clock_setPeriod ti_sysbios_knl_Clock_setPeriod
#define Clock_setTimeout ti_sysbios_knl_Clock_setTimeout
#define Clock_setFunc ti_sysbios_knl_Clock_setFunc
#define Clock_getPeriod ti_sysbios_knl_Clock_getPeriod
#define Clock_getTimeout ti_sysbios_knl_Clock_getTimeout
#define Clock_isActive ti_sysbios_knl_Clock_isActive
#define Clock_Module_startup ti_sysbios_knl_Clock_Module_startup
#define Clock_Module_startupDone ti_sysbios_knl_Clock_Module_startupDone
#define Clock_construct ti_sysbios_knl_Clock_construct
#define Clock_create ti_sysbios_knl_Clock_create
#define Clock_handle ti_sysbios_knl_Clock_handle
#define Clock_struct ti_sysbios_knl_Clock_struct
#define Clock_Instance_init ti_sysbios_knl_Clock_Instance_init
#define Clock_Object_count ti_sysbios_knl_Clock_Object_count
#define Clock_Object_get ti_sysbios_knl_Clock_Object_get
#define Clock_Object_first ti_sysbios_knl_Clock_Object_first
#define Clock_Object_next ti_sysbios_knl_Clock_Object_next
#define Clock_Object_sizeof ti_sysbios_knl_Clock_Object_sizeof
#define Clock_Params_init ti_sysbios_knl_Clock_Params_init
#define Clock_Instance_finalize ti_sysbios_knl_Clock_Instance_finalize
#define Clock_delete ti_sysbios_knl_Clock_delete
#define Clock_destruct ti_sysbios_knl_Clock_destruct
#define Clock_init ti_sysbios_knl_Clock_init
#define Clock_Module_state ti_sysbios_knl_Clock_Module_state
#define Clock_enqueueI ti_sysbios_knl_Clock_enqueueI
#define Clock_getClockFromObjElem ti_sysbios_knl_Clock_getClockFromObjElem
#define Clock_initI ti_sysbios_knl_Clock_initI
#define Clock_Params_default ti_sysbios_knl_Clock_Params_default
#define Clock_swiPriority ti_sysbios_knl_Clock_swiPriority
#define Clock_triggerFunc ti_sysbios_knl_Clock_triggerFunc

#endif

#if defined(ti_sysbios_knl_Clock__nolocalnames) && !defined(ti_sysbios_knl_Clock_long_names)

#undef Clock_Instance
#undef Clock_Handle
#undef Clock_Module
#undef Clock_Object
#undef Clock_Struct
#undef Clock_TickSource
#undef Clock_TickMode
#undef Clock_FuncPtr
#undef Clock_Instance_State
#undef Clock_Module_State
#undef Clock_TickSource_TIMER
#undef Clock_TickSource_USER
#undef Clock_TickSource_NULL
#undef Clock_TickMode_PERIODIC
#undef Clock_TickMode_DYNAMIC
#undef Clock_serviceMargin
#undef Clock_tickSource
#undef Clock_tickMode
#undef Clock_timerId
#undef Clock_tickPeriod
#undef Clock_doTickFunc
#undef Clock_triggerClock
#undef Clock_Module_State_clockQ
#undef Clock_Params
#undef Clock_getTicks
#undef Clock_getTimerHandle
#undef Clock_setTicks
#undef Clock_tickStop
#undef Clock_tickReconfig
#undef Clock_tickStart
#undef Clock_tick
#undef Clock_workFunc
#undef Clock_workFuncDynamic
#undef Clock_logTick
#undef Clock_getCompletedTicks
#undef Clock_getTickPeriod
#undef Clock_getTicksUntilInterrupt
#undef Clock_getTicksUntilTimeout
#undef Clock_walkQueueDynamic
#undef Clock_walkQueuePeriodic
#undef Clock_scheduleNextTick
#undef Clock_addI
#undef Clock_removeI
#undef Clock_start
#undef Clock_startI
#undef Clock_stop
#undef Clock_setPeriod
#undef Clock_setTimeout
#undef Clock_setFunc
#undef Clock_getPeriod
#undef Clock_getTimeout
#undef Clock_isActive
#undef Clock_Module_startup
#undef Clock_Module_startupDone
#undef Clock_construct
#undef Clock_create
#undef Clock_handle
#undef Clock_struct
#undef Clock_Instance_init
#undef Clock_Object_count
#undef Clock_Object_get
#undef Clock_Object_first
#undef Clock_Object_next
#undef Clock_Object_sizeof
#undef Clock_Params_init
#undef Clock_Instance_finalize
#undef Clock_delete
#undef Clock_destruct
#undef Clock_init
#undef Clock_Module_state
#undef Clock_enqueueI
#undef Clock_getClockFromObjElem
#undef Clock_initI
#undef Clock_Params_default
#undef Clock_swiPriority
#undef Clock_triggerFunc

#endif

/* @endcond */
