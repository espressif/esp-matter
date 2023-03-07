/*
 * Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#if (defined(__ARMCC_VERSION) || defined(__GNUC__) || defined(__ICCARM__))
#include <stdint.h>

extern uint32_t SystemCoreClock;
#endif

/* Hardware features */
#define configENABLE_MPU       0
#define configENABLE_FPU       0
#define configENABLE_TRUSTZONE 0

/* Scheduling */
#define configCPU_CLOCK_HZ                      (SystemCoreClock)
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configMAX_PRIORITIES                    5
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_16_BIT_TICKS                  0
#define configRUN_FREERTOS_SECURE_ONLY          1

/* Stack and heap */
#define configMINIMAL_STACK_SIZE        (uint16_t)128
#define configMINIMAL_SECURE_STACK_SIZE 1024
#define configTOTAL_HEAP_SIZE           (size_t)(50 * 1024)
#define configMAX_TASK_NAME_LEN         12

/* OS features */
#define configUSE_MUTEXES              1
#define configUSE_TICKLESS_IDLE        1
#define configUSE_APPLICATION_TASK_TAG 0
#define configUSE_NEWLIB_REENTRANT     0
#define configUSE_CO_ROUTINES          0
#define configUSE_COUNTING_SEMAPHORES  1
#define configUSE_RECURSIVE_MUTEXES    1
#define configUSE_QUEUE_SETS           0
#define configUSE_TASK_NOTIFICATIONS   1
#define configUSE_TRACE_FACILITY       1

/* Hooks */
#define configUSE_IDLE_HOOK          0
#define configUSE_TICK_HOOK          0
#define configUSE_MALLOC_FAILED_HOOK 0

/* Debug features */
#define configCHECK_FOR_STACK_OVERFLOW 0
#define configASSERT(x)           \
    if ((x) == 0) {               \
        taskDISABLE_INTERRUPTS(); \
        for (;;)                  \
            ;                     \
    }
#define configQUEUE_REGISTRY_SIZE 0

/* Timers and queues */
#define configUSE_TIMERS             1
#define configTIMER_TASK_PRIORITY    3
#define configTIMER_TASK_STACK_DEPTH configMINIMAL_STACK_SIZE
#define configTIMER_QUEUE_LENGTH     5

/* Task settings */
#define INCLUDE_vTaskPrioritySet             1
#define INCLUDE_uxTaskPriorityGet            1
#define INCLUDE_vTaskDelete                  1
#define INCLUDE_vTaskCleanUpResources        0
#define INCLUDE_vTaskSuspend                 1
#define INCLUDE_vTaskDelayUntil              1
#define INCLUDE_vTaskDelay                   1
#define INCLUDE_uxTaskGetStackHighWaterMark  0
#define INCLUDE_xTaskGetIdleTaskHandle       0
#define INCLUDE_eTaskGetState                1
#define INCLUDE_xTaskResumeFromISR           0
#define INCLUDE_xTaskGetCurrentTaskHandle    1
#define INCLUDE_xTaskGetSchedulerState       0
#define INCLUDE_xSemaphoreGetMutexHolder     0
#define INCLUDE_xTimerPendFunctionCall       1
#define configUSE_STATS_FORMATTING_FUNCTIONS 1
#define configCOMMAND_INT_MAX_OUTPUT_SIZE    2048
#ifdef __NVIC_PRIO_BITS
#define configPRIO_BITS __NVIC_PRIO_BITS
#else
#define configPRIO_BITS 4
#endif

/* Interrupt settings */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY      0x07
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY              (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY         (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#ifndef __IASMARM__
#define configGENERATE_RUN_TIME_STATS 0
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE() 0
#define configTICK_RATE_HZ               (TickType_t)10000
#endif /* __IASMARM__ */

#if defined(CPU_CORTEX_M3) || defined(CPU_CORTEX_M4) || defined(CPU_CORTEX_M7)
#define xPortPendSVHandler  PendSV_Handler
#define vPortSVCHandler     SVC_Handler
#define xPortSysTickHandler SysTick_Handler
#endif

#endif /* FREERTOS_CONFIG_H */
