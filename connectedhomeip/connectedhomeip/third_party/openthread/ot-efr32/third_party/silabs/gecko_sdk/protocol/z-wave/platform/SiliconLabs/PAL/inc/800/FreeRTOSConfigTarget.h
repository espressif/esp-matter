/* --------------------------------------------------------------------------
 * Copyright (c) 2013-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * --------------------------------------------------------------------------
 *
 * $Revision:   V10.2.0
 *
 * Project:     CMSIS-FreeRTOS
 * Title:       FreeRTOS configuration definitions
 *
 * --------------------------------------------------------------------------*/

#ifndef FREERTOS_CONFIG_800_H
#define FREERTOS_CONFIG_800_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#if !defined(__IAR_SYSTEMS_ASM__)
#if (defined(__ARMCC_VERSION) || defined(__GNUC__) || defined(__ICCARM__))
#include <stdint.h>

#endif

#include "em_assert.h"
extern uint32_t SystemCoreClock;

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#endif

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

//  <o>Minimal stack size [words] <0-65535>
//  <i> Stack for idle task and default task stack in words.
//  <i> Default: 140
#define configMINIMAL_STACK_SIZE                TASK_STACK_SIZE_MINIMUM

//  <o>Total heap size [bytes] <0-0xFFFFFFFF>
//  <i> Heap memory size in bytes.
//  <i> Default: 8192
//  Note: This value can be modified based on the requirement
//  of a given platform component
//  Some platform component also expect a calloc.
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 1024 * 8 ) )

//  <o>Timer task stack depth [words] <0-65535>
//  <i> Stack for timer task in words.
//  <i> Default: 140
#define configTIMER_TASK_STACK_DEPTH            TASK_STACK_SIZE_MINIMUM

//  <o>Timer queue length <0-1024>
//  <i> Timer command queue length.
//  <i> Default: 10
#define configTIMER_QUEUE_LENGTH                8

#ifdef __NVIC_PRIO_BITS
    /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
    #define configPRIO_BITS            __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS            3    /* 7 priority levels */
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         0x07
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    0x01
#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

//  <o>Preemption interrupt priority
//  <i> Maximum priority of interrupts that are safe to call FreeRTOS API.
//  <i> Default: 48
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

//  <q>Use time slicing
//  <i> Enable setting to use timeslicing.
//  <i> Default: 1
#define configUSE_TIME_SLICING                  1

//  <q>Idle should yield
//  <i> Control Yield behaviour of the idle task.
//  <i> Default: 1
#define configIDLE_SHOULD_YIELD                 1

//  <o>Check for stack overflow
//    <0=>Disable <1=>Method one <2=>Method two
//  <i> Enable or disable stack overflow checking.
//  <i> Callback function vApplicationStackOverflowHook implementation is required when stack checking is enabled.
//  <i> Default: 2
#define configCHECK_FOR_STACK_OVERFLOW          2

//  <q>Use idle hook
//  <i> Enable callback function call on each idle task iteration.
//  <i> Callback function vApplicationIdleHook implementation is required when idle hook is enabled.
//  <i> Default: 0
#define configUSE_IDLE_HOOK                     1

//  <q>Use tick hook
//  <i> Enable callback function call during each tick interrupt.
//  <i> Callback function vApplicationTickHook implementation is required when tick hook is enabled.
//  <i> Default: 0
#define configUSE_TICK_HOOK                     0

//  <q>Use deamon task startup hook
//  <i> Enable callback function call when timer service starts.
//  <i> Callback function vApplicationDaemonTaskStartupHook implementation is required when deamon task startup hook is enabled.
//  <i> Default: 0
#define configUSE_DAEMON_TASK_STARTUP_HOOK      0

//  <q>Use malloc failed hook
//  <i> Enable callback function call when out of dynamic memory.
//  <i> Callback function vApplicationMallocFailedHook implementation is required when malloc failed hook is enabled.
//  <i> Default: 0
#define configUSE_MALLOC_FAILED_HOOK            0

//  <o>Queue registry size
//  <i> Define maximum number of queue objects registered for debug purposes.
//  <i> The queue registry is used by kernel aware debuggers to locate queue and semaphore structures and display associated text names.
//  <i> Default: 10
#define configQUEUE_REGISTRY_SIZE               10

// <h>Event Recorder configuration
//  <i> Initialize and setup Event Recorder level filtering.
//  <i> Settings have no effect when Event Recorder is not present.

//  <q>Initialize Event Recorder
//  <i> Initialize Event Recorder before FreeRTOS kernel start.
//  <i> Default: 1
#define configEVR_INITIALIZE                    1

//  <e>Setup recording level filter
//  <i> Enable configuration of FreeRTOS events recording level
//  <i> Default: 1
#define configEVR_SETUP_LEVEL                   1

//  <o>Tasks functions
//  <i> Define event recording level bitmask for events generated from Tasks functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_TASKS                   0x05

//  <o>Queue functions
//  <i> Define event recording level bitmask for events generated from Queue functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_QUEUE                   0x05

//  <o>Timer functions
//  <i> Define event recording level bitmask for events generated from Timer functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_TIMERS                  0x05

//  <o>Event Groups functions
//  <i> Define event recording level bitmask for events generated from Event Groups functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_EVENTGROUPS             0x05

//  <o>Heap functions
//  <i> Define event recording level bitmask for events generated from Heap functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_HEAP                    0x05

//  <o>Stream Buffer functions
//  <i> Define event recording level bitmask for events generated from Stream Buffer functions.
//  <i> Default: 0x05
//    <0x00=>Off <0x01=>Errors <0x05=>Errors + Operation <0x0F=>All
#define configEVR_LEVEL_STREAMBUFFER            0x05
//  </e>
// </h>

// <h> Port Specific Features
// <i> Enable and configure port specific features.
// <i> Check FreeRTOS documentation for definitions that apply for the used port.

//  <q>Use Floating Point Unit
//  <i> Using Floating Point Unit (FPU) affects context handling.
//  <i> Enable FPU when application uses floating point operations.
//  <i> Default: 1
#define configENABLE_FPU                        1

//  <q>Use Memory Protection Unit
//  <i> Using Memory Protection Unit (MPU) requires detailed memory map definition.
//  <i> This setting is only releavant for MPU enabled ports.
//  <i> Default: 0
#define configENABLE_MPU                        0

//  <o>Minimal secure stack size [words] <0-65535>
//  <i> Stack for idle task Secure side context in words.
//  <i> This setting is only relevant when TrustZone extension is enabled.
//  <i> Default: 128
#define configMINIMAL_SECURE_STACK_SIZE       ((uint32_t)128)
// </h>

// <h> Thread Local Storage Settings
//  <o>Thread local storage pointers
//  <i> Thread local storage (or TLS) allows the application writer to store
//  <i> values inside a task's control block, making the value specific to
//  <i> (local to) the task itself.
//  <i> Default: 0
#define configNUM_USER_THREAD_LOCAL_STORAGE_POINTERS 0
// </h>

//------------- <<< end of configuration section >>> ---------------------------

/* Defines needed by FreeRTOS to implement CMSIS RTOS2 API. Do not change! */
#define configCPU_CLOCK_HZ                      (SystemCoreClock)
#define configUSE_16_BIT_TICKS                  0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0

/* Defines that include FreeRTOS functions which implement CMSIS RTOS2 API. Do not change! */
#define INCLUDE_xEventGroupSetBitsFromISR       1
#define INCLUDE_xSemaphoreGetMutexHolder        1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_vTaskSuspend                    1

/* Map the FreeRTOS port interrupt handlers to their CMSIS standard names. */
#define xPortPendSVHandler                      PendSV_Handler
#define vPortSVCHandler                         SVC_Handler

/* Ensure Cortex-M port compatibility. */
#define SysTick_Handler                         xPortSysTickHandler

/* Implement FreeRTOS configASSERT as emlib assert. */
#define configASSERT(x)                               EFM_ASSERT(x)

/* Energy saving modes. */
#define configUSE_TICKLESS_IDLE                       1

/* Definition used by Keil to replace default system clock source. */
#define configOVERRIDE_DEFAULT_TICK_CONFIGURATION     1

/* Maximum size of task name. */
#define configMAX_TASK_NAME_LEN                       10

/* Use queue sets? */
#define configUSE_QUEUE_SETS                          0

/* Generate run-time statistics? */
#define configGENERATE_RUN_TIME_STATS                 0

/* Optional resume from ISR functionality. */
#define INCLUDE_xResumeFromISR                        1

/* FreeRTOS Secure Side Only and TrustZone Security Extension */
#define configRUN_FREERTOS_SECURE_ONLY                1
#define configENABLE_TRUSTZONE                        0

/* Thread local storage pointers used by the SDK */
#ifndef configNUM_SDK_THREAD_LOCAL_STORAGE_POINTERS
  #define configNUM_SDK_THREAD_LOCAL_STORAGE_POINTERS 0
#endif

#define configNUM_THREAD_LOCAL_STORAGE_POINTERS (configNUM_USER_THREAD_LOCAL_STORAGE_POINTERS \
                                                 + configNUM_SDK_THREAD_LOCAL_STORAGE_POINTERS)

#if defined(SL_CATALOG_SYSTEMVIEW_TRACE_PRESENT)
#include "SEGGER_SYSVIEW_FreeRTOS.h"
#endif
#endif /* FREERTOS_CONFIG_800_H */
