/* Copyright Statement:
 *
 * (C) 2020-2036  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/**
 * @file FreeRTOSConfigDefault.h
 *
 *  FreeRTOSConfigDefault.h
 *
 */

#ifndef __FREERTOS_CONFIG_DEFAULT_H__
#define __FREERTOS_CONFIG_DEFAULT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Cortex M33 port configuration. */
#ifndef configENABLE_MPU
    #define configENABLE_MPU                           0
#endif

#ifndef configENABLE_FPU
    #define configENABLE_FPU                           1
#endif

#ifndef configENABLE_TRUSTZONE
    #define configENABLE_TRUSTZONE                     0
#endif

#ifndef configRUN_FREERTOS_SECURE_ONLY
#ifdef MTK_TFM_ENABLE
#define configRUN_FREERTOS_SECURE_ONLY                 0
#else
#define configRUN_FREERTOS_SECURE_ONLY                 1
#endif
#endif

/* Constants related to the behaviour or the scheduler. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
#define configUSE_PORT_OPTIMISED_TASK_SELECTION			   0
#endif

#ifndef configUSE_PREEMPTION
#define configUSE_PREEMPTION							             1
#endif

#ifndef configMAX_PRIORITIES
#define configMAX_PRIORITIES						               ( 20 )
#endif

#ifndef configIDLE_SHOULD_YIELD
#define configIDLE_SHOULD_YIELD							           1
#endif

#ifndef configUSE_16_BIT_TICKS
#define configUSE_16_BIT_TICKS							           0 /* Only for 8 and 16-bit hardware. */
#endif


/* Constants that describe the hardware and memory usage. */
#ifndef configCPU_CLOCK_HZ
#ifdef HAL_CLOCK_MODULE_ENABLED
extern uint32_t SystemCoreClock;
#define configCPU_CLOCK_HZ					             			SystemCoreClock
#else
#define configCPU_CLOCK_HZ						               	26000000 // default 26MHz
#endif
#endif

#ifndef configTICK_RATE_HZ
#define configTICK_RATE_HZ                            ( ( TickType_t ) 1000 )
#endif

#ifndef configMINIMAL_STACK_SIZE
#define configMINIMAL_STACK_SIZE					         	  ( ( uint16_t ) 256 )
#endif

#ifndef configMINIMAL_SECURE_STACK_SIZE
#define configMINIMAL_SECURE_STACK_SIZE					      ( 1024 )
#endif

#ifndef configMAX_TASK_NAME_LEN
#define configMAX_TASK_NAME_LEN			          				( 20 )
#endif

/* Constants that build features in or out. */
#ifndef configUSE_MUTEXES
#define configUSE_MUTEXES						              		1
#endif

#ifndef configUSE_TICKLESS_IDLE
#define configUSE_TICKLESS_IDLE					          		2
#endif

#ifndef configUSE_APPLICATION_TASK_TAG
#ifdef MTK_POSIX_SUPPORT_ENABLE
#define configUSE_POSIX_ERRNO                          1
#define configUSE_APPLICATION_TASK_TAG                 1
#else
#define configUSE_APPLICATION_TASK_TAG					       0
#endif
#endif

#ifndef configUSE_NEWLIB_REENTRANT
#define configUSE_NEWLIB_REENTRANT						         1
#endif

#ifndef configUSE_CO_ROUTINES
#define configUSE_CO_ROUTINES							             0
#endif

#ifndef configUSE_COUNTING_SEMAPHORES
#define configUSE_COUNTING_SEMAPHORES					         1
#endif

#ifndef configUSE_RECURSIVE_MUTEXES
#define configUSE_RECURSIVE_MUTEXES						         1
#endif

#ifndef configUSE_QUEUE_SETS
#define configUSE_QUEUE_SETS							             1
#endif

#ifndef configUSE_TASK_NOTIFICATIONS
#define configUSE_TASK_NOTIFICATIONS					         1
#endif

#ifndef configUSE_TRACE_FACILITY
#define configUSE_TRACE_FACILITY						           1
#endif

#ifndef configSUPPORT_STATIC_ALLOCATION
#if defined(MTK_TFM_ENABLE) || defined(MTK_POSIX_SUPPORT_ENABLE)
#define configSUPPORT_STATIC_ALLOCATION         1
#endif
#endif

#if (MTK_OS_HEAP_EXTEND == HEAP_EXTEND_MULTI)
#ifndef configMAX_HEAP_REGION
#define configMAX_HEAP_REGION                          (eHeapRegion_Default_MAX+1)
#endif

#ifndef configUSE_HEAP_REGION_DEFAULT
#define configUSE_HEAP_REGION_DEFAULT                  eHeapRegion_PLATFORM
#endif

#ifndef configTOTAL_HEAP_SIZE
#define configTOTAL_HEAP_SIZE                          ((size_t)(100 * 1024))
#endif

#ifndef configPlatform_HEAP_SIZE
#define configPlatform_HEAP_SIZE                       ((size_t)(500 * 1024))
#endif

#else
#ifndef configTOTAL_HEAP_SIZE
#define configTOTAL_HEAP_SIZE                          ((size_t)(100 * 1024))
#endif
#endif // #if (MTK_OS_HEAP_EXTEND == HEAP_EXTEND_MULTI)

//#ifndef configUSE_MALLOC_FAILED_HOOK
//#define configUSE_MALLOC_FAILED_HOOK                 1
//#endif

#ifndef configUSE_FREE_FAILED_HOOK
#define configUSE_FREE_FAILED_HOOK                     1
#endif

/* Constants that define which hook (callback) functions should be used. */
#ifndef configUSE_IDLE_HOOK
#define configUSE_IDLE_HOOK							               1
#endif

#ifndef configUSE_TICK_HOOK
#define configUSE_TICK_HOOK						                 0
#endif

#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
/* Run time stats gathering definitions. */
void vConfigureTimerForRunTimeStats(void);
uint32_t ulGetRunTimeCounterValue(void);

#ifndef configGENERATE_RUN_TIME_STATS
#define configGENERATE_RUN_TIME_STATS                   1
#endif

#ifndef portCONFIGURE_TIMER_FOR_RUN_TIME_STATS
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()        vConfigureTimerForRunTimeStats()
#endif

#ifndef portGET_RUN_TIME_COUNTER_VALUE
#define portGET_RUN_TIME_COUNTER_VALUE()                ulGetRunTimeCounterValue()
#endif
#endif //#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)

/* Software timer definitions. */
#ifndef configUSE_TIMERS
#define configUSE_TIMERS							                	1
#endif

#ifndef configTIMER_TASK_PRIORITY
#define configTIMER_TASK_PRIORITY					              (configMAX_PRIORITIES - 1)
#endif

#ifndef configTIMER_QUEUE_LENGTH
#define configTIMER_QUEUE_LENGTH						            10
#endif

#ifndef configTIMER_TASK_STACK_DEPTH
#define configTIMER_TASK_STACK_DEPTH				            ( configMINIMAL_STACK_SIZE * 2 )
#endif

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function.  NOTE:  Setting an INCLUDE_ parameter to 0 is
 * only necessary if the linker does not automatically remove functions that are
 * not referenced anyway. */
#ifndef INCLUDE_vTaskPrioritySet
#define INCLUDE_vTaskPrioritySet						            1
#endif
#ifndef INCLUDE_uxTaskPriorityGet
#define INCLUDE_uxTaskPriorityGet					              1
#endif
#ifndef INCLUDE_vTaskDelete
#define INCLUDE_vTaskDelete								              1
#endif
#ifndef INCLUDE_vTaskSuspend
#define INCLUDE_vTaskSuspend							              1
#endif

#ifndef INCLUDE_vTaskDelayUntil
#ifndef INCLUDE_xTaskDelayUntil
#define INCLUDE_xTaskDelayUntil           							1
#endif
#endif

#ifndef INCLUDE_vTaskDelay
#define INCLUDE_vTaskDelay								              1
#endif
#ifndef INCLUDE_uxTaskGetStackHighWaterMark
#define INCLUDE_uxTaskGetStackHighWaterMark			      	1
#endif
#ifndef INCLUDE_xTaskGetIdleTaskHandle
#define INCLUDE_xTaskGetIdleTaskHandle					        1         
#endif
#ifndef INCLUDE_eTaskGetState
#define INCLUDE_eTaskGetState							              1
#endif
#ifndef INCLUDE_xTaskResumeFromISR
#define INCLUDE_xTaskResumeFromISR						          1
#endif
#ifndef INCLUDE_xTaskGetCurrentTaskHandle
#define INCLUDE_xTaskGetCurrentTaskHandle				        1
#endif
#ifndef INCLUDE_xTaskGetSchedulerState
#define INCLUDE_xTaskGetSchedulerState					        1
#endif
#ifndef INCLUDE_xSemaphoreGetMutexHolder
#define INCLUDE_xSemaphoreGetMutexHolder				        1
#endif
#ifndef INCLUDE_xTimerPendFunctionCall
#define INCLUDE_xTimerPendFunctionCall					        1
#endif

/* This demo makes use of one or more example stats formatting functions.  These
 * format the raw data provided by the uxTaskGetSystemState() function in to
 * human readable ASCII form.  See the notes in the implementation of vTaskList()
 * within FreeRTOS/Source/tasks.c for limitations. */
#ifndef configUSE_STATS_FORMATTING_FUNCTIONS
#define configUSE_STATS_FORMATTING_FUNCTIONS			1
#endif


/* Interrupt priority configuration follows...................... */

/* Use the system definition, if there is one. */
#ifndef configPRIO_BITS
#ifdef __NVIC_PRIO_BITS
	#define configPRIO_BITS								                __NVIC_PRIO_BITS
#else
	#define configPRIO_BITS								                5	 /* 32 priority levels. */
#endif
#endif

/* The highest interrupt priority that can be used by any interrupt service
 * routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT
 * CALL INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A
 * HIGHER PRIORITY THAN THIS! (higher priorities are lower numeric values). */
#ifndef configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	  5
#endif


/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
 * See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#ifndef configMAX_SYSCALL_INTERRUPT_PRIORITY
#define configMAX_SYSCALL_INTERRUPT_PRIORITY		      	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )
#endif


#ifndef configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H
#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H    1
#define FREERTOS_TASKS_C_ADDITIONS_INIT              vApplicationTaskInit
#endif

#ifndef configASSERT
extern void platform_assert(const char *, const char *, int);
#define configASSERT( x )                                      if( (x) == 0 ) { platform_assert(#x, __FILE__, __LINE__); }
#endif

#ifndef portSUPPRESS_TICKS_AND_SLEEP
#if configUSE_TICKLESS_IDLE == 2
extern void tickless_handler( uint32_t xExpectedIdleTime );
#define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime )      tickless_handler( xExpectedIdleTime )
#ifndef TICKLESS_CONFIG
#define TICKLESS_CONFIG xDefaultTicklessConfig
#endif // TICKLESS_CONFIG
#endif
#endif

#if defined(MTK_SWLA_ENABLE)
    #undef traceTASK_SWITCHED_IN
    extern void vTraceTaskSwitchIn(uint32_t pRio);
    #define traceTASK_SWITCHED_IN() \
    {\
        vTraceTaskSwitchIn(pxCurrentTCB->uxPriority);\
    }
#endif /* MTK_SWLA_ENABLE*/

#ifndef configNUM_THREAD_LOCAL_STORAGE_POINTERS
    #define configNUM_THREAD_LOCAL_STORAGE_POINTERS    1
#endif

#if defined(MTK_OS_TIMER_LIST_ENABLE)
extern void vTraceTimerCreate(void * pxNewTimer);
#define traceTIMER_CREATE( pxNewTimer ) vTraceTimerCreate( pxNewTimer )
#endif

#if defined(MTK_OS_SEMAPHORE_LIST_ENABLE)
#undef configQUEUE_REGISTRY_SIZE
#define configQUEUE_REGISTRY_SIZE    64U

extern void vTraceQueueCreate(void * pxNewQueue);
#define traceQUEUE_CREATE( pxNewQueue ) vTraceQueueCreate( pxNewQueue )

extern void vTraceQueueSend(void * xQueue);
#define traceQUEUE_SEND( xQueue ) vTraceQueueSend( xQueue )

extern void vTraceQueueReceive(void * xQueue);
#define traceQUEUE_RECEIVE( xQueue ) vTraceQueueReceive( xQueue )

extern void vTraceQueueSendFromISR(void * xQueue);
#define traceQUEUE_SEND_FROM_ISR( xQueue ) vTraceQueueSendFromISR( xQueue )

extern void vTraceQueueReceiveFromISR(void * xQueue);
#define traceQUEUE_RECEIVE_FROM_ISR( xQueue ) vTraceQueueReceiveFromISR( xQueue )

extern void vTraceQueueSendFailed(void * xQueue);
#define traceQUEUE_SEND_FAILED( xQueue ) vTraceQueueSendFailed( xQueue )

extern void vTraceQueueReceiveFailed(void * xQueue);
#define traceQUEUE_RECEIVE_FAILED( xQueue ) vTraceQueueReceiveFailed( xQueue )

extern void vTraceQueueSendFromISRFailed(void * xQueue);
#define traceQUEUE_SEND_FROM_ISR_FAILED( xQueue ) vTraceQueueSendFromISRFailed( xQueue )

extern void vTraceQueueReceiveFromISRFailed(void * xQueue);
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( xQueue ) vTraceQueueReceiveFromISRFailed( xQueue )

extern void vTraceQueueDelete(void* xQueue);
#define traceQUEUE_DELETE( xQueue ) vTraceQueueDelete( xQueue )

#endif

#ifdef __cplusplus
}
#endif

#endif /* __FREERTOS_CONFIG_DEFAULT_H__ */
