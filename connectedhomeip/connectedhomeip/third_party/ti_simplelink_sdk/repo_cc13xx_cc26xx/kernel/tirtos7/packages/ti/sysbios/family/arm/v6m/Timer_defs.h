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

/* @cond LONG_NAMES */

/*
 * ======== Timer_defs.h ========
 */

/*
 * Remove any previous short name definitions already provided by
 * other Timer.h's in this compilation unit.
 */
#ifdef do_timer_undef_short_names
#undef do_timer_undef_short_names
#undef Timer_Instance
#undef Timer_Handle
#undef Timer_Module
#undef Timer_Object
#undef Timer_Struct
#undef Timer_FuncPtr
#undef Timer_ANY
#undef Timer_StartMode
#undef Timer_RunMode
#undef Timer_Status
#undef Timer_PeriodType
#undef Timer_MAX_PERIOD
#undef Timer_NUM_TIMER_DEVICES
#undef Timer_TIMER_CLOCK_DIVIDER
#undef Timer_MIN_SWEEP_PERIOD
#undef Timer_Instance_State
#undef Timer_Module_State
#undef Timer_StartMode_AUTO
#undef Timer_StartMode_USER
#undef Timer_RunMode_CONTINUOUS
#undef Timer_RunMode_ONESHOT
#undef Timer_RunMode_DYNAMIC
#undef Timer_Status_INUSE
#undef Timer_Status_FREE
#undef Timer_PeriodType_MICROSECS
#undef Timer_PeriodType_COUNTS
#undef Timer_anyMask
#undef Timer_startupNeeded
#undef Timer_Params
#undef Timer_getNumTimers
#undef Timer_getStatus
#undef Timer_startup
#undef Timer_getMaxTicks
#undef Timer_setNextTick
#undef Timer_start
#undef Timer_stop
#undef Timer_setPeriod
#undef Timer_setPeriodMicroSecs
#undef Timer_getPeriod
#undef Timer_getCount
#undef Timer_getFreq
#undef Timer_getFunc
#undef Timer_setFunc
#undef Timer_trigger
#undef Timer_getExpiredCounts
#undef Timer_getExpiredTicks
#undef Timer_getCurrentTick
#undef Timer_getHandle
#undef Timer_getTickCount
#undef Timer_reconfig
#undef Timer_Module_name
#undef Timer_Module_id
#undef Timer_Module_startup
#undef Timer_Module_startupDone
#undef Timer_Module_hasMask
#undef Timer_Module_getMask
#undef Timer_Module_setMask
#undef Timer_Object_heap
#undef Timer_Module_heap
#undef Timer_construct
#undef Timer_create
#undef Timer_handle
#undef Timer_struct
#undef Timer_Handle_label
#undef Timer_Handle_name
#undef Timer_Instance_init
#undef Timer_Object_count
#undef Timer_Object_get
#undef Timer_Object_first
#undef Timer_Object_next
#undef Timer_Object_sizeof
#undef Timer_Params_copy
#undef Timer_Params_init
#undef Timer_Instance_finalize
#undef Timer_delete
#undef Timer_destruct
#undef Timer_oneShotStub
#undef Timer_periodicStub
#undef Timer_initDevice
#undef Timer_Module_state
#undef Timer_Params_default
#undef Timer_postInit
#undef Timer_getTimerFromObjElem  
#endif /* do_timer_undef_short_names */

/*
 * Map short names to long names
 */
#ifdef do_timer_short_to_long_name_conversion
#undef do_timer_short_to_long_name_conversion
#define Timer_Instance ti_sysbios_family_arm_v6m_Timer_Instance
#define Timer_Handle ti_sysbios_family_arm_v6m_Timer_Handle
#define Timer_Module ti_sysbios_family_arm_v6m_Timer_Module
#define Timer_Object ti_sysbios_family_arm_v6m_Timer_Object
#define Timer_Struct ti_sysbios_family_arm_v6m_Timer_Struct
#define Timer_FuncPtr ti_sysbios_family_arm_v6m_Timer_FuncPtr
#define Timer_ANY ti_sysbios_family_arm_v6m_Timer_ANY
#define Timer_StartMode ti_sysbios_family_arm_v6m_Timer_StartMode
#define Timer_RunMode ti_sysbios_family_arm_v6m_Timer_RunMode
#define Timer_Status ti_sysbios_family_arm_v6m_Timer_Status
#define Timer_PeriodType ti_sysbios_family_arm_v6m_Timer_PeriodType
#define Timer_MAX_PERIOD ti_sysbios_family_arm_v6m_Timer_MAX_PERIOD
#define Timer_NUM_TIMER_DEVICES ti_sysbios_family_arm_v6m_Timer_NUM_TIMER_DEVICES
#define Timer_TIMER_CLOCK_DIVIDER ti_sysbios_family_arm_v6m_Timer_TIMER_CLOCK_DIVIDER
#define Timer_MIN_SWEEP_PERIOD ti_sysbios_family_arm_v6m_Timer_MIN_SWEEP_PERIOD
#define Timer_Instance_State ti_sysbios_family_arm_v6m_Timer_Instance_State
#define Timer_Module_State ti_sysbios_family_arm_v6m_Timer_Module_State
#define Timer_StartMode_AUTO ti_sysbios_family_arm_v6m_Timer_StartMode_AUTO
#define Timer_StartMode_USER ti_sysbios_family_arm_v6m_Timer_StartMode_USER
#define Timer_RunMode_CONTINUOUS ti_sysbios_family_arm_v6m_Timer_RunMode_CONTINUOUS
#define Timer_RunMode_ONESHOT ti_sysbios_family_arm_v6m_Timer_RunMode_ONESHOT
#define Timer_RunMode_DYNAMIC ti_sysbios_family_arm_v6m_Timer_RunMode_DYNAMIC
#define Timer_Status_INUSE ti_sysbios_family_arm_v6m_Timer_Status_INUSE
#define Timer_Status_FREE ti_sysbios_family_arm_v6m_Timer_Status_FREE
#define Timer_PeriodType_MICROSECS ti_sysbios_family_arm_v6m_Timer_PeriodType_MICROSECS
#define Timer_PeriodType_COUNTS ti_sysbios_family_arm_v6m_Timer_PeriodType_COUNTS
#define Timer_anyMask ti_sysbios_family_arm_v6m_Timer_anyMask
#define Timer_startupNeeded ti_sysbios_family_arm_v6m_Timer_startupNeeded
#define Timer_Params ti_sysbios_family_arm_v6m_Timer_Params
#define Timer_getNumTimers ti_sysbios_family_arm_v6m_Timer_getNumTimers
#define Timer_getStatus ti_sysbios_family_arm_v6m_Timer_getStatus
#define Timer_startup ti_sysbios_family_arm_v6m_Timer_startup
#define Timer_getMaxTicks ti_sysbios_family_arm_v6m_Timer_getMaxTicks
#define Timer_setNextTick ti_sysbios_family_arm_v6m_Timer_setNextTick
#define Timer_start ti_sysbios_family_arm_v6m_Timer_start
#define Timer_stop ti_sysbios_family_arm_v6m_Timer_stop
#define Timer_setPeriod ti_sysbios_family_arm_v6m_Timer_setPeriod
#define Timer_setPeriodMicroSecs ti_sysbios_family_arm_v6m_Timer_setPeriodMicroSecs
#define Timer_getPeriod ti_sysbios_family_arm_v6m_Timer_getPeriod
#define Timer_getCount ti_sysbios_family_arm_v6m_Timer_getCount
#define Timer_getFreq ti_sysbios_family_arm_v6m_Timer_getFreq
#define Timer_getFunc ti_sysbios_family_arm_v6m_Timer_getFunc
#define Timer_setFunc ti_sysbios_family_arm_v6m_Timer_setFunc
#define Timer_trigger ti_sysbios_family_arm_v6m_Timer_trigger
#define Timer_getExpiredCounts ti_sysbios_family_arm_v6m_Timer_getExpiredCounts
#define Timer_getExpiredTicks ti_sysbios_family_arm_v6m_Timer_getExpiredTicks
#define Timer_getCurrentTick ti_sysbios_family_arm_v6m_Timer_getCurrentTick
#define Timer_getHandle ti_sysbios_family_arm_v6m_Timer_getHandle
#define Timer_getTickCount ti_sysbios_family_arm_v6m_Timer_getTickCount
#define Timer_reconfig ti_sysbios_family_arm_v6m_Timer_reconfig
#define Timer_Module_name ti_sysbios_family_arm_v6m_Timer_Module_name
#define Timer_Module_id ti_sysbios_family_arm_v6m_Timer_Module_id
#define Timer_Module_startup ti_sysbios_family_arm_v6m_Timer_Module_startup
#define Timer_Module_startupDone ti_sysbios_family_arm_v6m_Timer_Module_startupDone
#define Timer_Module_hasMask ti_sysbios_family_arm_v6m_Timer_Module_hasMask
#define Timer_Module_getMask ti_sysbios_family_arm_v6m_Timer_Module_getMask
#define Timer_Module_setMask ti_sysbios_family_arm_v6m_Timer_Module_setMask
#define Timer_Object_heap ti_sysbios_family_arm_v6m_Timer_Object_heap
#define Timer_Module_heap ti_sysbios_family_arm_v6m_Timer_Module_heap
#define Timer_construct ti_sysbios_family_arm_v6m_Timer_construct
#define Timer_create ti_sysbios_family_arm_v6m_Timer_create
#define Timer_handle ti_sysbios_family_arm_v6m_Timer_handle
#define Timer_struct ti_sysbios_family_arm_v6m_Timer_struct
#define Timer_Handle_label ti_sysbios_family_arm_v6m_Timer_Handle_label
#define Timer_Handle_name ti_sysbios_family_arm_v6m_Timer_Handle_name
#define Timer_Instance_init ti_sysbios_family_arm_v6m_Timer_Instance_init
#define Timer_Object_count ti_sysbios_family_arm_v6m_Timer_Object_count
#define Timer_Object_get ti_sysbios_family_arm_v6m_Timer_Object_get
#define Timer_Object_first ti_sysbios_family_arm_v6m_Timer_Object_first
#define Timer_Object_next ti_sysbios_family_arm_v6m_Timer_Object_next
#define Timer_Object_sizeof ti_sysbios_family_arm_v6m_Timer_Object_sizeof
#define Timer_Params_copy ti_sysbios_family_arm_v6m_Timer_Params_copy
#define Timer_Params_init ti_sysbios_family_arm_v6m_Timer_Params_init
#define Timer_Instance_finalize ti_sysbios_family_arm_v6m_Timer_Instance_finalize
#define Timer_delete ti_sysbios_family_arm_v6m_Timer_delete
#define Timer_destruct ti_sysbios_family_arm_v6m_Timer_destruct
#define Timer_oneShotStub ti_sysbios_family_arm_v6m_Timer_oneShotStub
#define Timer_periodicStub ti_sysbios_family_arm_v6m_Timer_periodicStub
#define Timer_initDevice ti_sysbios_family_arm_v6m_Timer_initDevice
#define Timer_Module_state ti_sysbios_family_arm_v6m_Timer_Module_state
#define Timer_Params_default ti_sysbios_family_arm_v6m_Timer_Params_default
#define Timer_postInit ti_sysbios_family_arm_v6m_Timer_postInit
#define Timer_getTimerFromObjElem  ti_sysbios_family_arm_v6m_Timer_getTimerFromObjElem

#define ti_sysbios_family_arm_v6m_Timer_MAX_PERIOD (0xFFFFFFFF)
#define ti_sysbios_family_arm_v6m_Timer_MIN_SWEEP_PERIOD (1)
#define ti_sysbios_family_arm_v6m_Timer_ANY (~0U)
#define ti_sysbios_family_arm_v6m_Timer_NUM_TIMER_DEVICES 1
#endif /* ti_sysbios_family_arm_v6m_Timer__nolocalnames */

/* @endcond */
