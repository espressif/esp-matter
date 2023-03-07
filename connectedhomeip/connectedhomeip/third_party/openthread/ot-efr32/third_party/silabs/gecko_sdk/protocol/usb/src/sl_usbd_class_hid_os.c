/***************************************************************************//**
 * @file
 * @brief USB Device - USB Hid Class Real-Time Kernel Layer
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_usbd_core.h"
#include "sl_usbd_class_hid.h"
#include "sli_usbd_class_hid.h"
#include "sli_usbd_class_hid_os.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

// parameters for task
static osThreadId_t         timer_task_handle;
static const osThreadAttr_t timer_task_attr = {
  .name       = "HID Timer Thread",
  .attr_bits  = 0,
  .stack_mem  = NULL,
  .stack_size = (uint32_t)SL_USBD_HID_TIMER_TASK_STACK_SIZE,
  .cb_mem     = NULL,
  .cb_size    = 0,
  .priority   = (osPriority_t)SL_USBD_HID_TIMER_TASK_PRIORITY
};

// parameters for event flags
static osEventFlagsId_t     input_eventflags_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];
static osEventFlagsAttr_t   input_eventflags_attr[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

static osEventFlagsId_t     output_eventflags_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];
static osEventFlagsAttr_t   output_eventflags_attr[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

#define EVENT_FLAG_COMPLETE   0x1       // signal posted
#define EVENT_FLAG_ABORT      0x2       // signal aborted

// parameters for mutexes
static osMutexId_t          input_mutex_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];
static osMutexAttr_t        input_mutex_attr[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

static osMutexId_t          output_mutex_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];
static osMutexAttr_t        output_mutex_attr[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

static osMutexId_t          tx_mutex_handle[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];
static osMutexAttr_t        tx_mutex_attr[SL_USBD_HID_CLASS_INSTANCE_QUANTITY];

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void usbd_hid_os_timer_task(void *p_arg);

static uint32_t usbd_hid_os_ms_to_ticks(uint32_t milliseconds);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Initialize HID OS interface
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_init(void)
{
  uint32_t       class_nbr;

  for (class_nbr = 0u; class_nbr < SL_USBD_HID_CLASS_INSTANCE_QUANTITY; class_nbr++) {
    tx_mutex_attr[class_nbr].name = "HID Tx mutex";
    tx_mutex_attr[class_nbr].attr_bits = 0;
    tx_mutex_attr[class_nbr].cb_mem = NULL;
    tx_mutex_attr[class_nbr].cb_size = 0;

    tx_mutex_handle[class_nbr] = osMutexNew(&tx_mutex_attr[class_nbr]);

    if (tx_mutex_handle[class_nbr] == NULL) {
      return SL_STATUS_FAIL;
    }

    output_mutex_attr[class_nbr].name = "HID Output mutex";
    output_mutex_attr[class_nbr].attr_bits = 0;
    output_mutex_attr[class_nbr].cb_mem = NULL;
    output_mutex_attr[class_nbr].cb_size = 0;

    output_mutex_handle[class_nbr] = osMutexNew(&output_mutex_attr[class_nbr]);

    if (output_mutex_handle[class_nbr] == NULL) {
      return SL_STATUS_FAIL;
    }

    input_mutex_attr[class_nbr].name = "HID Input mutex";
    input_mutex_attr[class_nbr].attr_bits = 0;
    input_mutex_attr[class_nbr].cb_mem = NULL;
    input_mutex_attr[class_nbr].cb_size = 0;

    input_mutex_handle[class_nbr] = osMutexNew(&input_mutex_attr[class_nbr]);

    if (input_mutex_handle[class_nbr] == NULL) {
      return SL_STATUS_FAIL;
    }

    input_eventflags_attr[class_nbr].name       = "HID input events";
    input_eventflags_attr[class_nbr].attr_bits  = 0;
    input_eventflags_attr[class_nbr].cb_mem     = NULL;
    input_eventflags_attr[class_nbr].cb_size    = 0;

    input_eventflags_handle[class_nbr] = osEventFlagsNew(&input_eventflags_attr[class_nbr]);

    if (input_eventflags_handle[class_nbr] == NULL) {
      return SL_STATUS_FAIL;
    }

    output_eventflags_attr[class_nbr].name       = "HID output events";
    output_eventflags_attr[class_nbr].attr_bits  = 0;
    output_eventflags_attr[class_nbr].cb_mem     = NULL;
    output_eventflags_attr[class_nbr].cb_size    = 0;

    output_eventflags_handle[class_nbr] = osEventFlagsNew(&output_eventflags_attr[class_nbr]);

    if (input_eventflags_handle[class_nbr] == NULL) {
      return SL_STATUS_FAIL;
    }
  }

  timer_task_handle = osThreadNew(usbd_hid_os_timer_task,
                                  NULL,
                                  &timer_task_attr);

  if (timer_task_handle == NULL) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Lock class input report
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_lock_input(uint8_t class_nbr)
{
  if (osMutexAcquire(input_mutex_handle[class_nbr], osWaitForever) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Unlock class input report
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_unlock_input(uint8_t class_nbr)
{
  if (osMutexRelease(input_mutex_handle[class_nbr]) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Abort class output report
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_pend_abort_output(uint8_t class_nbr)
{
  if (osEventFlagsSet(output_eventflags_handle[class_nbr], EVENT_FLAG_ABORT) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Wait for output report data transfer to complete
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_pend_output(uint8_t  class_nbr,
                                        uint16_t timeout_ms)
{
  uint32_t ticks;
  uint32_t status;

  if (timeout_ms == 0) {
    ticks = osWaitForever;
  } else {
    ticks = usbd_hid_os_ms_to_ticks(timeout_ms);
  }

  status = osEventFlagsWait(output_eventflags_handle[class_nbr], EVENT_FLAG_COMPLETE | EVENT_FLAG_ABORT, osFlagsWaitAny, ticks);

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

/****************************************************************************************************//**
 * Signal that output report data is available
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_post_output(uint8_t class_nbr)
{
  if (osEventFlagsSet(output_eventflags_handle[class_nbr], EVENT_FLAG_COMPLETE) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Lock class output report
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_lock_output(uint8_t class_nbr)
{
  if (osMutexAcquire(output_mutex_handle[class_nbr], osWaitForever) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Unlock class output report
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_unlock_output(uint8_t class_nbr)
{
  if (osMutexRelease(output_mutex_handle[class_nbr]) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Lock class transmit
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_lock_tx(uint8_t class_nbr)
{
  if (osMutexAcquire(tx_mutex_handle[class_nbr], osWaitForever) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Unlock class transmit
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_unlock_tx(uint8_t class_nbr)
{
  if (osMutexRelease(tx_mutex_handle[class_nbr]) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Wait for input report data transfer to complete
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_pend_input(uint8_t  class_nbr,
                                       uint16_t timeout_ms)
{
  uint32_t ticks;
  uint32_t status;

  if (timeout_ms == 0) {
    ticks = osWaitForever;
  } else {
    ticks = usbd_hid_os_ms_to_ticks(timeout_ms);
  }

  status = osEventFlagsWait(input_eventflags_handle[class_nbr], EVENT_FLAG_COMPLETE | EVENT_FLAG_ABORT, osFlagsWaitAny, ticks);

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

/****************************************************************************************************//**
 * Abort any operation on input report
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_pend_abort_input(uint8_t class_nbr)
{
  if (osEventFlagsSet(input_eventflags_handle[class_nbr], EVENT_FLAG_ABORT) != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Signal that input report data transfer has completed
 *******************************************************************************************************/
sl_status_t sli_usbd_hid_os_post_input(uint8_t class_nbr)
{
  if (osEventFlagsSet(input_eventflags_handle[class_nbr], EVENT_FLAG_COMPLETE) != osOK) {
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
 *                                           usbd_hid_os_timer_task()
 *
 * @brief    OS-dependent shell task to process periodic HID input reports.
 *
 * @param    p_arg   Pointer to task initialization argument.
 *
 * @note     (1) Assumes tick rate frequency is greater than or equal to 250 Hz.  Otherwise, timer task
 *               scheduling rate will NOT be correct.
 *
 * @note     (2) Timer task MUST delay without failure.
 *               - (a) Failure to delay timer task will prevent some HID report task(s)/operation(s) from
 *                     functioning correctly. Thus, timer task is assumed to be successfully delayed
 *                     since NO error handling could be performed to counteract failure.
 *******************************************************************************************************/
static void usbd_hid_os_timer_task(void *p_arg)
{
  uint32_t dly_tick;
  uint32_t dly_until_tick;
  uint32_t previous_tick;
  (void)&p_arg;

  // Delay task at 4 ms rate (see Note #1).
  dly_tick = usbd_hid_os_ms_to_ticks(4);
  previous_tick = osKernelGetTickCount();

  while (true) {
    dly_until_tick = previous_tick + dly_tick;
    osDelayUntil(dly_until_tick);
    previous_tick = dly_until_tick;
    sli_usbd_hid_report_timer_task_handler();
  }
}

/*******************************************************************************************************
*  Converts milliseconds to kernel ticks.
*******************************************************************************************************/
static uint32_t usbd_hid_os_ms_to_ticks(uint32_t milliseconds)
{
  uint32_t freq;
  uint32_t ticks;

  freq = osKernelGetTickFreq();

  ticks = (((milliseconds * freq) + 1000u - 1u) / 1000u);

  return ticks;
}
