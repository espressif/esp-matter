/******************************************************************************

 @file  port.c

 @brief This API allows the software components in the uStack to be written
        independently of the specifics of the operating system, kernel or
        tasking environment (including control loops or connect-to-interrupt
        systems).

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

/*********************************************************************
 * INCLUDES
 */

#include <string.h>
#include <stdlib.h>

#if defined(FEATURE_POSIX)
#include "hal_board.h"
#endif /* FEATURE_POSIX */

#if defined(FEATURE_POSIX)
/* Note the POSIX module must be added in the corresponding "app_ble.cfg".
 */
#warning "var Settings = xdc.useModule('ti.sysbios.posix.Settings'); \
must be added to the corresponding "xxx_csdk.cfg"
#include <ti/sysbios/posix/pthread.h>
#include <ti/sysbios/posix/time.h>
#include <ti/sysbios/posix/mqueue.h>
#elif defined(FEATURE_NORTOS)
#error "FEATURE_NORTOS implementation must be added by customer."
#else /* Default to TIRTOS */
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Queue.h>
#endif

/* RTOS Port */
#include "port.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */
typedef struct port_timerObject_s
{
#if defined(FEATURE_POSIX)
  sigevent     sev;
  timer_t      timer;
#elif defined(FEATURE_NORTOS)
  #error "FEATURE_NORTOS implementation must be added by customer."
#else /* Default to TIRTOS */
  Clock_Handle clockHandle;
  Clock_Struct clockStruct;
#endif
} port_timerObject_t;

typedef struct port_queueObject_s
{
#if defined(FEATURE_POSIX)
  mqd_t          mqDescriptor;
  struct mq_attr mqAttrbute;
#elif defined(FEATURE_NORTOS)
  #error "FEATURE_NORTOS implementation must be added by customer."
#else /* Default to TIRTOS */
  /* Queue object used for internal messages */
  Queue_Handle queueHandle;
  Queue_Struct queueStruct;
#endif
} port_queueObject_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * TIMERS
 *********************************************************************/

/**
 * @fn port_timerCreate
 *
 * @brief This function creates a timer instance and return the handle of this
 *        timer instance.
 *
 * @param port_TimerCB - timer callback called upon timer expiration.
 * @param port_TimerEvent - timer callback parameter.
 *
 * @return pointer to port_timerObject_s
 */
struct port_timerObject_s *port_timerCreate( port_timerCB_t port_TimerCB,
                                             uint16_t port_TimerEvent )
{
  port_timerObject_t *handle;

  handle = malloc(sizeof(port_timerObject_t));
#if defined(FEATURE_POSIX)
  if (handle != NULL)
  {
    handle->sev.sigev_notify = SIGEV_SIGNAL;
    handle->sev.sigev_value.sival_int = (int16_t)port_TimerEvent;
    handle->sev.sigev_notify_function = (void (*)(sigval))port_TimerCB;
    handle->sev.sigev_notify_attributes = NULL;

    (void) timer_create(CLOCK_MONOTONIC,
                        &handle->sev,
                        &handle->timer);
  }
#elif defined(FEATURE_NORTOS)
  #error "FEATURE_NORTOS implementation must be added by customer."
#else /* Default to TIRTOS */
  if (handle != NULL)
  {
    Clock_Params clockParams;

    /* Setup BcastDuty timer as a one-shot timer */
    Clock_Params_init(&clockParams);
    clockParams.arg = port_TimerEvent;
    clockParams.period = 0;
    clockParams.startFlag = false;
    Clock_construct(&handle->clockStruct,
                    port_TimerCB,
                    0,
                    &clockParams);
    handle->clockHandle = Clock_handle(&handle->clockStruct);
  }
#endif
  return handle;
}

/**
 * @fn port_timerStart
 *
 * @brief This function starts a timer previously created.
 *
 * @param handle - pointer to the timer object previously created.
 * @param timeout - timeout in SYSTICK previously created.
 *
 * @return none
 */
void port_timerStart( struct port_timerObject_s *handle,
                      uint32_t timeout )
{
#if defined(FEATURE_POSIX)
  struct itimerspec its;

  /* Assume the SYSTICK is 10us */
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 0;
  its.it_value.tv_sec = timeout/100000;
  its.it_value.tv_nsec = (timeout%100000)*10000;
  (void) timer_settime(handle->timer, 0, &its, NULL);
#elif defined(FEATURE_NORTOS)
  /* FEATURE_NORTOS implementation must be added by customer. */
#else /* Default to TIRTOS */
  Clock_setTimeout(handle->clockHandle, timeout);
  Clock_start(handle->clockHandle);
#endif
}

/**
 * @fn port_timerStop
 *
 * @brief This function stops a timer previously created.
 *
 * @param handle - pointer to the timer instance previously created.
 *
 * @return none
 */
void port_timerStop( struct port_timerObject_s *handle )
{
#if defined(FEATURE_POSIX)
  struct itimerspec its;

  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 0;
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = 0;
  (void) timer_settime(handle->timer, 0, &its, NULL);
#elif defined(FEATURE_NORTOS)
  /* FEATURE_NORTOS implementation must be added by customer. */
#else /* Default to TIRTOS */
  if (Clock_isActive(handle->clockHandle))
  {
    /* stop the timer */
    Clock_stop(handle->clockHandle);
  }
#endif
}

/*********************************************************************
 * QUEUES
 *********************************************************************/

/**
 * @fn port_queueCreate
 *
 * @brief This function creates a queue instance and return the handle of this
 *        queue instance.
 *
 * @param portQueueName - null terminated string. Used in POSIX only.
 *        Ignored in TIRTOS.
 *
 * @return pointer to port_queueObject_s.
 */
struct port_queueObject_s *port_queueCreate( const char *portQueueName )
{
  port_queueObject_t *handle;

  handle = malloc(sizeof(port_queueObject_t));
#if defined(FEATURE_POSIX)
  if (handle != NULL)
  {
    handle->mqAttrbute.mq_msgsize = MQ_DEF_MSGSIZE;
    handle->mqAttrbute.mq_maxmsg = MQ_DEF_MAXMSG;
    handle->mqDescriptor = mq_open(portQueueName,
                                   O_RDWR | O_CREAT,
                                   0600,
                                   &handle->mqAttrbute);
  }
#elif defined(FEATURE_NORTOS)
  #error "FEATURE_NORTOS implementation must be added by customer."
#else /* Default to TIRTOS */
  (void) portQueueName;
  if (handle != NULL)
  {
    Queue_construct(&handle->queueStruct, NULL);
    handle->queueHandle = Queue_handle(&handle->queueStruct);
  }
#endif

  return handle;
}

/**
 * @fn port_queueGet
 *
 * @brief This function removes the element from the front of queue.
 *
 * @param handle - pointer to the queue instance previously created.
 * @param port_ppQueueElement - address of pointer to the retrieved queue element.
 *
 * @return - none.
 */
void port_queueGet( struct port_queueObject_s *handle,
                    port_queueElem_t **port_ppQueueElement )
{
#if defined(FEATURE_POSIX)
  uint32_t msg_prio;
  uint8_t  msg[MQ_DEF_MSGSIZE];
  (void) mq_receive(handle->mqDescriptor,
                    (char *)msg,
                    handle->mqAttrbute.mq_msgsize,
                    &msg_prio);
  *port_ppQueueElement = ((port_queueElem_t *)&msg[0])->posix_queueElem.pElem;
  memcpy(*port_ppQueueElement, msg, ((port_queueElem_t *)&msg[0])->posix_queueElem.size);
#elif defined(FEATURE_NORTOS)
  /* FEATURE_NORTOS implementation must be added by customer. */
#else /* Default to TIRTS */
  *port_ppQueueElement = Queue_get(handle->queueHandle);
#endif
}

/**
 * @fn port_queuePut
 *
 * @brief This function puts the queue element at the front of queue.
 *
 * @param handle - pointer to the queue instance previously created.
 * @param port_queueElement - pointer to the queue element.
 * @param size - the size of this element. This is ignored in TIRTOS.
 *
 * @return none
 */
void port_queuePut( struct port_queueObject_s *handle,
                    port_queueElem_t *port_queueElement,
                    uint16_t size )
{
#if defined(FEATURE_POSIX)
  port_queueElement->posix_queueElem.pElem = port_queueElement;
  port_queueElement->posix_queueElem.size  = size;

  (void) mq_send(handle->mqDescriptor,
                 (const char *)port_queueElement,
                 handle->mqAttrbute.mq_msgsize,
                 0);
#elif defined(FEATURE_NORTOS)
  /* FEATURE_NORTOS implementation must be added by customer. */
#else /* Default to TIRTS */
  (void)size;
  Queue_put(handle->queueHandle, (Queue_Elem *)port_queueElement);
#endif
}

/**
 * @fn port_queueEmpty
 *
 * @brief This function checks if the queue is empty.
 *
 * @param handle - pointer to the queue instance previously created.
 *
 * @return true if queue is empty.
 */
bool port_queueEmpty( struct port_queueObject_s *handle )
{
#if defined(FEATURE_POSIX)
  (void) mq_getattr(handle->mqDescriptor, &handle->mqAttrbute);
  return ((handle->mqAttrbute.mq_curmsgs == 0) ? true : false);
#elif defined(FEATURE_NORTOS)
  #error "FEATURE_NORTOS implementation must be added by customer."
#else /* Default to TIRTOS */
  return(Queue_empty(handle->queueHandle));
#endif
}

/**
 * @fn port_enterCS_HW
 *
 * @brief This function enters the critical section by disabling HWI.
 *
 * @return current key to be saved
 */
port_key_t port_enterCS_HW( void )
{
#if defined(FEATURE_POSIX)
  return (!IntMasterDisable());
#elif defined(FEATURE_NORTOS)
  #error "FEATURE_NORTOS implementation must be added by customer."
#else /* Default to TIRTOS */
  return (Hwi_disable());
#endif
}

/**
 * @fn port_exitCS_HW
 *
 * @brief This function exits the critical section by restoring HWI.
 *
 * @param key - restore the current status.
 *
 * @return - None.
 */
void port_exitCS_HW( port_key_t key )
{
#if defined(FEATURE_POSIX)
  if (key)
  {
    (void) IntMasterEnable();
  }
#elif defined(FEATURE_NORTOS)
  #error "FEATURE_NORTOS implementation must be added by customer."
#else /* Default to TIRTOS */
  Hwi_restore(key);
#endif
}

/**
 * @fn port_enterCS_SW
 *
 * @brief This function enters the critical section by disabling SWI.
 *        Note that this function will disable master interrupt in POSIX.
 *
 * @param None
 *
 * @return current key to be saved
 */
port_key_t port_enterCS_SW( void )
{
#if defined(FEATURE_POSIX)
  return (!IntMasterDisable());
#elif defined(FEATURE_NORTOS)
  #error "FEATURE_NORTOS implementation must be added by customer."
#else /* Default to TIRTOS */
  return (Swi_disable());
#endif
}

/**
 * @fn port_exitCS_SW
 *
 * @brief This function exits the critical section by restoring SWI.
 *        Note that this function will restore master interrupt in POSIX.
 *
 * @param key - restore the current status.
 *
 * @return - None.
 */
void port_exitCS_SW( port_key_t key )
{
#if defined(FEATURE_POSIX)
  if (key)
  {
    (void) IntMasterEnable();
  }
#elif defined(FEATURE_NORTOS)
  #error "FEATURE_NORTOS implementation must be added by customer."
#else /* Default to TIRTOS */
  Swi_restore(key);
#endif
}

/*********************************************************************
 * OTHERS
 *********************************************************************/

/*********************************************************************
 */
