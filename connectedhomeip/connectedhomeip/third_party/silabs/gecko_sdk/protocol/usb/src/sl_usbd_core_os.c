/***************************************************************************//**
 * @file
 * @brief USB Device Real-Time Kernel Layer
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cmsis_os2.h"

#include "em_core.h"

#include "sl_usbd_core.h"
#include "sl_usbd_core_config.h"

#include "sli_usbd_core.h"
#include "sli_usbd_driver.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

// parameters for task
static osThreadId_t         task_handle;
static const osThreadAttr_t task_attr = {
  .name       = "USBD Thread",
  .attr_bits  = 0,
  .stack_mem  = NULL,
  .stack_size = (uint32_t)SL_USBD_TASK_STACK_SIZE,
  .cb_mem     = NULL,
  .cb_size    = 0,
  .priority   = (osPriority_t)SL_USBD_TASK_PRIORITY
};

// parameters for message queue
static osMessageQueueId_t         message_queue_handle;
static const osMessageQueueAttr_t message_queue_attr = {
  .name       = "USBD Queue",
  .attr_bits  = 0,
  .cb_mem     = NULL,
  .cb_size    = 0,
  .mq_mem     = NULL,
  .mq_size    = 0
};

// parameters for event flags
static osEventFlagsId_t     eventflags_handle[SL_USBD_OPEN_ENDPOINTS_QUANTITY];
static osEventFlagsAttr_t   eventflags_attr[SL_USBD_OPEN_ENDPOINTS_QUANTITY];

#define EVENT_FLAG_COMPLETE   0x1       // signal posted
#define EVENT_FLAG_ABORT      0x2       // signal aborted

// parameters for mutex
static osMutexId_t          mutex_handle[SL_USBD_OPEN_ENDPOINTS_QUANTITY];
static osMutexAttr_t        mutex_attr[SL_USBD_OPEN_ENDPOINTS_QUANTITY];

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void usbd_core_os_task_main(void *p_arg);

static uint32_t usbd_core_os_ms_to_ticks(uint32_t milliseconds);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/*******************************************************************************************************
*  Create task and queue for task to pend on
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_create_task(void)
{
  message_queue_handle = osMessageQueueNew(SL_USBD_CORE_EVENT_BUS_NBR,
                                           sizeof(sli_usbd_core_event_t),
                                           &message_queue_attr);

  if (message_queue_handle == NULL) {
    return SL_STATUS_FAIL;
  }

  task_handle = osThreadNew(usbd_core_os_task_main,
                            NULL,
                            &task_attr);

  if (task_handle == NULL) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************************************
*  Create an OS signal
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_create_endpoint_signal(uint8_t endpoint)
{
  if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY) {
    return SL_STATUS_FAIL;
  }

  eventflags_attr[endpoint].name       = "USBD events";
  eventflags_attr[endpoint].attr_bits  = 0;
  eventflags_attr[endpoint].cb_mem     = NULL;
  eventflags_attr[endpoint].cb_size    = 0;

  eventflags_handle[endpoint] = osEventFlagsNew(&eventflags_attr[endpoint]);

  if (eventflags_handle[endpoint] == NULL) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************************************
*  Delete an OS signal
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_delete_endpoint_signal(uint8_t endpoint)
{
  if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY) {
    return SL_STATUS_FAIL;
  }

  if (osEventFlagsDelete(eventflags_handle[endpoint]) != osOK) {
    return SL_STATUS_FAIL;
  }

  eventflags_handle[endpoint] = NULL;

  return SL_STATUS_OK;
}

/*******************************************************************************************************
*  Wait for a signal to become available
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_pend_endpoint_signal(uint8_t  endpoint,
                                                  uint16_t timeout_ms)
{
  uint32_t ticks;
  uint32_t status;

  if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY) {
    return SL_STATUS_FAIL;
  }

  if (timeout_ms == 0) {
    ticks = osWaitForever;
  } else {
    ticks = usbd_core_os_ms_to_ticks(timeout_ms);
  }

  status = osEventFlagsWait(eventflags_handle[endpoint], EVENT_FLAG_COMPLETE | EVENT_FLAG_ABORT, osFlagsWaitAny, ticks);

  if (status == osFlagsErrorTimeout) {
    return SL_STATUS_TIMEOUT;
  }

  if (status & osFlagsError) {
    return SL_STATUS_FAIL;
  }

  if ((status & EVENT_FLAG_ABORT) == EVENT_FLAG_ABORT) {
    return SL_STATUS_ABORT;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************************************
*  Abort any wait operation on signal
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_abort_endpoint_signal(uint8_t endpoint)
{
  if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY) {
    return SL_STATUS_FAIL;
  }

  if (osEventFlagsSet(eventflags_handle[endpoint], EVENT_FLAG_ABORT) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************************************
*  Make a signal available
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_post_endpoint_signal(uint8_t endpoint)
{
  if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY) {
    return SL_STATUS_FAIL;
  }

  if (osEventFlagsSet(eventflags_handle[endpoint], EVENT_FLAG_COMPLETE) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************************************
*  Create an OS resource to use as an endpoint lock
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_create_endpoint_lock(uint8_t endpoint)
{
  if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY) {
    return SL_STATUS_FAIL;
  }

  mutex_attr[endpoint].name       = "USBD mutex";
  mutex_attr[endpoint].attr_bits  = 0;
  mutex_attr[endpoint].cb_mem     = NULL;
  mutex_attr[endpoint].cb_size    = 0;

  mutex_handle[endpoint] = osMutexNew(&mutex_attr[endpoint]);

  if (mutex_handle[endpoint] == NULL) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************************************
*  Delete the OS resource used as an endpoint lock
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_delete_endpoint_lock(uint8_t endpoint)
{
  if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY) {
    return SL_STATUS_FAIL;
  }

  if (osMutexDelete(mutex_handle[endpoint]) != osOK) {
    return SL_STATUS_FAIL;
  }

  mutex_handle[endpoint] = NULL;

  return SL_STATUS_OK;
}

/*******************************************************************************************************
*  Wait for an endpoint to become available and acquire its lock
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_acquire_endpoint_lock(uint8_t endpoint)
{
  if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY) {
    return SL_STATUS_FAIL;
  }

  if (osMutexAcquire(mutex_handle[endpoint], osWaitForever) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************************************
*  Release an endpoint lock
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_release_endpoint_lock(uint8_t endpoint)
{
  if (endpoint >= SL_USBD_OPEN_ENDPOINTS_QUANTITY) {
    return SL_STATUS_FAIL;
  }

  if (osMutexRelease(mutex_handle[endpoint]) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************************************
*   Wait until a core event is ready
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_get_core_event(void *p_event)
{
  uint8_t prio;

  if (osMessageQueueGet(message_queue_handle, p_event, &prio, osWaitForever) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/*******************************************************************************************************
*   Queues core event
*******************************************************************************************************/
sl_status_t sli_usbd_core_os_put_core_event(void *p_event)
{
  if (osMessageQueuePut(message_queue_handle, p_event, 0, 0) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           usbd_core_os_task_main()
 *
 * @brief    OS-dependent shell task to process USB core events.
 *
 * @param    p_arg   Pointer to task initialization argument
 *
 *******************************************************************************************************/
static void usbd_core_os_task_main(void *p_arg)
{
  (void)p_arg;

  while (true) {
    sli_usbd_core_task_handler();
  }
}

/****************************************************************************************************//**
 *                                           usbd_core_os_ms_to_ticks()
 *
 * @brief    Converts milliseconds to kernel ticks
 *
 * @param    milliseconds   milliseconds to convert to ticks
 *
 * @return   Number of ticks
 *******************************************************************************************************/
static uint32_t usbd_core_os_ms_to_ticks(uint32_t milliseconds)
{
  uint32_t freq;
  uint32_t ticks;

  freq = osKernelGetTickFreq();

  ticks = (((milliseconds * freq) + 1000u - 1u) / 1000u);

  return ticks;
}
