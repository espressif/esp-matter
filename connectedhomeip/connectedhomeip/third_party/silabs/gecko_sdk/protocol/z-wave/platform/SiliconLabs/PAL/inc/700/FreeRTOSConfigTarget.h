/*
    FreeRTOS V8.2.0 (good for V10.3.1) - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_700_H
#define FREERTOS_CONFIG_700_H

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

#define configUSE_IDLE_HOOK             1
#define configUSE_TICK_HOOK             0
#define configMINIMAL_STACK_SIZE        TASK_STACK_SIZE_MINIMUM
#define configTOTAL_HEAP_SIZE           ( ( size_t ) ( 1024 * 8 ) )
#define configMAX_TASK_NAME_LEN         ( 10 )  // This even includes the NULL terminator '\0'
#define configUSE_TRACE_FACILITY        1
#define configUSE_16_BIT_TICKS          0
#define configIDLE_SHOULD_YIELD         1
#ifdef NDEBUG
#define configCHECK_FOR_STACK_OVERFLOW  ( 0 )  // Disable stack overflow check for releas builds.
#else
#define configCHECK_FOR_STACK_OVERFLOW  ( 2 )  // !!! This will consume power on final products if left 'on' after release.
#endif
#define configQUEUE_REGISTRY_SIZE       0
#define configGENERATE_RUN_TIME_STATS   0
#define configUSE_MALLOC_FAILED_HOOK    0  // No need. No dynamic memory allocation used!


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             1  // Useful for Real-Time applications, like data acquisition.
#define INCLUDE_xTaskGetSchedulerState      1
#define INCLUDE_uxTaskGetStackHighWaterMark 1  // Recommended due to configCHECK_FOR_STACK_OVERFLOW = 2!
#define INCLUDE_eTaskGetState               1


/* Config the use of tickless idle task. When set to 1 the ARM processor will
 * be put into sleep mode when RTOS is idle */
#define configUSE_TICKLESS_IDLE                 1
/* This demo makes use of one or more example stats formatting functions.  These
format the raw data provided by the uxTaskGetSystemState() function in to human
readable ASCII form. See the notes in the implementation of vTaskList() within
FreeRTOS/Source/tasks.c for limitations. */
#define configUSE_STATS_FORMATTING_FUNCTIONS    0


/* This is the value being used as per the ST library which permits 16
priority values, 0 to 15.  This must correspond to the
configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
NVIC value of 255. */
//#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY   15



/*-----------------------------------------------------------
 * Macros required to setup the timer for the run time stats.
 *-----------------------------------------------------------*/
/* The run time stats time base just uses the existing high frequency timer
test clock.  All these macros do is clear and return the high frequency
interrupt count respectively. */
//extern unsigned long ulRunTimeStatsClock;
//#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()   ulRunTimeStatsClock = 0
//#define portGET_RUN_TIME_COUNTER_VALUE()           ulRunTimeStatsClock


/*-----------------------------------------------------------
 * Macros required to setup the software timer API.
 *-----------------------------------------------------------*/
#ifdef EFR32ZG14P
#define configTIMER_QUEUE_LENGTH      4 /* EFR32ZG14 has low requirement for queue length since timer events are added from task context only */
#else
#define configTIMER_QUEUE_LENGTH      8 /* End devices require a bigger queue because timer events are added from both task and intr context */
#endif
#define configTIMER_TASK_STACK_DEPTH  TASK_STACK_SIZE_MINIMUM

/* Specific information on low power for ARM Cortex-M:
   https://www.freertos.org/low-power-ARM-cortex-rtos.html */

/* Portions of this configuration file is inspired by:
   FreeRTOS/Demo/CORTEX_EFM32_Giant_Gecko_Simplicity_Studio/FreeRTOSConfig.h */
extern uint32_t SystemCoreClock;

#define configUSE_PORT_OPTIMISED_TASK_SELECTION    (0)
#define configCPU_CLOCK_HZ                         (SystemCoreClock)


/* Interrupt nesting behaviour configuration */
/* Cortex-M specific definitions. */


#ifdef __NVIC_PRIO_BITS
    /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
    #define configPRIO_BITS            __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS            3    /* 7 priority levels */
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         0x07
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    0x01

#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

#define configNUM_USER_THREAD_LOCAL_STORAGE_POINTERS 0

/* Thread local storage pointers used by the SDK */
#ifndef configNUM_SDK_THREAD_LOCAL_STORAGE_POINTERS
  #define configNUM_SDK_THREAD_LOCAL_STORAGE_POINTERS 0
#endif

#define configNUM_THREAD_LOCAL_STORAGE_POINTERS (configNUM_USER_THREAD_LOCAL_STORAGE_POINTERS \
                                                 + configNUM_SDK_THREAD_LOCAL_STORAGE_POINTERS)

#define vPortSVCHandler         SVC_Handler
#define xPortPendSVHandler      PendSV_Handler
#define xPortSysTickHandler     SysTick_Handler

/* For the linker. */
#define fabs __builtin_fabs

/*-----------------------------------------------------------
* Defining configASSERT
*-----------------------------------------------------------*/
#ifndef NDEBUG
#include <Assert.h>
#define configASSERT(BMUSTBETRUE) (ASSERT(BMUSTBETRUE))
#endif

#endif /* FREERTOS_CONFIG_700_H */
