/***************************************************************************//**
 * @file
 * @brief USB Device - USB MSC Class Real-Time Kernel Layer
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

#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cmsis_os2.h"

#include "sl_bit.h"
#include "sl_enum.h"
#include "sl_status.h"

#include "sl_usbd_core.h"
#include "sl_usbd_class_msc.h"
#include "sli_usbd_class_msc.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

// parameters for task
static osThreadId_t         task_handle[SL_USBD_MSC_CLASS_INSTANCE_QUANTITY];
static osThreadAttr_t       task_attr[SL_USBD_MSC_CLASS_INSTANCE_QUANTITY];

static osSemaphoreId_t      comm_signal_handle[SL_USBD_MSC_CLASS_INSTANCE_QUANTITY];
static osSemaphoreAttr_t    comm_signal_attr[SL_USBD_MSC_CLASS_INSTANCE_QUANTITY];

static osSemaphoreId_t      connect_signal_handle[SL_USBD_MSC_CLASS_INSTANCE_QUANTITY];
static osSemaphoreAttr_t    connect_signal_attr[SL_USBD_MSC_CLASS_INSTANCE_QUANTITY];

static osMutexId_t          lun_lock_handle[SL_USBD_MSC_CLASS_INSTANCE_QUANTITY * SL_USBD_MSC_LUN_QUANTITY];
static osMutexAttr_t        lun_lock_attr[SL_USBD_MSC_CLASS_INSTANCE_QUANTITY * SL_USBD_MSC_LUN_QUANTITY];

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void usbd_msc_os_task(void *p_arg);

static uint32_t usbd_msc_os_ms_to_ticks(uint32_t milliseconds);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 * Initialize MSC OS interface
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_init(void)
{
  uint32_t  class_nbr;
  uint32_t  lu_ix;

  for (class_nbr = 0u; class_nbr < SL_USBD_MSC_CLASS_INSTANCE_QUANTITY; class_nbr++) {
    comm_signal_attr[class_nbr].name      = "MSC comms signal";
    comm_signal_attr[class_nbr].attr_bits = 0;
    comm_signal_attr[class_nbr].cb_mem    = NULL;
    comm_signal_attr[class_nbr].cb_size   = 0;

    comm_signal_handle[class_nbr] = osSemaphoreNew(UINT32_MAX, 0, &comm_signal_attr[class_nbr]);

    if (comm_signal_handle[class_nbr] == NULL) {
      return SL_STATUS_FAIL;
    }

    connect_signal_attr[class_nbr].name       = "MSC connect signal";
    connect_signal_attr[class_nbr].attr_bits  = 0;
    connect_signal_attr[class_nbr].cb_mem     = NULL;
    connect_signal_attr[class_nbr].cb_size    = 0;

    connect_signal_handle[class_nbr] = osSemaphoreNew(UINT32_MAX, 0, &connect_signal_attr[class_nbr]);

    if (connect_signal_handle[class_nbr] == NULL) {
      return SL_STATUS_FAIL;
    }
  }

  for (lu_ix = 0u; lu_ix < (SL_USBD_MSC_CLASS_INSTANCE_QUANTITY * SL_USBD_MSC_LUN_QUANTITY); lu_ix++) {
    // Create lock for logical unit
    lun_lock_attr[lu_ix].name       = "MSC lun lock";
    lun_lock_attr[lu_ix].attr_bits  = 0;
    lun_lock_attr[lu_ix].cb_mem     = NULL;
    lun_lock_attr[lu_ix].cb_size    = 0;

    lun_lock_handle[lu_ix] = osMutexNew(&lun_lock_attr[lu_ix]);

    if (lun_lock_handle[lu_ix] == NULL) {
      return SL_STATUS_FAIL;
    }
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Create task for MSC class instance
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_create_task(uint8_t       class_nbr,
                                        uint32_t      msc_task_stack_size,
                                        osPriority_t  msc_task_priority)
{
  task_attr[class_nbr].name       = "MSC thread";
  task_attr[class_nbr].attr_bits  = 0;
  task_attr[class_nbr].stack_mem  = NULL;
  task_attr[class_nbr].stack_size = msc_task_stack_size;
  task_attr[class_nbr].cb_mem     = NULL;
  task_attr[class_nbr].cb_size    = 0;
  task_attr[class_nbr].priority   = msc_task_priority;

  task_handle[class_nbr] = osThreadNew(usbd_msc_os_task,
                                       (void *)(uint32_t)class_nbr,
                                       &task_attr[class_nbr]);

  if (task_handle[class_nbr] == NULL) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Post a semaphore used for MSC communication
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_post_comm_signal(uint8_t class_nbr)
{
  osStatus_t status;

  status = osSemaphoreRelease(comm_signal_handle[class_nbr]);

  if (status != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Wait on a semaphore to become available for MSC communication
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_pend_comm_signal(uint8_t  class_nbr,
                                             uint32_t timeout_ms)
{
  uint32_t ticks;
  osStatus_t status;

  if (timeout_ms == 0) {
    ticks = osWaitForever;
  } else {
    ticks = usbd_msc_os_ms_to_ticks(timeout_ms);
  }

  status = osSemaphoreAcquire(comm_signal_handle[class_nbr], ticks);

  if (status == osErrorTimeout) {
    return SL_STATUS_TIMEOUT;
  }

  if (status != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Post a semaphore used for MSC enumeration
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_post_connect_signal(uint8_t class_nbr)
{
  osStatus_t status;

  status = osSemaphoreRelease(connect_signal_handle[class_nbr]);

  if (status != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Wait on a semaphore to become available for MSC enumeration
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_pend_connect_signal(uint8_t  class_nbr,
                                                uint32_t timeout_ms)
{
  uint32_t ticks;
  osStatus_t status;

  if (timeout_ms == 0) {
    ticks = osWaitForever;
  } else {
    ticks = usbd_msc_os_ms_to_ticks(timeout_ms);
  }

  status = osSemaphoreAcquire(connect_signal_handle[class_nbr], ticks);

  if (status == osErrorTimeout) {
    return SL_STATUS_TIMEOUT;
  }

  if (status != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Acquire logical unit's state lock
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_acquire_lun_lock(uint8_t lu_ix)
{
  osStatus_t status;

  status = osMutexAcquire(lun_lock_handle[lu_ix], osWaitForever);

  if (status != osOK) {
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/****************************************************************************************************//**
 * Release logical unit's state lock
 *******************************************************************************************************/
sl_status_t sli_usbd_msc_os_release_lun_lock(uint8_t lu_ix)
{
  osStatus_t status;

  status = osMutexRelease(lun_lock_handle[lu_ix]);

  if (status != osOK) {
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
 *                                    usbd_msc_os_task()
 *
 * @brief    OS-dependent shell task to process MSC task
 *
 * @param    p_arg   Pointer to task initialization argument.
 *******************************************************************************************************/
static void usbd_msc_os_task(void *p_arg)
{
  uint8_t class_nbr = (uint8_t)(uint32_t)p_arg;

  sli_usbd_msc_task_handler(class_nbr);
}

/*******************************************************************************************************
*  Converts milliseconds to kernel ticks.
*******************************************************************************************************/
static uint32_t usbd_msc_os_ms_to_ticks(uint32_t milliseconds)
{
  uint32_t freq;
  uint32_t ticks;

  freq = osKernelGetTickFreq();

  ticks = (((milliseconds * freq) + 1000u - 1u) / 1000u);

  return ticks;
}
