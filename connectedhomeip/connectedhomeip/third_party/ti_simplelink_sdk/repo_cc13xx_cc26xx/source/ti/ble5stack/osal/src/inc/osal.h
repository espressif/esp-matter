/******************************************************************************

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2004-2022, Texas Instruments Incorporated
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

/**
 *  @file  osal.h
 *  @brief      This API allows the software components in the Z-Stack to be written
 *  independently of the specifics of the operating system, kernel, or
 *  tasking environment (including control loops or connect-to-interrupt
 *  systems).
 */

#ifndef OSAL_H
#define OSAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include <limits.h>

#include "comdef.h"
#include "osal_memory.h"
#include "osal_timers.h"

#ifdef USE_ICALL
#include <icall.h>
#endif /* USE_ICALL */

/*********************************************************************
 * MACROS
 */
/// @cond NODOC
#if ( UINT_MAX == 65535 ) /* 8-bit and 16-bit devices */
  #define osal_offsetof(type, member) ((uint16) &(((type *) 0)->member))
#else /* 32-bit devices */
  #define osal_offsetof(type, member) ((uint32) &(((type *) 0)->member))
#endif

#define OSAL_MSG_NEXT(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->next

#define OSAL_MSG_Q_INIT(q_ptr)      *(q_ptr) = NULL

#define OSAL_MSG_Q_EMPTY(q_ptr)     (*(q_ptr) == NULL)

#define OSAL_MSG_Q_HEAD(q_ptr)      (*(q_ptr))

#define OSAL_MSG_LEN(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->len

#define OSAL_MSG_ID(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->dest_id
/// @endcond // NODOC
/*********************************************************************
 * CONSTANTS
 */

/*** Interrupts ***/
#define INTS_ALL    0xFF      //!< All interrupts

/*********************************************************************
 * TYPEDEFS
 */
#ifdef USE_ICALL
typedef ICall_MsgHdr osal_msg_hdr_t;    //!< ICall Message header
#else /* USE_ICALL */
typedef struct
{
  void   *next;               //!< pointer to next
#ifdef OSAL_PORT2TIRTOS
  /* Limited OSAL port to TI-RTOS requires compatibility with ROM
   * code compiled with USE_ICALL compile flag.  */
  uint32 reserved;
#endif /* OSAL_PORT2TIRTOS */
  uint16 len;               //!< length
  uint8  dest_id;           //!< destination ID
} osal_msg_hdr_t;
#endif /* USE_ICALL */

/// @brief OSAL Event Header
typedef struct
{
  uint8  event;           //!< OSAL event
  uint8  status;          //!< event status
} osal_event_hdr_t;

typedef void * osal_msg_q_t;      //!< osal message queue

#ifdef USE_ICALL
/** @brief High resolution timer callback function type */
typedef void (*osal_highres_timer_cback_t)(void *arg);
#endif /* USE_ICALL */

#ifdef ICALL_LITE
/** @brief ICall Message hook*/
typedef void (*osal_icallMsg_hook_t)(void * param);
#endif /* ICALL_LITE */

/*********************************************************************
 * GLOBAL VARIABLES
 */
#ifdef USE_ICALL
#ifdef ICALL_EVENTS
extern ICall_SyncHandle osal_syncHandle;      //!< OSAL synchronization handle
#else /* !ICALL_EVENTS */
extern ICall_Semaphore osal_semaphore;        //!< OSAL semaphore
#endif /* ICALL_EVENTS */
extern ICall_EntityID osal_entity;            //!< OSAL entity
extern uint_least32_t osal_tickperiod;        //!< OSAL ticket period
extern void (*osal_eventloop_hook)(void);     //!< OSAL event loop hook
#endif /* USE_ICALL */


/*********************************************************************
 * FUNCTIONS
 */

/// @cond NODOC
/*** Message Management ***/

/**
 * @brief Allocate message
 *
 * This function is called by a task to allocate a message buffer
 * into which the task will encode the particular message it wishes
 * to send.  This common buffer scheme is used to strictly limit the
 * creation of message buffers within the system due to RAM size
 * limitations on the microprocessor.   Note that all message buffers
 * are a fixed size (at least initially).  The parameter len is kept
 * in case a message pool with varying fixed message sizes is later
 * created (for example, a pool of message buffers of size LARGE,
 * MEDIUM and SMALL could be maintained and allocated based on request
 * from the tasks).
 *
 * @param uint8 len wanted buffer length
 *
 * @return pointer to allocated buffer
 * @return NULL if allocation failed.
 */
  extern uint8 * osal_msg_allocate(uint16 len );

/**
 * @brief  Deallocate Message
 *
 * This function is used to deallocate a message buffer. This function
 * is called by a task (or processing element) after it has finished
 * processing a received message.
 *
 * @param uint8 *msg_ptr pointer to new message buffer
 *
 * @return @ref SUCCESS
 * @return @ref INVALID_MSG_POINTER
 */
  extern uint8 osal_msg_deallocate( uint8 *msg_ptr );

/**
 * @brief Send Osal Message
 *
 * This function is called by a task to send a command message to
 * another task or processing element.  The sending_task field must
 * refer to a valid task, since the task ID will be used
 * for the response message.  This function will also set a message
 * ready event in the destination tasks event list.
 *
 *
 * @param uint8 destination_task Send msg to Task ID
 * @param uint8 *msg_ptr pointer to new message buffer
 *
 * @return @ref SUCCESS
 * @return @ref INVALID_TASK
 * @return @ref INVALID_MSG_POINTER
 */
  extern uint8 osal_msg_send( uint8 destination_task, uint8 *msg_ptr );

/**
 * @brief Send Service Complete message
 *
 * This function is called by icall lite translation when it is done
 * servicing the direct API requested.  The sending_task field
 * refer the initial calling task (the proxy task calling the direct API):
 * It is always consider as valid.
 * This function will send a message to the proxy task.
 * ready event in the destination tasks event list.
 *
 *
 * @param uint8 destination_task Send msg to Task ID
 * @param uint8 *msg_ptr pointer to new message buffer
 *
 * @return @ref SUCCESS
 * @return @ref INVALID_TASK
 * @return @ref INVALID_MSG_POINTER
 */
  extern uint8 osal_service_complete( uint8 destination_task, uint8 *msg_ptr );

/**
 * @brief Push a command message to head of OSAL queue.
 *
 * This function is called by a task to push a command message
 * to the head of the OSAL queue. The destination_task field
 * must refer to a valid task, since the task ID will be used to
 * send the message to. This function will also set a message
 * ready event in the destination task's event list.
 *
 * @param uint8 destination_task Send msg to Task ID
 * @param uint8 *msg_ptr pointer to message buffer
 *
 * @return @ref SUCCESS
 * @return @ref INVALID_TASK
 * @return @ref INVALID_MSG_POINTER
 */
  extern uint8 osal_msg_push_front( uint8 destination_task, uint8 *msg_ptr );

/**
 * @brief Receive command message
 *
 * This function is called by a task to retrieve a received command
 * message. The calling task must deallocate the message buffer after
 * processing the message using the osal_msg_deallocate() call.
 *
 * @param uint8 task_id receiving tasks ID
 *
 * @return message information
 * @return NULL if no message
 */
  extern uint8 *osal_msg_receive( uint8 task_id );

/**
 * @brief Finds in place an OSAL message matching the task_id and event parameters.
 *
 * @param task_id The OSAL task id that the enqueued OSAL message must match.
 * @param event The OSAL event id that the enqueued OSAL message must match.
 *
 * @return NULL if no match
 * @return otherwise an in place pointer to the matching OSAL message
 */
  extern osal_event_hdr_t *osal_msg_find(uint8 task_id, uint8 event);

/**
 * @brief Counts the number of messages in the OSAL message queue with a
 *              a given task ID and message event type.
 *
 * @param task_id The OSAL task id that the enqueued OSAL message must match.
 * @param event The OSAL event id that the enqueued OSAL message must match. 0xFF for
 *              all events.
 *
 * @return The number of OSAL messages that match the task ID and Event.
 */
  extern uint8 osal_msg_count(uint8 task_id, uint8 event);

/**
 * @brief Equeues an OSAL message into an OSAL queue.
 *
 * @param q_ptr OSAL queue
 * @param msg_ptr OSAL message
 */
  extern void osal_msg_enqueue( osal_msg_q_t *q_ptr, void *msg_ptr );

/**
 * @brief Enqueues an OSAL message into an OSAL queue if
 *    the length of the queue is less than max.
 *
 * @param q_ptr OSAL queue
 * @param msg_ptr OSAL message
 * @param max maximum length of queue
 *
 * @return TRUE if message was enqueued
 * @return FALSE otherwise
 */
  extern uint8 osal_msg_enqueue_max( osal_msg_q_t *q_ptr, void *msg_ptr, uint8 max );

/**
 * @brief Dequeues an OSAL message from an OSAL queue.
 *
 * @param q_ptr OSAL queue
 *
 * @return NULL of queue is empty
 * @return otherwise, pointer to OSAL message
 */
  extern void *osal_msg_dequeue( osal_msg_q_t *q_ptr );

/**
 * @brief Pushes an OSAL message to the head of an OSAL queue.
 *
 * @param q_ptr OSAL queue
 * @param msg_ptr OSAL message
 */
  extern void osal_msg_push( osal_msg_q_t *q_ptr, void *msg_ptr );

/**
 * @brief Extracts and removes an OSAL message from the
 *    middle of an OSAL queue.
 *
 * @param q_ptr OSAL queue
 * @param msg_ptr OSAL message to be extracted
 * @param prev_ptr OSAL message before msg_ptr in queue
 */
  extern void osal_msg_extract( osal_msg_q_t *q_ptr, void *msg_ptr, void *prev_ptr );

#ifdef USE_ICALL
/**
 * @brief Service function for messaging service
 *
 * @param args  arguments.
 *
 * @return  ICall error code
 */
  extern ICall_Errno osal_service_entry(ICall_FuncArgsHdr *args);
#endif /* USE_ICALL */


/*** Task Synchronization  ***/

/**
 * @brief Set the event flags for a task.
 *
 * The event passed in is OR'd into the task's event variable.
 *
 * @param task_id receiving tasks ID
 * @param event_flag what event to set
 *
 * @return @ref SUCCESS
 * @return @ref MSG_BUFFER_NOT_AVAIL
 * @return @ref FAILURE
 * @return @ref INVALID_TASK
 */
  extern uint8 osal_set_event( uint8 task_id, uint16 event_flag );


/**
 * @brief Clear the event flags for a task.
 *
 * The event passed in is masked out of the task's event variable.
 *
 * @param task_id receiving tasks ID
 * @param event_flag what event to clear
 *
 * @return @ref SUCCESS
 * @return @ref INVALID_TASK
 */
  extern uint8 osal_clear_event( uint8 task_id, uint16 event_flag );


/*** Interrupt Management  ***/

/**
 * @brief Register a service routine with an interrupt.
 *
 * When the interrupt occurs, this service routine is called.
 *
 * @param interrupt_id Interrupt number
 * @param (*isr_ptr)( uint8* ) function pointer to ISR
 *
 * @return  SUCCESS
 * @return INVALID_INTERRUPT_ID
 */
  extern uint8 osal_isr_register( uint8 interrupt_id, void (*isr_ptr)( uint8* ) );

/**
 * @brief Enable an interrupt.
 *
 * Once enabled, occurrence of the interrupt causes the service routine associated
 * with that interrupt to be called.
 *
 * If @ref INTS_ALL is the interrupt_id, interrupts (in general) are enabled.
 * If a single interrupt is passed in, then interrupts still have
 * to be enabled with another call to @ref INTS_ALL.
 *
 * @param interrupt_id Interrupt number
 *
 * @return @ref SUCCESS
 * @return @ref INVALID_INTERRUPT_ID
 */
  extern uint8 osal_int_enable( uint8 interrupt_id );

/**
 * @brief Disable an interrupt.
 *
 * When a disabled interrupt occurs, the service routine associated with that
 * interrupt is not called.
 *
 * If @ref INTS_ALL is the interrupt_id, interrupts (in general) are disabled.
 * If a single interrupt is passed in, then just that interrupt is disabled.
 *
 * @param uint8 interrupt_id Interrupt number
 *
 * @return @ref SUCCESS
 * @return @ref INVALID_INTERRUPT_ID
 */
  extern uint8 osal_int_disable( uint8 interrupt_id );


/*** Task Management  ***/

#ifdef USE_ICALL
/**
 * @brief  Map a task id to an ICall entity id for messaging in
 *          both directions (sending and receiving).
 *
 * @param taskid OSAL task id
 * @param dispatchid ICall entity id
 */
  extern void osal_enroll_dispatchid(uint8 taskid,
                                     ICall_EntityID dispatchid);

/**
 * @brief   Map a task id to an ICall entity id
 *
 * This taks id shall be used
 * just for sending a message from an OSAL task.
 * Note that osal_enroll_dispatchid() must never be called
 * with the same OSAL task id used in this function call.
 * However, it is OK to call osal_enroll_dispatchid()
 * with the same ICall entity id and a different OSAL task id.
 *
 * @param taskid OSAL task id
 * @param dispatchid ICall entity id
 */
  extern void osal_enroll_senderid(uint8 taskid, ICall_EntityID dispatchid);

/**
 * @brief   Map no task id to an ICall entity id
 *
 * This task id shall be used
 * just for sending a message from non-OSAL task.
 *
 * @param dispatchid ICall entity id
 */
  extern void osal_enroll_notasksender(ICall_EntityID dispatchid);

#ifdef ICALL_JT
/**
 * @brief Initialize global OSAL timer variable.
 *
 * @param tickPeriod period of a RTOS tick in us
 * @param osalMaxMsecs max possible timer duration
 */
  void osal_timer_init(uint_least32_t tickPeriod, uint_least32_t osalMaxMsecs);
#endif /* ICALL_JT */

#endif /* USE_ICALL */

/**
 * @brief Initializes the "task" system by creating the
 *   tasks defined in the task table.
 *
 * @return  SUCCESS
 */
  extern uint8 osal_init_system( void );

  /*
   * System Processing Loop
   */
/**
 * @brief This function is the main loop function of the task system (if
 * ZBIT and UBIT are not defined).
 *
 * @note This function does not return.
 */
#if defined (ZBIT)
  extern __declspec(dllexport)  void osal_start_system( void );
#else
  extern void osal_start_system( void );
#endif

/**
 * @brief OSAL main loop
 *
 * This function will make one pass through the OSAL taskEvents table
 * and call the task_event_processor() function for the first task that
 * is found with at least one event pending. If there are no pending
 * events (all tasks), this function puts the processor into Sleep.
 */
  extern void osal_run_system( void );

/**
 * @brief This function returns the task ID of the current (active) task.
 *
 * @return active task ID
 * @return TASK_NO_TASK if no task is active
 */
  extern uint8 osal_self( void );

/// @endcond // NODOC

/*** Helper Functions ***/

/**
 * @brief Calculates the length of a string.
 *
 * @note The string must be null terminated.
 *
 * @param   *pString pointer to text string
 *
 * @return  number of characters
 */
  extern int osal_strlen( char *pString );

/**
 * @brief Generic memory copy.
 *
 * @note This function differs from the standard memcpy(), since
 * it returns the pointer to the next destination uint8. The
 * standard memcpy() returns the original destination address.
 *
 * @param dst destination address
 * @param src source address
 * @param len number of bytes to copy
 *
 * @return pointer to end of destination buffer
 */
  extern void *osal_memcpy( void *dst, const void GENERIC *src, unsigned int len);

/**
 * @brief   Allocates a buffer and copies
 *          the src buffer into the newly allocated space.
 *
 * Allocation is done with with @ref osal_mem_alloc
 *
 * @param src source address
 * @param len number of bytes to copy
 *
 * @return pointer to the new allocated buffer
 * @return NULL if allocation fails
 */
  extern void *osal_memdup( const void GENERIC *src, unsigned int len );

/**
 * @brief   Generic reverse memory copy.
 *
 * Starts at the end of the
 * source buffer, by taking the source address pointer and moving
 * pointer ahead "len" bytes, then decrementing the pointer.
 *
 * @note This function differs from the standard memcpy(), since
 * it returns the pointer to the next destination uint8. The
 * standard memcpy() returns the original destination address.
 *
 * @param dst destination address
 * @param src source address
 * @param len number of bytes to copy
 *
 * @return  pointer to end of destination buffer
 */
  extern void *osal_revmemcpy( void *dst, const void GENERIC *src, unsigned int len);

/**
 * @brief Generic memory compare.
 *
 * @param src1 source 1 address
 * @param src2 source 2 address
 * @param len number of bytes to compare
 *
 * @return TRUE if same
 * @return FALSE if different
 */
  extern uint8 osal_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len );

/**
 * @brief Set memory buffer to value.
 *
 * @param dest pointer to buffer
 * @param value what to set each uint8 of the message
 * @param size how big
 *
 * @return pointer to destination buffer
 */
  extern void *osal_memset( void *dest, uint8 value, int size );

/**
 * @brief Build a uint16 out of 2 bytes (0 then 1).
 *
 * @param swapped 0 then 1
 *
 * @return uint16
 */
  extern uint16 osal_build_uint16( uint8 *swapped );

/**
 * @brief Build a uint32 out of sequential bytes.
 *
 * @param swapped sequential bytes
 * @param len number of bytes in the uint8 array
 *
 * @return  uint32
 */
  extern uint32 osal_build_uint32( uint8 *swapped, uint8 len );

/**
 * @brief Convert a long unsigned int to a string.
 *
 * @param  l long to convert
 * @param  buf buffer to convert to
 * @param  radix 10 dec, 16 hex
 *
 * @return pointer to buffer
 */
  #if !defined ( ZBIT ) && !defined ( ZBIT2 ) && !defined (UBIT)
    extern uint8 *_ltoa( uint32 l, uint8 * buf, uint8 radix );
  #endif

/**
 * @brief  Random number generator
 *
 * @return  new random number
 */
  extern uint16 osal_rand( void );

/**
 * @brief Buffer an uint32 value - LSB first.
 *
 * @param buf buffer
 * @param val uint32 value
 *
 * @return pointer to end of destination buffer
 */
  extern uint8* osal_buffer_uint32( uint8 *buf, uint32 val );

/**
 * @brief   Buffer an uint24 value - LSB first
 *
 * @note type uint24 is typedef to uint32 in comdef.h
 *
 * @param   buf buffer
 * @param   val uint24 value
 *
 * @return pointer to end of destination buffer
 */
  extern uint8* osal_buffer_uint24( uint8 *buf, uint24 val );

/**
 * @brief Check if all of the array elements are set to a value
 *
 * @param buf buffer to check
 * @param val value to check each array element for
 * @param len length to check
 *
 * @return  TRUE if all "val"
 * @return  FALSE otherwise
 */
  extern uint8 osal_isbufset( uint8 *buf, uint8 val, uint8 len );

#ifdef ICALL_LITE
/**
 * @brief Set the hook used to parse icall message.
 *
 * @param param pointer to a function that will be called if a icall message
 *          is found.
 */
  extern void osal_set_icall_hook( osal_icallMsg_hook_t param );

/**
 * @brief Assign or retrieve a proxy OSAL task id for an external ICall entity.
 *
 * @param entity ICall entity id
 *
 * @return proxy OSAL task id
 */
  extern uint8 osal_alien2proxy(ICall_EntityID entity);

#endif /* ICALL_LITE */

/*-------------------------------------------------------------------
 * BLE_LOG FUNCTIONS
 */
//#define BLE_LOG
#ifdef BLE_LOG

#define BLE_LOG_MODULE_CTRL            0x01
#define BLE_LOG_MODULE_HOST            0x02
#define BLE_LOG_MODULE_APP             0x04
#define BLE_LOG_MODULE_OSAL_TASK       0x08
#define BLE_LOG_MODULE_RF_CMD          0x10

void bleLog_int_int(void *handle, uint32_t type, uint8_t *format, uint32_t param1, uint32_t param2);
void bleLog_int_str(void *handle, uint32_t type, uint8_t *format, uint32_t param1, char *param2);
void bleLog_int_time(void *handle, uint32_t type, uint8_t *start_str, uint32_t param1);

#define BLE_LOG_INT_INT(handle, type, format, param1, param2)         bleLog_int_int(handle, type, format, param1, param2)
#define BLE_LOG_INT_STR(handle, type, format, param1, param2)         bleLog_int_str(handle, type, format, param1, param2)
#define BLE_LOG_INT_TIME(handle, type, start_str, param1)             bleLog_int_time(handle, type, start_str, param1)
#else
#define BLE_LOG_INT_INT(handle, type, format, param1, param2)
#define BLE_LOG_INT_STR(handle, type, format, param1, param2)
#define BLE_LOG_INT_TIME(handle, type, start_str, param1)

#endif //BLE_LOG

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* OSAL_H */
