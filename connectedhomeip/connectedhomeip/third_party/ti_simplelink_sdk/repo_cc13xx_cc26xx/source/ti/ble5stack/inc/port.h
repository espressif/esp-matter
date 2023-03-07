/******************************************************************************

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
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
 *  @addtogroup Micro_BLE_Stack
 *  @{
 *  @defgroup PORT RTOS Interface
 *  @{
 *  @file  port.h
 *  @brief  Micro BLE RTOS Interface API
 *
 *  This file contains interfacing between Micro BLE Stack and the RTOS.
 */

#ifndef PORT_H
#define PORT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdbool.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/** @defgroup PORT_Constants RTOS Interface Constants
 * @{
 */

/* For POSIX only:
 * The message size is tailored to ubleEvtMsg_t plus length in uble.h
 */
#define MQ_DEF_MSGSIZE    16  //!< Maximum POSIX message size
#define MQ_DEF_MAXMSG     12  //!< Maximum number of messages in POSIX

/** @} End PORT_Constants */

/*********************************************************************
 * TYPEDEFS
 */

/** @defgroup Port_Timers RTOS Timer Interfaces
 * @{
 */

/** @defgroup Port_Timer_Callback Port RTOS Timer Callback
 * @{
 */

/**
 * @brief Function to process timer expiration.
 */
typedef void (*port_timerCB_t)(uint32_t param);

/** @} End Port_Timer_Callback */

/// @brief Opaque strut for timer object
struct port_timerObject_s;

/** @} End Port_Timers */

/** @defgroup Port_Queues RTOS Queue Interfaces
 * @{
 */

/// @brief Port RTOS queue element for TIRTOS
typedef struct port_queueElem_tirtos_s
{
  void *next;  //!< next pointer
  void *prev;  //!< previous pointer
} port_queueElem_tirtos_t;

/// @brief Port RTOS queue element for POSIX
typedef struct port_queueElem_posix_s
{
  void *pElem;  //!< pointer to the queue element
  uint16_t size;  //!< size of the queue element
} port_queueElem_posix_t;

/// @brief Union of port RTOS queue element for TIRTOS/POSIX
typedef union port_queueElem_s
{
  port_queueElem_tirtos_t tirtos_queueElem;  //!< TIRTOS queue element
  port_queueElem_posix_t posix_queueElem; //!< POISX queue element
} port_queueElem_t;

/// @brief Opaque strut for queue object
struct port_queueObject_s;

/** @} End Port_Queues */

/** @defgroup Port_Key RTOS Critical Section Key
 * @{
 */

/** @brief RTOS Critical Section Key type */
typedef uint32_t port_key_t;

/** @} End Port_Key */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/**
 * @brief This function creates a timer instance and return the handle of this
 *        timer instance.
 *
 * @param port_TimerCB - timer callback called upon timer expiration.
 * @param port_TimerEvent - timer callback parameter.
 *
 * @return pointer to port_timerObject_s
 */
extern struct port_timerObject_s *port_timerCreate( port_timerCB_t port_TimerCB,
                                                    uint16_t port_TimerEvent );

/**
 * @brief This function starts a timer previously created.
 *
 * @param handle - pointer to the timer object previously created.
 * @param timeout - timeout in SYSTICK previously created.
 *
 * @return none
 */
extern void port_timerStart( struct port_timerObject_s *handle,
                             uint32_t timeout );

/**
 * @brief This function stops a timer previously created.
 *
 * @param handle - pointer to the timer instance previously created.
 *
 * @return none
 */
extern void port_timerStop( struct port_timerObject_s *handle );

/**
 * @brief This function creates a queue instance and return the handle of this
 *        queue instance.
 *
 * @param portQueueName - null terminated string. Used in POSIX only.
 *        Ignored in TIRTOS.
 *
 * @return pointer to port_queueObject_s.
 */
extern struct port_queueObject_s *port_queueCreate( const char *portQueueName );

/**
 * @brief This function removes the element from the front of queue.
 *
 * @param handle - pointer to the queue instance previously created.
 * @param port_ppQueueElement - address of pointer to the retrieved queue element.
 *
 * @return - none.
 */
extern void port_queueGet( struct port_queueObject_s *handle,
                           port_queueElem_t **port_ppQueueElement );

/**
 * @brief This function puts the queue element at the front of queue.
 *
 * @param handle - pointer to the queue instance previously created.
 * @param port_queueElement - pointer to the queue element.
 * @param size - the size of this element. This is ignored in TIRTOS.
 *
 * @return none
 */
extern void port_queuePut( struct port_queueObject_s *handle,
                           port_queueElem_t *port_queueElement,
                           uint16_t size );

/**
 * @brief This function checks if the queue is empty.
 *
 * @param handle - pointer to the queue instance previously created.
 *
 * @return true if queue is empty.
 */
bool port_queueEmpty( struct port_queueObject_s *handle );

/**
 * @brief This function enters the critical section by disabling HWI.
 *
 * @return current key to be saved
 */
extern port_key_t port_enterCS_HW( void );

/**
 * @brief This function exits the critical section by restoring HWI.
 *
 * @param key - restore the current status.
 *
 * @return - None.
 */
extern void port_exitCS_HW( port_key_t key );

/**
 * @brief This function enters the critical section by disabling SWI.
 *        Note that this function will disable master interrupt in POSIX.
 *
 * @return current key to be saved
 */
extern port_key_t port_enterCS_SW( void );

/**
 * @brief This function exits the critical section by restoring SWI.
 *        Note that this function will restore master interrupt in POSIX.
 *
 * @param key - restore the current status.
 *
 * @return - None.
 */
extern void port_exitCS_SW( port_key_t key );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* PORT_H */

/** @} End PORT */

/** @} End Rtos_Port */
