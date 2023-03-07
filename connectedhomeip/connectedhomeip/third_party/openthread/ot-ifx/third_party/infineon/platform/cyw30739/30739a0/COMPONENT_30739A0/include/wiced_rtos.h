/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 *  Defines functions to access functions provided by the RTOS
 *  in a generic way
 *
 */

#pragma once


/* Include the actual RTOS definitions for:
 * - wiced_timed_event_t
 * - timed_event_handler_t
 */
#include "wiced_result.h"
#include "wiced_bt_dev.h"

#ifdef __cplusplus
extern "C" {
#endif


#define WICED_NO_WAIT       0
#define WICED_WAIT_FOREVER  ((uint32_t) 0xFFFFFFFF)
#define WICED_NEVER_TIMEOUT ((uint32_t) 0xFFFFFFFF)


/******************************************************
 * @cond            Enumerations
 ******************************************************/

/** Wiced event flag wait option */
typedef enum
{
    WAIT_FOR_ANY_EVENT,
    WAIT_FOR_ALL_EVENTS,
} wiced_event_flags_wait_option_t;

/** Wiced delay types */
typedef enum
{
    ALLOW_THREAD_TO_SLEEP, /**<  Allow the current running thread to sleep */
    KEEP_THREAD_ACTIVE,    /**<  Keep the current running thread active */
}wiced_delay_type_t;

/******************************************************
 *                 Type Definitions
 ******************************************************/


/******************************************************
 *             Structures
 ******************************************************/

typedef struct _wiced_thread_t        wiced_thread_t        ;
typedef struct _thread_monitor_info_t thread_monitor_info_t ;
typedef struct _wiced_queue_t         wiced_queue_t         ;
typedef struct _wiced_rtos_timer_t    wiced_rtos_timer_t    ;
typedef struct _wiced_semaphore_t     wiced_semaphore_t     ;
typedef struct _wiced_mutex_t         wiced_mutex_t         ;
typedef struct _wiced_event_flags_t   wiced_event_flags_t   ;
typedef struct _wiced_worker_thread_t wiced_worker_thread_t ;
typedef struct _wiced_timed_event_t   wiced_timed_event_t   ;

typedef wiced_result_t (*event_handler_t)( void* arg );

typedef void (*wiced_thread_function_t)( uint32_t arg );

/******************************************************
 *             Function declarations
 * @endcond
 ******************************************************/

extern wiced_worker_thread_t wiced_hardware_io_worker_thread;
extern wiced_worker_thread_t wiced_networking_worker_thread;

/*****************************************************************************/
/** @defgroup rtos       RTOS
 *
 *  AIROC Real-Time Operating System Functions
 */
/*****************************************************************************/


/*****************************************************************************/
/** @addtogroup threads       Threads
 *  @ingroup rtos
 *
 * Thread management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Creates a new instance of thread
 *
 * Creates a new thread (Allocates memory for a new thread instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_thread_t*  wiced_rtos_create_thread( void );

/** Initializes and starts a new thread
 *
 * Initializes and starts a new thread
 *
 * @param thread     : Pointer to variable that will receive the thread handle
 * @param priority   : A priority number or WICED_DEFAULT_APP_THREAD_PRIORITY.
 * @param name       : a text name for the thread (can be null)
 * @param function   : the main thread function
 * @param stack_size : stack size for this thread
 * @param arg        : argument which will be passed to thread function
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_thread( wiced_thread_t* thread, uint8_t priority, const char* name, wiced_thread_function_t function, uint32_t stack_size, void* arg );

#ifdef WICED_RTOS_D
/** Deletes a terminated thread
 *
 * @param thread     : the handle of the thread to delete
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delete_thread( wiced_thread_t* thread );

#endif // WICED_RTOS_D

/** Checks if there is a stackoverflow
 *
 * Verifies if the stack of the thread from which this API is invoked is corrupted
 *
 * @return    TRUE  : stack overflow
 * @return    FALSE : not stack overflow
 */
wiced_bool_t wiced_rtos_check_for_stack_overflow( void );

/** Return the thread stack size
 *
 * Returns stack size if thread signature is valid, else returns 0
 *
 * @return    uint32_t  : size
 * @return    0         : invalid thread pointer
 */
uint32_t wiced_rtos_thread_stack_size( wiced_thread_t* thread );

/** Returns maximum stack usage of the thread
 *
 * Returns maximum stack usage size if thread signature is valid, else returns 0
 *
 * @param[in]       thread   : point to the thread context
 *                             Note: thread is a return value from wiced_rtos_create_thread()
 *
 * @return          maximum stack usage
 *******************************************************************/
uint32_t wiced_bt_rtos_max_stack_use( wiced_thread_t* thread );

/** Sleep for a given period of milliseconds
 *
 * Causes the current thread to sleep for AT LEAST the
 * specified number of milliseconds. If the processor is heavily loaded
 * with higher priority tasks, the delay may be much longer than requested.
 *
 * @param milliseconds : the time to sleep in milliseconds
 * @param delay_type : delay type to be used during the duration of the delay.
 *                     Use the delay type as ALLOW_THREAD_TO_SLEEP to allow the
 *                     current running thread to sleep or as KEEP_THREAD_ACTIVE
 *                     to keep the current running thread active
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delay_milliseconds( uint32_t milliseconds, wiced_delay_type_t delay_type);

/** Delay for a given period of microseconds
 *
 * Causes the current thread to block for AT LEAST the
 * specified number of microseconds. If the processor is heavily loaded
 * with higher priority tasks, the delay may be much longer than requested.
 *
 * NOTE: All threads with equal or lower priority than the current thread
 *       will not be able to run while the delay is occurring.
 *
 * @param microseconds : the time to delay in microseconds
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_delay_microseconds( uint32_t microseconds );

/** Sleeps until another thread has terminated
 *
 * Causes the current thread to sleep until the specified other thread
 * has terminated. If the processor is heavily loaded
 * with higher priority tasks, this thread may not wake until significantly
 * after the thread termination.
 *
 * @param thread : the handle of the other thread which will terminate
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_thread_join( wiced_thread_t* thread );


/** Forcibly wakes another thread
 *
 * Causes the specified thread to wake from suspension. This will usually
 * cause an error or timeout in that thread, since the task it was waiting on
 * is not complete.
 *
 * @param thread : the handle of the other thread which will be woken
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_thread_force_awake( wiced_thread_t* thread );


/** Checks if a thread is the current thread
 *
 * Checks if a specified thread is the currently running thread
 *
 * @param thread : the handle of the other thread against which the current thread will be compared
 *
 * @return    WICED_SUCCESS : specified thread is the current thread
 * @return    WICED_ERROR   : specified thread is not currently running
 */
wiced_result_t wiced_rtos_is_current_thread( wiced_thread_t* thread );


/** @} */
/*****************************************************************************/
/** @addtogroup worker       Worker Threads
 *  @ingroup rtos
 *
 * Worker thread management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Creates a new instance of a worker thread
 *
 * Creates a new worker thread (Allocates memory for a new worker thread instance and
 * returns the pointer)
 * A worker thread is a thread in whose context timed and asynchronous events
 * execute.
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_worker_thread_t* wiced_rtos_create_worker_thread(void);


/** Initializes and starts a worker thread
 *
 * Initializes and starts a worker thread
 * A worker thread is a thread in whose context timed and asynchronous events
 * execute.
 *
 * @param worker_thread    : a pointer to the worker thread to be created
 * @param priority         : thread priority
 * @param stack_size       : thread's stack size in number of bytes
 * @param event_queue_size : number of events can be pushed into the queue
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 * Note :     The number of buffers in wiced_bt_cfg_settings_t.max_number_of_buffer_pools
 *            must be increased for each worker thread the application will utilize.
 */
wiced_result_t wiced_rtos_init_worker_thread( wiced_worker_thread_t* worker_thread, uint8_t priority, uint32_t stack_size, uint32_t event_queue_size );


/** @} */
/*****************************************************************************/
/** @addtogroup semaphores       Semaphores
 *  @ingroup rtos
 *
 * Semaphore management functionss
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Creates a new instance of semaphore
 *
 * Creates a new semaphore (Allocates memory for a new semaphore instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_semaphore_t*  wiced_rtos_create_semaphore( void );


/** Initialises a semaphore
 *
 * Initialises a counting semaphore
 *
 * @param semaphore : a pointer to the semaphore handle to be initialised
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_semaphore( wiced_semaphore_t* semaphore );


/** Set (post/put/increment) a semaphore
 *
 * Set (post/put/increment) a semaphore
 *
 * @param semaphore : a pointer to the semaphore handle to be set
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_set_semaphore( wiced_semaphore_t* semaphore );


/** Get (wait/decrement) a semaphore
 *
 * Attempts to get (wait/decrement) a semaphore. If semaphore is at zero already,
 * then the calling thread will be suspended until another thread sets the
 * semaphore with @ref wiced_rtos_set_semaphore
 *
 * @param semaphore : a pointer to the semaphore handle
 * @param timeout_ms: the number of milliseconds to wait before returning
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_get_semaphore( wiced_semaphore_t* semaphore, uint32_t timeout_ms );


/** @} */
/*****************************************************************************/
/** @addtogroup mutexes       Mutexes
 *  @ingroup rtos
 *
 * Mutex management functionss
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Creates a new instance of mutex
 *
 * Creates a new mutex (Allocates memory for a new mutex instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_mutex_t*  wiced_rtos_create_mutex( void );


/** Initialises a mutex
 *
 * Initialises a mutex
 * A mutex is different to a semaphore in that a thread that already holds
 * the lock on the mutex can request the lock again (nested) without causing
 * it to be suspended.
 *
 * @param mutex : a pointer to the mutex handle to be initialised
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_mutex( wiced_mutex_t* mutex );


/** Obtains the lock on a mutex
 *
 * Attempts to obtain the lock on a mutex. If the lock is already held
 * by another thead, the calling thread will be suspended until
 * the mutex lock is released by the other thread.
 *
 * @param mutex : a pointer to the mutex handle to be locked
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_lock_mutex( wiced_mutex_t* mutex );


/** Releases the lock on a mutex
 *
 * Releases a currently held lock on a mutex. If another thread
 * is waiting on the mutex lock, then it will be resumed.
 *
 * @param mutex : a pointer to the mutex handle to be unlocked
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_unlock_mutex( wiced_mutex_t* mutex );


#ifdef WICED_RTOS_D
/** De-initialise a mutex
 *
 * Deletes a mutex created with @ref wiced_rtos_init_mutex
 *
 * @param mutex : a pointer to the mutex handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_mutex( wiced_mutex_t* mutex );

#endif // WICED_RTOS_D

/** @} */
/*****************************************************************************/
/** @addtogroup queues       Queues
 *  @ingroup rtos
 *
 * Queue management functionss
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Creates a new instance of queue
 *
 * Creates a new FIFO queue (Allocates memory for a new queue instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_queue_t*  wiced_rtos_create_queue( void );


/** Initialises a queue
 *
 * Initialises a FIFO queue
 *
 * @param queue : a pointer to the queue handle to be initialised
 * @param name  : a text string name for the queue (NULL is allowed)
 * @param message_size : size in bytes of objects that will be held in the queue
 *                       (currently only 1 byte is acccepted)
 * @param number_of_messages : depth of the queue - i.e. max number of objects
 *                             in the queue
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 *
 * Note : This API will create a separate private memory buffer pool so please
 * increase max_number_of_buffer_pools (in wiced_bt_cfg_settings_t structure)
 * by the number of rtos queues initialized - i.e. one per call to this API.
 */
wiced_result_t wiced_rtos_init_queue( wiced_queue_t* queue, const char* name, uint32_t message_size, uint32_t number_of_messages );


/** Pushes an object onto a queue
 *
 * Pushes an object onto a queue
 *
 * @param queue : a pointer to the queue handle
 * @param message : the object to be added to the queue. Size is assumed to be
 *                  the size specified in @ref wiced_rtos_init_queue
 * @param timeout_ms: [DEPRECATED] the number of milliseconds to wait before returning
 *                    Note: timeout option is not supported and the API returns
 *                    immediately without waiting even when timeout_ms is non zero.
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred.
 */
wiced_result_t wiced_rtos_push_to_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms );


/** Pops an object off a queue
 *
 * Pops an object off a queue
 *
 * @param queue : a pointer to the queue handle
 * @param message : pointer to a buffer that will receive the object being
 *                  popped off the queue. Size is assumed to be
 *                  the size specified in @ref wiced_rtos_init_queue , hence
 *                  you must ensure the buffer is long enough or memory
 *                  corruption will result
 * @param timeout_ms: [DEPRECATED] the number of milliseconds to wait before returning
 *                    Note: timeout option is not supported and irrespective of timeout_ms value,
 *                    API would be in blocking state until it receives single message in a queue.
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred.
 */
wiced_result_t wiced_rtos_pop_from_queue( wiced_queue_t* queue, void* message, uint32_t timeout_ms );


#ifdef WICED_RTOS_D
/** De-initialise a queue
 *
 * Deletes a queue created with @ref wiced_rtos_init_queue
 *
 * @param queue : a pointer to the queue handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_deinit_queue( wiced_queue_t* queue );

#endif // WICED_RTOS_D

/** Check if a queue is empty
 *
 * @param queue : a pointer to the queue handle
 *
 * @return    WICED_SUCCESS : queue is empty.
 * @return    WICED_ERROR   : queue is not empty.
 */
wiced_bool_t wiced_rtos_is_queue_empty( wiced_queue_t* queue );


/** Check if a queue is full
 *
 * @param queue : a pointer to the queue handle
 *
 * @return    WICED_SUCCESS : queue is full.
 * @return    WICED_ERROR   : queue is not full.
 */
wiced_bool_t wiced_rtos_is_queue_full( wiced_queue_t* queue );


/** Get the queue occupancy
 *
 * @param queue : a pointer to the queue handle
 * @param count : pointer to integer for storing occupancy count
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_get_queue_occupancy( wiced_queue_t* queue, uint32_t *count );


/** @} */
/*****************************************************************************/
/** @addtogroup events       Events
 *  @ingroup rtos
 *
 * Event management functions
 *
 *
 *  @{
 */
/** Sends an asynchronous event to the associated worker thread
 *
 * Sends an asynchronous event to the associated worker thread
 *
 * @param worker_thread :the worker thread in which context the callback should execute from
 * @param function      : the callback function to be called from the worker thread
 * @param arg           : the argument to be passed to the callback function
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_send_asynchronous_event( wiced_worker_thread_t* worker_thread, event_handler_t function, void* arg );

/** @} */
/*****************************************************************************/
/** @addtogroup eventflags       Event Flags
 *  @ingroup rtos
 *
 * Event flags management functions
 *
 *
 *  @{
 */
/*****************************************************************************/

/** Creates a new instance of event flags
 *
 * Creates a new event_flag (Allocates memory for a new event flags instance and
 * returns the pointer)
 *
 * @return    valid pointer : on success.
 * @return    NULL          : if an error occurred
 */
wiced_event_flags_t*  wiced_rtos_create_event_flags( void );


/** Initialise an event flags
 *
 * @param event_flags : a pointer to the event flags handle
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_init_event_flags( wiced_event_flags_t* event_flags );


/** Wait for event flags to be set
 *
 * @param event_flags        : a pointer to the event flags handle
 * @param flags_to_wait_for  : a group of event flags (ORed bit-fields) to wait for
 * @param flags_set          : event flag(s) set
 * @param clear_set_flags    : option to clear set flag(s)
 * @param wait_option        : wait option
 * @param timeout_ms         : timeout in milliseconds
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_wait_for_event_flags( wiced_event_flags_t* event_flags, uint32_t flags_to_wait_for, uint32_t* flags_set, wiced_bool_t clear_set_flags, wiced_event_flags_wait_option_t wait_option, uint32_t timeout_ms );


/** Set event flags
 *
 * @param event_flags  : a pointer to the event flags handle
 * @param flags_to_set : a group of event flags (ORed bit-fields) to set
 *
 * @return    WICED_SUCCESS : on success.
 * @return    WICED_ERROR   : if an error occurred
 */
wiced_result_t wiced_rtos_set_event_flags( wiced_event_flags_t* event_flags, uint32_t flags_to_set );

/** @} */


#ifdef __cplusplus
} /*extern "C" */
#endif
