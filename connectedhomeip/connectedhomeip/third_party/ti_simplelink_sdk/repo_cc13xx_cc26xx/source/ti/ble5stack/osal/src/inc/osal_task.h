/******************************************************************************

 @file  osal_task.h

 @brief This file contains the OSAL Task create/manipulate definitions.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2010-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef OSAL_TASK_H
#define OSAL_TASK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "StackMacros.h"
#include "hal_types.h"

#define OSAL_SUCCESS             1
#define OSAL_ERROR               0

#define OSAL_PREEMPTION          configUSE_PREEMPTION
#define OSAL_MAX_TASK_NAME_LEN   configMAX_TASK_NAME_LEN
#define OSAL_MAX_PRIORITY        configMAX_PRIORITIES
#define OSAL_MIN_STACK_SIZE      configMINIMAL_STACK_SIZE

#define OSAL_TASK_YIELD          taskYIELD
#define OSAL_TASK_ENTER_CRITICAL taskENTER_CRITICAL
#define OSAL_TASK_EXIT_CRITICAL  taskENTER_CRITICAL
#define OSAL_DISABLE_INTERRUPTS  taskDISABLE_INTERRUPTS
#define OSAL_ENABLE_INTERRUPTS   taskENABLE_INTERRUPTS

#define OSAL_IDLE_TASK_PRIORITY   (unsigned long) 0
#define OSAL_TASK_PRIORITY_ONE    (unsigned long) 1
#define OSAL_TASK_PRIORITY_TWO    (unsigned long) 2
#define OSAL_TASK_PRIORITY_THREE  (unsigned long) 3
#define OSAL_TASK_PRIORITY_FOUR   (unsigned long) 4
#define OSAL_TASK_PRIORITY_HIGH   (unsigned long) 5


/*---------------------------------------------------------------------------------------
   Name: osal_task_create

   Purpose: Creates a task
   Parameters:
       task_func    Pointer to the task entry function. Tasks must be implemented to never
                    return (i.e. continuous loop).
       task_name    A descriptive name for the task. This is mainly used to facilitate
                    debugging. Max length defined by MAX_TASK_NAME_LEN.
       stack_depth  The size of the task stack specified as the number of variables the stack
                    can hold - not the number of bytes. For example, if the stack is 16 bits
                    wide and stack_depth is defined as 100, 200 bytes will be allocated for
                    stack storage. The stack depth multiplied by the stack width must not
                    exceed the maximum value that can be contained in a variable of type size_t.
       task_func_parameters     Pointer that will be used as the parameter for the task being
                                created.
       task_priority The priority at which the task should run.
       task_handle   Used to pass back a handle by which the created task can be referenced.
    returns: OSAL_SUCCESS  if the task was successfully created and added to a ready list
             OSAL_ERROR

    NOTES: handle is passed back to the user by which the created task can be referenced.
---------------------------------------------------------------------------------------*/
long osal_task_create( void (*task_func)( void * ), const int8 *task_name, uint16 stack_depth,
                       void *task_func_parameters, uint32 task_priority, void **task_handle );

/*---------------------------------------------------------------------------------------
   Name: osal_task_delete

   Purpose: Deletes a task
   Parameters:
      task_handle The handle of the task to be deleted. Passing NULL will cause the calling
                  task to be deleted
   returns:
      void
---------------------------------------------------------------------------------------*/
void osal_task_delete( void **task_handle );

/*---------------------------------------------------------------------------------------
   Name: osal_task_suspend

   Purpose: suspends a task. Passing a NULL handle will cause the calling task to
            be suspended.
   Parameters:
      task_handle Handle to the task being suspended. Passing a NULL handle will cause
                  the calling task to be suspended.
   returns:
      void
---------------------------------------------------------------------------------------*/
void osal_task_suspend( void **task_handle );

/*---------------------------------------------------------------------------------------
   Name: osal_task_resume

   Purpose: resumes a suspended task
   Parameters:
      task_handle Handle to the task being readied.
   returns: OSAL_ERROR if error
            OSAL_SUCCESS if success
---------------------------------------------------------------------------------------*/
void osal_task_resume( void **task_handle );

/*---------------------------------------------------------------------------------------
   Name: osal_task_priority_get

   Purpose: gets the priority of the task.
   Parameters:
      task_handle Handle to the task for which the priority is being set. Passing a NULL
                  handle results in the priority of the calling task being returned.
   returns: The priority of task
---------------------------------------------------------------------------------------*/
unsigned long osal_task_priority_get( void **task_handle );

/*---------------------------------------------------------------------------------------
   Name: osal_task_priority_set

   Purpose: sets the priority of the task.
   Parameters:
      task_handle    Handle to the task for which the priority is being set. Passing a NULL
                     handle results in the priority of the calling task being set.
      task_priority  The priority to which the task will be set.
   returns: The priority of task
---------------------------------------------------------------------------------------*/
void osal_task_priority_set( void **task_handle, uint32 task_priority );

/*---------------------------------------------------------------------------------------
   Name: osal_task_delay

   Purpose: Delay a task for a given number of ticks. The actual time that the task remains
            blocked depends on the tick rate. The constant TICK_RATE_MS can be used to
            calculate real time from the tick rate - with the resolution of one tick period.
   Parameters:
      ticks_to_delay  The amount of time, in tick periods, that the calling task should
                      block
   Returns
      void
---------------------------------------------------------------------------------------*/
void osal_task_delay( uint32 ticks_to_delay );

/*---------------------------------------------------------------------------------------
   Name: osal_task_delay_until

   Purpose: Delay a task until a specified time. This function can be used by cyclical
            tasks to ensure a constant execution frequency.
   Parameters:
      prev_wake_time Pointer to a variable that holds the time at which the task was
                     last unblocked. The variable must be initialized with the current
                     time prior to its first use.

      time_increment The cycle time period. The task will be unblocked at time
                     (prev_wake_time+time_increment). Calling osal_task_delay_until with
                     the same time_increment parameter value will cause the task to execute
                     with a fixed interval period
    Returns
       void
---------------------------------------------------------------------------------------*/
void osal_task_delay_until( uint32 * const prev_wake_time, uint32 time_increment );

/*---------------------------------------------------------------------------------------
   Name: osal_task_start_scheduler

   Purpose: Starts the real time kernel tick processing. After calling the kernel has control
            over which tasks are executed and when.
   Parameters:
      void
   Returns
      void
---------------------------------------------------------------------------------------*/
void osal_task_start_scheduler( void );

/*---------------------------------------------------------------------------------------
   Name: osal_task_end_scheduler

   Purpose: Stops the real time kernel tick. All created tasks will be automatically
            deleted and multitasking (either preemptive or cooperative) will stop.
            Execution then resumes from the point where vTaskStartScheduler() was called,
            as if vTaskStartScheduler() had just returned
   Parameters:
      void
   Returns
      void
---------------------------------------------------------------------------------------*/
void osal_task_end_scheduler( void );

/*---------------------------------------------------------------------------------------
   Name: osal_task_suspend_all

   Purpose: Suspends all real time kernel activity while keeping interrupts (including
            the kernel tick) enabled.
   Parameters:
      void
   Returns
      void
---------------------------------------------------------------------------------------*/
void osal_task_suspend_all( void );

/*---------------------------------------------------------------------------------------
   Name: osal_task_resume_all

   Purpose: Resumes real time kernel activity following a call to osal_task_suspend_all().
            After a call to osal_task_suspend_all() the kernel will take control of which
            task is executing at any time.
   Parameters:
      void
   Returns
      True or False. True if context switch happens, else false
---------------------------------------------------------------------------------------*/
long osal_task_resume_all( void );

/*---------------------------------------------------------------------------------------
   Name: osal_task_yield

   Purpose: Forces a context switch.
   Parameters:
      void
   Returns
      void
---------------------------------------------------------------------------------------*/
void osal_task_yield( void );

/*---------------------------------------------------------------------------------------
   Name: osal_queue_create

   Purpose: Creates a queue, used to pass items between tasks.
   Parameters:
      max_items   Maximum number of items that the queue can contain.
      item_size   Size, in bytes, of each item in the queue.
   Returns
      void*       Ptr to queue handle
---------------------------------------------------------------------------------------*/
void *osal_queue_create( uint32 max_items, uint32 item_size );

/*---------------------------------------------------------------------------------------
   Name: osal_queue_receive

   Purpose: Creates a queue, used to pass items between tasks.
   Parameters:
      handle      Pointer to handle of the queue from which data is received.
      buffer      Pointer to memory into which received data will be copied.
      wait_ticks  Maximum time to block task waiting for data to be available.
   Returns
      status      OSAL_SUCCESS is read successful, OSAL_ERROR if data not read
---------------------------------------------------------------------------------------*/
uint32 osal_queue_receive( void *handle, void *buffer, uint32 wait_ticks );

/*---------------------------------------------------------------------------------------
   Name: osal_queue_send

   Purpose: Sends (writes) an item to a queue.
   Parameters:
      handle      Pointer to handle of the queue to which data is written.
      buffer      Pointer to memory from which data will be copied.
      wait_ticks  Maximum time to block task waiting for space to be available.
   Returns
      status      OSAL_SUCCESS is send successful, OSAL_ERROR if data not written
---------------------------------------------------------------------------------------*/
uint32 osal_queue_send( void *handle, void *buffer, uint32 wait_ticks );

#ifdef __cplusplus
}
#endif

#endif


