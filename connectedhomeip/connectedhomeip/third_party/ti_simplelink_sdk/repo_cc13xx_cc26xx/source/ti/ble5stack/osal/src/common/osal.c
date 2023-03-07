/******************************************************************************

 @file  osal.c

 @brief This API allows the software components in the Z-stack to be written
        independently of the specifics of the operating system, kernel or
        tasking environment (including control loops or connect-to-interrupt
        systems).

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

/*********************************************************************
 * INCLUDES
 */

#include <string.h>

#include "comdef.h"
#include "hal_board.h"
#include "osal.h"
#include "osal_tasks.h"
#include "osal_memory.h"
#include "osal_pwrmgr.h"
#include "osal_clock.h"

#include "onboard.h"

/* HAL */
#include "hal_drivers.h"

#ifdef IAR_ARMCM3_LM
  #include "FreeRTOSConfig.h"
  #include "osal_task.h"
#endif

#ifdef USE_ICALL
#ifdef ICALL_JT
  #include "icall_jt.h"
#else
  #include <icall.h>
#endif /* ICALL_JT */
#endif /* USE_ICALL */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#ifdef USE_ICALL
// A bit mask to use to indicate a proxy OSAL task ID.
#define OSAL_PROXY_ID_FLAG       0x80

// Type of event OSAL will send to signal a proxy task
#define OSAL_EVENT_SERVICE      0
#define OSAL_EVENT_MSG          1

#endif // USE_ICALL

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Message Pool Definitions
osal_msg_q_t osal_qHead;

#ifdef USE_ICALL
// OSAL event loop hook function pointer
void (*osal_eventloop_hook)(void) = NULL;
#endif /* USE_ICALL */

#ifdef ICALL_LITE
osal_icallMsg_hook_t osal_icallMsg_hook = NULL;
#endif /* ICALL_LITE */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Index of active task
static uint8 activeTaskID = TASK_NO_TASK;

#ifdef USE_ICALL
// Maximum number of proxy tasks
#ifndef OSAL_MAX_NUM_PROXY_TASKS
#define OSAL_MAX_NUM_PROXY_TASKS 4
#endif // OSAL_MAX_NUM_PROXY_TASKS

// ICall entity ID value used to indicate invalid value
#define OSAL_INVALID_DISPATCH_ID 0xffu

// Synchronous object associated with OSAL RTOS thread receive queue
#ifdef ICALL_EVENTS
ICall_SyncHandle osal_syncHandle;
#else /* !ICALL_EVENTS */
ICall_Semaphore osal_semaphore;
#endif /* ICALL_EVENTS */

// Entity ID that OSAL RTOS thread has registered with
ICall_EntityID osal_entity;

// RTOS tick period in microseconds
uint_least32_t osal_tickperiod;

// Maximum timeout value in milliseconds that can be used with an RTOS timer
uint_least32_t osal_max_msecs;

// Timer ID for RTOS timer as back-end engine for OSAL timer
static ICall_TimerID osal_timerid_msec_timer;

// Timer callback sequence tracking counter to handle race condition
static unsigned osal_msec_timer_seq = 0;

// proxy task ID map
static uint8 osal_proxy_tasks[OSAL_MAX_NUM_PROXY_TASKS];

// service dispatcher entity IDs corresponding to OSAL tasks
static uint8 *osal_dispatch_entities;

static uint8 osal_notask_entity;

#endif // USE_ICALL

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

static uint8 osal_msg_enqueue_push( uint8 destination_task, uint8 *msg_ptr, uint8 urgent );

#ifdef USE_ICALL
static ICall_EntityID osal_proxy2alien(uint8 proxyid);
static uint8 osal_dispatch2id(ICall_EntityID entity);

#ifndef ICALL_LITE
static uint8 osal_alien2proxy(ICall_EntityID entity);
#endif /* ICALL_LITE */

static void osal_msec_timer_cback(void *arg);
#endif // USE_ICALL

/*********************************************************************
 * HELPER FUNCTIONS
 */
/* very ugly stub so Keil can compile */
#ifdef __KEIL__
char *  itoa ( int value, char * buffer, int radix )
{
  return(buffer);
}
#endif

/*********************************************************************
 * @fn      osal_strlen
 *
 * @brief
 *
 *   Calculates the length of a string.  The string must be null
 *   terminated.
 *
 * @param   char *pString - pointer to text string
 *
 * @return  int - number of characters
 */
int osal_strlen( char *pString )
{
  return (int)( strlen( pString ) );
}

/*********************************************************************
 * @fn      osal_memcpy
 *
 * @brief
 *
 *   Generic memory copy.
 *
 *   Note: This function differs from the standard memcpy(), since
 *         it returns the pointer to the next destination uint8. The
 *         standard memcpy() returns the original destination address.
 *
 * @param   dst - destination address
 * @param   src - source address
 * @param   len - number of bytes to copy
 *
 * @return  pointer to end of destination buffer
 */
void *osal_memcpy( void *dst, const void GENERIC *src, unsigned int len )
{
  uint8 *pDst;
  const uint8 GENERIC *pSrc;

  pSrc = src;
  pDst = dst;

  while ( len-- )
    *pDst++ = *pSrc++;

  return ( pDst );
}

/*********************************************************************
 * @fn      osal_revmemcpy
 *
 * @brief   Generic reverse memory copy.  Starts at the end of the
 *   source buffer, by taking the source address pointer and moving
 *   pointer ahead "len" bytes, then decrementing the pointer.
 *
 *   Note: This function differs from the standard memcpy(), since
 *         it returns the pointer to the next destination uint8. The
 *         standard memcpy() returns the original destination address.
 *
 * @param   dst - destination address
 * @param   src - source address
 * @param   len - number of bytes to copy
 *
 * @return  pointer to end of destination buffer
 */
void *osal_revmemcpy( void *dst, const void GENERIC *src, unsigned int len )
{
  uint8 *pDst;
  const uint8 GENERIC *pSrc;

  pSrc = src;
  pSrc += (len-1);
  pDst = dst;

  while ( len-- )
    *pDst++ = *pSrc--;

  return ( pDst );
}

/*********************************************************************
 * @fn      osal_memdup
 *
 * @brief   Allocates a buffer [with osal_mem_alloc()] and copies
 *          the src buffer into the newly allocated space.
 *
 * @param   src - source address
 * @param   len - number of bytes to copy
 *
 * @return  pointer to the new allocated buffer, or NULL if
 *          allocation problem.
 */
void *osal_memdup( const void GENERIC *src, unsigned int len )
{
  uint8 *pDst;

  pDst = osal_mem_alloc( len );
  if ( pDst )
  {
    VOID osal_memcpy( pDst, src, len );
  }

  return ( (void *)pDst );
}

/*********************************************************************
 * @fn      osal_memcmp
 *
 * @brief
 *
 *   Generic memory compare.
 *
 * @param   src1 - source 1 address
 * @param   src2 - source 2 address
 * @param   len - number of bytes to compare
 *
 * @return  TRUE - same, FALSE - different
 */
uint8 osal_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len )
{
  const uint8 GENERIC *pSrc1;
  const uint8 GENERIC *pSrc2;

  pSrc1 = src1;
  pSrc2 = src2;

  while ( len-- )
  {
    if( *pSrc1++ != *pSrc2++ )
      return FALSE;
  }
  return TRUE;
}


/*********************************************************************
 * @fn      osal_memset
 *
 * @brief
 *
 *   Set memory buffer to value.
 *
 * @param   dest - pointer to buffer
 * @param   value - what to set each uint8 of the message
 * @param   size - how big
 *
 * @return  pointer to destination buffer
 */
void *osal_memset( void *dest, uint8 value, int len )
{
  return memset( dest, value, len );
}

/*********************************************************************
 * @fn      osal_build_uint16
 *
 * @brief
 *
 *   Build a uint16 out of 2 bytes (0 then 1).
 *
 * @param   swapped - 0 then 1
 *
 * @return  uint16
 */
uint16 osal_build_uint16( uint8 *swapped )
{
  return ( BUILD_UINT16( swapped[0], swapped[1] ) );
}

/*********************************************************************
 * @fn      osal_build_uint32
 *
 * @brief
 *
 *   Build a uint32 out of sequential bytes.
 *
 * @param   swapped - sequential bytes
 * @param   len - number of bytes in the uint8 array
 *
 * @return  uint32
 */
uint32 osal_build_uint32( uint8 *swapped, uint8 len )
{
  if ( len == 2 )
    return ( BUILD_UINT32( swapped[0], swapped[1], 0L, 0L ) );
  else if ( len == 3 )
    return ( BUILD_UINT32( swapped[0], swapped[1], swapped[2], 0L ) );
  else if ( len == 4 )
    return ( BUILD_UINT32( swapped[0], swapped[1], swapped[2], swapped[3] ) );
  else
    return ( (uint32)swapped[0] );
}

#if !defined ( ZBIT ) && !defined ( ZBIT2 ) && !defined (UBIT)
/*********************************************************************
 * @fn      _ltoa
 *
 * @brief
 *
 *   convert a long unsigned int to a string.
 *
 * @param  l - long to convert
 * @param  buf - buffer to convert to
 * @param  radix - 10 dec, 16 hex
 *
 * @return  pointer to buffer
 */
unsigned char * _ltoa(unsigned long l, unsigned char *buf, unsigned char radix)
{
#if defined (__TI_COMPILER_VERSION) || defined (__TI_COMPILER_VERSION__)
    #if (((defined(__TI_ARM__) || defined(__MSP430__) || defined(__TMS320C2000__)) && __TI_COMPILER_VERSION__ < 19006000) || \
         (defined(__ARP32__) && __TI_COMPILER_VERSION__ < 1001000) ||       \
         (defined(__PRU__) && __TI_COMPILER_VERSION__ < 2003002) ||         \
         (defined(__TMS320C6X__) && __TI_COMPILER_VERSION__ < 8002006) ||   \
         (defined(__TMS320C6X__) && __TI_COMPILER_VERSION__ >= 8003000 && __TI_COMPILER_VERSION__ < 8003003))
        return (unsigned char*)ltoa( l, (char *)buf);
    #else
        return ( (unsigned char*)ltoa( l, (char *)buf, radix ) );
    #endif
#elif defined( __GNUC__ )
  return ( (unsigned char *)ltoa( l, (char *)buf, radix ) );
#else
  unsigned char tmp1[10] = "", tmp2[10] = "", tmp3[10] = "";
  unsigned short num1, num2, num3;
  unsigned char i;

  buf[0] = '\0';

  if ( radix == 10 )
  {
    num1 = l % 10000;
    num2 = (l / 10000) % 10000;
    num3 = (unsigned short)(l / 100000000);

    if (num3) _itoa(num3, tmp3, 10);
    if (num2) _itoa(num2, tmp2, 10);
    if (num1) _itoa(num1, tmp1, 10);

    if (num3)
    {
      strcpy((char*)buf, (char const*)tmp3);
      for (i = 0; i < 4 - strlen((char const*)tmp2); i++)
        strcat((char*)buf, "0");
    }
    strcat((char*)buf, (char const*)tmp2);
    if (num3 || num2)
    {
      for (i = 0; i < 4 - strlen((char const*)tmp1); i++)
        strcat((char*)buf, "0");
    }
    strcat((char*)buf, (char const*)tmp1);
    if (!num3 && !num2 && !num1)
      strcpy((char*)buf, "0");
  }
  else if ( radix == 16 )
  {
    num1 = l & 0x0000FFFF;
    num2 = l >> 16;

    if (num2) _itoa(num2, tmp2, 16);
    if (num1) _itoa(num1, tmp1, 16);

    if (num2)
    {
      strcpy((char*)buf,(char const*)tmp2);
      for (i = 0; i < 4 - strlen((char const*)tmp1); i++)
        strcat((char*)buf, "0");
    }
    strcat((char*)buf, (char const*)tmp1);
    if (!num2 && !num1)
      strcpy((char*)buf, "0");
  }
  else
    return NULL;

  return buf;
#endif
}
#endif // !defined(ZBIT) && !defined(ZBIT2)

/*********************************************************************
 * @fn        osal_rand
 *
 * @brief    Random number generator
 *
 * @param   none
 *
 * @return  uint16 - new random number
 */
uint16 osal_rand( void )
{
  return ( Onboard_rand() );
}

/*********************************************************************
 * API FUNCTIONS
 *********************************************************************/

#ifdef USE_ICALL
/*********************************************************************
 * @fn      osal_prepare_svc_enroll
 *
 * @brief   Initialize data structures that map OSAL task ids to
 *          ICall entity ids.
 *
 * @param   none
 *
 * @return  none
 */
static void osal_prepare_svc_enroll(void)
{
  osal_dispatch_entities = (uint8 *) osal_mem_alloc(tasksCnt * 2);
  osal_memset(osal_dispatch_entities, OSAL_INVALID_DISPATCH_ID, tasksCnt * 2);
  osal_memset(osal_proxy_tasks, OSAL_INVALID_DISPATCH_ID,
              OSAL_MAX_NUM_PROXY_TASKS);
}

/*********************************************************************
 * @fn      osal_enroll_dispatchid
 *
 * @brief   Map a task id to an ICall entity id for messaging in
 *          both directions (sending and receiving).
 *
 * @param   taskid       OSAL task id
 * @param   dispatchid   ICall entity id
 *
 * @return  none
 */
void osal_enroll_dispatchid(uint8 taskid, ICall_EntityID dispatchid)
{
  osal_dispatch_entities[taskid] = dispatchid;
  osal_dispatch_entities[tasksCnt + taskid] = dispatchid;
}

/*********************************************************************
 * @fn      osal_enroll_senderid
 *
 * @brief   Map a task id to an ICall entity id, which shall be used
 *          just for sending a message from an OSAL task.
 *          Note that osal_enroll_dispatchid() must never be called
 *          with the same OSAL task id used in this function call.
 *          However, it is OK to call osal_enroll_dispatchid()
 *          with the same ICall entity id and a different OSAL task id.
 *
 * @param   taskid       OSAL task id
 * @param   dispatchid   ICall entity id
 *
 * @return  none
 */
void osal_enroll_senderid(uint8 taskid, ICall_EntityID dispatchid)
{
  osal_dispatch_entities[tasksCnt + taskid] = dispatchid;
}

/*********************************************************************
 * @fn      osal_enroll_notasksender
 *
 * @brief   Map no task id to an ICall entity id, which shall be used
 *          just for sending a message from non-OSAL task.
 *
 * @param   dispatchid   ICall entity id
 *
 * @return  none
 */
void osal_enroll_notasksender(ICall_EntityID dispatchid)
{
  osal_notask_entity = dispatchid;
}
#endif /* USE_ICALL */

/*********************************************************************
 * @fn      osal_msg_allocate
 *
 * @brief
 *
 *    This function is called by a task to allocate a message buffer
 *    into which the task will encode the particular message it wishes
 *    to send.  This common buffer scheme is used to strictly limit the
 *    creation of message buffers within the system due to RAM size
 *    limitations on the microprocessor.   Note that all message buffers
 *    are a fixed size (at least initially).  The parameter len is kept
 *    in case a message pool with varying fixed message sizes is later
 *    created (for example, a pool of message buffers of size LARGE,
 *    MEDIUM and SMALL could be maintained and allocated based on request
 *    from the tasks).
 *
 *
 * @param   uint8 len  - wanted buffer length
 *
 *
 * @return  pointer to allocated buffer or NULL if allocation failed.
 */
uint8 * osal_msg_allocate( uint16 len )
{
  osal_msg_hdr_t *hdr;

  if ( len == 0 )
    return ( NULL );

  hdr = (osal_msg_hdr_t *) osal_mem_alloc( (short)(len + sizeof( osal_msg_hdr_t )) );
  if ( hdr )
  {
    hdr->next = NULL;
    hdr->len = len;
    hdr->dest_id = TASK_NO_TASK;
    return ( (uint8 *) (hdr + 1) );
  }
  else
    return ( NULL );
}

/*********************************************************************
 * @fn      osal_msg_deallocate
 *
 * @brief
 *
 *    This function is used to deallocate a message buffer. This function
 *    is called by a task (or processing element) after it has finished
 *    processing a received message.
 *
 *
 * @param   uint8 *msg_ptr - pointer to new message buffer
 *
 * @return  SUCCESS, INVALID_MSG_POINTER
 */
uint8 osal_msg_deallocate( uint8 *msg_ptr )
{
  uint8 *x;

  if ( msg_ptr == NULL )
    return ( INVALID_MSG_POINTER );

  // don't deallocate queued buffer
  if ( OSAL_MSG_ID( msg_ptr ) != TASK_NO_TASK )
    return ( MSG_BUFFER_NOT_AVAIL );

  x = (uint8 *)((uint8 *)msg_ptr - sizeof( osal_msg_hdr_t ));

  osal_mem_free( (void *)x );

  return ( SUCCESS );
}

/*********************************************************************
 * @fn      osal_event_send
 *
 * @brief
 *
 *    This function is a sub routine for osal_msg_send and
 *    osal_service_copmplete.
 *    since the difference between those 2 functions are only the type of event
 *    to send to the proxy task.
 *
 *
 * @param   uint8 type - Type of event to send: OSAL_EVENT_SERVICE or
 *                       OSAL_EVENT_MSG
 * @param   uint8 destination_task - Send msg to Task ID
 * @param   uint8 *msg_ptr - pointer to new message buffer
 *
 * @return  SUCCESS, INVALID_TASK, INVALID_MSG_POINTER
 */
static int8 osal_event_send( uint8 type, uint8 destination_task, uint8 *msg_ptr )
{
    /* Destination is a proxy task */
    osal_msg_hdr_t *hdr = (osal_msg_hdr_t *)msg_ptr - 1;
    ICall_EntityID src, dst;

    uint8 taskid = osal_self();
    if (taskid == TASK_NO_TASK)
    {
      /* Call must have been made from either an ISR or a user-thread */
      src = osal_notask_entity;
    }
    else
    {
      src = (ICall_EntityID) osal_dispatch_entities[taskid + tasksCnt];
    }
    if (src == OSAL_INVALID_DISPATCH_ID)
    {
      /* The source entity is not registered */
      /* abort */
      ICall_abort();
      return FAILURE;
    }
    dst = osal_proxy2alien(destination_task);
    hdr->dest_id = TASK_NO_TASK;
    if (type == OSAL_EVENT_SERVICE)
    {
      if (ICall_sendServiceComplete(src, dst, ICALL_MSG_FORMAT_KEEP, msg_ptr) ==
        ICALL_ERRNO_SUCCESS)
      {
        return SUCCESS;
      }

    }
    else if (type == OSAL_EVENT_MSG)
    {
      if (ICall_send(src, dst, ICALL_MSG_FORMAT_KEEP, msg_ptr) ==
          ICALL_ERRNO_SUCCESS)
      {
        return SUCCESS;
      }
    }
    osal_msg_deallocate(msg_ptr);
    return FAILURE;
}
/*********************************************************************
 * @fn      osal_msg_send
 *
 * @brief
 *
 *    This function is called by a task to send a command message to
 *    another task or processing element.  The sending_task field must
 *    refer to a valid task, since the task ID will be used
 *    for the response message.  This function will also set a message
 *    ready event in the destination tasks event list.
 *
 *
 * @param   uint8 destination_task - Send msg to Task ID
 * @param   uint8 *msg_ptr - pointer to new message buffer
 *
 * @return  SUCCESS, INVALID_TASK, INVALID_MSG_POINTER
 */
uint8 osal_msg_send( uint8 destination_task, uint8 *msg_ptr )
{
  BLE_LOG_INT_INT(0, BLE_LOG_MODULE_OSAL_TASK, "OASL: msg send from taskId=%d, to taskId=%d\n", osal_self(), destination_task);

#ifdef USE_ICALL
  if (destination_task & OSAL_PROXY_ID_FLAG)
  {
    return( osal_event_send( OSAL_EVENT_MSG, destination_task, msg_ptr ) );
  }
#endif /* USE_ICALL */
  return ( osal_msg_enqueue_push( destination_task, msg_ptr, FALSE ) );
}

#ifdef ICALL_LITE
/*********************************************************************
 * @fn      osal_service_complete
 *
 * @brief
 *
 *    This function is called by icall lite translation when it is done
 *     servicing the direct API requested.  The sending_task field
 *    refer the initial calling stack. it is alwasy consider as valid.
 *    This function will send a message to the proxy task.
 *    ready event in the destination tasks event list.
 *
 *
 * @param   uint8 destination_task - Send msg to Task ID
 * @param   uint8 *msg_ptr - pointer to new message buffer
 *
 * @return  SUCCESS, INVALID_TASK, INVALID_MSG_POINTER
 */
uint8 osal_service_complete( uint8 destination_task, uint8 *msg_ptr )
{
  if (destination_task & OSAL_PROXY_ID_FLAG)
  {
    return( osal_event_send( OSAL_EVENT_SERVICE, destination_task, msg_ptr ) );
  }
  return FAILURE;
}
#endif /* ICALL_LITE */



/*********************************************************************
 * @fn      osal_msg_push_front
 *
 * @brief
 *
 *    This function is called by a task to push a command message
 *    to the head of the OSAL queue. The destination_task field
 *    must refer to a valid task, since the task ID will be used to
 *    send the message to. This function will also set a message
 *    ready event in the destination task's event list.
 *
 * @param   uint8 destination_task - Send msg to Task ID
 * @param   uint8 *msg_ptr - pointer to message buffer
 *
 * @return  SUCCESS, INVALID_TASK, INVALID_MSG_POINTER
 */
uint8 osal_msg_push_front( uint8 destination_task, uint8 *msg_ptr )
{
  return ( osal_msg_enqueue_push( destination_task, msg_ptr, TRUE ) );
}

/*********************************************************************
 * @fn      osal_msg_enqueue_push
 *
 * @brief
 *
 *    This function is called by a task to either enqueue (append to
 *    queue) or push (prepend to queue) a command message to the OSAL
 *    queue. The destination_task field must refer to a valid task,
 *    since the task ID will be used to send the message to. This
 *    function will also set a message ready event in the destination
 *    task's event list.
 *
 * @param   uint8 destination_task - Send msg to Task ID
 * @param   uint8 *msg_ptr - pointer to message buffer
 * @param   uint8 push - TRUE to push, otherwise enqueue
 *
 * @return  SUCCESS, INVALID_TASK, INVALID_MSG_POINTER
 */
static uint8 osal_msg_enqueue_push( uint8 destination_task, uint8 *msg_ptr, uint8 push )
{
  if ( msg_ptr == NULL )
  {
    return ( INVALID_MSG_POINTER );
  }

#ifdef USE_ICALL
  if (destination_task & OSAL_PROXY_ID_FLAG)
  {
    ICall_abort();
  }
#endif /* USE_ICALL */

  if ( destination_task >= tasksCnt )
  {
    osal_msg_deallocate( msg_ptr );
    return ( INVALID_TASK );
  }

  // Check the message header
  if ( OSAL_MSG_NEXT( msg_ptr ) != NULL ||
       OSAL_MSG_ID( msg_ptr ) != TASK_NO_TASK )
  {
    osal_msg_deallocate( msg_ptr );
    return ( INVALID_MSG_POINTER );
  }

  OSAL_MSG_ID( msg_ptr ) = destination_task;

  if ( push == TRUE )
  {
    // prepend the message
    osal_msg_push( &osal_qHead, msg_ptr );
  }
  else
  {
    // append the message
    osal_msg_enqueue( &osal_qHead, msg_ptr );
  }

  // Signal the task that a message is waiting
  osal_set_event( destination_task, SYS_EVENT_MSG );

  return ( SUCCESS );
}

/*********************************************************************
 * @fn      osal_msg_receive
 *
 * @brief
 *
 *    This function is called by a task to retrieve a received command
 *    message. The calling task must deallocate the message buffer after
 *    processing the message using the osal_msg_deallocate() call.
 *
 * @param   uint8 task_id - receiving tasks ID
 *
 * @return  *uint8 - message information or NULL if no message
 */
uint8 *osal_msg_receive( uint8 task_id )
{
  osal_msg_hdr_t *listHdr;
  osal_msg_hdr_t *prevHdr = NULL;
  osal_msg_hdr_t *foundHdr = NULL;
  halIntState_t   intState;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION(intState);

  // Point to the top of the queue
  listHdr = osal_qHead;

  // Look through the queue for a message that belongs to the asking task
  while ( listHdr != NULL )
  {
    if ( (listHdr - 1)->dest_id == task_id )
    {
      if ( foundHdr == NULL )
      {
        // Save the first one
        foundHdr = listHdr;
      }
      else
      {
        // Second msg found, stop looking
        break;
      }
    }
    if ( foundHdr == NULL )
    {
      prevHdr = listHdr;
    }
    listHdr = OSAL_MSG_NEXT( listHdr );
  }

  // Is there more than one?
  if ( listHdr != NULL )
  {
    // Yes, Signal the task that a message is waiting
    osal_set_event( task_id, SYS_EVENT_MSG );
  }
  else
  {
    // No more
    osal_clear_event( task_id, SYS_EVENT_MSG );
  }

  // Did we find a message?
  if ( foundHdr != NULL )
  {
    // Take out of the link list
    osal_msg_extract( &osal_qHead, foundHdr, prevHdr );
  }

  // Release interrupts
  HAL_EXIT_CRITICAL_SECTION(intState);

  return ( (uint8*) foundHdr );
}

/**************************************************************************************************
 * @fn          osal_msg_find
 *
 * @brief       This function finds in place an OSAL message matching the task_id and event
 *              parameters.
 *
 * input parameters
 *
 * @param       task_id - The OSAL task id that the enqueued OSAL message must match.
 * @param       event - The OSAL event id that the enqueued OSAL message must match.
 *
 * output parameters
 *
 * None.
 *
 * @return      NULL if no match, otherwise an in place pointer to the matching OSAL message.
 **************************************************************************************************
 */
osal_event_hdr_t *osal_msg_find(uint8 task_id, uint8 event)
{
  osal_msg_hdr_t *pHdr;
  halIntState_t intState;

  HAL_ENTER_CRITICAL_SECTION(intState);  // Hold off interrupts.

  pHdr = osal_qHead;  // Point to the top of the queue.

  // Look through the queue for a message that matches the task_id and event parameters.
  while (pHdr != NULL)
  {
    if (((pHdr-1)->dest_id == task_id) && (((osal_event_hdr_t *)pHdr)->event == event))
    {
      break;
    }

    pHdr = OSAL_MSG_NEXT(pHdr);
  }

  HAL_EXIT_CRITICAL_SECTION(intState);  // Release interrupts.

  return (osal_event_hdr_t *)pHdr;
}

/**************************************************************************************************
 * @fn          osal_msg_count
 *
 * @brief       This function counts the number of messages, in the OSAL message queue with a
 *              a given task ID and message event type.
 *
 * input parameters
 *
 * @param       task_id - The OSAL task id that the enqueued OSAL message must match.
 * @param       event - The OSAL event id that the enqueued OSAL message must match. 0xFF for
 *              all events.
 *
 * output parameters
 *
 * None.
 *
 * @return      The number of OSAL messages that match the task ID and Event.
 **************************************************************************************************
 */
uint8 osal_msg_count( uint8 task_id, uint8 event )
{
  uint8 count = 0;
  osal_msg_hdr_t *pHdr;
  halIntState_t intState;

  HAL_ENTER_CRITICAL_SECTION(intState);  // Hold off interrupts.

  pHdr = osal_qHead;  // Point to the top of the queue.

  // Look through the queue for a message that matches the task_id and event parameters.
  while (pHdr != NULL)
  {
    if ( ((pHdr-1)->dest_id == task_id)
        && ((event == 0xFF) || (((osal_event_hdr_t *)pHdr)->event == event)) )
    {
      count++;
    }

    pHdr = OSAL_MSG_NEXT(pHdr);
  }

  HAL_EXIT_CRITICAL_SECTION(intState);  // Release interrupts.

  return ( count );
}

/*********************************************************************
 * @fn      osal_msg_enqueue
 *
 * @brief
 *
 *    This function enqueues an OSAL message into an OSAL queue.
 *
 * @param   osal_msg_q_t *q_ptr - OSAL queue
 * @param   void *msg_ptr  - OSAL message
 *
 * @return  none
 */
void osal_msg_enqueue( osal_msg_q_t *q_ptr, void *msg_ptr )
{
  void *list;
  halIntState_t intState;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION(intState);

  OSAL_MSG_NEXT( msg_ptr ) = NULL;
  // If first message in queue
  if ( *q_ptr == NULL )
  {
    *q_ptr = msg_ptr;
  }
  else
  {
    // Find end of queue
    for ( list = *q_ptr; OSAL_MSG_NEXT( list ) != NULL; list = OSAL_MSG_NEXT( list ) );

    // Add message to end of queue
    OSAL_MSG_NEXT( list ) = msg_ptr;
  }

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION(intState);
}

/*********************************************************************
 * @fn      osal_msg_dequeue
 *
 * @brief
 *
 *    This function dequeues an OSAL message from an OSAL queue.
 *
 * @param   osal_msg_q_t *q_ptr - OSAL queue
 *
 * @return  void * - pointer to OSAL message or NULL of queue is empty.
 */
void *osal_msg_dequeue( osal_msg_q_t *q_ptr )
{
  void *msg_ptr = NULL;
  halIntState_t intState;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION(intState);

  if ( *q_ptr != NULL )
  {
    // Dequeue message
    msg_ptr = *q_ptr;
    *q_ptr = OSAL_MSG_NEXT( msg_ptr );
    OSAL_MSG_NEXT( msg_ptr ) = NULL;
    OSAL_MSG_ID( msg_ptr ) = TASK_NO_TASK;
  }

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION(intState);

  return msg_ptr;
}

/*********************************************************************
 * @fn      osal_msg_push
 *
 * @brief
 *
 *    This function pushes an OSAL message to the head of an OSAL
 *    queue.
 *
 * @param   osal_msg_q_t *q_ptr - OSAL queue
 * @param   void *msg_ptr  - OSAL message
 *
 * @return  none
 */
void osal_msg_push( osal_msg_q_t *q_ptr, void *msg_ptr )
{
  halIntState_t intState;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION(intState);

  // Push message to head of queue
  OSAL_MSG_NEXT( msg_ptr ) = *q_ptr;
  *q_ptr = msg_ptr;

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION(intState);
}

/*********************************************************************
 * @fn      osal_msg_extract
 *
 * @brief
 *
 *    This function extracts and removes an OSAL message from the
 *    middle of an OSAL queue.
 *
 * @param   osal_msg_q_t *q_ptr - OSAL queue
 * @param   void *msg_ptr  - OSAL message to be extracted
 * @param   void *prev_ptr  - OSAL message before msg_ptr in queue
 *
 * @return  none
 */
void osal_msg_extract( osal_msg_q_t *q_ptr, void *msg_ptr, void *prev_ptr )
{
  halIntState_t intState;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION(intState);

  if ( msg_ptr == *q_ptr )
  {
    // remove from first
    *q_ptr = OSAL_MSG_NEXT( msg_ptr );
  }
  else
  {
    // remove from middle
    OSAL_MSG_NEXT( prev_ptr ) = OSAL_MSG_NEXT( msg_ptr );
  }
  OSAL_MSG_NEXT( msg_ptr ) = NULL;
  OSAL_MSG_ID( msg_ptr ) = TASK_NO_TASK;

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION(intState);
}

/*********************************************************************
 * @fn      osal_msg_enqueue_max
 *
 * @brief
 *
 *    This function enqueues an OSAL message into an OSAL queue if
 *    the length of the queue is less than max.
 *
 * @param   osal_msg_q_t *q_ptr - OSAL queue
 * @param   void *msg_ptr  - OSAL message
 * @param   uint8 max - maximum length of queue
 *
 * @return  TRUE if message was enqueued, FALSE otherwise
 */
uint8 osal_msg_enqueue_max( osal_msg_q_t *q_ptr, void *msg_ptr, uint8 max )
{
  void *list;
  uint8 ret = FALSE;
  halIntState_t intState;

  // Hold off interrupts
  HAL_ENTER_CRITICAL_SECTION(intState);

  // If first message in queue
  if ( *q_ptr == NULL )
  {
    *q_ptr = msg_ptr;
    ret = TRUE;
  }
  else
  {
    // Find end of queue or max
    list = *q_ptr;
    max--;
    while ( (OSAL_MSG_NEXT( list ) != NULL) && (max > 0) )
    {
      list = OSAL_MSG_NEXT( list );
      max--;
    }

    // Add message to end of queue if max not reached
    if ( max != 0 )
    {
      OSAL_MSG_NEXT( list ) = msg_ptr;
      ret = TRUE;
    }
  }

  // Re-enable interrupts
  HAL_EXIT_CRITICAL_SECTION(intState);

  return ret;
}

/*********************************************************************
 * @fn      osal_set_event
 *
 * @brief
 *
 *    This function is called to set the event flags for a task. The
 *    event passed in is OR'd into the task's event variable.
 *
 * @param   uint8 task_id - receiving tasks ID
 * @param   uint8 event_flag - what event to set
 *
 * @return  SUCCESS, MSG_BUFFER_NOT_AVAIL, FAILURE, INVALID_TASK
 */
#if !defined USE_ICALL && !defined OSAL_PORT2TIRTOS
uint8 osal_set_event_raw( uint8 task_id, uint16 event_flag )
#else /* OSAL_PORT2TIRTOS */
uint8 osal_set_event( uint8 task_id, uint16 event_flag )
#endif /* OSAL_PORT2TIRTOS */
{
#ifdef USE_ICALL
  if (task_id & OSAL_PROXY_ID_FLAG)
  {
    /* Destination is a proxy task */
    osal_msg_hdr_t *hdr;
    ICall_EntityID src, dst;
    uint8 taskid;

    struct _osal_event_msg_t
    {
      uint16 signature;
      uint16 event_flag;
    } *msg_ptr = (struct _osal_event_msg_t *)
      osal_msg_allocate(sizeof(*msg_ptr));

    if (!msg_ptr)
    {
      return MSG_BUFFER_NOT_AVAIL;
    }
    msg_ptr->signature = 0xffffu;
    msg_ptr->event_flag = event_flag;
    hdr = (osal_msg_hdr_t *)msg_ptr - 1;

    taskid = osal_self();
    if (taskid == TASK_NO_TASK)
    {
      /* Call must have been made from either an ISR or a user-thread */
      src = osal_notask_entity;
    }
    else
    {
      src = (ICall_EntityID) osal_dispatch_entities[taskid + tasksCnt];
    }

    if (src == OSAL_INVALID_DISPATCH_ID)
    {
      /* The source entity is not registered */
      osal_msg_deallocate((uint8 *) msg_ptr);
      ICall_abort();
      return FAILURE;
    }
    dst = osal_proxy2alien(task_id);
    hdr->dest_id = TASK_NO_TASK;
    if (ICall_send(src, dst,
                   ICALL_MSG_FORMAT_KEEP, msg_ptr) ==
        ICALL_ERRNO_SUCCESS)
    {
      return SUCCESS;
    }
    osal_msg_deallocate((uint8 *) msg_ptr);
    return FAILURE;
  }
#endif /* USE_ICALL */

  if ( task_id < tasksCnt )
  {
    halIntState_t   intState;
    HAL_ENTER_CRITICAL_SECTION(intState);    // Hold off interrupts
    tasksEvents[task_id] |= event_flag;  // Stuff the event bit(s)
    HAL_EXIT_CRITICAL_SECTION(intState);     // Release interrupts
#ifdef USE_ICALL
#ifdef ICALL_EVENTS
    ICall_signal(osal_syncHandle);
#else /* !ICALL_EVENTS */
    ICall_signal(osal_semaphore);
#endif /* ICALL_EVENTS */
#endif /* USE_ICALL */
    return ( SUCCESS );
  }
   else
  {
    return ( INVALID_TASK );
  }
}

/*********************************************************************
 * @fn      osal_clear_event
 *
 * @brief
 *
 *    This function is called to clear the event flags for a task. The
 *    event passed in is masked out of the task's event variable.
 *
 * @param   uint8 task_id - receiving tasks ID
 * @param   uint8 event_flag - what event to clear
 *
 * @return  SUCCESS, INVALID_TASK
 */
uint8 osal_clear_event( uint8 task_id, uint16 event_flag )
{
  if ( task_id < tasksCnt )
  {
    halIntState_t   intState;
    HAL_ENTER_CRITICAL_SECTION(intState);    // Hold off interrupts
    tasksEvents[task_id] &= ~(event_flag);   // Clear the event bit(s)
    HAL_EXIT_CRITICAL_SECTION(intState);     // Release interrupts
    return ( SUCCESS );
  }
   else
  {
    return ( INVALID_TASK );
  }
}

/*********************************************************************
 * @fn      osal_isr_register
 *
 * @brief
 *
 *   This function is called to register a service routine with an
 *   interrupt. When the interrupt occurs, this service routine is called.
 *
 * @param   uint8 interrupt_id - Interrupt number
 * @param   void (*isr_ptr)( uint8* ) - function pointer to ISR
 *
 * @return  SUCCESS, INVALID_INTERRUPT_ID,
 */
uint8 osal_isr_register( uint8 interrupt_id, void (*isr_ptr)( uint8* ) )
{
  // Remove these statements when functionality is complete
  (void)interrupt_id;
  (void)isr_ptr;
  return ( SUCCESS );
}

/*********************************************************************
 * @fn      osal_int_enable
 *
 * @brief
 *
 *   This function is called to enable an interrupt. Once enabled,
 *   occurrence of the interrupt causes the service routine associated
 *   with that interrupt to be called.
 *
 *   If INTS_ALL is the interrupt_id, interrupts (in general) are enabled.
 *   If a single interrupt is passed in, then interrupts still have
 *   to be enabled with another call to INTS_ALL.
 *
 * @param   uint8 interrupt_id - Interrupt number
 *
 * @return  SUCCESS or INVALID_INTERRUPT_ID
 */
uint8 osal_int_enable( uint8 interrupt_id )
{

  if ( interrupt_id == INTS_ALL )
  {
    HAL_ENABLE_INTERRUPTS();
    return ( SUCCESS );
  }
  else
  {
    return ( INVALID_INTERRUPT_ID );
  }
}

/*********************************************************************
 * @fn      osal_int_disable
 *
 * @brief
 *
 *   This function is called to disable an interrupt. When a disabled
 *   interrupt occurs, the service routine associated with that
 *   interrupt is not called.
 *
 *   If INTS_ALL is the interrupt_id, interrupts (in general) are disabled.
 *   If a single interrupt is passed in, then just that interrupt is disabled.
 *
 * @param   uint8 interrupt_id - Interrupt number
 *
 * @return  SUCCESS or INVALID_INTERRUPT_ID
 */
uint8 osal_int_disable( uint8 interrupt_id )
{

  if ( interrupt_id == INTS_ALL )
  {
    HAL_DISABLE_INTERRUPTS();
    return ( SUCCESS );
  }
  else
  {
    return ( INVALID_INTERRUPT_ID );
  }
}

/*********************************************************************
 * @fn      osal_init_system
 *
 * @brief
 *
 *   This function initializes the "task" system by creating the
 *   tasks defined in the task table (osal_tasks.h).
 *
 * @param   void
 *
 * @return  SUCCESS
 */
uint8 osal_init_system( void )
{
#if !defined USE_ICALL && !defined OSAL_PORT2TIRTOS
  // Initialize the Memory Allocation System
  osal_mem_init();
#endif /* !defined USE_ICALL && !defined OSAL_PORT2TIRTOS */

  // Initialize the message queue
  osal_qHead = NULL;

  // Initialize the timers
  osalTimerInit();

  // Initialize the Power Management System
  osal_pwrmgr_init();

#ifdef USE_ICALL
  /* Prepare memory space for service enrollment */
  osal_prepare_svc_enroll();
#endif /* USE_ICALL */

  // Initialize the system tasks.
  osalInitTasks();

#if !defined USE_ICALL && !defined OSAL_PORT2TIRTOS
  // Setup efficient search for the first free block of heap.
  osal_mem_kick();
#endif /* !defined USE_ICALL && !defined OSAL_PORT2TIRTOS */

#ifdef USE_ICALL
#ifndef ICALL_JT
  osal_tickperiod = (uint_least32_t) ICall_getTickPeriod();
  osal_max_msecs = (uint_least32_t) ICall_getMaxMSecs();
#endif
  /* Reduce ceiling considering potential latency */
  osal_max_msecs -= 2;
#endif /* USE_ICALL */

  return ( SUCCESS );
}

/*********************************************************************
 * @fn      osal_timer_init
 *
 * @brief
 *
 *   This function initialize global OSAL timer variable.
 *
 * @param   tickPeriod: period of a RTOS tick in us
 *          osalMaxMsecs: max possible timer duration
 *
 * @return  none
 */
#ifdef USE_ICALL
#ifdef ICALL_JT
void osal_timer_init(uint_least32_t tickPeriod, uint_least32_t osalMaxMsecs )
{
  osal_tickperiod = tickPeriod;
  osal_max_msecs = osalMaxMsecs;
}
#endif /* ICALL_JT */
#endif /* USE_ICALL */
/*********************************************************************
 * @fn      osal_start_system
 *
 * @brief
 *
 *   This function is the main loop function of the task system (if
 *   ZBIT and UBIT are not defined). This Function doesn't return.
 *
 * @param   void
 *
 * @return  none
 */
void osal_start_system( void )
{
#ifdef USE_ICALL
  /* Kick off timer service in order to allocate resources upfront.
   * The first timeout is required to schedule next OSAL timer event
   * as well. */
  ICall_Errno errno = ICall_setTimer(1, osal_msec_timer_cback,
                                     (void *) osal_msec_timer_seq,
                                     &osal_timerid_msec_timer);
  if (errno != ICALL_ERRNO_SUCCESS)
  {
    ICall_abort();
  }
#endif /* USE_ICALL */

#if !defined ( ZBIT ) && !defined ( UBIT )
  for(;;)  // Forever Loop
#endif
  {
    osal_run_system();

#ifdef USE_ICALL
    ICall_wait(ICALL_TIMEOUT_FOREVER);
#endif /* USE_ICALL */
  }
}

#ifdef USE_ICALL
/*********************************************************************
 * @fn      osal_alien2proxy
 *
 * @brief
 *
 *   Assign or retrieve a proxy OSAL task id for an external ICall entity.
 *
 * @param   origid  ICall entity id
 *
 * @return  proxy OSAL task id
 */
#ifdef ICALL_LITE
uint8 osal_alien2proxy(ICall_EntityID origid)
#else
static uint8 osal_alien2proxy(ICall_EntityID origid)
#endif /* ICALL_LITE */
{
  size_t i;

  for (i = 0; i < OSAL_MAX_NUM_PROXY_TASKS; i++)
  {
    if (osal_proxy_tasks[i] == OSAL_INVALID_DISPATCH_ID)
    {
      /* proxy not found. Create a new one */
      osal_proxy_tasks[i] = (uint8) origid;
      return (OSAL_PROXY_ID_FLAG | i);
    }
    else if ((ICall_EntityID) osal_proxy_tasks[i] == origid)
    {
      return (OSAL_PROXY_ID_FLAG | i);
    }
  }
  /* abort */
  ICall_abort();
  return TASK_NO_TASK;
}

/*********************************************************************
 * @fn      osal_proxy2alien
 *
 * @brief
 *
 *   Retrieve the ICall entity id for a proxy OSAL task id
 *
 * @param   proxyid  Proxy OSAL task id
 *
 * @return  ICall entity id
 */
static ICall_EntityID osal_proxy2alien(uint8 proxyid)
{
  proxyid ^= OSAL_PROXY_ID_FLAG;
  if (proxyid >= OSAL_MAX_NUM_PROXY_TASKS)
  {
    /* abort */
    ICall_abort();
  }
  return (ICall_EntityID) osal_proxy_tasks[proxyid];
}

/*********************************************************************
 * @fn      osal_dispatch2id
 *
 * @brief
 *
 *   Retrieve OSAL task id mapped to a designated ICall entity id
 *
 * @param   entity  ICall entity id
 *
 * @return  OSAL task id
 */
static uint8 osal_dispatch2id(ICall_EntityID entity)
{
  size_t i;

  for (i = 0; i < tasksCnt; i++)
  {
    if ((ICall_EntityID) osal_dispatch_entities[i] == entity)
    {
      return i;
    }
  }
  return TASK_NO_TASK;
}

/*********************************************************************
 * @fn      osal_msec_timer_cback
 *
 * @brief
 *
 *   This function is a callback function for ICall_setTimer() service
 *   used to implement OSAL timer
 *
 * @param   arg  In this case, the timer sequence number is passed.
 *
 * @return  None
 */
static void osal_msec_timer_cback(void *arg)
{
  unsigned seq = (unsigned) arg;
  halIntState_t intState;

  HAL_ENTER_CRITICAL_SECTION(intState);
  if (seq == osal_msec_timer_seq)
  {
#ifdef ICALL_EVENTS
    ICall_signal(osal_syncHandle);
#else /* !ICALL_EVENTS */
    ICall_signal(osal_semaphore);
#endif /* ICALL_EVENTS */
  }
  HAL_EXIT_CRITICAL_SECTION(intState);
}

/*********************************************************************
 * @fn      osal_service_entry
 *
 * @brief
 *
 *   This function is service function for messaging service
 *
 * @param   args  arguments.
 *
 * @return  ICall error code
 */
ICall_Errno osal_service_entry(ICall_FuncArgsHdr *args)
{
  if (args->func == ICALL_MSG_FUNC_GET_LOCAL_MSG_ENTITY_ID)
  {
    /* Get proxy ID */
    ((ICall_GetLocalMsgEntityIdArgs *)args)->localId =
      osal_alien2proxy(((ICall_GetLocalMsgEntityIdArgs *)args)->entity);
    if (((ICall_GetLocalMsgEntityIdArgs *)args)->localId == TASK_NO_TASK)
    {
      return ICALL_ERRNO_NO_RESOURCE;
    }
  }
  else
  {
    return ICALL_ERRNO_INVALID_FUNCTION;
  }
  return ICALL_ERRNO_SUCCESS;
}
#endif /* USE_ICALL */

#ifdef ICALL_LITE
/*********************************************************************
 * @fn      osal_set_icall_hook
 *
 * @brief
 *
 *   This function will set the hook used to parse icall message.
 *
 * @param   param pointer to a function that will be called if a icall message
 *          is found.
 *
 * @return  nones
 */
void osal_set_icall_hook(osal_icallMsg_hook_t param)
{
  osal_icallMsg_hook =  (osal_icallMsg_hook_t) param;
}
#endif /* ICALL_LITE */

/*********************************************************************
 * @fn      osal_run_system
 *
 * @brief
 *
 *   This function will make one pass through the OSAL taskEvents table
 *   and call the task_event_processor() function for the first task that
 *   is found with at least one event pending. If there are no pending
 *   events (all tasks), this function puts the processor into Sleep.
 *
 * @param   void
 *
 * @return  none
 */
void osal_run_system( void )
{
  uint8 idx = 0;
 halIntState_t intState;
#ifdef USE_ICALL
  uint32 next_timeout_prior = osal_next_timeout();
#else /* USE_ICALL */
#ifndef HAL_BOARD_CC2538
  osalTimeUpdate();
#endif

  Hal_ProcessPoll();
#endif /* USE_ICALL */

#ifdef USE_ICALL
  if (next_timeout_prior)
  {
    osal_timer_refTimeUpdate();
    /* Set a value that will never match osal_next_timeout()
     * return value so that the next time can be scheduled.
     */
    next_timeout_prior = 0xfffffffful;
  }
  if (osal_eventloop_hook)
  {
    osal_eventloop_hook();
  }

  for (;;)
  {
    void *msg;
    ICall_EntityID src, dst;
    osal_msg_hdr_t *hdr;
    uint8 dest_id;

    if (ICall_fetchMsg(&src, &dst, &msg) != ICALL_ERRNO_SUCCESS)
    {
      break;
    }

    hdr = (osal_msg_hdr_t *) msg - 1;

#ifdef ICALL_LITE
    if (hdr->format == ICALL_MSG_FORMAT_DIRECT_API_ID)
    {
        if (osal_icallMsg_hook)
        {
          (*osal_icallMsg_hook)(msg);
        }
    }
    else
#endif /* ICALL_LITE */
    {
      dest_id = osal_dispatch2id(dst);
      if (dest_id == TASK_NO_TASK)
      {
        /* Something wrong */
        ICall_abort();
      }
      else
      {
        /* Message towards one of the tasks */
        /* Create a proxy task ID if necessary and
         * queue the message to the OSAL internal queue.
         */
        uint8 proxyid = osal_alien2proxy(hdr->srcentity);

        if (hdr->format == ICALL_MSG_FORMAT_1ST_CHAR_TASK_ID)
        {
          uint8 *bytes = msg;
          *bytes = proxyid;
        }
        else if (hdr->format == ICALL_MSG_FORMAT_3RD_CHAR_TASK_ID)
        {
          uint8 *bytes = msg;
          bytes[2] = proxyid;
        }
        /* now queue the message to the OSAL queue */
        osal_msg_send(dest_id, msg);
      }
    }
  }
#endif /* USE_ICALL */

  do {
    if (tasksEvents[idx])  // Task is highest priority that is ready.
    {
      break;
    }
  } while (++idx < tasksCnt);

  if (idx < tasksCnt)
  {
    uint16 events;

    HAL_ENTER_CRITICAL_SECTION(intState);
    events = tasksEvents[idx];
    tasksEvents[idx] = 0;  // Clear the Events for this task.
    HAL_EXIT_CRITICAL_SECTION(intState);

    activeTaskID = idx;
    events = (tasksArr[idx])( idx, events );
    activeTaskID = TASK_NO_TASK;

    HAL_ENTER_CRITICAL_SECTION(intState);
    tasksEvents[idx] |= events;  // Add back unprocessed events to the current task.
    HAL_EXIT_CRITICAL_SECTION(intState);
  }
#if defined( POWER_SAVING ) && !defined(USE_ICALL)
  else  // Complete pass through all task events with no activity?
  {
    osal_pwrmgr_powerconserve();  // Put the processor/system into sleep
  }
#endif

  /* Yield in case cooperative scheduling is being used. */
#if defined (configUSE_PREEMPTION) && (configUSE_PREEMPTION == 0)
  {
    osal_task_yield();
  }
#endif

#if defined USE_ICALL
  /* Note that scheduling wakeup at this point instead of
   * scheduling it upon ever OSAL start timer request,
   * would only work if OSAL start timer call is made
   * from OSAL tasks, but not from either ISR or
   * non-OSAL application thread.
   * In case, OSAL start timer is called from non-OSAL
   * task, the scheduling should be part of OSAL_Timers
   * module.
   * Such a change to OSAL_Timers module was not made
   * in order not to diverge the OSAL implementations
   * too drastically between pure OSAL solution vs.
   * OSAL upon service dispatcher (RTOS).
   * TODO: reconsider the above statement.
   */
  {
    uint32 next_timeout_post = osal_next_timeout();
    if (next_timeout_post != next_timeout_prior)
    {
      /* Next wakeup time has to be scheduled */
      if (next_timeout_post == 0)
      {
        /* No timer. Set time to the max */
        next_timeout_post = OSAL_TIMERS_MAX_TIMEOUT;
      }
      if (next_timeout_post > osal_max_msecs)
      {
        next_timeout_post = osal_max_msecs;
      }
      /* Restart timer */
      HAL_ENTER_CRITICAL_SECTION(intState);
      ICall_stopTimer(osal_timerid_msec_timer);
      ICall_setTimerMSecs(next_timeout_post, osal_msec_timer_cback,
                          (void *) (++osal_msec_timer_seq),
                          &osal_timerid_msec_timer);
      HAL_EXIT_CRITICAL_SECTION(intState);
    }

#ifdef ICALL_EVENTS
    /*
     * When using events, OSAL needs to be sure that it will not block itself
     * if any message have yet to been processed.  The OSAL event must be
     * signaled when any messages remain unprocessed at the end of this
     * function.
     */
    if (osal_qHead)
    {
      ICall_signal(osal_syncHandle);
    }
    else
    {
      for (uint8 i = 0; i < tasksCnt; i++)
      {
        if (tasksEvents[i] != 0)
        {
          ICall_signal(osal_syncHandle);
        }
      }
    }
#endif /* ICALL_EVENTS */
  }
#endif /* USE_ICALL */
}

/*********************************************************************
 * @fn      osal_buffer_uint32
 *
 * @brief
 *
 *   Buffer an uint32 value - LSB first.
 *
 * @param   buf - buffer
 * @param   val - uint32 value
 *
 * @return  pointer to end of destination buffer
 */
uint8* osal_buffer_uint32( uint8 *buf, uint32 val )
{
  *buf++ = BREAK_UINT32( val, 0 );
  *buf++ = BREAK_UINT32( val, 1 );
  *buf++ = BREAK_UINT32( val, 2 );
  *buf++ = BREAK_UINT32( val, 3 );

  return buf;
}

/*********************************************************************
 * @fn      osal_buffer_uint24
 *
 * @brief
 *
 *   Buffer an uint24 value - LSB first. Note that type uint24 is
 *   typedef to uint32 in comdef.h
 *
 * @param   buf - buffer
 * @param   val - uint24 value
 *
 * @return  pointer to end of destination buffer
 */
uint8* osal_buffer_uint24( uint8 *buf, uint24 val )
{
  *buf++ = BREAK_UINT32( val, 0 );
  *buf++ = BREAK_UINT32( val, 1 );
  *buf++ = BREAK_UINT32( val, 2 );

  return buf;
}

/*********************************************************************
 * @fn      osal_isbufset
 *
 * @brief
 *
 *   Is all of the array elements set to a value?
 *
 * @param   buf - buffer to check
 * @param   val - value to check each array element for
 * @param   len - length to check
 *
 * @return  TRUE if all "val"
 *          FALSE otherwise
 */
uint8 osal_isbufset( uint8 *buf, uint8 val, uint8 len )
{
  uint8 x;

  if ( buf == NULL )
  {
    return ( FALSE );
  }

  for ( x = 0; x < len; x++ )
  {
    // Check for non-initialized value
    if ( buf[x] != val )
    {
      return ( FALSE );
    }
  }
  return ( TRUE );
}

/*********************************************************************
 * @fn      osal_self
 *
 * @brief
 *
 *   This function returns the task ID of the current (active) task.
 *
 * @param   void
 *
 * @return   active task ID or TASK_NO_TASK if no task is active
 */
uint8 osal_self( void )
{
  return ( activeTaskID );
}

/*-------------------------------------------------------------------
 * BLE_LOG FUNCTIONS
 */
#ifdef BLE_LOG
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <ti/sysbios/knl/Clock.h>

#define LOG_BUF_SIZE    0x8000

uint8 gBleLogBuffer[LOG_BUF_SIZE+0x100]; // 256 bytes spare for overflow
uint32 gBleLogIndex = 0;
uint32 gBleLogWrapCount = 0;
uint32 gBleLogMask = BLE_LOG_MODULE_CTRL | BLE_LOG_MODULE_HOST | BLE_LOG_MODULE_APP /* | BLE_LOG_MODULE_OSAL_TASK*/ /* | BLE_LOG_MODULE_RF_CMD*/;
void bleLog_handleCyclicBuf(uint32_t len);

void bleLog_int_int(void *handle, uint32_t type, uint8_t *format, uint32_t param1, uint32_t param2)
{
  uint32_t len;

  if ((type & gBleLogMask) == 0)
    return;

  len = sprintf((char *)&gBleLogBuffer[gBleLogIndex], (const char *)format, param1, param2);
  bleLog_handleCyclicBuf(len);
}

void bleLog_int_str(void *handle, uint32_t type, uint8_t *format, uint32_t param1, char *param2)
{
  uint32_t len;

  if ((type & gBleLogMask) == 0)
    return;

  len = sprintf((char *)&gBleLogBuffer[gBleLogIndex], (const char *)format, param1, param2);
  bleLog_handleCyclicBuf(len);
}

void bleLog_int_time(void *handle, uint32_t type, uint8_t *start_str, uint32_t param1)
{
  uint32_t len;
  uint32_t timemsec = Clock_getTicks()/100;

  if ((type & gBleLogMask) == 0)
    return;

  len = sprintf((char *)&gBleLogBuffer[gBleLogIndex], (const char *)"%s %d, ---- time[msec]=%d\n", start_str, param1, timemsec);
  bleLog_handleCyclicBuf(len);
}

void bleLog_handleCyclicBuf(uint32_t len)
{
  gBleLogIndex += len;
  sprintf((char *)&gBleLogBuffer[gBleLogIndex], "----- last\n");
  if (gBleLogIndex >= LOG_BUF_SIZE)
  {
    len = sprintf((char *)&gBleLogBuffer[0], "----- gBleLogWrapCount=%d\n", ++gBleLogWrapCount);
    gBleLogIndex = len;
  }
}

#endif //BLE_LOG

/*-------------------------------------------------------------------
 */
